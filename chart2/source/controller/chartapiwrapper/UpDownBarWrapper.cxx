/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: UpDownBarWrapper.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 17:19:45 $
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

#include "UpDownBarWrapper.hxx"
#include "macros.hxx"
#include "Chart2ModelContact.hxx"
#include "DiagramHelper.hxx"
#include "servicenames_charttypes.hxx"
#include "ContainerHelper.hxx"

#ifndef _COM_SUN_STAR_CHART2_XCHARTTYPE_HPP_
#include <com/sun/star/chart2/XChartType.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_XDATASERIESCONTAINER_HPP_
#include <com/sun/star/chart2/XDataSeriesContainer.hpp>
#endif

#include "LineProperties.hxx"
#include "FillProperties.hxx"
// #include "NamedProperties.hxx"
#include "UserDefinedProperties.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;

using ::com::sun::star::beans::Property;
using ::osl::MutexGuard;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::rtl::OUString;

namespace
{
static const OUString lcl_aServiceName(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.chart.ChartArea" ));

const Sequence< Property > & lcl_GetPropertySequence()
{
    static Sequence< Property > aPropSeq;

    // /--
    MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aPropSeq.getLength() )
    {
        // get properties
        ::std::vector< ::com::sun::star::beans::Property > aProperties;

        ::chart::LineProperties::AddPropertiesToVector( aProperties );
        ::chart::FillProperties::AddPropertiesToVector( aProperties );
//         ::chart::NamedProperties::AddPropertiesToVector( aProperties );
        ::chart::UserDefinedProperties::AddPropertiesToVector( aProperties );

        // and sort them for access via bsearch
        ::std::sort( aProperties.begin(), aProperties.end(),
                     ::chart::PropertyNameLess() );

        // transfer result to static Sequence
        aPropSeq = ::chart::ContainerHelper::ContainerToSequence( aProperties );
    }

    return aPropSeq;
}

} // anonymous namespace

// --------------------------------------------------------------------------------

namespace chart
{
namespace wrapper
{

UpDownBarWrapper::UpDownBarWrapper(
    bool bUp, ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact )
        : m_spChart2ModelContact( spChart2ModelContact )
        , m_aEventListenerContainer( m_aMutex )
        , m_aPropertySetName( bUp ? C2U("WhiteDay") : C2U("BlackDay") )
        , m_xInfo(0)
        , m_pPropertyArrayHelper()
//        , m_aWrappedFillGradientNameProperty( m_spChart2ModelContact )
//        , m_aWrappedFillHatchNameProperty( m_spChart2ModelContact )
//        , m_aWrappedFillBitmapNameProperty( m_spChart2ModelContact )
//        , m_aWrappedFillTransparenceGradientNameProperty( m_spChart2ModelContact )
//        , m_aWrappedLineDashNameProperty( m_spChart2ModelContact )
{
}

UpDownBarWrapper::~UpDownBarWrapper()
{
}

// ____ XComponent ____
void SAL_CALL UpDownBarWrapper::dispose()
    throw (uno::RuntimeException)
{
    Reference< uno::XInterface > xSource( static_cast< ::cppu::OWeakObject* >( this ) );
    m_aEventListenerContainer.disposeAndClear( lang::EventObject( xSource ) );

    // /--
    MutexGuard aGuard( GetMutex());
    m_xInfo.clear();
    // \--
}

void SAL_CALL UpDownBarWrapper::addEventListener(
    const Reference< lang::XEventListener >& xListener )
    throw (uno::RuntimeException)
{
    m_aEventListenerContainer.addInterface( xListener );
}

void SAL_CALL UpDownBarWrapper::removeEventListener(
    const Reference< lang::XEventListener >& aListener )
    throw (uno::RuntimeException)
{
    m_aEventListenerContainer.removeInterface( aListener );
}

::cppu::IPropertyArrayHelper& UpDownBarWrapper::getInfoHelper()
{
    if(!m_pPropertyArrayHelper.get())
    {
        // /--
        ::osl::MutexGuard aGuard( GetMutex() );
        if(!m_pPropertyArrayHelper.get())
        {
            sal_Bool bSorted = sal_True;
            m_pPropertyArrayHelper = ::boost::shared_ptr< ::cppu::OPropertyArrayHelper >( new ::cppu::OPropertyArrayHelper( lcl_GetPropertySequence(), bSorted ) );
        }
        // \--
    }
    return *m_pPropertyArrayHelper.get();
}

//XPropertySet
uno::Reference< beans::XPropertySetInfo > SAL_CALL UpDownBarWrapper::getPropertySetInfo()
                    throw (uno::RuntimeException)
{
    if( !m_xInfo.is() )
    {
        // /--
        ::osl::MutexGuard aGuard( GetMutex() );
        if( !m_xInfo.is() )
        {
            sal_Bool bSorted = sal_True;
            m_xInfo = ::cppu::OPropertySetHelper::createPropertySetInfo( getInfoHelper() );
        }
        // \--
    }
    return m_xInfo;
}
void SAL_CALL UpDownBarWrapper::setPropertyValue( const ::rtl::OUString& rPropertyName, const uno::Any& rValue )
                    throw (beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    Reference< beans::XPropertySet > xPropSet(0);

    Sequence< Reference< chart2::XChartType > > aTypes(
            ::chart::DiagramHelper::getChartTypesFromDiagram( m_spChart2ModelContact->getChart2Diagram() ) );
    for( sal_Int32 nN = 0; nN < aTypes.getLength(); nN++ )
    {
        Reference< chart2::XChartType > xType( aTypes[nN] );
        if( xType->getChartType().equals(CHART2_SERVICE_NAME_CHARTTYPE_CANDLESTICK) )
        {
            Reference< beans::XPropertySet > xTypeProps( aTypes[nN], uno::UNO_QUERY );
            if(xTypeProps.is())
            {
                xTypeProps->getPropertyValue( m_aPropertySetName ) >>= xPropSet;
            }
        }
    }
    if(xPropSet.is())
    {
//        if( rPropertyName.equals( m_aWrappedFillGradientNameProperty.getOuterName() ) )
//            m_aWrappedFillGradientNameProperty.setPropertyValue( rValue, xPropSet );
//        else if( rPropertyName.equals( m_aWrappedFillHatchNameProperty.getOuterName() ) )
//            m_aWrappedFillHatchNameProperty.setPropertyValue( rValue, xPropSet );
//        else if( rPropertyName.equals( m_aWrappedFillBitmapNameProperty.getOuterName() ) )
//            m_aWrappedFillBitmapNameProperty.setPropertyValue( rValue, xPropSet );
//        else if( rPropertyName.equals( m_aWrappedFillTransparenceGradientNameProperty.getOuterName() ) )
//            m_aWrappedFillTransparenceGradientNameProperty.setPropertyValue( rValue, xPropSet );
//        else if( rPropertyName.equals( m_aWrappedLineDashNameProperty.getOuterName() ) )
//            m_aWrappedLineDashNameProperty.setPropertyValue( rValue, xPropSet );
//        else
        xPropSet->setPropertyValue( rPropertyName, rValue );
    }
}
uno::Any SAL_CALL UpDownBarWrapper::getPropertyValue( const ::rtl::OUString& rPropertyName )
                    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    Any aRet;

    Reference< beans::XPropertySet > xPropSet(0);

    Sequence< Reference< chart2::XChartType > > aTypes(
            ::chart::DiagramHelper::getChartTypesFromDiagram( m_spChart2ModelContact->getChart2Diagram() ) );
    for( sal_Int32 nN = 0; nN < aTypes.getLength(); nN++ )
    {
        Reference< chart2::XChartType > xType( aTypes[nN] );
        if( xType->getChartType().equals(CHART2_SERVICE_NAME_CHARTTYPE_CANDLESTICK) )
        {
            Reference< beans::XPropertySet > xTypeProps( aTypes[nN], uno::UNO_QUERY );
            if(xTypeProps.is())
            {
                xTypeProps->getPropertyValue( m_aPropertySetName ) >>= xPropSet;
            }
        }
    }
    if(xPropSet.is())
    {
//        if( rPropertyName.equals( m_aWrappedFillGradientNameProperty.getOuterName() ) )
//            aRet = m_aWrappedFillGradientNameProperty.getPropertyValue( xPropSet );
//        else if( rPropertyName.equals( m_aWrappedFillHatchNameProperty.getOuterName() ) )
//            aRet = m_aWrappedFillHatchNameProperty.getPropertyValue( xPropSet );
//        else if( rPropertyName.equals( m_aWrappedFillBitmapNameProperty.getOuterName() ) )
//            aRet = m_aWrappedFillBitmapNameProperty.getPropertyValue( xPropSet );
//        else if( rPropertyName.equals( m_aWrappedFillTransparenceGradientNameProperty.getOuterName() ) )
//            aRet = m_aWrappedFillTransparenceGradientNameProperty.getPropertyValue( xPropSet );
//        else if( rPropertyName.equals( m_aWrappedLineDashNameProperty.getOuterName() ) )
//            aRet = m_aWrappedLineDashNameProperty.getPropertyValue( xPropSet );
//        else
        aRet = xPropSet->getPropertyValue( rPropertyName );
    }
    return aRet;
}

void SAL_CALL UpDownBarWrapper::addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& xListener )
                    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OSL_ENSURE(false,"not implemented");
}
void SAL_CALL UpDownBarWrapper::removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& aListener )
                    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OSL_ENSURE(false,"not implemented");
}
void SAL_CALL UpDownBarWrapper::addVetoableChangeListener( const ::rtl::OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener )
                    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OSL_ENSURE(false,"not implemented");
}
void SAL_CALL UpDownBarWrapper::removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener )
                    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OSL_ENSURE(false,"not implemented");
}

//XMultiPropertySet
//getPropertySetInfo() already declared in XPropertySet
void SAL_CALL UpDownBarWrapper::setPropertyValues( const uno::Sequence< ::rtl::OUString >& rNameSeq, const uno::Sequence< uno::Any >& rValueSeq )
                    throw (beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    bool bUnknownProperty = false;
    sal_Int32 nMinCount = std::min( rValueSeq.getLength(), rNameSeq.getLength() );
    for(sal_Int32 nN=0; nN<nMinCount; nN++)
    {
        ::rtl::OUString aPropertyName( rNameSeq[nN] );
        try
        {
            this->setPropertyValue( aPropertyName, rValueSeq[nN] );
        }
        catch( beans::UnknownPropertyException& ex )
        {
            ASSERT_EXCEPTION( ex );
            bUnknownProperty = true;
        }
    }
    //todo: store unknown properties elsewhere
//    if( bUnknownProperty )
//        throw beans::UnknownPropertyException();
}
uno::Sequence< uno::Any > SAL_CALL UpDownBarWrapper::getPropertyValues( const uno::Sequence< ::rtl::OUString >& rNameSeq )
                    throw (uno::RuntimeException)
{
    Sequence< Any > aRetSeq;
    if( rNameSeq.getLength() )
    {
        aRetSeq.realloc( rNameSeq.getLength() );
        for(sal_Int32 nN=0; nN<rNameSeq.getLength(); nN++)
        {
            ::rtl::OUString aPropertyName( rNameSeq[nN] );
            aRetSeq[nN] = this->getPropertyValue( aPropertyName );
        }
    }
    return aRetSeq;
}
void SAL_CALL UpDownBarWrapper::addPropertiesChangeListener( const uno::Sequence< ::rtl::OUString >& aPropertyNames, const uno::Reference< beans::XPropertiesChangeListener >& xListener )
                    throw (uno::RuntimeException)
{
    OSL_ENSURE(false,"not implemented");
}
void SAL_CALL UpDownBarWrapper::removePropertiesChangeListener( const uno::Reference< beans::XPropertiesChangeListener >& xListener )
                    throw (uno::RuntimeException)
{
    OSL_ENSURE(false,"not implemented");
}
void SAL_CALL UpDownBarWrapper::firePropertiesChangeEvent( const uno::Sequence< ::rtl::OUString >& aPropertyNames, const uno::Reference< beans::XPropertiesChangeListener >& xListener )
                    throw (uno::RuntimeException)
{
    OSL_ENSURE(false,"not implemented");
}

//XPropertyState
beans::PropertyState SAL_CALL UpDownBarWrapper::getPropertyState( const ::rtl::OUString& rPropertyName )
                    throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    uno::Any aDefault( this->getPropertyDefault( rPropertyName ) );
    uno::Any aValue( this->getPropertyValue( rPropertyName ) );

    if( aDefault == aValue )
        return beans::PropertyState_DEFAULT_VALUE;

    return beans::PropertyState_DIRECT_VALUE;
}
uno::Sequence< beans::PropertyState > SAL_CALL UpDownBarWrapper::getPropertyStates( const uno::Sequence< ::rtl::OUString >& rNameSeq )
                    throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    Sequence< beans::PropertyState > aRetSeq;
    if( rNameSeq.getLength() )
    {
        aRetSeq.realloc( rNameSeq.getLength() );
        for(sal_Int32 nN=0; nN<rNameSeq.getLength(); nN++)
        {
            ::rtl::OUString aPropertyName( rNameSeq[nN] );
            aRetSeq[nN] = this->getPropertyState( aPropertyName );
        }
    }
    return aRetSeq;
}
void SAL_CALL UpDownBarWrapper::setPropertyToDefault( const ::rtl::OUString& rPropertyName )
                    throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    this->setPropertyValue( rPropertyName, this->getPropertyDefault(rPropertyName) );
}
uno::Any SAL_CALL UpDownBarWrapper::getPropertyDefault( const ::rtl::OUString& rPropertyName )
                    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    static tPropertyValueMap aStaticDefaults;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aStaticDefaults.size() )
    {
        LineProperties::AddDefaultsToMap( aStaticDefaults );
        FillProperties::AddDefaultsToMap( aStaticDefaults );
    }

    sal_Int32 nHandle = getInfoHelper().getHandleByName( rPropertyName );

    tPropertyValueMap::const_iterator aFound(
        aStaticDefaults.find( nHandle ));

    if( aFound == aStaticDefaults.end())
        return uno::Any();

    return (*aFound).second;
}

//XMultiPropertyStates
//getPropertyStates() already declared in XPropertyState
void SAL_CALL UpDownBarWrapper::setAllPropertiesToDefault(  )
                    throw (uno::RuntimeException)
{
    const Sequence< beans::Property >&  rPropSeq = lcl_GetPropertySequence();
    for(sal_Int32 nN=0; nN<rPropSeq.getLength(); nN++)
    {
        ::rtl::OUString aPropertyName( rPropSeq[nN].Name );
        this->setPropertyToDefault( aPropertyName );
    }
}
void SAL_CALL UpDownBarWrapper::setPropertiesToDefault( const uno::Sequence< ::rtl::OUString >& rNameSeq )
                    throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    for(sal_Int32 nN=0; nN<rNameSeq.getLength(); nN++)
    {
        ::rtl::OUString aPropertyName( rNameSeq[nN] );
        this->setPropertyToDefault( aPropertyName );
    }
}
uno::Sequence< uno::Any > SAL_CALL UpDownBarWrapper::getPropertyDefaults( const uno::Sequence< ::rtl::OUString >& rNameSeq )
                    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    Sequence< Any > aRetSeq;
    if( rNameSeq.getLength() )
    {
        aRetSeq.realloc( rNameSeq.getLength() );
        for(sal_Int32 nN=0; nN<rNameSeq.getLength(); nN++)
        {
            ::rtl::OUString aPropertyName( rNameSeq[nN] );
            aRetSeq[nN] = this->getPropertyDefault( aPropertyName );
        }
    }
    return aRetSeq;
}


// ================================================================================

Sequence< OUString > UpDownBarWrapper::getSupportedServiceNames_Static()
{
    Sequence< OUString > aServices( 4 );
    aServices[ 0 ] = ::rtl::OUString::createFromAscii( "com.sun.star.chart.ChartArea" );
    aServices[ 1 ] = ::rtl::OUString::createFromAscii( "com.sun.star.drawing.LineProperties" );
    aServices[ 2 ] = ::rtl::OUString::createFromAscii( "com.sun.star.drawing.FillProperties" );
    aServices[ 3 ] = ::rtl::OUString::createFromAscii( "com.sun.star.xml.UserDefinedAttributeSupplier" );

    return aServices;
}

// implement XServiceInfo methods basing upon getSupportedServiceNames_Static
APPHELPER_XSERVICEINFO_IMPL( UpDownBarWrapper, lcl_aServiceName );

} //  namespace wrapper
} //  namespace chart
