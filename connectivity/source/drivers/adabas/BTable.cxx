/*************************************************************************
 *
 *  $RCSfile: BTable.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-18 08:48:06 $
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

#ifndef _CONNECTIVITY_ADABAS_TABLE_HXX_
#include "adabas/BTable.hxx"
#endif
#ifndef _CONNECTIVITY_ADABAS_INDEXES_HXX_
#include "adabas/BIndexes.hxx"
#endif
#ifndef _CONNECTIVITY_ADABAS_COLUMNS_HXX_
#include "adabas/BColumns.hxx"
#endif
#ifndef _CONNECTIVITY_ADABAS_KEYS_HXX_
#include "adabas/BKeys.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_KEYTYPE_HPP_
#include <com/sun/star/sdbcx/KeyType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_KEYRULE_HPP_
#include <com/sun/star/sdbc/KeyRule.hpp>
#endif
#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_COLUMNVALUE_HPP_
#include <com/sun/star/sdbc/ColumnValue.hpp>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif

using namespace ::comphelper;
using namespace connectivity::adabas;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

OAdabasTable::OAdabasTable( OAdabasConnection* _pConnection) : OTable_TYPEDEF(sal_True),m_pConnection(_pConnection)
{
    construct();
}
// -------------------------------------------------------------------------
OAdabasTable::OAdabasTable( OAdabasConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description ,
                    const ::rtl::OUString& _SchemaName,
                    const ::rtl::OUString& _CatalogName
                ) : OTable_TYPEDEF(sal_True,_Name,
                                  _Type,
                                  _Description,
                                  _SchemaName,
                                  _CatalogName)
                ,m_pConnection(_pConnection)
{
    construct();
    refreshColumns();
    refreshKeys();
    refreshIndexes();
}
// -------------------------------------------------------------------------
void OAdabasTable::refreshColumns()
{
    TStringVector aVector;
    if(!isNew())
    {
        Reference< XResultSet > xResult = m_pConnection->getMetaData()->getColumns(Any(),
                                                        m_SchemaName,m_Name,::rtl::OUString::createFromAscii("%"));

        if(xResult.is())
        {
            Reference< XRow > xRow(xResult,UNO_QUERY);
            while(xResult->next())
                aVector.push_back(xRow->getString(4));
        }
    }

    if(m_pColumns)
        m_pColumns->reFill(aVector);
    else
        m_pColumns  = new OColumns(this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void OAdabasTable::refreshPrimaryKeys(std::vector< ::rtl::OUString>& _rKeys)
{
    Reference< XResultSet > xResult = m_pConnection->getMetaData()->getPrimaryKeys(Any(),m_SchemaName,m_Name);

    if(xResult.is())
    {
        Reference< XRow > xRow(xResult,UNO_QUERY);
        if(xResult->next()) // there can be only one primary key
        {
            ::rtl::OUString aPkName = xRow->getString(6);
            _rKeys.push_back(aPkName);
        }
    }
}
// -------------------------------------------------------------------------
void OAdabasTable::refreshForgeinKeys(std::vector< ::rtl::OUString>& _rKeys)
{
    Reference< XResultSet > xResult = m_pConnection->getMetaData()->getImportedKeys(Any(),m_SchemaName,m_Name);

    if(xResult.is())
    {
        Reference< XRow > xRow(xResult,UNO_QUERY);
        while(xResult->next())
            _rKeys.push_back(xRow->getString(12));
    }
}
// -------------------------------------------------------------------------
void OAdabasTable::refreshKeys()
{
    TStringVector aVector;

    if(!isNew())
    {
        refreshPrimaryKeys(aVector);
        refreshForgeinKeys(aVector);
    }
    if(m_pKeys)
        m_pKeys->reFill(aVector);
    else
        m_pKeys = new OKeys(this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void OAdabasTable::refreshIndexes()
{
    TStringVector aVector;
    if(!isNew())
    {
        // fill indexes
        Reference< XResultSet > xResult = m_pConnection->getMetaData()->getIndexInfo(Any(),
        m_SchemaName,m_Name,sal_False,sal_False);

        if(xResult.is())
        {
            Reference< XRow > xRow(xResult,UNO_QUERY);
            ::rtl::OUString aName,aDot = ::rtl::OUString::createFromAscii(".");
            ::rtl::OUString sPreviousRoundName;
            while(xResult->next())
            {
                aName = xRow->getString(5);
                if(aName.getLength())
                    aName += aDot;
                aName += xRow->getString(6);
                if(aName.getLength())
                {
                    // don't insert the name if the last one we inserted was the same
                    if (sPreviousRoundName != aName)
                        aVector.push_back(aName);
                }
                sPreviousRoundName = aName;
            }
        }
    }

    if(m_pIndexes)
        m_pIndexes->reFill(aVector);
    else
        m_pIndexes  = new OIndexes(this,m_aMutex,aVector);
}
//--------------------------------------------------------------------------
Sequence< sal_Int8 > OAdabasTable::getUnoTunnelImplementationId()
{
    static ::cppu::OImplementationId * pId = 0;
    if (! pId)
    {
        ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
        if (! pId)
        {
            static ::cppu::OImplementationId aId;
            pId = &aId;
        }
    }
    return pId->getImplementationId();
}

// com::sun::star::lang::XUnoTunnel
//------------------------------------------------------------------
sal_Int64 OAdabasTable::getSomething( const Sequence< sal_Int8 > & rId ) throw (RuntimeException)
{
    if (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
        return (sal_Int64)this;

    return OTable_TYPEDEF::getSomething(rId);
}
// -------------------------------------------------------------------------
sal_Bool OAdabasTable::create() throw(SQLException, RuntimeException)
{
    ::rtl::OUString aSql = ::rtl::OUString::createFromAscii("CREATE TABLE ");
    ::rtl::OUString aQuote = m_pConnection->getMetaData()->getIdentifierQuoteString(  );
    ::rtl::OUString aDot = ::rtl::OUString::createFromAscii(".");
    ::rtl::OUString aComma = ::rtl::OUString::createFromAscii(", ");

    aSql += aQuote + m_SchemaName + aQuote + aDot + aQuote + m_Name + aQuote;
    aSql += ::rtl::OUString::createFromAscii(" ( ");

    sal_Int32 nCount = m_pColumns->getCount();
        Reference< XPropertySet > xProp;
    for(sal_Int32 i=0;i<nCount;++i)
    {
        ::cppu::extractInterface(xProp,m_pColumns->getByIndex(i));
        aSql += aQuote + getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) + aQuote
                + getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPENAME)));

        // add type definition
        switch(getINT32(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
                aSql += ::rtl::OUString::createFromAscii("(")
                            + ::rtl::OUString::valueOf(getINT32(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
                            + ::rtl::OUString::createFromAscii(")");
                break;

            case DataType::DECIMAL:
            case DataType::NUMERIC:
                aSql += ::rtl::OUString::createFromAscii("(")
                            + ::rtl::OUString::valueOf(getINT32(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
                            + ::rtl::OUString::createFromAscii(",")
                            + ::rtl::OUString::valueOf(getINT32(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))))
                            + ::rtl::OUString::createFromAscii(")");
                break;
        }

        ::rtl::OUString aDefault = getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_DEFAULTVALUE)));
                if(getINT32(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISNULLABLE))) == ColumnValue::NO_NULLS)
        {
            aSql += ::rtl::OUString::createFromAscii(" NOT NULL");
            if(aDefault.getLength())
                aSql += ::rtl::OUString::createFromAscii(" WITH DEFAULT");
        }
        else if(aDefault.getLength())
            aSql += ::rtl::OUString::createFromAscii(" DEFAULT ") + aDefault;

        aSql += aComma;
    }

    // create the key columns ( only the string )
    nCount = m_pKeys->getCount();

    for(i=0;i<nCount;++i)
    {
        ::cppu::extractInterface(xProp,m_pKeys->getByIndex(i));
                Reference< XColumnsSupplier > xKey(xProp,UNO_QUERY);
                Reference< ::com::sun::star::container::XIndexAccess > xCols(xKey->getColumns(),UNO_QUERY);
        switch(getINT32(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
        {
                case KeyType::PRIMARY:
            {
                sal_Int32 nCols = xCols->getCount();
                if(nCols)
                    aSql += ::rtl::OUString::createFromAscii(" PRIMARY KEY( ");
                for(sal_Int32 i=0;i<nCols;++i)
                {
                    ::cppu::extractInterface(xProp,xCols->getByIndex(i));
                    aSql += aQuote + getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) + aQuote;
                    aSql += aComma;
                }
                if(nCols)
                    aSql = aSql.replaceAt(aSql.getLength()-1,1,::rtl::OUString::createFromAscii(")"));
            }
                case KeyType::UNIQUE:
            {
                sal_Int32 nCols = xCols->getCount();
                if(nCols)
                    aSql += ::rtl::OUString::createFromAscii(" UNIQUE( ");
                for(sal_Int32 i=0;i<nCols;++i)
                {
                    ::cppu::extractInterface(xProp,xCols->getByIndex(i));
                    aSql += aQuote + getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) + aQuote;
                    aSql += aComma;
                }
                if(nCols)
                    aSql = aSql.replaceAt(aSql.getLength()-1,1,::rtl::OUString::createFromAscii(")"));
            }
                case KeyType::FOREIGN:
            {
                sal_Int32 nCols = xCols->getCount();
                if(nCols)
                {
                    aSql += ::rtl::OUString::createFromAscii(" FOREIGN KEY( ");
                    ::rtl::OUString aKeyName = getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)));
                    ::rtl::OUString aRefTableName = getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_REFERENCEDTABLE)));
                    sal_Int32 nDeleteRule = getINT32(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_DELETERULE)));
                    if(aKeyName.getLength())
                    {
                        aSql += aQuote + aKeyName + aQuote;
                        aSql += ::rtl::OUString::createFromAscii(" ");
                    }

                    for(sal_Int32 i=0;i<nCols;++i)
                    {
                        ::cppu::extractInterface(xProp,xCols->getByIndex(i));
                        aSql += aQuote + getString(xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) + aQuote;
                        aSql += aComma;
                    }
                    aSql = aSql.replaceAt(aSql.getLength()-1,1,::rtl::OUString::createFromAscii(")"));
                    aSql += ::rtl::OUString::createFromAscii(" REFERENCES ")
                                + aQuote + aRefTableName + aQuote;
                    switch(nDeleteRule)
                    {
                                        case KeyRule::CASCADE:
                        aSql += ::rtl::OUString::createFromAscii(" ON DELETE CASCADE ");
                        break;
                                        case KeyRule::RESTRICT:
                        aSql += ::rtl::OUString::createFromAscii(" ON DELETE RESTRICT ");
                        break;
                                        case KeyRule::SET_NULL:
                        aSql += ::rtl::OUString::createFromAscii(" ON DELETE SET NULL ");
                        break;
                                        case KeyRule::NO_ACTION:
                        break;
                                        case KeyRule::SET_DEFAULT:
                        aSql += ::rtl::OUString::createFromAscii(" ON DELETE SET DEFAULT ");
                        break;
                    }
                }

            }
        }
    }
        Reference< XStatement > xStmt = m_pConnection->createStatement(  );
    xStmt->execute(aSql);
    return sal_True;
}
// -------------------------------------------------------------------------
// XRename
void SAL_CALL OAdabasTable::rename( const ::rtl::OUString& newName ) throw(SQLException, ElementExistException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (
#ifdef GCC
        ::connectivity::sdbcx::OTableDescriptor_BASE::rBHelper.bDisposed
#else
        rBHelper.bDisposed
#endif
        )
                throw DisposedException();

    if(!isNew())
    {
        ::rtl::OUString aSql = ::rtl::OUString::createFromAscii("RENAME TABLE ");
        ::rtl::OUString aQuote = m_pConnection->getMetaData()->getIdentifierQuoteString(  );
        ::rtl::OUString aDot = ::rtl::OUString::createFromAscii(".");

        ::rtl::OUString aName,aSchema;
        sal_Int32 nLen = newName.indexOf('.');
        aSchema = newName.copy(0,nLen);
        aName   = newName.copy(nLen+1);

        aSql += aQuote + m_SchemaName + aQuote + aDot + aQuote + m_Name + aQuote
                    + ::rtl::OUString::createFromAscii(" TO ")
                    + aQuote + aSchema + aQuote + aDot + aQuote + aName + aQuote;

                Reference< XStatement > xStmt = m_pConnection->createStatement(  );
        xStmt->execute(aSql);
    }
    else
        m_Name = newName;
}
// -------------------------------------------------------------------------
// XAlterTable
void SAL_CALL OAdabasTable::alterColumnByName( const ::rtl::OUString& colName, const Reference< XPropertySet >& descriptor ) throw(SQLException, NoSuchElementException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (
#ifdef GCC
        ::connectivity::sdbcx::OTableDescriptor_BASE::rBHelper.bDisposed
#else
        rBHelper.bDisposed
#endif
        )
        throw DisposedException();

    if(m_pColumns && !m_pColumns->hasByName(colName))
        throw NoSuchElementException(colName,*this);


    if(!isNew())
    {
        if(getString(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))) != colName)
            throw SQLException(::rtl::OUString::createFromAscii("Not supported by this driver!"),*this,::rtl::OUString::createFromAscii("S1000"),0,Any() );

        Reference< XStatement > xStmt = m_pConnection->createStatement();
        xStmt->execute(::rtl::OUString::createFromAscii("SUBTRANS BEGIN") );

        try
        {
            // first we have to check what should be altered
            Reference<XPropertySet> xProp;
            m_pColumns->getByName(colName) >>= xProp;
            // first check the types
            sal_Int32 nOldType = 0,nNewType = 0;
            xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))     >>= nOldType;
            descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE)) >>= nNewType;
            if(nOldType != nNewType)
                alterColumnType(nNewType,colName,descriptor);

            // second: check the "is nullable" value
            sal_Int32 nOldNullable = 0,nNewNullable = 0;
            xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISNULLABLE))       >>= nOldNullable;
            descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISNULLABLE))  >>= nNewNullable;
            if(nNewNullable != nOldNullable)
                alterNotNullValue(nNewNullable,colName);

            // third: check the default values
            ::rtl::OUString sNewDefault,sOldDefault;
            xProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_DEFAULTVALUE))     >>= sOldDefault;
            descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_DEFAULTVALUE)) >>= sNewDefault;

            if(sOldDefault.getLength())
            {
                if(sNewDefault.getLength() && sOldDefault != sNewDefault)
                    alterDefaultValue(sNewDefault,colName);
                else if(!sNewDefault.getLength())
                    dropDefaultValue(colName);
            }
            else if(!sOldDefault.getLength() && sNewDefault.getLength())
                addDefaultValue(sNewDefault,colName);

            m_pColumns->refresh();
        }
        catch(const SQLException& e)
        {
            xStmt->execute(::rtl::OUString::createFromAscii("SUBTRANS ROLLBACK") );
            ::comphelper::disposeComponent(xStmt);
            throw;
        }
        xStmt->execute(::rtl::OUString::createFromAscii("SUBTRANS END") );
        ::comphelper::disposeComponent(xStmt);

    }
    else
    {
        if(m_pColumns)
        {
            m_pColumns->dropByName(colName);
            m_pColumns->appendByDescriptor(descriptor);
        }
    }

}
// -------------------------------------------------------------------------
void SAL_CALL OAdabasTable::alterColumnByIndex( sal_Int32 index, const Reference< XPropertySet >& descriptor ) throw(SQLException, ::com::sun::star::lang::IndexOutOfBoundsException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (
#ifdef GCC
        ::connectivity::sdbcx::OTableDescriptor_BASE::rBHelper.bDisposed
#else
        rBHelper.bDisposed
#endif
        )
                throw DisposedException();

        Reference< XPropertySet > xOld;
    if(::cppu::extractInterface(xOld,m_pColumns->getByIndex(index)) && xOld.is())
        alterColumnByName(getString(xOld->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))),descriptor);
}

// -------------------------------------------------------------------------
::rtl::OUString connectivity::adabas::getTypeString(const Reference< ::com::sun::star::beans::XPropertySet >& xColProp)
{
    ::rtl::OUString aValue;
    switch(getINT32(xColProp->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
    {
        case DataType::BIT:
            aValue = ::rtl::OUString::createFromAscii("BOOLEAN");
            break;
        case DataType::TINYINT:
            aValue = ::rtl::OUString::createFromAscii("SMALLINT");
            break;
        case DataType::SMALLINT:
            aValue = ::rtl::OUString::createFromAscii("SMALLINT");
            break;
        case DataType::INTEGER:
            aValue = ::rtl::OUString::createFromAscii("INT");
            break;
        case DataType::FLOAT:
            aValue = ::rtl::OUString::createFromAscii("FLOAT");
            break;
        case DataType::REAL:
            aValue = ::rtl::OUString::createFromAscii("REAL");
            break;
        case DataType::DOUBLE:
            aValue = ::rtl::OUString::createFromAscii("DOUBLE");
            break;
        case DataType::NUMERIC:
            aValue = ::rtl::OUString::createFromAscii("DECIMAL");
            break;
        case DataType::DECIMAL:
            aValue = ::rtl::OUString::createFromAscii("DECIMAL");
            break;
        case DataType::CHAR:
            aValue = ::rtl::OUString::createFromAscii("CHAR");
            break;
        case DataType::VARCHAR:
            aValue = ::rtl::OUString::createFromAscii("VARCHAR");
            break;
        case DataType::LONGVARCHAR:
            aValue = ::rtl::OUString::createFromAscii("LONG VARCHAR");
            break;
        case DataType::DATE:
            aValue = ::rtl::OUString::createFromAscii("DATE");
            break;
        case DataType::TIME:
            aValue = ::rtl::OUString::createFromAscii("TIME");
            break;
        case DataType::TIMESTAMP:
            aValue = ::rtl::OUString::createFromAscii("TIMESTAMP");
            break;
        case DataType::BINARY:
            aValue = ::rtl::OUString::createFromAscii("BOOLEAN");
            break;
        case DataType::VARBINARY:
            aValue = ::rtl::OUString::createFromAscii("VARCHAR BYTE");
            break;
        case DataType::LONGVARBINARY:
            aValue = ::rtl::OUString::createFromAscii("LONG BYTE");
            break;
    }
    return aValue;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OAdabasTable::getName() throw(::com::sun::star::uno::RuntimeException)
{
    ::rtl::OUString sName = m_SchemaName;
    sName += ::rtl::OUString::createFromAscii(".");
    sName += m_Name;
    return sName;
}
// -----------------------------------------------------------------------------
void SAL_CALL OAdabasTable::acquire() throw(::com::sun::star::uno::RuntimeException)
{
    OTable_TYPEDEF::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL OAdabasTable::release() throw(::com::sun::star::uno::RuntimeException)
{
    OTable_TYPEDEF::release();
}
// -----------------------------------------------------------------------------
void OAdabasTable::alterColumnType(sal_Int32 nNewType,const ::rtl::OUString& _rColName, const Reference<XPropertySet>& _xDescriptor)
{
    ::rtl::OUString aSql = ::rtl::OUString::createFromAscii("ALTER TABLE ");
    ::rtl::OUString aQuote = m_pConnection->getMetaData()->getIdentifierQuoteString(  );
    ::rtl::OUString aDot = ::rtl::OUString::createFromAscii(".");

    aSql += aQuote + m_SchemaName + aQuote + aDot + aQuote + m_Name + aQuote
                    + ::rtl::OUString::createFromAscii(" COLUMN ")
                    + aQuote + _rColName + aQuote;
    aSql += ::rtl::OUString::createFromAscii(" ")
            + getString(_xDescriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPENAME)));

    switch(nNewType)
    {
        case DataType::CHAR:
        case DataType::VARCHAR:
            aSql += ::rtl::OUString::createFromAscii("(")
                        + ::rtl::OUString::valueOf(getINT32(_xDescriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
                        + ::rtl::OUString::createFromAscii(")");
            break;

        case DataType::DECIMAL:
        case DataType::NUMERIC:
            aSql += ::rtl::OUString::createFromAscii("(")
                        + ::rtl::OUString::valueOf(getINT32(_xDescriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
                        + ::rtl::OUString::createFromAscii(",")
                        + ::rtl::OUString::valueOf(getINT32(_xDescriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))))
                        + ::rtl::OUString::createFromAscii(")");
            break;
    }

    Reference< XStatement > xStmt = m_pConnection->createStatement(  );
    xStmt->execute(aSql);
    ::comphelper::disposeComponent(xStmt);
}
// -----------------------------------------------------------------------------
void OAdabasTable::alterNotNullValue(sal_Int32 _nNewNullable,const ::rtl::OUString& _rColName)
{
    ::rtl::OUString aSql = ::rtl::OUString::createFromAscii("ALTER TABLE ");
    ::rtl::OUString aQuote = m_pConnection->getMetaData()->getIdentifierQuoteString(  );
    ::rtl::OUString aDot = ::rtl::OUString::createFromAscii(".");

    aSql += aQuote + m_SchemaName + aQuote + aDot + aQuote + m_Name + aQuote
                    + ::rtl::OUString::createFromAscii(" COLUMN ")
                    + aQuote + _rColName + aQuote;
    if(_nNewNullable == ColumnValue::NO_NULLS)
    {
        aSql += ::rtl::OUString::createFromAscii(" NOT NULL");
    }
    else
    {
        aSql += ::rtl::OUString::createFromAscii(" DEFAULT NULL");
    }

    Reference< XStatement > xStmt = m_pConnection->createStatement();
    xStmt->execute(aSql);
    ::comphelper::disposeComponent(xStmt);
}
// -----------------------------------------------------------------------------
void OAdabasTable::alterDefaultValue(const ::rtl::OUString& _sNewDefault,const ::rtl::OUString& _rColName)
{
    ::rtl::OUString aSql = ::rtl::OUString::createFromAscii("ALTER TABLE ");
    ::rtl::OUString aQuote = m_pConnection->getMetaData()->getIdentifierQuoteString(  );
    ::rtl::OUString aDot = ::rtl::OUString::createFromAscii(".");

    aSql += aQuote + m_SchemaName + aQuote + aDot + aQuote + m_Name + aQuote
                    + ::rtl::OUString::createFromAscii(" COLUMN ")
                    + aQuote + _rColName + aQuote;
    aSql += ::rtl::OUString::createFromAscii(" ALTER ") + _sNewDefault;

    Reference< XStatement > xStmt = m_pConnection->createStatement();
    xStmt->execute(aSql);
    ::comphelper::disposeComponent(xStmt);
}
// -----------------------------------------------------------------------------
void OAdabasTable::dropDefaultValue(const ::rtl::OUString& _rColName)
{
    ::rtl::OUString aSql = ::rtl::OUString::createFromAscii("ALTER TABLE ");
    ::rtl::OUString aQuote = m_pConnection->getMetaData()->getIdentifierQuoteString(  );
    ::rtl::OUString aDot = ::rtl::OUString::createFromAscii(".");

    aSql += aQuote + m_SchemaName + aQuote + aDot + aQuote + m_Name + aQuote
                    + ::rtl::OUString::createFromAscii(" COLUMN ")
                    + aQuote + _rColName + aQuote;
    aSql += ::rtl::OUString::createFromAscii(" DROP DEFAULT");

    Reference< XStatement > xStmt = m_pConnection->createStatement();
    xStmt->execute(aSql);
    ::comphelper::disposeComponent(xStmt);
}
// -----------------------------------------------------------------------------
void OAdabasTable::addDefaultValue(const ::rtl::OUString& _sNewDefault,const ::rtl::OUString& _rColName)
{
    ::rtl::OUString aSql = ::rtl::OUString::createFromAscii("ALTER TABLE ");
    ::rtl::OUString aQuote = m_pConnection->getMetaData()->getIdentifierQuoteString(  );
    ::rtl::OUString aDot = ::rtl::OUString::createFromAscii(".");

    aSql += aQuote + m_SchemaName + aQuote + aDot + aQuote + m_Name + aQuote
                    + ::rtl::OUString::createFromAscii(" COLUMN ")
                    + aQuote + _rColName + aQuote;
    aSql += ::rtl::OUString::createFromAscii(" ADD ") + _sNewDefault;

    Reference< XStatement > xStmt = m_pConnection->createStatement();
    xStmt->execute(aSql);
    ::comphelper::disposeComponent(xStmt);
}
// -----------------------------------------------------------------------------





