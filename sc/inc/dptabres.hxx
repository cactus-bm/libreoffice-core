/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef SC_DPTABRES_HXX
#define SC_DPTABRES_HXX

#include <svl/svarray.hxx>
#include <tools/string.hxx>
#include <com/sun/star/sheet/MemberResult.hpp>
#include <com/sun/star/sheet/DataResult.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include "global.hxx"       // enum ScSubTotalFunc
#include "dpcachetable.hxx"
#include <hash_map>
#include <hash_set>
#include <vector>
#include <memory>

namespace com { namespace sun { namespace star { namespace sheet {
    struct DataPilotFieldReference;
} } } }


class ScAddress;
class ScDocument;
class ScDPSource;
class ScDPDimension;
class ScDPDimensions;
class ScDPLevel;
class ScDPMember;
class ScDPAggData;
class ScDPResultMember;
class ScDPResultVisibilityData;

struct ScDPValueData;
// Wang Xu Ming -- 2009-8-17
// DataPilot Migration - Cache&&Performance
class ScDPItemData;
// End Comments
//
//  Member names that are being processed for InitFrom/LateInitFrom
//  (needed for initialization of grouped items)
//

class ScDPInitState
{
    long*           pIndex;     // array
// Wang Xu Ming -- 2009-8-17
// DataPilot Migration - Cache&&Performance
    SCROW*     pData; // array
 // End Comments
    long            nCount;

public:
            ScDPInitState();
            ~ScDPInitState();

    void    AddMember( long nSourceIndex,SCROW nMember);
    void    RemoveMember();

    long                GetCount() const    { return nCount; }
    const long*         GetSource() const   { return pIndex; }
// Wang Xu Ming -- 2009-8-17
// DataPilot Migration - Cache&&Performance
    const SCROW* GetNameIds() const    { return pData; }
    SCROW   GetNameIdForIndex( long nIndexValue ) const;
// End Comments
};

typedef ::std::vector<sal_Int32> ScMemberSortOrder;

//
//  selected subtotal information, passed down the dimensions
//

struct ScDPSubTotalState
{
    ScSubTotalFunc eColForce;
    ScSubTotalFunc eRowForce;
    long nColSubTotalFunc;
    long nRowSubTotalFunc;

    ScDPSubTotalState() :
        eColForce( SUBTOTAL_FUNC_NONE ),
        eRowForce( SUBTOTAL_FUNC_NONE ),
        nColSubTotalFunc( -1 ),
        nRowSubTotalFunc( -1 )
    {}
};

//
//  indexes when calculating running totals
//  Col/RowVisible: simple counts from 0 - without sort order applied - visible index
//                  (only used for running total / relative index)
//  Col/RowIndexes: with sort order applied - member index
//                  (used otherwise - so other members' children can be accessed)
//

class ScDPRunningTotalState
{
    ScDPResultMember*   pColResRoot;
    ScDPResultMember*   pRowResRoot;
    long*               pColVisible;
    long*               pColIndexes;
    long*               pRowVisible;
    long*               pRowIndexes;
    long                nColIndexPos;
    long                nRowIndexPos;

public:
            ScDPRunningTotalState( ScDPResultMember* pColRoot, ScDPResultMember* pRowRoot );
            ~ScDPRunningTotalState();

    ScDPResultMember*   GetColResRoot() const   { return pColResRoot; }
    ScDPResultMember*   GetRowResRoot() const   { return pRowResRoot; }

    const long*         GetColVisible() const   { return pColVisible; }
    const long*         GetColIndexes() const   { return pColIndexes; }
    const long*         GetRowVisible() const   { return pRowVisible; }
    const long*         GetRowIndexes() const   { return pRowIndexes; }

    void    AddColIndex( long nVisible, long nSorted );
    void    AddRowIndex( long nVisible, long nSorted );
    void    RemoveColIndex();
    void    RemoveRowIndex();
};

struct ScDPRelativePos
{
    long    nBasePos;       // simple count, without sort order applied
    long    nDirection;

    ScDPRelativePos( long nBase, long nDir );
};

//
//  aggregated data
//! separate header file?
//

//  Possible values for the nCount member:
//  (greater than 0 counts the collected values)
const long SC_DPAGG_EMPTY        =  0;  // empty during data collection
const long SC_DPAGG_DATA_ERROR   = -1;  // error during data collection
const long SC_DPAGG_RESULT_EMPTY = -2;  // empty result calculated
const long SC_DPAGG_RESULT_VALID = -3;  // valid result calculated
const long SC_DPAGG_RESULT_ERROR = -4;  // error in calculated result

class ScDPAggData
{
private:
    double          fVal;
    double          fAux;
    long            nCount;
    ScDPAggData*    pChild;

public:
            ScDPAggData() : fVal(0.0), fAux(0.0), nCount(SC_DPAGG_EMPTY), pChild(NULL) {}
            ~ScDPAggData() { delete pChild; }

    void    Update( const ScDPValueData& rNext, ScSubTotalFunc eFunc, const ScDPSubTotalState& rSubState );
    void    Calculate( ScSubTotalFunc eFunc, const ScDPSubTotalState& rSubState );
    BOOL    IsCalculated() const;

    double  GetResult() const;
    BOOL    HasError() const;
    BOOL    HasData() const;

    void    SetResult( double fNew );
    void    SetEmpty( BOOL bSet );
    void    SetError();

    double  GetAuxiliary() const;
    void    SetAuxiliary( double fNew );

    void    Reset();        // also deletes children

    const ScDPAggData*  GetExistingChild() const    { return pChild; }
    ScDPAggData*        GetChild();
};

//
//  Row and grand total state, passed down (column total is at result member)
//

class ScDPRowTotals
{
    ScDPAggData aRowTotal;
    ScDPAggData aGrandTotal;
    BOOL        bIsInColRoot;

public:
            ScDPRowTotals();
            ~ScDPRowTotals();

    ScDPAggData*    GetRowTotal( long nMeasure );
    ScDPAggData*    GetGrandTotal( long nMeasure );

    BOOL            IsInColRoot() const     { return bIsInColRoot; }
    void            SetInColRoot(BOOL bSet) { bIsInColRoot = bSet; }
};

// --------------------------------------------------------------------
//
//  results for a hierarchy dimension
//

#define SC_DP_RES_GROW  16

class ScDPResultDimension;
class ScDPDataDimension;
class ScDPDataMember;

#define SC_DPMEASURE_ALL    -1
#define SC_DPMEASURE_ANY    -2
// Wang Xu Ming -- 2009-8-17
// DataPilot Migration - Cache&&Performance

struct MemberHashIndexFunc : public std::unary_function< const SCROW &, size_t >
{
    size_t operator() (const SCROW &rDataIndex) const { return rDataIndex; }
};

class ScDPParentDimData
{
public:
    const SCROW                     mnOrder;                        //! Ref
    const ScDPDimension*            mpParentDim;            //! Ref
    const ScDPLevel*                mpParentLevel;          //! Ref
    const ScDPMember*               mpMemberDesc;           //! Ref

    ScDPParentDimData():mnOrder(-1), mpParentDim( NULL), mpParentLevel( NULL ), mpMemberDesc( NULL ){}
    ScDPParentDimData( const SCROW nIndex, ScDPDimension* pDim, const ScDPLevel* pLev, const ScDPMember* pMember ): mnOrder( nIndex ), mpParentDim( pDim), mpParentLevel( pLev ), mpMemberDesc( pMember ){}
};

typedef std::vector <ScDPParentDimData *>                 DimMemberArray;
typedef std::hash_map < SCROW, ScDPParentDimData *, MemberHashIndexFunc>  DimMemberHash;

class ResultMembers
{
    DimMemberHash      maMemberHash;
    BOOL                mbHasHideDetailsMember;
public:
    ScDPParentDimData* FindMember( const SCROW& nIndex ) const;
    void                             InsertMember(  ScDPParentDimData* pNew );
    BOOL            IsHasHideDetailsMembers() const { return mbHasHideDetailsMember; }
    void            SetHasHideDetailsMembers( BOOL b ) { mbHasHideDetailsMember=b; }
    ResultMembers();
    virtual ~ResultMembers();
};

class LateInitParams
{
private:
    const ::std::vector<ScDPDimension*>& mppDim;
    const ::std::vector<ScDPLevel*>& mppLev;

    BOOL      mbRow;
    BOOL      mbInitChild;
    BOOL      mbAllChildren;
public:
    LateInitParams( const ::std::vector<ScDPDimension*>& ppDim, const ::std::vector<ScDPLevel*>& ppLev,
        BOOL bRow, BOOL bInitChild = TRUE , BOOL bAllChildren = FALSE);
    ~LateInitParams();

    void  SetInitChild( BOOL b ) { mbInitChild = b; }
    void  SetInitAllChildren( BOOL b ) { mbAllChildren = b; }

    inline ScDPDimension* GetDim( size_t nPos ) const { return mppDim[nPos];}
    inline ScDPLevel*         GetLevel( size_t nPos ) const { return mppLev[nPos];}

    inline BOOL  GetInitChild() const {return mbInitChild; }
    inline BOOL  GetInitAllChild() const { return mbAllChildren; }
    inline BOOL  IsRow() const { return mbRow; }
    BOOL  IsEnd( size_t nPos ) const ;
};
// End Comments

class ScDPResultData
{
private:
    ScDPSource*             pSource;                //! Ref
    //! keep things like measure lists here

    long                    nMeasCount;
    ScSubTotalFunc*         pMeasFuncs;
    ::com::sun::star::sheet::DataPilotFieldReference* pMeasRefs;
    USHORT*                 pMeasRefOrient;
    String*                 pMeasNames;
    BOOL                    bLateInit;
    BOOL                    bDataAtCol;
    BOOL                    bDataAtRow;

    //! add "displayed values" settings
        mutable std::vector< ResultMembers* > mpDimMembers;
public:
                        ScDPResultData( ScDPSource* pSrc );     //! Ref
                        ~ScDPResultData();

    void                SetMeasureData( long nCount, const ScSubTotalFunc* pFunctions,
                                        const ::com::sun::star::sheet::DataPilotFieldReference* pRefs,
                                        const USHORT* pRefOrient, const String* pNames );
    void                SetDataLayoutOrientation( USHORT nOrient );
    void                SetLateInit( BOOL bSet );

    long                GetMeasureCount() const     { return nMeasCount; }
    ScSubTotalFunc      GetMeasureFunction(long nMeasure) const;
    String              GetMeasureString(long nMeasure, BOOL bForce, ScSubTotalFunc eForceFunc, bool& rbTotalResult) const;
    String              GetMeasureDimensionName(long nMeasure) const;
    const ::com::sun::star::sheet::DataPilotFieldReference& GetMeasureRefVal(long nMeasure) const;
    USHORT              GetMeasureRefOrient(long nMeasure) const;

    BOOL                IsDataAtCol() const             { return bDataAtCol; }
    BOOL                IsDataAtRow() const             { return bDataAtRow; }
    BOOL                IsLateInit() const              { return bLateInit; }

    long                GetColStartMeasure() const;
    long                GetRowStartMeasure() const;

    long                GetCountForMeasure( long nMeas ) const
                                { return ( nMeas == SC_DPMEASURE_ALL ) ? nMeasCount : 1; }

    BOOL                IsBaseForGroup( long nDim ) const;              // any group
    long                GetGroupBase( long nGroupDim ) const;
    BOOL                IsNumOrDateGroup( long nDim ) const;
 // Wang Xu Ming -- 2009-8-17
// DataPilot Migration - Cache&&Performance
    BOOL                IsInGroup( const ScDPItemData& rGroupData, long nGroupIndex,
                                               long nBaseDataId, long nBaseIndex ) const;
    BOOL                IsInGroup( SCROW nGroupDataId, long nGroupIndex,
                                              const ScDPItemData& rBaseData, long nBaseIndex ) const;
    BOOL                HasCommonElement( SCROW nFirstDataId, long nFirstIndex,
                                          const ScDPItemData& rSecondData, long nSecondIndex ) const;

    ResultMembers* GetDimResultMembers( long nDim , ScDPDimension* pDim , ScDPLevel*   pLevel) const ;

// End Comments
    const ScDPSource*   GetSource() const;
};


class ScDPResultMember
{
private:
    const ScDPResultData*   pResultData;
    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    ScDPParentDimData        aParentDimData;
    // End Comments
    ScDPResultDimension*    pChildDimension;
    ScDPDataMember*         pDataRoot;
    BOOL                    bHasElements;
    BOOL                    bForceSubTotal;
    BOOL                    bHasHiddenDetails;
    BOOL                    bInitialized;
    BOOL                    bAutoHidden;
    ScDPAggData             aColTotal;              // to store column totals

    USHORT                  nMemberStep;            // step to show details
public:
    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    ScDPResultMember(  const ScDPResultData* pData,  const ScDPParentDimData& rParentDimData,
        BOOL bForceSub );  //! Ref
    ScDPResultMember(  const ScDPResultData* pData, BOOL bForceSub );
    // End Comments
    ~ScDPResultMember();

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    void                InitFrom( const ::std::vector<ScDPDimension*>& ppDim,
                                        const ::std::vector<ScDPLevel*>& ppLev,
                                        size_t nPos,
                                        ScDPInitState& rInitState,
                                        BOOL bInitChild = TRUE );
    void               LateInitFrom(
                                        LateInitParams& rParams,
                                        const ::std::vector< SCROW >& pItemData,
                                        size_t nPos,
                                        ScDPInitState& rInitState);
    void              CheckShowEmpty( BOOL bShow = FALSE );
    // End Comments
    String              GetName() const;
    void                FillItemData( ScDPItemData& rData ) const;
    BOOL                IsValid() const;
    BOOL                IsVisible() const;
    long                GetSize(long nMeasure) const;
    BOOL                HasHiddenDetails() const;
    BOOL                IsSubTotalInTitle(long nMeasure) const;

//  BOOL                SubTotalEnabled() const;
    long                GetSubTotalCount( long* pUserSubStart = NULL ) const;

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    BOOL              IsNamedItem( SCROW nIndex ) const;
    bool IsValidEntry( const ::std::vector< SCROW >& aMembers ) const;
    // End Comments

    void                SetHasElements()    { bHasElements = TRUE; }
    void                SetAutoHidden()     { bAutoHidden = TRUE; }

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    void                ProcessData( const ::std::vector<SCROW>& aChildMembers,
                                        const ScDPResultDimension* pDataDim,
                                        const ::std::vector<SCROW>& aDataMembers,
                                        const ::std::vector<ScDPValueData>& aValues );
    // End Comments
    void                FillMemberResults( com::sun::star::uno::Sequence<
                                                com::sun::star::sheet::MemberResult>* pSequences,
                                            long& rPos, long nMeasure, BOOL bRoot,
                                            const String* pMemberName,
                                            const String* pMemberCaption );

    void                FillDataResults( const ScDPResultMember* pRefMember,
                                    com::sun::star::uno::Sequence<
                                        com::sun::star::uno::Sequence<
                                            com::sun::star::sheet::DataResult> >& rSequence,
                                    long& rRow, long nMeasure ) const;

    void                UpdateDataResults( const ScDPResultMember* pRefMember, long nMeasure ) const;
    void                UpdateRunningTotals( const ScDPResultMember* pRefMember, long nMeasure,
                                                ScDPRunningTotalState& rRunning, ScDPRowTotals& rTotals ) const;

    void                SortMembers( ScDPResultMember* pRefMember );
    void                DoAutoShow( ScDPResultMember* pRefMember );

    void                ResetResults( BOOL bRoot );

    void                DumpState( const ScDPResultMember* pRefMember, ScDocument* pDoc, ScAddress& rPos ) const;

                        //! this will be removed!
    const ScDPResultDimension*  GetChildDimension() const   { return pChildDimension; }
    ScDPResultDimension*        GetChildDimension()         { return pChildDimension; }

    ScDPDataMember*         GetDataRoot() const             { return pDataRoot; }

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    const ScDPDimension*  GetParentDim() const               { return aParentDimData.mpParentDim; }     //! Ref
    const ScDPLevel*         GetParentLevel() const         { return aParentDimData.mpParentLevel; }   //! Ref
    const ScDPMember*     GetDPMember()const              { return aParentDimData.mpMemberDesc; }    //! Ref
    inline SCROW               GetOrder() const                           { return aParentDimData.mnOrder; }         //! Ref
    inline BOOL                  IsRoot() const                       { return GetParentLevel() == NULL; }
    SCROW                       GetDataId( ) const ;
    // End Comments
    ScDPAggData*        GetColTotal( long nMeasure ) const;

    void                FillVisibilityData(ScDPResultVisibilityData& rData) const;
};

class ScDPDataMember
{
private:
    const ScDPResultData*       pResultData;
    const ScDPResultMember*     pResultMember;          //! Ref?
    ScDPDataDimension*      pChildDimension;
    ScDPAggData             aAggregate;

    void                UpdateValues( const ::std::vector<ScDPValueData>& aValues, const ScDPSubTotalState& rSubState );

public:
                        ScDPDataMember( const ScDPResultData* pData, const ScDPResultMember* pRes );
                        ~ScDPDataMember();

    void                InitFrom( const ScDPResultDimension* pDim );

    String              GetName() const;
    BOOL                IsVisible() const;
    BOOL                HasData( long nMeasure, const ScDPSubTotalState& rSubState ) const;

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    BOOL              IsNamedItem(   SCROW r ) const;
    // End Comments
    BOOL                HasHiddenDetails() const;

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    void                ProcessData( const ::std::vector< SCROW >& aChildMembers, const ::std::vector<ScDPValueData>& aValues,
                                       const ScDPSubTotalState& rSubState );
    // End Comments
    BOOL                HasError( long nMeasure, const ScDPSubTotalState& rSubState ) const;
    double              GetAggregate( long nMeasure, const ScDPSubTotalState& rSubState ) const;
    const ScDPAggData*  GetConstAggData( long nMeasure, const ScDPSubTotalState& rSubState ) const;
    ScDPAggData*        GetAggData( long nMeasure, const ScDPSubTotalState& rSubState );

    void                FillDataRow( const ScDPResultMember* pRefMember,
                                    com::sun::star::uno::Sequence<com::sun::star::sheet::DataResult>& rSequence,
                                    long& rCol, long nMeasure, BOOL bIsSubTotalRow,
                                    const ScDPSubTotalState& rSubState ) const;

    void                UpdateDataRow( const ScDPResultMember* pRefMember, long nMeasure, BOOL bIsSubTotalRow,
                                    const ScDPSubTotalState& rSubState );
    void                UpdateRunningTotals( const ScDPResultMember* pRefMember, long nMeasure, BOOL bIsSubTotalRow,
                                    const ScDPSubTotalState& rSubState, ScDPRunningTotalState& rRunning,
                                    ScDPRowTotals& rTotals, const ScDPResultMember& rRowParent );

    void                SortMembers( ScDPResultMember* pRefMember );
    void                DoAutoShow( ScDPResultMember* pRefMember );

    void                ResetResults();

    void                DumpState( const ScDPResultMember* pRefMember, ScDocument* pDoc, ScAddress& rPos ) const;

                        //! this will be removed!
    const ScDPDataDimension*    GetChildDimension() const   { return pChildDimension; }
    ScDPDataDimension*          GetChildDimension()         { return pChildDimension; }
};

//! replace PtrArr with 32-bit array ????

typedef ScDPDataMember* ScDPDataMemberPtr;
SV_DECL_PTRARR_DEL(ScDPDataMembers, ScDPDataMemberPtr, SC_DP_RES_GROW, SC_DP_RES_GROW)


//  result dimension contains only members

class ScDPResultDimension
{
public :
    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    typedef std::vector <ScDPResultMember *>                           MemberArray;
    typedef std::map < SCROW , ScDPResultMember *> MemberHash;
    // End Comments
private:
        const ScDPResultData*   pResultData;
    MemberArray             maMemberArray;
    MemberHash              maMemberHash;
    BOOL                    bInitialized;
    String                  aDimensionName;     //! or ptr to IntDimension?
    BOOL                    bIsDataLayout;      //! or ptr to IntDimension?
    BOOL                    bSortByData;
    BOOL                    bSortAscending;
    long                    nSortMeasure;
    ScMemberSortOrder       aMemberOrder;       // used when sorted by measure
    BOOL                    bAutoShow;
    BOOL                    bAutoTopItems;
    long                    nAutoMeasure;
    long                    nAutoCount;

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    ScDPResultMember*        FindMember(   SCROW  iData ) const;
    ScDPResultMember*        AddMember( const ScDPParentDimData& aData );
    ScDPResultMember*        InsertMember( ScDPParentDimData* pMemberData );
    void                                  InitWithMembers( LateInitParams& rParams,
                                                            const ::std::vector< SCROW >& pItemData,
                                                            size_t  nPos,
                                                            ScDPInitState& rInitState  );
    // End Comments
public:
       ScDPResultDimension( const ScDPResultData* pData );
    ~ScDPResultDimension();

                        //  allocates new members
    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    void                InitFrom( const ::std::vector<ScDPDimension*>& ppDim,
                                        const ::std::vector<ScDPLevel*>& ppLev,
                                        size_t nPos,
                                        ScDPInitState& rInitState ,  BOOL bInitChild = TRUE );
    void                LateInitFrom(  LateInitParams& rParams,
                                        const ::std::vector< SCROW >& pItemData,
                                        size_t nPos,
                                        ScDPInitState& rInitState );
    void               CheckShowEmpty( BOOL bShow = FALSE );

    // End Comments
    long                GetSize(long nMeasure) const;

    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    bool                IsValidEntry( const ::std::vector<SCROW>& aMembers ) const;

    //  modifies existing members, allocates data dimensions
    void                ProcessData( const ::std::vector<SCROW>& aMembers,
                                     const ScDPResultDimension* pDataDim,
                                     const ::std::vector<SCROW>& aDataMembers,
                                     const ::std::vector<ScDPValueData>& aValues ) const;   //! Test
 // End Comments
    void                FillMemberResults( com::sun::star::uno::Sequence<
                                                com::sun::star::sheet::MemberResult>* pSequences,
                                            long nStart, long nMeasure );

    void                FillDataResults( const ScDPResultMember* pRefMember,
                                    com::sun::star::uno::Sequence<
                                        com::sun::star::uno::Sequence<
                                            com::sun::star::sheet::DataResult> >& rSequence,
                                    long nRow, long nMeasure ) const;

    void                UpdateDataResults( const ScDPResultMember* pRefMember, long nMeasure ) const;
    void                UpdateRunningTotals( const ScDPResultMember* pRefMember, long nMeasure,
                                            ScDPRunningTotalState& rRunning, ScDPRowTotals& rTotals ) const;

    void                SortMembers( ScDPResultMember* pRefMember );
    long                GetSortedIndex( long nUnsorted ) const;

    void                DoAutoShow( ScDPResultMember* pRefMember );

    void                ResetResults();

                        //  called for the reference dimension
    ScDPDataMember*     GetRowReferenceMember( const ScDPRelativePos* pMemberPos, const String* pName,
                                    const long* pRowIndexes, const long* pColIndexes ) const;

                        //  uses row root member from ScDPRunningTotalState
    static ScDPDataMember* GetColReferenceMember( const ScDPRelativePos* pMemberPos, const String* pName,
                                    long nRefDimPos, const ScDPRunningTotalState& rRunning );

    void                DumpState( const ScDPResultMember* pRefMember, ScDocument* pDoc, ScAddress& rPos ) const;

                        //  for ScDPDataDimension::InitFrom
    long                GetMemberCount() const;
    const ScDPResultMember* GetMember(long n) const;
    ScDPResultMember*       GetMember(long n);

    const ScMemberSortOrder& GetMemberOrder() const     { return aMemberOrder; }
    ScMemberSortOrder&  GetMemberOrder()                { return aMemberOrder; }

    BOOL                IsDataLayout() const    { return bIsDataLayout; }
    String              GetName() const         { return aDimensionName; }

    BOOL                IsSortByData() const    { return bSortByData; }
    BOOL                IsSortAscending() const { return bSortAscending; }
    long                GetSortMeasure() const  { return nSortMeasure; }

    BOOL                IsAutoShow() const      { return bAutoShow; }
    BOOL                IsAutoTopItems() const  { return bAutoTopItems; }
    long                GetAutoMeasure() const  { return nAutoMeasure; }
    long                GetAutoCount() const    { return nAutoCount; }

    ScDPResultDimension* GetFirstChildDimension() const;

    void                FillVisibilityData(ScDPResultVisibilityData& rData) const;
};

class ScDPDataDimension
{
private:
    const ScDPResultData*       pResultData;
    const ScDPResultDimension* pResultDimension;  // column
    ScDPDataMembers     aMembers;
    BOOL                bIsDataLayout;      //! or ptr to IntDimension?

public:
                        ScDPDataDimension( const ScDPResultData* pData );
                        ~ScDPDataDimension();

    void                InitFrom( const ScDPResultDimension* pDim );        // recursive
    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    void                ProcessData( const ::std::vector< SCROW >& aDataMembers, const ::std::vector<ScDPValueData>& aValues,
                                       const ScDPSubTotalState& rSubState );
    // End Comments
    void                FillDataRow( const ScDPResultDimension* pRefDim,
                                    com::sun::star::uno::Sequence<com::sun::star::sheet::DataResult>& rSequence,
                                    long nCol, long nMeasure, BOOL bIsSubTotalRow,
                                    const ScDPSubTotalState& rSubState ) const;

    void                UpdateDataRow( const ScDPResultDimension* pRefDim, long nMeasure, BOOL bIsSubTotalRow,
                                    const ScDPSubTotalState& rSubState ) const;
    void                UpdateRunningTotals( const ScDPResultDimension* pRefDim, long nMeasure, BOOL bIsSubTotalRow,
                                    const ScDPSubTotalState& rSubState, ScDPRunningTotalState& rRunning,
                                    ScDPRowTotals& rTotals, const ScDPResultMember& rRowParent ) const;

    void                SortMembers( ScDPResultDimension* pRefDim );
    long                GetSortedIndex( long nUnsorted ) const;

    void                DoAutoShow( ScDPResultDimension* pRefDim );

    void                ResetResults();

    void                DumpState( const ScDPResultDimension* pRefDim, ScDocument* pDoc, ScAddress& rPos ) const;

    long                GetMemberCount() const;
    ScDPDataMember*     GetMember(long n) const;
};

// ----------------------------------------------------------------------------

/**
 * This class collects visible members of each dimension and uses that
 * information to create filtering criteria (e.g. for drill-down data).
 */
class ScDPResultVisibilityData
{
public:
    // Wang Xu Ming -- 2009-8-17
    // DataPilot Migration - Cache&&Performance
    ScDPResultVisibilityData( ScDPSource* pSource);
    // End Comments
    ~ScDPResultVisibilityData();

    void addVisibleMember(const String& rDimName, const ScDPItemData& rMemberItem);
    void fillFieldFilters(::std::vector<ScDPCacheTable::Criterion>& rFilters) const;

private:
    struct MemberHash
    {
        size_t operator()(const ScDPItemData& r) const;
    };
    typedef ::std::hash_set<ScDPItemData, MemberHash> VisibleMemberType;
    typedef ::std::hash_map<String, VisibleMemberType, ScStringHashCode> DimMemberType;
    DimMemberType maDimensions;

    ScDPSource* mpSource;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
