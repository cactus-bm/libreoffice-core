/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: sdrconnectorprimitive2d.cxx,v $
 *
 * $Revision: 1.2 $
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

#include <svx/sdr/primitive2d/sdrconnectorprimitive2d.hxx>
#include <svx/sdr/primitive2d/sdrdecompositiontools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <drawinglayer/primitive2d/groupprimitive2d.hxx>
#include <svx/sdr/primitive2d/svx_primitivetypes2d.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        Primitive2DSequence SdrConnectorPrimitive2D::createLocalDecomposition(const geometry::ViewInformation2D& /*aViewInformation*/) const
        {
            Primitive2DSequence aRetval;

            // prepare unit transform
            ::basegfx::B2DHomMatrix aEmptyMatrix;

            // add line
            if(getSdrLSTAttribute().getLine())
            {
                appendPrimitive2DReferenceToPrimitive2DSequence(aRetval, createPolygonLinePrimitive(getUnitPolygon(), aEmptyMatrix, *getSdrLSTAttribute().getLine(), getSdrLSTAttribute().getLineStartEnd()));
            }

            // add text
            if(getSdrLSTAttribute().getText())
            {
                appendPrimitive2DReferenceToPrimitive2DSequence(aRetval, createTextPrimitive(::basegfx::B2DPolyPolygon(getUnitPolygon()), aEmptyMatrix, *getSdrLSTAttribute().getText(), getSdrLSTAttribute().getLine(), false));
            }

            // add shadow
            if(getSdrLSTAttribute().getShadow())
            {
                const Primitive2DReference xShadow(createShadowPrimitive(aRetval, *getSdrLSTAttribute().getShadow()));

                if(xShadow.is())
                {
                    Primitive2DSequence aContentWithShadow(2L);
                    aContentWithShadow[0L] = xShadow;
                    aContentWithShadow[1L] = Primitive2DReference(new GroupPrimitive2D(aRetval));
                    aRetval = aContentWithShadow;
                }
            }

            return aRetval;
        }

        SdrConnectorPrimitive2D::SdrConnectorPrimitive2D(
            const attribute::SdrLineShadowTextAttribute& rSdrLSTAttribute,
            const ::basegfx::B2DPolygon& rUnitPolygon)
        :   BasePrimitive2D(),
            maSdrLSTAttribute(rSdrLSTAttribute),
            maUnitPolygon(rUnitPolygon)
        {
        }

        bool SdrConnectorPrimitive2D::operator==(const BasePrimitive2D& rPrimitive) const
        {
            if(BasePrimitive2D::operator==(rPrimitive))
            {
                const SdrConnectorPrimitive2D& rCompare = (SdrConnectorPrimitive2D&)rPrimitive;

                return (getUnitPolygon() == rCompare.getUnitPolygon()
                    && getSdrLSTAttribute() == rCompare.getSdrLSTAttribute());
            }

            return false;
        }

        // provide unique ID
        ImplPrimitrive2DIDBlock(SdrConnectorPrimitive2D, PRIMITIVE2D_ID_SDRCONNECTORPRIMITIVE2D)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
