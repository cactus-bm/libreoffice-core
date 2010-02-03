/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright IBM Corporation 2009.
 * Copyright 2009 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dptablecache.hxx,v $
 * $Revision: 1.0 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef DPTABLECACHE_HXX
#define DPTABLECACHE_HXX
// Wang Xu Ming -- 12/21/2008
// Add Data Cache Support.
#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
//Added by PengYunQuan for SODC_16015
#include <svtools/zforlist.hxx>
//end
#include <vector>
#include "dpglobal.hxx"

#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>

class ScDPTableDataCache;
class TypedStrData;
// --------------------------------------------------------------------
//
//  base class ScDPTableData to allow implementation with tabular data
//  by deriving only of this
//

class SC_DLLPUBLIC ScDPTableDataCache
{
    long    mnID;
    ScDocument* mpDoc;

    long                         mnColumnCount;     // Column count

    std::vector<ScDPItemData*>*      mpTableDataValues; //Data Pilot Table's index - value map
    std::vector<SCROW>*          mpSourceData;      //Data Pilot Table's Source data
    std::vector<SCROW>*          mpGlobalOrder;     //Sorted members index
    std::vector<SCROW>*          mpIndexOrder;      //Index the sorted number
    std::vector<ScDPItemData*>   mrLabelNames;      //Source Label data
    std::vector<BOOL>            mbEmptyRow;        //If empty row?
    mutable ScDPItemDataPool                 maAdditionalDatas;
public:
    SCROW GetOrder( long nDim, SCROW nIndex ) const;
    SCROW GetIdByItemData( long nDim,  String sItemData  ) const;
    SCROW GetIdByItemData( long nDim, const ScDPItemData& rData ) const;

    SCROW GetAdditionalItemID ( String sItemData );
    SCROW GetAdditionalItemID( const ScDPItemData& rData );

    SCCOL GetDimensionIndex( String sName) const;
    const ScDPItemData* GetSortedItemData( SCCOL nDim, SCROW nOrder ) const;
    ULONG GetNumType ( ULONG nFormat ) const;
    ULONG GetNumberFormat( long nDim ) const;
    BOOL  IsDateDimension( long nDim ) const ;
    ULONG GetDimNumType( SCCOL nDim) const;
    SCROW GetDimMemberCount( SCCOL nDim ) const;

    SCROW GetSortedItemDataId( SCCOL nDim, SCROW nOrder ) const;
    const std::vector<ScDPItemData*>& GetDimMemberValues( SCCOL nDim )const;
    void    SetId( long nId ){ mnID = nId;}
    void    AddRow( ScDPItemData* pRow, USHORT nCount );
    bool    InitFromDoc(  ScDocument* pDoc, const ScRange& rRange );
    bool InitFromDataBase (const  ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet>& xRowSet, const Date& rNullDate);

    SCROW   GetRowCount() const;
    SCROW   GetItemDataId( USHORT nDim, SCROW nRow, BOOL bRepeatIfEmpty ) const;
    String  GetDimensionName( USHORT nColumn ) const;
    bool    IsEmptyMember( SCROW nRow, USHORT nColumn ) const;
    bool    IsRowEmpty( SCROW nRow ) const;
    bool    IsValid() const;
    bool    ValidQuery( SCROW nRow, const ScQueryParam& rQueryParam, BOOL* pSpecial );

    ScDocument* GetDoc() const;//ms-cache-core
    long GetColumnCount() const;
    long    GetId() const;

    const ScDPItemData* GetItemDataById( long nDim, SCROW nId ) const;

    BOOL operator== ( const ScDPTableDataCache& r ) const;

//construction
    ScDPTableDataCache( ScDocument* pDoc );
//deconstruction
    virtual ~ScDPTableDataCache();

protected:
private:
    void        AddLabel( ScDPItemData* pData);
    BOOL    AddData( long nDim, ScDPItemData* itemData );
};

#endif //DPTABLECACHE_HXX
