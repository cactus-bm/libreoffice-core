/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: securityenvironment_nssimpl.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 17:33:57 $
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

#ifndef _XSECURITYENVIRONMENT_NSSIMPL_HXX_
#define _XSECURITYENVIRONMENT_NSSIMPL_HXX_

#ifndef _SAL_CONFIG_H_
#include <sal/config.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_EXCEPTION_HPP_
#include <com/sun/star/uno/Exception.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HPP_
#include <com/sun/star/uno/Reference.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSECVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif

#ifndef _COM_SUN_STAR_XML_CRYPTO_XSECURITYENVIRONMENT_HPP_
#include <com/sun/star/xml/crypto/XSecurityEnvironment.hpp>
#endif

#ifndef _COM_SUN_STAR_SECURITY_XCERTIFICATE_HPP_
#include <com/sun/star/security/XCertificate.hpp>
#endif

#ifndef _COM_SUN_STAR_SECURITY_CERTIFICATECHARACTERS_HPP_
#include <com/sun/star/security/CertificateCharacters.hpp>
#endif

#ifndef _COM_SUN_STAR_SECURITY_CERTIFICATEVALIDITY_HPP_
#include <com/sun/star/security/CertificateValidity.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

#include "pk11func.h"
#include "keyhi.h"
#include "certdb.h"
#include "list"

#include "xmlsec/xmlsec.h"

class SecurityEnvironment_NssImpl : public ::cppu::WeakImplHelper4<
    ::com::sun::star::xml::crypto::XSecurityEnvironment ,
    ::com::sun::star::lang::XInitialization ,
    ::com::sun::star::lang::XServiceInfo ,
    ::com::sun::star::lang::XUnoTunnel >
{
    private :

        PK11SlotInfo*                       m_pSlot ;
        CERTCertDBHandle*                   m_pHandler ;
        std::list< PK11SymKey* >            m_tSymKeyList ;
        std::list< SECKEYPublicKey* >       m_tPubKeyList ;
        std::list< SECKEYPrivateKey* >      m_tPriKeyList ;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager ;

    public :
        SecurityEnvironment_NssImpl( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aFactory ) ;
        virtual ~SecurityEnvironment_NssImpl() ;

        //Methods from XSecurityEnvironment

        //Methods from XInitialization
        virtual void SAL_CALL initialize(
            const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments
        ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        //Methods from XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException ) ;

        virtual sal_Bool SAL_CALL supportsService(
            const ::rtl::OUString& ServiceName
        ) throw( ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException ) ;

        //Helper for XServiceInfo
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > impl_getSupportedServiceNames() ;

        static ::rtl::OUString impl_getImplementationName() throw( ::com::sun::star::uno::RuntimeException ) ;

        //Helper for registry
        static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL impl_createInstance( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aServiceManager ) throw( ::com::sun::star::uno::RuntimeException ) ;

        static ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > impl_createFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aServiceManager ) ;

        virtual ::sal_Int32 SAL_CALL verifyCertificate( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& xCert ) throw (::com::sun::star::uno::SecurityException, ::com::sun::star::uno::RuntimeException) ;

        virtual ::sal_Int32 SAL_CALL getCertificateCharacters( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& xCert ) throw (::com::sun::star::uno::SecurityException, ::com::sun::star::uno::RuntimeException) ;

        virtual ::rtl::OUString SAL_CALL getSecurityEnvironmentInformation(  ) throw (::com::sun::star::uno::RuntimeException);

        //Methods from XUnoTunnel
        virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier )
            throw (::com::sun::star::uno::RuntimeException);

        static const ::com::sun::star::uno::Sequence< sal_Int8 >& getUnoTunnelId() ;
        static SecurityEnvironment_NssImpl* getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xObj ) ;

        //Native mehtods
        virtual PK11SlotInfo* getCryptoSlot() throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void setCryptoSlot( PK11SlotInfo* aSlot ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual CERTCertDBHandle* getCertDb() throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void setCertDb( CERTCertDBHandle* aCertDb ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void adoptSymKey( PK11SymKey* aSymKey ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void rejectSymKey( PK11SymKey* aSymKey ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual PK11SymKey* getSymKey( unsigned int position ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void adoptPubKey( SECKEYPublicKey* aPubKey ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void rejectPubKey( SECKEYPublicKey* aPubKey ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual SECKEYPublicKey* getPubKey( unsigned int position ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void adoptPriKey( SECKEYPrivateKey* aPriKey ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual void rejectPriKey( SECKEYPrivateKey* aPriKey ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual SECKEYPrivateKey* getPriKey( unsigned int position ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > > SAL_CALL getPersonalCertificates() throw(  ::com::sun::star::uno::SecurityException , ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL getCertificate( const ::rtl::OUString& issuerName, const ::com::sun::star::uno::Sequence< sal_Int8 >& serialNumber ) throw( ::com::sun::star::uno::SecurityException , ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL getCertificate( const ::rtl::OUString& issuerName, const ::rtl::OUString& serialNumber ) throw( ::com::sun::star::uno::SecurityException , ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > > SAL_CALL buildCertificatePath( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& beginCert ) throw(  ::com::sun::star::uno::SecurityException , ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL createCertificateFromRaw( const ::com::sun::star::uno::Sequence< sal_Int8 >& rawCertificate ) throw( ::com::sun::star::uno::SecurityException , ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL createCertificateFromAscii( const ::rtl::OUString& asciiCertificate ) throw( ::com::sun::star::uno::SecurityException , ::com::sun::star::uno::RuntimeException ) ;


        //Native mehtods
        virtual xmlSecKeysMngrPtr createKeysManager() throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;
        virtual void destroyKeysManager(xmlSecKeysMngrPtr pKeysMngr) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;
} ;

#endif  // _XSECURITYENVIRONMENT_NSSIMPL_HXX_

