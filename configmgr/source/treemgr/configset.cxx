/*************************************************************************
 *
 *  $RCSfile: configset.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: jb $ $Date: 2001-06-21 12:02:38 $
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
#include <stdio.h> // needed for Solaris 8
#include "configset.hxx"

#include "nodechange.hxx"
#include "nodefactory.hxx"
#include "nodechangeimpl.hxx"
#include "treeimpl.hxx"
#include "template.hxx"
#include "templateimpl.hxx"
#include "configgroup.hxx"

#include "valuenode.hxx"
#include "valuenodeimpl.hxx"
#include "setnodeimplbase.hxx"

#include <vos/refernce.hxx>

namespace configmgr
{
    namespace configuration
    {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class ElementRef
//-----------------------------------------------------------------------------

ElementRef::ElementRef(ElementTreeImpl* pTree)
: m_aTreeHolder(pTree)
{
}
//-----------------------------------------------------------------------------

ElementRef::ElementRef(ElementRef const& aOther)
: m_aTreeHolder(aOther.m_aTreeHolder)
{
}
//-----------------------------------------------------------------------------

ElementRef& ElementRef::operator=(ElementRef const& aOther)
{
    m_aTreeHolder = aOther.m_aTreeHolder;
    return *this;
}
//-----------------------------------------------------------------------------

ElementRef::~ElementRef()
{
}
//-----------------------------------------------------------------------------

bool ElementRef::isValid() const
{
    return !!m_aTreeHolder.isValid();
}

//-----------------------------------------------------------------------------

ElementTree ElementRef::getElementTree() const
{
   return ElementTree(m_aTreeHolder);
}
//-----------------------------------------------------------------------------

Name ElementRef::getName() const
{
    if (!isValid()) return Name();

    return m_aTreeHolder->getRootName();
}
//-----------------------------------------------------------------------------
// class ElementTree
//-----------------------------------------------------------------------------

ElementTree::ElementTree(ElementTreeImpl* pTree)
: m_aTreeHolder(pTree)
{
}
//-----------------------------------------------------------------------------

ElementTree::ElementTree(ElementTreeHolder const& pTree)
: m_aTreeHolder(pTree)
{
}
//-----------------------------------------------------------------------------

ElementTree::ElementTree(ElementTree const& aOther)
: m_aTreeHolder(aOther.m_aTreeHolder)
{
}
//-----------------------------------------------------------------------------

ElementTree& ElementTree::operator=(ElementTree const& aOther)
{
    m_aTreeHolder = aOther.m_aTreeHolder;
    return *this;
}
//-----------------------------------------------------------------------------

ElementTree::~ElementTree()
{
}
//-----------------------------------------------------------------------------

bool ElementTree::isValid() const
{
    return !!m_aTreeHolder.isValid();
}
//-----------------------------------------------------------------------------

ElementTreeHolder ElementTree::get() const
{
    return m_aTreeHolder;
}
//-----------------------------------------------------------------------------

ElementTreeImpl* ElementTree::getImpl() const
{
    return m_aTreeHolder.getBodyPtr();
}
//-----------------------------------------------------------------------------

ISynchronizedData* ElementTree::getTreeLock() const
{
    return m_aTreeHolder->getRootLock();
}
//-----------------------------------------------------------------------------

TemplateHolder ElementTree::getTemplate() const
{
    OSL_PRECOND(isValid(),"ERROR: Trying to get the template of a NULL element tree");

    if (m_aTreeHolder,isValid())
        return m_aTreeHolder->getTemplate();
    else
        return TemplateHolder();
}
//-----------------------------------------------------------------------------

ElementTreeImpl* ElementTree::operator->() const
{
    return m_aTreeHolder.operator->();
}
//-----------------------------------------------------------------------------

ElementTreeImpl& ElementTree::operator*() const
{
    return m_aTreeHolder.operator*();
}
//-----------------------------------------------------------------------------

Tree ElementTree::getTree() const
{
    return Tree(m_aTreeHolder.getBodyPtr());
}
//-----------------------------------------------------------------------------

ElementTree ElementTree::extract(Tree const& aTree)
{
    TreeImpl* pTree = TreeImplHelper::impl(aTree);
    ElementTreeImpl* pImpl = pTree ? pTree->asElementTree() : 0;
    return ElementTree(pImpl);
}
//-----------------------------------------------------------------------------

void ElementTree::releaseOwnedElement(std::auto_ptr<INode>& rNewOwner, ElementTree const& aElementTree)
{
    OSL_PRECOND(aElementTree.isValid(),"ERROR: Trying to take over the content of a NULL element tree");
    OSL_PRECOND(aElementTree->isFree(),"Trying to take over the content of a owned element tree - returning NULL");

    aElementTree->releaseTo(rNewOwner);
}

//-----------------------------------------------------------------------------
void ElementTree::releaseOwnedElementAs(std::auto_ptr<INode>& rNewOwner, ElementTree const& aElementTree, Name const& aNewName)
{
    OSL_PRECOND(aElementTree.isValid(),"ERROR: Trying to take over the content of a NULL element tree");
    OSL_PRECOND(aElementTree->isFree(),"Trying to take over the content of a owned element tree - returning NULL");

    aElementTree->releaseAs(rNewOwner,aNewName);
}

//-----------------------------------------------------------------------------
// class SetElementInfo
//-----------------------------------------------------------------------------

SetElementInfo::SetElementInfo(TemplateHolder const& aTemplate)
: m_aTemplate(aTemplate)
{
    OSL_ENSURE(m_aTemplate.isValid(), "ERROR: Configuration: Creating element info without template information");
    if (!m_aTemplate.isValid())
        throw configuration::Exception("Creating element info without template information");
}
//-----------------------------------------------------------------------------
/*SetElementInfo::SetElementInfo(UnoType const& aElementType)
: m_aTemplate( makeSimpleTemplate(aElementType))
{
    OSL_ENSURE(m_aTemplate.isValid(), "ERROR: Configuration: Cannot create template wrapper for simple type");
    if (!m_aTemplate.isValid())
        throw configuration::Exception("Cannot create template wrapper for simple type");
}*/
//-----------------------------------------------------------------------------

TemplateHolder SetElementInfo::getTemplate() const
{
    return m_aTemplate;
}
//-----------------------------------------------------------------------------

UnoType SetElementInfo::getElementType() const
{
    return m_aTemplate->getInstanceType();
}

//-----------------------------------------------------------------------------

Name SetElementInfo::getTemplateName() const
{
    return m_aTemplate->getName();
}
//-----------------------------------------------------------------------------

Name SetElementInfo::getTemplatePackage() const
{
    return m_aTemplate->getModule();
}
//-----------------------------------------------------------------------------

RelativePath SetElementInfo::getTemplatePath() const
{
    return m_aTemplate->getPath();
}
//-----------------------------------------------------------------------------

TemplateHolder SetElementInfo::extractElementInfo(Tree const& aTree, NodeRef const& aNode)
{
    OSL_ENSURE(!aTree.isEmpty(), "ERROR: Getting Element Info requires a valid tree");
    OSL_ENSURE(aNode.isValid(), "ERROR: Getting Element Info requires a valid node");
    OSL_ENSURE(aTree.isValidNode(aNode), "ERROR: Tree/Node mismatch");
    if (aNode.isValid() )
    {
        Node* pNode = TreeImplHelper::node(aNode);
        OSL_ENSURE (pNode->isSetNode(), "WARNING: Getting Element Info requires a SET node");
        if (pNode->isSetNode())
            return pNode->setImpl().getElementTemplate();
    }
    return TemplateHolder();
}

//-----------------------------------------------------------------------------
// class SetElementFactory
//-----------------------------------------------------------------------------

SetElementFactory::SetElementFactory(TemplateProvider const& aProvider)
: m_aProvider(aProvider)
{
    OSL_ENSURE(aProvider.m_aImpl.isValid(), "WARNING: Template Instance Factory created without template provider - cannot instantiate elements");
}
//-----------------------------------------------------------------------------

SetElementFactory::SetElementFactory(SetElementFactory const& aOther)
: m_aProvider(aOther.m_aProvider)
{
}
//-----------------------------------------------------------------------------

SetElementFactory& SetElementFactory::operator=(SetElementFactory const& aOther)
{
    m_aProvider = aOther.m_aProvider;
    return *this;
}
//-----------------------------------------------------------------------------

SetElementFactory::~SetElementFactory()
{
}

//-----------------------------------------------------------------------------
ElementTree SetElementFactory::instantiateTemplate(TemplateHolder const& aTemplate)
{
    OSL_ENSURE(m_aProvider.m_aImpl.isValid(), "ERROR: Template Instance Factory has no template provider - cannot instantiate element");
    OSL_ENSURE(aTemplate.isValid(), "ERROR: Template is NULL - cannot instantiate element");

    if (!m_aProvider.m_aImpl.isValid()) return ElementTree( 0 );
    if (!aTemplate.isValid()) return ElementTree( 0 );

    std::auto_ptr<INode> aInstanceNode( m_aProvider.m_aImpl->instantiate(aTemplate) );
    OSL_ENSURE(aInstanceNode.get(), "ERROR: Cannot create Element Instance: Provider could not instantiate template");

    if (!aInstanceNode.get()) return ElementTree( 0 );

    ElementTree aRet( new ElementTreeImpl( aInstanceNode, aTemplate, m_aProvider ) );

    return aRet;
}
//-----------------------------------------------------------------------------
ElementTree SetElementFactory::instantiateOnDefault(std::auto_ptr<INode> aTree, TemplateHolder const& aDummyTemplate)
{
//  OSL_ENSURE(m_aProvider.get().isValid(), "ERROR: Template Instance Factory has no template provider - cannot instantiate element");
    OSL_ENSURE(aTree.get(), "ERROR: Tree is NULL - cannot instantiate element");
    OSL_ENSURE(aDummyTemplate.isValid(), "ERROR: Template is NULL - cannot instantiate element");

    if (!aTree.get()) return ElementTree( 0 );

    ElementTree aRet( new ElementTreeImpl( aTree, aDummyTemplate, m_aProvider ) );
    // ElementTreeImpl* pNewTree = new ElementTreeImpl( NodeType::getDeferredChangeFactory(),*aTree, c_TreeDepthAll, aDummyTemplate, m_aProvider );
    // pNewTree->takeNodeFrom(aTree);

    // return ElementTree( pNewTree );

    return aRet;
}
//-----------------------------------------------------------------------------

TemplateProvider SetElementFactory::findTemplateProvider(Tree const& aTree, NodeRef const& aNode)
{
    OSL_ENSURE(!aTree.isEmpty(), "ERROR: Getting Element Factory requires a valid tree");
    OSL_ENSURE(aNode.isValid(), "ERROR: Getting Element Factory requires a valid node");
    OSL_ENSURE(aTree.isValidNode(aNode), "ERROR: Tree/Node mismatch");
    if (aNode.isValid() )
    {
        Node* pNode = TreeImplHelper::node(aNode);
        OSL_ENSURE (pNode->isSetNode(), "WARNING: Getting Element Factory requires a SET node");
        if (pNode->isSetNode())
            return pNode->setImpl().getTemplateProvider();
    }
    return TemplateProvider();
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class TreeSetUpdater and ValueSetUpdater
//-----------------------------------------------------------------------------

// Value Element Factory methods
//-----------------------------------------------------------------------------

ElementTreeHolder ValueSetUpdater::makeValueElement(Name const& aName, UnoAny const& aValue)
{
    UnoType aType = m_aTemplate->getInstanceType();

    std::auto_ptr<INode> pNode;
    if (aValue.hasValue())
        pNode.reset( new ValueNode(aName.toString(),aValue, m_aTemplate->getAttributes()) );
    else
        pNode.reset( new ValueNode(aName.toString(),aType, m_aTemplate->getAttributes()) );

    return new ElementTreeImpl(pNode, m_aTemplate, TemplateProvider() );
}
//-----------------------------------------------------------------------------

ElementTreeHolder ValueSetUpdater::makeValueElement(Name const& aName, ElementNodeRef const& , UnoAny const& aValue)
{
    // for now ignoring the node.
    // TODO: merge attributes etc. from that node's value
    return makeValueElement(aName, aValue);
}
//-----------------------------------------------------------------------------


TreeSetUpdater::TreeSetUpdater(Tree const& aParentTree, NodeRef const& aSetNode, SetElementInfo const& aInfo)
: m_aParentTree(aParentTree)
, m_aSetNode(aSetNode)
, m_aTemplate(aInfo.getTemplate())
{
    implValidateSet();
}
//-----------------------------------------------------------------------------

ValueSetUpdater::ValueSetUpdater(Tree const& aParentTree, NodeRef const& aSetNode,
                                 SetElementInfo const& aInfo, UnoTypeConverter const& xConverter)
: m_aParentTree(aParentTree)
, m_aSetNode(aSetNode)
, m_aTemplate(aInfo.getTemplate())
, m_xTypeConverter(xConverter)
{
    implValidateSet();
}
//-----------------------------------------------------------------------------

/// validates that a actual set and an updater's construction parameters match
static void doValidateSet(Tree const& aParentTree, NodeRef const& aSetNode)
{
    if (aParentTree.isEmpty())
        throw Exception("INTERNAL ERROR: Set Update: Unexpected NULL tree");

    if (!aSetNode.isValid())
        throw Exception("INTERNAL ERROR: Set Update: Unexpected NULL node");

    if (!aParentTree.isValidNode(aSetNode))
        throw Exception("INTERNAL ERROR: Set Update: node does not match tree");

    if (! TreeImplHelper::isSet(aSetNode))
        throw Exception("INTERNAL ERROR: Set Update: node is not a set");

    if (!aParentTree.getAttributes(aSetNode).bWritable)
        throw ConstraintViolation( "Set Update: Set is read-only !" );
}
//-----------------------------------------------------------------------------

/// validates that the actual set and the construction parameters match
void TreeSetUpdater::implValidateSet()
{
    doValidateSet(m_aParentTree,m_aSetNode);

    if (!m_aTemplate.isValid())
        throw Exception("INTERNAL ERROR: No template available for tree set update");

    if (m_aTemplate->isInstanceValue())
        throw Exception("INTERNAL ERROR: Tree set update invoked on a value-set");

    if ( TreeImplHelper::node(m_aSetNode)->setImpl().getElementTemplate() != m_aTemplate)
        throw Exception("INTERNAL ERROR: Set Update: template mismatch");
}
//-----------------------------------------------------------------------------

/// validates that the actual set and the construction parameters match
void ValueSetUpdater::implValidateSet()
{
    doValidateSet(m_aParentTree,m_aSetNode);

    UnoType aThisType = m_aTemplate->getInstanceType();

    switch ( aThisType.getTypeClass())
    {
    case uno::TypeClass_VOID:       throw Exception("INTERNAL ERROR: Value set element type is void");
    case uno::TypeClass_INTERFACE:  throw Exception("INTERNAL ERROR: Value update invoked on a complex set");

    case uno::TypeClass_STRUCT:
    case uno::TypeClass_EXCEPTION:  throw Exception("INTERNAL ERROR: Unexpected/Invalid type for set elements");

    default: break;
    }

    if ( TreeImplHelper::node(m_aSetNode)->setImpl().getElementTemplate()->getInstanceType() != aThisType)
        throw Exception("INTERNAL ERROR: Set Update: element type mismatch");
}
//-----------------------------------------------------------------------------

static void doValidateElement(ElementRef const& aElement)
{
    if (!aElement.isValid())
        throw Exception("INTERNAL ERROR: Set Update: Unexpected NULL element");

// DISABLED: replaceable/removable != writable
//  if (!aElement.getAttributes().writable)
//      throw ConstraintViolation( "Set Update: Existing element is read-only !" );

// DISABLED: replaceable/removable != nullable
//  if (!aElement.getAttributes().nullable)
//      throw ConstraintViolation( "Set Update: Existing element cannot be removed (or replaced) !" );
}
//-----------------------------------------------------------------------------

/// validates that the given element is valid in this context and returns its name
Name TreeSetUpdater::implValidateElement(ElementRef const& aElement)
{
    doValidateElement(aElement);

//  ElementTreeImpl* pElement = TreeImplHelper::elementImpl(aTree)->isTemplateInstance();
//  OSL_ENSURE( pElement, "INTERNAL ERROR: Set Element has wrong type of tree");
//  OSL_ENSURE( !pElement || pElement->isTemplateInstance(), "INTERNAL ERROR: Set Element without associated template found");
//  OSL_ENSURE( !pElement || pElement->isInstanceOf(m_aTemplate), "INTERNAL ERROR: Set Update: existing element does not match template");

    return aElement.getName();
}
//-----------------------------------------------------------------------------

/// validates that the given element is valid and can be replaced in this context and returns its name
Name ValueSetUpdater::implValidateElement(ElementRef const& aElement)
{
    doValidateElement(aElement);

#ifdef _DEBUG
    UnoType aNodeType = ElementHelper::getUnoType(aElement);

    OSL_ENSURE(aNodeType.getTypeClass() != uno::TypeClass_VOID, "INTERNAL ERROR: Set Element without associated type found");
    OSL_ENSURE(aNodeType.getTypeClass() != uno::TypeClass_INTERFACE,"INTERNAL ERROR: Set Element with complex type found");

    OSL_ENSURE(aNodeType == m_aTemplate->getInstanceType() ||
               uno::TypeClass_ANY == m_aTemplate->getInstanceType().getTypeClass(),
               "INTERNAL ERROR: Set Update: existing element does not match template type");
#endif

    return aElement.getName();
}
//-----------------------------------------------------------------------------
static void checkEligibleChild(ElementTree const& aElementTree, Tree const& aParentTree)
{
    ElementTreeImpl const * const pElement = aElementTree.getImpl();    OSL_ASSERT(pElement);

    if (pElement->getContextTree() != NULL)
        throw ConstraintViolation( "Set Update: cannot insert an element that already has a parent." );

    TreeImpl const* pAncestor = TreeImplHelper::impl(aParentTree);
    while (pAncestor != NULL)
    {
        if (pElement == pAncestor)
            throw ConstraintViolation( "Set Update: Circular insertion - trying to insert an element into self or descendant" );

        pAncestor = pAncestor->getContextTree();
        OSL_ENSURE(pAncestor != TreeImplHelper::impl(aParentTree), "ERROR: Circular tree found");
    }
}

//-----------------------------------------------------------------------------

void TreeSetUpdater::implValidateTree(ElementTree const& aElementTree)
{
    if (!aElementTree.isValid())
        throw ConstraintViolation( "Set Update: cannot replace element of complex set with NULL node. Remove the element instead !" );

    checkEligibleChild(aElementTree,m_aParentTree);

    if (! aElementTree->isTemplateInstance())
    {
        throw TypeMismatch(OUString(RTL_CONSTASCII_USTRINGPARAM("<Unnamed> (Template missing)")),
                                    m_aTemplate->getName().toString(), " - new element without template in Set Update");
    }

    if (!aElementTree->isInstanceOf(m_aTemplate))
    {
        throw TypeMismatch( aElementTree->getTemplate()->getPath().toString(),
                            m_aTemplate->getPath().toString(), " - new element without template in Set Update");
    }
}
//-----------------------------------------------------------------------------

UnoAny ValueSetUpdater::implValidateValue(UnoAny const& aValue)
{
    UnoType aValType        = aValue.getValueType();
    UnoType aThisType       = m_aTemplate->getInstanceType();

    if (aValType.getTypeClass() == uno::TypeClass_INTERFACE)
        throw TypeMismatch(aValType.getTypeName(), aThisType.getTypeName(), " - cannot replace value by complex tree in Set update");

    UnoAny aRet(aValue);
    if (aValue.hasValue())
    {
        if (aValType != aThisType && uno::TypeClass_ANY != aThisType.getTypeClass())
        {
            if (!convertCompatibleValue(m_xTypeConverter, aRet, aValue, aThisType))
                throw TypeMismatch(aValType.getTypeName(), aThisType.getTypeName(), " - new element does not match template in SetUpdate");
        }
    }
    else
    {
        //  cannot do anything about null values here
        OSL_ASSERT(aValType.getTypeClass() == uno::TypeClass_VOID);

    }
    return aRet;
}
//-----------------------------------------------------------------------------

UnoAny ValueSetUpdater::implValidateValue(ElementNodeRef const& aElementTree, UnoAny const& aValue)
{
    Attributes aAttributes = aElementTree.getAttributes(aElementTree.getRootNode());
    // Here we assume writable == removable/replaceable
    if (!aAttributes.bWritable)
        throw ConstraintViolation( "Set Update: Existing element is read-only !" );

    // Here we assume nullable != removable
    if (!aValue.hasValue())
    {
        if (!aAttributes.bNullable)
            throw ConstraintViolation( "Set Update: Value is not nullable !" );
    }
    return implValidateValue( aValue);
}
//-----------------------------------------------------------------------------

NodeChange TreeSetUpdater::validateInsertElement (Name const& aName, ElementTree const& aNewElement)
{
    SetEntry anEntry = TreeImplHelper::node(m_aSetNode)->setImpl().findElement(aName);
    if (anEntry.isValid())
        throw Exception("INTERNAL ERROR: Set Update: Element to be inserted already exists");

    implValidateTree(aNewElement);

    std::auto_ptr<SetChangeImpl> pChange( new SetInsertTreeImpl(aName, aNewElement.get()) );

    pChange->setTarget(TreeImplHelper::impl(m_aParentTree), TreeImplHelper::offset(m_aSetNode));

    return NodeChange(pChange.release());
}
//-----------------------------------------------------------------------------

NodeChange ValueSetUpdater::validateInsertElement (Name const& aName, UnoAny const& aNewValue)
{
    SetEntry anEntry = TreeImplHelper::node(m_aSetNode)->setImpl().findElement(aName);
    if (anEntry.isValid())
        throw Exception("INTERNAL ERROR: Set Update: Element to be inserted already exists");

    UnoAny aValidValue = implValidateValue(aNewValue);

    std::auto_ptr<SetChangeImpl> pChange( new SetInsertValueImpl(aName, makeValueElement(aName, aValidValue)) );

    pChange->setTarget(TreeImplHelper::impl(m_aParentTree), TreeImplHelper::offset(m_aSetNode));

    return NodeChange(pChange.release());
}
//-----------------------------------------------------------------------------

NodeChange TreeSetUpdater::validateReplaceElement(ElementRef const& aElement, ElementTree const& aNewElement)
{
    Name aName = implValidateElement(aElement);

    implValidateTree(aNewElement);

    std::auto_ptr<SetChangeImpl> pChange( new SetReplaceTreeImpl(aName, aNewElement.get()) );

    pChange->setTarget(TreeImplHelper::impl(m_aParentTree), TreeImplHelper::offset(m_aSetNode));

    return NodeChange(pChange.release());
}
//-----------------------------------------------------------------------------

NodeChange ValueSetUpdater::validateReplaceElement(ElementRef const& aElement, UnoAny const& aNewValue)
{
    Name aName = implValidateElement(aElement);

    ElementNodeRef aElementNode = extractElementNode(aElement);

    UnoAny aValidValue = implValidateValue(aElementNode, aNewValue);

    std::auto_ptr<SetChangeImpl> pChange( new SetReplaceValueImpl(aName, makeValueElement(aName, aElementNode,aValidValue)) );

    pChange->setTarget(TreeImplHelper::impl(m_aParentTree), TreeImplHelper::offset(m_aSetNode));

    return NodeChange(pChange.release());
}
//-----------------------------------------------------------------------------

NodeChange TreeSetUpdater::validateRemoveElement (ElementRef const& aElement)
{
    Name aName = implValidateElement(aElement);

    std::auto_ptr<SetChangeImpl> pChange( new SetRemoveTreeImpl(aName) );

    pChange->setTarget(TreeImplHelper::impl(m_aParentTree), TreeImplHelper::offset(m_aSetNode));

    return NodeChange(pChange.release());
}

//-----------------------------------------------------------------------------

NodeChange ValueSetUpdater::validateRemoveElement (ElementRef const& aElement)
{
    Name aName = implValidateElement(aElement);

    std::auto_ptr<SetChangeImpl> pChange( new SetRemoveValueImpl(aName) );

    pChange->setTarget(TreeImplHelper::impl(m_aParentTree), TreeImplHelper::offset(m_aSetNode));

    return NodeChange(pChange.release());
}

//-----------------------------------------------------------------------------

static inline
Tree impl_extractElementTree (ElementRef const& aElement)
{
    return aElement.getElementTree().getTree();
}
//-----------------------------------------------------------------------------

static inline
NodeRef impl_extractElementNode (ElementRef const& aElement)
{
    return impl_extractElementTree(aElement).getRootNode();
}
//-----------------------------------------------------------------------------

ValueSetUpdater::ElementNodeRef ValueSetUpdater::extractElementNode (ElementRef const& aElement)
{
    return impl_extractElementTree(aElement);
}
//-----------------------------------------------------------------------------

UnoType ElementHelper::getUnoType(ElementRef const& aElement)
{
    OSL_PRECOND( aElement.isValid(), "ERROR: Configuration: ElementRef operation requires valid node" );
    if (!aElement.isValid()) return getVoidCppuType();

    NodeRef aNode = impl_extractElementNode(aElement);
    OSL_ASSERT( aNode.isValid() );

    if (TreeImplHelper::isValueElement(aNode))
    {
        return TreeImplHelper::node(aNode)->valueElementImpl().getValueType();
    }
    else
    {
        uno::Reference< uno::XInterface > const * const selectInterface=0;
        return ::getCppuType(selectInterface);
    }
}
//-----------------------------------------------------------------------------

Name ElementHelper::getElementName(ElementRef const& aElement)
{
    OSL_PRECOND( aElement.isValid(), "ERROR: Configuration: ElementRef operation requires valid node" );

    Tree aElementTree = impl_extractElementTree(aElement);

    OSL_ASSERT( aElementTree.isEmpty() == !aElement.isValid() );

    return aElementTree.getRootName();
}
//-----------------------------------------------------------------------------
    }
}

