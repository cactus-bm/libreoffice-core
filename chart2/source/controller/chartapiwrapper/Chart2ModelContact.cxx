/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: Chart2ModelContact.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 17:16:34 $
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

#include "Chart2ModelContact.hxx"
#include "ChartModelHelper.hxx"
#include "LegendHelper.hxx"
#include "CommonConverters.hxx"
#include "macros.hxx"
#include "servicenames.hxx"
#include "ObjectIdentifier.hxx"
#include "chartview/ExplicitValueProvider.hxx"
#include "chartview/DrawModelWrapper.hxx"
#include "AxisHelper.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;

namespace chart
{
namespace wrapper
{

namespace
{

rtl::OUString lcl_getCIDForDiagram( const Reference< frame::XModel >& xChartModel )
{
    uno::Reference< XDiagram > xDiagram( ChartModelHelper::findDiagram( xChartModel ) );
    return ObjectIdentifier::createClassifiedIdentifierForObject( xDiagram, xChartModel );
}

} //anonymous namespace

Chart2ModelContact::Chart2ModelContact(
    const Reference< uno::XComponentContext > & xContext ) :
        m_xContext( xContext ),
        m_xChartModel( 0 ),
        m_xChartView(0)
{
}

Chart2ModelContact::~Chart2ModelContact()
{
    this->clear();
}

void Chart2ModelContact::setModel( const ::com::sun::star::uno::Reference<
                       ::com::sun::star::frame::XModel >& xChartModel )
{
    this->clear();
    m_xChartModel = xChartModel;
    uno::Reference< lang::XMultiServiceFactory > xTableFactory( xChartModel, uno::UNO_QUERY );
    if( xTableFactory.is() )
    {
        uno::Reference< container::XNameContainer > xDashTable( xTableFactory->createInstance( C2U( "com.sun.star.drawing.DashTable" ) ), uno::UNO_QUERY );
        uno::Reference< container::XNameContainer > xGradientTable( xTableFactory->createInstance( C2U( "com.sun.star.drawing.GradientTable" ) ), uno::UNO_QUERY );
        uno::Reference< container::XNameContainer > xHatchTable( xTableFactory->createInstance( C2U( "com.sun.star.drawing.HatchTable" ) ), uno::UNO_QUERY );
        uno::Reference< container::XNameContainer > xBitmapTable( xTableFactory->createInstance( C2U( "com.sun.star.drawing.BitmapTable" ) ), uno::UNO_QUERY );
        uno::Reference< container::XNameContainer > xTransparencyGradientTable( xTableFactory->createInstance( C2U( "com.sun.star.drawing.TransparencyGradientTable" ) ), uno::UNO_QUERY );
        //C2U( "com.sun.star.drawing.MarkerTable" )
        m_aTableMap[ C2U( "LineDashName" ) ] = xDashTable;
        m_aTableMap[ C2U( "FillGradientName" ) ] = xGradientTable;
        m_aTableMap[ C2U( "FillHatchName" ) ] = xHatchTable;
        m_aTableMap[ C2U( "FillBitmapName" ) ] = xBitmapTable;
        m_aTableMap[ C2U( "FillTransparenceGradientName" ) ] = xTransparencyGradientTable;
    }
}

void Chart2ModelContact::clear()
{
    m_xChartModel = uno::WeakReference< frame::XModel >(0);
}

Reference< frame::XModel > Chart2ModelContact::getChartModel() const
{
    return Reference< frame::XModel >( m_xChartModel.get(), uno::UNO_QUERY );
}

Reference< chart2::XChartDocument > Chart2ModelContact::getChart2Document() const
{
    return Reference< chart2::XChartDocument >( m_xChartModel.get(), uno::UNO_QUERY );
}

Reference< chart2::XDiagram > Chart2ModelContact::getChart2Diagram() const
{
    return ChartModelHelper::findDiagram( this->getChartModel() );
}

ExplicitValueProvider* Chart2ModelContact::getExplicitValueProvider() const
{
    if(!m_xChartView.is())
    {
        // get the chart view
        Reference<frame::XModel> xModel(m_xChartModel);
        uno::Reference< lang::XMultiServiceFactory > xFact( xModel, uno::UNO_QUERY );
        if( xFact.is() )
            m_xChartView = Reference< lang::XUnoTunnel >( xFact->createInstance( CHART_VIEW_SERVICE_NAME ), uno::UNO_QUERY );
    }

    if(!m_xChartView.is())
        return 0;

    //obtain the ExplicitValueProvider from the chart view
    ExplicitValueProvider* pProvider = reinterpret_cast<ExplicitValueProvider*>(m_xChartView->getSomething(
        ExplicitValueProvider::getUnoTunnelId() ));
    return pProvider;
}

uno::Reference< drawing::XDrawPage > Chart2ModelContact::getDrawPage()
{
    uno::Reference< drawing::XDrawPage > xResult;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider )
    {
        xResult.set( pProvider->getDrawModelWrapper()->getMainDrawPage() );
    }
    return xResult;
}

sal_Bool Chart2ModelContact::getExplicitValuesForAxis(
    const Reference< XAxis > & xAxis,
    ExplicitScaleData &  rOutExplicitScale,
    ExplicitIncrementData & rOutExplicitIncrement )
{
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider )
    {
        return pProvider->getExplicitValuesForAxis(
            xAxis, rOutExplicitScale, rOutExplicitIncrement );
    }
    return sal_False;
}

sal_Int32 Chart2ModelContact::getExplicitNumberFormatKeyForAxis(
            const Reference< chart2::XAxis >& xAxis )
{
    Reference< chart2::XCoordinateSystem > xCooSys(
        AxisHelper::getCoordinateSystemOfAxis(
              xAxis, ChartModelHelper::findDiagram( m_xChartModel ) ) );

    return ExplicitValueProvider::getExplicitNumberFormatKeyForAxis( xAxis, xCooSys
              , Reference< util::XNumberFormatsSupplier >( m_xChartModel.get(), uno::UNO_QUERY ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

uno::Any Chart2ModelContact::GetListElementByName(
    const rtl::OUString& rElementName, const rtl::OUString& rTableTypePropertyName )
{
    uno::Any aResult;
    try
    {
        tTableMap::iterator aIter( m_aTableMap.find(rTableTypePropertyName) );
        if( aIter == m_aTableMap.end() )
        {
            DBG_ERROR("no container found for the given type (gradient/hatch etc)");
            return aResult;
        }

        if( !aIter->second->hasByName(rElementName) )
        {
            DBG_ERROR("no element with the given name found");
            return aResult;
        }

        aResult = aIter->second->getByName(rElementName);
    }
    catch( uno::Exception& e)
    {
        ASSERT_EXCEPTION( e );
    }
    return aResult;
}

::rtl::OUString Chart2ModelContact::GetNameOfListElement(
        const uno::Any& rElementValue, const ::rtl::OUString& rTableTypePropertyName )
{
    try
    {
        tTableMap::iterator aIter( m_aTableMap.find(rTableTypePropertyName) );
        if( aIter == m_aTableMap.end() )
        {
            DBG_ERROR("no container found for the given type (gradient/hatch etc)");
            return ::rtl::OUString();
        }

        uno::Reference< container::XNameContainer > xTable( aIter->second, uno::UNO_QUERY );
        if( xTable.is() )
        {
            uno::Sequence< rtl::OUString > aNames( xTable->getElementNames() );
            for( sal_Int32 nN = aNames.getLength(); nN--; )
            {
                if( xTable->getByName( aNames[nN] ) == rElementValue )
                {
                    return aNames[nN];
                }
            }
        }
    }
    catch( uno::Exception& e)
    {
        ASSERT_EXCEPTION( e );
    }
    return ::rtl::OUString();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

awt::Size Chart2ModelContact::GetPageSize() const
{
    return ChartModelHelper::getPageSize(m_xChartModel);
}

awt::Size Chart2ModelContact::GetDiagramSize() const
{
    awt::Size aSize;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider )
    {
        aSize = ToSize( pProvider->getRectangleOfObject( lcl_getCIDForDiagram( m_xChartModel ) ) );
    }
    return aSize;
}

awt::Rectangle Chart2ModelContact::GetDiagramRectangleInclusive() const
{
    awt::Rectangle aRect;

    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider )
    {
        aRect = pProvider->getRectangleOfObject( lcl_getCIDForDiagram( m_xChartModel ) );
    }
    //add axis title sizes to the diagram size
    aRect = ExplicitValueProvider::calculateDiagramPositionAndSizeInclusiveTitle(
        m_xChartModel, m_xChartView, aRect );

    return aRect;
}

awt::Size Chart2ModelContact::GetDiagramSizeInclusive() const
{
    return ToSize( this->GetDiagramRectangleInclusive() );
}

awt::Point Chart2ModelContact::GetDiagramPositionInclusive() const
{
    return ToPoint( this->GetDiagramRectangleInclusive() );
}

awt::Point Chart2ModelContact::GetDiagramPosition() const
{
    awt::Point aPoint;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider )
    {
        aPoint = ToPoint( pProvider->getRectangleOfObject( lcl_getCIDForDiagram( m_xChartModel ) ) );
    }
    return aPoint;
}

awt::Size Chart2ModelContact::GetLegendSize() const
{
    awt::Size aSize;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider )
    {
        uno::Reference< chart2::XLegend > xLegend( LegendHelper::getLegend( m_xChartModel ) );
        rtl::OUString aCID( ObjectIdentifier::createClassifiedIdentifierForObject( xLegend, m_xChartModel ) );
        aSize = ToSize( pProvider->getRectangleOfObject( aCID ) );
    }
    return aSize;
}

awt::Point Chart2ModelContact::GetLegendPosition() const
{
    awt::Point aPoint;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider )
    {
        uno::Reference< chart2::XLegend > xLegend( LegendHelper::getLegend( m_xChartModel ) );
        rtl::OUString aCID( ObjectIdentifier::createClassifiedIdentifierForObject( xLegend, m_xChartModel ) );
        aPoint = ToPoint( pProvider->getRectangleOfObject( aCID ) );
    }
    return aPoint;
}

awt::Size Chart2ModelContact::GetTitleSize( const uno::Reference<
        ::com::sun::star::chart2::XTitle > & xTitle ) const
{
    awt::Size aSize;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider && xTitle.is() )
    {
        rtl::OUString aCID( ObjectIdentifier::createClassifiedIdentifierForObject( xTitle, m_xChartModel ) );
        aSize = ToSize( pProvider->getRectangleOfObject( aCID ) );
    }
    return aSize;
}

awt::Point Chart2ModelContact::GetTitlePosition( const uno::Reference<
        ::com::sun::star::chart2::XTitle > & xTitle ) const
{
    awt::Point aPoint;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider && xTitle.is() )
    {
        rtl::OUString aCID( ObjectIdentifier::createClassifiedIdentifierForObject( xTitle, m_xChartModel ) );
        aPoint = ToPoint( pProvider->getRectangleOfObject( aCID ) );
    }
    return aPoint;
}

awt::Size Chart2ModelContact::GetAxisSize( const uno::Reference<
        ::com::sun::star::chart2::XAxis > & xAxis ) const
{
    awt::Size aSize;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider && xAxis.is() )
    {
        rtl::OUString aCID( ObjectIdentifier::createClassifiedIdentifierForObject( xAxis, m_xChartModel ) );
        aSize = ToSize( pProvider->getRectangleOfObject( aCID ) );
    }
    return aSize;
}

awt::Point Chart2ModelContact::GetAxisPosition( const uno::Reference<
        ::com::sun::star::chart2::XAxis > & xAxis ) const
{
    awt::Point aPoint;
    ExplicitValueProvider* pProvider( getExplicitValueProvider() );
    if( pProvider && xAxis.is() )
    {
        rtl::OUString aCID( ObjectIdentifier::createClassifiedIdentifierForObject( xAxis, m_xChartModel ) );
        aPoint = ToPoint( pProvider->getRectangleOfObject( aCID ) );
    }
    return aPoint;
}

} //  namespace wrapper
} //  namespace chart
