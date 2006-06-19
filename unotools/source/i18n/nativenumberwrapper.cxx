/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: nativenumberwrapper.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 14:06:56 $
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

#include <unotools/nativenumberwrapper.hxx>

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _COMPHELPER_COMPONENTFACTORY_HXX_
#include <comphelper/componentfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#define LOCALEDATA_LIBRARYNAME "i18npool"
#define LOCALEDATA_SERVICENAME "com.sun.star.i18n.NativeNumberSupplier"

using namespace ::com::sun::star;


NativeNumberWrapper::NativeNumberWrapper(
            const uno::Reference< lang::XMultiServiceFactory > & xSF
            )
        :
        xSMgr( xSF )
{
    if ( xSMgr.is() )
    {
        try
        {
            xNNS = uno::Reference< i18n::XNativeNumberSupplier > ( xSMgr->createInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( LOCALEDATA_SERVICENAME ) ) ),
                uno::UNO_QUERY );
        }
        catch ( uno::Exception& e )
        {
            (void)e;
            DBG_ERRORFILE( "NativeNumberWrapper ctor: Exception caught!" );
        }
    }
    else
    {   // try to get an instance somehow
        DBG_ERRORFILE( "NativeNumberWrapper: no service manager, trying own" );
        try
        {
            uno::Reference< uno::XInterface > xI = ::comphelper::getComponentInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( LLCF_LIBNAME( LOCALEDATA_LIBRARYNAME ) ) ),
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( LOCALEDATA_SERVICENAME ) ) );
            if ( xI.is() )
            {
                uno::Any x = xI->queryInterface( ::getCppuType((const uno::Reference< i18n::XNativeNumberSupplier >*)0) );
                x >>= xNNS;
            }
        }
        catch ( uno::Exception& e )
        {
            (void)e;
            DBG_ERRORFILE( "getComponentInstance: Exception caught!" );
        }
    }
    DBG_ASSERT( xNNS.is(), "NativeNumberWrapper: no NativeNumberSupplier" );
}


NativeNumberWrapper::~NativeNumberWrapper()
{
}


::rtl::OUString
NativeNumberWrapper::getNativeNumberString(
                    const ::rtl::OUString& rNumberString,
                    const ::com::sun::star::lang::Locale& rLocale,
                    sal_Int16 nNativeNumberMode ) const
{
    try
    {
        if ( xNNS.is() )
            return xNNS->getNativeNumberString( rNumberString, rLocale, nNativeNumberMode );
    }
    catch ( uno::Exception& e )
    {
        (void)e;
        DBG_ERRORFILE( "getNativeNumberString: Exception caught!" );
    }
    return ::rtl::OUString();
}


sal_Bool
NativeNumberWrapper::isValidNatNum(
                    const ::com::sun::star::lang::Locale& rLocale,
                    sal_Int16 nNativeNumberMode ) const
{
    try
    {
        if ( xNNS.is() )
            return xNNS->isValidNatNum( rLocale, nNativeNumberMode );
    }
    catch ( uno::Exception& e )
    {
        (void)e;
        DBG_ERRORFILE( "isValidNatNum: Exception caught!" );
    }
    return sal_False;
}


i18n::NativeNumberXmlAttributes
NativeNumberWrapper::convertToXmlAttributes(
                    const ::com::sun::star::lang::Locale& rLocale,
                    sal_Int16 nNativeNumberMode ) const
{
    try
    {
        if ( xNNS.is() )
            return xNNS->convertToXmlAttributes( rLocale, nNativeNumberMode );
    }
    catch ( uno::Exception& e )
    {
        (void)e;
        DBG_ERRORFILE( "convertToXmlAttributes: Exception caught!" );
    }
    return i18n::NativeNumberXmlAttributes();
}


sal_Int16
NativeNumberWrapper::convertFromXmlAttributes(
                    const i18n::NativeNumberXmlAttributes& rAttr ) const
{
    try
    {
        if ( xNNS.is() )
            return xNNS->convertFromXmlAttributes( rAttr );
    }
    catch ( uno::Exception& e )
    {
        (void)e;
        DBG_ERRORFILE( "convertFromXmlAttributes: Exception caught!" );
    }
    return 0;
}


