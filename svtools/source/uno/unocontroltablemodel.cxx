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
#include "precompiled_svtools.hxx"

#include "unocontroltablemodel.hxx"
#include "unogridcolumnfacade.hxx"

#include "svtools/table/defaultinputhandler.hxx"
#include "svtools/table/gridtablerenderer.hxx"
#include "svtools/table/tablecontrol.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/awt/grid/XGridColumn.hpp>
#include <com/sun/star/view/SelectionType.hpp>
#include <com/sun/star/awt/grid/XGridColumnListener.hpp>
/** === end UNO includes === **/

#include <comphelper/stlunosequence.hxx>
#include <cppuhelper/weakref.hxx>
#include <tools/debug.hxx>
#include <tools/diagnose_ex.h>
#include <vcl/svapp.hxx>
#include <vos/mutex.hxx>

// .....................................................................................................................
namespace svt { namespace table
{
// .....................................................................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::UNO_QUERY;
    using ::com::sun::star::awt::grid::XGridColumn;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::awt::grid::XGridColumnListener;
    using ::com::sun::star::lang::EventObject;
    using ::com::sun::star::awt::grid::GridColumnEvent;
    using ::com::sun::star::awt::grid::XGridDataModel;
    using ::com::sun::star::awt::grid::XGridColumnModel;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::style::HorizontalAlignment_LEFT;
    using ::com::sun::star::style::HorizontalAlignment;
    using ::com::sun::star::uno::WeakReference;
    using ::com::sun::star::awt::grid::GridDataEvent;
    /** === end UNO using === **/

    //==================================================================================================================
    //= UnoControlTableModel_Impl
    //==================================================================================================================
    typedef ::std::vector< PTableModelListener >    ModellListeners;
    typedef ::std::vector< PColumnModel >           ColumnModels;
    struct UnoControlTableModel_Impl
    {
        ColumnModels                                    aColumns;
        bool                                            bHasColumnHeaders;
        bool                                            bHasRowHeaders;
        ScrollbarVisibility                             eVScrollMode;
        ScrollbarVisibility                             eHScrollMode;
        PTableRenderer                                  pRenderer;
        PTableInputHandler                              pInputHandler;
        TableMetrics                                    nRowHeight;
        TableMetrics                                    nColumnHeaderHeight;
        TableMetrics                                    nRowHeaderWidth;
        ::boost::optional< ::Color >                    m_aGridLineColor;
        ::boost::optional< ::Color >                    m_aHeaderBackgroundColor;
        ::boost::optional< ::Color >                    m_aHeaderTextColor;
        ::boost::optional< ::Color >                    m_aTextColor;
        ::boost::optional< ::Color >                    m_aTextLineColor;
        ::boost::optional< ::std::vector< ::Color > >   m_aRowColors;
        ::com::sun::star::style::VerticalAlignment      m_eVerticalAlign;
        ModellListeners                                 m_aListeners;
        WeakReference< XGridDataModel >                 m_aDataModel;
        WeakReference< XGridColumnModel >               m_aColumnModel;

        UnoControlTableModel_Impl()
            :aColumns                   ( )
            ,bHasColumnHeaders          ( true      )
            ,bHasRowHeaders             ( false     )
            ,eVScrollMode               ( ScrollbarShowNever )
            ,eHScrollMode               ( ScrollbarShowNever )
            ,pRenderer                  ( )
            ,pInputHandler              ( )
            ,nRowHeight                 ( 10 )
            ,nColumnHeaderHeight        ( 10 )
            ,nRowHeaderWidth            ( 10 )
            ,m_aGridLineColor           ( )
            ,m_aHeaderBackgroundColor   ( )
            ,m_aHeaderTextColor         ( )
            ,m_aTextColor               ( )
            ,m_aTextLineColor           ( )
            ,m_aRowColors               ( )
            ,m_eVerticalAlign           ( com::sun::star::style::VerticalAlignment_TOP )
        {
        }
    };

    //==================================================================================================================
    //= UnoControlTableModel
    //==================================================================================================================
#ifdef DBG_UTIL
    const char* UnoControlTableModel_checkInvariants( const void* _pInstance )
    {
        return static_cast< const UnoControlTableModel* >( _pInstance )->checkInvariants();
    }

    //------------------------------------------------------------------------------------------------------------------
    const char* UnoControlTableModel::checkInvariants() const
    {
        Reference< XGridDataModel > const xDataModel( m_pImpl->m_aDataModel );
        if ( !xDataModel.is() )
            return "data model anymore";

        // TODO: more?

        return NULL;
    }
#endif

#define DBG_CHECK_ME() \
    DBG_TESTSOLARMUTEX(); \
    DBG_CHKTHIS( UnoControlTableModel, UnoControlTableModel_checkInvariants )

    //------------------------------------------------------------------------------------------------------------------
    DBG_NAME( UnoControlTableModel )
    UnoControlTableModel::UnoControlTableModel()
        :m_pImpl( new UnoControlTableModel_Impl )
    {
        DBG_CTOR( UnoControlTableModel, UnoControlTableModel_checkInvariants );
        m_pImpl->bHasColumnHeaders = true;
        m_pImpl->bHasRowHeaders = false;
        m_pImpl->pRenderer.reset( new GridTableRenderer( *this ) );
        m_pImpl->pInputHandler.reset( new DefaultInputHandler );
    }

    //------------------------------------------------------------------------------------------------------------------
    UnoControlTableModel::~UnoControlTableModel()
    {
        DBG_DTOR( UnoControlTableModel, UnoControlTableModel_checkInvariants );
        DELETEZ( m_pImpl );
    }

    //------------------------------------------------------------------------------------------------------------------
    TableSize UnoControlTableModel::getColumnCount() const
    {
        DBG_CHECK_ME();
        return (TableSize)m_pImpl->aColumns.size();
    }

    //------------------------------------------------------------------------------------------------------------------
    TableSize UnoControlTableModel::getRowCount() const
    {
        DBG_CHECK_ME();

        TableSize nRowCount = 0;
        try
        {
            Reference< XGridDataModel > const xDataModel( m_pImpl->m_aDataModel );
            ENSURE_OR_THROW( xDataModel.is(), "no data model anymore!" );
            nRowCount = xDataModel->getRowCount();
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return nRowCount;
    }

    //------------------------------------------------------------------------------------------------------------------
    bool UnoControlTableModel::hasColumnHeaders() const
    {
        DBG_CHECK_ME();
        return m_pImpl->bHasColumnHeaders;
    }

    //------------------------------------------------------------------------------------------------------------------
    bool UnoControlTableModel::hasRowHeaders() const
    {
        DBG_CHECK_ME();
        return m_pImpl->bHasRowHeaders;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setRowHeaders(bool _bRowHeaders)
    {
        DBG_CHECK_ME();
        if ( m_pImpl->bHasRowHeaders == _bRowHeaders )
            return;

        m_pImpl->bHasRowHeaders = _bRowHeaders;
        impl_notifyTableMetricsChanged();
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setColumnHeaders(bool _bColumnHeaders)
    {
        DBG_CHECK_ME();
        if ( m_pImpl->bHasColumnHeaders == _bColumnHeaders )
            return;

        m_pImpl->bHasColumnHeaders = _bColumnHeaders;
        impl_notifyTableMetricsChanged();
    }

    //------------------------------------------------------------------------------------------------------------------
    bool UnoControlTableModel::isCellEditable( ColPos col, RowPos row ) const
    {
        DBG_CHECK_ME();
        (void)col;
        (void)row;
        return false;
    }

    //------------------------------------------------------------------------------------------------------------------
    PColumnModel UnoControlTableModel::getColumnModel( ColPos column )
    {
        DBG_CHECK_ME();
        ENSURE_OR_RETURN( ( column >= 0 ) && ( column < getColumnCount() ),
            "DefaultTableModel::getColumnModel: invalid index!", PColumnModel() );
        return m_pImpl->aColumns[ column ];
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::appendColumn( Reference< XGridColumn > const & i_column )
    {
        DBG_CHECK_ME();
        insertColumn( m_pImpl->aColumns.size(), i_column );
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::insertColumn( ColPos const i_position, Reference< XGridColumn > const & i_column )
    {
        DBG_CHECK_ME();
        ENSURE_OR_RETURN_VOID( ( i_position >= 0 ) && ( size_t( i_position ) <= m_pImpl->aColumns.size() ),
            "UnoControlTableModel::insertColumn: illegal position!" );

        const PColumnModel pColumn( new UnoGridColumnFacade( *this, i_column ) );
        m_pImpl->aColumns.insert( m_pImpl->aColumns.begin() + i_position, pColumn );

        // notify listeners
        ModellListeners aListeners( m_pImpl->m_aListeners );
        for (   ModellListeners::const_iterator loop = aListeners.begin();
                loop != aListeners.end();
                ++loop
            )
        {
            (*loop)->columnInserted( i_position );
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::removeColumn( ColPos const i_position )
    {
        DBG_CHECK_ME();
        ENSURE_OR_RETURN_VOID( ( i_position >= 0 ) && ( size_t( i_position ) <= m_pImpl->aColumns.size() ),
            "UnoControlTableModel::removeColumn: illegal position!" );

        // remove the column
        ColumnModels::iterator pos = m_pImpl->aColumns.begin() + i_position;
        const PColumnModel pColumn = *pos;
        m_pImpl->aColumns.erase( pos );

        // notify listeners
        ModellListeners aListeners( m_pImpl->m_aListeners );
        for (   ModellListeners::const_iterator loop = aListeners.begin();
                loop != aListeners.end();
                ++loop
            )
        {
            (*loop)->columnRemoved( i_position );
        }

        // dispose the column
        UnoGridColumnFacade* pColumnImpl = dynamic_cast< UnoGridColumnFacade* >( pColumn.get() );
        OSL_ENSURE( pColumnImpl != NULL, "UnoControlTableModel::removeColumn: illegal column implementation!" );
        if ( pColumnImpl )
            pColumnImpl->dispose();
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::removeAllColumns()
    {
        DBG_CHECK_ME();
        if ( m_pImpl->aColumns.empty() )
            return;

        // dispose the column instances
        for (   ColumnModels::const_iterator col = m_pImpl->aColumns.begin();
                col != m_pImpl->aColumns.end();
                ++col
            )
        {
            UnoGridColumnFacade* pColumn = dynamic_cast< UnoGridColumnFacade* >( col->get() );
            ENSURE_OR_CONTINUE( pColumn != NULL, "UnoControlTableModel::removeAllColumns: illegal column implementation!" );
            pColumn->dispose();
        }
        m_pImpl->aColumns.clear();

        // notify listeners
        ModellListeners aListeners( m_pImpl->m_aListeners );
        for (   ModellListeners::const_iterator loop = aListeners.begin();
                loop != aListeners.end();
                ++loop
            )
        {
            (*loop)->allColumnsRemoved();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::impl_notifyTableMetricsChanged() const
    {
        ModellListeners aListeners( m_pImpl->m_aListeners );
        for (   ModellListeners::const_iterator loop = aListeners.begin();
                loop != aListeners.end();
                ++loop
            )
        {
            (*loop)->tableMetricsChanged();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    PTableRenderer UnoControlTableModel::getRenderer() const
    {
        DBG_CHECK_ME();
        return m_pImpl->pRenderer;
    }

    //------------------------------------------------------------------------------------------------------------------
    PTableInputHandler UnoControlTableModel::getInputHandler() const
    {
        DBG_CHECK_ME();
        return m_pImpl->pInputHandler;
    }

    //------------------------------------------------------------------------------------------------------------------
    TableMetrics UnoControlTableModel::getRowHeight() const
    {
        DBG_CHECK_ME();
        return m_pImpl->nRowHeight;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setRowHeight(TableMetrics _nRowHeight)
    {
        DBG_CHECK_ME();
        if ( m_pImpl->nRowHeight == _nRowHeight )
            return;

        m_pImpl->nRowHeight = _nRowHeight;
        impl_notifyTableMetricsChanged();
    }

    //------------------------------------------------------------------------------------------------------------------
    TableMetrics UnoControlTableModel::getColumnHeaderHeight() const
    {
        DBG_CHECK_ME();
        DBG_ASSERT( hasColumnHeaders(), "DefaultTableModel::getColumnHeaderHeight: invalid call!" );
        return m_pImpl->nColumnHeaderHeight;
    }

    //------------------------------------------------------------------------------------------------------------------
    TableMetrics UnoControlTableModel::getRowHeaderWidth() const
    {
        DBG_CHECK_ME();
        DBG_ASSERT( hasRowHeaders(), "DefaultTableModel::getRowHeaderWidth: invalid call!" );
        return m_pImpl->nRowHeaderWidth;
    }
    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setColumnHeaderHeight(TableMetrics _nHeight)
    {
        DBG_CHECK_ME();
        if ( m_pImpl->nColumnHeaderHeight == _nHeight )
            return;

        m_pImpl->nColumnHeaderHeight = _nHeight;
        impl_notifyTableMetricsChanged();
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setRowHeaderWidth(TableMetrics _nWidth)
    {
        DBG_CHECK_ME();
        if ( m_pImpl->nRowHeaderWidth == _nWidth )
            return;

        m_pImpl->nRowHeaderWidth = _nWidth;
        impl_notifyTableMetricsChanged();
    }

    //------------------------------------------------------------------------------------------------------------------
    ScrollbarVisibility UnoControlTableModel::getVerticalScrollbarVisibility() const
    {
        DBG_CHECK_ME();
        return m_pImpl->eVScrollMode;
    }

    //------------------------------------------------------------------------------------------------------------------
    ScrollbarVisibility UnoControlTableModel::getHorizontalScrollbarVisibility() const
    {
        DBG_CHECK_ME();
        return m_pImpl->eHScrollMode;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::addTableModelListener( const PTableModelListener& i_listener )
    {
        DBG_CHECK_ME();
        ENSURE_OR_RETURN_VOID( !!i_listener, "illegal NULL listener" );
        m_pImpl->m_aListeners.push_back( i_listener );
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::removeTableModelListener( const PTableModelListener& i_listener )
    {
        DBG_CHECK_ME();
        for (   ModellListeners::iterator lookup = m_pImpl->m_aListeners.begin();
                lookup != m_pImpl->m_aListeners.end();
                ++lookup
            )
        {
            if ( *lookup == i_listener )
            {
                m_pImpl->m_aListeners.erase( lookup );
                return;
            }
        }
        OSL_ENSURE( false, "UnoControlTableModel::removeTableModelListener: listener is not registered - sure you're doing the right thing here?" );
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setVerticalScrollbarVisibility( ScrollbarVisibility const i_visibility ) const
    {
        DBG_CHECK_ME();
        m_pImpl->eVScrollMode = i_visibility;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setHorizontalScrollbarVisibility( ScrollbarVisibility const i_visibility ) const
    {
        DBG_CHECK_ME();
        m_pImpl->eHScrollMode = i_visibility;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setDataModel( Reference< XGridDataModel > const & i_gridDataModel )
    {
        DBG_CHECK_ME();
        m_pImpl->m_aDataModel = i_gridDataModel;
        // TODO: register as listener, so we're notified of row/data changes, and can multiplex them to our
        // own listeners
    }

    //------------------------------------------------------------------------------------------------------------------
    Reference< XGridDataModel > UnoControlTableModel::getDataModel() const
    {
        Reference< XGridDataModel > const xDataModel( m_pImpl->m_aDataModel );
        return xDataModel;
    }

    //------------------------------------------------------------------------------------------------------------------
    bool UnoControlTableModel::hasDataModel() const
    {
        return getDataModel().is();
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setColumnModel( Reference< XGridColumnModel > const & i_gridColumnModel )
    {
        DBG_CHECK_ME();
        m_pImpl->m_aColumnModel = i_gridColumnModel;
    }

    //------------------------------------------------------------------------------------------------------------------
    Reference< XGridColumnModel > UnoControlTableModel::getColumnModel() const
    {
        Reference< XGridColumnModel > const xColumnModel( m_pImpl->m_aColumnModel );
        return xColumnModel;
    }

    //------------------------------------------------------------------------------------------------------------------
    bool UnoControlTableModel::hasColumnModel() const
    {
        return getColumnModel().is();
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::getCellContent( ColPos const i_col, RowPos const i_row, Any& o_cellContent )
    {
        DBG_CHECK_ME();

        try
        {
            Reference< XGridDataModel > const xDataModel( m_pImpl->m_aDataModel );
            ENSURE_OR_THROW( xDataModel.is(), "no data model anymore!" );

            if ( i_col >= xDataModel->getColumnCount() )
            {
                // this is allowed, in case the column model has been dynamically extended, but the data model does
                // not (yet?) know about it.
                // So, handle it gracefully.
            #if OSL_DEBUG_LEVEL > 0
                {
                    Reference< XGridColumnModel > const xColumnModel( m_pImpl->m_aColumnModel );
                    OSL_ENSURE( xColumnModel.is() && i_col < xColumnModel->getColumnCount(),
                        "UnoControlTableModel::getCellContent: request a column's value which the ColumnModel doesn't know about!" );
                }
            #endif
                o_cellContent.clear();
            }
            else
            {
                o_cellContent = xDataModel->getCellData( i_col, i_row );
            }
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::getCellToolTip( ColPos const i_col, RowPos const i_row, Any& o_cellToolTip )
    {
        DBG_CHECK_ME();
        try
        {
            Reference< XGridDataModel > const xDataModel( m_pImpl->m_aDataModel );
            ENSURE_OR_THROW( xDataModel.is(), "no data model anymore!" );

            o_cellToolTip = xDataModel->getCellToolTip( i_col, i_row );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    ::rtl::OUString UnoControlTableModel::getRowHeader( RowPos const i_rowPos ) const
    {
        DBG_CHECK_ME();

        ::rtl::OUString sRowHeader;

        Reference< XGridDataModel > const xDataModel( m_pImpl->m_aDataModel );
        ENSURE_OR_RETURN( xDataModel.is(), "UnoControlTableModel::getRowHeader: no data model anymore!", sRowHeader );

        try
        {
            sRowHeader = xDataModel->getRowTitle( i_rowPos );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return sRowHeader;
    }

    //------------------------------------------------------------------------------------------------------------------
    namespace
    {
        void lcl_setColor( Any const & i_color, ::boost::optional< ::Color > & o_convertedColor )
        {
            if ( !i_color.hasValue() )
                o_convertedColor.reset();
            else
            {
                sal_Int32 nColor = COL_TRANSPARENT;
                if ( i_color >>= nColor )
                {
                    o_convertedColor.reset( ::Color( nColor ) );
                }
                else
                {
                    OSL_ENSURE( false, "lcl_setColor: could not extract color value!" );
                }
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    ::boost::optional< ::Color > UnoControlTableModel::getLineColor() const
    {
        DBG_CHECK_ME();
        return m_pImpl->m_aGridLineColor;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setLineColor( Any const & i_color )
    {
        DBG_CHECK_ME();
        lcl_setColor( i_color, m_pImpl->m_aGridLineColor );
    }

    //------------------------------------------------------------------------------------------------------------------
    ::boost::optional< ::Color > UnoControlTableModel::getHeaderBackgroundColor() const
    {
        DBG_CHECK_ME();
        return m_pImpl->m_aHeaderBackgroundColor;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setHeaderBackgroundColor( Any const & i_color )
    {
        DBG_CHECK_ME();
        lcl_setColor( i_color, m_pImpl->m_aHeaderBackgroundColor );
    }

    //------------------------------------------------------------------------------------------------------------------
    ::boost::optional< ::Color > UnoControlTableModel::getHeaderTextColor() const
    {
        DBG_CHECK_ME();
        return m_pImpl->m_aHeaderTextColor;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setHeaderTextColor( Any const & i_color )
    {
        DBG_CHECK_ME();
        lcl_setColor( i_color, m_pImpl->m_aHeaderTextColor );
    }

    //------------------------------------------------------------------------------------------------------------------
    ::boost::optional< ::Color > UnoControlTableModel::getTextColor() const
    {
        DBG_CHECK_ME();
        return m_pImpl->m_aTextColor;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setTextColor( Any const & i_color )
    {
        DBG_CHECK_ME();
        lcl_setColor( i_color, m_pImpl->m_aTextColor );
    }

    //------------------------------------------------------------------------------------------------------------------
    ::boost::optional< ::Color > UnoControlTableModel::getTextLineColor() const
    {
        DBG_CHECK_ME();
        return m_pImpl->m_aTextColor;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setTextLineColor( Any const & i_color )
    {
        DBG_CHECK_ME();
        lcl_setColor( i_color, m_pImpl->m_aTextLineColor );
    }

    //------------------------------------------------------------------------------------------------------------------
    ::boost::optional< ::std::vector< ::Color > > UnoControlTableModel::getRowBackgroundColors() const
    {
        DBG_CHECK_ME();
        return m_pImpl->m_aRowColors;
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::setRowBackgroundColors( ::com::sun::star::uno::Any const & i_APIValue )
    {
        DBG_CHECK_ME();
        Sequence< ::com::sun::star::util::Color > aAPIColors;
        if ( !( i_APIValue >>= aAPIColors ) )
            m_pImpl->m_aRowColors.reset();
        else
        {
            ::std::vector< ::Color > aColors( aAPIColors.getLength() );
            for ( sal_Int32 i=0; i<aAPIColors.getLength(); ++i )
            {
                aColors[i] = ::Color( aAPIColors[i] );
            }
            m_pImpl->m_aRowColors.reset( aColors );
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    ::com::sun::star::style::VerticalAlignment UnoControlTableModel::getVerticalAlign() const
    {
        DBG_CHECK_ME();
        return  m_pImpl->m_eVerticalAlign;
    }

    //--------------------------------------------------------------------
    void UnoControlTableModel::setVerticalAlign( com::sun::star::style::VerticalAlignment _xAlign )
    {
        DBG_CHECK_ME();
        m_pImpl->m_eVerticalAlign = _xAlign;
    }

    //--------------------------------------------------------------------
    ColPos UnoControlTableModel::getColumnPos( UnoGridColumnFacade const & i_column ) const
    {
        DBG_CHECK_ME();
        for (   ColumnModels::const_iterator col = m_pImpl->aColumns.begin();
                col != m_pImpl->aColumns.end();
                ++col
            )
        {
            if ( &i_column == col->get() )
                return col - m_pImpl->aColumns.begin();
        }
        OSL_ENSURE( false, "UnoControlTableModel::getColumnPos: column not found!" );
        return COL_INVALID;
    }

    //--------------------------------------------------------------------
    void UnoControlTableModel::notifyColumnChange( ColPos const i_columnPos, ColumnAttributeGroup const i_attributeGroup ) const
    {
        DBG_CHECK_ME();
        ENSURE_OR_RETURN_VOID( ( i_columnPos >= 0 ) && ( i_columnPos < getColumnCount() ),
            "UnoControlTableModel::notifyColumnChange: invalid column index!" );

        ModellListeners aListeners( m_pImpl->m_aListeners );
        for (   ModellListeners::const_iterator loop = aListeners.begin();
                loop != aListeners.end();
                ++loop
            )
        {
            (*loop)->columnChanged( i_columnPos, i_attributeGroup );
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::notifyRowsInserted( GridDataEvent const & i_event )
    {
        // check sanity of the event
        ENSURE_OR_RETURN_VOID( i_event.FirstRow >= 0, "UnoControlTableModel::notifyRowsInserted: invalid first row!" );
        ENSURE_OR_RETURN_VOID( i_event.LastRow >= i_event.FirstRow, "UnoControlTableModel::notifyRowsInserted: invalid row indexes!" );

        // check own sanity
        Reference< XGridColumnModel > const xColumnModel( m_pImpl->m_aColumnModel );
        ENSURE_OR_RETURN_VOID( xColumnModel.is(), "UnoControlTableModel::notifyRowsInserted: no column model anymore!" );

        Reference< XGridDataModel > const xDataModel( m_pImpl->m_aDataModel );
        ENSURE_OR_RETURN_VOID( xDataModel.is(), "UnoControlTableModel::notifyRowsInserted: no data model anymore!" );

        // implicitly add columns to the column model
        // TODO: is this really a good idea?
        sal_Int32 const dataColumnCount = xDataModel->getColumnCount();
        OSL_ENSURE( dataColumnCount > 0, "UnoControlTableModel::notifyRowsInserted: no columns at all?" );

        sal_Int32 const modelColumnCount = xColumnModel->getColumnCount();
        if ( ( modelColumnCount == 0 ) && ( dataColumnCount > 0 ) )
        {
            // TODO: shouldn't we clear the mutexes guard for this call?
            xColumnModel->setDefaultColumns( dataColumnCount );
        }

        // multiplex the event to our own listeners
        ModellListeners aListeners( m_pImpl->m_aListeners );
        for (   ModellListeners::const_iterator loop = aListeners.begin();
                loop != aListeners.end();
                ++loop
            )
        {
            (*loop)->rowsInserted( i_event.FirstRow, i_event.LastRow );
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void UnoControlTableModel::notifyRowsRemoved( GridDataEvent const & i_event )
    {
        // multiplex the event to our own listeners
        ModellListeners aListeners( m_pImpl->m_aListeners );
        for (   ModellListeners::const_iterator loop = aListeners.begin();
                loop != aListeners.end();
                ++loop
            )
        {
            (*loop)->rowsRemoved( i_event.FirstRow, i_event.LastRow );
        }
    }

// .....................................................................................................................
} } // svt::table
// .....................................................................................................................
