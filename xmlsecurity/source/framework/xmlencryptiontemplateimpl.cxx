/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlencryptiontemplateimpl.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: ihi $ $Date: 2007-04-17 10:19:59 $
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
#include "precompiled_xmlsecurity.hxx"

#ifndef _SAL_CONFIG_H_
#include <sal/config.h>
#endif

#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif

#ifndef _XMLENCRYPTIONTEMPLATEIMPL_HXX_
#include "xmlencryptiontemplateimpl.hxx"
#endif

using namespace ::com::sun::star::uno ;
using ::com::sun::star::lang::XMultiServiceFactory ;
using ::com::sun::star::lang::XSingleServiceFactory ;
using ::rtl::OUString ;

using ::com::sun::star::xml::wrapper::XXMLElementWrapper ;
using ::com::sun::star::xml::crypto::XXMLEncryptionTemplate ;

XMLEncryptionTemplateImpl :: XMLEncryptionTemplateImpl( const Reference< XMultiServiceFactory >& aFactory )
    : m_xTemplate( NULL ),
      m_xTarget( NULL ),
      m_xServiceManager( aFactory ),
      m_nStatus ( ::com::sun::star::xml::crypto::SecurityOperationStatus_UNKNOWN ) {
}

XMLEncryptionTemplateImpl :: ~XMLEncryptionTemplateImpl() {
}

/* XXMLEncryptionTemplate */
void SAL_CALL XMLEncryptionTemplateImpl :: setTemplate( const Reference< XXMLElementWrapper >& aTemplate )
    throw (com::sun::star::uno::RuntimeException, com::sun::star::lang::IllegalArgumentException)
{
    m_xTemplate = aTemplate ;
}

/* XXMLEncryptionTemplate */
Reference< XXMLElementWrapper > SAL_CALL XMLEncryptionTemplateImpl :: getTemplate()
throw (com::sun::star::uno::RuntimeException)
{
    return m_xTemplate ;
}

/* XXMLEncryptionTemplate */
void SAL_CALL XMLEncryptionTemplateImpl :: setTarget( const Reference< XXMLElementWrapper >& aTarget )
    throw( com::sun::star::lang::IllegalArgumentException ) {
    m_xTarget = aTarget ;
}

/* XXMLEncryptionTemplate */
Reference< XXMLElementWrapper > SAL_CALL XMLEncryptionTemplateImpl :: getTarget()
throw (com::sun::star::uno::RuntimeException)
{
    return m_xTarget ;
}

void SAL_CALL XMLEncryptionTemplateImpl::setStatus(
    ::com::sun::star::xml::crypto::SecurityOperationStatus status )
    throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
{
    m_nStatus = status;
}

::com::sun::star::xml::crypto::SecurityOperationStatus SAL_CALL XMLEncryptionTemplateImpl::getStatus(  )
    throw (::com::sun::star::uno::RuntimeException)
{
    return m_nStatus;
}

/* XInitialization */
void SAL_CALL XMLEncryptionTemplateImpl :: initialize( const Sequence< Any >& /*aArguments*/ )
    throw( Exception, RuntimeException ) {
    // TBD
} ;

/* XServiceInfo */
OUString SAL_CALL XMLEncryptionTemplateImpl :: getImplementationName() throw( RuntimeException ) {
    return impl_getImplementationName() ;
}

/* XServiceInfo */
sal_Bool SAL_CALL XMLEncryptionTemplateImpl :: supportsService( const OUString& serviceName) throw( RuntimeException ) {
    Sequence< OUString > seqServiceNames = getSupportedServiceNames() ;
    const OUString* pArray = seqServiceNames.getConstArray() ;
    for( sal_Int32 i = 0 ; i < seqServiceNames.getLength() ; i ++ ) {
        if( *( pArray + i ) == serviceName )
            return sal_True ;
    }
    return sal_False ;
}

/* XServiceInfo */
Sequence< OUString > SAL_CALL XMLEncryptionTemplateImpl :: getSupportedServiceNames() throw( RuntimeException ) {
    return impl_getSupportedServiceNames() ;
}

//Helper for XServiceInfo
Sequence< OUString > XMLEncryptionTemplateImpl :: impl_getSupportedServiceNames() {
    ::osl::Guard< ::osl::Mutex > aGuard( ::osl::Mutex::getGlobalMutex() ) ;
    Sequence< OUString > seqServiceNames( 1 ) ;
    seqServiceNames.getArray()[0] = OUString::createFromAscii( "com.sun.star.xml.crypto.XMLEncryptionTemplate" ) ;
    return seqServiceNames ;
}

OUString XMLEncryptionTemplateImpl :: impl_getImplementationName() throw( RuntimeException ) {
    return OUString::createFromAscii( "com.sun.star.xml.security.framework.XMLEncryptionTemplateImpl" ) ;
}

//Helper for registry
Reference< XInterface > SAL_CALL XMLEncryptionTemplateImpl :: impl_createInstance( const Reference< XMultiServiceFactory >& aServiceManager ) throw( RuntimeException ) {
    return Reference< XInterface >( *new XMLEncryptionTemplateImpl( aServiceManager ) ) ;
}

Reference< XSingleServiceFactory > XMLEncryptionTemplateImpl :: impl_createFactory( const Reference< XMultiServiceFactory >& aServiceManager ) {
    //Reference< XSingleServiceFactory > xFactory ;
    //xFactory = ::cppu::createSingleFactory( aServiceManager , impl_getImplementationName , impl_createInstance , impl_getSupportedServiceNames ) ;
    //return xFactory ;
    return ::cppu::createSingleFactory( aServiceManager , impl_getImplementationName() , impl_createInstance , impl_getSupportedServiceNames() ) ;
}

