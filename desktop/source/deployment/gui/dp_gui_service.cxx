/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dp_gui_service.cxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: ihi $ $Date: 2007-08-20 13:49:38 $
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
#include "precompiled_desktop.hxx"

#include "dp_gui_shared.hxx"
#include "dp_gui.h"
#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "unotools/configmgr.hxx"
#include "comphelper/servicedecl.hxx"
#include "comphelper/unwrapargs.hxx"
#include <i18npool/mslangid.hxx>
#include "vcl/svapp.hxx"
#include "vcl/msgbox.hxx"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/task/XJobExecutor.hpp"
#include "com/sun/star/ui/dialogs/XAsynchronousExecutableDialog.hpp"

#include "boost/bind.hpp"
#include "license_dialog.hxx"

using namespace ::dp_misc;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

using ::rtl::OUString;

namespace css = ::com::sun::star;
namespace dp_gui {

//==============================================================================
class MyApp : public Application, private boost::noncopyable
{
public:
    MyApp();
    virtual ~MyApp();

    // Application
    virtual void Main();
};

//______________________________________________________________________________
MyApp::~MyApp()
{
}

//______________________________________________________________________________
MyApp::MyApp()
{
}

//______________________________________________________________________________
void MyApp::Main()
{
}

//##############################################################################

namespace
{
    struct ProductName
        : public rtl::Static< String, ProductName > {};
    struct Version
        : public rtl::Static< String, Version > {};
    struct AboutBoxVersion
        : public rtl::Static< String, AboutBoxVersion > {};
    struct Extension
        : public rtl::Static< String, Extension > {};
}

void ReplaceProductNameHookProc( String& rStr )
{
    static int nAll = 0, nPro = 0;

    nAll++;
    if ( rStr.SearchAscii( "%PRODUCT" ) != STRING_NOTFOUND )
    {
        String &rProductName = ProductName::get();
        String &rVersion = Version::get();
        String &rAboutBoxVersion = AboutBoxVersion::get();
        String &rExtension = Extension::get();

        if ( !rProductName.Len() )
        {
            rtl::OUString aTmp;
            Any aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTNAME );
            aRet >>= aTmp;
            rProductName = aTmp;

            aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTVERSION );
            aRet >>= aTmp;
            rVersion = aTmp;

            aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::ABOUTBOXPRODUCTVERSION );
            aRet >>= aTmp;
            rAboutBoxVersion = aTmp;

            if ( !rExtension.Len() )
            {
                aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTEXTENSION );
                aRet >>= aTmp;
                rExtension = aTmp;
            }
        }

        nPro++;
        rStr.SearchAndReplaceAllAscii( "%PRODUCTNAME", rProductName );
        rStr.SearchAndReplaceAllAscii( "%PRODUCTVERSION", rVersion );
        rStr.SearchAndReplaceAllAscii( "%ABOUTBOXPRODUCTVERSION", rAboutBoxVersion );
        rStr.SearchAndReplaceAllAscii( "%PRODUCTEXTENSION", rExtension );
    }
}

//==============================================================================
class ServiceImpl
    : public ::cppu::WeakImplHelper2<ui::dialogs::XAsynchronousExecutableDialog,
                                     task::XJobExecutor>
{
    Reference<XComponentContext> const m_xComponentContext;
    boost::optional< Reference<awt::XWindow> > /* const */ m_parent;
    boost::optional<OUString> /* const */ m_view;
    boost::optional<Sequence<OUString> > m_extensions;
    OUString m_initialTitle;

public:
    ServiceImpl( Sequence<Any> const & args,
                 Reference<XComponentContext> const & xComponentContext );

    // XAsynchronousExecutableDialog
    virtual void SAL_CALL setDialogTitle( OUString const & aTitle )
        throw (RuntimeException);
    virtual void SAL_CALL startExecuteModal(
        Reference< ui::dialogs::XDialogClosedListener > const & xListener )
        throw (RuntimeException);

    // XJobExecutor
    virtual void SAL_CALL trigger( OUString const & event )
        throw (RuntimeException);
};

//______________________________________________________________________________
ServiceImpl::ServiceImpl( Sequence<Any> const& args,
                          Reference<XComponentContext> const& xComponentContext)
    : m_xComponentContext(xComponentContext)
{
    try {
        comphelper::unwrapArgs( args, m_parent, m_view );
        return;
    } catch (css::lang::IllegalArgumentException & ) {
    }
    try {
        comphelper::unwrapArgs( args, m_extensions, m_view );
    } catch (css::lang::IllegalArgumentException & ) {
    }

    ResHookProc pProc = ResMgr::GetReadStringHook();
    if ( !pProc )
        ResMgr::SetReadStringHook( ReplaceProductNameHookProc );
}

// XAsynchronousExecutableDialog
//______________________________________________________________________________
void ServiceImpl::setDialogTitle( OUString const & title )
    throw (RuntimeException)
{
    if (::dp_gui::DialogImpl::s_dialog.is()) {
        const ::vos::OGuard guard( Application::GetSolarMutex() );
        ::dp_gui::DialogImpl::get(
            m_xComponentContext,
            m_parent ? *m_parent : Reference<awt::XWindow>(),
            m_extensions ? *m_extensions : Sequence<OUString>(),
            m_view ? *m_view : OUString() )->SetText( title );
    }
    else
        m_initialTitle = title;
}

//______________________________________________________________________________
void ServiceImpl::startExecuteModal(
    Reference< ui::dialogs::XDialogClosedListener > const & xListener )
    throw (RuntimeException)
{
    ::std::auto_ptr<Application> app;
    if (! dp_gui::DialogImpl::s_dialog.is())
    {
        const bool bAppUp = (GetpApp() != 0);
        bool bOfficePipePresent;
        try {
            bOfficePipePresent = dp_misc::office_is_running();
        }
        catch (Exception & exc) {
            if (bAppUp) {
                const vos::OGuard guard( Application::GetSolarMutex() );
                std::auto_ptr<ErrorBox> box(
                    new ErrorBox( Application::GetActiveTopWindow(),
                                  WB_OK, exc.Message ) );
                box->Execute();
            }
            throw;
        }

        if (! bOfficePipePresent) {
            OSL_ASSERT( ! bAppUp );
            app.reset( new MyApp );
            if (! InitVCL( Reference<lang::XMultiServiceFactory>(
                               m_xComponentContext->getServiceManager(),
                               UNO_QUERY_THROW ) ))
                throw RuntimeException( OUSTR("Cannot initialize VCL!"),
                                        static_cast<OWeakObject *>(this) );
            AllSettings as = app->GetSettings();
            OUString slang;
            if (! (::utl::ConfigManager::GetDirectConfigProperty(
                       ::utl::ConfigManager::LOCALE ) >>= slang))
                throw RuntimeException( OUSTR("Cannot determine language!"),
                                        static_cast<OWeakObject *>(this) );
            as.SetUILanguage( MsLangId::convertIsoStringToLanguage( slang ) );
            app->SetSettings( as );
        }
    }
    else
    {
        //Currently we do not support the case that if the Extension Manager is
        //open in an office an unopkg can be executed. This should be fixed in the future.
        ResId warnId(WARNINGBOX_CONCURRENTINSTANCE,*DeploymentGuiResMgr::get() );
        WarningBox warn(NULL, warnId);
        warn.Execute();
    }

    {
        const ::vos::OGuard guard( Application::GetSolarMutex() );
        ::rtl::Reference< ::dp_gui::DialogImpl > dialog(
            ::dp_gui::DialogImpl::get(
                m_xComponentContext,
                m_parent ? *m_parent : Reference<awt::XWindow>(),
                m_extensions ? *m_extensions : Sequence<OUString>(),
                m_view ? *m_view : OUString() ) );
        if (m_initialTitle.getLength() > 0) {
            dialog->SetText( m_initialTitle );
            m_initialTitle = OUString();
        }
        dialog->Show();
        dialog->ToTop( TOTOP_RESTOREWHENMIN );
    }

    if (app.get() != 0) {
        Application::Execute();
        DeInitVCL();
    }

    if (xListener.is())
        xListener->dialogClosed(
            ui::dialogs::DialogClosedEvent(
                static_cast< ::cppu::OWeakObject * >(this),
                sal_Int16(0)) );
}

// XJobExecutor
//______________________________________________________________________________
void ServiceImpl::trigger( OUString const & ) throw (RuntimeException)
{
    startExecuteModal( Reference< ui::dialogs::XDialogClosedListener >() );
}

namespace sdecl = comphelper::service_decl;
sdecl::class_<ServiceImpl, sdecl::with_args<true> > serviceSI;
sdecl::ServiceDecl const serviceDecl(
    serviceSI,
    "com.sun.star.comp.deployment.ui.PackageManagerDialog",
    "com.sun.star.deployment.ui.PackageManagerDialog" );

sdecl::class_<LicenseDialog, sdecl::with_args<true> > licenseSI;
sdecl::ServiceDecl const licenseDecl(
    licenseSI,
    "com.sun.star.comp.deployment.ui.LicenseDialog",
    "com.sun.star.deployment.ui.LicenseDialog" );
} // namespace dp_gui

extern "C" {

void SAL_CALL component_getImplementationEnvironment(
    const sal_Char ** ppEnvTypeName, uno_Environment ** )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

sal_Bool SAL_CALL component_writeInfo(
    lang::XMultiServiceFactory * pServiceManager,
    registry::XRegistryKey * pRegistryKey )
{
    return component_writeInfoHelper(
        pServiceManager, pRegistryKey, dp_gui::serviceDecl, dp_gui::licenseDecl );
}

void * SAL_CALL component_getFactory(
    sal_Char const * pImplName,
    lang::XMultiServiceFactory * pServiceManager,
    registry::XRegistryKey * pRegistryKey )
{
    return component_getFactoryHelper(
        pImplName, pServiceManager, pRegistryKey, dp_gui::serviceDecl, dp_gui::licenseDecl );
}

} // extern "C"
