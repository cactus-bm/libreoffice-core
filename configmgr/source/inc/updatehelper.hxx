/*************************************************************************
 *
 *  $RCSfile: updatehelper.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: jb $ $Date: 2002-03-28 08:50:15 $
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

#ifndef CONFIGMGR_UPDATEHELPER_HXX
#define CONFIGMGR_UPDATEHELPER_HXX

#ifndef CONFIGMGR_CHANGE_HXX
#include "change.hxx"
#endif

//..........................................................................
namespace configmgr
{
//..........................................................................
    namespace memory    { class UpdateAccessor; }
    namespace data      { class NodeAddress; class NodeAccess; }

//..........................................................................

// adjust a set of changes to the target tree, return true, if there are changes left
    bool adjustUpdateToTree(SubtreeChange & _rUpdateTree, data::NodeAccess const & _aRootNode);

// adjust a set of changes to the target tree, return true, if there are changes left
    bool adjustUpdateToTree(SubtreeChange & _rUpdateTree, memory::UpdateAccessor& _anUpdateAccess, data::NodeAddress _aRootNode);

// apply a already matching set of changes to the target tree
    void applyUpdateToTree(SubtreeChange& _anUpdateTree, memory::UpdateAccessor& _anUpdateAccess, data::NodeAddress _aRootNode);

// apply a set of changes to the target tree
    void applyUpdateWithAdjustmentToTree(SubtreeChange& _anUpdateTree, memory::UpdateAccessor& _anUpdateAccess, data::NodeAddress _aRootNode);

// apply a set of changes to the target tree, return true, if there are changes found
    bool createUpdateFromDifference(SubtreeChange& _rResultingUpdateTree, data::NodeAccess const & _aExistingData, ISubtree const & _aNewData);

//..........................................................................
}   // namespace configmgr
//..........................................................................

#endif // CONFIGMGR_MERGEHELPER_HXX


