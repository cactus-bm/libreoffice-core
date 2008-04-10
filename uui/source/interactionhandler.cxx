/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: interactionhandler.cxx,v $
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

#include "iahndl.hxx"
#include "interactionhandler.hxx"

using namespace com::sun;

UUIInteractionHandler::UUIInteractionHandler(
    star::uno::Reference< star::lang::XMultiServiceFactory > const &
        rServiceFactory)
    SAL_THROW(())
        : m_xServiceFactory(rServiceFactory),
          m_pImpl(new UUIInteractionHelper(m_xServiceFactory))
{
}

UUIInteractionHandler::~UUIInteractionHandler()
{
    delete m_pImpl;
}

rtl::OUString SAL_CALL UUIInteractionHandler::getImplementationName()
    throw (star::uno::RuntimeException)
{
    return rtl::OUString::createFromAscii(m_aImplementationName);
}

sal_Bool SAL_CALL
UUIInteractionHandler::supportsService(rtl::OUString const & rServiceName)
    throw (star::uno::RuntimeException)
{
    star::uno::Sequence< rtl::OUString >
    aNames(getSupportedServiceNames_static());
    for (sal_Int32 i = 0; i < aNames.getLength(); ++i)
        if (aNames[i] == rServiceName)
            return true;
    return false;
}

star::uno::Sequence< rtl::OUString > SAL_CALL
UUIInteractionHandler::getSupportedServiceNames()
    throw (star::uno::RuntimeException)
{
    return getSupportedServiceNames_static();
}

void SAL_CALL
UUIInteractionHandler::initialize(
    star::uno::Sequence< star::uno::Any > const & rArguments)
    throw (star::uno::Exception)
{
    delete m_pImpl;
    m_pImpl = new UUIInteractionHelper(m_xServiceFactory, rArguments);
}

void SAL_CALL
UUIInteractionHandler::handle(
    star::uno::Reference< star::task::XInteractionRequest > const & rRequest)
    throw (star::uno::RuntimeException)
{
    try
    {
        m_pImpl->handleRequest(rRequest);
    }
    catch (star::uno::RuntimeException const & ex)
    {
        throw star::uno::RuntimeException(ex.Message, *this);
    }
}

char const UUIInteractionHandler::m_aImplementationName[]
    = "com.sun.star.comp.uui.UUIInteractionHandler";

star::uno::Sequence< rtl::OUString >
UUIInteractionHandler::getSupportedServiceNames_static()
{
    star::uno::Sequence< rtl::OUString > aNames(3);
    aNames[0] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.task.InteractionHandler"));
    // added to indicate support for configuration.backend.MergeRecoveryRequest
    aNames[1] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.configuration.backend.InteractionHandler"));
    aNames[2] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.uui.InteractionHandler"));
    // for backwards compatibility
    return aNames;
}

star::uno::Reference< star::uno::XInterface > SAL_CALL
UUIInteractionHandler::createInstance(
    star::uno::Reference< star::lang::XMultiServiceFactory > const &
        rServiceFactory)
    SAL_THROW((star::uno::Exception))
{
    try
    {
        return *new UUIInteractionHandler(rServiceFactory);
    }
    catch (std::bad_alloc const &)
    {
        throw star::uno::RuntimeException(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("out of memory")),
        0);
    }
}
