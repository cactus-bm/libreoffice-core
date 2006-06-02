/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdrlatheprimitive3d.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: aw $ $Date: 2006-06-02 13:57:59 $
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

#ifndef _DRAWINGLAYER_PRIMITIVE3D_SDRLATHEPRIMITIVE3D_HXX
#define _DRAWINGLAYER_PRIMITIVE3D_SDRLATHEPRIMITIVE3D_HXX

#ifndef _DRAWINGLAYER_PRIMITIVE3D_SDRPRIMITIVE3D_HXX
#include <drawinglayer/primitive3d/sdrprimitive3d.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////
// predefines

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive
    {
        class sdrLathePrimitive3D : public sdrPrimitive3D
        {
        private:
            // create slices
            void impCreateSlices();

            // geometry helper for slices
            sliceVector                                 maSlices;

        protected:
            ::basegfx::B2DPolyPolygon                   maPolyPolygon;
            sal_uInt32                                  mnHorizontalSegments;
            sal_uInt32                                  mnVerticalSegments;
            double                                      mfDiagonal;
            double                                      mfBackScale;
            double                                      mfRotation;

            // bitfield
            unsigned                                    mbSmoothNormals : 1; // Plane self
            unsigned                                    mbSmoothHorizontalNormals : 1; // always
            unsigned                                    mbSmoothLids : 1; // Front/back
            unsigned                                    mbCharacterMode : 1;
            unsigned                                    mbCloseFront : 1;
            unsigned                                    mbCloseBack : 1;

            //  create decomposition
            virtual void decompose(primitiveVector& rTarget, const ::drawinglayer::geometry::viewInformation& rViewInformation);

            // get (evtl. create) slices
            const sliceVector& getSlices() const;

        public:
            sdrLathePrimitive3D(
                const ::basegfx::B3DHomMatrix& rTransform,
                const ::basegfx::B2DVector& rTextureSize,
                const sdrLineFillShadowAttribute& rSdrLFSAttribute,
                const sdr3DObjectAttribute& rSdr3DObjectAttribute,
                const ::basegfx::B2DPolyPolygon& rPolyPolygon,
                sal_uInt32 nHorizontalSegments,
                sal_uInt32 nVerticalSegments,
                double fDiagonal,
                double fBackScale,
                double fRotation,
                bool bSmoothNormals,
                bool bSmoothHorizontalNormals,
                bool bSmoothLids,
                bool bCharacterMode,
                bool bCloseFront,
                bool bCloseBack);
            virtual ~sdrLathePrimitive3D();

            // compare operator
            virtual bool operator==(const basePrimitive& rPrimitive) const;

            // id generator
            virtual PrimitiveID getID() const;

            // get 3D range of primitive.
            virtual ::basegfx::B3DRange get3DRange(const ::drawinglayer::geometry::viewInformation& rViewInformation) const;

            // data access
            const ::basegfx::B2DPolyPolygon& getPolyPolygon() const { return maPolyPolygon; }
            sal_uInt32 getHorizontalSegments() const { return mnHorizontalSegments; }
            sal_uInt32 getVerticalSegments() const { return mnVerticalSegments ; }
            double getDiagonal() const { return mfDiagonal; }
            double getBackScale() const { return mfBackScale; }
            double getRotation() const { return mfRotation; }
            bool getSmoothNormals() const { return mbSmoothNormals; }
            bool getSmoothHorizontalNormals() const { return mbSmoothHorizontalNormals; }
            bool getSmoothLids() const { return mbSmoothLids; }
            bool getCharacterMode() const { return mbCharacterMode; }
            bool getCloseFront() const { return mbCloseFront; }
            bool getCloseBack() const { return mbCloseBack; }
        };
    } // end of namespace overlay
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //_DRAWINGLAYER_PRIMITIVE3D_SDRLATHEPRIMITIVE3D_HXX

// eof
