/*************************************************************************
 *
 *  $RCSfile: providerhelper.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kso $ $Date: 2000-10-26 15:15:21 $
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

#ifndef __HASH_MAP__
#include <stl/hash_map>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYACCESS_HPP_
#include <com/sun/star/beans/XPropertyAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XPROPERTYSETREGISTRYFACTORY_HPP_
#include <com/sun/star/ucb/XPropertySetRegistryFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XPROPERTYSETREGISTRY_HPP_
#include <com/sun/star/ucb/XPropertySetRegistry.hpp>
#endif
#ifndef _VOS_DIAGNOSE_HXX_
#include <vos/diagnose.hxx>
#endif
#ifndef _UCBHELPER_CONTENTIDENTIFIER_HXX
#include <ucbhelper/contentidentifier.hxx>
#endif
#ifndef _UCBHELPER_PROVIDERHELPER_HXX
#include <ucbhelper/providerhelper.hxx>
#endif
#ifndef _UCBHELPER_CONTENTHELPER_HXX
#include <ucbhelper/contenthelper.hxx>
#endif

using namespace rtl;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::lang;
using namespace com::sun::star::registry;
using namespace com::sun::star::ucb;
using namespace com::sun::star::uno;
using namespace ucb;

namespace ucb_impl
{

//=========================================================================
//
// Contents.
//
//=========================================================================

struct equalString
{
    bool operator()( const OUString& rKey11, const OUString& rKey22 ) const
      {
          return !!( rKey11 == rKey22 );
      }
};

struct hashString
{
    size_t operator()( const OUString & rName ) const
    {
        return rName.hashCode();
    }
};

typedef std::hash_map
<
    OUString,
    ContentImplHelper*,
    hashString,
    equalString
>
Contents;

//=========================================================================
//
// struct ContentProviderImplHelper_Impl.
//
//=========================================================================

struct ContentProviderImplHelper_Impl
{
    Reference< XPropertySetRegistry > m_xPropertySetRegistry;
    Contents m_aContents;
};

} // namespace ucb_impl

using namespace ucb_impl;

//=========================================================================
//=========================================================================
//
// ContentProviderImplHelper Implementation.
//
//=========================================================================
//=========================================================================

ContentProviderImplHelper::ContentProviderImplHelper(
                        const Reference< XMultiServiceFactory >& rXSMgr )
: m_xSMgr( rXSMgr ),
  m_pImpl( new ucb_impl::ContentProviderImplHelper_Impl )
{
}

//=========================================================================
// virtual
ContentProviderImplHelper::~ContentProviderImplHelper()
{
    delete m_pImpl;
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

XINTERFACE_IMPL_3( ContentProviderImplHelper,
                   XTypeProvider,
                   XServiceInfo,
                   XContentProvider );

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

XTYPEPROVIDER_IMPL_3( ContentProviderImplHelper,
                         XTypeProvider,
                         XServiceInfo,
                         XContentProvider );

//=========================================================================
//
// XServiceInfo methods.
//
//=========================================================================

/*

 Pure virtual. Must be implemented by derived classes!

// virtual
OUString SAL_CALL ContentProviderImplHelper::getImplementationName()
    throw( RuntimeException )
{
}

//=========================================================================
// virtual
::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
ContentProviderImplHelper::getSupportedServiceNames()
    throw( RuntimeException )
{
}

*/

//=========================================================================
// virtual
sal_Bool SAL_CALL ContentProviderImplHelper::supportsService(
                                            const OUString& ServiceName )
    throw( RuntimeException )
{
    Sequence< OUString > aSNL = getSupportedServiceNames();
    const OUString* pArray = aSNL.getConstArray();
    for ( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
    {
        if ( pArray[ i ] == ServiceName )
            return sal_True;
    }

    return sal_False;
}

//=========================================================================
//
// XContentProvider methods.
//
//=========================================================================

/*

 Pure virtual. Must be implemented by derived classes!

// virtual
Reference< XContent > SAL_CALL ContentProviderImplHelper::queryContent(
                        const Reference< XContentIdentifier >& Identifier )
    throw( IllegalIdentifierException, RuntimeException )
{
}

*/

//=========================================================================
// virtual
sal_Int32 SAL_CALL ContentProviderImplHelper::compareContentIds(
                                const Reference< XContentIdentifier >& Id1,
                                const Reference< XContentIdentifier >& Id2 )
    throw( RuntimeException )
{
    // Simply do a string compare.

    OUString aURL1( Id1->getContentIdentifier() );
    OUString aURL2( Id2->getContentIdentifier() );

    return aURL1.compareTo( aURL2 );;
}

//=========================================================================
//
// Non-interface methods
//
//=========================================================================

void ContentProviderImplHelper::addContent(
                                    ::ucb::ContentImplHelper* pContent )
{
    vos::OGuard aGuard( m_aMutex );

    const OUString aURL( pContent->getIdentifier()->getContentIdentifier() );
    Contents::const_iterator it = m_pImpl->m_aContents.find( aURL );
    if ( it == m_pImpl->m_aContents.end() )
        m_pImpl->m_aContents[ aURL ] = pContent;
    else
        VOS_ENSURE( (*it).second == pContent,
                    "ContentProviderImplHelper::addContent - "
                    "Another content already registered for this URL!" );
}

//=========================================================================
void ContentProviderImplHelper::removeContent(
                                    ::ucb::ContentImplHelper* pContent )
{
    vos::OGuard aGuard( m_aMutex );

    const OUString aURL( pContent->getIdentifier()->getContentIdentifier() );
    removeContent( aURL );
}

//=========================================================================
void ContentProviderImplHelper::removeContent( const OUString& rURL )
{
    vos::OGuard aGuard( m_aMutex );

    Contents::iterator it = m_pImpl->m_aContents.find( rURL );

//  VOS_ENSURE( it != m_pImpl->m_aContents.end(),
//              "ContentProviderImplHelper::removeContent - Not registered!" );

    if ( it != m_pImpl->m_aContents.end() )
        m_pImpl->m_aContents.erase( it );
}

//=========================================================================
vos::ORef< ContentImplHelper >
ContentProviderImplHelper::queryExistingContent(
                        const Reference< XContentIdentifier >& Identifier )
{
    return queryExistingContent( Identifier->getContentIdentifier() );
}

//=========================================================================
vos::ORef< ContentImplHelper >
ContentProviderImplHelper::queryExistingContent( const OUString& rURL )
{
    vos::OGuard aGuard( m_aMutex );

    // Check, if a content with given id already exists...

    Contents::const_iterator it = m_pImpl->m_aContents.find( rURL );
    if ( it != m_pImpl->m_aContents.end() )
    {
        // Already there. Return it.
        return vos::ORef< ContentImplHelper >( (*it).second );
    }

    return vos::ORef< ContentImplHelper >();
}

//=========================================================================
void ContentProviderImplHelper::queryExistingContents(
                                            ContentRefList& rContents )
{
    vos::OGuard aGuard( m_aMutex );

    Contents::const_iterator it  = m_pImpl->m_aContents.begin();
    Contents::const_iterator end = m_pImpl->m_aContents.end();

    while ( it != end )
    {
        rContents.push_back( ContentImplHelperRef( (*it).second ) );
        ++it;
    }
}

//=========================================================================
Reference< XPropertySetRegistry >
ContentProviderImplHelper::getAdditionalPropertySetRegistry()
{
    // Get propertyset registry.

    vos::OGuard aGuard( m_aMutex );

    if ( !m_pImpl->m_xPropertySetRegistry.is() )
    {
        Reference< XPropertySetRegistryFactory > xRegFac(
                m_xSMgr->createInstance(
                    OUString::createFromAscii( "com.sun.star.ucb.Store" ) ),
                UNO_QUERY );

        VOS_ENSURE( xRegFac.is(),
                    "ContentProviderImplHelper::getAdditionalPropertySet - "
                    "No UCB-Store service!" );

        if ( xRegFac.is() )
        {
            // Open/create a registry.
            m_pImpl->m_xPropertySetRegistry
                            = xRegFac->createPropertySetRegistry( OUString() );

            VOS_ENSURE( m_pImpl->m_xPropertySetRegistry.is(),
                    "ContentProviderImplHelper::getAdditionalPropertySet - "
                    "Error opening registry!" );
        }
    }

    return m_pImpl->m_xPropertySetRegistry;
}


//=========================================================================
Reference< XPersistentPropertySet >
ContentProviderImplHelper::getAdditionalPropertySet( const OUString& rKey,
                                                     sal_Bool bCreate )
{
    // Get propertyset registry.
    getAdditionalPropertySetRegistry();

    if ( m_pImpl->m_xPropertySetRegistry.is() )
    {
        // Open/create persistent property set.
        return Reference< XPersistentPropertySet >(
                      m_pImpl->m_xPropertySetRegistry->openPropertySet(
                                                            rKey, bCreate ) );
    }

    return Reference< XPersistentPropertySet >();
}

//=========================================================================
sal_Bool ContentProviderImplHelper::renameAdditionalPropertySet(
                                                      const OUString& rOldKey,
                                                      const OUString& rNewKey,
                                                      sal_Bool bRecursive )
{
    if ( rOldKey == rNewKey )
        return sal_True;

    vos::OGuard aGuard( m_aMutex );

    if ( bRecursive )
    {
        // Get propertyset registry.
        getAdditionalPropertySetRegistry();

        if ( m_pImpl->m_xPropertySetRegistry.is() )
        {
            Reference< XNameAccess > xNameAccess(
                            m_pImpl->m_xPropertySetRegistry, UNO_QUERY );
            if ( xNameAccess.is() )
            {
                Sequence< OUString > aKeys = xNameAccess->getElementNames();
                sal_Int32 nCount = aKeys.getLength();
                if ( nCount > 0 )
                {
                    const OUString* pKeys = aKeys.getConstArray();
                    for ( sal_Int32 n = 0; n < nCount; ++n )
                    {
                        const OUString& rKey = pKeys[ n ];
                        if ( rKey.compareTo(
                                        rOldKey, rOldKey.getLength() ) == 0 )
                        {
                            OUString aNewKey
                                = rKey.replaceAt(
                                    0, rOldKey.getLength(), rNewKey );
                            if ( !renameAdditionalPropertySet(
                                    rKey, aNewKey, sal_False ) )
                                return sal_False;
                        }
                    }
                }
            }
            else
                return sal_False;
        }
        else
            return sal_False;
    }
    else
    {
        // Get old property set, if exists.
        Reference< XPersistentPropertySet > xOldSet
                        = getAdditionalPropertySet( rOldKey, sal_False );
        if ( xOldSet.is() )
        {
            // Rename property set.
            Reference< XNamed > xNamed( xOldSet, UNO_QUERY );
            if ( xNamed.is() )
            {
                // ??? throws no exceptions and has no return value ???
                xNamed->setName( rNewKey );
            }
            else
                return sal_False;
        }
    }
    return sal_True;
}

//=========================================================================
sal_Bool ContentProviderImplHelper::copyAdditionalPropertySet(
                                                  const OUString& rSourceKey,
                                                  const OUString& rTargetKey,
                                                  sal_Bool bRecursive )
{
    if ( rSourceKey == rTargetKey )
        return sal_True;

    vos::OGuard aGuard( m_aMutex );

    if ( bRecursive )
    {
        // Get propertyset registry.
        getAdditionalPropertySetRegistry();

        if ( m_pImpl->m_xPropertySetRegistry.is() )
        {
            Reference< XNameAccess > xNameAccess(
                            m_pImpl->m_xPropertySetRegistry, UNO_QUERY );
            if ( xNameAccess.is() )
            {
                Sequence< OUString > aKeys = xNameAccess->getElementNames();
                sal_Int32 nCount = aKeys.getLength();
                if ( nCount > 0 )
                {
                    const OUString* pKeys = aKeys.getConstArray();
                    for ( sal_Int32 n = 0; n < nCount; ++n )
                    {
                        const OUString& rKey = pKeys[ n ];
                        if ( rKey.compareTo(
                                    rSourceKey, rSourceKey.getLength() ) == 0 )
                        {
                            OUString aNewKey
                                = rKey.replaceAt(
                                    0, rSourceKey.getLength(), rTargetKey );
                            if ( !copyAdditionalPropertySet(
                                    rKey, aNewKey, sal_False ) )
                                return sal_False;
                        }
                    }
                }
            }
            else
                return sal_False;
        }
        else
            return sal_False;
    }
    else
    {
        // Get old property set, if exists.
        Reference< XPersistentPropertySet > xOldPropSet
                        = getAdditionalPropertySet( rSourceKey, sal_False );
        if ( !xOldPropSet.is() )
            return sal_False;

        Reference< XPropertySetInfo > xPropSetInfo =
                                        xOldPropSet->getPropertySetInfo();
        if ( !xPropSetInfo.is() )
            return sal_False;

        Reference< XPropertyAccess > xOldPropAccess( xOldPropSet, UNO_QUERY );
        if ( !xOldPropAccess.is() )
            return sal_False;

        // Obtain all values from old set.
        Sequence< PropertyValue > aValues = xOldPropAccess->getPropertyValues();
        sal_Int32 nCount = aValues.getLength();

        Sequence< Property > aProps = xPropSetInfo->getProperties();

        if ( nCount )
        {
            // Fail, if property set with new key already exists.
            Reference< XPersistentPropertySet > xNewPropSet
                        = getAdditionalPropertySet( rTargetKey, sal_False );
            if ( xNewPropSet.is() )
                return sal_False;

            // Create new, empty set.
            xNewPropSet = getAdditionalPropertySet( rTargetKey, sal_True );
            if ( !xNewPropSet.is() )
                return sal_False;

            Reference< XPropertyContainer > xNewPropContainer(
                                                xNewPropSet, UNO_QUERY );
            if ( !xNewPropContainer.is() )
                return sal_False;
/*
            Reference< XPropertyAccess > xNewPropAccess(
                                                xNewPropSet, UNO_QUERY );
            if ( !xNewPropAccess.is() )
                return sal_False;
*/
            for ( sal_Int32 n = 0; n < nCount; ++n )
            {
                const PropertyValue& rValue = aValues[ n ];

                sal_Int16 nAttribs = 0;
                for ( sal_Int32 m = 0; m < aProps.getLength(); ++m )
                {
                    if ( aProps[ m ].Name == rValue.Name )
                    {
                        nAttribs = aProps[ m ].Attributes;
                        break;
                    }
                }

                try
                {
                    xNewPropContainer->addProperty(
                                        rValue.Name, nAttribs, rValue.Value );
                }
                catch ( PropertyExistException & )
                {
                }
                   catch ( IllegalTypeException & )
                {
                }
                catch ( IllegalArgumentException & )
                {
                }
            }
/*
            try
            {
                xNewPropAccess->setPropertyValues( aValues );
            }
            catch( UnknownPropertyException& )
            {
            }
            catch( PropertyVetoException& )
            {
            }
            catch( IllegalArgumentException& )
            {
            }
            catch( WrappedTargetException& )
            {
            }
*/
        }
    }
    return sal_True;
}

//=========================================================================
sal_Bool ContentProviderImplHelper::removeAdditionalPropertySet(
                                                      const OUString& rKey,
                                                      sal_Bool bRecursive )
{
    vos::OGuard aGuard( m_aMutex );

    if ( bRecursive )
    {
        // Get propertyset registry.
        getAdditionalPropertySetRegistry();

        if ( m_pImpl->m_xPropertySetRegistry.is() )
        {
            Reference< XNameAccess > xNameAccess(
                            m_pImpl->m_xPropertySetRegistry, UNO_QUERY );
            if ( xNameAccess.is() )
            {
                Sequence< OUString > aKeys = xNameAccess->getElementNames();
                sal_Int32 nCount = aKeys.getLength();
                if ( nCount > 0 )
                {
                    const OUString* pKeys = aKeys.getConstArray();
                    for ( sal_Int32 n = 0; n < nCount; ++n )
                    {
                        const OUString& rCurrKey = pKeys[ n ];
                        if ( rCurrKey.compareTo( rKey, rKey.getLength() ) == 0 )
                        {
                            if ( !removeAdditionalPropertySet(
                                                        rCurrKey, sal_False ) )
                                return sal_False;
                        }
                    }
                }
            }
            else
                return sal_False;
        }
        else
            return sal_False;
    }
    else
    {
        // Get propertyset registry.
        getAdditionalPropertySetRegistry();

        if ( m_pImpl->m_xPropertySetRegistry.is() )
            m_pImpl->m_xPropertySetRegistry->removePropertySet( rKey );
        else
            return sal_False;
    }
    return sal_True;
}

