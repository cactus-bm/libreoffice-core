/*************************************************************************
 *
 *  $RCSfile: digitalsignaturesdialog.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: mt $ $Date: 2004-07-12 13:15:24 $
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

#include <xmlsecurity/digitalsignaturesdialog.hxx>
#include <xmlsecurity/certificatechooser.hxx>
#include <xmlsecurity/certificateviewer.hxx>

#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#include <com/sun/star/embed/XTransactedObject.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/lang/XComponent.hpp>

// Only needed until sig is in storage:
#include <unotools/streamhelper.hxx>

/*-
 * deleted by AF. base64.h is a mozilla nss file, this file should not depend
 * on nss/nspr.
 */
//#include <external/base64.h>

#if OSL_DEBUG_LEVEL > 1
#include <vcl/msgbox.hxx>
#endif

#include <tools/intn.hxx>

#include "dialogs.hrc"
#include "resourcemanager.hxx"

// MM : added for password exception
#include <com/sun/star/security/NoPasswordException.hpp>
using namespace ::com::sun::star::security;

/* HACK: disable some warnings for MS-C */
#ifdef _MSC_VER
#pragma warning (disable : 4355)    // 4355: this used in initializer-list
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star;

DigitalSignaturesDialog::DigitalSignaturesDialog( Window* pParent, uno::Reference< lang::XMultiServiceFactory >& rxMSF, DocumentSignatureMode eMode )
    :ModalDialog        ( pParent, XMLSEC_RES( RID_XMLSECDLG_DIGSIG ) )
    ,maSignatureHelper  ( rxMSF )
    ,meSignatureMode    ( eMode )
    ,maHintDocFT        ( this, ResId( FT_HINT_DOC ) )
    ,maHintBasicFT      ( this, ResId( FT_HINT_BASIC ) )
    ,maHintPackageFT    ( this, ResId( FT_HINT_PACK ) )
    ,maSignaturesLB     ( this, ResId( LB_SIGNATURES ) )
    ,maViewBtn          ( this, ResId( BTN_VIEWCERT ) )
    ,maAddBtn           ( this, ResId( BTN_ADDCERT ) )
    ,maRemoveBtn        ( this, ResId( BTN_REMOVECERT ) )
    ,maBottomSepFL      ( this, ResId( FL_BOTTOM_SEP ) )
    ,maOKBtn            ( this, ResId( BTN_OK ) )
    ,maCancelBtn        ( this, ResId( BTN_CANCEL ) )
    ,maHelpBtn          ( this, ResId( BTN_HELP ) )
{
    static long nTabs[] = { 3, 0, 32*DS_LB_WIDTH/100, 64*DS_LB_WIDTH/100 };
    maSignaturesLB.SetTabs( &nTabs[ 0 ] );
    maSignaturesLB.InsertHeaderEntry( String( ResId( STR_HEADERBAR ) ) );

    FreeResource();
    mbVerifySignatures = true;

    maSignaturesLB.SetSelectHdl( LINK( this, DigitalSignaturesDialog, SignatureHighlightHdl ) );
    maSignaturesLB.SetDoubleClickHdl( LINK( this, DigitalSignaturesDialog, SignatureSelectHdl ) );

    maViewBtn.SetClickHdl( LINK( this, DigitalSignaturesDialog, ViewButtonHdl ) );
    maViewBtn.Disable();

    maAddBtn.SetClickHdl( LINK( this, DigitalSignaturesDialog, AddButtonHdl ) );

    maRemoveBtn.SetClickHdl( LINK( this, DigitalSignaturesDialog, RemoveButtonHdl ) );
    maRemoveBtn.Disable();

    switch( meSignatureMode )
    {
        case SignatureModeDocumentContent:  maHintDocFT.Show();     break;
        case SignatureModeMacros:           maHintBasicFT.Show();   break;
        case SignatureModePackage:          maHintPackageFT.Show(); break;
    }
}

DigitalSignaturesDialog::~DigitalSignaturesDialog()
{
}

BOOL DigitalSignaturesDialog::Init( const rtl::OUString& rTokenName )
{
    bool bInit = maSignatureHelper.Init( rTokenName );

    DBG_ASSERT( bInit, "Error initializing security context!" );

    if ( bInit )
    {
        maSignatureHelper.SetStartVerifySignatureHdl( LINK( this, DigitalSignaturesDialog, StartVerifySignatureHdl ) );
    }

    return bInit;
}

void DigitalSignaturesDialog::SetStorage( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >& rxStore )
{
    mxStore = rxStore;
    maSignatureHelper.SetStorage( mxStore );
}



short DigitalSignaturesDialog::Execute()
{
    // Verify Signatures and add certificates to ListBox...
    mbVerifySignatures = true;
    ImplGetSignatureInformations();
    ImplFillSignaturesBox();

    // Only verify once, content will not change.
    // But for refreshing signature information, StartVerifySignatureHdl will be called after each add/remove
    mbVerifySignatures = false;

    short nRet = Dialog::Execute();

    return nRet;
}

IMPL_LINK( DigitalSignaturesDialog, SignatureHighlightHdl, void*, EMPTYARG )
{
    bool    bSel = maSignaturesLB.FirstSelected() != 0;
    maViewBtn.Enable( bSel );
    maRemoveBtn.Enable( bSel );
    return 0;
}

IMPL_LINK( DigitalSignaturesDialog, SignatureSelectHdl, void*, EMPTYARG )
{
    ImplShowSignaturesDetails();
    return 0;
}

IMPL_LINK( DigitalSignaturesDialog, ViewButtonHdl, Button*, EMPTYARG )
{
    ImplShowSignaturesDetails();
    return 0;
}

IMPL_LINK( DigitalSignaturesDialog, AddButtonHdl, Button*, EMPTYARG )
{
    try
    {
        // Check if we have the password for the key store.
        // If no, we don't want to raise an empty dialog.
        maSignatureHelper.GetSecurityEnvironment()->getPersonalCertificates();

        uno::Reference<com::sun::star::xml::crypto::XSecurityEnvironment> xSecEnv = maSignatureHelper.GetSecurityEnvironment();
        CertificateChooser aChooser( (Window*)this, xSecEnv, aCurrentSignatureInformations );
        if( aChooser.Execute() )
        {
            maSignatureHelper.StartMission();

            sal_Int32 nSecurityId = maSignatureHelper.GetNewSecurityId();
            uno::Reference< ::com::sun::star::security::XCertificate > xCert = aChooser.GetSelectedCertificate();
            maSignatureHelper.SetX509Certificate( nSecurityId, xCert->getIssuerName(), bigIntegerToNumericString( xCert->getSerialNumber() ) );

            std::vector< rtl::OUString > aElements = CreateElementList( mxStore, rtl::OUString(), meSignatureMode );

            ::rtl::OUString aXMLExt( RTL_CONSTASCII_USTRINGPARAM( "XML" ) );
            sal_Int32 nElements = aElements.size();
            for ( sal_Int32 n = 0; n < nElements; n++ )
            {
                bool bBinaryMode = true;
                sal_Int32 nSep = aElements[n].lastIndexOf( '.' );
                if ( nSep != (-1) )
                {
                    ::rtl::OUString aExt = aElements[n].copy( nSep+1 );
                    if ( aExt.equalsIgnoreAsciiCase( aXMLExt ) )
                        bBinaryMode = false;
                }
                maSignatureHelper.AddForSigning( nSecurityId, aElements[n], aElements[n], bBinaryMode );
            }

            bool bDone = false;
            bool bRefreshInformations = false;

            if ( !maSigFileName.getLength() )
            {
                ::rtl::OUString aSIGStreamName( RTL_CONSTASCII_USTRINGPARAM( "SIGNATURES" ) );
                uno::Reference < io::XStream > xStream = mxStore->openStreamElement( aSIGStreamName, embed::ElementModes::WRITE );
                uno::Reference< io::XOutputStream > xOutputStream( xStream, uno::UNO_QUERY );
                // CHANGE TO KEEP OLD SIGS WHEN SIG IN STREAM WORKS!
                //bDone = maSignatureHelper.CreateAndWriteSignatue( xOutputStream );

                uno::Reference< com::sun::star::xml::sax::XDocumentHandler> xDocumentHandler = maSignatureHelper.CreateDocumentHandlerWithHeader( xOutputStream);
                // Export old signatures...
                int nInfos = aCurrentSignatureInformations.size();
                for ( int n = 0; n < nInfos; n++ )
                    maSignatureHelper.ExportSignature( xDocumentHandler, aCurrentSignatureInformations[n]);

                // Create a new one...
                bDone = maSignatureHelper.CreateAndWriteSignatue( xDocumentHandler );

                // That's it...
                maSignatureHelper.CloseDocumentHandler( xDocumentHandler);

                uno::Reference< embed::XTransactedObject > xTrans( mxStore, uno::UNO_QUERY );
                xTrans->commit();
                uno::Reference< lang::XComponent > xComp( mxStore, uno::UNO_QUERY );
                xComp->dispose();
            }
            else
            {
                SvFileStream* pStream = new SvFileStream( maSigFileName, STREAM_WRITE|STREAM_TRUNC );
                SvLockBytesRef xLockBytes = new SvLockBytes( pStream, TRUE );
                 uno::Reference< io::XOutputStream > xOutputStream = new utl::OOutputStreamHelper( xLockBytes );

                uno::Reference< com::sun::star::xml::sax::XDocumentHandler> xDocumentHandler = maSignatureHelper.CreateDocumentHandlerWithHeader( xOutputStream);
                // Export old signatures...
                int nInfos = aCurrentSignatureInformations.size();
                for ( int n = 0; n < nInfos; n++ )
                    maSignatureHelper.ExportSignature( xDocumentHandler, aCurrentSignatureInformations[n]);

                // Create a new one...
                bDone = maSignatureHelper.CreateAndWriteSignatue( xDocumentHandler );

                // That's it...
                maSignatureHelper.CloseDocumentHandler( xDocumentHandler);
            }

            maSignatureHelper.EndMission();

            if ( bDone )
            {
                // Can't simply remember current information, need parsing for getting full information :(
                ImplGetSignatureInformations();
                ImplFillSignaturesBox();
            }

        }
    }
    catch (NoPasswordException&)
    {
    }

    return 0;
}

IMPL_LINK( DigitalSignaturesDialog, RemoveButtonHdl, Button*, EMPTYARG )
{
    if( maSignaturesLB.FirstSelected() )
    {
        USHORT nSelected = (USHORT) maSignaturesLB.FirstSelected()->GetUserData();
        aCurrentSignatureInformations.erase( aCurrentSignatureInformations.begin()+nSelected );

        // Export all other signatures...
        if ( !maSigFileName.getLength() )
        {
            // See AddButtonHdl, later....
        }
        else
        {
            SvFileStream*   pStream = new SvFileStream( maSigFileName, STREAM_WRITE|STREAM_TRUNC );
            SvLockBytesRef  xLockBytes = new SvLockBytes( pStream, TRUE );
             uno::Reference< io::XOutputStream > xOutputStream = new utl::OOutputStreamHelper( xLockBytes );

            uno::Reference< css::xml::sax::XDocumentHandler> xDocumentHandler = maSignatureHelper.CreateDocumentHandlerWithHeader( xOutputStream);

            int nInfos = aCurrentSignatureInformations.size();
            for( int n = 0 ; n < nInfos ; ++n )
                maSignatureHelper.ExportSignature( xDocumentHandler, aCurrentSignatureInformations[ n ] );

            // That's it...
            maSignatureHelper.CloseDocumentHandler( xDocumentHandler );
        }

        ImplFillSignaturesBox();
    }

    return 0;
}

IMPL_LINK( DigitalSignaturesDialog, StartVerifySignatureHdl, void*, EMPTYARG )
{
    return mbVerifySignatures ? 1 : 0;
}

void DigitalSignaturesDialog::ImplFillSignaturesBox()
{
    maSignaturesLB.Clear();

    uno::Reference< ::com::sun::star::xml::crypto::XSecurityEnvironment > xSecEnv = maSignatureHelper.GetSecurityEnvironment();
    uno::Reference< ::com::sun::star::security::XCertificate > xCert;

    String  aCN_Id( String::CreateFromAscii( "CN" ) );
    int     nInfos = aCurrentSignatureInformations.size();
    for( int n = 0; n < nInfos; ++n )
    {
        const SignatureInformation& rInfo = aCurrentSignatureInformations[n];
        xCert = xSecEnv->getCertificate( rInfo.ouX509IssuerName, numericStringToBigInteger( rInfo.ouX509SerialNumber ) );

        // If we don't get it, create it from signature data:
        if ( !xCert.is() )
            xCert = xSecEnv->createCertificateFromAscii( rInfo.ouX509Certificate ) ;

        DBG_ASSERT( xCert.is(), "Certificate not found and can't be created!" );

        String  aSubject;
        String  aIssuer;
        String  aDateTimeStr;
        if( xCert.is() )
        {
            aSubject = XmlSec::GetContentPart( xCert->getSubjectName(), aCN_Id );
            aIssuer = XmlSec::GetContentPart( rInfo.ouX509IssuerName, aCN_Id );
            aDateTimeStr = XmlSec::GetDateString( xCert->getNotAfter() );
        }
        else
        {
#if OSL_DEBUG_LEVEL > 1
            aSubject = String::CreateFromAscii( "ERROR getting certificate!" );
#endif
        }
        SvLBoxEntry* pEntry = maSignaturesLB.InsertEntry( aSubject );
        maSignaturesLB.SetEntryText( aIssuer, pEntry, 1 );
        maSignaturesLB.SetEntryText( aDateTimeStr, pEntry, 2 );
        pEntry->SetUserData( ( void* ) n );     // missuse user data as index
    }

//  maViewBtn.Disable();
    SignatureHighlightHdl( NULL );
}

void DigitalSignaturesDialog::ImplGetSignatureInformations()
{
    aCurrentSignatureInformations.clear();

    maSignatureHelper.StartMission();

    bool bVerifyOK = false;

    if ( maSigFileName.getLength() )
    {
        SvFileStream* pStream = new SvFileStream( maSigFileName, STREAM_READ );
        pStream->Seek( STREAM_SEEK_TO_END );
        ULONG nBytes = pStream->Tell();
        pStream->Seek( STREAM_SEEK_TO_BEGIN );
        SvLockBytesRef xLockBytes = new SvLockBytes( pStream, TRUE );
        if ( nBytes )
        {
             uno::Reference< io::XInputStream > xInputStream = new utl::OInputStreamHelper( xLockBytes, nBytes );
            bVerifyOK = maSignatureHelper.ReadAndVerifySignatue( xInputStream );
            xInputStream->closeInput();
        }
    }
    else
    {
        ::rtl::OUString aSIGStreamName( RTL_CONSTASCII_USTRINGPARAM( "SIGNATURE" ) );
        if ( mxStore->isStreamElement( aSIGStreamName ) )
        {
            uno::Reference < io::XStream > xStream = mxStore->openStreamElement( aSIGStreamName, embed::ElementModes::READ );
            uno::Reference< io::XInputStream > xInputStream( xStream, uno::UNO_QUERY );
            bVerifyOK = maSignatureHelper.ReadAndVerifySignatue( xInputStream );
        }
    }

    if ( bVerifyOK )
        aCurrentSignatureInformations = maSignatureHelper.GetSignatureInformations();

    maSignatureHelper.EndMission();

    mbVerifySignatures = false;
}

void DigitalSignaturesDialog::ImplShowSignaturesDetails()
{
    if( maSignaturesLB.FirstSelected() )
    {
        USHORT nSelected = (USHORT) maSignaturesLB.FirstSelected()->GetUserData();
        const SignatureInformation& rInfo = aCurrentSignatureInformations[ nSelected ];
        uno::Reference< dcss::security::XCertificate > xCert = maSignatureHelper.GetSecurityEnvironment()->getCertificate( rInfo.ouX509IssuerName, numericStringToBigInteger( rInfo.ouX509SerialNumber ) );

        // If we don't get it, create it from signature data:
        if ( !xCert.is() )
            xCert = maSignatureHelper.GetSecurityEnvironment()->createCertificateFromAscii( rInfo.ouX509Certificate ) ;

        DBG_ASSERT( xCert.is(), "Certificate not found and can't be created!" );

        uno::Reference<com::sun::star::xml::crypto::XSecurityEnvironment> xSecEnv = maSignatureHelper.GetSecurityEnvironment();
        CertificateViewer aViewer( this, xSecEnv, xCert );
        aViewer.Execute();
    }
}

void ImplFillElementList( std::vector< rtl::OUString >& rList, uno::Reference < embed::XStorage >& rxStore, const ::rtl::OUString rRootStorageName, bool bRecursive )
{
    ::rtl::OUString aMetaInfName( RTL_CONSTASCII_USTRINGPARAM( "META-INF" ) );
    ::rtl::OUString aSep( RTL_CONSTASCII_USTRINGPARAM( "/" ) );

    uno::Reference < container::XNameAccess > xElements( rxStore, uno::UNO_QUERY );
    uno::Sequence< ::rtl::OUString > aElements = xElements->getElementNames();
    sal_Int32 nElements = aElements.getLength();
    const ::rtl::OUString* pNames = aElements.getConstArray();
    for ( sal_Int32 n = 0; n < nElements; n++ )
    {
        if ( pNames[n] != aMetaInfName )
        {
            if ( rxStore->isStreamElement( pNames[n] ) )
            {
                ::rtl::OUString aFullName( rRootStorageName + pNames[n] );
                rList.push_back( aFullName );
            }
            else if ( bRecursive && rxStore->isStorageElement( pNames[n] ) )
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( pNames[n], embed::ElementModes::READ );
                rtl::OUString aFullRootName( rRootStorageName + pNames[n] + aSep );
                ImplFillElementList( rList, rxStore, aFullRootName, bRecursive );
            }
        }
    }
}

std::vector< rtl::OUString > DigitalSignaturesDialog::CreateElementList( uno::Reference < embed::XStorage >& rxStore, const ::rtl::OUString rRootStorageName, DocumentSignatureMode eMode )
{
    std::vector< rtl::OUString > aElements;
    ::rtl::OUString aSep( RTL_CONSTASCII_USTRINGPARAM( "/" ) );


    switch ( eMode )
    {
        case SignatureModeDocumentContent:
        {
            // 1) Main content
            ImplFillElementList( aElements, rxStore, ::rtl::OUString(), false );

            // 2) Pictures...
            rtl::OUString aSubStorageName( rtl::OUString::createFromAscii( "Pictures" ) );
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }
        }
        break;
        case SignatureModeMacros:
        {
            // 1) Macros
            rtl::OUString aSubStorageName( rtl::OUString::createFromAscii( "Basic" ) );
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }

            // 2) Dialogs
            aSubStorageName = rtl::OUString::createFromAscii( "Dialogs") ;
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }
        }
        break;
        case SignatureModePackage:
        {
            // Everything except META-INF
            ImplFillElementList( aElements, rxStore, ::rtl::OUString(), true );
        }
        break;
    }

    return aElements;
}
