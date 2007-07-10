/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: vclprocessor2d.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: aw $ $Date: 2007-07-10 11:28:08 $
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

#ifndef INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLPROCESSOR2D_HXX
#define INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLPROCESSOR2D_HXX

#ifndef INCLUDED_DRAWINGLAYER_PROCESSOR2D_BASEPROCESSOR2D_HXX
#include <drawinglayer/processor2d/baseprocessor2d.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

#ifndef _BGFX_COLOR_BCOLORMODIFIER_HXX
#include <basegfx/color/bcolormodifier.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////
// predefines
class OutputDevice;

namespace basegfx {
    class BColor;
}

namespace drawinglayer { namespace primitive2d {
    class TextSimplePortionPrimitive2D;
    class PolygonHairlinePrimitive2D;
    class BitmapPrimitive2D;
    class FillBitmapPrimitive2D;
    class PolyPolygonGradientPrimitive2D;
    class PolyPolygonColorPrimitive2D;
    class MetafilePrimitive2D;
    class MaskPrimitive2D;
    class AlphaPrimitive2D;
    class TransformPrimitive2D;
    class MarkerArrayPrimitive2D;
    class ModifiedColorPrimitive2D;
}}

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace processor2d
    {
        class VclProcessor2D : public BaseProcessor2D
        {
        protected:
            // the destination OutDev
            OutputDevice*                                           mpOutputDevice;

            // the modifiedColorPrimitive stack
            basegfx::BColorModifierStack                            maBColorModifierStack;

            // the current transformation
            basegfx::B2DHomMatrix                                   maCurrentTransformation;

            //////////////////////////////////////////////////////////////////////////////
            // rendering support
            void RenderTextSimpleOrDecoratedPortionPrimitive2D(const primitive2d::TextSimplePortionPrimitive2D& rTextCandidate);
            void RenderPolygonHairlinePrimitive2D(const primitive2d::PolygonHairlinePrimitive2D& rPolygonCandidate);
            void RenderBitmapPrimitive2D(const primitive2d::BitmapPrimitive2D& rBitmapCandidate);
            void RenderFillBitmapPrimitive2D(const primitive2d::FillBitmapPrimitive2D& rFillBitmapCandidate);
            void RenderPolyPolygonGradientPrimitive2D(const primitive2d::PolyPolygonGradientPrimitive2D& rPolygonCandidate);
            void RenderPolyPolygonColorPrimitive2D(const primitive2d::PolyPolygonColorPrimitive2D& rPolygonCandidate);
            void RenderMetafilePrimitive2D(const primitive2d::MetafilePrimitive2D& rPolygonCandidate);
            void RenderMaskPrimitive2D(const primitive2d::MaskPrimitive2D& rMaskCandidate);
            void RenderModifiedColorPrimitive2D(const primitive2d::ModifiedColorPrimitive2D& rModifiedCandidate);
            void RenderAlphaPrimitive2D(const primitive2d::AlphaPrimitive2D& rTransCandidate);
            void RenderTransformPrimitive2D(const primitive2d::TransformPrimitive2D& rTransformCandidate);
            void RenderMarkerArrayPrimitive2D(const primitive2d::MarkerArrayPrimitive2D& rMarkerArrayCandidate);

            // as tooling, the process() implementation takes over API handling and calls this
            // virtual render method when the primitive implementation is BasePrimitive2D-based.
            virtual void processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate) = 0;

        public:
            // constructor/destructor
            VclProcessor2D(
                const geometry::ViewInformation2D& rViewInformation,
                OutputDevice& rOutDev);
            virtual ~VclProcessor2D();

            // the central processing method
            // This VCL base implementation takes over the API handling and calls processBasePrimitive2D
            // directly when it's a BasePrinitive2D implementation. This is used as tooling from derived
            // implementations
            virtual void process(const primitive2d::Primitive2DSequence& rSource);
        };
    } // end of namespace processor2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace processor2d
    {
        class VclMetafileProcessor2D : public VclProcessor2D
        {
        protected:
            // the local processor for BasePrinitive2D-Implementation based primitives,
            // called from the common process()-implementation
            virtual void processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate);

        public:
            // constructor/destructor
            VclMetafileProcessor2D(
                const geometry::ViewInformation2D& rViewInformation,
                OutputDevice& rOutDev);
            virtual ~VclMetafileProcessor2D();
        };
    } // end of namespace processor2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace processor2d
    {
        class VclPixelProcessor2D : public VclProcessor2D
        {
        protected:
            // the local processor for BasePrinitive2D-Implementation based primitives,
            // called from the common process()-implementation
            virtual void processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate);

        public:
            // constructor/destructor
            VclPixelProcessor2D(
                const geometry::ViewInformation2D& rViewInformation,
                OutputDevice& rOutDev);
            virtual ~VclPixelProcessor2D();

            // overloaded here to reset the MapMode at the target OutDev
            virtual void process(const primitive2d::Primitive2DSequence& rSource);
        };
    } // end of namespace processor2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif // INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLPROCESSOR2D_HXX

// eof
