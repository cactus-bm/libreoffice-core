/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdrextrudelathetools3d.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: aw $ $Date: 2006-05-19 09:34:49 $
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

#ifndef _DRAWINGLAYER_PRIMITIVE3D_SDREXTRUDELATHETOOLS3D_HXX
#define _DRAWINGLAYER_PRIMITIVE3D_SDREXTRUDELATHETOOLS3D_HXX

#include <vector>

#ifndef _BGFX_POLYGON_B3DPOLYPOLYGON_HXX
#include <basegfx/polygon/b3dpolypolygon.hxx>
#endif

#ifndef _BGFX_POLYPOLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////
// predefines

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive
    {
        // slice types
        enum sliceType
        {
            SLICETYPE_REGULAR,      // normal geoemtry slice
            SLICETYPE_FRONTCAP,     // front cap
            SLICETYPE_BACKCAP       // back cap
        };

        // class to hold one 3D slice
        class slice
        {
        protected:
            ::basegfx::B3DPolyPolygon                   maPolyPolygon;
            sliceType                                   maSliceType;

        public:
            slice(const ::basegfx::B2DPolyPolygon& rPolyPolygon, const ::basegfx::B3DHomMatrix& aTransform, sliceType aSliceType = SLICETYPE_REGULAR)
            :   maPolyPolygon(::basegfx::tools::createB3DPolyPolygonFromB2DPolyPolygon(rPolyPolygon)),
                maSliceType(aSliceType)
            {
                maPolyPolygon.transform(aTransform);
            }

            slice(const ::basegfx::B3DPolyPolygon& rPolyPolygon, sliceType aSliceType = SLICETYPE_REGULAR)
            :   maPolyPolygon(rPolyPolygon),
                maSliceType(aSliceType)
            {
            }

            // data access
            const ::basegfx::B3DPolyPolygon& getB3DPolyPolygon() const { return maPolyPolygon; }
            sliceType getSliceType() const { return maSliceType; }
        };

        // typedef for a group of slices
        typedef ::std::vector< slice > sliceVector;

        // helpers for creation
        void createLatheSlices(sliceVector& rSliceVector, const ::basegfx::B2DPolyPolygon& rSource,
            double fBackScale, double fDiagonal, double fRotation, sal_uInt32 nSteps,
            bool bCharacterMode, bool bCloseFront, bool bCloseBack);
        void createExtrudeSlices(sliceVector& rSliceVector, const ::basegfx::B2DPolyPolygon& rSource,
            double fBackScale, double fDiagonal, double fDepth,
            bool bCharacterMode, bool bCloseFront, bool bCloseBack);

        // helpers for geometry extraction
        void extractLinesFromSlice(::basegfx::B3DPolyPolygon& rLine, const sliceVector& rSliceVector, bool bClosed);
        void extractPlanesFromSlice(::std::vector< ::basegfx::B3DPolyPolygon >& rFill, const sliceVector& rSliceVector,
            bool bCreateNormals, bool bSmoothHorizontalNormals, bool bSmoothNormals, bool bSmoothLids, bool bClosed,
            double fSmoothNormalsMix, double fSmoothLidsMix, bool bCreateTextureCoordinates, const ::basegfx::B2DHomMatrix& rTexTransform);

    } // end of namespace overlay
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //_DRAWINGLAYER_PRIMITIVE3D_SDREXTRUDELATHETOOLS3D_HXX

// eof
