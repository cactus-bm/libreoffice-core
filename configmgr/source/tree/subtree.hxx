/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: subtree.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 23:30:16 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef CONFIGMGR_SUBTREE_HXX
#define CONFIGMGR_SUBTREE_HXX

#include "valuenode.hxx"

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#include <memory>
#include <set>

namespace configmgr
{

// ---------------------------------- STL set ----------------------------------
    struct ltNode
    {
        bool operator()(const configmgr::INode* n1, const configmgr::INode* n2) const
            {
                // return strcmp(n1->GetName(), n2->GetName()) < 0;
                return (n1->getName().compareTo(n2->getName()) < 0) ? 1 : 0;
            }
    };


    typedef std::set<INode*, ltNode> ChildList;

    class ChildListSet {
        ChildList m_aChildList;

        ChildListSet(ChildListSet const&);
        ChildListSet& operator=(ChildListSet const& aSet);
    public:
        ChildList& GetSet() {return m_aChildList;}
        ChildList const& GetSet() const {return m_aChildList;}

        ChildListSet() {}
        ChildListSet(ChildListSet const&, treeop::DeepChildCopy);
        ~ChildListSet();
    };

// Inner Node
    class Subtree : public ISubtree
    {
        //  set<Node*> children;
        ChildListSet        m_aChildren;
        virtual INode* doGetChild(OUString const& name) const;

    public:
        typedef treeop::NoChildCopy NoChildCopy;

        Subtree(){}
        Subtree(const OUString& _rName,
                const node::Attributes& _rAttrs)
                :ISubtree(_rName, _rAttrs){};

        Subtree(const ISubtree& _rOther, NoChildCopy)
            : ISubtree(_rOther), m_aChildren(){};

        Subtree(const OUString& _rName,
                const OUString& _rTemplateName, const OUString& _rTemplateModule,
                const node::Attributes& _rAttrs)
                :ISubtree(_rName, _rTemplateName, _rTemplateModule, _rAttrs){};

        Subtree(const Subtree& _rOther, treeop::DeepChildCopy _dc)
            : ISubtree(_rOther), m_aChildren(_rOther.m_aChildren,_dc){}

        virtual INode* addChild(std::auto_ptr<INode> node); // takes ownership
        virtual ::std::auto_ptr<INode> removeChild(OUString const& name);

        virtual std::auto_ptr<INode> clone() const;

// Iteration support
        virtual void forEachChild(NodeAction& anAction) const;
        virtual void forEachChild(NodeModification& anAction);

        // "rtti"
        RTTI(Subtree, ISubtree);
    };



    // to search in ChildListSet a value
    class SearchNode : public INode
    {
        // why you want to search with no names.
        SearchNode();

    public:
        SearchNode(OUString const& aName);
        virtual ~SearchNode();
        virtual std::auto_ptr<INode> clone() const;

// double dispatch support
            virtual void dispatch(NodeAction& /*anAction*/) const { }
            virtual void dispatch(NodeModification& /*anAction*/) { }
    };

// -----------------------------------------------------------------------------
} // namespace configmgr

#endif

