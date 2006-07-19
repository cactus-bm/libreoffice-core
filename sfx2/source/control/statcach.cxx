/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: statcach.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-19 17:18:05 $
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

#ifdef SOLARIS
// HACK: prevent conflict between STLPORT and Workshop headers on Solaris 8
#include <ctime>
#endif

#include <string> // HACK: prevent conflict between STLPORT and Workshop headers

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
#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#include <svtools/eitem.hxx>
#include <svtools/intitem.hxx>
#include <svtools/stritem.hxx>
#include <svtools/visitem.hxx>
#include <comphelper/processfactory.hxx>

#ifndef GCC
#pragma hdrstop
#endif

#include "app.hxx"
#include "statcach.hxx"
#include "msg.hxx"
#include "ctrlitem.hxx"
#include "dispatch.hxx"
#include "sfxtypes.hxx"
#include "sfxuno.hxx"
#include "unoctitm.hxx"
#include "msgpool.hxx"
#include "viewfrm.hxx"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;

//====================================================================

DBG_NAME(SfxStateCache)
DBG_NAME(SfxStateCacheSetState)

SFX_IMPL_XINTERFACE_2( BindDispatch_Impl, OWeakObject, ::com::sun::star::frame::XStatusListener, ::com::sun::star::lang::XEventListener )
SFX_IMPL_XTYPEPROVIDER_2( BindDispatch_Impl, ::com::sun::star::frame::XStatusListener, ::com::sun::star::lang::XEventListener )

//-----------------------------------------------------------------------------
BindDispatch_Impl::BindDispatch_Impl( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > & rDisp, const ::com::sun::star::util::URL& rURL, SfxStateCache *pStateCache, const SfxSlot* pS )
    : xDisp( rDisp )
    , aURL( rURL )
    , pCache( pStateCache )
    , pSlot( pS )
{
    DBG_ASSERT( pCache && pSlot, "Invalid BindDispatch!");
    aStatus.IsEnabled = sal_True;
}

void SAL_CALL BindDispatch_Impl::disposing( const ::com::sun::star::lang::EventObject& ) throw( ::com::sun::star::uno::RuntimeException )
{
    if ( xDisp.is() )
    {
        xDisp->removeStatusListener( (::com::sun::star::frame::XStatusListener*) this, aURL );
        xDisp = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > ();
    }
}

void SAL_CALL  BindDispatch_Impl::statusChanged( const ::com::sun::star::frame::FeatureStateEvent& rEvent ) throw( ::com::sun::star::uno::RuntimeException )
{
    aStatus = rEvent;
    if ( !pCache )
        return;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >  xRef( (::cppu::OWeakObject*)this, ::com::sun::star::uno::UNO_QUERY );
    if ( aStatus.Requery )
        pCache->Invalidate( sal_True );
    else
    {
        SfxPoolItem *pItem=NULL;
        sal_uInt16 nId = pCache->GetId();
        SfxItemState eState = SFX_ITEM_DISABLED;
        // pCache->Invalidate( sal_False );
        if ( !aStatus.IsEnabled )
        {
            // default
        }
        else if (aStatus.State.hasValue())
        {
            eState = SFX_ITEM_AVAILABLE;
            ::com::sun::star::uno::Any aAny = aStatus.State;

            ::com::sun::star::uno::Type pType = aAny.getValueType();
            if ( pType == ::getBooleanCppuType() )
            {
                sal_Bool bTemp = false;
                aAny >>= bTemp ;
                pItem = new SfxBoolItem( nId, bTemp );
            }
            else if ( pType == ::getCppuType((const sal_uInt16*)0) )
            {
                sal_uInt16 nTemp = 0;
                aAny >>= nTemp ;
                pItem = new SfxUInt16Item( nId, nTemp );
            }
            else if ( pType == ::getCppuType((const sal_uInt32*)0) )
            {
                sal_uInt32 nTemp ;
                aAny >>= nTemp ;
                pItem = new SfxUInt32Item( nId, nTemp );
            }
            else if ( pType == ::getCppuType((const ::rtl::OUString*)0) )
            {
                ::rtl::OUString sTemp ;
                aAny >>= sTemp ;
                pItem = new SfxStringItem( nId, sTemp );
            }
            else
            {
                if ( pSlot )
                    pItem = pSlot->GetType()->CreateItem();
                if ( pItem )
                {
                    pItem->SetWhich( nId );
                    pItem->PutValue( aAny );
                }
                else
                    pItem = new SfxVoidItem( nId );
            }
        }
        else
        {
            // DONTCARE status
            pItem = new SfxVoidItem(0);
            eState = SFX_ITEM_UNKNOWN;
        }

        for ( SfxControllerItem *pCtrl = pCache->GetItemLink();
            pCtrl;
            pCtrl = pCtrl->GetItemLink() )
            pCtrl->StateChanged( nId, eState, pItem );

       delete pItem;
    }
}

void BindDispatch_Impl::Release()
{
    if ( xDisp.is() )
    {
        xDisp->removeStatusListener( (::com::sun::star::frame::XStatusListener*) this, aURL );
        xDisp = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > ();
    }

    pCache = NULL;
    release();
}

const ::com::sun::star::frame::FeatureStateEvent& BindDispatch_Impl::GetStatus() const
{
    return aStatus;
}

void BindDispatch_Impl::Dispatch( sal_Bool bForceSynchron )
{
    if ( xDisp.is() && aStatus.IsEnabled )
    {
        ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue > aProps(1);
        aProps.getArray()[0].Name = DEFINE_CONST_UNICODE("SynchronMode");
        aProps.getArray()[0].Value <<= bForceSynchron ;
        xDisp->dispatch( aURL, aProps );
    }
}

//--------------------------------------------------------------------

/*  Dieser Konstruktor fuer einen ungueltigen Cache, der sich also
    bei der ersten Anfrage zun"achst updated.
 */

SfxStateCache::SfxStateCache( sal_uInt16 nFuncId ):
    pDispatch( 0 ),
    nId(nFuncId),
    pInternalController(0),
    pController(0),
    pLastItem( 0 ),
    eLastState( 0 ),
    bItemVisible( sal_True )
{
    DBG_MEMTEST();
    DBG_CTOR(SfxStateCache, 0);
    bCtrlDirty = sal_True;
    bSlotDirty = sal_True;
    bItemDirty = sal_True;
}

//--------------------------------------------------------------------

/*  Der Destruktor pr"uft per Assertion, ob noch Controller angemeldet
    sind.
 */

SfxStateCache::~SfxStateCache()
{
    DBG_MEMTEST();
    DBG_DTOR(SfxStateCache, 0);
    DBG_ASSERT( pController == 0 && pInternalController == 0, "es sind noch Controller angemeldet" );
    if ( !IsInvalidItem(pLastItem) )
        delete pLastItem;
    if ( pDispatch )
    {
        pDispatch->Release();
        pDispatch = NULL;
    }
}

//--------------------------------------------------------------------
// invalidates the cache (next request will force update)
void SfxStateCache::Invalidate( sal_Bool bWithMsg )
{
    bCtrlDirty = sal_True;
    if ( bWithMsg )
    {
        bSlotDirty = sal_True;
        aSlotServ.SetSlot( 0 );
        if ( pDispatch )
        {
            pDispatch->Release();
            pDispatch = NULL;
        }
    }
}

//--------------------------------------------------------------------

// gets the corresponding function from the dispatcher or the cache

const SfxSlotServer* SfxStateCache::GetSlotServer( SfxDispatcher &rDispat , const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > & xProv )
{
    DBG_MEMTEST();
    DBG_CHKTHIS(SfxStateCache, 0);

    if ( bSlotDirty )
    {
        // get the SlotServer; we need it for internal controllers anyway, but also in most cases
        rDispat._FindServer( nId, aSlotServ, sal_False );

        DBG_ASSERT( !pDispatch, "Old Dispatch not removed!" );

        // we don't need to check the dispatch provider if we only have an internal controller
        if ( xProv.is() )
        {
            const SfxSlot* pSlot = aSlotServ.GetSlot();
            if ( !pSlot )
                // get the slot - even if it is disabled on the dispatcher
                pSlot = SfxSlotPool::GetSlotPool( rDispat.GetFrame() ).GetSlot( nId );

            if ( !pSlot || !pSlot->pUnoName )
            {
                bSlotDirty = sal_False;
                bCtrlDirty = sal_True;
                return aSlotServ.GetSlot()? &aSlotServ: 0;
            }

            // create the dispatch name from the slot data
            ::com::sun::star::util::URL aURL;
            ::rtl::OUString aCmd = DEFINE_CONST_UNICODE(".uno:");
            aCmd += ::rtl::OUString::createFromAscii( pSlot->GetUnoName() );

            // try to get a dispatch object for this command
            aURL.Complete = aCmd;
            Reference < XURLTransformer > xTrans( ::comphelper::getProcessServiceFactory()->createInstance( rtl::OUString::createFromAscii("com.sun.star.util.URLTransformer" )), UNO_QUERY );
            xTrans->parseStrict( aURL );
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  xDisp = xProv->queryDispatch( aURL, ::rtl::OUString(), 0 );
            if ( xDisp.is() )
            {
                // test the dispatch object if it is just a wrapper for a SfxDispatcher
                ::com::sun::star::uno::Reference< ::com::sun::star::lang::XUnoTunnel > xTunnel( xDisp, ::com::sun::star::uno::UNO_QUERY );
                SfxOfficeDispatch* pDisp = NULL;
                if ( xTunnel.is() )
                {
                    sal_Int64 nImplementation = xTunnel->getSomething(SfxOfficeDispatch::impl_getStaticIdentifier());
                    pDisp = reinterpret_cast< SfxOfficeDispatch* >(sal::static_int_cast< sal_IntPtr >( nImplementation ));
                }

                if ( pDisp )
                {
                    // The intercepting object is an SFX component
                    // If this dispatch object does not use the wanted dispatcher or the AppDispatcher, it's treated like any other UNO component
                    // (intercepting by internal dispatches)
                    SfxDispatcher *pDispatcher = pDisp->GetDispatcher_Impl();
                    if ( pDispatcher == &rDispat || pDispatcher == SFX_APP()->GetAppDispatcher_Impl() )
                    {
                        // so we can use it directly
                        bSlotDirty = sal_False;
                        bCtrlDirty = sal_True;
                        return aSlotServ.GetSlot()? &aSlotServ: 0;
                    }
                }

                // so the dispatch object isn't a SfxDispatcher wrapper or it is one, but it uses another dispatcher, but not rDispat
                pDispatch = new BindDispatch_Impl( xDisp, aURL, this, pSlot );
                pDispatch->acquire();

                // flags must be set before adding StatusListener because the dispatch object will set the state
                bSlotDirty = sal_False;
                bCtrlDirty = sal_True;
                xDisp->addStatusListener( pDispatch, aURL );
            }
            else if ( rDispat.GetFrame() )
            {
                ::com::sun::star::uno::Reference < ::com::sun::star::frame::XDispatchProvider > xFrameProv(
                        rDispat.GetFrame()->GetFrame()->GetFrameInterface(), ::com::sun::star::uno::UNO_QUERY );
                if ( xFrameProv != xProv )
                    return GetSlotServer( rDispat, xFrameProv );
            }
        }

        bSlotDirty = sal_False;
        bCtrlDirty = sal_True;
    }

    // we *always* return a SlotServer (if there is one); but in case of an external dispatch we might not use it
    // for the "real" (non internal) controllers
    return aSlotServ.GetSlot()? &aSlotServ: 0;
}


//--------------------------------------------------------------------

// Status setzen in allen Controllern

void SfxStateCache::SetState
(
    SfxItemState        eState,     // <SfxItemState> von 'pState'
    const SfxPoolItem*  pState,     // Status des Slots, ggf. 0 oder -1
    BOOL bMaybeDirty
)

/*  [Beschreibung]

    Diese Methode verteilt die Status auf alle an dieser SID gebundenen
    <SfxControllerItem>s. Ist der Wert derselbe wie zuvor und wurde in-
    zwischen weder ein Controller angemeldet, noch ein Controller invalidiert,
    dann wird kein Wert weitergeleitet. Dadurch wird z.B. Flackern in
    ListBoxen vermieden.
*/

{
//    if ( pDispatch )
//        return;
    SetState_Impl( eState, pState, bMaybeDirty );
}

//--------------------------------------------------------------------

void SfxStateCache::SetVisibleState( BOOL bShow )
{
    SfxItemState        eState( SFX_ITEM_AVAILABLE );
    const SfxPoolItem*  pState( NULL );
    sal_Bool            bNotify( sal_False );
    sal_Bool            bDeleteItem( sal_False );

    if ( bShow != bItemVisible )
    {
        bItemVisible = bShow;
        if ( bShow )
        {
            if ( IsInvalidItem(pLastItem) || ( pLastItem == NULL ))
            {
                pState = new SfxVoidItem( nId );
                bDeleteItem = sal_True;
            }
            else
                pState = pLastItem;

            eState = eLastState;
            bNotify = ( pState != 0 );
        }
        else
        {
            pState = new SfxVisibilityItem( nId, FALSE );
            bDeleteItem = sal_True;
        }

        // Controller updaten
        if ( !pDispatch && pController )
        {
            for ( SfxControllerItem *pCtrl = pController;
                    pCtrl;
                    pCtrl = pCtrl->GetItemLink() )
                pCtrl->StateChanged( nId, eState, pState );
        }

        if ( pInternalController )
            pInternalController->StateChanged( nId, eState, pState );

        if ( !bDeleteItem )
            delete pState;
    }
}

//--------------------------------------------------------------------

void SfxStateCache::SetState_Impl
(
    SfxItemState        eState,     // <SfxItemState> von 'pState'
    const SfxPoolItem*  pState,     // Status des Slots, ggf. 0 oder -1
    BOOL bMaybeDirty
)
{
    (void)bMaybeDirty; //unused
    DBG_MEMTEST();
    DBG_CHKTHIS(SfxStateCache, 0);

    // wenn zwischen Enter- und LeaveRegistrations ein hartes Update kommt
    // k"onnen zwischenzeitlich auch Cached ohne Controller exisitieren
    if ( !pController && !pInternalController )
        return;

    DBG_ASSERT( bMaybeDirty || !bSlotDirty, "setting state of dirty message" );
//  DBG_ASSERT( bCtrlDirty || ( aSlotServ.GetSlot() && aSlotServ.GetSlot()->IsMode(SFX_SLOT_VOLATILE) ), ! Discussed with MBA
//              "setting state of non dirty controller" );
    DBG_ASSERT( SfxControllerItem::GetItemState(pState) == eState, "invalid SfxItemState" );
    DBG_PROFSTART(SfxStateCacheSetState);

    // m"ussen die Controller "uberhaupt benachrichtigt werden?
    FASTBOOL bNotify = bItemDirty;
    if ( !bItemDirty )
    {
        FASTBOOL bBothAvailable = pLastItem && pState &&
                    !IsInvalidItem(pState) && !IsInvalidItem(pLastItem);
        DBG_ASSERT( !bBothAvailable || pState != pLastItem, "setting state with own item" );
        if ( bBothAvailable )
            bNotify = pState->Type() != pLastItem->Type() ||
                      *pState != *pLastItem;
        else
            bNotify = ( pState != pLastItem ) || ( eState != eLastState );
    }

    if ( bNotify )
    {
        // Controller updaten
        if ( !pDispatch && pController )
        {
            for ( SfxControllerItem *pCtrl = pController;
                pCtrl;
                pCtrl = pCtrl->GetItemLink() )
                pCtrl->StateChanged( nId, eState, pState );
        }

        if ( pInternalController )
            ((SfxDispatchController_Impl *)pInternalController)->StateChanged( nId, eState, pState, &aSlotServ );

        // neuen Wert merken
        if ( !IsInvalidItem(pLastItem) )
            DELETEZ(pLastItem);
        if ( pState && !IsInvalidItem(pState) )
            pLastItem = pState->Clone();
        else
            pLastItem = 0;
        eLastState = eState;
        bItemDirty = sal_False;
    }

    bCtrlDirty = sal_False;
    DBG_PROFSTOP(SfxStateCacheSetState);
}


//--------------------------------------------------------------------

// alten Status in allen Controllern nochmal setzen

void SfxStateCache::SetCachedState( BOOL bAlways )
{
    DBG_MEMTEST();
    DBG_CHKTHIS(SfxStateCache, 0);
    DBG_ASSERT(pController==NULL||pController->GetId()==nId, "Cache mit falschem ControllerItem" );
    DBG_PROFSTART(SfxStateCacheSetState);

    // nur updaten wenn cached item vorhanden und auch verarbeitbar
    // (Wenn der State gesendet wird, mu\s sichergestellt sein, da\s ein
    // Slotserver vorhanden ist, s. SfxControllerItem::GetCoreMetric() )
    if ( bAlways || ( !bItemDirty && !bSlotDirty ) )
    {
        // Controller updaten
        if ( !pDispatch && pController )
        {
            for ( SfxControllerItem *pCtrl = pController;
                pCtrl;
                pCtrl = pCtrl->GetItemLink() )
                pCtrl->StateChanged( nId, eLastState, pLastItem );
        }

        if ( pInternalController )
            ((SfxDispatchController_Impl *)pInternalController)->StateChanged( nId, eLastState, pLastItem, &aSlotServ );

        // Controller sind jetzt ok
        bCtrlDirty = sal_True;
    }

    DBG_PROFSTOP(SfxStateCacheSetState);
}


//--------------------------------------------------------------------

// FloatingWindows in allen Controls mit dieser Id zerstoeren

void SfxStateCache::DeleteFloatingWindows()
{
    DBG_MEMTEST();
    DBG_CHKTHIS(SfxStateCache, 0);

    SfxControllerItem *pNextCtrl=0;
    for ( SfxControllerItem *pCtrl=pController; pCtrl; pCtrl=pNextCtrl )
    {
        DBG_TRACE((ByteString("pCtrl: ").Append(ByteString::CreateFromInt32((sal_uInt32)pCtrl))).GetBuffer());
        pNextCtrl = pCtrl->GetItemLink();
        pCtrl->DeleteFloatingWindow();
    }
}

::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  SfxStateCache::GetDispatch() const
{
    if ( pDispatch )
        return pDispatch->xDisp;
    return ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > ();
}

void SfxStateCache::Dispatch( sal_Bool bForceSynchron )
{
    // protect pDispatch against destruction in the call
    ::com::sun::star::uno::Reference < ::com::sun::star::frame::XStatusListener > xKeepAlive( pDispatch );
    if ( pDispatch )
        pDispatch->Dispatch( bForceSynchron );
}


