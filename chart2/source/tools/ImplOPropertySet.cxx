/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ImplOPropertySet.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 01:29:15 $
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
#include "ImplOPropertySet.hxx"

#include <algorithm>
#include <functional>

#ifndef _COM_SUN_STAR_BEANS_XFASTPROPERTYSET_HPP_
#include <com/sun/star/beans/XFastPropertySet.hpp>
#endif

using namespace ::com::sun::star;

namespace
{

struct lcl_getPropertyStateByHandle :
        public ::std::unary_function< beans::PropertyState, sal_Int32 >
{
    lcl_getPropertyStateByHandle(
        const ::property::impl::ImplOPropertySet::tPropertyMap & rMap )
            : m_rMap( rMap )
    {}

    inline beans::PropertyState operator() ( sal_Int32 nHandle )
    {
        if( m_rMap.end() == m_rMap.find( nHandle ))
            return beans::PropertyState_DEFAULT_VALUE;
        return beans::PropertyState_DIRECT_VALUE;
    }

private:
    const ::property::impl::ImplOPropertySet::tPropertyMap & m_rMap;
};

template< typename K, typename V >
struct lcl_eraseMapEntry :
        public ::std::unary_function< void, K >
{
    lcl_eraseMapEntry( ::std::map< K, V > & rMap )
            : m_rMap( rMap )
    {}

    inline void operator() ( const K & aKey )
    {
        m_rMap.erase( aKey );
    }

private:
    ::std::map< K, V > m_rMap;
};

} //  anonymous namespace

namespace property
{
namespace impl
{

ImplOPropertySet::ImplOPropertySet()
{}

beans::PropertyState ImplOPropertySet::GetPropertyStateByHandle( sal_Int32 nHandle ) const
{
    return lcl_getPropertyStateByHandle( m_aProperties ) ( nHandle );
}

uno::Sequence< beans::PropertyState > ImplOPropertySet::GetPropertyStatesByHandle(
    const ::std::vector< sal_Int32 > & aHandles ) const
{
    uno::Sequence< beans::PropertyState > aResult( aHandles.size());

    ::std::transform( aHandles.begin(), aHandles.end(),
                      aResult.getArray(),
                      lcl_getPropertyStateByHandle( m_aProperties ));

    return aResult;
}

void ImplOPropertySet::SetPropertyToDefault( sal_Int32 nHandle )
{
    tPropertyMap::iterator aFoundIter( m_aProperties.find( nHandle ) );

    if( m_aProperties.end() != aFoundIter )
    {
        m_aProperties.erase( aFoundIter );
    }
}

void ImplOPropertySet::SetPropertiesToDefault(
    const ::std::vector< sal_Int32 > & aHandles )
{
    ::std::for_each( aHandles.begin(), aHandles.end(),
                     lcl_eraseMapEntry< sal_Int32, uno::Any >( m_aProperties ) );
}

void ImplOPropertySet::SetAllPropertiesToDefault()
{
    m_aProperties.clear();
}

bool ImplOPropertySet::GetPropertyValueByHandle(
    uno::Any & rValue,
    sal_Int32 nHandle ) const
{
    bool bResult = false;

    tPropertyMap::const_iterator aFoundIter( m_aProperties.find( nHandle ) );

    if( m_aProperties.end() != aFoundIter )
    {
        rValue = (*aFoundIter).second;
        bResult = true;
    }

    return bResult;
}

void ImplOPropertySet::SetPropertyValueByHandle(
    sal_Int32 nHandle, const uno::Any & rValue, uno::Any * pOldValue )
{
    if( pOldValue != NULL )
    {
        tPropertyMap::const_iterator aFoundIter( m_aProperties.find( nHandle ) );
        if( m_aProperties.end() != aFoundIter )
            (*pOldValue) = (*aFoundIter).second;
    }

    m_aProperties[ nHandle ] = rValue;
}

bool ImplOPropertySet::SetStyle( const uno::Reference< style::XStyle > & xStyle )
{
    if( ! xStyle.is())
        return false;

    m_xStyle = xStyle;
    return true;
}

uno::Reference< style::XStyle > ImplOPropertySet::GetStyle() const
{
    return m_xStyle;
}


} //  namespace impl
} //  namespace chart
