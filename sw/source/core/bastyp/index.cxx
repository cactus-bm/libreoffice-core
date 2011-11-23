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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <stdlib.h>             // fuer qsort
#include <tools/solar.h>

#include "index.hxx"

#ifdef DBG_UTIL
int SwIndex::s_nSerial = 0;
#endif


TYPEINIT0(SwIndexReg);  // rtti


SwIndex::SwIndex(SwIndexReg *const pArr, xub_StrLen const nIdx)
    : nIndex( nIdx ), pArray( pArr ), pNext( 0 ), pPrev( 0 )
{
    if( !pArray )
    {
        pArray = &EmptyIndexArray::get();
        nIndex = 0;     // steht immer auf 0 !!!
    }

    if( !pArray->pFirst || !pArray->pLast )         // 1. Index ??
        pArray->pFirst = pArray->pLast = this;
    else if( nIdx > ((pArray->pLast->nIndex - pArray->pFirst->nIndex) / 2) )
        ChgValue( *pArray->pLast, nIdx );
    else
        ChgValue( *pArray->pFirst, nIdx );

#ifdef DBG_UTIL
    m_nSerial = ++s_nSerial;
#endif
}


SwIndex::SwIndex( const SwIndex& rIdx, short nIdx )
    : pArray( rIdx.pArray ), pNext( 0 ), pPrev( 0 )
{
    ChgValue( rIdx, rIdx.nIndex + nIdx );

#ifdef DBG_UTIL
    m_nSerial = ++s_nSerial;
#endif
}


SwIndex::SwIndex( const SwIndex& rIdx )
    : nIndex( rIdx.nIndex ), pArray( rIdx.pArray ), pNext( 0 ), pPrev( 0 )
{
    ChgValue( rIdx, rIdx.nIndex );
#ifdef DBG_UTIL
    m_nSerial = ++s_nSerial;
#endif
}


SwIndex& SwIndex::ChgValue( const SwIndex& rIdx, xub_StrLen nNewValue )
{
    SwIndex* pFnd = (SwIndex*)&rIdx;
    if( rIdx.nIndex > nNewValue )               // nach vorne versuchen
    {
        SwIndex* pPrv;
        while( 0 != ( pPrv = pFnd->pPrev ) && pPrv->nIndex > nNewValue )
            pFnd = pPrv;

        if( pFnd != this )
        {
            // an alter Position ausketten
            // erstmal an alter Position ausketten
            if( pPrev )
                pPrev->pNext = pNext;
            else if( pArray->pFirst == this )
                pArray->pFirst = pNext;

            if( pNext )
                pNext->pPrev = pPrev;
            else if( pArray->pLast == this )
                pArray->pLast = pPrev;

            pNext = pFnd;
            pPrev = pFnd->pPrev;
            if( pPrev )
                pPrev->pNext = this;
            else
                pArray->pFirst = this;
            pFnd->pPrev = this;
        }
    }
    else if( rIdx.nIndex < nNewValue )
    {
        SwIndex* pNxt;
        while( 0 != ( pNxt = pFnd->pNext ) && pNxt->nIndex < nNewValue )
            pFnd = pNxt;

        if( pFnd != this )
        {
            // erstmal an alter Position ausketten
            if( pPrev )
                pPrev->pNext = pNext;
            else if( pArray->pFirst == this )
                pArray->pFirst = pNext;

            if( pNext )
                pNext->pPrev = pPrev;
            else if( pArray->pLast == this )
                pArray->pLast = pPrev;

            pPrev = pFnd;
            pNext = pFnd->pNext;
            if( pNext )
                pNext->pPrev = this;
            else
                pArray->pLast = this;
            pFnd->pNext = this;
        }
    }
    else if( pFnd != this )
    {
        // erstmal an alter Position ausketten
        if( pPrev )
            pPrev->pNext = pNext;
        else if( pArray->pFirst == this )
            pArray->pFirst = pNext;

        if( pNext )
            pNext->pPrev = pPrev;
        else if( pArray->pLast == this )
            pArray->pLast = pPrev;

        pPrev = (SwIndex*)&rIdx;
        pNext = rIdx.pNext;
        pPrev->pNext = this;

        if( !pNext )            // im IndexArray als letzes
            pArray->pLast = this;
        else
            pNext->pPrev = this;
    }
    pArray = rIdx.pArray;

    if( pArray->pFirst == pNext )
        pArray->pFirst = this;
    if( pArray->pLast == pPrev )
        pArray->pLast = this;

    nIndex = nNewValue;

    return *this; }


void SwIndex::Remove()
{
    if( !pPrev )
        pArray->pFirst = pNext;
    else
        pPrev->pNext = pNext;

    if( !pNext )
        pArray->pLast = pPrev;
    else
        pNext->pPrev = pPrev;
}

/*************************************************************************
|*    SwIndex & SwIndex::operator=( const SwIndex & aSwIndex )
*************************************************************************/


SwIndex& SwIndex::operator=( const SwIndex& rIdx )
{
    int bEqual;
    if( rIdx.pArray != pArray )         // im alten abmelden !!
    {
        Remove();
        pArray = rIdx.pArray;
        pNext = pPrev = 0;
        bEqual = sal_False;
    }
    else
        bEqual = rIdx.nIndex == nIndex;

    if( !bEqual )
        ChgValue( rIdx, rIdx.nIndex );
    return *this;
}

/*************************************************************************
|*    SwIndex &SwIndex::Assign
*************************************************************************/


SwIndex& SwIndex::Assign( SwIndexReg* pArr, xub_StrLen nIdx )
{
    if( !pArr )
    {
        pArr = &EmptyIndexArray::get();
        nIdx = 0;       // steht immer auf 0 !!!
    }

    if( pArr != pArray )            // im alten abmelden !!
    {
        Remove();
        pArray = pArr;
        pNext = pPrev = 0;
        if( !pArr->pFirst )         // 1. Index ??
        {
            pArr->pFirst = pArr->pLast = this;
            nIndex = nIdx;
        }
        else if( pArr->pLast && (nIdx > ((pArr->pLast->nIndex - pArr->pFirst->nIndex) / 2)) )
            ChgValue( *pArr->pLast, nIdx );
        else
            ChgValue( *pArr->pFirst, nIdx );
    }
    else if( nIndex != nIdx )
        ChgValue( *this, nIdx );
    return *this;
}


SwIndexReg::SwIndexReg()
    : pFirst( 0 ), pLast( 0 )
{
}



SwIndexReg::~SwIndexReg()
{
    OSL_ENSURE( !pFirst || !pLast, "There are still indices registered" );
}



void SwIndexReg::Update( SwIndex const & rIdx, const xub_StrLen nDiff,
    const bool bNeg, const bool /* argument is only used in derived class*/ )
{
    SwIndex* pStt = const_cast<SwIndex*>(&rIdx);
    xub_StrLen nNewVal = rIdx.nIndex;
    if( bNeg )
    {
        xub_StrLen nLast = rIdx.GetIndex() + nDiff;
        while( pStt && pStt->nIndex == nNewVal )
        {
            pStt->nIndex = nNewVal;
            pStt = pStt->pPrev;
        }
        pStt = rIdx.pNext;
        while( pStt && pStt->nIndex >= nNewVal &&
                pStt->nIndex <= nLast )
        {
            pStt->nIndex = nNewVal;
            pStt = pStt->pNext;
        }
        while( pStt )
        {
            pStt->nIndex = pStt->nIndex - nDiff;
            pStt = pStt->pNext;
        }
    }
    else
    {
        while( pStt && pStt->nIndex == nNewVal )
        {
            pStt->nIndex = pStt->nIndex + nDiff;
            pStt = pStt->pPrev;
        }
        pStt = rIdx.pNext;
        while( pStt )
        {
            pStt->nIndex = pStt->nIndex + nDiff;
            pStt = pStt->pNext;
        }
    }
}

#ifdef DBG_UTIL

/*************************************************************************
|*    SwIndex::operator++()
*************************************************************************/
xub_StrLen SwIndex::operator++(int)
{
    OSL_ASSERT( nIndex < INVALID_INDEX );

    xub_StrLen nOldIndex = nIndex;
    ChgValue( *this, nIndex+1 );
    return nOldIndex;
}


xub_StrLen SwIndex::operator++()
{
    OSL_ASSERT( nIndex < INVALID_INDEX );

    ChgValue( *this, nIndex+1 );
    return nIndex;
}

/*************************************************************************
|*    SwIndex::operator--()
*************************************************************************/


xub_StrLen SwIndex::operator--(int)
{
    OSL_ASSERT( nIndex );

    xub_StrLen nOldIndex = nIndex;
    ChgValue( *this, nIndex-1 );
    return nOldIndex;
}


xub_StrLen SwIndex::operator--()
{
    OSL_ASSERT( nIndex );
    return ChgValue( *this, nIndex-1 ).nIndex;
}

/*************************************************************************
|*    SwIndex::operator+=( xub_StrLen )
*************************************************************************/

xub_StrLen SwIndex::operator+=( xub_StrLen nWert )
{
    OSL_ASSERT( nIndex < INVALID_INDEX - nWert );
    return ChgValue( *this, nIndex + nWert ).nIndex;
}

/*************************************************************************
|*    SwIndex::operator-=( xub_StrLen )
*************************************************************************/

xub_StrLen SwIndex::operator-=( xub_StrLen nWert )
{
    OSL_ASSERT( nIndex >= nWert );
    return ChgValue( *this, nIndex - nWert ).nIndex;
}

/*************************************************************************
|*    SwIndex::operator+=( const SwIndex & )
*************************************************************************/

xub_StrLen SwIndex::operator+=( const SwIndex & rIndex )
{
    OSL_ASSERT( nIndex < INVALID_INDEX - rIndex.nIndex );
    return ChgValue( *this, nIndex + rIndex.nIndex ).nIndex;
}

/*************************************************************************
|*    SwIndex::operator-=( const SwIndex & )
*************************************************************************/

xub_StrLen SwIndex::operator-=( const SwIndex & rIndex )
{
    OSL_ASSERT( nIndex >= rIndex.nIndex );
    return ChgValue( *this, nIndex - rIndex.nIndex ).nIndex;
}

/*************************************************************************
|*    SwIndex::operator<( const SwIndex & )
*************************************************************************/

sal_Bool SwIndex::operator<( const SwIndex & rIndex ) const
{
    OSL_ENSURE( pArray == rIndex.pArray, "Attempt to compare indices into different arrays.");
    return nIndex < rIndex.nIndex;
}

/*************************************************************************
|*    SwIndex::operator<=( const SwIndex & )
*************************************************************************/

sal_Bool SwIndex::operator<=( const SwIndex & rIndex ) const
{
    OSL_ENSURE( pArray == rIndex.pArray, "Attempt to compare indices into different arrays.");
    return nIndex <= rIndex.nIndex;
}

/*************************************************************************
|*    SwIndex::operator>( const SwIndex & )
*************************************************************************/

sal_Bool SwIndex::operator>( const SwIndex & rIndex ) const
{
    OSL_ENSURE( pArray == rIndex.pArray, "Attempt to compare indices into different arrays.");
    return nIndex > rIndex.nIndex;
}

/*************************************************************************
|*    SwIndex::operator>=( const SwIndex & )
*************************************************************************/

sal_Bool SwIndex::operator>=( const SwIndex & rIndex ) const
{
    OSL_ENSURE( pArray == rIndex.pArray, "Attempt to compare indices into different arrays.");
    return nIndex >= rIndex.nIndex;
}

/*************************************************************************
|*    SwIndex & SwIndex::operator=( xub_StrLen )
*************************************************************************/

SwIndex& SwIndex::operator=( xub_StrLen nWert )
{
    // Werte kopieren und im neuen Array anmelden
    if( nIndex != nWert )
        ChgValue( *this, nWert );

    return *this;
}

#endif // ifndef PRODUCT

void SwIndexReg::MoveTo( SwIndexReg& rArr )
{
    if( this != &rArr && pFirst )
    {
        SwIndex* pIdx = (SwIndex*)pFirst, *pNext;
        while( pIdx )
        {
            pNext = pIdx->pNext;
            pIdx->Assign( &rArr, pIdx->GetIndex() );
            pIdx = pNext;
        }
        pFirst = 0, pLast = 0;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
