/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsPageSelector.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:15:31 $
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

#include "controller/SlsPageSelector.hxx"

#include "SlideSorterViewShell.hxx"
#include "controller/SlideSorterController.hxx"
#include "model/SlsPageDescriptor.hxx"
#include "model/SlsPageEnumeration.hxx"
#include "model/SlideSorterModel.hxx"
#include "view/SlideSorterView.hxx"

#include "sdpage.hxx"
#include "ViewShell.hxx"
#include "DrawViewShell.hxx"
#include "ViewShellBase.hxx"

#ifndef _COM_SUN_STAR_DRAWING_XDRAWVIEW_HPP_
#include <com/sun/star/drawing/XDrawView.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::sd::slidesorter::model;
using namespace ::sd::slidesorter::view;

namespace sd { namespace slidesorter { namespace controller {


PageSelector::PageSelector (
    model::SlideSorterModel& rModel,
    SlideSorterController& rController)
    : mrModel(rModel),
      mrController (rController),
      mnSelectedPageCount(0),
      mnBroadcastDisableLevel(0),
      mbSelectionChangeBroadcastPending(false),
      mpMostRecentlySelectedPage(NULL),
      mpSelectionAnchor(NULL)
{
    CountSelectedPages ();
}




void PageSelector::SelectAllPages (void)
{
    int nPageCount = mrModel.GetPageCount();
    for (int nPageIndex=0; nPageIndex<nPageCount; nPageIndex++)
        SelectPage (nPageIndex);
}




void PageSelector::DeselectAllPages (void)
{
    int nPageCount = mrModel.GetPageCount();
    for (int nPageIndex=0; nPageIndex<nPageCount; nPageIndex++)
        DeselectPage (nPageIndex);
    DBG_ASSERT (mnSelectedPageCount==0,
        "PageSelector::DeselectAllPages: the selected pages counter is not 0");
    mnSelectedPageCount = 0;
    mpMostRecentlySelectedPage = NULL;
    mpSelectionAnchor = NULL;
}




void PageSelector::UpdateAllPages (void)
{
    mnSelectedPageCount = 0;
    model::SlideSorterModel::Enumeration aAllPages (
        mrModel.GetAllPagesEnumeration());
    while (aAllPages.HasMoreElements())
    {
        model::PageDescriptor& rDescriptor (aAllPages.GetNextElement());
        if (rDescriptor.UpdateSelection())
        {
            mrController.GetView().RequestRepaint (rDescriptor);
            if (mnBroadcastDisableLevel > 0)
                mbSelectionChangeBroadcastPending = true;
            else
                mrController.SelectionHasChanged();
        }

        if (rDescriptor.IsSelected())
            mnSelectedPageCount++;
    }
}




void PageSelector::SelectPage (int nPageIndex)
{
    PageDescriptor* pDescriptor = mrModel.GetPageDescriptor(nPageIndex);
    if (pDescriptor != NULL)
        SelectPage (*pDescriptor);
}




void PageSelector::SelectPage (const SdPage* pPage)
{
    int nPageIndex = (pPage->GetPageNum()-1) / 2;
    PageDescriptor* pDescriptor = mrModel.GetPageDescriptor(nPageIndex);
    if (pDescriptor!=NULL && pDescriptor->GetPage()==pPage)
        SelectPage (*pDescriptor);
}




void PageSelector::SelectPage (PageDescriptor& rDescriptor)
{
    if (rDescriptor.Select())
    {
        mnSelectedPageCount ++;
        mrController.GetView().RequestRepaint (rDescriptor);

        mpMostRecentlySelectedPage = &rDescriptor;
        if (mpSelectionAnchor == NULL)
            mpSelectionAnchor = &rDescriptor;

        if (mnBroadcastDisableLevel > 0)
            mbSelectionChangeBroadcastPending = true;
        else
            mrController.SelectionHasChanged();
    }
}




void PageSelector::DeselectPage (int nPageIndex)
{
    model::PageDescriptor* pDescriptor (
        mrModel.GetPageDescriptor(nPageIndex));
    if (pDescriptor != NULL)
        DeselectPage (*pDescriptor);
}




void PageSelector::DeselectPage (const SdPage* pPage)
{
    int nPageIndex = (pPage->GetPageNum()-1) / 2;
    PageDescriptor* pDescriptor = mrModel.GetPageDescriptor(nPageIndex);
    if (pDescriptor!=NULL && pDescriptor->GetPage()==pPage)
        DeselectPage (*pDescriptor);
}




void PageSelector::DeselectPage (PageDescriptor& rDescriptor)
{
    if (rDescriptor.Deselect())
    {
        mnSelectedPageCount --;
        mrController.GetView().RequestRepaint (rDescriptor);
        if (mpMostRecentlySelectedPage == &rDescriptor)
            mpMostRecentlySelectedPage = NULL;
        if (mnBroadcastDisableLevel > 0)
            mbSelectionChangeBroadcastPending = true;
        else
            mrController.SelectionHasChanged();
    }
}




bool PageSelector::IsPageSelected (int nPageIndex)
{
    PageDescriptor* pDescriptor = mrModel.GetPageDescriptor(nPageIndex);
    if (pDescriptor != NULL)
        return pDescriptor->IsSelected();
    else
        return false;
}




void PageSelector::SetCurrentPage (PageDescriptor& rDescriptor)
{
    // Set current page.  At the moment we have to do this in two different
    // ways.  The UNO way is the preferable one but, alas, it does not work
    // always correctly (after some kinds of model changes).  Therefore, we
    // call DrawViewShell::SwitchPage(), too.
    try
    {
        // First the traditional way.
        DrawViewShell* pDrawViewShell = dynamic_cast<DrawViewShell*>(
            mrController.GetViewShell().GetViewShellBase().GetMainViewShell());
        if (pDrawViewShell != NULL)
        {
            USHORT nPageNumber = (rDescriptor.GetPage()->GetPageNum()-1)/2;
            pDrawViewShell->SwitchPage(nPageNumber);
            pDrawViewShell->GetPageTabControl()->SetCurPageId(nPageNumber+1);
        }

        // Now the UNO way.
        do
        {
            Reference<beans::XPropertySet> xSet (
                mrController.GetViewShell().GetViewShellBase().GetController(),
                UNO_QUERY);
            if ( ! xSet.is())
                break;

            Any aPage;
            aPage <<= rDescriptor.GetPage()->getUnoPage();
            xSet->setPropertyValue (
                String::CreateFromAscii("CurrentPage"),
                aPage);
        }
        while (false);
    }
    catch (beans::UnknownPropertyException aException)
    {
        // We have not been able to set the current page at the main view.
        // This is sad but still leaves us in a valid state.  Therefore,
        // this exception is silently ignored.
    }
}




void PageSelector::SetCurrentPage (const SdPage* pPage)
{
    int nPageIndex = (pPage->GetPageNum()-1) / 2;
    PageDescriptor* pDescriptor = mrModel.GetPageDescriptor(nPageIndex);
    if (pDescriptor!=NULL && pDescriptor->GetPage()==pPage)
        SetCurrentPage(*pDescriptor);
}




void PageSelector::SetCurrentPage (int nPageIndex)
{
    PageDescriptor* pDescriptor = mrModel.GetPageDescriptor(nPageIndex);
    if (pDescriptor != NULL)
        SetCurrentPage (*pDescriptor);
}




int PageSelector::GetPageCount (void) const
{
    return mrModel.GetPageCount();
}




int PageSelector::GetSelectedPageCount (void) const
{
    return mnSelectedPageCount;
}




void PageSelector::PrepareModelChange (void)
{
    DeselectAllPages ();
}




void PageSelector::HandleModelChange (void)
{
    UpdateAllPages();
}




PageDescriptor* PageSelector::GetMostRecentlySelectedPage (void) const
{
    return mpMostRecentlySelectedPage;
}




PageDescriptor* PageSelector::GetSelectionAnchor (void) const
{
    return mpSelectionAnchor;
}




void PageSelector::CountSelectedPages (void)
{
    mnSelectedPageCount = 0;
    model::SlideSorterModel::Enumeration aSelectedPages (
        mrModel.GetSelectedPagesEnumeration());
    while (aSelectedPages.HasMoreElements())
    {
        mnSelectedPageCount++;
        aSelectedPages.GetNextElement();
    }
}




void PageSelector::EnableBroadcasting (bool bMakeSelectionVisible)
{
    if (mnBroadcastDisableLevel > 0)
        mnBroadcastDisableLevel --;
    if (mnBroadcastDisableLevel==0 && mbSelectionChangeBroadcastPending)
    {
        mrController.SelectionHasChanged(bMakeSelectionVisible);
        mbSelectionChangeBroadcastPending = false;
    }
}




void PageSelector::DisableBroadcasting (void)
{
    mnBroadcastDisableLevel ++;
}




::std::auto_ptr<PageSelector::PageSelection>
    PageSelector::GetPageSelection (void)
{
    ::std::auto_ptr<PageSelection> pSelection (new PageSelection());

    int nPageCount = GetPageCount();
    for (int nIndex=0; nIndex<nPageCount; nIndex++)
    {
        if (IsPageSelected(nIndex))
            pSelection->insert (nIndex);
    }

    return pSelection;
}




void PageSelector::SetPageSelection (const PageSelection& rSelection)
{
    PageSelection::const_iterator iIndex;
    for (iIndex=rSelection.begin(); iIndex!=rSelection.end(); ++iIndex)
        SelectPage (*iIndex);
}

} } } // end of namespace ::sd::slidesorter::controller
