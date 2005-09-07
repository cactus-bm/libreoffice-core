/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: DataSeriesStyle.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 00:57:33 $
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
#include "DataSeriesStyle.hxx"
#include "DataSeriesProperties.hxx"
#include "CharacterProperties.hxx"
#include "PropertyHelper.hxx"
#include "macros.hxx"
#include "algohelper.hxx"

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_XSTYLE_HPP_
#include <com/sun/star/style/XStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#include <algorithm>

using namespace ::com::sun::star;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::beans::Property;
using ::osl::MutexGuard;

// ____________________________________________________________

namespace
{
const uno::Sequence< Property > & lcl_GetPropertySequence()
{
    static uno::Sequence< Property > aPropSeq;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aPropSeq.getLength() )
    {
        // get properties
        ::std::vector< ::com::sun::star::beans::Property > aProperties;
        ::chart::DataSeriesProperties::AddPropertiesToVector(
            aProperties, /* bIncludeStyleProperties = */ false );
        ::chart::CharacterProperties::AddPropertiesToVector(
            aProperties, /* bIncludeStyleProperties = */ false );

        // and sort them for access via bsearch
        ::std::sort( aProperties.begin(), aProperties.end(),
                     ::chart::helper::PropertyNameLess() );

        // transfer result to static Sequence
        aPropSeq = ::chart::helper::VectorToSequence( aProperties );
    }

    return aPropSeq;
}
} // anonymous namespace

// ____________________________________________________________

namespace chart
{

DataSeriesStyle::DataSeriesStyle(
    const uno::Reference< container::XNameAccess > & xStyleFamily,
    ::osl::Mutex & _rMutex ) :
        ::property::OStyle( xStyleFamily, _rMutex ),
    m_rMutex( _rMutex )
{}

DataSeriesStyle::~DataSeriesStyle()
{}

// ____ XInterface ____
uno::Any SAL_CALL DataSeriesStyle::queryInterface(
    const uno::Type & rType )
    throw (uno::RuntimeException)
{
    return ::cppu::OWeakObject::queryInterface( rType );
}

void SAL_CALL DataSeriesStyle::acquire() throw ()
{
    ::cppu::OWeakObject::acquire();
}

void SAL_CALL DataSeriesStyle::release() throw ()
{
    ::cppu::OWeakObject::release();
}

// ____ OPropertySet ____
uno::Any DataSeriesStyle::GetDefaultValue( sal_Int32 nHandle ) const
    throw(beans::UnknownPropertyException)
{
    static helper::tPropertyValueMap aStaticDefaults;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( 0 == aStaticDefaults.size() )
    {
        // initialize defaults
        DataSeriesProperties::AddDefaultsToMap(
            aStaticDefaults,
            /* bIncludeStyleProperties = */ false );
        CharacterProperties::AddDefaultsToMap(
            aStaticDefaults,
            /* bIncludeStyleProperties = */ false );
    }

    helper::tPropertyValueMap::const_iterator aFound(
        aStaticDefaults.find( nHandle ));

    if( aFound == aStaticDefaults.end())
        throw beans::UnknownPropertyException();

    return (*aFound).second;
    // \--
}

::cppu::IPropertyArrayHelper & SAL_CALL DataSeriesStyle::getInfoHelper()
{
    return getInfoHelperConst();
}

::cppu::IPropertyArrayHelper & SAL_CALL DataSeriesStyle::getInfoHelperConst() const
{
    static ::cppu::OPropertyArrayHelper aArrayHelper(
        lcl_GetPropertySequence(),
        /* bSorted = */ sal_True );

    return aArrayHelper;
}

// ____ XPropertySet ____
Reference< beans::XPropertySetInfo > SAL_CALL
    DataSeriesStyle::getPropertySetInfo()
    throw (uno::RuntimeException)
{
    static Reference< beans::XPropertySetInfo > xInfo;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    if( !xInfo.is())
    {
        xInfo = ::cppu::OPropertySetHelper::createPropertySetInfo(
            getInfoHelper());
    }

    return xInfo;
    // \--
}

::osl::Mutex & DataSeriesStyle::GetMutex()
{
    return m_rMutex;
}

} //  namespace chart
