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
#include "vbahelper/vbaglobalbase.hxx"

#include <cppuhelper/component_context.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/container/XNameContainer.hpp>

using namespace com::sun::star;
using namespace ooo::vba;

rtl::OUString sApplication( RTL_CONSTASCII_USTRINGPARAM("Application") );

VbaGlobalsBase::VbaGlobalsBase(
const uno::Reference< ov::XHelperInterface >& xParent,
const uno::Reference< uno::XComponentContext >& xContext, const rtl::OUString& sDocCtxName )
:  Globals_BASE( xParent, xContext )
{
    // overwrite context with custom one ( that contains the application )
    ::cppu::ContextEntry_Init aHandlerContextInfo[] =
    {
        ::cppu::ContextEntry_Init( sApplication, uno::Any() ),
        ::cppu::ContextEntry_Init( sDocCtxName, uno::Any() ),
    };

    mxContext = ::cppu::createComponentContext( aHandlerContextInfo, sizeof( aHandlerContextInfo ) / sizeof( aHandlerContextInfo[0] ), xContext );

}


void
VbaGlobalsBase::init(  const uno::Sequence< beans::PropertyValue >& aInitArgs )
{
    sal_Int32 nLen = aInitArgs.getLength();
    for ( sal_Int32 nIndex = 0; nIndex < nLen; ++nIndex )
    {
        uno::Reference< container::XNameContainer > xNameContainer( mxContext, uno::UNO_QUERY_THROW );
        if ( aInitArgs[ nIndex ].Name.equals( sApplication ) )
        {
            xNameContainer->replaceByName( sApplication, aInitArgs[ nIndex ].Value );
            uno::Reference< XHelperInterface > xParent( aInitArgs[ nIndex ].Value, uno::UNO_QUERY );
            mxParent = xParent;
        }
        else
            xNameContainer->replaceByName( aInitArgs[ nIndex ].Name, aInitArgs[ nIndex ].Value );
    }
}

uno::Reference< uno::XInterface > SAL_CALL
VbaGlobalsBase::createInstance( const ::rtl::OUString& aServiceSpecifier ) throw (uno::Exception, uno::RuntimeException)
{
    uno::Reference< uno::XInterface > xReturn;

    if ( hasServiceName( aServiceSpecifier ) )
        xReturn = mxContext->getServiceManager()->createInstanceWithContext( aServiceSpecifier, mxContext );
    return xReturn;
}

uno::Reference< uno::XInterface > SAL_CALL
VbaGlobalsBase::createInstanceWithArguments( const ::rtl::OUString& ServiceSpecifier, const uno::Sequence< uno::Any >& Arguments ) throw (uno::Exception, uno::RuntimeException)
{

    uno::Reference< uno::XInterface > xReturn;

    if ( hasServiceName( ServiceSpecifier ) )
        xReturn = mxContext->getServiceManager()->createInstanceWithArgumentsAndContext( ServiceSpecifier, Arguments, mxContext );
    return xReturn;
}

uno::Sequence< ::rtl::OUString > SAL_CALL
VbaGlobalsBase::getAvailableServiceNames(  ) throw (uno::RuntimeException)
{
    static const rtl::OUString names[] = {
    // common
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( "ooo.vba.msforms.UserForm" ) ),
      };
    static uno::Sequence< rtl::OUString > serviceNames( names, sizeof( names )/ sizeof( names[0] ) );
    return serviceNames;
}

bool
VbaGlobalsBase::hasServiceName( const rtl::OUString& serviceName )
{
    uno::Sequence< rtl::OUString > sServiceNames( getAvailableServiceNames() );
    sal_Int32 nLen = sServiceNames.getLength();
    for ( sal_Int32 index = 0; index < nLen; ++index )
    {
        if ( sServiceNames[ index ].equals( serviceName ) )
            return true;
    }
    return false;
}


