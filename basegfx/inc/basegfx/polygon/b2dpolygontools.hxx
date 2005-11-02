/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: b2dpolygontools.hxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: kz $ $Date: 2005-11-02 13:53:36 $
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

#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#define _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif

#ifndef _BGFX_VECTOR_B2DVECTOR_HXX
#include <basegfx/vector/b2dvector.hxx>
#endif

#ifndef _BGFX_RANGE_B2DRECTANGLE_HXX
#include <basegfx/range/b2drectangle.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif

#ifndef _BGFX_POLYGON_B3DPOLYGON_HXX
#include <basegfx/polygon/b3dpolygon.hxx>
#endif

#include <vector>

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    // predefinitions
    class B2DPolygon;
    class B2DRange;

    namespace tools
    {
        // B2DPolygon tools

        /** Check if given polygon is closed.

            This is kind of a 'classic' method to support old polygon
            definitions.  Those old polygon definitions define the
            closed state of the polygon using identical start and
            endpoints. This method corrects this (removes double
            start/end points) and sets the Closed()-state of the
            polygon correctly.
        */
        void checkClosed(B2DPolygon& rCandidate);

        // Get successor and predecessor indices. Returning the same index means there
        // is none. Same for successor.
        sal_uInt32 getIndexOfPredecessor(sal_uInt32 nIndex, const B2DPolygon& rCandidate);
        sal_uInt32 getIndexOfSuccessor(sal_uInt32 nIndex, const B2DPolygon& rCandidate);

        // Get orientation of Polygon
        B2VectorOrientation getOrientation(const B2DPolygon& rCandidate);

        // isInside tests for B2dPoint and other B2dPolygon. On border is not inside as long as
        // not true is given in bWithBorder flag.
        bool isInside(const B2DPolygon& rCandidate, const B2DPoint& rPoint, bool bWithBorder = false);
        bool isInside(const B2DPolygon& rCandidate, const B2DPolygon& rPolygon, bool bWithBorder = false);

        // get size of polygon. Control vectors are included in that ranges.
        B2DRange getRange(const B2DPolygon& rCandidate);

        // get signed area of polygon
        double getSignedArea(const B2DPolygon& rCandidate);

        // get area of polygon
        double getArea(const B2DPolygon& rCandidate);

        // get length of polygon edge from point nIndex to nIndex + 1
        double getEdgeLength(const B2DPolygon& rCandidate, sal_uInt32 nIndex);

        // get length of polygon
        double getLength(const B2DPolygon& rCandidate);

        // get position on polygon for absolute given distance. If
        // length is given, it is assumed the correct polygon length, if 0.0 it is calculated
        // using getLength(...)
        B2DPoint getPositionAbsolute(const B2DPolygon& rCandidate, double fDistance, double fLength = 0.0);

        // get position on polygon for relative given distance in range [0.0 .. 1.0]. If
        // length is given, it is assumed the correct polygon length, if 0.0 it is calculated
        // using getLength(...)
        B2DPoint getPositionRelative(const B2DPolygon& rCandidate, double fDistance, double fLength = 0.0);

        // get a snippet from given polygon for absolute distances. The polygon is assumed
        // to be opened (not closed). fFrom and fTo need to be in range [0.0 .. fLength], where
        // fTo >= fFrom. If length is given, it is assumed the correct polygon length,
        // if 0.0 it is calculated using getLength(...)
        B2DPolygon getSnippetAbsolute(const B2DPolygon& rCandidate, double fFrom, double fTo, double fLength = 0.0);

        // get a snippet from given polygon for relative distances. The polygon is assumed
        // to be opened (not closed). fFrom and fTo need to be in range [0.0 .. 1.0], where
        // fTo >= fFrom. If length is given, it is assumed the correct polygon length,
        // if 0.0 it is calculated using getLength(...)
        B2DPolygon getSnippetRelative(const B2DPolygon& rCandidate, double fFrom = 0.0, double fTo = 1.0, double fLength = 0.0);

        // Continuity check for point with given index
        B2VectorContinuity getContinuityInPoint(const B2DPolygon& rCandidate, sal_uInt32 nIndex);

        // remove intersections
        B2DPolyPolygon removeIntersections(const B2DPolygon& rCandidate, bool bKeepOrientations = true);

        // Subdivide all contained curves. Use distanceBound value if given.
        B2DPolygon adaptiveSubdivideByDistance(const B2DPolygon& rCandidate, double fDistanceBound = 0.0);

        // Subdivide all contained curves. Use angleBound value if given.
        B2DPolygon adaptiveSubdivideByAngle(const B2DPolygon& rCandidate, double fAngleBound = 0.0);

        // #i37443# Subdivide all contained curves. Use distanceBound value if given.
        B2DPolygon adaptiveSubdivideByCount(const B2DPolygon& rCandidate, sal_uInt32 nCount = 0L);

        // Definitions for the cut flags used from the findCut methods
        typedef sal_uInt16 CutFlagValue;

        #define CUTFLAG_NONE            (0x0000)
        #define CUTFLAG_LINE            (0x0001)
        #define CUTFLAG_START1          (0x0002)
        #define CUTFLAG_START2          (0x0004)
        #define CUTFLAG_END1            (0x0008)
        #define CUTFLAG_END2            (0x0010)
        #define CUTFLAG_ALL         (CUTFLAG_LINE|CUTFLAG_START1|CUTFLAG_START2|CUTFLAG_END1|CUTFLAG_END2)
        #define CUTFLAG_DEFAULT     (CUTFLAG_LINE|CUTFLAG_START2|CUTFLAG_END2)

        // Calculate cut between the points given by the two indices. pCut1
        // and pCut2 will contain the cut coordinate on each edge in ]0.0, 1.0]
        // (if given) and the return value will contain a cut description.
        CutFlagValue findCut(
            const B2DPolygon& rCandidate,
            sal_uInt32 nIndex1, sal_uInt32 nIndex2,
            CutFlagValue aCutFlags = CUTFLAG_DEFAULT,
            double* pCut1 = 0L, double* pCut2 = 0L);

        // This version is working with two indexed edges from different
        // polygons.
        CutFlagValue findCut(
            const B2DPolygon& rCandidate1, sal_uInt32 nIndex1,
            const B2DPolygon& rCandidate2, sal_uInt32 nIndex2,
            CutFlagValue aCutFlags = CUTFLAG_DEFAULT,
            double* pCut1 = 0L, double* pCut2 = 0L);

        // This version works with two points and vectors to define the
        // edges for the cut test.
        CutFlagValue findCut(
            const B2DPoint& rEdge1Start, const B2DVector& rEdge1Delta,
            const B2DPoint& rEdge2Start, const B2DVector& rEdge2Delta,
            CutFlagValue aCutFlags = CUTFLAG_DEFAULT,
            double* pCut1 = 0L, double* pCut2 = 0L);

        // test if point is on the given edge in range ]0.0..1.0[ without
        // the start/end points. If so, return true and put the parameter
        // value in pCut (if provided)
        bool isPointOnEdge(
            const B2DPoint& rPoint,
            const B2DPoint& rEdgeStart,
            const B2DVector& rEdgeDelta,
            double* pCut = 0L);

        // Apply Line Dashing. This cuts the Polygon into line pieces
        // which are inserted as single polygons into the result.
        // If fFullDashDotLen is not given it will be calculated from the given
        // raDashDotArray.
        B2DPolyPolygon applyLineDashing(const B2DPolygon& rCandidate, const ::std::vector<double>& raDashDotArray, double fFullDashDotLen = 0.0);

        // test if point is inside epsilon-range around an edge defined
        // by the two given points. Can be used for HitTesting. The epsilon-range
        // is defined to be the rectangle centered to the given edge, using height
        // 2 x fDistance, and the circle around both points with radius fDistance.
        bool isInEpsilonRange(const B2DPoint& rEdgeStart, const B2DPoint& rEdgeEnd, const B2DPoint& rTestPosition, double fDistance);

        // test if point is inside epsilon-range around the given Polygon. Can be used
        // for HitTesting. The epsilon-range is defined to be the tube around the polygon
        // with distance fDistance and rounded edges (start and end point).
        bool isInEpsilonRange(const B2DPolygon& rCandidate, const B2DPoint& rTestPosition, double fDistance);

        /** Create a polygon from a rectangle.

            @param rRect
            The rectangle which describes the polygon size

            @param fRadius
            Radius of the edge rounding, relative to the rectangle size. 0.0 means no
            rounding, 1.0 will lead to an ellipse
         */
        B2DPolygon createPolygonFromRect( const B2DRectangle& rRect, double fRadius );

        /** Create a polygon from a rectangle.

            @param rRect
            The rectangle which describes the polygon size

            @param fRadiusX
            @param fRadiusY
            Radius of the edge rounding, relative to the rectangle size. 0.0 means no
            rounding, 1.0 will lead to an ellipse
         */
        B2DPolygon createPolygonFromRect( const B2DRectangle& rRect, double fRadiusX, double fRadiusY );

        /** Create a polygon from a rectangle.
         */
        B2DPolygon createPolygonFromRect( const B2DRectangle& rRect );

        /** Create a circle polygon with given radius.

            This method creates a circle approximation consisting of
            four cubic bezier segments, which approximate the given
            circle with an error of less than 0.5 percent.

            @param rCenter
            Center point of the circle

            @param fRadius
            Radius of the circle
         */
        B2DPolygon createPolygonFromCircle( const B2DPoint& rCenter, double fRadius );

        /** append a unit circle with one point and the control vectors to the given polygon
         */
        void appendUnitCircleQuadrant(B2DPolygon& rPolygon, sal_uInt32 nQuadrant, bool bEndPoint);

        /** append a segment of unit circle with one point and the control vectors to the given polygon
         */
        void appendUnitCircleQuadrantSegment(B2DPolygon& rPolygon, sal_uInt32 nQuadrant, double fStart, double fEnd, bool bEndPoint);

        /** create a polygon which describes the unit circle and close it
         */
        B2DPolygon createPolygonFromUnitCircle();

        /** Create an ellipse polygon with given radii.

            This method creates an ellipse approximation consisting of
            four cubic bezier segments, which approximate the given
            ellipse with an error of less than 0.5 percent.

            @param rCenter
            Center point of the circle

            @param fRadiusX
            Radius of the ellipse in X direction

            @param fRadiusY
            Radius of the ellipse in Y direction
         */
        B2DPolygon createPolygonFromEllipse( const B2DPoint& rCenter, double fRadiusX, double fRadiusY );

        /** Create an ellipse polygon with given radii and the given angles, from start to end

            This method creates an ellipse approximation consisting of
            four cubic bezier segments, which approximate the given
            ellipse with an error of less than 0.5 percent.

            @param rCenter
            Center point of the circle

            @param fRadiusX
            Radius of the ellipse in X direction

            @param fRadiusY
            Radius of the ellipse in Y direction

            @param fStart
            Start angle where the ellipe segment starts in the range [0.0 .. 2PI[

            @param fEnd
            End angle where the ellipe segment ends in the range [0.0 .. 2PI[
         */

        /** Create an unit ellipse polygon with the given angles, from start to end
         */
        B2DPolygon createPolygonFromEllipseSegment( const B2DPoint& rCenter, double fRadiusX, double fRadiusY, double fStart, double fEnd );

        B2DPolygon createPolygonFromUnitEllipseSegment( double fStart, double fEnd );

        /** Predicate whether a given polygon is a rectangle.

            @param rPoly
            Polygon to check

            @return true, if the polygon describes a rectangle
            (polygon is closed, and the points are either cw or ccw
            enumerations of a rectangle's vertices). Note that
            intermediate points and duplicate points are ignored.
         */
        bool isRectangle( const B2DPolygon& rPoly );

        // create 3d polygon from given 2d polygon. The given fZCoordinate is used to expand the
        // third coordinate.
        B3DPolygon createB3DPolygonFromB2DPolygon(const B2DPolygon& rCandidate, double fZCoordinate = 0.0);

        // create 2d PolyPolygon from given 3d PolyPolygon. All coordinates are transformed using the given
        // matrix and the resulting x,y is used to form the new polygon.
        B2DPolygon createB2DPolygonFromB3DPolygon(const B3DPolygon& rCandidate, const B3DHomMatrix& rMat);

        // calculate the smallest distance to given edge and return. The relative position on the edge is returned in Cut.
        double getSmallestDistancePointToEdge(const B2DPoint& rPointA, const B2DPoint& rPointB, const B2DPoint& rTestPoint, double* pCut = 0L);

        // for each contained edge calculate the smallest distance. Return the index to the smallest
        // edge in rEdgeIndex. The relative position on the edge is returned in rCut.
        // If nothing was found (e.g. empty input plygon), DBL_MAX is returned.
        double getSmallestDistancePointToPolygon(const B2DPolygon& rCandidate, const B2DPoint& rTestPoint, sal_uInt32& rEdgeIndex, double& rCut);

        // distort single point. rOriginal describes the original range, where the given points describe the distorted corresponding points.
        B2DPoint distort(const B2DPoint& rCandidate, const B2DRange& rOriginal, const B2DPoint& rTopLeft, const B2DPoint& rTopRight, const B2DPoint& rBottomLeft, const B2DPoint& rBottomRight);

        // distort polygon. rOriginal describes the original range, where the given points describe the distorted corresponding points.
        B2DPolygon distort(const B2DPolygon& rCandidate, const B2DRange& rOriginal, const B2DPoint& rTopLeft, const B2DPoint& rTopRight, const B2DPoint& rBottomLeft, const B2DPoint& rBottomRight);

        // rotate polygon around given point with given angle.
        B2DPolygon rotateAroundPoint(const B2DPolygon& rCandidate, const B2DPoint& rCenter, double fAngle);

        // expand all segments (which are not yet) to curve segments. This is done with setting the control
        // vectors on the 1/3 resp. 2/3 distances on each segment.
        B2DPolygon expandToCurve(const B2DPolygon& rCandidate);

        // expand given segment to curve segment. This is done with setting the control
        // vectors on the 1/3 resp. 2/3 distances. The return value describes if a change took place.
        bool expandToCurveInPoint(B2DPolygon& rCandidate, sal_uInt32 nIndex);

        // set continuity for the whole curve. If not a curve, nothing will change. Non-curve points are not changed, too.
        B2DPolygon setContinuity(const B2DPolygon& rCandidate, B2VectorContinuity eContinuity);

        // set continuity for given index. If not a curve, nothing will change. Non-curve points are not changed, too.
        // The return value describes if a change took place.
        bool setContinuityInPoint(B2DPolygon& rCandidate, sal_uInt32 nIndex, B2VectorContinuity eContinuity);

        // test if polygon contains neutral points. A neutral point is one whos orientation is neutral
        // e.g. positioned on the edge of it's predecessor and successor
        bool hasNeutralPoints(const B2DPolygon& rCandidate);

        // remove neutral points. A neutral point is one whos orientation is neutral
        // e.g. positioned on the edge of it's predecessor and successor
        B2DPolygon removeNeutralPoints(const B2DPolygon& rCandidate);

        // tests if polygon is convex
        bool isConvex(const ::basegfx::B2DPolygon& rCandidate);

        // calculates the orientation at edge nIndex
        B2VectorOrientation getOrientationForIndex(const B2DPolygon& rCandidate, sal_uInt32 nIndex);

        // calculates if given point is on given line, taking care of the numerical epsilon
        bool isPointOnLine(const B2DPoint& rStart, const B2DPoint& rEnd, const B2DPoint& rCandidate, bool bWithPoints = false);

        // test if candidate is inside triangle
        bool isPointInTriangle(const B2DPoint& rA, const B2DPoint& rB, const B2DPoint& rC, const B2DPoint& rCandidate, bool bWithBorder = false);

        // test if candidateA and candidateB are on the same side of the given line
        bool arePointsOnSameSideOfLine(const B2DPoint& rStart, const B2DPoint& rEnd, const B2DPoint& rCandidateA, const B2DPoint& rCandidateB, bool bWithLine = false);

        // add triangles for given rCandidate to rTarget. For each triangle, 3 points will be added to rCandidate.
        // All triangles will go from the start point of rCandidate to two consecutive points, building (rCandidate.count() - 2)
        // triangles.
        void addTriangleFan(const B2DPolygon& rCandidate, B2DPolygon& rTarget);

        bool isPolyPolygonEqualRectangle( const ::basegfx::B2DPolyPolygon& rPolyPoly, const ::basegfx::B2DRange& rRect );

    } // end of namespace tools
} // end of namespace basegfx

#endif /* _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX */
