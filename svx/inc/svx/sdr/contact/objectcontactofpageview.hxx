/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: objectcontactofpageview.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:57:39 $
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

#ifndef _SDR_CONTACT_OBJECTCONTACTOFPAGEVIEW_HXX
#define _SDR_CONTACT_OBJECTCONTACTOFPAGEVIEW_HXX

#ifndef _SDR_CONTACT_OBJECTCONTACT_HXX
#include <svx/sdr/contact/objectcontact.hxx>
#endif

#ifndef _SV_VIRDEV_HXX
#include <vcl/virdev.hxx>
#endif

#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////
// predeclarations

class SdrPageViewWindow;
class SdrPage;

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        class ObjectContactOfPageView : public ObjectContact
        {
        protected:
            // the owner of this ObjectContactOfPageView. Set from constructor and not
            // to be changed in any way.
            SdrPageViewWindow&                              mrPageViewWindow;

            // The last remembered StartPoint of the hierarchy
            SdrPage*                                        mpRememberedStartPage;

            // The VirtualDevice for PreRendering
            VirtualDevice                                   maPreRenderDevice;

            // Create and set the ExpandPaintClipRegion. This needs to be done before
            // any of the objects gets really painted because it relies on the invalidated
            // and not painted state of the single objects.
            void ExpandPaintClipRegion(DisplayInfo& rDisplayInfo);

            // Process the whole displaying, the real version
            void DoProcessDisplay(DisplayInfo& rDisplayInfo);

            // Decide if to PreRender
            sal_Bool DoPreRender(DisplayInfo& rDisplayInfo) const;

            // The PreRenderer itself which creates the PreRenderedBitmap
            // using the PreRenderDevice.
            void PreRender(DisplayInfo& rDisplayInfo);

            // Update Draw Hierarchy data
            virtual void EnsureValidDrawHierarchy(DisplayInfo& rDisplayInfo);

        public:
            // basic constructor, used from SdrPageViewWindow.
            ObjectContactOfPageView(SdrPageViewWindow& rPageViewWindow);

            // The destructor. When PrepareDelete() was not called before (see there)
            // warnings will be generated in debug version if there are still contacts
            // existing.
            virtual ~ObjectContactOfPageView();

            // A ViewObjectContact was deleted and shall be forgotten.
            // #i29181# Overload to clear selection at associated view
            virtual void RemoveViewObjectContact(ViewObjectContact& rVOContact);

            // Pre-Process the whole displaying. The default implementation
            // calls EnsureValidDrawHierarchy() to ensure a valid draw hierarchy.
            virtual void PreProcessDisplay(DisplayInfo& rDisplayInfo);

            // Process the whole displaying
            virtual void ProcessDisplay(DisplayInfo& rDisplayInfo);

            // test if visualizing of entered groups is switched on at all
            virtual sal_Bool DoVisualizeEnteredGroup() const;

            // Get the active group (the entered group). To get independent
            // from the old object/view classes return values use the new
            // classes.
            virtual ViewContact* GetActiveGroupContact() const;

            // Invalidate given rectangle at the window/output which is represented by
            // this ObjectContact.
            virtual void InvalidatePartOfView(const Rectangle& rRectangle) const;

            // #i37394# Non-painted object was changed. Test for potentially
            // getting visible
            virtual void ObjectGettingPotentiallyVisible(const ViewObjectContact& rVOC) const;

            // #i42815#
            // Get info if given Rectangle is visible in this view
            virtual sal_Bool IsAreaVisible(const Rectangle& rRectangle) const;

            // Get info about the need to visualize GluePoints. The default
            // is that it is not necessary.
            virtual sal_Bool AreGluePointsVisible() const;

            // check if text animation is allowed.
            virtual sal_Bool IsTextAnimationAllowed() const;

            // check if graphic animation is allowed.
            virtual sal_Bool IsGraphicAnimationAllowed() const;

            // check if asynchronious graphis loading is allowed. Default is sal_False.
            virtual sal_Bool IsAsynchronGraphicsLoadingAllowed() const;

            // check if buffering of MasterPages is allowed. Default is sal_False.
            virtual sal_Bool IsMasterPageBufferingAllowed() const;

        public:
                    // internal access to SdrPageViewWindow
            SdrPageViewWindow& GetPageViewWindow() const;

            // internal access to SdrPage of PageView
            SdrPage* GetSdrPage() const;
        };
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_CONTACT_OBJECTCONTACTOFPAGEVIEW_HXX

// eof
