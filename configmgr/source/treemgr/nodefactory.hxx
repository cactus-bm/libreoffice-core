/*************************************************************************
 *
 *  $RCSfile: nodefactory.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2000-11-07 14:35:59 $
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

#ifndef CONFIGMGR_CONFIGNODEFACTORY_HXX_
#define CONFIGMGR_CONFIGNODEFACTORY_HXX_

#include "apitypes.hxx"

#include <vos/ref.hxx>

#include <osl/diagnose.h>

namespace configmgr
{
    class INode;
    class ISubtree;
    class ValueNode;

    namespace configuration
    {
//-----------------------------------------------------------------------------
        typedef com::sun::star::uno::Any UnoAny;
        typedef com::sun::star::uno::Type UnoType;

        class NodeImpl;
        typedef vos::ORef<NodeImpl> NodeImplHolder;

        class Name;
        class Template;

//-----------------------------------------------------------------------------

// Creating Specific types of nodes
//-----------------------------------------------------------------------------

        struct NodeFactory
        {
            virtual NodeImplHolder makeValueNode(ValueNode& rOriginal) = 0;
            virtual NodeImplHolder makeGroupNode(ISubtree& rOriginal) = 0;
            virtual NodeImplHolder makeSetNode  (ISubtree& rOriginal, Template* pTemplate) = 0;
        };
//-----------------------------------------------------------------------------

        namespace NodeType
        {
        // Different standard (static) factories
        //---------------------------------------------------------------------

            /// provides a factory for read-only node implementations
            NodeFactory& getReadAccessFactory();
            /// provides a factory for immediately commiting node implementations
            NodeFactory& getDirectAccessFactory();
            /// provides a factory for nodes that cache changes temporarily
            NodeFactory& getDeferredChangeFactory();
        //---------------------------------------------------------------------
        }

//-----------------------------------------------------------------------------

    }
}

#endif // CONFIGMGR_CONFIGNODEFACTORY_HXX_
