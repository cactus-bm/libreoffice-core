/*************************************************************************
 *
 *  $RCSfile: xecontent.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-28 17:21:19 $
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

#ifndef SC_XECONTENT_HXX
#define SC_XECONTENT_HXX

#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"
#endif

#ifndef SC_XLCONTENT_HXX
#include "xlcontent.hxx"
#endif
#ifndef SC_XEROOT_HXX
#include "xeroot.hxx"
#endif
#ifndef SC_XERECORD_HXX
#include "xerecord.hxx"
#endif
#ifndef SC_XESTRING_HXX
#include "xestring.hxx"
#endif
#ifndef SC_XEFORMULA_HXX
#include "xeformula.hxx"
#endif

/* ============================================================================
Classes to export the big Excel document contents (related to several cells or
globals for the sheet or document).
- Shared string table
- Merged cells
- Hyperlinks
- Label ranges
- Conditional formatting
- Data validation
- Web Queries
============================================================================ */

// Shared string table ========================================================

class XclExpSstImpl;

/** Provides export of the SST (shared string table) record.
    @descr  Contains all strings in the document and writes the SST. */
class XclExpSst : public XclExpRecordBase
{
public:
    explicit            XclExpSst();
    virtual             ~XclExpSst();

    /** Inserts a new string into the table.
        @return  The index of the string in the SST, used in other records. */
    sal_uInt32          Insert( XclExpStringRef xString );

    /** Writes the complete SST and EXTSST records. */
    virtual void        Save( XclExpStream& rStrm );

private:
    typedef ::std::auto_ptr< XclExpSstImpl > XclExpSstImplPtr;
    XclExpSstImplPtr    mxImpl;
};

// Merged cells ===============================================================

/** Represents a MERGEDCELLS record containing all merged cell ranges in a sheet. */
class XclExpMergedcells : public XclExpRecordBase, protected XclExpRoot
{
public:
    explicit            XclExpMergedcells( const XclExpRoot& rRoot );

    /** Appends a new range to the list of merged cell ranges. */
    void                AppendRange( const ScRange& rRange, sal_uInt32 nBaseXFId );
    /** Returns the XF identifier of the top-left cell in a merged range. */
    sal_uInt32          GetBaseXFId( const ScAddress& rPos ) const;

    /** Writes the record, if it contains at least one merged cell range. */
    virtual void        Save( XclExpStream& rStrm );

private:
    ScRangeList         maMergedRanges;     /// All merged cell ranges of the sheet.
    ScfUInt32Vec        maBaseXFIds;        /// The XF identifiers of the top-left cells.
};

// Hyperlinks =================================================================

class SvxURLField;
class INetURLObject;

/** Provides export of hyperlink data. */
class XclExpHyperlink : public XclExpRecord
{
public:
    /** Constructs the HLINK record from an URL text field. */
    explicit            XclExpHyperlink( const XclExpRoot& rRoot,
                            const SvxURLField& rUrlField, const ScAddress& rScPos );
    virtual             ~XclExpHyperlink();

    /** Returns the cell representation text or 0, if not available. */
    inline const String* GetRepr() const { return mxRepr.get(); }

private:
    /** Builds file name from the passed file URL. Tries to convert to relative file name.
        @param rnLevel  (out-param) The parent directory level.
        @param rbRel  (out-param) true = path is relative. */
    String              BuildFileName(
                            sal_uInt16& rnLevel, bool& rbRel,
                            const String& rUrl, const XclExpRoot& rRoot ) const;

    /** Writes the body of the HLINK record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    typedef ::std::auto_ptr< String >   StringPtr;
    typedef ::std::auto_ptr< SvStream > SvStreamPtr;

    ScAddress           maScPos;            /// Position of the hyperlink.
    StringPtr           mxRepr;             /// Cell representation text.
    SvStreamPtr         mxVarData;          /// Buffer stream with variable data.
    sal_uInt32          mnFlags;            /// Option flags.
};

typedef XclExpRecordList< XclExpHyperlink > XclExpHyperlinkList;

// Label ranges ===============================================================

/** Provides export of the row/column label range list of a sheet. */
class XclExpLabelranges : public XclExpRecord
{
public:
    /** Fills the cell range lists with all ranges of the current sheet. */
    explicit            XclExpLabelranges( const XclExpRoot& rRoot );

    /** Writes the LABELRANGES record if it contains at least one range. */
    virtual void        Save( XclExpStream& rStrm );

private:
    /** Fills the specified range list with all label headers of the current sheet.
        @param rRanges  The cell range list to fill.
        @param xLabelRangesRef  The core range list with all ranges.
        @param nScTab  The current Calc sheet index. */
    void                FillRangeList( ScRangeList& rRanges, ScRangePairListRef xLabelRangesRef, SCTAB nScTab );

    /** Writes the body of the LABELRANGES record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    ScRangeList         maRowRanges;    /// Cell range list for row labels.
    ScRangeList         maColRanges;    /// Cell range list for column labels.
};

// Conditional formatting =====================================================

class ScCondFormatEntry;
class XclExpCFImpl;

/** Represents a CF record that contains one condition of a conditional format. */
class XclExpCF : public XclExpRecord, protected XclExpRoot
{
public:
    explicit            XclExpCF( const XclExpRoot& rRoot, const ScCondFormatEntry& rFormatEntry );
    virtual             ~XclExpCF();

private:
    /** Writes the body of the CF record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    typedef ::std::auto_ptr< XclExpCFImpl > XclExpCFImplPtr;
    XclExpCFImplPtr     mxImpl;
};

// ----------------------------------------------------------------------------

class ScConditionalFormat;

/** Represents a CONDFMT record that contains all conditions of a conditional format.
    @descr  Contains the conditions which are stored in CF records. */
class XclExpCondfmt : public XclExpRecord, protected XclExpRoot
{
public:
    explicit            XclExpCondfmt( const XclExpRoot& rRoot, const ScConditionalFormat& rCondFormat );
    virtual             ~XclExpCondfmt();

    /** Returns true, if this conditional format contains at least one cell range and CF record. */
    bool                IsValid() const;

    /** Writes the CONDFMT record with following CF records, if there is valid data. */
    virtual void        Save( XclExpStream& rStrm );

private:
    /** Writes the body of the CONDFMT record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    typedef XclExpRecordList< XclExpCF > XclExpCFList;

    XclExpCFList        maCFList;       /// List of CF records.
    ScRangeList         maRanges;       /// Cell ranges for this conditional format.
};

// ----------------------------------------------------------------------------

/** Contains all conditional formats of a specific sheet. */
class XclExpCondFormatBuffer : public XclExpRecordBase, protected XclExpRoot
{
public:
    /** Constructs CONDFMT and CF records containing the conditional formats of the current sheet. */
    explicit            XclExpCondFormatBuffer( const XclExpRoot& rRoot );

    /** Writes all contained CONDFMT records with their CF records. */
    virtual void        Save( XclExpStream& rStrm );

private:
    typedef XclExpRecordList< XclExpCondfmt > XclExpCondfmtList;
    XclExpCondfmtList   maCondfmtList;  /// List of CONDFMT records.
};

// Data Validation ============================================================

class ScValidationData;
class XclExpTokenArray;

/** Provides export of the data of a DV record.
    @descr  This record contains the settings for a data validation. In detail
    this is a pointer to the core validation data and a cell range list with all
    affected cells. The handle index is used to optimize list search algorithm. */
class XclExpDV : public XclExpRecord, protected XclExpRoot
{
public:
    explicit            XclExpDV( const XclExpRoot& rRoot, ULONG nScHandle );
    virtual             ~XclExpDV();

    /** Returns the core handle of the validation data. */
    inline ULONG        GetScHandle() const { return mnScHandle; }

    /** Inserts a new cell range into the cell range list. */
    void                InsertCellRange( const ScRange& rPos );
    /** Checks the record contents and crops the range list.
        @return  false = Do not write this record. */
    bool                CheckWriteRecord();

private:
    /** Writes the body of the DV record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    ScRangeList         maRanges;       /// A list with all affected cells.
    XclExpString        maPromptTitle;  /// The prompt title.
    XclExpString        maPromptText;   /// The prompt text.
    XclExpString        maErrorTitle;   /// The error title.
    XclExpString        maErrorText;    /// The error text.
    XclExpStringRef     mxString1;      /// String for first condition formula.
    XclExpTokenArrayRef mxTokArr1;      /// Formula for first condition.
    XclExpTokenArrayRef mxTokArr2;      /// Formula for second condition.
    sal_uInt32          mnFlags;        /// Miscellaneous flags.
    ULONG               mnScHandle;     /// The core handle for quick list search.
};

// ----------------------------------------------------------------------------

/** This class contains the DV record list following the DVAL record. */
class XclExpDval : public XclExpRecord, protected XclExpRoot
{
public:
    explicit            XclExpDval( const XclExpRoot& rRoot );
    virtual             ~XclExpDval();

    /** Inserts the cell range into the range list of the DV record with the specified handle. */
    void                InsertCellRange( const ScRange& rRange, ULONG nScHandle );

    /** Writes the DVAL record and the DV record list. */
    virtual void        Save( XclExpStream& rStrm );

private:
    /** Searches for or creates a XclExpDV record object with the specified handle. */
    XclExpDV&           SearchOrCreateDv( ULONG nScHandle );

    /** Writes the body of the DVAL record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    typedef XclExpRecordList< XclExpDV >    XclExpDVList;
    typedef XclExpDVList::RecordRefType     XclExpDVRef;

    XclExpDVList        maDVList;       /// List of DV records
    XclExpDVRef         mxLastFoundDV;  /// For search optimization.
};

// Web Queries ================================================================

/** Contains all records for a web query (linked tables in an HTML document).
    @descr  mode 1 (entire document): mpQryTables==0, mbEntireDoc==true;
    mode 2 (all tables): mpQryTables==0, mbEntireDoc==false;
    mode 3 (custom range list): mpQryTables!=0, mbEntireDoc==false. */
class XclExpWebQuery : public XclExpRecordBase
{
public:
    /** Constructs a web query record container with settings from Calc. */
    explicit            XclExpWebQuery(
                            const String& rRangeName,
                            const String& rUrl,
                            const String& rSource,
                            sal_Int32 nRefrSecs );
    virtual             ~XclExpWebQuery();

    /** Writes all needed records for this web query. */
    virtual void        Save( XclExpStream& rStrm );

private:
    XclExpString        maDestRange;    /// Destination range.
    XclExpString        maUrl;          /// Source document URL.
    XclExpStringRef     mxQryTables;    /// List of source range names.
    sal_Int16           mnRefresh;      /// Refresh time in minutes.
    bool                mbEntireDoc;    /// true = entire document.
};

// ----------------------------------------------------------------------------

/** Contains all web query records for this document. */
class XclExpWebQueryBuffer : public XclExpRecordList< XclExpWebQuery >
{
public:
    explicit            XclExpWebQueryBuffer( const XclExpRoot& rRoot );
};

// ============================================================================

#endif

