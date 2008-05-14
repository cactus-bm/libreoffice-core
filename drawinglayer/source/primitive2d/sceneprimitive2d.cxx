/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sceneprimitive2d.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: aw $ $Date: 2008-05-14 09:21:53 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_drawinglayer.hxx"

#include <drawinglayer/primitive2d/sceneprimitive2d.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b2dpolygonclipper.hxx>
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <drawinglayer/primitive2d/bitmapprimitive2d.hxx>
#include <drawinglayer/processor3d/zbufferprocessor3d.hxx>
#include <drawinglayer/processor3d/tbufferprocessor3d.hxx>
#include <drawinglayer/processor3d/shadow3dextractor.hxx>
#include <drawinglayer/processor3d/label3dextractor.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>
#include <drawinglayer/primitive2d/drawinglayer_primitivetypes2d.hxx>
#include <svtools/optionsdrawinglayer.hxx>
#include <drawinglayer/processor3d/geometry2dextractor.hxx>
#include <drawinglayer/primitive2d/polygonprimitive2d.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        void ScenePrimitive2D::calculateDsicreteSizes(
            const geometry::ViewInformation2D& rViewInformation,
            basegfx::B2DRange& rDiscreteRange,
            basegfx::B2DRange& rVisibleDiscreteRange,
            basegfx::B2DRange& rUnitVisibleRange) const
        {
            // use unit range and transform to discrete coordinates
            rDiscreteRange = basegfx::B2DRange(0.0, 0.0, 1.0, 1.0);
            rDiscreteRange.transform(rViewInformation.getViewTransformation() * getObjectTransformation());

            // force to discrete expanded bounds (it grows, so expanding works perfectly well)
            rDiscreteRange.expand(basegfx::B2DTuple(floor(rDiscreteRange.getMinX()), floor(rDiscreteRange.getMinY())));
            rDiscreteRange.expand(basegfx::B2DTuple(ceil(rDiscreteRange.getMaxX()), ceil(rDiscreteRange.getMaxY())));

            // clip it against discrete Viewport (if set)
            rVisibleDiscreteRange = rDiscreteRange;

            if(!rViewInformation.getViewport().isEmpty())
            {
                rVisibleDiscreteRange.intersect(rViewInformation.getDiscreteViewport());
            }

            // force to discrete expanded bounds, too
            rVisibleDiscreteRange.expand(basegfx::B2DTuple(floor(rVisibleDiscreteRange.getMinX()), floor(rVisibleDiscreteRange.getMinY())));
            rVisibleDiscreteRange.expand(basegfx::B2DTuple(ceil(rVisibleDiscreteRange.getMaxX()), ceil(rVisibleDiscreteRange.getMaxY())));

            if(rVisibleDiscreteRange.isEmpty())
            {
                rUnitVisibleRange = rVisibleDiscreteRange;
            }
            else
            {
                // create UnitVisibleRange containing unit range values [0.0 .. 1.0] describing
                // the relative position of rVisibleDiscreteRange inside rDiscreteRange
                const double fDiscreteScaleFactorX(basegfx::fTools::equalZero(rDiscreteRange.getWidth()) ? 1.0 : 1.0 / rDiscreteRange.getWidth());
                const double fDiscreteScaleFactorY(basegfx::fTools::equalZero(rDiscreteRange.getHeight()) ? 1.0 : 1.0 / rDiscreteRange.getHeight());

                const double fMinX(basegfx::fTools::equal(rVisibleDiscreteRange.getMinX(), rDiscreteRange.getMinX())
                    ? 0.0
                    : (rVisibleDiscreteRange.getMinX() - rDiscreteRange.getMinX()) * fDiscreteScaleFactorX);
                const double fMinY(basegfx::fTools::equal(rVisibleDiscreteRange.getMinY(), rDiscreteRange.getMinY())
                    ? 0.0
                    : (rVisibleDiscreteRange.getMinY() - rDiscreteRange.getMinY()) * fDiscreteScaleFactorY);

                const double fMaxX(basegfx::fTools::equal(rVisibleDiscreteRange.getMaxX(), rDiscreteRange.getMaxX())
                    ? 1.0
                    : (rVisibleDiscreteRange.getMaxX() - rDiscreteRange.getMinX()) * fDiscreteScaleFactorX);
                const double fMaxY(basegfx::fTools::equal(rVisibleDiscreteRange.getMaxY(), rDiscreteRange.getMaxY())
                    ? 1.0
                    : (rVisibleDiscreteRange.getMaxY() - rDiscreteRange.getMinY()) * fDiscreteScaleFactorY);

                rUnitVisibleRange = basegfx::B2DRange(fMinX, fMinY, fMaxX, fMaxY);
            }
        }

        Primitive2DSequence ScenePrimitive2D::createLocalDecomposition(const geometry::ViewInformation2D& rViewInformation) const
        {
            Primitive2DSequence aRetval;

            // create 2D shadows from contained 3D primitives. This creates the shadow primitives on demand and tells if
            // there are some or not. Do this at start, the shadow might still be visible even when the scene is not
            if(impGetShadow3D(rViewInformation))
            {
                // test visibility
                const basegfx::B2DRange aShadow2DRange(getB2DRangeFromPrimitive2DSequence(maShadowPrimitives, rViewInformation));
                const basegfx::B2DRange aViewRange(rViewInformation.getViewport());

                if(aViewRange.isEmpty() || aShadow2DRange.overlaps(aViewRange))
                {
                    // add extracted 2d shadows (before 3d scene creations itself)
                    aRetval = maShadowPrimitives;
                }
            }

            // get the involved ranges (see helper method calculateDsicreteSizes for details)
            basegfx::B2DRange aDiscreteRange;
            basegfx::B2DRange aVisibleDiscreteRange;
            basegfx::B2DRange aUnitVisibleRange;
            calculateDsicreteSizes(rViewInformation, aDiscreteRange, aVisibleDiscreteRange, aUnitVisibleRange);

            if(!aVisibleDiscreteRange.isEmpty())
            {
                // test if discrete view size (pixel) maybe too big and limit it
                double fViewSizeX(aVisibleDiscreteRange.getWidth());
                double fViewSizeY(aVisibleDiscreteRange.getHeight());
                const double fViewVisibleArea(fViewSizeX * fViewSizeY);
                const SvtOptionsDrawinglayer aDrawinglayerOpt;
                const double fMaximumVisibleArea(aDrawinglayerOpt.GetQuadratic3DRenderLimit());
                double fReduceFactor(1.0);

                if(fViewVisibleArea > fMaximumVisibleArea)
                {
                    fReduceFactor = sqrt(fMaximumVisibleArea / fViewVisibleArea);
                    fViewSizeX *= fReduceFactor;
                    fViewSizeY *= fReduceFactor;
                }

                // calculate logic render size in world coordinates for usage in renderer
                basegfx::B2DVector aLogicRenderSize(
                    aDiscreteRange.getWidth() * fReduceFactor,
                    aDiscreteRange.getHeight() * fReduceFactor);
                aLogicRenderSize *= rViewInformation.getInverseViewTransformation();

                // determine the oversample value
                static bool bDoOversample(false);
                static sal_uInt16 nDefaultOversampleValue(3);
                const sal_uInt16 nOversampleValue((bDoOversample || aDrawinglayerOpt.IsAntiAliasing()) ? nDefaultOversampleValue : 0);

                // use default 3D primitive processor to create BitmapEx for aUnitVisiblePart and process
                processor3d::ZBufferProcessor3D aZBufferProcessor3D(
                    rViewInformation,
                    getTransformation3D(),
                    getSdrSceneAttribute(),
                    getSdrLightingAttribute(),
                    aLogicRenderSize.getX(),
                    aLogicRenderSize.getY(),
                    aUnitVisibleRange,
                    nOversampleValue);

                aZBufferProcessor3D.processNonTransparent(getChildren3D());
                aZBufferProcessor3D.processTransparent(getChildren3D());
                const BitmapEx aNewBitmap(aZBufferProcessor3D.getBitmapEx());
                const Size aBitmapSizePixel(aNewBitmap.GetSizePixel());

                if(aBitmapSizePixel.getWidth() && aBitmapSizePixel.getHeight())
                {
                    // create transform for the created bitmap in discrete coordinates first
                    basegfx::B2DHomMatrix aNew2DTransform;

                    aNew2DTransform.set(0, 0, aVisibleDiscreteRange.getWidth());
                    aNew2DTransform.set(1, 1, aVisibleDiscreteRange.getHeight());
                    aNew2DTransform.set(0, 2, aVisibleDiscreteRange.getMinX());
                    aNew2DTransform.set(1, 2, aVisibleDiscreteRange.getMinY());

                    // transform back to world coordinates for usage in primitive creation
                    aNew2DTransform *= rViewInformation.getInverseViewTransformation();

                    // create bitmap primitive and add
                    const Primitive2DReference xRef(new BitmapPrimitive2D(aNewBitmap, aNew2DTransform));
                    appendPrimitive2DReferenceToPrimitive2DSequence(aRetval, xRef);

                    // test: Allow to add an outline in the debugger when tests are needed
                    static bool bAddOutlineToCreated3DSceneRepresentation(false);

                    if(bAddOutlineToCreated3DSceneRepresentation)
                    {
                        basegfx::B2DPolygon aOutline(basegfx::tools::createPolygonFromRect(basegfx::B2DRange(0.0, 0.0, 1.0, 1.0)));
                        aOutline.transform(aNew2DTransform);
                        const Primitive2DReference xRef2(new PolygonHairlinePrimitive2D(aOutline, basegfx::BColor(1.0, 0.0, 0.0)));
                        appendPrimitive2DReferenceToPrimitive2DSequence(aRetval, xRef2);
                    }
                }
            }

            // create 2D labels from contained 3D label primitives. This creates the label primitives on demand and tells if
            // there are some or not. Do this at end, the labels might still be visible even when the scene is not
            if(impGetLabel3D(rViewInformation))
            {
                // test visibility
                const basegfx::B2DRange aLabel2DRange(getB2DRangeFromPrimitive2DSequence(maLabelPrimitives, rViewInformation));
                const basegfx::B2DRange aViewRange(rViewInformation.getViewport());

                if(aViewRange.isEmpty() || aLabel2DRange.overlaps(aViewRange))
                {
                    // add extracted 2d labels (after 3d scene creations)
                    appendPrimitive2DSequenceToPrimitive2DSequence(aRetval, maLabelPrimitives);
                }
            }

            return aRetval;
        }

        Primitive2DSequence ScenePrimitive2D::getGeometry2D(const geometry::ViewInformation2D& rViewInformation) const
        {
            Primitive2DSequence aRetval;

            // create 2D shadows from contained 3D primitives
            if(impGetShadow3D(rViewInformation))
            {
                // add extracted 2d shadows (before 3d scene creations itself)
                aRetval = maShadowPrimitives;
            }

            // create 2D projected geometry from 3D geometry
            if(getChildren3D().hasElements())
            {
                // create 2D geometry extraction processor
                processor3d::Geometry2DExtractingProcessor aGeometryProcessor(
                    rViewInformation.getViewTime(),
                    getObjectTransformation(),
                    getTransformation3D().getWorldToView());

                // process local primitives
                aGeometryProcessor.process(getChildren3D());

                // fetch result and append
                Primitive2DSequence a2DExtractedPrimitives(aGeometryProcessor.getPrimitive2DSequence());
                appendPrimitive2DSequenceToPrimitive2DSequence(aRetval, a2DExtractedPrimitives);
            }

            // create 2D labels from contained 3D label primitives
            if(impGetLabel3D(rViewInformation))
            {
                // add extracted 2d labels (after 3d scene creations)
                appendPrimitive2DSequenceToPrimitive2DSequence(aRetval, maLabelPrimitives);
            }

            return aRetval;
        }

        ScenePrimitive2D::ScenePrimitive2D(
            const primitive3d::Primitive3DSequence& rxChildren3D,
            const attribute::SdrSceneAttribute& rSdrSceneAttribute,
            const attribute::SdrLightingAttribute& rSdrLightingAttribute,
            const basegfx::B2DHomMatrix& rObjectTransformation,
            const geometry::Transformation3D& rTransformation3D)
        :   BasePrimitive2D(),
            mxChildren3D(rxChildren3D),
            maSdrSceneAttribute(rSdrSceneAttribute),
            maSdrLightingAttribute(rSdrLightingAttribute),
            maObjectTransformation(rObjectTransformation),
            maTransformation3D(rTransformation3D),
            mbShadow3DChecked(false),
            mbLabel3DChecked(false),
            mfOldDiscreteSizeX(0.0),
            mfOldDiscreteSizeY(0.0),
            maOldUnitVisiblePart()
        {
        }

        bool ScenePrimitive2D::operator==(const BasePrimitive2D& rPrimitive) const
        {
            if(BasePrimitive2D::operator==(rPrimitive))
            {
                const ScenePrimitive2D& rCompare = (ScenePrimitive2D&)rPrimitive;

                return (primitive3d::arePrimitive3DSequencesEqual(getChildren3D(), rCompare.getChildren3D())
                    && getSdrSceneAttribute() == rCompare.getSdrSceneAttribute()
                    && getSdrLightingAttribute() == rCompare.getSdrLightingAttribute()
                    && getObjectTransformation() == rCompare.getObjectTransformation()
                    && getTransformation3D() == rCompare.getTransformation3D());
            }

            return false;
        }

        basegfx::B2DRange ScenePrimitive2D::getB2DRange(const geometry::ViewInformation2D& rViewInformation) const
        {
            // transform unit range to discrete coordinate range
            basegfx::B2DRange aRetval(0.0, 0.0, 1.0, 1.0);
            aRetval.transform(rViewInformation.getViewTransformation() * getObjectTransformation());

            // force to discrete expanded bounds (it grows, so expanding works perfectly well)
            aRetval.expand(basegfx::B2DTuple(floor(aRetval.getMinX()), floor(aRetval.getMinY())));
            aRetval.expand(basegfx::B2DTuple(ceil(aRetval.getMaxX()), ceil(aRetval.getMaxY())));

            // transform back from discrete (view) to world coordinates
            aRetval.transform(rViewInformation.getInverseViewTransformation());

            // expand by evtl. existing shadow primitives
            if(impGetShadow3D(rViewInformation))
            {
                const basegfx::B2DRange aShadow2DRange(getB2DRangeFromPrimitive2DSequence(maShadowPrimitives, rViewInformation));

                if(!aShadow2DRange.isEmpty())
                {
                    aRetval.expand(aShadow2DRange);
                }
            }

            // expand by evtl. existing label primitives
            if(impGetLabel3D(rViewInformation))
            {
                const basegfx::B2DRange aLabel2DRange(getB2DRangeFromPrimitive2DSequence(maLabelPrimitives, rViewInformation));

                if(!aLabel2DRange.isEmpty())
                {
                    aRetval.expand(aLabel2DRange);
                }
            }

            return aRetval;
        }

        Primitive2DSequence ScenePrimitive2D::get2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const
        {
            ::osl::MutexGuard aGuard( m_aMutex );

            // get the involved ranges (see helper method calculateDsicreteSizes for details)
            bool bNeedNewDecomposition(false);
            basegfx::B2DRange aDiscreteRange;
            basegfx::B2DRange aVisibleDiscreteRange;
            basegfx::B2DRange aUnitVisibleRange;
            calculateDsicreteSizes(rViewInformation, aDiscreteRange, aVisibleDiscreteRange, aUnitVisibleRange);

            if(getLocalDecomposition().hasElements())
            {
                // display has changed and cannot be reused when resolution did change
                if(!basegfx::fTools::equal(aVisibleDiscreteRange.getWidth(), mfOldDiscreteSizeX) ||
                    !basegfx::fTools::equal(aVisibleDiscreteRange.getHeight(), mfOldDiscreteSizeY))
                {
                    bNeedNewDecomposition = true;
                }

                // display has changed and cannot be reused when the shown relative part did change
                if(!bNeedNewDecomposition
                    && !aUnitVisibleRange.equal(maOldUnitVisiblePart))
                {
                    bNeedNewDecomposition = true;
                }
            }

            if(bNeedNewDecomposition)
            {
                // conditions of last local decomposition have changed, delete
                const_cast< ScenePrimitive2D* >(this)->setLocalDecomposition(Primitive2DSequence());
            }

            if(!getLocalDecomposition().hasElements())
            {
                // remember last used NewDiscreteSize and NewUnitVisiblePart
                ScenePrimitive2D* pThat = const_cast< ScenePrimitive2D* >(this);
                pThat->mfOldDiscreteSizeX = aVisibleDiscreteRange.getWidth();
                pThat->mfOldDiscreteSizeY = aVisibleDiscreteRange.getHeight();
                pThat->maOldUnitVisiblePart = aUnitVisibleRange;
            }

            // use parent implementation
            return BasePrimitive2D::get2DDecomposition(rViewInformation);
        }

        bool ScenePrimitive2D::impGetShadow3D(const geometry::ViewInformation2D& rViewInformation) const
        {
            osl::MutexGuard aGuard( m_aMutex );

            // create on demand
            if(!mbShadow3DChecked && getChildren3D().hasElements())
            {
                // create shadow extraction processor
                const basegfx::B3DHomMatrix aWorldToEye(getTransformation3D().getOrientation() * getTransformation3D().getTransformation());
                const basegfx::B3DHomMatrix aEyeToView(getTransformation3D().getDeviceToView() * getTransformation3D().getProjection());

                processor3d::Shadow3DExtractingProcessor aShadowProcessor(
                    rViewInformation.getViewTime(),
                    getObjectTransformation(),
                    aWorldToEye,
                    aEyeToView,
                    getSdrLightingAttribute(),
                    getChildren3D(),
                    getSdrSceneAttribute().getShadowSlant());

                // process local primitives
                aShadowProcessor.process(getChildren3D());

                // fetch result and set checked flag
                const_cast< ScenePrimitive2D* >(this)->maShadowPrimitives = aShadowProcessor.getPrimitive2DSequence();
                const_cast< ScenePrimitive2D* >(this)->mbShadow3DChecked = true;
            }

            // return if there are shadow primitives
            return maShadowPrimitives.hasElements();
        }

        bool ScenePrimitive2D::impGetLabel3D(const geometry::ViewInformation2D& rViewInformation) const
        {
            osl::MutexGuard aGuard( m_aMutex );

            // create on demand
            if(!mbLabel3DChecked && getChildren3D().hasElements())
            {
                // create label extraction processor
                processor3d::Label3DExtractingProcessor aLabelProcessor(
                    rViewInformation.getViewTime(),
                    getObjectTransformation(),
                    getTransformation3D().getWorldToView());

                // process local primitives
                aLabelProcessor.process(getChildren3D());

                // fetch result and set checked flag
                const_cast< ScenePrimitive2D* >(this)->maLabelPrimitives = aLabelProcessor.getPrimitive2DSequence();
                const_cast< ScenePrimitive2D* >(this)->mbLabel3DChecked = true;
            }

            // return if there are label primitives
            return maLabelPrimitives.hasElements();
        }

        // provide unique ID
        ImplPrimitrive2DIDBlock(ScenePrimitive2D, PRIMITIVE2D_ID_SCENEPRIMITIVE2D)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
