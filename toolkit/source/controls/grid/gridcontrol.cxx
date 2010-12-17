/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_toolkit.hxx"

#include "gridcontrol.hxx"
#include "grideventforwarder.hxx"

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/view/SelectionType.hpp>
#include <com/sun/star/awt/grid/XGridDataModel.hpp>
#include <com/sun/star/awt/grid/XGridColumnModel.hpp>
#include <com/sun/star/awt/grid/ScrollBarMode.hpp>
#include <toolkit/helper/unopropertyarrayhelper.hxx>
#include <toolkit/helper/property.hxx>
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#include <comphelper/processfactory.hxx>
#include <tools/diagnose_ex.h>

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::awt::grid;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::view;

namespace toolkit
{
//  ----------------------------------------------------
//  class UnoGridModel
//  ----------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
UnoGridModel::UnoGridModel( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& i_factory )
        :UnoControlModel( i_factory )
{
    ImplRegisterProperty( BASEPROPERTY_BACKGROUNDCOLOR );
    ImplRegisterProperty( BASEPROPERTY_BORDER );
    ImplRegisterProperty( BASEPROPERTY_BORDERCOLOR );
    ImplRegisterProperty( BASEPROPERTY_DEFAULTCONTROL );
    ImplRegisterProperty( BASEPROPERTY_ENABLED );
    ImplRegisterProperty( BASEPROPERTY_FILLCOLOR );
    ImplRegisterProperty( BASEPROPERTY_HELPTEXT );
    ImplRegisterProperty( BASEPROPERTY_HELPURL );
    ImplRegisterProperty( BASEPROPERTY_PRINTABLE );
    ImplRegisterProperty( BASEPROPERTY_SIZEABLE ); // resizeable
    ImplRegisterProperty( BASEPROPERTY_HSCROLL );
    ImplRegisterProperty( BASEPROPERTY_VSCROLL );
    ImplRegisterProperty( BASEPROPERTY_TABSTOP );
    ImplRegisterProperty( BASEPROPERTY_GRID_SHOWROWHEADER );
    ImplRegisterProperty( BASEPROPERTY_GRID_SHOWCOLUMNHEADER );
    ImplRegisterProperty( BASEPROPERTY_GRID_DATAMODEL );
    ImplRegisterProperty( BASEPROPERTY_GRID_COLUMNMODEL );
    ImplRegisterProperty( BASEPROPERTY_GRID_SELECTIONMODE );
    ImplRegisterProperty( BASEPROPERTY_FONTRELIEF );
    ImplRegisterProperty( BASEPROPERTY_FONTEMPHASISMARK );
    ImplRegisterProperty( BASEPROPERTY_FONTDESCRIPTOR );
    ImplRegisterProperty( BASEPROPERTY_TEXTCOLOR );
    ImplRegisterProperty( BASEPROPERTY_VERTICALALIGN );
    ImplRegisterProperty( BASEPROPERTY_GRID_EVEN_ROW_BACKGROUND );
    ImplRegisterProperty( BASEPROPERTY_GRID_HEADER_BACKGROUND );
    ImplRegisterProperty( BASEPROPERTY_GRID_LINE_COLOR );
    ImplRegisterProperty( BASEPROPERTY_GRID_ROW_BACKGROUND );

    osl_incrementInterlockedCount( &m_refCount );
    {
        setFastPropertyValue( BASEPROPERTY_GRID_COLUMNMODEL,
            makeAny( maContext.createComponent( "com.sun.star.awt.grid.DefaultGridColumnModel" ) ) );
        setFastPropertyValue( BASEPROPERTY_GRID_DATAMODEL,
            makeAny( maContext.createComponent( "com.sun.star.awt.grid.DefaultGridDataModel" ) ) );
    }
    osl_decrementInterlockedCount( &m_refCount );
}

//----------------------------------------------------------------------------------------------------------------------
UnoGridModel::UnoGridModel( const UnoGridModel& rModel )
: UnoControlModel( rModel )
{
}

//----------------------------------------------------------------------------------------------------------------------
UnoControlModel* UnoGridModel::Clone() const
{
    return new UnoGridModel( *this );
}

//----------------------------------------------------------------------------------------------------------------------
OUString UnoGridModel::getServiceName() throw(RuntimeException)
{
    return OUString::createFromAscii( szServiceName_GridControlModel );
}

//----------------------------------------------------------------------------------------------------------------------
Any UnoGridModel::ImplGetDefaultValue( sal_uInt16 nPropId ) const
{
    switch( nPropId )
    {
        case BASEPROPERTY_DEFAULTCONTROL:
            return uno::makeAny( ::rtl::OUString::createFromAscii( szServiceName_GridControl ) );
        case BASEPROPERTY_GRID_SELECTIONMODE:
            return uno::makeAny( SelectionType(1) );
        case BASEPROPERTY_GRID_SHOWROWHEADER:
            return uno::makeAny( (sal_Bool)sal_True );
        case BASEPROPERTY_GRID_SHOWCOLUMNHEADER:
            return uno::makeAny( (sal_Bool)sal_True );
        case BASEPROPERTY_GRID_DATAMODEL:
            return uno::makeAny( Reference<XGridDataModel> ());
        case BASEPROPERTY_GRID_COLUMNMODEL:
            return uno::makeAny(Reference<XGridColumnModel>() );
        case BASEPROPERTY_GRID_EVEN_ROW_BACKGROUND:
            return uno::makeAny( com::sun::star::util::Color(0xFFFFFF) );
        case BASEPROPERTY_GRID_HEADER_BACKGROUND:
            return uno::makeAny( com::sun::star::util::Color(0xFFFFFF) );
        case BASEPROPERTY_GRID_LINE_COLOR:
            return uno::makeAny( com::sun::star::util::Color(0xFFFFFF) );
        case BASEPROPERTY_GRID_ROW_BACKGROUND:
            return uno::makeAny(com::sun::star::util::Color(0xFFFFFF) );
        default:
            return UnoControlModel::ImplGetDefaultValue( nPropId );
    }

}

//----------------------------------------------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& UnoGridModel::getInfoHelper()
{
    static UnoPropertyArrayHelper* pHelper = NULL;
    if ( !pHelper )
    {
        Sequence<sal_Int32> aIDs = ImplGetPropertyIds();
        pHelper = new UnoPropertyArrayHelper( aIDs );
    }
    return *pHelper;
}

//----------------------------------------------------------------------------------------------------------------------
// XMultiPropertySet
Reference< XPropertySetInfo > UnoGridModel::getPropertySetInfo(  ) throw(RuntimeException)
{
    static Reference< XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}


//======================================================================================================================
//= UnoGridControl
//======================================================================================================================
UnoGridControl::UnoGridControl( const Reference< XMultiServiceFactory >& i_factory )
    :UnoGridControl_Base( i_factory )
    ,mSelectionMode(SelectionType(1))
    ,m_aSelectionListeners( *this )
    ,m_pEventForwarder( new GridEventForwarder( *this ) )
{
}

//----------------------------------------------------------------------------------------------------------------------
UnoGridControl::~UnoGridControl()
{
}

//----------------------------------------------------------------------------------------------------------------------
OUString UnoGridControl::GetComponentServiceName()
{
    return OUString::createFromAscii( "Grid" );
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::dispose(  ) throw(RuntimeException)
{
    lang::EventObject aEvt;
    aEvt.Source = (::cppu::OWeakObject*)this;
    m_aSelectionListeners.disposeAndClear( aEvt );
    UnoControl::dispose();
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::createPeer( const uno::Reference< awt::XToolkit > & rxToolkit, const uno::Reference< awt::XWindowPeer >  & rParentPeer ) throw(uno::RuntimeException)
{
    UnoControlBase::createPeer( rxToolkit, rParentPeer );

    const Reference< XGridControl >  xGrid( getPeer(), UNO_QUERY_THROW );
    xGrid->addSelectionListener(&m_aSelectionListeners);
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{
    void lcl_setEventForwarding( const Reference< XControlModel >& i_gridControlModel, const ::boost::scoped_ptr< GridEventForwarder >& i_listener,
        bool const i_add )
    {
        const Reference< XPropertySet > xModelProps( i_gridControlModel, UNO_QUERY );
        if ( !xModelProps.is() )
            return;

        try
        {
            const Reference< XContainer > xColModel(
                xModelProps->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ColumnModel" ) ) ),
                UNO_QUERY_THROW );
            if ( i_add )
                xColModel->addContainerListener( i_listener.get() );
            else
                xColModel->removeContainerListener( i_listener.get() );

            const Reference< XGridDataModel > xDataModel(
                xModelProps->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "GridDataModel" ) ) ),
                UNO_QUERY_THROW
            );
            if ( i_add )
                xDataModel->addDataListener( i_listener.get() );
            else
                xDataModel->removeDataListener( i_listener.get() );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
sal_Bool SAL_CALL UnoGridControl::setModel( const Reference< XControlModel >& i_model ) throw(RuntimeException)
{
    lcl_setEventForwarding( getModel(), m_pEventForwarder, false );
    if ( !UnoGridControl_Base::setModel( i_model ) )
        return sal_False;
    lcl_setEventForwarding( getModel(), m_pEventForwarder, true );
    return sal_True;
}

//----------------------------------------------------------------------------------------------------------------------
::sal_Int32 UnoGridControl::getItemIndexAtPoint(::sal_Int32 x, ::sal_Int32 y) throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XGridControl > xGrid ( getPeer(), UNO_QUERY_THROW );
    return xGrid->getItemIndexAtPoint( x, y );
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::setToolTip(const ::com::sun::star::uno::Sequence< ::rtl::OUString >& text, const ::com::sun::star::uno::Sequence< ::sal_Int32 >& columns) throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->setToolTip( text, columns );
}

//----------------------------------------------------------------------------------------------------------------------
::sal_Int32 SAL_CALL UnoGridControl::getMinSelectionIndex() throw (::com::sun::star::uno::RuntimeException)
{
    return Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->getMinSelectionIndex();
}

//----------------------------------------------------------------------------------------------------------------------
::sal_Int32 SAL_CALL UnoGridControl::getMaxSelectionIndex() throw (::com::sun::star::uno::RuntimeException)
{
    return Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->getMaxSelectionIndex();
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::selectRows(const ::com::sun::star::uno::Sequence< ::sal_Int32 >& rangeOfRows) throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->selectRows( rangeOfRows);
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::selectAllRows() throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->selectAllRows();
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::deselectRows(const ::com::sun::star::uno::Sequence< ::sal_Int32 >& rangeOfRows) throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->deselectRows( rangeOfRows);
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::deselectAllRows() throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->deselectAllRows();
}

//----------------------------------------------------------------------------------------------------------------------
::com::sun::star::uno::Sequence< ::sal_Int32 > SAL_CALL UnoGridControl::getSelection() throw (::com::sun::star::uno::RuntimeException)
{
    return Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->getSelection();
}

//----------------------------------------------------------------------------------------------------------------------
::sal_Bool SAL_CALL UnoGridControl::isSelectionEmpty() throw (::com::sun::star::uno::RuntimeException)
{
    return Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->isSelectionEmpty();
}

//----------------------------------------------------------------------------------------------------------------------
::sal_Bool SAL_CALL UnoGridControl::isSelectedIndex(::sal_Int32 index) throw (::com::sun::star::uno::RuntimeException)
{
    return Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->isSelectedIndex( index );
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::selectRow(::sal_Int32 y) throw (::com::sun::star::uno::RuntimeException)
{
    Reference< XGridControl >( getPeer(), UNO_QUERY_THROW )->selectRow( y );
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::addSelectionListener(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridSelectionListener > & listener) throw (::com::sun::star::uno::RuntimeException)
{
    m_aSelectionListeners.addInterface( listener );
}

//----------------------------------------------------------------------------------------------------------------------
void SAL_CALL UnoGridControl::removeSelectionListener(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::grid::XGridSelectionListener > & listener) throw (::com::sun::star::uno::RuntimeException)
{
    m_aSelectionListeners.removeInterface( listener );
}

}//namespace toolkit

Reference< XInterface > SAL_CALL GridControl_CreateInstance( const Reference< XMultiServiceFactory >& i_factory )
{
    return Reference < XInterface >( ( ::cppu::OWeakObject* ) new ::toolkit::UnoGridControl( i_factory ) );
}

Reference< XInterface > SAL_CALL GridControlModel_CreateInstance( const Reference< XMultiServiceFactory >& i_factory )
{
    return Reference < XInterface >( ( ::cppu::OWeakObject* ) new ::toolkit::UnoGridModel( i_factory ) );
}
