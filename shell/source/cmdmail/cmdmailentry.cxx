/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cmdmailentry.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 01:40:33 $
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
#include "precompiled_shell.hxx"

//-----------------------------------------------------------------------
//  includes of other projects
//-----------------------------------------------------------------------

#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XSET_HPP_
#include <com/sun/star/container/XSet.hpp>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _CMDMAILSUPPL_HXX_
#include "cmdmailsuppl.hxx"
#endif

//-----------------------------------------------------------------------
// namespace directives
//-----------------------------------------------------------------------

using namespace ::rtl                       ;
using namespace ::com::sun::star::uno       ;
using namespace ::com::sun::star::container ;
using namespace ::com::sun::star::lang      ;
using namespace ::com::sun::star::registry  ;
using namespace ::cppu                      ;
using com::sun::star::system::XSimpleMailClientSupplier;

//-----------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------

#define COMP_SERVICE_NAME  "com.sun.star.system.SimpleCommandMail"
#define COMP_IMPL_NAME     "com.sun.star.comp.system.SimpleCommandMail"
#define COMP_REGKEY_NAME   "/com.sun.star.comp.system.SimpleCommandMail/UNO/SERVICES/com.sun.star.system.SimpleCommandMail"

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------

namespace
{
    Reference< XInterface > SAL_CALL createInstance( const Reference< XComponentContext >& xContext )
    {
        return Reference< XInterface >( static_cast< XSimpleMailClientSupplier* >( new CmdMailSuppl( xContext ) ) );
    }
}

//-----------------------------------------------------------------------
// the 3 important functions which will be exported
//-----------------------------------------------------------------------

extern "C"
{

//----------------------------------------------------------------------
// component_getImplementationEnvironment
//----------------------------------------------------------------------

void SAL_CALL component_getImplementationEnvironment(
    const sal_Char ** ppEnvTypeName, uno_Environment ** /*ppEnv*/ )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------

sal_Bool SAL_CALL component_writeInfo( void* /*pServiceManager*/, void* pRegistryKey )
{
    if ( pRegistryKey )
    {
        try
        {
            Reference< XRegistryKey > pXNewKey( static_cast< XRegistryKey* >( pRegistryKey ) );
            pXNewKey->createKey( OUString( RTL_CONSTASCII_USTRINGPARAM( COMP_REGKEY_NAME ) ) );
            return sal_True;
        }
        catch( InvalidRegistryException& )
        {
            OSL_ENSURE(sal_False, "InvalidRegistryException caught");
        }
    }

    return sal_False;
}

//----------------------------------------------------------------------
// component_getFactory
//----------------------------------------------------------------------

void* SAL_CALL component_getFactory( const sal_Char* pImplName, uno_Interface* /*pSrvManager*/, uno_Interface* /*pRegistryKey*/ )
{
    Reference< XSingleComponentFactory > xFactory;

    if (0 == ::rtl_str_compare( pImplName, COMP_IMPL_NAME ))
    {
        OUString serviceName( RTL_CONSTASCII_USTRINGPARAM(COMP_SERVICE_NAME) );

        xFactory = ::cppu::createSingleComponentFactory(
            createInstance,
            OUString( RTL_CONSTASCII_USTRINGPARAM(COMP_IMPL_NAME) ),
            Sequence< OUString >( &serviceName, 1 ) );
    }

    if (xFactory.is())
        xFactory->acquire();

    return xFactory.get();
}

} // extern "C"
