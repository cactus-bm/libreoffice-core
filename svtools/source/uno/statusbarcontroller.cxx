/*************************************************************************
 *
 *  $RCSfile: statusbarcontroller.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-31 08:30:48 $
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

#ifndef _SVTOOLS_STATUSBARCONTROLLER_HXX
#include <statusbarcontroller.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_FRAME_XLAYOUTMANAGER_HPP_
#include <drafts/com/sun/star/frame/XLayoutManager.hpp>
#endif

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif
#ifndef _SV_STATUS_HXX
#include <vcl/status.hxx>
#endif
#ifndef _SVTOOLS_IMGDEF_HXX
#include <imgdef.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MISCOPT_HXX
#include <miscopt.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::drafts::com::sun::star::frame;

namespace svt
{

StatusbarController::StatusbarController(
    const Reference< XMultiServiceFactory >& rServiceManager,
    const Reference< XFrame >& xFrame,
    const OUString& aCommandURL,
    unsigned short nID ) :
    OWeakObject()
    ,   m_aListenerContainer( m_aMutex )
    ,   m_xServiceManager( rServiceManager )
    ,   m_xFrame( xFrame )
    ,   m_aCommandURL( aCommandURL )
    ,   m_bInitialized( sal_False )
    ,   m_bDisposed( sal_False )
    ,   m_nID( nID )
{
}

StatusbarController::StatusbarController() :
    OWeakObject()
    ,   m_aListenerContainer( m_aMutex )
    ,   m_bInitialized( sal_False )
    ,   m_bDisposed( sal_False )
    ,   m_nID( 0 )
{
}

StatusbarController::~StatusbarController()
{
}

Reference< XFrame > StatusbarController::getFrameInterface() const
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
    return m_xFrame;
}

Reference< XMultiServiceFactory > StatusbarController::getServiceManager() const
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
    return m_xServiceManager;
}

Reference< XLayoutManager > StatusbarController::getLayoutManager() const
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
    Reference< XLayoutManager > xLayoutManager;
    Reference< XPropertySet > xPropSet( m_xFrame, UNO_QUERY );
    if ( xPropSet.is() )
    {
        try
        {
            Any a;
            a = xPropSet->getPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" )));
            a >>= xLayoutManager;
        }
        catch ( Exception& )
        {
        }
    }

    return xLayoutManager;
}

Reference< XURLTransformer > StatusbarController::getURLTransformer() const
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
    if ( !m_xURLTransformer.is() && m_xServiceManager.is() )
    {
        m_xURLTransformer = Reference< XURLTransformer >(
                                m_xServiceManager->createInstance(
                                    rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.util.URLTransformer" ))),
                                UNO_QUERY );
    }

    return m_xURLTransformer;
}

// XInterface
Any SAL_CALL StatusbarController::queryInterface( const Type& rType )
throw ( RuntimeException )
{
    Any a = ::cppu::queryInterface(
                rType ,
                static_cast< XStatusbarController* >( this ),
                static_cast< XStatusListener* >( this ),
                static_cast< XEventListener* >( this ),
                static_cast< XInitialization* >( this ),
                static_cast< XComponent* >( this ),
                static_cast< XUpdatable* >( this ));

    if ( a.hasValue() )
        return a;

    return OWeakObject::queryInterface( rType );
}

void SAL_CALL StatusbarController::acquire() throw ()
{
    OWeakObject::acquire();
}

void SAL_CALL StatusbarController::release() throw ()
{
    OWeakObject::release();
}

void SAL_CALL StatusbarController::initialize( const Sequence< Any >& aArguments )
throw ( Exception, RuntimeException )
{
    const rtl::OUString aFrameName( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
    const rtl::OUString aCommandURLName( RTL_CONSTASCII_USTRINGPARAM( "CommandURL" ));
    const rtl::OUString aServiceManagerName( RTL_CONSTASCII_USTRINGPARAM( "ServiceManager" ));
    const rtl::OUString aParentWindow( RTL_CONSTASCII_USTRINGPARAM( "ParentWindow" ));
    const rtl::OUString aIdentifier( RTL_CONSTASCII_USTRINGPARAM( "Identifier" ));

    bool bInitialized( true );
    bool bBindListener( false );

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

        if ( m_bDisposed )
            throw DisposedException();

        bInitialized = m_bInitialized;
    }

    if ( !bInitialized )
    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
        m_bInitialized = sal_True;

        PropertyValue aPropValue;
        for ( int i = 0; i < aArguments.getLength(); i++ )
        {
            if ( aArguments[i] >>= aPropValue )
            {
                if ( aPropValue.Name.equalsAscii( "Frame" ))
                    aPropValue.Value >>= m_xFrame;
                else if ( aPropValue.Name.equalsAscii( "CommandURL" ))
                    aPropValue.Value >>= m_aCommandURL;
                else if ( aPropValue.Name.equalsAscii( "ServiceManager" ))
                    aPropValue.Value >>= m_xServiceManager;
                else if ( aPropValue.Name.equalsAscii( "ParentWindow" ))
                    aPropValue.Value >>= m_xParentWindow;
                else if ( aPropValue.Name.equalsAscii( "Identifier" ))
                    aPropValue.Value >>= m_nID;
            }
        }

        if ( m_aCommandURL.getLength() )
            m_aListenerMap.insert( URLToDispatchMap::value_type( m_aCommandURL, Reference< XDispatch >() ));
    }
}

void SAL_CALL StatusbarController::update()
throw ( RuntimeException )
{
    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
        if ( m_bDisposed )
            throw DisposedException();
    }

    // Bind all registered listeners to their dispatch objects
    bindListener();
}

// XComponent
void SAL_CALL StatusbarController::dispose()
throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XComponent > xThis( static_cast< OWeakObject* >(this), UNO_QUERY );

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
        if ( m_bDisposed )
            throw DisposedException();
    }

    com::sun::star::lang::EventObject aEvent( xThis );
    m_aListenerContainer.disposeAndClear( aEvent );

    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
    Reference< XStatusListener > xStatusListener( static_cast< OWeakObject* >( this ), UNO_QUERY );
    Reference< XURLTransformer > xURLTransformer = getURLTransformer();
    URLToDispatchMap::iterator pIter = m_aListenerMap.begin();
    com::sun::star::util::URL aTargetURL;
    while ( pIter != m_aListenerMap.end() )
    {
        try
        {
            Reference< XDispatch > xDispatch( pIter->second );
            aTargetURL.Complete = pIter->first;
            xURLTransformer->parseStrict( aTargetURL );

            if ( xDispatch.is() && xStatusListener.is() )
                xDispatch->removeStatusListener( xStatusListener, aTargetURL );
        }
        catch ( Exception& )
        {
        }

        ++pIter;
    }

    // clear hash map
    m_aListenerMap.clear();

    // release references
    m_xURLTransformer.clear();
    m_xServiceManager.clear();
    m_xFrame.clear();
    m_xParentWindow.clear();

    m_bDisposed = sal_True;
}

void SAL_CALL StatusbarController::addEventListener( const Reference< XEventListener >& xListener )
throw ( RuntimeException )
{
    m_aListenerContainer.addInterface( ::getCppuType( ( const Reference< XEventListener >* ) NULL ), xListener );
}

void SAL_CALL StatusbarController::removeEventListener( const Reference< XEventListener >& aListener )
throw ( RuntimeException )
{
    m_aListenerContainer.removeInterface( ::getCppuType( ( const Reference< XEventListener >* ) NULL ), aListener );
}

// XEventListener
void SAL_CALL StatusbarController::disposing( const EventObject& Source )
throw ( RuntimeException )
{
    Reference< XInterface > xSource( Source.Source );

    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    if ( m_bDisposed )
        return;

    URLToDispatchMap::iterator pIter = m_aListenerMap.begin();
    while ( pIter != m_aListenerMap.end() )
    {
        // Compare references and release dispatch references if they are equal.
        Reference< XInterface > xIfac( pIter->second, UNO_QUERY );
        if ( xSource == xIfac )
            pIter->second.clear();
        pIter++;
    }

    Reference< XInterface > xIfac( m_xFrame, UNO_QUERY );
    if ( xIfac == xSource )
        m_xFrame.clear();
}

// XStatusListener
void SAL_CALL StatusbarController::statusChanged( const FeatureStateEvent& Event )
throw ( RuntimeException )
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    if ( m_bDisposed )
        return;

    Window* pWindow = VCLUnoHelper::GetWindow( m_xParentWindow );
    if ( pWindow && pWindow->GetType() == WINDOW_STATUSBAR && m_nID != 0 )
    {
        rtl::OUString aStrValue;
        StatusBar*    pStatusBar = (StatusBar *)pWindow;

        if ( Event.State >>= aStrValue )
            pStatusBar->SetItemText( m_nID, aStrValue );
        else if ( !Event.State.hasValue() )
            pStatusBar->SetItemText( m_nID, String() );
    }
}

// XStatusbarController
::sal_Bool SAL_CALL StatusbarController::mouseButtonDown(
    const ::com::sun::star::awt::MouseEvent& aMouseEvent )
throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}

::sal_Bool SAL_CALL StatusbarController::mouseMove(
    const ::com::sun::star::awt::MouseEvent& aMouseEvent )
throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}

::sal_Bool SAL_CALL StatusbarController::mouseButtonUp(
    const ::com::sun::star::awt::MouseEvent& aMouseEvent )
throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}

void SAL_CALL StatusbarController::command(
    const ::com::sun::star::awt::Point& aPos,
    ::sal_Int32 nCommand,
    ::sal_Bool bMouseEvent,
    const ::com::sun::star::uno::Any& aData )
throw (::com::sun::star::uno::RuntimeException)
{
}

void SAL_CALL StatusbarController::paint(
    const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics >& xGraphics,
    const ::com::sun::star::awt::Rectangle& rOutputRectangle,
    ::sal_Int32 nItemId,
    ::sal_Int32 nStyle )
throw (::com::sun::star::uno::RuntimeException)
{
}

void SAL_CALL StatusbarController::click()
throw (::com::sun::star::uno::RuntimeException)
{
}

void SAL_CALL StatusbarController::doubleClick() throw (::com::sun::star::uno::RuntimeException)
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    if ( m_bDisposed )
        return;

    Sequence< PropertyValue > aArgs;
    execute( aArgs );
}

void StatusbarController::addStatusListener( const rtl::OUString& aCommandURL )
{
    Reference< XDispatch >       xDispatch;
    Reference< XStatusListener > xStatusListener;
    com::sun::star::util::URL    aTargetURL;

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
        URLToDispatchMap::iterator pIter = m_aListenerMap.find( aCommandURL );

        // Already in the list of status listener. Do nothing.
        if ( pIter != m_aListenerMap.end() )
            return;

        // Check if we are already initialized. Implementation starts adding itself as status listener when
        // intialize is called.
        if ( !m_bInitialized )
        {
            // Put into the hash_map of status listener. Will be activated when initialized is called
            m_aListenerMap.insert( URLToDispatchMap::value_type( aCommandURL, Reference< XDispatch >() ));
            return;
        }
        else
        {
            // Add status listener directly as intialize has already been called.
            Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );
            if ( m_xServiceManager.is() && xDispatchProvider.is() )
            {
                Reference< XURLTransformer > xURLTransformer = getURLTransformer();
                aTargetURL.Complete = aCommandURL;
                xURLTransformer->parseStrict( aTargetURL );
                xDispatch = xDispatchProvider->queryDispatch( aTargetURL, ::rtl::OUString(), 0 );

                xStatusListener = Reference< XStatusListener >( static_cast< OWeakObject* >( this ), UNO_QUERY );
                URLToDispatchMap::iterator pIter = m_aListenerMap.find( aCommandURL );
                if ( pIter != m_aListenerMap.end() )
                {
                    Reference< XDispatch > xOldDispatch( pIter->second );
                    pIter->second = xDispatch;

                    try
                    {
                        if ( xOldDispatch.is() )
                            xOldDispatch->removeStatusListener( xStatusListener, aTargetURL );
                    }
                    catch ( Exception& )
                    {
                    }
                }
                else
                    m_aListenerMap.insert( URLToDispatchMap::value_type( aCommandURL, xDispatch ));
            }
        }
    }

    // Call without locked mutex as we are called back from dispatch implementation
    try
    {
        if ( xDispatch.is() )
            xDispatch->addStatusListener( xStatusListener, aTargetURL );
    }
    catch ( Exception& )
    {
    }
}

void StatusbarController::removeStatusListener( const rtl::OUString& aCommandURL )
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    URLToDispatchMap::iterator pIter = m_aListenerMap.find( aCommandURL );
    if ( pIter != m_aListenerMap.end() )
    {
        Reference< XDispatch > xDispatch( pIter->second );
        Reference< XStatusListener > xStatusListener( static_cast< OWeakObject* >( this ), UNO_QUERY );
        m_aListenerMap.erase( pIter );

        try
        {
            Reference< XURLTransformer > xURLTransformer = getURLTransformer();
            com::sun::star::util::URL aTargetURL;
            aTargetURL.Complete = aCommandURL;
            xURLTransformer->parseStrict( aTargetURL );

            if ( xDispatch.is() && xStatusListener.is() )
                xDispatch->removeStatusListener( xStatusListener, aTargetURL );
        }
        catch ( Exception& )
        {
        }
    }
}

void StatusbarController::bindListener()
{
    std::vector< Listener > aDispatchVector;
    Reference< XStatusListener > xStatusListener;

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

        if ( !m_bInitialized )
            return;

        // Collect all registered command URL's and store them temporary
        Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );
        if ( m_xServiceManager.is() && xDispatchProvider.is() )
        {
            xStatusListener = Reference< XStatusListener >( static_cast< OWeakObject* >( this ), UNO_QUERY );
            URLToDispatchMap::iterator pIter = m_aListenerMap.begin();
            while ( pIter != m_aListenerMap.end() )
            {
                Reference< XURLTransformer > xURLTransformer = getURLTransformer();
                com::sun::star::util::URL aTargetURL;
                aTargetURL.Complete = pIter->first;
                xURLTransformer->parseStrict( aTargetURL );

                Reference< XDispatch > xDispatch( pIter->second );
                if ( xDispatch.is() )
                {
                    // We already have a dispatch object => we have to requery.
                    // Release old dispatch object and remove it as listener
                    try
                    {
                        xDispatch->removeStatusListener( xStatusListener, aTargetURL );
                    }
                    catch ( Exception& )
                    {
                    }
                }

                pIter->second.clear();
                xDispatch.clear();

                // Query for dispatch object. Old dispatch will be released with this, too.
                try
                {
                    xDispatch = xDispatchProvider->queryDispatch( aTargetURL, ::rtl::OUString(), 0 );
                }
                catch ( Exception& )
                {
                }
                pIter->second = xDispatch;

                Listener aListener( aTargetURL, xDispatch );
                aDispatchVector.push_back( aListener );
                ++pIter;
            }
        }
    }

    // Call without locked mutex as we are called back from dispatch implementation
    if ( xStatusListener.is() )
    {
        try
        {
            for ( sal_uInt32 i = 0; i < aDispatchVector.size(); i++ )
            {
                Listener& rListener = aDispatchVector[i];
                if ( rListener.xDispatch.is() )
                    rListener.xDispatch->addStatusListener( xStatusListener, rListener.aURL );
                else if ( rListener.aURL.Complete == m_aCommandURL )
                {
                    try
                    {
                        // Send status changed for the main URL, if we cannot get a valid dispatch object.
                        // UI disables the button. Catch exception as we release our mutex, it is possible
                        // that someone else already disposed this instance!
                        FeatureStateEvent aFeatureStateEvent;
                        aFeatureStateEvent.IsEnabled = sal_False;
                        aFeatureStateEvent.FeatureURL = rListener.aURL;
                        aFeatureStateEvent.State = Any();
                        xStatusListener->statusChanged( aFeatureStateEvent );
                    }
                    catch ( Exception& )
                    {
                    }
                }
            }
        }
        catch ( Exception& )
        {
        }
    }
}

void StatusbarController::unbindListener()
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    if ( !m_bInitialized )
        return;

    // Collect all registered command URL's and store them temporary
    Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );
    if ( m_xServiceManager.is() && xDispatchProvider.is() )
    {
        Reference< XStatusListener > xStatusListener( static_cast< OWeakObject* >( this ), UNO_QUERY );
        URLToDispatchMap::iterator pIter = m_aListenerMap.begin();
        while ( pIter != m_aListenerMap.end() )
        {
            Reference< XURLTransformer > xURLTransformer = getURLTransformer();
            com::sun::star::util::URL aTargetURL;
            aTargetURL.Complete = pIter->first;
            xURLTransformer->parseStrict( aTargetURL );

            Reference< XDispatch > xDispatch( pIter->second );
            if ( xDispatch.is() )
            {
                // We already have a dispatch object => we have to requery.
                // Release old dispatch object and remove it as listener
                try
                {
                    xDispatch->removeStatusListener( xStatusListener, aTargetURL );
                }
                catch ( Exception& )
                {
                }
            }
            pIter->second.clear();
            ++pIter;
        }
    }
}

sal_Bool StatusbarController::isBound() const
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    if ( !m_bInitialized )
        return sal_False;

    URLToDispatchMap::const_iterator pIter = m_aListenerMap.find( m_aCommandURL );
    if ( pIter != m_aListenerMap.end() )
        return ( pIter->second.is() );

    return sal_False;
}

void StatusbarController::updateStatus()
{
    bindListener();
}

void StatusbarController::updateStatus( const rtl::OUString aCommandURL )
{
    Reference< XDispatch > xDispatch;
    Reference< XStatusListener > xStatusListener;
    com::sun::star::util::URL aTargetURL;

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

        if ( !m_bInitialized )
            return;

        // Try to find a dispatch object for the requested command URL
        Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );
        xStatusListener = Reference< XStatusListener >( static_cast< OWeakObject* >( this ), UNO_QUERY );
        if ( m_xServiceManager.is() && xDispatchProvider.is() )
        {
            Reference< XURLTransformer > xURLTransformer = getURLTransformer();
            aTargetURL.Complete = aCommandURL;
            xURLTransformer->parseStrict( aTargetURL );
            xDispatch = xDispatchProvider->queryDispatch( aTargetURL, rtl::OUString(), 0 );
        }
    }

    if ( xDispatch.is() && xStatusListener.is() )
    {
        // Catch exception as we release our mutex, it is possible that someone else
        // has already disposed this instance!
        // Add/remove status listener to get a update status information from the
        // requested command.
        try
        {
            xDispatch->addStatusListener( xStatusListener, aTargetURL );
            xDispatch->removeStatusListener( xStatusListener, aTargetURL );
        }
        catch ( Exception& )
        {
        }
    }
}

void StatusbarController::execute( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs )
{
    Reference< XDispatch >       xDispatch;
    Reference< XURLTransformer > xURLTransformer;
    rtl::OUString                aCommandURL;

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

        if ( m_bDisposed )
            throw DisposedException();

        if ( m_bInitialized &&
             m_xFrame.is() &&
             m_xServiceManager.is() &&
             m_aCommandURL.getLength() )
        {
            xURLTransformer = getURLTransformer();
            aCommandURL = m_aCommandURL;
            URLToDispatchMap::iterator pIter = m_aListenerMap.find( m_aCommandURL );
            if ( pIter != m_aListenerMap.end() )
                xDispatch = pIter->second;
        }
    }

    if ( xDispatch.is() && xURLTransformer.is() )
    {
        try
        {
            com::sun::star::util::URL aTargetURL;

            aTargetURL.Complete = aCommandURL;
            xURLTransformer->parseStrict( aTargetURL );
            xDispatch->dispatch( aTargetURL, aArgs );
        }
        catch ( DisposedException& )
        {
        }
    }
}

void StatusbarController::execute(
    const rtl::OUString& aCommandURL,
    const Sequence< ::com::sun::star::beans::PropertyValue >& aArgs )
{
    Reference< XDispatch >      xDispatch;
    com::sun::star::util::URL   aTargetURL;

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

        if ( m_bDisposed )
            throw DisposedException();

        if ( m_bInitialized &&
             m_xFrame.is() &&
             m_xServiceManager.is() &&
             m_aCommandURL.getLength() )
        {
            Reference< XURLTransformer > xURLTransformer( getURLTransformer() );
            aTargetURL.Complete = aCommandURL;
            xURLTransformer->parseStrict( aTargetURL );

            URLToDispatchMap::iterator pIter = m_aListenerMap.find( aCommandURL );
            if ( pIter != m_aListenerMap.end() )
                xDispatch = pIter->second;
            else
            {
                Reference< ::com::sun::star::frame::XDispatchProvider > xDispatchProvider(
                    m_xFrame->getController(), UNO_QUERY );
                if ( xDispatchProvider.is() )
                    xDispatch = xDispatchProvider->queryDispatch( aTargetURL, ::rtl::OUString(), 0 );
            }
        }
    }

    if ( xDispatch.is() )
    {
        try
        {
            xDispatch->dispatch( aTargetURL, aArgs );
        }
        catch ( DisposedException& )
        {
        }
    }
}

} // svt
