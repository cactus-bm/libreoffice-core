/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: treeimpl.cxx,v $
 * $Revision: 1.32 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_configmgr.hxx"
#include <stdio.h>
#include "treeimpl.hxx"
#include "roottreeimpl.hxx"
#include "nodeimpl.hxx"
#include "nodechangeimpl.hxx"
#include "template.hxx"
#include "nodevisitor.hxx"
#include "valuenodeaccess.hxx"
#include "groupnodeaccess.hxx"
#include "setnodeaccess.hxx"
#include "valuenode.hxx"
#include "change.hxx"
#include "valuenodeimpl.hxx"
#include "setnodeimpl.hxx"
#include "groupnodeimpl.hxx"
#include "viewaccess.hxx"
#include "viewfactory.hxx"
#include "nodefactory.hxx"
#include <osl/diagnose.h>

namespace configmgr
{
//-----------------------------------------------------------------------------
    namespace configuration
    {
        using view::NodeFactory;
//-----------------------------------------------------------------------------
// class TreeImplBuilder - friend of TreeImpl
//-----------------------------------------------------------------------------

/** is a visitor-style algorithm to construct a <type>TreeImpl::NodeList</type>
    representing a configuration hierarchy
*/
class TreeImplBuilder : public data::NodeVisitor
{
public:
    /** constructs a TreeImplBuilder to append onto <var>rList</var>
        the products of <var>rFactory</var> up to depth <var>nDepth</var>
    */
    TreeImplBuilder(
            TemplateProvider const& aTemplateProvider,
            rtl::Reference<view::ViewStrategy> const& _xStrategy,
            TreeImpl& rTree
         )
        : m_xStrategy(_xStrategy)
        , m_aTemplateProvider(aTemplateProvider)
        , m_rFactory(_xStrategy->getNodeFactory())
        , m_rTree(rTree)
        , m_nParent(0)
        , m_nDepthLeft(rTree.m_nDepth)
    {
        OSL_ASSERT(m_rTree.m_aNodes.empty());
        OSL_DEBUG_ONLY(m_bMemberCheck = false);
        m_rTree.m_xStrategy = _xStrategy;
    }
protected:
    using NodeVisitor::handle;

private:
    /// implements the NodeVisitor handler for Value Nodes
    virtual Result handle(data::ValueNodeAccess const& _aValue);
    /// implements the NodeVisitor member for Group Nodes
    virtual Result handle(data::GroupNodeAccess const& _aGroup);
    /// implements the NodeVisitor member for Set Nodes
    virtual Result handle(data::SetNodeAccess const& _aSet);

    /// add a Node for group node <var>_aGroup</var> to the list
    void addGroup(data::GroupNodeAccess const& _aGroup);
    /// add a Node for set node <var>_aSet</var> to the list
    void addSet(data::SetNodeAccess const& _aSet);
    /// add a Node for value node <var>rValue</var> to the list
    void addValueElement(data::ValueNodeAccess const& _aValue);
    /// add a Member for value node <var>rValue</var> to the list
    void addValueMember(data::ValueNodeAccess const& _aValue);

    rtl::Reference<view::ViewStrategy>  m_xStrategy;
    TemplateProvider m_aTemplateProvider;
    NodeFactory&    m_rFactory;
    TreeImpl&       m_rTree;
    NodeOffset      m_nParent;
    TreeDepth       m_nDepthLeft;
#if OSL_DEBUG_LEVEL > 0
    bool m_bMemberCheck;
#endif
};
//-----------------------------------------------------------------------------
typedef TreeImplBuilder::Result VisitorStatus;
//-----------------------------------------------------------------------------

VisitorStatus TreeImplBuilder::handle(data::ValueNodeAccess const& _aValue)
{
    if (m_nParent == 0)
        addValueElement(_aValue); // if it is the root it is a value set element
    else
        addValueMember(_aValue); // if it is not the root it is a group member

    return CONTINUE;
}
//-----------------------------------------------------------------------------

VisitorStatus TreeImplBuilder::handle(data::GroupNodeAccess const& _aGroup)
{
    addGroup(_aGroup);
    return CONTINUE;
}
//-----------------------------------------------------------------------------

VisitorStatus TreeImplBuilder::handle(data::SetNodeAccess const& _aSet)
{
    addSet(_aSet);
    return CONTINUE;
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::addValueElement(data::ValueNodeAccess const& _aValue)
{
    rtl::Reference<NodeImpl> aValueNode( m_rFactory.makeValueNode(_aValue) );
    OSL_ENSURE( aValueNode.is(), "could not make value node wrapper" );

    OSL_ENSURE( m_nParent == 0, "Adding value element that is not root of its fragment" );
    // TODO:!isValid() => maybe substitute a SimpleValueNodeImpl if possible
    if( aValueNode.is() )
    {
        m_rTree.m_aNodes.push_back( NodeData(aValueNode,_aValue.getName(),m_nParent) );
    }
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::addValueMember(data::ValueNodeAccess const& )
{
    // nothing to do
    OSL_DEBUG_ONLY(m_bMemberCheck = true);
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::addGroup(data::GroupNodeAccess const& _aTree)
{
    rtl::Reference<NodeImpl> aGroupNode( m_rFactory.makeGroupNode(_aTree) );
    OSL_ENSURE( aGroupNode.is(), "could not make group node wrapper" );

    // TODO:!isValid() => maybe substitute a SimpleValueNodeImpl if possible
    if( aGroupNode.is() )
    {
        m_rTree.m_aNodes.push_back( NodeData(aGroupNode,_aTree.getName(),m_nParent) );

        // now fill in group members
        if (m_nDepthLeft > 0)
        {
            NodeOffset nSaveParent = m_nParent;
            decDepth(m_nDepthLeft);

            m_nParent = m_rTree.m_aNodes.size() + m_rTree.root_() - 1;

        #if OSL_DEBUG_LEVEL > 0
            bool bSaveMemberCheck = m_bMemberCheck;
            m_bMemberCheck = false;
        #endif

            // now recurse:
            this->visitChildren(_aTree);

            OSL_ENSURE(m_nParent < m_rTree.m_aNodes.size() || m_bMemberCheck,
                        "WARNING: Configuration: Group within requested depth has no members");

            OSL_DEBUG_ONLY(m_bMemberCheck = bSaveMemberCheck);

            incDepth(m_nDepthLeft);
            m_nParent = nSaveParent;
        }
    }
}
//-----------------------------------------------------------------------------

void TreeImplBuilder::addSet(data::SetNodeAccess const& _aSet)
{
    TemplateHolder aTemplate = makeSetElementTemplate(_aSet, m_aTemplateProvider);
    OSL_ASSERT(aTemplate.is());
    OSL_ENSURE(aTemplate->isInstanceTypeKnown(),"ERROR: Cannor create set instance without knowing the instance type");

    rtl::Reference<NodeImpl> aSetNode( m_rFactory.makeSetNode(_aSet,aTemplate.get()) );
    OSL_ENSURE( aSetNode.is(), "could not make set node wrapper" );

    // TODO:!isValid() => maybe substitute a SimpleValueNodeImpl if possible
    if( aSetNode.is() )
    {
        m_rTree.m_aNodes.push_back( NodeData(aSetNode,_aSet.getName(),m_nParent) );

        // this also relies on one based offsets
        NodeOffset nNodeAdded = m_rTree.m_aNodes.size() + m_rTree.root_() - 1;

        OSL_ASSERT(&m_rTree.m_aNodes.back().nodeImpl() == aSetNode.get());
        static_cast<SetNodeImpl&>(*aSetNode).initElements(m_aTemplateProvider, m_rTree, nNodeAdded, m_nDepthLeft);
    }
}

//-----------------------------------------------------------------------------
// class NodeData
//-----------------------------------------------------------------------------

NodeData::NodeData(rtl::Reference<NodeImpl> const& aSpecificNode, Name const& aName, NodeOffset nParent)
: m_pSpecificNode(aSpecificNode)
, m_aName_(aName)
, m_nParent(nParent)
{
}

//-----------------------------------------------------------------------------

void NodeData::rebuild(rtl::Reference<view::ViewStrategy> const & _xNewStrategy, data::NodeAccess const & _aNewData)
{
    using namespace data;

    rtl::Reference<NodeImpl> aNewImpl;
    if (this->isSetNode())
    {
        SetNodeAccess aNewSet(_aNewData);
        aNewImpl = _xNewStrategy->getNodeFactory().makeSetNode(aNewSet,NULL);

        SetNodeImpl & rOldSetData = this->setImpl();
        SetNodeImpl & rNewSetData = static_cast<SetNodeImpl &>(*aNewImpl);

        SetNodeAccess aOldSet = rOldSetData.getDataAccess();

        rNewSetData.rebuildFrom(rOldSetData,aNewSet);
    }
    else if (this->isGroupNode())
        aNewImpl = _xNewStrategy->getNodeFactory().makeGroupNode(GroupNodeAccess(_aNewData));

    else if (this->isValueElementNode())
        aNewImpl = _xNewStrategy->getNodeFactory().makeValueNode(ValueNodeAccess(_aNewData));

    m_pSpecificNode = aNewImpl;
}

//-----------------------------------------------------------------------------

bool NodeData::isSetNode() const
{
    return data::SetNodeAccess::isInstance(getOriginalNodeAccess());
}
//-----------------------------------------------------------------------------

bool NodeData::isValueElementNode() const
{
    return data::ValueNodeAccess::isInstance(getOriginalNodeAccess());
}
//-----------------------------------------------------------------------------

bool NodeData::isGroupNode() const
{
    return data::GroupNodeAccess::isInstance(getOriginalNodeAccess());
}
//-----------------------------------------------------------------------------

SetNodeImpl&   NodeData::implGetSetImpl()   const
{
    OSL_ASSERT(m_pSpecificNode != 0);
    OSL_ASSERT(isSetNode());

    if (!isSetNode())
            throw Exception( "INTERNAL ERROR: Node is not a set node. Cast failing." );

    return static_cast<SetNodeImpl&>(*m_pSpecificNode);
}
//---------------------------------------------------------------------

GroupNodeImpl& NodeData::implGetGroupImpl() const
{
    OSL_ASSERT(m_pSpecificNode != 0);
    OSL_ASSERT(isGroupNode());

    if (!isGroupNode())
            throw Exception( "INTERNAL ERROR: Node is not a group node. Cast failing." );

    return static_cast<GroupNodeImpl&>(*m_pSpecificNode);
}
//---------------------------------------------------------------------

ValueElementNodeImpl& NodeData::implGetValueImpl() const
{
    OSL_ASSERT(m_pSpecificNode != 0);
    OSL_ASSERT(isValueElementNode());

    if (!isValueElementNode())
            throw Exception( "INTERNAL ERROR: Node is not a value node. Cast failing." );

    return static_cast<ValueElementNodeImpl&>(*m_pSpecificNode);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class TreeImpl
//-----------------------------------------------------------------------------

/// creates a TreeImpl for a detached, virgin instance of <var>aTemplate</var>
TreeImpl::TreeImpl( )
: m_aNodes()
, m_pParentTree(0)
, m_nParentNode(0)
, m_nDepth(0)
//, m_nRoot(nRoot)
{
}
//-----------------------------------------------------------------------------

TreeImpl::TreeImpl( TreeImpl& rParentTree, NodeOffset nParentNode )
: m_aNodes()
, m_pParentTree(&rParentTree)
, m_nParentNode(nParentNode)
, m_nDepth(0)
//, m_nRoot(1)
{
}
//-----------------------------------------------------------------------------

TreeImpl::~TreeImpl()
{
}
//-----------------------------------------------------------------------------

void TreeImpl::disposeData()
{
    m_aNodes.clear();
}

//-----------------------------------------------------------------------------
void TreeImpl::rebuild(rtl::Reference<view::ViewStrategy> const & _xNewStrategy, data::NodeAccess const & _aNewData)
{
    m_xStrategy = _xNewStrategy;
    this->implRebuild( this->root_(), _aNewData);
}

//-----------------------------------------------------------------------------
void TreeImpl::implRebuild(NodeOffset nNode, data::NodeAccess const & _aNewData)
{
    NodeData * pNode = nodeData(nNode);
    if (pNode->isGroupNode())
    {
        // first rebuild the children
        data::GroupNodeAccess aNewGroupAccess(_aNewData);
        OSL_ASSERT(aNewGroupAccess.isValid());

        for (NodeOffset nChild = firstChild_(nNode); isValidNode(nChild); nChild = findNextChild_(nNode,nChild))
        {
            data::NodeAccess aChildAccess = aNewGroupAccess.getChildNode(implGetOriginalName(nChild));
            OSL_ASSERT(aChildAccess.isValid());
            implRebuild(nChild,aChildAccess);
        }
    }

    pNode->rebuild(m_xStrategy,_aNewData);
}

//-----------------------------------------------------------------------------

void ElementTreeImpl::doFinishRootPath(Path::Rep& rPath) const
{
    rPath.prepend( doGetRootName() );
    rPath.prepend( AbsolutePath::detachedRoot().rep() );
}
//-----------------------------------------------------------------------------

void RootTreeImpl::doFinishRootPath(Path::Rep& rPath) const
{
    rPath.prepend( m_aRootPath.rep() );
}

//-----------------------------------------------------------------------------

void TreeImpl::implPrependRootPath(Path::Rep& rPath) const
{
    if (m_pParentTree)
    {
        rPath.prepend( doGetRootName() );
        OSL_ASSERT(m_nParentNode);
        m_pParentTree->prependLocalPathTo(m_nParentNode,rPath);
        m_pParentTree->implPrependRootPath(rPath);
    }
    else
    {
        doFinishRootPath( rPath );
    }
}
//-----------------------------------------------------------------------------

AbsolutePath TreeImpl::getRootPath() const
{
    Path::Rep aPath;
    implPrependRootPath(aPath);
    return AbsolutePath(aPath);
}
//-----------------------------------------------------------------------------
void TreeImpl::build(rtl::Reference<view::ViewStrategy> const& _xStrategy, data::NodeAccess const& _aRootNode, TreeDepth nDepth, TemplateProvider const& aTemplateProvider)
{
    OSL_ASSERT(m_aNodes.empty());
    m_nDepth = nDepth;
    TreeImplBuilder a(aTemplateProvider, _xStrategy,*this);
    a.visitNode(_aRootNode);
}
//-----------------------------------------------------------------------------

rtl::Reference< view::ViewStrategy > TreeImpl::getViewBehavior() const
{
    return m_xStrategy;
}

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

    //  OSL_ENSURE( pParentTree->isSetNodeAt(nParentNode),"WARNING: Moving tree to node that is not a set");
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

//-----------------------------------------------------------------------------
// Node Collection navigation
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::parent_(NodeOffset nNode) const
{
    OSL_ASSERT(isValidNode(nNode));
    return nodeData(nNode)->getParent();
}
//-----------------------------------------------------------------------------
inline // is protected and should be used only here
Name TreeImpl::implGetOriginalName(NodeOffset nNode) const
{
    OSL_ASSERT(isValidNode(nNode));

    return nodeData(nNode)->getName();
}
//-----------------------------------------------------------------------------

Path::Component ElementTreeImpl::doGetRootName() const
{
    return makeExtendedName( m_aElementName );
}

//-----------------------------------------------------------------------------

Name ElementTreeImpl::getSimpleRootName() const
{
    // TreeImpl::getSimpleRootName tends to parse &
    // then split the same name, burning CPU L&R
    return Name( m_aElementName );
}

//-----------------------------------------------------------------------------

Path::Component RootTreeImpl::doGetRootName() const
{
    return m_aRootPath.getLocalName();
}
//-----------------------------------------------------------------------------


Name TreeImpl::getSimpleNodeName(NodeOffset nNode) const
{
    if (nNode == root_()) return getSimpleRootName();

    return implGetOriginalName(nNode);
}
//-----------------------------------------------------------------------------

Name TreeImpl::getSimpleRootName() const
{
    return doGetRootName().getName();
}

//-----------------------------------------------------------------------------

Path::Component TreeImpl::getExtendedRootName() const
{
    return doGetRootName();
}
//-----------------------------------------------------------------------------

TreeDepth TreeImpl::depthTo(NodeOffset nNode) const
{
    OSL_ASSERT(isValidNode(nNode));

    TreeDepth nDepth = 0;
    while( 0 != (nNode=parent_(nNode)) )
    {
        ++nDepth;
    }

    return nDepth;
}
//-----------------------------------------------------------------------------

void TreeImpl::prependLocalPathTo(NodeOffset nNode, Path::Rep& rNames)
{
    OSL_ASSERT(isValidNode(nNode));

    for (; nNode != root_(); nNode = parent_(nNode) )
    {
        OSL_ENSURE( isValidNode(nNode), "ERROR: Configuration: node has invalid parent");
        rNames.prepend( Path::wrapSimpleName( implGetOriginalName(nNode) ) );
    }

    OSL_ASSERT(nNode == root_());
}
//-----------------------------------------------------------------------------

// Node iteration and access
NodeOffset TreeImpl::firstChild_ (NodeOffset nParent) const
{
    return findNextChild_(nParent,nParent);
}
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::nextSibling_(NodeOffset nNode) const
{
    return findNextChild_(parent_(nNode),nNode);
}
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::findNextChild_(NodeOffset nParent, NodeOffset nStartAfter) const
{
    OSL_ASSERT(isValidNode(nParent));
    OSL_ASSERT(nStartAfter == 0 || isValidNode(nStartAfter));

    NodeOffset nPos = nStartAfter ? nStartAfter : root_()-1;
    NodeOffset const nAfterLast = nodeCount() + root_();
    while (++nPos < nAfterLast)
    {
        if(parent_(nPos) == nParent) return nPos;
    }
    return 0;
}
//-----------------------------------------------------------------------------

NodeOffset TreeImpl::findChild_(NodeOffset nParent, Name const& aName) const
{
    OSL_ASSERT(isValidNode(nParent));

    NodeOffset nPos = nParent;
    NodeOffset const nAfterLast = nodeCount() + root_();
    while (++nPos < nAfterLast)
    {
        if(parent_(nPos) == nParent && implGetOriginalName(nPos) == aName)
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

RootTreeImpl::RootTreeImpl( rtl::Reference<view::ViewStrategy> const& _xStrategy,
                            AbsolutePath const& aRootPath,
                            data::NodeAccess const& _aCacheNode, TreeDepth nDepth,
                            TemplateProvider const& aTemplateProvider)
: TreeImpl()
, m_aRootPath(aRootPath)
{
//    OSL_ENSURE( aRootPath.getLocalName().getName() == _aCacheNode.getName(),
//                "Constructing root node: Path does not match node name");

    TreeImpl::build(_xStrategy,_aCacheNode,nDepth,aTemplateProvider);
}

//-----------------------------------------------------------------------------
// class ElementTreeImpl
//-----------------------------------------------------------------------------

ElementTreeImpl::ElementTreeImpl(   rtl::Reference<view::ViewStrategy> const& _xStrategy,
                                    data::TreeAccessor const& _aCacheTree, TreeDepth nDepth,
                                    TemplateHolder aTemplateInfo,
                                    TemplateProvider const& aTemplateProvider )
: TreeImpl()
, m_aInstanceInfo(aTemplateInfo)
, m_aElementName(_aCacheTree.getName())
, m_aDataAddress(_aCacheTree)
, m_aOwnData()
{
    TreeImpl::build( _xStrategy, _aCacheTree.getRootNode(), nDepth, aTemplateProvider );
}
//-----------------------------------------------------------------------------

ElementTreeImpl::ElementTreeImpl(   rtl::Reference<view::ViewStrategy> const& _xStrategy,
                                    TreeImpl& rParentTree, NodeOffset nParentNode,
                                    data::TreeAccessor const& _aCacheTree, TreeDepth nDepth,
                                    TemplateHolder aTemplateInfo,
                                    TemplateProvider const& aTemplateProvider )
: TreeImpl( rParentTree, nParentNode )
, m_aInstanceInfo(aTemplateInfo)
, m_aElementName(_aCacheTree.getName())
, m_aDataAddress(_aCacheTree)
, m_aOwnData()
{
    TreeImpl::build( _xStrategy, _aCacheTree.getRootNode(), nDepth, aTemplateProvider );
}
//-----------------------------------------------------------------------------

ElementTreeImpl::ElementTreeImpl(   data::TreeSegment const& pNewTree,
                                    TemplateHolder aTemplate,
                                    TemplateProvider const& aTemplateProvider )
: TreeImpl()
, m_aInstanceInfo(aTemplate)
, m_aElementName(pNewTree.getName())
, m_aDataAddress(pNewTree.getBaseAddress())
, m_aOwnData(pNewTree)
{
    if (!pNewTree.is())
    throw Exception("ERROR: Provider can't create Element Instance From Template");

    data::NodeAccess aNewNodeWrapper( m_aOwnData.getSegmentRootNode() );

    TreeImpl::build( view::createDirectAccessStrategy(m_aOwnData), aNewNodeWrapper, c_TreeDepthAll, aTemplateProvider );
}
//-----------------------------------------------------------------------------

ElementTreeImpl::~ElementTreeImpl()
{
}
//-----------------------------------------------------------------------------

bool ElementTreeImpl::isUpdatableSegment(TreeImpl& _rTree)
{
    TreeImpl * pTree = &_rTree;

    while (ElementTreeImpl * pElement = pTree->asElementTree())
    {
        if (pElement->m_aOwnData.is())
        {
            OSL_ENSURE( pElement->getContextTree()==NULL ||
                        pElement->getContextTree()->getViewBehavior() != pElement->getViewBehavior(),
                        "ElementTree with parent in same fragment should not own its data");
            return true;
        }

        pTree = pElement->getContextTree();

        if (!pTree)
        {
            OSL_ENSURE( false, "ElementTree without own data should have a parent");
            return false;
        }

    }
    OSL_ENSURE( false, "Tree is not part of free-floating segment - cannot support direct update");

    return false;
}
//-----------------------------------------------------------------------------

void ElementTreeImpl::disposeData()
{
    TreeImpl::disposeData();
    m_aOwnData.clear();
}
//-----------------------------------------------------------------------------

Path::Component ElementTreeImpl::makeExtendedName(Name const& _aSimpleName) const
{
    Name aTypeName = implGetOriginalName(root_());

    OSL_ENSURE(this->isTemplateInstance(), "ElementTree: Cannot discover the type this instantiatiates");

    OSL_ENSURE(! this->isTemplateInstance() || this->getTemplate()->getName() == aTypeName,
                "ElementTree: Type name does not match template");

    return Path::makeCompositeName(_aSimpleName, aTypeName);
}
//-----------------------------------------------------------------------------

// ownership handling
//-----------------------------------------------------------------------------
void ElementTreeImpl::rebuild(rtl::Reference<view::ViewStrategy> const & _aStrategy, data::TreeAccessor const & _aNewTree)
{
    TreeImpl::rebuild(_aStrategy,_aNewTree.getRootNode());
    m_aDataAddress = _aNewTree;
    m_aElementName = _aNewTree.getName();
}

//-----------------------------------------------------------------------------
/// transfer ownership to the given set
// -----------------------------------------------------------------------------
void ElementTreeImpl::attachTo(data::SetNodeAccess const & aOwningSet, Name const& aElementName)
{
    OSL_ENSURE(m_aOwnData.is(),"ERROR: Cannot add a non-owned node to a subtree");

    if (m_aOwnData.is())
    {
        OSL_ENSURE(this->getSimpleRootName() == aElementName,"ElementTree: Attaching with unexpected element name");
        m_aOwnData.setName(aElementName);

        TreeImpl* pOwningTree = this->getContextTree();
        OSL_ENSURE(pOwningTree, "Element Tree Context must be set before attaching data");

        if (isUpdatableSegment(*pOwningTree))
        {
            // copy over to the new segment
            data::TreeAddress aNewElement = m_aOwnData.getTreeAccess().copyTree();

            data::SetNodeAccess::addElement(aOwningSet,  aNewElement);

            data::TreeAccessor aNewAccessor(aNewElement);

            rtl::Reference<view::ViewStrategy> xNewBehavior = pOwningTree->getViewBehavior();

            this->rebuild(xNewBehavior,aNewAccessor);
        }
        else
            OSL_ENSURE( false, "Cannot attach directly to new tree - no update access available");

        m_aOwnData.clearData();
        OSL_ASSERT(!m_aOwnData.is());
    }
}
//-----------------------------------------------------------------------------

/// tranfer ownership from the given set
void ElementTreeImpl::detachFrom(data::SetNodeAccess const & aOwningSet, Name const& aElementName)
{
    OSL_ENSURE(!m_aOwnData.is(),"ERROR: Cannot detach a already owned node from a subtree");
    if (!m_aOwnData.is())
    {
        OSL_ENSURE(this->getSimpleRootName() == aElementName,"ElementTree: Detaching with unexpected element name");

        rtl::Reference< view::ViewStrategy > xOldStrategy = this->getViewBehavior();
        OSL_ENSURE(xOldStrategy.is(), "Element Tree Context must still have the old strategy when detaching data");

        {
            using namespace data;

            // make a new segment with a copy of the data
            TreeSegment aNewSegment = TreeSegment::createNew( this->getOriginalTreeAccess() );

            OSL_ENSURE(aNewSegment.is(),"ERROR: Could not create detached copy of elment data");

            this->takeTreeAndRebuild( aNewSegment );

            TreeAddress aOldElement = data::SetNodeAccess::removeElement(aOwningSet, aElementName );
        OSL_ENSURE(aOldElement != NULL,"ERROR: Detached node not found in the given subtree");

            TreeAccessor::freeTree(aOldElement);
        }

        OSL_ENSURE(m_aOwnData.is(),"ERROR: Could not create own data segment for detached node");
    }
}
//-----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/// transfer ownership from the given owner
void ElementTreeImpl::takeTreeBack(data::TreeSegment const & _aDataSegment)
{
    OSL_ENSURE(!m_aOwnData.is(),"ERROR: Cannot take over a node - already owning");
    OSL_ENSURE(_aDataSegment.is(),"ERROR: Cannot take over NULL tree segment");

    m_aOwnData = _aDataSegment;
    OSL_ENSURE(m_aOwnData.is(),"ERROR: Could not take over data segment");

    m_aDataAddress = m_aOwnData.getBaseAddress();
    m_aElementName = m_aOwnData.getName();
}
//-----------------------------------------------------------------------------

/// transfer ownership from the given owner
void ElementTreeImpl::takeTreeAndRebuild(data::TreeSegment const & _aDataSegment)
{
    OSL_ENSURE(!m_aOwnData.is(),"ERROR: Cannot take over a node - already owning");
    OSL_ENSURE(_aDataSegment.is(),"ERROR: Cannot take over NULL tree segment");
    if (!m_aOwnData.is())
    {
        this->rebuild(view::createDirectAccessStrategy(_aDataSegment), _aDataSegment.getTreeAccess());
        m_aOwnData = _aDataSegment;
        OSL_ENSURE(m_aOwnData.is(),"ERROR: Could not take over data segment");
    }
}
//-----------------------------------------------------------------------------

/// transfer ownership to the given owner
data::TreeSegment ElementTreeImpl::getOwnedTree() const
{
    OSL_ENSURE(m_aOwnData.is(),"ERROR: Cannot provide segment for a non-owned node");
    return m_aOwnData;
}
//-----------------------------------------------------------------------------

/// release ownership
data::TreeSegment ElementTreeImpl::releaseOwnedTree()
{
    OSL_ENSURE(m_aOwnData.is(),"ERROR: Cannot release and rename a non-owned node");

    data::TreeSegment aTree = m_aOwnData;
    m_aOwnData.clear();

    if (aTree.is())
        aTree.setName(m_aElementName);

    return aTree;
}
//-----------------------------------------------------------------------------

// context handling
//-----------------------------------------------------------------------------

/// renames the tree's root without concern for context consistency !
void ElementTreeImpl::renameTree(Name const& aNewName)
{
    m_aElementName = aNewName;
}
//-----------------------------------------------------------------------------

void ElementTreeImpl::moveTree(TreeImpl* pParentTree, NodeOffset nParentNode)
{
    TreeImpl::setContext(pParentTree,nParentNode);
}
//-----------------------------------------------------------------------------

void ElementTreeImpl::detachTree()
{
    TreeImpl::clearContext();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
    }   // namespace configuration
}       // namespace configmgr

