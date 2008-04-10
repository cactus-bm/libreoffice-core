/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xserviceinfo.cxx,v $
 * $Revision: 1.3 $
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
// Sample implementation of the service XServiceInfo
// -------------------------------------------------
//
// CLASSNAME:  Replace this with your classes name
//
// m_aMutex:   This is the mutex member of your class
//

// --------------------

#include <vector>
#include <algorithm>

// --------------------

using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;
using ::osl::MutexGuard;
using ::com::sun::star::uno::RuntimeException;

// --------------------

// this should be a private method of your class
const ::std::vector< OUString > & CLASSNAME::GetServiceNames()
{
    static ::std::vector< OUString > aServices;

    // /--
    MutexGuard aGuard( m_aMutex );

    if( aServices.size() == 0 )
    {
        aServices.push_back( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sample.Service1" )));
        aServices.push_back( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sample.Service2" )));
        aServices.push_back( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sample.Service3" )));
        // ...
    }

    return aServices;
    // \--
}

// --------------------

// Alternative using double-checked-locking pattern
// (untested!)

// #ifndef INCLUDED_OSL_DOUBLECHECKEDLOCKING_H
// #include <osl/doublecheckedlocking.h>
// #endif

// this should be a private method of your class
// const ::std::vector< OUString > & CLASSNAME::GetServiceNames()
// {
//     static ::std::vector< OUString > aServices;
//     static bool bIsInitialized = false;

//     if( ! bIsInitialized )
//     {
//         // /--
//         MutexGuard aGuard( m_aMutex );

//         if( ! bIsInitialized )
//         {
//             aServices.push_back( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sample.Service1" )));
//             aServices.push_back( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sample.Service2" )));
//             aServices.push_back( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sample.Service3" )));
//             // ...
//             OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
//             bIsInitialized = true;
//         }
//         // \--
//     }
//     else
//         OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();

//     return aServices;
// }

// ------------------------------------
// ::com::sun::star::lang::XServiceInfo
// ------------------------------------

OUString SAL_CALL CLASSNAME::getImplementationName()
    throw (RuntimeException)
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.sample.CLASSNAME" ));
}

sal_Bool SAL_CALL CLASSNAME::supportsService( const ::rtl::OUString& ServiceName )
    throw (RuntimeException)
{
    const ::std::vector< OUString > & rServices = GetServiceNames();
    return ( rServices.end() != ::std::find( rServices.begin(), rServices.end(), ServiceName ) );
}

Sequence< OUString > SAL_CALL CLASSNAME::getSupportedServiceNames()
    throw (RuntimeException)
{
    const ::std::vector< OUString > & rServices = GetServiceNames();
    return Sequence< OUString >( &(* rServices.begin()), rServices.size() );
}
