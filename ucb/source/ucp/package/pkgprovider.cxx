/*************************************************************************
 *
 *  $RCSfile: pkgprovider.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: kso $ $Date: 2001-06-25 09:11:47 $
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
 *  Contributor(s): Kai Sommerfeld ( kso@sun.com )
 *
 *
 ************************************************************************/

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#include <hash_map>

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _UCBHELPER_CONTENTIDENTIFIER_HXX
#include <ucbhelper/contentidentifier.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif

#ifndef _PKGPROVIDER_HXX
#include "pkgprovider.hxx"
#endif
#ifndef _PKGCONTENT_HXX
#include "pkgcontent.hxx"
#endif
#ifndef _PKGURI_HXX
#include "pkguri.hxx"
#endif

using namespace com::sun;
using namespace com::sun::star;

namespace package_ucp
{

//=========================================================================
//
// class Package.
//
//=========================================================================

class Package : public cppu::OWeakObject,
                public com::sun::star::container::XHierarchicalNameAccess
{
    friend class ContentProvider;

    rtl::OUString                                        m_aName;
    uno::Reference< container::XHierarchicalNameAccess > m_xNA;
    ContentProvider*                                     m_pOwner;

public:
    Package( const rtl::OUString& rName,
             const uno::Reference< container::XHierarchicalNameAccess > & xNA,
             ContentProvider* pOwner )
    : m_aName( rName ), m_xNA( xNA ), m_pOwner( pOwner ) {}
    virtual ~Package() { m_pOwner->removePackage( m_aName ); }

    // XInterface
    virtual uno::Any SAL_CALL
    queryInterface( const uno::Type& aType )
        throw( uno::RuntimeException )
    { return m_xNA->queryInterface( aType ); }
    virtual void SAL_CALL
    acquire() throw()
    { OWeakObject::acquire(); }
    virtual void SAL_CALL
    release() throw()
    { OWeakObject::release(); }

    // XHierarchicalNameAccess
    virtual uno::Any SAL_CALL
    getByHierarchicalName( const rtl::OUString& aName )
        throw( container::NoSuchElementException, uno::RuntimeException )
    { return m_xNA->getByHierarchicalName( aName ); }
    virtual sal_Bool SAL_CALL
    hasByHierarchicalName( const rtl::OUString& aName )
        throw( uno::RuntimeException )
    { return m_xNA->hasByHierarchicalName( aName ); }
};

//=========================================================================
//
// Packages.
//
//=========================================================================

struct equalString
{
    bool operator()(
        const rtl::OUString& rKey1, const rtl::OUString& rKey2 ) const
    {
        return !!( rKey1 == rKey2 );
    }
};

struct hashString
{
    size_t operator()( const rtl::OUString & rName ) const
    {
        return rName.hashCode();
    }
};

typedef std::hash_map
<
    rtl::OUString,
    Package*,
    hashString,
    equalString
>
PackageMap;

class Packages : public PackageMap {};

}

using namespace package_ucp;

//=========================================================================
//=========================================================================
//
// ContentProvider Implementation.
//
//=========================================================================
//=========================================================================

ContentProvider::ContentProvider(
            const uno::Reference< lang::XMultiServiceFactory >& rSMgr )
: ::ucb::ContentProviderImplHelper( rSMgr ),
  m_pPackages( 0 )
{
}

//=========================================================================
// virtual
ContentProvider::~ContentProvider()
{
    delete m_pPackages;
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

XINTERFACE_IMPL_3( ContentProvider,
                   lang::XTypeProvider,
                   lang::XServiceInfo,
                   star::ucb::XContentProvider );

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

XTYPEPROVIDER_IMPL_3( ContentProvider,
                      lang::XTypeProvider,
                      lang::XServiceInfo,
                      star::ucb::XContentProvider );

//=========================================================================
//
// XServiceInfo methods.
//
//=========================================================================

XSERVICEINFO_IMPL_1( ContentProvider,
                     rtl::OUString::createFromAscii(
                        "com.sun.star.comp.ucb.PackageContentProvider" ),
                     rtl::OUString::createFromAscii(
                        PACKAGE_CONTENT_PROVIDER_SERVICE_NAME ) );

//=========================================================================
//
// Service factory implementation.
//
//=========================================================================

ONE_INSTANCE_SERVICE_FACTORY_IMPL( ContentProvider );

//=========================================================================
//
// XContentProvider methods.
//
//=========================================================================

// virtual
uno::Reference< star::ucb::XContent > SAL_CALL ContentProvider::queryContent(
            const uno::Reference< star::ucb::XContentIdentifier >& Identifier )
    throw( star::ucb::IllegalIdentifierException, uno::RuntimeException )
{
    vos::OGuard aGuard( m_aMutex );

    // Check URL scheme...

    rtl::OUString aScheme(
        rtl::OUString::createFromAscii( PACKAGE_URL_SCHEME ) );
    if ( !Identifier->getContentProviderScheme().equalsIgnoreAsciiCase(
                                                                aScheme ) )
        throw star::ucb::IllegalIdentifierException();

    // Normalize URL...
    PackageUri aUri( Identifier->getContentIdentifier() );
    uno::Reference< star::ucb::XContentIdentifier > xId
                = new ::ucb::ContentIdentifier( m_xSMgr, aUri.getUri() );

    // Check, if a content with given id already exists...
    uno::Reference< star::ucb::XContent > xContent
        = queryExistingContent( xId ).getBodyPtr();
    if ( xContent.is() )
        return xContent;

    // Create a new content. Note that the content will insert itself
    // into providers content list by calling addContent(...) from it's ctor.

    xContent = Content::create( m_xSMgr, this, Identifier ); // not xId!!!

    if ( xContent.is() && !xContent->getIdentifier().is() )
        throw star::ucb::IllegalIdentifierException();

    return xContent;
}

//=========================================================================
//
// Other methods.
//
//=========================================================================

uno::Reference< container::XHierarchicalNameAccess >
ContentProvider::createPackage( const rtl::OUString & rName )
{
    vos::OGuard aGuard( m_aMutex );

    if ( !rName.getLength() )
    {
        OSL_ENSURE( sal_False,
                    "ContentProvider::createPackage - Invalid URL!" );
        return uno::Reference< container::XHierarchicalNameAccess >();
    }

    if ( m_pPackages )
    {
        Packages::const_iterator it = m_pPackages->find( rName );
        if ( it != m_pPackages->end() )
        {
            // Already instanciated. Return package.
            return (*it).second->m_xNA;
        }
    }
    else
        m_pPackages = new Packages;

    // Create new package...
    try
    {
        uno::Sequence< uno::Any > aArguments( 1 );
        aArguments[ 0 ] <<= rName;

        uno::Reference< uno::XInterface > xIfc
            = m_xSMgr->createInstanceWithArguments(
                rtl::OUString::createFromAscii(
                                "com.sun.star.packages.comp.ZipPackage" ),
                aArguments );

        if ( xIfc.is() )
        {
            uno::Reference<
                container::XHierarchicalNameAccess > xNameAccess(
                                                        xIfc, uno::UNO_QUERY );

            OSL_ENSURE( xNameAccess.is(),
                        "ContentProvider::createPackage - "
                        "Got no hierarchical name access!" );

            rtl::Reference< Package> xPackage
                = new Package( rName, xNameAccess, this );

            (*m_pPackages)[ rName ] = xPackage.get();

            return xPackage.get();
        }
    }
    catch ( uno::RuntimeException const & )
    {
        // createInstanceWithArguemts
    }
    catch ( uno::Exception const & )
    {
        // createInstanceWithArguemts
    }

    return uno::Reference< container::XHierarchicalNameAccess >();
}

//=========================================================================
sal_Bool ContentProvider::removePackage( const rtl::OUString & rName )
{
    vos::OGuard aGuard( m_aMutex );

    if ( m_pPackages )
    {
        Packages::iterator it = m_pPackages->find( rName );
        if ( it != m_pPackages->end() )
        {
            m_pPackages->erase( it );
            return sal_True;
        }
    }
    return sal_False;
}

