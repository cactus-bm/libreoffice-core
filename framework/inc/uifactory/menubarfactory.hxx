/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_FRAMEWORK_INC_UIFACTORY_MENUBARFACTORY_HXX
#define INCLUDED_FRAMEWORK_INC_UIFACTORY_MENUBARFACTORY_HXX

#include <stdtypes.h>
#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/ui/XUIElementFactory.hpp>
#include <com/sun/star/frame/XModuleManager2.hpp>

#include <toolkit/awt/vclxmenu.hxx>
#include <cppuhelper/implbase2.hxx>
#include <rtl/ustring.hxx>

namespace framework
{
    class MenuBarFactory :  protected ThreadHelpBase                                    ,   // Struct for right initalization of mutex member! Must be first of baseclasses.
                            public ::cppu::WeakImplHelper2< com::sun::star::lang::XServiceInfo,
                                                            ::com::sun::star::ui::XUIElementFactory>
    {
        public:
            MenuBarFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext );
            virtual ~MenuBarFactory();

            //  XInterface, XTypeProvider, XServiceInfo
            DECLARE_XSERVICEINFO

            // XUIElementFactory
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIElement > SAL_CALL createUIElement( const OUString& ResourceURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Args ) throw ( ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );

            static void CreateUIElement(const OUString& ResourceURL
                        , const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Args
                        ,const char* _pExtraMode
                        ,const char* _pAsciiName
                        ,const ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIElement >& _xMenuBar
                        ,const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager2 >& _xModuleManager
                        ,const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& _rxContext);

        protected:
            MenuBarFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext,bool );

            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >     m_xContext;
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager2 >     m_xModuleManager;
    };
}

#endif // INCLUDED_FRAMEWORK_INC_UIFACTORY_MENUBARFACTORY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
