/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: gconflayer.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: vg $ $Date: 2006-11-01 14:22:41 $
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


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_shell.hxx"
#ifndef GCONFLAYER_HXX_
#include "gconflayer.hxx"
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_PROPERTYINFO_HPP_
#include <com/sun/star/configuration/backend/PropertyInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XLAYERCONTENTDESCIBER_HPP_
#include <com/sun/star/configuration/backend/XLayerContentDescriber.hpp>
#endif

#ifndef _RTL_STRBUF_HXX_
#include <rtl/strbuf.hxx>
#endif

#ifndef _OSL_SECURITY_HXX_
#include <osl/security.hxx>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

//==============================================================================

GconfLayer::GconfLayer( const uno::Reference<uno::XComponentContext>& xContext,
    const ConfigurationValue pConfigurationValuesList[],
    const sal_Int32 nConfigurationValues,
    const char * pPreloadValuesList[] )
    :m_pConfigurationValuesList( pConfigurationValuesList )
    ,m_nConfigurationValues( nConfigurationValues )
    ,m_pPreloadValuesList( pPreloadValuesList )
{
    //Create instance of LayerContentDescriber Service
    rtl::OUString const k_sLayerDescriberService( RTL_CONSTASCII_USTRINGPARAM(
        "com.sun.star.comp.configuration.backend.LayerDescriber" ) );

    typedef uno::Reference<backend::XLayerContentDescriber> LayerDescriber;
    uno::Reference< lang::XMultiComponentFactory > xServiceManager = xContext->getServiceManager();
    if( xServiceManager.is() )
    {
        m_xLayerContentDescriber = LayerDescriber::query(
            xServiceManager->createInstanceWithContext( k_sLayerDescriberService, xContext ) );
    }
    else
    {
        OSL_TRACE( "Could not retrieve ServiceManager" );
    }
}

//------------------------------------------------------------------------------

uno::Any makeAnyOfGconfValue( GConfValue *aGconfValue )
{
    switch( aGconfValue->type )
    {
        case GCONF_VALUE_BOOL:
            return uno::makeAny( (sal_Bool) gconf_value_get_bool( aGconfValue ) );

        case GCONF_VALUE_INT:
            return uno::makeAny( (sal_Int32) gconf_value_get_int( aGconfValue ) );

        case GCONF_VALUE_STRING:
            return uno::makeAny( OStringToOUString( rtl::OString(
                gconf_value_get_string(aGconfValue) ), RTL_TEXTENCODING_UTF8 ) );

        default:
            fprintf( stderr, "makeAnyOfGconfValue: Type not handled.\n" );
            break;
    }

    return uno::Any();
}

//------------------------------------------------------------------------------

static void splitFontName( GConfValue *aGconfValue, rtl::OUString &rName, sal_Int16 &rHeight)
{
   rtl::OString aFont( gconf_value_get_string( aGconfValue ) );
   aFont.trim();
   sal_Int32 nIdx = aFont.lastIndexOf( ' ' );
   if (nIdx < 1) { // urk
       rHeight = 12;
       nIdx = aFont.getLength();
   } else {
       rtl::OString aSize = aFont.copy( nIdx + 1 );
       rHeight = static_cast<sal_Int16>( aSize.toInt32() );
   }

   rName = rtl::OStringToOUString( aFont.copy( 0, nIdx ), RTL_TEXTENCODING_UTF8 );
}

//------------------------------------------------------------------------------

uno::Any translateToOOo( const ConfigurationValue aValue, GConfValue *aGconfValue )
{
    switch( aValue.nSettingId )
    {
        case SETTING_PROXY_MODE:
        {
            rtl::OUString aProxyMode;
            uno::Any aOriginalValue = makeAnyOfGconfValue( aGconfValue );
            aOriginalValue >>= aProxyMode;

            if( aProxyMode.equals( rtl::OUString::createFromAscii( "manual" ) ) )
                return uno::makeAny( (sal_Int32) 1 );
            else if( aProxyMode.equals( rtl::OUString::createFromAscii( "none" ) ) )
                return uno::makeAny( (sal_Int32) 0 );
        }
            break;

        case SETTING_MAILER_PROGRAM:
        {
            rtl::OUString aMailer;
            uno::Any aOriginalValue = makeAnyOfGconfValue( aGconfValue );
            aOriginalValue >>= aMailer;
            sal_Int32 nIndex = 0;
            return uno::makeAny( aMailer.getToken( 0, ' ', nIndex ) );
        }

#ifdef ENABLE_LOCKDOWN
        // "short" values need to be returned a sal_Int16
        case SETTING_FONT_ANTI_ALIASING_MIN_PIXEL:
        case SETTING_SYMBOL_SET:
        {
            sal_Int32 nShortValue;
            uno::Any aOriginalValue = makeAnyOfGconfValue( aGconfValue );
            aOriginalValue >>= nShortValue;
            return uno::makeAny( (sal_Int16) nShortValue );
        }
            break;
#endif // ENABLE_LOCKDOWN

        // "boolean" values that need a string to be returned
        case SETTING_ENABLE_ACCESSIBILITY:
#ifdef ENABLE_LOCKDOWN
        case SETTING_DISABLE_PRINTING:
#endif // ENABLE_LOCKDOWN
        {
            sal_Bool bBooleanValue = false;
            uno::Any aOriginalValue = makeAnyOfGconfValue( aGconfValue );
            aOriginalValue >>= bBooleanValue;
            return uno::makeAny( rtl::OUString::valueOf( (sal_Bool) bBooleanValue ) );
        }

        case SETTING_WORK_DIRECTORY:
        {
            osl::Security aSecurity;
            rtl::OUString aDocumentsDirURL;
            if ( aSecurity.getHomeDir( aDocumentsDirURL ) )
                aDocumentsDirURL += rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/Documents" ) );
            return uno::makeAny( aDocumentsDirURL );
        }

        case SETTING_USER_GIVENNAME:
        {
            rtl::OUString aCompleteName( rtl::OStringToOUString(
                g_get_real_name(), osl_getThreadTextEncoding() ) );
            sal_Int32 nIndex = 0;
            rtl::OUString aGivenName;
            do
                aGivenName = aCompleteName.getToken( 0, ' ', nIndex );
            while ( nIndex == 0 );

            return uno::makeAny( aGivenName );

        }

        case SETTING_USER_SURNAME:
        {
            rtl::OUString aCompleteName( rtl::OStringToOUString(
                g_get_real_name(), osl_getThreadTextEncoding() ) );
            sal_Int32 nIndex = 0;
            rtl::OUString aSurname;
            do
                aSurname = aCompleteName.getToken( 0, ' ', nIndex );
            while ( nIndex >= 0 );

            return uno::makeAny( aSurname );
        }

        case SETTING_SOURCEVIEWFONT_NAME:
        case SETTING_SOURCEVIEWFONT_HEIGHT:
        {
            rtl::OUString aName;
            sal_Int16 nHeight;

            splitFontName (aGconfValue, aName, nHeight);
            if (aValue.nSettingId == SETTING_SOURCEVIEWFONT_NAME)
                return uno::makeAny( aName );
            else
                return uno::makeAny( nHeight );
        }


        default:
            fprintf( stderr, "Unhandled setting to translate.\n" );
            break;
    }

    return uno::Any();
}

//------------------------------------------------------------------------------

sal_Bool SAL_CALL isDependencySatisfied( const ConfigurationValue aValue )
{
    switch( aValue.nDependsOn )
    {
        case SETTING_PROXY_MODE:
        {
            GConfClient* aClient = GconfBackend::getGconfClient();
            GConfValue* aGconfValue = gconf_client_get( aClient, GCONF_PROXY_MODE_KEY, NULL );

            if ( aGconfValue != NULL )
            {
                bool bOk = g_strcasecmp( "manual", gconf_value_get_string( aGconfValue ) ) == 0;
                gconf_value_free( aGconfValue );
                if (bOk) return sal_True;
            }
        }
            break;

        case SETTING_WORK_DIRECTORY:
        {
            osl::Security aSecurity;
            rtl::OUString aDocumentsDirURL;
            if ( aSecurity.getHomeDir( aDocumentsDirURL ) )
            {
                aDocumentsDirURL += rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/Documents" ) );
                osl::Directory aDocumentsDir( aDocumentsDirURL );

                if( osl::FileBase::E_None == aDocumentsDir.open() )
                    return sal_True;
            }
        }
            break;

        case SETTING_USER_GIVENNAME:
        {
            rtl::OUString aCompleteName( rtl::OStringToOUString(
                g_get_real_name(), osl_getThreadTextEncoding() ) );
            if( !aCompleteName.equalsAscii( "Unknown" ) )
                return sal_True;
        }
            break;

        case SETTING_USER_SURNAME:
        {
            rtl::OUString aCompleteName( rtl::OStringToOUString(
                g_get_real_name(), osl_getThreadTextEncoding() ) );
            if( !aCompleteName.equalsAscii( "Unknown" ) )
            {
                if( aCompleteName.trim().indexOf(rtl::OUString::createFromAscii(" "), 0) != -1 )
                    return sal_True;
            }
        }
            break;

#ifdef ENABLE_LOCKDOWN
        case SETTING_AUTO_SAVE:
        {
            GConfClient* aClient = GconfBackend::getGconfClient();
            GConfValue* aGconfValue = gconf_client_get( aClient, GCONF_AUTO_SAVE_KEY, NULL );

            if( ( aGconfValue != NULL ) )
            {
                bool bOk = gconf_value_get_bool( aGconfValue );
                gconf_value_free( aGconfValue );
                if (bOk) return sal_True;
            }
        }
            break;
#endif // ENABLE_LOCKDOWN

        default:
            fprintf( stderr, "Unhandled setting to check dependency.\n" );
            break;
    }

    return sal_False;
}

//------------------------------------------------------------------------------

void SAL_CALL GconfLayer::readData( const uno::Reference<backend::XLayerHandler>& xHandler )
    throw ( backend::MalformedDataException, lang::NullPointerException,
            lang::WrappedTargetException, uno::RuntimeException )
{
    if( ! m_xLayerContentDescriber.is() )
    {
        throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
            "Could not create com.sun.star.configuration.backend.LayerContentDescriber Service"
        ) ), static_cast < backend::XLayer * > (this) );
    }

    uno::Sequence<backend::PropertyInfo> aPropInfoList( m_nConfigurationValues );
    sal_Int32 nProperties = 0;

    GConfClient* aClient = GconfBackend::getGconfClient();
    GConfValue* aGconfValue;
    int i = 0;

    while( m_pPreloadValuesList[i] != NULL )
        gconf_client_preload( aClient, m_pPreloadValuesList[i++], GCONF_CLIENT_PRELOAD_ONELEVEL, NULL );

    for( i = 0; i < m_nConfigurationValues; i++ )
    {
        if( ( m_pConfigurationValuesList[i].nDependsOn != SETTINGS_LAST ) && !isDependencySatisfied( m_pConfigurationValuesList[i] ) )
            continue;

        aGconfValue = gconf_client_get( aClient, m_pConfigurationValuesList[i].GconfItem, NULL );

        if( aGconfValue != NULL )
        {
            aPropInfoList[nProperties].Name      = rtl::OUString::createFromAscii( m_pConfigurationValuesList[i].OOoConfItem );
            aPropInfoList[nProperties].Type      = rtl::OUString::createFromAscii( m_pConfigurationValuesList[i].OOoConfValueType );
            aPropInfoList[nProperties].Protected = m_pConfigurationValuesList[i].bLocked;

            if( m_pConfigurationValuesList[i].bNeedsTranslation )
                aPropInfoList[nProperties].Value = translateToOOo( m_pConfigurationValuesList[i], aGconfValue );
            else
                aPropInfoList[nProperties].Value = makeAnyOfGconfValue( aGconfValue );

            gconf_value_free( aGconfValue );

            nProperties++;
        }
    }

    if( nProperties > 0 )
    {
        aPropInfoList.realloc( nProperties );
        m_xLayerContentDescriber->describeLayer( xHandler, aPropInfoList );
    }
}

//------------------------------------------------------------------------------

rtl::OUString SAL_CALL GconfLayer::getTimestamp( void )
    throw (uno::RuntimeException)
{
    // Return a hash of the values as timestamp to avoid regenerating
    // the binary cache on each office launch.
    rtl::OStringBuffer aTimeStamp;

    // Make sure the timestamp differs from beta
    sal_Int32 nHashCode = 0;

    GConfClient* aClient = GconfBackend::getGconfClient();
    GConfValue* aGconfValue;
    int i = 0;

    while( m_pPreloadValuesList[i] != NULL )
        gconf_client_preload( aClient, m_pPreloadValuesList[i++], GCONF_CLIENT_PRELOAD_ONELEVEL, NULL );

    for( i = 0; i < m_nConfigurationValues; i++ )
    {
        aGconfValue = gconf_client_get( aClient, m_pConfigurationValuesList[i].GconfItem, NULL );

        if( aGconfValue != NULL )
        {
            switch( aGconfValue->type )
            {
                case GCONF_VALUE_BOOL:
                    nHashCode ^= (sal_Int32) !gconf_value_get_bool( aGconfValue );
                    break;

                case GCONF_VALUE_INT:
                    nHashCode ^= (sal_Int32) gconf_value_get_int( aGconfValue );
                    break;

                case GCONF_VALUE_STRING:
                    nHashCode ^= (sal_Int32) g_str_hash( gconf_value_get_string( aGconfValue ) );
                    break;

                default:
                    fprintf( stderr, "getTimestamp: Type not handled.\n" );
                    break;
            }
            nHashCode = (nHashCode << 5) - nHashCode;
            gconf_value_free( aGconfValue );
        }
    }

    return rtl::OUString::valueOf( nHashCode );
}
