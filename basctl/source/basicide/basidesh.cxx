/*************************************************************************
 *
 *  $RCSfile: basidesh.cxx,v $
 *
 *  $Revision: 1.23 $
 *
 *  last change: $Author: ab $ $Date: 2002-10-30 09:35:47 $
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

#pragma hdrstop

#define _SOLAR__PRIVATE 1

#ifndef _SBXCLASS_HXX //autogen
#include <svtools/sbx.hxx>
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

#define BasicIDEShell
#define SFX_TYPEMAP
#include <idetemp.hxx>
#include <basslots.hxx>
#include <iderdll.hxx>
#include <svx/pszctrl.hxx>
#include <svx/insctrl.hxx>

#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINER_HPP_
#include <com/sun/star/script/XLibraryContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINERPASSWORD_HPP_
#include <com/sun/star/script/XLibraryContainerPassword.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

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

BasicIDEShell::BasicIDEShell( SfxViewFrame *pFrame, Window * ):
        SfxViewShell( pFrame, IDE_VIEWSHELL_FLAGS ),
        aHScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_HSCROLL | WB_DRAG ) ),
        aVScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_VSCROLL | WB_DRAG ) ),
        aScrollBarBox( &GetViewFrame()->GetWindow(), WinBits( WB_SIZEABLE ) ),
        m_bAppBasicModified( FALSE )
{
    Init();
    GnBasicIDEShellCount++;
}


BasicIDEShell::BasicIDEShell( SfxViewFrame *pFrame, const BasicIDEShell & rView):
        SfxViewShell( pFrame, IDE_VIEWSHELL_FLAGS ),
        aHScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_HSCROLL | WB_DRAG ) ),
        aVScrollBar( &GetViewFrame()->GetWindow(), WinBits( WB_VSCROLL | WB_DRAG ) ),
        aScrollBarBox( &GetViewFrame()->GetWindow(), WinBits( WB_SIZEABLE ) ),
        m_bAppBasicModified( FALSE )
{
    DBG_ERROR( "Zweite Ansicht auf Debugger nicht moeglich!" );
    GnBasicIDEShellCount++;
}


BasicIDEShell::BasicIDEShell( SfxViewFrame* pFrame, SfxViewShell* /* pOldShell */ ) :
        SfxViewShell( pFrame, IDE_VIEWSHELL_FLAGS ),
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

    IDE_DLL()->GetExtraData()->ShellInCriticalSection() = TRUE;

    SetName( String( RTL_CONSTASCII_USTRINGPARAM( "BasicIDE" ) ) );
    SetHelpId( SVX_INTERFACE_BASIDE_VIEWSH );

    SFX_APP()->EnterBasicCall();

    LibBoxControl::RegisterControl( SID_BASICIDE_LIBSELECTOR );

    CreateModulWindowLayout();

    StartListening( *SFX_APP(), TRUE /* Nur einmal anmelden */ );

    GetViewFrame()->GetWindow().SetBackground();

    pCurWin = 0;
    pCurBasic = 0;
    pObjectCatalog = 0;
    bCreatingWindow = FALSE;

    // MT 08/00: BasicToolBar not longer in other ViewShells.
//  SFX_APP()->GetAppDispatcher().Push(*this);

    pTabBar = new BasicIDETabBar( &GetViewFrame()->GetWindow() );
    pTabBar->SetSplitHdl( LINK( this, BasicIDEShell, TabBarSplitHdl ) );
    bTabBarSplitted = FALSE;

    IDEBaseWindow* pTmpWin = 0;
    nCurKey = 100;
    InitScrollBars();
    InitTabBar();

    // Nicht 0, wird sonst beim naechsten Request nicht auf das entspr. Basic gesetzt.
    SetCurBasic( SFX_APP()->GetBasicManager()->GetStdLib(), FALSE );

    IDE_DLL()->pShell = this;
    IDE_DLL()->GetExtraData()->ShellInCriticalSection() = FALSE;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > aTmpRef;
    GetViewFrame()->GetFrame()->GetFrameInterface()->setComponent( aTmpRef, new BasicIDEController( this ) );

    UpdateWindows();
}

__EXPORT BasicIDEShell::~BasicIDEShell()
{
    IDE_DLL()->pShell = NULL;

    // Damit bei einem Basic-Fehler beim Speichern die Shell nicht sofort
    // wieder hoch kommt:
    IDE_DLL()->GetExtraData()->ShellInCriticalSection() = TRUE;

    SetWindow( 0 );
    SetCurWindow( 0 );

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

        SfxBindings& rBindings = BasicIDE::GetBindings();
        rBindings.Invalidate( SID_SAVEDOC );
        rBindings.Update( SID_SAVEDOC );
    }
}


USHORT __EXPORT BasicIDEShell::PrepareClose( BOOL bUI, BOOL bForBrowsing )
{
    // da es nach Drucken etc. (DocInfo) modifiziert ist, hier resetten
    GetViewFrame()->GetObjectShell()->SetModified(FALSE);

    if ( StarBASIC::IsRunning() )
    {
        String aErrorStr( IDEResId( RID_STR_CANNOTCLOSE ) );
        Window *pParent = &GetViewFrame()->GetWindow();
        InfoBox( pParent, aErrorStr ).Execute();
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
                if ( pCurBasic && ( pWin->GetBasic() != pCurBasic ) )
                    SetCurBasic( 0, FALSE );
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
    ((BasicIDETabBar*)pCurTabBar)->SetCurrentLib( pWin->GetBasic() );

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

    if ( nPos >= 0 && nPos < pTabBar->GetPageCount() )
    {
        IDEBaseWindow* pWin = aIDEWindowTable.Get( pTabBar->GetPageId( nPos ) );
        SetCurWindow( pWin, TRUE );
        pTabBar->SetCurrentLib( pWin->GetBasic() );
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
            pObjectCatalog->SetCancelHdl( LINK( this, BasicIDEShell, ObjectDialogCancelHdl ) );
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
            if ( ( ((SfxEventHint&)rHint).GetEventId() == SFX_EVENT_CREATEDOC ) ||
                 ( ((SfxEventHint&)rHint).GetEventId() == SFX_EVENT_OPENDOC ) )
            {
                UpdateWindows();
            }

            if ( ( ((SfxEventHint&)rHint).GetEventId() == SFX_EVENT_SAVEDOC ) ||
                 ( ((SfxEventHint&)rHint).GetEventId() == SFX_EVENT_SAVEASDOC ) )
            {
                StoreAllWindowData();
            }

            // TODO: move code from SFX_HINT_DYING to SFX_EVENT_CLOSEDOC?
            // (in SFX_HINT_DYING the SfxObjectShell is already destroyed)
            /*
            if ( ((SfxEventHint&)rHint).GetEventId() == SFX_EVENT_CLOSEDOC )
            {
                if ( rBC.IsA( TYPE( SfxObjectShell ) ) )
                {
                    SfxObjectShell* pShell = (SfxObjectShell*)&rBC;
                    BasicManager* pBasMgr = pShell->GetBasicManager();
                }
            }
            */
        }
        if ( rHint.IsA( TYPE( SfxSimpleHint ) ) )
        {
            if ( ((SfxSimpleHint&)rHint).GetId() == SFX_HINT_DOCCHANGED )
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
            else if ( ((SfxSimpleHint&)rHint).GetId() == SFX_HINT_MODECHANGED )
            {
                // ReadOnly toggled...
                if ( rBC.IsA( TYPE( SfxObjectShell ) ) )
                {
                    SfxObjectShell* pShell = (SfxObjectShell*)&rBC;
                    BasicManager* pBasMgr = pShell->GetBasicManager();
                    for ( ULONG nWin = aIDEWindowTable.Count(); nWin; )
                    {
                        IDEBaseWindow* pWin = aIDEWindowTable.GetObject( --nWin );
                        BasicManager* pM = BasicIDE::FindBasicManager( pWin->GetBasic() );
                        if ( pM == pBasMgr )
                            pWin->SetReadOnly( pShell->IsReadOnly() );
                    }
                }

            }
            else if ( ((SfxSimpleHint&)rHint).GetId() == SFX_HINT_TITLECHANGED )
            {
                BasicIDE::GetBindings().Invalidate( SID_BASICIDE_LIBSELECTOR, TRUE, FALSE );
                SetMDITitle();
            }
            else if ( ((SfxSimpleHint&)rHint).GetId() == SFX_HINT_DYING )
            {
                // Alle Fenster suchen, die auf einem Basic des Managers arbeiten,
                // Daten sichern, Fenster zerstoeren
                if ( rBC.IsA( TYPE( BasicManager ) ) )
                {
                    BOOL bSetCurWindow = FALSE;
                    BOOL bSetCurBasic = FALSE;
                    BasicManager* pBasMgr = (BasicManager* )&rBC;
                    USHORT nLibs = pBasMgr->GetLibCount();
                    // Alle Fenster, die ein Object dieser Lib anzeigen, schliessen
                    for ( USHORT nLib = 0; nLib < nLibs; nLib++ )
                    {
                        StarBASIC* pLib = pBasMgr->GetLib( nLib );
                        if ( !pLib )
                            continue;

                        for ( ULONG nWin = aIDEWindowTable.Count(); nWin; )
                        {
                            IDEBaseWindow* pWin = aIDEWindowTable.GetObject( --nWin );
                            if ( pWin->GetBasic() == pLib )
                            {
                                if ( pWin->GetStatus() & (BASWIN_RUNNINGBASIC|BASWIN_INRESCHEDULE) )
                                {
                                    pWin->AddStatus( BASWIN_TOBEKILLED );
                                    pWin->Hide();
                                    StarBASIC::Stop();
                                    // Es kommt kein Notify...
                                    pWin->BasicStopped();
                                }
                                else
                                {
                                    pWin->StoreData();
                                    if ( pWin == pCurWin )
                                        bSetCurWindow = TRUE;
                                    RemoveWindow( pWin, TRUE, FALSE );
                                }
                            }
                        }
                        if ( pLib == pCurBasic )
                            bSetCurBasic = TRUE;
                    }
                    if ( bSetCurBasic )
                        SetCurBasic( SFX_APP()->GetBasic(), TRUE );
                    else if ( bSetCurWindow )
                    {
                        IDEBaseWindow* pWin = FindWindow( 0 );
                        SetCurWindow( pWin, TRUE );
                    }
                }
                else if ( rBC.IsA( TYPE( StarBASIC ) ) )
                {
                    StarBASIC* pLib = (StarBASIC* )&rBC;
                    IDE_DLL()->GetExtraData()->GetLibInfos().DestroyInfo( pLib );
                }
                EndListening( rBC, TRUE /* Alle abmelden */ );
            }
            if ( rHint.IsA( TYPE( SbxHint ) ) )
            {
                SbxHint& rSbxHint = (SbxHint&)rHint;
                ULONG nHintId = rSbxHint.GetId();
                if (    ( nHintId == SBX_HINT_BASICSTART ) ||
                        ( nHintId == SBX_HINT_BASICSTOP ) )
                {
                    SfxBindings& rBindings = BasicIDE::GetBindings();
                    rBindings.Invalidate( SID_BASICRUN );
                    rBindings.Update( SID_BASICRUN );
                    rBindings.Invalidate( SID_BASICCOMPILE );
                    rBindings.Update( SID_BASICCOMPILE );
                    rBindings.Invalidate( SID_BASICSTEPOVER );
                    rBindings.Update( SID_BASICSTEPOVER );
                    rBindings.Invalidate( SID_BASICSTEPINTO );
                    rBindings.Update( SID_BASICSTEPINTO );
                    rBindings.Invalidate( SID_BASICSTEPOUT );
                    rBindings.Update( SID_BASICSTEPOUT );
                    rBindings.Invalidate( SID_BASICSTOP );
                    rBindings.Update( SID_BASICSTOP );
                    rBindings.Invalidate( SID_BASICIDE_TOGGLEBRKPNT );
                    rBindings.Update( SID_BASICIDE_TOGGLEBRKPNT );
                    rBindings.Invalidate( SID_BASICIDE_MODULEDLG );
                    rBindings.Update( SID_BASICIDE_MODULEDLG );
                    rBindings.Invalidate( SID_BASICLOAD );
                    rBindings.Update( SID_BASICLOAD );

                    if ( nHintId == SBX_HINT_BASICSTOP )
                    {
                        // Nicht nur bei Error/Break oder explizitem anhalten,
                        // falls durch einen Programmierfehler das Update abgeschaltet ist.
                        BasicIDE::BasicStopped();
                        UpdateModulWindowLayout();  // Leer machen...
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
        SetCurWindow( FindWindow( 0 ), TRUE );
}



void BasicIDEShell::RemoveWindows( StarBASIC* pBasic, BOOL bDestroy )
{
    BOOL bChangeCurWindow = pCurWin ? FALSE : TRUE;
    for ( ULONG nWin = 0; nWin < aIDEWindowTable.Count(); nWin++ )
    {
        IDEBaseWindow* pWin = aIDEWindowTable.GetObject( nWin );
        if ( pWin->GetBasic() == pBasic )
        {
            if ( pWin == pCurWin )
                bChangeCurWindow = TRUE;
            pWin->StoreData();
            RemoveWindow( pWin, bDestroy, FALSE );
            nWin--;
        }
    }
    if ( bChangeCurWindow )
        SetCurWindow( FindWindow( 0 ), TRUE );
}



void BasicIDEShell::UpdateWindows()
{
    // Alle Fenster, die nicht angezeigt werden duerfen, entfernen
    BOOL bChangeCurWindow = pCurWin ? FALSE : TRUE;
    if ( pCurBasic )
    {
        for ( ULONG nWin = 0; nWin < aIDEWindowTable.Count(); nWin++ )
        {
            IDEBaseWindow* pWin = aIDEWindowTable.GetObject( nWin );
            if ( pWin->GetBasic() != pCurBasic )
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

    SbModule* pNextActiveModule = NULL;

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
                // check, if library is password protected and not verified
                BOOL bProtected = FALSE;
                String aLibName = pLibNames[ i ];
                ::rtl::OUString aOULibName( aLibName );
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
                    StarBASIC* pLib = pBasicMgr->GetLib( aLibName );

                    if ( pLib && ( !pCurBasic || ( pLib == pCurBasic ) ) )
                    {
                        ImplStartListening( pLib );

                        LibInfo* pLibInf =  IDE_DLL()->GetExtraData()->GetLibInfos().GetInfo( pLib );

                        // modules
                        try
                        {
                            Sequence< ::rtl::OUString > aModNames = BasicIDE::GetModuleNames( pDocShell, aLibName );
                            sal_Int32 nModCount = aModNames.getLength();
                            const ::rtl::OUString* pModNames = aModNames.getConstArray();

                            for ( sal_Int32 j = 0 ; j < nModCount ; j++ )
                            {
                                String aModName = pModNames[ j ];
                                if ( !FindBasWin( pLib, aModName, FALSE ) )
                                    CreateBasWin( pLib, aModName );

                                if ( pLibInf && !pNextActiveModule && ( aModName == pLibInf->aCurrentModule ) )
                                    pNextActiveModule = pLib->FindModule( aModName );
                            }
                        }
                        catch ( container::NoSuchElementException& e )
                        {
                            ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
                            DBG_ERROR( aBStr.GetBuffer() );
                        }

                        // dialogs
                        try
                        {
                            Sequence< ::rtl::OUString > aDlgNames = BasicIDE::GetDialogNames( pDocShell, aLibName );
                            sal_Int32 nDlgCount = aDlgNames.getLength();
                            const ::rtl::OUString* pDlgNames = aDlgNames.getConstArray();

                            for ( sal_Int32 j = 0 ; j < nDlgCount ; j++ )
                            {
                                String aDlgName = pDlgNames[ j ];
                                if ( !FindDlgWin( pLib, aDlgName, FALSE ) ) // this find only looks for non-suspended windows;
                                    CreateDlgWin( pLib, aDlgName );         // suspended windows are handled in CreateDlgWin
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

        if ( pDocShell  )
            pDocShell = SfxObjectShell::GetNext( *pDocShell );
        else
            pDocShell = SfxObjectShell::GetFirst();

        pBasicMgr = ( pDocShell ? pDocShell->GetBasicManager() : 0 );
    }

    if ( bChangeCurWindow )
        SetCurWindow( FindWindow( pNextActiveModule ), TRUE );
}

void BasicIDEShell::RemoveWindow( IDEBaseWindow* pWindow, BOOL bDestroy, BOOL bAllowChangeCurWindow )
{
    DBG_ASSERT( pWindow, "Kann keinen NULL-Pointer loeschen!" );
    ULONG nKey = aIDEWindowTable.GetKey( pWindow );
    pTabBar->RemovePage( (USHORT)nKey );
    aIDEWindowTable.Remove( nKey );
    if ( pWindow == pCurWin )
    {
        if ( bAllowChangeCurWindow )
            SetCurWindow( FindWindow( 0 ), TRUE );
        else
            SetCurWindow( NULL, FALSE );
    }
    if ( bDestroy )
    {
        if ( !( pWindow->GetStatus() & BASWIN_INRESCHEDULE ) )
        {
            delete pWindow;
        }
        else
        {
            pWindow->AddStatus( BASWIN_TOBEKILLED );
            pWindow->Hide();
            StarBASIC::Stop();
            // Es kommt kein Notify...
            pWindow->BasicStopped();
            aIDEWindowTable.Insert( nKey, pWindow );    // wieder einhaegen
        }
    }
    else
    {
        pWindow->Hide();
        pWindow->AddStatus( BASWIN_SUSPENDED );
        pWindow->Deactivating();
        aIDEWindowTable.Insert( nKey, pWindow );    // wieder einhaegen
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
        SfxBindings& rBindings = BasicIDE::GetBindings();
        rBindings.Invalidate( SID_UNDO );
        rBindings.Invalidate( SID_REDO );
        rBindings.Invalidate( SID_SAVEDOC );
        rBindings.Invalidate( SID_BASICIDE_CHOOSEMACRO );
        rBindings.Invalidate( SID_BASICIDE_MODULEDLG );
        rBindings.Invalidate( SID_BASICIDE_OBJCAT );
        rBindings.Invalidate( SID_BASICSTOP );
        rBindings.Invalidate( SID_BASICRUN );
        rBindings.Invalidate( SID_BASICCOMPILE );
        rBindings.Invalidate( SID_BASICLOAD );
        rBindings.Invalidate( SID_BASICSAVEAS );
        rBindings.Invalidate( SID_BASICIDE_MATCHGROUP );
        rBindings.Invalidate( SID_BASICSTEPINTO );
        rBindings.Invalidate( SID_BASICSTEPOVER );
        rBindings.Invalidate( SID_BASICSTEPOUT );
        rBindings.Invalidate( SID_BASICIDE_TOGGLEBRKPNT );
        rBindings.Invalidate( SID_BASICIDE_ADDWATCH );
        rBindings.Invalidate( SID_BASICIDE_REMOVEWATCH );
        rBindings.Invalidate( SID_CHOOSE_CONTROLS );
        rBindings.Invalidate( SID_PRINTDOC );
        rBindings.Invalidate( SID_PRINTDOCDIRECT );
        rBindings.Invalidate( SID_SETUPPRINTER );
        rBindings.Invalidate( SID_DIALOG_TESTMODE );

        rBindings.Invalidate( SID_DOC_MODIFIED );
        rBindings.Invalidate( SID_BASICIDE_STAT_TITLE );
        rBindings.Invalidate( SID_BASICIDE_STAT_POS );
        rBindings.Invalidate( SID_ATTR_INSERT );
        rBindings.Invalidate( SID_ATTR_SIZE );
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

void BasicIDEShell::SetCurBasic( StarBASIC* pBasic, BOOL bUpdateWindows )
{
    pCurBasic = pBasic;
    if ( bUpdateWindows )
        UpdateWindows();

    pTabBar->SetCurrentLib( pBasic );

    BasicIDE::GetBindings().Invalidate( SID_BASICIDE_LIBSELECTOR );
}

void BasicIDEShell::ImplStartListening( StarBASIC* pBasic )
{
    StartListening( pBasic->GetBroadcaster(), TRUE /* Nur einmal anmelden */ );
}


