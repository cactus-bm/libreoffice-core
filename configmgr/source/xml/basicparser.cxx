/*************************************************************************
 *
 *  $RCSfile: basicparser.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2002-05-16 11:00:28 $
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

#include "basicparser.hxx"

#ifndef _COM_SUN_STAR_XML_SAX_SAXEXCEPTION_HPP_
#include <com/sun/star/xml/sax/SAXException.hpp>
#endif

#ifndef CONFIGMGR_XML_VALUECONVERTER_HXX
#include "valueconverter.hxx"
#endif
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

namespace
{
    typedef uno::Reference< script::XTypeConverter > TypeConverter;

    static inline
    uno::Reference< uno::XInterface > createTCV(BasicParser::ServiceFactory const & _xSvcFactory)
    {
        OSL_ENSURE(_xSvcFactory.is(),"Cannot create Parser without a ServiceManager");

        static const rtl::OUString k_sTCVService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.script.Converter"));

        return TypeConverter::query(_xSvcFactory->createInstance(k_sTCVService));
    }

    static inline
    TypeConverter asTCV(uno::Reference< uno::XInterface > const & _xTCV)
    {
        OSL_ASSERT(TypeConverter::query(_xTCV).get() == _xTCV.get());
        return static_cast< script::XTypeConverter * >(_xTCV.get());
    }
}
// -----------------------------------------------------------------------------

struct BasicParser::ValueData : ValueConverter
{
    OUString    content;
    OUString    locale;
    bool    isLocalized;

    ValueData(uno::Type const& _aType, TypeConverter const & _xTCV)
    : ValueConverter(_aType, _xTCV)
    , content()
    , locale()
    , isLocalized(false)
    {
    }

    uno::Any convertToAny() const
    {
        return ValueConverter::convertToAny(this->content);
    }

    void setLocalized(OUString const & _aLocale)
    {
        isLocalized = true;
        locale = _aLocale;
    }
};
// -----------------------------------------------------------------------------

BasicParser::BasicParser(ServiceFactory const & _xSvcFactory)
: m_xTypeConverter( createTCV(_xSvcFactory) )
, m_xLocator(NULL)
, m_aDataParser()
, m_aNodes()
, m_aValueType()
, m_pValueData(NULL)
, m_nSkipLevels(0)
, m_bEmpty()
{
    if (!m_xTypeConverter.is())
        throw uno::RuntimeException();
}
// -----------------------------------------------------------------------------

BasicParser::~BasicParser()
{
    delete m_pValueData;
}
// -----------------------------------------------------------------------------

void SAL_CALL BasicParser::startDocument(  )
        throw (sax::SAXException, uno::RuntimeException)
{
    m_aDataParser.reset();
    m_aValueType    = uno::Type();
    m_nSkipLevels   = 0;

    delete m_pValueData, m_pValueData = NULL;

    while (!m_aNodes.empty()) m_aNodes.pop();

    m_bEmpty = true;
}
// -----------------------------------------------------------------------------

void SAL_CALL BasicParser::endDocument(  ) throw (sax::SAXException, uno::RuntimeException)
{
    if (!m_aNodes.empty() || isSkipping() || isInValueData())
        raiseParseException( "Configuration XML Parser - Invalid XML: Unexpected end of document" );

    m_xLocator.clear();
}
// -----------------------------------------------------------------------------

void SAL_CALL BasicParser::characters( const OUString& aChars )
        throw (sax::SAXException, uno::RuntimeException)
{
    if (isInValueData())
    {
        m_pValueData->content += aChars;
    }
#ifdef CONFIG_XMLPARSER_VALIDATE_WHITESPACE
    else
        OSL_ENSURE( isSkipping() || aChars.trim().getLength() == 0, "Unexpected text content in configuration XML");
#endif
}
// -----------------------------------------------------------------------------

void SAL_CALL BasicParser::ignorableWhitespace( const OUString& aWhitespaces )
        throw (sax::SAXException, uno::RuntimeException)
{
    if (isInValueData())
    {
        OSL_ENSURE(false, "Configuration XML: Unexpected ignorable (!) whitespace instruction in value data");
        if (!m_pValueData->isNull())
            m_pValueData->content += aWhitespaces;
    }
#ifdef CONFIG_XMLPARSER_VALIDATE_WHITESPACE
    else
        OSL_ENSURE( aChars.trim().getLength() == 0, "Unexpected non-space content in ignorable whitespace");
#endif
}
// -----------------------------------------------------------------------------

void SAL_CALL BasicParser::processingInstruction( const OUString& aTarget, const OUString& aData )
        throw (sax::SAXException, uno::RuntimeException)
{
    OSL_ENSURE(false, "Unexpected processing instruction in Configuration XML");
}
// -----------------------------------------------------------------------------

void SAL_CALL BasicParser::setDocumentLocator( const uno::Reference< sax::XLocator >& xLocator )
        throw (sax::SAXException, uno::RuntimeException)
{
    m_xLocator = xLocator;
}
// -----------------------------------------------------------------------------

void BasicParser::startNode( ElementInfo const & aInfo, const uno::Reference< sax::XAttributeList >& xAttribs )
{
    OSL_ENSURE( !isSkipping(), "While skipping, call startSkipping() instead of startNode()");
    OSL_ENSURE( aInfo.type != ElementType::property, "For properties, call startProperty() instead of startNode()");

    if (isInProperty())
        raiseParseException( "Configuration XML Parser - Invalid Data: Cannot have a node nested in a property" );

    m_aNodes.push(aInfo);
    m_bEmpty = (aInfo.flags != 0) || (aInfo.op > Operation::modify);

    OSL_POSTCOND( isInNode(), "Could not start a node ");
}
// -----------------------------------------------------------------------------

void BasicParser::endNode( )
{
    OSL_ENSURE( !isSkipping(), "While skipping, honor wasSkipping() instead of calling endNode()");
    OSL_ENSURE( !isInProperty(), "For properties, call endProperty() instead of endNode()" );

    ensureInNode();

    m_aNodes.pop();
    m_bEmpty = false;
}
// -----------------------------------------------------------------------------

void BasicParser::ensureInNode( )
{
    if (!isInNode())
        raiseParseException("Unexpected endElement without matching startElement");
}
// -----------------------------------------------------------------------------

bool BasicParser::isInNode( )
{
    return ! m_aNodes.empty();
}
// -----------------------------------------------------------------------------

bool BasicParser::isEmptyNode( )
{
    return m_bEmpty;
}
// -----------------------------------------------------------------------------

ElementInfo const & BasicParser::getActiveNodeInfo( )
{
    ensureInNode();

    return m_aNodes.top();
}
// -----------------------------------------------------------------------------

void BasicParser::startProperty( ElementInfo const & aInfo, const uno::Reference< sax::XAttributeList >& xAttribs )
{
    OSL_ENSURE( !isSkipping(), "While skipping, call startSkipping() instead of startProperty()");
    OSL_ENSURE( aInfo.type == ElementType::property, "For non-property nodes, call startNode() instead of startProperty()");

    if (isInProperty())
        raiseParseException( "Configuration XML Parser - Invalid Data: Properties may not nest" );

    m_aValueType = getDataParser().getPropertyValueType(xAttribs);

    if (m_aValueType == uno::Type())
        raiseParseException( "Configuration XML Parser - Invalid Data: Property without a type" );

    m_aNodes.push(aInfo);
    m_bEmpty = true;

    OSL_POSTCOND( isInProperty(), "Could not get data to start a property" );
    OSL_POSTCOND( isInUnhandledProperty(), "Could not mark property as unhandled");
}
// -----------------------------------------------------------------------------

void BasicParser::endProperty( )
{
    OSL_ENSURE( !isSkipping(), "While skipping, honor wasSkipping() instead of calling endProperty()");
    OSL_ENSURE( isInProperty(), "For non-property nodes, call endNode() instead of endProperty()" );

    ensureInNode();

    m_aNodes.pop();
    m_bEmpty = false;

    m_aValueType = uno::Type();

    OSL_POSTCOND( !isInProperty(), "Could not get mark end of property" );
}
// -----------------------------------------------------------------------------

uno::Type BasicParser::getActivePropertyType()
{
    return m_aValueType;
}
// -----------------------------------------------------------------------------

bool BasicParser::isInProperty()
{
    return m_aValueType.getTypeClass() != uno::TypeClass_VOID;
}
// -----------------------------------------------------------------------------

bool BasicParser::isInUnhandledProperty()
{
    return m_bEmpty && m_aValueType.getTypeClass() != uno::TypeClass_VOID;
}
// -----------------------------------------------------------------------------

void BasicParser::startValueData(const uno::Reference< sax::XAttributeList >& xAttribs)
{
    if (!isInProperty())
        raiseParseException( "Configuration XML Parser - Invalid Data: A value may occur only within a property" );

    if (m_aValueType.getTypeClass() == uno::TypeClass_ANY)
        raiseParseException( "Configuration XML Parser - Invalid Data: Cannot have values for properties of type 'Any'" );

    if (isInValueData())
        raiseParseException( "Configuration XML Parser - Invalid Data: Unexpected element while parsing value data" );

    m_pValueData = new ValueData(m_aValueType, asTCV(m_xTypeConverter));

    m_pValueData->setIsNull( getDataParser().isNull(xAttribs) );

    OUString aLocale;
    if ( getDataParser().getLanguage(xAttribs,aLocale) )
        m_pValueData->setLocalized( aLocale );

    if (!m_pValueData->isNull() && m_pValueData->isList())
        m_pValueData->setSeparator( getDataParser().getSeparator(xAttribs) );

    else
        OSL_ENSURE(getDataParser().getSeparator(xAttribs).getLength() == 0,
                    "Warning: Spurious oor:separator on value that is not a list (or null)");
}
// -----------------------------------------------------------------------------

bool BasicParser::isInValueData()
{
    return m_pValueData != NULL;
}
// -----------------------------------------------------------------------------

bool BasicParser::isValueDataLocalized()
{
    OSL_ENSURE(isInValueData(), "There is no value data that could be localized");

    return m_pValueData && m_pValueData->isLocalized;
}
// -----------------------------------------------------------------------------

OUString BasicParser::getValueDataLocale()
{
    OSL_ENSURE(isValueDataLocalized(), "There is no value data or it is not localized");

    return m_pValueData->locale;
}
// -----------------------------------------------------------------------------

uno::Any BasicParser::getCurrentValue()
{
    OSL_ASSERT( isInValueData() );

    return m_pValueData->convertToAny();
}
// -----------------------------------------------------------------------------

/// end collecting data for a value
void BasicParser::endValueData()
{
    OSL_ASSERT( isInValueData() );

    delete m_pValueData, m_pValueData = NULL;
    m_bEmpty = false;

    OSL_POSTCOND( !isInValueData(), "Could not end value data tag" );
    OSL_POSTCOND( !isInUnhandledProperty(), "Could not mark property as handled" );
}
// -----------------------------------------------------------------------------

void BasicParser::startSkipping( const OUString& aName, const uno::Reference< sax::XAttributeList >& xAttribs )
{
    m_aNodes.push( ElementInfo(aName) );
    ++m_nSkipLevels;
}
// -----------------------------------------------------------------------------

bool BasicParser::wasSkipping( const OUString& aName )
{
    if (m_nSkipLevels == 0) return false;

    if (m_aNodes.empty())
        raiseParseException( "Configuration XML Parser - Invalid XML: Unexpected end of element (while skipping data)" );

    if (aName != m_aNodes.top().name)
        raiseParseException( "Configuration XML Parser - Invalid XML: End tag does not match start tag (while skipping data)" );

    --m_nSkipLevels;
    m_aNodes.pop();

    return true;
}
// -----------------------------------------------------------------------------

bool BasicParser::isSkipping( )
{
    return m_nSkipLevels != 0;
}
// -----------------------------------------------------------------------------

void BasicParser::raiseParseException( uno::Any const & _aTargetException, sal_Char const * _pMsg )
        CFG_THROW2 (sax::SAXException, uno::RuntimeException)
{
    if (_pMsg == 0) _pMsg = "Configuration XML Parser: Invalid Data: ";

    OUString sMessage = OUString::createFromAscii(_pMsg);

    uno::Exception aEx;
    if (_aTargetException >>= aEx)
        sMessage += aEx.Message;

    throw sax::SAXException( sMessage, *this, _aTargetException );
}
// -----------------------------------------------------------------------------

void BasicParser::raiseParseException( sal_Char const * _pMsg )
        CFG_THROW2 (sax::SAXException, uno::RuntimeException)
{
    if (_pMsg == 0) _pMsg = "Configuration XML Parser: Invalid XML";

    OUString const sMessage = OUString::createFromAscii(_pMsg);

    throw sax::SAXException( sMessage, *this, uno::Any() );
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
    } // namespace

// -----------------------------------------------------------------------------
} // namespace

