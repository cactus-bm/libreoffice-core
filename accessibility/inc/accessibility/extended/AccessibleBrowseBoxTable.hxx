/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: AccessibleBrowseBoxTable.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 15:18:37 $
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


#ifndef ACCESSIBILITY_EXT_ACCESSIBLEBROWSEBOXTABLE_HXX
#define ACCESSIBILITY_EXT_ACCESSIBLEBROWSEBOXTABLE_HXX

#ifndef ACCESSIBILITY_EXT_ACCESSIBLEBROWSEBOXTABLEBASE_HXX
#include "accessibility/extended/AccessibleBrowseBoxTableBase.hxx"
#endif

// ============================================================================

namespace accessibility {

// ============================================================================

/** This class represents the accessible object of the data table of a
    BrowseBox control. */
class AccessibleBrowseBoxTable : public AccessibleBrowseBoxTableBase
{
    friend class AccessibleBrowseBox;   // to create header bars

public:
    AccessibleBrowseBoxTable(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible >& rxParent,
        ::svt::IAccessibleTableProvider& rBrowseBox );

protected:
    virtual ~AccessibleBrowseBoxTable();

public:
    // XAccessibleContext -----------------------------------------------------

    /** @return  The XAccessible interface of the specified child. */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > SAL_CALL
    getAccessibleChild( sal_Int32 nChildIndex )
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException );

    /** @return  The index of this object among the parent's children. */
    virtual sal_Int32 SAL_CALL getAccessibleIndexInParent()
        throw ( ::com::sun::star::uno::RuntimeException );

    // XAccessibleComponent ---------------------------------------------------

    /** @return  The accessible child rendered under the given point. */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > SAL_CALL
    getAccessibleAtPoint( const ::com::sun::star::awt::Point& rPoint )
        throw ( ::com::sun::star::uno::RuntimeException );

    /** Grabs the focus to (the current cell of) the data table. */
    virtual void SAL_CALL grabFocus()
        throw ( ::com::sun::star::uno::RuntimeException );

    /** @return  The key bindings associated with this object. */
    virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding()
        throw ( ::com::sun::star::uno::RuntimeException );

    // XAccessibleTable -------------------------------------------------------

    /** @return  The description text of the specified row. */
    virtual ::rtl::OUString SAL_CALL getAccessibleRowDescription( sal_Int32 nRow )
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException );

    /** @return  The description text of the specified column. */
    virtual ::rtl::OUString SAL_CALL getAccessibleColumnDescription( sal_Int32 nColumn )
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException );

    /** @return  The XAccessibleTable interface of the row header bar. */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL
    getAccessibleRowHeaders()
        throw ( ::com::sun::star::uno::RuntimeException );

    /** @return  The XAccessibleTable interface of the column header bar. */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessibleTable > SAL_CALL
    getAccessibleColumnHeaders()
        throw ( ::com::sun::star::uno::RuntimeException );

    /** @return  An index list of completely selected rows. */
    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL
    getSelectedAccessibleRows()
        throw ( ::com::sun::star::uno::RuntimeException );

    /** @return  An index list of completely selected columns. */
    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL
    getSelectedAccessibleColumns()
        throw ( ::com::sun::star::uno::RuntimeException );

    /** @return  <TRUE/>, if the specified row is completely selected. */
    virtual sal_Bool SAL_CALL isAccessibleRowSelected( sal_Int32 nRow )
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException );

    /** @return  <TRUE/>, if the specified column is completely selected. */
    virtual sal_Bool SAL_CALL isAccessibleColumnSelected( sal_Int32 nColumn )
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException );

    /** @return The XAccessible interface of the cell object at the specified
                cell position. */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > SAL_CALL
    getAccessibleCellAt( sal_Int32 nRow, sal_Int32 nColumn )
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException );

    /** @return  <TRUE/>, if the specified cell is selected. */
    virtual sal_Bool SAL_CALL isAccessibleSelected( sal_Int32 nRow, sal_Int32 nColumn )
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException );

    // XServiceInfo -----------------------------------------------------------

    /** @return  The name of this class. */
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw ( ::com::sun::star::uno::RuntimeException );

protected:
    // internal virtual methods -----------------------------------------------

    /** @attention  This method requires locked mutex's and a living object.
        @return  The bounding box (VCL rect.) relative to the parent window. */
    virtual Rectangle implGetBoundingBox();
    /** @attention  This method requires locked mutex's and a living object.
        @return  The bounding box (VCL rect.) in screen coordinates. */
    virtual Rectangle implGetBoundingBoxOnScreen();

    // internal helper methods ------------------------------------------------

    /** @attention  This method requires a locked mutex.
        @return  The XAccessibleTable interface of the specified header bar. */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessibleTable >
    implGetHeaderBar( sal_Int32 nChildIndex )
        throw ( ::com::sun::star::uno::RuntimeException );
};

// ============================================================================

} // namespace accessibility

// ============================================================================

#endif

