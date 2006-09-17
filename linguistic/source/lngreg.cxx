/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: lngreg.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 03:54:25 $
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
#include "precompiled_linguistic.hxx"


#include <cppuhelper/factory.hxx>   // helper for factories
#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif

#include <com/sun/star/registry/XRegistryKey.hpp>

using namespace com::sun::star::lang;

using namespace com::sun::star::registry;

////////////////////////////////////////
// declaration of external RegEntry-functions defined by the service objects
//

extern sal_Bool SAL_CALL LngSvcMgr_writeInfo
(
    void * /*pServiceManager*/,
    XRegistryKey * pRegistryKey
);

extern sal_Bool SAL_CALL DicList_writeInfo
(
    void * /*pServiceManager*/, XRegistryKey * pRegistryKey
);

extern sal_Bool SAL_CALL LinguProps_writeInfo
(
    void * /*pServiceManager*/,
    XRegistryKey * pRegistryKey
);

extern sal_Bool SAL_CALL ConvDicList_writeInfo
(
    void * /*pServiceManager*/, XRegistryKey * pRegistryKey
);

extern void * SAL_CALL LngSvcMgr_getFactory
(
    const sal_Char * pImplName,
    XMultiServiceFactory * pServiceManager,
    void * /*pRegistryKey*/
);

extern void * SAL_CALL DicList_getFactory
(
    const sal_Char * pImplName,
    XMultiServiceFactory * pServiceManager,
    void *
);

void * SAL_CALL LinguProps_getFactory
(
    const sal_Char * pImplName,
    XMultiServiceFactory * pServiceManager,
    void *
);

extern void * SAL_CALL ConvDicList_getFactory
(
    const sal_Char * pImplName,
    XMultiServiceFactory * pServiceManager,
    void *
);

////////////////////////////////////////
// definition of the two functions that are used to provide the services
//

extern "C"
{

sal_Bool SAL_CALL component_writeInfo
(
    void * pServiceManager,
    XRegistryKey * pRegistryKey
)
{
    sal_Bool bRet = LngSvcMgr_writeInfo( pServiceManager, pRegistryKey );
    if(bRet)
        bRet = LinguProps_writeInfo( pServiceManager, pRegistryKey );
    if(bRet)
        bRet = DicList_writeInfo( pServiceManager, pRegistryKey );
    if(bRet)
        bRet = ConvDicList_writeInfo( pServiceManager, pRegistryKey );
    return bRet;
}

void SAL_CALL component_getImplementationEnvironment(
    const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

void * SAL_CALL component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    void * pRet =
        LngSvcMgr_getFactory(
            pImplName,
            reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
            pRegistryKey );

    if(!pRet)
        pRet = LinguProps_getFactory(
            pImplName,
            reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
            pRegistryKey );

    if(!pRet)
        pRet =  DicList_getFactory(
            pImplName,
            reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
            pRegistryKey );

    if(!pRet)
        pRet =  ConvDicList_getFactory(
            pImplName,
            reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
            pRegistryKey );

    return pRet;
}
}

///////////////////////////////////////////////////////////////////////////

