/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: documentdigitalsignatures.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-05 14:46:27 $
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

#include <documentdigitalsignatures.hxx>
#include <xmlsecurity/digitalsignaturesdialog.hxx>
#include <xmlsecurity/certificateviewer.hxx>
#include <xmlsecurity/macrosecurity.hxx>
#include <xmlsecurity/baseencoding.hxx>
#include <xmlsecurity/biginteger.hxx>
#include <xmlsecurity/global.hrc>

#include <../dialogs/resourcemanager.hxx>

#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XTRANSACTEDOBJECT_HPP_
#include <com/sun/star/embed/XTransactedObject.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif

#ifndef _COM_SUN_STAR_UCB_XCONTENT_HPP_
#include <com/sun/star/ucb/XContent.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTPROVIDER_HPP_
#include <com/sun/star/ucb/XContentProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTIDENTIFIERFACTORY_HPP_
#include <com/sun/star/ucb/XContentIdentifierFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDENVIRONMENT_HPP_
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDPROCESSOR_HPP_
#include <com/sun/star/ucb/XCommandProcessor.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_COMMAND_HPP_
#include <com/sun/star/ucb/Command.hpp>
#endif

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_SECURITYOPTIONS_HXX
#include <svtools/securityoptions.hxx>
#endif
#ifndef _COM_SUN_STAR_SECURITY_CERTIFICATEVALIDITY_HDL_
#include <com/sun/star/security/CertificateValidity.hdl>
#endif
#ifndef _UCBHELPER_CONTENTBROKER_HXX
#include <ucbhelper/contentbroker.hxx>
#endif
#ifndef _UNOTOOLS_UCBHELPER_HXX
#include <unotools/ucbhelper.hxx>
#endif

#include <stdio.h>


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
namespace css = ::com::sun::star;

DocumentDigitalSignatures::DocumentDigitalSignatures( const Reference< com::sun::star::lang::XMultiServiceFactory> rxMSF )
{
    mxMSF = rxMSF;
}

sal_Bool DocumentDigitalSignatures::signDocumentContent( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream ) throw (RuntimeException)
{
    return ImplViewSignatures( rxStorage, xSignStream, SignatureModeDocumentContent, false );
}

Sequence< ::com::sun::star::security::DocumentSignatureInformation > DocumentDigitalSignatures::verifyDocumentContentSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (RuntimeException)
{
    return ImplVerifySignatures( rxStorage, xSignInStream, SignatureModeDocumentContent );
}

void DocumentDigitalSignatures::showDocumentContentSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (RuntimeException)
{
    ImplViewSignatures( rxStorage, xSignInStream, SignatureModeDocumentContent, true );
}

::rtl::OUString DocumentDigitalSignatures::getDocumentContentSignatureDefaultStreamName() throw (::com::sun::star::uno::RuntimeException)
{
    return DocumentSignatureHelper::GetDocumentContentSignatureDefaultStreamName();
}

sal_Bool DocumentDigitalSignatures::signScriptingContent( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream ) throw (RuntimeException)
{
    return ImplViewSignatures( rxStorage, xSignStream, SignatureModeMacros, false );
}

Sequence< ::com::sun::star::security::DocumentSignatureInformation > DocumentDigitalSignatures::verifyScriptingContentSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (RuntimeException)
{
    return ImplVerifySignatures( rxStorage, xSignInStream, SignatureModeMacros );
}

void DocumentDigitalSignatures::showScriptingContentSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (RuntimeException)
{
    ImplViewSignatures( rxStorage, xSignInStream, SignatureModeMacros, true );
}

::rtl::OUString DocumentDigitalSignatures::getScriptingContentSignatureDefaultStreamName() throw (::com::sun::star::uno::RuntimeException)
{
    return DocumentSignatureHelper::GetScriptingContentSignatureDefaultStreamName();
}


sal_Bool DocumentDigitalSignatures::signPackage( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream  ) throw (RuntimeException)
{
    return ImplViewSignatures( rxStorage, xSignStream, SignatureModePackage, false );
}

Sequence< ::com::sun::star::security::DocumentSignatureInformation > DocumentDigitalSignatures::verifyPackageSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (RuntimeException)
{
    return ImplVerifySignatures( rxStorage, xSignInStream, SignatureModePackage );
}

void DocumentDigitalSignatures::showPackageSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (RuntimeException)
{
    ImplViewSignatures( rxStorage, xSignInStream, SignatureModePackage, true );
}

::rtl::OUString DocumentDigitalSignatures::getPackageSignatureDefaultStreamName(  ) throw (::com::sun::star::uno::RuntimeException)
{
    return DocumentSignatureHelper::GetPackageSignatureDefaultStreamName();
}


sal_Bool DocumentDigitalSignatures::ImplViewSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignStream, DocumentSignatureMode eMode, bool bReadOnly ) throw (RuntimeException)
{
    Reference< io::XStream > xStream;
    if ( xSignStream.is() )
        xStream = Reference< io::XStream >( xSignStream, UNO_QUERY );
    return ImplViewSignatures( rxStorage, xStream, eMode, bReadOnly );
}

sal_Bool DocumentDigitalSignatures::ImplViewSignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream, DocumentSignatureMode eMode, bool bReadOnly ) throw (RuntimeException)
{
    sal_Bool bChanges = sal_False;
    DigitalSignaturesDialog aSignaturesDialog( NULL, mxMSF, eMode, bReadOnly );
    bool bInit = aSignaturesDialog.Init( rtl::OUString() );
    DBG_ASSERT( bInit, "Error initializing security context!" );
    if ( bInit )
    {
        aSignaturesDialog.SetStorage( rxStorage );
        aSignaturesDialog.SetSignatureStream( xSignStream );
        if ( aSignaturesDialog.Execute() )
        {
            if ( aSignaturesDialog.SignaturesChanged() )
            {
                bChanges = TRUE;
                // If we have a storage and no stream, we are responsible for commit
                if ( rxStorage.is() && !xSignStream.is() )
                {
                    uno::Reference< embed::XTransactedObject > xTrans( rxStorage, uno::UNO_QUERY );
                    xTrans->commit();
                }
            }
        }
    }
    else
    {
        WarningBox aBox( NULL, XMLSEC_RES( RID_XMLSECWB_NO_MOZILLA_PROFILE ) );
        aBox.Execute();
    }

    return bChanges;
}

Sequence< ::com::sun::star::security::DocumentSignatureInformation > DocumentDigitalSignatures::ImplVerifySignatures( const Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignStream, DocumentSignatureMode eMode ) throw (RuntimeException)
{
    if (!rxStorage.is())
    {
        DBG_ASSERT(0, "Error, no XStorage provided");
        return Sequence<css::security::DocumentSignatureInformation>();
    }
    // First check for the InputStream, to avoid unnecessary initialization of the security environemnt...
    SignatureStreamHelper aStreamHelper;
    Reference< io::XInputStream > xInputStream = xSignStream;

    if ( !xInputStream.is() )
    {
        aStreamHelper = DocumentSignatureHelper::OpenSignatureStream( rxStorage, embed::ElementModes::READ, eMode );
        if ( aStreamHelper.xSignatureStream.is() )
            xInputStream = Reference< io::XInputStream >( aStreamHelper.xSignatureStream, UNO_QUERY );
    }

    if ( !xInputStream.is() )
        return Sequence< ::com::sun::star::security::DocumentSignatureInformation >(0);


    XMLSignatureHelper aSignatureHelper( mxMSF );

    bool bInit = aSignatureHelper.Init( rtl::OUString() );

    DBG_ASSERT( bInit, "Error initializing security context!" );

    if ( !bInit )
        return Sequence< ::com::sun::star::security::DocumentSignatureInformation >(0);

    aSignatureHelper.SetStorage( rxStorage );

    aSignatureHelper.StartMission();

    aSignatureHelper.ReadAndVerifySignature( xInputStream );

    aSignatureHelper.EndMission();

    Reference< ::com::sun::star::xml::crypto::XSecurityEnvironment > xSecEnv = aSignatureHelper.GetSecurityEnvironment();

    SignatureInformations aSignInfos = aSignatureHelper.GetSignatureInformations();
    int nInfos = aSignInfos.size();
    Sequence< css::security::DocumentSignatureInformation > aInfos(nInfos);
    css::security::DocumentSignatureInformation* arInfos = aInfos.getArray();

    if ( nInfos )
    {
        std::vector< rtl::OUString > aElementsToBeVerified = DocumentSignatureHelper::CreateElementList( rxStorage, ::rtl::OUString(), eMode );
        for( int n = 0; n < nInfos; ++n )
        {
            const SignatureInformation& rInfo = aSignInfos[n];
            css::security::DocumentSignatureInformation& rSigInfo = arInfos[n];

            if (rInfo.ouX509Certificate.getLength())
               rSigInfo.Signer = xSecEnv->createCertificateFromAscii( rInfo.ouX509Certificate ) ;
            if (!rSigInfo.Signer.is())
                rSigInfo.Signer = xSecEnv->getCertificate( rInfo.ouX509IssuerName, numericStringToBigInteger( rInfo.ouX509SerialNumber ) );

            // --> PB 2004-12-14 #i38744# time support again
            Date aDate( rInfo.stDateTime.Day, rInfo.stDateTime.Month, rInfo.stDateTime.Year );
            Time aTime( rInfo.stDateTime.Hours, rInfo.stDateTime.Minutes,
                        rInfo.stDateTime.Seconds, rInfo.stDateTime.HundredthSeconds );
            rSigInfo.SignatureDate = aDate.GetDate();
            rSigInfo.SignatureTime = aTime.GetTime();

            // Verify certificate
            //We have patched our version of libxmlsec, so that it does not verify the certificates. This has two
            //reasons. First we want two separate status for signature and certificate. Second libxmlsec calls
            //CERT_VerifyCertificate (solaris, linux) falsly, so that it always regards the certificate as valid.
            //On Window the checking of the certificate path is buggy. It does name matching (issuer, subject name)
            //to find the parent certificate. It does not take into account that there can be several certificates
            //with the same subject name.
            if (rSigInfo.Signer.is())
            {
                try {
                    rSigInfo.CertificateStatus = xSecEnv->verifyCertificate(rSigInfo.Signer);
                } catch (SecurityException& ) {
                    OSL_ENSURE(0, "Verification of certificate failed");
                    rSigInfo.CertificateStatus = css::security::CertificateValidity::INVALID;
                }
            }
            else
            {
                //We should always be aible to get the certificates because it is contained in the document,
                //unless the document is damaged so that signature xml file could not be parsed.
                rSigInfo.CertificateStatus = css::security::CertificateValidity::INVALID;
            }

            rSigInfo.SignatureIsValid = ( rInfo.nStatus == ::com::sun::star::xml::crypto::SecurityOperationStatus_OPERATION_SUCCEEDED );


            if ( rSigInfo.SignatureIsValid )
            {
                // Can only be valid if ALL streams are signed, which means real stream count == signed stream count
                unsigned int nRealCount = 0;
                for ( int i = rInfo.vSignatureReferenceInfors.size(); i; )
                {
                    const SignatureReferenceInformation& rInf = rInfo.vSignatureReferenceInfors[--i];
                    // There is also an extra entry of type TYPE_SAMEDOCUMENT_REFERENCE because of signature date.
                    if ( ( rInf.nType == TYPE_BINARYSTREAM_REFERENCE ) || ( rInf.nType == TYPE_XMLSTREAM_REFERENCE ) )
                        nRealCount++;
                }
                rSigInfo.SignatureIsValid = ( aElementsToBeVerified.size() == nRealCount );
            }

        }
    }
    return aInfos;

}

void DocumentDigitalSignatures::manageTrustedSources(  ) throw (RuntimeException)
{
    // MT: i45295
    // SecEnv is only needed to display certificate information from trusted sources.
    // Macro Security also has some options where no security environment is needed, so raise dialog anyway.
    // Later I should change the code so the Dialog creates the SecEnv on demand...

    cssu::Reference< dcss::xml::crypto::XSecurityEnvironment > xSecEnv;

    XMLSignatureHelper aSignatureHelper( mxMSF );
    if ( aSignatureHelper.Init( rtl::OUString() ) )
        xSecEnv = aSignatureHelper.GetSecurityEnvironment();

    MacroSecurity aDlg( NULL, xSecEnv );
    aDlg.Execute();
}

void DocumentDigitalSignatures::showCertificate( const Reference< ::com::sun::star::security::XCertificate >& _Certificate ) throw (RuntimeException)
{
    XMLSignatureHelper aSignatureHelper( mxMSF );

    bool bInit = aSignatureHelper.Init( rtl::OUString() );

    DBG_ASSERT( bInit, "Error initializing security context!" );

    if ( bInit )
    {
        CertificateViewer aViewer( NULL, aSignatureHelper.GetSecurityEnvironment(), _Certificate, FALSE );
        aViewer.Execute();
    }

}

::sal_Bool DocumentDigitalSignatures::isAuthorTrusted( const Reference< ::com::sun::star::security::XCertificate >& Author ) throw (RuntimeException)
{
    sal_Bool bFound = sal_False;
    ::rtl::OUString sSerialNum = bigIntegerToNumericString( Author->getSerialNumber() );

    Sequence< SvtSecurityOptions::Certificate > aTrustedAuthors = SvtSecurityOptions().GetTrustedAuthors();
    const SvtSecurityOptions::Certificate* pAuthors = aTrustedAuthors.getConstArray();
    const SvtSecurityOptions::Certificate* pAuthorsEnd = pAuthors + aTrustedAuthors.getLength();
    for ( ; pAuthors != pAuthorsEnd; ++pAuthors )
    {
        SvtSecurityOptions::Certificate aAuthor = *pAuthors;
        if ( ( aAuthor[0] == Author->getIssuerName() ) && ( aAuthor[1] == sSerialNum ) )
        {
            bFound = sal_True;
            break;
        }
    }

    return bFound;
}

::sal_Bool DocumentDigitalSignatures::isLocationTrusted( const ::rtl::OUString& Location ) throw (RuntimeException)
{
    sal_Bool bFound = sal_False;
    INetURLObject aLocObj( Location );
    INetURLObject aLocObjLowCase( Location.toAsciiLowerCase() ); // will be used for case insensitive comparing

    ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XContentProvider > xContentProvider;
    ::ucbhelper::ContentBroker* pBroker = NULL;

    //warning free code
    //if ( aLocObj.GetProtocol() == INET_PROT_FILE && ( pBroker = ::ucbhelper::ContentBroker::get() ) )
    //  xContentProvider = pBroker->getContentProviderInterface();
    if ( aLocObj.GetProtocol() == INET_PROT_FILE)
    {
        pBroker = ::ucbhelper::ContentBroker::get();
        if (pBroker)
            xContentProvider = pBroker->getContentProviderInterface();
    }

    Sequence< ::rtl::OUString > aSecURLs = SvtSecurityOptions().GetSecureURLs();
    const ::rtl::OUString* pSecURLs = aSecURLs.getConstArray();
    const ::rtl::OUString* pSecURLsEnd = pSecURLs + aSecURLs.getLength();
    for ( ; pSecURLs != pSecURLsEnd && !bFound; ++pSecURLs )
        bFound = ::utl::UCBContentHelper::IsSubPath( *pSecURLs, Location, xContentProvider );

    return bFound;
}

void DocumentDigitalSignatures::addAuthorToTrustedSources( const Reference< ::com::sun::star::security::XCertificate >& Author ) throw (RuntimeException)
{
    SvtSecurityOptions aSecOpts;

    SvtSecurityOptions::Certificate aNewCert( 3 );
    aNewCert[ 0 ] = Author->getIssuerName();
    aNewCert[ 1 ] = bigIntegerToNumericString( Author->getSerialNumber() );
    aNewCert[ 2 ] = baseEncode( Author->getEncoded(), BASE64 );

    Sequence< SvtSecurityOptions::Certificate > aTrustedAuthors = aSecOpts.GetTrustedAuthors();
    sal_Int32 nCnt = aTrustedAuthors.getLength();
    aTrustedAuthors.realloc( nCnt + 1 );
    aTrustedAuthors[ nCnt ] = aNewCert;

    aSecOpts.SetTrustedAuthors( aTrustedAuthors );
}

void DocumentDigitalSignatures::addLocationToTrustedSources( const ::rtl::OUString& Location ) throw (RuntimeException)
{
    SvtSecurityOptions aSecOpt;

    Sequence< ::rtl::OUString > aSecURLs = aSecOpt.GetSecureURLs();
    sal_Int32 nCnt = aSecURLs.getLength();
    aSecURLs.realloc( nCnt + 1 );
    aSecURLs[ nCnt ] = Location;

    aSecOpt.SetSecureURLs( aSecURLs );
}

rtl::OUString DocumentDigitalSignatures::GetImplementationName() throw (RuntimeException)
{
    return rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.security.DocumentDigitalSignatures" ) );
}

Sequence< rtl::OUString > DocumentDigitalSignatures::GetSupportedServiceNames() throw (cssu::RuntimeException)
{
    Sequence < rtl::OUString > aRet(1);
    rtl::OUString* pArray = aRet.getArray();
    pArray[0] =  rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.security.DocumentDigitalSignatures" ) );
    return aRet;
}


Reference< XInterface > DocumentDigitalSignatures_CreateInstance(
    const Reference< com::sun::star::lang::XMultiServiceFactory >& rSMgr) throw ( Exception )
{
    return (cppu::OWeakObject*) new DocumentDigitalSignatures( rSMgr );
}

