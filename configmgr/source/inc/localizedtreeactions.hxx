/*************************************************************************
 *
 *  $RCSfile: localizedtreeactions.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2001-11-14 16:35:13 $
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

#ifndef CONFIGMGR_LOCALIZEDTREEACTIONS_HXX
#define CONFIGMGR_LOCALIZEDTREEACTIONS_HXX

#include "valuenode.hxx"

//..........................................................................
namespace configmgr
{
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

//..........................................................................
}   // namespace configmgr
//..........................................................................

#endif // CONFIGMGR_LOCALIZEDTREEACTIONS_HXX


