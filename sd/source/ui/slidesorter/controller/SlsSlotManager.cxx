/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsSlotManager.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-23 11:02:01 $
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
#include "SlsSlotManager.hxx"

#include "SlideSorterViewShell.hxx"
#include "controller/SlideSorterController.hxx"
#include "controller/SlsPageSelector.hxx"
#include "controller/SlsClipboard.hxx"
#include "controller/SlsSelectionFunction.hxx"
#include "controller/SlsFocusManager.hxx"
#include "SlsHideSlideFunction.hxx"
#include "SlsCommand.hxx"
#include "model/SlideSorterModel.hxx"
#include "model/SlsPageDescriptor.hxx"
#include "model/SlsPageEnumeration.hxx"
#include "view/SlideSorterView.hxx"
#include "view/SlsLayouter.hxx"
#include "view/SlsViewOverlay.hxx"

#include "Window.hxx"
#include "fupoor.hxx"
#include "fuzoom.hxx"
#include "fucushow.hxx"
#include "fusldlg.hxx"
#include "fuexpand.hxx"
#include "fusumry.hxx"
#include "fuscale.hxx"
#include "slideshow.hxx"
#include "app.hrc"
#include "strings.hrc"
#include "sdresid.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "PaneManager.hxx"
#include "ViewShellBase.hxx"
#include "ViewShellImplementation.hxx"
#include "sdattr.hxx"
#include "PresentationViewShell.hxx"
#include "TaskPaneViewShell.hxx"
#include "FrameView.hxx"
#include "zoomlist.hxx"
#include "sdpage.hxx"
#include "sdxfer.hxx"
#include "helpids.h"
#include "glob.hrc"
#include "unmodpg.hxx"

#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/topfrm.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/svxids.hrc>
#include <svx/zoomitem.hxx>
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
#include <vcl/msgbox.hxx>
#include <svtools/intitem.hxx>
#include <svtools/whiter.hxx>
#include <svtools/itempool.hxx>
#include <svtools/aeitem.hxx>

#ifndef _COM_SUN_STAR_PRESENTATION_FADEEFFECT_HPP_
#include <com/sun/star/presentation/FadeEffect.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XMASTERPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGES_HPP_
#include <com/sun/star/drawing/XDrawPages.hpp>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sd { namespace slidesorter { namespace controller {


SlotManager::SlotManager (SlideSorterController& rController)
    : mrController (rController)
{
}




SlotManager::~SlotManager (void)
{
}




void SlotManager::FuTemporary (SfxRequest& rRequest)
{
    SdDrawDocument* pDocument = mrController.GetModel().GetDocument();

    SlideSorterViewShell& rShell (mrController.GetViewShell());
    if (rShell.GetActualFunction() != NULL)
    {
        rShell.GetActualFunction()->Deactivate();
        if (rShell.GetActualFunction() != rShell.GetOldFunction())
            delete rShell.GetActualFunction();
        rShell.SetCurrentFunction (NULL);
    }

    ::sd::Window* pWindow = rShell.GetActiveWindow();
    switch (rRequest.GetSlot())
    {
        case SID_PRESENTATION:
        case SID_REHEARSE_TIMINGS:
            ShowSlideShow (rRequest);
            rShell.Cancel();
            break;

        case SID_HIDE_SLIDE:
            rShell.SetCurrentFunction (
                new HideSlideFunction (
                    mrController,
                    rRequest));
            rShell.Cancel();
            break;

        case SID_ATTR_ZOOM:
        case SID_ZOOM_OUT:
        case SID_SIZE_REAL:
        case SID_ZOOM_IN:
        case SID_SIZE_ALL:
        case SID_SIZE_PAGE:
            HandleSizeSlots (rRequest);
            break;

        case SID_PAGES_PER_ROW:
            if (rRequest.GetArgs() != NULL)
            {
                SFX_REQUEST_ARG(rRequest, pPagesPerRow, SfxUInt16Item,
                    SID_PAGES_PER_ROW, FALSE);
                if (pPagesPerRow != NULL)
                {
                    sal_Int32 nColumnCount = pPagesPerRow->GetValue();
                    // Force the given number of columns by setting the
                    // minimal and maximal number of columns to the same
                    // value.
                    mrController.GetView().GetLayouter().SetColumnCount (
                        nColumnCount, nColumnCount);
                    // Force a repaint and re-layout.
                    rShell.ArrangeGUIElements ();
                    // Rearrange the UI-elements controlled by the
                    // controller and force a rearrangement of the view.
                    mrController.Rearrange(true);
                }
            }
            rShell.Cancel();
            rRequest.Done();
            break;

        case SID_SELECTALL:
            mrController.GetPageSelector().SelectAllPages();
            rShell.Cancel();
            rRequest.Done();
            break;

        case SID_SLIDE_TRANSITIONS_PANEL:
        {
            // Make the slide transition panel visible (expand it) in the
            // tool pane.
            SfxBoolItem aMakeToolPaneVisible (ID_VAL_ISVISIBLE, TRUE);
            SfxUInt32Item aPanelId (ID_VAL_PANEL_INDEX,
                toolpanel::TaskPaneViewShell::PID_SLIDE_TRANSITION);
            rShell.GetViewFrame()->GetDispatcher()->Execute(
                SID_TASK_PANE,
                SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD,
                &aMakeToolPaneVisible,
                &aPanelId,
                NULL);

            rShell.Cancel();
            rRequest.Ignore ();
            break;
        }

        case SID_PRESENTATION_DLG:
            rShell.SetCurrentFunction (
                new FuSlideShowDlg (
                    &rShell,
                    mrController.GetView().GetWindow(),
                    &mrController.GetView(),
                    pDocument,
                    rRequest));
            rShell.Cancel();
            break;

        case SID_CUSTOMSHOW_DLG:
            rShell.SetCurrentFunction (
                new FuCustomShowDlg (
                    &rShell,
                    mrController.GetView().GetWindow(),
                    &mrController.GetView(),
                    pDocument,
                    rRequest));
            rShell.Cancel();
            break;

        case SID_EXPAND_PAGE:
            rShell.SetCurrentFunction (
                new FuExpandPage (
                    &rShell,
                    mrController.GetView().GetWindow(),
                    &mrController.GetView(),
                    pDocument,
                    rRequest));
            rShell.Cancel();
            break;

        case SID_SUMMARY_PAGE:
            rShell.SetCurrentFunction (
                new FuSummaryPage (
                    &rShell,
                    mrController.GetView().GetWindow(),
                    &mrController.GetView(),
                    pDocument,
                    rRequest));
            rShell.Cancel();
            break;

        case SID_INSERTPAGE:
        case SID_INSERT_MASTER_PAGE:
            InsertSlide(rRequest);
            rShell.Cancel();
            rRequest.Done();
            break;

        case SID_DELETE_PAGE:
        case SID_DELETE_MASTER_PAGE:
        case SID_DELETE: // we need SID_CUT to handle the delete key
                      // (DEL -> accelerator -> SID_CUT).
             if (mrController.GetModel().GetPageCount() > 1)
             {
                 mrController.DeleteSelectedPages();
             }

             rShell.Cancel();
             rRequest.Done();
             break;

        case SID_RENAMEPAGE:
        case SID_RENAME_MASTER_PAGE:
            RenameSlide ();
            rShell.Cancel();
            rRequest.Done ();
            break;

        case SID_ASSIGN_LAYOUT:
        {
            SFX_REQUEST_ARG (rRequest, pWhatPage, SfxUInt32Item, ID_VAL_WHATPAGE, FALSE);
            SFX_REQUEST_ARG (rRequest, pWhatLayout, SfxUInt32Item, ID_VAL_WHATLAYOUT, FALSE);
            mrController.GetViewShell().mpImpl->AssignLayout(
                pDocument->GetSdPage((USHORT)pWhatPage->GetValue(),
                    mrController.GetModel().GetPageType()),
                (AutoLayout)pWhatLayout->GetValue());
            rShell.Cancel();
            rRequest.Done ();
        }
        break;

        default:
            break;
    }

    if (rShell.GetActualFunction() != NULL)
    {
        rShell.GetActualFunction()->Activate();
    }
}




void SlotManager::FuPermanent (SfxRequest& rRequest)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());
    if (rShell.GetActualFunction() != NULL)
    {
        if (rShell.GetOldFunction() == rShell.GetActualFunction())
            rShell.SetOldFunction (NULL);

        rShell.GetActualFunction()->Deactivate();
        delete rShell.GetActualFunction();
        rShell.SetCurrentFunction (NULL);
    }

    switch (rRequest.GetSlot())
    {
        case SID_OBJECT_SELECT:
            rShell.SetCurrentFunction (
                new SelectionFunction (mrController, rRequest));
            rRequest.Done();
            break;

        default:
                break;
    }

    if (rShell.GetOldFunction() != NULL)
    {
        rShell.GetOldFunction()->Deactivate();
        delete rShell.GetOldFunction();
        rShell.SetOldFunction (NULL);
    }

    if (rShell.GetActualFunction() != NULL)
    {
        rShell.GetActualFunction()->Activate();
        rShell.SetOldFunction (rShell.GetActualFunction());
    }

    //! das ist nur bis das ENUM-Slots sind
    //  Invalidate( SID_OBJECT_SELECT );
}




void SlotManager::FuSupport (SfxRequest& rRequest)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());
    switch (rRequest.GetSlot())
    {
        case SID_STYLE_FAMILY:
            if (rRequest.GetArgs() != NULL)
            {
                SdDrawDocument* pDocument
                    = mrController.GetModel().GetDocument();
                if (pDocument != NULL)
                {
                    const SfxPoolItem& rItem (
                        rRequest.GetArgs()->Get(SID_STYLE_FAMILY));
                    pDocument->GetDocSh()->SetStyleFamily(
                        static_cast<const SfxUInt16Item&>(rItem).GetValue());
                }
            }
            break;

        case SID_CUT:
        case SID_COPY:
        case SID_PASTE:
        case SID_DELETE:
            mrController.GetClipboard().HandleSlotCall(rRequest);
            break;

        case SID_DRAWINGMODE:
        case SID_NOTESMODE:
        case SID_HANDOUTMODE:
        case SID_DIAMODE:
        case SID_OUTLINEMODE:
            rShell.GetViewShellBase().GetPaneManager().HandleModeChangeSlot (
                rRequest.GetSlot(),
                rRequest);
            rRequest.Done();
            break;

        case SID_ZOOM_PREV:
        case SID_ZOOM_NEXT:
            HandleSizeSlots (rRequest);
            break;

        case SID_UNDO :
            rShell.ImpSidUndo (FALSE, rRequest);
            break;

        case SID_REDO :
            rShell.ImpSidRedo (FALSE, rRequest);
            break;

        default:
            break;
    }
}




void SlotManager::ExecCtrl (SfxRequest& rRequest)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());
    USHORT nSlot = rRequest.GetSlot();
    switch (nSlot)
    {
        case SID_RELOAD:
        {
            // Undo-Manager leeren
            mrController.GetModel().GetDocument()->GetDocSh()->GetUndoManager()->Clear();

            // Normale Weiterleitung an ViewFrame zur Ausfuehrung
            rShell.GetViewFrame()->ExecuteSlot(rRequest);

            // Muss sofort beendet werden
            return;
        }
        break;

        case SID_OUTPUT_QUALITY_COLOR:
        case SID_OUTPUT_QUALITY_GRAYSCALE:
        case SID_OUTPUT_QUALITY_BLACKWHITE:
        case SID_OUTPUT_QUALITY_CONTRAST:
        {
            // flush page cache
            //AF            mrController.GetView().UpdateAllPages();
            rShell.ExecReq (rRequest);
            break;
        }

        case SID_MAIL_SCROLLBODY_PAGEDOWN:
        {
            rShell.ExecReq (rRequest);
            break;
        }

        case SID_OPT_LOCALE_CHANGED:
        {
            mrController.UpdateAllPages();
            mrController.GetViewShell().UpdatePreview (
                mrController.GetViewShell().GetActualPage());
            rRequest.Done();
            break;
        }

        case SID_SEARCH_DLG:
            // We have to handle the SID_SEARCH_DLG slot explicitly because
            // in some cases (when the slide sorter is displayed in the
            // center pane) we want to disable the search dialog.  Therefore
            // we have to handle the execution of that slot as well.
            // We try to do that by forwarding the request to the view frame
            // of the view shell.
            mrController.GetViewShell().GetViewFrame()->ExecuteSlot (
                rRequest);
            break;

        default:
            break;
    }
}




void SlotManager::GetAttrState (SfxItemSet& rSet)
{
    // Iteratate over all items.
    SfxWhichIter aIter (rSet);
    USHORT nWhich = aIter.FirstWhich();
    while (nWhich)
    {
        USHORT nSlotId = SfxItemPool::IsWhich(nWhich)
            ? mrController.GetViewShell().GetPool().GetSlotId(nWhich)
            : nWhich;
        switch (nSlotId)
        {
            case SID_PAGES_PER_ROW:
                rSet.Put (
                    SfxUInt16Item (
                        nSlotId,
                        (USHORT)mrController.GetView().GetLayouter().GetColumnCount()
                        )
                    );
            break;
        }
        nWhich = aIter.NextWhich();
    }
}




void SlotManager::GetCtrlState (SfxItemSet& rSet)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());
    if (rSet.GetItemState(SID_RELOAD) != SFX_ITEM_UNKNOWN)
    {
        // "Letzte Version" vom SFx en/disablen lassen
        SfxViewFrame* pSlideViewFrame = rShell.GetViewFrame();
        DBG_ASSERT(pSlideViewFrame!=NULL,
            "SlideSorterController::GetCtrlState: ViewFrame not found");
        if (pSlideViewFrame->ISA(SfxTopViewFrame))
        {
            pSlideViewFrame->GetSlotState (SID_RELOAD, NULL, &rSet);
        }
        else        // MI sagt: kein MDIFrame --> disablen
        {
            rSet.DisableItem(SID_RELOAD);
        }
    }

    // Output quality.
    if (rSet.GetItemState(SID_OUTPUT_QUALITY_COLOR)==SFX_ITEM_AVAILABLE
        ||rSet.GetItemState(SID_OUTPUT_QUALITY_GRAYSCALE)==SFX_ITEM_AVAILABLE
        ||rSet.GetItemState(SID_OUTPUT_QUALITY_BLACKWHITE)==SFX_ITEM_AVAILABLE
        ||rSet.GetItemState(SID_OUTPUT_QUALITY_CONTRAST)==SFX_ITEM_AVAILABLE)
    {
        ULONG nMode = mrController.GetView().GetWindow()->GetDrawMode();
        //rShell.GetWindow()->GetDrawMode();
        UINT16 nQuality = 0;

        switch (nMode)
        {
            case ::sd::ViewShell::OUTPUT_DRAWMODE_COLOR:
                nQuality = 0;
                break;
            case ::sd::ViewShell::OUTPUT_DRAWMODE_GRAYSCALE:
                nQuality = 1;
                break;
            case ::sd::ViewShell::OUTPUT_DRAWMODE_BLACKWHITE:
                nQuality = 2;
                break;
            case ::sd::ViewShell::OUTPUT_DRAWMODE_CONTRAST:
                nQuality = 3;
                break;
        }

        rSet.Put (SfxBoolItem (SID_OUTPUT_QUALITY_COLOR,
                (BOOL)(nQuality==0)));
        rSet.Put (SfxBoolItem (SID_OUTPUT_QUALITY_GRAYSCALE,
                (BOOL)(nQuality==1)));
        rSet.Put (SfxBoolItem (SID_OUTPUT_QUALITY_BLACKWHITE,
                (BOOL)(nQuality==2)));
        rSet.Put (SfxBoolItem (SID_OUTPUT_QUALITY_CONTRAST,
                (BOOL)(nQuality==3)));
    }

    if (rSet.GetItemState(SID_MAIL_SCROLLBODY_PAGEDOWN) == SFX_ITEM_AVAILABLE)
    {
        rSet.Put (SfxBoolItem( SID_MAIL_SCROLLBODY_PAGEDOWN, TRUE));
    }
}




void SlotManager::GetMenuState ( SfxItemSet& rSet)
{
    EditMode eEditMode = mrController.GetModel().GetEditMode();
    SlideSorterViewShell& rShell (mrController.GetViewShell());
    DrawDocShell* pDocShell
        = mrController.GetModel().GetDocument()->GetDocSh();

    if (rShell.GetActualFunction())
    {
        USHORT nSId = rShell.GetActualFunction()->GetSlotID();

        rSet.Put( SfxBoolItem( nSId, TRUE ) );
    }
    rSet.Put( SfxBoolItem( SID_DRAWINGMODE, FALSE ) );
    rSet.Put( SfxBoolItem( SID_DIAMODE, TRUE ) );
    rSet.Put( SfxBoolItem( SID_OUTLINEMODE, FALSE ) );
    rSet.Put( SfxBoolItem( SID_NOTESMODE, FALSE ) );
    rSet.Put( SfxBoolItem( SID_HANDOUTMODE, FALSE ) );

    // Vorlagenkatalog darf nicht aufgerufen werden
    rSet.DisableItem(SID_STYLE_CATALOG);
    //  rSet.DisableItem(SID_SIZE_ALL);
    if (rShell.IsMainViewShell())
    {
        rSet.DisableItem(SID_SPELL_DIALOG);
        rSet.DisableItem(SID_SEARCH_DLG);
    }

    if ( ! rShell.GetZoomList()->IsNextPossible())
    {
        rSet.DisableItem(SID_ZOOM_NEXT);
    }
    if ( ! rShell.GetZoomList()->IsPreviousPossible())
    {
        rSet.DisableItem(SID_ZOOM_PREV);
    }

    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ZOOM_IN ) ||
        SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ZOOM_OUT ) ||
        SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_SIZE_REAL ) )
    {
        BOOL bUIActive = pDocShell->IsUIActive();

        ::sd::Window* pWindow = rShell.GetActiveWindow();
        if( pWindow->GetZoom() <= pWindow->GetMinZoom() || bUIActive )
            rSet.DisableItem( SID_ZOOM_IN );
        if( pWindow->GetZoom() >= pWindow->GetMaxZoom() || bUIActive )
            rSet.DisableItem( SID_ZOOM_OUT );
        if( 100 >= pWindow->GetMaxZoom() || bUIActive )
            rSet.DisableItem( SID_SIZE_REAL );
    }

    if (SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_EXPAND_PAGE))
    {
        bool bDisable = true;
        if (eEditMode == EM_PAGE)
        {
            // At least one of the selected pages has to contain an outline
            // presentation objects in order to enable the expand page menu
            // entry.
            model::SlideSorterModel::Enumeration aSelectedPages (
                mrController.GetModel().GetSelectedPagesEnumeration());
            while (aSelectedPages.HasMoreElements())
            {
                SdPage* pPage = aSelectedPages.GetNextElement().GetPage();
                SdrObject* pObj = pPage->GetPresObj(PRESOBJ_OUTLINE);
                if (pObj!=NULL && !pObj->IsEmptyPresObj())
                    bDisable = false;
            }
        }

        if (bDisable)
            rSet.DisableItem (SID_EXPAND_PAGE);
    }

    if (SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_SUMMARY_PAGE))
    {
        bool bDisable = true;
        if (eEditMode == EM_PAGE)
        {
            // At least one of the selected pages has to contain a title
            // presentation objects in order to enable the summary page menu
            // entry.
            model::SlideSorterModel::Enumeration aSelectedPages (
                mrController.GetModel().GetSelectedPagesEnumeration());
            while (aSelectedPages.HasMoreElements())
            {
                SdPage* pPage = aSelectedPages.GetNextElement().GetPage();
                SdrObject* pObj = pPage->GetPresObj(PRESOBJ_TITLE);

                if (pObj!=NULL && !pObj->IsEmptyPresObj())
                    bDisable = false;
            }
        }
        if (bDisable)
            rSet.DisableItem (SID_SUMMARY_PAGE);
    }

    // Starten der Praesentation moeglich?
    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_PRESENTATION ) ||
        SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_REHEARSE_TIMINGS ) )
    {
        BOOL bDisable = TRUE;
        model::SlideSorterModel::Enumeration aAllPages (
            mrController.GetModel().GetAllPagesEnumeration());
        while (aAllPages.HasMoreElements())
        {
            SdPage* pPage = aAllPages.GetNextElement().GetPage();

            if( !pPage->IsExcluded() )
                bDisable = FALSE;
        }
        if( bDisable || pDocShell->IsPreview())
        {
            rSet.DisableItem( SID_PRESENTATION );
            rSet.DisableItem( SID_REHEARSE_TIMINGS );
        }
    }

    SdTransferable* pTransferClip = SD_MOD()->pTransferClip;

    // Keine eigenen Clipboard-Daten?
    if ( !pTransferClip || !pTransferClip->GetDocShell() )
    {
        rSet.DisableItem(SID_PASTE);
    }
    else
    {
        SfxObjectShell* pShell = pTransferClip->GetDocShell();

        if( !pShell || ( (DrawDocShell*) pShell)->GetDoc()->GetPageCount() <= 1 )
        {
            // Eigene Clipboard-Daten haben nur eine Seite
            rSet.DisableItem(SID_PASTE);
        }
    }

    // Copy and paste of master pages is not yet implemented properly
    if (rSet.GetItemState(SID_COPY) == SFX_ITEM_AVAILABLE
        || rSet.GetItemState(SID_PASTE)  == SFX_ITEM_AVAILABLE
        || rSet.GetItemState(SID_PASTE2)  == SFX_ITEM_AVAILABLE)
    {
        if (mrController.GetModel().GetEditMode() == EM_MASTERPAGE)
        {
            if (rSet.GetItemState(SID_COPY) == SFX_ITEM_AVAILABLE)
                rSet.DisableItem(SID_COPY);
            if (rSet.GetItemState(SID_PASTE) == SFX_ITEM_AVAILABLE)
                rSet.DisableItem(SID_PASTE);
            if (rSet.GetItemState(SID_PASTE2) == SFX_ITEM_AVAILABLE)
                rSet.DisableItem(SID_PASTE2);
        }
    }

    // Cut, copy, and delete page are disabled when there is no selection.
    if (rSet.GetItemState(SID_CUT) == SFX_ITEM_AVAILABLE
        || rSet.GetItemState(SID_COPY)  == SFX_ITEM_AVAILABLE
        || rSet.GetItemState(SID_DELETE) == SFX_ITEM_AVAILABLE
        || rSet.GetItemState(SID_DELETE_PAGE) == SFX_ITEM_AVAILABLE
        || rSet.GetItemState(SID_DELETE_MASTER_PAGE) == SFX_ITEM_AVAILABLE)
    {
        model::SlideSorterModel::Enumeration aSelectedPages (
            mrController.GetModel().GetSelectedPagesEnumeration());

        // For copy to work we have to have at least one selected page.
        if ( ! aSelectedPages.HasMoreElements())
            rSet.DisableItem(SID_COPY);

        bool bDisable = false;
        // The operations that lead to the deletion of a page are valid if
        // a) there is at least one selected page
        // b) deleting the selected pages leaves at least one page in the
        // document
        // c) selected master pages must not be used by slides.

        // Test a).
        if ( ! aSelectedPages.HasMoreElements())
            bDisable = true;
        // Test b): Count the number of selected pages.  It has to be less
        // than the number of all pages.
        else if (mrController.GetPageSelector().GetSelectedPageCount()
            >= mrController.GetPageSelector().GetPageCount())
            bDisable = true;
        // Test c): Iterate over the selected pages and look for a master
        // page that is used by at least one page.
        else while (aSelectedPages.HasMoreElements())
        {
            SdPage* pPage = aSelectedPages.GetNextElement().GetPage();
            int nUseCount (mrController.GetModel().GetDocument()
                ->GetMasterPageUserCount(pPage));
            if (nUseCount > 0)
            {
                bDisable = true;
                break;
            }
        }

        if (bDisable)
        {
            rSet.DisableItem(SID_CUT);
            rSet.DisableItem(SID_DELETE_PAGE);
            rSet.DisableItem(SID_DELETE_MASTER_PAGE);
        }
    }

    // Disable the rename slots when there are no or more than one slides/master
    // pages selected.
    if (rSet.GetItemState(SID_RENAMEPAGE) == SFX_ITEM_AVAILABLE
        || rSet.GetItemState(SID_RENAME_MASTER_PAGE)  == SFX_ITEM_AVAILABLE)
    {
        if (mrController.GetPageSelector().GetSelectedPageCount() != 1)
        {
            rSet.DisableItem(SID_RENAMEPAGE);
            rSet.DisableItem(SID_RENAME_MASTER_PAGE);
        }
    }
}




void SlotManager::GetStatusBarState (SfxItemSet& rSet)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());

    // Zoom-Item
    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ATTR_ZOOM ) )
    {
        SvxZoomItem* pZoomItem;
        UINT16 nZoom = (UINT16) rShell.GetActiveWindow()->GetZoom();

        pZoomItem = new SvxZoomItem( SVX_ZOOM_PERCENT, nZoom );

        // Bereich einschraenken
        USHORT nZoomValues = SVX_ZOOM_ENABLE_ALL;
        nZoomValues &= ~SVX_ZOOM_ENABLE_OPTIMAL;
        nZoomValues &= ~SVX_ZOOM_ENABLE_PAGEWIDTH;
        nZoomValues &= ~(SVX_ZOOM_ENABLE_100|SVX_ZOOM_ENABLE_150|SVX_ZOOM_ENABLE_200);

        pZoomItem->SetValueSet( nZoomValues );
        rSet.Put( *pZoomItem );
        delete pZoomItem;
    }

    // Seitenanzeige und Layout
    /*
    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_STATUS_PAGE ) ||
        SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_STATUS_LAYOUT ) )
    */
    SdPage* pPage      = NULL;
    SdPage* pFirstPage = NULL;
    USHORT  nFirstPage;
    USHORT  nSelectedPages = mrController.GetPageSelector().GetSelectedPageCount();
    String aPageStr;
    String aLayoutStr;

    if (nSelectedPages > 0)
        aPageStr = String(SdResId(STR_SD_PAGE));

    if (nSelectedPages == 1)
    {
        model::SlideSorterModel::Enumeration aSelectedPages (
            mrController.GetModel().GetSelectedPagesEnumeration());
        pPage = aSelectedPages.GetNextElement().GetPage();
        nFirstPage = pPage->GetPageNum()/2;
        pFirstPage = pPage;

        aPageStr += sal_Unicode(' ');
        aPageStr += String::CreateFromInt32( nFirstPage + 1 );
        aPageStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " / " ));
        aPageStr += String::CreateFromInt32(
            mrController.GetModel().GetPageCount());

        aLayoutStr = pFirstPage->GetLayoutName();
        aLayoutStr.Erase( aLayoutStr.SearchAscii( SD_LT_SEPARATOR ) );
    }

    rSet.Put( SfxStringItem( SID_STATUS_PAGE, aPageStr ) );
    rSet.Put( SfxStringItem( SID_STATUS_LAYOUT, aLayoutStr ) );
}




void SlotManager::HandleSizeSlots (SfxRequest& rRequest)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());
    bool bRequestRepaint = true;
    ::sd::Window* pWindow = mrController.GetView().GetWindow();

    switch (rRequest.GetSlot())
    {
        case SID_ATTR_ZOOM:
        {
            const SfxItemSet* pArgs = rRequest.GetArgs();

            if (pArgs != NULL)
            {
                SvxZoomType aZoomType = static_cast<const SvxZoomItem&>(
                    pArgs->Get(SID_ATTR_ZOOM)).GetType();
                switch (aZoomType)
                {
                    case SVX_ZOOM_PERCENT:
                        rShell.SetZoom (
                            (long)static_cast<const SvxZoomItem&> (
                                pArgs->Get(SID_ATTR_ZOOM)).GetValue());
                        break;

                    case SVX_ZOOM_WHOLEPAGE:
                        rShell.GetViewFrame()->GetDispatcher()->Execute(
                            SID_SIZE_PAGE,
                            SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
                        break;
                }
                rShell.Invalidate (SID_ATTR_ZOOM);
                rRequest.Done();
            }
            else
            {
                rShell.SetCurrentFunction (
                    new FuScale (
                        &rShell,
                        pWindow,
                        &mrController.GetView(),
                        mrController.GetModel().GetDocument(),
                        rRequest));
            }
            rShell.Cancel();
        }
        break;

        case SID_ZOOM_OUT:
            rShell.SetCurrentFunction (
                new FuZoom (
                    &rShell,
                    pWindow,
                    &mrController.GetView(),
                    mrController.GetModel().GetDocument(),
                    rRequest));
            rRequest.Done();
            break;

        case SID_ZOOM_IN:
            rShell.SetZoom( Max(
                (long) ( pWindow->GetZoom() / 2 ),
                (long) pWindow->GetMinZoom() ) );
            rShell.GetZoomList()->InsertZoomRect(
                pWindow->PixelToLogic(
                    Rectangle(
                        Point(0,0),
                        pWindow->GetOutputSizePixel())));
            rShell.Invalidate (SID_ATTR_ZOOM);
            rShell.Invalidate (SID_ZOOM_OUT);
            rShell.Invalidate (SID_ZOOM_IN);
            rShell.Invalidate (SID_SIZE_REAL);
            rShell.Cancel();
            rRequest.Done();
            break;

        case SID_SIZE_REAL:
            rShell.SetZoom (100);
            rShell.GetZoomList()->InsertZoomRect (
                pWindow->PixelToLogic(
                    Rectangle(
                        Point(0,0),
                        pWindow->GetOutputSizePixel())));
            rShell.Invalidate (SID_ATTR_ZOOM);
            rShell.Invalidate (SID_ZOOM_OUT);
            rShell.Invalidate (SID_ZOOM_IN);
            rShell.Invalidate (SID_SIZE_REAL);

            rShell.Cancel();
            rRequest.Done();
            break;

        case SID_SIZE_ALL:
        case SID_SIZE_PAGE:
        {
            // The zoom is set so that
            // a) for SID_SIZE_PAGE the first page is fully visible and as
            // large as possible,
            // b) for SID_SIZE_ALL and non-empty selection that all selected
            // pages are visible,
            // c) for SID_SIZE_ALL and empty selection that all pages are
            // visible.
            Rectangle aZoomRect;
            int nPageCount = mrController.GetModel().GetPageCount();

            if (nPageCount > 0)
            {
                // a) Use the bounding box of the first page.
                if (rRequest.GetSlot() == SID_SIZE_PAGE)
                    aZoomRect = mrController.GetView().GetPageBoundingBox(
                        0,
                        view::SlideSorterView::CS_MODEL,
                        view::SlideSorterView::BBT_INFO);
                else
                {
                    // b) and c) use the enumeration of the selected pages,
                    // or if that is empty, the enumeration of all pages to
                    // create the union of their bounding boxes.
                    model::SlideSorterModel::Enumeration aPages (
                        mrController.GetModel().GetSelectedPagesEnumeration());
                    if ( ! aPages.HasMoreElements())
                        aPages =
                            mrController.GetModel().GetAllPagesEnumeration();
                    while (aPages.HasMoreElements())
                    {
                        aZoomRect.Union (
                            mrController.GetView().GetPageBoundingBox(
                                aPages.GetNextElement(),
                                view::SlideSorterView::CS_MODEL,
                                view::SlideSorterView::BBT_INFO));
                    }
                }
            }

            // Add some extra space at the borders and set the new zoom
            // rectangle.
            if ( ! aZoomRect.IsEmpty())
            {
                Point aPagePosition (aZoomRect.Center());
                Size aPageSize (aZoomRect.GetSize());
                aPageSize.Width()  = aPageSize.Width()  * 11 / 10;
                aPageSize.Height() = aPageSize.Height() * 11 / 10;
                aPagePosition.X() -= aPageSize.Width() / 2;
                aPagePosition.Y() -= aPageSize.Height() / 2;

                rShell.SetZoomRect (Rectangle(aPagePosition, aPageSize));

                Rectangle aVisibleArea = pWindow->PixelToLogic(
                    Rectangle(
                        Point(0,0),
                        pWindow->GetOutputSizePixel()));
                rShell.GetZoomList()->InsertZoomRect (aVisibleArea);
            }

            rShell.Invalidate (SID_ATTR_ZOOM);
            rShell.Cancel();
            rRequest.Done();
        }
        break;

        case SID_ZOOM_PREV:
            if (rShell.GetZoomList()->IsPreviousPossible())
            {
                // Go to previous zoom rectangle.
                rShell.SetZoomRect (
                    rShell.GetZoomList()->GetPreviousZoomRect());
            }
            rRequest.Done ();
            break;

        case SID_ZOOM_NEXT:
            if (rShell.GetZoomList()->IsNextPossible())
            {
                // Go to next zoom rectangle.
                rShell.SetZoomRect (
                    rShell.GetZoomList()->GetNextZoomRect());
            }
            rRequest.Done ();
            break;

        default:
            bRequestRepaint = false;
    }
    if (bRequestRepaint)
        mrController.GetView().RequestRepaint ();
}




void SlotManager::ShowSlideShow( SfxRequest& rRequest)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());

    if ( ! rShell.IsMainViewShell())
    {
        // We are not the main sub shell, so delegate this call to the main
        // sub shell.  Because there is no SFX functionality to forward a
        // slot call further down the stack we have to do that here
        // manually.
        ViewShell* pMainViewShell
            = rShell.GetViewShellBase().GetPaneManager().GetViewShell(
                PaneManager::PT_CENTER);
        if (pMainViewShell->ISA(DrawViewShell))
            PTR_CAST(DrawViewShell, pMainViewShell)->FuSupport (
                rRequest);
    }
    else
    {
        SFX_REQUEST_ARG(rRequest,
            pFullScreen,
            SfxBoolItem,
            ATTR_PRESENT_FULLSCREEN,
            FALSE);
        bool bFullScreen;
        if (rRequest.GetSlot()==SID_REHEARSE_TIMINGS && pFullScreen!=NULL)
            bFullScreen = pFullScreen->GetValue();
        else
            bFullScreen
                = mrController.GetModel().GetDocument()->getPresentationSettings().mbFullScreen;
        if (bFullScreen)
        {
            PresentationViewShell::CreateFullScreenShow(&rShell, rRequest);
            rShell.Cancel();
            rRequest.Done();
        }
        else
        {
            // Temporarily replace this sub shell by one that is able to
            // display the slide show.
            FrameView* pFrameView = rShell.GetFrameView();
            pFrameView->SetPresentationViewShellId(SID_VIEWSHELL1);
            pFrameView->SetPreviousViewShellType (rShell.GetShellType());
            pFrameView->SetSlotId (rRequest.GetSlot());
            pFrameView->SetPageKind (PK_STANDARD);
            rShell.GetViewShellBase().GetPaneManager().RequestMainViewShellChange(
                ViewShell::ST_IMPRESS,
                PaneManager::CM_ASYNCHRONOUS);
            rShell.GetViewFrame()->GetDispatcher()->Execute(rRequest.GetSlot(),
                SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
            rRequest.Done();
        }
    }

}




void SlotManager::RenameSlide (void)
{
    PageKind ePageKind = mrController.GetModel().GetPageType();
    View* pDrView = &mrController.GetView();

    if (ePageKind==PK_STANDARD || ePageKind==PK_NOTES)
    {
        if ( pDrView->IsTextEdit() )
        {
            pDrView->EndTextEdit();
        }

        SdPage* pSelectedPage = NULL;
        model::PageEnumeration aSelectedPages (
            mrController.GetModel().GetSelectedPagesEnumeration());
        if (aSelectedPages.HasMoreElements())
            pSelectedPage = aSelectedPages.GetNextElement().GetPage();
        if (pSelectedPage != NULL)
        {
            String aTitle( SdResId( STR_TITLE_RENAMESLIDE ) );
            String aDescr( SdResId( STR_DESC_RENAMESLIDE ) );
            String aPageName = pSelectedPage->GetName();

            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "Dialogdiet fail!");
            AbstractSvxNameDialog* aNameDlg = pFact->CreateSvxNameDialog(
                mrController.GetViewShell().GetActiveWindow(),
                aPageName, aDescr, ResId(RID_SVXDLG_NAME) );
            DBG_ASSERT(aNameDlg, "Dialogdiet fail!");
            aNameDlg->SetText( aTitle );
            aNameDlg->SetCheckNameHdl( LINK( this, SlotManager, RenameSlideHdl ), true );
            aNameDlg->SetEditHelpId( HID_SD_NAMEDIALOG_PAGE );

            if( aNameDlg->Execute() == RET_OK )
            {
                String aNewName;
                aNameDlg->GetName( aNewName );
                if( ! aNewName.Equals( aPageName ) )
                {
                    bool bResult = RenameSlideFromDrawViewShell(
                        pSelectedPage->GetPageNum()/2, aNewName );
                    DBG_ASSERT( bResult, "Couldn't rename slide" );
                }
            }
            delete aNameDlg;
        }
    }
}

IMPL_LINK(SlotManager, RenameSlideHdl, AbstractSvxNameDialog*, pDialog)
{
    if( ! pDialog )
        return 0;

    String aNewName;
    pDialog->GetName( aNewName );

    SdPage* pCurrentPage = mrController.GetActualPage();

    return ( aNewName.Equals( pCurrentPage->GetName() )
        || mrController.GetViewShell().GetDocSh()->IsNewPageNameValid( aNewName ) );
}

bool SlotManager::RenameSlideFromDrawViewShell( USHORT nPageId, const String & rName  )
{
    BOOL   bOutDummy;
    SdDrawDocument* pDocument = mrController.GetModel().GetDocument();
    if( pDocument->GetPageByName( rName, bOutDummy ) != SDRPAGE_NOTFOUND )
        return false;

    SdPage* pPageToRename = NULL;
    PageKind ePageKind = mrController.GetModel().GetPageType();

    if( mrController.GetModel().GetEditMode() == EM_PAGE )
    {
        pPageToRename = mrController.GetActualPage();

        // Undo
        SdPage* pUndoPage = pPageToRename;
        SdrLayerAdmin &  rLayerAdmin = pDocument->GetLayerAdmin();
        BYTE nBackground = rLayerAdmin.GetLayerID( String( SdResId( STR_LAYER_BCKGRND )), FALSE );
        BYTE nBgObj = rLayerAdmin.GetLayerID( String( SdResId( STR_LAYER_BCKGRNDOBJ )), FALSE );
        SetOfByte aVisibleLayers = mrController.GetActualPage()->TRG_GetMasterPageVisibleLayers();

        // (#67720#)
        SfxUndoManager* pManager = pDocument->GetDocSh()->GetUndoManager();
        ModifyPageUndoAction* pAction = new ModifyPageUndoAction(
            pManager, pDocument, pUndoPage, rName, pUndoPage->GetAutoLayout(),
            aVisibleLayers.IsSet( nBackground ),
            aVisibleLayers.IsSet( nBgObj ));
        pManager->AddUndoAction( pAction );

        // rename
        pPageToRename->SetName( rName );

        if( ePageKind == PK_STANDARD )
        {
            // also rename notes-page
            SdPage* pNotesPage = pDocument->GetSdPage( nPageId, PK_NOTES );
            if (pNotesPage != NULL)
                pNotesPage->SetName (rName);
        }
    }
    else
    {
        // rename MasterPage -> rename LayoutTemplate
        pPageToRename = pDocument->GetMasterSdPage( nPageId, ePageKind );
        if (pPageToRename != NULL)
            pDocument->RenameLayoutTemplate(
                pPageToRename->GetLayoutName(), rName );
    }

    bool bSuccess = ( FALSE != rName.Equals( pPageToRename->GetName()));

    if( bSuccess )
    {
        // user edited page names may be changed by the page so update control
        //        aTabControl.SetPageText( nPageId, rName );

        // set document to modified state
        pDocument->SetChanged( TRUE );

        // inform navigator about change
        SfxBoolItem aItem( SID_NAVIGATOR_INIT, TRUE );
        mrController.GetViewShell().GetDispatcher()->Execute(
            SID_NAVIGATOR_INIT, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );
    }

    return bSuccess;
}




/** Insert a slide.  The insertion position depends on a) the selection and
    b) the mouse position when there is no selection.

    When there is a selection then insertion takes place after the last
    slide of the selection.  For this to work all but the last selected
    slide are deselected first.

    Otherwise, when there is no selection but the insertion marker is visible
    the slide is inserted at that position.  The slide before that marker is
    selected first.

    When both the selection and the insertion marker are not visible--can
    that happen?--the new slide is inserted after the last slide.
*/
void SlotManager::InsertSlide (SfxRequest& rRequest)
{
    PageSelector& rSelector (mrController.GetPageSelector());
    // The fallback insertion position is after the last slide.
    sal_Int32 nInsertionIndex (rSelector.GetPageCount() - 1);
    if (rSelector.GetSelectedPageCount() > 0)
    {
        // Deselect all but the last selected slide.
        bool bLastSelectedSlideSeen (false);
        for (int nIndex=rSelector.GetPageCount()-1; nIndex>=0; --nIndex)
        {
            if (rSelector.IsPageSelected(nIndex))
                if (bLastSelectedSlideSeen)
                    rSelector.DeselectPage (nIndex);
                else
                {
                    nInsertionIndex = nIndex;
                    bLastSelectedSlideSeen = true;
                }
        }
    }

    // No selection.  Is there an insertion indicator?
    else if (mrController.GetView().GetOverlay()
        .GetInsertionIndicatorOverlay().IsShowing())
    {
        // Select the page before the insertion indicator.
        nInsertionIndex = mrController.GetView().GetOverlay()
            .GetInsertionIndicatorOverlay().GetInsertionPageIndex();
        nInsertionIndex --;
        rSelector.SelectPage (nInsertionIndex);
    }

    // Select the last page when there is at least one page.
    else if (rSelector.GetPageCount() > 0)
    {
        nInsertionIndex = rSelector.GetPageCount() - 1;
        rSelector.SelectPage (nInsertionIndex);
    }

    // Hope for the best that CreateOrDuplicatePage() can cope with an empty
    // selection.
    else
    {
        nInsertionIndex = -1;
    }

    USHORT nPageCount (mrController.GetModel().GetPageCount());

    rSelector.DisableBroadcasting();
    // In order for SlideSorterController::GetActualPage() to select the
    // selected page as current page we have to turn off the focus
    // temporarily.
    {
        FocusManager::FocusHider aTemporaryFocusHider (
            mrController.GetFocusManager());

        SdPage* pPreviousPage = NULL;
        if (nInsertionIndex >= 0)
            pPreviousPage = mrController.GetModel()
                .GetPageDescriptor(nInsertionIndex)->GetPage();

        if (mrController.GetModel().GetEditMode() == EM_PAGE)
            mrController.GetViewShell().CreateOrDuplicatePage (
                rRequest,
                mrController.GetModel().GetPageType(),
                pPreviousPage);
        else
        {
            // Use the API to create a new page.
            SdDrawDocument* pDocument = mrController.GetModel().GetDocument();
            Reference<drawing::XMasterPagesSupplier> xMasterPagesSupplier (
                pDocument->getUnoModel(), UNO_QUERY);
            if (xMasterPagesSupplier.is())
            {
                Reference<drawing::XDrawPages> xMasterPages (
                    xMasterPagesSupplier->getMasterPages());
                if (xMasterPages.is())
                {
                    xMasterPages->insertNewByIndex (nInsertionIndex+1);

                    // Create shapes for the default layout.
                    SdPage* pMasterPage = pDocument->GetMasterSdPage(
                        (USHORT)(nInsertionIndex+1), PK_STANDARD);
                    pMasterPage->CreateTitleAndLayout (TRUE,TRUE);
                }
            }
        }
    }

    // When a new page has been inserted then select it and make it the
    // current page.
    mrController.GetView().LockRedraw(TRUE);
    if (mrController.GetModel().GetPageCount() > nPageCount)
    {
        nInsertionIndex++;
        rSelector.DeselectAllPages ();
        rSelector.SelectPage (nInsertionIndex);
        rSelector.SetCurrentPage (nInsertionIndex);
    }
    rSelector.EnableBroadcasting();
    mrController.GetView().LockRedraw(FALSE);
}




void SlotManager::AssignTransitionEffect (void)
{
    SlideSorterViewShell& rShell (mrController.GetViewShell());
    model::SlideSorterModel& rModel (mrController.GetModel());

    // We have to manually select the pages in the document that are
    // selected in the slide sorter.
    rModel.SynchronizeDocumentSelection();

    // #i34011#: Needs review, AF's bugfix is removed here
    //rShell.AssignFromSlideChangeWindow(rModel.GetEditMode());

    // We have to remove the selection of master pages to not confuse the
    // model.
    if (rModel.GetEditMode() == EM_MASTERPAGE)
    {
        SdDrawDocument* pDocument = mrController.GetModel().GetDocument();
        USHORT nMasterPageCount = pDocument->GetMasterSdPageCount(PK_STANDARD);
        for (USHORT nIndex=0; nIndex<nMasterPageCount; nIndex++)
        {
            SdPage* pPage = pDocument->GetMasterSdPage(nIndex, PK_STANDARD);
            if (pPage != NULL)
                pPage->SetSelected (FALSE);
        }
    }
}




void SlotManager::ExecuteCommandAsynchronously (::std::auto_ptr<Command> pCommand)
{
    // Ownership of command is (implicitely) transferred to the queue.
    maCommandQueue.push(pCommand.get());
    pCommand.release();
    ULONG nUserEventId = Application::PostUserEvent(LINK(this,SlotManager,UserEventCallback));
}




IMPL_LINK(SlotManager, UserEventCallback, void*, pFoo)
{
    if ( ! maCommandQueue.empty())
    {
        Command* pCommand = maCommandQueue.front();
        maCommandQueue.pop();

        if (pCommand != NULL)
        {
            // The queue ownes the command that has just been removed from
            // it.  Therefore it is deleted after it has been executed.
            (*pCommand)();
            delete pCommand;
        }
    }

    return 1;
}


} } } // end of namespace ::sd::slidesorter::controller
