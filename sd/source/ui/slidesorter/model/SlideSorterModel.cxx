/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SlideSorterModel.cxx,v $
 * $Revision: 1.11 $
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

#include "model/SlideSorterModel.hxx"

#include "SlideSorter.hxx"
#include "model/SlsPageDescriptor.hxx"
#include "model/SlsPageEnumerationProvider.hxx"
#include "controller/SlideSorterController.hxx"
#include "controller/SlsProperties.hxx"
#include "controller/SlsPageSelector.hxx"
#include "controller/SlsCurrentSlideManager.hxx"
#include "view/SlideSorterView.hxx"
#include "taskpane/SlideSorterCacheDisplay.hxx"
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/UnknownPropertyException.hpp>

#include "ViewShellBase.hxx"
#include "DrawViewShell.hxx"
#include "DrawDocShell.hxx"
#include "drawdoc.hxx"
#include "sdpage.hxx"
#include "FrameView.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sd { namespace slidesorter { namespace model {

namespace {
    class CompareToXDrawPage
    {
    public:
        CompareToXDrawPage (const Reference<drawing::XDrawPage>& rxSlide) : mxSlide(rxSlide) {}
        bool operator() (const SharedPageDescriptor& rpDescriptor)
        { return rpDescriptor.get()!=NULL && rpDescriptor->GetXDrawPage()==mxSlide; }
    private:
        Reference<drawing::XDrawPage> mxSlide;
    };
}




SlideSorterModel::SlideSorterModel (SlideSorter& rSlideSorter)
    : maMutex(),
      mrSlideSorter(rSlideSorter),
      mxSlides(),
      mePageKind(PK_STANDARD),
      meEditMode(EM_PAGE),
      maPageDescriptors(0)
{
}




SlideSorterModel::~SlideSorterModel (void)
{
    ClearDescriptorList ();
}




void SlideSorterModel::Init (void)
{
}




void SlideSorterModel::Dispose (void)
{
    ClearDescriptorList ();
}




SdDrawDocument* SlideSorterModel::GetDocument (void)
{
    if (mrSlideSorter.GetViewShellBase() != NULL)
        return mrSlideSorter.GetViewShellBase()->GetDocument();
    else
         return NULL;
}




bool SlideSorterModel::SetEditMode (EditMode eEditMode)
{
    bool bEditModeChanged = false;
    if (meEditMode != eEditMode)
    {
        meEditMode = eEditMode;
        UpdatePageList();
        ClearDescriptorList();
        bEditModeChanged = true;
    }
    return bEditModeChanged;
}




bool SlideSorterModel::SetEditModeFromController (void)
{
    bool bIsMasterPageMode = false;
    // Get the edit mode from the controller.
    try
    {
        Reference<beans::XPropertySet> xSet (mrSlideSorter.GetXController(), UNO_QUERY_THROW);
        Any aValue (xSet->getPropertyValue(
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("IsMasterPageMode"))));
        aValue >>= bIsMasterPageMode;
    }
    catch (RuntimeException&)
    {
        // When the property is not supported then the master
        // page mode is not supported, too.
        bIsMasterPageMode = false;
    }

    return SetEditMode(bIsMasterPageMode ? EM_MASTERPAGE : EM_PAGE);
}




EditMode SlideSorterModel::GetEditMode (void) const
{
    return meEditMode;
}




PageKind SlideSorterModel::GetPageType (void) const
{
    return mePageKind;
}




sal_Int32 SlideSorterModel::GetPageCount (void) const
{
    return maPageDescriptors.size();
}




SharedPageDescriptor SlideSorterModel::GetPageDescriptor (
    const sal_Int32 nPageIndex,
    const bool bCreate) const
{
    ::osl::MutexGuard aGuard (maMutex);

    SharedPageDescriptor pDescriptor;

    if (nPageIndex>=0 && nPageIndex<GetPageCount())
    {
        pDescriptor = maPageDescriptors[nPageIndex];
        if (pDescriptor == NULL && bCreate && mxSlides.is())
        {
            SdDrawDocument* pModel = const_cast<SlideSorterModel*>(this)->GetDocument();
            SdPage* pPage = NULL;
            if (pModel != NULL)
            {
                if (meEditMode == EM_PAGE)
                    pPage = pModel->GetSdPage ((USHORT)nPageIndex, mePageKind);
                else
                    pPage = pModel->GetMasterSdPage ((USHORT)nPageIndex, mePageKind);
            }
            pDescriptor.reset(new PageDescriptor (
                Reference<drawing::XDrawPage>(mxSlides->getByIndex(nPageIndex),UNO_QUERY),
                pPage,
                nPageIndex));
            maPageDescriptors[nPageIndex] = pDescriptor;
        }
    }

    return pDescriptor;
}




sal_Int32 SlideSorterModel::GetIndex (const Reference<drawing::XDrawPage>& rxSlide) const
{
    ::osl::MutexGuard aGuard (maMutex);

    // First try to guess the right index.
    Reference<beans::XPropertySet> xSet (rxSlide, UNO_QUERY);
    if (xSet.is())
    {
        try
        {
            const Any aNumber (xSet->getPropertyValue(::rtl::OUString::createFromAscii("Number")));
            sal_Int16 nNumber (-1);
            aNumber >>= nNumber;
            nNumber -= 1;
            SharedPageDescriptor pDescriptor (GetPageDescriptor(nNumber, false));
            if (pDescriptor.get() != NULL
                && pDescriptor->GetXDrawPage() == rxSlide)
            {
                return nNumber;
            }
        }
        catch (beans::UnknownPropertyException&)
        {
            OSL_ASSERT(false);
        }
        catch (lang::DisposedException&)
        {
            OSL_ASSERT(false);
        }
    }

    // Guess was wrong, iterate over all slides and search for the right
    // one.
    const sal_Int32 nCount (maPageDescriptors.size());
    for (sal_Int32 nIndex=0; nIndex<nCount; ++nIndex)
    {
        SharedPageDescriptor pDescriptor (maPageDescriptors[nIndex]);

        // Make sure that the descriptor exists.  Without it the given slide
        // can not be found.
        if (pDescriptor.get() == NULL)
        {
            // Call GetPageDescriptor() to create the missing descriptor.
            pDescriptor = GetPageDescriptor(nIndex,true);
        }

        if (pDescriptor->GetXDrawPage() == rxSlide)
            return nIndex;
    }

    return  -1;
}




sal_Int32 SlideSorterModel::GetIndex (const SdrPage* pPage) const
{
    if (pPage == NULL)
        return -1;

    ::osl::MutexGuard aGuard (maMutex);

    // First try to guess the right index.
    sal_Int16 nNumber ((pPage->GetPageNum()-1)/2);
    SharedPageDescriptor pDescriptor (GetPageDescriptor(nNumber, false));
    if (pDescriptor.get() != NULL
        && pDescriptor->GetPage() == pPage)
    {
        return nNumber;
    }

    // Guess was wrong, iterate over all slides and search for the right
    // one.
    const sal_Int32 nCount (maPageDescriptors.size());
    for (sal_Int32 nIndex=0; nIndex<nCount; ++nIndex)
    {
        pDescriptor = maPageDescriptors[nIndex];

        // Make sure that the descriptor exists.  Without it the given slide
        // can not be found.
        if (pDescriptor.get() == NULL)
        {
            // Call GetPageDescriptor() to create the missing descriptor.
            pDescriptor = GetPageDescriptor(nIndex, true);
        }

        if (pDescriptor->GetPage() == pPage)
            return nIndex;
    }

    return  -1;
}




/** For now this method uses a trivial algorithm: throw away all descriptors
    and create them anew (on demand).  The main problem that we are facing
    when designing a better algorithm is that we can not compare pointers to
    pages stored in the PageDescriptor objects and those obtained from the
    document: pages may have been deleted and others may have been created
    at the exact same memory locations.
*/
void SlideSorterModel::Resync (void)
{
    ::osl::MutexGuard aGuard (maMutex);
    ClearDescriptorList ();
    AdaptSize();
}




void SlideSorterModel::ClearDescriptorList (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Clear the cache of page descriptors.
    DescriptorContainer::iterator I;
    for (I=maPageDescriptors.begin(); I!=maPageDescriptors.end(); I++)
    {
        if (I->get() != NULL)
        {
            if ( ! I->unique())
            {
                OSL_TRACE("SlideSorterModel::ClearDescriptorList: trying to delete page descriptor  that is still used with count %d", I->use_count());
                // No assertion here because that can hang the office when
                // opening a dialog from here.
            }
            I->reset();
        }
    }
}




void SlideSorterModel::SynchronizeDocumentSelection (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    PageEnumeration aAllPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aAllPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aAllPages.GetNextElement());
        pDescriptor->GetPage()->SetSelected(pDescriptor->HasState(PageDescriptor::ST_Selected));
    }
}




void SlideSorterModel::SynchronizeModelSelection (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    PageEnumeration aAllPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aAllPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aAllPages.GetNextElement());
        pDescriptor->SetState(PageDescriptor::ST_Selected, pDescriptor->GetPage()->IsSelected());
    }
}




::osl::Mutex& SlideSorterModel::GetMutex (void)
{
    return maMutex;
}




void SlideSorterModel::SetDocumentSlides (
    const Reference<container::XIndexAccess>& rxSlides)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Reset the current page so to cause everbody to release references to it.
    mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(-1);

    mxSlides = rxSlides;
    Resync();

    ViewShell* pViewShell = mrSlideSorter.GetViewShell();
    if (pViewShell != NULL)
    {
        SdPage* pPage = pViewShell->getCurrentPage();
        if (pPage != NULL)
            mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(
                pPage);
        else
        {
            // No current page.  This can only be when the slide sorter is
            // the main view shell.  Get current slide form frame view.
            const FrameView* pFrameView = pViewShell->GetFrameView();
            if (pFrameView != NULL)
                mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(
                    pFrameView->GetSelectedPage());
            else
            {
                // No frame view.  As a last resort use the first slide as
                // current slide.
                mrSlideSorter.GetController().GetCurrentSlideManager()->NotifyCurrentSlideChange(
                    sal_Int32(0));
            }
        }
    }
}




Reference<container::XIndexAccess> SlideSorterModel::GetDocumentSlides (void) const
{
    ::osl::MutexGuard aGuard (maMutex);
    return mxSlides;
}




void SlideSorterModel::UpdatePageList (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    Reference<container::XIndexAccess> xPages;

    // Get the list of pages according to the edit mode.
    Reference<frame::XController> xController (mrSlideSorter.GetXController());
    if (xController.is())
    {
        switch (meEditMode)
        {
            case EM_MASTERPAGE:
            {
                Reference<drawing::XMasterPagesSupplier> xSupplier (
                    xController->getModel(), UNO_QUERY);
                if (xSupplier.is())
                {
                    xPages = Reference<container::XIndexAccess>(
                        xSupplier->getMasterPages(), UNO_QUERY);
                }
            }
            break;

            case EM_PAGE:
            {
                Reference<drawing::XDrawPagesSupplier> xSupplier (
                    xController->getModel(), UNO_QUERY);
                if (xSupplier.is())
                {
                    xPages = Reference<container::XIndexAccess>(
                        xSupplier->getDrawPages(), UNO_QUERY);
                }
            }
            break;

            default:
                // We should never get here.
                OSL_ASSERT(false);
                break;
        }
    }

    mrSlideSorter.GetController().SetDocumentSlides(xPages);
}




void SlideSorterModel::AdaptSize (void)
{
    if (mxSlides.is())
        maPageDescriptors.resize(mxSlides->getCount());
    else
        maPageDescriptors.resize(0);
}




bool SlideSorterModel::IsReadOnly (void) const
{
    if (mrSlideSorter.GetViewShellBase() != NULL
        && mrSlideSorter.GetViewShellBase()->GetDocShell())
        return mrSlideSorter.GetViewShellBase()->GetDocShell()->IsReadOnly();
    else
        return true;
}




void SlideSorterModel::SaveCurrentSelection (void)
{
    PageEnumeration aPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aPages.GetNextElement());
        pDescriptor->SetState(
            PageDescriptor::ST_WasSelected,
            pDescriptor->HasState(PageDescriptor::ST_Selected));
    }
}




Region SlideSorterModel::RestoreSelection (void)
{
    Region aRepaintRegion;
    PageEnumeration aPages (PageEnumerationProvider::CreateAllPagesEnumeration(*this));
    while (aPages.HasMoreElements())
    {
        SharedPageDescriptor pDescriptor (aPages.GetNextElement());
        if (pDescriptor->SetState(
            PageDescriptor::ST_Selected,
            pDescriptor->HasState(PageDescriptor::ST_WasSelected)))
        {
            aRepaintRegion.Union(pDescriptor->GetBoundingBox());
        }
    }
    return aRepaintRegion;
}


} } } // end of namespace ::sd::slidesorter::model
