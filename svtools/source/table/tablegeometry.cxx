/*************************************************************************
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


#include "tablegeometry.hxx"
#include "tablecontrol_impl.hxx"

#include <tools/debug.hxx>

//........................................................................
namespace svt { namespace table
{
//........................................................................

    //====================================================================
    //= TableRowGeometry
    //====================================================================
    //--------------------------------------------------------------------
    TableRowGeometry::TableRowGeometry( const TableControl_Impl& _rControl, const Rectangle& _rBoundaries,
            RowPos _nRow )
        :TableGeometry( _rControl, _rBoundaries )
        ,m_nRowPos( _nRow )
    {
        if ( m_nRowPos == ROW_COL_HEADERS )
        {
            m_aRect.Top() = 0;
            m_aRect.Bottom() = m_rControl.m_nColHeaderHeightPixel - 1;
        }
        else
        {
            impl_initRect();
        }
    }

    //--------------------------------------------------------------------
    void TableRowGeometry::impl_initRect()
    {
        if ( ( m_nRowPos >= m_rControl.m_nTopRow ) && ( m_nRowPos < m_rControl.m_pModel->getRowCount() ) )
        {
            m_aRect.Top() = m_rControl.m_nColHeaderHeightPixel + ( m_nRowPos - m_rControl.m_nTopRow ) * m_rControl.m_nRowHeightPixel;
            m_aRect.Bottom() = m_aRect.Top() + m_rControl.m_nRowHeightPixel - 1;
        }
        else
            m_aRect.SetEmpty();
    }

    //--------------------------------------------------------------------
    bool TableRowGeometry::moveDown()
    {
        if ( m_nRowPos == ROW_COL_HEADERS )
        {
            m_nRowPos = m_rControl.m_nTopRow;
            impl_initRect();
        }
        else
        {
            if ( ++m_nRowPos < m_rControl.m_pModel->getRowCount() )
                m_aRect.Move( 0, m_rControl.m_nRowHeightPixel );
            else
                m_aRect.SetEmpty();
        }
        return isValid();
    }

    //====================================================================
    //= TableColumnGeometry
    //====================================================================
    //--------------------------------------------------------------------
    TableColumnGeometry::TableColumnGeometry( const TableControl_Impl& _rControl, const Rectangle& _rBoundaries,
            ColPos _nCol )
        :TableGeometry( _rControl, _rBoundaries )
        ,m_nColPos( _nCol )
    {
        if ( m_nColPos == COL_ROW_HEADERS )
        {
            m_aRect.Left() = 0;
            m_aRect.Right() = m_rControl.m_nRowHeaderWidthPixel - 1;
        }
        else
        {
            impl_initRect();
        }
    }

    //--------------------------------------------------------------------
    void TableColumnGeometry::impl_initRect()
    {
        ColPos nLeftColumn = m_rControl.m_nLeftColumn;
        if ( ( m_nColPos >= nLeftColumn ) && ( m_nColPos < (ColPos)m_rControl.m_aColumnWidths.size() ) )
        {
            m_aRect.Left() = m_rControl.m_nRowHeaderWidthPixel;
            // TODO: take into account any possibly frozen columns

            for ( ColPos col = nLeftColumn; col < m_nColPos; ++col )
                m_aRect.Left() += m_rControl.m_aColumnWidths[ col ].getWidth();
            m_aRect.Right() = m_aRect.Left() + m_rControl.m_aColumnWidths[ m_nColPos ].getWidth() - 1;
        }
        else
            m_aRect.SetEmpty();
    }

    //--------------------------------------------------------------------
    bool TableColumnGeometry::moveRight()
    {
        if ( m_nColPos == COL_ROW_HEADERS )
        {
            m_nColPos = m_rControl.m_nLeftColumn;
            impl_initRect();
        }
        else
        {
            if ( ++m_nColPos < (ColPos)m_rControl.m_aColumnWidths.size() )
            {
                m_aRect.Left() = m_aRect.Right() + 1;
                m_aRect.Right() += m_rControl.m_aColumnWidths[ m_nColPos ].getWidth();
            }
            else
                m_aRect.SetEmpty();
        }

        return isValid();
    }

//........................................................................
} } // namespace svt::table
//........................................................................
