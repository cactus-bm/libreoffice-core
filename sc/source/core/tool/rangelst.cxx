/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rangelst.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: ihi $ $Date: 2006-10-18 12:23:03 $
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


//------------------------------------------------------------------------

#define SC_RANGELST_CXX         //fuer ICC

#include <tools/debug.hxx>
#include <stdlib.h>             // qsort
#include <unotools/collatorwrapper.hxx>

#include "rangelst.hxx"
#include "document.hxx"
#include "refupdat.hxx"
#include "rechead.hxx"


// === ScRangeList ====================================================

ScRangeList::~ScRangeList()
{
    for ( ScRangePtr pR = First(); pR; pR = Next() )
        delete pR;
}

void ScRangeList::RemoveAll()
{
    for ( ScRangePtr pR = First(); pR; pR = Next() )
        delete pR;
    Clear();
}

static void defaultDelimiter( char& cDelimiter, ScAddress::Convention eConv)
{
    if( cDelimiter == 0)
    {
        switch( eConv )
        {
        default :
        case ScAddress::CONV_OOO :
            cDelimiter = ';';
            break;

        case ScAddress::CONV_XL_A1 :
        case ScAddress::CONV_XL_R1C1 :
            cDelimiter = ',';
            break;
        }
    }
}

USHORT ScRangeList::Parse( const String& rStr, ScDocument* pDoc, USHORT nMask,
                           ScAddress::Convention eConv,
                           char cDelimiter )
{
    if ( rStr.Len() )
    {
        defaultDelimiter( cDelimiter, eConv);

        nMask |= SCA_VALID;             // falls das jemand vergessen sollte
        USHORT nResult = (USHORT)~0;    // alle Bits setzen
        ScRange aRange;
        String aOne;
        SCTAB nTab = 0;
        if ( pDoc )
        {
            //! erste markierte Tabelle gibts nicht mehr am Dokument
            //! -> uebergeben? oder spaeter an den Ranges setzen
        }
        else
            nTab = 0;
        USHORT nTCount = rStr.GetTokenCount( cDelimiter );
        for ( USHORT i=0; i<nTCount; i++ )
        {
            aOne = rStr.GetToken( i, cDelimiter );
            // FIXME : broken for Lotus
            if ( aOne.Search( ':' ) == STRING_NOTFOUND )
            {   // Range muss es sein
                String aStrTmp( aOne );
                aOne += ':';
                aOne += aStrTmp;
            }
            aRange.aStart.SetTab( nTab );   // Default Tab wenn nicht angegeben
            USHORT nRes = aRange.Parse( aOne, pDoc, eConv );
            if ( (nRes & nMask) == nMask )
                Append( aRange );
            nResult &= nRes;        // alle gemeinsamen Bits bleiben erhalten
        }
        return nResult;             // SCA_VALID gesetzt wenn alle ok
    }
    else
        return 0;
}


void ScRangeList::Format( String& rStr, USHORT nFlags, ScDocument* pDoc,
                          ScAddress::Convention eConv,
                          char cDelimiter ) const
{
    rStr.Erase();

    defaultDelimiter( cDelimiter, eConv);

    ULONG nCnt = Count();
    for ( ULONG nIdx = 0; nIdx < nCnt; nIdx++ )
    {
        String aStr;
        GetObject( nIdx )->Format( aStr, nFlags, pDoc, eConv );
        if ( nIdx )
            rStr += cDelimiter;
        rStr += aStr;
    }
}


void ScRangeList::Join( const ScRange& r, BOOL bIsInList )
{
    if ( !Count() )
    {
        Append( r );
        return ;
    }
    SCCOL nCol1 = r.aStart.Col();
    SCROW nRow1 = r.aStart.Row();
    SCTAB nTab1 = r.aStart.Tab();
    SCCOL nCol2 = r.aEnd.Col();
    SCROW nRow2 = r.aEnd.Row();
    SCTAB nTab2 = r.aEnd.Tab();
    ScRangePtr pOver = (ScRangePtr) &r;     // fies aber wahr wenn bInList
    ULONG nOldPos;
    if ( bIsInList )
    {   // merken um ggbf. zu loeschen bzw. wiederherzustellen
        nOldPos = GetPos( pOver );
    }
    BOOL bJoinedInput = FALSE;
    for ( ScRangePtr p = First(); p && pOver; p = Next() )
    {
        if ( p == pOver )
            continue;           // derselbe, weiter mit dem naechsten
        BOOL bJoined = FALSE;
        if ( p->In( r ) )
        {   // Range r in Range p enthalten oder identisch
            if ( bIsInList )
                bJoined = TRUE;     // weg mit Range r
            else
            {   // das war's dann
                bJoinedInput = TRUE;    // nicht anhaengen
                break;  // for
            }
        }
        else if ( r.In( *p ) )
        {   // Range p in Range r enthalten, r zum neuen Range machen
            *p = r;
            bJoined = TRUE;
        }
        if ( !bJoined && p->aStart.Tab() == nTab1 && p->aEnd.Tab() == nTab2 )
        {   // 2D
            if ( p->aStart.Col() == nCol1 && p->aEnd.Col() == nCol2 )
            {
                if ( p->aStart.Row() == nRow2+1 )
                {   // oben
                    p->aStart.SetRow( nRow1 );
                    bJoined = TRUE;
                }
                else if ( p->aEnd.Row() == nRow1-1 )
                {   // unten
                    p->aEnd.SetRow( nRow2 );
                    bJoined = TRUE;
                }
            }
            else if ( p->aStart.Row() == nRow1 && p->aEnd.Row() == nRow2 )
            {
                if ( p->aStart.Col() == nCol2+1 )
                {   // links
                    p->aStart.SetCol( nCol1 );
                    bJoined = TRUE;
                }
                else if ( p->aEnd.Col() == nCol1-1 )
                {   // rechts
                    p->aEnd.SetCol( nCol2 );
                    bJoined = TRUE;
                }
            }
        }
        if ( bJoined )
        {
            if ( bIsInList )
            {   // innerhalb der Liste Range loeschen
                Remove( nOldPos );
                delete pOver;
                pOver = NULL;
                if ( nOldPos )
                    nOldPos--;          // Seek richtig aufsetzen
            }
            bJoinedInput = TRUE;
            Join( *p, TRUE );           // rekursiv!
        }
    }
    if ( bIsInList )
        Seek( nOldPos );
    else if ( !bJoinedInput )
        Append( r );
}


BOOL ScRangeList::operator==( const ScRangeList& r ) const
{
    if ( this == &r )
        return TRUE;                // identische Referenz
    if ( Count() != r.Count() )
        return FALSE;
    ULONG nCnt = Count();
    for ( ULONG nIdx = 0; nIdx < nCnt; nIdx++ )
    {
        if ( *GetObject( nIdx ) != *r.GetObject( nIdx ) )
            return FALSE;           // auch andere Reihenfolge ist ungleich
    }
    return TRUE;
}


BOOL ScRangeList::Store( SvStream& rStream ) const
{
    BOOL bOk = TRUE;
    ULONG nCount = Count();
    ULONG nBytes = sizeof(UINT32) + nCount * sizeof(ScRange);
    ScWriteHeader aHdr( rStream, nBytes );
#if SC_ROWLIMIT_STREAM_ACCESS
#error address types changed!
    rStream << (UINT32) nCount;
    for ( ULONG j = 0; j < nCount && bOk; j++ )
    {
        rStream << *GetObject( j );
        if( rStream.GetError() != SVSTREAM_OK )
            bOk = FALSE;
    }
    return bOk;
#else
    return FALSE;
#endif // SC_ROWLIMIT_STREAM_ACCESS
}


BOOL ScRangeList::Load( SvStream& rStream, USHORT nVer )
{
    BOOL bOk = TRUE;
    ScReadHeader aHdr( rStream );
#if SC_ROWLIMIT_STREAM_ACCESS
#error address types changed!
    ScRange aRange;
    UINT32 n;
    rStream >> n;
    ULONG nCount = n;
    for ( ULONG j = 0; j < nCount && bOk; j++ )
    {
        rStream >> aRange;
        Append( aRange );
        if( rStream.GetError() != SVSTREAM_OK )
            bOk = FALSE;
    }
    return bOk;
#else
    return FALSE;
#endif // SC_ROWLIMIT_STREAM_ACCESS
}


BOOL ScRangeList::UpdateReference( UpdateRefMode eUpdateRefMode,
                                    ScDocument* pDoc, const ScRange& rWhere,
                                    SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    BOOL bChanged = FALSE;
    if ( Count() )
    {
        SCCOL nCol1;
        SCROW nRow1;
        SCTAB nTab1;
        SCCOL nCol2;
        SCROW nRow2;
        SCTAB nTab2;
        rWhere.GetVars( nCol1, nRow1, nTab1, nCol2, nRow2, nTab2 );
        for ( ScRange* pR = First(); pR; pR = Next() )
        {
            SCCOL theCol1;
            SCROW theRow1;
            SCTAB theTab1;
            SCCOL theCol2;
            SCROW theRow2;
            SCTAB theTab2;
            pR->GetVars( theCol1, theRow1, theTab1, theCol2, theRow2, theTab2 );
            if ( ScRefUpdate::Update( pDoc, eUpdateRefMode,
                    nCol1, nRow1, nTab1, nCol2, nRow2, nTab2,
                    nDx, nDy, nDz,
                    theCol1, theRow1, theTab1, theCol2, theRow2, theTab2 )
                    != UR_NOTHING )
            {
                bChanged = TRUE;
                pR->aStart.Set( theCol1, theRow1, theTab1 );
                pR->aEnd.Set( theCol2, theRow2, theTab2 );
            }
        }
    }
    return bChanged;
}


ScRange* ScRangeList::Find( const ScAddress& rAdr ) const
{
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
    {
        ScRange* pR = GetObject( j );
        if ( pR->In( rAdr ) )
            return pR;
    }
    return NULL;
}


ScRange* ScRangeList::Find( const ScRange& rRange ) const
{
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
    {
        ScRange* pR = GetObject( j );
        if ( *pR == rRange )
            return pR;
    }
    return NULL;
}


ScRangeList::ScRangeList( const ScRangeList& rList )
{
    ULONG nCount = rList.Count();
    for ( ULONG j = 0; j < nCount; j++ )
        Append( *rList.GetObject( j ) );
}


ScRangeList* ScRangeList::Clone() const
{
    return new ScRangeList( *this );
}


ScRangeList& ScRangeList::operator=(const ScRangeList& rList)
{
    RemoveAll();

    ULONG nCount = rList.Count();
    for ( ULONG j = 0; j < nCount; j++ )
        Append( *rList.GetObject( j ) );

    return *this;
}


BOOL ScRangeList::Intersects( const ScRange& rRange ) const
{
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
        if ( GetObject(j)->Intersects( rRange ) )
            return TRUE;

    return FALSE;
}


BOOL ScRangeList::In( const ScRange& rRange ) const
{
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
        if ( GetObject(j)->In( rRange ) )
            return TRUE;

    return FALSE;
}


ULONG ScRangeList::GetCellCount() const
{
    ULONG nCellCount = 0;
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
    {
        ScRange* pR = GetObject( j );
        nCellCount += ULONG(pR->aEnd.Col() - pR->aStart.Col() + 1)
            * ULONG(pR->aEnd.Row() - pR->aStart.Row() + 1)
            * ULONG(pR->aEnd.Tab() - pR->aStart.Tab() + 1);
    }
    return nCellCount;
}


// === ScRangePairList ====================================================

ScRangePairList::~ScRangePairList()
{
    for ( ScRangePair* pR = First(); pR; pR = Next() )
        delete pR;
}


void ScRangePairList::Join( const ScRangePair& r, BOOL bIsInList )
{
    if ( !Count() )
    {
        Append( r );
        return ;
    }
    const ScRange& r1 = r.GetRange(0);
    const ScRange& r2 = r.GetRange(1);
    SCCOL nCol1 = r1.aStart.Col();
    SCROW nRow1 = r1.aStart.Row();
    SCTAB nTab1 = r1.aStart.Tab();
    SCCOL nCol2 = r1.aEnd.Col();
    SCROW nRow2 = r1.aEnd.Row();
    SCTAB nTab2 = r1.aEnd.Tab();
    ScRangePair* pOver = (ScRangePair*) &r;     // fies aber wahr wenn bInList
    ULONG nOldPos;
    if ( bIsInList )
    {   // merken um ggbf. zu loeschen bzw. wiederherzustellen
        nOldPos = GetPos( pOver );
    }
    BOOL bJoinedInput = FALSE;
    for ( ScRangePair* p = First(); p && pOver; p = Next() )
    {
        if ( p == pOver )
            continue;           // derselbe, weiter mit dem naechsten
        BOOL bJoined = FALSE;
        ScRange& rp1 = p->GetRange(0);
        ScRange& rp2 = p->GetRange(1);
        if ( rp2 == r2 )
        {   // nur wenn Range2 gleich ist
            if ( rp1.In( r1 ) )
            {   // RangePair r in RangePair p enthalten oder identisch
                if ( bIsInList )
                    bJoined = TRUE;     // weg mit RangePair r
                else
                {   // das war's dann
                    bJoinedInput = TRUE;    // nicht anhaengen
                    break;  // for
                }
            }
            else if ( r1.In( rp1 ) )
            {   // RangePair p in RangePair r enthalten, r zum neuen RangePair machen
                *p = r;
                bJoined = TRUE;
            }
        }
        if ( !bJoined && rp1.aStart.Tab() == nTab1 && rp1.aEnd.Tab() == nTab2
          && rp2.aStart.Tab() == r2.aStart.Tab()
          && rp2.aEnd.Tab() == r2.aEnd.Tab() )
        {   // 2D, Range2 muss genauso nebeneinander liegen wie Range1
            if ( rp1.aStart.Col() == nCol1 && rp1.aEnd.Col() == nCol2
              && rp2.aStart.Col() == r2.aStart.Col()
              && rp2.aEnd.Col() == r2.aEnd.Col() )
            {
                if ( rp1.aStart.Row() == nRow2+1
                  && rp2.aStart.Row() == r2.aEnd.Row()+1 )
                {   // oben
                    rp1.aStart.SetRow( nRow1 );
                    rp2.aStart.SetRow( r2.aStart.Row() );
                    bJoined = TRUE;
                }
                else if ( rp1.aEnd.Row() == nRow1-1
                  && rp2.aEnd.Row() == r2.aStart.Row()-1 )
                {   // unten
                    rp1.aEnd.SetRow( nRow2 );
                    rp2.aEnd.SetRow( r2.aEnd.Row() );
                    bJoined = TRUE;
                }
            }
            else if ( rp1.aStart.Row() == nRow1 && rp1.aEnd.Row() == nRow2
              && rp2.aStart.Row() == r2.aStart.Row()
              && rp2.aEnd.Row() == r2.aEnd.Row() )
            {
                if ( rp1.aStart.Col() == nCol2+1
                  && rp2.aStart.Col() == r2.aEnd.Col()+1 )
                {   // links
                    rp1.aStart.SetCol( nCol1 );
                    rp2.aStart.SetCol( r2.aStart.Col() );
                    bJoined = TRUE;
                }
                else if ( rp1.aEnd.Col() == nCol1-1
                  && rp2.aEnd.Col() == r2.aEnd.Col()-1 )
                {   // rechts
                    rp1.aEnd.SetCol( nCol2 );
                    rp2.aEnd.SetCol( r2.aEnd.Col() );
                    bJoined = TRUE;
                }
            }
        }
        if ( bJoined )
        {
            if ( bIsInList )
            {   // innerhalb der Liste RangePair loeschen
                Remove( nOldPos );
                delete pOver;
                pOver = NULL;
                if ( nOldPos )
                    nOldPos--;          // Seek richtig aufsetzen
            }
            bJoinedInput = TRUE;
            Join( *p, TRUE );           // rekursiv!
        }
    }
    if ( bIsInList )
        Seek( nOldPos );
    else if ( !bJoinedInput )
        Append( r );
}


BOOL ScRangePairList::operator==( const ScRangePairList& r ) const
{
    if ( this == &r )
        return TRUE;                // identische Referenz
    if ( Count() != r.Count() )
        return FALSE;
    ULONG nCnt = Count();
    for ( ULONG nIdx = 0; nIdx < nCnt; nIdx++ )
    {
        if ( *GetObject( nIdx ) != *r.GetObject( nIdx ) )
            return FALSE;           // auch andere Reihenfolge ist ungleich
    }
    return TRUE;
}


BOOL ScRangePairList::Store( SvStream& rStream ) const
{
    BOOL bOk = TRUE;
    ULONG nCount = Count();
    ULONG nBytes = sizeof(UINT32) + nCount * sizeof(ScRangePair);
#if SC_ROWLIMIT_STREAM_ACCESS
#error address types changed!
    ScWriteHeader aHdr( rStream, nBytes );
    rStream << (UINT32) nCount;
    for ( ULONG j = 0; j < nCount && bOk; j++ )
    {
        rStream << *GetObject( j );
        if( rStream.GetError() != SVSTREAM_OK )
            bOk = FALSE;
    }
    return bOk;
#else
    return FALSE;
#endif // SC_ROWLIMIT_STREAM_ACCESS
}


BOOL ScRangePairList::Load( SvStream& rStream, USHORT nVer )
{
    BOOL bOk = TRUE;
    ScReadHeader aHdr( rStream );
#if SC_ROWLIMIT_STREAM_ACCESS
#error address types changed!
    ScRangePair aRangePair;
    ScRange aRange;
    UINT32 n;
    rStream >> n;
    ULONG nCount = n;
    for ( ULONG j = 0; j < nCount && bOk; j++ )
    {
        if ( nVer < SC_COLROWNAME_RANGEPAIR )
        {   // aus technical Beta 4.0 versuchen mit altem Verhalten zu uebernehmen
            rStream >> aRange;
            aRangePair.GetRange(0) = aRange;
            ScRange& r = aRangePair.GetRange(1);
            r = aRange;
            SCCOL nCol2 = aRange.aEnd.Col();
            SCROW nRow2 = aRange.aEnd.Row();
            if ( static_cast<SCCOLROW>(nCol2 - aRange.aStart.Col()) >=
                    (nRow2 - aRange.aStart.Row()) )
            {   // ColNames
                r.aStart.SetRow( Min( static_cast<SCROW>(nRow2 + 1), MAXROW ) );
                r.aEnd.SetRow( MAXROW );
            }
            else
            {   // RowNames
                r.aStart.SetCol( Min( static_cast<SCCOL>(nCol2 + 1), MAXCOL ) );
                r.aEnd.SetCol( MAXCOL );
            }
        }
        else
            rStream >> aRangePair;
        Append( aRangePair );
        if( rStream.GetError() != SVSTREAM_OK )
            bOk = FALSE;
    }
    return bOk;
#else
    return FALSE;
#endif // SC_ROWLIMIT_STREAM_ACCESS
}


BOOL ScRangePairList::UpdateReference( UpdateRefMode eUpdateRefMode,
                                    ScDocument* pDoc, const ScRange& rWhere,
                                    SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    BOOL bChanged = FALSE;
    if ( Count() )
    {
        SCCOL nCol1;
        SCROW nRow1;
        SCTAB nTab1;
        SCCOL nCol2;
        SCROW nRow2;
        SCTAB nTab2;
        rWhere.GetVars( nCol1, nRow1, nTab1, nCol2, nRow2, nTab2 );
        for ( ScRangePair* pR = First(); pR; pR = Next() )
        {
            for ( USHORT j=0; j<2; j++ )
            {
                ScRange& rRange = pR->GetRange(j);
                SCCOL theCol1;
                SCROW theRow1;
                SCTAB theTab1;
                SCCOL theCol2;
                SCROW theRow2;
                SCTAB theTab2;
                rRange.GetVars( theCol1, theRow1, theTab1, theCol2, theRow2, theTab2 );
                if ( ScRefUpdate::Update( pDoc, eUpdateRefMode,
                        nCol1, nRow1, nTab1, nCol2, nRow2, nTab2,
                        nDx, nDy, nDz,
                        theCol1, theRow1, theTab1, theCol2, theRow2, theTab2 )
                        != UR_NOTHING )
                {
                    bChanged = TRUE;
                    rRange.aStart.Set( theCol1, theRow1, theTab1 );
                    rRange.aEnd.Set( theCol2, theRow2, theTab2 );
                }
            }
        }
    }
    return bChanged;
}


void ScRangePairList::DeleteOnTab( SCTAB nTab )
{
    // Delete entries that have the labels (first range) on nTab

    ULONG nCount = Count();
    ULONG nPos = 0;
    while ( nPos < nCount )
    {
        ScRangePair* pR = GetObject( nPos );
        ScRange aRange = pR->GetRange(0);
        if ( aRange.aStart.Tab() == nTab && aRange.aEnd.Tab() == nTab )
        {
            Remove( nPos );
            delete pR;
            nCount = Count();
        }
        else
            ++nPos;
    }
}


ScRangePair* ScRangePairList::Find( const ScAddress& rAdr ) const
{
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
    {
        ScRangePair* pR = GetObject( j );
        if ( pR->GetRange(0).In( rAdr ) )
            return pR;
    }
    return NULL;
}


ScRangePair* ScRangePairList::Find( const ScRange& rRange ) const
{
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
    {
        ScRangePair* pR = GetObject( j );
        if ( pR->GetRange(0) == rRange )
            return pR;
    }
    return NULL;
}


ScRangePairList* ScRangePairList::Clone() const
{
    ScRangePairList* pNew = new ScRangePairList;
    ULONG nCount = Count();
    for ( ULONG j = 0; j < nCount; j++ )
    {
        pNew->Append( *GetObject( j ) );
    }
    return pNew;
}


struct ScRangePairNameSort
{
    ScRangePair*    pPair;
    ScDocument*     pDoc;
};


int
#ifdef WNT
__cdecl
#endif
ScRangePairList::QsortNameCompare( const void* p1, const void* p2 )
{
    const ScRangePairNameSort* ps1 = (const ScRangePairNameSort*)p1;
    const ScRangePairNameSort* ps2 = (const ScRangePairNameSort*)p2;
    const ScAddress& rPos1 = ps1->pPair->GetRange(0).aStart;
    const ScAddress& rPos2 = ps2->pPair->GetRange(0).aStart;
    String aStr1, aStr2;
    sal_Int32 nComp;
    if ( rPos1.Tab() == rPos2.Tab() )
        nComp = COMPARE_EQUAL;
    else
    {
        ps1->pDoc->GetName( rPos1.Tab(), aStr1 );
        ps2->pDoc->GetName( rPos2.Tab(), aStr2 );
        nComp = ScGlobal::pCollator->compareString( aStr1, aStr2 );
    }
    switch ( nComp )
    {
        case COMPARE_LESS:
            return -1;
        break;
        case COMPARE_GREATER:
            return 1;
        break;
        default:
            // gleiche Tabs
            if ( rPos1.Col() < rPos2.Col() )
                return -1;
            if ( rPos1.Col() > rPos2.Col() )
                return 1;
            // gleiche Cols
            if ( rPos1.Row() < rPos2.Row() )
                return -1;
            if ( rPos1.Row() > rPos2.Row() )
                return 1;
            // erste Ecke gleich, zweite Ecke
            {
                const ScAddress& rPos1 = ps1->pPair->GetRange(0).aEnd;
                const ScAddress& rPos2 = ps2->pPair->GetRange(0).aEnd;
                if ( rPos1.Tab() == rPos2.Tab() )
                    nComp = COMPARE_EQUAL;
                else
                {
                    ps1->pDoc->GetName( rPos1.Tab(), aStr1 );
                    ps2->pDoc->GetName( rPos2.Tab(), aStr2 );
                    nComp = ScGlobal::pCollator->compareString( aStr1, aStr2 );
                }
                switch ( nComp )
                {
                    case COMPARE_LESS:
                        return -1;
                    break;
                    case COMPARE_GREATER:
                        return 1;
                    break;
                    default:
                        // gleiche Tabs
                        if ( rPos1.Col() < rPos2.Col() )
                            return -1;
                        if ( rPos1.Col() > rPos2.Col() )
                            return 1;
                        // gleiche Cols
                        if ( rPos1.Row() < rPos2.Row() )
                            return -1;
                        if ( rPos1.Row() > rPos2.Row() )
                            return 1;
                        return 0;
                }
            }
            return 0;
    }
    return 0; // just in case
}


ScRangePair** ScRangePairList::CreateNameSortedArray( ULONG& nCount,
        ScDocument* pDoc ) const
{
    nCount = Count();
    DBG_ASSERT( nCount * sizeof(ScRangePairNameSort) <= (size_t)~0x1F,
        "ScRangePairList::CreateNameSortedArray nCount * sizeof(ScRangePairNameSort) > (size_t)~0x1F" );
    ScRangePairNameSort* pSortArray = (ScRangePairNameSort*)
        new BYTE [ nCount * sizeof(ScRangePairNameSort) ];
    ULONG j;
    for ( j=0; j < nCount; j++ )
    {
        pSortArray[j].pPair = GetObject( j );
        pSortArray[j].pDoc = pDoc;
    }
#if !(defined(ICC ) && defined(OS2))
    qsort( (void*)pSortArray, nCount, sizeof(ScRangePairNameSort), &ScRangePairList::QsortNameCompare );
#else
    qsort( (void*)pSortArray, nCount, sizeof(ScRangePairNameSort), ICCQsortRPairCompare );
#endif
    // ScRangePair Pointer aufruecken
    ScRangePair** ppSortArray = (ScRangePair**)pSortArray;
    for ( j=0; j < nCount; j++ )
    {
        ppSortArray[j] = pSortArray[j].pPair;
    }
    return ppSortArray;
}




