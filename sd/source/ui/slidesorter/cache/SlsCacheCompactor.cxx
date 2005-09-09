/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsCacheCompactor.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:09:41 $
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

#include "SlsCacheCompactor.hxx"

#include "SlsBitmapCache.hxx"
#include <svx/svdpage.hxx>
#include <set>

using namespace std;

namespace sd { namespace slidesorter { namespace cache {

/** Compare elements of the bitmap cache according to their last access
    time.
*/
class AccessTimeComparator
{
public:
    bool operator () (
        const CacheBitmapContainer::value_type& e1,
        const CacheBitmapContainer::value_type& e2)
    {
        return e1.second.mnLastAccessTime < e2.second.mnLastAccessTime;
    }
};

typedef set<CacheBitmapContainer::value_type, AccessTimeComparator>
    SortedCache;


//=====  NoCompaction  ========================================================

void NoCompaction::operator() (BitmapCache& rCache, sal_Int32 nMaximalSize)
{
}




//=====  CompactionByRemoval ==================================================

void CompactionByRemoval::operator() (
    BitmapCache& rCache,
    sal_Int32 nMaximalSize)
{
    if (rCache.GetSize()  > nMaximalSize)
    {
        OSL_TRACE ("bitmap cache uses to much space: %d > %d",
            rCache.GetSize(), nMaximalSize);

        // Sort the entries in the cache by creating a new container.
        SortedCache aSortedCache;
        copy (
            rCache.GetContainer().begin(),
            rCache.GetContainer().end(),
            inserter(aSortedCache, aSortedCache.end()));

        // Free one bitmap at a time until there is enough free space.  The
        // number of bitmaps in the cache is used as a guard against
        // infinite loops when the maximal cache size is too small of the
        // bitmaps are too large.
        while (rCache.GetSize() > nMaximalSize && ! aSortedCache.empty())
        {
            CacheBitmapContainer::value_type aEntry (*aSortedCache.begin());
            if ( ! aEntry.second.mbIsPrecious)
            {
                // Free the bitmap and thus increase the available space.
                rCache.ReleaseBitmap (aEntry.first);

                OSL_TRACE ("    there are now %d bytes occupied after releasing the bitmap for page %d",
                    rCache.GetSize(), aEntry.first->GetPageNum());
            }

            aSortedCache.erase (aSortedCache.begin());
        }
    }
}




//=====  CompactionByReduction  ===============================================

void CompactionByReduction::operator() (
    BitmapCache& rCache,
    sal_Int32 nMaximalSize)
{
    if (rCache.GetSize()  > nMaximalSize)
    {
        OSL_TRACE ("bitmap cache uses to much space: %d > %d",
            rCache.GetSize(), nMaximalSize);

        // Sort the entries in the cache by creating a new container.
        SortedCache aSortedCache;
        copy (
            rCache.GetContainer().begin(),
            rCache.GetContainer().end(),
            inserter(aSortedCache, aSortedCache.end()));

        // Free one bitmap at a time until there is enough free space.  The
        // number of bitmaps in the cache is used as a guard against
        // infinite loops when the maximal cache size is too small or the
        // bitmaps are too large.
        while (rCache.GetSize() > nMaximalSize && ! aSortedCache.empty())
        {
            CacheBitmapContainer::value_type aEntry (*aSortedCache.begin());
            if ( ! aEntry.second.mbIsPrecious)
            {
                Size aSize (aEntry.second.maBitmap.GetSizePixel());
                if (aSize.Width() != 0)
                {
                    int nWidth = 100;
                    int nHeight = aSize.Height() * nWidth / aSize.Width() ;
                    aEntry.second.maBitmap.Scale (Size(nWidth,nHeight));
                    rCache.SetBitmap (
                        aEntry.first,
                        aEntry.second.maBitmap,
                        false);
                }

                OSL_TRACE ("    there are now %d bytes occupied after scaling down bitmap for page %d",
                    rCache.GetSize(), aEntry.first->GetPageNum());
            }
            aSortedCache.erase (aSortedCache.begin());
        }
    }
}




} } } // end of namespace ::sd::slidesorter::cache
