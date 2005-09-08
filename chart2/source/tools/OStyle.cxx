/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: OStyle.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 01:32:44 $
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
#include "OStyle.hxx"
#include "macros.hxx"

#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif
#ifndef _CPPUHELPER_QUERYINTERFACE_HXX_
#include <cppuhelper/queryinterface.hxx>
#endif
#include "com/sun/star/uno/RuntimeException.hpp"

using namespace ::com::sun::star;

using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::rtl::OUString;
using ::osl::MutexGuard;

namespace property
{

OStyle::OStyle( const Reference< container::XNameAccess > & xStyleFamily,
                ::osl::Mutex & _rMutex ) :
        OPropertySet( _rMutex ),
        m_xStyleFamily( xStyleFamily ),
        m_bUserDefined( false ),
        m_rMutex( _rMutex )
{}

OStyle::~OStyle()
{}

::osl::Mutex & OStyle::GetMutex()
{
    return m_rMutex;
}

// ____ XStyle ____
sal_Bool SAL_CALL OStyle::isUserDefined()
    throw (uno::RuntimeException)
{
    // /--
    MutexGuard aGuard( GetMutex() );
    return m_bUserDefined;
    // \--
}

sal_Bool SAL_CALL OStyle::isInUse()
    throw (uno::RuntimeException)
{
    // /--
    MutexGuard aGuard( GetMutex() );

    // aBoundLC is a member of cppuhelper::OPropertySetHelper
    // it is assumed that a style is in use whenever some component is
    // registered here as listener
    return ( aBoundLC.getContainedTypes().getLength() > 0 );
    // \--
}

OUString SAL_CALL OStyle::getParentStyle()
    throw (uno::RuntimeException)
{
    // /--
    MutexGuard aGuard( GetMutex() );
    return m_aParentStyleName;
    // \--
}

void SAL_CALL OStyle::setParentStyle( const OUString& aParentStyle )
    throw (container::NoSuchElementException,
           uno::RuntimeException)
{
    // /--
    MutexGuard aGuard( GetMutex() );
    m_aParentStyleName = aParentStyle;
    // \--
}

// ____ XNamed (base of XStyle) ____

OUString SAL_CALL OStyle::getName()
    throw (uno::RuntimeException)
{
    // /--
    MutexGuard aGuard( GetMutex() );
    return m_aName;
    // \--
}

void SAL_CALL OStyle::setName( const ::rtl::OUString& aName )
    throw (uno::RuntimeException)
{
    // /--
    MutexGuard aGuard( GetMutex() );
    OSL_ASSERT( m_xStyleFamily.is() );

    // note: RuntimeException is not very apropriate for this, but I have no
    // other choice
    if( m_xStyleFamily->hasByName( aName ))
    {
        OSL_ENSURE( false, "Style name already exists!" );
        throw uno::RuntimeException(
            C2U( "Style name already exists: " ) + aName,
            static_cast< style::XStyle * >( this ));
    }
    else
    {
        // ToDo: Change the name in the container (XStyleFamiliy)
        m_aName = aName;
    }
    // \--
}

// ____ XInterface ____
uno::Any SAL_CALL OStyle::queryInterface( const uno::Type& aType )
    throw (uno::RuntimeException)
{
    uno::Any aResult = OPropertySet::queryInterface( aType );

    if( ! aResult.hasValue())
    {
        return ::cppu::queryInterface(
            aType,
            static_cast< style::XStyle * >( this ));
    }

    return aResult;
}

// void SAL_CALL OStyle::acquire() throw ()
// {
//     OPropertySet::acquire();
// }

// void SAL_CALL OStyle::release() throw ()
// {
//     OPropertySet::release();
// }



// ____ XServiceInfo ____
// OUString SAL_CALL
//     OStyle::getImplementationName()
//     throw (uno::RuntimeException)
// {
//     return OUString( RTL_CONSTASCII_USTRINGPARAM( "property::OStyle" ));
// }

// sal_Bool SAL_CALL
//     OStyle::supportsService( const OUString& ServiceName )
//     throw (uno::RuntimeException)
// {
//     Sequence< OUString > aServices( getSupportedServiceNames() );

//     sal_Int32 nI = aServices.getLength() - 1;
//     for( ; nI >= 0; --nI )
//     {
//         if( aServices[ nI ].equals( ServiceName ))
//             return sal_True;
//     }
//     return sal_False;
// }

// Sequence< OUString > SAL_CALL
//     OStyle::getSupportedServiceNames()
//     throw (uno::RuntimeException)
// {
//     Sequence< OUString > aServiceNames( 2 );
//     // from base OPropertySet
//     aServiceNames[ 0 ] = OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.beans.PropertySet" ));
//     // new service
//     aServiceNames[ 1 ] = OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.style.Style" ));
//     return aServiceNames;
// }


// ____ XTypeProvider ____
// Sequence< uno::Type > SAL_CALL
//     OStyle::getTypes()
//     throw (uno::RuntimeException)
// {
//     Sequence< uno::Type > aResult( OPropertySet::getTypes() );
//     aResult.realloc( aResult.getLength() + 1 );
//     aResult[ aResult.getLength() - 1 ] =
//         ::getCppuType( reinterpret_cast< const Reference< style::XStyle > *>(0));

//     return aResult;
// }

// Sequence< sal_Int8 > SAL_CALL
//     OStyle::getImplementationId()
//     throw (uno::RuntimeException)
// {
//  static uno::Sequence< sal_Int8 > aId;
//  if( aId.getLength() == 0 )
//  {
//      aId.realloc( 16 );
//      rtl_createUuid( (sal_uInt8 *)aId.getArray(), 0, sal_True );
//  }
//  return aId;
// }


} //  namespace property
