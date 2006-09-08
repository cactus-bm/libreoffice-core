/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: bmpacc3.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2006-09-08 08:33:54 $
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

#ifndef _SV_SALBTYPE_HXX
#include <salbtype.hxx>
#endif
#ifndef _SV_BITMAP_HXX
#include <bitmap.hxx>
#endif
#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _SV_REGION_HXX
#include <region.hxx>
#endif
#ifndef _SV_BMPACC_HXX
#include <bmpacc.hxx>
#endif
#ifndef _SV_BMPFAST_HXX
#include <bmpfast.hxx>
#endif

// ---------------------
// - BitmapWriteAccess -
// ---------------------

void BitmapWriteAccess::SetLineColor()
{
    delete mpLineColor;
    mpLineColor = NULL;
}

// ------------------------------------------------------------------

void BitmapWriteAccess::SetLineColor( const Color& rColor )
{
    delete mpLineColor;

    if( rColor.GetTransparency() == 255 )
        mpLineColor = NULL;
    else
        mpLineColor = ( HasPalette() ? new BitmapColor(  (BYTE) GetBestPaletteIndex( rColor ) ) : new BitmapColor( rColor ) );
}

// ------------------------------------------------------------------

Color BitmapWriteAccess::GetLineColor() const
{
    Color aRet;

    if( mpLineColor )
        aRet = (const Color&) *mpLineColor;
    else
        aRet.SetTransparency( 255 );

    return aRet;
}

// ------------------------------------------------------------------

void BitmapWriteAccess::SetFillColor()
{
    delete mpFillColor;
    mpFillColor = NULL;
}

// ------------------------------------------------------------------

void BitmapWriteAccess::SetFillColor( const Color& rColor )
{
    delete mpFillColor;

    if( rColor.GetTransparency() == 255 )
        mpFillColor = NULL;
    else
        mpFillColor = ( HasPalette() ? new BitmapColor(  (BYTE) GetBestPaletteIndex( rColor ) ) : new BitmapColor( rColor ) );
}

// ------------------------------------------------------------------

Color BitmapWriteAccess::GetFillColor() const
{
    Color aRet;

    if( mpFillColor )
        aRet = (const Color&) *mpFillColor;
    else
        aRet.SetTransparency( 255 );

    return aRet;
}

// ------------------------------------------------------------------

void BitmapWriteAccess::Erase( const Color& rColor )
{
    // convert the color format from RGB to palette index if needed
    // TODO: provide and use Erase( BitmapColor& method)
    BitmapColor aColor = rColor;
    if( HasPalette() )
        aColor = BitmapColor( (BYTE)GetBestPaletteIndex( rColor) );
    // try fast bitmap method first
    if( ImplFastEraseBitmap( *mpBuffer, aColor ) )
        return;

    // use the canonical method to clear the bitmap
    BitmapColor*    pOldFillColor = mpFillColor ? new BitmapColor( *mpFillColor ) : NULL;
    const Point     aPoint;
    const Rectangle aRect( aPoint, maBitmap.GetSizePixel() );

    SetFillColor( rColor );
    FillRect( aRect );
    delete mpFillColor;
    mpFillColor = pOldFillColor;
}

// ------------------------------------------------------------------

void BitmapWriteAccess::DrawLine( const Point& rStart, const Point& rEnd )
{
    if( mpLineColor )
    {
        const BitmapColor&  rLineColor = *mpLineColor;
        long                nX, nY;

        if ( rStart.X() == rEnd.X() )
        {
            // vertikale Line
            const long nEndY = rEnd.Y();

            nX = rStart.X();
            nY = rStart.Y();

            if ( nEndY > nY )
            {
                for (; nY <= nEndY; nY++ )
                    SetPixel( nY, nX, rLineColor );
            }
            else
            {
                for (; nY >= nEndY; nY-- )
                    SetPixel( nY, nX, rLineColor );
            }
        }
        else if ( rStart.Y() == rEnd.Y() )
        {
            // horizontale Line
            const long nEndX = rEnd.X();

            nX = rStart.X();
            nY = rStart.Y();

            if ( nEndX > nX )
            {
                for (; nX <= nEndX; nX++ )
                    SetPixel( nY, nX, rLineColor );
            }
            else
            {
                for (; nX >= nEndX; nX-- )
                    SetPixel( nY, nX, rLineColor );
            }
        }
        else
        {
            const long  nDX = labs( rEnd.X() - rStart.X() );
            const long  nDY = labs( rEnd.Y() - rStart.Y() );
            long        nX1;
            long        nY1;
            long        nX2;
            long        nY2;

            if ( nDX >= nDY )
            {
                if ( rStart.X() < rEnd.X() )
                {
                    nX1 = rStart.X();
                    nY1 = rStart.Y();
                    nX2 = rEnd.X();
                    nY2 = rEnd.Y();
                }
                else
                {
                    nX1 = rEnd.X();
                    nY1 = rEnd.Y();
                    nX2 = rStart.X();
                    nY2 = rStart.Y();
                }

                const long  nDYX = ( nDY - nDX ) << 1;
                const long  nDY2 = nDY << 1;
                long        nD = nDY2 - nDX;
                BOOL        bPos = nY1 < nY2;

                for ( nX = nX1, nY = nY1; nX <= nX2; nX++ )
                {
                    SetPixel( nY, nX, rLineColor );

                    if ( nD < 0 )
                        nD += nDY2;
                    else
                    {
                        nD += nDYX;

                        if ( bPos )
                            nY++;
                        else
                            nY--;
                    }
                }
            }
            else
            {
                if ( rStart.Y() < rEnd.Y() )
                {
                    nX1 = rStart.X();
                    nY1 = rStart.Y();
                    nX2 = rEnd.X();
                    nY2 = rEnd.Y();
                }
                else
                {
                    nX1 = rEnd.X();
                    nY1 = rEnd.Y();
                    nX2 = rStart.X();
                    nY2 = rStart.Y();
                }

                const long  nDYX = ( nDX - nDY ) << 1;
                const long  nDY2 = nDX << 1;
                long        nD = nDY2 - nDY;
                BOOL        bPos = nX1 < nX2;

                for ( nX = nX1, nY = nY1; nY <= nY2; nY++ )
                {
                    SetPixel( nY, nX, rLineColor );

                    if ( nD < 0 )
                        nD += nDY2;
                    else
                    {
                        nD += nDYX;

                        if ( bPos )
                            nX++;
                        else
                            nX--;
                    }
                }
            }
        }
    }
}

// ------------------------------------------------------------------

void BitmapWriteAccess::FillRect( const Rectangle& rRect )
{
    if( mpFillColor )
    {
        const BitmapColor&  rFillColor = *mpFillColor;
        Point               aPoint;
        Rectangle           aRect( aPoint, maBitmap.GetSizePixel() );

        aRect.Intersection( rRect );

        if( !aRect.IsEmpty() )
        {
            const long  nStartX = rRect.Left();
            const long  nStartY = rRect.Top();
            const long  nEndX = rRect.Right();
            const long  nEndY = rRect.Bottom();

            for( long nY = nStartY; nY <= nEndY; nY++ )
                for( long nX = nStartX; nX <= nEndX; nX++ )
                    SetPixel( nY, nX, rFillColor );
        }
    }
}

// ------------------------------------------------------------------

void BitmapWriteAccess::DrawRect( const Rectangle& rRect )
{
    if( mpFillColor )
        FillRect( rRect );

    if( mpLineColor && ( !mpFillColor || ( *mpFillColor != *mpLineColor ) ) )
    {
        DrawLine( rRect.TopLeft(), rRect.TopRight() );
        DrawLine( rRect.TopRight(), rRect.BottomRight() );
        DrawLine( rRect.BottomRight(), rRect.BottomLeft() );
        DrawLine( rRect.BottomLeft(), rRect.TopLeft() );
    }
}

// ------------------------------------------------------------------

void BitmapWriteAccess::FillPolygon( const Polygon& rPoly )
{
    const USHORT nSize = rPoly.GetSize();

    if( nSize && mpFillColor )
    {
        const BitmapColor&  rFillColor = *mpFillColor;
        Region              aRegion( rPoly );
        Rectangle           aRect;

        aRegion.Intersect( Rectangle( Point(), Size( Width(), Height() ) ) );

        if( !aRegion.IsEmpty() )
        {
            RegionHandle aRegHandle( aRegion.BeginEnumRects() );

            while( aRegion.GetNextEnumRect( aRegHandle, aRect ) )
                for( long nY = aRect.Top(), nEndY = aRect.Bottom(); nY <= nEndY; nY++ )
                    for( long nX = aRect.Left(), nEndX = aRect.Right(); nX <= nEndX; nX++ )
                        SetPixel( nY, nX, rFillColor );

            aRegion.EndEnumRects( aRegHandle );
        }
    }
}

// ------------------------------------------------------------------

void BitmapWriteAccess::DrawPolygon( const Polygon& rPoly )
{
    if( mpFillColor )
        FillPolygon( rPoly );

    if( mpLineColor && ( !mpFillColor || ( *mpFillColor != *mpLineColor ) ) )
    {
        const USHORT nSize = rPoly.GetSize();

        for( USHORT i = 0, nSize1 = nSize - 1; i < nSize1; i++ )
            DrawLine( rPoly[ i ], rPoly[ i + 1 ] );

        if( rPoly[ nSize - 1 ] != rPoly[ 0 ] )
            DrawLine( rPoly[ nSize - 1 ], rPoly[ 0 ] );
    }
}

// ------------------------------------------------------------------

void BitmapWriteAccess::FillPolyPolygon( const PolyPolygon& rPolyPoly )
{
    const USHORT nCount = rPolyPoly.Count();

    if( nCount && mpFillColor )
    {
        const BitmapColor&  rFillColor = *mpFillColor;
        Region              aRegion( rPolyPoly );
        Rectangle           aRect;

        aRegion.Intersect( Rectangle( Point(), Size( Width(), Height() ) ) );

        if( !aRegion.IsEmpty() )
        {
            RegionHandle aRegHandle( aRegion.BeginEnumRects() );

            while( aRegion.GetNextEnumRect( aRegHandle, aRect ) )
                for( long nY = aRect.Top(), nEndY = aRect.Bottom(); nY <= nEndY; nY++ )
                    for( long nX = aRect.Left(), nEndX = aRect.Right(); nX <= nEndX; nX++ )
                        SetPixel( nY, nX, rFillColor );

            aRegion.EndEnumRects( aRegHandle );
        }
    }
}

// ------------------------------------------------------------------

void BitmapWriteAccess::DrawPolyPolygon( const PolyPolygon& rPolyPoly )
{
    if( mpFillColor )
        FillPolyPolygon( rPolyPoly );

    if( mpLineColor && ( !mpFillColor || ( *mpFillColor != *mpLineColor ) ) )
    {
        for( USHORT n = 0, nCount = rPolyPoly.Count(); n < nCount; )
        {
            const Polygon&  rPoly = rPolyPoly[ n++ ];
            const USHORT    nSize = rPoly.GetSize();

            if( nSize )
            {
                for( USHORT i = 0, nSize1 = nSize - 1; i < nSize1; i++ )
                    DrawLine( rPoly[ i ], rPoly[ i + 1 ] );

                if( rPoly[ nSize - 1 ] != rPoly[ 0 ] )
                    DrawLine( rPoly[ nSize - 1 ], rPoly[ 0 ] );
            }
        }
    }
}
