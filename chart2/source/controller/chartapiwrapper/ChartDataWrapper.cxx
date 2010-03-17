/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "ChartDataWrapper.hxx"
#include "macros.hxx"
#include "DiagramHelper.hxx"
#include "DataSourceHelper.hxx"
#include "servicenames_charttypes.hxx"
#include "ContainerHelper.hxx"
#include "CommonFunctors.hxx"
#include "ChartModelHelper.hxx"
#include "DataSeriesHelper.hxx"
#include "ControllerLockGuard.hxx"
#include "Chart2ModelContact.hxx"
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/chart2/XTitled.hpp>
#include <com/sun/star/chart2/data/XNumericalDataSequence.hpp>
#include <com/sun/star/chart2/data/XTextualDataSequence.hpp>
#include <com/sun/star/chart2/data/XDataSource.hpp>
#include <com/sun/star/chart2/XDataSeries.hpp>
#include <com/sun/star/chart2/XDataSeriesContainer.hpp>
#include <com/sun/star/chart2/XCoordinateSystemContainer.hpp>
#include <com/sun/star/chart2/XChartTypeContainer.hpp>
#include <com/sun/star/chart2/data/XDataReceiver.hpp>
#include <com/sun/star/chart/ChartDataRowSource.hpp>
#include <com/sun/star/chart/XChartDocument.hpp>

#include "CharacterProperties.hxx"
#include "LineProperties.hxx"
#include "FillProperties.hxx"

#include <map>
#include <algorithm>
#include <rtl/math.hxx>

using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;
using ::osl::MutexGuard;
using ::com::sun::star::chart::XComplexDescriptionAccess;
using ::com::sun::star::chart::XChartData;
using ::com::sun::star::chart::XChartDataArray;

namespace
{
static const ::rtl::OUString lcl_aServiceName(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.chart.ChartData" ));

uno::Sequence< uno::Sequence< double > > lcl_getNANInsteadDBL_MIN( const uno::Sequence< uno::Sequence< double > >& rData )
{
    uno::Sequence< uno::Sequence< double > > aRet;
    const sal_Int32 nOuterSize = rData.getLength();
    aRet.realloc( nOuterSize );
    for( sal_Int32 nOuter=0; nOuter<nOuterSize; ++nOuter )
    {
        sal_Int32 nInnerSize = rData[nOuter].getLength();
        aRet[nOuter].realloc( nInnerSize );
        for( sal_Int32 nInner=0; nInner<nInnerSize; ++nInner )
        {
            aRet[nOuter][nInner] = rData[nOuter][nInner];
            double& rValue = aRet[nOuter][nInner];
            if( rValue == DBL_MIN )
                ::rtl::math::setNan( &rValue );
        }
    }
    return aRet;
}

uno::Sequence< uno::Sequence< double > > lcl_getDBL_MINInsteadNAN( const uno::Sequence< uno::Sequence< double > >& rData )
{
    uno::Sequence< uno::Sequence< double > > aRet;
    const sal_Int32 nOuterSize = rData.getLength();
    aRet.realloc( nOuterSize );
    for( sal_Int32 nOuter=0; nOuter<nOuterSize; ++nOuter )
    {
        sal_Int32 nInnerSize = rData[nOuter].getLength();
        aRet[nOuter].realloc( nInnerSize );
        for( sal_Int32 nInner=0; nInner<nInnerSize; ++nInner )
        {
            aRet[nOuter][nInner] = rData[nOuter][nInner];
            double& rValue = aRet[nOuter][nInner];
            if( ::rtl::math::isNan( rValue ) )
                rValue = DBL_MIN;
        }
    }
    return aRet;
}

} // anonymous namespace

// --------------------------------------------------------------------------------

namespace chart
{
namespace wrapper
{

//--------------------------------------------------------------------------------------

struct lcl_Operator
{
    lcl_Operator()
    {
    }
    virtual ~lcl_Operator()
    {
    }
    virtual void apply( const Reference< XComplexDescriptionAccess >& xDataAccess ) = 0;

    virtual bool setsCategories( bool /*bDataInColumns*/ )
    {
        return false;
    }
};

//--------------------------------------------------------------------------

struct lcl_AllOperator : public lcl_Operator
{
    lcl_AllOperator( const Reference< XChartData >& xDataToApply )
        : lcl_Operator()
        , m_xDataToApply( xDataToApply )
    {
    }

    virtual bool setsCategories( bool /*bDataInColumns*/ )
    {
        return true;
    }

    virtual void apply( const Reference< XComplexDescriptionAccess >& xDataAccess )
    {
        if( !xDataAccess.is() )
            return;

        Reference< XComplexDescriptionAccess > xNewComplex( m_xDataToApply, uno::UNO_QUERY );
        if( xNewComplex.is() )
        {
            xDataAccess->setData( xNewComplex->getData() );
            xDataAccess->setComplexRowDescriptions( xNewComplex->getComplexRowDescriptions() );
            xDataAccess->setComplexColumnDescriptions( xNewComplex->getComplexColumnDescriptions() );
        }
        else
        {
            Reference< XChartDataArray > xNew( m_xDataToApply, uno::UNO_QUERY );
            if( xNew.is() )
            {
                xDataAccess->setData( xNew->getData() );
                xDataAccess->setRowDescriptions( xNew->getRowDescriptions() );
                xDataAccess->setColumnDescriptions( xNew->getColumnDescriptions() );
            }
        }
    }

    Reference< XChartData > m_xDataToApply;
};

//--------------------------------------------------------------------------

struct lcl_DataOperator : public lcl_Operator
{
    lcl_DataOperator( const Sequence< Sequence< double > >& rData )
        : lcl_Operator()
        , m_rData( rData )
    {
    }

    virtual void apply( const Reference< XComplexDescriptionAccess >& xDataAccess )
    {
        if( xDataAccess.is() )
            xDataAccess->setData( lcl_getNANInsteadDBL_MIN( m_rData ) );
    }

    const Sequence< Sequence< double > >& m_rData;
};

//--------------------------------------------------------------------------

struct lcl_RowDescriptionsOperator : public lcl_Operator
{
    lcl_RowDescriptionsOperator( const Sequence< OUString >& rRowDescriptions )
        : lcl_Operator()
        , m_rRowDescriptions( rRowDescriptions )
    {
    }

    virtual bool setsCategories( bool bDataInColumns )
    {
        return bDataInColumns;
    }

    virtual void apply( const Reference< XComplexDescriptionAccess >& xDataAccess )
    {
        if( xDataAccess.is() )
            xDataAccess->setRowDescriptions( m_rRowDescriptions );
    }

    const Sequence< OUString >& m_rRowDescriptions;
};

//--------------------------------------------------------------------------

struct lcl_ComplexRowDescriptionsOperator : public lcl_Operator
{
    lcl_ComplexRowDescriptionsOperator( const Sequence< Sequence< OUString > >& rComplexRowDescriptions )
        : lcl_Operator()
        , m_rComplexRowDescriptions( rComplexRowDescriptions )
    {
    }

    virtual bool setsCategories( bool bDataInColumns )
    {
        return !bDataInColumns;
    }

    virtual void apply( const Reference< XComplexDescriptionAccess >& xDataAccess )
    {
        if( xDataAccess.is() )
            xDataAccess->setComplexRowDescriptions( m_rComplexRowDescriptions );
    }

    const Sequence< Sequence< OUString > >& m_rComplexRowDescriptions;
};

//--------------------------------------------------------------------------

struct lcl_ColumnDescriptionsOperator : public lcl_Operator
{
    lcl_ColumnDescriptionsOperator( const Sequence< OUString >& rColumnDescriptions )
        : lcl_Operator()
        , m_rColumnDescriptions( rColumnDescriptions )
    {
    }

    virtual bool setsCategories( bool bDataInColumns )
    {
        return !bDataInColumns;
    }

    virtual void apply( const Reference< XComplexDescriptionAccess >& xDataAccess )
    {
        if( xDataAccess.is() )
            xDataAccess->setColumnDescriptions( m_rColumnDescriptions );
    }

    const Sequence< OUString >& m_rColumnDescriptions;
};

//--------------------------------------------------------------------------

struct lcl_ComplexColumnDescriptionsOperator : public lcl_Operator
{
    lcl_ComplexColumnDescriptionsOperator( const Sequence< Sequence< OUString > >& rComplexColumnDescriptions )
        : lcl_Operator()
        , m_rComplexColumnDescriptions( rComplexColumnDescriptions )
    {
    }

    virtual bool setsCategories( bool bDataInColumns )
    {
        return !bDataInColumns;
    }

    virtual void apply( const Reference< XComplexDescriptionAccess >& xDataAccess )
    {
        if( xDataAccess.is() )
            xDataAccess->setComplexColumnDescriptions( m_rComplexColumnDescriptions );
    }

    const Sequence< Sequence< OUString > >& m_rComplexColumnDescriptions;
};

//--------------------------------------------------------------------------

ChartDataWrapper::ChartDataWrapper( ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact ) :
        m_spChart2ModelContact( spChart2ModelContact ),
        m_aEventListenerContainer( m_aMutex )
{
    initDataAccess();
}

ChartDataWrapper::ChartDataWrapper( ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact,
                                    const Reference< XChartData >& xNewData ) :
        m_spChart2ModelContact( spChart2ModelContact ),
        m_aEventListenerContainer( m_aMutex )
{
    lcl_AllOperator aOperator( xNewData );
    applyData( aOperator );
}

ChartDataWrapper::~ChartDataWrapper()
{
    // @todo: implement XComponent and call this in dispose().  In the DTOR the
    // ref-count is 0, thus creating a stack reference to this calls the DTOR at
    // the end of the block recursively
//     uno::Reference< uno::XInterface > xSource( static_cast< ::cppu::OWeakObject* >( this ) );
//     m_aEventListenerContainer.disposeAndClear( lang::EventObject( xSource ) );
}

// ____ XChartDataArray (read)____
Sequence< Sequence< double > > SAL_CALL ChartDataWrapper::getData()
    throw (uno::RuntimeException)
{
    initDataAccess();
    if( m_xDataAccess.is() )
        return lcl_getDBL_MINInsteadNAN( m_xDataAccess->getData() );
    return Sequence< Sequence< double > >();
}
Sequence< OUString > SAL_CALL ChartDataWrapper::getRowDescriptions()
    throw (uno::RuntimeException)
{
    initDataAccess();
    if( m_xDataAccess.is() )
        return m_xDataAccess->getRowDescriptions();
    return Sequence< OUString >();
}
Sequence< OUString > SAL_CALL ChartDataWrapper::getColumnDescriptions()
    throw (uno::RuntimeException)
{
    initDataAccess();
    if( m_xDataAccess.is() )
        return m_xDataAccess->getColumnDescriptions();
    return Sequence< OUString > ();
}
// ____ XComplexDescriptionAccess (read) ____
Sequence< Sequence< OUString > > SAL_CALL ChartDataWrapper::getComplexRowDescriptions() throw (uno::RuntimeException)
{
    initDataAccess();
    if( m_xDataAccess.is() )
        return m_xDataAccess->getComplexRowDescriptions();
    return Sequence< Sequence< OUString > >();
}
Sequence< Sequence< OUString > > SAL_CALL ChartDataWrapper::getComplexColumnDescriptions() throw (uno::RuntimeException)
{
    initDataAccess();
    if( m_xDataAccess.is() )
        return m_xDataAccess->getComplexColumnDescriptions();
    return Sequence< Sequence< OUString > >();
}

// ____ XChartDataArray (write)____
void SAL_CALL ChartDataWrapper::setData( const Sequence< Sequence< double > >& rData )
    throw (uno::RuntimeException)
{
    lcl_DataOperator aOperator( rData );
    applyData( aOperator );
}
void SAL_CALL ChartDataWrapper::setRowDescriptions( const Sequence< OUString >& rRowDescriptions )
    throw (uno::RuntimeException)
{
    lcl_RowDescriptionsOperator aOperator( rRowDescriptions );
    applyData( aOperator );
}
void SAL_CALL ChartDataWrapper::setColumnDescriptions( const Sequence< OUString >& rColumnDescriptions )
    throw (uno::RuntimeException)
{
    lcl_ColumnDescriptionsOperator aOperator( rColumnDescriptions );
    applyData( aOperator );
}

// ____ XComplexDescriptionAccess (write) ____
void SAL_CALL ChartDataWrapper::setComplexRowDescriptions( const Sequence< Sequence< ::rtl::OUString > >& rRowDescriptions ) throw (uno::RuntimeException)
{
    lcl_ComplexRowDescriptionsOperator aOperator( rRowDescriptions );
    applyData( aOperator );
}
void SAL_CALL ChartDataWrapper::setComplexColumnDescriptions( const Sequence< Sequence< ::rtl::OUString > >& rColumnDescriptions ) throw (uno::RuntimeException)
{
    lcl_ComplexColumnDescriptionsOperator aOperator( rColumnDescriptions );
    applyData( aOperator );
}

//--------------------------------------------------------------------------------------

// ____ XChartData (base of XChartDataArray) ____
void SAL_CALL ChartDataWrapper::addChartDataChangeEventListener(
    const uno::Reference<
        ::com::sun::star::chart::XChartDataChangeEventListener >& aListener )
    throw (uno::RuntimeException)
{
    m_aEventListenerContainer.addInterface( aListener );
}

void SAL_CALL ChartDataWrapper::removeChartDataChangeEventListener(
    const uno::Reference<
        ::com::sun::star::chart::XChartDataChangeEventListener >& aListener )
    throw (uno::RuntimeException)
{
    m_aEventListenerContainer.removeInterface( aListener );
}

double SAL_CALL ChartDataWrapper::getNotANumber()
    throw (uno::RuntimeException)
{
    return DBL_MIN;
}

sal_Bool SAL_CALL ChartDataWrapper::isNotANumber( double nNumber )
    throw (uno::RuntimeException)
{
    return DBL_MIN == nNumber
        || ::rtl::math::isNan( nNumber )
        || ::rtl::math::isInf( nNumber );
}

// ____ XComponent ____
void SAL_CALL ChartDataWrapper::dispose()
    throw (uno::RuntimeException)
{
    m_aEventListenerContainer.disposeAndClear( lang::EventObject( static_cast< ::cppu::OWeakObject* >( this )));
    m_xDataAccess=0;
}

void SAL_CALL ChartDataWrapper::addEventListener(
    const uno::Reference< lang::XEventListener > & xListener )
    throw (uno::RuntimeException)
{
    m_aEventListenerContainer.addInterface( xListener );
}

void SAL_CALL ChartDataWrapper::removeEventListener(
    const uno::Reference< lang::XEventListener >& aListener )
    throw (uno::RuntimeException)
{
    m_aEventListenerContainer.removeInterface( aListener );
}

// ____ XEventListener ____
void SAL_CALL ChartDataWrapper::disposing( const lang::EventObject& /* Source */ )
    throw (uno::RuntimeException)
{
}

//  ::com::sun::star::chart::ChartDataChangeEvent aEvent;
//  aEvent.Type = chart::ChartDataChangeType_ALL;
//  aEvent.StartColumn = 0;
//  aEvent.EndColumn = 0;
//  aEvent.StartRow = 0;
//  aEvent.EndRow = 0;
void ChartDataWrapper::fireChartDataChangeEvent(
    ::com::sun::star::chart::ChartDataChangeEvent& aEvent )
{
    if( ! m_aEventListenerContainer.getLength() )
        return;

    uno::Reference< uno::XInterface > xSrc( static_cast< cppu::OWeakObject* >( this ));
    OSL_ASSERT( xSrc.is());
    if( xSrc.is() )
        aEvent.Source = xSrc;

    ::cppu::OInterfaceIteratorHelper aIter( m_aEventListenerContainer );

    while( aIter.hasMoreElements() )
    {
        uno::Reference<
            ::com::sun::star::chart::XChartDataChangeEventListener > xListener(
                aIter.next(), uno::UNO_QUERY );
        xListener->chartDataChanged( aEvent );
    }
}

// --------------------------------------------------------------------------------

void ChartDataWrapper::switchToInternalDataProvider()
{
    //create an internal data provider that is connected to the model
    Reference< chart2::XChartDocument > xChartDoc( m_spChart2ModelContact->getChart2Document() );
    if( xChartDoc.is() )
        xChartDoc->createInternalDataProvider( true /*bCloneExistingData*/ );
    initDataAccess();
}

void ChartDataWrapper::initDataAccess()
{
    Reference< chart2::XChartDocument > xChartDoc( m_spChart2ModelContact->getChart2Document() );
    if( !xChartDoc.is() )
        return;
    if( xChartDoc->hasInternalDataProvider() )
        m_xDataAccess = Reference< XComplexDescriptionAccess >( xChartDoc->getDataProvider(), uno::UNO_QUERY_THROW );
    else
    {
        //create a separate "internal data provider" that is not connected to the model
        m_xDataAccess = Reference< XComplexDescriptionAccess >( ChartModelHelper::createInternalDataProvider(
            xChartDoc, false /*bConnectToModel*/ ), uno::UNO_QUERY_THROW );
    }
}

void ChartDataWrapper::applyData( lcl_Operator& rDataOperator )
{
    //bool bSetValues, bool bSetRowDescriptions, bool bSetColumnDescriptions
    Reference< chart2::XChartDocument > xChartDoc( m_spChart2ModelContact->getChart2Document() );
    if( !xChartDoc.is() )
        return;

    // remember some diagram properties to reset later
    sal_Bool bStacked = sal_False;
    sal_Bool bPercent = sal_False;
    sal_Bool bDeep = sal_False;
    uno::Reference< ::com::sun::star::chart::XChartDocument > xOldDoc( xChartDoc, uno::UNO_QUERY );
    OSL_ASSERT( xOldDoc.is());
    uno::Reference< beans::XPropertySet > xDiaProp( xOldDoc->getDiagram(), uno::UNO_QUERY );
    if( xDiaProp.is())
    {
        xDiaProp->getPropertyValue( C2U("Stacked")) >>= bStacked;
        xDiaProp->getPropertyValue( C2U("Percent")) >>= bPercent;
        xDiaProp->getPropertyValue( C2U("Deep")) >>= bDeep;
    }

    //detect arguments for the new data source
    ::rtl::OUString aRangeString;
    bool bUseColumns = true;
    bool bFirstCellAsLabel = true;
    bool bHasCategories = true;
    uno::Sequence< sal_Int32 > aSequenceMapping;

    DataSourceHelper::detectRangeSegmentation(
        uno::Reference< frame::XModel >( xChartDoc, uno::UNO_QUERY ),
        aRangeString, aSequenceMapping, bUseColumns, bFirstCellAsLabel, bHasCategories );

    if( !bHasCategories && rDataOperator.setsCategories( bUseColumns ) )
        bHasCategories = true;

    uno::Sequence< beans::PropertyValue > aArguments( DataSourceHelper::createArguments(
            aRangeString, aSequenceMapping, bUseColumns, bFirstCellAsLabel, bHasCategories ) );


    // /-- locked controllers
    ControllerLockGuard aCtrlLockGuard( uno::Reference< frame::XModel >( xChartDoc, uno::UNO_QUERY ));

    // create and attach new data source
    switchToInternalDataProvider();
    rDataOperator.apply(m_xDataAccess);
    uno::Reference< chart2::data::XDataProvider > xDataProvider( xChartDoc->getDataProvider() );
    OSL_ASSERT( xDataProvider.is() );
    if( !xDataProvider.is() )
        return;
    uno::Reference< chart2::data::XDataSource > xSource( xDataProvider->createDataSource( aArguments ) );

    // determine a template
    uno::Reference< lang::XMultiServiceFactory > xFact( xChartDoc->getChartTypeManager(), uno::UNO_QUERY );
    uno::Reference< chart2::XDiagram > xDia( xChartDoc->getFirstDiagram());
    DiagramHelper::tTemplateWithServiceName aTemplateAndService =
        DiagramHelper::getTemplateForDiagram( xDia, xFact );
    ::rtl::OUString aServiceName( aTemplateAndService.second );
    uno::Reference< chart2::XChartTypeTemplate > xTemplate = aTemplateAndService.first;

    // (fall-back)
    if( ! xTemplate.is())
    {
        if( aServiceName.getLength() == 0 )
            aServiceName = C2U("com.sun.star.chart2.template.Column");
        xTemplate.set( xFact->createInstance( aServiceName ), uno::UNO_QUERY );
    }
    OSL_ASSERT( xTemplate.is());

    if( xTemplate.is() && xSource.is())
    {
        // argument detection works with internal knowledge of the
        // ArrayDataProvider
        OSL_ASSERT( xDia.is());
        xTemplate->changeDiagramData( xDia, xSource, aArguments );
    }

    //correct stacking mode
    if( bStacked || bPercent || bDeep )
    {
        StackMode eStackMode = StackMode_Y_STACKED;
        if( bDeep )
            eStackMode = StackMode_Z_STACKED;
        else if( bPercent )
            eStackMode = StackMode_Y_STACKED_PERCENT;
        DiagramHelper::setStackMode( xDia, eStackMode );
    }

    // notify listeners
    ::com::sun::star::chart::ChartDataChangeEvent aEvent(
        static_cast< ::cppu::OWeakObject* >( this ),
        ::com::sun::star::chart::ChartDataChangeType_ALL, 0, 0, 0, 0 );
    fireChartDataChangeEvent( aEvent );
    // \-- locked controllers
}

// --------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > ChartDataWrapper::getSupportedServiceNames_Static()
{
    uno::Sequence< ::rtl::OUString > aServices( 2 );
    aServices[ 0 ] = C2U( "com.sun.star.chart.ChartDataArray" );
    aServices[ 1 ] = C2U( "com.sun.star.chart.ChartData" );

    return aServices;
}

// ================================================================================

// implement XServiceInfo methods basing upon getSupportedServiceNames_Static
APPHELPER_XSERVICEINFO_IMPL( ChartDataWrapper, lcl_aServiceName );

} //  namespace wrapper
} //  namespace chart
