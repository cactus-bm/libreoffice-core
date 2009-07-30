/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
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
#include "precompiled_vcl.hxx"

#include "Xproto.h"

#include "salunx.h"
#include "saldata.hxx"
#include "saldisp.hxx"
#include "salgdi.h"
#include "salframe.h"
#include "salvd.h"
#include "xrender_peer.hxx"

#include "vcl/printergfx.hxx"
#include "vcl/jobdata.hxx"

#include "tools/debug.hxx"

#include "basegfx/polygon/b2dpolygon.hxx"
#include "basegfx/polygon/b2dpolypolygon.hxx"
#include "basegfx/polygon/b2dpolypolygontools.hxx"
#include "basegfx/polygon/b2dpolygontools.hxx"
#include "basegfx/polygon/b2dpolygonclipper.hxx"
#include "basegfx/polygon/b2dlinegeometry.hxx"
#include "basegfx/matrix/b2dhommatrix.hxx"
#include "basegfx/polygon/b2dpolypolygoncutter.hxx"

#include <vector>
#include <queue>
#include <set>

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
    m_aRenderPicture    = 0;
    m_pRenderFormat     = NULL;

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
    ReleaseFonts();
    freeResources();
}

// -=-= SalGraphics / X11SalGraphics =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void X11SalGraphics::freeResources()
{
    Display *pDisplay = GetXDisplay();

    DBG_ASSERT( !pPaintRegion_, "pPaintRegion_" );
    if( pClipRegion_ ) XDestroyRegion( pClipRegion_ ), pClipRegion_ = None;

    if( hBrush_ )       XFreePixmap( pDisplay, hBrush_ ), hBrush_ = None;
    if( pPenGC_ )       XFreeGC( pDisplay, pPenGC_ ), pPenGC_ = None;
    if( pFontGC_ )      XFreeGC( pDisplay, pFontGC_ ), pFontGC_ = None;
    if( pBrushGC_ )     XFreeGC( pDisplay, pBrushGC_ ), pBrushGC_ = None;
    if( pMonoGC_ )      XFreeGC( pDisplay, pMonoGC_ ), pMonoGC_ = None;
    if( pCopyGC_ )      XFreeGC( pDisplay, pCopyGC_ ), pCopyGC_ = None;
    if( pMaskGC_ )      XFreeGC( pDisplay, pMaskGC_ ), pMaskGC_ = None;
    if( pInvertGC_ )    XFreeGC( pDisplay, pInvertGC_ ), pInvertGC_ = None;
    if( pInvert50GC_ )  XFreeGC( pDisplay, pInvert50GC_ ), pInvert50GC_ = None;
    if( pStippleGC_ )   XFreeGC( pDisplay, pStippleGC_ ), pStippleGC_ = None;
    if( pTrackingGC_ )  XFreeGC( pDisplay, pTrackingGC_ ), pTrackingGC_ = None;
    if( m_pDeleteColormap )
        delete m_pDeleteColormap, m_pColormap = m_pDeleteColormap = NULL;

    if( m_aRenderPicture )
        XRenderPeer::GetInstance().FreePicture( m_aRenderPicture ), m_aRenderPicture = 0;

    bPenGC_ = bFontGC_ = bBrushGC_ = bMonoGC_ = bCopyGC_ = bInvertGC_ = bInvert50GC_ = bStippleGC_ = bTrackingGC_ = false;
}

void X11SalGraphics::SetDrawable( Drawable aDrawable, int nScreen )
{
    // shortcut if nothing changed
    if( hDrawable_ == aDrawable )
        return;

    // free screen specific resources if needed
    if( nScreen != m_nScreen )
    {
        freeResources();
        m_pColormap = &GetX11SalData()->GetDisplay()->GetColormap( nScreen );
        m_nScreen = nScreen;
    }

    hDrawable_ = aDrawable;
    SetXRenderFormat( NULL );
    if( m_aRenderPicture )
    {
        XRenderPeer::GetInstance().FreePicture( m_aRenderPicture );
        m_aRenderPicture = 0;
    }

    if( hDrawable_ )
    {
        nPenPixel_      = GetPixel( nPenColor_ );
        nTextPixel_     = GetPixel( nTextColor_ );
        nBrushPixel_    = GetPixel( nBrushColor_ );
    }
}

void X11SalGraphics::Init( SalFrame *pFrame, Drawable aTarget, int nScreen )
{
#if 0 // TODO: use SetDrawable() instead
    m_pColormap     = &GetX11SalData()->GetDisplay()->GetColormap(nScreen);
    hDrawable_      = aTarget;
    m_nScreen       = nScreen;
    SetXRenderFormat( NULL );
    if( m_aRenderPicture )
        XRenderPeer::GetInstance().FreePicture( m_aRenderPicture ), m_aRenderPicture = 0;

    nPenPixel_      = GetPixel( nPenColor_ );
    nTextPixel_     = GetPixel( nTextColor_ );
    nBrushPixel_    = GetPixel( nBrushColor_ );
#else
    m_pColormap     = &GetX11SalData()->GetDisplay()->GetColormap(nScreen);
    m_nScreen = nScreen;
    SetDrawable( aTarget, nScreen );
#endif

    bWindow_        = TRUE;
    m_pFrame        = pFrame;
    m_pVDev         = NULL;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::DeInit()
{
    SetDrawable( None, m_nScreen );
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

    if( !pPenGC_ )
    {
        XGCValues values;
        values.subwindow_mode       = ClipByChildren;
        values.fill_rule            = EvenOddRule;      // Pict import/ Gradient
        values.graphics_exposures   = False;

        pPenGC_ = XCreateGC( pDisplay, hDrawable_,
                             GCSubwindowMode | GCFillRule | GCGraphicsExposures,
                             &values );
    }

    if( !bPenGC_ )
    {
        if( nPenColor_ != SALCOLOR_NONE )
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

    DBG_ASSERT( nBrushColor_ != SALCOLOR_NONE, "Brush Transparent" );

    if( !pBrushGC_ )
    {
        XGCValues values;
        // values.subwindow_mode        = IncludeInferiors;
        values.subwindow_mode       = ClipByChildren;
        values.fill_rule            = EvenOddRule;      // Pict import/ Gradient
        values.graphics_exposures   = False;

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

        values.graphics_exposures   = False;
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
                                GC                 pGC,
                                bool               bClose
                                )
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
    if( bClose )
    {
        if( rPoints[nPoints-1].x != rPoints[0].x || rPoints[nPoints-1].y != rPoints[0].y )
            drawLine( rPoints[nPoints-1].x, rPoints[nPoints-1].y, rPoints[0].x, rPoints[0].y );
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Dithern: Calculate a dither-pixmap and make a brush of it
#define P_DELTA         51
#define DMAP( v, m )    ((v % P_DELTA) > m ? (v / P_DELTA) + 1 : (v / P_DELTA))

BOOL X11SalGraphics::GetDitherPixmap( SalColor nSalColor )
{
    static const short nOrdDither8Bit[ 8 ][ 8 ] =
    {
        { 0, 38,  9, 48,  2, 40, 12, 50},
        {25, 12, 35, 22, 28, 15, 37, 24},
        { 6, 44,  3, 41,  8, 47,  5, 44},
        {32, 19, 28, 16, 34, 21, 31, 18},
        { 1, 40, 11, 49,  0, 39, 10, 48},
        {27, 14, 36, 24, 26, 13, 36, 23},
        { 8, 46,  4, 43,  7, 45,  4, 42},
        {33, 20, 30, 17, 32, 20, 29, 16}
    };

    // test for correct depth (8bit)
    if( GetColormap().GetVisual().GetDepth() != 8 )
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
               GetDisplay()->GetCopyGC( m_nScreen ),
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
    const SalDisplay *pDisplay = GetDisplay();

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
            printf("Forcing Resolution from %" SAL_PRIdINT32 "x%" SAL_PRIdINT32 " to %" SAL_PRIdINT32 "x%" SAL_PRIdINT32 "\n",
                    rDPIX,rDPIY,rDPIY,rDPIY);
#endif
            rDPIX = rDPIY; // y-resolution is more trustworthy
        }
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
USHORT X11SalGraphics::GetBitCount() // const
{
    return GetVisual().GetDepth();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long X11SalGraphics::GetGraphicsWidth() const
{
    if( m_pFrame )
        return m_pFrame->maGeometry.nWidth;
    else if( m_pVDev )
        return m_pVDev->GetWidth();
    else
        return 0;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long X11SalGraphics::GetGraphicsHeight() const
{
    if( m_pFrame )
        return m_pFrame->maGeometry.nHeight;
    else if( m_pVDev )
        return m_pVDev->GetHeight();
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
void X11SalGraphics::BeginSetClipRegion( ULONG )
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
bool X11SalGraphics::unionClipRegion( const ::basegfx::B2DPolyPolygon& )
{
        // TODO: implement and advertise OutDevSupport_B2DClip support
        return false;
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
    if( nPenColor_ != SALCOLOR_NONE )
    {
        nPenColor_      = SALCOLOR_NONE;
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
    if( nBrushColor_ != SALCOLOR_NONE )
    {
        bDitherBrush_   = FALSE;
        nBrushColor_    = SALCOLOR_NONE;
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
        if( TrueColor != GetColormap().GetVisual().GetClass()
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
            nPenPixel_ = (Pixel)(1 << GetVisual().GetDepth()) - 1;
            break;
        case SAL_ROP_INVERT : // 2
            nPenPixel_ = (Pixel)(1 << GetVisual().GetDepth()) - 1;
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
            nBrushPixel_ = (Pixel)(1 << GetVisual().GetDepth()) - 1;
            break;
        case SAL_ROP_INVERT : // 2
            nBrushPixel_ = (Pixel)(1 << GetVisual().GetDepth()) - 1;
            break;
    }
    bDitherBrush_   = FALSE;
    nBrushColor_    = GetColormap().GetColor( nBrushPixel_ );
    bBrushGC_       = FALSE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::SetXORMode( bool bSet, bool )
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
    if( nPenColor_ !=  SALCOLOR_NONE )
        XDrawPoint( GetXDisplay(), GetDrawable(), SelectPen(), nX, nY );
}

void X11SalGraphics::drawPixel( long nX, long nY, SalColor nSalColor )
{
    if( nSalColor != SALCOLOR_NONE )
    {
        Display *pDisplay = GetXDisplay();

        if( (nPenColor_ == SALCOLOR_NONE) && !bPenGC_ )
        {
            SetLineColor( nSalColor );
            XDrawPoint( pDisplay, GetDrawable(), SelectPen(), nX, nY );
            nPenColor_ = SALCOLOR_NONE;
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
    if( nPenColor_ != SALCOLOR_NONE )
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
    if( nBrushColor_ != SALCOLOR_NONE )
    {
        XFillRectangle( GetXDisplay(),
                        GetDrawable(),
                        SelectBrush(),
                        nX, nY, nDX, nDY );
    }
    // Beschreibung DrawRect verkehrt, deshalb -1
    if( nPenColor_ != SALCOLOR_NONE )
        XDrawRectangle( GetXDisplay(),
                        GetDrawable(),
                        SelectPen(),
                        nX, nY, nDX-1, nDY-1 );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPolyLine( ULONG nPoints, const SalPoint *pPtAry )
{
    drawPolyLine( nPoints, pPtAry, false );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPolyLine( ULONG nPoints, const SalPoint *pPtAry, bool bClose )
{
    if( nPenColor_ != 0xFFFFFFFF )
    {
        SalPolyLine Points( nPoints, pPtAry );

        DrawLines( nPoints, Points, SelectPen(), bClose );
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPolygon( ULONG nPoints, const SalPoint* pPtAry )
{
    if( nPoints == 0 )
        return;

    if( nPoints < 3 )
    {
        if( !bXORMode_ )
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

    /* WORKAROUND: some Xservers (Xorg, VIA chipset in this case)
     * do not draw the visible part of a polygon
     * if it overlaps to the left of screen 0,y.
     * This happens to be the case in the gradient drawn in the
     * menubar background. workaround for the special case of
     * of a rectangle overlapping to the left.
     */
    if( nPoints == 5 &&
    Points[ 0 ].x == Points[ 1 ].x &&
        Points[ 1 ].y == Points[ 2 ].y &&
        Points[ 2 ].x == Points[ 3 ].x &&
        Points[ 0 ].x == Points[ 4 ].x && Points[ 0 ].y == Points[ 4 ].y
       )
    {
        bool bLeft = false;
        bool bRight = false;
        for(unsigned int i = 0; i < nPoints; i++ )
    {
            if( Points[i].x < 0 )
                bLeft = true;
            else
                bRight= true;
    }
    if( bLeft && ! bRight )
        return;
    if( bLeft && bRight )
        {
            for( unsigned int i = 0; i < nPoints; i++ )
                if( Points[i].x < 0 )
                    Points[i].x = 0;
        }
    }

    if( nBrushColor_ != SALCOLOR_NONE )
        XFillPolygon( GetXDisplay(),
                      GetDrawable(),
                      SelectBrush(),
                      &Points[0], nPoints,
                      Complex, CoordModeOrigin );

    if( nPenColor_ != 0xFFFFFFFF )
        DrawLines( nPoints, Points, SelectPen(), true );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void X11SalGraphics::drawPolyPolygon( sal_uInt32        nPoly,
                                   const sal_uInt32    *pPoints,
                                   PCONSTSALPOINT  *pPtAry )
{
    if( nBrushColor_ != SALCOLOR_NONE )
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

   if( nPenColor_ != SALCOLOR_NONE )
       for( ULONG i = 0; i < nPoly; i++ )
           drawPolyLine( pPoints[i], pPtAry[i], true );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

sal_Bool X11SalGraphics::drawPolyLineBezier( ULONG, const SalPoint*, const BYTE* )
{
    return sal_False;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

sal_Bool X11SalGraphics::drawPolygonBezier( ULONG, const SalPoint*, const BYTE* )
{
    return sal_False;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

sal_Bool X11SalGraphics::drawPolyPolygonBezier( sal_uInt32, const sal_uInt32*,
                                                const SalPoint* const*, const BYTE* const* )
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
        DrawLines ( nPoints, Points, pGC, true );
    else
        XFillPolygon( GetXDisplay(),
                      GetDrawable(),
                      pGC,
                      &Points[0], nPoints,
                      Complex, CoordModeOrigin );
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

BOOL X11SalGraphics::drawEPS( long,long,long,long,void*,ULONG )
{
    return FALSE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

XID X11SalGraphics::GetXRenderPicture()
{
    if( !m_aRenderPicture )
    {
        // check xrender support for matching visual
        // find a XRenderPictFormat compatible with the Drawable
        XRenderPeer& rRenderPeer = XRenderPeer::GetInstance();
        XRenderPictFormat* pVisualFormat = static_cast<XRenderPictFormat*>(GetXRenderFormat());
        if( !pVisualFormat )
        {
            Visual* pVisual = GetDisplay()->GetVisual( m_nScreen ).GetVisual();
            pVisualFormat = rRenderPeer.FindVisualFormat( pVisual );
            if( !pVisualFormat )
                return 0;
            // cache the XRenderPictFormat
            SetXRenderFormat( static_cast<void*>(pVisualFormat) );
        }

        // get the matching xrender target for drawable
        m_aRenderPicture = rRenderPeer.CreatePicture( hDrawable_, pVisualFormat, 0, NULL );
    }

#if 0
    // setup clipping so the callers don't have to do it themselves
    // TODO: avoid clipping if already set correctly
    if( pClipRegion_ && !XEmptyRegion( pClipRegion_ ) )
        rRenderPeer.SetPictureClipRegion( aDstPic, pClipRegion_ );
#endif

    return m_aRenderPicture;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SystemGraphicsData X11SalGraphics::GetGraphicsData() const
{
    SystemGraphicsData aRes;

    aRes.nSize = sizeof(aRes);
    aRes.pDisplay  = GetXDisplay();
    aRes.hDrawable = hDrawable_;
    aRes.pVisual   = GetDisplay()->GetVisual( m_nScreen ).GetVisual();
    aRes.nScreen   = m_nScreen;
    aRes.nDepth    = GetDisplay()->GetVisual( m_nScreen ).GetDepth();
    aRes.aColormap = GetDisplay()->GetColormap( m_nScreen ).GetXColormap();
    aRes.pRenderFormat = m_pRenderFormat;
    return aRes;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// B2DPolygon support methods

namespace { // anonymous namespace to prevent export
// the methods and structures here are used by the
// B2DPolyPolygon->RenderTrapezoid conversion algorithm

// compare two line segments
// assumption: both segments point downward
// assumption: they must have at least some y-overlap
// assumption: rA.p1.y <= rB.p1.y
bool IsLeftOf( const XLineFixed& rA, const XLineFixed& rB )
{
    bool bAbove = (rA.p1.y <= rB.p1.y);
    const XLineFixed& rU = bAbove ? rA : rB;
    const XLineFixed& rL = bAbove ? rB : rA;

    const XFixed aXDiff = rU.p2.x - rU.p1.x;
    const XFixed aYDiff = rU.p2.y - rU.p1.y;

    if( (rU.p1.y != rL.p1.y) || (rU.p1.x != rL.p1.x) )
    {
        const sal_Int64 n1 = (sal_Int64)aXDiff * (rL.p1.y - rU.p1.y);
        const sal_Int64 n2 = (sal_Int64)aYDiff * (rL.p1.x - rU.p1.x);
        if( n1 != n2 )
            return ((n1 < n2) == bAbove);
    }

    if( (rU.p2.y != rL.p2.y) || (rU.p2.x != rL.p2.x) )
    {
        const sal_Int64 n3 = (sal_Int64)aXDiff * (rL.p2.y - rU.p1.y);
        const sal_Int64 n4 = (sal_Int64)aYDiff * (rL.p2.x - rU.p1.x);
        if( n3 != n4 )
            return ((n3 < n4) == bAbove);
    }

    // both segments overlap
    return false;
}

struct HalfTrapezoid
{
    // assumptions:
    //    maLine.p1.y <= mnY < maLine.p2.y
    XLineFixed  maLine;
    XFixed      mnY;
};

struct HalfTrapCompare
{
    bool operator()( const HalfTrapezoid& rA, const HalfTrapezoid& rB ) const
    {
        bool bIsTopLeft = false;
        if( rA.mnY != rB.mnY )  // sort top-first if possible
            bIsTopLeft = (rA.mnY < rB.mnY);
        else                    // else sort left-first
            bIsTopLeft = IsLeftOf( rA.maLine, rB.maLine );
        // adjust to priority_queue sorting convention
        return !bIsTopLeft;
    }
};

typedef std::priority_queue< HalfTrapezoid, std::vector<HalfTrapezoid>, HalfTrapCompare > HTQueueBase;
// we need a priority queue with a reserve() to prevent countless reallocations
class HTQueue
:   public HTQueueBase
{
public:
    void    reserve( size_t n ) { c.reserve( n ); }
    int     capacity() { return c.capacity(); }
};

typedef std::vector<XTrapezoid> TrapezoidVector;

class TrapezoidXCompare
{
    const TrapezoidVector& mrVector;
public:
    TrapezoidXCompare( const TrapezoidVector& rVector )
        : mrVector( rVector ) {}
    bool operator()( int nA, int nB ) const
        { return IsLeftOf( mrVector[nA].left, mrVector[nB].left ); }
};

typedef std::multiset< int, TrapezoidXCompare > ActiveTrapSet;

class TrapezoidYCompare
{
    const TrapezoidVector& mrVector;
public:
    TrapezoidYCompare( const TrapezoidVector& rVector )
        : mrVector( rVector ) {}
    bool operator()( int nA, int nB ) const
        { return (mrVector[nA].bottom < mrVector[nB].bottom); }
};

typedef std::multiset< int, TrapezoidYCompare > VerticalTrapSet;
} // end of anonymous namespace

// draw a poly-polygon
bool X11SalGraphics::drawPolyPolygon( const ::basegfx::B2DPolyPolygon& rPolyPoly, double fTransparency)
{
    // nothing to do for empty polypolygons
    const int nPolygonCount = rPolyPoly.count();
    if( nPolygonCount <= 0 )
        return TRUE;

    // nothing to do if everything is transparent
    if( (nBrushColor_ == SALCOLOR_NONE)
    &&  (nPenColor_ == SALCOLOR_NONE) )
        return TRUE;

    // cannot handle pencolor!=brushcolor yet
    if( (nPenColor_ != SALCOLOR_NONE)
    &&  (nPenColor_ != nBrushColor_) )
        return FALSE;

    // TODO: remove the env-variable when no longer needed
    static const char* pRenderEnv = getenv( "SAL_DISABLE_RENDER_POLY" );
    if( pRenderEnv )
        return FALSE;

    // check xrender support for trapezoids
    XRenderPeer& rRenderPeer = XRenderPeer::GetInstance();
    if( !rRenderPeer.AreTrapezoidsSupported() )
        return FALSE;
    Picture aDstPic = GetXRenderPicture();
    // check xrender support for this drawable
    if( !aDstPic )
        return FALSE;

    // don't bother with polygons outside of visible area
    const basegfx::B2DRange aViewRange( 0, 0, GetGraphicsWidth(), GetGraphicsHeight() );
    const basegfx::B2DRange aPolyRange = basegfx::tools::getRange( rPolyPoly );
    const bool bNeedViewClip = !aPolyRange.isInside( aViewRange );
    if( !aPolyRange.overlaps( aViewRange ) )
        return true;

    // convert the polypolygon to trapezoids

    // first convert the B2DPolyPolygon to HalfTrapezoids
    // #i100922# try to prevent priority-queue reallocations by reservering enough
    int nHTQueueReserve = 0;
    for( int nOuterPolyIdx = 0; nOuterPolyIdx < nPolygonCount; ++nOuterPolyIdx )
    {
        const ::basegfx::B2DPolygon aOuterPolygon = rPolyPoly.getB2DPolygon( nOuterPolyIdx );
        const int nPointCount = aOuterPolygon.count();
        nHTQueueReserve += aOuterPolygon.areControlPointsUsed() ? 8 * nPointCount : nPointCount;
    }
    nHTQueueReserve = ((4*nHTQueueReserve) | 0x1FFF) + 1;
    HTQueue aHTQueue;
    aHTQueue.reserve( nHTQueueReserve );
    for( int nOuterPolyIdx = 0; nOuterPolyIdx < nPolygonCount; ++nOuterPolyIdx )
    {
        const ::basegfx::B2DPolygon aOuterPolygon = rPolyPoly.getB2DPolygon( nOuterPolyIdx );

        // render-trapezoids should be inside the view => clip polygon against view range
        basegfx::B2DPolyPolygon aClippedPolygon( aOuterPolygon );
        if( bNeedViewClip )
        {
            aClippedPolygon = basegfx::tools::clipPolygonOnRange( aOuterPolygon, aViewRange, true, false );
            DBG_ASSERT( aClippedPolygon.count(), "polygon confirmed to overlap with view should not get here" );
            if( !aClippedPolygon.count() )
                continue;
        }

        // render-trapezoids have linear edges => get rid of bezier segments
        if( aClippedPolygon.areControlPointsUsed() )
            aClippedPolygon = ::basegfx::tools::adaptiveSubdivideByDistance( aClippedPolygon, 0.125 );

        // test and remove self intersections
        // TODO: make code intersection save, then remove this test
        basegfx::B2DPolyPolygon aInnerPolyPoly(basegfx::tools::solveCrossovers( aClippedPolygon));
        const int nInnerPolyCount = aInnerPolyPoly.count();
        for( int nInnerPolyIdx = 0; nInnerPolyIdx < nInnerPolyCount; ++nInnerPolyIdx )
        {
            ::basegfx::B2DPolygon aInnerPolygon = aInnerPolyPoly.getB2DPolygon( nInnerPolyIdx );
            const int nPointCount = aInnerPolygon.count();
            if( !nPointCount )
                continue;

            aHTQueue.reserve( aHTQueue.size() + 8 * nPointCount );

            // convert polygon point pairs to HalfTrapezoids
            // connect the polygon point with the first one if needed
            XPointFixed aOldXPF = { 0, 0 };
            XPointFixed aNewXPF;
            for( int nPointIdx = 0; nPointIdx <= nPointCount; ++nPointIdx, aOldXPF = aNewXPF )
            {
                const int k = (nPointIdx < nPointCount) ? nPointIdx : 0;
                const ::basegfx::B2DPoint& aPoint = aInnerPolygon.getB2DPoint( k );

                // convert the B2DPoint into XRENDER units
                if(getAntiAliasB2DDraw())
                {
                    aNewXPF.x = XDoubleToFixed( aPoint.getX() );
                    aNewXPF.y = XDoubleToFixed( aPoint.getY() );
                }
                else
                {
                    aNewXPF.x = XDoubleToFixed( basegfx::fround( aPoint.getX() ) );
                    aNewXPF.y = XDoubleToFixed( basegfx::fround( aPoint.getY() ) );
                }

                // check if enough data is available for a new HalfTrapezoid
                if( nPointIdx == 0 )
                    continue;
                // ignore vertical segments
                if( aNewXPF.y == aOldXPF.y )
                    continue;

                // construct HalfTrapezoid as topdown segment
                HalfTrapezoid aHT;
                if( aNewXPF.y < aOldXPF.y )
                {
                    aHT.maLine.p1 = aNewXPF;
                    aHT.maLine.p2 = aOldXPF;
                }
                else
                {
                    aHT.maLine.p2 = aNewXPF;
                    aHT.maLine.p1 = aOldXPF;
                }

                aHT.mnY = aHT.maLine.p1.y;

#if 0 // ignore clipped HalfTrapezoids
            if( aHT.mnY < 0 )
                aHT.mnY = 0;
            else if( aHT.mnY > 10000 )
                continue;
#endif

                // queue up the HalfTrapezoid
                aHTQueue.push( aHT );
            }
        }
    }

    if( aHTQueue.empty() )
        return TRUE;

    // then convert the HalfTrapezoids into full Trapezoids
    TrapezoidVector aTrapVector;
    aTrapVector.reserve( aHTQueue.size() * 2 ); // just a guess

    TrapezoidXCompare aTrapXCompare( aTrapVector );
    ActiveTrapSet aActiveTraps( aTrapXCompare );

    TrapezoidYCompare aTrapYCompare( aTrapVector );
    VerticalTrapSet aVerticalTraps( aTrapYCompare );

    while( !aHTQueue.empty() )
    {
        XTrapezoid aTrapezoid;

        // convert a HalfTrapezoid pair
        const HalfTrapezoid& rLeft = aHTQueue.top();
        aTrapezoid.top = rLeft.mnY;
        aTrapezoid.bottom = rLeft.maLine.p2.y;
        aTrapezoid.left = rLeft.maLine;

#if 0
        // ignore empty trapezoids
        if( aTrapezoid.bottom <= aTrapezoid.top )
            continue;
#endif

        aHTQueue.pop();
        if( aHTQueue.empty() ) // TODO: assert
            break;
        const HalfTrapezoid& rRight = aHTQueue.top();
        aTrapezoid.right = rRight.maLine;
        aHTQueue.pop();

        aTrapezoid.bottom = aTrapezoid.left.p2.y;
        if( aTrapezoid.bottom > aTrapezoid.right.p2.y )
            aTrapezoid.bottom = aTrapezoid.right.p2.y;

        // keep the full Trapezoid candidate
        aTrapVector.push_back( aTrapezoid );

        // unless it splits an older trapezoid
        bool bSplit = false;
        for(;;)
        {
            // check if the new trapezoid overlaps with an old trapezoid
            ActiveTrapSet::iterator aActiveTrapsIt
                = aActiveTraps.upper_bound( aTrapVector.size()-1 );
            if( aActiveTrapsIt == aActiveTraps.begin() )
                break;
            --aActiveTrapsIt;

            XTrapezoid& rLeftTrap = aTrapVector[ *aActiveTrapsIt ];

            // in the ActiveTrapSet there are still trapezoids where
            // a vertical overlap with new trapezoids is no longer possible
            // they could have been removed in the verticaltraps loop below
            // but this would have been expensive and is not needed as we can
            // simply ignore them now and remove them from the ActiveTrapSet
            // so they won't bother us in the future
            if( rLeftTrap.bottom <= aTrapezoid.top )
            {
                aActiveTraps.erase( aActiveTrapsIt );
                continue;
            }

            // check if there is horizontal overlap
            // aTrapezoid.left==rLeftTrap.right is allowed though
            if( !IsLeftOf( aTrapezoid.left, rLeftTrap.right ) )
                break;

            // split the old trapezoid and keep its upper part
            // find the old trapezoids entry in the VerticalTrapSet and remove it
            typedef std::pair<VerticalTrapSet::iterator, VerticalTrapSet::iterator> VTSPair;
            VTSPair aVTSPair = aVerticalTraps.equal_range( *aActiveTrapsIt );
            VerticalTrapSet::iterator aVTSit = aVTSPair.first;
            for(; (aVTSit != aVTSPair.second) && (*aVTSit != *aActiveTrapsIt); ++aVTSit ) ;
            if( aVTSit != aVTSPair.second )
                aVerticalTraps.erase( aVTSit );
            // then update the old trapezoid's bottom
            rLeftTrap.bottom = aTrapezoid.top;
            // enter the updated old trapzoid in VerticalTrapSet
            aVerticalTraps.insert( aVerticalTraps.begin(), *aActiveTrapsIt );
            // the old trapezoid is no longer active
            aActiveTraps.erase( aActiveTrapsIt );

            // the trapezoid causing the split has become obsolete
            // so its both sides have to be re-queued
            HalfTrapezoid aHT;
            aHT.mnY = aTrapezoid.top;
            aHT.maLine = aTrapezoid.left;
            aHTQueue.push( aHT );
            aHT.maLine = aTrapezoid.right;
            aHTQueue.push( aHT );

            bSplit = true;
            break;
        }

        // keep or forget the resulting full Trapezoid
        if( bSplit )
            aTrapVector.pop_back();
        else
        {
            aActiveTraps.insert( aTrapVector.size()-1 );
            aVerticalTraps.insert( aTrapVector.size()-1 );
        }

        // mark trapezoids that can no longer be split as inactive
        // and recycle their sides which were not fully resolved
        static const XFixed nMaxTop = +0x7FFFFFFF;
        XFixed nNewTop = aHTQueue.empty() ? nMaxTop : aHTQueue.top().mnY;
        while( !aVerticalTraps.empty() )
        {
            const XTrapezoid& rOldTrap = aTrapVector[ *aVerticalTraps.begin() ];
            if( nNewTop < rOldTrap.bottom )
                break;
            // the reference Trapezoid can no longer be split
            aVerticalTraps.erase( aVerticalTraps.begin() );

            // recycle its sides that were not fully resolved
            HalfTrapezoid aHT;
            aHT.mnY = rOldTrap.bottom;
            if( rOldTrap.left.p2.y > rOldTrap.bottom )
            {
                aHT.maLine = rOldTrap.left;
                aHTQueue.push( aHT );
            }
            if( rOldTrap.right.p2.y > rOldTrap.bottom )
            {
                aHT.maLine = rOldTrap.right;
                aHTQueue.push( aHT );
            }
        }
    }

    // create xrender Picture for polygon foreground
    SalDisplay::RenderEntry& rEntry = GetDisplay()->GetRenderEntries( m_nScreen )[ 32 ];
    if( !rEntry.m_aPicture )
    {
        Display* pXDisplay = GetXDisplay();

        rEntry.m_aPixmap = ::XCreatePixmap( pXDisplay, hDrawable_, 1, 1, 32 );
        XRenderPictureAttributes aAttr;
        aAttr.repeat = true;

        XRenderPictFormat* pXRPF = rRenderPeer.FindStandardFormat( PictStandardARGB32 );
        rEntry.m_aPicture = rRenderPeer.CreatePicture( rEntry.m_aPixmap, pXRPF, CPRepeat, &aAttr );
    }

    // set polygon foreground color and opacity
    XRenderColor aRenderColor = GetXRenderColor( nBrushColor_ , fTransparency );
    rRenderPeer.FillRectangle( PictOpSrc, rEntry.m_aPicture, &aRenderColor, 0, 0, 1, 1 );

    // set clipping
    // TODO: move into GetXRenderPicture?
    if( pClipRegion_ && !XEmptyRegion( pClipRegion_ ) )
        rRenderPeer.SetPictureClipRegion( aDstPic, pClipRegion_ );

    // render the trapezoids
    const XRenderPictFormat* pMaskFormat = rRenderPeer.GetStandardFormatA8();
    rRenderPeer.CompositeTrapezoids( PictOpOver,
        rEntry.m_aPicture, aDstPic, pMaskFormat, 0, 0, &aTrapVector[0], aTrapVector.size() );

    return TRUE;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool X11SalGraphics::drawPolyLine(const ::basegfx::B2DPolygon& rPolygon, const ::basegfx::B2DVector& rLineWidth, basegfx::B2DLineJoin eLineJoin)
{
    // #i101491#
    if(rPolygon.count() > 1000)
    {
        // the used basegfx::tools::createAreaGeometry is simply too
        // expensive with very big polygons; fallback to caller (who
        // should use ImplLineConverter normally)
        return false;
    }

    const XRenderPeer& rRenderPeer = XRenderPeer::GetInstance();
    if( !rRenderPeer.AreTrapezoidsSupported() )
        return false;

    // get the area polygon for the line polygon
    basegfx::B2DPolygon aPolygon = rPolygon;
    if( (rLineWidth.getX() != rLineWidth.getY())
    && !basegfx::fTools::equalZero( rLineWidth.getY() ) )
    {
        // prepare for createAreaGeometry() with anisotropic linewidth
        basegfx::B2DHomMatrix aAnisoMatrix;
        aAnisoMatrix.scale( 1.0, rLineWidth.getX() / rLineWidth.getY() );
        aPolygon.transform( aAnisoMatrix );
    }

    // AW: reSegment no longer needed; new createAreaGeometry will remove exteme positions
    // and create bezier polygons
    //if( aPolygon.areControlPointsUsed() )
    //    aPolygon = basegfx::tools::reSegmentPolygonEdges( aPolygon, 8, true, false );
    //const basegfx::B2DPolyPolygon aAreaPolyPoly = basegfx::tools::createAreaGeometryForSimplePolygon(
    //    aPolygon, 0.5*rLineWidth.getX(), eLineJoin );
    const basegfx::B2DPolyPolygon aAreaPolyPoly(basegfx::tools::createAreaGeometry(aPolygon, 0.5*rLineWidth.getX(), eLineJoin));

    if( (rLineWidth.getX() != rLineWidth.getY())
    && !basegfx::fTools::equalZero( rLineWidth.getX() ) )
    {
        // postprocess createAreaGeometry() for anisotropic linewidth
        basegfx::B2DHomMatrix aAnisoMatrix;
        aAnisoMatrix.scale( 1.0, rLineWidth.getY() / rLineWidth.getX() );
        aPolygon.transform( aAnisoMatrix );
    }

    // temporarily adjust brush color to pen color
    // since the line is drawn as an area-polygon
    const SalColor aKeepBrushColor = nBrushColor_;
    nBrushColor_ = nPenColor_;

    // draw each area polypolygon component individually
    // to emulate the polypolygon winding rule "non-zero"
    bool bDrawOk = true;
    const int nPolyCount = aAreaPolyPoly.count();
    for( int nPolyIdx = 0; nPolyIdx < nPolyCount; ++nPolyIdx )
    {
        const ::basegfx::B2DPolyPolygon aOnePoly( aAreaPolyPoly.getB2DPolygon( nPolyIdx ) );
        bDrawOk = drawPolyPolygon( aOnePoly, 0.0);
        if( !bDrawOk )
            break;
    }

    // restore the original brush GC
    nBrushColor_ = aKeepBrushColor;
    return bDrawOk;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
