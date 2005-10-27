/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ZipPackageFolder.cxx,v $
 *
 *  $Revision: 1.79 $
 *
 *  last change: $Author: hr $ $Date: 2005-10-27 14:14:09 $
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
#ifndef _ZIP_PACKAGE_FOLDER_HXX
#include <ZipPackageFolder.hxx>
#endif
#ifndef _ZIP_FILE_HXX
#include <ZipFile.hxx>
#endif
#ifndef _ZIP_OUTPUT_STREAM_HXX
#include <ZipOutputStream.hxx>
#endif
#ifndef _ZIP_PACKAGE_STREAM_HXX
#include <ZipPackageStream.hxx>
#endif
#ifndef _PACKAGE_CONSTANTS_HXX_
#include <PackageConstants.hxx>
#endif
#ifndef _ZIP_PACKAGE_FOLDER_ENUMERATION_HXX
#include <ZipPackageFolderEnumeration.hxx>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPCONSTANTS_HPP_
#include <com/sun/star/packages/zip/ZipConstants.hpp>
#endif
#ifndef _VOS_DIAGNOSE_H_
#include <vos/diagnose.hxx>
#endif
#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif
#ifndef _RTL_DIGEST_H_
#include <rtl/digest.h>
#endif
#ifndef _CONTENT_INFO_HXX_
#include <ContentInfo.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XSEEKABLE_HPP_
#include <com/sun/star/io/XSeekable.hpp>
#endif
#ifndef _ENCRYPTED_DATA_HEADER_HXX_
#include <EncryptedDataHeader.hxx>
#endif
#ifndef _RTL_RANDOM_H_
#include <rtl/random.h>
#endif
#include <memory>

using namespace com::sun::star::packages::zip::ZipConstants;
using namespace com::sun::star::packages::zip;
using namespace com::sun::star::container;
using namespace com::sun::star::packages;
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::io;
using namespace cppu;
using namespace rtl;
using namespace std;
using vos::ORef;

Sequence < sal_Int8 > ZipPackageFolder::aImplementationId = Sequence < sal_Int8 > ();

ZipPackageFolder::ZipPackageFolder ( const Reference< XMultiServiceFactory >& xFactory,
                                     sal_Bool bPackageFormat,
                                     sal_Bool bAllowRemoveOnInsert )
: m_xFactory( xFactory )
, m_bPackageFormat( bPackageFormat )
{
    OSL_ENSURE( m_xFactory.is(), "No factory is provided to the package folder!" );

    this->mbAllowRemoveOnInsert = bAllowRemoveOnInsert;

    SetFolder ( sal_True );
    aEntry.nVersion     = -1;
    aEntry.nFlag        = 0;
    aEntry.nMethod      = STORED;
    aEntry.nTime        = -1;
    aEntry.nCrc         = 0;
    aEntry.nCompressedSize  = 0;
    aEntry.nSize        = 0;
    aEntry.nOffset      = -1;
    if ( !aImplementationId.getLength() )
        {
        aImplementationId = getImplementationId();
        }
}


ZipPackageFolder::~ZipPackageFolder()
{
}

void ZipPackageFolder::copyZipEntry( ZipEntry &rDest, const ZipEntry &rSource)
{
      rDest.nVersion            = rSource.nVersion;
    rDest.nFlag             = rSource.nFlag;
    rDest.nMethod           = rSource.nMethod;
    rDest.nTime             = rSource.nTime;
    rDest.nCrc              = rSource.nCrc;
    rDest.nCompressedSize   = rSource.nCompressedSize;
    rDest.nSize             = rSource.nSize;
    rDest.nOffset           = rSource.nOffset;
    rDest.sName             = rSource.sName;
    rDest.nNameLen          = rSource.nNameLen;
    rDest.nExtraLen         = rSource.nExtraLen;
}

    // XNameContainer
void SAL_CALL ZipPackageFolder::insertByName( const OUString& aName, const Any& aElement )
        throw(IllegalArgumentException, ElementExistException, WrappedTargetException, RuntimeException)
{
    if (hasByName(aName))
        throw ElementExistException();
    else
    {
        Reference < XUnoTunnel > xRef;
        aElement >>= xRef;
        if ( (  aElement >>= xRef ) )
        {
            sal_Int64 nTest;
            ZipPackageEntry *pEntry;
            if ( ( nTest = xRef->getSomething ( ZipPackageFolder::static_getImplementationId() ) ) != 0 )
            {
                ZipPackageFolder *pFolder = reinterpret_cast < ZipPackageFolder * > ( nTest );
                pEntry = static_cast < ZipPackageEntry * > ( pFolder );
            }
            else if ( ( nTest = xRef->getSomething ( ZipPackageStream::static_getImplementationId() ) ) != 0 )
            {
                ZipPackageStream *pStream = reinterpret_cast < ZipPackageStream * > ( nTest );
                pEntry = static_cast < ZipPackageEntry * > ( pStream );
            }
            else
                throw IllegalArgumentException();

            if (pEntry->getName() != aName )
                pEntry->setName (aName);
            doInsertByName ( pEntry, sal_True );
        }
        else
            throw IllegalArgumentException();
    }
}
void SAL_CALL ZipPackageFolder::removeByName( const OUString& Name )
        throw(NoSuchElementException, WrappedTargetException, RuntimeException)
{
    ContentHash::iterator aIter = maContents.find ( Name );
    if ( aIter == maContents.end() )
        throw NoSuchElementException();
    maContents.erase( aIter );
}
    // XEnumerationAccess
Reference< XEnumeration > SAL_CALL ZipPackageFolder::createEnumeration(  )
        throw(RuntimeException)
{
    return Reference < XEnumeration> (new ZipPackageFolderEnumeration(maContents));
}
    // XElementAccess
Type SAL_CALL ZipPackageFolder::getElementType(  )
        throw(RuntimeException)
{
    return ::getCppuType ((const Reference< XUnoTunnel > *) 0);
}
sal_Bool SAL_CALL ZipPackageFolder::hasElements(  )
        throw(RuntimeException)
{
    return maContents.size() > 0;
}
    // XNameAccess
ContentInfo& ZipPackageFolder::doGetByName( const OUString& aName )
    throw(NoSuchElementException, WrappedTargetException, RuntimeException)
{
    ContentHash::iterator aIter = maContents.find ( aName );
    if ( aIter == maContents.end())
        throw NoSuchElementException();
    return *(*aIter).second;
}
Any SAL_CALL ZipPackageFolder::getByName( const OUString& aName )
    throw(NoSuchElementException, WrappedTargetException, RuntimeException)
{
    return makeAny ( doGetByName ( aName ).xTunnel );
}
Sequence< OUString > SAL_CALL ZipPackageFolder::getElementNames(  )
        throw(RuntimeException)
{
    sal_uInt32 i=0, nSize = maContents.size();
    Sequence < OUString > aSequence ( nSize );
    OUString *pNames = aSequence.getArray();
    for ( ContentHash::const_iterator aIterator = maContents.begin(), aEnd = maContents.end();
          aIterator != aEnd;
          ++i, ++aIterator)
        pNames[i] = (*aIterator).first;
    return aSequence;
}
sal_Bool SAL_CALL ZipPackageFolder::hasByName( const OUString& aName )
    throw(RuntimeException)
{
    return maContents.find ( aName ) != maContents.end ();
}
    // XNameReplace
void SAL_CALL ZipPackageFolder::replaceByName( const OUString& aName, const Any& aElement )
        throw(IllegalArgumentException, NoSuchElementException, WrappedTargetException, RuntimeException)
{
    if ( hasByName( aName ) )
        removeByName( aName );
    else
        throw NoSuchElementException();
    insertByName(aName, aElement);
}

static void ImplSetStoredData( ZipEntry & rEntry, Reference < XInputStream> & rStream )
{
    // It's very annoying that we have to do this, but lots of zip packages
    // don't allow data descriptors for STORED streams, meaning we have to
    // know the size and CRC32 of uncompressed streams before we actually
    // write them !
    CRC32 aCRC32;
    rEntry.nMethod = STORED;
    rEntry.nCompressedSize = rEntry.nSize = aCRC32.updateStream ( rStream );
    rEntry.nCrc = aCRC32.getValue();
}

void ZipPackageFolder::saveContents(OUString &rPath, std::vector < Sequence < PropertyValue > > &rManList, ZipOutputStream & rZipOut, Sequence < sal_Int8 > &rEncryptionKey, rtlRandomPool &rRandomPool)
    throw(RuntimeException)
{
    sal_Bool bWritingFailed = sal_False;
    ZipPackageFolder *pFolder = NULL;
    ZipPackageStream *pStream = NULL;
    const OUString sMediaTypeProperty ( RTL_CONSTASCII_USTRINGPARAM ( "MediaType" ) );
    const OUString sFullPathProperty ( RTL_CONSTASCII_USTRINGPARAM ( "FullPath" ) );
    const OUString sInitialisationVectorProperty ( RTL_CONSTASCII_USTRINGPARAM ( "InitialisationVector" ) );
    const OUString sSaltProperty ( RTL_CONSTASCII_USTRINGPARAM ( "Salt" ) );
    const OUString sIterationCountProperty ( RTL_CONSTASCII_USTRINGPARAM ( "IterationCount" ) );
    const OUString sSizeProperty ( RTL_CONSTASCII_USTRINGPARAM ( "Size" ) );
    const OUString sDigestProperty ( RTL_CONSTASCII_USTRINGPARAM ( "Digest" ) );

    sal_Bool bHaveEncryptionKey = rEncryptionKey.getLength() ? sal_True : sal_False;

    if ( maContents.begin() == maContents.end() && rPath.getLength() )
    {
        // it is an empty subfolder, use workaround to store it
        ZipEntry* pTempEntry = new ZipEntry();
        ZipPackageFolder::copyZipEntry ( *pTempEntry, aEntry );
        pTempEntry->nNameLen = (sal_Int16)rPath.getLength();
        pTempEntry->nExtraLen = -1;
        pTempEntry->sName = rPath;

        try
        {
            vos::ORef < EncryptionData > aEmptyEncr;
            rZipOut.putNextEntry ( *pTempEntry, aEmptyEncr, sal_False );
            rZipOut.rawCloseEntry();
        }
        catch ( ZipException& )
        {
            VOS_ENSURE( 0, "Error writing ZipOutputStream" );
            bWritingFailed = sal_True;
        }
        catch ( IOException& )
        {
            VOS_ENSURE( 0, "Error writing ZipOutputStream" );
            bWritingFailed = sal_True;
        }
    }

    for ( ContentHash::const_iterator aCI = maContents.begin(), aEnd = maContents.end();
          aCI != aEnd;
          aCI++)
    {
        const OUString &rShortName = (*aCI).first;
        const ContentInfo &rInfo = *(*aCI).second;

        Sequence < PropertyValue > aPropSet (2);
        PropertyValue *pValue = aPropSet.getArray();

        if ( rInfo.bFolder )
            pFolder = rInfo.pFolder;
        else
            pStream = rInfo.pStream;

        if ( rInfo.bFolder )
        {
            OUString sTempName = rPath + rShortName + OUString( RTL_CONSTASCII_USTRINGPARAM ( "/" ) );

            pValue[0].Name = sMediaTypeProperty;
            pValue[0].Value <<= pFolder->GetMediaType();
            pValue[1].Name = sFullPathProperty;
            pValue[1].Value <<= sTempName;

            pFolder->saveContents( sTempName, rManList, rZipOut, rEncryptionKey, rRandomPool);
        }
        else
        {
            // if pTempEntry is necessary, it will be released and passed to the ZipOutputStream
            // and be deleted in the ZipOutputStream destructor
            auto_ptr < ZipEntry > pTempEntry ( new ZipEntry );

            // In case the entry we are reading is also the entry we are writing, we will
            // store the ZipEntry data in pTempEntry

            ZipPackageFolder::copyZipEntry ( *pTempEntry, pStream->aEntry );
            pTempEntry->sName = rPath + rShortName;
            pTempEntry->nNameLen = (sal_Int16)( pTempEntry->sName.getLength() );

            sal_Bool bToBeEncrypted = pStream->IsToBeEncrypted() && (bHaveEncryptionKey || pStream->HasOwnKey());
            sal_Bool bToBeCompressed = bToBeEncrypted ? sal_True : pStream->IsToBeCompressed();

            pValue[0].Name = sMediaTypeProperty;
            pValue[0].Value <<= pStream->GetMediaType( );
            pValue[1].Name = sFullPathProperty;
            pValue[1].Value <<= pTempEntry->sName;


            OSL_ENSURE( pStream->GetStreamMode() != PACKAGE_STREAM_NOTSET, "Unacceptable ZipPackageStream mode!" );

            sal_Bool bRawStream = sal_False;
            if ( pStream->GetStreamMode() == PACKAGE_STREAM_DETECT )
                bRawStream = pStream->ParsePackageRawStream();
            else if ( pStream->GetStreamMode() == PACKAGE_STREAM_RAW )
                bRawStream = sal_True;

            sal_Bool bTransportOwnEncrStreamAsRaw = sal_False;
            // During the storing the original size of the stream can be changed
            // TODO/LATER: get rid of this hack
            sal_Int32 nOwnStreamOrigSize = bRawStream ? pStream->GetMagicalHackSize() : pStream->getSize();

            sal_Bool bUseNonSeekableAccess = sal_False;
            Reference < XInputStream > xStream;
            if ( !pStream->IsPackageMember() && !bRawStream && !bToBeEncrypted && bToBeCompressed )
            {
                // the stream is not a package member, not a raw stream,
                // it should not be encrypted and it should be compressed,
                // in this case nonseekable access can be used

                xStream = pStream->GetOwnStreamNoWrap();
                Reference < XSeekable > xSeek ( xStream, UNO_QUERY );

                bUseNonSeekableAccess = ( xStream.is() && !xSeek.is() );
            }

            if ( !bUseNonSeekableAccess )
            {
                xStream = pStream->getRawData();

                if ( !xStream.is() )
                {
                    VOS_ENSURE( 0, "ZipPackageStream didn't have a stream associated with it, skipping!" );
                    bWritingFailed = sal_True;
                    continue;
                }

                Reference < XSeekable > xSeek ( xStream, UNO_QUERY );
                try
                {
                    if ( xSeek.is() )
                    {
                        // If the stream is a raw one, then we should be positioned
                        // at the beginning of the actual data
                        if ( !bToBeCompressed || bRawStream )
                        {
                            // The raw stream can neither be encrypted nor connected
                            OSL_ENSURE( !bRawStream || !bToBeCompressed && !bToBeEncrypted, "The stream is already encrypted!\n" );
                            xSeek->seek ( bRawStream ? pStream->GetMagicalHackPos() : 0 );
                            ImplSetStoredData ( *pTempEntry, xStream );

                            // TODO/LATER: Get rid of hacks related to switching of Flag Method and Size properties!
                        }
                        else if ( bToBeEncrypted )
                        {
                            // this is the correct original size
                            pTempEntry->nSize = static_cast < sal_Int32 > ( xSeek->getLength() );
                            nOwnStreamOrigSize = pTempEntry->nSize;
                        }

                        xSeek->seek ( 0 );
                    }
                    else
                    {
                        // Okay, we don't have an xSeekable stream. This is possibly bad.
                        // check if it's one of our own streams, if it is then we know that
                        // each time we ask for it we'll get a new stream that will be
                        // at position zero...otherwise, assert and skip this stream...
                        if ( pStream->IsPackageMember() )
                        {
                            // if the password has been changed than the stream should not be package member any more
                            if ( pStream->IsEncrypted() && pStream->IsToBeEncrypted() )
                            {
                                // Should be handled close to the raw stream handling
                                bTransportOwnEncrStreamAsRaw = sal_True;
                                pTempEntry->nMethod = STORED;

                                // TODO/LATER: get rid of this situation
                                // this size should be different from the one that will be stored in manifest.xml
                                // it is used in storing algorithms and after storing the correct size will be set
                                pTempEntry->nSize = pTempEntry->nCompressedSize;
                            }
                        }
                        else
                        {
                            VOS_ENSURE( 0, "The package component requires that every stream either be FROM a package or it must support XSeekable!" );
                            continue;
                        }
                    }
                }
                catch ( Exception& )
                {
                    VOS_ENSURE( 0, "The stream provided to the package component has problems!" );
                    bWritingFailed = sal_True;
                    continue;
                }

                if ( bToBeEncrypted || bRawStream || bTransportOwnEncrStreamAsRaw )
                {
                    if ( bToBeEncrypted && !bTransportOwnEncrStreamAsRaw )
                    {
                        Sequence < sal_uInt8 > aSalt ( 16 ), aVector ( 8 );
                        Sequence < sal_Int8 > aKey ( 16 );
                        rtl_random_getBytes ( rRandomPool, aSalt.getArray(), 16 );
                        rtl_random_getBytes ( rRandomPool, aVector.getArray(), 8 );
                        sal_Int32 nIterationCount = 1024;

                        if ( pStream->HasOwnKey() )
                            rtl_digest_PBKDF2 ( reinterpret_cast < sal_uInt8 * > (aKey.getArray()), 16,
                                                reinterpret_cast < const sal_uInt8 * > (pStream->getKey().getConstArray()), pStream->getKey().getLength(),
                                                reinterpret_cast < const sal_uInt8 * > ( aSalt.getConstArray() ), 16,
                                                nIterationCount );
                        else
                            rtl_digest_PBKDF2 ( reinterpret_cast < sal_uInt8 * > (aKey.getArray()), 16,
                                                reinterpret_cast < const sal_uInt8 * > (rEncryptionKey.getConstArray()), rEncryptionKey.getLength(),
                                                reinterpret_cast < const sal_uInt8 * > ( aSalt.getConstArray() ), 16,
                                                nIterationCount );
                        pStream->setInitialisationVector ( aVector );
                        pStream->setSalt ( aSalt );
                        pStream->setIterationCount ( nIterationCount );
                        pStream->setKey ( aKey );
                    }

                    aPropSet.realloc(7); // 7th property is digest, which is inserted later if we didn't have
                                         // a magic header
                    pValue = aPropSet.getArray();
                    pValue[2].Name = sInitialisationVectorProperty;
                    pValue[2].Value <<= pStream->getInitialisationVector();
                    pValue[3].Name = sSaltProperty;
                    pValue[3].Value <<= pStream->getSalt();
                    pValue[4].Name = sIterationCountProperty;
                    pValue[4].Value <<= pStream->getIterationCount ();

                    // Need to store the uncompressed size in the manifest
                    OSL_ENSURE( nOwnStreamOrigSize >= 0, "The stream size was not correctly initialized!\n" );
                    pValue[5].Name = sSizeProperty;
                    pValue[5].Value <<= nOwnStreamOrigSize;

                    if ( bRawStream || bTransportOwnEncrStreamAsRaw )
                    {
                        pValue[6].Name = sDigestProperty;
                        pValue[6].Value <<= pStream->getDigest();
                    }
                }
            }

            // If the entry is already stored in the zip file in the format we
            // want for this write...copy it raw
            if ( !bUseNonSeekableAccess &&
                ( bRawStream || bTransportOwnEncrStreamAsRaw ||
                    ( pStream->IsPackageMember()          && !bToBeEncrypted &&
                        ( pStream->aEntry.nMethod == DEFLATED &&  bToBeCompressed ) ||
                        ( pStream->aEntry.nMethod == STORED   && !bToBeCompressed ) ) ) )
            {
                // If it's a PackageMember, then it's an unbuffered stream and we need
                // to get a new version of it as we can't seek backwards.
                if ( pStream->IsPackageMember() )
                {
                    xStream = pStream->getRawData();
                    if ( !xStream.is() )
                    {
                        // Make sure that we actually _got_ a new one !
                        VOS_ENSURE( 0, "ZipPackageStream didn't have a stream associated with it, skipping!" );
                        continue;
                    }
                }

                try
                {
                    if ( bRawStream )
                        xStream->skipBytes( pStream->GetMagicalHackPos() );

                    rZipOut.putNextEntry ( *(pTempEntry.get()), pStream->getEncryptionData(), sal_False );
                    Sequence < sal_Int8 > aSeq ( n_ConstBufferSize );
                    sal_Int32 nLength;

                    do
                    {
                        nLength = xStream->readBytes( aSeq, n_ConstBufferSize );
                        rZipOut.rawWrite(aSeq, 0, nLength);
                    }
                    while ( nLength == n_ConstBufferSize );

                    rZipOut.rawCloseEntry();
                }
                catch ( ZipException& )
                {
                    VOS_ENSURE( 0, "Error writing ZipOutputStream" );
                    bWritingFailed = sal_True;
                }
                catch ( IOException& )
                {
                    VOS_ENSURE( 0, "Error writing ZipOutputStream" );
                    bWritingFailed = sal_True;
                }
            }
            else
            {
                // This stream is defenitly not a raw stream

                // If nonseekable access is used the stream should be at the beginning and
                // is useless after the storing. Thus if the storing fails the package should
                // be thrown away ( as actually it is done currently )!
                // To allow to reuse the package after the error, the optimization must be removed!

                // If it's a PackageMember, then our previous reference held a 'raw' stream
                // so we need to re-get it, unencrypted, uncompressed and positioned at the
                // beginning of the stream
                if ( pStream->IsPackageMember() )
                {
                    xStream = pStream->getInputStream();
                    if ( !xStream.is() )
                    {
                        // Make sure that we actually _got_ a new one !
                        VOS_ENSURE( 0, "ZipPackageStream didn't have a stream associated with it, skipping!" );
                        continue;
                    }
                }

                if ( bToBeCompressed )
                {
                    pTempEntry->nMethod = DEFLATED;
                    pTempEntry->nCrc = pTempEntry->nCompressedSize = pTempEntry->nSize = -1;
                }

                try
                {
                    rZipOut.putNextEntry ( *(pTempEntry.get()), pStream->getEncryptionData(), bToBeEncrypted);
                    sal_Int32 nLength;
                    Sequence < sal_Int8 > aSeq (n_ConstBufferSize);
                    do
                    {
                        nLength = xStream->readBytes(aSeq, n_ConstBufferSize);
                        rZipOut.write(aSeq, 0, nLength);
                    }
                    while ( nLength == n_ConstBufferSize );

                    rZipOut.closeEntry();
                }
                catch ( ZipException& )
                {
                    VOS_ENSURE( 0, "Error writing ZipOutputStream" );
                    bWritingFailed = sal_True;
                }
                catch ( IOException& )
                {
                    VOS_ENSURE( 0, "Error writing ZipOutputStream" );
                    bWritingFailed = sal_True;
                }

                if ( bToBeEncrypted )
                {
                    pValue[6].Name = sDigestProperty;
                    pValue[6].Value <<= pStream->getDigest();
                    pStream->SetIsEncrypted ( sal_True );
                }
            }

            if( !bWritingFailed )
            {
                if ( !pStream->IsPackageMember() )
                {
                    pStream->CloseOwnStreamIfAny();
                    pStream->SetPackageMember ( sal_True );
                }

                if ( bRawStream )
                {
                    // the raw stream was integrated and now behaves
                    // as usual encrypted stream
                    pStream->SetToBeEncrypted( sal_True );
                }

                // Remove hacky bit from entry flags
                if ( pTempEntry->nFlag & ( 1 << 4 ) )
                {
                    pTempEntry->nFlag &= ~( 1 << 4 );
                    pTempEntry->nMethod = STORED;
                }

                // Then copy it back afterwards...
                ZipPackageFolder::copyZipEntry ( pStream->aEntry, *pTempEntry );
                // all the dangerous stuff has passed, so we can release pTempEntry
                pTempEntry.release();

                // TODO/LATER: get rid of this hack ( the encrypted stream size property is changed during saving )
                if ( pStream->IsEncrypted() )
                    pStream->setSize( nOwnStreamOrigSize );

                pStream->aEntry.sName = rShortName;
                pStream->aEntry.nOffset *= -1;
            }
        }
        rManList.push_back (aPropSet);
    }

    if( bWritingFailed )
        throw RuntimeException();
}

void ZipPackageFolder::releaseUpwardRef( void )
{
    // Now it is possible that a package folder is disconnected from the package before removing of the folder.
    // Such a scenario is used in storage implementation. When a new version of a folder is provided the old
    // one is retrieved, removed from the package but preserved for the error handling.
    // In this scenario the referencing to the parent is not really useful, since it requires disposing.

    // Actually there is no need in having a reference to the parent, it even make things more complicated and
    // requires disposing mechanics. Using of a simple pointer seems to be easier solution and also a safe enough.

    clearParent();

#if 0
    for ( ContentHash::const_iterator aCI = maContents.begin();
          aCI!=maContents.end();
          aCI++)
    {
        ContentInfo &rInfo = * (*aCI).second;
        if ( rInfo.bFolder )// && ! rInfo.pFolder->HasReleased () )
            rInfo.pFolder->releaseUpwardRef();
        else //if ( !rInfo.bFolder && !rInfo.pStream->HasReleased() )
            rInfo.pStream->clearParent();
    }
    clearParent();

    VOS_ENSURE ( m_refCount == 1, "Ref-count is not 1!" );
#endif
}

sal_Int64 SAL_CALL ZipPackageFolder::getSomething( const Sequence< sal_Int8 >& aIdentifier )
    throw(RuntimeException)
{
    sal_Int64 nMe = 0;
    if ( aIdentifier.getLength() == 16 &&
         0 == rtl_compareMemory(static_getImplementationId().getConstArray(),  aIdentifier.getConstArray(), 16 ) )
        nMe = reinterpret_cast < sal_Int64 > ( this );
    return nMe;
}
void SAL_CALL ZipPackageFolder::setPropertyValue( const OUString& aPropertyName, const Any& aValue )
        throw(UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    if (aPropertyName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("MediaType")))
    {
        if ( !m_bPackageFormat )
            throw PropertyVetoException();

        aValue >>= sMediaType;
    }
    else if (aPropertyName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Size") ) )
        aValue >>= aEntry.nSize;
    else
        throw UnknownPropertyException();
}
Any SAL_CALL ZipPackageFolder::getPropertyValue( const OUString& PropertyName )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "MediaType" ) ) )
        return makeAny ( sMediaType );
    else if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "Size" ) ) )
        return makeAny ( aEntry.nSize );
    else
        throw UnknownPropertyException();
}

void ZipPackageFolder::doInsertByName ( ZipPackageEntry *pEntry, sal_Bool bSetParent )
        throw(IllegalArgumentException, ElementExistException, WrappedTargetException, RuntimeException)
{
    if ( pEntry->IsFolder() )
        maContents[pEntry->aEntry.sName] = new ContentInfo ( static_cast < ZipPackageFolder *> ( pEntry ) );
    else
        maContents[pEntry->aEntry.sName] = new ContentInfo ( static_cast < ZipPackageStream *> ( pEntry ) );

    if ( bSetParent )
        pEntry->setParent ( *this );
}
OUString ZipPackageFolder::getImplementationName()
    throw (RuntimeException)
{
    return OUString ( RTL_CONSTASCII_USTRINGPARAM ( "ZipPackageFolder" ) );
}

Sequence< OUString > ZipPackageFolder::getSupportedServiceNames()
    throw (RuntimeException)
{
    Sequence< OUString > aNames(1);
    aNames[0] = OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.packages.PackageFolder" ) );
    return aNames;
}
sal_Bool SAL_CALL ZipPackageFolder::supportsService( OUString const & rServiceName )
    throw (RuntimeException)
{
    return rServiceName == getSupportedServiceNames()[0];
}
