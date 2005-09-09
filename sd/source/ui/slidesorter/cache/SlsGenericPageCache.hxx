/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsGenericPageCache.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:10:13 $
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
#ifndef SD_SLIDESORTER_GENERIC_PAGE_CACHE_HXX
#define SD_SLIDESORTER_GENERIC_PAGE_CACHE_HXX

#include "SlsQueueProcessor.hxx"
#include "view/SlsPageObjectViewObjectContact.hxx"


namespace sd { namespace slidesorter { namespace model {
class SlideSorterModel;
} } }

namespace sd { namespace slidesorter { namespace view {
class SlideSorterView;
} } }

namespace sd { namespace slidesorter { namespace cache {


template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CacheCompactionPolicy,
         class QueueProcessor>
class GenericPageCache
{
public:
    /** The page chache is created with references both to the view and the
        model so that it can fill itself with requests for all or just the
        visible pages.
    */
    GenericPageCache (
        view::SlideSorterView& rView,
        model::SlideSorterModel& rModel,
        sal_Int32 nMaximalCacheSize);

    ~GenericPageCache (void);

    /** Request a preview bitmap for the specified page object in the
        specified size.  The returned bitmap may be preview of the preview,
        i.e. either a scaled (up or down) version of a previous preview (of
        the wrong size) or an empty bitmap.  In this case a request for the
        generation of a new preview is created and inserted into the request
        queue.  When the preview is available the page shape will be told to
        paint itself again.  When it then calls this method again if
        receives the correctly sized preview bitmap.
        @param rRequestData
            This data is used to determine the preview.
        @param rSize
            The size of the requested preview bitmap.
        @return
            Returns a bitmap that is either empty, contains a scaled (up or
            down) version or is the requested bitmap.
    */
    BitmapEx GetPreviewBitmap (
        RequestData& rRequestData,
        const Size& rSize);

    /** When the requested preview bitmap does not yet exist or is not
        up-to-date then the rendering of one is scheduled.  Otherwise this
        method does nothing.
        @param rRequestData
            This data is used to determine the preview.
        @param rSize
            The size of the requested preview bitmap in pixel coordinates.
        @param bMayBeUpToDate
            This flag helps the method to determine whether an existing
            preview that matches the request is up to date.  If the caller
            know that it is not then by passing <FALSE/> he tells us that we
            do not have to check the up-to-date flag a second time.  If
            unsure pass <TRUE/>.
    */
    void RequestPreviewBitmap (
        RequestData& rRequestData,
        const Size& rSize,
        bool bMayBeUpToDate = true);

    /** Tell the cache to replace the bitmap associated with the given
        request data with a new one that reflects recent changes in the
        content of the page object.
    */
    void InvalidatePreviewBitmap (const RequestData& rRequestData);

    /** Lower the priority with which the request associated with the given
        data will be processed.  Call this method when the visibility of a
        page object changes (from visible to not visible) and the request
        becomes a ahead-of-time request. When the request is already in the
        lowest class it will be removed.
    */
    void DecreaseRequestPriority (RequestData& rRequestData);

    /** Move the request associated with the given data into a higher
        priority class and increase its priority in that class above all
        other elements in the class.
    */
    void IncreaseRequestPriority (RequestData& rRequestData);

    /** Call this method when a view-object-contact object is being deleted
        and does not need (a) its current bitmap in the cache and (b) a
        requested new bitmap.
    */
    void ReleasePreviewBitmap (RequestData& rRequestData);

    /** Call this method when all preview bitmaps have to be generated anew.
        This is the case when the size of the page objects on the screen has
        changed or when the model has changed.
    */
    void InvalidateCache (void);

    /** With the precious flag you can control whether a bitmap can be
        removed or reduced in size to make room for other bitmaps or is so
        precious that it will not be touched.  A typical use is to set the
        precious flag for exactly the visible pages.
    */
    void SetPreciousFlag (RequestData& rRequestData, bool bIsPrecious);

private:
    view::SlideSorterView& mrView;

    model::SlideSorterModel& mrModel;

    BitmapCache maBitmapCache;

    RequestQueue maRequestQueue;

    QueueProcessor* mpQueueProcessor;

    const sal_Int32 mnMaximalCacheSize;

    /** Remember whether the cache limit has been reached at least once
        after a Clear() call.  This is important because afterwards the
        cache will be constantly at its limit of capacity.  Therefore
        requests with another than the highest priority class will not be
        processed since the resulting preview bitmaps would be removed
        shortly afterwards.
    */
    bool mbLimitHasBeenReached;
};




//=====  GenericPageCache =====================================================

template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
         class QueueProcessor>
GenericPageCache<
    RequestData,
    CreationManager,
    BitmapCache,
    RequestQueue,
    CompactionPolicy, QueueProcessor
    >::GenericPageCache (
        view::SlideSorterView& rView,
        model::SlideSorterModel& rModel,
        sal_Int32 nMaximalCacheSize)
        : mrView(rView),
          mrModel(rModel),
          maBitmapCache (),
          maRequestQueue(),
          mpQueueProcessor(
              new QueueProcessor(mrView,maRequestQueue,maBitmapCache)),
          mnMaximalCacheSize(nMaximalCacheSize),
          mbLimitHasBeenReached (false)
{
}




template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
         class QueueProcessor>
GenericPageCache<
    RequestData, CreationManager, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::~GenericPageCache (void)
{
    OSL_TRACE("terminating thread %p", mpQueueProcessor);
    mpQueueProcessor->Stop();
    maRequestQueue.Clear();
    mpQueueProcessor->Terminate();
    //    delete mpQueueProcessor;
    OSL_TRACE("thread %p stopped and terminated", mpQueueProcessor);
}




template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
         class QueueProcessor>
BitmapEx GenericPageCache<
    RequestData, CreationManager, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::GetPreviewBitmap (
        RequestData& rRequestData,
        const Size& rSize)
{
    BitmapEx aPreview;
    bool bMayBeUpToDate = true;
    if (maBitmapCache.HasBitmap (rRequestData.GetPage()))
    {
        aPreview = maBitmapCache.GetBitmap (rRequestData.GetPage());
        Size aBitmapSize (aPreview.GetSizePixel());
        if (aBitmapSize != rSize)
        {
            // The bitmap has the wrong size.
            DBG_ASSERT (rSize.Width() < 1000,
                "GenericPageCache<>::GetPreviewBitmap(): bitmap requested with large width.  This may indicate an error.");

            // Scale the bitmap to the desired size when that is possible,
            // i.e. the bitmap is not empty.
            if (aBitmapSize.Width()>0 && aBitmapSize.Height()>0)
                aPreview.Scale (rSize, BMP_SCALE_FAST);
        }
        bMayBeUpToDate = true;
    }
    else
        bMayBeUpToDate = false;

    // Request the creation of a correctly sized preview bitmap.  We do this
    // even when the size of the bitmap in the cache is correct because its
    // content may be not up-to-date anymore.
    RequestPreviewBitmap (rRequestData, rSize, bMayBeUpToDate);

    return aPreview;
}




template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
         class QueueProcessor>
void GenericPageCache<
    RequestData, CreationManager, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::RequestPreviewBitmap (
        RequestData& rRequestData,
        const Size& rSize,
        bool bMayBeUpToDate)
{
    const SdrPage* pPage = rRequestData.GetPage();

    // Determine if the available bitmap is up to date.
    bool bIsUpToDate = false;
    if (bMayBeUpToDate)
        bIsUpToDate = maBitmapCache.BitmapIsUpToDate (pPage);
    if (bIsUpToDate)
    {
        BitmapEx aPreview (maBitmapCache.GetBitmap (pPage));
        if (aPreview.GetSizePixel() != rSize)
            bIsUpToDate = false;
    }

    if ( ! bIsUpToDate)
    {
        // No, the bitmap is not up-to-date.  Request a new one.
        int nPriorityClass = rRequestData.GetPageDescriptor().IsVisible() ? 0 : 1;
        maRequestQueue.AddRequest (rRequestData, nPriorityClass);
        mpQueueProcessor->Start (nPriorityClass);
    }

    // Reduce the cache size if it grew too large.
    if (maBitmapCache.GetSize() > mnMaximalCacheSize)
    {
        mbLimitHasBeenReached = true;
        CompactionPolicy()(maBitmapCache, mnMaximalCacheSize);
    }
}




template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
        class QueueProcessor>
void GenericPageCache<
    RequestData, CreationManager, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::InvalidatePreviewBitmap (const RequestData& rRequestData)
{
    maBitmapCache.InvalidateBitmap (rRequestData.GetPage());
}




template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
        class QueueProcessor>
void GenericPageCache<
    RequestData, CreationManager, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::DecreaseRequestPriority (RequestData& rRequestData)
{
    if (mbLimitHasBeenReached)
        maRequestQueue.RemoveRequest (rRequestData);
    else
        maRequestQueue.ChangePriorityClass (rRequestData,+1);
}




template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
        class QueueProcessor>
void GenericPageCache<
    RequestData, CreationManager, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::IncreaseRequestPriority (RequestData& rRequestData)
{
    maRequestQueue.ChangePriorityClass (rRequestData,-1);
}




template<class RequestData,
         class CreationManager,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
        class QueueProcessor>
void GenericPageCache<
    RequestData, CreationManager, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::ReleasePreviewBitmap (RequestData& rRequestData)
{
    mpQueueProcessor->RemoveRequest (rRequestData);
    maRequestQueue.RemoveRequest (rRequestData);

    // We do not relase the preview bitmap that is associated with the page
    // of the given request data because this method is called when the
    // request data, typically a view-object-contact object, is destroyed.
    // The page object usually lives longer than that and thus the preview
    // bitmap may be used later on.
    //    maBitmapCache.ReleaseBitmap (rRequestData);
}




template<class RequestData,
         class RequestFactory,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
        class QueueProcessor>
void GenericPageCache<
    RequestData, RequestFactory, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::InvalidateCache (void)
{
    // 1.) Stop the timer while the queues are being updated.
    mpQueueProcessor->Stop();

    // 2.) Clear the request queue of their current content.
    maRequestQueue.Clear();
    mbLimitHasBeenReached = false;

    // 3.) Create the new requests for filling the cache with at least the
    // visible previews.
    RequestFactory()(mrModel, mrView,maRequestQueue);

    // 4.) Start the timer again.
    mpQueueProcessor->Start();
}




template<class RequestData,
         class RequestFactory,
         class BitmapCache,
         class RequestQueue,
         class CompactionPolicy,
        class QueueProcessor>
void GenericPageCache<
    RequestData, RequestFactory, BitmapCache, RequestQueue,
    CompactionPolicy, QueueProcessor
    >::SetPreciousFlag (RequestData& rRequestData, bool bIsPrecious)
{
    maBitmapCache.SetPrecious (rRequestData.GetPage(), bIsPrecious);
}



} } } // end of namespace ::sd::slidesorter::cache

#endif
