/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: registerucb.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-05 14:55:57 $
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
#include "precompiled_ucbhelper.hxx"

#ifndef _UCBHELPER_REGISTERUCB_HXX_
#include <registerucb.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTPROVIDERMANAGER_HPP_
#include <com/sun/star/ucb/XContentProviderManager.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XPARAMETERIZEDCONTENTPROVIDER_HPP_
#include <com/sun/star/ucb/XParameterizedContentProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTPROVIDERFACTORY_HPP_
#include <com/sun/star/ucb/XContentProviderFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif

#include "osl/diagnose.h"

#ifndef _UCBHELPER_CONFIGUREUCB_HXX_
#include <ucbhelper/configureucb.hxx>
#endif

using namespace com::sun::star;

namespace ucbhelper {

//============================================================================
//
//  registerAtUcb
//
//============================================================================

bool
registerAtUcb(
    uno::Reference< ucb::XContentProviderManager > const & rManager,
    uno::Reference< lang::XMultiServiceFactory > const & rServiceFactory,
    rtl::OUString const & rName,
    rtl::OUString const & rArguments,
    rtl::OUString const & rTemplate,
    ContentProviderRegistrationInfo * pInfo)
    throw (uno::RuntimeException)
{
    OSL_ENSURE(rServiceFactory.is(),
               "ucb::registerAtUcb(): No service factory");

    bool bNoProxy
        = rArguments.compareToAscii(RTL_CONSTASCII_STRINGPARAM("{noproxy}"))
              == 0;
    rtl::OUString
        aProviderArguments(bNoProxy ?
                               rArguments.
                                   copy(RTL_CONSTASCII_LENGTH("{noproxy}")) :
                               rArguments);

    // First, try to instantiate proxy for provider:
    uno::Reference< ucb::XContentProvider > xProvider;
    if (!bNoProxy)
    {
        uno::Reference< ucb::XContentProviderFactory > xProxyFactory;
        try
        {
            xProxyFactory
                = uno::Reference< ucb::XContentProviderFactory >(
                      rServiceFactory->
                          createInstance(
                              rtl::OUString(
                                  RTL_CONSTASCII_USTRINGPARAM(
                            "com.sun.star.ucb.ContentProviderProxyFactory"))),
                      uno::UNO_QUERY);
        }
        catch (uno::Exception const &) {}
        OSL_ENSURE(xProxyFactory.is(), "No ContentProviderProxyFactory");
        if (xProxyFactory.is())
            xProvider = xProxyFactory->createContentProvider(rName);
    }

    // Then, try to instantiate provider directly:
    if (!xProvider.is())
        try
        {
            xProvider = uno::Reference< ucb::XContentProvider >(
                            rServiceFactory->createInstance(rName),
                            uno::UNO_QUERY);
        }
        catch (uno::RuntimeException const &) { throw; }
        catch (uno::Exception const &) {}

    uno::Reference< ucb::XContentProvider >
        xOriginalProvider(xProvider);
    uno::Reference< ucb::XParameterizedContentProvider >
        xParameterized(xProvider, uno::UNO_QUERY);
    if (xParameterized.is())
    {
        uno::Reference< ucb::XContentProvider > xInstance;
        try
        {
            xInstance = xParameterized->registerInstance(rTemplate,
                                                         aProviderArguments,
                                                         true);
                //@@@ if this call replaces an old instance, the commit-or-
                // rollback code below will not work
        }
        catch (lang::IllegalArgumentException const &) {}

        if (xInstance.is())
            xProvider = xInstance;
    }

    bool bSuccess = false;
    if (rManager.is() && xProvider.is())
        try
        {
            rManager->registerContentProvider(xProvider, rTemplate, true);
            bSuccess = true;
        }
        catch (ucb::DuplicateProviderException const &)
        {
            if (xParameterized.is())
                try
                {
                    xParameterized->deregisterInstance(rTemplate,
                                                       aProviderArguments);
                }
                catch (lang::IllegalArgumentException const &) {}
        }
        catch (...)
        {
            if (xParameterized.is())
                try
                {
                    xParameterized->deregisterInstance(rTemplate,
                                                       aProviderArguments);
                }
                catch (lang::IllegalArgumentException const &) {}
                catch (uno::RuntimeException const &) {}
            throw;
        }

    if (bSuccess && pInfo)
    {
        pInfo->m_xProvider = xOriginalProvider;
        pInfo->m_aArguments = aProviderArguments;
        pInfo->m_aTemplate = rTemplate;
    }
    return bSuccess;
}

//============================================================================
//
//  deregisterFromUcb
//
//============================================================================

void
deregisterFromUcb(
    uno::Reference< ucb::XContentProviderManager > const & rManager,
    ContentProviderRegistrationInfo const & rInfo)
    throw (uno::RuntimeException)
{
    uno::Reference< ucb::XContentProvider >
        xProvider(rInfo.m_xProvider);
    uno::Reference< ucb::XParameterizedContentProvider >
        xParameterized(xProvider, uno::UNO_QUERY);
    if (xParameterized.is())
    {
        uno::Reference< ucb::XContentProvider > xInstance;
        try
        {
            xInstance
                = xParameterized->deregisterInstance(rInfo.m_aTemplate,
                                                     rInfo.m_aArguments);
        }
        catch (lang::IllegalArgumentException const &) {}

        if (xInstance.is())
            xProvider = xInstance;
    }

    if (rManager.is())
        rManager->deregisterContentProvider(xProvider, rInfo.m_aTemplate);
            //@@@ if this fails, a roll-back of deregisterInstance() is
            // missing
}

}
