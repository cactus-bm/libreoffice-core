/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: viewcontactofpageobj.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:59:23 $
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

#ifndef _SDR_CONTACT_VIEWCONTACTOFPAGEOBJ_HXX
#define _SDR_CONTACT_VIEWCONTACTOFPAGEOBJ_HXX

#ifndef _SDR_CONTACT_VIEWCONTACTOFSDROBJ_HXX
#include <svx/sdr/contact/viewcontactofsdrobj.hxx>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// predeclarations

class SdrPageObj;
class SdrPage;

namespace sdr
{
    namespace contact
    {
        class OCOfPageObjPagePainter;
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        class SVX_DLLPUBLIC ViewContactOfPageObj : public ViewContactOfSdrObj
        {
            // The painter for the page. As long as the page does not change,
            // the incarnation can be reused.
            OCOfPageObjPagePainter*                     mpPagePainter;

            // bitfield
            // Flag to avoid recursive displaying of page objects, e.g.
            // when the page object itself shows a page which displays
            // page objects.
            unsigned                                    mbIsPainting : 1;

            // #i35972# flag to avoid recursive ActionChange events
            unsigned                                    mbIsInActionChange : 1;

        protected:
            // internal access to SdrObject
            SdrPageObj& GetPageObj() const
            {
                return (SdrPageObj&)GetSdrObject();
            }

            // Access to referenced page
            const SdrPage* GetReferencedPage() const;

            // method to recalculate the PaintRectangle if the validity flag shows that
            // it is invalid. The flag is set from GetPaintRectangle, thus the implementation
            // only needs to refresh maPaintRectangle itself.
            virtual void CalcPaintRectangle();

            /** Return the rectangle that specifies where and how large the
                page will be painted.  This rectangle will usually be
                identical to the one returned by GetPaintRectangle().
                The returned rectangle has to lie completly inside the
                rectangle returned by GetPaintRectangle().  Making it
                smaller results in a border arround the page rectangle.

                Note: This method may calculate and store internally the
                requested rectangle and thus can not be const.
            */
            virtual Rectangle GetPageRectangle (void);

            // get rid of evtl. remembered PagePainter
            void GetRidOfPagePainter();

            // Prepare a PagePainter for current referenced page. This may
            // refresh, create or delete a PagePainter instance in
            // mpPagePainter
            void PreparePagePainter(const SdrPage* pPage);

            /** Paint support methods for page content painting
                @param rPaintRectangle
                    The painting of the page content will be transformed so
                    that it fills exactly this rectangle.  Usually this will
                    be the paint rectangle.  Making the content rectangle
                    smaller will result in a border between the outer paint
                    rectangle (the bounding box) and the page content
                    rectangle.
            */
            sal_Bool PaintPageContents(
                DisplayInfo& rDisplayInfo,
                const Rectangle& rPaintRectangle,
                const ViewObjectContact& rAssociatedVOC);
            sal_Bool PaintPageReplacement(
                DisplayInfo& rDisplayInfo,
                const Rectangle& rPaintRectangle,
                const ViewObjectContact& rAssociatedVOC);
            sal_Bool PaintPageBorder(
                DisplayInfo& rDisplayInfo,
                const Rectangle& rPaintRectangle,
                const ViewObjectContact& rAssociatedVOC);

            // On StopGettingViewed the PagePainter can be dismissed.
            virtual void StopGettingViewed();

        public:
            // basic constructor, used from SdrObject.
            ViewContactOfPageObj(SdrPageObj& rPageObj);

            // The destructor. When PrepareDelete() was not called before (see there)
            // warnings will be generated in debug version if there are still contacts
            // existing.
            virtual ~ViewContactOfPageObj();

            // Paint this object. This is before evtl. SubObjects get painted. It needs to return
            // sal_True when something was pained and the paint output rectangle in rPaintRectangle.
            virtual sal_Bool PaintObject(
                DisplayInfo& rDisplayInfo, Rectangle& rPaintRectangle,
                const ViewObjectContact& rAssociatedVOC);

            // #WIP# React on changes of the object of this ViewContact
            virtual void ActionChanged();
        };
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_CONTACT_VIEWCONTACTOFPAGEOBJ_HXX

// eof
