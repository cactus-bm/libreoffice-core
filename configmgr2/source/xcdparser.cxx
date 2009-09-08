/*************************************************************************
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2009 by Sun Microsystems, Inc.
*
* OpenOffice.org - a multi-platform office productivity suite
*
* $RCSfile: code,v $
*
* $Revision: 1.4 $
*
* This file is part of OpenOffice.org.
*
* OpenOffice.org is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License version 3
* only, as published by the Free Software Foundation.
*
* OpenOffice.org is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License version 3 for more details
* (a copy is included in the LICENSE file that accompanied this code).
*
* You should have received a copy of the GNU Lesser General Public License
* version 3 along with OpenOffice.org.  If not, see
* <http://www.openoffice.org/license.html>
* for a copy of the LGPLv3 License.
************************************************************************/

#include "precompiled_configmgr.hxx"
#include "sal/config.h"

#include <cstddef>

#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "osl/diagnose.hxx"
#include "rtl/string.h"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

#include "span.hxx"
#include "xcdparser.hxx"
#include "xcsparser.hxx"
#include "xcuparser.hxx"
#include "xmldata.hxx"
#include "xmlreader.hxx"

namespace configmgr {

namespace {

namespace css = com::sun::star;

}

XcdParser::XcdParser(
    int layer, Dependencies const & dependencies, Data * data):
    layer_(layer), dependencies_(dependencies), data_(data), state_(STATE_START)
{}

XcdParser::~XcdParser() {}

XmlReader::Text XcdParser::getTextMode() {
    return nestedParser_.is()
        ? nestedParser_->getTextMode() : XmlReader::TEXT_NONE;
}

bool XcdParser::startElement(
    XmlReader * reader, XmlReader::Namespace ns, Span const & name)
{
    if (nestedParser_.is()) {
        OSL_ASSERT(nesting_ != LONG_MAX);
        ++nesting_;
        return nestedParser_->startElement(reader, ns, name);
    }
    switch (state_) {
    case STATE_START:
        if (ns == XmlReader::NAMESPACE_OOR &&
            name.equals(RTL_CONSTASCII_STRINGPARAM("data")))
        {
            state_ = STATE_DEPENDENCIES;
            return true;
        }
        break;
    case STATE_DEPENDENCIES:
        if (ns == XmlReader::NAMESPACE_NONE &&
            name.equals(RTL_CONSTASCII_STRINGPARAM("dependency")))
        {
            if (dependency_.getLength() == 0) {
                Span attrFile;
                for (;;) {
                    XmlReader::Namespace attrNs;
                    Span attrLn;
                    if (!reader->nextAttribute(&attrNs, &attrLn)) {
                        break;
                    }
                    if (attrNs == XmlReader::NAMESPACE_NONE && //TODO: _OOR
                        attrLn.equals(RTL_CONSTASCII_STRINGPARAM("file")))
                    {
                        attrFile = reader->getAttributeValue(false);
                    }
                }
                if (!attrFile.is()) {
                    throw css::uno::RuntimeException(
                        (rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                "no dependency file attribute in ")) +
                         reader->getUrl()),
                        css::uno::Reference< css::uno::XInterface >());
                }
                dependency_ = xmldata::convertFromUtf8(attrFile);
                if (dependency_.getLength() == 0) {
                    throw css::uno::RuntimeException(
                        (rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                "bad dependency file attribute in ")) +
                         reader->getUrl()),
                        css::uno::Reference< css::uno::XInterface >());
                }
            }
            if (dependencies_.find(dependency_) == dependencies_.end()) {
                return false;
            }
            state_ = STATE_DEPENDENCY;
            dependency_ = rtl::OUString();
            return true;
        }
        state_ = STATE_COMPONENTS;
        // fall through
    case STATE_COMPONENTS:
        if (ns == XmlReader::NAMESPACE_OOR &&
            name.equals(RTL_CONSTASCII_STRINGPARAM("component-schema")))
        {
            nestedParser_ = new XcsParser(layer_, data_);
            nesting_ = 1;
            return nestedParser_->startElement(reader, ns, name);
        }
        if (ns == XmlReader::NAMESPACE_OOR &&
            name.equals(RTL_CONSTASCII_STRINGPARAM("component-data")))
        {
            nestedParser_ = new XcuParser(layer_ + 1, data_);
            nesting_ = 1;
            return nestedParser_->startElement(reader, ns, name);
        }
        break;
    default: // STATE_DEPENDENCY
        OSL_ASSERT(false); // this cannot happen
        break;
    }
    throw css::uno::RuntimeException(
        (rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("bad member <")) +
         xmldata::convertFromUtf8(name) +
         rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("> in ")) +
         reader->getUrl()),
        css::uno::Reference< css::uno::XInterface >());
}

void XcdParser::endElement(XmlReader const * reader) {
    if (nestedParser_.is()) {
        nestedParser_->endElement(reader);
        if (--nesting_ == 0) {
            nestedParser_.clear();
        }
    } else {
        switch (state_) {
        case STATE_DEPENDENCY:
            state_ = STATE_DEPENDENCIES;
            break;
        case STATE_DEPENDENCIES:
        case STATE_COMPONENTS:
            break;
        default:
            OSL_ASSERT(false); // this cannot happen
            break;
        }
    }
}

void XcdParser::characters(Span const & text) {
    if (nestedParser_.is()) {
        nestedParser_->characters(text);
    }
}

}
