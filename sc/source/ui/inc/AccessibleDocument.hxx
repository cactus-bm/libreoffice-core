/*************************************************************************
 *
 *  $RCSfile: AccessibleDocument.hxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: sab $ $Date: 2002-03-22 16:26:36 $
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


#ifndef _SC_ACCESSIBLEDOCUMENT_HXX
#define _SC_ACCESSIBLEDOCUMENT_HXX

#ifndef _SC_ACCESSIBLEDOCUMENTBASE_HXX
#include "AccessibleDocumentBase.hxx"
#endif
#ifndef SC_VIEWDATA_HXX
#include "viewdata.hxx"
#endif

#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_SHAPE_TREE_INFO_HXX
#include <svx/AccessibleShapeTreeInfo.hxx>
#endif

class ScTabViewShell;
class ScAccessibleSpreadsheet;
class SdrPage;

namespace accessibility
{
    class AccessibleShape;
}

struct ScAccessibleShapeData
{
    ScAccessibleShapeData() : pAccShape(NULL), nIndex(-1) {}
    ScAccessibleShapeData(sal_Int32 nTempIndex) : pAccShape(NULL), nIndex(nTempIndex) {}
    accessibility::AccessibleShape* pAccShape;
    sal_Int32           nIndex; // Index in drawpage.
};

/** @descr
        This base class provides an implementation of the
        <code>AccessibleContext</code> service.
*/

class ScAccessibleDocument
    :   public ScAccessibleDocumentBase
{
public:
    //=====  internal  ========================================================
    ScAccessibleDocument(
        const ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible>& rxParent,
        ScTabViewShell* pViewShell,
        ScSplitPos eSplitPos);
protected:
    virtual ~ScAccessibleDocument(void);
public:

     virtual void SAL_CALL disposing();

   ///=====  SfxListener  =====================================================

    virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    ///=====  XAccessibleComponent  ============================================

    virtual ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible >
        SAL_CALL getAccessibleAt(
        const ::com::sun::star::awt::Point& rPoint )
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL grabFocus(  )
        throw (::com::sun::star::uno::RuntimeException);

    ///=====  XAccessibleContext  ==============================================

    /// Return the number of currently visible children.
    virtual long SAL_CALL
        getAccessibleChildCount(void)
        throw (::com::sun::star::uno::RuntimeException);

    /// Return the specified child or NULL if index is invalid.
    virtual ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleChild(long nIndex)
        throw (::com::sun::star::uno::RuntimeException,
                ::com::sun::star::lang::IndexOutOfBoundsException);

    /// Return the set of current states.
    virtual ::com::sun::star::uno::Reference<
            ::drafts::com::sun::star::accessibility::XAccessibleStateSet> SAL_CALL
        getAccessibleStateSet(void)
        throw (::com::sun::star::uno::RuntimeException);

    ///=====  XServiceInfo  ====================================================

    /** Returns an identifier for the implementation of this object.
    */
    virtual ::rtl::OUString SAL_CALL
        getImplementationName(void)
        throw (::com::sun::star::uno::RuntimeException);

    /** Returns a list of all supported services.
    */
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
        getSupportedServiceNames(void)
        throw (::com::sun::star::uno::RuntimeException);

    ///=====  XTypeProvider  ===================================================

    /** Returns a implementation id.
    */
    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
        getImplementationId(void)
        throw (::com::sun::star::uno::RuntimeException);

protected:
    /// Return this object's description.
    virtual ::rtl::OUString SAL_CALL
        createAccessibleDescription(void)
        throw (::com::sun::star::uno::RuntimeException);

    /// Return the object's current name.
    virtual ::rtl::OUString SAL_CALL
        createAccessibleName(void)
        throw (::com::sun::star::uno::RuntimeException);

    /// Return the object's current bounding box relative to the desktop.
    virtual Rectangle GetBoundingBoxOnScreen(void)
        throw (::com::sun::star::uno::RuntimeException);

    /// Return the object's current bounding box relative to the parent object.
    virtual Rectangle GetBoundingBox(void)
        throw (::com::sun::star::uno::RuntimeException);

private:
    ScTabViewShell* mpViewShell;
    ScSplitPos      meSplitPos;
    ScAccessibleSpreadsheet* mpAccessibleSpreadsheet;
    std::vector<ScAccessibleShapeData> maShapes;
    accessibility::AccessibleShapeTreeInfo maShapeTreeInfo;

    sal_uInt16 getVisibleTable();

    ::com::sun::star::uno::Reference
        < ::drafts::com::sun::star::accessibility::XAccessible >
        GetAccessibleSpreadsheet();

    void FreeAccessibleSpreadsheet();

    SdrPage* GetDrawPage();
    sal_Int32 GetShapesCount();
    com::sun::star::uno::Reference< drafts::com::sun::star::accessibility::XAccessible >
            GetShape(sal_Int32 nIndex);

    sal_Bool IsDefunc(
        const com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessibleStateSet>& rxParentStates);
    sal_Bool IsEditable(
        const com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessibleStateSet>& rxParentStates);
};


#endif
