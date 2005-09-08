/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: basicparser.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 04:38:11 $
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

#ifndef CONFIGMGR_XML_BASICPARSER_HXX
#define CONFIGMGR_XML_BASICPARSER_HXX

#ifndef CONFIGMGR_XML_ELEMENTPARSER_HXX
#include "elementparser.hxx"
#endif

#ifndef CONFIGMGR_UTILITY_HXX_
#include "utility.hxx"
#endif
#ifndef CONFIGMGR_STACK_HXX_
#include "stack.hxx"
#endif
#ifndef CONFIGMGR_LOGGER_HXX_
#include "logger.hxx"
#endif

#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace xml
    {
// -----------------------------------------------------------------------------
        namespace uno           = ::com::sun::star::uno;
        namespace lang          = ::com::sun::star::lang;

        namespace sax           = ::com::sun::star::xml::sax;

        using rtl::OUString;
// -----------------------------------------------------------------------------

        typedef ::cppu::WeakImplHelper1<sax::XDocumentHandler> Parser_Base;

        class BasicParser
        :   public Parser_Base
        {
            struct ValueData;

            uno::Reference< uno::XInterface > m_xTypeConverter;
            uno::Reference< sax::XLocator > m_xLocator;
            ElementParser                   m_aDataParser;
            Stack< ElementInfo >            m_aNodes;
            uno::Type                       m_aValueType;
            ValueData *                     m_pValueData;
            sal_uInt16                      m_nSkipLevels;
            bool                            m_bEmpty;
            bool                            m_bInProperty;

#if OSL_DEBUG_LEVEL > 0
#ifdef DBG_UTIL
            OUString  dbgPublicId,    dbgSystemId;
            sal_Int32 dbgLineNo,      dbgColumnNo;
#endif // DBG_UTIL
            void dbgUpdateLocation();
#endif // OSL_DEBUG_LEVEL

        public:
            typedef uno::Reference< uno::XComponentContext > Context;

            explicit BasicParser(Context const & _xContext);
            virtual ~BasicParser();

        // XDocumentHandler
        public:
            virtual void SAL_CALL
                startDocument(  )
                    throw (sax::SAXException, uno::RuntimeException);

            virtual void SAL_CALL
                endDocument(  ) throw (sax::SAXException, uno::RuntimeException);

            virtual void SAL_CALL
                characters( const OUString& aChars )
                    throw (sax::SAXException, uno::RuntimeException);

            virtual void SAL_CALL
                ignorableWhitespace( const OUString& aWhitespaces )
                    throw (sax::SAXException, uno::RuntimeException);

            virtual void SAL_CALL
                processingInstruction( const OUString& aTarget, const OUString& aData )
                    throw (sax::SAXException, uno::RuntimeException);

            virtual void SAL_CALL
                setDocumentLocator( const uno::Reference< sax::XLocator >& xLocator )
                    throw (sax::SAXException, uno::RuntimeException);

        protected:
            ElementParser const & getDataParser() const { return m_aDataParser; }

            Logger const & getLogger() { return m_aDataParser.logger(); }

            /// start an node
            void startNode( ElementInfo const & aInfo, const uno::Reference< sax::XAttributeList >& xAttribs );
            /// are we in the content of a  node ?
            bool isInNode();
            /// are we in the content of node for which no content was started yet ?
            bool isEmptyNode();
            /// make sure we are in the content of a  node ?
            void ensureInNode();
            /// get the info about of the node currently being processed
            ElementInfo const & getActiveNodeInfo();
            /// end a node
            void endNode();

            /// start a property
            void startProperty( ElementInfo const & aInfo, const uno::Reference< sax::XAttributeList >& xAttribs );
            /// are we in the content of a property node ?
            bool isInProperty();
            /// are we in the content of a property node (and there has been no value for that property) ?
            bool isInUnhandledProperty();
            /// get the data type of the active property ?
            uno::Type getActivePropertyType();
            /// end a property
            void endProperty();

            /// start collecting data for a value - returns the locale of the value (property must have been started)
            void startValueData(const uno::Reference< sax::XAttributeList >& xAttribs);
            /// are we in the content of a property node ?
            bool isInValueData();
            /// check if the current value data has a locale set
            bool isValueDataLocalized();
            /// get the locale of the current value data, if localized
            OUString getValueDataLocale();
            /// return the collected value
            uno::Any getCurrentValue();
            /// end collecting data for a value
            void endValueData();

            /// start a node to be skipped
            void startSkipping( const OUString& aTag, const uno::Reference< sax::XAttributeList >& xAttribs );
            /// are we inside a skipped node ?
            bool isSkipping( );
            /// ending a node: was this skipped ?
            bool wasSkipping( const OUString& aTag );

        protected:
            void raiseParseException( uno::Any const & _aTargetException, sal_Char const * _pMsg = NULL)
                    CFG_THROW2 (sax::SAXException, uno::RuntimeException);
            void raiseParseException( sal_Char const * _pMsg )
                    CFG_THROW2 (sax::SAXException, uno::RuntimeException);
            void raiseParseException( OUString const & aMsg )
                    CFG_THROW2 (sax::SAXException, uno::RuntimeException);
        };
// -----------------------------------------------------------------------------
    } // namespace xml
// -----------------------------------------------------------------------------

} // namespace configmgr
#endif




