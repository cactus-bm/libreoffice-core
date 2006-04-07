/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlsignaturehelper.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: vg $ $Date: 2006-04-07 11:56:44 $
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

#include <xmlsecurity/xmlsignaturehelper.hxx>
#include <xsecctl.hxx>

#include <xmlsignaturehelper2.hxx>

#include <tools/stream.hxx>
#include <tools/debug.hxx>

#include <xmloff/attrlist.hxx>

#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/lang/XComponent.hpp>

#include <tools/date.hxx>
#include <tools/time.hxx>

//MM : search for the default profile
//#include <unotools/streamhelper.hxx>
//MM : end

/* SEInitializer component */
#define SEINITIALIZER_COMPONENT "com.sun.star.xml.crypto.SEInitializer"

#define TAG_DOCUMENTSIGNATURES  "document-signatures"
#define NS_DOCUMENTSIGNATURES   "http://openoffice.org/2004/documentsignatures"

using namespace ::com::sun::star;

XMLSignatureHelper::XMLSignatureHelper( const uno::Reference< lang::XMultiServiceFactory>& rxMSF )
    : mxMSF(rxMSF)
{
    mpXSecController = new XSecController;
    mxSecurityController = mpXSecController;
    mpXSecController->setFactory( rxMSF );
    mbError = false;
}

XMLSignatureHelper::~XMLSignatureHelper()
{
    if ( mxSEInitializer.is() && mxSecurityContext.is() )
        mxSEInitializer->freeSecurityContext( mxSecurityContext );
}

bool XMLSignatureHelper::Init( const rtl::OUString& rTokenPath )
{
    DBG_ASSERT( !mxSEInitializer.is(), "XMLSignatureHelper::Init - mxSEInitializer already set!" );
    DBG_ASSERT( !mxSecurityContext.is(), "XMLSignatureHelper::Init - mxSecurityContext already set!" );

    ImplCreateSEInitializer();

    if ( mxSEInitializer.is() )
        mxSecurityContext = mxSEInitializer->createSecurityContext( rTokenPath );

    return mxSecurityContext.is();
}

void XMLSignatureHelper::ImplCreateSEInitializer()
{
    rtl::OUString sSEInitializer;

    sSEInitializer = rtl::OUString::createFromAscii( SEINITIALIZER_COMPONENT );
    mxSEInitializer = uno::Reference< com::sun::star::xml::crypto::XSEInitializer > (
            mxMSF->createInstance( sSEInitializer ), uno::UNO_QUERY );
}

void XMLSignatureHelper::SetUriBinding( com::sun::star::uno::Reference< com::sun::star::xml::crypto::XUriBinding >& rxUriBinding )
{
    mxUriBinding = rxUriBinding;
}

com::sun::star::uno::Reference< com::sun::star::xml::crypto::XUriBinding > XMLSignatureHelper::GetUriBinding() const
{
    return mxUriBinding;
}

void XMLSignatureHelper::SetStorage( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >& rxStorage )
{
    DBG_ASSERT( !mxUriBinding.is(), "SetStorage - UriBinding already set!" );
    mxUriBinding = new UriBindingHelper( rxStorage );
}


void XMLSignatureHelper::SetStartVerifySignatureHdl( const Link& rLink )
{
    maStartVerifySignatureHdl = rLink;
}


void XMLSignatureHelper::StartMission()
{
    if ( !mxUriBinding.is() )
        mxUriBinding = new UriBindingHelper();

    mpXSecController->startMission( mxUriBinding, mxSecurityContext );
}

void XMLSignatureHelper::EndMission()
{
    mpXSecController->endMission();
}

sal_Int32 XMLSignatureHelper::GetNewSecurityId()
{
    return mpXSecController->getNewSecurityId();
}

void XMLSignatureHelper::SetX509Certificate(
        sal_Int32 nSecurityId,
        const rtl::OUString& ouX509IssuerName,
        const rtl::OUString& ouX509SerialNumber,
        const rtl::OUString& ouX509Cert)
{
    mpXSecController->setX509Certificate(
        nSecurityId,
        ouX509IssuerName,
        ouX509SerialNumber,
        ouX509Cert);
}

void XMLSignatureHelper::SetX509Certificate(
        sal_Int32 nSecurityId,
        sal_Int32 nSecurityEnvironmentIndex,
        const rtl::OUString& ouX509IssuerName,
        const rtl::OUString& ouX509SerialNumber,
        const rtl::OUString& ouX509Cert)
{
    mpXSecController->setX509Certificate(
        nSecurityId,
        nSecurityEnvironmentIndex,
        ouX509IssuerName,
        ouX509SerialNumber,
        ouX509Cert);
}

void XMLSignatureHelper::SetDateTime( sal_Int32 nSecurityId, const Date& rDate, const Time& rTime )
{
    /*
    rtl::OUString aDate = String::CreateFromInt32( rDate.GetDate() );
    rtl::OUString aTime = String::CreateFromInt32( rTime.GetTime() );
    mpXSecController->setDateTime( nSecurityId, aDate, aTime );
    */
    ::com::sun::star::util::DateTime stDateTime;
    stDateTime.HundredthSeconds = (::sal_uInt16)rTime.Get100Sec();
    stDateTime.Seconds = (::sal_uInt16)rTime.GetSec();
    stDateTime.Minutes = (::sal_uInt16)rTime.GetMin();
    stDateTime.Hours = (::sal_uInt16)rTime.GetHour();
    stDateTime.Day = (::sal_uInt16)rDate.GetDay();
    stDateTime.Month = (::sal_uInt16)rDate.GetMonth();
    stDateTime.Year = (::sal_uInt16)rDate.GetYear();
    mpXSecController->setDate( nSecurityId, stDateTime );
}

void XMLSignatureHelper::AddForSigning( sal_Int32 nSecurityId, const rtl::OUString& uri, const rtl::OUString& objectURL, sal_Bool bBinary )
{
    mpXSecController->signAStream( nSecurityId, uri, objectURL, bBinary );
}

uno::Reference<xml::sax::XDocumentHandler> XMLSignatureHelper::CreateDocumentHandlerWithHeader(
    const com::sun::star::uno::Reference< com::sun::star::io::XOutputStream >& xOutputStream )
{
    /*
     * get SAX writer component
     */
    uno::Reference< io::XActiveDataSource > xSaxWriter(
        mxMSF->createInstance(rtl::OUString::createFromAscii(
            "com.sun.star.xml.sax.Writer")),
        uno::UNO_QUERY );

    DBG_ASSERT( xSaxWriter.is(), "can't instantiate XML writer" );

    /*
     * connect XML writer to output stream
     */
    xSaxWriter->setOutputStream( xOutputStream );

    /*
     * prepare document handler
     */
    uno::Reference<xml::sax::XDocumentHandler>
        xDocHandler( xSaxWriter,uno::UNO_QUERY);

    /*
     * write the xml context for signatures
     */
    rtl::OUString tag_AllSignatures(RTL_CONSTASCII_USTRINGPARAM(TAG_DOCUMENTSIGNATURES));

    SvXMLAttributeList *pAttributeList = new SvXMLAttributeList();
    pAttributeList->AddAttribute(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(ATTR_XMLNS)),
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(NS_DOCUMENTSIGNATURES)));

    xDocHandler->startDocument();
    xDocHandler->startElement(
        tag_AllSignatures,
        uno::Reference< com::sun::star::xml::sax::XAttributeList > (pAttributeList));

    return xDocHandler;
}

void XMLSignatureHelper::CloseDocumentHandler( const uno::Reference<xml::sax::XDocumentHandler>& xDocumentHandler )
{
    rtl::OUString tag_AllSignatures(RTL_CONSTASCII_USTRINGPARAM(TAG_DOCUMENTSIGNATURES));
    xDocumentHandler->endElement( tag_AllSignatures );
    xDocumentHandler->endDocument();
}

void XMLSignatureHelper::ExportSignature(
    const uno::Reference< xml::sax::XDocumentHandler >& xDocumentHandler,
    const SignatureInformation& signatureInfo )
{
    mpXSecController->exportSignature(xDocumentHandler, signatureInfo);
}

bool XMLSignatureHelper::CreateAndWriteSignature( const uno::Reference< xml::sax::XDocumentHandler >& xDocumentHandler )
{
    mbError = false;

    /*
     * create a signature listener
     */
    ImplXMLSignatureListener* pSignatureListener = new ImplXMLSignatureListener(
                                                    LINK( this, XMLSignatureHelper, SignatureCreationResultListener ),
                                                    LINK( this, XMLSignatureHelper, SignatureVerifyResultListener ),
                                                    LINK( this, XMLSignatureHelper, StartVerifySignatureElement ) );

    /*
     * configure the signature creation listener
     */
    //mpXSecController->setSignatureCreationResultListener( pSignatureListener );

    /*
     * write signatures
     */
    if ( !mpXSecController->WriteSignature( xDocumentHandler ) )
    {
        mbError = true;
    }

    /*
     * clear up the signature creation listener
     */
    //mpXSecController->setSignatureCreationResultListener( NULL );

    return !mbError;
}

bool XMLSignatureHelper::CreateAndWriteSignature( const com::sun::star::uno::Reference< com::sun::star::io::XOutputStream >& xOutputStream )
{
    uno::Reference<xml::sax::XDocumentHandler> xDocHandler
        = CreateDocumentHandlerWithHeader(xOutputStream);

    bool rc = CreateAndWriteSignature( xDocHandler );

    CloseDocumentHandler(xDocHandler);

    return rc;
}

bool XMLSignatureHelper::ReadAndVerifySignature( const com::sun::star::uno::Reference< com::sun::star::io::XInputStream >& xInputStream )
{
    mbError = false;

    DBG_ASSERT(xInputStream.is(), "input stream missing");

    /*
     * prepare ParserInputSrouce
     */
    xml::sax::InputSource aParserInput;
    // aParserInput.sSystemId = ouName;
    aParserInput.aInputStream = xInputStream;

    /*
     * get SAX parser component
     */
    uno::Reference< xml::sax::XParser > xParser(
        mxMSF->createInstance(
            rtl::OUString::createFromAscii("com.sun.star.xml.sax.Parser") ),
        uno::UNO_QUERY );
    DBG_ASSERT( xParser.is(), "Can't create parser" );

    /*
     * create a signature reader
     */
    uno::Reference< xml::sax::XDocumentHandler > xHandler
        = mpXSecController->createSignatureReader( );

    /*
     * create a signature listener
     */
    ImplXMLSignatureListener* pSignatureListener = new ImplXMLSignatureListener(
                                                    LINK( this, XMLSignatureHelper, SignatureCreationResultListener ),
                                                    LINK( this, XMLSignatureHelper, SignatureVerifyResultListener ),
                                                    LINK( this, XMLSignatureHelper, StartVerifySignatureElement ) );

    /*
     * configure the signature verify listener
     */
    //mpXSecController->setSignatureVerifyResultListener( pSignatureListener );

    /*
     * setup the connection:
     * Parser -> SignatureListener -> SignatureReader
     */
    pSignatureListener->setNextHandler(xHandler);
    xParser->setDocumentHandler( pSignatureListener );

    /*
     * parser the stream
     */
    try
    {
        xParser->parseStream( aParserInput );
    }
    catch( xml::sax::SAXParseException& )
    {
        mbError = true;
    }
    catch( xml::sax::SAXException& )
    {
        mbError = true;
    }
    catch( com::sun::star::io::IOException& )
    {
        mbError = true;
    }
    catch( uno::Exception& )
    {
        mbError = true;
    }

    /*
     * clear up the connection
     */
    pSignatureListener->setNextHandler( NULL );

    /*
     * clear up the signature verify listener
     */
    //mpXSecController->setSignatureVerifyResultListener( NULL );

    /*
     * release the signature reader
     */
    mpXSecController->releaseSignatureReader( );

    return !mbError;
}

SignatureInformation XMLSignatureHelper::GetSignatureInformation( sal_Int32 nSecurityId ) const
{
    return mpXSecController->getSignatureInformation( nSecurityId );
}

SignatureInformations XMLSignatureHelper::GetSignatureInformations() const
{
    return mpXSecController->getSignatureInformations();
}

uno::Reference< ::com::sun::star::xml::crypto::XSecurityEnvironment > XMLSignatureHelper::GetSecurityEnvironment()
{
    return (mxSecurityContext.is()?(mxSecurityContext->getSecurityEnvironment()): uno::Reference< ::com::sun::star::xml::crypto::XSecurityEnvironment >());
}

uno::Reference< ::com::sun::star::xml::crypto::XSecurityEnvironment > XMLSignatureHelper::GetSecurityEnvironmentByIndex(sal_Int32 nId)
{
    return (mxSecurityContext.is()?(mxSecurityContext->getSecurityEnvironmentByIndex(nId)): uno::Reference< ::com::sun::star::xml::crypto::XSecurityEnvironment >());
}

sal_Int32 XMLSignatureHelper::GetSecurityEnvironmentNumber()
{
    return (mxSecurityContext.is()?(mxSecurityContext->getSecurityEnvironmentNumber()): 0);
}


/*
void XMLSignatureHelper::createSecurityContext( rtl::OUString tokenPath )
{
    if ( !mxSEInitializer.is() )
        ImplCreateSEInitializer();

    mxSecurityContext = mxSEInitializer->createSecurityContext(tokenPath);
}

void XMLSignatureHelper::freeSecurityContext()
{
    if ( !mxSEInitializer.is() )
        ImplCreateSEInitializer();

    mxSEInitializer->freeSecurityContext( mxSecurityContext );
}
*/

IMPL_LINK( XMLSignatureHelper, SignatureCreationResultListener, XMLSignatureCreationResult*, pResult )
{
    maCreationResults.insert( maCreationResults.begin() + maCreationResults.size(), *pResult );
    if ( pResult->nSignatureCreationResult != com::sun::star::xml::crypto::SecurityOperationStatus_OPERATION_SUCCEEDED )
        mbError = true;
    return 0;
}

IMPL_LINK( XMLSignatureHelper, SignatureVerifyResultListener, XMLSignatureVerifyResult*, pResult )
{
    maVerifyResults.insert( maVerifyResults.begin() + maVerifyResults.size(), *pResult );
    if ( pResult->nSignatureVerifyResult != com::sun::star::xml::crypto::SecurityOperationStatus_OPERATION_SUCCEEDED )
        mbError = true;
    return 0;
}

IMPL_LINK( XMLSignatureHelper, StartVerifySignatureElement, const uno::Reference< com::sun::star::xml::sax::XAttributeList >*, pAttrs )
{
    if ( !maStartVerifySignatureHdl.IsSet() || maStartVerifySignatureHdl.Call( (void*)pAttrs ) )
    {
        sal_Int32 nSignatureId = mpXSecController->getNewSecurityId();
        mpXSecController->addSignature( nSignatureId );
    }

    return 0;
}



