/*************************************************************************
 *
 *  $RCSfile: treeimpl.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2000-11-07 14:35:59 $
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

#include "treeimpl.hxx"
#include "roottreeimpl.hxx"

#include "nodeimpl.hxx"
#include "nodechangeimpl.hxx"
#include "template.hxx"

#include "cmtreemodel.hxx"

#include <osl/diagnose.h>

namespace configmgr
{
    namespace configuration
    {

//-----------------------------------------------------------------------------
// class TreeImplBuilder - friend of TreeImpl
//-----------------------------------------------------------------------------

/** is a visitor-style algorithm to construct a <type>TreeImpl::NodeList</type>
    representing a configuration hierarchy
*/
class TreeImplBuilder : public NodeModification
{
public:
    /** constructs a TreeImplBuilder to append onto <var>rList</var>
        the products of <var>rFactory</var> up to depth <var>nDepth</var>
    */
    TreeImplBuilder(NodeFactory& rFactory, TreeImpl& rTree)
        : m_rFactory(rFactory)
        , m_rTree(rTree)
        , m_nParent(0)
        , m_nDepthLeft(rTree.m_nDepth)
    {
        OSL_ASSERT(m_rTree.m_aNodes.empty());
    }

    /** helper function to locate or build a <type>Template</type>
        for the elements of set <var>rSet</var>
    */
    static TemplateHolder findElementTemplate(ISubtree& rSet);

private:
    /// implements the NodeModification handler for ValueNodes
    virtual void handle(ValueNode& rValue);
    /// implements the NodeModification member for ValueNodes
    virtual void handle(ISubtree& rTree);

    /// add a Node for group node <var>rGroup</var> to the list
    void addGroup(ISubtree& rGroup);
    /// add a Node for set node <var>rSet</var> to the list
    void addSet(ISubtree& rSet);
    /// add a Node for value node <var>rValue</var> to the list
    void addValue(ValueNode& rValue);

    NodeFactory&    m_rFactory;
    TreeImpl&   m_rTree;
    NodeOffset  m_nParent;
    TreeDepth   m_nDepthLeft;
};
//-----------------------------------------------------------------------------

TemplateHolder TreeImplBuilder::findElementTemplate(ISubtree& rSet)
{
    OUString sFullName = rSet.getChildTemplateName();

    return Template::fromPath(sFullName);
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::handle(ValueNode& rValue)
{
    addValue(rValue);
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::handle(ISubtree& rTree)
{
    if (rTree.isSetNode())
        addSet(rTree);
    else
        addGroup(rTree);
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::addValue(ValueNode& rValue)
{
    NodeImplHolder aValueNode( m_rFactory.makeValueNode(rValue) );
    OSL_ENSURE( aValueNode.isValid(), "could not make value node wrapper" );

    // TODO:!isValid() => maybe substitute a SimpleValueNodeImpl if possible
    if( aValueNode.isValid() )
    {
        m_rTree.m_aNodes.push_back( Node(aValueNode,m_nParent) );
    }
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::addGroup(ISubtree& rTree)
{
    NodeImplHolder aGroupNode( m_rFactory.makeGroupNode(rTree) );
    OSL_ENSURE( aGroupNode.isValid(), "could not make group node wrapper" );

    // TODO:!isValid() => maybe substitute a SimpleValueNodeImpl if possible
    if( aGroupNode.isValid() )
    {
        m_rTree.m_aNodes.push_back( Node(aGroupNode,m_nParent) );

        // now fill in group members
        if (m_nDepthLeft > 0)
        {
            NodeOffset nSaveParent = m_nParent;
            --m_nDepthLeft;

            m_nParent = m_rTree.m_aNodes.size() + m_rTree.root() - 1;

            // now recurse:
            this->applyToChildren(rTree);

            OSL_ENSURE(m_nParent < m_rTree.m_aNodes.size(),"WARNING: Configuration: Group within requested depth has no members");
            ++m_nDepthLeft;
            m_nParent = nSaveParent;
        }
    }
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::addSet(ISubtree& rTree)
{
    TemplateHolder aTemplate = findElementTemplate(rTree);
    OSL_ASSERT(aTemplate.isValid());

    NodeImplHolder aSetNode( m_rFactory.makeSetNode(rTree,aTemplate.getBodyPtr()) );
    OSL_ENSURE( aSetNode.isValid(), "could not make set node wrapper" );

    // TODO:!isValid() => maybe substitute a SimpleValueNodeImpl if possible
    if( aSetNode.isValid() )
    {
        m_rTree.m_aNodes.push_back( Node(aSetNode,m_nParent) );

        // this also relies on one based offsets
        NodeOffset nNodeAdded = m_rTree.m_aNodes.size() + m_rTree.root() - 1;

        m_rTree.m_aNodes.back().setImpl().initElements(m_rTree, nNodeAdded, m_nDepthLeft);
    }
}

//-----------------------------------------------------------------------------
// class Node
//-----------------------------------------------------------------------------

Node::Node(NodeImplHolder const& aSpecificNode, NodeOffset nParent)
: m_pSpecificNode(aSpecificNode)
, m_nParent(nParent)
{
}
//-----------------------------------------------------------------------------

static
inline
NodeInfo implInfo(NodeImpl const& aNode)
{
    NodeInfo info;
    aNode.getNodeInfo(info);
    return info;
}
//-----------------------------------------------------------------------------

Name Node::name()   const
{
    return implInfo(*m_pSpecificNode).name;
}
//-----------------------------------------------------------------------------

Attributes Node::attributes()   const
{
    return implInfo(*m_pSpecificNode).is;
}
//-----------------------------------------------------------------------------

NodeInfo Node::info()   const
{
    return implInfo(*m_pSpecificNode);
}
//-----------------------------------------------------------------------------

void Node::renameNode(Name const& aName)
{
    m_pSpecificNode->setNodeName(aName);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class TreeImpl
//-----------------------------------------------------------------------------

/// creates a TreeImpl for a detached, virgin instance of <var>aTemplate</var>
TreeImpl::TreeImpl()
: m_aNodes()
, m_aContextPath( AbsolutePath::detachedRoot() )
, m_pParentTree(0)
, m_nParentNode(0)
, m_nDepth( 0 )
, m_nRoot(1)
{
}
//-----------------------------------------------------------------------------

TreeImpl::TreeImpl( AbsolutePath const& aContextPath, NodeOffset nRoot)
: m_aNodes()
, m_aContextPath(aContextPath)
, m_pParentTree(0)
, m_nParentNode(0)
, m_nDepth(0)
, m_nRoot(nRoot)
{
}
//-----------------------------------------------------------------------------

TreeImpl::TreeImpl( TreeImpl& rParentTree, NodeOffset nParentNode )
: m_aNodes()
, m_aContextPath(rParentTree.getContextPath())
, m_pParentTree(&rParentTree)
, m_nParentNode(nParentNode)
, m_nDepth(0)
, m_nRoot(1)
{
    initPath(); // completes the path
}
//-----------------------------------------------------------------------------

TreeImpl::~TreeImpl()
{
}
//-----------------------------------------------------------------------------

void TreeImpl::initPath()
{
    OSL_ASSERT(m_pParentTree);
    Path::Components aNames = m_aContextPath.rep().components();
    m_pParentTree->appendPathTo(m_nParentNode,aNames);
    m_aContextPath = AbsolutePath(aNames);
}
//-----------------------------------------------------------------------------

void TreeImpl::build(NodeFactory& rFactory, INode& rCacheNode, TreeDepth nDepth)
{
    OSL_ASSERT(m_aNodes.empty());
    m_nDepth = nDepth;
    TreeImplBuilder(rFactory,*this).applyToNode(rCacheNode);
}
//-----------------------------------------------------------------------------

// context handling
//-----------------------------------------------------------------------------

void TreeImpl::setContext(TreeImpl* pParentTree, NodeOffset nParentNode)
{
    OSL_ENSURE(pParentTree,"ERROR: Moving tree to nowhere");

    if (pParentTree)
    {
        OSL_ENSURE( pParentTree->isValidNode(nParentNode),"ERROR: Moving tree to invalid node");
        if (!pParentTree->isValidNode(nParentNode))
            throw Exception("INTERNAL ERROR: Moving tree to invalid parent node");

        OSL_ENSURE( pParentTree->node(nParentNode)->isSetNode(),"WARNING: Moving tree to node that is not a set");
    }
    else
    {
        OSL_ENSURE( nParentNode == 0,"WARNING: Moving tree to node without a tree");
        nParentNode = 0;
    }

    m_pParentTree = pParentTree;
    m_nParentNode = nParentNode;
}
//-----------------------------------------------------------------------------

void TreeImpl::clearContext()
{
    m_pParentTree = 0;
    m_nParentNode = 0;
}
//-----------------------------------------------------------------------------

bool TreeImpl::hasChanges() const
{
    return node(root())->hasChanges();
}
//-----------------------------------------------------------------------------

void TreeImpl::collectChanges(NodeChanges& rChanges) const
{
    implCollectChangesFrom(root(),rChanges);
}
//-----------------------------------------------------------------------------

void TreeImpl::implCollectChangesFrom(NodeOffset nNode, NodeChanges& rChanges) const
{
    Node const* pNode = node(nNode);
    if (pNode->hasChanges())
    {
        pNode->collectChanges(rChanges);
        for (NodeOffset nChild = firstChild(nNode); isValidNode(nChild); nChild = findNextChild(nNode, nChild) )
        {
            implCollectChangesFrom(nChild,rChanges);
        }
    }
}
//-----------------------------------------------------------------------------

void TreeImpl::markChanged(NodeOffset nNode)
{
    OSL_ASSERT(isValidNode(nNode));

    if (nNode)
    {
        do
        {
            Node* pNode = node(nNode);
            pNode->markChanged();

            nNode = pNode->parent();
        }
        while (nNode && node(nNode)->hasChanges());
    }

    if (nNode == 0) // just marked parent
    {
        TreeImpl* pContext = getContextTree();
        NodeOffset nContext = getContextNode();
        if (pContext)
        {
            OSL_ASSERT(pContext->isValidNode(nContext));
            pContext->markChanged(nContext);
        }
    }
}
//-----------------------------------------------------------------------------

void TreeImpl::commitChanges()
{
    implCommitChangesFrom(root());
}
//-----------------------------------------------------------------------------

void TreeImpl::implCommitChangesFrom(NodeOffset nNode)
{
    Node* pNode = node(nNode);
    if (pNode->hasChanges())
    {
        pNode->commitChanges();
        for (NodeOffset nChild = firstChild(nNode); isValidNode(nChild); nChild = findNextChild(nNode, nChild) )
        {
            implCommitChangesFrom(nChild);
        }
    }
}
//-----------------------------------------------------------------------------

void TreeImpl::makeIndirect(bool bIndirect)
{
    // do it from outside in
    for(NodeList::reverse_iterator it = m_aNodes.rbegin(), stop = m_aNodes.rend();
        it != stop;
        ++it)
        it->makeIndirect(bIndirect);
}
//-----------------------------------------------------------------------------

// Node Collection navigation
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::parent(NodeOffset nNode) const
{
    OSL_ASSERT(isValidNode(nNode));
    return node(nNode)->parent();
}
//-----------------------------------------------------------------------------

Name TreeImpl::name(NodeOffset nNode) const
{
    return node(nNode)->name();
}
//-----------------------------------------------------------------------------

TreeDepth TreeImpl::depthTo(NodeOffset nNode) const
{
    OSL_ASSERT(isValidNode(nNode));

    TreeDepth nDepth = 0;
    while( 0 != (nNode=parent(nNode)) )
    {
        ++nDepth;
    }

    return nDepth;
}
//-----------------------------------------------------------------------------

void TreeImpl::appendPathTo(NodeOffset nNode, Path::Components& rNames)
{
    // nNode == 0 is handled correctly ...
    OSL_ASSERT(nNode == 0 || isValidNode(nNode));

    Path::Components::size_type nStart = rNames.size();

    for (; nNode != 0; nNode = parent(nNode) )
    {
        OSL_ENSURE( isValidNode(nNode), "ERROR: Configuration: node has invalid parent");
        rNames.push_back( name(nNode) );
    }
    std::reverse(rNames.begin() + nStart, rNames.end());
}
//-----------------------------------------------------------------------------

// Node iteration and access
NodeOffset TreeImpl::firstChild (NodeOffset nParent) const
{
    return findNextChild(nParent,nParent);
}
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::nextSibling(NodeOffset nNode) const
{
    return findNextChild(parent(nNode),nNode);
}
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::findNextChild(NodeOffset nParent, NodeOffset nStartAfter) const
{
    OSL_ASSERT(isValidNode(nParent));
    OSL_ASSERT(nStartAfter == 0 || isValidNode(nStartAfter));

    NodeOffset nPos = nStartAfter ? nStartAfter : root()-1;
    NodeOffset const nAfterLast = nodeCount() + root();
    while (++nPos < nAfterLast)
    {
        if(parent(nPos) == nParent) return nPos;
    }
    return 0;
}
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::findChild(NodeOffset nParent, Name const& aName) const
{
    OSL_ASSERT(isValidNode(nParent));

    NodeOffset nPos = nParent;
    NodeOffset const nAfterLast = nodeCount() + root();
    while (++nPos < nAfterLast)
    {
        if(parent(nPos) == nParent && name(nPos) == aName)
            return nPos;
    }
    return 0;
}
//-----------------------------------------------------------------------------

// dynamic-casting
//-----------------------------------------------------------------------------

ElementTreeImpl * TreeImpl::asElementTree()
{
    ElementTreeImpl const* const pResult = doCastToElementTree();
    TreeImpl        const* const pTest = pResult;
    if (pTest== this)
        return const_cast<ElementTreeImpl *>(pResult);
    else
        return 0;
}
//-----------------------------------------------------------------------------

RootTreeImpl * TreeImpl::asRootTree()
{
    RootTreeImpl    const* const pResult = doCastToRootTree();
    TreeImpl        const* const pTest = pResult;
    if (pTest== this)
        return const_cast<RootTreeImpl *>(pResult);
    else
        return 0;
}
//-----------------------------------------------------------------------------

ElementTreeImpl const* TreeImpl::asElementTree() const
{
    return doCastToElementTree();
}
//-----------------------------------------------------------------------------

RootTreeImpl const* TreeImpl::asRootTree() const
{
    return doCastToRootTree();
}
//-----------------------------------------------------------------------------

RootTreeImpl const* ElementTreeImpl::doCastToRootTree() const
{
    return 0;
}
//-----------------------------------------------------------------------------

ElementTreeImpl const* ElementTreeImpl::doCastToElementTree() const
{
    return this;
}
//-----------------------------------------------------------------------------

RootTreeImpl const* RootTreeImpl::doCastToRootTree() const
{
    return this;
}
//-----------------------------------------------------------------------------

ElementTreeImpl const* RootTreeImpl::doCastToElementTree() const
{
    return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class RootTreeImpl
//-----------------------------------------------------------------------------

RootTreeImpl::RootTreeImpl( NodeFactory& rNodeFactory,
                        AbsolutePath const& aContextPath,
                        ISubtree& rCacheNode, TreeDepth nDepth,
                        NodeOffset nRoot)
: TreeImpl(aContextPath,nRoot)
, m_aTreeLock()
{
    TreeImpl::build(rNodeFactory,rCacheNode,nDepth);
}
//-----------------------------------------------------------------------------

void RootTreeImpl::acquireReadAccess() const
{
    m_aTreeLock.acquireReadAccess();
}
//-----------------------------------------------------------------------------
void RootTreeImpl::releaseReadAccess() const
{
    m_aTreeLock.releaseReadAccess();
}
//-----------------------------------------------------------------------------
void RootTreeImpl::acquireWriteAccess()
{
    m_aTreeLock.acquireWriteAccess();
}
//-----------------------------------------------------------------------------
void RootTreeImpl::releaseWriteAccess()
{
    m_aTreeLock.releaseWriteAccess();
}
//-----------------------------------------------------------------------------
ISynchronizedData* RootTreeImpl::getRootLock()
{
    return this;
}

//-----------------------------------------------------------------------------
// class ElementTreeImpl
//-----------------------------------------------------------------------------

ElementTreeImpl::ElementTreeImpl(   NodeFactory& rFactory,
                                    INode& rCacheNode, TreeDepth nDepth,
                                    TemplateHolder aTemplateInfo )
: TreeImpl()
, m_aInstanceInfo(aTemplateInfo)
, m_pOwnedNode(0)
{
    TreeImpl::build( rFactory, rCacheNode, nDepth );
}
//-----------------------------------------------------------------------------

ElementTreeImpl::ElementTreeImpl(   NodeFactory& rFactory,
                                    TreeImpl& rParentTree, NodeOffset nParentNode,
                                    INode& rCacheNode, TreeDepth nDepth,
                                    TemplateHolder aTemplateInfo )
: TreeImpl( rParentTree, nParentNode)
, m_aInstanceInfo(aTemplateInfo)
, m_pOwnedNode(0)
{
    TreeImpl::build( rFactory, rCacheNode, nDepth );
}
//-----------------------------------------------------------------------------

ElementTreeImpl::ElementTreeImpl( TemplateHolder aTemplate, ITemplateProvider* pProvider )
: TreeImpl()
, m_aInstanceInfo(aTemplate)
, m_pOwnedNode(0)
{
    OSL_ENSURE(pProvider, "INTERNAL ERROR: Cannot create Element: No Template provider");
    OSL_ENSURE(aTemplate.isValid(), "INTERNAL ERROR: Cannot create Element: No Template");
    if (!pProvider)             throw Exception("INTERNAL ERROR: Cannot create Element Instance: No Template provider");
    if (!aTemplate.isValid())   throw Exception("INTERNAL ERROR: Cannot create Element Instance: No Template");

    std::auto_ptr<INode> pNewNode( pProvider->createInstance(aTemplate->getPath().toString()) );
    if (!pNewNode.get())
    {
        throw Exception("ERROR: Provider can't create Element Instance From Template");
    }

    TreeImpl::build( NodeType::getDirectAccessFactory(), *pNewNode, ~0u );
    m_pOwnedNode = pNewNode.release();
}
//-----------------------------------------------------------------------------

ElementTreeImpl::ElementTreeImpl( TemplateHolder aTemplate, std::auto_ptr<INode>& pNewNode )
: TreeImpl()
, m_aInstanceInfo(aTemplate)
, m_pOwnedNode(0)
{
    if (!pNewNode.get())
    {
        throw Exception("ERROR: Provider can't create Element Instance From Template");
    }

    TreeImpl::build( NodeType::getDirectAccessFactory(), *pNewNode, ~0u );
    m_pOwnedNode = pNewNode.release();
}
//-----------------------------------------------------------------------------

ElementTreeImpl::~ElementTreeImpl()
{
    delete m_pOwnedNode;
}
//-----------------------------------------------------------------------------

// ownership handling
//-----------------------------------------------------------------------------

/// transfer ownership to the given set
void ElementTreeImpl::attachTo(ISubtree& rOwningSet, Name const& aElementName)
{
    OSL_ENSURE(m_pOwnedNode,"ERROR: Cannot add a non-owned node to a subtree");

    if (m_pOwnedNode)
    {
        m_pOwnedNode->setName(aElementName.toString());

        std::auto_ptr<INode> aNode(m_pOwnedNode);
        m_pOwnedNode = 0;

        rOwningSet.addChild(aNode);
    }
}
//-----------------------------------------------------------------------------

/// tranfer ownership from the given set
void ElementTreeImpl::detachFrom(ISubtree& rOwningSet, Name const& aElementName)
{
    OSL_ENSURE(!m_pOwnedNode,"ERROR: Cannot detach a already owned node from a subtree");
    if (!m_pOwnedNode)
    {
        std::auto_ptr<INode> aNode( rOwningSet.removeChild(aElementName.toString()) );
        OSL_ENSURE(aNode.get(),"ERROR: Detached node not found in the given subtree");

        m_pOwnedNode = aNode.release();
    }
}

// context handling
//-----------------------------------------------------------------------------

/// renames the tree's root without concern for context consistency !
void ElementTreeImpl::renameTree(Name const& aNewName)
{
    node(root())->renameNode(aNewName);
}
//-----------------------------------------------------------------------------

void ElementTreeImpl::moveTree(TreeImpl* pParentTree, NodeOffset nParentNode)
{
    TreeImpl::setContext(pParentTree,nParentNode);
    if (pParentTree)
    {
        m_pLockImpl = pParentTree->getRootLock();
    }
}
//-----------------------------------------------------------------------------

void ElementTreeImpl::detachTree()
{
    TreeImpl::clearContext();
}
//-----------------------------------------------------------------------------
void ElementTreeImpl::acquireReadAccess() const
{
    m_pLockImpl->acquireReadAccess();
}
//-----------------------------------------------------------------------------
void ElementTreeImpl::releaseReadAccess() const
{
    m_pLockImpl->releaseReadAccess();
}
//-----------------------------------------------------------------------------
void ElementTreeImpl::acquireWriteAccess()
{
    m_pLockImpl->acquireWriteAccess();
}
//-----------------------------------------------------------------------------
void ElementTreeImpl::releaseWriteAccess()
{
    m_pLockImpl->releaseWriteAccess();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ISynchronizedData* ElementTreeImpl::getRootLock()
{
    return m_pLockImpl;
}
//void ElementTreeImpl::commit()
//{
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
    }   // namespace configuration
}       // namespace configmgr

