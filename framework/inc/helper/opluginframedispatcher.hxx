/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: opluginframedispatcher.hxx,v $
 * $Revision: 1.8 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef __FRAMEWORK_HELPER_OPLUGINFRAMEDISPATCHER_HXX_
#define __FRAMEWORK_HELPER_OPLUGINFRAMEDISPATCHER_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/debug.hxx>
#include <general.h>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/mozilla/XPluginInstance.hpp>
#include <com/sun/star/mozilla/XPluginInstancePeer.hpp>
#include <com/sun/star/mozilla/XPluginInstanceNotifySink.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/beans/PROPERTYVALUE.hpp>

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/weakref.hxx>
#include <cppuhelper/interfacecontainer.hxx>

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
    Use OMultiTypeInterfaceContainerHelperVar-template to create new class to get a container
    to combine string values with listeners.
*//*-*************************************************************************************************************/

struct IMPL_hashCode
{
    size_t operator()(const ::rtl::OUString& sString) const
    {
        return sString.hashCode();
    }
};

typedef ::cppu::OMultiTypeInterfaceContainerHelperVar<  ::rtl::OUString                     ,
                                                        IMPL_hashCode                       ,
                                                        ::std::equal_to< ::rtl::OUString > > IMPL_ListenerContainerHelper ;

/*-************************************************************************************************************//**
    @short          -
    @descr          -

    @implements     XInterface
                    XDispatchProvider
                    XDispatch
                    XPluginInstanceNotifySink
    @base           OWeakObject

    @devstatus      ready to use
*//*-*************************************************************************************************************/

class OPlugInFrameDispatcher    :   private ThreadHelpBase                          ,
                                    public ::cppu::WeakImplHelper3< ::com::sun::star::frame::XDispatchProvider, ::com::sun::star::frame::XDispatch, ::com::sun::star::mozilla::XPluginInstanceNotifySink >
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------

    public:

        static void                                                     AddArguments    (   const   css::uno::Sequence< css::beans::PropertyValue > &   rArgs   ,
                                                                                            const   ::rtl::OUString&                                    rURL    );
        static const css::uno::Sequence < css::beans::PropertyValue >*  GetArguments    (   const   ::rtl::OUString&                                    rURL    );
        static void                                                     RemoveArguments (   const   ::rtl::OUString&                                    rURL    );

        //---------------------------------------------------------------------------------------------------------
        //  constructor / destructor
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      standard ctor
            @descr      These initialize a new instance of ths class with needed informations for work.

            @seealso    using at owner

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

         OPlugInFrameDispatcher(    const   css::uno::Reference< css::lang::XMultiServiceFactory >&     xFactory    ,
                                const   css::uno::Reference< css::mozilla::XPluginInstance >&       xOwner      ,
                                const   css::uno::Reference< css::mozilla::XPluginInstancePeer >&   xPlugInDLL  );

        //---------------------------------------------------------------------------------------------------------
        //  XDispatchProvider
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      search a dispatcher for given URL
            @descr      If no interceptor is set on owner, we search for right frame and dispatch URL to it.
                        If no frame was found, we do nothing.

            @seealso    -

            @param      "aURL"              , URL to dispatch.
            @param      "sTargetFrameName"  , name of searched frame.
            @param      "nSearchFlags"      , flags for searching.
            @return     A reference a dispatcher for these URL (if someone was found!).

            @onerror    We return a NULL-reference.
        *//*-*****************************************************************************************************/

        virtual css::uno::Reference< css::frame::XDispatch > SAL_CALL queryDispatch(    const   css::util::URL&     aURL                ,
                                                                                        const   ::rtl::OUString&    sTargetFrameName    ,
                                                                                                sal_Int32           nSearchFlags        ) throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      search dispatcher for more the one URL
            @descr      Call this method, if you search dispatcher for more then one URL at the same time.
                        ( But these mode is not supported yet! )

            @seealso    -

            @param      "seqDescripts", descriptor to search more then one dispatcher.
            @return     A list of founded dispatcher. (if somewhere exist!)

            @onerror    An empty list is returned.
        *//*-*****************************************************************************************************/

        virtual css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL queryDispatches( const css::uno::Sequence< css::frame::DispatchDescriptor >& seqDescripts ) throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //  XDispatch
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      dispatch URL with arguments
            @descr      This class implement XDispatch interface to. We use the implementation of ouer owner
                        directly to do this. He must support a function impl_loadComponent().

            @seealso    methode <owner>::impl_loadComponent()

            @param      "aURL"          , URL to dispatch.
            @param      "seqArguments"  , list of optional arguments.
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual void SAL_CALL dispatch( const   css::util::URL&                                     aURL        ,
                                        const   css::uno::Sequence< css::beans::PropertyValue >&    seqArguments) throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      add listener for state events
            @descr      -

            @seealso    -

            @param      "xControl"  , reference to a valid listener for state events.
            @param      "aURL"      , URL about listener will be informed, if something occured.
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual void SAL_CALL addStatusListener(    const   css::uno::Reference< css::frame::XStatusListener >& xControl,
                                                    const   css::util::URL&                                     aURL    ) throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      remove listener
            @descr      -

            @seealso    -

            @param      "xControl"  , reference to a valid listener.
            @param      "aURL"      , URL on which listener has registered.
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual void SAL_CALL removeStatusListener( const   css::uno::Reference< css::frame::XStatusListener >& xControl,
                                                    const   css::util::URL&                                     aURL    ) throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //  XPluginInstanceNotifySink
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL notifyURL( const ::rtl::OUString& sURL ) throw( css::uno::RuntimeException );

    //-------------------------------------------------------------------------------------------------------------
    //  protected methods
    //-------------------------------------------------------------------------------------------------------------

    protected:

        /*-****************************************************************************************************//**
            @short      standard destructor
            @descr      This method destruct an instance of this class and clear some member.
                        This method is protected, because its not allowed to use an instance of this class as a member!
                        You MUST use a pointer.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual ~OPlugInFrameDispatcher();

    //-------------------------------------------------------------------------------------------------------------
    //  private methods
    //-------------------------------------------------------------------------------------------------------------

    private:

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void impl_sendStatusEvent(  const   ::rtl::OUString&    sURL                ,
                                            sal_Bool            bLoadingSuccessful  );

    //-------------------------------------------------------------------------------------------------------------
    //  debug methods
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      debug-method to check incoming parameter of some other mehods of this class
            @descr      The following methods are used to check parameters for other methods
                        of this class. The return value is used directly for an ASSERT(...).

            @seealso    ASSERTs in implementation!

            @param      references to checking variables
            @return     sal_False on invalid parameter<BR>
                        sal_True  otherway

            @onerror    -
        *//*-*****************************************************************************************************/

    #ifdef ENABLE_ASSERTIONS

    private:

         static sal_Bool impldbg_checkParameter_OPlugInFrameDispatcherCtor  (   const   css::uno::Reference< css::lang::XMultiServiceFactory >&     xFactory            ,
                                                                                const   css::uno::Reference< css::mozilla::XPluginInstance >&       xOwner              ,
                                                                                const   css::uno::Reference< css::mozilla::XPluginInstancePeer >&   xPlugInDLL          );
        static sal_Bool impldbg_checkParameter_queryDispatch                (   const   css::util::URL&                                             aURL                ,
                                                                                const   ::rtl::OUString&                                            sTargetFrameName    ,
                                                                                        sal_Int32                                                   nSearchFlags        );
        static sal_Bool impldbg_checkParameter_queryDispatches              (   const   css::uno::Sequence< css::frame::DispatchDescriptor >&       seqDescriptor       );
        static sal_Bool impldbg_checkParameter_dispatch                     (   const   css::util::URL&                                             aURL                ,
                                                                                const   css::uno::Sequence< css::beans::PropertyValue >&            seqArguments        );
        static sal_Bool impldbg_checkParameter_addStatusListener            (   const   css::uno::Reference< css::frame::XStatusListener >&         xControl            ,
                                                                                const   css::util::URL&                                             aURL                );
        static sal_Bool impldbg_checkParameter_removeStatusListener         (   const   css::uno::Reference< css::frame::XStatusListener >&         xControl            ,
                                                                                const   css::util::URL&                                             aURL                );
        static sal_Bool impldbg_checkParameter_notifyURL                    (   const   ::rtl::OUString&                                            sURL                );

    #endif  // #ifdef ENABLE_ASSERTIONS

    //-------------------------------------------------------------------------------------------------------------
    //  variables
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------

    private:

        css::uno::Reference< css::lang::XMultiServiceFactory >          m_xFactory              ;   /// servicemanager to create uno services
        css::uno::WeakReference< css::mozilla::XPluginInstance >        m_xOwnerWeak            ;   /// weakreference to owner (Don't use a hard reference. Owner can't delete us then!)
        css::uno::Reference< css::mozilla::XPluginInstancePeer >        m_xPlugInDLL            ;   /// Interface for communication with browser.
        ::rtl::OUString                                                 m_sTargetFrameName      ;   /// We need it as target for get/postURL.
        IMPL_ListenerContainerHelper                                    m_aListenerContainer    ;   /// Container to combine listeners and URLs.

};      //  class OPlugInFrameDispatcher

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_HELPER_OPLUGINFRAMEDISPATCHER_HXX_
