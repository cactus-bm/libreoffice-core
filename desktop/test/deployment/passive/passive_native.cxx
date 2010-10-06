/*************************************************************************
*
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2000, 2010 Oracle and/or its affiliates.
*
* OpenOffice.org - a multi-platform office productivity suite
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

#include "precompiled_desktop.hxx"
#include "sal/config.h"

#include "boost/noncopyable.hpp"
#include "com/sun/star/awt/MessageBoxButtons.hpp"
#include "com/sun/star/awt/Rectangle.hpp"
#include "com/sun/star/awt/XMessageBox.hpp"
#include "com/sun/star/awt/XMessageBoxFactory.hpp"
#include "com/sun/star/awt/XWindowPeer.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/frame/DispatchDescriptor.hpp"
#include "com/sun/star/frame/XDesktop.hpp"
#include "com/sun/star/frame/XDispatch.hpp"
#include "com/sun/star/frame/XDispatchProvider.hpp"
#include "com/sun/star/frame/XFrame.hpp"
#include "com/sun/star/frame/XStatusListener.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/util/URL.hpp"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase1.hxx"
#include "cppuhelper/implbase3.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/weak.hxx"
#include "osl/diagnose.h"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "uno/lbnames.h"

namespace {

namespace css = com::sun::star;

namespace service {

rtl::OUString getImplementationName() {
    return rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.comp.test.deployment.passive_native"));
}

css::uno::Sequence< rtl::OUString > getSupportedServiceNames() {
    rtl::OUString name(
        RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.test.deployment.passive_native"));
    return css::uno::Sequence< rtl::OUString >(&name, 1);
}

}

class Service:
    public cppu::WeakImplHelper3<
        css::lang::XServiceInfo, css::frame::XDispatchProvider,
        css::frame::XDispatch >,
    private boost::noncopyable
{
public:
    Service(css::uno::Reference< css::uno::XComponentContext > const & context):
        context_(context) { OSL_ASSERT(context.is()); }

private:
    virtual ~Service() {}

    virtual rtl::OUString SAL_CALL getImplementationName()
        throw (css::uno::RuntimeException)
    { return service::getImplementationName(); }

    virtual sal_Bool SAL_CALL supportsService(rtl::OUString const & ServiceName)
        throw (css::uno::RuntimeException)
    { return ServiceName == getSupportedServiceNames()[0]; } //TODO

    virtual css::uno::Sequence< rtl::OUString > SAL_CALL
    getSupportedServiceNames() throw (css::uno::RuntimeException)
    { return service::getSupportedServiceNames(); }

    virtual css::uno::Reference< css::frame::XDispatch > SAL_CALL queryDispatch(
        css::util::URL const &, rtl::OUString const &, sal_Int32)
        throw (css::uno::RuntimeException)
    { return this; }

    virtual css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > >
    SAL_CALL queryDispatches(
        css::uno::Sequence< css::frame::DispatchDescriptor > const & Requests)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL dispatch(
        css::util::URL const &,
        css::uno::Sequence< css::beans::PropertyValue > const &)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL addStatusListener(
        css::uno::Reference< css::frame::XStatusListener > const &,
        css::util::URL const &)
        throw (css::uno::RuntimeException)
    {}

    virtual void SAL_CALL removeStatusListener(
        css::uno::Reference< css::frame::XStatusListener > const &,
        css::util::URL const &)
        throw (css::uno::RuntimeException)
    {}

    css::uno::Reference< css::uno::XComponentContext > context_;
};

css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > >
Service::queryDispatches(
    css::uno::Sequence< css::frame::DispatchDescriptor > const & Requests)
    throw (css::uno::RuntimeException)
{
    css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > s(
        Requests.getLength());
    for (sal_Int32 i = 0; i < s.getLength(); ++i) {
        s[i] = queryDispatch(
            Requests[i].FeatureURL, Requests[i].FrameName,
            Requests[i].SearchFlags);
    }
    return s;
}

void Service::dispatch(
    css::util::URL const &,
    css::uno::Sequence< css::beans::PropertyValue > const &)
    throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::lang::XMultiComponentFactory > smgr(
        context_->getServiceManager(), css::uno::UNO_SET_THROW);
    css::uno::Reference< css::awt::XMessageBox > box(
        css::uno::Reference< css::awt::XMessageBoxFactory >(
            smgr->createInstanceWithContext(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                  "com.sun.star.awt.Toolkit")), context_),
            css::uno::UNO_QUERY_THROW)->createMessageBox(
                css::uno::Reference< css::awt::XWindowPeer >(
                    css::uno::Reference< css::frame::XFrame >(
                        css::uno::Reference< css::frame::XDesktop >(
                            smgr->createInstanceWithContext(
                                rtl::OUString(
                                    RTL_CONSTASCII_USTRINGPARAM(
                                        "com.sun.star.frame.Desktop")),
                                context_),
                            css::uno::UNO_QUERY_THROW)->getCurrentFrame(),
                        css::uno::UNO_SET_THROW)->getComponentWindow(),
                    css::uno::UNO_QUERY_THROW),
                css::awt::Rectangle(),
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("infobox")),
                css::awt::MessageBoxButtons::BUTTONS_OK,
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("passive")),
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("native"))),
        css::uno::UNO_SET_THROW);
    box->execute();
    css::uno::Reference< css::lang::XComponent >(
        box, css::uno::UNO_QUERY_THROW)->dispose();
}

class Factory:
    public cppu::WeakImplHelper1< css::lang::XSingleComponentFactory >,
    private boost::noncopyable
{
public:
    Factory() {}

private:
    virtual ~Factory() {}

    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL
    createInstanceWithContext(
        css::uno::Reference< css::uno::XComponentContext > const & Context)
        throw (css::uno::Exception, css::uno::RuntimeException)
    { return static_cast< cppu::OWeakObject * >(new Service(Context)); }

    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL
    createInstanceWithArgumentsAndContext(
        css::uno::Sequence< css::uno::Any > const &,
        css::uno::Reference< css::uno::XComponentContext > const & Context)
        throw (css::uno::Exception, css::uno::RuntimeException)
    { return createInstanceWithContext(Context); }
};

css::uno::Reference< css::uno::XInterface > SAL_CALL dummy(
    css::uno::Reference< css::uno::XComponentContext > const &)
    SAL_THROW((css::uno::Exception))
{
    OSL_ASSERT(false);
    return css::uno::Reference< css::uno::XInterface >();
}

css::uno::Reference< css::lang::XSingleComponentFactory > SAL_CALL
createFactory(
    cppu::ComponentFactoryFunc, rtl::OUString const &,
    css::uno::Sequence< rtl::OUString > const &, rtl_ModuleCount *)
    SAL_THROW(())
{
    return new Factory;
}

static cppu::ImplementationEntry const services[] = {
    { &dummy, &service::getImplementationName,
      &service::getSupportedServiceNames, &createFactory, 0, 0 },
    { 0, 0, 0, 0, 0, 0 }
};

}

extern "C" void * SAL_CALL component_getFactory(
    char const * pImplName, void * pServiceManager, void * pRegistryKey)
{
    return cppu::component_getFactoryHelper(
        pImplName, pServiceManager, pRegistryKey, services);
}

extern "C" void SAL_CALL component_getImplementationEnvironment(
    char const ** ppEnvTypeName, uno_Environment **)
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}
