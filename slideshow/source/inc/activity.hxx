/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: activity.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 15:01:19 $
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

#ifndef INCLUDED_SLIDESHOW_ACTIVITY_HXX
#define INCLUDED_SLIDESHOW_ACTIVITY_HXX

#include <sal/types.h>

#include <boost/shared_ptr.hpp>

#include "disposable.hxx"


/* Definition of Activity interface */

namespace slideshow
{
    namespace internal
    {

        class Activity : public Disposable
        {
        public:
            /** Perform the activity associated with this interface's
                implementation.

                @return true, if activity continues, or false, if activity has
                ended.
            */
            virtual bool perform() = 0;

            /** Calculates whether the activity lags time.

                If this method returns a time lag greater than 0.0,
                the ActivitiesQueue will adjust the global slideshow
                time, by subtracting the given amount of lag.

                @return time lag or 0.0. Value must be greater or
                equal than zero.
            */
            virtual double calcTimeLag() const = 0;

            /** Query whether this activity is still continuing

                @return true, if this activity still
                continues. Returns false, if activity has ended. It is
                required that operator() returns false, when
                isActive() returns false. Furthermore, it is required
                that the inactive state is persistent; an activity
                that has become inactive (i.e. isActive() once
                returned false) must stay in that state eternally.
            */
            virtual bool isActive() const = 0;

            /** Notifies the Activity that it has now left the
                ActivitiesQueue

                Use this method to react on the queue removal
                event. For animated shapes, this is e.g. used to
                switch back to the non-sprite presentation mode of the
                shape.
             */
            virtual void dequeued() = 0;

            /** Forces this activity deactivate and get to its end state
                (if possible), but does _not_ dispose.
            */
            virtual void end() = 0;
        };

        typedef ::boost::shared_ptr< Activity > ActivitySharedPtr;

    }
}

#endif /* INCLUDED_SLIDESHOW_ACTIVITY_HXX */
