/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ErrorBar.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-25 08:57:00 $
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
#include "ErrorBar.hxx"
#include "macros.hxx"
#include "LineProperties.hxx"
#include "ContainerHelper.hxx"

#ifndef CHART_PROPERTYHELPER_HXX
#include "PropertyHelper.hxx"
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_ERRORBARSTYLE_HPP_
#include <com/sun/star/chart2/ErrorBarStyle.hpp>
#endif

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

using namespace ::com::sun::star;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::com::sun::star::beans::Property;
using ::osl::MutexGuard;

namespace
{
static const OUString lcl_aServiceName(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.chart2.ErrorBar" ));

enum
{
    PROP_ERROR_BAR_STYLE,
    PROP_ERROR_BAR_POS_ERROR,
    PROP_ERROR_BAR_NEG_ERROR,
    PROP_ERROR_BAR_WEIGHT,
    PROP_ERROR_BAR_SHOW_POS_ERROR,
    PROP_ERROR_BAR_SHOW_NEG_ERROR
};

void lcl_AddPropertiesToVector(
    ::std::vector< Property > & rOutProperties )
{
    rOutProperties.push_back(
        Property( C2U( "ErrorBarStyle" ),
                  PROP_ERROR_BAR_STYLE,
                  ::getCppuType( reinterpret_cast< const chart2::ErrorBarStyle * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    rOutProperties.push_back(
        Property( C2U( "PositiveError" ),
                  PROP_ERROR_BAR_POS_ERROR,
                  ::getCppuType( reinterpret_cast< const double * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    rOutProperties.push_back(
        Property( C2U( "NegativeError" ),
                  PROP_ERROR_BAR_NEG_ERROR,
                  ::getCppuType( reinterpret_cast< const double * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    rOutProperties.push_back(
        Property( C2U( "Weight" ),
                  PROP_ERROR_BAR_WEIGHT,
                  ::getCppuType( reinterpret_cast< const double * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    rOutProperties.push_back(
        Property( C2U( "ShowPositiveError" ),
                  PROP_ERROR_BAR_SHOW_POS_ERROR,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    rOutProperties.push_back(
        Property( C2U( "ShowNegativeError" ),
                  PROP_ERROR_BAR_SHOW_NEG_ERROR,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
}

void lcl_AddDefaultsToMap(
    ::chart::tPropertyValueMap & rOutMap )
{
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_ERROR_BAR_STYLE ));
    rOutMap[ PROP_ERROR_BAR_STYLE ] =
        uno::makeAny( chart2::ErrorBarStyle_NONE );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_ERROR_BAR_POS_ERROR ));
    rOutMap[ PROP_ERROR_BAR_POS_ERROR ] =
        uno::makeAny( 0.0 );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_ERROR_BAR_NEG_ERROR ));
    rOutMap[ PROP_ERROR_BAR_NEG_ERROR ] =
        uno::makeAny( 0.0 );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_ERROR_BAR_WEIGHT ));
    rOutMap[ PROP_ERROR_BAR_WEIGHT ] =
        uno::makeAny( 1.0 );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_ERROR_BAR_SHOW_POS_ERROR ));
    rOutMap[ PROP_ERROR_BAR_SHOW_POS_ERROR ] =
        uno::makeAny( sal_True );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_ERROR_BAR_SHOW_NEG_ERROR ));
    rOutMap[ PROP_ERROR_BAR_SHOW_NEG_ERROR ] =
        uno::makeAny( sal_True );
}

const uno::Sequence< Property > & lcl_GetPropertySequence()
{
    static uno::Sequence< Property > aPropSeq;

    // /--
    MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aPropSeq.getLength() )
    {
        // get properties
        ::std::vector< ::com::sun::star::beans::Property > aProperties;
        lcl_AddPropertiesToVector( aProperties );
        ::chart::LineProperties::AddPropertiesToVector( aProperties );

        // and sort them for access via bsearch
        ::std::sort( aProperties.begin(), aProperties.end(),
                     ::chart::PropertyNameLess() );

        // transfer result to static Sequence
        aPropSeq = ::chart::ContainerHelper::ContainerToSequence( aProperties );
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

ErrorBar::ErrorBar(
    uno::Reference< uno::XComponentContext > const & xContext ) :
        ::property::OPropertySet( m_aMutex ),
    m_xContext( xContext ),
    m_xModifyEventForwarder( new ModifyListenerHelper::ModifyEventForwarder( m_aMutex ))
{}

ErrorBar::ErrorBar( const ErrorBar & rOther ) :
        MutexContainer(),
        impl::ErrorBar_Base(),
        ::property::OPropertySet( rOther, m_aMutex ),
    m_xContext( rOther.m_xContext ),
    m_xModifyEventForwarder( new ModifyListenerHelper::ModifyEventForwarder( m_aMutex ))
{}

ErrorBar::~ErrorBar()
{}

uno::Reference< util::XCloneable > SAL_CALL ErrorBar::createClone()
    throw (uno::RuntimeException)
{
    return uno::Reference< util::XCloneable >( new ErrorBar( *this ));
}

// ================================================================================

// ____ OPropertySet ____
uno::Any ErrorBar::GetDefaultValue( sal_Int32 nHandle ) const
    throw(beans::UnknownPropertyException)
{
    static tPropertyValueMap aStaticDefaults;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aStaticDefaults.size() )
    {
        // initialize defaults
        lcl_AddDefaultsToMap( aStaticDefaults );
        LineProperties::AddDefaultsToMap( aStaticDefaults );
    }

    tPropertyValueMap::const_iterator aFound(
        aStaticDefaults.find( nHandle ));

    if( aFound == aStaticDefaults.end())
        return uno::Any();

    return (*aFound).second;
    // \--
}

::cppu::IPropertyArrayHelper & SAL_CALL ErrorBar::getInfoHelper()
{
    return lcl_getInfoHelper();
}


// ____ XPropertySet ____
uno::Reference< beans::XPropertySetInfo > SAL_CALL
    ErrorBar::getPropertySetInfo()
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

// ____ XModifyBroadcaster ____
void SAL_CALL ErrorBar::addModifyListener( const uno::Reference< util::XModifyListener >& aListener )
    throw (uno::RuntimeException)
{
    try
    {
        uno::Reference< util::XModifyBroadcaster > xBroadcaster( m_xModifyEventForwarder, uno::UNO_QUERY_THROW );
        xBroadcaster->addModifyListener( aListener );
    }
    catch( const uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

void SAL_CALL ErrorBar::removeModifyListener( const uno::Reference< util::XModifyListener >& aListener )
    throw (uno::RuntimeException)
{
    try
    {
        uno::Reference< util::XModifyBroadcaster > xBroadcaster( m_xModifyEventForwarder, uno::UNO_QUERY_THROW );
        xBroadcaster->removeModifyListener( aListener );
    }
    catch( const uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

// ____ XModifyListener ____
void SAL_CALL ErrorBar::modified( const lang::EventObject& aEvent )
    throw (uno::RuntimeException)
{
    m_xModifyEventForwarder->modified( aEvent );
}

// ____ XEventListener (base of XModifyListener) ____
void SAL_CALL ErrorBar::disposing( const lang::EventObject& /* Source */ )
    throw (uno::RuntimeException)
{
    // nothing
}

// ____ OPropertySet ____
void ErrorBar::firePropertyChangeEvent()
{
    fireModifyEvent();
}

void ErrorBar::fireModifyEvent()
{
    m_xModifyEventForwarder->modified( lang::EventObject( static_cast< uno::XWeak* >( this )));
}

// ================================================================================

uno::Sequence< ::rtl::OUString > ErrorBar::getSupportedServiceNames_Static()
{
    uno::Sequence< ::rtl::OUString > aServices( 2 );
    aServices[ 0 ] = lcl_aServiceName;
    aServices[ 1 ] = C2U( "com.sun.star.chart2.ErrorBar" );
    return aServices;
}

// implement XServiceInfo methods basing upon getSupportedServiceNames_Static
APPHELPER_XSERVICEINFO_IMPL( ErrorBar, lcl_aServiceName );

// needed by MSC compiler
using impl::ErrorBar_Base;

IMPLEMENT_FORWARD_XINTERFACE2( ErrorBar, ErrorBar_Base, OPropertySet )
IMPLEMENT_FORWARD_XTYPEPROVIDER2( ErrorBar, ErrorBar_Base, OPropertySet )

} //  namespace chart
