/*************************************************************************
 *
 *  $RCSfile: AccessibleOLEShape.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: af $ $Date: 2002-04-22 14:32:17 $
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



#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_OLE_SHAPE_HXX
#include "AccessibleOLEShape.hxx"
#endif

#include "ShapeTypeHandler.hxx"
#include "SvxShapeTypes.hxx"

using namespace accessibility;
using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::accessibility;

//=====  internal  ============================================================

AccessibleOLEShape::AccessibleOLEShape (const ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape>& rxShape,
    const ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible>& rxParent,
    const AccessibleShapeTreeInfo& rShapeTreeInfo,
    long nIndex)
    :      AccessibleShape (rxShape, rxParent, rShapeTreeInfo, nIndex)
{
}




AccessibleOLEShape::~AccessibleOLEShape (void)
{
}




//=====  XAccessibleAction  ===================================================

sal_Int32 SAL_CALL AccessibleOLEShape::getAccessibleActionCount (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return 0;
}




sal_Bool SAL_CALL AccessibleOLEShape::doAccessibleAction (sal_Int32 nIndex)
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    throw lang::IndexOutOfBoundsException();
}




::rtl::OUString SAL_CALL AccessibleOLEShape::getAccessibleActionDescription (sal_Int32 nIndex)
    throw (::com::sun::star::lang::IndexOutOfBoundsException,
        ::com::sun::star::uno::RuntimeException)
{
    throw lang::IndexOutOfBoundsException();
}




::com::sun::star::uno::Any SAL_CALL AccessibleOLEShape::getAccessibleKeyBinding (sal_Int32 nIndex)
    throw (::com::sun::star::lang::IndexOutOfBoundsException,
        ::com::sun::star::uno::RuntimeException)
{
    throw lang::IndexOutOfBoundsException();
}




//=====  XInterface  ==========================================================

com::sun::star::uno::Any SAL_CALL
    AccessibleOLEShape::queryInterface (const com::sun::star::uno::Type & rType)
    throw (::com::sun::star::uno::RuntimeException)
{
    ::com::sun::star::uno::Any aReturn = AccessibleShape::queryInterface (rType);
    if ( ! aReturn.hasValue())
        aReturn = ::cppu::queryInterface (rType,
            static_cast<XAccessibleAction*>(this));
    return aReturn;
}



void SAL_CALL
    AccessibleOLEShape::acquire (void)
    throw ()
{
    AccessibleShape::acquire ();
}



void SAL_CALL
    AccessibleOLEShape::release (void)
    throw ()
{
    AccessibleShape::release ();
}




//=====  XServiceInfo  ========================================================

::rtl::OUString SAL_CALL
    AccessibleOLEShape::getImplementationName (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AccessibleOLEShape"));
}




//=====  XTypeProvider  ===================================================

uno::Sequence<uno::Type> SAL_CALL
    AccessibleOLEShape::getTypes (void)
    throw (uno::RuntimeException)
{
    // Get list of types from the context base implementation...
    uno::Sequence<uno::Type> aTypeList (AccessibleShape::getTypes());
    // ...and add the additional type for the component.
    long nTypeCount = aTypeList.getLength();
    aTypeList.realloc (nTypeCount + 1);
    const uno::Type aActionType =
        ::getCppuType((const uno::Reference<XAccessibleAction>*)0);
    aTypeList[nTypeCount] = aActionType;

    return aTypeList;
}




/// Set this object's name if is different to the current name.
::rtl::OUString
    AccessibleOLEShape::CreateAccessibleBaseName (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ::rtl::OUString sName;

    ShapeTypeId nShapeType = ShapeTypeHandler::Instance().GetTypeId (mxShape);
    switch (nShapeType)
    {
        case DRAWING_APPLET:
            sName = ::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM("AppletOLEShape"));
            break;
        case DRAWING_FRAME:
            sName = ::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM("FrameOLEShape"));
            break;
        case DRAWING_OLE:
            sName = ::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM("OLEShape"));
            break;
        case DRAWING_PLUGIN:
            sName = ::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM("PluginOLEShape"));
            break;

        default:
            sName = ::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM("UnknownAccessibleOLEShape"));
            uno::Reference<drawing::XShapeDescriptor> xDescriptor (mxShape, uno::UNO_QUERY);
            if (xDescriptor.is())
                sName += ::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM(": "))
                    + xDescriptor->getShapeType();
    }

    return sName;
}



::rtl::OUString
    AccessibleOLEShape::CreateAccessibleDescription (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return CreateAccessibleName ();
}


