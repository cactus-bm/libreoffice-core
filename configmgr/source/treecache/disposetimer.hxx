/*************************************************************************
 *
 *  $RCSfile: disposetimer.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2000-12-15 16:14:03 $
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

#ifndef CONFIGMGR_DISPOSETIMER_HXX
#define CONFIGMGR_DISPOSETIMER_HXX

#include "apitypes.hxx"
#include "timestamp.hxx"
#include "options.hxx"
#include <osl/mutex.hxx>
#include <vos/timer.hxx>
#include <map>
#include <queue>

namespace configmgr
{
    class TreeManager;
////////////////////////////////////////////////////////////////////////////////
    class OTreeDisposeScheduler
    {
        typedef std::multimap< TimeStamp, vos::ORef< OOptions >, ltTimeStamp > Agenda;

        class Timer : public vos::OTimer
        {
        public:
            OTreeDisposeScheduler& rParent;

            Timer(OTreeDisposeScheduler& _rParent) : rParent(_rParent) {};

            // vos::OTimer
            virtual void SAL_CALL onShot();
        };
        friend void Timer::onShot();

    private:
        mutable osl::Mutex  m_aMutex;

        Agenda              m_aAgenda;

        vos::ORef<Timer>    m_xTimer;
        TreeManager&        m_rTreeManager;

        TimeInterval m_aCleanupDelay;
        TimeInterval m_aCleanupInterval;
    public:
    //-------- Construction and destruction -----------------------------------
        explicit
        OTreeDisposeScheduler(TreeManager& _rTreeManager, TimeInterval const& _aCleanupDelay)
            : m_xTimer(0)
            , m_rTreeManager(_rTreeManager)
            , m_aCleanupDelay(_aCleanupDelay)
            , m_aCleanupInterval(_aCleanupDelay)
        {}

        explicit
        OTreeDisposeScheduler(TreeManager& _rTreeManager, TimeInterval const& _aCleanupDelay, TimeInterval const& _aCleanupInterval)
            : m_xTimer(0)
            , m_rTreeManager(_rTreeManager)
            , m_aCleanupDelay(_aCleanupDelay)
            , m_aCleanupInterval(_aCleanupInterval)
        {}

        ~OTreeDisposeScheduler() { stopAndClearTasks(); }

    //-------- Delay and Interval ---------------------------------------------
        /// sets the initial delay to be used for cleanup in the future, does not affect an already started process
        void setCleanupDelay(TimeInterval const& _aCleanupDelay)
        {
            osl::MutexGuard aGuard(m_aMutex);
            m_aCleanupDelay = _aCleanupDelay;
        }

        /// sets the initial delay and recurrance interval to be used for cleanup in the future, does not affect an already started process
        void setCleanupDelay(TimeInterval const& _aCleanupDelay, TimeInterval const& _aCleanupInterval)
        {
            osl::MutexGuard aGuard(m_aMutex);
            m_aCleanupDelay = _aCleanupDelay;
            m_aCleanupInterval = _aCleanupInterval;
        }

        /// sets the recurrance interval to be used for cleanup in the future, does not affect an already started process
        void setCleanupInterval(TimeInterval const& _aCleanupInterval)
        {
            osl::MutexGuard aGuard(m_aMutex);
            m_aCleanupInterval = _aCleanupInterval;
        }

        /// retrieves the initial delay used for cleanup
        TimeInterval const& getCleanupDelay() const
        {
            osl::MutexGuard aGuard(m_aMutex);
            return m_aCleanupDelay;
        }

        /// retrieves the recurrance interval used for cleanup
        TimeInterval const& getCleanupInterval() const
        {
            osl::MutexGuard aGuard(m_aMutex);
            return m_aCleanupInterval;
        }

        /// calculate the time when to cleanup an pbject that became eligible at <var>aBaseTime</var>.
        TimeStamp getCleanupTime(TimeStamp const& aBaseTime = TimeStamp::getCurrentTime())
        {
            return implGetCleanupTime(aBaseTime, getCleanupDelay());
        }

    //-------- Control of execution  ------------------------------------------
        /// ensure this will execute cleanup duties for _xOptions (no later than after getCleanupDelay() has elapsed)
        void scheduleCleanup(vos::ORef< OOptions > const& _xOptions);

        /// stop and discard pending activities for _xOptions
        void clearTasks(vos::ORef< OOptions > const& _xOptions);

        /// stop and discard pending activities
        void stopAndClearTasks();
    private:
        // vos::OTimer
        void onTimerShot();

        vos::ORef< OOptions > getTask(TimeStamp const& _aActualTime, TimeStamp& _aNextTime);

        TimeStamp runDisposer(TimeStamp const& _aActualTime);
    private:
        TimeStamp implAddTask(vos::ORef< OOptions > const& _xOptions, TimeStamp const& _aTime);
        void implStartBefore(TimeStamp const& _aTime);

        static TimeStamp implGetCleanupTime(TimeStamp const& aBaseTime, TimeInterval const& aDelay)
        { return aBaseTime + aDelay; }
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace configmgr

#endif // CONFIGMGR_DISPOSETIMER_HXX

