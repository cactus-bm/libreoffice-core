/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dpobject.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-28 11:26:37 $
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

#ifndef SC_DPOBJECT_HXX
#define SC_DPOBJECT_HXX

#ifndef INCLUDED_SCDLLAPI_H
#include "scdllapi.h"
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif

#ifndef SC_COLLECT_HXX
#include "collect.hxx"
#endif

#ifndef _COM_SUN_STAR_SHEET_XDIMENSIONSSUPPLIER_HPP_
#include <com/sun/star/sheet/XDimensionsSupplier.hpp>
#endif


//------------------------------------------------------------------


class Rectangle;
class SvStream;
class ScDPSaveData;
class ScDPOutput;
struct ScDPPositionData;
class ScMultipleReadHeader;
class ScMultipleWriteHeader;
class ScPivot;
class ScPivotCollection;
struct ScPivotParam;
struct ScImportSourceDesc;
struct ScSheetSourceDesc;
class StrCollection;
class TypedStrCollection;
struct PivotField;


struct ScDPServiceDesc
{
    String  aServiceName;
    String  aParSource;
    String  aParName;
    String  aParUser;
    String  aParPass;

    ScDPServiceDesc( const String& rServ, const String& rSrc, const String& rNam,
                        const String& rUser, const String& rPass ) :
        aServiceName( rServ ), aParSource( rSrc ), aParName( rNam ),
        aParUser( rUser ), aParPass( rPass ) {  }

    BOOL operator== ( const ScDPServiceDesc& rOther ) const
        { return aServiceName == rOther.aServiceName &&
                 aParSource   == rOther.aParSource &&
                 aParName     == rOther.aParName &&
                 aParUser     == rOther.aParUser &&
                 aParPass     == rOther.aParPass; }
};


class SC_DLLPUBLIC ScDPObject : public DataObject
{
private:
    ScDocument*             pDoc;
                                            // settings
    ScDPSaveData*           pSaveData;
    String                  aTableName;
    String                  aTableTag;
    ScRange                 aOutRange;
    ScSheetSourceDesc*      pSheetDesc;     //  for sheet data
    ScImportSourceDesc*     pImpDesc;       //  for database data
    ScDPServiceDesc*        pServDesc;      //  for external service
                                            // cached data
    com::sun::star::uno::Reference<com::sun::star::sheet::XDimensionsSupplier> xSource;
    ScDPOutput*             pOutput;
    BOOL                    bSettingsChanged;
    BOOL                    bAlive;         // FALSE if only used to hold settings
    BOOL                    bAllowMove;
    BOOL                    bInfoValid;     // affects: nHeaderRows
    long                    nHeaderRows;    // page fields plus filter button


    SC_DLLPRIVATE void              CreateObjects();
    SC_DLLPRIVATE void              CreateOutput();

public:
                ScDPObject( ScDocument* pD );
                ScDPObject(const ScDPObject& r);
    virtual     ~ScDPObject();

    virtual DataObject* Clone() const;

    void                SetAlive(BOOL bSet);
    void                SetAllowMove(BOOL bSet);

    void                InvalidateData();
    void                InvalidateSource();

    void                Output();
    ScRange             GetNewOutputRange( BOOL& rOverflow );

    void                SetSaveData(const ScDPSaveData& rData);
    ScDPSaveData*       GetSaveData() const     { return pSaveData; }

    void                SetOutRange(const ScRange& rRange);
    const ScRange&      GetOutRange() const     { return aOutRange; }

    void                SetSheetDesc(const ScSheetSourceDesc& rDesc);
    void                SetImportDesc(const ScImportSourceDesc& rDesc);
    void                SetServiceData(const ScDPServiceDesc& rDesc);

    void                WriteSourceDataTo( ScDPObject& rDest ) const;
    void                WriteTempDataTo( ScDPObject& rDest ) const;

    const ScSheetSourceDesc* GetSheetDesc() const   { return pSheetDesc; }
    const ScImportSourceDesc* GetImportSourceDesc() const   { return pImpDesc; }
    const ScDPServiceDesc* GetDPServiceDesc() const { return pServDesc; }

    com::sun::star::uno::Reference<com::sun::star::sheet::XDimensionsSupplier> GetSource();

    BOOL                IsSheetData() const;
    BOOL                IsImportData() const { return(pImpDesc != NULL); }
    BOOL                IsServiceData() const { return(pServDesc != NULL); }

    void                SetName(const String& rNew);
    const String&       GetName() const                 { return aTableName; }
    void                SetTag(const String& rNew);
    const String&       GetTag() const                  { return aTableTag; }

    BOOL                IsDimNameInUse( const String& rName ) const;
    String              GetDimName( long nDim, BOOL& rIsDataLayout );
    BOOL                IsDuplicated( long nDim );
    long                GetDimCount();
    void                GetPositionData( ScDPPositionData& rData, const ScAddress& rPos );
    long                GetHeaderDim( const ScAddress& rPos, USHORT& rOrient );
    BOOL                GetHeaderDrag( const ScAddress& rPos, BOOL bMouseLeft, BOOL bMouseTop,
                                        long nDragDim,
                                        Rectangle& rPosRect, USHORT& rOrient, long& rDimPos );
    BOOL                IsFilterButton( const ScAddress& rPos );

    void                GetMemberResultNames( StrCollection& rNames, long nDimension );

    void                FillPageList( TypedStrCollection& rStrings, long nField );

    void                ToggleDetails( ScDPPositionData& rElemDesc, ScDPObject* pDestObj );

    BOOL                StoreOld(SvStream& rStream, ScMultipleWriteHeader& rHdr ) const;
    BOOL                StoreNew(SvStream& rStream, ScMultipleWriteHeader& rHdr ) const;
    BOOL                LoadNew(SvStream& rStream, ScMultipleReadHeader& rHdr );
    BOOL                FillOldParam(ScPivotParam& rParam, BOOL bForFile) const;
    BOOL                FillLabelData(ScPivotParam& rParam);
    void                InitFromOldPivot(const ScPivot& rOld, ScDocument* pDoc, BOOL bSetSource);

    BOOL                GetHierarchiesNA( sal_Int32 nDim, com::sun::star::uno::Reference< com::sun::star::container::XNameAccess >& xHiers );
    BOOL                GetHierarchies( sal_Int32 nDim, com::sun::star::uno::Sequence< rtl::OUString >& rHiers );

    sal_Int32           GetUsedHierarchy( sal_Int32 nDim );

    BOOL                GetMembersNA( sal_Int32 nDim, com::sun::star::uno::Reference< com::sun::star::container::XNameAccess >& xMembers );
    BOOL                GetMembers( sal_Int32 nDim,
                            com::sun::star::uno::Sequence< rtl::OUString >& rMembers,
                            com::sun::star::uno::Sequence< sal_Bool >* pVisible = 0,
                            com::sun::star::uno::Sequence< sal_Bool >* pShowDet = 0 );

    BOOL                GetMembersNA( sal_Int32 nDim, sal_Int32 nHier, com::sun::star::uno::Reference< com::sun::star::container::XNameAccess >& xMembers );
    BOOL                GetMembers( sal_Int32 nDim, sal_Int32 nHier,
                            com::sun::star::uno::Sequence< rtl::OUString >& rMembers,
                            com::sun::star::uno::Sequence< sal_Bool >* pVisible = 0,
                            com::sun::star::uno::Sequence< sal_Bool >* pShowDet = 0 );

    void                UpdateReference( UpdateRefMode eUpdateRefMode,
                                         const ScRange& r, SCsCOL nDx, SCsROW nDy, SCsTAB nDz );
    BOOL                RefsEqual( const ScDPObject& r ) const;
    void                WriteRefsTo( ScDPObject& r ) const;

    // apply drop-down attribute, initialize nHeaderRows, without accessing the source
    // (button attribute must be present)
    void                RefreshAfterLoad();

    static BOOL         HasRegisteredSources();
    static com::sun::star::uno::Sequence<rtl::OUString> GetRegisteredSources();
    static com::sun::star::uno::Reference<com::sun::star::sheet::XDimensionsSupplier>
                        CreateSource( const ScDPServiceDesc& rDesc );

    static void         ConvertOrientation( ScDPSaveData& rSaveData,
                            PivotField* pFields, SCSIZE nCount, USHORT nOrient,
                            ScDocument* pDoc, SCROW nRow, SCTAB nTab,
                            const com::sun::star::uno::Reference<
                                com::sun::star::sheet::XDimensionsSupplier>& xSource,
                            BOOL bOldDefaults,
                            PivotField* pRefColFields = NULL, SCSIZE nRefColCount = 0,
                            PivotField* pRefRowFields = NULL, SCSIZE nRefRowCount = 0,
                            PivotField* pRefPageFields = NULL, SCSIZE nRefPageCount = 0 );
};


class ScDPCollection : public Collection
{
private:
    ScDocument* pDoc;

public:
                ScDPCollection(ScDocument* pDocument);
                ScDPCollection(const ScDPCollection& r);
    virtual     ~ScDPCollection();

    virtual DataObject* Clone() const;

    ScDPObject* operator[](USHORT nIndex) const {return (ScDPObject*)At(nIndex);}

    BOOL        StoreOld( SvStream& rStream ) const;
    BOOL        StoreNew( SvStream& rStream ) const;
    BOOL        LoadNew( SvStream& rStream );

    void        ConvertOldTables( ScPivotCollection& rOldColl );

    void        DeleteOnTab( SCTAB nTab );
    void        UpdateReference( UpdateRefMode eUpdateRefMode,
                                 const ScRange& r, SCsCOL nDx, SCsROW nDy, SCsTAB nDz );

    BOOL        RefsEqual( const ScDPCollection& r ) const;
    void        WriteRefsTo( ScDPCollection& r ) const;

    String      CreateNewName( USHORT nMin = 1 ) const;
    void        EnsureNames();
};


#endif

