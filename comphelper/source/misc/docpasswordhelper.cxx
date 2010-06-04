/***********************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_comphelper.hxx"

#include "comphelper/docpasswordhelper.hxx"
#include <com/sun/star/task/XInteractionHandler.hpp>
#include "comphelper/mediadescriptor.hxx"

using ::rtl::OUString;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_SET_THROW;
using ::com::sun::star::task::PasswordRequestMode;
using ::com::sun::star::task::PasswordRequestMode_PASSWORD_ENTER;
using ::com::sun::star::task::PasswordRequestMode_PASSWORD_REENTER;
using ::com::sun::star::task::XInteractionHandler;
using ::com::sun::star::task::XInteractionRequest;

namespace comphelper {

// ============================================================================

IDocPasswordVerifier::~IDocPasswordVerifier()
{
}

// ============================================================================
sal_uInt32 DocPasswordHelper::GetWordHashAsUINT32(
                const ::rtl::OUString& aUString )
{
    static sal_uInt16 pInitialCode[] = {
        0xE1F0, // 1
        0x1D0F, // 2
        0xCC9C, // 3
        0x84C0, // 4
        0x110C, // 5
        0x0E10, // 6
        0xF1CE, // 7
        0x313E, // 8
        0x1872, // 9
        0xE139, // 10
        0xD40F, // 11
        0x84F9, // 12
        0x280C, // 13
        0xA96A, // 14
        0x4EC3  // 15
    };

    static sal_uInt16 pEncryptionMatrix[15][7] = {
        { 0xAEFC, 0x4DD9, 0x9BB2, 0x2745, 0x4E8A, 0x9D14, 0x2A09}, // last-14
        { 0x7B61, 0xF6C2, 0xFDA5, 0xEB6B, 0xC6F7, 0x9DCF, 0x2BBF}, // last-13
        { 0x4563, 0x8AC6, 0x05AD, 0x0B5A, 0x16B4, 0x2D68, 0x5AD0}, // last-12
        { 0x0375, 0x06EA, 0x0DD4, 0x1BA8, 0x3750, 0x6EA0, 0xDD40}, // last-11
        { 0xD849, 0xA0B3, 0x5147, 0xA28E, 0x553D, 0xAA7A, 0x44D5}, // last-10
        { 0x6F45, 0xDE8A, 0xAD35, 0x4A4B, 0x9496, 0x390D, 0x721A}, // last-9
        { 0xEB23, 0xC667, 0x9CEF, 0x29FF, 0x53FE, 0xA7FC, 0x5FD9}, // last-8
        { 0x47D3, 0x8FA6, 0x8FA6, 0x1EDA, 0x3DB4, 0x7B68, 0xF6D0}, // last-7
        { 0xB861, 0x60E3, 0xC1C6, 0x93AD, 0x377B, 0x6EF6, 0xDDEC}, // last-6
        { 0x45A0, 0x8B40, 0x06A1, 0x0D42, 0x1A84, 0x3508, 0x6A10}, // last-5
        { 0xAA51, 0x4483, 0x8906, 0x022D, 0x045A, 0x08B4, 0x1168}, // last-4
        { 0x76B4, 0xED68, 0xCAF1, 0x85C3, 0x1BA7, 0x374E, 0x6E9C}, // last-3
        { 0x3730, 0x6E60, 0xDCC0, 0xA9A1, 0x4363, 0x86C6, 0x1DAD}, // last-2
        { 0x3331, 0x6662, 0xCCC4, 0x89A9, 0x0373, 0x06E6, 0x0DCC}, // last-1
        { 0x1021, 0x2042, 0x4084, 0x8108, 0x1231, 0x2462, 0x48C4}  // last
    };

    sal_uInt32 nResult = 0;
    sal_uInt32 nLen = aUString.getLength();

    if ( nLen )
    {
        if ( nLen > 15 )
            nLen = 15;

        sal_uInt16 nHighResult = pInitialCode[nLen - 1];
        sal_uInt16 nLowResult = 0;

        const sal_Unicode* pStr = aUString.getStr();
        for ( sal_uInt32 nInd = 0; nInd < nLen; nInd++ )
        {
            // NO Encoding during conversion!
            // The specification says that the low byte should be used in case it is not NULL
            char nHighChar = (char)( pStr[nInd] >> 8 );
            char nLowChar = (char)( pStr[nInd] & 0xFF );
            char nChar = nLowChar ? nLowChar : nHighChar;

            for ( int nMatrixInd = 0; nMatrixInd < 7; ++nMatrixInd )
            {
                if ( ( nChar & ( 1 << nMatrixInd ) ) != 0 )
                    nHighResult = nHighResult ^ pEncryptionMatrix[15 - nLen + nInd][nMatrixInd];
            }

            nLowResult = ( ( ( nLowResult >> 14 ) & 0x0001 ) | ( ( nLowResult << 1 ) & 0x7FFF ) ) ^ nChar;
        }

        nLowResult = ( ( ( nLowResult >> 14 ) & 0x001 ) | ( ( nLowResult << 1 ) & 0x7FF ) ) ^ nLen ^ 0xCE4B;

        nResult = ( nHighResult << 16 ) | nLowResult;
    }

    return nResult;
}

// ============================================================================
Sequence< sal_Int8 > DocPasswordHelper::GetWordHashAsSequence(
                const ::rtl::OUString& aUString )
{
    sal_uInt32 nHash = GetWordHashAsUINT32( aUString );
    Sequence< sal_Int8 > aResult( 4 );
    aResult[0] = ( nHash >> 24 );
    aResult[1] = ( nHash & 0xFF0000 );
    aResult[2] = ( nHash & 0xFF00 );
    aResult[3] = ( nHash & 0xFF );

    return aResult;
}

// ============================================================================
sal_uInt16 DocPasswordHelper::GetXLHashAsUINT16(
                const ::rtl::OUString& aUString,
                rtl_TextEncoding nEnc )
{
    sal_uInt16 nResult = 0;

    ::rtl::OString aString = ::rtl::OUStringToOString( aUString, nEnc );

    if ( aString.getLength() && aString.getLength() <= SAL_MAX_UINT16 )
    {
        for ( sal_Int32 nInd = aString.getLength() - 1; nInd >= 0; nInd-- )
        {
            nResult = ( ( nResult >> 14 ) & 0x01 ) | ( ( nResult << 1 ) & 0x7FFF );
            nResult ^= aString.getStr()[nInd];
        }

        nResult = ( ( nResult >> 14 ) & 0x01 ) | ( ( nResult << 1 ) & 0x7FFF );
        nResult ^= ( 0x8000 | ( 'N' << 8 ) | 'K' );
        nResult ^= aString.getLength();
    }

    return nResult;
}

// ============================================================================
Sequence< sal_Int8 > DocPasswordHelper::GetXLHashAsSequence(
                const ::rtl::OUString& aUString,
                rtl_TextEncoding nEnc )
{
    sal_uInt16 nHash = GetXLHashAsUINT16( aUString, nEnc );
    Sequence< sal_Int8 > aResult( 2 );
    aResult[0] = ( nHash >> 8 );
    aResult[1] = ( nHash & 0xFF );

    return aResult;
}

// ============================================================================

/*static*/ OUString DocPasswordHelper::requestAndVerifyDocPassword(
        IDocPasswordVerifier& rVerifier,
        const OUString& rMediaPassword,
        const Reference< XInteractionHandler >& rxInteractHandler,
        const OUString& rDocumentName,
        DocPasswordRequestType eRequestType,
        const ::std::vector< OUString >* pDefaultPasswords,
        bool* pbIsDefaultPassword )
{
    OUString aPassword;
    DocPasswordVerifierResult eResult = DocPasswordVerifierResult_WRONG_PASSWORD;

    // first, try provided default passwords
    if( pbIsDefaultPassword )
        *pbIsDefaultPassword = false;
    if( pDefaultPasswords )
    {
        for( ::std::vector< OUString >::const_iterator aIt = pDefaultPasswords->begin(), aEnd = pDefaultPasswords->end(); (eResult == DocPasswordVerifierResult_WRONG_PASSWORD) && (aIt != aEnd); ++aIt )
        {
            aPassword = *aIt;
            OSL_ENSURE( aPassword.getLength() > 0, "DocPasswordHelper::requestAndVerifyDocPassword - unexpected empty default password" );
            if( aPassword.getLength() > 0 )
            {
                eResult = rVerifier.verifyPassword( aPassword );
                if( pbIsDefaultPassword )
                    *pbIsDefaultPassword = eResult == DocPasswordVerifierResult_OK;
            }
        }
    }

    // try media password (skip, if result is OK or ABORT)
    if( eResult == DocPasswordVerifierResult_WRONG_PASSWORD )
    {
        aPassword = rMediaPassword;
        if( aPassword.getLength() > 0 )
            eResult = rVerifier.verifyPassword( aPassword );
    }

    // request a password (skip, if result is OK or ABORT)
    if( (eResult == DocPasswordVerifierResult_WRONG_PASSWORD) && rxInteractHandler.is() ) try
    {
        PasswordRequestMode eRequestMode = PasswordRequestMode_PASSWORD_ENTER;
        while( eResult == DocPasswordVerifierResult_WRONG_PASSWORD )
        {
            DocPasswordRequest* pRequest = new DocPasswordRequest( eRequestType, eRequestMode, rDocumentName );
            Reference< XInteractionRequest > xRequest( pRequest );
            rxInteractHandler->handle( xRequest );
            if( pRequest->isPassword() )
            {
                aPassword = pRequest->getPassword();
                if( aPassword.getLength() > 0 )
                    eResult = rVerifier.verifyPassword( aPassword );
            }
            else
            {
                eResult = DocPasswordVerifierResult_ABORT;
            }
            eRequestMode = PasswordRequestMode_PASSWORD_REENTER;
        }
    }
    catch( Exception& )
    {
    }

    return (eResult == DocPasswordVerifierResult_OK) ? aPassword : OUString();
}

/*static*/ OUString DocPasswordHelper::requestAndVerifyDocPassword(
        IDocPasswordVerifier& rVerifier,
        MediaDescriptor& rMediaDesc,
        DocPasswordRequestType eRequestType,
        const ::std::vector< OUString >* pDefaultPasswords )
{
    OUString aMediaPassword = rMediaDesc.getUnpackedValueOrDefault(
        MediaDescriptor::PROP_PASSWORD(), OUString() );
    Reference< XInteractionHandler > xInteractHandler = rMediaDesc.getUnpackedValueOrDefault(
        MediaDescriptor::PROP_INTERACTIONHANDLER(), Reference< XInteractionHandler >() );
    OUString aDocumentName = rMediaDesc.getUnpackedValueOrDefault(
        MediaDescriptor::PROP_URL(), OUString() );

    bool bIsDefaultPassword = false;
    OUString aPassword = requestAndVerifyDocPassword(
        rVerifier, aMediaPassword, xInteractHandler, aDocumentName, eRequestType, pDefaultPasswords, &bIsDefaultPassword );

    // insert valid password into media descriptor (but not a default password)
    if( (aPassword.getLength() > 0) && !bIsDefaultPassword )
        rMediaDesc[ MediaDescriptor::PROP_PASSWORD() ] <<= aPassword;

    return aPassword;
}

// ============================================================================

} // namespace comphelper

