/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: externalshapebase.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 14:53:00 $
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

#ifndef INCLUDED_SLIDESHOW_EXTERNALSHAPEBASE_HXX
#define INCLUDED_SLIDESHOW_EXTERNALSHAPEBASE_HXX

#include <vector>

#include "animatableshape.hxx"
#include "unoview.hxx"
#include "subsettableshapemanager.hxx"
#include "slideshowexceptions.hxx"
#include "slideshowcontext.hxx"


namespace slideshow
{
    namespace internal
    {
        /** Base class for shapes rendered by external engines.

            Used as the common base for e.g. MediaShape or
            AppletShape, all of which are rendered by external
            components (and all employ distinct windows).

            Please note that this base class indeed assumes the shape
            does not interfere with the internal shapes in any way
            (including mutual overdraw). It therefore reports yes for
            the isBackgroundDetached() question.
         */
        class ExternalShapeBase : public AnimatableShape
        {
        public:
            /** Create a shape for the given XShape for an external shape

                @param xShape
                The XShape to represent.

                @param nPrio
                Externally-determined shape priority (used e.g. for
                paint ordering). This number _must be_ unique!
             */
            ExternalShapeBase( const ::com::sun::star::uno::Reference<
                                     ::com::sun::star::drawing::XShape >&   xShape,
                               double                                       nPrio,
                               const SlideShowContext&                      rContext ); // throw ShapeLoadFailedException;
            virtual ~ExternalShapeBase();

            virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShape > getXShape() const;

            // animation methods
            //------------------------------------------------------------------

            virtual void enterAnimationMode();
            virtual void leaveAnimationMode();


            // render methods
            //------------------------------------------------------------------

            virtual bool update() const;
            virtual bool render() const;
            virtual bool isContentChanged() const;


            // Shape attributes
            //------------------------------------------------------------------

            virtual ::basegfx::B2DRectangle getBounds() const;
            virtual ::basegfx::B2DRectangle getDomBounds() const;
            virtual ::basegfx::B2DRectangle getUpdateArea() const;
            virtual bool   isVisible() const;
            virtual double getPriority() const;
            virtual bool   isBackgroundDetached() const;

        protected:
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::uno::XComponentContext>  mxComponentContext;

        private:
            class ExternalShapeBaseListener; friend class ExternalShapeBaseListener;

            /// override in derived class to render preview
            virtual bool implRender( const ::basegfx::B2DRange& rCurrBounds ) const = 0;

            /// override in derived class to resize
            virtual void implViewChanged( const UnoViewSharedPtr& rView ) = 0;
            /// override in derived class to resize
            virtual void implViewsChanged() = 0;

            /// override in derived class to start external viewer
            virtual bool implStartIntrinsicAnimation() = 0;
            /// override in derived class to stop external viewer
            virtual bool implEndIntrinsicAnimation() = 0;


            /// The associated XShape
            ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >   mxShape;

            boost::shared_ptr<ExternalShapeBaseListener>                            mpListener;

            SubsettableShapeManagerSharedPtr                                        mpShapeManager;
            EventMultiplexer&                                                       mrEventMultiplexer;

            // The attributes of this Shape
            const double                                                            mnPriority;
            ::basegfx::B2DRectangle                                                 maBounds;
        };
    }
}

#endif /* INCLUDED_SLIDESHOW_EXTERNALSHAPEBASE_HXX */
