/*************************************************************************
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: core.cxx,v $
 *
 * $Revision: 1.3.34.1 $
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
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_extensions.hxx"

#include <com/sun/star/oooimprovement/XCore.hpp>

#include "oooimprovecore_module.hxx"
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/oooimprovement/XCoreController.hpp>
#include <comphelper/componentmodule.hxx>
#include <comphelper/uieventslogger.hxx>
#include <comphelper/configurationhelper.hxx>
#include <cppuhelper/implbase2.hxx>
#include <svx/optimprove.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>


// declaration
namespace oooimprovecore
{
    using namespace ::com::sun::star::oooimprovement;
    using ::com::sun::star::lang::XServiceInfo;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::lang::XMultiServiceFactory;
    using ::com::sun::star::uno::XInterface;

    class Core : public ::cppu::WeakImplHelper2<XCore,XServiceInfo>
    {
        public:
            // XServiceInfo - static version
            static ::rtl::OUString SAL_CALL getImplementationName_static();
            static Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static();
            static Reference<XInterface> Create(const Reference<XComponentContext>& context );

        protected:
            Core(const Reference<XComponentContext>& context);
            virtual ~Core();

            // XCore
            virtual sal_Int32 SAL_CALL getSessionLogEventCount() throw(RuntimeException);
            virtual sal_Bool SAL_CALL getUiEventsLoggerEnabled() throw(RuntimeException);
            virtual void SAL_CALL inviteUser() throw(RuntimeException);

            // XServiceInfo
            virtual ::rtl::OUString SAL_CALL getImplementationName() throw(RuntimeException);
            virtual ::sal_Bool SAL_CALL supportsService(const ::rtl::OUString& service_name) throw(RuntimeException);
            virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(RuntimeException);

        private:
            Reference<XMultiServiceFactory> m_ServiceFactory;
    };
}


// implementation
namespace oooimprovecore {
    using namespace ::rtl;
    using namespace ::com::sun::star::uno;

    Core::Core(const Reference<XComponentContext>& context)
        : m_ServiceFactory(Reference<XMultiServiceFactory>(
            context->getServiceManager()->createInstanceWithContext(
                OUString::createFromAscii("com.sun.star.lang.MultiServiceFactory"), context),
            UNO_QUERY))
    { }

    Core::~Core()
    { }

    sal_Int32 SAL_CALL Core::getSessionLogEventCount() throw(RuntimeException)
    { return ::comphelper::UiEventsLogger::getSessionLogEventCount(); }

    sal_Bool SAL_CALL Core::getUiEventsLoggerEnabled() throw(RuntimeException)
    { return ::comphelper::UiEventsLogger::isEnabled(); }

    void SAL_CALL Core::inviteUser() throw(RuntimeException)
    {
        OUString help_url;
        Reference<XCoreController> core_c(
            m_ServiceFactory->createInstance(OUString::createFromAscii("com.sun.star.oooimprovement.CoreController")),
            UNO_QUERY);
        if(core_c.is())
            ::comphelper::ConfigurationHelper::readDirectKey(
                m_ServiceFactory,
                ::rtl::OUString::createFromAscii("/org.openoffice.Office.OOoImprovement.Settings"),
                ::rtl::OUString::createFromAscii("Participation"),
                ::rtl::OUString::createFromAscii("HelpUrl"),
                ::comphelper::ConfigurationHelper::E_READONLY) >>= help_url;
        else
            help_url = OUString::createFromAscii("http://www.openoffice.org");

        {
            ::vos::OGuard aGuard( Application::GetSolarMutex() );
            SvxImprovementDialog dlg(NULL, help_url);
            dlg.Execute();
        }
    }

    sal_Bool SAL_CALL Core::supportsService(const OUString& service_name) throw(RuntimeException)
    {
        const Sequence<OUString> service_names(getSupportedServiceNames());
        for (sal_Int32 idx = service_names.getLength()-1; idx>=0; --idx)
            if(service_name == service_names[idx]) return sal_True;
        return sal_False;
    }

    OUString SAL_CALL Core::getImplementationName() throw(RuntimeException)
    { return getImplementationName_static(); }

    Sequence<OUString> SAL_CALL Core::getSupportedServiceNames() throw(RuntimeException)
    { return getSupportedServiceNames_static(); }

    OUString SAL_CALL Core::getImplementationName_static()
    { return OUString::createFromAscii("com.sun.star.comp.extensions.oooimprovecore.Core"); }

    Sequence<OUString> SAL_CALL Core::getSupportedServiceNames_static()
    {
        Sequence<OUString> aServiceNames(1);
        aServiceNames[0] = OUString::createFromAscii("com.sun.star.oooimprovement.Core");
        return aServiceNames;
    }

    Reference<XInterface> Core::Create(const Reference<XComponentContext>& context)
    { return *(new Core(context)); }

    void createRegistryInfo_Core()
    {
        static OAutoRegistration<Core> auto_reg;
    }
}
