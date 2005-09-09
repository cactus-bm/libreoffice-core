/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsRequestFactory.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:12:37 $
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

#ifndef SD_SLIDESORTER_REQUEST_FACTORY_HXX
#define SD_SLIDESORTER_REQUEST_FACTORY_HXX

#include "model/SlideSorterModel.hxx"
#include "model/SlsPageDescriptor.hxx"
#include "view/SlideSorterView.hxx"
#include <svx/svdpagv.hxx>

#include <svx/sdr/contact/viewcontact.hxx>

namespace sd { namespace slidesorter { namespace view {
class PageObjectViewObjectContact;
} } }

namespace sd { namespace slidesorter { namespace cache {

template <class Queue, bool UseAheadOfTimeRequests>
class RequestFactory
{
public:
    void operator() (
        const model::SlideSorterModel& rModel,
        const view::SlideSorterView& rView,
        Queue& mrQueue);
};




//=============================================================================
// Implementation

//=====  RequestFactory  ======================================================

template <class Queue,bool UseAheadOfTimeRequests>
    void RequestFactory<Queue,UseAheadOfTimeRequests>::operator() (
        const model::SlideSorterModel& rModel,
        const view::SlideSorterView& rView,
        Queue& rQueue)
{
    // Fill the queues with the new content.  Visible page objects go into
    // the regular queue, non-visible page objects go into the ahead-of-time
    // queue.
    SdrPageView* pPageView = rView.GetPageViewPvNum(0);
    SdrPageViewWindow* pPageViewWindow = NULL;
    if (pPageView != NULL)
        pPageViewWindow = pPageView->GetWindow(0);
    if (pPageViewWindow != NULL)
    {
        ::sdr::contact::ObjectContact& rObjectContact (
            rView.GetPageViewPvNum(0)->GetWindow(0)->GetObjectContact());

        // Add the requests for the visible pages.
        model::SlideSorterModel::Enumeration aPageEnumeration (
            rModel.GetVisiblePagesEnumeration());
        while (aPageEnumeration.HasMoreElements())
        {
            model::PageDescriptor& rDescriptor (
                aPageEnumeration.GetNextElement());
            view::PageObjectViewObjectContact* pRequest (
                rDescriptor.GetViewObjectContact());
            if (pRequest != NULL)
                rQueue.InsertFrontRequest (*pRequest, 0);
        }

        if (UseAheadOfTimeRequests)
        {
            // Add the requests for the non-visible pages.
            aPageEnumeration = rModel.GetAllPagesEnumeration();
            while (aPageEnumeration.HasMoreElements())
            {
                model::PageDescriptor& rDescriptor (
                    aPageEnumeration.GetNextElement());
                view::PageObjectViewObjectContact* pRequest =
                    static_cast<view::PageObjectViewObjectContact*>(
                        &rDescriptor.GetPageObject()->GetViewContact()
                        .GetViewObjectContact(rObjectContact));
                if ( ! rDescriptor.IsVisible())
                    rQueue.InsertFrontRequest (*pRequest, 1);
            }
        }
    }
}



} } } // end of namespace ::sd::slidesorter::cache


#endif
