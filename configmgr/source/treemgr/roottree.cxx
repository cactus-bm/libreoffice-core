/*************************************************************************
 *
 *  $RCSfile: roottree.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: jl $ $Date: 2001-03-21 12:29:50 $
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
#include "roottree.hxx"

#include "roottreeimpl.hxx"
#include "nodefactory.hxx"
#include "noderef.hxx"
#include "nodechangeinfo.hxx"
#include "cmtreemodel.hxx"

namespace configmgr
{
    namespace configuration
    {
//-----------------------------------------------------------------------------
// factory methods
//-----------------------------------------------------------------------------

RootTree createReadOnlyTree(    AbsolutePath const& aContextPath,
                                ISubtree& rCacheNode, TreeDepth nDepth,
                                TemplateProvider const& aTemplateProvider)
{
    return RootTree( new RootTreeImpl(  NodeType::getReadAccessFactory(),
                                        aContextPath, rCacheNode, nDepth,
                                        aTemplateProvider
                                    ));
}
//-----------------------------------------------------------------------------

RootTree createUpdatableTree(   AbsolutePath const& aContextPath,
                                ISubtree& rCacheNode, TreeDepth nDepth,
                                TemplateProvider const& aTemplateProvider)
{
    return RootTree( new RootTreeImpl(  NodeType::getDeferredChangeFactory(),
                                        aContextPath, rCacheNode, nDepth,
                                        aTemplateProvider
                                    ));
}

//-----------------------------------------------------------------------------
// update on notify method
//-----------------------------------------------------------------------------
bool adjustToChanges(   NodeChangesInformation& rLocalChanges,
                        Tree const& aBaseTree, NodeRef const& aBaseNode,
                        Change const& aExternalChange,
                        TemplateProvider const& aTemplateProvider)
{
    OSL_PRECOND( !aBaseTree.isEmpty(), "ERROR: Configuration: Tree operation requires a valid Tree");
    OSL_PRECOND(  aBaseTree.isValidNode(aBaseNode), "ERROR: Configuration: NodeRef does not match Tree");

    if (!aBaseTree.isEmpty())
    {
        OSL_ENSURE(rLocalChanges.empty(), "Should pass empty container to adjustToChanges(...)");

        TreeImplHelper::impl(aBaseTree)->adjustToChanges(rLocalChanges, TreeImplHelper::offset(aBaseNode), aExternalChange,aTemplateProvider);

        return !rLocalChanges.empty();
    }
    else
        return false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class CommitHelper
//-----------------------------------------------------------------------------

CommitHelper::CommitHelper(Tree const& aTree)
{
    m_pTree = TreeImplHelper::impl(aTree);
    OSL_ENSURE(m_pTree, "INTERNAL ERROR: Unexpected NULL tree in commit helper");
}
//-----------------------------------------------------------------------------

bool CommitHelper::prepareCommit(TreeChangeList& rChangeList)
{
    OSL_ENSURE(m_pTree,"Cannot commit without a tree");
    if (m_pTree == NULL)
        return false;

    // get and check the changes
    std::auto_ptr<Change> pBaseChange(m_pTree->legacyCommitChanges());
    if (pBaseChange.get() == NULL)
        return false;

    OSL_ENSURE(pBaseChange->ISA(SubtreeChange),"Cannot commit non-tree nodes");
    if (!pBaseChange->ISA(SubtreeChange))
    {
        m_pTree->legacyRevertCommit(*pBaseChange);
        return false;
    }

    std::auto_ptr<SubtreeChange> pTreeChange( static_cast<SubtreeChange*>(pBaseChange.release()) );

    // find the name and path of the change
    Name aRootName(m_pTree->name(m_pTree->root()));
    AbsolutePath aPath = m_pTree->getContextPath();

    OSL_ENSURE(aRootName.toString() == pTreeChange->getNodeName(), "ERROR in Commit: Change Name Mismatch");

    // now fill the TreeChangeList
    rChangeList.pathToRoot = ConfigurationName(aPath.toString());
    rChangeList.root.swap( *pTreeChange );

    return true;
}
//-----------------------------------------------------------------------------

void CommitHelper::finishCommit(TreeChangeList& rChangeList)
{
    OSL_ENSURE(m_pTree,"INTERNAL ERROR: Nothing to finish without a tree");

    // find the name and path of the change
    Name aRootName(m_pTree->name(m_pTree->root()));
    AbsolutePath aPath = m_pTree->getContextPath();

    OSL_ENSURE(rChangeList.pathToRoot.fullName() == aPath.toString(), "ERROR: FinishCommit cannot handle rebased changes trees");
    if (rChangeList.pathToRoot.fullName() != aPath.toString())
        throw configuration::Exception("INTERNAL ERROR: FinishCommit cannot handle rebased changes trees");

    m_pTree->legacyFinishCommit(rChangeList.root);
}
//-----------------------------------------------------------------------------

void CommitHelper::revertCommit(TreeChangeList& rChangeList)
{
    OSL_ENSURE(m_pTree,"INTERNAL ERROR: Nothing to finish without a tree");

    Name aRootName(m_pTree->name(m_pTree->root()));
    AbsolutePath aPath = m_pTree->getContextPath();

    OSL_ENSURE(rChangeList.pathToRoot.fullName() == aPath.toString(), "ERROR: cannot handle rebased changes trees");
    if (rChangeList.pathToRoot.fullName() != aPath.toString())
        throw configuration::Exception("INTERNAL ERROR: RevertCommit cannot handle rebased changes trees");

    m_pTree->legacyRevertCommit(rChangeList.root);
}
//-----------------------------------------------------------------------------

void CommitHelper::failedCommit(TreeChangeList& rChangeList)
{
    OSL_ENSURE(m_pTree,"INTERNAL ERROR: Nothing to finish without a tree");

    Name aRootName(m_pTree->name(m_pTree->root()));
    AbsolutePath aPath = m_pTree->getContextPath();

    OSL_ENSURE(rChangeList.pathToRoot.fullName() == aPath.toString(), "ERROR: cannot handle rebased changes trees");
    if (rChangeList.pathToRoot.fullName() != aPath.toString())
        throw configuration::Exception("INTERNAL ERROR: FailedCommit cannot handle rebased changes trees");

    m_pTree->legacyFailedCommit(rChangeList.root);
}
//-----------------------------------------------------------------------------

    }
}

