/*************************************************************************
 *
 *  $RCSfile: asynceventnotifier.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: tra $ $Date: 2002-02-21 14:45:39 $
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

#ifndef _ASYNCEVENTNOTIFIER_HXX_
#define _ASYNCEVENTNOTIFIER_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERLISTENER_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerListener.hpp>
#endif

#ifndef _CPPUHELPER_INTERFACECONTAINER_H_
#include <cppuhelper/interfacecontainer.h>
#endif

#include <windows.h>

#include <list>
#include <utility>

//---------------------------------------------
//
//---------------------------------------------

class CAsyncEventNotifier
{
public:
    typedef void (SAL_CALL ::com::sun::star::ui::dialogs::XFilePickerListener::*EventListenerMethod_t)(const ::com::sun::star::ui::dialogs::FilePickerEvent&);

public:
    CAsyncEventNotifier(cppu::OBroadcastHelper& rBroadcastHelper);

    bool SAL_CALL start();
    void SAL_CALL stop();

    void SAL_CALL notifyEvent( EventListenerMethod_t aListenerMethod, ::com::sun::star::ui::dialogs::FilePickerEvent aEvent );

private:
    typedef std::pair<EventListenerMethod_t, ::com::sun::star::ui::dialogs::FilePickerEvent> EventRecord_t;

    size_t        SAL_CALL getEventListSize();
    void          SAL_CALL resetNotifyEvent();
    EventRecord_t SAL_CALL getNextEventRecord();
    void          SAL_CALL removeNextEventRecord();

    void SAL_CALL run( );

    static unsigned int WINAPI ThreadProc( LPVOID pParam );

private:
    std::list<EventRecord_t>    m_EventList;
    HANDLE                      m_hEventNotifierThread;
    bool                        m_bRun;
    unsigned                    m_ThreadId;
    ::cppu::OBroadcastHelper&   m_rBroadcastHelper;
    osl::Condition              m_NotifyEvent;
    osl::Mutex                  m_Mutex;

// prevent copy and assignment
private:
    CAsyncEventNotifier( const CAsyncEventNotifier& );
    CAsyncEventNotifier& operator=( const CAsyncEventNotifier& );
};

#endif