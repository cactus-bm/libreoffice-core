/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: b2dpolypolygontools.hxx,v $
 * $Revision: 1.19 $
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

#ifndef _BGFX_POLYPOLYGON_B2DPOLYGONTOOLS_HXX
#define _BGFX_POLYPOLYGON_B2DPOLYGONTOOLS_HXX

#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/vector/b2dvector.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b3dpolypolygon.hxx>

#include <vector>

namespace rtl
{
    class OUString;
}

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    // predefinitions
    class B2DPolyPolygon;
    class B2DRange;

    namespace tools
    {
        // B2DPolyPolygon tools

        // Check and evtl. correct orientations of all contained Polygons so that
        // the orientations of contained polygons will variate to express areas and
        // holes
        B2DPolyPolygon correctOrientations(const B2DPolyPolygon& rCandidate);

        // Remove all intersections, the self intersections and the in-between
        // polygon intersections. After this operation there are no more intersections
        // in the given PolyPolygon. Only closed polygons are handled. The non-closed
        // polygons or the ones with less than 3 points are preserved, but not
        // computed.
        // bForceOrientation: If true, the orientations of all contained polygons
        // is changed to ORIENTATION_POSITIVE before computing.
        B2DPolyPolygon removeIntersections(const B2DPolyPolygon& rCandidate);
        B2DPolyPolygon removeAllIntersections(const B2DPolyPolygon& rCandidate);
        B2DPolyPolygon removeNeutralPolygons(const B2DPolyPolygon& rCandidate, bool bUseOr);

        // Subdivide all contained curves. Use distanceBound value if given.
        B2DPolyPolygon adaptiveSubdivideByDistance(const B2DPolyPolygon& rCandidate, double fDistanceBound = 0.0);

        // Subdivide all contained curves. Use distanceBound value if given. Else, a convenient one
        // is created.
        B2DPolyPolygon adaptiveSubdivideByAngle(const B2DPolyPolygon& rCandidate, double fAngleBound = 0.0);

        // Subdivide all contained curves. Use nCount divisions if given. Else, a convenient one
        // is created.
        B2DPolyPolygon adaptiveSubdivideByCount(const B2DPolyPolygon& rCandidate, sal_uInt32 nCount = 0L);

        // isInside test for B2dPoint. On border is not inside as long as not true is given
        // in bWithBorder flag. It is assumed that the orientations of the given polygon are correct.
        bool isInside(const B2DPolyPolygon& rCandidate, const B2DPoint& rPoint, bool bWithBorder = false);

        // get size of PolyPolygon. Control vectors are included in that ranges.
        B2DRange getRange(const B2DPolyPolygon& rCandidate);

        // Apply Line Dashing. This cuts every contained PolyPolygon into line pieces
        // which are inserted as single polygons into the result.
        // If fFullDashDotLen is not given it will be calculated from the given
        // raDashDotArray.
        B2DPolyPolygon applyLineDashing(const B2DPolyPolygon& rCandidate, const ::std::vector<double>& raDashDotArray, double fFullDashDotLen = 0.0);

        // Merge contained Polygons to longer ones if the end point of one Polygon equals
        // the start point of the next one. Only direct successors are checked.
        // This method is mainly for joining line snippets which reach over an original
        // polygon edge after using applyLineDashing. This is necessary if the dashed line
        // parts shall be used for creating thick line geometry to be able to do correct
        // line joints.
        B2DPolyPolygon mergeDashedLines(const B2DPolyPolygon& rCandidate);

        // test if point is inside epsilon-range around the given PolyPolygon. Can be used
        // for HitTesting. The epsilon-range is defined to be the tube around the PolyPolygon
        // with distance fDistance and rounded edges (start and end point).
        bool isInEpsilonRange(const B2DPolyPolygon& rCandidate, const B2DPoint& rTestPosition, double fDistance);

        /** Read poly-polygon from SVG.

            This function imports a poly-polygon from an SVG-D
            attribute. Currently, elliptical arc elements are not yet
            supported (and ignored during parsing).

            @param o_rPolyPoly
            The output poly-polygon

            @param rSvgDAttribute
            A valid SVG-D attribute string

            @return true, if the string was successfully parsed
         */
        bool importFromSvgD( B2DPolyPolygon&        o_rPolyPoly,
                             const ::rtl::OUString& rSvgDAttribute );

        /** Read poly-polygon from SVG.

            This function imports a poly-polygon from an SVG points
            attribute (a plain list of coordinate pairs).

            @param o_rPoly
            The output polygon. Note that svg:points can only define a
            single polygon

            @param rSvgPointsAttribute
            A valid SVG points attribute string

            @return true, if the string was successfully parsed
         */
        bool importFromSvgPoints( B2DPolygon&            o_rPoly,
                                  const ::rtl::OUString& rSvgPointsAttribute );


        // create 3d PolyPolygon from given 2d PolyPolygon. The given fZCoordinate is used to expand the
        // third coordinate.
        B3DPolyPolygon createB3DPolyPolygonFromB2DPolyPolygon(const B2DPolyPolygon& rCandidate, double fZCoordinate = 0.0);

        // create 2d PolyPolygon from given 3d PolyPolygon. All coordinates are transformed using the given
        // matrix and the resulting x,y is used to form the new polygon.
        B2DPolyPolygon createB2DPolyPolygonFromB3DPolyPolygon(const B3DPolyPolygon& rCandidate, const B3DHomMatrix& rMat);

        // for each contained edge in each contained polygon calculate the smallest distance. Return the index to the smallest
        // edge in rEdgeIndex and the index to the polygon in rPolygonIndex. The relative position on the edge is returned in rCut.
        // If nothing was found (e.g. empty input plygon), DBL_MAX is returned.
        double getSmallestDistancePointToPolyPolygon(const B2DPolyPolygon& rCandidate, const B2DPoint& rTestPoint, sal_uInt32& rPolygonIndex, sal_uInt32& rEdgeIndex, double& rCut);

        // distort PolyPolygon. rOriginal describes the original range, where the given points describe the distorted
        // corresponding points.
        B2DPolyPolygon distort(const B2DPolyPolygon& rCandidate, const B2DRange& rOriginal, const B2DPoint& rTopLeft, const B2DPoint& rTopRight, const B2DPoint& rBottomLeft, const B2DPoint& rBottomRight);

        // rotate PolyPolygon around given point with given angle.
        B2DPolyPolygon rotateAroundPoint(const B2DPolyPolygon& rCandidate, const B2DPoint& rCenter, double fAngle);

        // expand all segments (which are not yet) to curve segments. This is done with setting the control
        // vectors on the 1/3 resp. 2/3 distances on each segment.
        B2DPolyPolygon expandToCurve(const B2DPolyPolygon& rCandidate);

        // set continuity for the whole curve. If not a curve, nothing will change. Non-curve points are not changed, too.
        B2DPolyPolygon setContinuity(const B2DPolyPolygon& rCandidate, B2VectorContinuity eContinuity);

        /** Predicate whether a given poly-polygon is a rectangle.

            @param rPoly
            PolyPolygon to check

            @return true, if the poly-polygon describes a rectangle
            (contains exactly one polygon, polygon is closed, and the
            points are either cw or ccw enumerations of a rectangle's
            vertices). Note that intermediate points and duplicate
            points are ignored.
         */
        bool isRectangle( const B2DPolyPolygon& rPoly );

        /** Export poly-polygon to SVG.

            This function exports a poly-polygon into an SVG-D
            statement. Currently, output of relative point sequences
            is not yet supported (might cause slightly larger output)

            @param rPolyPoly
            The poly-polygon to export

            @param bUseRelativeCoordinates
            When true, all coordinate values are exported as relative
            to the current position. This tends to save some space,
            since fewer digits needs to be written.

            @param bDetectQuadraticBeziers
            When true, the export tries to detect cubic bezier
            segments in the input polygon, which can be represented by
            quadratic bezier segments. Note that the generated string
            causes versions prior to OOo2.0 to crash.

            @return the generated SVG-D statement (the XML d attribute
            value alone, without any "<path ...>" or "d="...")
         */
        ::rtl::OUString exportToSvgD( const B2DPolyPolygon& rPolyPoly,
                                      bool                  bUseRelativeCoordinates=true,
                                      bool                  bDetectQuadraticBeziers=true );

        // #i76891# Try to remove existing curve segments if they are simply edges
        B2DPolyPolygon simplifyCurveSegments(const B2DPolyPolygon& rCandidate);

        //////////////////////////////////////////////////////////////////////
        // comparators with tolerance for 2D PolyPolygons
        bool equal(const B2DPolyPolygon& rCandidateA, const B2DPolyPolygon& rCandidateB, const double& rfSmallValue);
        bool equal(const B2DPolyPolygon& rCandidateA, const B2DPolyPolygon& rCandidateB);

    } // end of namespace tools
} // end of namespace basegfx

#endif /* _BGFX_POLYPOLYGON_B2DPOLYGONTOOLS_HXX */
