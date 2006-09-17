/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XUnbufferedStream.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 17:27:48 $
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
#include "precompiled_package.hxx"
#ifndef _XUNBUFFERED_STREAM_HXX
#include <XUnbufferedStream.hxx>
#endif
#ifndef _ENCRYPTION_DATA_HXX_
#include <EncryptionData.hxx>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPCONSTANTS_HPP_
#include <com/sun/star/packages/zip/ZipConstants.hpp>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPIOEXCEPTION_HPP_
#include <com/sun/star/packages/zip/ZipIOException.hpp>
#endif
#ifndef _PACKAGE_CONSTANTS_HXX_
#include <PackageConstants.hxx>
#endif
#ifndef _RTL_CIPHER_H_
#include <rtl/cipher.h>
#endif
#ifndef _ZIP_FILE_HXX
#include <ZipFile.hxx>
#endif
#ifndef _ENCRYPTED_DATA_HEADER_HXX_
#include <EncryptedDataHeader.hxx>
#endif
#include <algorithm>

#if 0
// for debugging purposes here
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <comphelper/processfactory.hxx>
using namespace ::com::sun::star;
#endif

using namespace com::sun::star::packages::zip::ZipConstants;
using namespace com::sun::star::io;
using namespace com::sun::star::uno;
using com::sun::star::lang::IllegalArgumentException;
using com::sun::star::packages::zip::ZipIOException;
using ::rtl::OUString;

XUnbufferedStream::XUnbufferedStream( ZipEntry & rEntry,
                           Reference < XInputStream > xNewZipStream,
                           const vos::ORef < EncryptionData > &rData,
                           sal_Int8 nStreamMode,
                           sal_Bool bIsEncrypted,
                          const ::rtl::OUString& aMediaType,
                          sal_Bool bRecoveryMode )
: mxZipStream ( xNewZipStream )
, mxZipSeek ( xNewZipStream, UNO_QUERY )
, maEntry ( rEntry )
, mxData ( rData )
, maCipher ( NULL )
, maInflater ( sal_True )
, mbRawStream ( nStreamMode == UNBUFF_STREAM_RAW || nStreamMode == UNBUFF_STREAM_WRAPPEDRAW )
, mbWrappedRaw ( nStreamMode == UNBUFF_STREAM_WRAPPEDRAW )
, mbFinished ( sal_False )
, mnHeaderToRead ( 0 )
, mnZipCurrent ( 0 )
, mnZipEnd ( 0 )
, mnZipSize ( 0 )
, mnMyCurrent ( 0 )
, mbCheckCRC( !bRecoveryMode )
{
    mnZipCurrent = maEntry.nOffset;
    if ( mbRawStream )
    {
        mnZipSize = maEntry.nMethod == DEFLATED ? maEntry.nCompressedSize : maEntry.nSize;
        mnZipEnd = maEntry.nOffset + mnZipSize;
    }
    else
    {
        mnZipSize = maEntry.nSize;
        mnZipEnd = maEntry.nMethod == DEFLATED ? maEntry.nOffset + maEntry.nCompressedSize : maEntry.nOffset + maEntry.nSize;
    }
    sal_Bool bHaveEncryptData = ( !rData.isEmpty() && rData->aSalt.getLength() && rData->aInitVector.getLength() && rData->nIterationCount != 0 ) ? sal_True : sal_False;
    sal_Bool bMustDecrypt = ( nStreamMode == UNBUFF_STREAM_DATA && bHaveEncryptData && bIsEncrypted ) ? sal_True : sal_False;

    if ( bMustDecrypt )
        ZipFile::StaticGetCipher ( rData, maCipher );
    if ( bHaveEncryptData && mbWrappedRaw && bIsEncrypted )
    {
        // if we have the data needed to decrypt it, but didn't want it decrypted (or
        // we couldn't decrypt it due to wrong password), then we prepend this
        // data to the stream

        // Make a buffer big enough to hold both the header and the data itself
        maHeader.realloc  ( n_ConstHeaderSize +
                            rData->aInitVector.getLength() +
                            rData->aSalt.getLength() +
                            rData->aDigest.getLength() +
                            aMediaType.getLength() * sizeof( sal_Unicode ) );
        sal_Int8 * pHeader = maHeader.getArray();
        ZipFile::StaticFillHeader ( rData, rEntry.nSize, aMediaType, pHeader );
        mnHeaderToRead = static_cast < sal_Int16 > ( maHeader.getLength() );
    }
}

// allows to read package raw stream
XUnbufferedStream::XUnbufferedStream( const Reference < XInputStream >& xRawStream,
                    const vos::ORef < EncryptionData > &rData )
: mxZipStream ( xRawStream )
, mxZipSeek ( xRawStream, UNO_QUERY )
, mxData ( rData )
, maCipher ( NULL )
, maInflater ( sal_True )
, mbRawStream ( sal_False )
, mbWrappedRaw ( sal_False )
, mbFinished ( sal_False )
, mnHeaderToRead ( 0 )
, mnZipCurrent ( 0 )
, mnZipEnd ( 0 )
, mnZipSize ( 0 )
, mnMyCurrent ( 0 )
, mbCheckCRC( sal_False )
{
    // for this scenario maEntry is not set !!!
    OSL_ENSURE( mxZipSeek.is(), "The stream must be seekable!\n" );

    // skip raw header, it must be already parsed to rData
    mnZipCurrent = n_ConstHeaderSize + rData->aInitVector.getLength() +
                            rData->aSalt.getLength() + rData->aDigest.getLength();

    try {
        if ( mxZipSeek.is() )
            mnZipSize = mxZipSeek->getLength();
    } catch( Exception& )
    {
        // in case of problem the size will stay set to 0
    }

    mnZipEnd = mnZipCurrent + mnZipSize;

    ZipFile::StaticGetCipher ( rData, maCipher );
}

XUnbufferedStream::~XUnbufferedStream()
{
    if ( maCipher )
        rtl_cipher_destroy ( maCipher );
}

sal_Int32 SAL_CALL XUnbufferedStream::readBytes( Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead )
        throw( NotConnectedException, BufferSizeExceededException, IOException, RuntimeException)
{
    sal_Int32 nRequestedBytes = nBytesToRead;
    OSL_ENSURE( !mnHeaderToRead || mbWrappedRaw, "Only encrypted raw stream can be provided with header!" );
    if ( mnMyCurrent + nRequestedBytes > mnZipSize + maHeader.getLength() )
        nRequestedBytes = static_cast < sal_Int32 > ( mnZipSize + maHeader.getLength() - mnMyCurrent );

    sal_Int32 nRead = 0, nLastRead = 0, nTotal = 0;
    aData.realloc ( nRequestedBytes );
    if ( nRequestedBytes )
    {
        if ( mbRawStream )
        {
            sal_Int64 nDiff = mnZipEnd - mnZipCurrent;

            if ( mbWrappedRaw && mnHeaderToRead )
            {
                sal_Int16 nHeadRead = static_cast< sal_Int16 >(( nRequestedBytes > mnHeaderToRead ?
                                                                                 mnHeaderToRead : nRequestedBytes ));
                memcpy ( aData.getArray(), maHeader.getConstArray() + maHeader.getLength() - mnHeaderToRead, nHeadRead );
                mnHeaderToRead = mnHeaderToRead - nHeadRead;

                if ( nHeadRead < nRequestedBytes )
                {
                    sal_Int32 nToRead = nRequestedBytes - nHeadRead;
                    nToRead = ( nDiff < nToRead ) ? sal::static_int_cast< sal_Int32 >( nDiff ) : nToRead;

                    Sequence< sal_Int8 > aPureData( nToRead );
                    mxZipSeek->seek ( mnZipCurrent );
                    nRead = mxZipStream->readBytes ( aPureData, nToRead );
                    mnZipCurrent += nRead;

                    aPureData.realloc( nRead );
                    if ( mbCheckCRC )
                        maCRC.update( aPureData );

                    aData.realloc( nHeadRead + nRead );

                    sal_Int8* pPureBuffer = aPureData.getArray();
                    sal_Int8* pBuffer = aData.getArray();
                    for ( sal_Int32 nInd = 0; nInd < nRead; nInd++ )
                        pBuffer[ nHeadRead + nInd ] = pPureBuffer[ nInd ];
                }

                nRead += nHeadRead;
            }
            else
            {
                mxZipSeek->seek ( mnZipCurrent );

                nRead = mxZipStream->readBytes (
                                        aData,
                                        static_cast < sal_Int32 > ( nDiff < nRequestedBytes ? nDiff : nRequestedBytes ) );

                mnZipCurrent += nRead;

                aData.realloc( nRead );
                if ( mbWrappedRaw && mbCheckCRC )
                    maCRC.update( aData );
            }
        }
        else
        {
            while ( 0 == ( nLastRead = maInflater.doInflateSegment( aData, nRead, aData.getLength() - nRead ) ) ||
                  ( nRead + nLastRead != nRequestedBytes && mnZipCurrent < mnZipEnd ) )
            {
                nRead += nLastRead;

                if ( nRead > nRequestedBytes )
                    throw RuntimeException(
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "Should not be possible to read more then requested!" ) ),
                        Reference< XInterface >() );

                if ( maInflater.finished() )
                    throw ZipIOException( OUString( RTL_CONSTASCII_USTRINGPARAM( "The stream seems to be broken!" ) ),
                                        Reference< XInterface >() );

                if ( maInflater.needsDictionary() )
                    throw ZipIOException( OUString( RTL_CONSTASCII_USTRINGPARAM( "Dictionaries are not supported!" ) ),
                                        Reference< XInterface >() );

                sal_Int32 nDiff = static_cast < sal_Int32 > ( mnZipEnd - mnZipCurrent );
                if ( nDiff > 0 )
                {
                    mxZipSeek->seek ( mnZipCurrent );
                    sal_Int32 nToRead = std::min ( nDiff, std::max ( nRequestedBytes, static_cast< sal_Int32 >( 8192 ) ) );
                    sal_Int32 nZipRead = mxZipStream->readBytes ( maCompBuffer, nToRead );
                    mnZipCurrent += nZipRead;
                    // maCompBuffer now has the data, check if we need to decrypt
                    // before passing to the Inflater
                    if ( maCipher )
                    {
                        if ( mbCheckCRC )
                            maCRC.update( maCompBuffer );

                        Sequence < sal_Int8 > aCryptBuffer ( nZipRead );
                         rtlCipherError aResult =
                            rtl_cipher_decode ( maCipher,
                                      maCompBuffer.getConstArray(),
                                      nZipRead,
                                      reinterpret_cast < sal_uInt8 * > (aCryptBuffer.getArray()),
                                      nZipRead);
                        if( aResult != rtl_Cipher_E_None ) {
                            OSL_ASSERT (aResult == rtl_Cipher_E_None);
                        }
                        maCompBuffer = aCryptBuffer; // Now it holds the decrypted data

                    }
                    maInflater.setInput ( maCompBuffer );
                }
                else
                {
                    throw ZipIOException( OUString( RTL_CONSTASCII_USTRINGPARAM( "The stream seems to be broken!" ) ),
                                        Reference< XInterface >() );
                }
            }
        }

        mnMyCurrent += nRead + nLastRead;
        nTotal = nRead + nLastRead;
        if ( nTotal < nRequestedBytes)
            aData.realloc ( nTotal );

        if ( mbCheckCRC && ( !mbRawStream || mbWrappedRaw ) )
        {
            if ( !maCipher && !mbWrappedRaw )
                maCRC.update( aData );

#if 0
            // for debugging purposes here
            if ( mbWrappedRaw )
            {
                if ( 0 )
                {
                    uno::Reference< lang::XMultiServiceFactory > xFactory = comphelper::getProcessServiceFactory();
                    uno::Reference< ucb::XSimpleFileAccess > xAccess( xFactory->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ), uno::UNO_QUERY );
                    uno::Reference< io::XOutputStream > xOut = xAccess->openFileWrite( ::rtl::OUString::createFromAscii( "file:///d:/777/Encrypted/picture" ) );
                    xOut->writeBytes( aData );
                    xOut->closeOutput();
                }
            }
#endif

            if ( mnZipSize + maHeader.getLength() == mnMyCurrent && maCRC.getValue() != maEntry.nCrc )
                throw ZipIOException( OUString( RTL_CONSTASCII_USTRINGPARAM( "The stream seems to be broken!" ) ),
                                    Reference< XInterface >() );
        }
    }

    return nTotal;
}

sal_Int32 SAL_CALL XUnbufferedStream::readSomeBytes( Sequence< sal_Int8 >& aData, sal_Int32 nMaxBytesToRead )
        throw( NotConnectedException, BufferSizeExceededException, IOException, RuntimeException)
{
    return readBytes ( aData, nMaxBytesToRead );
}
void SAL_CALL XUnbufferedStream::skipBytes( sal_Int32 nBytesToSkip )
        throw( NotConnectedException, BufferSizeExceededException, IOException, RuntimeException)
{
    if ( nBytesToSkip )
    {
        Sequence < sal_Int8 > aSequence ( nBytesToSkip );
        readBytes ( aSequence, nBytesToSkip );
    }
}

sal_Int32 SAL_CALL XUnbufferedStream::available(  )
        throw( NotConnectedException, IOException, RuntimeException)
{
    return static_cast < sal_Int32 > ( mnZipSize - mnMyCurrent );
}

void SAL_CALL XUnbufferedStream::closeInput(  )
        throw( NotConnectedException, IOException, RuntimeException)
{
}
/*
void SAL_CALL XUnbufferedStream::seek( sal_Int64 location )
        throw( IllegalArgumentException, IOException, RuntimeException)
{
}
sal_Int64 SAL_CALL XUnbufferedStream::getPosition(  )
        throw(IOException, RuntimeException)
{
    return mnMyCurrent;
}
sal_Int64 SAL_CALL XUnbufferedStream::getLength(  )
        throw(IOException, RuntimeException)
{
    return mnZipSize;
}
*/
