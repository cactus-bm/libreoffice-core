/*************************************************************************
 *
 *  $RCSfile: pluginframe.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: cd $ $Date: 2002-05-16 13:28:34 $
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

#ifndef __FRAMEWORK_SERVICES_PLUGINFRAME_HXX_
#define __FRAMEWORK_SERVICES_PLUGINFRAME_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_SERVICES_TASK_HXX_
#include <services/task.hxx>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_MOZILLA_XPLUGININSTANCE_HPP_
#include <com/sun/star/mozilla/XPluginInstance.hpp>
#endif

#ifndef _COM_SUN_STAR_MOZILLA_XPLUGININSTANCEPEER_HPP_
#include <com/sun/star/mozilla/XPluginInstancePeer.hpp>
#endif

#ifndef _COM_SUN_STAR_MOZILLA_XPLUGINWINDOWPEER_HPP_
#include <com/sun/star/mozilla/XPluginWindowPeer.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHRESULTLISTENER_HPP_
#include <com/sun/star/frame/XDispatchResultListener.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_FEATURESTATEEVENT_HPP_
#include <com/sun/star/frame/FeatureStateEvent.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_EVENTOBJECT_HPP_
#include <com/sun/star/lang/EventObject.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _VCL_ATOM_HXX
#include <vcl/threadex.hxx>
#endif

#ifndef INCLUDED_SVTOOLS_CMDOPTIONS_HXX
#include <svtools/cmdoptions.hxx>
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
    @short      implements an special frame - a plugin frame
    @descr      -

    @implements XInitialization
                XPluginInstance
                XDispatchResultListener
    @base       Task
*//*-*************************************************************************************************************/

class PlugInFrame   :   public css::lang::XInitialization   ,
                        public css::mozilla::XPluginInstance,
                        public css::frame::XDispatchResultListener  ,   // => XEVENTLISTENER
                        public Task                             // Order of baseclasses is neccessary for right initialization!
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------

    public:

        //---------------------------------------------------------------------------------------------------------
        //  constructor / destructor
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      standard constructor to create instance
            @descr      This constructor initialize a new instance of this class,
                        and will be set valid values on his member and baseclasses.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

         PlugInFrame( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory );

        /*-****************************************************************************************************//**
            @short      standard destructor
            @descr      This method destruct an instance of this class and clear some member.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual ~PlugInFrame();

        //---------------------------------------------------------------------------------------------------------
        //  XInterface, XTypeProvider, XServiceInfo
        //---------------------------------------------------------------------------------------------------------

        DECLARE_XINTERFACE
        DECLARE_XTYPEPROVIDER
        DECLARE_XSERVICEINFO

        //---------------------------------------------------------------------------------------------------------
        //  XInitialization
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL initialize( const css::uno::Sequence< css::uno::Any >& seqArguments ) throw(  css::uno::Exception         ,
                                                                                                     css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //  XPluginInstance
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL start         () throw( css::uno::RuntimeException );
        void SAL_CALL implcb_start  () throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL stop          () throw( css::uno::RuntimeException );
        void SAL_CALL implcb_stop   () throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL destroy       () throw( css::uno::RuntimeException );
        void SAL_CALL implcb_destroy() throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL createWindow          (   const   css::uno::Any&      aPlatformWindowHandle   ,
                                                        sal_Bool            bEmbedded               ) throw( css::uno::RuntimeException );
        void SAL_CALL implcb_createWindow   (   const   css::uno::Any&      aPlatformWindowHandle   ,
                                                        sal_Bool            bEmbedded               ) throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL newStream     (   const   ::rtl::OUString&                                sMIMEDescription,
                                        const   ::rtl::OUString&                                sURL            ,
                                        const   ::rtl::OUString&                                sFilter         ,
                                        const   css::uno::Reference< css::io::XInputStream >&   xStream         ,
                                        const   css::uno::Any&                                  aSessionId      ) throw( css::uno::RuntimeException );
        void SAL_CALL implcb_newStream
                                    (   const   ::rtl::OUString&                                sMIMEDescription,
                                        const   ::rtl::OUString&                                sURL            ,
                                        const   ::rtl::OUString&                                sFilter         ,
                                        const   css::uno::Reference< css::io::XInputStream >&   xStream         ,
                                        const   css::uno::Any&                                  aSessionId      ) throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL newURL        (   const   ::rtl::OUString&    sMIMEDescription,
                                        const   ::rtl::OUString&    sURL            ,
                                        const   ::rtl::OUString&    sFilter         ,
                                        const   css::uno::Any&      aSessionId      ) throw( css::uno::RuntimeException );
        void SAL_CALL implcb_newURL (   const   ::rtl::OUString&    sMIMEDescription,
                                        const   ::rtl::OUString&    sURL            ,
                                        const   ::rtl::OUString&    sFilter         ,
                                        const   css::uno::Any&      aSessionId      ) throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual void SAL_CALL getHttpServerURL( ::rtl::OUString&    sHost   ,
                                                sal_uInt16&         nPort   ,
                                                ::rtl::OUString&    sPrefix ) throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //   XDispatchProvider
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual css::uno::Reference< css::frame::XDispatch > SAL_CALL queryDispatch(    const   css::util::URL&     aURL            ,
                                                                                        const   ::rtl::OUString&    sTargetFrameName,
                                                                                                sal_Int32           nSearchFlags    ) throw( css::uno::RuntimeException );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL queryDispatches( const css::uno::Sequence< css::frame::DispatchDescriptor >& seqDescripts ) throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //   XDispatchResultListener
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual void SAL_CALL dispatchFinished      ( const css::frame::DispatchResultEvent&                    aEvent     ) throw( css::uno::RuntimeException );

        //---------------------------------------------------------------------------------------------------------
        //   XEventListener
        //---------------------------------------------------------------------------------------------------------

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        void SAL_CALL disposing( const css::lang::EventObject& aEvent ) throw( css::uno::RuntimeException );

    //-------------------------------------------------------------------------------------------------------------
    //  protected methods
    //-------------------------------------------------------------------------------------------------------------

    protected:

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

        void impl_tryToLoadDocument();
        sal_Bool impl_registerRemoteFactories( const css::uno::Reference< css::lang::XMultiServiceFactory >& xRemoteServiceManager );

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

        static sal_Bool impldbg_checkParameter_initialize           (   const   css::uno::Sequence< css::uno::Any >&                    seqArguments            );
        static sal_Bool impldbg_checkParameter_createWindow         (   const   css::uno::Any&                                          aPlatformWindowHandle   ,
                                                                                sal_Bool                                                bEmbedded               );
        static sal_Bool impldbg_checkParameter_newStream            (   const   ::rtl::OUString&                                        sMIMEDescription        ,
                                                                        const   ::rtl::OUString&                                        sURL                    ,
                                                                        const   ::rtl::OUString&                                        sFilter                 ,
                                                                        const   css::uno::Reference< css::io::XInputStream >&           xStream                 );
        static sal_Bool impldbg_checkParameter_newURL               (   const   ::rtl::OUString&                                        sMIMEDescription        ,
                                                                        const   ::rtl::OUString&                                        sURL                    ,
                                                                        const   ::rtl::OUString&                                        sFilter                 );
        static sal_Bool impldbg_checkParameter_getHttpServerURL     (           ::rtl::OUString&                                        sHost                   ,
                                                                                sal_uInt16&                                             nPort                   ,
                                                                                ::rtl::OUString&                                        sPrefix                 );
        static sal_Bool impldbg_checkParameter_queryDispatch        (   const   css::util::URL&                                         aURL                    ,
                                                                        const   ::rtl::OUString&                                        sTargetFrameName        ,
                                                                                sal_Int32                                               nSearchFlags            );
        static sal_Bool impldbg_checkParameter_queryDispatches      (   const   css::uno::Sequence< css::frame::DispatchDescriptor >&   seqDescripts            );
        static sal_Bool impldbg_checkParameter_statusChanged        (   const   css::frame::FeatureStateEvent&                          aEvent                  );
        static sal_Bool impldbg_checkParameter_disposing            (   const   css::lang::EventObject&                                 aEvent                  );

    #endif  // #ifdef ENABLE_ASSERTIONS

    //-------------------------------------------------------------------------------------------------------------
    //  variables
    //  (should be private everyway!)
    //-------------------------------------------------------------------------------------------------------------

    private:

        css::uno::Reference< css::mozilla::XPluginInstancePeer >            m_xPlugInInstancePeer   ;   /// Reference to UNO interface of PlugIn dll for communication with browser
        css::uno::Reference< css::mozilla::XPluginWindowPeer >              m_xPlugInWindowPeer     ;   /// Reference to set child window at plugin window
        css::uno::Sequence< css::beans::PropertyValue >                     m_seqProperties         ;   /// Sequence of properties as arguments for load document
        css::util::URL                                                      m_aURL                  ;   /// URL for document to load
        sal_Bool                                                            m_bILoad                ;   /// PlugInFrame has a valid loader which load the document and wait for finished/cancelled
        sal_Bool                                                            m_bIHaveDocument        ;   /// We have a document loaded successful.
        css::uno::Reference< css::frame::XDispatchProvider >                m_xPlugInDispatcher     ;   /// Dispatcher to forward dispatches to browser
        static sal_Bool                                                     m_bRemoteFactoriesExist ;   /// indicates, if remote factories was already registered (neccessary one times only!)
        SvtCommandOptions                                                   m_aCommandOptions       ;   /// ref counted class to support disabling commands defined by configuration file

};      //  class PlugInFrame

/*-************************************************************************************************************//**
    @short      used to forward all asynchronous calls which use VCL internal to the main thread
    @descr      We need this asynchronous mechanism to prevent us against dead locks. Sometimes our main thread
                can call us for event handling like FOCUS, ACTIVATE and something else. But at the same time we will call
                the main thread to CREATE A WINDOW, SET IT VISIBLE ... Then we have a problem. We must send us himself a event
                to run our code synchronized with our main thread!

    @implements
    @base       -
*//*-*************************************************************************************************************/

enum eIMPL_PluginCommand
{
    START           ,
    STOP            ,
    CREATEWINDOW    ,
    DESTROY         ,
    NEWSTREAM       ,
    NEWURL
};

class cIMPL_MainThreadExecutorRequest
{
    public:
        cIMPL_MainThreadExecutorRequest(    eIMPL_PluginCommand                             eCommand                ,
                                            PlugInFrame*                                    pPluginInstance         );

        cIMPL_MainThreadExecutorRequest(    eIMPL_PluginCommand                             eCommand                ,
                                            PlugInFrame*                                    pPluginInstance         ,
                                    const   css::uno::Any&                                  aPlatformWindowHandle   ,
                                            sal_Bool                                        bEmbedded               );

        cIMPL_MainThreadExecutorRequest(    eIMPL_PluginCommand                             eCommand                ,
                                            PlugInFrame*                                    pPluginInstance         ,
                                    const   ::rtl::OUString&                                sMIMEDescription        ,
                                    const   ::rtl::OUString&                                sURL                    ,
                                    const   ::rtl::OUString&                                sFilter                 ,
                                    const   css::uno::Reference< css::io::XInputStream >&   xStream                 ,
                                    const   css::uno::Any&                                  aSessionId              );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/

        virtual long doIt();

    //-------------------------------------------------------------------------------------------------------------
    //  private variables
    //-------------------------------------------------------------------------------------------------------------
    private:

        eIMPL_PluginCommand                             m_eCommand              ;   /// switch to specify forward function
        PlugInFrame*                                    m_pPluginInstance       ;   /// instance wich has started this swicth mechanism and wish to called back from us
        css::uno::Reference< css::uno::XInterface >     m_xPluginInstance       ;   /// reference to our plugin frame to prevent that plugin frame dies before we call back
        css::uno::Any                                   m_aPlatformWindowHandle ;   /// parameter for XPluginInstance->createWindow()
        sal_Bool                                        m_bEmbedded             ;   /// parameter for XPluginInstance->createWindow()
        ::rtl::OUString                                 m_sMIMEDescription      ;   /// parameter for XPluginInstance->newStream()/newURL()
        ::rtl::OUString                                 m_sURL                  ;   /// parameter for XPluginInstance->newStream()/newURL()
        ::rtl::OUString                                 m_sFilter               ;   /// parameter for XPluginInstance->newStream()/newURL()
        css::uno::Reference< css::io::XInputStream >    m_xStream               ;   /// parameter for XPluginInstance->newStream()/newURL()
        css::uno::Any                                   m_aSessionId            ;   /// parameter for XPluginInstance->newStream()/newURL()
};

class cIMPL_MainThreadExecutor
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------
    public:
        static void execute( cIMPL_MainThreadExecutorRequest* pRequest );

        /*-****************************************************************************************************//**
            @short      -
            @descr      -

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        DECL_STATIC_LINK( cIMPL_MainThreadExecutor, worker, cIMPL_MainThreadExecutorRequest* );
};

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_SERVICES_PLUGINFRAME_HXX_
