/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: presvish.cxx,v $
 *
 *  $Revision: 1.29 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:47:42 $
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

#include "PresentationViewShell.hxx"

#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFX_TOPFRM_HXX
#include <sfx2/topfrm.hxx>
#endif
#ifndef _SFX_DISPATCH_HXX
#include <sfx2/dispatch.hxx>
#endif

#include <sfx2/objface.hxx>

#include <svx/svxids.hrc>
#include <sfx2/app.hxx>
#ifndef SD_FRAME_VIEW
#include "FrameView.hxx"
#endif
#include "sdresid.hxx"
#include "DrawDocShell.hxx"
#ifndef _SD_SLIDESHOW_HXX
#include "slideshow.hxx"
#endif
#include "sdattr.hxx"
#include "sdpage.hxx"
#include "drawdoc.hxx"
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#include "app.hrc"
#include "strings.hrc"
#include "glob.hrc"
#include "PaneManager.hxx"
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_FACTORY_IDS_HXX
#include "FactoryIds.hxx"
#endif

// #110496#
#include "slideshow.hxx"
#include "fupoor.hxx"
#include "Window.hxx"

#define PresentationViewShell
using namespace sd;
#include "sdslots.hxx"


namespace sd {

// -------------------
// - PresentationViewShell -
// -------------------

SFX_IMPL_INTERFACE( PresentationViewShell, DrawViewShell, SdResId( STR_PRESVIEWSHELL ) )
{
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_TOOLS | SFX_VISIBILITY_STANDARD |
                                SFX_VISIBILITY_FULLSCREEN | SFX_VISIBILITY_SERVER,
                                SdResId(RID_DRAW_TOOLBOX));
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_APPLICATION | SFX_VISIBILITY_DESKTOP | SFX_VISIBILITY_STANDARD | SFX_VISIBILITY_CLIENT | SFX_VISIBILITY_VIEWER | SFX_VISIBILITY_READONLYDOC,
                                SdResId(RID_DRAW_VIEWER_TOOLBOX) );
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OPTIONS | SFX_VISIBILITY_STANDARD |
                                SFX_VISIBILITY_SERVER,
                                SdResId(RID_DRAW_OPTIONS_TOOLBOX));
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_COMMONTASK | SFX_VISIBILITY_STANDARD |
                                SFX_VISIBILITY_SERVER,
                                SdResId(RID_DRAW_COMMONTASK_TOOLBOX));
}


TYPEINIT1( PresentationViewShell, DrawViewShell );




PresentationViewShell::PresentationViewShell (
    SfxViewFrame* pFrame,
    ViewShellBase& rViewShellBase,
    ::Window* pParentWindow,
    FrameView* pFrameView)
    : DrawViewShell (
        pFrame,
        rViewShellBase,
        pParentWindow,
        PK_STANDARD,
        pFrameView),
    mbShowStarted( sal_False )
{
    if( GetDocSh() && GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        maOldVisArea = GetDocSh()->GetVisArea( ASPECT_CONTENT );
    meShellType = ST_PRESENTATION;
}




PresentationViewShell::PresentationViewShell (
    SfxViewFrame* pFrame,
    ::Window* pParentWindow,
    const DrawViewShell& rShell)
    : DrawViewShell (pFrame, pParentWindow, rShell),
      mbShowStarted( sal_False )
{
    if( GetDocSh() && GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        maOldVisArea = GetDocSh()->GetVisArea( ASPECT_CONTENT );
    meShellType = ST_PRESENTATION;
}




PresentationViewShell::~PresentationViewShell (void)
{
    if( GetDocSh() && GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED && !maOldVisArea.IsEmpty() )
        GetDocSh()->SetVisArea( maOldVisArea );

    if( GetViewFrame() && GetViewFrame()->GetTopFrame() )
    {
        WorkWindow* pWorkWindow = (WorkWindow*) GetViewFrame()->GetTopFrame()->GetWindow().GetParent();

        if( pWorkWindow )
            pWorkWindow->StartPresentationMode( FALSE, mpSlideShow ? mpSlideShow->isAlwaysOnTop() : 0 );
    }

    if( mpSlideShow )
    {
        mpSlideShow->deactivate();
        mpSlideShow->stopShow();
        mpSlideShow->dispose();
        delete mpSlideShow;
        mpSlideShow = NULL;
    }
}




void PresentationViewShell::FinishInitialization (
    FrameView* pFrameView,
    SfxRequest& rRequest,
    USHORT nPageNumber)
{
    DrawViewShell::Init(true);

    // Use the frame view that comes form the view shell that initiated our
    // creation.
    if (pFrameView != NULL)
    {
        GetFrameView()->Disconnect();
        SetFrameView (pFrameView);
        pFrameView->Connect();
    }
    SetRuler(false);
    SwitchPage (nPageNumber);
    WriteFrameViewData();

    mpSlideShow = new sd::Slideshow( this, GetView(), GetDoc() );
    mpSlideShow->setRehearseTimings(
        rRequest.GetSlot() == SID_REHEARSE_TIMINGS );
    GetActiveWindow()->GrabFocus();

    // Start the show.
    if (mpSlideShow->startShow(0))
        mbShowStarted = sal_True;
    else
    {
        delete mpSlideShow;
        mpSlideShow = 0;
    }

    GetViewFrame()->Show();

    Activate(TRUE);
}


SvxRuler* PresentationViewShell::CreateHRuler(::sd::Window* pWin, BOOL bIsFirst)
{
    return NULL;
}

SvxRuler* PresentationViewShell::CreateVRuler(::sd::Window* pWin)
{
    return NULL;
}


void PresentationViewShell::Activate( BOOL bIsMDIActivate )
{
    DrawViewShell::Activate( bIsMDIActivate );

    if( bIsMDIActivate )
    {
        ::sd::View*     pView = GetView();
        SfxBoolItem aItem( SID_NAVIGATOR_INIT, TRUE );

        GetViewFrame()->GetDispatcher()->Execute( SID_NAVIGATOR_INIT, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

        if( mpSlideShow)
            mpSlideShow->activate();

        if( HasCurrentFunction() )
            GetCurrentFunction()->Activate();

        if( pView )
            pView->ShowMarkHdl( NULL );
    }

    if( bIsMDIActivate )
        ReadFrameViewData( pFrameView );
    GetDocSh()->Connect( this );

    if( mpSlideShow && !mbShowStarted )
    {
        if (mpSlideShow->startShow(0))
            mbShowStarted = sal_True;
        else {
            delete mpSlideShow;
            mpSlideShow = 0;
        }
    }
}




void PresentationViewShell::Paint( const Rectangle& rRect, ::sd::Window* pWin )
{
    // allow paints only if show is already started
    if( mbShowStarted && mpSlideShow )
        mpSlideShow->paint(rRect);
}




void PresentationViewShell::CreateFullScreenShow (
    ViewShell* pOriginShell,
    SfxRequest& rRequest)
{
    SdDrawDocument* pDoc = pOriginShell->GetDoc();
    SdPage* pCurrentPage = pOriginShell->GetActualPage();

    SFX_REQUEST_ARG (rRequest, pAlwaysOnTop, SfxBoolItem,
        ATTR_PRESENT_ALWAYS_ON_TOP, FALSE);
    bool bAlwaysOnTop =
        ((rRequest.GetSlot() !=  SID_REHEARSE_TIMINGS) && pAlwaysOnTop )
        ? pAlwaysOnTop->GetValue()
        : pDoc->getPresentationSettings().mbAlwaysOnTop;

    WorkWindow* pWorkWindow = new WorkWindow (
        NULL,
        WB_HIDE | WB_CLIPCHILDREN);
    pWorkWindow->StartPresentationMode (
        TRUE,
        bAlwaysOnTop ? PRESENTATION_HIDEALLAPPS : 0);
    pWorkWindow->SetBackground(Wallpaper(COL_BLACK));
    if (pWorkWindow->IsVisible())
    {
        // The new frame is created hidden.  To make it visible and activate
        // the new view shell--a prerequisite to process slot calls and
        // initialize its panes--a GrabFocus() has to be called later on.
        SfxTopFrame* pNewFrame = SfxTopFrame::Create (
            pDoc->GetDocSh(),
            pWorkWindow,
            PRESENTATION_FACTORY_ID,
            TRUE);
        pNewFrame->SetPresentationMode (TRUE);

        ViewShellBase* pBase = static_cast<ViewShellBase*>(
            pNewFrame->GetCurrentViewFrame()->GetViewShell());
        if (pBase != NULL)
        {
            // Get the page where the show is to be started.  This normally
            // is the current page of the shell from which the show has been
            // started.  This, however, may be NULL, e.g. when started from
            // the slide sorter and that has an empty selection.
            USHORT nStartPage = 0;
            if (pCurrentPage != NULL)
                nStartPage = (pCurrentPage->GetPageNum() - 1) / 2;

//            pBase->GetViewFrame()->Show();
            // The following GrabFocus() is responsible for activating the
            // new view shell.  Without it the screen remains blank (under
            // Windows and some Linux variants.)
            pBase->GetWindow()->GrabFocus();

            PresentationViewShell* pShell = PTR_CAST(PresentationViewShell, pBase->GetMainViewShell());
            if (pShell != NULL)
                pShell->FinishInitialization (
                    pOriginShell->GetFrameView(),
                    rRequest,
                    nStartPage);
        }
    }
}

} // end of namespace sd
