/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: wrkwin.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 19:43:32 $
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

#ifndef _SV_SVSYS_HXX
#include <svsys.h>
#endif
#ifndef _SV_SALFRAME_HXX
#include <salframe.hxx>
#endif

#ifndef _DEBUG_HXX
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
#ifndef _SV_BRDWIN_HXX
#include <brdwin.hxx>
#endif
#ifndef _SV_WINDOW_H
#include <window.h>
#endif
#ifndef _SV_WRKWIN_HXX
#include <wrkwin.hxx>
#endif
#ifndef _SV_OPENGL_HXX
#include <opengl.hxx>
#endif
#ifndef _SV_SYSDATA_HXX
#include <sysdata.hxx>
#endif

// =======================================================================

#define WORKWIN_WINDOWSTATE_FULLSCREEN          ((ULONG)0x00010000)
#define WORKWIN_WINDOWSTATE_ALL                 ((ULONG)0x00FF0000)

// =======================================================================

void WorkWindow::ImplInitWorkWindowData()
{
    mnIcon                  = 0; // Should be removed in the next top level update - now in SystemWindow

    mnPresentationFlags     = 0;
    mbPresentationMode      = FALSE;
    mbPresentationVisible   = FALSE;
    mbPresentationFull      = FALSE;
    mbFullScreenMode        = FALSE;
}

// -----------------------------------------------------------------------

void WorkWindow::ImplInit( Window* pParent, WinBits nStyle, SystemParentData* pSystemParentData )
{
#if defined WNT
    /*
     * #98153# since SystemParentData typically contains a HWND from
     * another process and the OpenGL implementation does not like
     * our child window of another processes frame we disable it here.
     */
    if( pSystemParentData )
        OpenGL::Invalidate();
#endif
    USHORT nFrameStyle = BORDERWINDOW_STYLE_FRAME;
    if ( nStyle & WB_APP )
        nFrameStyle |= BORDERWINDOW_STYLE_APP;

    ImplBorderWindow* pBorderWin = new ImplBorderWindow( pParent, pSystemParentData, nStyle, nFrameStyle );
    Window::ImplInit( pBorderWin, nStyle & (WB_3DLOOK | WB_CLIPCHILDREN | WB_DIALOGCONTROL | WB_SYSTEMFLOATWIN), NULL );
    pBorderWin->mpWindowImpl->mpClientWindow = this;
    pBorderWin->GetBorder( mpWindowImpl->mnLeftBorder, mpWindowImpl->mnTopBorder, mpWindowImpl->mnRightBorder, mpWindowImpl->mnBottomBorder );
    mpWindowImpl->mpBorderWindow  = pBorderWin;
//        mpWindowImpl->mpRealParent    = pParent; // !!! Muesste eigentlich gesetzt werden, aber wegen Fehlern mit dem MenuBar erstmal nicht gesetzt !!!

    if ( nStyle & WB_APP )
    {
        ImplSVData* pSVData = ImplGetSVData();
        DBG_ASSERT( !pSVData->maWinData.mpAppWin, "WorkWindow::WorkWindow(): More than one window with style WB_APP" );
        pSVData->maWinData.mpAppWin = this;
    }

    SetActivateMode( ACTIVATE_MODE_GRABFOCUS );
}

// -----------------------------------------------------------------------

void WorkWindow::ImplInit( Window* pParent, WinBits nStyle, const ::com::sun::star::uno::Any& aSystemWorkWindowToken )
{
    if( aSystemWorkWindowToken.hasValue() )
    {
        ::com::sun::star::uno::Sequence< sal_Int8 > aSeq;
        aSystemWorkWindowToken >>= aSeq;
        SystemParentData* pData = (SystemParentData*)aSeq.getArray();
        DBG_ASSERT( aSeq.getLength() == sizeof( SystemParentData ) && pData->nSize == sizeof( SystemParentData ), "WorkWindow::WorkWindow( Window*, const Any&, WinBits ) called with invalid Any" );
        // init with style 0 as does WorkWindow::WorkWindow( SystemParentData* );
        ImplInit( pParent, 0, pData );
    }
    else
        ImplInit( pParent, nStyle, NULL );
}

// -----------------------------------------------------------------------

WorkWindow::WorkWindow( WindowType nType ) :
    SystemWindow( nType )
{
    ImplInitWorkWindowData();
}

// -----------------------------------------------------------------------

WorkWindow::WorkWindow( Window* pParent, WinBits nStyle ) :
    SystemWindow( WINDOW_WORKWINDOW )
{
    ImplInitWorkWindowData();
    ImplInit( pParent, nStyle, NULL );
}

// -----------------------------------------------------------------------

WorkWindow::WorkWindow( Window* pParent, const ResId& rResId ) :
    SystemWindow( WINDOW_WORKWINDOW )
{
    ImplInitWorkWindowData();
    rResId.SetRT( RSC_WORKWIN );
    ImplInit( pParent, ImplInitRes( rResId ) );
    ImplLoadRes( rResId );
}

// -----------------------------------------------------------------------

WorkWindow::WorkWindow( Window* pParent, const ::com::sun::star::uno::Any& aSystemWorkWindowToken, WinBits nStyle ) :
    SystemWindow( WINDOW_WORKWINDOW )
{
    ImplInitWorkWindowData();
    mbSysChild = TRUE;
    ImplInit( pParent, nStyle, aSystemWorkWindowToken );
}

// -----------------------------------------------------------------------

WorkWindow::WorkWindow( SystemParentData* pParent ) :
    SystemWindow( WINDOW_WORKWINDOW )
{
    ImplInitWorkWindowData();
    mbSysChild = TRUE;
    ImplInit( NULL, 0, pParent );
}

// -----------------------------------------------------------------------

void WorkWindow::ImplLoadRes( const ResId& rResId )
{
    SystemWindow::ImplLoadRes( rResId );

    ReadLongRes();
    if ( !(rResId.aWinBits & WB_HIDE) && (RSC_WORKWIN == rResId.GetRT()) )
        Show();
}

// -----------------------------------------------------------------------

WorkWindow::~WorkWindow()
{
    ImplSVData* pSVData = ImplGetSVData();
    if ( pSVData->maWinData.mpAppWin == this )
    {
        pSVData->maWinData.mpAppWin = NULL;
        Application::Quit();
    }
}

// -----------------------------------------------------------------------

void WorkWindow::ShowFullScreenMode( BOOL bFullScreenMode )
{
    if ( !mbFullScreenMode == !bFullScreenMode )
        return;

    mbFullScreenMode = bFullScreenMode != 0;
    if ( !mbSysChild )
    {
        mpWindowImpl->mpFrameWindow->mpWindowImpl->mbWaitSystemResize = TRUE;
        ImplGetFrame()->ShowFullScreen( bFullScreenMode );
    }
}

// -----------------------------------------------------------------------

void WorkWindow::StartPresentationMode( BOOL bPresentation, USHORT nFlags )
{
    if ( !bPresentation == !mbPresentationMode )
        return;

    if ( bPresentation )
    {
        mbPresentationMode      = TRUE;
        mbPresentationVisible   = IsVisible();
        mbPresentationFull      = mbFullScreenMode;
        mnPresentationFlags     = nFlags;

        if ( !(mnPresentationFlags & PRESENTATION_NOFULLSCREEN) )
            ShowFullScreenMode( TRUE );
        if ( !mbSysChild )
        {
            if ( mnPresentationFlags & PRESENTATION_HIDEALLAPPS )
                mpWindowImpl->mpFrame->SetAlwaysOnTop( TRUE );
            if ( !(mnPresentationFlags & PRESENTATION_NOAUTOSHOW) )
                ToTop();
            mpWindowImpl->mpFrame->StartPresentation( TRUE );
        }

        if ( !(mnPresentationFlags & PRESENTATION_NOAUTOSHOW) )
            Show();
    }
    else
    {
        Show( mbPresentationVisible );
        if ( !mbSysChild )
        {
            mpWindowImpl->mpFrame->StartPresentation( FALSE );
            if ( mnPresentationFlags & PRESENTATION_HIDEALLAPPS )
                mpWindowImpl->mpFrame->SetAlwaysOnTop( FALSE );
        }
        ShowFullScreenMode( mbPresentationFull );

        mbPresentationMode      = FALSE;
        mbPresentationVisible   = FALSE;
        mbPresentationFull      = FALSE;
        mnPresentationFlags     = 0;
    }
}

// -----------------------------------------------------------------------

BOOL WorkWindow::IsMinimized() const
{
    //return mpWindowImpl->mpFrameData->mbMinimized;
    SalFrameState aState;
    mpWindowImpl->mpFrame->GetWindowState(&aState);
    return (( aState.mnState & SAL_FRAMESTATE_MINIMIZED ) != 0);
}

// -----------------------------------------------------------------------

BOOL WorkWindow::SetPluginParent( SystemParentData* pParent )
{
    DBG_ASSERT( ! mbPresentationMode && ! mbFullScreenMode, "SetPluginParent in fullscreen or presentation mode !" );

    bool bWasDnd = Window::ImplStopDnd();

    BOOL bShown = IsVisible();
    Show( FALSE );
    BOOL bRet = mpWindowImpl->mpFrame->SetPluginParent( pParent );
    Show( bShown );

    if( bWasDnd )
        Window::ImplStartDnd();

    return bRet;
}

void WorkWindow::ImplSetFrameState( ULONG aFrameState )
{
    SalFrameState   aState;
    aState.mnMask   = SAL_FRAMESTATE_MASK_STATE;
    aState.mnState  = aFrameState;
    mpWindowImpl->mpFrame->SetWindowState( &aState );
}


void WorkWindow::Minimize()
{
    ImplSetFrameState( SAL_FRAMESTATE_MINIMIZED );
}

void WorkWindow::Restore()
{
    ImplSetFrameState( SAL_FRAMESTATE_NORMAL );
}

BOOL WorkWindow::Close()
{
    BOOL bCanClose = SystemWindow::Close();

    // Ist es das Applikationsfenster, dann beende die Applikation
    if ( bCanClose && ( ImplGetSVData()->maWinData.mpAppWin == this ) )
        GetpApp()->Quit();

    return bCanClose;
}

void WorkWindow::Maximize( BOOL bMaximize )
{
    ImplSetFrameState( bMaximize ? SAL_FRAMESTATE_MAXIMIZED : SAL_FRAMESTATE_NORMAL );
}

BOOL WorkWindow::IsMaximized()
{
    BOOL bRet = FALSE;

    SalFrameState aState;
    if( mpWindowImpl->mpFrame->GetWindowState( &aState ) )
    {
        if( aState.mnState & (SAL_FRAMESTATE_MAXIMIZED          |
                              SAL_FRAMESTATE_MAXIMIZED_HORZ     |
                              SAL_FRAMESTATE_MAXIMIZED_VERT ) )
            bRet = TRUE;
    }
    return bRet;
}
