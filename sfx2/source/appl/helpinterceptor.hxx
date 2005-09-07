/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: helpinterceptor.hxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 17:39:58 $
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
#ifndef INCLUDED_SFX_HELPINTERCEPTOR_HXX
#define INCLUDED_SFX_HELPINTERCEPTOR_HXX

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase3.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDERINTERCEPTOR_HPP_
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XINTERCEPTORINFO_HPP_
#include <com/sun/star/frame/XInterceptorInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDERINTERCEPTION_HPP_
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTATUSLISTENER_HPP_
#include <com/sun/star/frame/XStatusListener.hpp>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif
#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif

struct HelpHistoryEntry_Impl
{
    String  aURL;
    com::sun::star::uno::Any    aViewData;

    HelpHistoryEntry_Impl( const String& rURL, const com::sun::star::uno::Any& rViewData ) :
        aURL( rURL ), aViewData(rViewData) {}
};

DECLARE_LIST(HelpHistoryList_Impl,HelpHistoryEntry_Impl*);

class SfxHelpWindow_Impl;
class HelpInterceptor_Impl : public ::cppu::WeakImplHelper3<

        ::com::sun::star::frame::XDispatchProviderInterceptor,
        ::com::sun::star::frame::XInterceptorInfo,
        ::com::sun::star::frame::XDispatch >

{
private:
friend class HelpDispatch_Impl;
friend class SfxHelpWindow_Impl;

    // the component which's dispatches we're intercepting
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterception > m_xIntercepted;

    // chaining
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > m_xSlaveDispatcher;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > m_xMasterDispatcher;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > m_xListener;

    HelpHistoryList_Impl*       m_pHistory;
    SfxHelpWindow_Impl*         m_pWindow;
    ULONG                       m_nCurPos;
    String                      m_aCurrentURL;
    com::sun::star::uno::Any    m_aViewData;

    void                        addURL( const String& rURL );

public:
    HelpInterceptor_Impl();
    ~HelpInterceptor_Impl();

    void                    setInterception( ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > xFrame );
    void                    SetStartURL( const String& rURL );
    String                  GetCurrentURL() const { return m_aCurrentURL; }



    const com::sun::star::uno::Any&     GetViewData()const {return m_aViewData;}

    sal_Bool                HasHistoryPred() const;     // is there a predecessor for the current in the history
    sal_Bool                HasHistorySucc() const;     // is there a successor for the current in the history

    // XDispatchProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > SAL_CALL
                            queryDispatch( const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > > SAL_CALL
                            queryDispatches( const ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchDescriptor >& aDescripts ) throw(::com::sun::star::uno::RuntimeException);

    // XDispatchProviderInterceptor
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > SAL_CALL
                            getSlaveDispatchProvider(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL   setSlaveDispatchProvider( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider >& xNewSlave ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > SAL_CALL
                            getMasterDispatchProvider(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL   setMasterDispatchProvider( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider >& xNewMaster ) throw(::com::sun::star::uno::RuntimeException);

    // XInterceptorInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
                            getInterceptedURLs(  ) throw(::com::sun::star::uno::RuntimeException);

    // XDispatch
    virtual void SAL_CALL   dispatch( const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL   addStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl, const ::com::sun::star::util::URL& aURL ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL   removeStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl, const ::com::sun::star::util::URL& aURL ) throw(::com::sun::star::uno::RuntimeException);

    // extras
    void                    InitWaiter( SfxHelpWindow_Impl* pWindow )
                                { m_pWindow = pWindow; }
    SfxHelpWindow_Impl*     GetHelpWindow() const { return m_pWindow; }
};

// HelpListener_Impl -----------------------------------------------------

class HelpListener_Impl : public ::cppu::WeakImplHelper1< ::com::sun::star::frame::XStatusListener >
{
private:
    HelpInterceptor_Impl*   pInterceptor;
    Link                    aChangeLink;
    String                  aFactory;

public:
    HelpListener_Impl( HelpInterceptor_Impl* pInter );

    virtual void SAL_CALL   statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event )
                                throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL   disposing( const ::com::sun::star::lang::EventObject& obj )
                                throw( ::com::sun::star::uno::RuntimeException );

    void                    SetChangeHdl( const Link& rLink ) { aChangeLink = rLink; }
    String                  GetFactory() const { return aFactory; }
};
// HelpStatusListener_Impl -----------------------------------------------------

class HelpStatusListener_Impl : public
::cppu::WeakImplHelper1< ::com::sun::star::frame::XStatusListener >
{
private:
    ::com::sun::star::uno::Reference < ::com::sun::star::frame::XDispatch > xDispatch;
    ::com::sun::star::frame::FeatureStateEvent                              aStateEvent;

public:
    HelpStatusListener_Impl(
        ::com::sun::star::uno::Reference < ::com::sun::star::frame::XDispatch > xDispatch,
        com::sun::star::util::URL& rURL);
    ~HelpStatusListener_Impl();

    virtual void SAL_CALL   statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event )
                                throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL   disposing( const ::com::sun::star::lang::EventObject& obj )
                                throw( ::com::sun::star::uno::RuntimeException );
    const ::com::sun::star::frame::FeatureStateEvent&
                            GetStateEvent() const {return aStateEvent;}
};


#endif // #ifndef INCLUDED_SFX_HELPINTERCEPTOR_HXX

