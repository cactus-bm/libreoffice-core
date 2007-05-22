/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: VCartesianAxis.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 19:10:36 $
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
#include "precompiled_chart2.hxx"

#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#include "VCartesianAxis.hxx"
#include "PlottingPositionHelper.hxx"
#include "ShapeFactory.hxx"
#include "CommonConverters.hxx"
#include "macros.hxx"
#include "ViewDefines.hxx"
#include "PropertyMapper.hxx"
#include "chartview/NumberFormatterWrapper.hxx"
#include "LabelPositionHelper.hxx"
#include "TrueGuard.hxx"
#include "BaseGFXHelper.hxx"

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXT_HPP_
#include <com/sun/star/text/XText.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_WRITINGMODE2_HPP_
#include <com/sun/star/text/WritingMode2.hpp>
#endif

#ifndef _SVX_UNOPRNMS_HXX
#include <svx/unoprnms.hxx>
#endif

#include <algorithm>
#include <memory>

//.............................................................................
namespace chart
{
//.............................................................................
using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;
using namespace ::rtl::math;
using ::com::sun::star::uno::Reference;

void lcl_correctRotation_Left( double& rfXCorrection, double& rfYCorrection
                           , double fAnglePositiveDegree, const awt::Size& aSize )
{
    //correct label positions left of an axis with right centered alignment
    double fAnglePi = fAnglePositiveDegree*F_PI/180.0;
    if( fAnglePositiveDegree==0.0 )
    {
    }
    else if( fAnglePositiveDegree<= 90.0 )
    {
        rfXCorrection  = -aSize.Height*rtl::math::sin( fAnglePi )/2.0;
    }
    else if( fAnglePositiveDegree<= 180.0 )
    {
        double beta = fAnglePi-F_PI/2.0;
        rfXCorrection  = -aSize.Width *rtl::math::sin( beta )
            -aSize.Height *rtl::math::cos( beta )/2.0;
        rfYCorrection = -aSize.Width *rtl::math::cos( beta );
    }
    else if( fAnglePositiveDegree<= 270.0 )
    {
        double beta = fAnglePi - F_PI;
        rfXCorrection  = -aSize.Width *rtl::math::cos( beta )
            -aSize.Height*rtl::math::sin( beta )/2.0;
        rfYCorrection = aSize.Width *rtl::math::sin( beta );
    }
    else
    {
        double beta = 2*F_PI - fAnglePi;
        rfXCorrection = -aSize.Height*rtl::math::sin( beta )/2.0;

    }
}

void lcl_correctRotation_Right( double& rfXCorrection, double& rfYCorrection
                           , double fAnglePositiveDegree, const awt::Size& aSize )
{
    //correct label positions right of an axis with left centered alignment
    double fAnglePi = fAnglePositiveDegree*F_PI/180.0;
    if( fAnglePositiveDegree== 0.0 )
    {
    }
    else if( fAnglePositiveDegree<= 90.0 )
    {
        rfXCorrection = aSize.Height*rtl::math::sin( fAnglePi )/2.0;
    }
    else if( fAnglePositiveDegree<= 180.0 )
    {
        double beta = F_PI - fAnglePi;
        rfXCorrection  = aSize.Width *rtl::math::cos( beta )
            + aSize.Height*rtl::math::sin( beta )/2.0;
        rfYCorrection  = aSize.Width *rtl::math::sin( beta );
    }
    else if( fAnglePositiveDegree<= 270.0 )
    {
        double beta = 3*F_PI/2.0 - fAnglePi;
        rfXCorrection  = aSize.Width *rtl::math::sin( beta )
                    +aSize.Height*rtl::math::cos( beta )/2.0;
        rfYCorrection  = -aSize.Width *rtl::math::cos( beta );
    }
    else
    {
        rfXCorrection  = aSize.Height*rtl::math::sin( 2*F_PI - fAnglePi )/2.0;
    }
}

void lcl_correctRotation_Top( double& rfXCorrection, double& rfYCorrection
                           , double fAnglePositiveDegree, const awt::Size& aSize )
{
    //correct label positions on top of an axis with bottom centered alignment
    double fAnglePi = fAnglePositiveDegree*F_PI/180.0;
    if( fAnglePositiveDegree== 0.0 )
    {
    }
    else if( fAnglePositiveDegree<= 90.0 )
    {
        rfXCorrection = aSize.Width*rtl::math::cos( fAnglePi )/2.0
            +aSize.Height*rtl::math::sin( fAnglePi )/2.0;
        rfYCorrection = -aSize.Width*rtl::math::sin( fAnglePi )/2.0;
    }
    else if( fAnglePositiveDegree<= 180.0 )
    {
        double beta = fAnglePi - F_PI/2.0;
        rfYCorrection = -aSize.Width*rtl::math::cos( beta )/2.0
            - aSize.Height*rtl::math::sin( beta );
        rfXCorrection = - aSize.Width*rtl::math::sin( beta )/2.0
            + aSize.Height*rtl::math::cos( beta )/2.0;
    }
    else if( fAnglePositiveDegree<= 270.0 )
    {
        double beta = fAnglePi - F_PI;
        rfXCorrection = +aSize.Width *rtl::math::cos( beta )/2.0
            -aSize.Height *rtl::math::sin( beta )/2.0; ;
        rfYCorrection = -aSize.Width *rtl::math::sin( beta )/2.0
            -aSize.Height *rtl::math::cos( beta );
    }
    else
    {
        rfXCorrection = -aSize.Width*rtl::math::cos( fAnglePi )/2.0
            +aSize.Height*rtl::math::sin( fAnglePi )/2.0;
        rfYCorrection = aSize.Width*rtl::math::sin( fAnglePi )/2.0;
    }
}

void lcl_correctRotation_Bottom( double& rfXCorrection, double& rfYCorrection
                           , double fAnglePositiveDegree, const awt::Size& aSize )
{
    //correct label positions below of an axis with top centered alignment
    double fAnglePi = fAnglePositiveDegree*F_PI/180.0;
    if( fAnglePositiveDegree==0.0 )
    {
    }
    else if( fAnglePositiveDegree<= 90.0 )
    {
        rfXCorrection  = -aSize.Width *rtl::math::cos( fAnglePi )/2.0
                         -aSize.Height*rtl::math::sin( fAnglePi )/2.0;
        rfYCorrection = aSize.Width*rtl::math::sin( fAnglePi )/2.0;
    }
    else if( fAnglePositiveDegree<= 180.0 )
    {
        double beta = fAnglePi-F_PI/2.0;
        rfYCorrection = aSize.Width *rtl::math::cos( beta )/2.0
            +aSize.Height*rtl::math::sin( beta );
        rfXCorrection  = aSize.Width *rtl::math::sin( beta )/2.0
                    -aSize.Height*rtl::math::cos( beta )/2.0;
    }
    else if( fAnglePositiveDegree<= 270.0 )
    {
        double beta = 3*F_PI/2.0 - fAnglePi;
        rfXCorrection  = -aSize.Width *rtl::math::sin( beta )/2.0
                         +aSize.Height*rtl::math::cos( beta )/2.0;
        rfYCorrection = aSize.Height*rtl::math::sin( beta )
                        +aSize.Width*rtl::math::cos( beta )/2.0;
    }
    else
    {
        double beta = 2*F_PI - fAnglePi;
        rfXCorrection  = aSize.Height*rtl::math::sin( beta )/2.0
                        +aSize.Width*rtl::math::cos( beta )/2.0;
        rfYCorrection = aSize.Width*rtl::math::sin( beta )/2.0;
    }
}

void lcl_correctPositionForRotation(
                  const Reference< drawing::XShape >& xShape2DText
                , LabelAlignment eLabelAlignment
                , const double fRotationAngle )
{
    if( !xShape2DText.is() )
        return;

    awt::Point aOldPos = xShape2DText->getPosition();
    awt::Size  aSize   = xShape2DText->getSize();

    double fYCorrection = 0.0;
    double fXCorrection  = 0.0;

    double fAnglePositiveDegree = fRotationAngle;
    while(fAnglePositiveDegree<0.0)
        fAnglePositiveDegree+=360.0;

    switch(eLabelAlignment)
    {
        case LABEL_ALIGN_LEFT:
        case LABEL_ALIGN_LEFT_TOP:
        case LABEL_ALIGN_LEFT_BOTTOM:
            lcl_correctRotation_Left( fXCorrection, fYCorrection, fAnglePositiveDegree, aSize );
            break;
        case LABEL_ALIGN_RIGHT:
        case LABEL_ALIGN_RIGHT_TOP:
        case LABEL_ALIGN_RIGHT_BOTTOM:
            lcl_correctRotation_Right( fXCorrection, fYCorrection, fAnglePositiveDegree, aSize );
            break;
        case LABEL_ALIGN_TOP:
            lcl_correctRotation_Top( fXCorrection, fYCorrection, fAnglePositiveDegree, aSize );
            break;
        case LABEL_ALIGN_BOTTOM:
            lcl_correctRotation_Bottom( fXCorrection, fYCorrection, fAnglePositiveDegree, aSize );
        default: //LABEL_ALIGN_CENTER
            break;
    }

    xShape2DText->setPosition( awt::Point(
          static_cast<sal_Int32>(aOldPos.X + fXCorrection  )
        , static_cast<sal_Int32>(aOldPos.Y + fYCorrection ) ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

VCartesianAxis::VCartesianAxis( const AxisProperties& rAxisProperties
            , const Reference< util::XNumberFormatsSupplier >& xNumberFormatsSupplier
            , sal_Int32 nDimensionIndex, sal_Int32 nDimensionCount
            , PlottingPositionHelper* pPosHelper )//takes ownership
            : VAxisBase( nDimensionIndex, nDimensionCount, rAxisProperties, xNumberFormatsSupplier )
{
    if( pPosHelper )
        m_pPosHelper = pPosHelper;
    else
        m_pPosHelper = new PlottingPositionHelper();
}

VCartesianAxis::~VCartesianAxis()
{
    delete m_pPosHelper;
    m_pPosHelper = NULL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Reference< drawing::XShape > createSingleLabel(
            const Reference< lang::XMultiServiceFactory>& xShapeFactory
          , const Reference< drawing::XShapes >& xTarget
          , const awt::Point& rAnchorScreenPosition2D
          , const rtl::OUString& rLabel
          , const AxisLabelProperties& rAxisLabelProperties
          , const AxisProperties& rAxisProperties
          , const tNameSequence& rPropNames
          , const tAnySequence& rPropValues
          )
{
    if(!rLabel.getLength())
        return 0;

    double fRotationAnglePi = rAxisLabelProperties.fRotationAngleDegree*F_PI/180.0;
    uno::Any aATransformation = ShapeFactory::makeTransformation( rAnchorScreenPosition2D, fRotationAnglePi );
    rtl::OUString aLabel = ShapeFactory::getStackedString( rLabel, rAxisLabelProperties.bStackCharacters );

    Reference< drawing::XShape > xShape2DText = ShapeFactory(xShapeFactory)
                    .createText( xTarget, aLabel, rPropNames, rPropValues, aATransformation );

    //correctPositionForRotation
    lcl_correctPositionForRotation( xShape2DText
        , rAxisProperties.m_aLabelAlignment
        , rAxisLabelProperties.fRotationAngleDegree );

    return xShape2DText;
}

bool lcl_doesShapeOverlapWithTickmark( const Reference< drawing::XShape >& xShape
                       , double fRotationAngleDegree
                       , const basegfx::B2DVector& rTickScreenPosition
                       , TickmarkHelper_2D* pTickmarkHelper )
{
    if(!xShape.is())
        return false;

    ::basegfx::B2IRectangle aShapeRect = BaseGFXHelper::makeRectangle(xShape->getPosition(),ShapeFactory::getSizeAfterRotation( xShape, fRotationAngleDegree ));

    if( pTickmarkHelper->isVerticalAxis() )
    {
        return ( (rTickScreenPosition.getY() >= aShapeRect.getMinY())
            && (rTickScreenPosition.getY() <= aShapeRect.getMaxY()) );
    }
    if( pTickmarkHelper->isHorizontalAxis() )
    {
        return ( (rTickScreenPosition.getX() >= aShapeRect.getMinX())
            && (rTickScreenPosition.getX() <= aShapeRect.getMaxX()) );
    }

    basegfx::B2IVector aPosition(
        static_cast<sal_Int32>( rTickScreenPosition.getX() )
        , static_cast<sal_Int32>( rTickScreenPosition.getY() ) );
    return aShapeRect.isInside(aPosition);
}

bool doesOverlap( const Reference< drawing::XShape >& xShape1
                , const Reference< drawing::XShape >& xShape2
                , double fRotationAngleDegree )
{
    if( !xShape1.is() || !xShape2.is() )
        return false;

    ::basegfx::B2IRectangle aRect1( BaseGFXHelper::makeRectangle(xShape1->getPosition(),ShapeFactory::getSizeAfterRotation( xShape1, fRotationAngleDegree )));
    ::basegfx::B2IRectangle aRect2( BaseGFXHelper::makeRectangle(xShape2->getPosition(),ShapeFactory::getSizeAfterRotation( xShape2, fRotationAngleDegree )));
    return aRect1.overlaps(aRect2);
}

void removeShapesAtWrongRythm( TickIter& rIter
                              , sal_Int32 nCorrectRhythm
                              , sal_Int32 nMaxTickToCheck
                              , const Reference< drawing::XShapes >& xTarget )
{
    sal_Int32 nTick = 0;
    for( TickInfo* pTickInfo = rIter.firstInfo()
        ; pTickInfo && nTick <= nMaxTickToCheck
        ; pTickInfo = rIter.nextInfo(), nTick++ )
    {
        //remove labels which does not fit into the rythm
        if( nTick%nCorrectRhythm != 0)
        {
            if(pTickInfo->xTextShape.is())
            {
                xTarget->remove(pTickInfo->xTextShape);
                pTickInfo->xTextShape = NULL;
            }
        }
    }
}

class LabelIterator : private TickIter
{
    //this Iterator iterates over existing text labels

    //if the labels are staggered and bInnerLine is true
    //we iterate only through the labels which are lying more inside the diagram

    //if the labels are staggered and bInnerLine is false
    //we iterate only through the labels which are lying more outside the diagram

    //if the labels are not staggered
    //we iterate through all labels

public:
    LabelIterator( ::std::vector< ::std::vector< TickInfo > >& rTickInfos
            , const ::com::sun::star::chart2::ExplicitIncrementData& rIncrement
            , const AxisLabelStaggering eAxisLabelStaggering
            , bool bInnerLine
            , sal_Int32 nMinDepth=0, sal_Int32 nMaxDepth=-1 );

    virtual TickInfo*   firstInfo();
    virtual TickInfo*   nextInfo();

private: //methods
    LabelIterator();

private: //member
    const AxisLabelStaggering   m_eAxisLabelStaggering;
    bool        m_bInnerLine;
};

LabelIterator::LabelIterator( ::std::vector< ::std::vector< TickInfo > >& rTickInfos
            , const ::com::sun::star::chart2::ExplicitIncrementData& rIncrement
            , const AxisLabelStaggering eAxisLabelStaggering
            , bool bInnerLine
            , sal_Int32 nMinDepth, sal_Int32 nMaxDepth )
            : TickIter( rTickInfos, rIncrement, nMinDepth, nMaxDepth )
            , m_eAxisLabelStaggering(eAxisLabelStaggering)
            , m_bInnerLine(bInnerLine)
{
}

TickInfo* LabelIterator::firstInfo()
{
    TickInfo* pTickInfo = TickIter::firstInfo();
    while( pTickInfo && !pTickInfo->xTextShape.is() )
        pTickInfo = TickIter::nextInfo();
    if(!pTickInfo)
        return NULL;
    if( (STAGGER_EVEN==m_eAxisLabelStaggering && m_bInnerLine)
        ||
        (STAGGER_ODD==m_eAxisLabelStaggering && !m_bInnerLine)
        )
    {
        //skip first label
        do
            pTickInfo = TickIter::nextInfo();
        while( pTickInfo && !pTickInfo->xTextShape.is() );
    }
    if(!pTickInfo)
        return NULL;
    return pTickInfo;
}

TickInfo* LabelIterator::nextInfo()
{
    TickInfo* pTickInfo = NULL;
    //get next label
    do
        pTickInfo = TickIter::nextInfo();
    while( pTickInfo && !pTickInfo->xTextShape.is() );

    if(  STAGGER_EVEN==m_eAxisLabelStaggering
      || STAGGER_ODD==m_eAxisLabelStaggering )
    {
        //skip one label
        do
            pTickInfo = TickIter::nextInfo();
        while( pTickInfo && !pTickInfo->xTextShape.is() );
    }
    return pTickInfo;
}

B2DVector lcl_getStaggerDistance( LabelIterator& rIter, const B2DVector& rDistanceTickToText )
{
    //calculates the height or width of the first line of labels
    //thus the second line of labels needs to be shifted for that distance

    B2DVector aRet(0,0);

    sal_Int32 nDistanceTickToText = static_cast<sal_Int32>( rDistanceTickToText.getLength() );
    if( nDistanceTickToText==0.0)
        return aRet;

    B2DVector aStaggerDirection(rDistanceTickToText);
    aStaggerDirection.normalize();

    sal_Int32 nDistance=0;
    Reference< drawing::XShape >  xShape2DText(NULL);
    for( TickInfo* pTickInfo = rIter.firstInfo()
        ; pTickInfo
        ; pTickInfo = rIter.nextInfo() )
    {
        xShape2DText = pTickInfo->xTextShape;
        DBG_ASSERT(xShape2DText.is(),"LabelIterator does not work correctly");

        awt::Size aSize  = xShape2DText->getSize();
        if(abs(aStaggerDirection.getX())>abs(aStaggerDirection.getY()))
            nDistance = ::std::max(nDistance,aSize.Width);
        else
            nDistance = ::std::max(nDistance,aSize.Height);
    }

    aRet = aStaggerDirection*nDistance;

    //add extra distance for vertical distance
    if(abs(aStaggerDirection.getX())>abs(aStaggerDirection.getY()))
        aRet += rDistanceTickToText;

    return aRet;
}

void lcl_correctPositionForStaggering( LabelIterator& rIter, const B2DVector& rStaggerDistance )
{
    if(rStaggerDistance.getLength()==0.0)
        return;
    Reference< drawing::XShape >  xShape2DText(NULL);
    for( TickInfo* pTickInfo = rIter.firstInfo()
        ; pTickInfo
        ; pTickInfo = rIter.nextInfo() )
    {
        xShape2DText = pTickInfo->xTextShape;
        DBG_ASSERT(xShape2DText.is(),"LabelIterator does not work correctly");

        awt::Point aPos  = xShape2DText->getPosition();
        aPos.X += static_cast<sal_Int32>(rStaggerDistance.getX());
        aPos.Y += static_cast<sal_Int32>(rStaggerDistance.getY());
        xShape2DText->setPosition( aPos );
    }
}

class MaxLabelTickIter : public TickIter
{
    //iterate over first two and last two labels and the longest label
public:
    MaxLabelTickIter( ::std::vector< ::std::vector< TickInfo > >& rTickInfos
            , const ::com::sun::star::chart2::ExplicitIncrementData& rIncrement
            , sal_Int32 nLongestLabelIndex );
    virtual ~MaxLabelTickIter();

    virtual TickInfo*   nextInfo();

private:
    sal_Int32 m_nLongestLabelIndex;
};

MaxLabelTickIter::MaxLabelTickIter( ::std::vector< ::std::vector< TickInfo > >& rTickInfos
            , const ::com::sun::star::chart2::ExplicitIncrementData& rIncrement
            , sal_Int32 nLongestLabelIndex )
            : TickIter( rTickInfos, rIncrement, 0//nMinDepth
                                                , 0//nMaxDepth
                                                )
            , m_nLongestLabelIndex( nLongestLabelIndex )
{
    sal_Int32 nMaxIndex = getMaxIndex();

    //ensure correct value:
    if( m_nLongestLabelIndex<0 || m_nLongestLabelIndex>nMaxIndex)
        m_nLongestLabelIndex = 0;

    //last label is checked anyhow
    if( m_nLongestLabelIndex==nMaxIndex )
        m_nLongestLabelIndex = 0;
    //label before last is checked anyhow
    if( m_nLongestLabelIndex+1==nMaxIndex )
        m_nLongestLabelIndex = 0;
}
MaxLabelTickIter::~MaxLabelTickIter()
{
}

TickInfo* MaxLabelTickIter::nextInfo()
{
    sal_Int32 nCurrentPos = getCurrentIndex();
    sal_Int32 nMaxIndex = getMaxIndex();
    if( nCurrentPos>0 )
    {
        if( m_nLongestLabelIndex>1 && nCurrentPos<m_nLongestLabelIndex  )
            gotoIndex( m_nLongestLabelIndex-1 ) ;
        else
        {
            if( nMaxIndex>3 && nCurrentPos<nMaxIndex-1 )
                gotoIndex( nMaxIndex-2 );
            else if( nMaxIndex>2 && nCurrentPos<nMaxIndex )
                gotoIndex( nMaxIndex-1 );
        }
    }

    return TickIter::nextInfo();
}

bool VCartesianAxis::isBreakOfLabelsAllowed( const AxisLabelProperties& rAxisLabelProperties
                                                     , TickmarkHelper_2D* pTickmarkHelper )
{
    if( m_aTextLabels.getLength() > 100 )
        return false;
    if( !rAxisLabelProperties.bLineBreakAllowed )
        return false;
    if( rAxisLabelProperties.bStackCharacters )
        return false;
    //no break for value axis
    if( !m_bUseTextLabels )
        return false;
    if( !::rtl::math::approxEqual( rAxisLabelProperties.fRotationAngleDegree, 0.0 ) )
        return false;
    //break only for horizontal axis
    if( pTickmarkHelper )
        return pTickmarkHelper->isHorizontalAxis();
    return false;
}

bool VCartesianAxis::isAutoStaggeringOfLabelsAllowed( const AxisLabelProperties& rAxisLabelProperties
                                                     , TickmarkHelper_2D* pTickmarkHelper )
{
    if( rAxisLabelProperties.eStaggering != STAGGER_AUTO )
        return false;
    if( rAxisLabelProperties.bOverlapAllowed )
        return false;
    if( rAxisLabelProperties.bLineBreakAllowed ) //auto line break or auto staggering, doing both automatisms they may conflict...
        return false;
    if( !::rtl::math::approxEqual( rAxisLabelProperties.fRotationAngleDegree, 0.0 ) )
        return false;
    //automatic staggering only for horizontal axis with horizontal text
    //or vertical axis with vertical text
    if( pTickmarkHelper )
    {
        if( pTickmarkHelper->isHorizontalAxis() )
            return !rAxisLabelProperties.bStackCharacters;
        if( pTickmarkHelper->isVerticalAxis() )
            return rAxisLabelProperties.bStackCharacters;
    }
    return false;
}

bool VCartesianAxis::createTextShapes(
                       const Reference< drawing::XShapes >& xTarget
                     , TickIter& rTickIter
                     , AxisLabelProperties& rAxisLabelProperties
                     , TickmarkHelper_2D* pTickmarkHelper )
{
    //returns true if the text shapes have been created succesfully
    //otherwise false - in this case the AxisLabelProperties have changed
    //and contain new instructions for the next try for text shape creation

    uno::Sequence< rtl::OUString >* pCategories = m_bUseTextLabels? &m_aTextLabels : 0;

    Reference< XScaling > xInverseScaling( NULL );
    if( m_aScale.Scaling.is() )
        xInverseScaling = m_aScale.Scaling->getInverseScaling();

    FixedNumberFormatter aFixedNumberFormatter(
                m_xNumberFormatsSupplier, rAxisLabelProperties.nNumberFormatKey );

    B2DVector aTextToTickDistance( pTickmarkHelper->getDistanceTickToText( m_aAxisProperties ) );

    TickInfo* pPreviousVisibleTickInfo = NULL;
    TickInfo* pPREPreviousVisibleTickInfo = NULL;
    TickInfo* pLastVisibleNeighbourTickInfo = NULL;
    bool bIsStaggered = rAxisLabelProperties.getIsStaggered();
    sal_Int32 nLimitedSpace = -1;
    if( isBreakOfLabelsAllowed( rAxisLabelProperties, pTickmarkHelper ) )
    {
        nLimitedSpace = TickmarkHelper_2D::getTickScreenDistance( rTickIter );
        if( bIsStaggered )
            nLimitedSpace *= 2;

        if( nLimitedSpace > 0 )
        { //reduce space for a small amount to have a visible distance between the labels:
            sal_Int32 nReduce = (nLimitedSpace*5)/100;
            if(!nReduce)
                nReduce = 1;
            nLimitedSpace -= nReduce;
        }
        //maybe @todo in future:
        //if the labeled tickmarks are not equidistant [this is not considered to be a relevant case so far]
        //the limited space maybe needs to be calculated for each tickmark seperatly
        //or the staggering could be ignored in that case
    }

    //------------------------------------------------
    //prepare properties for multipropertyset-interface of shape
    tNameSequence aPropNames;
    tAnySequence aPropValues;

    Reference< beans::XPropertySet > xProps( m_aAxisProperties.m_xAxisModel, uno::UNO_QUERY );
    PropertyMapper::getTextLabelMultiPropertyLists( xProps, aPropNames, aPropValues, false
        , nLimitedSpace, fabs(aTextToTickDistance.getX()) > fabs(aTextToTickDistance.getY()) );
    LabelPositionHelper::doDynamicFontResize( aPropValues, aPropNames, xProps
        , m_aAxisLabelProperties.m_aFontReferenceSize );
    LabelPositionHelper::changeTextAdjustment( aPropValues, aPropNames, m_aAxisProperties.m_aLabelAlignment );

    uno::Any* pColorAny = PropertyMapper::getValuePointer(aPropValues,aPropNames,C2U("CharColor"));
    sal_Int32 nColor = Color( COL_AUTO ).GetColor();
    if(pColorAny)
        *pColorAny >>= nColor;
    //------------------------------------------------

    sal_Int32 nTick = 0;
    for( TickInfo* pTickInfo = rTickIter.firstInfo()
        ; pTickInfo
        ; pTickInfo = rTickIter.nextInfo(), nTick++ )
    {
        pLastVisibleNeighbourTickInfo = bIsStaggered ?
                    pPREPreviousVisibleTickInfo : pPreviousVisibleTickInfo;

        //don't create labels which does not fit into the rythm
        if( nTick%rAxisLabelProperties.nRhythm != 0)
            continue;

        //don't create labels for invisible ticks
        if( !pTickInfo->bPaintIt )
            continue;

        //if NO OVERLAP -> don't create labels where the tick overlaps
        //with the text of the last neighbour tickmark
        if( pLastVisibleNeighbourTickInfo && !rAxisLabelProperties.bOverlapAllowed )
        {
            if( lcl_doesShapeOverlapWithTickmark( pLastVisibleNeighbourTickInfo->xTextShape
                       , rAxisLabelProperties.fRotationAngleDegree
                       , pTickInfo->aTickScreenPosition, pTickmarkHelper ) )
            {
                bool bOverlapAlsoAfterSwitchingOnAutoStaggering = true;
                if( !bIsStaggered && isAutoStaggeringOfLabelsAllowed( rAxisLabelProperties, pTickmarkHelper ) )
                {
                    bIsStaggered = true;
                    rAxisLabelProperties.eStaggering = STAGGER_EVEN;
                    pLastVisibleNeighbourTickInfo = pPREPreviousVisibleTickInfo;
                    if( !pLastVisibleNeighbourTickInfo ||
                        !lcl_doesShapeOverlapWithTickmark( pLastVisibleNeighbourTickInfo->xTextShape
                                , rAxisLabelProperties.fRotationAngleDegree
                                , pTickInfo->aTickScreenPosition, pTickmarkHelper ) )
                        bOverlapAlsoAfterSwitchingOnAutoStaggering = false;
                }
                if( bOverlapAlsoAfterSwitchingOnAutoStaggering )
                {
                    if( rAxisLabelProperties.bRhythmIsFix )
                        continue;
                    rAxisLabelProperties.nRhythm++;
                    TickIter aRemoveIter( m_aAllTickInfos, m_aIncrement, 0, 0 );
                    removeShapesAtWrongRythm( aRemoveIter, rAxisLabelProperties.nRhythm, nTick, xTarget );
                    return false;
                }
            }
        }

        pTickInfo->updateUnscaledValue( xInverseScaling );

        bool bHasExtraColor=false;
        sal_Int32 nExtraColor=0;

        rtl::OUString aLabel;
        if(pCategories)
        {
            sal_Int32 nIndex = static_cast< sal_Int32 >(pTickInfo->fUnscaledTickValue) - 1; //first category (index 0) matches with real number 1.0
            if( nIndex>=0 && nIndex<pCategories->getLength() )
                aLabel = (*pCategories)[nIndex];
        }
        else
            aLabel = aFixedNumberFormatter.getFormattedString( pTickInfo->fUnscaledTickValue, nExtraColor, bHasExtraColor );

        if(pColorAny)
            *pColorAny = uno::makeAny(bHasExtraColor?nExtraColor:nColor);

        B2DVector aTickScreenPos2D( pTickInfo->aTickScreenPosition );
        aTickScreenPos2D += aTextToTickDistance;
        awt::Point aAnchorScreenPosition2D(
            static_cast<sal_Int32>(aTickScreenPos2D.getX())
            ,static_cast<sal_Int32>(aTickScreenPos2D.getY()));

        //create single label
        if(!pTickInfo->xTextShape.is())
            pTickInfo->xTextShape = createSingleLabel( m_xShapeFactory, xTarget
                                    , aAnchorScreenPosition2D, aLabel
                                    , rAxisLabelProperties, m_aAxisProperties
                                    , aPropNames, aPropValues );
        if(!pTickInfo->xTextShape.is())
            continue;

        recordMaximumTextSize( pTickInfo->xTextShape, rAxisLabelProperties.fRotationAngleDegree );

        //if NO OVERLAP -> remove overlapping shapes
        if( pLastVisibleNeighbourTickInfo && !rAxisLabelProperties.bOverlapAllowed )
        {
            if( doesOverlap( pLastVisibleNeighbourTickInfo->xTextShape, pTickInfo->xTextShape, m_aAxisLabelProperties.fRotationAngleDegree ) )
            {
                bool bOverlapAlsoAfterSwitchingOnAutoStaggering = true;
                if( !bIsStaggered && isAutoStaggeringOfLabelsAllowed( rAxisLabelProperties, pTickmarkHelper ) )
                {
                    bIsStaggered = true;
                    rAxisLabelProperties.eStaggering = STAGGER_EVEN;
                    pLastVisibleNeighbourTickInfo = pPREPreviousVisibleTickInfo;
                    if( !pLastVisibleNeighbourTickInfo ||
                        !lcl_doesShapeOverlapWithTickmark( pLastVisibleNeighbourTickInfo->xTextShape
                            , rAxisLabelProperties.fRotationAngleDegree
                            , pTickInfo->aTickScreenPosition, pTickmarkHelper ) )
                        bOverlapAlsoAfterSwitchingOnAutoStaggering = false;
                }
                if( bOverlapAlsoAfterSwitchingOnAutoStaggering )
                {
                    if( rAxisLabelProperties.bRhythmIsFix )
                    {
                        xTarget->remove(pTickInfo->xTextShape);
                        pTickInfo->xTextShape = NULL;
                        continue;
                    }
                    rAxisLabelProperties.nRhythm++;
                    TickIter aRemoveIter( m_aAllTickInfos, m_aIncrement, 0, 0 );
                    removeShapesAtWrongRythm( aRemoveIter, rAxisLabelProperties.nRhythm, nTick, xTarget );
                    return false;
                }
            }
        }

        pPREPreviousVisibleTickInfo = pPreviousVisibleTickInfo;
        pPreviousVisibleTickInfo = pTickInfo;
    }
    return true;
}

drawing::PointSequenceSequence lcl_makePointSequence( B2DVector& rStart, B2DVector& rEnd )
{
    drawing::PointSequenceSequence aPoints(1);
    aPoints[0].realloc(2);
    aPoints[0][0].X = static_cast<sal_Int32>(rStart.getX());
    aPoints[0][0].Y = static_cast<sal_Int32>(rStart.getY());
    aPoints[0][1].X = static_cast<sal_Int32>(rEnd.getX());
    aPoints[0][1].Y = static_cast<sal_Int32>(rEnd.getY());
    return aPoints;
}

double VCartesianAxis::getLogicValueWhereMainLineCrossesOtherAxis() const
{
    double fMin = (m_nDimensionIndex==1) ? m_pPosHelper->getLogicMinX() : m_pPosHelper->getLogicMinY();
    double fMax = (m_nDimensionIndex==1) ? m_pPosHelper->getLogicMaxX() : m_pPosHelper->getLogicMaxY();

    double fCrossesOtherAxis;
    if(m_aAxisProperties.m_pfMainLinePositionAtOtherAxis)
        fCrossesOtherAxis = *m_aAxisProperties.m_pfMainLinePositionAtOtherAxis;
    else
    {
        bool bMinimumForLeftAxis = ( (m_nDimensionIndex==1) && m_pPosHelper->isMathematicalOrientationX() )
                            || ( (m_nDimensionIndex!=1) && m_pPosHelper->isMathematicalOrientationY() );
        if(!m_aAxisProperties.m_bIsMainAxis)
            bMinimumForLeftAxis = !bMinimumForLeftAxis;
        fCrossesOtherAxis = bMinimumForLeftAxis ? fMin : fMax;
    }
    return fCrossesOtherAxis;
}

bool VCartesianAxis::getLogicValueWhereExtraLineCrossesOtherAxis( double& fCrossesOtherAxis ) const
{
    if( !m_aAxisProperties.m_pfExrtaLinePositionAtOtherAxis )
        return false;
    double fMin = (m_nDimensionIndex==1) ? m_pPosHelper->getLogicMinX() : m_pPosHelper->getLogicMinY();
    double fMax = (m_nDimensionIndex==1) ? m_pPosHelper->getLogicMaxX() : m_pPosHelper->getLogicMaxY();
    if( *m_aAxisProperties.m_pfExrtaLinePositionAtOtherAxis <= fMin
        || *m_aAxisProperties.m_pfExrtaLinePositionAtOtherAxis >= fMax )
        return false;
    fCrossesOtherAxis = *m_aAxisProperties.m_pfExrtaLinePositionAtOtherAxis;
    return true;
}
void VCartesianAxis::get2DAxisMainLine( B2DVector& rStart, B2DVector& rEnd, double fCrossesOtherAxis ) const
{
    double fXStart = m_pPosHelper->getLogicMinX();//@todo get x value at left border
    double fYStart = m_pPosHelper->getLogicMinY();//@todo get y value at bottom
    double fZStart = m_pPosHelper->getLogicMinZ();//@todo get z value at front

    double fXEnd = fXStart;
    double fYEnd = fYStart;
    double fZEnd = fZStart;

    if(!m_nDimensionIndex)
    {
        //x-axis
        fYStart = fYEnd = fCrossesOtherAxis;
        fXEnd=m_pPosHelper->getLogicMaxX();
    }
    else if(m_nDimensionIndex==1)
    {
        //y-axis
        fXStart = fXEnd = fCrossesOtherAxis;
        fYEnd=m_pPosHelper->getLogicMaxY();
    }
    else
    {
        //z-axis
        fZEnd = m_pPosHelper->getLogicMaxZ();
        if( !m_pPosHelper->isSwapXAndY() )
        {
            fXStart = fXEnd = m_pPosHelper->isMathematicalOrientationX() ? m_pPosHelper->getLogicMaxX() : m_pPosHelper->getLogicMinX();
            fYStart = fYEnd = m_pPosHelper->isMathematicalOrientationY() ? m_pPosHelper->getLogicMinY() : m_pPosHelper->getLogicMaxY();
        }
        else
        {
            fXStart = fXEnd = m_pPosHelper->isMathematicalOrientationX() ? m_pPosHelper->getLogicMinX() : m_pPosHelper->getLogicMaxX();
            fYStart = fYEnd = m_pPosHelper->isMathematicalOrientationY() ? m_pPosHelper->getLogicMaxY() : m_pPosHelper->getLogicMinY();
        }
    }

    drawing::Position3D aSceneStart = m_pPosHelper->transformLogicToScene( fXStart, fYStart, fZStart, true );
    drawing::Position3D aSceneEnd = m_pPosHelper->transformLogicToScene( fXEnd, fYEnd, fZEnd, true );

    rStart.setX( aSceneStart.PositionX );
    rStart.setY( aSceneStart.PositionY );
    rEnd.setX( aSceneEnd.PositionX );
    rEnd.setY( aSceneEnd.PositionY );

    if(3==m_nDimension)
    {
        tPropertyNameMap aDummyPropertyNameMap;
        //create 3D anchor shape
        Reference< drawing::XShape > xShape3DAnchor_Start = m_pShapeFactory->createCube( m_xLogicTarget
                , aSceneStart,drawing::Direction3D(1,1,1)
                , 0, 0, aDummyPropertyNameMap);
        awt::Point aStart = xShape3DAnchor_Start->getPosition(); //get 2D position from xShape3DAnchor
        rStart.setX( aStart.X );
        rStart.setY( aStart.Y );
        m_xLogicTarget->remove(xShape3DAnchor_Start);

        //create 3D anchor shape
        Reference< drawing::XShape > xShape3DAnchor_End = m_pShapeFactory->createCube( m_xLogicTarget
                , aSceneEnd, drawing::Direction3D(1,1,1)
                , 0, 0, aDummyPropertyNameMap);
        awt::Point aEnd = xShape3DAnchor_End->getPosition(); //get 2D position from xShape3DAnchor
        rEnd.setX( aEnd.X );
        rEnd.setY( aEnd.Y );
        m_xLogicTarget->remove(xShape3DAnchor_End);
    }
}

TickmarkHelper* VCartesianAxis::createTickmarkHelper()
{
    return createTickmarkHelper2D();
}

TickmarkHelper_2D* VCartesianAxis::createTickmarkHelper2D()
{
    B2DVector aStart, aEnd;
    this->get2DAxisMainLine( aStart, aEnd, this->getLogicValueWhereMainLineCrossesOtherAxis() );
    return new TickmarkHelper_2D( m_aScale, m_aIncrement, aStart, aEnd );
}

sal_Int32 VCartesianAxis::estimateMaximumAutoMainIncrementCount()
{
    sal_Int32 nRet = 10;

    if( m_nMaximumTextWidthSoFar==0 && m_nMaximumTextHeightSoFar==0 )
        return nRet;

    B2DVector aStart, aEnd;
    this->get2DAxisMainLine( aStart, aEnd, this->getLogicValueWhereMainLineCrossesOtherAxis() );

    sal_Int32 nMaxHeight = static_cast<sal_Int32>(abs(aEnd.getY()-aStart.getY()));
    sal_Int32 nMaxWidth = static_cast<sal_Int32>(abs(aEnd.getX()-aStart.getX()));

    sal_Int32 nTotalAvailable = nMaxHeight;
    sal_Int32 nSingleNeeded = m_nMaximumTextHeightSoFar;

    //for horizontal axis:
    if( (m_nDimensionIndex == 0 && !m_aAxisProperties.m_bSwapXAndY)
        || (m_nDimensionIndex == 1 && m_aAxisProperties.m_bSwapXAndY) )
    {
        nTotalAvailable = nMaxWidth;
        nSingleNeeded = m_nMaximumTextWidthSoFar;
    }

    if( nSingleNeeded>0 )
        nRet = nTotalAvailable/nSingleNeeded;

    return nRet;
}

void VCartesianAxis::doStaggeringOfLabels( const AxisLabelProperties& rAxisLabelProperties, TickmarkHelper_2D* pTickmarkHelper2D )
{
    if( !pTickmarkHelper2D )
        return;

    if( rAxisLabelProperties.getIsStaggered() )
    {
        LabelIterator aInnerIter( m_aAllTickInfos, m_aIncrement
            , rAxisLabelProperties.eStaggering, true, 0, 0 );
        LabelIterator aOuterIter( m_aAllTickInfos, m_aIncrement
            , rAxisLabelProperties.eStaggering, false, 0, 0 );

        lcl_correctPositionForStaggering( aOuterIter
            , lcl_getStaggerDistance( aInnerIter
                , pTickmarkHelper2D->getDistanceTickToText( m_aAxisProperties ) ) );
    }
}

void SAL_CALL VCartesianAxis::createLabels()
{
    if( !prepareShapeCreation() )
        return;

    //-----------------------------------------
    //create labels
    if( m_aAxisProperties.m_bDisplayLabels )
    {
        std::auto_ptr< TickmarkHelper_2D > apTickmarkHelper2D( this->createTickmarkHelper2D() );
        TickmarkHelper_2D* pTickmarkHelper2D = apTickmarkHelper2D.get();
        if( !pTickmarkHelper2D )
            return;

        //-----------------------------------------
        //get the transformed screen values for all tickmarks in aAllTickInfos
        pTickmarkHelper2D->updateScreenValues( m_aAllTickInfos );
        //-----------------------------------------
        //'hide' tickmarks with identical screen values in aAllTickInfos
        pTickmarkHelper2D->hideIdenticalScreenValues( m_aAllTickInfos );

        TickIter aRemoveIter( m_aAllTickInfos, m_aIncrement, 0, 0 );
        removeTextShapesFromTicks( aRemoveIter, m_xTextTarget );

        //create tick mark text shapes
        TickIter aTickIter( m_aAllTickInfos, m_aIncrement, 0, 0 );
        while( !createTextShapes( m_xTextTarget, aTickIter
                        , m_aAxisLabelProperties, pTickmarkHelper2D ) )
        {
        };

        doStaggeringOfLabels( m_aAxisLabelProperties, pTickmarkHelper2D );
    }
}

void SAL_CALL VCartesianAxis::createMaximumLabels()
{
    TrueGuard aRecordMaximumTextSize(m_bRecordMaximumTextSize);

    if( !prepareShapeCreation() )
        return;

    //-----------------------------------------
    //create labels
    if( m_aAxisProperties.m_bDisplayLabels )
    {
        std::auto_ptr< TickmarkHelper_2D > apTickmarkHelper2D( this->createTickmarkHelper2D() );
        TickmarkHelper_2D* pTickmarkHelper2D = apTickmarkHelper2D.get();
        if( !pTickmarkHelper2D )
            return;

        //-----------------------------------------
        //get the transformed screen values for all tickmarks in aAllTickInfos
        pTickmarkHelper2D->updateScreenValues( m_aAllTickInfos );

        //create tick mark text shapes
        //@todo: iterate through all tick depth wich should be labeled

        sal_Int32 nLongestLabelIndex = m_bUseTextLabels
            ? this->getIndexOfLongestLabel( m_aTextLabels ) : 0;
        MaxLabelTickIter aTickIter( m_aAllTickInfos, m_aIncrement, nLongestLabelIndex );
        AxisLabelProperties aAxisLabelProperties( m_aAxisLabelProperties );
        if( isAutoStaggeringOfLabelsAllowed( aAxisLabelProperties, pTickmarkHelper2D ) )
            aAxisLabelProperties.eStaggering = STAGGER_EVEN;
        aAxisLabelProperties.bOverlapAllowed = true;
        aAxisLabelProperties.bLineBreakAllowed = false;
        while( !createTextShapes( m_xTextTarget, aTickIter
                        , aAxisLabelProperties, pTickmarkHelper2D ) )
        {
        };

        doStaggeringOfLabels( aAxisLabelProperties, pTickmarkHelper2D );
    }
}

void SAL_CALL VCartesianAxis::updatePositions()
{
    //-----------------------------------------
    //update positions of labels
    if( m_aAxisProperties.m_bDisplayLabels )
    {
        std::auto_ptr< TickmarkHelper_2D > apTickmarkHelper2D( this->createTickmarkHelper2D() );
        TickmarkHelper_2D* pTickmarkHelper2D = apTickmarkHelper2D.get();
        if( !pTickmarkHelper2D )
            return;

        //-----------------------------------------
        //update positions of all existing text shapes
        pTickmarkHelper2D->updateScreenValues( m_aAllTickInfos );

        TickIter aTickIter( m_aAllTickInfos, m_aIncrement, 0, 0 );

        Reference< drawing::XShape > xShape2DText;
        for( TickInfo* pTickInfo = aTickIter.firstInfo()
            ; pTickInfo; pTickInfo = aTickIter.nextInfo() )
        {
            if( !pTickInfo )
                continue;

            xShape2DText = pTickInfo->xTextShape;
            if( xShape2DText.is() )
            {
                B2DVector aTextToTickDistance( pTickmarkHelper2D->getDistanceTickToText( m_aAxisProperties ) );
                B2DVector aTickScreenPos2D( pTickInfo->aTickScreenPosition );
                aTickScreenPos2D += aTextToTickDistance;
                awt::Point aAnchorScreenPosition2D(
                    static_cast<sal_Int32>(aTickScreenPos2D.getX())
                    ,static_cast<sal_Int32>(aTickScreenPos2D.getY()));

                uno::Any aATransformation = ShapeFactory::makeTransformation(
                    aAnchorScreenPosition2D, m_aAxisLabelProperties.fRotationAngleDegree*F_PI/180.0 );

                //set new position
                uno::Reference< beans::XPropertySet > xProp( xShape2DText, uno::UNO_QUERY );
                if( xProp.is() )
                {
                    try
                    {
                        xProp->setPropertyValue( C2U( "Transformation" ), aATransformation );
                    }
                    catch( uno::Exception& e )
                    {
                        ASSERT_EXCEPTION( e );
                    }
                }

                //correctPositionForRotation
                lcl_correctPositionForRotation( xShape2DText
                    , m_aAxisProperties.m_aLabelAlignment
                    , m_aAxisLabelProperties.fRotationAngleDegree );
            }
        }

        doStaggeringOfLabels( m_aAxisLabelProperties, pTickmarkHelper2D );
    }
}

void SAL_CALL VCartesianAxis::createShapes()
{
    if( !prepareShapeCreation() )
        return;

    std::auto_ptr< TickmarkHelper_2D > apTickmarkHelper2D( this->createTickmarkHelper2D() );
    TickmarkHelper_2D* pTickmarkHelper2D = apTickmarkHelper2D.get();
    if( !pTickmarkHelper2D )
        return;

    //-----------------------------------------
    //create tick mark line shapes
    if(2==m_nDimension)
    {
        ::std::vector< ::std::vector< TickInfo > >::iterator aDepthIter             = m_aAllTickInfos.begin();
        const ::std::vector< ::std::vector< TickInfo > >::const_iterator aDepthEnd  = m_aAllTickInfos.end();

        if(aDepthIter == aDepthEnd)//no tickmarks at all
            return;

        sal_Int32 nTickmarkPropertiesCount = m_aAxisProperties.m_aTickmarkPropertiesList.size();
        for( sal_Int32 nDepth=0
            ; aDepthIter != aDepthEnd && nDepth < nTickmarkPropertiesCount
            ; aDepthIter++, nDepth++ )
        {
            const TickmarkProperties& rTickmarkProperties = m_aAxisProperties.m_aTickmarkPropertiesList[nDepth];

            sal_Int32 nPointCount = (*aDepthIter).size();
            drawing::PointSequenceSequence aPoints(nPointCount);

            ::std::vector< TickInfo >::const_iterator       aTickIter = (*aDepthIter).begin();
            const ::std::vector< TickInfo >::const_iterator aTickEnd  = (*aDepthIter).end();
            sal_Int32 nN = 0;
            for( ; aTickIter != aTickEnd; aTickIter++ )
            {
                if( !(*aTickIter).bPaintIt )
                    continue;
                apTickmarkHelper2D->addPointSequenceForTickLine( aPoints, nN, (*aTickIter).fScaledTickValue
                    , m_aAxisProperties.m_fInnerDirectionSign, rTickmarkProperties );
                nN++;
            }
            aPoints.realloc(nN);
            m_pShapeFactory->createLine2D( m_xGroupShape_Shapes, aPoints
                                        , &rTickmarkProperties.aLineProperties );
        }
        //-----------------------------------------
        //create axis main lines
        //it serves also as the handle shape for the axis selection
        {
            drawing::PointSequenceSequence aPoints(1);
            apTickmarkHelper2D->createPointSequenceForAxisMainLine( aPoints );
            Reference< drawing::XShape > xShape = m_pShapeFactory->createLine2D(
                    m_xGroupShape_Shapes, aPoints
                    , &m_aAxisProperties.m_aLineProperties );
            //because of this name this line will be used for marking the axis
            m_pShapeFactory->setShapeName( xShape, C2U("MarkHandles") );
        }
        //-----------------------------------------
        //create an additional line at NULL
        double fExtraLineCrossesOtherAxis;
        if( getLogicValueWhereExtraLineCrossesOtherAxis(fExtraLineCrossesOtherAxis) )
        {
            B2DVector aStart, aEnd;
            this->get2DAxisMainLine( aStart, aEnd, fExtraLineCrossesOtherAxis );
            drawing::PointSequenceSequence aPoints( lcl_makePointSequence(aStart,aEnd) );
            Reference< drawing::XShape > xShape = m_pShapeFactory->createLine2D(
                    m_xGroupShape_Shapes, aPoints, &m_aAxisProperties.m_aLineProperties );
        }
    }

    //createLabels();
}

//.............................................................................
} //namespace chart
//.............................................................................
