/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: VLegend.cxx,v $
 *
 *  $Revision: 1.37 $
 *
 *  last change: $Author: ihi $ $Date: 2006-11-14 15:36:42 $
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
#include "VLegend.hxx"
#include "macros.hxx"
#include "PropertyMapper.hxx"
#include "CommonConverters.hxx"
#include "VLegendSymbolFactory.hxx"
#include "chartview/ObjectIdentifier.hxx"
#include "LayoutHelper.hxx"
#include "ShapeFactory.hxx"
#include "RelativeSizeHelper.hxx"
#include "RegressionCurveHelper.hxx"

#ifndef _COM_SUN_STAR_TEXT_XTEXTRANGE_HPP_
#include <com/sun/star/text/XTextRange.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XDATASERIESTREEPARENT_HPP_
#include <com/sun/star/chart2/XDataSeriesTreeParent.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_TEXTHORIZONTALADJUST_HPP_
#include <com/sun/star/drawing/TextHorizontalAdjust.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XDATASOURCE_HPP_
#include <com/sun/star/chart2/XDataSource.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XCHARTTYPEGROUP_HPP_
#include <com/sun/star/chart2/XChartTypeGroup.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XIDENTIFIABLE_HPP_
#include <com/sun/star/chart2/XIdentifiable.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_LINEJOINT_HPP_
#include <com/sun/star/drawing/LineJoint.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_LEGENDEXPANSION_HPP_
#include <com/sun/star/chart2/LegendExpansion.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_LEGENDPOSITION_HPP_
#include <com/sun/star/chart2/LegendPosition.hpp>
#endif
#ifndef _COM_SUN_STAR_LAYOUT_ANCHORPOINT_HPP_
#include <com/sun/star/layout/AnchorPoint.hpp>
#endif
#ifndef _COM_SUN_STAR_LAYOUT_RELATIVEPOSITION_HPP_
#include <com/sun/star/layout/RelativePosition.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XREGRESSIONCURVECONTAINER_HPP_
#include <com/sun/star/chart2/XRegressionCurveContainer.hpp>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#include <vector>

using namespace ::com::sun::star;

//.............................................................................
namespace chart
{
//.............................................................................

namespace
{

struct LegendEntry
{
    uno::Reference< drawing::XShapes >     xGroupShapes;
    uno::Reference< drawing::XShape >      xTextShape;
    uno::Reference< beans::XPropertySet >  xSymbolProperties;
    uno::Reference< chart2::XChartType >   xChartType;

    LegendEntry( uno::Reference< drawing::XShapes > _GroupShapeForSingleEntry,
                 uno::Reference< drawing::XShape > _xTextShape,
                 uno::Reference< beans::XPropertySet > _xSymbolProperties,
                 uno::Reference< chart2::XChartType > _xChartType ) :
            xGroupShapes( _GroupShapeForSingleEntry ),
            xTextShape( _xTextShape ),
            xSymbolProperties( _xSymbolProperties ),
            xChartType( _xChartType )
    {}
};
typedef ::std::vector< LegendEntry > tEntryGroup;

typedef ::std::pair< ::chart::tNameSequence, ::chart::tAnySequence > tPropertyValues;


::rtl::OUString lcl_DataToString( const uno::Sequence< uno::Any > & rSeq )
{
    sal_Int32 nMax = rSeq.getLength() - 1;
    ::rtl::OUString aVal;
    ::rtl::OUStringBuffer aBuf;
    for( sal_Int32 i = 0; i <= nMax; ++i )
    {
        if( rSeq[i] >>= aVal )
        {
            aBuf.append( aVal );
            if( i < nMax )
                aBuf.append( sal_Unicode( ' ' ));
        }
    }
    return aBuf.makeStringAndClear();
}

::rtl::OUString lcl_getLabelForSeries( const uno::Reference< chart2::XDataSource > & xSource )
{
    ::rtl::OUString aResult;

    if( xSource.is())
    {
        uno::Sequence< uno::Reference< chart2::XDataSequence > > aSeq( xSource->getDataSequences());
        for( sal_Int32 i=0; i < aSeq.getLength(); ++i )
        {
            uno::Reference< beans::XPropertySet > xProp( aSeq[i], uno::UNO_QUERY );
            ::rtl::OUString aRole;
            if( xProp.is() &&
                (xProp->getPropertyValue( C2U( "Role" )) >>= aRole) )
            {
                if( aRole.equals( C2U( "label" )))
                {
                    aResult = lcl_DataToString( aSeq[i]->getData() );
                    break;
                }
            }
        }
    }

    return aResult;
}

void lcl_getProperties(
    const uno::Reference< beans::XPropertySet > & xLegendProp,
    tPropertyValues & rOutLineFillProperties,
    tPropertyValues & rOutTextProperties,
    sal_Int32 nMaxLabelWidth,
    const awt::Size & rReferenceSize )
{
    // Get Line- and FillProperties from model legend
    if( xLegendProp.is())
    {
        // set rOutLineFillProperties
        ::chart::tPropertyNameValueMap aLineFillValueMap;
        ::chart::tMakePropertyNameMap aNameMap = ::chart::PropertyMapper::getPropertyNameMapForFillProperties();
        const ::chart::tMakePropertyNameMap& rLinePropMap = ::chart::PropertyMapper::getPropertyNameMapForLineProperties();
        aNameMap.insert( rLinePropMap.begin(), rLinePropMap.end());
        ::chart::PropertyMapper::getValueMap( aLineFillValueMap, aNameMap, xLegendProp );

        aLineFillValueMap[ C2U("LineJoint") ] = uno::makeAny( drawing::LineJoint_ROUND );

        ::chart::PropertyMapper::getMultiPropertyListsFromValueMap(
            rOutLineFillProperties.first, rOutLineFillProperties.second, aLineFillValueMap );

        // set rOutTextProperties
        ::chart::tPropertyNameValueMap aTextValueMap;
        ::chart::tMakePropertyNameMap aCharNameMap = ::chart::PropertyMapper::getPropertyNameMapForCharacterProperties();
        ::chart::PropertyMapper::getValueMap( aTextValueMap, aCharNameMap, xLegendProp );

        drawing::TextHorizontalAdjust eHorizAdjust( drawing::TextHorizontalAdjust_LEFT );
        aTextValueMap[ C2U("TextAutoGrowHeight") ] = uno::makeAny( sal_True );
        aTextValueMap[ C2U("TextAutoGrowWidth") ] = uno::makeAny( sal_True );
        aTextValueMap[ C2U("TextHorizontalAdjust") ] = uno::makeAny( eHorizAdjust );
        aTextValueMap[ C2U("TextMaximumFrameWidth") ] = uno::makeAny( nMaxLabelWidth );

        // recalculate font size
        awt::Size aPropRefSize;
        float fFontHeight;
        if( (xLegendProp->getPropertyValue( C2U( "ReferencePageSize" )) >>= aPropRefSize) &&
            (aPropRefSize.Height > 0) &&
            (aTextValueMap[ C2U("CharHeight") ] >>= fFontHeight) )
        {
            aTextValueMap[ C2U("CharHeight") ] = uno::makeAny(
                static_cast< float >(
                    ::chart::RelativeSizeHelper::calculate( fFontHeight, aPropRefSize, rReferenceSize )));

            if( aTextValueMap[ C2U("CharHeightAsian") ] >>= fFontHeight )
            {
                aTextValueMap[ C2U("CharHeightAsian") ] = uno::makeAny(
                    static_cast< float >(
                        ::chart::RelativeSizeHelper::calculate( fFontHeight, aPropRefSize, rReferenceSize )));
            }
            if( aTextValueMap[ C2U("CharHeightComplex") ] >>= fFontHeight )
            {
                aTextValueMap[ C2U("CharHeightComplex") ] = uno::makeAny(
                    static_cast< float >(
                        ::chart::RelativeSizeHelper::calculate( fFontHeight, aPropRefSize, rReferenceSize )));
            }
        }

        ::chart::PropertyMapper::getMultiPropertyListsFromValueMap(
            rOutTextProperties.first, rOutTextProperties.second, aTextValueMap );
    }
}

/** creates a symbol shape that is returned and added to the given
    xShapeContainer The adding to the container may have to be done here, if you
    use group shapes, because you can only add shapes to a group that itself has
    been added to an XShapes before.
 */
uno::Reference< drawing::XShape >
    lcl_getSymbol(
    const uno::Reference< chart2::XChartType > & xChartType,
    const uno::Reference< beans::XPropertySet > & xSeriesProp,
    const uno::Reference< lang::XMultiServiceFactory > & xFact,
    const uno::Reference< drawing::XShapes > & xShapeContainer )
{
    uno::Reference< drawing::XShape > xResult;

    xResult.set( xFact->createInstance(
                     C2U( "com.sun.star.drawing.GroupShape" )), uno::UNO_QUERY );
    xShapeContainer->add( xResult );
    uno::Reference< drawing::XShapes > xGroup( xResult, uno::UNO_QUERY );
    if( ! xGroup.is())
        return uno::Reference< drawing::XShape >();

    xShapeContainer->add( xResult );

    if( xChartType.is())
    {
        ::rtl::OUString aChartType( xChartType->getChartType());
        VLegendSymbolFactory::tPropertyType ePropType =
            VLegendSymbolFactory::PROP_TYPE_FILLED_SERIES;

        chart2::LegendSymbolStyle eSymbolStyle = chart2::LegendSymbolStyle_BOX;

        // todo: offer UNO components that support the given service-name
        // and are capable of creating data representations as well as
        // legend symbols

        if( aChartType.equals( C2U( "com.sun.star.chart2.BarChart" )) ||
            aChartType.equals( C2U( "com.sun.star.chart2.AreaChart" )))
        {
            eSymbolStyle = chart2::LegendSymbolStyle_BOX;
        }
        else if( aChartType.equals( C2U( "com.sun.star.chart2.LineChart" )) ||
                 aChartType.equals( C2U( "com.sun.star.chart2.ScatterChart" )))
        {
            eSymbolStyle = chart2::LegendSymbolStyle_LINE;
            ePropType = VLegendSymbolFactory::PROP_TYPE_LINE_SERIES;
            try
            {
                // use a box for 3d-line charts
                sal_Int32 nDim;
                uno::Reference< beans::XPropertySet > xCTProp( xChartType, uno::UNO_QUERY_THROW );
                if(( xCTProp->getPropertyValue( C2U( "Dimension" )) >>= nDim ) &&
                   nDim == 3 )
                {
                    eSymbolStyle = chart2::LegendSymbolStyle_BOX;
                    ePropType = VLegendSymbolFactory::PROP_TYPE_FILLED_SERIES;
                }
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
        }
        else if( aChartType.equals( C2U( "com.sun.star.chart2.PieChart" )))
        {
            eSymbolStyle = chart2::LegendSymbolStyle_CIRCLE;
        }
        else if( aChartType.equals( C2U( "com.sun.star.chart2.NetChart" )))
        {
            eSymbolStyle = chart2::LegendSymbolStyle_LINE;
            ePropType = VLegendSymbolFactory::PROP_TYPE_LINE_SERIES;
        }

        ::chart::VLegendSymbolFactory::createSymbol(
            xGroup, eSymbolStyle, xFact, xSeriesProp, ePropType );
    }
    else
    {
        // no chart-type group => assume regression line
        ::chart::VLegendSymbolFactory::createSymbol(
            xGroup, chart2::LegendSymbolStyle_DIAGONAL_LINE, xFact, xSeriesProp,
            VLegendSymbolFactory::PROP_TYPE_LINE );
    }

    return xResult;
}

typedef ::std::pair< uno::Reference< drawing::XShape >, uno::Reference< drawing::XShapes > >
    tShapeWithGroup;

tShapeWithGroup lcl_getLegendEntry(
        const uno::Reference< lang::XMultiServiceFactory > & xShapeFactory,
        const uno::Reference< drawing::XShapes > & xTarget,
        const ::rtl::OUString & rText,
        const tPropertyValues & rProperties,
        awt::Size & rOutMinExtentSoFar,
        awt::Size & rOutMaxExtentSoFar,
        tEntryGroup & rOutEntryContainer,
        const ::rtl::OUString & rCID )
{
    uno::Reference< drawing::XShape > xEntry;
    uno::Reference< drawing::XShapes > xGroupShapes;
    try
    {
        // create label shape
        uno::Reference< drawing::XShape > xGroupShapeForSingleEntry(
            xShapeFactory->createInstance(
                C2U( "com.sun.star.drawing.GroupShape" )), uno::UNO_QUERY_THROW );
        xEntry.set( xShapeFactory->createInstance(
                C2U( "com.sun.star.drawing.TextShape" )), uno::UNO_QUERY_THROW );

        xTarget->add( xGroupShapeForSingleEntry );

        xGroupShapes.set( xGroupShapeForSingleEntry, uno::UNO_QUERY_THROW );
        xGroupShapes->add( xEntry );

        // set label text
        uno::Reference< text::XTextRange > xRange( xEntry, uno::UNO_QUERY );
        if( xRange.is())
            xRange->setString( rText );

        // set character properties
        ::chart::PropertyMapper::setMultiProperties(
            rProperties.first, rProperties.second,
            uno::Reference< beans::XPropertySet >( xEntry,uno::UNO_QUERY ) );

        // adapt min-/max-extent
        awt::Size aEntrySize( xEntry->getSize() );
        rOutMaxExtentSoFar.Width =  ::std::max( rOutMaxExtentSoFar.Width,  aEntrySize.Width );
        rOutMaxExtentSoFar.Height = ::std::max( rOutMaxExtentSoFar.Height, aEntrySize.Height );
        if( aEntrySize.Height > 0 )
        {
            // setting initial value (otherwise (0,0) is always the minimum)
            if( rOutMinExtentSoFar.Height == 0 )
                rOutMinExtentSoFar = aEntrySize;
            else
            {
                rOutMinExtentSoFar.Width =  ::std::min( rOutMinExtentSoFar.Width,  aEntrySize.Width );
                rOutMinExtentSoFar.Height = ::std::min( rOutMinExtentSoFar.Height, aEntrySize.Height );
            }
        }

        // set identifier for selection handling
        uno::Reference< beans::XPropertySet > xEntryProps( xGroupShapeForSingleEntry,uno::UNO_QUERY );
        xEntryProps->setPropertyValue( C2U("Name"), uno::makeAny( rCID ));
    }
    catch( uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }

    return ::std::make_pair( xEntry, xGroupShapes );
}

/** Note: rOutMinExtentSoFar is the smallest non-zero size
 */
void lcl_getLegendEntries(
    const uno::Reference< chart2::XDataSeriesTreeParent > & xParent,
    tEntryGroup & rOutEntryContainer,
    const uno::Reference< drawing::XShapes > & xTarget,
    const uno::Reference< lang::XMultiServiceFactory > & xShapeFactory,
    const uno::Reference< chart2::XChartType > & xChartType,
    const tPropertyValues & rProperties,
    awt::Size & rOutMinExtentSoFar,
    awt::Size & rOutMaxExtentSoFar,
    const rtl::OUString& rLegendIdentifier,
    uno::Reference< frame::XModel > & xModel )
{
    uno::Sequence< uno::Reference< chart2::XDataSeriesTreeNode > > aChildren(
        xParent->getChildren());

    for( sal_Int32 nI = 0; nI < aChildren.getLength(); ++nI )
    {
        uno::Reference< chart2::XDataSeriesTreeParent > xNewParent( aChildren[ nI ], uno::UNO_QUERY );
        if( xNewParent.is())
        {
            // recurse !
            lcl_getLegendEntries( xNewParent, rOutEntryContainer, xTarget, xShapeFactory,
                                  xChartType, rProperties, rOutMinExtentSoFar, rOutMaxExtentSoFar,
                                  rLegendIdentifier, xModel );
        }
        else
        {
            try
            {
                // get label text via data source
                ::rtl::OUString aName;
                uno::Reference< chart2::XDataSource > xSeriesSource(
                    aChildren[ nI ], uno::UNO_QUERY );
                if( xSeriesSource.is())
                    aName = lcl_getLabelForSeries( xSeriesSource );

                // identifier for selection handling
                uno::Reference< beans::XPropertySet > xSeriesProp( xSeriesSource, uno::UNO_QUERY );
                rtl::OUString aSeriesIdentifier;
                xSeriesProp->getPropertyValue( C2U( "Identifier" ) ) >>= aSeriesIdentifier;

                rtl::OUString aCID = chart::ObjectIdentifier::createClassifiedIdentifier(
                    chart::OBJECTTYPE_LEGEND_ENTRY, aSeriesIdentifier
                    , chart::ObjectIdentifier::createParticle( chart::OBJECTTYPE_LEGEND, rLegendIdentifier ) );

                tShapeWithGroup aResult(
                    lcl_getLegendEntry( xShapeFactory, xTarget, aName, rProperties,
                                        rOutMinExtentSoFar, rOutMaxExtentSoFar,
                                        rOutEntryContainer, aCID ));

                // add entry to list
                rOutEntryContainer.push_back(
                    tEntryGroup::value_type( aResult.second, aResult.first, xSeriesProp, xChartType ));

                uno::Reference< chart2::XRegressionCurveContainer > xRegrCont(
                    aChildren[ nI ], uno::UNO_QUERY );
                if( xRegrCont.is())
                {
                    uno::Sequence< uno::Reference< chart2::XRegressionCurve > > aCurves(
                        xRegrCont->getRegressionCurves());
                    for( sal_Int32 nJ = 0; nJ < aCurves.getLength(); ++ nJ )
                    {
                        if( aCurves[ nJ ].is())
                        {
                            uno::Reference< chart2::XRegressionCurveCalculator >xCalc(
                                aCurves[ nJ ]->getCalculator());
                            if( xCalc.is())
                            {
                                RegressionCurveHelper::initializeCurveCalculator(
                                    xCalc,
                                    uno::Reference< chart2::XDataSeries >( aChildren[ nI ], uno::UNO_QUERY ),
                                    xModel );

                                ::rtl::OUString aText( xCalc->getRepresentation());
                                aResult =
                                    lcl_getLegendEntry( xShapeFactory, xTarget, aText, rProperties,
                                                        rOutMinExtentSoFar, rOutMaxExtentSoFar,
                                                        rOutEntryContainer, aCID );
                                // add entry to list
                                rOutEntryContainer.push_back(
                                    tEntryGroup::value_type( aResult.second, aResult.first,
                                                             uno::Reference< beans::XPropertySet >(
                                                                 aCurves[ nJ ], uno::UNO_QUERY ),
                                                             uno::Reference< chart2::XChartType >() ));
                            }
                        }
                    }
                }
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
        }
    }
}

void lcl_createLegend(
    const tEntryGroup & rEntries,
    chart2::LegendExpansion eExpansion,
    const uno::Reference< drawing::XShapes > & xTarget,
    const uno::Reference< lang::XMultiServiceFactory > & xShapeFactory,
    const awt::Size & rAvailableSpace,
    awt::Size aMinEntryExtent,
    awt::Size aMaxEntryExtent,
    awt::Size & rOutSize
    )
{
    const sal_Int32 nXPadding = 200;
    const sal_Int32 nYPadding = 250;
    const sal_Int32 nXOffset = 100;
    const sal_Int32 nYOffset = 100;

    awt::Size aMaxSymbolExtent( aMinEntryExtent.Height * 3/2, aMinEntryExtent.Height );
    sal_Int32 nCurrentXPos = nXPadding;
    sal_Int32 nCurrentYPos = nYPadding;
    sal_Int32 nMaxEntryWidth = 2 * nXOffset + aMaxSymbolExtent.Width + aMaxEntryExtent.Width;
    sal_Int32 nMaxEntryHeight = nYOffset + aMaxEntryExtent.Height;
    sal_Int32 nNumberOfEntries = rEntries.size();

    sal_Int32 nNumberOfColumns = 0, nNumberOfRows = 0;

    // determine layout depending on LegendExpansion
    if( eExpansion == chart2::LegendExpansion_HIGH )
    {
        sal_Int32 nMaxNumberOfRows = (rAvailableSpace.Height - 2*nYPadding ) / nMaxEntryHeight;

        nNumberOfColumns = static_cast< sal_Int32 >(
            ceil( static_cast< double >( nNumberOfEntries ) /
                  static_cast< double >( nMaxNumberOfRows ) ));
        nNumberOfRows =  static_cast< sal_Int32 >(
            ceil( static_cast< double >( nNumberOfEntries ) /
                  static_cast< double >( nNumberOfColumns ) ));
    }
    else if( eExpansion == chart2::LegendExpansion_WIDE )
    {
        sal_Int32 nMaxNumberOfColumns = (rAvailableSpace.Width - 2*nXPadding ) / nMaxEntryWidth;

        nNumberOfRows = static_cast< sal_Int32 >(
            ceil( static_cast< double >( nNumberOfEntries ) /
                  static_cast< double >( nMaxNumberOfColumns ) ));
        nNumberOfColumns = static_cast< sal_Int32 >(
            ceil( static_cast< double >( nNumberOfEntries ) /
                  static_cast< double >( nNumberOfRows ) ));
    }
    else // chart2::LegendExpansion_BALANCED
    {
        double fAspect =
            static_cast< double >( nMaxEntryWidth ) / static_cast< double >( nMaxEntryHeight );

        nNumberOfRows = static_cast< sal_Int32 >(
            ceil( sqrt( static_cast< double >( nNumberOfEntries ) * fAspect )));
        nNumberOfColumns = static_cast< sal_Int32 >(
            ceil( static_cast< double >( nNumberOfEntries ) /
                  static_cast< double >( nNumberOfRows ) ));
    }

    if(nNumberOfRows<=0)
        return;

    // calculate maximum height for current row
    std::vector< sal_Int32 > nMaxHeights( nNumberOfRows );
    sal_Int32 nRow = 0;
    sal_Int32 nColumn = 0;
    for( ; nRow < nNumberOfRows; ++nRow )
    {
        sal_Int32 nMaxHeight = 0;
        for( nColumn = 0; nColumn < nNumberOfColumns; ++nColumn )
        {
            sal_Int32 nEntry = ( eExpansion == chart2::LegendExpansion_WIDE )
                ? (nColumn + nRow * nNumberOfColumns)
                // HIGH or BALANCED
                : (nRow + nColumn * nNumberOfRows);
            if( nEntry < nNumberOfEntries )
                nMaxHeight = ::std::max(
                    nMaxHeight, nYOffset + rEntries[ nEntry ].xTextShape->getSize().Height );
        }
        nMaxHeights[ nRow ] = nMaxHeight;
    }

    // place entries ordered in optimal-width columns
    sal_Int32 nMaxYPos = 0;
    for( nColumn = 0; nColumn < nNumberOfColumns; ++nColumn )
    {
        sal_Int32 nMaxWidth = 0;
        nCurrentYPos = nYPadding;

        for( nRow = 0; nRow < nNumberOfRows; ++nRow )
        {
            sal_Int32 nEntry = ( eExpansion == chart2::LegendExpansion_WIDE )
                ? (nColumn + nRow * nNumberOfColumns)
                // HIGH or BALANCED
                : (nRow + nColumn * nNumberOfRows);

            if( nEntry >= nNumberOfEntries )
                break;

            const LegendEntry & rEntry = rEntries[ nEntry ];

            // symbol
            uno::Reference< drawing::XShape > xSymbol(
                lcl_getSymbol( rEntry.xChartType,
                               rEntry.xSymbolProperties,
                               xShapeFactory,
                               rEntry.xGroupShapes ));

            if( xSymbol.is())
            {
                // Note: aspect ratio should always be 3:2

                // set symbol size to 75% of maximum space
                awt::Size aSymbolSize(
                    aMaxSymbolExtent.Width  * 75 / 100,
                    aMaxSymbolExtent.Height * 75 / 100 );
                xSymbol->setSize( aSymbolSize );
                xSymbol->setPosition(
                    awt::Point(
                        nCurrentXPos + ((aMaxSymbolExtent.Width - aSymbolSize.Width) / 2),
                        nCurrentYPos + ((aMaxSymbolExtent.Height - aSymbolSize.Height) / 2)));

                //because of this name the symbol shape will be used for marking the legend
                chart::ShapeFactory(xShapeFactory).setShapeName( xSymbol, C2U("MarkHandles") );
            }

            // position text shape
            awt::Size aTextSize( rEntry.xTextShape->getSize());
            nMaxWidth = ::std::max(
                nMaxWidth, 2 * nXOffset + aMaxSymbolExtent.Width + aTextSize.Width );
            rEntry.xTextShape->setPosition(
                awt::Point( nCurrentXPos + aMaxSymbolExtent.Width, nCurrentYPos ));

            nCurrentYPos += nMaxHeights[ nRow ];
            nMaxYPos = ::std::max( nMaxYPos, nCurrentYPos );
        }
        nCurrentXPos += nMaxWidth;
    }

    rOutSize.Width  = nCurrentXPos + nXPadding;
    rOutSize.Height = nMaxYPos + nYPadding;
}

layout::AnchorPoint lcl_getAnchorByAutoPos( chart2::LegendPosition ePos )
{
    layout::AnchorPoint aResult;

    switch( ePos )
    {
        case chart2::LegendPosition_LINE_START:
            aResult.Alignment = ::layout_defaults::const_aLineStart;
            aResult.EscapeDirection = 0.0;
            break;
        case chart2::LegendPosition_LINE_END:
            aResult.Alignment = ::layout_defaults::const_aLineEnd;
            aResult.EscapeDirection = 180.0;
            break;
        case chart2::LegendPosition_PAGE_START:
            aResult.Alignment = ::layout_defaults::const_aPageStart;
            aResult.EscapeDirection = 270.0;
            break;
        case chart2::LegendPosition_PAGE_END:
            aResult.Alignment = ::layout_defaults::const_aPageEnd;
            aResult.EscapeDirection = 90.0;
            break;

        case chart2::LegendPosition_CUSTOM:
            // to avoid warning
        case chart2::LegendPosition_MAKE_FIXED_SIZE:
            // nothing to be set
            break;
    }

    return aResult;
}

awt::Point lcl_calculatePositionAndRemainingSpace( awt::Rectangle & rOutAvailableSpace,
                                 const awt::Size & rReferenceSize,
                                 double fPrimaryOffset, double fSecondaryOffset,
                                 chart2::LegendPosition ePos, const awt::Size& aLegendSize )
{
    layout::RelativePosition aOffset( fPrimaryOffset,fSecondaryOffset );
    layout::AnchorPoint aAnchor = lcl_getAnchorByAutoPos( ePos );

    // rotate aOffset (out-param)
    helper::LayoutHelper::rotatePoint( aAnchor.EscapeDirection, aOffset.Primary, aOffset.Secondary );

    sal_Int32 nOffsetX = static_cast< sal_Int32 >(
        aOffset.Primary * rReferenceSize.Width );
    // the standard angles are in a coordinate system where y goes up, in
    // the drawing layer y goes down, so negate it
    sal_Int32 nOffsetY = - static_cast< sal_Int32 >(
        aOffset.Secondary * rReferenceSize.Height );

    awt::Point aPos( static_cast< sal_Int32 >(
                            rOutAvailableSpace.X +
                            rOutAvailableSpace.Width * aAnchor.Alignment.Primary) +
                            nOffsetX,
                        static_cast< sal_Int32 >(
                            rOutAvailableSpace.Y +
                            rOutAvailableSpace.Height * aAnchor.Alignment.Secondary) +
                            nOffsetY );
    layout::Alignment aAlignment( helper::LayoutHelper::getStandardAlignmentByAngle(
                                        aAnchor.EscapeDirection ));

    // set position according to Alignment
    aPos.X -= static_cast< sal_Int32 >(
        ::rtl::math::round( aAlignment.Primary * static_cast< double >( aLegendSize.Width )));
    aPos.Y -= static_cast< sal_Int32 >(
        ::rtl::math::round( aAlignment.Secondary * static_cast< double >( aLegendSize.Height )));

    // adapt rOutAvailableSpace if LegendPosition is not CUSTOM
    switch( ePos )
    {
        case chart2::LegendPosition_LINE_START:
        {
            sal_Int32 nExtent = aLegendSize.Width + nOffsetX;
            rOutAvailableSpace.Width -= nExtent;
            rOutAvailableSpace.X += nExtent;
        }
        break;
        case chart2::LegendPosition_LINE_END:
            rOutAvailableSpace.Width -= ( aLegendSize.Width + nOffsetX );
            break;
        case chart2::LegendPosition_PAGE_START:
        {
            sal_Int32 nExtent = aLegendSize.Height + nOffsetY;
            rOutAvailableSpace.Height -= nExtent;
            rOutAvailableSpace.Y += nExtent;
        }
        break;
        case chart2::LegendPosition_PAGE_END:
            rOutAvailableSpace.Height -= ( aLegendSize.Height + nOffsetY );
            break;

        default:
            // nothing
            break;
    }
    return aPos;
}

} // anonymous namespace

VLegend::VLegend(
    const uno::Reference< chart2::XLegend > & xLegend ) :
        m_xLegend( xLegend )
{
}

// ----------------------------------------

void SAL_CALL VLegend::init(
    const uno::Reference< drawing::XShapes >& xTargetPage,
    const uno::Reference< lang::XMultiServiceFactory >& xFactory,
    const uno::Reference< frame::XModel >& xModel )
{
    m_xTarget = xTargetPage;
    m_xShapeFactory = xFactory;
    m_xModel = xModel;
}

// ----------------------------------------

// static
bool VLegend::isVisible( const uno::Reference< chart2::XLegend > & xLegend )
{
    if( ! xLegend.is())
        return sal_False;

    sal_Bool bShow = sal_False;
    try
    {
        uno::Reference< beans::XPropertySet > xLegendProp( xLegend, uno::UNO_QUERY_THROW );
        xLegendProp->getPropertyValue( C2U( "Show" )) >>= bShow;
    }
    catch( uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }

    return bShow;
}

// ----------------------------------------

void VLegend::createShapes(
    const awt::Size & rAvailableSpace,
    const awt::Size & rPageSize )
{
    if(! (m_xLegend.is() &&
          m_xShapeFactory.is() &&
          m_xTarget.is()))
        return;

    try
    {
        //create shape and add to page
        m_xShape.set( m_xShapeFactory->createInstance(
                          C2U( "com.sun.star.drawing.GroupShape" )), uno::UNO_QUERY );
        m_xTarget->add( m_xShape );

        // set Name
        rtl::OUString aLegendIdentifier;
        {
            uno::Reference< beans::XPropertySet > xShapeProp( m_xShape, uno::UNO_QUERY );
            uno::Reference< chart2::XIdentifiable > xLegendIdent( m_xLegend, uno::UNO_QUERY );
            if( xShapeProp.is() &&
                xLegendIdent.is())
            {
                aLegendIdentifier = xLegendIdent->getIdentifier();
                rtl::OUString aCID = ObjectIdentifier::createClassifiedIdentifier(
                    OBJECTTYPE_LEGEND, aLegendIdentifier );
                xShapeProp->setPropertyValue( C2U("Name"), uno::makeAny( aCID ));
            }
        }

        // create and insert sub-shapes
        uno::Reference< drawing::XShapes > xLegendContainer( m_xShape, uno::UNO_QUERY );
        if( xLegendContainer.is())
        {
            uno::Reference< drawing::XShape > xBorder(
                m_xShapeFactory->createInstance(
                    C2U( "com.sun.star.drawing.RectangleShape" )), uno::UNO_QUERY );

            // for quickly setting properties
            tPropertyValues aLineFillProperties;
            tPropertyValues aTextProperties;

            uno::Reference< beans::XPropertySet > xLegendProp( m_xLegend, uno::UNO_QUERY );
            chart2::LegendExpansion eExpansion = chart2::LegendExpansion_HIGH;
            float fMaxFontHeight = 6.0;
            if( xLegendProp.is())
            {
                // limit the width of texts to 20% of the total available width
                sal_Int32 nMaxLabelWidth = rAvailableSpace.Width / 5;
                lcl_getProperties( xLegendProp, aLineFillProperties, aTextProperties, nMaxLabelWidth,
                                   rPageSize );

                // get Expansion property
                xLegendProp->getPropertyValue( C2U( "Expansion" )) >>= eExpansion;
                xLegendProp->getPropertyValue( C2U( "CharHeight" )) >>= fMaxFontHeight;
            }

            if( xBorder.is())
            {
                xLegendContainer->add( xBorder );

                // apply legend properties
                PropertyMapper::setMultiProperties(
                    aLineFillProperties.first, aLineFillProperties.second,
                    uno::Reference< beans::XPropertySet >( xBorder, uno::UNO_QUERY ));

                //because of this name this border will be used for marking the legend
                ShapeFactory(m_xShapeFactory).setShapeName( xBorder, C2U("MarkHandles") );
            }

            // create entries
            uno::Sequence< uno::Reference< chart2::XLegendEntry > > aEntries( m_xLegend->getEntries());
            tEntryGroup aEntryGroup;
            awt::Size aMinEntryExtent;
            awt::Size aMaxEntryExtent;

            for( sal_Int32 nI = 0; nI < aEntries.getLength(); ++nI )
            {
                uno::Reference< chart2::XDataSeriesTreeParent > xGroup( aEntries[ nI ], uno::UNO_QUERY );
                if( xGroup.is())
                {
                    uno::Reference< chart2::XChartTypeGroup > xCTGroup( xGroup, uno::UNO_QUERY );
                    uno::Reference< chart2::XChartType > xChartType;
                    if( xCTGroup.is())
                        xChartType.set( xCTGroup->getChartType());

                    lcl_getLegendEntries( xGroup, aEntryGroup, xLegendContainer,
                                          m_xShapeFactory, xChartType,
                                          aTextProperties,
                                          aMinEntryExtent, aMaxEntryExtent, aLegendIdentifier,
                                          m_xModel );
                }
            }

            // place entries
            awt::Size aLegendSize;
            lcl_createLegend( aEntryGroup, eExpansion,
                              xLegendContainer, m_xShapeFactory,
                              rAvailableSpace, aMinEntryExtent, aMaxEntryExtent,
                              aLegendSize );

            if( xBorder.is())
                xBorder->setSize( aLegendSize );
        }
    }
    catch( uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

// ----------------------------------------

void VLegend::changePosition(
    awt::Rectangle & rOutAvailableSpace,
    const awt::Size & rReferenceSize )
{
    if(! m_xShape.is())
        return;

    try
    {
        // determine position and alignment depending on default position
        awt::Size aLegendSize = m_xShape->getSize();

        uno::Reference< beans::XPropertySet > xLegendProp( m_xLegend, uno::UNO_QUERY_THROW );

        bool bAutoPosition = true;
        double fPrimaryOffset=0.02;// shift legend about 2% into the primary direction by default
        double fSecondaryOffset=0.0;
        ::com::sun::star::layout::RelativePosition aRelativePosition;
        if( (xLegendProp->getPropertyValue( C2U( "RelativePosition" ) )>>=aRelativePosition) )
        {
            fPrimaryOffset = aRelativePosition.Primary;
            fSecondaryOffset = aRelativePosition.Secondary;
            bAutoPosition = false;
        }
        chart2::LegendPosition ePos = chart2::LegendPosition_CUSTOM;
        xLegendProp->getPropertyValue( C2U( "AnchorPosition" )) >>= ePos;

        awt::Rectangle aAvailableSpace(rOutAvailableSpace);

        //calculate position:
        if(!bAutoPosition) //autoposition relative to whole page
            rOutAvailableSpace = awt::Rectangle(0,0,rReferenceSize.Width,rReferenceSize.Height);
        awt::Point aPos = lcl_calculatePositionAndRemainingSpace(
            rOutAvailableSpace, rReferenceSize,
            fPrimaryOffset, fSecondaryOffset, ePos, aLegendSize );
        m_xShape->setPosition( aPos );

        //calculate remaining space as if having autoposition:
        if(!bAutoPosition)
        {
            fPrimaryOffset=0.02;// shift legend about 2% into the primary direction by default
            fSecondaryOffset=0.0;
            lcl_calculatePositionAndRemainingSpace(
                aAvailableSpace, rReferenceSize, fPrimaryOffset, fSecondaryOffset, ePos, aLegendSize );
            rOutAvailableSpace=aAvailableSpace;
        }
    }
    catch( uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

//.............................................................................
} //namespace chart
//.............................................................................
