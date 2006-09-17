/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: activitybase.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 08:31:50 $
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

#include <activitybase.hxx>


namespace presentation
{
    namespace internal
    {
        // TODO(P1): Elide some virtual function calls, by templifying this
        // static hierarchy

        ActivityBase::ActivityBase( const ActivityParameters& rParms ) :
            mpEndEvent( rParms.mrEndEvent ),
            mrEventQueue( rParms.mrEventQueue ),
            mpShape(),
            mpAttributeLayer(),
            maRepeats( rParms.mrRepeats ),
            mnAccelerationFraction( rParms.mnAccelerationFraction ),
            mnDecelerationFraction( rParms.mnDecelerationFraction ),
            mbAutoReverse( rParms.mbAutoReverse ),
            mbFirstPerformCall( true ),
            mbIsActive( true ) {}

        void ActivityBase::dispose()
        {
            // deactivate
            mbIsActive = false;

            // dispose event
            if( mpEndEvent.get() )
                mpEndEvent->dispose();

            // release references
            mpEndEvent.reset();
            mpShape.reset();
            mpAttributeLayer.reset();
        }

        double ActivityBase::calcTimeLag() const
        {
            // TODO(Q1): implement different init process!
            if (isActive() && mbFirstPerformCall)
            {
                mbFirstPerformCall = false;

                // notify derived classes that we're
                // starting now
                const_cast<ActivityBase *>(this)->startAnimation();
            }
            return 0.0;
        }

        bool ActivityBase::perform()
        {
            // still active?
            if( !isActive() )
                return false; // no, early exit.

            OSL_ASSERT( ! mbFirstPerformCall );

            return true;
        }

        bool ActivityBase::isActive() const
        {
            return mbIsActive;
        }

        bool ActivityBase::needsScreenUpdate() const
        {
            // animations, while active, _do_ need screen updates
            // after perform() calls. This is because
            // e.g. SlideChanger internally uses XCustomSprites,
            // which don't repaint by themselves.
            //
            // What is actually a feature, because then, the
            // activities thread can issue synchronized repaints,
            // updating all visible sprites at once.

            // FIXME(P1):
            // Since by definition, the animation must not change the
            // sprite when it is no longer active, we could possibly
            // return true here only as long as mbIsActive is true.
            // Unfortunately, the ActivitiesQueue asks for necessary
            // screen updates after the perform calls, and this is a
            // problem for the very last perform call of an activity
            // (the one that ends the activity): this last one usually
            // _does_ need a screen update, but has already set
            // mbIsActive to false. I would consider delayed false returns
            // here a hack, since it relies on call order. For the time
            // being, we thus return true here, always (note that a
            // finished Activity will normally be not queried for screen
            // updates, anyway, since it's removed from the queues).
            return true;
        }

        void ActivityBase::setTargets( const AnimatableShapeSharedPtr&      rShape,
                                       const ShapeAttributeLayerSharedPtr&  rAttrLayer )
        {
            ENSURE_AND_THROW( rShape.get(),
                              "ActivityBase::setTargets(): Invalid shape" );
            ENSURE_AND_THROW( rAttrLayer.get(),
                              "ActivityBase::setTargets(): Invalid attribute layer" );

            mpShape = rShape;
            mpAttributeLayer = rAttrLayer;
        }

        void ActivityBase::endActivity()
        {
            // this is a regular activity end
            mbIsActive = false;

            // Activity is ending, queue event, then
            if( mpEndEvent.get() )
                mrEventQueue.addEvent( mpEndEvent );

            // release references
            mpEndEvent.reset();
        }

        void ActivityBase::dequeued()
        {
            // xxx todo:
//             // ignored here, if we're still active. Discrete
//             // activities are dequeued after every perform() call,
//             // thus, the call is only significant when isActive() ==
//             // false.
            if( !isActive() )
                endAnimation();
        }

        void ActivityBase::end()
        {
            if (!mbIsActive || isDisposed())
                return;
            // assure animation is started:
            if (mbFirstPerformCall) {
                mbFirstPerformCall = false;
                // notify derived classes that we're starting now
                this->startAnimation();
            }

            performEnd(); // calling private virtual

            endAnimation();

            endActivity();
        }

        double ActivityBase::calcAcceleratedTime( double nT ) const
        {
            // Handle acceleration/deceleration
            // ================================

            // clamp nT to permissible [0,1] range
            nT = ::canvas::tools::clamp( nT, 0.0, 1.0 );

            // take acceleration/deceleration into account. if the sum
            // of mnAccelerationFraction and mnDecelerationFraction
            // exceeds 1.0, ignore both (that's according to SMIL spec)
            if( (mnAccelerationFraction > 0.0 ||
                 mnDecelerationFraction > 0.0) &&
                mnAccelerationFraction + mnDecelerationFraction <= 1.0 )
            {
                // calc accelerated/decelerated time.
                //
                // We have three intervals:
                // 1 [0,a]
                // 2 [a,d]
                // 3 [d,1] (with a and d being acceleration/deceleration
                // fraction, resp.)
                //
                // The change rate during interval 1 is constantly
                // increasing, reaching 1 at a. It then stays at 1,
                // starting a linear decrease at d, ending with 0 at
                // time 1. The integral of this function is the
                // required new time nT'.
                //
                // As we arbitrarily assumed 1 as the upper value of
                // the change rate, the integral must be normalized to
                // reach nT'=1 at the end of the interval. This
                // normalization constant is:
                //
                // c = 1 - 0.5a - 0.5d
                //
                // The integral itself then amounts to:
                //
                // 0.5 nT^2 / a + (nT-a) + (nT - 0.5 nT^2 / d)
                //
                // (where each of the three summands correspond to the
                // three intervals above, and are applied only if nT
                // has reached the corresponding interval)
                //
                // The graph of the change rate is a trapezoid:
                //
                //   |
                //  1|      /--------------\
                //   |     /                \
                //   |    /                  \
                //   |   /                    \
                //   -----------------------------
                //      0   a              d  1
                //
                //
                const double nC( 1.0 - 0.5*mnAccelerationFraction - 0.5*mnDecelerationFraction );

                // this variable accumulates the new time value
                double nTPrime(0.0);

                if( nT < mnAccelerationFraction )
                {
                    nTPrime += 0.5*nT*nT/mnAccelerationFraction; // partial first interval
                }
                else
                {
                    nTPrime += 0.5*mnAccelerationFraction; // full first interval

                    if( nT <= 1.0-mnDecelerationFraction )
                    {
                        nTPrime += nT-mnAccelerationFraction; // partial second interval
                    }
                    else
                    {
                        nTPrime += 1.0 - mnAccelerationFraction - mnDecelerationFraction; // full second interval

                        const double nTRelative( nT - 1.0 + mnDecelerationFraction );

                        nTPrime += nTRelative - 0.5*nTRelative*nTRelative / mnDecelerationFraction;
                    }
                }

                // normalize, and assign to work variable
                nT = nTPrime / nC;
            }

            return nT;
        }
    }
}
