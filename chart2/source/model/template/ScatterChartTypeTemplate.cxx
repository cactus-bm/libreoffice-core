/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ScatterChartTypeTemplate.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 01:24:28 $
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
#include "ScatterChartTypeTemplate.hxx"
#include "ScatterChartType.hxx"
#include "macros.hxx"
#include "algohelper.hxx"
#include "DataSeriesTreeHelper.hxx"

#ifndef _COM_SUN_STAR_CHART2_SYMBOLSTYLE_HPP_
#include <com/sun/star/chart2/SymbolStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_SYMBOL_HPP_
#include <com/sun/star/chart2/Symbol.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_LINESTYLE_HPP_
#include <com/sun/star/drawing/LineStyle.hpp>
#endif

#ifndef CHART_PROPERTYHELPER_HXX
#include "PropertyHelper.hxx"
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

#include <algorithm>

using namespace ::com::sun::star;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;
using ::com::sun::star::beans::Property;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::osl::MutexGuard;

namespace
{

static const OUString lcl_aServiceName(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart2.ScatterChartTypeTemplate" ));

enum
{
    PROP_SCATTERCHARTTYPE_TEMPLATE_CURVE_RESOLUTION,
    PROP_SCATTERCHARTTYPE_TEMPLATE_SPLINE_ORDER

};

void lcl_AddPropertiesToVector(
    ::std::vector< Property > & rOutProperties )
{
    rOutProperties.push_back(
        Property( C2U( "CurveResolution" ),
                  PROP_SCATTERCHARTTYPE_TEMPLATE_CURVE_RESOLUTION,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    rOutProperties.push_back(
        Property( C2U( "SplineOrder" ),
                  PROP_SCATTERCHARTTYPE_TEMPLATE_SPLINE_ORDER,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
}

void lcl_AddDefaultsToMap(
    ::chart::helper::tPropertyValueMap & rOutMap )
{
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_SCATTERCHARTTYPE_TEMPLATE_CURVE_RESOLUTION ));
    rOutMap[ PROP_SCATTERCHARTTYPE_TEMPLATE_CURVE_RESOLUTION ] =
        uno::makeAny( sal_Int32( 20 ) );

    // todo: check whether order 3 means polygons of order 3 or 2. (see
    // http://www.people.nnov.ru/fractal/Splines/Basis.htm )
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_SCATTERCHARTTYPE_TEMPLATE_SPLINE_ORDER ));
    rOutMap[ PROP_SCATTERCHARTTYPE_TEMPLATE_SPLINE_ORDER ] =
        uno::makeAny( sal_Int32( 3 ) );
}

const Sequence< Property > & lcl_GetPropertySequence()
{
    static Sequence< Property > aPropSeq;

    // /--
    MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aPropSeq.getLength() )
    {
        // get properties
        ::std::vector< ::com::sun::star::beans::Property > aProperties;
        lcl_AddPropertiesToVector( aProperties );

        // and sort them for access via bsearch
        ::std::sort( aProperties.begin(), aProperties.end(),
                     ::chart::helper::PropertyNameLess() );

        // transfer result to static Sequence
        aPropSeq = ::chart::helper::VectorToSequence( aProperties );
    }

    return aPropSeq;
}

::cppu::IPropertyArrayHelper & lcl_getInfoHelper()
{
    static ::cppu::OPropertyArrayHelper aArrayHelper(
        lcl_GetPropertySequence(),
        /* bSorted = */ sal_True );

    return aArrayHelper;
}

} // anonymous namespace

namespace chart
{

ScatterChartTypeTemplate::ScatterChartTypeTemplate(
    Reference<
        uno::XComponentContext > const & xContext,
    const OUString & rServiceName,
    chart2::CurveStyle eCurveStyle,
    bool bSymbols,
    bool bHasLines /* = true */,
    sal_Int32 nDim /* = 2 */ ) :
        ChartTypeTemplate( xContext, rServiceName ),
        ::property::OPropertySet( m_aMutex ),
    m_eCurveStyle( eCurveStyle ),
    m_bHasSymbols( bSymbols ),
    m_bHasLines( bHasLines ),
    m_nDim( nDim )
{}

ScatterChartTypeTemplate::~ScatterChartTypeTemplate()
{}

// ____ OPropertySet ____
uno::Any ScatterChartTypeTemplate::GetDefaultValue( sal_Int32 nHandle ) const
    throw(beans::UnknownPropertyException)
{
    static helper::tPropertyValueMap aStaticDefaults;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aStaticDefaults.size() )
    {
        // initialize defaults
        lcl_AddDefaultsToMap( aStaticDefaults );
    }

    helper::tPropertyValueMap::const_iterator aFound(
        aStaticDefaults.find( nHandle ));

    if( aFound == aStaticDefaults.end())
        return uno::Any();

    return (*aFound).second;
    // \--
}

::cppu::IPropertyArrayHelper & SAL_CALL ScatterChartTypeTemplate::getInfoHelper()
{
    return lcl_getInfoHelper();
}


// ____ XPropertySet ____
uno::Reference< beans::XPropertySetInfo > SAL_CALL
    ScatterChartTypeTemplate::getPropertySetInfo()
    throw (uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo > xInfo;

    // /--
    MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( !xInfo.is())
    {
        xInfo = ::cppu::OPropertySetHelper::createPropertySetInfo(
            getInfoHelper());
    }

    return xInfo;
    // \--
}

sal_Int32 ScatterChartTypeTemplate::getDimension() const
{
    return m_nDim;
}

Reference< chart2::XChartType > ScatterChartTypeTemplate::getDefaultChartType()
    throw (uno::RuntimeException)
{
    sal_Int32 nRes;
    sal_Int32 nOrder;

    if( ( getFastPropertyValue( PROP_SCATTERCHARTTYPE_TEMPLATE_CURVE_RESOLUTION ) >>= nRes ) &&
        ( getFastPropertyValue( PROP_SCATTERCHARTTYPE_TEMPLATE_SPLINE_ORDER ) >>= nOrder ) )
    {
        return new ScatterChartType( m_nDim, m_eCurveStyle, nRes, nOrder );
    }

    return new ScatterChartType( m_nDim, m_eCurveStyle );
}

// ____ XChartTypeTemplate ____
Reference< chart2::XDiagram > SAL_CALL
    ScatterChartTypeTemplate::createDiagram(
        const Sequence< Reference< chart2::XDataSeries > >& aSeriesSeq )
    throw (uno::RuntimeException)
{
    // set symbol type at data series
    chart2::SymbolStyle eStyle = m_bHasSymbols
        ? chart2::SymbolStyle_STANDARD
        : chart2::SymbolStyle_NONE;

    uno::Any aLineStyleToSet( uno::makeAny(
                                  m_bHasLines
                                  ? drawing::LineStyle_SOLID
                                  : drawing::LineStyle_NONE ));
    for( sal_Int32 i = 0; i < aSeriesSeq.getLength(); ++i )
    {
        try
        {
            chart2::Symbol aSymbProp;
            Reference< beans::XPropertySet > xProp( aSeriesSeq[i], uno::UNO_QUERY_THROW );
            if( (xProp->getPropertyValue( C2U( "Symbol" )) >>= aSymbProp ) )
            {
                aSymbProp.aStyle = eStyle;
                if( m_bHasSymbols )
                    aSymbProp.nStandardSymbol = i;
                xProp->setPropertyValue( C2U( "Symbol" ), uno::makeAny( aSymbProp ));
            }

            if( m_bHasLines )
            {
                // keep line-styles that are not NONE
                drawing::LineStyle eLineStyle;
                if( (xProp->getPropertyValue( C2U( "LineStyle" )) >>= eLineStyle ) &&
                    eLineStyle == drawing::LineStyle_NONE )
                {
                    xProp->setPropertyValue( C2U( "LineStyle" ), aLineStyleToSet );
                }
            }
            else
                xProp->setPropertyValue( C2U( "LineStyle" ), aLineStyleToSet );
        }
        catch( uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    // todo: set symbol type at data points

    return ChartTypeTemplate::createDiagram( aSeriesSeq );
}

sal_Bool SAL_CALL ScatterChartTypeTemplate::matchesTemplate(
    const Reference< chart2::XDiagram >& xDiagram )
    throw (uno::RuntimeException)
{
    sal_Bool bResult = ChartTypeTemplate::matchesTemplate( xDiagram );

    // check curve-style
    if( bResult )
    {
        try
        {
            Reference< beans::XPropertySet > xCTProp(
                helper::DataSeriesTreeHelper::getChartTypeByIndex( xDiagram->getTree(), 0 ),
                uno::UNO_QUERY_THROW );
            chart2::CurveStyle eCurveStyle;
            if( xCTProp->getPropertyValue( C2U( "CurveStyle" )) >>= eCurveStyle )
            {
                bResult = ( eCurveStyle == m_eCurveStyle );
            }
        }
        catch( uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    // check symbol-style
    if( bResult )
    {
        Sequence< Reference< chart2::XDataSeries > > aSeriesSeq(
            helper::DataSeriesTreeHelper::getDataSeriesFromDiagram( xDiagram ));
        chart2::SymbolStyle eStyle = m_bHasSymbols
            ? chart2::SymbolStyle_STANDARD
            : chart2::SymbolStyle_NONE;

        for( sal_Int32 i = 0; i < aSeriesSeq.getLength(); ++i )
        {
            try
            {
                chart2::Symbol aSymbProp;
                drawing::LineStyle eLineStyle;
                Reference< beans::XPropertySet > xProp( aSeriesSeq[i], uno::UNO_QUERY_THROW );
                if( ( (xProp->getPropertyValue( C2U( "Symbol" )) >>= aSymbProp) &&
                      (aSymbProp.aStyle != eStyle) ) ||
                    ( (xProp->getPropertyValue( C2U( "LineStyle" )) >>= eLineStyle) &&
                      (m_bHasLines != ( eLineStyle != drawing::LineStyle_NONE )) ) )
                {
                    bResult = false;
                    break;
                }
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
        }
    }

    return bResult;
}

// ----------------------------------------

Sequence< OUString > ScatterChartTypeTemplate::getSupportedServiceNames_Static()
{
    Sequence< OUString > aServices( 2 );
    aServices[ 0 ] = lcl_aServiceName;
    aServices[ 1 ] = C2U( "com.sun.star.chart2.ChartTypeTemplate" );
    return aServices;
}

// implement XServiceInfo methods basing upon getSupportedServiceNames_Static
APPHELPER_XSERVICEINFO_IMPL( ScatterChartTypeTemplate, lcl_aServiceName );

IMPLEMENT_FORWARD_XINTERFACE2( ScatterChartTypeTemplate, ChartTypeTemplate, OPropertySet )
IMPLEMENT_FORWARD_XTYPEPROVIDER2( ScatterChartTypeTemplate, ChartTypeTemplate, OPropertySet )

} //  namespace chart
