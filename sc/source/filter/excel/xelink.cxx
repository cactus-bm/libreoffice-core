/*************************************************************************
 *
 *  $RCSfile: xelink.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: obo $ $Date: 2004-10-18 15:14:37 $
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

#ifndef SC_XELINK_HXX
#include "xelink.hxx"
#endif

#include <algorithm>

#ifndef _UNOTOOLS_COLLATORWRAPPER_HXX
#include <unotools/collatorwrapper.hxx>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_CELL_HXX
#include "cell.hxx"
#endif
#ifndef _SCEXTOPT_HXX
#include "scextopt.hxx"
#endif

#include "root.hxx"

// ============================================================================
// *** Helper classes ***
// ============================================================================

// External names =============================================================

/** This is a base class for any external name (i.e. add-in names or DDE links).
    @descr  Derived classes implement creation and export of the external names. */
class XclExpExtNameBase : public XclExpRecord
{
public:
    /** @param nFlags  The flags to export directly.
        @param nAddSize  The size of additional data derived classes will write. */
    explicit            XclExpExtNameBase(
                            const String& rName,
                            sal_uInt16 nFlags = 0,
                            sal_uInt32 nAddSize = 0 );

    virtual             ~XclExpExtNameBase();

    /** Returns the name string of the external name. */
    inline const String& GetName() const { return maName; }

private:
    /** Writes the start of the record that is equal in all EXTERNNAME records and calls WriteAddData(). */
    virtual void        WriteBody( XclExpStream& rStrm );

    /** Called to write additional data following the common record contents.
        @descr  Derived classes should overwrite this function to write their data. */
    virtual void        WriteAddData( XclExpStream& rStrm );

private:
    String              maName;         /// Name of the external name.
    sal_uInt16          mnFlags;        /// Flags for record export.
};

// ----------------------------------------------------------------------------

/** Represents an EXTERNNAME record for an add-in function name. */
class XclExpExtNameAddIn : public XclExpExtNameBase
{
public:
    inline explicit     XclExpExtNameAddIn( const String& rName ) :
                            XclExpExtNameBase( rName, 0, 4 ) {}

private:
    /** Writes additional record contents. */
    virtual void        WriteAddData( XclExpStream& rStrm );
};

// ----------------------------------------------------------------------------

/** Represents an EXTERNNAME record for a DDE link. */
class XclExpExtNameDde : public XclExpExtNameBase
{
public:
    explicit            XclExpExtNameDde( const String& rName, sal_uInt16 nFlags );

    /** Inserts result list of a DDE link. */
    bool                InsertDde(
                            const XclExpRoot& rRoot,
                            const String& rApplic,
                            const String& rTopic,
                            const String& rItem );

private:
    /** Writes additional record contents. */
    virtual void        WriteAddData( XclExpStream& rStrm );

private:
    typedef ::std::auto_ptr< XclExpCachedMatrix > XclExpCachedMatrixPtr;

    XclExpCachedMatrixPtr mxMatrix;     /// Cached results of the DDE link.
    sal_uInt32          mnBaseSize;     /// Cached size of base record.
};

// List of external names =====================================================

/** List of all external names of a sheet. */
class XclExpExtNameBuffer : XclExpRecordBase
{
public:
    explicit            XclExpExtNameBuffer() : mbHasDde( false ) {}

    /** Inserts an add-in function name
        @return  The 1-based (Excel-like) list index of the name. */
    sal_uInt16          InsertAddIn( const String& rName );
    /** Inserts a DDE link.
        @return  The 1-based (Excel-like) list index of the DDE link. */
    sal_uInt16          InsertDde(
                            const XclExpRoot& rRoot,
                            const String& rApplic,
                            const String& rTopic,
                            const String& rItem );

    /** Writes the EXTERNNAME record list. */
    virtual void        Save( XclExpStream& rStrm );

private:
    typedef XclExpRecordList< XclExpExtNameBase >   XclExpExtNameList;
    typedef XclExpExtNameList::RecordRefType        XclExpExtNameRef;

private:
    /** Returns the 1-based (Excel-like) list index of the external name or 0, if not found. */
    sal_uInt16          GetIndex( const String& rName ) const;

    /** Appends the external name.
        @return  The 1-based (Excel-like) list index of the appended name. */
    sal_uInt16          Append( XclExpExtNameRef xName );

private:
    XclExpExtNameList   maNameList;     /// The list with all EXTERNNAME records.
    bool                mbHasDde;       /// true = contains DDE links.
};

// Cached external cells ======================================================

/** Base class to store the contents of one external cell (record CRN). */
class XclExpCrn : public XclExpRecord
{
protected:
    /** @param nAddSize  The size of additional data derived classes will write. */
    explicit            XclExpCrn( SCCOL nScCol, SCROW nScRow, sal_uInt8 nId, sal_uInt32 nAddLen = 0 );

private:
    /** Writes the start of the record that is equal in all CRN records and calls WriteAddData(). */
    virtual void        WriteBody( XclExpStream& rStrm );

    /** Called to write additional data following the common record contents.
        @descr  Derived classes should overwrite this function to write their data. */
    virtual void        WriteAddData( XclExpStream& rStrm ) = 0;

private:
    sal_uInt16          mnXclCol;   /// Column index of the external cell.
    sal_uInt16          mnXclRow;   /// Row index of the external cell.
    sal_uInt8           mnId;       /// Identifier for data type (EXC_CACHEDVAL_***).
};

// ----------------------------------------------------------------------------

/** Cached data of an external value cell. */
class XclExpCrnDouble : public XclExpCrn
{
public:
    explicit            XclExpCrnDouble( SCCOL nScCol, SCROW nScRow, double fVal );

private:
    /** Writes the double value following the common record contents. */
    virtual void        WriteAddData( XclExpStream& rStrm );

private:
    double              mfVal;      /// Value of the cached cell.
};

// ----------------------------------------------------------------------------

/** Cached data of an external text cell. */
class XclExpCrnString : public XclExpCrn
{
public:
    explicit            XclExpCrnString( SCCOL nScCol, SCROW nScRow, const String& rText );

private:
    /** Writes the string following the common record contents. */
    virtual void        WriteAddData( XclExpStream& rStrm );

private:
    XclExpString        maText;     /// Text of the cached cell.
};

// ----------------------------------------------------------------------------

/// Cached data of an external Boolean cell. */
class XclExpCrnBool : public XclExpCrn
{
public:
    explicit            XclExpCrnBool( SCCOL nScCol, SCROW nScRow, bool bBoolVal );

private:
    /** Writes the Boolean value following the common record contents. */
    virtual void        WriteAddData( XclExpStream& rStrm );

private:
    sal_uInt16          mnBool;     /// Boolean value of the cached cell.
};

// Cached cells of a sheet ====================================================

/// Represents the record XCT which is the header record of a CRN record list. */
class XclExpXct : public XclExpRecord
{
public:
    explicit            XclExpXct( const String& rTabName, sal_uInt16 nSBTab );

    /** Returns the external sheet name. */
    inline const XclExpString& GetTabName() const { return maTabName; }

    /** Stores all cells in the given range in the CRN list. */
    void                StoreCellRange( const XclExpRoot& rRoot, const ScRange& rRange );

    /** Writes the XCT and all CRN records. */
    virtual void        Save( XclExpStream& rStrm );

private:
    /** Writes the XCT record contents. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    typedef XclExpRecordList< XclExpCrn >   XclExpCrnList;
    typedef XclExpCrnList::RecordRefType    XclExpCrnRef;

    XclExpCrnList       maCrnList;      /// CRN records that follow this record.
    ScMarkData          maUsedCells;    /// Contains addresses of all stored cells.
    XclExpString        maTabName;      /// Sheet name of the external sheet.
    sal_uInt16          mnSBTab;        /// Referred sheet index in SUPBOOK record.
};

// External documents (SUPBOOK, BIFF8) ========================================

/** The SUPBOOK record contains data for an external document (URL, sheet names, external values). */
class XclExpSupbook : public XclExpRecord
{
public:
    /** Creates a SUPBOOK record for internal references. */
    explicit            XclExpSupbook( sal_uInt16 nXclTabCount );
    /** Creates a SUPBOOK record for add-in functions. */
    explicit            XclExpSupbook();
    /** Creates a SUPBOOK record for an external document. */
    explicit            XclExpSupbook( const XclExpRoot& rRoot, const String& rUrl );
    /** Creates a SUPBOOK record for a DDE link. */
    explicit            XclExpSupbook( const XclExpRoot& rRoot, const String& rApplic, const String& rTopic );

    /** Returns document URL encoded for Excel. */
    inline const XclExpString& GetUrlEncoded() const { return maUrlEncoded; }
    /** Returns the sheet name inside of this SUPBOOK. */
    const XclExpString* GetTabName( sal_uInt16 nXclTab ) const;

    /** Returns true, if this SUPBOOK contains the passed URL of an external document. */
    bool                IsUrlLink( const String& rUrl ) const;
    /** Returns true, if this SUPBOOK contains the passed DDE link. */
    bool                IsDdeLink( const String& rApplic, const String& rTopic ) const;

    /** Stores all cells in the given range in the CRN list of the specified SUPBOOK sheet. */
    void                StoreCellRange( const XclExpRoot& rRoot, const ScRange& rRange, sal_uInt16 nSBTab );

    /** Inserts a new sheet name into the SUPBOOK and returns the SUPBOOK internal sheet index. */
    sal_uInt16          InsertTabName( const String& rTabName );
    /** Finds or inserts an EXTERNNAME record for add-ins.
        @return  The 1-based EXTERNNAME record index. */
    sal_uInt16          InsertAddIn( const String& rName );
    /** Finds or inserts an EXTERNNAME record for DDE links.
        @return  The 1-based EXTERNNAME record index. */
    sal_uInt16          InsertDde( const XclExpRoot& rRoot, const String& rItem );

    /** Writes the SUPBOOK and all EXTERNNAME, XCT and CRN records. */
    virtual void        Save( XclExpStream& rStrm );

private:
    /** Creates and returns the list of EXTERNNAME records. */
    XclExpExtNameBuffer& GetExtNameBuffer();

    /** Writes the SUPBOOK record contents. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    typedef XclExpRecordList< XclExpXct >           XclExpXctList;
    typedef XclExpXctList::RecordRefType            XclExpXctRef;
    typedef ::std::auto_ptr< XclExpExtNameBuffer >  XclExpExtNameBfrPtr;

    XclExpXctList       maXctList;      /// List of XCT records (which contain CRN records).
    String              maUrl;          /// URL of the external document or application name for DDE.
    String              maDdeTopic;     /// Topic of an DDE link.
    XclExpString        maUrlEncoded;   /// Document name encoded for Excel.
    XclExpExtNameBfrPtr mxExtNameBfr;   /// List of EXTERNNAME records.
    XclSupbookType      meType;         /// Type of this SUPBOOK record.
    sal_uInt16          mnXclTabCount;  /// Number of internal sheets.
};

// All SUPBOOKS in a document =================================================

/** Contains a list of all SUPBOOK records and index arrays of external sheets. */
class XclExpSupbookBuffer : public XclExpRecordBase, protected XclExpRoot
{
public:
    explicit            XclExpSupbookBuffer( const XclExpRoot& rRoot );

    /** Returns the external document URL of the Excel sheet nXclTab. */
    const XclExpString* GetUrl( sal_uInt16 nXclTab ) const;
    /** Returns the external sheet name of the Excel sheet nXclTab. */
    const XclExpString* GetTabName( sal_uInt16 nXclTab ) const;

    /** Finds SUPBOOK index and SUPBOOK sheet range from given Excel sheet range.
        @param rnSupbook  Returns the index of the SUPBOOK record containing the sheet range.
        @param rnSBTabFirst  Returns the index of the first XTI structure.
        @param rnSBTabLast  Returns the index of the last XTI structure. */
    void                GetSBTabRange(
                            sal_uInt16& rnSupbook, sal_uInt16& rnSBTabFirst, sal_uInt16& rnSBTabLast,
                            sal_uInt16 nXclFirst, sal_uInt16 nXclLast ) const;

    /** Stores all cells in the given range in a CRN record list. */
    void                StoreCellRange( const ScRange& rRange );

    /** Finds or inserts an EXTERNNAME record for an add-in function name.
        @param rnSupbook  Returns the index of the SUPBOOK record which contains the add-in function name.
        @param rnExtName  Returns the 1-based EXTERNNAME record index. */
    void                InsertAddIn(
                            sal_uInt16& rnSupbook, sal_uInt16& rnExtName,
                            const String& rName );
    /** Finds or inserts an EXTERNNAME record for DDE links.
        @param rnSupbook  Returns the index of the SUPBOOK record which contains the DDE link.
        @param rnExtName  Returns the 1-based EXTERNNAME record index. */
    void                InsertDde(
                            sal_uInt16& rnSupbook, sal_uInt16& rnExtName,
                            const String& rApplic, const String& rTopic, const String& rItem );

    /** Writes all SUPBOOK records with their sub records. */
    virtual void        Save( XclExpStream& rStrm );

private:
    typedef XclExpRecordList< XclExpSupbook >   XclExpSupbookList;
    typedef XclExpSupbookList::RecordRefType    XclExpSupbookRef;

private:
    /** Returns the SUPBOOK record of an Excel sheet. */
    XclExpSupbookRef    GetSupbook( sal_uInt16 nXclTab ) const;
    /** Searches for the SUPBOOK record containing the passed document URL.
        @param rnIndex  Returns the list index, if the SUPBOOK exists.
        @return  The SUPBOOK record or 0, if not found. */
    XclExpSupbookRef    GetSupbookUrl( sal_uInt16& rnIndex, const String& rUrl ) const;
    /** Searches for the SUPBOOK record containing the passed DDE link.
        @param rnIndex  Returns the list index, if the SUPBOOK exists.
        @return  The SUPBOOK record or 0, if not found. */
    XclExpSupbookRef    GetSupbookDde( sal_uInt16& rnIndex, const String& rApplic, const String& rTopic ) const;

    /** Appends a new SUPBOOK to the list.
        @return  The list index of the SUPBOOK record. */
    sal_uInt16          Append( XclExpSupbookRef xSupbook );
    /** Creates and appends an external SUPBOOK record from the Calc sheet nScTab. */
    void                AddExt( SCTAB nScTab );

private:
    XclExpSupbookList   maSupbookList;      /// List of all SUPBOOK records.
    ScfUInt16Vec        maSBIndexBuffer;    /// SUPBOOK index for each Excel sheet.
    ScfUInt16Vec        maSBTabBuffer;      /// Sheet name indexes inside of SUPBOOK records.
    sal_uInt16          mnAddInSB;          /// Index to add-in SUPBOOK.
};

// Export link manager ========================================================

/** This struct contains a sheet index range for 3D references.
    @descr  This reference consists of an index to a SUPBOOK record and indexes
    to SUPBOOK sheet names. */
struct XclExpXti
{
    sal_uInt16          mnSupbook;      /// Index to SUPBOOK record.
    sal_uInt16          mnSBTabFirst;   /// Index to the first sheet of the range in the SUPBOOK.
    sal_uInt16          mnSBTabLast;    /// Index to the last sheet of the range in the SUPBOOK.

    inline explicit     XclExpXti() : mnSupbook( 0 ), mnSBTabFirst( 0 ), mnSBTabLast( 0 ) {}
    inline explicit     XclExpXti( sal_uInt16 nSupbook, sal_uInt16 nSBTabFirst, sal_uInt16 nSBTabLast ) :
                            mnSupbook( nSupbook ), mnSBTabFirst( nSBTabFirst ), mnSBTabLast( nSBTabLast ) {}

    /** Writes this XTI structure (inside of the EXTERNSHEET record). */
    inline void         Save( XclExpStream& rStrm ) const
                            { rStrm << mnSupbook << mnSBTabFirst << mnSBTabLast; }
};

inline bool operator==( const XclExpXti& rLeft, const XclExpXti& rRight )
{
    return
        (rLeft.mnSupbook    == rRight.mnSupbook)    &&
        (rLeft.mnSBTabFirst == rRight.mnSBTabFirst) &&
        (rLeft.mnSBTabLast  == rRight.mnSBTabLast);
}

// ----------------------------------------------------------------------------

/** Implementation of the link manager. */
class XclExpLinkManagerImpl : public XclExpRecordBase, protected XclExpRoot
{
public:
    explicit            XclExpLinkManagerImpl( const XclExpRoot& rRoot );

    /** Searches for XTI structure with the given Excel sheet range. Adds new XTI if not found.
        @return  The list index of the XTI structure. */
    sal_uInt16          FindXti( sal_uInt16 nXclFirst, sal_uInt16 nXclLast );

    /** Returns the external document URL of the specified Excel sheet. */
    inline const XclExpString* GetUrl( sal_uInt16 nXclTab ) const
                            { return maSBBuffer.GetUrl( nXclTab ); }
    /** Returns the external sheet name of the specified Excel sheet. */
    inline const XclExpString* GetTabName( sal_uInt16 nXclTab ) const
                            { return maSBBuffer.GetTabName( nXclTab ); }

    /** Stores all cells in the given range in a CRN record list. */
    void                StoreCellRange( const SingleRefData& rRef1, const SingleRefData& rRef2 );

    /** Finds or inserts an EXTERNNAME record for an add-in function name.
        @param rnXti  Returns the index of the XTI structure which contains the add-in function name.
        @param rnExtName  Returns the 1-based EXTERNNAME record index. */
    void                InsertAddIn(
                            sal_uInt16& rnXti, sal_uInt16& rnExtName,
                            const String& rName );
    /** Finds or inserts an EXTERNNAME record for DDE links.
        @param rnXti  Returns the index of the XTI structure which contains the DDE link.
        @param rnExtName  Returns the 1-based EXTERNNAME record index. */
    bool                InsertDde(
                            sal_uInt16& rnXti, sal_uInt16& rnExtName,
                            const String& rApplic, const String& rTopic, const String& rItem );

    /** Writes all SUPBOOK records with their sub records and the trailing EXTERNSHEET record. */
    virtual void        Save( XclExpStream& rStrm );

private:
    /** Searches for or inserts a new XTI structure.
        @return  The list index of the XTI structure. */
    sal_uInt16          InsertXti( const XclExpXti& rXti );

private:
    typedef ::std::vector< XclExpXti > XclExpXtiVec;

    XclExpXtiVec        maXtiVec;       /// List of XTI structures.
    XclExpSupbookBuffer maSBBuffer;     /// List of all SUPBOOK records.
};

// ============================================================================
// *** Implementation ***
// ============================================================================

// Excel sheet indexes ========================================================

const sal_uInt8 EXC_TABBUF_IGNORE   = 0x01;     /// Sheet will be ignored completely.
const sal_uInt8 EXC_TABBUF_EXTERN   = 0x02;     /// Sheet is extern linked.
const sal_uInt8 EXC_TABBUF_SKIPMASK = 0x0F;     /// Sheet will be skipped, if any flag is set.
const sal_uInt8 EXC_TABBUF_VISIBLE  = 0x10;     /// Sheet is visible.
const sal_uInt8 EXC_TABBUF_SELECTED = 0x20;     /// Sheet is selected.

// ----------------------------------------------------------------------------

XclExpTabInfo::XclExpTabInfo( const XclExpRoot& rRoot ) :
    mnScCnt( 0 ),
    mnXclCnt( 0 ),
    mnXclExtCnt( 0 ),
    mnXclCodeCnt( 0 ),
    mnXclSelected( 0 ),
    mnXclActive( 0 ),
    mnXclFirstVis( 0 ),
    mbEnableLog( false )
{
    ScDocument& rDoc = rRoot.GetDoc();
    ScExtDocOptions& rExtDocOpt = rRoot.GetExtDocOptions();

    mnScCnt = rDoc.GetTableCount();

    SCTAB nScTab;
    SCTAB nScFirstVis = SCTAB_MAX;   // first visible sheet
    SCTAB nScFirstExp = SCTAB_MAX;   // first exported sheet

    // --- initialize the flags in the index buffer ---

    maTabInfoVec.resize( mnScCnt, ScTabInfoEntry( 0, 0 ) );
    for( nScTab = 0; nScTab < mnScCnt; ++nScTab )
    {
        // ignored sheets (skipped by export, with invalid Excel sheet index)
        if( rDoc.IsScenario( nScTab ) )
        {
            SetFlag( nScTab, EXC_TABBUF_IGNORE );
        }

        // external sheets (skipped, but with valid Excel sheet index for ref's)
        else if( rDoc.GetLinkMode( nScTab ) == SC_LINK_VALUE )
        {
            SetFlag( nScTab, EXC_TABBUF_EXTERN );
        }

        // exported sheets
        else
        {
            // remember first exported sheet
            if( nScFirstExp == SCTAB_MAX )
               nScFirstExp = nScTab;
            // remember first visible exported sheet
            if( (nScFirstVis == SCTAB_MAX) && rDoc.IsVisible( nScTab ) )
               nScFirstVis = nScTab;

            // sheet visible (only exported sheets)
            SetFlag( nScTab, EXC_TABBUF_VISIBLE, rDoc.IsVisible( nScTab ) );

            // sheet selected (only exported sheets)
            if( const ScExtTabOptions* pExtTabOpt = rExtDocOpt.GetExtTabOptions( nScTab ) )
                SetFlag( nScTab, EXC_TABBUF_SELECTED, pExtTabOpt->bSelected );
        }
    }

    // --- visible/selected sheets ---

    // #112908# find first visible exported sheet
    if( (nScFirstVis == SCTAB_MAX) || !IsExportTab( nScFirstVis ) )
    {
        // no exportable visible sheet -> use first exportable sheet
        nScFirstVis = nScFirstExp;
        if( (nScFirstVis == SCTAB_MAX) || !IsExportTab( nScFirstVis ) )
        {
            // no exportable sheet at all -> use active sheet and export it
            nScFirstVis = static_cast<SCTAB>(rExtDocOpt.nActTab);
            SetFlag( nScFirstVis, EXC_TABBUF_SKIPMASK, false ); // clear skip flags
        }
        SetFlag( nScFirstVis, EXC_TABBUF_VISIBLE ); // must be visible, even if originally hidden
    }

    // find current active sheet
    SCTAB nScActive = static_cast<SCTAB>(rExtDocOpt.nActTab);
    if( !IsExportTab( nScActive ) ) // selected sheet not exported (i.e. scenario) -> use first visible
        nScActive = nScFirstVis;
    SetFlag( nScActive, EXC_TABBUF_VISIBLE | EXC_TABBUF_SELECTED );

    // number of selected sheets
    for( nScTab = 0; nScTab < mnScCnt; ++nScTab )
        if( IsSelectedTab( nScTab ) )
            ++mnXclSelected;

    // --- calculate resulting Excel sheet indexes ---

    CalcXclIndexes();
    mnXclFirstVis = GetXclTab( nScFirstVis );
    mnXclActive = GetXclTab( nScActive );

    // --- sorted vectors for index lookup ---

    CalcSortedIndexes( rDoc );

    // --- VB codenames ---

    if( CodenameList* pCList = rExtDocOpt.GetCodenames() )
        mnXclCodeCnt = ulimit_cast< sal_uInt16 >( pCList->Count() );
}

bool XclExpTabInfo::IsExportTab( SCTAB nScTab ) const
{
    DBG_ASSERT( nScTab < mnScCnt, "XclExpTabInfo::IsExportTab - sheet out of range" );
    return (nScTab < mnScCnt) && !GetFlag( nScTab, EXC_TABBUF_SKIPMASK );
}

bool XclExpTabInfo::IsExternalTab( SCTAB nScTab ) const
{
    /*  Check sheet index before to avoid assertion (called from formula
        compiler also for deleted references). */
    return (nScTab < mnScCnt) && GetFlag( nScTab, EXC_TABBUF_EXTERN );
}

bool XclExpTabInfo::IsVisibleTab( SCTAB nScTab ) const
{
    return GetFlag( nScTab, EXC_TABBUF_VISIBLE );
}

bool XclExpTabInfo::IsSelectedTab( SCTAB nScTab ) const
{
    return GetFlag( nScTab, EXC_TABBUF_SELECTED );
}

bool XclExpTabInfo::IsActiveTab( SCTAB nScTab ) const
{
    DBG_ASSERT( nScTab < mnScCnt, "XclExpTabInfo::IsActiveTab - sheet out of range" );
    return GetXclTab( nScTab ) == mnXclActive;
}

sal_uInt16 XclExpTabInfo::GetXclTab( SCTAB nScTab ) const
{
    return (nScTab < mnScCnt) ? maTabInfoVec[ nScTab ].first : EXC_EXTSH_DELETED;
}

SCTAB XclExpTabInfo::GetRealScTab( SCTAB nSortedScTab ) const
{
    DBG_ASSERT( nSortedScTab < mnScCnt, "XclExpTabInfo::GetRealScTab - sheet out of range" );
    return (nSortedScTab < mnScCnt) ? maFromSortedVec[ nSortedScTab ] : SCTAB_MAX;
}

SCTAB XclExpTabInfo::GetSortedScTab( SCTAB nScTab ) const
{
    DBG_ASSERT( nScTab < mnScCnt, "XclExpTabInfo::GetSortedScTab - sheet out of range" );
    return (nScTab < mnScCnt) ? maToSortedVec[ nScTab ] : SCTAB_MAX;
}

void XclExpTabInfo::StartRefLog()
{
    maRefLog.clear();
    mbEnableLog = true;
}

const XclExpRefLogVec& XclExpTabInfo::EndRefLog()
{
    mbEnableLog = false;
    return maRefLog;
}

void XclExpTabInfo::AppendTabRef( sal_uInt16 nXclFirst, sal_uInt16 nXclLast )
{
    if( mbEnableLog )
        maRefLog.push_back( XclExpRefLogEntry( nXclFirst, nXclLast ) );
}

bool XclExpTabInfo::GetFlag( SCTAB nScTab, sal_uInt8 nFlags ) const
{
    DBG_ASSERT( nScTab < mnScCnt, "XclExpTabInfo::GetFlag - sheet out of range" );
    return (nScTab < mnScCnt) && ::get_flag( maTabInfoVec[ nScTab ].second, nFlags );
}

void XclExpTabInfo::SetFlag( SCTAB nScTab, sal_uInt8 nFlags, bool bSet )
{
    DBG_ASSERT( nScTab < mnScCnt, "XclExpTabInfo::SetFlag - sheet out of range" );
    if( nScTab < mnScCnt )
        ::set_flag( maTabInfoVec[ nScTab ].second, nFlags, bSet );
}

void XclExpTabInfo::CalcXclIndexes()
{
    sal_uInt16 nXclTab = 0;
    SCTAB nScTab = 0;

    // --- pass 1: process regular sheets ---
    for( nScTab = 0; nScTab < mnScCnt; ++nScTab )
    {
        if( IsExportTab( nScTab ) )
        {
            maTabInfoVec[ nScTab ].first = nXclTab;
            ++nXclTab;
        }
        else
            maTabInfoVec[ nScTab ].first = EXC_EXTSH_DELETED;
    }
    mnXclCnt = nXclTab;

    // --- pass 2: process external sheets (nXclTab continues) ---
    for( nScTab = 0; nScTab < mnScCnt; ++nScTab )
    {
        if( IsExternalTab( nScTab ) )
        {
            maTabInfoVec[ nScTab ].first = nXclTab;
            ++nXclTab;
            ++mnXclExtCnt;
        }
    }

    // result: first occur all exported sheets, followed by all external sheets
}

typedef ::std::pair< String, SCTAB >    XclExpTabName;
typedef ::std::vector< XclExpTabName >  XclExpTabNameVec;

inline bool operator<( const XclExpTabName& rArg1, const XclExpTabName& rArg2 )
{
    // compare the sheet names only
    return ScGlobal::pCollator->compareString( rArg1.first, rArg2.first ) == COMPARE_LESS;
}

void XclExpTabInfo::CalcSortedIndexes( ScDocument& rDoc )
{
    XclExpTabNameVec aVec( mnScCnt );
    SCTAB nScTab;

    // fill with sheet names
    for( nScTab = 0; nScTab < mnScCnt; ++nScTab )
    {
        rDoc.GetName( nScTab, aVec[ nScTab ].first );
        aVec[ nScTab ].second = nScTab;
    }
    ::std::sort( aVec.begin(), aVec.end() );

    // fill index vectors from sorted sheet name vector
    maFromSortedVec.resize( mnScCnt );
    maToSortedVec.resize( mnScCnt );
    for( nScTab = 0; nScTab < mnScCnt; ++nScTab )
    {
        maFromSortedVec[ nScTab ] = aVec[ nScTab ].second;
        maToSortedVec[ aVec[ nScTab ].second ] = nScTab;
    }
}

// External names =============================================================

XclExpExtNameBase::XclExpExtNameBase( const String& rName, sal_uInt16 nFlags, sal_uInt32 nAddSize ) :
    XclExpRecord( EXC_ID_EXTERNNAME, rName.Len() + 8 + nAddSize ),
    maName( rName ),
    mnFlags( nFlags )
{
    DBG_ASSERT( maName.Len() <= 255, "XclExpExtNameBase::XclExpExtNameBase - string too long" );
}

XclExpExtNameBase::~XclExpExtNameBase()
{
}

void XclExpExtNameBase::WriteBody( XclExpStream& rStrm )
{
    rStrm   << mnFlags
            << sal_uInt32( 0 )
            << XclExpString( maName, EXC_STR_8BITLENGTH );
    WriteAddData( rStrm );
}

void XclExpExtNameBase::WriteAddData( XclExpStream& rStrm )
{
}

// ----------------------------------------------------------------------------

void XclExpExtNameAddIn::WriteAddData( XclExpStream& rStrm )
{
    rStrm << sal_uInt16( 2 ) << sal_uInt16( 0x171C );  // error value 0x17
}

// ----------------------------------------------------------------------------

XclExpExtNameDde::XclExpExtNameDde( const String& rName, sal_uInt16 nFlags ) :
    XclExpExtNameBase( rName, nFlags ),
    mnBaseSize( GetRecSize() )
{
}

bool XclExpExtNameDde::InsertDde(
        const XclExpRoot& rRoot,
        const String& rApplic, const String& rTopic, const String& rItem )
{
    ScDocument& rDoc = rRoot.GetDoc();
    USHORT nPos;
    bool bRet = rDoc.FindDdeLink( rApplic, rTopic, rItem, SC_DDE_IGNOREMODE, nPos );
    if( bRet )
    {
        // try to find results, but it is not necessary for success of this func (bRet remains true)
        if( const ScMatrix* pScMatrix = rDoc.GetDdeLinkResultMatrix( nPos ) )
        {
            mxMatrix.reset( new XclExpCachedMatrix( *pScMatrix ) );
            SetRecSize( mnBaseSize + mxMatrix->GetSize() );
        }
        else
        {
            mxMatrix.reset();
            SetRecSize( mnBaseSize );
        }
    }
    return bRet;
}

void XclExpExtNameDde::WriteAddData( XclExpStream& rStrm )
{
    if( mxMatrix.get() )
        mxMatrix->Save( rStrm );
}

// List of external names =====================================================

sal_uInt16 XclExpExtNameBuffer::InsertAddIn( const String& rName )
{
    sal_uInt16 nIndex = GetIndex( rName );
    return nIndex ? nIndex : Append( XclExpExtNameRef( new XclExpExtNameAddIn( rName ) ) );
}

sal_uInt16 XclExpExtNameBuffer::InsertDde(
        const XclExpRoot& rRoot,
        const String& rApplic, const String& rTopic, const String& rItem )
{
    if( !mbHasDde )
    {
        Append( XclExpExtNameRef( new XclExpExtNameDde(
            String( RTL_CONSTASCII_USTRINGPARAM( "StdDocumentName" ) ),
            EXC_EXTN_EXPDDE_STDDOC ) ) );
        mbHasDde = true;
    }

    sal_uInt16 nIndex = GetIndex( rItem );
    if( !nIndex )
    {
        XclExpExtNameDde* pName = new XclExpExtNameDde( rItem, EXC_EXTN_EXPDDE );
        XclExpExtNameRef xName( pName );
        if( pName->InsertDde( rRoot, rApplic, rTopic, rItem ) )
            nIndex = Append( xName );
    }
    return nIndex;
}

void XclExpExtNameBuffer::Save( XclExpStream& rStrm )
{
    maNameList.Save( rStrm );
}

sal_uInt16 XclExpExtNameBuffer::GetIndex( const String& rName ) const
{
    for( size_t nNamePos = 0, nNameSize = maNameList.Size(); nNamePos < nNameSize; ++nNamePos )
        if( maNameList.GetRecord( nNamePos )->GetName() == rName )
            return ulimit_cast< sal_uInt16 >( nNamePos + 1 );
    return 0;
}

sal_uInt16 XclExpExtNameBuffer::Append( XclExpExtNameRef xName )
{
    maNameList.AppendRecord( xName );
    return ulimit_cast< sal_uInt16 >( maNameList.Size() );
}

// Cached external cells ======================================================

XclExpCrn::XclExpCrn( SCCOL nScCol, SCROW nScRow, sal_uInt8 nId, sal_uInt32 nAddLen ) :
    XclExpRecord( EXC_ID_CRN, 5 + nAddLen ),
    mnXclCol( static_cast< sal_uInt16 >( nScCol ) ),
    mnXclRow( static_cast< sal_uInt16 >( nScRow ) ),
    mnId( nId )
{
}

void XclExpCrn::WriteBody( XclExpStream& rStrm )
{
    rStrm   << static_cast< sal_uInt8 >( mnXclCol )
            << static_cast< sal_uInt8 >( mnXclCol )
            << mnXclRow
            << mnId;
    WriteAddData( rStrm );
}

// ----------------------------------------------------------------------------

XclExpCrnDouble::XclExpCrnDouble( SCCOL nScCol, SCROW nScRow, double fVal ) :
    XclExpCrn( nScCol, nScRow, EXC_CACHEDVAL_DOUBLE, 8 ),
    mfVal( fVal )
{
}

void XclExpCrnDouble::WriteAddData( XclExpStream& rStrm )
{
    rStrm << mfVal;
}

// ----------------------------------------------------------------------------

XclExpCrnString::XclExpCrnString( SCCOL nScCol, SCROW nScRow, const String& rText ) :
    XclExpCrn( nScCol, nScRow, EXC_CACHEDVAL_STRING ),
    maText( rText )
{
    // set correct size after maText is initialized
    SetRecSize( GetRecSize() + maText.GetSize() );
}

void XclExpCrnString::WriteAddData( XclExpStream& rStrm )
{
    rStrm << maText;
}

// ----------------------------------------------------------------------------

XclExpCrnBool::XclExpCrnBool( SCCOL nScCol, SCROW nScRow, bool bBoolVal ) :
    XclExpCrn( nScCol, nScRow, EXC_CACHEDVAL_BOOL, 8 ),
    mnBool( bBoolVal ? 1 : 0 )
{
}

void XclExpCrnBool::WriteAddData( XclExpStream& rStrm )
{
    rStrm << mnBool;
    rStrm.WriteZeroBytes( 6 );
}

// Cached cells of a sheet ====================================================

XclExpXct::XclExpXct( const String& rTabName, sal_uInt16 nSBTab ) :
    XclExpRecord( EXC_ID_XCT, 4 ),
    maTabName( rTabName ),
    mnSBTab( nSBTab )
{
}

void XclExpXct::StoreCellRange( const XclExpRoot& rRoot, const ScRange& rRange )
{
    ScDocument& rDoc = rRoot.GetDoc();
    SvNumberFormatter& rFormatter = rRoot.GetFormatter();
    SCTAB nScTab = rRange.aStart.Tab();
    SCCOL nScLastCol = rRange.aEnd.Col();
    SCROW nScLastRow = rRange.aEnd.Row();

    for( SCROW nScRow = rRange.aStart.Row(); nScRow <= nScLastRow; ++nScRow )
    {
        for( SCCOL nScCol = rRange.aStart.Col(); nScCol <= nScLastCol; ++nScCol )
        {
            if( !maUsedCells.IsCellMarked( nScCol, nScRow, TRUE ) )
            {
                XclExpCrnRef xCrn;
                if( rDoc.HasValueData( nScCol, nScRow, nScTab ) )
                {
                    ScAddress   aAddr( nScCol, nScRow, nScTab );
                    double      fVal    = rDoc.GetValue( aAddr );
                    ULONG       nFormat = rDoc.GetNumberFormat( aAddr );
                    short       nType   = rFormatter.GetType( nFormat );
                    bool        bIsBool = (nType == NUMBERFORMAT_LOGICAL);

                    if( !bIsBool && ((nFormat % SV_COUNTRY_LANGUAGE_OFFSET) == 0) &&
                            (rDoc.GetCellType( aAddr ) == CELLTYPE_FORMULA) )
                        if( ScFormulaCell* pCell = static_cast< ScFormulaCell* >( rDoc.GetCell( aAddr ) ) )
                            bIsBool = (pCell->GetFormatType() == NUMBERFORMAT_LOGICAL);

                    if( bIsBool && ((fVal == 0.0) || (fVal == 1.0)) )
                        xCrn.reset( new XclExpCrnBool( nScCol, nScRow, (fVal == 1.0) ) );
                    else
                        xCrn.reset( new XclExpCrnDouble( nScCol, nScRow, fVal ) );
                }
                else
                {
                    String aText;
                    rDoc.GetString( nScCol, nScRow, nScTab, aText );
                    xCrn.reset( new XclExpCrnString( nScCol, nScRow, aText ) );
                }
                maCrnList.AppendRecord( xCrn );
            }
        }
    }

    maUsedCells.SetMultiMarkArea( rRange );
}

void XclExpXct::Save( XclExpStream& rStrm )
{
    XclExpRecord::Save( rStrm );
    maCrnList.Save( rStrm );
}

void XclExpXct::WriteBody( XclExpStream& rStrm )
{
    sal_uInt16 nCount = ulimit_cast< sal_uInt16 >( maCrnList.Size() );
    rStrm << nCount << mnSBTab;
}

// External document (SUPBOOK, BIFF8) =========================================

XclExpSupbook::XclExpSupbook( sal_uInt16 nXclTabCount ) :
    XclExpRecord( EXC_ID_SUPBOOK, 4 ),
    meType( EXC_SBTYPE_SELF ),
    mnXclTabCount( nXclTabCount )
{
}

XclExpSupbook::XclExpSupbook() :
    XclExpRecord( EXC_ID_SUPBOOK, 4 ),
    meType( EXC_SBTYPE_ADDIN ),
    mnXclTabCount( 1 )
{
}

XclExpSupbook::XclExpSupbook( const XclExpRoot& rRoot, const String& rUrl ) :
    XclExpRecord( EXC_ID_SUPBOOK ),
    maUrl( rUrl ),
    maUrlEncoded( XclExpUrlHelper::EncodeUrl( rRoot, rUrl ) ),
    meType( EXC_SBTYPE_EXTERN ),
    mnXclTabCount( 0 )
{
    SetRecSize( 2 + maUrlEncoded.GetSize() );
}

XclExpSupbook::XclExpSupbook( const XclExpRoot& rRoot, const String& rApplic, const String& rTopic ) :
    XclExpRecord( EXC_ID_SUPBOOK ),
    maUrl( rApplic ),
    maDdeTopic( rTopic ),
    maUrlEncoded( XclExpUrlHelper::EncodeDde( rApplic, rTopic ) ),
    meType( EXC_SBTYPE_SPECIAL ),
    mnXclTabCount( 0 )
{
    SetRecSize( 2 + maUrlEncoded.GetSize() );
}

const XclExpString* XclExpSupbook::GetTabName( sal_uInt16 nXclTab ) const
{
    const XclExpXctRef xXct = maXctList.GetRecord( nXclTab );
    return xXct.get() ? &xXct->GetTabName() : 0;
}

bool XclExpSupbook::IsUrlLink( const String& rUrl ) const
{
    return (meType == EXC_SBTYPE_EXTERN) && (maUrl == rUrl);
}

bool XclExpSupbook::IsDdeLink( const String& rApplic, const String& rTopic ) const
{
    return (meType == EXC_SBTYPE_SPECIAL) && (maUrl == rApplic) && (maDdeTopic == rTopic);
}

void XclExpSupbook::StoreCellRange( const XclExpRoot& rRoot, const ScRange& rRange, sal_uInt16 nSBTab )
{
    XclExpXctRef xXct = maXctList.GetRecord( nSBTab );
    if( xXct.get() )
        xXct->StoreCellRange( rRoot, rRange );
}

sal_uInt16 XclExpSupbook::InsertTabName( const String& rTabName )
{
    DBG_ASSERT( meType == EXC_SBTYPE_EXTERN, "XclExpSupbook::InsertTabName - don't insert sheet names here" );
    sal_uInt16 nSBTab = ulimit_cast< sal_uInt16 >( maXctList.Size() );
    XclExpXctRef xXct( new XclExpXct( rTabName, nSBTab ) );
    SetRecSize( GetRecSize() + xXct->GetTabName().GetSize() );
    maXctList.AppendRecord( xXct );
    return nSBTab;
}

sal_uInt16 XclExpSupbook::InsertAddIn( const String& rName )
{
    return GetExtNameBuffer().InsertAddIn( rName );
}

sal_uInt16 XclExpSupbook::InsertDde( const XclExpRoot& rRoot, const String& rItem )
{
    return GetExtNameBuffer().InsertDde( rRoot, maUrl, maDdeTopic, rItem );
}

XclExpExtNameBuffer& XclExpSupbook::GetExtNameBuffer()
{
    if( !mxExtNameBfr.get() )
        mxExtNameBfr.reset( new XclExpExtNameBuffer );
    return *mxExtNameBfr;
}

void XclExpSupbook::Save( XclExpStream& rStrm )
{
    XclExpRecord::Save( rStrm );
    maXctList.Save( rStrm );
    if( mxExtNameBfr.get() )
        mxExtNameBfr->Save( rStrm );
}

void XclExpSupbook::WriteBody( XclExpStream& rStrm )
{
    switch( meType )
    {
        case EXC_SBTYPE_SELF:
            rStrm << mnXclTabCount << EXC_SUPB_SELF;
        break;
        case EXC_SBTYPE_EXTERN:
        case EXC_SBTYPE_SPECIAL:
        {
            sal_uInt16 nCount = ulimit_cast< sal_uInt16 >( maXctList.Size() );
            rStrm << nCount << maUrlEncoded;

            for( size_t nPos = 0, nSize = maXctList.Size(); nPos < nSize; ++nPos )
                rStrm << maXctList.GetRecord( nPos )->GetTabName();
        }
        break;
        case EXC_SBTYPE_ADDIN:
            rStrm << mnXclTabCount << EXC_SUPB_ADDIN;
        break;
        default:
            DBG_ERRORFILE( "XclExpSupbook::WriteBody - unknown SUPBOOK type" );
    }
}

// All SUPBOOKS in a document =================================================

XclExpSupbookBuffer::XclExpSupbookBuffer( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    mnAddInSB( 0xFFFF )
{
    XclExpTabInfo& rTabInfo = GetTabInfo();

    SCTAB nScCnt = rTabInfo.GetScTabCount();
    sal_uInt16 nXclCnt = rTabInfo.GetXclTabCount();
    size_t nCount = nXclCnt + rTabInfo.GetXclExtTabCount();

    if( nCount )
    {
        maSBIndexBuffer.resize( nCount );
        maSBTabBuffer.resize( nCount );

        // self-ref SUPBOOK first of list
        XclExpSupbookRef xSupbook( new XclExpSupbook( ::std::max( nXclCnt, rTabInfo.GetXclCodenameCount() ) ) );
        sal_uInt16 nSelfSB = Append( xSupbook );
        for( sal_uInt16 nXclTab = 0; nXclTab < nXclCnt; ++nXclTab )
        {
            maSBIndexBuffer[ nXclTab ] = nSelfSB;
            maSBTabBuffer[ nXclTab ] = nXclTab;
        }

        // add SUPBOOKs with external references
        for( SCTAB nScTab = 0; nScTab < nScCnt; ++nScTab )
            if( rTabInfo.IsExternalTab( nScTab ) )
                AddExt( nScTab );
    }
}

const XclExpString* XclExpSupbookBuffer::GetUrl( sal_uInt16 nXclTab ) const
{
    const XclExpSupbookRef xSupbook = GetSupbook( nXclTab );
    const XclExpString* pString = xSupbook.get() ? &xSupbook->GetUrlEncoded() : 0;
    return (pString && pString->Len()) ? pString : 0;
}

const XclExpString* XclExpSupbookBuffer::GetTabName( sal_uInt16 nXclTab ) const
{
    DBG_ASSERT( nXclTab < maSBTabBuffer.size(), "XclExpSupbookBuffer::GetTabName - out of range" );
    const XclExpSupbookRef xSupbook = GetSupbook( nXclTab );
    return xSupbook.get() ? xSupbook->GetTabName( maSBTabBuffer[ nXclTab ] ) : 0;
}

void XclExpSupbookBuffer::GetSBTabRange(
        sal_uInt16& rnSupbook, sal_uInt16& rnSBTabFirst, sal_uInt16& rnSBTabLast,
        sal_uInt16 nXclFirst, sal_uInt16 nXclLast ) const
{
    size_t nCount = maSBIndexBuffer.size();
    if( (nXclFirst < nCount) && (nXclLast < nCount) )
    {
        // external reference
        rnSupbook = maSBIndexBuffer[ nXclFirst ];

        // all sheets in the same supbook?
        bool bConflict = false;
        for( sal_uInt16 nXclTab = nXclFirst + 1; !bConflict && (nXclTab <= nXclLast); ++nXclTab )
        {
            bConflict = (maSBIndexBuffer[ nXclTab ] != rnSupbook);
            if( bConflict )
                nXclLast = nXclTab - 1;
        }
        rnSBTabFirst = maSBTabBuffer[ nXclFirst ];
        rnSBTabLast = maSBTabBuffer[ nXclLast ];
    }
    else
    {
        // internal reference
        rnSupbook = 0;
        rnSBTabFirst = nXclFirst;
        rnSBTabLast = nXclLast;
    }
}

void XclExpSupbookBuffer::StoreCellRange( const ScRange& rRange )
{
    sal_uInt16 nXclTab = GetTabInfo().GetXclTab( rRange.aStart.Tab() );
    DBG_ASSERT( nXclTab < maSBTabBuffer.size(), "XclExpSupbookBuffer::StoreCellRange - out of range" );

    XclExpSupbookRef xSupbook = GetSupbook( nXclTab );
    if( xSupbook.get() )
        xSupbook->StoreCellRange( GetRoot(), rRange, maSBTabBuffer[ nXclTab ] );
}

void XclExpSupbookBuffer::InsertAddIn( sal_uInt16& rnSupbook, sal_uInt16& rnExtName, const String& rName )
{
    XclExpSupbookRef xSupbook;
    if( mnAddInSB == 0xFFFF )
    {
        xSupbook.reset( new XclExpSupbook );
        mnAddInSB = Append( xSupbook );
    }
    else
        xSupbook = maSupbookList.GetRecord( mnAddInSB );
    DBG_ASSERT( xSupbook.get(), "XclExpSupbookBuffer::InsertAddin - missing add-in supbook" );
    rnSupbook = mnAddInSB;
    rnExtName = xSupbook->InsertAddIn( rName );
}

void XclExpSupbookBuffer::InsertDde(
        sal_uInt16& rnSupbook, sal_uInt16& rnExtName,
        const String& rApplic, const String& rTopic, const String& rItem )
{
    XclExpSupbookRef xSupbook = GetSupbookDde( rnSupbook, rApplic, rTopic );
    if( !xSupbook.get() )
    {
        xSupbook.reset( new XclExpSupbook( GetRoot(), rApplic, rTopic ) );
        rnSupbook = Append( xSupbook );
    }
    rnExtName = xSupbook->InsertDde( GetRoot(), rItem );
}

void XclExpSupbookBuffer::Save( XclExpStream& rStrm )
{
    maSupbookList.Save( rStrm );
}

XclExpSupbookBuffer::XclExpSupbookRef XclExpSupbookBuffer::GetSupbook( sal_uInt16 nXclTab ) const
{
    DBG_ASSERT( nXclTab < maSBIndexBuffer.size(), "XclSupbookList::GetSupbook - out of range" );
    return maSupbookList.GetRecord( maSBIndexBuffer[ nXclTab ] );
}

XclExpSupbookBuffer::XclExpSupbookRef XclExpSupbookBuffer::GetSupbookUrl( sal_uInt16& rnIndex, const String& rUrl ) const
{
    for( size_t nPos = 0, nSize = maSupbookList.Size(); nPos < nSize; ++nPos )
    {
        XclExpSupbookRef xSupbook = maSupbookList.GetRecord( nPos );
        if( xSupbook->IsUrlLink( rUrl ) )
        {
            rnIndex = ulimit_cast< sal_uInt16 >( nPos );
            return xSupbook;
        }
    }
    return XclExpSupbookRef();
}

XclExpSupbookBuffer::XclExpSupbookRef XclExpSupbookBuffer::GetSupbookDde( sal_uInt16& rnIndex, const String& rApplic, const String& rTopic ) const
{
    for( size_t nPos = 0, nSize = maSupbookList.Size(); nPos < nSize; ++nPos )
    {
        XclExpSupbookRef xSupbook = maSupbookList.GetRecord( nPos );
        if( xSupbook->IsDdeLink( rApplic, rTopic ) )
        {
            rnIndex = ulimit_cast< sal_uInt16 >( nPos );
            return xSupbook;
        }
    }
    return XclExpSupbookRef();
}

sal_uInt16 XclExpSupbookBuffer::Append( XclExpSupbookRef xSupbook )
{
    maSupbookList.AppendRecord( xSupbook );
    return ulimit_cast< sal_uInt16 >( maSupbookList.Size() - 1 );
}

void XclExpSupbookBuffer::AddExt( SCTAB nScTab )
{
    sal_uInt16 nXclTab = GetTabInfo().GetXclTab( nScTab );
    DBG_ASSERT( nXclTab < maSBIndexBuffer.size(), "XclExpSupbookBuffer::AddExt - out of range" );

    // find ext doc name or append new one, save position in maSBIndexBuffer
    const String& rUrl = GetDoc().GetLinkDoc( nScTab );
    DBG_ASSERT( rUrl.Len(), "XclExpSupbookBuffer::AddExt - missing external linked sheet" );
    sal_uInt16 nPos = 0;
    XclExpSupbookRef xSupbook = GetSupbookUrl( nPos, rUrl );
    if( !xSupbook.get() )
    {
        xSupbook.reset( new XclExpSupbook( GetRoot(), rUrl ) );
        nPos = Append( xSupbook );
    }
    maSBIndexBuffer[ nXclTab ] = nPos;

    // append new sheet name, save position in maSBTabBuffer
    maSBTabBuffer[ nXclTab ] = xSupbook->InsertTabName( GetDoc().GetLinkTab( nScTab ) );
}

// Export link manager ========================================================

XclExpLinkManagerImpl::XclExpLinkManagerImpl( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    maSBBuffer( rRoot )
{
}

sal_uInt16 XclExpLinkManagerImpl::FindXti( sal_uInt16 nXclFirst, sal_uInt16 nXclLast )
{
    XclExpXti aXti;
    maSBBuffer.GetSBTabRange( aXti.mnSupbook, aXti.mnSBTabFirst, aXti.mnSBTabLast, nXclFirst, nXclLast );
    return InsertXti( aXti );
}

void XclExpLinkManagerImpl::StoreCellRange( const SingleRefData& rRef1, const SingleRefData& rRef2 )
{
    if( !rRef1.IsTabDeleted() && !rRef2.IsTabDeleted() )
        for( SCTAB nScTab = rRef1.nTab, nLastScTab = rRef2.nTab; nScTab <= nLastScTab; ++nScTab )
            if( GetTabInfo().IsExternalTab( nScTab ) )
                maSBBuffer.StoreCellRange( ScRange(
                    rRef1.nCol, rRef1.nRow, nScTab, rRef2.nCol, rRef2.nRow, nScTab ) );
}

void XclExpLinkManagerImpl::InsertAddIn( sal_uInt16& rnXti, sal_uInt16& rnExtName, const String& rName )
{
    sal_uInt16 nSupbook;
    maSBBuffer.InsertAddIn( nSupbook, rnExtName, rName );
    rnXti = InsertXti( XclExpXti( nSupbook, EXC_EXTSH_EXTERNAL, EXC_EXTSH_EXTERNAL ) );
}

bool XclExpLinkManagerImpl::InsertDde(
        sal_uInt16& rnXti, sal_uInt16& rnExtName,
        const String& rApplic, const String& rTopic, const String& rItem )
{
    sal_uInt16 nSupbook;
    maSBBuffer.InsertDde( nSupbook, rnExtName, rApplic, rTopic, rItem );
    if( !rnExtName )
        return false;
    rnXti = InsertXti( XclExpXti( nSupbook, EXC_EXTSH_EXTERNAL, EXC_EXTSH_EXTERNAL ) );
    return true;
}

sal_uInt16 XclExpLinkManagerImpl::InsertXti( const XclExpXti& rXti )
{
    for( XclExpXtiVec::const_iterator aIt = maXtiVec.begin(), aEnd = maXtiVec.end(); aIt != aEnd; ++aIt )
        if( *aIt == rXti )
            return ulimit_cast< sal_uInt16 >( aIt - maXtiVec.begin() );
    maXtiVec.push_back( rXti );
    return ulimit_cast< sal_uInt16 >( maXtiVec.size() - 1 );
}

void XclExpLinkManagerImpl::Save( XclExpStream& rStrm )
{
    DBG_ASSERT_BIFF( GetBiff() >= xlBiff8 );

    if( !maXtiVec.empty() )
    {
        // SUPBOOKs, XCTs, CRNs, EXTERNNAMEs
        maSBBuffer.Save( rStrm );

        // EXTERNSHEET
        sal_uInt16 nCount = ulimit_cast< sal_uInt16 >( maXtiVec.size() );
        rStrm.StartRecord( EXC_ID_EXTERNSHEET, 2 + 6 * nCount );
        rStrm << nCount;
        rStrm.SetSliceSize( 6 );
        for( XclExpXtiVec::const_iterator aIt = maXtiVec.begin(), aEnd = maXtiVec.end(); aIt != aEnd; ++aIt )
            aIt->Save( rStrm );
        rStrm.EndRecord();
    }
}

// ============================================================================

XclExpLinkManager::XclExpLinkManager( const XclExpRoot& rRoot ) :
    mpImpl( new XclExpLinkManagerImpl( rRoot ) )
{
}

XclExpLinkManager::~XclExpLinkManager()
{
}

sal_uInt16 XclExpLinkManager::FindXti( sal_uInt16 nXclFirst, sal_uInt16 nXclLast )
{
    return mpImpl->FindXti( nXclFirst, nXclLast );
}

const XclExpString* XclExpLinkManager::GetUrl( sal_uInt16 nXclTab ) const
{
    return mpImpl->GetUrl( nXclTab );
}

const XclExpString* XclExpLinkManager::GetTabName( sal_uInt16 nXclTab ) const
{
    return mpImpl->GetTabName( nXclTab );
}

void XclExpLinkManager::StoreCell( const SingleRefData& rRef )
{
    mpImpl->StoreCellRange( rRef, rRef );
}

void XclExpLinkManager::StoreCellRange( const SingleRefData& rRef1, const SingleRefData& rRef2 )
{
    mpImpl->StoreCellRange( rRef1, rRef2 );
}

void XclExpLinkManager::InsertAddIn( sal_uInt16& rnXti, sal_uInt16& rnExtName, const String& rName )
{
    mpImpl->InsertAddIn( rnXti, rnExtName, rName );
}

bool XclExpLinkManager::InsertDde(
        sal_uInt16& rnXti, sal_uInt16& rnExtName,
        const String& rApplic, const String& rTopic, const String& rItem )
{
    return mpImpl->InsertDde( rnXti, rnExtName, rApplic, rTopic, rItem );
}

void XclExpLinkManager::Save( XclExpStream& rStrm )
{
    mpImpl->Save( rStrm );
}

// ============================================================================

