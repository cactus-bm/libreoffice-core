/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TKey.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 02:00:14 $
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


#ifndef CONNECTIVITY_TKEY_HXX
#include "connectivity/TKey.hxx"
#endif
#ifndef CONNECTIVITY_TKEYCOLUMNS_HXX
#include "connectivity/TKeyColumns.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef CONNECTIVITY_CONNECTION_HXX
#include "TConnection.hxx"
#endif
#ifndef CONNECTIVITY_TABLEHELPER_HXX
#include "connectivity/TTableHelper.hxx"
#endif

using namespace connectivity;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
//  using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
// -------------------------------------------------------------------------
OTableKeyHelper::OTableKeyHelper(OTableHelper* _pTable) : connectivity::sdbcx::OKey(sal_True)
    ,m_pTable(_pTable)
{
    construct();
}
// -------------------------------------------------------------------------
OTableKeyHelper::OTableKeyHelper(   OTableHelper* _pTable,
            const ::rtl::OUString& _Name,
            const ::rtl::OUString& _ReferencedTable,
            sal_Int32       _Type,
            sal_Int32       _UpdateRule,
            sal_Int32       _DeleteRule
            ) : connectivity::sdbcx::OKey(_Name,
                          _ReferencedTable,
                          _Type,
                          _UpdateRule,
                          _DeleteRule,sal_True)
                ,m_pTable(_pTable)
{
    construct();
    refreshColumns();
}
// -------------------------------------------------------------------------
void OTableKeyHelper::refreshColumns()
{
    if ( !m_pTable )
        return;

    ::std::vector< ::rtl::OUString> aVector;
    if ( !isNew() )
    {
        ::dbtools::OPropertyMap& rPropMap = OMetaConnection::getPropMap();
        ::rtl::OUString aSchema,aTable;
        m_pTable->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_SCHEMANAME)) >>= aSchema;
        m_pTable->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_NAME))       >>= aTable;

        if ( m_Name.getLength() ) // foreign key
        {

            Reference< XResultSet > xResult = m_pTable->getMetaData()->getImportedKeys(m_pTable->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_CATALOGNAME)),
                aSchema,aTable);

            if ( xResult.is() )
            {
                Reference< XRow > xRow(xResult,UNO_QUERY);
                while( xResult->next() )
                {
                    ::rtl::OUString aForeignKeyColumn = xRow->getString(8);
                    if(xRow->getString(12) == m_Name)
                        aVector.push_back(aForeignKeyColumn);
                }
            }
        }
        if ( aVector.empty() )
        {
            Reference< XResultSet > xResult = m_pTable->getMetaData()->getPrimaryKeys(m_pTable->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_CATALOGNAME)),
                aSchema,aTable);

            if ( xResult.is() )
            {
                Reference< XRow > xRow(xResult,UNO_QUERY);
                while( xResult->next() )
                    aVector.push_back(xRow->getString(4));
            }
        }
    }


    if ( m_pColumns )
        m_pColumns ->reFill(aVector);
    else
        m_pColumns  = new OKeyColumnsHelper(this,m_aMutex,aVector);
}
// -----------------------------------------------------------------------------

