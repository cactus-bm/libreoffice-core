/*************************************************************************
 *
 *  $RCSfile: treeactions.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: jb $ $Date: 2001-09-28 12:44:15 $
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

#ifndef _CONFIGMGR_TREEACTIONS_HXX_
#define _CONFIGMGR_TREEACTIONS_HXX_

#ifndef CONFIGMGR_CHANGE_HXX
#include "change.hxx"
#endif

//..........................................................................
namespace configmgr
{
//..........................................................................
class OTreeNodeFactory;
extern OTreeNodeFactory& getDefaultTreeNodeFactory();
//==========================================================================
//= OIdPropagator
//==========================================================================
/** propagates a node id to a subtree and its descendants
*/
class OIdPropagator : private NodeModification
{
    ::rtl::OUString sId;

    OIdPropagator(const ::rtl::OUString& _rId) : sId(_rId) { }
    virtual void handle(ValueNode& _rValueNode);
    virtual void handle(ISubtree& _rSubtree);

public:
    static void propagateIdToChildren(ISubtree& rTree);
    static void propagateIdToTree( ::rtl::OUString const& aId, ISubtree& rTree);
};

//==========================================================================
//= OIdRemover
//==========================================================================
/** propagates a node id to a subtree and its descendants
*/
class OIdRemover : private NodeModification
{
    virtual void handle(ValueNode& _rValueNode);
    virtual void handle(ISubtree& _rSubtree);

public:
    static void removeIds(INode& rNode);
};

//==========================================================================
//= OChangeActionCounter
//==========================================================================
/** collects meta data about a changes tree
*/
struct OChangeActionCounter : public ChangeTreeAction
{
    sal_Int32       nValues, nAdds, nRemoves;

    OChangeActionCounter() :nValues(0), nAdds(0), nRemoves(0) {}

    virtual void handle(ValueChange const& aValueNode);
    virtual void handle(AddNode const& aAddNode);
    virtual void handle(RemoveNode const& aRemoveNode);
    virtual void handle(SubtreeChange const& aSubtree);

    sal_Bool hasChanges() const {return nValues || nAdds || nRemoves;}
};

//==========================================================================
//= ONodeConverter
//==========================================================================

class ONodeConverter : public ChangeTreeModification
{
    OTreeNodeFactory&       m_rFactory;
    std::auto_ptr<INode>    m_pNode;
public:
    static std::auto_ptr<ISubtree>  createCorrespondingNode(SubtreeChange   const& _rChange, OTreeNodeFactory& _rFactory);
    static std::auto_ptr<ValueNode> createCorrespondingNode(ValueChange     const& _rChange, OTreeNodeFactory& _rFactory);
    static std::auto_ptr<INode>     extractCorrespondingNode(AddNode& _rChange);
    static std::auto_ptr<INode>     convertToNode(Change& _rChange, OTreeNodeFactory& _rFactory);
public:
    explicit
    ONodeConverter(OTreeNodeFactory& rFactory)
        : m_rFactory(rFactory)
    {
    }

    virtual void handle(ValueChange& aValueNode);
    virtual void handle(AddNode& aAddNode);
    virtual void handle(RemoveNode& aRemoveNode);
    virtual void handle(SubtreeChange& aSubtree);
};

//==========================================================================
//= OMergeTreeAction
//==========================================================================
//= This class tests changes on an existing tree and drops them if they
//= are not need anymore or alters add nodes in node changes and vice versa
//==========================================================================
struct OMergeTreeAction : public ChangeTreeModification
{
    SubtreeChange&      m_rChangeList;  // list which containes changes merged with the existing nodes
    const ISubtree*     m_pRefTree;     // reference node needed for merging
    OTreeNodeFactory&   m_rNodeFactory;
public:
    OMergeTreeAction(SubtreeChange& rList, const ISubtree* pTree)
        :m_rChangeList(rList)
        ,m_pRefTree(pTree)
        ,m_rNodeFactory( getDefaultTreeNodeFactory())
    {}

    OMergeTreeAction(SubtreeChange& rList, const ISubtree* pTree, OTreeNodeFactory& rFactory )
        :m_rChangeList(rList)
        ,m_pRefTree(pTree)
        ,m_rNodeFactory(rFactory)
    {}

    void handle(ValueChange& aValueNode);
    void handle(AddNode& aAddNode);
    void handle(RemoveNode& aRemoveNode);
    void handle(SubtreeChange& aSubtree);

private:
    // ensuring the correct state
    bool checkTree() const;
};

//==========================================================================
//= OCreateSubtreeAction
//==========================================================================
//= creates a subtree out of a changes list
//==========================================================================
struct OCreateSubtreeAction : public ChangeTreeModification
{
    ISubtree&           m_rTree;
    OTreeNodeFactory&   m_rNodeFactory;

public:
    OCreateSubtreeAction(ISubtree& _rTree, OTreeNodeFactory& rFactory)
        :m_rTree(_rTree)
        ,m_rNodeFactory(rFactory) {}

    void handle(ValueChange& aValueNode);
    void handle(AddNode& aAddNode);
    void handle(RemoveNode& aRemoveNode);
    void handle(SubtreeChange& aSubtree);

    static std::auto_ptr<ISubtree> createSubtree(SubtreeChange& aSubtree, OTreeNodeFactory& rFactory);
};


//==========================================================================
//= OCloneForLocale
//==========================================================================
//= clones a subtree , in the process selecting only the best match locale
//= from the set representation of localized values
//==========================================================================
class OCloneForLocale : public NodeAction
{
    rtl::OUString           m_sTargetLocale;
    std::auto_ptr<INode>    m_pClone;
public:
    OCloneForLocale(OUString const& aLocale) : m_sTargetLocale(aLocale) {}
    std::auto_ptr<INode> getResult() { return m_pClone; }

private:
    void handle(ValueNode const& _aValue);
    void handle(ISubtree const&  _aSubtree);
};

//==========================================================================
//= OExpandLocalizedValues
//==========================================================================
//= clones a subtree , in the process expanding localized value nodes to a one-element set
//==========================================================================
class OExpandLocalizedValues : public NodeAction
{
    std::auto_ptr<INode>    m_pClone;
public:
    OExpandLocalizedValues() {}
    std::auto_ptr<INode> getResult() { return m_pClone; }

private:
    void handle(ValueNode const& _aValue);
    void handle(ISubtree const&  _aSubtree);
};
//==========================================================================
// Helper function to invoke the previous ones properly

// convert to the given locale format, no matter what the original representation
std::auto_ptr<INode> cloneForLocale(INode const* _pNode, OUString const& _sLocale);
// convert to the given locale format, assuming the original representation was expanded
std::auto_ptr<INode> cloneExpandedForLocale(INode const* _pNode, OUString const& _sLocale);
// convert to the given locale format, assuming the original representation was expanded
std::auto_ptr<INode> cloneExpandedForLocale(ISubtree const* _pNode, OUString const& _sLocale);
// convert to the given locale format, assuming the original representation was expanded
std::auto_ptr<INode> reduceExpandedForLocale(std::auto_ptr<ISubtree> _pNode, OUString const& _sLocale);

// ===================================================================
// = OChangeCounter
// ===================================================================
struct OChangeCounter : public ChangeTreeAction
{
    sal_Int32   nCount;
    OChangeCounter() : nCount(0) { }

    virtual void handle(ValueChange const& aValueNode);
    virtual void handle(AddNode const& aAddNode);
    virtual void handle(RemoveNode const& aRemoveNode);
    virtual void handle(SubtreeChange const& aSubtree);
};

// ===================================================================
// = CollectNames
// ===================================================================
class CollectNames :  public NodeAction
{
public:
    typedef std::vector<OUString> NameList;

public:
    NameList const& list() const { return aList; }

    CollectNames() : aList() {}

    void handle(ValueNode const& aValue)    { add(aValue); }
    void handle(ISubtree const&  m_aSubtree)    { add(m_aSubtree); }

    void add(INode const& aNode)
    {
        aList.push_back(aNode.getName());
    }
private:
    NameList aList;
};

// --------------------------------- TreeUpdate ---------------------------------

class TreeUpdate : public ChangeTreeModification
{
    ISubtree* m_pCurrentSubtree;
#if DBUG
    std::vector<OString> aLog;
#endif

public:
    TreeUpdate(ISubtree* pSubtree):m_pCurrentSubtree(pSubtree){}

    void handle(ValueChange& aValueNode);
    void handle(AddNode& aAddNode);
    void handle(RemoveNode& aRemoveNode);
    void handle(SubtreeChange& aSubtree);
};

//..........................................................................
}   // namespace configmgr
//..........................................................................

#endif // _CONFIGMGR_TREEACTIONS_HXX_


