/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SlsSelectionManager.cxx,v $
 *
 * $Revision: 1.4 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include "precompiled_sd.hxx"

#include "controller/SlsSelectionManager.hxx"

#include "SlideSorter.hxx"
#include "SlsSelectionCommand.hxx"
#include "controller/SlideSorterController.hxx"
#include "controller/SlsAnimator.hxx"
#include "controller/SlsAnimationFunction.hxx"
#include "controller/SlsCurrentSlideManager.hxx"
#include "controller/SlsFocusManager.hxx"
#include "controller/SlsProperties.hxx"
#include "controller/SlsScrollBarManager.hxx"
#include "controller/SlsSlotManager.hxx"
#include "model/SlideSorterModel.hxx"
#include "model/SlsPageEnumerationProvider.hxx"
#include "model/SlsPageDescriptor.hxx"
#include "view/SlideSorterView.hxx"
#include "view/SlsLayouter.hxx"
#include "drawdoc.hxx"
#include "Window.hxx"
#include <svx/svxids.hrc>
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>

#include "res_bmp.hrc"
#include "sdresid.hxx"
#include "strings.hrc"
#include "app.hrc"
#include "glob.hrc"


using namespace ::com::sun::star;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::uno;
using namespace ::sd::slidesorter::model;
using namespace ::sd::slidesorter::view;
using namespace ::sd::slidesorter::controller;

namespace sd { namespace slidesorter { namespace controller {


namespace {
    class VisibleAreaScroller
    {
    public:
        VisibleAreaScroller (
            SlideSorter& rSlideSorter,
            const Point aStart,
            const Point aEnd);
        void operator() (const double nValue);
    private:
        SlideSorter& mrSlideSorter;
        Point maStart;
        const Point maEnd;
        const ::boost::function<double(double)> maAccelerationFunction;
    };
}




SelectionManager::SelectionManager (SlideSorter& rSlideSorter)
    : mrSlideSorter(rSlideSorter),
      mrController(rSlideSorter.GetController()),
      maSelectionBeforeSwitch(),
      mbIsMakeSelectionVisiblePending(true),
      mnInsertionPosition(-1),
      mnAnimationId(Animator::NotAnAnimationId),
      maRequestedTopLeft()
{
}




SelectionManager::~SelectionManager (void)
{
    if (mnAnimationId != Animator::NotAnAnimationId)
        mrController.GetAnimator()->RemoveAnimation(mnAnimationId);
}




void SelectionManager::DeleteSelectedPages (void)
{
    // Create some locks to prevent updates of the model, view, selection
    // state while modifying any of them.
    SlideSorterController::ModelChangeLock aLock (mrController);
    SlideSorterView::DrawLock aDrawLock (mrSlideSorter);
    PageSelector::UpdateLock aSelectionLock (mrSlideSorter);

    // Hide focus.
    bool bIsFocusShowing = mrController.GetFocusManager().IsFocusShowing();
    if (bIsFocusShowing)
        mrController.GetFocusManager().ToggleFocus();

    // Store pointers to all selected page descriptors.  This is necessary
    // because the pages get deselected when the first one is deleted.
    model::PageEnumeration aPageEnumeration (
        PageEnumerationProvider::CreateSelectedPagesEnumeration(mrSlideSorter.GetModel()));
    ::std::vector<SdPage*> aSelectedPages;
    sal_Int32 nLastPageIndex (-1);
    while (aPageEnumeration.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aPageEnumeration.GetNextElement());
        aSelectedPages.push_back(pDescriptor->GetPage());
        nLastPageIndex = pDescriptor->GetPageIndex();
    }
    if (aSelectedPages.empty())
        return;

    // Determine the slide to select (and thereby make the current slide)
    // after the deletion.
    sal_Int32 nNewCurrentSlide (nLastPageIndex - aSelectedPages.size() + 1);

    // The actual deletion of the selected pages is done in one of two
    // helper functions.  They are specialized for normal respectively for
    // master pages.
    mrSlideSorter.GetView().BegUndo (SdResId(STR_UNDO_DELETEPAGES));
    if (mrSlideSorter.GetModel().GetEditMode() == EM_PAGE)
        DeleteSelectedNormalPages(aSelectedPages);
    else
        DeleteSelectedMasterPages(aSelectedPages);
    mrSlideSorter.GetView().EndUndo ();

    mrController.HandleModelChange();
    aLock.Release();

    // Show focus and move it to next valid location.
    if (bIsFocusShowing)
        mrController.GetFocusManager().ToggleFocus();

    // Set the new current slide.
    if (nNewCurrentSlide >= mrSlideSorter.GetModel().GetPageCount())
        nNewCurrentSlide = mrSlideSorter.GetModel().GetPageCount()-1;
    mrController.GetPageSelector().SelectPage(nNewCurrentSlide);
    mrController.GetFocusManager().SetFocusedPage(nNewCurrentSlide);
}




void SelectionManager::DeleteSelectedNormalPages (const ::std::vector<SdPage*>& rSelectedPages)
{
    // Prepare the deletion via the UNO API.
    OSL_ASSERT(mrSlideSorter.GetModel().GetEditMode() == EM_PAGE);

    try
    {
        Reference<drawing::XDrawPagesSupplier> xDrawPagesSupplier( mrSlideSorter.GetModel().GetDocument()->getUnoModel(), UNO_QUERY_THROW );
        Reference<drawing::XDrawPages> xPages( xDrawPagesSupplier->getDrawPages(), UNO_QUERY_THROW );

        // Iterate over all pages that where seleted when this method was called
        // and delete the draw page the notes page.  The iteration is done in
        // reverse order so that when one slide is not deleted (to avoid an
        // empty document) the remaining slide is the first one.
        ::std::vector<SdPage*>::const_reverse_iterator aI;
        for (aI=rSelectedPages.rbegin(); aI!=rSelectedPages.rend(); aI++)
        {
            // Do not delete the last slide in the document.
            if (xPages->getCount() <= 1)
                break;

            USHORT nPage = ((*aI)->GetPageNum()-1) / 2;

            Reference< XDrawPage > xPage( xPages->getByIndex( nPage ), UNO_QUERY_THROW );
            xPages->remove(xPage);
        }
    }
    catch( Exception& )
    {
        DBG_ERROR("SelectionManager::DeleteSelectedNormalPages(), exception caught!");
    }
}




void SelectionManager::DeleteSelectedMasterPages (const ::std::vector<SdPage*>& rSelectedPages)
{
    // Prepare the deletion via the UNO API.
    OSL_ASSERT(mrSlideSorter.GetModel().GetEditMode() == EM_MASTERPAGE);

    try
    {
        Reference<drawing::XMasterPagesSupplier> xDrawPagesSupplier( mrSlideSorter.GetModel().GetDocument()->getUnoModel(), UNO_QUERY_THROW );
        Reference<drawing::XDrawPages> xPages( xDrawPagesSupplier->getMasterPages(), UNO_QUERY_THROW );

        // Iterate over all pages that where seleted when this method was called
        // and delete the draw page the notes page.  The iteration is done in
        // reverse order so that when one slide is not deleted (to avoid an
        // empty document) the remaining slide is the first one.
        ::std::vector<SdPage*>::const_reverse_iterator aI;
        for (aI=rSelectedPages.rbegin(); aI!=rSelectedPages.rend(); aI++)
        {
            // Do not delete the last slide in the document.
            if (xPages->getCount() <= 1)
                break;

            USHORT nPage = ((*aI)->GetPageNum()-1) / 2;

            Reference< XDrawPage > xPage( xPages->getByIndex( nPage ), UNO_QUERY_THROW );
            xPages->remove(xPage);
        }
    }
    catch( Exception& )
    {
        DBG_ERROR("SelectionManager::DeleteSelectedMasterPages(), exception caught!");
    }
}




bool SelectionManager::MoveSelectedPages (const sal_Int32 nTargetPageIndex)
{
    bool bMoved (false);
    PageSelector& rSelector (mrController.GetPageSelector());

    view::SlideSorterView::DrawLock aDrawLock (mrSlideSorter);
    SlideSorterController::ModelChangeLock aLock (mrController);

    // Transfer selection of the slide sorter to the document.
    mrSlideSorter.GetModel().SynchronizeDocumentSelection ();

    // Detect how many pages lie between document start and insertion
    // position.
    sal_Int32 nPageCountBeforeTarget (0);
    ::boost::shared_ptr<PageSelector::PageSelection> pSelection (rSelector.GetPageSelection());
    PageSelector::PageSelection::const_iterator iSelectedPage (pSelection->begin());
    PageSelector::PageSelection::const_iterator iSelectionEnd (pSelection->end());
    for ( ; iSelectedPage!=iSelectionEnd; ++iSelectedPage)
    {
        if (*iSelectedPage==NULL)
            continue;
        if (((*iSelectedPage)->GetPageNum()-1)/2 <= nTargetPageIndex)
            ++nPageCountBeforeTarget;
        else
            break;
    }

    // Prepare to select the moved pages.
    SelectionCommand* pCommand = new SelectionCommand(
        rSelector,mrController.GetCurrentSlideManager(),mrSlideSorter.GetModel());
    sal_Int32 nSelectedPageCount (rSelector.GetSelectedPageCount());
    for (sal_Int32 nOffset=0; nOffset<nSelectedPageCount; ++nOffset)
        pCommand->AddSlide(sal::static_int_cast<USHORT>(
            nTargetPageIndex + nOffset - nPageCountBeforeTarget + 1));

    // At the moment we can not move master pages.
    if (nTargetPageIndex>=0)
    {
        if (mrSlideSorter.GetModel().GetEditMode() == EM_PAGE)
            bMoved = mrSlideSorter.GetModel().GetDocument()->MovePages(
                sal::static_int_cast<sal_uInt16>(nTargetPageIndex));
    }
    if (bMoved)
        mrController.GetSlotManager()->ExecuteCommandAsynchronously(
            ::std::auto_ptr<controller::Command>(pCommand));

    return bMoved;
}




void SelectionManager::SelectionHasChanged (const bool bMakeSelectionVisible)
{
    if (bMakeSelectionVisible)
        mbIsMakeSelectionVisiblePending = true;

    ViewShell* pViewShell = mrSlideSorter.GetViewShell();
    if (pViewShell != NULL)
    {
        pViewShell->Invalidate (SID_EXPAND_PAGE);
        pViewShell->Invalidate (SID_SUMMARY_PAGE);
        pViewShell->Invalidate(SID_SHOW_SLIDE);
        pViewShell->Invalidate(SID_HIDE_SLIDE);
        pViewShell->Invalidate(SID_DELETE_PAGE);
        pViewShell->Invalidate(SID_DELETE_MASTER_PAGE);

        // StatusBar
        pViewShell->Invalidate (SID_STATUS_PAGE);
        pViewShell->Invalidate (SID_STATUS_LAYOUT);

        OSL_ASSERT(mrController.GetCurrentSlideManager());
        SharedPageDescriptor pDescriptor(mrController.GetCurrentSlideManager()->GetCurrentSlide());
        if (pDescriptor.get() != NULL)
            pViewShell->UpdatePreview(pDescriptor->GetPage());

        // Tell the slection change listeners that the selection has changed.
        ::std::vector<Link>::iterator iListener (maSelectionChangeListeners.begin());
        ::std::vector<Link>::iterator iEnd (maSelectionChangeListeners.end());
        for (; iListener!=iEnd; ++iListener)
        {
            iListener->Call(NULL);
        }

        // Reset the insertion position: until set again it is calculated from
        // the current selection.
        mnInsertionPosition = -1;
    }
}




bool SelectionManager::IsMakeSelectionVisiblePending (void) const
{
    return mbIsMakeSelectionVisiblePending;
}




void SelectionManager::ResetMakeSelectionVisiblePending (void)
{
    mbIsMakeSelectionVisiblePending = false;
}




/** We have to distinguish three cases: 1) the selection is empty, 2) the
    selection fits completely into the visible area, 3) it does not.
    1) The visible area is not modified.
    2) When the selection fits completely into the visible area we have to
    decide how to align it.  It is done by scrolling it there and thus when
    we scoll up the (towards the end of the document) the bottom of the
    selection is aligned with the bottom of the window.  When we scroll
    down (towards the beginning of the document) the top of the selection is
    aligned with the top of the window.
    3) We have to decide what part of the selection to make visible.  Here
    we use the eSelectionHint and concentrate on either the first, the last,
    or the most recently selected page.  We then again apply the algorithm
    of a).

*/
Size SelectionManager::MakeSelectionVisible (const SelectionHint eSelectionHint)
{
    SharedSdWindow pWindow (mrSlideSorter.GetContentWindow());
    if (pWindow == NULL)
        return Size(0,0);

    mbIsMakeSelectionVisiblePending = false;

    // Determine the descriptors of the first and last selected page and the
    // bounding box that encloses their page objects.
    model::SharedPageDescriptor pFirst;
    model::SharedPageDescriptor pLast;
    Rectangle aSelectionBox;
    const view::Layouter& rLayouter (mrSlideSorter.GetView().GetLayouter());
    model::PageEnumeration aSelectedPages (
        PageEnumerationProvider::CreateSelectedPagesEnumeration(mrSlideSorter.GetModel()));
    while (aSelectedPages.HasMoreElements())
    {
        model::SharedPageDescriptor pDescriptor (aSelectedPages.GetNextElement());

        if (pFirst.get() == NULL)
            pFirst = pDescriptor;
        pLast = pDescriptor;

        aSelectionBox.Union(rLayouter.GetPageObjectBox(pDescriptor->GetPageIndex(), true));
    }

    if (pFirst != NULL)
    {
        // Determine scroll direction and the position in model coordinates
        // that will be aligned with the top/left or bottom/right window
        // border.
        if (DoesSelectionExceedVisibleArea(aSelectionBox))
        {
            // We can show only a part of the selection.
            aSelectionBox = ResolveLargeSelection(pFirst,pLast, eSelectionHint);
        }

        return MakeRectangleVisible(aSelectionBox);
    }

    return Size(0,0);
}




Size SelectionManager::MakeRectangleVisible (const Rectangle& rBox)
{
    SharedSdWindow pWindow (mrSlideSorter.GetContentWindow());
    if (pWindow == NULL)
        return Size(0,0);

    Rectangle aVisibleArea (pWindow->PixelToLogic(
        Rectangle(
            Point(0,0),
            pWindow->GetOutputSizePixel())));

    sal_Int32 nNewTop (aVisibleArea.Top());
    sal_Int32 nNewLeft (aVisibleArea.Left());

    if (mrSlideSorter.GetView().GetOrientation() != SlideSorterView::HORIZONTAL)
    {
        // Scroll the visible area to make aSelectionBox visible.
        if (mrSlideSorter.GetProperties()->IsCenterSelection())
        {
            nNewTop = rBox.Top() - (aVisibleArea.GetHeight() - rBox.GetHeight()) / 2;
        }
        else
        {
            if (rBox.Top() < aVisibleArea.Top())
                nNewTop = rBox.Top();
            else if (rBox.Bottom() > aVisibleArea.Bottom())
                nNewTop = rBox.Bottom() - aVisibleArea.GetHeight();
            // otherwise we do not modify the visible area.
        }

        // Make some corrections of the new visible area.
        Rectangle aModelArea (mrSlideSorter.GetView().GetModelArea());
        if (nNewTop + aVisibleArea.GetHeight() > aModelArea.Bottom())
            nNewTop = aModelArea.GetHeight() - aVisibleArea.GetHeight();
        if (nNewTop < aModelArea.Top())
            nNewTop = aModelArea.Top();

    }

    if (mrSlideSorter.GetView().GetOrientation() != SlideSorterView::VERTICAL)
    {
        // Scroll the visible area to make aSelectionBox visible.
        if (mrSlideSorter.GetProperties()->IsCenterSelection())
        {
            nNewLeft = rBox.Left() - (aVisibleArea.GetWidth() - rBox.GetWidth()) / 2;
        }
        else
        {
            if (rBox.Left() < aVisibleArea.Left())
                nNewLeft = rBox.Left();
            else if (rBox.Right() > aVisibleArea.Right())
                nNewLeft = rBox.Right() - aVisibleArea.GetWidth();
            // otherwise we do not modify the visible area.
        }

        // Make some corrections of the new visible area.
        Rectangle aModelArea (mrSlideSorter.GetView().GetModelArea());
        if (nNewLeft + aVisibleArea.GetWidth() > aModelArea.Right())
            nNewLeft = aModelArea.GetWidth() - aVisibleArea.GetWidth();
        if (nNewLeft < aModelArea.Left())
            nNewLeft = aModelArea.Left();
    }

    // Scroll when the visible area is not already at the requested location
    // and there is no active animation to scroll to it.
    if ((nNewTop != aVisibleArea.Top() || nNewLeft != aVisibleArea.Left())
        && (mnAnimationId==Animator::NotAnAnimationId
            || maRequestedTopLeft != Point(nNewLeft,nNewTop)))
    {
        if (mnAnimationId != Animator::NotAnAnimationId)
            mrController.GetAnimator()->RemoveAnimation(mnAnimationId);

        maRequestedTopLeft = Point(nNewLeft, nNewTop);
        VisibleAreaScroller aAnimation(
            mrSlideSorter,
            aVisibleArea.TopLeft(),
            maRequestedTopLeft);
        if (mrSlideSorter.GetProperties()->IsSmoothSelectionScrolling())
            mnAnimationId = mrController.GetAnimator()->AddAnimation(aAnimation, 0, 300);
        else
            aAnimation(1.0);
    }

    return Size(aVisibleArea.Left() - nNewLeft, aVisibleArea.Top() - nNewTop);
}




void SelectionManager::AddSelectionChangeListener (const Link& rListener)
{
    if (::std::find (
        maSelectionChangeListeners.begin(),
        maSelectionChangeListeners.end(),
        rListener) == maSelectionChangeListeners.end())
    {
        maSelectionChangeListeners.push_back (rListener);
    }
}




void SelectionManager::RemoveSelectionChangeListener(const Link&rListener)
{
    maSelectionChangeListeners.erase (
        ::std::find (
            maSelectionChangeListeners.begin(),
            maSelectionChangeListeners.end(),
            rListener));
}




bool SelectionManager::DoesSelectionExceedVisibleArea (const Rectangle& rSelectionBox) const
{
    SharedSdWindow pWindow (mrSlideSorter.GetContentWindow());
    if (pWindow == NULL)
        return true;

    Rectangle aVisibleArea (pWindow->PixelToLogic(
        Rectangle(
            Point(0,0),
            pWindow->GetOutputSizePixel())));
    if (mrSlideSorter.GetView().GetOrientation() == SlideSorterView::VERTICAL)
        return rSelectionBox.GetHeight() > aVisibleArea.GetHeight();
    else
        return rSelectionBox.GetWidth() > aVisibleArea.GetWidth();
}




Rectangle SelectionManager::ResolveLargeSelection (
    const SharedPageDescriptor& rpFirst,
    const SharedPageDescriptor& rpLast,
    const SelectionHint eSelectionHint)
{
    OSL_ASSERT(rpFirst.get()!=NULL);
    OSL_ASSERT(rpLast.get()!=NULL);

    // The mose recently selected page is assumed to lie in the range
    // between first and last selected page.  Therefore the bounding box is
    // not modified.
    model::SharedPageDescriptor pRecent (
        mrController.GetPageSelector().GetMostRecentlySelectedPage());

    // Get the bounding box of the page object on which to concentrate.
    model::SharedPageDescriptor pRepresentative;
    switch (eSelectionHint)
    {
        case SH_FIRST:
            pRepresentative = rpFirst;
            break;

        case SH_LAST:
            pRepresentative = rpLast;
            break;

        case SH_RECENT:
        default:
            if (pRecent.get() == NULL)
                pRepresentative = rpFirst;
            else
                pRepresentative = pRecent;
            break;
    }
    OSL_ASSERT(pRepresentative.get() != NULL);

    return mrSlideSorter.GetView().GetLayouter().GetPageObjectBox(
        pRepresentative->GetPageIndex(),
        true);
}




sal_Int32 SelectionManager::GetInsertionPosition (void) const
{
    sal_Int32 nInsertionPosition (mnInsertionPosition);
    if (nInsertionPosition < 0)
    {
        model::PageEnumeration aSelectedPages
            (model::PageEnumerationProvider::CreateSelectedPagesEnumeration(
                mrSlideSorter.GetModel()));
        // Initialize (for the case of an empty selection) with the position
        // at the end of the document.
        nInsertionPosition = mrSlideSorter.GetModel().GetPageCount();
        while (aSelectedPages.HasMoreElements())
        {
            const sal_Int32 nPosition (aSelectedPages.GetNextElement()->GetPage()->GetPageNum());
            // Convert *2+1 index to straight index (n-1)/2 after the page
            // (+1).
            nInsertionPosition = (nPosition-1)/2 + 1;
        }

    }
    return nInsertionPosition;
}




void SelectionManager::SetInsertionPosition (const sal_Int32 nInsertionPosition)
{
    if (nInsertionPosition < 0)
        mnInsertionPosition = -1;
    else if (nInsertionPosition > mrSlideSorter.GetModel().GetPageCount())
    {
        // Assert but then ignore invalid values.
        OSL_ASSERT(nInsertionPosition<=mrSlideSorter.GetModel().GetPageCount());
        return;
    }
    else
        mnInsertionPosition = nInsertionPosition;
}




//===== VerticalVisibleAreaScroller ===========================================

namespace {

const static sal_Int32 gnMaxScrollDistance = 300;

VisibleAreaScroller::VisibleAreaScroller (
    SlideSorter& rSlideSorter,
    const Point aStart,
    const Point aEnd)
    : mrSlideSorter(rSlideSorter),
      maStart(aStart),
      maEnd(aEnd),
      maAccelerationFunction(
          controller::AnimationParametricFunction(
              controller::AnimationBezierFunction (0.1,0.6)))
{
    // When the distance to scroll is larger than a threshold then first
    // jump to within this distance of the final value and start the
    // animation from there.
    if (abs(aStart.X()-aEnd.X()) > gnMaxScrollDistance)
        if (aStart.X() < aEnd.X())
            maStart.X() = aEnd.X()-gnMaxScrollDistance;
        else
            maStart.X() = aEnd.X()+gnMaxScrollDistance;
    if (abs(aStart.Y()-aEnd.Y()) > gnMaxScrollDistance)
        if (aStart.Y() < aEnd.Y())
            maStart.Y() = aEnd.Y()-gnMaxScrollDistance;
        else
            maStart.Y() = aEnd.Y()+gnMaxScrollDistance;
}




void VisibleAreaScroller::operator() (const double nTime)
{
    const double nLocalTime (maAccelerationFunction(nTime));
    mrSlideSorter.GetController().GetScrollBarManager().SetTopLeft(
        Point(
            sal_Int32(0.5 + maStart.X() * (1.0 - nLocalTime) + maEnd.X() * nLocalTime),
            sal_Int32 (0.5 + maStart.Y() * (1.0 - nLocalTime) + maEnd.Y() * nLocalTime)));
}

} // end of anonymous namespace

} } } // end of namespace ::sd::slidesorter
