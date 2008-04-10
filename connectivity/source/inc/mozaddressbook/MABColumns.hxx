/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: MABColumns.hxx,v $
 * $Revision: 1.7 $
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

#ifndef _CONNECTIVITY_MAB_COLUMNS_HXX_
#define _CONNECTIVITY_MAB_COLUMNS_HXX_

#include "file/FColumns.hxx"

namespace connectivity
{
    namespace mozaddressbook
    {
        class OMozabColumns : public file::OColumns
        {
        protected:
            virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
        public:
            OMozabColumns(file::OFileTable* _pTable,
                            ::osl::Mutex& _rMutex,
                            const ::std::vector< ::rtl::OUString> &_rVector
                         ) : file::OColumns(_pTable,_rMutex,_rVector)
            {}

        };
    }
}

#endif // _CONNECTIVITY_MAB_COLUMNS_HXX_

