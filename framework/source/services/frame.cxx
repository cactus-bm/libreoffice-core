/*************************************************************************
 *
 *  $RCSfile: frame.cxx,v $
 *
 *  $Revision: 1.63 $
 *
 *  last change: $Author: mba $ $Date: 2002-10-24 12:25:54 $
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

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_SERVICES_FRAME_HXX_
#include <services/frame.hxx>
#endif

#ifndef __FRAMEWORK_DISPATCH_DISPATCHPROVIDER_HXX_
#include <dispatch/dispatchprovider.hxx>
#endif

#ifndef __FRAMEWORK_DISPATCH_INTERCEPTIONHELPER_HXX_
#include <dispatch/interceptionhelper.hxx>
#endif

#ifndef __FRAMEWORK_HELPER_COMPONENTLOADER_HXX_
#include <helper/componentloader.hxx>
#endif

#ifndef __FRAMEWORK_HELPER_OFRAMES_HXX_
#include <helper/oframes.hxx>
#endif

#ifndef __FRAMEWORK_HELPER_STATUSINDICATORFACTORY_HXX_
#include <helper/statusindicatorfactory.hxx>
#endif

#ifndef __FRAMEWORK_CLASSES_TARGETFINDER_HXX_
#include <classes/targetfinder.hxx>
#endif

#ifndef __FRAMEWORK_CLASSES_ARGUMENTANALYZER_HXX_
#include <classes/argumentanalyzer.hxx>
#endif

#ifndef __FRAMEWORK_CLASSES_FILTERCACHE_HXX_
#include <classes/filtercache.hxx>
#endif

#ifndef __FRAMEWORK_CLASSES_DROPTARGETLISTENER_HXX_
#include <classes/droptargetlistener.hxx>
#endif

#ifndef __FRAMEWORK_CLASSES_TASKCREATOR_HXX_
#include <classes/taskcreator.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_TRANSACTIONGUARD_HXX_
#include <threadhelp/transactionguard.hxx>
#endif

#ifndef __FRAMEWORK_SERVICES_H_
#include <services.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_TASK_XJOBEXECUTOR_HPP_
#include <com/sun/star/task/XJobExecutor.hpp>
#endif

#ifndef _COM_SUN_STAR_MOZILLA_XPLUGININSTANCE_HPP_
#include <com/sun/star/mozilla/XPluginInstance.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XDEVICE_HPP_
#include <com/sun/star/awt/XDevice.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XTOPWINDOW_HPP_
#include <com/sun/star/awt/XTopWindow.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCHFLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XWINDOWPEER_HPP_
#include <com/sun/star/awt/XWindowPeer.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XVCLWINDOWPEER_HPP_
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORSUPPLIER_HPP_
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XDATATRANSFERPROVIDERACCESS_HPP_
#include <com/sun/star/awt/XDataTransferProviderAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDROPTARGET_HPP_
#include <com/sun/star/datatransfer/dnd/XDropTarget.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_WINDOWATTRIBUTE_HPP_
#include <com/sun/star/awt/WindowAttribute.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________

#ifndef _CPPUHELPER_QUERYINTERFACE_HXX_
#include <cppuhelper/queryinterface.hxx>
#endif

#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif

#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif

#ifndef _CPPUHELPER_PROPTYPEHLP_HXX
#include <cppuhelper/proptypehlp.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif

#ifndef _SV_WRKWIN_HXX
#include <vcl/wrkwin.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif

#ifndef _TOOLKIT_AWT_VCLXWINDOW_HXX_
#include <toolkit/awt/vclxwindow.hxx>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif

#ifdef ENABLE_ASSERTIONS
    #ifndef _RTL_STRBUF_HXX_
    #include <rtl/strbuf.hxx>
    #endif
#endif

#include <vcl/menu.hxx>

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//  non exported const
//_________________________________________________________________________________________________________________

#define PROPERTYNAME_TITLE                          DECLARE_ASCII("Title"                   )
#define PROPERTYNAME_DISPATCHRECORDERSUPPLIER       DECLARE_ASCII("DispatchRecorderSupplier")

#define PROPERTYHANDLE_TITLE                        1
#define PROPERTYHANDLE_DISPATCHRECORDERSUPPLIER     2

#define PROPERTYCOUNT                               2

//_________________________________________________________________________________________________________________
//  non exported definitions
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  declarations
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//  XInterface, XTypeProvider, XServiceInfo
//*****************************************************************************************************************
DEFINE_XINTERFACE_20                (   Frame                                                                   ,
                                        OWeakObject                                                             ,
                                        DIRECT_INTERFACE(css::lang::XTypeProvider                               ),
                                        DIRECT_INTERFACE(css::lang::XServiceInfo                                ),
                                        DIRECT_INTERFACE(css::frame::XFramesSupplier                            ),
                                        DIRECT_INTERFACE(css::frame::XFrame                                     ),
                                        DIRECT_INTERFACE(css::lang::XComponent                                  ),
                                        DIRECT_INTERFACE(css::task::XStatusIndicatorFactory                     ),
                                        DIRECT_INTERFACE(css::frame::XDispatchProvider                          ),
                                        DIRECT_INTERFACE(dcss::frame::XDispatchInformationProvider              ),
                                        DIRECT_INTERFACE(css::frame::XDispatchProviderInterception              ),
                                        DIRECT_INTERFACE(css::beans::XMultiPropertySet                          ),
                                        DIRECT_INTERFACE(css::beans::XFastPropertySet                           ),
                                        DIRECT_INTERFACE(css::beans::XPropertySet                               ),
                                        DIRECT_INTERFACE(css::awt::XWindowListener                              ),
                                        DIRECT_INTERFACE(css::awt::XTopWindowListener                           ),
                                        DIRECT_INTERFACE(css::awt::XFocusListener                               ),
                                        DERIVED_INTERFACE(css::lang::XEventListener, css::awt::XWindowListener  ),
                                        DIRECT_INTERFACE(css::document::XActionLockable                         ),
                                        DIRECT_INTERFACE(css::util::XCloseable                                  ),
                                        DIRECT_INTERFACE(css::util::XCloseBroadcaster                           ),
                                        DIRECT_INTERFACE(css::frame::XComponentLoader                           )
                                    )

DEFINE_XTYPEPROVIDER_19             (   Frame                                                                   ,
                                        css::lang::XTypeProvider                                                ,
                                        css::lang::XServiceInfo                                                 ,
                                        css::frame::XFramesSupplier                                             ,
                                        css::frame::XFrame                                                      ,
                                        css::lang::XComponent                                                   ,
                                        css::task::XStatusIndicatorFactory                                      ,
                                        css::beans::XMultiPropertySet                                           ,
                                        css::beans::XFastPropertySet                                            ,
                                        css::beans::XPropertySet                                                ,
                                        css::frame::XDispatchProvider                                           ,
                                        dcss::frame::XDispatchInformationProvider                               ,
                                        css::frame::XDispatchProviderInterception                               ,
                                        css::awt::XWindowListener                                               ,
                                        css::awt::XTopWindowListener                                            ,
                                        css::awt::XFocusListener                                                ,
                                        css::lang::XEventListener                                               ,
                                        css::util::XCloseable                                                   ,
                                        css::util::XCloseBroadcaster                                            ,
                                        css::frame::XComponentLoader
                                    )

DEFINE_XSERVICEINFO_MULTISERVICE    (   Frame                                                                   ,
                                        ::cppu::OWeakObject                                                     ,
                                        SERVICENAME_FRAME                                                       ,
                                        IMPLEMENTATIONNAME_FRAME
                                    )

DEFINE_INIT_SERVICE                 (   Frame,
                                        {
                                            /*Attention
                                                I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                                                to create a new instance of this class by our own supported service factory.
                                                see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                                            */

                                            //-------------------------------------------------------------------------------------------------------------
                                            // Initialize a new dispatchhelper-object to handle dispatches.
                                            // We use these helper as slave for our interceptor helper ... not directly!
                                            // But he is event listener on THIS instance!
                                            DispatchProvider* pDispatchHelper = new DispatchProvider( m_xFactory, this );
                                            css::uno::Reference< css::frame::XDispatchProvider > xDispatchProvider( static_cast< ::cppu::OWeakObject* >(pDispatchHelper), css::uno::UNO_QUERY );

                                            //-------------------------------------------------------------------------------------------------------------
                                            // Initialize a new interception helper object to handle dispatches and implement an interceptor mechanism.
                                            // Set created dispatch provider as slowest slave of it.
                                            // Hold interception helper by reference only - not by pointer!
                                            // So it's easiear to destroy it.
                                            InterceptionHelper* pInterceptionHelper = new InterceptionHelper( this, xDispatchProvider );
                                            m_xDispatchHelper = css::uno::Reference< css::frame::XDispatchProvider >( static_cast< ::cppu::OWeakObject* >(pInterceptionHelper), css::uno::UNO_QUERY );

                                            //-------------------------------------------------------------------------------------------------------------
                                            // Initialize a new XFrames-helper-object to handle XIndexAccess and XElementAccess.
                                            // We hold member as reference ... not as pointer too!
                                            // Attention: We share our frame container with this helper. Container is threadsafe himself ... So I think we can do that.
                                            // But look on dispose() for right order of deinitialization.
                                            OFrames* pFramesHelper = new OFrames( m_xFactory, this, &m_aChildFrameContainer );
                                            m_xFramesHelper = css::uno::Reference< css::frame::XFrames >( static_cast< ::cppu::OWeakObject* >(pFramesHelper), css::uno::UNO_QUERY );

                                            //-------------------------------------------------------------------------------------------------------------
                                            // Initialize a the drop target listener.
                                            // We hold member as reference ... not as pointer too!
                                            DropTargetListener* pDropListener = new DropTargetListener( m_xFactory, this );
                                            m_xDropTargetListener = css::uno::Reference< css::datatransfer::dnd::XDropTargetListener >( static_cast< ::cppu::OWeakObject* >(pDropListener), css::uno::UNO_QUERY );

                                            // Safe impossible cases
                                            // We can't work without these helpers!
                                            LOG_ASSERT2( xDispatchProvider.is    ()==sal_False, "Frame::impl_initService()", "Slowest slave for dispatch- and interception helper isn't valid. XDispatchProvider, XDispatch, XDispatchProviderInterception are not full supported!" )
                                            LOG_ASSERT2( m_xDispatchHelper.is    ()==sal_False, "Frame::impl_initService()", "Interception helper isn't valid. XDispatchProvider, XDispatch, XDispatchProviderInterception are not full supported!"                                 )
                                            LOG_ASSERT2( m_xFramesHelper.is      ()==sal_False, "Frame::impl_initService()", "Frames helper isn't valid. XFrames, XIndexAccess and XElementAcces are not supported!"                                                                )
                                            LOG_ASSERT2( m_xDropTargetListener.is()==sal_False, "Frame::impl_initService()", "DropTarget helper isn't valid. Drag and drop without functionality!"                                                                                  )
                                        }
                                    )

/*-****************************************************************************************************//**
    @short      standard constructor to create instance by factory
    @descr      This constructor initialize a new instance of this class by valid factory,
                and will be set valid values on his member and baseclasses.

    @attention  a)  Don't use your own reference during an UNO-Service-ctor! There is no guarantee, that you
                    will get over this. (e.g. using of your reference as parameter to initialize some member)
                    Do such things in DEFINE_INIT_SERVICE() method, which is called automaticly after your ctor!!!
                b)  Baseclass OBroadcastHelper is a typedef in namespace cppu!
                    The microsoft compiler has some problems to handle it right BY using namespace explicitly ::cppu::OBroadcastHelper.
                    If we write it without a namespace or expand the typedef to OBrodcastHelperVar<...> -> it will be OK!?
                    I don't know why! (other compiler not tested .. but it works!)

    @seealso    method DEFINE_INIT_SERVICE()

    @param      "xFactory" is the multi service manager, which create this instance.
                The value must be different from NULL!
    @return     -

    @onerror    ASSERT in debug version or nothing in relaese version.
*//*-*****************************************************************************************************/
Frame::Frame( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory )
        //  init baseclasses first!
        //  Attention: Don't change order of initialization!
        :   ThreadHelpBase              ( &Application::GetSolarMutex()                     )
        ,   TransactionBase             (                                                   )
        ,   ::cppu::OBroadcastHelperVar< ::cppu::OMultiTypeInterfaceContainerHelper, ::cppu::OMultiTypeInterfaceContainerHelper::keyType >           ( m_aLock.getShareableOslMutex()         )
        ,   ::cppu::OPropertySetHelper  ( *(static_cast< ::cppu::OBroadcastHelper* >(this)) )
        ,   ::cppu::OWeakObject         (                                                   )
        //  init member
        ,   m_xFactory                  ( xFactory                                          )
        ,   m_aListenerContainer        ( m_aLock.getShareableOslMutex()                    )
        ,   m_aChildFrameContainer      (                                                   )
        ,   m_xParent                   (                                                   )
        ,   m_xContainerWindow          (                                                   )
        ,   m_xComponentWindow          (                                                   )
        ,   m_xController               (                                                   )
        ,   m_eActiveState              ( E_INACTIVE                                        )
        ,   m_sName                     (                                                   )
        ,   m_bIsFrameTop               ( sal_True                                          ) // I think we are top without a parent ... and there is no parent yet!
        ,   m_bConnected                ( sal_False                                         ) // There exist no component inside of use => sal_False, we are not connected!
        ,   m_nExternalLockCount        ( 0                                                 )
        ,   m_bSelfClose                ( sal_False                                         ) // Important!
        ,   m_bIsPlugIn                 ( sal_False                                         )
        ,   m_aPoster                   ( LINK( this, Frame, implts_windowClosing )         )
{
    // Check incoming parameter to avoid against wrong initialization.
    LOG_ASSERT2( implcp_ctor( xFactory ), "Frame::Frame()", "Invalid parameter detected!" )

    /* Please have a look on "@attentions" of description before! */
}

/*-****************************************************************************************************//**
    @short      standard destructor
    @descr      This one do NOTHING! Use dispose() instaed of this.

    @seealso    method dispose()

    @param      -
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
Frame::~Frame()
{
    LOG_ASSERT2( m_aTransactionManager.getWorkingMode()!=E_CLOSE, "Frame::~Frame()", "Who forgot to dispose this service?" )
}

/*-************************************************************************************************************//**
    @interface  XComponentLoader
    @short      try to load given URL into a task
    @descr      You can give us some informations about the content, which you will load into a frame.
                We search or create this target for you, make a type detection of given URL and try to load it.
                As result of this operation we return the new created component or nothing, if loading failed.

    @seealso    -

    @param      "sURL"              , URL, which represant the content
    @param      "sTargetFrameName"  , name of target frame or special value like "_self", "_blank" ...
    @param      "nSearchFlags"      , optional arguments for frame search, if target isn't a special one
    @param      "lArguments"        , optional arguments for loading
    @return     A valid component reference, if loading was successfully.
                A null reference otherwise.

    @onerror    We return a null reference.
    @threadsafe yes
*//*-*************************************************************************************************************/
css::uno::Reference< css::lang::XComponent > SAL_CALL Frame::loadComponentFromURL( const ::rtl::OUString&                                 sURL            ,
                                                                                   const ::rtl::OUString&                                 sTargetFrameName,
                                                                                         sal_Int32                                        nSearchFlags    ,
                                                                                   const css::uno::Sequence< css::beans::PropertyValue >& lArguments      ) throw( css::io::IOException                ,
                                                                                                                                                                   css::lang::IllegalArgumentException ,
                                                                                                                                                                   css::uno::RuntimeException          )
{
    /* SAFE { */
    ReadGuard aReadLock(m_aLock);
    ComponentLoader* pLoader = new ComponentLoader(m_xFactory,this);
    aReadLock.unlock();
    /* } SAFE */

    css::uno::Reference< css::frame::XComponentLoader > xLoader(static_cast< ::cppu::OWeakObject* >(pLoader), css::uno::UNO_QUERY);
    return xLoader->loadComponentFromURL(sURL,sTargetFrameName,nSearchFlags,lArguments);
}

/*-****************************************************************************************************//**
    @short      return access to append or remove childs on desktop
    @descr      We don't implement these interface directly. We use a helper class to do this.
                If you wish to add or delete childs to/from the container, call these method to get
                a reference to the helper.

    @seealso    class OFrames

    @param      -
    @return     A reference to the helper which answer your queries.

    @onerror    A null reference is returned.
*//*-*****************************************************************************************************/
css::uno::Reference< css::frame::XFrames > SAL_CALL Frame::getFrames() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.

/*TODO
    This is a temp. HACK!
    Our parent (a Task!) stand in close/dispose and set working mode to E_BEFOERECLOSE
    and call dispose on us! We tra to get this xFramesHelper and are reject by an "already closed" pranet instance ....
    => We use SOFTEXCEPTIONS here ... but we should make it right in further times ....
 */

    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    // Return access to all child frames to caller.
    // Ouer childframe container is implemented in helper class OFrames and used as a reference m_xFramesHelper!
    return m_xFramesHelper;
}

/*-****************************************************************************************************//**
    @short      get the current active child frame
    @descr      It must be a frameto. Direct childs of a frame are frames only! No task or desktop is accepted.
                We don't save this information directly in this class. We use ouer container-helper
                to do that.

    @seealso    class OFrameContainer
    @seealso    method setActiveFrame()

    @param      -
    @return     A reference to ouer current active childframe, if anyone exist.
    @return     A null reference, if nobody is active.

    @onerror    A null reference is returned.
*//*-*****************************************************************************************************/
css::uno::Reference< css::frame::XFrame > SAL_CALL Frame::getActiveFrame() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    // Return current active frame.
    // This information is avaliable on the container.
    return m_aChildFrameContainer.getActive();
}

/*-****************************************************************************************************//**
    @short      set the new active direct child frame
    @descr      It must be a frame to. Direct childs of frame are frames only! No task or desktop is accepted.
                We don't save this information directly in this class. We use ouer container-helper
                to do that.

    @seealso    class OFrameContainer
    @seealso    method getActiveFrame()

    @param      "xFrame", reference to new active child. It must be an already existing child!
    @return     -

    @onerror    An assertion is thrown and element is ignored, if given frame is'nt already a child of us.
*//*-*****************************************************************************************************/
void SAL_CALL Frame::setActiveFrame( const css::uno::Reference< css::frame::XFrame >& xFrame ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameters.
    LOG_ASSERT2( implcp_setActiveFrame( xFrame ), "Frame::setActiveFrame()", "Invalid parameter detected!" )
    // Look for rejected calls!
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );

    // Copy neccessary member for threadsafe access!
    // m_aChildFrameContainer is threadsafe himself and he live if we live!!!
    // ...and our transaction is non breakable too ...
    css::uno::Reference< css::frame::XFrame > xActiveChild = m_aChildFrameContainer.getActive();
    EActiveState                              eActiveState = m_eActiveState                    ;

    aWriteLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    // Don't work, if "new" active frame is'nt different from current one!
    // (xFrame==NULL is allowed to UNSET it!)
    if( xActiveChild != xFrame )
    {
        // ... otherwise set new and deactivate old one.
        m_aChildFrameContainer.setActive( xFrame );
        if  (
                ( eActiveState      !=  E_INACTIVE  )   &&
                ( xActiveChild.is() ==  sal_True    )
            )
        {
            xActiveChild->deactivate();
        }
    }

    if( xFrame.is() == sal_True )
    {
        // If last active frame had focus ...
        // ... reset state to ACTIVE and send right FrameActionEvent for focus lost.
        if( eActiveState == E_FOCUS )
        {
            aWriteLock.lock();
            eActiveState   = E_ACTIVE    ;
            m_eActiveState = eActiveState;
            aWriteLock.unlock();
            implts_sendFrameActionEvent( css::frame::FrameAction_FRAME_UI_DEACTIVATING );
        }

        // If last active frame was active ...
        // but new one isn't it ...
        // ... set it as active one.
        if  (
                ( eActiveState          ==  E_ACTIVE    )   &&
                ( xFrame->isActive()    ==  sal_False   )
            )
        {
            xFrame->activate();
        }
    }
    else
    // If this frame is active and has no active subframe anymore it is UI active too
    if( eActiveState == E_ACTIVE )
    {
        aWriteLock.lock();
        eActiveState   = E_FOCUS     ;
        m_eActiveState = eActiveState;
        aWriteLock.unlock();
        implts_sendFrameActionEvent( css::frame::FrameAction_FRAME_UI_ACTIVATED );
    }
}

/*-****************************************************************************************************//**
    @short      initialize frame instance
    @descr      A frame needs a window. This method set a new one ... but should called one times only!
                We use this window to listen for window events and forward it to our set component.
                Its used as parent of component window too.

    @seealso    method getContainerWindow()
    @seealso    method setComponent()
    @seealso    member m_xContainerWindow

    @param      "xWindow", reference to new container window - must be valid!
    @return     -

    @onerror    We do nothing.
*//*-*****************************************************************************************************/
void SAL_CALL Frame::initialize( const css::uno::Reference< css::awt::XWindow >& xWindow ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_initialize( xWindow ), "Frame::initialize()", "Invalid parameter detected!" )

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );

    // Look for rejected calls first!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    // Enable object for real working ... so follow impl methods don't must handle it special! (e.g. E_SOFTEXCEPTIONS for rejected calls)
    m_aTransactionManager.setWorkingMode( E_WORK );

    // This must be the first call of this method!
    // We should initialize our object and open it for working.
    // Set the new window.
    LOG_ASSERT2( m_xContainerWindow.is()==sal_True, "Frame::initialize()", "Leak detected! This state should never occure ..." )
    m_xContainerWindow = xWindow;

    // Now we can use our indicator factory helper to support XStatusIndicatorFactory interface.
    // We have a valid parent window for it!
    // Initialize helper.
    if( m_xContainerWindow.is() == sal_True )
    {
        StatusIndicatorFactory* pIndicatorFactoryHelper = new StatusIndicatorFactory( m_xFactory, this, m_xContainerWindow );
        m_xIndicatorFactoryHelper = css::uno::Reference< css::task::XStatusIndicatorFactory >( static_cast< ::cppu::OWeakObject* >( pIndicatorFactoryHelper ), css::uno::UNO_QUERY );
    }

    // Release lock ... because we call some impl methods, which are threadsafe by himself.
    // If we hold this lock - we will produce our own deadlock!
    aWriteLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    // Start listening for events after setting it on helper class ...
    // So superflous messages are filtered to NULL :-)
    implts_startWindowListening();
}

/*-****************************************************************************************************//**
    @short      returns current set container window
    @descr      The ContainerWindow property is used as a container for the component
                in this frame. So this object implements a container interface too.
                The instantiation of the container window is done by the user of this class.
                The frame is the owner of its container window.

    @seealso    method initialize()

    @param      -
    @return     A reference to current set containerwindow.

    @onerror    A null reference is returned.
*//*-*****************************************************************************************************/
css::uno::Reference< css::awt::XWindow > SAL_CALL Frame::getContainerWindow() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    return m_xContainerWindow;
}

/*-****************************************************************************************************//**
    @short      set parent frame
    @descr      We need a parent to support some functionality! e.g. findFrame()
                By the way we use the chance to set an internal information about our top state.
                So we must not check this information during every isTop() call.
                We are top, if our parent is the desktop instance or we havent any parent.

    @seealso    getCreator()
    @seealso    findFrame()
    @seealso    isTop()
    @seealos    m_bIsFrameTop

    @param      xCreator
                    valid reference to our new owner frame, which should implement a supplier interface

    @threadsafe yes
    @modified   08.05.2002 09:35, as96863
*//*-*****************************************************************************************************/
void SAL_CALL Frame::setCreator( const css::uno::Reference< css::frame::XFramesSupplier >& xCreator ) throw( css::uno::RuntimeException )
{
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE { */
        WriteGuard aWriteLock( m_aLock );
            m_xParent = xCreator;
        aWriteLock.unlock();
    /* } SAFE */

    css::uno::Reference< css::frame::XDesktop > xIsDesktop( xCreator, css::uno::UNO_QUERY );
    m_bIsFrameTop = ( xIsDesktop.is() || ! xCreator.is() );
}

/*-****************************************************************************************************//**
    @short      returns current parent frame
    @descr      The Creator is the parent frame container. If it is NULL, the frame is the uppermost one.

    @seealso    method setCreator()

    @param      -
    @return     A reference to current set parent frame container.

    @onerror    A null reference is returned.
*//*-*****************************************************************************************************/
css::uno::Reference< css::frame::XFramesSupplier > SAL_CALL Frame::getCreator() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    return m_xParent;
}

/*-****************************************************************************************************//**
    @short      returns current set name of frame
    @descr      This name is used to find target of findFrame() or queryDispatch() calls.

    @seealso    method setName()

    @param      -
    @return     Current set name of frame.

    @onerror    An empty string is returned.
*//*-*****************************************************************************************************/
::rtl::OUString SAL_CALL Frame::getName() throw( css::uno::RuntimeException )
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );
    return m_sName;
    /* } SAFE */
}

/*-****************************************************************************************************//**
    @short      set new name for frame
    @descr      This name is used to find target of findFrame() or queryDispatch() calls.

    @attention  Special names like "_blank", "_self" aren't allowed ...
                "_beamer" or "_menubar" excepts this rule!

    @seealso    method getName()

    @param      "sName", new frame name.
    @return     -

    @onerror    We do nothing.
*//*-*****************************************************************************************************/
void SAL_CALL Frame::setName( const ::rtl::OUString& sName ) throw( css::uno::RuntimeException )
{
    /* SAFE { */
    WriteGuard aWriteLock( m_aLock );
    // Set new name ... but look for invalid special target names!
    // They are not allowed to set.
    m_sName = sName;
    impl_filterSpecialTargets( m_sName );
    aWriteLock.unlock();
    /* } SAFE */
}

/*-****************************************************************************************************//**
    @short      search for frames
    @descr      This method searches for a frame with the specified name.
                Frames may contain other frames (e.g. a frameset) and may
                be contained in other frames. This hierarchie ist searched by
                this method.
                First some special names are taken into account, i.e. "",
                "_self", "_top", "_blank" etc. The nSearchFlags are ignored
                when comparing these names with sTargetFrameName, further steps are
                controlled by the search flags. If allowed, the name of the frame
                itself is compared with the desired one, then ( again if allowed )
                the method findFrame() is called for all children, for siblings
                and as last for the parent frame.
                If no frame with the given name is found until the top frames container,
                a new top one is created, if this is allowed by a special
                flag. The new frame also gets the desired name.

    @param      sTargetFrameName
                    special names (_blank, _self) or real name of target frame
    @param      nSearchFlags
                    optional flags which regulate search for non special target frames

    @return     A reference to found or may be new created frame.
    @threadsafe yes
    @modified   16.05.2002 11:08, as96863
*//*-*****************************************************************************************************/
css::uno::Reference< css::frame::XFrame > SAL_CALL Frame::findFrame( const ::rtl::OUString&  sTargetFrameName,
                                                                           sal_Int32         nSearchFlags    ) throw( css::uno::RuntimeException )
{
    css::uno::Reference< css::frame::XFrame > xTarget;

    //-----------------------------------------------------------------------------------------------------
    // 0) Ignore wrong parameter!
    //    We doesn't support search for following special targets.
    //    If we reject this requests - we mustnt check for such names
    //    in following code again and again. If we do not so -wrong
    //    search results can occure!
    //-----------------------------------------------------------------------------------------------------
    if (
        (sTargetFrameName==SPECIALTARGET_DEFAULT  )   ||    // valid for dispatches - not for findFrame()!
        (sTargetFrameName==SPECIALTARGET_MENUBAR  )   ||    // valid for dispatches - not for findFrame()!
        (sTargetFrameName==SPECIALTARGET_HELPAGENT)         // valid for dispatches - not for findFrame()!
       )
    {
        return NULL;
    }

    //-----------------------------------------------------------------------------------------------------
    // I) check for special defined targets first which must be handled exclusive.
    //    force using of "if() else if() ..."
    //-----------------------------------------------------------------------------------------------------

    // get threadsafe some neccessary member which are neccessary for following functionality
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::frame::XFrame >              xParent  ( m_xParent, css::uno::UNO_QUERY );
    css::uno::Reference< css::lang::XMultiServiceFactory > xFactory = m_xFactory;
    aReadLock.unlock();
    /* } SAFE */

    //-----------------------------------------------------------------------------------------------------
    // I.I) "_blank"
    //  Not allowed for a normal frame - but for the desktop.
    //  Use helper class to do so. It use the desktop automaticly.
    //-----------------------------------------------------------------------------------------------------
    if ( sTargetFrameName==SPECIALTARGET_BLANK )
    {
        TaskCreator aCreator(xFactory);
        xTarget = aCreator.createTask(::rtl::OUString(),sal_False);
    }

    //-----------------------------------------------------------------------------------------------------
    // I.II) "_parent"
    //  It doesn't matter if we have a valid parent or not. User ask for him and get it.
    //  An empty result is a valid result too.
    //-----------------------------------------------------------------------------------------------------
    else
    if ( sTargetFrameName==SPECIALTARGET_PARENT )
    {
        xTarget = xParent;
    }

    //-----------------------------------------------------------------------------------------------------
    // I.III) "_top"
    //  If we are not the top frame in this hierarchy, we must forward request to our parent.
    //  Otherwhise we must return ourself.
    //-----------------------------------------------------------------------------------------------------
    else
    if ( sTargetFrameName==SPECIALTARGET_TOP )
    {
        if (isTop())
            xTarget = this;
        else
        if (xParent.is()) // If we are not top - the parent MUST exist. But may it's better to check it again .-)
            xTarget = xParent->findFrame(SPECIALTARGET_TOP,0);
    }

    //-----------------------------------------------------------------------------------------------------
    // I.IV) "_self", ""
    //  This mean this frame in every case.
    //-----------------------------------------------------------------------------------------------------
    else
    if (
        ( sTargetFrameName==SPECIALTARGET_SELF ) ||
        ( sTargetFrameName.getLength()<1       )
       )
    {
        xTarget = this;
    }

    //-----------------------------------------------------------------------------------------------------
    // I.V) "_beamer"
    //  This is a special sub frame of any task. We must return it if we found it on our direct childrens
    //  or create it there if it not already exists.
    //  Note: Such beamer exists for task(top) frames only!
    //-----------------------------------------------------------------------------------------------------
    else
    if ( sTargetFrameName==SPECIALTARGET_BEAMER )
    {
        // We are a task => search or create the beamer
        if (isTop())
        {
            xTarget = m_aChildFrameContainer.searchOnDirectChildrens(SPECIALTARGET_BEAMER);
            if ( ! xTarget.is() )
            {
                /* TODO
                    Creation not supported yet!
                    Wait for new layout manager service because we can't plug it
                    inside already opened document of this frame ...
                */
            }
        }
        // We arent a task => forward request to our parent or ignore it.
        else
        if (xParent.is())
            xTarget = xParent->findFrame(SPECIALTARGET_BEAMER,0);
    }

    else
    {
        //-------------------------------------------------------------------------------------------------
        // II) otherwhise use optional given search flags
        //  force using of combinations of such flags. means no "else" part of use if() statements.
        //  But we ust break further searches if target was already found.
        //  Order of using flags is fix: SELF - CHILDREN - SIBLINGS - PARENT
        //  TASK and CREATE are handled special.
        //-------------------------------------------------------------------------------------------------

        // get threadsafe some neccessary member which are neccessary for following functionality
        /* SAFE { */
        aReadLock.lock();
        ::rtl::OUString sOwnName = m_sName;
        aReadLock.unlock();
        /* } SAFE */

        //-------------------------------------------------------------------------------------------------
        // II.I) SELF
        //  Check for right name. If it's the searched one return ourself - otherwhise
        //  ignore this flag.
        //-------------------------------------------------------------------------------------------------
        if (
            (nSearchFlags &  css::frame::FrameSearchFlag::SELF)  &&
            (sOwnName     == sTargetFrameName                 )
           )
        {
            xTarget = this;
        }

        //-------------------------------------------------------------------------------------------------
        // II.II) CHILDREN
        //  Search on all children for the given target name.
        //  An empty name value can't occure here - because it must be already handled as "_self"
        //  before. Used helper function of container doesn't create any frame.
        //  It makes a deep search only.
        //-------------------------------------------------------------------------------------------------
        if (
            ( ! xTarget.is()                                     ) &&
            (nSearchFlags & css::frame::FrameSearchFlag::CHILDREN)
           )
        {
            xTarget = m_aChildFrameContainer.searchOnAllChildrens(sTargetFrameName);
        }

        //-------------------------------------------------------------------------------------------------
        // II.III) TASKS
        //  This is a special flag. It regulate search on this task tree only or allow search on
        //  all other ones (which are sibling trees of us) too.
        //  Upper search must stop at this frame if we are the topest one and the TASK flag isn't set
        //  or we can ignore it if we have no valid parent.
        //-------------------------------------------------------------------------------------------------
        if (
            ( isTop() && (nSearchFlags & css::frame::FrameSearchFlag::TASKS) )   ||
            ( !isTop()                                                       )
           )
        {
            //-------------------------------------------------------------------------------------------------
            // II.III.I) SIBLINGS
            //  Search on all our direct siblings - means all childrens of our parent.
            //  Use this flag in combination with TASK. We must supress such upper search if
            //  user has not set it and if we are a top frame.
            //
            //  Attention: Don't forward this request to our parent as a findFrame() call.
            //  In such case we must protect us against recursive calls.
            //  Use snapshot of our parent. But don't use queryFrames() of XFrames interface.
            //  Because it's return all siblings and all her childrens including our children too
            //  if we call it with the CHILDREN flag. We doesn't need that - we need the direct container
            //  items of our parent only to start searches there. So we must use the container interface
            //  XIndexAccess instead of XFrames.
            //-------------------------------------------------------------------------------------------------
            if (
                ( ! xTarget.is()                                      ) &&
                (nSearchFlags &  css::frame::FrameSearchFlag::SIBLINGS) &&
                (   xParent.is()                                      ) // search on siblings is impossible without a parent
               )
            {
                css::uno::Reference< css::frame::XFramesSupplier > xSupplier( xParent, css::uno::UNO_QUERY );
                if (xSupplier.is())
                {
                    css::uno::Reference< css::container::XIndexAccess > xContainer( xSupplier->getFrames(), css::uno::UNO_QUERY );
                    if (xContainer.is())
                    {
                        sal_Int32 nCount = xContainer->getCount();
                        for( sal_Int32 i=0; i<nCount; ++i )
                        {
                            css::uno::Any aItem = xContainer->getByIndex(i);
                            css::uno::Reference< css::frame::XFrame > xSibling;
                            if (
                                ( !(aItem>>=xSibling)                                 ) ||  // control unpacking
                                ( ! xSibling.is()                                     ) ||  // check for valid items
                                ( xSibling==static_cast< ::cppu::OWeakObject* >(this) )     // ignore ourself! (We are a part of this container too - but search on our children was already done.)
                            )
                            {
                                continue;
                            }

                            // Don't allow upper search here! Use rigth flags to regulate it.
                            // And allow deep search on children only - if it was allowed for us too.
                            sal_Int32 nRightFlags = css::frame::FrameSearchFlag::SELF;
                            if (nSearchFlags & css::frame::FrameSearchFlag::CHILDREN)
                                nRightFlags |= css::frame::FrameSearchFlag::CHILDREN;
                            xTarget = xSibling->findFrame(sTargetFrameName, nRightFlags );
                            // perform search be breaking further search if a result exist.
                            if (xTarget.is())
                                break;
                        }
                    }
                }
            }

            //-------------------------------------------------------------------------------------------------
            // II.III.II) PARENT
            //  Forward search to our parent (if he exists.)
            //  To prevent us against recursive and superflous calls (which can occure if we allow him
            //  to search on his childrens too) we must change used search flags.
            //-------------------------------------------------------------------------------------------------
            if (
                ( ! xTarget.is()                                    ) &&
                (nSearchFlags &  css::frame::FrameSearchFlag::PARENT) &&
                (   xParent.is()                                    )
               )
            {
                if (xParent->getName() == sTargetFrameName)
                    xTarget = xParent;
                else
                {
                    sal_Int32 nRightFlags  = nSearchFlags;
                              nRightFlags &= ~css::frame::FrameSearchFlag::CHILDREN;
                    xTarget = xParent->findFrame(sTargetFrameName, nRightFlags);
                }
            }
        }

        //-------------------------------------------------------------------------------------------------
        // II.IV) CREATE
        //  If we haven't found any valid target frame by using normal flags - but user allowed us to create
        //  a new one ... we should do that. Used TaskCreator use Desktop instance automaticly as parent!
        //-------------------------------------------------------------------------------------------------
        if (
            ( ! xTarget.is()                                   )    &&
            (nSearchFlags & css::frame::FrameSearchFlag::CREATE)
           )
        {
            TaskCreator aCreator(xFactory);
            xTarget = aCreator.createTask(sTargetFrameName,sal_False);
        }
    }

    return xTarget;
}

/*-****************************************************************************************************//**
    @short      -
    @descr      Returns sal_True, if this frame is a "top frame", otherwise sal_False.
                The "m_bIsFrameTop" member must be set in the ctor or setCreator() method.
                A top frame is a member of the top frame container or a member of the
                task frame container. Both containers can create new frames if the findFrame()
                method of their css::frame::XFrame interface is called with a frame name not yet known.

    @seealso    ctor
    @seealso    method setCreator()
    @seealso    method findFrame()

    @param      -
    @return     true, if is it a top frame ... false otherwise.

    @onerror    No error should occure!
*//*-*****************************************************************************************************/
sal_Bool SAL_CALL Frame::isTop() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    // This information is set in setCreator().
    // We are top, if ouer parent is a task or the desktop or if no parent exist!
    return m_bIsFrameTop;
}

/*-****************************************************************************************************//**
    @short      activate frame in hierarchy
    @descr      This feature is used to mark active pathes in our frame hierarchy.
                You can be a listener for this event to react for it ... change some internal states or something else.

    @seealso    method deactivate()
    @seealso    method isActivate()
    @seealso    enum EActiveState
    @seealso    listener mechanism

    @param      -
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::activate() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );

    // Copy neccessary member and free the lock.
    // It's not neccessary for m_aChildFrameContainer ... because
    // he is threadsafe himself and live if we live.
    // We use a registered transaction to prevent us against
    // breaks during this operation!
    css::uno::Reference< css::frame::XFrame >           xActiveChild    = m_aChildFrameContainer.getActive()                              ;
    css::uno::Reference< css::frame::XFramesSupplier >  xParent         ( m_xParent, css::uno::UNO_QUERY )                                ;
    css::uno::Reference< css::frame::XFrame >           xThis           ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    css::uno::Reference< css::awt::XWindow >            xComponentWindow( m_xComponentWindow, css::uno::UNO_QUERY )                       ;
    EActiveState                                        eState          = m_eActiveState                                                  ;

    aWriteLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    //_________________________________________________________________________________________________________
    //  1)  If I'am not active before ...
    if( eState == E_INACTIVE )
    {
        // ... do it then.
        aWriteLock.lock();
        eState         = E_ACTIVE;
        m_eActiveState = eState;
        aWriteLock.unlock();
        // Deactivate sibling path and forward activation to parent ... if any parent exist!
        if( xParent.is() == sal_True )
        {
            // Everytime set THIS frame as active child of parent and activate it.
            // We MUST have a valid path from bottom to top as active path!
            // But we must deactivate the old active sibling path first.

            // Attention: Deactivation of an active path, deactivate the whole path ... from bottom to top!
            // But we wish to deactivate founded sibling-tree only.
            // [ see deactivate() / step 4) for further informations! ]

            xParent->setActiveFrame( xThis );

            // Then we can activate from here to top.
            // Attention: We are ACTIVE now. And the parent will call activate() at us!
            // But we do nothing then! We are already activated.
            xParent->activate();
        }
        // Its neccessary to send event NOW - not before.
        // Activation goes from bottom to top!
        // Thats the reason to activate parent first and send event now.
        implts_sendFrameActionEvent( css::frame::FrameAction_FRAME_ACTIVATED );
    }

    //_________________________________________________________________________________________________________
    //  2)  I was active before or current activated and there is a path from here to bottom, who CAN be active.
    //      But ouer direct child of path is not active yet.
    //      (It can be, if activation occur in the middle of a current path!)
    //      In these case we activate path to bottom to set focus on right frame!
    if  (
            ( eState                    ==  E_ACTIVE    )   &&
            ( xActiveChild.is()         ==  sal_True    )   &&
            ( xActiveChild->isActive()  ==  sal_False   )
        )
    {
        xActiveChild->activate();
    }

    //_________________________________________________________________________________________________________
    //  3)  I was active before or current activated. But if I have no active child => I will get the focus!
    if  (
            ( eState            ==  E_ACTIVE    )   &&
            ( xActiveChild.is() ==  sal_False   )
        )
    {
        aWriteLock.lock();
        eState         = E_FOCUS;
        m_eActiveState = eState;
        aWriteLock.unlock();
        implts_sendFrameActionEvent( css::frame::FrameAction_FRAME_UI_ACTIVATED );
        ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
        Window* pWindow = VCLUnoHelper::GetWindow( xComponentWindow );
        if ( !pWindow )
            pWindow = VCLUnoHelper::GetWindow( m_xContainerWindow );

        if( pWindow != NULL && isTop() && m_xController.is() )
        {
            Application::SetDefModalDialogParent( pWindow );
        }
        aSolarGuard.clear();
    }
}

/*-****************************************************************************************************//**
    @short      deactivate frame in hierarchy
    @descr      This feature is used to deactive pathes in our frame hierarchy.
                You can be a listener for this event to react for it ... change some internal states or something else.

    @seealso    method activate()
    @seealso    method isActivate()
    @seealso    enum EActiveState
    @seealso    listener mechanism

    @param      -
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::deactivate() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );

    // Copy neccessary member and free the lock.
    css::uno::Reference< css::frame::XFrame >           xActiveChild    = m_aChildFrameContainer.getActive()                              ;
    css::uno::Reference< css::frame::XFramesSupplier >  xParent         ( m_xParent, css::uno::UNO_QUERY )                                ;
    css::uno::Reference< css::frame::XFrame >           xThis           ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    EActiveState                                        eState          = m_eActiveState                                                  ;

    aWriteLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    // Work only, if there something to do!
    if( eState != E_INACTIVE )
    {
        //_____________________________________________________________________________________________________
        //  1)  Deactivate all active childs.
        if  (
                ( xActiveChild.is()         ==  sal_True    )   &&
                ( xActiveChild->isActive()  ==  sal_True    )
            )
        {
            xActiveChild->deactivate();
        }

        //_____________________________________________________________________________________________________
        //  2)  If I have the focus - I will lost it now.
        if( eState == E_FOCUS )
        {
            // Set new state INACTIVE(!) and send message to all listener.
            // Don't set ACTIVE as new state. This frame is deactivated for next time - due to activate().
            aWriteLock.lock();
            eState          = E_ACTIVE;
            m_eActiveState  = eState  ;
            aWriteLock.unlock();
            implts_sendFrameActionEvent( css::frame::FrameAction_FRAME_UI_DEACTIVATING );
        }

        //_____________________________________________________________________________________________________
        //  3)  If I'am active - I will be deactivated now.
        if( eState == E_ACTIVE )
        {
            // Set new state and send message to all listener.
            aWriteLock.lock();
            eState          = E_INACTIVE;
            m_eActiveState  = eState    ;
            aWriteLock.unlock();
            implts_sendFrameActionEvent( css::frame::FrameAction_FRAME_DEACTIVATING );
        }

        //_____________________________________________________________________________________________________
        //  4)  If there is a path from here to my parent ...
        //      ... I'am on the top or in the middle of deactivated subtree and action was started here.
        //      I must deactivate all frames from here to top, which are members of current path.
        //      Stop, if THESE frame not the active frame of ouer parent!
        if  (
                ( xParent.is()              ==  sal_True    )   &&
                ( xParent->getActiveFrame() ==  xThis       )
            )
        {
            // We MUST break the path - otherwise we will get the focus - not ouer parent! ...
            // Attention: Ouer parent don't call us again - WE ARE NOT ACTIVE YET!
            // [ see step 3 and condition "if ( m_eActiveState!=INACTIVE ) ..." in this method! ]
            xParent->deactivate();
        }
    }
}

/*-****************************************************************************************************//**
    @short      returns active state
    @descr      Call it to get informations about current active state of this frame.

    @seealso    method activate()
    @seealso    method deactivate()
    @seealso    enum EActiveState

    @param      -
    @return     true if active, false otherwise.

    @onerror    No error should occure.
*//*-*****************************************************************************************************/
sal_Bool SAL_CALL Frame::isActive() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    return  (
                ( m_eActiveState    ==  E_ACTIVE    )   ||
                ( m_eActiveState    ==  E_FOCUS     )
            );
}

/*-****************************************************************************************************//**
    @short      ???
    @descr      -

    @seealso    -

    @param      -
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::contextChanged() throw( css::uno::RuntimeException )
{
    // Look for rejected calls!
    // Sometimes called during closing object... => soft exceptions
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );
    // Impl-method is threadsafe himself!
    // Send event to all listener for frame actions.
    implts_sendFrameActionEvent( css::frame::FrameAction_CONTEXT_CHANGED );
}

/*-****************************************************************************************************//**
    @short      set new component inside the frame
    @descr      A frame is a container for a component. Use this method to set, change or realease it!
                We accept null references! The xComponentWindow will be a child of our container window
                and get all window events from us.

    @attention  (a) A current set component can disagree with the suspend() request!
                    We don't set the new one and return with false then.
                (b) It's possible to set:
                        (b1) a simple component here which supports the window only - no controller;
                        (b2) a full featured component which supports window and controller;
                        (b3) or both to NULL if outside code which to forget this component.

    @seealso    method getComponentWindow()
    @seealso    method getController()

    @param      xComponentWindow
                    valid reference to new component window which will be a child of internal container window
                    May <NULL/> for releasing.
    @param      xController
                    reference to new component controller
                    (may <NULL/> for relasing or setting of a simple component)

    @return     <TRUE/> if operation was successful, <FALSE/> otherwise.

    @onerror    We return <FALSE/>.
    @threadsafe yes
    @modified   06.05.2002 11:39, as96863
*//*-*****************************************************************************************************/
sal_Bool SAL_CALL Frame::setComponent(  const   css::uno::Reference< css::awt::XWindow >&       xComponentWindow ,
                                        const   css::uno::Reference< css::frame::XController >& xController      ) throw( css::uno::RuntimeException )
{
    //_____________________________________________________________________________________________________
    // Ignore this HACK of sfx2!
    // He call us with an valid controller without a valid window ... Thats not allowed!
    if  ( xController.is() && ! xComponentWindow.is() )
        return sal_True;

    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    //_____________________________________________________________________________________________________
    // Get threadsafe some copies of used members.
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::awt::XWindow >       xContainerWindow    = m_xContainerWindow;
    css::uno::Reference< css::awt::XWindow >       xOldComponentWindow = m_xComponentWindow;
    css::uno::Reference< css::frame::XController > xOldController      = m_xController;
    sal_Bool                                       bHadFocus           = ( m_eActiveState==E_FOCUS && m_xComponentWindow.is() );
    sal_Bool                                       bWasConnected       = m_bConnected;
    aReadLock.unlock();
    /* } SAFE */

    // Before dispose() of this window we must search another def modal dialog parent.
    // Set the container window of this frame as the new one - but do it only if our old
    // component window was this special dialog parent realy.
    /* SOLAR SAFE { */
    ::vos::OClearableGuard aGlobalSolarLock( Application::GetSolarMutex() );
    Window* pContainerWindow    = VCLUnoHelper::GetWindow( xContainerWindow    );
    Window* pOldComponentWindow = VCLUnoHelper::GetWindow( xOldComponentWindow );
    if (
        ( pOldComponentWindow                    != NULL                )   &&
        ( Application::GetDefModalDialogParent() == pOldComponentWindow ) &&
        isTop()
        )
    {
        Application::SetDefModalDialogParent( pContainerWindow );
    }

    aGlobalSolarLock.clear();
    /* } SAFE */

    //_____________________________________________________________________________________________________
    // stop listening on old window
    // May it produce some trouble.
    // But don't forget to listen on new window again ... or reactivate listening
    // if we reject this setComponent() request and leave this method without changing the old window.
    implts_stopWindowListening();

    // Notify all listener, that this component (if current one exist) will be unloaded.
    if (bWasConnected)
        implts_sendFrameActionEvent( css::frame::FrameAction_COMPONENT_DETACHING );

    //_____________________________________________________________________________________________________
    // otherwhise release old component first
    // Always release controller before releasing window,
    // because controller may want to access its window!
    // But check for real changes - may the new controller is the old one.
    if (
        (xOldController.is()          )   &&
        (xOldController != xController)
       )
    {
        /* ATTENTION
            Don't suspend the old controller here. Because the outside caller must do that
            by definition. We have to dispose it here only.
         */

        // Before we dispose this controller we should hide it inside this frame instance.
        // We hold it alive for next calls by using xOldController!
        /* SAFE {*/
        WriteGuard aWriteLock( m_aLock );
        m_xController = NULL;
        aWriteLock.unlock();
        /* } SAFE */

        css::uno::Reference< css::lang::XComponent > xDisposable( xOldController, css::uno::UNO_QUERY );
        if (xDisposable.is())
            xDisposable->dispose();
        xOldController = NULL;
    }

    //_____________________________________________________________________________________________________
    // Now it's time to release the component window.
    // If controller wasn't released successfully - this code line shouldn't be reached.
    // Because in case of "suspend()==false" we return immediately with false ...
    // see before
    // Check for real changes too.
    if (
        (xOldComponentWindow.is()               )   &&
        (xOldComponentWindow != xComponentWindow)
       )
    {
        /* SAFE { */
        WriteGuard aWriteLock( m_aLock );
        m_xComponentWindow = NULL;
        aWriteLock.unlock();
        /* } SAFE */

        css::uno::Reference< css::lang::XComponent > xDisposable( xOldComponentWindow, css::uno::UNO_QUERY );
        if (xDisposable.is())
            xDisposable->dispose();
        xOldComponentWindow = NULL;
    }

    //_____________________________________________________________________________________________________
    // Now it's time to set the new component ...
    // By the way - find out our new "load state" - means if we have a valid component inside.
    /* SAFE { */
    WriteGuard aWriteLock( m_aLock );
    m_xComponentWindow = xComponentWindow;
    m_xController      = xController     ;
    m_bConnected       = (m_xComponentWindow.is() || m_xController.is());
    sal_Bool bIsConnected       = m_bConnected;
    aWriteLock.unlock();
    /* } SAFE */

    //_____________________________________________________________________________________________________
    // notifies all interest listener, that current component was changed or a new one was loaded
    if (bIsConnected && bWasConnected)
        implts_sendFrameActionEvent( css::frame::FrameAction_COMPONENT_REATTACHED );
    else
    if (bIsConnected && !bWasConnected)
        implts_sendFrameActionEvent( css::frame::FrameAction_COMPONENT_ATTACHED   );

    //_____________________________________________________________________________________________________
    // A new component window doesn't know anything about current active/focus states.
    // Set this information on it!
    if ( xComponentWindow.is() && m_eActiveState == E_FOCUS )
    {
        if ( bHadFocus )
            xComponentWindow->setFocus();

        /* SOLAR SAFE { */
        ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
        Window* pWindow = VCLUnoHelper::GetWindow( xComponentWindow );
        if ( pWindow!=NULL && isTop() && xController.is() )
            Application::SetDefModalDialogParent( pWindow );
        aSolarGuard.clear();
        /* } SOLAR SAFE */
    }

    // If it was a new component window - we must resize it to fill out
    // our container window.
    implts_resizeComponentWindow();
    // New component should change our current icon ...
    implts_setIconOnWindow();
    // OK - start listening on new window again - or do nothing if it is an empty one.
    implts_startWindowListening();

    return sal_True;
}

/*-****************************************************************************************************//**
    @short      returns current set component window
    @descr      Frames are used to display components. The actual displayed component is
                held by the m_xComponentWindow property. If the component implements only a
                XComponent interface, the communication between the frame and the
                component is very restricted. Better integration is achievable through a
                XController interface.
                If the component wants other objects to be able to get information about its
                ResourceDescriptor it has to implement a XModel interface.
                This frame is the owner of the component window.

    @seealso    method setComponent()

    @param      -
    @return     css::uno::Reference to current set component window.

    @onerror    A null reference is returned.
*//*-*****************************************************************************************************/
css::uno::Reference< css::awt::XWindow > SAL_CALL Frame::getComponentWindow() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    return m_xComponentWindow;
}

/*-****************************************************************************************************//**
    @short      returns current set controller
    @descr      Frames are used to display components. The actual displayed component is
                held by the m_xComponentWindow property. If the component implements only a
                XComponent interface, the communication between the frame and the
                component is very restricted. Better integration is achievable through a
                XController interface.
                If the component wants other objects to be able to get information about its
                ResourceDescriptor it has to implement a XModel interface.
                This frame is the owner of the component window.

    @seealso    method setComponent()

    @param      -
    @return     css::uno::Reference to current set controller.

    @onerror    A null reference is returned.
*//*-*****************************************************************************************************/
css::uno::Reference< css::frame::XController > SAL_CALL Frame::getController() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Register transaction and reject wrong calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    return m_xController;
}

/*-****************************************************************************************************//**
    @short      add/remove listener for activate/deactivate/contextChanged events
    @descr      -

    @seealso    method activate()
    @seealso    method deactivate()
    @seealso    method contextChanged()

    @param      "xListener" reference to your listener object
    @return     -

    @onerror    Listener is ignored.
*//*-*****************************************************************************************************/
void SAL_CALL Frame::addFrameActionListener( const css::uno::Reference< css::frame::XFrameActionListener >& xListener ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_addFrameActionListener( xListener ), "Frame::addFrameActionListener()", "Invalid parameter detected." )
    // Listener container is threadsafe by himself ... but we must look for rejected calls!
    // Our OMenuDispatch-helper (is a member of ODispatchProvider!) is create at startup of this frame BEFORE initialize!
    // => soft exceptions!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    m_aListenerContainer.addInterface( ::getCppuType( (const css::uno::Reference< css::frame::XFrameActionListener >*)NULL ), xListener );
}

//*****************************************************************************************************************
void SAL_CALL Frame::removeFrameActionListener( const css::uno::Reference< css::frame::XFrameActionListener >& xListener ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_removeFrameActionListener( xListener ), "Frame::removeFrameActionListener()", "Invalid parameter detected." )
    // Listener container is threadsafe by himself ... but we must look for rejected calls after disposing!
    // But we must work with E_SOFTEXCEPTIONS ... because sometimes we are called from our listeners
    // during dispose! Our work mode is E_BEFORECLOSE then ... and E_HARDEXCEPTIONS whould throw a DisposedException.
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    m_aListenerContainer.removeInterface( ::getCppuType( (const css::uno::Reference< css::frame::XFrameActionListener >*)NULL ), xListener );
}

/*-****************************************************************************************************//**
    @short      support two way mechanism to release a frame
    @descr      This method ask internal component (controller) if he accept this close request.
                In case of <TRUE/> nothing will be happen (from point of caller of this close method).
                In case of <FALSE/> a CloseVetoException is thrown. After such exception given parameter
                <var>bDeliverOwnerShip</var> regulate which will be the new owner of this instance.

    @attention  It's the replacement for XTask::close() which is marked as obsolete method.

    @param      bDeliverOwnerShip
                    If parameter is set to <FALSE/> the original caller will be the owner after thrown
                    veto exception and must try to close this frame at later time again. Otherwhise the
                    source of throwed exception is the right one. May it will be the frame himself.

    @thrown     CloseVetoException
                    if any internal things willn't be closed

    @threadsafe yes
    @modified   06.05.2002 08:33, as96863
*//*-*****************************************************************************************************/
void SAL_CALL Frame::close( sal_Bool bDeliverOwnerShip ) throw( css::util::CloseVetoException,
                                                                css::uno::RuntimeException   )
{
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    // At the end of this method may we must dispose ourself ...
    // and may nobody from outside hold a reference to us ...
    // then it's a good idea to do that by ourself.
    css::uno::Reference< css::uno::XInterface > xSelfHold( static_cast< ::cppu::OWeakObject* >(this) );

    // Check any close listener before we look for currently running internal processes.
    // Because if a listener disagree with this close() request - we hace time to finish this
    // internal operations too ...
    // Note: container is threadsafe himself.
    css::lang::EventObject             aSource    (static_cast< ::cppu::OWeakObject*>(this));
    ::cppu::OInterfaceContainerHelper* pContainer = m_aListenerContainer.getContainer( ::getCppuType( ( const css::uno::Reference< css::util::XCloseListener >*) NULL ) );
    if (pContainer!=NULL)
    {
        ::cppu::OInterfaceIteratorHelper pIterator(*pContainer);
        while (pIterator.hasMoreElements())
        {
            try
            {
                ((css::util::XCloseListener*)pIterator.next())->queryClosing( aSource, bDeliverOwnerShip );
            }
            catch( css::uno::RuntimeException& )
            {
                pIterator.remove();
            }
        }
    }

    // Ok - no listener disagreed with this close() request
    // check if this frame is used for any load process currently
    if (isActionLocked())
    {
        if (bDeliverOwnerShip)
        {
            /* SAFE */
            WriteGuard aWriteLock( m_aLock );
            m_bSelfClose = sal_True;
            aWriteLock.unlock();
            /* SAFE */
        }

        throw css::util::CloseVetoException(DECLARE_ASCII("Frame in use for loading document ..."),static_cast< ::cppu::OWeakObject*>(this));
    }

    if ( ! setComponent(NULL,NULL) )
        throw css::util::CloseVetoException(DECLARE_ASCII("Component couldn't be deattached ..."),static_cast< ::cppu::OWeakObject*>(this));

    // If closing is allowed ... inform all istener and dispose this frame!
    pContainer = m_aListenerContainer.getContainer( ::getCppuType( ( const css::uno::Reference< css::util::XCloseListener >*) NULL ) );
    if (pContainer!=NULL)
    {
        ::cppu::OInterfaceIteratorHelper pIterator(*pContainer);
        while (pIterator.hasMoreElements())
        {
            try
            {
                ((css::util::XCloseListener*)pIterator.next())->notifyClosing( aSource );
            }
            catch( css::uno::RuntimeException& )
            {
                pIterator.remove();
            }
        }
    }

    // Attention: We must release our own registered transaction here. Otherwhise following dispose() call
    // wait for us too ....
    aTransaction.stop();
    dispose();
}

/*-****************************************************************************************************//**
    @short      be a listener for close events!
    @descr      Adds/remove a CloseListener at this frame instance. If the close() method is called on
                this object, the such listener are informed and can disagree with that by throwing
                a CloseVetoException.

    @seealso    Frame::close()

    @param      xListener
                    reference to your listener object

    @onerror    Listener is ignored.

    @threadsafe yes
    @modified   06.05.2002 10:03, as96863
*//*-*****************************************************************************************************/
void SAL_CALL Frame::addCloseListener( const css::uno::Reference< css::util::XCloseListener >& xListener ) throw (css::uno::RuntimeException)
{
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );
    // We doesn't need any lock here ...
    // Container lives if we live and is threadsafe by himself.
    m_aListenerContainer.addInterface( ::getCppuType( ( const css::uno::Reference< css::util::XCloseListener >* ) NULL ), xListener );
}

//*****************************************************************************************************************
void SAL_CALL Frame::removeCloseListener( const css::uno::Reference< css::util::XCloseListener >& xListener ) throw (css::uno::RuntimeException)
{
    // Use soft exception mode - moslty this method is called during disposing of this frame ...
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );
    // We doesn't need any lock here ...
    // Container lives if we live and is threadsafe by himself.
    m_aListenerContainer.removeInterface( ::getCppuType( ( const css::uno::Reference< css::util::XCloseListener >* ) NULL ), xListener );
}

/*-****************************************************************************************************//**
    @short      destroy instance
    @descr      The owner of this object calles the dispose method if the object
                should be destroyed. All other objects and components, that are registered
                as an EventListener are forced to release their references to this object.
                Furthermore this frame is removed from its parent frame container to release
                this reference. The reference attributes are disposed and released also.

    @attention  Look for globale description at beginning of file too!
                (DisposedException, FairRWLock ..., initialize, dispose)

    @seealso    method initialize()
    @seealso    baseclass FairRWLockBase!

    @param      -
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::dispose() throw( css::uno::RuntimeException )
{
    // We should hold a reference to ourself ...
    // because our owner dispose us and release our reference ...
    // May be we will die before we could finish this method ...
    css::uno::Reference< css::frame::XFrame > xThis( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );

    // First operation should be ... "stopp all listening for window events on our container window".
    // These events are superflous but can make trouble!
    // We will die, die and die ...
    implts_stopWindowListening();

    // Send message to all listener and forget her references.
    LOG_DISPOSEEVENT( "Frame", sName )
    css::lang::EventObject aEvent( xThis );
    m_aListenerContainer.disposeAndClear( aEvent );

    // Disable this instance for further work.
    // This will wait for all current running ones ...
    // and reject all further requests!
    m_aTransactionManager.setWorkingMode( E_BEFORECLOSE );

    // Don't show any dialogs, errors or something else any more!
    // If somewhere called dispose() whitout close() before - normaly no dialogs
    // should exist. Otherwhise it's the problem of the outside caller.
    // Note:
    //      (a) Do it after stopWindowListening(). May that force some active/deactive
    //          notifications which we doesn't need here realy.
    //      (b) Don't forget to enable this dialog mode at the end of this dipose()
    //          again. Otherwhise no dialogs will be shown in any frame anymore.
    Application::EnableDialogCancel( sal_True );

    // We should be alone for ever and further dispose calls are rejected by lines before ...
    // I hope it :-)

    // Forget our internal component and her window first.
    // So we can release our container window later without problems.
    // Because this container window is the parent of the component window ...
    // Note: Dispose it hard - because suspending must be done inside close() call!
    // But try to dispose the controller first befor you destroy the window.
    // Because the window is used by the controller too ...
    if (m_xController.is())
    {
        LOG_WARNING("Frame::dispose()", "Sorry - but controller shouldn't exist at this point!")
        css::uno::Reference< css::lang::XComponent > xDisposable( m_xController, css::uno::UNO_QUERY );
        if (xDisposable.is())
            xDisposable->dispose();
    }

    if (m_xComponentWindow.is())
    {
        LOG_WARNING("Frame::dispose()", "Sorry - but component window shouldn't exist at this point!")
        css::uno::Reference< css::lang::XComponent > xDisposable( m_xComponentWindow, css::uno::UNO_QUERY );
        if (xDisposable.is())
            xDisposable->dispose();
    }

    // Free references of our frame tree.
    // Force parent container to forget this frame too ...
    // ( It's contained in m_xParent and so no css::lang::XEventListener for m_xParent! )
    // It's important to do that before we free some other internal structures.
    // Because if our parent gets an activate and found us as last possible active frame
    // he try to deactivate us ... and we run into some trouble.
    if( m_xParent.is() == sal_True )
    {
        m_xParent->getFrames()->remove( xThis );
        m_xParent = css::uno::Reference< css::frame::XFramesSupplier >();
    }

    /*ATTENTION
        It's neccessary to release our StatusIndicatorFactory-helper at first!
        He share our container window as parent for any created status indicator objects ...
        and if we dispose this container window before we release this helper ...
        we will run into some trouble!
     */
    m_xIndicatorFactoryHelper = NULL;
    impl_disposeContainerWindow( m_xContainerWindow );

    /*ATTENTION
        Clear container after successful removing from parent container ...
        because our parent could be the desktop which stand in dispose too!
        If we have already cleared our own container we lost our child before this could be
        remove himself at this instance ...
        Release m_xFramesHelper after that ... it's the same problem between parent and child!
        "m_xParent->getFrames()->remove( xThis );" needs this helper ...
        Otherwise we get a null reference and could finish removing successfuly.
        => You see: Order of calling operations is important!!!
     */
    m_aChildFrameContainer.clear();
    m_xFramesHelper = NULL;

    // Release some other references.
    // This calls should be easy ... I hope it :-)
    m_xDispatchHelper           = NULL;
    m_xFactory                  = NULL;
    m_xDropTargetListener       = NULL;
    m_xDispatchRecorderSupplier = NULL;

    // Disable this instance for further working realy!
    m_aTransactionManager.setWorkingMode( E_CLOSE );

    // Don't forget it - otherwhise no dialogs can be shown anymore in other frames.
    Application::EnableDialogCancel( sal_False );
}

/*-****************************************************************************************************//**
    @short      Be a listener for dispose events!
    @descr      Adds/remove an EventListener to this object. If the dispose method is called on
                this object, the disposing method of the listener is called.

    @seealso    -

    @param      "xListener" reference to your listener object.
    @return     -

    @onerror    Listener is ignored.
*//*-*****************************************************************************************************/
void SAL_CALL Frame::addEventListener( const css::uno::Reference< css::lang::XEventListener >& xListener ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_addEventListener( xListener ), "Frame::addEventListener()", "Invalid parameter detected." )
    // Look for rejected calls only!
    // Container is threadsafe.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    m_aListenerContainer.addInterface( ::getCppuType( ( const css::uno::Reference< css::lang::XEventListener >* ) NULL ), xListener );
}

//*****************************************************************************************************************
void SAL_CALL Frame::removeEventListener( const css::uno::Reference< css::lang::XEventListener >& xListener ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_removeEventListener( xListener ), "Frame::removeEventListener()", "Invalid parameter detected." )
    // Look for rejected calls only!
    // Container is threadsafe.
    // Use E_SOFTEXCEPTIONS to allow removing listeners during dispose call!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    m_aListenerContainer.removeInterface( ::getCppuType( ( const css::uno::Reference< css::lang::XEventListener >* ) NULL ), xListener );
}

/*-****************************************************************************************************//**
    @short      create new status indicator
    @descr      Use returned status indicator to show progresses and some text informations.
                All created objects share the same dialog! Only the last one can show his information.

    @seealso    class StatusIndicatorFactory
    @seealso    class StatusIndicator

    @param      -
    @return     A reference to created object.

    @onerror    We return a null reference.
*//*-*****************************************************************************************************/
css::uno::Reference< css::task::XStatusIndicator > SAL_CALL Frame::createStatusIndicator() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */
    // Look for rejected calls!
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    // Make snapshot of neccessary member and define default return value.
    css::uno::Reference< css::task::XStatusIndicator >           xIndicator                                        ;
    css::uno::Reference< css::task::XStatusIndicatorSupplier >   xSupplier   ( m_xController, css::uno::UNO_QUERY );
    css::uno::Reference< css::task::XStatusIndicatorFactory >    xFactory    = m_xIndicatorFactoryHelper           ;

    aReadLock.unlock();
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */
    // If controller can create this status indicator ... use it for return.
    // Otherwise use our own indicator factory helper!
    if( xSupplier.is() == sal_True )
    {
        xIndicator = xSupplier->getStatusIndicator();
    }

    if(
        ( xIndicator.is() == sal_False ) &&
        ( xFactory.is()   == sal_True  )
      )
    {
        xIndicator = xFactory->createStatusIndicator();
    }

    return xIndicator;
}

/*-****************************************************************************************************//**
    @interface  XDispatchInformationProvider
    @short      get informations about supported dispatch commands
    @descr      Use this interface to get informations about possibility to dispatch special commands.
                ( e.g. "print", "close" ... )
                In current implmentation we forward requests to our internal controller ...
                but in following implementaions we could add own infos to returned values.

    @seealso    interface XDispatchInformationProvider

    @param      "sURL"          , queried command
    @param      "lURLs"         , list of commands for faster access
    @param      "lDescriptions" , result set if more then one URLs was queried
    @return     Configuration of dispatch informations at method getConfigurableDispatchInformation().

    @onerror    -
    @threadsafe yes
*//*-*****************************************************************************************************/
::rtl::OUString SAL_CALL Frame::queryDescription( const ::rtl::OUString& sURL ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */
    // Look for rejected calls!
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    // Declare default return value if method failed.
    ::rtl::OUString sInfo;

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< dcss::frame::XDispatchInformationProvider > xProvider( m_xController, css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */

    if( xProvider.is() == sal_True )
    {
        sInfo = xProvider->queryDescription( sURL );
    }
    return sInfo;
}

//*****************************************************************************************************************
void SAL_CALL Frame::queryDescriptions( const css::uno::Sequence< ::rtl::OUString >& lURLs         ,
                                              css::uno::Sequence< ::rtl::OUString >& lDescriptions ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */
    // Look for rejected calls!
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< dcss::frame::XDispatchInformationProvider > xProvider( m_xController, css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */

    if( xProvider.is() == sal_True )
    {
        xProvider->queryDescriptions( lURLs, lDescriptions );
    }
}

//*****************************************************************************************************************
css::uno::Sequence< dcss::frame::DispatchInformation > SAL_CALL Frame::getConfigurableDispatchInformation() throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */
    // Look for rejected calls!
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< dcss::frame::XDispatchInformationProvider > xProvider( m_xController, css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */

    css::uno::Sequence< dcss::frame::DispatchInformation > lInfos;
    if( xProvider.is() == sal_True )
    {
        lInfos = xProvider->getConfigurableDispatchInformation();
    }
    return lInfos;
}

/*-****************************************************************************************************//**
    @short      search for target to load URL
    @descr      This method searches for a dispatch for the specified DispatchDescriptor.
                The FrameSearchFlags and the FrameName of the DispatchDescriptor are
                treated as described for findFrame.

    @seealso    method findFrame()
    @seealso    method queryDispatches()
    @seealso    method set/getName()
    @seealso    class TargetFinder

    @param      "aURL"              , URL for loading
    @param      "sTargetFrameName"  , name of target frame
    @param      "nSearchFlags"      , additional flags to regulate search if sTargetFrameName isn't clear
    @return     css::uno::Reference to dispatch handler.

    @onerror    A null reference is returned.
*//*-*****************************************************************************************************/
css::uno::Reference< css::frame::XDispatch > SAL_CALL Frame::queryDispatch( const css::util::URL&   aURL            ,
                                                                            const ::rtl::OUString&  sTargetFrameName,
                                                                                  sal_Int32         nSearchFlags    ) throw( css::uno::RuntimeException )
{
    const char UNO_PROTOCOL[] = ".uno:";

    // Don't check incoming parameter here! Our helper do it for us and it isn't a good idea to do it more then ones!
    // But look for rejected calls!
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    // Remove uno and cmd protocol part as we want to support both of them. We store only the command part
    // in our hash map. All other protocols are stored with the protocol part.
    String aCommand( aURL.Main );
    if ( aURL.Protocol.equalsIgnoreAsciiCaseAsciiL( UNO_PROTOCOL, sizeof( UNO_PROTOCOL )-1 ))
        aCommand = aURL.Path;

    // Make hash_map lookup if the current URL is in the disabled list
    if ( m_aCommandOptions.Lookup( SvtCommandOptions::CMDOPTION_DISABLED, aCommand ) )
        return css::uno::Reference< css::frame::XDispatch >();
    else
    {
        // We use a helper to support these interface and an interceptor mechanism.
        // Our helper is threadsafe by himself!
        return m_xDispatchHelper->queryDispatch( aURL, sTargetFrameName, nSearchFlags );
    }
}

/*-****************************************************************************************************//**
    @short      handle more then ones dispatches at same call
    @descr      Returns a sequence of dispatches. For details see the queryDispatch method.
                For failed dispatches we return empty items in list!

    @seealso    method queryDispatch()

    @param      "lDescriptor" list of dispatch arguments for queryDispatch()!
    @return     List of dispatch references. Some elements can be NULL!

    @onerror    An empty list is returned.
*//*-*****************************************************************************************************/
css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL Frame::queryDispatches( const css::uno::Sequence< css::frame::DispatchDescriptor >& lDescriptor ) throw( css::uno::RuntimeException )
{
    // Don't check incoming parameter here! Our helper do it for us and it isn't a good idea to do it more then ones!
    // But look for rejected calls!
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    // We use a helper to support these interface and an interceptor mechanism.
    // Our helper is threadsafe by himself!
    return m_xDispatchHelper->queryDispatches( lDescriptor );
}

/*-****************************************************************************************************//**
    @short      register/unregister interceptor for dispatch calls
    @descr      If you whish to handle some dispatches by himself ... you should be
                an interceptor for it. Please see class OInterceptionHelper for further informations.

    @seealso    class OInterceptionHelper

    @param      "xInterceptor", reference to your interceptor implementation.
    @return     -

    @onerror    Interceptor is ignored.
*//*-*****************************************************************************************************/
void SAL_CALL Frame::registerDispatchProviderInterceptor( const css::uno::Reference< css::frame::XDispatchProviderInterceptor >& xInterceptor ) throw( css::uno::RuntimeException )
{
    // We use a helper to support these interface and an interceptor mechanism.
    // This helper is threadsafe himself and check incoming parameter too.
    // I think we don't need any lock here!
    // But we must look for rejected calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    css::uno::Reference< css::frame::XDispatchProviderInterception > xInterceptionHelper( m_xDispatchHelper, css::uno::UNO_QUERY );
    xInterceptionHelper->registerDispatchProviderInterceptor( xInterceptor );
}

//*****************************************************************************************************************
void SAL_CALL Frame::releaseDispatchProviderInterceptor( const css::uno::Reference< css::frame::XDispatchProviderInterceptor >& xInterceptor ) throw( css::uno::RuntimeException )
{
    // We use a helper to support these interface and an interceptor mechanism.
    // This helper is threadsafe himself and check incoming parameter too.
    // I think we don't need any lock here!
    // But we must look for rejected calls ...
    // Sometimes we are called during our dispose() method ... => soft exceptions!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    css::uno::Reference< css::frame::XDispatchProviderInterception > xInterceptionHelper( m_xDispatchHelper, css::uno::UNO_QUERY );
    xInterceptionHelper->releaseDispatchProviderInterceptor( xInterceptor );
}

/*-****************************************************************************************************//**
    @short      notifications for window events
    @descr      We are a listener on our container window to forward it to our component window.

    @seealso    method setComponent()
    @seealso    member m_xContainerWindow
    @seealso    member m_xComponentWindow

    @param      "aEvent" describe source of detected event
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::windowResized( const css::awt::WindowEvent& aEvent ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_windowResized( aEvent ), "Frame::windowResized()", "Invalid parameter detected." )
    // Look for rejected calls.
    // Part of dispose-mechanism => soft exceptions
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // Impl-method is threadsafe!
    // If we have a current component window - we must resize it!
    implts_resizeComponentWindow();
}

//*****************************************************************************************************************
void SAL_CALL Frame::focusGained( const css::awt::FocusEvent& aEvent ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_focusGained( aEvent ), "Frame::focusGained()", "Invalid parameter detected." )
    // Look for rejected calls.
    // Part of dispose() mechanism ... => soft exceptions!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    // Make snapshot of member!
    css::uno::Reference< css::awt::XWindow > xComponentWindow = m_xComponentWindow;
    aReadLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    if( xComponentWindow.is() == sal_True )
    {
        xComponentWindow->setFocus();
    }
}

/*-****************************************************************************************************//**
    @short      notifications for window events
    @descr      We are a listener on our container window to forward it to our component window ...
                but a XTopWindowListener we are only if we are a top frame!

    @seealso    method setComponent()
    @seealso    member m_xContainerWindow
    @seealso    member m_xComponentWindow

    @param      "aEvent" describe source of detected event
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::windowActivated( const css::lang::EventObject& aEvent ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_windowActivated( aEvent ), "Frame::windowActivated()", "Invalid parameter detected." )
    // Look for rejected calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    // Make snapshot of member!
    EActiveState eState = m_eActiveState;
    aReadLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Activate the new active path from here to top.
    if( eState == E_INACTIVE )
    {
        CheckMenuCloser_Impl();
        setActiveFrame( css::uno::Reference< css::frame::XFrame >() );
        activate();
    }
}

//*****************************************************************************************************************
void SAL_CALL Frame::windowDeactivated( const css::lang::EventObject& aEvent ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_windowDeactivated( aEvent ), "Frame::windowDeactivated()", "Invalid parameter detected." )
    // Look for rejected calls.
    // Sometimes called during dispose() => soft exceptions
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    css::uno::Reference< css::frame::XFrame > xParent          ( m_xParent, css::uno::UNO_QUERY );
    css::uno::Reference< css::awt::XWindow >  xContainerWindow = m_xContainerWindow;
    EActiveState                              eActiveState     = m_eActiveState    ;

    aReadLock.unlock();

    if( eActiveState != E_INACTIVE )
    {
        // Deactivation is always done implicitely by activation of another frame.
        // Only if no activation is done, deactivations have to be processed if the activated window
        // is a parent window of the last active Window!
        ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
        CheckMenuCloser_Impl();
        Window* pFocusWindow = Application::GetFocusWindow();
        if  (
                ( xContainerWindow.is()                                                              ==  sal_True    )   &&
                ( xParent.is()                                                                       ==  sal_True    )   &&
                ( (css::uno::Reference< css::frame::XDesktop >( xParent, css::uno::UNO_QUERY )).is() ==  sal_False   )
            )
        {
            css::uno::Reference< css::awt::XWindow >  xParentWindow   = xParent->getContainerWindow()             ;
            Window*                                   pOwnWindow      = VCLUnoHelper::GetWindow( xContainerWindow );
            Window*                                   pParentWindow   = VCLUnoHelper::GetWindow( xParentWindow    );
            if( pFocusWindow==NULL || pParentWindow->IsChild( pFocusWindow ) )
            {
                css::uno::Reference< css::frame::XFramesSupplier > xSupplier( xParent, css::uno::UNO_QUERY );
                if( xSupplier.is() == sal_True )
                {
                    aSolarGuard.clear();
                    xSupplier->setActiveFrame( css::uno::Reference< css::frame::XFrame >() );
                }
            }
        }
    }
}

//*****************************************************************************************************************
void SAL_CALL Frame::windowClosing( const css::lang::EventObject& aEvent ) throw( css::uno::RuntimeException )
{
    /* Attention:
        Don't use any transaction registration here. Because we try to kill ourself.
        And if we have registered this function as non breakable and force a close()->dispose() operation
        this will wait for us for ever ...
     */

    // deactivate this frame and try to close it
    // But do it asynchron inside the main thread.
    // VCL has no fun to do such things outside his main thread :-(
    deactivate();
    m_aPoster.Post(0);
}

IMPL_LINK( Frame, implts_windowClosing, void*, pVoid )
{
    try
    {
        // try to close this frame
        // But don't deliver the ownership to anyone.
        // Because our "UI owner" will try it again if this request
        // will fail.
        // e.g. Somewhere try to open an impress document ... and during
        // load process the wizard dialog comes up. Then it's possible for
        // the user to click the window closer of the document window - this task
        // disagree but gets the owner shipt and practice suicide directly after
        // closing of the dialog. That's not fine.
        // We must ignore this close() request then and user can try it later again ...

        /* SAFE */
        ReadGuard aReadLock( m_aLock );
        css::uno::Reference< css::frame::XController > xController = m_xController;
        aReadLock.unlock();
        /* SAFE */

        if ( xController.is() && !xController->suspend(sal_True) )
            return 0;
        close( sal_False );
    }
    catch( css::util::CloseVetoException& )
    {
    }

    return 0;
}

/*-****************************************************************************************************//**
    @short      react for a show event for the internal container window
    @descr      Normaly we doesn't need this information realy. But we can use it to
                implement the special feature "trigger first visible task".

                Algorithm: - first we have to check if we are a top (task) frame
                             It's not enough to be a top frame! Because we MUST have the desktop as parent.
                             But frames without a parent are top too. So it's not possible to check isTop() here!
                             We have to look for the type of our parent.
                           - if we are a task frame, then we have to check if we are the first one.
                             We use a static variable to do so. They will be reset to afterwards be shure
                             that further calls of this method doesn't do anything then.
                           - Then we have to trigger the right event string on the global job executor.

    @seealso    css::task::JobExecutor

    @param      aEvent
                    describes the source of this event
                    We are not interested on this information. We are interested on the visible state only.

    @threadsafe yes
    @modified   31.07.2002 07:56, as96863
*//*-*****************************************************************************************************/
void SAL_CALL Frame::windowShown( const css::lang::EventObject& aEvent ) throw(css::uno::RuntimeException)
{
    static sal_Bool bFirstVisibleTask = sal_True;

    /* SAFE { */
    css::uno::Reference< css::frame::XDesktop >             xDesktopCheck( m_xParent, css::uno::UNO_QUERY );
    css::uno::Reference< css::lang::XMultiServiceFactory >  xFactory     = m_xFactory;
    /* } SAFE */

    if (xDesktopCheck.is())
    {
        /* STATIC SAFE { */
        WriteGuard aStaticWriteLock( LockHelper::getGlobalLock() );
        sal_Bool bMustBeTriggered  = bFirstVisibleTask;
                 bFirstVisibleTask = sal_False;
        aStaticWriteLock.unlock();
        /* } STATIC SAFE */

        if (bMustBeTriggered)
        {
            css::uno::Reference< css::task::XJobExecutor > xExecutor( xFactory->createInstance( SERVICENAME_JOBEXECUTOR ), css::uno::UNO_QUERY );
            if (xExecutor.is())
            {
                xExecutor->trigger( DECLARE_ASCII("onFirstVisibleTask") );
            }
        }
    }
}

/*-****************************************************************************************************//**
    @short      called by dispose of our windows!
    @descr      This object is forced to release all references to the interfaces given
                by the parameter source. We are a listener at our container window and
                should listen for his diposing.

    @seealso    XWindowListener
    @seealso    XTopWindowListener
    @seealso    XFocusListener

    @param      -
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::disposing( const css::lang::EventObject& aEvent ) throw( css::uno::RuntimeException )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Check incoming parameter.
    LOG_ASSERT2( implcp_disposing( aEvent ), "Frame::disposing()", "Invalid parameter detected." )
    // Look for rejected calls.
    // May be we are called during releasing our windows in our in dispose call!? => soft exceptions
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );

    if( aEvent.Source == m_xContainerWindow )
    {
        // NECCESSARY: Impl-method is threadsafe by himself!
        aWriteLock.unlock();
        implts_stopWindowListening();
        aWriteLock.lock();
        m_xContainerWindow = css::uno::Reference< css::awt::XWindow >();
    }
}

/*-************************************************************************************************************//**
    @interface  com.sun.star.document.XActionLockable
    @short      implement locking of frame/task from outside
    @descr      Sometimes we have problems to decide if closing of task is allowed. Because; frame/task
                could be used for pending loading jobs. So you can lock this object from outside and
                prevent instance against closing during using! But - don't do it in a wrong or expensive manner.
                Otherwise task couldn't die anymore!!!

    @seealso    interface XActionLockable
    @seeelso    method BaseDispatcher::implts_loadIt()
    @seeelso    method Desktop::loadComponentFromURL()

    @param      -
    @return     true if frame/task is locked
                false otherwise

    @onerror    -
    @threadsafe yes
*//*-*************************************************************************************************************/
sal_Bool SAL_CALL Frame::isActionLocked() throw( css::uno::RuntimeException )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );
    return( m_nExternalLockCount!=0);
}

//*****************************************************************************************************************
void SAL_CALL Frame::addActionLock() throw( css::uno::RuntimeException )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    ++m_nExternalLockCount;
}

//*****************************************************************************************************************
void SAL_CALL Frame::removeActionLock() throw( css::uno::RuntimeException )
{
    // Register no transaction here! Otherwhise we wait for ever inside possible
    // implts_checkSuicide()/dispose() request ...

    /* SAFE AREA */{
        WriteGuard aWriteLock( m_aLock );
        LOG_ASSERT2( m_nExternalLockCount<=0, "Frame::removeActionLock()", "Frame isn't locked! Possible multithreading problem detected." )
        --m_nExternalLockCount;
    }/* SAFE */

    implts_checkSuicide();
}

//*****************************************************************************************************************
void SAL_CALL Frame::setActionLocks( sal_Int16 nLock ) throw( css::uno::RuntimeException )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    WriteGuard aWriteLock( m_aLock );
    // Attention: If somewhere called resetActionLocks() before and get e.g. 5 locks ...
    //            and tried to set these 5 ones here after his operations ...
    //            we can't ignore setted requests during these two calls!
    //            So we must add(!) these 5 locks here.
    m_nExternalLockCount += nLock;
}

//*****************************************************************************************************************
sal_Int16 SAL_CALL Frame::resetActionLocks() throw( css::uno::RuntimeException )
{
    // Register no transaction here! Otherwhise we wait for ever inside possible
    // implts_checkSuicide()/dispose() request ...

    sal_Int16 nCurrentLocks = 0;
    /* SAFE */{
        WriteGuard aWriteLock( m_aLock );
        nCurrentLocks = m_nExternalLockCount;
        m_nExternalLockCount = 0;
    }/* SAFE */

    // Attention:
    // external lock count is 0 here every time ... but if
    // member m_bSelfClose is set to true too .... we call our own close()/dispose().
    // See close() for further informations
    implts_checkSuicide();

    return nCurrentLocks;
}

/*-****************************************************************************************************//**
    @short      try to convert a property value
    @descr      This method is calling from helperclass "OPropertySetHelper".
                Don't use this directly!
                You must try to convert the value of given propertyhandle and
                return results of this operation. This will be use to ask vetoable
                listener. If no listener have a veto, we will change value realy!
                ( in method setFastPropertyValue_NoBroadcast(...) )

    @seealso    OPropertySetHelper
    @seealso    setFastPropertyValue_NoBroadcast()

    @param      "aConvertedValue"   new converted value of property
    @param      "aOldValue"         old value of property
    @param      "nHandle"           handle of property
    @param      "aValue"            new value of property
    @return     sal_True if value will be changed, sal_FALSE otherway

    @onerror    IllegalArgumentException, if you call this with an invalid argument
*//*-*****************************************************************************************************/
sal_Bool SAL_CALL Frame::convertFastPropertyValue(          css::uno::Any&        aConvertedValue ,
                                                            css::uno::Any&        aOldValue       ,
                                                            sal_Int32             nHandle         ,
                                                    const   css::uno::Any&        aValue          ) throw( css::lang::IllegalArgumentException )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // We don't need any mutex or lock here ... if we work with our properties only!
    // The propertyset helper synchronize for us.
    // Bur if we try to work with some other member ... we must make it threadsafe!!!

    //  Check, if value of property will changed in method "setFastPropertyValue_NoBroadcast()".
    //  Return TRUE, if changed - else return FALSE.
    //  Attention:
    //      Method "impl_tryToChangeProperty()" can throw the IllegalArgumentException !!!

    //  Initialize state with FALSE !!!
    //  (Handle can be invalid)
    sal_Bool bReturn = sal_False;

    switch( nHandle )
    {
        case PROPERTYHANDLE_TITLE :
                bReturn = impl_tryToChangeProperty( implts_getTitleFromWindow(), aValue, aOldValue, aConvertedValue );
                break;

        case PROPERTYHANDLE_DISPATCHRECORDERSUPPLIER :
                bReturn = impl_tryToChangeProperty( m_xDispatchRecorderSupplier, aValue, aOldValue, aConvertedValue );
                break;

        #ifdef ENABLE_WARNINGS
        default :   LOG_WARNING( "Frame::convertFastPropertyValue()", "Invalid handle detected!" )
                    break;
        #endif
    }

    // Return state of operation.
    return bReturn ;
}

/*-****************************************************************************************************//**
    @short      set value of a transient property
    @descr      This method is calling from helperclass "OPropertySetHelper".
                Don't use this directly!
                Handle and value are valid everyway! You must set the new value only.
                After this, baseclass send messages to all listener automaticly.

    @seealso    OPropertySetHelper

    @param      "nHandle"   handle of property to change
    @param      "aValue"    new value of property
    @return     -

    @onerror    An exception is thrown.
*//*-*****************************************************************************************************/
void SAL_CALL Frame::setFastPropertyValue_NoBroadcast(          sal_Int32       nHandle ,
                                                        const   css::uno::Any&  aValue  ) throw( css::uno::Exception )
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // We don't need any mutex or lock here ... if we work with our properties only!
    // The propertyset helper synchronize for us.
    // Bur if we try to work with some other member ... we must make it threadsafe!!!

    // Search for right handle ... and try to set property value.
    switch ( nHandle )
    {
        case PROPERTYHANDLE_TITLE :
                {
                    ::rtl::OUString sTitle;
                    aValue >>= sTitle;
                    implts_setTitleOnWindow( sTitle );
                }
                break;

        case PROPERTYHANDLE_DISPATCHRECORDERSUPPLIER :
                aValue >>= m_xDispatchRecorderSupplier;
                break;

        #ifdef ENABLE_WARNINGS
        default :
                LOG_WARNING( "Frame::setFastPropertyValue_NoBroadcast()", "Invalid handle detected!" )
                break;
        #endif
    }
}

/*-****************************************************************************************************//**
    @short      get value of a transient property
    @descr      This method is calling from helperclass "OPropertySetHelper".
                Don't use this directly!

    @seealso    OPropertySetHelper

    @param      "nHandle"   handle of property to change
    @param      "aValue"    current value of property
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void SAL_CALL Frame::getFastPropertyValue(  css::uno::Any&  aValue  ,
                                            sal_Int32       nHandle ) const
{
    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // We don't need any mutex or lock here ... if we work with our properties only!
    // The propertyset helper synchronize for us.
    // Bur if we try to work with some other member ... we must make it threadsafe!!!

    // Search for right handle ... and try to set property value.
    switch( nHandle )
    {
        case PROPERTYHANDLE_TITLE :
                aValue <<= implts_getTitleFromWindow();
                break;

        case PROPERTYHANDLE_DISPATCHRECORDERSUPPLIER :
                aValue <<= m_xDispatchRecorderSupplier;
                break;

        #ifdef ENABLE_WARNINGS
        default :
                LOG_WARNING( "Frame::getFastPropertyValue()", "Invalid handle detected!" )
                break;
        #endif
    }
}

/*-****************************************************************************************************//**
    @short      return structure and information about transient properties
    @descr      This method is calling from helperclass "OPropertySetHelper".
                Don't use this directly!

    @seealso    OPropertySetHelper

    @param      -
    @return     structure with property-informations

    @onerror    -
*//*-*****************************************************************************************************/
::cppu::IPropertyArrayHelper& SAL_CALL Frame::getInfoHelper()
{
    // Optimize this method !
    // We initialize a static variable only one time. And we don't must use a mutex at every call!
    // For the first call; pInfoHelper is NULL - for the second call pInfoHelper is different from NULL!
    static ::cppu::OPropertyArrayHelper* pInfoHelper = NULL;

    if( pInfoHelper == NULL )
    {
        // Ready for multithreading
        ::osl::MutexGuard aGuard( LockHelper::getGlobalLock().getShareableOslMutex() );
        // Control this pointer again, another instance can be faster then these!
        if( pInfoHelper == NULL )
        {
            // Define static member to give structure of properties to baseclass "OPropertySetHelper".
            // "impl_getStaticPropertyDescriptor" is a non exported and static funtion, who will define a static propertytable.
            // "sal_True" say: Table is sorted by name.
            static ::cppu::OPropertyArrayHelper aInfoHelper( impl_getStaticPropertyDescriptor(), sal_True );
            pInfoHelper = &aInfoHelper;
        }
    }

    return (*pInfoHelper);
}

/*-****************************************************************************************************//**
    @short      return propertysetinfo
    @descr      You can call this method to get information about transient properties
                of this object.

    @seealso    OPropertySetHelper
    @seealso    XPropertySet
    @seealso    XMultiPropertySet

    @param      -
    @return     reference to object with information [XPropertySetInfo]

    @onerror    -
*//*-*****************************************************************************************************/
css::uno::Reference< css::beans::XPropertySetInfo > SAL_CALL Frame::getPropertySetInfo () throw (::com::sun::star::uno::RuntimeException)
{
    // Optimize this method !
    // We initialize a static variable only one time. And we don't must use a mutex at every call!
    // For the first call; pInfo is NULL - for the second call pInfo is different from NULL!
    static css::uno::Reference< css::beans::XPropertySetInfo >* pInfo = NULL ;

    if( pInfo == NULL )
    {
        // Ready for multithreading
        ::osl::MutexGuard aGuard( LockHelper::getGlobalLock().getShareableOslMutex() );
        // Control this pointer again, another instance can be faster then these!
        if( pInfo == NULL )
        {
            // Create structure of propertysetinfo for baseclass "OPropertySetHelper".
            // (Use method "getInfoHelper()".)
            static css::uno::Reference< css::beans::XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
            pInfo = &xInfo;
        }
    }

    return (*pInfo);
}

/*-****************************************************************************************************//**
    @short      build information struct of our supported properties
    @descr      We create it one times only and return it every time.

    @seealso    OPropertySetHelper
    @seealso    XPropertySet
    @seealso    XMultiPropertySet

    @param      -
    @return     const struct with information about supported properties

    @onerror    -
*//*-*****************************************************************************************************/
const css::uno::Sequence< css::beans::Property > Frame::impl_getStaticPropertyDescriptor()
{
    // Create a new static property array to initialize sequence!
    // Table of all predefined properties of this class. Its used from OPropertySetHelper-class!
    // Don't forget to change the defines (see begin of this file), if you add, change or delete a property in this list!!!
    // It's necessary for methods of OPropertySetHelper.
    // ATTENTION:
    //      YOU MUST SORT FOLLOW TABLE BY NAME !!!

    static const css::beans::Property pPropertys[] =
    {
        css::beans::Property( PROPERTYNAME_DISPATCHRECORDERSUPPLIER, PROPERTYHANDLE_DISPATCHRECORDERSUPPLIER, ::getCppuType((const css::uno::Reference< css::frame::XDispatchRecorderSupplier >*)NULL), css::beans::PropertyAttribute::TRANSIENT ),
        css::beans::Property( PROPERTYNAME_TITLE                   , PROPERTYHANDLE_TITLE                   , ::getCppuType((const ::rtl::OUString*)NULL)                                             , css::beans::PropertyAttribute::TRANSIENT ),
    };
    // Use it to initialize sequence!
    static const css::uno::Sequence< css::beans::Property > lPropertyDescriptor( pPropertys, PROPERTYCOUNT );
    // Return static "PropertyDescriptor"
    return lPropertyDescriptor;
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
sal_Bool Frame::impl_tryToChangeProperty(   const   ::rtl::OUString&    sProperty       ,
                                            const   css::uno::Any&      aValue          ,
                                                    css::uno::Any&      aOldValue       ,
                                                    css::uno::Any&      aConvertedValue ) throw( css::lang::IllegalArgumentException )
{
    // Set default return value.
    sal_Bool bReturn = sal_False;

    // Clear information of return parameter!
    aOldValue.clear();
    aConvertedValue.clear();

    // Get new value from any.
    // IllegalArgumentException() can be thrown!
    ::rtl::OUString sNewValue;
    ::cppu::convertPropertyValue( sNewValue, aValue );

    // If value change ...
    if( sNewValue != sProperty )
    {
        // ... set information of change.
        aOldValue.setValue      ( &sProperty, ::getCppuType((const ::rtl::OUString*)NULL) );
        aConvertedValue.setValue( &sNewValue, ::getCppuType((const ::rtl::OUString*)NULL) );
        // Return OK - "value will be change ..."
        bReturn = sal_True;
    }

    return bReturn;
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
sal_Bool Frame::impl_tryToChangeProperty(   const   css::uno::Reference< css::frame::XDispatchRecorderSupplier >&   xProperty       ,
                                            const   css::uno::Any&                                                  aValue          ,
                                                    css::uno::Any&                                                  aOldValue       ,
                                                    css::uno::Any&                                                  aConvertedValue ) throw( css::lang::IllegalArgumentException )
{
    // Set default return value.
    sal_Bool bReturn = sal_False;

    // Clear information of return parameter!
    aOldValue.clear();
    aConvertedValue.clear();

    // Get new value from any.
    // IllegalArgumentException() can be thrown!
    css::uno::Reference< css::frame::XDispatchRecorderSupplier > xNewValue;
    ::cppu::convertPropertyValue( xNewValue, aValue );

    // If value change ...
    if( xNewValue != xProperty )
    {
        // ... set information of change.
        aOldValue.setValue      ( &xProperty, ::getCppuType((const css::uno::Reference< css::frame::XDispatchRecorderSupplier >*)NULL) );
        aConvertedValue.setValue( &xNewValue, ::getCppuType((const css::uno::Reference< css::frame::XDispatchRecorderSupplier >*)NULL) );
        // Return OK - "value will be change ..."
        bReturn = sal_True;
    }

    return bReturn;
}

/*-****************************************************************************************************//**
    @short      dispose old container window and forget his reference
    @descr      Sometimes we must repair our "modal dialog parent mechanism" too!

    @seealso    -

    @param      "xWindow", reference to old container window to dispose it
    @return     An empty reference.

    @onerror    -
    @threadsafe NO!
*//*-*****************************************************************************************************/
void Frame::impl_disposeContainerWindow( css::uno::Reference< css::awt::XWindow >& xWindow )
{
    if( xWindow.is() == sal_True )
    {
        // All VclComponents are XComponents; so call dispose before discarding
        // a css::uno::Reference< XVclComponent >, because this frame is the owner of the window
        ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
        Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
        if  (
                ( pWindow                                !=  NULL     )   &&
                ( Application::GetDefModalDialogParent() ==  pWindow  )
            )
        {
            Application::SetDefModalDialogParent( NULL );
        }
        aSolarGuard.clear();

        xWindow->setVisible( sal_False );
        xWindow->dispose();
        xWindow = css::uno::Reference< css::awt::XWindow >();
    }
}

/*-****************************************************************************************************//**
    @short      send frame action event to our listener
    @descr      This method is threadsafe AND can be called by our dispose method too!

    @seealso    -

    @param      "aAction", describe the event for sending
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void Frame::implts_sendFrameActionEvent( const css::frame::FrameAction& aAction )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Sometimes used by dispose() => soft exceptions!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    // Log informations about order of events to file!
    // (only activated in debug version!)
    LOG_FRAMEACTIONEVENT( "Frame", m_sName, aAction )

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // Send css::frame::FrameAction event to all listener.
    // Get container for right listener.
    // FOLLOW LINES ARE THREADSAFE!!!
    // ( OInterfaceContainerHelper is synchronized with m_aListenerContainer! )
    ::cppu::OInterfaceContainerHelper* pContainer = m_aListenerContainer.getContainer( ::getCppuType( ( const css::uno::Reference< css::frame::XFrameActionListener >*) NULL ) );

    if( pContainer != NULL )
    {
        // Build action event.
        css::frame::FrameActionEvent aFrameActionEvent( static_cast< ::cppu::OWeakObject* >(this), this, aAction );

        // Get iterator for access to listener.
        ::cppu::OInterfaceIteratorHelper aIterator( *pContainer );
        // Send message to all listener.
        while( aIterator.hasMoreElements() == sal_True )
        {
            try
            {
                ((css::frame::XFrameActionListener*)aIterator.next())->frameAction( aFrameActionEvent );
            }
            catch( css::uno::RuntimeException& )
            {
                aIterator.remove();
            }
        }
    }
}

/*-****************************************************************************************************//**
    @short      helper to resize our component window
    @descr      A frame contains 2 windows - a container ~ and a component window.
                This method resize inner component window to full size of outer container window.
                This method is threadsafe AND can be called by our dispose method too!

    @seealso    -

    @param      -
    @return     -

    @onerror    -
*//*-*****************************************************************************************************/
void Frame::implts_resizeComponentWindow()
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Sometimes used by dispose() => soft exceptions!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    ReadGuard aReadLock( m_aLock );

    // Make snapshot of both windows.
    css::uno::Reference< css::awt::XWindow >   xContainerWindow    =   m_xContainerWindow  ;
    css::uno::Reference< css::awt::XWindow >   xComponentWindow    =   m_xComponentWindow  ;

    aReadLock.unlock();
    /* UNSAFE AREA ----------------------------------------------------------------------------------------- */

    // Work only if container window is set!
    if  (
            ( xContainerWindow.is() == sal_True )   &&
            ( xComponentWindow.is() == sal_True )
        )
    {
        // Get reference to his device.
        css::uno::Reference< css::awt::XDevice > xDevice( xContainerWindow, css::uno::UNO_QUERY );
        // Convert relativ size to output size.
        css::awt::Rectangle  aRectangle  = xContainerWindow->getPosSize();
        css::awt::DeviceInfo aInfo       = xDevice->getInfo();
        css::awt::Size       aSize       (   aRectangle.Width  - aInfo.LeftInset - aInfo.RightInset  ,
                                             aRectangle.Height - aInfo.TopInset  - aInfo.BottomInset );
        // Resize ouer component window.
        xComponentWindow->setPosSize( 0, 0, aSize.Width, aSize.Height, css::awt::PosSize::SIZE );
    }
}

/*-****************************************************************************************************//**
    @short      helper to set/get a title on/from our container window
    @descr      We need this impl method to make it threadsafe. Another reason is - we can't hold this value
                by using an own member ... because if somewhere change the title by using the vcl-window directly ...
                we never get this information! That's why we write and rewad this property direct via toolkit and vcl!

    @seealso    interface XVclWindowPeer

    @param      "sTitle", new value to set it on our window
    @return     Current title of our window.

    @onerror    We do nothing or return an empty value.
*//*-*****************************************************************************************************/
void Frame::implts_setTitleOnWindow( const ::rtl::OUString& sTitle )
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Look for rejected calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // Make snapshot of neccessary members and release lock.
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::awt::XVclWindowPeer > xContainerWindow( m_xContainerWindow, css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    // Use non well known feature of toolkit to set property on window!
    // -> cast window to XVclWindowPeer interface and call setProperty() ...
    if( xContainerWindow.is() == sal_True )
    {
        css::uno::Any aValue;
        aValue <<= sTitle;
        xContainerWindow->setProperty( DECLARE_ASCII("Title"), aValue );
    }
}

//*****************************************************************************************************************
const ::rtl::OUString Frame::implts_getTitleFromWindow() const
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Look for rejected calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // Make snapshot of neccessary members and release lock.
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::awt::XVclWindowPeer > xContainerWindow( m_xContainerWindow, css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    // Use non well known feature of toolkit to get property from window!
    // -> cast window to XVclWindowPeer interface and call getProperty() ...
    ::rtl::OUString sTitle;
    if( xContainerWindow.is() == sal_True )
    {
        css::uno::Any aValue = xContainerWindow->getProperty( DECLARE_ASCII("Title") );
        aValue >>= sTitle;
    }
    return sTitle;
}

/*-****************************************************************************************************//**
    @short      helper to set icon on our container window (if it is a system window!)
    @descr      We use our internal set controller (if it exist) to specify which factory he represanted.
                These information can be used to find right icon. But our controller can say it us directly
                too ... we should ask his optional property set first ...

    @seealso    method Window::SetIcon()

    @param      -
    @return     -

    @onerror    We do nothing.
*//*-*****************************************************************************************************/
void Frame::implts_setIconOnWindow()
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Look for rejected calls.
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // Make snapshot of neccessary members and release lock.
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::awt::XWindow >       xContainerWindow( m_xContainerWindow, css::uno::UNO_QUERY );
    css::uno::Reference< css::frame::XController > xController     ( m_xController     , css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    if(
        ( xContainerWindow.is() == sal_True )   &&
        ( xController.is()      == sal_True )
      )
    {
        //-------------------------------------------------------------------------------------------------------------
        // a) set default value to an invalid one. So we can start further searches for right icon id, if
        //    first steps failed!
        //    We must reset it to any fallback value - if no search step returns a valid result.
        sal_Int32 nIcon = -1;

        //-------------------------------------------------------------------------------------------------------------
        // b) try to find information on controller propertyset directly
        //    Don't forget to catch possible exceptions - because these property is an optional one!
        css::uno::Reference< css::beans::XPropertySet > xSet( xController, css::uno::UNO_QUERY );
        if( xSet.is() == sal_True )
        {
            try
            {
                css::uno::Any aID = xSet->getPropertyValue( DECLARE_ASCII("IconId") );
                if( aID.hasValue() == sal_True )
                {
                    aID >>= nIcon;
                }
            }
            catch( css::beans::UnknownPropertyException& )
            {
            }
        }

        //-------------------------------------------------------------------------------------------------------------
        // c) if b) failed ... analyze argument list of currently loaded document insde the frame to find the filter.
        //    He can be used to detect right factory - and these can be used to match factory to icon ...
        if( nIcon == -1 )
        {
            css::uno::Reference< css::frame::XModel > xModel = xController->getModel();
            if( xModel.is() == sal_True )
            {
                ::rtl::OUString  sFilter;
                ArgumentAnalyzer aAnalyzer(xModel->getArgs(),sal_True);
                aAnalyzer.getArgument( E_FILTERNAME, sFilter );

                if( sFilter.getLength() > 0 )
                {
                    FilterCache aCacheRef; // Cache use refcount and static impl_data_container! So we can use it so!
                    Filter      aFilter  = aCacheRef.getFilter( sFilter );

                    SvtModuleOptions::EFactory eFactory;
                    if( SvtModuleOptions::ClassifyFactoryByName( aFilter.sDocumentService, eFactory ) == sal_True )
                    {
                        nIcon = SvtModuleOptions().GetFactoryIcon( eFactory );
                    }
                }
            }
        }

        //-------------------------------------------------------------------------------------------------------------
        // d) if all steps failed - use fallback!
        if( nIcon == -1 )
        {
            nIcon = 0;
        }

        //-------------------------------------------------------------------------------------------------------------
        // e) set icon on container window now
        //    Don't forget SolarMutex! We use vcl directly :-(
        //    Check window pointer for right WorkWindow class too!!!
        /* SAFE AREA ----------------------------------------------------------------------------------------------- */
        ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
        Window* pWindow = (VCLUnoHelper::GetWindow( xContainerWindow ));
        if(
            ( pWindow            != NULL              ) &&
            ( pWindow->GetType() == WINDOW_WORKWINDOW )
        )
        {
            WorkWindow* pWorkWindow = (WorkWindow*)pWindow;
            pWorkWindow->SetIcon( (sal_uInt16)nIcon );
        }
        aSolarGuard.clear();
        /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    }
}

/*-************************************************************************************************************//**
    @short          filter special names
    @attention      If somewhere have a name value ... but don't know if he can set it on a frame ...
                    he should call this helper to clear all questions.
                    Some special target names are not allowed as frame name!
*//*-*************************************************************************************************************/
void Frame::impl_filterSpecialTargets( ::rtl::OUString& sTarget )
{
    if  (
            ( sTarget   ==  SPECIALTARGET_SELF      )   ||
            ( sTarget   ==  SPECIALTARGET_PARENT    )   ||
            ( sTarget   ==  SPECIALTARGET_TOP       )   ||
            ( sTarget   ==  SPECIALTARGET_DEFAULT   )   ||
            ( sTarget   ==  SPECIALTARGET_BLANK     )
        )
    {
        sTarget = ::rtl::OUString();
    }
}

/*-************************************************************************************************************//**
    @short      helper to start/stop listeneing for window events on container window
    @descr      If we get a new container window, we must set it on internal memeber ...
                and stop listening at old one ... and start listening on new one!
                But sometimes (in dispose() call!) it's neccessary to stop listeneing without starting
                on new connections. So we split this functionality to make it easier at use.

    @seealso    method initialize()
    @seealso    method dispose()

    @param      -
    @return     -

    @onerror    We do nothing!
    @threadsafe yes
*//*-*************************************************************************************************************/
void Frame::implts_startWindowListening()
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    TransactionGuard aTransaction( m_aTransactionManager, E_HARDEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // Make snapshot of neccessary member!
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::awt::XWindow >                            xContainerWindow    = m_xContainerWindow   ;
    css::uno::Reference< css::lang::XMultiServiceFactory >              xFactory            = m_xFactory           ;
    css::uno::Reference< css::datatransfer::dnd::XDropTargetListener >  xDragDropListener   = m_xDropTargetListener;
    css::uno::Reference< css::awt::XWindowListener >                    xWindowListener     ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    css::uno::Reference< css::awt::XFocusListener >                     xFocusListener      ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    css::uno::Reference< css::awt::XTopWindowListener >                 xTopWindowListener  ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    if( xContainerWindow.is() == sal_True )
    {
        xContainerWindow->addWindowListener( xWindowListener);
        xContainerWindow->addFocusListener ( xFocusListener );

        css::uno::Reference< css::awt::XTopWindow > xTopWindow( xContainerWindow, css::uno::UNO_QUERY );
        if( xTopWindow.is() == sal_True )
        {
            xTopWindow->addTopWindowListener( xTopWindowListener );

            css::uno::Reference< css::awt::XDataTransferProviderAccess > xTransfer( xFactory->createInstance( SERVICENAME_VCLTOOLKIT ), css::uno::UNO_QUERY );
            if( xTransfer.is() == sal_True )
            {
                css::uno::Reference< css::datatransfer::dnd::XDropTarget > xDropTarget = xTransfer->getDropTarget( xContainerWindow );
                if( xDropTarget.is() == sal_True )
                {
                    xDropTarget->addDropTargetListener( xDragDropListener );
                    xDropTarget->setActive( sal_True );
                }
            }
        }
    }
}

//*****************************************************************************************************************
void Frame::implts_stopWindowListening()
{
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */
    // Sometimes used by dispose() => soft exceptions!
    TransactionGuard aTransaction( m_aTransactionManager, E_SOFTEXCEPTIONS );

    /* SAFE AREA ----------------------------------------------------------------------------------------------- */
    // Make snapshot of neccessary member!
    ReadGuard aReadLock( m_aLock );
    css::uno::Reference< css::awt::XWindow >                            xContainerWindow    = m_xContainerWindow   ;
    css::uno::Reference< css::lang::XMultiServiceFactory >              xFactory            = m_xFactory           ;
    css::uno::Reference< css::datatransfer::dnd::XDropTargetListener >  xDragDropListener   = m_xDropTargetListener;
    css::uno::Reference< css::awt::XWindowListener >                    xWindowListener     ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    css::uno::Reference< css::awt::XFocusListener >                     xFocusListener      ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    css::uno::Reference< css::awt::XTopWindowListener >                 xTopWindowListener  ( static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY );
    aReadLock.unlock();
    /* UNSAFE AREA --------------------------------------------------------------------------------------------- */

    if( xContainerWindow.is() == sal_True )
    {
        xContainerWindow->removeWindowListener( xWindowListener);
        xContainerWindow->removeFocusListener ( xFocusListener );

        css::uno::Reference< css::awt::XTopWindow > xTopWindow( xContainerWindow, css::uno::UNO_QUERY );
        if( xTopWindow.is() == sal_True )
        {
            xTopWindow->removeTopWindowListener( xTopWindowListener );

            css::uno::Reference< css::awt::XDataTransferProviderAccess > xTransfer( xFactory->createInstance( SERVICENAME_VCLTOOLKIT ), css::uno::UNO_QUERY );
            if( xTransfer.is() == sal_True )
            {
                css::uno::Reference< css::datatransfer::dnd::XDropTarget > xDropTarget = xTransfer->getDropTarget( xContainerWindow );
                if( xDropTarget.is() == sal_True )
                {
                    xDropTarget->removeDropTargetListener( xDragDropListener );
                    xDropTarget->setActive( sal_False );
                }
            }
        }
    }
}

/*-****************************************************************************************************//**
    @short      helper to force breaked close() request again
    @descr      If we self disagree with a close() request, and detect that all external locks are gone ...
                then we must try to close this frame again.

    @seealso    XCloseable::close()
    @seealso    Frame::close()
    @seealso    Frame::removeActionLock()
    @seealso    Frame::resetActionLock()
    @seealso    m_bSelfClose
    @seealso    m_nExternalLockCount

    @threadsafe yes
    @modified   06.05.2002 09:31, as96863
*//*-*****************************************************************************************************/
void Frame::implts_checkSuicide()
{
    sal_Bool bSuicide = sal_False;
    /* SAFE */{
        // in case of lock==0 and safed state of previous close() request m_bSelfClose
        // we must force close() again. Because we had disagreed with that before.
        bSuicide     = (m_nExternalLockCount==0 && m_bSelfClose);
        m_bSelfClose = sal_False;
    }
    // force close and deliver owner ship to source of possible throwed veto exception
    // Attention: Because this method isn't designed to throw such exception we must supress
    // it for outside code!
    try
    {
        if (bSuicide)
            close(sal_True);
    }
    catch( css::util::CloseVetoException& )
    {
    }
}

void Frame::CheckMenuCloser_Impl()
{
    // checks if there is more than one "real" (not help) task window
    // in this case a close button is inserted into the menubar
    if ( !getController().is() )
        // dummy component
        return;

    css::uno::Reference < ::com::sun::star::frame::XFramesSupplier > xDesktop( getCreator(), css::uno::UNO_QUERY );
    if ( !xDesktop.is() )
        // test only for task windows
        return;

    sal_Bool bLastTask = sal_False;
    css::uno::Reference < ::com::sun::star::container::XIndexAccess >
            xList ( xDesktop->getFrames(), ::com::sun::star::uno::UNO_QUERY );
    sal_Int32 nCount = xList->getCount();
    if ( nCount<=1 )
        // only one task
        bLastTask = sal_True;
    else if ( nCount==2 )
    {
        // if we have to tasks, one can be the help task, that should be ignored
        for( sal_Int32 i=0; i<nCount; ++i )
        {
            css::uno::Reference < ::com::sun::star::frame::XFrame > xTask;
            ::com::sun::star::uno::Any aVal = xList->getByIndex(i);
            if ( (aVal>>=xTask) && xTask.is() && xTask->getName().compareToAscii("OFFICE_HELP_TASK") == COMPARE_EQUAL )
            {
                // one of the two open tasks was the help task -> ignored
                bLastTask = sal_True;
                break;
            }
        }
    }

    ::vos::OGuard aSolarGuard( Application::GetSolarMutex() );
    Window* pWindow = VCLUnoHelper::GetWindow( getContainerWindow() );
    if ( pWindow && pWindow->IsSystemWindow() )
    {
        MenuBar* pMenu = ((SystemWindow*)pWindow)->GetMenuBar();
        if ( pMenu )
            pMenu->ShowCloser(bLastTask);
    }
}

//_________________________________________________________________________________________________________________
//  debug methods
//_________________________________________________________________________________________________________________

/*-----------------------------------------------------------------------------------------------------------------
    The follow methods checks the parameter for other functions. If a parameter or his value is non valid,
    we return "sal_True". (otherwise sal_False) This mechanism is used to throw an ASSERT!
-----------------------------------------------------------------------------------------------------------------*/

#ifdef ENABLE_ASSERTIONS

//*****************************************************************************************************************
// We don't accept null pointer or references!
sal_Bool Frame::implcp_ctor( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory )
{
    return  (
                ( &xFactory     ==  NULL        )   ||
                ( xFactory.is() ==  sal_False   )
            );
}

//*****************************************************************************************************************
// Its allowed to reset the active frame membervariable with a NULL-css::uno::Reference but not with a NULL-pointer!
// And we accept frames only! No tasks and desktops!
sal_Bool Frame::implcp_setActiveFrame( const css::uno::Reference< css::frame::XFrame >& xFrame )
{
    return  (
                ( &xFrame                                                                                   ==  NULL        )   ||
                ( css::uno::Reference< css::frame::XDesktop >( xFrame, css::uno::UNO_QUERY ).is()           ==  sal_True    )   ||
                ( css::uno::Reference< css::mozilla::XPluginInstance >( xFrame, css::uno::UNO_QUERY ).is()  ==  sal_True    )
            );
}

//*****************************************************************************************************************
// We don't accept null pointer ... but NULL-References are allowed!
sal_Bool Frame::implcp_initialize( const css::uno::Reference< css::awt::XWindow >& xWindow )
{
    return( &xWindow == NULL );
}

//*****************************************************************************************************************
// We don't accept null pointer or references!
sal_Bool Frame::implcp_setCreator( const css::uno::Reference< css::frame::XFramesSupplier >& xCreator )
{
    return  (
                ( &xCreator     ==  NULL        )   ||
                ( xCreator.is() ==  sal_False   )
            );
}

//*****************************************************************************************************************
// We don't accept null pointer or references!
sal_Bool Frame::implcp_setName( const ::rtl::OUString& sName )
{
    return( &sName == NULL );
}

//*****************************************************************************************************************
// We don't accept null pointer or references!
// An empty target name is allowed => is the same like "_self"
sal_Bool Frame::implcp_findFrame(  const   ::rtl::OUString& sTargetFrameName,
                                            sal_Int32        nSearchFlags    )
{
    return( &sTargetFrameName == NULL );
}

//*****************************************************************************************************************
// We don't accept null pointer!
sal_Bool Frame::implcp_setComponent(   const   css::uno::Reference< css::awt::XWindow >&       xComponentWindow    ,
                                        const   css::uno::Reference< css::frame::XController >& xController         )
{
    return  (
                ( &xComponentWindow ==  NULL    )   ||
                ( &xController      ==  NULL    )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_addFrameActionListener( const css::uno::Reference< css::frame::XFrameActionListener >& xListener )
{
    return  (
                ( &xListener        ==  NULL        )   ||
                ( xListener.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_removeFrameActionListener( const css::uno::Reference< css::frame::XFrameActionListener >& xListener )
{
    return  (
                ( &xListener        ==  NULL        )   ||
                ( xListener.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_addEventListener( const css::uno::Reference< css::lang::XEventListener >& xListener )
{
    return  (
                ( &xListener        ==  NULL        )   ||
                ( xListener.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_removeEventListener( const css::uno::Reference< css::lang::XEventListener >& xListener )
{
    return  (
                ( &xListener        ==  NULL        )   ||
                ( xListener.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_windowResized( const css::awt::WindowEvent& aEvent )
{
    return  (
                ( &aEvent               ==  NULL        )   ||
                ( aEvent.Source.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_focusGained( const css::awt::FocusEvent& aEvent )
{
    return  (
                ( &aEvent               ==  NULL        )   ||
                ( aEvent.Source.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_windowActivated( const css::lang::EventObject& aEvent )
{
    return  (
                ( &aEvent               ==  NULL        )   ||
                ( aEvent.Source.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_windowDeactivated( const css::lang::EventObject& aEvent )
{
    return  (
                ( &aEvent               ==  NULL        )   ||
                ( aEvent.Source.is()    ==  sal_False   )
            );
}

//*****************************************************************************************************************
sal_Bool Frame::implcp_disposing( const css::lang::EventObject& aEvent )
{
    return  (
                ( &aEvent               ==  NULL        )   ||
                ( aEvent.Source.is()    ==  sal_False   )
            );
}

#endif  // #ifdef ENABLE_ASSERTIONS

}   // namespace framework
