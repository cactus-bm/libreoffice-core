/*************************************************************************
 *
 *  $RCSfile: poly2.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2003-07-02 14:21:27 $
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

#define _SV_POLY2_CXX

#ifdef HAVE_GPC_H

#ifndef __gpc_h
extern "C"
{
    #include <external/gpc/gpc.h>
}
#endif

#else

// No GPC
#define GPC_INT 0
#define GPC_UNION 0
#define GPC_DIFF 0
#define GPC_XOR 0

#endif // HAVE_GPC_H

#include <cstring>
#include <cmath>
#ifndef _POLY_H
#include <poly.h>
#endif
#ifndef _POLY_HXX
#include <poly.hxx>
#endif
#ifndef _DEBUG_HXX //autogen
#include <debug.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <stream.hxx>
#endif
#ifndef _VCOMPAT_HXX
#include <vcompat.hxx>
#endif

// ---------------
// - PolyPolygon -
// ---------------

DBG_NAME( PolyPolygon );

// -----------------------------------------------------------------------

ImplPolyPolygon::ImplPolyPolygon( USHORT nInitSize )
{
    mnRefCount  = 1;
    mnCount     = nInitSize;
    mnSize      = nInitSize;
    mnResize    = 16;
    mpPolyAry   = new SVPPOLYGON[ nInitSize ];
}

// -----------------------------------------------------------------------

ImplPolyPolygon::ImplPolyPolygon( const ImplPolyPolygon& rImplPolyPoly )
{
    mnRefCount  = 1;
    mnCount     = rImplPolyPoly.mnCount;
    mnSize      = rImplPolyPoly.mnSize;
    mnResize    = rImplPolyPoly.mnResize;

    if ( rImplPolyPoly.mpPolyAry )
    {
        mpPolyAry = new SVPPOLYGON[mnSize];
        for ( USHORT i = 0; i < mnCount; i++ )
            mpPolyAry[i] = new Polygon( *rImplPolyPoly.mpPolyAry[i] );
    }
    else
        mpPolyAry = NULL;
}

// -----------------------------------------------------------------------

ImplPolyPolygon::~ImplPolyPolygon()
{
    if ( mpPolyAry )
    {
        for ( USHORT i = 0; i < mnCount; i++ )
            delete mpPolyAry[i];
        delete[] mpPolyAry;
    }
}

// =======================================================================

PolyPolygon::PolyPolygon( USHORT nInitSize, USHORT nResize )
{
    DBG_CTOR( PolyPolygon, NULL );

    if ( nInitSize > MAX_POLYGONS )
        nInitSize = MAX_POLYGONS;
    else if ( !nInitSize )
        nInitSize = 1;
    if ( nResize > MAX_POLYGONS )
        nResize = MAX_POLYGONS;
    else if ( !nResize )
        nResize = 1;
    mpImplPolyPolygon = new ImplPolyPolygon( nInitSize, nResize );
}

// -----------------------------------------------------------------------

PolyPolygon::PolyPolygon( const Polygon& rPoly )
{
    DBG_CTOR( PolyPolygon, NULL );

    if ( rPoly.GetSize() )
    {
        mpImplPolyPolygon = new ImplPolyPolygon( 1 );
        mpImplPolyPolygon->mpPolyAry[0] = new Polygon( rPoly );
    }
    else
        mpImplPolyPolygon = new ImplPolyPolygon( 16, 16 );
}

// -----------------------------------------------------------------------

PolyPolygon::PolyPolygon( USHORT nPoly, const USHORT* pPointCountAry,
                          const Point* pPtAry )
{
    DBG_CTOR( PolyPolygon, NULL );

    if ( nPoly > MAX_POLYGONS )
        nPoly = MAX_POLYGONS;

    mpImplPolyPolygon = new ImplPolyPolygon( nPoly );
    for ( USHORT i = 0; i < nPoly; i++ )
    {
        mpImplPolyPolygon->mpPolyAry[i] = new Polygon( *pPointCountAry, pPtAry );
        pPtAry += *pPointCountAry;
        pPointCountAry++;
    }
}

// -----------------------------------------------------------------------

PolyPolygon::PolyPolygon( const PolyPolygon& rPolyPoly )
{
    DBG_CTOR( PolyPolygon, NULL );
    DBG_CHKOBJ( &rPolyPoly, PolyPolygon, NULL );
    DBG_ASSERT( rPolyPoly.mpImplPolyPolygon->mnRefCount < 0xFFFE, "PolyPolygon: RefCount overflow" );

    mpImplPolyPolygon = rPolyPoly.mpImplPolyPolygon;
    mpImplPolyPolygon->mnRefCount++;
}

// -----------------------------------------------------------------------

PolyPolygon::~PolyPolygon()
{
    DBG_DTOR( PolyPolygon, NULL );

    if ( mpImplPolyPolygon->mnRefCount > 1 )
        mpImplPolyPolygon->mnRefCount--;
    else
        delete mpImplPolyPolygon;
}

// -----------------------------------------------------------------------

void PolyPolygon::Insert( const Polygon& rPoly, USHORT nPos )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    if ( mpImplPolyPolygon->mnCount >= MAX_POLYGONS )
        return;

    if ( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    if ( nPos > mpImplPolyPolygon->mnCount )
        nPos = mpImplPolyPolygon->mnCount;

    if ( !mpImplPolyPolygon->mpPolyAry )
        mpImplPolyPolygon->mpPolyAry = new SVPPOLYGON[mpImplPolyPolygon->mnSize];
    else if ( mpImplPolyPolygon->mnCount == mpImplPolyPolygon->mnSize )
    {
        USHORT      nOldSize = mpImplPolyPolygon->mnSize;
        USHORT      nNewSize = nOldSize + mpImplPolyPolygon->mnResize;
        SVPPOLYGON* pNewAry;

        if ( nNewSize >= MAX_POLYGONS )
            nNewSize = MAX_POLYGONS;
        pNewAry = new SVPPOLYGON[nNewSize];
        memcpy( pNewAry, mpImplPolyPolygon->mpPolyAry, nPos*sizeof(SVPPOLYGON) );
        memcpy( pNewAry+nPos+1, mpImplPolyPolygon->mpPolyAry+nPos,
                (nOldSize-nPos)*sizeof(SVPPOLYGON) );
        delete[] mpImplPolyPolygon->mpPolyAry;
        mpImplPolyPolygon->mpPolyAry = pNewAry;
        mpImplPolyPolygon->mnSize = nNewSize;
    }
    else if ( nPos < mpImplPolyPolygon->mnCount )
    {
        memmove( mpImplPolyPolygon->mpPolyAry+nPos+1,
                 mpImplPolyPolygon->mpPolyAry+nPos,
                 (mpImplPolyPolygon->mnCount-nPos)*sizeof(SVPPOLYGON) );
    }

    mpImplPolyPolygon->mpPolyAry[nPos] = new Polygon( rPoly );
    mpImplPolyPolygon->mnCount++;
}

// -----------------------------------------------------------------------

void PolyPolygon::Remove( USHORT nPos )
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_ASSERT( nPos < Count(), "PolyPolygon::Remove(): nPos >= nSize" );

    if ( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    delete mpImplPolyPolygon->mpPolyAry[nPos];
    mpImplPolyPolygon->mnCount--;
    memmove( mpImplPolyPolygon->mpPolyAry+nPos,
             mpImplPolyPolygon->mpPolyAry+nPos+1,
             (mpImplPolyPolygon->mnCount-nPos)*sizeof(SVPPOLYGON) );
}

// -----------------------------------------------------------------------

void PolyPolygon::Replace( const Polygon& rPoly, USHORT nPos )
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_ASSERT( nPos < Count(), "PolyPolygon::Replace(): nPos >= nSize" );

    if ( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    delete mpImplPolyPolygon->mpPolyAry[nPos];
    mpImplPolyPolygon->mpPolyAry[nPos] = new Polygon( rPoly );
}

// -----------------------------------------------------------------------

const Polygon& PolyPolygon::GetObject( USHORT nPos ) const
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_ASSERT( nPos < Count(), "PolyPolygon::GetObject(): nPos >= nSize" );

    return *(mpImplPolyPolygon->mpPolyAry[nPos]);
}

// -----------------------------------------------------------------------

BOOL PolyPolygon::IsRect() const
{
    BOOL bIsRect = FALSE;
    if ( Count() == 1 )
        bIsRect = mpImplPolyPolygon->mpPolyAry[ 0 ]->IsRect();
    return bIsRect;
}

// -----------------------------------------------------------------------

void PolyPolygon::Clear()
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    if ( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( mpImplPolyPolygon->mnResize,
                                                 mpImplPolyPolygon->mnResize );
    }
    else
    {
        if ( mpImplPolyPolygon->mpPolyAry )
        {
            for ( USHORT i = 0; i < mpImplPolyPolygon->mnCount; i++ )
                delete mpImplPolyPolygon->mpPolyAry[i];
            delete[] mpImplPolyPolygon->mpPolyAry;
            mpImplPolyPolygon->mpPolyAry = NULL;
            mpImplPolyPolygon->mnCount   = 0;
            mpImplPolyPolygon->mnSize    = mpImplPolyPolygon->mnResize;
        }
    }
}

// -----------------------------------------------------------------------

void PolyPolygon::Optimize( ULONG nOptimizeFlags, const PolyOptimizeData* pData )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    if( nOptimizeFlags )
    {
        double      fArea;
        const BOOL  bEdges = ( nOptimizeFlags & POLY_OPTIMIZE_EDGES ) == POLY_OPTIMIZE_EDGES;
        USHORT      nPercent;

        if( bEdges )
        {
            const Rectangle aBound( GetBoundRect() );

            fArea = ( aBound.GetWidth() + aBound.GetHeight() ) * 0.5;
            nPercent = pData ? pData->GetPercentValue() : 50;
            nOptimizeFlags &= ~POLY_OPTIMIZE_EDGES;
        }

        // watch for ref counter
        if( mpImplPolyPolygon->mnRefCount > 1 )
        {
            mpImplPolyPolygon->mnRefCount--;
            mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
        }

        // Optimize polygons
        for( USHORT i = 0, nPolyCount = mpImplPolyPolygon->mnCount; i < nPolyCount; i++ )
        {
            if( bEdges )
            {
                mpImplPolyPolygon->mpPolyAry[ i ]->Optimize( POLY_OPTIMIZE_NO_SAME );
                Polygon::ImplReduceEdges( *( mpImplPolyPolygon->mpPolyAry[ i ] ), fArea, nPercent );
            }

            if( nOptimizeFlags )
                mpImplPolyPolygon->mpPolyAry[ i ]->Optimize( nOptimizeFlags, pData );
        }
    }
}

// -----------------------------------------------------------------------

void PolyPolygon::GetSimple( PolyPolygon& rResult, long nDelta ) const
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    rResult.Clear();

    Polygon aPolygon;

    for( USHORT i = 0; i < mpImplPolyPolygon->mnCount; i++ )
    {
        mpImplPolyPolygon->mpPolyAry[ i ]->GetSimple( aPolygon, nDelta );
        rResult.Insert( aPolygon );
    }
}

// -----------------------------------------------------------------------

void PolyPolygon::AdaptiveSubdivide( PolyPolygon& rResult, const double d ) const
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    rResult.Clear();

    Polygon aPolygon;

    for( USHORT i = 0; i < mpImplPolyPolygon->mnCount; i++ )
    {
        mpImplPolyPolygon->mpPolyAry[ i ]->AdaptiveSubdivide( aPolygon, d );
        rResult.Insert( aPolygon );
    }
}

// -----------------------------------------------------------------------

void PolyPolygon::GetIntersection( const PolyPolygon& rPolyPoly, PolyPolygon& rResult ) const
{
    ImplDoOperation( rPolyPoly, rResult, GPC_INT );
}

// -----------------------------------------------------------------------

void PolyPolygon::GetUnion( const PolyPolygon& rPolyPoly, PolyPolygon& rResult ) const
{
    ImplDoOperation( rPolyPoly, rResult, GPC_UNION );
}

// -----------------------------------------------------------------------

void PolyPolygon::GetDifference( const PolyPolygon& rPolyPoly, PolyPolygon& rResult ) const
{
    ImplDoOperation( rPolyPoly, rResult, GPC_DIFF );
}

// -----------------------------------------------------------------------

void PolyPolygon::GetXOR( const PolyPolygon& rPolyPoly, PolyPolygon& rResult ) const
{
    ImplDoOperation( rPolyPoly, rResult, GPC_XOR );
}

// -----------------------------------------------------------------------

#ifdef HAVE_GPC_H

void* PolyPolygon::ImplCreateGPCPolygon() const
{
    gpc_polygon* pRet = new gpc_polygon;

    pRet->num_contours = 0;
    pRet->hole = NULL;
    pRet->contour = NULL;

    for( USHORT i = 0, nCount = Count(); i < nCount; i++ )
    {
        const Polygon&  rPoly = GetObject( i );
        const USHORT    nSize = rPoly.GetSize();

        if( nSize > 1 )
        {
            gpc_vertex_list aVertexList;
            gpc_vertex*     pVertex;

            aVertexList.num_vertices = nSize;
            aVertexList.vertex = pVertex = new gpc_vertex[ nSize ];

            for( USHORT nPos = 0; nPos < nSize; nPos++, pVertex++ )
            {
                const Point& rPoint = rPoly[ nPos ];
                pVertex->x = rPoint.X();
                pVertex->y = rPoint.Y();
            }

            gpc_add_contour( pRet, &aVertexList, 0 );
            delete[] aVertexList.vertex;
        }
    }

    return pRet;
}

// -----------------------------------------------------------------------

void PolyPolygon::ImplDoOperation( const PolyPolygon& rPolyPoly, PolyPolygon& rResult, ULONG nOperation ) const
{
    gpc_polygon* pGPCPoly1 = (gpc_polygon*) ImplCreateGPCPolygon();
    gpc_polygon* pGPCPoly2 = (gpc_polygon*) rPolyPoly.ImplCreateGPCPolygon();
    gpc_polygon* pResult = new gpc_polygon;

    pResult->num_contours = 0;
    pResult->hole = NULL;
    pResult->contour = NULL;

    gpc_polygon_clip( (gpc_op) nOperation, pGPCPoly1, pGPCPoly2, pResult );

    rResult.Clear();

    for( int i = 0; i < pResult->num_contours; i++ )
    {
        gpc_vertex_list&    rVertexList = pResult->contour[ i ];
        Polygon             aPoly( rVertexList.num_vertices );

        for( int j = 0; j < rVertexList.num_vertices; j++ )
        {
            Point& rPt = aPoly[ j ];
            rPt.X() = FRound( rVertexList.vertex[ j ].x );
            rPt.Y() = FRound( rVertexList.vertex[ j ].y );
        }

        rResult.Insert( aPoly );
    }

    gpc_free_polygon( pGPCPoly1 );
    delete pGPCPoly1;

    gpc_free_polygon( pGPCPoly2 );
    delete pGPCPoly2;

    gpc_free_polygon( pResult );
    delete pResult;
}

#else
// No GPC implementation

void* PolyPolygon::ImplCreateGPCPolygon() const
{
    return NULL;
}

void PolyPolygon::ImplDoOperation( const PolyPolygon& rPolyPoly, PolyPolygon& rResult, ULONG nOperation ) const
{
    return;
}

#endif // HAVE_GPC_H

// -----------------------------------------------------------------------

USHORT PolyPolygon::Count() const
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    return mpImplPolyPolygon->mnCount;
}

// -----------------------------------------------------------------------

void PolyPolygon::Move( long nHorzMove, long nVertMove )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    // Diese Abfrage sollte man fuer die DrawEngine durchfuehren
    if( nHorzMove || nVertMove )
    {
        // Referenzcounter beruecksichtigen
        if ( mpImplPolyPolygon->mnRefCount > 1 )
        {
            mpImplPolyPolygon->mnRefCount--;
            mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
        }

        // Punkte verschieben
        USHORT nPolyCount = mpImplPolyPolygon->mnCount;
        for ( USHORT i = 0; i < nPolyCount; i++ )
            mpImplPolyPolygon->mpPolyAry[i]->Move( nHorzMove, nVertMove );
    }
}

// -----------------------------------------------------------------------

void PolyPolygon::Translate( const Point& rTrans )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    // Referenzcounter beruecksichtigen
    if( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    // Punkte verschieben
    for ( USHORT i = 0, nCount = mpImplPolyPolygon->mnCount; i < nCount; i++ )
        mpImplPolyPolygon->mpPolyAry[ i ]->Translate( rTrans );
}

// -----------------------------------------------------------------------

void PolyPolygon::Scale( double fScaleX, double fScaleY )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    // Referenzcounter beruecksichtigen
    if( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    // Punkte verschieben
    for ( USHORT i = 0, nCount = mpImplPolyPolygon->mnCount; i < nCount; i++ )
        mpImplPolyPolygon->mpPolyAry[ i ]->Scale( fScaleX, fScaleY );
}

// -----------------------------------------------------------------------

void PolyPolygon::Rotate( const Point& rCenter, USHORT nAngle10 )
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    nAngle10 %= 3600;

    if( nAngle10 )
    {
        const double fAngle = F_PI1800 * nAngle10;
        Rotate( rCenter, sin( fAngle ), cos( fAngle ) );
    }
}

// -----------------------------------------------------------------------

void PolyPolygon::Rotate( const Point& rCenter, double fSin, double fCos )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    // Referenzcounter beruecksichtigen
    if( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    // Punkte verschieben
    for ( USHORT i = 0, nCount = mpImplPolyPolygon->mnCount; i < nCount; i++ )
        mpImplPolyPolygon->mpPolyAry[ i ]->Rotate( rCenter, fSin, fCos );
}

// -----------------------------------------------------------------------

void PolyPolygon::SlantX( long nYRef, double fSin, double fCos )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    // Referenzcounter beruecksichtigen
    if( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    // Punkte verschieben
    for ( USHORT i = 0, nCount = mpImplPolyPolygon->mnCount; i < nCount; i++ )
        mpImplPolyPolygon->mpPolyAry[ i ]->SlantX( nYRef, fSin, fCos );
}

// -----------------------------------------------------------------------

void PolyPolygon::SlantY( long nXRef, double fSin, double fCos )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    // Referenzcounter beruecksichtigen
    if( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    // Punkte verschieben
    for ( USHORT i = 0, nCount = mpImplPolyPolygon->mnCount; i < nCount; i++ )
        mpImplPolyPolygon->mpPolyAry[ i ]->SlantY( nXRef, fSin, fCos );
}

// -----------------------------------------------------------------------

void PolyPolygon::Distort( const Rectangle& rRefRect, const Polygon& rDistortedRect )
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    // Referenzcounter beruecksichtigen
    if( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    // Punkte verschieben
    for ( USHORT i = 0, nCount = mpImplPolyPolygon->mnCount; i < nCount; i++ )
        mpImplPolyPolygon->mpPolyAry[ i ]->Distort( rRefRect, rDistortedRect );
}


// -----------------------------------------------------------------------

void PolyPolygon::Clip( const Rectangle& rRect )
{
    // Polygon-Clippen
    USHORT nPolyCount = mpImplPolyPolygon->mnCount;
    USHORT i;

    if ( !nPolyCount )
        return;

    // Referenzcounter beruecksichtigen
    if ( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    // Erst jedes Polygon Clippen und dann die leeren entfernen
    for ( i = 0; i < nPolyCount; i++ )
        mpImplPolyPolygon->mpPolyAry[i]->Clip( rRect );
    while ( nPolyCount )
    {
        if ( GetObject( nPolyCount-1 ).GetSize() <= 2 )
            Remove( nPolyCount-1 );
        nPolyCount--;
    }
}

// -----------------------------------------------------------------------

Rectangle PolyPolygon::GetBoundRect() const
{
    DBG_CHKTHIS( PolyPolygon, NULL );

    long    nXMin, nXMax, nYMin, nYMax;
    BOOL    bFirst = TRUE;
    USHORT  nPolyCount = mpImplPolyPolygon->mnCount;

    for ( USHORT n = 0; n < nPolyCount; n++ )
    {
        const Polygon*  pPoly = mpImplPolyPolygon->mpPolyAry[n];
        const Point*    pAry = pPoly->GetConstPointAry();
        USHORT          nPointCount = pPoly->GetSize();

        for ( USHORT i = 0; i < nPointCount; i++ )
        {
            const Point* pPt = &pAry[ i ];

            if ( bFirst )
            {
                nXMin = nXMax = pPt->X();
                nYMin = nYMax = pPt->Y();
                bFirst = FALSE;
            }
            else
            {
                if ( pPt->X() < nXMin )
                    nXMin = pPt->X();
                if ( pPt->X() > nXMax )
                    nXMax = pPt->X();
                if ( pPt->Y() < nYMin )
                    nYMin = pPt->Y();
                if ( pPt->Y() > nYMax )
                    nYMax = pPt->Y();
            }
        }
    }

    if ( !bFirst )
        return Rectangle( nXMin, nYMin, nXMax, nYMax );
    else
        return Rectangle();
}

// -----------------------------------------------------------------------

Polygon& PolyPolygon::operator[]( USHORT nPos )
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_ASSERT( nPos < Count(), "PolyPolygon::[](): nPos >= nSize" );

    if ( mpImplPolyPolygon->mnRefCount > 1 )
    {
        mpImplPolyPolygon->mnRefCount--;
        mpImplPolyPolygon = new ImplPolyPolygon( *mpImplPolyPolygon );
    }

    return *(mpImplPolyPolygon->mpPolyAry[nPos]);
}

// -----------------------------------------------------------------------

PolyPolygon& PolyPolygon::operator=( const PolyPolygon& rPolyPoly )
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_CHKOBJ( &rPolyPoly, PolyPolygon, NULL );
    DBG_ASSERT( rPolyPoly.mpImplPolyPolygon->mnRefCount < 0xFFFE, "PolyPolygon: RefCount overflow" );

    rPolyPoly.mpImplPolyPolygon->mnRefCount++;

    if ( mpImplPolyPolygon->mnRefCount > 1 )
        mpImplPolyPolygon->mnRefCount--;
    else
        delete mpImplPolyPolygon;

    mpImplPolyPolygon = rPolyPoly.mpImplPolyPolygon;
    return *this;
}

// -----------------------------------------------------------------------

BOOL PolyPolygon::operator==( const PolyPolygon& rPolyPoly ) const
{
    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_CHKOBJ( &rPolyPoly, PolyPolygon, NULL );

    if ( rPolyPoly.mpImplPolyPolygon == mpImplPolyPolygon )
        return TRUE;
    else
        return FALSE;
}

// -----------------------------------------------------------------------

sal_Bool PolyPolygon::IsEqual( const PolyPolygon& rPolyPoly ) const
{
    sal_Bool bIsEqual = sal_True;
    if ( Count() != rPolyPoly.Count() )
        bIsEqual = sal_False;
    else
    {
        sal_uInt16 i;
        for ( i = 0; i < Count(); i++ )
        {
            if (!GetObject( i ).IsEqual( rPolyPoly.GetObject( i ) ) )
            {
                sal_Bool bIsEqual = sal_False;
                break;
            }
        }
    }
    return bIsEqual;
}

// -----------------------------------------------------------------------

SvStream& operator>>( SvStream& rIStream, PolyPolygon& rPolyPoly )
{
    DBG_CHKOBJ( &rPolyPoly, PolyPolygon, NULL );
    DBG_ASSERTWARNING( rIStream.GetVersion(), "PolyPolygon::>> - Solar-Version not set on rIStream" );

    Polygon* pPoly;
    USHORT   nPolyCount;

    // Anzahl der Polygone einlesen
    rIStream >> nPolyCount;

    // Daten anlegen
    if( nPolyCount )
    {
        // Referenzcounter beruecksichtigen
        if ( rPolyPoly.mpImplPolyPolygon->mnRefCount > 1 )
            rPolyPoly.mpImplPolyPolygon->mnRefCount--;
        else
            delete rPolyPoly.mpImplPolyPolygon;

        rPolyPoly.mpImplPolyPolygon = new ImplPolyPolygon( nPolyCount );

        for ( USHORT i = 0; i < nPolyCount; i++ )
        {
            pPoly = new Polygon;
            rIStream >> *pPoly;
            rPolyPoly.mpImplPolyPolygon->mpPolyAry[i] = pPoly;
        }
    }
    else
        rPolyPoly = PolyPolygon();

    return rIStream;
}

// -----------------------------------------------------------------------

SvStream& operator<<( SvStream& rOStream, const PolyPolygon& rPolyPoly )
{
    DBG_CHKOBJ( &rPolyPoly, PolyPolygon, NULL );
    DBG_ASSERTWARNING( rOStream.GetVersion(), "PolyPolygon::<< - Solar-Version not set on rOStream" );

    // Anzahl der Polygone rausschreiben
    USHORT nPolyCount = rPolyPoly.mpImplPolyPolygon->mnCount;
    rOStream << nPolyCount;

    // Die einzelnen Polygone ausgeben
    for ( USHORT i = 0; i < nPolyCount; i++ )
        rOStream << *(rPolyPoly.mpImplPolyPolygon->mpPolyAry[i]);

    return rOStream;
}

// -----------------------------------------------------------------------

void PolyPolygon::Read( SvStream& rIStream )
{
    VersionCompat aCompat( rIStream, STREAM_READ );

    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_ASSERTWARNING( rIStream.GetVersion(), "PolyPolygon::>> - Solar-Version not set on rIStream" );

    Polygon* pPoly;
    USHORT   nPolyCount;

    // Anzahl der Polygone einlesen
    rIStream >> nPolyCount;

    // Daten anlegen
    if( nPolyCount )
    {
        // Referenzcounter beruecksichtigen
        if ( mpImplPolyPolygon->mnRefCount > 1 )
            mpImplPolyPolygon->mnRefCount--;
        else
            delete mpImplPolyPolygon;

        mpImplPolyPolygon = new ImplPolyPolygon( nPolyCount );

        for ( USHORT i = 0; i < nPolyCount; i++ )
        {
            pPoly = new Polygon;
            pPoly->ImplRead( rIStream );
            mpImplPolyPolygon->mpPolyAry[i] = pPoly;
        }
    }
    else
        *this = PolyPolygon();
}

// -----------------------------------------------------------------------

void PolyPolygon::Write( SvStream& rOStream ) const
{
    VersionCompat aCompat( rOStream, STREAM_WRITE, 1 );

    DBG_CHKTHIS( PolyPolygon, NULL );
    DBG_ASSERTWARNING( rOStream.GetVersion(), "PolyPolygon::<< - Solar-Version not set on rOStream" );

    // Anzahl der Polygone rausschreiben
    USHORT nPolyCount = mpImplPolyPolygon->mnCount;
    rOStream << nPolyCount;

    // Die einzelnen Polygone ausgeben
    for ( USHORT i = 0; i < nPolyCount; i++ )
        mpImplPolyPolygon->mpPolyAry[i]->ImplWrite( rOStream );;
}
