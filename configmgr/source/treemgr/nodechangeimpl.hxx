/*************************************************************************
 *
 *  $RCSfile: nodechangeimpl.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: jb $ $Date: 2001-07-05 17:05:51 $
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

#ifndef CONFIGMGR_CONFIGCHANGEIMPL_HXX_
#define CONFIGMGR_CONFIGCHANGEIMPL_HXX_

#include "apitypes.hxx"
#include "configexcept.hxx"
#include "configpath.hxx"
#include "treeimpl.hxx"

#include <vos/ref.hxx>

namespace configmgr
{
    namespace configuration
    {
//-----------------------------------------------------------------------------

        typedef com::sun::star::uno::Type       UnoType;
        typedef com::sun::star::uno::Any        UnoAny;
//-----------------------------------------------------------------------------

        typedef vos::ORef<TreeImpl>         TreeHolder;
        typedef vos::ORef<ElementTreeImpl>  ElementTreeHolder;
//-----------------------------------------------------------------------------

        class Node;
        class ValueMemberNode;
        class ValueMemberUpdate;
        class SetNodeImpl;
//-----------------------------------------------------------------------------
        class NodeChangeData;
        class NodeChangeLocation;
        class NodeChangeInformation;


        /// represents a node position in some tree
        class NodeChangeImpl
        : public vos::OReference
        {
        public:
            explicit
            NodeChangeImpl(bool bNoCheck = false);

        public:
        // related/affected nodes and trees
            /// the tree on which the operation originated
            TreeHolder getBaseTree() const; // default is the changing tree
            /// the node thru which the operation originated
            NodeOffset getBaseNode() const; // default is the owning node

            /// the tree within which the change occurs
            TreeHolder getAffectedTree() const;
            /// the node that is affected by the change
            NodeOffset getAffectedNode() const;

            /// the path from the base to the changing node
            RelativePath getPathToChangingNode() const;

        protected:
            /// setup the 'target' node that is to be affected or changed
            void setAffected(TreeHolder const& aAffectedTree, NodeOffset nAffectedNode);

        public:
        // related/affected nodes and trees
            /// checks, if this represents an actual change - with or without requiring a preceding test
            bool isChange(bool bAllowUntested) const;

            /// fills in pre- and post-change values, returns whether they may differ
            bool fillChangeData(NodeChangeData& rChange) const;

            /// fills in change location, returns whether it is set
            bool fillChangeLocation(NodeChangeLocation& rChange) const;

            /// fills in change location and values, returns whether data may be changed
            bool fillChangeInfo(NodeChangeInformation& rChange) const;

        /// test whether this really is a change to the stored 'changing' node
            void test();

        /// apply this change to the stored 'changing' node
            void apply();

        protected:
        /// virtual hooks for some of the public methods
            /// the tree on which the operation originated
            virtual TreeHolder doGetBaseTree() const; // default is the same as the target tree

            /// the node thru which the operation originated
            virtual NodeOffset doGetBaseNode() const; // default is the the same as the owning node
        private:
            /// the path from base to 'affected' node
            virtual RelativePath doGetChangingNodePath() const = 0;

            /// is the change really affecting a child of the affected node (true for values)
            virtual bool doIsChangingSubnode() const = 0;

        private:
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const = 0;

            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeData& rChange) const = 0;

            /// dry-check whether this is a change
            virtual void doTest( Node& rTarget) = 0;
            /// do apply the actual change
            virtual void doApply( Node& rTarget) = 0;
        private:
            typedef sal_uInt16 State;
            TreeHolder m_aAffectedTree;
            NodeOffset m_nAffectedNode;
            State      m_nState;

            void implApply();
            Node* implGetTarget() const;
        };
//-----------------------------------------------------------------------------

        /// represents a node position in some tree
        class ValueChangeImpl
        : public NodeChangeImpl
        {
            Name   m_aName;
            UnoAny m_aNewValue;
            UnoAny m_aOldValue;
        public:
            explicit ValueChangeImpl();
            explicit ValueChangeImpl(UnoAny const& aNewValue);
            explicit ValueChangeImpl(UnoAny const& aNewValue, UnoAny const& aOldValue);
            ~ValueChangeImpl();

        public:
            /// setup the 'target' node that is to be affected or changed
            void setTarget(TreeHolder const& aAffectedTree, NodeOffset nParentNode, Name const& sNodeName);

        public:
            /// get the name of the value
            Name getValueName() const { return m_aName; }

            /// get the pre-change value (if known)
            UnoAny getOldValue() const { return m_aOldValue; }
            /// get the post-change value (if known)
            UnoAny getNewValue() const { return m_aNewValue; }

        protected:
        // override information items
            /// the path from base to 'affected' node - here is the name of the changing node
            virtual RelativePath doGetChangingNodePath() const;

            /// is the change really affecting a child of the affected node (true here)
            virtual bool doIsChangingSubnode() const;

        protected:
        // override change information items
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const;

            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeData& rChange) const = 0;

        protected:
        // override apply functionality
            /// retrieve the old value from the given node
            virtual void doTest( Node& rTarget);
            /// do apply the actual change
            virtual void doApply( Node& rTarget);

        protected:
        // new overrideables
            /// extract the pre-change value from the target context
            virtual void preCheckValue(ValueMemberNode& rNode, UnoAny& rOld, UnoAny& rNew);
            /// extract the post-change value from the target context
            virtual void postCheckValue(ValueMemberNode& rNode, UnoAny& rNew);
            /// apply the new value to the target context
            virtual void doApplyChange(ValueMemberUpdate& rNode) = 0;
        };
//-----------------------------------------------------------------------------

        /// represents setting a value node to a given value
        class ValueReplaceImpl
        : public ValueChangeImpl
        {
        public:
            explicit ValueReplaceImpl(UnoAny const& aNewValue);
            explicit ValueReplaceImpl(UnoAny const& aNewValue, UnoAny const& aOldValue);

        protected:
            // implement: set the target to the new value
            virtual void doApplyChange( ValueMemberUpdate& rNode);

            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeData& rChange) const;

//          friend class SetReplaceValueImpl;
        };
//-----------------------------------------------------------------------------

        /// represents resetting a value node to its default value
        class ValueResetImpl
        : public ValueChangeImpl
        {
        public:
            explicit ValueResetImpl();
            explicit ValueResetImpl(UnoAny const& aNewValue, UnoAny const& aOldValue);

        protected:
            // override: set the new value as well
            virtual void preCheckValue(ValueMemberNode& rNode, UnoAny& rOld, UnoAny& rNew);

            // implement: set the target to default
            virtual void doApplyChange( ValueMemberUpdate& rNode);

            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeData& rChange) const;
        };
//-----------------------------------------------------------------------------

        /// represents setting a deeply nested value node to a given value
        class DeepValueReplaceImpl
        : public ValueReplaceImpl
        {
            TreeHolder m_aBaseTree;
            NodeOffset m_nBaseNode;
            RelativePath m_aNestedPath;
        public:
            explicit DeepValueReplaceImpl(RelativePath const& aNestedPath, UnoAny const& aNewValue);
            explicit DeepValueReplaceImpl(RelativePath const& aNestedPath, UnoAny const& aNewValue, UnoAny const& aOldValue);

            /// set up the node where this is originated
            void setBaseContext(TreeHolder const& aBaseTree, NodeOffset nBaseNode);

        protected:
            /// the path from base to 'affected' node - uses own (explicit) path
            virtual RelativePath doGetChangingNodePath() const;

            /// the tree on which the operation originated
            virtual TreeHolder doGetBaseTree() const; // uses own
            /// the node thru which the operation originated
            virtual NodeOffset doGetBaseNode() const; // uses own
        };
//-----------------------------------------------------------------------------

        /// represents a change to a set (as a container)
        class SetChangeImpl
        : public NodeChangeImpl
        {
            Path::Component m_aName;
        public:
            explicit SetChangeImpl(Path::Component const& aName, bool bNoCheck = false);

            /// the name of the element being changed
            Path::Component getFullElementName() const { return m_aName; }

            /// the name of the element being changed
            Name getElementName() const { return m_aName.getName(); }

            /// setup the 'target' node that is to be affected or changed
            void setTarget(TreeHolder const& aAffectedTree, NodeOffset nAffectedNode);
        protected:
        /// virtual hooks for some of the public methods
            /// the path from base to 'affected' node - use element name
            virtual RelativePath doGetChangingNodePath() const;

            /// is the change really affecting a child of the affected node (false here)
            virtual bool doIsChangingSubnode() const;

            /// retrieve the old value from the given node
            virtual void doTest( Node& rTarget);
            /// do apply the actual change
            virtual void doApply( Node& rTarget);

        private:
            /// new overridable: retrieve the old value from a properly typed node
            virtual void doTestElement(SetNodeImpl& rNode, Name const& aName) = 0;
            /// new overridable: apply the change to a properly typed node
            virtual void doApplyToElement(SetNodeImpl& rNode, Name const& aName) = 0;
        };
//-----------------------------------------------------------------------------

        /// represents an insertion into a set of trees
        class SetInsertTreeImpl
        : public SetChangeImpl
        {
            ElementTreeHolder m_aNewTree;
        public:
            explicit SetInsertTreeImpl(Path::Component const& aName, ElementTreeHolder const& aNewTree, bool bNoCheck = false);

        protected:
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const;
            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeData& rChange) const;

            /// new overridable: retrieve the old value from a properly typed node
            virtual void doTestElement(SetNodeImpl& rNode, Name const& aName);
            /// new overridable: apply the change to a properly typed node
            virtual void doApplyToElement(SetNodeImpl& rNode, Name const& aName);
        };
//-----------------------------------------------------------------------------

        /// represents a substitution within a set of trees
        class SetReplaceTreeImpl
        : public SetChangeImpl
        {
            ElementTreeHolder m_aNewTree;
            ElementTreeHolder m_aOldTree;
        public:
            explicit SetReplaceTreeImpl(Path::Component const& aName, ElementTreeHolder const& aNewTree);
            explicit SetReplaceTreeImpl(Path::Component const& aName, ElementTreeHolder const& aNewTree, ElementTreeHolder const& aOldTree);

        protected:
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const;
            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeData& rChange) const;

            /// new overridable: retrieve the old value from a properly typed node
            virtual void doTestElement(SetNodeImpl& rNode, Name const& aName);
            /// new overridable: apply the change to a properly typed node
            virtual void doApplyToElement(SetNodeImpl& rNode, Name const& aName);
        };
//-----------------------------------------------------------------------------

        /// represents a removal from of a set of trees
        class SetRemoveTreeImpl
        : public SetChangeImpl
        {
            ElementTreeHolder m_aOldTree;
        public:
            explicit SetRemoveTreeImpl(Path::Component const& aName);
            explicit SetRemoveTreeImpl(Path::Component const& aName, ElementTreeHolder const& aOldTree);

        protected:
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const;
            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeData& rChange) const;

            /// new overridable: retrieve the old value from a properly typed node
            virtual void doTestElement(SetNodeImpl& rNode, Name const& aName);
            /// new overridable: apply the change to a properly typed node
            virtual void doApplyToElement(SetNodeImpl& rNode, Name const& aName);
        };
//-----------------------------------------------------------------------------
        typedef SetInsertTreeImpl   SetInsertValueImpl;
        typedef SetReplaceTreeImpl  SetReplaceValueImpl;
        typedef SetRemoveTreeImpl   SetRemoveValueImpl;
/*
        /// represents an insertion into a set of values
        class SetInsertValueImpl
        : public SetChangeImpl
        {
            UnoAny  m_aNewValue;
        public:
            explicit SetInsertValueImpl(Name const& aName, UnoAny const& aNewValue);

            /// get the post-change value (if known)
            UnoAny getNewValue() const { return m_aNewValue; }

        protected:
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const;
            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeInfo& rChange) const;

            /// new overridable: retrieve the old value from a properly typed node
            virtual void doTestElement(SetNodeImpl& rNode, Name const& aName);
            /// new overridable: apply the change to a properly typed node
            virtual void doApplyToElement(SetNodeImpl& rNode, Name const& aName);
        };
//-----------------------------------------------------------------------------

        /// represents a substitution within a set of trees
        class SetReplaceValueImpl
        : public SetChangeImpl
        {
            ElementTreeHolder   m_aOldTree;
            UnoAny  m_aNewValue;
        public:
            explicit SetReplaceValueImpl(Name const& aName, UnoAny const& aNewValue);

            /// get the post-change value (if known)
            UnoAny getNewValue() const { return m_aNewValue; }
            /// get the pre-change value (if known)
            UnoAny getOldValue() const;

        protected:
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const;
            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeInfo& rChange) const;

            /// new overridable: retrieve the old value from a properly typed node
            virtual void doTestElement(SetNodeImpl& rNode, Name const& aName);
            /// new overridable: apply the change to a properly typed node
            virtual void doApplyToElement(SetNodeImpl& rNode, Name const& aName);
        };

//-----------------------------------------------------------------------------

        /// represents a removal from of a set of values
        class SetRemoveValueImpl
        : public SetChangeImpl
        {
            ElementTreeHolder   m_aOldTree;
        public:
            explicit SetRemoveValueImpl(Name const& aName);

            /// get the pre-change value (if known)
            UnoAny getOldValue() const;

        protected:
            /// checks, if this represents an actual change (given whether the change has been applied or not)
            virtual bool doIsChange(bool bBefore) const;
            /// fills in pre- and post-change values, returns wether they differ
            virtual bool doFillChange(NodeChangeInfo& rChange) const;

            /// new overridable: retrieve the old value from a properly typed node
            virtual void doTestElement(SetNodeImpl& rNode, Name const& aName);
            /// new overridable: apply the change to a properly typed node
            virtual void doApplyToElement(SetNodeImpl& rNode, Name const& aName);
        };
*/
//-----------------------------------------------------------------------------
    }
}

#endif // CONFIGMGR_CONFIGCHANGEIMPL_HXX_
