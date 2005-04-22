/*************************************************************************
 *
 *  $RCSfile: wakeupevent.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-22 13:29:25 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

// must be first
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>

#include <wakeupevent.hxx>


namespace presentation
{
    namespace internal
    {
        WakeupEvent::WakeupEvent(
            boost::shared_ptr<canvas::tools::ElapsedTime> const & pTimeBase,
            ActivitiesQueue& rActivityQueue ) :
            maTimer(pTimeBase),
            mnNextTime(0.0),
            mpActivity(),
            mrActivityQueue( rActivityQueue )
        {
        }

        void WakeupEvent::dispose()
        {
            mpActivity.reset();
        }

        bool WakeupEvent::fire()
        {
            if( !mpActivity.get() )
                return false;

            return mrActivityQueue.addActivity( mpActivity );
        }

        bool WakeupEvent::isCharged() const
        {
            // this event won't expire, we fire everytime we're
            // re-inserted into the event queue.
            return true;
        }

        double WakeupEvent::getActivationTime( double nCurrentTime ) const
        {
            const double nElapsedTime( maTimer.getElapsedTime() );

            return ::std::max( nCurrentTime,
                               nCurrentTime - nElapsedTime + mnNextTime );
        }

        void WakeupEvent::start()
        {
            // start timer
            maTimer.reset();
        }

        void WakeupEvent::setNextTimeout( double rNextTime )
        {
            mnNextTime = rNextTime;
        }

        void WakeupEvent::setActivity( const ActivitySharedPtr& rActivity )
        {
            mpActivity = rActivity;
        }
    }
}
