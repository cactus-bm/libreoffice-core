/*************************************************************************
 *
 *  $RCSfile: ChildrenManagerImpl.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: af $ $Date: 2002-05-21 14:33:43 $
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

#ifndef _SVX_ACCESSIBILITY_CHILDREN_MANAGER_IMPL_HXX

#ifndef _SVX_ACCESSIBILITY_IACCESSIBLE_VIEW_FORWARDER_LISTENER_HXX
#include "IAccessibleViewForwarderListener.hxx"
#endif
#ifndef _SVX_ACCESSIBILITY_IACCESSIBLE_PARENT_HXX
#include "IAccessibleParent.hxx"
#endif
#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_SHAPE_TREE_INFO_HXX
#include "AccessibleShapeTreeInfo.hxx"
#endif
#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_CONTEXT_BASE_HXX
#include "AccessibleContextBase.hxx"
#endif

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#include <vos/mutex.hxx>
#include <vector>
#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPES_HPP_
#include <com/sun/star/drawing/XShapes.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTLISTENER_HPP_
#include <com/sun/star/document/XEventListener.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XSELECTIONCHANGELISTENER_HPP_
#include <com/sun/star/view/XSelectionChangeListener.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBLE_XACCESSIBLE_HPP_
#include <drafts/com/sun/star/accessibility/XAccessible.hpp>
#endif
#include <vector>


using namespace ::rtl;
using namespace ::com::sun::star;

namespace accessibility {

class AccessibleShape;

class ChildDescriptor; // See below for declaration.
class CMShapeIterator;


/** This class contains the actual implementation of the children manager.

    <p>It maintains a set of visible accessible shapes in
    <member>maVisibleChildren</member>.  The objects in this list stem from
    two sources.  The first is a list of UNO shapes like the list of shapes
    in a draw page.  A reference to this list is held in
    <member>maShapeList</member>.  Accessible objects for these shapes are
    created on demand.  The list can be replaced by calls to the
    <member>SetShapeList</member> method.  The second source is a list of
    already accessible objects.  It can be modified by calls to the
    <member>AddAccessibleShape</member> and
    <member>ClearAccessibleShapeList</member> methods.</p>

    <p>Each call of the <member>Update</member> method leads to a
    re-calculation of the visible shapes which then can be queried with the
    <member>GetChildCount</member> and <member>GetChild</member> methods.
    Events are send informing all listeners about the removed shapes which are
    not visible anymore and about the added shapes.</p>

    <p> The visible area which is used to determine the visibility of the
    shapes is taken from the view forwarder.  Thus, to signal a change of
    the visible area call <member>ViewForwarderChanged</member>.</p>

    @see ChildrenManager
*/
class ChildrenManagerImpl
    :   public cppu::WeakImplHelper2<
            ::com::sun::star::document::XEventListener,
            ::com::sun::star::view::XSelectionChangeListener>,
        public IAccessibleViewForwarderListener,
        public IAccessibleParent
{
public:
    /** Create a children manager, which manages the children of the given
        parent.  The parent is used for creating accessible objects.  The
        list of shapes for which to create those objects is not derived from
        the parent and has to be provided seperately by calling one of the
        update methods.
        @param rxParent
            The parent of the accessible objects which will be created
            on demand at some point of time in the future.
        @param rxShapeList
            List of UNO shapes to manage.
        @param rShapeTreeInfo
            Bundel of information passed down the shape tree.
        @param rContext
            An accessible context object that is called for fireing events
            for new and deleted children, i.e. that holds a list of
            listeners to be informed.
    */
    ChildrenManagerImpl (const ::com::sun::star::uno::Reference<
            ::drafts::com::sun::star::accessibility::XAccessible>& rxParent,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::XShapes>& rxShapeList,
        const AccessibleShapeTreeInfo& rShapeTreeInfo,
        AccessibleContextBase& rContext);

    /** If there still are managed children these are disposed and
        released.
    */
    ~ChildrenManagerImpl (void);

    /** Do that part of the initialization that you can not or should not do
        in the constructor like registering at broadcasters.
    */
    void Init (void);

    /** Return the number of currently visible accessible children.
        @return
            If there are no children a 0 is returned.
    */
    long GetChildCount (void) const throw ();

    /** Return the requested accessible child or throw and
        IndexOutOfBoundsException if the given index is invalid.
        @param nIndex
            Index of the requested child.  Call getChildCount for obtaining
            the number of children.
        @return
            In case of a valid index this method returns a reference to the
            requested accessible child.  This reference is empty if it has
            not been possible to create the accessible object of the
            corresponding shape.
        @raises
            Throws an IndexOutOfBoundsException if the index is not valid.
    */
    ::com::sun::star::uno::Reference<
            ::drafts::com::sun::star::accessibility::XAccessible>
        GetChild (long nIndex)
        throw (::com::sun::star::uno::RuntimeException);

    /** Return the requested accessible child.
        @param aChildDescriptor
            This object contains references to the original shape and its
            associated accessible object.
        @return
            Returns a reference to the requested accessible child.  This
            reference is empty if it has not been possible to create the
            accessible object of the corresponding shape.
    */
    ::com::sun::star::uno::Reference<
            ::drafts::com::sun::star::accessibility::XAccessible>
        GetChild (ChildDescriptor& aChildDescriptor)
        throw (::com::sun::star::uno::RuntimeException);

    /** Return the requested accessible child given a shape.  This method
        searches the list of descriptors for the one that holds the
        association of the given shape to the requested accessible object
        and returns that.  If no such descriptor is found that is
        interpreted so that the specified shape is not visible at the moment.
        @param xShape
            The shape for which to return the associated accessible object.
        @return
            Returns a reference to the requested accessible child.  The
            reference is empty if there is no shape descriptor that
            associates the shape with an accessible object.
    */
    ::com::sun::star::uno::Reference<
            ::drafts::com::sun::star::accessibility::XAccessible>
        GetChild (const ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::XShape>& xShape)
        throw (::com::sun::star::uno::RuntimeException);

    /** Update the child manager.  Take care of a modified set of children
        and modified visible area.  This method can optimize the update
        process with respect seperate updates of a modified children list
        and visible area.
        @param bCreateNewObjectsOnDemand
            If </true> then accessible objects associated with the visible
            shapes are created only when asked for.  No event is sent on
            creation.  If </false> then the accessible objects are created
            before this method returns and events are sent to inform the
            listeners of the new object.
    */
    void Update (bool bCreateNewObjectsOnDemand = true);

    /** Set the list of UNO shapes to the given list.  This removes the old
        list and does not add to it.  The list of accessible shapes that is
        build up by calls to <member>AddAccessibleShape</member> is not
        modified.  Neither is the list of visible children.  Accessible
        objects are created on demand.
        @param xShapeList
            The list of UNO shapes that replaces the old list.
    */
    void SetShapeList (const ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShapes>& xShapeList);

    /** Add a accessible shape.  This does not modify the list of UNO shapes
        or the list of visible shapes.  Accessible shapes are, at the
        moment, not tested against the visible area but are always appended
        to the list of visible children.
        @param pShape
            The new shape that is added to the list of accessible shapes.
    */
    void AddAccessibleShape (std::auto_ptr<AccessibleShape> pShape);

    /** Clear the lists of accessible shapes and that of visible accessible
        shapes.  The list of UNO shapes is not modified.
    */
    void ClearAccessibleShapeList (void);

    /** Set a new event shape tree info.  Call this method to inform the
        children manager of a change of the info bundle.
        @param rShapeTreeInfo
            The new info that replaces the current one.
    */
    void SetInfo (const AccessibleShapeTreeInfo& rShapeTreeInfo);

    /** Update the SELECTED state of all visible children according to the
        given selection.  This includes setting <em>and</em> resetting the
        state.
    */
    void UpdateSelection (void);

    //=====  lang::XEventListener  ============================================

    virtual void SAL_CALL
        disposing (const ::com::sun::star::lang::EventObject& rEventObject)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  document::XEventListener  ========================================

    virtual void SAL_CALL
        notifyEvent (const ::com::sun::star::document::EventObject& rEventObject)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  view::XSelectionChangeListener  ==================================

    virtual void  SAL_CALL
        selectionChanged (const ::com::sun::star::lang::EventObject& rEvent)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  IAccessibleViewForwarderListener  ================================

    /** Informs this children manager and its children about a change of one
        (or more) aspect of the view forwarder.
        @param aChangeType
            A change type of <const>VISIBLE_AREA</const> leads to a call to
            the <member>Update</memeber> which creates accessible objects of
            new shapes immediately.  Other change types are passed to the
            visible accessible children without calling
            <member>Update</memeber>.
        @param pViewForwarder
            The modified view forwarder.  Use this one from now on.
    */
    virtual void ViewForwarderChanged (ChangeType aChangeType,
        const IAccessibleViewForwarder* pViewForwarder);

    //=====  IAccessibleParent  ===============================================

    /** Replace the specified child with a replacement.
        @param pCurrentChild
            This child is to be replaced.
        @param pReplacement
            The replacement for the current child.
        @return
            The returned value indicates wether the replacement has been
            finished successfully.
    */
    virtual sal_Bool ReplaceChild (
        AccessibleShape* pCurrentChild,
        AccessibleShape* pReplacement)
        throw (::com::sun::star::uno::RuntimeException);


protected:
    /// Mutex guarding objects of this class.
    mutable ::vos::OMutex maMutex;

    /** This list holds the descriptors of all currently visible shapes and
        associated accessible object.

        <p>With the descriptors it maintains a mapping of shapes to
        accessible objects.  It acts as a cache in that accessible objects
        are only created on demand and released with every update (where the
        latter may be optimized by the update methods).<p>

        <p>The list is realized as a vector because it remains unchanged
        between updates (i.e. complete rebuilds of the list) and allows a
        fast (constant time) access to its elements for given indices.</p>
    */
    typedef ::std::vector<ChildDescriptor> ChildDescriptorListType;
    ChildDescriptorListType maVisibleChildren;

    /** The original list of UNO shapes.  The visible shapes are inserted
        into the list of visible children
        <member>maVisibleChildren</member>.
    */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShapes> mxShapeList;

    /** This list of additional accessible shapes that can or shall not be
        created by the shape factory.
    */
    typedef std::vector< ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible> > AccessibleShapeList;
    AccessibleShapeList maAccessibleShapes;

    /** Rectangle that describes the visible area in which a shape has to lie
        at least partly, to be accessible through this class.  Used to
        detect changes of the visible area after changes of the view forwarder.
    */
    Rectangle maVisibleArea;

    /** The parent of the shapes.  It is used for creating accessible
        objects for given shapes.
    */
    ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible> mxParent;

    /** Bundel of information passed down the shape tree.
    */
    AccessibleShapeTreeInfo maShapeTreeInfo;

    /** Reference to an accessible context object that is used to inform its
        listeners of new and remved children.
    */
    AccessibleContextBase& mrContext;

    /** Experimental: Get the index of the specified accessible object with
        respect to the list of children maintained by this object.

        @return
            Return the index of the given child or -1 to indicate that the
            child is unknown.
    */
    long GetChildIndex (const ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible>& xChild) const
        throw (::com::sun::star::uno::RuntimeException);

private:
    // Don't use the copy constructor or the assignment operator.  They are
    // not implemented (and are not intended to be).
    ChildrenManagerImpl (const ChildrenManagerImpl&);
    ChildrenManagerImpl& operator= (const ChildrenManagerImpl&);

    /** Three helper functions for the <member>Update</member> method.
    */

    /** Create a list of visible shapes from the list of UNO shapes
        <member>maShapeList</member> and the list of accessible objects.
        @param raChildList
            For every visible shape from the two sources mentioned above one
            descriptor is added to this list.
    */
    void CreateListOfVisibleShapes (ChildDescriptorListType& raChildList);

    /** From the internal list of (former) visible shapes remove those that
        are not member of the given list.  Send appropriate events for every
        such shape.
        @param raChildList
            The new list of visible children against which the internal one
            is compared.
    */
    void RemoveNonVisibleChildren (ChildDescriptorListType& raChildList);

    /** Merge the information that is already known about the visible shapes
        from the current list into the new list.
        @param raChildList
            Information is merged from the current list of visible children
            to this list.
    */
    void MergeAccessibilityInformation (ChildDescriptorListType& raChildList);

    /** If the visible area has changed then send events that signal a
        change of their bounding boxes for all shapes that are members of
        both the current and the new list of visible shapes.
        @param raChildList
            Events are sent to all entries of this list that already contain
            an accessible object.
    */
    void SendVisibleAreaEvents (ChildDescriptorListType& raChildList);

    /** If children have to be created immediately and not on demand the
        create the missing accessible objects now.
        @param raDescriptorList
            Create an accessible object for every member of this list where
            that obejct does not already exist.
    */
    void CreateAccessibilityObjects (ChildDescriptorListType& raChildList);
};




/** Pair for each visible shape, that contains a reference to the shape a
    reference to the corresponding accessible object.  The accessible object
    is only created on demand and is initially empty.
*/
class ChildDescriptor
{
public:
    /** Reference to a (partially) visible shape.
    */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape> mxShape;

    /** The corresponding accessible object.  This reference is initially
        empty and only replaced by a reference to a new object when that is
        requested from the outside.
    */
    ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible> mxAccessibleShape;

    /** Return a pointer to the implementation object of the accessible
        shape of this descriptor.
        @return
            The result is NULL if either the UNO reference to the accessible
            shape is empty or it can not be transformed into a pointer to
            the desired class.
    */
    AccessibleShape* GetAccessibleShape (void) const;

    /** This flag is set during the visibility calculation and indicates
        that at one time in this process an event is sent that informs the
        listners of the creation of a new accessible object.  This flags is
        not reset afterwards.  Don't use it unless you know exactly what you
        are doing.
    */
    bool mbCreateEventPending;

    /** Create a new descriptor for the specified shape with empty reference
        to accessible object.
    */
    explicit ChildDescriptor (const ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape>& xShape);

    /** Create a new descriptor for the specified shape with empty reference
        to the original shape.
    */
    explicit ChildDescriptor (const ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible>& rxAccessibleShape);


    bool operator == (const ChildDescriptor& aDescriptor);
    bool operator < (const ChildDescriptor& aDescriptor);

};



} // end of namespace accessibility

#endif
