/*************************************************************************
 *
 *  $RCSfile: timerhelper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: as $ $Date: 2001-06-11 10:18:30 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef __FRAMEWORK_HELPER_TIMERHELPER_HXX_
#define __FRAMEWORK_HELPER_TIMERHELPER_HXX_

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef _VOS_TIMER_HXX_
#include <vos/timer.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

//........................................................................
namespace framework
{
//........................................................................

    //====================================================================
    //= ITimerListener
    //====================================================================
    class ITimerListener
    {
    public:
        virtual void    timerExpired() = 0;
    };

    //====================================================================
    //= OTimerHelper
    //====================================================================
    /** implements a timer
    */
    class OTimerHelper : private ThreadHelpBase
                       , public ::vos::OTimer
    {
    protected:
        ITimerListener* m_pListener;

    public:
        OTimerHelper( const ::vos::TTimeValue& _rExpireTime );

        // the link set here will be called with m_rMutex acquired
        void            setListener( ITimerListener* _pListener )       { m_pListener = _pListener; }
        ITimerListener* getListener(                            ) const { return m_pListener;       }

    protected:
        virtual void SAL_CALL onShot();
    };

//........................................................................
}   // namespace framework
//........................................................................

#endif // _FRAMEWORK_HELPER_TIMERHELPER_HXX_

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2001/05/07 13:40:22  fs
 *  initial checkin - a vos-based timer using a callback on expiration
 *
 *
 *  Revision 1.0 03.05.01 16:10:30  fs
 ************************************************************************/

