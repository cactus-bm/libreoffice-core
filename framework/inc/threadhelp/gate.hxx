/*************************************************************************
 *
 *  $RCSfile: gate.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: as $ $Date: 2001-06-11 10:23:00 $
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

#ifndef __FRAMEWORK_THREADHELP_GATE_HXX_
#define __FRAMEWORK_THREADHELP_GATE_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELP_INONCOPYABLE_H_
#include <threadhelp/inoncopyable.h>
#endif

#ifndef __FRAMEWORK_THREADHELP_IGATE_H_
#include <threadhelp/igate.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//  const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
    @short          implement a gate to block multiple threads at same time or unblock all
    @descr          A gate can be used as a negative-condition! You can open a "door" - wait() will not block ...
                    or you can close it - wait() blocks till open() is called again.
                    As a special feature you can open the gate a little bit by sing openGap().
                    Then all currently waiting threads are running immediately - but new ones are blocked!

    @attention      To prevent us against wrong using, the default ctor, copy ctor and the =operator are maked private!

    @implements     IGate
    @base           IGate
                    INonCopyable

    @devstatus      ready to use
*//*-*************************************************************************************************************/
class Gate : public  IGate
           , private INonCopyable
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        /*-****************************************************************************************************//**
            @short      ctor
            @descr      These initialize the object right as an open gate.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline Gate()
            :   m_bClosed   ( sal_False )
            ,   m_bGapOpen  ( sal_False )
        {
            open();
        }

        /*-****************************************************************************************************//**
            @short      dtor
            @descr      Is user forget it - we open the gate ...
                        blocked threads can running ... but I don't know
                        if it's right - we are destroyed yet!?

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline ~Gate()
        {
            open();
        }

        /*-****************************************************************************************************//**
            @interface  IGate
            @short      open the gate
            @descr      A wait() call will not block then.

            @seealso    method close()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void open()
        {
            // We must safe access to our internal member!
            ::osl::MutexGuard aLock( m_aAccessLock );
            // Set condition -> wait don't block any longer -> gate is open
            m_aPassage.set();
            // Check if operation was successful!
            // Check returns false if condition isn't set => m_bClosed will be true then => we must return false; opening failed
            m_bClosed = ( m_aPassage.check() == sal_False );
        }

        /*-****************************************************************************************************//**
            @interface  IGate
            @short      close the gate
            @descr      A wait() call will block then.

            @seealso    method open()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void close()
        {
            // We must safe access to our internal member!
            ::osl::MutexGuard aLock( m_aAccessLock );
            // Reset condition -> wait blocks now -> gate is closed
            m_aPassage.reset();
            // Check if operation was successful!
            // Check returns false if condition was reseted => m_bClosed will be true then => we can return true; closing ok
            m_bClosed = ( m_aPassage.check() == sal_False );
        }

        /*-****************************************************************************************************//**
            @interface  IGate
            @short      open gate for current waiting threads
            @descr      All current waiting threads stand in wait() at line "m_aPassage.wait()" ...
                        With this call you can open the passage for these waiting ones.
                        The "gap" is closed by any new thread which call wait() automaticly!

            @seealso    method wait()
            @seealso    method open()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void openGap()
        {
            // We must safe access to our internal member!
            ::osl::MutexGuard aLock( m_aAccessLock );
            // Open passage for current waiting threads.
            m_aPassage.set();
            // Check state of condition.
            // If condition is set check() returns true => m_bGapOpen will be true too => we can use it as return value.
            m_bGapOpen = ( m_aPassage.check() == sal_True );
        }

        /*-****************************************************************************************************//**
            @interface  IGate
            @short      must be called to pass the gate
            @descr      If gate "open"   => wait() will not block.
                        If gate "closed" => wait() will block till somewhere open it again.
                        If gap  "open"   => currently waiting threads unblocked, new ones blocked

            @seealso    method wait()
            @seealso    method open()

            @param      "pTimeOut", optional parameter to wait a certain time
            @return     true, if wait was successful (gate was opened)
                        false, if condition has an error or timeout was reached!

            @onerror    We return false.
        *//*-*****************************************************************************************************/
        inline virtual sal_Bool wait( const TimeValue* pTimeOut = NULL )
        {
            // We must safe access to our internal member!
            ::osl::ClearableMutexGuard aLock( m_aAccessLock );
            // If gate not closed - caller can pass it.
            sal_Bool bSuccessful = sal_True;
            if( m_bClosed == sal_True )
            {
                // Otherwise first new thread must close an open gap!
                if( m_bGapOpen == sal_True )
                {
                    m_bGapOpen = sal_False;
                    m_aPassage.reset();
                }
                // Then we must release used access lock -
                // because next call will block ...
                // and if we hold the access lock nobody else can use this object without a dadlock!
                aLock.clear();
                // Wait for opening gate ...
                bSuccessful = ( m_aPassage.wait( pTimeOut ) == osl_cond_result_ok );
            }

            return bSuccessful;
        }

    //-------------------------------------------------------------------------------------------------------------
    //  private member
    //-------------------------------------------------------------------------------------------------------------
    private:

        ::osl::Mutex        m_aAccessLock   ;
        ::osl::Condition    m_aPassage      ;
        sal_Bool            m_bClosed       ;
        sal_Bool            m_bGapOpen      ;

};      //  class Gate

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_THREADHELP_GATE_HXX_
