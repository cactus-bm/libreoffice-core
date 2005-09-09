/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: listenermultiplexer.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 13:22:15 $
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

#include <toolkit/helper/listenermultiplexer.hxx>

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif

//  ----------------------------------------------------
//  class ListenerMultiplexerBase
//  ----------------------------------------------------
ListenerMultiplexerBase::ListenerMultiplexerBase( ::cppu::OWeakObject& rSource )
    : ::cppu::OInterfaceContainerHelper( GetMutex() ), mrContext( rSource )
{
}

// ::com::sun::star::uno::XInterface
::com::sun::star::uno::Any ListenerMultiplexerBase::queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException)
{
    return ::cppu::queryInterface( rType, SAL_STATIC_CAST( ::com::sun::star::uno::XInterface*, this ) );
}


//  ----------------------------------------------------
//  class EventListenerMultiplexer
//  ----------------------------------------------------
EventListenerMultiplexer::EventListenerMultiplexer( ::cppu::OWeakObject& rSource )
    : ListenerMultiplexerBase( rSource )
{
}

// ::com::sun::star::uno::XInterface
::com::sun::star::uno::Any EventListenerMultiplexer::queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException)
{
    ::com::sun::star::uno::Any aRet = ::cppu::queryInterface( rType,
                                        SAL_STATIC_CAST( ::com::sun::star::lang::XEventListener*, this ) );
    return (aRet.hasValue() ? aRet : ListenerMultiplexerBase::queryInterface( rType ));
}

// ::com::sun::star::lang::XEventListener
void EventListenerMultiplexer::disposing( const ::com::sun::star::lang::EventObject& ) throw(::com::sun::star::uno::RuntimeException)
{
}

//  ----------------------------------------------------
//  class FocusListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( FocusListenerMultiplexer, ::com::sun::star::awt::XFocusListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( FocusListenerMultiplexer, ::com::sun::star::awt::XFocusListener, focusGained, ::com::sun::star::awt::FocusEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( FocusListenerMultiplexer, ::com::sun::star::awt::XFocusListener, focusLost, ::com::sun::star::awt::FocusEvent )

//  ----------------------------------------------------
//  class WindowListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( WindowListenerMultiplexer, ::com::sun::star::awt::XWindowListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( WindowListenerMultiplexer, ::com::sun::star::awt::XWindowListener, windowResized, ::com::sun::star::awt::WindowEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( WindowListenerMultiplexer, ::com::sun::star::awt::XWindowListener, windowMoved, ::com::sun::star::awt::WindowEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( WindowListenerMultiplexer, ::com::sun::star::awt::XWindowListener, windowShown, ::com::sun::star::lang::EventObject )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( WindowListenerMultiplexer, ::com::sun::star::awt::XWindowListener, windowHidden, ::com::sun::star::lang::EventObject )

//  ----------------------------------------------------
//  class VclContainerListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( VclContainerListenerMultiplexer, ::com::sun::star::awt::XVclContainerListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( VclContainerListenerMultiplexer, ::com::sun::star::awt::XVclContainerListener, windowAdded, ::com::sun::star::awt::VclContainerEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( VclContainerListenerMultiplexer, ::com::sun::star::awt::XVclContainerListener, windowRemoved, ::com::sun::star::awt::VclContainerEvent )

//  ----------------------------------------------------
//  class KeyListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( KeyListenerMultiplexer, ::com::sun::star::awt::XKeyListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( KeyListenerMultiplexer, ::com::sun::star::awt::XKeyListener, keyPressed, ::com::sun::star::awt::KeyEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( KeyListenerMultiplexer, ::com::sun::star::awt::XKeyListener, keyReleased, ::com::sun::star::awt::KeyEvent )

//  ----------------------------------------------------
//  class MouseListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( MouseListenerMultiplexer, ::com::sun::star::awt::XMouseListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MouseListenerMultiplexer, ::com::sun::star::awt::XMouseListener, mousePressed, ::com::sun::star::awt::MouseEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MouseListenerMultiplexer, ::com::sun::star::awt::XMouseListener, mouseReleased, ::com::sun::star::awt::MouseEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MouseListenerMultiplexer, ::com::sun::star::awt::XMouseListener, mouseEntered, ::com::sun::star::awt::MouseEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MouseListenerMultiplexer, ::com::sun::star::awt::XMouseListener, mouseExited, ::com::sun::star::awt::MouseEvent )

//  ----------------------------------------------------
//  class MouseMotionListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( MouseMotionListenerMultiplexer, ::com::sun::star::awt::XMouseMotionListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MouseMotionListenerMultiplexer, ::com::sun::star::awt::XMouseMotionListener, mouseDragged, ::com::sun::star::awt::MouseEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MouseMotionListenerMultiplexer, ::com::sun::star::awt::XMouseMotionListener, mouseMoved, ::com::sun::star::awt::MouseEvent )

//  ----------------------------------------------------
//  class PaintListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( PaintListenerMultiplexer, ::com::sun::star::awt::XPaintListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( PaintListenerMultiplexer, ::com::sun::star::awt::XPaintListener, windowPaint, ::com::sun::star::awt::PaintEvent )

//  ----------------------------------------------------
//  class TopWindowListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener, windowOpened, ::com::sun::star::lang::EventObject )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener, windowClosing, ::com::sun::star::lang::EventObject )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener, windowClosed, ::com::sun::star::lang::EventObject )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener, windowMinimized, ::com::sun::star::lang::EventObject )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener, windowNormalized, ::com::sun::star::lang::EventObject )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener, windowActivated, ::com::sun::star::lang::EventObject )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TopWindowListenerMultiplexer, ::com::sun::star::awt::XTopWindowListener, windowDeactivated, ::com::sun::star::lang::EventObject )

//  ----------------------------------------------------
//  class TextListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( TextListenerMultiplexer, ::com::sun::star::awt::XTextListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( TextListenerMultiplexer, ::com::sun::star::awt::XTextListener, textChanged, ::com::sun::star::awt::TextEvent )

//  ----------------------------------------------------
//  class ActionListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( ActionListenerMultiplexer, ::com::sun::star::awt::XActionListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( ActionListenerMultiplexer, ::com::sun::star::awt::XActionListener, actionPerformed, ::com::sun::star::awt::ActionEvent )

//  ----------------------------------------------------
//  class ItemListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( ItemListenerMultiplexer, ::com::sun::star::awt::XItemListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( ItemListenerMultiplexer, ::com::sun::star::awt::XItemListener, itemStateChanged, ::com::sun::star::awt::ItemEvent )

//  ----------------------------------------------------
//  class ContainerListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( ContainerListenerMultiplexer, ::com::sun::star::container::XContainerListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( ContainerListenerMultiplexer, ::com::sun::star::container::XContainerListener, elementInserted, ::com::sun::star::container::ContainerEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( ContainerListenerMultiplexer, ::com::sun::star::container::XContainerListener, elementRemoved, ::com::sun::star::container::ContainerEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( ContainerListenerMultiplexer, ::com::sun::star::container::XContainerListener, elementReplaced, ::com::sun::star::container::ContainerEvent )

//  ----------------------------------------------------
//  class SpinListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( SpinListenerMultiplexer, ::com::sun::star::awt::XSpinListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( SpinListenerMultiplexer, ::com::sun::star::awt::XSpinListener, up, ::com::sun::star::awt::SpinEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( SpinListenerMultiplexer, ::com::sun::star::awt::XSpinListener, down, ::com::sun::star::awt::SpinEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( SpinListenerMultiplexer, ::com::sun::star::awt::XSpinListener, first, ::com::sun::star::awt::SpinEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( SpinListenerMultiplexer, ::com::sun::star::awt::XSpinListener, last, ::com::sun::star::awt::SpinEvent )

//  ----------------------------------------------------
//  class AdjustmentListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( AdjustmentListenerMultiplexer, ::com::sun::star::awt::XAdjustmentListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( AdjustmentListenerMultiplexer, ::com::sun::star::awt::XAdjustmentListener, adjustmentValueChanged, ::com::sun::star::awt::AdjustmentEvent )

//  ----------------------------------------------------
//  class MenuListenerMultiplexer
//  ----------------------------------------------------
IMPL_LISTENERMULTIPLEXER_BASEMETHODS( MenuListenerMultiplexer, ::com::sun::star::awt::XMenuListener )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MenuListenerMultiplexer, ::com::sun::star::awt::XMenuListener, highlight, ::com::sun::star::awt::MenuEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MenuListenerMultiplexer, ::com::sun::star::awt::XMenuListener, select, ::com::sun::star::awt::MenuEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MenuListenerMultiplexer, ::com::sun::star::awt::XMenuListener, activate, ::com::sun::star::awt::MenuEvent )
IMPL_LISTENERMULTIPLEXER_LISTENERMETHOD( MenuListenerMultiplexer, ::com::sun::star::awt::XMenuListener, deactivate, ::com::sun::star::awt::MenuEvent )



