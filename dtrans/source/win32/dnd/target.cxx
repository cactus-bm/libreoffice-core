/*************************************************************************
 *
 *  $RCSfile: target.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: mh $ $Date: 2001-01-31 15:37:19 $
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

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_DNDCONSTANTS_HPP_
#include <com/sun/star/datatransfer/dnd/DNDConstants.hpp>
#endif
#ifndef _COM_SUN_STAR_DATATRANSFER_XTRANSFERABLE_HPP_
#include <com/sun/star/datatransfer/XTransferable.hpp>
#endif

#include "target.hxx"
#include "globals.hxx"
#include "targetdropcontext.hxx"
#include "targetdragcontext.hxx"
#include "DataObjectWrapper.hxx"
//#include "windroptarget.hxx"
#include <rtl/ustring.h>
using namespace rtl;
using namespace cppu;
using namespace osl;
using namespace com::sun::star::datatransfer;
using namespace com::sun::star::datatransfer::dnd;
using namespace com::sun::star::datatransfer::dnd::DNDConstants;

DropTarget::DropTarget( const Reference<XMultiServiceFactory>& sf, HWND hwin):
                        m_hWnd( hwin),
                        m_serviceFactory( sf),
                        WeakComponentImplHelper1<XDropTarget>(m_mutex),
                        m_bDropTargetRegistered(sal_False),
                        m_nDefaultActions(ACTION_COPY|ACTION_MOVE|ACTION_LINK),
                        m_nListenerDropAction( ACTION_NONE),
                        m_currentEventId(0)
{
    OSL_ASSERT( IsWindow(m_hWnd));
}

DropTarget::~DropTarget()
{

}

#ifdef DEBUG
void SAL_CALL DropTarget::release()
{
    if( m_refCount == 1)
    {
        int a = m_refCount;
    }
    WeakComponentImplHelper1<XDropTarget>::release();

}
#endif

void SAL_CALL DropTarget::disposing()
{
    if( m_bDropTargetRegistered)
    {
        RevokeDragDrop(m_hWnd);
        CoLockObjectExternal( static_cast<IDropTarget*>(this), FALSE, TRUE);
    }
}

// XInitialization
//void SAL_CALL DropTarget::initialize( const Sequence< Any >& aArguments )
//      throw(Exception, RuntimeException)
//{
//
//
//  m_hWnd= *(HWND*)aArguments[0].getValue();
//  OSL_ASSERT( IsWindow(m_hWnd));
//
//  if( ! IsWindow( m_hWnd))
//      throw Exception( OUString::createFromAscii("Initialization of DropTarget ")
//            + OUString( RTL_CONSTASCII_USTRINGPARAM( DNDTARGET_SERVICE_NAME) )
//            + OUString::createFromAscii("service failed"),
//            Reference<XInterface>((XInitialization*)this, UNO_QUERY));
//
//}

// XDropTarget
void SAL_CALL DropTarget::addDropTargetListener( const Reference< XDropTargetListener >& dtl )
        throw(RuntimeException)
{
    rBHelper.addListener( ::getCppuType( &dtl ), dtl );
}

void SAL_CALL DropTarget::removeDropTargetListener( const Reference< XDropTargetListener >& dtl )
        throw(RuntimeException)
{
    rBHelper.addListener( ::getCppuType( &dtl ), dtl );
}

sal_Bool SAL_CALL DropTarget::isActive(  ) throw(RuntimeException)
{
    return m_bDropTargetRegistered;
}

void SAL_CALL DropTarget::setActive( sal_Bool _b ) throw(RuntimeException)
{
    MutexGuard g(m_mutex);

    if( _b == sal_True)
    {
        if( SUCCEEDED( CoLockObjectExternal( static_cast<IDropTarget*>(this), TRUE, FALSE)))
            if( SUCCEEDED( RegisterDragDrop( m_hWnd,  static_cast<IDropTarget*>(this))))
                m_bDropTargetRegistered= sal_True;
            else
                CoLockObjectExternal( static_cast<IDropTarget*>(this), FALSE, FALSE);

            OSL_ASSERT( m_bDropTargetRegistered);
    }
    else
    {
        if( m_bDropTargetRegistered)
        {
            HRESULT hr1= RevokeDragDrop(m_hWnd);
            OSL_ASSERT(SUCCEEDED(hr1));
            HRESULT hr2= CoLockObjectExternal( static_cast<IDropTarget*>(this), FALSE, FALSE);
            OSL_ASSERT(SUCCEEDED(hr2));
            m_bDropTargetRegistered= sal_False;
        }
    }
}

sal_Int8 SAL_CALL DropTarget::getDefaultActions(  ) throw(RuntimeException)
{
    return m_nDefaultActions;
}

void SAL_CALL DropTarget::setDefaultActions( sal_Int8 actions ) throw(RuntimeException)
{
    OSL_ENSURE( actions < 8, "No valid default actions");
    m_nDefaultActions= actions;
}


//IDropTarget
HRESULT STDMETHODCALLTYPE DropTarget::QueryInterface( REFIID riid, void  **ppvObject)
{
    if( !ppvObject)
        return E_POINTER;
    *ppvObject= NULL;

    if( IsEqualGUID( riid, __uuidof( IUnknown)))
        *ppvObject= static_cast<IUnknown*>( this);
    else if ( IsEqualGUID( riid, __uuidof( IDropTarget)))
        *ppvObject= static_cast<IDropTarget*>( this);

    if(*ppvObject)
    {
        AddRef();
        return S_OK;
    }
    else
        return E_NOINTERFACE;

}

ULONG STDMETHODCALLTYPE DropTarget::AddRef( void)
{
    acquire();
    return (ULONG) m_refCount;
}

ULONG STDMETHODCALLTYPE DropTarget::Release( void)
{
    release();
    return (ULONG) m_refCount;
}

// Per default COPY is allowed as long as as listener does not object.
STDMETHODIMP DropTarget::DragEnter( IDataObject __RPC_FAR *pDataObj,
                                    DWORD grfKeyState,
                                    POINTL pt,
                                    DWORD  *pdwEffect)
{
    // Dont consider the allowed source action at this point. Because if pdwEffect is COPY
    // and grfKeyState is only MK_LBUTTON ( default is ACTION_MOVE ) than we  give the listener the chance to change
    // the action to COPY or something. Otherwise the source would display a NONE from the beginning.
    m_nListenerDropAction= getFilteredActions( grfKeyState);
    m_userAction= m_nListenerDropAction;
    if( m_nListenerDropAction != ACTION_NONE)
    {
        DropTargetDragEvent e;
        e.DropAction= m_nListenerDropAction;
        e.Source= Reference<XInterface>( static_cast<XDropTarget*>(this),UNO_QUERY);
        e.Context= static_cast<XDropTargetDragContext*>( new TargetDragContext( static_cast<DropTarget*>(this),
                                                         m_currentEventId ));
        POINT point={ pt.x, pt.y};
        ScreenToClient( m_hWnd, &point);
        e.Location.X= point.x;
        e.Location.Y= point.y;
        e.SourceActions= dndOleDropEffectsToActions( *pdwEffect);

        m_currentData= static_cast<XTransferable*>( new DNDTransferable( pDataObj) );

        // The Event contains a XDropTargetDragContext Implementation. A listener is only allowd to call
        // on it in the same thread and in event handler where it receives that event.
        // m_currentEventId is used  to verify the callback's validity.
        fire_dragEnter( e);
        m_currentEventId++;

        // Check if the action derived from grfKeyState (m_nListenerDropAction) or the action set
        // by the listener (m_nListenerDropAction) is allowed by the source. Only a allowed action is set
        // in pdwEffect.
        // If there is more then one valid action which is the case when ALT or RIGHT MOUSE BUTTON is pressed
        // then getDropEffect returns DROPEFFECT_MOVE which is the default value if no other modifier is pressed.
        // On drop the target should present the user a dialog from which the user may change the action.
        sal_Int8 allowedActions= dndOleDropEffectsToActions( *pdwEffect);
        *pdwEffect= dndActionsToSingleDropEffect( m_nListenerDropAction & allowedActions);
    }
    else
    {
        *pdwEffect= DROPEFFECT_NONE;
    }
    return S_OK;
}

STDMETHODIMP DropTarget::DragOver( DWORD grfKeyState,
                                   POINTL pt,
                                   DWORD  *pdwEffect)
{
    // A listener can change this value during fire_dragOver
    m_nListenerDropAction= getFilteredActions( grfKeyState);

    if( m_nListenerDropAction)
    {
        DropTargetDragEvent e;
        e.DropAction= m_nListenerDropAction;
        e.Source= Reference<XInterface>(static_cast<XDropTarget*>(this),UNO_QUERY);
        e.Context= static_cast<XDropTargetDragContext*>( new TargetDragContext( static_cast<DropTarget*>(this),
                                                         m_currentEventId));
        POINT point={ pt.x, pt.y};
        ScreenToClient( m_hWnd, &point);
        e.Location.X= point.x;
        e.Location.Y= point.y;
        e.SourceActions= dndOleDropEffectsToActions( *pdwEffect);

        // if grfKeyState has changed since the last DragOver then fire events.
        // A listener might change m_nListenerDropAction by calling the
        // XDropTargetDragContext::acceptDrag function. But this is not important
        // because in the afterwards fired dragOver event the action reflects
        // grgKeyState again.
        sal_Int8 tmpAction= m_nListenerDropAction;
        if( m_nListenerDropAction != m_userAction)
            fire_dropActionChanged( e);
        m_nListenerDropAction= tmpAction;

        // The Event contains a XDropTargetDragContext Implementation. A listener is only allowd to call
        // on it in the same thread and in event handler where it receives that envent.
        // m_currentEventId is used  to verify the callback's validity.
        fire_dragOver( e);
        m_currentEventId++;
        // Check if the action derived from grfKeyState (m_nListenerDropAction) or the action set
        // by the listener (m_nListenerDropAction) is allowed by the source. Only a allowed action is set
        // in pdwEffect.
        // If there is more then one valid action which is the case when ALT or RIGHT MOUSE BUTTON is pressed
        // then getDropEffect returns DROPEFFECT_MOVE which is the default value if no other modifier is pressed.
        // On drop the target should present the user a dialog from which the user may change the action.
        sal_Int8 allowedActions= dndOleDropEffectsToActions( *pdwEffect);
        *pdwEffect= dndActionsToSingleDropEffect( m_nListenerDropAction & allowedActions);
    }
    else
    {
        *pdwEffect= DROPEFFECT_NONE;
    }
    return S_OK;
}

STDMETHODIMP DropTarget::DragLeave( void)
{
    m_currentData=0;
    if( m_nDefaultActions != ACTION_NONE)
    {
        DropTargetEvent e;
        e.Source=  static_cast<XDropTarget*>(this);

        fire_dragExit( e);
    }
    return S_OK;
}

STDMETHODIMP DropTarget::Drop( IDataObject  *pDataObj,
                   DWORD grfKeyState,
                   POINTL pt,
                   DWORD __RPC_FAR *pdwEffect)
{
    m_bDropComplete= sal_False;

    m_nListenerDropAction= getFilteredActions( grfKeyState);

    if( m_nListenerDropAction)
    {
        DropTargetDropEvent e;
        e.DropAction= m_nListenerDropAction;
        e.Source= Reference<XInterface>( static_cast<XDropTarget*>(this), UNO_QUERY);
        e.Context= static_cast<XDropTargetDropContext*>( new TargetDropContext( static_cast<DropTarget*>(this ),  m_currentEventId)  );
        POINT point={ pt.x, pt.y};
        ScreenToClient( m_hWnd, &point);
        e.Location.X= point.x;
        e.Location.Y= point.y;
        e.SourceActions= dndOleDropEffectsToActions( *pdwEffect);
        e.Transferable= m_currentData;
        fire_drop( e);
        m_currentEventId++;

        //if fire_drop returns than a listener might have modified m_nListenerDropAction
        // XDropTargetDropContext::dropComplete or the other functions
        if( m_bDropComplete == sal_True)
        {
            sal_Int8 allowedActions= dndOleDropEffectsToActions( *pdwEffect);
            *pdwEffect= dndActionsToSingleDropEffect( m_nListenerDropAction & allowedActions);
        }
        else
            *pdwEffect= DROPEFFECT_NONE;
    }
    else
        *pdwEffect= DROPEFFECT_NONE;

    m_currentData= 0;
    return S_OK;
}



void DropTarget::fire_drop( const DropTargetDropEvent& dte)
{
    OInterfaceContainerHelper* pContainer= rBHelper.getContainer( getCppuType( (Reference<XDropTargetListener>* )0 ) );
    if( pContainer)
    {
        OInterfaceIteratorHelper iter( *pContainer);
        while( iter.hasMoreElements())
        {
            Reference<XDropTargetListener> listener( static_cast<XDropTargetListener*>( iter.next()));
            listener->drop( dte);
        }
    }
}

void DropTarget::fire_dragEnter( const DropTargetDragEvent& dtde )
{
    OInterfaceContainerHelper* pContainer= rBHelper.getContainer( getCppuType( (Reference<XDropTargetListener>* )0 ) );
    if( pContainer)
    {
        OInterfaceIteratorHelper iter( *pContainer);
        while( iter.hasMoreElements())
        {
            Reference<XDropTargetListener> listener( static_cast<XDropTargetListener*>( iter.next()));
            listener->dragEnter( dtde);
        }
    }
}

void DropTarget::fire_dragExit( const DropTargetEvent& dte )
{
    OInterfaceContainerHelper* pContainer= rBHelper.getContainer( getCppuType( (Reference<XDropTargetListener>* )0 ) );

    if( pContainer)
    {
        OInterfaceIteratorHelper iter( *pContainer);
        while( iter.hasMoreElements())
        {
            Reference<XDropTargetListener> listener( static_cast<XDropTargetListener*>( iter.next()));
            listener->dragExit( dte);
        }
    }
}

void DropTarget::fire_dragOver( const DropTargetDragEvent& dtde )
{
    OInterfaceContainerHelper* pContainer= rBHelper.getContainer( getCppuType( (Reference<XDropTargetListener>* )0 ) );
    if( pContainer)
    {
        OInterfaceIteratorHelper iter( *pContainer );
        while( iter.hasMoreElements())
        {
            Reference<XDropTargetListener> listener( static_cast<XDropTargetListener*>( iter.next()));
            listener->dragOver( dtde);
        }
    }
}

void DropTarget::fire_dropActionChanged( const DropTargetDragEvent& dtde )
{
    OInterfaceContainerHelper* pContainer= rBHelper.getContainer( getCppuType( (Reference<XDropTargetListener>* )0 ) );
    if( pContainer)
    {
        OInterfaceIteratorHelper iter( *pContainer);
        while( iter.hasMoreElements())
        {
            Reference<XDropTargetListener> listener( static_cast<XDropTargetListener*>( iter.next()));
            listener->dropActionChanged( dtde);
        }
    }
}

// Non - interface functions ============================================================
// DropTarget fires events to XDropTargetListeners. The event object contains an
// XDropTargetDropContext implementaion. When the listener calls on that interface
// then the calls are delegated from DropContext (XDropTargetDropContext) to these
// functions.
// Only one listener which visible area is affected is allowed to call on
// XDropTargetDropContext
// Returning sal_False would cause the XDropTargetDropContext or ..DragContext implementation
// to throw an InvalidDNDOperationException, meaning that a Drag is not currently performed.
// return sal_False results in throwing a InvalidDNDOperationException in the caller.

void DropTarget::_acceptDrop(sal_Int8 dropOperation, sal_uInt32 id)
         throw(InvalidDNDOperationException)
{
    if( m_currentEventId == id)
    {
        m_nListenerDropAction= dropOperation;
    }
    else
        throw InvalidDNDOperationException();
}

void DropTarget::_rejectDrop(sal_uInt32 id)
          throw(InvalidDNDOperationException)
{
    if( m_currentEventId == id)
    {
        m_nListenerDropAction= ACTION_NONE;
    }
    else
        throw InvalidDNDOperationException();
}

void DropTarget::_dropComplete(sal_Bool success, sal_uInt32 id)
         throw(InvalidDNDOperationException)
{
    if( m_currentEventId == id)
    {
        m_bDropComplete= success;
    }
    else
        throw InvalidDNDOperationException();
}
// --------------------------------------------------------------------------------------
// DropTarget fires events to XDropTargetListeners. The event object can contains an
// XDropTargetDagContext implementaion. When the listener calls on that interface
// then the calls are delegated from DragContext (XDropTargetDragContext) to these
// functions.
// Only one listener which visible area is affected is allowed to call on
// XDropTargetDragContext
void DropTarget::_acceptDrag( sal_Int8 dragOperation, sal_uInt32 id)
         throw(InvalidDNDOperationException)
{
    if( m_currentEventId == id)
    {
        m_nListenerDropAction= dragOperation;
    }
    else
        throw InvalidDNDOperationException();
}

void DropTarget::_rejectDrag( sal_uInt32 id)
    throw(InvalidDNDOperationException)
{
    if( m_currentEventId == id)
    {
        m_nListenerDropAction= ACTION_NONE;
    }
    else
        throw InvalidDNDOperationException();
}

Sequence<DataFlavor> DropTarget::_getCurrentDataFlavors( sal_uInt32 id)
         throw( InvalidDNDOperationException)
{
    Sequence<DataFlavor> retVal;
    if( m_currentEventId == id)
    {
        if( m_currentData.is())
            retVal= m_currentData->getTransferDataFlavors();
    }
    else
        throw InvalidDNDOperationException();
    return retVal;
}

sal_Bool DropTarget::_isDataFlavorSupported( const DataFlavor& df, sal_uInt32 id)
         throw(InvalidDNDOperationException)
{
    sal_Bool ret= sal_False;
    if( m_currentEventId == id)
    {
        if( m_currentData.is())
            ret= m_currentData->isDataFlavorSupported( df);
    }
    else
        throw InvalidDNDOperationException();
    return ret;
}
//--------------------------------------------------------------------------------------


// This function determines the action dependend on the pressed
// key modifiers ( CTRL, SHIFT, ALT, Right Mouse Button). The result
// is then checked against the allowed actions which can be set through
// XDropTarget::setDefaultActions. Only those values which are also
// default actions are returned. If setDefaultActions has not been called
// beforehand the the default actions comprise all possible actions.
// params: grfKeyState - the modifier keys and mouse buttons currently pressed
inline sal_Int8 DropTarget::getFilteredActions( DWORD grfKeyState)
{
//  sal_Int8 actions= getDropAction( grfKeyState);
    sal_Int8 actions= dndOleKeysToAction( grfKeyState);
    return actions &  m_nDefaultActions;
}


