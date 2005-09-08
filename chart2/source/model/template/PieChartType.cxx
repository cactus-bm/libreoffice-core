/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PieChartType.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 01:22:58 $
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
#include "PieChartType.hxx"
#include "PropertyHelper.hxx"
#include "algohelper.hxx"
#include "macros.hxx"

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

using namespace ::com::sun::star;

using ::rtl::OUString;
using ::com::sun::star::beans::Property;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::osl::MutexGuard;

namespace
{

enum
{
    PROP_PIECHARTTYPE_DIMENSION,
    PROP_PIECHARTTYPE_OFFSET
};

void lcl_AddPropertiesToVector(
    ::std::vector< Property > & rOutProperties )
{
    rOutProperties.push_back(
        Property( C2U( "Dimension" ),
                  PROP_PIECHARTTYPE_DIMENSION,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    rOutProperties.push_back(
        Property( C2U( "Offset" ),
                  PROP_PIECHARTTYPE_OFFSET,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
}

void lcl_AddDefaultsToMap(
    ::chart::helper::tPropertyValueMap & rOutMap )
{
    // must match default in CTOR!
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_PIECHARTTYPE_DIMENSION ));
    rOutMap[ PROP_PIECHARTTYPE_DIMENSION ] =
        uno::makeAny( sal_Int32( 2 ) );

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_PIECHARTTYPE_OFFSET ));
    rOutMap[ PROP_PIECHARTTYPE_OFFSET ] =
        uno::makeAny( sal_Int32( 0 ) );
}

const Sequence< Property > & lcl_GetPropertySequence()
{
    static Sequence< Property > aPropSeq;

    // /--
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
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

} // anonymous namespace

namespace chart
{

PieChartType::PieChartType(
    sal_Int32 nDim    /* = 2 */,
    sal_Int32 nOffset /* = 0 */ ) :
        ChartType( nDim )
{
    if( nDim != 2 )
        setFastPropertyValue_NoBroadcast( PROP_PIECHARTTYPE_DIMENSION, uno::makeAny( nDim ));
    if( nOffset != 0 )
        setFastPropertyValue_NoBroadcast( PROP_PIECHARTTYPE_OFFSET, uno::makeAny( nOffset ));
}

PieChartType::~PieChartType()
{}

// ____ XChartType ____
::rtl::OUString SAL_CALL PieChartType::getChartType()
    throw (uno::RuntimeException)
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart2.PieChart" ));
}


// ____ OPropertySet ____
uno::Any PieChartType::GetDefaultValue( sal_Int32 nHandle ) const
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

// ____ OPropertySet ____
::cppu::IPropertyArrayHelper & SAL_CALL PieChartType::getInfoHelper()
{
    static ::cppu::OPropertyArrayHelper aArrayHelper( lcl_GetPropertySequence(),
                                                      /* bSorted = */ sal_True );

    return aArrayHelper;
}


// ____ XPropertySet ____
uno::Reference< beans::XPropertySetInfo > SAL_CALL
    PieChartType::getPropertySetInfo()
    throw (uno::RuntimeException)
{
    static uno::Reference< beans::XPropertySetInfo > xInfo;

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

} //  namespace chart
