/*************************************************************************
 *
 *  $RCSfile: xecontent.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2004-03-02 09:36:16 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

// ============================================================================

#ifndef SC_XECONTENT_HXX
#include "xecontent.hxx"
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XAREALINKS_HPP_
#include <com/sun/star/sheet/XAreaLinks.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XAREALINK_HPP_
#include <com/sun/star/sheet/XAreaLink.hpp>
#endif

#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
#ifndef _EEITEM_HXX
#include <svx/eeitem.hxx>
#endif
#define ITEMID_FIELD EE_FEATURE_FIELD
#ifndef _SVX_FLDITEM_HXX
#include <svx/flditem.hxx>
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_VALIDAT_HXX
#include "validat.hxx"
#endif
#ifndef SC_UNONAMES_HXX
#include "unonames.hxx"
#endif
#ifndef SC_CONVUNO_HXX
#include "convuno.hxx"
#endif
#ifndef SC_RANGENAM_HXX
#include "rangenam.hxx"
#endif
#ifndef SC_STLPOOL_HXX
#include "stlpool.hxx"
#endif
#ifndef SC_SCPATATR_HXX
#include "patattr.hxx"
#endif

#ifndef SC_FAPIHELPER_HXX
#include "fapihelper.hxx"
#endif
#ifndef SC_XLFORMULA_HXX
#include "xlformula.hxx"
#endif
#ifndef SC_XESTYLE_HXX
#include "xestyle.hxx"
#endif

using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::container::XIndexAccess;
using ::com::sun::star::frame::XModel;
using ::com::sun::star::table::CellRangeAddress;
using ::com::sun::star::sheet::XAreaLinks;
using ::com::sun::star::sheet::XAreaLink;

#include "excupn.hxx"
#include "excrecds.hxx"


// Shared string table ========================================================

// 1 = SST hash table statistics prompt
#define EXC_INCL_SST_STATISTICS 0


// ----------------------------------------------------------------------------

/** A single string entry in the hash table. */
struct XclExpHashEntry
{
    const XclExpString*         mpString;       /// Pointer to the string (no ownership).
    sal_uInt32                  mnSstIndex;     /// The SST index of this string.
    inline explicit             XclExpHashEntry( const XclExpString* pString = 0, sal_uInt32 nSstIndex = 0 ) :
                                    mpString( pString ), mnSstIndex( nSstIndex ) {}
};

/** Function object for strict weak ordering. */
struct XclExpHashEntrySWO
{
    inline bool                 operator()( const XclExpHashEntry& rLeft, const XclExpHashEntry& rRight ) const
                                    { return *rLeft.mpString < *rRight.mpString; }
};


// ----------------------------------------------------------------------------

/** Implementation of the SST export.
    @descr  Stores all passed strings in a hash table and prevents repeated
    insertion of equal strings. */
class XclExpSst_Impl
{
public:
    explicit                    XclExpSst_Impl();

    /** Inserts the passed string, if not already inserted, and returns the unique SST index. */
    sal_uInt32                  Insert( XclExpStringPtr pString );

    /** Writes the complete SST and EXTSST records. */
    void                        Save( XclExpStream& rStrm );

private:
    typedef ::std::list< XclExpStringPtr >      XclExpStringList;
    typedef ::std::vector< XclExpHashEntry >    XclExpHashVec;
    typedef ::std::vector< XclExpHashVec >      XclExpHashTab;

    XclExpStringList            maStringList;   /// List of unique strings (in SST ID order).
    XclExpHashTab               maHashTab;      /// Hashed table that manages string pointers.
    sal_uInt32                  mnTotal;        /// Total count of strings (including doubles).
    sal_uInt32                  mnSize;         /// Size of the SST (count of unique strings).
};


// ----------------------------------------------------------------------------

const sal_uInt32 EXC_SST_HASHTABLE_SIZE = 2048;

XclExpSst_Impl::XclExpSst_Impl() :
    maHashTab( EXC_SST_HASHTABLE_SIZE ),
    mnTotal( 0 ),
    mnSize( 0 )
{
}

sal_uInt32 XclExpSst_Impl::Insert( XclExpStringPtr pString )
{
    DBG_ASSERT( pString.get(), "XclExpSst_Impl::Insert - empty pointer not allowed" );
    if( !pString.get() )
        pString.reset( new XclExpString );

    ++mnTotal;
    sal_uInt32 nSstIndex = 0;

    // calculate hash value in range [0,EXC_SST_HASHTABLE_SIZE)
    sal_uInt16 nHash = pString->GetHash();
    (nHash ^= (nHash / EXC_SST_HASHTABLE_SIZE)) %= EXC_SST_HASHTABLE_SIZE;

    XclExpHashVec& rVec = maHashTab[ nHash ];
    XclExpHashEntry aEntry( pString.get(), mnSize );
    XclExpHashVec::iterator aIt = ::std::lower_bound( rVec.begin(), rVec.end(), aEntry, XclExpHashEntrySWO() );
    if( (aIt == rVec.end()) || (*aIt->mpString != *pString) )
    {
        nSstIndex = mnSize;
        maStringList.push_back( pString );
        rVec.insert( aIt, aEntry );
        ++mnSize;
    }
    else
    {
        nSstIndex = aIt->mnSstIndex;
    }

    return nSstIndex;
}

void XclExpSst_Impl::Save( XclExpStream& rStrm )
{
    if( maStringList.empty() )
        return;

#if (OSL_DEBUG_LEVEL > 1) && EXC_INCL_SST_STATISTICS
    { // own scope for the statistics
#define APPENDINT( value ) Append( ByteString::CreateFromInt32( value ) )
        ScfUInt32Vec aVec;
        sal_uInt32 nPerBucket = mnSize / EXC_SST_HASHTABLE_SIZE + 1, nEff = 0;
        for( XclExpHashTab::const_iterator aTIt = maHashTab.begin(), aTEnd = maHashTab.end(); aTIt != aTEnd; ++aTIt )
        {
            sal_uInt32 nSize = aTIt->size();
            if( nSize >= aVec.size() ) aVec.resize( nSize + 1, 0 );
            ++aVec[ nSize ];
            if( nSize > nPerBucket ) nEff += nSize - nPerBucket;
        }
        ByteString aStr( "SST HASHING STATISTICS\n\n" );
        aStr.Append( "Total count:\t" ).APPENDINT( mnTotal ).Append( " strings\n" );
        aStr.Append( "Reduced to:\t" ).APPENDINT( mnSize ).Append( " strings (" );
        aStr.APPENDINT( 100 * mnSize / mnTotal ).Append( "%)\n" );
        aStr.Append( "Effectivity:\t\t" ).APPENDINT( 100 - 100 * nEff / mnSize );
        aStr.Append( "% (best: " ).APPENDINT( nPerBucket ).Append( " strings per bucket)\n" );
        aStr.Append( "\t\tCount of buckets\nBucket size\ttotal\tmax\tTotal strings\n" );
        for( sal_uInt32 nIx = 0, nSize = aVec.size(), nInc = 1; nIx < nSize; nIx += nInc )
        {
            if( (nIx == 10) || (nIx == 100) || (nIx == 1000) ) nInc = nIx;
            sal_uInt32 nMaxIx = ::std::min( nIx + nInc, nSize ), nCount = 0, nMaxCount = 0, nStrings = 0;
            for( sal_uInt32 nSubIx = nIx; nSubIx < nMaxIx; ++nSubIx )
            {
                nCount += aVec[ nSubIx ];
                if( aVec[ nSubIx ] > nMaxCount ) nMaxCount = aVec[ nSubIx ];
                nStrings += nSubIx * aVec[ nSubIx ];
            }
            if( nMaxCount )
            {
                aStr.APPENDINT( nIx );
                if( nMaxIx - nIx > 1 ) aStr.Append( '-' ).APPENDINT( nMaxIx - 1 );
                aStr.Append( "\t\t" ).APPENDINT( nCount ).Append( '\t' ).APPENDINT( nMaxCount );
                aStr.Append( '\t' ).APPENDINT( nStrings ).Append( '\n' );
            }
        }
        DBG_ERRORFILE( aStr.GetBuffer() );
#undef APPENDINT
    }
#endif

    SvMemoryStream aExtSst( 8192 );

    sal_uInt32 nBucket = mnSize;
    while( nBucket > 0x0100 )
        nBucket /= 2;

    sal_uInt16 nPerBucket = static_cast< sal_uInt16 >( ::std::max( 8UL, nBucket ) );
    sal_uInt16 nBucketIndex = 0;

    // *** write the SST record ***

    rStrm.StartRecord( EXC_ID_SST, 8 );

    rStrm << mnTotal << mnSize;
    for( XclExpStringList::const_iterator aIt = maStringList.begin(), aEnd = maStringList.end(); aIt != aEnd; ++aIt )
    {
        if( !nBucketIndex )
        {
            // write bucket info before string to get correct record position
            sal_uInt32 nStrmPos = rStrm.GetStreamPos();
            sal_uInt16 nRecPos = static_cast< sal_uInt16 >( rStrm.GetRecPos() + 4 );
            aExtSst << nStrmPos             // stream position
                    << nRecPos              // position from start of SST or CONTINUE
                    << sal_uInt16( 0 );     // reserved
        }

        rStrm << **aIt;

        if( ++nBucketIndex == nPerBucket )
            nBucketIndex = 0;
    }

    rStrm.EndRecord();

    // *** write the EXTSST record ***

    rStrm.StartRecord( EXC_ID_EXTSST, 0 );

    rStrm << nPerBucket;
    rStrm.SetSliceSize( 8 );    // size of one bucket info
    aExtSst.Seek( STREAM_SEEK_TO_BEGIN );
    rStrm.CopyFromStream( aExtSst );

    rStrm.EndRecord();
}


// ----------------------------------------------------------------------------

XclExpSst::XclExpSst() :
    mpImpl( new XclExpSst_Impl )
{
}

XclExpSst::~XclExpSst()
{
}

sal_uInt32 XclExpSst::Insert( XclExpStringPtr pString )
{
    return mpImpl->Insert( pString );
}

void XclExpSst::Save( XclExpStream& rStrm )
{
    mpImpl->Save( rStrm );
}


// Hyperlinks =================================================================

XclExpHyperlink::XclExpHyperlink( const XclExpRoot& rRoot, const SvxURLField& rUrlField ) :
    XclExpRecord( EXC_ID_HLINK ),
    mpVarData( new SvMemoryStream ),
    mnFlags( 0 )
{
    const String& rUrl = rUrlField.GetURL();
    const String& rRepr = rUrlField.GetRepresentation();
    INetURLObject aUrlObj( rUrl );
    const INetProtocol eProtocol = aUrlObj.GetProtocol();
    bool bWithRepr = rRepr.Len() > 0;
    XclExpStream aXclStrm( *mpVarData, rRoot );         // using in raw write mode.
    XclExpStringPtr pTextMark;

    // description
    if( bWithRepr )
    {
        XclExpString aDescr( rRepr, EXC_STR_FORCEUNICODE, 255 );
        aXclStrm << sal_uInt32( aDescr.Len() + 1 );     // string length + 1 trailing zero word
        aDescr.WriteBuffer( aXclStrm );                 // NO flags
        aXclStrm << sal_uInt16( 0 );

        mnFlags |= EXC_HLINK_DESCR;
        mpRepr.reset( new String( rRepr ) );
    }

    // file link or URL
    if( eProtocol == INET_PROT_FILE )
    {
        sal_uInt16 nLevel;
        bool bRel;
        String aFileName( BuildFileName( nLevel, bRel, rUrl, rRoot ) );

        if( !bRel )
            mnFlags |= EXC_HLINK_ABS;
        mnFlags |= EXC_HLINK_BODY;

        ByteString aAsciiLink( aFileName, rRoot.GetCharSet() );
        XclExpString aLink( aFileName, EXC_STR_FORCEUNICODE, 255 );
        aXclStrm    << XclTools::maGuidFileMoniker
                    << nLevel
                    << sal_uInt32( aAsciiLink.Len() + 1 );      // string length + 1 trailing zero byte
        aXclStrm.Write( aAsciiLink.GetBuffer(), aAsciiLink.Len() );
        aXclStrm    << sal_uInt8( 0 )
                    << sal_uInt32( 0xDEADFFFF );
        aXclStrm.WriteZeroBytes( 20 );
        aXclStrm    << sal_uInt32( aLink.GetBufferSize() + 6 )
                    << sal_uInt32( aLink.GetBufferSize() )      // byte count, not string length
                    << sal_uInt16( 0x0003 );
        aLink.WriteBuffer( aXclStrm );                          // NO flags

        if( !mpRepr.get() )
            mpRepr.reset( new String( aFileName ) );
    }
    else if( eProtocol != INET_PROT_NOT_VALID )
    {
        XclExpString aUrl( aUrlObj.GetURLNoMark(), EXC_STR_FORCEUNICODE, 255 );
        aXclStrm    << XclTools::maGuidUrlMoniker
                    << sal_uInt32( aUrl.GetBufferSize() + 2 );  // byte count + 1 trailing zero word
        aUrl.WriteBuffer( aXclStrm );                           // NO flags
        aXclStrm    << sal_uInt16( 0 );

        mnFlags |= EXC_HLINK_BODY | EXC_HLINK_ABS;
        if( !mpRepr.get() )
            mpRepr.reset( new String( rUrl ) );
    }
    else if( rUrl.GetChar( 0 ) == '#' )     // hack for #89066#
    {
        String aTextMark( rUrl.Copy( 1 ) );
        aTextMark.SearchAndReplace( '.', '!' );
        pTextMark.reset( new XclExpString( aTextMark, EXC_STR_FORCEUNICODE, 255 ) );
    }

    // text mark
    if( !pTextMark.get() && aUrlObj.HasMark() )
        pTextMark.reset( new XclExpString( aUrlObj.GetMark(), EXC_STR_FORCEUNICODE, 255 ) );

    if( pTextMark.get() )
    {
        aXclStrm    << sal_uInt32( pTextMark->Len() + 1 );  // string length + 1 trailing zero word
        pTextMark->WriteBuffer( aXclStrm );                 // NO flags
        aXclStrm    << sal_uInt16( 0 );

        mnFlags |= EXC_HLINK_MARK;
    }

    SetRecSize( 32 + mpVarData->Tell() );
}

XclExpHyperlink::~XclExpHyperlink()
{
}

String XclExpHyperlink::BuildFileName(
        sal_uInt16& rnLevel, bool& rbRel, const String& rUrl, const XclExpRoot& rRoot ) const
{
    String aDosName( INetURLObject( rUrl ).getFSysPath( INetURLObject::FSYS_DOS ) );
    rnLevel = 0;
    rbRel = rRoot.IsRelUrl();

    if( rbRel )
    {
        // try to convert to relative file name
        String aTmpName( aDosName );
        aDosName = INetURLObject::GetRelURL( rRoot.GetBasePath(), rUrl,
            INetURLObject::WAS_ENCODED, INetURLObject::DECODE_WITH_CHARSET );

        if( aDosName.SearchAscii( INET_FILE_SCHEME ) == 0 )
        {
            // not converted to rel -> back to old, return absolute flag
            aDosName = aTmpName;
            rbRel = false;
        }
        else if( aDosName.SearchAscii( "./" ) == 0 )
        {
            aDosName.Erase( 0, 2 );
        }
        else
        {
            while( aDosName.SearchAndReplaceAscii( "../", EMPTY_STRING ) == 0 )
                ++rnLevel;
        }
    }
    return aDosName;
}

void XclExpHyperlink::WriteBody( XclExpStream& rStrm )
{
    sal_uInt16 nCol = static_cast< sal_uInt16 >( maPos.Col() );
    sal_uInt16 nRow = static_cast< sal_uInt16 >( maPos.Row() );
    mpVarData->Seek( STREAM_SEEK_TO_BEGIN );

    rStrm   << nRow << nRow << nCol << nCol
            << XclTools::maGuidStdLink
            << sal_uInt32( 2 )
            << mnFlags;
    rStrm.CopyFromStream( *mpVarData );
}


// Label ranges ===============================================================

XclExpLabelranges::XclExpLabelranges( const XclExpRoot& rRoot ) :
    XclExpRecord( EXC_ID_LABELRANGES )
{
    USHORT nScTab = rRoot.GetCurrScTab();

    // row label ranges
    FillRangeList( maRowRanges, rRoot.GetDoc().GetRowNameRangesRef(), nScTab );
    // row labels only over 1 column (restriction of Excel97/2000/XP)
    for( ScRange* pRange = maRowRanges.First(); pRange; pRange = maRowRanges.Next() )
        if( pRange->aStart.Col() != pRange->aEnd.Col() )
            pRange->aEnd.SetCol( pRange->aStart.Col() );
    rRoot.CheckCellRangeList( maRowRanges );

    // col label ranges
    FillRangeList( maColRanges, rRoot.GetDoc().GetColNameRangesRef(), nScTab );
    rRoot.CheckCellRangeList( maColRanges );

    SetRecSize( 4 + 8 * (maRowRanges.Count() + maColRanges.Count()) );
}

void XclExpLabelranges::FillRangeList( ScRangeList& rRanges, ScRangePairListRef xLabelRangesRef, USHORT nScTab )
{
    for( const ScRangePair* pRangePair = xLabelRangesRef->First(); pRangePair; pRangePair = xLabelRangesRef->Next() )
    {
        const ScRange& rRange = pRangePair->GetRange( 0 );
        if( rRange.aStart.Tab() == nScTab )
            rRanges.Append( rRange );
    }
}

void XclExpLabelranges::Save( XclExpStream& rStrm )
{
    if( maRowRanges.Count() || maColRanges.Count() )
        XclExpRecord::Save( rStrm );
}

void XclExpLabelranges::WriteBody( XclExpStream& rStrm )
{
    rStrm << maRowRanges << maColRanges;
}


// Conditional formatting  ====================================================

/** Represents a CF record that contains one condition of a conditional format. */
class XclExpCF_Impl : protected XclExpRoot
{
public:
    explicit                    XclExpCF_Impl( const XclExpRoot& rRoot, const ScCondFormatEntry& rFormatEntry );

    /** Writes the body of the CF record. */
    void                        WriteBody( XclExpStream& rStrm );

private:
    const ScCondFormatEntry&    mrFormatEntry;  /// Calc conditional format entry.
    XclFontData                 maFontData;     /// Font formatting attributes.
    XclExpCellBorder            maBorder;       /// Border formatting attributes.
    XclExpCellArea              maArea;         /// Pattern formatting attributes.
    sal_uInt32                  mnFontColorId;  /// Font color ID.
    bool                        mbHeightUsed;   /// true = Font height used.
    bool                        mbWeightUsed;   /// true = Font weight used.
    bool                        mbColorUsed;    /// true = Font color used.
    bool                        mbUnderlUsed;   /// true = Font underline type used.
    bool                        mbItalicUsed;   /// true = Font posture used.
    bool                        mbStrikeUsed;   /// true = Font strikeout used.
    bool                        mbFontUsed;     /// true = Any font attribute used.
    bool                        mbBorderUsed;   /// true = Border attribute used.
    bool                        mbPattUsed;     /// true = Pattern attribute used.
};


// ----------------------------------------------------------------------------

XclExpCF_Impl::XclExpCF_Impl( const XclExpRoot& rRoot, const ScCondFormatEntry& rFormatEntry ) :
    XclExpRoot( rRoot ),
    mrFormatEntry( rFormatEntry ),
    mnFontColorId( 0 ),
    mbFontUsed( false ),
    mbHeightUsed( false ),
    mbWeightUsed( false ),
    mbColorUsed( false ),
    mbUnderlUsed( false ),
    mbItalicUsed( false ),
    mbStrikeUsed( false ),
    mbBorderUsed( false ),
    mbPattUsed( false )
{
    /*  Get formatting attributes here, and not in WriteBody(). This is needed to
        correctly insert all colors into the palette. */

    if( SfxStyleSheetBase* pStyleSheet = GetDoc().GetStyleSheetPool()->Find( mrFormatEntry.GetStyle(), SFX_STYLE_FAMILY_PARA ) )
    {
        const SfxItemSet& rItemSet = pStyleSheet->GetItemSet();

        // font
        mbHeightUsed = ScfTools::CheckItem( rItemSet, ATTR_FONT_HEIGHT,     true );
        mbWeightUsed = ScfTools::CheckItem( rItemSet, ATTR_FONT_WEIGHT,     true );
        mbColorUsed  = ScfTools::CheckItem( rItemSet, ATTR_FONT_COLOR,      true );
        mbUnderlUsed = ScfTools::CheckItem( rItemSet, ATTR_FONT_UNDERLINE,  true );
        mbItalicUsed = ScfTools::CheckItem( rItemSet, ATTR_FONT_POSTURE,    true );
        mbStrikeUsed = ScfTools::CheckItem( rItemSet, ATTR_FONT_CROSSEDOUT, true );
        mbFontUsed = mbHeightUsed || mbWeightUsed || mbColorUsed || mbUnderlUsed || mbItalicUsed || mbStrikeUsed;
        if( mbFontUsed )
        {
            Font aFont;
            ScPatternAttr::GetFont( aFont, rItemSet, SC_AUTOCOL_RAW );
            maFontData.FillFromFont( aFont );
            mnFontColorId = GetPalette().InsertColor( aFont.GetColor(), xlColorCellText );
        }

        // border
        mbBorderUsed = ScfTools::CheckItem( rItemSet, ATTR_BORDER, true );
        if( mbBorderUsed )
            maBorder.FillFromItemSet( rItemSet, GetPalette(), GetBiff() );

        // pattern
        mbPattUsed = ScfTools::CheckItem( rItemSet, ATTR_BACKGROUND, true );
        if( mbPattUsed )
            maArea.FillFromItemSet( rItemSet, GetPalette(), GetBiff() );
    }
}

void XclExpCF_Impl::WriteBody( XclExpStream& rStrm )
{
    // *** mode and comparison operator ***

    sal_uInt8 nType = EXC_CF_TYPE_CELL;
    sal_uInt8 nOperator = EXC_CF_CMP_NONE;
    bool bFmla2 = false;

    switch( mrFormatEntry.GetOperation() )
    {
        case SC_COND_NONE:          nType = EXC_CF_TYPE_NONE;                               break;
        case SC_COND_BETWEEN:       nOperator = EXC_CF_CMP_BETWEEN;         bFmla2 = true;  break;
        case SC_COND_NOTBETWEEN:    nOperator = EXC_CF_CMP_NOT_BETWEEN;     bFmla2 = true;  break;
        case SC_COND_EQUAL:         nOperator = EXC_CF_CMP_EQUAL;                           break;
        case SC_COND_NOTEQUAL:      nOperator = EXC_CF_CMP_NOT_EQUAL;                       break;
        case SC_COND_GREATER:       nOperator = EXC_CF_CMP_GREATER;                         break;
        case SC_COND_LESS:          nOperator = EXC_CF_CMP_LESS;                            break;
        case SC_COND_EQGREATER:     nOperator = EXC_CF_CMP_GREATER_EQUAL;                   break;
        case SC_COND_EQLESS:        nOperator = EXC_CF_CMP_LESS_EQUAL;                      break;
        case SC_COND_DIRECT:        nType = EXC_CF_TYPE_FMLA;                               break;
        default:                    nType = EXC_CF_TYPE_NONE;
            DBG_ERRORFILE( "XclExpCF::WriteBody - unknown condition type" );
    }

    rStrm << nType << nOperator;

    // *** formulas ***

    ::std::auto_ptr< ScTokenArray > pScTokArr( mrFormatEntry.CreateTokenArry( 0 ) );
    EC_Codetype eDummy;
    ::std::auto_ptr< ExcUPN > pXclTokArr1( new ExcUPN( mpRD, *pScTokArr, eDummy, NULL, TRUE ) );
    sal_uInt16 nFmlaSize1 = pXclTokArr1->GetLen();

    ::std::auto_ptr< ExcUPN > pXclTokArr2;
    sal_uInt16 nFmlaSize2 = 0;
    if( bFmla2 )
    {
        pScTokArr.reset( mrFormatEntry.CreateTokenArry( 1 ) );
        pXclTokArr2.reset( new ExcUPN( mpRD, *pScTokArr, eDummy, NULL, TRUE ) );
        nFmlaSize2 = pXclTokArr2->GetLen();
    }

    rStrm << nFmlaSize1 << nFmlaSize2;

    // *** formatting blocks ***

    if( mbFontUsed || mbBorderUsed || mbPattUsed )
    {
        sal_uInt32 nFlags = EXC_CF_ALLDEFAULT;

        ::set_flag( nFlags, EXC_CF_BLOCK_FONT,   mbFontUsed );
        ::set_flag( nFlags, EXC_CF_BLOCK_BORDER, mbBorderUsed );
        ::set_flag( nFlags, EXC_CF_BLOCK_AREA,   mbPattUsed );

        // attributes used -> set flags to 0.
        ::set_flag( nFlags, EXC_CF_BORDER_ALL, !mbBorderUsed );
        ::set_flag( nFlags, EXC_CF_AREA_ALL,   !mbPattUsed );

        rStrm << nFlags << sal_uInt16( 0 );

        if( mbFontUsed )
        {
            // font height, 0xFFFFFFFF indicates unused
            sal_uInt32 nHeight = mbHeightUsed ? maFontData.mnHeight : 0xFFFFFFFF;
            // font style: italic and strikeout
            sal_uInt32 nStyle = 0;
            ::set_flag( nStyle, EXC_CF_FONT_STYLE,     maFontData.mbItalic );
            ::set_flag( nStyle, EXC_CF_FONT_STRIKEOUT, maFontData.mbStrikeout );
            // font color, 0xFFFFFFFF indicates unused
            sal_uInt32 nColor = mbColorUsed ? GetPalette().GetColorIndex( mnFontColorId ) : 0xFFFFFFFF;
            // font used flags for italic, weight, and strikeout -> 0 = used, 1 = default
            sal_uInt32 nFontFlags1 = EXC_CF_FONT_ALLDEFAULT;
            ::set_flag( nFontFlags1, EXC_CF_FONT_STYLE, !(mbItalicUsed || mbWeightUsed) );
            ::set_flag( nFontFlags1, EXC_CF_FONT_STRIKEOUT, !mbStrikeUsed );
            // font used flag for underline -> 0 = used, 1 = default
            sal_uInt32 nFontFlags3 = mbUnderlUsed ? 0 : EXC_CF_FONT_UNDERL;

            rStrm.WriteZeroBytes( 64 );
            rStrm   << nHeight
                    << nStyle
                    << maFontData.mnWeight
                    << EXC_FONTESC_NONE
                    << maFontData.mnUnderline;
            rStrm.WriteZeroBytes( 3 );
            rStrm   << nColor
                    << sal_uInt32( 0 )
                    << nFontFlags1
                    << EXC_CF_FONT_ESCAPEM      // escapement never used -> set the flag
                    << nFontFlags3;
            rStrm.WriteZeroBytes( 16 );
            rStrm   << sal_uInt16( 1 );         // must be 1
        }

        if( mbBorderUsed )
        {
            sal_uInt16 nLineStyle = 0;
            sal_uInt32 nLineColor = 0;
            maBorder.SetFinalColors( GetPalette() );
            maBorder.FillToCF8( nLineStyle, nLineColor );
            rStrm << nLineStyle << nLineColor << sal_uInt16( 0 );
        }

        if( mbPattUsed )
        {
            sal_uInt16 nPattern = 0, nColor = 0;
            maArea.SetFinalColors( GetPalette() );
            maArea.FillToCF8( nPattern, nColor );
            rStrm << nPattern << nColor;
        }
    }
    else
    {
        // no data blocks at all
        rStrm << sal_uInt32( 0 ) << sal_uInt16( 0 );
    }

    // *** formulas ***

    if( pXclTokArr1.get() )
        rStrm.Write( pXclTokArr1->GetData(), pXclTokArr1->GetLen() );
    if( pXclTokArr2.get() )
        rStrm.Write( pXclTokArr2->GetData(), pXclTokArr2->GetLen() );
}


// ----------------------------------------------------------------------------

XclExpCF::XclExpCF( const XclExpRoot& rRoot, const ScCondFormatEntry& rFormatEntry ) :
    XclExpRecord( EXC_ID_CF ),
    XclExpRoot( rRoot ),
    mpImpl( new XclExpCF_Impl( rRoot, rFormatEntry ) )
{
}

XclExpCF::~XclExpCF()
{
}

void XclExpCF::WriteBody( XclExpStream& rStrm )
{
    mpImpl->WriteBody( rStrm );
}


// ----------------------------------------------------------------------------

XclExpCondfmt::XclExpCondfmt( const XclExpRoot& rRoot, const ScConditionalFormat& rCondFormat ) :
    XclExpRecord( EXC_ID_CONDFMT ),
    XclExpRoot( rRoot )
{
    GetDoc().FindConditionalFormat( rCondFormat.GetKey(), maRanges, GetCurrScTab() );
    CheckCellRangeList( maRanges );

    if( maRanges.Count() )
        for( USHORT nIndex = 0, nCount = rCondFormat.Count(); nIndex < nCount; ++nIndex )
            if( const ScCondFormatEntry* pEntry = rCondFormat.GetEntry( nIndex ) )
                maCFList.Append( new XclExpCF( GetRoot(), *pEntry ) );
}

XclExpCondfmt::~XclExpCondfmt()
{
}

bool XclExpCondfmt::IsValid() const
{
    return !maCFList.Empty() && maRanges.Count();
}

void XclExpCondfmt::Save( XclExpStream& rStrm )
{
    if( IsValid() )
    {
        XclExpRecord::Save( rStrm );
        maCFList.Save( rStrm );
    }
}

void XclExpCondfmt::WriteBody( XclExpStream& rStrm )
{
    DBG_ASSERT( !maCFList.Empty(), "XclExpCondfmt::WriteBody - no CF records to write" );
    DBG_ASSERT( maRanges.Count(), "XclExpCondfmt::WriteBody - no cell ranges found" );

    // build the minimum range containing all conditionally formatted cells
    ScAddress aMinPos( GetXclMaxPos() );
    ScAddress aMaxPos( 0, 0, 0 );
    for( const ScRange* pRange = maRanges.First(); pRange; pRange = maRanges.Next() )
    {
        if( pRange->aStart.Col() < aMinPos.Col() )
            aMinPos.SetCol( pRange->aStart.Col() );
        if( pRange->aStart.Row() < aMinPos.Row() )
            aMinPos.SetRow( pRange->aStart.Row() );
        if( pRange->aEnd.Col() > aMaxPos.Col() )
            aMaxPos.SetCol( pRange->aEnd.Col() );
        if( pRange->aEnd.Row() > aMaxPos.Row() )
            aMaxPos.SetRow( pRange->aEnd.Row() );
    }

    rStrm   << static_cast< sal_uInt16 >( maCFList.Count() )
            << sal_uInt16( 1 )
            << static_cast< sal_uInt16 >( aMinPos.Row() )
            << static_cast< sal_uInt16 >( aMaxPos.Row() )
            << static_cast< sal_uInt16 >( aMinPos.Col() )
            << static_cast< sal_uInt16 >( aMaxPos.Col() )
            << maRanges;
}

// ----------------------------------------------------------------------------

XclExpCondFormatBuffer::XclExpCondFormatBuffer( const XclExpRoot& rRoot )
{
    if( const ScConditionalFormatList* pCondFmtList = rRoot.GetDoc().GetCondFormList() )
    {
        if( const ScConditionalFormatPtr* ppCondFmt = pCondFmtList->GetData() )
        {
            const ScConditionalFormatPtr* ppCondEnd = ppCondFmt + pCondFmtList->Count();
            for( ; ppCondFmt < ppCondEnd; ++ppCondFmt )
            {
                if( *ppCondFmt )
                {
                    ::std::auto_ptr< XclExpCondfmt > pCondfmtRec( new XclExpCondfmt( rRoot, **ppCondFmt ) );
                    if( pCondfmtRec->IsValid() )
                        maCondfmtList.Append( pCondfmtRec.release() );
                }
            }
        }
    }
}

void XclExpCondFormatBuffer::Save( XclExpStream& rStrm )
{
    maCondfmtList.Save( rStrm );
}


// Validation =================================================================

/** Writes a formula for the DV record. */
void lcl_xecontent_WriteDvFormula( XclExpStream& rStrm, const ExcUPN* pXclTokArr )
{
    sal_uInt16 nSize = pXclTokArr ? pXclTokArr->GetLen() : 0;
    const sal_Char* pData = pXclTokArr ? pXclTokArr->GetData() : NULL;

    rStrm << nSize << sal_uInt16( 0 );
    rStrm.Write( pData, nSize );
}

/** Writes a formula for the DV record, based on a single string. */
void lcl_xecontent_WriteDvFormula( XclExpStream& rStrm, const XclExpString& rString )
{
    // fake a formula with a single tStr token
    rStrm   << static_cast< sal_uInt16 >( rString.GetSize() + 1 )
            << sal_uInt16( 0 )
            << sal_uInt8( 0x17 )    // TODO: define tStr
            << rString;
}


// ----------------------------------------------------------------------------

XclExpDV::XclExpDV( const XclExpRoot& rRoot, sal_uInt32 nHandle ) :
    XclExpRecord( EXC_ID_DV ),
    XclExpRoot( rRoot ),
    mpValData( rRoot.GetDoc().GetValidationEntry( nHandle ) ),
    mnHandle( nHandle )
{
}

void XclExpDV::InsertCellRange( const ScRange& rRange )
{
    maRanges.Join( rRange );
}

bool XclExpDV::CheckWriteRecord()
{
    CheckCellRangeList( maRanges );
    return mpValData && maRanges.Count();
}

void XclExpDV::WriteBody( XclExpStream& rStrm )
{
    DBG_ASSERT( mpValData, "XclExpDV::WriteBody - missing core data" );
    if( !mpValData ) return;

    // prompt box - empty string represented by single NUL character
    String aTitle, aText;
    bool bShowPrompt = (mpValData->GetInput( aTitle, aText ) == TRUE);
    XclExpString aPromptTitle( aTitle );
    if( !aTitle.Len() )
        aPromptTitle.Assign( '\0' );
    XclExpString aPromptText( aText );
    if( !aText.Len() )
        aPromptText.Assign( '\0' );

    // error box - empty string represented by single NUL character
    ScValidErrorStyle eScErrorStyle;
    bool bShowError = (mpValData->GetErrMsg( aTitle, aText, eScErrorStyle ) == TRUE);
    XclExpString aErrorTitle( aTitle );
    if( !aTitle.Len() )
        aErrorTitle.Assign( '\0' );
    XclExpString aErrorText( aText );
    if( !aText.Len() )
        aErrorText.Assign( '\0' );

    // flags
    sal_uInt32 nFlags = 0;
    switch( mpValData->GetDataMode() )
    {
        case SC_VALID_ANY:      nFlags |= EXC_DV_MODE_ANY;      break;
        case SC_VALID_WHOLE:    nFlags |= EXC_DV_MODE_WHOLE;    break;
        case SC_VALID_DECIMAL:  nFlags |= EXC_DV_MODE_DECIMAL;  break;
        case SC_VALID_LIST:     nFlags |= EXC_DV_MODE_LIST;     break;
        case SC_VALID_DATE:     nFlags |= EXC_DV_MODE_DATE;     break;
        case SC_VALID_TIME:     nFlags |= EXC_DV_MODE_TIME;     break;
        case SC_VALID_TEXTLEN:  nFlags |= EXC_DV_MODE_TEXTLEN;  break;
        case SC_VALID_CUSTOM:   nFlags |= EXC_DV_MODE_CUSTOM;   break;
        default:                DBG_ERRORFILE( "XclExpDV::SaveCont - unknown mode" );
    }
    switch( mpValData->GetOperation() )
    {
        case SC_COND_NONE:
        case SC_COND_EQUAL:     nFlags |= EXC_DV_COND_EQUAL;        break;
        case SC_COND_LESS:      nFlags |= EXC_DV_COND_LESS;         break;
        case SC_COND_GREATER:   nFlags |= EXC_DV_COND_GREATER;      break;
        case SC_COND_EQLESS:    nFlags |= EXC_DV_COND_EQLESS;       break;
        case SC_COND_EQGREATER: nFlags |= EXC_DV_COND_EQGREATER;    break;
        case SC_COND_NOTEQUAL:  nFlags |= EXC_DV_COND_NOTEQUAL;     break;
        case SC_COND_BETWEEN:   nFlags |= EXC_DV_COND_BETWEEN;      break;
        case SC_COND_NOTBETWEEN:nFlags |= EXC_DV_COND_NOTBETWEEN;   break;
        default:                DBG_ERRORFILE( "XclExpDV::SaveCont - unknown condition" );
    }
    switch( eScErrorStyle )
    {
        case SC_VALERR_STOP:    nFlags |= EXC_DV_ERROR_STOP;    break;
        case SC_VALERR_WARNING: nFlags |= EXC_DV_ERROR_WARNING; break;
        case SC_VALERR_INFO:    nFlags |= EXC_DV_ERROR_INFO;    break;
        case SC_VALERR_MACRO:
            // #111781# set INFO for validity with macro call, delete title
            nFlags |= EXC_DV_ERROR_INFO;
            aErrorTitle.Assign( '\0' );     // contains macro name
        break;
        default:                DBG_ERRORFILE( "XclExpDV::SaveCont - unknown error style" );
    }
    if( mpValData->IsIgnoreBlank() )
        nFlags |= EXC_DV_IGNOREBLANK;
    if( mpValData->GetListType() == ValidListType::INVISIBLE )
        nFlags |= EXC_DV_SUPPRESSDROPDOWN;
    if( bShowPrompt )
        nFlags |= EXC_DV_SHOWPROMPT;
    if( bShowError )
        nFlags |= EXC_DV_SHOWERROR;

    // formulas
    ::std::auto_ptr< ScTokenArray > pScTokArr;
    EC_Codetype eDummy;

    // first formula
    ::std::auto_ptr< ExcUPN > pXclTokArr1;
    XclExpStringPtr pXclString;
    pScTokArr.reset( mpValData->CreateTokenArry( 0 ) );
    if( pScTokArr.get() )
    {
        String aString;
        if( (mpValData->GetDataMode() == SC_VALID_LIST) &&
            XclTokenArrayHelper::GetStringList( aString, *pScTokArr, '\n' ) )
        {
            /*  Formula is a list of string tokens -> build the Excel string.
                Data validity is BIFF8 only (important for the XclExpString object).
                Excel uses the NUL character as string list separator. */
            pXclString.reset( new XclExpString( EXC_STR_8BITLENGTH ) );
            xub_StrLen nTokenCnt = aString.GetTokenCount( '\n' );
            xub_StrLen nStringIx = 0;
            for( xub_StrLen nToken = 0; nToken < nTokenCnt; ++nToken )
            {
                String aToken( aString.GetToken( 0, '\n', nStringIx ) );
                if( nToken > 0 )
                    pXclString->Append( '\0' );
                pXclString->Append( aToken );
            }
            nFlags |= EXC_DV_STRINGLIST;
        }
        else
        {
            // no list validation -> convert the formula
            pXclTokArr1.reset( new ExcUPN( mpRD, *pScTokArr, eDummy, NULL, true ) );
        }

        /*  All formulas are stored like conditional formatting formulas (with
            tRefN/tAreaN tokens as value or array class). But NOT the cell references
            and defined names in list validation - they are stored as reference class
            tokens... Example:
            1) Cell must be equal to A1 -> formula is =A1 -> writes tRefNV token
            2) List is taken from A1    -> formula is =A1 -> writes tRefNR token

            Following a VERY dirty hack that looks into the Excel token array. If there
            is a leading tRefN*, tAreaN*, or tName* token, it is converted to reference
            class. This is because the formula compiler is already too obscure, so
            adding this special case will surely break anything else there.
            TODO: Remove this mess when the formula compiler is cleaned up! */
        if( (mpValData->GetDataMode() == SC_VALID_LIST) && pXclTokArr1.get() )
        {
            sal_Char* pData = const_cast< sal_Char* >( pXclTokArr1->GetData() );
            if( pData && pXclTokArr1->GetLen() )
            {
                if( (*pData == 0x43) || (*pData == 0x63) ||     // tNameV, tNameA
                    (*pData == 0x4C) || (*pData == 0x6C) ||     // tRefNV, tRefNA
                    (*pData == 0x4D) || (*pData == 0x6D) )      // tAreaNV, tAreaNA
                    // remove any token class and add reference token class
                    (*pData &= 0x1F) |= 0x20;
            }
        }
    }

    // second formula
    ::std::auto_ptr< ExcUPN > pXclTokArr2;
    pScTokArr.reset( mpValData->CreateTokenArry( 1 ) );
    if( pScTokArr.get() )
    {
        EC_Codetype eDummy;
        pXclTokArr2.reset( new ExcUPN( mpRD, *pScTokArr, eDummy, NULL, true ) );
    }

    // export the record
    rStrm << nFlags << aPromptTitle << aErrorTitle << aPromptText << aErrorText;

    if( pXclString.get() )
        lcl_xecontent_WriteDvFormula( rStrm, *pXclString );
    else
        lcl_xecontent_WriteDvFormula( rStrm, pXclTokArr1.get() );
    lcl_xecontent_WriteDvFormula( rStrm, pXclTokArr2.get() );

    rStrm << maRanges;
}


// ----------------------------------------------------------------------------

XclExpDval::XclExpDval( const XclExpRoot& rRoot ) :
    XclExpRecord( EXC_ID_DVAL, 18 ),
    XclExpRoot( rRoot ),
    mpLastFoundDV( NULL )
{
}

XclExpDV& XclExpDval::SearchOrCreateDv( sal_uInt32 nHandle )
{
    // test last found record
    if( mpLastFoundDV && (mpLastFoundDV->GetHandle() == nHandle) )
        return *mpLastFoundDV;

    // binary search
    sal_uInt32 nCurrIndex = 0;
    if( !maDVList.Empty() )
    {
        sal_uInt32 nFirst = 0;
        sal_uInt32 nLast = maDVList.Count() - 1;
        bool bLoop = true;
        sal_uInt32 nCurrHandle;
        while( (nFirst <= nLast) && bLoop )
        {
            nCurrIndex = (nFirst + nLast) / 2;
            mpLastFoundDV = maDVList.GetObject( nCurrIndex );
            nCurrHandle = mpLastFoundDV->GetHandle();
            if( nCurrHandle == nHandle )
                bLoop = false;
            else if( nCurrHandle < nHandle )
                nFirst = nCurrIndex + 1;
            else if( nCurrIndex )
                nLast = nCurrIndex - 1;
            else    // special case for nLast = -1
                bLoop = false;
        }
        if( nCurrHandle == nHandle )
            return *mpLastFoundDV;
        else if( nCurrHandle < nHandle )
            ++nCurrIndex;
    }

    // create new DV record
    mpLastFoundDV = new XclExpDV( *this, nHandle );
    maDVList.Insert( mpLastFoundDV, nCurrIndex );
    return *mpLastFoundDV;
}

void XclExpDval::InsertCellRange( const ScRange& rRange, sal_uInt32 nHandle )
{
    XclExpDV& rDVRec = SearchOrCreateDv( nHandle );
    rDVRec.InsertCellRange( rRange );
}

void XclExpDval::Save( XclExpStream& rStrm )
{
    // check all records
    sal_uInt32 nIndex = maDVList.Count();
    while( nIndex )
    {
        --nIndex;   // backwards to keep nIndex valid
        XclExpDV* pDVRec = maDVList.GetObject( nIndex );
        if( !pDVRec->CheckWriteRecord() )
            maDVList.Delete( nIndex );
    }

    // write the DVAL and the DV's
    if( !maDVList.Empty() )
    {
        XclExpRecord::Save( rStrm );
        maDVList.Save( rStrm );
    }
}

void XclExpDval::WriteBody( XclExpStream& rStrm )
{
    rStrm.WriteZeroBytes( 10 );
    rStrm << EXC_DVAL_NOOBJ << maDVList.Count();
}


// Web Queries ================================================================

XclExpWebQuery::XclExpWebQuery(
        const String& rRangeName,
        const String& rUrl,
        const String& rSource,
        sal_Int32 nRefrSecs ) :
    maDestRange( rRangeName ),
    maUrl( rUrl ),
    // refresh delay time: seconds -> minutes
    mnRefresh( static_cast< sal_Int16 >( ::std::min( (nRefrSecs + 59L) / 60L, 0x7FFFL ) ) ),
    mbEntireDoc( false )
{
    // comma separated list of HTML table names or indexes
    xub_StrLen nTokenCnt = rSource.GetTokenCount( ';' );
    String aNewTables, aAppendTable;
    xub_StrLen nStringIx = 0;
    bool bExitLoop = false;
    for( xub_StrLen nToken = 0; (nToken < nTokenCnt) && !bExitLoop; ++nToken )
    {
        String aToken( rSource.GetToken( 0, ';', nStringIx ) );
        mbEntireDoc = ScfTools::IsHTMLDocName( aToken );
        bExitLoop = mbEntireDoc || ScfTools::IsHTMLTablesName( aToken );
        if( !bExitLoop && ScfTools::GetHTMLNameFromName( aToken, aAppendTable ) )
            ScGlobal::AddToken( aNewTables, aAppendTable, ',' );
    }

    if( !bExitLoop )    // neither HTML_all nor HTML_tables found
    {
        if( aNewTables.Len() )
            mpQryTables.reset( new XclExpString( aNewTables ) );
        else
            mbEntireDoc = true;
    }
}

XclExpWebQuery::~XclExpWebQuery()
{
}

void XclExpWebQuery::Save( XclExpStream& rStrm )
{
    DBG_ASSERT( !mbEntireDoc || !mpQryTables.get(), "XclExpWebQuery::Save - illegal mode" );
    sal_uInt16 nFlags;

    // QSI record
    rStrm.StartRecord( EXC_ID_QSI, 10 + maDestRange.GetSize() );
    rStrm   << EXC_QSI_DEFAULTFLAGS
            << sal_uInt16( 0x0010 )
            << sal_uInt16( 0x0012 )
            << sal_uInt32( 0x00000000 )
            << maDestRange;
    rStrm.EndRecord();

    // PARAMQRY record
    nFlags = EXC_PQRY_DEFAULTFLAGS | (mbEntireDoc ? EXC_PQRY_DOC : EXC_PQRY_TABLES);
    rStrm.StartRecord( EXC_ID_PQRY, 12 );
    rStrm   << nFlags
            << sal_uInt16( 0x0000 )
            << sal_uInt16( 0x0001 );
    rStrm.WriteZeroBytes( 6 );
    rStrm.EndRecord();

    // SXSTRING record
    rStrm.StartRecord( EXC_ID_SXSTRING, maUrl.GetSize() );
    rStrm << maUrl;
    rStrm.EndRecord();

    // unknown record 0x0802
    rStrm.StartRecord( EXC_ID_0802, 16 + maDestRange.GetSize() );
    rStrm   << EXC_ID_0802;             // repeated record id ?!?
    rStrm.WriteZeroBytes( 6 );
    rStrm   << sal_uInt16( 0x0003 )
            << sal_uInt32( 0x00000000 )
            << sal_uInt16( 0x0010 )
            << maDestRange;
    rStrm.EndRecord();

    // WEBQRYSETTINGS record
    nFlags = mpQryTables.get() ? EXC_WQSETT_SPECTABLES : EXC_WQSETT_ALL;
    rStrm.StartRecord( EXC_ID_WQSETT, 28 );
    rStrm   << EXC_ID_WQSETT            // repeated record id ?!?
            << sal_uInt16( 0x0000 )
            << sal_uInt16( 0x0004 )
            << sal_uInt16( 0x0000 )
            << EXC_WQSETT_DEFAULTFLAGS
            << nFlags;
    rStrm.WriteZeroBytes( 10 );
    rStrm   << mnRefresh                // refresh delay in minutes
            << EXC_WQSETT_FORMATFULL
            << sal_uInt16( 0x0000 );
    rStrm.EndRecord();

    // WEBQRYTABLES record
    if( mpQryTables.get() )
    {
        rStrm.StartRecord( EXC_ID_WQTABLES, 4 + mpQryTables->GetSize() );
        rStrm   << EXC_ID_WQTABLES          // repeated record id ?!?
                << sal_uInt16( 0x0000 )
                << *mpQryTables;            // comma separated list of source tables
        rStrm.EndRecord();
    }
}


// ----------------------------------------------------------------------------

XclExpWebQueryBuffer::XclExpWebQueryBuffer( const XclExpRoot& rRoot )
{
    USHORT nScTab = rRoot.GetCurrScTab();
    ScDocument& rDoc = rRoot.GetDoc();
    SfxObjectShell* pShell = rRoot.GetDocShell();
    if( !pShell ) return;
    Reference< XPropertySet > xPropSet( pShell->GetModel(), UNO_QUERY );
    if( !xPropSet.is() ) return;

    Reference< XAreaLinks > xAreaLinks;
    ::getPropValue( xAreaLinks, xPropSet, OUString( RTL_CONSTASCII_USTRINGPARAM( SC_UNO_AREALINKS ) ) );
    Reference< XIndexAccess > xLinksIA( xAreaLinks, UNO_QUERY );
    if( !xLinksIA.is() ) return;

    const OUString aPropFilter( RTL_CONSTASCII_USTRINGPARAM( SC_UNONAME_FILTER ) );
//  const OUString aPropFilterOpt( RTL_CONSTASCII_USTRINGPARAM( SC_UNONAME_FILTOPT ) );
    const OUString aPropUrl( RTL_CONSTASCII_USTRINGPARAM( SC_UNONAME_LINKURL ) );
    const OUString aPropRefresh( RTL_CONSTASCII_USTRINGPARAM( SC_UNONAME_REFDELAY ) );
    OUString aFilter, /*aFilterOpt,*/ aUrl;
    const OUString aWebQueryFilter( RTL_CONSTASCII_USTRINGPARAM( EXC_WEBQRY_FILTER ) );
    String aRangeName;
    sal_Int32 nRefresh;

    sal_Int32 nCount = xLinksIA->getCount();
    for( sal_Int32 nIndex = 0; nIndex < nCount; ++nIndex )
    {
        Reference< XAreaLink > xAreaLink;
        Any aLinkAny( xLinksIA->getByIndex( nIndex ) );
        if( aLinkAny >>= xAreaLink )
        {
            CellRangeAddress aDestRange( xAreaLink->getDestArea() );
            if( static_cast< USHORT >( aDestRange.Sheet ) == nScTab )
            {
                Reference< XPropertySet > xLinkProp( xAreaLink, UNO_QUERY );
                if( xLinkProp.is() && ::getPropValue( aFilter, xLinkProp, aPropFilter ) && (aFilter == aWebQueryFilter) )
                {
                    // get properties
//                  ::getPropValue( aFilterOpt, xLinkProp, aPropFilterOpt );
                    ::getPropValue( aUrl, xLinkProp, aPropUrl );
                    ::getPropValue( nRefresh, xLinkProp, aPropRefresh );

                    String aAbsDoc( ScGlobal::GetAbsDocName( aUrl, pShell ) );
                    INetURLObject aUrlObj( aAbsDoc );
                    String aWebQueryUrl( aUrlObj.getFSysPath( INetURLObject::FSYS_DOS ) );
                    if( !aWebQueryUrl.Len() )
                        aWebQueryUrl = aAbsDoc;

                    // find range or create a new range
                    ScRange aScDestRange;
                    ScUnoConversion::FillScRange( aScDestRange, aDestRange );
                    ScRangeData* pRangeData = rDoc.GetRangeName()->GetRangeAtBlock( aScDestRange );
                    if( pRangeData )
                        aRangeName = pRangeData->GetName();
                    else
                    {
                        ExcName* pExcName = new ExcName( *rRoot.mpRD, aScDestRange, aUrlObj.getBase() );
                        aRangeName = pExcName->GetName();
                        rRoot.mpRD->pNameList->InsertSorted( *rRoot.mpRD, pExcName, nScTab );
                    }

                    // create and store the web query record
                    Append( new XclExpWebQuery( aRangeName, aWebQueryUrl, xAreaLink->getSourceArea(), nRefresh ) );
                }
            }
        }
    }
}


// ============================================================================

