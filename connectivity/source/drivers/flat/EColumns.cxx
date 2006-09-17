/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: EColumns.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 02:36:59 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_connectivity.hxx"

#ifndef _CONNECTIVITY_FLAT_COLUMNS_HXX_
#include "flat/EColumns.hxx"
#endif
#ifndef _CONNECTIVITY_FLAT_TABLE_HXX_
#include "flat/ETable.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/sdbcx/VColumn.hxx"
#endif

using namespace connectivity::flat;
using namespace connectivity;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;


sdbcx::ObjectType OFlatColumns::createObject(const ::rtl::OUString& _rName)
{

    OFlatTable* pTable = (OFlatTable*)m_pTable;
    ::vos::ORef<OSQLColumns> aCols = pTable->getTableColumns();
    OSQLColumns::const_iterator aIter = find(aCols->begin(),aCols->end(),_rName,::comphelper::UStringMixEqual(isCaseSensitive()));
    sdbcx::ObjectType xRet;
    if(aIter != aCols->end())
        xRet = sdbcx::ObjectType(*aIter,UNO_QUERY);
    return xRet;
}
// -------------------------------------------------------------------------


