/*************************************************************************
 *
 *  $RCSfile: encryptorimpl.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-10 18:06:53 $
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

#ifndef _ENCRYPTORIMPL_HXX
#define _ENCRYPTORIMPL_HXX

#ifndef _COM_SUN_STAR_XML_CRYPTO_SAX_XENCRYPTIONRESULTBROADCASTER_HPP_
#include <com/sun/star/xml/crypto/sax/XEncryptionResultBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_CRYPTO_SAX_XENCRYPTIONRESULTLISTENER_HPP_
#include <com/sun/star/xml/crypto/sax/XEncryptionResultListener.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_CRYPTO_SAX_XSIGNATURECOLLECTOR_HPP_
#include <com/sun/star/xml/crypto/sax/XReferenceCollector.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_CRYPTO_XSECURITYENVIRONMENT_HPP_
#include <com/sun/star/xml/crypto/XSecurityEnvironment.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif

#include "encryptionengine.hxx"

class EncryptorImpl : public cppu::ImplInheritanceHelper4
<
    EncryptionEngine,
    com::sun::star::xml::crypto::sax::XEncryptionResultBroadcaster,
    com::sun::star::xml::crypto::sax::XReferenceCollector,
    com::sun::star::lang::XInitialization,
    com::sun::star::lang::XServiceInfo
>
/****** EncryptorImpl.hxx/CLASS EncryptorImpl *********************************
 *
 *   NAME
 *  EncryptorImpl -- generates an encryption
 *
 *   FUNCTION
 *  Collects all resources for an encryption generation, then generates the
 *  encryption by invoking a xmlsec-based encryption bridge component.
 *
 *   HISTORY
 *  05.01.2004 -    Interface supported: XInitialization, XServiceInfo
 *
 *   AUTHOR
 *  Michael Mi
 *  Email: michael.mi@sun.com
 ******************************************************************************/
{
private:
    /*
     * the Id of the encryption, which is used for the result listener to
     * identify the encryption.
     */
    sal_Int32 m_nEncryptionId;

    /*
     * the Id of the element to be encrypted.
     */
    sal_Int32 m_nReferenceId;

    /*
     * the decryption result,
     * remembers whether the encryption succeeds.
     */
    bool      m_bEncryptionSucceed;

    com::sun::star::uno::Reference<
        com::sun::star::xml::crypto::XSecurityEnvironment > m_xSecurityEnvironment;

    virtual void notifyResultListener() const
        throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);
    virtual bool checkReady() const;
    virtual void startEngine( const com::sun::star::uno::Reference<
        com::sun::star::xml::crypto::XXMLEncryptionTemplate >&
        xEncryptionTemplate)
        throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

public:
    explicit EncryptorImpl( const com::sun::star::uno::Reference<
        com::sun::star::lang::XMultiServiceFactory >& rxMSF);
    virtual ~EncryptorImpl();

    /* XEncryptionResultBroadcaster */
    virtual void SAL_CALL addEncryptionResultListener(
        const com::sun::star::uno::Reference<
            com::sun::star::xml::crypto::sax::XEncryptionResultListener >&
            listener )
            throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeEncryptionResultListener(
            const com::sun::star::uno::Reference<
                com::sun::star::xml::crypto::sax::XEncryptionResultListener >&
                listener )
            throw (com::sun::star::uno::RuntimeException);

    /* XReferenceCollector */
    virtual void SAL_CALL setReferenceCount( sal_Int32 count )
        throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL setReferenceId( sal_Int32 id )
        throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

    /* XInitialization */
    virtual void SAL_CALL initialize(
        const com::sun::star::uno::Sequence< com::sun::star::uno::Any >& aArguments )
        throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

    /* XServiceInfo */
    virtual rtl::OUString SAL_CALL getImplementationName(  )
        throw (com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName )
        throw (com::sun::star::uno::RuntimeException);
    virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(  )
        throw (com::sun::star::uno::RuntimeException);
};

rtl::OUString EncryptorImpl_getImplementationName()
    throw ( com::sun::star::uno::RuntimeException );

sal_Bool SAL_CALL EncryptorImpl_supportsService( const rtl::OUString& ServiceName )
    throw ( com::sun::star::uno::RuntimeException );

com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL EncryptorImpl_getSupportedServiceNames(  )
    throw ( com::sun::star::uno::RuntimeException );

com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
SAL_CALL EncryptorImpl_createInstance(
    const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rSMgr)
    throw ( com::sun::star::uno::Exception );

#endif

