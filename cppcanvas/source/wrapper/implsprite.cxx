/*************************************************************************
 *
 *  $RCSfile: implsprite.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-10 13:31:36 $
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

#ifndef _COM_SUN_STAR_RENDERING_XSPRITE_HPP_
#include <com/sun/star/rendering/XSprite.hpp>
#endif
#ifndef _COM_SUN_STAR_RENDERING_XANIMATEDSPRITE_HPP_
#include <com/sun/star/rendering/XAnimatedSprite.hpp>
#endif

#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif
#ifndef _CANVAS_CANVASTOOLS_HXX
#include <canvas/canvastools.hxx>
#endif


#include <implsprite.hxx>


using namespace ::com::sun::star;
using namespace ::com::sun::star;

namespace cppcanvas
{
    namespace internal
    {

        ImplSprite::ImplSprite( const uno::Reference< rendering::XSpriteCanvas >&       rParentCanvas,
                                const uno::Reference< rendering::XSprite >&             rSprite,
                                const ImplSpriteCanvas::TransformationArbiterSharedPtr& rTransformArbiter ) :
            mxGraphicDevice(),
            mxSprite( rSprite ),
            mxAnimatedSprite(),
            mpTransformArbiter( rTransformArbiter )
        {
            // Avoiding ternary operator in initializer list (Solaris
            // compiler bug, when function call and temporary is
            // involved)
            if( rParentCanvas.is() )
                mxGraphicDevice = rParentCanvas->getDevice();

            OSL_ENSURE( rParentCanvas.is() , "ImplSprite::ImplSprite(): Invalid canvas");
            OSL_ENSURE( mxGraphicDevice.is(), "ImplSprite::ImplSprite(): Invalid graphic device");
            OSL_ENSURE( mxSprite.is(), "ImplSprite::ImplSprite(): Invalid sprite");
            OSL_ENSURE( mpTransformArbiter.get(), "ImplSprite::ImplSprite(): Invalid transformation arbiter");
        }

        ImplSprite::ImplSprite( const uno::Reference< rendering::XSpriteCanvas >&       rParentCanvas,
                                const uno::Reference< rendering::XAnimatedSprite >&     rSprite,
                                const ImplSpriteCanvas::TransformationArbiterSharedPtr& rTransformArbiter ) :
            mxGraphicDevice(),
            mxSprite( uno::Reference< rendering::XSprite >(rSprite,
                                                           uno::UNO_QUERY) ),
            mxAnimatedSprite( rSprite ),
            mpTransformArbiter( rTransformArbiter )
        {
            // Avoiding ternary operator in initializer list (Solaris
            // compiler bug, when function call and temporary is
            // involved)
            if( rParentCanvas.is() )
                mxGraphicDevice = rParentCanvas->getDevice();

            OSL_ENSURE( rParentCanvas.is() , "ImplSprite::ImplSprite(): Invalid canvas");
            OSL_ENSURE( mxGraphicDevice.is(), "ImplSprite::ImplSprite(): Invalid graphic device");
            OSL_ENSURE( mxSprite.is(), "ImplSprite::ImplSprite(): Invalid sprite");
            OSL_ENSURE( mpTransformArbiter.get(), "ImplSprite::ImplSprite(): Invalid transformation arbiter");
        }

        ImplSprite::~ImplSprite()
        {
            // hide the sprite on the canvas. If we don't hide the
            // sprite, it will stay on the canvas forever, since the
            // canvas naturally keeps a list of visible sprites
            // (otherwise, it wouldn't be able to paint them
            // autonomously)
            if( mxSprite.is() )
                mxSprite->hide();
        }

        void ImplSprite::setAlpha( const double& rAlpha )
        {
            OSL_ENSURE( mxSprite.is(), "ImplSprite::setAlpha(): Invalid sprite");

            if( mxSprite.is() )
                mxSprite->setAlpha( rAlpha );
        }

        void ImplSprite::movePixel( const ::basegfx::B2DPoint& rNewPos )
        {
            OSL_ENSURE( mxSprite.is(), "ImplSprite::movePixel(): Invalid sprite");

            if( mxSprite.is() )
            {
                rendering::ViewState    aViewState;
                rendering::RenderState  aRenderState;

                ::canvas::tools::initViewState( aViewState );
                ::canvas::tools::initRenderState( aRenderState );

                mxSprite->move( ::basegfx::unotools::point2DFromB2DPoint( rNewPos ),
                                aViewState,
                                aRenderState );
            }
        }

        void ImplSprite::move( const ::basegfx::B2DPoint& rNewPos )
        {
            OSL_ENSURE( mxSprite.is(), "ImplSprite::move(): Invalid sprite");

            if( mxSprite.is() )
            {
                rendering::ViewState    aViewState;
                rendering::RenderState  aRenderState;

                ::canvas::tools::initViewState( aViewState );
                ::canvas::tools::initRenderState( aRenderState );

                ::canvas::tools::setViewStateTransform( aViewState,
                                                        mpTransformArbiter->getTransformation() );

                mxSprite->move( ::basegfx::unotools::point2DFromB2DPoint( rNewPos ),
                                aViewState,
                                aRenderState );
            }
        }

        void ImplSprite::transform( const ::basegfx::B2DHomMatrix& rMatrix )
        {
            OSL_ENSURE( mxSprite.is(), "ImplSprite::transform(): Invalid sprite");

            if( mxSprite.is() )
            {
                geometry::AffineMatrix2D aMatrix;

                mxSprite->transform( ::basegfx::unotools::affineMatrixFromHomMatrix( aMatrix,
                                                                                     rMatrix ) );
            }
        }

        void ImplSprite::setClipPixel( const ::basegfx::B2DPolyPolygon& rClipPoly )
        {
            OSL_ENSURE( mxGraphicDevice.is(), "ImplSprite::setClip(): Invalid canvas");
            OSL_ENSURE( mxSprite.is(), "ImplSprite::transform(): Invalid sprite");

            if( mxSprite.is() && mxGraphicDevice.is() )
            {
                mxSprite->clip( ::basegfx::unotools::xPolyPolygonFromB2DPolyPolygon( mxGraphicDevice,
                                                                                     rClipPoly ) );
            }
        }

        void ImplSprite::setClip( const ::basegfx::B2DPolyPolygon& rClipPoly )
        {
            OSL_ENSURE( mxGraphicDevice.is(), "ImplSprite::setClip(): Invalid canvas");
            OSL_ENSURE( mxSprite.is(), "ImplSprite::transform(): Invalid sprite");

            if( mxSprite.is() && mxGraphicDevice.is() )
            {
                ::basegfx::B2DPolyPolygon   aTransformedClipPoly( rClipPoly );

                // extract linear part of canvas view transformation (linear means:
                // without translational components)
                ::basegfx::B2DHomMatrix     aViewTransform( mpTransformArbiter->getTransformation() );
                aViewTransform.set( 0, 2, 0.0 );
                aViewTransform.set( 1, 2, 0.0 );

                // transform polygon from view to device coordinate space
                aTransformedClipPoly.transform( aViewTransform );

                mxSprite->clip( ::basegfx::unotools::xPolyPolygonFromB2DPolyPolygon( mxGraphicDevice,
                                                                                     aTransformedClipPoly ) );
            }
        }

        void ImplSprite::show()
        {
            OSL_ENSURE( mxSprite.is(), "ImplSprite::show(): Invalid sprite");

            if( mxSprite.is() )
                mxSprite->show();
        }

        void ImplSprite::hide()
        {
            OSL_ENSURE( mxSprite.is(), "ImplSprite::hide(): Invalid sprite");

            if( mxSprite.is() )
                mxSprite->hide();
        }

        uno::Reference< rendering::XSprite > ImplSprite::getUNOSprite() const
        {
            return mxSprite;
        }

        uno::Reference< rendering::XGraphicDevice > ImplSprite::getGraphicDevice() const
        {
            return mxGraphicDevice;
        }
    }
}
