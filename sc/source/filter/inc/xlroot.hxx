/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xlroot.hxx,v $
 *
 *  $Revision: 1.26 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-10 14:04:29 $
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

#ifndef SC_XLROOT_HXX
#define SC_XLROOT_HXX

#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif
#ifndef _SOT_STORAGE_HXX
#include <sot/storage.hxx>
#endif
#ifndef SC_XLCONST_HXX
#include "xlconst.hxx"
#endif
#ifndef SC_XLTOOLS_HXX
#include "xltools.hxx"
#endif

// Forward declarations of objects in public use ==============================

struct XclAddress;
struct XclRange;
class XclRangeList;
class XclTokenArray;

typedef ScfRef< XclTokenArray > XclTokenArrayRef;

// Global data ================================================================

#ifdef DBG_UTIL
/** Counts the number of created root objects. */
struct XclDebugObjCounter
{
    sal_Int32           mnObjCnt;
    inline explicit     XclDebugObjCounter() : mnObjCnt( 0 ) {}
                        ~XclDebugObjCounter();
};
#endif

// ----------------------------------------------------------------------------

class SfxMedium;
class ScEditEngineDefaulter;
class ScHeaderEditEngine;
class EditEngine;
class ScExtDocOptions;
class XclFontPropSetHelper;
class XclChPropSetHelper;
class XclTracer;

struct RootData;//!

/** Stores global buffers and data needed elsewhere in the Excel filters. */
struct XclRootData
#ifdef DBG_UTIL
    : public XclDebugObjCounter
#endif
{
    typedef ScfRef< ScEditEngineDefaulter > ScEEDefaulterRef;
    typedef ScfRef< ScHeaderEditEngine >    ScHeaderEERef;
    typedef ScfRef< EditEngine >            EditEngineRef;
    typedef ScfRef< XclFontPropSetHelper >  XclFontPropSetHlpRef;
    typedef ScfRef< XclChPropSetHelper >    XclChPropSetHlpRef;
    typedef ScfRef< ScExtDocOptions >       ScExtDocOptRef;
    typedef ScfRef< XclTracer >             XclTracerRef;
    typedef ScfRef< RootData >              RootDataRef;

    XclBiff             meBiff;             /// Current BIFF version.
    SfxMedium&          mrMedium;           /// The medium to import from.
    SotStorageRef       mxRootStrg;         /// The root OLE storage of imported/exported file.
    ScDocument&         mrDoc;              /// The source or destination document.
    String              maDocUrl;           /// Document URL of imported/exported file.
    String              maBasePath;         /// Base path of imported/exported file (path of maDocUrl).
    String              maPassw;            /// Entered password for stream encryption/decryption.
    CharSet             meCharSet;          /// Character set to import/export byte strings.
    LanguageType        meSysLang;          /// System language.
    LanguageType        meDocLang;          /// Document language (import: from file, export: from system).
    LanguageType        meUILang;           /// UI language (import: from file, export: from system).
    sal_Int16           mnDefApiScript;     /// Default script type for blank cells (API constant).
    ScAddress           maScMaxPos;         /// Highest Calc cell position.
    ScAddress           maXclMaxPos;        /// Highest Excel cell position.
    ScAddress           maMaxPos;           /// Highest position valid in Calc and Excel.

    ScEEDefaulterRef    mxEditEngine;       /// Edit engine for rich strings etc.
    ScHeaderEERef       mxHFEditEngine;     /// Edit engine for header/footer.
    EditEngineRef       mxDrawEditEng;      /// Edit engine for text boxes.

    XclFontPropSetHlpRef mxFontPropSetHlp;  /// Property set helper for fonts.
    XclChPropSetHlpRef  mxChPropSetHlp;     /// Property set helper for chart filter.

    ScExtDocOptRef      mxExtDocOpt;        /// Extended document options.
    XclTracerRef        mxTracer;           /// Filter tracer.
    RootDataRef         mxRD;               /// Old RootData struct. Will be removed.

    long                mnCharWidth;        /// Width of '0' in default font (twips).
    SCTAB               mnScTab;            /// Current Calc sheet index.
    const bool          mbExport;           /// false = Import, true = Export.
    bool                mbHasPassw;         /// true = Password already querried.

    explicit            XclRootData( XclBiff eBiff, SfxMedium& rMedium,
                            SotStorageRef xRootStrg, ScDocument& rDoc,
                            CharSet eCharSet, bool bExport );
    virtual             ~XclRootData();
};

// ----------------------------------------------------------------------------

class SfxObjectShell;
class ScModelObj;
class SfxPrinter;
class SvNumberFormatter;
class SdrPage;
class ScDocumentPool;
class ScStyleSheetPool;
class ScRangeName;
class ScDBCollection;
struct XclFontData;

/** Access to global data for a filter object (imported or exported document) from other classes. */
class XclRoot
{
public:
    explicit            XclRoot( XclRootData& rRootData );
                        XclRoot( const XclRoot& rRoot );

    virtual             ~XclRoot();

    XclRoot&            operator=( const XclRoot& rRoot );

    /** Returns this root instance - for code readability in derived classes. */
    inline const XclRoot& GetRoot() const { return *this; }
    /** Returns old RootData struct. Deprecated. */
    inline RootData&    GetOldRoot() const { return *mrData.mxRD; }

    /** Returns the current BIFF version of the importer/exporter. */
    inline XclBiff      GetBiff() const { return mrData.meBiff; }
    /** Returns true, if currently a document is imported. */
    inline bool         IsImport() const { return !mrData.mbExport; }
    /** Returns true, if currently a document is exported. */
    inline bool         IsExport() const { return mrData.mbExport; }
    /** Returns the character set to import/export byte strings. */
    inline CharSet      GetCharSet() const { return mrData.meCharSet; }
    /** Returns the system language, i.e. for number formats. */
    inline LanguageType GetSysLanguage() const { return mrData.meSysLang; }
    /** Returns the document language. */
    inline LanguageType GetDocLanguage() const { return mrData.meDocLang; }
    /** Returns the UI language. */
    inline LanguageType GetUILanguage() const { return mrData.meUILang; }
    /** Returns the default script type, e.g. for blank cells. */
    inline sal_Int16    GetDefApiScript() const { return mrData.mnDefApiScript; }
    /** Returns the width of the '0' character (default font) for the current printer (twips). */
    inline long         GetCharWidth() const { return mrData.mnCharWidth; }
    /** Returns the current Calc sheet index. */
    inline bool         IsInGlobals() const { return mrData.mnScTab == SCTAB_GLOBAL; }
    /** Returns the current Calc sheet index. */
    inline SCTAB        GetCurrScTab() const { return mrData.mnScTab; }

    /** Returns the medium to import from. */
    inline SfxMedium&   GetMedium() const { return mrData.mrMedium; }
    /** Returns the document URL of the imported/exported file. */
    inline const String& GetDocUrl() const { return mrData.maDocUrl; }
    /** Returns the base path of the imported/exported file. */
    inline const String& GetBasePath() const { return mrData.maBasePath; }
    /** Queries a password from the user and returns it (empty string -> input cancelled). */
    const String&       QueryPassword() const;

    /** Returns the OLE2 root storage of the imported/exported file.
        @return  Pointer to root storage or 0, if the file is a simple stream. */
    inline SotStorageRef GetRootStorage() const { return mrData.mxRootStrg; }
    /** Returns true, if the document contains a VBA storage. */
    bool                HasVbaStorage() const;

    /** Tries to open a storage as child of the specified storage for reading or writing. */
    SotStorageRef       OpenStorage( SotStorageRef xStrg, const String& rStrgName ) const;
    /** Tries to open a storage as child of the root storage for reading or writing. */
    SotStorageRef       OpenStorage( const String& rStrgName ) const;
    /** Tries to open a new stream in the specified storage for reading or writing. */
    SotStorageStreamRef OpenStream( SotStorageRef xStrg, const String& rStrmName ) const;
    /** Tries to open a new stream in the root storage for reading or writing. */
    SotStorageStreamRef OpenStream( const String& rStrmName ) const;

    /** Returns the destination document (import) or source document (export). */
    inline ScDocument&  GetDoc() const { return mrData.mrDoc; }
    /** Returns pointer to the destination document (import) or source document (export). */
    inline ScDocument*  GetDocPtr() const { return &mrData.mrDoc; }
    /** Returns the object shell of the Calc document. May be 0 (i.e. import from clipboard). */
    SfxObjectShell*     GetDocShell() const;
    /** Returns the object model of the Calc document. */
    ScModelObj*         GetDocModelObj() const;
    /** Returns pointer to the printer of the Calc document. */
    SfxPrinter*         GetPrinter() const;
    /** Returns the number formatter of the Calc document. */
    SvNumberFormatter&  GetFormatter() const;
    /** Returns the style sheet pool of the Calc document. */
    ScStyleSheetPool&   GetStyleSheetPool() const;
    /** Returns the defined names container of the Calc document. */
    ScRangeName&        GetNamedRanges() const;
    /** Returns the database ranges container of the Calc document. */
    ScDBCollection&     GetDatabaseRanges() const;
    /** Returns the drawing layer page of the passed sheet, if present. */
    SdrPage*            GetSdrPage( SCTAB nScTab ) const;

    /** Returns the edit engine for import/export of rich strings etc. */
    ScEditEngineDefaulter& GetEditEngine() const;
    /** Returns the edit engine for import/export of headers/footers. */
    ScHeaderEditEngine& GetHFEditEngine() const;
    /** Returns the edit engine for import/export of drawing text boxes. */
    EditEngine&         GetDrawEditEngine() const;

    /** Returns the property set helper for fonts. */
    XclFontPropSetHelper& GetFontPropSetHelper() const;
    /** Returns the property set helper for the chart filters. */
    XclChPropSetHelper& GetChartPropSetHelper() const;

    /** Returns the extended document options. */
    ScExtDocOptions&    GetExtDocOptions() const;
    /** Returns the filter tracer. */
    XclTracer&          GetTracer() const;

    /** Returns the highest possible cell address in a Calc document. */
    inline const ScAddress& GetScMaxPos() const { return mrData.maScMaxPos; }
    /** Returns the highest possible cell address in an Excel document (using current BIFF version). */
    inline const ScAddress& GetXclMaxPos() const { return mrData.maXclMaxPos; }
    /** Returns the highest possible cell address valid in Calc and Excel (using current BIFF version). */
    inline const ScAddress& GetMaxPos() const { return mrData.maMaxPos; }

    /** Sets the document language. */
    inline void         SetDocLanguage( LanguageType eLang ) { mrData.meDocLang = eLang; }
    /** Sets the UI language, i.e. if it has been read from a file. */
    inline void         SetUILanguage( LanguageType eLang ) { mrData.meUILang = eLang; }
    /** Sets the character set to import/export byte strings. */
    inline void         SetCharSet( CharSet eCharSet ) { mrData.meCharSet = eCharSet; }
    /** Sets the width of the '0' character (default font) for the current printer (twips).
        @param rFontData  The font used for the '0' character. */
    void                SetCharWidth( const XclFontData& rFontData );
    /** Sets the current Calc sheet index. */
    inline void         SetCurrScTab( SCTAB nScTab ) { mrData.mnScTab = nScTab; }
    /** Increases the current Calc sheet index by 1. */
    inline void         IncCurrScTab() { ++mrData.mnScTab; }

private:
    mutable XclRootData& mrData;        /// Reference to the global data struct.
};

// ============================================================================

#endif

