/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: slide.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 15:15:07 $
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

#ifndef INCLUDED_SLIDESHOW_SLIDE_HXX
#define INCLUDED_SLIDESHOW_SLIDE_HXX

#include "shapemanager.hxx"
#include "subsettableshapemanager.hxx"
#include "unoviewcontainer.hxx"
#include "slidebitmap.hxx"
#include "shapemaps.hxx"

#include <boost/shared_ptr.hpp>

namespace com {  namespace sun { namespace star {
    namespace drawing {
        class XDrawPage;
    }
    namespace uno {
        class XComponentContext;
    }
    namespace animations {
        class XAnimationNode;
} } } }

namespace basegfx
{
    class B2IVector;
}

/* Definition of Slide interface */

namespace slideshow
{
    namespace internal
    {
        class RGBColor;
        class ScreenUpdater;

        class Slide
        {
        public:
            // Showing
            // -------------------------------------------------------------------

            /** Prepares to show slide.

                Call this method to reduce the timeout show(), and
                getInitialSlideBitmap() need to complete. If
                prefetch() is not called explicitely, the named
                methods will call it implicitely.
             */
            virtual bool prefetch() = 0;

            /** Shows the slide on all registered views

                After this call, the slide will render itself to the
                views, and start its animations.

                @param bSlideBackgoundPainted
                When true, the initial slide content on the background
                layer is already rendered (e.g. from a previous slide
                transition). When false, Slide renders initial content of
                slide.
             */
            virtual bool show( bool bSlideBackgoundPainted ) = 0;

            /** Force-ends the slide

                After this call, the slide has stopped all animations,
                and ceased rendering/visualization on all views.
             */
            virtual void hide() = 0;


            // Queries
            // -------------------------------------------------------------------

            /** Query the size of this slide in user coordinates

                This value is retrieved from the XDrawPage properties.
             */
            virtual basegfx::B2IVector getSlideSize() const = 0;

            /// Gets the underlying API page
            virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XDrawPage > getXDrawPage() const = 0;

            /// Gets the animation node.
            virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::animations::XAnimationNode > getXAnimationNode() const = 0;


            // Slide bitmaps
            // -------------------------------------------------------------------

            /** Request bitmap for current slide appearance.

                The bitmap returned by this method depends on the
                current state of the slide and the contained
                animations. A newly generated slide will return the
                initial slide content here (e.g. with all 'appear'
                effect shapes invisible), a slide whose effects are
                currently running will return a bitmap corresponding
                to the current position on the animation timeline, and
                a slide whose effects have all been run will generate
                a bitmap with the final slide appearance (e.g. with
                all 'hide' effect shapes invisible).

                @param rView
                View to retrieve bitmap for (note that the bitmap will
                have device-pixel equivalence to the content that
                would have been rendered onto the given view). Note
                that the view must have been added to this slide
                before via viewAdded().
             */
            virtual SlideBitmapSharedPtr
                getCurrentSlideBitmap( const UnoViewSharedPtr& rView ) const = 0;
        };

        typedef ::boost::shared_ptr< Slide > SlideSharedPtr;

        class EventQueue;
        class CursorManager;
        class EventMultiplexer;
        class ActivitiesQueue;
        class UserEventQueue;
        class RGBColor;

        /** Construct from XDrawPage

            The Slide object generally works in XDrawPage model
            coordinates, that is, the page will have the width and
            height as specified in the XDrawPage's property
            set. The top, left corner of the page will be rendered
            at (0,0) in the given canvas' view coordinate system.

            Does not render anything initially

            @param xDrawPage
            Page to display on this slide

            @param xRootNode
            Root of the SMIL animation tree. Used to animate the slide.

            @param rEventQueue
            EventQueue. Used to post events.

            @param rActivitiesQueue
            ActivitiesQueue. Used to run animations.

            @param rEventMultiplexer
            Event source

            @param rUserEventQueue
            UserEeventQueue
        */
        SlideSharedPtr createSlide( const ::com::sun::star::uno::Reference<
                                       ::com::sun::star::drawing::XDrawPage >&         xDrawPage,
                                    const ::com::sun::star::uno::Reference<
                                       ::com::sun::star::animations::XAnimationNode >& xRootNode,
                                    EventQueue&                                        rEventQueue,
                                    EventMultiplexer&                                  rEventMultiplexer,
                                    ScreenUpdater&                                     rScreenUpdater,
                                    ActivitiesQueue&                                   rActivitiesQueue,
                                    UserEventQueue&                                    rUserEventQueue,
                                    CursorManager&                                     rCursorManager,
                                    const UnoViewContainer&                            rViewContainer,
                                    const ::com::sun::star::uno::Reference<
                                       ::com::sun::star::uno::XComponentContext >&     xContext,
                                    const ShapeEventListenerMap&                       rShapeListenerMap,
                                    const ShapeCursorMap&                              rShapeCursorMap,
                                    RGBColor const&                                    aUserPaintColor,
                                    bool                                               bUserPaintEnabled,
                                    bool                                               bIntrinsicAnimationsAllowed,
                                    bool                                               bDisableAnimationZOrder );
    }
}

#endif /* INCLUDED_SLIDESHOW_SLIDE_HXX */
