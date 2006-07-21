/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xipivot.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-21 12:12:34 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"

#ifndef SC_XIPIVOT_HXX
#include "xipivot.hxx"
#endif

#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDSORTINFO_HPP_
#include <com/sun/star/sheet/DataPilotFieldSortInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDAUTOSHOWINFO_HPP_
#include <com/sun/star/sheet/DataPilotFieldAutoShowInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDLAYOUTINFO_HPP_
#include <com/sun/star/sheet/DataPilotFieldLayoutInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDREFERENCE_HPP_
#include <com/sun/star/sheet/DataPilotFieldReference.hpp>
#endif

#ifndef _DATETIME_HXX
#include <tools/datetime.hxx>
#endif
#ifndef _ZFORMAT_HXX
#include <svtools/zformat.hxx>
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_DPSAVE_HXX
#include "dpsave.hxx"
#endif
#ifndef SC_DPDIMSAVE_HXX
#include "dpdimsave.hxx"
#endif
#ifndef SC_DPOBJECT_HXX
#include "dpobject.hxx"
#endif
#ifndef SC_DPSHTTAB_HXX
#include "dpshttab.hxx"
#endif

#ifndef SC_XLTRACER_HXX
#include "xltracer.hxx"
#endif
#ifndef SC_XISTREAM_HXX
#include "xistream.hxx"
#endif
#ifndef SC_XIHELPER_HXX
#include "xihelper.hxx"
#endif
#ifndef SC_XILINK_HXX
#include "xilink.hxx"
#endif
#ifndef SC_XIESCHER_HXX
#include "xiescher.hxx"
#endif

using ::rtl::OUString;
using ::com::sun::star::sheet::DataPilotFieldOrientation;
using ::com::sun::star::sheet::DataPilotFieldOrientation_DATA;
using ::com::sun::star::sheet::DataPilotFieldSortInfo;
using ::com::sun::star::sheet::DataPilotFieldAutoShowInfo;
using ::com::sun::star::sheet::DataPilotFieldLayoutInfo;
using ::com::sun::star::sheet::DataPilotFieldReference;

// ============================================================================
// Pivot cache
// ============================================================================

XclImpPCItem::XclImpPCItem( XclImpStream& rStrm )
{
    switch( rStrm.GetRecId() )
    {
        case EXC_ID_SXDOUBLE:   ReadSxdouble( rStrm );      break;
        case EXC_ID_SXBOOLEAN:  ReadSxboolean( rStrm );     break;
        case EXC_ID_SXERROR:    ReadSxerror( rStrm );       break;
        case EXC_ID_SXINTEGER:  ReadSxinteger( rStrm );     break;
        case EXC_ID_SXSTRING:   ReadSxstring( rStrm );      break;
        case EXC_ID_SXDATETIME: ReadSxdatetime( rStrm );    break;
        case EXC_ID_SXEMPTY:    ReadSxempty( rStrm );       break;
        default:    DBG_ERRORFILE( "XclImpPCItem::XclImpPCItem - unknown record id" );
    }
}

void XclImpPCItem::ReadSxdouble( XclImpStream& rStrm )
{
    DBG_ASSERT( rStrm.GetRecSize() == 8, "XclImpPCItem::ReadSxdouble - wrong record size" );
    SetDouble( rStrm.ReadDouble() );
}

void XclImpPCItem::ReadSxboolean( XclImpStream& rStrm )
{
    DBG_ASSERT( rStrm.GetRecSize() == 2, "XclImpPCItem::ReadSxboolean - wrong record size" );
    SetBool( rStrm.ReaduInt16() != 0 );
}

void XclImpPCItem::ReadSxerror( XclImpStream& rStrm )
{
    DBG_ASSERT( rStrm.GetRecSize() == 2, "XclImpPCItem::ReadSxerror - wrong record size" );
    SetError( rStrm.ReaduInt16() );
}

void XclImpPCItem::ReadSxinteger( XclImpStream& rStrm )
{
    DBG_ASSERT( rStrm.GetRecSize() == 2, "XclImpPCItem::ReadSxinteger - wrong record size" );
    SetInteger( rStrm.ReadInt16() );
}

void XclImpPCItem::ReadSxstring( XclImpStream& rStrm )
{
    DBG_ASSERT( rStrm.GetRecSize() >= 3, "XclImpPCItem::ReadSxstring - wrong record size" );
    SetText( rStrm.ReadUniString() );
}

void XclImpPCItem::ReadSxdatetime( XclImpStream& rStrm )
{
    DBG_ASSERT( rStrm.GetRecSize() == 8, "XclImpPCItem::ReadSxdatetime - wrong record size" );

    sal_uInt16 nYear, nMonth;
    sal_uInt8 nDay, nHour, nMin, nSec;
    rStrm >> nYear >> nMonth >> nDay >> nHour >> nMin >> nSec;

    DateTime aNullDate( *rStrm.GetRoot().GetFormatter().GetNullDate() );
    DateTime aDate( Date( nDay, nMonth, nYear ), Time( nHour, nMin, nSec ) );
    SetDate( aDate - aNullDate );
}

void XclImpPCItem::ReadSxempty( XclImpStream& rStrm )
{
    DBG_ASSERT( rStrm.GetRecSize() == 0, "XclImpPCItem::ReadSxempty - wrong record size" );
    SetEmpty();
}

// ============================================================================

XclImpPCField::XclImpPCField( XclImpStream& rStrm, XclImpPivotCache& rPCache, sal_uInt16 nFieldIdx ) :
    XclPCField( EXC_PCFIELD_UNKNOWN, nFieldIdx ),
    mrPCache( rPCache ),
    mnTotalItemCount( 0 ),
    mbNumGroupInfoRead( false )
{
    ReadSxfield( rStrm );
}

XclImpPCField::~XclImpPCField()
{
}

// general field/item access --------------------------------------------------

const String& XclImpPCField::GetFieldName( const ScfStringVec& rVisNames ) const
{
    if( IsGroupChildField() && (mnFieldIdx < rVisNames.size()) )
    {
        const String& rVisName = rVisNames[ mnFieldIdx ];
        if( rVisName.Len() )
            return rVisName;
    }
    return maFieldInfo.maName;
}

const XclImpPCField* XclImpPCField::GetGroupBaseField() const
{
    DBG_ASSERT( IsGroupChildField(), "XclImpPCField::GetGroupBaseField - this field type does not have a base field" );
    return IsGroupChildField() ? mrPCache.GetField( maFieldInfo.mnGroupBase ) : 0;
}

sal_uInt16 XclImpPCField::GetItemCount() const
{
    return static_cast< sal_uInt16 >( maItemList.Count() );
}

const XclImpPCItem* XclImpPCField::GetItem( sal_uInt16 nItemIdx ) const
{
    return maItemList.GetObject( nItemIdx );
}

const XclImpPCItem* XclImpPCField::GetLimitItem( sal_uInt16 nItemIdx ) const
{
    DBG_ASSERT( nItemIdx < 3, "XclImpPCField::GetLimitItem - invalid item index" );
    DBG_ASSERT( nItemIdx < maNumGroupItemList.Count(), "XclImpPCField::GetLimitItem - no item found" );
    return maNumGroupItemList.GetObject( nItemIdx );
}

// records --------------------------------------------------------------------

void XclImpPCField::ReadSxfield( XclImpStream& rStrm )
{
    rStrm >> maFieldInfo;

    /*  Detect the type of this field. This is done very restrictive to detect
        any unexpected state. */

    bool bItems  = ::get_flag( maFieldInfo.mnFlags, EXC_SXFIELD_HASITEMS );
    bool bPostp  = ::get_flag( maFieldInfo.mnFlags, EXC_SXFIELD_POSTPONE );
    bool bCalced = ::get_flag( maFieldInfo.mnFlags, EXC_SXFIELD_CALCED );
    bool bChild  = ::get_flag( maFieldInfo.mnFlags, EXC_SXFIELD_HASCHILD );
    bool bNum    = ::get_flag( maFieldInfo.mnFlags, EXC_SXFIELD_NUMGROUP );

    sal_uInt16 nVisC   = maFieldInfo.mnVisItems;
    sal_uInt16 nGroupC = maFieldInfo.mnGroupItems;
    sal_uInt16 nBaseC  = maFieldInfo.mnBaseItems;
    sal_uInt16 nOrigC  = maFieldInfo.mnOrigItems;

    sal_uInt16 nType = maFieldInfo.mnFlags & EXC_SXFIELD_DATA_MASK;
    bool bType =
        (nType == EXC_SXFIELD_DATA_STR) ||
        (nType == EXC_SXFIELD_DATA_INT) ||
        (nType == EXC_SXFIELD_DATA_DBL) ||
        (nType == EXC_SXFIELD_DATA_STR_INT) ||
        (nType == EXC_SXFIELD_DATA_STR_DBL) ||
        (nType == EXC_SXFIELD_DATA_DATE) ||
        (nType == EXC_SXFIELD_DATA_DATE_NUM) ||
        (nType == EXC_SXFIELD_DATA_DATE_STR);
    bool bTypeNone =
        (nType == EXC_SXFIELD_DATA_NONE);
    // for now, ignore data type of calculated fields
    DBG_ASSERT( bCalced || bType || bTypeNone, "XclImpPCField::ReadSxfield - unknown item data type" );

    meFieldType = EXC_PCFIELD_UNKNOWN;

    bool bItemCount = (bItems && nVisC) || (bPostp && !nOrigC);
    DBG_ASSERT( bItemCount, "XclImpPCField::ReadSxfield - field without items" );

    if( bItemCount )
    {
        // 1) standard field without grouping
        if( !bNum && !nGroupC && !nBaseC && (bPostp || (nOrigC == nVisC)) && bType )
            meFieldType = EXC_PCFIELD_STANDARD;

        // 2) standard grouping field
        else if( !bNum && (nGroupC == nVisC) && nBaseC && !nOrigC && bTypeNone )
            meFieldType = EXC_PCFIELD_STDGROUP;

        // 3) numerical grouping fields
        else if( bNum )
        {
            // 3a) single date grouping field without child grouping field
            if( !bChild && (nGroupC == nVisC) && !nBaseC && nOrigC && bType )
            {
                switch( nType )
                {
                    case EXC_SXFIELD_DATA_INT:
                    case EXC_SXFIELD_DATA_DBL:  meFieldType = EXC_PCFIELD_NUMGROUP;     break;
                    case EXC_SXFIELD_DATA_DATE: meFieldType = EXC_PCFIELD_DATEGROUP;    break;
                    default:    DBG_ERRORFILE( "XclImpPCField::ReadSxfield - numeric group with wrong data type" );
                }
            }

            // 3b) first date grouping field with child grouping field
            else if( bChild && (nGroupC == nVisC) && !nBaseC && nOrigC && (nType == EXC_SXFIELD_DATA_DATE) )
                meFieldType = EXC_PCFIELD_DATEGROUP;

            // 3c) additional date grouping field
            else if( (nGroupC == nVisC) && !nBaseC && !nOrigC && bTypeNone )
                meFieldType = EXC_PCFIELD_DATECHILD;
        }

        // 4) calculated field
        else if( bCalced )
            meFieldType = EXC_PCFIELD_CALCED;

        DBG_ASSERT( meFieldType != EXC_PCFIELD_UNKNOWN, "XclImpPCField::ReadSxfield - unknown state of SXFIELD" );
    }

    // never seen postponed items in a grouping field
    DBG_ASSERT( IsStandardField() || !bPostp, "XclImpPCField::ReadSxfield - postponed items in a nonstandard field" );
}

void XclImpPCField::ReadItem( XclImpStream& rStrm )
{
    bool bPostp = HasPostponedItems();
    if( bPostp || (GetItemCount() < maFieldInfo.mnVisItems) )
    {
        maItemList.Append( new XclImpPCItem( rStrm ) );
        if( bPostp ) ++maFieldInfo.mnVisItems;
    }
    else if( (IsNumGroupField() || IsDateGroupField()) && mbNumGroupInfoRead && (maNumGroupItemList.Count() < 3) )
    {
        maNumGroupItemList.Append( new XclImpPCItem( rStrm ) );
    }
    // else ignore the record
    ++mnTotalItemCount;
}

void XclImpPCField::ReadSxnumgroup( XclImpStream& rStrm )
{
    DBG_ASSERT( IsNumGroupField() || IsDateGroupField(), "XclImpPCField::ReadSxnumgroup - SXNUMGROUP outside numeric grouping field" );
    DBG_ASSERT( !mbNumGroupInfoRead, "XclImpPCField::ReadSxnumgroup - multiple SXNUMGROUP records" );
    DBG_ASSERT( mnTotalItemCount == maFieldInfo.mnVisItems, "XclImpPCField::ReadSxnumgroup - SXNUMGROUP out of record order" );
    rStrm >> maNumGroupInfo;
    mbNumGroupInfoRead = true;
}

void XclImpPCField::ReadSxgroupinfo( XclImpStream& rStrm )
{
    DBG_ASSERT( IsStdGroupField(), "XclImpPCField::ReadSxgroupinfo - SXGROUPINFO outside grouping field" );
    DBG_ASSERT( maGroupOrder.empty(), "XclImpPCField::ReadSxgroupinfo - multiple SXGROUPINFO records" );
    DBG_ASSERT( mnTotalItemCount == maFieldInfo.mnVisItems, "XclImpPCField::ReadSxgroupinfo - SXGROUPINFO out of record order" );
    DBG_ASSERT( (rStrm.GetRecLeft() / 2) == maFieldInfo.mnBaseItems, "XclImpPCField::ReadSxgroupinfo - wrong SXGROUPINFO size" );
    maGroupOrder.clear();
    size_t nSize = rStrm.GetRecLeft() / 2;
    maGroupOrder.resize( nSize, 0 );
    for( size_t nIdx = 0; nIdx < nSize; ++nIdx )
        rStrm >> maGroupOrder[ nIdx ];
}

// grouping -------------------------------------------------------------------

void XclImpPCField::ApplyGroupField( ScDPSaveData& rSaveData, const ScfStringVec& rVisNames ) const
{
    if( GetFieldName( rVisNames ).Len() )
    {
        if( IsStdGroupField() )
            ApplyStdGroupField( rSaveData, rVisNames );
        else if( IsNumGroupField() )
            ApplyNumGroupField( rSaveData, rVisNames );
        else if( IsDateGroupField() )
            ApplyDateGroupField( rSaveData, rVisNames );
    }
}

// private --------------------------------------------------------------------

void XclImpPCField::ApplyStdGroupField( ScDPSaveData& rSaveData, const ScfStringVec& rVisNames ) const
{
    if( const XclImpPCField* pBaseField = GetGroupBaseField() )
    {
        const String& rBaseFieldName = pBaseField->GetFieldName( rVisNames );
        if( rBaseFieldName.Len() )
        {
            // *** create a ScDPSaveGroupItem for each own item, they collect base item names ***
            typedef ::std::vector< ScDPSaveGroupItem > ScDPSaveGroupItemVec;
            ScDPSaveGroupItemVec aGroupItems;
            aGroupItems.reserve( maItemList.Count() );
            // initialize with own item names
            for( const XclImpPCItem* pItem = maItemList.First(); pItem; pItem = maItemList.Next() )
                aGroupItems.push_back( ScDPSaveGroupItem( pItem->ConvertToText() ) );

            // *** iterate over all base items, set their names at corresponding own items ***
            for( sal_uInt16 nItemIdx = 0, nItemCount = static_cast< sal_uInt16 >( maGroupOrder.size() ); nItemIdx < nItemCount; ++nItemIdx )
                if( maGroupOrder[ nItemIdx ] < aGroupItems.size() )
                    if( const XclImpPCItem* pBaseItem = pBaseField->GetItem( nItemIdx ) )
                        if( const XclImpPCItem* pGroupItem = GetItem( maGroupOrder[ nItemIdx ] ) )
                            if( *pBaseItem != *pGroupItem )
                                aGroupItems[ maGroupOrder[ nItemIdx ] ].AddElement( pBaseItem->ConvertToText() );

            // *** create the ScDPSaveGroupDimension object, fill with grouping info ***
            ScDPSaveGroupDimension aGroupDim( rBaseFieldName, GetFieldName( rVisNames ) );
            for( ScDPSaveGroupItemVec::const_iterator aIt = aGroupItems.begin(), aEnd = aGroupItems.end(); aIt != aEnd; ++aIt )
                if( !aIt->IsEmpty() )
                    aGroupDim.AddGroupItem( *aIt );
            rSaveData.GetDimensionData()->AddGroupDimension( aGroupDim );
        }
    }
}

void XclImpPCField::ApplyNumGroupField( ScDPSaveData& rSaveData, const ScfStringVec& rVisNames ) const
{
    ScDPNumGroupInfo aNumInfo( GetScNumGroupInfo() );
    ScDPSaveNumGroupDimension aNumGroupDim( GetFieldName( rVisNames ), aNumInfo );
    rSaveData.GetDimensionData()->AddNumGroupDimension( aNumGroupDim );
}

void XclImpPCField::ApplyDateGroupField( ScDPSaveData& rSaveData, const ScfStringVec& rVisNames ) const
{
    ScDPNumGroupInfo aDateInfo( GetScDateGroupInfo() );
    sal_Int32 nScDateType = maNumGroupInfo.GetScDateType();

    switch( meFieldType )
    {
        case EXC_PCFIELD_DATEGROUP:
        {
            if( aDateInfo.DateValues )
            {
                // special case for days only with step value - create numeric grouping
                ScDPSaveNumGroupDimension aNumGroupDim( GetFieldName( rVisNames ), aDateInfo );
                rSaveData.GetDimensionData()->AddNumGroupDimension( aNumGroupDim );
            }
            else
            {
                ScDPSaveNumGroupDimension aNumGroupDim( GetFieldName( rVisNames ), ScDPNumGroupInfo() );
                aNumGroupDim.SetDateInfo( aDateInfo, nScDateType );
                rSaveData.GetDimensionData()->AddNumGroupDimension( aNumGroupDim );
            }
        }
        break;

        case EXC_PCFIELD_DATECHILD:
        {
            if( const XclImpPCField* pBaseField = GetGroupBaseField() )
            {
                const String& rBaseFieldName = pBaseField->GetFieldName( rVisNames );
                if( rBaseFieldName.Len() )
                {
                    ScDPSaveGroupDimension aGroupDim( rBaseFieldName, GetFieldName( rVisNames ) );
                    aGroupDim.SetDateInfo( aDateInfo, nScDateType );
                    rSaveData.GetDimensionData()->AddGroupDimension( aGroupDim );
                }
            }
        }
        break;

        default:
            DBG_ERRORFILE( "XclImpPCField::ApplyDateGroupField - unknown date field type" );
    }
}

ScDPNumGroupInfo XclImpPCField::GetScNumGroupInfo() const
{
    ScDPNumGroupInfo aNumInfo;
    aNumInfo.Enable = sal_True;
    aNumInfo.DateValues = sal_False;
    aNumInfo.AutoStart = sal_True;
    aNumInfo.AutoEnd = sal_True;

    if( const double* pfMinValue = GetNumGroupLimit( EXC_SXFIELD_INDEX_MIN ) )
    {
        aNumInfo.Start = *pfMinValue;
        aNumInfo.AutoStart = ::get_flag( maNumGroupInfo.mnFlags, EXC_SXNUMGROUP_AUTOMIN );
    }
    if( const double* pfMaxValue = GetNumGroupLimit( EXC_SXFIELD_INDEX_MAX ) )
    {
        aNumInfo.End = *pfMaxValue;
        aNumInfo.AutoEnd = ::get_flag( maNumGroupInfo.mnFlags, EXC_SXNUMGROUP_AUTOMAX );
    }
    if( const double* pfStepValue = GetNumGroupLimit( EXC_SXFIELD_INDEX_STEP ) )
        aNumInfo.Step = *pfStepValue;

    return aNumInfo;
}

ScDPNumGroupInfo XclImpPCField::GetScDateGroupInfo() const
{
    ScDPNumGroupInfo aDateInfo;
    aDateInfo.Enable = sal_True;
    aDateInfo.DateValues = sal_False;
    aDateInfo.AutoStart = sal_True;
    aDateInfo.AutoEnd = sal_True;

    if( const double* pfMinValue = GetDateGroupLimit( EXC_SXFIELD_INDEX_MIN ) )
    {
        aDateInfo.Start = *pfMinValue;
        aDateInfo.AutoStart = ::get_flag( maNumGroupInfo.mnFlags, EXC_SXNUMGROUP_AUTOMIN );
    }
    if( const double* pfMaxValue = GetDateGroupLimit( EXC_SXFIELD_INDEX_MAX ) )
    {
        aDateInfo.End = *pfMaxValue;
        aDateInfo.AutoEnd = ::get_flag( maNumGroupInfo.mnFlags, EXC_SXNUMGROUP_AUTOMAX );
    }
    // GetDateGroupStep() returns a value for date type "day" in single date groups only
    if( const sal_Int16* pnStepValue = GetDateGroupStep() )
    {
        aDateInfo.Step = *pnStepValue;
        aDateInfo.DateValues = sal_True;
    }

    return aDateInfo;
}

const double* XclImpPCField::GetNumGroupLimit( sal_uInt16 nLimitIdx ) const
{
    DBG_ASSERT( IsNumGroupField(), "XclImpPCField::GetNumGroupLimit - only for numeric grouping fields" );
    if( const XclImpPCItem* pItem = GetLimitItem( nLimitIdx ) )
    {
        DBG_ASSERT( pItem->GetDouble(), "XclImpPCField::GetNumGroupLimit - SXDOUBLE item expected" );
        return pItem->GetDouble();
    }
    return 0;
}

const double* XclImpPCField::GetDateGroupLimit( sal_uInt16 nLimitIdx ) const
{
    DBG_ASSERT( IsDateGroupField(), "XclImpPCField::GetDateGroupLimit - only for date grouping fields" );
    if( const XclImpPCItem* pItem = GetLimitItem( nLimitIdx ) )
    {
        DBG_ASSERT( pItem->GetDate(), "XclImpPCField::GetDateGroupLimit - SXDATETIME item expected" );
        return pItem->GetDate();
    }
    return 0;
}

const sal_Int16* XclImpPCField::GetDateGroupStep() const
{
    // only for single date grouping fields, not for grouping chains
    if( !IsGroupBaseField() && !IsGroupChildField() )
    {
        // only days may have a step value, return 0 for all other date types
        if( maNumGroupInfo.GetXclDataType() == EXC_SXNUMGROUP_TYPE_DAY )
        {
            if( const XclImpPCItem* pItem = GetLimitItem( EXC_SXFIELD_INDEX_STEP ) )
            {
                DBG_ASSERT( pItem->GetInteger(), "XclImpPCField::GetDateGroupStep - SXINTEGER item expected" );
                if( const sal_Int16* pnStep = pItem->GetInteger() )
                {
                    DBG_ASSERT( *pnStep > 0, "XclImpPCField::GetDateGroupStep - invalid step count" );
                    // return nothing for step count 1 - this is also a standard date group in Excel
                    return (*pnStep > 1) ? pnStep : 0;
                }
            }
        }
    }
    return 0;
}

// ============================================================================

XclImpPivotCache::XclImpPivotCache( const XclImpStream& rBookStrm, sal_uInt16 nStrmId ) :
    XclImpRoot( rBookStrm.GetRoot() ),
    mnSrcType( EXC_SXVS_UNKNOWN )
{
    SotStorageRef xSvStrg = OpenStorage( EXC_STORAGE_PTCACHE );
    SotStorageStreamRef xSvStrm = OpenStream( xSvStrg, ScfTools::GetHexStr( nStrmId ) );
    if( xSvStrm.Is() )
    {
        XclImpStream aStrm( *xSvStrm, GetRoot() );
        aStrm.CopyDecrypterFrom( rBookStrm );
        ReadPivotCacheStream( aStrm );
    }
}

XclImpPivotCache::~XclImpPivotCache()
{
}

// data access ----------------------------------------------------------------

bool XclImpPivotCache::IsSelfRef() const
{
    return mbSelf && (mnSrcType == EXC_SXVS_SHEET) && (maPCInfo.mnSrcType == EXC_SXDB_SRC_SHEET);
}

sal_uInt16 XclImpPivotCache::GetFieldCount() const
{
    return static_cast< sal_uInt16 >( maFieldList.Count() );
}

const XclImpPCField* XclImpPivotCache::GetField( sal_uInt16 nFieldIdx ) const
{
    return maFieldList.GetObject( nFieldIdx );
}

// records --------------------------------------------------------------------

void XclImpPivotCache::ReadDconref( XclImpStream& rStrm )
{
    // read DCONREF only once, there may be others in another context
    if( maTabName.Len() )
        return;

    XclRange aXclRange( ScAddress::UNINITIALIZED );
    aXclRange.Read( rStrm, false );
    String aEncUrl( rStrm.ReadUniString() );

    XclImpUrlHelper::DecodeUrl( maUrl, maTabName, mbSelf, GetRoot(), aEncUrl );
    if( !maTabName.Len() )
    {
        maTabName = maUrl;
        maUrl.Erase();
    }
    // Do not convert maTabName to Calc sheet name -> original name is used to find the sheet.

    // Sheet index will be found later in XclImpPivotTable::Apply() (sheet may not exist yet).
    GetAddressConverter().ConvertRange( maSrcRange, aXclRange, 0, 0, true );
}

void XclImpPivotCache::ReadSxvs( XclImpStream& rStrm )
{
    rStrm >> mnSrcType;
    GetTracer().TracePivotDataSource( mnSrcType == EXC_SXVS_EXTERN );
}

// private --------------------------------------------------------------------

void XclImpPivotCache::ReadPivotCacheStream( XclImpStream& rStrm )
{
    XclImpPCField* pCurrField = 0;  // Current field for new items.
    bool bExistPostponed = false;   // true = Any field with postponed items.
    bool bReadPostponed = false;    // true = Read now into postponed fields.
    bool bLoop = true;              // true = Continue loop.

    while( bLoop && rStrm.StartNextRecord() )
    {
        bool bLastWasItem = false;  // true = Last record was item data.

        switch( rStrm.GetRecId() )
        {
            case EXC_ID_EOF:
                bLoop = false;
            break;

            case EXC_ID_SXDB:
                rStrm >> maPCInfo;
            break;

            case EXC_ID_SXFIELD:
            {
                sal_uInt16 nNewFieldIdx = GetFieldCount();
                maFieldList.Append( pCurrField = new XclImpPCField( rStrm, *this, nNewFieldIdx ) );
                bExistPostponed |= pCurrField->HasPostponedItems();
            }
            break;

            case EXC_ID_SXIDARRAY:
                // Items of postponed fields start now.
                bReadPostponed = bExistPostponed;
                if( bReadPostponed )
                {
                    // Find the first postponed field.
                    pCurrField = maFieldList.First();
                    while( pCurrField && !pCurrField->HasPostponedItems() )
                        pCurrField = maFieldList.Next();
                }
            break;

            case EXC_ID_SXDOUBLE:
            case EXC_ID_SXBOOLEAN:
            case EXC_ID_SXERROR:
            case EXC_ID_SXINTEGER:
            case EXC_ID_SXSTRING:
            case EXC_ID_SXDATETIME:
            case EXC_ID_SXEMPTY:
                if( pCurrField )
                    pCurrField->ReadItem( rStrm );
                bLastWasItem = true;
            break;

            case EXC_ID_SXNUMGROUP:
                if( pCurrField )
                    pCurrField->ReadSxnumgroup( rStrm );
            break;

            case EXC_ID_SXGROUPINFO:
                if( pCurrField )
                    pCurrField->ReadSxgroupinfo( rStrm );
            break;

            // known but ignored records
            case EXC_ID_SXRULE:
            case EXC_ID_SXFILT:
            case EXC_ID_00F5:
            case EXC_ID_SXNAME:
            case EXC_ID_SXPAIR:
            case EXC_ID_SXFMLA:
            case EXC_ID_SXFORMULA:
            case EXC_ID_SXDBEX:
            case EXC_ID_SXFDBTYPE:
            break;

            default:
                DBG_ERROR1( "XclImpPivotCache::ReadPivotCacheStream - unknown record 0x%04hX", rStrm.GetRecId() );
        }

        if( bReadPostponed && bLastWasItem )
        {
            // Find the next postponed field.
            pCurrField = maFieldList.Next();
            while( pCurrField && !pCurrField->HasPostponedItems() )
                pCurrField = maFieldList.Next();
        }
    }

    DBG_ASSERT( maPCInfo.mnTotalFields == maFieldList.Count(),
        "XclImpPivotCache::ReadPivotCacheStream - field count mismatch" );
}

// ============================================================================
// Pivot table
// ============================================================================

XclImpPTItem::XclImpPTItem( const XclImpPCField* pCacheField ) :
    mpCacheField( pCacheField )
{
}

const String* XclImpPTItem::GetItemName() const
{
    if( mpCacheField )
        if( const XclImpPCItem* pCacheItem = mpCacheField->GetItem( maItemInfo.mnCacheIdx ) )
            //! TODO: use XclImpPCItem::ConvertToString(), if all conversions are available
            return pCacheItem->IsEmpty() ? &EMPTY_STRING : pCacheItem->GetText();
    return 0;
}

const String* XclImpPTItem::GetVisItemName() const
{
    return maItemInfo.HasVisName() ? maItemInfo.GetVisName() : GetItemName();
}

void XclImpPTItem::ReadSxvi( XclImpStream& rStrm )
{
    rStrm >> maItemInfo;
}

void XclImpPTItem::ApplyItem( ScDPSaveDimension& rSaveDim ) const
{
    if( const String* pItemName = GetItemName() )
    {
        ScDPSaveMember& rMember = *rSaveDim.GetMemberByName( *pItemName );
        rMember.SetIsVisible( !::get_flag( maItemInfo.mnFlags, EXC_SXVI_HIDDEN ) );
        rMember.SetShowDetails( !::get_flag( maItemInfo.mnFlags, EXC_SXVI_HIDEDETAIL ) );
    }
}

// ============================================================================

XclImpPTField::XclImpPTField( const XclImpPivotTable& rPTable, sal_uInt16 nCacheIdx ) :
    mrPTable( rPTable )
{
    maFieldInfo.mnCacheIdx = nCacheIdx;
}

// general field/item access --------------------------------------------------

const XclImpPCField* XclImpPTField::GetCacheField() const
{
    if( const XclImpPivotCache* pPCache = mrPTable.GetPivotCache() )
        return pPCache->GetField( maFieldInfo.mnCacheIdx );
    return 0;
}

const String& XclImpPTField::GetFieldName() const
{
    if( const XclImpPCField* pField = GetCacheField() )
        return pField->GetFieldName( mrPTable.GetVisFieldNames() );
    return EMPTY_STRING;
}

const String& XclImpPTField::GetVisFieldName() const
{
    if( const String* pVisName = maFieldInfo.GetVisName() )
        return *pVisName;
    return EMPTY_STRING;
}

const XclImpPTItem* XclImpPTField::GetItem( sal_uInt16 nItemIdx ) const
{
    return maItemList.GetObject( nItemIdx );
}

const String* XclImpPTField::GetItemName( sal_uInt16 nItemIdx ) const
{
    if( const XclImpPTItem* pItem = GetItem( nItemIdx ) )
        return pItem->GetItemName();
    return 0;
}

const String* XclImpPTField::GetVisItemName( sal_uInt16 nItemIdx ) const
{
    if( const XclImpPTItem* pItem = GetItem( nItemIdx ) )
        return pItem->GetVisItemName();
    return 0;
}

// records --------------------------------------------------------------------

void XclImpPTField::ReadSxvd( XclImpStream& rStrm )
{
    rStrm >> maFieldInfo;
}

void XclImpPTField::ReadSxvdex( XclImpStream& rStrm )
{
    rStrm >> maFieldExtInfo;
}

void XclImpPTField::ReadSxvi( XclImpStream& rStrm )
{
    XclImpPTItem* pItem = new XclImpPTItem( GetCacheField() );
    maItemList.Append( pItem );
    pItem->ReadSxvi( rStrm );
}

// row/column fields ----------------------------------------------------------

void XclImpPTField::ApplyRowColField( ScDPSaveData& rSaveData ) const
{
    DBG_ASSERT( maFieldInfo.mnAxes & EXC_SXVD_AXIS_ROWCOL, "XclImpPTField::ApplyRowColField - no row/column field" );
    // special data orientation field?
    if( maFieldInfo.mnCacheIdx == EXC_SXIVD_DATA )
        rSaveData.GetDataLayoutDimension()->SetOrientation( maFieldInfo.GetApiOrient( EXC_SXVD_AXIS_ROWCOL ) );
    else
        ApplyRCPField( rSaveData );
}

// page fields ----------------------------------------------------------------

void XclImpPTField::SetPageFieldInfo( const XclPTPageFieldInfo& rPageInfo )
{
    maPageInfo = rPageInfo;
}

void XclImpPTField::ApplyPageField( ScDPSaveData& rSaveData ) const
{
    DBG_ASSERT( maFieldInfo.mnAxes & EXC_SXVD_AXIS_PAGE, "XclImpPTField::ApplyPageField - no page field" );
    if( ScDPSaveDimension* pSaveDim = ApplyRCPField( rSaveData ) )
        pSaveDim->SetCurrentPage( GetItemName( maPageInfo.mnSelItem ) );
}

// hidden fields --------------------------------------------------------------

void XclImpPTField::ApplyHiddenField( ScDPSaveData& rSaveData ) const
{
    DBG_ASSERT( (maFieldInfo.mnAxes & EXC_SXVD_AXIS_ROWCOLPAGE) == 0, "XclImpPTField::ApplyHiddenField - field not hidden" );
    ApplyRCPField( rSaveData );
}

// data fields ----------------------------------------------------------------

bool XclImpPTField::HasDataFieldInfo() const
{
    return !maDataInfoList.empty();
}

void XclImpPTField::AddDataFieldInfo( const XclPTDataFieldInfo& rDataInfo )
{
    DBG_ASSERT( maFieldInfo.mnAxes & EXC_SXVD_AXIS_DATA, "XclImpPTField::AddDataFieldInfo - no data field" );
    maDataInfoList.push_back( rDataInfo );
}

void XclImpPTField::ApplyDataField( ScDPSaveData& rSaveData ) const
{
    DBG_ASSERT( maFieldInfo.mnAxes & EXC_SXVD_AXIS_DATA, "XclImpPTField::ApplyDataField - no data field" );
    DBG_ASSERT( !maDataInfoList.empty(), "XclImpPTField::ApplyDataField - no data field info" );
    if( !maDataInfoList.empty() )
    {
        const String& rFieldName = GetFieldName();
        if( rFieldName.Len() )
        {
            XclPTDataFieldInfoList::const_iterator aIt = maDataInfoList.begin(), aEnd = maDataInfoList.end();

            ScDPSaveDimension& rSaveDim = *rSaveData.GetNewDimensionByName( rFieldName );
            ApplyDataField( rSaveDim, *aIt );

            // multiple data fields -> clone dimension
            for( ++aIt; aIt != aEnd; ++aIt )
            {
                ScDPSaveDimension& rDupDim = rSaveData.DuplicateDimension( rSaveDim );
                ApplyDataFieldInfo( rDupDim, *aIt );
            }
        }
    }
}

// private --------------------------------------------------------------------

ScDPSaveDimension* XclImpPTField::ApplyRCPField( ScDPSaveData& rSaveData ) const
{
    const String& rFieldName = GetFieldName();
    if( !rFieldName.Len() )
        return 0;

    const XclImpPCField* pCacheField = GetCacheField();
    if( !pCacheField || !pCacheField->IsSupportedField() )
        return 0;

    ScDPSaveDimension& rSaveDim = *rSaveData.GetNewDimensionByName( rFieldName );

    // orientation
    rSaveDim.SetOrientation( maFieldInfo.GetApiOrient( EXC_SXVD_AXIS_ROWCOLPAGE ) );

    // general field info
    ApplyFieldInfo( rSaveDim );

    // visible name
    if( const String* pVisName = maFieldInfo.GetVisName() )
        if( pVisName->Len() )
            rSaveDim.SetLayoutName( pVisName );

    // subtotal function(s)
    XclPTSubtotalVec aSubtotalVec;
    maFieldInfo.GetSubtotals( aSubtotalVec );
    if( !aSubtotalVec.empty() )
        rSaveDim.SetSubTotals( static_cast< long >( aSubtotalVec.size() ), &aSubtotalVec[ 0 ] );

    // sorting
    DataPilotFieldSortInfo aSortInfo;
    aSortInfo.Field = mrPTable.GetDataFieldName( maFieldExtInfo.mnSortField );
    aSortInfo.IsAscending = ::get_flag( maFieldExtInfo.mnFlags, EXC_SXVDEX_SORT_ASC );
    aSortInfo.Mode = maFieldExtInfo.GetApiSortMode();
    rSaveDim.SetSortInfo( &aSortInfo );

    // auto show
    DataPilotFieldAutoShowInfo aShowInfo;
    aShowInfo.IsEnabled = ::get_flag( maFieldExtInfo.mnFlags, EXC_SXVDEX_AUTOSHOW );
    aShowInfo.ShowItemsMode = maFieldExtInfo.GetApiAutoShowMode();
    aShowInfo.ItemCount = maFieldExtInfo.GetApiAutoShowCount();
    aShowInfo.DataField = mrPTable.GetDataFieldName( maFieldExtInfo.mnShowField );
    rSaveDim.SetAutoShowInfo( &aShowInfo );

    // layout
    DataPilotFieldLayoutInfo aLayoutInfo;
    aLayoutInfo.LayoutMode = maFieldExtInfo.GetApiLayoutMode();
    aLayoutInfo.AddEmptyLines = ::get_flag( maFieldExtInfo.mnFlags, EXC_SXVDEX_LAYOUT_BLANK );
    rSaveDim.SetLayoutInfo( &aLayoutInfo );

    // grouping info
    pCacheField->ApplyGroupField( rSaveData, mrPTable.GetVisFieldNames() );

    return &rSaveDim;
}

void XclImpPTField::ApplyFieldInfo( ScDPSaveDimension& rSaveDim ) const
{
    rSaveDim.SetShowEmpty( ::get_flag( maFieldExtInfo.mnFlags, EXC_SXVDEX_SHOWALL ) );
    ApplyItems( rSaveDim );
}

void XclImpPTField::ApplyDataField( ScDPSaveDimension& rSaveDim, const XclPTDataFieldInfo& rDataInfo ) const
{
    // orientation
    rSaveDim.SetOrientation( DataPilotFieldOrientation_DATA );
    // general field info
    ApplyFieldInfo( rSaveDim );
    // extended data field info
    ApplyDataFieldInfo( rSaveDim, rDataInfo );
}

void XclImpPTField::ApplyDataFieldInfo( ScDPSaveDimension& rSaveDim, const XclPTDataFieldInfo& rDataInfo ) const
{
    // visible name
    if( const String* pVisName = rDataInfo.GetVisName() )
        if( pVisName->Len() )
            rSaveDim.SetLayoutName( pVisName );

    // aggregation function
    rSaveDim.SetFunction( rDataInfo.GetApiAggFunc() );

    // result field reference
    sal_Int32 nRefType = rDataInfo.GetApiRefType();
    if( nRefType != ::com::sun::star::sheet::DataPilotFieldReferenceType::NONE )
    {
        DataPilotFieldReference aFieldRef;
        aFieldRef.ReferenceType = nRefType;

        if( const XclImpPTField* pRefField = mrPTable.GetField( rDataInfo.mnRefField ) )
        {
            aFieldRef.ReferenceField = pRefField->GetFieldName();
            aFieldRef.ReferenceItemType = rDataInfo.GetApiRefItemType();
            if( aFieldRef.ReferenceItemType == ::com::sun::star::sheet::DataPilotFieldReferenceItemType::NAMED )
                if( const String* pRefItemName = pRefField->GetItemName( rDataInfo.mnRefItem ) )
                    aFieldRef.ReferenceItemName = *pRefItemName;
        }

        rSaveDim.SetReferenceValue( &aFieldRef );
    }
}

void XclImpPTField::ApplyItems( ScDPSaveDimension& rSaveDim ) const
{
    for( const XclImpPTItem* pItem = maItemList.First(); pItem; pItem = maItemList.Next() )
        pItem->ApplyItem( rSaveDim );
}

// ============================================================================

XclImpPivotTable::XclImpPivotTable( const XclImpRoot& rRoot ) :
    XclImpRoot( rRoot ),
    mpPCache( 0 ),

    maDataOrientField( *this, EXC_SXIVD_DATA ),
    mpCurrField( 0 )
{
}

XclImpPivotTable::~XclImpPivotTable()
{
}

// cache/field access, misc. --------------------------------------------------

sal_uInt16 XclImpPivotTable::GetFieldCount() const
{
    return static_cast< sal_uInt16 >( maFieldList.Count() );
}

const XclImpPTField* XclImpPivotTable::GetField( sal_uInt16 nFieldIdx ) const
{
    return (nFieldIdx == EXC_SXIVD_DATA) ? &maDataOrientField : maFieldList.GetObject( nFieldIdx );
}

XclImpPTField* XclImpPivotTable::GetFieldAcc( sal_uInt16 nFieldIdx )
{
    return maFieldList.GetObject( nFieldIdx );  // do not return maDataOrientField
}

const String& XclImpPivotTable::GetFieldName( sal_uInt16 nFieldIdx ) const
{
    if( const XclImpPTField* pField = GetField( nFieldIdx ) )
        return pField->GetFieldName();
    return EMPTY_STRING;
}

const XclImpPTField* XclImpPivotTable::GetDataField( sal_uInt16 nDataFieldIdx ) const
{
    if( nDataFieldIdx < maOrigDataFields.size() )
        return GetField( maOrigDataFields[ nDataFieldIdx ] );
    return 0;
}

const String& XclImpPivotTable::GetDataFieldName( sal_uInt16 nDataFieldIdx ) const
{
    if( const XclImpPTField* pField = GetDataField( nDataFieldIdx ) )
        return pField->GetFieldName();
    return EMPTY_STRING;
}

// records --------------------------------------------------------------------

void XclImpPivotTable::ReadSxview( XclImpStream& rStrm )
{
    rStrm >> maPTInfo;

    GetAddressConverter().ConvertRange(
        maOutScRange, maPTInfo.maOutXclRange, GetCurrScTab(), GetCurrScTab(), true );

    mpPCache = GetPivotTableManager().GetPivotCache( maPTInfo.mnCacheIdx );
    mpCurrField = 0;
}

void XclImpPivotTable::ReadSxvd( XclImpStream& rStrm )
{
    sal_uInt16 nFieldCount = GetFieldCount();
    if( nFieldCount < EXC_PT_MAXFIELDCOUNT )
    {
        // cache index for the field is equal to the SXVD record index
        mpCurrField = new XclImpPTField( *this, nFieldCount );
        maFieldList.Append( mpCurrField );
        mpCurrField->ReadSxvd( rStrm );
        // add visible name of new field to list of visible names
        maVisFieldNames.push_back( mpCurrField->GetVisFieldName() );
        DBG_ASSERT( maFieldList.Count() == maVisFieldNames.size(),
            "XclImpPivotTable::ReadSxvd - wrong size of visible name array" );
    }
    else
        mpCurrField = 0;
}

void XclImpPivotTable::ReadSxvi( XclImpStream& rStrm )
{
    if( mpCurrField )
        mpCurrField->ReadSxvi( rStrm );
}

void XclImpPivotTable::ReadSxvdex( XclImpStream& rStrm )
{
    if( mpCurrField )
        mpCurrField->ReadSxvdex( rStrm );
}

void XclImpPivotTable::ReadSxivd( XclImpStream& rStrm )
{
    mpCurrField = 0;

    // find the index vector to fill (row SXIVD doesn't exist without row fields)
    ScfUInt16Vec* pFieldVec = 0;
    if( maRowFields.empty() && (maPTInfo.mnRowFields > 0) )
        pFieldVec = &maRowFields;
    else if( maColFields.empty() && (maPTInfo.mnColFields > 0) )
        pFieldVec = &maColFields;

    // fill the vector from record data
    if( pFieldVec )
    {
        sal_uInt16 nSize = ulimit_cast< sal_uInt16 >( rStrm.GetRecSize() / 2, EXC_PT_MAXROWCOLCOUNT );
        pFieldVec->reserve( nSize );
        for( sal_uInt16 nIdx = 0; nIdx < nSize; ++nIdx )
        {
            sal_uInt16 nFieldIdx;
            rStrm >> nFieldIdx;
            pFieldVec->push_back( nFieldIdx );

            // set orientation at special data orientation field
            if( nFieldIdx == EXC_SXIVD_DATA )
            {
                sal_uInt16 nAxis = (pFieldVec == &maRowFields) ? EXC_SXVD_AXIS_ROW : EXC_SXVD_AXIS_COL;
                maDataOrientField.SetAxes( nAxis );
            }
        }
    }
}

void XclImpPivotTable::ReadSxpi( XclImpStream& rStrm )
{
    mpCurrField = 0;

    sal_uInt16 nSize = ulimit_cast< sal_uInt16 >( rStrm.GetRecSize() / 6 );
    for( sal_uInt16 nEntry = 0; nEntry < nSize; ++nEntry )
    {
        XclPTPageFieldInfo aPageInfo;
        rStrm >> aPageInfo;
        if( XclImpPTField* pField = GetFieldAcc( aPageInfo.mnField ) )
        {
            maPageFields.push_back( aPageInfo.mnField );
            pField->SetPageFieldInfo( aPageInfo );
        }
        GetObjectManager().SetInvalidObj( GetCurrScTab(), aPageInfo.mnObjId );
    }
}

void XclImpPivotTable::ReadSxdi( XclImpStream& rStrm )
{
    mpCurrField = 0;

    XclPTDataFieldInfo aDataInfo;
    rStrm >> aDataInfo;
    if( XclImpPTField* pField = GetFieldAcc( aDataInfo.mnField ) )
    {
        maOrigDataFields.push_back( aDataInfo.mnField );
        // DataPilot does not support double data fields -> add first appearence to index list only
        if( !pField->HasDataFieldInfo() )
            maFiltDataFields.push_back( aDataInfo.mnField );
        pField->AddDataFieldInfo( aDataInfo );
    }
}

void XclImpPivotTable::ReadSxex( XclImpStream& rStrm )
{
    rStrm >> maPTExtInfo;
}

// ----------------------------------------------------------------------------

void XclImpPivotTable::Apply() const
{
    if( !mpPCache || !mpPCache->IsSelfRef() )
        return;

    ScRange aSrcRange( mpPCache->GetSourceRange() );
    SCTAB nScTab = GetTabInfo().GetScTabFromXclName( mpPCache->GetTabName() );
    if( nScTab == SCTAB_MAX )
        return;

    ScDPSaveData aSaveData;

    // *** global settings ***

    aSaveData.SetRowGrand( ::get_flag( maPTInfo.mnFlags, EXC_SXVIEW_ROWGRAND ) );
    aSaveData.SetColumnGrand( ::get_flag( maPTInfo.mnFlags, EXC_SXVIEW_COLGRAND ) );
    aSaveData.SetFilterButton( FALSE );
    aSaveData.SetDrillDown( ::get_flag( maPTExtInfo.mnFlags, EXC_SXEX_DRILLDOWN ) );

    // *** fields ***

    ScfUInt16Vec::const_iterator aIt, aEnd;

    // row fields
    for( aIt = maRowFields.begin(), aEnd = maRowFields.end(); aIt != aEnd; ++aIt )
        if( const XclImpPTField* pField = GetField( *aIt ) )
            pField->ApplyRowColField( aSaveData );

    // column fields
    for( aIt = maColFields.begin(), aEnd = maColFields.end(); aIt != aEnd; ++aIt )
        if( const XclImpPTField* pField = GetField( *aIt ) )
            pField->ApplyRowColField( aSaveData );

    // page fields
    for( aIt = maPageFields.begin(), aEnd = maPageFields.end(); aIt != aEnd; ++aIt )
        if( const XclImpPTField* pField = GetField( *aIt ) )
            pField->ApplyPageField( aSaveData );

    // hidden fields
    for( sal_uInt16 nField = 0, nCount = GetFieldCount(); nField < nCount; ++nField )
        if( const XclImpPTField* pField = GetField( nField ) )
            if( (pField->GetAxes() & EXC_SXVD_AXIS_ROWCOLPAGE) == 0 )
                pField->ApplyHiddenField( aSaveData );

    // data fields
    for( aIt = maFiltDataFields.begin(), aEnd = maFiltDataFields.end(); aIt != aEnd; ++aIt )
        if( const XclImpPTField* pField = GetField( *aIt ) )
            pField->ApplyDataField( aSaveData );

    // *** insert into Calc document ***

    // create source descriptor
    aSrcRange.aStart.SetTab( nScTab );
    aSrcRange.aEnd.SetTab( nScTab );
    ScSheetSourceDesc aDesc;
    aDesc.aSourceRange = aSrcRange;

    // adjust output range to include the page fields
    ScRange aOutRange( maOutScRange );
    if( !maPageFields.empty() )
    {
        SCsROW nDecRows = ::std::min< SCsROW >( aOutRange.aStart.Row(), maPageFields.size() + 1 );
        aOutRange.aStart.IncRow( -nDecRows );
    }

    // create the DataPilot
    ScDPObject* pDPObj = new ScDPObject( GetDocPtr() );
    pDPObj->SetName( maPTInfo.maTableName );
    pDPObj->SetSaveData( aSaveData );
    pDPObj->SetSheetDesc( aDesc );
    pDPObj->SetOutRange( aOutRange );
    pDPObj->SetAlive( TRUE );
    GetDoc().GetDPCollection()->Insert( pDPObj );
}

// ============================================================================
// ============================================================================

XclImpPivotTableManager::XclImpPivotTableManager( const XclImpRoot& rRoot ) :
    XclImpRoot( rRoot )
{
}

XclImpPivotTableManager::~XclImpPivotTableManager()
{
}

// access functions -----------------------------------------------------------

const XclImpPivotCache* XclImpPivotTableManager::GetPivotCache( sal_uInt16 nCacheIdx ) const
{
    return maPCacheList.GetObject( nCacheIdx );
}

// pivot cache records --------------------------------------------------------

void XclImpPivotTableManager::ReadSxidstm( XclImpStream& rStrm )
{
    sal_uInt16 nStrmId;
    rStrm >> nStrmId;
    maPCacheList.Append( new XclImpPivotCache( rStrm, nStrmId ) );
}

void XclImpPivotTableManager::ReadDconref( XclImpStream& rStrm )
{
    if( !maPCacheList.Empty() )
        maPCacheList.Last()->ReadDconref( rStrm );
}

void XclImpPivotTableManager::ReadSxvs( XclImpStream& rStrm )
{
    if( !maPCacheList.Empty() )
        maPCacheList.Last()->ReadSxvs( rStrm );
}

// pivot table records --------------------------------------------------------

void XclImpPivotTableManager::ReadSxview( XclImpStream& rStrm )
{
    XclImpPivotTable* pPTable = new XclImpPivotTable( GetRoot() );
    maPTableList.Append( pPTable );
    pPTable->ReadSxview( rStrm );
}

void XclImpPivotTableManager::ReadSxvd( XclImpStream& rStrm )
{
    if( !maPTableList.Empty() )
        maPTableList.Last()->ReadSxvd( rStrm );
}

void XclImpPivotTableManager::ReadSxvdex( XclImpStream& rStrm )
{
    if( !maPTableList.Empty() )
        maPTableList.Last()->ReadSxvdex( rStrm );
}

void XclImpPivotTableManager::ReadSxivd( XclImpStream& rStrm )
{
    if( !maPTableList.Empty() )
        maPTableList.Last()->ReadSxivd( rStrm );
}

void XclImpPivotTableManager::ReadSxpi( XclImpStream& rStrm )
{
    if( !maPTableList.Empty() )
        maPTableList.Last()->ReadSxpi( rStrm );
}

void XclImpPivotTableManager::ReadSxdi( XclImpStream& rStrm )
{
    if( !maPTableList.Empty() )
        maPTableList.Last()->ReadSxdi( rStrm );
}

void XclImpPivotTableManager::ReadSxvi( XclImpStream& rStrm )
{
    if( !maPTableList.Empty() )
        maPTableList.Last()->ReadSxvi( rStrm );
}

void XclImpPivotTableManager::ReadSxex( XclImpStream& rStrm )
{
    if( !maPTableList.Empty() )
        maPTableList.Last()->ReadSxex( rStrm );
}

// ----------------------------------------------------------------------------

void XclImpPivotTableManager::Apply() const
{
    for( const XclImpPivotTable* pPTable = maPTableList.First(); pPTable; pPTable = maPTableList.Next() )
        pPTable->Apply();
}

// ============================================================================

