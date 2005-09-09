/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dialog.cxx,v $
 *
 *  $Revision: 1.33 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 12:23:07 $
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

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _SV_RC_H
#include <tools/rc.h>
#endif
#ifndef _SV_SVDATA_HXX
#include <svdata.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <svapp.hxx>
#endif
#ifndef _SV_WINDOW_H
#include <window.h>
#endif
#ifndef _SV_EVENT_HXX
#include <event.hxx>
#endif
#ifndef _SV_BRDWIN_HXX
#include <brdwin.hxx>
#endif
#ifndef _SV_WRKWIN_HXX
#include <wrkwin.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <button.hxx>
#endif
#ifndef _SV_MNEMONIC_HXX
#include <mnemonic.hxx>
#endif
#ifndef _SV_DIALOG_HXX
#include <dialog.hxx>
#endif

#ifndef _SV_DECOVIEW_HXX
#include <decoview.hxx>
#endif

#ifdef DBG_UTIL
#ifndef _SV_MSGBOX_HXX
#include <msgbox.hxx>
#endif
#endif

#include <unowrap.hxx>



// =======================================================================

#ifdef DBG_UTIL

static ByteString ImplGetDialogText( Dialog* pDialog )
{
    ByteString aErrorStr( pDialog->GetText(), RTL_TEXTENCODING_UTF8 );
    if ( (pDialog->GetType() == WINDOW_MESSBOX) ||
         (pDialog->GetType() == WINDOW_INFOBOX) ||
         (pDialog->GetType() == WINDOW_WARNINGBOX) ||
         (pDialog->GetType() == WINDOW_ERRORBOX) ||
         (pDialog->GetType() == WINDOW_QUERYBOX) )
    {
        aErrorStr += ", ";
        aErrorStr += ByteString( ((MessBox*)pDialog)->GetMessText(), RTL_TEXTENCODING_UTF8 );
    }
    return aErrorStr;
}

#endif

// =======================================================================

static BOOL ImplIsMnemonicCtrl( Window* pWindow )
{
    if( ! pWindow->GetSettings().GetStyleSettings().GetAutoMnemonic() )
        return FALSE;

    if ( (pWindow->GetType() == WINDOW_RADIOBUTTON) ||
         (pWindow->GetType() == WINDOW_CHECKBOX) ||
         (pWindow->GetType() == WINDOW_TRISTATEBOX) ||
         (pWindow->GetType() == WINDOW_PUSHBUTTON) )
        return TRUE;

    if ( pWindow->GetType() == WINDOW_FIXEDTEXT )
    {
        if ( pWindow->GetStyle() & (WB_INFO | WB_NOLABEL) )
            return FALSE;
        Window* pNextWindow = pWindow->GetWindow( WINDOW_NEXT );
        if ( !pNextWindow )
            return FALSE;
        pNextWindow = pNextWindow->GetWindow( WINDOW_CLIENT );
        if ( !(pNextWindow->GetStyle() & WB_TABSTOP) ||
             (pNextWindow->GetType() == WINDOW_FIXEDTEXT) ||
             (pNextWindow->GetType() == WINDOW_GROUPBOX) ||
             (pNextWindow->GetType() == WINDOW_RADIOBUTTON) ||
             (pNextWindow->GetType() == WINDOW_CHECKBOX) ||
             (pNextWindow->GetType() == WINDOW_TRISTATEBOX) ||
             (pNextWindow->GetType() == WINDOW_PUSHBUTTON) )
            return FALSE;

        return TRUE;
    }

    return FALSE;
}

// -----------------------------------------------------------------------

void ImplWindowAutoMnemonic( Window* pWindow )
{
    MnemonicGenerator   aMnemonicGenerator;
    Window*                 pGetChild;
    Window*                 pChild;

    // Die schon vergebenen Mnemonics registieren
    pGetChild = pWindow->GetWindow( WINDOW_FIRSTCHILD );
    while ( pGetChild )
    {
        pChild = pGetChild->ImplGetWindow();
        aMnemonicGenerator.RegisterMnemonic( pChild->GetText() );
        pGetChild = pGetChild->GetWindow( WINDOW_NEXT );
    }

    // Bei TabPages auch noch die Controls vom Dialog beruecksichtigen
    if ( pWindow->GetType() == WINDOW_TABPAGE )
    {
        Window* pParent = pWindow->GetParent();
        if ( pParent->GetType() == WINDOW_TABCONTROL )
            pParent = pParent->GetParent();

        if ( (pParent->GetStyle() & (WB_DIALOGCONTROL | WB_NODIALOGCONTROL)) == WB_DIALOGCONTROL )
        {
            pGetChild = pParent->GetWindow( WINDOW_FIRSTCHILD );
            while ( pGetChild )
            {
                pChild = pGetChild->ImplGetWindow();
                aMnemonicGenerator.RegisterMnemonic( pChild->GetText() );
                pGetChild = pGetChild->GetWindow( WINDOW_NEXT );
            }
        }
    }

    // Die Mnemonics an die Controls vergeben, die noch keinen haben
    pGetChild = pWindow->GetWindow( WINDOW_FIRSTCHILD );
    while ( pGetChild )
    {
        pChild = pGetChild->ImplGetWindow();
        if ( ImplIsMnemonicCtrl( pChild ) )
        {
            XubString aText = pChild->GetText();
            if ( aMnemonicGenerator.CreateMnemonic( aText ) )
                pChild->SetText( aText );
        }

        pGetChild = pGetChild->GetWindow( WINDOW_NEXT );
    }
}

// =======================================================================

static PushButton* ImplGetDefaultButton( Dialog* pDialog )
{
    Window* pChild = pDialog->GetWindow( WINDOW_FIRSTCHILD );
    while ( pChild )
    {
        if ( pChild->ImplIsPushButton() )
        {
            PushButton* pPushButton = (PushButton*)pChild;
            if ( pPushButton->ImplIsDefButton() )
                return pPushButton;
        }

        pChild = pChild->GetWindow( WINDOW_NEXT );
    }

    return NULL;
}

// -----------------------------------------------------------------------

static PushButton* ImplGetOKButton( Dialog* pDialog )
{
    Window* pChild = pDialog->GetWindow( WINDOW_FIRSTCHILD );
    while ( pChild )
    {
        if ( pChild->GetType() == WINDOW_OKBUTTON )
            return (PushButton*)pChild;

        pChild = pChild->GetWindow( WINDOW_NEXT );
    }

    return NULL;
}

// -----------------------------------------------------------------------

static PushButton* ImplGetCancelButton( Dialog* pDialog )
{
    Window* pChild = pDialog->GetWindow( WINDOW_FIRSTCHILD );
    while ( pChild )
    {
        if ( pChild->GetType() == WINDOW_CANCELBUTTON )
            return (PushButton*)pChild;

        pChild = pChild->GetWindow( WINDOW_NEXT );
    }

    return NULL;
}

// -----------------------------------------------------------------------

static void ImplMouseAutoPos( Dialog* pDialog )
{
    ULONG nMouseOptions = pDialog->GetSettings().GetMouseSettings().GetOptions();
    if ( nMouseOptions & MOUSE_OPTION_AUTOCENTERPOS )
    {
        Size aSize = pDialog->GetOutputSizePixel();
        pDialog->SetPointerPosPixel( Point( aSize.Width()/2, aSize.Height()/2 ) );
    }
    else if ( nMouseOptions & MOUSE_OPTION_AUTODEFBTNPOS )
    {
        Window* pWindow = ImplGetDefaultButton( pDialog );
        if ( !pWindow )
            pWindow = ImplGetOKButton( pDialog );
        if ( !pWindow )
            pWindow = ImplGetCancelButton( pDialog );
        if ( !pWindow )
            pWindow = pDialog;
        Size aSize = pWindow->GetOutputSizePixel();
        pWindow->SetPointerPosPixel( Point( aSize.Width()/2, aSize.Height()/2 ) );
    }
}

// =======================================================================

void Dialog::ImplInitData()
{
    mpWindowImpl->mbDialog            = TRUE;

    mpDialogParent      = NULL;
    mpResult            = NULL;
    mpPrevExecuteDlg    = NULL;
    mbInExecute         = FALSE;
    mbOldSaveBack       = FALSE;
    mbInClose           = FALSE;
    mbModalMode         = FALSE;
    mnMousePositioned   = 0;
}

// -----------------------------------------------------------------------

void Dialog::ImplInit( Window* pParent, WinBits nStyle )
{
    USHORT nSysWinMode = Application::GetSystemWindowMode();

    if ( !(nStyle & WB_NODIALOGCONTROL) )
        nStyle |= WB_DIALOGCONTROL;
    nStyle |= WB_ROLLABLE;

    // Now, all Dialogs are per default system windows !!!
    nStyle |= WB_SYSTEMWINDOW;


    // parent is NULL: get the default Dialog parent
    if ( !pParent )
    {
        pParent = Application::GetDefDialogParent();
        if ( !pParent && !(nStyle & WB_SYSTEMWINDOW) )
            pParent = ImplGetSVData()->maWinData.mpAppWin;

        // If Parent is disabled, then we search for a modal dialog
        // in this frame
        if ( pParent && !pParent->IsInputEnabled() )
        {
            ImplSVData* pSVData = ImplGetSVData();
            Dialog*     pExeDlg = pSVData->maWinData.mpLastExecuteDlg;
            while ( pExeDlg )
            {
                // Nur wenn er sichtbar und enabled ist
                if ( pParent->ImplGetFirstOverlapWindow()->IsWindowOrChild( pExeDlg, TRUE ) &&
                     pExeDlg->IsReallyVisible() &&
                     pExeDlg->IsEnabled() && pExeDlg->IsInputEnabled() )
                {
                    pParent = pExeDlg;
                    break;
                }

                pExeDlg = pExeDlg->mpPrevExecuteDlg;
            }
        }
    }
    // DIALOG_NO_PARENT: explicitly don't have a parent for this Dialog
    else if( pParent == DIALOG_NO_PARENT )
        pParent = NULL;

/*
    // Now, all Dialogs are per default system windows !!!
    if ( pParent && !(nSysWinMode & SYSTEMWINDOW_MODE_NOAUTOMODE) )
    {
        if ( !pParent->mpWindowImpl->mpFrameWindow->IsVisible() )
            pParent = NULL;
        else
        {
            if ( pParent->mpWindowImpl->mpFrameWindow->IsDialog() )
            {
                Size aOutSize = pParent->mpWindowImpl->mpFrameWindow->GetOutputSizePixel();
                if ( (aOutSize.Width() < 210) ||(aOutSize.Height() < 160) )
                    nStyle |= WB_SYSTEMWINDOW;
            }
        }
    }
*/

    if ( !pParent || (nStyle & WB_SYSTEMWINDOW) ||
         (pParent->mpWindowImpl->mpFrameData->mbNeedSysWindow && !(nSysWinMode & SYSTEMWINDOW_MODE_NOAUTOMODE)) ||
         (nSysWinMode & SYSTEMWINDOW_MODE_DIALOG) )
    {
        // create window with a small border ?
        if ( (nStyle & (WB_BORDER | WB_NOBORDER | WB_MOVEABLE | WB_SIZEABLE | WB_CLOSEABLE)) == WB_BORDER )
        {
            ImplBorderWindow* pBorderWin  = new ImplBorderWindow( pParent, nStyle, BORDERWINDOW_STYLE_FRAME );
            SystemWindow::ImplInit( pBorderWin, nStyle & ~WB_BORDER, NULL );
            pBorderWin->mpWindowImpl->mpClientWindow = this;
            pBorderWin->GetBorder( mpWindowImpl->mnLeftBorder, mpWindowImpl->mnTopBorder, mpWindowImpl->mnRightBorder, mpWindowImpl->mnBottomBorder );
            mpWindowImpl->mpBorderWindow  = pBorderWin;
            mpWindowImpl->mpRealParent    = pParent;
        }
        else
        {
            mpWindowImpl->mbFrame         = TRUE;
            mpWindowImpl->mbOverlapWin    = TRUE;
            SystemWindow::ImplInit( pParent, nStyle & (WB_MOVEABLE | WB_SIZEABLE | WB_ROLLABLE | WB_CLOSEABLE | WB_STANDALONE) | WB_CLOSEABLE, NULL );
            // Now set all style bits
            mpWindowImpl->mnStyle = nStyle;
        }
    }
    else
    {
        ImplBorderWindow* pBorderWin  = new ImplBorderWindow( pParent, nStyle, BORDERWINDOW_STYLE_OVERLAP | BORDERWINDOW_STYLE_BORDER );
        SystemWindow::ImplInit( pBorderWin, nStyle & ~WB_BORDER, NULL );
        pBorderWin->mpWindowImpl->mpClientWindow = this;
        pBorderWin->GetBorder( mpWindowImpl->mnLeftBorder, mpWindowImpl->mnTopBorder, mpWindowImpl->mnRightBorder, mpWindowImpl->mnBottomBorder );
        mpWindowImpl->mpBorderWindow  = pBorderWin;
        mpWindowImpl->mpRealParent    = pParent;
    }

    SetActivateMode( ACTIVATE_MODE_GRABFOCUS );

    ImplInitSettings();
}

// -----------------------------------------------------------------------

void Dialog::ImplInitSettings()
{
    // Wir haben graue Dialoge
    if ( IsControlBackground() )
        SetBackground( GetControlBackground() );
    else
        SetBackground( GetSettings().GetStyleSettings().GetDialogColor() );
}

// -----------------------------------------------------------------------

void Dialog::ImplCenterDialog()
{
    Rectangle   aDeskRect = ImplGetFrameWindow()->GetDesktopRectPixel();
    Point       aDeskPos = aDeskRect.TopLeft();
    Size        aDeskSize = aDeskRect.GetSize();
    Size        aWinSize = GetSizePixel();
    Window *pWindow = this;
    while ( pWindow->mpWindowImpl->mpBorderWindow )
        pWindow = pWindow->mpWindowImpl->mpBorderWindow;
    Point       aWinPos( ((aDeskSize.Width() - aWinSize.Width()) / 2) + aDeskPos.X(),
                         ((aDeskSize.Height() - aWinSize.Height()) / 2) + aDeskPos.Y() );

    // Pruefen, ob Dialogbox ausserhalb des Desks liegt
    if ( (aWinPos.X() + aWinSize.Width()) > (aDeskPos.X()+aDeskSize.Width()) )
        aWinPos.X() = aDeskPos.X()+aDeskSize.Width() - aWinSize.Width();
    if ( (aWinPos.Y()+aWinSize.Height()) > (aDeskPos.Y()+aDeskSize.Height()) )
        aWinPos.Y() = aDeskPos.Y()+aDeskSize.Height() - aWinSize.Height();
    // Linke Ecke bevorzugen, da Titelbar oben ist
    if ( aWinPos.X() < aDeskPos.X() )
        aWinPos.X() = aDeskPos.X();
    if ( aWinPos.Y() < aDeskPos.Y() )
        aWinPos.Y() = aDeskPos.Y();

    //SetPosPixel( aWinPos );
    SetPosPixel( pWindow->ScreenToOutputPixel( aWinPos ) );
}

// -----------------------------------------------------------------------

Dialog::Dialog( WindowType nType ) :
    SystemWindow( nType )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

Dialog::Dialog( Window* pParent, WinBits nStyle ) :
    SystemWindow( WINDOW_DIALOG )
{
    ImplInitData();
    ImplInit( pParent, nStyle );
}

// -----------------------------------------------------------------------

Dialog::Dialog( Window* pParent, const ResId& rResId ) :
    SystemWindow( WINDOW_DIALOG )
{
    ImplInitData();
    rResId.SetRT( RSC_DIALOG );
    ImplInit( pParent, ImplInitRes( rResId ) );
    ImplLoadRes( rResId );
}

// -----------------------------------------------------------------------

long Dialog::Notify( NotifyEvent& rNEvt )
{
    // Zuerst Basisklasse rufen wegen TabSteuerung
    long nRet = SystemWindow::Notify( rNEvt );
    if ( !nRet )
    {
        if ( rNEvt.GetType() == EVENT_KEYINPUT )
        {
            const KeyEvent* pKEvt = rNEvt.GetKeyEvent();
            KeyCode         aKeyCode = pKEvt->GetKeyCode();
            USHORT          nKeyCode = aKeyCode.GetCode();

            if ( (nKeyCode == KEY_ESCAPE) &&
                 ((GetStyle() & WB_CLOSEABLE) || ImplGetCancelButton( this ) || ImplGetOKButton( this )) )
            {
                Close();
                return TRUE;
            }
        }
        else if ( rNEvt.GetType() == EVENT_GETFOCUS )
        {
            // make sure the dialog is still modal
            // changing focus between application frames may
            // have re-enabled input for our parent
            if( mbInExecute )
            {
                // do not change modal counter (pSVData->maAppData.mnModalDialog)
                SetModalInputMode( FALSE );
                SetModalInputMode( TRUE );

                // #93022# def-button might have changed after show
                if( !mnMousePositioned )
                {
                    mnMousePositioned = 1;
                    ImplMouseAutoPos( this );
                }

            }
        }
    }

    return nRet;
}

// -----------------------------------------------------------------------

void Dialog::StateChanged( StateChangedType nType )
{
    SystemWindow::StateChanged( nType );

    if ( nType == STATE_CHANGE_INITSHOW )
    {
        if ( GetSettings().GetStyleSettings().GetAutoMnemonic() )
            ImplWindowAutoMnemonic( this );

        //if ( IsDefaultPos() && !mpWindowImpl->mbFrame )
        //    ImplCenterDialog();
        if ( !HasChildPathFocus() || HasFocus() )
            GrabFocusToFirstControl();
        if ( !(GetStyle() & WB_CLOSEABLE) )
        {
            if ( ImplGetCancelButton( this ) || ImplGetOKButton( this ) )
            {
                if ( ImplGetBorderWindow() )
                    ((ImplBorderWindow*)ImplGetBorderWindow())->SetCloser();
            }
        }

        ImplMouseAutoPos( this );
    }
    else if ( nType == STATE_CHANGE_CONTROLBACKGROUND )
    {
        ImplInitSettings();
        Invalidate();
    }
}

// -----------------------------------------------------------------------

void Dialog::DataChanged( const DataChangedEvent& rDCEvt )
{
    SystemWindow::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        ImplInitSettings();
        Invalidate();
    }
}

// -----------------------------------------------------------------------

BOOL Dialog::Close()
{
    ImplDelData aDelData;
    ImplAddDel( &aDelData );
    ImplCallEventListeners( VCLEVENT_WINDOW_CLOSE );
    if ( aDelData.IsDelete() )
        return FALSE;
    ImplRemoveDel( &aDelData );

    if ( mpWindowImpl->mxWindowPeer.is() && IsCreatedWithToolkit() && !IsInExecute() )
        return FALSE;

    mbInClose = TRUE;

    if ( !(GetStyle() & WB_CLOSEABLE) )
    {
        BOOL bRet = TRUE;
        ImplDelData aDelData;
        ImplAddDel( &aDelData );
        PushButton* pButton = ImplGetCancelButton( this );
        if ( pButton )
            pButton->Click();
        else
        {
            pButton = ImplGetOKButton( this );
            if ( pButton )
                pButton->Click();
            else
                bRet = FALSE;
        }
        if ( aDelData.IsDelete() )
            return TRUE;
        ImplRemoveDel( &aDelData );
        return bRet;
    }

    if ( IsInExecute() )
    {
        EndDialog( FALSE );
        mbInClose = FALSE;
        return TRUE;
    }
    else
    {
        mbInClose = FALSE;
        return SystemWindow::Close();
    }
}

// -----------------------------------------------------------------------

short Dialog::Execute()
{
    if ( mbInExecute )
    {
#ifdef DBG_UTIL
        ByteString aErrorStr( "Dialog::Execute() is called in Dialog::Execute(): " );
        aErrorStr += ImplGetDialogText( this );
        DBG_ERROR( aErrorStr.GetBuffer() );
#endif
        return 0;
    }

    if ( Application::IsDialogCancelEnabled() )
    {
#ifdef DBG_UTIL
        ByteString aErrorStr( "Dialog::Execute() is called in a none UI application: " );
        aErrorStr += ImplGetDialogText( this );
        DBG_ERROR( aErrorStr.GetBuffer() );
#endif
        return 0;
    }

#ifdef DBG_UTIL
    Window* pParent = GetParent();
    if ( pParent )
    {
        pParent = pParent->ImplGetFirstOverlapWindow();
        DBG_ASSERT( pParent->IsReallyVisible(),
                    "Dialog::Execute() - Parent not visible" );
        DBG_ASSERT( pParent->IsInputEnabled(),
                    "Dialog::Execute() - Parent already disabled, use another parent to ensure modality!" );
    }
#endif

    ImplSVData* pSVData = ImplGetSVData();

    // Dialoge, die sich in Execute befinden, miteinander verketten
    mpPrevExecuteDlg = pSVData->maWinData.mpLastExecuteDlg;
    pSVData->maWinData.mpLastExecuteDlg = this;

    // Capture beenden, damit der Dialog bedient werden kann
    if ( pSVData->maWinData.mpTrackWin )
        pSVData->maWinData.mpTrackWin->EndTracking( ENDTRACK_CANCEL );
    if ( pSVData->maWinData.mpCaptureWin )
        pSVData->maWinData.mpCaptureWin->ReleaseMouse();
    EnableInput( TRUE, TRUE );

    if ( GetParent() )
    {
        NotifyEvent aNEvt( EVENT_EXECUTEDIALOG, this );
        GetParent()->Notify( aNEvt );
    }
    long nRet;
    mpResult = &nRet;
    mbInExecute = TRUE;
    SetModalInputMode( TRUE );
    mbOldSaveBack = IsSaveBackgroundEnabled();
    EnableSaveBackground();
    Show();


    // Solange Yielden, bis EndDialog aufgerufen wird, oder der Dialog
    // zerstoert wird (sollte nicht sein und ist nur vorsichtsmassnahme)
    ImplDelData aDelData;
    ImplAddDel( &aDelData );
    ImplDelData aParentDelData;

    pSVData->maAppData.mnModalMode++;
    //DBG_ASSERT( mpDialogParent, "Dialog::Execute() - no Parent: cannot set modal count!" );
    Window* pDialogParent = mpDialogParent;
    if( pDialogParent )
    {
        pDialogParent->ImplIncModalCount();        // #106303# support frame based modal count
        pDialogParent->ImplAddDel( &aParentDelData );
    }
    while ( !aDelData.IsDelete() && mbInExecute )
        Application::Yield();
    pSVData->maAppData.mnModalMode--;
    if( pDialogParent  )
    {
        if( ! aParentDelData.IsDelete() )
        {
            pDialogParent->ImplDecModalCount();        // #106303# support frame based modal count
            pDialogParent->ImplRemoveDel( &aParentDelData );
        }
#ifdef DBG_UTIL
        else
        {
            DBG_ERROR( "Dialog::Execute() - Parent of dialog destroyed in Execute()" );
        }
#endif
    }
    if ( !aDelData.IsDelete() )
        ImplRemoveDel( &aDelData );
#ifdef DBG_UTIL
    else
    {
        DBG_ERROR( "Dialog::Execute() - Dialog destroyed in Execute()" );
    }
#endif

    return (short)nRet;
}

// -----------------------------------------------------------------------

void Dialog::EndDialog( long nResult )
{
    if ( mbInExecute )
    {
        SetModalInputMode( FALSE );

        // Dialog aus der Kette der Dialoge die in Execute stehen entfernen
        ImplSVData* pSVData = ImplGetSVData();
        Dialog* pExeDlg = pSVData->maWinData.mpLastExecuteDlg;
        while ( pExeDlg )
        {
            if ( pExeDlg == this )
            {
                pSVData->maWinData.mpLastExecuteDlg = mpPrevExecuteDlg;
                break;
            }
            pExeDlg = pExeDlg->mpPrevExecuteDlg;
        }
        // set focus to previous modal dialogue if it is modal for
        // the same frame parent (or NULL)
        if( mpPrevExecuteDlg )
        {
            Window* pFrameParent = ImplGetFrameWindow()->ImplGetParent();
            Window* pPrevFrameParent = mpPrevExecuteDlg->ImplGetFrameWindow()->ImplGetParent();
            if( ( !pFrameParent && !pPrevFrameParent ) ||
                ( pFrameParent && pPrevFrameParent && pFrameParent->ImplGetFrame() == pPrevFrameParent->ImplGetFrame() )
                )
            {
                mpPrevExecuteDlg->GrabFocus();
            }
        }
        mpPrevExecuteDlg = NULL;

        Hide();
        EnableSaveBackground( mbOldSaveBack );
        if ( GetParent() )
        {
            NotifyEvent aNEvt( EVENT_ENDEXECUTEDIALOG, this );
            GetParent()->Notify( aNEvt );
        }
        if ( mpResult )
            *mpResult = nResult;
        mpResult    = NULL;
        mbInExecute = FALSE;
    }
}

// -----------------------------------------------------------------------

void Dialog::EndAllDialogs( Window* pParent )
{
   ImplSVData*  pSVData = ImplGetSVData();
   Dialog*      pTempModDialog;
   Dialog*      pModDialog = pSVData->maWinData.mpLastExecuteDlg;
   while ( pModDialog )
   {
     pTempModDialog = pModDialog->mpPrevExecuteDlg;
     if( !pParent || ( pParent && pParent->IsWindowOrChild( pModDialog, TRUE ) ) )
     {
        pModDialog->EndDialog( FALSE );
        pModDialog->PostUserEvent( Link() );
     }
     pModDialog = pTempModDialog;
   }
}

// -----------------------------------------------------------------------

void Dialog::SetModalInputMode( BOOL bModal )
{
    if ( bModal == mbModalMode )
        return;

    ImplSVData* pSVData = ImplGetSVData();
    mbModalMode = bModal;
    if ( bModal )
    {
        pSVData->maAppData.mnModalDialog++;

        // Diable the prev Modal Dialog, because our dialog must close at first,
        // before the other dialog can be closed (because the other dialog
        // is on stack since our dialog returns)
        if ( mpPrevExecuteDlg && !mpPrevExecuteDlg->IsWindowOrChild( this, TRUE ) )
            mpPrevExecuteDlg->EnableInput( FALSE, TRUE, TRUE, this );

        // determine next overlap dialog parent
        Window* pParent = GetParent();
        if ( pParent )
        {
            // #103716# dialogs should always be modal to the whole frame window
            mpDialogParent = pParent->mpWindowImpl->mpFrameWindow;

            // #115933# disable the whole frame hierarchie, useful if our parent
            // is a modeless dialog
            Window *pFrame = mpDialogParent;
            while( pFrame )
            {
                pFrame->EnableInput( FALSE, TRUE, TRUE, this );
                if( pFrame->GetParent() )
                    pFrame = pFrame->GetParent()->mpWindowImpl->mpFrameWindow;
                else
                    pFrame = NULL;
            }
        }

    }
    else
    {
        pSVData->maAppData.mnModalDialog--;

        if ( mpDialogParent )
        {
            // #115933# re-enable the whole frame hierarchie again (see above)
            // note that code in getfocus assures that we do not accidentally enable
            // windows that were disabled before
            Window *pFrame = mpDialogParent;
            while( pFrame )
            {
                pFrame->EnableInput( TRUE, TRUE, TRUE, this );
                if( pFrame->GetParent() )
                    pFrame = pFrame->GetParent()->mpWindowImpl->mpFrameWindow;
                else
                    pFrame = NULL;
            }

        }

        // Enable the prev Modal Dialog
        if ( mpPrevExecuteDlg && !mpPrevExecuteDlg->IsWindowOrChild( this, TRUE ) )
        {
            mpPrevExecuteDlg->EnableInput( TRUE, TRUE, TRUE, this );
            // ensure continued modality of prev dialog
            // do not change modality counter
            mpPrevExecuteDlg->SetModalInputMode( FALSE );
            mpPrevExecuteDlg->SetModalInputMode( TRUE );
        }
    }
}

// -----------------------------------------------------------------------

void Dialog::SetModalInputMode( BOOL bModal, BOOL bSubModalDialogs )
{
    if ( bSubModalDialogs )
    {
        Window* pOverlap = ImplGetFirstOverlapWindow();
        pOverlap = pOverlap->mpWindowImpl->mpFirstOverlap;
        while ( pOverlap )
        {
            if ( pOverlap->IsDialog() )
                ((Dialog*)pOverlap)->SetModalInputMode( bModal, TRUE );
            pOverlap = pOverlap->mpWindowImpl->mpNext;
        }
    }

    SetModalInputMode( bModal );
}

// -----------------------------------------------------------------------

void Dialog::GrabFocusToFirstControl()
{
    Window* pFocusControl;

    // Wenn Dialog den Focus hat, versuchen wr trotzdem
    // ein Focus-Control zu finden
    if ( HasFocus() )
        pFocusControl = NULL;
    else
    {
        // Wenn schon ein Child-Fenster mal den Focus hatte,
        // dann dieses bevorzugen
        pFocusControl = ImplGetFirstOverlapWindow()->mpWindowImpl->mpLastFocusWindow;
        // Control aus der Dialog-Steuerung suchen
        if ( pFocusControl )
            pFocusControl = ImplFindDlgCtrlWindow( pFocusControl );
    }
    // Kein Control hatte vorher den Focus, oder das Control
    // befindet sich nicht in der Tab-Steuerung, dann das erste
    // Control in der TabSteuerung den Focus geben
    if ( !pFocusControl ||
         !(pFocusControl->GetStyle() & WB_TABSTOP) ||
         !pFocusControl->IsVisible() ||
         !pFocusControl->IsEnabled() || !pFocusControl->IsInputEnabled() )
    {
        USHORT n = 0;
        pFocusControl = ImplGetDlgWindow( n, DLGWINDOW_FIRST );
    }
    if ( pFocusControl )
        pFocusControl->ImplControlFocus( GETFOCUS_INIT );
}

void Dialog::GetDrawWindowBorder( sal_Int32& rLeftBorder, sal_Int32& rTopBorder, sal_Int32& rRightBorder, sal_Int32& rBottomBorder ) const
{
    ImplBorderWindow aImplWin( (Window*)this, WB_BORDER|WB_STDWORK, BORDERWINDOW_STYLE_OVERLAP );
//  aImplWin.SetText( GetText() );
//  aImplWin.SetPosSizePixel( aPos.X(), aPos.Y(), aSize.Width(), aSize.Height() );
//  aImplWin.SetDisplayActive( TRUE );
//  aImplWin.InitView();
    aImplWin.GetBorder( rLeftBorder, rTopBorder, rRightBorder, rBottomBorder );
}


void Dialog::Draw( OutputDevice* pDev, const Point& rPos, const Size& rSize, ULONG nFlags )
{
    Point aPos = pDev->LogicToPixel( rPos );
    Size aSize = pDev->LogicToPixel( rSize );

    ImplInitSettings();

    pDev->Push();
    pDev->SetMapMode();
    pDev->SetLineColor();
    pDev->SetFillColor( GetBackground().GetColor() );

    pDev->DrawRect( Rectangle( aPos, aSize ) );

    ImplBorderWindow aImplWin( this, WB_BORDER|WB_STDWORK, BORDERWINDOW_STYLE_OVERLAP );
    aImplWin.SetText( GetText() );
    aImplWin.SetPosSizePixel( aPos.X(), aPos.Y(), aSize.Width(), aSize.Height() );
    aImplWin.SetDisplayActive( TRUE );
    aImplWin.InitView();

    aImplWin.Draw( Rectangle( aPos, aSize ), pDev, aPos );

    pDev->Pop();
}


// =======================================================================

ModelessDialog::ModelessDialog( Window* pParent, WinBits nStyle ) :
    Dialog( WINDOW_MODELESSDIALOG )
{
    ImplInit( pParent, nStyle );
}

// -----------------------------------------------------------------------

ModelessDialog::ModelessDialog( Window* pParent, const ResId& rResId ) :
    Dialog( WINDOW_MODELESSDIALOG )
{
    rResId.SetRT( RSC_MODELESSDIALOG );
    WinBits nStyle = ImplInitRes( rResId );
    ImplInit( pParent, nStyle );
    ImplLoadRes( rResId );

    if ( !(nStyle & WB_HIDE) )
        Show();
}

// =======================================================================

ModalDialog::ModalDialog( Window* pParent, WinBits nStyle ) :
    Dialog( WINDOW_MODALDIALOG )
{
    ImplInit( pParent, nStyle );
}

// -----------------------------------------------------------------------

ModalDialog::ModalDialog( Window* pParent, const ResId& rResId ) :
    Dialog( WINDOW_MODALDIALOG )
{
    rResId.SetRT( RSC_MODALDIALOG );
    ImplInit( pParent, ImplInitRes( rResId ) );
    ImplLoadRes( rResId );
}
