/*************************************************************************
 *
 *  $RCSfile: persistentwindowstate.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-17 12:52:54 $
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

#ifndef __FRAMEWORK_HELPER_PERSISTENTWINDOWSTATE_HXX_
#include <helper/persistentwindowstate.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_WRITEGUARD_HXX_
#include <threadhelp/writeguard.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_READGUARD_HXX_
#include <threadhelp/readguard.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#include <macros/generic.hxx>
#endif

#ifndef __FRAMEWORK_SERVICES_H_
#include <services.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICXEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_FRAME_XMODULEMANAGER_HPP_
#include <drafts/com/sun/star/frame/XModuleManager.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _COMPHELPER_CONFIGURATIONHELPER_HXX_
#include <comphelper/configurationhelper.hxx>
#endif

#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif

#ifndef _SV_SYSWIN_HXX
#include <vcl/syswin.hxx>
#endif

#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _SV_WRKWIN_HXX
#include <vcl/wrkwin.hxx>
#endif

//_________________________________________________________________________________________________________________
//  namespace

namespace framework{

//_________________________________________________________________________________________________________________
//  definitions

//*****************************************************************************************************************
//  XInterface, XTypeProvider

DEFINE_XINTERFACE_4(PersistentWindowState                                                       ,
                    OWeakObject                                                                 ,
                    DIRECT_INTERFACE (css::lang::XTypeProvider                                  ),
                    DIRECT_INTERFACE (css::lang::XInitialization                                ),
                    DIRECT_INTERFACE (css::frame::XFrameActionListener                          ),
                    DERIVED_INTERFACE(css::lang::XEventListener,css::frame::XFrameActionListener))

DEFINE_XTYPEPROVIDER_4(PersistentWindowState           ,
                       css::lang::XTypeProvider        ,
                       css::lang::XInitialization      ,
                       css::frame::XFrameActionListener,
                       css::lang::XEventListener       )

//*****************************************************************************************************************
PersistentWindowState::PersistentWindowState(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ThreadHelpBase(&Application::GetSolarMutex())
    , m_xSMGR       (xSMGR                        )
{
}

//*****************************************************************************************************************
PersistentWindowState::~PersistentWindowState()
{
}

//*****************************************************************************************************************
void SAL_CALL PersistentWindowState::initialize(const css::uno::Sequence< css::uno::Any >& lArguments)
    throw(css::uno::Exception       ,
          css::uno::RuntimeException)
{
    // check arguments
    css::uno::Reference< css::frame::XFrame > xFrame;
    if (lArguments.getLength() < 1)
        throw css::lang::IllegalArgumentException(
                DECLARE_ASCII("Empty argument list!"),
                static_cast< ::cppu::OWeakObject* >(this),
                1);

    lArguments[0] >>= xFrame;
    if (!xFrame.is())
        throw css::lang::IllegalArgumentException(
                DECLARE_ASCII("No valid frame specified!"),
                static_cast< ::cppu::OWeakObject* >(this),
                1);

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    // hold the frame as weak reference(!) so it can die everytimes :-)
    m_xFrame = xFrame;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    // start listening
    xFrame->addFrameActionListener(this);
}

//*****************************************************************************************************************
void SAL_CALL PersistentWindowState::frameAction(const css::frame::FrameActionEvent& aEvent)
    throw(css::uno::RuntimeException)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR ;
    css::uno::Reference< css::frame::XFrame >              xFrame(m_xFrame.get(), css::uno::UNO_QUERY);
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // frame already gone ? We hold it weak only ...
    if (!xFrame.is())
        return;

    // no window -> no position and size available
    css::uno::Reference< css::awt::XWindow > xWindow = xFrame->getContainerWindow();
    if (!xWindow.is())
        return;

    // unknown module -> no configuration available!
    ::rtl::OUString sModuleName = PersistentWindowState::implst_identifyModule(xSMGR, xFrame);
    if (!sModuleName.getLength())
        return;

    switch(aEvent.Action)
    {
        case css::frame::FrameAction_COMPONENT_ATTACHED :
            {
                ::rtl::OUString sWindowState = PersistentWindowState::implst_getWindowStateFromConfig(xSMGR, sModuleName);
                PersistentWindowState::implst_setWindowStateOnWindow(xWindow,sWindowState);
            }
            break;

        case css::frame::FrameAction_COMPONENT_REATTACHED :
            {
                // nothing todo here, because its not allowed to change position and size
                // of an alredy existing frame!
            }
            break;

        case css::frame::FrameAction_COMPONENT_DETACHING :
            {
                ::rtl::OUString sWindowState = PersistentWindowState::implst_getWindowStateFromWindow(xWindow);
                PersistentWindowState::implst_setWindowStateOnConfig(xSMGR, sModuleName, sWindowState);
            }
            break;
    }
}

//*****************************************************************************************************************
void SAL_CALL PersistentWindowState::disposing(const css::lang::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    // nothing todo here - because we hold the frame as weak reference only
}

//*****************************************************************************************************************
::rtl::OUString PersistentWindowState::implst_identifyModule(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR ,
                                                             const css::uno::Reference< css::frame::XFrame >&              xFrame)
{
    ::rtl::OUString sModuleName;

    css::uno::Reference< dcss::frame::XModuleManager > xModuleManager(
        xSMGR->createInstance(SERVICENAME_MODULEMANAGER),
        css::uno::UNO_QUERY_THROW);

    try
    {
        sModuleName = xModuleManager->identify(xFrame);
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        { sModuleName = ::rtl::OUString(); }

    return sModuleName;
}

//*****************************************************************************************************************
::rtl::OUString PersistentWindowState::implst_getWindowStateFromConfig(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR      ,
                                                                       const ::rtl::OUString&                                        sModuleName)
{
    ::rtl::OUString sWindowState;

    ::rtl::OUStringBuffer sRelPathBuf(256);
    sRelPathBuf.appendAscii("Office/Factories/*[\"");
    sRelPathBuf.append     (sModuleName            );
    sRelPathBuf.appendAscii("\"]"                  );

    ::rtl::OUString sPackage = ::rtl::OUString::createFromAscii("org.openoffice.Setup/");
    ::rtl::OUString sRelPath = sRelPathBuf.makeStringAndClear();
    ::rtl::OUString sKey     = ::rtl::OUString::createFromAscii("ooSetupFactoryWindowAttributes");

    try
    {
        css::uno::Any aWindowState = ::comphelper::ConfigurationHelper::readDirectKey(xSMGR,
                                                                                      sPackage,
                                                                                      sRelPath,
                                                                                      sKey,
                                                                                      ::comphelper::ConfigurationHelper::E_READONLY);
        aWindowState >>= sWindowState;
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        { sWindowState = ::rtl::OUString(); }

    return sWindowState;
}

//*****************************************************************************************************************
void PersistentWindowState::implst_setWindowStateOnConfig(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR       ,
                                                          const ::rtl::OUString&                                        sModuleName ,
                                                          const ::rtl::OUString&                                        sWindowState)
{
    ::rtl::OUStringBuffer sRelPathBuf(256);
    sRelPathBuf.appendAscii("Office/Factories/*[\"");
    sRelPathBuf.append     (sModuleName            );
    sRelPathBuf.appendAscii("\"]"                  );

    ::rtl::OUString sPackage = ::rtl::OUString::createFromAscii("org.openoffice.Setup/");
    ::rtl::OUString sRelPath = sRelPathBuf.makeStringAndClear();
    ::rtl::OUString sKey     = ::rtl::OUString::createFromAscii("ooSetupFactoryWindowAttributes");

    try
    {
        ::comphelper::ConfigurationHelper::writeDirectKey(xSMGR,
                                                          sPackage,
                                                          sRelPath,
                                                          sKey,
                                                          css::uno::makeAny(sWindowState),
                                                          ::comphelper::ConfigurationHelper::E_STANDARD);
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        {}
}

//*****************************************************************************************************************
::rtl::OUString PersistentWindowState::implst_getWindowStateFromWindow(const css::uno::Reference< css::awt::XWindow >& xWindow)
{
    ::rtl::OUString sWindowState;

    if (xWindow.is())
    {
        // SOLAR SAFE -> ------------------------
        ::vos::OClearableGuard aSolarLock(Application::GetSolarMutex());

        Window* pWindow = VCLUnoHelper::GetWindow(xWindow);
        // check for system window is neccessary to guarantee correct pointer cast!
        if (
            (pWindow                  ) &&
            (pWindow->IsSystemWindow())
           )
        {
            ULONG nMask  =   WINDOWSTATE_MASK_ALL;
                  nMask &= ~(WINDOWSTATE_MASK_MINIMIZED);
            sWindowState = B2U_ENC(
                            ((SystemWindow*)pWindow)->GetWindowState(nMask),
                            RTL_TEXTENCODING_UTF8);
        }

        aSolarLock.clear();
        // <- SOLAR SAFE ------------------------
    }

    return sWindowState;
}


//*********************************************************************************************************
void PersistentWindowState::implst_setWindowStateOnWindow(const css::uno::Reference< css::awt::XWindow >& xWindow     ,
                                                          const ::rtl::OUString&                          sWindowState)
{
    if (
        (!xWindow.is()                ) ||
        ( sWindowState.getLength() < 1)
       )
        return;

    // SOLAR SAFE -> ------------------------
    ::vos::OClearableGuard aSolarLock(Application::GetSolarMutex());

    Window* pWindow = VCLUnoHelper::GetWindow(xWindow);
    if (!pWindow)
        return;

    // check for system and work window - its neccessary to guarantee correct pointer cast!
    sal_Bool bSystemWindow = pWindow->IsSystemWindow();
    sal_Bool bWorkWindow   = (pWindow->GetType() == WINDOW_WORKWINDOW);

    if (!bSystemWindow && !bWorkWindow)
        return;

    SystemWindow* pSystemWindow = (SystemWindow*)pWindow;
    WorkWindow*   pWorkWindow   = (WorkWindow*  )pWindow;

    // dont save this special state!
    if (pWorkWindow->IsMinimized())
        return;

    pSystemWindow->SetWindowState(U2B_ENC(sWindowState,RTL_TEXTENCODING_UTF8));

    aSolarLock.clear();
    // <- SOLAR SAFE ------------------------
}

} // namespace framework
