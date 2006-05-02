/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: appinit.cxx,v $
 *
 *  $Revision: 1.53 $
 *
 *  last change: $Author: rt $ $Date: 2006-05-02 16:15:59 $
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

#include "app.hxx"

#ifndef _COM_SUN_STAR_FRAME_XTERMINATELISTENER_HPP_
#include <com/sun/star/frame/XTerminateListener.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif

#include <svtools/soerr.hxx>
#include <svtools/svtools.hrc>
#include <svtools/saveopt.hxx>
#include <svtools/localisationoptions.hxx>

#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif
#ifndef _SV_RESARY_HXX
#include <tools/resary.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _EHDL_HXX
#include <svtools/ehdl.hxx>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _VOS_SECURITY_HXX_
#include <vos/security.hxx>
#endif
#ifndef _UCBHELPER_CONFIGURATIONKEYS_HXX_
#include <ucbhelper/configurationkeys.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_HISTORYOPTIONS_HXX
#include <svtools/historyoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#include <rtl/logfile.hxx>
#include <vcl/edit.hxx>

#ifndef GCC
#pragma hdrstop
#endif

#include "unoctitm.hxx"
#include "app.hrc"
#include "sfxlocal.hrc"
#include "appdata.hxx"
#include "arrdecl.hxx"
#include "dispatch.hxx"
#include "docfac.hxx"
#include "evntconf.hxx"
#include "intro.hxx"
#include "macrconf.hxx"
#include "mnumgr.hxx"
#include "msgpool.hxx"
#include "newhdl.hxx"
#include "progress.hxx"
#include "sfxhelp.hxx"
#include "sfxresid.hxx"
#include "sfxtypes.hxx"
#include "viewsh.hxx"
#include "nochaos.hxx"
#include "fcontnr.hxx"
#include "helper.hxx"   // SfxContentHelper::Kill()
#include "sfxpicklist.hxx"

#ifdef UNX
#define stricmp(a,b) strcmp(a,b)
#endif

#ifdef MAC
int svstricmp( const char* pStr1, const char* pStr2);
#define stricmp svstricmp
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star;
namespace css = ::com::sun::star;

class SfxTerminateListener_Impl : public ::cppu::WeakImplHelper1< XTerminateListener  >
{
public:
    virtual void SAL_CALL queryTermination( const EventObject& aEvent ) throw( TerminationVetoException, RuntimeException );
    virtual void SAL_CALL notifyTermination( const EventObject& aEvent ) throw( RuntimeException );
    virtual void SAL_CALL disposing( const EventObject& Source ) throw( RuntimeException );
};

void SAL_CALL SfxTerminateListener_Impl::disposing( const EventObject& Source ) throw( RuntimeException )
{
}

void SAL_CALL SfxTerminateListener_Impl::queryTermination( const EventObject& aEvent ) throw(TerminationVetoException, RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( !SFX_APP()->QueryExit_Impl() )
        throw TerminationVetoException();
}

void SAL_CALL SfxTerminateListener_Impl::notifyTermination( const EventObject& aEvent ) throw(RuntimeException )
{
    static ::rtl::OUString SERVICE_GLOBALEVENTBROADCASTER = ::rtl::OUString::createFromAscii("com.sun.star.frame.GlobalEventBroadcaster");
    static ::rtl::OUString EVENT_QUIT_APP                 = ::rtl::OUString::createFromAscii("OnCloseApp");

    Reference< XDesktop > xDesktop( aEvent.Source, UNO_QUERY );
    if( xDesktop.is() == sal_True )
        xDesktop->removeTerminateListener( this );

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    utl::ConfigManager::GetConfigManager()->StoreConfigItems();
    SfxApplication* pApp = SFX_APP();
    pApp->Get_Impl()->aLateInitTimer.Stop();
    pApp->Broadcast( SfxSimpleHint( SFX_HINT_DEINITIALIZING ) );
    pApp->Get_Impl()->pAppDispatch->ReleaseAll();
    pApp->Get_Impl()->pAppDispatch->release();
    pApp->NotifyEvent(SfxEventHint( SFX_EVENT_CLOSEAPP) );

    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = ::comphelper::getProcessServiceFactory();
    css::uno::Reference< css::document::XEventListener > xGlobalBroadcaster(xSMGR->createInstance(SERVICE_GLOBALEVENTBROADCASTER), css::uno::UNO_QUERY);
    if (xGlobalBroadcaster.is())
    {
        css::document::EventObject aEvent;
        aEvent.EventName = EVENT_QUIT_APP;
        xGlobalBroadcaster->notifyEvent(aEvent);
    }

    //pApp->Deinitialize();
    delete pApp;
    Application::Quit();
}

//====================================================================

#define DOSTRING( x )                       #x
#define STRING( x )                         DOSTRING( x )

typedef String ( *PFunc_getSpecialCharsForEdit)( Window* pParent, const Font& rFont );

//====================================================================
// Lazy binding of the GetSpecialCharsForEdit function as it resides in
// a library above us.
//====================================================================

String GetSpecialCharsForEdit(Window* pParent, const Font& rFont)
{
    static bool bDetermineFunction = false;
    static PFunc_getSpecialCharsForEdit pfunc_getSpecialCharsForEdit = 0;

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( !bDetermineFunction )
    {
        bDetermineFunction = true;

        String sLibName = String::CreateFromAscii( STRING( DLL_NAME ) );
        sLibName.SearchAndReplace( String( RTL_CONSTASCII_USTRINGPARAM( "sfx" ) ), String( RTL_CONSTASCII_USTRINGPARAM( "svx" ) ) );

        rtl::OUString aLibName( sLibName );
        oslModule handleMod = osl_loadModule( aLibName.pData, 0 );

        // get symbol
        ::rtl::OUString aSymbol( RTL_CONSTASCII_USTRINGPARAM( "GetSpecialCharsForEdit" ) );
        pfunc_getSpecialCharsForEdit = (PFunc_getSpecialCharsForEdit)osl_getSymbol( handleMod, aSymbol.pData );
        DBG_ASSERT( pfunc_getSpecialCharsForEdit, "GetSpecialCharsForEdit() not found!" );
    }

    if ( pfunc_getSpecialCharsForEdit )
        return (*pfunc_getSpecialCharsForEdit)( pParent, rFont );
    else
        return String();
}

//====================================================================

FASTBOOL SfxApplication::Initialize_Impl()
{
    RTL_LOGFILE_CONTEXT( aLog, "sfx2 (mb93783) ::SfxApplication::Initialize_Impl" );

#ifdef TLX_VALIDATE
    StgIo::SetErrorLink( LINK( this, SfxStorageErrHdl, Error ) );
#endif

    Reference < XDesktop > xDesktop ( ::comphelper::getProcessServiceFactory()->createInstance( DEFINE_CONST_UNICODE("com.sun.star.frame.Desktop") ), UNO_QUERY );
    if (!xDesktop.is())
         throw RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Couldn't create mandatory desktop service!" )), xDesktop );
    xDesktop->addTerminateListener( new SfxTerminateListener_Impl() );

    Application::EnableAutoHelpId();

    pAppData_Impl->pAppDispatch = new SfxStatusDispatcher;
    pAppData_Impl->pAppDispatch->acquire();

    // SV-Look
    Help::EnableContextHelp();
    Help::EnableExtHelp();

    SvtLocalisationOptions aLocalisation;
    Application::EnableAutoMnemonic ( aLocalisation.IsAutoMnemonic() );
    Application::SetDialogScaleX    ( (short)(aLocalisation.GetDialogScale()) );


#ifdef DBG_UTIL
    // Der SimplerErrorHandler dient Debugzwecken. In der Product werden
    // nichtgehandelte Fehler durch Errorcode 1 an SFX gegeben.
    new SimpleErrorHandler;
#endif
    new SfxErrorHandler(RID_ERRHDL, ERRCODE_AREA_TOOLS, ERRCODE_AREA_LIB1);

    new SfxErrorHandler(
        RID_SO_ERROR_HANDLER, ERRCODE_AREA_SO, ERRCODE_AREA_SO_END);
    new SfxErrorHandler(
        (RID_SJ_START +1), ERRCODE_AREA_JAVA, ERRCODE_AREA_JAVA_END);
    new SfxErrorHandler(
        RID_BASIC_START, ERRCODE_AREA_SBX, ERRCODE_AREA_SBX_END );

    // diverse Pointer
    SfxPickList::GetOrCreate( SvtHistoryOptions().GetSize( ePICKLIST ) );

    /////////////////////////////////////////////////////////////////

    DBG_ASSERT( !pAppData_Impl->pAppDispat, "AppDispatcher already exists" );
    pAppData_Impl->pAppDispat = new SfxDispatcher((SfxDispatcher*)0);
    pAppData_Impl->pSlotPool = new SfxSlotPool;
    pAppData_Impl->pTbxCtrlFac = new SfxTbxCtrlFactArr_Impl;
    pAppData_Impl->pStbCtrlFac = new SfxStbCtrlFactArr_Impl;
    pAppData_Impl->pMenuCtrlFac = new SfxMenuCtrlFactArr_Impl;
    pAppData_Impl->pViewFrames = new SfxViewFrameArr_Impl;
    pAppData_Impl->pViewShells = new SfxViewShellArr_Impl;
    pAppData_Impl->pObjShells = new SfxObjectShellArr_Impl;
    pAppData_Impl->nInterfaces = SFX_INTERFACE_APP+8;
    pAppData_Impl->pInterfaces = new SfxInterface*[pAppData_Impl->nInterfaces];
    memset( pAppData_Impl->pInterfaces, 0, sizeof(SfxInterface*) * pAppData_Impl->nInterfaces );

    // create NewHandler and its Resource
    SfxNewHdl* pNewHdl = SfxNewHdl::GetOrCreate();
    Registrations_Impl();

//    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    // TODO/LATER: exchange confusing defines; CREATEDOC -> NEWDOC, DOCCREATED -> CREATEDOC
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_STARTAPP,        String(SfxResId(STR_EVENT_STARTAPP)),   SfxObjectShell::GetEventNames_Impl()[0] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_CLOSEAPP,        String(SfxResId(STR_EVENT_CLOSEAPP)),   SfxObjectShell::GetEventNames_Impl()[1] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_CREATEDOC,       String(SfxResId(STR_EVENT_CREATEDOC)),  SfxObjectShell::GetEventNames_Impl()[2] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_OPENDOC,         String(SfxResId(STR_EVENT_OPENDOC)),    SfxObjectShell::GetEventNames_Impl()[3] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVEASDOC,       String(SfxResId(STR_EVENT_SAVEASDOC)),  SfxObjectShell::GetEventNames_Impl()[4] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVEASDOCDONE,   String(SfxResId(STR_EVENT_SAVEASDOCDONE)),  SfxObjectShell::GetEventNames_Impl()[5] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVEDOC,         String(SfxResId(STR_EVENT_SAVEDOC)),        SfxObjectShell::GetEventNames_Impl()[6] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVEDOCDONE,     String(SfxResId(STR_EVENT_SAVEDOCDONE)),    SfxObjectShell::GetEventNames_Impl()[7] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_PREPARECLOSEDOC, String(SfxResId(STR_EVENT_PREPARECLOSEDOC)),SfxObjectShell::GetEventNames_Impl()[8] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_CLOSEDOC,        String(SfxResId(STR_EVENT_CLOSEDOC)),       SfxObjectShell::GetEventNames_Impl()[9] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_ACTIVATEDOC,     String(SfxResId(STR_EVENT_ACTIVATEDOC)),    SfxObjectShell::GetEventNames_Impl()[10] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_DEACTIVATEDOC,   String(SfxResId(STR_EVENT_DEACTIVATEDOC)),  SfxObjectShell::GetEventNames_Impl()[11] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_PRINTDOC,        String(SfxResId(STR_EVENT_PRINTDOC)),       SfxObjectShell::GetEventNames_Impl()[12] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_MODIFYCHANGED,   String(SfxResId(STR_EVENT_MODIFYCHANGED)),  SfxObjectShell::GetEventNames_Impl()[13] );

    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVETODOC,           String(), SfxObjectShell::GetEventNames_Impl()[14] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVETODOCDONE,       String(), SfxObjectShell::GetEventNames_Impl()[15] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_VIEWCREATED,         String(), SfxObjectShell::GetEventNames_Impl()[16] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_PREPARECLOSEVIEW,    String(), SfxObjectShell::GetEventNames_Impl()[17] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_CLOSEVIEW,           String(), SfxObjectShell::GetEventNames_Impl()[18] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_VISAREACHANGED,      String(), SfxObjectShell::GetEventNames_Impl()[19] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_DOCCREATED,          String(), SfxObjectShell::GetEventNames_Impl()[20] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_LOADFINISHED,        String(), SfxObjectShell::GetEventNames_Impl()[21] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVEASDOCFAILED,     String(), SfxObjectShell::GetEventNames_Impl()[22] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVEDOCFAILED,       String(), SfxObjectShell::GetEventNames_Impl()[23] );
    SfxEventConfiguration::RegisterEvent(SFX_EVENT_SAVETODOCFAILED,     String(), SfxObjectShell::GetEventNames_Impl()[24] );
    SfxEventConfiguration::RegisterEvent(SFX_HINT_TITLECHANGED,         String(), SfxObjectShell::GetEventNames_Impl()[25] );

    // Subklasse initialisieren
    pAppData_Impl->bDowning = sal_False;
    Init();

    // get CHAOS item pool...
    pAppData_Impl->pPool = NoChaos::GetItemPool();
    SetPool( pAppData_Impl->pPool );

    InsertLateInitHdl( LINK(pNewHdl, SfxNewHdl, InitMem_Impl) );
    InsertLateInitHdl( LINK(this, SfxApplication,SpecialService_Impl) );
    InsertLateInitHdl( STATIC_LINK( pAppData_Impl, SfxAppData_Impl, CreateDocumentTemplates ) );

    if ( pAppData_Impl->bDowning )
        return sal_False;

    // App-Dispatcher aufbauen
    pAppData_Impl->pAppDispat->Push(*this);
    pAppData_Impl->pAppDispat->Flush();
    pAppData_Impl->pAppDispat->DoActivate_Impl( sal_True );

    // start LateInit
    SfxAppData_Impl *pAppData = Get_Impl();
    pAppData->aLateInitTimer.SetTimeout( 250 );
    pAppData->aLateInitTimer.SetTimeoutHdl( LINK( this, SfxApplication, LateInitTimerHdl_Impl ) );
    pAppData->aLateInitTimer.Start();

    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );
        // Set special characters callback on vcl edit control
        Edit::SetGetSpecialCharsFunction(&GetSpecialCharsForEdit);
    }

    return sal_True;
}

IMPL_LINK( SfxApplication, SpecialService_Impl, void*, pVoid )
{
    // StarOffice registration
    INetURLObject aORegObj( SvtPathOptions().GetUserConfigPath(), INET_PROT_FILE );
    aORegObj.insertName( DEFINE_CONST_UNICODE( "oreg.ini" ) );
    Config aCfg( aORegObj.PathToFileName() );
    aCfg.SetGroup( "reg" );
    sal_uInt16 nRegKey = (sal_uInt16) aCfg.ReadKey( "registration", "0" ).ToInt32();
    if( nRegKey == 0 )
        GetAppDispatcher_Impl()->Execute(SID_ONLINE_REGISTRATION_DLG, SFX_CALLMODE_ASYNCHRON);

    return 0;
}

