/*************************************************************************
 *
 *  $RCSfile: canvashelper.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 17:12:04 $
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

#include <canvas/debug.hxx>

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif
#ifndef _SV_BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif
#ifndef _SV_BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif
#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#include <utility>

#include <canvas/canvastools.hxx>

#include "textlayout.hxx"
#include "parametricpolypolygon.hxx"
#include "canvashelper.hxx"
#include "canvasbitmap.hxx"
#include "impltools.hxx"
#include "canvasfont.hxx"


using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star;


namespace vclcanvas
{
    CanvasHelper::CanvasHelper() :
        mxDevice(),
        mpProtectedOutDev(),
        mpOutDev(),
        mp2ndOutDev()
    {
    }

    void CanvasHelper::disposing()
    {
        mxDevice.reset();
        mpProtectedOutDev.reset();
        mpOutDev.reset();
        mp2ndOutDev.reset();
    }

    void CanvasHelper::setGraphicDevice( const WindowGraphicDevice::ImplRef& rDevice )
    {
        mxDevice = rDevice;
    }

    void CanvasHelper::setOutDev( const OutDevProviderSharedPtr& rOutDev, bool bProtect )
    {
        mpOutDev = rOutDev;

        if( bProtect )
            mpProtectedOutDev = rOutDev;
    }

    void CanvasHelper::setBackgroundOutDev( const OutDevProviderSharedPtr& rOutDev )
    {
        mp2ndOutDev = rOutDev;
    }

    void SAL_CALL CanvasHelper::drawPoint( const geometry::RealPoint2D&     aPoint,
                                           const rendering::ViewState&      viewState,
                                           const rendering::RenderState&    renderState )
    {
        // are we disposed?
        if( mpOutDev.get() )
        {
            // nope, render
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            setupOutDevState( viewState, renderState, LINE_COLOR );

            const Point aOutPoint( tools::mapRealPoint2D( aPoint,
                                                          viewState, renderState ) );
            // TODO(F1): alpha
            mpOutDev->getOutDev().DrawPixel( aOutPoint );

            if( mp2ndOutDev.get() )
                mp2ndOutDev->getOutDev().DrawPixel( aOutPoint );
        }
    }

    void SAL_CALL CanvasHelper::drawLine( const geometry::RealPoint2D&      aStartRealPoint2D,
                                          const geometry::RealPoint2D&      aEndRealPoint2D,
                                          const rendering::ViewState&       viewState,
                                          const rendering::RenderState&     renderState )
    {
        // are we disposed?
        if( mpOutDev.get() )
        {
            // nope, render
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            setupOutDevState( viewState, renderState, LINE_COLOR );

            const Point aStartPoint( tools::mapRealPoint2D( aStartRealPoint2D,
                                                            viewState, renderState ) );
            const Point aEndPoint( tools::mapRealPoint2D( aEndRealPoint2D,
                                                          viewState, renderState ) );
            // TODO(F2): alpha
            mpOutDev->getOutDev().DrawLine( aStartPoint, aEndPoint );

            if( mp2ndOutDev.get() )
                mp2ndOutDev->getOutDev().DrawLine( aStartPoint, aEndPoint );
        }
    }

    void SAL_CALL CanvasHelper::drawBezier( const geometry::RealBezierSegment2D&    aBezierSegment,
                                            const geometry::RealPoint2D&            _aEndPoint,
                                            const rendering::ViewState&             viewState,
                                            const rendering::RenderState&           renderState )
    {
        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            setupOutDevState( viewState, renderState, LINE_COLOR );

            const Point aStartPoint( tools::mapRealPoint2D( geometry::RealPoint2D(aBezierSegment.Px, aBezierSegment.Py),
                                                            viewState, renderState ) );
            const Point aEndPoint( tools::mapRealPoint2D( _aEndPoint,
                                                          viewState, renderState ) );
            // TODO(F1): Provide true beziers here!
            // TODO(F2): alpha
            mpOutDev->getOutDev().DrawLine( aStartPoint, aEndPoint );
            if( mp2ndOutDev.get() )
                mp2ndOutDev->getOutDev().DrawLine( aStartPoint, aEndPoint );
        }
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::drawPolyPolygon( const uno::Reference< rendering::XPolyPolygon2D >&    xPolyPolygon,
                                                                                          const rendering::ViewState&                           viewState,
                                                                                          const rendering::RenderState&                         renderState )
    {
        CHECK_AND_THROW( xPolyPolygon.is(),
                         "CanvasHelper::drawPolyPolygon(): polygon is NULL");

        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            setupOutDevState( viewState, renderState, LINE_COLOR );

            const PolyPolygon aPolyPoly( tools::mapPolyPolygon( tools::polyPolygonFromXPolyPolygon2D(xPolyPolygon),
                                                                viewState, renderState ) );
            mpOutDev->getOutDev().DrawPolyPolygon( aPolyPoly );

            if( mp2ndOutDev.get() )
                mp2ndOutDev->getOutDev().DrawPolyPolygon( aPolyPoly );
        }

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::strokePolyPolygon( const uno::Reference< rendering::XPolyPolygon2D >&  xPolyPolygon,
                                                                                            const rendering::ViewState&                         viewState,
                                                                                            const rendering::RenderState&                       renderState,
                                                                                            const rendering::StrokeAttributes&                  strokeAttributes )
    {
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::strokeTexturedPolyPolygon( const uno::Reference< rendering::XPolyPolygon2D >&  xPolyPolygon,
                                                                                                    const rendering::ViewState&                         viewState,
                                                                                                    const rendering::RenderState&                       renderState,
                                                                                                    const uno::Sequence< rendering::Texture >&          textures,
                                                                                                    const rendering::StrokeAttributes&                  strokeAttributes )
    {
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::strokeTextureMappedPolyPolygon( const uno::Reference< rendering::XPolyPolygon2D >& xPolyPolygon,
                                                                                                         const rendering::ViewState&                        viewState,
                                                                                                         const rendering::RenderState&                      renderState,
                                                                                                         const uno::Sequence< rendering::Texture >&         textures,
                                                                                                         const uno::Reference< geometry::XMapping2D >&      xMapping,
                                                                                                         const rendering::StrokeAttributes&                 strokeAttributes )
    {
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XPolyPolygon2D >   SAL_CALL CanvasHelper::queryStrokeShapes( const uno::Reference< rendering::XPolyPolygon2D >&  xPolyPolygon,
                                                                                            const rendering::ViewState&                         viewState,
                                                                                            const rendering::RenderState&                       renderState,
                                                                                            const rendering::StrokeAttributes&                  strokeAttributes )
    {
        return uno::Reference< rendering::XPolyPolygon2D >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::fillPolyPolygon( const uno::Reference< rendering::XPolyPolygon2D >&    xPolyPolygon,
                                                                                          const rendering::ViewState&                           viewState,
                                                                                          const rendering::RenderState&                         renderState )
    {
        CHECK_AND_THROW( xPolyPolygon.is(),
                         "CanvasHelper::fillPolyPolygon(): polygon is NULL");

        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            const int nTransparency( setupOutDevState( viewState, renderState, FILL_COLOR ) );
            const int nTransPercent( (nTransparency * 100 + 128) / 255 );  // normal rounding, no truncation here
            const PolyPolygon aPolyPoly( tools::mapPolyPolygon( tools::polyPolygonFromXPolyPolygon2D(xPolyPolygon),
                                                                viewState, renderState ) );

            if( !nTransparency )
                mpOutDev->getOutDev().DrawPolyPolygon( aPolyPoly );
            else
                mpOutDev->getOutDev().DrawTransparent( aPolyPoly, nTransPercent );

            if( mp2ndOutDev.get() )
            {
                if( nTransparency )
                    mp2ndOutDev->getOutDev().DrawPolyPolygon( aPolyPoly );
                else
                    mp2ndOutDev->getOutDev().DrawTransparent( aPolyPoly, nTransPercent );
            }
        }

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::fillTexturedPolyPolygon( const uno::Reference< rendering::XPolyPolygon2D >&    xPolyPolygon,
                                                                                                  const rendering::ViewState&                           viewState,
                                                                                                  const rendering::RenderState&                         renderState,
                                                                                                  const uno::Sequence< rendering::Texture >&            textures )
    {
        CHECK_AND_THROW( xPolyPolygon.is(),
                         "CanvasHelper::fillPolyPolygon(): polygon is NULL");
        CHECK_AND_THROW( textures.getLength(),
                         "CanvasHelper::fillTexturedPolyPolygon: empty texture sequence");

        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            const int nTransparency( setupOutDevState( viewState, renderState, FILL_COLOR ) );
            const PolyPolygon aPolyPoly( tools::mapPolyPolygon( tools::polyPolygonFromXPolyPolygon2D(xPolyPolygon),
                                                                viewState, renderState ) );

            // TODO(F1): Multi-texturing
            if( textures[0].Gradient.is() )
            {
                uno::Reference< lang::XServiceInfo > xRef( textures[0].Gradient,
                                                           uno::UNO_QUERY );

                if( xRef.is() &&
                    xRef->getImplementationName().equals(
                        ::rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                PARAMETRICPOLYPOLYGON_IMPLEMENTATION_NAME))) )
                {
                    // TODO(Q1): Maybe use dynamic_cast here

                    // TODO(E1): Return value
                    // TODO(F1): FillRule
                    static_cast<ParametricPolyPolygon*>(textures[0].Gradient.get())->fill(
                        mpOutDev->getOutDev(),
                        mp2ndOutDev.get() ? &mp2ndOutDev->getOutDev() : (OutputDevice*)NULL,
                        aPolyPoly,
                        viewState,
                        renderState,
                        textures[0],
                        nTransparency );
                }
            }
        }

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::fillTextureMappedPolyPolygon( const uno::Reference< rendering::XPolyPolygon2D >&   xPolyPolygon,
                                                                                                       const rendering::ViewState&                          viewState,
                                                                                                       const rendering::RenderState&                        renderState,
                                                                                                       const uno::Sequence< rendering::Texture >&           textures,
                                                                                                       const uno::Reference< geometry::XMapping2D >&        xMapping     )
    {
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCanvasFont > SAL_CALL CanvasHelper::createFont( const rendering::FontRequest&                   fontRequest,
                                                                                const uno::Sequence< beans::PropertyValue >&    extraFontProperties,
                                                                                const geometry::Matrix2D&                       fontMatrix )
    {
        if( mpOutDev.get() )
        {
            // TODO(F2): font properties and font matrix
            return uno::Reference< rendering::XCanvasFont >(
                    new CanvasFont(fontRequest, extraFontProperties, fontMatrix, mpOutDev) );
        }

        return uno::Reference< rendering::XCanvasFont >();
    }

    uno::Sequence< rendering::FontInfo > SAL_CALL CanvasHelper::queryAvailableFonts( const rendering::FontInfo&                     aFilter,
                                                                                     const uno::Sequence< beans::PropertyValue >&   aFontProperties )
    {
        // TODO
        return uno::Sequence< rendering::FontInfo >();
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::drawText( const rendering::StringContext&                  text,
                                                                                   const uno::Reference< rendering::XCanvasFont >&  xFont,
                                                                                   const rendering::ViewState&                      viewState,
                                                                                   const rendering::RenderState&                    renderState,
                                                                                   sal_Int8                                         textDirection )
    {
        CHECK_AND_THROW( xFont.is(),
                         "CanvasHelper::drawText(): font is NULL");

        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            ::Point aOutpos;
            if( !setupTextOutput( aOutpos, viewState, renderState, xFont ) )
                return uno::Reference< rendering::XCachedPrimitive >(NULL); // no output necessary

            // TODO(F2): alpha
            mpOutDev->getOutDev().DrawText( aOutpos,
                                            text.Text,
                                            ::canvas::tools::numeric_cast<USHORT>(text.StartPosition),
                                            ::canvas::tools::numeric_cast<USHORT>(text.Length) );

            if( mp2ndOutDev.get() )
                mp2ndOutDev->getOutDev().DrawText( aOutpos,
                                                   text.Text,
                                                   ::canvas::tools::numeric_cast<USHORT>(text.StartPosition),
                                                   ::canvas::tools::numeric_cast<USHORT>(text.Length) );
        }

        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::drawTextLayout( const uno::Reference< rendering::XTextLayout >&    xLayoutedText,
                                                                                         const rendering::ViewState&                        viewState,
                                                                                         const rendering::RenderState&                      renderState )
    {
        CHECK_AND_THROW( xLayoutedText.is(),
                         "CanvasHelper::drawTextLayout(): layout is NULL");

        uno::Reference< lang::XServiceInfo > xRef( xLayoutedText,
                                                   uno::UNO_QUERY );

        TextLayout* pTextLayout = NULL;

        if( xRef.is() &&
            xRef->getImplementationName().equals( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(TEXTLAYOUT_IMPLEMENTATION_NAME))) )
        {
            // TODO(P2): Maybe use dynamic_cast here (saves us a queryInterface)
            pTextLayout = static_cast<TextLayout*>(xLayoutedText.get());
        }
        else
        {
            CHECK_AND_THROW( false,
                             "CanvasHelper::drawTextLayout(): TextLayout not compatible with this canvas" );
        }

        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            ::Point aOutpos;
            if( !setupTextOutput( aOutpos, viewState, renderState, xLayoutedText->getFont() ) )
                return uno::Reference< rendering::XCachedPrimitive >(NULL); // no output necessary

            // TODO(F2): What about the offset scalings?
            // TODO(F2): alpha
            pTextLayout->draw( mpOutDev->getOutDev(), aOutpos, viewState, renderState );

            if( mp2ndOutDev.get() )
                pTextLayout->draw( mp2ndOutDev->getOutDev(), aOutpos, viewState, renderState );
        }

        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::drawBitmap( const uno::Reference< rendering::XBitmap >&    xBitmap,
                                                                                     const rendering::ViewState&                    viewState,
                                                                                     const rendering::RenderState&                  renderState )
    {
        CHECK_AND_THROW( xBitmap.is(),
                         "CanvasHelper::drawBitmap(): bitmap is NULL");

        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            setupOutDevState( viewState, renderState, IGNORE_COLOR );

            ::basegfx::B2DHomMatrix aMatrix;
            ::canvas::tools::mergeViewAndRenderTransform(aMatrix, viewState, renderState);

            ::basegfx::B2DPoint aOutputPos( 0.0, 0.0 );
            aOutputPos *= aMatrix;

            BitmapEx aBmpEx;

            if( !aMatrix.isIdentity() &&
                (!::basegfx::fTools::equalZero( aMatrix.get(0,1) ) ||
                 !::basegfx::fTools::equalZero( aMatrix.get(1,0) )) )
            {
                // complex transformation, use generic affine bitmap
                // transformation
                aBmpEx = tools::transformBitmap( tools::bitmapExFromXBitmap(xBitmap),
                                                 viewState, renderState );
            }
            else if( !aMatrix.isIdentity() &&
                     (!::rtl::math::approxEqual(aMatrix.get(0,0), 1.0) ||
                      !::rtl::math::approxEqual(aMatrix.get(1,1), 1.0)) )
            {
                // optimized case 2: only scale changes, use available
                // OutputDevice methods
                aBmpEx = tools::bitmapExFromXBitmap(xBitmap);

                ::Size aOutputSize( aBmpEx.GetSizePixel() );
                aOutputSize.setWidth ( ::basegfx::fround( aOutputSize.getWidth()  * aMatrix.get(0,0) ) );
                aOutputSize.setHeight( ::basegfx::fround( aOutputSize.getHeight() * aMatrix.get(1,1) ) );

                mpOutDev->getOutDev().DrawBitmapEx( ::vcl::unotools::pointFromB2DPoint( aOutputPos ),
                                                    aOutputSize,
                                                    aBmpEx );

                if( mp2ndOutDev.get() )
                    mp2ndOutDev->getOutDev().DrawBitmapEx( ::vcl::unotools::pointFromB2DPoint( aOutputPos ),
                                                           aOutputSize,
                                                           aBmpEx );

                // TODO(P1): Provide caching here.
                return uno::Reference< rendering::XCachedPrimitive >(NULL);
            }
            else
            {
                // optimized case: identity matrix, or only
                // translational components.
                aBmpEx = tools::bitmapExFromXBitmap(xBitmap);
            }

            mpOutDev->getOutDev().DrawBitmapEx( ::vcl::unotools::pointFromB2DPoint( aOutputPos ),
                                                aBmpEx );

            if( mp2ndOutDev.get() )
                mp2ndOutDev->getOutDev().DrawBitmapEx( ::vcl::unotools::pointFromB2DPoint( aOutputPos ),
                                                       aBmpEx );
        }

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XCachedPrimitive > SAL_CALL CanvasHelper::drawBitmapModulated( const uno::Reference< rendering::XBitmap >&   xBitmap,
                                                                                              const rendering::ViewState&                   viewState,
                                                                                              const rendering::RenderState&                 renderState )
    {
        CHECK_AND_THROW( xBitmap.is(),
                         "CanvasHelper::drawBitmap(): bitmap is NULL");

        // no color set -> this is equivalent to a plain drawBitmap(), then
        if( renderState.DeviceColor.getLength() < 3 )
            return drawBitmap( xBitmap, viewState, renderState );

        if( mpOutDev.get() )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            setupOutDevState( viewState, renderState, IGNORE_COLOR );

            ::basegfx::B2DHomMatrix aMatrix;
            ::canvas::tools::mergeViewAndRenderTransform(aMatrix, viewState, renderState);

            ::basegfx::B2DPoint aOutputPos( 0.0, 0.0 );
            aOutputPos *= aMatrix;

            BitmapEx aBmpEx;

            // TODO(P1): Have an optimized path for identity transform
            // and color modulation

            // always call transformBitmap, have to modulate colors
            aBmpEx = tools::transformBitmap( tools::bitmapExFromXBitmap(xBitmap),
                                             viewState, renderState,
                                             tools::MODULATE_WITH_DEVICECOLOR );

            mpOutDev->getOutDev().DrawBitmapEx( ::vcl::unotools::pointFromB2DPoint( aOutputPos ),
                                                aBmpEx );

            if( mp2ndOutDev.get() )
                mp2ndOutDev->getOutDev().DrawBitmapEx( ::vcl::unotools::pointFromB2DPoint( aOutputPos ),
                                                       aBmpEx );
        }

        // TODO(P1): Provide caching here.
        return uno::Reference< rendering::XCachedPrimitive >(NULL);
    }

    uno::Reference< rendering::XGraphicDevice > SAL_CALL CanvasHelper::getDevice()
    {
        return mxDevice.get();
    }

    void SAL_CALL CanvasHelper::copyRect( const uno::Reference< rendering::XBitmapCanvas >&     sourceCanvas,
                                          const geometry::RealRectangle2D&                      sourceRect,
                                          const rendering::ViewState&                           sourceViewState,
                                          const rendering::RenderState&                         sourceRenderState,
                                          const geometry::RealRectangle2D&                      destRect,
                                          const rendering::ViewState&                           destViewState,
                                          const rendering::RenderState&                         destRenderState )
    {
        // TODO(F1)
    }

    geometry::IntegerSize2D SAL_CALL CanvasHelper::getSize()
    {
        if( !mpOutDev.get() )
            return geometry::IntegerSize2D(); // we're disposed

        return ::vcl::unotools::integerSize2DFromSize( mpOutDev->getOutDev().GetOutputSizePixel() );
    }

    uno::Reference< rendering::XBitmap > SAL_CALL CanvasHelper::getScaledBitmap( const geometry::RealSize2D&    newSize,
                                                                                 sal_Bool                       beFast )
    {
        if( !mpOutDev.get() )
            return uno::Reference< rendering::XBitmap >(); // we're disposed

        // TODO(F2): Support alpha vdev canvas here
        const Point aEmptyPoint(0,0);
        const Size  aBmpSize( mpOutDev->getOutDev().GetOutputSizePixel() );

        Bitmap aBitmap( mpOutDev->getOutDev().GetBitmap(aEmptyPoint, aBmpSize) );

        aBitmap.Scale( ::vcl::unotools::sizeFromRealSize2D(newSize),
                       beFast ? BMP_SCALE_FAST : BMP_SCALE_INTERPOLATE );

        return uno::Reference< rendering::XBitmap >( new CanvasBitmap( aBitmap,
                                                                       mxDevice ) );
    }

    uno::Sequence< sal_Int8 > SAL_CALL CanvasHelper::getData( const geometry::IntegerRectangle2D& rect )
    {
        if( !mpOutDev.get() )
            return uno::Sequence< sal_Int8 >(); // we're disposed

        // TODO(F2): Support alpha canvas here
        const Rectangle aRect( ::vcl::unotools::rectangleFromIntegerRectangle2D(rect) );

        Bitmap aBitmap( mpOutDev->getOutDev().GetBitmap(aRect.TopLeft(),
                                                        aRect.GetSize()) );

        ScopedBitmapReadAccess pReadAccess( aBitmap.AcquireReadAccess(),
                                            aBitmap );

        if( pReadAccess.get() != NULL )
        {
            // TODO(F1): Support more formats.
            const sal_Int32 nWidth( rect.X2 - rect.X1 );
            const sal_Int32 nHeight( rect.Y2 - rect.Y1 );

            // for the time being, always return as BGRA
            uno::Sequence< sal_Int8 > aRes( 4*nWidth*nHeight );

            int nCurrPos(0);
            for( int y=0; y<nHeight; ++y )
            {
                for( int x=0; x<nWidth; ++x )
                {
                    aRes[ nCurrPos++ ] = pReadAccess->GetColor( y, x ).GetBlue();
                    aRes[ nCurrPos++ ] = pReadAccess->GetColor( y, x ).GetGreen();
                    aRes[ nCurrPos++ ] = pReadAccess->GetColor( y, x ).GetRed();
                    aRes[ nCurrPos++ ] = (sal_Int8)255L;
                }
            }

            return aRes;
        }

        return uno::Sequence< sal_Int8 >();
    }

    void SAL_CALL CanvasHelper::setData( const uno::Sequence< sal_Int8 >&    data,
                                         const geometry::IntegerRectangle2D& rect )
    {
        if( !mpOutDev.get() )
            return; // we're disposed

        OutputDevice& rOutDev( mpOutDev->getOutDev() );

        const Rectangle         aRect( ::vcl::unotools::rectangleFromIntegerRectangle2D(rect) );
        const USHORT            nBitCount( ::std::min( (USHORT)24U,
                                                       (USHORT)rOutDev.GetBitCount() ) );
        const BitmapPalette*    pPalette = NULL;

        if( nBitCount <= 8 )
        {
            // TODO(Q1): Extract this to a common place, e.g. GraphicDevice

            // try to determine palette from output device (by
            // extracting a 1,1 bitmap, and querying it)
            const Point aEmptyPoint;
            const Size  aSize(1,1);
            Bitmap aTmpBitmap( rOutDev.GetBitmap( aEmptyPoint,
                                                  aSize ) );

            ScopedBitmapReadAccess pReadAccess( aTmpBitmap.AcquireReadAccess(),
                                                aTmpBitmap );

            pPalette = &pReadAccess->GetPalette();
        }

        // TODO(F2): Support alpha canvas here
        Bitmap aBitmap( aRect.GetSize(), nBitCount, pPalette );

        bool bCopyBack( false ); // only copy something back, if we
                                 // actually changed some pixel

        {
            ScopedBitmapWriteAccess pWriteAccess( aBitmap.AcquireWriteAccess(),
                                                  aBitmap );

            if( pWriteAccess.get() != NULL )
            {
                // for the time being, always read as BGRA
                const sal_Int32 nWidth( rect.X2 - rect.X1 );
                const sal_Int32 nHeight( rect.Y2 - rect.Y1 );
                int x, y, nCurrPos(0);
                for( y=0; y<nHeight; ++y )
                {
                    switch( pWriteAccess->GetScanlineFormat() )
                    {
                        case BMP_FORMAT_8BIT_PAL:
                        {
                            Scanline pScan = pWriteAccess->GetScanline( y );

                            for( x=0; x<nWidth; ++x )
                            {
                                *pScan++ = (BYTE)pWriteAccess->GetBestPaletteIndex(
                                    BitmapColor( data[ nCurrPos+2 ],
                                                 data[ nCurrPos+1 ],
                                                 data[ nCurrPos ] ) );

                                nCurrPos += 4; // skip three colors, _plus_ alpha
                            }
                        }
                        break;

                        case BMP_FORMAT_24BIT_TC_BGR:
                        {
                            Scanline pScan = pWriteAccess->GetScanline( y );

                            for( x=0; x<nWidth; ++x )
                            {
                                Scanline pTmp = pScan + x * 3;

                                *pScan++ = data[ nCurrPos   ];
                                *pScan++ = data[ nCurrPos+1 ];
                                *pScan++ = data[ nCurrPos+2 ];

                                nCurrPos += 4; // skip three colors, _plus_ alpha
                            }
                        }
                        break;

                        case BMP_FORMAT_24BIT_TC_RGB:
                        {
                            Scanline pScan = pWriteAccess->GetScanline( y );

                            for( x=0; x<nWidth; ++x )
                            {
                                Scanline pTmp = pScan + x * 3;

                                *pScan++ = data[ nCurrPos+2 ];
                                *pScan++ = data[ nCurrPos+1 ];
                                *pScan++ = data[ nCurrPos   ];

                                nCurrPos += 4; // skip three colors, _plus_ alpha
                            }
                        }
                        break;

                        default:
                        {
                            for( x=0; x<nWidth; ++x )
                            {
                                pWriteAccess->SetPixel( y, x, BitmapColor( data[ nCurrPos+2 ],
                                                                           data[ nCurrPos+1 ],
                                                                           data[ nCurrPos ] ) );
                                nCurrPos += 4; // skip three colors, _plus_ alpha
                            }
                        }
                        break;
                    }
                }

                bCopyBack = true;
            }
        }

        // copy back only here, since the BitmapAccessors must be
        // destroyed beforehand
        if( bCopyBack )
        {
            tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

            // TODO(F2): Support alpha canvas here
            rOutDev.EnableMapMode( FALSE );
            rOutDev.DrawBitmap(aRect.TopLeft(), aBitmap);
        }
    }

    void SAL_CALL CanvasHelper::setPixel( const uno::Sequence< sal_Int8 >&  color,
                                          const geometry::IntegerPoint2D&   pos )
    {
        if( !mpOutDev.get() )
            return; // we're disposed

        OutputDevice& rOutDev( mpOutDev->getOutDev() );

        const Size aBmpSize( rOutDev.GetOutputSizePixel() );

        CHECK_AND_THROW( pos.X >= 0 && pos.X < aBmpSize.Width(),
                         "CanvasHelper::setPixel: X coordinate out of bounds" );
        CHECK_AND_THROW( pos.Y >= 0 && pos.Y < aBmpSize.Height(),
                         "CanvasHelper::setPixel: Y coordinate out of bounds" );
        CHECK_AND_THROW( color.getLength() > 3,
                         "CanvasHelper::setPixel: not enough color components" );

        tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

        rOutDev.EnableMapMode( FALSE );

        // TODO(F2): Support alpha canvas here
        rOutDev.DrawPixel( ::vcl::unotools::pointFromIntegerPoint2D( pos ),
                           ::vcl::unotools::sequenceToColor(
                               mxDevice.getRef(),
                               color ) );
    }

    uno::Sequence< sal_Int8 > SAL_CALL CanvasHelper::getPixel( const geometry::IntegerPoint2D& pos )
    {
        if( !mpOutDev.get() )
            return uno::Sequence< sal_Int8 >(); // we're disposed

        OutputDevice& rOutDev( mpOutDev->getOutDev() );

        const Size aBmpSize( rOutDev.GetOutputSizePixel() );

        CHECK_AND_THROW( pos.X >= 0 && pos.X < aBmpSize.Width(),
                         "CanvasHelper::getPixel: X coordinate out of bounds" );
        CHECK_AND_THROW( pos.Y >= 0 && pos.Y < aBmpSize.Height(),
                         "CanvasHelper::getPixel: Y coordinate out of bounds" );

        tools::OutDevStateKeeper aStateKeeper( mpProtectedOutDev );

        rOutDev.EnableMapMode( FALSE );

        // TODO(F2): Support alpha canvas here
        return ::vcl::unotools::colorToIntSequence( mxDevice.getRef(),
                                                    rOutDev.GetPixel(
                                                        ::vcl::unotools::pointFromIntegerPoint2D( pos ) ) );
    }

    uno::Reference< rendering::XBitmapPalette > SAL_CALL CanvasHelper::getPalette()
    {
        // TODO(F1): Provide palette support
        return uno::Reference< rendering::XBitmapPalette >();
    }

    rendering::IntegerBitmapLayout SAL_CALL CanvasHelper::getMemoryLayout()
    {
        // TODO(F1): finish that one
        rendering::IntegerBitmapLayout aLayout;

        if( !mpOutDev.get() )
            return aLayout; // we're disposed

        const Size aBmpSize( mpOutDev->getOutDev().GetOutputSizePixel() );

        aLayout.ScanLines = aBmpSize.Height();
        aLayout.ScanLineBytes = aBmpSize.Width()*4;
        aLayout.ScanLineStride = aLayout.ScanLineBytes;
        aLayout.Format = 0;
        aLayout.NumComponents = 4;
        aLayout.ComponentMasks = uno::Sequence<sal_Int64>();
        aLayout.Endianness = 0;
        aLayout.IsPseudoColor = false;

        return aLayout;
    }

    int CanvasHelper::setupOutDevState( const rendering::ViewState&     viewState,
                                        const rendering::RenderState&   renderState,
                                        ColorType                       eColorType )
    {
        ENSURE_AND_THROW( mpOutDev.get(),
                          "CanvasHelper::setupOutDevState(): outdev null. Are we disposed?" );

        OutputDevice& rOutDev( mpOutDev->getOutDev() );
        OutputDevice* p2ndOutDev = NULL;

        if( mp2ndOutDev.get() )
            p2ndOutDev = &mp2ndOutDev->getOutDev();

        int nTransparency(0);

        // TODO(P2): Don't change clipping all the time, maintain current clip
        // state and change only when update is necessary

        // accumulate non-empty clips into one region
        // ==========================================

        Region aClipRegion;

        if( viewState.Clip.is() )
        {
            ::basegfx::B2DPolyPolygon aClipPoly(
                tools::polyPolygonFromXPolyPolygon2D(
                    viewState.Clip) );

            if( aClipPoly.count() )
            {
                // setup non-empty clipping
                ::basegfx::B2DHomMatrix aMatrix;
                aClipPoly.transform(
                    ::basegfx::unotools::homMatrixFromAffineMatrix( aMatrix,
                                                                    viewState.AffineTransform ) );

                aClipRegion = Region( ::PolyPolygon( aClipPoly ) );
            }
        }

        if( renderState.Clip.is() )
        {
            ::basegfx::B2DPolyPolygon aClipPoly(
                tools::polyPolygonFromXPolyPolygon2D(
                    renderState.Clip) );

            ::basegfx::B2DHomMatrix aMatrix;
            aClipPoly.transform(
                ::canvas::tools::mergeViewAndRenderTransform( aMatrix,
                                                              viewState,
                                                              renderState ) );

            if( aClipPoly.count() )
            {
                // setup non-empty clipping
                Region aRegion = Region( ::PolyPolygon( aClipPoly ) );

                if( aClipRegion.IsEmpty() )
                    aClipRegion = aRegion;
                else
                    aClipRegion.Intersect( Region( ::PolyPolygon( aClipPoly ) ) );
            }
            else
            {
                // clip polygon is empty
                aClipRegion.SetEmpty();
            }
        }

        // setup accumulated clip region. Note that setting an
        // empty clip region denotes "clip everything" on the
        // OutputDevice (which is why we translate that into
        // SetClipRegion() here). When both view and render clip
        // are empty, aClipRegion remains default-constructed,
        // i.e. also empty.
        if( aClipRegion.IsEmpty() )
        {
            rOutDev.SetClipRegion();

            if( p2ndOutDev )
                p2ndOutDev->SetClipRegion();
        }
        else
        {
            rOutDev.SetClipRegion( aClipRegion );

            if( p2ndOutDev )
                p2ndOutDev->SetClipRegion( aClipRegion );
        }

        if( eColorType != IGNORE_COLOR )
        {
            Color aColor( COL_WHITE );

            if( renderState.DeviceColor.getLength() > 2 )
            {
                aColor = ::vcl::unotools::sequenceToColor( mxDevice.getRef(),
                                                           renderState.DeviceColor );
            }

            // extract alpha, and make color opaque
            // afterwards. Otherwise, OutputDevice won't draw anything
            nTransparency = aColor.GetTransparency();
            aColor.SetTransparency(0);

            switch( eColorType )
            {
                case LINE_COLOR:
                    rOutDev.SetLineColor( aColor );
                    rOutDev.SetFillColor();

                    if( p2ndOutDev )
                    {
                        p2ndOutDev->SetLineColor( aColor );
                        p2ndOutDev->SetFillColor();
                    }
                    break;

                case FILL_COLOR:
                    rOutDev.SetFillColor( aColor );
                    rOutDev.SetLineColor();

                    if( p2ndOutDev )
                    {
                        p2ndOutDev->SetFillColor( aColor );
                        p2ndOutDev->SetLineColor();
                    }
                    break;

                case TEXT_COLOR:
                    rOutDev.SetTextColor( aColor );

                    if( p2ndOutDev )
                        p2ndOutDev->SetTextColor( aColor );
                    break;

                default:
                    ENSURE_AND_THROW( false,
                                      "CanvasHelper::setupOutDevState(): Unexpected color type");
                    break;
            }
        }

        return nTransparency;
    }

    bool CanvasHelper::setupTextOutput( ::Point&                                        o_rOutPos,
                                        const rendering::ViewState&                     viewState,
                                        const rendering::RenderState&                   renderState,
                                        const uno::Reference< rendering::XCanvasFont >& xFont   )
    {
        ENSURE_AND_THROW( mpOutDev.get(),
                          "CanvasHelper::setupTextOutput(): outdev null. Are we disposed?" );

        setupOutDevState( viewState, renderState, TEXT_COLOR );

        OutputDevice& rOutDev( mpOutDev->getOutDev() );

        uno::Reference< lang::XServiceInfo > xRef( xFont,
                                                   uno::UNO_QUERY );

        ::Font aVCLFont;

        if( xRef.is() &&
            xRef->getImplementationName().equals( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(CANVASFONT_IMPLEMENTATION_NAME))) )
        {
            // TODO(P2): Maybe use dynamic_cast here (saves us a queryInterface)
            aVCLFont = static_cast<CanvasFont*>(xFont.get())->getVCLFont();
        }
        else
        {
            CHECK_AND_THROW( false,
                             "CanvasHelper::setupTextOutput(): Font not compatible with this canvas" );
        }

        Color aColor( COL_BLACK );

        if( renderState.DeviceColor.getLength() > 2 )
        {
            aColor = ::vcl::unotools::sequenceToColor( mxDevice.getRef(),
                                                       renderState.DeviceColor );
        }

        // setup font color
        aVCLFont.SetColor( aColor );
        aVCLFont.SetFillColor( aColor );

        // no need to replicate this for mp2ndOutDev, we're modifying only aVCLFont here.
        if( !tools::setupFontTransform( o_rOutPos, aVCLFont, viewState, renderState, rOutDev ) )
            return false;

        rOutDev.SetFont( aVCLFont );

        if( mp2ndOutDev.get() )
            mp2ndOutDev->getOutDev().SetFont( aVCLFont );

        return true;
    }

    void CanvasHelper::flush() const
    {
        if( mpOutDev.get() && mpOutDev->getOutDev().GetOutDevType() == OUTDEV_WINDOW )
        {
            // TODO(Q3): Evil downcast. And what's more, Window::Flush is
            // not even const. Wah.
            static_cast<Window&>(mpOutDev->getOutDev()).Flush();
        }

        if( mp2ndOutDev.get() && mp2ndOutDev->getOutDev().GetOutDevType() == OUTDEV_WINDOW )
        {
            // TODO(Q3): Evil downcast. And what's more, Window::Flush is
            // not even const. Wah.
            static_cast<Window&>(mp2ndOutDev->getOutDev()).Flush();
        }
    }

}
