/*************************************************************************
 *
 *  $RCSfile: cmdmailsuppl.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obr $ $Date: 2001-06-27 06:30:38 $
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

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

#ifndef _CMDMAILSUPPL_HXX_
#include "cmdmailsuppl.hxx"
#endif

#ifndef _CMDMAILMSG_HXX_
#include "cmdmailmsg.hxx"
#endif

#ifndef _COM_SUN_STAR_SYSTEM_SIMPLEMAILCLIENTFLAGS_HPP_
#include <com/sun/star/system/SimpleMailClientFlags.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

//------------------------------------------------------------------------
// namespace directives
//------------------------------------------------------------------------

using com::sun::star::beans::PropertyValue;
using com::sun::star::system::XSimpleMailClientSupplier;
using com::sun::star::system::XSimpleMailClient;
using com::sun::star::system::XSimpleMailMessage;
using com::sun::star::container::XNameAccess;
using com::sun::star::container::NoSuchElementException;
using rtl::OUString;
using rtl::OUStringToOString;
using osl::MutexGuard;
using osl::FileBase;

using namespace cppu;
using namespace com::sun::star::system::SimpleMailClientFlags;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;

//------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------

#define COMP_IMPL_NAME  "com.sun.star.comp.system.SimpleCommandMail"

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

namespace // private
{
    Sequence< OUString > SAL_CALL Component_getSupportedServiceNames()
    {
        Sequence< OUString > aRet(1);
        aRet[0] = OUString::createFromAscii("com.sun.star.system.SimpleCommandMail");
        return aRet;
    }

} // end private namespace

//-------------------------------------------------
//
//-------------------------------------------------

CmdMailSuppl::CmdMailSuppl( const Reference< XMultiServiceFactory >& xServiceManager ) :
    WeakComponentImplHelper4< XSimpleMailClientSupplier, XSimpleMailClient, XEventListener, XServiceInfo >( m_aMutex ),
    m_xServiceManager( xServiceManager )
{
}

//-------------------------------------------------
// XSimpleMailClientSupplier
//-------------------------------------------------

Reference< XSimpleMailClient > SAL_CALL CmdMailSuppl::querySimpleMailClient(  )
    throw (RuntimeException)
{
    return static_cast < XSimpleMailClient * > (this);
}

//------------------------------------------------
// XSimpleMailClient
//------------------------------------------------

Reference< XSimpleMailMessage > SAL_CALL CmdMailSuppl::createSimpleMailMessage(  )
        throw (::com::sun::star::uno::RuntimeException)
{
    return Reference< XSimpleMailMessage >( new CmdMailMsg() );
}

//------------------------------------------------
// XSimpleMailClient
//------------------------------------------------

void SAL_CALL CmdMailSuppl::sendSimpleMailMessage( const Reference< XSimpleMailMessage >& xSimpleMailMessage, sal_Int32 aFlag )
    throw (IllegalArgumentException, Exception, RuntimeException)
{
    MutexGuard aGuard( m_aMutex );
    OUString aCommandLine;

    // create config manager if not already done so
    if( m_xServiceManager.is() && !m_xConfigurationProvider.is() )
    {
        try
        {
            m_xConfigurationProvider = Reference< XMultiServiceFactory > (
                m_xServiceManager->createInstance(
                    OUString::createFromAscii( "com.sun.star.configuration.ConfigurationProvider" ) ),
                UNO_QUERY );

            Reference< XComponent > xComponent =
                Reference< XComponent >( m_xConfigurationProvider, UNO_QUERY );

            if( xComponent.is() )
                xComponent->addEventListener( static_cast < XEventListener * > (this) );
        }

        // release service manager instance on runtime exceptions
        catch ( RuntimeException e )
        {
            m_xServiceManager.clear();
            OSL_TRACE( "ShellExec: can not instanciate configuration provider." );
            throw e;
        }
    }

    if( m_xConfigurationProvider.is() )
    {
        try
        {
            OUString aConfigRoot = OUString::createFromAscii( "org.openoffice.Office.Common/ExternalMailer" );

            PropertyValue aProperty;
            aProperty.Name = OUString::createFromAscii( "nodepath" );
            aProperty.Value = makeAny( aConfigRoot );

            Sequence< Any > aArgumentList( 1 );
            aArgumentList[0] = makeAny( aProperty );

            // query the configured external mail program
            Reference< XNameAccess > xNameAccess =
                Reference< XNameAccess > (
                    m_xConfigurationProvider->createInstanceWithArguments(
                        OUString::createFromAscii( "com.sun.star.configuration.ConfigurationAccess" ),
                        aArgumentList ),
                    UNO_QUERY );

            if( xNameAccess.is() )
            {
                OUString aProgram, aProfile;

                // save the registered program and commmand line profile
                xNameAccess->getByName( OUString::createFromAscii( "Program" ) ) >>= aProgram;
                xNameAccess->getByName( OUString::createFromAscii( "CommandProfile" ) ) >>= aProfile;

                if( aProgram.getLength() )
                {
                    // convert to file url
                    FileBase::getFileURLFromSystemPath( aProgram, aProgram );

                    OUString aProgramConfig = OUString::createFromAscii( "base" );

                    aConfigRoot += OUString::createFromAscii( "/Profiles/" );
                    aConfigRoot += aProfile;

                    // create name access to format strings
                    aProperty.Value = makeAny( aConfigRoot + OUString::createFromAscii( "/FormatStrings" ) );
                    aArgumentList[0] = makeAny( aProperty );

                    Reference< XNameAccess > xFormatStringAccess =
                        Reference< XNameAccess > (
                            m_xConfigurationProvider->createInstanceWithArguments(
                                OUString::createFromAscii( "com.sun.star.configuration.ConfigurationAccess" ),
                                aArgumentList ),
                            UNO_QUERY );

                    // create name access to enum delimiters
                    aProperty.Value = makeAny( aConfigRoot + OUString::createFromAscii( "/EnumDelimiters" ) );
                    aArgumentList[0] = makeAny( aProperty );

                    Reference< XNameAccess > xDelimiterAccess =
                        Reference< XNameAccess > (
                            m_xConfigurationProvider->createInstanceWithArguments(
                                OUString::createFromAscii( "com.sun.star.configuration.ConfigurationAccess" ),
                                aArgumentList ),
                            UNO_QUERY );

                    // query for XNameAccess
                    Reference< XNameAccess > xMessageAccess =
                        Reference< XNameAccess > ( xSimpleMailMessage, UNO_QUERY );

                    if( xFormatStringAccess.is() && xDelimiterAccess.is() && xMessageAccess.is() )
                    {
                        Sequence< OUString > aKeyList = xMessageAccess->getElementNames();

                        OUString aBaseDelimiter, aBaseParameter;
                        OUString aParameter;

                        // iterate over all mail parts
                        for( sal_Int32 n = 0, nmax = aKeyList.getLength(); n < nmax; n++ )
                        {
                            // first check if the key is also in FormatStrings list
                            if( xFormatStringAccess->hasByName( aKeyList[n] ) )
                            {
                                xFormatStringAccess->getByName( aKeyList[n] ) >>= aParameter;

                                // check if we have something to replace
                                sal_Int32 nIndex = aParameter.indexOf( OUString::createFromAscii( "%s" ) );
                                if( nIndex >= 0 )
                                {
                                    OUString aReplaceWithString;
                                    Any aValue = xMessageAccess->getByName( aKeyList[n] );

                                    // handle sequence of string as value
                                    if( aValue.getValueType() == getCppuType( ( Sequence< OUString > * ) NULL ) )
                                    {
                                        Sequence< OUString > aDataList;

                                        aValue >>= aDataList;
                                        aReplaceWithString = aDataList[0];

                                        // for multiple data we need a delimiter
                                        if( ( aDataList.getLength() > 1 ) && xDelimiterAccess->hasByName( aKeyList[n] ) )
                                        {
                                            OUString aDelimiter;
                                            xDelimiterAccess->getByName( aKeyList[n] ) >>= aDelimiter;

                                            for( sal_Int32 m = 1, mmax = aDataList.getLength(); m < mmax; m++ )
                                            {
                                                aReplaceWithString += aDelimiter;
                                                aReplaceWithString += aDataList[m];
                                            }
                                        }
                                    }

                                    // handle string as value
                                    else if( aValue.getValueType() == getCppuType( ( OUString * ) NULL ) )
                                    {
                                        aValue >>= aReplaceWithString;
                                    }

                                    if( aReplaceWithString.getLength() )
                                        aParameter = aParameter.replaceAt( nIndex, 2, aReplaceWithString );
                                    else
                                        aParameter = OUString();
                                }
                            }

                            // something to append ?
                            if( aParameter.getLength() )
                            {
                                if( aBaseParameter.getLength() )
                                {
                                    // do we have a delimiter for command line
                                    if( ( 0 == aBaseDelimiter.getLength() ) && xDelimiterAccess->hasByName( aProgramConfig ) )
                                    {
                                        xDelimiterAccess->getByName( aProgramConfig ) >>= aBaseDelimiter;
                                    }

                                    if( aBaseDelimiter.getLength() )
                                    {
                                        aBaseParameter += aBaseDelimiter;
                                        aBaseParameter += aParameter;
                                    }
                                }
                                else
                                    aBaseParameter = aParameter;

                            }
                        } // for()

                        xFormatStringAccess->getByName( aProgramConfig ) >>= aCommandLine;

                        // check if we have something to replace
                        sal_Int32 nIndex = aCommandLine.indexOf( OUString::createFromAscii( "%s" ) );
                        if( nIndex >= 0 )
                            aCommandLine = aCommandLine.replaceAt( nIndex, 2, aBaseParameter );

                    }  // xFormatStringAccess.is() && xDelimiterAccess.is() && xMessageAccess()

                    // split up parameters
                    Sequence< OUString > aArgumentList( 1 );
                    sal_Int32 nArguments = 1;

                    sal_Bool  bInQuote = sal_False;
                    sal_Int32 nLastIndex = 0;
                    const sal_Unicode *pBuffer = aCommandLine.getStr();

                    for( sal_Int32 n = 0, nmax = aCommandLine.getLength(); n < nmax; n++ )
                    {
                        if( (sal_Unicode) '"' == pBuffer[n] )
                        {
                            bInQuote = !bInQuote;
                        }
                        else if( (sal_Unicode) ' ' == pBuffer[n] )
                        {
                            if( !bInQuote )
                            {
                                // ignore multiple blanks
                                if( nLastIndex == n )
                                    nLastIndex++;
                                else
                                {
                                    // seem to have something to split
                                    aArgumentList.realloc( ++nArguments );
                                    aArgumentList[nArguments - 2] = aCommandLine.copy( nLastIndex, n - nLastIndex );
                                    nLastIndex = n + 1;
                                }
                            }
                        }
                    }

                    aArgumentList[nArguments - 1] = aCommandLine.copy( nLastIndex );

                    oslProcess aProcess;
                    rtl_uString **pArgumentArray = (rtl_uString **) aArgumentList.getArray();

                    osl_executeProcess( aProgram.pData,
                        pArgumentArray,
                        nArguments,
                        osl_Process_DETACHED | osl_Process_SEARCHPATH,
                        NULL,
                        NULL,
                        NULL,
                        0,
#if SUPD < 634
                        NULL,
#endif
                        &aProcess);
                }
            }
        }

        catch( NoSuchElementException e )
        {
            OSL_TRACE( "CmdMail: corrupted configuration.\n" );
            OSL_TRACE( OUStringToOString( e.Message, RTL_TEXTENCODING_ASCII_US ).getStr() );
        }

        catch( WrappedTargetException e )
        {
            OSL_TRACE( "CmdMail: unexspected exception.\n" );
            OSL_TRACE( OUStringToOString( e.Message, RTL_TEXTENCODING_ASCII_US ).getStr() );
        }

        catch( RuntimeException e )
        {
            m_xConfigurationProvider.clear();
            OSL_TRACE( "CmdMail: configuration provider." );
            OSL_TRACE( OUStringToOString( e.Message, RTL_TEXTENCODING_ASCII_US ).getStr() );
            throw e;
        }

        catch( Exception e )
        {
            OSL_TRACE( "CmdMail: unexspected exception.\n" );
            OSL_TRACE( OUStringToOString( e.Message, RTL_TEXTENCODING_ASCII_US ).getStr() );
        }
    }
}

//------------------------------------------------
// XEventListener
//------------------------------------------------

void SAL_CALL CmdMailSuppl::disposing( const ::com::sun::star::lang::EventObject& aEvent )
    throw(::com::sun::star::uno::RuntimeException)
{
    MutexGuard aGuard( m_aMutex );

    if( m_xServiceManager == aEvent.Source )
    {
        m_xServiceManager.clear();
    }

    else if( m_xConfigurationProvider == aEvent.Source )
    {
        m_xConfigurationProvider.clear();
    }
}

// -------------------------------------------------
// XServiceInfo
// -------------------------------------------------

OUString SAL_CALL CmdMailSuppl::getImplementationName(  )
    throw( RuntimeException )
{
    return OUString::createFromAscii( COMP_IMPL_NAME );
}

// -------------------------------------------------
//  XServiceInfo
// -------------------------------------------------

sal_Bool SAL_CALL CmdMailSuppl::supportsService( const OUString& ServiceName )
    throw( RuntimeException )
{
    Sequence < OUString > SupportedServicesNames = Component_getSupportedServiceNames();

    for ( sal_Int32 n = SupportedServicesNames.getLength(); n--; )
        if (SupportedServicesNames[n].compareTo(ServiceName) == 0)
            return sal_True;

    return sal_False;
}

// -------------------------------------------------
//  XServiceInfo
// -------------------------------------------------

Sequence< OUString > SAL_CALL CmdMailSuppl::getSupportedServiceNames(    )
    throw( RuntimeException )
{
    return Component_getSupportedServiceNames();
}

