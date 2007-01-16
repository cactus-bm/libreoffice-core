/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: basidesh.cxx,v $
 *
 *  $Revision: 1.40 $
 *
 *  last change: $Author: vg $ $Date: 2007-01-16 16:29:49 $
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
#include "precompiled_basctl.hxx"

// CLOOKS:
//#define _MENUBTN_HXX
#define _SPIN_HXX
#define _PRVWIN_HXX
//#define _FIELD_HXX ***
//#define _TAB_HXX ***
#define _DIALOGS_HXX
#define _SVRTF_HXX
#define _ISETBRW_HXX
#define _VCTRLS_HXX
#define SI_NOCONTROL
#define SI_NOSBXCONTROLS

#define ITEMID_SIZE 0

// Falls ohne PCH's:
#include <ide_pch.hxx>


#define _SOLAR__PRIVATE 1

#ifndef _SBXCLASS_HXX //autogen
#include <basic/sbx.hxx>
#endif
#ifndef _SFXHINT_HXX //autogen
#include <svtools/hint.hxx>
#endif
#include <basidesh.hrc>
#include <basidesh.hxx>
#include <basdoc.hxx>
#include <basobj.hxx>
#include <bastypes.hxx>
#include <basicbox.hxx>
#include <objdlg.hxx>
#include <sbxitem.hxx>
#include <tbxctl.hxx>
#include <iderdll2.hxx>
#include <basidectrlr.hxx>
#include <localizationmgr.hxx>

#define BasicIDEShell
#define SFX_TYPEMAP
#include <idetemp.hxx>
#include <basslots.hxx>
#include <iderdll.hxx>
#include <svx/pszctrl.hxx>
#include <svx/insctrl.hxx>
#include <svx/srchdlg.hxx>
#include <svx/lboxctrl.hxx>
#include <svx/tbcontrl.hxx>

#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINER_HPP_
#include <com/sun/star/script/XLibraryContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINERPASSWORD_HPP_
#include <com/sun/star/script/XLibraryContainerPassword.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif

#include <svx/xmlsecctrl.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;
using namespace ::rtl;


TYPEINIT1( BasicIDEShell, SfxViewShell );

SFX_IMPL_VIEWFACTORY( BasicIDEShell, SVX_INTERFACE_BASIDE_VIEWSH )
{
    SFX_VIEW_REGISTRATION( BasicDocShell );
}


// MI: Prinzipiel IDL, aber ich lieber doch nicht?
// SFX_IMPL_ /*IDL_*/ INTERFACE( BasicIDEShell, SfxViewShell, IDEResId( RID_STR_IDENAME ) )
SFX_IMPL_INTERFACE( BasicIDEShell, SfxViewShell, IDEResId( RID_STR_IDENAME ) )
{
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_MACRO | SFX_VISIBILITY_STANDARD
        | SFX_VISIBILITY_FULLSCREEN | SFX_VISIBILITY_CLIENT, IDEResId( RID_BASICIDE_OBJECTBAR ) );
    SFX_CHILDWINDOW_REGISTRATION( SID_SEARCH_DLG );
    SFX_FEATURED_CHILDWINDOW_REGISTRATION(SID_SHOW_PROPERTYBROWSER, BASICIDE_UI_FEATURE_SHOW_BROWSER);
    SFX_POPUPMENU_REGISTRATION( IDEResId( RID_POPUP_DLGED ) );
}



#define IDE_VIEWSHELL_FLAGS     SFX_VIEW_MAXIMIZE_FIRST|SFX_VIEW_CAN_PRINT|SFX_VIEW_NO_NEWWINDOW


// Hack for #101048
static sal_Int32 GnBasicIDEShellCount;
sal_Int32 getBasicIDEShellCount( void )
    { return GnBasicIDEShellCount; }

BasicIDEShell::BasicIDEShell( SfxViewFrame *pFrame_, Window * ):
        SfxViewShell( pFrame_, IDE_VIEWSHELL_FLAGS ),
        aHScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_HSCROLL | WB_DRAG ) ),
        aVScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_VSCROLL | WB_DRAG ) ),
        aScrollBarBox( &GetViewFrame()->GetWindow(), WinBits( WB_SIZEABLE ) ),
        m_bAppBasicModified( FALSE )
{
    Init();
    GnBasicIDEShellCount++;
}


BasicIDEShell::BasicIDEShell( SfxViewFrame *pFrame_, const BasicIDEShell& ):
        SfxViewShell( pFrame_, IDE_VIEWSHELL_FLAGS ),
        aHScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_HSCROLL | WB_DRAG ) ),
        aVScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_VSCROLL | WB_DRAG ) ),
        aScrollBarBox( &GetViewFrame()->GetWindow(), WinBits( WB_SIZEABLE ) ),
        m_bAppBasicModified( FALSE )
{
    DBG_ERROR( "Zweite Ansicht auf Debugger nicht moeglich!" );
    GnBasicIDEShellCount++;
}


BasicIDEShell::BasicIDEShell( SfxViewFrame* pFrame_, SfxViewShell* /* pOldShell */ ) :
        SfxViewShell( pFrame_, IDE_VIEWSHELL_FLAGS ),
        aHScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_HSCROLL | WB_DRAG ) ),
        aVScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_VSCROLL | WB_DRAG ) ),
        aScrollBarBox( &GetViewFrame()->GetWindow(), WinBits( WB_SIZEABLE ) ),
        m_bAppBasicModified( FALSE )
{
    Init();
    GnBasicIDEShellCount++;
}



void BasicIDEShell::Init()
{
    TbxControls::RegisterControl( SID_CHOOSE_CONTROLS );
    SvxPosSizeStatusBarControl::RegisterControl();
    SvxInsertStatusBarControl::RegisterControl();
    XmlSecStatusBarControl::RegisterControl( SID_SIGNATURE );
    SvxSimpleUndoRedoController::RegisterControl( SID_UNDO );
    SvxSimpleUndoRedoController::RegisterControl( SID_REDO );

    SvxSearchDialogWrapper::RegisterChildWindow( sal_False );

    IDE_DLL()->GetExtraData()->ShellInCriticalSection() = TRUE;

    SetName( String( RTL_CONSTASCII_USTRINGPARAM( "BasicIDE" ) ) );
    SetHelpId( SVX_INTERFACE_BASIDE_VIEWSH );

    SFX_APP()->EnterBasicCall();

    LibBoxControl::RegisterControl( SID_BASICIDE_LIBSELECTOR );
    LanguageBoxControl::RegisterControl( SID_BASICIDE_CURRENT_LANG );

    CreateModulWindowLayout();

    StartListening( *SFX_APP(), TRUE /* Nur einmal anmelden */ );

    GetViewFrame()->GetWindow().SetBackground();

    pCurWin = 0;
    m_pCurShell = 0;
    pObjectCatalog = 0;
    bCreatingWindow = FALSE;

    m_pCurLocalizationMgr = NULL;

    // MT 08/00: BasicToolBar not longer in other ViewShells.
//  SFX_APP()->GetAppDispatcher().Push(*this);

    pTabBar = new BasicIDETabBar( &GetViewFrame()->GetWindow() );
    pTabBar->SetSplitHdl( LINK( this, BasicIDEShell, TabBarSplitHdl ) );
    bTabBarSplitted = FALSE;

    nCurKey = 100;
    InitScrollBars();
    InitTabBar();

    SetCurLib( 0, String::CreateFromAscii( "Standard" ), false, false );

    if ( IDE_DLL() && IDE_DLL()->pShell == NULL )
        IDE_DLL()->pShell = this;

    IDE_DLL()->GetExtraData()->ShellInCriticalSection() = FALSE;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > aTmpRef;
    GetViewFrame()->GetFrame()->GetFrameInterface()->setComponent( aTmpRef, new BasicIDEController( this ) );

    UpdateWindows();
}

__EXPORT BasicIDEShell::~BasicIDEShell()
{
    if ( IDE_DLL() && IDE_DLL()->pShell == this )
        IDE_DLL()->pShell = NULL;

    // Damit bei einem Basic-Fehler beim Speichern die Shell nicht sofort
    // wieder hoch kommt:
    IDE_DLL()->GetExtraData()->ShellInCriticalSection() = TRUE;

    SetWindow( 0 );
    SetCurWindow( 0 );
    SfxObjectShell::SetWorkingDocument( SfxObjectShell::Current() );

    // Alle Fenster zerstoeren:
    IDEBaseWindow* pWin = aIDEWindowTable.First();
    while ( pWin )
    {
        // Kein Store, passiert bereits, wenn die BasicManager zerstoert werden.
        delete pWin;
        pWin = aIDEWindowTable.Next();
    }

    aIDEWindowTable.Clear();
    delete pTabBar;
    delete pObjectCatalog;
    DestroyModulWindowLayout();
    // MI: Das gab einen GPF im SDT beim Schliessen da dann der ViewFrame die
    // ObjSh loslaesst. Es wusste auch keiner mehr wozu das gut war.
    // GetViewFrame()->GetObjectShell()->Broadcast( SfxSimpleHint( SFX_HINT_DYING ) );

    SFX_APP()->LeaveBasicCall();
    IDE_DLL()->GetExtraData()->ShellInCriticalSection() = FALSE;

    GnBasicIDEShellCount--;
}

sal_Bool BasicIDEShell::HasBasic() const
{
    return FALSE;
}

void BasicIDEShell::StoreAllWindowData( BOOL bPersistent )
{
    for ( ULONG nWin = 0; nWin < aIDEWindowTable.Count(); nWin++ )
    {
        IDEBaseWindow* pWin = aIDEWindowTable.GetObject( nWin );
        DBG_ASSERT( pWin, "PrepareClose: NULL-Pointer in Table?" );
        if ( !pWin->IsSuspended() )
            pWin->StoreData();
    }

    if ( bPersistent  )
    {
        SFX_APP()->SaveBasicContainer();
        SFX_APP()->SaveDialogContainer();
        SetAppBasicModified( FALSE );

        SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
        if ( pBindings )
        {
            pBindings->Invalidate( SID_SAVEDOC );
            pBindings->Update( SID_SAVEDOC );
        }
    }
}


USHORT __EXPORT BasicIDEShell::PrepareClose( BOOL bUI, BOOL bForBrowsing )
{
    (void)bForBrowsing;

    // da es nach Drucken etc. (DocInfo) modifiziert ist, hier resetten
    GetViewFrame()->GetObjectShell()->SetModified(FALSE);

    if ( StarBASIC::IsRunning() )
    {
        if( bUI )
        {
            String aErrorStr( IDEResId( RID_STR_CANNOTCLOSE ) );
            Window *pParent = &GetViewFrame()->GetWindow();
            InfoBox( pParent, aErrorStr ).Execute();
        }
        return FALSE;
    }
    else
    {
        // Hier unguenstig, wird zweimal gerufen...
//      StoreAllWindowData();

        BOOL bCanClose = TRUE;
        for ( ULONG nWin = 0; bCanClose && ( nWin < aIDEWindowTable.Count() ); nWin++ )
        {
            IDEBaseWindow* pWin = aIDEWindowTable.GetObject( nWin );
            if ( /* !pWin->IsSuspended() && */ !pWin->CanClose() )
            {
                if ( m_aCurLibName.Len() && ( pWin->GetShell() != m_pCurShell || pWin->GetLibName() != m_aCurLibName ) )
                    SetCurLib( 0, String(), false );
                SetCurWindow( pWin, TRUE );
                bCanClose = FALSE;
            }
        }

        if ( bCanClose )
            StoreAllWindowData( FALSE );    // Nicht auf Platte schreiben, das passiert am Ende automatisch

        return bCanClose;
    }
}

void BasicIDEShell::InitScrollBars()
{
    aVScrollBar.SetLineSize( 300 );
    aVScrollBar.SetPageSize( 2000 );
    aHScrollBar.SetLineSize( 300 );
    aHScrollBar.SetPageSize( 2000 );
    aHScrollBar.Enable();
    aVScrollBar.Enable();
    aVScrollBar.Show();
    aHScrollBar.Show();
    aScrollBarBox.Show();
}



void BasicIDEShell::InitTabBar()
{
    pTabBar->Enable();
    pTabBar->Show();
    pTabBar->SetSelectHdl( LINK( this, BasicIDEShell, TabBarHdl ) );
}


Size __EXPORT BasicIDEShell::GetOptimalSizePixel() const
{
    return Size( 400, 300 );
}



void __EXPORT BasicIDEShell::OuterResizePixel( const Point &rPos, const Size &rSize )
{
    // Adjust fliegt irgendwann raus...
    AdjustPosSizePixel( rPos, rSize );
}


IMPL_LINK_INLINE_START( BasicIDEShell, TabBarSplitHdl, TabBar *, pTBar )
{
    (void)pTBar;
    bTabBarSplitted = TRUE;
    ArrangeTabBar();

    return 0;
}
IMPL_LINK_INLINE_END( BasicIDEShell, TabBarSplitHdl, TabBar *, pTBar )



IMPL_LINK( BasicIDEShell, TabBarHdl, TabBar *, pCurTabBar )
{
    USHORT nCurId = pCurTabBar->GetCurPageId();
    IDEBaseWindow* pWin = aIDEWindowTable.Get( nCurId );
    DBG_ASSERT( pWin, "Eintrag in TabBar passt zu keinem Fenster!" );
    SetCurWindow( pWin );

    return 0;
}



BOOL BasicIDEShell::NextPage( BOOL bPrev )
{
    BOOL bRet = FALSE;
    USHORT nPos = pTabBar->GetPagePos( pTabBar->GetCurPageId() );

    if ( bPrev )
        --nPos;
    else
        ++nPos;

    if ( nPos < pTabBar->GetPageCount() )
    {
        IDEBaseWindow* pWin = aIDEWindowTable.Get( pTabBar->GetPageId( nPos ) );
        SetCurWindow( pWin, TRUE );
        bRet = TRUE;
    }

    return bRet;
}



void BasicIDEShell::ArrangeTabBar()
{
    Size aSz( GetViewFrame()->GetWindow().GetOutputSizePixel() );
    long nBoxPos = aScrollBarBox.GetPosPixel().X() - 1;
    long nPos = pTabBar->GetSplitSize();
    if ( nPos <= nBoxPos )
    {
        Point aPnt( pTabBar->GetPosPixel() );
        long nH = aHScrollBar.GetSizePixel().Height();
        pTabBar->SetPosSizePixel( aPnt, Size( nPos, nH ) );
        long nScrlStart = aPnt.X() + nPos;
        aHScrollBar.SetPosSizePixel( Point( nScrlStart, aPnt.Y() ), Size( nBoxPos - nScrlStart + 2, nH ) );
        aHScrollBar.Update();
    }
}



SfxUndoManager* BasicIDEShell::GetUndoManager()
{
    SfxUndoManager* pMgr = NULL;
    if( pCurWin )
        pMgr = pCurWin->GetUndoManager();

    return pMgr;
}



void BasicIDEShell::ShowObjectDialog( BOOL bShow, BOOL bCreateOrDestroy )
{
    if ( bShow )
    {
        if ( !pObjectCatalog && bCreateOrDestroy )
        {
            pObjectCatalog = new ObjectCatalog( &GetViewFrame()->GetWindow() );
            // Position wird in BasicIDEData gemerkt und vom Dlg eingestellt
            if ( pObjectCatalog )
            {
                pObjectCatalog->SetCancelHdl( LINK( this, BasicIDEShell, ObjectDialogCancelHdl ) );
                BasicEntryDescriptor aDesc;
                IDEBaseWindow* pCurWin_ = GetCurWindow();
                if ( pCurWin_ )
                    aDesc = pCurWin_->CreateEntryDescriptor();
                pObjectCatalog->SetCurrentEntry( aDesc );
            }
        }

        // Die allerletzten Aenderungen...
        if ( pCurWin )
            pCurWin->StoreData();

        if ( pObjectCatalog )
        {
            pObjectCatalog->UpdateEntries();
            pObjectCatalog->Show();
        }
    }
    else if ( pObjectCatalog )
    {
        pObjectCatalog->Hide();
        if ( bCreateOrDestroy )
        {
            // Wegen OS/2-Focus-Problem pObjectCatalog vorm delete auf NULL
            ObjectCatalog* pTemp = pObjectCatalog;
            pObjectCatalog = 0;
            delete pTemp;
        }
    }
}



void __EXPORT BasicIDEShell::SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId&,
                                        const SfxHint& rHint, const TypeId& )
{
    if ( IDE_DLL()->GetShell() )
    {
        if ( rHint.IsA( TYPE( SfxEventHint ) ) )
        {
            switch ( ((SfxEventHint&)rHint).GetEventId() )
            {
                case SFX_EVENT_CREATEDOC:
                case SFX_EVENT_OPENDOC:
                {
                    UpdateWindows();
                }
                break;
                case SFX_EVENT_SAVEDOC:
                case SFX_EVENT_SAVEASDOC:
                {
                    StoreAllWindowData();
                }
                break;
                case SFX_EVENT_CLOSEDOC:
                {
                    if ( rBC.IsA( TYPE( SfxObjectShell ) ) )
                    {
                        bool bSetCurWindow = false;
                        bool bSetCurLib = false;
                        SfxObjectShell* pShell = (SfxObjectShell*)&rBC;
                        Sequence< ::rtl::OUString > aLibNames = BasicIDE::GetLibraryNames( pShell );
                        sal_Int32 nLibCount = aLibNames.getLength();
                        const ::rtl::OUString* pLibNames = aLibNames.getConstArray();

                        for ( sal_Int32 i = 0 ; i < nLibCount ; ++i )
                        {
                            String aLibName = pLibNames[ i ];
                            if ( !aLibName.Len() )
                                continue;

                            // remove all windows which belong to this library
                            for ( ULONG nWin = aIDEWindowTable.Count(); nWin; )
                            {
                                IDEBaseWindow* pWin = aIDEWindowTable.GetObject( --nWin );
                                if ( pWin->GetShell() == pShell && pWin->GetLibName() == aLibName )
                                {
                                    if ( pWin->GetStatus() & (BASWIN_RUNNINGBASIC|BASWIN_INRESCHEDULE) )
                                    {
                                        pWin->AddStatus( BASWIN_TOBEKILLED );
                                        pWin->Hide();
                                        StarBASIC::Stop();
                                        // there's no notify
                                        pWin->BasicStopped();
                                    }
                                    else
                                    {
                                        pWin->StoreData();
                                        if ( pWin == pCurWin )
                                            bSetCurWindow = true;
                                        RemoveWindow( pWin, TRUE, FALSE );
                                    }
                                }
                            }

                            // remove lib info
                            BasicIDEData* pData = IDE_DLL()->GetExtraData();
                            if ( pData )
                                pData->GetLibInfos().RemoveInfo( LibInfoKey( pShell, aLibName ) );

                            if ( pShell == m_pCurShell && aLibName == m_aCurLibName )
                                bSetCurLib = true;
                        }

                        if ( bSetCurLib )
                            SetCurLib( 0, String::CreateFromAscii( "Standard" ), true, false );
                        else if ( bSetCurWindow )
                            SetCurWindow( FindWindow(), TRUE );

                        EndListening( rBC, TRUE );
                    }
                }
                break;
            }
        }

        if ( rHint.IsA( TYPE( SfxSimpleHint ) ) )
        {
            switch ( ((SfxSimpleHint&)rHint).GetId() )
            {
                case SFX_HINT_DOCCHANGED:
                {
                    // If a document is modified, there's no need for any action
                    // within the BasicIDE.
                    //
                    // If a document is reloaded, the old document is destroyed
                    // and a new document is loaded. The BasicIDE is first notified
                    // with a SFX_HINT_DYING and then with a SFX_EVENT_OPENDOC.
                    // The BasicIDE windows are destroyed after the SFX_HINT_DYING
                    // notification and new windows are created after the
                    // SFX_EVENT_OPENDOC notification.

                    //UpdateWindows();
                }
                break;
                case SFX_HINT_MODECHANGED:
                {
                    // ReadOnly toggled...
                    if ( rBC.IsA( TYPE( SfxObjectShell ) ) )
                    {
                        SfxObjectShell* pShell = (SfxObjectShell*)&rBC;
                        for ( ULONG nWin = aIDEWindowTable.Count(); nWin; )
                        {
                            IDEBaseWindow* pWin = aIDEWindowTable.GetObject( --nWin );
                            if ( pWin->GetShell() == pShell )
                                pWin->SetReadOnly( pShell->IsReadOnly() );
                        }
                    }
                }
                break;
                case SFX_HINT_TITLECHANGED:
                {
                    SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
                    if ( pBindings )
                        pBindings->Invalidate( SID_BASICIDE_LIBSELECTOR, TRUE, FALSE );
                    SetMDITitle();
                }
                break;
                case SFX_HINT_DYING:
                {
                    EndListening( rBC, TRUE /* Alle abmelden */ );
                    if ( pObjectCatalog )
                        pObjectCatalog->UpdateEntries();
                }
                break;
            }

            if ( rHint.IsA( TYPE( SbxHint ) ) )
            {
                SbxHint& rSbxHint = (SbxHint&)rHint;
                ULONG nHintId = rSbxHint.GetId();
                if (    ( nHintId == SBX_HINT_BASICSTART ) ||
                        ( nHintId == SBX_HINT_BASICSTOP ) )
                {
                    SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
                    if ( pBindings )
                    {
                        pBindings->Invalidate( SID_BASICRUN );
                        pBindings->Update( SID_BASICRUN );
                        pBindings->Invalidate( SID_BASICCOMPILE );
                        pBindings->Update( SID_BASICCOMPILE );
                        pBindings->Invalidate( SID_BASICSTEPOVER );
                        pBindings->Update( SID_BASICSTEPOVER );
                        pBindings->Invalidate( SID_BASICSTEPINTO );
                        pBindings->Update( SID_BASICSTEPINTO );
                        pBindings->Invalidate( SID_BASICSTEPOUT );
                        pBindings->Update( SID_BASICSTEPOUT );
                        pBindings->Invalidate( SID_BASICSTOP );
                        pBindings->Update( SID_BASICSTOP );
                        pBindings->Invalidate( SID_BASICIDE_TOGGLEBRKPNT );
                        pBindings->Update( SID_BASICIDE_TOGGLEBRKPNT );
                        pBindings->Invalidate( SID_BASICIDE_MANAGEBRKPNTS );
                        pBindings->Update( SID_BASICIDE_MANAGEBRKPNTS );
                        pBindings->Invalidate( SID_BASICIDE_MODULEDLG );
                        pBindings->Update( SID_BASICIDE_MODULEDLG );
                        pBindings->Invalidate( SID_BASICLOAD );
                        pBindings->Update( SID_BASICLOAD );
                    }

                    if ( nHintId == SBX_HINT_BASICSTOP )
                    {
                        // Nicht nur bei Error/Break oder explizitem anhalten,
                        // falls durch einen Programmierfehler das Update abgeschaltet ist.
                        BasicIDE::BasicStopped();
                        UpdateModulWindowLayout( true );    // Leer machen...
                        if( m_pCurLocalizationMgr )
                            m_pCurLocalizationMgr->handleBasicStopped();
                    }
                    else if( m_pCurLocalizationMgr )
                    {
                        m_pCurLocalizationMgr->handleBasicStarted();
                    }

                    IDEBaseWindow* pWin = aIDEWindowTable.First();
                    while ( pWin )
                    {
                        if ( nHintId == SBX_HINT_BASICSTART )
                            pWin->BasicStarted();
                        else
                            pWin->BasicStopped();
                        pWin = aIDEWindowTable.Next();
                    }
                }
            }
        }
    }
}



void BasicIDEShell::CheckWindows()
{
    BOOL bSetCurWindow = FALSE;
    for ( ULONG nWin = 0; nWin < aIDEWindowTable.Count(); nWin++ )
    {
        IDEBaseWindow* pWin = aIDEWindowTable.GetObject( nWin );
        if ( pWin->GetStatus() & BASWIN_TOBEKILLED )
        {
            pWin->StoreData();
            if ( pWin == pCurWin )
                bSetCurWindow = TRUE;
            RemoveWindow( pWin, TRUE, FALSE );
            nWin--;
        }
    }
    if ( bSetCurWindow )
        SetCurWindow( FindWindow(), TRUE );
}



void BasicIDEShell::RemoveWindows( SfxObjectShell* pShell, const String& rLibName, BOOL bDestroy )
{
    BOOL bChangeCurWindow = pCurWin ? FALSE : TRUE;
    for ( ULONG nWin = 0; nWin < aIDEWindowTable.Count(); nWin++ )
    {
        IDEBaseWindow* pWin = aIDEWindowTable.GetObject( nWin );
        if ( pWin->GetShell() == pShell && pWin->GetLibName() == rLibName )
        {
            if ( pWin == pCurWin )
                bChangeCurWindow = TRUE;
            pWin->StoreData();
            RemoveWindow( pWin, bDestroy, FALSE );
            nWin--;
        }
    }
    if ( bChangeCurWindow )
        SetCurWindow( FindWindow(), TRUE );
}



void BasicIDEShell::UpdateWindows()
{
    // Alle Fenster, die nicht angezeigt werden duerfen, entfernen
    BOOL bChangeCurWindow = pCurWin ? FALSE : TRUE;
    if ( m_aCurLibName.Len() )
    {
        for ( ULONG nWin = 0; nWin < aIDEWindowTable.Count(); nWin++ )
        {
            IDEBaseWindow* pWin = aIDEWindowTable.GetObject( nWin );
            if ( pWin->GetShell() != m_pCurShell || pWin->GetLibName() != m_aCurLibName )
            {
                if ( pWin == pCurWin )
                    bChangeCurWindow = TRUE;
                pWin->StoreData();
                // Die Abfrage auf RUNNING verhindert den Absturz, wenn in Reschedule.
                // Fenster bleibt erstmal stehen, spaeter sowieso mal umstellen,
                // dass Fenster nur als Hidden markiert werden und nicht
                // geloescht.
                if ( !(pWin->GetStatus() & ( BASWIN_TOBEKILLED | BASWIN_RUNNINGBASIC | BASWIN_SUSPENDED ) ) )
                {
                    RemoveWindow( pWin, FALSE, FALSE );
                    nWin--;
                }
            }
        }
    }

    if ( bCreatingWindow )
        return;

    IDEBaseWindow* pNextActiveWindow = 0;

    // Alle anzuzeigenden Fenster anzeigen
    BasicManager* pBasicMgr = SFX_APP()->GetBasicManager();
    SfxObjectShell* pDocShell = 0;
    while ( pBasicMgr )
    {
        // Nur, wenn es ein dazugehoeriges Fenster gibt, damit nicht die
        // Gecachten Docs, die nicht sichtbar sind ( Remot-Dokumente )
        if ( !pDocShell || ( ( pBasicMgr != SFX_APP()->GetBasicManager() )
                                && ( SfxViewFrame::GetFirst( pDocShell ) ) ) )
        {
            StartListening( *pBasicMgr, TRUE /* Nur einmal anmelden */ );
            if ( pDocShell )
                StartListening( *pDocShell, TRUE );

            // libraries
            Sequence< ::rtl::OUString > aLibNames = BasicIDE::GetLibraryNames( pDocShell );
            sal_Int32 nLibCount = aLibNames.getLength();
            const ::rtl::OUString* pLibNames = aLibNames.getConstArray();

            for ( sal_Int32 i = 0 ; i < nLibCount ; i++ )
            {
                String aLibName = pLibNames[ i ];
                ::rtl::OUString aOULibName( aLibName );

                if ( !m_aCurLibName.Len() || ( pDocShell == m_pCurShell && aLibName == m_aCurLibName ) )
                {
                    // check, if library is password protected and not verified
                    BOOL bProtected = FALSE;
                    Reference< script::XLibraryContainer > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pDocShell ), UNO_QUERY );
                    if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) )
                    {
                        Reference< script::XLibraryContainerPassword > xPasswd( xModLibContainer, UNO_QUERY );
                        if ( xPasswd.is() && xPasswd->isLibraryPasswordProtected( aOULibName ) && !xPasswd->isLibraryPasswordVerified( aOULibName ) )
                        {
                            bProtected = TRUE;
                        }
                    }

                    if ( !bProtected )
                    {
                        LibInfoItem* pLibInfoItem = 0;
                        BasicIDEData* pData = IDE_DLL()->GetExtraData();
                        if ( pData )
                            pLibInfoItem = pData->GetLibInfos().GetInfo( LibInfoKey( pDocShell, aLibName ) );

                        // modules
                        if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) )
                        {
                            StarBASIC* pLib = pBasicMgr->GetLib( aLibName );
                            if ( pLib )
                                ImplStartListening( pLib );

                            try
                            {
                                Sequence< ::rtl::OUString > aModNames = BasicIDE::GetModuleNames( pDocShell, aLibName );
                                sal_Int32 nModCount = aModNames.getLength();
                                const ::rtl::OUString* pModNames = aModNames.getConstArray();

                                for ( sal_Int32 j = 0 ; j < nModCount ; j++ )
                                {
                                    String aModName = pModNames[ j ];
                                    ModulWindow* pWin = FindBasWin( pDocShell, aLibName, aModName, FALSE );
                                    if ( !pWin )
                                        pWin = CreateBasWin( pDocShell, aLibName, aModName );
                                    if ( !pNextActiveWindow && pLibInfoItem && pLibInfoItem->GetCurrentName() == aModName &&
                                         pLibInfoItem->GetCurrentType() == BASICIDE_TYPE_MODULE )
                                    {
                                        pNextActiveWindow = (IDEBaseWindow*)pWin;
                                    }
                                }
                            }
                            catch ( container::NoSuchElementException& e )
                            {
                                ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
                                DBG_ERROR( aBStr.GetBuffer() );
                            }
                        }

                        // dialogs
                        Reference< script::XLibraryContainer > xDlgLibContainer( BasicIDE::GetDialogLibraryContainer( pDocShell ), UNO_QUERY );
                        if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) )
                        {
                            try
                            {
                                Sequence< ::rtl::OUString > aDlgNames = BasicIDE::GetDialogNames( pDocShell, aLibName );
                                sal_Int32 nDlgCount = aDlgNames.getLength();
                                const ::rtl::OUString* pDlgNames = aDlgNames.getConstArray();

                                for ( sal_Int32 j = 0 ; j < nDlgCount ; j++ )
                                {
                                    String aDlgName = pDlgNames[ j ];
                                    // this find only looks for non-suspended windows;
                                    // suspended windows are handled in CreateDlgWin
                                    DialogWindow* pWin = FindDlgWin( pDocShell, aLibName, aDlgName, FALSE );
                                    if ( !pWin )
                                        pWin = CreateDlgWin( pDocShell, aLibName, aDlgName );
                                    if ( !pNextActiveWindow && pLibInfoItem && pLibInfoItem->GetCurrentName() == aDlgName &&
                                         pLibInfoItem->GetCurrentType() == BASICIDE_TYPE_DIALOG )
                                    {
                                        pNextActiveWindow = (IDEBaseWindow*)pWin;
                                    }
                                }
                            }
                            catch ( container::NoSuchElementException& e )
                            {
                                ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
                                DBG_ERROR( aBStr.GetBuffer() );
                            }
                        }
                    }
                }
            }
        }

        if ( pDocShell  )
            pDocShell = SfxObjectShell::GetNext( *pDocShell );
        else
            pDocShell = SfxObjectShell::GetFirst();

        pBasicMgr = ( pDocShell ? pDocShell->GetBasicManager() : 0 );
    }

    if ( bChangeCurWindow )
    {
        if ( !pNextActiveWindow )
            pNextActiveWindow = FindWindow();
        SetCurWindow( pNextActiveWindow, TRUE );
    }
}

void BasicIDEShell::RemoveWindow( IDEBaseWindow* pWindow_, BOOL bDestroy, BOOL bAllowChangeCurWindow )
{
    DBG_ASSERT( pWindow_, "Kann keinen NULL-Pointer loeschen!" );
    ULONG nKey = aIDEWindowTable.GetKey( pWindow_ );
    pTabBar->RemovePage( (USHORT)nKey );
    aIDEWindowTable.Remove( nKey );
    if ( pWindow_ == pCurWin )
    {
        if ( bAllowChangeCurWindow )
            SetCurWindow( FindWindow(), TRUE );
        else
            SetCurWindow( NULL, FALSE );
    }
    if ( bDestroy )
    {
        if ( !( pWindow_->GetStatus() & BASWIN_INRESCHEDULE ) )
        {
            delete pWindow_;
        }
        else
        {
            pWindow_->AddStatus( BASWIN_TOBEKILLED );
            pWindow_->Hide();
            StarBASIC::Stop();
            // Es kommt kein Notify...
            pWindow_->BasicStopped();
            aIDEWindowTable.Insert( nKey, pWindow_ );   // wieder einhaegen
        }
    }
    else
    {
        pWindow_->Hide();
        pWindow_->AddStatus( BASWIN_SUSPENDED );
        pWindow_->Deactivating();
        aIDEWindowTable.Insert( nKey, pWindow_ );   // wieder einhaegen
    }

}



USHORT BasicIDEShell::InsertWindowInTable( IDEBaseWindow* pNewWin )
{
    // Eigentlich prueffen,
    nCurKey++;
    aIDEWindowTable.Insert( nCurKey, pNewWin );
    return nCurKey;
}



void BasicIDEShell::InvalidateBasicIDESlots()
{
    // Nur die, die eine optische Auswirkung haben...

    if ( IDE_DLL()->GetShell() )
    {
        SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
        if ( pBindings )
        {
            pBindings->Invalidate( SID_UNDO );
            pBindings->Invalidate( SID_REDO );
            pBindings->Invalidate( SID_SAVEDOC );
            pBindings->Invalidate( SID_SIGNATURE );
            pBindings->Invalidate( SID_BASICIDE_CHOOSEMACRO );
            pBindings->Invalidate( SID_BASICIDE_MODULEDLG );
            pBindings->Invalidate( SID_BASICIDE_OBJCAT );
            pBindings->Invalidate( SID_BASICSTOP );
            pBindings->Invalidate( SID_BASICRUN );
            pBindings->Invalidate( SID_BASICCOMPILE );
            pBindings->Invalidate( SID_BASICLOAD );
            pBindings->Invalidate( SID_BASICSAVEAS );
            pBindings->Invalidate( SID_BASICIDE_MATCHGROUP );
            pBindings->Invalidate( SID_BASICSTEPINTO );
            pBindings->Invalidate( SID_BASICSTEPOVER );
            pBindings->Invalidate( SID_BASICSTEPOUT );
            pBindings->Invalidate( SID_BASICIDE_TOGGLEBRKPNT );
            pBindings->Invalidate( SID_BASICIDE_MANAGEBRKPNTS );
            pBindings->Invalidate( SID_BASICIDE_ADDWATCH );
            pBindings->Invalidate( SID_BASICIDE_REMOVEWATCH );
            pBindings->Invalidate( SID_CHOOSE_CONTROLS );
            pBindings->Invalidate( SID_PRINTDOC );
            pBindings->Invalidate( SID_PRINTDOCDIRECT );
            pBindings->Invalidate( SID_SETUPPRINTER );
            pBindings->Invalidate( SID_DIALOG_TESTMODE );

            pBindings->Invalidate( SID_DOC_MODIFIED );
            pBindings->Invalidate( SID_BASICIDE_STAT_TITLE );
            pBindings->Invalidate( SID_BASICIDE_STAT_POS );
            pBindings->Invalidate( SID_ATTR_INSERT );
            pBindings->Invalidate( SID_ATTR_SIZE );
        }
    }
}

void BasicIDEShell::EnableScrollbars( BOOL bEnable )
{
    if ( bEnable )
    {
        aHScrollBar.Enable();
        aVScrollBar.Enable();
    }
    else
    {
        aHScrollBar.Disable();
        aVScrollBar.Disable();
    }
}

void BasicIDEShell::SetCurLib( SfxObjectShell* pShell, String aLibName, bool bUpdateWindows, bool bCheck )
{
    if ( !bCheck || ( pShell != m_pCurShell || aLibName != m_aCurLibName ) )
    {
        m_pCurShell = pShell;
        m_aCurLibName = aLibName;
        if ( bUpdateWindows )
            UpdateWindows();

        SetMDITitle();

        SetCurLibForLocalization( pShell, aLibName );

        SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
        if ( pBindings )
        {
            pBindings->Invalidate( SID_BASICIDE_LIBSELECTOR );
            pBindings->Invalidate( SID_BASICIDE_CURRENT_LANG );
            pBindings->Invalidate( SID_BASICIDE_MANAGE_LANG );
        }
    }
}

void BasicIDEShell::SetCurLibForLocalization( SfxObjectShell* pShell, String aLibName )
{
    // Create LocalizationMgr
    delete m_pCurLocalizationMgr;
    Reference< resource::XStringResourceManager > xStringResourceManager;
    if( aLibName.Len() )
    {
        Reference< container::XNameContainer > xDialogLib =
            BasicIDE::GetDialogLibrary( pShell, aLibName, TRUE );

        xStringResourceManager = LocalizationMgr::getStringResourceFromDialogLibrary( xDialogLib );
    }
    m_pCurLocalizationMgr = new LocalizationMgr
        ( this, pShell, aLibName, xStringResourceManager );

    m_pCurLocalizationMgr->handleTranslationbar();
}

void BasicIDEShell::ImplStartListening( StarBASIC* pBasic )
{
    StartListening( pBasic->GetBroadcaster(), TRUE /* Nur einmal anmelden */ );
}


