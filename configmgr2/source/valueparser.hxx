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

#ifndef INCLUDED_CONFIGMGR_SOURCE_VALUEPARSER_HXX
#define INCLUDED_CONFIGMGR_SOURCE_VALUEPARSER_HXX

#include "sal/config.h"

#include <vector>

#include "boost/noncopyable.hpp"
#include "rtl/ref.hxx"
#include "rtl/ustring.hxx"

#include "pad.hxx"
#include "span.hxx"
#include "type.hxx"
#include "xmlreader.hxx"

namespace com { namespace sun { namespace star { namespace uno {
    class Any;
} } } }

namespace configmgr {

class Node;

class ValueParser: private boost::noncopyable {
public:
    ValueParser(int layer);

    ~ValueParser();

    XmlReader::Text getTextMode() const;

    bool startElement(
        XmlReader & reader, XmlReader::Namespace ns, Span const & name);

    bool endElement(XmlReader const & reader);

    void characters(Span const & text);

    void start(
        rtl::Reference< Node > const & property,
        rtl::OUString const & localizedName = rtl::OUString());

    int getLayer() const;

    Type type_;
    Span separator_;

private:
    void checkEmptyPad(XmlReader const & reader) const;

    template< typename T > com::sun::star::uno::Any convertItems();

    enum State { STATE_TEXT, STATE_TEXT_UNICODE, STATE_IT, STATE_IT_UNICODE };

    int layer_;
    rtl::Reference< Node > node_;
    rtl::OUString localizedName_;
    State state_;
    Pad pad_;
    std::vector< com::sun::star::uno::Any > items_;
};

}

#endif
