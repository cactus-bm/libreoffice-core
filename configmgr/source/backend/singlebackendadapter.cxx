/*************************************************************************
 *
 *  $RCSfile: singlebackendadapter.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: jb $ $Date: 2002-09-02 17:24:39 $
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

#ifndef CONFIGMGR_BACKEND_SINGLEBACKENDADAPTER_HXX_
#include "singlebackendadapter.hxx"
#endif // CONFIGMGR_BACKEND_SINGLEBACKENDADAPTER_HXX_

#ifndef CONFIGMGR_API_FACTORY_HXX_
#include "confapifactory.hxx"
#endif // CONFIGMGR_API_FACTORY_HXX_

#ifndef CONFIGMGR_SERVICEINFOHELPER_HXX_
#include "serviceinfohelper.hxx"
#endif // CONFIGMGR_SERVICEINFOHELPER_HXX_

#ifndef _COM_SUN_STAR_CONFIGURATION_CANNOTLOADCONFIGURATIONEXCEPTION_HPP_
#include <com/sun/star/configuration/CannotLoadConfigurationException.hpp>
#endif

namespace configmgr { namespace backend {

//==============================================================================

SingleBackendAdapter::SingleBackendAdapter(
        const uno::Reference<lang::XMultiServiceFactory>& aFactory)
: BackendBase(mMutex), mFactory(aFactory) {
}
//------------------------------------------------------------------------------

SingleBackendAdapter::~SingleBackendAdapter(void) {}
//------------------------------------------------------------------------------

void SAL_CALL SingleBackendAdapter::initialize(
        const uno::Sequence<uno::Any>& aParameters)
    throw (uno::RuntimeException, uno::Exception) {

    uno::Any const * const pParams = aParameters.getConstArray();
    sal_Int32 nCount = aParameters.getLength();

    for (sal_Int32 ix = 0; ix < nCount; ++ix)
    {
        if (pParams[ix] >>= mBackend) break;
    }

    if (!mBackend.is())
    {
        throw com::sun::star::configuration::CannotLoadConfigurationException(
            OUString::createFromAscii("Online Backend Adapter: Cannot operate without real (Single)Backend"),
            *this);
    }
}
//------------------------------------------------------------------------------

uno::Reference<backenduno::XSchema>
SAL_CALL SingleBackendAdapter::getComponentSchema(
        const rtl::OUString& aComponent)
    throw (backenduno::BackendAccessException,
            lang::IllegalArgumentException,
            uno::RuntimeException)
{
    return mBackend->getSchema(aComponent) ;
}
//------------------------------------------------------------------------------

uno::Sequence<uno::Reference<backenduno::XLayer> >
SAL_CALL SingleBackendAdapter::listOwnLayers(const rtl::OUString& aComponent)
    throw (backenduno::BackendAccessException,
            lang::IllegalArgumentException,
            uno::RuntimeException)
{
    return listLayers(aComponent, mBackend->getOwnId()) ;
}
//------------------------------------------------------------------------------

uno::Reference<backenduno::XUpdateHandler>
SAL_CALL SingleBackendAdapter::getOwnUpdateHandler(
                                            const rtl::OUString& aComponent)
    throw (backenduno::BackendAccessException,
            lang::IllegalArgumentException,
            uno::RuntimeException)
{
    return getUpdateHandler(aComponent, mBackend->getOwnId()) ;
}
//------------------------------------------------------------------------------

uno::Sequence<uno::Reference<backenduno::XLayer> >
SAL_CALL SingleBackendAdapter::listLayers(const rtl::OUString& aComponent,
                                          const rtl::OUString& aEntity)
    throw (backenduno::BackendAccessException,
            lang::IllegalArgumentException,
            uno::RuntimeException)
{
    return mBackend->getLayers(mBackend->listLayerIds(aComponent, aEntity),
                               rtl::OUString()) ;
}
//------------------------------------------------------------------------------

uno::Reference<backenduno::XUpdateHandler>
SAL_CALL SingleBackendAdapter::getUpdateHandler(const rtl::OUString& aComponent,
                                                const rtl::OUString& aEntity)
    throw (backenduno::BackendAccessException,
            lang::IllegalArgumentException,
            uno::RuntimeException)
{
    static const rtl::OUString kUpdateMerger(RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.configuration.backend.LayerUpdateMerger")) ;
    uno::Sequence<uno::Any> arguments(1) ;

    arguments [0] <<= mBackend->getUpdatableLayer(
                                        mBackend->getUpdateLayerId(aComponent,
                                                                   aEntity)) ;
    return uno::Reference<backenduno::XUpdateHandler>::query(
        mFactory->createInstanceWithArguments(kUpdateMerger, arguments)) ;
}
//------------------------------------------------------------------------------

static const sal_Char * const kBackendService = "com.sun.star.configuration.backend.Backend" ;
static const sal_Char * const kAdapterService = "com.sun.star.configuration.backend.BackendAdapter" ;
static const sal_Char * const kOnlineService  = "com.sun.star.configuration.backend.OnlineBackend" ;

static const sal_Char * const kImplementation =
                "com.sun.star.comp.configuration.backend.SingleBackendAdapter" ;

static const AsciiServiceName kServiceNames [] = { kOnlineService, kAdapterService, kBackendService, 0 } ;
static const ServiceInfo kServiceInfo = { kImplementation, kServiceNames } ;

const ServiceInfo *getSingleBackendAdapterServiceInfo(void) {
    return &kServiceInfo ;
}

uno::Reference<uno::XInterface> SAL_CALL
instantiateSingleBackendAdapter(const CreationContext& aContext) {
    return *new SingleBackendAdapter(aContext) ;
}
//------------------------------------------------------------------------------

static const rtl::OUString kImplementationName(
                                RTL_CONSTASCII_USTRINGPARAM(kImplementation)) ;

rtl::OUString SAL_CALL SingleBackendAdapter::getName(void) {
    return kImplementationName ;
}
//------------------------------------------------------------------------------

rtl::OUString SAL_CALL SingleBackendAdapter::getImplementationName(void)
    throw (uno::RuntimeException)
{
    return getName() ;
}
//------------------------------------------------------------------------------

sal_Bool SAL_CALL SingleBackendAdapter::supportsService(const rtl::OUString& aServiceName)
    throw (uno::RuntimeException)
{
    return  aServiceName.equalsAscii(kOnlineService) ||
            aServiceName.equalsAscii(kAdapterService) ||
            aServiceName.equalsAscii(kBackendService) ;
}
//------------------------------------------------------------------------------

uno::Sequence<rtl::OUString> SAL_CALL SingleBackendAdapter::getServices()
{
    uno::Sequence< OUString > ret(3);
    ret[0] = OUString::createFromAscii(kOnlineService);
    ret[1] = OUString::createFromAscii(kAdapterService);
    ret[2] = OUString::createFromAscii(kBackendService);
    return ret;
}
//------------------------------------------------------------------------------

uno::Sequence<rtl::OUString>
SAL_CALL SingleBackendAdapter::getSupportedServiceNames(void)
    throw (uno::RuntimeException)
{
    return getServices() ;
}
//------------------------------------------------------------------------------

} } // configmgr.backend

