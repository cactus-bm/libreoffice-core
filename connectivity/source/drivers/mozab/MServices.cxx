/*************************************************************************
 *
 *  $RCSfile: MServices.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-21 12:23:26 $
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

#include "MDriver.hxx"
#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _COM_SUN_STAR_MOZILLA_XMOZILLABOOTSTRAP_HPP_
#include <com/sun/star/mozilla/XMozillaBootstrap.hpp>
#endif
#ifndef CONNECTIVITY_SMOZILLABOOTSTRAP_HXX
#include "bootstrap/MMozillaBootstrap.hxx"
#endif

using namespace connectivity::mozab;
using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::registry::XRegistryKey;
using ::com::sun::star::lang::XSingleServiceFactory;
using ::com::sun::star::lang::XMultiServiceFactory;
using ::com::sun::star::mozilla::XMozillaBootstrap;

typedef Reference< XSingleServiceFactory > (SAL_CALL *createFactoryFunc)
        (
            const Reference< XMultiServiceFactory > & rServiceManager,
            const OUString & rComponentName,
            ::cppu::ComponentInstantiation pCreateFunction,
            const Sequence< OUString > & rServiceNames,
            rtl_ModuleCount* _pTemp
        );

//***************************************************************************************
//
// Die vorgeschriebene C-Api muss erfuellt werden!
// Sie besteht aus drei Funktionen, die von dem Modul exportiert werden muessen.
//

//---------------------------------------------------------------------------------------
void REGISTER_PROVIDER(
        const OUString& aServiceImplName,
        const Sequence< OUString>& Services,
        const Reference< ::com::sun::star::registry::XRegistryKey > & xKey)
{
    OUString aMainKeyName;
    aMainKeyName = OUString( RTL_CONSTASCII_USTRINGPARAM("/"));
    aMainKeyName += aServiceImplName;
    aMainKeyName += OUString( RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));

    Reference< ::com::sun::star::registry::XRegistryKey >  xNewKey( xKey->createKey(aMainKeyName) );
    OSL_ENSURE(xNewKey.is(), "MOZAB::component_writeInfo : could not create a registry key !");

    for (sal_uInt32 i=0; i<Services.getLength(); ++i)
        xNewKey->createKey(Services[i]);
}


//---------------------------------------------------------------------------------------
struct ProviderRequest
{
    Reference< XSingleServiceFactory > xRet;
    Reference< XMultiServiceFactory > const xServiceManager;
    OUString const sImplementationName;

    ProviderRequest(
        void* pServiceManager,
        sal_Char const* pImplementationName
    )
    : xServiceManager(reinterpret_cast<XMultiServiceFactory*>(pServiceManager))
    , sImplementationName(OUString::createFromAscii(pImplementationName))
    {
    }

    inline
    sal_Bool CREATE_PROVIDER(
                const OUString& Implname,
                const Sequence< OUString > & Services,
                ::cppu::ComponentInstantiation Factory,
                createFactoryFunc creator
            )
    {
        if (!xRet.is() && (Implname == sImplementationName))
        try
        {
            xRet = creator( xServiceManager, sImplementationName,Factory, Services,0);
        }
        catch(...)
        {
        }
        return xRet.is();
    }

    void* getProvider() const { return xRet.get(); }
};

//---------------------------------------------------------------------------------------

extern "C" void SAL_CALL component_getImplementationEnvironment(
                const sal_Char  **ppEnvTypeName,
                uno_Environment **ppEnv
            )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

//---------------------------------------------------------------------------------------
extern "C" sal_Bool SAL_CALL component_writeInfo(
                void* pServiceManager,
                void* pRegistryKey
            )
{
    if (pRegistryKey)
    try
    {
        Reference< ::com::sun::star::registry::XRegistryKey > xKey(reinterpret_cast< ::com::sun::star::registry::XRegistryKey*>(pRegistryKey));


        REGISTER_PROVIDER(
            MozabDriver::getImplementationName_Static(),
            MozabDriver::getSupportedServiceNames_Static(), xKey);

        Sequence< ::rtl::OUString > aSNS( 1 );
        aSNS[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.mozilla.MozillaBootstrap"));
        REGISTER_PROVIDER(
             ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.mozilla.MozillaBootstrap")),
             aSNS, xKey);

        return sal_True;
    }
    catch (::com::sun::star::registry::InvalidRegistryException& )
    {
        OSL_ENSURE(sal_False, "Mozab::component_writeInfo : could not create a registry key ! ## InvalidRegistryException !");
    }

    return sal_False;
}
typedef void* (SAL_CALL * OMozillaBootstrap_CreateInstanceFunction)(const Reference< XMultiServiceFactory >& _rxFactory );
::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  SAL_CALL createMozillaBootstrap(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory) throw( ::com::sun::star::uno::Exception )
{
        const ::rtl::OUString sModuleName = ::rtl::OUString::createFromAscii(SAL_MODULENAME( "mozabdrv2" ));

        // load the dbtools library
        oslModule s_hModule = osl_loadModule(sModuleName.pData, 0);
        OSL_ENSURE(NULL != s_hModule, "MozabDriver::registerClient: could not load the dbtools library!");
        if (NULL != s_hModule)
        {

            // get the symbol for the method creating the factory
            const ::rtl::OUString sFactoryCreationFunc = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("OMozillaBootstrap_CreateInstance"));
            // reinterpret_cast<OMozabConnection_CreateInstanceFunction> removed GNU C
            OMozillaBootstrap_CreateInstanceFunction s_pCreationFunc = (OMozillaBootstrap_CreateInstanceFunction)(osl_getSymbol(s_hModule, sFactoryCreationFunc.pData));

            if (NULL == s_pCreationFunc)
            {   // did not find the symbol
                OSL_ENSURE(sal_False, "MozabDriver::registerClient: could not find the symbol for creating the factory!");
                osl_unloadModule(s_hModule);
                s_hModule = NULL;
            }
            MozillaBootstrap * pBootstrap = reinterpret_cast<MozillaBootstrap*>((*s_pCreationFunc)(_rxFactory));
            return *pBootstrap;
        }
        return NULL;
}
//---------------------------------------------------------------------------------------
extern "C" void* SAL_CALL component_getFactory(
                    const sal_Char* pImplementationName,
                    void* pServiceManager,
                    void* pRegistryKey)
{
    void* pRet = 0;
    if (pServiceManager)
    {
        OUString aImplName( OUString::createFromAscii( pImplementationName ) );
        ProviderRequest aReq(pServiceManager,pImplementationName);
        if (aImplName.equals(  MozabDriver::getImplementationName_Static()  ))
        {
            aReq.CREATE_PROVIDER(
                MozabDriver::getImplementationName_Static(),
                MozabDriver::getSupportedServiceNames_Static(),
                MozabDriver_CreateInstance, ::cppu::createSingleFactory);
        }
        else if (aImplName.equals(  ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.mozilla.MozillaBootstrap"))  ))
        {
            Sequence< ::rtl::OUString > aSNS( 1 );
            aSNS[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.mozilla.MozillaBootstrap"));
            aReq.CREATE_PROVIDER(
                aImplName,
                aSNS,
                createMozillaBootstrap, ::cppu::createSingleFactory);
        }
        if(aReq.xRet.is())
            aReq.xRet->acquire();
        pRet = aReq.getProvider();
    }

    return pRet;
};


