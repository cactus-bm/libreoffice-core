/*************************************************************************
 *
 *  $RCSfile: AccessibleComponentBase.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: af $ $Date: 2002-04-18 16:20:07 $
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


#include "AccessibleComponentBase.hxx"

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLE_ROLE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleRole.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPES_HPP_
#include <com/sun/star/drawing/XShapes.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEDESCRIPTOR_HPP_
#include <com/sun/star/drawing/XShapeDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_INDEXOUTOFBOUNDSEXCEPTION_HPP_
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star::accessibility;

namespace accessibility {

//=====  internal  ============================================================

AccessibleComponentBase::AccessibleComponentBase (void)
{
}




AccessibleComponentBase::~AccessibleComponentBase (void)
{
}




//=====  XAccessibleComponent  ================================================

sal_Bool SAL_CALL AccessibleComponentBase::contains (
        const ::com::sun::star::awt::Point& aPoint)
        throw (::com::sun::star::uno::RuntimeException)
{
    awt::Size aSize (getSize());
    return (aPoint.X >= 0)
        && (aPoint.X < aSize.Width)
        && (aPoint.Y >= 0)
        && (aPoint.Y <= aSize.Height);
}




uno::Reference<XAccessible > SAL_CALL
    AccessibleComponentBase::getAccessibleAt (
        const awt::Point& aPoint)
    throw (uno::RuntimeException)
{
    return uno::Reference<XAccessible>();
}




awt::Rectangle SAL_CALL AccessibleComponentBase::getBounds (void)
    throw (uno::RuntimeException)
{
    return awt::Rectangle();
}




awt::Point SAL_CALL AccessibleComponentBase::getLocation (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    awt::Rectangle aBBox (getBounds());
    return awt::Point (aBBox.X, aBBox.Y);
}




awt::Point SAL_CALL AccessibleComponentBase::getLocationOnScreen (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return awt::Point();
}




::com::sun::star::awt::Size SAL_CALL AccessibleComponentBase::getSize (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    awt::Rectangle aBBox (getBounds());
    return awt::Size (aBBox.Width, aBBox.Height);
}




sal_Bool SAL_CALL AccessibleComponentBase::isShowing (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}




sal_Bool SAL_CALL AccessibleComponentBase::isVisible (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}




sal_Bool SAL_CALL AccessibleComponentBase::isFocusTraversable (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}




void SAL_CALL AccessibleComponentBase::addFocusListener (
    const ::com::sun::star::uno::Reference<
    ::com::sun::star::awt::XFocusListener >& xListener)
    throw (::com::sun::star::uno::RuntimeException)
{
    // emtpy
}




void SAL_CALL AccessibleComponentBase::removeFocusListener (const ::com::sun::star::uno::Reference<
    ::com::sun::star::awt::XFocusListener >& xListener )
    throw (::com::sun::star::uno::RuntimeException)
{
    // emtpy
}




void SAL_CALL AccessibleComponentBase::grabFocus (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    // emtpy
}




::com::sun::star::uno::Any SAL_CALL AccessibleComponentBase::getAccessibleKeyBinding (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return uno::Any();
}




//=====  XAccessibleExtendedComponent  ========================================

sal_Int32 SAL_CALL AccessibleComponentBase::getForeground (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    return 0;
}




sal_Int32 SAL_CALL AccessibleComponentBase::getBackground (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return -1;
}




::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont > SAL_CALL
        AccessibleComponentBase::getFont (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    return uno::Reference<awt::XFont>();
}




awt::FontDescriptor SAL_CALL
        AccessibleComponentBase::getFontMetrics (const uno::Reference<awt::XFont >& xFont)
        throw (::com::sun::star::uno::RuntimeException)
{
    awt::FontDescriptor aFontDescriptor;
    return aFontDescriptor;
}




sal_Bool SAL_CALL AccessibleComponentBase::isEnabled (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return sal_True;
}




::rtl::OUString SAL_CALL AccessibleComponentBase::getTitledBorderText (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    return ::rtl::OUString::createFromAscii ("Dummy Title Border Text");
}




::rtl::OUString SAL_CALL AccessibleComponentBase::getToolTipText (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return ::rtl::OUString::createFromAscii ("Dummy Tool Tip Text");
}




//=====  XTypeProvider  ===================================================

uno::Sequence<uno::Type> SAL_CALL
    AccessibleComponentBase::getTypes (void)
    throw (uno::RuntimeException)
{
    // Get list of types from the context base implementation...
    uno::Sequence<uno::Type> aTypeList (2);
    // ...and add the additional type for the component.
    const uno::Type aComponentType =
         ::getCppuType((const uno::Reference<XAccessibleComponent>*)0);
    const uno::Type aExtendedComponentType =
        ::getCppuType((const uno::Reference<XAccessibleExtendedComponent>*)0);
    aTypeList[0] = aComponentType;
    aTypeList[1] = aExtendedComponentType;

    return aTypeList;
}


} // end of namespace accessibility
