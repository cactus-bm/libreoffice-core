/*************************************************************************
 *
 *  $RCSfile: ViewShellBase.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-03-17 11:28:52 $
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

#include "ViewShellBase.hxx"

#ifndef SD_DRAW_DOC_SHELL_HXX
#include "DrawDocShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_SUB_SHELL_MANAGER_HXX
#include "SubShellManager.hxx"
#endif
#ifndef SD_OBJECT_BAR_MANAGER_HXX
#include "ObjectBarManager.hxx"
#endif
#ifndef SD_RESID_HXX
#include "sdresid.hxx"
#endif
#include "strings.hrc"
#ifndef _SFXREQUEST_HXX
#include <sfx2/request.hxx>
#endif
#include "app.hrc"
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _DRAWDOC_HXX
#include "drawdoc.hxx"
#endif
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#include "glob.hrc"
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXEVENT_HXX
#include <sfx2/event.hxx>
#endif
#include <svx/svxids.hrc>
#ifndef SD_DRAW_CONTROLLER_HXX
#include "DrawController.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif

using namespace sd;
#define ViewShellBase
#include "sdslots.hxx"

namespace sd {

TYPEINIT1(ViewShellBase, SfxViewShell);

SFX_IMPL_VIEWFACTORY(ViewShellBase, SdResId(STR_DEFAULTVIEW))
{
    SFX_VIEW_REGISTRATION(DrawDocShell);
}

SFX_IMPL_INTERFACE(ViewShellBase, SfxViewShell, SdResId(STR_DRAWVIEWSHELL))
{
}

ViewShellBase::ViewShellBase (
    SfxViewFrame* pFrame,
    SfxViewShell* pOldShell,
    ViewShell::ShellType eDefaultSubShell)
    : SfxViewShell (pFrame,
        SFX_VIEW_MAXIMIZE_FIRST   |
        SFX_VIEW_OPTIMIZE_EACH    |
        SFX_VIEW_DISABLE_ACCELS   |
        SFX_VIEW_OBJECTSIZE_EMBEDDED |
        SFX_VIEW_CAN_PRINT           |
        SFX_VIEW_HAS_PRINTOPTIONS),
      mpDocShell (static_cast<DrawDocShell*>(
        GetViewFrame()->GetObjectShell())),
      mpDocument (mpDocShell->GetDoc()),
      mpController (NULL),
      maPrintManager (*this)
{
    Construct (eDefaultSubShell);
}




void ViewShellBase::Construct (
    ViewShell::ShellType eDefaultSubShell)
{
    // Now that this new object has (almost) finished its construction
    // we can pass it as argument to the SubShellManager constructor.
    mpSubShellManager = ::std::auto_ptr<SubShellManager>(
        new SubShellManager(*this));
    GetSubShellManager().SetMainSubShellType (eDefaultSubShell);

    StartListening(*GetViewFrame());
    StartListening(*GetDocShell());

    // Make sure that an instance of the controller exists.  Its
    // creation includes registration at the frame.
    GetDrawController();
}




ViewShellBase::~ViewShellBase (void)
{
    EndListening(*GetViewFrame());
    EndListening(*GetDocShell());
    mpSubShellManager.reset();
}




SubShellManager& ViewShellBase::GetSubShellManager (void) const
{
    return *mpSubShellManager.get();
}




ObjectBarManager& ViewShellBase::GetObjectBarManager (void) const
{
    return mpSubShellManager->GetObjectBarManager();
}




ViewShell* ViewShellBase::GetMainViewShell (SfxViewFrame* pViewFrame)
{
    ViewShell* pViewShell = NULL;

    if (pViewFrame != NULL)
    {
        // Get the view shell for the frame and cast it to
        // sd::ViewShellBase.
        SfxViewShell* pSfxViewShell = pViewFrame->GetViewShell();
        if (pSfxViewShell->ISA(::sd::ViewShellBase))
        {
            ViewShellBase* pSdViewShellBase =
                PTR_CAST(::sd::ViewShellBase, pSfxViewShell);
            if (pSdViewShellBase != NULL)
            {
                // Use the main sub shell manager to obtain the
                // desired view shell.
                pViewShell =
                    pSdViewShellBase->GetSubShellManager().GetMainSubShell();
            }
        }
    }

    return pViewShell;
}




void ViewShellBase::ExecuteModeChange (SfxRequest& rRequest)
{
    bool bIsActive = true;
    ViewShell::ShellType eType = ViewShell::ST_NONE;

    switch (rRequest.GetSlot())
    {
        case SID_SWITCH_SHELL:
        {
            SFX_REQUEST_ARG(rRequest, pMode, SfxInt32Item,
                SID_SWITCH_SHELL, FALSE);
            eType = static_cast<ViewShell::ShellType>(
                pMode->GetValue());
            switch (eType)
            {
                case ViewShell::ST_IMPRESS:
                case ViewShell::ST_HANDOUT:
                case ViewShell::ST_NOTES:
                case ViewShell::ST_SLIDE:
                case ViewShell::ST_OUTLINE:
                {
                    // Turn off effects.
                    ViewShell* pShell = GetSubShellManager().GetMainSubShell();
                    /*af the current shell will be destroyed in a short time
                         so calling SetAnimationMode() should not be
                         necessary.

                    if (pShell != NULL)
                        if (pShell->ISA(DrawViewShell))
                            static_cast<DrawView*>(pShell->GetDrawView())
                                ->SetAnimationMode(FALSE);
                        */
                    // AutoLayouts have to be ready.
                    GetDocument()->StopWorkStartupDelay();
                    GetSubShellManager().SetMainSubShellType (eType);
                    //          Invalidate ();
                    rRequest.Ignore ();
                }
                break;

                case ViewShell::ST_NONE:
                case ViewShell::ST_PRESENTATION:
                    GetSubShellManager().SetMainSubShellType (eType);
                    rRequest.Done ();
                    break;

                case ViewShell::ST_DRAW:
                case ViewShell::ST_PREVIEW:
                    break;
            }
        }
    }
}




void ViewShellBase::GetMenuState (SfxItemSet& rSet)
{
}




DrawController* ViewShellBase::GetDrawController (void)
{
    if (mpController == NULL)
    {
        ::osl::MutexGuard aGuard (maMutex);
        if (mpController == NULL)
            mpController = new DrawController (*this);
    }
    return mpController;
}




DrawDocShell* ViewShellBase::GetDocShell (void) const
{
    return mpDocShell;
}



SdDrawDocument* ViewShellBase::GetDocument (void) const
{
    return mpDocument;
}




void ViewShellBase::SFX_NOTIFY(SfxBroadcaster& rBC,
    const TypeId& rBCType,
    const SfxHint& rHint,
    const TypeId& rHintType)
{
    SfxViewShell::SFX_NOTIFY(rBC, rBCType, rHint, rHintType);

    if (rHint.IsA(TYPE(SfxEventHint)))
    {
        if( ((SfxEventHint&)rHint).GetEventId() == SFX_EVENT_OPENDOC )
        {
            if( GetDocument() && GetDocument()->IsStartWithPresentation() )
            {
                if( GetViewFrame() )
                {
                    GetDocument()->SetStartWithPresentation( false );
                    GetViewFrame()->GetDispatcher()->Execute(
                        SID_PRESENTATION, SFX_CALLMODE_ASYNCHRON );
                }
            }
        }
    }
}




void ViewShellBase::InnerResizePixel(const Point &rPos, const Size &rSize)
{
    SfxViewShell::InnerResizePixel (rPos, rSize);
    ViewShell* pMainViewShell = GetSubShellManager().GetMainSubShell();
    if (pMainViewShell != NULL)
        pMainViewShell->InnerResizePixel (rPos, rSize);
}




void ViewShellBase::OuterResizePixel(const Point &rPos, const Size &rSize)
{
    /*af
    SfxViewShell::OuterResizePixel (rPos, rSize);
    ViewShell* pMainViewShell = GetSubShellManager().GetMainSubShell();
    if (pMainViewShell != NULL)
        pMainViewShell->OuterResizePixel (rPos, rSize);
    */
}




ErrCode ViewShellBase::DoVerb (long nVerb)
{
    ErrCode aResult = ERRCODE_NONE;

    ViewShell* pShell = GetSubShellManager().GetMainSubShell();
    if (pShell != NULL)
        aResult = pShell->DoVerb (nVerb);

    return aResult;
}




SfxPrinter* ViewShellBase::GetPrinter (BOOL bCreate)
{
    return maPrintManager.GetPrinter (bCreate);
}




USHORT ViewShellBase::SetPrinter (
    SfxPrinter* pNewPrinter,

    USHORT nDiffFlags)
{
    return maPrintManager.SetPrinter (pNewPrinter, nDiffFlags);
}




PrintDialog* ViewShellBase::CreatePrintDialog (::Window *pParent)
{
    return maPrintManager.CreatePrintDialog (pParent);
}




SfxTabPage*  ViewShellBase::CreatePrintOptionsPage(
    ::Window *pParent,
    const SfxItemSet &rOptions)
{
    return maPrintManager.CreatePrintOptionsPage (pParent, rOptions);
}




USHORT  ViewShellBase::Print(SfxProgress& rProgress, PrintDialog* pDlg)
{
    return maPrintManager.Print (rProgress, pDlg);
}




ErrCode ViewShellBase::DoPrint (
    SfxPrinter* pPrinter,
    PrintDialog* pPrintDialog,
    BOOL bSilent)
{
    return maPrintManager.DoPrint (pPrinter, pPrintDialog, bSilent);
}




USHORT ViewShellBase::SetPrinterOptDlg (
    SfxPrinter* pNewPrinter,
    USHORT nDiffFlags,
    BOOL bShowDialog)
{
   return maPrintManager.SetPrinterOptDlg (
       pNewPrinter, nDiffFlags, bShowDialog);
}




void ViewShellBase::PreparePrint (PrintDialog* pPrintDialog)
{
    return maPrintManager.PreparePrint (pPrintDialog);
}



} // end of namespace sd
