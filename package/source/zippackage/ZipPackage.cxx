/*************************************************************************
 *
 *  $RCSfile: ZipPackage.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: mtg $ $Date: 2000-11-29 03:21:56 $
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
#ifndef _ZIP_PACKAGE_HXX
#include "ZipPackage.hxx"
#endif

using namespace rtl;
using namespace com::sun::star::io;
using namespace com::sun::star::registry;
using namespace com::sun::star::util;
using namespace com::sun::star::uno;
using namespace com::sun::star::container;
using namespace com::sun::star::package;
using namespace com::sun::star::lang;
using namespace com::sun::star::package::ZipConstants;

ZipPackage::ZipPackage (Reference < XInputStream > &xNewInput,
                        const Reference < XMultiServiceFactory > &xNewFactory)
: pContent(NULL)
, pZipFile(NULL)
, pRootFolder(NULL)
, xContentStream (xNewInput)
, xContentSeek (xNewInput, UNO_QUERY)
, xRootFolder (NULL)
, xZipFile (NULL)
, xBuffer (NULL)
, xFactory(xNewFactory)
{
    pZipFile    = new ZipFile(xContentStream);
    pRootFolder = new ZipPackageFolder( );
    xRootFolder = Reference < XUnoTunnel >   (pRootFolder );
    getZipFileContents();
}

ZipPackage::ZipPackage (const Reference < XMultiServiceFactory > &xNewFactory)
: pContent(NULL)
, pZipFile(NULL)
, pRootFolder(NULL)
, xContentStream (NULL)
, xContentSeek (NULL)
, xRootFolder (NULL)
, xZipFile (NULL)
, xFactory(xNewFactory)
{

    pRootFolder = new ZipPackageFolder();
    xRootFolder = Reference < XUnoTunnel >   (pRootFolder );
}

ZipPackage::~ZipPackage( void )
{
    if (pContent)
        delete pContent;
/*  if (pZipFile)
        delete pZipFile;
    if (pZipOut)
        delete pZipOut;
    if (pZipBuffer)
        delete pZipBuffer;
    if (pRootFolder)
        delete pRootFolder;
    destroyFolderTree(xRootFolder);
*/
}

void ZipPackage::destroyFolderTree( Reference < XUnoTunnel > xFolder )
{

    ZipPackageFolder *pCurrentFolder = reinterpret_cast < ZipPackageFolder* > (xFolder->getSomething(ZipPackageFolder::getUnoTunnelImplementationId()));
    Reference < XEnumeration > xEnum = pCurrentFolder->createEnumeration();
    while (xEnum->hasMoreElements())
    {
        Reference < XUnoTunnel > xTunnel;
        Any aAny = xEnum->nextElement();

        ZipPackageFolder *pFolder = NULL;
        ZipPackageStream *pStream = NULL;
        sal_Bool bIsFolder = sal_False;

        aAny >>= xTunnel;
        try
        {
            pFolder = reinterpret_cast < ZipPackageFolder* > (xTunnel->getSomething(ZipPackageFolder::getUnoTunnelImplementationId()));
            bIsFolder = sal_True;
        }
        catch (RuntimeException)
        {
            pStream = reinterpret_cast < ZipPackageStream* > (xTunnel->getSomething(ZipPackageStream::getUnoTunnelImplementationId()));
            bIsFolder = sal_False;
        }
        if (bIsFolder)
        {
            destroyFolderTree(xTunnel);
            delete pFolder;
        }
        else
        {
            delete pStream;
        }
    }
}

void ZipPackage::getZipFileContents()
{
    Reference< XEnumeration > xEnum = pZipFile->entries();
    Reference< XNameContainer > xCurrent (xRootFolder, UNO_QUERY);
    ZipPackageStream *pPkgStream;
    ZipPackageFolder *pPkgFolder;
    ZipEntry aEntry;

    while (xEnum->hasMoreElements())
    {
        xCurrent  = Reference < XNameContainer > (pRootFolder);
        sal_Int32 nOldIndex =0,nIndex = 0;
        Any aAny = xEnum->nextElement();
        aAny >>= aEntry;
        OUString &rName = aEntry.sName;

        if (rName.lastIndexOf('/') == rName.getLength()-1)
        {
            while ((nIndex = rName.indexOf('/', nOldIndex)) != -1)
            {
                OUString sTemp = rName.copy (nOldIndex, nIndex - nOldIndex);
                if (nIndex == nOldIndex) //sTemp.getLength() == 1)
                    break;
                if (!xCurrent->hasByName(sTemp))
                {
                    pPkgFolder = new ZipPackageFolder();//*this);
                    pPkgFolder->setName(sTemp);
                    pPkgFolder->setParent( Reference < XInterface >(xCurrent, UNO_QUERY));
                    aAny <<= Reference < XUnoTunnel > (pPkgFolder);
                    xCurrent->insertByName(sTemp, aAny);
                    xCurrent = Reference < XNameContainer > (pPkgFolder);
                }
                else
                {
                    aAny = xCurrent->getByName(sTemp);
                    Reference < XUnoTunnel> xRef;
                    aAny >>= xRef;
                    xCurrent = Reference < XNameContainer > (xRef, UNO_QUERY);
                }

                nOldIndex = nIndex+1;
            }
        }
        else
        {
            while ((nIndex = rName.indexOf('/', nOldIndex)) != -1)
            {
                OUString sTemp = rName.copy (nOldIndex, nIndex - nOldIndex);
                if (nIndex == nOldIndex) //sTemp.getLength() == 1)
                    break;
                if (!xCurrent->hasByName(sTemp))
                {
                    pPkgFolder = new ZipPackageFolder();//*this);
                    pPkgFolder->setName(sTemp);
                    pPkgFolder->setParent( Reference < XInterface >(xCurrent, UNO_QUERY));
                    aAny <<= Reference < XUnoTunnel > (pPkgFolder);
                    xCurrent->insertByName(sTemp, aAny);
                    xCurrent = Reference < XNameContainer > (pPkgFolder);
                }
                else
                {
                    aAny = xCurrent->getByName(sTemp);
                    Reference < XUnoTunnel> xRef;
                    aAny >>= xRef;
                    xCurrent = Reference < XNameContainer > (xRef, UNO_QUERY);
                }
                nOldIndex = nIndex+1;
            }
            OUString sStreamName = rName.copy( nOldIndex, rName.getLength() - nOldIndex);
            if (isZipFile(aEntry))
            {
                Reference < XInputStream > xContentStream = pZipFile->getInputStream(aEntry);
                ZipPackage *pInZip = new ZipPackage (xContentStream, xFactory );
                aContainedZips.push_back (Reference < XSingleServiceFactory > (pInZip));
                pPkgFolder = pInZip->getRootFolder();
                pPkgFolder->setName(sStreamName);
                pPkgFolder->setParent( Reference < XInterface >(xCurrent, UNO_QUERY));
                aAny <<= Reference < XUnoTunnel > (pPkgFolder);
                xCurrent->insertByName(sStreamName, aAny);
            }
            else
            {
                pPkgStream = new ZipPackageStream( pZipFile );
                pPkgStream->bPackageMember = sal_True;
                pPkgStream->setParent( Reference < XInterface > (xCurrent, UNO_QUERY));
                pPkgStream->setZipEntry( aEntry );
                pPkgStream->setName( sStreamName );
                aAny <<= Reference < XUnoTunnel > (pPkgStream);
                xCurrent->insertByName(sStreamName, aAny);
            }
        }
    }
    if (hasByHierarchicalName(OUString::createFromAscii("META-INF/manifest.xml")))
    {
        Any aAny = getByHierarchicalName(OUString::createFromAscii("META-INF/manifest.xml"));
        Reference < XUnoTunnel > xTunnel;
        aAny >>= xTunnel;
        Reference < XActiveDataSink > xSink (xTunnel, UNO_QUERY);
        ManifestReader aReader(*this, xSink->getInputStream(), xFactory);
        aReader.Read();
    }
}
// XInitialization
void SAL_CALL ZipPackage::initialize( const Sequence< Any >& aArguments )
        throw(Exception, RuntimeException)
{
    OUString sURL;
    aArguments[0] >>= sURL;
    pContent = new ::ucb::Content(sURL, Reference < com::sun::star::ucb::XCommandEnvironment >() );
    Reference < XActiveDataSink > xSink = new ZipPackageSink;
    if (pContent->openStream ( xSink) )
        xContentStream = xSink->getInputStream();

    xContentSeek = Reference < XSeekable > (xContentStream, UNO_QUERY);
    pZipFile    = new ZipFile(xContentStream);
    //xZipFile  = Reference < XZipFile >         (pZipFile);
    getZipFileContents();
}
// XHierarchicalNameAccess
Any SAL_CALL ZipPackage::getByHierarchicalName( const OUString& aName )
        throw(NoSuchElementException, RuntimeException)
{
    sal_Int32 nOldIndex =0,nIndex = 0;
    Any aAny;
    Reference < XNameContainer > xCurrent  = Reference < XNameContainer > (pRootFolder);
    if (aName[nOldIndex] == '/')
        nOldIndex++;

    if (aName == OUString::createFromAscii("/"))
    {
        aAny <<= Reference < XUnoTunnel > (pRootFolder);
    }
    else if (aName.lastIndexOf('/') == aName.getLength()-1)
    {
        while ((nIndex = aName.indexOf('/', nOldIndex)) != -1)
        {
            OUString sTemp = aName.copy (nOldIndex, nIndex - nOldIndex);
            if (nIndex == nOldIndex) //sTemp.getLength() == 1)
                break;
            if (xCurrent->hasByName(sTemp))
            {
                Reference < XUnoTunnel > xRef;
                aAny = xCurrent->getByName(sTemp);
                aAny >>= xRef;
                xCurrent = Reference < XNameContainer > (xRef, UNO_QUERY);
            }
            else
                throw (NoSuchElementException());
            nOldIndex = nIndex+1;
        }
    }
    else
    {
        while ((nIndex = aName.indexOf('/', nOldIndex)) != -1)
        {
            OUString sTemp = aName.copy (nOldIndex, nIndex - nOldIndex);
            if (nIndex == nOldIndex) //sTemp.getLength() == 1)
                break;
            if (xCurrent->hasByName(sTemp))
            {
                Reference < XUnoTunnel > xChildRef;
                aAny = xCurrent->getByName(sTemp);
                aAny >>= xChildRef;
                xCurrent = Reference < XNameContainer > (xChildRef, UNO_QUERY);
            }
            else
                throw (NoSuchElementException());

            nOldIndex = nIndex+1;
        }
        OUString sStreamName = aName.copy( nOldIndex, aName.getLength() - nOldIndex);
        if (xCurrent->hasByName(sStreamName))
            aAny <<= xCurrent->getByName(sStreamName);
        else
            throw (NoSuchElementException());
    }
    return aAny;
}
sal_Bool SAL_CALL ZipPackage::hasByHierarchicalName( const OUString& aName )
        throw(RuntimeException)
{
    sal_Int32 nOldIndex =0,nIndex = 0;
    Any aAny;
    Reference < XNameContainer > xCurrent  = Reference < XNameContainer > (pRootFolder);

    if (aName[nOldIndex] == '/')
        nOldIndex++;

    if (aName == OUString::createFromAscii("/"))
    {
        return sal_True;
    }
    else if (aName.lastIndexOf('/') == aName.getLength()-1)
    {
        while ((nIndex = aName.indexOf('/', nOldIndex)) != -1)
        {
            OUString sTemp = aName.copy (nOldIndex, nIndex - nOldIndex);
            if (nIndex == nOldIndex) //sTemp.getLength() == 1)
                break;
            if (xCurrent->hasByName(sTemp))
            {
                Reference < XUnoTunnel > xRef;
                aAny = xCurrent->getByName(sTemp);
                aAny >>= xRef;
                xCurrent = Reference < XNameContainer > (xRef, UNO_QUERY);
            }
            else
                return sal_False;
            nOldIndex = nIndex+1;
        }
        return sal_True;
    }
    else
    {
        while ((nIndex = aName.indexOf('/', nOldIndex)) != -1)
        {
            OUString sTemp = aName.copy (nOldIndex, nIndex - nOldIndex);
            if (nIndex == nOldIndex) //sTemp.getLength() == 1)
                break;
            if (xCurrent->hasByName(sTemp))
            {
                Reference < XUnoTunnel > xChildRef;
                aAny = xCurrent->getByName(sTemp);
                aAny >>= xChildRef;
                xCurrent = Reference < XNameContainer > (xChildRef, UNO_QUERY);
            }
            else
                return sal_False;
            nOldIndex = nIndex+1;
        }
        OUString sStreamName = aName.copy( nOldIndex, aName.getLength() - nOldIndex);
        return xCurrent->hasByName(sStreamName);
    }
}
// XSingleServiceFactory
Reference< XInterface > SAL_CALL ZipPackage::createInstance(  )
        throw(Exception, RuntimeException)
{
    Reference < XInterface > xRef = *(new ZipPackageStream ( pZipFile ));
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
        xRef = *new ZipPackageStream ( pZipFile );

    return xRef;
}
// XChangesBatch
void SAL_CALL ZipPackage::commitChanges(  )
        throw(WrappedTargetException, RuntimeException)
{
    Reference < XNameContainer > xMetaFolder;
    Reference < XUnoTunnel > xMetaTunnel;
    std::vector < ManifestEntry * > aManList;
    Any aAny;
    ZipPackageBuffer *pZipBuffer = new ZipPackageBuffer( 65535 );
    Reference < XOutputStream > xOutStream (pZipBuffer);
    ZipOutputStream *pZipOut = new ZipOutputStream( xOutStream, 65535 );
    Reference < XZipOutputStream > xZipOut (pZipOut);
    pZipOut->setMethod(DEFLATED);
    pZipOut->setLevel(DEFAULT_COMPRESSION);

    sal_Bool bAddMetaFolder = sal_False;
    if (hasByHierarchicalName(OUString::createFromAscii("META-INF/")))
    {
        aAny = getByHierarchicalName(OUString::createFromAscii("META-INF/"));
        aAny >>= xMetaTunnel;
        xMetaFolder = Reference < XNameContainer > (xMetaTunnel, UNO_QUERY);
        if (xMetaFolder->hasByName(OUString::createFromAscii("manifest.xml")))
            xMetaFolder->removeByName(OUString::createFromAscii("manifest.xml"));
    }
    else
        bAddMetaFolder = sal_True;

    pRootFolder->saveContents(OUString::createFromAscii(""), aManList, *pZipOut);

    if (bAddMetaFolder)
    {
        sal_Bool bFolder = sal_True;
        aAny <<= bFolder;
        xMetaFolder = Reference < XNameContainer> (createInstanceWithArguments( Sequence < Any > (&aAny, 1)), UNO_QUERY);
        Reference < XNameContainer > xZipRoot = Reference < XNameContainer > (xRootFolder, UNO_QUERY);
        xMetaTunnel = Reference < XUnoTunnel > (xMetaFolder, UNO_QUERY);
        aAny <<= xMetaTunnel;
        xZipRoot->insertByName(OUString::createFromAscii("META-INF"), aAny);
    }

    Reference < XUnoTunnel > xManifestStream = Reference < XUnoTunnel > (createInstance(), UNO_QUERY);
    ZipPackageStream * pManifestStream = reinterpret_cast < ZipPackageStream * > (xManifestStream->getSomething(ZipPackageStream::getUnoTunnelImplementationId()));
    ZipPackageBuffer *pBuffer = new ZipPackageBuffer(65535);

    Reference < XOutputStream > xManOutStream (pBuffer);
    pManifestStream->aEntry.sName = OUString::createFromAscii("META-INF/manifest.xml");
    pManifestStream->aEntry.nVersion = -1;
    pManifestStream->aEntry.nFlag = 0;
    pManifestStream->aEntry.nMethod = STORED;
    pManifestStream->aEntry.nTime = -1;
    pManifestStream->aEntry.nCrc = 0;
    pManifestStream->aEntry.nOffset = -1;
    pManifestStream->bPackageMember = sal_True;

    ManifestWriter aWriter ( xManOutStream, xFactory, aManList);
    aWriter.Write();
    pManifestStream->aEntry.nSize = pManifestStream->aEntry.nCompressedSize = pBuffer->getPosition();
    pBuffer->aBuffer.realloc(pBuffer->getPosition());
    CRC32 aCRC;
    aCRC.update(pBuffer->aBuffer);
    pManifestStream->aEntry.nCrc = aCRC.getValue();

    pZipOut->putNextEntry(pManifestStream->aEntry);
    pZipOut->write(pBuffer->aBuffer, 0, pBuffer->getPosition());
    pZipOut->closeEntry();

    pZipOut->finish();
    pZipBuffer->seek(0);

    aAny <<= xManifestStream;
    xMetaFolder->insertByName(OUString::createFromAscii("manifest.xml"), aAny);

    pZipFile->updateFromManList( aManList );
    for (sal_Int32 i=0 ; i < aManList.size(); i++)
    {
        ZipEntry * pEntry = aManList[i]->pEntry;
        pEntry->sName = aManList[i]->sShortName;
    }
    pContent->writeStream(Reference < XInputStream > (pZipBuffer), sal_True);
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

sal_Bool ZipPackage::isZipFile(com::sun::star::package::ZipEntry &rEntry)
{
    Reference < XInputStream > xContentStream = pZipFile->getInputStream(rEntry);
    Sequence < sal_Int8 > aSequence (4);
    xContentStream->readBytes(aSequence, 4);
    sal_uInt32 nTestSig = static_cast < sal_uInt32 >
            (static_cast < sal_uInt8> (aSequence[0]& 0xFF)
           | static_cast < sal_uInt8> (aSequence[1]& 0xFF) << 8
           | static_cast < sal_uInt8> (aSequence[2]& 0xFF) << 16
           | static_cast < sal_uInt8> (aSequence[3]& 0xFF) << 24);
    if (nTestSig == LOCSIG)
        return sal_True;
    else
        return sal_False;
}

/**
 * Function to create a new component instance; is needed by factory helper implementation.
 * @param xMgr service manager to if the components needs other component instances
 */
Reference < XInterface >SAL_CALL ZipPackage_create(
    const Reference< XMultiServiceFactory > & xMgr )
{
    return Reference< XInterface >( *new ZipPackage(xMgr) );
    //return Reference < XInterface > (NULL);
}

extern "C" void SAL_CALL component_getImplementationEnvironment(
                const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}
Sequence< OUString > ZipPackage_getSupportedServiceNames()
{
    Sequence< OUString > seqNames(1);
    seqNames.getArray()[0] = OUString::createFromAscii( "com.sun.star.package.Package" );
    return seqNames;
}

/**
 * This function creates an implementation section in the registry and another subkey
 * for each supported service.
 * @param pServiceManager generic uno interface providing a service manager
 * @param pRegistryKey generic uno interface providing registry key to write
 */
extern "C" sal_Bool SAL_CALL component_writeInfo( void* pServiceManager, void* pRegistryKey )
{
    if (pRegistryKey)
    {
        try
        {
            Reference< XRegistryKey > xNewKey(
            reinterpret_cast< XRegistryKey * >( pRegistryKey )->createKey(
                 OUString::createFromAscii("/com.sun.star.package.Package/UNO/SERVICES") ) );
            const Sequence< OUString > & rSNL = ZipPackage_getSupportedServiceNames();
            const OUString * pArray = rSNL.getConstArray();
            for ( sal_Int32 nPos = rSNL.getLength(); nPos--; )
                xNewKey->createKey( pArray[nPos] );
           return sal_True;
        }
        catch ( InvalidRegistryException& )
        {
            VOS_DEBUG_ONLY( "InvalidRegistryException detected\n");
            return sal_False;
        }

    }
    return sal_False;
}


/**
 * This function is called to get service factories for an implementation.
 * @param pImplName name of implementation
 * @param pServiceManager generic uno interface providing a service manager to instantiate components
 * @param pRegistryKey registry data key to read and write component persistent data
 * @return a component factory (generic uno interface)
 */
extern "C" void * SAL_CALL component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    void * pRet = 0;
    // which implementation is demanded?
    if (pServiceManager && !rtl_str_compare( pImplName, "com.sun.star.package.Package" ))
    {
        OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.package.Package") );
        Reference< XSingleServiceFactory > xFactory(
           cppu::createSingleFactory( // helper function from cppuhelper lib
           reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
           OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.package.Package") ),
           ZipPackage_create, ZipPackage_getSupportedServiceNames() ) );

        if (xFactory.is())
        {
           xFactory->acquire();
           pRet = xFactory.get();
        }
    }
    return pRet;
}
