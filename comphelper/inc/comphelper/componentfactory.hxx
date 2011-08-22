/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef _COMPHELPER_COMPONENTFACTORY_HXX
#define _COMPHELPER_COMPONENTFACTORY_HXX
#include "comphelper/comphelperdllapi.h"

#include <com/sun/star/lang/XSingleServiceFactory.hpp>


/**
 * @Descr
 * Utilities to get an instance of a component if a ProcessServiceFactory
 * is not available like it is the case in "small tools" as the Setup.
 */
#include <com/sun/star/uno/Reference.h>


#ifdef UNX
// "libNAMExy.so" (__DLLEXTENSION == "xy.so")
#define LLCF_LIBNAME( name )    "lib" name __DLLEXTENSION
#else
// "NAMExy.dll" (__DLLEXTENSION == "xy")
#define LLCF_LIBNAME( name )    name __DLLEXTENSION ".dll"
#endif


namespace rtl {
    class OUString;
}
namespace com { namespace sun { namespace star {
    namespace lang {
        class XSingleServiceFactory;
        class XMultiServiceFactory;
    }
    namespace registry {
        class XRegistryKey;
    }
}}}


namespace comphelper
{

::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory >
    loadLibComponentFactory(
        const ::rtl::OUString & rLibraryName,
        const ::rtl::OUString & rImplementationName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & xSF,
        const ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey > & xKey
        );


}   // namespace comphelper

#endif // _COMPHELPER_COMPONENTFACTORY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
