/*************************************************************************
 *
 *  $RCSfile: nodeimplobj.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: jb $ $Date: 2002-02-11 13:47:56 $
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

#ifndef CONFIGMGR_NODEIMPLOBJECTS_HXX_
#define CONFIGMGR_NODEIMPLOBJECTS_HXX_

#ifndef CONFIGMGR_CONFIGNODEBEHAVIOR_HXX_
#include "nodeimpl.hxx"
#endif
#ifndef CONFIGMGR_GROUPNODEBEHAVIOR_HXX_
#include "groupnodeimpl.hxx"
#endif
#ifndef CONFIGMGR_SETNODEBEHAVIOR_HXX_
#include "setnodeimpl.hxx"
#endif
#ifndef CONFIGMGR_VALUENODEBEHAVIOR_HXX_
#include "valuenodeimpl.hxx"
#endif
#ifndef CONFIGMGR_NODEADDRESS_HXX
#include "nodeaddress.hxx"
#endif

#ifndef _SALHELPER_SIMPLEREFERENCEOBJECT_HXX_
#include <salhelper/simplereferenceobject.hxx>
#endif

#ifndef INCLUDED_MEMORY
#include <memory>
#define INCLUDED_MEMORY
#endif

namespace configmgr
{
    namespace configuration
    {
//-----------------------------------------------------------------------------

// Specific types of nodes for direct or read only access
//-----------------------------------------------------------------------------

// Value Nodes
//-----------------------------------------------------------------------------

        class ValueMemberNode::DeferredImpl : public salhelper::SimpleReferenceObject
        {
            data::ValueNodeAddress m_aValueRef;

            UnoAny      m_aNewValue;
            bool        m_bToDefault;
            bool        m_bChange;
        public:
            explicit DeferredImpl(data::ValueNodeAccess const& _aValueNode) ;

            /// does this wrap a change
            bool isChange() const   { return m_bChange; }

            /// retrieve the underlying (original) node location
            data::ValueNodeAddress getOriginalNodeAddress() const
            { return m_aValueRef; }

            /// retrieve the underlying (original) node
            data::ValueNodeAccess getOriginalNode(data::Accessor const& _aAccessor) const
            { return data::ValueNodeAccess(_aAccessor,m_aValueRef); }

            /// Does this node change to default
            bool isToDefault()      const { return m_bToDefault; }

            /// retrieve the current value of this node
            UnoAny  getNewValue()   const { return m_aNewValue; }

            /// Set this node to a new value
            void    setValue(UnoAny const& aNewValue, data::ValueNodeAccess const& _aOriginalNode);

            /// Set this node to assume its default value
            void    setValueToDefault(data::ValueNodeAccess const& _aOriginalNode);

        public:
            // commit protocol
            std::auto_ptr<ValueChange> preCommitChange(data::Accessor const& _aAccessor);
            void finishCommit(ValueChange& rChange, data::Accessor const& _aAccessor);
            void revertCommit(ValueChange& rChange, data::Accessor const& _aAccessor);
            void failedCommit(ValueChange& rChange, data::Accessor const& _aAccessor);

            // void commitDirect(data::Accessor const& _aAccessor);

            ValueChangeImpl* collectChange(data::Accessor const& _aAccessor);
            ValueChangeImpl* adjustToChange(ValueChange const& rExternalChange);

            // notification protocol
            void adjustToChange(NodeChangesInformation& rLocalChange, ValueChange const& rExternalChange, TreeImpl& rParentTree, NodeOffset nParentPos, Name const& aName);
        };
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

// Group Nodes
//-----------------------------------------------------------------------------

        class DeferredGroupNodeImpl : public GroupNodeImpl
        {
        public:
            explicit
            DeferredGroupNodeImpl(data::GroupNodeAddress const& _aNodeRef);
            explicit
            DeferredGroupNodeImpl(data::GroupNodeAddress const& _aNewAddress, GroupNodeImpl& rOriginal);

            ~DeferredGroupNodeImpl();

        public:
        // commit protocol
            std::auto_ptr<SubtreeChange> preCommitValueChanges(data::Accessor const& _aAccessor);
            void finishCommit(data::Accessor const& _aAccessor, SubtreeChange& rChange);
            void revertCommit(data::Accessor const& _aAccessor, SubtreeChange& rChange);
            void failedCommit(data::Accessor const& _aAccessor, SubtreeChange& rChange);

            void collectValueChanges(data::Accessor const& _aAccessor, NodeChanges& rChanges, TreeImpl* pParent, NodeOffset nNode) const;

        public:
        // data access
            bool hasChanges() const;
            void markChanged();

            typedef ValueMemberNode::DeferredImplRef MemberChange;
            MemberChange findValueChange(Name const& aName);

            ValueMemberNode makeValueMember(data::Accessor const& _aAccessor, Name const& _aName, bool _bForUpdate);

        private:
            typedef std::map< Name, MemberChange > MemberChanges;

            MemberChanges    m_aChanges;
        };
//-----------------------------------------------------------------------------

// Set nodes
//-----------------------------------------------------------------------------

        class DeferredSetNodeImpl : public SetNodeImpl
        {
        public:
            explicit
            DeferredSetNodeImpl(data::SetNodeAddress const& _aNodeRef, Template* pTemplate);

        public:
            bool hasChanges() const;
            void markChanged();
            void collectElementChanges(data::Accessor const& _aAccessor, NodeChanges& rChanges) const;

        public:
            std::auto_ptr<SubtreeChange> preCommitChanges(data::Accessor const& _aAccessor, ElementList& _rRemovedElements);
            void failedCommit(data::Accessor const& _aAccessor, SubtreeChange& rChanges);
            void finishCommit(data::Accessor const& _aAccessor, SubtreeChange& rChanges);
            void revertCommit(data::Accessor const& _aAccessor, SubtreeChange& rChanges);

            void insertNewElement(Name const& aName, Element const& aNewElement);
            void removeOldElement(Name const& aName);
        // Base Overrideables
        private:
        // NodeImpl implementation
            virtual bool                   doIsEmpty() const;
            virtual ElementTreeImpl*       doFindElement(Name const& aName) ;
            virtual SetNodeVisitor::Result doDispatchToElements(data::Accessor const& _aAccessor, SetNodeVisitor& aVisitor);

            virtual void doDifferenceToDefaultState(data::Accessor const& _aAccessor, SubtreeChange& _rChangeToDefault, ISubtree& _rDefaultTree);

            virtual SetElementChangeImpl* doAdjustToAddedElement(data::Accessor const& _aAccessor, Name const& aName, AddNode const& aAddNodeChange, Element const & aNewElement);
            virtual SetElementChangeImpl* doAdjustToRemovedElement(data::Accessor const& _aAccessor, Name const& aName, RemoveNode const& aRemoveNodeChange);

            virtual SetElementChangeImpl* doAdjustChangedElement(data::Accessor const& _aAccessor, NodeChangesInformation& rLocalChanges, Name const& aName, Change const& aChange);

            virtual void doTransferElements(ElementSet& rReplacement);

        // Implementation
        private:
            void rebuildElement(data::Accessor const& _aAccessor, Name const& aName, Element const& _aElement);

        private:
            ElementSet m_aChangedData;
            bool    m_bChanged;
            bool    m_bDefault;
        };
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
    }
}

#endif // CONFIGMGR_NODEIMPLOBJECTS_HXX_
