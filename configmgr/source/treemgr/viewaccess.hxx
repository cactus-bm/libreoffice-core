/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewaccess.hxx,v $
 * $Revision: 1.6 $
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

#ifndef CONFIGMGR_VIEWACCESS_HXX_
#define CONFIGMGR_VIEWACCESS_HXX_

#include "viewnode.hxx"
#include "viewstrategy.hxx"
#include <rtl/ref.hxx>

//-----------------------------------------------------------------------------
namespace configmgr
{
//-----------------------------------------------------------------------------
    namespace configuration { class NodeRef; }
//-----------------------------------------------------------------------------
    namespace view
    {
//-----------------------------------------------------------------------------

    class ViewTreeAccess
    {
            rtl::Reference< ViewStrategy >  m_xStrategy;
            Tree                            m_aTree;

        public:
            explicit ViewTreeAccess(Tree const & _aTree);
            explicit ViewTreeAccess(configuration::TreeImpl& _rTree);

            rtl::Reference< view::ViewStrategy > getViewBehavior() { return m_xStrategy; }
        public:
            typedef configuration::NodeRef const& NodeArg;
            typedef configuration::NodeOffset NodeOffset;
            configuration::NodeData* nodeData(NodeArg    _aNodeArg) const;
            configuration::NodeData* nodeData(NodeOffset _aNodePos) const;
            bool isValid(NodeArg _aNodeArg) const;

            Node makeNode(NodeArg    _aNodeArg) const { return Node(m_aTree,nodeData(_aNodeArg)); }
            Node makeNode(NodeOffset _aNodePos) const { return Node(m_aTree,nodeData(_aNodePos)); }

            bool isSetNode  (NodeArg _aNodeArg) const { return makeNode(_aNodeArg).isSetNode(); }
            bool isGroupNode(NodeArg _aNodeArg) const { return makeNode(_aNodeArg).isGroupNode(); }
            bool isValueNode(NodeArg _aNodeArg) const { return makeNode(_aNodeArg).isValueNode(); }

            bool isSetNodeAt  (NodeOffset _aNodeArg) const { return makeNode(_aNodeArg).isSetNode(); }
            bool isGroupNodeAt(NodeOffset _aNodeArg) const { return makeNode(_aNodeArg).isGroupNode(); }
            bool isValueNodeAt(NodeOffset _aNodeArg) const { return makeNode(_aNodeArg).isValueNode(); }

            SetNode   toSetNode  (NodeArg _aNodeArg) const
            { return SetNode  (makeNode(_aNodeArg)); }

            GroupNode toGroupNode(NodeArg _aNodeArg) const
            { return GroupNode(makeNode(_aNodeArg)); }

            ValueNode toValueNode(NodeArg _aNodeArg) const
            { return ValueNode(makeNode(_aNodeArg)); }

            SetNode   getSetNodeAt  (NodeOffset _aNodeArg) const
            { return SetNode  (makeNode(_aNodeArg)); }

            GroupNode getGroupNodeAt(NodeOffset _aNodeArg) const
            { return GroupNode(makeNode(_aNodeArg)); }

            ValueNode getValueNodeAt(NodeOffset _aNodeArg) const
            { return ValueNode(makeNode(_aNodeArg)); }
        // node attributes
        public:
            /// retrieve the name of the node
            Name getName(NodeArg _aNode)    const
            { return m_xStrategy->getName(makeNode(_aNode)); }

            /// retrieve the attributes of the node
            node::Attributes getAttributes(NodeArg _aNode)  const
            { return m_xStrategy->getAttributes(makeNode(_aNode)); }

            /// retrieve the name of the tree root
            Name getRootName()  const
            { return m_xStrategy->getName( getRootNode(m_aTree) ); }

            /// retrieve the attributes of the tree root
            node::Attributes getRootAttributes()    const
            { return m_xStrategy->getAttributes( getRootNode(m_aTree) ); }

        // tracking pending changes
        public:
            void collectChanges(configuration::NodeChanges& rChanges)   const
            { m_xStrategy->collectChanges(m_aTree,rChanges); }

            bool hasChanges() const
            { return m_xStrategy->hasChanges(m_aTree); }

            bool hasChanges(NodeArg _aNode) const
            { return m_xStrategy->hasChanges(makeNode(_aNode)); }

            void markChanged(NodeArg _aNode)
            { m_xStrategy->markChanged(makeNode(_aNode)); }

        // commit protocol
        public:
            std::auto_ptr<SubtreeChange> preCommitChanges(configuration::ElementList& _rRemovedElements)
            { return m_xStrategy->preCommitChanges(m_aTree,_rRemovedElements); }

            void finishCommit(SubtreeChange& rRootChange)
            { m_xStrategy->finishCommit(m_aTree,rRootChange); }

            void revertCommit(SubtreeChange& rRootChange)
            { m_xStrategy->revertCommit(m_aTree,rRootChange); }

            void recoverFailedCommit(SubtreeChange& rRootChange)
            { m_xStrategy->recoverFailedCommit(m_aTree,rRootChange); }

        // notification protocol
        public:
            /// Adjust the internal representation after external changes to the original data - build NodeChangeInformation objects for notification
            void    adjustToChanges(configuration::NodeChangesInformation& rLocalChanges, NodeArg _aNode, SubtreeChange const& aExternalChange)
            { m_xStrategy->adjustToChanges(rLocalChanges,makeNode(_aNode), aExternalChange); }

        // visitor dispatch
        public:
            configuration::GroupMemberVisitor::Result dispatchToValues(GroupNode const& _aNode, configuration::GroupMemberVisitor& _aVisitor) const
            { return m_xStrategy->dispatchToValues(_aNode,_aVisitor); }

            /// Call <code>aVisitor.visit(aElement)</code> for each element in this set until SetNodeVisitor::DONE is returned.
            configuration::SetNodeVisitor::Result dispatchToElements(SetNode const& _aNode, configuration::SetNodeVisitor& _aVisitor) const
            { return m_xStrategy->dispatchToElements(_aNode,_aVisitor); }

        // value (element) node specific operations
        public:
        /// Does this node assume its default value
        /// retrieve the current value of this node
        UnoAny getValue(ValueNode const& _aNode) const
            { return m_xStrategy->getValue(_aNode); }

        /// get the type of this value
        UnoType getValueType(ValueNode const& _aNode)   const
            { return m_xStrategy->getValueType(_aNode); }


        // group node specific operations
        public:
            /// does this hold a child value of the given name
            bool hasValue(GroupNode const& _aNode, Name const& _aName) const
            { return m_xStrategy->hasValue(_aNode,_aName); }

            /// does this hold a child value
            bool hasValue(GroupNode const& _aNode) const
            { return m_xStrategy->hasValue(_aNode); }

            /// are defaults for this node available ?
            bool areValueDefaultsAvailable(GroupNode const& _aNode) const
            { return m_xStrategy->areValueDefaultsAvailable(_aNode); }

            /// retrieve data for the child value of the given name
            configuration::ValueMemberNode getValue(GroupNode const& _aNode, Name const& _aName) const
            { return m_xStrategy->getValue(_aNode,_aName); }

            /// retrieve data for updating the child value of the given name
            configuration::ValueMemberUpdate getValueForUpdate(GroupNode const & _aNode, Name const& _aName) const
            { return m_xStrategy->getValueForUpdate(_aNode,_aName); }

        // set node specific operations
        public:
            /// does this set contain any elements (loads elements if needed)
            bool isEmpty(SetNode const& _aNode) const
            { return m_xStrategy->isEmpty(_aNode); }

            /// does this set contain an element named <var>aName</var> (loads elements if needed)
            configuration::SetEntry findElement(SetNode const& _aNode, Name const& aName) const
            { return m_xStrategy->findElement(_aNode,aName); }

            /// does this set contain an element named <var>aName</var> (and is that element loaded ?)
            configuration::SetEntry findAvailableElement(SetNode const& _aNode, Name const& aName) const
            { return m_xStrategy->findAvailableElement(_aNode,aName); }

            /// insert a new entry into this set
            void        insertElement(SetNode const& _aNode, Name const& aName, configuration::SetEntry const& aNewEntry)
            { m_xStrategy->insertElement(_aNode,aName,aNewEntry); }

            /// remove an existing entry into this set
            void        removeElement(SetNode const& _aNode, Name const& aName)
            { m_xStrategy->removeElement(_aNode,aName); }

            /** Create a Subtree change as 'diff' which allows transforming the set to its default state
                (given that <var>_rDefaultTree</var> points to a default instance of this set)
                <p>Ownership of added trees should be transferred to the SubtreeChange.</p>
            */
            std::auto_ptr<SubtreeChange> differenceToDefaultState(SetNode const& _aNode, ISubtree& _rDefaultTree)
            { return m_xStrategy->differenceToDefaultState(_aNode,_rDefaultTree); }

            /// Get the template that describes elements of this set
            configuration::TemplateHolder getElementTemplate(SetNode const& _aNode) const
            { return m_xStrategy->getElementTemplate(_aNode); }

            /// Get a template provider that can create new elements for this set
            configuration::TemplateProvider getTemplateProvider(SetNode const& _aNode) const
            { return m_xStrategy->getTemplateProvider(_aNode); }

        // changing state/strategy
        public:
            // replace m_xStrategy by a direct ViewStrategy (commiting changes to the data), if possible
         //   void makeDirect  ();

        };

//-----------------------------------------------------------------------------
        inline
        ViewTreeAccess::ViewTreeAccess(Tree const & _aTree)
        : m_xStrategy()
        , m_aTree(_aTree)
        {
            m_xStrategy = _aTree.get_impl()->getViewBehavior();
        }

//-----------------------------------------------------------------------------
        inline
        ViewTreeAccess::ViewTreeAccess(configuration::TreeImpl& _rTree)
            : m_xStrategy(_rTree.getViewBehavior())
            , m_aTree(_rTree)
        {
        }

//-----------------------------------------------------------------------------
    }
}

#endif // CONFIGMGR_VIEWACCESS_HXX_
