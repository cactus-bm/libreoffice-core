/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: showwin.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:07:09 $
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

#include "ShowWindow.hxx"

#ifndef INCLUDED_SVTOOLS_SYSLOCALE_HXX
#include <svtools/syslocale.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif

#pragma hdrstop

#include "res_bmp.hrc"

#ifndef _SD_SLIDESHOW_HXX
#include "slideshow.hxx"
#endif

#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_SLIDE_VIEW_SHELL_HXX
#include "SlideViewShell.hxx"
#endif
#ifndef _SD_SLIDESHOW_HXX
#include "slideshow.hxx"
#endif
#include "sdresid.hxx"
#include "helpids.h"
#include "strings.hrc"


namespace sd {

// =============================================================================

ShowWindow::ShowWindow( ::Window* pParent )
: ::sd::Window( pParent ),
    meShowWindowMode(SHOWWINDOWMODE_NORMAL),
    mnRestartPageIndex( PAGE_NO_END ),
    mnPauseTimeout( SLIDE_NO_TIMEOUT ),
    mbShowNavigatorAfterSpecialMode( FALSE ),
    mpSlideshow(0)
{
    // Do never mirror the preview window.  This explicitly includes right
    // to left writing environments.
    EnableRTL (FALSE);

    MapMode aMap(GetMapMode());
    aMap.SetMapUnit(MAP_100TH_MM);
    SetMapMode(aMap);

    // HelpId setzen
    SetHelpId( HID_SD_WIN_PRESENTATION );
    SetUniqueId( HID_SD_WIN_PRESENTATION );

    maPauseTimer.SetTimeoutHdl( LINK( this, ShowWindow, PauseTimeoutHdl ) );
    maPauseTimer.SetTimeout( 1000 );

    maShowBackground = Wallpaper( Color( COL_BLACK ) );
//  SetBackground( Wallpaper( Color( COL_BLACK ) ) );
    SetBackground(); // avoids that VCL paints any background!
    GetParent()->Show();
}

ShowWindow::~ShowWindow(void)
{
}

/*************************************************************************
|*
|* Keyboard event
|*
\************************************************************************/

void ShowWindow::KeyInput(const KeyEvent& rKEvt)
{
    BOOL bReturn = FALSE;

    if( SHOWWINDOWMODE_PREVIEW == meShowWindowMode )
    {
        TerminateShow();
        bReturn = true;
    }
    else if( SHOWWINDOWMODE_END == meShowWindowMode )
    {
        const int nKeyCode = rKEvt.GetKeyCode().GetCode();
        switch( nKeyCode )
        {
        case KEY_PAGEUP:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_P:
        case KEY_HOME:
        case KEY_END:
            // these keys will be handled by the slide show even
            // while in end mode
            break;
        default:
            TerminateShow();
            bReturn = true;
        }
    }
    else if( SHOWWINDOWMODE_BLANK == meShowWindowMode )
    {
        RestartShow();
        bReturn = true;
    }
    else if( SHOWWINDOWMODE_PAUSE == meShowWindowMode )
    {
        const int nKeyCode = rKEvt.GetKeyCode().GetCode();
        switch( nKeyCode )
        {
        case KEY_ESCAPE:
            TerminateShow();
            bReturn = true;
            break;
        case KEY_PAGEUP:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_P:
        case KEY_HOME:
        case KEY_END:
            // these keys will be handled by the slide show even
            // while in end mode
            break;
        default:
            RestartShow();
            bReturn = true;
            break;
        }
    }

    if( !bReturn )
    {
        if( mpViewShell )
            bReturn = mpViewShell->KeyInput(rKEvt, this);

        if( !bReturn )
            Window::KeyInput(rKEvt);
    }

    if( mpViewShell )
        mpViewShell->SetActiveWindow( this );
}

/*************************************************************************
|*
|* MouseButtonDown event
|*
\************************************************************************/

void ShowWindow::MouseButtonDown(const MouseEvent& rMEvt)
{
    if( SHOWWINDOWMODE_PREVIEW == meShowWindowMode )
    {
        TerminateShow();
    }
    else if( SHOWWINDOWMODE_NORMAL == meShowWindowMode )
    {
        if( mpViewShell )
            mpViewShell->MouseButtonDown( rMEvt, this );
    }
    else if( mpViewShell )
    {
        mpViewShell->SetActiveWindow( this );
    }
}

/*************************************************************************
|*
|* MouseMove event
|*
\************************************************************************/

void ShowWindow::MouseMove(const MouseEvent& rMEvt)
{
    if( mpViewShell )
    {
        if( SHOWWINDOWMODE_NORMAL == meShowWindowMode )
        {
            mpViewShell->MouseMove( rMEvt, this );
        }
        else
        {
            mpViewShell->SetActiveWindow( this );
        }
    }
}

/*************************************************************************
|*
|* MouseButtonUp event
|*
\************************************************************************/

void ShowWindow::MouseButtonUp(const MouseEvent& rMEvt)
{
    if( SHOWWINDOWMODE_PREVIEW == meShowWindowMode )
    {
        TerminateShow();
    }
    else if( (SHOWWINDOWMODE_END == meShowWindowMode) && !rMEvt.IsRight() )
    {
        TerminateShow();
    }
    else if( ( SHOWWINDOWMODE_BLANK == meShowWindowMode ) ||
             ( SHOWWINDOWMODE_PAUSE == meShowWindowMode ) )
    {
        RestartShow();
    }
    else if( mpViewShell )
    {
        mpViewShell->MouseButtonUp( rMEvt, this );
    }
}

/*************************************************************************
|*
|* Paint-Event: wenn FuSlideShow noch erreichbar ist, weiterleiten
|*
\************************************************************************/

void ShowWindow::Paint(const Rectangle& rRect)
{
    if( (meShowWindowMode == SHOWWINDOWMODE_NORMAL) || (meShowWindowMode == SHOWWINDOWMODE_PREVIEW) )
    {
/*
        Region aOldClipRegion( GetClipRegion() );

        Region aClipRegion( rRect );
        aClipRegion.Exclude( maPresArea );
        SetClipRegion( aClipRegion );

        DrawWallpaper( rRect, maShowBackground );

        SetClipRegion( aOldClipRegion );
*/
        if( mpSlideshow )
        {
            mpSlideshow->paint(rRect);
        }
        else
        {
            if(mpViewShell )
            {
                Slideshow* pSlideShow = mpViewShell->GetSlideShow();
                if( pSlideShow )
                    pSlideShow->paint(rRect);
                else
                    mpViewShell->Paint(rRect, this);
            }
        }
    }
    else
    {
        DrawWallpaper( rRect, maShowBackground );

        if( SHOWWINDOWMODE_END == meShowWindowMode )
        {
            DrawEndScene();
        }
        else if( SHOWWINDOWMODE_PAUSE == meShowWindowMode )
        {
            DrawPauseScene( FALSE );
        }
        else if( SHOWWINDOWMODE_BLANK == meShowWindowMode )
        {
            DrawBlankScene();
        }
    }
}

/*************************************************************************
|*
|* Notify
|*
\************************************************************************/

long ShowWindow::Notify(NotifyEvent& rNEvt)
{
    long nOK = FALSE;
/*
    if( mpViewShell && rNEvt.GetType() == EVENT_GETFOCUS )
    {
        NotifyEvent aNEvt(EVENT_GETFOCUS, this);
        nOK = mpViewShell->GetViewFrame()->GetWindow().Notify(aNEvt);
    }
*/
    if (!nOK)
        nOK = Window::Notify(rNEvt);

    return nOK;
}


// -----------------------------------------------------------------------------

void ShowWindow::GetFocus()
{
    // Basisklasse
    Window::GetFocus();
/*
    if( mpViewShell )
    {
        NotifyEvent aNEvt(EVENT_GETFOCUS, this);
        mpViewShell->GetViewFrame()->GetWindow().Notify(aNEvt);
    }
*/
}

// -----------------------------------------------------------------------------

void ShowWindow::LoseFocus()
{
    Window::LoseFocus();

    if( SHOWWINDOWMODE_PREVIEW == meShowWindowMode)
        TerminateShow();
}

// -----------------------------------------------------------------------------

void ShowWindow::Resize()
{
    ::sd::Window::Resize();
}

// -----------------------------------------------------------------------------

void ShowWindow::Move()
{
    ::sd::Window::Move();
}

// -----------------------------------------------------------------------------

BOOL ShowWindow::SetEndMode()
{
    if( ( SHOWWINDOWMODE_NORMAL == meShowWindowMode ) && mpViewShell && mpViewShell->GetView() )
    {
        mpViewShell->GetView()->DelWin( this );
        meShowWindowMode = SHOWWINDOWMODE_END;
//      maShowBackground = GetBackground();
//      SetBackground( Wallpaper( Color( COL_BLACK ) ) );
        maShowBackground = Wallpaper( Color( COL_BLACK ) );

        // hide navigator if it is visible
        if( mpViewShell->GetViewFrame()->GetChildWindow( SID_NAVIGATOR ) )
        {
            mpViewShell->GetViewFrame()->ShowChildWindow( SID_NAVIGATOR, FALSE );
            mbShowNavigatorAfterSpecialMode = TRUE;
        }

        Invalidate();
    }

    return( SHOWWINDOWMODE_END == meShowWindowMode );
}

// -----------------------------------------------------------------------------

BOOL ShowWindow::SetPauseMode( sal_Int32 nPageIndexToRestart, sal_Int32 nTimeout, Graphic* pLogo )
{
    if( mpViewShell && mpViewShell->GetSlideShow() && !nTimeout )
    {
        mpViewShell->GetSlideShow()->jumpToPageIndex( nPageIndexToRestart );
    }
    else if( ( SHOWWINDOWMODE_NORMAL == meShowWindowMode ) && mpViewShell && mpViewShell->GetView() )
    {
        mpViewShell->GetView()->DelWin( this );
        mnPauseTimeout = nTimeout;
        mnRestartPageIndex = nPageIndexToRestart;
        meShowWindowMode = SHOWWINDOWMODE_PAUSE;
//      maShowBackground = GetBackground();
//      SetBackground( Wallpaper( Color( COL_BLACK ) ) );
        maShowBackground = Wallpaper( Color( COL_BLACK ) );

        // hide navigator if it is visible
        if( mpViewShell->GetViewFrame()->GetChildWindow( SID_NAVIGATOR ) )
        {
            mpViewShell->GetViewFrame()->ShowChildWindow( SID_NAVIGATOR, FALSE );
            mbShowNavigatorAfterSpecialMode = TRUE;
        }

        if( pLogo )
            maLogo = *pLogo;

        Invalidate();

        if( SLIDE_NO_TIMEOUT != mnPauseTimeout )
            maPauseTimer.Start();
    }

    return( SHOWWINDOWMODE_PAUSE == meShowWindowMode );
}

// -----------------------------------------------------------------------------

BOOL ShowWindow::SetBlankMode( sal_Int32 nPageIndexToRestart, const Color& rBlankColor )
{
    if( ( SHOWWINDOWMODE_NORMAL == meShowWindowMode ) && mpViewShell && mpViewShell->GetView() )
    {
        mpViewShell->GetView()->DelWin( this );
        mnRestartPageIndex = nPageIndexToRestart;
        meShowWindowMode = SHOWWINDOWMODE_BLANK;
//      maShowBackground = GetBackground();
//      SetBackground( Wallpaper( rBlankColor ) );
        maShowBackground = Wallpaper( rBlankColor );

        // hide navigator if it is visible
        if( mpViewShell->GetViewFrame()->GetChildWindow( SID_NAVIGATOR ) )
        {
            mpViewShell->GetViewFrame()->ShowChildWindow( SID_NAVIGATOR, FALSE );
            mbShowNavigatorAfterSpecialMode = TRUE;
        }

        Invalidate();
    }

    return( SHOWWINDOWMODE_BLANK == meShowWindowMode );
}

// -----------------------------------------------------------------------------

void ShowWindow::SetPreviewMode()
{
    meShowWindowMode = SHOWWINDOWMODE_PREVIEW;
}

// -----------------------------------------------------------------------------

void ShowWindow::TerminateShow()
{
    maLogo.Clear();
    maPauseTimer.Stop();
    Erase();
//  SetBackground( maShowBackground );
    maShowBackground = Wallpaper( Color( COL_BLACK ) );
    meShowWindowMode = SHOWWINDOWMODE_NORMAL;
    mnPauseTimeout = SLIDE_NO_TIMEOUT;

    if( mpViewShell )
    {
        // show navigator?
        if( mbShowNavigatorAfterSpecialMode )
        {
            mpViewShell->GetViewFrame()->ShowChildWindow( SID_NAVIGATOR, TRUE );
            mbShowNavigatorAfterSpecialMode = FALSE;
        }

        mpViewShell->GetViewShellBase().StopPresentation();
    }

    mnRestartPageIndex = PAGE_NO_END;
}

// -----------------------------------------------------------------------------

void ShowWindow::RestartShow()
{
    RestartShow( mnRestartPageIndex );
}

// -----------------------------------------------------------------------------

void ShowWindow::RestartShow( sal_Int32 nPageIndexToRestart )

{
    ShowWindowMode eOldShowWindowMode = meShowWindowMode;

    maLogo.Clear();
    maPauseTimer.Stop();
    Erase();
//  SetBackground( maShowBackground );
    maShowBackground = Wallpaper( Color( COL_BLACK ) );
    meShowWindowMode = SHOWWINDOWMODE_NORMAL;
    mnPauseTimeout = SLIDE_NO_TIMEOUT;

    if( mpViewShell )
    {
        sd::Slideshow* pSlideShow = mpViewShell->GetSlideShow();

        if( pSlideShow )
        {
            if( mpViewShell->GetView() )
                mpViewShell->GetView()->AddWin( this );

            if( SHOWWINDOWMODE_BLANK == eOldShowWindowMode )
            {
                pSlideShow->pause(false);
                Invalidate();
            }
            else
            {
                pSlideShow->jumpToPageIndex( nPageIndexToRestart );
            }
        }
    }

    mnRestartPageIndex = PAGE_NO_END;

    // show navigator?
    if( mbShowNavigatorAfterSpecialMode )
    {
        mpViewShell->GetViewFrame()->ShowChildWindow( SID_NAVIGATOR, TRUE );
        mbShowNavigatorAfterSpecialMode = FALSE;
    }
}

// -----------------------------------------------------------------------------

void ShowWindow::DrawPauseScene( BOOL bTimeoutOnly )
{
    const MapMode&  rMap = GetMapMode();
    const Point     aOutOrg( PixelToLogic( Point() ) );
    const Size      aOutSize( GetOutputSize() );
    const Size      aTextSize( LogicToLogic( Size( 0, 14 ), MAP_POINT, rMap ) );
    const Size      aOffset( LogicToLogic( Size( 1000, 1000 ), MAP_100TH_MM, rMap ) );
    String          aText( SdResId( STR_PRES_PAUSE ) );
    BOOL            bDrawn = FALSE;

    Font            aFont( GetSettings().GetStyleSettings().GetMenuFont() );
    const Font      aOldFont( GetFont() );

    aFont.SetSize( aTextSize );
    aFont.SetColor( COL_WHITE );
    aFont.SetCharSet( aOldFont.GetCharSet() );
    aFont.SetLanguage( aOldFont.GetLanguage() );

    if( !bTimeoutOnly && ( maLogo.GetType() != GRAPHIC_NONE ) )
    {
        Size aGrfSize;

        if( maLogo.GetPrefMapMode() == MAP_PIXEL )
            aGrfSize = PixelToLogic( maLogo.GetPrefSize() );
        else
            aGrfSize = LogicToLogic( maLogo.GetPrefSize(), maLogo.GetPrefMapMode(), rMap );

        const Point aGrfPos( Max( aOutOrg.X() + aOutSize.Width() - aGrfSize.Width() - aOffset.Width(), aOutOrg.X() ),
                             Max( aOutOrg.Y() + aOutSize.Height() - aGrfSize.Height() - aOffset.Height(), aOutOrg.Y() ) );

        if( maLogo.IsAnimated() )
            maLogo.StartAnimation( this, aGrfPos, aGrfSize, (long) this );
        else
            maLogo.Draw( this, aGrfPos, aGrfSize );
    }

    if( SLIDE_NO_TIMEOUT != mnPauseTimeout )
    {
        MapMode         aVMap( rMap );
        VirtualDevice   aVDev( *this );

        aVMap.SetOrigin( Point() );
        aVDev.SetMapMode( aVMap );
        aVDev.SetBackground( Wallpaper( Color( COL_BLACK ) ) );

        // set font first, to determine real output height
        aVDev.SetFont( aFont );

        const Size aVDevSize( aOutSize.Width(), aVDev.GetTextHeight() );

        if( aVDev.SetOutputSize( aVDevSize ) )
        {
            // Note: if performance gets an issue here, we can use NumberFormatter directly
            SvtSysLocale                aSysLocale;
            const LocaleDataWrapper&    aLocaleData = aSysLocale.GetLocaleData();

            aText.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " ( " ));
            aText += aLocaleData.getDuration( Time( 0, 0, mnPauseTimeout ) );
            aText.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " )" ));
            aVDev.DrawText( Point( aOffset.Width(), 0 ), aText );
            DrawOutDev( Point( aOutOrg.X(), aOffset.Height() ), aVDevSize, Point(), aVDevSize, aVDev );
            bDrawn = TRUE;
        }
    }

    if( !bDrawn )
    {
        SetFont( aFont );
        DrawText( Point( aOutOrg.X() + aOffset.Width(), aOutOrg.Y() + aOffset.Height() ), aText );
        SetFont( aOldFont );
    }
}

// -----------------------------------------------------------------------------

void ShowWindow::DrawEndScene()
{
    const Font      aOldFont( GetFont() );
    Font            aFont( GetSettings().GetStyleSettings().GetMenuFont() );

    const Point     aOutOrg( PixelToLogic( Point() ) );
    const Size      aTextSize( LogicToLogic( Size( 0, 14 ), MAP_POINT, GetMapMode() ) );
    const String    aText( SdResId( STR_PRES_SOFTEND ) );

    aFont.SetSize( aTextSize );
    aFont.SetColor( COL_WHITE );
    aFont.SetCharSet( aOldFont.GetCharSet() );
    aFont.SetLanguage( aOldFont.GetLanguage() );
    SetFont( aFont );
    DrawText( Point( aOutOrg.X() + aTextSize.Height(), aOutOrg.Y() + aTextSize.Height() ), aText );
    SetFont( aOldFont );
}

// -----------------------------------------------------------------------------

void ShowWindow::DrawBlankScene()
{
    // just blank through background color => nothing to be done here
}

// -----------------------------------------------------------------------------

IMPL_LINK( ShowWindow, PauseTimeoutHdl, Timer*, pTimer )
{
    if( !( --mnPauseTimeout ) )
        RestartShow();
    else
    {
        DrawPauseScene( TRUE );
        pTimer->Start();
    }

    return 0L;
}


void ShowWindow::SetPresentationArea( const Rectangle& rPresArea )
{
    maPresArea = rPresArea;
}

} // end of namespace sd
