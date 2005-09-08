/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdoimp.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:33:42 $
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

#ifndef _SVX_SVDOIMP_HXX
#include <svdoimp.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef _SVX_XLNSTIT_HXX
#include <xlnstit.hxx>
#endif

#ifndef _SVX_XLNEDIT_HXX
#include "xlnedit.hxx"
#endif

#ifndef _SVX_XLNWTIT_HXX
#include <xlnwtit.hxx>
#endif

#ifndef _SVX_XLINEIT0_HXX
#include <xlineit0.hxx>
#endif

#ifndef _SVX_XLNSTWIT_HXX
#include <xlnstwit.hxx>
#endif

#ifndef _SVX_XLNEDWIT_HXX
#include <xlnedwit.hxx>
#endif

#ifndef _SVX_XLNSTCIT_HXX
#include <xlnstcit.hxx>
#endif

#ifndef _SVX_XLNEDCIT_HXX
#include <xlnedcit.hxx>
#endif

#ifndef _SVX_XLINJOIT_HXX
#include <xlinjoit.hxx>
#endif

#ifndef _SVX_XLNDSIT_HXX
#include <xlndsit.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif

#ifndef _BGFX_VECTOR_B2DVECTOR_HXX
#include <basegfx/vector/b2dvector.hxx>
#endif

#ifndef _BGFX_RANGE_B2DRANGE_HXX
#include <basegfx/range/b2drange.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

#ifndef _BGFX_POLYPOLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DLINEGEOMETRY_HXX
#include <basegfx/polygon/b2dlinegeometry.hxx>
#endif

static double SMALLEST_DASH_WIDTH(26.95);

///////////////////////////////////////////////////////////////////////////////
// ImpCreateDotDashArray takes a XDash and translates it into an array of doubles
// which describe the lengths of the dashes, dots and empty passages. It returns
// the complete length of a full DashDot sequence and fills the given vetor of
// doubles accordingly (also resizing it).

double ImpCreateDotDashArray(::std::vector< double >& rDotDashArray, const XDash& mrDash, sal_Int32 nLineWidth)
{
    double fFullDashDotLen(0.0);
    const sal_uInt16 nNumDotDashArray = (mrDash.GetDots() + mrDash.GetDashes()) * 2;
    rDotDashArray.resize( nNumDotDashArray, 0.0 );
    sal_uInt16 a;
    sal_uInt16 nIns(0);
    double fDashDotDistance = (double)mrDash.GetDistance();
    double fSingleDashLen = (double)mrDash.GetDashLen();
    double fSingleDotLen = (double)mrDash.GetDotLen();
    double fLineWidth = (double)nLineWidth;

    if(mrDash.GetDashStyle() == XDASH_RECTRELATIVE || mrDash.GetDashStyle() == XDASH_ROUNDRELATIVE)
    {
        if(nLineWidth)
        {
            double fFactor = fLineWidth / 100.0;

            if(mrDash.GetDashes())
            {
                if(mrDash.GetDashLen())
                {
                    // is a dash
                    fSingleDashLen *= fFactor;
                }
                else
                {
                    // is a dot
                    fSingleDashLen = fLineWidth;
                }
            }

            if(mrDash.GetDots())
            {
                if(mrDash.GetDotLen())
                {
                    // is a dash
                    fSingleDotLen *= fFactor;
                }
                else
                {
                    // is a dot
                    fSingleDotLen = fLineWidth;
                }
            }

            if(mrDash.GetDashes() || mrDash.GetDots())
            {
                if(mrDash.GetDistance())
                {
                    fDashDotDistance *= fFactor;
                }
                else
                {
                    fDashDotDistance = fLineWidth;
                }
            }
        }
        else
        {
            if(mrDash.GetDashes())
            {
                if(mrDash.GetDashLen())
                {
                    // is a dash
                    fSingleDashLen = (SMALLEST_DASH_WIDTH * fSingleDashLen) / 100.0;
                }
                else
                {
                    // is a dot
                    fSingleDashLen = SMALLEST_DASH_WIDTH;
                }
            }

            if(mrDash.GetDots())
            {
                if(mrDash.GetDotLen())
                {
                    // is a dash
                    fSingleDotLen = (SMALLEST_DASH_WIDTH * fSingleDotLen) / 100.0;
                }
                else
                {
                    // is a dot
                    fSingleDotLen = SMALLEST_DASH_WIDTH;
                }
            }

            if(mrDash.GetDashes() || mrDash.GetDots())
            {
                if(mrDash.GetDistance())
                {
                    // dash as distance
                    fDashDotDistance = (SMALLEST_DASH_WIDTH * fDashDotDistance) / 100.0;
                }
                else
                {
                    // dot as distance
                    fDashDotDistance = SMALLEST_DASH_WIDTH;
                }
            }
        }
    }
    else
    {
        // smallest dot size compare value
        double fDotCompVal(nLineWidth ? fLineWidth : SMALLEST_DASH_WIDTH);

        // absolute values
        if(mrDash.GetDashes())
        {
            if(mrDash.GetDashLen())
            {
                // is a dash
                if(fSingleDashLen < SMALLEST_DASH_WIDTH)
                {
                    fSingleDashLen = SMALLEST_DASH_WIDTH;
                }
            }
            else
            {
                // is a dot
                if(fSingleDashLen < fDotCompVal)
                {
                    fSingleDashLen = fDotCompVal;
                }
            }
        }

        if(mrDash.GetDots())
        {
            if(mrDash.GetDotLen())
            {
                // is a dash
                if(fSingleDotLen < SMALLEST_DASH_WIDTH)
                {
                    fSingleDotLen = SMALLEST_DASH_WIDTH;
                }
            }
            else
            {
                // is a dot
                if(fSingleDotLen < fDotCompVal)
                {
                    fSingleDotLen = fDotCompVal;
                }
            }
        }

        if(mrDash.GetDashes() || mrDash.GetDots())
        {
            if(mrDash.GetDistance())
            {
                // dash as distance
                if(fDashDotDistance < SMALLEST_DASH_WIDTH)
                {
                    fDashDotDistance = SMALLEST_DASH_WIDTH;
                }
            }
            else
            {
                // dot as distance
                if(fDashDotDistance < fDotCompVal)
                {
                    fDashDotDistance = fDotCompVal;
                }
            }
        }
    }

    for(a=0;a<mrDash.GetDots();a++)
    {
        rDotDashArray[nIns++] = fSingleDotLen;
        fFullDashDotLen += fSingleDotLen;
        rDotDashArray[nIns++] = fDashDotDistance;
        fFullDashDotLen += fDashDotDistance;
    }

    for(a=0;a<mrDash.GetDashes();a++)
    {
        rDotDashArray[nIns++] = fSingleDashLen;
        fFullDashDotLen += fSingleDashLen;
        rDotDashArray[nIns++] = fDashDotDistance;
        fFullDashDotLen += fDashDotDistance;
    }

    return fFullDashDotLen;
}

///////////////////////////////////////////////////////////////////////////////

ImpLineStyleParameterPack::ImpLineStyleParameterPack(
    const SfxItemSet& rSet,
    bool _bForceHair)
:   mbForceNoArrowsLeft(false),
    mbForceNoArrowsRight(false),
    mbForceHair(_bForceHair)
{
    maStartPolygon = (((const XLineStartItem&)(rSet.Get(XATTR_LINESTART))).GetValue()).getB2DPolygon();

    if(maStartPolygon.count())
    {
        if(maStartPolygon.areControlPointsUsed())
        {
            maStartPolygon = ::basegfx::tools::adaptiveSubdivideByAngle(maStartPolygon);
        }

        if(::basegfx::ORIENTATION_NEGATIVE == ::basegfx::tools::getOrientation(maStartPolygon))
        {
            maStartPolygon.flip();
        }
    }

    maEndPolygon = (((const XLineEndItem&)(rSet.Get(XATTR_LINEEND))).GetValue()).getB2DPolygon();

    if(maEndPolygon.count())
    {
        if(maEndPolygon.areControlPointsUsed())
        {
            maEndPolygon = ::basegfx::tools::adaptiveSubdivideByAngle(maEndPolygon);
        }

        if(::basegfx::ORIENTATION_NEGATIVE == ::basegfx::tools::getOrientation(maEndPolygon))
        {
            maEndPolygon.flip();
        }
    }

    mnLineWidth = ((const XLineWidthItem&)(rSet.Get(XATTR_LINEWIDTH))).GetValue();
    mbLineStyleSolid = (XLINE_SOLID == (XLineStyle)((const XLineStyleItem&)rSet.Get(XATTR_LINESTYLE)).GetValue());

    mnStartWidth = ((const XLineStartWidthItem&)(rSet.Get(XATTR_LINESTARTWIDTH))).GetValue();
    if(mnStartWidth < 0)
        mnStartWidth = -mnLineWidth * mnStartWidth / 100L;

    mnEndWidth = ((const XLineEndWidthItem&)(rSet.Get(XATTR_LINEENDWIDTH))).GetValue();
    if(mnEndWidth < 0)
        mnEndWidth = -mnLineWidth * mnEndWidth / 100L;

    mbStartCentered = ((const XLineStartCenterItem&)(rSet.Get(XATTR_LINESTARTCENTER))).GetValue();
    mbEndCentered = ((const XLineEndCenterItem&)(rSet.Get(XATTR_LINEENDCENTER))).GetValue();

    mfDegreeStepWidth = 10.0;
    meLineJoint = ((const XLineJointItem&)(rSet.Get(XATTR_LINEJOINT))).GetValue();

    XDash aDash = ((const XLineDashItem&)(rSet.Get(XATTR_LINEDASH))).GetValue();

    // fill local dash info
    mfFullDashDotLen = ImpCreateDotDashArray(maDotDashArray, aDash, GetDisplayLineWidth());
}

ImpLineStyleParameterPack::~ImpLineStyleParameterPack()
{
}

void ImpLineGeometryCreator::ImpCreateLineGeometry(const ::basegfx::B2DPolygon& rSourcePoly)
{
    if(rSourcePoly.count() > 1L)
    {
        ::basegfx::B2DPolygon aSourceLineGeometry(rSourcePoly);

        if(aSourceLineGeometry.areControlPointsUsed())
        {
            aSourceLineGeometry = ::basegfx::tools::adaptiveSubdivideByAngle(aSourceLineGeometry);
        }

        sal_uInt32 nCount(aSourceLineGeometry.count());

        if(!aSourceLineGeometry.isClosed())
        {
            nCount--;
            const double fPolyLength(::basegfx::tools::getLength(aSourceLineGeometry));
            double fStart(0.0);
            double fEnd(0.0);

            if(mrLineAttr.IsStartActive())
            {
                // create line start polygon and move line end
                ::basegfx::B2DPolyPolygon aArrowPolyPolygon;
                aArrowPolyPolygon.append(mrLineAttr.GetStartPolygon());

                ::basegfx::B2DPolyPolygon aArrow = ::basegfx::tools::createAreaGeometryForLineStartEnd(
                    aSourceLineGeometry,
                    aArrowPolyPolygon,
                    true,
                    (double)mrLineAttr.GetStartWidth(),
                    mrLineAttr.IsStartCentered() ? 0.5 : 0.0,
                    &fStart);

                maAreaPolyPolygon.append(aArrow);
                fStart *= 0.8;
            }

            if(mrLineAttr.IsEndActive())
            {
                // create line end polygon and move line end
                ::basegfx::B2DPolyPolygon aArrowPolyPolygon;
                aArrowPolyPolygon.append(mrLineAttr.GetEndPolygon());

                ::basegfx::B2DPolyPolygon aArrow = ::basegfx::tools::createAreaGeometryForLineStartEnd(
                    aSourceLineGeometry,
                    aArrowPolyPolygon,
                    false,
                    (double)mrLineAttr.GetEndWidth(),
                    mrLineAttr.IsEndCentered() ? 0.5 : 0.0,
                    &fEnd);

                maAreaPolyPolygon.append(aArrow);
                fEnd *= 0.8;
            }

            if(fStart != 0.0 || fEnd != 0.0)
            {
                // build new poly, consume something from old poly
                aSourceLineGeometry = ::basegfx::tools::getSnippetAbsolute(
                    aSourceLineGeometry, fStart, fPolyLength - fEnd, fPolyLength);
                nCount = aSourceLineGeometry.count() - 1L;
            }
        }

        if(nCount)
        {
            ::basegfx::B2DPolyPolygon aHairLinePolyPolygon;

            if(mbLineDraft || mrLineAttr.IsLineStyleSolid())
            {
                // no LineStyle
                aHairLinePolyPolygon.append(aSourceLineGeometry);
            }
            else
            {
                // apply LineStyle
                aHairLinePolyPolygon = ::basegfx::tools::applyLineDashing(aSourceLineGeometry, mrLineAttr.GetDotDash(), mrLineAttr.GetFullDashDotLen());

                // merge LineStyle polygons to bigger parts
                aHairLinePolyPolygon = ::basegfx::tools::mergeDashedLines(aHairLinePolyPolygon);
            }

            if(!mrLineAttr.GetDisplayLineWidth())
            {
                // LineWidth zero, add directly to linePoly
                maLinePolyPolygon.append(aHairLinePolyPolygon);
            }
            else
            {
                ::basegfx::tools::B2DLineJoin aB2DLineJoin;

                switch(mrLineAttr.GetLineJoint())
                {
                case XLINEJOINT_NONE    : aB2DLineJoin = ::basegfx::tools::B2DLINEJOIN_NONE;    break;
                case XLINEJOINT_MIDDLE  : aB2DLineJoin = ::basegfx::tools::B2DLINEJOIN_MIDDLE;  break;
                case XLINEJOINT_BEVEL   : aB2DLineJoin = ::basegfx::tools::B2DLINEJOIN_BEVEL;   break;
                case XLINEJOINT_MITER   : aB2DLineJoin = ::basegfx::tools::B2DLINEJOIN_MITER;   break;
                case XLINEJOINT_ROUND   : aB2DLineJoin = ::basegfx::tools::B2DLINEJOIN_ROUND;   break;
                }

                for(sal_uInt32 a(0L); a < aHairLinePolyPolygon.count(); a++)
                {
                    ::basegfx::B2DPolygon aCandidate = aHairLinePolyPolygon.getB2DPolygon(a);
                    ::basegfx::B2DPolyPolygon aAreaPolyPolygon = ::basegfx::tools::createAreaGeometryForPolygon(
                        aCandidate,
                        (double)mrLineAttr.GetDisplayLineWidth() / 2.0,
                        aB2DLineJoin,
                        (double)mrLineAttr.GetDegreeStepWidth() * F_PI180,
                        (double)mrLineAttr.GetLinejointMiterMinimumAngle() * F_PI180);
                    maAreaPolyPolygon.append(aAreaPolyPolygon);
                }
            }
        }
    }
}

// eof
