/*************************************************************************
 *
 *  $RCSfile: app.cxx,v $
 *
 *  $Revision: 1.175 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-18 14:43:08 $
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

#include <unistd.h>
#include "app.hxx"
#include "desktop.hrc"
#include "appinit.hxx"
#include "intro.hxx"
#include "officeipcthread.hxx"
#include "cmdlineargs.hxx"
#include "desktopresid.hxx"
#include "dispatchwatcher.hxx"
#include "configinit.hxx"
#include "lockfile.hxx"
#include "checkinstall.hxx"
#include "cmdlinehelp.hxx"
#include "userinstall.hxx"
#include "desktopcontext.hxx"
#include "exithelper.hxx"
#include "javainteractionhandler.hxx"

#ifndef _COM_SUN_STAR_FRAME_XSESSIONMANAGERLISTENER_HPP_
#include <com/sun/star/frame/XSessionManagerListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSYNCHRONOUSDISPATCH_HPP_
#include <com/sun/star/frame/XSynchronousDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_CORRUPTEDFILTERCONFIGURATION_HPP_
#include <com/sun/star/document/CorruptedFilterConfigurationException.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMODIFIABLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XFLUSHABLE_HPP_
#include <com/sun/star/util/XFlushable.hpp>
#endif
#ifndef _COM_SUN_STAR_SYSTEM_XSYSTEMSHELLEXECUTE_HPP_
#include <com/sun/star/system/XSystemShellExecute.hpp>
#endif
#ifndef _COM_SUN_STAR_SYSTEM_SYSTEMSHELLEXECUTEFLAGS_HPP_
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_IOEXCEPTION_HPP_
#include <com/sun/star/io/IOException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_WRAPPEDTARGETEXCEPTION_HPP_
#include <com/sun/star/lang/WrappedTargetException.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_VIEW_XPRINTABLE_HPP_
#include <com/sun/star/view/XPrintable.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAMESSUPPLIER_HPP_
#include <com/sun/star/frame/XFramesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTOPWINDOW_HPP_
#include <com/sun/star/awt/XTopWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_SERVICENOTREGISTEREDEXCEPTION_HPP_
#include <com/sun/star/lang/ServiceNotRegisteredException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_MISSINGBOOTSTRAPFILEEXCEPTION_HPP_
#include <com/sun/star/configuration/MissingBootstrapFileException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_INVALIDBOOTSTRAPFILEEXCEPTION_HPP_
#include <com/sun/star/configuration/InvalidBootstrapFileException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_INSTALLATIONINCOMPLETEEXCEPTION_HPP_
#include <com/sun/star/configuration/InstallationIncompleteException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_BACKENDSETUPEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/BackendSetupException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_BACKENDACCESSEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/BackendAccessException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XENUMERATION_HPP_
#include <com/sun/star/container/XEnumeration.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XEXECUTABLEDIALOG_HPP_
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_EXECUTABLEDIALOGRESULTS_HPP_
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XJOBEXECUTOR_HPP_
#include <com/sun/star/task/XJobExecutor.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XJOBEXECUTOR_HPP_
#include <com/sun/star/task/XJob.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_NAMEDVALUE_HPP_
#include <com/sun/star/beans/NamedValue.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XJOB_HPP_
#include <com/sun/star/task/XJob.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTLISTENER_HPP_
#include <com/sun/star/document/XEventListener.hpp>
#endif

#include <com/sun/star/java/XJavaVM.hpp>

#ifndef _TOOLS_TESTTOOLLOADER_HXX_
#include <tools/testtoolloader.hxx>
#endif
#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif
#ifndef _VOS_SECURITY_HXX_
#include <vos/security.hxx>
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _VOS_PROCESS_HXX_
#include <vos/process.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COMPHELPER_CONFIGURATIONHELPER_HXX_
#include <comphelper/configurationhelper.hxx>
#endif
#ifndef _UTL__HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _UNOTOOLS_CONFIGNODE_HXX_
#include <unotools/confignode.hxx>
#endif
#ifndef _UNOTOOLS_UCBHELPER_HXX
#include <unotools/ucbhelper.hxx>
#endif
#ifndef _TOOLS_TEMPFILE_HXX
#include <tools/tempfile.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif
#ifndef _OSL_MODULE_H_
#include <osl/module.h>
#endif
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _SVTOOLS_LANGUAGEOPTIONS_HXX
#include <svtools/languageoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_INTERNALOPTIONS_HXX
#include <svtools/internaloptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MISCOPT_HXX
#include <svtools/miscopt.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_SYSLOCALEOPTIONS_HXX
#include <svtools/syslocaleoptions.hxx>
#endif
#ifndef SVTOOLS_FOLDER_RESTRICTION_HXX
#include <svtools/folderrestriction.hxx>
#endif
#ifndef _UNOTOOLS_TEMPFILE_HXX
#include <unotools/tempfile.hxx>
#endif

#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _RTL_STRBUF_HXX_
#include <rtl/strbuf.hxx>
#endif
#ifndef _RTL_BOOTSTRAP_HXX_
#include <rtl/bootstrap.hxx>
#endif
#ifndef INCLUDED_RTL_INSTANCE_HXX
#include <rtl/instance.hxx>
#endif
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _SV_HELP_HXX
#include <vcl/help.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_BITMAP_HXX
#include <vcl/bitmap.hxx>
#endif
#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFX_HRC
#include <sfx2/sfx.hrc>
#endif
#ifndef _UCBHELPER_CONTENTBROKER_HXX
#include <ucbhelper/contentbroker.hxx>
#endif
#ifndef _UTL_BOOTSTRAP_HXX
#include <unotools/bootstrap.hxx>
#endif

#include <svtools/fontsubstconfig.hxx>
#include <svtools/accessibilityoptions.hxx>
#include <svtools/apearcfg.hxx>
#include <svtools/misccfg.hxx>
#include <svtools/filter.hxx>

#include "langselect.hxx"

#include <stdio.h>

#define DEFINE_CONST_UNICODE(CONSTASCII)        UniString(RTL_CONSTASCII_USTRINGPARAM(CONSTASCII))
#define U2S(STRING)                                ::rtl::OUStringToOString(STRING, RTL_TEXTENCODING_UTF8)

using namespace vos;
using namespace rtl;

//Gives an ICE with MSVC6
//namespace css = ::com::sun::star;

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
//using namespace ::com::sun::star::bridge;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::view;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::system;
using namespace ::com::sun::star::ui::dialogs;
using namespace ::com::sun::star::container;

namespace css = ::com::sun::star;

ResMgr*                    desktop::Desktop::pResMgr = 0;
sal_Bool                desktop::Desktop::bSuppressOpenDefault = sal_False;

namespace desktop
{

static SalMainPipeExchangeSignalHandler* pSignalHandler = 0;

// ----------------------------------------------------------------------------

char const INSTALLMODE_STANDALONE[] = "STANDALONE";
char const INSTALLMODE_NETWORK[]    = "NETWORK";

// ----------------------------------------------------------------------------

ResMgr* Desktop::GetDesktopResManager()
{
    if ( !Desktop::pResMgr )
    {
        String aMgrName = String::CreateFromAscii( "dkt" );
        aMgrName += String::CreateFromInt32(SUPD); // current version number

        // Create desktop resource manager and bootstrap process
        // was successful. Use default way to get language specific message.
        if ( Application::IsInExecute() )
            Desktop::pResMgr = ResMgr::CreateResMgr( U2S( aMgrName ));

        if ( !Desktop::pResMgr )
        {
            // Use VCL to get the correct language specific message as we
            // are in the bootstrap process and not able to get the installed
            // language!!
/*
            LanguageType aLanguageType = LANGUAGE_DONTKNOW;

            Desktop::pResMgr = ResMgr::SearchCreateResMgr( U2S( aMgrName ), aLanguageType );
            AllSettings as = GetSettings();
            as.SetUILanguage(aLanguageType);
            SetSettings(as);
*/
            // LanguageSelection langselect;
            OUString aUILocaleString = LanguageSelection::getLanguageString();
            sal_Int32 nIndex = 0;
            OUString aLanguage = aUILocaleString.getToken( 0, '-', nIndex);
            OUString aCountry = aUILocaleString.getToken( 0, '-', nIndex);
            OUString aVariant = aUILocaleString.getToken( 0, '-', nIndex);

            ::com::sun::star::lang::Locale aLocale( aLanguage, aCountry, aVariant );

            Desktop::pResMgr = ResMgr::SearchCreateResMgr( U2S( aMgrName ), aLocale);
            AllSettings as = GetSettings();
            as.SetUILocale(aLocale);
            SetSettings(as);
        }
    }

    return Desktop::pResMgr;
}

// ----------------------------------------------------------------------------
// Get a message string securely. There is a fallback string if the resource
// is not available.

OUString Desktop::GetMsgString( USHORT nId, const OUString& aFaultBackMsg )
{
    ResMgr* pResMgr = GetDesktopResManager();
    if ( !pResMgr )
        return aFaultBackMsg;
    else
        return OUString( String( ResId( nId, pResMgr )));
}

OUString MakeStartupErrorMessage(OUString const & aErrorMessage)
{
    OUStringBuffer    aDiagnosticMessage( 100 );

    ResMgr* pResMgr = Desktop::GetDesktopResManager();
    if ( pResMgr )
        aDiagnosticMessage.append( OUString(String(ResId(STR_BOOTSTRAP_ERR_CANNOT_START, pResMgr))) );
    else
        aDiagnosticMessage.appendAscii( "The program cannot be started." );

    aDiagnosticMessage.appendAscii( "\n" );

    aDiagnosticMessage.append( aErrorMessage );

    return aDiagnosticMessage.makeStringAndClear();
}

OUString MakeStartupConfigAccessErrorMessage( OUString const & aInternalErrMsg )
{
    OUStringBuffer aDiagnosticMessage( 200 );

    ResMgr* pResMgr = Desktop::GetDesktopResManager();
    if ( pResMgr )
        aDiagnosticMessage.append( OUString(String(ResId(STR_BOOTSTRAP_ERR_CFG_DATAACCESS, pResMgr ))) );
    else
        aDiagnosticMessage.appendAscii( "The program cannot be started." );

    if ( aInternalErrMsg.getLength() > 0 )
    {
        aDiagnosticMessage.appendAscii( "\n\n" );
        if ( pResMgr )
            aDiagnosticMessage.append( OUString(String(ResId(STR_INTERNAL_ERRMSG, pResMgr ))) );
        else
            aDiagnosticMessage.appendAscii( "The following internal error has occured:\n\n" );
        aDiagnosticMessage.append( aInternalErrMsg );
    }

    return aDiagnosticMessage.makeStringAndClear();
}

void FatalErrorExit(OUString const & aMessage)
{
    OUString aProductKey = ::utl::Bootstrap::getProductKey();

    if (!aProductKey.getLength())
    {
        ::vos::OStartupInfo aInfo;
        aInfo.getExecutableFile( aProductKey );

        sal_uInt32     lastIndex = aProductKey.lastIndexOf('/');
        if ( lastIndex > 0 )
            aProductKey = aProductKey.copy( lastIndex+1 );
    }

    ErrorBox aBootstrapFailedBox( NULL, WB_OK, aMessage );
    aBootstrapFailedBox.SetText( aProductKey );
    aBootstrapFailedBox.Execute();

    _exit( ExitHelper::E_FATAL_ERROR );
}

void FatalError(OUString const & aMessage)
{
    OUString aProductKey = ::utl::Bootstrap::getProductKey();

    if (!aProductKey.getLength())
    {
        ::vos::OStartupInfo aInfo;
        aInfo.getExecutableFile( aProductKey );

        sal_uInt32     lastIndex = aProductKey.lastIndexOf('/');
        if ( lastIndex > 0 )
            aProductKey = aProductKey.copy( lastIndex+1 );
    }

    ErrorBox aBootstrapFailedBox( NULL, WB_OK, aMessage );
    aBootstrapFailedBox.SetText( aProductKey );
    aBootstrapFailedBox.Execute();
}

static bool ShouldSuppressUI(CommandLineArgs* pCmdLine)
{
    return  pCmdLine->IsInvisible() ||
            pCmdLine->IsHeadless() ||
            pCmdLine->IsQuickstart();
}

CommandLineArgs* Desktop::GetCommandLineArgs()
{
    static CommandLineArgs* pArgs = 0;
    if ( !pArgs )
    {
        ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
        if ( !pArgs )
    {
            ::vos::OExtCommandLine aArgs;
            pArgs = new CommandLineArgs( aArgs );
    }
    }

    return pArgs;
}

sal_Bool InitConfiguration()
{
    RTL_LOGFILE_CONTEXT( aLog, "desktop (jb99855) ::InitConfiguration" );

    Reference< XMultiServiceFactory > xProvider( CreateApplicationConfigurationProvider( ) );
    return xProvider.is();
}

namespace
{
    struct BrandName
        : public rtl::Static< String, BrandName > {};
    struct Version
        : public rtl::Static< String, Version > {};
    struct Extension
        : public rtl::Static< String, Extension > {};
    struct XMLFileFormatVersion
        : public rtl::Static< String, XMLFileFormatVersion > {};
    struct WriterCompatibilityVersionOOo11
        : public rtl::Static< String, WriterCompatibilityVersionOOo11 > {};
}

void ReplaceStringHookProc( UniString& rStr )
{
    static int nAll = 0, nPro = 0;

    String &rBrandName = BrandName::get();
    String &rVersion = Version::get();
    String &rExtension = Extension::get();
    String &rXMLFileFormatVersion = XMLFileFormatVersion::get();
    String &rWriterCompatibilityVersionOOo11 = WriterCompatibilityVersionOOo11::get();
    if ( !rBrandName.Len() )
    {
        Any aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTNAME );
        rtl::OUString aTmp;
        aRet >>= aTmp;
        rBrandName = aTmp;

        aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTXMLFILEFORMATVERSION );
        aRet >>= aTmp;
        rXMLFileFormatVersion = aTmp;

        aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTVERSION );
        aRet >>= aTmp;
        rVersion = aTmp;

        if ( !rExtension.Len() )
        {
            aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTEXTENSION );
            aRet >>= aTmp;
            rExtension = aTmp;
        }

        if ( !rWriterCompatibilityVersionOOo11.Len() )
        {
            aRet = ::utl::ConfigManager::GetDirectConfigProperty(
                ::utl::ConfigManager::WRITERCOMPATIBILITYVERSIONOOO11 );
            aRet >>= aTmp;
            rWriterCompatibilityVersionOOo11 = aTmp;
}
    }

    nAll++;
    if ( rStr.SearchAscii( "%PRODUCT" ) != STRING_NOTFOUND )
    {
        nPro++;
        rStr.SearchAndReplaceAllAscii( "%PRODUCTNAME", rBrandName );
        rStr.SearchAndReplaceAllAscii( "%PRODUCTVERSION", rVersion );
        rStr.SearchAndReplaceAllAscii( "%PRODUCTEXTENSION", rExtension );
        rStr.SearchAndReplaceAllAscii( "%PRODUCTXMLFILEFORMATVERSION", rXMLFileFormatVersion );
    }

    if ( rStr.SearchAscii( "%WRITERCOMPATIBILITYVERSIONOOO11" ) != STRING_NOTFOUND )
        rStr.SearchAndReplaceAllAscii( "%WRITERCOMPATIBILITYVERSIONOOO11",
                                       rWriterCompatibilityVersionOOo11 );
}

Desktop::Desktop()
: m_pIntro( 0 )
, m_aBootstrapError( BE_OK )
, m_pLockfile( NULL )
, m_bServicesRegistered( false )
{
    RTL_LOGFILE_TRACE( "desktop (cd100003) ::Desktop::Desktop" );
}

Desktop::~Desktop()
{
}

void Desktop::Init()
{
    RTL_LOGFILE_CONTEXT( aLog, "desktop (cd100003) ::Desktop::Init" );

    // create service factory...
    Reference < XMultiServiceFactory > rSMgr = CreateApplicationServiceManager();
    if( rSMgr.is() )
    {
        ::comphelper::setProcessServiceFactory( rSMgr );
    }
    else
    {
        SetBootstrapError( BE_UNO_SERVICEMANAGER );
    }

    if ( GetBootstrapError() == BE_OK )
    {
        // prepare language
        if ( !LanguageSelection::prepareLanguage() )
            SetBootstrapError( BE_LANGUAGE_MISSING );
    }

    if ( GetBootstrapError() == BE_OK )
    {
        CommandLineArgs* pCmdLineArgs = GetCommandLineArgs();
#ifdef UNX
    //  check whether we need to print cmdline help
    if ( pCmdLineArgs->IsHelp() ) {
        displayCmdlineHelp();
        _exit( ExitHelper::E_NO_ERROR );
    }
#endif
        // start ipc thread only for non-remote offices
        RTL_LOGFILE_CONTEXT( aLog2, "desktop (cd100003) ::OfficeIPCThread::EnableOfficeIPCThread" );
        OfficeIPCThread::Status aStatus = OfficeIPCThread::EnableOfficeIPCThread();
        if ( aStatus == OfficeIPCThread::IPC_STATUS_BOOTSTRAP_ERROR )
        {
            SetBootstrapError( BE_PATHINFO_MISSING );
        }
        else if ( aStatus == OfficeIPCThread::IPC_STATUS_2ND_OFFICE )
        {
            // 2nd office startup should terminate after sending cmdlineargs through pipe
            _exit( ExitHelper::E_SECOND_OFFICE );
        }
        else if ( pCmdLineArgs->IsHelp() )
        {
            // disable IPC thread in an instance that is just showing a help message
            OfficeIPCThread::DisableOfficeIPCThread();
        }
        pSignalHandler = new SalMainPipeExchangeSignalHandler;
    }
}

void Desktop::DeInit()
{
    RTL_LOGFILE_CONTEXT( aLog, "desktop (cd100003) ::Desktop::DeInit" );

    try {
        // instead of removing of the configManager just let it commit all the changes
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "<- store config items" );
        utl::ConfigManager::GetConfigManager()->StoreConfigItems();
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "<- store config items" );

        // close splashscreen if it's still open
        CloseSplashScreen();
        Reference<XMultiServiceFactory> xXMultiServiceFactory(::comphelper::getProcessServiceFactory());
        DestroyApplicationServiceManager( xXMultiServiceFactory );
        // nobody should get a destroyd service factory...
        ::comphelper::setProcessServiceFactory( NULL );

        // clear lockfile
        if (m_pLockfile != NULL)
            m_pLockfile->clean();

        OfficeIPCThread::DisableOfficeIPCThread();
        if( pSignalHandler )
            DELETEZ( pSignalHandler );
    } catch (RuntimeException&) {
        // someone threw an exception during shutdown
        // this will leave some garbage behind..
    }

    RTL_LOGFILE_CONTEXT_TRACE( aLog, "FINISHED WITH Destop::DeInit" );
}

BOOL Desktop::QueryExit()
{
    const sal_Char SUSPEND_QUICKSTARTVETO[] = "SuspendQuickstartVeto";

    Reference< ::com::sun::star::frame::XDesktop >
            xDesktop( ::comphelper::getProcessServiceFactory()->createInstance( OUSTRING(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop")) ),
                UNO_QUERY );

    Reference < ::com::sun::star::beans::XPropertySet > xPropertySet( xDesktop, UNO_QUERY );
    if ( xPropertySet.is() )
    {
        Any a;
        a <<= (sal_Bool)sal_True;
        xPropertySet->setPropertyValue( OUSTRING(RTL_CONSTASCII_USTRINGPARAM( SUSPEND_QUICKSTARTVETO )), a );
    }

    BOOL bExit = ( !xDesktop.is() || xDesktop->terminate() );


    if ( !bExit && xPropertySet.is() )
    {
        Any a;
        a <<= (sal_Bool)sal_False;
        xPropertySet->setPropertyValue( OUSTRING(RTL_CONSTASCII_USTRINGPARAM( SUSPEND_QUICKSTARTVETO )), a );
    } else {
        if (m_pLockfile != NULL) m_pLockfile->clean();
    }

    return bExit;
}

void Desktop::StartSetup( const OUString& aParameters )
{
    OUString aProgName;
    OUString aSysPathFileName;
    OUString aDir;

    ::vos::OStartupInfo aInfo;
    aInfo.getExecutableFile( aProgName );

    sal_uInt32     lastIndex = aProgName.lastIndexOf('/');
    if ( lastIndex > 0 )
    {
        aProgName    = aProgName.copy( 0, lastIndex+1 );
        aDir        = aProgName;

        aProgName    += OUString( RTL_CONSTASCII_USTRINGPARAM( "setup" ));
#ifdef WNT
        aProgName    += OUString( RTL_CONSTASCII_USTRINGPARAM( ".exe" ));
#endif
    }

    OUString                aArgListArray[1];
    ::vos::OSecurity        aSecurity;
    ::vos::OEnvironment        aEnv;
    ::vos::OArgumentList    aArgList;

    aArgListArray[0] = aParameters;
    OArgumentList aArgumentList( aArgListArray, 1 );

    ::vos::OProcess    aProcess( aProgName, aDir );
    ::vos::OProcess::TProcessError aProcessError =
        aProcess.execute( OProcess::TOption_Detached,
                          aSecurity,
                          aArgumentList,
                          aEnv );

    if ( aProcessError != OProcess::E_None )
    {
        OUString aMessage( GetMsgString(
            STR_SETUP_ERR_CANNOT_START,
            OUString( RTL_CONSTASCII_USTRINGPARAM( "Couldn't start setup application! Please start it manually." )) ));

        ErrorBox aBootstrapFailedBox( NULL, WB_OK, aMessage );
        aBootstrapFailedBox.Execute();
    }
}

void Desktop::HandleBootstrapPathErrors( ::utl::Bootstrap::Status aBootstrapStatus, const OUString& aDiagnosticMessage )
{
    if ( aBootstrapStatus != ::utl::Bootstrap::DATA_OK )
    {
        sal_Bool            bWorkstationInstallation = sal_False;
        ::rtl::OUString        aBaseInstallURL;
        ::rtl::OUString        aUserInstallURL;
        ::rtl::OUString        aProductKey;
        ::rtl::OUString        aTemp;
        ::vos::OStartupInfo aInfo;

        aInfo.getExecutableFile( aProductKey );
        sal_uInt32     lastIndex = aProductKey.lastIndexOf('/');
        if ( lastIndex > 0 )
            aProductKey = aProductKey.copy( lastIndex+1 );

        aTemp = ::utl::Bootstrap::getProductKey( aProductKey );
        if ( aTemp.getLength() > 0 )
            aProductKey = aTemp;

        ::utl::Bootstrap::PathStatus aBaseInstallStatus = ::utl::Bootstrap::locateBaseInstallation( aBaseInstallURL );
        ::utl::Bootstrap::PathStatus aUserInstallStatus = ::utl::Bootstrap::locateUserInstallation( aUserInstallURL );

        if (( aBaseInstallStatus == ::utl::Bootstrap::PATH_EXISTS &&
              aUserInstallStatus == ::utl::Bootstrap::PATH_EXISTS        ))
        {
            if ( aBaseInstallURL != aUserInstallURL )
                bWorkstationInstallation = sal_True;
        }

        OUString        aMessage;
        OUStringBuffer    aBuffer( 100 );
        aBuffer.append( aDiagnosticMessage );

        aBuffer.appendAscii( "\n" );

        if (( aBootstrapStatus == ::utl::Bootstrap::MISSING_USER_INSTALL ) || bWorkstationInstallation )
        {
            // Check installation mode to suppress error message if we are currently running with a network installation.
            OUString aInstallMode( RTL_CONSTASCII_USTRINGPARAM( INSTALLMODE_STANDALONE ));

            aInstallMode = utl::Bootstrap::getInstallMode( aInstallMode );
            if ( aInstallMode.equalsIgnoreAsciiCaseAscii( INSTALLMODE_NETWORK ))
            {
                // network installation => start setup without error message
                OUString aParameters;
                StartSetup( aParameters );
            }
            else
            {
                OUString aAskSetupStr( GetMsgString(
                    STR_ASK_START_SETUP,
                    OUString( RTL_CONSTASCII_USTRINGPARAM( "Start setup application to check installation?" )) ));

                aBuffer.append( aAskSetupStr );
                aMessage = aBuffer.makeStringAndClear();

                ErrorBox aBootstrapFailedBox( NULL, WB_YES_NO, aMessage );
                aBootstrapFailedBox.SetText( aProductKey );
                int nResult = aBootstrapFailedBox.Execute();

                if ( nResult == RET_YES )
                {
                    OUString aParameters;
                    StartSetup( aParameters );
                }
            }
        }
        else if (( aBootstrapStatus == utl::Bootstrap::INVALID_USER_INSTALL ) ||
                    ( aBootstrapStatus == utl::Bootstrap::INVALID_BASE_INSTALL )     )
        {
            OUString aAskSetupRepairStr( GetMsgString(
                STR_ASK_START_SETUP_REPAIR,
                OUString( RTL_CONSTASCII_USTRINGPARAM( "Start setup application to repair installation?" )) ));

            aBuffer.append( aAskSetupRepairStr );
            aMessage = aBuffer.makeStringAndClear();

            ErrorBox aBootstrapFailedBox( NULL, WB_YES_NO, aMessage );
            aBootstrapFailedBox.SetText( aProductKey );
            int nResult = aBootstrapFailedBox.Execute();

            if ( nResult == RET_YES )
            {
                    OUString aParameters( RTL_CONSTASCII_USTRINGPARAM( "-repair" ));
                StartSetup( aParameters );
            }
        }
    }
}

// Create a error message depending on bootstrap failure code and an optional file url
::rtl::OUString    Desktop::CreateErrorMsgString(
    utl::Bootstrap::FailureCode nFailureCode,
    const ::rtl::OUString& aFileURL )
{
    OUString        aMsg;
    OUString        aFilePath;
    sal_Bool        bFileInfo = sal_True;

    switch ( nFailureCode )
    {
        /// the shared installation directory could not be located
        case ::utl::Bootstrap::MISSING_INSTALL_DIRECTORY:
        {
            aMsg = GetMsgString( STR_BOOTSTRAP_ERR_PATH_INVALID,
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "The installation path is not available." )) );
            bFileInfo = sal_False;
        }
        break;

        /// the bootstrap INI file could not be found or read
        case ::utl::Bootstrap::MISSING_BOOTSTRAP_FILE:
        {
            aMsg = GetMsgString( STR_BOOTSTRAP_ERR_FILE_MISSING,
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "The configuration file \"$1\" is missing." )) );
        }
        break;

        /// the bootstrap INI is missing a required entry
        /// the bootstrap INI contains invalid data
         case ::utl::Bootstrap::MISSING_BOOTSTRAP_FILE_ENTRY:
         case ::utl::Bootstrap::INVALID_BOOTSTRAP_FILE_ENTRY:
        {
            aMsg = GetMsgString( STR_BOOTSTRAP_ERR_FILE_CORRUPT,
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "The configuration file \"$1\" is corrupt." )) );
        }
        break;

        /// the version locator INI file could not be found or read
        case ::utl::Bootstrap::MISSING_VERSION_FILE:
        {
            aMsg = GetMsgString( STR_BOOTSTRAP_ERR_FILE_MISSING,
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "The configuration file \"$1\" is missing." )) );
        }
        break;

        /// the version locator INI has no entry for this version
         case ::utl::Bootstrap::MISSING_VERSION_FILE_ENTRY:
        {
            aMsg = GetMsgString( STR_BOOTSTRAP_ERR_NO_SUPPORT,
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "The main configuration file \"$1\" does not support the current version." )) );
        }
        break;

        /// the user installation directory does not exist
           case ::utl::Bootstrap::MISSING_USER_DIRECTORY:
        {
            aMsg = GetMsgString( STR_BOOTSTRAP_ERR_DIR_MISSING,
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "The configuration directory \"$1\" is missing." )) );
        }
        break;

        /// some bootstrap data was invalid in unexpected ways
        case ::utl::Bootstrap::INVALID_BOOTSTRAP_DATA:
        {
            aMsg = GetMsgString( STR_BOOTSTRAP_ERR_INTERNAL,
                        OUString( RTL_CONSTASCII_USTRINGPARAM( "An internal failure occurred." )) );
            bFileInfo = sal_False;
        }
        break;
    }

    if ( bFileInfo )
    {
        String aMsgString( aMsg );

        osl::File::getSystemPathFromFileURL( aFileURL, aFilePath );

        aMsgString.SearchAndReplaceAscii( "$1", aFilePath );
        aMsg = aMsgString;
    }

    return MakeStartupErrorMessage( aMsg );
}

void Desktop::HandleBootstrapErrors( BootstrapError aBootstrapError )
{
    if ( aBootstrapError == BE_PATHINFO_MISSING )
    {
        OUString                    aErrorMsg;
        OUString                    aBuffer;
        utl::Bootstrap::Status        aBootstrapStatus;
        utl::Bootstrap::FailureCode    nFailureCode;

        aBootstrapStatus = ::utl::Bootstrap::checkBootstrapStatus( aBuffer, nFailureCode );
        if ( aBootstrapStatus != ::utl::Bootstrap::DATA_OK )
        {
            switch ( nFailureCode )
            {
                case ::utl::Bootstrap::MISSING_INSTALL_DIRECTORY:
                case ::utl::Bootstrap::INVALID_BOOTSTRAP_DATA:
                {
                    aErrorMsg = CreateErrorMsgString( nFailureCode, OUString() );
                }
                break;

                /// the bootstrap INI file could not be found or read
                /// the bootstrap INI is missing a required entry
                /// the bootstrap INI contains invalid data
                 case ::utl::Bootstrap::MISSING_BOOTSTRAP_FILE_ENTRY:
                 case ::utl::Bootstrap::INVALID_BOOTSTRAP_FILE_ENTRY:
                case ::utl::Bootstrap::MISSING_BOOTSTRAP_FILE:
                {
                    OUString aBootstrapFileURL;

                    utl::Bootstrap::locateBootstrapFile( aBootstrapFileURL );
                    aErrorMsg = CreateErrorMsgString( nFailureCode, aBootstrapFileURL );
                }
                break;

                /// the version locator INI file could not be found or read
                /// the version locator INI has no entry for this version
                /// the version locator INI entry is not a valid directory URL
                   case ::utl::Bootstrap::INVALID_VERSION_FILE_ENTRY:
                 case ::utl::Bootstrap::MISSING_VERSION_FILE_ENTRY:
                 case ::utl::Bootstrap::MISSING_VERSION_FILE:
                {
                    OUString aVersionFileURL;

                    utl::Bootstrap::locateVersionFile( aVersionFileURL );
                    aErrorMsg = CreateErrorMsgString( nFailureCode, aVersionFileURL );
                }
                break;

                /// the user installation directory does not exist
                   case ::utl::Bootstrap::MISSING_USER_DIRECTORY:
                {
                    OUString aUserInstallationURL;

                    utl::Bootstrap::locateUserInstallation( aUserInstallationURL );
                    aErrorMsg = CreateErrorMsgString( nFailureCode, aUserInstallationURL );
                }
                break;
            }

            HandleBootstrapPathErrors( aBootstrapStatus, aErrorMsg );
        }
    }
    else if ( aBootstrapError == BE_UNO_SERVICEMANAGER || aBootstrapError == BE_UNO_SERVICE_CONFIG_MISSING )
    {
        // Uno service manager is not available. VCL needs a uno service manager to display a message box!!!
        // Currently we are not able to display a message box with a service manager due to this limitations inside VCL.

        // When UNO is not properly initialized, all kinds of things can fail
        // and cause the process to crash (e.g., a call to GetMsgString may
        // crash when somewhere deep within that call Any::operator <= is used
        // with a PropertyValue, and no binary UNO type description for
        // PropertyValue is available).  To give the user a hint even if
        // generating and displaying a message box below crashes, print a
        // hard-coded message on stderr first:
        fputs(
            aBootstrapError == BE_UNO_SERVICEMANAGER
            ? ("The application cannot be started. " "\n"
               "The component manager is not available." "\n")
                // STR_BOOTSTRAP_ERR_CANNOT_START, STR_BOOTSTRAP_ERR_NO_SERVICE
            : ("The application cannot be started. " "\n"
               "The configuration service is not available." "\n"),
                // STR_BOOTSTRAP_ERR_CANNOT_START,
                // STR_BOOTSTRAP_ERR_NO_CFG_SERVICE
            stderr);

        // First sentence. We cannot bootstrap office further!
        OUString            aMessage;
        OUStringBuffer        aDiagnosticMessage( 100 );

        OUString aErrorMsg;

        if ( aBootstrapError == BE_UNO_SERVICEMANAGER )
            aErrorMsg = GetMsgString( STR_BOOTSTRAP_ERR_NO_SERVICE,
                            OUString( RTL_CONSTASCII_USTRINGPARAM( "The service manager is not available." )) );
        else
            aErrorMsg = GetMsgString( STR_BOOTSTRAP_ERR_NO_CFG_SERVICE,
                            OUString( RTL_CONSTASCII_USTRINGPARAM( "The configuration service is not available." )) );

        aDiagnosticMessage.append( aErrorMsg );
        aDiagnosticMessage.appendAscii( "\n" );

        // Due to the fact the we haven't a backup applicat.rdb file anymore it is not possible to
        // repair the installation with the setup executable besides the office executable. Now
        // we have to ask the user to start the setup on CD/installation directory manually!!
        OUString aStartSetupManually( GetMsgString(
            STR_ASK_START_SETUP_MANUALLY,
            OUString( RTL_CONSTASCII_USTRINGPARAM( "Start setup application to repair the installation from CD, or the folder containing the installation packages." )) ));

        aDiagnosticMessage.append( aStartSetupManually );
        aMessage = MakeStartupErrorMessage( aDiagnosticMessage.makeStringAndClear() );

        FatalError( aMessage);
    }
    else if ( aBootstrapError == BE_USERINSTALL_FAILED )
    {
        OUString aMessage;
        OUStringBuffer aDiagnosticMessage( 100 );
        OUString aErrorMsg;
        aErrorMsg = GetMsgString( STR_BOOTSTRAP_ERR_INTERNAL,
            OUString( RTL_CONSTASCII_USTRINGPARAM( "User installation could not be completed" )) );
        aDiagnosticMessage.append( aErrorMsg );
        aMessage = MakeStartupErrorMessage( aDiagnosticMessage.makeStringAndClear() );
        FatalError(aMessage);
    }
    else if ( aBootstrapError == BE_LANGUAGE_MISSING )
    {
        OUString aMessage;
        OUStringBuffer aDiagnosticMessage( 100 );
        OUString aErrorMsg;
        aErrorMsg = GetMsgString(
            //@@@ FIXME: should use an own resource string => #i36213#
            STR_BOOTSTRAP_ERR_INTERNAL,
            OUString( RTL_CONSTASCII_USTRINGPARAM(
                "Language could not be determined." )) );
        aDiagnosticMessage.append( aErrorMsg );
        aMessage = MakeStartupErrorMessage(
            aDiagnosticMessage.makeStringAndClear() );
        FatalError(aMessage);
     }
     return;
}

//-----------------------------------------------
/** @short  check if recovery must be started or not.

    @param  bCrashed [boolean ... out!]
            the office crashed last times.
            But may be there are no recovery data.
            Usefull to trigger the error report tool without
            showing the recovery UI.

    @param  bRecoveryDataExists [boolean ... out!]
            there exists some recovery data.

    @param  bSessionDataExists [boolean ... out!]
            there exists some session data.
            Because the user may be logged out last time from it's
            unix session...
*/
void impl_checkRecoveryState(sal_Bool& bCrashed           ,
                             sal_Bool& bRecoveryDataExists,
                             sal_Bool& bSessionDataExists )
{
    static ::rtl::OUString SERVICENAME_RECOVERYCORE = ::rtl::OUString::createFromAscii("com.sun.star.frame.AutoRecovery");
    static ::rtl::OUString PROP_CRASHED             = ::rtl::OUString::createFromAscii("Crashed"                        );
    static ::rtl::OUString PROP_EXISTSRECOVERY      = ::rtl::OUString::createFromAscii("ExistsRecoveryData"             );
    static ::rtl::OUString PROP_EXISTSSESSION       = ::rtl::OUString::createFromAscii("ExistsSessionData"              );

    bCrashed            = sal_False;
    bRecoveryDataExists = sal_False;
    bSessionDataExists  = sal_False;

    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = ::comphelper::getProcessServiceFactory();
    try
    {
        css::uno::Reference< css::beans::XPropertySet > xRecovery(
            xSMGR->createInstance(SERVICENAME_RECOVERYCORE),
            css::uno::UNO_QUERY_THROW);

        xRecovery->getPropertyValue(PROP_CRASHED       ) >>= bCrashed           ;
        xRecovery->getPropertyValue(PROP_EXISTSRECOVERY) >>= bRecoveryDataExists;
        xRecovery->getPropertyValue(PROP_EXISTSSESSION ) >>= bSessionDataExists ;
    }
    catch(const css::uno::Exception&) {}
}

//-----------------------------------------------
/*  @short  start the recovery wizard.

    @param  bEmergencySave
            differs between EMERGENCY_SAVE and RECOVERY
*/
sal_Bool impl_callRecoveryUI(sal_Bool bEmergencySave     ,
                         sal_Bool bCrashed           ,
                         sal_Bool bExistsRecoveryData)
{
    static ::rtl::OUString SERVICENAME_RECOVERYUI = ::rtl::OUString::createFromAscii("com.sun.star.comp.svx.RecoveryUI"          );
    static ::rtl::OUString SERVICENAME_URLPARSER  = ::rtl::OUString::createFromAscii("com.sun.star.util.URLTransformer"          );
    static ::rtl::OUString COMMAND_EMERGENCYSAVE  = ::rtl::OUString::createFromAscii("vnd.sun.star.autorecovery:/doEmergencySave");
    static ::rtl::OUString COMMAND_RECOVERY       = ::rtl::OUString::createFromAscii("vnd.sun.star.autorecovery:/doAutoRecovery" );
    static ::rtl::OUString COMMAND_CRASHREPORT    = ::rtl::OUString::createFromAscii("vnd.sun.star.autorecovery:/doCrashReport"  );

    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = ::comphelper::getProcessServiceFactory();

    css::uno::Reference< css::frame::XSynchronousDispatch > xRecoveryUI(
        xSMGR->createInstance(SERVICENAME_RECOVERYUI),
        css::uno::UNO_QUERY_THROW);

    css::uno::Reference< css::util::XURLTransformer > xURLParser(
        xSMGR->createInstance(SERVICENAME_URLPARSER),
        css::uno::UNO_QUERY_THROW);

    css::util::URL aURL;
    if (bEmergencySave)
        aURL.Complete = COMMAND_EMERGENCYSAVE;
    else
    {
        if (bExistsRecoveryData)
            aURL.Complete = COMMAND_RECOVERY;
        else
        if (bCrashed)
            aURL.Complete = COMMAND_CRASHREPORT;
    }
    xURLParser->parseStrict(aURL);

    css::uno::Any aRet = xRecoveryUI->dispatchWithReturnValue(aURL, css::uno::Sequence< css::beans::PropertyValue >());
    sal_Bool bRet = sal_False;
    aRet >>= bRet;
    return bRet;
}

/*
 * Save all open documents so they will be reopened
 * the next time the application ist started
 *
 * returns sal_True if at least one document could be saved...
 *
 */

sal_Bool Desktop::_bTasksSaved = sal_False;

sal_Bool Desktop::SaveTasks(sal_Int32 options)
{
    return impl_callRecoveryUI(
        sal_True , // sal_True => force emergency save
        sal_False, // 2. and 3. param not used if 1. = true!
        sal_False);
}

USHORT Desktop::Exception(USHORT nError)
{
    // protect against recursive calls
    static BOOL bInException = FALSE;

    sal_uInt16 nOldMode = Application::GetSystemWindowMode();
    Application::SetSystemWindowMode( nOldMode & ~SYSTEMWINDOW_MODE_NOAUTOMODE );
    Application::SetDefDialogParent( NULL );

    if ( bInException )
    {
        String aDoubleExceptionString;
        Application::Abort( aDoubleExceptionString );
    }

    bInException = TRUE;
    BOOL bRecovery = FALSE;
    CommandLineArgs* pArgs = GetCommandLineArgs();

    // save all modified documents ... if it's allowed doing so.
    sal_Bool bRestart = sal_False;
    if ( !pArgs->IsNoRestore() && ( nError & EXC_MAJORTYPE ) != EXC_DISPLAY )
        bRestart = SaveTasks(DESKTOP_SAVETASKS_MOD);

    // because there is no method to flush the condiguration data, we must dispose the ConfigManager
    Reference < XFlushable > xCFGFlush( ::utl::ConfigManager::GetConfigManager()->GetConfigurationProvider(), UNO_QUERY );
    if (xCFGFlush.is())
    {
        xCFGFlush->flush();
    }
    else
    {
        Reference < XComponent > xCFGDispose( ::utl::ConfigManager::GetConfigManager()->GetConfigurationProvider(), UNO_QUERY );
        if (xCFGDispose.is())
            xCFGDispose->dispose();
    }

    switch( nError & EXC_MAJORTYPE )
    {
/*
        case EXC_USER:
            if( nError == EXC_OUTOFMEMORY )
            {
                // not possible without a special NewHandler!
                String aMemExceptionString;
                Application::Abort( aMemExceptionString );
            }
            break;
*/
        case EXC_RSCNOTLOADED:
        {
            String aResExceptionString;
            Application::Abort( aResExceptionString );
            break;
        }

        case EXC_SYSOBJNOTCREATED:
        {
            String aSysResExceptionString;
            Application::Abort( aSysResExceptionString );
            break;
        }

        default:
        {
            if ( pArgs->IsNoRestore() ) {
                if (m_pLockfile != NULL) {
                    m_pLockfile->clean();
                }
                _exit( ExitHelper::E_LOCKFILE );
            }

            if( bRestart )
            {
                OfficeIPCThread::DisableOfficeIPCThread();
                if( pSignalHandler )
                    DELETEZ( pSignalHandler );

                if (m_pLockfile != NULL) {
                    m_pLockfile->clean();
                }
                _exit( ExitHelper::E_CRASH_WITH_RESTART );
            }
            else
            {
                bInException = sal_False;
                _exit( ExitHelper::E_CRASH );
            }

            break;
        }
    }

    return 0;

    // ConfigManager is disposed, so no way to continue
}

void Desktop::AppEvent( const ApplicationEvent& rAppEvent )
{
    HandleAppEvent( rAppEvent );
}

void Desktop::Main()
{
    RTL_LOGFILE_CONTEXT( aLog, "desktop (cd100003) ::Desktop::Main" );

    // Remember current context object
    com::sun::star::uno::ContextLayer layer(
        com::sun::star::uno::getCurrentContext() );

    BootstrapError eError = GetBootstrapError();
    if ( eError != BE_OK )
    {
        HandleBootstrapErrors( eError );
        return;
    }

    // Detect desktop environment - need to do this as early as possible
    com::sun::star::uno::setCurrentContext(
        new DesktopContext( com::sun::star::uno::getCurrentContext() ) );

    CommandLineArgs* pCmdLineArgs = GetCommandLineArgs();

    // setup configuration error handling
    ConfigurationErrorHandler aConfigErrHandler;
    if (!ShouldSuppressUI(pCmdLineArgs))
        aConfigErrHandler.activate();

    // Startup screen
    RTL_LOGFILE_CONTEXT_TRACE( aLog, "desktop (lo119109) Desktop::Main { OpenSplashScreen" );
    OpenSplashScreen();
    RTL_LOGFILE_CONTEXT_TRACE( aLog, "desktop (lo119109) Desktop::Main } OpenSplashScreen" );

    {
        UserInstall::UserInstallError instErr_lang = UserInstall::configureLanguage();
        UserInstall::UserInstallError instErr_fin = UserInstall::finalize();
        if ( instErr_lang != UserInstall::E_None || instErr_fin != UserInstall::E_None)
        {
            OSL_ENSURE(sal_False, "userinstall failed");
            HandleBootstrapErrors( BE_USERINSTALL_FAILED );
            return;
        }
        // refresh path information
        utl::Bootstrap::reloadData();
        SetSplashScreenProgress(10);
    }

    Reference< XMultiServiceFactory > xSMgr =
        ::comphelper::getProcessServiceFactory();

    SvtLanguageOptions* pLanguageOptions = NULL;
    SvtPathOptions* pPathOptions = NULL;

    try
    {
        RegisterServices( xSMgr );

        SetSplashScreenProgress(15);

#ifndef UNX
        if ( pCmdLineArgs->IsHelp() ) {
            displayCmdlineHelp();
            return;
        }
#endif

        ResMgr::SetReadStringHook( ReplaceStringHookProc );
        SetAppName( DEFINE_CONST_UNICODE("soffice") );

        // check user installation directory for lockfile so we can be sure
        // there is no other instance using our data files from a remote host
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "desktop (lo119109) Desktop::Main -> Lockfile" );
        m_pLockfile = new Lockfile;
        if ( !pCmdLineArgs->IsInvisible() && !pCmdLineArgs->IsNoLockcheck() && !m_pLockfile->check( Lockfile_execWarning )) {
            // Lockfile exists, and user clicked 'no'
            return;
        }
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "desktop (lo119109) Desktop::Main <- Lockfile" );

        // check if accessibility is enabled but not working and allow to quit
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "{ GetEnableATToolSupport" );
        if( Application::GetSettings().GetMiscSettings().GetEnableATToolSupport() )
        {
            BOOL bQuitApp;

            if( !InitAccessBridge( true, bQuitApp ) )
                if( bQuitApp )
                    return;
        }
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "} GetEnableATToolSupport" );

        // terminate if requested...
        if( pCmdLineArgs->IsTerminateAfterInit() ) return;


        //  Read the common configuration items for optimization purpose
        if ( !InitializeConfiguration() ) return;

        SetSplashScreenProgress(20);

        // create title string
        sal_Bool bCheckOk = sal_False;
        ::com::sun::star::lang::Locale aLocale;
        String aMgrName = String::CreateFromAscii( "iso" );
        aMgrName += String::CreateFromInt32(SUPD); // current build version
        ResMgr* pLabelResMgr = ResMgr::SearchCreateResMgr( U2S( aMgrName ), aLocale );
        if ( !pLabelResMgr )
        {
            // no "iso" resource -> search for "ooo" resource
            aMgrName = String::CreateFromAscii( "ooo" );
            aMgrName += String::CreateFromInt32(SUPD); // current build version
            pLabelResMgr = ResMgr::SearchCreateResMgr( U2S( aMgrName ), aLocale);
        }
        String aTitle = pLabelResMgr ? String( ResId( RID_APPTITLE, pLabelResMgr ) ) : String();
        delete pLabelResMgr;

        // Check for StarOffice/Suite specific extensions runs also with OpenOffice installation sets
        OUString aTitleString( aTitle );
        bCheckOk = CheckInstallation( aTitleString );
        if ( !bCheckOk )
            return;
        else
            aTitle = aTitleString;

#ifndef PRODUCT
        //include version ID in non product builds
        ::rtl::OUString aDefault;
        aTitle += DEFINE_CONST_UNICODE(" [");
        String aVerId( utl::Bootstrap::getBuildIdData( aDefault ));
        aTitle += aVerId;
        aTitle += ']';
#endif

        SetDisplayName( aTitle );
        SetSplashScreenProgress(30);
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "{ create SvtPathOptions and SvtLanguageOptions" );
        pPathOptions = new SvtPathOptions;
        SetSplashScreenProgress(40);
        pLanguageOptions = new SvtLanguageOptions(sal_True);
        SetSplashScreenProgress(45);
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "} create SvtPathOptions and SvtLanguageOptions" );

        // Check special env variable #111015#
        std::vector< String > aUnrestrictedFolders;
        svt::getUnrestrictedFolders( aUnrestrictedFolders );

        if ( aUnrestrictedFolders.size() > 0 )
        {
            // Set different working directory. The first entry is
            // the new work path.
            String aWorkPath = aUnrestrictedFolders[0];
            SvtPathOptions().SetWorkPath( aWorkPath );
        }

        RTL_LOGFILE_CONTEXT_TRACE( aLog, "{ set locale settings" );
        /*
        String sLanguage = SvtPathOptions().SubstituteVariable(String::CreateFromAscii("$(langid)"));
        LanguageType eUILanguage = (LanguageType) sLanguage.ToInt32();
        */

        //LanguageSelection langselect;
        OUString aUILocaleString = LanguageSelection::getLanguageString();
        sal_Int32 nIndex = 0;
        OUString aLanguage = aUILocaleString.getToken( 0, '-', nIndex);
        OUString aCountry = aUILocaleString.getToken( 0, '-', nIndex);
        OUString aVariant = aUILocaleString.getToken( 0, '-', nIndex);

        ::com::sun::star::lang::Locale aUILocale( aLanguage, aCountry, aVariant );

        LanguageType eLanguage = SvtSysLocaleOptions().GetLocaleLanguageType();

        // #i39040#, do not call anything between GetSettings and SetSettings that might have
        // a side effect on the settings (like, eg, SvtSysLocaleOptions().GetLocaleLanguageType(),
        // which changes the MiscSettings !!! )
        AllSettings aSettings( Application::GetSettings() );
        aSettings.SetUILocale( aUILocale );
        aSettings.SetLanguage( eLanguage );
        Application::SetSettings( aSettings );
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "} set locale settings" );

        aMgrName = String::CreateFromAscii("ofa");
        aMgrName += String::CreateFromInt32(SOLARUPD); // aktuelle Versionsnummer
         ResMgr* pOffResMgr = ResMgr::CreateResMgr(U2S(aMgrName));
        Resource::SetResManager( pOffResMgr );

        // create service for loadin SFX (still needed in startup)
        Reference < css::document::XEventListener > xGlobalBroadcaster( xSMgr->createInstance(
            DEFINE_CONST_UNICODE( "com.sun.star.frame.GlobalEventBroadcaster" ) ), UNO_QUERY );

        // initialize test-tool library (if available)
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "{ tools::InitTestToolLib" );
        tools::InitTestToolLib();
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "} tools::InitTestToolLib" );

        // First Start Wizard
        {
            Reference< XJob > xFirstStartJob( xSMgr->createInstance(
                DEFINE_CONST_UNICODE( "com.sun.star.comp.desktop.FirstStart" ) ), UNO_QUERY );
            if (xFirstStartJob.is())
            {
                sal_Bool bDone = sal_False;
                xFirstStartJob->execute(Sequence<NamedValue>()) >>= bDone;
                if (!bDone) {
                    return;
                }
            }
        }
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "} FirstStartWizard" );

        if (xGlobalBroadcaster.is())
        {
            css::document::EventObject aEvent;
            aEvent.EventName = ::rtl::OUString::createFromAscii("OnStartApp");
            xGlobalBroadcaster->notifyEvent(aEvent);
        }

        SetSplashScreenProgress(50);

        // Backing Component
        sal_Bool bCrashed            = sal_False;
        sal_Bool bExistsRecoveryData = sal_False;
        sal_Bool bExistsSessionData  = sal_False;

        RTL_LOGFILE_CONTEXT_TRACE( aLog, "{ impl_checkRecoveryState" );
        impl_checkRecoveryState(bCrashed, bExistsRecoveryData, bExistsSessionData);
        RTL_LOGFILE_CONTEXT_TRACE( aLog, "} impl_checkRecoveryState" );

        if (
            (pCmdLineArgs->IsEmptyOrAcceptOnly()                                   ) &&
            (SvtModuleOptions().IsModuleInstalled(SvtModuleOptions::E_SSTARTMODULE)) &&
            (!bExistsRecoveryData                                                  ) &&
            (!bExistsSessionData                                                   )
           )
        {
            ::desktop::Desktop::bSuppressOpenDefault = sal_True;
            RTL_LOGFILE_CONTEXT_TRACE( aLog, "{ create BackingComponent" );
            Reference< XFrame > xDesktopFrame( xSMgr->createInstance(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))), UNO_QUERY );
            if (xDesktopFrame.is())
            {
                SetSplashScreenProgress(60);
                Reference< XFrame > xBackingFrame;
                Reference< ::com::sun::star::awt::XWindow > xContainerWindow;

                xBackingFrame = xDesktopFrame->findFrame(OUString( RTL_CONSTASCII_USTRINGPARAM( "_blank" )), 0);
                if (xBackingFrame.is())
                    xContainerWindow = xBackingFrame->getContainerWindow();
                if (xContainerWindow.is())
                {
                    SetSplashScreenProgress(70);
                    Sequence< Any > lArgs(1);
                    lArgs[0] <<= xContainerWindow;

                    Reference< XController > xBackingComp(
                        xSMgr->createInstanceWithArguments(OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.StartModule") ), lArgs),
                        UNO_QUERY);
                    SetSplashScreenProgress(80);
                    if (xBackingComp.is())
                    {
                        Reference< ::com::sun::star::awt::XWindow > xBackingWin(xBackingComp, UNO_QUERY);
                        // Attention: You MUST(!) call setComponent() before you call attachFrame().
                        // Because the backing component set the property "IsBackingMode" of the frame
                        // to true inside attachFrame(). But setComponent() reset this state everytimes ...
                        xBackingFrame->setComponent(xBackingWin, xBackingComp);
                        SetSplashScreenProgress(90);
                        xBackingComp->attachFrame(xBackingFrame);
                        CloseSplashScreen();
                        xContainerWindow->setVisible(sal_True);

                        Window* pCompWindow = VCLUnoHelper::GetWindow(xBackingFrame->getComponentWindow());
                        if (pCompWindow)
                            pCompWindow->Update();
                    }
                }
            }
            RTL_LOGFILE_CONTEXT_TRACE( aLog, "} create BackingComponent" );
        }
    }
    catch ( com::sun::star::lang::WrappedTargetException& wte )
    {
        com::sun::star::uno::Exception te;
        wte.TargetException >>= te;
        FatalError( MakeStartupConfigAccessErrorMessage(wte.Message + te.Message) );
        return;
    }
    catch ( com::sun::star::uno::Exception& e )
    {
        FatalError( MakeStartupErrorMessage(e.Message) );
        return;
    }
    /*
    catch ( ... )
    {
        FatalError( MakeStartupErrorMessage(
            OUString::createFromAscii(
            "Unknown error during startup (Office wrapper service).\nInstallation could be damaged.")));
        return;
    }
    */
    SetSplashScreenProgress(55);

    SvtFontSubstConfig().Apply();

    SvtTabAppearanceCfg aAppearanceCfg;
    //aAppearanceCfg.SetInitialized();
    aAppearanceCfg.SetApplicationDefaults( this );
    SvtAccessibilityOptions aOptions;
    aOptions.SetVCLSettings();
    SetSplashScreenProgress(60);

    Application::SetFilterHdl( LINK( this, Desktop, ImplInitFilterHdl ) );

    sal_Bool bTerminateRequested = sal_False;

    // Preload function depends on an initialized sfx application!
    SetSplashScreenProgress(70);

    sal_Bool bUseSystemFileDialog(sal_True);
    if ( pCmdLineArgs->IsHeadless() )
    {
        // Ensure that we use not the system file dialogs as
        // headless mode relies on Application::EnableHeadlessMode()
        // which does only work for VCL dialogs!!
        SvtMiscOptions aMiscOptions;
        bUseSystemFileDialog = aMiscOptions.UseSystemFileDialog();
        aMiscOptions.SetUseSystemFileDialog( sal_False );
    }

    // use system window dialogs
    Application::SetSystemWindowMode( SYSTEMWINDOW_MODE_DIALOG );

    SetSplashScreenProgress(80);

    if ( !bTerminateRequested && !pCmdLineArgs->IsInvisible() )
        InitializeQuickstartMode( xSMgr );

    RTL_LOGFILE_CONTEXT( aLog2, "desktop (cd100003) createInstance com.sun.star.frame.Desktop" );
    try
    {
        Reference< XDesktop > xDesktop( xSMgr->createInstance(
            OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))), UNO_QUERY );
        if ( xDesktop.is() )
            xDesktop->addTerminateListener( new OfficeIPCThreadController );
        SetSplashScreenProgress(100);
    }
    catch ( com::sun::star::uno::Exception& e )
    {
        FatalError( MakeStartupErrorMessage(e.Message) );
        return;
    }
    /*
    catch ( ... )
    {
        FatalError( MakeStartupErrorMessage(
            OUString::createFromAscii(
            "Unknown error during startup (TD/Desktop service).\nInstallation could be damaged.")));
        return;
    }
    */

    // Release solar mutex just before we wait for our client to connect
    int nAcquireCount = 0;
    ::vos::IMutex& rMutex = Application::GetSolarMutex();
    if ( rMutex.tryToAcquire() )
        nAcquireCount = Application::ReleaseSolarMutex() - 1;

    // Post user event to startup first application component window
    // We have to send this OpenClients message short before execute() to
    // minimize the risk that this message overtakes type detection contruction!!
    Application::PostUserEvent( LINK( this, Desktop, OpenClients_Impl ) );

    // Post event to enable acceptors
    Application::PostUserEvent( LINK( this, Desktop, EnableAcceptors_Impl) );

    // The configuration error handler currently is only for startup
    aConfigErrHandler.deactivate();

   // Acquire solar mutex just before we enter our message loop
    if ( nAcquireCount )
        Application::AcquireSolarMutex( nAcquireCount );

    // call Application::Execute to process messages in vcl message loop
    RTL_LOGFILE_TRACE( "PERFORMANCE - enter Application::Execute()" );

    try
    {
        // The JavaContext contains an interaction handler which is used when
        // the creation of a Java Virtual Machine fails
        com::sun::star::uno::ContextLayer layer(
            new JavaContext( com::sun::star::uno::getCurrentContext() ) );

        Execute();
    }
    catch(const com::sun::star::document::CorruptedFilterConfigurationException& exFilterCfg)
    {
        FatalError( MakeStartupErrorMessage(exFilterCfg.Message) );
        // dont kill the office here! Terminate it in the right way. It shouldnt be a problem ...
    }

    Resource::SetResManager( NULL );
    delete pResMgr;

    // Restore old value
    if ( pCmdLineArgs->IsHeadless() )
        SvtMiscOptions().SetUseSystemFileDialog( bUseSystemFileDialog );

    // remove temp directory
    RemoveTemporaryDirectory();

    DeregisterServices();

    tools::DeInitTestToolLib();

    RTL_LOGFILE_CONTEXT_TRACE( aLog, "-> dispose path/language options" );
    if (pLanguageOptions != NULL)
        delete pLanguageOptions;
    if (pPathOptions != NULL)
        delete pPathOptions;
    RTL_LOGFILE_CONTEXT_TRACE( aLog, "<- dispose path/language options" );

    RTL_LOGFILE_CONTEXT_TRACE( aLog, "-> deinit ucb" );
    ::ucb::ContentBroker::deinitialize();
    RTL_LOGFILE_CONTEXT_TRACE( aLog, "<- deinit ucb" );

    RTL_LOGFILE_CONTEXT_TRACE( aLog, "FINISHED WITH Destop::Main" );
}

IMPL_LINK( Desktop, ImplInitFilterHdl, ConvertData*, pData )
{
    return GraphicFilter::GetGraphicFilter()->GetFilterCallback().Call( pData );
}

sal_Bool Desktop::InitializeConfiguration()
{
    sal_Bool bOk = sal_False;

    try
    {
        bOk = InitConfiguration();
    }
    catch( ::com::sun::star::lang::ServiceNotRegisteredException& )
    {
        this->HandleBootstrapErrors( Desktop::BE_UNO_SERVICE_CONFIG_MISSING );
    }
    catch( ::com::sun::star::configuration::MissingBootstrapFileException& e )
    {
        OUString aMsg( CreateErrorMsgString( utl::Bootstrap::MISSING_BOOTSTRAP_FILE,
                                                e.BootstrapFileURL ));
        HandleBootstrapPathErrors( ::utl::Bootstrap::INVALID_USER_INSTALL, aMsg );
    }
    catch( ::com::sun::star::configuration::InvalidBootstrapFileException& e )
    {
        OUString aMsg( CreateErrorMsgString( utl::Bootstrap::INVALID_BOOTSTRAP_FILE_ENTRY,
                                                e.BootstrapFileURL ));
        HandleBootstrapPathErrors( ::utl::Bootstrap::INVALID_BASE_INSTALL, aMsg );
    }
    catch( ::com::sun::star::configuration::InstallationIncompleteException& )
    {
        OUString aVersionFileURL;
        OUString aMsg;
        utl::Bootstrap::PathStatus aPathStatus = utl::Bootstrap::locateVersionFile( aVersionFileURL );
        if ( aPathStatus == utl::Bootstrap::PATH_EXISTS )
            aMsg = CreateErrorMsgString( utl::Bootstrap::MISSING_VERSION_FILE_ENTRY, aVersionFileURL );
        else
            aMsg = CreateErrorMsgString( utl::Bootstrap::MISSING_VERSION_FILE, aVersionFileURL );

        HandleBootstrapPathErrors( ::utl::Bootstrap::MISSING_USER_INSTALL, aMsg );
    }
    catch ( com::sun::star::configuration::backend::BackendAccessException& exception)
    {
        // [cm122549] It is assumed in this case that the message
        // coming from InitConfiguration (in fact CreateApplicationConf...)
        // is suitable for display directly.
        FatalError( MakeStartupErrorMessage( exception.Message ) );
    }
    catch ( com::sun::star::configuration::backend::BackendSetupException& exception)
    {
        // [cm122549] It is assumed in this case that the message
        // coming from InitConfiguration (in fact CreateApplicationConf...)
        // is suitable for display directly.
        FatalError( MakeStartupErrorMessage( exception.Message ) );
    }
    catch ( ::com::sun::star::configuration::CannotLoadConfigurationException& )
    {
        OUString aMsg( CreateErrorMsgString( utl::Bootstrap::INVALID_BOOTSTRAP_DATA,
                                                OUString() ));
        HandleBootstrapPathErrors( ::utl::Bootstrap::INVALID_BASE_INSTALL, aMsg );
    }
    catch( ::com::sun::star::uno::Exception& )
    {
        OUString aMsg( CreateErrorMsgString( utl::Bootstrap::INVALID_BOOTSTRAP_DATA,
                                                OUString() ));
        HandleBootstrapPathErrors( ::utl::Bootstrap::INVALID_BASE_INSTALL, aMsg );
    }

    return bOk;
}

sal_Bool Desktop::InitializeQuickstartMode( Reference< XMultiServiceFactory >& rSMgr )
{
    try
    {
        // the shutdown icon sits in the systray and allows the user to keep
        // the office instance running for quicker restart
        // this will only be activated if -quickstart was specified on cmdline
        RTL_LOGFILE_CONTEXT( aLog, "desktop (cd100003) createInstance com.sun.star.office.Quickstart" );

        sal_Bool bQuickstart = GetCommandLineArgs()->IsQuickstart();
        Sequence< Any > aSeq( 1 );
        aSeq[0] <<= bQuickstart;

        // Try to instanciate quickstart service. This service is not mandatory, so
        // do nothing if service is not available.
        Reference < XComponent > xQuickstart( rSMgr->createInstanceWithArguments(
                                                DEFINE_CONST_UNICODE( "com.sun.star.office.Quickstart" ), aSeq ),
                                                UNO_QUERY );
        return sal_True;
    }
    catch( ::com::sun::star::uno::Exception& )
    {
        return sal_False;
    }
}

void Desktop::SystemSettingsChanging( AllSettings& rSettings, Window* pFrame )
{
    if ( !SvtTabAppearanceCfg::IsInitialized () )
        return;

#   define DRAGFULL_OPTION_ALL \
         ( DRAGFULL_OPTION_WINDOWMOVE | DRAGFULL_OPTION_WINDOWSIZE  \
         | DRAGFULL_OPTION_OBJECTMOVE  | DRAGFULL_OPTION_OBJECTSIZE \
         | DRAGFULL_OPTION_DOCKING     | DRAGFULL_OPTION_SPLIT      \
         | DRAGFULL_OPTION_SCROLL )
#   define DRAGFULL_OPTION_NONE ((sal_uInt32)~DRAGFULL_OPTION_ALL)

    StyleSettings hStyleSettings   = rSettings.GetStyleSettings();
    MouseSettings hMouseSettings = rSettings.GetMouseSettings();

    sal_uInt32 nDragFullOptions = hStyleSettings.GetDragFullOptions();

    SvtTabAppearanceCfg aAppearanceCfg;
    sal_uInt16 nGet = aAppearanceCfg.GetDragMode();
    switch ( nGet )
    {
    case DragFullWindow:
        nDragFullOptions |= DRAGFULL_OPTION_ALL;
        break;
    case DragFrame:
        nDragFullOptions &= DRAGFULL_OPTION_NONE;
        break;
    case DragSystemDep:
    default:
        break;
    }

    sal_uInt32 nFollow = hMouseSettings.GetFollow();
    hMouseSettings.SetFollow( aAppearanceCfg.IsMenuMouseFollow() ? (nFollow|MOUSE_FOLLOW_MENU) : (nFollow&~MOUSE_FOLLOW_MENU));
    rSettings.SetMouseSettings(hMouseSettings);

    sal_uInt16 nTabStyle = hStyleSettings.GetTabControlStyle();
    nTabStyle &= ~STYLE_TABCONTROL_SINGLELINE;
    if( aAppearanceCfg.IsSingleLineTabCtrl() )
        nTabStyle |=STYLE_TABCONTROL_SINGLELINE;

    nTabStyle &= ~STYLE_TABCONTROL_COLOR;
    if( aAppearanceCfg.IsColoredTabCtrl() )
        nTabStyle |= STYLE_TABCONTROL_COLOR;

    hStyleSettings.SetTabControlStyle(nTabStyle);

    hStyleSettings.SetDragFullOptions( nDragFullOptions );
    rSettings.SetStyleSettings ( hStyleSettings );

    MiscSettings aMiscSettings( rSettings.GetMiscSettings() );
    aMiscSettings.SetTwoDigitYearStart( (USHORT) SfxMiscCfg().GetYear2000() );
    rSettings.SetMiscSettings( aMiscSettings );
}

// ========================================================================
IMPL_LINK( Desktop, AsyncInitFirstRun, void*, NOTINTERESTEDIN )
{
    DoFirstRunInitializations();
    return 0L;
}

// ========================================================================

IMPL_STATIC_LINK( Desktop, AsyncTerminate, void*, NOTINTERESTEDIN )
{
    Reference<XMultiServiceFactory> rFactory = ::comphelper::getProcessServiceFactory();
    Reference< XDesktop > xDesktop( rFactory->createInstance(
        OUString::createFromAscii("com.sun.star.frame.Desktop")),
        UNO_QUERY );
    xDesktop.is() && xDesktop->terminate();
    return 0L;
}

IMPL_LINK( Desktop, OpenClients_Impl, void*, pvoid )
{
    RTL_LOGFILE_CONTEXT( aLog, "PERFORMANCE - DesktopOpenClients_Impl()" );

    OpenClients();

    // CloseStartupScreen();
    CloseSplashScreen();

    CheckFirstRun( );

    // allow ipc interaction
    OfficeIPCThread::SetReady();

    EnableOleAutomation();
    return 0;
}

// enable acceptos
IMPL_LINK( Desktop, EnableAcceptors_Impl, void*, pvoid )
{
    enableAcceptors();
    return 0;
}


// Registers a COM class factory of the service manager with the windows operating system.
void Desktop::EnableOleAutomation()
{
      RTL_LOGFILE_CONTEXT( aLog, "desktop (jl97489) ::Desktop::EnableOleAutomation" );
#ifdef WNT
    Reference< XMultiServiceFactory > xSMgr=  comphelper::getProcessServiceFactory();
    xSMgr->createInstance(DEFINE_CONST_UNICODE("com.sun.star.bridge.OleApplicationRegistration"));
    xSMgr->createInstance(DEFINE_CONST_UNICODE("com.sun.star.comp.ole.EmbedServer"));
#endif
}

sal_Bool Desktop::CheckOEM()
{
    Reference<XMultiServiceFactory> rFactory = ::comphelper::getProcessServiceFactory();
    Reference<XJob> rOemJob(rFactory->createInstance(
        OUString::createFromAscii("com.sun.star.office.OEMPreloadJob")),
        UNO_QUERY );
    Sequence<NamedValue> args;
    sal_Bool bResult = sal_False;
    if (rOemJob.is()) {
        Any aResult = rOemJob->execute(args);
        aResult >>= bResult;
        return bResult;
    } else {
        return sal_True;
    }
}

void Desktop::OpenClients()
{

    // check if a document has been recovered - if there is one of if a document was loaded by cmdline, no default document
    // should be created
    Reference < XComponent > xFirst;
    BOOL bLoaded = FALSE;

    CommandLineArgs* pArgs = GetCommandLineArgs();
    SvtInternalOptions  aInternalOptions;

    Reference<XMultiServiceFactory> rFactory = ::comphelper::getProcessServiceFactory();

    if (!pArgs->IsQuickstart()) {
        sal_Bool bShowHelp = sal_False;
        ::rtl::OUStringBuffer aHelpURLBuffer;
        if (pArgs->IsHelpWriter()) {
            bShowHelp = sal_True;
            aHelpURLBuffer.appendAscii("vnd.sun.star.help://swriter/start");
        } else if (pArgs->IsHelpCalc()) {
            bShowHelp = sal_True;
            aHelpURLBuffer.appendAscii("vnd.sun.star.help://scalc/start");
        } else if (pArgs->IsHelpDraw()) {
            bShowHelp = sal_True;
            aHelpURLBuffer.appendAscii("vnd.sun.star.help://sdraw/start");
        } else if (pArgs->IsHelpImpress()) {
            bShowHelp = sal_True;
            aHelpURLBuffer.appendAscii("vnd.sun.star.help://simpress/start");
        } else if (pArgs->IsHelpBase()) {
            bShowHelp = sal_True;
            aHelpURLBuffer.appendAscii("vnd.sun.star.help://sdatabase/start");
        } else if (pArgs->IsHelpBasic()) {
            bShowHelp = sal_True;
            aHelpURLBuffer.appendAscii("vnd.sun.star.help://sbasic/start");
        } else if (pArgs->IsHelpMath()) {
            bShowHelp = sal_True;
            aHelpURLBuffer.appendAscii("vnd.sun.star.help://smath/start");
        }
        if (bShowHelp) {
            Help *pHelp = Application::GetHelp();

            Any aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::LOCALE );
            rtl::OUString aTmp;
            aRet >>= aTmp;
            aHelpURLBuffer.appendAscii("?Language=");
            aHelpURLBuffer.append(aTmp);
#if defined UNX
            aHelpURLBuffer.appendAscii("&System=UNX");
#elif defined WNT
            aHelpURLBuffer.appendAscii("&System=WIN");
#elif defined MAC
            aHelpURLBuffer.appendAscii("&System=MAC");
#endif
            pHelp->Start(aHelpURLBuffer.makeStringAndClear(), NULL);
            return;
        }
    }

    // Disable AutoSave feature in case "-norestore" is given on the command line.
    // The reason behind: AutoSave/EmergencySave/AutoRecovery share the same data.
    // But the require that all documents, which are saved as backup should exists inside
    // memory. May be this mechanism will be inconsistent if the configuration exists ...
    // but no document inside memory corrspodn to this data.
    // Furter it's not acceptable to recover such documents without any UI. It can
    // need some time, where the user wont see any results and wait for finishing the office startup ...
    if ( pArgs->IsNoRestore() )
    {
        ::comphelper::ConfigurationHelper::writeDirectKey(
                ::comphelper::getProcessServiceFactory(),
                ::rtl::OUString::createFromAscii("org.openoffice.Office.Recovery"),
                ::rtl::OUString::createFromAscii("AutoSave"),
                ::rtl::OUString::createFromAscii("Enabled"),
                ::com::sun::star::uno::makeAny(sal_False),
                ::comphelper::ConfigurationHelper::E_STANDARD);
    }

    if ( !pArgs->IsServer() && !pArgs->IsNoRestore())
    {
        sal_Bool bCrashed            = sal_False;
        sal_Bool bExistsRecoveryData = sal_False;
        sal_Bool bExistsSessionData  = sal_False;

        impl_checkRecoveryState(bCrashed, bExistsRecoveryData, bExistsSessionData);

        if (
            ( ! bLoaded ) &&
            (
                ( bExistsRecoveryData ) || // => crash with files    => recovery
                ( bCrashed            )    // => crash without files => error report
            )
           )
        {
            try
            {
                impl_callRecoveryUI(
                    sal_False          , // false => force recovery instead of emergency save
                    bCrashed           ,
                    bExistsRecoveryData);
                /* TODO we cant be shure, that at least one document could be recovered here successfully
                    So we set bLoaded=TRUE to supress opening of the default document.
                    But we should make it more safe. Otherwhise we have an office without an UI ...
                    ...
                    May be we can check the desktop if some documents are existing there.
                 */
                Reference< XFramesSupplier > xTasksSupplier(
                        ::comphelper::getProcessServiceFactory()->createInstance( OUSTRING(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop")) ),
                        ::com::sun::star::uno::UNO_QUERY_THROW );
                Reference< XElementAccess > xList( xTasksSupplier->getFrames(), UNO_QUERY_THROW );
                if ( xList->hasElements() )
                    bLoaded = sal_True;
            }
            catch(const css::uno::Exception& e)
            {
                OUString aMessage = OUString::createFromAscii("Error during recovery\n")
                    + e.Message;
                OSL_ENSURE(sal_False, OUStringToOString(aMessage, RTL_TEXTENCODING_ASCII_US).getStr());
            }
        }

        Reference< XInitialization > xSessionListener;
        try
        {
            xSessionListener = Reference< XInitialization >(::comphelper::getProcessServiceFactory()->createInstance(
                        OUString::createFromAscii("com.sun.star.frame.SessionListener")), UNO_QUERY_THROW);
            xSessionListener->initialize(Sequence< Any >(0));
        }
        catch(const com::sun::star::uno::Exception& e)
        {
            OUString aMessage = OUString::createFromAscii("Registration of session listener failed\n")
                + e.Message;
            OSL_ENSURE(sal_False, OUStringToOString(aMessage, RTL_TEXTENCODING_ASCII_US).getStr());
        }

        if (
            ( ! bLoaded            ) &&
            (   bExistsSessionData )
           )
        {
            // session management
            try
            {
                Reference< XSessionManagerListener > r(xSessionListener, UNO_QUERY_THROW);
                bLoaded = r->doRestore();
            }
            catch(const com::sun::star::uno::Exception& e)
            {
                OUString aMessage = OUString::createFromAscii("Error in session management\n")
                    + e.Message;
                OSL_ENSURE(sal_False, OUStringToOString(aMessage, RTL_TEXTENCODING_ASCII_US).getStr());
            }
        }
    }

    if ( !pArgs->IsServer() )
    {
        ProcessDocumentsRequest aRequest;
        aRequest.pcProcessed = NULL;

        pArgs->GetOpenList( aRequest.aOpenList );
        pArgs->GetViewList( aRequest.aViewList );
        pArgs->GetStartList( aRequest.aStartList );
        pArgs->GetPrintList( aRequest.aPrintList );
        pArgs->GetPrintToList( aRequest.aPrintToList );
        pArgs->GetPrinterName( aRequest.aPrinterName );
        pArgs->GetForceOpenList( aRequest.aForceOpenList );
        pArgs->GetForceNewList( aRequest.aForceNewList );

        if ( aRequest.aOpenList.getLength() > 0 ||
             aRequest.aViewList.getLength() > 0 ||
             aRequest.aStartList.getLength() > 0 ||
             aRequest.aPrintList.getLength() > 0 ||
             aRequest.aForceOpenList.getLength() > 0 ||
             aRequest.aForceNewList.getLength() > 0 ||
             ( aRequest.aPrintToList.getLength() > 0 && aRequest.aPrinterName.getLength() > 0 ))
        {
            bLoaded = sal_True;

            if ( pArgs->HasModuleParam() )
            {
                SvtModuleOptions    aOpt;

                // Support command line parameters to start a module (as preselection)
                if ( pArgs->IsWriter() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
                    aRequest.aModule = aOpt.GetFactoryName( SvtModuleOptions::E_WRITER );
                else if ( pArgs->IsCalc() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SCALC ) )
                    aRequest.aModule = aOpt.GetFactoryName( SvtModuleOptions::E_CALC );
                else if ( pArgs->IsImpress() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SIMPRESS ) )
                    aRequest.aModule= aOpt.GetFactoryName( SvtModuleOptions::E_IMPRESS );
                else if ( pArgs->IsDraw() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SDRAW ) )
                    aRequest.aModule= aOpt.GetFactoryName( SvtModuleOptions::E_DRAW );
            }

            // Process request
            OfficeIPCThread::ExecuteCmdLineRequests( aRequest );
        }
    }

    // no default document if a document was loaded by recovery or by command line or if soffice is used as server
    if ( bLoaded || xFirst.is() || pArgs->IsServer() )
        return;

    if ( pArgs->IsQuickstart() || pArgs->IsInvisible() || pArgs->IsBean() )
        // soffice was started as tray icon ...
        return;
    {
        OpenDefault();
    }
}

void Desktop::OpenDefault()
{
    if (::desktop::Desktop::bSuppressOpenDefault)
        return;

    RTL_LOGFILE_CONTEXT( aLog, "desktop (cd100003) ::Desktop::OpenDefault" );

    ::rtl::OUString        aName;
    SvtModuleOptions    aOpt;

    CommandLineArgs* pArgs = GetCommandLineArgs();
    if ( pArgs->IsNoDefault() ) return;
    if ( pArgs->HasModuleParam() )
    {
        // Support new command line parameters to start a module
        if ( pArgs->IsWriter() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_WRITER );
        else if ( pArgs->IsCalc() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SCALC ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_CALC );
        else if ( pArgs->IsImpress() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SIMPRESS ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_IMPRESS );
        else if ( pArgs->IsBase() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SDATABASE ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_DATABASE );
        else if ( pArgs->IsDraw() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SDRAW ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_DRAW );
        else if ( pArgs->IsMath() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SMATH ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_MATH );
        else if ( pArgs->IsGlobal() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_WRITERGLOBAL );
        else if ( pArgs->IsWeb() && aOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_WRITERWEB );
    }

    if ( !aName.getLength() )
    {
        // Old way to create a default document
        if ( aOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_WRITER );
        else if ( aOpt.IsModuleInstalled( SvtModuleOptions::E_SCALC ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_CALC );
        else if ( aOpt.IsModuleInstalled( SvtModuleOptions::E_SIMPRESS ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_IMPRESS );
        else if ( aOpt.IsModuleInstalled( SvtModuleOptions::E_SDATABASE ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_DATABASE );
        else if ( aOpt.IsModuleInstalled( SvtModuleOptions::E_SDRAW ) )
            aName = aOpt.GetFactoryEmptyDocumentURL( SvtModuleOptions::E_DRAW );
        else
            return;
    }

    Sequence < PropertyValue > aNoArgs;
    Reference< XComponentLoader > xDesktop(
            ::comphelper::getProcessServiceFactory()->createInstance(
            OUSTRING(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop")) ),
            ::com::sun::star::uno::UNO_QUERY );

    // TODO: the dispatch has to be done for loadComponentFromURL as well. Please ask AS for moer detail.
    if ( !aName.compareToAscii( "service:"  , 8 ) )
    {
        URL             aURL ;
        aURL.Complete = aName;

        Reference < XDispatch >         xDispatcher ;
        Reference < XDispatchProvider > xProvider   ( xDesktop, UNO_QUERY );
        Reference < XURLTransformer >   xParser     ( ::comphelper::getProcessServiceFactory()->createInstance( OUSTRING(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.util.URLTransformer")) ), ::com::sun::star::uno::UNO_QUERY );

        if( xParser.is() == sal_True )
            xParser->parseStrict( aURL );

        if( xProvider.is() == sal_True )
            xDispatcher = xProvider->queryDispatch( aURL, ::rtl::OUString(), 0 );

        if( xDispatcher.is() == sal_True )
        {
            try
            {
                // We have to be listener to catch errors during dispatching URLs.
                // Otherwise it would be possible to have an office running without an open
                // window!!
                Reference < XNotifyingDispatch > xDisp( xDispatcher, UNO_QUERY );
                if ( xDisp.is() )
                    xDisp->dispatchWithNotification( aURL, aNoArgs, DispatchWatcher::GetDispatchWatcher() );
                else
                    xDispatcher->dispatch( aURL, aNoArgs );
            }
            catch ( ::com::sun::star::uno::Exception& )
            {
                OUString aMsg = OUString::createFromAscii(
                    "Desktop::OpenDefault() IllegalArgumentException while calling XNotifyingDispatch: ");
                OSL_ENSURE( sal_False, OUStringToOString(aMsg, RTL_TEXTENCODING_ASCII_US).getStr());
            }
        }
    }
    else
    {


        Reference<XComponent> aComp;
        try
        {
            aComp = Reference< XComponent >(xDesktop->loadComponentFromURL(
                aName, ::rtl::OUString::createFromAscii( "_default" ), 0, aNoArgs ), UNO_QUERY);
        }
        catch ( ::com::sun::star::lang::IllegalArgumentException& iae)
        {
            OUString aMsg = OUString::createFromAscii(
                "Desktop::OpenDefault() IllegalArgumentException while calling loadComponentFromURL: ")
                + iae.Message;
            OSL_ENSURE( sal_False, OUStringToOString(aMsg, RTL_TEXTENCODING_ASCII_US).getStr());
        }
        catch (com::sun::star::io::IOException& ioe)
        {
            OUString aMsg = OUString::createFromAscii(
                "Desktop::OpenDefault() IOException while calling loadComponentFromURL: ")
                + ioe.Message;
            OSL_ENSURE( sal_False, OUStringToOString(aMsg, RTL_TEXTENCODING_ASCII_US).getStr());
        }
        // shut down again if no component could be loaded
        OSL_ENSURE(aComp.is(), "Desktop::OpenDesfault(), no component was loaded.");
        if (!aComp.is())
            Application::PostUserEvent( STATIC_LINK( 0, Desktop, AsyncTerminate ) );
    }
}


String GetURL_Impl( const String& rName )
{
    // if rName is a vnd.sun.star.script URL do not attempt to parse it
    // as INetURLObj does not handle handle there URLs
    if (rName.CompareToAscii("vnd.sun.star.script" , 19) == COMPARE_EQUAL)
    {
        return rName;
    }

    // dont touch file urls, those should already be in internal form
    // they won't get better here (#112849#)
    if (rName.CompareToAscii("file:" , 5) == COMPARE_EQUAL)
    {
        return rName;
    }

    if ( rName.CompareToAscii("service:" , 8) == COMPARE_EQUAL )
    {
        return rName;
    }

    // if the filename is a physical name, it is the client file system, not the file system
    // of the machine where the office is running ( if this are different machines )
    // so in the remote case we can't handle relative filenames as arguments, because they
    // are parsed relative to the program path
    // the file system of the client is addressed through the "file:" protocol

    // Get current working directory to support relativ pathes
    ::rtl::OUString aWorkingDir;
    osl_getProcessWorkingDir( &aWorkingDir.pData );

    // Add path seperator to these directory and make given URL (rName) absolute by using of current working directory
    // Attention: "setFianlSlash()" is neccessary for calling "smartRel2Abs()"!!!
    // Otherwhise last part will be ignored and wrong result will be returned!!!
    // "smartRel2Abs()" interpret given URL as file not as path. So he truncate last element to get the base path ...
    // But if we add a seperator - he doesn't do it anymore.
    INetURLObject aObj( aWorkingDir );
    aObj.setFinalSlash();

    // Use the provided parameters for smartRel2Abs to support the usage of '%' in system paths.
    // Otherwise this char won't get encoded and we are not able to load such files later,
    // see #110156#
    bool bWasAbsolute;
    INetURLObject aURL     = aObj.smartRel2Abs( rName, bWasAbsolute, false, INetURLObject::WAS_ENCODED,
                                                RTL_TEXTENCODING_UTF8, true );
    String        aFileURL = aURL.GetMainURL(INetURLObject::NO_DECODE);

    ::osl::FileStatus aStatus( FileStatusMask_FileURL );
    ::osl::DirectoryItem aItem;
    if( ::osl::FileBase::E_None == ::osl::DirectoryItem::get( aFileURL, aItem ) &&
        ::osl::FileBase::E_None == aItem.getFileStatus( aStatus ) )
            aFileURL = aStatus.getFileURL();

    return aFileURL;
}

void Desktop::HandleAppEvent( const ApplicationEvent& rAppEvent )
{
    if ( rAppEvent.GetEvent() == "APPEAR" && !GetCommandLineArgs()->IsInvisible() )
    {
        // find active task - the active task is always a visible task
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFramesSupplier >
                xDesktop( ::comphelper::getProcessServiceFactory()->createInstance( OUSTRING(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop")) ),
                ::com::sun::star::uno::UNO_QUERY );
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > xTask = xDesktop->getActiveFrame();
        if ( !xTask.is() )
        {
            // get any task if there is no active one
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > xList( xDesktop->getFrames(), ::com::sun::star::uno::UNO_QUERY );
            if ( xList->getCount()>0 )
                xList->getByIndex(0) >>= xTask;
        }

        if ( xTask.is() )
        {
            Reference< com::sun::star::awt::XTopWindow > xTop( xTask->getContainerWindow(), UNO_QUERY );
            xTop->toFront();
        }
        else
            // no visible task that could be activated found
            OpenDefault();
    }
    else if ( rAppEvent.GetEvent() == "QUICKSTART" && !GetCommandLineArgs()->IsInvisible()  )
    {
        // If the office has been started the second time its command line arguments are sent through a pipe
        // connection to the first office. We want to reuse the quickstart option for the first office.
        // NOTICE: The quickstart service must be initialized inside the "main thread", so we use the
        // application events to do this (they are executed inside main thread)!!!
        // Don't start quickstart service if the user specified "-invisible" on the command line!
        sal_Bool bQuickstart( sal_True );
        Sequence< Any > aSeq( 1 );
        aSeq[0] <<= bQuickstart;

        Reference < XInitialization > xQuickstart( ::comphelper::getProcessServiceFactory()->createInstance(
                                            DEFINE_CONST_UNICODE( "com.sun.star.office.Quickstart" )),
                                            UNO_QUERY );
        if ( xQuickstart.is() )
            xQuickstart->initialize( aSeq );
    }
    else if ( rAppEvent.GetEvent() == "ACCEPT" )
    {
        // every time an accept parameter is used we create an acceptor
        // with the corresponding accept-string
        OUString aAcceptString(rAppEvent.GetData().GetBuffer());
        createAcceptor(aAcceptString);
    }
    else if ( rAppEvent.GetEvent() == "UNACCEPT" )
    {
        // try to remove corresponding acceptor
        OUString aUnAcceptString(rAppEvent.GetData().GetBuffer());
        destroyAcceptor(aUnAcceptString);
    }
    else if ( rAppEvent.GetEvent() == "SaveDocuments" )
    {
        Desktop::_bTasksSaved = sal_False;
        Desktop::_bTasksSaved = SaveTasks(DESKTOP_SAVETASKS_ALL);
        // SaveTasks(DESKTOP_SAVETASKS_MOD);
    }
    else if ( rAppEvent.GetEvent() == "OPENHELPURL" )
    {
        // start help for a specific URL
        OUString aHelpURL(rAppEvent.GetData().GetBuffer());
        Help *pHelp = Application::GetHelp();
        pHelp->Start(aHelpURL, NULL);
    }
#ifndef UNX
    else if ( rAppEvent.GetEvent() == "HELP" )
    {
        // in non unix version allow showing of cmdline help window
        displayCmdlineHelp();
    }
#endif
}

void Desktop::OpenSplashScreen()
{
       ::rtl::OUString        aTmpString;
    CommandLineArgs*    pCmdLine = GetCommandLineArgs();
    sal_Bool bVisible = sal_False;
    // Show intro only if this is normal start (e.g. no server, no quickstart, no printing )
    if ( !pCmdLine->IsInvisible() &&
         !pCmdLine->IsQuickstart() &&
         !pCmdLine->IsMinimized() &&
         !pCmdLine->IsNoLogo() &&
         !pCmdLine->IsTerminateAfterInit() &&
         !pCmdLine->GetPrintList( aTmpString ) &&
         !pCmdLine->GetPrintToList( aTmpString ) )
    {
        bVisible = sal_True;
        Sequence< Any > aSeq( 1 );
        aSeq[0] <<= bVisible;
        m_rSplashScreen = Reference<XStatusIndicator>(
            comphelper::getProcessServiceFactory()->createInstanceWithArguments(
            OUString::createFromAscii("com.sun.star.office.SplashScreen"),
            aSeq), UNO_QUERY);

        if(m_rSplashScreen.is())
                m_rSplashScreen->start(OUString::createFromAscii("SplashScreen"), 100);
    }

}

void Desktop::SetSplashScreenProgress(sal_Int32 iProgress)
{
    if(m_rSplashScreen.is())
    {
        m_rSplashScreen->setValue(iProgress);
    }
}

void Desktop::CloseSplashScreen()
{
    if(m_rSplashScreen.is())
    {
        m_rSplashScreen->end();
        m_rSplashScreen = NULL;
    }
}

// ========================================================================
void Desktop::DoFirstRunInitializations()
{
    try
    {
        Reference< XJobExecutor > xExecutor( ::comphelper::getProcessServiceFactory()->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.task.JobExecutor" ) ), UNO_QUERY );
        if( xExecutor.is() )
            xExecutor->trigger( ::rtl::OUString::createFromAscii("onFirstRunInitialization") );
    }
    catch(const ::com::sun::star::uno::Exception&)
    {
        OSL_ENSURE( sal_False, "Desktop::DoFirstRunInitializations: caught an exception while trigger job executor ..." );
    }
}

// ========================================================================
void Desktop::CheckFirstRun( )
{
    const ::rtl::OUString sCommonMiscNodeName = ::rtl::OUString::createFromAscii( "/org.openoffice.Office.Common/Misc" );
    const ::rtl::OUString sFirstRunNodeName = ::rtl::OUString::createFromAscii( "FirstRun" );

    // --------------------------------------------------------------------
    // check if this is the first office start

    // for this, open the Common/Misc node where this info is stored
    ::utl::OConfigurationTreeRoot aCommonMisc = ::utl::OConfigurationTreeRoot::createWithServiceFactory(
        ::comphelper::getProcessServiceFactory( ),
        sCommonMiscNodeName,
        2,
        ::utl::OConfigurationTreeRoot::CM_UPDATABLE
    );

    // read the flag
    OSL_ENSURE( aCommonMisc.isValid(), "Desktop::CheckFirstRun: could not open the config node needed!" );
    sal_Bool bIsFirstRun = sal_False;
    aCommonMisc.getNodeValue( sFirstRunNodeName ) >>= bIsFirstRun;

    if ( !bIsFirstRun )
        // nothing to do ....
        return;

    // --------------------------------------------------------------------
    // it is the first run
    // this has once been done using a vos timer. this could lead to problems when
    // the timer would trigger when the app is already going down again, since VCL would
    // no longer be available. Since the old handler would do a postUserEvent to the main
    // thread anyway, we can use a vcl timer here to prevent the race contition (#107197#)
    m_firstRunTimer.SetTimeout(3000); // 3 sec.
    m_firstRunTimer.SetTimeoutHdl(LINK(this, Desktop, AsyncInitFirstRun));
    m_firstRunTimer.Start();

    // --------------------------------------------------------------------
    // reset the config flag

    // set the value
    aCommonMisc.setNodeValue( sFirstRunNodeName, makeAny( (sal_Bool)sal_False ) );
    // commit the changes
    aCommonMisc.commit();
}

}
