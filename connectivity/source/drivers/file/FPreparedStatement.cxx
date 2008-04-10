/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: FPreparedStatement.cxx,v $
 * $Revision: 1.42 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_connectivity.hxx"

#include <stdio.h>
#include "connectivity/sdbcx/VColumn.hxx"
#include <osl/diagnose.h>
#include "file/FPreparedStatement.hxx"
#include <com/sun/star/sdbc/DataType.hpp>
#include "file/FResultSetMetaData.hxx"
#include <cppuhelper/typeprovider.hxx>
#include <comphelper/sequence.hxx>
#include <com/sun/star/lang/DisposedException.hpp>
#include "connectivity/dbconversion.hxx"
#include "connectivity/dbexception.hxx"
#include "connectivity/dbtools.hxx"
#include "connectivity/PColumn.hxx"
#include "diagnose_ex.h"
#include <comphelper/types.hxx>
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <tools/debug.hxx>

using namespace connectivity;
using namespace comphelper;
using namespace ::dbtools;
using namespace connectivity::file;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;
using namespace com::sun::star::container;
using namespace com::sun::star::util;

IMPLEMENT_SERVICE_INFO(OPreparedStatement,"com.sun.star.sdbc.driver.file.PreparedStatement","com.sun.star.sdbc.PreparedStatement");

DBG_NAME( file_OPreparedStatement )
// -------------------------------------------------------------------------
OPreparedStatement::OPreparedStatement( OConnection* _pConnection)
    : OStatement_BASE2( _pConnection )
    ,m_pResultSet(NULL)
{
    DBG_CTOR( file_OPreparedStatement, NULL );
}

// -------------------------------------------------------------------------
OPreparedStatement::~OPreparedStatement()
{
    DBG_DTOR( file_OPreparedStatement, NULL );
}

// -------------------------------------------------------------------------
void OPreparedStatement::disposing()
{
    ::osl::MutexGuard aGuard(m_aMutex);

    clearMyResultSet();
    OStatement_BASE2::disposing();

    if(m_pResultSet)
    {
        m_pResultSet->release();
        m_pResultSet = NULL;
    }

    m_xParamColumns = NULL;


    m_xMetaData = NULL;
    if(m_aParameterRow.isValid())
    {
        m_aParameterRow->clear();
        m_aParameterRow = NULL;
    }


}
// -------------------------------------------------------------------------
void OPreparedStatement::construct(const ::rtl::OUString& sql)  throw(SQLException, RuntimeException)
{
    OStatement_Base::construct(sql);

    m_aParameterRow = new OValueRefVector();
    m_aParameterRow->push_back(new ORowSetValueDecorator(sal_Int32(0)) );

    Reference<XIndexAccess> xNames(m_xColNames,UNO_QUERY);

    if ( m_aSQLIterator.getStatementType() == SQL_STATEMENT_SELECT )
        m_xParamColumns = m_aSQLIterator.getParameters();
    else
    {
        m_xParamColumns = new OSQLColumns();
        // describe all parameters need for the resultset
        describeParameter();
    }

    OValueRefRow aTemp;
    OResultSet::setBoundedColumns(m_aEvaluateRow,aTemp,m_xParamColumns,xNames,sal_False,m_xDBMetaData,m_aColMapping);

    m_pResultSet = createResultSet();
    m_pResultSet->acquire();
    m_xResultSet = Reference<XResultSet>(m_pResultSet);
    initializeResultSet(m_pResultSet);
}
// -------------------------------------------------------------------------

Any SAL_CALL OPreparedStatement::queryInterface( const Type & rType ) throw(RuntimeException)
{
    Any aRet = OStatement_BASE2::queryInterface(rType);
    return aRet.hasValue() ? aRet : ::cppu::queryInterface( rType,
                                        static_cast< XPreparedStatement*>(this),
                                        static_cast< XParameters*>(this),
                                        static_cast< XResultSetMetaDataSupplier*>(this));
}
// -------------------------------------------------------------------------
::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL OPreparedStatement::getTypes(  ) throw(::com::sun::star::uno::RuntimeException)
{
        ::cppu::OTypeCollection aTypes( ::getCppuType( (const ::com::sun::star::uno::Reference< XPreparedStatement > *)0 ),
                                        ::getCppuType( (const ::com::sun::star::uno::Reference< XParameters > *)0 ),
                                        ::getCppuType( (const ::com::sun::star::uno::Reference< XResultSetMetaDataSupplier > *)0 ));

    return ::comphelper::concatSequences(aTypes.getTypes(),OStatement_BASE2::getTypes());
}
// -------------------------------------------------------------------------

Reference< XResultSetMetaData > SAL_CALL OPreparedStatement::getMetaData(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);


    if(!m_xMetaData.is())
        m_xMetaData = new OResultSetMetaData(m_aSQLIterator.getSelectColumns(),m_aSQLIterator.getTables().begin()->first,m_pTable);
    return m_xMetaData;
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::close(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);


    clearMyResultSet();
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OPreparedStatement::execute(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);

    initResultSet();

    return m_aSQLIterator.getStatementType() == SQL_STATEMENT_SELECT;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OPreparedStatement::executeUpdate(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);

    initResultSet();

    return m_pResultSet ? m_pResultSet->getRowCountResult() : sal_Int32(0);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setString( sal_Int32 parameterIndex, const ::rtl::OUString& x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}
// -------------------------------------------------------------------------

Reference< XConnection > SAL_CALL OPreparedStatement::getConnection(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);

    return (Reference< XConnection >)m_pConnection;
}
// -------------------------------------------------------------------------

Reference< XResultSet > SAL_CALL OPreparedStatement::executeQuery(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);

    return initResultSet();
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setBoolean( sal_Int32 parameterIndex, sal_Bool x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}
// -------------------------------------------------------------------------
void SAL_CALL OPreparedStatement::setByte( sal_Int32 parameterIndex, sal_Int8 x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setDate( sal_Int32 parameterIndex, const Date& aData ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,DBTypeConversion::toDouble(aData));
}
// -------------------------------------------------------------------------
void SAL_CALL OPreparedStatement::setTime( sal_Int32 parameterIndex, const Time& aVal ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,DBTypeConversion::toDouble(aVal));
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setTimestamp( sal_Int32 parameterIndex, const DateTime& aVal ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,DBTypeConversion::toDouble(aVal));
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setDouble( sal_Int32 parameterIndex, double x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}

// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setFloat( sal_Int32 parameterIndex, float x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setInt( sal_Int32 parameterIndex, sal_Int32 x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setLong( sal_Int32 /*parameterIndex*/, sal_Int64 /*aVal*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XParameters::setLong", *this );
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setNull( sal_Int32 parameterIndex, sal_Int32 /*sqlType*/ ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkAndResizeParameters(parameterIndex);

    if ( m_aAssignValues.isValid() )
        (*m_aAssignValues)[m_aParameterIndexes[parameterIndex]]->setNull();
    else
        (*m_aParameterRow)[parameterIndex]->setNull();
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setClob( sal_Int32 /*parameterIndex*/, const Reference< XClob >& /*x*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XParameters::setClob", *this );
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setBlob( sal_Int32 /*parameterIndex*/, const Reference< XBlob >& /*x*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XParameters::setBlob", *this );
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setArray( sal_Int32 /*parameterIndex*/, const Reference< XArray >& /*x*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XParameters::setArray", *this );
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setRef( sal_Int32 /*parameterIndex*/, const Reference< XRef >& /*x*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XParameters::setRef", *this );
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setObjectWithInfo( sal_Int32 parameterIndex, const Any& x, sal_Int32 sqlType, sal_Int32 scale ) throw(SQLException, RuntimeException)
{
    switch(sqlType)
    {
        case DataType::DECIMAL:
        case DataType::NUMERIC:
            setString(parameterIndex,::comphelper::getString(x));
            break;
        default:
            ::dbtools::setObjectWithInfo(this,parameterIndex,x,sqlType,scale);
            break;
    }
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setObjectNull( sal_Int32 parameterIndex, sal_Int32 sqlType, const ::rtl::OUString& /*typeName*/ ) throw(SQLException, RuntimeException)
{
    setNull(parameterIndex,sqlType);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setObject( sal_Int32 /*parameterIndex*/, const Any& /*x*/ ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);

    throwFeatureNotImplementedException( "XParameters::setObject", *this );
    //  setObject (parameterIndex, x, sqlType, 0);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setShort( sal_Int32 parameterIndex, sal_Int16 x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setBytes( sal_Int32 parameterIndex, const Sequence< sal_Int8 >& x ) throw(SQLException, RuntimeException)
{
    setParameter(parameterIndex,x);
}
// -------------------------------------------------------------------------


void SAL_CALL OPreparedStatement::setCharacterStream( sal_Int32 parameterIndex, const Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length ) throw(SQLException, RuntimeException)
{
    setBinaryStream(parameterIndex,x,length );
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::setBinaryStream( sal_Int32 parameterIndex, const Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length ) throw(SQLException, RuntimeException)
{
    if(!x.is())
        ::dbtools::throwFunctionSequenceException(*this);

    Sequence<sal_Int8> aSeq;
    x->readSomeBytes(aSeq,length);
    setParameter(parameterIndex,aSeq);
}
// -------------------------------------------------------------------------

void SAL_CALL OPreparedStatement::clearParameters(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OStatement_BASE::rBHelper.bDisposed);

    m_aParameterRow->clear();
    m_aParameterRow->push_back(new ORowSetValueDecorator(sal_Int32(0)) );
}
// -------------------------------------------------------------------------
OResultSet* OPreparedStatement::createResultSet()
{
    return new OResultSet(this,m_aSQLIterator);
}
// -----------------------------------------------------------------------------
Reference<XResultSet> OPreparedStatement::initResultSet()
{
    m_pResultSet->clear();
    Reference<XResultSet> xRs(m_pResultSet);

    // check if we got enough paramters
    if ( (m_aParameterRow.isValid() && ( m_aParameterRow->size() -1 ) < m_xParamColumns->size()) ||
         (m_xParamColumns.isValid() && !m_aParameterRow.isValid() && !m_aParameterRow->empty()) )
        throw SQLException(::rtl::OUString::createFromAscii("Invalid count of parameters supplied!")
                            ,*this
                            ,::rtl::OUString::createFromAscii("S1000")
                            ,1000
                            ,Any());

    m_pResultSet->OpenImpl();
    m_pResultSet->setMetaData(getMetaData());

    return xRs;
}
// -----------------------------------------------------------------------------
void SAL_CALL OPreparedStatement::acquire() throw()
{
    OStatement_BASE2::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL OPreparedStatement::release() throw()
{
    OStatement_BASE2::release();
}
// -----------------------------------------------------------------------------
void OPreparedStatement::checkAndResizeParameters(sal_Int32 parameterIndex)
{
    ::connectivity::checkDisposed(OStatement_BASE::rBHelper.bDisposed);
    if ( m_aAssignValues.isValid() && (parameterIndex < 1 || parameterIndex >= static_cast<sal_Int32>(m_aParameterIndexes.size())) )
        throwInvalidIndexException(*this);
    else if ( static_cast<sal_Int32>((*m_aParameterRow).size()) <= parameterIndex )
    {
        sal_Int32 i = m_aParameterRow->size();
        (*m_aParameterRow).resize(parameterIndex+1);
        for ( ;i <= parameterIndex+1; ++i )
        {
            if ( !(*m_aParameterRow)[i].isValid() )
                (*m_aParameterRow)[i] = new ORowSetValueDecorator;
        }
    }
}
// -----------------------------------------------------------------------------
void OPreparedStatement::setParameter(sal_Int32 parameterIndex, const ORowSetValue& x)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkAndResizeParameters(parameterIndex);

    if(m_aAssignValues.isValid())
        *(*m_aAssignValues)[m_aParameterIndexes[parameterIndex]] = x;
    else
        *((*m_aParameterRow)[parameterIndex]) = x;
}
// -----------------------------------------------------------------------------
UINT32 OPreparedStatement::AddParameter(OSQLParseNode * pParameter, const Reference<XPropertySet>& _xCol)
{
    OSL_UNUSED( pParameter );
    OSL_ENSURE(SQL_ISRULE(pParameter,parameter),"OResultSet::AddParameter: Argument ist kein Parameter");
    OSL_ENSURE(pParameter->count() > 0,"OResultSet: Fehler im Parse Tree");
#if OSL_DEBUG_LEVEL > 0
    OSQLParseNode * pMark = pParameter->getChild(0);
    OSL_UNUSED( pMark );
#endif

    ::rtl::OUString sParameterName;
    // Parameter-Column aufsetzen:
    sal_Int32 eType = DataType::VARCHAR;
    UINT32 nPrecision = 255;
    sal_Int32 nScale = 0;
    sal_Int32 nNullable = ColumnValue::NULLABLE;

    if (_xCol.is())
    {
        // Typ, Precision, Scale ... der angegebenen Column verwenden,
        // denn dieser Column wird der Wert zugewiesen bzw. mit dieser
        // Column wird der Wert verglichen.
        _xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))         >>= eType;
        _xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION))    >>= nPrecision;
        _xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))        >>= nScale;
        _xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISNULLABLE))   >>= nNullable;
        _xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))         >>= sParameterName;
    }

    Reference<XPropertySet> xParaColumn = new connectivity::parse::OParseColumn(sParameterName
                                                    ,::rtl::OUString()
                                                    ,::rtl::OUString()
                                                    ,nNullable
                                                    ,nPrecision
                                                    ,nScale
                                                    ,eType
                                                    ,sal_False
                                                    ,sal_False
                                                    ,m_aSQLIterator.isCaseSensitive());
    m_xParamColumns->push_back(xParaColumn);
    return m_xParamColumns->size();
}
// -----------------------------------------------------------------------------
void OPreparedStatement::describeColumn(OSQLParseNode* _pParameter,OSQLParseNode* _pNode,const OSQLTable& _xTable)
{
    Reference<XPropertySet> xProp;
    if(SQL_ISRULE(_pNode,column_ref))
    {
        ::rtl::OUString sColumnName,sTableRange;
        m_aSQLIterator.getColumnRange(_pNode,sColumnName,sTableRange);
        if ( sColumnName.getLength() )
        {
            Reference<XNameAccess> xNameAccess = _xTable->getColumns();
            if(xNameAccess->hasByName(sColumnName))
                xNameAccess->getByName(sColumnName) >>= xProp;
            AddParameter(_pParameter,xProp);
        }
    }
    //  else
        //  AddParameter(_pParameter,xProp);
}
// -------------------------------------------------------------------------
void OPreparedStatement::describeParameter()
{
    ::std::vector< OSQLParseNode*> aParseNodes;
    scanParameter(m_pParseTree,aParseNodes);
    if ( !aParseNodes.empty() )
    {
        //  m_xParamColumns = new OSQLColumns();
        const OSQLTables& xTabs = m_aSQLIterator.getTables();
        if( !xTabs.empty() )
        {
            OSQLTable xTable = xTabs.begin()->second;
            ::std::vector< OSQLParseNode*>::const_iterator aIter = aParseNodes.begin();
            for (;aIter != aParseNodes.end();++aIter )
            {
                describeColumn(*aIter,(*aIter)->getParent()->getChild(0),xTable);
            }
        }
    }
}
// -----------------------------------------------------------------------------
void OPreparedStatement::initializeResultSet(OResultSet* _pResult)
{
    OStatement_Base::initializeResultSet(_pResult);

    m_pResultSet->setParameterColumns(m_xParamColumns);
    m_pResultSet->setParameterRow(m_aParameterRow);

    // Parameter substituieren (AssignValues und Kriterien):
    if (!m_xParamColumns->empty())
    {
        // Zunaechst AssignValues
        USHORT nParaCount=0; // gibt die aktuelle Anzahl der bisher gesetzen Parameter an

        // Nach zu substituierenden Parametern suchen:
        size_t nCount = m_aAssignValues.isValid() ? m_aAssignValues->size() : 1; // 1 ist wichtig fuer die Kriterien
        for (size_t j = 1; j < nCount; j++)
        {
            UINT32 nParameter = (*m_aAssignValues).getParameterIndex(j);
            if (nParameter == SQL_NO_PARAMETER)
                continue;   // dieser AssignValue ist kein Parameter

            ++nParaCount; // ab hier ist der Parameter gueltig
            // Parameter ersetzen. Wenn Parameter nicht verfuegbar,
            //  Value auf NULL setzen.
            //  (*m_aAssignValues)[j] = (*m_aParameterRow)[(UINT16)nParameter];
        }

        if (m_aParameterRow.isValid() &&  (m_xParamColumns->size()+1) != m_aParameterRow->size() )
        {
            sal_Int32 i = m_aParameterRow->size();
            sal_Int32 nParamColumns = m_xParamColumns->size()+1;
            m_aParameterRow->resize(nParamColumns);
            for ( ;i < nParamColumns; ++i )
            {
                if ( !(*m_aParameterRow)[i].isValid() )
                    (*m_aParameterRow)[i] = new ORowSetValueDecorator;
            }
            //m_aParameterRow->resize(m_xParamColumns->size()+1);
        }
        if (m_aParameterRow.isValid() && nParaCount < m_aParameterRow->size() )
        {

            m_pSQLAnalyzer->bindParameterRow(m_aParameterRow);
        }
    }
}
// -----------------------------------------------------------------------------
void OPreparedStatement::parseParamterElem(const String& _sColumnName,OSQLParseNode* pRow_Value_Constructor_Elem)
{
    Reference<XPropertySet> xCol;
    m_xColNames->getByName(_sColumnName) >>= xCol;
    sal_Int32 nParameter = -1;
    if(m_xParamColumns.isValid())
    {
        OSQLColumns::const_iterator aIter = find(m_xParamColumns->begin(),m_xParamColumns->end(),_sColumnName,::comphelper::UStringMixEqual(m_pTable->isCaseSensitive()));
        if(aIter != m_xParamColumns->end())
            nParameter = m_xParamColumns->size() - (m_xParamColumns->end() - aIter) + 1;// +1 because the rows start at 1
    }
    if(nParameter == -1)
        nParameter = AddParameter(pRow_Value_Constructor_Elem,xCol);
    // Nr. des Parameters in der Variablen merken:
    SetAssignValue(_sColumnName, String(), TRUE, nParameter);
}
// -----------------------------------------------------------------------------


