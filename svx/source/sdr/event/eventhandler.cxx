/*************************************************************************
 *
 *  $RCSfile: eventhandler.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2003-11-24 16:47:03 $
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

#ifndef _SDR_EVENT_EVENTHANDLER_HXX
#include <svx/sdr/event/eventhandler.hxx>
#endif

// for SOLARIS compiler include of algorithm part of _STL is necesary to
// get access to basic algos like ::std::find
#include <algorithm>

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace event
    {
        BaseEvent::BaseEvent(EventHandler& rEventHandler)
        :   mrEventHandler(rEventHandler)
        {
            mrEventHandler.AddEvent(*this);
        }

        BaseEvent::~BaseEvent()
        {
            mrEventHandler.RemoveEvent(*this);
        }
    } // end of namespace mixer
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace event
    {
        void EventHandler::AddEvent(BaseEvent& rBaseEvent)
        {
            maVector.push_back(&rBaseEvent);
        }

        void EventHandler::RemoveEvent(BaseEvent& rBaseEvent)
        {
            if(maVector.back() == &rBaseEvent)
            {
                // the one to remove is the last, pop
                maVector.pop_back();
            }
            else
            {
                const BaseEventVector::iterator aFindResult = ::std::find(
                    maVector.begin(), maVector.end(), &rBaseEvent);
                DBG_ASSERT(aFindResult != maVector.end(),
                    "EventHandler::RemoveEvent: Event to be removed not found (!)");
                maVector.erase(aFindResult);
            }
        }

        BaseEvent* EventHandler::GetEvent()
        {
            if(maVector.size())
            {
                // get the last event, that one is fastest to be removed
                return maVector.back();
            }
            else
            {
                return 0L;
            }
        }

        EventHandler::EventHandler()
        {
        }

        EventHandler::~EventHandler()
        {
            while(maVector.size())
            {
                delete GetEvent();
            }
        }

        // Trigger and consume the events
        void EventHandler::ExecuteEvents()
        {
            BaseEvent* pEvent;

            while(pEvent = GetEvent())
            {
                pEvent->ExecuteEvent();
                delete pEvent;
            }
        }

        // for control
        sal_Bool EventHandler::IsEmpty() const
        {
            return (0L == maVector.size());
        }
    } // end of namespace mixer
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace event
    {
        TimerEventHandler::TimerEventHandler(sal_uInt32 nTimeout)
        {
            SetTimeout(nTimeout);
            Stop();
        }

        TimerEventHandler::~TimerEventHandler()
        {
            Stop();
        }

        // The timer when it is triggered; from class Timer
        void TimerEventHandler::Timeout()
        {
            ExecuteEvents();
        }

        // reset the timer
        void TimerEventHandler::Restart()
        {
            Start();
        }
    } // end of namespace mixer
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
