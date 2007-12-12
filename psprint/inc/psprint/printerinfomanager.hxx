/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: printerinfomanager.hxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: kz $ $Date: 2007-12-12 14:55:25 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _PSPRINT_PRINTERINFOMANAGER_HXX_
#define _PSPRINT_PRINTERINFOMANAGER_HXX_

#include <hash_map>
#ifndef __SGI_STL_LIST
#include <list>
#endif
#ifndef _PSPRINT_HELPER_HXX_
#include <psprint/helper.hxx>
#endif
#ifndef _PSPRINT_JOBDATA_HXX_
#include <psprint/jobdata.hxx>
#endif
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _PSPRINT_HELPER_HXX_
#include <psprint/helper.hxx>
#endif
#include <cstdio>

namespace psp
{

class SystemQueueInfo;

struct PrinterInfo : JobData
{
    // basename of PPD
    rtl::OUString             m_aDriverName;
    // can be the queue
    rtl::OUString             m_aLocation;
    // a user defined comment
    rtl::OUString             m_aComment;
    // a command line to pipe a PS-file to
    rtl::OUString             m_aCommand;
    // a command line to pipe a PS-file to in case of direct print
    rtl::OUString             m_aQuickCommand;
    // a list of special features separated by ',' not used by psprint
    // but assigned from the outside (currently for "fax","pdf=","autoqueue","external_dialog")
    rtl::OUString             m_aFeatures;
    // a mapping of fonts to other fonts.
    // this provides a method for the user
    // to replace arbitrary fonts by printer builtin fonts
    // currently this is only a mapping between font names
    // assuming that only adbobe standard encoding fonts are
    // built into the printer. in future it may be necessary
    // to map to a font name and UCS2 vector which should be mapped
    // this vector is currently implicitly given by the adobe
    // standard encoding
    bool                        m_bPerformFontSubstitution;
    std::hash_map< rtl::OUString, rtl::OUString, rtl::OUStringHash >
    m_aFontSubstitutes;
    std::hash_map< fontID, fontID >
    m_aFontSubstitutions;

    PrinterInfo() :
            JobData(),
            m_bPerformFontSubstitution( false )
    {}
};

class PrinterInfoManager
{
public:
    enum Type { Default = 0, CUPS = 1 };

    struct SystemPrintQueue
    {
        rtl::OUString       m_aQueue;
        rtl::OUString       m_aLocation;
        rtl::OUString       m_aComment;
    };
protected:
    // needed for checkPrintersChanged: files (not necessarily existant)
    // and their last known modification time
    struct WatchFile
    {
        // the file in question
        rtl::OUString         m_aFilePath;
        // the last know modification time or 0, if file did not exist
        TimeValue               m_aModified;
    };

    // internal data to describe a printer
    struct Printer
    {
        // configuration file containing this printer
        // empty means a freshly added printer that has to be saved yet
        rtl::OUString         m_aFile;
        // details other config files that have this printer
        // in case of removal all have to be removed
        std::list< rtl::OUString > m_aAlternateFiles;
        // group in m_aFile containing the printer
        // this must be unique over all configuration files
        // it usually should be the printer name
        rtl::OString          m_aGroup;
        // whether changes need to be saved
        bool                    m_bModified;
        // the corresponding info and job data
        PrinterInfo             m_aInfo;
    };

    std::hash_map< rtl::OUString, Printer, rtl::OUStringHash > m_aPrinters;
    PrinterInfo                         m_aGlobalDefaults;
    std::list< WatchFile >            m_aWatchFiles;
    rtl::OUString                     m_aDefaultPrinter;
    rtl::OUString                     m_aSystemPrintCommand;

    std::list< SystemPrintQueue >     m_aSystemPrintQueues;

    SystemQueueInfo*                  m_pQueueInfo;

    Type                              m_eType;
    bool                              m_bUseIncludeFeature;
    rtl::OUString                     m_aSystemDefaultPaper;

    PrinterInfoManager( Type eType = Default );
    virtual ~PrinterInfoManager();

    virtual void initialize();

    // fill in font substitutions
    // the resulting hash_map maps from source to target font ids
    void fillFontSubstitutions( PrinterInfo& rInfo ) const;

    // fill default paper if not configured in config file
    // default paper is e.g. locale dependent
    // if a paper is already set it will not be overwritten
    void setDefaultPaper( PPDContext& rInfo ) const;

    void initSystemDefaultPaper();
public:

    // there can only be one
    static PrinterInfoManager& get();

    // get PrinterInfoManager type
    Type getType() const { return m_eType; }

    // lists the names of all known printers
    void listPrinters( std::list< rtl::OUString >& rList ) const;

    // gets the number of known printers
    int countPrinters() const { return m_aPrinters.size(); }

    // gets info about a named printer
    const PrinterInfo& getPrinterInfo( const rtl::OUString& rPrinter ) const;

    // gets the name of the default printer
    const rtl::OUString& getDefaultPrinter() const { return m_aDefaultPrinter; }

    virtual void setupJobContextData( JobData& rData );

    // changes the info about a named printer
    virtual void changePrinterInfo( const rtl::OUString& rPrinter, const PrinterInfo& rNewInfo );

    // check if the printer configuration has changed
    // if bwait is true, then this method waits for eventual asynchronous
    // printer discovery to finish
    virtual bool checkPrintersChanged( bool bWait );

    // members for administration (->padmin)

    // add a named printer
    // addPrinter fails if a printer with the same name already exists
    // or the driver does not exist
    virtual bool addPrinter( const rtl::OUString& rPrinterName, const rtl::OUString& rDriverName );

    // remove a named printer
    // this fails if the config file belonging to this printer
    // is not writeable
    // if bCheckOnly is true, the printer is not really removed;
    // this is for checking if the removal would fail
    virtual bool removePrinter( const rtl::OUString& rPrinterName, bool bCheckOnly = false );

    // save the changes to all printers. this fails if there
    // is no writable config file at all
    virtual bool writePrinterConfig();

    // set a new default printer
    // fails if the specified printer does not exist
    virtual bool setDefaultPrinter( const rtl::OUString& rPrinterName );

    // primarily used internally but also by padmin
    // returns the printer queue names
    virtual const std::list< SystemPrintQueue >& getSystemPrintQueues();

    // similar but returnse whole commandlines
    virtual void getSystemPrintCommands( std::list< rtl::OUString >& rCommands );

    // abstract print command
    // returns a stdio FILE* that a postscript file may be written to
    // this may either be a regular file or the result of popen()
    virtual FILE* startSpool( const rtl::OUString& rPrinterName, bool bQuickCommand );
    // close the FILE* returned by startSpool and does the actual spooling
    // returns a numerical job id
    virtual int endSpool( const rtl::OUString& rPrinterName, const rtl::OUString& rJobTitle, FILE* pFile, const JobData& rDocumentJobData );

    // for spadmin: whether adding or removing a printer is possible
    virtual bool addOrRemovePossible() const;

    bool getUseIncludeFeature() const { return m_bUseIncludeFeature; }

    // check whether a printer's feature string contains a subfeature
    bool checkFeatureToken( const rtl::OUString& rPrinterName, const char* pToken ) const;
};

} // namespace

#endif // _PSPRINT_PRINTERINFOMANAGER_HXX_
