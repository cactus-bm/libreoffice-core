/*************************************************************************
 *
 *  $RCSfile: event.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-10 13:54:06 $
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

#ifndef _SLIDESHOW_EVENT_HXX
#define _SLIDESHOW_EVENT_HXX

#include <boost/shared_ptr.hpp>
#include <vector>

#include <disposable.hxx>


/* Definition of Event interface */

namespace presentation
{
    namespace internal
    {

        class Event : public Disposable
        {
        public:
            /** Execute the event.

                @return true, if event was successfully executed.
             */
            virtual bool fire() = 0;

            /** Query whether this event has been fired.

                @return true, if this event has already been fired.
             */
            virtual bool wasFired() const = 0;

            /** Query the activation time instant this event shall be
                fired, if it was inserted at instant nCurrentTime into
                the queue.

                @param nCurrentTime
                The time from which the activation time is to be
                calculated from.

                @return the time instant in seconds, on which this
                event is to be fired.
             */
            virtual double getActivationTime( double nCurrentTime ) const = 0;
        };

        typedef ::boost::shared_ptr< Event > EventSharedPtr;
        typedef ::std::vector< EventSharedPtr > VectorOfEvents;

    }
}

#endif /* _SLIDESHOW_EVENT_HXX */
