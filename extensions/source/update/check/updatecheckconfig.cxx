/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: updatecheckconfig.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2007-07-31 15:57:08 $
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
#include "precompiled_extensions.hxx"


#include "updatecheckconfig.hxx"

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_CHANGESEVENT_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif

#include <osl/security.hxx>
#include <osl/time.h>
#include <osl/file.hxx>

#ifdef WNT
#pragma warning(push,1) // disable warnings within system headers
#pragma warning(disable: 4917)
#include <shlobj.h>
#pragma warning(pop)
#endif

namespace container = com::sun::star::container ;
namespace beans = com::sun::star::beans ;
namespace lang = com::sun::star::lang ;
namespace util = com::sun::star::util ;
namespace uno = com::sun::star::uno ;

#define UNISTRING(s) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(s))

#define LAST_CHECK              "LastCheck"
#define VERSION_FOUND           "UpdateVersionFound"
#define UPDATE_VERSION          "UpdateVersion"
#define UPDATE_BUILDID          "UpdateBuildId"
#define UPDATE_DESCRIPTION      "UpdateDescription"
#define DOWNLOAD_URL            "DownloadURL"
#define IS_DIRECT_DOWNLOAD      "IsDirectDownload"
#define OLD_VERSION             "UpdateFoundFor"
#define AUTOCHECK_ENABLED       "AutoCheckEnabled"
#define AUTODOWNLOAD_ENABLED    "AutoDownloadEnabled"
#define CHECK_INTERVAL          "CheckInterval"
#define LOCAL_FILE              "LocalFile"
#define DOWNLOAD_SIZE           "DownloadSize"
#define DOWNLOAD_PAUSED         "DownloadPaused"
#define DOWNLOAD_DESTINATION    "DownloadDestination"
#define RELEASE_NOTE            "ReleaseNote"

static const sal_Char * const aUpdateEntryProperties[] = {
    UPDATE_VERSION,
    UPDATE_BUILDID,
    UPDATE_DESCRIPTION,
    DOWNLOAD_URL,
    IS_DIRECT_DOWNLOAD,
    RELEASE_NOTE"1",
    RELEASE_NOTE"2",
    RELEASE_NOTE"3",
    RELEASE_NOTE"4",
    RELEASE_NOTE"5",
    OLD_VERSION
};

static const sal_uInt32 nUpdateEntryProperties = sizeof(aUpdateEntryProperties) / sizeof(sal_Char *);

//------------------------------------------------------------------------------

NamedValueByNameAccess::~NamedValueByNameAccess()
{
}

//------------------------------------------------------------------------------

::com::sun::star::uno::Any
NamedValueByNameAccess::getValue(const sal_Char * pName)
{
    const sal_Int32 nLen = m_rValues.getLength();
    for( sal_Int32 n=0; n < nLen; ++n )
    {
        if( m_rValues[n].Name.equalsAscii( pName ) )
            return m_rValues[n].Value;
    }
    return ::com::sun::star::uno::Any();
}


//------------------------------------------------------------------------------

bool
UpdateCheckROModel::isAutoCheckEnabled() const
{
    return sal_True == m_aNameAccess.getValue(AUTOCHECK_ENABLED).get< sal_Bool >();
}

//------------------------------------------------------------------------------

bool
UpdateCheckROModel::isAutoDownloadEnabled() const
{
    return sal_True == m_aNameAccess.getValue(AUTODOWNLOAD_ENABLED).get< sal_Bool >();
}

//------------------------------------------------------------------------------

bool
UpdateCheckROModel::isDownloadPaused() const
{
    return sal_True == m_aNameAccess.getValue(DOWNLOAD_PAUSED).get< sal_Bool >();
}

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckROModel::getStringValue(const sal_Char * pStr) const
{
    uno::Any aAny( m_aNameAccess.getValue(pStr) );
    rtl::OUString aRet;

    aAny >>= aRet;

    return aRet;
}

//------------------------------------------------------------------------------

rtl::OUString UpdateCheckROModel::getLocalFileName() const
{
    return getStringValue(LOCAL_FILE);
};

//------------------------------------------------------------------------------

sal_Int64 UpdateCheckROModel::getDownloadSize() const
{
    uno::Any aAny( m_aNameAccess.getValue(DOWNLOAD_SIZE) );
    sal_Int64 nRet = -1;

    aAny >>= nRet;
    return nRet;
};

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckROModel::getUpdateEntryVersion() const
{
    return getStringValue(OLD_VERSION);
}

//------------------------------------------------------------------------------

void
UpdateCheckROModel::getUpdateEntry(UpdateInfo& rInfo) const
{
    rInfo.BuildId = getStringValue(UPDATE_BUILDID);
    rInfo.Version = getStringValue(UPDATE_VERSION);
    rInfo.Description = getStringValue(UPDATE_DESCRIPTION);

    sal_Bool isDirectDownload = sal_False;
    m_aNameAccess.getValue(IS_DIRECT_DOWNLOAD) >>= isDirectDownload;

    rInfo.Sources.push_back( DownloadSource( isDirectDownload, getStringValue(DOWNLOAD_URL) ) );

    rtl::OString aStr(RELEASE_NOTE);
    for(sal_Int32 n=1; n < 6; ++n )
    {
        rtl::OUString aUStr = getStringValue(aStr + rtl::OString::valueOf(n));
        if( aUStr.getLength() > 0 )
            rInfo.ReleaseNotes.push_back(ReleaseNote((sal_Int8) n, aUStr));
    }
}


//------------------------------------------------------------------------------

rtl::OUString UpdateCheckConfig::getDesktopDirectory()
{
    rtl::OUString aRet;

#ifdef WNT
    WCHAR szPath[MAX_PATH];

    if( ! FAILED( SHGetSpecialFolderPathW( NULL, szPath, CSIDL_DESKTOPDIRECTORY, true ) ) )
    {
        aRet = rtl::OUString( szPath );
        osl::FileBase::getFileURLFromSystemPath( aRet, aRet );
    }
#else
    // This should become a desktop specific setting in some system backend ..
    osl::Security().getHomeDir(aRet);
    aRet += rtl::OUString::createFromAscii("/Desktop");
#endif

    return aRet;
}

//------------------------------------------------------------------------------

rtl::OUString UpdateCheckConfig::getAllUsersDirectory()
{
    rtl::OUString aRet;

#ifdef WNT
    WCHAR szPath[MAX_PATH];

    if( ! FAILED( SHGetSpecialFolderPathW( NULL, szPath, CSIDL_COMMON_DOCUMENTS, true ) ) )
    {
        aRet = rtl::OUString( szPath );
        osl::FileBase::RC rc;
        rc = osl::FileBase::getFileURLFromSystemPath( aRet, aRet );
    }
#else
    osl::FileBase::getTempDirURL(aRet);
#endif

    return aRet;
}

//------------------------------------------------------------------------------

UpdateCheckConfig::UpdateCheckConfig(
    const uno::Reference<container::XNameContainer>& xContainer,
    const ::rtl::Reference< UpdateCheckConfigListener >& rListener
) : m_xContainer(xContainer), m_rListener(rListener)
{
}

//------------------------------------------------------------------------------

UpdateCheckConfig::~UpdateCheckConfig()
{
}

//------------------------------------------------------------------------------

::rtl::Reference< UpdateCheckConfig >
UpdateCheckConfig::get(
    const uno::Reference<uno::XComponentContext>& xContext,
    const ::rtl::Reference< UpdateCheckConfigListener >& rListener)
{
    if( !xContext.is() )
        throw uno::RuntimeException(
            UNISTRING( "UpdateCheckConfig: empty component context" ),
            uno::Reference< uno::XInterface >() );

    uno::Reference< lang::XMultiComponentFactory > xServiceManager(xContext->getServiceManager());

    if( !xServiceManager.is() )
        throw uno::RuntimeException(
            UNISTRING( "UpdateCheckConfig: unable to obtain service manager from component context" ),
            uno::Reference< uno::XInterface >() );

    uno::Reference< lang::XMultiServiceFactory > xConfigProvider(
        xServiceManager->createInstanceWithContext( UNISTRING( "com.sun.star.configuration.ConfigurationProvider" ), xContext ),
        uno::UNO_QUERY_THROW);

    beans::PropertyValue aProperty;
    aProperty.Name  = UNISTRING( "nodepath" );
    aProperty.Value = uno::makeAny( UNISTRING("org.openoffice.Office.Jobs/Jobs/UpdateCheck/Arguments") );

    uno::Sequence< uno::Any > aArgumentList( 1 );
    aArgumentList[0] = uno::makeAny( aProperty );

    uno::Reference< container::XNameContainer > xContainer(
        xConfigProvider->createInstanceWithArguments(
            UNISTRING("com.sun.star.configuration.ConfigurationUpdateAccess"), aArgumentList ),
        uno::UNO_QUERY_THROW );

    return new UpdateCheckConfig( xContainer, rListener );
}

//------------------------------------------------------------------------------

bool
UpdateCheckConfig::isAutoCheckEnabled() const
{
    sal_Bool nValue;
    const_cast < UpdateCheckConfig *> (this)->getByName( UNISTRING( AUTOCHECK_ENABLED ) ) >>= nValue;
    return nValue;
}

//------------------------------------------------------------------------------

bool
UpdateCheckConfig::isAutoDownloadEnabled() const
{
    sal_Bool nValue;
    const_cast < UpdateCheckConfig *> (this)->getByName( UNISTRING( AUTODOWNLOAD_ENABLED ) ) >>= nValue;
    return nValue;
}

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckConfig::getUpdateEntryVersion() const
{
    rtl::OUString aValue;

    // getByName is defined as non const in XNameAccess
    const_cast < UpdateCheckConfig *> (this)->getByName( UNISTRING( OLD_VERSION ) ) >>= aValue;

    return aValue;
}

//------------------------------------------------------------------------------

sal_Int64
UpdateCheckConfig::getLastChecked() const
{
    sal_Int64 nValue;

    // getByName is defined as non const in XNameAccess
    const_cast < UpdateCheckConfig *> (this)->getByName( UNISTRING( LAST_CHECK ) ) >>= nValue;

    return nValue;
}

//------------------------------------------------------------------------------

sal_Int64
UpdateCheckConfig::getCheckInterval() const
{
    sal_Int64 nValue;

    // getByName is defined as non const in XNameAccess
    const_cast < UpdateCheckConfig *> (this)->getByName( UNISTRING( CHECK_INTERVAL ) ) >>= nValue;

    return nValue;
}

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckConfig::getLocalFileName() const
{
    rtl::OUString aName = UNISTRING(LOCAL_FILE);
    rtl::OUString aRet;

    if( m_xContainer->hasByName(aName) )
        m_xContainer->getByName(aName) >>= aRet;

    return aRet;
}

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckConfig::getDownloadDestination() const
{
    rtl::OUString aName = UNISTRING(DOWNLOAD_DESTINATION);
    rtl::OUString aRet;

    const_cast <UpdateCheckConfig *> (this)->getByName(aName) >>= aRet;

    return aRet;
}

//------------------------------------------------------------------------------

void
UpdateCheckConfig::storeLocalFileName(const rtl::OUString& rLocalFileName, sal_Int64 nFileSize)
{
    const sal_uInt8 nItems = 2;
    const rtl::OUString aNameList[nItems] = { UNISTRING(LOCAL_FILE), UNISTRING(DOWNLOAD_SIZE) };
    const uno::Any aValueList[nItems] = { uno::makeAny(rLocalFileName), uno::makeAny(nFileSize) };

    for( sal_uInt8 i=0; i < nItems; ++i )
    {
        if( m_xContainer->hasByName(aNameList[i]) )
            m_xContainer->replaceByName(aNameList[i], aValueList[i]);
        else
            m_xContainer->insertByName(aNameList[i],aValueList[i]);
    }

    commitChanges();
}

//------------------------------------------------------------------------------

void
UpdateCheckConfig::clearLocalFileName()
{
    const sal_uInt8 nItems = 2;
    const rtl::OUString aNameList[nItems] = { UNISTRING(LOCAL_FILE), UNISTRING(DOWNLOAD_SIZE) };

    for( sal_uInt8 i=0; i < nItems; ++i )
    {
        if( m_xContainer->hasByName(aNameList[i]) )
            m_xContainer->removeByName(aNameList[i]);
    }

    commitChanges();
}

//------------------------------------------------------------------------------

void
UpdateCheckConfig::storeDownloadPaused(bool paused)
{
    replaceByName(UNISTRING(DOWNLOAD_PAUSED) , uno::makeAny(paused));
    commitChanges();
}

//------------------------------------------------------------------------------

void
UpdateCheckConfig::updateLastChecked()
{
    TimeValue systime;
    osl_getSystemTime(&systime);

    sal_Int64 lastCheck = systime.Seconds;

    replaceByName(UNISTRING(LAST_CHECK), uno::makeAny(lastCheck));
}

//------------------------------------------------------------------------------

void
UpdateCheckConfig::storeUpdateFound( const UpdateInfo& rInfo, const rtl::OUString& aCurrentBuild)

{
    bool autoDownloadEnabled = isAutoDownloadEnabled();

    uno::Any aValues[nUpdateEntryProperties] =
    {
        uno::makeAny(rInfo.Version),
        uno::makeAny(rInfo.BuildId),
        uno::makeAny(rInfo.Description),
        uno::makeAny(rInfo.Sources[0].URL),
        uno::makeAny(rInfo.Sources[0].IsDirect ? sal_True : sal_False),
        uno::makeAny(getReleaseNote(rInfo, 1, autoDownloadEnabled) ),
        uno::makeAny(getReleaseNote(rInfo, 2, autoDownloadEnabled) ),
        uno::makeAny(getReleaseNote(rInfo, 3, autoDownloadEnabled) ),
        uno::makeAny(getReleaseNote(rInfo, 4, autoDownloadEnabled) ),
        uno::makeAny(getReleaseNote(rInfo, 5, autoDownloadEnabled) ),
        uno::makeAny(aCurrentBuild)
    };

    rtl::OUString aName;
    for( sal_uInt32 n=0; n < nUpdateEntryProperties; ++n )
    {
        aName = rtl::OUString::createFromAscii(aUpdateEntryProperties[n]);

        if( m_xContainer->hasByName(aName) )
            m_xContainer->replaceByName(aName, aValues[n]);
        else
            m_xContainer->insertByName(aName,aValues[n]);
    }

    commitChanges();
}

//------------------------------------------------------------------------------

void
UpdateCheckConfig::clearUpdateFound()
{
    rtl::OUString aName;

    for( sal_uInt32 n=0; n < nUpdateEntryProperties; ++n )
    {
        aName = rtl::OUString::createFromAscii(aUpdateEntryProperties[n]);

        try {
            if( m_xContainer->hasByName(aName) )
                m_xContainer->removeByName(aName);
        } catch(const lang::WrappedTargetException& ) {
            // Can not remove value, probably in share layer
            OSL_ASSERT(false);
            m_xContainer->replaceByName(aName, uno::makeAny(rtl::OUString()));
        }
    }

    /* As we have removed UpdateVersionFound from the shared configuration
     * existing entries in the user layer do not have a oor operation and
     * thus are completly ignored (which also means they can not be removed).
     */

    commitChanges();
}

//------------------------------------------------------------------------------

uno::Sequence< rtl::OUString >
UpdateCheckConfig::getServiceNames()
{
    uno::Sequence< rtl::OUString > aServiceList(1);
    aServiceList[0] = UNISTRING( "com.sun.star.setup.UpdateCheckConfig");
    return aServiceList;
}

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckConfig::getImplName()
{
    return UNISTRING( "vnd.sun.UpdateCheckConfig");
}

//------------------------------------------------------------------------------

uno::Type SAL_CALL
UpdateCheckConfig::getElementType() throw (uno::RuntimeException)
{
    return m_xContainer->getElementType();
}

//------------------------------------------------------------------------------

sal_Bool SAL_CALL
UpdateCheckConfig::hasElements() throw (uno::RuntimeException)
{
    return m_xContainer->hasElements();
}

//------------------------------------------------------------------------------

uno::Any SAL_CALL
UpdateCheckConfig::getByName( const ::rtl::OUString& aName )
    throw (container::NoSuchElementException, lang::WrappedTargetException,  uno::RuntimeException)
{
    uno::Any aValue = m_xContainer->getByName( aName );

    // Provide dynamic default value
    if( aName.equalsAscii(DOWNLOAD_DESTINATION) )
    {
        rtl::OUString aStr;
        aValue >>= aStr;

        if( aStr.getLength() == 0 )
            aValue = uno::makeAny(getDesktopDirectory());
    }

    return aValue;
}

//------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > SAL_CALL
UpdateCheckConfig::getElementNames(  ) throw (uno::RuntimeException)
{
    return m_xContainer->getElementNames();
}

//------------------------------------------------------------------------------

sal_Bool SAL_CALL
UpdateCheckConfig::hasByName( const ::rtl::OUString& aName ) throw (uno::RuntimeException)
{
    return m_xContainer->hasByName( aName );
}

//------------------------------------------------------------------------------

void SAL_CALL
UpdateCheckConfig::replaceByName( const ::rtl::OUString& aName, const uno::Any& aElement )
    throw (lang::IllegalArgumentException, container::NoSuchElementException,
           lang::WrappedTargetException, uno::RuntimeException)
{
    return m_xContainer->replaceByName( aName, aElement );
}

//------------------------------------------------------------------------------
// XChangesBatch

void SAL_CALL
UpdateCheckConfig::commitChanges()
    throw (lang::WrappedTargetException, uno::RuntimeException)
{
    uno::Reference< util::XChangesBatch > xChangesBatch(m_xContainer, uno::UNO_QUERY);
    if( xChangesBatch.is() && xChangesBatch->hasPendingChanges() )
    {
        util::ChangesSet aChangesSet = xChangesBatch->getPendingChanges();
        xChangesBatch->commitChanges();

        if( m_rListener.is() )
        {
            const sal_Int32 nChanges = aChangesSet.getLength();
            rtl::OUString aString;

            for( sal_Int32 i=0; i<nChanges; ++i )
            {
                aChangesSet[i].Accessor >>= aString;

                // FIXME: use non IgnoreAsciiCase version as soon as it becomes available
                if( aString.endsWithIgnoreAsciiCaseAsciiL(AUTOCHECK_ENABLED "']", sizeof(AUTOCHECK_ENABLED)+1) )
                {
                    sal_Bool bEnabled;
                    aChangesSet[i].Element >>= bEnabled;
                    m_rListener->autoCheckStatusChanged(sal_True == bEnabled);
                }
                // FIXME: use non IgnoreAsciiCase version as soon as it becomes available
                else if( aString.endsWithIgnoreAsciiCaseAsciiL(CHECK_INTERVAL "']", sizeof(CHECK_INTERVAL)+1) )
                {
                    m_rListener->autoCheckIntervalChanged();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

sal_Bool SAL_CALL
UpdateCheckConfig::hasPendingChanges(  ) throw (uno::RuntimeException)
{
    uno::Reference< util::XChangesBatch > xChangesBatch(m_xContainer, uno::UNO_QUERY);
    if( xChangesBatch.is() )
        return xChangesBatch->hasPendingChanges();

    return sal_False;
}

//------------------------------------------------------------------------------

uno::Sequence< util::ElementChange > SAL_CALL
UpdateCheckConfig::getPendingChanges(  ) throw (uno::RuntimeException)
{
    uno::Reference< util::XChangesBatch > xChangesBatch(m_xContainer, uno::UNO_QUERY);
    if( xChangesBatch.is() )
        return xChangesBatch->getPendingChanges();

    return uno::Sequence< util::ElementChange >();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

rtl::OUString SAL_CALL
UpdateCheckConfig::getImplementationName()  throw (uno::RuntimeException)
{
    return getImplName();
}

//------------------------------------------------------------------------------

sal_Bool SAL_CALL
UpdateCheckConfig::supportsService(rtl::OUString const & serviceName)
    throw (uno::RuntimeException)
{
    uno::Sequence< rtl::OUString > aServiceNameList = getServiceNames();

    for( sal_Int32 n=0; n < aServiceNameList.getLength(); n++ )
        if( aServiceNameList[n].equals(serviceName) )
            return sal_True;

    return sal_False;
}

//------------------------------------------------------------------------------

uno::Sequence< rtl::OUString > SAL_CALL
UpdateCheckConfig::getSupportedServiceNames()  throw (uno::RuntimeException)
{
    return getServiceNames();
}

