/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SlsQueueProcessor.cxx,v $
 * $Revision: 1.10 $
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

#include "SlsQueueProcessor.hxx"
#include "SlsCacheConfiguration.hxx"
#include "SlsRequestQueue.hxx"
#include "SlsIdleDetector.hxx"

namespace sd { namespace slidesorter { namespace cache {


//=====  QueueProcessor  ======================================================

QueueProcessor::QueueProcessor (
    RequestQueue& rQueue,
    const ::boost::shared_ptr<BitmapCache>& rpCache,
    const Size& rPreviewSize,
    const SharedCacheContext& rpCacheContext)
    : maMutex(),
      maTimer(),
      mnTimeBetweenHighPriorityRequests (10/*ms*/),
      mnTimeBetweenLowPriorityRequests (100/*ms*/),
      mnTimeBetweenRequestsWhenNotIdle (1000/*ms*/),
      maPreviewSize(rPreviewSize),
      mpCacheContext(rpCacheContext),
      mrQueue(rQueue),
      mpCache(rpCache),
      maBitmapFactory(),
      mbIsPaused(false)
{
    // Look into the configuration if there for overriding values.
    ::com::sun::star::uno::Any aTimeBetweenReqeusts;
    aTimeBetweenReqeusts = CacheConfiguration::Instance()->GetValue(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TimeBetweenHighPriorityRequests")));
    if (aTimeBetweenReqeusts.has<sal_Int32>())
        aTimeBetweenReqeusts >>= mnTimeBetweenHighPriorityRequests;

    aTimeBetweenReqeusts = CacheConfiguration::Instance()->GetValue(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TimeBetweenLowPriorityRequests")));
    if (aTimeBetweenReqeusts.has<sal_Int32>())
        aTimeBetweenReqeusts >>= mnTimeBetweenLowPriorityRequests;

    aTimeBetweenReqeusts = CacheConfiguration::Instance()->GetValue(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TimeBetweenRequestsDuringShow")));
    if (aTimeBetweenReqeusts.has<sal_Int32>())
        aTimeBetweenReqeusts >>= mnTimeBetweenRequestsWhenNotIdle;

    maTimer.SetTimeoutHdl (LINK(this,QueueProcessor,ProcessRequestHdl));
    maTimer.SetTimeout (mnTimeBetweenHighPriorityRequests);
}





QueueProcessor::~QueueProcessor (void)
{
}




void QueueProcessor::Start (int nPriorityClass)
{
    if (mbIsPaused)
        return;
    if ( ! maTimer.IsActive())
    {
        if (nPriorityClass == 0)
            maTimer.SetTimeout (mnTimeBetweenHighPriorityRequests);
        else
            maTimer.SetTimeout (mnTimeBetweenLowPriorityRequests);
        maTimer.Start();
    }
}




void QueueProcessor::Stop (void)
{
    if (maTimer.IsActive())
        maTimer.Stop();
}




void QueueProcessor::Pause (void)
{
    mbIsPaused = true;
}




void QueueProcessor::Resume (void)
{
    mbIsPaused = false;
    if ( ! mrQueue.IsEmpty())
        Start(mrQueue.GetFrontPriorityClass());
}




void QueueProcessor::Terminate (void)
{
}




void QueueProcessor::SetPreviewSize (const Size& rPreviewSize)
{
    maPreviewSize = rPreviewSize;
}




IMPL_LINK(QueueProcessor, ProcessRequestHdl, Timer*, EMPTYARG)
{
    ProcessRequests();
    return 1;
}




void QueueProcessor::ProcessRequests (void)
{
    OSL_ASSERT(mpCacheContext.get()!=NULL);

    while ( ! mrQueue.IsEmpty() && ! mbIsPaused)
    {
        if ( ! mpCacheContext->IsIdle())
            break;

        CacheKey aKey = NULL;
        RequestPriorityClass ePriorityClass (NOT_VISIBLE);
        {
            ::osl::MutexGuard aGuard (mrQueue.GetMutex());

            if ( ! mrQueue.IsEmpty())
            {
                // Get the request with the highest priority from the queue.
                ePriorityClass = mrQueue.GetFrontPriorityClass();
                aKey = mrQueue.GetFront();
                mrQueue.PopFront();
            }
        }

        if (aKey != NULL)
            ProcessOneRequest(aKey, ePriorityClass);

        // Requests of lower priority are processed one at a time.
        {
            ::osl::MutexGuard aGuard (mrQueue.GetMutex());
            if ( ! mrQueue.IsEmpty())
                if (mrQueue.GetFrontPriorityClass() > 0)
                    break;
        }
    }

    // Schedule the processing of the next element(s).
    {
        ::osl::MutexGuard aGuard (mrQueue.GetMutex());
        if ( ! mrQueue.IsEmpty())
            /*
            if (bIsShowingFullScreenShow)
                Start(mnTimeBetweenRequestsWhenNotIdle);
            else
            */
            Start(mrQueue.GetFrontPriorityClass());
    }
}




void QueueProcessor::ProcessOneRequest (
    CacheKey aKey,
    const RequestPriorityClass ePriorityClass)
{
    const SdrPage* pPage = mpCacheContext->GetPage(aKey);
    SSCD_SET_STATUS(pPage,RENDERING);

#ifdef VERBOSE
    OSL_TRACE ("processing request for page %d with priority class %d",
        (pPage->GetPageNum()-1)/2,
        ePriorityClass);
#endif
    try
    {
        ::osl::MutexGuard aGuard (maMutex);
        // Create a new preview bitmap and store it in the cache.
        if (mpCache.get() != NULL)
        {
            const SdPage* pSdPage = dynamic_cast<const SdPage*>(pPage);
            if (pSdPage != NULL)
            {
                const ::boost::shared_ptr<BitmapEx> pPreview (
                    maBitmapFactory.CreateBitmap(*pSdPage, maPreviewSize));
                mpCache->SetBitmap (
                    pPage,
                    pPreview,
                    ePriorityClass!=NOT_VISIBLE);

                // Initiate a repaint of the new preview.
                mpCacheContext->NotifyPreviewCreation(aKey, pPreview);

                SSCD_SET_STATUS(pPage,NONE);
            }
        }
    }
    catch (::com::sun::star::uno::RuntimeException aException)
    {
        OSL_ASSERT("RuntimeException caught in QueueProcessor");
        (void) aException;
    }
    catch (::com::sun::star::uno::Exception aException)
    {
        OSL_ASSERT("Exception caught in QueueProcessor");
        (void) aException;
    }
}




void QueueProcessor::RemoveRequest (CacheKey aKey)
{
    (void)aKey;
    // See the method declaration above for an explanation why this makes sense.
    ::osl::MutexGuard aGuard (maMutex);
}




void QueueProcessor::SetBitmapCache (
    const ::boost::shared_ptr<BitmapCache>& rpCache)
{
    mpCache = rpCache;
}


} } } // end of namespace ::sd::slidesorter::cache
