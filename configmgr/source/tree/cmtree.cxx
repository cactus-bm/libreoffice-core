/*************************************************************************
 *
 *  $RCSfile: cmtree.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: jb $ $Date: 2001-12-07 10:39:46 $
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

#include "subtree.hxx"
#ifndef CONFIGMGR_CHANGE_HXX
#include "change.hxx"
#endif
#ifndef CONFIGMGR_TREECHANGELIST_HXX
#include "treechangelist.hxx"
#endif

#ifndef CONFIGMGR_TREEPROVIDER_HXX
#include "treeprovider.hxx"
#endif

//#include "treeactions.hxx"

#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef INCLUDED_DEQUE
#include <deque>
#define INCLUDED_DEQUE
#endif
#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif
#ifndef INCLUDED_IOSTREAM
#include <iostream>
#define INCLUDED_IOSTREAM
#endif
#ifndef INCLUDED_EXCEPTION
#include <exception>
#define INCLUDED_EXCEPTION
#endif
#ifndef INCLUDED_SET
#include <set>
#define INCLUDED_SET
#endif

using namespace std;
using namespace rtl;
using namespace com::sun::star::uno;

namespace configmgr
{

// ------------------------ ChildListSet implementations ------------------------
    ChildListSet::ChildListSet(ChildListSet const& aSet, treeop::DeepChildCopy)
    {
        for(ChildList::iterator it = aSet.GetSet().begin();
            it != aSet.GetSet().end();
            ++it)
            m_aChildList.insert(m_aChildList.end(), (*it)->clone());
    }
    ChildListSet::~ChildListSet()
    {
        for(ChildList::iterator it = m_aChildList.begin();
            it != m_aChildList.end();
            ++it)
            delete *it;
    }


// ---------------------------- Node implementation ----------------------------

    INode::INode(configuration::Attributes _aAttr):m_aAttributes(_aAttr){}
    INode::INode(OUString const& aName, configuration::Attributes _aAttr)
          :m_aName(aName)
          ,m_aAttributes(_aAttr){}
    // CopyCTor will be create automatically

    INode::~INode() {}

    ISubtree*         INode::asISubtree(){return NULL;}
    ISubtree const*   INode::asISubtree() const {return NULL;}
    ValueNode*       INode::asValueNode() {return NULL;}
    ValueNode const* INode::asValueNode() const {return NULL;}

    void INode::modifyState(node::State _eNewState)
    {
        m_aAttributes.setState(_eNewState);
    }

    void INode::modifyAccess(bool _bWritable,bool _bFinalized)
    {
        // this state can only occurs a s a result of forceWritableToFinalized()
        OSL_ENSURE(!(_bWritable && _bFinalized),"Invalid access state: Node is both writable and finalized");

        m_aAttributes.bWritable  = _bWritable;
        m_aAttributes.bFinalized = _bFinalized;
    }

      void INode::forceWritableToFinalized()
      {
        if (!m_aAttributes.bWritable)
        {
            m_aAttributes.bFinalized = true;
            m_aAttributes.bWritable  = true;
        }
    }

// ------------------------- SearchNode implementation -------------------------
    SearchNode::SearchNode():INode(configuration::Attributes()){}
    SearchNode::SearchNode(OUString const& aName)
        :INode(aName, configuration::Attributes()){}

    INode* SearchNode::clone() const {return new SearchNode(*this);}

    SearchNode::~SearchNode(){}

    //==========================================================================
    //= OPropagateLevels
    //==========================================================================
    /** fills a subtree with the correct level informations
    */
    struct OPropagateLevels : public NodeModification
    {
    public:
        typedef sal_Int16 Level;
        OPropagateLevels(Level _nParentLevel, Level _nParentDefaultLevel)
        : m_nLevel          ( childLevel(_nParentLevel) )
        , m_nDefaultLevel   ( childLevel(_nParentDefaultLevel) )
        {
        }
        virtual void handle(ValueNode&) { /* not interested in value nodes */ }
        virtual void handle(ISubtree& _rSubtree)
        {
            _rSubtree.setLevels(m_nLevel, m_nDefaultLevel);
        }

        static Level childLevel(Level _nLevel)
        {
            OSL_ASSERT(0 > ITreeProvider::ALL_LEVELS);
            return (_nLevel > 0) ? _nLevel-1 : _nLevel;
        }
    protected:
        Level   m_nLevel;
        Level   m_nDefaultLevel;
    };


// -------------------------- ISubtree implementation --------------------------
    ISubtree* ISubtree::asISubtree() {return this;}
    ISubtree const* ISubtree::asISubtree() const {return this;}

    //--------------------------------------------------------------------------
    static inline bool adjustLevel(sal_Int16& _rLevel, sal_Int16 _nNewLevel)
    {
        if (_rLevel == ITreeProvider::ALL_LEVELS)   return false;
        if (_nNewLevel <= _rLevel &&
            _nNewLevel != ITreeProvider::ALL_LEVELS) return false;

        _rLevel = _nNewLevel;
        return true;
    }

    //--------------------------------------------------------------------------
    void ISubtree::setLevels(sal_Int16 _nLevel, sal_Int16 _nDefaultLevels)
    {
        bool bActive = false;

        if (_nLevel && adjustLevel(m_nLevel, _nLevel))
            bActive = true;

        if (_nDefaultLevels && adjustLevel(m_nDefaultLevels, _nDefaultLevels))
            bActive = true;

        // forward the level numbers to any child subtrees we have
        if (bActive)
        {
            OPropagateLevels aPropagate(_nLevel,_nDefaultLevels);
            aPropagate.applyToChildren(*this);
        }
    }

// --------------------------- Subtree implementation ---------------------------
    INode* Subtree::clone() const {return new Subtree(*this, treeop::DeepChildCopy());}

    INode* Subtree::doGetChild(OUString const& aName) const
    {
        SearchNode searchObj(aName);

#ifdef DEBUG
        for (ChildList::iterator it2 = m_aChildren.GetSet().begin();
            it2 != m_aChildren.GetSet().end();
            ++it2)
        {
            INode* pINode = *it2;
            OUString aName2 = pINode->getName();
            volatile int dummy = 0;
        }
#endif

        ChildList::iterator it = m_aChildren.GetSet().find(&searchObj);
        if (it == m_aChildren.GetSet().end())
            return NULL;
        else
            return *it;
    }

    INode* Subtree::addChild(std::auto_ptr<INode> aNode)    // takes ownership
    {
        OUString aName = aNode->getName();
        std::pair<ChildList::iterator, bool> aInserted =
            m_aChildren.GetSet().insert(aNode.get());
        if (aInserted.second)
            aNode.release();
        return *aInserted.first;
    }

    ::std::auto_ptr<INode> Subtree::removeChild(OUString const& aName)
    {
        SearchNode searchObj(aName);
        ChildList::const_iterator it = m_aChildren.GetSet().find(&searchObj);

        ::std::auto_ptr<INode> aReturn;
        if (m_aChildren.GetSet().end() != it)
        {
            aReturn = ::std::auto_ptr<INode>(*it);
            m_aChildren.GetSet().erase(it);
        }
        return aReturn;
    }
//  // -------------------------- ValueNode implementation --------------------------

    void Subtree::forEachChild(NodeAction& anAction) const
    {
        for(ChildList::const_iterator it = m_aChildren.GetSet().begin();
            it != m_aChildren.GetSet().end();
            ++it)
            (**it).dispatch(anAction);
    }

    void Subtree::forEachChild(NodeModification& anAction)
    {
        ChildList::iterator it = m_aChildren.GetSet().begin();
        while( it != m_aChildren.GetSet().end() )
        {
            // modification-safe iteration
            (**it++).dispatch(anAction);
        }
      }

//  // -------------------------- ValueNode implementation --------------------------
    void ValueNode::setValue(Any const& _aValue)
    {
        m_aValuePair.setFirst(_aValue);
        this->markAsDefault(false);
    }

    void ValueNode::changeDefault(Any const& _aValue)
    {
        m_aValuePair.setSecond(_aValue);
    }

    void ValueNode::setDefault()
    {
        OSL_PRECOND( hasDefault(), "No default value to set for value node");
        m_aValuePair.clear( selectValue() );
        this->markAsDefault();
        OSL_POSTCOND( isDefault(), "Could not set value node to default");
    }

    INode* ValueNode::clone() const
    {
        return new ValueNode(*this);
    }

    ValueNode* ValueNode::asValueNode() {return this;}
    ValueNode const* ValueNode::asValueNode() const {return this;}

} // namespace configmgr


