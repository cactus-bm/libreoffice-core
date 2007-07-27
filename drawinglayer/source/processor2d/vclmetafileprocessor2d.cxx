/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: vclmetafileprocessor2d.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: aw $ $Date: 2007-07-27 09:03:34 $
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

#ifndef INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLMETAFILEPROCESSOR2D_HXX
#include <drawinglayer/processor2d/vclmetafileprocessor2d.hxx>
#endif

#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif

#ifndef _SV_VIRDEV_HXX
#include <vcl/virdev.hxx>
#endif

#ifndef _SV_GDIMTF_HXX
#include <vcl/gdimtf.hxx>
#endif

#ifndef _SV_GRADIENT_HXX
#include <vcl/gradient.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLATTRIBUTE_HXX
#include <drawinglayer/attribute/fillattribute.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_PRIMITIVETYPES2D_HXX
#include <drawinglayer/primitive2d/drawinglayer_primitivetypes2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE_TEXTPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/textprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_POLYPOLYGONPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/polypolygonprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_POLYGONPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/polygonprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_BITMAPPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/bitmapprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_METAFILEPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/metafileprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_MASKPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/maskprimitive2d.hxx>
#endif

#ifndef _BGFX_POLYPOLYGON_B2DPOLYGONCLIPPER_HXX
#include <basegfx/polygon/b2dpolygonclipper.hxx>
#endif

#ifndef _BGFX_POLYPOLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_MODIFIEDCOLORPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_UNIFIEDALPHAPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/unifiedalphaprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_ALPHAPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/alphaprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_FILLGRADIENTPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/fillgradientprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLPIXELPROCESSOR2D_HXX
#include <drawinglayer/processor2d/vclpixelprocessor2d.hxx>
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TRANSFORMPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_MARKERARRAYPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/markerarrayprimitive2d.hxx>
#endif

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_POINTRARRAYPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/pointarrayprimitive2d.hxx>
#endif

#ifndef _VCL_GRAPHICTOOLS_HXX_
#include <vcl/graphictools.hxx>
#endif

#ifndef _SV_METAACT_HXX
#include <vcl/metaact.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace processor2d
    {
        Rectangle VclMetafileProcessor2D::impDumpToMetaFile(const primitive2d::Primitive2DSequence& rContent, GDIMetaFile& o_rContentMetafile)
        {
            // Prepare VDev, MetaFile and connections
            OutputDevice* pLastOutputDevice = mpOutputDevice;
            const basegfx::B2DRange aPrimitiveRange(primitive2d::getB2DRangeFromPrimitive2DSequence(rContent, getViewInformation2D()));
            const Rectangle aPrimitiveRectangle(
                basegfx::fround(aPrimitiveRange.getMinX()), basegfx::fround(aPrimitiveRange.getMinY()),
                basegfx::fround(aPrimitiveRange.getMaxX()), basegfx::fround(aPrimitiveRange.getMaxY()));
            VirtualDevice aContentVDev;
            MapMode aNewMapMode(pLastOutputDevice->GetMapMode());

            mpOutputDevice = &aContentVDev;
            aContentVDev.EnableOutput(false);
            aContentVDev.SetMapMode(pLastOutputDevice->GetMapMode());
            o_rContentMetafile.Record(&aContentVDev);
            aContentVDev.SetLineColor(pLastOutputDevice->GetLineColor());
            aContentVDev.SetFillColor(pLastOutputDevice->GetFillColor());
            aContentVDev.SetFont(pLastOutputDevice->GetFont());
            aContentVDev.SetDrawMode(pLastOutputDevice->GetDrawMode());
            aContentVDev.SetSettings(pLastOutputDevice->GetSettings());
            aContentVDev.SetRefPoint(pLastOutputDevice->GetRefPoint());

            // dump to MetaFile
            process(rContent);

            // cleanups
            o_rContentMetafile.Stop();
            o_rContentMetafile.WindStart();
            aNewMapMode.SetOrigin(aPrimitiveRectangle.TopLeft());
            o_rContentMetafile.SetPrefMapMode(aNewMapMode);
            o_rContentMetafile.SetPrefSize(aPrimitiveRectangle.GetSize());
            mpOutputDevice = pLastOutputDevice;

            return aPrimitiveRectangle;
        }

        void VclMetafileProcessor2D::impConvertFillGradientAttributeToVCLGradient(Gradient& o_rVCLGradient, const attribute::FillGradientAttribute& rFiGrAtt)
        {
            o_rVCLGradient.SetStartColor(Color(maBColorModifierStack.getModifiedColor(rFiGrAtt.getStartColor())));
            o_rVCLGradient.SetEndColor(Color(maBColorModifierStack.getModifiedColor(rFiGrAtt.getEndColor())));
            o_rVCLGradient.SetAngle(static_cast< sal_uInt16 >(rFiGrAtt.getAngle() * (1.0 / F_PI1800)));
            o_rVCLGradient.SetBorder(static_cast< sal_uInt16 >(rFiGrAtt.getBorder() * 100.0));
            o_rVCLGradient.SetOfsX(static_cast< sal_uInt16 >(rFiGrAtt.getOffsetX() * 100.0));
            o_rVCLGradient.SetOfsY(static_cast< sal_uInt16 >(rFiGrAtt.getOffsetY() * 100.0));
            o_rVCLGradient.SetSteps(rFiGrAtt.getSteps());

            // defaults for intensity; those were computed into the start/end colors already
            o_rVCLGradient.SetStartIntensity(100);
            o_rVCLGradient.SetEndIntensity(100);

            switch(rFiGrAtt.getStyle())
            {
                default : // attribute::GRADIENTSTYLE_LINEAR :
                {
                    o_rVCLGradient.SetStyle(GRADIENT_LINEAR);
                    break;
                }
                case attribute::GRADIENTSTYLE_AXIAL :
                {
                    o_rVCLGradient.SetStyle(GRADIENT_AXIAL);
                    break;
                }
                case attribute::GRADIENTSTYLE_RADIAL :
                {
                    o_rVCLGradient.SetStyle(GRADIENT_RADIAL);
                    break;
                }
                case attribute::GRADIENTSTYLE_ELLIPTICAL :
                {
                    o_rVCLGradient.SetStyle(GRADIENT_ELLIPTICAL);
                    break;
                }
                case attribute::GRADIENTSTYLE_SQUARE :
                {
                    o_rVCLGradient.SetStyle(GRADIENT_SQUARE);
                    break;
                }
                case attribute::GRADIENTSTYLE_RECT :
                {
                    o_rVCLGradient.SetStyle(GRADIENT_RECT);
                    break;
                }
            }
        }

        void VclMetafileProcessor2D::impStartSvtGraphicFill(SvtGraphicFill* pSvtGraphicFill)
        {
            if(pSvtGraphicFill && !mnSvtGraphicFillCount)
            {
                SvMemoryStream aMemStm;

                aMemStm << *pSvtGraphicFill;
                mrMetaFile.AddAction(new MetaCommentAction("XPATHFILL_SEQ_BEGIN", 0, static_cast< const BYTE* >(aMemStm.GetData()), aMemStm.Seek(STREAM_SEEK_TO_END)));
                mnSvtGraphicFillCount++;
            }
        }

        void VclMetafileProcessor2D::impEndSvtGraphicFill(SvtGraphicFill* pSvtGraphicFill)
        {
            if(pSvtGraphicFill && mnSvtGraphicFillCount)
            {
                mnSvtGraphicFillCount--;
                mrMetaFile.AddAction(new MetaCommentAction("XPATHFILL_SEQ_END"));
                delete pSvtGraphicFill;
            }
        }

        SvtGraphicStroke* VclMetafileProcessor2D::impTryToCreateSvtGraphicStroke(
            const basegfx::B2DPolygon& rB2DPolygon,
            const basegfx::BColor* pColor,
            const attribute::StrokeAttribute* pStrokeAttribute,
            const attribute::StrokeArrowAttribute* pStart,
            const attribute::StrokeArrowAttribute* pEnd)
        {
            SvtGraphicStroke* pRetval = 0;

            if(rB2DPolygon.count() && !mnSvtGraphicStrokeCount)
            {
                basegfx::BColor aStrokeColor;
                PolyPolygon aStartPolyPolygon;
                PolyPolygon aEndPolyPolygon;

                if(pColor)
                {
                    aStrokeColor = *pColor;
                }
                else if(pStrokeAttribute)
                {
                    aStrokeColor = maBColorModifierStack.getModifiedColor(pStrokeAttribute->getColor());
                }

                // copied from ImpDrawLineGeometry. Ckecked.
                // It IS needed to record the stroke color at all in the metafile,
                // SvtGraphicStroke has NO entry for stroke color(!)
                mpOutputDevice->SetLineColor(Color(aStrokeColor));

                if(!rB2DPolygon.isClosed())
                {
                    if(pStart && pStart->isActive())
                    {
                        const basegfx::B2DPolyPolygon aStartArrow(basegfx::tools::createAreaGeometryForLineStartEnd(
                            rB2DPolygon, pStart->getB2DPolyPolygon(), true, pStart->getWidth(), pStart->isCentered() ? 0.5 : 0.0, 0));
                        aStartPolyPolygon = PolyPolygon(aStartArrow);
                    }

                    if(pEnd && pEnd->isActive())
                    {
                        const basegfx::B2DPolyPolygon aEndArrow(basegfx::tools::createAreaGeometryForLineStartEnd(
                            rB2DPolygon, pEnd->getB2DPolyPolygon(), false, pEnd->getWidth(), pEnd->isCentered() ? 0.5 : 0.0, 0));
                        aEndPolyPolygon = PolyPolygon(aEndArrow);
                    }
                }

                SvtGraphicStroke::JoinType eJoin(SvtGraphicStroke::joinNone);
                double fLineWidth(0.0);
                double fMiterLength(0.0);
                SvtGraphicStroke::DashArray aDashArray;

                if(pStrokeAttribute)
                {
                    // pre-fill fLineWidth
                    fLineWidth = pStrokeAttribute->getWidth();

                    // pre-fill fMiterLength
                    fMiterLength = fLineWidth;

                    // get Join
                    switch(pStrokeAttribute->getLineJoin())
                    {
                        default : // basegfx::tools::B2DLINEJOIN_NONE :
                        {
                            eJoin = SvtGraphicStroke::joinNone;
                            break;
                        }
                        case basegfx::tools::B2DLINEJOIN_BEVEL :
                        {
                            eJoin = SvtGraphicStroke::joinBevel;
                            break;
                        }
                        case basegfx::tools::B2DLINEJOIN_MIDDLE :
                        case basegfx::tools::B2DLINEJOIN_MITER :
                        {
                            eJoin = SvtGraphicStroke::joinMiter;
                            // ATM 15 degrees is assumed
                            fMiterLength /= rtl::math::sin(M_PI * (15.0 / 360.0));
                            break;
                        }
                        case basegfx::tools::B2DLINEJOIN_ROUND :
                        {
                            eJoin = SvtGraphicStroke::joinRound;
                            break;
                        }
                    }

                    // copy dash array
                    aDashArray = pStrokeAttribute->getDotDashArray();
                }

                pRetval = new SvtGraphicStroke(
                    Polygon(rB2DPolygon),
                    aStartPolyPolygon,
                    aEndPolyPolygon,
                    mfCurrentUnifiedTransparence,
                    fLineWidth,
                    SvtGraphicStroke::capButt,
                    eJoin,
                    fMiterLength,
                    aDashArray);
            }

            return pRetval;
        }

        void VclMetafileProcessor2D::impStartSvtGraphicStroke(SvtGraphicStroke* pSvtGraphicStroke)
        {
            if(pSvtGraphicStroke && !mnSvtGraphicStrokeCount)
            {
                SvMemoryStream aMemStm;

                aMemStm << *pSvtGraphicStroke;
                mrMetaFile.AddAction(new MetaCommentAction("XPATHSTROKE_SEQ_BEGIN", 0, static_cast< const BYTE* >(aMemStm.GetData()), aMemStm.Seek(STREAM_SEEK_TO_END)));
                mnSvtGraphicStrokeCount++;
            }
        }

        void VclMetafileProcessor2D::impEndSvtGraphicStroke(SvtGraphicStroke* pSvtGraphicStroke)
        {
            if(pSvtGraphicStroke && mnSvtGraphicStrokeCount)
            {
                mnSvtGraphicStrokeCount--;
                mrMetaFile.AddAction(new MetaCommentAction("XPATHSTROKE_SEQ_END"));
                delete pSvtGraphicStroke;
            }
        }

        VclMetafileProcessor2D::VclMetafileProcessor2D(const geometry::ViewInformation2D& rViewInformation, OutputDevice& rOutDev)
        :   VclProcessor2D(rViewInformation, rOutDev),
            mrMetaFile(*rOutDev.GetConnectMetaFile()),
            mnSvtGraphicFillCount(0),
            mnSvtGraphicStrokeCount(0),
            mfCurrentUnifiedTransparence(0.0)
        {
            OSL_ENSURE(rOutDev.GetConnectMetaFile(), "VclMetafileProcessor2D: Used on OutDev which has no MetaFile Target (!)");
            // draw to logic coordinates, do not initialize maCurrentTransformation to viewTransformation,
            // do not change MapMode of destination
        }

        VclMetafileProcessor2D::~VclMetafileProcessor2D()
        {
            // MapMode was not changed, no restore necessary
        }

        /*
            Support of MetaCommentActions in the VclMetafileProcessor2D
            Found MetaCommentActions and how they are supported:

            XGRAD_SEQ_BEGIN, XGRAD_SEQ_END:

            Used inside OutputDevice::DrawGradient to mark the start and end of a MetaGradientEx action.
            It is used in various exporters/importers to have direct access to the gradient before it
            is rendered by VCL (and thus fragmented to polygon color actions and others). On that base, e.g.
            the Metafile to SdrObject import creates it's gradient objects.
            Best (and safest) way to support it here is to use PRIMITIVE2D_ID_POLYPOLYGONGRADIENTPRIMITIVE2D,
            map it back to the corresponding tools PolyPolygon and the Gradient and just call
            OutputDevice::DrawGradient which creates the necessary compatible actions.

            XPATHFILL_SEQ_BEGIN, XPATHFILL_SEQ_END:

            Two producers, one is vcl/source/gdi/gdimtf.cxx, line 1273. There, it is transformed
            inside GDIMetaFile::Rotate, nothing to take care of here.
            The second producer is in graphics/svx/source/svdraw/impgrfll.cxx, line 374. This is used
            with each incarnation of ImpGraphicFill when a metafile is recorded, fillstyle is not
            XFILL_NONE and not completely transparent. It creates a SvtGraphicFill and streams it
            to the comment action. A closing end token is created in the destructor.
            Usages of ImpGraphicFill are in DoPaintObject-methods of SdrCircObj, SdrPathObj and
            SdrRectObj.
            The token users pick various actions from SvtGraphicFill, so it may need to be added for all kind
            of filled objects, even simple colored polygons. It is added as extra information; the
            Metafile actions between the two tokens are interpreted as output generated from those
            fills. Thus, users have the choice to use the SvtGraphicFill info or the created output
            actions.
            Even for XFillTransparenceItem it is used, thus it may need to be supported in
            UnifiedAlphaPrimitive2D, too, when interpreted as normally filled PolyPolygon.
            Implemented for:
                PRIMITIVE2D_ID_POLYPOLYGONBITMAPPRIMITIVE2D,
                PRIMITIVE2D_ID_POLYPOLYGONHATCHPRIMITIVE2D,
                PRIMITIVE2D_ID_POLYPOLYGONGRADIENTPRIMITIVE2D,
                PRIMITIVE2D_ID_POLYPOLYGONCOLORPRIMITIVE2D,
                and for PRIMITIVE2D_ID_UNIFIEDALPHAPRIMITIVE2D when detected unified alpha




            To be done:


            XPATHSTROKE_SEQ_BEGIN, XPATHSTROKE_SEQ_END:
            Similar to pathfill, but using SvtGraphicStroke instead. It also has two producers where one
            is also the GDIMetaFile::Rotate. Another user is MetaCommentAction::Move which modifies the
            contained path accordingly.
            The other one is SdrObject::ImpDrawLineGeometry. It's done when MetaFile is set at OutDev and
            only when geometry is a single polygon (!). I see no reason for that; in the PS exporter this
            would hinder to make use of PolyPolygon strokes. I will need to add support at:
                PRIMITIVE2D_ID_POLYGONHAIRLINEPRIMITIVE2D
                PRIMITIVE2D_ID_POLYGONSTROKEPRIMITIVE2D
                PRIMITIVE2D_ID_POLYGONSTROKEARROWPRIMITIVE2D
            This can be done hierarchical, too.
            Okay, base implementation done based on those three primitives.










            FIELD_SEQ_BEGIN
            FIELD_SEQ_BEGIN;PageField
            FIELD_SEQ_END

            EPSReplacementGraphic

            XTEXT
            XTEXT_EOC
            XTEXT_EOS
            XTEXT_EOL
            XTEXT_EOP

            XTEXT_PAINTSHAPE_BEGIN
            XTEXT_PAINTSHAPE_END

            XTEXT_SCROLLRECT
            XTEXT_PAINTRECT

            PRNSPOOL_TRANSPARENTBITMAP_BEGIN
            PRNSPOOL_TRANSPARENTBITMAP_END

        */

        void VclMetafileProcessor2D::processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate)
        {
            switch(rCandidate.getPrimitiveID())
            {
                case PRIMITIVE2D_ID_TEXTSIMPLEPORTIONPRIMITIVE2D :
                case PRIMITIVE2D_ID_TEXTDECORATEDPORTIONPRIMITIVE2D :
                {
                    // directdraw of text simple portion; use default processing
                    RenderTextSimpleOrDecoratedPortionPrimitive2D(static_cast< const primitive2d::TextSimplePortionPrimitive2D& >(rCandidate));
                    break;
                }
                case PRIMITIVE2D_ID_POLYGONHAIRLINEPRIMITIVE2D :
                {
                    // direct draw of hairline; use default processing
                    // also support SvtGraphicStroke MetaCommentAction
                    const primitive2d::PolygonHairlinePrimitive2D& rHairlinePrimitive = static_cast< const primitive2d::PolygonHairlinePrimitive2D& >(rCandidate);
                    const basegfx::BColor aLineColor(maBColorModifierStack.getModifiedColor(rHairlinePrimitive.getBColor()));
                    SvtGraphicStroke* pSvtGraphicStroke = impTryToCreateSvtGraphicStroke(rHairlinePrimitive.getB2DPolygon(), &aLineColor, 0, 0, 0);

                    impStartSvtGraphicStroke(pSvtGraphicStroke);
                    RenderPolygonHairlinePrimitive2D(static_cast< const primitive2d::PolygonHairlinePrimitive2D& >(rCandidate));
                    impEndSvtGraphicStroke(pSvtGraphicStroke);
                    break;
                }
                case PRIMITIVE2D_ID_POLYGONSTROKEPRIMITIVE2D :
                {
                    // support SvtGraphicStroke MetaCommentAction
                    const primitive2d::PolygonStrokePrimitive2D& rStrokePrimitive = static_cast< const primitive2d::PolygonStrokePrimitive2D& >(rCandidate);
                    SvtGraphicStroke* pSvtGraphicStroke = impTryToCreateSvtGraphicStroke(rStrokePrimitive.getB2DPolygon(), 0, &rStrokePrimitive.getStrokeAttribute(), 0, 0);

                    impStartSvtGraphicStroke(pSvtGraphicStroke);
                    process(rCandidate.get2DDecomposition(getViewInformation2D()));
                    impEndSvtGraphicStroke(pSvtGraphicStroke);
                    break;
                }
                case PRIMITIVE2D_ID_POLYGONSTROKEARROWPRIMITIVE2D :
                {
                    // support SvtGraphicStroke MetaCommentAction
                    const primitive2d::PolygonStrokeArrowPrimitive2D& rStrokeArrowPrimitive = static_cast< const primitive2d::PolygonStrokeArrowPrimitive2D& >(rCandidate);
                    SvtGraphicStroke* pSvtGraphicStroke = impTryToCreateSvtGraphicStroke(rStrokeArrowPrimitive.getB2DPolygon(), 0, &rStrokeArrowPrimitive.getStrokeAttribute(),
                        &rStrokeArrowPrimitive.getStart(), &rStrokeArrowPrimitive.getEnd());

                    impStartSvtGraphicStroke(pSvtGraphicStroke);
                    process(rCandidate.get2DDecomposition(getViewInformation2D()));
                    impEndSvtGraphicStroke(pSvtGraphicStroke);
                    break;
                }
                case PRIMITIVE2D_ID_BITMAPPRIMITIVE2D :
                {
                    // direct draw of transformed BitmapEx primitive; use default processing
                    RenderBitmapPrimitive2D(static_cast< const primitive2d::BitmapPrimitive2D& >(rCandidate));
                    break;
                }
                case PRIMITIVE2D_ID_POLYPOLYGONBITMAPPRIMITIVE2D :
                {
                    // need to handle PolyPolygonBitmapPrimitive2D here to support XPATHFILL_SEQ_BEGIN/XPATHFILL_SEQ_END
                    SvtGraphicFill* pSvtGraphicFill = 0;

                    if(!mnSvtGraphicFillCount)
                    {
                        const primitive2d::PolyPolygonBitmapPrimitive2D& rBitmapCandidate = static_cast< const primitive2d::PolyPolygonBitmapPrimitive2D& >(rCandidate);
                        basegfx::B2DPolyPolygon aLocalPolyPolygon(rBitmapCandidate.getB2DPolyPolygon());
                        aLocalPolyPolygon.transform(maCurrentTransformation);

                        if(aLocalPolyPolygon.count())
                        {
                            // calculate transformation. To get the needed start offset, get the range from the
                            // outline and compare top left with top left of FillBitmapAttribute
                            const attribute::FillBitmapAttribute& rFillBitmapAttribute = rBitmapCandidate .getFillBitmap();
                            const basegfx::B2DRange aOutlineRange(basegfx::tools::getRange(basegfx::tools::adaptiveSubdivideByAngle(aLocalPolyPolygon)));
                            const basegfx::B2DVector aStartOffset(rFillBitmapAttribute.getTopLeft() - aOutlineRange.getMinimum());

                            // the scaling needs scale from pixel to logic coordinate system
                            const Bitmap& rBitmap = rFillBitmapAttribute.getBitmap();
                            Size aBmpSizePixel(rBitmap.GetSizePixel());

                            if(!aBmpSizePixel.Width())
                            {
                                aBmpSizePixel.Width() = 1;
                            }

                            if(!aBmpSizePixel.Height())
                            {
                                aBmpSizePixel.Height() = 1;
                            }

                            const double fScaleX(rFillBitmapAttribute.getSize().getX() / aBmpSizePixel.Width());
                            const double fScaleY(rFillBitmapAttribute.getSize().getY() / aBmpSizePixel.Height());

                            // setup transformation like in impgrfll
                            SvtGraphicFill::Transform aTransform;

                            aTransform.matrix[0] *= fScaleX;
                            aTransform.matrix[4] *= fScaleY;
                            aTransform.matrix[2] += aStartOffset.getX();
                            aTransform.matrix[5] += aStartOffset.getY();

                            // setup fill graphic like in impgrfll
                            Graphic aFillGraphic = Graphic(rBitmap);
                            aFillGraphic.SetPrefMapMode(MapMode(MAP_PIXEL));
                            aFillGraphic.SetPrefSize(aBmpSizePixel);

                            pSvtGraphicFill = new SvtGraphicFill(
                                PolyPolygon(aLocalPolyPolygon),
                                Color(),
                                0.0,
                                SvtGraphicFill::fillEvenOdd,
                                SvtGraphicFill::fillTexture,
                                aTransform,
                                rFillBitmapAttribute.getTiling(),
                                SvtGraphicFill::hatchSingle,
                                Color(),
                                SvtGraphicFill::gradientLinear,
                                Color(),
                                Color(),
                                0,
                                aFillGraphic);
                        }
                    }

                    // Do use decomposition; encapsulate with SvtGraphicFill
                    impStartSvtGraphicFill(pSvtGraphicFill);
                    process(rCandidate.get2DDecomposition(getViewInformation2D()));
                    impEndSvtGraphicFill(pSvtGraphicFill);

                    break;
                }
                case PRIMITIVE2D_ID_POLYPOLYGONHATCHPRIMITIVE2D :
                {
                    // need to handle PolyPolygonHatchPrimitive2D here to support XPATHFILL_SEQ_BEGIN/XPATHFILL_SEQ_END
                    SvtGraphicFill* pSvtGraphicFill = 0;

                    if(!mnSvtGraphicFillCount)
                    {
                        const primitive2d::PolyPolygonHatchPrimitive2D& rHatchCandidate = static_cast< const primitive2d::PolyPolygonHatchPrimitive2D& >(rCandidate);
                        basegfx::B2DPolyPolygon aLocalPolyPolygon(rHatchCandidate.getB2DPolyPolygon());
                        aLocalPolyPolygon.transform(maCurrentTransformation);

                        if(aLocalPolyPolygon.count())
                        {
                            // re-create a VCL hatch as base data
                            const attribute::FillHatchAttribute& rFillHatchAttribute = rHatchCandidate.getFillHatch();
                            SvtGraphicFill::HatchType eHatch(SvtGraphicFill::hatchSingle);

                            switch(rFillHatchAttribute.getStyle())
                            {
                                default: // attribute::HATCHSTYLE_SINGLE :
                                {
                                    eHatch = SvtGraphicFill::hatchSingle;
                                    break;
                                }
                                case attribute::HATCHSTYLE_DOUBLE :
                                {
                                    eHatch = SvtGraphicFill::hatchDouble;
                                    break;
                                }
                                case attribute::HATCHSTYLE_TRIPLE :
                                {
                                    eHatch = SvtGraphicFill::hatchTriple;
                                    break;
                                }
                            }

                            SvtGraphicFill::Transform aTransform;

                            // scale
                            aTransform.matrix[0] *= rFillHatchAttribute.getDistance();
                            aTransform.matrix[4] *= rFillHatchAttribute.getDistance();

                            // rotate (was never correct in impgrfll anyways, use correct angle now)
                            aTransform.matrix[0] *= cos(rFillHatchAttribute.getAngle());
                            aTransform.matrix[1] *= -sin(rFillHatchAttribute.getAngle());
                            aTransform.matrix[3] *= sin(rFillHatchAttribute.getAngle());
                            aTransform.matrix[4] *= cos(rFillHatchAttribute.getAngle());

                            pSvtGraphicFill = new SvtGraphicFill(
                                PolyPolygon(aLocalPolyPolygon),
                                Color(),
                                0.0,
                                SvtGraphicFill::fillEvenOdd,
                                SvtGraphicFill::fillHatch,
                                aTransform,
                                false,
                                eHatch,
                                Color(rFillHatchAttribute.getColor()),
                                SvtGraphicFill::gradientLinear,
                                Color(),
                                Color(),
                                0,
                                Graphic());
                        }
                    }

                    // Do use decomposition; encapsulate with SvtGraphicFill
                    impStartSvtGraphicFill(pSvtGraphicFill);
                    process(rCandidate.get2DDecomposition(getViewInformation2D()));
                    impEndSvtGraphicFill(pSvtGraphicFill);

                    break;
                }
                case PRIMITIVE2D_ID_POLYPOLYGONGRADIENTPRIMITIVE2D :
                {
                    const primitive2d::PolyPolygonGradientPrimitive2D& rGradientCandidate = static_cast< const primitive2d::PolyPolygonGradientPrimitive2D& >(rCandidate);

                    // for support of MetaCommentActions of the form XGRAD_SEQ_BEGIN, XGRAD_SEQ_END
                    // it is safest to use the VCL OutputDevice::DrawGradient method which creates those.
                    // re-create a VCL-gradient from FillGradientPrimitive2D and the needed tools PolyPolygon
                    Gradient aVCLGradient;
                    impConvertFillGradientAttributeToVCLGradient(aVCLGradient, rGradientCandidate.getFillGradient());
                    basegfx::B2DPolyPolygon aLocalPolyPolygon(rGradientCandidate.getB2DPolyPolygon());
                    aLocalPolyPolygon.transform(maCurrentTransformation);
                    const PolyPolygon aToolsPolyPolygon(aLocalPolyPolygon);

                    // XPATHFILL_SEQ_BEGIN/XPATHFILL_SEQ_END support
                    SvtGraphicFill* pSvtGraphicFill = 0;

                    if(!mnSvtGraphicFillCount && aLocalPolyPolygon.count())
                    {
                        // setup gradient stuff like in like in impgrfll
                        SvtGraphicFill::GradientType eGrad(SvtGraphicFill::gradientLinear);

                        switch(aVCLGradient.GetStyle())
                        {
                            default : // GRADIENT_LINEAR:
                            case GRADIENT_AXIAL:
                                eGrad = SvtGraphicFill::gradientLinear;
                                break;
                            case GRADIENT_RADIAL:
                            case GRADIENT_ELLIPTICAL:
                                eGrad = SvtGraphicFill::gradientRadial;
                                break;
                            case GRADIENT_SQUARE:
                            case GRADIENT_RECT:
                                eGrad = SvtGraphicFill::gradientRectangular;
                                break;
                        }

                        pSvtGraphicFill = new SvtGraphicFill(
                            aToolsPolyPolygon,
                            Color(),
                            0.0,
                            SvtGraphicFill::fillEvenOdd,
                            SvtGraphicFill::fillGradient,
                            SvtGraphicFill::Transform(),
                            false,
                            SvtGraphicFill::hatchSingle,
                            Color(),
                            eGrad,
                            aVCLGradient.GetStartColor(),
                            aVCLGradient.GetEndColor(),
                            aVCLGradient.GetSteps(),
                            Graphic());
                    }

                    // call VCL directly; encapsulate with SvtGraphicFill
                    impStartSvtGraphicFill(pSvtGraphicFill);
                    mpOutputDevice->DrawGradient(aToolsPolyPolygon, aVCLGradient);
                    impEndSvtGraphicFill(pSvtGraphicFill);

                    // NO usage of common own gradient randerer, not used ATM for VCL MetaFile, see text above
                    // RenderPolyPolygonGradientPrimitive2D(static_cast< const primitive2d::PolyPolygonGradientPrimitive2D& >(rCandidate));
                    break;
                }
                case PRIMITIVE2D_ID_POLYPOLYGONCOLORPRIMITIVE2D :
                {
                    const primitive2d::PolyPolygonColorPrimitive2D& rPolygonCandidate(static_cast< const primitive2d::PolyPolygonColorPrimitive2D& >(rCandidate));
                    const basegfx::BColor aPolygonColor(maBColorModifierStack.getModifiedColor(rPolygonCandidate.getBColor()));
                    basegfx::B2DPolyPolygon aLocalPolyPolygon(rPolygonCandidate.getB2DPolyPolygon());
                    aLocalPolyPolygon.transform(maCurrentTransformation);

                    // XPATHFILL_SEQ_BEGIN/XPATHFILL_SEQ_END support
                    SvtGraphicFill* pSvtGraphicFill = 0;

                    if(!mnSvtGraphicFillCount && aLocalPolyPolygon.count())
                    {
                        // setup simple color fill stuff like in impgrfll
                        pSvtGraphicFill = new SvtGraphicFill(
                            PolyPolygon(aLocalPolyPolygon),
                            Color(aPolygonColor),
                            0.0,
                            SvtGraphicFill::fillEvenOdd,
                            SvtGraphicFill::fillSolid,
                            SvtGraphicFill::Transform(),
                            false,
                            SvtGraphicFill::hatchSingle,
                            Color(),
                            SvtGraphicFill::gradientLinear,
                            Color(),
                            Color(),
                            0,
                            Graphic());
                    }

                    // call VCL directly; encapsulate with SvtGraphicFill
                    impStartSvtGraphicFill(pSvtGraphicFill);
                    mpOutputDevice->SetFillColor(Color(aPolygonColor));
                    mpOutputDevice->SetLineColor();
                    mpOutputDevice->DrawPolyPolygon(aLocalPolyPolygon);
                    impEndSvtGraphicFill(pSvtGraphicFill);

                    break;
                }
                case PRIMITIVE2D_ID_METAFILEPRIMITIVE2D :
                {
                    // direct draw of MetaFile, use default pocessing
                    RenderMetafilePrimitive2D(static_cast< const primitive2d::MetafilePrimitive2D& >(rCandidate));
                    break;
                }
                case PRIMITIVE2D_ID_MASKPRIMITIVE2D :
                {
                    // mask group. Special handling for MetaFiles.
                    const primitive2d::MaskPrimitive2D& rMaskCandidate = static_cast< const primitive2d::MaskPrimitive2D& >(rCandidate);

                    if(rMaskCandidate.getChildren().hasElements())
                    {
                        basegfx::B2DPolyPolygon aMask(rMaskCandidate.getMask());

                        if(aMask.count())
                        {
                            // prepare new mask polygon and rescue current one
                            aMask.transform(maCurrentTransformation);
                            const basegfx::B2DPolyPolygon aLastClipPolyPolygon(maClipPolyPolygon);

                            if(maClipPolyPolygon.count())
                            {
                                // there is already a clip polygon set; build clipped union of
                                // current mask polygon and new one
                                maClipPolyPolygon = basegfx::tools::clipPolyPolygonOnPolyPolygon(aMask, maClipPolyPolygon, false, false);
                            }
                            else
                            {
                                // use mask directly
                                maClipPolyPolygon = aMask;
                            }

                            if(maClipPolyPolygon.count())
                            {
                                // set VCL clip region; subdivide before conversion to tools polygon
                                mpOutputDevice->Push(PUSH_CLIPREGION);
                                mpOutputDevice->SetClipRegion(Region(PolyPolygon(basegfx::tools::adaptiveSubdivideByAngle(maClipPolyPolygon))));
                            }

                            // recursively paint content
                            process(rMaskCandidate.getChildren());

                            if(maClipPolyPolygon.count())
                            {
                                // restore VCL clip region
                                mpOutputDevice->Pop();
                            }

                            // restore to rescued clip polygon
                            maClipPolyPolygon = aLastClipPolyPolygon;
                        }
                    }

                    break;
                }
                case PRIMITIVE2D_ID_MODIFIEDCOLORPRIMITIVE2D :
                {
                    // modified color group. Force output to unified color. Use default pocessing.
                    RenderModifiedColorPrimitive2D(static_cast< const primitive2d::ModifiedColorPrimitive2D& >(rCandidate));
                    break;
                }
                case PRIMITIVE2D_ID_UNIFIEDALPHAPRIMITIVE2D :
                {
                    // for metafile: Need to examine what the pure vcl version is doing here actually
                    // - uses DrawTransparent with metafile for content and a gradient
                    // - uses DrawTransparent for single PolyPoylgons directly. Can be detected by
                    //   checking the content for single PolyPolygonColorPrimitive2D
                    const primitive2d::UnifiedAlphaPrimitive2D& rUniAlphaCandidate = static_cast< const primitive2d::UnifiedAlphaPrimitive2D& >(rCandidate);
                    const primitive2d::Primitive2DSequence rContent = rUniAlphaCandidate.getChildren();

                    if(rContent.hasElements())
                    {
                        // try to identify a single PolyPolygonColorPrimitive2D in the
                        // content part of the alpha primitive
                        const primitive2d::PolyPolygonColorPrimitive2D* pPoPoColor = 0;
                        static bool bForceToMetafile(false);

                        if(!bForceToMetafile && 1 == rContent.getLength())
                        {
                            const primitive2d::Primitive2DReference xReference(rContent[0]);
                            pPoPoColor = dynamic_cast< const primitive2d::PolyPolygonColorPrimitive2D* >(xReference.get());
                        }

                        if(pPoPoColor)
                        {
                            // single transparent PolyPolygon identified, use directly
                            const basegfx::BColor aPolygonColor(maBColorModifierStack.getModifiedColor(pPoPoColor->getBColor()));
                            basegfx::B2DPolyPolygon aLocalPolyPolygon(pPoPoColor->getB2DPolyPolygon());
                            aLocalPolyPolygon.transform(maCurrentTransformation);

                            // XPATHFILL_SEQ_BEGIN/XPATHFILL_SEQ_END support
                            SvtGraphicFill* pSvtGraphicFill = 0;

                            if(!mnSvtGraphicFillCount && aLocalPolyPolygon.count())
                            {
                                // setup simple color with transparence fill stuff like in impgrfll
                                pSvtGraphicFill = new SvtGraphicFill(
                                    PolyPolygon(aLocalPolyPolygon),
                                    Color(aPolygonColor),
                                    rUniAlphaCandidate.getAlpha(),
                                    SvtGraphicFill::fillEvenOdd,
                                    SvtGraphicFill::fillSolid,
                                    SvtGraphicFill::Transform(),
                                    false,
                                    SvtGraphicFill::hatchSingle,
                                    Color(),
                                    SvtGraphicFill::gradientLinear,
                                    Color(),
                                    Color(),
                                    0,
                                    Graphic());
                            }

                            // call VCL directly; encapsulate with SvtGraphicFill
                            impStartSvtGraphicFill(pSvtGraphicFill);
                            const sal_uInt16 nTransPercentVcl((sal_uInt16)basegfx::fround(rUniAlphaCandidate.getAlpha() * 100.0));
                            mpOutputDevice->SetFillColor(Color(aPolygonColor));
                            mpOutputDevice->SetLineColor();
                            mpOutputDevice->DrawTransparent(
                                PolyPolygon(basegfx::tools::adaptiveSubdivideByAngle(aLocalPolyPolygon)),
                                nTransPercentVcl);
                            impEndSvtGraphicFill(pSvtGraphicFill);
                        }
                        else
                        {
                            // svae old mfCurrentUnifiedTransparence and set new one
                            // so that contained SvtGraphicStroke may use the current one
                            const double fLastCurrentUnifiedTransparence(mfCurrentUnifiedTransparence);
                            mfCurrentUnifiedTransparence = rUniAlphaCandidate.getAlpha();

                            // various content, create content-metafile
                            GDIMetaFile aContentMetafile;
                            const Rectangle aPrimitiveRectangle(impDumpToMetaFile(rContent, aContentMetafile));

                            // restore mfCurrentUnifiedTransparence; it may have been used
                            // while processing the sub-content in impDumpToMetaFile
                            mfCurrentUnifiedTransparence = fLastCurrentUnifiedTransparence;

                            // create uniform VCL gradient for uniform transparency
                            Gradient aVCLGradient;
                            const sal_uInt8 nTransPercentVcl((sal_uInt8)basegfx::fround(rUniAlphaCandidate.getAlpha() * 255.0));
                            const Color aTransColor(nTransPercentVcl, nTransPercentVcl, nTransPercentVcl);

                            aVCLGradient.SetStyle(GRADIENT_LINEAR);
                            aVCLGradient.SetStartColor(aTransColor);
                            aVCLGradient.SetEndColor(aTransColor);
                            aVCLGradient.SetAngle(0);
                            aVCLGradient.SetBorder(0);
                            aVCLGradient.SetOfsX(0);
                            aVCLGradient.SetOfsY(0);
                            aVCLGradient.SetStartIntensity(100);
                            aVCLGradient.SetEndIntensity(100);
                            aVCLGradient.SetSteps(2);

                            // render it to VCL
                            mpOutputDevice->DrawTransparent(
                                aContentMetafile, aPrimitiveRectangle.TopLeft(), aPrimitiveRectangle.GetSize(), aVCLGradient);
                        }
                    }

                    break;
                }
                case PRIMITIVE2D_ID_ALPHAPRIMITIVE2D :
                {
                    // for metafile: Need to examine what the pure vcl version is doing here actually
                    // - uses DrawTransparent with metafile for content and a gradient
                    // i can detect this here with checking the gradient part for a single
                    // FillGradientPrimitive2D and reconstruct the gradient.
                    // If that detection goes wrong, i have to create an alpha-blended bitmap. Eventually
                    // do that in stripes, else RenderAlphaPrimitive2D may just be used
                    const primitive2d::AlphaPrimitive2D& rAlphaCandidate = static_cast< const primitive2d::AlphaPrimitive2D& >(rCandidate);
                    const primitive2d::Primitive2DSequence rContent = rAlphaCandidate.getChildren();
                    const primitive2d::Primitive2DSequence rAlpha = rAlphaCandidate.getAlpha();

                    if(rContent.hasElements() && rAlpha.hasElements())
                    {
                        // try to identify a single FillGradientPrimitive2D in the
                        // alpha part of the primitive
                        const primitive2d::FillGradientPrimitive2D* pFiGradient = 0;
                        static bool bForceToBigTransparentVDev(false);

                        if(!bForceToBigTransparentVDev && 1 == rAlpha.getLength())
                        {
                            const primitive2d::Primitive2DReference xReference(rAlpha[0]);
                            pFiGradient = dynamic_cast< const primitive2d::FillGradientPrimitive2D* >(xReference.get());
                        }

                        if(pFiGradient)
                        {
                            // various content, create content-metafile
                            GDIMetaFile aContentMetafile;
                            const Rectangle aPrimitiveRectangle(impDumpToMetaFile(rContent, aContentMetafile));

                            // re-create a VCL-gradient from FillGradientPrimitive2D
                            Gradient aVCLGradient;
                            impConvertFillGradientAttributeToVCLGradient(aVCLGradient, pFiGradient->getFillGradient());

                            // render it to VCL
                            mpOutputDevice->DrawTransparent(
                                aContentMetafile, aPrimitiveRectangle.TopLeft(), aPrimitiveRectangle.GetSize(), aVCLGradient);
                        }
                        else
                        {
                            // sub-transparence group. Draw to VDev first.
                            // this may get refined to tiling when resolution is too big here

                            // need to avoid switching off MapMode stuff here; maybe need another
                            // tooling class, cannot just do the same as with the pixel renderer.
                            // Need to experiment...

                            // Okay, basic implementation finished and tested. The DPI stuff was hard
                            // and not easy to find out that it's needed.
                            // Since this will not yet happen normally (as long as noone constructs
                            // alpha primitives with non-trivial alpha content) i will for now not
                            // refine to tiling here.

                            basegfx::B2DRange aViewRange(primitive2d::getB2DRangeFromPrimitive2DSequence(rContent, getViewInformation2D()));
                            aViewRange.transform(maCurrentTransformation);
                            const Rectangle aRectLogic(
                                (sal_Int32)floor(aViewRange.getMinX()), (sal_Int32)floor(aViewRange.getMinY()),
                                (sal_Int32)ceil(aViewRange.getMaxX()), (sal_Int32)ceil(aViewRange.getMaxY()));
                            const Rectangle aRectPixel(mpOutputDevice->LogicToPixel(aRectLogic));
                            const Size aSizePixel(aRectPixel.GetSize());
                            const Point aEmptyPoint;
                            VirtualDevice aBufferDevice;

                            if(aBufferDevice.SetOutputSizePixel(aSizePixel))
                            {
                                // create and set MapModes for target devices
                                MapMode aNewMapMode(mpOutputDevice->GetMapMode());
                                aNewMapMode.SetOrigin(Point(-aRectLogic.Left(), -aRectLogic.Top()));
                                aBufferDevice.SetMapMode(aNewMapMode);

                                // prepare view transformation for target renderers
                                // ATTENTION! Need to apply another scaling because of the potential DPI differences
                                // between Printer and VDev (mpOutputDevice and aBufferDevice here).
                                // To get the DPI, LogicToPixel from (1,1) from MAP_INCH needs to be used.
                                basegfx::B2DHomMatrix aViewTransform(aBufferDevice.GetViewTransformation());
                                const Size aDPIOld(mpOutputDevice->LogicToPixel(Size(1, 1), MAP_INCH));
                                const Size aDPINew(aBufferDevice.LogicToPixel(Size(1, 1), MAP_INCH));
                                const double fDPIXChange((double)aDPIOld.getWidth() / (double)aDPINew.getWidth());
                                const double fDPIYChange((double)aDPIOld.getHeight() / (double)aDPINew.getHeight());

                                if(!basegfx::fTools::equal(fDPIXChange, 1.0) || !basegfx::fTools::equal(fDPIYChange, 1.0))
                                {
                                    aViewTransform.scale(fDPIXChange, fDPIYChange);
                                }

                                // create view information and pixel renderer
                                const geometry::ViewInformation2D aViewInfo(aViewTransform, aViewRange, 0.0);
                                VclPixelProcessor2D aBufferProcessor(aViewInfo, aBufferDevice);

                                // draw content using pixel renderer
                                aBufferProcessor.process(rContent);
                                const Bitmap aBmContent(aBufferDevice.GetBitmap(aEmptyPoint, aSizePixel));

                                // draw alpha using pixel renderer
                                aBufferDevice.Erase();
                                aBufferProcessor.process(rAlpha);
                                const AlphaMask aBmAlpha(aBufferDevice.GetBitmap(aEmptyPoint, aSizePixel));

#ifdef DBG_UTIL
                                static bool bDoSaveForVisualControl(false);
                                if(bDoSaveForVisualControl)
                                {
                                    SvFileStream aNew(String(ByteString( "c:\\test.bmp" ), RTL_TEXTENCODING_UTF8), STREAM_WRITE|STREAM_TRUNC);
                                    aNew << aBmContent;
                                }
#endif

                                // paint
                                mpOutputDevice->DrawBitmapEx(
                                    aRectLogic.TopLeft(),
                                    aRectLogic.GetSize(),
                                    BitmapEx(aBmContent, aBmAlpha));
                            }
                        }
                    }

                    break;
                }
                case PRIMITIVE2D_ID_TRANSFORMPRIMITIVE2D :
                {
                    // use default transform group pocessing
                    RenderTransformPrimitive2D(static_cast< const primitive2d::TransformPrimitive2D& >(rCandidate));
                    break;
                }
                case PRIMITIVE2D_ID_MARKERARRAYPRIMITIVE2D :
                {
                    // use default marker array pocessing
                    RenderMarkerArrayPrimitive2D(static_cast< const primitive2d::MarkerArrayPrimitive2D& >(rCandidate));
                    break;
                }
                case PRIMITIVE2D_ID_POINTARRAYPRIMITIVE2D :
                {
                    // use default point array pocessing
                    RenderPointArrayPrimitive2D(static_cast< const primitive2d::PointArrayPrimitive2D& >(rCandidate));
                    break;
                }
                default :
                {
                    // process recursively
                    process(rCandidate.get2DDecomposition(getViewInformation2D()));
                    break;
                }
            }
        }
    } // end of namespace processor2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
