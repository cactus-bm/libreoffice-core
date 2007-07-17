/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mediashape.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 14:54:14 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_slideshow.hxx"

// must be first
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>
#include <canvas/canvastools.hxx>

#include <com/sun/star/drawing/XShape.hpp>

#include "mediashape.hxx"
#include "viewmediashape.hxx"
#include "externalshapebase.hxx"
#include "slideshowcontext.hxx"
#include "shape.hxx"
#include "tools.hxx"

#include <boost/bind.hpp>
#include <algorithm>


using namespace ::com::sun::star;


namespace slideshow
{
    namespace internal
    {
        /** Represents a media shape.

            This implementation offers support for media shapes.
            Such shapes need special treatment.
         */
        class MediaShape : public ExternalShapeBase
        {
        public:
            /** Create a shape for the given XShape for a media object

                @param xShape
                The XShape to represent.

                @param nPrio
                Externally-determined shape priority (used e.g. for
                paint ordering). This number _must be_ unique!
             */
            MediaShape( const ::com::sun::star::uno::Reference<
                               ::com::sun::star::drawing::XShape >& xShape,
                        double                                      nPrio,
                        const SlideShowContext&                     rContext ); // throw ShapeLoadFailedException;

        private:

            // View layer methods
            //------------------------------------------------------------------

            virtual void addViewLayer( const ViewLayerSharedPtr&    rNewLayer,
                                       bool                         bRedrawLayer );
            virtual bool removeViewLayer( const ViewLayerSharedPtr& rNewLayer );
            virtual bool clearAllViewLayers();


            // ExternalShapeBase methods
            //------------------------------------------------------------------

            virtual bool implRender( const ::basegfx::B2DRange& rCurrBounds ) const;
            virtual void implViewChanged( const UnoViewSharedPtr& rView );
            virtual void implViewsChanged();
            virtual bool implStartIntrinsicAnimation();
            virtual bool implEndIntrinsicAnimation();

            /// the list of active view shapes (one for each registered view layer)
            typedef ::std::vector< ViewMediaShapeSharedPtr > ViewMediaShapeVector;
            ViewMediaShapeVector                             maViewMediaShapes;
        };


        MediaShape::MediaShape( const uno::Reference< drawing::XShape >& xShape,
                                  double                                     nPrio,
                                const SlideShowContext&                  rContext ) :
            ExternalShapeBase( xShape, nPrio, rContext ),
            maViewMediaShapes()
        {
        }

        // ---------------------------------------------------------------------

        void MediaShape::implViewChanged( const UnoViewSharedPtr& rView )
        {
            // determine ViewMediaShape that needs update
            ViewMediaShapeVector::const_iterator       aIter(maViewMediaShapes.begin());
            ViewMediaShapeVector::const_iterator const aEnd (maViewMediaShapes.end());
            while( aIter != aEnd )
            {
                if( (*aIter)->getViewLayer()->isOnView(rView) )
                    (*aIter)->resize(getBounds());

                ++aIter;
            }
        }

        // ---------------------------------------------------------------------

        void MediaShape::implViewsChanged()
        {
            // resize all ViewShapes
            ::std::for_each( maViewMediaShapes.begin(),
                             maViewMediaShapes.end(),
                             ::boost::bind(
                                 &ViewMediaShape::resize,
                                 _1,
                                 ::boost::cref( getBounds())) );
        }

        // ---------------------------------------------------------------------

        void MediaShape::addViewLayer( const ViewLayerSharedPtr& rNewLayer,
                                       bool                      bRedrawLayer )
        {
            maViewMediaShapes.push_back(
                ViewMediaShapeSharedPtr( new ViewMediaShape( rNewLayer,
                                                             getXShape(),
                                                             mxComponentContext )));

            // push new size to view shape
            maViewMediaShapes.back()->resize( getBounds() );

            // render the Shape on the newly added ViewLayer
            if( bRedrawLayer )
                maViewMediaShapes.back()->render( getBounds() );
        }

        // ---------------------------------------------------------------------

        bool MediaShape::removeViewLayer( const ViewLayerSharedPtr& rLayer )
        {
            const ViewMediaShapeVector::iterator aEnd( maViewMediaShapes.end() );

            OSL_ENSURE( ::std::count_if(maViewMediaShapes.begin(),
                                        aEnd,
                                        ::boost::bind<bool>(
                                            ::std::equal_to< ViewLayerSharedPtr >(),
                                            ::boost::bind( &ViewMediaShape::getViewLayer, _1 ),
                                            ::boost::cref( rLayer ) ) ) < 2,
                        "MediaShape::removeViewLayer(): Duplicate ViewLayer entries!" );

            ViewMediaShapeVector::iterator aIter;

            if( (aIter=::std::remove_if( maViewMediaShapes.begin(),
                                         aEnd,
                                         ::boost::bind<bool>(
                                             ::std::equal_to< ViewLayerSharedPtr >(),
                                             ::boost::bind( &ViewMediaShape::getViewLayer,
                                                            _1 ),
                                             ::boost::cref( rLayer ) ) )) == aEnd )
            {
                // view layer seemingly was not added, failed
                return false;
            }

            // actually erase from container
            maViewMediaShapes.erase( aIter, aEnd );

            return true;
        }

        // ---------------------------------------------------------------------

        bool MediaShape::clearAllViewLayers()
        {
            maViewMediaShapes.clear();
            return true;
        }

        // ---------------------------------------------------------------------

        bool MediaShape::implRender( const ::basegfx::B2DRange& rCurrBounds ) const
        {
            // redraw all view shapes, by calling their update() method
            if( ::std::count_if( maViewMediaShapes.begin(),
                                 maViewMediaShapes.end(),
                                 ::boost::bind<bool>(
                                     ::boost::mem_fn( &ViewMediaShape::render ),
                                     _1,
                                     ::boost::cref( rCurrBounds ) ) )
                != static_cast<ViewMediaShapeVector::difference_type>(maViewMediaShapes.size()) )
            {
                // at least one of the ViewShape::update() calls did return
                // false - update failed on at least one ViewLayer
                return false;
            }

            return true;
        }

        // ---------------------------------------------------------------------

        bool MediaShape::implStartIntrinsicAnimation()
        {
            ::std::for_each( maViewMediaShapes.begin(),
                             maViewMediaShapes.end(),
                             ::boost::mem_fn( &ViewMediaShape::startMedia ) );

            return true;
        }

        // ---------------------------------------------------------------------

        bool MediaShape::implEndIntrinsicAnimation()
        {
            ::std::for_each( maViewMediaShapes.begin(),
                             maViewMediaShapes.end(),
                             ::boost::mem_fn( &ViewMediaShape::endMedia ) );

            return true;
        }

        ShapeSharedPtr createMediaShape(
            const uno::Reference< drawing::XShape >& xShape,
            double                                   nPrio,
            const SlideShowContext&                  rContext)
        {
            boost::shared_ptr< MediaShape > pMediaShape(
                new MediaShape(xShape, nPrio, rContext));

            return pMediaShape;
        }

    }
}
