/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: resetableguard.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:37:42 $
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

#ifndef __FRAMEWORK_THREADHELP_RESETABLEGUARD_HXX_
#define __FRAMEWORK_THREADHELP_RESETABLEGUARD_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELP_INONCOPYABLE_H_
#include <threadhelp/inoncopyable.h>
#endif

#ifndef __FRAMEWORK_THREADHELP_IMUTEX_H_
#include <threadhelp/imutex.h>
#endif

//#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
//#include <threadhelp/threadhelpbase.hxx>
//#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
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
    @short          implement a guard for implementing save thread access
    @descr          These guard has an additional feature to well known one ::osl::Guard.
                    You can lock() and unlock() it very often!
                    A set bool flag inside protect this implementation against multiple lock() calls
                    without any unlock()! So the increasing of guarded mutex couldn't be greater then 1 ...

    @attention      a) To prevent us against wrong using, the default ctor, copy ctor and the =operator are maked private!
                    b) Use interface "IMutex" of set LockHelper only - because we must support an exclusiv locking.
                       Interface "IRWLock" should be used by special guard implementations ... like "ReadGuard" or "WriteGuard"!

    @implements     -
    @base           INonCopyable

    @devstatus      ready to use
*//*-*************************************************************************************************************/
class ResetableGuard : private INonCopyable
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        /*-****************************************************************************************************//**
            @short      ctors
            @descr      Use these ctor methods to initialize the guard right.
                        Given lock reference must be valid - otherwise crashes could occure!

            @seealso    -

            @param      "pLock", pointer to lock helper of user
            @param      "rLock", reference to lock helper of user
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline ResetableGuard( IMutex* pLock )
            :   m_pLock    ( pLock     )
            ,   m_bLocked  ( sal_False )
        {
            lock();
        }

        //*********************************************************************************************************
        inline ResetableGuard( IMutex& rLock )
            :   m_pLock    ( &rLock    )
            ,   m_bLocked  ( sal_False )
        {
            lock();
        }

        /*-****************************************************************************************************//**
            @short      dtor
            @descr      We must release set mutex if programmer forget it ...

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline ~ResetableGuard()
        {
            unlock();
        }

        /*-****************************************************************************************************//**
            @short      enable/disable the lock
            @descr      Use this methods to lock or unlock the mutex.
                        You can do it so often you wish to do that ...

            @attention  We use another member to prevent us against multiple acquire calls of the same guard
                        without suitable release calls!
                        You don't must protect access at these bool member by using an own mutex ....
                        because nobody use the same guard instance from different threads!
                        It will be a function-local object every time.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline void lock()
        {
            if( m_bLocked == sal_False )
            {
                m_pLock->acquire();
                m_bLocked = sal_True;
            }
        }

        //*********************************************************************************************************
        inline void unlock()
        {
            if( m_bLocked == sal_True )
            {
                m_pLock->release();
                m_bLocked = sal_False;
            }
        }

    //-------------------------------------------------------------------------------------------------------------
    //  private methods
    //-------------------------------------------------------------------------------------------------------------
    private:

        /*-****************************************************************************************************//**
            @short      disable using of these functions!
            @descr      It's not allowed to use this methods. Different problem can occure otherwise.
                        Thats why we disable it by make it private.

            @seealso    other ctor

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        ResetableGuard();

    //-------------------------------------------------------------------------------------------------------------
    //  private member
    //-------------------------------------------------------------------------------------------------------------
    private:

        IMutex*         m_pLock     ;   /// pointer to safed lock member of user
        sal_Bool        m_bLocked   ;   /// protection against multiple lock() calls without unlock()

};      //  class ResetableGuard

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_THREADHELP_RESETABLEGUARD_HXX_
