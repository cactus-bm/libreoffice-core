/*************************************************************************
 *
 *  $RCSfile: AccessibleCellBase.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: sab $ $Date: 2002-02-21 17:23:53 $
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


#include "AccessibleCellBase.hxx"
#ifndef SC_SCATTR_HXX
#include "attrib.hxx"
#endif
#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
#ifndef SC_MISCUNO_HXX
#include "miscuno.hxx"
#endif
#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_DOCFUNC_HXX
#include "docfunc.hxx"
#endif
#ifndef SC_CELL_HXX
#include "cell.hxx"
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEROLE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleRole.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLESTATETYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleStateType.hpp>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif

#include <float.h>

using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star::accessibility;

//=====  internal  ============================================================

ScAccessibleCellBase::ScAccessibleCellBase (
        const uno::Reference<XAccessible>& rxParent,
        ScDocument* pDoc,
        ScAddress& rCellAddress,
        sal_Int32 nIndex)
    :
    ScAccessibleContextBase (rxParent, AccessibleRole::TABLE_CELL),
    mpDoc(pDoc),
    maCellAddress(rCellAddress),
    mnIndex(nIndex)
{
}

ScAccessibleCellBase::~ScAccessibleCellBase ()
{
}

    //=====  XInterface  ======================================================

uno::Any SAL_CALL
    ScAccessibleCellBase::queryInterface(
    const uno::Type & rType )
    throw(uno::RuntimeException)
{
    SC_QUERYINTERFACE( XAccessibleValue )

    return ScAccessibleContextBase::queryInterface( rType );
}

/** Increase the reference count.
*/
void SAL_CALL
    ScAccessibleCellBase::acquire (void)
    throw ()
{
    OWeakObject::acquire ();
}

/** Decrease the reference count.
*/
void SAL_CALL
    ScAccessibleCellBase::release (void)
    throw ()
{
    OWeakObject::release ();
}

    //=====  XAccessibleComponent  ============================================

sal_Bool SAL_CALL ScAccessibleCellBase::isVisible(  )
        throw (uno::RuntimeException)
{
     ::osl::MutexGuard aGuard (maMutex);
    // test whether the cell is hidden (column/row - hidden/filtered)
    sal_Bool bVisible(sal_True);
    if (mpDoc)
    {
        BYTE nColFlags = mpDoc->GetColFlags(maCellAddress.Col(), maCellAddress.Tab());
        BYTE nRowFlags = mpDoc->GetRowFlags(maCellAddress.Row(), maCellAddress.Tab());
        if (((nColFlags & CR_HIDDEN) == CR_HIDDEN) || ((nColFlags & CR_FILTERED) == CR_FILTERED) ||
            ((nRowFlags & CR_HIDDEN) == CR_HIDDEN) || ((nRowFlags & CR_FILTERED) == CR_FILTERED))
            bVisible = sal_False;
    }
    return bVisible;
}

    //=====  XAccessibleContext  ==============================================

sal_Int32
    ScAccessibleCellBase::getAccessibleIndexInParent (void)
        throw (uno::RuntimeException)
{
    ::osl::MutexGuard aGuard (maMutex);
    return mnIndex;
}

::rtl::OUString SAL_CALL
    ScAccessibleCellBase::createAccessibleDescription (void)
    throw (uno::RuntimeException)
{
    rtl::OUString sDescription;

    if (mpDoc)
    {
        ScPostIt aNote;
        mpDoc->GetNote(maCellAddress.Col(), maCellAddress.Row(), maCellAddress.Tab(), aNote);
        sDescription = rtl::OUString (aNote.GetText());
    }

    if (!sDescription.getLength())
        sDescription = getAccessibleName();

    return sDescription;
}

::rtl::OUString SAL_CALL
    ScAccessibleCellBase::createAccessibleName (void)
    throw (uno::RuntimeException)
{
    rtl::OUString sName;
    String sAddress;
    // Document not needed, because only the cell address, but not the tablename is needed
    maCellAddress.Format( sAddress, SCA_VALID, NULL );
    sName = rtl::OUString(sAddress);
    return sName;
}

    //=====  XAccessibleValue  ================================================

uno::Any SAL_CALL
    ScAccessibleCellBase::getCurrentValue(  )
    throw (uno::RuntimeException)
{
     ::osl::MutexGuard aGuard (maMutex);
    uno::Any aAny;
    if (mpDoc)
        aAny <<= mpDoc->GetValue(maCellAddress);

    return aAny;
}

sal_Bool SAL_CALL
    ScAccessibleCellBase::setCurrentValue( const uno::Any& aNumber )
    throw (uno::RuntimeException)
{
     ::osl::MutexGuard aGuard (maMutex);
    double fValue;
    sal_Bool bResult(sal_False);
    if((aNumber >>= fValue) && mpDoc && mpDoc->GetDocumentShell())
    {
        uno::Reference<XAccessibleStateSet> xParentStates;
        if (getAccessibleParent().is())
        {
            uno::Reference<XAccessibleContext> xParentContext = getAccessibleParent()->getAccessibleContext();
            xParentStates = xParentContext->getAccessibleStateSet();
        }
        if (IsEditable(xParentStates))
        {
            ScDocShell* pDocShell = (ScDocShell*) mpDoc->GetDocumentShell();
            ScDocFunc aFunc(*pDocShell);
            bResult = aFunc.PutCell( maCellAddress, new ScValueCell(fValue), TRUE );
        }
    }
    return bResult;
}

uno::Any SAL_CALL
    ScAccessibleCellBase::getMaximumValue(  )
    throw (uno::RuntimeException)
{
    uno::Any aAny;
    aAny <<= DBL_MAX;

    return aAny;
}

uno::Any SAL_CALL
    ScAccessibleCellBase::getMinimumValue(  )
    throw (uno::RuntimeException)
{
    uno::Any aAny;
    aAny <<= DBL_MIN;

    return aAny;
}

    //=====  XServiceInfo  ====================================================

::rtl::OUString SAL_CALL ScAccessibleCellBase::getImplementationName (void)
        throw (uno::RuntimeException)
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM ("ScAccessibleCellBase"));
}

    //=====  XTypeProvider  ===================================================

uno::Sequence< uno::Type> SAL_CALL ScAccessibleCellBase::getTypes (void)
        throw (uno::RuntimeException)
{
    ::osl::MutexGuard aGuard (maMutex);
    uno::Sequence< uno::Type>
        aTypeSequence = ScAccessibleContextBase::getTypes();
    sal_Int32 nOldSize(aTypeSequence.getLength());
    aTypeSequence.realloc(nOldSize + 1);
    uno::Type* pTypes = aTypeSequence.getArray();

    pTypes[nOldSize] = ::getCppuType((const uno::Reference<
            XAccessibleValue>*)0);

    return aTypeSequence;
}


sal_Bool ScAccessibleCellBase::IsEditable(
    const uno::Reference<XAccessibleStateSet>& rxParentStates)
{
    sal_Bool bEditable(sal_False);
    if (rxParentStates.is() && rxParentStates->contains(AccessibleStateType::EDITABLE))
        bEditable = sal_True;
    return bEditable;
}

