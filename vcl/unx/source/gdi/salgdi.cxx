/*************************************************************************
 *
 *  $RCSfile: salgdi.cxx,v $
 *
 *  $Revision: 1.34 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 15:17:05 $
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

#include <stdio.h>
#include <stdlib.h>

#include <prex.h>
// [ed] 6/15/02 There's a conflicting definition of INT8 within the Xmd.h header
// and the solar.h OOo header.  So, wrap the X11 header with a bogus #define
// to use the OOo definition of the symbol for INT8.
// [fa] 4/12/04 With 64-bit changes, X11 headers also conflict with INT64

#define INT8 blehBlahFooBarINT8
#define INT64 blehBlahFooBarINT64

#include <X11/Xproto.h>

#undef INT8
#undef INT64
#include <postx.h>

#include <salunx.h>

#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALDISP_HXX
#include <saldisp.hxx>
#endif
#ifndef _SV_SALGDI_HXX
#include <salgdi.h>
#endif
#ifndef _SV_SALFRAME_H
#include <salframe.h>
#endif
#ifndef _SV_SALVD_H
#include <salvd.h>
#endif
#include <tools/debug.hxx>

#ifndef _USE_PRINT_EXTENSION_
#include <psprint/printergfx.hxx>
#include <psprint/jobdata.hxx>
#endif

// -=-= SalPolyLine =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define STATIC_POINTS 64

class SalPolyLine
{
            XPoint              Points_[STATIC_POINTS];
            XPoint             *pFirst_;
public:
    inline                      SalPolyLine( ULONG nPoints );
    inline                      SalPolyLine( ULONG nPoints, const SalPoint *p );
    inline                      ~SalPolyLine();
    inline  XPoint             &operator [] ( ULONG n ) const
                                { return pFirst_[n]; }
};

inline SalPolyLine::SalPolyLine( ULONG nPoints )
    : pFirst_( nPoints+1 > STATIC_POINTS ? new XPoint[nPoints+1] : Points_ )
{}

inline SalPolyLine::SalPolyLine( ULONG nPoints, const SalPoint *p )
    : pFirst_( nPoints+1 > STATIC_POINTS ? new XPoint[nPoints+1] : Points_ )
{
    for( ULONG i = 0; i < nPoints; i++ )
    {
        pFirst_[i].x = (short)p[i].mnX;
        pFirst_[i].y = (short)p[i].mnY;
    }
    pFirst_[nPoints] = pFirst_[0]; // close polyline
}

inline SalPolyLine::~SalPolyLine()
{ if( pFirst_ != Points_ ) delete [] pFirst_; }

#undef STATIC_POINTS
// -=-= X11SalGraphics =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
X11SalGraphics::X11SalGraphics()
{
    m_pFrame            = NULL;
    m_pVDev             = NULL;
    m_pDeleteColormap   = NULL;
    hDrawable_          = None;

    pClipRegion_            = NULL;
    pPaintRegion_       = NULL;

    pPenGC_         = NULL;
    nPenPixel_          = 0;
    nPenColor_          = MAKE_SALCOLOR( 0x00, 0x00, 0x00 ); // Black

    pFontGC_            = NULL;
    for( int i = 0; i < MAX_FALLBACK; ++i )
    {
        mXFont[i]       = NULL;
        mpServerFont[i] = NULL;
    }

    nTextPixel_         = 0;
    nTextColor_         = MAKE_SALCOLOR( 0x00, 0x00, 0x00 ); // Black

    pBrushGC_           = NULL;
    nBrushPixel_            = 0;
    nBrushColor_        = MAKE_SALCOLOR( 0xFF, 0xFF, 0xFF ); // White
    hBrush_             = None;

    pMonoGC_            = NULL;
    pCopyGC_            = NULL;
    pMaskGC_            = NULL;
    pInvertGC_          = NULL;
    pInvert50GC_        = NULL;
    pStippleGC_         = NULL;
    pTrackingGC_        = NULL;

    bWindow_            = FALSE;
    bPrinter_           = FALSE;
    bVirDev_            = FALSE;
    bPenGC_         = FALSE;
    bFontGC_            = FALSE;
    bBrushGC_           = FALSE;
    bMonoGC_            = FALSE;
    bCopyGC_            = FALSE;
    bInvertGC_          = FALSE;
    bInvert50GC_        = FALSE;
    bStippleGC_         = FALSE;
    bTrackingGC_        = FALSE;
    bXORMode_           = FALSE;
    bDitherBrush_       = FALSE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
X11SalGraphics::~X11SalGraphics()
{
    Display *pDisplay = GetXDisplay();

    DBG_ASSERT( !pPaintRegion_, "pPaintRegion_" )
        if( pClipRegion_ ) XDestroyRegion( pClipRegion_ );

    if( hBrush_ )       XFreePixmap( pDisplay, hBrush_ );
    if( pPenGC_ )       XFreeGC( pDisplay, pPenGC_ );
    if( pFontGC_ )      XFreeGC( pDisplay, pFontGC_ );
    if( pBrushGC_ )     XFreeGC( pDisplay, pBrushGC_ );
    if( pMonoGC_ )      XFreeGC( pDisplay, pMonoGC_ );
    if( pCopyGC_ )      XFreeGC( pDisplay, pCopyGC_ );
    if( pMaskGC_ )      XFreeGC( pDisplay, pMaskGC_ );
    if( pInvertGC_ )    XFreeGC( pDisplay, pInvertGC_ );
    if( pInvert50GC_ )  XFreeGC( pDisplay, pInvert50GC_ );
    if( pStippleGC_ )   XFreeGC( pDisplay, pStippleGC_ );
    if( pTrackingGC_ )  XFreeGC( pDisplay, pTrackingGC_ );
    if( m_pDeleteColormap )
        delete m_pDeleteColormap;
}

// -=-= SalGraphics / X11SalGraphics =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void X11SalGraphics::Init( SalFrame *pFrame, Drawable aTarget )
{
    m_pColormap     = &GetSalData()->GetDisplay()->GetColormap();
    hDrawable_      = aTarget;

    bWindow_        = TRUE;
    m_pFrame        = pFrame;
    m_pVDev         = NULL;

    nPenPixel_      = GetPixel( nPenColor_ );
    nTextPixel_     = GetPixel( nTextColor_ );
    nBrushPixel_    = GetPixel( nBrushColor_ );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::DeInit()
{
    hDrawable_ = None;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetClipRegion( GC pGC, XLIB_Region pXReg ) const
{
    Display *pDisplay = GetXDisplay();

    int n = 0;
    XLIB_Region Regions[3];

    if( pClipRegion_ /* && !XEmptyRegion( pClipRegion_ ) */ )
        Regions[n++] = pClipRegion_;
//  if( pPaintRegion_ /* && !XEmptyRegion( pPaintRegion_ ) */ )
//      Regions[n++] = pPaintRegion_;

    if( pXReg && !XEmptyRegion( pXReg ) )
        Regions[n++] = pXReg;

    if( 0 == n )
        XSetClipMask( pDisplay, pGC, None );
    else if( 1 == n )
        XSetRegion( pDisplay, pGC, Regions[0] );
    else
    {
        XLIB_Region pTmpRegion = XCreateRegion();
        XIntersectRegion( Regions[0], Regions[1], pTmpRegion );
//      if( 3 == n )
//          XIntersectRegion( Regions[2], pTmpRegion, pTmpRegion );
        XSetRegion( pDisplay, pGC, pTmpRegion );
        XDestroyRegion( pTmpRegion );
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
GC X11SalGraphics::SelectPen()
{
    Display *pDisplay = GetXDisplay();

    DBG_ASSERT( nPenColor_ != 0xFFFFFFFF, "Pen Transparent" );

    if( !pPenGC_ )
    {
        XGCValues values;
        //values.subwindow_mode     = IncludeInferiors;
        values.subwindow_mode       = ClipByChildren;
        values.fill_rule            = EvenOddRule;      // Pict import/ Gradient
        values.graphics_exposures   = True;

        pPenGC_ = XCreateGC( pDisplay, hDrawable_,
                             GCSubwindowMode | GCFillRule | GCGraphicsExposures,
                             &values );
    }

    if( !bPenGC_ )
    {
        XSetForeground( pDisplay, pPenGC_, nPenPixel_ );
        XSetFunction  ( pDisplay, pPenGC_, bXORMode_ ? GXxor : GXcopy );
        SetClipRegion( pPenGC_ );
        bPenGC_ = TRUE;
    }

    return pPenGC_;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
GC X11SalGraphics::SelectBrush()
{
    Display *pDisplay = GetXDisplay();

    DBG_ASSERT( nBrushColor_ != 0xFFFFFFFF, "Brush Transparent" );

    if( !pBrushGC_ )
    {
        XGCValues values;
        // values.subwindow_mode        = IncludeInferiors;
        values.subwindow_mode       = ClipByChildren;
        values.fill_rule            = EvenOddRule;      // Pict import/ Gradient
        values.graphics_exposures   = True;

        pBrushGC_ = XCreateGC( pDisplay, hDrawable_,
                               GCSubwindowMode | GCFillRule | GCGraphicsExposures,
                               &values );
    }

    if( !bBrushGC_ )
    {
        if( !bDitherBrush_ )
        {
            XSetFillStyle ( pDisplay, pBrushGC_, FillSolid );
            XSetForeground( pDisplay, pBrushGC_, nBrushPixel_ );
                        #if defined(_USE_PRINT_EXTENSION_)
                        XSetBackground( pDisplay, pBrushGC_,
                                        WhitePixel(pDisplay, DefaultScreen(pDisplay)) );
                        #else
            if( bPrinter_ )
                XSetTile( pDisplay, pBrushGC_, None );
            #endif
        }
        else
        {
            // Bug in Sun Solaris 2.5.1, XFillPolygon doesn't allways reflect
            // changes of the tile. PROPERTY_BUG_Tile doesn't fix this !
            if (GetDisplay()->GetProperties() & PROPERTY_BUG_FillPolygon_Tile)
                XSetFillStyle ( pDisplay, pBrushGC_, FillSolid );

            XSetFillStyle ( pDisplay, pBrushGC_, FillTiled );
            XSetTile      ( pDisplay, pBrushGC_, hBrush_ );
        }
        XSetFunction  ( pDisplay, pBrushGC_, bXORMode_ ? GXxor : GXcopy );
        SetClipRegion( pBrushGC_ );

        bBrushGC_ = TRUE;
    }

    return pBrushGC_;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
GC X11SalGraphics::GetTrackingGC()
{
    const char    dash_list[2] = {2, 2};

    if( !pTrackingGC_ )
    {
        XGCValues     values;

        values.graphics_exposures   = True;
        values.foreground           = m_pColormap->GetBlackPixel()
                                      ^ m_pColormap->GetWhitePixel();
        values.function             = GXxor;
        values.line_width           = 1;
        values.line_style           = LineOnOffDash;

        pTrackingGC_ = XCreateGC( GetXDisplay(), GetDrawable(),
                                  GCGraphicsExposures | GCForeground | GCFunction
                                  | GCLineWidth | GCLineStyle,
                                  &values );
        XSetDashes( GetXDisplay(), pTrackingGC_, 0, dash_list, 2 );
    }

    if( !bTrackingGC_ )
    {
        SetClipRegion( pTrackingGC_ );
        bTrackingGC_ = TRUE;
    }

    return pTrackingGC_;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::DrawLines( ULONG              nPoints,
                                       const SalPolyLine &rPoints,
                                       GC                 pGC )
{
    // errechne wie viele Linien XWindow auf einmal zeichnen kann
    ULONG nMaxLines = (GetDisplay()->GetMaxRequestSize() - sizeof(xPolyPointReq))
                      / sizeof(xPoint);
    if( nMaxLines > nPoints ) nMaxLines = nPoints;

    // gebe alle Linien aus, die XWindows zeichnen kann.
    ULONG n;
    for( n = 0; nPoints - n > nMaxLines; n += nMaxLines - 1 )
        XDrawLines( GetXDisplay(),
                    GetDrawable(),
                    pGC,
                    &rPoints[n],
                    nMaxLines,
                    CoordModeOrigin );

    if( n < nPoints )
        XDrawLines( GetXDisplay(),
                    GetDrawable(),
                    pGC,
                    &rPoints[n],
                    nPoints - n,
                    CoordModeOrigin );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Dithern: Calculate a dither-pixmap and make a brush of it
#define P_DELTA         51
#define DMAP( v, m )    ((v % P_DELTA) > m ? (v / P_DELTA) + 1 : (v / P_DELTA))

BOOL X11SalGraphics::GetDitherPixmap( SalColor nSalColor )
{
    static const short nOrdDither8Bit[ 8 ][ 8 ] =
    {
         0, 38,  9, 48,  2, 40, 12, 50,
        25, 12, 35, 22, 28, 15, 37, 24,
         6, 44,  3, 41,  8, 47,  5, 44,
        32, 19, 28, 16, 34, 21, 31, 18,
         1, 40, 11, 49,  0, 39, 10, 48,
        27, 14, 36, 24, 26, 13, 36, 23,
         8, 46,  4, 43,  7, 45,  4, 42,
        33, 20, 30, 17, 32, 20, 29, 16
    };

    // test for correct depth (8bit)
    if( GetColormap().GetVisual()->GetDepth() != 8 )
        return FALSE;

    char    pBits[64];
    char   *pBitsPtr = pBits;

    // Set the pallette-entries for the dithering tile
    UINT8 nSalColorRed   = SALCOLOR_RED   ( nSalColor );
    UINT8 nSalColorGreen = SALCOLOR_GREEN ( nSalColor );
    UINT8 nSalColorBlue  = SALCOLOR_BLUE  ( nSalColor );

    for( int nY = 0; nY < 8; nY++ )
    {
        for( int nX = 0; nX < 8; nX++ )
        {
            short nMagic = nOrdDither8Bit[nY][nX];
            UINT8 nR   = P_DELTA * DMAP( nSalColorRed,   nMagic );
            UINT8 nG   = P_DELTA * DMAP( nSalColorGreen, nMagic );
            UINT8 nB   = P_DELTA * DMAP( nSalColorBlue,  nMagic );

            *pBitsPtr++ = GetColormap().GetPixel( MAKE_SALCOLOR( nR, nG, nB ) );
        }
    }

    // create the tile as ximage and an according pixmap -> caching
    XImage *pImage = XCreateImage( GetXDisplay(),
                                   GetColormap().GetXVisual(),
                                   8,
                                   ZPixmap,
                                   0,               // offset
                                   pBits,           // data
                                   8, 8,            // width & height
                                   8,               // bitmap_pad
                                   0 );             // (default) bytes_per_line

    if ( GetDisplay()->GetProperties() & PROPERTY_BUG_Tile )
    {
        if (hBrush_)
            XFreePixmap (GetXDisplay(), hBrush_);
        hBrush_ = XCreatePixmap( GetXDisplay(), GetDrawable(), 8, 8, 8 );
    }
    else
    if( !hBrush_ )
        hBrush_ = XCreatePixmap( GetXDisplay(), GetDrawable(), 8, 8, 8 );

    // put the ximage to the pixmap
    XPutImage( GetXDisplay(),
               hBrush_,
               GetDisplay()->GetCopyGC(),
               pImage,
               0, 0,                        // Source
               0, 0,                        // Destination
               8, 8 );                      // width & height

    // destroy image-frame but not palette-data
    pImage->data = NULL;
    XDestroyImage( pImage );

    return TRUE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::GetResolution( sal_Int32 &rDPIX, sal_Int32 &rDPIY ) // const
{
    SalDisplay *pDisplay = GetDisplay();

    rDPIX = pDisplay->GetResolution().A();
    rDPIY = pDisplay->GetResolution().B();
    if( !pDisplay->GetExactResolution() && rDPIY < 96 )
    {
        rDPIX = Divide( rDPIX * 96, rDPIY );
            rDPIY = 96;
    }
    else if ( rDPIY > 200 )
    {
        rDPIX = Divide( rDPIX * 200, rDPIY );
        rDPIY = 200;
    }

    // #i12705# equalize x- and y-resolution if they are close enough
    if( rDPIX != rDPIY )
    {
        // different x- and y- resolutions are usually artifacts of
        // a wrongly calculated screen size.
        //if( (13*rDPIX >= 10*rDPIY) && (13*rDPIY >= 10*rDPIX) )  //+-30%
        {
#ifdef DEBUG
            printf("Forcing Resolution from %ldx%ld to %ldx%ld\n",
                rDPIX,rDPIY,rDPIY,rDPIY);
#endif
            rDPIX = rDPIY; // y-resolution is more trustworthy
        }
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::GetScreenFontResolution( sal_Int32 &rDPIX, sal_Int32 &rDPIY ) // const
{
    GetDisplay()->GetScreenFontResolution( rDPIX, rDPIY );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
USHORT X11SalGraphics::GetBitCount() // const
{
    return GetVisual()->GetDepth();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long X11SalGraphics::GetGraphicsWidth()
{
    if( m_pFrame )
        return m_pFrame->maGeometry.nWidth;
    else if( m_pVDev )
        return m_pVDev->GetWidth();
    else
        return 0;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::ResetClipRegion()
{
    if( pClipRegion_ )
    {
        bPenGC_         = FALSE;
        bFontGC_        = FALSE;
        bBrushGC_       = FALSE;
        bMonoGC_        = FALSE;
        bCopyGC_        = FALSE;
        bInvertGC_      = FALSE;
        bInvert50GC_    = FALSE;
        bStippleGC_     = FALSE;
        bTrackingGC_    = FALSE;

        XDestroyRegion( pClipRegion_ );
        pClipRegion_    = NULL;
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::BeginSetClipRegion( ULONG n )
{
    if( pClipRegion_ )
        XDestroyRegion( pClipRegion_ );
    pClipRegion_ = XCreateRegion();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
BOOL X11SalGraphics::unionClipRegion( long nX, long nY, long nDX, long nDY )
{
    if (!nDX || !nDY)
        return TRUE;

    XRectangle aRect;
    aRect.x         = (short)nX;
    aRect.y         = (short)nY;
    aRect.width     = (unsigned short)nDX;
    aRect.height    = (unsigned short)nDY;

    XUnionRectWithRegion( &aRect, pClipRegion_, pClipRegion_ );

    return TRUE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::EndSetClipRegion()
{
    bPenGC_         = FALSE;
    bFontGC_        = FALSE;
    bBrushGC_       = FALSE;
    bMonoGC_        = FALSE;
    bCopyGC_        = FALSE;
    bInvertGC_      = FALSE;
    bInvert50GC_    = FALSE;
    bStippleGC_     = FALSE;
    bTrackingGC_    = FALSE;

    if( XEmptyRegion( pClipRegion_ ) )
    {
        XDestroyRegion( pClipRegion_ );
        pClipRegion_= NULL;
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetLineColor()
{
    if( nPenColor_ != 0xFFFFFFFF )
    {
        nPenColor_      = 0xFFFFFFFF;
        bPenGC_         = FALSE;
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetLineColor( SalColor nSalColor )
{
    if( nPenColor_ != nSalColor )
    {
        nPenColor_      = nSalColor;
        nPenPixel_      = GetPixel( nSalColor );
        bPenGC_         = FALSE;
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetFillColor()
{
    if( nBrushColor_ != 0xFFFFFFFF )
    {
        bDitherBrush_   = FALSE;
        nBrushColor_    = 0xFFFFFFFF;
        bBrushGC_       = FALSE;
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetFillColor( SalColor nSalColor )
{
    if( nBrushColor_ != nSalColor )
    {
        bDitherBrush_   = FALSE;
        nBrushColor_    = nSalColor;
        nBrushPixel_    = GetPixel( nSalColor );
        if( TrueColor != GetColormap().GetVisual()->GetClass()
            && GetColormap().GetColor( nBrushPixel_ ) != nBrushColor_
            && nSalColor != MAKE_SALCOLOR( 0x00, 0x00, 0x00 ) // black
            && nSalColor != MAKE_SALCOLOR( 0x00, 0x00, 0x80 ) // blue
            && nSalColor != MAKE_SALCOLOR( 0x00, 0x80, 0x00 ) // green
            && nSalColor != MAKE_SALCOLOR( 0x00, 0x80, 0x80 ) // cyan
            && nSalColor != MAKE_SALCOLOR( 0x80, 0x00, 0x00 ) // red
            && nSalColor != MAKE_SALCOLOR( 0x80, 0x00, 0x80 ) // magenta
            && nSalColor != MAKE_SALCOLOR( 0x80, 0x80, 0x00 ) // brown
            && nSalColor != MAKE_SALCOLOR( 0x80, 0x80, 0x80 ) // gray
            && nSalColor != MAKE_SALCOLOR( 0xC0, 0xC0, 0xC0 ) // light gray
            && nSalColor != MAKE_SALCOLOR( 0x00, 0x00, 0xFF ) // light blue
            && nSalColor != MAKE_SALCOLOR( 0x00, 0xFF, 0x00 ) // light green
            && nSalColor != MAKE_SALCOLOR( 0x00, 0xFF, 0xFF ) // light cyan
            && nSalColor != MAKE_SALCOLOR( 0xFF, 0x00, 0x00 ) // light red
            && nSalColor != MAKE_SALCOLOR( 0xFF, 0x00, 0xFF ) // light magenta
            && nSalColor != MAKE_SALCOLOR( 0xFF, 0xFF, 0x00 ) // light brown
            && nSalColor != MAKE_SALCOLOR( 0xFF, 0xFF, 0xFF ) )
            bDitherBrush_ = GetDitherPixmap(nSalColor);
        bBrushGC_       = FALSE;
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetROPLineColor( SalROPColor nROPColor )
{
    switch( nROPColor )
    {
        case SAL_ROP_0 : // 0
            nPenPixel_ = (Pixel)0;
            break;
        case SAL_ROP_1 : // 1
            nPenPixel_ = (Pixel)(1 << GetVisual()->GetDepth()) - 1;
            break;
        case SAL_ROP_INVERT : // 2
            nPenPixel_ = (Pixel)(1 << GetVisual()->GetDepth()) - 1;
            break;
    }
    nPenColor_  = GetColormap().GetColor( nPenPixel_ );
    bPenGC_     = FALSE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetROPFillColor( SalROPColor nROPColor )
{
    switch( nROPColor )
    {
        case SAL_ROP_0 : // 0
            nBrushPixel_ = (Pixel)0;
            break;
        case SAL_ROP_1 : // 1
            nBrushPixel_ = (Pixel)(1 << GetVisual()->GetDepth()) - 1;
            break;
        case SAL_ROP_INVERT : // 2
            nBrushPixel_ = (Pixel)(1 << GetVisual()->GetDepth()) - 1;
            break;
    }
    bDitherBrush_   = FALSE;
    nBrushColor_    = GetColormap().GetColor( nBrushPixel_ );
    bBrushGC_       = FALSE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetXORMode( BOOL bSet )
{
    if( !bXORMode_ == bSet )
    {
        bXORMode_   = bSet;
        bPenGC_     = FALSE;
        bBrushGC_   = FALSE;
        bMonoGC_        = FALSE;
        bCopyGC_        = FALSE;
        bInvertGC_  = FALSE;
        bInvert50GC_    = FALSE;
        bStippleGC_ = FALSE;
        bTrackingGC_    = FALSE;
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPixel( long nX, long nY )
{
    if( nPenColor_ !=  0xFFFFFFFF )
        XDrawPoint( GetXDisplay(), GetDrawable(), SelectPen(), nX, nY );
}

void X11SalGraphics::drawPixel( long nX, long nY, SalColor nSalColor )
{
    if( nSalColor != 0xFFFFFFFF )
    {
        Display *pDisplay = GetXDisplay();

        if( nPenColor_ == 0xFFFFFFFF && !bPenGC_ )
        {
            SetLineColor( nSalColor );
            XDrawPoint( pDisplay, GetDrawable(), SelectPen(), nX, nY );
            nPenColor_ = 0xFFFFFFFF;
            bPenGC_ = False;
        }
        else
        {
            GC pGC = SelectPen();

            if( nSalColor != nPenColor_ )
                XSetForeground( pDisplay, pGC, GetPixel( nSalColor ) );

            XDrawPoint( pDisplay, GetDrawable(), pGC, nX, nY );

            if( nSalColor != nPenColor_ )
                XSetForeground( pDisplay, pGC, nPenPixel_ );
        }
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawLine( long nX1, long nY1, long nX2, long nY2 )
{
    if( nPenColor_ != 0xFFFFFFFF )
    {
        if ( GetDisplay()->GetProperties() & PROPERTY_BUG_DrawLine )
        {
            GC aGC = SelectPen();
            XDrawPoint (GetXDisplay(), GetDrawable(), aGC, (int)nX1, (int)nY1);
            XDrawPoint (GetXDisplay(), GetDrawable(), aGC, (int)nX2, (int)nY2);
            XDrawLine  (GetXDisplay(), GetDrawable(), aGC, nX1, nY1, nX2, nY2 );
        }
        else
            XDrawLine( GetXDisplay(), GetDrawable(),SelectPen(),
                       nX1, nY1, nX2, nY2 );
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawRect( long nX, long nY, long nDX, long nDY )
{
    if( nBrushColor_ != 0xFFFFFFFF )
    {
        XFillRectangle( GetXDisplay(),
                        GetDrawable(),
                        SelectBrush(),
                        nX, nY, nDX, nDY );
    }
    // Beschreibung DrawRect verkehrt, deshalb -1
    if( nPenColor_ != 0xFFFFFFFF )
        XDrawRectangle( GetXDisplay(),
                        GetDrawable(),
                        SelectPen(),
                        nX, nY, nDX-1, nDY-1 );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPolyLine( ULONG nPoints, const SalPoint *pPtAry )
{
    if( nPenColor_ != 0xFFFFFFFF )
    {
        SalPolyLine Points( nPoints, pPtAry );

        DrawLines( nPoints, Points, SelectPen() );
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPolygon( ULONG nPoints, const SalPoint* pPtAry )
{
    if( nPoints < 3 )
    {
        if( !nPoints )
        {
#if OSL_DEBUG_LEVEL > 1
            fprintf( stderr, "X11SalGraphicsDrawPolygon !nPoints\n" );
#endif
        }
        else if( !bXORMode_ )
        {
            if( 1 == nPoints  )
                drawPixel( pPtAry[0].mnX, pPtAry[0].mnY );
            else
                drawLine( pPtAry[0].mnX, pPtAry[0].mnY,
                          pPtAry[1].mnX, pPtAry[1].mnY );
        }
        return;
    }

    SalPolyLine Points( nPoints, pPtAry );

    nPoints++;

    if( nBrushColor_ != 0xFFFFFFFF )
        XFillPolygon( GetXDisplay(),
                      GetDrawable(),
                      SelectBrush(),
                      &Points[0], nPoints,
                      Complex, CoordModeOrigin );

    if( nPenColor_ != 0xFFFFFFFF )
        DrawLines( nPoints, Points, SelectPen() );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPolyPolygon( ULONG         nPoly,
                                   const ULONG     *pPoints,
                                   PCONSTSALPOINT  *pPtAry )
{
    if( nBrushColor_ != 0xFFFFFFFF )
    {
        ULONG       i, n;
        XLIB_Region pXRegA  = NULL;

        for( i = 0; i < nPoly; i++ ) {
            n = pPoints[i];
            SalPolyLine Points( n, pPtAry[i] );
            if( n > 2 )
            {
                XLIB_Region pXRegB = XPolygonRegion( &Points[0], n+1, WindingRule );
                if( !pXRegA )
                    pXRegA = pXRegB;
                else
                {
                    XXorRegion( pXRegA, pXRegB, pXRegA );
                    XDestroyRegion( pXRegB );
                }
            }
        }

        if( pXRegA )
        {
            XRectangle aXRect;
            XClipBox( pXRegA, &aXRect );

            GC pGC = SelectBrush();
            SetClipRegion( pGC, pXRegA ); // ??? doppelt
            XDestroyRegion( pXRegA );
            bBrushGC_ = FALSE;

            XFillRectangle( GetXDisplay(),
                            GetDrawable(),
                            pGC,
                            aXRect.x, aXRect.y, aXRect.width, aXRect.height );
        }
   }

   if( nPenColor_ != 0xFFFFFFFF )
       for( ULONG i = 0; i < nPoly; i++ )
           drawPolyLine( pPoints[i], pPtAry[i] );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

sal_Bool X11SalGraphics::drawPolyLineBezier( ULONG nPoints, const SalPoint* pPtAry, const BYTE* pFlgAry )
{
    return sal_False;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

sal_Bool X11SalGraphics::drawPolygonBezier( ULONG nPoints, const SalPoint* pPtAry, const BYTE* pFlgAry )
{
    return sal_False;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

sal_Bool X11SalGraphics::drawPolyPolygonBezier( ULONG nPoly, const ULONG* pPoints,
                                                const SalPoint* const* pPtAry, const BYTE* const* pFlgAry )
{
    return sal_False;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void X11SalGraphics::invert( ULONG nPoints,
                             const SalPoint* pPtAry,
                             SalInvert nFlags )
{
    SalPolyLine Points ( nPoints, pPtAry );

    GC pGC;
    if( SAL_INVERT_50 & nFlags )
        pGC = GetInvert50GC();
    else
        if ( SAL_INVERT_TRACKFRAME & nFlags )
            pGC = GetTrackingGC();
        else
            pGC = GetInvertGC();

    if( SAL_INVERT_TRACKFRAME & nFlags )
        DrawLines ( nPoints, Points, pGC );
    else
        XFillPolygon( GetXDisplay(),
                      GetDrawable(),
                      pGC,
                      &Points[0], nPoints,
                      Complex, CoordModeOrigin );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

BOOL X11SalGraphics::drawEPS( long nX, long nY, long nWidth, long nHeight, void* pPtr, ULONG nSize )
{
    return FALSE;
}
