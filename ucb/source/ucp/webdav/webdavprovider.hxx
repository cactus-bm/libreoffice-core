/*************************************************************************
 *
 *  $RCSfile: webdavprovider.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kso $ $Date: 2001-05-16 15:30:00 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
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

#ifndef _WEBDAV_UCP_PROVIDER_HXX
#define _WEBDAV_UCP_PROVIDER_HXX

#include <hash_set>

#ifndef _COM_SUN_STAR_BEANS_PROPERTY_HPP_
#include <com/sun/star/beans/Property.hpp>
#endif

#ifndef _DAVSESSIONFACTORY_HXX_
#include "DAVSessionFactory.hxx"
#endif

#ifndef _UCBHELPER_PROVIDERHELPER_HXX
#include <ucbhelper/providerhelper.hxx>
#endif

#ifndef _WEBDAV_UCP_PROPERTYMAP_HXX
#include "PropertyMap.hxx"
#endif

namespace webdav_ucp {

//=========================================================================

// UNO service name for the provider. This name will be used by the UCB to
// create instances of the provider.
#define WEBDAV_CONTENT_PROVIDER_SERVICE_NAME \
                "com.sun.star.ucb.WebDAVContentProvider"
#define WEBDAV_CONTENT_PROVIDER_SERVICE_NAME_LENGTH 38

// URL scheme. This is the scheme the provider will be able to create
// contents for. The UCB will select the provider ( i.e. in order to create
// contents ) according to this scheme.
#define WEBDAV_URL_SCHEME \
                "vnd.sun.star.webdav"
#define WEBDAV_URL_SCHEME_LENGTH    19

#define HTTP_URL_SCHEME         "http"
#define HTTP_URL_SCHEME_LENGTH  4

#define HTTPS_URL_SCHEME        "https"
#define HTTPS_URL_SCHEME_LENGTH 5

#define HTTP_CONTENT_TYPE \
                "application/" HTTP_URL_SCHEME "-content"

#define WEBDAV_CONTENT_TYPE    HTTP_CONTENT_TYPE
#define WEBDAV_COLLECTION_TYPE \
                "application/" WEBDAV_URL_SCHEME "-collection"

//=========================================================================

class ContentProvider : public ::ucb::ContentProviderImplHelper
{
    DAVSessionFactory   m_aDAVSessionFactory;
    PropertyMap *       m_pProps;

public:
    ContentProvider( const ::com::sun::star::uno::Reference<
                        ::com::sun::star::lang::XMultiServiceFactory >& rSMgr );
    virtual ~ContentProvider();

    // XInterface
    XINTERFACE_DECL()

    // XTypeProvider
    XTYPEPROVIDER_DECL()

    // XServiceInfo
    XSERVICEINFO_DECL()

    // XContentProvider
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::ucb::XContent > SAL_CALL
    queryContent( const ::com::sun::star::uno::Reference<
                    ::com::sun::star::ucb::XContentIdentifier >& Identifier )
        throw( ::com::sun::star::ucb::IllegalIdentifierException,
               ::com::sun::star::uno::RuntimeException );

    //////////////////////////////////////////////////////////////////////
    // Additional interfaces
    //////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////
    // Non-interface methods.
    //////////////////////////////////////////////////////////////////////

    DAVSessionFactory* getDAVSessionFactory() { return &m_aDAVSessionFactory; }

    bool getProperty( const ::rtl::OUString & rPropName,
                      ::com::sun::star::beans::Property & rProp,
                      bool bStrict = false );
};

}

#endif
