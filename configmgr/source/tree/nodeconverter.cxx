/*************************************************************************
 *
 *  $RCSfile: nodeconverter.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jb $ $Date: 2002-02-11 13:47:55 $
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

#include <stdio.h>

#include "nodeconverter.hxx"

#ifndef CONFIGMGR_TREE_NODEFACTORY_HXX
#include "treenodefactory.hxx"
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif


//..........................................................................
namespace configmgr
{

//==========================================================================
//= OTreeNodeConverter
//==========================================================================
OTreeNodeConverter::OTreeNodeConverter()
: m_rFactory( getDefaultTreeNodeFactory() )
{
}

std::auto_ptr<ISubtree>  OTreeNodeConverter::createCorrespondingNode(SubtreeChange const& _rChange)
{
    std::auto_ptr<ISubtree> aRet;

    //if ( isLocalizedValueSet(aSubtree) ) { ... } else - no special case yet
    if (_rChange.isSetNodeChange())
    {
        aRet = nodeFactory().createSetNode(_rChange.getNodeName(),
                                       _rChange.getElementTemplateName(),
                                       _rChange.getElementTemplateModule(),
                                       _rChange.getAttributes());
    }
    else
    {
        aRet = nodeFactory().createGroupNode(_rChange.getNodeName(),
                                           _rChange.getAttributes());
    }
    return aRet;
}

//--------------------------------------------------------------------------
std::auto_ptr<ValueNode> OTreeNodeConverter::createCorrespondingNode(ValueChange const&  _rChange)
{
   // DEFAULT-TODO
    OSL_ENSURE(_rChange.getValueType().getTypeClass() != uno::TypeClass_VOID, "Losing type information converting change to value");

    std::auto_ptr<ValueNode> aRet;
    if (_rChange.getNewValue().hasValue())
       aRet = nodeFactory().createValueNode(_rChange.getNodeName(), _rChange.getNewValue(), _rChange.getAttributes());

    else
       aRet = nodeFactory().createNullValueNode(_rChange.getNodeName(), _rChange.getValueType(), _rChange.getAttributes());

    OSL_ENSURE(aRet.get() && aRet->isValid(), "Could not create corresponding value node");

    return aRet;
}

//==========================================================================
//= ONodeConverter
//==========================================================================
class ONodeConverter : public ChangeTreeModification
{
    OTreeNodeConverter&     m_rFactory;
    std::auto_ptr<INode>    m_pNode;

public:
    explicit
    ONodeConverter(OTreeNodeConverter& rFactory)
        : m_rFactory(rFactory)
    {
    }

    virtual void handle(ValueChange& aValueNode);
    virtual void handle(AddNode& aAddNode);
    virtual void handle(RemoveNode& aRemoveNode);
    virtual void handle(SubtreeChange& aSubtree);

    std::auto_ptr<INode> result() { return m_pNode; }

};
//==========================================================================
//= OCreateSubtreeAction
//==========================================================================
//= creates a subtree out of a changes list
//==========================================================================
struct OCreateSubtreeAction : public ChangeTreeModification
{
    ISubtree&           m_rTree;
    OTreeNodeConverter& m_rNodeFactory;

public:
    OCreateSubtreeAction(ISubtree& _rTree, OTreeNodeConverter& rFactory)
        :m_rTree(_rTree)
        ,m_rNodeFactory(rFactory) {}

    void handle(ValueChange& aValueNode);
    void handle(AddNode& aAddNode);
    void handle(RemoveNode& aRemoveNode);
    void handle(SubtreeChange& aSubtree);
};


//--------------------------------------------------------------------------
std::auto_ptr<INode> OTreeNodeConverter::createCorrespondingNode(Change& _rChange)
{
    ONodeConverter aAction(*this);
    aAction.applyToChange(_rChange);
    return aAction.result();
}

//--------------------------------------------------------------------------
std::auto_ptr<ISubtree> OTreeNodeConverter::createCorrespondingTree(SubtreeChange& _rChange)
{
    std::auto_ptr<ISubtree> pBaseTree = this->createCorrespondingNode(_rChange);

    OCreateSubtreeAction aNextLevel(*pBaseTree,*this);
    _rChange.forEachChange(aNextLevel);

    return pBaseTree;
}

//--------------------------------------------------------------------------
void ONodeConverter::handle(ValueChange& aValueNode)
{
    m_pNode = base_ptr(m_rFactory.createCorrespondingNode(aValueNode));
}

//--------------------------------------------------------------------------
void ONodeConverter::handle(AddNode& aAddNode)
{
    m_pNode = aAddNode.getNewTree().cloneData(true);
}

//--------------------------------------------------------------------------
void ONodeConverter::handle(RemoveNode& aRemoveNode)
{
    m_pNode.reset();
}

//--------------------------------------------------------------------------
void ONodeConverter::handle(SubtreeChange& aSubtree)
{
    m_pNode = base_ptr(m_rFactory.createCorrespondingNode(aSubtree));
}

//--------------------------------------------------------------------------
void OCreateSubtreeAction::handle(ValueChange& _rChange)
{
    // create a node by a ValueChange
    std::auto_ptr<ValueNode> pNode = m_rNodeFactory.createCorrespondingNode(_rChange);

    m_rTree.addChild(base_ptr(pNode));
}

//--------------------------------------------------------------------------
void OCreateSubtreeAction::handle(SubtreeChange& _rChange)
{
    // create a node from a SubtreeChange (recursively)
    std::auto_ptr<ISubtree> pNode = m_rNodeFactory.createCorrespondingTree(_rChange);

    // add it to the tree
    m_rTree.addChild(base_ptr(pNode));
}

//--------------------------------------------------------------------------
void OCreateSubtreeAction::handle(RemoveNode& _rChange)
{
    // we have nothing to do
    OSL_ENSURE(!m_rTree.getChild(_rChange.getNodeName()), "Removed node found in tree being built");
}

//--------------------------------------------------------------------------
void OCreateSubtreeAction::handle(AddNode& _rChange)
{
    // free the node and add it to the subtree
    data::TreeSegment aNewNode = _rChange.getNewTree();
    m_rTree.addChild(aNewNode.cloneData(true));
}


//--------------------------------------------------------------------------

//..........................................................................
}   // namespace configmgr
//..........................................................................


