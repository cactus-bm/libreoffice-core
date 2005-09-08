/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: blankdispatcher.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:11:09 $
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

#ifndef __FRAMEWORK_DISPATCH_BLANKDISPATCHER_HXX_
#define __FRAMEWORK_DISPATCH_BLANKDISPATCHER_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_DISPATCH_BASEDISPATCHER_HXX_
#include <dispatch/basedispatcher.hxx>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

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
    @short          helper for desktop only(!) to create new tasks on demand for dispatches
    @descr          Use this class as member only! Never use it as baseclass.
                    XInterface will be ambigous and we hold a weakreference to ouer OWNER - not to ouer SUPERCLASS!

    @implements     XInterface
                    XDispatch
                    XStatusListener
                    XLoadEventListener
                    XEventListener

    @base           BaseDispatcher
                    OWeakObject

    @devstatus      ready to use
    @threadsafe     yes
*//*-*************************************************************************************************************/
class BlankDispatcher   :   // -interfaces  ... are supported by our BaseDispatcher!
                            // -baseclasses ... order is neccessary for right initialization!
                            public BaseDispatcher
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------
    public:
        //  ctor
                              BlankDispatcher     ( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory    ,
                                                    const css::uno::Reference< css::frame::XFrame >&              xDesktop,
                                                    sal_Bool bIsDefaultDispatcher    );
        //  XDispatch
        virtual void SAL_CALL dispatch            ( const css::util::URL&                                         aURL        ,
                                                    const css::uno::Sequence< css::beans::PropertyValue >&        lArguments  ) throw( css::uno::RuntimeException );

    //-------------------------------------------------------------------------------------------------------------
    //  protected methods
    //-------------------------------------------------------------------------------------------------------------
    protected:
        virtual void SAL_CALL reactForLoadingState ( const css::util::URL&                                        aURL        ,
                                                     const css::uno::Sequence< css::beans::PropertyValue >&       lDescriptor ,
                                                     const css::uno::Reference< css::frame::XFrame >&             xTarget     ,
                                                           sal_Bool                                               bState      ,
                                                     const css::uno::Any&                                         aAsyncInfo  );

        virtual void SAL_CALL reactForHandlingState( const css::util::URL&                                        aURL        ,
                                                     const css::uno::Sequence< css::beans::PropertyValue >&       lDescriptor ,
                                                           sal_Bool                                               bState      ,
                                                     const css::uno::Any&                                         aAsyncInfo  );

    //-------------------------------------------------------------------------------------------------------------
    //  protected methods
    //-------------------------------------------------------------------------------------------------------------
    private:
        sal_Bool implts_findAndActivateAlreadyLoadedTask( const css::util::URL&                                   aURL       ,
                                                          const css::uno::Sequence< css::beans::PropertyValue >&  lArguments ,
                                                          const css::uno::Reference< css::frame::XFrame >         xDesktop   );

        css::uno::Reference< css::frame::XFrame > implts_findAndLockRecycleTask( const css::util::URL&                                   aURL       ,
                                                                                 const css::uno::Sequence< css::beans::PropertyValue >&  lArguments ,
                                                                                 const css::uno::Reference< css::frame::XFrame >         xDesktop   );

    //-------------------------------------------------------------------------------------------------------------
    //  variables
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------
    private:
        sal_Bool        m_bIsDefaultDispatcher;
};      //  class BlankDispatcher

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_DISPATCH_BLANKDISPATCHER_HXX_
