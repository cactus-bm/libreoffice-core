/*************************************************************************
 *
 *  $RCSfile: pathexpression.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-23 11:37:55 $
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

#ifndef _PATHEXPRESSION_HXX
#define _PATHEXPRESSION_HXX


// include for parent class
#include "computedexpression.hxx"

// includes for member variables
#include <vector>

// forward declaractions
namespace com { namespace sun { namespace star
{
    namespace xml { namespace dom
    {
        class XNodeList;
        namespace events { class XEventListener; }
    } }
} } }



namespace xforms
{

/** PathExpression represents an XPath Expression and caches results */
class PathExpression : public ComputedExpression
{
public:
    typedef std::vector<com::sun::star::uno::Reference<com::sun::star::xml::dom::XNode> > NodeVector_t;

private:
    /// the node-list result from the last bind (cached from mxResult)
    NodeVector_t maNodes;

protected:
    /// get expression for evaluation
    const rtl::OUString _getExpressionForEvaluation() const;


public:
    PathExpression();
    ~PathExpression();

    /// set the expression string
    /// (overridden to do remove old listeners)
    /// (also defines simple expressions)
    void setExpression( const rtl::OUString& rExpression );


    /// evaluate the expression relative to the content node.
    bool evaluate( const xforms::EvaluationContext& rContext );


    // get the result of this expression as node/node list/...
    com::sun::star::uno::Reference<com::sun::star::xml::dom::XNode> getNode() const;
    const NodeVector_t getNodeList() const;
    com::sun::star::uno::Reference<com::sun::star::xml::dom::XNodeList> getXNodeList() const;

};

} // namespace xforms

#endif
