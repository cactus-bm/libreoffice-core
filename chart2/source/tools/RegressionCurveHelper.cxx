/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: RegressionCurveHelper.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-25 09:00:04 $
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
#include "RegressionCurveHelper.hxx"
#include "MeanValueRegressionCurveCalculator.hxx"
#include "LinearRegressionCurveCalculator.hxx"
#include "LogarithmicRegressionCurveCalculator.hxx"
#include "ExponentialRegressionCurveCalculator.hxx"
#include "PotentialRegressionCurveCalculator.hxx"
#include "CommonConverters.hxx"
#include "RegressionCurveModel.hxx"
#include "ChartTypeHelper.hxx"
#include "ChartModelHelper.hxx"
#include "macros.hxx"
#include "PropertyHelper.hxx"
#include "ResId.hxx"
#include "Strings.hrc"

#ifndef _COM_SUN_STAR_CHART2_XCHARTDOCUMENT_HPP_
#include <com/sun/star/chart2/XChartDocument.hpp>
#endif

//.............................................................................
namespace chart
{
//.............................................................................

using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::XComponentContext;
using ::com::sun::star::lang::XServiceName;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Exception;
using ::rtl::OUString;

// static
Reference< XRegressionCurve > RegressionCurveHelper::createMeanValueLine(
    const Reference< XComponentContext > & xContext )
{
    return Reference< XRegressionCurve >(
        new MeanValueRegressionCurve( xContext ));
}

// static
Reference< XRegressionCurve > RegressionCurveHelper::createRegressionCurveByServiceName(
    const Reference< XComponentContext > & xContext,
    ::rtl::OUString aServiceName )
{
    Reference< XRegressionCurve > xResult;

    // todo: use factory methods with service name
    if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                "com.sun.star.chart2.LinearRegressionCurve" )))
    {
        xResult.set(
            new LinearRegressionCurve( xContext ));
    }
    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.LogarithmicRegressionCurve" )))
    {
        xResult.set(
            new LogarithmicRegressionCurve( xContext ));
    }
    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.ExponentialRegressionCurve" )))
    {
        xResult.set(
            new ExponentialRegressionCurve( xContext ));
    }
    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.PotentialRegressionCurve" )))
    {
        xResult.set(
            new PotentialRegressionCurve( xContext ));
    }

    return xResult;
}

// ------------------------------------------------------------

// static
Reference< XRegressionCurveCalculator > RegressionCurveHelper::createMeanValueLineCalculator()
{
    return Reference< XRegressionCurveCalculator >( new MeanValueRegressionCurveCalculator());
}

// static
Reference< XRegressionCurveCalculator > RegressionCurveHelper::createRegressionCurveCalculatorByServiceName(
    ::rtl::OUString aServiceName )
{
    Reference< XRegressionCurveCalculator > xResult;

    // todo: use factory methods with service name
    if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.MeanValueRegressionCurve" )))
    {
        xResult.set( new MeanValueRegressionCurveCalculator());
    }
    if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.LinearRegressionCurve" )))
    {
        xResult.set( new LinearRegressionCurveCalculator());
    }
    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.LogarithmicRegressionCurve" )))
    {
        xResult.set( new LogarithmicRegressionCurveCalculator());
    }
    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.ExponentialRegressionCurve" )))
    {
        xResult.set( new ExponentialRegressionCurveCalculator());
    }
    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                     "com.sun.star.chart2.PotentialRegressionCurve" )))
    {
        xResult.set( new PotentialRegressionCurveCalculator());
    }

    return xResult;
}

// static
void RegressionCurveHelper::initializeCurveCalculator(
    const Reference< XRegressionCurveCalculator > & xOutCurveCalculator,
    const Reference< data::XDataSource > & xSource,
    bool bUseXValuesIfAvailable /* = true */ )
{
    if( ! (xOutCurveCalculator.is() &&
           xSource.is() ))
        return;

    Sequence< double > aXValues, aYValues;
    bool bXValuesFound = false, bYValuesFound = false;

    Sequence< Reference< data::XLabeledDataSequence > > aDataSeqs( xSource->getDataSequences());
    sal_Int32 i = 0;
    for( i=0;
         ! (bXValuesFound && bYValuesFound) && i<aDataSeqs.getLength();
         ++i )
    {
        try
        {
            Reference< data::XDataSequence > xSeq( aDataSeqs[i]->getValues());
            Reference< XPropertySet > xProp( xSeq, uno::UNO_QUERY_THROW );
            ::rtl::OUString aRole;
            if( xProp->getPropertyValue(
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Role" ))) >>= aRole )
            {
                if( bUseXValuesIfAvailable &&
                    ! bXValuesFound &&
                    aRole.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "values-x" )))
                {
                    aXValues = DataSequenceToDoubleSequence( xSeq );
                    bXValuesFound = true;
                }
                else if( ! bYValuesFound &&
                         aRole.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "values-y" )))
                {
                    aYValues = DataSequenceToDoubleSequence( xSeq );
                    bYValuesFound = true;
                }
            }
        }
        catch( Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    if( ! bXValuesFound &&
        bYValuesFound )
    {
        // initialize with 1, 2, ...
        //first category (index 0) matches with real number 1.0
        aXValues.realloc( aYValues.getLength());
        for( i=0; i<aXValues.getLength(); ++i )
            aXValues[i] = i+1;
        bXValuesFound = true;
    }

    if( bXValuesFound && bYValuesFound &&
        aXValues.getLength() > 0 &&
        aYValues.getLength() > 0 )
        xOutCurveCalculator->recalculateRegression( aXValues, aYValues );
}

// static
void RegressionCurveHelper::initializeCurveCalculator(
    const Reference< XRegressionCurveCalculator > & xOutCurveCalculator,
    const Reference< XDataSeries > & xSeries,
    const Reference< frame::XModel > & xModel )
{
    sal_Int32 nAxisType = ChartTypeHelper::getAxisType(
        ChartModelHelper::getChartTypeOfSeries( xModel, xSeries ), 0 ); // x-axis

    initializeCurveCalculator( xOutCurveCalculator,
                               uno::Reference< data::XDataSource >( xSeries, uno::UNO_QUERY ),
                               (nAxisType == AxisType::REALNUMBER) );
}

// ----------------------------------------

// static
bool RegressionCurveHelper::hasMeanValueLine(
    const uno::Reference< XRegressionCurveContainer > & xRegCnt )
{
    if( !xRegCnt.is())
        return false;

    try
    {
        uno::Sequence< uno::Reference< XRegressionCurve > > aCurves(
            xRegCnt->getRegressionCurves());
        for( sal_Int32 i = 0; i < aCurves.getLength(); ++i )
        {
            if( isMeanValueLine( aCurves[i] ))
                return true;
        }
    }
    catch( Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }

    return false;
}

// static
bool RegressionCurveHelper::isMeanValueLine(
    const uno::Reference< chart2::XRegressionCurve > & xRegCurve )
{
    uno::Reference< XServiceName > xServName( xRegCurve, uno::UNO_QUERY );
    if( xServName.is() &&
        xServName->getServiceName().equals(
            C2U( "com.sun.star.chart2.MeanValueRegressionCurve" )))
        return true;
    return false;
}

// static
uno::Reference< chart2::XRegressionCurve >
    RegressionCurveHelper::getMeanValueLine(
        const uno::Reference< chart2::XRegressionCurveContainer > & xRegCnt )
{
    if( xRegCnt.is())
    {
        try
        {
            uno::Sequence< uno::Reference< XRegressionCurve > > aCurves(
                xRegCnt->getRegressionCurves());
            for( sal_Int32 i = 0; i < aCurves.getLength(); ++i )
            {
                if( isMeanValueLine( aCurves[i] ))
                    return aCurves[i];
            }
        }
        catch( Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    return uno::Reference< chart2::XRegressionCurve >();
}

// static
void RegressionCurveHelper::addMeanValueLine(
    uno::Reference< XRegressionCurveContainer > & xRegCnt,
    const uno::Reference< XComponentContext > & xContext,
    const uno::Reference< XPropertySet > & xSeriesProp )
{
    if( !xRegCnt.is() ||
        ::chart::RegressionCurveHelper::hasMeanValueLine( xRegCnt ) )
        return;

    // todo: use a valid context
    uno::Reference< XRegressionCurve > xCurve( createMeanValueLine( xContext ));
    xRegCnt->addRegressionCurve( xCurve );

    if( xSeriesProp.is())
    {
        uno::Reference< XPropertySet > xProp( xCurve, uno::UNO_QUERY );
        if( xProp.is())
        {
            xProp->setPropertyValue( C2U( "LineColor" ),
                                     xSeriesProp->getPropertyValue( C2U( "Color" )));
        }
    }
}

// static
void RegressionCurveHelper::removeMeanValueLine(
    Reference< XRegressionCurveContainer > & xRegCnt )
{
    if( !xRegCnt.is())
        return;

    try
    {
        Sequence< Reference< XRegressionCurve > > aCurves(
            xRegCnt->getRegressionCurves());
        for( sal_Int32 i = 0; i < aCurves.getLength(); ++i )
        {
            if( isMeanValueLine( aCurves[i] ))
            {
                xRegCnt->removeRegressionCurve( aCurves[i] );
                // attention: the iterator i has become invalid now

                // note: assume that there is only one mean-value curve
                // to remove multiple mean-value curves remove the break
                break;
            }
        }
    }
    catch( Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

void RegressionCurveHelper::addRegressionCurve(
    tRegressionType eType,
    uno::Reference< XRegressionCurveContainer > & xRegCnt,
    const uno::Reference< XComponentContext > & /* xContext */,
    const uno::Reference< beans::XPropertySet >& xPropertySource )
{
    if( !xRegCnt.is() )
        return;

    if( eType == REGRESSION_TYPE_NONE )
    {
        OSL_ENSURE(false,"don't create a regression curve of type none");
        return;
    }

    uno::Reference< chart2::XRegressionCurve > xCurve;
    ::rtl::OUString aServiceName;

    switch( eType )
    {
        case REGRESSION_TYPE_LINEAR:
            aServiceName = C2U( "com.sun.star.chart2.LinearRegressionCurve" );
            break;
        case REGRESSION_TYPE_LOG:
            aServiceName = C2U( "com.sun.star.chart2.LogarithmicRegressionCurve" );
            break;
        case REGRESSION_TYPE_EXP:
            aServiceName = C2U( "com.sun.star.chart2.ExponentialRegressionCurve" );
            break;
        case REGRESSION_TYPE_POWER:
            aServiceName = C2U( "com.sun.star.chart2.PotentialRegressionCurve" );
            break;
        default:
            OSL_ENSURE(false,"unknown regression curve type - use linear instead");
            aServiceName = C2U( "com.sun.star.chart2.LinearRegressionCurve" );
            break;
    }

    if( aServiceName.getLength())
    {
        // todo: use a valid context
        xCurve.set( createRegressionCurveByServiceName(
                         uno::Reference< uno::XComponentContext >(), aServiceName ));

        uno::Reference< beans::XPropertySet > xProp( xCurve, uno::UNO_QUERY );
        if( xProp.is())
        {
            if( xPropertySource.is())
                ::chart::PropertyHelper::copyProperties( xPropertySource, xProp );
            else
            {
                uno::Reference< XPropertySet > xSeriesProp( xRegCnt, uno::UNO_QUERY );
                if( xSeriesProp.is())
                {
                    xProp->setPropertyValue( C2U( "LineColor" ),
                                             xSeriesProp->getPropertyValue( C2U( "Color" )));
                }
                xProp->setPropertyValue( C2U( "LineWidth" ), uno::makeAny( sal_Int32( 100 )));
            }
        }
    }
    xRegCnt->addRegressionCurve( xCurve );
}

/** removes all regression curves that are not of type mean value
    and returns true, if anything was removed
 */
bool RegressionCurveHelper::removeAllExceptMeanValueLine(
    uno::Reference< chart2::XRegressionCurveContainer > & xRegCnt )
{
    bool bRemovedSomething = false;
    if( xRegCnt.is())
    {
        try
        {
            uno::Sequence< uno::Reference< chart2::XRegressionCurve > > aCurves(
                xRegCnt->getRegressionCurves());
            ::std::vector< uno::Reference< chart2::XRegressionCurve > > aCurvesToDelete;
            for( sal_Int32 i = 0; i < aCurves.getLength(); ++i )
            {
                if( ! isMeanValueLine( aCurves[i] ))
                {
                    aCurvesToDelete.push_back( aCurves[ i ] );
                }
            }

            for( ::std::vector< uno::Reference< chart2::XRegressionCurve > >::const_iterator aIt = aCurvesToDelete.begin();
                     aIt != aCurvesToDelete.end(); ++aIt )
            {
                xRegCnt->removeRegressionCurve( *aIt );
                bRemovedSomething = true;
            }
        }
        catch( uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }
    return bRemovedSomething;
}

// static
uno::Reference< chart2::XRegressionCurve > RegressionCurveHelper::getFirstCurveNotMeanValueLine(
    const Reference< XRegressionCurveContainer > & xRegCnt )
{
    if( !xRegCnt.is())
        return NULL;

    try
    {
        uno::Sequence< uno::Reference< chart2::XRegressionCurve > > aCurves(
            xRegCnt->getRegressionCurves());
        ::std::vector< uno::Reference< chart2::XRegressionCurve > > aCurvesToDelete;
        for( sal_Int32 i = 0; i < aCurves.getLength(); ++i )
        {
            if( ! isMeanValueLine( aCurves[i] ))
            {
                return aCurves[ i ];
            }
        }
    }
    catch( Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }

    return NULL;
}

// static
RegressionCurveHelper::tRegressionType RegressionCurveHelper::getFirstRegressTypeNotMeanValueLine(
    const Reference< XRegressionCurveContainer > & xRegCnt )
{
    tRegressionType eResult = REGRESSION_TYPE_NONE;

    if( xRegCnt.is())
    {
        try
        {
            Sequence< Reference< XRegressionCurve > > aCurves(
                xRegCnt->getRegressionCurves());
            for( sal_Int32 i = 0; i < aCurves.getLength(); ++i )
            {
                Reference< lang::XServiceName > xServName( aCurves[i], uno::UNO_QUERY );
                if( xServName.is())
                {
                    ::rtl::OUString aServiceName( xServName->getServiceName() );

                    // note: take first regression curve that matches any known
                    // type (except mean-value line)
                    if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                                "com.sun.star.chart2.LinearRegressionCurve" )))
                    {
                        eResult = REGRESSION_TYPE_LINEAR;
                        break;
                    }
                    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                                "com.sun.star.chart2.LogarithmicRegressionCurve" )))
                    {
                        eResult = REGRESSION_TYPE_LOG;
                        break;
                    }
                    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                                "com.sun.star.chart2.ExponentialRegressionCurve" )))
                    {
                        eResult = REGRESSION_TYPE_EXP;
                        break;
                    }
                    else if( aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                                "com.sun.star.chart2.PotentialRegressionCurve" )))
                    {
                        eResult = REGRESSION_TYPE_POWER;
                        break;
                    }
                }
            }
        }
        catch( Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    return eResult;
}

OUString RegressionCurveHelper::getUINameForRegressionCurve( const Reference< XRegressionCurve >& xRegressionCurve )
{
    OUString aResult;
    Reference< lang::XServiceName > xServiceName( xRegressionCurve, uno::UNO_QUERY );
    if( ! xServiceName.is())
        return aResult;

    OUString aServiceName( xServiceName->getServiceName());
    if( aServiceName.equalsAsciiL(
            RTL_CONSTASCII_STRINGPARAM( "com.sun.star.chart2.MeanValueRegressionCurve" )))
    {
        OSL_ENSURE( false, "Meanvalue lines in legend not supported" );
        aResult = OUString();
        // aResult = ::chart::SchResId::getResString( STR_OBJECT_AVERAGE_LINE );
    }
    else if( aServiceName.equalsAsciiL(
                 RTL_CONSTASCII_STRINGPARAM( "com.sun.star.chart2.LinearRegressionCurve" )))
    {
        aResult = ::chart::SchResId::getResString( STR_REGRESSION_LINEAR );
    }
    else if( aServiceName.equalsAsciiL(
                 RTL_CONSTASCII_STRINGPARAM( "com.sun.star.chart2.LogarithmicRegressionCurve" )))
    {
        aResult = ::chart::SchResId::getResString( STR_REGRESSION_LOG );
    }
    else if( aServiceName.equalsAsciiL(
                 RTL_CONSTASCII_STRINGPARAM( "com.sun.star.chart2.ExponentialRegressionCurve" )))
    {
        aResult = ::chart::SchResId::getResString( STR_REGRESSION_EXP );
    }
    else if( aServiceName.equalsAsciiL(
                 RTL_CONSTASCII_STRINGPARAM( "com.sun.star.chart2.PotentialRegressionCurve" )))
    {
        aResult = ::chart::SchResId::getResString( STR_REGRESSION_POWER );
    }

    return aResult;
}

//.............................................................................
} //namespace chart
//.............................................................................
