/*************************************************************************
 *
 *  $RCSfile: BColumns.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-14 11:41:57 $
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
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _CONNECTIVITY_ADABAS_COLUMNS_HXX_
#include "adabas/BColumns.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/sdbcx/VColumn.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/sdbcx/VColumn.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_COLUMNVALUE_HPP_
#include <com/sun/star/sdbc/ColumnValue.hpp>
#endif
#ifndef _CONNECTIVITY_ADABAS_TABLE_HXX_
#include "adabas/BTable.hxx"
#endif


using namespace connectivity::adabas;
using namespace connectivity::sdbcx;
using namespace connectivity;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
//  using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
typedef connectivity::sdbcx::OCollection OCollection_TYPE;

Reference< XNamed > OColumns::createObject(const ::rtl::OUString& _rName)
{
    Reference< XResultSet > xResult = m_pTable->getConnection()->getMetaData()->getColumns(Any(),
    m_pTable->getSchema(),m_pTable->getTableName(),_rName);

    Reference< XNamed > xRet = NULL;
    if(xResult.is())
    {
                Reference< XRow > xRow(xResult,UNO_QUERY);
        while(xResult->next())
        {
            if(xRow->getString(4) == _rName)
            {
                OColumn* pRet = new OColumn(_rName,
                                            xRow->getString(6),
                                            xRow->getString(13),
                                            xRow->getInt(11),
                                            xRow->getInt(7),
                                            xRow->getInt(9),
                                            xRow->getInt(5),
                                            sal_False,sal_False,sal_False,sal_True);
                xRet = pRet;
                break;
            }
        }
    }

    return xRet;
}

// -------------------------------------------------------------------------
void OColumns::impl_refresh() throw(RuntimeException)
{
    m_pTable->refreshColumns();
}
// -------------------------------------------------------------------------
Reference< XPropertySet > OColumns::createEmptyObject()
{
    OColumn* pNew = new OColumn(sal_True);
    Reference< XPropertySet > xRet = pNew;
    return xRet;
}
// -------------------------------------------------------------------------
// XAppend
void SAL_CALL OColumns::appendByDescriptor( const Reference< XPropertySet >& descriptor ) throw(SQLException, ElementExistException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_rMutex);
    if(!m_pTable->isNew())
    {
        ::rtl::OUString aSql    = ::rtl::OUString::createFromAscii("ALTER TABLE ");
        ::rtl::OUString aQuote  = m_pTable->getConnection()->getMetaData()->getIdentifierQuoteString(  );
        ::rtl::OUString aDot    = ::rtl::OUString::createFromAscii(".");

        aSql = aSql + aQuote + m_pTable->getSchema() + aQuote + aDot + aQuote + m_pTable->getTableName() + aQuote;
        aSql = aSql + ::rtl::OUString::createFromAscii(" ADD ");
        aSql = aSql + aQuote + getString(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) + aQuote;
        aSql = aSql + ::rtl::OUString::createFromAscii(" ");

        Any aTypeName = descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPENAME));
        if(aTypeName.hasValue() && getString(aTypeName).getLength())
            aSql = aSql + getString(aTypeName);
        else
            aSql = aSql + getTypeString(descriptor) + ::rtl::OUString::createFromAscii(" ");

        //  aSql = aSql + getString(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPENAME));

        switch(getINT32(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
                aSql = aSql + ::rtl::OUString::createFromAscii("(")
                            + ::rtl::OUString::valueOf(getINT32(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION))))
                            + ::rtl::OUString::createFromAscii(")");
                break;

            case DataType::DECIMAL:
            case DataType::NUMERIC:
                aSql = aSql + ::rtl::OUString::createFromAscii("(")
                            + ::rtl::OUString::valueOf(getINT32(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION))))
                            + ::rtl::OUString::createFromAscii(",")
                            + ::rtl::OUString::valueOf(getINT32(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))))
                            + ::rtl::OUString::createFromAscii(")");
                break;
        }
        ::rtl::OUString aDefault = getString(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_DEFAULTVALUE)));
        if(getINT32(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISNULLABLE))) == ColumnValue::NO_NULLS)
        {
            aSql = aSql + ::rtl::OUString::createFromAscii(" NOT NULL");
            if(aDefault.getLength())
                aSql = aSql + ::rtl::OUString::createFromAscii(" WITH DEFAULT");
        }
        else if(aDefault.getLength())
            aSql = aSql + ::rtl::OUString::createFromAscii(" DEFAULT ") + aDefault;

        Reference< XStatement > xStmt = m_pTable->getConnection()->createStatement(  );
        xStmt->execute(aSql);
    }
    OCollection_TYPE::appendByDescriptor(descriptor);
}
// -------------------------------------------------------------------------
// XDrop
void SAL_CALL OColumns::dropByName( const ::rtl::OUString& elementName ) throw(SQLException, NoSuchElementException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_rMutex);
    if(!m_pTable->isNew())
    {
        ::rtl::OUString aSql    = ::rtl::OUString::createFromAscii("ALTER TABLE ");
        ::rtl::OUString aQuote  = m_pTable->getConnection()->getMetaData()->getIdentifierQuoteString(  );
        ::rtl::OUString aDot    = ::rtl::OUString::createFromAscii(".");

        aSql = aSql + aQuote + m_pTable->getSchema() + aQuote + aDot + aQuote + m_pTable->getTableName() + aQuote;
        aSql = aSql + ::rtl::OUString::createFromAscii(" DROP ");
        aSql = aSql + aQuote + elementName + aQuote;

        Reference< XStatement > xStmt = m_pTable->getConnection()->createStatement(  );
        xStmt->execute(aSql);
    }

    OCollection_TYPE::dropByName(elementName);
}
// -------------------------------------------------------------------------
void SAL_CALL OColumns::dropByIndex( sal_Int32 index ) throw(SQLException, IndexOutOfBoundsException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_rMutex);
    if (index < 0 || index >= getCount())
        throw IndexOutOfBoundsException(::rtl::OUString::valueOf(index),*this);

    if(!m_pTable->isNew())
    {
        ::rtl::OUString aSql    = ::rtl::OUString::createFromAscii("ALTER TABLE ");
        ::rtl::OUString aQuote  = m_pTable->getConnection()->getMetaData()->getIdentifierQuoteString(  );
        ::rtl::OUString aDot    = ::rtl::OUString::createFromAscii(".");

        aSql = aSql + aQuote + m_pTable->getSchema() + aQuote + aDot + aQuote + m_pTable->getTableName() + aQuote;
        aSql = aSql + ::rtl::OUString::createFromAscii(" DROP ");
        aSql = aSql + aQuote + m_aElements[index]->first + aQuote;

        Reference< XStatement > xStmt = m_pTable->getConnection()->createStatement(  );
        xStmt->execute(aSql);
    }
    OCollection_TYPE::dropByIndex(index);
}


