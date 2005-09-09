/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlsignature_nssimpl.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 17:36:20 $
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

#ifndef _SAL_CONFIG_H_
#include <sal/config.h>
#endif

#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif

#ifndef _XMLSIGNATURE_NSSIMPL_HXX_
#include "xmlsignature_nssimpl.hxx"
#endif

#ifndef _XMLDOCUMENTWRAPPER_XMLSECIMPL_HXX_
#include "xmldocumentwrapper_xmlsecimpl.hxx"
#endif

#ifndef _XMLELEMENTWRAPPER_XMLSECIMPL_HXX_
#include "xmlelementwrapper_xmlsecimpl.hxx"
#endif

#ifndef _SECURITYENVIRONMENT_NSSIMPL_HXX_
#include "securityenvironment_nssimpl.hxx"
#endif

#ifndef _XMLSECURITYCONTEXT_NSSIMPL_HXX_
#include "xmlsecuritycontext_nssimpl.hxx"
#endif

#ifndef _XMLSTREAMIO_XMLSECIMPL_HXX_
#include "xmlstreamio.hxx"
#endif

#ifndef _ERRORCALLBACK_XMLSECIMPL_HXX_
#include "errorcallback.hxx"
#endif

#include "xmlsec/xmlsec.h"
#include "xmlsec/xmldsig.h"
#include "xmlsec/crypto.h"

using namespace ::com::sun::star::uno ;
using namespace ::com::sun::star::lang ;
using ::com::sun::star::lang::XMultiServiceFactory ;
using ::com::sun::star::lang::XSingleServiceFactory ;
using ::rtl::OUString ;

using ::com::sun::star::xml::wrapper::XXMLElementWrapper ;
using ::com::sun::star::xml::wrapper::XXMLDocumentWrapper ;
using ::com::sun::star::xml::crypto::XSecurityEnvironment ;
using ::com::sun::star::xml::crypto::XXMLSignature ;
using ::com::sun::star::xml::crypto::XXMLSignatureTemplate ;
using ::com::sun::star::xml::crypto::XSecurityEnvironment ;
using ::com::sun::star::xml::crypto::XXMLSecurityContext ;
using ::com::sun::star::xml::crypto::XUriBinding ;
using ::com::sun::star::xml::crypto::XMLSignatureException ;

XMLSignature_NssImpl :: XMLSignature_NssImpl( const Reference< XMultiServiceFactory >& aFactory ) : m_xServiceManager( aFactory ) {
}

XMLSignature_NssImpl :: ~XMLSignature_NssImpl() {
}

/* XXMLSignature */
Reference< XXMLSignatureTemplate >
SAL_CALL XMLSignature_NssImpl :: generate(
    const Reference< XXMLSignatureTemplate >& aTemplate ,
    const Reference< XSecurityEnvironment >& aEnvironment
) throw( com::sun::star::xml::crypto::XMLSignatureException,
         com::sun::star::uno::SecurityException )
{
    xmlSecKeysMngrPtr pMngr = NULL ;
    xmlSecDSigCtxPtr pDsigCtx = NULL ;
    xmlNodePtr pNode = NULL ;

    if( !aTemplate.is() )
        throw RuntimeException() ;

    if( !aEnvironment.is() )
        throw RuntimeException() ;

    //Get the xml node
    Reference< XXMLElementWrapper > xElement = aTemplate->getTemplate() ;
    if( !xElement.is() ) {
        throw RuntimeException() ;
    }

    Reference< XUnoTunnel > xNodTunnel( xElement , UNO_QUERY ) ;
    if( !xNodTunnel.is() ) {
        throw RuntimeException() ;
    }

    XMLElementWrapper_XmlSecImpl* pElement = ( XMLElementWrapper_XmlSecImpl* )xNodTunnel->getSomething( XMLElementWrapper_XmlSecImpl::getUnoTunnelImplementationId() ) ;
    if( pElement == NULL ) {
        throw RuntimeException() ;
    }

    pNode = pElement->getNativeElement() ;

    //Get the stream/URI binding
    Reference< XUriBinding > xUriBinding = aTemplate->getBinding() ;
    if( xUriBinding.is() ) {
        //Register the stream input callbacks into libxml2
        if( xmlRegisterStreamInputCallbacks( xUriBinding ) < 0 )
            throw RuntimeException() ;
    }

    //Get Keys Manager
    Reference< XUnoTunnel > xSecTunnel( aEnvironment , UNO_QUERY ) ;
    if( !xSecTunnel.is() ) {
         throw RuntimeException() ;
    }

#if 0 //i39448 : the key manager should be retrieved from SecurityEnvironment, instead of SecurityContext
    XMLSecurityContext_NssImpl* pSecCtxt = ( XMLSecurityContext_NssImpl* )xSecTunnel->getSomething( XMLSecurityContext_NssImpl::getUnoTunnelId() ) ;
    if( pSecCtxt == NULL )
        throw RuntimeException() ;
#endif

    SecurityEnvironment_NssImpl* pSecEnv = ( SecurityEnvironment_NssImpl* )xSecTunnel->getSomething( SecurityEnvironment_NssImpl::getUnoTunnelId() ) ;
    if( pSecEnv == NULL )
        throw RuntimeException() ;

     setErrorRecorder( aTemplate );

    pMngr = pSecEnv->createKeysManager() ; //i39448
    if( !pMngr ) {
        throw RuntimeException() ;
    }

    //Create Signature context
    pDsigCtx = xmlSecDSigCtxCreate( pMngr ) ;
    if( pDsigCtx == NULL )
    {
        pSecEnv->destroyKeysManager( pMngr ) ; //i39448
        //throw XMLSignatureException() ;
        clearErrorRecorder();
        return aTemplate;
    }

    //Sign the template
    if( xmlSecDSigCtxSign( pDsigCtx , pNode ) < 0 ) {
        xmlSecDSigCtxDestroy( pDsigCtx ) ;
        pSecEnv->destroyKeysManager( pMngr ) ; //i39448

        //Unregistered the stream/URI binding
        if( xUriBinding.is() )
            xmlUnregisterStreamInputCallbacks() ;

        //throw XMLSignatureException() ;
        clearErrorRecorder();
        return aTemplate;
    }

    xmlSecDSigCtxDestroy( pDsigCtx ) ;
    pSecEnv->destroyKeysManager( pMngr ) ; //i39448

    //Unregistered the stream/URI binding
    if( xUriBinding.is() )
        xmlUnregisterStreamInputCallbacks() ;

    clearErrorRecorder();
    return aTemplate ;
}

/* XXMLSignature */
Reference< XXMLSignatureTemplate >
SAL_CALL XMLSignature_NssImpl :: validate(
    const Reference< XXMLSignatureTemplate >& aTemplate ,
    const Reference< XXMLSecurityContext >& aSecurityCtx
) throw( com::sun::star::uno::RuntimeException,
         com::sun::star::uno::SecurityException,
         com::sun::star::xml::crypto::XMLSignatureException ) {
    xmlSecKeysMngrPtr pMngr = NULL ;
    xmlSecDSigCtxPtr pDsigCtx = NULL ;
    xmlNodePtr pNode = NULL ;
    //sal_Bool valid ;

    if( !aTemplate.is() )
        throw RuntimeException() ;

    if( !aSecurityCtx.is() )
        throw RuntimeException() ;

    //Get the xml node
    Reference< XXMLElementWrapper > xElement = aTemplate->getTemplate() ;
    if( !xElement.is() )
        throw RuntimeException() ;

    Reference< XUnoTunnel > xNodTunnel( xElement , UNO_QUERY ) ;
    if( !xNodTunnel.is() ) {
        throw RuntimeException() ;
    }

    XMLElementWrapper_XmlSecImpl* pElement = ( XMLElementWrapper_XmlSecImpl* )xNodTunnel->getSomething( XMLElementWrapper_XmlSecImpl::getUnoTunnelImplementationId() ) ;
    if( pElement == NULL )
        throw RuntimeException() ;

    pNode = pElement->getNativeElement() ;

    //Get the stream/URI binding
    Reference< XUriBinding > xUriBinding = aTemplate->getBinding() ;
    if( xUriBinding.is() ) {
        //Register the stream input callbacks into libxml2
        if( xmlRegisterStreamInputCallbacks( xUriBinding ) < 0 )
            throw RuntimeException() ;
    }

     setErrorRecorder( aTemplate );

    sal_Int32 nSecurityEnvironment = aSecurityCtx->getSecurityEnvironmentNumber();
    sal_Int32 i;

    for (i=0; i<nSecurityEnvironment; ++i)
    {
        Reference< XSecurityEnvironment > aEnvironment = aSecurityCtx->getSecurityEnvironmentByIndex(i);

        //Get Keys Manager
        Reference< XUnoTunnel > xSecTunnel( aEnvironment , UNO_QUERY ) ;
        if( !xSecTunnel.is() ) {
             throw RuntimeException() ;
        }

        SecurityEnvironment_NssImpl* pSecEnv = ( SecurityEnvironment_NssImpl* )xSecTunnel->getSomething( SecurityEnvironment_NssImpl::getUnoTunnelId() ) ;
        if( pSecEnv == NULL )
            throw RuntimeException() ;

        pMngr = pSecEnv->createKeysManager() ; //i39448
        if( !pMngr ) {
            throw RuntimeException() ;
        }

        //Create Signature context
        pDsigCtx = xmlSecDSigCtxCreate( pMngr ) ;
        if( pDsigCtx == NULL )
        {
            pSecEnv->destroyKeysManager( pMngr ) ; //i39448
            //throw XMLSignatureException() ;
            clearErrorRecorder();
            return aTemplate;
        }

        //Verify signature
        int rs = xmlSecDSigCtxVerify( pDsigCtx , pNode );

        xmlSecDSigCtxDestroy( pDsigCtx ) ;
        pSecEnv->destroyKeysManager( pMngr ) ; //i39448

        if (rs >= 0)
        {
            break;
        }
    }

    //Unregistered the stream/URI binding
    if( xUriBinding.is() )
        xmlUnregisterStreamInputCallbacks() ;

    //return valid ;
    clearErrorRecorder();
    return aTemplate;
}

/* XInitialization */
void SAL_CALL XMLSignature_NssImpl :: initialize( const Sequence< Any >& aArguments ) throw( Exception, RuntimeException ) {
    // TBD
} ;

/* XServiceInfo */
OUString SAL_CALL XMLSignature_NssImpl :: getImplementationName() throw( RuntimeException ) {
    return impl_getImplementationName() ;
}

/* XServiceInfo */
sal_Bool SAL_CALL XMLSignature_NssImpl :: supportsService( const OUString& serviceName) throw( RuntimeException ) {
    Sequence< OUString > seqServiceNames = getSupportedServiceNames() ;
    const OUString* pArray = seqServiceNames.getConstArray() ;
    for( sal_Int32 i = 0 ; i < seqServiceNames.getLength() ; i ++ ) {
        if( *( pArray + i ) == serviceName )
            return sal_True ;
    }
    return sal_False ;
}

/* XServiceInfo */
Sequence< OUString > SAL_CALL XMLSignature_NssImpl :: getSupportedServiceNames() throw( RuntimeException ) {
    return impl_getSupportedServiceNames() ;
}

//Helper for XServiceInfo
Sequence< OUString > XMLSignature_NssImpl :: impl_getSupportedServiceNames() {
    ::osl::Guard< ::osl::Mutex > aGuard( ::osl::Mutex::getGlobalMutex() ) ;
    Sequence< OUString > seqServiceNames( 1 ) ;
    seqServiceNames.getArray()[0] = OUString::createFromAscii( "com.sun.star.xml.crypto.XMLSignature" ) ;
    return seqServiceNames ;
}

OUString XMLSignature_NssImpl :: impl_getImplementationName() throw( RuntimeException ) {
    return OUString::createFromAscii( "com.sun.star.xml.security.bridge.xmlsec.XMLSignature_NssImpl" ) ;
}

//Helper for registry
Reference< XInterface > SAL_CALL XMLSignature_NssImpl :: impl_createInstance( const Reference< XMultiServiceFactory >& aServiceManager ) throw( RuntimeException ) {
    return Reference< XInterface >( *new XMLSignature_NssImpl( aServiceManager ) ) ;
}

Reference< XSingleServiceFactory > XMLSignature_NssImpl :: impl_createFactory( const Reference< XMultiServiceFactory >& aServiceManager ) {
    //Reference< XSingleServiceFactory > xFactory ;
    //xFactory = ::cppu::createSingleFactory( aServiceManager , impl_getImplementationName , impl_createInstance , impl_getSupportedServiceNames ) ;
    //return xFactory ;
    return ::cppu::createSingleFactory( aServiceManager , impl_getImplementationName() , impl_createInstance , impl_getSupportedServiceNames() ) ;
}

