/*************************************************************************
 *
 *  $RCSfile: ZipPackage.cxx,v $
 *
 *  $Revision: 1.64 $
 *
 *  last change: $Author: mtg $ $Date: 2001-09-26 12:41:36 $
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
 *  Contributor(s): Martin Gallwey (gallwey@sun.com)
 *
 *
 ************************************************************************/
#ifndef _ZIP_PACKAGE_HXX
#include <ZipPackage.hxx>
#endif
#ifndef _ZIP_PACKAGE_SINK_HXX
#include <ZipPackageSink.hxx>
#endif
#ifndef _ZIP_ENUMERATION_HXX
#include <ZipEnumeration.hxx>
#endif
#ifndef _ZIP_PACKAGE_STREAM_HXX
#include <ZipPackageStream.hxx>
#endif
#ifndef _ZIP_PACKAGE_FOLDER_HXX
#include <ZipPackageFolder.hxx>
#endif
#ifndef _ZIP_OUTPUT_STREAM_HXX
#include <ZipOutputStream.hxx>
#endif
#ifndef _ZIP_PACKAGE_BUFFER_HXX
#include <ZipPackageBuffer.hxx>
#endif
#ifndef _ZIP_FILE_HXX
#include <ZipFile.hxx>
#endif
#ifndef _PACKAGE_CONSTANTS_HXX_
#include <PackageConstants.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPCONSTANTS_HPP_
#include <com/sun/star/packages/zip/ZipConstants.hpp>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_MANIFEST_XMANIFESTREADER_HPP_
#include <com/sun/star/packages/manifest/XManifestReader.hpp>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_MANIFEST_XMANIFESTWRITER_HPP_
#include <com/sun/star/packages/manifest/XManifestWriter.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVEAUGMENTEDIOEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveAugmentedIOException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_INTERACTIVEWRONGMEDIUMEXCEPTION_HPP_
#include <com/sun/star/ucb/InteractiveWrongMediumException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_IOERRORCODE_HPP
#include <com/sun/star/ucb/IOErrorCode.hpp>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _INTERACTION_REQUEST_HXX_
#include <InteractionRequest.hxx>
#endif
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif
#ifndef _COM_SUN_STAR_UCB_TRANSFERINFO_HPP_
#include <com/sun/star/ucb/TransferInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_NAMECLASH_HPP_
#include <com/sun/star/ucb/NameClash.hpp>
#endif
#ifndef _CONTENT_INFO_HXX_
#include <ContentInfo.hxx>
#endif
#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif
#include <memory>

using namespace rtl;
using namespace ucb;
using namespace std;
using namespace osl;
using namespace cppu;
using namespace com::sun::star::io;
using namespace com::sun::star::uno;
using namespace com::sun::star::ucb;
using namespace com::sun::star::util;
using namespace com::sun::star::lang;
using namespace com::sun::star::task;
using namespace com::sun::star::beans;
using namespace com::sun::star::packages;
using namespace com::sun::star::container;
using namespace com::sun::star::packages::zip;
using namespace com::sun::star::packages::manifest;
using namespace com::sun::star::packages::zip::ZipConstants;

#define LOGFILE_AUTHOR "mg115289"

struct SuffixGenerator
{
    sal_Char mpSuffix[30], *mpSuffixMid, *mpSuffixEnd;
    sal_uInt16 mnDigits;
    SuffixGenerator(): mnDigits (2), mpSuffixMid ( mpSuffix), mpSuffixEnd ( mpSuffix + 2)
    {
        memset ( mpSuffix, 0, 30 );
        mpSuffixMid[0] = 'a';
        mpSuffixMid[1] = 'a'; // first generated suffix will be .ab
    }
    void generateFileName( OUString &rFileName, const OUString &rPrefix );
    void generateFileName( OUString &rFileName, const OUString &rPrefix, sal_uInt16 nDiskNum);
};

void SuffixGenerator::generateFileName( OUString &rFileName, const OUString &rPrefix )
{
    // same file prefix, different extension
    OUStringBuffer aStringBuf;
    sal_Char *p;
    for ( p = mpSuffixEnd-1; *p == 'z'; p--)
        *p = 'a';
    ++(*p);
    if (*p == 'z' && p == mpSuffixMid )
    {
        ++mnDigits;
        ++mpSuffixMid;
        *mpSuffixEnd++ = 'a';
        *mpSuffixEnd++ = 'a';
    }
    aStringBuf.append ( rPrefix );
    aStringBuf.appendAscii ( mpSuffix );
    rFileName = aStringBuf.makeStringAndClear();
}
void SuffixGenerator::generateFileName( OUString &rFileName, const OUString &rPrefix, sal_uInt16 nDiskNum)
{
    // same file prefix, different extension
    OUStringBuffer aStringBuf;
    aStringBuf.append ( rPrefix );
    aStringBuf.appendAscii ( nDiskNum < 10 ? "00" : nDiskNum < 100 ? "0" : "" );
    aStringBuf.append ( static_cast < sal_Int32 > ( nDiskNum-1) );

    rFileName = aStringBuf.makeStringAndClear();
}

#ifdef MTG_DEBUG
char * ImplGetChars( const OUString & rString )
{
    // Memory leak ? Bah!
    sal_Int32 nLength = rString.getLength();
    const sal_Unicode *pString = rString.getStr();
    char * pChar = new char [nLength+1];
    for ( sal_Int16 i = 0; i < nLength; i++ )
    {
        pChar[i] = static_cast < char > (pString[i]);
    }
    pChar[nLength] = '\0';
    return pChar;
}
#endif

ZipPackage::ZipPackage (const Reference < XMultiServiceFactory > &xNewFactory)
: pZipFile(NULL)
, pRootFolder(NULL)
, xContentStream (NULL)
, xContentSeek (NULL)
, xRootFolder (NULL)
, xFactory(xNewFactory)
, bHasEncryptedEntries ( sal_False )
, nSegmentSize ( 0 )
{
    xRootFolder = pRootFolder = new ZipPackageFolder();
}

ZipPackage::~ZipPackage( void )
{
    delete pZipFile;
    // As all folders and streams contain references to their parents,
    // we must remove these references so that they will be deleted when
    // the hash_map of the root folder is cleared, releasing all subfolders
    // and substreams which in turn release theirs, etc. When xRootFolder is
    // released when this destructor completes, the folder tree should be
    // deleted fully (and automagically).

    pRootFolder->releaseUpwardRef();
}

void ZipPackage::getZipFileContents()
{
    auto_ptr < ZipEnumeration > pEnum ( pZipFile->entries() );
    ZipPackageStream *pPkgStream;
    ZipPackageFolder *pPkgFolder, *pCurrent;
    OUString sTemp, sDirName;
    sal_Int32 nOldIndex, nIndex, nStreamIndex;
    FolderHash::iterator aIter;

    while (pEnum->hasMoreElements())
    {
        nIndex = nOldIndex = 0;
        pCurrent = pRootFolder;
        const ZipEntry & rEntry = *pEnum->nextElement();
        const OUString & rName = rEntry.sName;

        nStreamIndex = rName.lastIndexOf ( '/' );
        if ( nStreamIndex != -1 )
        {
            sDirName = rName.copy ( 0, nStreamIndex);
            aIter = aRecent.find ( sDirName );
            if ( aIter != aRecent.end() )
                pCurrent = (*aIter).second;
        }

        if ( pCurrent == pRootFolder )
        {
            while ( (nIndex = rName.indexOf('/', nOldIndex) ) != -1 )
            {
                sTemp = rName.copy ( nOldIndex, nIndex - nOldIndex );
                if (nIndex == nOldIndex)
                    break;
                if ( !pCurrent->hasByName( sTemp ) )
                {
                    pPkgFolder = new ZipPackageFolder();
                    pPkgFolder->setName( sTemp );
                    pPkgFolder->doSetParent( pCurrent, sal_True );
                    pCurrent = pPkgFolder;
                }
                else
                    pCurrent = pCurrent->doGetByName(sTemp).pFolder;
                nOldIndex = nIndex+1;
            }
            if ( nStreamIndex != -1 && sDirName.getLength() )
                aRecent [ sDirName ] = pCurrent;
        }
        if ( rName.getLength() -1 != nStreamIndex )
        {
            nStreamIndex++;
            sTemp = rName.copy( nStreamIndex, rName.getLength() - nStreamIndex);
            pPkgStream = new ZipPackageStream( *this );
            pPkgStream->SetPackageMember( sal_True );
            pPkgStream->setZipEntry( rEntry );
            pPkgStream->setName( sTemp );
            pPkgStream->doSetParent( pCurrent, sal_True );
        }
    }
    const OUString sManifest (RTL_CONSTASCII_USTRINGPARAM( "META-INF/manifest.xml") );
    if (hasByHierarchicalName( sManifest ) )
    {
        Reference < XUnoTunnel > xTunnel;
        Any aAny = getByHierarchicalName( sManifest );
        aAny >>= xTunnel;
        Reference < XActiveDataSink > xSink (xTunnel, UNO_QUERY);
        if (xSink.is())
        {
            OUString sManifestReader ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.packages.manifest.ManifestReader" ) );
            Reference < XManifestReader > xReader (xFactory->createInstance( sManifestReader ), UNO_QUERY );
            if ( xReader.is() )
            {
                const OUString sPropFullPath ( RTL_CONSTASCII_USTRINGPARAM ( "FullPath" ) );
                const OUString sPropMediaType ( RTL_CONSTASCII_USTRINGPARAM ( "MediaType" ) );
                const OUString sPropInitialisationVector ( RTL_CONSTASCII_USTRINGPARAM ( "InitialisationVector" ) );
                const OUString sPropSalt ( RTL_CONSTASCII_USTRINGPARAM ( "Salt" ) );
                const OUString sPropIterationCount ( RTL_CONSTASCII_USTRINGPARAM ( "IterationCount" ) );
                const OUString sPropSize ( RTL_CONSTASCII_USTRINGPARAM ( "Size" ) );
                const OUString sPropDigest ( RTL_CONSTASCII_USTRINGPARAM ( "Digest" ) );

                Sequence < Sequence < PropertyValue > > aManifestSequence = xReader->readManifestSequence ( xSink->getInputStream() );
                sal_Int32 nLength = aManifestSequence.getLength();
                const Sequence < PropertyValue > *pSequence = aManifestSequence.getConstArray();
                ZipPackageStream *pStream = NULL;
                ZipPackageFolder *pFolder = NULL;

                for (sal_Int32 i = 0; i < nLength ; i++, pSequence++)
                {
                    OUString sPath, sMediaType;
                    const PropertyValue *pValue = pSequence->getConstArray();
                    const Any *pSalt = NULL, *pVector = NULL, *pCount = NULL, *pSize = NULL, *pDigest = NULL;
                    for (sal_Int32 j = 0, nNum = pSequence->getLength(); j < nNum; j++ )
                    {
                        if (pValue[j].Name.equals( sPropFullPath ) )
                            pValue[j].Value >>= sPath;
                        else if (pValue[j].Name.equals( sPropMediaType ) )
                            pValue[j].Value >>= sMediaType;
                        else if (pValue[j].Name.equals( sPropSalt ) )
                            pSalt = &(pValue[j].Value);
                        else if (pValue[j].Name.equals( sPropInitialisationVector ) )
                            pVector = &(pValue[j].Value);
                        else if (pValue[j].Name.equals( sPropIterationCount ) )
                            pCount = &(pValue[j].Value);
                        else if (pValue[j].Name.equals( sPropSize ) )
                            pSize = &(pValue[j].Value);
                        else if (pValue[j].Name.equals( sPropDigest ) )
                            pDigest = &(pValue[j].Value);
                    }
                    if (sPath.getLength() && hasByHierarchicalName ( sPath ) )
                    {
                        Any aAny = getByHierarchicalName( sPath );
                        Reference < XUnoTunnel > xTunnel;
                        aAny >>= xTunnel;
                        sal_Int64 nTest=0;
                        if ((nTest = xTunnel->getSomething(ZipPackageFolder::getUnoTunnelImplementationId())) != 0)
                        {
                            pFolder = reinterpret_cast < ZipPackageFolder* > ( nTest );
                            pFolder->SetMediaType ( sMediaType );
                        }
                        else
                        {
                            pStream = reinterpret_cast < ZipPackageStream* > ( xTunnel->getSomething(ZipPackageStream::getUnoTunnelImplementationId()));
                            pStream->SetMediaType ( sMediaType );

                            if (pSalt && pVector && pCount && pSize)
                            {
                                Sequence < sal_uInt8 > aSequence;
                                sal_Int32 nCount, nSize;
                                pStream->SetToBeEncrypted ( sal_True );

                                *pSalt >>= aSequence;
                                pStream->setSalt ( aSequence );

                                *pVector >>= aSequence;
                                pStream->setInitialisationVector ( aSequence );

                                *pCount >>= nCount;
                                pStream->setIterationCount ( nCount );

                                *pSize >>= nSize;
                                pStream->setSize ( nSize );

                                if ( pDigest )
                                {
                                    *pDigest >>= aSequence;
                                    pStream->setDigest ( aSequence );
                                }

                                pStream->SetToBeEncrypted ( sal_True );
                                bHasEncryptedEntries = sal_True;
                            }
                        }
                    }
                }
            }
        }
    }
    const OUString sMeta ( RTL_CONSTASCII_USTRINGPARAM ( "META-INF" ) );
    if (xRootFolder->hasByName( sMeta ) )
        xRootFolder->removeByName( sMeta );
}
// XInitialization
void SAL_CALL ZipPackage::initialize( const Sequence< Any >& aArguments )
        throw(Exception, RuntimeException)
{
    RTL_LOGFILE_TRACE_AUTHOR ( "package", LOGFILE_AUTHOR, "{ ZipPackage::initialize" );
    if (! (aArguments[0] >>= sURL))
        throw com::sun::star::uno::Exception ( OUString( RTL_CONSTASCII_USTRINGPARAM ( "Bad URL." ) ),
            static_cast < ::cppu::OWeakObject * > ( this ) );
    Content aContent (sURL, Reference < XCommandEnvironment >() );
    sal_Bool bBadZipFile = sal_False, bHaveZipFile = sal_True;

#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG: initialise called on %s\n", ImplGetChars ( sURL ) );
#endif

    sal_Bool bSpanned = sal_False;
    try
    {
        Reference < XActiveDataSink > xSink = new ZipPackageSink;
        if (aContent.openStream ( xSink ) )
            xContentStream = xSink->getInputStream();
        if (xContentStream.is())
        {
            xContentSeek = Reference < XSeekable > (xContentStream, UNO_QUERY);
            Sequence < sal_Int8 > aSequence ( 4 );
            xContentStream->readBytes( aSequence, 4 );
            xContentSeek->seek ( 0 );
            const sal_Int8 *pSeq = aSequence.getConstArray();
            if (pSeq[0] == 'P' &&
                pSeq[1] == 'K' &&
                pSeq[2] == 7 &&
                pSeq[3] == 8 )
                bSpanned = sal_True;
       }
        else
            bHaveZipFile = sal_False;
    }
    catch (com::sun::star::uno::Exception&)
    {
        // Exception derived from uno::Exception thrown. This probably
        // means the file doesn't exist...we'll create it at
        // commitChanges time
        bHaveZipFile = sal_False;
    }
    if ( bHaveZipFile )
    {
        try
        {
            if ( bSpanned)
                // after unSpanFile, xContentStream will refer to the tempfile containing
                // the unspanned file
                unSpanFile ();
            pZipFile = new ZipFile ( xContentStream, xFactory, sal_True );
            getZipFileContents();
        }
        catch ( IOException & )
        {
            bBadZipFile = sal_True;
        }
        catch ( ZipException & )
        {
            bBadZipFile = sal_True;
        }
        if ( bBadZipFile )
        {
            // clean up the memory, and tell the UCB about the error
            delete pZipFile; pZipFile = NULL;
            throw com::sun::star::uno::Exception ( OUString( RTL_CONSTASCII_USTRINGPARAM ( "Bad Zip File." ) ),
                static_cast < ::cppu::OWeakObject * > ( this ) );
        }
    }
    RTL_LOGFILE_TRACE_AUTHOR ( "package", LOGFILE_AUTHOR, "} ZipPackage::initialize" );
}

Any SAL_CALL ZipPackage::getByHierarchicalName( const OUString& aName )
        throw(NoSuchElementException, RuntimeException)
{
    OUString sTemp, sDirName;
    sal_Int32 nOldIndex, nIndex, nStreamIndex;
    FolderHash::iterator aIter;

    if ( (nIndex = aName.getLength() ) == 1 && *aName.getStr() == '/' )
        return makeAny ( Reference < XUnoTunnel > (pRootFolder) );
    else
    {
        nStreamIndex = aName.lastIndexOf ( '/' );
        bool bFolder = nStreamIndex == nIndex-1;
        if ( nStreamIndex != -1 )
        {
            sDirName = aName.copy ( 0, nStreamIndex);
            aIter = aRecent.find ( sDirName );
            if ( aIter != aRecent.end() )
            {
                if ( bFolder )
                {
                    sal_Int32 nDirIndex = aName.lastIndexOf ( '/', nStreamIndex );
                    sTemp = aName.copy ( nDirIndex == -1 ? 0 : nDirIndex+1, nStreamIndex-nDirIndex-1 );
                    if ( sTemp == (*aIter).second->getName() )
                        return makeAny ( Reference < XUnoTunnel > ( (*aIter).second ) );
                    else
                        aRecent.erase ( aIter );
                }
                else
                {
                    sTemp = aName.copy ( nStreamIndex + 1 );
                    if ( (*aIter).second->hasByName( sTemp ) )
                        return (*aIter).second->getByName( sTemp );
                    else
                        aRecent.erase( aIter );
                }
            }
        }
        else
        {
            if ( pRootFolder->hasByName ( aName ) )
                return pRootFolder->getByName ( aName );
        }
        nOldIndex = 0;
        ZipPackageFolder * pCurrent = pRootFolder, *pPrevious;
        while ( ( nIndex = aName.indexOf('/', nOldIndex)) != -1)
        {
            sTemp = aName.copy (nOldIndex, nIndex - nOldIndex);
            if ( nIndex == nOldIndex )
                break;
            if ( pCurrent->hasByName( sTemp ) )
            {
                pPrevious = pCurrent;
                pCurrent = pCurrent->doGetByName(sTemp).pFolder;
            }
            else
                throw NoSuchElementException();
            nOldIndex = nIndex+1;
        }
        if ( bFolder )
        {
            if (nStreamIndex != -1 )
                aRecent[sDirName] = pPrevious;
            return makeAny ( Reference < XUnoTunnel > ( pCurrent ) );
        }
        else
        {
            sTemp = aName.copy( nOldIndex, aName.getLength() - nOldIndex);
            if ( pCurrent->hasByName ( sTemp ) )
            {
                if (nStreamIndex != -1 )
                    aRecent[sDirName] = pCurrent;
                return pCurrent->getByName( sTemp );
            }
            else
                throw NoSuchElementException();
        }
    }
}

sal_Bool SAL_CALL ZipPackage::hasByHierarchicalName( const OUString& aName )
        throw(RuntimeException)
{
    OUString sTemp, sDirName;
    sal_Int32 nOldIndex, nIndex, nStreamIndex;
    FolderHash::iterator aIter;

    if ( (nIndex = aName.getLength() ) == 1 && *aName.getStr() == '/' )
        return sal_True;
    else
    {
        nStreamIndex = aName.lastIndexOf ( '/' );
        bool bFolder = nStreamIndex == nIndex-1;
        if ( nStreamIndex != -1 )
        {
            sDirName = aName.copy ( 0, nStreamIndex);
            aIter = aRecent.find ( sDirName );
            if ( aIter != aRecent.end() )
            {
                if ( bFolder )
                {
                    sal_Int32 nDirIndex = aName.lastIndexOf ( '/', nStreamIndex );
                    sTemp = aName.copy ( nDirIndex == -1 ? 0 : nDirIndex+1, nStreamIndex-nDirIndex-1 );
                    if ( sTemp == (*aIter).second->getName() )
                        return sal_True;
                    else
                        aRecent.erase ( aIter );
                }
                else
                {
                    sTemp = aName.copy ( nStreamIndex + 1 );
                    if ( (*aIter).second->hasByName( sTemp ) )
                        return sal_True;
                    else
                        aRecent.erase( aIter );
                }
            }
        }
        else
        {
            if ( pRootFolder->hasByName ( aName ) )
                return sal_True;
        }
        ZipPackageFolder * pCurrent = pRootFolder, *pPrevious;
        nOldIndex = 0;
        while ( ( nIndex = aName.indexOf('/', nOldIndex)) != -1)
        {
            sTemp = aName.copy (nOldIndex, nIndex - nOldIndex);
            if ( nIndex == nOldIndex )
                break;
            if ( pCurrent->hasByName( sTemp ) )
            {
                pPrevious = pCurrent;
                pCurrent = pCurrent->doGetByName( sTemp ).pFolder;
            }
            else
                return sal_False;
            nOldIndex = nIndex+1;
        }
        if ( bFolder )
        {
            aRecent[sDirName] = pPrevious;
            return sal_True;
        }
        else
        {
            sTemp = aName.copy( nOldIndex, aName.getLength() - nOldIndex);

            if ( pCurrent->hasByName( sTemp ) )
            {
                aRecent[sDirName] = pCurrent;
                return sal_True;
            }
        }
        return sal_False;
    }
}

// XSingleServiceFactory
Reference< XInterface > SAL_CALL ZipPackage::createInstance(  )
        throw(Exception, RuntimeException)
{
    Reference < XInterface > xRef = *(new ZipPackageStream ( *this ));
    return xRef;
}
Reference< XInterface > SAL_CALL ZipPackage::createInstanceWithArguments( const Sequence< Any >& aArguments )
        throw(Exception, RuntimeException)
{
    sal_Bool bArg;
    Reference < XInterface > xRef;
    aArguments[0] >>= bArg;
    if (bArg)
        xRef = *new ZipPackageFolder ( );
    else
        xRef = *new ZipPackageStream ( *this );

    return xRef;
}

void ZipPackage::writeTempFile()
{
    // First, create a temporary file to write to:
    const OUString sServiceName ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.io.TempFile" ) );
    Reference < XOutputStream > xTempOut = Reference < XOutputStream > ( xFactory->createInstance ( sServiceName ), UNO_QUERY );
#ifdef MTG_DEBUG
    fprintf(stderr, "We have a %s tempfile!\n", xTempOut.is() ? "good" : "bad" );
#endif

    // Hand it to the ZipOutputStream:
    ZipOutputStream aZipOut ( xTempOut, nSegmentSize != 0);
    aZipOut.setMethod(DEFLATED);
    aZipOut.setLevel(DEFAULT_COMPRESSION);

    // Remove the old META-INF directory (if there is one) as the
    // manifest will be re-generated and the META-INF directory implicitly
    // created
    const OUString sMeta ( RTL_CONSTASCII_USTRINGPARAM ( "META-INF" ) );
    if (xRootFolder->hasByName( sMeta ) )
        xRootFolder->removeByName( sMeta );

    // Create a vector to store data for the manifest.xml file
    vector < Sequence < PropertyValue > > aManList;

    // Make a reference to the manifest output stream so it persists
    // until the call to ZipOutputStream->finish()
    Reference < XOutputStream > xManOutStream;
    const OUString sMediaType ( RTL_CONSTASCII_USTRINGPARAM ( "MediaType" ) );
    const OUString sFullPath ( RTL_CONSTASCII_USTRINGPARAM ( "FullPath" ) );

    Sequence < PropertyValue > aPropSeq ( 2 );
    aPropSeq [0].Name = sMediaType;
    aPropSeq [0].Value <<= pRootFolder->GetMediaType( );
    aPropSeq [1].Name = sFullPath;
    aPropSeq [1].Value <<= OUString ( RTL_CONSTASCII_USTRINGPARAM ( "/" ) );

    aManList.push_back( aPropSeq );

    // Get a random number generator and seed it with current timestamp
    // This will be used to generate random salt and initialisation vectors
    // for encrypted streams
    TimeValue aTime;
    osl_getSystemTime( &aTime );
    rtlRandomPool aRandomPool = rtl_random_createPool ();
    rtl_random_addBytes ( aRandomPool, &aTime, 8 );

    // call saveContents (it will recursively save sub-directories
    pRootFolder->saveContents( OUString(), aManList, aZipOut, aEncryptionKey, aRandomPool );

    // Clean up random pool memory
    rtl_random_destroyPool ( aRandomPool );

    // Write the manifest
    OUString sManifestWriter( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.packages.manifest.ManifestWriter" ) );
    Reference < XManifestWriter > xWriter ( xFactory->createInstance( sManifestWriter ), UNO_QUERY );
    if ( xWriter.is() )
    {
        ZipEntry * pEntry = new ZipEntry;
        ZipPackageBuffer *pBuffer = new ZipPackageBuffer( n_ConstBufferSize );
        xManOutStream = Reference < XOutputStream > (*pBuffer, UNO_QUERY);

        pEntry->sName = OUString( RTL_CONSTASCII_USTRINGPARAM ( "META-INF/manifest.xml") );
        pEntry->nMethod = DEFLATED;
        pEntry->nCrc = pEntry->nSize = pEntry->nCompressedSize = -1;
        pEntry->nTime = ZipOutputStream::getCurrentDosTime();

        // Convert vector into a Sequence
        Sequence < Sequence < PropertyValue > > aManifestSequence ( aManList.size() );
        Sequence < PropertyValue > * pSequence = aManifestSequence.getArray();
        for (vector < Sequence < PropertyValue > >::const_iterator aIter = aManList.begin(), aEnd = aManList.end();
             aIter != aEnd;
             aIter++, pSequence++)
            *pSequence= (*aIter);
        xWriter->writeManifestSequence ( xManOutStream,  aManifestSequence );

        sal_Int32 nBufferLength = static_cast < sal_Int32 > ( pBuffer->getPosition() );
        pBuffer->realloc( nBufferLength );

        try
        {
            // the manifest.xml is never encrypted - so pass an empty reference
            vos::ORef < EncryptionData > xEmpty;
            aZipOut.putNextEntry( *pEntry, xEmpty );
            aZipOut.write( pBuffer->getSequence(), 0, nBufferLength );
            aZipOut.closeEntry();
        }
        catch (::com::sun::star::io::IOException & )
        {
            VOS_ENSURE( 0, "Error adding META-INF/manifest.xml to the ZipOutputStream" );
        }
    }
    try
    {
        aZipOut.finish();
    }
    catch (::com::sun::star::io::IOException & )
    {
        VOS_ENSURE( 0, "Error writing ZIP file to disk" );
    }

    // Update our References to point to the new temp file
    xContentStream = Reference < XInputStream > ( xTempOut, UNO_QUERY );
    xContentSeek = Reference < XSeekable > ( xTempOut, UNO_QUERY );

    // seek back to the beginning of the temp file so we can read segments from it
    xContentSeek->seek ( 0 );
    pZipFile->setInputStream ( xContentStream );
}

// XChangesBatch
void SAL_CALL ZipPackage::commitChanges(  )
        throw(WrappedTargetException, RuntimeException)
{
    RTL_LOGFILE_TRACE_AUTHOR ( "package", LOGFILE_AUTHOR, "{ ZipPackage::commitChanges" );
    // First we write the entire package to a temporary file. After writeTempFile,
    // xContentSeek and xContentStream will reference the new temporary file.
    writeTempFile();

    if (!nSegmentSize )
    {
        Reference < XPropertySet > xPropSet ( xContentStream, UNO_QUERY );
        Content aContent ( sURL, Reference < XCommandEnvironment > () );
        if ( xPropSet.is() )
        {
            OUString sTempURL;
            Any aAny = xPropSet->getPropertyValue ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Uri" ) ) );
            aAny >>= sTempURL;
            TransferInfo aInfo;
            aInfo.NameClash = NameClash::OVERWRITE;
            aInfo.MoveData = sal_False;
            aInfo.SourceURL = sTempURL;
            aInfo.NewTitle = sURL.copy ( 1 + sURL.lastIndexOf ( static_cast < sal_Unicode > ( '/' ) ) );
            aAny <<= aInfo;
            try
            {
                aContent.executeCommand ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "transfer" ) ), aAny );
            }
            catch (::com::sun::star::uno::Exception& r)
            {
                throw WrappedTargetException( OUString( RTL_CONSTASCII_USTRINGPARAM ( "Unable to write Zip File to disk!" ) ),
                        static_cast < OWeakObject * > ( this ), makeAny( r ) );
            }
        }
        else
        {
            // not quite sure how it could happen that xContentStream WOULDN'T support
            // XPropertySet, but just in case... :)
            try
            {
                aContent.writeStream ( xContentStream, sal_True );
            }
            catch (::com::sun::star::uno::Exception& r)
            {
                throw WrappedTargetException( OUString( RTL_CONSTASCII_USTRINGPARAM ( "Unable to write Zip File to disk!" ) ),
                        static_cast < OWeakObject * > ( this ), makeAny( r ) );
            }
        }
    }
    else
    {
        // We want to span...first, make sure we have an interaction handler...
        getInteractionHandler();

        sal_Int16 nDiskNum = 0;
        VolumeInfo aInfo ( osl_VolumeInfo_Mask_FreeSpace | osl_VolumeInfo_Mask_DeviceHandle | osl_VolumeInfo_Mask_Attributes );
        FileBase::RC aRC = Directory::getVolumeInfo ( sURL, aInfo );
#ifdef MTG_DEBUG
        fprintf(stderr, "MTG: url is %s getVolumeInfo returned %d\n", ImplGetChars ( sURL), aRC);
        fprintf(stderr, "MTG: isRemovable returned %d\n", aInfo.getRemoveableFlag() );
        fprintf(stderr, "MTG: isValid osl_VolumeInfo_Mask_FreeSpace returned %d\n", aInfo.isValid(osl_VolumeInfo_Mask_FreeSpace) );
        fprintf(stderr, "MTG: isValid osl_VolumeInfo_Mask_DeviceHandle returned %d\n", aInfo.isValid(osl_VolumeInfo_Mask_DeviceHandle) );
        fprintf(stderr, "MTG: isValid osl_VolumeInfo_Mask_Attributes returned %d\n", aInfo.isValid(osl_VolumeInfo_Mask_Attributes) );
        fprintf(stderr, "MTG: getFreeSpace returned %ld\n", static_cast < sal_Int32 > ( aInfo.getFreeSpace() ) );
        fprintf(stderr, "MTG: getUsedSpace returned %ld\n", static_cast < sal_Int32 > ( aInfo.getUsedSpace() ) );
        fprintf(stderr, "MTG: getTotalSpace returned %ld\n", static_cast < sal_Int32 > ( aInfo.getTotalSpace() ) );
#endif
        VolumeDevice aDevice = aInfo.getDeviceHandle();
        OUString sFileName, sMountPath = aDevice.getMountPath();
#ifdef MTG_DEBUG
        fprintf(stderr, "MTG: mount path is %s %d\n", ImplGetChars ( sMountPath ), aRC);
#endif
        sal_Int32 nLastSlash = sURL.lastIndexOf ( '/' );
        OUString sFilePrefix = sURL.copy ( 1 + nLastSlash,  sURL.lastIndexOf ( '.' ) - nLastSlash );

#ifdef MTG_DEBUG
        fprintf(stderr, "MTG: url is %s first getVolumeInfo on %s returned %d\n", ImplGetChars ( sURL), ImplGetChars ( sMountPath), aRC);
#endif
        sal_Bool bIsRemovable = aInfo.getRemoveableFlag();
#ifdef MTG_DEBUG
        fprintf(stderr, "MTG: Removable flag is %d\n", bIsRemovable);
#endif
        SegmentEnum eRet = e_Finished;
        SuffixGenerator aGenerator;
        do
        {
            nDiskNum++;
            sal_Int64 nCurrentPos = xContentSeek->getPosition();
            if ( nDiskNum == 1 )
                sFileName = sURL.copy ( 1 + nLastSlash );
            else
                aGenerator.generateFileName ( sFileName, sFilePrefix, nDiskNum );
            if ( bIsRemovable )
            {
                if ( nDiskNum > 1 && RequestDisk( sMountPath, nDiskNum ) < 0 )
                    return;

                do
                {
                    eRet = writeSegment ( sFileName, sMountPath, xContentStream, nDiskNum );
                    if (eRet == e_Aborted)
                        return;
                    else if ( eRet == e_Retry )
                        xContentSeek->seek ( nCurrentPos );
                }
                while ( eRet == e_Retry );
            }
            else
            {
                OUString sFullPath = sURL.copy ( 0, nLastSlash + 1 ) + sFileName;
                if ( xContentSeek->getLength() > static_cast < sal_Int64 > ( aInfo.getFreeSpace() ) )
                {
                    // no room on the hard drive, display a message and return
                    HandleError (  osl_File_E_NOSPC, EC_YES, sFullPath );
                    return;
                }
                else if ( nSegmentSize < 0 )
                {
                    try
                    {
                        Content aContent (sURL, Reference < XCommandEnvironment >() );
                        aContent.writeStream ( xContentStream, sal_True );
                    }
                    catch (::com::sun::star::uno::Exception& r)
                    {
                        throw WrappedTargetException( OUString( RTL_CONSTASCII_USTRINGPARAM ( "Unable to write Zip File to disk!" ) ),
                                static_cast < OWeakObject * > ( this ), makeAny( r ) );
                    }
                }
                else
                {
                    do
                    {
                        eRet = writeSegment ( sFullPath, xContentStream );
                        if (eRet == e_Aborted)
                            return;
                        else if ( eRet == e_Retry )
                            xContentSeek->seek ( nCurrentPos );
                    }
                    while ( eRet == e_Retry );
                }
            }
        }
        while ( eRet != e_Finished );
    }
#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG: ZipPackage Commit finished\n");
#endif
    RTL_LOGFILE_TRACE_AUTHOR ( "package", LOGFILE_AUTHOR, "} ZipPackage::commitChanges" );
}

sal_Int32 ZipPackage::RequestDisk ( OUString &rMountPath, sal_Int16 nDiskNum)
{
    VolumeInfo aInfo ( osl_VolumeInfo_Mask_FreeSpace | osl_VolumeInfo_Mask_DeviceHandle | osl_VolumeInfo_Mask_Attributes );
    VolumeDevice aDevice;
    FileBase::RC aRC;

    do
    {
        aRC = Directory::getVolumeInfo ( rMountPath, aInfo );
#ifdef MTG_DEBUG
        fprintf ( stderr, "MTG: Requesting disk for %s, result is %d\n", ImplGetChars ( rMountPath ), aRC );
#endif
        if ( aRC == FileBase::E_None )
            aDevice = aInfo.getDeviceHandle();
        else
        {
#ifdef MTG_DEBUG
        fprintf ( stderr, "MTG: Requesting disk calling HandleError with osl_File_E_INVAL\n" );
#endif
            if ( ! HandleError ( (oslFileError) aRC, EC_RETRY|EC_ABORT, rMountPath) )
                return -1;
        }
    }
    while ( aRC != FileBase::E_None );
#ifdef UNX
    do
    {
        aRC = aDevice.unmount();
#ifdef MTG_DEBUG
        fprintf ( stderr, "MTG: unmount returned %d\n", aRC );
#endif
        if ( aRC != FileBase::E_None )
        {
            if ( ! HandleError ( (oslFileError) aRC, EC_RETRY|EC_ABORT, rMountPath) )
                return -1;
        }
    }
    while ( aRC != FileBase::E_None );
#endif

    Any aExceptionAny, aMediumException;
    InteractiveWrongMediumException aException;
    aMediumException <<= static_cast < sal_Int16 > (nDiskNum-1);
    aException.Medium = aMediumException;
    aExceptionAny <<= aException;
    if ( !HandleError ( aExceptionAny, EC_YES|EC_ABORT ) )
        return -1;
#ifdef UNX
    do
    {
        aRC = aDevice.automount();

#ifdef MTG_DEBUG
        fprintf ( stderr, "MTG: automount returned %d\n", aRC );
#endif
        if ( aRC != FileBase::E_None )
        {
            if ( ! HandleError ( (oslFileError) aRC, EC_RETRY|EC_ABORT, rMountPath) )
                return -1;
        }
    }
    while ( aRC != FileBase::E_None );
    OUString aNewMountPath ( aDevice.getMountPath() );

    if (aNewMountPath != rMountPath)
        rMountPath = aNewMountPath;
#ifdef MTG_DEBUG
        fprintf ( stderr, "MTG: returning, new mountpath is %s\n", ImplGetChars ( rMountPath ) );
#endif
#endif
    return FileBase::E_None;
}
SegmentEnum ZipPackage::writeSegment ( const OUString &rFileName, OUString &rMountPath, Reference < XInputStream > &xInBuffer, const sal_Int16 nDiskNum )
{
    File *pFile = NULL;
    FileBase::RC aRC;
    sal_Bool bDynamicSpan = nSegmentSize < 0;
    Sequence < sal_Int8 > aBuffer;

    sal_Int32 nRead = n_ConstBufferSize;
    sal_uInt64 nLeft, nWritten;
    VolumeInfo aInfo ( osl_VolumeInfo_Mask_FreeSpace | osl_VolumeInfo_Mask_DeviceHandle | osl_VolumeInfo_Mask_Attributes );

#ifdef MTG_DEBUG
    fprintf (stderr, "MTG: In writeSegment, disk num is %d, file is %s, dir is %s\n",
                     nDiskNum, ImplGetChars(rFileName), ImplGetChars(rMountPath));
#endif
    do
    {
        aRC = Directory::getVolumeInfo ( rMountPath, aInfo );
#ifdef MTG_DEBUG
        fprintf(stderr, "MTG: getVolumeInfo returned %d\n", aRC );
#endif
        // return value is not useful, so we check if we have valid attributes instead
        if ( aInfo.isValid ( osl_VolumeInfo_Mask_FreeSpace) )
        {
            sal_Bool bReCheck;
            OUStringBuffer aBuffer;
            aBuffer.append ( rMountPath );
            if ( rMountPath.lastIndexOf ( '/' ) != rMountPath.getLength()-1 )
                aBuffer.appendAscii ( "/" );
            aBuffer.append ( rFileName );
            OUString sFullPath ( aBuffer.makeStringAndClear() );
            do
            {
                bReCheck = sal_False;
                sal_uInt64 nFree = aInfo.getFreeSpace();
#ifdef MTG_DEBUG
                fprintf(stderr, "MTG: free is  %d\n", static_cast < sal_Int32 > ( nFree ) );
#endif
                if ( (  bDynamicSpan && nFree < 1000         ) ||
                     ( !bDynamicSpan && nFree < nSegmentSize ) )
                {
                    if ( !HandleError (  osl_File_E_NOSPC, EC_RETRY|EC_ABORT, sFullPath ) )
                    {
                        if ( pFile )
                            delete pFile;
                        return e_Aborted;
                    }
                    else
                    {
                        aRC = Directory::getVolumeInfo ( rMountPath, aInfo );
                        bReCheck = sal_True;
                    }
                }
                else
                {
                    nLeft = bDynamicSpan ? nFree : nSegmentSize;
#ifdef MTG_DEBUG
                    fprintf(stderr, "MTG: left is %ld\n", static_cast < sal_Int32 > ( nLeft ) );
#endif
                }
            }
            while ( bReCheck );
#ifdef MTG_DEBUG
            fprintf( stderr, "MTG: sDirectoryName is %s sFileName is %s FullPath is %s\n",
                              ImplGetChars ( rMountPath ), ImplGetChars ( rFileName ), ImplGetChars ( sFullPath ) );
#endif
            pFile = new File ( sFullPath );
            sal_Bool bExists = sal_False;
            aRC = pFile->open ( osl_File_OpenFlag_Create | osl_File_OpenFlag_Write );
            if ( aRC == FileBase::E_EXIST )
            {
                aRC = pFile->open ( osl_File_OpenFlag_Write );
                bExists = sal_True;
            }
            if ( aRC != FileBase::E_None )
            {
#ifdef MTG_DEBUG
                fprintf(stderr, "MTG: file open returned %d\n", aRC );
#endif
                if ( ! HandleError (  (oslFileError) aRC, EC_RETRY|EC_ABORT, sFullPath ) )
                {
                    delete pFile;
                    return e_Aborted;
                }
            }
            else if ( bExists )
            {
                // Truncate to 0 if necessary
                aRC = pFile->setSize ( 0 );
            }
        }
        else
        {
            aRC = FileBase::E_IO;
            if ( ! HandleError (  (oslFileError) aRC, EC_RETRY|EC_ABORT, rMountPath ) )
                return e_Aborted;
        }
    }
    while (aRC != FileBase::E_None );

    // Now! We should have an open file on a disk which has at least nSegmentSize if not
    // dynamic spanning and 1000 bytes if dynamic spanning.


    // Let's read it all into the buffer in case something goes wrong and also
    // so that the spannable checks in ByteChucker and ZipOutputStream work

    nRead = xInBuffer->readBytes ( aBuffer, static_cast < sal_Int32 > ( nLeft ) );

    aRC = pFile->write ( aBuffer.getConstArray(), nRead, nWritten );
#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG: write returned %d\n", aRC );
#endif

    sal_Bool bRetry = sal_False;

    if ( nWritten != nRead || aRC != FileBase::E_None )
        bRetry = sal_True;
    else
    {
        aRC = pFile->close ();
#ifdef MTG_DEBUG
        fprintf ( stderr, "MTG: close returned %d\n", aRC );
#endif
        if ( aRC != FileBase::E_None )
            bRetry = sal_True;
    }
    delete pFile;
    return nRead < nLeft ? e_Finished : bRetry ? e_Retry : e_Success;
}

SegmentEnum ZipPackage::writeSegment ( const OUString &rFileName, Reference < XInputStream > &xInBuffer )
{
    FileBase::RC aRC;
    Sequence < sal_Int8 > aBuffer ( nSegmentSize );

    sal_uInt64 nWritten;

    sal_Int32 nRead = xInBuffer->readBytes ( aBuffer, static_cast < sal_Int32 > ( nSegmentSize ) );

    File aFile ( rFileName );
    aRC = aFile.open ( osl_File_OpenFlag_Create | osl_File_OpenFlag_Write );
    if ( aRC == FileBase::E_EXIST )
        aRC = aFile.open ( osl_File_OpenFlag_Write );
    if ( aRC != FileBase::E_None )
    {
        if ( ! HandleError (  (oslFileError) aRC, EC_RETRY|EC_ABORT, rFileName ) )
            return e_Aborted;
    }

    aRC = aFile.write ( aBuffer.getConstArray(), nRead, nWritten );
#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG: write returned %d\n", aRC );
#endif

    sal_Bool bRetry = sal_False;
    if ( nWritten != nRead || aRC != FileBase::E_None )
        bRetry = sal_True;
    else
    {
        aRC = aFile.close ();
#ifdef MTG_DEBUG
        fprintf ( stderr, "MTG: close returned %d\n", aRC );
#endif
        if ( aRC != FileBase::E_None )
            bRetry = sal_True;
    }
    return nRead < nSegmentSize ? e_Finished : bRetry ? e_Retry : e_Success;
}

SegmentEnum ZipPackage::readSegment ( const OUString &rFileName, OUString &rMountPath, Reference < XOutputStream > &xTempOut, const sal_Int16 nDiskNum )
{
    File *pFile = NULL;
    FileBase::RC aRC;
    SegmentEnum eRet;

    OUStringBuffer aStringBuffer;
    aStringBuffer.append ( rMountPath );
    if ( rMountPath.lastIndexOf ( '/' ) != rMountPath.getLength()-1 )
        aStringBuffer.appendAscii ( "/" );
    aStringBuffer.append ( rFileName );
    OUString sFullPath ( aStringBuffer.makeStringAndClear() );

#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG: trying to read %s\n", ImplGetChars ( sFullPath ) );
#endif
    DirectoryItem aItem;
    do
    {
        aRC = DirectoryItem::get ( sFullPath, aItem );
#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG: DirectoryItem::get returned %d\n", aRC );
#endif
        if ( aRC != FileBase::E_None &&  !HandleError (  (oslFileError) aRC, EC_RETRY|EC_ABORT, sFullPath ) )
            return e_Aborted;
    }
    while (aRC != FileBase::E_None );
    do
    {
        pFile = new File ( sFullPath );
        aRC = pFile->open ( osl_File_OpenFlag_Read );
#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG:file open returned %d\n", aRC );
#endif
        if ( aRC != FileBase::E_None )
        {
            delete pFile;
            if ( ! HandleError (  (oslFileError) aRC, EC_RETRY|EC_ABORT, sFullPath ) )
                return e_Aborted;
        }
    }
    while (aRC != FileBase::E_None );

    FileStatus aStatus ( FileStatusMask_FileSize );
    aItem.getFileStatus ( aStatus );
    sal_uInt64 nRead, nLeft = aStatus.getFileSize(), nToRead;

    Sequence < sal_Int8 > aBuffer ( n_ConstBufferSize );
    do
    {
        nToRead = nLeft < n_ConstBufferSize ? nLeft : n_ConstBufferSize;
        aRC = pFile->read ( static_cast < void* > ( aBuffer.getArray() ), nToRead, nRead );
#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG:file read returned %d\n", aRC );
#endif
        if ( aRC != FileBase::E_None )
        {
            if ( ! HandleError (  (oslFileError) aRC, EC_RETRY|EC_ABORT, sFullPath ) )
                return e_Aborted;
        }
        else
        {
            Sequence < sal_Int8 > aReadBuf ( aBuffer.getConstArray(), static_cast < sal_Int32 > ( nRead ) );
            nLeft -= nRead;
            xTempOut->writeBytes ( aReadBuf );
            if (nLeft == 0)
                eRet = checkEnd ( aReadBuf ) ? e_Finished : e_Success;
        }
    }
    while (nLeft > 0 );

    aRC = pFile->close ();
    delete pFile;
    return eRet;
}

sal_Bool SAL_CALL ZipPackage::hasPendingChanges(  )
        throw(RuntimeException)
{
    return sal_False;
}
Sequence< ElementChange > SAL_CALL ZipPackage::getPendingChanges(  )
        throw(RuntimeException)
{
    return Sequence < ElementChange > ( NULL, 0 );
}

/**
 * Function to create a new component instance; is needed by factory helper implementation.
 * @param xMgr service manager to if the components needs other component instances
 */
Reference < XInterface >SAL_CALL ZipPackage_createInstance(
    const Reference< XMultiServiceFactory > & xMgr )
{
    return Reference< XInterface >( *new ZipPackage(xMgr) );
}

OUString ZipPackage::getImplementationName()
{
#if SUPD>625
    return OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.packages.comp.ZipPackage" ) );
#else
    return OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.package.Package" ) );
#endif
}

Sequence< OUString > ZipPackage::getSupportedServiceNames()
{
    Sequence< OUString > aNames(1);
#if SUPD>625
    aNames[0] = OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.packages.comp.ZipPackage" ) );
#else
    aNames[0] = OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.package.Package" ) );
#endif
    return aNames;
}
sal_Bool SAL_CALL ZipPackage::supportsService( OUString const & rServiceName )
    throw (RuntimeException)
{
    return rServiceName == getSupportedServiceNames()[0];
}
Reference < XSingleServiceFactory > ZipPackage::createServiceFactory( Reference < XMultiServiceFactory > const & rServiceFactory )
{
    return cppu::createSingleFactory (rServiceFactory,
                                           getImplementationName(),
                                           ZipPackage_createInstance,
                                           getSupportedServiceNames());
}

//XInterface
Any SAL_CALL ZipPackage::queryInterface( const Type& rType )
    throw(RuntimeException)
{
        return ::cppu::queryInterface ( rType                                       ,
                                        // OWeakObject interfaces
                                        reinterpret_cast< XInterface*       > ( this )  ,
                                        static_cast< XWeak*         > ( this )  ,
                                        // my own interfaces
                                        static_cast< XInitialization*       > ( this )  ,
                                        static_cast< XSingleServiceFactory*     > ( this )  ,
                                        static_cast< XUnoTunnel*        > ( this )  ,
                                        static_cast< XHierarchicalNameAccess*       > ( this )  ,
                                        static_cast< XPropertySet*      > ( this )  ,
                                        static_cast< XChangesBatch* > ( this ) );
}

void SAL_CALL ZipPackage::acquire(  )
    throw()
{
    OWeakObject::acquire();
}
void SAL_CALL ZipPackage::release(  )
    throw()
{
    OWeakObject::release();
}

// XUnoTunnel
Sequence< sal_Int8 > ZipPackage::getUnoTunnelImplementationId( void )
    throw (RuntimeException)
{
    static ::cppu::OImplementationId * pId = 0;
    if (! pId)
    {
        ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
        if (! pId)
        {
            static ::cppu::OImplementationId aId;
            pId = &aId;
        }
    }
    return pId->getImplementationId();
}

sal_Int64 SAL_CALL ZipPackage::getSomething( const Sequence< sal_Int8 >& aIdentifier )
    throw(RuntimeException)
{
    if (aIdentifier.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  aIdentifier.getConstArray(), 16 ) )
        return reinterpret_cast < sal_Int64 > ( this );
    return 0;
}

Reference< XPropertySetInfo > SAL_CALL ZipPackage::getPropertySetInfo(  )
        throw(RuntimeException)
{
    return Reference < XPropertySetInfo > (NULL);
}
void SAL_CALL ZipPackage::setPropertyValue( const OUString& aPropertyName, const Any& aValue )
        throw(UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    if (aPropertyName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("HasEncryptedEntries") ) )
        throw IllegalArgumentException (); // This property is read-only
    else if (aPropertyName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("EncryptionKey") ) )
    {
        if (!( aValue >>= aEncryptionKey ) )
            throw IllegalArgumentException();
    }
    else if (aPropertyName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("SegmentSize") ) )
    {
        if (!( aValue >>= nSegmentSize ) )
            throw IllegalArgumentException();
    }
    else
        throw UnknownPropertyException();
}
Any SAL_CALL ZipPackage::getPropertyValue( const OUString& PropertyName )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    Any aAny;
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "EncryptionKey" ) ) )
    {
        aAny <<= aEncryptionKey;
        return aAny;
    }
    else if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "SegmentSize" ) ) )
    {
        aAny <<= nSegmentSize;
        return aAny;
    }
    if (PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "HasEncryptedEntries" ) ) )
    {
        aAny <<= bHasEncryptedEntries;
        return aAny;
    }
    throw UnknownPropertyException();
}
void SAL_CALL ZipPackage::addPropertyChangeListener( const OUString& aPropertyName, const Reference< XPropertyChangeListener >& xListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
void SAL_CALL ZipPackage::removePropertyChangeListener( const OUString& aPropertyName, const Reference< XPropertyChangeListener >& aListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
void SAL_CALL ZipPackage::addVetoableChangeListener( const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
void SAL_CALL ZipPackage::removeVetoableChangeListener( const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener )
        throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
}
void ZipPackage::getInteractionHandler()
{
    if ( ! xInteractionHandler.is() )
    {
        OUString sServiceName ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.task.InteractionHandler" ) );
        xInteractionHandler = Reference < XInteractionHandler > ( xFactory->createInstance ( sServiceName ), UNO_QUERY );
    }
}

IOErrorCode Impl_OSLFileErrorToUCBIoErrorCode ( oslFileError aRC )
{
    IOErrorCode eReturn = IOErrorCode_UNKNOWN;
    switch ( aRC )
    {
        // open
        case osl_File_E_NOMEM://        not enough memory for allocating structures <br>
            eReturn = IOErrorCode_OUT_OF_MEMORY;
        break;
        case osl_File_E_NAMETOOLONG://  pathname was too long<br>
            eReturn = IOErrorCode_NAME_TOO_LONG;
        break;
        case osl_File_E_NOENT://        No such file or directory<br>
            eReturn = IOErrorCode_NOT_EXISTING;
        break;
        case osl_File_E_ACCES://        permission denied<P>
            eReturn = IOErrorCode_ACCESS_DENIED;
        break;
        case osl_File_E_ISDIR://        Is a directory<p>
            eReturn = IOErrorCode_INVALID_ACCESS;
        break;
        case osl_File_E_NOTDIR://       Not a directory<br>
            eReturn = IOErrorCode_NO_DIRECTORY;
        break;
        case osl_File_E_NXIO://         No such device or address<br>
            eReturn = IOErrorCode_INVALID_DEVICE;
        break;
        case osl_File_E_NODEV://        No such device<br>
            eReturn = IOErrorCode_INVALID_DEVICE;
        break;
        case osl_File_E_ROFS://         Read-only file system<br>
            eReturn = IOErrorCode_ACCESS_DENIED;
        break;
        case osl_File_E_FAULT://        Bad address<br>
            eReturn = IOErrorCode_INVALID_DEVICE;
        break;
        case osl_File_E_LOOP://         Too many symbolic links encountered<br>
        break;
        case osl_File_E_MFILE://        too many open files used by the process<br>
        break;
        case osl_File_E_NFILE://        too many open files in the system<br>
        break;
        case osl_File_E_EXIST://        File exists<br>
            eReturn = IOErrorCode_CANT_CREATE;
        break;
        case osl_File_E_MULTIHOP://     Multihop attempted<br>
        break;
        case osl_File_E_FBIG://         File too large<br>
            eReturn = IOErrorCode_INVALID_LENGTH;
        break;

        // write
        case osl_File_E_AGAIN://        Operation would block<br>
        break;
        case osl_File_E_NOLCK://        No record locks available<br>
        break;
        case osl_File_E_NOSPC://        No space left on device<br>
            eReturn = IOErrorCode_OUT_OF_DISK_SPACE;
        break;
        case osl_File_E_INVAL://        the format of the parameters was not valid<p>
            eReturn = IOErrorCode_INVALID_PARAMETER;
        break;

        // close
        case osl_File_E_BADF://         Bad file<br>
            eReturn = IOErrorCode_NO_FILE;
        break;
        case osl_File_E_INTR://         function call was interrupted<br>
            eReturn = IOErrorCode_ABORT;
        break;
        case osl_File_E_NOLINK://       Link has been severed<br>
        break;
        case osl_File_E_IO://           I/O error<p>
            eReturn = IOErrorCode_GENERAL;
        break;
    }
    return eReturn;
}

sal_Bool ZipPackage::HandleError ( Any &rAny, sal_uInt16 eContinuations )
{
    InteractionRequest* pRequest;
    Reference < XInteractionRequest > xRequest ( pRequest = new InteractionRequest ( rAny, eContinuations ));
    xInteractionHandler->handle ( xRequest );
    const sal_uInt16 nSelection = pRequest->getSelection();
    return nSelection == EC_YES || nSelection == EC_RETRY;
}

sal_Bool ZipPackage::HandleError ( oslFileError aRC, sal_uInt16 eContinuations, const OUString &rFileName )
{
    Any aAny;
    InteractiveAugmentedIOException aException;
    aException.Code = Impl_OSLFileErrorToUCBIoErrorCode ( aRC );
    aException.Arguments.realloc ( 1 );
    PropertyValue aProperty;
    aProperty.Name = OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Uri" ) );
    aProperty.Handle = static_cast < sal_Int32 > ( -1 );
    aProperty.Value <<= rFileName;
    aException.Arguments[0] <<= aProperty;
    aAny <<= aException;
    return HandleError (aAny, eContinuations );
}

void ZipPackage::unSpanFile ( )
{
    const OUString sServiceName ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.io.TempFile" ) );
    Reference < XInputStream > xTempIn = Reference < XInputStream > ( xFactory->createInstance ( sServiceName ), UNO_QUERY );
    Reference < XOutputStream > xTempOut = Reference < XOutputStream > ( xTempIn, UNO_QUERY );
    Reference < XSeekable > xTempSeek = Reference < XSeekable > ( xTempIn, UNO_QUERY );

#ifdef MTG_DEBUG
    fprintf ( stderr, "MTG: unspanning %s\n", ImplGetChars ( sURL ) );
#endif
    Sequence < sal_Int8 > aBuffer;
    sal_Int64 nRead;
    xContentSeek->seek ( 0 );
    do
    {
        nRead = xContentStream->readBytes ( aBuffer, n_ConstBufferSize );
        xTempOut->writeBytes ( aBuffer );
    }
    while ( nRead == n_ConstBufferSize );

    // Clear the references to the first segment so that we can unmount the disk
    xContentStream = xTempIn;
    xContentSeek = xTempSeek;

    // Check if the buffer just read is the last one
    if ( checkEnd ( aBuffer ) )
        return;

    sal_Int16 nDiskNum = 1;
    VolumeInfo aInfo ( osl_VolumeInfo_Mask_FreeSpace | osl_VolumeInfo_Mask_DeviceHandle | osl_VolumeInfo_Mask_Attributes );
    FileBase::RC aRC = Directory::getVolumeInfo ( sURL, aInfo );
#ifdef MTG_DEBUG
    fprintf ( stderr, "GetVolumeInfo returned %d\n", aRC );
#endif
    VolumeDevice aDevice = aInfo.getDeviceHandle();
    sal_Bool bIsRemovable = aInfo.getRemoveableFlag();
#ifdef MTG_DEBUG
    fprintf ( stderr, "isRemovable is %d\n", bIsRemovable );
#endif

    sal_Int32 nLastSlash = sURL.lastIndexOf ( '/' );
    OUString sFileName, sMountPath = aDevice.getMountPath();
#ifdef MTG_DEBUG
    fprintf ( stderr, "mountpath is %s\n", ImplGetChars ( sMountPath ) );
#endif
    const OUString sFilePrefix = sURL.copy ( 1 + nLastSlash,  sURL.lastIndexOf ( '.' ) - nLastSlash );
    SegmentEnum eRet = e_Finished;
    SuffixGenerator aGenerator;
    do
    {
        //pBuffer->nextSegment( ++nDiskNum);
        nDiskNum++;
        aGenerator.generateFileName ( sFileName, sFilePrefix, nDiskNum );
        if ( bIsRemovable )
        {
            // We need an interaction handler to request disks
            getInteractionHandler();
            if ( RequestDisk( sMountPath, nDiskNum ) < 0 )
                return;
#ifdef MTG_DEBUG
    fprintf ( stderr, "unSpanFile calling readSegment on disk number %d (mount path is %s )\n", nDiskNum, ImplGetChars ( sMountPath ) );
#endif
            eRet = readSegment ( sFileName, sMountPath, xTempOut, nDiskNum );
            if (eRet == e_Aborted)
                return;
        }
        else
        {
            OUString sFullPath = sURL.copy ( 0, nLastSlash + 1 ) + sFileName;
            Reference < XInputStream > xStream;
            Content aContent (sFullPath, Reference < XCommandEnvironment >() );
            Reference < XActiveDataSink > xSink = new ZipPackageSink;
            try
            {
                if (aContent.openStream ( xSink ) )
                    xStream = xSink->getInputStream();
                if ( xStream.is() )
                {
                    do
                    {
                        nRead = xStream->readBytes ( aBuffer, n_ConstBufferSize );
                        xTempOut->writeBytes ( aBuffer );
                    }
                    while ( nRead == n_ConstBufferSize );
                    eRet = checkEnd ( aBuffer ) ? e_Finished : e_Success;
                }
            }
            catch (com::sun::star::uno::Exception&)
            {
                // bad juju
                //
            }
        }
    }
    while ( eRet != e_Finished );
}

sal_Bool ZipPackage::checkEnd ( Sequence < sal_Int8 > &rSequence )
{
    sal_Int32 nLength, nPos, nEnd;

    nLength = static_cast <sal_Int32 > (rSequence.getLength());
    // Start at what should be the beginning of the end header
    if ( nLength == 0 || nLength < ENDHDR )
        return sal_False;
    nPos = nLength - ENDHDR - ZIP_MAXNAMELEN;
    // We'll only look for the END signature within a sane range (ZIP_MAXNAMELEN)
    nEnd = nPos >= 0 ? nPos : 0;

    const sal_Int8 *pBuffer = rSequence.getConstArray();
    nPos = nLength - ENDHDR;
    while ( nPos >= nEnd )
    {
        if (pBuffer[nPos] == 'P' && pBuffer[nPos+1] == 'K' && pBuffer[nPos+2] == 5 && pBuffer[nPos+3] == 6 )
            return sal_True;
        nPos--;
    }
    return sal_False;
}
