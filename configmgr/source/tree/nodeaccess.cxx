/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: nodeaccess.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 15:21:45 $
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

#include "nodeaccess.hxx"

#ifndef CONFIGMGR_UPDATEACCESSOR_HXX
#include "updateaccessor.hxx"
#endif

#ifndef CONFIGMGR_TREEACCESSOR_HXX
#include "treeaccessor.hxx"
#endif
#ifndef CONFIGMGR_SETNODEACCESS_HXX
#include "setnodeaccess.hxx"
#endif
#ifndef CONFIGMGR_GROUPNODEACCESS_HXX
#include "groupnodeaccess.hxx"
#endif
#ifndef CONFIGMGR_VALUENODEACCESS_HXX
#include "valuenodeaccess.hxx"
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

// -----------------------------------------------------------------------------

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace data
    {
    // -------------------------------------------------------------------------

        NodeAddress::DataType* NodeAccess::access(NodeAddressType const& _aNodeRef, memory::UpdateAccessor& _rUpdateAccess)
        {
            return static_cast<NodeAddress::DataType*>(_rUpdateAccess.access(_aNodeRef.m_pData));
        }
    // -------------------------------------------------------------------------

        NodeAccess getSubnode(NodeAccessRef const & _aParent, NodeAccess::Name const & _aName)
        {
            if (GroupNodeAccess::isInstance(_aParent))
            {
                return GroupNodeAccess(_aParent).getChildNode(_aName).toNodeAccess();
            }
            else if (SetNodeAccess::isInstance(_aParent))
            {
                TreeAccessor aElement = SetNodeAccess(_aParent).getElementTree(_aName);
                return aElement.isValid() ? aElement.getRootNode().toNodeAccess() : NodeAccess::emptyNode();
            }
            else
            {
                OSL_ENSURE( ValueNodeAccess::isInstance(_aParent),"ERROR: Unknown node type");
                OSL_ENSURE(!ValueNodeAccess::isInstance(_aParent),"ERROR: Trying to access child of value node");
                return NodeAccess::emptyNode();
            }
        }
    // -------------------------------------------------------------------------

        NodeAddress getSubnodeAddress(memory::Accessor const& _aAccess, NodeAddress const & _aNodeAddress, NodeAccess::Name const & _aName)
        {
            return getSubnode( NodeAccessRef(&_aAccess,_aNodeAddress), _aName ).address();
        }
    // -------------------------------------------------------------------------

        NodeAddress getSubnodeAddress(memory::UpdateAccessor& _aAccess, NodeAddress const & _aNodeAddress, NodeAccess::Name const & _aName)
        {
            memory::Accessor aAccess = _aAccess.accessor();
            return getSubnode( NodeAccessRef(&aAccess,_aNodeAddress), _aName ).address();
        }
    // -------------------------------------------------------------------------

        SetNodeAddress toSetNodeAddress(memory::Accessor const & _aAccess, NodeAddress const & _aNodeAddr)
        {
            SetNodeAccess aNodeAccess( NodeAccessRef(&_aAccess,_aNodeAddr) );
            return aNodeAccess.address();
        }
    // -------------------------------------------------------------------------

        SetNodeAddress toSetNodeAddress(memory::UpdateAccessor & _aAccess, NodeAddress const & _aNodeAddr)
        {
            SetNodeAccess aNodeAccess( NodeAccess(_aAccess.accessor(),_aNodeAddr) );
            return aNodeAccess.address();
        }
    // -------------------------------------------------------------------------

        GroupNodeAddress toGroupNodeAddress(memory::Accessor const & _aAccess, NodeAddress const & _aNodeAddr)
        {
            GroupNodeAccess aNodeAccess( NodeAccess(_aAccess,_aNodeAddr) );
            return aNodeAccess.address();
        }
    // -------------------------------------------------------------------------

        GroupNodeAddress toGroupNodeAddress(memory::UpdateAccessor & _aAccess, NodeAddress const & _aNodeAddr)
        {
            GroupNodeAccess aNodeAccess( NodeAccess(_aAccess.accessor(),_aNodeAddr) );
            return aNodeAccess.address();
        }
    // -------------------------------------------------------------------------

        ValueNodeAddress toValueNodeAddress(memory::Accessor const & _aAccess, NodeAddress const & _aNodeAddr)
        {
            ValueNodeAccess aNodeAccess( NodeAccess(_aAccess,_aNodeAddr) );
            return aNodeAccess.address();
        }
    // -------------------------------------------------------------------------

        ValueNodeAddress toValueNodeAddress(memory::UpdateAccessor & _aAccess, NodeAddress const & _aNodeAddr)
        {
            ValueNodeAccess aNodeAccess( NodeAccess(_aAccess.accessor(),_aNodeAddr) );
            return aNodeAccess.address();
        }
    // -------------------------------------------------------------------------
    }
// -----------------------------------------------------------------------------
} // namespace configmgr


