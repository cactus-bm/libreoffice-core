/*************************************************************************
 *
 *  $RCSfile: OConnection.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: oj $ $Date: 2001-10-12 11:48:07 $
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
#ifndef _CONNECTIVITY_OTOOLS_HXX_
#include "odbc/OTools.hxx"
#endif
#ifndef _CONNECTIVITY_ODBC_OCONNECTION_HXX_
#include "odbc/OConnection.hxx"
#endif
#ifndef _CONNECTIVITY_ODBC_ODATABASEMETADATA_HXX_
#include "odbc/ODatabaseMetaData.hxx"
#endif
#ifndef _CONNECTIVITY_RESOURCE_HRC_
#include "Resource.hrc"
#endif
#ifndef _CONNECTIVITY_MODULECONTEXT_HXX_
#include "ModuleContext.hxx"
#endif
#ifndef _CONNECTIVITY_ODBC_OFUNCTIONS_HXX_
#include "odbc/OFunctions.hxx"
#endif
#ifndef _CONNECTIVITY_ODBC_ODRIVER_HXX_
#include "odbc/ODriver.hxx"
#endif
#ifndef _CONNECTIVITY_ODBC_OSTATEMENT_HXX_
#include "odbc/OStatement.hxx"
#endif
#ifndef _CONNECTIVITY_ODBC_OPREPAREDSTATEMENT_HXX_
#include "odbc/OPreparedStatement.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_COLUMNVALUE_HPP_
#include <com/sun/star/sdbc/ColumnValue.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _DBHELPER_DBCHARSET_HXX_
#include <connectivity/dbcharset.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif

using namespace connectivity::odbc;
using namespace connectivity;
using namespace dbtools;

//------------------------------------------------------------------------------
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
// --------------------------------------------------------------------------------
OConnection::OConnection(const SQLHANDLE _pDriverHandle,ODBCDriver* _pDriver)
                         : OSubComponent<OConnection, OConnection_BASE>((::cppu::OWeakObject*)_pDriver, this),
                         m_pDriverHandleCopy(_pDriverHandle),
                         m_pDriver(_pDriver),
                         m_bClosed(sal_True),
                         m_xMetaData(NULL),
                         m_bUseCatalog(sal_False),
                         m_bUseOldDateFormat(sal_False),
                         m_nStatementCount(0)
{
    m_pDriver->acquire();
    ModuleContext::AddRef();
}
//-----------------------------------------------------------------------------
OConnection::~OConnection()
{
    if(!isClosed(  ))
        close();
    m_pDriver->release();
    m_pDriver = NULL;
    ModuleContext::ReleaseRef();
}
//-----------------------------------------------------------------------------
void SAL_CALL OConnection::release() throw(RuntimeException)
{
    relase_ChildImpl();
}
// -----------------------------------------------------------------------------
void* OConnection::getOdbcFunction(sal_Int32 _nIndex)  const
{
    OSL_ENSURE(m_pDriver,"OConnection::getOdbcFunction: m_pDriver is null!");
    return m_pDriver->getOdbcFunction(_nIndex);
}
//-----------------------------------------------------------------------------
SQLRETURN OConnection::OpenConnection(const ::rtl::OUString& aConnectStr,sal_Int32 nTimeOut, sal_Bool bSilent)
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if (m_aConnectionHandle == SQL_NULL_HANDLE)
        return -1;

    sal_Bool bReadOnly; //weil Methode statisch hier noch einmal ein lokales bReadOnly
    SQLRETURN nSQLRETURN = 0;
    SDB_ODBC_CHAR szConnStrOut[4096];
    SDB_ODBC_CHAR szConnStrIn[2048];
    SWORD cbConnStrOut;
    memset(szConnStrOut,'\0',4096);
    memset(szConnStrIn,'\0',2048);
    ::rtl::OString aConStr(::rtl::OUStringToOString(aConnectStr,getTextEncoding()));
    memcpy(szConnStrIn, (SDB_ODBC_CHAR*) aConStr.getStr(), ::std::min<sal_Int32>((sal_Int32)2048,aConStr.getLength()));


    N3SQLSetConnectAttr(m_aConnectionHandle,SQL_ATTR_LOGIN_TIMEOUT,(SQLPOINTER)nTimeOut,SQL_IS_INTEGER);
    // Verbindung aufbauen

#ifdef LINUX

    nSQLRETURN = N3SQLDriverConnect(m_aConnectionHandle,
                      NULL,
                      szConnStrIn,
                      (SQLSMALLINT) ::std::min((sal_Int32)2048,aConStr.getLength()),
                      szConnStrOut,
                      (SQLSMALLINT) sizeof szConnStrOut,
                      &cbConnStrOut,
                      SQL_DRIVER_NOPROMPT);
    if (nSQLRETURN == SQL_ERROR || nSQLRETURN == SQL_NO_DATA || SQL_SUCCESS_WITH_INFO == nSQLRETURN)
        return nSQLRETURN;
#else

    SQLUSMALLINT nSilent =  bSilent ? SQL_DRIVER_NOPROMPT : SQL_DRIVER_COMPLETE;
    nSQLRETURN = N3SQLDriverConnect(m_aConnectionHandle,
                      NULL,
                      szConnStrIn,
                      (SQLSMALLINT) ::std::min<sal_Int32>((sal_Int32)2048,aConStr.getLength()),
                      szConnStrOut,
                      (SQLSMALLINT) sizeof szConnStrOut,
                      &cbConnStrOut,
                      nSilent);
    if (nSQLRETURN == SQL_ERROR || nSQLRETURN == SQL_NO_DATA)
        return nSQLRETURN;

    m_bClosed = sal_False;

#endif //LINUX

    try
    {
        ::rtl::OUString aVal;
        OTools::GetInfo(this,m_aConnectionHandle,SQL_DATA_SOURCE_READ_ONLY,aVal,*this,getTextEncoding());
        bReadOnly = !aVal.compareToAscii("Y");
    }
    catch(...)
    {
        bReadOnly = sal_True;
    }

#ifndef MAC
    // autocoomit ist immer default

    if (!bReadOnly)
        N3SQLSetConnectAttr(m_aConnectionHandle,SQL_ATTR_AUTOCOMMIT,(SQLPOINTER)SQL_AUTOCOMMIT_ON,SQL_IS_INTEGER);
#endif

    return nSQLRETURN;
}
//-----------------------------------------------------------------------------
SQLRETURN OConnection::Construct(const ::rtl::OUString& url,const Sequence< PropertyValue >& info)  throw(SQLException)
{
    m_aConnectionHandle  = SQL_NULL_HANDLE;
    m_aURL  = url;
    m_aInfo = info;

    // Connection allozieren
    N3SQLAllocHandle(SQL_HANDLE_DBC,m_pDriverHandleCopy,&m_aConnectionHandle);
    if(m_aConnectionHandle == SQL_NULL_HANDLE)
        throw SQLException();

    sal_Int32 nLen = url.indexOf(':');
    nLen = url.indexOf(':',nLen+1);
    ::rtl::OUString aDSN(RTL_CONSTASCII_USTRINGPARAM("DSN=")), aUID, aPWD, aSysDrvSettings;
    aDSN += url.copy(nLen+1);

    const char* pUser       = "user";
    const char* pTimeout    = "Timeout";
    const char* pSilent     = "Silent";
    const char* pPwd        = "password";
    const char* pUseCatalog = "UseCatalog";
    const char* pSysDrv     = "SystemDriverSettings";
    const char* pCharSet    = "CharSet";

    sal_Int32 nTimeout = 20;
    sal_Bool bSilent = sal_True;
    const PropertyValue *pBegin = info.getConstArray();
    const PropertyValue *pEnd   = pBegin + info.getLength();
    for(;pBegin != pEnd;++pBegin)
    {
        if(!pBegin->Name.compareToAscii(pTimeout))
            pBegin->Value >>= nTimeout;
        else if(!pBegin->Name.compareToAscii(pSilent))
            pBegin->Value >>= bSilent;
        else if(!pBegin->Name.compareToAscii(pUser))
        {
            pBegin->Value >>= aUID;
            aDSN = aDSN + ::rtl::OUString::createFromAscii(";UID=") + aUID;
        }
        else if(!pBegin->Name.compareToAscii(pPwd))
        {
            pBegin->Value >>= aPWD;
            aDSN = aDSN + ::rtl::OUString::createFromAscii(";PWD=") + aPWD;
        }
        else if(!pBegin->Name.compareToAscii(pUseCatalog))
        {
            m_bUseCatalog = ::cppu::any2bool(pBegin->Value);
        }
        else if(!pBegin->Name.compareToAscii(pSysDrv))
        {
            pBegin->Value >>= aSysDrvSettings;
            aDSN += ::rtl::OUString::createFromAscii(";");
            aDSN += aSysDrvSettings;
        }
        else if(0 == pBegin->Name.compareToAscii(pCharSet))
        {
            ::rtl::OUString sIanaName;
            pBegin->Value >>= sIanaName;

            ::dbtools::OCharsetMap aLookupIanaName;
            ::dbtools::OCharsetMap::const_iterator aLookup = aLookupIanaName.find(sIanaName, ::dbtools::OCharsetMap::IANA());
            if (aLookup != aLookupIanaName.end())
                m_nTextEncoding = (*aLookup).getEncoding();
            else
                m_nTextEncoding = RTL_TEXTENCODING_DONTKNOW;
            if(m_nTextEncoding == RTL_TEXTENCODING_DONTKNOW)
                m_nTextEncoding = osl_getThreadTextEncoding();

        }
    }
    m_sUser = aUID;

    SQLRETURN nSQLRETURN = OpenConnection(aDSN,nTimeout, bSilent);
    if (nSQLRETURN == SQL_ERROR || nSQLRETURN == SQL_NO_DATA)
    {
        OTools::ThrowException(this,nSQLRETURN,m_aConnectionHandle,SQL_HANDLE_DBC,*this);
    }
    else if(SQL_SUCCESS_WITH_INFO == nSQLRETURN) // this driver does not support odbc3
    {
    }
    return nSQLRETURN;
}
// XServiceInfo
// --------------------------------------------------------------------------------
IMPLEMENT_SERVICE_INFO(OConnection, "com.sun.star.sdbc.drivers.odbc.OConnection", "com.sun.star.sdbc.Connection")

// --------------------------------------------------------------------------------
Reference< XStatement > SAL_CALL OConnection::createStatement(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);

    Reference< XStatement > xReturn = new OStatement(this);
    m_aStatements.push_back(WeakReferenceHelper(xReturn));
    return xReturn;
}
// --------------------------------------------------------------------------------
Reference< XPreparedStatement > SAL_CALL OConnection::prepareStatement( const ::rtl::OUString& sql ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);

    if(m_aTypeInfo.empty())
        buildTypeInfo();

    Reference< XPreparedStatement > xReturn = new OPreparedStatement(this,m_aTypeInfo,sql);
    m_aStatements.push_back(WeakReferenceHelper(xReturn));
    return xReturn;
}
// --------------------------------------------------------------------------------
Reference< XPreparedStatement > SAL_CALL OConnection::prepareCall( const ::rtl::OUString& sql ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);

    return NULL;
}
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OConnection::nativeSQL( const ::rtl::OUString& sql ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );

    ::rtl::OString aSql(::rtl::OUStringToOString(sql.getStr(),getTextEncoding()));
    char pOut[2048];
    SQLINTEGER nOutLen;
    OTools::ThrowException(this,N3SQLNativeSql(m_aConnectionHandle,(SDB_ODBC_CHAR*)aSql.getStr(),aSql.getLength(),(SDB_ODBC_CHAR*)pOut,2048,&nOutLen),m_aConnectionHandle,SQL_HANDLE_DBC,*this);
    return ::rtl::OUString(pOut,nOutLen,getTextEncoding());
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setAutoCommit( sal_Bool autoCommit ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    OTools::ThrowException(this,N3SQLSetConnectAttr(m_aConnectionHandle,
                                   SQL_ATTR_AUTOCOMMIT,
                                   (SQLPOINTER)((autoCommit) ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF) ,SQL_IS_INTEGER),
                                   m_aConnectionHandle,SQL_HANDLE_DBC,*this);
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL OConnection::getAutoCommit(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    sal_uInt32 nOption = 0;
    OTools::ThrowException(this,N3SQLGetConnectAttr(m_aConnectionHandle,
                                   SQL_ATTR_AUTOCOMMIT, &nOption,0,0),m_aConnectionHandle,SQL_HANDLE_DBC,*this);
    return nOption == SQL_AUTOCOMMIT_ON ;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::commit(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    OTools::ThrowException(this,N3SQLEndTran(SQL_HANDLE_DBC,m_aConnectionHandle,SQL_COMMIT),m_aConnectionHandle,SQL_HANDLE_DBC,*this);
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::rollback(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    OTools::ThrowException(this,N3SQLEndTran(SQL_HANDLE_DBC,m_aConnectionHandle,SQL_ROLLBACK),m_aConnectionHandle,SQL_HANDLE_DBC,*this);
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL OConnection::isClosed(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );

    return OConnection_BASE::rBHelper.bDisposed;
}
// --------------------------------------------------------------------------------
Reference< XDatabaseMetaData > SAL_CALL OConnection::getMetaData(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    Reference< XDatabaseMetaData > xMetaData = m_xMetaData;
    if(!xMetaData.is())
    {
        xMetaData = new ODatabaseMetaData(m_aConnectionHandle,this);
        m_xMetaData = xMetaData;
    }

    return xMetaData;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setReadOnly( sal_Bool readOnly ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    OTools::ThrowException(this,
        N3SQLSetConnectAttr(m_aConnectionHandle,SQL_ATTR_ACCESS_MODE,(SQLPOINTER)readOnly,SQL_IS_INTEGER),
        m_aConnectionHandle,SQL_HANDLE_DBC,*this);
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL OConnection::isReadOnly(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    ::rtl::OUString aValue;
    OTools::GetInfo(this,m_aConnectionHandle,SQL_DATA_SOURCE_READ_ONLY,aValue,*this,getTextEncoding());
    return !aValue.compareToAscii("Y");
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setCatalog( const ::rtl::OUString& catalog ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    ::rtl::OString aCat(::rtl::OUStringToOString(catalog.getStr(),getTextEncoding()));
    OTools::ThrowException(this,
        N3SQLSetConnectAttr(m_aConnectionHandle,SQL_ATTR_CURRENT_CATALOG,(SDB_ODBC_CHAR*)aCat.getStr(),SQL_NTS),
        m_aConnectionHandle,SQL_HANDLE_DBC,*this);
}
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OConnection::getCatalog(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    sal_Int32 nValueLen;
    char pCat[1024];
    OTools::ThrowException(this,
        N3SQLGetConnectAttr(m_aConnectionHandle,SQL_ATTR_CURRENT_CATALOG,(SDB_ODBC_CHAR*)pCat,1024,&nValueLen),
        m_aConnectionHandle,SQL_HANDLE_DBC,*this);

    return ::rtl::OUString(pCat,nValueLen,getTextEncoding());
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setTransactionIsolation( sal_Int32 level ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    OTools::ThrowException(this,N3SQLSetConnectAttr(m_aConnectionHandle,
                                   SQL_ATTR_TXN_ISOLATION,
                                   (SQLPOINTER)level,SQL_IS_INTEGER),
                                   m_aConnectionHandle,SQL_HANDLE_DBC,*this);
}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL OConnection::getTransactionIsolation(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    sal_Int32 nTxn = 0;
    SQLINTEGER nValueLen;
    OTools::ThrowException(this,
        N3SQLGetConnectAttr(m_aConnectionHandle,SQL_ATTR_TXN_ISOLATION,&nTxn,sizeof nTxn,&nValueLen),
        m_aConnectionHandle,SQL_HANDLE_DBC,*this);
    return nTxn;
}
// --------------------------------------------------------------------------------
Reference< ::com::sun::star::container::XNameAccess > SAL_CALL OConnection::getTypeMap(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    return NULL;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setTypeMap( const Reference< ::com::sun::star::container::XNameAccess >& typeMap ) throw(SQLException, RuntimeException)
{
}
// --------------------------------------------------------------------------------
// XCloseable
void SAL_CALL OConnection::close(  ) throw(SQLException, RuntimeException)
{
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        checkDisposed(OConnection_BASE::rBHelper.bDisposed);

    }
    dispose();
}
// --------------------------------------------------------------------------------
// XWarningsSupplier
Any SAL_CALL OConnection::getWarnings(  ) throw(SQLException, RuntimeException)
{
    return Any();
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::clearWarnings(  ) throw(SQLException, RuntimeException)
{
}
//--------------------------------------------------------------------
void OConnection::buildTypeInfo() throw( SQLException)
{
    ::osl::MutexGuard aGuard( m_aMutex );

    Reference< XResultSet> xRs = getMetaData ()->getTypeInfo ();
    if(xRs.is())
    {
        Reference< XRow> xRow(xRs,UNO_QUERY);
        // Information for a single SQL type

        // Loop on the result set until we reach end of file

        while (xRs->next ())
        {
            OTypeInfo aInfo;
            aInfo.aTypeName         = xRow->getString   (1);
            aInfo.nType             = xRow->getShort    (2);
            aInfo.nPrecision        = xRow->getInt      (3);
            aInfo.aLiteralPrefix    = xRow->getString   (4);
            aInfo.aLiteralSuffix    = xRow->getString   (5);
            aInfo.aCreateParams     = xRow->getString   (6);
            aInfo.bNullable         = xRow->getBoolean  (7) == ColumnValue::NULLABLE;
            aInfo.bCaseSensitive    = xRow->getBoolean  (8);
            aInfo.nSearchType       = xRow->getShort    (9);
            aInfo.bUnsigned         = xRow->getBoolean  (10);
            aInfo.bCurrency         = xRow->getBoolean  (11);
            aInfo.bAutoIncrement    = xRow->getBoolean  (12);
            aInfo.aLocalTypeName    = xRow->getString   (13);
            aInfo.nMinimumScale     = xRow->getShort    (14);
            aInfo.nMaximumScale     = xRow->getShort    (15);
            aInfo.nNumPrecRadix     = (sal_Int16)xRow->getInt(18);



            // Now that we have the type info, save it
            // in the Hashtable if we don't already have an
            // entry for this SQL type.

            m_aTypeInfo.push_back(aInfo);
        }

        // Close the result set/statement.

        Reference< XCloseable> xClose(xRs,UNO_QUERY);
        if(xClose.is())
            xClose->close();
    }
}
//------------------------------------------------------------------------------
void OConnection::disposing()
{
    ::osl::MutexGuard aGuard(m_aMutex);


    //  m_aTables.disposing();
    for (OWeakRefArray::iterator i = m_aStatements.begin(); m_aStatements.end() != i; ++i)
    {
        Reference< XComponent > xComp(i->get(), UNO_QUERY);
        if (xComp.is())
            xComp->dispose();
    }
    m_aStatements.clear();

    for (::std::map< SQLHANDLE,OConnection*>::iterator aConIter = m_aConnections.begin();aConIter != m_aConnections.end();++aConIter )
        aConIter->second->dispose();

    ::std::map< SQLHANDLE,OConnection*>().swap(m_aConnections);

    if(!m_bClosed)
        N3SQLDisconnect(m_aConnectionHandle);
    m_bClosed   = sal_True;
    m_xMetaData = ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XDatabaseMetaData>();

    dispose_ChildImpl();
    OConnection_BASE::disposing();
}
// -----------------------------------------------------------------------------
OConnection* OConnection::cloneConnection()
{
    return new OConnection(m_pDriverHandleCopy,m_pDriver);
}
// -----------------------------------------------------------------------------
SQLHANDLE OConnection::createStatementHandle()
{
    OConnection* pConnectionTemp = this;
    sal_Bool bNew = sal_False;
    try
    {
        sal_Int32 nMaxStatements = getMetaData()->getMaxStatements();
        if(nMaxStatements && nMaxStatements <= m_nStatementCount)
        {
            OConnection* pConnection = cloneConnection();
            pConnection->acquire();
            pConnection->Construct(m_aURL,m_aInfo);
            pConnectionTemp = pConnection;
            bNew = sal_True;
        }
    }
    catch(SQLException&)
    {
    }

    SQLHANDLE aStatementHandle = SQL_NULL_HANDLE;
    SQLRETURN nRetcode = N3SQLAllocHandle(SQL_HANDLE_STMT,pConnectionTemp->getConnection(),&aStatementHandle);
    ++m_nStatementCount;
    if(bNew)
        m_aConnections.insert(::std::map< SQLHANDLE,OConnection*>::value_type(aStatementHandle,pConnectionTemp));

    return aStatementHandle;

}
// -----------------------------------------------------------------------------
void OConnection::freeStatementHandle(SQLHANDLE& _pHandle)
{
    ::std::map< SQLHANDLE,OConnection*>::iterator aFind = m_aConnections.find(_pHandle);

    N3SQLFreeStmt(_pHandle,SQL_RESET_PARAMS);
    N3SQLFreeStmt(_pHandle,SQL_UNBIND);
    N3SQLFreeStmt(_pHandle,SQL_CLOSE);
    N3SQLFreeHandle(SQL_HANDLE_STMT,_pHandle);

    _pHandle = SQL_NULL_HANDLE;

    if(aFind != m_aConnections.end())
    {
        aFind->second->dispose();
        m_aConnections.erase(aFind);
    }
    --m_nStatementCount;
}
// -----------------------------------------------------------------------------



