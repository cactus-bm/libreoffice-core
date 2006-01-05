/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: provider.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: kz $ $Date: 2006-01-05 18:18:03 $
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

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#include <stdio.h>
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _VOS_DIAGNOSE_HXX_
#include <vos/diagnose.hxx>
#endif
#ifndef _UCBHELPER_CONTENTIDENTIFIER_HXX
#include <ucbhelper/contentidentifier.hxx>
#endif
#ifndef _DATABASES_HXX_
#include <provider/databases.hxx>
#endif
#ifndef _PROVIDER_HXX
#include <provider/provider.hxx>
#endif
#ifndef _CONTENT_HXX
#include <provider/content.hxx>
#endif
#ifndef _DATABASES_HXX_
#include <provider/databases.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCONFIGMANAGER_HPP_
#include <com/sun/star/frame/XConfigManager.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSTATE_HPP_
#include <com/sun/star/beans/PropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINER_HPP_
#include <com/sun/star/container/XContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEREPLACE_HPP_
#include <com/sun/star/container/XNameReplace.hpp>
#endif

using namespace com::sun::star::frame;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::lang;
using namespace com::sun::star::ucb;
using namespace com::sun::star::uno;
using namespace rtl;

using namespace chelp;

//=========================================================================
//=========================================================================
//
// ContentProvider Implementation.
//
//=========================================================================
//=========================================================================

ContentProvider::ContentProvider( const Reference< XMultiServiceFactory >& rSMgr )
    : ::ucb::ContentProviderImplHelper( rSMgr ),
        isInitialized( false ),
        m_aScheme( OUString::createFromAscii( MYUCP_URL_SCHEME ) ),
        m_pDatabases( 0 )
{
}

//=========================================================================
// virtual
ContentProvider::~ContentProvider()
{
    delete m_pDatabases;
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

XINTERFACE_IMPL_6( ContentProvider,
                   XTypeProvider,
                   XServiceInfo,
                   XContentProvider,
                   XComponent,
                   XEventListener, /* base of XContainerListener */
                   XContainerListener);

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

XTYPEPROVIDER_IMPL_5( ContentProvider,
                         XTypeProvider,
                         XServiceInfo,
                         XContentProvider,
                      XComponent,
                      XContainerListener);

//=========================================================================
//
// XServiceInfo methods.
//
//=========================================================================

rtl::OUString SAL_CALL ContentProvider::getImplementationName()
    throw( com::sun::star::uno::RuntimeException )
{
    return getImplementationName_Static();
}


rtl::OUString ContentProvider::getImplementationName_Static()
{
    return OUString::createFromAscii("CHelpContentProvider" );
}


sal_Bool SAL_CALL
ContentProvider::supportsService(const rtl::OUString& ServiceName )
    throw( com::sun::star::uno::RuntimeException )
{
    com::sun::star::uno::Sequence< rtl::OUString > aSNL =
        getSupportedServiceNames();
    const rtl::OUString* pArray = aSNL.getArray();
    for( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
    {
        if( pArray[ i ] == ServiceName )
            return sal_True;
    }

    return sal_False;
}

com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
ContentProvider::getSupportedServiceNames()
    throw( com::sun::star::uno::RuntimeException )
{
    return getSupportedServiceNames_Static();
}

static com::sun::star::uno::Reference<
com::sun::star::uno::XInterface > SAL_CALL
ContentProvider_CreateInstance(
    const com::sun::star::uno::Reference<
    com::sun::star::lang::XMultiServiceFactory> & rSMgr )
    throw( com::sun::star::uno::Exception )
{
    com::sun::star::lang::XServiceInfo* pX =
        (com::sun::star::lang::XServiceInfo*)new ContentProvider( rSMgr );
    return com::sun::star::uno::Reference<
        com::sun::star::uno::XInterface >::query( pX );
}


com::sun::star::uno::Sequence< rtl::OUString >
ContentProvider::getSupportedServiceNames_Static()
{
    com::sun::star::uno::Sequence< rtl::OUString > aSNS( 2 );
    aSNS.getArray()[ 0 ] =
        OUString::createFromAscii(
            MYUCP_CONTENT_PROVIDER_SERVICE_NAME1 );
    aSNS.getArray()[ 1 ] =
        OUString::createFromAscii(
            MYUCP_CONTENT_PROVIDER_SERVICE_NAME2 );

    return aSNS;
}





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
Reference< XContent > SAL_CALL ContentProvider::queryContent( const Reference< XContentIdentifier >& xCanonicId )
    throw( IllegalIdentifierException, RuntimeException )
{
    if ( ! xCanonicId->getContentProviderScheme().equalsIgnoreAsciiCase( m_aScheme ) )
    {   // Wrong URL-scheme
        throw IllegalIdentifierException();
    }

    {
        osl::MutexGuard aGuard( m_aMutex );
        if( ! isInitialized )
            init();
    }

    if( ! m_pDatabases )
        throw RuntimeException();

    rtl::OUString aOUString( m_pDatabases->getInstallPathAsURL() );
    rtl::OString aOString( aOUString.getStr(),
                           aOUString.getLength(),
                           RTL_TEXTENCODING_UTF8 );

    // Check, if a content with given id already exists...
    Reference< XContent > xContent
        = queryExistingContent( xCanonicId ).getBodyPtr();
    if ( xContent.is() )
        return xContent;

    xContent = new Content( m_xSMgr,this,xCanonicId,m_pDatabases );

    // Further checks

    if ( !xContent->getIdentifier().is() )
        throw IllegalIdentifierException();

    return xContent;
}

void SAL_CALL
ContentProvider::dispose()
    throw ( com::sun::star::uno::RuntimeException)
{
    if(m_xContainer.is())
    {
        m_xContainer->removeContainerListener(this);
        m_xContainer = Reference<XContainer>(0);
    }
}


#include <provider/debughelper.hxx>

void SAL_CALL
ContentProvider::elementReplaced(const ContainerEvent& Event)
    throw (::com::sun::star::uno::RuntimeException)
{
    if(!m_pDatabases)
        return;

    rtl::OUString accessor;
    Event.Accessor >>= accessor;
    if(accessor.compareToAscii("HelpStyleSheet"))
        return;

    rtl::OUString replacedElement,element;
    Event.ReplacedElement >>= replacedElement;
    Event.Element >>= element;

    if(replacedElement == element)
        return;

    m_pDatabases->changeCSS(element);
}


void ContentProvider::init()
{
    osl::MutexGuard aGuard( m_aMutex );

    isInitialized = true;
    Reference< XMultiServiceFactory >  sProvider( getConfiguration() );
    Reference< XHierarchicalNameAccess > xHierAccess( getHierAccess( sProvider,
                                                                     "org.openoffice.Office.Common" ) );

    rtl::OUString instPath( getKey( xHierAccess,"Path/Current/Help" ) );
    if( ! instPath.getLength() )
        // try to determine path from default
        instPath = rtl::OUString::createFromAscii( "$(instpath)/help" );
    // replace anything like $(instpath);
    subst( instPath );


    rtl::OUString stylesheet(getKey(xHierAccess,"Help/HelpStyleSheet"));
    try {
        // now adding as configuration change listener for the stylesheet
        Reference<XNameAccess> xAccess(xHierAccess,UNO_QUERY);
        if(xAccess.is()) {
            Any aAny =
                xAccess->getByName(rtl::OUString::createFromAscii("Help"));
            aAny >>= m_xContainer;
            if(m_xContainer.is())
                m_xContainer->addContainerListener(this);
        }
    } catch(const com::sun::star::uno::Exception&) {
    }

    /**
     *  now determing
     *  productname,
     *  productversion,
     *  vendorname,
     *  vendorversion,
     *  vendorshort
     */

    xHierAccess = getHierAccess( sProvider,
                                 "org.openoffice.Setup" );
    rtl::OUString productname( getKey(  xHierAccess,"Product/ooName" ) );

    rtl::OUString setupversion( getKey(  xHierAccess,"Product/ooSetupVersion" ) );
    rtl::OUString setupextension( getKey(  xHierAccess,"Product/ooSetupExtension") );
    rtl::OUString productversion( setupversion +
                                  rtl::OUString::createFromAscii( " " ) +
                                  setupextension );


    xHierAccess = getHierAccess( sProvider,
                                 "org.openoffice.Webtop.Common" );
    rtl::OUString vendorname( getKey(  xHierAccess,"Product/ooName" ) );

    setupversion = rtl::OUString( getKey(  xHierAccess,"Product/ooSetupVersion" ) );
    setupextension = rtl::OUString ( getKey(  xHierAccess,"Product/ooSetupExtension") );
    rtl::OUString vendorversion( setupversion +
                                 rtl::OUString::createFromAscii( " " ) +
                                 setupextension );
    rtl::OUString vendorshort = vendorname;


    Sequence< rtl::OUString > aImagesZipPaths( 2 );
    xHierAccess = getHierAccess( sProvider,
                                 "org.openoffice.Office.Common" );

    rtl::OUString aPath( getKey( xHierAccess, "Path/Current/UserConfig" ) );
    subst( aPath );
    aImagesZipPaths[ 0 ] = aPath;

    aPath = getKey( xHierAccess, "Path/Current/Config" );
    subst( aPath );
    aImagesZipPaths[ 1 ] = aPath;

    sal_Bool showBasic = getBooleanKey(xHierAccess,"Help/ShowBasic");
    m_pDatabases = new Databases( showBasic,
                                  instPath,
                                  aImagesZipPaths,
                                  productname,
                                  productversion,
                                  vendorname,
                                  vendorversion,
                                  vendorshort,
                                  stylesheet,
                                  m_xSMgr );

}




Reference< XMultiServiceFactory > ContentProvider::getConfiguration() const
{
    Reference< XMultiServiceFactory > sProvider;
    if( m_xSMgr.is() )
    {
        Any aAny;
        aAny <<= rtl::OUString::createFromAscii( "plugin" );
        PropertyValue aProp( rtl::OUString::createFromAscii( "servertype" ),
                             -1,
                             aAny,
                             PropertyState_DIRECT_VALUE );

        Sequence< Any > seq(1);
        seq[0] <<= aProp;

        try
        {
            rtl::OUString sProviderService =
                rtl::OUString::createFromAscii(
                    "com.sun.star.configuration.ConfigurationProvider" );
            sProvider =
                Reference< XMultiServiceFactory >(
                    m_xSMgr->createInstanceWithArguments(
                        sProviderService,seq ),
                    UNO_QUERY );
        }
        catch( const com::sun::star::uno::Exception& )
        {
          OSL_ENSURE( sProvider.is(),"cant instantiate configuration" );
        }
    }

    return sProvider;
}



Reference< XHierarchicalNameAccess >
ContentProvider::getHierAccess( const Reference< XMultiServiceFactory >& sProvider,
                                const char* file ) const
{
    Reference< XHierarchicalNameAccess > xHierAccess;

    if( sProvider.is() )
    {
        Sequence< Any > seq(1);
        rtl::OUString sReaderService =
            rtl::OUString::createFromAscii( "com.sun.star.configuration.ConfigurationAccess" );

        seq[0] <<= rtl::OUString::createFromAscii( file );

        try
        {
            xHierAccess =
                Reference< XHierarchicalNameAccess >
                ( sProvider->createInstanceWithArguments( sReaderService,seq ),
                  UNO_QUERY );
        }
        catch( const com::sun::star::uno::Exception& )
        {
        }
    }

    return xHierAccess;
}



rtl::OUString
ContentProvider::getKey(const Reference<
                        XHierarchicalNameAccess >& xHierAccess,
                        const char* key) const
{
    rtl::OUString instPath;
    if( xHierAccess.is() )
    {
        Any aAny;
        try
        {
            aAny =
                xHierAccess->getByHierarchicalName(
                    rtl::OUString::createFromAscii(key));
        }
        catch( const com::sun::star::container::NoSuchElementException& )
        {
        }
        aAny >>= instPath;
    }
    return instPath;
}


sal_Bool
ContentProvider::getBooleanKey(const Reference<
                        XHierarchicalNameAccess >& xHierAccess,
                        const char* key) const
{
  sal_Bool ret = sal_False;
  if( xHierAccess.is() )
    {
      Any aAny;
      try
        {
          aAny =
            xHierAccess->getByHierarchicalName(
                                               rtl::OUString::createFromAscii(key));
        }
      catch( const com::sun::star::container::NoSuchElementException& )
        {
        }
      aAny >>= ret;
    }
  return ret;
}



void ContentProvider::subst( rtl::OUString& instpath ) const
{
    Reference< XConfigManager >  xCfgMgr;
    if( m_xSMgr.is() )
    {
        try
        {
            xCfgMgr =
                Reference< XConfigManager >(
                    m_xSMgr->createInstance( rtl::OUString::createFromAscii( "com.sun.star.config.SpecialConfigManager" ) ),
                    UNO_QUERY );
        }
        catch( const com::sun::star::uno::Exception&)
        {
            OSL_ENSURE( xCfgMgr.is()," cant instantiate the special config manager " );
        }
    }

    OSL_ENSURE( xCfgMgr.is(), "specialconfigmanager not found\n" );

    if( xCfgMgr.is() )
        instpath = xCfgMgr->substituteVariables( instpath );
}
