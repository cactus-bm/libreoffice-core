/*************************************************************************
 *
 *  $RCSfile: pathexpression.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 10:55:06 $
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

#include "pathexpression.hxx"
#include "unohelper.hxx"
#include "evaluationcontext.hxx"
#include "NameContainer.hxx"
#include "xmlhelper.hxx"

#include <com/sun/star/xml/dom/XNode.hpp>
#include <com/sun/star/xml/dom/XNodeList.hpp>
#include <com/sun/star/xml/dom/NodeType.hpp>
#include <com/sun/star/xml/dom/events/XEventListener.hpp>
#include <com/sun/star/xml/dom/events/XEventTarget.hpp>
#include <com/sun/star/xml/xpath/XXPathObject.hpp>
#include <com/sun/star/xml/xpath/XXPathAPI.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <rtl/ustrbuf.hxx>

#include <unotools/textsearch.hxx>

#include <algorithm>
#include <functional>


using rtl::OUString;
using rtl::OUStringBuffer;
using com::sun::star::uno::Reference;
using com::sun::star::uno::Sequence;
using com::sun::star::xml::dom::XNode;
using com::sun::star::xml::dom::XNodeList;
using com::sun::star::xml::dom::events::XEventListener;
using com::sun::star::xml::dom::events::XEventTarget;
using com::sun::star::container::XNameContainer;
using com::sun::star::xml::xpath::XXPathAPI;
using com::sun::star::xml::xpath::XXPathObject;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::uno::UNO_QUERY_THROW;
using com::sun::star::xml::dom::NodeType_TEXT_NODE;
using com::sun::star::xml::xpath::XPathObjectType_XPATH_UNDEFINED;
using namespace std;




namespace xforms
{

PathExpression::PathExpression()
    : ComputedExpression(),
      maNodes()
{
}

PathExpression::~PathExpression()
{
}



void PathExpression::setExpression( const OUString& rExpression )
{
    // set new expression, and clear pre-computed results
    ComputedExpression::setExpression( rExpression );

    // check expression against regular expression to determine
    // whether it contains only 'simple' (i.e. static) conditions. For
    // now, we check whether it only contains number positions.
    // (TODO: Only works for names containing only ASCII letters+digits.)
    mbIsSimple =
        _checkExpression( "( */@?[a-zA-Z0-9:]+( *\\[ *[0-9 ]+ *\\] *)?)+" );

    maNodes.clear();
}

const rtl::OUString PathExpression::_getExpressionForEvaluation() const
{
    OUString sExpr = ComputedExpression::_getExpressionForEvaluation();
    if( sExpr.getLength() == 0 )
        sExpr = OUSTRING(".");
    return sExpr;
}

bool PathExpression::evaluate( const EvaluationContext& rContext )
{
    // for simple expression we don't need to re-bind (if we were bound before)
    // (we will evaluate empty expressions, since they are interpreted as ".")
    if( mxResult.is() && isSimpleExpression() )
        return true;

    bool bResult = _evaluate( rContext, _getExpressionForEvaluation() );

    // clear old result, and copy new
    maNodes.clear();
    if( mxResult.is() )
    {
        // copy node list
        Reference<XNodeList> xNodeList = mxResult->getNodeList();
        OSL_ENSURE( xNodeList.is(), "empty object (instead of empty list)" );
        sal_Int32 nLength = xNodeList.is() ? xNodeList->getLength() : 0;
        for( sal_Int32 n = 0; n < nLength; n++ )
            maNodes.push_back( xNodeList->item( n ) );
    }

    return bResult;
}


bool PathExpression::isElementName() const
{
    return isValidQName( getExpression(), getNamespaces() );
}


Reference<XNode> PathExpression::getNode() const
{
    Reference<XNode> xResult;
    if( ! maNodes.empty() )
        xResult = *maNodes.begin();
    return xResult;
}

const PathExpression::NodeVector_t PathExpression::getNodeList() const
{
    return maNodes;
}

Reference<XNodeList> PathExpression::getXNodeList() const
{
    return mxResult.is() ? mxResult->getNodeList() : Reference<XNodeList>();
}


} // namespace xforms
