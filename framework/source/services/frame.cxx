/*************************************************************************
 *
 *  $RCSfile: frame.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: mba $ $Date: 2001-03-15 10:35:49 $
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

#ifndef __FRAMEWORK_HELPER_ODISPATCHPROVIDER_HXX_
#include <helper/odispatchprovider.hxx>
#endif

#ifndef __FRAMEWORK_HELPER_OINTERCEPTIONHELPER_HXX_
#include <helper/ointerceptionhelper.hxx>
#endif

#ifndef __FRAMEWORK_HELPER_OFRAMES_HXX_
#include <helper/oframes.hxx>
#endif

#ifndef __FRAMEWORK_HELPER_OSTATUSINDICATORFACTORY_HXX_
#include <helper/ostatusindicatorfactory.hxx>
#endif

#ifndef __FRAMEWORK_CLASSES_TARGETFINDER_HXX_
#include <classes/targetfinder.hxx>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_AWT_XDEVICE_HPP_
#include <com/sun/star/awt/XDevice.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XTOPWINDOW_HPP_
#include <com/sun/star/awt/XTopWindow.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XTASK_HPP_
#include <com/sun/star/frame/XTask.hpp>
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

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
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

#ifdef DEBUG
    #ifndef _RTL_STRBUF_HXX_
    #include <rtl/strbuf.hxx>
    #endif
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

using namespace ::com::sun::star                            ;
using namespace ::com::sun::star::beans                     ;
using namespace ::com::sun::star::container                 ;
using namespace ::com::sun::star::frame                     ;
using namespace ::com::sun::star::lang                      ;
using namespace ::com::sun::star::task                      ;
using namespace ::com::sun::star::uno                       ;
using namespace ::com::sun::star::util                      ;
using namespace ::cppu                                      ;
using namespace ::osl                                       ;
using namespace ::rtl                                       ;

//_________________________________________________________________________________________________________________
//  non exported const
//_________________________________________________________________________________________________________________

#define DEFAULT_EACTIVESTATE                                INACTIVE
#define DEFAULT_BRECURSIVESEARCHPROTECTION                  sal_False
#define DEFAULT_BISFRAMETOP                                 sal_True
#define DEFAULT_BALREADYDISPOSED                            sal_False
#define DEFAULT_BCONNECTED                                  sal_True
#define DEFAULT_SNAME                                       OUString()

//_________________________________________________________________________________________________________________
//  non exported definitions
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  declarations
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//  constructor
//*****************************************************************************************************************
Frame::Frame( const Reference< XMultiServiceFactory >& xFactory )
        //  Init baseclasses first
        //  Attention:
        //      Don't change order of initialization!
        //      OMutexMember is a struct with a mutex as member. We can't use a mutex as member, while
        //      we must garant right initialization and a valid value of this! First initialize
        //      baseclasses and then members. And we need the mutex for other baseclasses !!!
        :   OMutexMember                (                                   )
        ,   OWeakObject                 (                                   )
        // Init member
        ,   m_xFactory                  ( xFactory                          )
        ,   m_aListenerContainer        ( m_aMutex                          )
        ,   m_aChildFrameContainer      (                                   )
        // Init flags
        ,   m_eActiveState              ( DEFAULT_EACTIVESTATE              )
        ,   m_bRecursiveSearchProtection( DEFAULT_BRECURSIVESEARCHPROTECTION)
        ,   m_bIsFrameTop               ( DEFAULT_BISFRAMETOP               )
        ,   m_bAlreadyDisposed          ( DEFAULT_BALREADYDISPOSED          )
        ,   m_bConnected                ( DEFAULT_BCONNECTED                )
        ,   m_sName                     ( DEFAULT_SNAME                     )
{
    // We cant create the dispatchhelper and frameshelper, because they hold wekreferences to us!
    // But with a HACK (++refcount) its "OK" :-(
    ++m_refCount ;

    // Initialize a new dispatchhelper-object to handle dispatches for SELF private and fast!
    // We use these helper as slave for our interceptor helper ...
    // (Attention: These helper hold a weakreference to us!)
#if SUPD>614
    ODispatchProvider* pDispatchHelper = new ODispatchProvider( m_xFactory, this );
#else
    ODispatchProvider* pDispatchHelper = new ODispatchProvider( m_xFactory, this, m_aMutex );
#endif

    // Initialize a new interception helper object to handle dispatches and interceptor mechanism PRIVATE!
    // These helper don't need any reference to use ...
    OInterceptionHelper* pInterceptionHelper = new OInterceptionHelper(  Reference< XFrame >( this ), Reference< XDispatchProvider >( static_cast< OWeakObject* >( pDispatchHelper ), UNO_QUERY ) );
    m_xDispatchHelper = Reference< XDispatchProvider >( static_cast< OWeakObject* >(pInterceptionHelper), UNO_QUERY );

    // Initialize a new frameshelper-object to handle indexaccess and elementaccess!
    // Attention: OFrames need the this-pointer for initializing. You must use "this" directly.
    // If you define an extra variable to do that (like: Reference< XFrame > xTHIS( ... )) and
    // forget to clear this reference BEFORE "--m_refCount" (!), your refcount will be less then 0
    // and the new Desktop-instance will be destroyed instantly!!!...
    OFrames* pFramesHelper  = new OFrames( m_xFactory, m_aMutex, this, &m_aChildFrameContainer );
    m_xFramesHelper = Reference< XFrames >( static_cast< OWeakObject* >(pFramesHelper), UNO_QUERY );

    // Safe impossible cases
    // We can't work without these helpers!
    LOG_ASSERT( !(m_xDispatchHelper.is()==sal_False), "Frame::Frame()\nDispatchHelper is not valid. XDispatchProvider, XDispatch, XDispatchProviderInterception are not supported!\n"   )
    LOG_ASSERT( !(m_xFramesHelper.is  ()==sal_False), "Frame::Frame()\nFramesHelper is not valid. XFrames, XIndexAccess and XElementAcces are not supported!\n"                         )

    // Don't forget these - or we live for ever!
    --m_refCount ;
}

//*****************************************************************************************************************
//  destructor
//*****************************************************************************************************************
Frame::~Frame()
{
}

//*****************************************************************************************************************
//  XInterface, XTypeProvider, XServiceInfo
//*****************************************************************************************************************

DEFINE_XINTERFACE_13                (   Frame                                               ,
                                        OWeakObject                                         ,
                                        DIRECT_INTERFACE(XTypeProvider                      ),
                                        DIRECT_INTERFACE(XServiceInfo                       ),
                                        DIRECT_INTERFACE(XFramesSupplier                    ),
                                        DIRECT_INTERFACE(XFrame                             ),
                                        DIRECT_INTERFACE(XComponent                         ),
                                        DIRECT_INTERFACE(XStatusIndicatorFactory            ),
                                        DIRECT_INTERFACE(XDispatchProvider                  ),
                                        DIRECT_INTERFACE(XDispatchProviderInterception      ),
                                        DIRECT_INTERFACE(XBrowseHistoryRegistry             ),
                                        DIRECT_INTERFACE(awt::XWindowListener                   ),
                                        DIRECT_INTERFACE(awt::XTopWindowListener                ),
                                        DIRECT_INTERFACE(awt::XFocusListener                    ),
                                        DERIVED_INTERFACE(XEventListener, awt::XWindowListener  )
                                    )

DEFINE_XTYPEPROVIDER_13             (   Frame                               ,
                                        XTypeProvider                       ,
                                        XServiceInfo                        ,
                                        XFramesSupplier                     ,
                                        XFrame                              ,
                                        XComponent                          ,
                                        XStatusIndicatorFactory             ,
                                        XDispatchProvider                   ,
                                        XDispatchProviderInterception       ,
                                        XBrowseHistoryRegistry              ,
                                        awt::XWindowListener                ,
                                        awt::XTopWindowListener             ,
                                        awt::XFocusListener                 ,
                                        XEventListener
                                    )

DEFINE_XSERVICEINFO_MULTISERVICE    (   Frame                               ,
                                        SERVICENAME_FRAME                   ,
                                        IMPLEMENTATIONNAME_FRAME
                                    )

//*****************************************************************************************************************
//   XFramesSupplier
//*****************************************************************************************************************
Reference< XFrames > SAL_CALL Frame::getFrames() throw( RuntimeException )
{
    // Return access to all child frames to caller.
    // Ouer childframe container is implemented in helper class OFrames and used as a member m_xFramesHelper!
    return m_xFramesHelper;
}

//*****************************************************************************************************************
//   XFramesSupplier
//*****************************************************************************************************************
Reference< XFrame > SAL_CALL Frame::getActiveFrame() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::getActiveFrame()" )

    // Return current active frame.
    // This information is avaliable on the container.
    return m_aChildFrameContainer.getActive();
}

//*****************************************************************************************************************
//   XFramesSupplier
//*****************************************************************************************************************
void SAL_CALL Frame::setActiveFrame( const Reference< XFrame >& xFrame ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::setActiveFrame()" )

    // Safe impossible cases
    // This method is not defined for all incoming parameters.
    // I accept valid frames only. No tasks or desktops!
    // (But a NULL-reference stop down search in tree and is allowed!)
    LOG_ASSERT( impldbg_checkParameter_setActiveFrame( xFrame ), "Frame::setActiveFrame()\nInvalid parameter detected.\n" )

    // We don't safe the current active frame directly in this class! We set the information at container.
    // This is neccessar to control, if the active frame is a direct child of us!
    Reference< XFrame > xActiveChild = m_aChildFrameContainer.getActive();

    // Don't work, if "new" active frame is'nt different from current one!
    if ( xActiveChild != xFrame )
    {
        // Set the new active child frame.
        m_aChildFrameContainer.setActive( xFrame );
        if( isActive() && xActiveChild.is() )
            xActiveChild->deactivate();
    }

    if ( xFrame.is() )
    {
        if( m_eActiveState == FOCUS )
        {
            m_eActiveState = ACTIVE;
            impl_sendFrameActionEvent( FrameAction_FRAME_UI_DEACTIVATING );
        }

        if ( m_eActiveState == ACTIVE && !xFrame->isActive() )
            xFrame->activate();
    }
    else if ( m_eActiveState == ACTIVE )
    {
        // If this frame is active and has no active subframe anymore it is UI active too
        m_eActiveState = FOCUS;
        impl_sendFrameActionEvent( FrameAction_FRAME_UI_ACTIVATED );
    }
}

//*****************************************************************************************************************
//   XStatusIndicatorFactory
//*****************************************************************************************************************
Reference< XStatusIndicator > SAL_CALL Frame::createStatusIndicator() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::createStatusIndicator()" )

    // Forward operation to our helper.
    return m_xIndicatorFactoryHelper->createStatusIndicator();
}

//*****************************************************************************************************************
//   XDispatchProvider
//*****************************************************************************************************************
Reference< XDispatch > SAL_CALL Frame::queryDispatch(   const   URL&        aURL            ,
                                                        const   OUString&   sTargetFrameName,
                                                                sal_Int32   nSearchFlags    ) throw( RuntimeException )
{
    // We use a helper to support these interface and an interceptor mechanism.
    // These helper implementation use his own mutex and check incoming parameter for us!
    return m_xDispatchHelper->queryDispatch( aURL, sTargetFrameName, nSearchFlags );
}

//*****************************************************************************************************************
//   XDispatchProvider
//*****************************************************************************************************************
Sequence< Reference< XDispatch > > SAL_CALL Frame::queryDispatches( const Sequence< DispatchDescriptor >& seqDescriptor ) throw( RuntimeException )
{
    // We use a helper to support these interface and an interceptor mechanism.
    // These helper implementation use his own mutex and check incoming parameter for us!
    return m_xDispatchHelper->queryDispatches( seqDescriptor );
}

//*****************************************************************************************************************
//   XDispatchProviderInterception
//*****************************************************************************************************************
void SAL_CALL Frame::registerDispatchProviderInterceptor( const Reference< XDispatchProviderInterceptor >& xInterceptor ) throw( RuntimeException )
{
    // We use a helper to support these interface and an interceptor mechanism.
    // These helper implementation use his own mutex and check incoming parameter for us!
    Reference< XDispatchProviderInterception > xHelper( m_xDispatchHelper, UNO_QUERY );
    xHelper->registerDispatchProviderInterceptor( xInterceptor );
}

//*****************************************************************************************************************
//   XDispatchProviderInterception
//*****************************************************************************************************************
void SAL_CALL Frame::releaseDispatchProviderInterceptor( const Reference< XDispatchProviderInterceptor >& xInterceptor ) throw( RuntimeException )
{
    // We use a helper to support these interface and an interceptor mechanism.
    // These helper implementation use his own mutex and check incoming parameter for us!
    Reference< XDispatchProviderInterception > xHelper( m_xDispatchHelper, UNO_QUERY );
    xHelper->releaseDispatchProviderInterceptor( xInterceptor );
}

//*****************************************************************************************************************
//   XBrowseHistoryRegistry
//*****************************************************************************************************************
void SAL_CALL Frame::updateViewData( const Any& aValue ) throw( RuntimeException )
{
    LOG_ASSERT( sal_False, "Frame::updateViewData()\nNot implemented yet!\n" )
}

//*****************************************************************************************************************
//   XBrowseHistoryRegistry
//*****************************************************************************************************************
void SAL_CALL Frame::createNewEntry(    const   OUString&                   sURL        ,
                                          const Sequence< PropertyValue >&  seqArguments,
                                        const   OUString&                   sTitle      ) throw( RuntimeException )
{
    LOG_ASSERT( sal_False, "Frame::createNewEntry()\nNot implemented yet!\n" )
}

//*****************************************************************************************************************
//   awt::XWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowResized( const awt::WindowEvent& aEvent ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::windowResized()" )
    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_windowResized( aEvent ), "Frame::windowResized()\nInvalid parameter detected.\n" )

    // If we have a current component window - we must resize it!
    impl_resizeComponentWindow();
}

//*****************************************************************************************************************
//   awt::XWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowMoved( const awt::WindowEvent& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   awt::XWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowShown( const EventObject& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   awt::XWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowHidden( const EventObject& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   XTopWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowOpened( const EventObject& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   XTopWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowClosing( const EventObject& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   XTopWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowClosed( const EventObject& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   XTopWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowMinimized( const EventObject& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   XTopWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowNormalized( const EventObject& aEvent ) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//   XTopWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowActivated( const EventObject& aEvent ) throw( RuntimeException )
{
    // Activate the new active path from here to top.
    if  ( m_eActiveState == INACTIVE )
    {
        LOCK_MUTEX( aGuard, m_aMutex, "Frame::windowActivated()" )

        // Safe impossible cases
        LOG_ASSERT( impldbg_checkParameter_windowActivated( aEvent ), "Frame::windowActivated()\nInvalid parameter detected.\n" )

        setActiveFrame( Reference< XFrame >() );
        activate();
    }
}

//*****************************************************************************************************************
//   XTopWindowListener
//*****************************************************************************************************************
void SAL_CALL Frame::windowDeactivated( const EventObject& aEvent ) throw( RuntimeException )
{
    if( m_eActiveState != INACTIVE )
    {
        LOCK_MUTEX( aGuard, m_aMutex, "Frame::windowDeactivated()" )
        // Safe impossible cases
        LOG_ASSERT( impldbg_checkParameter_windowDeactivated( aEvent ), "Frame::windowDeactivated()\nInvalid parameter detected.\n" )

        // Deactivation is always done implicitely by activation of another frame.
        // Only if no activation is done, deactivations have to be processed if the activated window
        // is a parent window of the last active Window!
        Reference< awt::XWindowPeer >   xOwnWindow  ( m_xContainerWindow, UNO_QUERY );
        Window*                         pFocusWindow= Application::GetFocusWindow();
        if  (
                ( xOwnWindow.is()                                       ==  sal_True    )   &&
                ( pFocusWindow                                          !=  NULL        )   &&
                ( m_xParent.is()                                        ==  sal_True    )   &&
                ( (Reference< XDesktop >( m_xParent, UNO_QUERY )).is()  ==  sal_False   )
            )
        {
            Reference< awt::XWindow >   xParentWindow   = m_xParent->getContainerWindow()                           ;
            Window*                 pOwnWindow      = VCLUnoHelper::GetWindow( xOwnWindow      )   ;
            Window*                 pParentWindow   = VCLUnoHelper::GetWindow( xParentWindow   )   ;
            if( pParentWindow->IsChild( pFocusWindow ) )
            {
                m_xParent->setActiveFrame( Reference< XFrame >() );
            }
        }
    }
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::initialize( const Reference< awt::XWindow >& xWindow ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::initialize()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_initialize( xWindow ), "Frame::initialize()\nInvalid parameter detected.\n"              )
    LOG_ASSERT( !(m_xContainerWindow.is() == sal_True )     , "Frame::initialize()\nMethod already called! Don't do it again.\n")

    // Protection against more then one calls ...
    if ( m_xContainerWindow.is() == sal_False )
    {
        // ... and set the new window.
        impl_setContainerWindow( xWindow );
        // Now we can use our indicator factory helper to support XStatusIndicatorFactory interface.
        // We have a valid parent window for it!
        // Initialize helper.
        OStatusIndicatorFactory* pIndicatorFactoryHelper = new OStatusIndicatorFactory( m_xFactory, m_xContainerWindow );
        m_xIndicatorFactoryHelper = Reference< XStatusIndicatorFactory >( static_cast< OWeakObject* >( pIndicatorFactoryHelper ), UNO_QUERY );
    }
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
Reference< awt::XWindow > SAL_CALL Frame::getContainerWindow() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::getContainerWindow()" )

    // Return reference to my own window - if it exist!
    return m_xContainerWindow;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::setCreator( const Reference< XFramesSupplier >& xCreator ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::setCreator()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_setCreator( xCreator ), "Frame::setCreator()\nInvalid parameter detected.\n" )

    // Safe new reference to different parent.
    m_xParent = xCreator;
    // Set/reset "is top" flag, if ouer new parent a frame, task or a desktop.
    m_bIsFrameTop = impl_willFrameTop( m_xParent );
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
Reference< XFramesSupplier > SAL_CALL Frame::getCreator() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::getCreator()" )

    // Return reference to my creator - It's my parent too.
    return m_xParent;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
OUString SAL_CALL Frame::getName() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::getName()" )

    // Return name of this frame.
    return m_sName;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::setName( const OUString& sName ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::setName()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_setName( sName ), "Frame::setName()\nInvalid parameter detected.\n" )

    if ( sName != SPECIALTARGET_BLANK &&
         sName != SPECIALTARGET_SELF &&
         sName != SPECIALTARGET_PARENT &&
         sName != SPECIALTARGET_TOP )
    {
        // Take the new one.
        m_sName = sName;
    }
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
Reference< XFrame > SAL_CALL Frame::findFrame(  const   OUString&   sTargetFrameName    ,
                                                            sal_Int32   nSearchFlags        ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::findFrame()" )
    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_findFrame( sTargetFrameName, nSearchFlags ), "Frame::findFrame()\nInvalid parameter detected.\n" )

    // Set default return value if method failed.
    Reference< XFrame > xSearchedFrame;

    // Protection against recursion while searching in parent frames!
    // See switch-statement eSIBLINGS, eALL for further informations.
    if( m_bRecursiveSearchProtection == sal_False )
    {
        LOG_PARAMETER_FINDFRAME( "Frame", m_sName, sTargetFrameName, nSearchFlags )
        // Use helper to classify search direction.
        // Attention: If he return ...BOTH -> please search down first; upper then!
        Reference< XFrame > xParent          ( m_xParent, UNO_QUERY )   ;
        sal_Bool            bCreationAllowed = sal_False                ;
        ETargetClass eResult = TargetFinder::classify(  E_FRAME                                         ,
                                                           sTargetFrameName                             ,
                                                        nSearchFlags                                    ,
                                                        bCreationAllowed                                ,
                                                        m_aChildFrameContainer.hasElements()            ,
                                                        m_sName                                         ,
                                                        xParent.is()                                    ,
                                                        xParent.is() ? xParent->getName() : OUString()  );
        switch( eResult )
        {
            case E_SELF         :   {
                                        xSearchedFrame = this;
                                    }
                                    break;
            case E_PARENT       :   {
                                        xSearchedFrame = xParent;
                                    }
                                    break;
            case E_FORWARD_UP   :   {
                                        m_bRecursiveSearchProtection = sal_True;
                                        xSearchedFrame = m_xParent->findFrame( sTargetFrameName, nSearchFlags );
                                        m_bRecursiveSearchProtection = sal_False;
                                    }
                                    break;
            case E_DEEP_DOWN    :   {
                                        xSearchedFrame = m_aChildFrameContainer.searchDeepDown( sTargetFrameName );
                                    }
                                    break;
            case E_DEEP_BOTH    :   {
                                        xSearchedFrame = m_aChildFrameContainer.searchDeepDown( sTargetFrameName );
                                        if( xSearchedFrame.is() == sal_False )
                                        {
                                            m_bRecursiveSearchProtection = sal_True;
                                            xSearchedFrame = m_xParent->findFrame( sTargetFrameName, nSearchFlags );
                                            m_bRecursiveSearchProtection = sal_False;
                                        }
                                    }
                                    break;
            case E_FLAT_DOWN    :   {
                                        xSearchedFrame = m_aChildFrameContainer.searchFlatDown( sTargetFrameName );
                                    }
                                    break;
            case E_FLAT_BOTH    :   {
                                        xSearchedFrame = m_aChildFrameContainer.searchFlatDown( sTargetFrameName );
                                        if( xSearchedFrame.is() == sal_False )
                                        {
                                            m_bRecursiveSearchProtection = sal_True;
                                            xSearchedFrame = m_xParent->findFrame( sTargetFrameName, nSearchFlags );
                                            m_bRecursiveSearchProtection = sal_False;
                                        }
                                    }
                                    break;
            #ifdef ENABLE_ASSERTIONS
            default:    LOG_ERROR( "Frame::findFrame()", "Unexpected result of TargetFinder::classify() detected!" )
                        break;
            #endif
        }
        LOG_RESULT_FINDFRAME( "Frame", m_sName, xSearchedFrame )
    }
    // Return result of operation.
    return xSearchedFrame;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
sal_Bool SAL_CALL Frame::isTop() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::isTop()" )

    // Return state of this instance.
    // This information is set in setCreator()!
    // We are top, if ouer parent is a task or the desktop.
    return m_bIsFrameTop;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::activate() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::activate()" )

    // Get the current active child frame.
    Reference< XFrame > xActiveChild = m_aChildFrameContainer.getActive();

    //_____________________________________________________________________________________________________________
    //  1)
    //  If I'am not active before ...
    if ( m_eActiveState == INACTIVE )
    {
        // ... do it then.
        m_eActiveState = ACTIVE;
        // Deactivate sibling path and forward activation to parent ... if any parent exist!
        if ( m_xParent.is() == sal_True )
        {
            // Everytime set THIS frame as active child of parent and activate it.
            // We MUST have a valid path from bottom to top as active path!
            // But we must deactivate the old active sibling path first.

            // Attention: Deactivation of an active path, deactivate the whole path ... from bottom to top!
            // But we wish to deactivate founded sibling-tree only.
            // [ see deactivate() / step 4) for further informations! ]

            m_xParent->setActiveFrame( this );

            // Then we can activate from here to top.
            // Attention: We are ACTIVE now. And the parent will call activate() at us!
            // But we do nothing then! We are already activated.
            m_xParent->activate();
        }
        // Its neccessary to send event NOW - not before.
        // Activation goes from bottom to top!
        // Thats the reason to activate parent first and send event now.
        impl_sendFrameActionEvent( FrameAction_FRAME_ACTIVATED );
    }

    //_____________________________________________________________________________________________________________
    //  2)
    //  Else;
    //  I was active before or current activated and there is a path from here to bottom, who CAN be active.
    //  But ouer direct child of path is not active yet.
    //  (It can be, if activation occur in the middle of a current path!)
    //  In these case we activate path to bottom to set focus on right frame!
    if  (
            ( m_eActiveState            ==  ACTIVE      )   &&
            ( xActiveChild.is()         ==  sal_True    )   &&
            ( xActiveChild->isActive()  ==  sal_False   )
        )
    {
        xActiveChild->activate();
    }

    //_____________________________________________________________________________________________________________
    //  3)
    //  I was active before or current activated. But if i have no active child => i will become the focus!
    if  (
            ( m_eActiveState    ==  ACTIVE      )   &&
            ( xActiveChild.is() ==  sal_False   )
        )
    {
        // Set FOCUS-state and send event to all listener.
//      if( m_xComponentWindow.is() == sal_True )
//      {
//          m_xComponentWindow->setFocus();
//      }
        m_eActiveState = FOCUS;
        impl_sendFrameActionEvent( FrameAction_FRAME_UI_ACTIVATED );
    }
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::deactivate() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::deactivate()" )

    // Work only, if there something to do!
    if ( m_eActiveState != INACTIVE )
    {
        //_____________________________________________________________________________________________________________
        //  1)
        //  Deactivate all active childs.
        Reference< XFrame > xActiveChild = m_aChildFrameContainer.getActive();
        if (( xActiveChild.is() == sal_True ) && ( xActiveChild->isActive() == sal_True ))
        {
            xActiveChild->deactivate();
        }

        //_____________________________________________________________________________________________________________
        //  2)
        //  If i have the focus - i will lost it now.
        if ( m_eActiveState == FOCUS )
        {
            // Set new state INACTIVE(!) and send message to all listener.
            // Don't set ACTIVE as new state. This frame is deactivated for next time - due to activate().
            m_eActiveState = ACTIVE;
            impl_sendFrameActionEvent( FrameAction_FRAME_UI_DEACTIVATING );
        }

        //_____________________________________________________________________________________________________________
        //  3)
        //  If i'am active - i will be deactivated now.
        if ( m_eActiveState == ACTIVE )
        {
            // Set new state and send message to all listener.
            m_eActiveState = INACTIVE;
            impl_sendFrameActionEvent( FrameAction_FRAME_DEACTIVATING );
        }

        //_____________________________________________________________________________________________________________
        //  4)
        //  If there is a path from here to my parent ...
        //  ... I'am on the top or in the middle of deactivated subtree and action was started here.
        //  I must deactivate all frames from here to top, which are members of current path.
        //  Stop, if THESE frame not the active frame of ouer parent!
        Reference< XFrame > xTHIS( (OWeakObject*)this, UNO_QUERY );
        if  (
                ( m_xParent.is()                ==  sal_True    )   &&
                ( m_xParent->getActiveFrame()   ==  xTHIS       )
            )
        {
            // We MUST break the path - otherwise we will get the focus - not ouer parent! ...
            // Attention: Ouer parent don't call us again - WE ARE NOT ACTIVE YET!
            // [ see step 3 and condition "if ( m_eActiveState!=INACTIVE ) ..." in this method! ]
            m_xParent->deactivate();
        }
    }
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
sal_Bool SAL_CALL Frame::isActive() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::isActive()" )

    // Set default return value to NO.
    sal_Bool bReturn = sal_False;

    // If i'am a member of the current active path ... reset return value to YES.
    if  (
            ( m_eActiveState == ACTIVE  )   ||
            ( m_eActiveState == FOCUS   )
        )
    {
        bReturn = sal_True;
    }

    // Return result of this operation.
    return bReturn;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
sal_Bool SAL_CALL Frame::setComponent(  const   Reference< awt::XWindow >&      xComponentWindow    ,
                                          const Reference< XController >&   xController         ) throw( RuntimeException )
{
    /* HACK for sfx2! */
    if ( xController.is() && !xComponentWindow.is() )
        return sal_False;
    /* HACK for sfx2! */

    // mutex should be locked as short as possible, because otherwise deadlocks in multithreaded environments
    // are guaranteed, because (de)registering listeners or disposing a VCL componente always tries to get
    // the solar mutex; if this mutex is hold in another thread, this thread would be blocked if it tried to
    // access this frame ( f.e. when a focus lost event should be processed )
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::setComponent()" )

    LOG_ASSERT( impldbg_checkParameter_setComponent( xComponentWindow, xController ), "Frame::setComponent()\nInvalid parameter detected.\n" )

    // always release controller before releasing window, because controller may want to access its window
    sal_Bool bNewController = ( m_xController       != xController      );
    sal_Bool bNewWindow     = ( m_xComponentWindow  != xComponentWindow );
    sal_Bool bHasController = m_xController.is();
    sal_Bool bHasWindow     = m_xComponentWindow.is();

    UNLOCK_MUTEX( aGuard, "Frame::setComponent()" )

    // Release current component, if there is any
    if  ( bHasController || bHasWindow )
    {
        // Send FrameAction event to all listeners
        impl_sendFrameActionEvent( FrameAction_COMPONENT_DETACHING );
    }

    if( bNewController == sal_True )
    {
        impl_setController( Reference< XController >() );
    }
    if( bNewWindow == sal_True )
    {
        impl_setComponentWindow( xComponentWindow );
    }
    if( bNewController == sal_True )
    {
        impl_setController( xController );
    }

    // Send FrameActionEvent to all listeners
    if  (
            ( xController.is()        ==  sal_True    )   ||
            ( xComponentWindow.is()   ==  sal_True    )
        )
    {
        if ( m_bConnected == sal_True )
        {
            impl_sendFrameActionEvent( FrameAction_COMPONENT_REATTACHED );
        }
        else
        {
            impl_sendFrameActionEvent( FrameAction_COMPONENT_ATTACHED   );
        }
    }

    m_bConnected = sal_True;

    // A new component doesn't know anything about current active/focus states
    if  (
            ( m_eActiveState            ==  FOCUS       )   &&
            ( m_xComponentWindow.is()   ==  sal_True    )
        )
    {
        m_xComponentWindow->setFocus();
    }

    return sal_True;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
Reference< awt::XWindow > SAL_CALL Frame::getComponentWindow() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::getComponentWindow()" )

    return m_xComponentWindow;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
Reference< XController > SAL_CALL Frame::getController() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::getController()" )

    // Return current controller.
    return m_xController;
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::contextChanged() throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::contextChanged()" )
    // Send event to all istener for frame actions.
    impl_sendFrameActionEvent( FrameAction_CONTEXT_CHANGED );
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::addFrameActionListener( const Reference< XFrameActionListener >& xListener ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::addFrameActionListener()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_addFrameActionListener( xListener ), "Frame::addFrameActionListener()\nInvalid parameter detected.\n" )

    // Add listener to container
    m_aListenerContainer.addInterface( ::getCppuType( ( const Reference< XFrameActionListener >* ) NULL ), xListener );
}

//*****************************************************************************************************************
//   XFrame
//*****************************************************************************************************************
void SAL_CALL Frame::removeFrameActionListener( const Reference< XFrameActionListener >& xListener ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::removeFrameActionListener()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_removeFrameActionListener( xListener ), "Frame::removeFrameActionListener()\nInvalid parameter detected.\n" )

    // Rmove listener from container
    m_aListenerContainer.removeInterface( ::getCppuType( ( const Reference< XFrameActionListener >* ) NULL ), xListener );
}

//*****************************************************************************************************************
//   XComponent
//*****************************************************************************************************************
void SAL_CALL Frame::dispose() throw( RuntimeException )
{
    Reference < XFrame > xThis( this );

    // mutex should be locked as short as possible, because otherwise deadlocks in multithreaded environments
    // are guaranteed, because (de)registering listeners or disposing a VCL componente always tries to get
    // the solar mutex; if this mutex is hold in another thread, this thread would be blocked if it tried to
    // access this frame ( f.e. when a focus lost event should be processed )
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::dispose()" )

    // Protection against recursive disposing!
    if ( m_bAlreadyDisposed == sal_False )
    {
        // Set flag against recursive or following calls.
        m_bAlreadyDisposed = sal_True ;

        UNLOCK_MUTEX( aGuard, "Frame::dispose()" )

        // Send message to all DISPOSE-listener.
        impl_sendDisposeEvent();

        // Delete current component and controller.
        setComponent( Reference< awt::XWindow >(), Reference< XController >() );

        EventObject aEvent;
        aEvent.Source = xThis;
        m_aListenerContainer.disposeAndClear( aEvent );

        LOCK_MUTEX( anotherGuard, m_aMutex, "Frame::dispose()" )

        // Force parent container to forget this frame.
        // ( It's contained in m_xParent and so no XEventListener for m_xParent! )
        if ( m_xParent.is() == sal_True )
        {
            m_xParent->getFrames()->remove( xThis );
            m_xParent = Reference< XFramesSupplier >();
        }

        // Release current indicator factory helper.
        m_xIndicatorFactoryHelper = Reference< XStatusIndicatorFactory >();

        // If we have our own window ... release it!
        if ( m_xContainerWindow.is() == sal_True )
        {
            impl_setContainerWindow( Reference< awt::XWindow >() );
        }

        // Forget global servicemanager
        m_xFactory = Reference< XMultiServiceFactory >();

        // Free memory for container and other helper.
        m_aChildFrameContainer.clear();
        m_xFramesHelper     = Reference< XFrames >();
        m_xDispatchHelper   = Reference< XDispatchProvider >();

        // Reset flags and other members ...
        m_eActiveState                  = DEFAULT_EACTIVESTATE              ;
        m_bRecursiveSearchProtection    = DEFAULT_BRECURSIVESEARCHPROTECTION;
        m_bIsFrameTop                   = DEFAULT_BISFRAMETOP               ;
        m_bConnected                    = DEFAULT_BCONNECTED                ;
        m_sName                         = DEFAULT_SNAME                     ;
    }
}

//*****************************************************************************************************************
//   XComponent
//*****************************************************************************************************************
void SAL_CALL Frame::addEventListener( const Reference< XEventListener >& xListener ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::addEventListener()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_addEventListener( xListener ), "Frame::addEventListener()\nInvalid parameter detected.\n" )

    // Add listener to container.
    m_aListenerContainer.addInterface( ::getCppuType( ( const Reference< XEventListener >* ) NULL ), xListener );
}

//*****************************************************************************************************************
//   XComponent
//*****************************************************************************************************************
void SAL_CALL Frame::removeEventListener( const Reference< XEventListener >& xListener ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::removeEventListener()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_removeEventListener( xListener ), "Frame::removeEventListener()\nInvalid parameter detected.\n" )

    // Add listener to container.
    m_aListenerContainer.removeInterface( ::getCppuType( ( const Reference< XEventListener >* ) NULL ), xListener );
}

//*****************************************************************************************************************
//   XEventListener
//*****************************************************************************************************************
void SAL_CALL Frame::disposing( const EventObject& aEvent ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::disposing()" )

    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_disposing( aEvent ), "Frame::disposing()\nInvalid parameter detected.\n" )

    // This instance is forced to release references to the specified interfaces by event-source.
    if ( aEvent.Source == m_xContainerWindow )
    {
        impl_setContainerWindow( Reference< awt::XWindow >() );
    }
}

//*****************************************************************************************************************
//   XFocusListener
//*****************************************************************************************************************
void SAL_CALL Frame::focusGained( const awt::FocusEvent& aEvent ) throw( RuntimeException )
{
    // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::focusGained()" )
    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_focusGained( aEvent ), "Frame::focusGained()\nInvalid parameter detected.\n" )
/*
    // We must safe this new state, send event to listener ...
    m_eActiveState = FOCUS;
    impl_sendFrameActionEvent( FrameAction_FRAME_UI_ACTIVATED );
    // ... and forward our new focus to our component window!
    if( m_xComponentWindow.is() == sal_True )
    {
        m_xComponentWindow->setFocus();
    }
*/
    if( m_xComponentWindow.is() == sal_True )
    {
        m_xComponentWindow->setFocus();
    }
}

//*****************************************************************************************************************
//   XFocusListener
//*****************************************************************************************************************
void SAL_CALL Frame::focusLost( const awt::FocusEvent& aEvent ) throw( RuntimeException )
{
/*  // Ready for multithreading
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::focusLost()" )
    // Safe impossible cases
    LOG_ASSERT( impldbg_checkParameter_focusLost( aEvent ), "Frame::focusLost()\nInvalid parameter detected.\n" )

    // We must send UI_DEACTIVATING to our listener and forget our current FOCUS state!
    m_eActiveState = ACTIVE;
    impl_sendFrameActionEvent( FrameAction_FRAME_UI_DEACTIVATING );
*/
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
void Frame::impl_setContainerWindow( const Reference< awt::XWindow >& xWindow )
{
    // mutex should be locked as short as possible, because otherwise deadlocks in multithreaded environments
    // are guaranteed, because (de)registering listeners or disposing a VCL componente always tries to get
    // the solar mutex; if this mutex is hold in another thread, this thread would be blocked if it tried to
    // access this frame ( f.e. when a focus lost event should be processed )
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::impl_setContainerWindow()" )

    // Remember old window; dispose later to avoid flickering.
    Reference< awt::XWindow > xOld = m_xContainerWindow;

    // Save new window reference
    m_xContainerWindow = xWindow;

    UNLOCK_MUTEX( aGuard, "Frame::impl_setContainerWindow()" )

    // Remove this instance from old WindowListener container.
    if ( xOld.is() == sal_True )
    {
        xOld->removeWindowListener( this );
        xOld->removeFocusListener( this );
    }

    // Register this instance as new listener.
    if ( xWindow.is() == sal_True )
    {
        xWindow->addWindowListener( this );
        xWindow->addFocusListener( this );

        // if possible register as TopWindowListener
        Reference< awt::XTopWindow > xTopWindow( xWindow, UNO_QUERY );
        if ( xTopWindow.is() == sal_True )
            xTopWindow->addTopWindowListener( this );
    }

    // Dispose old window now
    if ( xOld.is() == sal_True )
    {
        // All VclComponents are XComponents; so call dispose before discarding
        // a Reference< XVclComponent >, because this frame is the owner of the window
        xOld->setVisible( sal_False );
        xOld->dispose();
    }
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
void Frame::impl_setComponentWindow( const Reference< awt::XWindow >& xWindow )
{
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::impl_setComponentWindow()" )

    // Work only, if window will changing.
    if ( xWindow != m_xComponentWindow )
    {
        // Remember old component; dispose later to avoid flickering.
        Reference< awt::XWindow > xOld = m_xComponentWindow;
        // Take the new one.
        m_xComponentWindow = xWindow;

        UNLOCK_MUTEX( aGuard, "Frame::impl_setComponentWindow()" )

        // Set correct size before showing the window.
        impl_resizeComponentWindow();

        // Destroy old window.
        if ( xOld.is() == sal_True )
        {
            // All VclComponents are XComponents; so call dispose before discarding
            // a Reference< XVclComponent >, because this frame is the owner of the Component.
            xOld->dispose();
        }
    }
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
void Frame::impl_setController( const Reference< XController >& xController )
{
    LOCK_MUTEX( aGuard, m_aMutex, "Frame::impl_setController()" )

    // Safe old value for disposing AFTER set of new controller!
    Reference< XController > xOld = m_xController;
    // Take the new one.
    m_xController = xController;

    UNLOCK_MUTEX( aGuard, "Frame::impl_setController()" )

    // Dispose old instance.
    if ( xOld.is() == sal_True )
    {
        xOld->dispose();
    }
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
void Frame::impl_sendFrameActionEvent( const FrameAction& aAction )
{
    // Log informations about order of events to file!
    // (only activated in debug version!)
    LOG_FRAMEACTIONEVENT( "Frame", m_sName, aAction )

    // Send FrameAction event to all listener.
    // Get container for right listener.
    OInterfaceContainerHelper* pContainer = m_aListenerContainer.getContainer( ::getCppuType( ( const Reference< XFrameActionListener >*) NULL ) );

    if ( pContainer != NULL )
    {
        // Build action event.
        FrameActionEvent aFrameActionEvent( (OWeakObject*)this, this, aAction );

        // Get iterator for access to listener.
        OInterfaceIteratorHelper aIterator( *pContainer );
        // Send message to all listener.
        while ( aIterator.hasMoreElements() == sal_True )
        {
            ((XFrameActionListener *)aIterator.next())->frameAction( aFrameActionEvent );
        }
    }
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
void Frame::impl_sendDisposeEvent()
{
    // Log informations about order of events to file!
    // (only activated in debug version!)
    LOG_DISPOSEEVENT( "Frame", m_sName )

    // Send event to all listener.
    // Get container for right listener.
    OInterfaceContainerHelper* pContainer = m_aListenerContainer.getContainer( ::getCppuType( ( const Reference< XEventListener >*) NULL ) );

    if ( pContainer != NULL )
    {
        // Build event.
        EventObject aEvent( (OWeakObject*)this );

        // Get iterator for access to listener.
        OInterfaceIteratorHelper aIterator( *pContainer );
        // Send message to all listener.
        while ( aIterator.hasMoreElements() == sal_True )
        {
            ((XEventListener*)aIterator.next())->disposing( aEvent );
        }
    }
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
sal_Bool Frame::impl_willFrameTop( const REFERENCE< XFRAMESSUPPLIER >& xParent )
{
    // Set default return value.
    sal_Bool bWillFrameTop = sal_False;

    // This frame is a topframe, if ouer parent is a task, the desktop or no parent exist!
    // Cast parent to right interfaces ...
    Reference< XTask >      xIsTask     ( xParent, UNO_QUERY );
    Reference< XDesktop >   xIsDesktop  ( xParent, UNO_QUERY );
    // ... and control it.
    if  (
            ( xIsTask.is()      ==  sal_True    )   ||
            ( xIsDesktop.is()   ==  sal_True    )   ||
            ( m_xParent.is()    ==  sal_False   )
        )
    {
        bWillFrameTop = sal_True;
    }

    // Return result of this operation.
    return bWillFrameTop;
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
void Frame::impl_resizeComponentWindow()
{
    // Work only if container window is set!
    if  (
            ( m_xContainerWindow.is() == sal_True ) &&
            ( m_xComponentWindow.is() == sal_True )
        )
    {
        // Get reference to his device.
        Reference< awt::XDevice > xDevice( m_xContainerWindow, UNO_QUERY );
        // Convert relativ size to output size.
        awt::Rectangle  aRectangle  = m_xContainerWindow->getPosSize();
        awt::DeviceInfo aInfo       = xDevice->getInfo();
        awt::Size       aSize       (   aRectangle.Width    - aInfo.LeftInset   - aInfo.RightInset  ,
                                    aRectangle.Height   - aInfo.TopInset    - aInfo.BottomInset );
         // Resize ouer component window.
        m_xComponentWindow->setPosSize( 0, 0, aSize.Width, aSize.Height, awt::PosSize::SIZE );
    }
}

//_________________________________________________________________________________________________________________
//  debug methods
//_________________________________________________________________________________________________________________

/*-----------------------------------------------------------------------------------------------------------------
    The follow methods checks the parameter for other functions. If a parameter or his value is non valid,
    we return "sal_False". (else sal_True) This mechanism is used to throw an ASSERT!

    ATTENTION

        If you miss a test for one of this parameters, contact the autor or add it himself !(?)
        But ... look for right testing! See using of this methods!
-----------------------------------------------------------------------------------------------------------------*/

#ifdef ENABLE_ASSERTIONS

//*****************************************************************************************************************
// append() accept valid references and pure frames only! No tasks or desktops.
sal_Bool Frame::impldbg_checkParameter_append( const Reference< XFrame >& xFrame )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xFrame                                           ==  NULL        )   ||
            ( xFrame.is()                                       ==  sal_False   )   ||
            ( (Reference< XTask >( xFrame, UNO_QUERY )).is()    ==  sal_True    )   ||
            ( (Reference< XDesktop >( xFrame, UNO_QUERY )).is() ==  sal_True    )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
// queryFrames() accept valid searchflags only. If a new one will exist, we know it, if this check failed!
sal_Bool Frame::impldbg_checkParameter_queryFrames( sal_Int32 nSearchFlags )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            (    nSearchFlags != FrameSearchFlag::AUTO        ) &&
            ( !( nSearchFlags &  FrameSearchFlag::PARENT    ) ) &&
            ( !( nSearchFlags &  FrameSearchFlag::SELF      ) ) &&
            ( !( nSearchFlags &  FrameSearchFlag::CHILDREN  ) ) &&
            ( !( nSearchFlags &  FrameSearchFlag::CREATE    ) ) &&
            ( !( nSearchFlags &  FrameSearchFlag::SIBLINGS  ) ) &&
            ( !( nSearchFlags &  FrameSearchFlag::TASKS     ) ) &&
            ( !( nSearchFlags &  FrameSearchFlag::ALL       ) ) &&
            ( !( nSearchFlags &  FrameSearchFlag::GLOBAL    ) )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
// remove() accept valid references and pure frames only! No tasks or desktops.
sal_Bool Frame::impldbg_checkParameter_remove( const Reference< XFrame >& xFrame )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xFrame                                           ==  NULL        )   ||
            ( xFrame.is()                                       ==  sal_False   )   ||
            ( (Reference< XTask >( xFrame, UNO_QUERY )).is()    ==  sal_True    )   ||
            ( (Reference< XDesktop >( xFrame, UNO_QUERY )).is() ==  sal_True    )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
// Its allowed to reset the active frame membervariable with a NULL-Reference but not with a NULL-pointer!
// And we accept frames only! No tasks and desktops!
sal_Bool Frame::impldbg_checkParameter_setActiveFrame( const Reference< XFrame >& xFrame )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if ( &xFrame == NULL )
    {
        bOK = sal_False ;
    }
    else
    if  (
            ( (Reference< XTask >( xFrame, UNO_QUERY )).is()    ==  sal_True    )   ||
            ( (Reference< XDesktop >( xFrame, UNO_QUERY )).is() ==  sal_True    )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_updateViewData( const Any& aValue )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &aValue           ==  NULL        )   ||
            ( aValue.hasValue() ==  sal_False   )
            //ASMUSS Wenn der Typ noch bekannt ist, dann auch den abfragen!
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_createNewEntry(  const   OUString&                   sURL        ,
                                                          const Sequence< PropertyValue >&  seqArguments,
                                                        const   OUString&                   sTitle      )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &sURL                     ==  NULL    )   ||
            ( sURL.getLength()          <   1       )   ||
            ( &seqArguments             ==  NULL    )   ||
            ( seqArguments.getLength()  <   1       )   ||
            ( &sTitle                   ==  NULL    )   ||
            ( sTitle.getLength()        <   1       )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_windowResized( const awt::WindowEvent& aEvent )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &aEvent == NULL )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_windowActivated( const EventObject& aEvent )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &aEvent == NULL )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_windowDeactivated( const EventObject& aEvent )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &aEvent == NULL )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_initialize( const Reference< awt::XWindow >& xWindow )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xWindow      ==  NULL        )   ||
            ( xWindow.is()  ==  sal_False   )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_setCreator( const Reference< XFramesSupplier >& xCreator )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xCreator     ==  NULL        )   ||
            ( xCreator.is() ==  sal_False   )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
// An empty name is not fine but allowed ... !
sal_Bool Frame::impldbg_checkParameter_setName( const OUString& sName )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &sName == NULL )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_findFrame(   const   OUString&   sTargetFrameName    ,
                                                             sal_Int32  nSearchFlags        )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &sTargetFrameName     ==  NULL    )   ||
            // sTargetFrameName can be ""!
            (
                (    nSearchFlags != FrameSearchFlag::AUTO        ) &&
                ( !( nSearchFlags &  FrameSearchFlag::PARENT    ) ) &&
                ( !( nSearchFlags &  FrameSearchFlag::SELF      ) ) &&
                ( !( nSearchFlags &  FrameSearchFlag::CHILDREN  ) ) &&
                ( !( nSearchFlags &  FrameSearchFlag::CREATE    ) ) &&
                ( !( nSearchFlags &  FrameSearchFlag::SIBLINGS  ) ) &&
                ( !( nSearchFlags &  FrameSearchFlag::TASKS     ) ) &&
                ( !( nSearchFlags &  FrameSearchFlag::ALL       ) ) &&
                ( !( nSearchFlags &  FrameSearchFlag::GLOBAL    ) )
            )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_setComponent(    const   Reference< awt::XWindow >&      xComponentWindow    ,
                                                              const Reference< XController >&   xController         )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xComponentWindow     ==  NULL        )   ||
            ( &xController          ==  NULL        )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_addFrameActionListener( const Reference< XFrameActionListener >& xListener )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xListener            ==  NULL        )   ||
            ( xListener.is()        ==  sal_False   )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_removeFrameActionListener( const Reference< XFrameActionListener >& xListener )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xListener            ==  NULL        )   ||
            ( xListener.is()        ==  sal_False   )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_addEventListener( const Reference< XEventListener >& xListener )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xListener            ==  NULL        )   ||
            ( xListener.is()        ==  sal_False   )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_removeEventListener( const Reference< XEventListener >& xListener )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &xListener            ==  NULL        )   ||
            ( xListener.is()        ==  sal_False   )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_disposing( const EventObject& aEvent )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &aEvent               ==  NULL        )   ||
            ( aEvent.Source.is()    ==  sal_False   )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_focusGained( const awt::FocusEvent& aEvent )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &aEvent   ==  NULL    )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

//*****************************************************************************************************************
sal_Bool Frame::impldbg_checkParameter_focusLost( const awt::FocusEvent& aEvent )
{
    // Set default return value.
    sal_Bool bOK = sal_True;

    // Check parameter.
    if  (
            ( &aEvent   ==  NULL    )
        )
    {
        bOK = sal_False ;
    }

    // Return result of check.
    return bOK ;
}

#endif  // #ifdef ENABLE_ASSERTIONS

/*-----------------------------------------------------------------------------------------------------------------
    Follow method is used to print out the content of current container.
    Use this to get information about the tree.
-----------------------------------------------------------------------------------------------------------------*/

#ifdef ENABLE_SERVICEDEBUG  // Is defined in debug version only.

//*****************************************************************************************************************
OUString Frame::impldbg_getTreeNames( sal_Int16 nLevel )
{
    // Create an "empty stream" with enough place for ouer own container informations.
    OUStringBuffer sOutPut(1024);

    // Add my own information to stream.
    // Format of output : "<Level*TAB>[<level>:<name>:<extra informations>]\n"
    // Add "<Level*TAB>"
    for ( sal_Int8 nTabCount=1; nTabCount<=nLevel; ++nTabCount )
    {
        sOutPut.appendAscii( "\t" );
    }
    // Add "[<level>:<name>:"
    sOutPut.append( (sal_Unicode)'['    );
    sOutPut.append( (sal_Int32)nLevel   );
    sOutPut.append( (sal_Unicode)':'    );
    sOutPut.append( (sal_Unicode)'"'    );
    sOutPut.append( m_sName );
    sOutPut.append( (sal_Unicode)'"'    );
    sOutPut.append( (sal_Unicode)':'    );
    // Add "<extra informations>"
    switch( m_eActiveState )
    {
        case ACTIVE :   sOutPut.appendAscii( "ACTIVE");
                        break;
        case FOCUS  :   sOutPut.appendAscii( "FOCUS" );
                        break;
    }
    Reference< XFrame > xActiveChild        = m_aChildFrameContainer.getActive();
    Reference< XFrame > xTHISFrame          ( (OWeakObject*)this, UNO_QUERY );

    Reference< XFrame > xActiveParentChild;
    if ( m_xParent.is() == sal_True )
    {
        xActiveParentChild = m_xParent->getActiveFrame();
    }

    // If "active path" from my parent to one of my childs not broken => I'am in the middle of an active path.
    if ( xActiveChild.is() == sal_True && xActiveParentChild == xTHISFrame )
    {
        sOutPut.appendAscii( ":MIDDLEPATH" );
    }
    // If "active path" exist to one of my childs only => I'am on the top of an active path.
    if ( xActiveChild.is() == sal_True && xActiveParentChild != xTHISFrame )
    {
        sOutPut.appendAscii( ":STARTPATH" );
    }
    // If "active path" exist from my parent to me, but not to one of my childs => I'am at the end of an active path.
    if ( xActiveChild.is() == sal_False && xActiveParentChild == xTHISFrame )
    {
        sOutPut.appendAscii( ":ENDPATH" );
    }
    // Else; There is no active path in the near of this node.

    // Add "]\n"
    sOutPut.append( (sal_Unicode)']' );
    sOutPut.appendAscii( "\n" );

    // Step over all elements in current container and collect names.
    // We must lock the container, to have exclusiv access to elements!
    m_aChildFrameContainer.lock();
    sal_uInt32 nCount = m_aChildFrameContainer.getCount();
    for ( sal_uInt32 nPosition=0; nPosition<nCount; ++nPosition )
    {
        // Step during tree deep first - from the left site to the right one.
        // Print subtree of this child to stream!
        Reference< XFrame > xItem = m_aChildFrameContainer[nPosition];
        Reference< XSPECIALDEBUGINTERFACE > xDebug( xItem, UNO_QUERY );
        sOutPut.append( xDebug->dumpVariable( DUMPVARIABLE_TREEINFO, nLevel+1 ) );
    }
    // Don't forget to unlock the container!
    m_aChildFrameContainer.unlock();

    // Now we have anough informations about tree.
    // Return it to caller.
    return sOutPut.makeStringAndClear();
}

#endif  // #ifdef ENABLE_SERVICEDEBUG

}   // namespace framework
