/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dispatchprovider.hxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 10:50:27 $
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

#ifndef __FRAMEWORK_DISPATCH_DISPATCHPROVIDER_HXX_
#define __FRAMEWORK_DISPATCH_DISPATCHPROVIDER_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_SERVICES_FRAME_HXX_
#include <services/frame.hxx>
#endif

#include <classes/protocolhandlercache.hxx>

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_TRANSACTIONBASE_HXX_
#include <threadhelp/transactionbase.hxx>
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

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#include <macros/generic.hxx>
#endif

#ifndef __FRAMEWORK_STDTYPES_H_
#include <stdtypes.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_DISPATCHDESCRIPTOR_HPP_
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _CPPUHELPER_WEAKREF_HXX_
#include <cppuhelper/weakref.hxx>
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________

/**
    @descr          We know some special dispatch objects with diffrent functionality.
                    The can be created internaly by the following DispatchProvider.
                    Here we define some identifier to force creation of the right one.

    @modified       20.08.2003 08:34, as96863
*/
enum EDispatchHelper
{
    E_DEFAULTDISPATCHER     ,
    E_MENUDISPATCHER        ,
    E_HELPAGENTDISPATCHER   ,
    E_CREATEDISPATCHER      ,
    E_BLANKDISPATCHER       ,
    E_SELFDISPATCHER        ,
    E_CLOSEDISPATCHER
};

//_________________________________________________________________________________________________________________

/**
    @short          implement a helper for XDispatchProvider interface
    @descr          The result of a queryDispatch() call depends from the owner, which use an instance of this class.
                    (frame, desktop) All of them must provides different functionality.
                    E.g:    - task can be created by the desktop only
                            - a task can have a beamer as direct child
                            - a normal frame never can create a new one by himself

    @attention      Use this class as member only! Never use it as baseclass.
                    XInterface will be ambigous and we hold a weakreference to ouer OWNER - not to ouer SUPERCLASS!

    @base           ThreadHelpBase
                        supports threadsafe mechanism
    @base           OWeakObject
                        provides ref count and weak mechanism

    @devstatus      ready to use
    @threadsafe     yes
    @modified       17.05.2002 07:56, as96863
*/
class DispatchProvider  :   // interfaces
                            public  css::lang::XTypeProvider            ,
                            public  css::frame::XDispatchProvider       ,
                            // baseclasses
                            // Order is neccessary for right initialization!
                            private ThreadHelpBase                      ,
                            private TransactionBase                     ,
                            public  ::cppu::OWeakObject
{
    /* member */
    private:
        /// reference to global service manager to create new services
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xFactory;
        /// weakreference to owner frame (Don't use a hard reference. Owner can't delete us then!)
        css::uno::WeakReference< css::frame::XFrame > m_xFrame;
        /// different dispatcher to handle special dispatch calls, protocols or URLs (they will be created on demand.)
        css::uno::Reference< css::frame::XDispatch > m_xMenuDispatcher     ;
        css::uno::Reference< css::frame::XDispatch > m_xHelpAgentDispatcher;
/*      css::uno::Reference< css::frame::XDispatch > m_xBlankDispatcher    ;
        css::uno::Reference< css::frame::XDispatch > m_xSelfDispatcher     ;
        css::uno::Reference< css::frame::XDispatch > m_xDefaultDispatcher  ;*/
        /// cache of some other dispatch provider which are registered inside configuration to handle special URL protocols
        HandlerCache m_aProtocolHandlerCache;

    /* interface */
    public:
        FWK_DECLARE_XINTERFACE
        FWK_DECLARE_XTYPEPROVIDER

        DispatchProvider( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory ,
                          const css::uno::Reference< css::frame::XFrame >&              xFrame   );

        virtual css::uno::Reference< css::frame::XDispatch > SAL_CALL                       queryDispatch  ( const css::util::URL&                                       aURL             ,
                                                                                                             const ::rtl::OUString&                                      sTargetFrameName ,
                                                                                                                   sal_Int32                                             nSearchFlags     ) throw( css::uno::RuntimeException );
        virtual css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL queryDispatches( const css::uno::Sequence< css::frame::DispatchDescriptor >& lDescriptions    ) throw( css::uno::RuntimeException );

    /* helper */
    protected:
        // Let him protected! So nobody can use us as base ...
        virtual ~DispatchProvider();

    private:
        css::uno::Reference< css::frame::XDispatch > implts_getOrCreateDispatchHelper   (       EDispatchHelper                            eHelper                       ,
                                                                                          const css::uno::Reference< css::frame::XFrame >& xOwner                        ,
                                                                                          const ::rtl::OUString&                           sTarget = ::rtl::OUString()   ,
                                                                                                sal_Int32                                  nSearchFlags = 0              );
        sal_Bool                                     implts_isLoadableContent           ( const css::util::URL&                            aURL                          );
        css::uno::Reference< css::frame::XDispatch > implts_queryDesktopDispatch        ( const css::uno::Reference< css::frame::XFrame >  xDesktop                      ,
                                                                                          const css::util::URL&                            aURL                          ,
                                                                                          const ::rtl::OUString&                           sTargetFrameName              ,
                                                                                                sal_Int32                                  nSearchFlags                  );
        css::uno::Reference< css::frame::XDispatch > implts_queryFrameDispatch          ( const css::uno::Reference< css::frame::XFrame >  xFrame                        ,
                                                                                          const css::util::URL&                            aURL                          ,
                                                                                          const ::rtl::OUString&                           sTargetFrameName              ,
                                                                                                sal_Int32                                  nSearchFlags                  );
        css::uno::Reference< css::frame::XDispatch > implts_searchProtocolHandler       ( const css::util::URL&                            aURL                          );

}; // class DispatchProvider

} // namespace framework

#endif // #ifndef __FRAMEWORK_DISPATCH_DISPATCHPROVIDER_HXX_
