/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewnode.hxx,v $
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

#ifndef CONFIGMGR_VIEWNODE_HXX_
#define CONFIGMGR_VIEWNODE_HXX_

#include "treeimpl.hxx"

#include "valuenodeaccess.hxx"
#include "groupnodeaccess.hxx"

namespace configmgr
{
//-----------------------------------------------------------------------------
    namespace configuration
    {
        class Name;
        struct ElementTreeData;
    }
//-----------------------------------------------------------------------------
    namespace view
    {
    //-------------------------------------------------------------------------
        using configuration::Name;
    //-------------------------------------------------------------------------
        struct Tree        // is a tree fragment
        {
            typedef configuration::TreeImpl TreeData;

            Tree(TreeData& _ref)
            : m_addr(&_ref)
            {}

        // low-level access
//            TreeData* operator->() const { return m_addr; }

            TreeData* get_impl() const
            { return m_addr; }

        private:
            TreeData *  m_addr;  // has a TreeAddress or NodeAddress
        };
    //-------------------------------------------------------------------------
        struct Node
        {
            typedef configuration::NodeOffset   NodeOffset;
            typedef configuration::NodeData     NodeData;
            typedef configuration::NodeImpl *   NodeAddress;

            Node(Tree const& _tree, NodeData* _addr)
            : m_tree(_tree), m_addr(_addr)
            {}

            Node(Tree::TreeData& _ref, NodeOffset _offs)
                : m_tree(_ref), m_addr( _ref.nodeData(_offs))
            {}

            bool is() const { return m_addr != 0; }

            Node getParent() const;

            bool isSetNode()    const { return is() && data().isSetNode(); }
            bool isGroupNode()  const { return is() && data().isGroupNode(); }
            bool isValueNode()  const { return is() && data().isValueElementNode(); }

        // low-level access
//            NodeAddress operator->() const { return &data().nodeImpl(); }

            NodeData& data() const { return *m_addr; }

            NodeAddress get_impl() const
            { return is() ? &data().nodeImpl() : NULL; }

            NodeOffset get_offset() const
            { return is() ? m_tree.get_impl()->nodeOffset( this->data() ) : 0; }

            Tree tree() const   // has a Tree
            { return m_tree; }

            data::NodeAccess getAccessRef() const;   // has a NodeAccess

        private:
            Tree        m_tree;   // has a Tree + Accessor
            NodeData*   m_addr;       // has a NodeAddress
        };
    //-------------------------------------------------------------------------
        struct ValueNode        // has/is a Node
        {
            typedef configuration::ValueElementNodeImpl NodeType;
            Node m_node;

            explicit
            ValueNode(Node const& _node)
            : m_node(_node)
            {}

            bool is() const { return m_node.isValueNode(); }

        // low-level access
 //           NodeType* operator->() const { return &m_node.data().valueElementImpl(); }

            NodeType* get_impl() const
            { return is() ? &m_node.data().valueElementImpl() : NULL; }

            Node node() const   // has a Node
            { return m_node; }

            Tree tree() const   // has a Tree
            { return m_node.tree(); }
        };
    //-------------------------------------------------------------------------
        struct GroupNode        // has/is a Node
        {
            typedef configuration::GroupNodeImpl NodeType;
            Node m_node;

            explicit
            GroupNode(Node const& _node)
            : m_node(_node)
            {}

            bool is() const { return m_node.isGroupNode(); }

            Node findChild(configuration::Name const& _aName) const;
            Node getFirstChild() const;
            Node getNextChild(Node const& _aAfterNode) const;

//            NodeType* operator->() const { return &m_node.data().groupImpl(); }

            NodeType* get_impl() const
            { return is() ? &m_node.data().groupImpl() : NULL; }

            Node node() const   // has a Node
            { return m_node; }

            Tree tree() const   // has a Tree
            { return m_node.tree(); }

            data::GroupNodeAccess getAccess() const;   // has a NodeAccess
        };
    //-------------------------------------------------------------------------
        struct SetNode          // has/is a Node
        {
            typedef configuration::SetNodeImpl NodeType;
            typedef configuration::ElementTreeData Element;

            Node m_node;

            explicit
            SetNode(Node const& _node)
            : m_node(_node)
            {}

            bool is() const { return m_node.isSetNode(); }

   //         NodeType* operator->() const  { return &m_node.data().setImpl(); }

            NodeType* get_impl() const
            { return is() ? &m_node.data().setImpl() : 0; }

            Node node() const   // has a Node
            { return m_node; }

            Tree tree() const   // has a Tree
            { return m_node.tree(); }

            data::SetNodeAccess getAccess() const;   // has a NodeAccess
        };
    //-------------------------------------------------------------------------
        inline
        Node getRootNode(Tree const & _aTree)
        {
            Tree::TreeData* pTreeData = _aTree.get_impl();
            return Node(_aTree,pTreeData->nodeData(pTreeData->root_()));
        }

    //-------------------------------------------------------------------------
        inline
        Name getSimpleRootName(Tree const & _aTree)
        {
            Tree::TreeData* pTreeData = _aTree.get_impl();
            return pTreeData->getSimpleRootName();
        }

    //-------------------------------------------------------------------------
        inline
        bool isValidNode(Node const & _aNode)
        {
            Tree::TreeData* pTreeData = _aNode.tree().get_impl();
            return pTreeData->isValidNode(_aNode.get_offset());
        }

    //-------------------------------------------------------------------------
        inline
        Name getSimpleNodeName(Node const & _aNode)
        {
            Tree::TreeData* pTreeData = _aNode.tree().get_impl();
            return pTreeData->getSimpleNodeName(_aNode.get_offset());
        }

    //-------------------------------------------------------------------------
        extern
        rtl::Reference< view::ViewStrategy > getViewBehavior(Tree const & _aTree);
    //-------------------------------------------------------------------------
    }
//-----------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------

#endif // CONFIGMGR_VIEWNODE_HXX_
