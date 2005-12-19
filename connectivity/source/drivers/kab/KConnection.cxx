/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: KConnection.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2005-12-19 16:48:35 $
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

#include "KConnection.hxx"
#ifndef _CONNECTIVITY_KAB_DATABASEMETADATA_HXX_
#include "KDatabaseMetaData.hxx"
#endif
#ifndef _CONNECTIVITY_KAB_STATEMENT_HXX_
#include "KStatement.hxx"
#endif
#ifndef _CONNECTIVITY_KAB_PREPAREDSTATEMENT_HXX_
#include "KPreparedStatement.hxx"
#endif
#ifndef _CONNECTIVITY_KAB_DRIVER_HXX_
#include "KDriver.hxx"
#endif
#ifndef _CONNECTIVITY_KAB_CATALOG_HXX_
#include "KCatalog.hxx"
#endif

#ifndef _COM_SUN_STAR_SDBC_COLUMNVALUE_HPP_
#include <com/sun/star/sdbc/ColumnValue.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_TRANSACTIONISOLATION_HPP_
#include <com/sun/star/sdbc/TransactionIsolation.hpp>
#endif

using namespace connectivity::kab;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;

IMPLEMENT_SERVICE_INFO(KabConnection, "com.sun.star.sdbc.drivers.KabConnection", "com.sun.star.sdbc.Connection")
//-----------------------------------------------------------------------------
KabConnection::KabConnection(KabDriver* _pDriver)
         : OSubComponent<KabConnection, KabConnection_BASE>((::cppu::OWeakObject*)_pDriver, this),
         OMetaConnection_BASE(m_aMutex),
         m_pAddressBook(NULL),
         m_pDriver(_pDriver),
         m_xMetaData(NULL)
{
    m_pDriver->acquire();
}
//-----------------------------------------------------------------------------
KabConnection::~KabConnection()
{
    if (!isClosed())
        close();

    m_pDriver->release();
    m_pDriver = NULL;
}
//-----------------------------------------------------------------------------
void SAL_CALL KabConnection::release() throw()
{
    relase_ChildImpl();
}
// -----------------------------------------------------------------------------
void KabConnection::construct(const ::rtl::OUString& url, const Sequence< PropertyValue >& info) throw(SQLException)
{
    osl_incrementInterlockedCount( &m_refCount );

    // create a KDE address book object
    m_pAddressBook = KABC::StdAddressBook::self();
    m_pAddressBook->setAutomaticSave(false);
// perharps we should analyze the URL to know whether the addressbook is local, over LDAP, etc...
// perharps we should get some user and password information from "info" properties

    osl_decrementInterlockedCount( &m_refCount );
}
// XServiceInfo
// --------------------------------------------------------------------------------
Reference< XStatement > SAL_CALL KabConnection::createStatement(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // create a statement
    // the statement can only be executed once
    Reference< XStatement > xReturn = new KabStatement(this);
    m_aStatements.push_back(WeakReferenceHelper(xReturn));
    return xReturn;
}
// --------------------------------------------------------------------------------
Reference< XPreparedStatement > SAL_CALL KabConnection::prepareStatement( const ::rtl::OUString& _sSql ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // create a statement
    // the statement can only be executed more than once
    Reference< XPreparedStatement > xReturn = new KabPreparedStatement(this, _sSql);
    m_aStatements.push_back(WeakReferenceHelper(xReturn));
    return xReturn;
}
// --------------------------------------------------------------------------------
Reference< XPreparedStatement > SAL_CALL KabConnection::prepareCall( const ::rtl::OUString& _sSql ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // not implemented yet :-) a task to do
    return NULL;
}
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL KabConnection::nativeSQL( const ::rtl::OUString& _sSql ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    // when you need to transform SQL92 to you driver specific you can do it here

    return _sSql;
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::setAutoCommit( sal_Bool autoCommit ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);
    // here you  have to set your commit mode please have a look at the jdbc documentation to get a clear explanation
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL KabConnection::getAutoCommit(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);
    // you have to distinguish which if you are in autocommit mode or not
    // at normal case true should be fine here

    return sal_True;
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::commit(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // when you database does support transactions you should commit here
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::rollback(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // same as commit but for the other case
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL KabConnection::isClosed(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );

    // just simple -> we are close when we are disposed taht means someone called dispose(); (XComponent)
    return KabConnection_BASE::rBHelper.bDisposed;
}
// --------------------------------------------------------------------------------
Reference< XDatabaseMetaData > SAL_CALL KabConnection::getMetaData(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // here we have to create the class with biggest interface
    // The answer is 42 :-)
    Reference< XDatabaseMetaData > xMetaData = m_xMetaData;
    if (!xMetaData.is())
    {
        xMetaData = new KabDatabaseMetaData(this); // need the connection because it can return it
        m_xMetaData = xMetaData;
    }

    return xMetaData;
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::setReadOnly( sal_Bool readOnly ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // set you connection to readonly
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL KabConnection::isReadOnly(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // return if your connection to readonly
    return sal_False;
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::setCatalog( const ::rtl::OUString& catalog ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // if your database doesn't work with catalogs you go to next method otherwise you kjnow what to do
}
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL KabConnection::getCatalog(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);


    // return your current catalog
    return ::rtl::OUString();
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::setTransactionIsolation( sal_Int32 level ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // set your isolation level
    // please have a look at @see com.sun.star.sdbc.TransactionIsolation
}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL KabConnection::getTransactionIsolation(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);


    // please have a look at @see com.sun.star.sdbc.TransactionIsolation
    return TransactionIsolation::NONE;
}
// --------------------------------------------------------------------------------
Reference< ::com::sun::star::container::XNameAccess > SAL_CALL KabConnection::getTypeMap(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(KabConnection_BASE::rBHelper.bDisposed);

    // if your driver has special database types you can return it here

    return NULL;
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::setTypeMap( const Reference< ::com::sun::star::container::XNameAccess >& typeMap ) throw(SQLException, RuntimeException)
{
    // the other way around
}
// --------------------------------------------------------------------------------
// XCloseable
void SAL_CALL KabConnection::close(  ) throw(SQLException, RuntimeException)
{
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        checkDisposed(KabConnection_BASE::rBHelper.bDisposed);
    }
    dispose();
}
// --------------------------------------------------------------------------------
// XWarningsSupplier
Any SAL_CALL KabConnection::getWarnings(  ) throw(SQLException, RuntimeException)
{
    // when you collected some warnings -> return it
    return Any();
}
// --------------------------------------------------------------------------------
void SAL_CALL KabConnection::clearWarnings(  ) throw(SQLException, RuntimeException)
{
    // you should clear your collected warnings here
}
//------------------------------------------------------------------------------
void KabConnection::disposing()
{
    // we noticed that we should be destroied in near future so we have to dispose our statements
    ::osl::MutexGuard aGuard(m_aMutex);

    for (OWeakRefArray::iterator i = m_aStatements.begin(); m_aStatements.end() != i; ++i)
    {
        Reference< XComponent > xComp(i->get(), UNO_QUERY);
        if (xComp.is())
            xComp->dispose();
    }
    m_aStatements.clear();

    if (m_pAddressBook != NULL)
    {
        m_pAddressBook->close();
        m_pAddressBook = NULL;
    }

    m_xMetaData = ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XDatabaseMetaData>();

    dispose_ChildImpl();
    KabConnection_BASE::disposing();
}
// -----------------------------------------------------------------------------
Reference< XTablesSupplier > SAL_CALL KabConnection::createCatalog()
{
    ::osl::MutexGuard aGuard( m_aMutex );

    Reference< XTablesSupplier > xTab = m_xCatalog;
    if (!m_xCatalog.is())
    {
        KabCatalog *pCat = new KabCatalog(this);
        xTab = pCat;
        m_xCatalog = xTab;
    }
    return xTab;
}
