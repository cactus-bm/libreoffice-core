/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: salgdiutils.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: ihi $ $Date: 2008-01-14 16:17:58 $
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
#include "precompiled_vcl.hxx"

#include "salgdi.h"
#include "salframe.h"

#include "basebmp/scanlineformats.hxx"
#include "basebmp/color.hxx"
#include "basegfx/range/b2drectangle.hxx"
#include "basegfx/range/b2irange.hxx"
#include "basegfx/vector/b2ivector.hxx"
#include "basegfx/polygon/b2dpolygon.hxx"
#include "basegfx/polygon/b2dpolygontools.hxx"
#include <boost/bind.hpp>

#include "vcl/svapp.hxx"
#include "saldata.hxx"

// ----------------------------------------------------------------------

void AquaSalGraphics::SetWindowGraphics( AquaSalFrame* pFrame )
{
    mpFrame     = pFrame;

    mbWindow    = true;
    mbPrinter   = false;
    mbVirDev    = false;
}

void AquaSalGraphics::SetPrinterGraphics( CGContextRef xContext, long nDPIX, long nDPIY, double fScale )
{
    mbWindow    = false;
    mbPrinter   = true;
    mbVirDev    = false;

    mrContext   = xContext;
    mfFakeDPIScale = fScale;
    mnRealDPIX  = nDPIX;
    mnRealDPIY  = nDPIY;

    if( mrContext )
    {
        CGContextSetFillColorSpace( mrContext, GetSalData()->mxRGBSpace );
        CGContextSetStrokeColorSpace( mrContext, GetSalData()->mxRGBSpace );
        CGContextSaveGState( mrContext );
        SetState();
    }
}

static void nil_free( void* )
{
}

void AquaSalGraphics::SetVirDevGraphics( CGContextRef xContext, bool bScreenCompatible )
{
    mbWindow    = false;
    mbPrinter   = false;
    mbVirDev    = true;

    mrContext   = xContext;
    sal_uInt8* pMemory = NULL;
    if( mrContext )
    {
        pMemory = reinterpret_cast<sal_uInt8*>(CGBitmapContextGetData( mrContext ));
        CGContextSetFillColorSpace( mrContext, GetSalData()->mxRGBSpace );
        CGContextSetStrokeColorSpace( mrContext, GetSalData()->mxRGBSpace );
        CGContextSaveGState( mrContext );
        SetState();
    }
    maContextMemory.reset( pMemory, boost::bind( nil_free, _1 ) );
}

// ----------------------------------------------------------------------

void AquaSalGraphics::SetState()
{
    CGContextRestoreGState( mrContext );
    CGContextSaveGState( mrContext );
    // set up clipping area
    if( mrClippingPath )
    {
        CGContextBeginPath( mrContext );                // discard any existing path
        CGContextAddPath( mrContext, mrClippingPath );  // set the current path to the clipping path
        CGContextClip( mrContext );                     // use it for clipping
    }

    // set RGB colorspace and line and fill colors
    CGContextSetFillColor( mrContext, mpFillColor );
    CGContextSetStrokeColor( mrContext, mpLineColor );
    if( mbXORMode && (mbWindow || mbVirDev) )
    {
        CGContextSetBlendMode(mrContext, kCGBlendModeDifference);
        int nWidth  = CGBitmapContextGetWidth(mrContext);
        int nHeight = CGBitmapContextGetHeight(mrContext);
        if( ! maXORDevice )
        {
            maXORDevice = basebmp::createBitmapDevice( basegfx::B2IVector( nWidth, nHeight ),
                                                       mbWindow,
                                                       basebmp::Format::THIRTYTWO_BIT_TC_MASK_ARGB,
                                                       maContextMemory,
                                                       basebmp::PaletteMemorySharedVector() );
            if( mrClippingPath && maClippingRects.size() == 1 )
            {
                /*
                * optimization: the case with only one clip rectangle is quite common
                * in this case it is much cheaper to constrain the XOR device to a smaller
                * area than to clip every pixel with the mask
                */
                maXORClipMask.reset();
                CGRect aBounds( maClippingRects.front() );
                basegfx::B2IRange aRect( static_cast<sal_Int32>(aBounds.origin.x),
                      static_cast<sal_Int32>(aBounds.origin.y),
                                         static_cast<sal_Int32>(aBounds.origin.x+aBounds.size.width),
                                         static_cast<sal_Int32>(aBounds.origin.y+aBounds.size.height) );
                maXORDevice = basebmp::subsetBitmapDevice( maXORDevice, aRect );
            }
        }
        if( mrClippingPath )
        {
            if( ! maXORClipMask && maClippingRects.size() > 1 )
            {
                maXORClipMask = basebmp::createBitmapDevice( basegfx::B2IVector( nWidth, nHeight ),
                                                             mbWindow,
                                                             basebmp::Format::ONE_BIT_MSB_GREY );
                maXORClipMask->clear( basebmp::Color(0xffffffff) );
                for( std::vector<CGRect>::const_iterator it = maClippingRects.begin(); it != maClippingRects.end(); ++it )
                {
                    basegfx::B2DRectangle aRect( it->origin.x, it->origin.y,
                                                 it->origin.x+it->size.width,
                                                 it->origin.y+it->size.height );
                    maXORClipMask->fillPolyPolygon( basegfx::B2DPolyPolygon( basegfx::tools::createPolygonFromRect( aRect ) ),
                                                    basebmp::Color( 0 ),
                                                    basebmp::DrawMode_PAINT
                                                    );
                }
            }
        }
        else
            maXORClipMask.reset();
    }

    CGContextSetShouldAntialias( mrContext, false );
}

// ----------------------------------------------------------------------

bool AquaSalGraphics::CheckContext()
{
    if( mpFrame != NULL )
    {
        const unsigned int nWidth = mpFrame->maGeometry.nWidth;
        const unsigned int nHeight = mpFrame->maGeometry.nHeight;

        CGContextRef rReleaseContext = 0;
        unsigned int nReleaseContextWidth = 0;
        unsigned int nReleaseContextHeight = 0;

        boost::shared_array<sal_uInt8> aOldMem;
        if( mrContext )
        {
            nReleaseContextWidth = CGBitmapContextGetWidth(mrContext);
            nReleaseContextHeight = CGBitmapContextGetHeight(mrContext);
            // check if window size changed and we need to create a new bitmap context
            if( (nReleaseContextWidth != nWidth)  || (nReleaseContextHeight != nHeight) )
            {
                rReleaseContext = mrContext;
                mrContext = 0;
                aOldMem = maContextMemory;
                maXORDevice.reset();
                maXORClipMask.reset();
            }
        }

        if( !mrContext )
        {
            maContextMemory.reset( reinterpret_cast<sal_uInt8*>( rtl_allocateMemory( nWidth * 4 * nHeight ) ),
                                   boost::bind( rtl_freeMemory, _1 ) );
            if( maContextMemory )
            {
                mrContext = CGBitmapContextCreate( maContextMemory.get(), nWidth, nHeight, 8, nWidth * 4, GetSalData()->mxRGBSpace, kCGImageAlphaNoneSkipFirst );

                if( mrContext )
                {
                    // copy bitmap data to new context
                    if( rReleaseContext )
                    {
                        CGRect aBounds;
                        aBounds.origin.x = aBounds.origin.y = 0;
                        aBounds.size.width = nReleaseContextWidth;
                        aBounds.size.height = nReleaseContextHeight;
                        CGImageRef xImage = CGBitmapContextCreateImage( rReleaseContext );
                        CGContextDrawImage( mrContext, aBounds, xImage );
                        CGImageRelease(xImage);
                    }

                    CGContextTranslateCTM( mrContext, 0, nHeight );
                    CGContextScaleCTM( mrContext, 1.0, -1.0 );
                    CGContextSetFillColorSpace( mrContext, GetSalData()->mxRGBSpace );
                    CGContextSetStrokeColorSpace( mrContext, GetSalData()->mxRGBSpace );
                    CGContextSaveGState( mrContext );
                    SetState();
                }
                else
                {
                    maContextMemory.reset(); // free memory again
                }
            }
        }

        if( rReleaseContext ) // released memory runs out of scope and is then freed
            CFRelease( rReleaseContext );
    }
    if( mrContext )
    {
        if( mbXORMode )
        {
            if( ! maXORDevice )
                SetState();
        }
        return true;
    }
    else
    {
        AquaLog("<<<WARNING>>> AquaSalGraphics::CheckContext() FAILED!!!!\n" );
        return false;
    }
}


void AquaSalGraphics::RefreshRect(float lX, float lY, float lWidth, float lHeight)
{
    if( ! mbWindow ) // view only on Window graphics
        return;

    if( mpFrame )
    {
        // update a little more around the designated rectangle
        // this helps with antialiased rendering
        NSRect aRect = { { lX-1, lY-1 }, { lWidth+2, lHeight+2 } };
        mpFrame->VCLToCocoa( aRect, false );
        [mpFrame->getView() setNeedsDisplayInRect: aRect];
    }
}

CGPoint* AquaSalGraphics::makeCGptArray(ULONG nPoints, const SalPoint*  pPtAry)
{
    AquaLog("-->%s\n",__func__);
    CGPoint *CGpoints = new (CGPoint[nPoints]);
    if ( CGpoints )
      {
        for(ULONG i=0;i<nPoints;i++)
          {
            CGpoints[i].x = (float)(pPtAry[i].mnX);
            CGpoints[i].y = (float)(pPtAry[i].mnY);
          }
      }
    return CGpoints;
}


// -----------------------------------------------------------------------

void AquaSalGraphics::UpdateWindow( NSRect& rRect )
{
    // FIXME: optimize UpdateWindow wrt to aRect
    NSGraphicsContext* pContext = [NSGraphicsContext currentContext];
    if( mrContext != NULL && mpFrame != NULL && pContext != nil )
    {
        CGContextRef rCGContext = reinterpret_cast<CGContextRef>([pContext graphicsPort]);

        CGMutablePathRef rClip = mpFrame->getClipPath();
        if( rClip )
        {
            CGContextSaveGState( rCGContext );
            CGContextBeginPath( rCGContext );
            CGContextAddPath( rCGContext, rClip );
            CGContextClip( rCGContext );
        }

        CGRect aBitmapRect = {
            { 0, 0 },
            { CGBitmapContextGetWidth(mrContext), CGBitmapContextGetHeight(mrContext) }
        };

        CGImageRef xImage = CGBitmapContextCreateImage( mrContext );
        CGContextDrawImage( rCGContext, aBitmapRect, xImage );
        CGImageRelease( xImage );
        CGContextFlush( rCGContext );
        if( rClip ) // cleanup clipping
            CGContextRestoreGState( rCGContext );
    }
}

// -----------------------------------------------------------------------

