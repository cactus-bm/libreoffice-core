/*************************************************************************
 *
 *  $RCSfile: layerparser.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2002-05-16 11:00:29 $
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
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "layerparser.hxx"

// -----------------------------------------------------------------------------

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace xml
    {
// -----------------------------------------------------------------------------
        namespace uno       = ::com::sun::star::uno;
        namespace sax       = ::com::sun::star::xml::sax;
// -----------------------------------------------------------------------------

LayerParser::LayerParser(ServiceFactory const & _xSvcFactory, uno::Reference< backenduno::XLayerHandler > const & _xHandler)
: BasicParser(_xSvcFactory)
, m_xHandler(_xHandler)
, m_bRemoved(false)
, m_bNewProp(false)
{
    if (!m_xHandler.is())
    {
        OUString sMessage(RTL_CONSTASCII_USTRINGPARAM("Cannot create LayerParser: Unexpected NULL Handler"));
        throw uno::RuntimeException(sMessage, NULL);
    }
}
// -----------------------------------------------------------------------------

LayerParser::~LayerParser()
{
}
// -----------------------------------------------------------------------------

void SAL_CALL LayerParser::startDocument(  )
        throw (sax::SAXException, uno::RuntimeException)
{
    BasicParser::startDocument();

    OSL_ENSURE(isEmptyNode(), "BasicParser does not mark new document as empty");

    m_xHandler->startLayer();
    m_bRemoved = false;
    m_bNewProp = false;
}
// -----------------------------------------------------------------------------

void SAL_CALL LayerParser::endDocument(  ) throw (sax::SAXException, uno::RuntimeException)
{
    if (isEmptyNode()) OSL_TRACE("Configuration Parser: XML layer document ended without any data");

    BasicParser::endDocument();

    m_xHandler->endLayer();
}
// -----------------------------------------------------------------------------

void SAL_CALL LayerParser::startElement( const OUString& aName, const uno::Reference< sax::XAttributeList >& xAttribs )
        throw (sax::SAXException, uno::RuntimeException)
{
    if ( this->isSkipping() )
    {
        this->startSkipping( aName, xAttribs );
        return;
    }

    ElementInfo aInfo = getDataParser().parseElementInfo(aName,xAttribs);

    switch (aInfo.type)
    {
    case ElementType::group: case ElementType::set:
        OSL_ENSURE( false, "Layer XML parser - Unexpected: found group/set element (should be 'node')\n");
        // fall thru
    case ElementType::node:
        this->startNode(aInfo,xAttribs);
        OSL_ASSERT( this->isInNode() && !this->isInProperty() );
        break;

    case ElementType::property:
        this->startProperty(aInfo,xAttribs);
        OSL_ASSERT( this->isInUnhandledProperty() );
        break;

    case ElementType::value:
        this->startValueData(xAttribs);
        OSL_ASSERT( this->isInValueData() );
        break;

    default: // skip unknown elements
        OSL_ENSURE( aInfo.type >= ElementType::other, "Layer XML parser - Unexpected: found schema element in layer data\n");
        OSL_ENSURE( aInfo.type <  ElementType::other, "Layer XML parser - Unexpected: found unknown element tag\n");
        this->startSkipping( aName, xAttribs );
        return;
    }
}
// -----------------------------------------------------------------------------

void SAL_CALL LayerParser::endElement( const OUString& aName )
        throw (sax::SAXException, uno::RuntimeException)
{
    if ( this->wasSkipping(aName) )
        return;

    if ( this->isInValueData())
        this->endValueData();

    else if (this->isInProperty())
        this->endProperty();

    else if (this->isInNode())
        this->endNode();

    else
        this->raiseParseException("Layer parser: Invalid XML: endElement without matching startElement");

}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void LayerParser::startNode( ElementInfo const & aInfo, const uno::Reference< sax::XAttributeList >& xAttribs )
{
    this->checkNotRemoved();

    BasicParser::startNode(aInfo,xAttribs);

    switch (aInfo.op)
    {
    case Operation::none:
    case Operation::modify:
        m_xHandler->overrideNode(aInfo.name,aInfo.flags);
        break;

    case Operation::replace:
        {
            backenduno::TemplateIdentifier aTemplate;
            if (getDataParser().getInstanceType(xAttribs,aTemplate.Name,aTemplate.Component))
                m_xHandler->addOrReplaceNodeFromTemplate(aInfo.name,aTemplate,aInfo.flags);
            else
                m_xHandler->addOrReplaceNode(aInfo.name,aInfo.flags);
        }
        break;

    case Operation::remove:
        m_xHandler->dropNode(aInfo.name);
        m_bRemoved = true;
        break;

    default: OSL_ASSERT(false);
    case Operation::unknown:
        this->raiseParseException("Layer parser: Invalid Data: unknown operation");
    }
}
// -----------------------------------------------------------------------------

void LayerParser::endNode()
{
    if (!this->isInRemoved())
        m_xHandler->endNode();
    else
        m_bRemoved = false;

    BasicParser::endNode();
}
// -----------------------------------------------------------------------------

void LayerParser::startProperty( ElementInfo const & aInfo, const uno::Reference< sax::XAttributeList >& xAttribs )
{
    this->checkNotRemoved();

    BasicParser::startProperty(aInfo,xAttribs);

    switch (aInfo.op)
    {
    case Operation::none:
    case Operation::modify:
    case Operation::replace:
        // defer to later
        m_bNewProp = true;
        break;

    case Operation::remove:
        this->raiseParseException("Layer parser: Invalid Data: operation 'remove' is not permitted for properties");

    default: OSL_ASSERT(false);
    case Operation::unknown:
        this->raiseParseException("Layer parser: Invalid Data: unknown operation");
    }
}
// -----------------------------------------------------------------------------

void LayerParser::endProperty()
{
    OSL_ASSERT(!this->isInRemoved());

    if (m_bNewProp)
    {
        OSL_ASSERT(this->isEmptyNode());

        ElementInfo const & aInfo = getActiveNodeInfo();
        switch (aInfo.op)
        {
        case Operation::none:
        case Operation::modify:
            OSL_ENSURE(aInfo.flags,"Warning: Empty property modification (and without attributes) in Layer");
            m_xHandler->overridePropertyAttributes(aInfo.name,aInfo.flags);
            break;

        case Operation::replace:
            m_xHandler->addProperty(aInfo.name,aInfo.flags,getActivePropertyType());
            break;

        default:
            OSL_ASSERT(false);
        }
        m_bNewProp = false;
    }
    else
        OSL_ASSERT(!this->isEmptyNode());

    BasicParser::endProperty();
}
// -----------------------------------------------------------------------------

void LayerParser::startValueData(const uno::Reference< sax::XAttributeList >& xAttribs)
{
    this->checkNotRemoved();

    BasicParser::startValueData(xAttribs);
}
// -----------------------------------------------------------------------------

void LayerParser::endValueData()
{
    uno::Any aValue = this->getCurrentValue();

    ElementInfo const & aInfo = this->getActiveNodeInfo();

    if (this->isValueDataLocalized())
    {
        OUString aLocale = this->getValueDataLocale();

        if (m_bNewProp && aInfo.op == Operation::replace)
            m_xHandler->addProperty(aInfo.name,aInfo.flags,getActivePropertyType());

        m_xHandler->overridePropertyValueForLocale(aInfo.name,aLocale,aValue);
    }
    else
    {
        if (aInfo.op != Operation::replace)
            m_xHandler->overridePropertyValue(aInfo.name,aInfo.flags,aValue);

        else if (aValue.hasValue())
            m_xHandler->addPropertyWithValue(aInfo.name,aInfo.flags,aValue);

        else
            m_xHandler->addProperty(aInfo.name,aInfo.flags,getActivePropertyType());
    }

    m_bNewProp = false;

    BasicParser::endValueData();
}
// -----------------------------------------------------------------------------

void LayerParser::checkNotRemoved()
{
    if (m_bRemoved)
        raiseParseException("Layer parser: Invalid Data: Data inside removed node.");
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
    } // namespace

// -----------------------------------------------------------------------------
} // namespace

