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

#ifndef INCLUDED_CONFIGMGR_SOURCE_PARSEMANAGER_HXX
#define INCLUDED_CONFIGMGR_SOURCE_PARSEMANAGER_HXX

#include "sal/config.h"

#include "com/sun/star/container/NoSuchElementException.hpp"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "rtl/ref.hxx"
#include "sal/types.h"
#include "salhelper/simplereferenceobject.hxx"

#include "span.hxx"
#include "xmlreader.hxx"

namespace rtl { class OUString; }

namespace configmgr {

class Parser;

class ParseManager: public salhelper::SimpleReferenceObject {
public:
    ParseManager(
        rtl::OUString const & url, rtl::Reference< Parser > const & parser)
        SAL_THROW((
            com::sun::star::container::NoSuchElementException,
            com::sun::star::uno::UnoRuntimeException));

    bool parse();

private:
    virtual ~ParseManager();

    XmlReader reader_;
    rtl::Reference< Parser > parser_;
    Span itemData_;
    XmlReader::Namespace itemNamespace_;
};

}

#endif
