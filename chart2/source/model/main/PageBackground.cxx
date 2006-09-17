/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PageBackground.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 13:13:32 $
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
#include "PageBackground.hxx"
#include "macros.hxx"
#include "algohelper.hxx"
#include "LineProperties.hxx"
#include "FillProperties.hxx"
#include "UserDefinedProperties.hxx"

#ifndef CHART_PROPERTYHELPER_HXX
#include "PropertyHelper.hxx"
#endif

#ifndef _COM_SUN_STAR_DRAWING_FILLSTYLE_HPP_
#include <com/sun/star/drawing/FillStyle.hpp>
#endif

#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif
#ifndef _CPPUHELPER_QUERYINTERFACE_HXX_
#include <cppuhelper/queryinterface.hxx>
#endif

#include <vector>
#include <algorithm>

using namespace ::com::sun::star;

using ::com::sun::star::beans::Property;
using ::osl::MutexGuard;

namespace
{

static const ::rtl::OUString lcl_aServiceName(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.chart2.PageBackground" ));

void lcl_AddDefaultsToMap(
    ::chart::helper::tPropertyValueMap & rOutMap )
{
    rOutMap[ ::chart::FillProperties::PROP_FILL_STYLE ] =
        uno::makeAny( drawing::FillStyle_NONE );
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
         ::chart::LineProperties::AddPropertiesToVector(
             aProperties, /* bIncludeStyleProperties = */ true );
        ::chart::FillProperties::AddPropertiesToVector(
            aProperties, /* bIncludeStyleProperties = */ true );
        ::chart::UserDefinedProperties::AddPropertiesToVector( aProperties );

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

// ================================================================================

namespace chart
{

PageBackground::PageBackground( const uno::Reference< uno::XComponentContext > & xContext ) :
        ::property::OPropertySet( m_aMutex ),
    m_xContext( xContext )
{}

PageBackground::~PageBackground()
{}

// ================================================================================

// ____ OPropertySet ____
uno::Any PageBackground::GetDefaultValue( sal_Int32 nHandle ) const
    throw(beans::UnknownPropertyException)
{
    static helper::tPropertyValueMap aStaticDefaults;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aStaticDefaults.size() )
    {
        // initialize defaults
        LineProperties::AddDefaultsToMap(
             aStaticDefaults,
             /* bIncludeStyleProperties = */ true );
        FillProperties::AddDefaultsToMap(
            aStaticDefaults,
            /* bIncludeStyleProperties = */ true );
        // overrides defaults of FillProperties
        lcl_AddDefaultsToMap( aStaticDefaults );
    }

    helper::tPropertyValueMap::const_iterator aFound(
        aStaticDefaults.find( nHandle ));

    if( aFound == aStaticDefaults.end())
        return uno::Any();

    return (*aFound).second;
    // \--
}

::cppu::IPropertyArrayHelper & SAL_CALL PageBackground::getInfoHelper()
{
    return lcl_getInfoHelper();
}


// ____ XPropertySet ____
uno::Reference< beans::XPropertySetInfo > SAL_CALL
    PageBackground::getPropertySetInfo()
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

// ____ XInterface ____
uno::Any SAL_CALL PageBackground::queryInterface( const uno::Type& aType )
    throw (uno::RuntimeException)
{
    return ::cppu::OWeakObject::queryInterface( aType );
}

void SAL_CALL PageBackground::acquire() throw ()
{
    ::cppu::OWeakObject::acquire();
}

void SAL_CALL PageBackground::release() throw ()
{
    ::cppu::OWeakObject::release();
}

// ================================================================================

uno::Sequence< ::rtl::OUString > PageBackground::getSupportedServiceNames_Static()
{
    uno::Sequence< ::rtl::OUString > aServices( 2 );
    aServices[ 0 ] = C2U( "com.sun.star.chart2.PageBackground" );
    aServices[ 1 ] = C2U( "com.sun.star.beans.PropertySet" );
    return aServices;
}

// implement XServiceInfo methods basing upon getSupportedServiceNames_Static
APPHELPER_XSERVICEINFO_IMPL( PageBackground, lcl_aServiceName );

} //  namespace chart
