/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ShapeFactory.cxx,v $
 * $Revision: 1.25.44.1 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_chart2.hxx"
#include "ShapeFactory.hxx"
#include "ViewDefines.hxx"
#include "Stripe.hxx"
#include "CommonConverters.hxx"
#include "macros.hxx"
#include "PropertyMapper.hxx"
#include <comphelper/InlineContainer.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/drawing/CircleKind.hpp>
#include <com/sun/star/drawing/DoubleSequence.hpp>
#include <com/sun/star/drawing/FlagSequence.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/drawing/NormalsKind.hpp>
#include <com/sun/star/drawing/PointSequence.hpp>
#include <com/sun/star/drawing/PolygonKind.hpp>
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#include <com/sun/star/drawing/ProjectionMode.hpp>
#include <com/sun/star/drawing/ShadeMode.hpp>
#include <com/sun/star/drawing/TextFitToSizeType.hpp>
#include <com/sun/star/drawing/TextureProjectionMode.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/uno/Any.hxx>


#include <svx/unoprnms.hxx>
#include <tools/color.hxx>
#include <tools/debug.hxx>
#include <rtl/math.hxx>
#include <svx/svdocirc.hxx>
#include <svx/svdopath.hxx>

#ifndef _BGFX_VECTOR_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#include <basegfx/matrix/b3dhommatrix.hxx>

#include <algorithm>

using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;

//.............................................................................
namespace chart
{
//.............................................................................

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// set a name/CID at a shape (is used for selection handling)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//static
void ShapeFactory::setShapeName( const uno::Reference< drawing::XShape >& xShape
                               , const rtl::OUString& rName )
{
    if(!xShape.is())
        return;
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            xProp->setPropertyValue( C2U( UNO_NAME_MISC_OBJ_NAME )
                , uno::makeAny( rName ) );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
}

//-----------------------------------------------------------------------------

//static
rtl::OUString ShapeFactory::getShapeName( const uno::Reference< drawing::XShape >& xShape )
{
    rtl::OUString aRet;

    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            xProp->getPropertyValue( C2U( UNO_NAME_MISC_OBJ_NAME ) ) >>= aRet;
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }

    return aRet;
}

//-----------------------------------------------------------------------------

uno::Reference< drawing::XShapes > ShapeFactory::getChartRootShape(
    const uno::Reference< drawing::XDrawPage>& xDrawPage )
{
    uno::Reference< drawing::XShapes > xRet;
    uno::Reference< drawing::XShapes > xShapes( xDrawPage, uno::UNO_QUERY );
    if( xShapes.is() )
    {
        sal_Int32 nCount = xShapes->getCount();
        uno::Reference< drawing::XShape > xShape;
        for( sal_Int32 nN = nCount; nN--; )
        {
            if( xShapes->getByIndex( nN ) >>= xShape )
            {
                if( ShapeFactory::getShapeName( xShape ).equals(C2U("com.sun.star.chart2.shapes")) )
                {
                    xRet = uno::Reference< drawing::XShapes >( xShape, uno::UNO_QUERY );
                    break;
                }
            }
        }
    }
    return xRet;
}

//-----------------------------------------------------------------------------

uno::Reference< drawing::XShapes > ShapeFactory::getOrCreateChartRootShape(
    const uno::Reference< drawing::XDrawPage>& xDrawPage )
{
    uno::Reference< drawing::XShapes > xRet( ShapeFactory::getChartRootShape( xDrawPage ) );
    if( !xRet.is()  )
    {
        //create the root shape
        xRet = this->createGroup2D(
            uno::Reference<drawing::XShapes>( xDrawPage, uno::UNO_QUERY )
            , C2U("com.sun.star.chart2.shapes") );
    }
    return xRet;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  diverse PolyPolygon create methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

uno::Any createPolyPolygon_Cube(
            const drawing::Direction3D& rSize, double fRoundedEdge, bool bRounded = true )
{
    DBG_ASSERT(fRoundedEdge>=0, "fRoundedEdge needs to be >= 0");

    // always use extra points, so set percent diagonal to 0.4 which is 0% in the UI (old Chart comment)
    if( fRoundedEdge == 0.0  && bRounded)
        fRoundedEdge = 0.4 / 200.0;
    else if(!bRounded)
        fRoundedEdge = 0.0;

    //fWidthH stands for Half Width
    const double fWidthH = rSize.DirectionX >=0.0?  rSize.DirectionX/2.0  : -rSize.DirectionX/2.0;
    const double fHeight = rSize.DirectionY;
//     const double fDepth  = rSize.DirectionZ >=0.0?  rSize.DirectionZ      : -rSize.DirectionZ ;

    const double fHeightSign = fHeight >= 0.0 ? 1.0 : -1.0;

    const double fOffset = (fWidthH * fRoundedEdge) * 1.05; // increase by 5% for safety
    const bool bRoundEdges = fRoundedEdge && fOffset < fWidthH && 2.0 * fOffset < fHeightSign*fHeight;
    const sal_Int32 nPointCount = bRoundEdges ? 13 : 5;

    //--------------------------------------
    drawing::PolyPolygonShape3D aPP;

    aPP.SequenceX.realloc(1);
    aPP.SequenceY.realloc(1);
    aPP.SequenceZ.realloc(1);

    drawing::DoubleSequence* pOuterSequenceX = aPP.SequenceX.getArray();
    drawing::DoubleSequence* pOuterSequenceY = aPP.SequenceY.getArray();
    drawing::DoubleSequence* pOuterSequenceZ = aPP.SequenceZ.getArray();

    pOuterSequenceX->realloc(nPointCount);
    pOuterSequenceY->realloc(nPointCount);
    pOuterSequenceZ->realloc(nPointCount);

    double* pInnerSequenceX = pOuterSequenceX->getArray();
    double* pInnerSequenceY = pOuterSequenceY->getArray();
    double* pInnerSequenceZ = pOuterSequenceZ->getArray();

    for(sal_Int32 nN = nPointCount; nN--;)
        *pInnerSequenceZ++ = 0.0;
        //*pInnerSequenceZ++ = -fDepth/2.0;

    if(nPointCount == 5)
    {
        *pInnerSequenceY++ = 0.0;
        *pInnerSequenceY++ = 0.0;
        *pInnerSequenceY++ = fHeight;
        *pInnerSequenceY++ = fHeight;
        *pInnerSequenceY++ = 0.0;

        *pInnerSequenceX++ = -fWidthH;
        *pInnerSequenceX++ = fWidthH;
        *pInnerSequenceX++ = fWidthH;
        *pInnerSequenceX++ = -fWidthH;
        *pInnerSequenceX++ = -fWidthH;
    }
    else
    {
        *pInnerSequenceY++ = 0.0;
        *pInnerSequenceY++ = 0.0;
        *pInnerSequenceY++ = 0.0;
        *pInnerSequenceY++ = fHeightSign*fOffset;
        *pInnerSequenceY++ = fHeight - fHeightSign*fOffset;
        *pInnerSequenceY++ = fHeight;
        *pInnerSequenceY++ = fHeight;
        *pInnerSequenceY++ = fHeight;
        *pInnerSequenceY++ = fHeight;
        *pInnerSequenceY++ = fHeight - fHeightSign*fOffset;
        *pInnerSequenceY++ = fHeightSign*fOffset;
        *pInnerSequenceY++ = 0.0;
        *pInnerSequenceY++ = 0.0;

        *pInnerSequenceX++ = -fWidthH + fOffset;
        *pInnerSequenceX++ = fWidthH - fOffset;
        *pInnerSequenceX++ = fWidthH;
        *pInnerSequenceX++ = fWidthH;
        *pInnerSequenceX++ = fWidthH;
        *pInnerSequenceX++ = fWidthH;
        *pInnerSequenceX++ = fWidthH - fOffset;
        *pInnerSequenceX++ = -fWidthH + fOffset;
        *pInnerSequenceX++ = -fWidthH;
        *pInnerSequenceX++ = -fWidthH;
        *pInnerSequenceX++ = -fWidthH;
        *pInnerSequenceX++ = -fWidthH;
        *pInnerSequenceX++ = -fWidthH + fOffset;
    }
    return uno::Any( &aPP, ::getCppuType((const drawing::PolyPolygonShape3D*)0) );
}

uno::Any createPolyPolygon_Cylinder(
             double fHeight
           , double fRadius
           , sal_Int32& nVerticalSegmentCount )
{
    //fHeight may be negative
    DBG_ASSERT(fRadius>0, "The radius of a cylinder needs to be > 0");

    //--------------------------------------
    drawing::PolyPolygonShape3D aPP;

    nVerticalSegmentCount=1;

    aPP.SequenceX.realloc(3);
    aPP.SequenceY.realloc(3);
    aPP.SequenceZ.realloc(3);

    drawing::DoubleSequence* pOuterSequenceX = aPP.SequenceX.getArray();
    drawing::DoubleSequence* pOuterSequenceY = aPP.SequenceY.getArray();
    drawing::DoubleSequence* pOuterSequenceZ = aPP.SequenceZ.getArray();

    pOuterSequenceX->realloc(2);
    pOuterSequenceY->realloc(2);
    pOuterSequenceZ->realloc(2);

    double* pInnerSequenceX = pOuterSequenceX->getArray();
    double* pInnerSequenceY = pOuterSequenceY->getArray();
    double* pInnerSequenceZ = pOuterSequenceZ->getArray();

    double fY1 = 0.0;
    double fY2 = fHeight;

    if( fHeight<0.0 )
        ::std::swap(fY1,fY2);

    //----------------------------
    for(sal_Int32 nN = 2; nN--;)
        *pInnerSequenceZ++ = 0.0;

    *pInnerSequenceX++ = 0.0;
    *pInnerSequenceY++ = fY1;

    *pInnerSequenceX++ = fRadius;
    *pInnerSequenceY++ = fY1;
    //----------------------------

    pOuterSequenceX++;pOuterSequenceY++;pOuterSequenceZ++;
    pOuterSequenceX->realloc(2);
    pOuterSequenceY->realloc(2);
    pOuterSequenceZ->realloc(2);

    pInnerSequenceX = pOuterSequenceX->getArray();
    pInnerSequenceY = pOuterSequenceY->getArray();
    pInnerSequenceZ = pOuterSequenceZ->getArray();

    //----------------------------
    for(sal_Int32 nN = 2; nN--;)
        *pInnerSequenceZ++ = 0.0;

    *pInnerSequenceX++ = fRadius;
    *pInnerSequenceY++ = fY1;

    *pInnerSequenceX++ = fRadius;
    *pInnerSequenceY++ = fY2;
    //----------------------------

    pOuterSequenceX++;pOuterSequenceY++;pOuterSequenceZ++;
    pOuterSequenceX->realloc(2);
    pOuterSequenceY->realloc(2);
    pOuterSequenceZ->realloc(2);

    pInnerSequenceX = pOuterSequenceX->getArray();
    pInnerSequenceY = pOuterSequenceY->getArray();
    pInnerSequenceZ = pOuterSequenceZ->getArray();

    //----------------------------
    for(sal_Int32 nN = 2; nN--;)
        *pInnerSequenceZ++ = 0.0;

    *pInnerSequenceX++ = fRadius;
    *pInnerSequenceY++ = fY2;

    *pInnerSequenceX++ = 0.0;
    *pInnerSequenceY++ = fY2;
    //----------------------------

    return uno::Any( &aPP, ::getCppuType((const drawing::PolyPolygonShape3D*)0) );
}

uno::Any createPolyPolygon_Cone( double fHeight, double fRadius, double fTopHeight
            , sal_Int32& nVerticalSegmentCount )
{
    DBG_ASSERT(fRadius>0, "The radius of a cone needs to be > 0");

    //for stacked charts we need cones without top -> fTopHeight != 0 resp. bTopless == true
    //fTopHeight indicates the high of the cutted top only (not the full height)
    bool bTopless = !::rtl::math::approxEqual( fHeight, fHeight + fTopHeight );

    double r1= 0.0, r2 = fRadius;
    if(bTopless)
        // #i63212# fHeight may be negative, fTopHeight is always positive -> use fabs(fHeight)
        r1 = fRadius * (fTopHeight)/(fabs(fHeight)+fTopHeight);

    nVerticalSegmentCount=1;
    drawing::PolyPolygonShape3D aPP;

    aPP.SequenceX.realloc(2);
    aPP.SequenceY.realloc(2);
    aPP.SequenceZ.realloc(2);

    drawing::DoubleSequence* pOuterSequenceX = aPP.SequenceX.getArray();
    drawing::DoubleSequence* pOuterSequenceY = aPP.SequenceY.getArray();
    drawing::DoubleSequence* pOuterSequenceZ = aPP.SequenceZ.getArray();

    pOuterSequenceX->realloc(2);
    pOuterSequenceY->realloc(2);
    pOuterSequenceZ->realloc(2);

    double* pInnerSequenceX = pOuterSequenceX->getArray();
    double* pInnerSequenceY = pOuterSequenceY->getArray();
    double* pInnerSequenceZ = pOuterSequenceZ->getArray();

    double fX1 = 0.0;
    double fX2 = r2;
    double fX3 = r1;

    double fY1 = 0.0;
    double fY2 = 0.0;
    double fY3 = fHeight;

    if( fHeight<0.0 )
    {
        ::std::swap(fX1,fX3);
        ::std::swap(fY1,fY3);
    }

    //----------------------------
    for(sal_Int32 nN = 2; nN--;)
        *pInnerSequenceZ++ = 0.0;

    *pInnerSequenceY++ = fY1;
    *pInnerSequenceX++ = fX1;

    *pInnerSequenceY++ = fY2;
    *pInnerSequenceX++ = fX2;
    //----------------------------

    pOuterSequenceX++;pOuterSequenceY++;pOuterSequenceZ++;
    pOuterSequenceX->realloc(2);
    pOuterSequenceY->realloc(2);
    pOuterSequenceZ->realloc(2);

    pInnerSequenceX = pOuterSequenceX->getArray();
    pInnerSequenceY = pOuterSequenceY->getArray();
    pInnerSequenceZ = pOuterSequenceZ->getArray();

    //----------------------------
    for(sal_Int32 nN = 2; nN--;)
        *pInnerSequenceZ++ = 0.0;

    *pInnerSequenceY++ = fY2;
    *pInnerSequenceX++ = fX2;

    *pInnerSequenceY++ = fY3;
    *pInnerSequenceX++ = fX3;
    //----------------------------

    return uno::Any( &aPP, ::getCppuType((const drawing::PolyPolygonShape3D*)0) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  methods for 3D shape creation
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

uno::Reference<drawing::XShape>
        ShapeFactory::createCube(
            const uno::Reference<drawing::XShapes>& xTarget
            , const drawing::Position3D& rPosition, const drawing::Direction3D& rSize
            , sal_Int32 nRotateZAngleHundredthDegree
            , const uno::Reference< beans::XPropertySet >& xSourceProp
            , const tPropertyNameMap& rPropertyNameMap
            , bool bRounded )
{
    if( !xTarget.is() )
        return 0;
    if( bRounded )
    {
        try
        {
            if( xSourceProp.is() )
            {
                drawing::LineStyle aLineStyle;
                xSourceProp->getPropertyValue( C2U( "BorderStyle" ) ) >>= aLineStyle;
                if( aLineStyle == drawing::LineStyle_SOLID )
                    bRounded = false;
            }
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    uno::Reference<drawing::XShape> xShape = impl_createCube( xTarget, rPosition, rSize, nRotateZAngleHundredthDegree, bRounded );
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    if( xSourceProp.is())
        PropertyMapper::setMappedProperties( xProp, xSourceProp, rPropertyNameMap );
    return xShape;
}

uno::Reference<drawing::XShape>
        ShapeFactory::impl_createCube(
              const uno::Reference<drawing::XShapes>& xTarget
            , const drawing::Position3D& rPosition, const drawing::Direction3D& rSize
            , sal_Int32 nRotateZAngleHundredthDegree
            , bool bRounded )
{
    if( !xTarget.is() )
        return 0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.Shape3DExtrudeObject") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            //depth
            double fDepth = rSize.DirectionZ;
            if(fDepth<0)
                fDepth*=-1.0;
            xProp->setPropertyValue( C2U( UNO_NAME_3D_EXTRUDE_DEPTH )
                , uno::makeAny((sal_Int32)fDepth) );

            //PercentDiagonal
            sal_Int16 nPercentDiagonal = bRounded ? 3 : 0;
            xProp->setPropertyValue( C2U( UNO_NAME_3D_PERCENT_DIAGONAL )
                , uno::makeAny( nPercentDiagonal ) );

            //Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_3D_POLYPOLYGON3D )
                , createPolyPolygon_Cube( rSize, double(nPercentDiagonal)/200.0,bRounded) );

            //Matrix for position
            {
                ::basegfx::B3DHomMatrix aM;
                if(nRotateZAngleHundredthDegree!=0)
                    aM.rotate(0.0,0.0,-nRotateZAngleHundredthDegree/18000.00*F_PI);
                aM.translate(rPosition.PositionX
                            , rPosition.PositionY
                            , rPosition.PositionZ - (fDepth/2.0));
                drawing::HomogenMatrix aHM = B3DHomMatrixToHomogenMatrix(aM);
                xProp->setPropertyValue( C2U( UNO_NAME_3D_TRANSFORM_MATRIX )
                    , uno::makeAny(aHM) );
            }
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Reference<drawing::XShape>
        ShapeFactory::createCylinder(
            const uno::Reference<drawing::XShapes>& xTarget
          , const drawing::Position3D& rPosition, const drawing::Direction3D& rSize
          , sal_Int32 nRotateZAngleHundredthDegree )
{
    return impl_createConeOrCylinder(
              xTarget, rPosition, rSize, 0.0, nRotateZAngleHundredthDegree, true );
}

uno::Reference<drawing::XShape>
        ShapeFactory::createPyramid(
            const uno::Reference<drawing::XShapes>& xTarget
          , const drawing::Position3D& rPosition, const drawing::Direction3D& rSize
          , double fTopHeight, bool bRotateZ
          , const uno::Reference< beans::XPropertySet >& xSourceProp
          , const tPropertyNameMap& rPropertyNameMap )
{
    if( !xTarget.is() )
        return 0;

    Reference< drawing::XShapes > xGroup( ShapeFactory::createGroup3D( xTarget, rtl::OUString() ) );

    sal_Bool bDoubleSided = true;
    bool bRotatedTexture = false;

    const double fWidth = rSize.DirectionX;
    const double fDepth = rSize.DirectionZ;
    const double fHeight = rSize.DirectionY;

    drawing::Position3D aBottomP1( rPosition.PositionX, rPosition.PositionY, rPosition.PositionZ - fDepth/2.0  );
    if(bRotateZ)
        aBottomP1.PositionY -= fWidth/2.0;
    else
        aBottomP1.PositionX -= fWidth/2.0;
    drawing::Position3D aBottomP2( aBottomP1 );
    if(bRotateZ)
        aBottomP2.PositionY += fWidth;
    else
        aBottomP2.PositionX += fWidth;
    drawing::Position3D aBottomP3( aBottomP2 );
    drawing::Position3D aBottomP4( aBottomP1 );
    aBottomP3.PositionZ += fDepth;
    aBottomP4.PositionZ += fDepth;

    const double fTopFactor = (fTopHeight)/(fabs(fHeight)+fTopHeight);
    drawing::Position3D aTopP1( rPosition.PositionX, rPosition.PositionY, rPosition.PositionZ - fDepth*fTopFactor/2.0  );
    if(bRotateZ)
    {
        aTopP1.PositionY -= fWidth*fTopFactor/2.0;
        aTopP1.PositionX += fHeight;
    }
    else
    {
        aTopP1.PositionX -= fWidth*fTopFactor/2.0;
        aTopP1.PositionY += fHeight;
    }
    drawing::Position3D aTopP2( aTopP1 );
    if(bRotateZ)
        aTopP2.PositionY += fWidth*fTopFactor;
    else
        aTopP2.PositionX += fWidth*fTopFactor;
    drawing::Position3D aTopP3( aTopP2 );
    drawing::Position3D aTopP4( aTopP1 );
    aTopP3.PositionZ += fDepth*fTopFactor;
    aTopP4.PositionZ += fDepth*fTopFactor;

    Stripe aStripeBottom( aBottomP1, aBottomP4, aBottomP3, aBottomP2 );

    drawing::Position3D aNormalsBottomP1( aBottomP1 );
    drawing::Position3D aNormalsBottomP2( aBottomP2 );
    drawing::Position3D aNormalsBottomP3( aBottomP3 );
    drawing::Position3D aNormalsBottomP4( aBottomP4 );
    drawing::Position3D aNormalsTopP1( aBottomP1 );
    drawing::Position3D aNormalsTopP2( aBottomP2 );
    drawing::Position3D aNormalsTopP3( aBottomP3 );
    drawing::Position3D aNormalsTopP4( aBottomP4 );
    if( bRotateZ )
    {
        aNormalsTopP1.PositionX += fHeight;
        aNormalsTopP2.PositionX += fHeight;
        aNormalsTopP3.PositionX += fHeight;
        aNormalsTopP4.PositionX += fHeight;
    }
    else
    {
        aNormalsTopP1.PositionY += fHeight;
        aNormalsTopP2.PositionY += fHeight;
        aNormalsTopP3.PositionY += fHeight;
        aNormalsTopP4.PositionY += fHeight;
    }

    if(fHeight<0.0)
    {
        std::swap( aBottomP1, aTopP1);
        std::swap( aBottomP2, aTopP2);
        std::swap( aBottomP3, aTopP3);
        std::swap( aBottomP4, aTopP4);

        std::swap( aNormalsBottomP1, aNormalsTopP1);
        std::swap( aNormalsBottomP2, aNormalsTopP2);
        std::swap( aNormalsBottomP3, aNormalsTopP3);
        std::swap( aNormalsBottomP4, aNormalsTopP4);
    }

    Stripe aStripe1( aTopP2, aTopP1, aBottomP1, aBottomP2 );
    Stripe aStripe2( aTopP3, aTopP2, aBottomP2, aBottomP3 );
    Stripe aStripe3( aTopP4, aTopP3, aBottomP3, aBottomP4 );
    Stripe aStripe4( aTopP1, aTopP4, aBottomP4, aBottomP1 );

    Stripe aNormalsStripe1( aNormalsBottomP2, aNormalsBottomP1, aNormalsTopP1, aNormalsTopP2   );
    Stripe aNormalsStripe2( aNormalsBottomP3, aNormalsBottomP2, aNormalsTopP2, aNormalsTopP3 );
    Stripe aNormalsStripe3( aNormalsBottomP4, aNormalsBottomP3, aNormalsTopP3, aNormalsTopP4 );
    Stripe aNormalsStripe4( aNormalsBottomP1, aNormalsBottomP4, aNormalsTopP4, aNormalsTopP1 );

    if(bRotateZ)
    {
        bRotatedTexture = true;
        aStripe1 = Stripe( aTopP1, aTopP2, aBottomP2, aBottomP1 );
        aStripe2 = Stripe( aTopP2, aTopP3, aBottomP3, aBottomP2  );
        aStripe3 = Stripe( aTopP3, aTopP4, aBottomP4, aBottomP3  );
        aStripe4 = Stripe( aTopP4, aTopP1, aBottomP1, aBottomP4  );

        aNormalsStripe1 = Stripe( aNormalsBottomP1, aNormalsBottomP2, aNormalsTopP2, aNormalsTopP1   );
        aNormalsStripe2 = Stripe( aNormalsBottomP2, aNormalsBottomP3, aNormalsTopP3, aNormalsTopP2 );
        aNormalsStripe3 = Stripe( aNormalsBottomP3, aNormalsBottomP4, aNormalsTopP4, aNormalsTopP3 );
        aNormalsStripe4 = Stripe( aNormalsBottomP4, aNormalsBottomP1, aNormalsTopP1, aNormalsTopP4 );
    }

    aStripe1.SetManualNormal( aNormalsStripe1.getNormal() );
    aStripe2.SetManualNormal( aNormalsStripe2.getNormal() );
    aStripe3.SetManualNormal( aNormalsStripe3.getNormal() );
    aStripe4.SetManualNormal( aNormalsStripe4.getNormal() );

    const bool bFlatNormals = false;
    ShapeFactory::createStripe( xGroup, aStripe1, xSourceProp, rPropertyNameMap, bDoubleSided, bRotatedTexture, bFlatNormals );
    ShapeFactory::createStripe( xGroup, aStripe2, xSourceProp, rPropertyNameMap, bDoubleSided, bRotatedTexture, bFlatNormals );
    ShapeFactory::createStripe( xGroup, aStripe3, xSourceProp, rPropertyNameMap, bDoubleSided, bRotatedTexture, bFlatNormals );
    ShapeFactory::createStripe( xGroup, aStripe4, xSourceProp, rPropertyNameMap, bDoubleSided, bRotatedTexture, bFlatNormals );
    ShapeFactory::createStripe( xGroup, aStripeBottom, xSourceProp, rPropertyNameMap, bDoubleSided, bRotatedTexture );

    return Reference< drawing::XShape >( xGroup, uno::UNO_QUERY );
}

uno::Reference<drawing::XShape>
        ShapeFactory::createCone(
            const uno::Reference<drawing::XShapes>& xTarget
          , const drawing::Position3D& rPosition, const drawing::Direction3D& rSize
          , double fTopHeight, sal_Int32 nRotateZAngleHundredthDegree )
{
    return impl_createConeOrCylinder( xTarget, rPosition, rSize, fTopHeight, nRotateZAngleHundredthDegree );
}

uno::Reference<drawing::XShape>
        ShapeFactory::impl_createConeOrCylinder(
              const uno::Reference<drawing::XShapes>& xTarget
            , const drawing::Position3D& rPosition, const drawing::Direction3D& rSize
            , double fTopHeight, sal_Int32 nRotateZAngleHundredthDegree
            , bool bCylinder )
{
    if( !xTarget.is() )
        return 0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
            m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.Shape3DLatheObject") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    double fWidth      = rSize.DirectionX*0.6; //The depth will be corrrected within Matrix
    double fRadius     = fWidth; //!!!!!!!! problem in drawing layer: rotation object calculates wrong needed size -> wrong camera (it's a problem with bounding boxes)
    double fHeight     = rSize.DirectionY;

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            //PercentDiagonal
            sal_Int16 nPercentDiagonal = 5;
            xProp->setPropertyValue( C2U( UNO_NAME_3D_PERCENT_DIAGONAL )
                , uno::makeAny( nPercentDiagonal ) );

            //Polygon
            sal_Int32 nVerticalSegmentCount = 0;
            uno::Any aPPolygon = bCylinder ? createPolyPolygon_Cylinder(
                                                fHeight, fRadius, nVerticalSegmentCount)
                                           : createPolyPolygon_Cone(
                                                fHeight, fRadius, fTopHeight, nVerticalSegmentCount);
            xProp->setPropertyValue( C2U( UNO_NAME_3D_POLYPOLYGON3D ), aPPolygon );

            //Matrix for position
            {
                ::basegfx::B3DHomMatrix aM;
                if(nRotateZAngleHundredthDegree!=0)
                    aM.rotate(0.0,0.0,-nRotateZAngleHundredthDegree/18000.00*F_PI);
                //stretch the symmetric objects to given depth
                aM.scale(1.0,1.0,rSize.DirectionZ/rSize.DirectionX);
                aM.translate(rPosition.PositionX, rPosition.PositionY, rPosition.PositionZ);
                drawing::HomogenMatrix aHM = B3DHomMatrixToHomogenMatrix(aM);
                xProp->setPropertyValue( C2U( UNO_NAME_3D_TRANSFORM_MATRIX )
                    , uno::makeAny(aHM) );
            }

            //Segments
            xProp->setPropertyValue( C2U( UNO_NAME_3D_HORZ_SEGS )
                , uno::makeAny(CHART_3DOBJECT_SEGMENTCOUNT) );
            xProp->setPropertyValue( C2U( UNO_NAME_3D_VERT_SEGS )
                , uno::makeAny((sal_Int32)nVerticalSegmentCount) );//depends on point count of the used polygon

            //Reduced lines
            xProp->setPropertyValue( C2U( UNO_NAME_3D_REDUCED_LINE_GEOMETRY )
                , uno::makeAny((sal_Bool)sal_True) );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

void appendAndCloseBezierCoords( drawing::PolyPolygonBezierCoords& rReturn, const drawing::PolyPolygonBezierCoords& rAdd, sal_Bool bAppendInverse )
{
    if(!rAdd.Coordinates.getLength())
        return;
    sal_Int32 nAddCount = rAdd.Coordinates[0].getLength();
    if(!nAddCount)
        return;

    sal_Int32 nOldCount = rReturn.Coordinates[0].getLength();

    rReturn.Coordinates[0].realloc(nOldCount+nAddCount+1);
    rReturn.Flags[0].realloc(nOldCount+nAddCount+1);

    for(sal_Int32 nN=0;nN<nAddCount; nN++ )
    {
        sal_Int32 nAdd = bAppendInverse ? (nAddCount-1-nN) : nN;
        rReturn.Coordinates[0][nOldCount+nN] = rAdd.Coordinates[0][nAdd];
        rReturn.Flags[0][nOldCount+nN] = rAdd.Flags[0][nAdd];
    }

    //close
    rReturn.Coordinates[0][nOldCount+nAddCount] = rReturn.Coordinates[0][0];
    rReturn.Flags[0][nOldCount+nAddCount] = rReturn.Flags[0][0];
}

//------------------------------------------------------------------------------------------------------------

drawing::PolyPolygonBezierCoords getCircularArcBezierCoords(
        double fStartAngleRadian, double fWidthAngleRadian, double fUnitRadius
        , const ::basegfx::B2DHomMatrix& rTransformationFromUnitCircle
        , const double fAngleSubdivisionRadian )
{
    //at least one polygon is created using two normal and two control points
    //if the angle is larger it is separated into multiple sub angles

    drawing::PolyPolygonBezierCoords aReturn = drawing::PolyPolygonBezierCoords();
    sal_Int32 nSegmentCount = static_cast< sal_Int32 >( fWidthAngleRadian/fAngleSubdivisionRadian );
    if( fWidthAngleRadian > fAngleSubdivisionRadian*nSegmentCount )
        nSegmentCount++;

    double fFirstSegmentAngle = fAngleSubdivisionRadian;
    double fLastSegmentAngle = fAngleSubdivisionRadian;
    if(nSegmentCount==1)
    {
        fFirstSegmentAngle = fWidthAngleRadian;
        fLastSegmentAngle = 0.0;
    }
    else
    {
        double fFirstAngleOnSubDevision = (static_cast<sal_Int32>(fStartAngleRadian/fAngleSubdivisionRadian)+1)*fAngleSubdivisionRadian;
        if( !::rtl::math::approxEqual( fStartAngleRadian, fFirstAngleOnSubDevision ) )
            fFirstSegmentAngle = fFirstAngleOnSubDevision-fStartAngleRadian;

        if(nSegmentCount>1)
        {
            fLastSegmentAngle = fWidthAngleRadian-fFirstSegmentAngle-fAngleSubdivisionRadian*(nSegmentCount-2);
            if( fLastSegmentAngle<0 )
                nSegmentCount--;
            if( fLastSegmentAngle>fAngleSubdivisionRadian )
            {
                fLastSegmentAngle-=fAngleSubdivisionRadian;
                nSegmentCount++;
            }
        }
    }

    sal_Int32 nPointCount     = 1 + 3*nSegmentCount; //first point of next segment equals last point of former segment

    aReturn.Coordinates = drawing::PointSequenceSequence(1);
    aReturn.Flags       = drawing::FlagSequenceSequence(1);

    drawing::PointSequence aPoints(nPointCount);
    drawing::FlagSequence  aFlags(nPointCount);

    //

    //!! applying matrix to vector does ignore translation, so it is important to use a B2DPoint here instead of B2DVector
    ::basegfx::B2DPoint P0,P1,P2,P3;
    ::basegfx::B2DPoint POrigin = rTransformationFromUnitCircle * ::basegfx::B2DPoint(0.0, 0.0);

    sal_Int32 nPoint=0;
    double fCurrentRotateAngle = fStartAngleRadian;
    for(sal_Int32 nSegment=0; nSegment<nSegmentCount; nSegment++)
    {
        double fCurrentSegmentAngle = fAngleSubdivisionRadian;
        if(nSegment==0)//first segment gets only a smaller peace until the next subdevision
            fCurrentSegmentAngle = fFirstSegmentAngle;
        else if(nSegment==(nSegmentCount-1)) //the last segment gets the rest angle that does not fit into equal pieces
            fCurrentSegmentAngle = fLastSegmentAngle;

        //first create untransformed points for a unit circle arc:
        const double fCos = cos(fCurrentSegmentAngle/2.0);
        const double fSin = sin(fCurrentSegmentAngle/2.0);
        P0.setX(fCos);
        P3.setX(fCos);
        P0.setY(-fSin);
        P3.setY(-P0.getY());

        P1.setX((4.0-fCos)/3.0);
        P2.setX(P1.getX());
        P1.setY((1.0-fCos)*(fCos-3.0)/(3.0*fSin));
        P2.setY(-P1.getY());
        //transform thus startangle equals NULL
        ::basegfx::B2DHomMatrix aStart;
        aStart.rotate(fCurrentSegmentAngle/2.0 + fCurrentRotateAngle );
        fCurrentRotateAngle+=fCurrentSegmentAngle;

        aStart.scale( fUnitRadius, fUnitRadius );

        //apply given transformation to get final points
        P0 = rTransformationFromUnitCircle*(aStart*P0);
        P1 = rTransformationFromUnitCircle*(aStart*P1);
        P2 = rTransformationFromUnitCircle*(aStart*P2);
        P3 = rTransformationFromUnitCircle*(aStart*P3);

        aPoints[nPoint].X = static_cast< sal_Int32 >( P0.getX());
        aPoints[nPoint].Y = static_cast< sal_Int32 >( P0.getY());
        aFlags [nPoint++] = drawing::PolygonFlags_NORMAL;

        aPoints[nPoint].X = static_cast< sal_Int32 >( P1.getX());
        aPoints[nPoint].Y = static_cast< sal_Int32 >( P1.getY());
        aFlags[nPoint++] = drawing::PolygonFlags_CONTROL;

        aPoints[nPoint].X = static_cast< sal_Int32 >( P2.getX());
        aPoints[nPoint].Y = static_cast< sal_Int32 >( P2.getY());
        aFlags [nPoint++] = drawing::PolygonFlags_CONTROL;

        if(nSegment==(nSegmentCount-1))
        {
            aPoints[nPoint].X = static_cast< sal_Int32 >( P3.getX());
            aPoints[nPoint].Y = static_cast< sal_Int32 >( P3.getY());
            aFlags [nPoint++] = drawing::PolygonFlags_NORMAL;
        }
    }

    aReturn.Coordinates[0] = aPoints;
    aReturn.Flags[0] = aFlags;

    return aReturn;
}

//------------------------------------------------------------------------------------------------------------

drawing::PolyPolygonBezierCoords getRingBezierCoords(
            double fUnitCircleInnerRadius
            , double fUnitCircleOuterRadius
            , double fStartAngleRadian, double fWidthAngleRadian
            , ::basegfx::B2DHomMatrix aTransformationFromUnitCircle
            , const double fAngleSubdivisionRadian )
{
    drawing::PolyPolygonBezierCoords aReturn = drawing::PolyPolygonBezierCoords();

    aReturn.Coordinates = drawing::PointSequenceSequence(1);
    aReturn.Flags       = drawing::FlagSequenceSequence(1);

    drawing::PolyPolygonBezierCoords aOuterArc = getCircularArcBezierCoords(
        fStartAngleRadian, fWidthAngleRadian, fUnitCircleOuterRadius, aTransformationFromUnitCircle, fAngleSubdivisionRadian );
    aReturn.Coordinates[0] = aOuterArc.Coordinates[0];
    aReturn.Flags[0] = aOuterArc.Flags[0];

    drawing::PolyPolygonBezierCoords aInnerArc = getCircularArcBezierCoords(
        fStartAngleRadian, fWidthAngleRadian, fUnitCircleInnerRadius, aTransformationFromUnitCircle, fAngleSubdivisionRadian );
    appendAndCloseBezierCoords( aReturn, aInnerArc, sal_True );

    //fill rMarkHandlePoints
    /*
    {
        rMarkHandlePoints.realloc(1);
        rMarkHandlePoints[0].realloc(6);
        sal_Int32 nHandleCount=0;
        sal_Int32 nOuterArcCount = aOuterArc.Coordinates[0].getLength();
        if(nOuterArcCount>0)
            rMarkHandlePoints[0][nHandleCount++]=aOuterArc.Coordinates[0][0];
        if(nOuterArcCount>1)
            rMarkHandlePoints[0][nHandleCount++]=aOuterArc.Coordinates[0][nOuterArcCount-1];
        sal_Int32 nInnerArcCount = aInnerArc.Coordinates[0].getLength();
        if(nInnerArcCount>0)
            rMarkHandlePoints[0][nHandleCount++]=aInnerArc.Coordinates[0][0];
        if(nInnerArcCount>1)
            rMarkHandlePoints[0][nHandleCount++]=aInnerArc.Coordinates[0][nInnerArcCount-1];
        rMarkHandlePoints[0].realloc(nHandleCount);
    }
    */

    return aReturn;
}

//------------------------------------------------------------------------------------------------------------

uno::Reference< drawing::XShape >
        ShapeFactory::createPieSegment2D(
                    const uno::Reference< drawing::XShapes >& xTarget
                    , double fUnitCircleStartAngleDegree, double fUnitCircleWidthAngleDegree
                    , double fUnitCircleInnerRadius, double fUnitCircleOuterRadius
                    , const drawing::Direction3D& rOffset
                    , const drawing::HomogenMatrix& rUnitCircleToScene )
{
    if( !xTarget.is() )
        return 0;

    while(fUnitCircleWidthAngleDegree>360)
        fUnitCircleWidthAngleDegree -= 360.0;
    while(fUnitCircleWidthAngleDegree<0)
        fUnitCircleWidthAngleDegree += 360.0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
            m_xShapeFactory->createInstance(
                C2U("com.sun.star.drawing.ClosedBezierShape") ), uno::UNO_QUERY );
    xTarget->add(xShape); //need to add the shape before setting of properties

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            ::basegfx::B2DHomMatrix aTransformationFromUnitCircle( IgnoreZ( HomogenMatrixToB3DHomMatrix(rUnitCircleToScene) ) );
            aTransformationFromUnitCircle.translate(rOffset.DirectionX,rOffset.DirectionY);

            const double fAngleSubdivisionRadian = F_PI/10.0;

            drawing::PolyPolygonBezierCoords aCoords = getRingBezierCoords(
                fUnitCircleInnerRadius, fUnitCircleOuterRadius
                , fUnitCircleStartAngleDegree*F_PI/180.0, fUnitCircleWidthAngleDegree*F_PI/180.0
                , aTransformationFromUnitCircle, fAngleSubdivisionRadian );

            xProp->setPropertyValue( C2U( "PolyPolygonBezier" ), uno::makeAny( aCoords ) );

            //add shape for markhandles
            /*
            drawing::PointSequenceSequence aMarkHandlePoints(1); to be filled within getRingBezierCoords
            if( xGroup.is() )
            {
                VLineProperties aHandleLineProperties;
                aHandleLineProperties.LineStyle    = uno::makeAny( drawing::LineStyle_NONE );
                uno::Reference< drawing::XShape > xHandleShape =
                    this->createLine2D( xGroup, aMarkHandlePoints, &aHandleLineProperties );
                this->setShapeName( xHandleShape, C2U("HandlesOnly") );
            }
            */
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }

    return xShape;
}

//------------------------------------------------------------------------------------------------------------

uno::Reference< drawing::XShape >
        ShapeFactory::createPieSegment(
                    const uno::Reference< drawing::XShapes >& xTarget
                    , double fUnitCircleStartAngleDegree, double fUnitCircleWidthAngleDegree
                    , double fUnitCircleInnerRadius, double fUnitCircleOuterRadius
                    , const drawing::Direction3D& rOffset
                    , const drawing::HomogenMatrix& rUnitCircleToScene
                    , double fDepth )
{
    if( !xTarget.is() )
        return 0;

    while(fUnitCircleWidthAngleDegree>360)
        fUnitCircleWidthAngleDegree -= 360.0;
    while(fUnitCircleWidthAngleDegree<0)
        fUnitCircleWidthAngleDegree += 360.0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.Shape3DExtrudeObject") ), uno::UNO_QUERY );
    xTarget->add(xShape); //need to add the shape before setting of properties

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            ::basegfx::B2DHomMatrix aTransformationFromUnitCircle( IgnoreZ( HomogenMatrixToB3DHomMatrix(rUnitCircleToScene) ) );
            aTransformationFromUnitCircle.translate(rOffset.DirectionX,rOffset.DirectionY);

            const double fAngleSubdivisionRadian = F_PI/32.0;

            drawing::PolyPolygonBezierCoords aCoords = getRingBezierCoords(
                fUnitCircleInnerRadius, fUnitCircleOuterRadius
                , fUnitCircleStartAngleDegree*F_PI/180.0, fUnitCircleWidthAngleDegree*F_PI/180.0
                , aTransformationFromUnitCircle, fAngleSubdivisionRadian );

            //depth
            xProp->setPropertyValue( C2U( UNO_NAME_3D_EXTRUDE_DEPTH )
                , uno::makeAny((sal_Int32)fDepth) );

            //PercentDiagonal
            sal_Int16 nPercentDiagonal = 0;
            xProp->setPropertyValue( C2U( UNO_NAME_3D_PERCENT_DIAGONAL )
                , uno::makeAny( nPercentDiagonal ) );

            //Polygon
            drawing::PolyPolygonShape3D aPoly( BezierToPoly(aCoords) );
            ShapeFactory::closePolygon( aPoly );
            xProp->setPropertyValue( C2U( UNO_NAME_3D_POLYPOLYGON3D )
                , uno::makeAny( aPoly ) );

            //DoubleSided
            xProp->setPropertyValue( C2U( UNO_NAME_3D_DOUBLE_SIDED )
                , uno::makeAny( (sal_Bool)true) );

            //Reduced lines
            xProp->setPropertyValue( C2U( UNO_NAME_3D_REDUCED_LINE_GEOMETRY )
                , uno::makeAny((sal_Bool)sal_True) );

            //TextureProjectionMode
            xProp->setPropertyValue( C2U( UNO_NAME_3D_TEXTURE_PROJ_Y )
                , uno::makeAny( drawing::TextureProjectionMode_OBJECTSPECIFIC ) );

            //TextureProjectionMode
            xProp->setPropertyValue( C2U( UNO_NAME_3D_TEXTURE_PROJ_X )
                , uno::makeAny( drawing::TextureProjectionMode_PARALLEL ) );
            xProp->setPropertyValue( C2U( UNO_NAME_3D_TEXTURE_PROJ_Y )
                , uno::makeAny( drawing::TextureProjectionMode_OBJECTSPECIFIC ) );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

uno::Reference< drawing::XShape >
        ShapeFactory::createStripe( const uno::Reference< drawing::XShapes >& xTarget
                    , const Stripe& rStripe
                    , const uno::Reference< beans::XPropertySet >& xSourceProp
                    , const tPropertyNameMap& rPropertyNameMap
                    , sal_Bool bDoubleSided
                    , bool bRotatedTexture
                    , bool bFlatNormals )
{
    if( !xTarget.is() )
        return 0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
            m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.Shape3DPolygonObject" ) ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            //Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_3D_POLYPOLYGON3D )
                , rStripe.getPolyPolygonShape3D() );

            //TexturePolygon
            xProp->setPropertyValue( C2U( UNO_NAME_3D_TEXTUREPOLYGON3D )
                , rStripe.getTexturePolygon( bRotatedTexture ) );

            //Normals Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_3D_NORMALSPOLYGON3D )
                , rStripe.getNormalsPolygon() );
            //NormalsKind
            if(bFlatNormals)
                xProp->setPropertyValue( C2U( UNO_NAME_3D_NORMALS_KIND )
                    , uno::makeAny( drawing::NormalsKind_FLAT ) );

            //LineOnly
            xProp->setPropertyValue( C2U( UNO_NAME_3D_LINEONLY )
                , uno::makeAny( (sal_Bool)false) );

            //DoubleSided
            xProp->setPropertyValue( C2U( UNO_NAME_3D_DOUBLE_SIDED )
                , uno::makeAny(bDoubleSided) );

            if( xSourceProp.is())
                PropertyMapper::setMappedProperties( xProp, xSourceProp, rPropertyNameMap );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createArea3D( const uno::Reference< drawing::XShapes >& xTarget
                    , const drawing::PolyPolygonShape3D& rPolyPolygon
                    , double fDepth )
{
    if( !xTarget.is() )
        return 0;

    if( !rPolyPolygon.SequenceX.getLength())
        return 0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.Shape3DExtrudeObject") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            //depth
            xProp->setPropertyValue( C2U( UNO_NAME_3D_EXTRUDE_DEPTH )
                , uno::makeAny((sal_Int32)fDepth) );

            //PercentDiagonal
            sal_Int16 nPercentDiagonal = 0;
            xProp->setPropertyValue( C2U( UNO_NAME_3D_PERCENT_DIAGONAL )
                , uno::makeAny( nPercentDiagonal ) );

            //Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_3D_POLYPOLYGON3D )
                , uno::makeAny( rPolyPolygon ) );

            //DoubleSided
            xProp->setPropertyValue( C2U( UNO_NAME_3D_DOUBLE_SIDED )
                , uno::makeAny( (sal_Bool)true) );

            //the z component of the polygon is now ignored by the drawing layer,
            //so we nned to translate the object via transformation matrix

            //Matrix for position
            if( rPolyPolygon.SequenceZ.getLength()&& rPolyPolygon.SequenceZ[0].getLength() )
            {
                ::basegfx::B3DHomMatrix aM;
                aM.translate( 0
                            , 0
                            , rPolyPolygon.SequenceZ[0][0] );
                drawing::HomogenMatrix aHM = B3DHomMatrixToHomogenMatrix(aM);
                xProp->setPropertyValue( C2U( UNO_NAME_3D_TRANSFORM_MATRIX )
                    , uno::makeAny(aHM) );
            }
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createArea2D( const uno::Reference< drawing::XShapes >& xTarget
                    , const drawing::PolyPolygonShape3D& rPolyPolygon )
{
    if( !xTarget.is() )
        return 0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.PolyPolygonShape") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            //UNO_NAME_POLYGON "Polygon" drawing::PointSequence*
            drawing::PointSequenceSequence aPoints( PolyToPointSequence(rPolyPolygon) );

            //Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_POLYPOLYGON )
                , uno::makeAny( aPoints ) );

            //ZOrder
            //an area should always be behind other shapes
            xProp->setPropertyValue( C2U( UNO_NAME_MISC_OBJ_ZORDER )
                , uno::makeAny( sal_Int32(0) ) );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

enum SymbolType { SYMBOL_SQUARE=0
                 , SYMBOL_DIAMOND
                 , SYMBOL_ARROW_DOWN
                 , SYMBOL_ARROW_UP
                 , SYMBOL_ARROW_RIGHT
                 , SYMBOL_ARROW_LEFT
                 , SYMBOL_BOWTIE
                 , SYMBOL_SANDGLASS
                 , SYMBOL_COUNT
                  };

sal_Int32 ShapeFactory::getSymbolCount()
{
    return SYMBOL_COUNT;
}

drawing::PolyPolygonShape3D createPolyPolygon_Symbol( const drawing::Position3D& rPos
                                 , const drawing::Direction3D& rSize
                                 , sal_Int32 nStandardSymbol )
{
    if(nStandardSymbol<0)
        nStandardSymbol*=-1;
    nStandardSymbol = nStandardSymbol%ShapeFactory::getSymbolCount();
    SymbolType eSymbolType=static_cast<SymbolType>(nStandardSymbol);

    const double& fX = rPos.PositionX;
    const double& fY = rPos.PositionY;

    const double fWidthH  = rSize.DirectionX/2.0; //fWidthH stands for Half Width
    const double fHeightH = rSize.DirectionY/2.0; //fHeightH stands for Half Height

    sal_Int32 nPointCount = 4; //all arrow symbols only need 4 points
    switch( eSymbolType )
    {
        case SYMBOL_SQUARE:
        case SYMBOL_DIAMOND:
        case SYMBOL_BOWTIE:
        case SYMBOL_SANDGLASS:
            nPointCount = 5;
            break;
        default:
            break;
    }

    //--------------------------------------
    drawing::PolyPolygonShape3D aPP;

    aPP.SequenceX.realloc(1);
    aPP.SequenceY.realloc(1);
    aPP.SequenceZ.realloc(1);

    drawing::DoubleSequence* pOuterSequenceX = aPP.SequenceX.getArray();
    drawing::DoubleSequence* pOuterSequenceY = aPP.SequenceY.getArray();
    drawing::DoubleSequence* pOuterSequenceZ = aPP.SequenceZ.getArray();

    pOuterSequenceX->realloc(nPointCount);
    pOuterSequenceY->realloc(nPointCount);
    pOuterSequenceZ->realloc(nPointCount);

    double* pInnerSequenceX = pOuterSequenceX->getArray();
    double* pInnerSequenceY = pOuterSequenceY->getArray();
    double* pInnerSequenceZ = pOuterSequenceZ->getArray();

    for(sal_Int32 nN = nPointCount; nN--;)
        *pInnerSequenceZ++ = 0.0;

    switch(eSymbolType)
    {
        case SYMBOL_ARROW_UP:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;
            break;
        }
        case SYMBOL_ARROW_DOWN:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;
            break;
        }
        case SYMBOL_ARROW_RIGHT:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;
            break;
        }
        case SYMBOL_ARROW_LEFT:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY;
            break;
        }
        case SYMBOL_BOWTIE:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;
            break;
        }
        case SYMBOL_SANDGLASS:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;


            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;
            break;
        }
        case SYMBOL_DIAMOND:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY;

            *pInnerSequenceX++ = fX;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY;

            *pInnerSequenceX++ = fX;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY;
            break;
        }
        default: //case SYMBOL_SQUARE:
        {
            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY+fHeightH;

            *pInnerSequenceX++ = fX+fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;

            *pInnerSequenceX++ = fX-fWidthH;
            *pInnerSequenceY++ = fY-fHeightH;
            break;
        }
    }
    //return uno::Any( &aPP, ::getCppuType((const drawing::PolyPolygonShape3D*)0) );
    return aPP;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createSymbol2D(
                      const uno::Reference< drawing::XShapes >& xTarget
                    , const drawing::Position3D& rPosition
                    , const drawing::Direction3D& rSize
                    , sal_Int32 nStandardSymbol
                    , sal_Int32 nBorderColor
                    , sal_Int32 nFillColor )
{
    if( !xTarget.is() )
        return 0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.PolyPolygonShape") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            drawing::PointSequenceSequence aPoints( PolyToPointSequence(
                createPolyPolygon_Symbol( rPosition, rSize, nStandardSymbol ) ));

            //Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_POLYPOLYGON )
                , uno::makeAny( aPoints ) );

            //LineColor
            xProp->setPropertyValue( C2U( UNO_NAME_LINECOLOR )
                , uno::makeAny( nBorderColor ) );

            //FillColor
            xProp->setPropertyValue( C2U( UNO_NAME_FILLCOLOR )
                , uno::makeAny( nFillColor ) );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createGraphic2D(
                      const uno::Reference< drawing::XShapes >& xTarget
                    , const drawing::Position3D& rPosition
                    , const drawing::Direction3D& rSize
                    , const uno::Reference< graphic::XGraphic >& xGraphic )
{
    if( !xTarget.is() || !xGraphic.is() )
        return 0;

    // @todo: change this to a rectangle shape with a fill bitmap for
    // performance reasons (ask AW, said CL)

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.GraphicObjectShape") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    try
    {
        // assume position is upper left corner. Transform to center.
        drawing::Position3D aCenterPosition(
            rPosition.PositionX - (rSize.DirectionX / 2.0),
            rPosition.PositionY - (rSize.DirectionY / 2.0),
            rPosition.PositionZ );
        xShape->setPosition( Position3DToAWTPoint( aCenterPosition ));
        xShape->setSize( Direction3DToAWTSize( rSize ));
    }
    catch( const uno::Exception & e )
    {
        ASSERT_EXCEPTION( e );
    }
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            xProp->setPropertyValue( C2U("Graphic"), uno::makeAny( xGraphic ));
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Reference< drawing::XShapes >
        ShapeFactory::createGroup2D( const uno::Reference< drawing::XShapes >& xTarget
        , ::rtl::OUString aName )
{
    if( !xTarget.is() )
        return 0;
    try
    {
        //create and add to target
        uno::Reference< drawing::XShape > xShape(
                    m_xShapeFactory->createInstance( C2U(
                    "com.sun.star.drawing.GroupShape" ) ), uno::UNO_QUERY );
        xTarget->add(xShape);

        //set name
        if(aName.getLength())
            setShapeName( xShape , aName );

        {//workaround
            //need this null size as otherwise empty group shapes where painted with a gray border
            xShape->setSize(awt::Size(0,0));
        }

        //return
        uno::Reference< drawing::XShapes > xShapes =
            uno::Reference<drawing::XShapes>( xShape, uno::UNO_QUERY );
        return xShapes;
    }
    catch( uno::Exception& e )
    {
        ASSERT_EXCEPTION( e );
    }
    return 0;
}

uno::Reference< drawing::XShapes >
        ShapeFactory::createGroup3D( const uno::Reference< drawing::XShapes >& xTarget
        , ::rtl::OUString aName )
{
    if( !xTarget.is() )
        return 0;
    try
    {
        //create shape
        uno::Reference< drawing::XShape > xShape(
                m_xShapeFactory->createInstance( C2U(
                "com.sun.star.drawing.Shape3DSceneObject" ) ), uno::UNO_QUERY );

        xTarget->add(xShape);

        //it is necessary to set the transform matrix to initialize the scene properly (bug #106316#)
        //otherwise all objects which are placed into this Group will not be visible
        //the following should be unnecessary after a the bug is fixed
        {
            //set properties
            uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
            DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
            if( xProp.is())
            {
                try
                {
                    ::basegfx::B3DHomMatrix aM;
                    xProp->setPropertyValue( C2U( UNO_NAME_3D_TRANSFORM_MATRIX )
                        , uno::makeAny(B3DHomMatrixToHomogenMatrix(aM)) );
                }
                catch( uno::Exception& e )
                {
                    ASSERT_EXCEPTION( e );
                }
            }
        }

        //set name
        if(aName.getLength())
            setShapeName( xShape , aName );

        //return
        uno::Reference< drawing::XShapes > xShapes =
                uno::Reference<drawing::XShapes>( xShape, uno::UNO_QUERY );
        return xShapes;
    }
    catch( uno::Exception& e )
    {
        ASSERT_EXCEPTION( e );
    }
    return 0;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createCircle2D( const uno::Reference< drawing::XShapes >& xTarget
                    , const drawing::Position3D& rPosition
                    , const drawing::Direction3D& rSize )
{
    if( !xTarget.is() )
        return 0;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.EllipseShape") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    try
    {
        drawing::Position3D aCenterPosition(
            rPosition.PositionX - (rSize.DirectionX / 2.0),
            rPosition.PositionY - (rSize.DirectionY / 2.0),
            rPosition.PositionZ );
        xShape->setPosition( Position3DToAWTPoint( aCenterPosition ));
        xShape->setSize( Direction3DToAWTSize( rSize ));
    }
    catch( const uno::Exception & e )
    {
        ASSERT_EXCEPTION( e );
    }

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            drawing::CircleKind eKind = drawing::CircleKind_FULL;
            xProp->setPropertyValue( C2U( UNO_NAME_CIRCKIND )
                , uno::makeAny( eKind ) );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createLine3D( const uno::Reference< drawing::XShapes >& xTarget
                    , const drawing::PolyPolygonShape3D& rPoints
                    , const VLineProperties& rLineProperties )
{
    if( !xTarget.is() )
        return 0;

    if(!rPoints.SequenceX.getLength())
        return NULL;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.Shape3DPolygonObject") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            //Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_3D_POLYPOLYGON3D )
                , uno::makeAny( rPoints ) );

            //LineOnly
            xProp->setPropertyValue( C2U( UNO_NAME_3D_LINEONLY )
                , uno::makeAny( (sal_Bool)true ) );

            //Transparency
            if(rLineProperties.Transparence.hasValue())
                xProp->setPropertyValue( C2U( UNO_NAME_LINETRANSPARENCE )
                    , rLineProperties.Transparence );

            //LineStyle
            if(rLineProperties.LineStyle.hasValue())
                xProp->setPropertyValue( C2U( UNO_NAME_LINESTYLE )
                    , rLineProperties.LineStyle );

            //LineWidth
            if(rLineProperties.Width.hasValue())
                xProp->setPropertyValue( C2U( UNO_NAME_LINEWIDTH )
                    , rLineProperties.Width );

            //LineColor
            if(rLineProperties.Color.hasValue())
                xProp->setPropertyValue( C2U( UNO_NAME_LINECOLOR )
                    , rLineProperties.Color );
                    //, uno::makeAny( sal_Int32( Color(COL_RED).GetColor()) ) );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createLine2D( const uno::Reference< drawing::XShapes >& xTarget
                    , const drawing::PointSequenceSequence& rPoints
                    , const VLineProperties* pLineProperties )
{
    if( !xTarget.is() )
        return 0;

    if(!rPoints.getLength())
        return NULL;

    //create shape
    uno::Reference< drawing::XShape > xShape(
        m_xShapeFactory->createInstance( C2U(
            //"com.sun.star.drawing.LineShape") ), uno::UNO_QUERY );
            "com.sun.star.drawing.PolyLineShape") ), uno::UNO_QUERY );
            //"com.sun.star.drawing.PolyLinePathShape") ), uno::UNO_QUERY );
            //"com.sun.star.drawing.PolyPolygonPathShape") ), uno::UNO_QUERY );
            //"com.sun.star.drawing.PolyPolygonShape") ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set properties
    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xProp.is(), "created shape offers no XPropertySet");
    if( xProp.is())
    {
        try
        {
            //Polygon
            xProp->setPropertyValue( C2U( UNO_NAME_POLYPOLYGON )
                , uno::makeAny( rPoints ) );

            if(pLineProperties)
            {
                //Transparency
                if(pLineProperties->Transparence.hasValue())
                    xProp->setPropertyValue( C2U( UNO_NAME_LINETRANSPARENCE )
                        , pLineProperties->Transparence );

                //LineStyle
                if(pLineProperties->LineStyle.hasValue())
                    xProp->setPropertyValue( C2U( UNO_NAME_LINESTYLE )
                        , pLineProperties->LineStyle );

                //LineWidth
                if(pLineProperties->Width.hasValue())
                    xProp->setPropertyValue( C2U( UNO_NAME_LINEWIDTH )
                        , pLineProperties->Width );

                //LineColor
                if(pLineProperties->Color.hasValue())
                    xProp->setPropertyValue( C2U( UNO_NAME_LINECOLOR )
                        , pLineProperties->Color );

                //LineDashName
                if(pLineProperties->DashName.hasValue())
                    xProp->setPropertyValue( C2U( "LineDashName" )
                        , pLineProperties->DashName );
            }
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

uno::Any ShapeFactory::makeTransformation( const awt::Point& rScreenPosition2D, double fRotationAnglePi )
{
    ::basegfx::B2DHomMatrix aM;
    //As autogrow is active the rectangle is automatically expanded to that side
    //to which the text is not adjusted.
    // aM.scale( 1, 1 ); Oops? A scale with this parameters is neutral, line commented out
    aM.rotate( fRotationAnglePi );
    aM.translate( rScreenPosition2D.X, rScreenPosition2D.Y );
    uno::Any aATransformation = uno::makeAny( B2DHomMatrixToHomogenMatrix3(aM) );
    return aATransformation;
}

void ShapeFactory::makeShapeInvisible( const uno::Reference< drawing::XShape >& xShape )
{
    uno::Reference< beans::XPropertySet > xShapeProp( xShape, uno::UNO_QUERY );
    DBG_ASSERT(xShapeProp.is(), "created shape offers no XPropertySet");
    if( xShapeProp.is())
    {
        try
        {
            xShapeProp->setPropertyValue( C2U("LineStyle"), uno::makeAny( drawing::LineStyle_NONE ));
            xShapeProp->setPropertyValue( C2U("FillStyle"), uno::makeAny( drawing::FillStyle_NONE ));
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
}

uno::Reference< drawing::XShape > ShapeFactory::createInvisibleRectangle(
            const uno::Reference< drawing::XShapes >& xTarget
            , const awt::Size& rSize )
{
    try
    {
        if(!xTarget.is())
            return 0;

        uno::Reference< drawing::XShape > xShape( m_xShapeFactory->createInstance(
                C2U( "com.sun.star.drawing.RectangleShape" )), uno::UNO_QUERY );
        if( xTarget.is() && xShape.is())
        {
            xTarget->add( xShape );
            ShapeFactory::makeShapeInvisible( xShape );
            xShape->setSize( rSize );
        }
        return xShape;
    }
    catch( uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
    return 0;
}

uno::Reference< drawing::XShape >
        ShapeFactory::createText( const uno::Reference< drawing::XShapes >& xTarget
                    , const ::rtl::OUString& rText
                    , const tNameSequence& rPropNames
                    , const tAnySequence& rPropValues
                    , const uno::Any& rATransformation )
{
    if( !xTarget.is() )
        return 0;

    if(!rText.getLength())
        return 0;

    //create shape and add to page
    uno::Reference< drawing::XShape > xShape(
            m_xShapeFactory->createInstance( C2U(
            "com.sun.star.drawing.TextShape" ) ), uno::UNO_QUERY );
    xTarget->add(xShape);

    //set text
    uno::Reference< text::XTextRange > xTextRange( xShape, uno::UNO_QUERY );
    if( xTextRange.is() )
        xTextRange->setString( rText );

    uno::Reference< beans::XPropertySet > xProp( xShape, uno::UNO_QUERY );
    if( xProp.is() )
    {
        //set properties
        PropertyMapper::setMultiProperties( rPropNames, rPropValues, xProp );

        //set position matrix
        //the matrix needs to be set at the end behind autogrow and such position influencing properties
        try
        {
            xProp->setPropertyValue( C2U( "Transformation" ), rATransformation );
        }
        catch( uno::Exception& e )
        {
            ASSERT_EXCEPTION( e );
        }
    }
    return xShape;
}

//static
rtl::OUString ShapeFactory::getStackedString( const rtl::OUString& rString, bool bStacked )
{
    sal_Int32 nLen = rString.getLength();
    if(!bStacked || !nLen)
        return rString;

    rtl::OUStringBuffer aStackStr;
    rtl::OUStringBuffer aSource(rString);

    //add a newline after each letter
    //as we do not no letters here add a newline after each char
    for( sal_Int32 nPosSrc=0; nPosSrc < nLen; nPosSrc++ )
    {
        if( nPosSrc )
            aStackStr.append( sal_Unicode('\r') );
        aStackStr.append( aSource.charAt( nPosSrc ) );
    }
    return aStackStr.makeStringAndClear();
}

//static
bool ShapeFactory::hasPolygonAnyLines( drawing::PolyPolygonShape3D& rPoly)
{
    // #i67757# check all contained polygons, if at least one polygon contains 2 or more points, return true
    for( sal_Int32 nIdx = 0, nCount = rPoly.SequenceX.getLength(); nIdx < nCount; ++nIdx )
        if( rPoly.SequenceX[ nIdx ].getLength() > 1 )
            return true;
    return false;
}

//static
bool ShapeFactory::isPolygonEmptyOrSinglePoint( drawing::PolyPolygonShape3D& rPoly)
{
    // true, if empty polypolygon or one polygon with one point
    return (rPoly.SequenceX.getLength() == 0) ||
        ((rPoly.SequenceX.getLength() == 1) && (rPoly.SequenceX[0].getLength() <= 1));
}

//static
void ShapeFactory::closePolygon( drawing::PolyPolygonShape3D& rPoly)
{
    DBG_ASSERT( rPoly.SequenceX.getLength() <= 1, "ShapeFactory::closePolygon - single polygon expected" );
    //add a last point == first point
    if(isPolygonEmptyOrSinglePoint(rPoly))
        return;
    drawing::Position3D aFirst(rPoly.SequenceX[0][0],rPoly.SequenceY[0][0],rPoly.SequenceZ[0][0]);
    AddPointToPoly( rPoly, aFirst );
}

//static
awt::Size ShapeFactory::calculateNewSizeRespectingAspectRatio(
         const awt::Size& rTargetSize
         , const awt::Size& rSourceSizeWithCorrectAspectRatio )
{
    awt::Size aNewSize;

    double fFactorWidth = double(rTargetSize.Width)/double(rSourceSizeWithCorrectAspectRatio.Width);
    double fFactorHeight = double(rTargetSize.Height)/double(rSourceSizeWithCorrectAspectRatio.Height);
    double fFactor = std::min(fFactorWidth,fFactorHeight);
    aNewSize.Width=static_cast<sal_Int32>(fFactor*rSourceSizeWithCorrectAspectRatio.Width);
    aNewSize.Height=static_cast<sal_Int32>(fFactor*rSourceSizeWithCorrectAspectRatio.Height);

    return aNewSize;
}

//static
awt::Point ShapeFactory::calculateTopLeftPositionToCenterObject(
           const awt::Point& rTargetAreaPosition
         , const awt::Size& rTargetAreaSize
         , const awt::Size& rObjectSize )
{
    awt::Point aNewPosition(rTargetAreaPosition);
    aNewPosition.X += static_cast<sal_Int32>(double(rTargetAreaSize.Width-rObjectSize.Width)/2.0);
    aNewPosition.Y += static_cast<sal_Int32>(double(rTargetAreaSize.Height-rObjectSize.Height)/2.0);
    return aNewPosition;
}

//static
::basegfx::B2IRectangle ShapeFactory::getRectangleOfShape(
        const uno::Reference< drawing::XShape >& xShape )
{
    ::basegfx::B2IRectangle aRet;

    if( xShape.is() )
    {
        awt::Point aPos = xShape->getPosition();
        awt::Size aSize = xShape->getSize();
        aRet = BaseGFXHelper::makeRectangle(aPos,aSize);
    }
    return aRet;

}

//static
awt::Size ShapeFactory::getSizeAfterRotation(
         const uno::Reference< drawing::XShape >& xShape, double fRotationAngleDegree )
{
    awt::Size aRet(0,0);
    if(xShape.is())
    {
        const awt::Size aSize( xShape->getSize() );

        if( ::rtl::math::approxEqual( fRotationAngleDegree, 0.0 ) )
            aRet = aSize;
        else
        {
            while(fRotationAngleDegree>=360.0)
                fRotationAngleDegree-=360.0;
            while(fRotationAngleDegree<0.0)
                fRotationAngleDegree+=360.0;
            if(fRotationAngleDegree>270.0)
                fRotationAngleDegree=360.0-fRotationAngleDegree;
            else if(fRotationAngleDegree>180.0)
                fRotationAngleDegree=fRotationAngleDegree-180.0;
            else if(fRotationAngleDegree>90.0)
                fRotationAngleDegree=180.0-fRotationAngleDegree;

            const double fAnglePi = fRotationAngleDegree*F_PI/180.0;

            aRet.Height = static_cast<sal_Int32>(
                aSize.Width*rtl::math::sin( fAnglePi )
                + aSize.Height*rtl::math::cos( fAnglePi ));
            aRet.Width = static_cast<sal_Int32>(
                aSize.Width*rtl::math::cos( fAnglePi )
                + aSize.Height*rtl::math::sin( fAnglePi ));
        }
    }
    return aRet;
}

void ShapeFactory::removeSubShapes( const uno::Reference< drawing::XShapes >& xShapes )
{
    if( xShapes.is() )
    {
        sal_Int32 nSubCount = xShapes->getCount();
        uno::Reference< drawing::XShape > xShape;
        for( sal_Int32 nS = nSubCount; nS--; )
        {
            if( xShapes->getByIndex( nS ) >>= xShape )
                xShapes->remove( xShape );
        }
    }
}

//.............................................................................
} //namespace chart
//.............................................................................
