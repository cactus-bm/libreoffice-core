/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: b2drange.hxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: ihi $ $Date: 2006-11-14 14:06:16 $
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

#ifndef _BGFX_RANGE_B2DRANGE_HXX
#define _BGFX_RANGE_B2DRANGE_HXX

#ifndef _BGFX_VECTOR_B2DVECTOR_HXX
#include <basegfx/vector/b2dvector.hxx>
#endif
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_TUPLE_B2DTUPLE_HXX
#include <basegfx/tuple/b2dtuple.hxx>
#endif

#ifndef _BGFX_RANGE_BASICRANGE_HXX
#include <basegfx/range/basicrange.hxx>
#endif

#include <vector>


namespace basegfx
{
    // predeclarations
    class B2IRange;

    class B2DRange
    {
    public:
        typedef double          ValueType;
        typedef DoubleTraits    TraitsType;

        B2DRange()
        {
        }

        explicit B2DRange(const B2DTuple& rTuple)
        :   maRangeX(rTuple.getX()),
            maRangeY(rTuple.getY())
        {
        }

        B2DRange(double x1,
                 double y1,
                 double x2,
                 double y2)
        :   maRangeX(x1),
            maRangeY(y1)
        {
            maRangeX.expand(x2);
            maRangeY.expand(y2);
        }

        B2DRange(const B2DTuple& rTuple1,
                 const B2DTuple& rTuple2)
        :   maRangeX(rTuple1.getX()),
            maRangeY(rTuple1.getY())
        {
            expand( rTuple2 );
        }

        B2DRange(const B2DRange& rRange)
        :   maRangeX(rRange.maRangeX),
            maRangeY(rRange.maRangeY)
        {
        }

        explicit B2DRange(const B2IRange& rRange);

        bool isEmpty() const
        {
            return (
                maRangeX.isEmpty()
                || maRangeY.isEmpty()
                );
        }

        void reset()
        {
            maRangeX.reset();
            maRangeY.reset();
        }

        bool operator==( const B2DRange& rRange ) const
        {
            return (maRangeX == rRange.maRangeX
                && maRangeY == rRange.maRangeY);
        }

        bool operator!=( const B2DRange& rRange ) const
        {
            return (maRangeX != rRange.maRangeX
                || maRangeY != rRange.maRangeY);
        }

        void operator=(const B2DRange& rRange)
        {
            maRangeX = rRange.maRangeX;
            maRangeY = rRange.maRangeY;
        }

        bool equal(const B2DRange& rRange) const
        {
            return (maRangeX.equal(rRange.maRangeX)
                    && maRangeY.equal(rRange.maRangeY));
        }

        bool equal(const B2DRange& rRange, const double& rfSmallValue) const
        {
            return (maRangeX.equal(rRange.maRangeX,rfSmallValue)
                    && maRangeY.equal(rRange.maRangeY,rfSmallValue));
        }

        double getMinX() const
        {
            return maRangeX.getMinimum();
        }

        double getMinY() const
        {
            return maRangeY.getMinimum();
        }

        double getMaxX() const
        {
            return maRangeX.getMaximum();
        }

        double getMaxY() const
        {
            return maRangeY.getMaximum();
        }

        double getWidth() const
        {
            return maRangeX.getRange();
        }

        double getHeight() const
        {
            return maRangeY.getRange();
        }

        B2DPoint getMinimum() const
        {
            return B2DPoint(
                maRangeX.getMinimum(),
                maRangeY.getMinimum()
                );
        }

        B2DPoint getMaximum() const
        {
            return B2DPoint(
                maRangeX.getMaximum(),
                maRangeY.getMaximum()
                );
        }

        B2DVector getRange() const
        {
            return B2DVector(
                maRangeX.getRange(),
                maRangeY.getRange()
                );
        }

        B2DPoint getCenter() const
        {
            return B2DPoint(
                maRangeX.getCenter(),
                maRangeY.getCenter()
                );
        }

        double getCenterX() const
        {
            return maRangeX.getCenter();
        }

        double getCenterY() const
        {
            return maRangeY.getCenter();
        }

        bool isInside(const B2DTuple& rTuple) const
        {
            return (
                maRangeX.isInside(rTuple.getX())
                && maRangeY.isInside(rTuple.getY())
                );
        }

        bool isInside(const B2DRange& rRange) const
        {
            return (
                maRangeX.isInside(rRange.maRangeX)
                && maRangeY.isInside(rRange.maRangeY)
                );
        }

        bool overlaps(const B2DRange& rRange) const
        {
            return (
                maRangeX.overlaps(rRange.maRangeX)
                && maRangeY.overlaps(rRange.maRangeY)
                );
        }

        void expand(const B2DTuple& rTuple)
        {
            maRangeX.expand(rTuple.getX());
            maRangeY.expand(rTuple.getY());
        }

        void expand(const B2DRange& rRange)
        {
            maRangeX.expand(rRange.maRangeX);
            maRangeY.expand(rRange.maRangeY);
        }

        void intersect(const B2DRange& rRange)
        {
            maRangeX.intersect(rRange.maRangeX);
            maRangeY.intersect(rRange.maRangeY);
        }

        void grow(double fValue)
        {
            maRangeX.grow(fValue);
            maRangeY.grow(fValue);
        }

        void transform(const B2DHomMatrix& rMatrix);

    private:
        typedef ::basegfx::BasicRange< ValueType, TraitsType >  MyBasicRange;

        MyBasicRange        maRangeX;
        MyBasicRange        maRangeY;
    };

    /** Round double to nearest integer for 2D range

        @return the nearest integer for this range
    */
    B2IRange fround(const B2DRange& rRange);

    /** Compute the set difference of the two given ranges

        This method calculates the symmetric difference (aka XOR)
        between the two given ranges, and returning the resulting
        ranges. Thus, the result will contain all areas where one, but
        not both ranges lie.

        @param o_rResult
        Result vector. The up to four difference ranges are returned
        within this vector

        @param rFirst
        The first range

        @param rSecond
        The second range

        @return the input vector
     */
    ::std::vector< B2DRange >& computeSetDifference( ::std::vector< B2DRange >& o_rResult,
                                                     const B2DRange&            rFirst,
                                                     const B2DRange&            rSecond );

} // end of namespace basegfx


#endif /* _BGFX_RANGE_B2DRANGE_HXX */
