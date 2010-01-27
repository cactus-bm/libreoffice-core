/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: shadowprimitive2d.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: aw $ $Date: 2008-05-27 14:11:20 $
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
#include "precompiled_drawinglayer.hxx"

#include <drawinglayer/primitive2d/shadowprimitive2d.hxx>
#include <basegfx/color/bcolormodifier.hxx>
#include <drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <drawinglayer/primitive2d/transparenceprimitive2d.hxx>
#include <drawinglayer/primitive2d/UnifiedTransparencePrimitive2D.hxx>
#include <drawinglayer/primitive2d/drawinglayer_primitivetypes2d.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        ShadowPrimitive2D::ShadowPrimitive2D(
            const basegfx::B2DHomMatrix& rShadowTransform,
            const basegfx::BColor& rShadowColor,
            const Primitive2DSequence& rChildren)
        :   GroupPrimitive2D(rChildren),
            maShadowTransform(rShadowTransform),
            maShadowColor(rShadowColor)
        {
        }

        bool ShadowPrimitive2D::operator==(const BasePrimitive2D& rPrimitive) const
        {
            if(BasePrimitive2D::operator==(rPrimitive))
            {
                const ShadowPrimitive2D& rCompare = static_cast< const ShadowPrimitive2D& >(rPrimitive);

                return (getShadowTransform() == rCompare.getShadowTransform()
                    && getShadowColor() == rCompare.getShadowColor());
            }

            return false;
        }

        basegfx::B2DRange ShadowPrimitive2D::getB2DRange(const geometry::ViewInformation2D& rViewInformation) const
        {
            basegfx::B2DRange aRetval(getB2DRangeFromPrimitive2DSequence(getChildren(), rViewInformation));
            aRetval.transform(getShadowTransform());
            return aRetval;
        }

        Primitive2DSequence ShadowPrimitive2D::get2DDecomposition(const geometry::ViewInformation2D& /*rViewInformation*/) const
        {
            Primitive2DSequence aRetval;

            if(getChildren().hasElements())
            {
                // create a modifiedColorPrimitive containing the shadow color and the content
                const basegfx::BColorModifier aBColorModifier(getShadowColor());
                const Primitive2DReference xRefA(new ModifiedColorPrimitive2D(getChildren(), aBColorModifier));
                const Primitive2DSequence aSequenceB(&xRefA, 1L);

                // build transformed primitiveVector with shadow offset and add to target
                const Primitive2DReference xRefB(new TransformPrimitive2D(getShadowTransform(), aSequenceB));
                aRetval = Primitive2DSequence(&xRefB, 1L);
            }

            return aRetval;
        }

        // provide unique ID
        ImplPrimitrive2DIDBlock(ShadowPrimitive2D, PRIMITIVE2D_ID_SHADOWPRIMITIVE2D)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
