/*************************************************************************
 *
 *  $RCSfile: AccessiblePreviewHeaderCell.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: thb $ $Date: 2002-05-17 19:05:08 $
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

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif

#ifndef _SC_ACCESSIBLETEXT_HXX
#include "AccessibleText.hxx"
#endif
#ifndef SC_EDITSRC_HXX
#include "editsrc.hxx"
#endif
#ifndef _SVX_ACCESSILE_TEXT_HELPER_HXX_
#include <svx/AccessibleTextHelper.hxx>
#endif
#include "AccessiblePreviewHeaderCell.hxx"
#include "prevwsh.hxx"
#include "unoguard.hxx"
#include "miscuno.hxx"
#include "prevloc.hxx"

#include <drafts/com/sun/star/accessibility/AccessibleRole.hpp>
#include <drafts/com/sun/star/accessibility/AccessibleStateType.hpp>

#include <vcl/window.hxx>
#include <svtools/smplhint.hxx>
#include <unotools/accessiblestatesethelper.hxx>
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star::accessibility;

//=====  internal  ============================================================

ScAccessiblePreviewHeaderCell::ScAccessiblePreviewHeaderCell( const ::com::sun::star::uno::Reference<
                                ::drafts::com::sun::star::accessibility::XAccessible>& rxParent,
                            ScPreviewShell* pViewShell,
                            const ScAddress& rCellPos, sal_Bool bIsColHdr, sal_Bool bIsRowHdr,
                            sal_Int32 nIndex, const Rectangle& rPosition ) :
    ScAccessibleContextBase( rxParent, AccessibleRole::TABLE ),
    mpViewShell( pViewShell ),
    maCellPos( rCellPos ),
    mbColumnHeader( bIsColHdr ),
    mbRowHeader( bIsRowHdr ),
    mnIndex( nIndex ),
    maPosition( rPosition )
{
    if (mpViewShell)
        mpViewShell->AddAccessibilityObject(*this);
}

ScAccessiblePreviewHeaderCell::~ScAccessiblePreviewHeaderCell()
{
    if (mpViewShell)
        mpViewShell->RemoveAccessibilityObject(*this);
}

void SAL_CALL ScAccessiblePreviewHeaderCell::disposing()
{
    if (mpViewShell)
    {
        mpViewShell->RemoveAccessibilityObject(*this);
        mpViewShell = NULL;
    }

    ScAccessibleContextBase::disposing();
}

//=====  SfxListener  =====================================================

void ScAccessiblePreviewHeaderCell::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
    ScAccessibleContextBase::Notify(rBC, rHint);
}

//=====  XInterface  =====================================================

uno::Any SAL_CALL ScAccessiblePreviewHeaderCell::queryInterface( uno::Type const & rType )
    throw (uno::RuntimeException)
{
    uno::Any aAny (ScAccessiblePreviewHeaderCellImpl::queryInterface(rType));
    return aAny.hasValue() ? aAny : ScAccessibleContextBase::queryInterface(rType);
}

void SAL_CALL ScAccessiblePreviewHeaderCell::acquire()
    throw ()
{
    ScAccessibleContextBase::acquire();
}

void SAL_CALL ScAccessiblePreviewHeaderCell::release()
    throw ()
{
    ScAccessibleContextBase::release();
}

//=====  XAccessibleValue  ================================================

uno::Any SAL_CALL ScAccessiblePreviewHeaderCell::getCurrentValue() throw (uno::RuntimeException)
{
    //  column or row headers don't have a value
    return uno::Any();
}

sal_Bool SAL_CALL ScAccessiblePreviewHeaderCell::setCurrentValue( const uno::Any& aNumber )
                                                                throw (uno::RuntimeException)
{
    //  column or row headers don't have a value
    return sal_False;
}

uno::Any SAL_CALL ScAccessiblePreviewHeaderCell::getMaximumValue() throw (uno::RuntimeException)
{
    //  column or row headers don't have a value
    return uno::Any();
}

uno::Any SAL_CALL ScAccessiblePreviewHeaderCell::getMinimumValue() throw (uno::RuntimeException)
{
    //  column or row headers don't have a value
    return uno::Any();
}

//=====  XAccessibleComponent  ============================================

uno::Reference< XAccessible > SAL_CALL ScAccessiblePreviewHeaderCell::getAccessibleAt( const awt::Point& rPoint )
                                throw (uno::RuntimeException)
{
     ScUnoGuard aGuard;
    if(!mpTextHelper)
        CreateTextHelper();

    return mpTextHelper->GetAt(rPoint);
}

void SAL_CALL ScAccessiblePreviewHeaderCell::grabFocus() throw (uno::RuntimeException)
{
     ScUnoGuard aGuard;
    if (getAccessibleParent().is())
    {
        uno::Reference<XAccessibleComponent> xAccessibleComponent(getAccessibleParent()->getAccessibleContext(), uno::UNO_QUERY);
        if (xAccessibleComponent.is())
            xAccessibleComponent->grabFocus();
    }
}

//=====  XAccessibleContext  ==============================================

sal_Int32 SAL_CALL ScAccessiblePreviewHeaderCell::getAccessibleChildCount() throw(uno::RuntimeException)
{
    ScUnoGuard aGuard;
    if (!mpTextHelper)
        CreateTextHelper();
    return mpTextHelper->GetChildCount();
}

uno::Reference< XAccessible > SAL_CALL ScAccessiblePreviewHeaderCell::getAccessibleChild(sal_Int32 nIndex)
                            throw (uno::RuntimeException, lang::IndexOutOfBoundsException)
{
    ScUnoGuard aGuard;
    if (!mpTextHelper)
        CreateTextHelper();
    return mpTextHelper->GetChild(nIndex);
}

sal_Int32 SAL_CALL ScAccessiblePreviewHeaderCell::getAccessibleIndexInParent() throw (uno::RuntimeException)
{
    return mnIndex;
}

uno::Reference< XAccessibleRelationSet > SAL_CALL ScAccessiblePreviewHeaderCell::getAccessibleRelationSet()
                                throw (uno::RuntimeException)
{
    //! missing
    return NULL;
}

uno::Reference<XAccessibleStateSet> SAL_CALL ScAccessiblePreviewHeaderCell::getAccessibleStateSet()
                            throw(uno::RuntimeException)
{
    ScUnoGuard aGuard;

    uno::Reference<XAccessibleStateSet> xParentStates;
    if (getAccessibleParent().is())
    {
        uno::Reference<XAccessibleContext> xParentContext = getAccessibleParent()->getAccessibleContext();
        xParentStates = xParentContext->getAccessibleStateSet();
    }
    utl::AccessibleStateSetHelper* pStateSet = new utl::AccessibleStateSetHelper();
    if (IsDefunc(xParentStates))
        pStateSet->AddState(AccessibleStateType::DEFUNC);
    pStateSet->AddState(AccessibleStateType::ENABLED);
    pStateSet->AddState(AccessibleStateType::MULTILINE);
    if (isShowing())
        pStateSet->AddState(AccessibleStateType::SHOWING);
    pStateSet->AddState(AccessibleStateType::TRANSIENT);
    if (isVisible())
        pStateSet->AddState(AccessibleStateType::VISIBLE);
    return pStateSet;
}

//=====  XServiceInfo  ====================================================

rtl::OUString SAL_CALL ScAccessiblePreviewHeaderCell::getImplementationName() throw(uno::RuntimeException)
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ScAccessiblePreviewHeaderCell"));
}

uno::Sequence<rtl::OUString> SAL_CALL ScAccessiblePreviewHeaderCell::getSupportedServiceNames()
                                                    throw(uno::RuntimeException)
{
    uno::Sequence< ::rtl::OUString > aSequence = ScAccessibleContextBase::getSupportedServiceNames();
    sal_Int32 nOldSize(aSequence.getLength());
    aSequence.realloc(nOldSize + 1);
    ::rtl::OUString* pNames = aSequence.getArray();

    pNames[nOldSize] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("drafts.com.sun.star.AccessibleCell"));

    return aSequence;
}

//=====  XTypeProvider  =======================================================

uno::Sequence< uno::Type > SAL_CALL ScAccessiblePreviewHeaderCell::getTypes()
        throw (uno::RuntimeException)
{
    return comphelper::concatSequences(ScAccessiblePreviewHeaderCellImpl::getTypes(), ScAccessibleContextBase::getTypes());
}

uno::Sequence<sal_Int8> SAL_CALL
    ScAccessiblePreviewHeaderCell::getImplementationId(void)
    throw (uno::RuntimeException)
{
    ScUnoGuard aGuard;
    static uno::Sequence<sal_Int8> aId;
    if (aId.getLength() == 0)
    {
        aId.realloc (16);
        rtl_createUuid (reinterpret_cast<sal_uInt8 *>(aId.getArray()), 0, sal_True);
    }
    return aId;
}

//====  internal  =========================================================

Rectangle ScAccessiblePreviewHeaderCell::GetBoundingBoxOnScreen() throw (uno::RuntimeException)
{
    Rectangle aCellRect(GetBoundingBox());
    if (mpViewShell)
    {
        Window* pWindow = mpViewShell->GetWindow();
        if (pWindow)
        {
            Rectangle aRect = pWindow->GetWindowExtentsRelative(NULL);
            aCellRect.setX(aCellRect.getX() + aRect.getX());
            aCellRect.setY(aCellRect.getY() + aRect.getY());
        }
    }
    return aCellRect;
}

Rectangle ScAccessiblePreviewHeaderCell::GetBoundingBox() throw (uno::RuntimeException)
{
    return maPosition;
}

rtl::OUString SAL_CALL ScAccessiblePreviewHeaderCell::createAccessibleDescription() throw(uno::RuntimeException)
{
    return createAccessibleName();
}

rtl::OUString SAL_CALL ScAccessiblePreviewHeaderCell::createAccessibleName() throw(uno::RuntimeException)
{
    rtl::OUString sName;

    if ( mbColumnHeader )
    {
        if ( mbRowHeader )
        {
            //! name for corner cell?

            sName = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Column/Row Header"));
        }
        else
        {
            // name of column header
            sName = ColToAlpha( maCellPos.Col() );
        }
    }
    else
    {
        // name of row header
        sName = rtl::OUString::valueOf( (sal_Int32) ( maCellPos.Row() + 1 ) );
    }

    return sName;
}

sal_Bool ScAccessiblePreviewHeaderCell::IsDefunc( const uno::Reference<XAccessibleStateSet>& rxParentStates )
{
    return ScAccessibleContextBase::IsDefunc() || (mpViewShell == NULL) || !getAccessibleParent().is() ||
        (rxParentStates.is() && rxParentStates->contains(AccessibleStateType::DEFUNC));
}

void ScAccessiblePreviewHeaderCell::CreateTextHelper()
{
    if (!mpTextHelper)
    {
        ::std::auto_ptr < ScAccessibleTextData > pAccessiblePreviewHeaderCellTextData
            (new ScAccessiblePreviewHeaderCellTextData(mpViewShell, String(getAccessibleName()), maCellPos, mbColumnHeader, mbRowHeader));
        ::std::auto_ptr< SvxEditSource > pEditSource (new ScAccessibilityEditSource(pAccessiblePreviewHeaderCellTextData));

        mpTextHelper = new accessibility::AccessibleTextHelper(this, pEditSource );
    }
}
