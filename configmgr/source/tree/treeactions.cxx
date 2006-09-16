/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: treeactions.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 15:22:54 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_configmgr.hxx"

#include <stdio.h>

#include "treeactions.hxx"

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif


//..........................................................................
namespace configmgr
{
// -------------------------------------------------------------------------
namespace
{
    struct ForceWritable : NodeModification
    {
        void handle(ValueNode& _rValue)     { implForceWritable(_rValue); }
        void handle(ISubtree& _rSubtree)    { implForceWritable(_rSubtree); applyToChildren(_rSubtree); }

        void implForceWritable(INode& _rNode) { _rNode.forceReadonlyToFinalized(); }
    };
}

// -------------------------------------------------------------------------
void forceWritable(INode& _rNode)
{
    ForceWritable aVisitor;

    aVisitor.applyToNode(_rNode);
}

//==========================================================================
//= OChangeActionCounter
//==========================================================================

//--------------------------------------------------------------------------
void OChangeActionCounter::handle(ValueChange const& /*aValueNode*/){ ++nValues; }

//--------------------------------------------------------------------------
void OChangeActionCounter::handle(AddNode const& /*aAddNode*/){ ++nAdds; }

//--------------------------------------------------------------------------
void OChangeActionCounter::handle(RemoveNode const& /*aRemoveNode*/){ ++nRemoves; }

//--------------------------------------------------------------------------
void OChangeActionCounter::handle(SubtreeChange const& aSubtree)
{
    applyToChildren(aSubtree);
}

//..........................................................................
}   // namespace configmgr
//..........................................................................


