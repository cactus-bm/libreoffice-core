/*************************************************************************
 *
 *  $RCSfile: webdavcontent.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: kso $ $Date: 2001-11-26 09:45:37 $
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

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSETINFOCHANGE_HPP_
#include <com/sun/star/beans/PropertySetInfoChange.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSETINFOCHANGEEVENT_HPP_
#include <com/sun/star/beans/PropertySetInfoChangeEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XACTIVEDATASINK_HPP_
#include <com/sun/star/io/XActiveDataSink.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XOutputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALACCESSEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalAccessException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_CONTENTINFOATTRIBUTE_HPP_
#include <com/sun/star/ucb/ContentInfoAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INSERTCOMMANDARGUMENT_HPP_
#include <com/sun/star/ucb/InsertCommandArgument.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVEBADTRANSFRERURLEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveBadTransferURLException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVEAUGMENTEDIOEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveAugmentedIOException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVENETWORKCONNECTEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveNetworkConnectException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVENETWORKGENBERALEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveNetworkGeneralException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVENETWORKREADEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveNetworkReadException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVENETWORKRESOLVENAMEEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveNetworkResolveNameException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVENETWORKWRITEEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveNetworkWriteException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_NAMECLASH_HPP_
#include <com/sun/star/ucb/NameClash.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_NAMECLASHEXCEPTION_HPP_
#include <com/sun/star/ucb/NameClashException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_OPENCOMMANDARGUMENT2_HPP_
#include <com/sun/star/ucb/OpenCommandArgument2.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_OPENMODE_HPP_
#include <com/sun/star/ucb/OpenMode.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_POSTCOMMANDARGUMENT2_HPP_
#include <com/sun/star/ucb/PostCommandArgument2.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_TRANSFERINFO_HPP_
#include <com/sun/star/ucb/TransferInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDINFO_HPP_
#include <com/sun/star/ucb/XCommandInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XPERSISTENTPROPERTYSET_HPP_
#include <com/sun/star/ucb/XPersistentPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_MISSINGINPUTSTREAMEXCEPTION_HPP_
#include <com/sun/star/ucb/MissingInputStreamException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_MISSINGPROPERTIESEXCEPTION_HPP_
#include <com/sun/star/ucb/MissingPropertiesException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_UNSUPPORTEDCOMMANDEXCEPTION_HPP_
#include <com/sun/star/ucb/UnsupportedCommandException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_UNSUPPORTEDDATASINKEXCEPTION_HPP_
#include <com/sun/star/ucb/UnsupportedDataSinkException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_UNSUPPORTEDNAMECLASHEXCEPTION_HPP_
#include <com/sun/star/ucb/UnsupportedNameClashException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_UNSUPPORTEDOPENMODEEXCEPTION_HPP_
#include <com/sun/star/ucb/UnsupportedOpenModeException.hpp>
#endif

#ifndef _UCBHELPER_CONTENTIDENTIFIER_HXX
#include <ucbhelper/contentidentifier.hxx>
#endif
#ifndef _UCBHELPER_PROPERTYVALUESET_HXX
#include <ucbhelper/propertyvalueset.hxx>
#endif
#ifndef _UCBHELPER_INTERACTIONREQUEST_HXX
#include <ucbhelper/interactionrequest.hxx>
#endif
#ifndef _UCBHELPER_CANCELCOMMANDEXECUTION_HXX_
#include <ucbhelper/cancelcommandexecution.hxx>
#endif

#ifndef _WEBDAV_UCP_CONTENT_HXX
#include "webdavcontent.hxx"
#endif
#ifndef _WEBDAV_UCP_PROVIDER_HXX
#include "webdavprovider.hxx"
#endif
#ifndef _WEBDAV_UCP_RESULTSET_HXX
#include "webdavresultset.hxx"
#endif
#ifndef _WEBDAV_UCP_CONTENTPROPERTIES_HXX
#include "ContentProperties.hxx"
#endif
#ifndef _NEONURI_HXX_
#include "NeonUri.hxx"
#endif
#ifndef _UCBDEADPROPERTYVALUE_HXX_
#include "UCBDeadPropertyValue.hxx"
#endif

using namespace com::sun;
using namespace com::sun::star;
using namespace webdav_ucp;

//=========================================================================
//=========================================================================
//
// Content Implementation.
//
//=========================================================================
//=========================================================================

//=========================================================================
// ctr for content on an existing webdav resource
Content::Content(
          const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
          ContentProvider* pProvider,
          const uno::Reference< star::ucb::XContentIdentifier >& Identifier,
          rtl::Reference< DAVSessionFactory > const & rSessionFactory )
  throw ( star::ucb::ContentCreationException )
: ContentImplHelper( rxSMgr, pProvider, Identifier ),
  m_pProvider( pProvider ),
  m_bTransient( sal_False ),
  m_bCollection( sal_False )
{
    try
    {
        m_xResAccess.reset( new DAVResourceAccess(
            rxSMgr, rSessionFactory, Identifier->getContentIdentifier() ) );
    }
    catch ( DAVException const & )
    {
        throw star::ucb::ContentCreationException();
    }

    NeonUri aURI( Identifier->getContentIdentifier() );
       m_aEscapedTitle = aURI.GetPathBaseName();
}

//=========================================================================
// ctr for content on an non-existing webdav resource
Content::Content(
            const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
            ContentProvider* pProvider,
            const uno::Reference< star::ucb::XContentIdentifier >& Identifier,
            rtl::Reference< DAVSessionFactory > const & rSessionFactory,
            sal_Bool isCollection )
  throw ( star::ucb::ContentCreationException )
: ContentImplHelper( rxSMgr, pProvider, Identifier, sal_False ),
  m_pProvider( pProvider ),
  m_bTransient( sal_True ),
  m_bCollection( isCollection )
{
    try
    {
        m_xResAccess.reset( new DAVResourceAccess(
            rxSMgr, rSessionFactory, Identifier->getContentIdentifier() ) );
    }
    catch ( DAVException const & )
    {
        throw star::ucb::ContentCreationException();
    }

    // Do not set m_aEscapedTitle here! Content::insert relays on this!!!
}

//=========================================================================
// virtual
Content::~Content()
{
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

// virtual
void SAL_CALL Content::acquire()
    throw( )
{
    ContentImplHelper::acquire();
}

//=========================================================================
// virtual
void SAL_CALL Content::release()
    throw( )
{
    ContentImplHelper::release();
}

//=========================================================================
// virtual
uno::Any SAL_CALL Content::queryInterface( const uno::Type & rType )
    throw ( uno::RuntimeException )
{
    // Note: isFolder may require network activities! So call it only
    //       if it is really necessary!!!
    uno::Any aRet = cppu::queryInterface( rType,
                        static_cast< star::ucb::XContentCreator * >( this ) );
    if ( aRet.hasValue() )
        return isFolder( uno::Reference< star::ucb::XCommandEnvironment >() )
               ? aRet
               : uno::Any();

      return aRet.hasValue() ? aRet : ContentImplHelper::queryInterface( rType );
}

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

XTYPEPROVIDER_COMMON_IMPL( Content );

//=========================================================================
// virtual
uno::Sequence< uno::Type > SAL_CALL Content::getTypes()
    throw( uno::RuntimeException )
{
    static cppu::OTypeCollection* pCollection = NULL;

      if ( !pCollection )
    {
        osl::Guard< osl::Mutex > aGuard( osl::Mutex::getGlobalMutex() );
          if ( !pCollection )
        {
            if ( isFolder(
                    uno::Reference< star::ucb::XCommandEnvironment >() ) )
            {
                static cppu::OTypeCollection aCollection(
                        CPPU_TYPE_REF( lang::XTypeProvider ),
                        CPPU_TYPE_REF( lang::XServiceInfo ),
                        CPPU_TYPE_REF( lang::XComponent ),
                        CPPU_TYPE_REF( star::ucb::XContent ),
                        CPPU_TYPE_REF( star::ucb::XCommandProcessor ),
                        CPPU_TYPE_REF( beans::XPropertiesChangeNotifier ),
                        CPPU_TYPE_REF( star::ucb::XCommandInfoChangeNotifier ),
                        CPPU_TYPE_REF( beans::XPropertyContainer ),
                        CPPU_TYPE_REF( beans::XPropertySetInfoChangeNotifier ),
                        CPPU_TYPE_REF( container::XChild ),
                        CPPU_TYPE_REF( star::ucb::XContentCreator ) ); // !!
                  pCollection = &aCollection;
            }
              else
            {
                static cppu::OTypeCollection aCollection(
                        CPPU_TYPE_REF( lang::XTypeProvider ),
                        CPPU_TYPE_REF( lang::XServiceInfo ),
                        CPPU_TYPE_REF( lang::XComponent ),
                        CPPU_TYPE_REF( star::ucb::XContent ),
                        CPPU_TYPE_REF( star::ucb::XCommandProcessor ),
                        CPPU_TYPE_REF( beans::XPropertiesChangeNotifier ),
                        CPPU_TYPE_REF( star::ucb::XCommandInfoChangeNotifier ),
                        CPPU_TYPE_REF( beans::XPropertyContainer ),
                        CPPU_TYPE_REF( beans::XPropertySetInfoChangeNotifier ),
                        CPPU_TYPE_REF( container::XChild ) );
                  pCollection = &aCollection;
            }
        }
    }
      return (*pCollection).getTypes();
}

//=========================================================================
//
// XServiceInfo methods.
//
//=========================================================================

// virtual
rtl::OUString SAL_CALL Content::getImplementationName()
    throw( uno::RuntimeException )
{
    return rtl::OUString::createFromAscii(
                            "com.sun.star.comp.ucb.WebDAVContent" );
}

//=========================================================================
// virtual
uno::Sequence< rtl::OUString > SAL_CALL Content::getSupportedServiceNames()
    throw( uno::RuntimeException )
{
    uno::Sequence< rtl::OUString > aSNS( 1 );
    aSNS.getArray()[ 0 ]
            = rtl::OUString::createFromAscii( WEBDAV_CONTENT_SERVICE_NAME );
    return aSNS;
}

//=========================================================================
//
// XContent methods.
//
//=========================================================================

// virtual
rtl::OUString SAL_CALL Content::getContentType()
    throw( uno::RuntimeException )
{
    if ( isFolder( uno::Reference< star::ucb::XCommandEnvironment >() ) )
        return rtl::OUString::createFromAscii( WEBDAV_COLLECTION_TYPE );

    return rtl::OUString::createFromAscii( WEBDAV_CONTENT_TYPE );
}

//=========================================================================
//
// XCommandProcessor methods.
//
//=========================================================================

// virtual
uno::Any SAL_CALL Content::execute(
        const star::ucb::Command& aCommand,
        sal_Int32 CommandId,
        const uno::Reference< star::ucb::XCommandEnvironment >& Environment )
    throw( uno::Exception,
           star::ucb::CommandAbortedException,
           uno::RuntimeException )
{
    uno::Any aRet;

    if ( aCommand.Name.equalsAsciiL(
            RTL_CONSTASCII_STRINGPARAM( "getPropertyValues" ) ) )
    {
          //////////////////////////////////////////////////////////////////
          // getPropertyValues
          //////////////////////////////////////////////////////////////////

        uno::Sequence< beans::Property > Properties;
          if ( !( aCommand.Argument >>= Properties ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "Wrong argument type!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 ) ),
                Environment );
            // Unreachable
        }

          aRet <<= getPropertyValues( Properties, Environment );
    }
      else if ( aCommand.Name.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "setPropertyValues" ) ) )
    {
          //////////////////////////////////////////////////////////////////
          // setPropertyValues
          //////////////////////////////////////////////////////////////////

        uno::Sequence< beans::PropertyValue > aProperties;
          if ( !( aCommand.Argument >>= aProperties ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "Wrong argument type!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 ) ),
                Environment );
            // Unreachable
        }

          if ( !aProperties.getLength() )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "No properties!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 ) ),
                Environment );
            // Unreachable
        }

        aRet <<= setPropertyValues( aProperties, Environment );
    }
      else if ( aCommand.Name.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "getPropertySetInfo" ) ) )
       {
          //////////////////////////////////////////////////////////////////
          // getPropertySetInfo
          //////////////////////////////////////////////////////////////////

          // Note: Implemented by base class.
          aRet <<= getPropertySetInfo( Environment,
                                     sal_False /* don't cache data */ );
       }
      else if ( aCommand.Name.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "getCommandInfo" ) ) )
       {
          //////////////////////////////////////////////////////////////////
          // getCommandInfo
          //////////////////////////////////////////////////////////////////

          // Note: Implemented by base class.
          aRet <<= getCommandInfo( Environment, sal_False );
       }
    else if ( aCommand.Name.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "open" ) ) )
    {
        //////////////////////////////////////////////////////////////////
          // open
          //////////////////////////////////////////////////////////////////

        star::ucb::OpenCommandArgument2 aOpenCommand;
        if ( !( aCommand.Argument >>= aOpenCommand ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "Wrong argument type!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 ) ),
                Environment );
            // Unreachable
        }

        sal_Bool bOpenFolder =
            ( ( aOpenCommand.Mode == star::ucb::OpenMode::ALL ) ||
                ( aOpenCommand.Mode == star::ucb::OpenMode::FOLDERS ) ||
                ( aOpenCommand.Mode == star::ucb::OpenMode::DOCUMENTS ) );

        if ( bOpenFolder && isFolder( Environment ) )
        {
            // Open collection.

            uno::Reference< star::ucb::XDynamicResultSet > xSet
                = new DynamicResultSet(
                        m_xSMgr, this, aOpenCommand, Environment );
            aRet <<= xSet;
        }

        if ( aOpenCommand.Sink.is() )
        {
            // Open document.

            if ( ( aOpenCommand.Mode
                    == star::ucb::OpenMode::DOCUMENT_SHARE_DENY_NONE ) ||
                 ( aOpenCommand.Mode
                    == star::ucb::OpenMode::DOCUMENT_SHARE_DENY_WRITE ) )
            {
                // Currently(?) unsupported.
                ucbhelper::cancelCommandExecution(
                    uno::makeAny(
                        star::ucb::UnsupportedOpenModeException(
                                rtl::OUString(),
                                static_cast< cppu::OWeakObject * >( this ),
                                sal_Int16( aOpenCommand.Mode ) ) ),
                    Environment );
                // Unreachable
            }

            rtl::OUString aURL = m_xIdentifier->getContentIdentifier();
            uno::Reference< io::XOutputStream > xOut
                = uno::Reference< io::XOutputStream >(
                    aOpenCommand.Sink, uno::UNO_QUERY );
            if ( xOut.is() )
            {
                // PUSH: write data
                try
                {
                    m_xResAccess->GET( xOut, Environment );
                }
                catch ( DAVException const & e )
                {
                    cancelCommandExecution( e, Environment );
                    // Unreachable
                }
            }
            else
            {
                uno::Reference< io::XActiveDataSink > xDataSink
                    = uno::Reference< io::XActiveDataSink >(
                                        aOpenCommand.Sink, uno::UNO_QUERY );
                if ( xDataSink.is() )
                {
                    // PULL: wait for client read
                    try
                    {
                        uno::Reference< io::XInputStream > xIn
                            = m_xResAccess->GET( Environment );
                        xDataSink->setInputStream( xIn );
                    }
                    catch ( DAVException const & e )
                    {
                        cancelCommandExecution( e, Environment );
                        // Unreachable
                    }
                }
                else
                {
                    // Note: aOpenCommand.Sink may contain an XStream
                    //       implementation. Support for this type of
                    //       sink is optional...
                    ucbhelper::cancelCommandExecution(
                        uno::makeAny(
                            star::ucb::UnsupportedDataSinkException(
                                rtl::OUString(),
                                static_cast< cppu::OWeakObject * >( this ),
                                aOpenCommand.Sink ) ),
                        Environment );
                    // Unreachable
                }
            }
        }
    }
      else if ( aCommand.Name.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "insert" ) ) )
    {
        //////////////////////////////////////////////////////////////////
          // insert
          //////////////////////////////////////////////////////////////////

        star::ucb::InsertCommandArgument arg;
          if ( !( aCommand.Argument >>= arg ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "Wrong argument type!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 ) ),
                Environment );
            // Unreachable
        }

          insert( arg.Data, arg.ReplaceExisting, Environment );
    }
      else if ( aCommand.Name.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM( "delete" ) ) )
    {
          //////////////////////////////////////////////////////////////////
          // delete
          //////////////////////////////////////////////////////////////////

          sal_Bool bDeletePhysical = sal_False;
          aCommand.Argument >>= bDeletePhysical;

//  KSO: Ignore parameter and destroy the content, if you don't support
//       putting objects into trashcan. ( Since we do not have a trash can
//       service yet (src603), you actually have no other choice. )
//      if ( bDeletePhysical )
//      {
            try
            {
                m_xResAccess->DESTROY( Environment );
            }
            catch ( DAVException const & e )
            {
                cancelCommandExecution( e, Environment, sal_True );
                // Unreachable
            }
//      }

        // Propagate destruction.
        destroy( bDeletePhysical );

        // Remove own and all children's Additional Core Properties.
        removeAdditionalPropertySet( sal_True );
    }
    else if ( aCommand.Name.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "transfer" ) )
              && isFolder( Environment ) )
      {
        //////////////////////////////////////////////////////////////////
        // transfer
        //  ( Not available at documents )
          //////////////////////////////////////////////////////////////////

        star::ucb::TransferInfo transferArgs;
        if ( !( aCommand.Argument >>= transferArgs ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "Wrong argument type!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 ) ),
                Environment );
            // Unreachable
        }

        transfer( transferArgs, Environment );
    }
    else if ( aCommand.Name.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "post" ) ) )
      {
        //////////////////////////////////////////////////////////////////
        // post
          //////////////////////////////////////////////////////////////////

        star::ucb::PostCommandArgument2 aArg;
        if ( !( aCommand.Argument >>= aArg ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "Wrong argument type!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 ) ),
                Environment );
            // Unreachable
        }

        uno::Reference< io::XActiveDataSink > xSink(
                                        aArg.Sink, uno::UNO_QUERY );
        if ( xSink.is() )
        {
            try
            {
                uno::Reference< io::XInputStream > xResult
                    = m_xResAccess->POST( aArg.MediaType,
                                          aArg.Referer,
                                          aArg.Source,
                                          Environment );
                xSink->setInputStream( xResult );
            }
            catch ( DAVException const & e )
            {
                cancelCommandExecution( e, Environment, sal_True );
                // Unreachable
            }
        }
        else
        {
            uno::Reference< io::XOutputStream > xResult(
                                            aArg.Sink, uno::UNO_QUERY );
            if ( xResult.is() )
            {
                try
                {
                    m_xResAccess->POST( aArg.MediaType,
                                        aArg.Referer,
                                        aArg.Source,
                                        xResult,
                                        Environment );
                }
                catch ( DAVException const & e )
                {
                    cancelCommandExecution( e, Environment, sal_True );
                    // Unreachable
                }
            }
            else
            {
                ucbhelper::cancelCommandExecution(
                    uno::makeAny(
                        star::ucb::UnsupportedDataSinkException(
                            rtl::OUString(),
                            static_cast< cppu::OWeakObject * >( this ),
                            aArg.Sink ) ),
                    Environment );
                // Unreachable
            }
        }
    }
      else
    {
          //////////////////////////////////////////////////////////////////
          // Unsupported command
          //////////////////////////////////////////////////////////////////

        ucbhelper::cancelCommandExecution(
            uno::makeAny( star::ucb::UnsupportedCommandException(
                                rtl::OUString(),
                                static_cast< cppu::OWeakObject * >( this ) ) ),
            Environment );
        // Unreachable
    }

    return aRet;
}

//=========================================================================
// virtual
void SAL_CALL Content::abort( sal_Int32 CommandId )
    throw( uno::RuntimeException )
{
    // @@@ Implement logic to abort running commands, if this makes
    //     sense for your content.
}

//=========================================================================
//
// XPropertyContainer methods.
//
//=========================================================================

// virtual
void SAL_CALL Content::addProperty( const rtl::OUString& Name,
                                    sal_Int16 Attributes,
                                    const uno::Any& DefaultValue )
    throw( beans::PropertyExistException,
           beans::IllegalTypeException,
           lang::IllegalArgumentException,
           uno::RuntimeException )
{
//  if ( m_bTransient )
//      @@@ ???

//  osl::Guard< osl::Mutex > aGuard( m_aMutex );

    if ( !Name.getLength() )
        throw lang::IllegalArgumentException();

    // Check property type.
    if ( !UCBDeadPropertyValue::supportsType( DefaultValue.getValueType() ) )
    {
        OSL_ENSURE( sal_False, "Content::addProperty - "
                               "Unsupported property type!" );
        throw beans::IllegalTypeException();
    }

    //////////////////////////////////////////////////////////////////////
    // Make sure a property with the requested name does not already
    // exist in dynamic and static(!) properties.
    //////////////////////////////////////////////////////////////////////

    // @@@ Need real command environment here, but where to get it from?
    //     XPropertyContainer interface should be replaced by
    //     XCommandProcessor commands!
    uno::Reference< star::ucb::XCommandEnvironment > xEnv;

    // Note: This requires network access!
    if ( getPropertySetInfo( xEnv, sal_False /* don't cache data */ )
            ->hasPropertyByName( Name ) )
    {
        // Property does already exist.
        throw beans::PropertyExistException();
    }

    //////////////////////////////////////////////////////////////////////
    // Add a new dynamic property.
    //////////////////////////////////////////////////////////////////////

    ProppatchValue aValue( PROPSET, Name, DefaultValue );

    std::vector< ProppatchValue > aProppatchValues;
    aProppatchValues.push_back( aValue );

    try
    {
        // Set property value at server.
        m_xResAccess->PROPPATCH( aProppatchValues, xEnv );

        // Notify propertyset info change listeners.
        beans::PropertySetInfoChangeEvent evt(
                            static_cast< cppu::OWeakObject * >( this ),
                            Name,
                            -1, // No handle available
                            beans::PropertySetInfoChange::PROPERTY_INSERTED );
        notifyPropertySetInfoChange( evt );
    }
    catch ( DAVException const & e )
    {
        if ( e.getStatus() == 403 /* FORBIDDEN */ )
        {
            // Support for setting arbitrary dead properties is optional!

            // Store property locally.
            ContentImplHelper::addProperty( Name, Attributes, DefaultValue );
        }
        else
        {
            try
            {
                DAVCapabilities caps;
                m_xResAccess->OPTIONS( caps, xEnv );

                if ( caps.class1 )
                {
                    // DAV resource!
                    throw lang::IllegalArgumentException();
                }
                else
                {
                    // HTTP resource!

                    // Store property locally.
                    ContentImplHelper::addProperty(
                                            Name, Attributes, DefaultValue );
                }
            }
            catch ( DAVException const & )
            {
                throw lang::IllegalArgumentException();
            }
        }
    }
}

//=========================================================================
// virtual
void SAL_CALL Content::removeProperty( const rtl::OUString& Name )
    throw( beans::UnknownPropertyException,
           beans::NotRemoveableException,
           uno::RuntimeException )
{
//  osl::Guard< osl::Mutex > aGuard( m_aMutex );

    // @@@ Need real command environment here, but where to get it from?
    //     XPropertyContainer interface should be replaced by
    //     XCommandProcessor commands!
    uno::Reference< star::ucb::XCommandEnvironment > xEnv;

#if 0
    // @@@ REMOVEABLE z.Z. nicht richtig an der PropSetInfo gesetzt!!!
    try
    {
        beans::Property aProp
            = getPropertySetInfo( xEnv, sal_False /* don't cache data */ )
                ->getPropertyByName( Name );

        if ( !( aProp.Attributes & beans::PropertyAttribute::REMOVEABLE ) )
        {
            // Not removeable!
            throw beans::NotRemoveableException();
        }
    }
    catch ( beans::UnknownPropertyException const & )
    {
//      OSL_ENSURE( sal_False, "removeProperty - Unknown property!" );
        throw;
    }
#endif

    //////////////////////////////////////////////////////////////////////
    // Try to remove property from server.
    //////////////////////////////////////////////////////////////////////

    try
    {
        std::vector< ProppatchValue > aProppatchValues;
        ProppatchValue aValue( PROPREMOVE, Name, uno::Any() );
        aProppatchValues.push_back( aValue );

        // Remove property value from server.
        m_xResAccess->PROPPATCH( aProppatchValues, xEnv );

        // Notify propertyset info change listeners.
        beans::PropertySetInfoChangeEvent evt(
                            static_cast< cppu::OWeakObject * >( this ),
                            Name,
                            -1, // No handle available
                            beans::PropertySetInfoChange::PROPERTY_REMOVED );
        notifyPropertySetInfoChange( evt );
    }
    catch ( DAVException const & e )
    {
        if ( e.getStatus() == 403 /* FORBIDDEN */ )
        {
            // Support for setting arbitrary dead properties is optional!

            // Try to remove property from local store.
            ContentImplHelper::removeProperty( Name );
        }
        else
        {
            try
            {
                DAVCapabilities caps;
                m_xResAccess->OPTIONS( caps, xEnv );

                if ( caps.class1 )
                {
                    // DAV resource!
                    throw beans::UnknownPropertyException();
                }
                else
                {
                    // HTTP resource!

                    // Try to remove property from local store.
                    ContentImplHelper::removeProperty( Name );
                }
            }
            catch ( DAVException const & )
            {
                throw beans::UnknownPropertyException();
            }
        }
    }
}

//=========================================================================
//
// XContentCreator methods.
//
//=========================================================================

// virtual
uno::Sequence< star::ucb::ContentInfo > SAL_CALL
Content::queryCreatableContentsInfo()
    throw( uno::RuntimeException )
{
//  if ( isFolder() )
    {
        osl::Guard< osl::Mutex > aGuard( m_aMutex );

        uno::Sequence< star::ucb::ContentInfo > aSeq( 2 );

        // document.
        aSeq.getArray()[ 0 ].Type
            = rtl::OUString::createFromAscii( WEBDAV_CONTENT_TYPE );
        aSeq.getArray()[ 0 ].Attributes
            = star::ucb::ContentInfoAttribute::INSERT_WITH_INPUTSTREAM
              | star::ucb::ContentInfoAttribute::KIND_DOCUMENT;

        beans::Property aProp;
        m_pProvider->getProperty(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Title" ) ), aProp );

        uno::Sequence< beans::Property > aDocProps( 1 );
        aDocProps.getArray()[ 0 ] = aProp;
        aSeq.getArray()[ 0 ].Properties = aDocProps;

        // folder.
        aSeq.getArray()[ 1 ].Type
            = rtl::OUString::createFromAscii( WEBDAV_COLLECTION_TYPE );
        aSeq.getArray()[ 1 ].Attributes
            = star::ucb::ContentInfoAttribute::KIND_FOLDER;

        uno::Sequence< beans::Property > aFolderProps( 1 );
        aFolderProps.getArray()[ 0 ] = aProp;
        aSeq.getArray()[ 1 ].Properties = aFolderProps;
        return aSeq;
    }
/*
    else
    {
        OSL_ENSURE( sal_False,
                    "queryCreatableContentsInfo called on non-folder object!" );

        return Sequence< ContentInfo >( 0 );
    }
*/
}

//=========================================================================
// virtual
uno::Reference< star::ucb::XContent > SAL_CALL
Content::createNewContent( const star::ucb::ContentInfo& Info )
    throw( uno::RuntimeException )
{
//  if ( isFolder() )
    {
        osl::Guard< osl::Mutex > aGuard( m_aMutex );

          if ( !Info.Type.getLength() )
            return uno::Reference< star::ucb::XContent >();

          if ( ( !Info.Type.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM( WEBDAV_COLLECTION_TYPE ) ) )
                &&
                ( !Info.Type.equalsAsciiL(
                     RTL_CONSTASCII_STRINGPARAM( WEBDAV_CONTENT_TYPE ) ) ) )
            return uno::Reference< star::ucb::XContent >();

        rtl::OUString aURL = m_xIdentifier->getContentIdentifier();

          OSL_ENSURE( aURL.getLength() > 0,
                      "WebdavContent::createNewContent - empty identifier!" );

        if ( ( aURL.lastIndexOf( '/' ) + 1 ) != aURL.getLength() )
            aURL += rtl::OUString::createFromAscii( "/" );

          sal_Bool isCollection;
          if ( Info.Type.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( WEBDAV_COLLECTION_TYPE ) ) )
        {
            aURL += rtl::OUString::createFromAscii( "[New_Collection]" );
            isCollection = sal_True;
          }
          else
        {
            aURL += rtl::OUString::createFromAscii( "[New_Content]" );
            isCollection = sal_False;
          }

        uno::Reference< star::ucb::XContentIdentifier > xId(
                          new ::ucb::ContentIdentifier( m_xSMgr, aURL ) );

          // create the local content
          try
        {
            return new ::webdav_ucp::Content( m_xSMgr,
                                              m_pProvider,
                                                xId,
                                              m_xResAccess->getSessionFactory(),
                                                isCollection );
        }
        catch ( star::ucb::ContentCreationException & )
          {
            return uno::Reference< star::ucb::XContent >();
          }
    }
/*
      else
    {
        OSL_ENSURE( sal_False, "createNewContent called on non-folder object!" );
        return uno::Reference< star::ucb::XContent >();
    }
*/
}

//=========================================================================
// virtual
rtl::OUString Content::getParentURL()
{
    // <scheme>://              -> ""
    // <scheme>://foo           -> ""
    // <scheme>://foo/          -> ""
    // <scheme>://foo/bar       -> <scheme>://foo/
    // <scheme>://foo/bar/      -> <scheme>://foo/
    // <scheme>://foo/bar/abc   -> <scheme>://foo/bar/

    rtl::OUString aURL = m_xIdentifier->getContentIdentifier();

    sal_Int32 nPos = aURL.lastIndexOf( '/' );
    if ( nPos == ( aURL.getLength() - 1 ) )
    {
        // Trailing slash found. Skip.
        nPos = aURL.lastIndexOf( '/', nPos );
    }

    sal_Int32 nPos1 = aURL.lastIndexOf( '/', nPos );
    if ( nPos1 != -1 )
        nPos1 = aURL.lastIndexOf( '/', nPos1 );

    if ( nPos1 == -1 )
        return rtl::OUString();

    return rtl::OUString( aURL.copy( 0, nPos + 1 ) );
}

//=========================================================================
//
// Non-interface methods.
//
//=========================================================================

// static
uno::Reference< sdbc::XRow > Content::getPropertyValues(
           const uno::Reference< lang::XMultiServiceFactory >& rSMgr,
           const uno::Sequence< beans::Property >& rProperties,
           const ContentProperties& rData,
           const rtl::Reference< ::ucb::ContentProviderImplHelper >& rProvider,
           const rtl::OUString& rContentId )
{
      // Note: Empty sequence means "get values of all supported properties".

    rtl::Reference< ::ucb::PropertyValueSet > xRow
        = new ::ucb::PropertyValueSet( rSMgr );

      sal_Int32 nCount = rProperties.getLength();
      if ( nCount )
    {
        uno::Reference< beans::XPropertySet > xAdditionalPropSet;
          sal_Bool bTriedToGetAdditonalPropSet = sal_False;

        const beans::Property* pProps = rProperties.getConstArray();
          for ( sal_Int32 n = 0; n < nCount; ++n )
        {
            const beans::Property& rProp = pProps[ n ];

              // Process Core properties.

#if 0
              if ( rProp.Name.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM( "ContentType" ) ) )
               {
                if ( rData.pIsFolder && *rData.pIsFolder )
                    xRow->appendString( rProp, rtl::OUString::createFromAscii(
                                                      WEBDAV_COLLECTION_TYPE ) );
                else
                    xRow->appendString( rProp, rtl::OUString::createFromAscii(
                                                      WEBDAV_CONTENT_TYPE ) );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "Title" ) ) )
            {
                  xRow->appendString ( rProp, rData.aTitle );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "IsDocument" ) ) )
            {
                if ( rData.pIsDocument )
                      xRow->appendBoolean( rProp, *rData.pIsDocument );
                else
                      xRow->appendBoolean( rProp, sal_True );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "IsFolder" ) ) )
            {
                if ( rData.pIsFolder )
                      xRow->appendBoolean( rProp, *rData.pIsFolder );
                else
                      xRow->appendBoolean( rProp, sal_False );
            }
#else
              if ( rProp.Name.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM( "ContentType" ) ) )
               {
                if ( rData.pIsFolder && *rData.pIsFolder )
                    xRow->appendString( rProp, rtl::OUString::createFromAscii(
                                                      WEBDAV_COLLECTION_TYPE ) );
                else if ( rData.pIsDocument && *rData.pIsDocument )
                    xRow->appendString( rProp, rtl::OUString::createFromAscii(
                                                      WEBDAV_CONTENT_TYPE ) );
                else
                    xRow->appendVoid( rProp );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "Title" ) ) )
            {
                  xRow->appendString ( rProp, rData.aTitle );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "IsDocument" ) ) )
            {
                if ( rData.pIsDocument )
                      xRow->appendBoolean( rProp, *rData.pIsDocument );
                else
                    xRow->appendVoid( rProp );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "IsFolder" ) ) )
            {
                if ( rData.pIsFolder )
                      xRow->appendBoolean( rProp, *rData.pIsFolder );
                else
                    xRow->appendVoid( rProp );
            }
#endif
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "Size" ) ) )
              {
                if ( rData.pSize )
                      xRow->appendLong( rProp, *rData.pSize );
                else
                    xRow->appendVoid( rProp );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "DateCreated" ) ) )
              {
                if ( rData.pDateCreated )
                      xRow->appendTimestamp( rProp, *rData.pDateCreated );
                else
                    xRow->appendVoid( rProp );
            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "DateModified" ) ) )
              {
                if ( rData.pDateModified )
                      xRow->appendTimestamp( rProp, *rData.pDateModified );
                else
                    xRow->appendVoid( rProp );

            }
              else if ( rProp.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "MediaType" ) ) )
            {
                if ( rData.pgetcontenttype )
                      xRow->appendString( rProp, *rData.pgetcontenttype );
                else
                    xRow->appendVoid( rProp );
            }
              else if ( rProp.Name.equals( DAVProperties::CREATIONDATE ) )
            {
                if ( rData.pcreationdate )
                    xRow->appendString( rProp, *rData.pcreationdate );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::DISPLAYNAME ) )
            {
                if ( rData.pdisplayname )
                    xRow->appendString( rProp, *rData.pdisplayname );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::GETCONTENTLANGUAGE ) )
            {
                if ( rData.pgetcontentlanguage )
                    xRow->appendString( rProp, *rData.pgetcontentlanguage );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::GETCONTENTLENGTH ) )
            {
                if ( rData.pgetcontentlength )
                    xRow->appendString( rProp, *rData.pgetcontentlength );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::GETCONTENTTYPE ) )
            {
                if ( rData.pgetcontenttype )
                    xRow->appendString( rProp, *rData.pgetcontenttype );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::GETETAG ) )
            {
                if ( rData.pgetetag )
                    xRow->appendString( rProp, *rData.pgetetag );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::GETLASTMODIFIED ) )
            {
                if ( rData.pgetlastmodified )
                    xRow->appendString( rProp, *rData.pgetlastmodified );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::LOCKDISCOVERY ) )
            {
                if ( rData.plockdiscovery )
                    xRow->appendObject( rProp,
                                        uno::makeAny( *rData.plockdiscovery ) );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::RESOURCETYPE ) )
            {
                if ( rData.presourcetype )
                    xRow->appendObject( rProp,
                                        uno::makeAny( *rData.presourcetype ) );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::SOURCE ) )
            {
                if ( rData.psource )
                    xRow->appendObject( rProp, uno::makeAny( *rData.psource ) );
                else
                    xRow->appendVoid( rProp );
              }
              else if ( rProp.Name.equals( DAVProperties::SUPPORTEDLOCK ) )
            {
                if ( rData.psupportedlock )
                    xRow->appendObject( rProp,
                                        uno::makeAny( *rData.psupportedlock ) );
                else
                    xRow->appendVoid( rProp );
              }
            else
            {
                sal_Bool bAppened = sal_False;

                if ( rData.pOtherProps )
                {
                    // Process additional properties (DAV "dead" properties).
                    const PropertyValueMap::const_iterator it
                        = rData.pOtherProps->find( rProp.Name );
                    if ( it != rData.pOtherProps->end() )
                    {
                        xRow->appendObject( rProp, (*it).second );
                        bAppened = sal_True;
                    }
                }

                if ( !bAppened )
                {
                    // Process local additional properties.
                      if ( !bTriedToGetAdditonalPropSet
                           && !xAdditionalPropSet.is() )
                    {
                          xAdditionalPropSet
                            = uno::Reference< beans::XPropertySet >(
                                rProvider->getAdditionalPropertySet(
                                    rContentId, sal_False ),
                                uno::UNO_QUERY );
                          bTriedToGetAdditonalPropSet = sal_True;
                    }

                      if ( xAdditionalPropSet.is() )
                    {
                          if ( xRow->appendPropertySetValue(
                                         xAdditionalPropSet, rProp ) )
                            bAppened = sal_True;
                    }
                }

                if ( !bAppened )
                {
                      // Append empty entry.
                      xRow->appendVoid( rProp );
                }
            }
        }
    }
    else
    {
        // Append all Core Properties.
        ContentProvider * pProvider
            = static_cast< ContentProvider * >( rProvider.get() );
        beans::Property aProp;

#if 0
        pProvider->getProperty(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "ContentType" ) ), aProp );
        if ( rData.pIsFolder && *rData.pIsFolder )
            xRow->appendString( aProp, rtl::OUString::createFromAscii(
                                                    WEBDAV_COLLECTION_TYPE ) );
        else
            xRow->appendString( aProp, rtl::OUString::createFromAscii(
                                                      WEBDAV_CONTENT_TYPE ) );
        pProvider->getProperty(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Title" ) ), aProp );
          xRow->appendString( aProp, rData.aTitle );

        pProvider->getProperty(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "IsDocument" ) ), aProp );
        if ( rData.pIsDocument )
              xRow->appendBoolean( aProp, *rData.pIsDocument );
        else
              xRow->appendBoolean( aProp, sal_True );

        pProvider->getProperty(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsFolder" ) ), aProp );
        if ( rData.pIsFolder )
              xRow->appendBoolean( aProp,   *rData.pIsFolder );
        else
              xRow->appendBoolean( aProp, sal_False );
#else
        pProvider->getProperty(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Title" ) ), aProp );
          xRow->appendString( aProp, rData.aTitle );

        if ( rData.pIsFolder && *rData.pIsFolder )
        {
            pProvider->getProperty(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "ContentType" ) ), aProp );
            xRow->appendString( aProp, rtl::OUString::createFromAscii(
                                                    WEBDAV_COLLECTION_TYPE ) );
        }
        else if ( rData.pIsDocument && *rData.pIsDocument )
        {
            pProvider->getProperty(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "ContentType" ) ), aProp );
            xRow->appendString( aProp, rtl::OUString::createFromAscii(
                                                    WEBDAV_CONTENT_TYPE ) );
        }

        if ( rData.pIsDocument )
        {
            pProvider->getProperty(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "IsDocument" ) ), aProp );
              xRow->appendBoolean( aProp, *rData.pIsDocument );
        }

        if ( rData.pIsFolder )
        {
            pProvider->getProperty(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "IsFolder" ) ), aProp );
              xRow->appendBoolean( aProp,   *rData.pIsFolder );
        }
#endif
        if ( rData.pSize )
        {
            pProvider->getProperty(
                rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Size" ) ), aProp );
              xRow->appendLong( aProp, *rData.pSize );
        }

        if ( rData.pDateCreated )
        {
            pProvider->getProperty(
                rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DateCreated" ) ),
                aProp );
              xRow->appendTimestamp( aProp, *rData.pDateCreated );
        }

        if ( rData.pDateModified )
        {
            pProvider->getProperty(
                rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DateModified" ) ),
                aProp );
              xRow->appendTimestamp( aProp, *rData.pDateModified );
        }

        if ( rData.pgetcontenttype )
        {
            pProvider->getProperty(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM( "MediaType" ) ), aProp );
              xRow->appendString( aProp, *rData.pgetcontenttype );
        }

        if ( rData.pcreationdate )
        {
            pProvider->getProperty( DAVProperties::CREATIONDATE, aProp );
              xRow->appendString( aProp, *rData.pcreationdate );
        }

        if ( rData.pdisplayname )
        {
            pProvider->getProperty( DAVProperties::DISPLAYNAME, aProp );
              xRow->appendString( aProp, *rData.pdisplayname );
        }

        if ( rData.pgetcontentlanguage )
        {
            pProvider->getProperty( DAVProperties::GETCONTENTLANGUAGE, aProp );
              xRow->appendString( aProp, *rData.pgetcontentlanguage );
        }

        if ( rData.pgetcontentlength )
        {
            pProvider->getProperty( DAVProperties::GETCONTENTLENGTH, aProp );
              xRow->appendString( aProp, *rData.pgetcontentlength );
        }

        if ( rData.pgetcontenttype )
        {
            pProvider->getProperty( DAVProperties::GETCONTENTTYPE, aProp );
              xRow->appendString( aProp, *rData.pgetcontenttype );
        }

        if ( rData.pgetetag )
        {
            pProvider->getProperty( DAVProperties::GETETAG, aProp );
              xRow->appendString( aProp, *rData.pgetetag );
        }

        if ( rData.pgetlastmodified )
        {
            pProvider->getProperty( DAVProperties::GETLASTMODIFIED, aProp );
              xRow->appendString( aProp, *rData.pgetlastmodified );
        }

        if ( rData.plockdiscovery )
        {
            pProvider->getProperty( DAVProperties::LOCKDISCOVERY, aProp );
            xRow->appendObject( aProp, uno::makeAny( *rData.plockdiscovery ) );
        }

        if ( rData.presourcetype )
        {
            pProvider->getProperty( DAVProperties::RESOURCETYPE, aProp );
              xRow->appendString( aProp, *rData.presourcetype );
        }

        if ( rData.psource )
        {
            pProvider->getProperty( DAVProperties::SOURCE, aProp );
            xRow->appendObject( aProp, uno::makeAny( *rData.psource ) );
        }

        if ( rData.psupportedlock )
        {
            pProvider->getProperty( DAVProperties::SUPPORTEDLOCK, aProp );
            xRow->appendObject( aProp, uno::makeAny( *rData.psupportedlock ) );
        }

        // Process additional properties (DAV "dead" properties).
        if ( rData.pOtherProps )
        {
            PropertyValueMap::const_iterator it  = rData.pOtherProps->begin();
            PropertyValueMap::const_iterator end = rData.pOtherProps->end();

            beans::Property aProp;
            while ( it != end )
            {
                pProvider->getProperty( (*it).first, aProp );
                xRow->appendObject( aProp, (*it).second );
                it++;
            }
        }

        // Append all local Additional Core Properties.
        uno::Reference< beans::XPropertySet > xSet(
                 rProvider->getAdditionalPropertySet( rContentId, sal_False ),
                 uno::UNO_QUERY );
          xRow->appendPropertySet( xSet );
    }

    return uno::Reference< sdbc::XRow >( xRow.get() );
}

//=========================================================================
uno::Reference< sdbc::XRow > Content::getPropertyValues(
                const uno::Sequence< beans::Property >& rProperties,
                const uno::Reference< star::ucb::XCommandEnvironment >& xEnv )
{
    osl::Guard< osl::Mutex > aGuard( m_aMutex );

    if ( m_bTransient )
    {
        // No PROPFIND, but minimal local props.
        return getPropertyValues( m_xSMgr,
                                      rProperties,
                                        ContentProperties(
                                      NeonUri::unescape( m_aEscapedTitle ),
                                         m_bCollection ),
                                  rtl::Reference<
                                    ::ucb::ContentProviderImplHelper >(
                                        m_xProvider.getBodyPtr() ),
                                        m_xIdentifier->getContentIdentifier() );
    }

    // Only title requested? No PROPFIND necessary.
    if ( ( rProperties.getLength() == 1 )
         &&
         rProperties[ 0 ].Name.equalsAsciiL(
                            RTL_CONSTASCII_STRINGPARAM( "Title" ) ) )
    {
        return getPropertyValues( m_xSMgr,
                                      rProperties,
                                        ContentProperties(
                                      NeonUri::unescape( m_aEscapedTitle ),
                                         m_bCollection ),
                                  rtl::Reference<
                                    ::ucb::ContentProviderImplHelper >(
                                        m_xProvider.getBodyPtr() ),
                                        m_xIdentifier->getContentIdentifier() );
    }

    bool bSuccess = true;
    std::vector< rtl::OUString > aPropNames;
    ContentProperties::UCBNamesToDAVNames( rProperties, aPropNames );

    std::vector< DAVResource > resources;
    if ( aPropNames.size() > 0 )
    {
        try
        {
            m_xResAccess->PROPFIND( ZERO, aPropNames, resources, xEnv );
        }
        catch ( DAVException const & e )
        {
#if 1
            if ( ( e.getStatus() == 404 /* not found */ ) ||
                 ( e.getError() == DAVException::DAV_HTTP_LOOKUP ) )
            {
                // PROPFIND failed, only Title prop available.
                return getPropertyValues( m_xSMgr,
                                          rProperties,
                                          ContentProperties(
                                            NeonUri::unescape(
                                                m_aEscapedTitle ) ),
                                          rtl::Reference<
                                            ::ucb::ContentProviderImplHelper >(
                                                m_xProvider.getBodyPtr() ),
                                        m_xIdentifier->getContentIdentifier() );
            }
#endif

              bSuccess = false;
        }
    }

    bSuccess &= ( resources.size() == 1 );

    if ( bSuccess )
    {
        return getPropertyValues( m_xSMgr,
                                      rProperties,
                                        ContentProperties( resources[ 0 ] ),
                                  rtl::Reference<
                                    ::ucb::ContentProviderImplHelper >(
                                        m_xProvider.getBodyPtr() ),
                                        m_xIdentifier->getContentIdentifier() );
    }
    else
    {
        // PROPFIND failed, but minimal local props "available".
        return getPropertyValues( m_xSMgr,
                                      rProperties,
                                        ContentProperties(
                                      NeonUri::unescape( m_aEscapedTitle ),
                                         sal_False /* no collection */ ),
                                  rtl::Reference<
                                    ::ucb::ContentProviderImplHelper >(
                                        m_xProvider.getBodyPtr() ),
                                        m_xIdentifier->getContentIdentifier() );
    }
}

//=========================================================================
uno::Sequence< uno::Any > Content::setPropertyValues(
                const uno::Sequence< beans::PropertyValue >& rValues,
                const uno::Reference< star::ucb::XCommandEnvironment >& xEnv )
{
      osl::ClearableGuard< osl::Mutex > aGuard( m_aMutex );

    uno::Sequence< uno::Any > aRet( rValues.getLength() );
    uno::Sequence< beans::PropertyChangeEvent > aChanges( rValues.getLength() );
      sal_Int32 nChanged = 0;

    beans::PropertyChangeEvent aEvent;
    aEvent.Source         = static_cast< cppu::OWeakObject * >( this );
      aEvent.Further          = sal_False;
      // aEvent.PropertyName =
      aEvent.PropertyHandle = -1;
      // aEvent.OldValue     =
      // aEvent.NewValue   =

    sal_Bool bCheckedForDAV = sal_False;
    sal_Bool bDAV = sal_False;

    std::vector< ProppatchValue > aProppatchValues;
    std::vector< sal_Int32 > aProppatchPropsPositions;

    uno::Reference< star::ucb::XPersistentPropertySet > xAdditionalPropSet;
      sal_Bool bTriedToGetAdditonalPropSet = sal_False;

      sal_Bool bExchange = sal_False;
    rtl::OUString aNewTitle;
    rtl::OUString aOldTitle;
    sal_Int32 nTitlePos = -1;

    uno::Reference< beans::XPropertySetInfo > xInfo;

    const beans::PropertyValue* pValues = rValues.getConstArray();
      sal_Int32 nCount = rValues.getLength();
      for ( sal_Int32 n = 0; n < nCount; ++n )
      {
        const beans::PropertyValue& rValue = pValues[ n ];

        beans::Property aTmpProp;
        m_pProvider->getProperty( rValue.Name, aTmpProp );

        if ( aTmpProp.Attributes & beans::PropertyAttribute::READONLY )
        {
              // Read-only property!
            aRet[ n ] <<= lang::IllegalAccessException(
                            rtl::OUString::createFromAscii(
                                "Property is read-only!" ),
                            static_cast< cppu::OWeakObject * >( this ) );
            continue;
        }

        //////////////////////////////////////////////////////////////////
        // Mandatory props.
        //////////////////////////////////////////////////////////////////

        if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "ContentType" ) ) )
        {
            // Read-only property!
            aRet[ n ] <<= lang::IllegalAccessException(
                            rtl::OUString::createFromAscii(
                                "Property is read-only!" ),
                            static_cast< cppu::OWeakObject * >( this ) );
        }
        else if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "IsDocument" ) ) )
        {
            // Read-only property!
            aRet[ n ] <<= lang::IllegalAccessException(
                            rtl::OUString::createFromAscii(
                                "Property is read-only!" ),
                            static_cast< cppu::OWeakObject * >( this ) );
        }
        else if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "IsFolder" ) ) )
        {
            // Read-only property!
            aRet[ n ] <<= lang::IllegalAccessException(
                            rtl::OUString::createFromAscii(
                                "Property is read-only!" ),
                            static_cast< cppu::OWeakObject * >( this ) );
        }
        else if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "Title" ) ) )
        {
            rtl::OUString aNewValue;
            if ( rValue.Value >>= aNewValue )
              {
                // No empty titles!
                if ( aNewValue.getLength() > 0 )
                {
                    NeonUri aURI( m_xIdentifier->getContentIdentifier() );
                       aOldTitle = aURI.GetPathBaseNameUnescaped();

                    if ( aNewValue != aOldTitle )
                      {
                        // modified title -> modified URL -> exchange !
                        if ( !m_bTransient )
                            bExchange = sal_True;

                        // new value will be set later...
                        aNewTitle = aNewValue;

                        // remember position within sequence of values (for
                        // error handling).
                        nTitlePos = n;
                      }
                }
                else
                {
                    aRet[ n ] <<= lang::IllegalArgumentException(
                                    rtl::OUString::createFromAscii(
                                        "Empty title not allowed!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    -1 );
                }
            }
            else
            {
                aRet[ n ] <<= beans::IllegalTypeException(
                                rtl::OUString::createFromAscii(
                                    "Property value has wrong type!" ),
                                static_cast< cppu::OWeakObject * >( this ) );
            }
        }
        else
        {
            //////////////////////////////////////////////////////////////
            // Optional props.
            //////////////////////////////////////////////////////////////

            if ( !xInfo.is() )
                xInfo = getPropertySetInfo( xEnv,
                                            sal_False /* don't cache data */ );

            if ( !xInfo->hasPropertyByName( rValue.Name ) )
            {
                // Check, whether property exists. Skip otherwise.
                // PROPPATCH::set would add the property automatically, which
                // is not allowed for "setPropertyValues" command!
                aRet[ n ] <<= beans::UnknownPropertyException(
                                rtl::OUString::createFromAscii(
                                    "Property is unknown!" ),
                                static_cast< cppu::OWeakObject * >( this ) );
                continue;
            }

            if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "Size" ) ) )
            {
                // Read-only property!
                aRet[ n ] <<= lang::IllegalAccessException(
                                rtl::OUString::createFromAscii(
                                    "Property is read-only!" ),
                                static_cast< cppu::OWeakObject * >( this ) );
            }
            else if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "DateCreated" ) ) )
            {
                // Read-only property!
                aRet[ n ] <<= lang::IllegalAccessException(
                                rtl::OUString::createFromAscii(
                                    "Property is read-only!" ),
                                static_cast< cppu::OWeakObject * >( this ) );
            }
            else if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "DateModified" ) ) )
            {
                // Read-only property!
                aRet[ n ] <<= lang::IllegalAccessException(
                                rtl::OUString::createFromAscii(
                                    "Property is read-only!" ),
                                static_cast< cppu::OWeakObject * >( this ) );
            }
            else if ( rValue.Name.equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( "MediaType" ) ) )
            {
                // Read-only property!
                // (but could be writable, if 'getcontenttype' would be)
                aRet[ n ] <<= lang::IllegalAccessException(
                                rtl::OUString::createFromAscii(
                                    "Property is read-only!" ),
                                static_cast< cppu::OWeakObject * >( this ) );
            }
            else
            {
                if ( !bCheckedForDAV )
                {
                    bCheckedForDAV = sal_True;

                    try
                    {
                        DAVCapabilities caps;
                        m_xResAccess->OPTIONS( caps, xEnv );
                        bDAV = caps.class1;
                    }
                    catch ( DAVException const & )
                    {
                        bDAV = sal_False;
                    }
                }

                if ( bDAV )
                {
                    // Property value will be set on server.
                    ProppatchValue aValue( PROPSET, rValue.Name, rValue.Value );
                    aProppatchValues.push_back( aValue );

                    // remember position within sequence of values (for
                    // error handling).
                    aProppatchPropsPositions.push_back( n );
                }
                else
                {
                    // Property value will be stored in local property store.
                    if ( !bTriedToGetAdditonalPropSet &&
                         !xAdditionalPropSet.is() )
                    {
                        xAdditionalPropSet
                            = getAdditionalPropertySet( sal_False );
                        bTriedToGetAdditonalPropSet = sal_True;
                    }

                    if ( xAdditionalPropSet.is() )
                    {
                        try
                        {
                            uno::Any aOldValue
                                = xAdditionalPropSet->getPropertyValue(
                                                                rValue.Name );
                            if ( aOldValue != rValue.Value )
                            {
                                xAdditionalPropSet->setPropertyValue(
                                                rValue.Name, rValue.Value );

                                aEvent.PropertyName = rValue.Name;
                                aEvent.OldValue     = aOldValue;
                                aEvent.NewValue     = rValue.Value;

                                aChanges.getArray()[ nChanged ] = aEvent;
                                nChanged++;
                            }
                        }
                        catch ( beans::UnknownPropertyException const & e )
                        {
                            aRet[ n ] <<= e;
                        }
                        catch ( lang::WrappedTargetException const & e )
                        {
                            aRet[ n ] <<= e;
                        }
                        catch ( beans::PropertyVetoException const & e )
                        {
                            aRet[ n ] <<= e;
                        }
                        catch ( lang::IllegalArgumentException const & e )
                        {
                            aRet[ n ] <<= e;
                        }
                    }
                    else
                    {
                        aRet[ n ] <<= uno::Exception(
                                rtl::OUString::createFromAscii(
                                    "No property set for storing the value!" ),
                                static_cast< cppu::OWeakObject * >( this ) );
                    }
                }
            }
        }
    } // for

    if ( !m_bTransient && aProppatchValues.size() )
    {
        try
        {
            // Set property values at server.
            m_xResAccess->PROPPATCH( aProppatchValues, xEnv );

            std::vector< ProppatchValue >::const_iterator it
                = aProppatchValues.begin();
            std::vector< ProppatchValue >::const_iterator end
                = aProppatchValues.end();

            while ( it != end )
            {
                aEvent.PropertyName = (*it).name;
                aEvent.OldValue     = uno::Any(); // @@@ to expensive to obtain!
                aEvent.NewValue     = (*it).value;

                aChanges.getArray()[ nChanged ] = aEvent;
                nChanged++;

                ++it;
            }
        }
        catch ( DAVException const & e )
        {
//            OSL_ENSURE( sal_False,
//                        "Content::setPropertyValues - PROPPATCH failed!" );

            // Note: PROPPATCH either sets ALL property values OR NOTHING.

            std::vector< sal_Int32 >::const_iterator it
                = aProppatchPropsPositions.begin();
            std::vector< sal_Int32 >::const_iterator end
                = aProppatchPropsPositions.end();

            while ( it != end )
            {
                // Set error .
                aRet[ (*it) ] <<= MapDAVException( e, sal_True );
                ++it;
            }
        }
    }

      if ( bExchange )
      {
        // Assemble new content identifier...

        rtl::OUString aNewURL = getParentURL();
          if ( aNewURL.lastIndexOf( '/' ) != ( aNewURL.getLength() - 1 ) )
            aNewURL += rtl::OUString::createFromAscii( "/" );

          aNewURL += NeonUri::escapeSegment( aNewTitle );

        uno::Reference< star::ucb::XContentIdentifier > xNewId
            = new ::ucb::ContentIdentifier( m_xSMgr, aNewURL );
        uno::Reference< star::ucb::XContentIdentifier > xOldId
            = m_xIdentifier;

        try
        {
            NeonUri sourceURI( xOldId->getContentIdentifier() );
            NeonUri targetURI( xNewId->getContentIdentifier() );
            targetURI.SetScheme( rtl::OUString::createFromAscii( "http" ) );

            m_xResAccess->MOVE(
                sourceURI.GetPath(), targetURI.GetURI(), sal_False, xEnv );
            // @@@ Should check for resources that could not be moved
            //     (due to source access or target overwrite) and send
            //     this information through the interaction handler.

            // @@@ Existing content should be checked to see if it needs
            //     to be deleted at the source

            // @@@ Existing content should be checked to see if it has
            //     been overwritten at the target

            aGuard.clear();
            if ( exchangeIdentity( xNewId ) )
            {
                m_xResAccess->setURL( aNewURL );

// DAV resources store all additional props on server!
//              // Adapt Additional Core Properties.
//              renameAdditionalPropertySet( xOldId->getContentIdentifier(),
//                                           xNewId->getContentIdentifier(),
//                                           sal_True );
            }
            else
            {
                // Do not set new title!
                aNewTitle = rtl::OUString();

                // Set error .
                aRet[ nTitlePos ] <<= uno::Exception(
                        rtl::OUString::createFromAscii( "Exchange failed!" ),
                        static_cast< cppu::OWeakObject * >( this ) );
            }
        }
        catch ( DAVException const & e )
        {
            // Do not set new title!
            aNewTitle = rtl::OUString();

            // Set error .
            aRet[ nTitlePos ] <<= MapDAVException( e, sal_True );
        }
      }

    if ( aNewTitle.getLength() )
    {
        aEvent.PropertyName = rtl::OUString::createFromAscii( "Title" );
        aEvent.OldValue     = uno::makeAny( aOldTitle );
        aEvent.NewValue     = uno::makeAny( aNewTitle );

        m_aEscapedTitle = NeonUri::escapeSegment( aNewTitle );

        aChanges.getArray()[ nChanged ] = aEvent;
        nChanged++;
    }

    if ( nChanged > 0 )
    {
        aGuard.clear();
          aChanges.realloc( nChanged );
          notifyPropertiesChange( aChanges );
    }

    return aRet;
}

//=========================================================================
void Content::queryChildren( ContentRefList& rChildren )
{
    // Obtain a list with a snapshot of all currently instanciated contents
    // from provider and extract the contents which are direct children
    // of this content.

    ::ucb::ContentRefList aAllContents;
    m_xProvider->queryExistingContents( aAllContents );

    rtl::OUString aURL = m_xIdentifier->getContentIdentifier();
    sal_Int32 nPos = aURL.lastIndexOf( '/' );

    if ( nPos != ( aURL.getLength() - 1 ) )
    {
        // No trailing slash found. Append.
        aURL += rtl::OUString::createFromAscii( "/" );
    }

    sal_Int32 nLen = aURL.getLength();

    ::ucb::ContentRefList::const_iterator it  = aAllContents.begin();
    ::ucb::ContentRefList::const_iterator end = aAllContents.end();

    while ( it != end )
    {
        ::ucb::ContentImplHelperRef xChild = (*it);
        rtl::OUString aChildURL
            = xChild->getIdentifier()->getContentIdentifier();

        // Is aURL a prefix of aChildURL?
        if ( ( aChildURL.getLength() > nLen ) &&
             ( aChildURL.compareTo( aURL, nLen ) == 0 ) )
        {
            sal_Int32 nPos = nLen;
            nPos = aChildURL.indexOf( '/', nPos );

            if ( ( nPos == -1 ) ||
                 ( nPos == ( aChildURL.getLength() - 1 ) ) )
            {
                // No further slashes / only a final slash. It's a child!
                rChildren.push_back(
                    ::webdav_ucp::Content::ContentRef(
                        static_cast< ::webdav_ucp::Content * >(
                            xChild.getBodyPtr() ) ) );
            }
        }
        ++it;
    }
}

//=========================================================================
void Content::insert(
        const uno::Reference< io::XInputStream > & xInputStream,
        sal_Bool bReplaceExisting,
        const uno::Reference< star::ucb::XCommandEnvironment >& Environment )
    throw( uno::Exception )
{
    osl::ClearableGuard< osl::Mutex > aGuard( m_aMutex );

    // Check, if all required properties are present.

    if ( m_aEscapedTitle.getLength() == 0 )
    {
        OSL_ENSURE( sal_False, "Content::insert - Title missing!" );

        uno::Sequence< rtl::OUString > aProps( 1 );
        aProps[ 0 ] = rtl::OUString::createFromAscii( "Title" );
        ucbhelper::cancelCommandExecution(
            uno::makeAny( star::ucb::MissingPropertiesException(
                                rtl::OUString(),
                                static_cast< cppu::OWeakObject * >( this ),
                                aProps ) ),
            Environment );
        // Unreachable
    }

#if 0
    // This an API change that will be done in the future!!!

    /* [RFC 2616] - HTTP

        The PUT method requests that the enclosed entity be stored under the
        supplied Request-URI. If the Request-URI refers to an already
        existing resource, the enclosed entity SHOULD be considered as a
        modified version of the one residing on the origin server.
    */

    /* [RFC 2518] - WebDAV

        MKCOL creates a new collection resource at the location specified by
        the Request-URI.  If the resource identified by the Request-URI is
        non-null then the MKCOL MUST fail.
    */

    if ( !bReplaceExisting )
    {
        // Complain on PUT, continue on MKCOL.
        if ( !m_bTransient || ( m_bTransient && !m_bCollection  ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny(
                    star::ucb::UnsupportedNameClashException(
                                    rtl::OUString::createFromAscii(
                                        "Unable to write without overwrite!" ),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    star::ucb::NameClash::ERROR ) ),
                Environment );

            // Unreachable
        }
    }
#endif

    if ( m_bTransient )
    {
        // Assemble new content identifier...
        rtl::OUString aURL = getParentURL();
        if ( aURL.lastIndexOf( '/' ) != ( aURL.getLength() - 1 ) )
            aURL += rtl::OUString::createFromAscii( "/" );

        aURL += m_aEscapedTitle;

        try
        {
            m_xResAccess->setURL( aURL );

            if ( m_bCollection )
                m_xResAccess->MKCOL( Environment );
            else
                m_xResAccess->PUT( xInputStream, Environment );
          }
        catch ( DAVException const & e )
        {
            if ( m_bCollection )
            {
                if ( e.getStatus() == 405 )
                {
                    // [RFC 2518] - WebDAV
                    // 405 (Method Not Allowed) - MKCOL can only be
                    // executed on a deleted/non-existent resource.

                    if ( bReplaceExisting )
                    {
                        // Destroy old resource.
                        try
                        {
                            m_xResAccess->DESTROY( Environment );
                        }
                        catch ( DAVException const & e )
                        {
                            cancelCommandExecution( e, Environment, sal_True );
                            // Unreachable
                        }

                        // Insert (recursion!).
                        insert( xInputStream, bReplaceExisting, Environment );

                        // Success!
                        return;
                    }
                    else
                    {
                        NeonUri aURI( aURL );
                        const rtl::OUString aTitle
                            = aURI.GetPathBaseNameUnescaped();

                        ucbhelper::cancelCommandExecution(
                            uno::makeAny(
                                star::ucb::NameClashException(
                                    rtl::OUString(),
                                    static_cast< cppu::OWeakObject * >( this ),
                                    task::InteractionClassification_ERROR,
                                    aTitle ) ),
                            Environment );
                        // Unreachable
                    }
                }
            }

            cancelCommandExecution( e, Environment, sal_True );
            // Unreachable
          }

        m_xIdentifier = new ::ucb::ContentIdentifier( m_xSMgr, aURL );

        aGuard.clear();
          inserted();
          m_bTransient = sal_False;
    }
    else
    {
        if ( !xInputStream.is() )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny(
                    star::ucb::MissingInputStreamException(
                                rtl::OUString(),
                                static_cast< cppu::OWeakObject * >( this ) ) ),
                Environment );
            // Unreachable
          }

        try
        {
            m_xResAccess->PUT( xInputStream, Environment );
          }
        catch ( DAVException const & e )
        {
            cancelCommandExecution( e, Environment, sal_True );
            // Unreachable
          }
    }
}

//=========================================================================
void Content::transfer(
        const star::ucb::TransferInfo & rArgs,
        const uno::Reference< star::ucb::XCommandEnvironment >& Environment )
    throw( uno::Exception )
{
    NeonUri sourceURI( rArgs.SourceURL );
    NeonUri targetURI( m_xIdentifier->getContentIdentifier() );

    try
    {
        // Check source's and target's URL scheme
        //
        const rtl::OUString aScheme = sourceURI.GetScheme().toAsciiLowerCase();
        if ( aScheme.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( WEBDAV_URL_SCHEME ) ) )
        {
            sourceURI.SetScheme(
                rtl::OUString::createFromAscii( HTTP_URL_SCHEME ) );
        }
        else
        {
            if ( !aScheme.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM( HTTP_URL_SCHEME ) ) &&
                 !aScheme.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM( HTTPS_URL_SCHEME ) ) )
            {
                ucbhelper::cancelCommandExecution(
                    uno::makeAny(
                        star::ucb::InteractiveBadTransferURLException(
                            rtl::OUString::createFromAscii(
                                "Unsupported URL scheme!" ),
                            static_cast< cppu::OWeakObject * >( this ) ) ),
                    Environment );
                // Unreachable
            }
        }

        if ( targetURI.GetScheme().toAsciiLowerCase().equalsAsciiL(
                        RTL_CONSTASCII_STRINGPARAM( WEBDAV_URL_SCHEME ) ) )
            targetURI.SetScheme(
                rtl::OUString::createFromAscii( HTTP_URL_SCHEME ) );

        // @@@ This implementation of 'transfer' only works
        //     if the source and target are located at same host.
        //     (Neon does not support cross-server copy/move)

        // Check for same host
        //
        if ( sourceURI.GetHost().getLength() &&
             ( sourceURI.GetHost() != targetURI.GetHost() ) )
        {
            ucbhelper::cancelCommandExecution(
                uno::makeAny( star::ucb::InteractiveBadTransferURLException(
                                rtl::OUString::createFromAscii(
                                    "Different hosts!" ),
                                static_cast< cppu::OWeakObject * >( this ) ) ),
                Environment );
            // Unreachable
        }

        rtl::OUString aTitle = rArgs.NewTitle;

        if ( !aTitle.getLength() )
            aTitle = sourceURI.GetPathBaseNameUnescaped();

        if ( aTitle.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "/" ) ) )
        {
            // kso: ???
            aTitle = rtl::OUString();
        }

        targetURI.AppendPath( aTitle );

        rtl::OUString aTargetURL = m_xIdentifier->getContentIdentifier();
        if ( ( aTargetURL.lastIndexOf( '/' ) + 1 )
                != aTargetURL.getLength() )
            aTargetURL += rtl::OUString::createFromAscii( "/" );

        aTargetURL += aTitle;

        uno::Reference< star::ucb::XContentIdentifier > xTargetId
            = new ::ucb::ContentIdentifier( m_xSMgr, aTargetURL );

        DAVResourceAccess aSourceAccess( m_xSMgr,
                                         m_xResAccess->getSessionFactory(),
                                         sourceURI.GetURI() );

        if ( rArgs.MoveData == sal_True )
        {
            uno::Reference< star::ucb::XContentIdentifier > xId
                = new ::ucb::ContentIdentifier( m_xSMgr, rArgs.SourceURL );

            // Note: The static cast is okay here, because its sure that
            //       m_xProvider is always the WebDAVContentProvider.
            rtl::Reference< Content > xSource
                = static_cast< Content * >(
                    m_xProvider->queryContent( xId ).get() );

            // [RFC 2518] - WebDAV
            // If a resource exists at the destination and the Overwrite
            // header is "T" then prior to performing the move the server
            // MUST perform a DELETE with "Depth: infinity" on the
            // destination resource.  If the Overwrite header is set to
            // "F" then the operation will fail.

            aSourceAccess.MOVE( sourceURI.GetPath(),
                                targetURI.GetURI(),
                                rArgs.NameClash
                                    == star::ucb::NameClash::OVERWRITE,
                                Environment );

            if ( xSource.is() )
            {
                // Propagate destruction to listeners.
                xSource->destroy( sal_True );
            }

// DAV resources store all additional props on server!
//              // Rename own and all children's Additional Core Properties.
//              renameAdditionalPropertySet( xId->getContentIdentifier(),
//                                           xTargetId->getContentIdentifier(),
//                                           sal_True );
        }
        else
        {
            // [RFC 2518] - WebDAV
            // If a resource exists at the destination and the Overwrite
            // header is "T" then prior to performing the copy the server
            // MUST perform a DELETE with "Depth: infinity" on the
            // destination resource.  If the Overwrite header is set to
            // "F" then the operation will fail.

            aSourceAccess.COPY( sourceURI.GetPath(),
                                targetURI.GetURI(),
                                rArgs.NameClash
                                    == star::ucb::NameClash::OVERWRITE,
                                Environment );

// DAV resources store all additional props on server!
//              // Copy own and all children's Additional Core Properties.
//              copyAdditionalPropertySet( xId->getContentIdentifier(),
//                                         xTargetId->getContentIdentifier(),
//                                         sal_True );
        }

        // Note: The static cast is okay here, because its sure that
        //       m_xProvider is always the WebDAVContentProvider.
        rtl::Reference< Content > xTarget
            = static_cast< Content * >(
                    m_xProvider->queryContent( xTargetId ).get() );

        // Announce transfered content in its new folder.
        xTarget->inserted();
    }
    catch ( star::ucb::IllegalIdentifierException const & )
    {
        // queryContent
    }
    catch ( DAVException const & e )
    {
        // [RFC 2518] - WebDAV
        // 412 (Precondition Failed) - The server was unable to maintain
        // the liveness of the properties listed in the propertybehavior
        // XML element or the Overwrite header is "F" and the state of
        // the destination resource is non-null.

        if ( e.getStatus() == 412 )
        {
            switch ( rArgs.NameClash )
            {
                case star::ucb::NameClash::ERROR:
                {
                    ucbhelper::cancelCommandExecution(
                        uno::makeAny(
                            star::ucb::NameClashException(
                                rtl::OUString(),
                                static_cast< cppu::OWeakObject * >( this ),
                                task::InteractionClassification_ERROR,
                                targetURI.GetPathBaseNameUnescaped() ) ),
                        Environment );
                    // Unreachable
                }

                case star::ucb::NameClash::OVERWRITE:
                    break;

                case star::ucb::NameClash::KEEP: // deprecated
                case star::ucb::NameClash::RENAME:
                case star::ucb::NameClash::ASK:
                default:
                {
                    ucbhelper::cancelCommandExecution(
                        uno::makeAny(
                            star::ucb::UnsupportedNameClashException(
                                rtl::OUString(),
                                static_cast< cppu::OWeakObject * >( this ),
                                rArgs.NameClash ) ),
                        Environment );
                    // Unreachable
                }
            }
        }

        cancelCommandExecution( e, Environment, sal_True );
        // Unreachable
    }
}

//=========================================================================
void Content::destroy( sal_Bool bDeletePhysical )
    throw( uno::Exception )
{
    // @@@ take care about bDeletePhysical -> trashcan support
    rtl::OUString aURL = m_xIdentifier->getContentIdentifier();

    uno::Reference< star::ucb::XContent > xThis = this;

    deleted();

    osl::Guard< osl::Mutex > aGuard( m_aMutex );

    // Process instanciated children...

    ::webdav_ucp::Content::ContentRefList aChildren;
    queryChildren( aChildren );

    ContentRefList::const_iterator it  = aChildren.begin();
    ContentRefList::const_iterator end = aChildren.end();

    while ( it != end )
    {
        (*it)->destroy( bDeletePhysical );
        ++it;
    }
}

//=========================================================================
sal_Bool Content::exchangeIdentity(
    const uno::Reference< star::ucb::XContentIdentifier >& xNewId )
{
    if ( !xNewId.is() )
        return sal_False;

    osl::ClearableGuard< osl::Mutex > aGuard( m_aMutex );

    uno::Reference< star::ucb::XContent > xThis = this;

    // Already persistent?
    if ( m_bTransient )
    {
        OSL_ENSURE( sal_False, "Content::exchangeIdentity - Not persistent!" );
        return sal_False;
    }

    // Exchange own identitity.

    // Fail, if a content with given id already exists.
//  if ( !hasData( xNewId ) )
    {
        rtl::OUString aOldURL = m_xIdentifier->getContentIdentifier();

        aGuard.clear();
        if ( exchange( xNewId ) )
        {
            // Process instanciated children...

            ContentRefList aChildren;
            queryChildren( aChildren );

            ContentRefList::const_iterator it  = aChildren.begin();
            ContentRefList::const_iterator end = aChildren.end();

            while ( it != end )
            {
                ContentRef xChild = (*it);

                // Create new content identifier for the child...
                uno::Reference< star::ucb::XContentIdentifier >
                    xOldChildId = xChild->getIdentifier();
                rtl::OUString aOldChildURL
                    = xOldChildId->getContentIdentifier();
                rtl::OUString aNewChildURL
                        = aOldChildURL.replaceAt(
                                        0,
                                        aOldURL.getLength(),
                                        xNewId->getContentIdentifier() );
                uno::Reference< star::ucb::XContentIdentifier >
                    xNewChildId
                        = new ::ucb::ContentIdentifier( m_xSMgr, aNewChildURL );

                if ( !xChild->exchangeIdentity( xNewChildId ) )
                    return sal_False;

                ++it;
            }
            return sal_True;
        }
    }

    OSL_ENSURE( sal_False,
                "Content::exchangeIdentity - "
                "Panic! Cannot exchange identity!" );
    return sal_False;
}

//=========================================================================
sal_Bool Content::isFolder(
            const uno::Reference< star::ucb::XCommandEnvironment >& xEnv )
{
    if ( m_bTransient )
    {
        return m_bCollection;
    }
    else
    {
        std::vector< rtl::OUString > aPropNames;
        aPropNames.push_back( DAVProperties::RESOURCETYPE );

        std::vector< DAVResource > resources;
        try
        {
            m_xResAccess->PROPFIND( ZERO, aPropNames, resources, xEnv );
        }
        catch ( DAVException const & )
        {
          }

        if ( resources.size() != 1 )
            return sal_False;

        ContentProperties aContentProperties( resources[ 0 ] );
        return ( aContentProperties.pIsFolder
                    && *aContentProperties.pIsFolder );
    }
}

//=========================================================================
uno::Any Content::MapDAVException( const DAVException & e, sal_Bool bWrite )
{
    // Map DAVException...
    uno::Any aException;

    switch ( e.getStatus() )
    {
        case 404: // Not Found
        {
            uno::Sequence< uno::Any > aArgs( 1 );
            aArgs[ 0 ] <<= m_xIdentifier->getContentIdentifier();

            aException <<=
                star::ucb::InteractiveAugmentedIOException(
                    rtl::OUString::createFromAscii( "Not found!" ),
                    static_cast< cppu::OWeakObject * >( this ),
                    task::InteractionClassification_ERROR,
                    star::ucb::IOErrorCode_NOT_EXISTING,
                    aArgs );
            return aException;
        }

        default:
            break;
    }

    switch ( e.getError() )
    {
        case DAVException::DAV_HTTP_ERROR:
        {
            if ( bWrite )
                aException <<=
                    star::ucb::InteractiveNetworkWriteException(
                            rtl::OUString(),
                            static_cast< cppu::OWeakObject * >( this ),
                            task::InteractionClassification_ERROR,
                            e.getData() );
            else
                aException <<=
                    star::ucb::InteractiveNetworkReadException(
                            rtl::OUString(),
                            static_cast< cppu::OWeakObject * >( this ),
                            task::InteractionClassification_ERROR,
                            e.getData() );
            break;
        }

        case DAVException::DAV_HTTP_LOOKUP:
            aException <<=
                star::ucb::InteractiveNetworkResolveNameException(
                            rtl::OUString(),
                            static_cast< cppu::OWeakObject * >( this ),
                            task::InteractionClassification_ERROR,
                            e.getData() );
            break;

//        case DAVException::DAV_HTTP_AUTH:
//            break;
//
//        case DAVException::DAV_HTTP_AUTHPROXY:
//            break;
//
//        case DAVException::DAV_HTTP_SERVERAUTH:
//            break;
//
//        case DAVException::DAV_HTTP_PROXYAUTH:
//            break;

        case DAVException::DAV_HTTP_CONNECT:
            aException <<=
                star::ucb::InteractiveNetworkConnectException(
                            rtl::OUString(),
                            static_cast< cppu::OWeakObject * >( this ),
                            task::InteractionClassification_ERROR,
                            e.getData() );
            break;

/*
        case DAVException::DAV_HTTP_TIMEOUT:
            break;
*/

//        case DAVException::DAV_HTTP_REDIRECT:
//            break;

//        case DAVException::DAV_SESSION_CREATE:
//            break;

        case DAVException::DAV_INVALID_ARG:
            aException <<=
                lang::IllegalArgumentException(
                            rtl::OUString(),
                            static_cast< cppu::OWeakObject * >( this ),
                            -1 );
            break;

        case DAVException::DAV_FILE_OPEN:
        {
            uno::Sequence< uno::Any > aArgs( 1 );
            aArgs[ 0 ] <<= e.getData();

            aException <<=
                star::ucb::InteractiveAugmentedIOException(
                         rtl::OUString::createFromAscii( "Cannot open file!" ),
                         static_cast< cppu::OWeakObject * >( this ),
                         task::InteractionClassification_ERROR,
                         star::ucb::IOErrorCode_CANT_READ,
                         aArgs );
            break;
        }

        case DAVException::DAV_FILE_WRITE:
        {
            uno::Sequence< uno::Any > aArgs( 1 );
            aArgs[ 0 ] <<= e.getData();
            aException <<=
                star::ucb::InteractiveAugmentedIOException(
                         rtl::OUString::createFromAscii(
                                            "Cannot write to file!" ),
                         static_cast< cppu::OWeakObject * >( this ),
                         task::InteractionClassification_ERROR,
                         star::ucb::IOErrorCode_CANT_WRITE,
                         aArgs );
            break;
        }

        default:
            aException <<=
                star::ucb::InteractiveNetworkGeneralException(
                            rtl::OUString(),
                            static_cast< cppu::OWeakObject * >( this ),
                            task::InteractionClassification_ERROR );
            break;
    }

    return aException;
}

//=========================================================================
void Content::cancelCommandExecution(
                const DAVException & e,
                const uno::Reference< star::ucb::XCommandEnvironment > & xEnv,
                sal_Bool bWrite /* = sal_False */ )
    throw ( uno::Exception )
{
    ucbhelper::cancelCommandExecution( MapDAVException( e, bWrite ), xEnv );
    // Unreachable
}

