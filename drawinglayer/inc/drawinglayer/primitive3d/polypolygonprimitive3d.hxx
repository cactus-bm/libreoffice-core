/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: polypolygonprimitive3d.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: aw $ $Date: 2006-06-02 13:57:58 $
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

#ifndef _DRAWINGLAYER_PRIMITIVE3D_POLYPOLYGONPRIMITIVE_HXX
#define _DRAWINGLAYER_PRIMITIVE3D_POLYPOLYGONPRIMITIVE_HXX

#ifndef _DRAWINGLAYER_PRIMITIVE_PRIMITIVE_HXX
#include <drawinglayer/primitive/primitive.hxx>
#endif

#ifndef _DRAWINGLAYER_PRIMITIVE3D_MATERIALATTRIBUTE3D_HXX
#include <drawinglayer/primitive3d/materialattribute3d.hxx>
#endif

#ifndef _BGFX_POLYGON_B3DPOLYPOLYGON_HXX
#include <basegfx/polygon/b3dpolypolygon.hxx>
#endif

#ifndef _DRAWINGLAYER_PRIMITIVE_TEXTURE_HXX
#include <drawinglayer/primitive/texture.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive
    {
        class polyPolygonMaterialPrimitive3D : public basePrimitive
        {
        protected:
            ::basegfx::B3DPolyPolygon                   maPolyPolygon;
            materialAttribute3D                         maMaterial;

            // bitfield
            unsigned                                    mbDoubleSided : 1;

        public:
            polyPolygonMaterialPrimitive3D(const ::basegfx::B3DPolyPolygon& rPolyPolygon, const materialAttribute3D& rMaterial, bool bDoubleSided);
            virtual ~polyPolygonMaterialPrimitive3D();

            // get data
            const ::basegfx::B3DPolyPolygon& getB3DPolyPolygon() const { return maPolyPolygon; }
            const materialAttribute3D& getMaterial() const { return maMaterial; }
            bool getDoubleSided() const { return mbDoubleSided; }

            // compare operator
            virtual bool operator==(const basePrimitive& rPrimitive) const;

            // id generator
            virtual PrimitiveID getID() const;

            // get 3Drange of primitive. Default implementation uses decomposition
            virtual ::basegfx::B3DRange get3DRange(const ::drawinglayer::geometry::viewInformation& rViewInformation) const;
        };
    } // end of namespace primitive
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //_DRAWINGLAYER_PRIMITIVE3D_POLYPOLYGONPRIMITIVE_HXX

// eof
