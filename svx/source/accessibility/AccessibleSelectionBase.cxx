/*************************************************************************
 *
 *  $RCSfile: AccessibleSelectionBase.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-24 16:54:40 $
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

#include "AccessibleSelectionBase.hxx"

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::accessibility;

namespace accessibility
{
    // ---------------------------
    // - AccessibleSelectionBase -
    // ---------------------------

    AccessibleSelectionBase::AccessibleSelectionBase()
    {
    }

    //--------------------------------------------------------------------

    AccessibleSelectionBase::~AccessibleSelectionBase()
    {
    }

    //--------------------------------------------------------------------

    void SAL_CALL AccessibleSelectionBase::selectAccessibleChild( sal_Int32 nChildIndex ) throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( implGetMutex() );
        OCommonAccessibleSelection::selectAccessibleChild( nChildIndex );
    }

    //--------------------------------------------------------------------

    sal_Bool SAL_CALL AccessibleSelectionBase::isAccessibleChildSelected( sal_Int32 nChildIndex ) throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( implGetMutex() );
        return( OCommonAccessibleSelection::isAccessibleChildSelected( nChildIndex ) );
    }

    //--------------------------------------------------------------------

    void SAL_CALL AccessibleSelectionBase::clearAccessibleSelection(  ) throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( implGetMutex() );
        OCommonAccessibleSelection::clearAccessibleSelection();
    }

    //--------------------------------------------------------------------

    void SAL_CALL AccessibleSelectionBase::selectAllAccessible(  ) throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( implGetMutex() );
        OCommonAccessibleSelection::selectAllAccessible();
    }

    //--------------------------------------------------------------------

    sal_Int32 SAL_CALL AccessibleSelectionBase::getSelectedAccessibleChildCount(  ) throw (uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( implGetMutex() );
        return( OCommonAccessibleSelection::getSelectedAccessibleChildCount() );
    }

    //--------------------------------------------------------------------

    uno::Reference< XAccessible > SAL_CALL AccessibleSelectionBase::getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex ) throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( implGetMutex() );
        return( OCommonAccessibleSelection::getSelectedAccessibleChild( nSelectedChildIndex ) );
    }

    //--------------------------------------------------------------------

    void SAL_CALL AccessibleSelectionBase::deselectAccessibleChild( sal_Int32 nSelectedChildIndex ) throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard( implGetMutex() );
        OCommonAccessibleSelection::deselectAccessibleChild( nSelectedChildIndex );
    }
};
