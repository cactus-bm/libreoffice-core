/*************************************************************************
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
#include "precompiled_desktop.hxx"

#include "dp_ucb.h"
#include "dp_resource.h"
#include "dp_platform.hxx"
#include "dp_manager.h"
#include "dp_identifier.hxx"
#include "rtl/ustrbuf.hxx"
#include "rtl/string.hxx"
#include "rtl/uri.hxx"
#include "rtl/bootstrap.hxx"
#include "osl/diagnose.h"
#include "osl/file.hxx"
#include "osl/security.hxx"
#include "cppuhelper/weakref.hxx"
#include "cppuhelper/exc_hlp.hxx"
#include "cppuhelper/implbase1.hxx"
#include "cppuhelper/interfacecontainer.hxx"
#include "comphelper/servicedecl.hxx"
#include "comphelper/sequence.hxx"
#include "xmlscript/xml_helper.hxx"
#include "svl/inettype.hxx"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/lang/WrappedTargetRuntimeException.hpp"
#include "com/sun/star/beans/UnknownPropertyException.hpp"
#include "com/sun/star/util/XUpdatable.hpp"
#include "com/sun/star/sdbc/XResultSet.hpp"
#include "com/sun/star/sdbc/XRow.hpp"
#include "com/sun/star/ucb/XContentAccess.hpp"
#include "com/sun/star/ucb/NameClash.hpp"
#include "com/sun/star/deployment/VersionException.hpp"
#include "com/sun/star/deployment/InstallException.hpp"
#include "com/sun/star/task/XInteractionApprove.hpp"
#include "com/sun/star/ucb/UnsupportedCommandException.hpp"
#include "boost/bind.hpp"
#include "tools/urlobj.hxx"

#include "osl/file.hxx"
#include <vector>
#include <list>
#include "dp_descriptioninfoset.hxx"


using namespace ::dp_misc;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using ::rtl::OUString;

namespace dp_log {
extern comphelper::service_decl::ServiceDecl const serviceDecl;
}

namespace dp_registry {
Reference<deployment::XPackageRegistry> create(
    OUString const & context,
    OUString const & cachePath, bool readOnly,
    Reference<XComponentContext> const & xComponentContext );
}

namespace dp_manager {

struct MatchTempDir
{
    OUString m_str;
    MatchTempDir( OUString const & str ) : m_str( str ) {}
    bool operator () ( ActivePackages::Entries::value_type const & v ) const {
        return v.second.temporaryName.equalsIgnoreAsciiCase( m_str );
    }
};

struct MatchExtension
{
    const ActivePackages::Data m_data;
    MatchExtension(ActivePackages::Data const & data ) :  m_data(data) {}
    bool operator () ( OUString const & temporaryName ) const;
};

bool MatchExtension::operator () (OUString const & temporaryName) const
{
    //case 1: The temporary file and thus the extension folder are already
    //removed.
    return m_data.temporaryName.equals(temporaryName);
}

namespace {
OUString getExtensionFolder(OUString const &  parentFolder,
                            Reference<ucb::XCommandEnvironment> const & xCmdEnv)
{
    ::ucbhelper::Content tempFolder(
        parentFolder, xCmdEnv );
    Reference<sdbc::XResultSet> xResultSet(
        tempFolder.createCursor(
            Sequence<OUString>( &StrTitle::get(), 1 ),
            ::ucbhelper::INCLUDE_FOLDERS_ONLY ) );

    OUString title;
    while (xResultSet->next())
    {
        title = Reference<sdbc::XRow>(
            xResultSet, UNO_QUERY_THROW )->getString(1 /* Title */ ) ;
        break;
    }
    return title;
}
/* adds an unencoded segment to the URL.

   Throws an com.sun.star.uno.Exception if this failed.
*/
OUString appendURLSegement(OUString const & baseURL, OUString const & segment)
{
    OUString url;
    INetURLObject inet(baseURL);
    if (inet.insertName(
            segment, false, INetURLObject::LAST_SEGMENT, true,
            INetURLObject::ENCODE_ALL))
        url = inet.GetMainURL(INetURLObject::NO_DECODE);
    else
        throw Exception(
            OUSTR("ExtensionManager: failed to add segment to URL"), 0);
    return url;
}
}
//______________________________________________________________________________
void PackageManagerImpl::initActivationLayer(
    Reference<XCommandEnvironment> const & xCmdEnv )
{
    if (m_activePackages.getLength() == 0)
    {
        OSL_ASSERT( m_registryCache.getLength() == 0 );
        // documents temp activation:
        m_activePackagesDB.reset( new ActivePackages );
        ::ucbhelper::Content ucbContent;
        if (create_ucb_content( &ucbContent, m_context, xCmdEnv,
                                false /* no throw */ ))
        {
            // scan for all entries in m_packagesDir:
            Reference<sdbc::XResultSet> xResultSet(
                ucbContent.createCursor(
                    Sequence<OUString>( &StrTitle::get(), 1 ),
                    ::ucbhelper::INCLUDE_FOLDERS_AND_DOCUMENTS ) );
            while (xResultSet->next())
            {
                Reference<sdbc::XRow> xRow( xResultSet, UNO_QUERY_THROW );
                OUString title( xRow->getString( 1 /* Title */ ) );
                // xxx todo: remove workaround for tdoc
                if (title.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                                            "this_is_a_dummy_stream_just_there_"
                                            "as_a_workaround_for_a_"
                                            "temporary_limitation_of_the_"
                                            "storage_api_implementation") ))
                    continue;
                if (title.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(
                                             "META-INF") ) )
                    continue;

                ::ucbhelper::Content sourceContent(
                    Reference<XContentAccess>(
                        xResultSet, UNO_QUERY_THROW )->queryContent(),
                    xCmdEnv );

                OUString mediaType( detectMediaType( sourceContent,
                                                     false /* no throw */) );
                if (mediaType.getLength() >0)
                {
                    ActivePackages::Data dbData;
                    insertToActivationLayer(
                        mediaType, sourceContent, title, &dbData );

                    insertToActivationLayerDB( title, dbData );
                        //TODO #i73136#: insertToActivationLayerDB needs id not
                        // title, but the whole m_activePackages.getLength()==0
                        // case (i.e., document-relative deployment) currently
                        // does not work, anyway.
                }
            }
        }
    }
    else
    {
        // user|share:
        OSL_ASSERT( m_activePackages.getLength() > 0 );
        m_activePackages_expanded = expandUnoRcUrl( m_activePackages );
        m_registrationData_expanded = expandUnoRcUrl(m_registrationData);
        create_folder( 0, m_activePackages_expanded, xCmdEnv, true);

        OUString dbName;
        if (m_context.equals(OUSTR("user")))
            dbName = m_activePackages_expanded + OUSTR(".db");
        else
        {
            //Create the extension data base in the user installation
            create_folder( 0, m_registrationData_expanded, xCmdEnv, true);
            dbName = m_registrationData_expanded + OUSTR("/extensions.db");
        }
        //The data base can always be written because it it always in the user installation
        m_activePackagesDB.reset(
            new ActivePackages( dbName, false ) );

        if (! m_readOnly && ! m_context.equals(OUSTR("bundled")))
        {
            // clean up activation layer, scan for zombie temp dirs:
            ActivePackages::Entries id2temp( m_activePackagesDB->getEntries() );

            ::ucbhelper::Content tempFolder(
                m_activePackages_expanded, xCmdEnv );
            Reference<sdbc::XResultSet> xResultSet(
                tempFolder.createCursor(
                    Sequence<OUString>( &StrTitle::get(), 1 ),
                    ::ucbhelper::INCLUDE_DOCUMENTS_ONLY ) );
            // get all temp directories:
            ::std::vector<OUString> tempEntries;
            ::std::vector<OUString> removedEntries;
            while (xResultSet->next())
            {
                OUString title(
                    Reference<sdbc::XRow>(
                        xResultSet, UNO_QUERY_THROW )->getString(
                            1 /* Title */ ) );

                const char extensionRemoved[] = "removed";
                if (title.endsWithAsciiL(
                        extensionRemoved, sizeof(extensionRemoved) - 1))
                {
                    //save the file name withouth the "removed" part
                    sal_Int32 index = title.lastIndexOfAsciiL(
                        extensionRemoved, sizeof(extensionRemoved) - 1);
                    OUString remFile = title.copy(0, index);
                    removedEntries.push_back(::rtl::Uri::encode(
                                                remFile, rtl_UriCharClassPchar,
                                                rtl_UriEncodeIgnoreEscapes,
                                                RTL_TEXTENCODING_UTF8 ) );
                }
                else
                {
                    tempEntries.push_back( ::rtl::Uri::encode(
                                               title, rtl_UriCharClassPchar,
                                               rtl_UriEncodeIgnoreEscapes,
                                               RTL_TEXTENCODING_UTF8 ) );
                }
            }

            bool bShared = m_context.equals(OUSTR("shared")) ? true : false;
            for ( ::std::size_t pos = 0; pos < tempEntries.size(); ++pos )
            {
                OUString const & tempEntry = tempEntries[ pos ];
                const MatchTempDir match( tempEntry );
                if (::std::find_if( id2temp.begin(), id2temp.end(), match ) ==
                    id2temp.end())
                {
                    const OUString url(
                        makeURL(m_activePackages_expanded, tempEntry ) );

                    //In case of shared extensions, new entries are regarded as
                    //added extensions if there is no xxx.tmpremoved file.
                    if (bShared)
                    {
                        if (::std::find(removedEntries.begin(), removedEntries.end(), tempEntry) ==
                            removedEntries.end())
                        {
                            continue;
                        }
                        else
                        {
                            //Make sure only the same user removes the extension, who
                            //previously unregistered it. This is avoid races if multiple instances
                            //of OOo are running which all have write access to the shared installation.
                            //For example, user a uses extension a, removes the extension, but keeps OOo
                            //running. Parts of the extension may still be loaded and used by OOo.
                            //Therefore the extension is only deleted the next time the extension manager is
                            //run after restarting OOo. While OOo is still running, another user starts OOo
                            //which would deleted the extension files. If the same user starts another
                            //instance of OOo then the lock file will prevent this.
                            OUString aUserName;
                            ::osl::Security aSecurity;
                            aSecurity.getUserName( aUserName );
                            ucbhelper::Content remFileContent(
                                url + OUSTR("removed"), Reference<XCommandEnvironment>());
                            ::rtl::ByteSequence data = dp_misc::readFile(remFileContent);
                            ::rtl::OString osData(reinterpret_cast<const sal_Char*>(data.getConstArray()),
                                                  data.getLength());
                            OUString sData = ::rtl::OStringToOUString(
                                osData, RTL_TEXTENCODING_UTF8);
                            if (!sData.equals(aUserName))
                                continue;
                        }
                    }
                    // temp entry not needed anymore:
                    erase_path( url + OUSTR("_"),
                                Reference<XCommandEnvironment>(),
                                false /* no throw: ignore errors */ );
                    erase_path( url, Reference<XCommandEnvironment>(),
                                false /* no throw: ignore errors */ );
                    //delete the xxx.tmpremoved file
                    erase_path(url + OUSTR("removed"),
                               Reference<XCommandEnvironment>(), false);
                }
            }
        }
    }
}

//______________________________________________________________________________
void PackageManagerImpl::initRegistryBackends()
{
    if (m_registryCache.getLength() > 0)
        create_folder( 0, m_registryCache,
                       Reference<XCommandEnvironment>(), false);
    m_xRegistry.set( ::dp_registry::create(
                         m_context, m_registryCache, false,
                         m_xComponentContext ) );
}

//______________________________________________________________________________
Reference<deployment::XPackageManager> PackageManagerImpl::create(
    Reference<XComponentContext> const & xComponentContext,
    OUString const & context )
{
    PackageManagerImpl * that = new PackageManagerImpl(
        xComponentContext, context );
    Reference<deployment::XPackageManager> xPackageManager( that );

    OUString packages, logFile, stampURL;
    if (context.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("user") )) {
        that->m_activePackages = OUSTR(
            "vnd.sun.star.expand:$UNO_USER_PACKAGES_CACHE/uno_packages");
        that->m_registrationData = OUSTR(
            "vnd.sun.star.expand:$UNO_USER_PACKAGES_CACHE");
        that->m_registryCache = OUSTR(
            "vnd.sun.star.expand:$UNO_USER_PACKAGES_CACHE/registry");
        logFile = OUSTR(
            "vnd.sun.star.expand:$UNO_USER_PACKAGES_CACHE/log.txt");
        //We use the extension .sys for the file because on Windows Vista a sys
        //(as well as exe and dll) file
        //will not be written in the VirtualStore. For example if the process has no
        //admin right once cannot write to the %programfiles% folder. However, when
        //virtualization is used, the file will be written into the VirtualStore and
        //it appears as if one could write to %programfiles%. When we test for write
        //access to the office/shared folder for shared extensions then this typically
        //fails because a normal user typically cannot write to this folder. However,
        //using virtualization it appears that he/she can. Then a shared extension can
        //be installed but is only visible for the user (because the extension is in
        //the virtual store).
        stampURL = OUSTR(
            "vnd.sun.star.expand:$UNO_USER_PACKAGES_CACHE/stamp.sys");
    }
    else if (context.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("shared") )) {
        that->m_activePackages = OUSTR(
            "vnd.sun.star.expand:$UNO_SHARED_PACKAGES_CACHE/uno_packages");
        that->m_registrationData = OUSTR(
            "vnd.sun.star.expand:$SHARED_EXTENSIONS_USER");
        that->m_registryCache = OUSTR(
            "vnd.sun.star.expand:$SHARED_EXTENSIONS_USER/registry");
        logFile = OUSTR(
            "vnd.sun.star.expand:$SHARED_EXTENSIONS_USER/log.txt");
        stampURL = OUSTR(
            "vnd.sun.star.expand:$UNO_SHARED_PACKAGES_CACHE/stamp.sys");
    }
    else if (context.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("bundled") )) {
        that->m_activePackages = OUSTR(
            "vnd.sun.star.expand:$BUNDLED_EXTENSIONS");
        that->m_registrationData = OUSTR(
            "vnd.sun.star.expand:$BUNDLED_EXTENSIONS_USER");
        that->m_registryCache = OUSTR(
            "vnd.sun.star.expand:$BUNDLED_EXTENSIONS_USER/registry");
        logFile = OUSTR(
            "vnd.sun.star.expand:$BUNDLED_EXTENSIONS_USER/log.txt");
        //No stamp file. We assume that bundled is always readonly. It must not be
        //modified from ExtensionManager but only by the installer
    }
    else if (context.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("tmp") )) {
        that->m_activePackages = OUSTR(
            "vnd.sun.star.expand:$TMP_EXTENSIONS/extensions");
        that->m_registrationData = OUSTR(
            "vnd.sun.star.expand:$TMP_EXTENSIONS");
        that->m_registryCache = OUSTR(
            "vnd.sun.star.expand:$TMP_EXTENSIONS/registry");
        stampURL = OUSTR(
            "vnd.sun.star.expand:$TMP_EXTENSIONS/stamp.sys");
    }
    else if (! context.matchAsciiL(
                 RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.tdoc:/") )) {
        throw lang::IllegalArgumentException(
            OUSTR("invalid context given: ") + context,
            Reference<XInterface>(), static_cast<sal_Int16>(-1) );
    }

    Reference<XCommandEnvironment> xCmdEnv;

    try {
        //There is no stampURL for the bundled folder
        if (stampURL.getLength() > 0)
        {
#define CURRENT_STAMP "1"
            try {
                //The osl file API does not allow to find out if one can write
                //into a folder. Therefore we try to write a file. Then we delete
                //it, so that it does not hinder uninstallation of OOo
                // probe writing:
                ::ucbhelper::Content ucbStamp( stampURL, xCmdEnv );
                ::rtl::OString stamp(
                    RTL_CONSTASCII_STRINGPARAM(CURRENT_STAMP) );
                Reference<io::XInputStream> xData(
                    ::xmlscript::createInputStream(
                        ::rtl::ByteSequence(
                            reinterpret_cast<sal_Int8 const *>(stamp.getStr()),
                            stamp.getLength() ) ) );
                ucbStamp.writeStream( xData, true /* replace existing */ );
                that->m_readOnly = false;
                erase_path( stampURL, xCmdEnv );
            }
            catch (RuntimeException &) {
                try {
                    erase_path( stampURL, xCmdEnv );
                } catch (...)
                {
                }
                throw;
            }
            catch (Exception &) {
                that->m_readOnly = true;
            }
        }

        if (!that->m_readOnly && logFile.getLength() > 0)
        {
            const Any any_logFile(logFile);
            that->m_xLogFile.set(
                that->m_xComponentContext->getServiceManager()
                ->createInstanceWithArgumentsAndContext(
                    dp_log::serviceDecl.getSupportedServiceNames()[0],
                    Sequence<Any>( &any_logFile, 1 ),
                    that->m_xComponentContext ),
                UNO_QUERY_THROW );
            xCmdEnv.set( new CmdEnvWrapperImpl( xCmdEnv, that->m_xLogFile ) );
        }

        that->initRegistryBackends();
        that->initActivationLayer( xCmdEnv );

        return xPackageManager;

    }
    catch (RuntimeException &) {
        throw;
    }
    catch (Exception &) {
        Any exc( ::cppu::getCaughtException() );
        ::rtl::OUStringBuffer buf;
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("[context=\"") );
        buf.append( context );
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(
                             "\"] caught unexpected exception!") );
        throw lang::WrappedTargetRuntimeException(
            buf.makeStringAndClear(), Reference<XInterface>(), exc );
    }
}

//______________________________________________________________________________
PackageManagerImpl::~PackageManagerImpl()
{
}

//______________________________________________________________________________
void PackageManagerImpl::fireModified()
{
    ::cppu::OInterfaceContainerHelper * pContainer = rBHelper.getContainer(
        util::XModifyListener::static_type() );
    if (pContainer != 0) {
        pContainer->forEach<util::XModifyListener>(
            boost::bind(&util::XModifyListener::modified, _1,
                        lang::EventObject(static_cast<OWeakObject *>(this))) );
    }
}

//______________________________________________________________________________
void PackageManagerImpl::disposing()
{
    try {
//     // xxx todo: guarding?
//     ::osl::MutexGuard guard( getMutex() );
        try_dispose( m_xLogFile );
        m_xLogFile.clear();
        try_dispose( m_xRegistry );
        m_xRegistry.clear();
        m_activePackagesDB.reset(0);
        m_xComponentContext.clear();

        t_pm_helper::disposing();

    }
    catch (RuntimeException &) {
        throw;
    }
    catch (Exception &) {
        Any exc( ::cppu::getCaughtException() );
        throw lang::WrappedTargetRuntimeException(
            OUSTR("caught unexpected exception while disposing..."),
            static_cast<OWeakObject *>(this), exc );
    }
}

// XComponent
//______________________________________________________________________________
void PackageManagerImpl::dispose() throw (RuntimeException)
{
    check();
    WeakComponentImplHelperBase::dispose();
}

//______________________________________________________________________________
void PackageManagerImpl::addEventListener(
    Reference<lang::XEventListener> const & xListener ) throw (RuntimeException)
{
    check();
    WeakComponentImplHelperBase::addEventListener( xListener );
}

//______________________________________________________________________________
void PackageManagerImpl::removeEventListener(
    Reference<lang::XEventListener> const & xListener ) throw (RuntimeException)
{
    check();
    WeakComponentImplHelperBase::removeEventListener( xListener );
}

// XPackageManager
//______________________________________________________________________________
OUString PackageManagerImpl::getContext() throw (RuntimeException)
{
    check();
    return m_context;
}

//______________________________________________________________________________
Sequence< Reference<deployment::XPackageTypeInfo> >
PackageManagerImpl::getSupportedPackageTypes() throw (RuntimeException)
{
    OSL_ASSERT( m_xRegistry.is() );
    return m_xRegistry->getSupportedPackageTypes();
}

//______________________________________________________________________________
Reference<task::XAbortChannel> PackageManagerImpl::createAbortChannel()
    throw (RuntimeException)
{
    check();
    return new AbortChannel;
}

// XModifyBroadcaster
//______________________________________________________________________________
void PackageManagerImpl::addModifyListener(
    Reference<util::XModifyListener> const & xListener )
    throw (RuntimeException)
{
    check();
    rBHelper.addListener( ::getCppuType( &xListener ), xListener );
}

//______________________________________________________________________________
void PackageManagerImpl::removeModifyListener(
    Reference<util::XModifyListener> const & xListener )
    throw (RuntimeException)
{
    check();
    rBHelper.removeListener( ::getCppuType( &xListener ), xListener );
}

//______________________________________________________________________________
OUString PackageManagerImpl::detectMediaType(
    ::ucbhelper::Content const & ucbContent_, bool throw_exc )
{
    ::ucbhelper::Content ucbContent(ucbContent_);
    OUString url( ucbContent.getURL() );
    OUString mediaType;
    if (url.matchAsciiL( RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.tdoc:") ) ||
        url.matchAsciiL( RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.pkg:") ))
    {
        try {
            ucbContent.getPropertyValue( OUSTR("MediaType") ) >>= mediaType;
        }
        catch (beans::UnknownPropertyException &) {
        }
        OSL_ENSURE( mediaType.getLength() > 0, "### no media-type?!" );
    }
    if (mediaType.getLength() == 0)
    {
        try {
            Reference<deployment::XPackage> xPackage(
                m_xRegistry->bindPackage(
                    url, OUString(), false, OUString(), ucbContent.getCommandEnvironment() ) );
            const Reference<deployment::XPackageTypeInfo> xPackageType(
                xPackage->getPackageType() );
            OSL_ASSERT( xPackageType.is() );
            if (xPackageType.is())
                mediaType = xPackageType->getMediaType();
        }
        catch (lang::IllegalArgumentException & exc) {
            if (throw_exc)
                throw;
            (void) exc;
            OSL_ENSURE( 0, ::rtl::OUStringToOString(
                            exc.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
        }
    }
    return mediaType;
}

//______________________________________________________________________________
OUString PackageManagerImpl::insertToActivationLayer(
    OUString const & mediaType, ::ucbhelper::Content const & sourceContent_,
    OUString const & title, ActivePackages::Data * dbData )
{
    ::ucbhelper::Content sourceContent(sourceContent_);
    Reference<XCommandEnvironment> xCmdEnv(
        sourceContent.getCommandEnvironment() );
    OUString destFolder, tempEntry;
    if (::osl::File::createTempFile(
            m_activePackages_expanded.getLength() == 0
            ? 0 : &m_activePackages_expanded,
            0, &tempEntry ) != ::osl::File::E_None)
        throw RuntimeException(
            OUSTR("::osl::File::createTempFile() failed!"), 0 );
    if (m_activePackages_expanded.getLength() == 0) {
        destFolder = tempEntry;
    }
    else {
        tempEntry = tempEntry.copy( tempEntry.lastIndexOf( '/' ) + 1 );
        // tweak user|share to macrofied url:
        destFolder = makeURL( m_activePackages, tempEntry );
    }
    destFolder += OUSTR("_");

    // prepare activation folder:
    ::ucbhelper::Content destFolderContent;
    create_folder( &destFolderContent, destFolder, xCmdEnv );

    // copy content into activation temp dir:
    if (mediaType.matchIgnoreAsciiCaseAsciiL(
            RTL_CONSTASCII_STRINGPARAM(
                "application/vnd.sun.star.package-bundle") ) ||
        // xxx todo: more sophisticated parsing
        mediaType.matchIgnoreAsciiCaseAsciiL(
            RTL_CONSTASCII_STRINGPARAM(
                "application/vnd.sun.star.legacy-package-bundle") ))
    {
        // inflate content:
        ::rtl::OUStringBuffer buf;
        if (!sourceContent.isFolder())
        {
            buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("vnd.sun.star.zip://") );
            buf.append( ::rtl::Uri::encode( sourceContent.getURL(),
                                            rtl_UriCharClassRegName,
                                            rtl_UriEncodeIgnoreEscapes,
                                            RTL_TEXTENCODING_UTF8 ) );
        }
        else
        {
            //Folder. No need to unzip, just copy
            buf.append(sourceContent.getURL());
        }
        buf.append( static_cast<sal_Unicode>('/') );
        sourceContent = ::ucbhelper::Content(
            buf.makeStringAndClear(), xCmdEnv );
    }
    if (! destFolderContent.transferContent(
            sourceContent, ::ucbhelper::InsertOperation_COPY,
            title, NameClash::OVERWRITE ))
        throw RuntimeException( OUSTR("UCB transferContent() failed!"), 0 );


    // write to DB:
    //bundled extensions should only be added by the synchronizeAddedExtensions
    //functions. Moreover, there is no "temporary folder" for bundled extensions.
    OSL_ASSERT(!m_context.equals(OUSTR("bundled")));
    DescriptionInfoset info =
        dp_misc::getDescriptionInfoset(
            appendURLSegement(destFolderContent.getURL(), title));
    dbData->temporaryName = tempEntry;
    dbData->fileName = title;
    dbData->mediaType = mediaType;
    dbData->version = info.getVersion();
    return destFolder;
}

//______________________________________________________________________________
void PackageManagerImpl::insertToActivationLayerDB(
    OUString const & id, ActivePackages::Data const & dbData )
{
    m_activePackagesDB->put( id, dbData );
}

//______________________________________________________________________________
/* The function returns true if there is an extension with the same id already
    installed which needs to be uninstalled, before the new extension can be installed.
*/
bool PackageManagerImpl::isInstalled(
    Reference<deployment::XPackage> const & package)
{
    OUString id(dp_misc::getIdentifier(package));
    OUString fn(package->getName());
    bool bInstalled = false;
    if (m_activePackagesDB->has( id, fn ))
    {
        bInstalled = true;
    }
    return bInstalled;
}

// XPackageManager
//______________________________________________________________________________
Reference<deployment::XPackage> PackageManagerImpl::importExtension(
    Reference<deployment::XPackage> const & extension,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<XCommandEnvironment> const & xCmdEnv_ )
    throw (deployment::DeploymentException, CommandFailedException,
           CommandAbortedException, lang::IllegalArgumentException,
           RuntimeException)
{
    return addPackage(extension->getURL(), OUString(), xAbortChannel, xCmdEnv_);
}

/* The function adds an extension but does not register it!!!
    It may not do any user interaction. This is done in XExtensionManager::addExtension
*/
Reference<deployment::XPackage> PackageManagerImpl::addPackage(
    OUString const & url, OUString const & mediaType_,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<XCommandEnvironment> const & xCmdEnv_ )
    throw (deployment::DeploymentException, CommandFailedException,
           CommandAbortedException, lang::IllegalArgumentException,
           RuntimeException)
{
    check();
    if (m_readOnly)
    {
        OUString message;
        if (m_context == OUSTR("shared"))
            message = OUSTR("You need write permissions to install a shared extension!");
        else
            message = OUSTR("You need write permissions to install this extension!");
        throw deployment::DeploymentException(
            message, static_cast<OWeakObject *>(this), Any() );
    }
    Reference<XCommandEnvironment> xCmdEnv;
    if (m_xLogFile.is())
        xCmdEnv.set( new CmdEnvWrapperImpl( xCmdEnv_, m_xLogFile ) );
    else
        xCmdEnv.set( xCmdEnv_ );

    try {
        ::ucbhelper::Content sourceContent;
        create_ucb_content( &sourceContent, url, xCmdEnv ); // throws exc
        const OUString title(sourceContent.getPropertyValue(
                             StrTitle::get() ).get<OUString>() );
        const OUString title_enc( ::rtl::Uri::encode(
                                      title, rtl_UriCharClassPchar,
                                      rtl_UriEncodeIgnoreEscapes,
                                      RTL_TEXTENCODING_UTF8 ) );
        OUString destFolder;

        OUString mediaType(mediaType_);
        if (mediaType.getLength() == 0)
            mediaType = detectMediaType( sourceContent );

        Reference<deployment::XPackage> xPackage;
        // copy file:
        progressUpdate(
            getResourceString(RID_STR_COPYING_PACKAGE) + title, xCmdEnv );
        if (m_activePackages.getLength() == 0)
        {
            ::ucbhelper::Content docFolderContent;
            create_folder( &docFolderContent, m_context, xCmdEnv );
            // copy into document, first:
            if (! docFolderContent.transferContent(
                    sourceContent, ::ucbhelper::InsertOperation_COPY,
                    OUString(),
                    NameClash::ASK /* xxx todo: ASK not needed? */))
                throw RuntimeException(
                    OUSTR("UCB transferContent() failed!"), 0 );
            // set media-type:
            ::ucbhelper::Content docContent(
                makeURL( m_context, title_enc ), xCmdEnv );
                //TODO #i73136#: using title instead of id can lead to
                // clashes, but the whole m_activePackages.getLength()==0
                // case (i.e., document-relative deployment) currently does
                // not work, anyway.
            docContent.setPropertyValue(
                OUSTR("MediaType"), Any(mediaType) );

            // xxx todo: obsolete in the future
            try {
                docFolderContent.executeCommand( OUSTR("flush"), Any() );
            }
            catch (UnsupportedCommandException &) {
            }
        }
        ActivePackages::Data dbData;
        destFolder = insertToActivationLayer(
            mediaType, sourceContent, title, &dbData );


        // bind activation package:
        //Because every shared/user extension will be unpacked in a folder,
        //which was created with a unique name we will always have two different
        //XPackage objects, even if the second extension is the same.
        //Therefore bindPackage does not need a guard here.
        xPackage = m_xRegistry->bindPackage(
            makeURL( destFolder, title_enc ), mediaType, false, OUString(), xCmdEnv );

        OSL_ASSERT( xPackage.is() );
        if (xPackage.is())
        {
            bool install = false;
            try
            {
                OUString const id = dp_misc::getIdentifier( xPackage );
                //This guard is used to prevent that an extension is
                //installed twice. Do not use it in other functions.
                //Imagine addPackage is called two times by different
                //threads for the same extension quickly after each other.
                //The second call would calculate "bAlreadyInstalled =
                //false" if the first thread has not yet reached
                //insertToActivationLayerDB.
                ::osl::MutexGuard g(m_addMutex);
                if (isInstalled(xPackage))
                {
                    //Do not guard the complete function with the getMutex
                    removePackage(id, xPackage->getName(), xAbortChannel,
                                  xCmdEnv);
                }
                install = true;
                const ::osl::MutexGuard guard( getMutex() );
                //access to the database must be guarded. See removePackage_
                insertToActivationLayerDB(id, dbData);

            }
            catch (...)
            {
                deletePackageFromCache( xPackage, destFolder );
                throw;
            }
            if (!install)
            {
                deletePackageFromCache( xPackage, destFolder );
            }
            //ToDo: We should notify only if the extension is registered
            fireModified();
        }
        return xPackage;
    }
    catch (RuntimeException &) {
        throw;
    }
    catch (CommandFailedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (CommandAbortedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (deployment::DeploymentException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (Exception &) {
        Any exc( ::cppu::getCaughtException() );
        logIntern( exc );
        throw deployment::DeploymentException(
            getResourceString(RID_STR_ERROR_WHILE_ADDING) + url,
            static_cast<OWeakObject *>(this), exc );
    }
}
void PackageManagerImpl::deletePackageFromCache(
    Reference<deployment::XPackage> const & xPackage,
    OUString const & destFolder)
{
    try_dispose( xPackage );

    //we remove the package from the uno cache
    //no service from the package may be loaded at this time!!!
    erase_path( destFolder, Reference<XCommandEnvironment>(),
        false /* no throw: ignore errors */ );
    //rm last character '_'
    OUString url = destFolder.copy(0, destFolder.getLength() - 1);
    erase_path( url, Reference<XCommandEnvironment>(),
        false /* no throw: ignore errors */ );

}
//______________________________________________________________________________
void PackageManagerImpl::removePackage(
    OUString const & id, ::rtl::OUString const & fileName,
    Reference<task::XAbortChannel> const & /*xAbortChannel*/,
    Reference<XCommandEnvironment> const & xCmdEnv_ )
    throw (deployment::DeploymentException, CommandFailedException,
           CommandAbortedException, lang::IllegalArgumentException,
           RuntimeException)
{
    check();

    Reference<XCommandEnvironment> xCmdEnv;
    if (m_xLogFile.is())
        xCmdEnv.set( new CmdEnvWrapperImpl( xCmdEnv_, m_xLogFile ) );
    else
        xCmdEnv.set( xCmdEnv_ );

    try {
        Reference<deployment::XPackage> xPackage;
        {
            const ::osl::MutexGuard guard(getMutex());
            //Check if this extension exist and throw an IllegalArgumentException
            //if it does not
            //If the files of the extension are already removed, or there is a
            //different extension at the same place, for example after updating the
            //extension, then the returned object is that which uses the database data.
            xPackage = getDeployedPackage_(id, fileName, xCmdEnv );


            //Because the extension is only removed the next time the extension
            //manager runs after restarting OOo, we need to indicate that a
            //shared extension was "deleted". When a user starts OOo, then it
            //will check if something changed in the shared repository. Based on
            //the flag file it will then recognize, that the extension was
            //deleted and can then update the extnesion database of the shared
            //extensions in the user installation.
            if (! m_readOnly && !xPackage->isRemoved() && m_context.equals(OUSTR("shared")))
            {
                ActivePackages::Data val;
                m_activePackagesDB->get( & val, id, fileName);
                OSL_ASSERT(val.temporaryName.getLength());
                OUString url(makeURL(m_activePackages_expanded,
                                     val.temporaryName + OUSTR("removed")));
                ::ucbhelper::Content contentRemoved(url, xCmdEnv );
                OUString aUserName;
                ::osl::Security aSecurity;
                aSecurity.getUserName( aUserName );

                ::rtl::OString stamp = ::rtl::OUStringToOString(aUserName, RTL_TEXTENCODING_UTF8);
                Reference<css::io::XInputStream> xData(
                    ::xmlscript::createInputStream(
                        ::rtl::ByteSequence(
                            reinterpret_cast<sal_Int8 const *>(stamp.getStr()),
                            stamp.getLength() ) ) );
                contentRemoved.writeStream( xData, true /* replace existing */ );
            }
            m_activePackagesDB->erase( id, fileName ); // to be removed upon next start
        }
        try_dispose( xPackage );

        fireModified();
    }
    catch (RuntimeException &) {
        throw;
    }
    catch (lang::IllegalArgumentException &) {
        throw;
    }
    catch (CommandFailedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (CommandAbortedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (deployment::DeploymentException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (Exception &) {
        Any exc( ::cppu::getCaughtException() );
        logIntern( exc );
        throw deployment::DeploymentException(
            getResourceString(RID_STR_ERROR_WHILE_REMOVING) + id,
            static_cast<OWeakObject *>(this), exc );
    }
}

//______________________________________________________________________________
OUString PackageManagerImpl::getDeployPath( ActivePackages::Data const & data )
{
    ::rtl::OUStringBuffer buf;
    buf.append( data.temporaryName );
    //The bundled extensions are not contained in an additional folder
    //with a unique name. data.temporaryName contains already the
    //UTF8 encoded folder name. See PackageManagerImpl::synchronize
    if (!m_context.equals(OUSTR("bundled")))
    {
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("_/") );
        buf.append( ::rtl::Uri::encode( data.fileName, rtl_UriCharClassPchar,
                                    rtl_UriEncodeIgnoreEscapes,
                                    RTL_TEXTENCODING_UTF8 ) );
    }
    return makeURL( m_activePackages, buf.makeStringAndClear() );
}

//______________________________________________________________________________
Reference<deployment::XPackage> PackageManagerImpl::getDeployedPackage_(
    OUString const & id, OUString const & fileName,
    Reference<XCommandEnvironment> const & xCmdEnv )
{
    ActivePackages::Data val;
    if (m_activePackagesDB->get( &val, id, fileName ))
    {
        return getDeployedPackage_( id, val, xCmdEnv, false );
    }
    throw lang::IllegalArgumentException(
        getResourceString(RID_STR_NO_SUCH_PACKAGE) + id,
        static_cast<OWeakObject *>(this), static_cast<sal_Int16>(-1) );
}

//______________________________________________________________________________
Reference<deployment::XPackage> PackageManagerImpl::getDeployedPackage_(
    OUString const & id, ActivePackages::Data const & data,
    Reference<XCommandEnvironment> const & xCmdEnv, bool ignoreAlienPlatforms )
{
    if (ignoreAlienPlatforms)
    {
        String type, subType;
        INetContentTypeParameterList params;
        if (INetContentTypes::parse( data.mediaType, type, subType, &params ))
        {
            INetContentTypeParameter const * param = params.find(
                ByteString("platform") );
            if (param != 0 && !platform_fits( param->m_sValue ))
                throw lang::IllegalArgumentException(
                    getResourceString(RID_STR_NO_SUCH_PACKAGE) + id,
                    static_cast<OWeakObject *>(this),
                    static_cast<sal_Int16>(-1) );
        }
    }
    Reference<deployment::XPackage> xExtension;
    try
    {
        xExtension = m_xRegistry->bindPackage(
            getDeployPath( data ), data.mediaType, false, OUString(), xCmdEnv );
    }
    catch (deployment::InvalidRemovedParameterException& e)
    {
        xExtension = e.Extension;
    }
    return xExtension;
}

//______________________________________________________________________________
Sequence< Reference<deployment::XPackage> >
PackageManagerImpl::getDeployedPackages_(
    Reference<XCommandEnvironment> const & xCmdEnv )
{
    ::std::vector< Reference<deployment::XPackage> > packages;
    ActivePackages::Entries id2temp( m_activePackagesDB->getEntries() );
    ActivePackages::Entries::const_iterator iPos( id2temp.begin() );
    ActivePackages::Entries::const_iterator const iEnd( id2temp.end() );
    for ( ; iPos != iEnd; ++iPos )
    {
        try {
            packages.push_back(
                getDeployedPackage_(
                    iPos->first, iPos->second, xCmdEnv,
                    true /* xxx todo: think of GUI:
                            ignore other platforms than the current one */ ) );
        }
        catch (lang::IllegalArgumentException & exc) {
            // ignore
            (void) exc; // avoid warnings
            OSL_ENSURE( 0, ::rtl::OUStringToOString(
                            exc.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
        }
        catch (deployment::DeploymentException& exc) {
            // ignore
            (void) exc; // avoid warnings
            OSL_ENSURE( 0, ::rtl::OUStringToOString(
                            exc.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
        }
    }
    return comphelper::containerToSequence(packages);
}

//______________________________________________________________________________
Reference<deployment::XPackage> PackageManagerImpl::getDeployedPackage(
    OUString const & id, ::rtl::OUString const & fileName,
    Reference<XCommandEnvironment> const & xCmdEnv_ )
    throw (deployment::DeploymentException, CommandFailedException,
           lang::IllegalArgumentException, RuntimeException)
{
    check();
    Reference<XCommandEnvironment> xCmdEnv;
    if (m_xLogFile.is())
        xCmdEnv.set( new CmdEnvWrapperImpl( xCmdEnv_, m_xLogFile ) );
    else
        xCmdEnv.set( xCmdEnv_ );

    try {
        const ::osl::MutexGuard guard( getMutex() );
        return getDeployedPackage_( id, fileName, xCmdEnv );
    }
    catch (RuntimeException &) {
        throw;
    }
    catch (CommandFailedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (lang::IllegalArgumentException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (deployment::DeploymentException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (Exception &) {
        Any exc( ::cppu::getCaughtException() );
        logIntern( exc );
        throw deployment::DeploymentException(
            // ought never occur...
            OUSTR("error while accessing deployed package: ") + id,
            static_cast<OWeakObject *>(this), exc );
    }
}

//______________________________________________________________________________
Sequence< Reference<deployment::XPackage> >
PackageManagerImpl::getDeployedPackages(
    Reference<task::XAbortChannel> const &,
    Reference<XCommandEnvironment> const & xCmdEnv_ )
    throw (deployment::DeploymentException, CommandFailedException,
           CommandAbortedException, lang::IllegalArgumentException,
           RuntimeException)
{
    check();
    Reference<XCommandEnvironment> xCmdEnv;
    if (m_xLogFile.is())
        xCmdEnv.set( new CmdEnvWrapperImpl( xCmdEnv_, m_xLogFile ) );
    else
        xCmdEnv.set( xCmdEnv_ );

    try {
        const ::osl::MutexGuard guard( getMutex() );
        return getDeployedPackages_( xCmdEnv );
    }
    catch (RuntimeException &) {
        throw;
    }
    catch (CommandFailedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (CommandAbortedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (deployment::DeploymentException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (Exception &) {
        Any exc( ::cppu::getCaughtException() );
        logIntern( exc );
        throw deployment::DeploymentException(
            // ought never occur...
            OUSTR("error while getting all deployed packages: ") + m_context,
            static_cast<OWeakObject *>(this), exc );
    }
}

//______________________________________________________________________________


//ToDo: the function must not call registerPackage, do this in
//XExtensionManager.reinstallDeployedExtensions
void PackageManagerImpl::reinstallDeployedPackages(
    Reference<task::XAbortChannel> const &  /*xAbortChannel*/,
    Reference<XCommandEnvironment> const & xCmdEnv_ )
    throw (deployment::DeploymentException,
           CommandFailedException, CommandAbortedException,
           lang::IllegalArgumentException, RuntimeException)
{
    check();
    if (office_is_running())
        throw RuntimeException(
            OUSTR("You must close any running Office process before "
                  "reinstalling packages!"), static_cast<OWeakObject *>(this) );

    Reference<XCommandEnvironment> xCmdEnv;
    if (m_xLogFile.is())
        xCmdEnv.set( new CmdEnvWrapperImpl( xCmdEnv_, m_xLogFile ) );
    else
        xCmdEnv.set( xCmdEnv_ );

    try {
        ProgressLevel progress(
            xCmdEnv, OUSTR("Reinstalling all deployed packages...") );

        try_dispose( m_xRegistry );
        m_xRegistry.clear();
        if (m_registryCache.getLength() > 0)
            erase_path( m_registryCache, xCmdEnv );
        initRegistryBackends();
        Reference<util::XUpdatable> xUpdatable( m_xRegistry, UNO_QUERY );
        if (xUpdatable.is())
            xUpdatable->update();

        //registering is done by the ExtensionManager service.
    }
    catch (RuntimeException &) {
        throw;
    }
    catch (CommandFailedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (CommandAbortedException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (deployment::DeploymentException & exc) {
        logIntern( Any(exc) );
        throw;
    }
    catch (Exception &) {
        Any exc( ::cppu::getCaughtException() );
        logIntern( exc );
        throw deployment::DeploymentException(
            OUSTR("Error while reinstalling all previously deployed "
                  "packages of context ") + m_context,
            static_cast<OWeakObject *>(this), exc );
    }
}


::sal_Bool SAL_CALL PackageManagerImpl::isReadOnly(  )
        throw (::com::sun::star::uno::RuntimeException)
{
    return m_readOnly;
}
void PackageManagerImpl::synchronizeRemovedExtensions(
    Sequence<Reference<deployment::XPackage> > & out_removedExtensions,
        Reference<task::XAbortChannel> const & /*xAbortChannel*/,
    Reference<css::ucb::XCommandEnvironment> const & xCmdEnv)
{

    //find all which are in the extension data base but which
    //are removed already.
    OSL_ASSERT(!m_context.equals(OUSTR("user")));
    ActivePackages::Entries id2temp( m_activePackagesDB->getEntries() );

    //Iterate over the contents of the extension folder and gather the
    //temp file names (shared) or the folder names of the bundled extension.
    ::ucbhelper::ResultSetInclude includeType = ::ucbhelper::INCLUDE_DOCUMENTS_ONLY;
    if (m_context.equals(OUSTR("bundled")))
        includeType = ::ucbhelper::INCLUDE_FOLDERS_ONLY;
    ::ucbhelper::Content tempFolder(
        m_activePackages_expanded, xCmdEnv );
    Reference<sdbc::XResultSet> xResultSet(
        tempFolder.createCursor(
            Sequence<OUString>( &StrTitle::get(), 1 ), includeType) );
    // get all temp directories:
    ::std::vector<OUString> tempEntries;
    while (xResultSet->next()) {
        OUString title(
            Reference<sdbc::XRow>(
                xResultSet, UNO_QUERY_THROW )->getString(
                    1 /* Title */ ) );
        //also add the xxx.tmpremoved files for remove shared extensions
        //this does not matter
        tempEntries.push_back( ::rtl::Uri::encode(
                                   title, rtl_UriCharClassPchar,
                                   rtl_UriEncodeIgnoreEscapes,
                                   RTL_TEXTENCODING_UTF8 ) );
    }

    typedef ActivePackages::Entries::const_iterator ITActive;
    bool bShared = m_context.equals(OUSTR("shared"));
    ::std::vector<Reference<deployment::XPackage> > removedExtensions;
    for (ITActive i = id2temp.begin(); i != id2temp.end(); i++)
    {
        //Get the URL to the extensions folder, first make the url for the
        //shared repository including the temporary name
//         OUString url(m_activePackages_expanded + OUSTR("/")
//                      + i->second.temporaryName);
//         if (bShared)
//             url = appendURLSegement(m_activePackages_expanded + OUSTR("/")
//                                     + i->second.temporaryName + OUSTR("_"),
//                                     i->second.fileName);
        OUString url = makeURL(m_activePackages, i->second.temporaryName);
        if (bShared)
            url = makeURLAppendSysPathSegment( url + OUSTR("_"), i->second.fileName);

        const MatchExtension  match(i->second);
        bool bRemoved = false;
        if (::std::find_if(tempEntries.begin(), tempEntries.end(), match) ==
            tempEntries.end())
        {
            //The the URL from the data base entry does not exist anymore. That is the
            //folder was removed.
            bRemoved = true;
        }
        else
        {
            //The folder is in the extension database, but it can still be deleted.
            //look for the xxx.tmpremoved file
            if (bShared)
            {
                ::ucbhelper::Content contentRemoved;
                if (create_ucb_content(
                        &contentRemoved,
                        m_activePackages_expanded + OUSTR("/") +
                        i->second.temporaryName + OUSTR("removed"),
                        Reference<XCommandEnvironment>(), false))
                {
                    bRemoved = true;
                }
            }
            if (!bRemoved)
            {
                //There may be another extensions at the same place
                dp_misc::DescriptionInfoset infoset =
                    dp_misc::getDescriptionInfoset(url);
                OSL_ENSURE(infoset.hasDescription(),
                           "Extension Manager: bundled and shared extensions "
                           "must have an identifer and a version");
                if (infoset.hasDescription() &&
                    infoset.getIdentifier() &&
                    (! i->first.equals(*(infoset.getIdentifier()))
                   || ! i->second.version.equals(infoset.getVersion())))
                {
                    bRemoved = true;
                }
            }
        }
        if (bRemoved)
        {
            Reference<deployment::XPackage> xPackage = m_xRegistry->bindPackage(
                url, i->second.mediaType, true, i->first, xCmdEnv );
            OSL_ASSERT(xPackage.is()); //Even if the files are removed, we must get the object.
            removedExtensions.push_back(xPackage);
        }
    }
    out_removedExtensions = ::comphelper::containerToSequence(removedExtensions);
}

void PackageManagerImpl::synchronizeAddedExtensions(
    Sequence<Reference<deployment::XPackage> > & out_addedExtensions,
        Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<css::ucb::XCommandEnvironment> const & xCmdEnv)
{
       // clean up activation layer, scan for zombie temp dirs:
    ActivePackages::Entries id2temp( m_activePackagesDB->getEntries() );

    ::ucbhelper::Content tempFolder(
        m_activePackages_expanded, xCmdEnv );
    Reference<sdbc::XResultSet> xResultSet(
        tempFolder.createCursor(
            Sequence<OUString>( &StrTitle::get(), 1 ),
            ::ucbhelper::INCLUDE_FOLDERS_ONLY ) );


    ::std::vector<Reference<deployment::XPackage> > addedExtensions;
    while (xResultSet->next())
    {
        OUString title(
            Reference<sdbc::XRow>(
                xResultSet, UNO_QUERY_THROW )->getString(
                    1 /* Title */ ) );
        //The temporary folders of user and shared have an '_' at then end.
        //But the name in ActivePackages.temporaryName is saved without.
        OUString title2 = title;
        bool bNotBundled = !m_context.equals(OUSTR("bundled"));
        if (bNotBundled)
        {
            OSL_ASSERT(title2[title2.getLength() -1] == '_');
            title2 = title2.copy(0, title2.getLength() -1);
        }
        OUString titleEncoded =  ::rtl::Uri::encode(
                title2, rtl_UriCharClassPchar,
                rtl_UriEncodeIgnoreEscapes,
                RTL_TEXTENCODING_UTF8);

        const MatchTempDir match(titleEncoded);
        if (::std::find_if( id2temp.begin(), id2temp.end(), match ) ==
            id2temp.end())
        {

            // The folder was not found in the data base, so it must be
            // an added extension
            OUString url(m_activePackages_expanded + OUSTR("/") + titleEncoded);
            OUString sExtFolder;
            if (bNotBundled) //that is, shared
            {
                //Check if the extension was not "deleted" already which is indicated
                //by a xxx.tmpremoved file
                ::ucbhelper::Content contentRemoved;
                if (create_ucb_content(&contentRemoved, url + OUSTR("removed"),
                                       Reference<XCommandEnvironment>(), false))
                    continue;
                sExtFolder = getExtensionFolder(
                    m_activePackages_expanded +
                        OUString(OUSTR("/")) + titleEncoded + OUSTR("_"), xCmdEnv);
                url = appendURLSegement(m_activePackages_expanded, title);
                url = appendURLSegement(url, sExtFolder);
            }
            Reference<deployment::XPackage> xPackage = m_xRegistry->bindPackage(
                url, OUString(), false, OUString(), xCmdEnv );
            if (xPackage.is())
            {
                try
                {
                    //ToDo: We need to prevent that removed shared extensions are
                    //added again. This can happen if there is still the folder of the
                    //extension. However, there is the "removed" flag file which indicates
                    //that the extension was removed.
                    //Prepare the database entry
                    ActivePackages::Data dbData;
                    //There is no temporary folder for bundled extensions. It is therefore
                    //an empty string.
                    dbData.temporaryName = titleEncoded;
                    if (bNotBundled)
                        dbData.fileName = sExtFolder;
                    else
                        dbData.fileName = title;
                    dbData.mediaType = xPackage->getPackageType()->getMediaType();
                    dbData.version = xPackage->getVersion();
                    OSL_ENSURE(dbData.version.getLength() > 0,
                               "Extension Manager: bundled and shared extensions must have "
                               "an identifier and a version");

                    OUString id = dp_misc::getIdentifier( xPackage );
                    sal_Bool bAlreadyInstalled = sal_False;
                    if (xPackage->checkPrerequisites(
                            xAbortChannel, xCmdEnv, bAlreadyInstalled, m_context))
                    {
                        const ::osl::MutexGuard guard( getMutex() );
                        //access to the database must be guarded. See removePackage_
                        insertToActivationLayerDB(id, dbData);
                    }
                    else
                    {
                        //ToDo: Remember that this failed. For example, the user
                        //could have declined the license. Then the next time the
                        //extension folder is investigated we do not want to
                        //try to install the extension again.
                    }
                    addedExtensions.push_back(xPackage);
                }
                catch (...)
                {
                }
            }
        }
    }
    out_addedExtensions = ::comphelper::containerToSequence(addedExtensions);
}

void PackageManagerImpl::synchronize(
    Sequence<Reference<deployment::XPackage> > & out_addedExtensions,
    Sequence<Reference<deployment::XPackage> > & out_removedExtensions,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<css::ucb::XCommandEnvironment> const & xCmdEnv)
    throw (css::deployment::DeploymentException,
           css::ucb::CommandFailedException,
           css::ucb::CommandAbortedException,
           css::uno::RuntimeException)
{

    check();
    if (m_context.equals(OUSTR("user")))
        return;
    synchronizeRemovedExtensions(
        out_removedExtensions, xAbortChannel, xCmdEnv);
    synchronizeAddedExtensions(
        out_addedExtensions, xAbortChannel, xCmdEnv);
}


//##############################################################################

//______________________________________________________________________________
PackageManagerImpl::CmdEnvWrapperImpl::~CmdEnvWrapperImpl()
{
}

//______________________________________________________________________________
PackageManagerImpl::CmdEnvWrapperImpl::CmdEnvWrapperImpl(
    Reference<XCommandEnvironment> const & xUserCmdEnv,
    Reference<XProgressHandler> const & xLogFile )
    : m_xLogFile( xLogFile )
{
    if (xUserCmdEnv.is()) {
        m_xUserProgress.set( xUserCmdEnv->getProgressHandler() );
        m_xUserInteractionHandler.set( xUserCmdEnv->getInteractionHandler() );
    }
}

// XCommandEnvironment
//______________________________________________________________________________
Reference<task::XInteractionHandler>
PackageManagerImpl::CmdEnvWrapperImpl::getInteractionHandler()
    throw (RuntimeException)
{
    return m_xUserInteractionHandler;
}

//______________________________________________________________________________
Reference<XProgressHandler>
PackageManagerImpl::CmdEnvWrapperImpl::getProgressHandler()
    throw (RuntimeException)
{
    return this;
}

// XProgressHandler
//______________________________________________________________________________
void PackageManagerImpl::CmdEnvWrapperImpl::push( Any const & Status )
    throw (RuntimeException)
{
    if (m_xLogFile.is())
        m_xLogFile->push( Status );
    if (m_xUserProgress.is())
        m_xUserProgress->push( Status );
}

//______________________________________________________________________________
void PackageManagerImpl::CmdEnvWrapperImpl::update( Any const & Status )
    throw (RuntimeException)
{
    if (m_xLogFile.is())
        m_xLogFile->update( Status );
    if (m_xUserProgress.is())
        m_xUserProgress->update( Status );
}

//______________________________________________________________________________
void PackageManagerImpl::CmdEnvWrapperImpl::pop() throw (RuntimeException)
{
    if (m_xLogFile.is())
        m_xLogFile->pop();
    if (m_xUserProgress.is())
        m_xUserProgress->pop();
}

} // namespace dp_manager

