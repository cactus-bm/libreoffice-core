/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SVTXGridControl.hxx,v $
 * $Revision: 1.32 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _SVT_GRIDCONTROL_HXX_
#define _SVT_GRIDCONTROL_HXX_

#include <unocontroltablemodel.hxx>
#include <svtools/table/tablecontrol.hxx>
#include <com/sun/star/awt/grid/XGridControl.hpp>
#include <com/sun/star/awt/grid/XGridDataListener.hpp>
#include <com/sun/star/awt/grid/GridDataEvent.hpp>
#include <com/sun/star/awt/grid/XGridColumnModel.hpp>
#include <com/sun/star/awt/grid/XGridDataModel.hpp>
#include <com/sun/star/awt/grid/XGridSelectionListener.hpp>
#include <toolkit/awt/vclxwindow.hxx>
#include <toolkit/awt/vclxwindows.hxx>
#include <cppuhelper/typeprovider.hxx>
#include <cppuhelper/implbase2.hxx>
//#include <toolkit/helper/listenermultiplexer.hxx>


using namespace ::svt::table;

class SVTXGridControl : public ::cppu::ImplInheritanceHelper2< VCLXWindow, ::com::sun::star::awt::grid::XGridControl,
                                 ::com::sun::star::awt::grid::XGridDataListener>
{
private:
    UnoControlTableModel* m_pTableModel;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridDataModel >m_xDataModel;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridColumnModel >m_xColumnModel;
    bool m_bHasColumnHeaders;
    bool m_bHasRowHeaders;
    bool m_bVScroll;
    bool m_bHScroll;

public:
    SVTXGridControl();
    ~SVTXGridControl();
    //XGridDataListener overridables
    virtual void SAL_CALL rowAdded(const ::com::sun::star::awt::grid::GridDataEvent& Event) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL rowRemoved(const ::com::sun::star::awt::grid::GridDataEvent & Event) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL dataChanged(const ::com::sun::star::awt::grid::GridDataEvent & Event) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw(::com::sun::star::uno::RuntimeException);

    ::com::sun::star::uno::Any                  SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
    void                                        SAL_CALL acquire() throw()  { VCLXWindow::acquire(); }
    void                                        SAL_CALL release() throw()  { VCLXWindow::release(); }

    // ::com::sun::star::lang::XTypeProvider
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >  SAL_CALL getTypes() throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< sal_Int8 >                     SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);

    //::com::sun::star::awt::grid::XGridControl
    virtual ::sal_Int32 SAL_CALL getMinSelectionIndex() throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getMaxSelectionIndex() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL insertIndexIntervall(::sal_Int32 start, ::sal_Int32 length) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeIndexIntervall(::sal_Int32 start, ::sal_Int32 end) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::sal_Int32 > SAL_CALL getSelection() throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL isCellEditable() throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL isSelectionEmpty() throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL isSelectedIndex(::sal_Int32 index) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL selectRow(::sal_Int32 y) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL selectColumn(::sal_Int32 x) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addSelectionListener(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridSelectionListener > & listener) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeSelectionListener(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridSelectionListener > & listener) throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getItemIndexAtPoint(::sal_Int32 x, ::sal_Int32 y) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setToolTip(const ::com::sun::star::uno::Sequence< ::rtl::OUString >& text, const ::com::sun::star::uno::Sequence< sal_Int32 >& columns) throw (::com::sun::star::uno::RuntimeException);

    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::uno::RuntimeException);
    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds );
    void SAL_CALL setVisible(sal_Bool bVisible) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setFocus() throw(::com::sun::star::uno::RuntimeException);
    ::rtl::OUString impl_convertToString(::com::sun::star::uno::Any _value);
    };
 #endif // _SVT_GRIDCONTROL_HXX_
