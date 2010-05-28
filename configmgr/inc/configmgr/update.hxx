/*************************************************************************
*
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2000, 2010 Oracle and/or its affiliates.
*
* OpenOffice.org - a multi-platform office productivity suite
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
*
************************************************************************/

#ifndef INCLUDED_CONFIGMGR_UPDATE_HXX
#define INCLUDED_CONFIGMGR_UPDATE_HXX

#include "sal/config.h"

#include <set>

#include "configmgr/detail/configmgrdllapi.hxx"

namespace rtl { class OUString; }

namespace configmgr {

namespace update {

OOO_DLLPUBLIC_CONFIGMGR void insertExtensionXcsFile(
    bool shared, rtl::OUString const & fileUri);

OOO_DLLPUBLIC_CONFIGMGR void insertExtensionXcuFile(
    bool shared, rtl::OUString const & fileUri);

OOO_DLLPUBLIC_CONFIGMGR void insertModificationXcuFile(
    rtl::OUString const & fileUri,
    std::set< rtl::OUString > const & includedPaths,
    std::set< rtl::OUString > const & excludedPaths);

}

}

#endif
