/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewstrategy.hxx,v $
 * $Revision: 1.8 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef CONFIGMGR_VIEWBEHAVIOR_HXX_
#define CONFIGMGR_VIEWBEHAVIOR_HXX_

#include "viewnode.hxx"
#include "groupnodeimpl.hxx"
#include "setnodeimpl.hxx"
#include "utility.hxx"
#include <rtl/ref.hxx>

namespace configmgr
{
//-----------------------------------------------------------------------------
    namespace configuration
    {
        class SetElementChangeImpl;
        class ValueChangeImpl;
    }
//-----------------------------------------------------------------------------
    namespace view
    {
//-----------------------------------------------------------------------------
        struct NodeFactory;
//-----------------------------------------------------------------------------
        using configuration::Name;
        using configuration::NodeOffset;
        using configuration::TreeDepth;

        typedef com::sun::star::uno::Any    UnoAny;
        typedef com::sun::star::uno::Type   UnoType;
//-----------------------------------------------------------------------------
        class ViewStrategy : public configmgr::SimpleReferenceObject
        {
        // node attributes
        public:
            /// retrieve the attributes of the node
            Name getName(Node const& _aNode)    const;

            /// retrieve the attributes of the node
            node::Attributes getAttributes(Node const& _aNode)  const;

        // tracking pending changes
        public:
            typedef configuration::NodeChanges NodeChanges;

            void collectChanges(Tree const& _aTree, NodeChanges& rChanges) const;

            bool hasChanges(Tree const& _aTree) const;

            bool hasChanges(Node const& _aNode) const;

            void markChanged(Node const& _aNode);

        // commit protocol
        public:
            std::auto_ptr<SubtreeChange> preCommitChanges(Tree const& _aTree, configuration::ElementList& _rRemovedElements);

            void finishCommit(Tree const& _aTree, SubtreeChange& rRootChange);

            void revertCommit(Tree const& _aTree, SubtreeChange& rRootChange);

            void recoverFailedCommit(Tree const& _aTree, SubtreeChange& rRootChange);

        // notification protocol
        public:
            typedef configuration::NodeChangesInformation NodeChangesInformation;

            /// Adjust the internal representation after external changes to the original data - build NodeChangeInformation objects for notification
            void    adjustToChanges(NodeChangesInformation& rLocalChanges, Node const & _aNode, SubtreeChange const& aExternalChange);

        // visitor dispatch
        public:
            typedef configuration::GroupMemberVisitor GroupMemberVisitor;
            typedef configuration::SetNodeVisitor SetNodeVisitor;

            GroupMemberVisitor::Result dispatchToValues(GroupNode const& _aNode, GroupMemberVisitor& _aVisitor);

            /// Call <code>aVisitor.visit(aElement)</code> for each element in this set until SetNodeVisitor::DONE is returned.
            SetNodeVisitor::Result dispatchToElements(SetNode const& _aNode, SetNodeVisitor& _aVisitor);

        // value (element) node specific operations
        public:
            /// Does this node assume its default value
            /// retrieve the current value of this node
            UnoAny  getValue(ValueNode const& _aNode) const;

            /// get the type of this value
            UnoType getValueType(ValueNode const& _aNode)   const;


        // group node specific operations
        public:
            typedef configuration::ValueMemberNode   ValueMemberNode;
            typedef configuration::ValueMemberUpdate ValueMemberUpdate;

            /// does this hold a child value of the given name
            bool hasValue(GroupNode const& _aNode, Name const& _aName) const;

            /// does this hold a child value
            bool hasValue(GroupNode const& _aNode) const;

            /// are defaults for this node available ?
            bool areValueDefaultsAvailable(GroupNode const& _aNode) const;

            /// retrieve data for the child value of the given name
            ValueMemberNode getValue(GroupNode const& _aNode, Name const& _aName) const;

            /// retrieve data for updating the child value of the given name
            ValueMemberUpdate getValueForUpdate(GroupNode const & _aNode, Name const& _aName);

        // set node specific operations
        public:
            typedef configuration::ElementTreeData  SetNodeElement;
            typedef configuration::SetEntry         SetNodeEntry;

            /// does this set contain any elements (loads elements if needed)
            bool isEmpty(SetNode const& _aNode) const;

            /// does this set contain an element named <var>aName</var> (loads elements if needed)
            SetNodeEntry    findElement(SetNode const& _aNode, Name const& aName) const;

            /// does this set contain an element named <var>aName</var> (and is that element loaded ?)
            SetNodeEntry    findAvailableElement(SetNode const& _aNode, Name const& aName) const;

            /// insert a new entry into this set
            void        insertElement(SetNode const& _aNode, Name const& aName, SetNodeEntry const& aNewEntry);

            /// remove an existing entry into this set
            void        removeElement(SetNode const& _aNode, Name const& aName);

            /** Create a Subtree change as 'diff' which allows transforming the set to its default state
                (given that <var>_rDefaultTree</var> points to a default instance of this set)
                <p>Ownership of added trees should be transferred to the SubtreeChange.</p>
            */
            std::auto_ptr<SubtreeChange> differenceToDefaultState(SetNode const& _aNode, ISubtree& _rDefaultTree) const;

            /// Get the template that describes elements of this set
            configuration::TemplateHolder getElementTemplate(SetNode const& _aNode) const;

            /// Get a template provider that can create new elements for this set
            configuration::TemplateProvider getTemplateProvider(SetNode const& _aNode) const;

            // create a view::Tree from a configuration::SetEntry
            Tree extractTree(SetNodeEntry const& _anEntry);

        // creating/changing state/strategy
        public:
            NodeFactory& getNodeFactory();

        // access to node innards
        protected:
            /// provide access to the data of the underlying node
            data::NodeAccess getNodeAccess(Node const& _aNode) const;

            /// provide access to the address of the underlying node
            data::NodeAddress getNodeAddress(Node const& _aNode) const;

            /// retrieve the name of the underlying node
            Name getNodeName(Node const& _aNode) const;

            /// retrieve the attributes of the underlying node
            node::Attributes getNodeAttributes(Node const& _aNode) const;

        protected:
            //helper for migration to new (info based) model for adjusting to changes
            static void addLocalChangeHelper( NodeChangesInformation& rLocalChanges, configuration::NodeChange const& aChange);

        private:
            void implAdjustToValueChanges(NodeChangesInformation& rLocalChanges, GroupNode const& _aGroupNode, SubtreeChange const& rExternalChanges);
            void implAdjustToSubChanges(NodeChangesInformation& rLocalChanges, GroupNode const & _aGroupNode, SubtreeChange const& rExternalChanges);
            void implAdjustToElementChanges(NodeChangesInformation& rLocalChanges, SetNode const& _aNode, SubtreeChange const& rExternalChanges, TreeDepth nDepth);
            void implAdjustToElementChange (NodeChangesInformation& rLocalChanges, SetNode const& _aNode, Change const& rElementChange, TreeDepth nElementDepth);
            void implCommitDirectIn(data::TreeAccessor const& _aPlaceHolder, Node const& _aNode);

        protected:
            void            checkInstance(Tree const& _aTreeForThis) const;
            SetNodeEntry    implFindElement(SetNode const& _aNode, Name const& aName) const;
            SetNodeElement  implMakeElement(SetNode const& _aNode, SetNodeEntry const& anEntry) const;

        // virtual interface - these functions must be provided
        private:
            // change handling
            virtual bool doHasChanges(Node const& _aNode) const     = 0;
            virtual void doMarkChanged(Node const& _aNode)          = 0;

            virtual NodeFactory& doGetNodeFactory()  = 0;

        // virtual interface - these functions all have default implementations without support for pending changes
        protected:
            // change handling
            virtual void doCollectChanges(Node const& _aNode, NodeChanges& rChanges) const;

            // commit protocol
            virtual std::auto_ptr<SubtreeChange> doPreCommitChanges(Tree const& _aTree, configuration::ElementList& _rRemovedElements);
            virtual void doFailedCommit(Tree const& _aTree, SubtreeChange& rChanges);
            virtual void doFinishCommit(Tree const& _aTree, SubtreeChange& rChanges);
            virtual void doRevertCommit(Tree const& _aTree, SubtreeChange& rChanges);

            // notification protocol
            virtual configuration::ValueChangeImpl* doAdjustToValueChange(GroupNode const& _aGroupNode, Name const& aName, ValueChange const& rExternalChange);

            // common attributes
            virtual node::Attributes doAdjustAttributes(node::Attributes const& _aAttributes) const = 0;

            // group member access
            virtual ValueMemberNode doGetValueMember(GroupNode const& _aNode, Name const& _aName, bool _bForUpdate) const = 0;

            // set element access
            virtual void doInsertElement(SetNode const& _aNode, Name const& aName, SetNodeEntry const& aNewEntry) = 0;
            virtual void doRemoveElement(SetNode const& _aNode, Name const& aName) = 0;
        };

//-----------------------------------------------------------------------------
        inline Name ViewStrategy::getName(Node const& _aNode)   const
        { return getNodeName(_aNode); }

        inline node::Attributes ViewStrategy::getAttributes(Node const& _aNode) const
        { return doAdjustAttributes(getNodeAttributes(_aNode)); }

        inline bool ViewStrategy::hasChanges(Node const& _aNode) const
        { return doHasChanges(_aNode); }

        inline NodeFactory& ViewStrategy::getNodeFactory()
        { return doGetNodeFactory(); }

//-----------------------------------------------------------------------------
    }
//-----------------------------------------------------------------------------
}

#endif // CONFIGMGR_CONFIGNODEBEHAVIOR_HXX_
