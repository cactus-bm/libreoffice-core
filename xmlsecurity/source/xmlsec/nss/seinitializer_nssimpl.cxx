/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: seinitializer_nssimpl.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 17:34:13 $
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

/*
 * Turn off DEBUG Assertions
 */
#ifdef _DEBUG
    #define _DEBUG_WAS_DEFINED _DEBUG
    #undef _DEBUG
#else
    #undef _DEBUG_WAS_DEFINED
#endif

/*
 * and turn off the additional virtual methods which are part of some interfaces when compiled
 * with debug
 */
#ifdef DEBUG
    #define DEBUG_WAS_DEFINED DEBUG
    #undef DEBUG
#else
    #undef DEBUG_WAS_DEFINED
#endif


#include <sal/types.h>

#include <tools/debug.hxx>
#include <rtl/logfile.hxx>

#include "seinitializer_nssimpl.hxx"

#include "securityenvironment_nssimpl.hxx"

#ifndef _COM_SUN_STAR_MOZILLA_XMOZILLABOOTSTRAP_HPP_
#include <com/sun/star/mozilla/XMozillaBootstrap.hpp>
#endif

#include "nspr.h"
#include "prtypes.h"
#include "pk11func.h"
#ifdef SYSTEM_MOZILLA
#include "nssrenam.h"
#endif
#include "cert.h"
#include "cryptohi.h"
#include "certdb.h"
#include "nss.h"

namespace cssu = com::sun::star::uno;
namespace cssl = com::sun::star::lang;
namespace cssxc = com::sun::star::xml::crypto;

using namespace com::sun::star;

#define SERVICE_NAME "com.sun.star.xml.crypto.SEInitializer"
#define IMPLEMENTATION_NAME "com.sun.star.xml.security.bridge.xmlsec.SEInitializer_NssImpl"
#define SECURITY_ENVIRONMENT "com.sun.star.xml.crypto.SecurityEnvironment"
#define SECURITY_CONTEXT "com.sun.star.xml.crypto.XMLSecurityContext"

bool nsscrypto_initialize( const char* token ) {
    static char initialized = 0 ;

    //PR_Init( PR_SYSTEM_THREAD, PR_PRIORITY_NORMAL, 1 ) ;
    if( !initialized ) {
        PR_Init( PR_USER_THREAD, PR_PRIORITY_NORMAL, 1 ) ;

        if( NSS_Init( token ) != SECSuccess )
            return false ;

        initialized = 1 ;
    }

    return true ;
}

void nsscrypto_finalize() {
    PK11_LogoutAll();
    NSS_Shutdown();
}

bool getMozillaCurrentProfile(
    const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory > &rxMSF,
    rtl::OUString& profilePath)
{
    /*
     * first, try to get the profile from "MOZILLA_CERTIFICATE_FOLDER"
     */
        char * env = getenv("MOZILLA_CERTIFICATE_FOLDER");
        if (env)
        {
            profilePath = rtl::OUString::createFromAscii( env );
            RTL_LOGFILE_PRODUCT_TRACE1( "XMLSEC: Using env MOZILLA_CERTIFICATE_FOLDER: %s", rtl::OUStringToOString( profilePath, RTL_TEXTENCODING_ASCII_US ).getStr() );
            return true;
        }
        else
        {
            RTL_LOGFILE_TRACE( "getMozillaCurrentProfile: Using MozillaBootstrap..." );
        mozilla::MozillaProductType productTypes[4] = {
            mozilla::MozillaProductType_Thunderbird,
            mozilla::MozillaProductType_Mozilla,
            mozilla::MozillaProductType_Firefox,
            mozilla::MozillaProductType_Default };
        int nProduct = 4;

        uno::Reference<uno::XInterface> xInstance = rxMSF->createInstance(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.mozilla.MozillaBootstrap")) );
        OSL_ENSURE( xInstance.is(), "failed to create instance" );

        uno::Reference<mozilla::XMozillaBootstrap> xMozillaBootstrap
            =  uno::Reference<mozilla::XMozillaBootstrap>(xInstance,uno::UNO_QUERY);
        OSL_ENSURE( xMozillaBootstrap.is(), "failed to create instance" );

        if (xMozillaBootstrap.is())
        {
            for (int i=0; i<nProduct; i++)
            {
                ::rtl::OUString profile = xMozillaBootstrap->getDefaultProfile(productTypes[i]);

                RTL_LOGFILE_TRACE2( "getMozillaCurrentProfile: getDefaultProfile [%i] returns %s", i, rtl::OUStringToOString( profile, RTL_TEXTENCODING_ASCII_US ).getStr() );

                if (profile != NULL && profile.getLength()>0)
                {
                    profilePath = xMozillaBootstrap->getProfilePath(productTypes[i],profile);
                    RTL_LOGFILE_PRODUCT_TRACE1( "XMLSEC: Using Mozilla Profile: %s", rtl::OUStringToOString( profilePath, RTL_TEXTENCODING_ASCII_US ).getStr() );
                    return true;
                }
            }
        }

        RTL_LOGFILE_PRODUCT_TRACE( "XMLSEC: No Mozilla Profile found!" );
        return false;
    }
}

SEInitializer_NssImpl::SEInitializer_NssImpl(
    const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory > &rxMSF)
    :mxMSF( rxMSF )
{
}

SEInitializer_NssImpl::~SEInitializer_NssImpl()
{
}

/* XSEInitializer */
cssu::Reference< cssxc::XXMLSecurityContext > SAL_CALL
    SEInitializer_NssImpl::createSecurityContext(
    const rtl::OUString& sCertDB )
    throw (cssu::RuntimeException)
{
    CERTCertDBHandle    *pCertHandle = NULL ;
    PK11SlotInfo        *pSlot = NULL , *pInternalSlot = NULL ;
    PK11SymKey          *pSymKey = NULL ;
    PRBool              found;

    rtl::OString sCertDir;
    if( sCertDB.getLength() )
    {
        sCertDir = rtl::OString(sCertDB, sCertDB.getLength(), RTL_TEXTENCODING_ASCII_US);
    }
    else
    {
        static rtl::OString* pDefaultCertDir = NULL;
        if ( !pDefaultCertDir )
        {
            pDefaultCertDir = new rtl::OString;
            rtl::OUString ouCertDir;



            if ( getMozillaCurrentProfile(mxMSF, ouCertDir) )
                *pDefaultCertDir = rtl::OString(ouCertDir, ouCertDir.getLength(), RTL_TEXTENCODING_ASCII_US);
        }
        sCertDir = *pDefaultCertDir;

    }

    if( !sCertDir.getLength() )
    {
        RTL_LOGFILE_TRACE( "XMLSEC: Error - No certificate directory!" );
        // return NULL;
    }


    /* Initialize NSPR and NSS */
    /* Replaced with new methods by AF. ----
    //PR_Init( PR_SYSTEM_THREAD, PR_PRIORITY_NORMAL, 1 ) ;
    PR_Init( PR_USER_THREAD, PR_PRIORITY_NORMAL, 1 ) ;

    if (NSS_Init(sCertDir.getStr()) != SECSuccess )
    {
        PK11_LogoutAll();
        return NULL;
    }
    ----*/
    if( !nsscrypto_initialize( sCertDir.getStr() ) )
    {
        RTL_LOGFILE_TRACE( "XMLSEC: Error - nsscrypto_initialize() failed." );
        if ( NSS_NoDB_Init(NULL) != SECSuccess )
        {
            RTL_LOGFILE_TRACE( "XMLSEC: NSS_NoDB_Init also failed, NSS Security not available!" );
            return NULL;
        }
        else
        {
            RTL_LOGFILE_TRACE( "XMLSEC: NSS_NoDB_Init works, enough for verifying signatures..." );
        }
    }
    else
        atexit( nsscrypto_finalize ) ;

    pCertHandle = CERT_GetDefaultCertDB() ;

    /*- i39448 - we will get all slots defined in the profile
    * ---------- and alloc them in each SecurityEnviroment .
    * ---------- By CP/20050105
    --------------------------*/
    pInternalSlot = PK11_GetInternalKeySlot() ;
    if (pInternalSlot == NULL)
    {
    //  PK11_LogoutAll();
    //  NSS_Shutdown();
        RTL_LOGFILE_TRACE( "XMLSEC: Error - pInternalSlot is NULL!" );
        return NULL;
    }
    try
    {
        PK11SlotList* soltList ;
        PK11SlotListElement* soltEle ;

        /* Build XML Security Context */
        const rtl::OUString sSecyrutyContext ( RTL_CONSTASCII_USTRINGPARAM( SECURITY_CONTEXT ) );
        cssu::Reference< cssxc::XXMLSecurityContext > xSecCtx( mxMSF->createInstance ( sSecyrutyContext ), cssu::UNO_QUERY );
        if( !xSecCtx.is() )
        {
        //  PK11_LogoutAll();
        //  NSS_Shutdown();
            return NULL;
        }

        soltList = PK11_GetAllTokens( CKM_INVALID_MECHANISM, PR_FALSE, PR_FALSE, NULL ) ;
        if( soltList != NULL ) {
            for( soltEle = soltList->head ; soltEle != NULL; soltEle = soltEle->next )
            {
                RTL_LOGFILE_TRACE( "XMLSEC: Trying token..." );
                found = PR_FALSE;

                pSlot = soltEle->slot ;

                if(pSlot != NULL){
                    RTL_LOGFILE_TRACE2( "XMLSEC: Found a slot: SlotName=%s, TokenName=%s", PK11_GetSlotName(pSlot), PK11_GetTokenName(pSlot) );
                    pSymKey = PK11_KeyGen( pSlot , CKM_DES3_CBC, NULL, 128, NULL ) ;
                    if( pSymKey == NULL )
                    {
                        PK11_FreeSlot( pSlot ) ;
                    //  PK11_LogoutAll();
                    //  NSS_Shutdown();
                        RTL_LOGFILE_TRACE( "XMLSEC: Error - pSymKey is NULL" );
                        return NULL;
                    }

                    /* Build Security Environment */
                    const rtl::OUString sSecyrutyEnvironment ( RTL_CONSTASCII_USTRINGPARAM( SECURITY_ENVIRONMENT ) );
                    cssu::Reference< cssxc::XSecurityEnvironment > xSecEnv( mxMSF->createInstance ( sSecyrutyEnvironment ), cssu::UNO_QUERY );
                    if( !xSecEnv.is() )
                    {
                        PK11_FreeSymKey( pSymKey ) ;
                        PK11_FreeSlot( pSlot ) ;
                    //  PK11_LogoutAll();
                    //  NSS_Shutdown();
                        return NULL;
                    }

                    /* Setup key slot and certDb */
                    cssu::Reference< cssl::XUnoTunnel > xEnvTunnel( xSecEnv , cssu::UNO_QUERY ) ;
                    if( !xEnvTunnel.is() )
                    {
                        PK11_FreeSymKey( pSymKey ) ;
                        PK11_FreeSlot( pSlot ) ;
                    //  PK11_LogoutAll();
                    //  NSS_Shutdown();
                        return NULL;
                    }

                    SecurityEnvironment_NssImpl* pSecEnv = ( SecurityEnvironment_NssImpl* )xEnvTunnel->getSomething( SecurityEnvironment_NssImpl::getUnoTunnelId() ) ;
                    if( pSecEnv == NULL )
                    {
                        PK11_FreeSymKey( pSymKey ) ;
                        PK11_FreeSlot( pSlot ) ;
                    //  PK11_LogoutAll();
                    //  NSS_Shutdown();
                        return NULL;
                    }

                    // search the internal slot.
                    //PR_fprintf(PR_STDOUT, "Token:%s\n",PK11_GetSlotName(pSlot));
                    //found = PK11_IsInternal(pSlot) ; //This method will return two true result.
                    if((!strcmp(PK11_GetSlotName(pInternalSlot),PK11_GetSlotName(pSlot))&&(!strcmp(PK11_GetTokenName(pInternalSlot),PK11_GetTokenName(pSlot)))))
                    {
                        found = PR_TRUE;
                    }

                    pSecEnv->setCryptoSlot( pSlot ) ;
                    PK11_FreeSlot( pSlot ) ;
                    pSlot = NULL;

                    pSecEnv->setCertDb( pCertHandle ) ;

                    pSecEnv->adoptSymKey( pSymKey ) ;
                    PK11_FreeSymKey( pSymKey ) ;
                    pSymKey = NULL;

                    sal_Int32 n = xSecCtx->addSecurityEnvironment( xSecEnv ) ;

                    if(found != PR_FALSE)
                    {
                        RTL_LOGFILE_TRACE( "XMLSEC: Using this slot as the Default Security Environment." );
                        xSecCtx->setDefaultSecurityEnvironmentIndex( n ) ;
                    }


                }// end of if(pSlot != NULL)
            }// end of for
        }// end of if( soltList != NULL )

        if(pInternalSlot != NULL)
        {
            PK11_FreeSlot(pInternalSlot) ;
            pInternalSlot = NULL ;
        }

        return xSecCtx;
    }
    catch( cssu::Exception& )
    {
        if (pSymKey != NULL)
        {
            PK11_FreeSymKey( pSymKey ) ;
        }

        if (pSlot != NULL)
        {
            PK11_FreeSlot( pSlot ) ;
        }

        if(pInternalSlot != NULL)
        {
            PK11_FreeSlot(pInternalSlot) ;
            pInternalSlot = NULL ;
        }

        //PK11_LogoutAll();
        //NSS_Shutdown();
        return NULL;
    }
}

void SAL_CALL SEInitializer_NssImpl::freeSecurityContext( const cssu::Reference< cssxc::XXMLSecurityContext >& securityContext )
    throw (cssu::RuntimeException)
{
    /*
     * because the security context will free all its content when it
     * is destructed, so here no free process for the security context
     * is needed.
     */
    //PK11_LogoutAll();
    //NSS_Shutdown();
}

rtl::OUString SEInitializer_NssImpl_getImplementationName ()
    throw (cssu::RuntimeException)
{
    return rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( IMPLEMENTATION_NAME ) );
}

sal_Bool SAL_CALL SEInitializer_NssImpl_supportsService( const rtl::OUString& ServiceName )
    throw (cssu::RuntimeException)
{
    return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( SERVICE_NAME ));
}

cssu::Sequence< rtl::OUString > SAL_CALL SEInitializer_NssImpl_getSupportedServiceNames(  )
    throw (cssu::RuntimeException)
{
    cssu::Sequence < rtl::OUString > aRet(1);
    rtl::OUString* pArray = aRet.getArray();
    pArray[0] =  rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );
    return aRet;
}
#undef SERVICE_NAME

cssu::Reference< cssu::XInterface > SAL_CALL SEInitializer_NssImpl_createInstance( const cssu::Reference< cssl::XMultiServiceFactory > & rSMgr)
    throw( cssu::Exception )
{
    return (cppu::OWeakObject*) new SEInitializer_NssImpl(rSMgr);
}

/* XServiceInfo */
rtl::OUString SAL_CALL SEInitializer_NssImpl::getImplementationName(  )
    throw (cssu::RuntimeException)
{
    return SEInitializer_NssImpl_getImplementationName();
}
sal_Bool SAL_CALL SEInitializer_NssImpl::supportsService( const rtl::OUString& rServiceName )
    throw (cssu::RuntimeException)
{
    return SEInitializer_NssImpl_supportsService( rServiceName );
}
cssu::Sequence< rtl::OUString > SAL_CALL SEInitializer_NssImpl::getSupportedServiceNames(  )
    throw (cssu::RuntimeException)
{
    return SEInitializer_NssImpl_getSupportedServiceNames();
}

