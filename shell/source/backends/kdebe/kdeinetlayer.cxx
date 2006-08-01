/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: kdeinetlayer.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: ihi $ $Date: 2006-08-01 10:26:10 $
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

#ifndef KDEINETLAYER_HXX_
#include "kdeinetlayer.hxx"
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_PROPERTYINFO_HPP_
#include <com/sun/star/configuration/backend/PropertyInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XLAYERCONTENTDESCIBER_HPP_
#include <com/sun/star/configuration/backend/XLayerContentDescriber.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#define COMMA      ','
#define SEMI_COLON ';'

//==============================================================================

KDEInetLayer::KDEInetLayer(const uno::Reference<uno::XComponentContext>& xContext)
{
    //Create instance of LayerContentDescriber Service
    rtl::OUString const k_sLayerDescriberService(RTL_CONSTASCII_USTRINGPARAM(
        "com.sun.star.comp.configuration.backend.LayerDescriber"));

    typedef uno::Reference<backend::XLayerContentDescriber> LayerDescriber;
    uno::Reference< lang::XMultiComponentFactory > xServiceManager = xContext->getServiceManager();
    if( xServiceManager.is() )
    {
        m_xLayerContentDescriber = LayerDescriber::query(
            xServiceManager->createInstanceWithContext(k_sLayerDescriberService, xContext));
    }
    else
    {
        OSL_TRACE("Could not retrieve ServiceManager");
    }
}

//------------------------------------------------------------------------------

void SAL_CALL KDEInetLayer::readData( const uno::Reference<backend::XLayerHandler>& xHandler)
    throw ( backend::MalformedDataException, lang::NullPointerException,
            lang::WrappedTargetException, uno::RuntimeException)
{
    if( ! m_xLayerContentDescriber.is() )
    {
        throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
            "Could not create com.sun.star.configuration.backend.LayerContentDescriber Service"
        ) ), static_cast < backend::XLayer * > (this) );
    }

    uno::Sequence<backend::PropertyInfo> aPropInfoList(6);
    sal_Int32 nProperties = 0;

    switch ( KProtocolManager::proxyType() )
    {
        case KProtocolManager::ManualProxy: // Proxies are manually configured
            setProxy(aPropInfoList, nProperties, 1,
                     KProtocolManager::noProxyFor(),
                     KProtocolManager::proxyFor( "HTTP" ),
                     KProtocolManager::proxyFor( "FTP" ) );
            break;
        case KProtocolManager::PACProxy:    // A proxy configuration URL has been given
        case KProtocolManager::WPADProxy:   // A proxy should be automatically discovered
        case KProtocolManager::EnvVarProxy: // Use the proxy values set through environment variables
// In such cases, the proxy address is not stored in KDE, but determined dynamically.
// The proxy address may depend on the requested address, on the time of the day, on the speed of the wind...
// The best we can do here is to ask the current value for a given address.
            setProxy(aPropInfoList, nProperties, 1,
                     KProtocolManager::noProxyFor(),
                     KProtocolManager::proxyForURL( "http://www.openoffice.org" ),
                     KProtocolManager::proxyForURL( "ftp://ftp.openoffice.org" ) );
            break;
        default:                            // No proxy is used
            setProxy(aPropInfoList, nProperties, 0);
    }

    if ( nProperties > 0 )
    {
      aPropInfoList.realloc(nProperties);
      m_xLayerContentDescriber->describeLayer(xHandler, aPropInfoList);
    }
}

//------------------------------------------------------------------------------

rtl::OUString SAL_CALL KDEInetLayer::getTimestamp(void)
    throw (uno::RuntimeException)
{
    // Return the value as timestamp to avoid regenerating the binary cache
    // on each office launch.

    QString aProxyType, aNoProxyFor, aHTTPProxy, aFTPProxy;

    switch ( KProtocolManager::proxyType() )
    {
        case KProtocolManager::ManualProxy:
            aProxyType = '1';
            aNoProxyFor = KProtocolManager::noProxyFor();
            aHTTPProxy = KProtocolManager::proxyFor( "HTTP" );
            aFTPProxy = KProtocolManager::proxyFor( "FTP" );
            break;
        case KProtocolManager::PACProxy:
        case KProtocolManager::WPADProxy:
        case KProtocolManager::EnvVarProxy:
            aProxyType = '1';
            aNoProxyFor = KProtocolManager::noProxyFor();
            aHTTPProxy = KProtocolManager::proxyForURL( "http://www.openoffice.org" );
            aFTPProxy = KProtocolManager::proxyForURL( "ftp://ftp.openoffice.org" );
            break;
        default:
            aProxyType = '0';
    }

    ::rtl::OUString sTimeStamp,
                    sep( RTL_CONSTASCII_USTRINGPARAM( "$" ) );

    sTimeStamp = (const sal_Unicode *) aProxyType.ucs2();
    sTimeStamp += sep;
    sTimeStamp += (const sal_Unicode *) aNoProxyFor.ucs2();
    sTimeStamp += sep;
    sTimeStamp += (const sal_Unicode *) aHTTPProxy.ucs2();
    sTimeStamp += sep;
    sTimeStamp += (const sal_Unicode *) aFTPProxy.ucs2();

    return sTimeStamp;
}

//------------------------------------------------------------------------------

void SAL_CALL KDEInetLayer::setProxy
    (uno::Sequence<backend::PropertyInfo> &aPropInfoList, sal_Int32 &nProperties,
     int nProxyType, const QString &aNoProxy, const QString &aHTTPProxy, const QString &aFTPProxy) const
{
    aPropInfoList[nProperties].Name = rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetProxyType") );
    aPropInfoList[nProperties].Type = rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM( "int" ) );
    aPropInfoList[nProperties].Protected = sal_False;
    aPropInfoList[nProperties++].Value = uno::makeAny( (sal_Int32) nProxyType );

    if (nProxyType == 0) return;

    if ( !aNoProxy.isEmpty() )
    {
        QString aNoProxyFor(aNoProxy);
        ::rtl::OUString sNoProxyFor;

        aNoProxyFor = aNoProxyFor.replace( COMMA, SEMI_COLON );
        sNoProxyFor = (const sal_Unicode *) aNoProxyFor.ucs2();

        aPropInfoList[nProperties].Name = rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetNoProxy") );
        aPropInfoList[nProperties].Type = rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM( "string" ) );
        aPropInfoList[nProperties].Protected = sal_False;
        aPropInfoList[nProperties++].Value = uno::makeAny( sNoProxyFor );
    }

    if ( !aHTTPProxy.isEmpty() )
    {
        KURL aProxy(aHTTPProxy);
        ::rtl::OUString sProxy = (const sal_Unicode *) aProxy.host().ucs2();
        sal_Int32 nPort = aProxy.port();

        aPropInfoList[nProperties].Name = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetHTTPProxyName") );
        aPropInfoList[nProperties].Type = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "string" ) );
        aPropInfoList[nProperties].Protected = sal_False;
        aPropInfoList[nProperties++].Value = uno::makeAny( sProxy );

        aPropInfoList[nProperties].Name = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetHTTPProxyPort") );
        aPropInfoList[nProperties].Type = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "int" ) );
        aPropInfoList[nProperties].Protected = sal_False;
        aPropInfoList[nProperties++].Value = uno::makeAny( nPort );
    }

    if ( !aFTPProxy.isEmpty() )
    {
        KURL aProxy(aFTPProxy);
        ::rtl::OUString sProxy = (const sal_Unicode *) aProxy.host().ucs2();
        sal_Int32 nPort = aProxy.port();

        aPropInfoList[nProperties].Name = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetFTPProxyName") );
        aPropInfoList[nProperties].Type = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "string" ) );
        aPropInfoList[nProperties].Protected = sal_False;
        aPropInfoList[nProperties++].Value = uno::makeAny( sProxy );

        aPropInfoList[nProperties].Name = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetFTPProxyPort") );
        aPropInfoList[nProperties].Type = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "int" ) );
        aPropInfoList[nProperties].Protected = sal_False;
        aPropInfoList[nProperties++].Value = uno::makeAny( nPort );
    }
}

