/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ViewShellImplementation.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: rt $ $Date: 2006-01-10 14:33:04 $
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
#include "ViewShellImplementation.hxx"

#include "sdpage.hxx"
#include "drawdoc.hxx"
#include "sdresid.hxx"
#include "glob.hrc"
#include "app.hrc"
#include "new_foil.hrc"
#include "strings.hrc"
#include "strings.hrc"
#include "helpids.h"
#include "sdattr.hxx"
#include "sdabstdlg.hxx"
#include "unmodpg.hxx"
#include "Window.hxx"
#include "optsitem.hxx"
#include "DrawDocShell.hxx"
#include "FactoryIds.hxx"
#include "slideshow.hxx"
#include "TaskPaneViewShell.hxx"
#include "ViewShellBase.hxx"

#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/request.hxx>
#include <svtools/aeitem.hxx>
#include <vcl/msgbox.hxx>
#include <basic/sbstar.hxx>
#include "undo/undoobjects.hxx"


namespace sd {

ViewShell::Implementation::Implementation (ViewShell& rViewShell)
    : mbIsShowingUIControls(false),
      mbIsMainViewShell(false),
      mrViewShell (rViewShell),
      mbIsInitialized(false)
{
}




ViewShell::Implementation::~Implementation (void)
{
}




void ViewShell::Implementation::ProcessModifyPageSlot (
    SfxRequest& rRequest,
    SdPage* pCurrentPage,
    PageKind ePageKind)
{
    SdDrawDocument* pDocument = mrViewShell.GetDoc();
    SdrLayerAdmin& rLayerAdmin = pDocument->GetLayerAdmin();
    BYTE aBckgrnd = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRND)), FALSE);
    BYTE aBckgrndObj = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRNDOBJ)), FALSE);
    SetOfByte aVisibleLayers;
    BOOL bHandoutMode = FALSE;
    SdPage* pHandoutMPage = NULL;
    String aNewName;

    // #95981#
    String aOldName;

    AutoLayout aNewAutoLayout;

    // #95981#
    AutoLayout aOldAutoLayout;

    BOOL bBVisible;
    BOOL bBObjsVisible;
    const SfxItemSet* pArgs = rRequest.GetArgs();

    if (pCurrentPage->TRG_HasMasterPage())
        aVisibleLayers = pCurrentPage->TRG_GetMasterPageVisibleLayers();
    else
        aVisibleLayers.SetAll();

    do
    {
        if (!pArgs || pArgs->Count() == 1 || pArgs->Count() == 2 )
        {
            if (pArgs && pArgs->Count() == 2)
            {
                // We have been called with a request that contains two
                // arguments.  One was used as preselected layout in a
                // dialog.  We could select that layout in the
                // layout panel instead.
                /*
                    SFX_REQUEST_ARG (rRequest, pNewAutoLayout, SfxUInt32Item, ID_VAL_WHATLAYOUT, FALSE);
                    eNewAutoLayout = (AutoLayout) pNewAutoLayout->GetValue
                    ();
                */
            }

            // Make the layout menu visible in the tool pane.
            SfxBoolItem aMakeToolPaneVisible (ID_VAL_ISVISIBLE, TRUE);
            SfxUInt32Item aPanelId (ID_VAL_PANEL_INDEX,
                ::sd::toolpanel::TaskPaneViewShell::PID_LAYOUT);
            SfxViewFrame* pFrame = mrViewShell.GetViewFrame();
            if (pFrame!=NULL && pFrame->GetDispatcher()!=NULL)
            {
                pFrame->GetDispatcher()->Execute (
                    SID_TASK_PANE,
                    SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD,
                    &aMakeToolPaneVisible,
                    &aPanelId,
                    NULL);
            }
            else
            {
                DBG_ASSERT(pFrame!=NULL && pFrame->GetDispatcher()!=NULL,
                    "ViewShell::Implementation::ProcessModifyPageSlot(): can not get dispatcher");
            }

            // We have activated a non-modal control in the task pane.
            // Because it does not return anything we can not do anything
            // more right now and have to exit here.
            break;
        }
        else if (pArgs->Count() == 4)
        {
            SFX_REQUEST_ARG (rRequest, pNewName, SfxStringItem, ID_VAL_PAGENAME, FALSE);
            SFX_REQUEST_ARG (rRequest, pNewAutoLayout, SfxUInt32Item, ID_VAL_WHATLAYOUT, FALSE);
            SFX_REQUEST_ARG (rRequest, pBVisible, SfxBoolItem, ID_VAL_ISPAGEBACK, FALSE);
            SFX_REQUEST_ARG (rRequest, pBObjsVisible, SfxBoolItem, ID_VAL_ISPAGEOBJ, FALSE);
            AutoLayout aLayout ((AutoLayout)pNewAutoLayout->GetValue ());
            if (aLayout >= AUTOLAYOUT__START
                && aLayout < AUTOLAYOUT__END)
            {
                aNewName        = pNewName->GetValue ();
                aNewAutoLayout = (AutoLayout) pNewAutoLayout->GetValue ();
                bBVisible       = pBVisible->GetValue ();
                bBObjsVisible   = pBObjsVisible->GetValue ();
            }
            else
            {
                StarBASIC::FatalError (SbERR_BAD_PROP_VALUE);
                rRequest.Ignore ();
                break;
            }
            if (ePageKind == PK_HANDOUT)
            {
                bHandoutMode = TRUE;
                pHandoutMPage = pDocument->GetMasterSdPage(0, PK_HANDOUT);
            }
        }
        else
        {
            StarBASIC::FatalError (SbERR_WRONG_ARGS);
            rRequest.Ignore ();
            break;
        }

        SdPage* pUndoPage =
            bHandoutMode ? pHandoutMPage : pCurrentPage;

        SfxUndoManager* pUndoManager = mrViewShell.GetDocSh()->GetUndoManager();
        DBG_ASSERT(pUndoManager, "No UNDO MANAGER ?!?");

        if( pUndoManager )
        {
            String aComment( SdResId(STR_UNDO_MODIFY_PAGE) );
            pUndoManager->EnterListAction(aComment, aComment);
            ModifyPageUndoAction* pAction = new ModifyPageUndoAction(
                pUndoManager, pDocument, pUndoPage, aNewName, aNewAutoLayout, bBVisible, bBObjsVisible);
            pUndoManager->AddUndoAction(pAction);

            // Clear the selection because the selectec object may be removed as
            // a result of the ssignment of the layout.
            mrViewShell.GetDrawView()->UnmarkAll();

            if (!bHandoutMode)
            {
                if (pCurrentPage->GetName() != aNewName)
                {
                    pCurrentPage->SetName(aNewName);

                    if (ePageKind == PK_STANDARD)
                    {
                        USHORT nPage = (pCurrentPage->GetPageNum()-1) / 2;
                        SdPage* pNotesPage = pDocument->GetSdPage(nPage, PK_NOTES);
                        if (pNotesPage != NULL)
                            pNotesPage->SetName(aNewName);
                    }
                }

                pCurrentPage->SetAutoLayout(aNewAutoLayout, TRUE);

                aBckgrnd = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRND)), FALSE);
                aBckgrndObj = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRNDOBJ)), FALSE);
                aVisibleLayers.Set(aBckgrnd, bBVisible);
                aVisibleLayers.Set(aBckgrndObj, bBObjsVisible);
                pCurrentPage->TRG_SetMasterPageVisibleLayers(aVisibleLayers);
            }
            else
            {
                pHandoutMPage->SetAutoLayout(aNewAutoLayout, TRUE);
            }

            mrViewShell.GetViewFrame()->GetDispatcher()->Execute(SID_SWITCHPAGE,
                SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);

            BOOL bSetModified = TRUE;

            if (pArgs && pArgs->Count() == 1)
            {
                bSetModified = (BOOL) ((SfxBoolItem&) pArgs->Get(SID_MODIFYPAGE)).GetValue();
            }

            pUndoManager->AddUndoAction( new UndoAutoLayoutPosAndSize( *pUndoPage ) );
            pUndoManager->LeaveListAction();

            pDocument->SetChanged(bSetModified);
        }
    }
    while (false);

    mrViewShell.Cancel();
    rRequest.Done ();
}




void ViewShell::Implementation::AssignLayout (
    SdPage* pPage,
    AutoLayout aLayout)
{
    // Transform the given request into the four argument form that is
    // understood by ProcessModifyPageSlot().
    SdrLayerAdmin& rLayerAdmin (mrViewShell.GetViewShellBase().GetDocument()->GetLayerAdmin());
    BYTE aBackground (rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRND)), FALSE));
    BYTE aBackgroundObject (rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRNDOBJ)), FALSE));
    SetOfByte aVisibleLayers (pPage->TRG_GetMasterPageVisibleLayers());
    SfxRequest aRequest (mrViewShell.GetViewShellBase().GetViewFrame(), SID_MODIFYPAGE);
    aRequest.AppendItem(SfxStringItem (ID_VAL_PAGENAME, pPage->GetName()));
    aRequest.AppendItem(SfxUInt32Item (ID_VAL_WHATLAYOUT, aLayout));
    aRequest.AppendItem(SfxBoolItem(ID_VAL_ISPAGEBACK, aVisibleLayers.IsSet(aBackground)));
    aRequest.AppendItem(SfxBoolItem(ID_VAL_ISPAGEOBJ, aVisibleLayers.IsSet(aBackgroundObject)));

    // Forward the call with the new arguments.
    ProcessModifyPageSlot (
        aRequest,
        pPage,
        pPage->GetPageKind());
}




sal_uInt16 ViewShell::Implementation::GetViewId (void)
{
    switch (mrViewShell.GetShellType())
    {
        case ViewShell::ST_IMPRESS:
        case ViewShell::ST_NOTES:
        case ViewShell::ST_HANDOUT:
            return IMPRESS_FACTORY_ID;

        case ViewShell::ST_DRAW:
            return DRAW_FACTORY_ID;

        case ViewShell::ST_OUTLINE:
            return OUTLINE_FACTORY_ID;

        case ViewShell::ST_SLIDE:
        case ViewShell::ST_SLIDE_SORTER:
            return SLIDE_SORTER_FACTORY_ID;

        case ViewShell::ST_PRESENTATION:
            return PRESENTATION_FACTORY_ID;

        // Since we have to return a view id for every possible shell type
        // and there is not (yet) a proper ViewShellBase sub class for the
        // remaining types we chose the Impress factory as a fall back.
        case ViewShell::ST_TASK_PANE:
        case ViewShell::ST_NONE:
        default:
            return IMPRESS_FACTORY_ID;
    }
}



} // end of namespace sd
