/*************************************************************************
 *
 *  $RCSfile: xetable.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-10-18 15:15:19 $
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

#ifndef SC_XETABLE_HXX
#include "xetable.hxx"
#endif

#include <map>

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HPP_
#include <com/sun/star/i18n/ScriptType.hpp>
#endif

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_DOCITER_HXX
#include "dociter.hxx"
#endif
#ifndef SC_OUTLINETAB_HXX
#include "olinetab.hxx"
#endif
#ifndef SC_CELL_HXX
#include "cell.hxx"
#endif
#ifndef SC_SCPATATR_HXX
#include "patattr.hxx"
#endif
#ifndef SC_SCATTR_HXX
#include "attrib.hxx"
#endif

#ifndef SC_XLFORMULA_HXX
#include "xlformula.hxx"
#endif
#ifndef SC_XEHELPER_HXX
#include "xehelper.hxx"
#endif
#ifndef SC_XECONTENT_HXX
#include "xecontent.hxx"
#endif
#ifndef SC_XEESCHER_HXX
#include "xeescher.hxx"
#endif

#include "excupn.hxx"

// ============================================================================
// Helper records for cell records
// ============================================================================

XclExpStringRec::XclExpStringRec( const XclExpRoot& rRoot, const String& rResult ) :
    XclExpRecord( EXC_ID_STRING ),
    mxResult( XclExpStringHelper::CreateString( rRoot, rResult ) )
{
    DBG_ASSERT( (rRoot.GetBiff() <= xlBiff7) || (mxResult->Len() > 0),
        "XclExpStringRec::XclExpStringRec - empty result not allowed in BIFF8+" );
    SetRecSize( mxResult->GetSize() );
}

void XclExpStringRec::WriteBody( XclExpStream& rStrm )
{
    rStrm << *mxResult;
}

// Additional records for special formula ranges ==============================

XclExpRangeFmlaBase::XclExpRangeFmlaBase(
        sal_uInt16 nRecId, sal_uInt32 nRecSize, sal_uInt16 nXclCol, sal_uInt16 nXclRow ) :
    XclExpRecord( nRecId, nRecSize ),
    mnUsedCount( 1 ),
    mnFirstXclCol( nXclCol ),
    mnFirstXclRow( nXclRow ),
    mnLastXclCol( nXclCol ),
    mnLastXclRow( nXclRow ),
    mnBaseXclCol( nXclCol ),
    mnBaseXclRow( nXclRow )
{
}

XclExpRangeFmlaBase::XclExpRangeFmlaBase( sal_uInt16 nRecId, sal_uInt32 nRecSize,
        sal_uInt16 nFirstXclCol, sal_uInt16 nFirstXclRow, sal_uInt16 nLastXclCol, sal_uInt16 nLastXclRow ) :
    XclExpRecord( nRecId, nRecSize ),
    mnUsedCount( 1 ),
    mnFirstXclCol( nFirstXclCol ),
    mnFirstXclRow( nFirstXclRow ),
    mnLastXclCol( nLastXclCol ),
    mnLastXclRow( nLastXclRow ),
    mnBaseXclCol( nFirstXclCol ),
    mnBaseXclRow( nFirstXclRow )
{
}

void XclExpRangeFmlaBase::Extend( sal_uInt16 nXclCol, sal_uInt16 nXclRow )
{
    mnFirstXclCol = ::std::min( mnFirstXclCol, nXclCol );
    mnFirstXclRow = ::std::min( mnFirstXclRow, nXclRow );
    mnLastXclCol = ::std::max( mnLastXclCol, nXclCol );
    mnLastXclRow = ::std::max( mnLastXclRow, nXclRow );
    ++mnUsedCount;
}

bool XclExpRangeFmlaBase::Contains( sal_uInt16 nXclCol, sal_uInt16 nXclRow ) const
{
    return  (mnFirstXclCol <= nXclCol) && (nXclCol <= mnLastXclCol) &&
            (mnFirstXclRow <= nXclRow) && (nXclRow <= mnLastXclRow);
}

void XclExpRangeFmlaBase::WriteRangeAddress( XclExpStream& rStrm ) const
{
    rStrm   << mnFirstXclRow << mnLastXclRow
            << static_cast< sal_uInt8 >( mnFirstXclCol )
            << static_cast< sal_uInt8 >( mnLastXclCol );
}

// Array formulas =============================================================

XclExpArray::XclExpArray( const ExcUPN& rXclTokArr,
        sal_uInt16 nFirstXclCol, sal_uInt16 nFirstXclRow, sal_uInt16 nLastXclCol, sal_uInt16 nLastXclRow ) :
    XclExpRangeFmlaBase( EXC_ID_ARRAY, 0, nFirstXclCol, nFirstXclRow, nLastXclCol, nLastXclRow )
{
    rXclTokArr.GetFmlaData( maFmlaData );
    SetRecSize( static_cast< sal_uInt32 >( 14 + maFmlaData.size() ) );
}

void XclExpArray::WriteCellTokenArray( XclExpStream& rStrm, sal_uInt16 nXclCol, sal_uInt16 nXclRow ) const
{
    DBG_ASSERT( Contains( nXclCol, nXclRow ), "XclExpArray::WriteCellTokenArray - invalid cell address" );
    rStrm << sal_uInt16( 5 ) << EXC_TOKID_EXP << mnBaseXclRow << mnBaseXclCol;
}

void XclExpArray::WriteBody( XclExpStream& rStrm )
{
    sal_uInt16 nFmlaSize = static_cast< sal_uInt16 >( maFmlaData.size() );
    WriteRangeAddress( rStrm );
    rStrm << EXC_ARRAY_DEFAULTFLAGS << sal_uInt32( 0 ) << nFmlaSize;
    if( nFmlaSize )
        rStrm.Write( &maFmlaData.front(), nFmlaSize );
}

// Shared formulas ============================================================

XclExpShrfmla::XclExpShrfmla( const ExcUPN& rXclTokArr, sal_uInt16 nXclCol, sal_uInt16 nXclRow ) :
    XclExpRangeFmlaBase( EXC_ID_SHRFMLA, 0, nXclCol, nXclRow )
{
    rXclTokArr.GetFmlaData( maFmlaData );
    SetRecSize( static_cast< sal_uInt32 >( 10 + maFmlaData.size() ) );
}

void XclExpShrfmla::ExtendRange( sal_uInt16 nXclCol, sal_uInt16 nXclRow )
{
    Extend( nXclCol, nXclRow );
}

void XclExpShrfmla::WriteCellTokenArray( XclExpStream& rStrm, sal_uInt16 nXclCol, sal_uInt16 nXclRow ) const
{
    DBG_ASSERT( Contains( nXclCol, nXclRow ), "XclExpShrfmla::WriteCellTokenArray - invalid cell address" );
    rStrm << sal_uInt16( 5 ) << EXC_TOKID_EXP << mnBaseXclRow << mnBaseXclCol;
}

void XclExpShrfmla::WriteBody( XclExpStream& rStrm )
{
    sal_uInt16 nFmlaSize = static_cast< sal_uInt16 >( maFmlaData.size() );
    WriteRangeAddress( rStrm );
    rStrm << static_cast< sal_uInt16 >( GetUsedCount() ) << nFmlaSize;
    if( nFmlaSize )
        rStrm.Write( &maFmlaData.front(), nFmlaSize );
}

// ----------------------------------------------------------------------------

XclExpShrfmlaBuffer::XclExpShrfmlaBuffer( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot )
{
}

XclExpShrfmlaRef XclExpShrfmlaBuffer::CreateOrExtendShrfmla(
        const ScTokenArray& rScTokArr, const ExcUPN& rXclTokArr,
        sal_uInt16 nXclCol, sal_uInt16 nXclRow )
{
    XclExpShrfmlaRef xRec;
    if( const ScRangeData* pRangeData = XclTokenArrayHelper::GetSharedFormula( GetRoot(), rScTokArr ) )
    {
        XclExpShrfmlaMap::iterator aIt = maRecMap.find( pRangeData );
        if( aIt == maRecMap.end() )
        {
            // create a new record
            xRec.reset( new XclExpShrfmla( rXclTokArr, nXclCol, nXclRow ) );
            maRecMap[ pRangeData ] = xRec;
        }
        else
        {
            // extend existing record
            DBG_ASSERT( aIt->second, "XclExpShrfmlaBuffer::CreateOrExtendShrfmla - missing record" );
            xRec = aIt->second;
            xRec->ExtendRange( nXclCol, nXclRow );
        }
    }
    return xRec;
}

// Multiple operations ========================================================

XclExpTableop::XclExpTableop( const ScAddress& rScPos,
        const XclMultipleOpRefs& rRefs, sal_uInt8 nScMode ) :
    XclExpRangeFmlaBase( EXC_ID_TABLEOP, 16, static_cast< sal_uInt16 >( rScPos.Col() ), static_cast< sal_uInt16 >( rScPos.Row() ) ),
    mnLastAppXclCol( static_cast< sal_uInt16 >( rScPos.Col() ) ),
    mnColInpXclCol( static_cast< sal_uInt16 >( rRefs.maColFirstScPos.Col() ) ),
    mnColInpXclRow( static_cast< sal_uInt16 >( rRefs.maColFirstScPos.Row() ) ),
    mnRowInpXclCol( static_cast< sal_uInt16 >( rRefs.maRowFirstScPos.Col() ) ),
    mnRowInpXclRow( static_cast< sal_uInt16 >( rRefs.maRowFirstScPos.Row() ) ),
    mnScMode( nScMode ),
    mbValid( false )
{
}

bool XclExpTableop::TryExtend( const ScAddress& rScPos, const XclMultipleOpRefs& rRefs )
{
    sal_uInt16 nXclCol = static_cast< sal_uInt16 >( rScPos.Col() );
    sal_uInt16 nXclRow = static_cast< sal_uInt16 >( rScPos.Row() );

    bool bOk = IsAppendable( nXclCol, nXclRow );
    if( bOk )
    {
        SCCOL nFirstScCol  = static_cast< SCCOL >( mnFirstXclCol );
        SCROW nFirstScRow  = static_cast< SCROW >( mnFirstXclRow );
        SCCOL nLastScCol   = static_cast< SCCOL >( mnLastXclCol );
        SCROW nLastScRow   = static_cast< SCROW >( mnLastXclRow );
        SCCOL nColInpScCol = static_cast< SCCOL >( mnColInpXclCol );
        SCROW nColInpScRow = static_cast< SCROW >( mnColInpXclRow );
        SCCOL nRowInpScCol = static_cast< SCCOL >( mnRowInpXclCol );
        SCROW nRowInpScRow = static_cast< SCROW >( mnRowInpXclRow );

        bOk =   ((mnScMode == 2) == rRefs.mbDblRefMode) &&
                (rScPos.Tab() == rRefs.maFmlaScPos.Tab()) &&
                (nColInpScCol == rRefs.maColFirstScPos.Col()) &&
                (nColInpScRow == rRefs.maColFirstScPos.Row()) &&
                (rScPos.Tab() == rRefs.maColFirstScPos.Tab()) &&
                (rScPos.Tab() == rRefs.maColRelScPos.Tab());

        if( bOk ) switch( mnScMode )
        {
            case 0:
                bOk =   (rScPos.Col() == rRefs.maFmlaScPos.Col()) &&
                        (nFirstScRow  == rRefs.maFmlaScPos.Row() + 1) &&
                        (nFirstScCol  == rRefs.maColRelScPos.Col() + 1) &&
                        (rScPos.Row() == rRefs.maColRelScPos.Row());
            break;
            case 1:
                bOk =   (nFirstScCol  == rRefs.maFmlaScPos.Col() + 1) &&
                        (rScPos.Row() == rRefs.maFmlaScPos.Row()) &&
                        (rScPos.Col() == rRefs.maColRelScPos.Col()) &&
                        (nFirstScRow  == rRefs.maColRelScPos.Row() + 1);
            break;
            case 2:
                bOk =   (nFirstScCol  == rRefs.maFmlaScPos.Col() + 1) &&
                        (nFirstScRow  == rRefs.maFmlaScPos.Row() + 1) &&
                        (nFirstScCol  == rRefs.maColRelScPos.Col() + 1) &&
                        (rScPos.Row() == rRefs.maColRelScPos.Row()) &&
                        (nRowInpScCol == rRefs.maRowFirstScPos.Col()) &&
                        (nRowInpScRow == rRefs.maRowFirstScPos.Row()) &&
                        (rScPos.Tab() == rRefs.maRowFirstScPos.Tab()) &&
                        (rScPos.Col() == rRefs.maRowRelScPos.Col()) &&
                        (nFirstScRow  == rRefs.maRowRelScPos.Row() + 1) &&
                        (rScPos.Tab() == rRefs.maRowRelScPos.Tab());
            break;
            default:
                bOk = false;
        }

        if( bOk )
        {
            // extend the cell range
            DBG_ASSERT( IsAppendable( nXclCol, nXclRow ), "XclExpTableop::TryExtend - wrong cell address" );
            Extend( nXclCol, nXclRow );
            mnLastAppXclCol = nXclCol;
        }
    }

    return bOk;
}

void XclExpTableop::Finalize()
{
    // is the range complete? (last appended cell is in last column)
    mbValid = mnLastXclCol == mnLastAppXclCol;
    // if last row is incomplete, try to shorten the used range
    if( !mbValid && (mnFirstXclRow < mnLastXclRow) )
    {
        --mnLastXclRow;
        mbValid = true;
    }

    // check if referred cells are outside of own range
    if( mbValid ) switch( mnScMode )
    {
        case 0:
            mbValid =   (mnColInpXclCol + 1 < mnFirstXclCol) || (mnColInpXclCol > mnLastXclCol) ||
                        (mnColInpXclRow     < mnFirstXclRow) || (mnColInpXclRow > mnLastXclRow);
        break;
        case 1:
            mbValid =   (mnColInpXclCol     < mnFirstXclCol) || (mnColInpXclCol > mnLastXclCol) ||
                        (mnColInpXclRow + 1 < mnFirstXclRow) || (mnColInpXclRow > mnLastXclRow);
        break;
        case 2:
            mbValid =   ((mnColInpXclCol + 1 < mnFirstXclCol) || (mnColInpXclCol > mnLastXclCol) ||
                         (mnColInpXclRow + 1 < mnFirstXclRow) || (mnColInpXclRow > mnLastXclRow)) &&
                        ((mnRowInpXclCol + 1 < mnFirstXclCol) || (mnRowInpXclCol > mnLastXclCol) ||
                         (mnRowInpXclRow + 1 < mnFirstXclRow) || (mnRowInpXclRow > mnLastXclRow));
        break;
    }
}

void XclExpTableop::WriteCellTokenArray( XclExpStream& rStrm, sal_uInt16 nXclCol, sal_uInt16 nXclRow ) const
{
    if( mbValid && Contains( nXclCol, nXclRow ) )
        rStrm << sal_uInt16( 5 ) << EXC_TOKID_TBL << mnBaseXclRow << mnBaseXclCol;
    else
        rStrm << sal_uInt16( 2 ) << EXC_TOKID_ERR << EXC_ERR_NA;
}

void XclExpTableop::Save( XclExpStream& rStrm )
{
    if( mbValid )
        XclExpRangeFmlaBase::Save( rStrm );
}

bool XclExpTableop::IsAppendable( sal_uInt16 nXclCol, sal_uInt16 nXclRow ) const
{
    return  ((nXclCol == mnLastAppXclCol + 1) && (nXclRow == mnFirstXclRow)) ||
            ((nXclCol == mnLastAppXclCol + 1) && (nXclCol <= mnLastXclCol) && (nXclRow == mnLastXclRow)) ||
            ((mnLastAppXclCol == mnLastXclCol) && (nXclCol == mnFirstXclCol) && (nXclRow == mnLastXclRow + 1));
}

void XclExpTableop::WriteBody( XclExpStream& rStrm )
{
    sal_uInt16 nFlags = EXC_TABLEOP_DEFAULTFLAGS;
    switch( mnScMode )
    {
        case 1: ::set_flag( nFlags, EXC_TABLEOP_ROW );  break;
        case 2: ::set_flag( nFlags, EXC_TABLEOP_BOTH ); break;
    }

    WriteRangeAddress( rStrm );
    rStrm << nFlags;
    if( mnScMode == 2 )
        rStrm << mnRowInpXclRow << mnRowInpXclCol << mnColInpXclRow << mnColInpXclCol;
    else
        rStrm << mnColInpXclRow << mnColInpXclCol << sal_uInt32( 0 );
}

// ----------------------------------------------------------------------------

XclExpTableopBuffer::XclExpTableopBuffer( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot )
{
}

XclExpTableopRef XclExpTableopBuffer::CreateOrExtendTableop( const ScTokenArray& rScTokArr, const ScAddress& rScPos )
{
    XclExpTableopRef xRec;

    // try to extract cell references of a multiple operations formula
    XclMultipleOpRefs aRefs;
    if( XclTokenArrayHelper::GetMultipleOpRefs( aRefs, rScTokArr ) )
    {
        // try to find an existing TABLEOP record for this cell position
        bool bFound = false;
        for( size_t nPos = 0, nSize = maTableopList.Size(); !xRec && (nPos < nSize); ++nPos )
        {
            XclExpTableopRef xTempRec = maTableopList.GetRecord( nPos );
            if( xTempRec->TryExtend( rScPos, aRefs ) )
                xRec = xTempRec;
        }

        // no record found, or found record not extensible
        if( !xRec )
            xRec = TryCreate( rScPos, aRefs );
    }

    return xRec;
}

void XclExpTableopBuffer::Finalize()
{
    for( size_t nPos = 0, nSize = maTableopList.Size(); nPos < nSize; ++nPos )
        maTableopList.GetRecord( nPos )->Finalize();
}

XclExpTableopRef XclExpTableopBuffer::TryCreate( const ScAddress& rScPos, const XclMultipleOpRefs& rRefs )
{
    sal_uInt8 nScMode = 0;
    bool bOk =  (rScPos.Tab() == rRefs.maFmlaScPos.Tab()) &&
                (rScPos.Tab() == rRefs.maColFirstScPos.Tab()) &&
                (rScPos.Tab() == rRefs.maColRelScPos.Tab());

    if( bOk )
    {
        if( rRefs.mbDblRefMode )
        {
            nScMode = 2;
            bOk =   (rScPos.Col() == rRefs.maFmlaScPos.Col() + 1) &&
                    (rScPos.Row() == rRefs.maFmlaScPos.Row() + 1) &&
                    (rScPos.Col() == rRefs.maColRelScPos.Col() + 1) &&
                    (rScPos.Row() == rRefs.maColRelScPos.Row()) &&
                    (rScPos.Tab() == rRefs.maRowFirstScPos.Tab()) &&
                    (rScPos.Col() == rRefs.maRowRelScPos.Col()) &&
                    (rScPos.Row() == rRefs.maRowRelScPos.Row() + 1) &&
                    (rScPos.Tab() == rRefs.maRowRelScPos.Tab());
        }
        else if( (rScPos.Col() == rRefs.maFmlaScPos.Col()) &&
                 (rScPos.Row() == rRefs.maFmlaScPos.Row() + 1) &&
                 (rScPos.Col() == rRefs.maColRelScPos.Col() + 1) &&
                 (rScPos.Row() == rRefs.maColRelScPos.Row()) )
        {
            nScMode = 0;
        }
        else if( (rScPos.Col() == rRefs.maFmlaScPos.Col() + 1) &&
                 (rScPos.Row() == rRefs.maFmlaScPos.Row()) &&
                 (rScPos.Col() == rRefs.maColRelScPos.Col()) &&
                 (rScPos.Row() == rRefs.maColRelScPos.Row() + 1) )
        {
            nScMode = 1;
        }
        else
        {
            bOk = false;
        }
    }

    XclExpTableopRef xRec;
    if( bOk )
    {
        xRec.reset( new XclExpTableop( rScPos, rRefs, nScMode ) );
        maTableopList.AppendRecord( xRec );
    }

    return xRec;
}

// ============================================================================
// Cell records
// ============================================================================

XclExpCellBase::XclExpCellBase(
        sal_uInt16 nRecId, sal_uInt32 nContSize, sal_uInt16 nXclCol, sal_uInt16 nXclRow ) :
    XclExpRecord( nRecId, nContSize + 4 ),
    mnXclCol( nXclCol ),
    mnXclRow( nXclRow )
{
}

bool XclExpCellBase::IsMultiLineText() const
{
    return false;
}

bool XclExpCellBase::TryMerge( const XclExpCellBase& rCell )
{
    return false;
}

void XclExpCellBase::GetBlankXFIndexes( ScfUInt16Vec& rXFIndexes ) const
{
    // default: do nothing
}

void XclExpCellBase::RemoveUnusedBlankCells( const ScfUInt16Vec& rXFIndexes )
{
    // default: do nothing
}

// Single cell records ========================================================

XclExpSingleCellBase::XclExpSingleCellBase(
        sal_uInt16 nRecId, sal_uInt32 nContSize,
        sal_uInt16 nXclCol, sal_uInt16 nXclRow, sal_uInt32 nXFId ) :
    XclExpCellBase( nRecId, 2, nXclCol, nXclRow ),
    maXFId( nXFId ),
    mnContSize( nContSize )
{
}

XclExpSingleCellBase::XclExpSingleCellBase( const XclExpRoot& rRoot,
        sal_uInt16 nRecId, sal_uInt32 nContSize, sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_Int16 nScript, sal_uInt32 nForcedXFId ) :
    XclExpCellBase( nRecId, 2, nXclCol, nXclRow ),
    maXFId( nForcedXFId ),
    mnContSize( nContSize )
{
    if( GetXFId() == EXC_XFID_NOTFOUND )
        SetXFId( rRoot.GetXFBuffer().Insert( pPattern, nScript ) );
}

sal_uInt16 XclExpSingleCellBase::GetLastXclCol() const
{
    return GetXclCol();
}

sal_uInt32 XclExpSingleCellBase::GetFirstXFId() const
{
    return GetXFId();
}

bool XclExpSingleCellBase::IsEmpty() const
{
    return false;
}

void XclExpSingleCellBase::ConvertXFIndexes( const XclExpRoot& rRoot )
{
    maXFId.ConvertXFIndex( rRoot );
}

void XclExpSingleCellBase::Save( XclExpStream& rStrm )
{
    DBG_ASSERT_BIFF( rStrm.GetRoot().GetBiff() >= xlBiff3 );
    SetRecSize( GetRecSize() + mnContSize );
    XclExpCellBase::Save( rStrm );
}

void XclExpSingleCellBase::WriteBody( XclExpStream& rStrm )
{
    rStrm << GetXclRow() << GetXclCol() << maXFId.mnXFIndex;
    WriteContents( rStrm );
}

// ----------------------------------------------------------------------------

IMPL_FIXEDMEMPOOL_NEWDEL( XclExpNumberCell, 256, 256 )

XclExpNumberCell::XclExpNumberCell(
        const XclExpRoot& rRoot, sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId, double fValue ) :
    XclExpSingleCellBase( rRoot, EXC_ID_NUMBER, 8, nXclCol, nXclRow,
        pPattern, ::com::sun::star::i18n::ScriptType::WEAK, nForcedXFId ),
    mfValue( fValue )
{
}

void XclExpNumberCell::WriteContents( XclExpStream& rStrm )
{
    rStrm << mfValue;
}

// ----------------------------------------------------------------------------

IMPL_FIXEDMEMPOOL_NEWDEL( XclExpBooleanCell, 256, 256 )

XclExpBooleanCell::XclExpBooleanCell(
        const XclExpRoot rRoot, sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId, bool bValue ) :
    XclExpSingleCellBase( rRoot, EXC_ID_BOOLERR, 2, nXclCol, nXclRow,
        pPattern, ::com::sun::star::i18n::ScriptType::WEAK, nForcedXFId ),
    mbValue( bValue )
{
}

void XclExpBooleanCell::WriteContents( XclExpStream& rStrm )
{
    rStrm << sal_uInt16( mbValue ? 1 : 0 ) << EXC_BOOLERR_BOOL;
}

// ----------------------------------------------------------------------------

IMPL_FIXEDMEMPOOL_NEWDEL( XclExpErrorCell, 256, 256 )

XclExpErrorCell::XclExpErrorCell(
        const XclExpRoot rRoot, sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId, sal_uInt8 nErrCode ) :
    XclExpSingleCellBase( rRoot, EXC_ID_BOOLERR, 2, nXclCol, nXclRow,
        pPattern, ::com::sun::star::i18n::ScriptType::WEAK, nForcedXFId ),
    mnErrCode( nErrCode )
{
}

void XclExpErrorCell::WriteContents( XclExpStream& rStrm )
{
    rStrm << mnErrCode << EXC_BOOLERR_ERROR;
}

// ----------------------------------------------------------------------------

IMPL_FIXEDMEMPOOL_NEWDEL( XclExpLabelCell, 256, 256 )

XclExpLabelCell::XclExpLabelCell(
        const XclExpRoot& rRoot, sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId, const ScStringCell& rCell ) :
    XclExpSingleCellBase( EXC_ID_LABEL, 0, nXclCol, nXclRow, nForcedXFId )
{
    sal_uInt16 nMaxLen = (rRoot.GetBiff() >= xlBiff8) ? EXC_STR_MAXLEN : EXC_LABEL_MAXLEN;
    XclExpStringRef xText = XclExpStringHelper::CreateCellString( rRoot, rCell, pPattern, EXC_STR_DEFAULT, nMaxLen );
    Init( rRoot, pPattern, xText );
}

XclExpLabelCell::XclExpLabelCell(
        const XclExpRoot& rRoot, sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId,
        const ScEditCell& rCell, XclExpHyperlinkHelper& rLinkHelper ) :
    XclExpSingleCellBase( EXC_ID_LABEL, 0, nXclCol, nXclRow, nForcedXFId )
{
    sal_uInt16 nMaxLen = (rRoot.GetBiff() >= xlBiff8) ? EXC_STR_MAXLEN : EXC_LABEL_MAXLEN;
    XclExpStringRef xText = XclExpStringHelper::CreateCellString( rRoot, rCell, pPattern, rLinkHelper, EXC_STR_DEFAULT, nMaxLen );
    Init( rRoot, pPattern, xText );
}

bool XclExpLabelCell::IsMultiLineText() const
{
    return mxText->IsWrapped();
}

void XclExpLabelCell::Init( const XclExpRoot& rRoot,
        const ScPatternAttr* pPattern, XclExpStringRef xText )
{
    DBG_ASSERT( xText.is() && xText->Len(), "XclExpLabelCell::XclExpLabelCell - empty string passed" );
    mxText = xText;
    mnSstIndex = 0;

    // create the cell format
    sal_uInt16 nXclFont = mxText->RemoveLeadingFont();
    DBG_ASSERT( nXclFont != EXC_FONT_NOTFOUND, "XclExpLabelCell::Init - missing first font" );
    if( GetXFId() == EXC_XFID_NOTFOUND )
    {
        bool bForceLineBreak = mxText->IsWrapped();
        SetXFId( rRoot.GetXFBuffer().InsertWithFont(
            pPattern, ::com::sun::star::i18n::ScriptType::WEAK, nXclFont, bForceLineBreak ) );
    }

    // initialize the record contents
    switch( rRoot.GetBiff() )
    {
        case xlBiff5:
        case xlBiff7:
            // BIFF5-BIFF7: create a LABEL or RSTRING record
            DBG_ASSERT( mxText->Len() <= EXC_LABEL_MAXLEN, "XclExpLabelCell::XclExpLabelCell - string too long" );
            SetContSize( mxText->GetSize() );
            // formatted string is exported in an RSTRING record
            if( mxText->IsRich() )
            {
                DBG_ASSERT( mxText->GetFormatsCount() <= EXC_LABEL_MAXLEN, "XclExpLabelCell::WriteContents - too many formats" );
                mxText->LimitFormatCount( EXC_LABEL_MAXLEN );
                SetRecId( EXC_ID_RSTRING );
                SetContSize( GetContSize() + 1 + 2 * mxText->GetFormatsCount() );
            }
        break;
        case xlBiff8:
            // BIFF8+: create a LABELSST record
            mnSstIndex = rRoot.GetSst().Insert( xText );
            SetRecId( EXC_ID_LABELSST );
            SetContSize( 4 );
        break;
        default:    DBG_ERROR_BIFF();
    }
}

void XclExpLabelCell::WriteContents( XclExpStream& rStrm )
{
    switch( rStrm.GetRoot().GetBiff() )
    {
        case xlBiff5:
        case xlBiff7:
            rStrm << *mxText;
            if( mxText->IsRich() )
            {
                rStrm << static_cast< sal_uInt8 >( mxText->GetFormatsCount() );
                mxText->WriteFormats( rStrm );
            }
        break;
        case xlBiff8:
            rStrm << mnSstIndex;
        break;
        default:    DBG_ERROR_BIFF();
    }
}

// ----------------------------------------------------------------------------

IMPL_FIXEDMEMPOOL_NEWDEL( XclExpFormulaCell, 256, 256 )

XclExpFormulaCell::XclExpFormulaCell( const XclExpRoot& rRoot,
        sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId,
        const ScFormulaCell& rScFmlaCell,
        XclExpShrfmlaBuffer& rShrfmlaBfr, XclExpTableopBuffer& rTableopBfr ) :
    XclExpSingleCellBase( EXC_ID_FORMULA, 0, nXclCol, nXclRow, nForcedXFId ),
    mrScFmlaCell( const_cast< ScFormulaCell& >( rScFmlaCell ) ),
    mbFirstAddRec( false )
{
    // *** Find result number format overwriting cell number format *** -------

    if( GetXFId() == EXC_XFID_NOTFOUND )
    {
        SvNumberFormatter& rFormatter = rRoot.GetFormatter();
        XclExpNumFmtBuffer& rNumFmtBfr = rRoot.GetNumFmtBuffer();

        // current cell number format
        ULONG nScNumFmt = pPattern ?
            GETITEMVALUE( pPattern->GetItemSet(), SfxUInt32Item, ATTR_VALUE_FORMAT, ULONG ) :
            rNumFmtBfr.GetStandardFormat();

        // alternative number format passed to XF buffer
        ULONG nAltScNumFmt = NUMBERFORMAT_ENTRY_NOT_FOUND;
        /*  #73420# Xcl doesn't know Boolean number formats, we write
            "TRUE";"FALSE" (language dependent). Don't do it for automatic
            formula formats, because Excel gets them right. */
        /*  #i8640# Don't set text format, if we have string results. */
        short nFormatType = mrScFmlaCell.GetFormatType();
        if( ((nScNumFmt % SV_COUNTRY_LANGUAGE_OFFSET) == 0) &&
                (nFormatType != NUMBERFORMAT_LOGICAL) &&
                (nFormatType != NUMBERFORMAT_TEXT) )
            nAltScNumFmt = mrScFmlaCell.GetStandardFormat( rFormatter, nScNumFmt );
        /*  #73420# If cell number format is Boolean and automatic formula
            format is Boolean don't write that ugly special format. */
        else if( (nFormatType == NUMBERFORMAT_LOGICAL) &&
                (rFormatter.GetType( nScNumFmt ) == NUMBERFORMAT_LOGICAL) )
            nAltScNumFmt = rNumFmtBfr.GetStandardFormat();

        SetXFId( rRoot.GetXFBuffer().InsertWithNumFmt(
            pPattern, ::com::sun::star::i18n::ScriptType::WEAK, nAltScNumFmt ) );
    }

    // *** Convert the formula token array *** --------------------------------

    ScAddress aScPos( static_cast< SCCOL >( nXclCol ), static_cast< SCROW >( nXclRow ), rRoot.GetCurrScTab() );
    const ScTokenArray& rScTokArr = *mrScFmlaCell.GetCode();

    // first try to create multiple operations
    mxAddRec = rTableopBfr.CreateOrExtendTableop( rScTokArr, aScPos );
    if( !mxAddRec )
    {
        ScMatrixMode eMatMode = static_cast< ScMatrixMode >( mrScFmlaCell.GetMatrixFlag() );

        EC_Codetype eCodeType = EC_StdFmla;
        ::std::auto_ptr< ExcUPN > xXclTokArr;

        switch( eMatMode )
        {
            case MM_FORMULA:
                xXclTokArr.reset( new ExcUPN( TRUE, rRoot.mpRD, rScTokArr, eCodeType, aScPos ) );
            break;
            case MM_REFERENCE:
                xXclTokArr.reset( new ExcUPN( rScTokArr, eCodeType ) );
            break;
        }

        if( !xXclTokArr.get() || (eCodeType != EC_ArrayFmla) )
            xXclTokArr.reset( new ExcUPN( rRoot.mpRD, rScTokArr, eCodeType, &aScPos, FALSE ) );

        switch( eCodeType )
        {
            case EC_StdFmla:
                xXclTokArr->GetFmlaData( maFmlaData );
            break;

            case EC_ArrayFmla:
            {
                xXclTokArr->GetShrdFmla( maFmlaData );
                if( eMatMode == MM_FORMULA )
                {
                    // origin of the matrix - create the ARRAY record
                    SCCOL nMatWidth;
                    SCROW nMatHeight;
                    mrScFmlaCell.GetMatColsRows( nMatWidth, nMatHeight );
                    DBG_ASSERT( nMatWidth && nMatHeight, "XclExpFormulaCell::XclExpFormulaCell - empty matrix" );
                    ScRange aMatRange( aScPos );
                    ScAddress& rMatEnd = aMatRange.aEnd;
                    rMatEnd.IncCol( static_cast< SCsCOL >( nMatWidth - 1 ) );
                    rMatEnd.IncRow( static_cast< SCsROW >( nMatHeight - 1 ) );
                    rRoot.CheckCellRange( aMatRange );
                    mxAddRec.reset( new XclExpArray( *xXclTokArr, nXclCol, nXclRow,
                        static_cast< sal_uInt16 >( rMatEnd.Col() ), static_cast< sal_uInt16 >( rMatEnd.Row() ) ) );
                }
            }
            break;

            case EC_ShrdFmla:
                mxAddRec = rShrfmlaBfr.CreateOrExtendShrfmla( rScTokArr, *xXclTokArr, nXclCol, nXclRow );
            break;
        }
    }

    size_t nFmlaSize = maFmlaData.empty() ? 5 : maFmlaData.size();
    SetContSize( static_cast< sal_uInt32 >( 16 + nFmlaSize ) );
    mbFirstAddRec = mxAddRec.is() && (mxAddRec->GetUsedCount() == 1);
}

void XclExpFormulaCell::Save( XclExpStream& rStrm )
{
    XclExpSingleCellBase::Save( rStrm );
    // additional records
    if( mxAddRec.is() && mbFirstAddRec )
        mxAddRec->Save( rStrm );
    if( mxStringRec.is() )
        mxStringRec->Save( rStrm );
}

void XclExpFormulaCell::WriteContents( XclExpStream& rStrm )
{
    // result of the formula
    switch( mrScFmlaCell.GetFormatType() )
    {
        case NUMBERFORMAT_NUMBER:
        {
            // either value or error code
            USHORT nScErrCode = mrScFmlaCell.GetErrCode();
            if( nScErrCode )
                rStrm << EXC_FORMULA_RES_ERROR << sal_uInt8( 0 )
                      << XclTools::GetXclErrorCode( nScErrCode )
                      << sal_uInt8( 0 ) << sal_uInt16( 0 )
                      << sal_uInt16( 0xFFFF );
            else
                rStrm << mrScFmlaCell.GetValue();
        }
        break;

        case NUMBERFORMAT_TEXT:
        {
            String aResult;
            mrScFmlaCell.GetString( aResult );
            if( aResult.Len() || (rStrm.GetRoot().GetBiff() <= xlBiff7) )
            {
                rStrm << EXC_FORMULA_RES_STRING;
                mxStringRec.reset( new XclExpStringRec( rStrm.GetRoot(), aResult ) );
            }
            else
                rStrm << EXC_FORMULA_RES_EMPTY;     // BIFF8 only
            rStrm << sal_uInt8( 0 ) << sal_uInt32( 0 ) << sal_uInt16( 0xFFFF );
        }
        break;

        case NUMBERFORMAT_LOGICAL:
        {
            sal_uInt8 nXclValue = (mrScFmlaCell.GetValue() == 0.0) ? 0 : 1;
            rStrm << EXC_FORMULA_RES_BOOL << sal_uInt8( 0 )
                  << nXclValue << sal_uInt8( 0 ) << sal_uInt16( 0 )
                  << sal_uInt16( 0xFFFF );
        }
        break;

        default:
            rStrm << mrScFmlaCell.GetValue();
    }

    // flags
    sal_uInt16 nFlags = EXC_FORMULA_DEFAULTFLAGS;
    if( mxAddRec.is() )
        ::set_flag( nFlags, EXC_FORMULA_SHARED, mxAddRec->GetRecId() == EXC_ID_SHRFMLA );
    rStrm << nFlags << sal_uInt32( 0 );

    // formula token array
    if( mxAddRec.is() )
    {
        mxAddRec->WriteCellTokenArray( rStrm, GetXclCol(), GetXclRow() );
    }
    else
    {
        sal_uInt16 nFmlaSize = static_cast< sal_uInt16 >( maFmlaData.size() );
        rStrm << nFmlaSize;
        if( nFmlaSize )
            rStrm.Write( &maFmlaData.front(), nFmlaSize );
    }
}

// Multiple cell records ======================================================

XclExpMultiCellBase::XclExpMultiCellBase(
        sal_uInt16 nRecId, sal_uInt16 nMulRecId, sal_uInt32 nContSize,
        sal_uInt16 nXclCol, sal_uInt16 nXclRow ) :
    XclExpCellBase( nRecId, 0, nXclCol, nXclRow ),
    mnMulRecId( nMulRecId ),
    mnContSize( nContSize )
{
}

sal_uInt16 XclExpMultiCellBase::GetLastXclCol() const
{
    return GetXclCol() + GetCellCount() - 1;
}

sal_uInt32 XclExpMultiCellBase::GetFirstXFId() const
{
    return maXFIds.empty() ? XclExpXFBuffer::GetDefCellXFId() : maXFIds.front().mnXFId;
}

bool XclExpMultiCellBase::IsEmpty() const
{
    return maXFIds.empty();
}

void XclExpMultiCellBase::ConvertXFIndexes( const XclExpRoot& rRoot )
{
    for( XclExpMultiXFIdDeq::iterator aIt = maXFIds.begin(), aEnd = maXFIds.end(); aIt != aEnd; ++aIt )
        aIt->ConvertXFIndex( rRoot );
}

void XclExpMultiCellBase::Save( XclExpStream& rStrm )
{
    DBG_ASSERT_BIFF( rStrm.GetRoot().GetBiff() >= xlBiff3 );

    XclExpMultiXFIdDeq::const_iterator aEnd = maXFIds.end();
    XclExpMultiXFIdDeq::const_iterator aRangeBeg = maXFIds.begin();
    XclExpMultiXFIdDeq::const_iterator aRangeEnd = aRangeBeg;
    sal_uInt16 nBegXclCol = GetXclCol();
    sal_uInt16 nEndXclCol = nBegXclCol;

    while( aRangeEnd != aEnd )
    {
        // find begin of next used XF range
        aRangeBeg = aRangeEnd;
        nBegXclCol = nEndXclCol;
        while( (aRangeBeg != aEnd) && (aRangeBeg->mnXFIndex == EXC_XF_NOTFOUND) )
        {
            nBegXclCol += aRangeBeg->mnCount;
            ++aRangeBeg;
        }
        // find end of next used XF range
        aRangeEnd = aRangeBeg;
        nEndXclCol = nBegXclCol;
        while( (aRangeEnd != aEnd) && (aRangeEnd->mnXFIndex != EXC_XF_NOTFOUND) )
        {
            nEndXclCol += aRangeEnd->mnCount;
            ++aRangeEnd;
        }

        // export this range as a record
        if( aRangeBeg != aRangeEnd )
        {
            sal_uInt16 nCount = nEndXclCol - nBegXclCol;
            bool bIsMulti = nCount > 1;
            sal_uInt32 nTotalSize = GetRecSize() + (2 + mnContSize) * nCount;
            if( bIsMulti ) nTotalSize += 2;

            rStrm.StartRecord( bIsMulti ? mnMulRecId : GetRecId(), nTotalSize );
            rStrm << GetXclRow() << nBegXclCol;

            sal_uInt16 nRelCol = nBegXclCol - GetXclCol();
            for( XclExpMultiXFIdDeq::const_iterator aIt = aRangeBeg; aIt != aRangeEnd; ++aIt )
            {
                for( sal_uInt16 nIdx = 0; nIdx < aIt->mnCount; ++nIdx )
                {
                    rStrm << aIt->mnXFIndex;
                    WriteContents( rStrm, nRelCol );
                    ++nRelCol;
                }
            }
            if( bIsMulti )
                rStrm << static_cast< sal_uInt16 >( nEndXclCol - 1 );
            rStrm.EndRecord();
        }
    }
}

sal_uInt16 XclExpMultiCellBase::GetCellCount() const
{
    sal_uInt16 nCount = 0;
    for( XclExpMultiXFIdDeq::const_iterator aIt = maXFIds.begin(), aEnd = maXFIds.end(); aIt != aEnd; ++aIt )
        nCount += aIt->mnCount;
    return nCount;
}

void XclExpMultiCellBase::AppendXFId( const XclExpMultiXFId& rXFId )
{
    if( maXFIds.empty() || (maXFIds.back().mnXFId != rXFId.mnXFId) )
        maXFIds.push_back( rXFId );
    else
        maXFIds.back().mnCount += rXFId.mnCount;
}

void XclExpMultiCellBase::AppendXFId( const XclExpRoot& rRoot,
        const ScPatternAttr* pPattern, sal_uInt16 nScript, sal_uInt32 nForcedXFId, sal_uInt16 nCount )
{
    sal_uInt32 nXFId = (nForcedXFId == EXC_XFID_NOTFOUND) ?
        rRoot.GetXFBuffer().Insert( pPattern, nScript ) : nForcedXFId;
    AppendXFId( XclExpMultiXFId( nXFId, nCount ) );
}

bool XclExpMultiCellBase::TryMergeXFIds( const XclExpMultiCellBase& rCell )
{
    if( GetLastXclCol() + 1 == rCell.GetXclCol() )
    {
        maXFIds.insert( maXFIds.end(), rCell.maXFIds.begin(), rCell.maXFIds.end() );
        return true;
    }
    return false;
}

void XclExpMultiCellBase::GetXFIndexes( ScfUInt16Vec& rXFIndexes ) const
{
    DBG_ASSERT( GetLastXclCol() < rXFIndexes.size(), "XclExpMultiCellBase::GetXFIndexes - vector too small" );
    ScfUInt16Vec::iterator aDestIt = rXFIndexes.begin() + GetXclCol();
    for( XclExpMultiXFIdDeq::const_iterator aIt = maXFIds.begin(), aEnd = maXFIds.end(); aIt != aEnd; ++aIt )
    {
        ::std::fill( aDestIt, aDestIt + aIt->mnCount, aIt->mnXFIndex );
        aDestIt += aIt->mnCount;
    }
}

void XclExpMultiCellBase::RemoveUnusedXFIndexes( const ScfUInt16Vec& rXFIndexes )
{
    // save last column before calling maXFIds.clear()
    sal_uInt16 nLastXclCol = GetLastXclCol();
    DBG_ASSERT( nLastXclCol < rXFIndexes.size(), "XclExpMultiCellBase::RemoveUnusedXFIndexes - XF index vector too small" );

    // build new XF index vector, containing passed XF indexes
    maXFIds.clear();
    XclExpMultiXFId aXFId( 0 );
    for( ScfUInt16Vec::const_iterator aIt = rXFIndexes.begin() + GetXclCol(), aEnd = rXFIndexes.begin() + nLastXclCol + 1; aIt != aEnd; ++aIt )
    {
        // AppendXFId() tests XclExpXFIndex::mnXFId, set it too
        aXFId.mnXFId = aXFId.mnXFIndex = *aIt;
        AppendXFId( aXFId );
    }

    // remove leading and trailing unused XF indexes
    if( !maXFIds.empty() && (maXFIds.front().mnXFIndex == EXC_XF_NOTFOUND) )
    {
        SetXclCol( GetXclCol() + maXFIds.front().mnCount );
        maXFIds.pop_front();
    }
    if( !maXFIds.empty() && (maXFIds.back().mnXFIndex == EXC_XF_NOTFOUND) )
        maXFIds.pop_back();

    // The Save() function will skip all XF indexes equal to EXC_XF_NOTFOUND.
}

// ----------------------------------------------------------------------------

IMPL_FIXEDMEMPOOL_NEWDEL( XclExpBlankCell, 256, 256 )

XclExpBlankCell::XclExpBlankCell( sal_uInt16 nXclCol, sal_uInt16 nXclRow, const XclExpMultiXFId& rXFId ) :
    XclExpMultiCellBase( EXC_ID_BLANK, EXC_ID_MULBLANK, 0, nXclCol, nXclRow )
{
    DBG_ASSERT( rXFId.mnCount > 0, "XclExpBlankCell::XclExpBlankCell - invalid count" );
    AppendXFId( rXFId );
}

XclExpBlankCell::XclExpBlankCell( const XclExpRoot& rRoot,
        sal_uInt16 nXclCol, sal_uInt16 nLastXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId ) :
    XclExpMultiCellBase( EXC_ID_BLANK, EXC_ID_MULBLANK, 0, nXclCol, nXclRow )
{
    DBG_ASSERT( nXclCol <= nLastXclCol, "XclExpBlankCell::XclExpBlankCell - invalid column range" );
    AppendXFId( rRoot, pPattern, ::com::sun::star::i18n::ScriptType::WEAK, nForcedXFId, nLastXclCol - nXclCol + 1 );
}

bool XclExpBlankCell::TryMerge( const XclExpCellBase& rCell )
{
    if( const XclExpBlankCell* pBlankCell = dynamic_cast< const XclExpBlankCell* >( &rCell ) )
        if( TryMergeXFIds( *pBlankCell ) )
            return true;
    return false;
}

void XclExpBlankCell::GetBlankXFIndexes( ScfUInt16Vec& rXFIndexes ) const
{
    GetXFIndexes( rXFIndexes );
}

void XclExpBlankCell::RemoveUnusedBlankCells( const ScfUInt16Vec& rXFIndexes )
{
    RemoveUnusedXFIndexes( rXFIndexes );
}

void XclExpBlankCell::WriteContents( XclExpStream& rStrm, sal_uInt16 nRelCol )
{
}

// ----------------------------------------------------------------------------

IMPL_FIXEDMEMPOOL_NEWDEL( XclExpRkCell, 256, 256 )

XclExpRkCell::XclExpRkCell(
        const XclExpRoot& rRoot, sal_uInt16 nXclCol, sal_uInt16 nXclRow,
        const ScPatternAttr* pPattern, sal_uInt32 nForcedXFId, sal_Int32 nRkValue ) :
    XclExpMultiCellBase( EXC_ID_RK, EXC_ID_MULRK, 4, nXclCol, nXclRow )
{
    AppendXFId( rRoot, pPattern, ::com::sun::star::i18n::ScriptType::WEAK, nForcedXFId );
    maRkValues.push_back( nRkValue );
}

bool XclExpRkCell::TryMerge( const XclExpCellBase& rCell )
{
    if( const XclExpRkCell* pRkCell = dynamic_cast< const XclExpRkCell* >( &rCell ) )
    {
        if( TryMergeXFIds( *pRkCell ) )
        {
            maRkValues.insert( maRkValues.end(), pRkCell->maRkValues.begin(), pRkCell->maRkValues.end() );
            return true;
        }
    }
    return false;
}

void XclExpRkCell::WriteContents( XclExpStream& rStrm, sal_uInt16 nRelCol )
{
    DBG_ASSERT( nRelCol < maRkValues.size(), "XclExpRkCell::WriteContents - overflow error" );
    rStrm << maRkValues[ nRelCol ];
}

// ============================================================================
// Rows and Columns
// ============================================================================

XclExpOutlineBuffer::XclExpOutlineBuffer( const XclExpRoot& rRoot, bool bRows ) :
        mpScOLArray( 0 ),
        maLevelInfos( SC_OL_MAXDEPTH ),
        mnCurrLevel( 0 ),
        mbCurrCollapse( false )
{
    if( const ScOutlineTable* pOutlineTable = rRoot.GetDoc().GetOutlineTable( rRoot.GetCurrScTab() ) )
        mpScOLArray = bRows ? pOutlineTable->GetRowArray() : pOutlineTable->GetColArray();

    if( mpScOLArray )
        for( USHORT nLevel = 0; nLevel < SC_OL_MAXDEPTH; ++nLevel )
            if( ScOutlineEntry* pEntry = mpScOLArray->GetEntryByPos( nLevel, 0 ) )
                maLevelInfos[ nLevel ].mnScEndPos = pEntry->GetEnd();
}

void XclExpOutlineBuffer::UpdateColRow( SCCOLROW nScPos )
{
    if( mpScOLArray )
    {
        // find open level index for passed position
        USHORT nNewOpenScLevel = 0; // new open level (0-based Calc index)
        sal_uInt8 nNewLevel = 0;    // new open level (1-based Excel index)

        if( mpScOLArray->FindTouchedLevel( nScPos, nScPos, nNewOpenScLevel ) )
            nNewLevel = static_cast< sal_uInt8 >( nNewOpenScLevel + 1 );
        // else nNewLevel keeps 0 to show that there are no groups

        mbCurrCollapse = false;
        if( nNewLevel >= mnCurrLevel )
        {
            // new level(s) opened, or no level closed - update all level infos
            for( USHORT nScLevel = 0; nScLevel <= nNewOpenScLevel; ++nScLevel )
            {
                /*  In each level: check if a new group is started (there may be
                    neighbored groups without gap - therefore check ALL levels). */
                if( maLevelInfos[ nScLevel ].mnScEndPos < nScPos )
                {
                    if( ScOutlineEntry* pEntry = mpScOLArray->GetEntryByPos( nScLevel, nScPos ) )
                    {
                        maLevelInfos[ nScLevel ].mnScEndPos = pEntry->GetEnd();
                        maLevelInfos[ nScLevel ].mbHidden = pEntry->IsHidden();
                    }
                }
            }
        }
        else
        {
            // level(s) closed - check if any of the closed levels are collapsed
            // Calc uses 0-based level indexes
            USHORT nOldOpenScLevel = mnCurrLevel - 1;
            for( USHORT nScLevel = nNewOpenScLevel + 1; !mbCurrCollapse && (nScLevel <= nOldOpenScLevel); ++nScLevel )
                mbCurrCollapse = maLevelInfos[ nScLevel ].mbHidden;
        }

        // cache new opened level
        mnCurrLevel = nNewLevel;
    }
}

// ----------------------------------------------------------------------------

XclExpGuts::XclExpGuts( const XclExpRoot& rRoot ) :
    XclExpRecord( EXC_ID_GUTS, 8 ),
    mnColLevels( 0 ),
    mnColWidth( 0 ),
    mnRowLevels( 0 ),
    mnRowWidth( 0 )
{
    if( const ScOutlineTable* pOutlineTable = rRoot.GetDoc().GetOutlineTable( rRoot.GetCurrScTab() ) )
    {
        // column outline groups
        if( const ScOutlineArray* pColArray = pOutlineTable->GetColArray() )
            mnColLevels = ulimit_cast< sal_uInt16 >( pColArray->GetDepth(), EXC_OUTLINE_MAX );
        if( mnColLevels )
        {
            ++mnColLevels;
            mnColWidth = 12 * mnColLevels + 5;
        }

        // row outline groups
        if( const ScOutlineArray* pRowArray = pOutlineTable->GetRowArray() )
            mnRowLevels = ulimit_cast< sal_uInt16 >( pRowArray->GetDepth(), EXC_OUTLINE_MAX );
        if( mnRowLevels )
        {
            ++mnRowLevels;
            mnRowWidth = 12 * mnRowLevels + 5;
        }
    }
}

void XclExpGuts::WriteBody( XclExpStream& rStrm )
{
    rStrm << mnRowWidth << mnColWidth << mnRowLevels << mnColLevels;
}

// ----------------------------------------------------------------------------

XclExpDimensions::XclExpDimensions( const XclExpRoot& rRoot ) :
    mnFirstUsedXclRow( 0 ),
    mnFirstFreeXclRow( 0 ),
    mnFirstUsedXclCol( 0 ),
    mnFirstFreeXclCol( 0 )
{
    switch( rRoot.GetBiff() )
    {
        case xlBiff2:   SetRecHeader( EXC_ID2_DIMENSIONS, 8 );  break;
        case xlBiff3:
        case xlBiff4:
        case xlBiff5:
        case xlBiff7:   SetRecHeader( EXC_ID_DIMENSIONS, 10 );  break;
        case xlBiff8:   SetRecHeader( EXC_ID_DIMENSIONS, 14 );  break;
        default:        DBG_ERROR_BIFF();
    }
}

void XclExpDimensions::SetDimensions(
        sal_uInt16 nFirstUsedXclCol, sal_uInt32 nFirstUsedXclRow,
        sal_uInt16 nFirstFreeXclCol, sal_uInt32 nFirstFreeXclRow )
{
    mnFirstUsedXclRow = nFirstUsedXclRow;
    mnFirstFreeXclRow = nFirstFreeXclRow;
    mnFirstUsedXclCol = nFirstUsedXclCol;
    mnFirstFreeXclCol = nFirstFreeXclCol;
}

void XclExpDimensions::WriteBody( XclExpStream& rStrm )
{
    XclBiff eBiff = rStrm.GetRoot().GetBiff();
    if( eBiff >= xlBiff8 )
        rStrm << mnFirstUsedXclRow << mnFirstFreeXclRow;
    else
        rStrm << static_cast< sal_uInt16 >( mnFirstUsedXclRow ) << static_cast< sal_uInt16 >( mnFirstFreeXclRow );
    rStrm << mnFirstUsedXclCol << mnFirstFreeXclCol;
    if( eBiff >= xlBiff3 )
        rStrm << sal_uInt16( 0 );
}

// ============================================================================

namespace {

double lclGetCorrectedColWidth( const XclExpRoot& rRoot, sal_uInt16 nXclColWidth )
{
    long nFontHt = rRoot.GetFontBuffer().GetAppFontData().mnHeight;
    return nXclColWidth - XclTools::GetXclDefColWidthCorrection( nFontHt );
}

} // namespace

// ----------------------------------------------------------------------------

XclExpDefcolwidth::XclExpDefcolwidth( const XclExpRoot& rRoot ) :
    XclExpUInt16Record( EXC_ID_DEFCOLWIDTH, EXC_DEFCOLWIDTH_DEF ),
    XclExpRoot( rRoot )
{
}

bool XclExpDefcolwidth::IsDefWidth( sal_uInt16 nXclColWidth ) const
{
    double fNewColWidth = lclGetCorrectedColWidth( GetRoot(), nXclColWidth );
    // exactly matched, if difference is less than 1/16 of a character to the left or to the right
    return Abs( static_cast< long >( GetValue() * 256.0 - fNewColWidth + 0.5 ) ) < 16;
}

void XclExpDefcolwidth::SetDefWidth( sal_uInt16 nXclColWidth )
{
    double fNewColWidth = lclGetCorrectedColWidth( GetRoot(), nXclColWidth );
    SetValue( limit_cast< sal_uInt16 >( fNewColWidth / 256.0 + 0.5 ) );
}

// ----------------------------------------------------------------------------

XclExpColinfo::XclExpColinfo( const XclExpRoot& rRoot,
        SCCOL nScCol, SCROW nLastScRow, XclExpColOutlineBuffer& rOutlineBfr ) :
    XclExpRecord( EXC_ID_COLINFO, 12 ),
    XclExpRoot( rRoot ),
    mnWidth( 0 ),
    mnFlags( 0 ),
    mnFirstXclCol( static_cast< sal_uInt16 >( nScCol ) ),
    mnLastXclCol( static_cast< sal_uInt16 >( nScCol ) )
{
    ScDocument& rDoc = GetDoc();
    SCTAB nScTab = GetCurrScTab();

    // column default format
    maXFId.mnXFId = GetXFBuffer().Insert(
        rDoc.GetMostUsedPattern( nScCol, 0, nLastScRow, nScTab ),
        ::com::sun::star::i18n::ScriptType::WEAK );

    // column width
    USHORT nScWidth = rDoc.GetColWidth( nScCol, nScTab );
    mnWidth = XclTools::GetXclColumnWidth( nScWidth, GetCharWidth() );

    // column flags
    BYTE nScColFlags = rDoc.GetColFlags( nScCol, nScTab );
    ::set_flag( mnFlags, EXC_COLINFO_HIDDEN, (nScColFlags & CR_HIDDEN) != 0 );

    // outline data
    rOutlineBfr.Update( nScCol );
    ::set_flag( mnFlags, EXC_COLINFO_COLLAPSED, rOutlineBfr.IsCollapsed() );
    ::insert_value( mnFlags, rOutlineBfr.GetLevel(), 8, 3 );
}

sal_uInt16 XclExpColinfo::ConvertXFIndexes()
{
    maXFId.ConvertXFIndex( GetRoot() );
    return maXFId.mnXFIndex;
}

bool XclExpColinfo::IsDefault( const XclExpDefcolwidth& rDefColWidth ) const
{
    return (maXFId.mnXFIndex == EXC_XF_DEFAULTCELL) && (mnFlags == 0) && rDefColWidth.IsDefWidth( mnWidth );
}

bool XclExpColinfo::TryMerge( const XclExpColinfo& rColInfo )
{
    if( (maXFId.mnXFIndex == rColInfo.maXFId.mnXFIndex) &&
        (mnWidth == rColInfo.mnWidth) &&
        (mnFlags == rColInfo.mnFlags) &&
        (mnLastXclCol + 1 == rColInfo.mnFirstXclCol) )
    {
        mnLastXclCol = rColInfo.mnLastXclCol;
        return true;
    }
    return false;
}

void XclExpColinfo::WriteBody( XclExpStream& rStrm )
{
    // if last column is equal to last possible column, Excel adds one more
    sal_uInt16 nLastXclCol = mnLastXclCol;
    if( nLastXclCol == static_cast< sal_uInt16 >( rStrm.GetRoot().GetMaxPos().Col() ) )
        ++nLastXclCol;

    rStrm   << mnFirstXclCol
            << nLastXclCol
            << mnWidth
            << maXFId.mnXFIndex
            << mnFlags
            << sal_uInt16( 0 );
}

// ----------------------------------------------------------------------------

XclExpColinfoBuffer::XclExpColinfoBuffer( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    maDefcolwidth( rRoot ),
    maOutlineBfr( rRoot )
{
}

void XclExpColinfoBuffer::Initialize( SCROW nLastScRow )
{

    for( sal_uInt16 nScCol = 0, nLastScCol = GetMaxPos().Col(); nScCol <= nLastScCol; ++nScCol )
        maColInfos.AppendRecord( XclExpColinfoRef(
            new XclExpColinfo( GetRoot(), nScCol, nLastScRow, maOutlineBfr ) ) );
}

void XclExpColinfoBuffer::Finalize( ScfUInt16Vec& rXFIndexes )
{
    rXFIndexes.clear();
    rXFIndexes.reserve( maColInfos.Size() );

    size_t nPos, nSize;

    // do not cache the record list size, it may change in the loop
    for( nPos = 0; nPos < maColInfos.Size(); ++nPos )
    {
        XclExpColinfoRef xRec = maColInfos.GetRecord( nPos );
        xRec->ConvertXFIndexes();

        // try to merge with previous record
        if( nPos > 0 )
        {
            XclExpColinfoRef xPrevRec = maColInfos.GetRecord( nPos - 1 );
            if( xPrevRec->TryMerge( *xRec ) )
                // adjust nPos to get the next COLINFO record at the same position
                maColInfos.RemoveRecord( nPos-- );
        }
    }

    // put XF indexes into passed vector, collect use count of all different widths
    typedef ::std::map< sal_uInt16, sal_uInt16 > XclExpWidthMap;
    XclExpWidthMap aWidthMap;
    sal_uInt16 nMaxColCount = 0;
    sal_uInt16 nMaxUsedWidth = 0;
    for( nPos = 0, nSize = maColInfos.Size(); nPos < nSize; ++nPos )
    {
        const XclExpColinfoRef xRec = maColInfos.GetRecord( nPos );
        sal_uInt16 nColCount = xRec->GetColCount();

        // add XF index to passed vector
        rXFIndexes.resize( rXFIndexes.size() + nColCount, xRec->GetXFIndex() );

        // collect use count of column width
        sal_uInt16 nWidth = xRec->GetColWidth();
        sal_uInt16& rnMapCount = aWidthMap[ nWidth ];
        rnMapCount += nColCount;
        if( rnMapCount > nMaxColCount )
        {
            nMaxColCount = rnMapCount;
            nMaxUsedWidth = nWidth;
        }
    }
    maDefcolwidth.SetDefWidth( nMaxUsedWidth );

    // remove all default COLINFO records
    nPos = 0;
    while( nPos < maColInfos.Size() )
    {
        XclExpColinfoRef xRec = maColInfos.GetRecord( nPos );
        if( xRec->IsDefault( maDefcolwidth ) )
            maColInfos.RemoveRecord( nPos );
        else
            ++nPos;
    }
}

void XclExpColinfoBuffer::Save( XclExpStream& rStrm )
{
    // DEFCOLWIDTH
    maDefcolwidth.Save( rStrm );
    // COLINFO records
    maColInfos.Save( rStrm );
}

// ============================================================================

XclExpDefaultRowData::XclExpDefaultRowData() :
    mnFlags( EXC_DEFROW_DEFAULTFLAGS ),
    mnHeight( EXC_DEFROW_DEFAULTHEIGHT )
{
}

XclExpDefaultRowData::XclExpDefaultRowData( const XclExpRow& rRow ) :
    mnFlags( EXC_DEFROW_DEFAULTFLAGS ),
    mnHeight( rRow.GetHeight() )
{
    ::set_flag( mnFlags, EXC_DEFROW_HIDDEN, rRow.IsHidden() );
    ::set_flag( mnFlags, EXC_DEFROW_UNSYNCED, rRow.IsUnsynced() );
}

bool operator<( const XclExpDefaultRowData& rLeft, const XclExpDefaultRowData& rRight )
{
    return (rLeft.mnHeight < rRight.mnHeight) ||
        ((rLeft.mnHeight == rRight.mnHeight) && (rLeft.mnFlags < rRight.mnFlags));
}

// ----------------------------------------------------------------------------

XclExpDefrowheight::XclExpDefrowheight() :
    XclExpRecord( EXC_ID_DEFROWHEIGHT, 4 )
{
}

void XclExpDefrowheight::SetDefaultData( const XclExpDefaultRowData& rDefData )
{
    maDefData = rDefData;
}

void XclExpDefrowheight::WriteBody( XclExpStream& rStrm )
{
    DBG_ASSERT_BIFF( rStrm.GetRoot().GetBiff() >= xlBiff3 );
    rStrm << maDefData.mnFlags << maDefData.mnHeight;
}

// ----------------------------------------------------------------------------

XclExpRow::XclExpRow( const XclExpRoot& rRoot, sal_uInt16 nXclRow,
        XclExpRowOutlineBuffer& rOutlineBfr, bool bAlwaysEmpty ) :
    XclExpRecord( EXC_ID_ROW, 16 ),
    XclExpRoot( rRoot ),
    mnXclRow( nXclRow ),
    mnHeight( 0 ),
    mnFlags( EXC_ROW_DEFAULTFLAGS ),
    mnXFIndex( EXC_XF_DEFAULTCELL ),
    mbAlwaysEmpty( bAlwaysEmpty ),
    mbEnabled( true )
{
    SCTAB nScTab = GetCurrScTab();
    SCROW nScRow = static_cast< SCROW >( mnXclRow );

    // *** Row flags *** ------------------------------------------------------

    BYTE nRowFlags = GetDoc().GetRowFlags( nScRow, nScTab );
    bool bUserHeight = ::get_flag< BYTE >( nRowFlags, CR_MANUALSIZE );
    bool bHidden = ::get_flag< BYTE >( nRowFlags, CR_HIDDEN );
    ::set_flag( mnFlags, EXC_ROW_UNSYNCED, bUserHeight );
    ::set_flag( mnFlags, EXC_ROW_HIDDEN, bHidden );

    // *** Row height *** -----------------------------------------------------

    USHORT nScHeight = GetDoc().GetRowHeight( nScRow, nScTab );
    if( nScHeight == 0 )
    {
        ::set_flag( mnFlags, EXC_ROW_HIDDEN );
        mnHeight = EXC_ROW_DEFAULTHEIGHT;
    }
    else
    {
        // Calc and Excel use twips
        mnHeight = static_cast< sal_uInt16 >( nScHeight );
    }
    // #76250# not usable in Applix
//    ::set_flag( mnHeight, EXC_ROW_FLAGDEFHEIGHT, !bUserHeight );

    // *** Outline data *** ---------------------------------------------------

    rOutlineBfr.Update( nScRow );
    ::set_flag( mnFlags, EXC_ROW_COLLAPSED, rOutlineBfr.IsCollapsed() );
    ::insert_value( mnFlags, rOutlineBfr.GetLevel(), 0, 3 );

    // *** Progress bar *** ---------------------------------------------------

    XclExpProgressBar& rProgress = GetProgressBar();
    rProgress.IncRowRecordCount();
    rProgress.Progress();
}

void XclExpRow::AppendCell( XclExpCellRef xCell )
{
    DBG_ASSERT( !mbAlwaysEmpty, "XclExpRow::AppendCell - row is marked to be always empty" );
    // try to merge with last existing cell
    InsertCell( xCell, maCellList.Size() );
}

void XclExpRow::Finalize( const ScfUInt16Vec& rColXFIndexes )
{
    size_t nPos, nSize;

    // *** Convert XF identifiers *** -----------------------------------------

    // additionally collect the blank XF indexes
    size_t nColCount = GetMaxPos().Col() + 1;
    DBG_ASSERT( rColXFIndexes.size() == nColCount, "XclExpRow::Finalize - wrong column XF index count" );

    ScfUInt16Vec aXFIndexes( nColCount, EXC_XF_NOTFOUND );
    for( nPos = 0, nSize = maCellList.Size(); nPos < nSize; ++nPos )
    {
        XclExpCellRef xCell = maCellList.GetRecord( nPos );
        xCell->ConvertXFIndexes( GetRoot() );
        xCell->GetBlankXFIndexes( aXFIndexes );
    }

    // *** Fill gaps with BLANK/MULBLANK cell records *** ---------------------

    /*  This is needed because nonexistant cells in Calc are not formatted at all,
        but in Excel they would have the column default format. Blank cells that
        are equal to the respective column default are removed later in this function. */
    if( !mbAlwaysEmpty )
    {
        // XF identifier representing default cell XF
        XclExpMultiXFId aXFId( XclExpXFBuffer::GetDefCellXFId() );
        aXFId.ConvertXFIndex( GetRoot() );

        nPos = 0;
        while( nPos <= maCellList.Size() )  // don't cache list size, may change in the loop
        {
            // get column index that follows previous cell
            sal_uInt16 nFirstFreeXclCol = (nPos > 0) ? (maCellList.GetRecord( nPos - 1 )->GetLastXclCol() + 1) : 0;
            // get own column index
            sal_uInt16 nNextUsedXclCol = (nPos < maCellList.Size()) ? maCellList.GetRecord( nPos )->GetXclCol() : (GetMaxPos().Col() + 1);

            // is there a gap?
            if( nFirstFreeXclCol < nNextUsedXclCol )
            {
                aXFId.mnCount = nNextUsedXclCol - nFirstFreeXclCol;
                XclExpCellRef xNewCell( new XclExpBlankCell( nFirstFreeXclCol, mnXclRow, aXFId ) );
                // insert the cell, InsertCell() may merge it with existing BLANK records
                InsertCell( xNewCell, nPos );
                // insert default XF indexes into aXFIndexes
                ::std::fill( aXFIndexes.begin() + nFirstFreeXclCol,
                    aXFIndexes.begin() + nNextUsedXclCol, aXFId.mnXFIndex );
                // don't step forward with nPos, InsertCell() may remove records
            }
            else
                ++nPos;
        }
    }

    // *** Find default row format *** ----------------------------------------

    ScfUInt16Vec::iterator aCellBeg = aXFIndexes.begin(), aCellEnd = aXFIndexes.end(), aCellIt;
    ScfUInt16Vec::const_iterator aColBeg = rColXFIndexes.begin(), aColEnd = rColXFIndexes.end(), aColIt;

    // find most used XF index in the row
    typedef ::std::map< sal_uInt16, size_t > XclExpXFIndexMap;
    XclExpXFIndexMap aIndexMap;
    sal_uInt16 nRowXFIndex = EXC_XF_DEFAULTCELL;
    size_t nMaxXFCount = 0;
    for( aCellIt = aCellBeg; aCellIt != aCellEnd; ++aCellIt )
    {
        if( *aCellIt != EXC_XF_NOTFOUND )
        {
            size_t& rnCount = aIndexMap[ *aCellIt ];
            ++rnCount;
            if( rnCount > nMaxXFCount )
            {
                nRowXFIndex = *aCellIt;
                nMaxXFCount = rnCount;
            }
        }
    }

    // decide whether to use the row default XF index or column default XF indexes
    bool bUseColDefXFs = nRowXFIndex == EXC_XF_DEFAULTCELL;
    if( !bUseColDefXFs )
    {
        // count needed XF indexes for blank cells with and without row default XF index
        size_t nXFCountWithRowDefXF = 0;
        size_t nXFCountWithoutRowDefXF = 0;
        for( aCellIt = aCellBeg, aColIt = aColBeg; aCellIt != aCellEnd; ++aCellIt, ++aColIt )
        {
            sal_uInt16 nXFIndex = *aCellIt;
            if( nXFIndex != EXC_XF_NOTFOUND )
            {
                if( nXFIndex != nRowXFIndex )
                    ++nXFCountWithRowDefXF;     // with row default XF index
                if( nXFIndex != *aColIt )
                    ++nXFCountWithoutRowDefXF;  // without row default XF index
            }
        }

        // use column XF indexes if this would cause less or equal number of BLANK records
        bUseColDefXFs = nXFCountWithoutRowDefXF <= nXFCountWithRowDefXF;
    }

    // *** Remove unused BLANK cell records *** -------------------------------

    if( bUseColDefXFs )
    {
        // use column default XF indexes
        // #i194#: remove cell XF indexes equal to column default XF indexes
        for( aCellIt = aCellBeg, aColIt = aColBeg; aCellIt != aCellEnd; ++aCellIt, ++aColIt )
            if( *aCellIt == *aColIt )
                *aCellIt = EXC_XF_NOTFOUND;
    }
    else
    {
        // use row default XF index
        mnXFIndex = nRowXFIndex;
        ::set_flag( mnFlags, EXC_ROW_USEDEFXF );
        // #98133#, #i194#, #i27407#: remove cell XF indexes equal to row default XF index
        for( aCellIt = aCellBeg; aCellIt != aCellEnd; ++aCellIt )
            if( *aCellIt == nRowXFIndex )
                *aCellIt = EXC_XF_NOTFOUND;
    }

    // remove unused parts of BLANK/MULBLANK cell records
    nPos = 0;
    while( nPos < maCellList.Size() )   // do not cache list size, may change in the loop
    {
        XclExpCellRef xCell = maCellList.GetRecord( nPos );
        xCell->RemoveUnusedBlankCells( aXFIndexes );
        if( xCell->IsEmpty() )
            maCellList.RemoveRecord( nPos );
        else
            ++nPos;
    }

    // progress bar includes disabled rows
    GetProgressBar().Progress();
}

sal_uInt16 XclExpRow::GetFirstUsedXclCol() const
{
    return maCellList.Empty() ? 0 : maCellList.GetRecord( 0 )->GetXclCol();
}

sal_uInt16 XclExpRow::GetFirstFreeXclCol() const
{
    return maCellList.Empty() ? 0 : (maCellList.GetLastRecord()->GetLastXclCol() + 1);
}

bool XclExpRow::IsDefaultable() const
{
    const sal_uInt16 nAllowedFlags = EXC_ROW_DEFAULTFLAGS | EXC_ROW_HIDDEN | EXC_ROW_UNSYNCED;
    return !::get_flag< sal_uInt16 >( mnFlags, ~nAllowedFlags ) && IsEmpty();
}

void XclExpRow::DisableIfDefault( const XclExpDefaultRowData& rDefRowData )
{
    mbEnabled = !IsDefaultable() ||
        (mnHeight != rDefRowData.mnHeight) ||
        (IsHidden() != rDefRowData.IsHidden()) ||
        (IsUnsynced() != rDefRowData.IsUnsynced());
}

void XclExpRow::WriteCellList( XclExpStream& rStrm )
{
    DBG_ASSERT( mbEnabled || maCellList.Empty(), "XclExpRow::WriteCellList - cells in disabled row" );
    maCellList.Save( rStrm );
}

void XclExpRow::Save( XclExpStream& rStrm )
{
    if( mbEnabled )
        XclExpRecord::Save( rStrm );
}

void XclExpRow::InsertCell( XclExpCellRef xCell, size_t nPos )
{
    DBG_ASSERT( xCell, "XclExpRow::InsertCell - missing cell" );

    /*  #109751# If we have a multi-line text in a merged cell, and the resulting
        row height has not been confirmed, we need to force the EXC_ROW_UNSYNCED
        flag to be true to ensure Excel works correctly. */
    if( xCell->IsMultiLineText() )
        ::set_flag( mnFlags, EXC_ROW_UNSYNCED );

    // try to merge with previous cell, insert the new cell if not successful
    XclExpCellRef xPrevCell = maCellList.GetRecord( nPos - 1 );
    if( xPrevCell.is() && xPrevCell->TryMerge( *xCell ) )
        xCell = xPrevCell;
    else
        maCellList.InsertRecord( xCell, nPos++ );
    // nPos points now to following cell

    // try to merge with following cell, remove it if successful
    XclExpCellRef xNextCell = maCellList.GetRecord( nPos );
    if( xNextCell.is() && xCell->TryMerge( *xNextCell ) )
        maCellList.RemoveRecord( nPos );
}

void XclExpRow::WriteBody( XclExpStream& rStrm )
{
    rStrm   << mnXclRow
            << GetFirstUsedXclCol()
            << GetFirstFreeXclCol()
            << mnHeight
            << sal_uInt32( 0 )
            << mnFlags
            << mnXFIndex;
}

// ----------------------------------------------------------------------------

XclExpRowBuffer::XclExpRowBuffer( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    maOutlineBfr( rRoot ),
    maDimensions( rRoot ),
    mpLastUsedRow( 0 ),
    mnLastUsedXclRow( 0 )
{
}

void XclExpRowBuffer::AppendCell( XclExpCellRef xCell )
{
    DBG_ASSERT( xCell, "XclExpRowBuffer::AppendCell - missing cell" );
    GetOrCreateRow( xCell->GetXclRow(), false ).AppendCell( xCell );
}

void XclExpRowBuffer::CreateRows( SCROW nFirstFreeScRow )
{
    if( nFirstFreeScRow > 0 )
        GetOrCreateRow( static_cast< sal_uInt16 >( nFirstFreeScRow - 1 ), true );
}

void XclExpRowBuffer::Finalize( XclExpDefaultRowData& rDefRowData, const ScfUInt16Vec& rColXFIndexes )
{
    size_t nPos, nSize;

    // *** Finalize all rows *** ----------------------------------------------

    GetProgressBar().ActivateFinalRowsSegment();

    // unused blank cell records will be removed
    for( nPos = 0, nSize = maRowList.Size(); nPos < nSize; ++nPos )
        maRowList.GetRecord( nPos )->Finalize( rColXFIndexes );

    // *** Default row format *** ---------------------------------------------

    typedef ::std::map< XclExpDefaultRowData, size_t > XclExpDefRowDataMap;
    XclExpDefRowDataMap aDefRowMap;

    // find default row format for rows beyond used area
    sal_uInt32 nDefaultXclRow = maRowList.Empty() ? 0 : (maRowList.GetLastRecord()->GetXclRow() + 1);
    XclExpDefaultRowData aMaxDefData;
    size_t nMaxDefCount = 0;
    /*  #i30411# Files saved with SO7/OOo1.x with nonstandard default column
        formatting cause big Excel files, because all rows from row 1 to row
        32000 are exported. Now, if the used area goes exactly to row 32000,
        ignore all rows >32000. */
    if( nDefaultXclRow != 32000 )
    {
        sal_uInt16 nLastXclRow = static_cast< sal_uInt16 >( GetMaxPos().Row() );
        if( nDefaultXclRow <= nLastXclRow )
        {
            // create a dummy ROW record and fill aMaxDefData
            XclExpRowOutlineBuffer aOutlineBfr( GetRoot() );
            XclExpRow aRow( GetRoot(), nLastXclRow, aOutlineBfr, true );
            aMaxDefData = XclExpDefaultRowData( aRow );
            aDefRowMap[ aMaxDefData ] = nMaxDefCount =
                static_cast< size_t >( nLastXclRow - nDefaultXclRow + 1 );
        }
    }

    // only look for default format in existing rows, if there are more than unused
    nSize = maRowList.Size();
    if( nMaxDefCount < nSize )
    {
        for( nPos = 0; nPos < nSize; ++nPos )
        {
            XclExpRowRef xRow = maRowList.GetRecord( nPos );
            /*  Collect formats of unused rows (rows without cells), which are able
                to be defaulted (i.e. no explicit format or outline level). */
            if( xRow->IsDefaultable() )
            {
                XclExpDefaultRowData aDefData( *xRow );
                size_t& rnDefCount = aDefRowMap[ aDefData ];
                ++rnDefCount;
                if( rnDefCount > nMaxDefCount )
                {
                    nMaxDefCount = rnDefCount;
                    aMaxDefData = aDefData;
                }
            }
        }
    }

    // return the default row format to caller
    rDefRowData = aMaxDefData;

    // *** Disable unused ROW records, find used area *** ---------------------

    sal_uInt16 nFirstUsedXclCol = SAL_MAX_UINT16;
    sal_uInt16 nFirstFreeXclCol = 0;
    sal_uInt32 nFirstUsedXclRow = SAL_MAX_UINT32;
    sal_uInt32 nFirstFreeXclRow = 0;

    for( nPos = 0, nSize = maRowList.Size(); nPos < nSize; ++nPos )
    {
        XclExpRowRef xRow = maRowList.GetRecord( nPos );

        // disable unused rows
        xRow->DisableIfDefault( aMaxDefData );

        // find used column range
        if( !xRow->IsEmpty() )      // empty rows return (0...0) as used range
        {
            nFirstUsedXclCol = ::std::min( nFirstUsedXclCol, xRow->GetFirstUsedXclCol() );
            nFirstFreeXclCol = ::std::max( nFirstFreeXclCol, xRow->GetFirstFreeXclCol() );
        }

        // find used row range
        if( xRow->IsEnabled() )
        {
            sal_uInt16 nXclRow = xRow->GetXclRow();
            nFirstUsedXclRow = ::std::min< sal_uInt32 >( nFirstUsedXclRow, nXclRow );
            nFirstFreeXclRow = ::std::max< sal_uInt32 >( nFirstFreeXclRow, nXclRow + 1 );
        }
    }

    // adjust start position, if there are no or only empty/disabled ROW records
    nFirstUsedXclCol = ::std::min( nFirstUsedXclCol, nFirstFreeXclCol );
    nFirstUsedXclRow = ::std::min( nFirstUsedXclRow, nFirstFreeXclRow );

    // initialize the DIMENSIONS record
    maDimensions.SetDimensions(
        nFirstUsedXclCol, nFirstUsedXclRow, nFirstFreeXclCol, nFirstFreeXclRow );
}

void XclExpRowBuffer::Save( XclExpStream& rStrm )
{
    // DIMENSIONS record
    maDimensions.Save( rStrm );

    // save in blocks of 32 rows, each block contains first all ROWs, then all cells
    size_t nSize = maRowList.Size();
    size_t nBlockStart = 0;
    sal_uInt16 nStartXclRow = (nSize == 0) ? 0 : maRowList.GetRecord( 0 )->GetXclRow();

    while( nBlockStart < nSize )
    {
        // find end of row block
        size_t nBlockEnd = nBlockStart + 1;
        while( (nBlockEnd < nSize) && (maRowList.GetRecord( nBlockEnd )->GetXclRow() - nStartXclRow < EXC_ROW_ROWBLOCKSIZE) )
            ++nBlockEnd;

        // write the ROW records
        size_t nPos;
        for( nPos = nBlockStart; nPos < nBlockEnd; ++nPos )
            maRowList.GetRecord( nPos )->Save( rStrm );

        // write the cell records
        for( nPos = nBlockStart; nPos < nBlockEnd; ++nPos )
            maRowList.GetRecord( nPos )->WriteCellList( rStrm );

        nBlockStart = nBlockEnd;
        nStartXclRow += EXC_ROW_ROWBLOCKSIZE;
    }
}

XclExpRow& XclExpRowBuffer::GetOrCreateRow( sal_uInt16 nXclRow, bool bRowAlwaysEmpty )
{
    if( !mpLastUsedRow || (mnLastUsedXclRow != nXclRow) )
    {
        // fill up missing ROW records
        // do not use sal_uInt16 for nFirstFreeXclRow, would cause loop in full sheets
        for( size_t nFirstFreeXclRow = maRowList.Size(); nFirstFreeXclRow <= nXclRow; ++nFirstFreeXclRow )
            maRowList.AppendRecord( XclExpRowRef( new XclExpRow(
                GetRoot(), static_cast< sal_uInt16 >( nFirstFreeXclRow ), maOutlineBfr, bRowAlwaysEmpty ) ) );

        mpLastUsedRow = maRowList.GetRecord( nXclRow ).get();
        mnLastUsedXclRow = nXclRow;
    }
    return *mpLastUsedRow;
}

// ============================================================================
// Cell Table
// ============================================================================

XclExpCellTable::XclExpCellTable( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    maColInfoBfr( rRoot ),
    maRowBfr( rRoot ),
    maShrfmlaBfr( rRoot ),
    maTableopBfr( rRoot ),
    mxDefrowheight( new XclExpDefrowheight ),
    mxGuts( new XclExpGuts( rRoot ) ),
    mxNoteList( new XclExpNoteList ),
    mxMergedcells( new XclExpMergedcells( rRoot ) ),
    mxHyperlinkList( new XclExpHyperlinkList ),
    mxDval( new XclExpDval( rRoot ) )
{
    ScDocument& rDoc = GetDoc();
    SCTAB nScTab = GetCurrScTab();
    SvNumberFormatter& rFormatter = GetFormatter();

    // maximum sheet limits
    SCCOL nMaxScCol = GetMaxPos().Col();
    SCROW nMaxScRow = GetMaxPos().Row();

    // find used area (non-empty cells)
    SCCOL nLastUsedScCol;
    SCROW nLastUsedScRow;
    rDoc.GetTableArea( nScTab, nLastUsedScCol, nLastUsedScRow );

    ScRange aUsedRange( 0, 0, nScTab, nLastUsedScCol, nLastUsedScRow, nScTab );
    CheckCellRange( aUsedRange );
    nLastUsedScCol = aUsedRange.aEnd.Col();
    nLastUsedScRow = aUsedRange.aEnd.Row();

    // first row without any set attributes (height/hidden/...)
    SCROW nFirstUnflaggedScRow = rDoc.GetLastFlaggedRow( nScTab ) + 1;

    // find range of outlines
    SCROW nFirstUngroupedScRow = 0;
    if( const ScOutlineTable* pOutlineTable = rDoc.GetOutlineTable( nScTab ) )
    {
        SCCOLROW nScStartPos, nScEndPos;
        if( const ScOutlineArray* pRowArray = pOutlineTable->GetRowArray() )
        {
            pRowArray->GetRange( nScStartPos, nScEndPos );
            // +1 because open/close button is in next row in Excel, +1 for "end->first unused"
            nFirstUngroupedScRow = static_cast< SCROW >( nScEndPos + 2 );
        }
    }

    // column settings
    /*  #i30411# Files saved with SO7/OOo1.x with nonstandard default column
        formatting cause big Excel files, because all rows from row 1 to row
        32000 are exported. Now, if the used area goes exactly to row 32000,
        use this row as default and ignore all rows >32000. */
    if( (nLastUsedScRow == 31999) && (nFirstUnflaggedScRow < 32000) && (nFirstUngroupedScRow <= 32000) )
        nMaxScRow = 31999;
    maColInfoBfr.Initialize( nMaxScRow );

    // range for cell iterator
    SCCOL nLastIterScCol = nMaxScCol;
    SCROW nLastIterScRow = ulimit_cast< SCROW >( nLastUsedScRow + 128, nMaxScRow );
    ScUsedAreaIterator aIt( &rDoc, nScTab, 0, 0, nLastIterScCol, nLastIterScRow );

    // activate the correct segment and sub segment at the progress bar
    GetProgressBar().ActivateCreateRowsSegment();

    for( bool bIt = aIt.GetNext(); bIt; bIt = aIt.GetNext() )
    {
        SCCOL nScCol = aIt.GetStartCol();
        SCROW nScRow = aIt.GetRow();
        SCCOL nLastScCol = aIt.GetEndCol();
        ScAddress aScPos( nScCol, nScRow, nScTab );

        sal_uInt16 nXclCol = static_cast< sal_uInt16 >( nScCol );
        sal_uInt16 nLastXclCol = static_cast< sal_uInt16 >( nLastScCol );
        sal_uInt16 nXclRow = static_cast< sal_uInt16 >( nScRow );

        const ScBaseCell* pScCell = aIt.GetCell();
        XclExpCellRef xCell;

        const ScPatternAttr* pPattern = aIt.GetPattern();

        // handle overlapped merged cells before creating the cell record
        sal_uInt32 nMergeBaseXFId = EXC_XFID_NOTFOUND;
        if( pPattern )
        {
            // overlapped cell in a merged range
            const ScMergeFlagAttr& rMergeFlagItem = GETITEM( pPattern->GetItemSet(), ScMergeFlagAttr, ATTR_MERGE_FLAG );
            // in Excel all merged cells must contain same XF index, for correct border
            if( rMergeFlagItem.IsOverlapped() )
                nMergeBaseXFId = mxMergedcells->GetBaseXFId( aScPos );
        }

        String aAddNoteText;    // additional text to be appended to a note

        CellType eCellType = pScCell ? pScCell->GetCellType() : CELLTYPE_NONE;
        switch( eCellType )
        {
            case CELLTYPE_VALUE:
            {
                double fValue = static_cast< const ScValueCell* >( pScCell )->GetValue();

                // try to create a Boolean cell
                if( pPattern && ((fValue == 0.0) || (fValue == 1.0)) )
                {
                    ULONG nScNumFmt = GETITEMVALUE( pPattern->GetItemSet(), SfxUInt32Item, ATTR_VALUE_FORMAT, ULONG );
                    if( rFormatter.GetType( nScNumFmt ) == NUMBERFORMAT_LOGICAL )
                        xCell.reset( new XclExpBooleanCell(
                            GetRoot(), nXclCol, nXclRow, pPattern, nMergeBaseXFId, fValue != 0.0 ) );
                }

                // try to create an RK value (compressed floating-point number)
                sal_Int32 nRkValue;
                if( !xCell && XclTools::GetRKFromDouble( nRkValue, fValue ) )
                    xCell.reset( new XclExpRkCell(
                        GetRoot(), nXclCol, nXclRow, pPattern, nMergeBaseXFId, nRkValue ) );

                // else: simple floating-point number cell
                if( !xCell )
                    xCell.reset( new XclExpNumberCell(
                        GetRoot(), nXclCol, nXclRow, pPattern, nMergeBaseXFId, fValue ) );
            }
            break;

            case CELLTYPE_STRING:
            {
                const ScStringCell& rScStrCell = *static_cast< const ScStringCell* >( pScCell );
                xCell.reset( new XclExpLabelCell(
                    GetRoot(), nXclCol, nXclRow, pPattern, nMergeBaseXFId, rScStrCell ) );
            }
            break;

            case CELLTYPE_EDIT:
            {
                const ScEditCell& rScEditCell = *static_cast< const ScEditCell* >( pScCell );
                XclExpHyperlinkHelper aLinkHelper( GetRoot(), aScPos );
                xCell.reset( new XclExpLabelCell(
                    GetRoot(), nXclCol, nXclRow, pPattern, nMergeBaseXFId, rScEditCell, aLinkHelper ) );

                // add a single created HLINK record to the record list
                if( aLinkHelper.HasLinkRecord() )
                    mxHyperlinkList->AppendRecord( aLinkHelper.GetLinkRecord() );
                // add list of multiple URLs to the additional cell note text
                if( aLinkHelper.HasMultipleUrls() )
                    ScGlobal::AddToken( aAddNoteText, aLinkHelper.GetUrlList(), '\n', 2 );
            }
            break;

            case CELLTYPE_FORMULA:
            {
                const ScFormulaCell& rScFmlaCell = *static_cast< const ScFormulaCell* >( pScCell );
                xCell.reset( new XclExpFormulaCell(
                    GetRoot(), nXclCol, nXclRow, pPattern, nMergeBaseXFId,
                    rScFmlaCell, maShrfmlaBfr, maTableopBfr ) );
            }
            break;

            default:
                DBG_ERRORFILE( "XclExpCellTable::XclExpCellTable - unknown cell type" );
                // run-through!
            case CELLTYPE_NONE:
            case CELLTYPE_NOTE:
            {
                xCell.reset( new XclExpBlankCell(
                    GetRoot(), nXclCol, nLastXclCol, nXclRow, pPattern, nMergeBaseXFId ) );
            }
            break;
        }

        // insert the cell into the current row
        if( xCell.is() )
            maRowBfr.AppendCell( xCell );

        // notes
        const ScPostIt* pScNote = pScCell ? pScCell->GetNotePtr() : 0;
        if( pScNote || aAddNoteText.Len() )
            mxNoteList->AppendRecord( XclExpNoteList::RecordRefType(
                new XclExpNote( GetRoot(), aScPos, pScNote, aAddNoteText ) ) );

        // other sheet contents
        if( pPattern )
        {
            const SfxItemSet& rItemSet = pPattern->GetItemSet();

            // base cell in a merged range
            const ScMergeAttr& rMergeItem = GETITEM( rItemSet, ScMergeAttr, ATTR_MERGE );
            if( rMergeItem.IsMerged() )
            {
                ScRange aScRange( aScPos );
                aScRange.aEnd.IncCol( rMergeItem.GetColMerge() - 1 );
                aScRange.aEnd.IncRow( rMergeItem.GetRowMerge() - 1 );
                sal_uInt32 nXFId = xCell.is() ? xCell->GetFirstXFId() : EXC_XFID_NOTFOUND;
                mxMergedcells->AppendRange( aScRange, nXFId );
            }

            // data validation
            if( ScfTools::CheckItem( rItemSet, ATTR_VALIDDATA, false ) )
            {
                ULONG nScHandle = GETITEMVALUE( rItemSet, SfxUInt32Item, ATTR_VALIDDATA, ULONG );
                ScRange aScRange( aScPos );
                aScRange.aEnd.SetCol( nLastScCol );
                mxDval->InsertCellRange( aScRange, nScHandle );
            }
        }
    }

    // create missing row settings for rows anyhow flagged or with outlines
    maRowBfr.CreateRows( ::std::max( nFirstUnflaggedScRow, nFirstUngroupedScRow ) );
}

void XclExpCellTable::Finalize()
{
    // Finalize multiple operations.
    maTableopBfr.Finalize();

    /*  Finalize column buffer. This calculates column default XF indexes from
        the XF identifiers and fills a vector with these XF indexes. */
    ScfUInt16Vec aColXFIndexes;
    maColInfoBfr.Finalize( aColXFIndexes );

    /*  Finalize row buffer. This calculates all cell XF indexes from the XF
        identifiers. Then the XF index vector aColXFIndexes (filled above) is
        used to calculate the row default formats. With this, all unneeded blank
        cell records (equal to row default or column default) will be removed.
        The function returns the (most used) default row format in aDefRowData. */
    XclExpDefaultRowData aDefRowData;
    maRowBfr.Finalize( aDefRowData, aColXFIndexes );

    // Initialize the DEFROWHEIGHT record.
    mxDefrowheight->SetDefaultData( aDefRowData );
}

XclExpRecordRef XclExpCellTable::CreateRecord( sal_uInt16 nRecId ) const
{
    XclExpRecordRef xRec;
    switch( nRecId )
    {
        case EXC_ID_DEFROWHEIGHT:   xRec = mxDefrowheight;  break;
        case EXC_ID_GUTS:           xRec = mxGuts;          break;
        case EXC_ID_NOTE:           xRec = mxNoteList;      break;
        case EXC_ID_MERGEDCELLS:    xRec = mxMergedcells;   break;
        case EXC_ID_HLINK:          xRec = mxHyperlinkList; break;
        case EXC_ID_DVAL:           xRec = mxDval;          break;
        default:    DBG_ERRORFILE( "XclExpCellTable::CreateRecord - unknown record id" );
    }
    return xRec;
}

void XclExpCellTable::Save( XclExpStream& rStrm )
{
    // DEFCOLWIDTH and COLINFOs
    maColInfoBfr.Save( rStrm );
    // ROWs and cell records
    maRowBfr.Save( rStrm );
}

// ============================================================================

