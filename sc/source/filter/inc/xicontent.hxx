/*************************************************************************
 *
 *  $RCSfile: xicontent.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-26 18:05:10 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

// ============================================================================

#ifndef SC_XICONTENT_HXX
#define SC_XICONTENT_HXX

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_XIHELPER_HXX
#include "xihelper.hxx"
#endif

class XclImpStream;


/* ============================================================================
Classes to import the big Excel document contents (related to several cells or
globals for the document).
- Shared string tables
- Background bitmap
- Hyperlinks
- Label ranges
- Data validation
- Web queries
============================================================================ */

// Shared string table ========================================================

/** The SST (shared string table) contains all strings used in a BIFF8 file.
    @descr  This class loads the SST and provides access to the strings. */
class XclImpSst : protected XclImpRoot
{
private:
    ScfDelList< XclImpString >  maStringList;       /// List with formatted and unformatted strings.
    XclImpString                maErrorString;      /// Placeholder for strings not found in the list.

public:
    explicit                    XclImpSst( const XclImpRoot& rRoot );

    /** Reads the entire SST record.
        @descr  Import stream must be located at start of a SST record. */
    void                        ReadSst( XclImpStream& rStrm );

    /** Returns a pointer to the string with the passed index. */
    inline const XclImpString*  GetString( sal_uInt32 nSstIndex ) const;

    /** Creates a new text cell or edit cell for a Calc document.
        @param nXFIndex  Index to XF for first text portion (checks escapement). */
    ScBaseCell*                 CreateCell( sal_uInt32 nSstIndex, sal_uInt32 nXFIndex = 0 ) const;
};

inline const XclImpString*      XclImpSst::GetString( sal_uInt32 nSstIndex ) const
{
    return maStringList.GetObject( nSstIndex );
}


// Background bitmap ==========================================================

/** Provides importing a page background bitmap. */
class XclImpBitmap : ScfNoInstance
{
public:
    /** Reads a BITMAP record and inserts it into the current sheet.
        @descr  Import stream must be located at start of a BITMAP record. */
    static void                 ReadBitmap( XclImpStream& rStrm );
};


// Hyperlinks =================================================================

/** Provides importing hyperlinks and inserting them into a document. */
class XclImpHyperlink : ScfNoInstance
{
public:
    /** Reads a HLINK record and inserts it into the document.
        @descr  Import stream must be located at start of a HLINK record. */
    static void                 ReadHlink( XclImpStream& rStrm );

private:
    /** Reads character array and stores it into rString.
        @param nChars  Number of following characters (not byte count!).
        @param b16Bit  true = 16-bit characters, false = 8-bit characters. */
    static void                 AppendString32( String& rString, XclImpStream& rStrm, sal_uInt32 nChars, bool b16Bit );
    /** Reads 32-bit string length and the character array and stores it into rString.
        @param b16Bit  true = 16-bit characters, false = 8-bit characters. */
    static void                 AppendString32( String& rString, XclImpStream& rStrm, bool b16Bit );

    /** Reads 32-bit string length and ignores following character array.
        @param b16Bit  true = 16-bit characters, false = 8-bit characters. */
    static void                 IgnoreString32( XclImpStream& rStrm, bool b16Bit );

    /** Converts a path to an absolute path.
        @param rPath  The source path. The resulting path is returned here.
        @param nLevel  Number of parent directories to add in front of the path. */
    static void                 GetAbsPath( String& rPath, sal_uInt16 nLevel, SfxObjectShell* pDocShell );

    /** Inserts the URL into a text cell. Does not modify value or formula cells. */
    static void                 InsertUrl(
                                    const XclImpRoot& rRoot, const String& rURL,
                                    sal_uInt16 nCol, sal_uInt16 nRow );
};


// Label ranges ===============================================================

/** Provides importing label ranges and inserting them into a document. */
class XclImpLabelranges : ScfNoInstance
{
public:
    /** Reads a LABELRANGES record and inserts the label ranges into the document.
        @descr  Import stream must be located at start of a LABELRANGES record. */
    static void                 ReadLabelranges( XclImpStream& rStrm );
};


// Data Validation ============================================================

class ExcelToSc;

/** Provides importing validation data and inserting it into a document. */
class XclImpValidation : ScfNoInstance
{
public:
    /** Reads a DVAL record and sets marks the dropdown arrow control to be ignored. */
    static void                 ReadDval( XclImpStream& rStrm );
    /** Reads a DV record and inserts validation data into the document. */
    static void                 ReadDv( XclImpStream& rStrm, ExcelToSc& rFmlaConv );
};


// Web queries ================================================================

/** Stores the data of one web query. */
class XclImpWebQuery : ScfNoCopy
{
private:
    /** Specifies the type of the web query (which ranges are imported). */
    enum XclImpWebQueryMode
    {
        xlWQUnknown,                /// Not specified.
        xlWQDocument,               /// Entire document.
        xlWQAllTables,              /// All tables.
        xlWQSpecTables              /// Specific tables.
    };

    String                      maURL;          /// Source document URL.
    String                      maTables;       /// List of source range names.
    ScRange                     maDestRange;    /// Destination range.
    XclImpWebQueryMode          meMode;         /// Current mode of the web query.
    sal_uInt16                  mnRefresh;      /// Refresh time in minutes.

public:
                                XclImpWebQuery( const ScRange& rDestRange );

    /** Reads a PARAMQRY record and sets data to the web query. */
    void                        ReadParamqry( XclImpStream& rStrm );
    /** Reads a SXSTRING record and sets URL. */
    void                        ReadSxstring( XclImpStream& rStrm );
    /** Reads a WEBQRYSETTINGS record and sets refresh rate. */
    void                        ReadWqsettings( XclImpStream& rStrm );
    /** Reads a WEBQRYTABLES record and sets source range list. */
    void                        ReadWqtables( XclImpStream& rStrm );

    /** Inserts the web query into the document. */
    void                        Apply( ScDocument& rDoc, const String& rFilterName );
};


// ----------------------------------------------------------------------------

class XclImpWebQueryBuffer : protected XclImpRoot
{
private:
    ScfDelList< XclImpWebQuery > maWQList;      /// List of the web query objects.

public:
    inline                      XclImpWebQueryBuffer( const XclImpRoot& rRoot ) :
                                    XclImpRoot( rRoot ) {}

    /** Reads the QSI record and creates a new web query in the buffer. */
    void                        ReadQsi( XclImpStream& rStrm );
    /** Reads a PARAMQRY record and sets data to the current web query. */
    void                        ReadParamqry( XclImpStream& rStrm );
    /** Reads a SXSTRING record and sets URL to the current web query. */
    void                        ReadSxstring( XclImpStream& rStrm );
    /** Reads a WEBQRYSETTINGS record and sets refresh rate to the current web query. */
    void                        ReadWqsettings( XclImpStream& rStrm );
    /** Reads a WEBQRYTABLES record and sets source range list to the current web query. */
    void                        ReadWqtables( XclImpStream& rStrm );

    /** Inserts all web queries into the document. */
    void                        Apply();
};


// ============================================================================

#endif

