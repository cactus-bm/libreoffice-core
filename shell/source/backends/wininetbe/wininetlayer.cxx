/*************************************************************************
 *
 *  $RCSfile: wininetlayer.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-17 13:05:29 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards OOurce License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free OOftware; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free OOftware Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free OOftware
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards OOurce License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  OOurce License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  OOftware provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE OOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the OOftware.
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

#ifndef _WININETLAYER_HXX_
#include "wininetlayer.hxx"
#endif

#include <malloc.h>

#define EQUAL_SIGN '='
#define COLON      ':'
#define SPACE      ' '
#define SEMI_COLON ';'

typedef struct
{
    rtl::OUString Server;
    rtl::OUString Port;
} ProxyEntry;

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

namespace // private
{
    ProxyEntry ReadProxyEntry(const rtl::OUString& aProxy, sal_Int32& i)
    {
        ProxyEntry aProxyEntry;

        aProxyEntry.Server = aProxy.getToken( 0, COLON, i );
        if ( i > -1 )
            aProxyEntry.Port = aProxy.getToken( 0, COLON, i );

        return aProxyEntry;
    }

    ProxyEntry FindProxyEntry(const rtl::OUString& aProxyList, const rtl::OUString& aType)
    {
        sal_Int32 nIndex = 0;

        do
        {
            // get the next token, e.g. ftp=server:port
            rtl::OUString nextToken = aProxyList.getToken( 0, SPACE, nIndex );

            // split the next token again into the parts separated
            // through '=', e.g. ftp=server:port -> ftp and server:port
            sal_Int32 i = 0;
            if( nextToken.indexOf( EQUAL_SIGN ) > -1 )
            {
                if( aType.equals( nextToken.getToken( 0, EQUAL_SIGN, i ) ) )
                    return ReadProxyEntry(nextToken, i);
            }
            else if( aType.getLength() == 0)
                return ReadProxyEntry(nextToken, i);

        } while ( nIndex >= 0 );

        return ProxyEntry();
    }

} // end private namespace

//------------------------------------------------------------------------------

WinInetLayer::WinInetLayer( InternetQueryOption_Proc_T lpfnInternetQueryOption,
    const uno::Reference<uno::XComponentContext>& xContext)
  : m_lpfnInternetQueryOption(lpfnInternetQueryOption)
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

void SAL_CALL WinInetLayer::readData(
    const uno::Reference<backend::XLayerHandler>& xHandler)
    throw ( backend::MalformedDataException,
            lang::NullPointerException,
            lang::WrappedTargetException,
            uno::RuntimeException)
{

    if (m_xLayerContentDescriber.is() && m_lpfnInternetQueryOption)
    {
        LPINTERNET_PROXY_INFO lpi = NULL;

        // query for the neccessary space
        DWORD dwLength = 0;
        BOOL bRet = m_lpfnInternetQueryOption(
            NULL,
            INTERNET_OPTION_PROXY,
            (LPVOID)lpi,
            &dwLength );

        // allocate sufficient space on the heap
        // insufficient space on the heap results
        // in a stack overflow exception, we assume
        // this never happens, because of the relatively
        // small amount of memory we need
        // _alloca is nice because it is fast and we don't
        // have to free the allocated memory, it will be
        // automatically done
        lpi = reinterpret_cast< LPINTERNET_PROXY_INFO >(
            _alloca( dwLength ) );

        bRet = m_lpfnInternetQueryOption(
            NULL,
            INTERNET_OPTION_PROXY,
            (LPVOID)lpi,
            &dwLength );

        // if a proxy is disabled, InternetQueryOption returns
        // an empty proxy list, so we don't have to check if
        // proxy is enabled or not

        rtl::OUString aProxyList       = rtl::OUString::createFromAscii( lpi->lpszProxy );
        rtl::OUString aProxyBypassList = rtl::OUString::createFromAscii( lpi->lpszProxyBypass );

        // override default for ProxyType, which is "0" meaning "No proxies".
        uno::Sequence<backend::PropertyInfo> aPropInfoList(6);
        sal_Int32 nProperties = 1;

        aPropInfoList[0].Name = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetProxyType") );
        aPropInfoList[0].Type = rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "int" ) );
        aPropInfoList[0].Protected = sal_False;
        aPropInfoList[0].Value = uno::makeAny( nProperties );

        // fill proxy bypass list
        if( aProxyBypassList.getLength() > 0 )
        {
            aPropInfoList[nProperties].Name = rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetNoProxy") );
            aPropInfoList[nProperties].Type = rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "string" ) );
            aPropInfoList[nProperties].Protected = sal_False;
            aPropInfoList[nProperties++].Value = uno::makeAny( aProxyBypassList.replace( SPACE, SEMI_COLON ) );
        }

        if( aProxyList.getLength() > 0 )
        {
            //-------------------------------------------------
            // this implementation follows the algorithm
            // of the internet explorer
            // if there are type-dependent proxy settings
            // and type independent proxy settings in the
            // registry the internet explorer chooses the
            // type independent proxy for all settings
            // e.g. imagine the following registry entry
            // ftp=server:port;http=server:port;server:port
            // the last token server:port is type independent
            // so the ie chooses this proxy server

            // if there is no port specified for a type independent
            // server the ie uses the port of an http server if
            // there is one and it has a port
            //-------------------------------------------------

            ProxyEntry aTypeIndepProxy = FindProxyEntry( aProxyList, rtl::OUString());
            ProxyEntry aHttpProxy = FindProxyEntry( aProxyList, rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "http" ) ) );
            ProxyEntry aFtpProxy  = FindProxyEntry( aProxyList, rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "ftp" ) ) );

            if( aTypeIndepProxy.Server.getLength() )
            {
                aHttpProxy.Server = aTypeIndepProxy.Server;
                aFtpProxy.Server  = aTypeIndepProxy.Server;

                if( aTypeIndepProxy.Port.getLength() )
                {
                    aHttpProxy.Port = aTypeIndepProxy.Port;
                    aFtpProxy.Port  = aTypeIndepProxy.Port;
                }
                else
                {
                    aFtpProxy.Port  = aHttpProxy.Port;
                }
            }

            // http proxy name
            if( aHttpProxy.Server.getLength() > 0 )
            {
                aPropInfoList[nProperties].Name = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetHTTPProxyName") );
                aPropInfoList[nProperties].Type = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "string" ) );
                aPropInfoList[nProperties].Protected = sal_False;
                aPropInfoList[nProperties++].Value = uno::makeAny( aHttpProxy.Server );
            }

            // http proxy port
            if( aHttpProxy.Port.getLength() > 0 )
            {
                aPropInfoList[nProperties].Name = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetHTTPProxyPort") );
                aPropInfoList[nProperties].Type = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "int" ) );
                aPropInfoList[nProperties].Protected = sal_False;
                aPropInfoList[nProperties++].Value = uno::makeAny( aHttpProxy.Port.toInt32() );
            }

            // ftp proxy name
            if( aFtpProxy.Server.getLength() > 0 )
            {
                aPropInfoList[nProperties].Name = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetFTPProxyName") );
                aPropInfoList[nProperties].Type = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "string" ) );
                aPropInfoList[nProperties].Protected = sal_False;
                aPropInfoList[nProperties++].Value = uno::makeAny( aFtpProxy.Server );
            }

            // ftp proxy port
            if( aFtpProxy.Port.getLength() > 0 )
            {
                aPropInfoList[nProperties].Name = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.Inet/Settings/ooInetFTPProxyPort") );
                aPropInfoList[nProperties].Type = rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "int" ) );
                aPropInfoList[nProperties].Protected = sal_False;
                aPropInfoList[nProperties++].Value = uno::makeAny( aFtpProxy.Port.toInt32() );
            }
        }

        // resize the property info list appropriately
        aPropInfoList.realloc(nProperties);

        m_xLayerContentDescriber->describeLayer(xHandler, aPropInfoList);
    }
    else
    {
        OSL_TRACE("Could not create com.sun.star.configuration.backend.LayerContentDescriber Service");
    }
}

//------------------------------------------------------------------------------

rtl::OUString SAL_CALL WinInetLayer::getTimestamp(void)
    throw (uno::RuntimeException)
{
    rtl::OUString aTimestamp;

    if (m_lpfnInternetQueryOption)
    {
        LPINTERNET_PROXY_INFO lpi = NULL;

        // query for the neccessary space
        DWORD dwLength = 0;
        BOOL bRet = m_lpfnInternetQueryOption(
            NULL,
            INTERNET_OPTION_PROXY,
            (LPVOID)lpi,
            &dwLength );

        // allocate sufficient space on the heap
        // insufficient space on the heap results
        // in a stack overflow exception, we assume
        // this never happens, because of the relatively
        // small amount of memory we need
        // _alloca is nice because it is fast and we don't
        // have to free the allocated memory, it will be
        // automatically done
        lpi = reinterpret_cast< LPINTERNET_PROXY_INFO >(
            _alloca( dwLength ) );

        bRet = m_lpfnInternetQueryOption(
            NULL,
            INTERNET_OPTION_PROXY,
            (LPVOID)lpi,
            &dwLength );

        aTimestamp = rtl::OUString::createFromAscii( lpi->lpszProxy );
        aTimestamp += rtl::OUString::createFromAscii( lpi->lpszProxyBypass );
    }

    return aTimestamp;
}

//------------------------------------------------------------------------------
