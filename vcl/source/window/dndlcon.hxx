/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dndlcon.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 20:30:14 $
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

#ifndef _DNDLCON_HXX_
#define _DNDLCON_HXX_

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDRAGGESTURERECOGNIZER_HPP_
#include <com/sun/star/datatransfer/dnd/XDragGestureRecognizer.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDRAGSOURCE_HPP_
#include <com/sun/star/datatransfer/dnd/XDragSource.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDROPTARGET_HPP_
#include <com/sun/star/datatransfer/dnd/XDropTarget.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDROPTARGETDRAGCONTEXT_HPP_
#include <com/sun/star/datatransfer/dnd/XDropTargetDragContext.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDROPTARGETDROPCONTEXT_HPP_
#include <com/sun/star/datatransfer/dnd/XDropTargetDropContext.hpp>
#endif

#ifndef _CPPUHELPER_COMPBASE4_HXX_
#include <cppuhelper/compbase4.hxx>
#endif

#include <vcl/unohelp2.hxx>

class DNDListenerContainer :    public ::vcl::unohelper::MutexHelper,
                                public ::cppu::WeakComponentImplHelper4<
    ::com::sun::star::datatransfer::dnd::XDragGestureRecognizer, \
    ::com::sun::star::datatransfer::dnd::XDropTargetDragContext,
    ::com::sun::star::datatransfer::dnd::XDropTargetDropContext,
    ::com::sun::star::datatransfer::dnd::XDropTarget >
{
    sal_Bool m_bActive;
    sal_Int8 m_nDefaultActions;

    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDragContext > m_xDropTargetDragContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDropContext > m_xDropTargetDropContext;

public:

    DNDListenerContainer( sal_Int8 nDefaultActions );
    virtual ~DNDListenerContainer();

    sal_uInt32 fireDropEvent(
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDropContext >& context,
        sal_Int8 dropAction, sal_Int32 locationX, sal_Int32 locationY, sal_Int8 sourceActions,
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& transferable );

    sal_uInt32 fireDragExitEvent();

    sal_uInt32 fireDragOverEvent(
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDragContext >& context,
        sal_Int8 dropAction, sal_Int32 locationX, sal_Int32 locationY, sal_Int8 sourceActions );

    sal_uInt32 fireDragEnterEvent(
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDragContext >& context,
        sal_Int8 dropAction, sal_Int32 locationX, sal_Int32 locationY, sal_Int8 sourceActions,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::datatransfer::DataFlavor >& dataFlavor );

    sal_uInt32 fireDropActionChangedEvent(
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetDragContext >& context,
        sal_Int8 dropAction, sal_Int32 locationX, sal_Int32 locationY, sal_Int8 sourceActions );

    sal_uInt32 fireDragGestureEvent(
        sal_Int8 dragAction, sal_Int32 dragOriginX, sal_Int32 dragOriginY,
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDragSource >& dragSource,
        const ::com::sun::star::uno::Any& triggerEvent );

    /*
     * XDragGestureRecognizer
     */

    virtual void SAL_CALL addDragGestureListener( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDragGestureListener >& dgl ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeDragGestureListener( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDragGestureListener >& dgl ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL resetRecognizer(  ) throw(::com::sun::star::uno::RuntimeException);

       /*
     * XDropTargetDragContext
     */

    virtual void SAL_CALL acceptDrag( sal_Int8 dragOperation ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL rejectDrag(  ) throw (::com::sun::star::uno::RuntimeException);


       /*
     * XDropTargetDropContext
     */

    virtual void SAL_CALL acceptDrop( sal_Int8 dropOperation ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL rejectDrop(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL dropComplete( sal_Bool success ) throw (::com::sun::star::uno::RuntimeException);

    /*
     * XDropTarget
     */

    virtual void SAL_CALL addDropTargetListener( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetListener >& dtl ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeDropTargetListener( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTargetListener >& dtl ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isActive(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setActive( sal_Bool active ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Int8 SAL_CALL getDefaultActions(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setDefaultActions( sal_Int8 actions ) throw(::com::sun::star::uno::RuntimeException);
};


//==================================================================================================
//
//==================================================================================================

#endif
