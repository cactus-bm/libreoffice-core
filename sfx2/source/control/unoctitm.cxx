/*************************************************************************
 *
 *  $RCSfile: unoctitm.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: mba $ $Date: 2001-11-21 14:55:18 $
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

#include <tools/debug.hxx>

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#include <svtools/intitem.hxx>
#include <svtools/itemset.hxx>

#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCONTROLLER_HPP_
#include <com/sun/star/frame/XController.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAMEACTIONLISTENER_HPP_
#include <com/sun/star/frame/XFrameActionListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMEACTIONEVENT_HPP_
#include <com/sun/star/frame/FrameActionEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMEACTION_HPP_
#include <com/sun/star/frame/FrameAction.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_DISPATCHRESULTSTATE_HPP_
#include <com/sun/star/frame/DispatchResultState.hpp>
#endif

#include <comphelper/processfactory.hxx>
#include <vos/mutex.hxx>

#include "unoctitm.hxx"
#include "viewfrm.hxx"
#include "frame.hxx"
#include "ctrlitem.hxx"
#include "sfxuno.hxx"
#include "bindings.hxx"
#include "dispatch.hxx"
#include "sfxsids.hrc"
#include "request.hxx"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
//long nOfficeDispatchCount = 0;

SFX_IMPL_XINTERFACE_2( SfxUnoControllerItem, OWeakObject, ::com::sun::star::frame::XStatusListener, ::com::sun::star::lang::XEventListener )
SFX_IMPL_XTYPEPROVIDER_2( SfxUnoControllerItem, ::com::sun::star::frame::XStatusListener, ::com::sun::star::lang::XEventListener )

SfxUnoControllerItem::SfxUnoControllerItem( SfxControllerItem *pItem, SfxBindings& rBind, const String& rCmd )
    : pCtrlItem( pItem )
    , pBindings( &rBind )
{
    DBG_ASSERT( !pCtrlItem || !pCtrlItem->IsBound(), "ControllerItem fehlerhaft!" );

    aCommand.Complete = rCmd;
    Reference < XURLTransformer > xTrans( ::comphelper::getProcessServiceFactory()->createInstance( rtl::OUString::createFromAscii("com.sun.star.util.URLTransformer" )), UNO_QUERY );
    xTrans->parseStrict( aCommand );
    pBindings->RegisterUnoController_Impl( this );
}

SfxUnoControllerItem::~SfxUnoControllerItem()
{
    // tell bindings to forget this controller ( if still connected )
    if ( pBindings )
        pBindings->ReleaseUnoController_Impl( this );
}

void SfxUnoControllerItem::UnBind()
{
    // connection to SfxControllerItem is lost
    pCtrlItem = NULL;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >  aRef( (::cppu::OWeakObject*)this, ::com::sun::star::uno::UNO_QUERY );
    ReleaseDispatch();
}

void SAL_CALL SfxUnoControllerItem::statusChanged(const ::com::sun::star::frame::FeatureStateEvent& rEvent) throw ( ::com::sun::star::uno::RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    DBG_ASSERT( pCtrlItem, "Dispatch hat den StatusListener nicht entfern!" );

    if ( rEvent.Requery )
    {
        // Fehler kann nur passieren, wenn das alte Dispatch fehlerhaft implementiert
        // ist, also removeStatusListener nicht gefunzt hat. Aber sowas soll
        // ja vorkommen ...
        // Also besser vor ReleaseDispatch gegen Abflug sch"utzen!
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >  aRef( (::cppu::OWeakObject*)this, ::com::sun::star::uno::UNO_QUERY  );
        ReleaseDispatch();
        if ( pCtrlItem )
            GetNewDispatch();       // asynchron ??
    }
    else if ( pCtrlItem )
    {
        // pass new status to the SfxControllerItem
        if ( aCommand.Complete.compareToAscii( "macro:", 6 ) == COMPARE_EQUAL )
            return;

        SfxItemState eState = SFX_ITEM_DISABLED;
        SfxPoolItem* pItem = NULL;
        if ( rEvent.IsEnabled )
        {
            eState = SFX_ITEM_AVAILABLE;
            ::com::sun::star::uno::Type pType = rEvent.State.getValueType();

            if ( pType == ::getBooleanCppuType() )
            {
                sal_Bool bTemp ;
                rEvent.State >>= bTemp ;
                pItem = new SfxBoolItem( pCtrlItem->GetId(), bTemp );
            }
            else if ( pType == ::getCppuType((const sal_uInt16*)0) )
            {
                sal_uInt16 nTemp ;
                rEvent.State >>= nTemp ;
                pItem = new SfxUInt16Item( pCtrlItem->GetId(), nTemp );
            }
            else if ( pType == ::getCppuType((const sal_uInt32*)0) )
            {
                sal_uInt32 nTemp ;
                rEvent.State >>= nTemp ;
                pItem = new SfxUInt32Item( pCtrlItem->GetId(), nTemp );
            }
            else if ( pType == ::getCppuType((const ::rtl::OUString*)0) )
            {
                ::rtl::OUString sTemp ;
                rEvent.State >>= sTemp ;
                pItem = new SfxStringItem( pCtrlItem->GetId(), sTemp );
            }
            else
                pItem = new SfxVoidItem( pCtrlItem->GetId() );
        }

        pCtrlItem->StateChanged( pCtrlItem->GetId(), eState, pItem );
        delete pItem;
    }
}

void  SAL_CALL SfxUnoControllerItem::disposing( const ::com::sun::star::lang::EventObject& rEvent ) throw ( ::com::sun::star::uno::RuntimeException )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >  aRef( (::cppu::OWeakObject*)this, ::com::sun::star::uno::UNO_QUERY );
    ReleaseDispatch();
}

void SfxUnoControllerItem::ReleaseDispatch()
{
    if ( xDispatch.is() )
    {
        xDispatch->removeStatusListener( (::com::sun::star::frame::XStatusListener*) this, aCommand );
        xDispatch = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > ();
    }
}

void SfxUnoControllerItem::GetNewDispatch()
{
    if ( !pBindings )
    {
        // Bindings released
        DBG_ERROR( "Tried to get dispatch, but no Bindings!" );
        return;
    }

    // forget old dispatch
    xDispatch = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > ();

    // no arms, no cookies !
    if ( !pBindings->GetDispatcher_Impl() || !pBindings->GetDispatcher_Impl()->GetFrame() )
        return;

    SfxFrame *pFrame = pBindings->GetDispatcher_Impl()->GetFrame()->GetFrame();
    SfxFrame *pParent = pFrame->GetParentFrame();
    if ( pParent )
        // parent may intercept
        xDispatch = TryGetDispatch( pParent );

    if ( !xDispatch.is() )
    {
        // no interception
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >  xFrame = pFrame->GetFrameInterface();
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider >  xProv( xFrame, ::com::sun::star::uno::UNO_QUERY );
        if ( xProv.is() )
            xDispatch = xProv->queryDispatch( aCommand, ::rtl::OUString(), 0 );
    }

    if ( xDispatch.is() )
        xDispatch->addStatusListener( (::com::sun::star::frame::XStatusListener*) this, aCommand );
    else if ( pCtrlItem )
        pCtrlItem->StateChanged( pCtrlItem->GetId(), SFX_ITEM_DISABLED, NULL );
}

::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  SfxUnoControllerItem::TryGetDispatch( SfxFrame *pFrame )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  xDisp;
    SfxFrame *pParent = pFrame->GetParentFrame();
    if ( pParent )
        // parent may intercept
        xDisp = TryGetDispatch( pParent );

    // only components may intercept
    if ( !xDisp.is() && pFrame->HasComponent() )
    {
        // no interception
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >  xFrame = pFrame->GetFrameInterface();
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider >  xProv( xFrame, ::com::sun::star::uno::UNO_QUERY );
        if ( xProv.is() )
            xDisp = xProv->queryDispatch( aCommand, ::rtl::OUString(), 0 );
    }

    return xDisp;
}

void SfxUnoControllerItem::Execute()
{
    // dispatch the resource
    if ( xDispatch.is() )
        xDispatch->dispatch( aCommand, ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >() );
}

void SfxUnoControllerItem::ReleaseBindings()
{
    // connection to binding is lost; so forget the binding and the dispatch
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >  aRef( (::cppu::OWeakObject*)this, ::com::sun::star::uno::UNO_QUERY );
    ReleaseDispatch();
    if ( pBindings )
        pBindings->ReleaseUnoController_Impl( this );
    pBindings = NULL;
}

void SfxStatusDispatcher::ReleaseAll()
{
    ::com::sun::star::lang::EventObject aObject;
    aObject.Source = (::cppu::OWeakObject*) this;
    aListeners.disposeAndClear( aObject );
}

void SAL_CALL SfxStatusDispatcher::dispatch( const ::com::sun::star::util::URL&, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& ) throw ( ::com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL SfxStatusDispatcher::dispatchWithNotification( const ::com::sun::star::util::URL& aURL,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& rListener ) throw( ::com::sun::star::uno::RuntimeException )
{
}

SFX_IMPL_XINTERFACE_1( SfxStatusDispatcher, OWeakObject, ::com::sun::star::frame::XNotifyingDispatch )
SFX_IMPL_XTYPEPROVIDER_1( SfxStatusDispatcher, ::com::sun::star::frame::XNotifyingDispatch )
//IMPLNAME "com.sun.star.comp.sfx2.StatusDispatcher",

SfxStatusDispatcher::SfxStatusDispatcher()
    : aListeners( aMutex )
{
}

void SAL_CALL SfxStatusDispatcher::addStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & aListener, const ::com::sun::star::util::URL& aURL) throw ( ::com::sun::star::uno::RuntimeException )
{
    aListeners.addInterface( aURL.Complete, aListener );
    if ( aURL.Complete.compareToAscii(".uno:LifeTime")==0 )
    {
        ::com::sun::star::frame::FeatureStateEvent aEvent;
        aEvent.FeatureURL = aURL;
        aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
        aEvent.IsEnabled = sal_True;
        aEvent.Requery = sal_False;
        aListener->statusChanged( aEvent );
    }
}

void SAL_CALL SfxStatusDispatcher::removeStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & aListener, const ::com::sun::star::util::URL& aURL ) throw ( ::com::sun::star::uno::RuntimeException )
{
    aListeners.removeInterface( aURL.Complete, aListener );
}

SFX_IMPL_XINTERFACE_1( SfxOfficeDispatch, SfxStatusDispatcher, ::com::sun::star::lang::XUnoTunnel )
SFX_IMPL_XTYPEPROVIDER_2( SfxOfficeDispatch, ::com::sun::star::frame::XNotifyingDispatch, ::com::sun::star::lang::XUnoTunnel )


//-------------------------------------------------------------------------
// XUnoTunnel
sal_Int64 SAL_CALL SfxOfficeDispatch::getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException)
{
    if ( aIdentifier == impl_getStaticIdentifier() )
        return (sal_Int64)this;
    else
        return 0;
}

/* ASDBG
void* SfxOfficeDispatch::getImplementation(Reflection *p)
{
    if( p == ::getCppuType((const SfxOfficeDispatch*)0) )
        return this;
    else
        return ::cppu::OWeakObject::getImplementation(p);

}

Reflection* ::getCppuType((const SfxOfficeDispatch*)0)
{
    static StandardClassReflection aRefl(
        0,
        createStandardClass(
            "SfxOfficeDispatch", ::cppu::OWeakObject::get::cppu::OWeakObjectIdlClass(),
            1,
            ::getCppuType((const ::com::sun::star::frame::XDispatch*)0) ) );
    return &aRefl;
}
*/

SfxOfficeDispatch::SfxOfficeDispatch( SfxBindings& rBindings, SfxDispatcher* pDispat, sal_uInt16 nSlotId, const ::com::sun::star::util::URL& rURL, sal_Bool bInter )
    : bIntercept( bInter )
{
//    nOfficeDispatchCount++;

    // this object is an adapter that shows a ::com::sun::star::frame::XDispatch-Interface to the outside and uses a SfxControllerItem to monitor a state
    pControllerItem = new SfxDispatchController_Impl( this, &rBindings, pDispat, nSlotId, rURL, bInter );
}

SfxOfficeDispatch::SfxOfficeDispatch( SfxDispatcher* pDispat, sal_uInt16 nSlotId, const ::com::sun::star::util::URL& rURL, sal_Bool bInter )
    : bIntercept( bInter )
{
//    nOfficeDispatchCount++;

    // this object is an adapter that shows a ::com::sun::star::frame::XDispatch-Interface to the outside and uses a SfxControllerItem to monitor a state
    pControllerItem = new SfxDispatchController_Impl( this, NULL, pDispat, nSlotId, rURL, bInter );
}

SfxOfficeDispatch::~SfxOfficeDispatch()
{
//    --nOfficeDispatchCount;

    if ( pControllerItem )
    {
        // when dispatch object is released, destroy its connection to this object and destroy it
        pControllerItem->UnBindController();
        delete pControllerItem;
    }
}

#ifdef MACOSX
    //MACOSX moved outside of class above
    // {38 57 CA 80 09 36 11 d4 83 FE 00 50 04 52 6B 21}
    static sal_uInt8 pGUID[16] = { 0x38, 0x57, 0xCA, 0x80, 0x09, 0x36, 0x11, 0xd4, 0x83, 0xFE, 0x00, 0x50, 0x04, 0x52, 0x6B, 0x21 };
    static ::com::sun::star::uno::Sequence< sal_Int8 > seqID((sal_Int8*)pGUID,16) ;
#endif

const ::com::sun::star::uno::Sequence< sal_Int8 >& SfxOfficeDispatch::impl_getStaticIdentifier()
{
#ifndef MACOSX
    //MACOSX moved outside of class above
    // {38 57 CA 80 09 36 11 d4 83 FE 00 50 04 52 6B 21}
    static sal_uInt8 pGUID[16] = { 0x38, 0x57, 0xCA, 0x80, 0x09, 0x36, 0x11, 0xd4, 0x83, 0xFE, 0x00, 0x50, 0x04, 0x52, 0x6B, 0x21 };
    static ::com::sun::star::uno::Sequence< sal_Int8 > seqID((sal_Int8*)pGUID,16) ;
#endif
    return seqID ;
}


void SAL_CALL SfxOfficeDispatch::dispatch( const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs ) throw ( ::com::sun::star::uno::RuntimeException )
{
    // ControllerItem is the Impl class
    if ( pControllerItem )
        pControllerItem->dispatch( aURL, aArgs, ::com::sun::star::uno::Reference < ::com::sun::star::frame::XDispatchResultListener >() );
}

void SAL_CALL SfxOfficeDispatch::dispatchWithNotification( const ::com::sun::star::util::URL& aURL,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& rListener ) throw( ::com::sun::star::uno::RuntimeException )
{
    // ControllerItem is the Impl class
    if ( pControllerItem )
        pControllerItem->dispatch( aURL, aArgs, rListener );
}

void SAL_CALL SfxOfficeDispatch::addStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & aListener, const ::com::sun::star::util::URL& aURL) throw ( ::com::sun::star::uno::RuntimeException )
{
    GetListeners().addInterface( aURL.Complete, aListener );
    if ( pControllerItem )
    {
        // ControllerItem is the Impl class
        pControllerItem->addStatusListener( aListener, aURL );
    }
}

SfxDispatcher* SfxOfficeDispatch::GetDispatcher_Impl()
{
    return pControllerItem->GetDispatcher();
}

SfxDispatchController_Impl::SfxDispatchController_Impl( SfxOfficeDispatch* pDisp, SfxBindings* pBind, SfxDispatcher* pDispat, sal_uInt16 nSlotId, const ::com::sun::star::util::URL& rURL, sal_Bool bInter )
    : pDispatch( pDisp )
    , aDispatchURL( rURL )
    , pBindings( pBind )
    , pDispatcher( pDispat )
    , pLastState( 0 )
    , nSlot( nSlotId )
{
    SetId( nSlot );
}

SfxDispatchController_Impl::~SfxDispatchController_Impl()
{
    if ( pLastState && !IsInvalidItem( pLastState ) )
        delete pLastState;

    if ( pDispatch )
    {
        // disconnect
        pDispatch->pControllerItem = NULL;

        // force all listeners to release the dispatch object
        ::com::sun::star::lang::EventObject aObject;
        aObject.Source = (::cppu::OWeakObject*) pDispatch;
        pDispatch->GetListeners().disposeAndClear( aObject );
    }
}

void SfxDispatchController_Impl::UnBindController()
{
    pDispatch = NULL;
    if ( IsBound() )
    {
        GetBindings().ENTERREGISTRATIONS();
        SfxControllerItem::UnBind();
        GetBindings().LEAVEREGISTRATIONS();
    }
}

void SAL_CALL SfxDispatchController_Impl::dispatch( const ::com::sun::star::util::URL& aURL,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& rListener ) throw( ::com::sun::star::uno::RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( pDispatch && aURL == aDispatchURL )
    {
        if ( !GetId() && pBindings )
        {
            pBindings->ENTERREGISTRATIONS();
            Bind( nSlot, pBindings );
            pBindings->LEAVEREGISTRATIONS();
        }

        if ( !pDispatcher && pBindings )
            pDispatcher = GetBindings().GetDispatcher_Impl();

        SfxCallMode nCall = SFX_CALLMODE_SLOT;
        sal_Int32 nCount = aArgs.getLength();
        const ::com::sun::star::beans::PropertyValue* pPropsVal = aArgs.getConstArray();
        if ( rListener.is() )
            nCall = SFX_CALLMODE_SYNCHRON;
        else
        {
            for ( sal_Int32 n=0; n<nCount; n++ )
            {
                const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[n];
                String aName = rProp.Name;
                sal_Bool bTemp ;
                if ( aName.EqualsAscii("SynchronMode") && (rProp.Value >>= bTemp) )
                {
                    nCall = SFX_CALLMODE_SYNCHRON;
                    break;
                }
            }
        }

        sal_Bool bSuccess = sal_False;
        sal_Bool bFailure = sal_False;
        const SfxPoolItem* pItem = NULL;
        if ( pDispatcher->GetBindings() )
        {
            // execute using bindings, enables support for toggle/enum etc.
            if ( !pDispatcher->IsLocked( GetId() ) )
            {
                SfxShell *pShell = 0;
                const SfxSlot *pSlot = 0;
                if ( pDispatcher->GetShellAndSlot_Impl( GetId(), &pShell, &pSlot, sal_False,
                        SFX_CALLMODE_MODAL==(nCall&SFX_CALLMODE_MODAL) ) )
                {
                    SfxAllItemSet aSet( pShell->GetPool() );
                    TransformParameters( GetId(), aArgs, aSet );
                    if ( aSet.Count() )
                    {
                        SfxRequest aReq( GetId(), nCall, aSet );
                        pDispatcher->GetBindings()->Execute_Impl( aReq, pSlot, pShell );
                        pItem = aReq.GetReturnValue();
                        bSuccess = aReq.IsDone() || pItem != NULL;
                        bFailure = aReq.IsCancelled();
                    }
                    else
                    {
                        SfxRequest aReq( GetId(), nCall, pShell->GetPool() );
                        pDispatcher->GetBindings()->Execute_Impl( aReq, pSlot, pShell );
                        pItem = aReq.GetReturnValue();
                        bSuccess = aReq.IsDone() || pItem != NULL;
                        bFailure = aReq.IsCancelled();
                    }
                }
            }
        }
        else
        {
            SfxAllItemSet aSet( SFX_APP()->GetPool() );
            TransformParameters( GetId(), aArgs, aSet );
            if ( aSet.Count() )
                pItem = pDispatcher->Execute( GetId(), nCall, aSet );
            else
                // SfxRequests take empty sets as argument sets, GetArgs() returning non-zero!
                pItem = pDispatcher->Execute( GetId(), nCall );

            // no bindings, no invalidate ( usually done in SfxDispatcher::Call_Impl()! )
            const SfxPoolItem* pState=0;
            SfxItemState eState = pDispatcher->QueryState( GetId(), pState );
            StateChanged( GetId(), eState, pState );
            bSuccess = (pItem != NULL);
        }

        if ( rListener.is() )
        {
            ::com::sun::star::frame::DispatchResultEvent aEvent;
            if ( bSuccess )
                aEvent.State = com::sun::star::frame::DispatchResultState::SUCCESS;
//            else if ( bFailure )
            else
                aEvent.State = com::sun::star::frame::DispatchResultState::FAILURE;
//            else
//                aEvent.State = com::sun::star::frame::DispatchResultState::DONTKNOW;

            aEvent.Source = (::com::sun::star::frame::XDispatch*) pDispatch;
            if ( bSuccess && pItem && !pItem->ISA(SfxVoidItem) )
                pItem->QueryValue( aEvent.Result );

            rListener->dispatchFinished( aEvent );
        }
    }
}

SfxDispatcher* SfxDispatchController_Impl::GetDispatcher()
{
    if ( !pDispatcher && pBindings )
        pDispatcher = GetBindings().GetDispatcher_Impl();
    return pDispatcher;
}

void SAL_CALL SfxDispatchController_Impl::addStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & aListener, const ::com::sun::star::util::URL& aURL) throw ( ::com::sun::star::uno::RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( !pDispatch )
        return;

    if ( !GetId() && pBindings )
    {
        pBindings->ENTERREGISTRATIONS();
        Bind( nSlot, pBindings );
        pBindings->LEAVEREGISTRATIONS();
    }

    const SfxPoolItem *pItem = 0;
    if ( !pDispatcher && pBindings )
        pDispatcher = GetBindings().GetDispatcher_Impl();
    SfxItemState eState = pDispatcher->QueryState( GetId(), pItem );
    ::com::sun::star::uno::Any aState;
    if ( pItem && !pItem->ISA(SfxVoidItem) )
        pItem->QueryValue( aState );

    ::com::sun::star::frame::FeatureStateEvent aEvent;
    aEvent.FeatureURL = aURL;
    aEvent.Source = (::com::sun::star::frame::XDispatch*) pDispatch;
    aEvent.IsEnabled = eState != SFX_ITEM_DISABLED;
    aEvent.Requery = sal_False;
    aEvent.State = aState;

    aListener->statusChanged( aEvent );
}

void SfxDispatchController_Impl::StateChanged( sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState )
{
    if ( !pDispatch )
        return;

    if ( pDispatch->IsInterceptDispatch() )
    {
        // If this Controller is made for an interception, the state can't be set from outside
        // because this would be the state of the interceptor itself!
        if ( !pDispatcher && pBindings )
            pDispatcher = GetBindings().GetDispatcher_Impl();
        eState = pDispatcher->QueryState( GetId(), pState );
    }

    // Bindings instance notifies controller about a state change, listeners must be notified also
    sal_Bool bNotify = sal_True;
    sal_Bool bBothAvailable = pLastState && pState && !IsInvalidItem(pLastState) && !IsInvalidItem(pState);
    if ( bBothAvailable )
        bNotify = pState->Type() != pLastState->Type() || *pState != *pLastState;
    else
        bNotify = ( pLastState != pState );
    if ( pLastState && !IsInvalidItem( pLastState ) )
        delete pLastState;
    pLastState = ( pState && !IsInvalidItem(pState) ) ? pState->Clone() : pState;

    ::cppu::OInterfaceContainerHelper* pContnr = pDispatch->GetListeners().getContainer ( aDispatchURL.Complete );
    if ( bNotify && pContnr )
    {
        ::com::sun::star::uno::Any aState;
        if ( pState && !pState->ISA(SfxVoidItem) )
            pState->QueryValue( aState );

        ::com::sun::star::frame::FeatureStateEvent aEvent;
        aEvent.FeatureURL = aDispatchURL;
        aEvent.Source = (::com::sun::star::frame::XDispatch*) pDispatch;
        aEvent.IsEnabled = eState != SFX_ITEM_DISABLED;
        aEvent.Requery = sal_False;
        aEvent.State = aState;

        ::cppu::OInterfaceIteratorHelper aIt( *pContnr );
        while( aIt.hasMoreElements() )
            ((::com::sun::star::frame::XStatusListener *)aIt.next())->statusChanged( aEvent );
    }
}


