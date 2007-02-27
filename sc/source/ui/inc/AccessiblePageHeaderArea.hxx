/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: AccessiblePageHeaderArea.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2007-02-27 13:17:47 $
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


#ifndef _SC_ACCESSIBLEPAGEHEADERAREA_HXX
#define _SC_ACCESSIBLEPAGEHEADERAREA_HXX

#ifndef _SC_ACCESSIBLE_CONTEXT_BASE_HXX
#include "AccessibleContextBase.hxx"
#endif
#ifndef _SVX_SVXENUM_HXX
#include <svx/svxenum.hxx>
#endif

class EditTextObject;
namespace accessibility
{
    class AccessibleTextHelper;
}
class ScPreviewShell;

class ScAccessiblePageHeaderArea
    :   public ScAccessibleContextBase
{
public:
    //=====  internal  ========================================================
    ScAccessiblePageHeaderArea(
        const ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible>& rxParent,
        ScPreviewShell* pViewShell,
        const EditTextObject* pEditObj,
        sal_Bool bHeader,
        SvxAdjust eAdjust);
protected:
    virtual ~ScAccessiblePageHeaderArea (void);
public:
    const EditTextObject* GetEditTextObject() const { return mpEditObj; }

    using ScAccessibleContextBase::disposing;
    virtual void SAL_CALL disposing();

   ///=====  SfxListener  =====================================================

    virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    ///=====  XAccessibleComponent  ============================================

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >
        SAL_CALL getAccessibleAtPoint(
        const ::com::sun::star::awt::Point& rPoint )
        throw (::com::sun::star::uno::RuntimeException);

    ///=====  XAccessibleContext  ==============================================

    /// Return the number of currently visible children.
    // is overloaded to calculate this on demand
    virtual sal_Int32 SAL_CALL
        getAccessibleChildCount(void)
                    throw (::com::sun::star::uno::RuntimeException);

    /// Return the specified child or NULL if index is invalid.
    // is overloaded to calculate this on demand
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleChild(sal_Int32 nIndex)
        throw (::com::sun::star::uno::RuntimeException,
                ::com::sun::star::lang::IndexOutOfBoundsException);

    /// Return the set of current states.
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleStateSet> SAL_CALL
        getAccessibleStateSet(void)
        throw (::com::sun::star::uno::RuntimeException);

    ///=====  XServiceInfo  ====================================================

    /** Returns an identifier for the implementation of this object.
    */
    virtual ::rtl::OUString SAL_CALL
        getImplementationName(void)
        throw (::com::sun::star::uno::RuntimeException);

    /** Returns a list of all supported services.  In this case that is just
        the AccessibleContext and Accessible service.
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
    virtual ::rtl::OUString SAL_CALL createAccessibleDescription(void) throw(::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL createAccessibleName(void) throw (::com::sun::star::uno::RuntimeException);

    virtual Rectangle GetBoundingBoxOnScreen(void) const throw(::com::sun::star::uno::RuntimeException);
    virtual Rectangle GetBoundingBox(void) const throw (::com::sun::star::uno::RuntimeException);

private:
    EditTextObject* mpEditObj;
    accessibility::AccessibleTextHelper* mpTextHelper;
    ScPreviewShell* mpViewShell;
    sal_Bool mbHeader;
    SvxAdjust meAdjust;

    void CreateTextHelper();
};


#endif

