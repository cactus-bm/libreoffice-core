/*************************************************************************
 *
 *  $RCSfile: ScriptSecurityManager.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: npower $ $Date: 2003-01-30 16:08:41 $
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

#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/util/XMacroExpander.hpp>
#include <com/sun/star/util/XStringSubstitution.hpp>
#include "ScriptSecurityManager.hxx"
#include <util/util.hxx>
#include <util/scriptingconstants.hxx>


using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
//using namespace ::drafts::com::sun::star::script::framework;

namespace scripting_securitymgr
{

static OUString s_configProv = ::rtl::OUString::createFromAscii(
    "com.sun.star.configuration.ConfigurationProvider");

static OUString s_configAccess = ::rtl::OUString::createFromAscii(
    "com.sun.star.configuration.ConfigurationAccess");

//*************************************************************************
// ScriptSecurityManager Constructor
ScriptSecurityManager::ScriptSecurityManager(
    const Reference< XComponentContext > & xContext ) throw ( RuntimeException )
    : m_xContext( xContext)
{
    OSL_TRACE( "< ScriptSecurityManager ctor called >\n" );
    validateXRef( m_xContext,
        "ScriptSecurityManager::ScriptSecurityManager: invalid context" );
    // test purposes only
    readConfiguration();
}

void ScriptSecurityManager::addScriptStorage( rtl::OUString url,
    sal_Int32 storageID)
{
    readConfiguration();
    StoragePerm newPerm;
    newPerm.url=url;
    newPerm.storageID=storageID;

    // we err on the side of caution!!
    newPerm.execPermission=false;
    switch( m_officeBasic )
    {
        case 0:         // never
            break;
        case 1:         // according to path list
            {
                // check path
                rtl::OUString path = url.copy( 0, url.lastIndexOf( '/' ) );
                for(int j=m_secureURL.getLength();j>0;j--)
                {
                    if( path.equals( m_secureURL[j-1] ) )
                    {
                        newPerm.execPermission=true;
                        break;
                    }
                }
                // confirm dialog
                break;
            }
        case 2:         // always
            newPerm.execPermission=true;
            break;
        default:
            //
            throw RuntimeException(
                OUSTR( "ScriptSecurityManager::addScriptStorage got invalid OfficeBasic setting"),
                Reference< XInterface > ());
    }
    m_permissionSettings.push_back(newPerm);
}
/**
 * checks to see whether the requested ScriptPeremission is allowed.
 * This was modelled after the Java AccessController, but at this time
 * we can't see a good reason not to return a bool, rather than throw
 * an exception if the request is not granted (as is the case in Java).
 */
sal_Bool ScriptSecurityManager::checkPermission( const OUString & scriptStorageURL,
    const OUString & permissionRequest )
    throw ( RuntimeException )
{
    if( permissionRequest.equals( OUString::createFromAscii( "execute" ) ) )
    {
        OSL_TRACE(
            "ScriptSecurityManager::checkPermission: execute permission request for %s",
            ::rtl::OUStringToOString( scriptStorageURL,
                RTL_TEXTENCODING_ASCII_US ).pData->buffer);
        ::std::vector< StoragePerm >::const_iterator iter;
        ::std::vector< StoragePerm >::const_iterator iterEnd =
            m_permissionSettings.end();
        for ( iter = m_permissionSettings.begin() ; iter != iterEnd; ++iter )
        {
            if ( iter->url.equals( scriptStorageURL ) )
            {
                // warning dialog if necessary
                return iter->execPermission;
            }
        }
        // we should never get here!!
        throw RuntimeException( OUString::createFromAscii( "ScriptSecurityManager::checkPermission: storageURL not found" ), Reference< XInterface > () );
    }
    else
        return sal_True;
}

void ScriptSecurityManager::readConfiguration()
    throw ( RuntimeException)
{
    // get the serice manager from the context
    Reference< lang::XMultiComponentFactory > xMgr = m_xContext->getServiceManager();
    validateXRef( xMgr,
        "ScriptSecurityManager::ScriptSecurityManager: cannot get ServiceManager" );
    // create an instance of the ConfigurationProvider
    Reference< XInterface > xInterface = xMgr->createInstanceWithContext(
        s_configProv, m_xContext );
    validateXRef( xInterface,
        "ScriptSecurityManager::ScriptSecurityManager: cannot get ConfigurationProvider" );
    beans::PropertyValue configPath;
    configPath.Name = ::rtl::OUString::createFromAscii( "nodepath" );
    configPath.Value <<= ::rtl::OUString::createFromAscii( "org.openoffice.Office.Common/Security/Scripting" );
    Sequence < Any > aargs( 1 );
    aargs[ 0 ] <<= configPath;
    // create an instance of the ConfigurationAccess for accessing the
    // scripting security settings
    Reference < lang::XMultiServiceFactory > xFactory( xInterface, UNO_QUERY );
    validateXRef( xFactory,
        "ScriptSecurityManager::ScriptSecurityManager: cannot get XMultiServiceFactory interface from ConfigurationProvider" );
    xInterface = xFactory->createInstanceWithArguments( s_configAccess,
            aargs );
    validateXRef( xInterface,
        "ScriptSecurityManager::ScriptSecurityManager: cannot get ConfigurationAccess" );
    // get the XPropertySet interface from the ConfigurationAccess service
    Reference < beans::XPropertySet > xPropSet( xInterface, UNO_QUERY );
    Any value;
    value=xPropSet->getPropertyValue( OUSTR( "Confirmation" ) );
    if ( sal_False == ( value >>= m_confirmationRequired ) )
    {
        throw RuntimeException(
            OUSTR( "ScriptSecurityManager: can't get Confirmation setting" ),
            Reference< XInterface > () );
    }
    if ( m_confirmationRequired == sal_True )
    {
        OSL_TRACE( "ScriptSecurityManager: confirmation is true" );
    }
    else
    {
        OSL_TRACE( "ScriptSecurityManager: confirmation is false" );
    }
    value=xPropSet->getPropertyValue( OUSTR( "Warning" ) );
    if ( sal_False == ( value >>= m_warning ) )
    {
        throw RuntimeException(
            OUSTR( "ScriptSecurityManager: can't get Warning setting" ),
            Reference< XInterface > () );
    }
    if ( m_warning == sal_True )
    {
        OSL_TRACE( "ScriptSecurityManager: warning is true" );
    }
    else
    {
        OSL_TRACE( "ScriptSecurityManager: warning is false" );
    }
    value=xPropSet->getPropertyValue( OUSTR( "OfficeBasic" ) );
    if ( sal_False == ( value >>= m_officeBasic ) )
    {
        throw RuntimeException(
            OUSTR( "ScriptSecurityManager: can't get OfficeBasic setting" ),
            Reference< XInterface > () );
    }
    OSL_TRACE( "ScriptSecurityManager: OfficeBasic = %d", m_officeBasic );
    value=xPropSet->getPropertyValue( OUSTR( "SecureURL" ) );
    if ( sal_False == ( value >>= m_secureURL ) )
    {
        throw RuntimeException(
            OUSTR( "ScriptSecurityManager: can't get SecureURL setting" ),
            Reference< XInterface > () );
    }
    // need debug output for contents of sequence
    for(int i=m_secureURL.getLength();i>0;i--)
    {
        OSL_TRACE( "ScriptSecurityManager: path = %s",
            ::rtl::OUStringToOString(m_secureURL[i-1] ,
            RTL_TEXTENCODING_ASCII_US ).pData->buffer  );

        xInterface = xMgr->createInstanceWithContext(
            ::rtl::OUString::createFromAscii(
            "com.sun.star.util.PathSubstitution"), m_xContext);
        validateXRef( xInterface,
            "ScriptSecurityManager::ScriptSecurityManager: cannot get ConfigurationProvider" );
        Reference< util::XStringSubstitution > xStringSubstitution(
            xInterface, UNO_QUERY);
        validateXRef( xStringSubstitution,
            "ScriptSecurityManager::ScriptSecurityManager: cannot get ConfigurationProvider" );
        OSL_TRACE( "ScriptSecurityManager: subpath = %s",
            ::rtl::OUStringToOString(
            xStringSubstitution->substituteVariables( m_secureURL[i-1], true ),
            RTL_TEXTENCODING_ASCII_US ).pData->buffer );
        m_secureURL[i-1] = xStringSubstitution->substituteVariables( m_secureURL[i-1], true );
    }
    for(int j=m_secureURL.getLength();j>0;j--)
    {
        OSL_TRACE( "ScriptSecurityManager: path = %s",
            ::rtl::OUStringToOString(m_secureURL[j-1] ,
            RTL_TEXTENCODING_ASCII_US ).pData->buffer  );
    }
}

//*************************************************************************
// ScriptSecurityManager Destructor
ScriptSecurityManager::~ScriptSecurityManager()
{
    OSL_TRACE( "< ScriptSecurityManager dtor called >\n" );
}

} // Namespace
