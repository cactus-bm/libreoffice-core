/*************************************************************************
 *
 *  $RCSfile: colrowst.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: dr $ $Date: 2001-05-11 09:19:40 $
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

#include "colrowst.hxx"

#include <string.h>

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
#ifndef _SVX_BOXITEM_HXX
#include <svx/boxitem.hxx>
#endif

#include "document.hxx"
#include "XclImpStream.hxx"
#include "root.hxx"
#include "xfbuff.hxx"



ColRowSettings::ColRowSettings( void )
{
    pHorizPb = pVertPb = NULL;

    nDefWidth = nDefHeight = 0;

    pWidth = new INT32 [ MAXCOL + 1 ];
    pColHidden = new BOOL [ MAXCOL + 1 ];

    pHeight = new UINT16 [ MAXROW + 1 ];
    pRowFlags = new INT8[ MAXROW + 1 ];

    pExtTabOpt = NULL;

    Reset();
}


ColRowSettings::~ColRowSettings()
{
    if( pHorizPb )
        delete pHorizPb;

    if( pVertPb )
        delete pVertPb;

    delete[] pRowFlags;
    delete[] pHeight;
    delete[] pColHidden;
    delete[] pWidth;

    if( pExtTabOpt )
        delete pExtTabOpt;
}


void ColRowSettings::Reset( void )
{
    UINT16  nC;
    for( nC = 0 ; nC <= MAXCOL ; nC++ )
    {
        pColHidden[ nC ] = FALSE;
        pWidth[ nC ] = -1;
    }

    memset( pRowFlags, 0x00, sizeof( INT8 ) * ( MAXROW + 1 ) );

    if( pExtTabOpt )
    {
        delete pExtTabOpt;
        pExtTabOpt = NULL;
    }

    if( pHorizPb )
    {
        delete pHorizPb;
        pHorizPb = NULL;
    }

    if( pVertPb )
    {
        delete pVertPb;
        pVertPb = NULL;
    }

    bDirty = TRUE;
    nMaxRow = 0;

    bSetByStandard = FALSE;
}


void ColRowSettings::Apply( const UINT16 nAktTab )
{
    if( !bDirty )
        return;

    UINT16                  nC;
    UINT16                  nStart = 0;
    UINT16                  nWidth;
    UINT16                  nLastWidth = ( pWidth[ 0 ] >= 0 )? ( UINT16 ) pWidth[ 0 ] : nDefWidth;
    ScDocument&             rD = *pExcRoot->pDoc;
    BOOL                    bExtraHide;

    rD.IncSizeRecalcLevel( nAktTab );

    // Column-Bemachung
    for( nC = 0 ; nC <= MAXCOL ; nC++ )
    {
        if( pWidth[ nC ] >= 0 )
            // eingestellte Width
            nWidth = ( UINT16 ) pWidth[ nC ];
        else
            // Default-Width
            nWidth = nDefWidth;

        bExtraHide = ( nWidth == 0 );

        if( !bExtraHide )
            rD.SetColWidth( nC, nAktTab, nWidth );

        if( pColHidden[ nC ] || bExtraHide )
            // Column versteckt
            rD.SetColFlags( nC, nAktTab, rD.GetColFlags( nC, nAktTab ) | CR_HIDDEN );
    }

    // Row-Bemachung

    INT8                    nFlags;
    nStart = 0;
    UINT16                  nHeight;

    UINT16                  nLastHeight;
    nFlags = pRowFlags[ 0 ];
    if( nFlags & ROWFLAG_USED )
    {
        if( nFlags & ROWFLAG_DEFAULT )
            nLastHeight = nDefHeight;
        else
        {
            nLastHeight = pHeight[ 0 ];
            if( !nLastHeight )
                nLastHeight = nDefHeight;
        }
    }
    else
        nLastHeight = nDefHeight;

    for( nC = 0 ; nC <= nMaxRow ; nC++ )
    {
        nFlags = pRowFlags[ nC ];

        if( nFlags & ROWFLAG_USED )
        {
            if( nFlags & ROWFLAG_DEFAULT )
                nHeight = nDefHeight;
            else
            {
                nHeight = pHeight[ nC ];
                if( !nHeight )
                    nHeight = nDefHeight;
            }

            if( nFlags & ( ROWFLAG_HIDDEN | ROWFLAG_MAN ) )
            {
                BYTE        nSCFlags = rD.GetRowFlags( nC, nAktTab );

                if( nFlags & ROWFLAG_HIDDEN )
                    nSCFlags |= CR_HIDDEN;

                if( nFlags & ROWFLAG_MAN )
                    nSCFlags |= CR_MANUALSIZE;

                rD.SetRowFlags( nC, nAktTab, nSCFlags );
            }
        }
        else
            nHeight = nDefHeight;

        if( !nHeight )
            rD.SetRowFlags( nC, nAktTab, rD.GetRowFlags( nC, nAktTab ) | CR_HIDDEN );

        if( nLastHeight != nHeight )
        {
            DBG_ASSERT( nC, "ColRowSettings::Apply(): Algorithmus-Fehler!" );

            if( nLastHeight )
                rD.SetRowHeightRange( nStart, nC - 1, nAktTab, nLastHeight );

            nStart = nC;
            nLastHeight = nHeight;
        }
    }

    if( nLastHeight )
        rD.SetRowHeightRange( nStart, nMaxRow, nAktTab, nLastHeight );

    if( pExtTabOpt )
        pExcRoot->pExtDocOpt->Add( *this );

    if( pHorizPb )
        {
        UINT16  n = pHorizPb->First();

        while( n && n <= MAXROW )
        {
            rD.SetRowFlags( n, nAktTab, rD.GetRowFlags( n, nAktTab ) | CR_MANUALBREAK );
            n = pHorizPb->Next();
        }
    }

    if( pVertPb )
    {
        UINT16  n = pVertPb->First();

        while( n && n <= MAXCOL )
        {
            rD.SetColFlags( n, nAktTab, rD.GetColFlags( n, nAktTab ) | CR_MANUALBREAK );
            n = pVertPb->Next();
        }
    }

    bDirty = FALSE; // jetzt stimmt Tabelle im ScDocument

    rD.DecSizeRecalcLevel( nAktTab );
}


void ColRowSettings::HideColRange( UINT16 nColFirst, UINT16 nColLast )
{
    DBG_ASSERT( nColFirst <= nColLast, "+ColRowSettings::HideColRange(): First > Last?!" );
    DBG_ASSERT( nColLast <= MAXCOL, "+ColRowSettings::HideColRange(): ungueltige Column" );

    if( nColLast > MAXCOL )
        nColLast = MAXCOL;

    BOOL*   pHidden;
    BOOL*   pFinish;
    pHidden = &pColHidden[ nColFirst ];
    pFinish = &pColHidden[ nColLast ];
    while( pHidden <= pFinish )
        *( pHidden++ ) = TRUE;
}


void ColRowSettings::SetWidthRange( UINT16 nColFirst, UINT16 nColLast, UINT16 nNew )
{
    DBG_ASSERT( nColFirst <= nColLast, "+ColRowSettings::SetColWidthRange(): First > Last?!" );
    DBG_ASSERT( nColLast <= MAXCOL, "+ColRowSettings::SetColWidthRange(): ungueltige Column" );

    if( nColLast > MAXCOL )
        nColLast = MAXCOL;

    INT32*  pWidthCount;
    INT32*  pFinish;
    pWidthCount = &pWidth[ nColFirst ];
    pFinish = &pWidth[ nColLast ];

    while( pWidthCount <= pFinish )
        *( pWidthCount++ ) = nNew;
}


void ColRowSettings::SetDefaultXF( UINT16 nColFirst, UINT16 nColLast, UINT16 nXF )
{
    DBG_ASSERT( nColFirst <= nColLast, "+ColRowSettings::SetDefaultXF(): First > Last?!" );
    DBG_ASSERT( nColLast <= MAXCOL, "+ColRowSettings::SetDefaultXF(): ungueltige Column" );

    if( nColLast > MAXCOL )
        nColLast = MAXCOL;

    ScDocument&         rDoc = *pExcRoot->pDoc;
    XF_Buffer&          rXFBuff = *pExcRoot->pXF_Buffer;
    const UINT16        nTab = *pExcRoot->pAktTab;

    for( UINT16 n = nColFirst ; n <= nColLast ; n++ )
        rDoc.ApplyPatternAreaTab( n, 0, n, MAXROW, nTab, rXFBuff.GetPattern( nXF ) );
}


void ColRowSettings::SetDefaults( UINT16 nWidth, UINT16 nHeight )
{
    nDefWidth = nWidth;
    nDefHeight = nHeight;
}


void ColRowSettings::_SetRowSettings( const UINT16 nRow, const UINT16 nExcelHeight, const UINT16 nGrbit )
{
    pHeight[ nRow ] = ( UINT16 ) ( ( double ) ( nExcelHeight & 0x7FFF ) * pExcRoot->fRowScale );

    INT8    nFlags = ROWFLAG_USED;

    if( nExcelHeight & 0x8000 )
        nFlags |= ROWFLAG_DEFAULT;

    if( nGrbit & EXC_ROW_UNSYNCED )
        nFlags |= ROWFLAG_MAN;

    if( nGrbit &  EXC_ROW_ZEROHEIGHT )
        nFlags |= ROWFLAG_HIDDEN;

    pRowFlags[ nRow ] = nFlags;

    if( nRow > nMaxRow )
        nMaxRow = nRow;
}


void ColRowSettings::ReadSplit( XclImpStream& rIn )
{
    GetExtTabOpt();

    pExtTabOpt->bFrozen = FALSE;

    rIn >> pExtTabOpt->nSplitX >> pExtTabOpt->nSplitY >> pExtTabOpt->nTopSplitRow >> pExtTabOpt->nLeftSplitCol;

    if( (pExcRoot->eHauptDateiTyp == Biff5) || (pExcRoot->eHauptDateiTyp == Biff8) )
        rIn >> pExtTabOpt->nActPane;
    else
        pExtTabOpt->nActPane = rIn.ReaduInt8();

    pExtTabOpt->nTabNum = *pExcRoot->pAktTab;
}


void ColRowSettings::SetVisCorner( UINT16 nCol, UINT16 nRow )
{
    GetExtTabOpt().nLeftCol = nCol;
    GetExtTabOpt().nTopRow = nRow;
}



void ColRowSettings::SetFrozen( const BOOL bFrozen )
{
    GetExtTabOpt().nTabNum = *pExcRoot->pAktTab;
    GetExtTabOpt().bFrozen = bFrozen;
}



void ColRowSettings::SetHorizPagebreak( const UINT16 n )
{
    DBG_ASSERT( n, "+ColRowSettings::SetHorizPagebreak(): 0 ist hier nicht zulaessig!" );

    if( !pHorizPb )
        pHorizPb = new UINT16List;

    pHorizPb->Append( n );
}


void ColRowSettings::SetVertPagebreak( const UINT16 n )
{
    DBG_ASSERT( n, "+ColRowSettings::SetVertPagebreak(): 0 ist hier nicht zulaessig!" );

    if( !pVertPb )
        pVertPb = new UINT16List;

    pVertPb->Append( n );
}




const UINT16        FltColumn::nDefCleared = 0xFFFF;
#ifdef DEBUG
UINT16              FltColumn::nSizeFixedArray = 128;
#else
const UINT16        FltColumn::nSizeFixedArray = 128;
#endif


struct FltColumnItem
{
    FltColumnItem*  pNext;
    FltColumnItem*  pPrev;

    UINT16          nStart;
    UINT16          nEnd;
    UINT16          nXF;

    inline          FltColumnItem( UINT16 nRow, UINT16 nXF );
    inline          FltColumnItem( UINT16 nStartRow, UINT16 nEndRow, UINT16 nXF );
};


inline FltColumnItem::FltColumnItem( UINT16 nR, UINT16 nN )
{
    nStart = nEnd = nR;
    nXF = nN;
#ifdef DEBUG
    pNext = pPrev = NULL;
#endif
}


inline FltColumnItem::FltColumnItem( UINT16 nStartRow, UINT16 nEndRow, UINT16 nN )
{
    nStart = nStartRow;
    nEnd = nEndRow;
    nXF = nN;
#ifdef DEBUG
    pNext = pPrev = NULL;
#endif
}




inline FltColumnItem* FltColumn::First( void )
{
//  return ( FltColumnItem* ) List::First();
    pAct = pFirst;
    return pAct;
}


inline FltColumnItem* FltColumn::Next( void )
{
//  return ( FltColumnItem* ) List::Next();
    if( pAct )
        pAct = pAct->pNext;
    return pAct;
}


inline FltColumnItem* FltColumn::Last( void )
{
//  return ( FltColumnItem* ) List::Last();
    pAct = pLast;
    return pAct;
}


inline FltColumnItem* FltColumn::Prev( void )
{
//  return ( FltColumnItem* ) List::Prev();
    if( pAct )
        pAct = pAct->pPrev;
    return pAct;
}


inline void FltColumn::Append( FltColumnItem* p )
{
//  List::Insert( p, LIST_APPEND );
    if( pLast )
    {
        pLast->pNext = p;
        p->pPrev = pLast;
    }
    else
    {   // first element
        pFirst = p;
        p->pPrev = NULL;
    }

    p->pNext = NULL;
    pLast = p;
}


inline void FltColumn::InsertBefore( FltColumnItem* pA, FltColumnItem* pP )
{
    pP->pNext = pA;

    if( pA->pPrev )
    {
        pP->pPrev = pA->pPrev;

        pA->pPrev->pNext = pP;
    }
    else
    {   // first
        pP->pPrev = NULL;

        pFirst = pP;
    }

    pA->pPrev = pP;
}


void FltColumn::InsertIn( FltColumnItem* pA, FltColumnItem* pI )
{   //
    DBG_ASSERT( pA->nStart <= pI->nStart && pA->nEnd >= pI->nEnd, "*FltColumn::InsertIn(): wrong use" );
    DBG_ASSERT( pA->pPrev, "*FltColumn::InsertIn(): old can't be the first" );

    if( pA->nEnd == pI->nStart )
    {   // at the end of old
        pA->nEnd--;
        if( pA->pNext )
            InsertBefore( pA->pNext, pI );
        else
            // only after, no insert!
            Append( pI );
    }
    else if( pA->nStart == pI->nStart )
    {   // at the begining of old
        pA->nStart++;
        InsertBefore( pA, pI );
    }
    else
    {   // somewhere in the middle of old
        FltColumnItem*      pPre = new FltColumnItem( pA->nStart, pI->nStart - 1, pA->nXF );
                                        // range before new
        InsertBefore( pA, pPre );       // insert in list before old: pPre<->pA
        pA->nStart = pI->nEnd + 1;      // shorten old range to start after inserted

        InsertBefore( pA, pI );         // pPre<->pI<->pA
    }
}


BOOL FltColumn::Merge( FltColumnItem* pA, UINT16 n )
{
    UINT16  nP1 = n + 1;

    if( nP1 < pA->nStart )
        // merge not possible, but insert new item in list before
        InsertBefore( pA, new FltColumnItem( n, pA->nXF ) );
    else if( nP1 == pA->nStart )
        pA->nStart = n;
    else if( n > pA->nEnd )
        return FALSE;       // try the following item

    return TRUE;
}


BOOL FltColumn::Insert( FltColumnItem* pA, UINT16 n, UINT16 nXF )
{
    if( n <= pA->nStart )
        // before or at start of act range
        InsertBefore( pA, new FltColumnItem( n, nXF ) );
    else if( n <= pA->nEnd )
        // in act range
        InsertIn( pA, new FltColumnItem( n, nXF ) );
    else
        return FALSE;

    return TRUE;
}


void FltColumn::_SetXF( UINT16 nRow, UINT16 nXF )
{
    FltColumnItem*  p = Last();

    if( p )
    {
        UINT16      n = p->nEnd + 1;

        if( n == nRow && p->nXF == nXF )
        {   // appends last
            p->nEnd = n;
            return;
        }
        else if( n > nRow )
        {   // must be inserted before
            p = First();
            while( p )
            {
                if( p->nXF == nXF )
                {
                    if( Merge( p, nRow ) )
                        return;
                }
                else
                {
                    if( Insert( p, nRow, nXF ) )
                        return;
                }

                p = Next();
            }
            return;
        }
    }

    Append( new FltColumnItem( nRow, nXF ) );
}


void FltColumn::DeleteList( void )
{
    if( pFirst )
    {
        FltColumnItem*  p = First();
        FltColumnItem*  pDel;

        while( p )
        {
            pDel = p;
            p = Next();
            delete pDel;
        }

        pFirst = pLast = pAct = NULL;
    }
}


FltColumn::FltColumn( RootData* pRD, UINT16 nNewCol ) : ExcRoot( pRD )
{
    DBG_ASSERT( nNewCol <= MAXCOL, "-FltColumn::Ctor: Column > MAXCOL! MIIIIIIIIIIIIIIIIIIST!" );

    pData = new UINT16[ nSizeFixedArray ];
    nCol = nNewCol;
    pFirst = pLast = pAct = NULL;
    Reset();
}


FltColumn::~FltColumn()
{
    delete[] pData;

    DeleteList();
}


void FltColumn::Reset( void )
{
    for( UINT16 nC = 0 ; nC < nSizeFixedArray ; nC++ )
        pData[ nC ] = nDefCleared;

    nLastRow = 0;

    DeleteList();
}


void FltColumn::Apply( const UINT16 nTab ) const
{
    ScDocument&     rD = *pExcRoot->pDoc;
    XF_Buffer&      rXF_Buff = *pExcRoot->pXF_Buffer;

    UINT16          nCount, nLastXF, nAktXF, nLastCount;

    nLastCount = 0;
    nLastXF = pData[ nLastCount ];

    // XFs from array
    for( nCount = 1 ; nCount < nSizeFixedArray ; nCount++ )
    {
        nAktXF = pData[ nCount ];
        if( nAktXF != nLastXF )
        {
            if( nLastXF != nDefCleared )
                rD.ApplyPatternAreaTab( nCol, nLastCount, nCol, nCount - 1, nTab, rXF_Buff.GetPattern( nLastXF ) );
            nLastCount = nCount;
            nLastXF = nAktXF;
        }
    }
    // ...und den Rest applyen
    if( nLastXF != nDefCleared )
        rD.ApplyPatternAreaTab( nCol, nLastCount, nCol, nCount - 1, nTab, rXF_Buff.GetPattern( nLastXF ) );

    // Xfs from list
    FltColumnItem*  p = ( ( FltColumn* ) this )->First();
    while( p )
    {
        rD.ApplyPatternAreaTab( nCol, p->nStart, nCol, p->nEnd, nTab, rXF_Buff.GetPattern( p->nXF ) );
        p = ( ( FltColumn* ) this )->Next();
    }
}




FltTabelle::FltTabelle( RootData* pRD ) : ExcRoot( pRD )
{
    nSize = MAXCOL + 1;
    nLastCol = 0;
    pData = new FltColumn *[ nSize ];

    pData[ 0 ] = new FltColumn( pExcRoot, 0 );  // Column 0 gibt's per Default

    for( UINT16 nC = 1 ; nC < nSize ; nC++ )
        pData[ nC ] = NULL;

    pFirst = pLast = pCurr = NULL;
}


FltTabelle::~FltTabelle()
{
    FltColumn**     pDel = pData;
    for( UINT16 nC = 0 ; nC <= nLastCol ; nC++, pDel++ )
        if( *pDel ) delete *pDel;

    delete [] pData;

    DeleteList();
}


void FltTabelle::Append( FltCellMerge* pNew )
{
    if( pLast )
        pLast->pNext = pNew;
    else
        pFirst = pNew;
    pLast = pNew;
}


void FltTabelle::DeleteList()
{
    FltCellMerge* pDel;
    while( pFirst )
    {
        pDel = pFirst;
        pFirst = pFirst->pNext;
        delete pDel;
    }
    pFirst = pLast = pCurr = NULL;
}


void FltTabelle::SetXF( UINT16 nCol, UINT16 nRow, UINT16 nNewXF, const BOOL bBlank )
{
    DBG_ASSERT( nCol <= MAXCOL, "-FltTabelle::SetXF(): Col > MAXCOL!");
    DBG_ASSERT( nRow <= MAXROW, "-FltTabelle::SetXF(): Row > MAXROW!");

    if( !pData[ nCol ] )
    {
        pData[ nCol ] = new FltColumn( pExcRoot, nCol );
        if( nCol > nLastCol ) nLastCol = nCol;
    }

    const XF_Data*  pXFData = pExcRoot->pXF_Buffer->GetXF( nNewXF );

#ifdef _DEBUG
    if( !pXFData )
    {
        DBG_WARNING( "+FltTabelle::SetXF(): Nicht so schnell mein Freund?!" );
    }
#endif

    if( pXFData && pXFData->HorizAlign() == EHA_CentAcrSel )
    {
        if( bBlank && pLast && (pLast->nLastCol + 1 == nCol) )
            pLast->nLastCol = nCol;
        else
            AppendMerge( nCol, nRow );
    }

    pData[ nCol ]->SetXF( nRow, nNewXF );
}


void FltTabelle::Reset( void )
{
    for( UINT16 nC = 0 ; nC <= nLastCol ; nC++ )
        if( pData[ nC ] ) pData[ nC ]->Reset();

    DeleteList();
}


void FltTabelle::SetBorderLine( const FltCellMerge& rMerge, USHORT nTab, USHORT nLine )
{
    USHORT nFromCol = (nLine == BOX_LINE_RIGHT) ? rMerge.nLastCol : rMerge.nFirstCol;
    USHORT nFromRow = (nLine == BOX_LINE_BOTTOM) ? rMerge.nLastRow : rMerge.nFirstRow;

    const SvxBoxItem* pFromItem = (const SvxBoxItem*)
        pExcRoot->pDoc->GetAttr( nFromCol, nFromRow, nTab, ATTR_BORDER );
    const SvxBoxItem* pToItem = (const SvxBoxItem*)
        pExcRoot->pDoc->GetAttr( rMerge.nFirstCol, rMerge.nFirstRow, nTab, ATTR_BORDER );

    SvxBoxItem aNewItem( *pToItem );
    aNewItem.SetLine( pFromItem->GetLine( nLine ), nLine );
    pExcRoot->pDoc->ApplyAttr( rMerge.nFirstCol, rMerge.nFirstRow, nTab, aNewItem );
}


void FltTabelle::Apply( const UINT16 nTab )
{
    for( UINT16 nC = 0 ; nC <= nLastCol ; nC++ )
        if( pData[ nC ] ) pData[ nC ]->Apply( nTab );

    for( FltCellMerge* pMerge = First(); pMerge; pMerge = Next() )
    {
        if( pMerge->nFirstCol != pMerge->nLastCol )     // get right border
            SetBorderLine( *pMerge, nTab, BOX_LINE_RIGHT );
        if( pMerge->nFirstRow != pMerge->nLastRow )     // get lower border
            SetBorderLine( *pMerge, nTab, BOX_LINE_BOTTOM );
        pExcRoot->pDoc->DoMerge( nTab, pMerge->nFirstCol, pMerge->nFirstRow,
            pMerge->nLastCol, pMerge->nLastRow );
    }
}




void ScExtTabOptions::SetSelection( const ScRange& r )
{
    if( r.aStart.Row() <= MAXROW && r.aEnd.Row() <= MAXROW )
    {
        bValidSel = TRUE;
        aLastSel = r;
    }
    else
        bValidSel = FALSE;
}


void ScExtTabOptions::SetDimension( const ScRange& r )
{
    if( r.aStart.Row() <= MAXROW && r.aEnd.Row() <= MAXROW )
    {
        bValidDim = TRUE;
        aDim = r;
    }
    else
        bValidDim = FALSE;
}




CodenameList::CodenameList( const CodenameList& r )
{
    UINT32          n = 0;
    const String*   p = ( const String*) r.GetObject( n );

    while( p )
    {
        Append( *p );
        n++;
        p = ( const String*) r.GetObject( n );
    }
}


CodenameList::~CodenameList()
{
    String*         p = ( String* ) List::First();

    while( p )
    {
        delete p;
        p = ( String* ) List::Next();
    }
}




ScExtDocOptions::ScExtDocOptions( void )
{
    pGridCol = NULL;
    nActTab = nSelTabs = nCurCol = nCurRow = 0;
    nLinkCnt = 0;       // -> 'Root'-Dokument
    nZoom = 100;

    ppExtTabOpts = new ScExtTabOptions *[ MAXTAB + 1 ];
    for( UINT16 nCnt = 0 ; nCnt <= MAXTAB ; nCnt++ )
        ppExtTabOpts[ nCnt ] = NULL;

    pCodenameWB = NULL;
    pCodenames = NULL;

    fColScale = 0.0;
}


ScExtDocOptions::~ScExtDocOptions()
{
    if( pGridCol )
        delete pGridCol;

    for( UINT16 nCnt = 0 ; nCnt <= MAXTAB ; nCnt++ )
    {
        if( ppExtTabOpts[ nCnt ] )
            delete ppExtTabOpts[ nCnt ];
    }

    delete[] ppExtTabOpts;

    if( pCodenameWB )
        delete pCodenameWB;
    if( pCodenames )
        delete pCodenames;
}


ScExtDocOptions& ScExtDocOptions::operator =( const ScExtDocOptions& rCpy )
{
    nLinkCnt = rCpy.nLinkCnt;
    nActTab = rCpy.nActTab;
    nSelTabs = rCpy.nSelTabs;
    nCurCol = rCpy.nCurCol;
    nCurRow = rCpy.nCurRow;

    if( pGridCol )
    {
        if( rCpy.pGridCol )
            *pGridCol = *rCpy.pGridCol;
        else
        {
            delete pGridCol;
            pGridCol = NULL;
        }
    }
    else if( rCpy.pGridCol )
        pGridCol = new Color( *rCpy.pGridCol );

    nZoom = rCpy.nZoom;
    nCurCol = rCpy.nCurCol;
    nCurRow = rCpy.nCurRow;

    for( UINT16 nCnt = 0 ; nCnt <= MAXTAB ; nCnt++ )
    {
        const ScExtTabOptions*  pT = rCpy.ppExtTabOpts[ nCnt ];
        if( pT )
        {
            if( ppExtTabOpts[ nCnt ] )
                *ppExtTabOpts[ nCnt ] = *pT;
            else
                ppExtTabOpts[ nCnt ] = new ScExtTabOptions( *pT );
        }
        else
        {
            if( ppExtTabOpts[ nCnt ] )
                delete ppExtTabOpts[ nCnt ];

            ppExtTabOpts[ nCnt ] = NULL;
        }
    }

    if( rCpy.pCodenameWB )
        pCodenameWB = new String( *rCpy.pCodenameWB );
    if( rCpy.pCodenames )
        pCodenames = new CodenameList( *rCpy.pCodenames );

    fColScale = rCpy.fColScale;

    return *this;
}


void ScExtDocOptions::SetExtTabOptions( UINT16 nTabNum, ScExtTabOptions* pTabOpt )
{
    if( ppExtTabOpts[ nTabNum ] )
        delete ppExtTabOpts[ nTabNum ];
    ppExtTabOpts[ nTabNum ] = pTabOpt;
}


void ScExtDocOptions::SetGridCol( const Color& rColor )
{
    if( pGridCol )
        pGridCol->SetColor( rColor.GetColor() );
    else
        pGridCol = new Color( rColor );
}


void ScExtDocOptions::SetActTab( UINT16 nTab )
{
    nActTab = ( nTab <= MAXTAB )? nTab : MAXTAB;
}


void ScExtDocOptions::SetCursor( UINT16 nCol, UINT16 nRow )
{
    nCurCol = ( nCol <= MAXCOL )? nCol : MAXCOL;
    nCurRow = ( nRow <= MAXROW )? nRow : MAXROW;
}


void ScExtDocOptions::SetZoom( UINT16 nZaehler, UINT16 nNenner )
{
    nZoom = 100 * nZaehler / nNenner;
    if( nZoom < 20 )
        nZoom = 20;
    else if( nZoom > 400 )
        nZoom = 400;
}


void ScExtDocOptions::Add( const ColRowSettings& rCRS )
{
    const UINT16 nTab = *rCRS.pExcRoot->pAktTab;

    if( nTab <= MAXTAB )
    {
        if( rCRS.pExtTabOpt )
        {
            if( ppExtTabOpts[ nTab ] )
                *ppExtTabOpts[ nTab ] = *rCRS.pExtTabOpt;
            else
                ppExtTabOpts[ nTab ] = new ScExtTabOptions( *rCRS.pExtTabOpt );
        }
    }
    else
    {
        DBG_WARNING( "ScExtDocOptions::Add(): Aetsch... falsch bedient!" );
    }
}


void ScExtDocOptions::SetCodename( const String& r )
{
    if( pCodenameWB )
        *pCodenameWB = r;
    else
        pCodenameWB = new String( r );
}


void ScExtDocOptions::AddCodename( const String& r )
{
    if( !pCodenames )
        pCodenames = new CodenameList;

    pCodenames->Append( r );
}



