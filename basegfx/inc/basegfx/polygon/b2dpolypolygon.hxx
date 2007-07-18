/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: b2dpolypolygon.hxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-18 11:03:16 $
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

#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#define _BGFX_POLYGON_B2DPOLYPOLYGON_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef INCLUDED_O3TL_COW_WRAPPER_HXX
#include <o3tl/cow_wrapper.hxx>
#endif

// predeclarations
class ImplB2DPolyPolygon;

namespace basegfx
{
    class B2DPolygon;
    class B2DHomMatrix;
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    class B2DPolyPolygon
    {
    public:
        typedef o3tl::cow_wrapper< ImplB2DPolyPolygon > ImplType;

    private:
        ImplType                                        mpPolyPolygon;

    public:
        B2DPolyPolygon();
        B2DPolyPolygon(const B2DPolyPolygon& rPolyPolygon);
        explicit B2DPolyPolygon(const B2DPolygon& rPolygon);
        ~B2DPolyPolygon();

        // assignment operator
        B2DPolyPolygon& operator=(const B2DPolyPolygon& rPolyPolygon);

        /// unshare this poly-polygon (and all included polygons) with all internally shared instances
        void makeUnique();

        // compare operators
        bool operator==(const B2DPolyPolygon& rPolyPolygon) const;
        bool operator!=(const B2DPolyPolygon& rPolyPolygon) const;

        // polygon interface
        sal_uInt32 count() const;

        B2DPolygon getB2DPolygon(sal_uInt32 nIndex) const;
        void setB2DPolygon(sal_uInt32 nIndex, const B2DPolygon& rPolygon);

        // test for curve
        bool areControlPointsUsed() const;

        // insert/append single polygon
        void insert(sal_uInt32 nIndex, const B2DPolygon& rPolygon, sal_uInt32 nCount = 1);
        void append(const B2DPolygon& rPolygon, sal_uInt32 nCount = 1);

        // insert/append multiple polygons
        void insert(sal_uInt32 nIndex, const B2DPolyPolygon& rPolyPolygon);
        void append(const B2DPolyPolygon& rPolyPolygon);

        // remove
        void remove(sal_uInt32 nIndex, sal_uInt32 nCount = 1);

        // reset to empty state
        void clear();

        // closed state
        bool isClosed() const;
        void setClosed(bool bNew);

        // flip polygon direction
        void flip();

        // test if PolyPolygon has double points
        bool hasDoublePoints() const;

        // remove double points, at the begin/end and follow-ups, too
        void removeDoublePoints();

        // apply transformation given in matrix form to the polygon
        void transform(const ::basegfx::B2DHomMatrix& rMatrix);
    };
} // end of namespace basegfx

#endif /* _BGFX_POLYGON_B2DPOLYPOLYGON_HXX */
