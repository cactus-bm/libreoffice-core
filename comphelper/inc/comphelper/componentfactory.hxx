/*************************************************************************
 *
 *  $RCSfile: componentfactory.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-16 15:53:38 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _COMPHELPER_COMPONENTFACTORY_HXX
#define _COMPHELPER_COMPONENTFACTORY_HXX
#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include "comphelper/comphelperdllapi.h"
#endif

#include <com/sun/star/lang/XSingleServiceFactory.hpp>


/**
 * @Descr
 * Utilities to get an instance of a component if a ProcessServiceFactory
 * is not available like it is the case in "small tools" as the Setup.
 */

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif


#define LLCF_DOSTRING( x )      #x
#define LLCF_STRING( x )        LLCF_DOSTRING( x )
#ifdef UNX
// "libNAME603xy.so" (__DLLEXTENSION == "xy.so")
#define LLCF_LIBNAME( name )    "lib" name LLCF_STRING( SUPD ) __DLLEXTENSION
#else
// "NAME603xy.dll" (__DLLEXTENSION == "xy")
#define LLCF_LIBNAME( name )    name LLCF_STRING( SUPD ) __DLLEXTENSION ".dll"
#endif


namespace rtl {
    class OUString;
}
namespace com { namespace sun { namespace star {
    namespace uno {
        class XInterface;
    }
    namespace lang {
        class XSingleServiceFactory;
        class XMultiServiceFactory;
    }
    namespace registry {
        class XRegistryKey;
    }
}}}


namespace comphelper
{

/**
 * Get an instance of the component <code>rImplementationName</code> located
 * in library <code>rLibraryName</code>. The instance must then be queried
 * for the desired interface with a queryInterface call.
 * The library name must be constructed with the macro
 * <code>LLCF_LIBNAME( name )</code> if it is a library from the normal build
 * process which includes build number and platform name.
 *
 * @example:C++
 * <listing>

    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;
    Reference< whatever::XYourComponent > xComp;
    // library name, e.g. xyz603mi.dll or libxyz603.so
    ::rtl::OUString aLibName( RTL_CONSTASCII_USTRINGPARAM( LLCF_LIBNAME( "xyz" ) ) );
    ::rtl::OUString aImplName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.whatever.YourComponent" ) );
    Reference< Xinterface > xI = ::comphelper::getComponentInstance( aLibName, aImplName );
    if ( xI.is() )
    {
        Any x = xI->queryInterface( ::getCppuType((const Reference< whatever::XYourComponent >*)0) );
        x >>= xComp;
    }
    if ( !xComp.is() )
        // you're lost

 * </listing>
 */
COMPHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
    getComponentInstance(
        const ::rtl::OUString & rLibraryName,
        const ::rtl::OUString & rImplementationName
        );


::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory >
    loadLibComponentFactory(
        const ::rtl::OUString & rLibraryName,
        const ::rtl::OUString & rImplementationName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & xSF,
        const ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey > & xKey
        );


}   // namespace comphelper

#endif // _COMPHELPER_COMPONENTFACTORY_HXX
