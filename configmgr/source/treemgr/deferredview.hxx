/*************************************************************************
 *
 *  $RCSfile: deferredview.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2002-02-11 14:55:53 $
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

#ifndef CONFIGMGR_DEFERREDVIEW_HXX_
#define CONFIGMGR_DEFERREDVIEW_HXX_

#ifndef CONFIGMGR_VIEWBEHAVIOR_HXX_
#include "viewstrategy.hxx"
#endif


namespace configmgr
{
    namespace view
    {
//-----------------------------------------------------------------------------
// View behavior for direct data access
//-----------------------------------------------------------------------------

        class DeferredViewStrategy : public ViewStrategy
        {
            memory::Segment const * m_pSegment;
        public:
            explicit
            DeferredViewStrategy(memory::Segment const * _pSegment)
            : m_pSegment(_pSegment)
            {}

        // ViewStrategy implementation
        private:
            // change handling -required
            virtual bool doHasChanges(Node const& _aNode) const;
            virtual void doMarkChanged(Node const& _aNode);


            // change handling
            virtual void doCollectChanges(Node const& _aNode, configuration::NodeChanges& rChanges) const;

            // commit protocol
            virtual std::auto_ptr<SubtreeChange> doPreCommitChanges(Tree const& _aTree, configuration::ElementList& _rRemovedElements);
            virtual void doFailedCommit(Tree const& _aTree, SubtreeChange& rChanges);
            virtual void doFinishCommit(Tree const& _aTree, SubtreeChange& rChanges);
            virtual void doRevertCommit(Tree const& _aTree, SubtreeChange& rChanges);

            // notification protocol
            virtual configuration::ValueChangeImpl* doAdjustToValueChange(GroupNode const& _aGroupNode, Name const& aName, ValueChange const& rExternalChange);
        //  virtual void doAdjustToElementChanges(configuration::NodeChangesInformation& rLocalChanges, SetNode const& _aNode, SubtreeChange const& rExternalChanges, TreeDepth nDepth);

            // common attributes
            virtual node::Attributes doAdjustAttributes(node::Attributes const& _aAttributes) const;

            // group member access
            virtual configuration::ValueMemberNode doGetValueMember(GroupNode const& _aNode, Name const& _aName, bool _bForUpdate) const;

            // set element access
            virtual void doInsertElement(SetNode const& _aNode, Name const& aName, configuration::SetEntry const& aNewEntry);
            virtual void doRemoveElement(SetNode const& _aNode, Name const& aName);

            virtual memory::Segment const * doGetDataSegment() const { return m_pSegment; }

            virtual NodeFactory& doGetNodeFactory();
        private:
            void implCollectChangesIn(Node const& _aNode, NodeChanges& rChanges) const;
            // commit protocol
            std::auto_ptr<SubtreeChange> implPreCommitChanges(Node const& _aNode, configuration::ElementList& _rRemovedElements);
            void implFailedCommit(Node const& _aNode, SubtreeChange& rChanges);
            void implFinishCommit(Node const& _aNode, SubtreeChange& rChanges);
            void implRevertCommit(Node const& _aNode, SubtreeChange& rChanges);

            void implPreCommitSubChanges(GroupNode const & _aGroup, configuration::ElementList& _rRemovedElements, SubtreeChange& _rParentChange);
            void implFailedSubCommitted(GroupNode const & _aGroup, SubtreeChange& rChanges);
            void implFinishSubCommitted(GroupNode const & _aGroup, SubtreeChange& rChanges);
            void implRevertSubCommitted(GroupNode const & _aGroup, SubtreeChange& rChanges);

        };
//-----------------------------------------------------------------------------
    }
}

#endif // CONFIGMGR_DEFERREDVIEW_HXX_
