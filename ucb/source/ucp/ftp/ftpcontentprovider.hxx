/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ftpcontentprovider.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 15:33:50 $
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

#ifndef _FTP_FTPCONTENTPROVIDER_HXX_
#define _FTP_FTPCONTENTPROVIDER_HXX_

#include <vector>
#include <osl/mutex.hxx>
#include <ucbhelper/macros.hxx>
#include <ucbhelper/proxydecider.hxx>
#include <ucbhelper/providerhelper.hxx>
#include <com/sun/star/ucb/XContentProviderManager.hpp>
#include "ftphandleprovider.hxx"
#include "ftpurl.hxx"

// UNO service name for the provider. This name will be used by the UCB to
// create instances of the provider.

#define MYUCP_CONTENT_PROVIDER_SERVICE_NAME  "com.sun.star.ucb.FTPContentProvider"
#define MYUCP_CONTENT_PROVIDER_SERVICE_NAME_LENGTH  35
#define MYUCP_URL_SCHEME        "ftp"
#define MYUCP_URL_SCHEME_LENGTH 3
#define MYUCP_CONTENT_TYPE      "application/ftp-content"    // UCB Content Type.


/**
 *  Definition of ftpcontentprovider
 */



namespace ftp {


    class FTPLoaderThread;


    class FTPContentProvider:
        public ::ucb::ContentProviderImplHelper,
        public FTPHandleProvider
    {
    public:

        FTPContentProvider(
            const com::sun::star::uno::Reference<
            com::sun::star::lang::XMultiServiceFactory>& xMSF );

        ~FTPContentProvider();

        // XInterface
        XINTERFACE_DECL()

        // XTypeProvider
        XTYPEPROVIDER_DECL()

        // XServiceInfo
        XSERVICEINFO_DECL()

        // XContentProvider
        virtual com::sun::star::uno::Reference< com::sun::star::ucb::XContent > SAL_CALL
        queryContent(
            const com::sun::star::uno::Reference< com::sun::star::ucb::XContentIdentifier >& Identifier )
            throw( com::sun::star::ucb::IllegalIdentifierException,
                   com::sun::star::uno::RuntimeException );

        // FTPHandleProvider.

        virtual CURL* handle();

        virtual bool forHost(const rtl::OUString& host,
                             const rtl::OUString& port,
                             const rtl::OUString& username,
                             rtl::OUString& password,
                             rtl::OUString& account);

        virtual bool setHost(const rtl::OUString& host,
                             const rtl::OUString& port,
                             const rtl::OUString& username,
                             const rtl::OUString& password,
                             const rtl::OUString& account);


        struct ServerInfo {
            rtl::OUString host;
            rtl::OUString port;
            rtl::OUString username;
            rtl::OUString password;
            rtl::OUString account;
        };

    private:

        osl::Mutex m_aMutex;
        FTPLoaderThread *m_ftpLoaderThread;
        ucbhelper::InternetProxyDecider *m_pProxyDecider;
        std::vector<ServerInfo> m_ServerInfo;

        void init();

        com::sun::star::uno::Reference<com::sun::star::ucb::XContentProvider>
        getHttpProvider()
            throw(com::sun::star::uno::RuntimeException);

    };  // end class FTPContentProvider

}       // end namespace ftp

#endif

