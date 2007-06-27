/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsClipboard.cxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 15:45:20 $
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
#include "precompiled_sd.hxx"

#include "controller/SlsClipboard.hxx"

#include "SlideSorterViewShell.hxx"
#include "model/SlideSorterModel.hxx"
#include "model/SlsPageDescriptor.hxx"
#include "model/SlsPageEnumeration.hxx"
#include "view/SlideSorterView.hxx"
#include "view/SlsViewOverlay.hxx"
#include "view/SlsPageObject.hxx"
#include "controller/SlideSorterController.hxx"
#include "controller/SlsPageSelector.hxx"
#include "controller/SlsSelectionFunction.hxx"
#include "SlsTransferable.hxx"
#include "controller/SlsScrollBarManager.hxx"
#include "controller/SlsFocusManager.hxx"

#include "ViewShellBase.hxx"
#include "DrawViewShell.hxx"
#include "Window.hxx"
#include "fupoor.hxx"
#include "fuslhide.hxx"
#include "fuzoom.hxx"
#include "fucushow.hxx"
#include "fusldlg.hxx"
#include "fuexpand.hxx"
#include "fusumry.hxx"
#include "app.hrc"
#include "glob.hrc"
#include "strings.hrc"
#include "sdresid.hxx"
#include "sdxfer.hxx"
#include "sdmod.hxx"
#include "sddll.hxx"
#include "ins_paste.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "sdpage.hxx"

#include <com/sun/star/datatransfer/dnd/DNDConstants.hpp>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/docfile.hxx>
#include <svx/svxids.hrc>
#include <vcl/msgbox.hxx>
#include <tools/urlobj.hxx>
#include <rtl/ustring.hxx>

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

namespace sd { namespace slidesorter { namespace controller {


Clipboard::Clipboard (SlideSorterController& rController)
    : ViewClipboard (rController.GetView()),
      mrController (rController),
      maPagesToRemove(),
      maPagesToSelect(),
      mbUpdateSelectionPending(false)
{
}




Clipboard::~Clipboard (void)
{
}




/** With the current implementation the forwarded calls to the current
    function will come back eventually to call the local Do(Cut|Copy|Paste)
    methods.  A shortcut is possible but would be an unclean hack.
*/
void Clipboard::HandleSlotCall (SfxRequest& rRequest)
{
    FunctionReference xFunc( mrController.GetViewShell().GetCurrentFunction() );
    switch (rRequest.GetSlot())
    {
        case SID_CUT:
            if (mrController.GetModel().GetEditMode() != EM_MASTERPAGE)
            {
                if(xFunc.is())
                    xFunc->DoCut();
                else
                    DoCut();
            }
            rRequest.Done();
            break;

        case SID_COPY:
            if (mrController.GetModel().GetEditMode() != EM_MASTERPAGE)
            {
                if(xFunc.is())
                    xFunc->DoCopy();
                else
                    DoCopy();
            }
            rRequest.Done();
            break;

        case SID_PASTE:
            // Prevent redraws while inserting pages from the clipboard
            // because the intermediate inconsistent state might lead to
            // a crash.
            if (mrController.GetModel().GetEditMode() != EM_MASTERPAGE)
            {
                mrController.GetView().LockRedraw (TRUE);
                if(xFunc.is())
                    xFunc->DoPaste();
                else
                    DoPaste();
                mrController.MakeSelectionVisible();
                mrController.GetView().LockRedraw (FALSE);
            }
            rRequest.Done();
            break;

        case SID_DELETE:
            DoDelete();
            rRequest.Done();
            break;
    }
}




void Clipboard::DoCut (::Window* pWindow)
{
    if (mrController.GetModel().GetPageCount() > 1)
    {
        DoCopy(pWindow);
        DoDelete(pWindow);
    }
}




void Clipboard::DoDelete (::Window* )
{
    if (mrController.GetModel().GetPageCount() > 1)
    {
        mrController.DeleteSelectedPages();
    }
}




void Clipboard::DoCopy (::Window* pWindow )
{
    CreateSlideTransferable( pWindow, FALSE );
}




void Clipboard::DoPaste (::Window* pWindow)
{
    SdTransferable* pClipTransferable = SD_MOD()->pTransferClip;

    if (pClipTransferable!=NULL && pClipTransferable->IsPageTransferable())
    {
        sal_Int32 nInsertPosition = GetInsertionPosition(pWindow);

        if (nInsertPosition >= 0)
        {
            // Paste the pages from the clipboard.
            sal_Int32 nInsertPageCount = PasteTransferable(nInsertPosition);
            // Select the pasted pages and make the first of them the
            // current page.
            mrController.GetView().GetWindow()->GrabFocus();
            SelectPageRange(nInsertPosition, nInsertPageCount);
        }
    }
}




sal_Int32 Clipboard::GetInsertionPosition (::Window* pWindow)
{
    sal_Int32 nInsertPosition = -1;

    // Determine the insertion position:
    // a) When the insertion indicator is visible, then at that position.
    // b) When the focus indicator is visible, then before or after the
    // focused page, depending on user input to a dialog.
    // c) When there is a selection but no focus, then after the
    // selection.
    // d) After the last page when there is no selection and no focus.

    view::InsertionIndicatorOverlay& rInsertionIndicatorOverlay (
        mrController.GetView().GetOverlay().GetInsertionIndicatorOverlay());
    if (rInsertionIndicatorOverlay.IsShowing())
    {
        nInsertPosition = rInsertionIndicatorOverlay.GetInsertionPageIndex();
    }
    else if (mrController.GetFocusManager().IsFocusShowing())
    {
        SdInsertPasteDlg aDialog (pWindow);
        if (aDialog.Execute() == RET_OK)
        {
            nInsertPosition = mrController.GetFocusManager().GetFocusedPageIndex();
            if ( ! aDialog.IsInsertBefore())
                nInsertPosition ++;
        }
    }
    else
    {
        model::SlideSorterModel::Enumeration aSelectedPages
            (mrController.GetModel().GetSelectedPagesEnumeration());
        // Initialize (for the case of an empty selection) with the position
        // at the end of the document.
        nInsertPosition = mrController.GetModel().GetPageCount();
        while (aSelectedPages.HasMoreElements())
        {
            nInsertPosition = aSelectedPages.GetNextElement()->GetPage()->GetPageNum();
            // Convert *2+1 index to straight index ((n-1)/2) after the page
            // (+1).
            nInsertPosition = (nInsertPosition-1)/2 + 1;
        }
    }

    return nInsertPosition;
}




sal_Int32 Clipboard::PasteTransferable (sal_Int32 nInsertPosition)
{
    SdTransferable* pClipTransferable = SD_MOD()->pTransferClip;
    bool bMergeMasterPages = !pClipTransferable->HasSourceDoc (
        mrController.GetModel().GetDocument());
    USHORT nInsertIndex ((USHORT)(nInsertPosition * 2 + 1));
    sal_Int32 nInsertPageCount (0);
    if (pClipTransferable->HasPageBookmarks())
    {
        const List& rBookmarkList = pClipTransferable->GetPageBookmarks();
        const ::vos::OGuard aGuard (Application::GetSolarMutex());

        nInsertPageCount = (USHORT) rBookmarkList.Count();
        mrController.GetModel().GetDocument()->InsertBookmarkAsPage(
            const_cast<List*>(&rBookmarkList),
            NULL,
            FALSE,
            FALSE,
            nInsertIndex,
            FALSE,
            pClipTransferable->GetPageDocShell(),
            TRUE,
            bMergeMasterPages,
            FALSE);
    }
    else
    {
        SfxObjectShell* pShell = pClipTransferable->GetDocShell();
        DrawDocShell* pDataDocSh = (DrawDocShell*)pShell;
        SdDrawDocument* pDataDoc = pDataDocSh->GetDoc();

        if (pDataDoc!=NULL
            && pDataDoc->GetSdPageCount(PK_STANDARD))
        {
            const ::vos::OGuard aGuard (Application::GetSolarMutex());

            bMergeMasterPages = (pDataDoc != mrController.GetModel().GetDocument());
            nInsertPageCount = pDataDoc->GetSdPageCount( PK_STANDARD );
            mrController.GetModel().GetDocument()->InsertBookmarkAsPage(
                NULL,
                NULL,
                FALSE,
                FALSE,
                nInsertIndex,
                FALSE,
                pDataDocSh,
                TRUE,
                bMergeMasterPages,
                FALSE);
        }
    }
    mrController.HandleModelChange();
    return nInsertPageCount;
}




void Clipboard::SelectPageRange (sal_Int32 nFirstIndex, sal_Int32 nPageCount)
{
    // Select the newly inserted pages.  That are the nInsertPageCount pages
    // after the nInsertIndex position.
    PageSelector& rSelector (mrController.GetPageSelector());
    rSelector.DeselectAllPages();
    for (USHORT i=0; i<nPageCount; i++)
    {
        model::SharedPageDescriptor pDescriptor (
            mrController.GetModel().GetPageDescriptor(nFirstIndex + i));
        if (pDescriptor.get() != NULL)
        {
            rSelector.SelectPage(pDescriptor);
            // The first page of the new selection is made the current page.
            if (i == 0)
            {
                rSelector.SetCurrentPage(pDescriptor);
                mrController.GetFocusManager().SetFocusedPage(pDescriptor);
            }
        }
    }
}




void Clipboard::CreateSlideTransferable (
    ::Window* pWindow,
    bool bDrag)
{
    List aBookmarkList;

    // Insert all selected pages into a bookmark list and remember them in
    // maPagesToRemove for possible later removal.
    model::SlideSorterModel::Enumeration aSelectedPages
        (mrController.GetModel().GetSelectedPagesEnumeration());
    while (aSelectedPages.HasMoreElements())
    {
        model::SharedPageDescriptor pDescriptor (aSelectedPages.GetNextElement());
        aBookmarkList.Insert (
            new String(pDescriptor->GetPage()->GetName()),
            LIST_APPEND);
        maPagesToRemove.push_back (pDescriptor->GetPage());
    }

    if (aBookmarkList.Count() > 0)
    {
        mrController.GetView().BrkAction();
        SdDrawDocument* pDocument = mrController.GetModel().GetDocument();
        SdTransferable* pTransferable = new Transferable (
            pDocument, NULL, FALSE, &mrController.GetViewShell());

        if (bDrag)
            SD_MOD()->pTransferDrag = pTransferable;
        else
            SD_MOD()->pTransferClip = pTransferable;

        pDocument->CreatingDataObj (pTransferable);
        pTransferable->SetWorkDocument( dynamic_cast<SdDrawDocument*>(pDocument->AllocModel()) );
        pDocument->CreatingDataObj (NULL);
        TransferableObjectDescriptor aObjDesc;
        pTransferable->GetWorkDocument()->GetDocSh()
            ->FillTransferableObjectDescriptor (aObjDesc);

        if (pDocument->GetDocSh() != NULL)
            aObjDesc.maDisplayName = pDocument->GetDocSh()
                ->GetMedium()->GetURLObject().GetURLNoPass();

        ::Window* pActionWindow = pWindow;
        if (pActionWindow == NULL)
            pActionWindow = mrController.GetViewShell().GetActiveWindow();

        pTransferable->SetStartPos (pActionWindow->PixelToLogic(
            pActionWindow->GetPointerPosPixel()));
        pTransferable->SetObjectDescriptor (aObjDesc);
        pTransferable->SetPageBookmarks (aBookmarkList, !bDrag);

        for (void* p=aBookmarkList.First(); p!=NULL; p=aBookmarkList.Next())
            delete static_cast<String*>(p);

        if (bDrag)
        {
            pTransferable->SetView (&mrController.GetView());
            sal_Int8 nDragSourceActions (DND_ACTION_COPY);
            // The move action is available only when not all pages would be
            // moved.  Otherwise an empty document would remain.  Crash.
            sal_Int32 nRemainingPages = mrController.GetModel().GetPageCount() - aBookmarkList.Count();
            if (nRemainingPages > 0)
                nDragSourceActions |= DND_ACTION_MOVE;
            pTransferable->StartDrag (pActionWindow, nDragSourceActions);
        }
        else
            pTransferable->CopyToClipboard (pActionWindow);
    }
}




void Clipboard::StartDrag (
    const Point&,
    ::Window* pWindow)
{
    maPagesToRemove.clear();
    maPagesToSelect.clear();
    mbUpdateSelectionPending = false;
    CreateSlideTransferable (pWindow, TRUE);
}




void Clipboard::DragFinished (sal_Int8 nDropAction)
{
    // Hide the substitution display and insertion indicator.
    mrController.GetView().GetOverlay().GetSubstitutionOverlay().Hide();
    mrController.GetView().GetOverlay().GetInsertionIndicatorOverlay().Hide();

    SdTransferable* pDragTransferable = SD_MOD()->pTransferDrag;

    if (pDragTransferable != NULL)
        pDragTransferable->SetView (NULL);

    PageSelector& rSelector (mrController.GetPageSelector());
    if ((nDropAction & DND_ACTION_MOVE) != 0
        && ! maPagesToRemove.empty())
    {
        // Remove the pages that have been moved to another place (possibly
        // in the same document.)
        rSelector.DeselectAllPages();
        PageList::iterator aDraggedPage;
        for (aDraggedPage=maPagesToRemove.begin();
             aDraggedPage!=maPagesToRemove.end();
             aDraggedPage++)
        {
            rSelector.SelectPage (*aDraggedPage);
        }
        mrController.DeleteSelectedPages ();
    }

    SelectPages();
}




void Clipboard::SelectPages (void)
{
    PageSelector& rSelector (mrController.GetPageSelector());

    // Select the dropped pages.
    PageList::iterator iPage;
    rSelector.DeselectAllPages();
    for (iPage=maPagesToSelect.begin(); iPage!=maPagesToSelect.end(); ++iPage)
    {
        rSelector.SelectPage(*iPage);
    }
}




sal_Int8 Clipboard::AcceptDrop (
    const AcceptDropEvent& rEvent,
    DropTargetHelper& rTargetHelper,
    ::sd::Window* pTargetWindow,
    USHORT nPage,
    USHORT nLayer)
{
    sal_Int8 nResult = DND_ACTION_NONE;

    switch (IsDropAccepted())
    {
        case DT_PAGE:
        {
            // Accept a drop.
            nResult = rEvent.mnAction;

            // Use the copy action when the drop action is the default, i.e. not
            // explicitly set to move or link, and when the source and
            // target models are not the same.
            const SdTransferable* pDragTransferable = SD_MOD()->pTransferDrag;
            if (pDragTransferable != NULL
                && pDragTransferable->IsPageTransferable()
                && ((rEvent.maDragEvent.DropAction
                        & ::com::sun::star::datatransfer::dnd::DNDConstants::ACTION_DEFAULT) != 0)
                && (mrController.GetModel().GetDocument()->GetDocSh()
                    != pDragTransferable->GetPageDocShell()))
            {
                nResult = DND_ACTION_COPY;
            }

            // Show the insertion marker and the substitution for a drop.
            Point aPosition = pTargetWindow->PixelToLogic (rEvent.maPosPixel);
            view::ViewOverlay& rOverlay (mrController.GetView().GetOverlay());
            rOverlay.GetInsertionIndicatorOverlay().SetPosition (aPosition);
            rOverlay.GetInsertionIndicatorOverlay().Show();
            rOverlay.GetSubstitutionOverlay().SetPosition (aPosition);

            // Scroll the window when the mouse reaches the window border.
            mrController.GetScrollBarManager().AutoScroll (rEvent.maPosPixel);
        }
        break;

        case DT_SHAPE:
            nResult = ExecuteOrAcceptShapeDrop(
                DC_ACCEPT,
                rEvent.maPosPixel,
                &rEvent,
                rTargetHelper,
                pTargetWindow,
                nPage,
                nLayer);
            break;
        default:
            break;
    }

    return nResult;
}




sal_Int8 Clipboard::ExecuteDrop (
    const ExecuteDropEvent& rEvent,
    DropTargetHelper& rTargetHelper,
    ::sd::Window* pTargetWindow,
    USHORT nPage,
    USHORT nLayer)
{
    sal_Int8 nResult = DND_ACTION_NONE;

    switch (IsDropAccepted())
    {
        case DT_PAGE:
        {
            const SdTransferable* pDragTransferable = SD_MOD()->pTransferDrag;
            const Point aEventModelPosition (
                pTargetWindow->PixelToLogic (rEvent.maPosPixel));
            long int nXOffset = labs (pDragTransferable->GetStartPos().X()
                - aEventModelPosition.X());
            long int nYOffset = labs (pDragTransferable->GetStartPos().Y()
                - aEventModelPosition.Y());
            const bool bContinue =
                ( pDragTransferable->GetView() != &mrController.GetView() )
                || ( nXOffset >= 2 && nYOffset >= 2 );

            // Get insertion position and then turn off the insertion indicator.
            view::ViewOverlay& rOverlay (mrController.GetView().GetOverlay());
            rOverlay.GetInsertionIndicatorOverlay().SetPosition(
                aEventModelPosition);
            USHORT nIndex = DetermineInsertPosition (*pDragTransferable);
            OSL_TRACE ("Clipboard::AcceptDrop() called for index %d",
                nIndex);
            rOverlay.GetInsertionIndicatorOverlay().Hide();

            if (bContinue)
            {
                SlideSorterController::ModelChangeLock aModelChangeLock (mrController);

                if (pDragTransferable->GetView() == &mrController.GetView()
                    && rEvent.mnAction == DND_ACTION_MOVE)
                {
                    // We are asked to move pages inside one view.  For this we
                    // call MoveSelectedPages() which is faster than going the
                    // generic way.

                    // Remember to select the moved pages afterwards.
                    maPagesToRemove.swap(maPagesToSelect);
                    maPagesToRemove.clear();

                    USHORT nSdrModelIndex;
                    if (nIndex != SDRPAGE_NOTFOUND)
                        nSdrModelIndex = nIndex / 2 - 1;
                    else
                        nSdrModelIndex = SDRPAGE_NOTFOUND;
                    mrController.MoveSelectedPages(nSdrModelIndex);
                    mbUpdateSelectionPending = true;
                    nResult = DND_ACTION_NONE;
                }
                else
                {
                    // Handle a general drop operation.
                    HandlePageDrop (*pDragTransferable);
                    nResult = rEvent.mnAction;
                }
            }
        }
        break;

        case DT_SHAPE:
            nResult = ExecuteOrAcceptShapeDrop(
                DC_EXECUTE,
                rEvent.maPosPixel,
                &rEvent,
                rTargetHelper,
                pTargetWindow,
                nPage,
                nLayer);
            break;
        default:
            break;
    }

    return nResult;
}




USHORT Clipboard::DetermineInsertPosition (const SdTransferable& )
{
    USHORT nInsertPosition = SDRPAGE_NOTFOUND;

    // Tell the model to move the dragged pages behind the one with the
    // index nInsertionIndex which first has to be transformed into an index
    // understandable by the document.
    view::InsertionIndicatorOverlay& rOverlay (
        mrController.GetView().GetOverlay().GetInsertionIndicatorOverlay());
    sal_Int32 nInsertionIndex (rOverlay.GetInsertionPageIndex());

    // The index returned by the overlay starts with 1 for the first slide.
    // This is now converted that to an SdModel index that also starts with 1.
    if (nInsertionIndex >= 0)
        nInsertPosition = (USHORT)nInsertionIndex * 2 + 1;

    return nInsertPosition;
}




USHORT Clipboard::InsertSlides (
    const SdTransferable& rTransferable,
    USHORT nInsertPosition)
{
    USHORT nInsertedPageCount = ViewClipboard::InsertSlides (
        rTransferable,
        nInsertPosition);

    // Remember the inserted pages so that they can be selected when the
    // operation is finished.
    int nDocumentIndex = nInsertPosition / 2 - 1;
    for (USHORT i=1; i<=nInsertedPageCount; i++)
    {
        model::SharedPageDescriptor pDescriptor (
            mrController.GetModel().GetPageDescriptor(nDocumentIndex + i));
        if (pDescriptor.get() != NULL)
            maPagesToSelect.push_back (pDescriptor->GetPage());
    }

    mbUpdateSelectionPending |= (nInsertedPageCount>0);

    return nInsertedPageCount;
}




Clipboard::DropType Clipboard::IsDropAccepted (void) const
{
    DropType eResult (DT_NONE);

    const SdTransferable* pDragTransferable = SD_MOD()->pTransferDrag;
    if (pDragTransferable != NULL)
    {
        if (pDragTransferable->IsPageTransferable())
        {
            if (mrController.GetModel().GetEditMode() != EM_MASTERPAGE)
                eResult = DT_PAGE;
        }
        else
        {
            eResult = DT_SHAPE;
        }
    }

    return eResult;
}




sal_Int8 Clipboard::ExecuteOrAcceptShapeDrop (
    DropCommand eCommand,
    const Point& rPosition,
    const void* pDropEvent,
    DropTargetHelper& rTargetHelper,
    ::sd::Window* pTargetWindow,
    USHORT nPage,
    USHORT nLayer)
{
    sal_Int8 nResult = 0;

    // The dropping of a shape is accepted or executed only when there is
    // DrawViewShell available to which we can forward this call.  This has
    // technical reasons:  The actual code to accept or execute a shape drop
    // is implemented in the ViewShell class and uses the page view of the
    // main edit view.  This is not possible without a DrawViewShell.
    DrawViewShell* pDrawViewShell = dynamic_cast<DrawViewShell*>(
        mrController.GetViewShell().GetViewShellBase().GetMainViewShell().get());
    if (pDrawViewShell != NULL
        && (pDrawViewShell->GetShellType() == ViewShell::ST_IMPRESS
            || pDrawViewShell->GetShellType() == ViewShell::ST_DRAW))
    {
        // The drop is only accepted or executed when it takes place over a
        // page object.  Therefore we replace a missing page number by the
        // number of the page under the mouse.
        if (nPage == SDRPAGE_NOTFOUND)
        {
            model::SharedPageDescriptor pDescriptor (
                mrController.GetModel().GetPageDescriptor(
                    mrController.GetView().GetPageIndexAtPoint(rPosition)));
            if (pDescriptor.get() != NULL && pDescriptor->GetPage()!=NULL)
                nPage = (pDescriptor->GetPage()->GetPageNum() - 1) / 2;
        }

        // Now comes the code that is different for the Execute and Accept:
        // We simply forward the call to the AcceptDrop() or ExecuteDrop()
        // methods of the DrawViewShell in the center pane.
        if (nPage != SDRPAGE_NOTFOUND)
            switch (eCommand)
            {
                case DC_ACCEPT:
                    nResult = pDrawViewShell->AcceptDrop(
                        *reinterpret_cast<const AcceptDropEvent*>(pDropEvent),
                        rTargetHelper,
                        pTargetWindow,
                        nPage,
                        nLayer);
                    break;

                case DC_EXECUTE:
                    nResult = pDrawViewShell->ExecuteDrop(
                        *reinterpret_cast<const ExecuteDropEvent*>(pDropEvent),
                        rTargetHelper,
                        pTargetWindow,
                        nPage,
                        nLayer);
                    break;
            }
    }

    return nResult;
}



} } } // end of namespace ::sd::slidesorter::controller

