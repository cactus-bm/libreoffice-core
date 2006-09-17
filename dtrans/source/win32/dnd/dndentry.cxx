/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dndentry.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 16:57:57 $
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
#include "precompiled_dtrans.hxx"

#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XSET_HPP_
#include <com/sun/star/container/XSet.hpp>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#include "source.hxx"
#include "target.hxx"

using namespace ::rtl                       ;
using namespace ::com::sun::star::uno       ;
using namespace ::com::sun::star::registry  ;
using namespace ::cppu                      ;
using namespace ::com::sun::star::lang;

rtl_StandardModuleCount g_moduleCount = MODULE_COUNT_INIT;

Reference< XInterface > SAL_CALL createDragSource( const Reference< XMultiServiceFactory >& rServiceManager )
{
    DragSource* pSource= new DragSource( rServiceManager );
    return Reference<XInterface>( static_cast<XInitialization*>(pSource), UNO_QUERY);
}

Reference< XInterface > SAL_CALL createDropTarget( const Reference< XMultiServiceFactory >& rServiceManager )
{
    DropTarget* pTarget= new DropTarget( rServiceManager );
    return Reference<XInterface>( static_cast<XInitialization*>(pTarget), UNO_QUERY);
}


extern "C"
{
sal_Bool SAL_CALL component_canUnload( TimeValue *pTime )
{
    return g_moduleCount.canUnload( &g_moduleCount , pTime );
}

//----------------------------------------------------------------------
// component_getImplementationEnvironment
//----------------------------------------------------------------------

void SAL_CALL component_getImplementationEnvironment(
    const sal_Char ** ppEnvTypeName, uno_Environment ** /*ppEnv*/ )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

//-------------------------------------------------------------------------
// component_writeInfo - to register a UNO-Service
// to register a UNO-Service use: regcomp -register -r *.rdb -c *.dll
// to view the registry use: regview *.rdb /SERVICES/ServiceName
// (you must use the full services name e.g. com.sun.star.frame.FilePicker
//-------------------------------------------------------------------------

sal_Bool SAL_CALL component_writeInfo( void* /*pServiceManager*/, void* pRegistryKey )
{
    sal_Bool bRetVal = sal_False;

    if ( pRegistryKey )
    {
        try
        {
            Reference< XRegistryKey > pXNewKey( static_cast< XRegistryKey* >( pRegistryKey ) );
            pXNewKey->createKey( OUString( RTL_CONSTASCII_USTRINGPARAM( DNDSOURCE_REGKEY_NAME ) ) );
            bRetVal = sal_True;

            pXNewKey=  static_cast< XRegistryKey* >( pRegistryKey );
            pXNewKey->createKey( OUString( RTL_CONSTASCII_USTRINGPARAM( DNDTARGET_REGKEY_NAME ) ) );
            bRetVal = sal_True;
        }
        catch( InvalidRegistryException& )
        {
            OSL_ENSURE(sal_False, "InvalidRegistryException caught");
            bRetVal = sal_False;
        }
    }

    return bRetVal;
}

//----------------------------------------------------------------------
// component_getFactory
// returns a factory to create XFilePicker-Services
//----------------------------------------------------------------------

void* SAL_CALL component_getFactory( const sal_Char* pImplName, uno_Interface* pSrvManager, uno_Interface* /*pRegistryKey*/ )
{
    void* pRet = 0;
    Reference< XSingleServiceFactory > xFactory;

    if ( pSrvManager && ( 0 == rtl_str_compare( pImplName, DNDSOURCE_IMPL_NAME ) ) )
    {
        Sequence< OUString > aSNS( 1 );
        aSNS.getArray( )[0] = OUString( RTL_CONSTASCII_USTRINGPARAM( DNDSOURCE_SERVICE_NAME ) );

        xFactory= createSingleFactory(
            reinterpret_cast< XMultiServiceFactory* > ( pSrvManager ),
            OUString::createFromAscii( pImplName ),
            createDragSource,
            aSNS,
            &g_moduleCount.modCnt);

    }
    else if( pSrvManager && ( 0 == rtl_str_compare( pImplName, DNDTARGET_IMPL_NAME ) ) )
    {
        Sequence< OUString > aSNS( 1 );
        aSNS.getArray( )[0] = OUString( RTL_CONSTASCII_USTRINGPARAM( DNDTARGET_SERVICE_NAME ) );

        xFactory= createSingleFactory(
            reinterpret_cast< XMultiServiceFactory* > ( pSrvManager ),
            OUString::createFromAscii( pImplName ),
            createDropTarget,
            aSNS);

    }

    if ( xFactory.is() )
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }

    return pRet;
}

} // extern "C"
