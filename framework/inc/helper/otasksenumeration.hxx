/*************************************************************************
 *
 *  $RCSfile: otasksenumeration.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: as $ $Date: 2001-03-29 13:17:10 $
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

#ifndef __FRAMEWORK_HELPER_OTASKSENUMERATION_HXX_
#define __FRAMEWORK_HELPER_OTASKSENUMERATION_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_OMUTEXMEMBER_HXX_
#include <helper/omutexmember.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#include <macros/generic.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XINTERFACE_HXX_
#include <macros/xinterface.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XTYPEPROVIDER_HXX_
#include <macros/xtypeprovider.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_DEBUG_HXX_
#include <macros/debug.hxx>
#endif

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XENUMERATION_HPP_
#include <com/sun/star/container/XEnumeration.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XTASK_HPP_
#include <com/sun/star/frame/XTask.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//  exported const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  exported definitions
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
    @short          implement a helper for a oneway enumeration of tasks
    @descr          You can step during this list only for one time! Its a snapshot.
                    Don't forget to release the reference. You are the owner of an instance of this implementation.
                    You cant use this as a baseclass. Please use it as a dynamical object for return.

    @implements     XInterface
                    XTypeProvider
                    XEventListener
                    XEnumeration

    @base           OMutexMember
                    OWeakObject

    @devstatus      ready to use
*//*-*************************************************************************************************************/

class OTasksEnumeration :   public css::lang::XTypeProvider     ,
                            public css::lang::XEventListener    ,
                            public css::container::XEnumeration ,
                            public OMutexMember                 ,
                            public ::cppu::OWeakObject
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------

    public:

        //---------------------------------------------------------------------------------------------------------
        //  constructor / destructor
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      constructor to initialize this enumeration
            @descr      An enumeration is a list with oneway-access! You can get every member only for one time.
                        This method allow to initialize this oneway list with values.

            @seealso    -

            @param      "seqTasks" is a sequence of interfaces, which are tasks.
            @return     -

            @onerror    Do nothing and reset this object to default with an empty list.
        *//*-*****************************************************************************************************/

         OTasksEnumeration( const css::uno::Sequence< css::uno::Reference< css::frame::XFrame > >& seqTasks );

        //---------------------------------------------------------------------------------------------------------
        //  XInterface
        //---------------------------------------------------------------------------------------------------------

        DECLARE_XINTERFACE
        DECLARE_XTYPEPROVIDER

        //---------------------------------------------------------------------------------------------------------
        //  XEventListener
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      last chance to release all references and free memory
            @descr      This method is called, if the enumeration is used completly and has no more elements.
                        Then we must destroy ouer list and release all references to other objects.

            @seealso    interface XEventListener

            @param      "aEvent" describe the source of this event.
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual void SAL_CALL disposing( const css::lang::EventObject& aEvent ) throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //  XEnumeration
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      check count of accessible elements of enumeration
            @descr      You can call this method to get information about accessible elements in future.
                        Elements you have already getted are not accessible!

            @seealso    interface XEnumeration

            @param      -
            @return     sal_True  = if more elements accessible<BR>
                        sal_False = other way

            @onerror    sal_False<BR>
                        (List is emtpy and there no accessible elements ...)
        *//*-*****************************************************************************************************/

        virtual sal_Bool SAL_CALL hasMoreElements() throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      give the next element, if some exist
            @descr      If a call "hasMoreElements()" return true, you can get the next element of list.

            @seealso    interface XEnumeration

            @param      -
            @return     A Reference to a task, safed in an Any-structure.

            @onerror    If end of enumeration is arrived or there are no elements in list => a NoSuchElementException is thrown.
        *//*-*****************************************************************************************************/

        virtual css::uno::Any SAL_CALL nextElement() throw( css::container::NoSuchElementException  ,
                                                             css::lang::WrappedTargetException      ,
                                                            css::uno::RuntimeException              );

    //-------------------------------------------------------------------------------------------------------------
    //  protected methods
    //-------------------------------------------------------------------------------------------------------------

    protected:

        /*-****************************************************************************************************//**
            @short      standard destructor
            @descr      This method destruct an instance of this class and clear some member.
                        We make it protected, because its not supported to use this class as normal instance!
                        You must create it dynamical in memory and use a pointer.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual ~OTasksEnumeration();

        /*-****************************************************************************************************//**
            @short      reset instance to default values

            @descr      There are two ways to delete an instance of this class.<BR>
                        1) delete with destructor<BR>
                        2) dispose from parent or factory ore ...<BR>
                        This method do the same for both ways! It free used memory and release references ...

            @seealso    method dispose()
            @seealso    destructor ~TaskEnumeration()

            @param      -

            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual void impl_resetObject();

    //-------------------------------------------------------------------------------------------------------------
    //  private methods
    //-------------------------------------------------------------------------------------------------------------

    private:

    //-------------------------------------------------------------------------------------------------------------
    //  debug methods
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      debug-method to check incoming parameter of some other mehods of this class
            @descr      The following methods are used to check parameters for other methods
                        of this class. The return value is used directly for an ASSERT(...).

            @seealso    ASSERT in implementation!

            @param      references to checking variables
            @return     sal_False on invalid parameter<BR>
                        sal_True  otherway

            @onerror    -
        *//*-*****************************************************************************************************/

    #ifdef ENABLE_ASSERTIONS

    private:

        static sal_Bool impldbg_checkParameter_OTasksEnumerationCtor    (   const   css::uno::Sequence< css::uno::Reference< css::frame::XFrame > >&    seqTasks    );
        static sal_Bool impldbg_checkParameter_disposing                (   const   css::lang::EventObject&                                             aEvent      );

    #endif  // #ifdef ENABLE_ASSERTIONS

    //-------------------------------------------------------------------------------------------------------------
    //  variables
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------

    private:

        sal_uInt32                                                          m_nPosition         ;   /// current position in enumeration
        css::uno::Sequence< css::uno::Reference< css::frame::XTask > >      m_seqTasks          ;   /// list of current tasks

};      //  class OTasksEnumeration

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_HELPER_OTASKSENUMERATION_HXX_
