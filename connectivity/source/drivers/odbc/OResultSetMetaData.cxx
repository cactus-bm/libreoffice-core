/*************************************************************************
 *
 *  $RCSfile: OResultSetMetaData.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-15 08:18:13 $
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

#ifndef _CONNECTIVITY_ODBC_ORESULTSETMETADATA_HXX_
#include "odbc/OResultSetMetaData.hxx"
#endif
#ifndef _CONNECTIVITY_OTOOLS_HXX_
#include "odbc/OTools.hxx"
#endif

using namespace connectivity::odbc;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::sdbc;

// -------------------------------------------------------------------------
OResultSetMetaData::~OResultSetMetaData()
{
}
// -------------------------------------------------------------------------
::rtl::OUString OResultSetMetaData::getCharColAttrib(sal_Int32 _column,sal_Int32 ident) throw(SQLException, RuntimeException)
{
    sal_Int32 column = _column;
    if(_column <(sal_Int32) m_vMapping.size()) // use mapping
        column = m_vMapping[_column];

    sal_Int32 BUFFER_LEN = 128;
    char *pName = new char[BUFFER_LEN];
    SQLSMALLINT nRealLen=0;
    OTools::ThrowException(m_pConnection,N3SQLColAttribute(m_aStatementHandle,
                                    (SQLUSMALLINT)column,
                                    (SQLSMALLINT)ident,
                                    (SQLPOINTER)pName,
                                    BUFFER_LEN,
                                    &nRealLen,
                                    NULL
                                    ),m_aStatementHandle,SQL_HANDLE_STMT,*this);
    if(nRealLen > BUFFER_LEN)
    {
        delete pName;
        pName = new char[nRealLen];
        OTools::ThrowException(m_pConnection,N3SQLColAttribute(m_aStatementHandle,
                                    (SQLUSMALLINT)column,
                                    (SQLUSMALLINT)ident,
                                    (SQLPOINTER)pName,
                                    nRealLen,
                                    &nRealLen,
                                    NULL
                                    ),m_aStatementHandle,SQL_HANDLE_STMT,*this);
    }

    return ::rtl::OUString::createFromAscii(pName);
}
// -------------------------------------------------------------------------
sal_Int32 OResultSetMetaData::getNumColAttrib(sal_Int32 _column,sal_Int32 ident) throw(SQLException, RuntimeException)
{
    sal_Int32 column = _column;
    if(_column < (sal_Int32)m_vMapping.size()) // use mapping
        column = m_vMapping[_column];

    sal_Int32 nValue=0;
    sal_Int16 nLen = sizeof(nValue);
    OTools::ThrowException(m_pConnection,N3SQLColAttribute(m_aStatementHandle,
                                         (SQLUSMALLINT)column,
                                         (SQLUSMALLINT)ident,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &nValue),m_aStatementHandle,SQL_HANDLE_STMT,*this);
    return nValue;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OResultSetMetaData::getColumnDisplaySize( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_DISPLAY_SIZE);
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OResultSetMetaData::getColumnType( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    sal_Int32 nType = 0;
    try
    {
        nType = OTools::MapOdbcType2Jdbc(getNumColAttrib(column,SQL_DESC_TYPE));
    }
    catch(SQLException& ) // in this case we have an odbc 2.0 driver
    {
        nType = OTools::MapOdbcType2Jdbc(getNumColAttrib(column,SQL_DESC_CONCISE_TYPE ));
    }
    return nType;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OResultSetMetaData::getColumnCount(  ) throw(SQLException, RuntimeException)
{
    if(m_nColCount != -1)
        return m_nColCount;
    sal_Int16 nNumResultCols=0;
    OTools::ThrowException(m_pConnection,N3SQLNumResultCols(m_aStatementHandle,&nNumResultCols),m_aStatementHandle,SQL_HANDLE_STMT,*this);
    return m_nColCount = nNumResultCols;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSetMetaData::isCaseSensitive( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_CASE_SENSITIVE) == SQL_TRUE;
}
// -------------------------------------------------------------------------

::rtl::OUString SAL_CALL OResultSetMetaData::getSchemaName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getCharColAttrib(column,SQL_DESC_SCHEMA_NAME);
}
// -------------------------------------------------------------------------

::rtl::OUString SAL_CALL OResultSetMetaData::getColumnName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getCharColAttrib(column,SQL_DESC_NAME);
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OResultSetMetaData::getTableName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getCharColAttrib(column,SQL_DESC_TABLE_NAME);
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OResultSetMetaData::getCatalogName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getCharColAttrib(column,SQL_DESC_CATALOG_NAME);
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OResultSetMetaData::getColumnTypeName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getCharColAttrib(column,SQL_DESC_TYPE_NAME
);
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OResultSetMetaData::getColumnLabel( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getCharColAttrib(column,SQL_DESC_LABEL);
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OResultSetMetaData::getColumnServiceName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return ::rtl::OUString();
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSetMetaData::isCurrency( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_FIXED_PREC_SCALE) == SQL_TRUE;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSetMetaData::isAutoIncrement( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_AUTO_UNIQUE_VALUE) == SQL_TRUE;
}
// -------------------------------------------------------------------------


sal_Bool SAL_CALL OResultSetMetaData::isSigned( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_UNSIGNED) == SQL_FALSE;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OResultSetMetaData::getPrecision( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_PRECISION);
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL OResultSetMetaData::getScale( sal_Int32 column ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_SCALE);
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OResultSetMetaData::isNullable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_NULLABLE);
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSetMetaData::isSearchable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_SEARCHABLE) != SQL_PRED_NONE;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSetMetaData::isReadOnly( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_UPDATABLE) == SQL_ATTR_READONLY;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OResultSetMetaData::isDefinitelyWritable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_UPDATABLE) == SQL_ATTR_WRITE;
;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OResultSetMetaData::isWritable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    return getNumColAttrib(column,SQL_DESC_UPDATABLE) == SQL_ATTR_WRITE;
}
// -------------------------------------------------------------------------

