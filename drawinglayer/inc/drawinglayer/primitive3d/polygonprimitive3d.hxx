/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: polygonprimitive3d.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: aw $ $Date: 2006-11-07 15:49:06 $
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

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE3D_POLYGONPRIMITIVE3D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE3D_POLYGONPRIMITIVE3D_HXX

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE3D_BASEPRIMITIVE3D_HXX
#include <drawinglayer/primitive3d/baseprimitive3d.hxx>
#endif

#ifndef _BGFX_COLOR_BCOLOR_HXX
#include <basegfx/color/bcolor.hxx>
#endif

#ifndef _BGFX_POLYGON_B3DPOLYGON_HXX
#include <basegfx/polygon/b3dpolygon.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_ATTRIBUTE_STROKEATTRIBUTE_HXX
#include <drawinglayer/attribute/strokeattribute.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive3d
    {
        class PolygonHairlinePrimitive3D : public BasePrimitive3D
        {
        private:
            basegfx::B3DPolygon                     maPolygon;
            basegfx::BColor                         maBColor;

        public:
            PolygonHairlinePrimitive3D(
                const basegfx::B3DPolygon& rPolygon,
                const basegfx::BColor& rBColor);

            // get data
            const basegfx::B3DPolygon& getB3DPolygon() const { return maPolygon; }
            const basegfx::BColor& getBColor() const { return maBColor; }

            // compare operator
            virtual bool operator==(const BasePrimitive3D& rPrimitive) const;

            // get range
            virtual basegfx::B3DRange getB3DRange(double fTime) const;

            // provide unique ID
            DeclPrimitrive3DIDBlock()
        };
    } // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive3d
    {
        class PolygonStrokePrimitive3D : public BasePrimitive3D
        {
        private:
            basegfx::B3DPolygon                     maPolygon;
            attribute::StrokeAttribute              maStrokeAttribute;

        protected:
            // local decomposition.
            virtual Primitive3DSequence createLocalDecomposition(double fTime) const;

        public:
            PolygonStrokePrimitive3D(
                const basegfx::B3DPolygon& rPolygon,
                const attribute::StrokeAttribute& rStrokeAttribute);

            // get data
            basegfx::B3DPolygon getB3DPolygon() const { return maPolygon; }
            const attribute::StrokeAttribute& getStrokeAttribute() const { return maStrokeAttribute; }

            // compare operator
            virtual bool operator==(const BasePrimitive3D& rPrimitive) const;

            // provide unique ID
            DeclPrimitrive3DIDBlock()
        };
    } // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE3D_POLYGONPRIMITIVE3D_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
