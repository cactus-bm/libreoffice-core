/*************************************************************************
 *
 *  $RCSfile: HDriver.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-24 14:39:27 $
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
 *  Source License Version 1.1 (the License); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an AS IS basis,
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
#ifndef CONNECTIVITY_HSQLDB_DRIVER_HXX
#include "hsqldb/HDriver.hxx"
#endif
#include "hsqldb/HConnection.hxx"

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include "connectivity/dbexception.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XDRIVERACCESS_HPP_
#include <com/sun/star/sdbc/XDriverAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XTRANSACTIONBROADCASTER_HPP_
#include <com/sun/star/embed/XTransactionBroadcaster.hpp>
#endif
#ifndef CONNECTIVITY_CONNECTION_HXX
#include "TConnection.hxx"
#endif
#include "hsqldb/HStorageMap.hxx"
#ifndef INCLUDED_JVMFWK_FRAMEWORK_H
#include <jvmfwk/framework.h>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_XPROXYFACTORY_HPP_
#include <com/sun/star/reflection/XProxyFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif
#ifndef CONNECTIVITY_HSQLDB_TERMINATELISTENER_HXX
#include "HTerminateListener.hxx"
#endif

#ifndef _OSL_FILE_H_
#include <osl/file.h>
#endif
//........................................................................
namespace connectivity
{
//........................................................................
    using namespace hsqldb;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::embed;
    using namespace ::com::sun::star::reflection;

    namespace hsqldb
    {
        Reference< XInterface >  SAL_CALL ODriverDelegator_CreateInstance(const Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFac) throw( Exception )
        {
            return *(new ODriverDelegator(_rxFac));
        }
    }



    //====================================================================
    //= ODriverDelegator
    //====================================================================
    //--------------------------------------------------------------------
    ODriverDelegator::ODriverDelegator(const Reference< XMultiServiceFactory >& _rxFactory)
        : ODriverDelegator_BASE(m_aMutex)
        ,m_xFactory(_rxFactory)
        ,m_bInShutDownConnections(sal_False)
    {
    }

    //--------------------------------------------------------------------
    ODriverDelegator::~ODriverDelegator()
    {
        try
        {
            ::comphelper::disposeComponent(m_xDriver);
        }
        catch(const Exception&)
        {
        }
    }

    // --------------------------------------------------------------------------------
    void SAL_CALL ODriverDelegator::disposing()
    {
        ::osl::MutexGuard aGuard(m_aMutex);

        try
        {
            for (TWeakPairVector::iterator i = m_aConnections.begin(); m_aConnections.end() != i; ++i)
            {
                Reference<XInterface > xTemp = i->first.get();
                ::comphelper::disposeComponent(xTemp);
            }
        }
        catch(Exception&)
        {
            // not interested in
        }
        m_aConnections.clear();
        TWeakPairVector().swap(m_aConnections);

        cppu::WeakComponentImplHelperBase::disposing();
    }
    //--------------------------------------------------------------------
    Reference< XDriver > ODriverDelegator::loadDriver( )
    {
        if ( !m_xDriver.is() )
        {
            ::rtl::OUString sURL(RTL_CONSTASCII_USTRINGPARAM("jdbc:hsqldb:db"));
            Reference<XDriverAccess> xDriverAccess(m_xFactory->createInstance(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdbc.DriverManager")) ),UNO_QUERY);
            OSL_ENSURE(xDriverAccess.is(),"Could not load driver manager!");
            if ( xDriverAccess.is() )
                m_xDriver = xDriverAccess->getDriverByURL(sURL);
        }

        return m_xDriver;
    }

    //--------------------------------------------------------------------
    Reference< XConnection > SAL_CALL ODriverDelegator::connect( const ::rtl::OUString& url, const Sequence< PropertyValue >& info ) throw (SQLException, RuntimeException)
    {
        Reference< XConnection > xConnection;
        if ( acceptsURL(url) )
        {
            Reference< XDriver > xDriver = loadDriver();
            if ( xDriver.is() )
            {
                ::rtl::OUString sURL;
                Reference<XStorage> xStorage;
                const PropertyValue* pIter = info.getConstArray();
                const PropertyValue* pEnd = pIter + info.getLength();

                for (;pIter != pEnd; ++pIter)
                {
                    if ( pIter->Name.equalsAscii("Storage") )
                    {
                        xStorage.set(pIter->Value,UNO_QUERY);
                    }
                    else if ( pIter->Name.equalsAscii("URL") )
                    {
                        pIter->Value >>= sURL;
                    }
                }

                if ( !xStorage.is() || !sURL.getLength() )
                    throw SQLException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("No storage property was set."))
                        ,*this
                        ,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HY0000"))
                        ,1000,Any());

                ::rtl::OUString sSystemPath;
                osl_getSystemPathFromFileURL( sURL.pData, &sSystemPath.pData );
                sal_Int32 nIndex = sSystemPath.lastIndexOf('.');
                if ( nIndex == -1 )
                    throw SQLException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("File URL is not correct."))
                        ,*this
                        ,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HY0000"))
                        ,1000,Any());

                Sequence< PropertyValue > aConvertedProperties(8);
                sal_Int32 nPos = 0;
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("storage_key"));
                ::rtl::OUString sConnPartURL = sSystemPath.copy(0,nIndex);
                ::rtl::OUString sKey = StorageContainer::registerStorage(xStorage,sConnPartURL);
                aConvertedProperties[nPos++].Value <<= sKey;
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("storage_class_name"));
                aConvertedProperties[nPos++].Value <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdbcx.comp.hsqldb.StorageAccess"));
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("fileaccess_class_name"));
                aConvertedProperties[nPos++].Value <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdbcx.comp.hsqldb.StorageFileAccess"));
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("JavaDriverClass"));
                aConvertedProperties[nPos++].Value <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("org.hsqldb.jdbcDriver"));
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("IsAutoRetrievingEnabled"));
                aConvertedProperties[nPos++].Value <<= sal_True;
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AutoRetrievingStatement"));
                aConvertedProperties[nPos++].Value <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CALL IDENTITY()"));
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ParameterNameSubstitution"));
                aConvertedProperties[nPos++].Value <<= sal_True;
                aConvertedProperties[nPos].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("IgnoreDriverPrivileges"));
                aConvertedProperties[nPos++].Value <<= sal_True;

                ::rtl::OUString sConnectURL(RTL_CONSTASCII_USTRINGPARAM("jdbc:hsqldb:"));

                sConnectURL += sConnPartURL;
                Reference<XConnection> xOrig;
                try
                {
                    xOrig = xDriver->connect( sConnectURL, aConvertedProperties );
                }
                catch(SQLException e)
                {
                    StorageContainer::revokeStorage(sKey,NULL);
                    throw e;
                }
                catch(Exception e)
                {
                    StorageContainer::revokeStorage(sKey,NULL);
                    throw e;
                }

                if ( xOrig.is() )
                {
                    OMetaConnection* pMetaConnection = NULL;
                    // now we have to set the URL to get the correct answer for metadata()->getURL()
                    Reference< XUnoTunnel> xTunnel(xOrig,UNO_QUERY);
                    if ( xTunnel.is() )
                    {
                        pMetaConnection = reinterpret_cast<OMetaConnection*>(xTunnel->getSomething( OMetaConnection::getUnoTunnelImplementationId() ));
                        if ( pMetaConnection )
                            pMetaConnection->setURL(url);
                    }

                    Reference<XComponent> xComp(xOrig,UNO_QUERY);
                    if ( xComp.is() )
                        xComp->addEventListener(this);

                    // we want to close all connections when the office shuts down
                    static Reference< XTerminateListener> s_xTerminateListener;
                    if( !s_xTerminateListener.is() )
                    {
                        Reference< XDesktop > xDesktop( m_xFactory->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop")) ), UNO_QUERY );

                        if( xDesktop.is() )
                        {
                            s_xTerminateListener = new OConnectionController(this);
                            xDesktop->addTerminateListener(s_xTerminateListener);
                        }
                    }
                    Reference< XComponent> xIfc = new OConnectionWeakWrapper(xOrig,m_xFactory);
                    xConnection.set(xIfc,UNO_QUERY);
                    m_aConnections.push_back(TWeakPair(WeakReferenceHelper(xOrig),TWeakConnectionPair(sKey,WeakReferenceHelper(xConnection))));

                    Reference<XTransactionBroadcaster> xBroad(xStorage,UNO_QUERY);
                    if ( xBroad.is() )
                    {
                        Reference<XTransactionListener> xListener(*this,UNO_QUERY);
                        xBroad->addTransactionListener(xListener);
                    }
                }
            }
        }
        return xConnection;
    }

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL ODriverDelegator::acceptsURL( const ::rtl::OUString& url ) throw (SQLException, RuntimeException)
    {
        sal_Bool bEnabled = sal_False;
        javaFrameworkError eErr = jfw_getEnabled( &bEnabled );
        OSL_ENSURE( JFW_E_NONE == eErr,"error in jfw_getEnabled" );
        return bEnabled  && url.compareToAscii("sdbc:embedded:hsqldb",sizeof("sdbc:embedded:hsqldb")) == 0;
    }

    //--------------------------------------------------------------------
    Sequence< DriverPropertyInfo > SAL_CALL ODriverDelegator::getPropertyInfo( const ::rtl::OUString& url, const Sequence< PropertyValue >& info ) throw (SQLException, RuntimeException)
    {
        ::std::vector< DriverPropertyInfo > aDriverInfo;
        if ( acceptsURL(url) )
        {
            aDriverInfo.push_back(DriverPropertyInfo(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Storage"))
                    ,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Defines the storage where the database will be stored."))
                    ,sal_True
                    ,::rtl::OUString()
                    ,Sequence< ::rtl::OUString >())
                    );
            aDriverInfo.push_back(DriverPropertyInfo(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("URL"))
                    ,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Defines the url of the data source."))
                    ,sal_True
                    ,::rtl::OUString()
                    ,Sequence< ::rtl::OUString >())
                    );
            aDriverInfo.push_back(DriverPropertyInfo(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AutoRetrievingStatement"))
                    ,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Defines the statement which will be executed to retrieve auto increment values."))
                    ,sal_False
                    ,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CALL IDENTITY()"))
                    ,Sequence< ::rtl::OUString >())
                    );
        }
        return Sequence< DriverPropertyInfo >(&(aDriverInfo[0]), aDriverInfo.size());
    }

    //--------------------------------------------------------------------
    sal_Int32 SAL_CALL ODriverDelegator::getMajorVersion(  ) throw (RuntimeException)
    {
        return 1;
    }

    //--------------------------------------------------------------------
    sal_Int32 SAL_CALL ODriverDelegator::getMinorVersion(  ) throw (RuntimeException)
    {
        return 0;
    }

    //--------------------------------------------------------------------
    Reference< XTablesSupplier > SAL_CALL ODriverDelegator::getDataDefinitionByConnection( const Reference< XConnection >& connection ) throw (SQLException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        checkDisposed(ODriverDelegator_BASE::rBHelper.bDisposed);

        Reference< XTablesSupplier > xTab;
        return xTab;
    }

    //--------------------------------------------------------------------
    Reference< XTablesSupplier > SAL_CALL ODriverDelegator::getDataDefinitionByURL( const ::rtl::OUString& url, const Sequence< PropertyValue >& info ) throw (SQLException, RuntimeException)
    {
        if ( ! acceptsURL(url) )
            ::dbtools::throwGenericSQLException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Invalid URL!")) ,*this);

        return getDataDefinitionByConnection(connect(url,info));
    }

    // XServiceInfo
    // --------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    rtl::OUString ODriverDelegator::getImplementationName_Static(  ) throw(RuntimeException)
    {
        return rtl::OUString::createFromAscii("com.sun.star.sdbcx.comp.hsqldb.Driver");
    }
    //------------------------------------------------------------------------------
    Sequence< ::rtl::OUString > ODriverDelegator::getSupportedServiceNames_Static(  ) throw (RuntimeException)
    {
        Sequence< ::rtl::OUString > aSNS( 1 );
        aSNS[0] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdbc.Driver"));
        //  aSNS[1] = ::rtl::OUString::createFromAscii("com.sun.star.sdbcx.Driver");
        return aSNS;
    }
    //------------------------------------------------------------------
    ::rtl::OUString SAL_CALL ODriverDelegator::getImplementationName(  ) throw(RuntimeException)
    {
        return getImplementationName_Static();
    }

    //------------------------------------------------------------------
    sal_Bool SAL_CALL ODriverDelegator::supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException)
    {
        Sequence< ::rtl::OUString > aSupported(getSupportedServiceNames());
        const ::rtl::OUString* pSupported = aSupported.getConstArray();
        const ::rtl::OUString* pEnd = pSupported + aSupported.getLength();
        for (;pSupported != pEnd && !pSupported->equals(_rServiceName); ++pSupported)
            ;

        return pSupported != pEnd;
    }
    //------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL ODriverDelegator::getSupportedServiceNames(  ) throw(RuntimeException)
    {
        return getSupportedServiceNames_Static();
    }
    //------------------------------------------------------------------
    void SAL_CALL ODriverDelegator::createCatalog( const Sequence< PropertyValue >& info ) throw (SQLException, ::com::sun::star::container::ElementExistException, RuntimeException)
    {
    }
    //------------------------------------------------------------------
    void ODriverDelegator::shutdownConnection(const TWeakPairVector::iterator& _aIter )
    {
        OSL_ENSURE(m_aConnections.end() != _aIter,"Iterator equals .end()");
        sal_Bool bLastOne = sal_True;
        try
        {
            Reference<XConnection> _xConnection(_aIter->first.get(),UNO_QUERY);

            if ( _xConnection.is() )
            {
                Reference<XStatement> xStmt = _xConnection->createStatement();
                if ( xStmt.is() )
                {
                    Reference<XResultSet> xRes(xStmt->executeQuery(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" SELECT COUNT(*) FROM SYSTEM_SESSIONS WHERE USER_NAME ='SA'"))),UNO_QUERY);
                    Reference<XRow> xRow(xRes,UNO_QUERY);
                    if ( xRow.is() && xRes->next() )
                        bLastOne = xRow->getInt(1) == 1;
                    if ( bLastOne )
                        xStmt->execute(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("shutdown")));
                }
            }
        }
        catch(Exception&)
        {
        }
        if ( bLastOne )
        {
            Reference<XTransactionListener> xListener(*this,UNO_QUERY);
            StorageContainer::revokeStorage(_aIter->second.first,xListener);
        }
        if ( !m_bInShutDownConnections )
            m_aConnections.erase(_aIter);
    }
    //------------------------------------------------------------------
    void SAL_CALL ODriverDelegator::disposing( const ::com::sun::star::lang::EventObject& Source ) throw(::com::sun::star::uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard(m_aMutex);
        Reference<XConnection> xCon(Source.Source,UNO_QUERY);
        TWeakPairVector::iterator i = m_aConnections.begin();
        for (; m_aConnections.end() != i; ++i)
        {
            if ( i->first.get() == xCon.get() )
            {
                shutdownConnection(i);
                break;
            }
        }
        if ( !xCon.is() )
        {
            Reference< XStorage> xStorage(Source.Source,UNO_QUERY);
            if ( xStorage.is() )
            {
                ::rtl::OUString sKey = StorageContainer::getRegisteredKey(xStorage);
                i = ::std::find_if(m_aConnections.begin(),m_aConnections.end(),::std::compose1(
                                ::std::bind2nd(::std::equal_to< ::rtl::OUString >(),sKey)
                                ,::std::compose1(::std::select1st<TWeakConnectionPair>(),::std::select2nd< TWeakPair >())));
                if ( i != m_aConnections.end() )
                    shutdownConnection(i);
            }
        }
    }
    //------------------------------------------------------------------
    void ODriverDelegator::shutdownConnections()
    {
        m_bInShutDownConnections = sal_True;
        TWeakPairVector::iterator aEnd = m_aConnections.end();
        for (TWeakPairVector::iterator i = m_aConnections.begin(); aEnd != i; ++i)
        {
            try
            {
                Reference<XConnection> xCon(i->first,UNO_QUERY);
                ::comphelper::disposeComponent(xCon);
            }
            catch(Exception&)
            {
            }
        }
        m_aConnections.clear();
        m_bInShutDownConnections = sal_True;
    }
    //------------------------------------------------------------------
    void SAL_CALL ODriverDelegator::preCommit( const ::com::sun::star::lang::EventObject& aEvent ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
    {
        ::osl::MutexGuard aGuard(m_aMutex);

        Reference< XStorage> xStorage(aEvent.Source,UNO_QUERY);
        ::rtl::OUString sKey = StorageContainer::getRegisteredKey(xStorage);
        TWeakPairVector::iterator i = ::std::find_if(m_aConnections.begin(),m_aConnections.end(),::std::compose1(
                         ::std::bind2nd(::std::equal_to< ::rtl::OUString >(),sKey)
                        ,::std::compose1(::std::select1st<TWeakConnectionPair>(),::std::select2nd< TWeakPair >())));
        if ( i != m_aConnections.end() )
        {
            try
            {
                Reference<XConnection> xConnection(i->first,UNO_QUERY);
                if ( xConnection.is() )
                {
                    Reference< XStatement> xStmt = xConnection->createStatement();
                    if ( xStmt.is() )
                        xStmt->execute(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SET WRITE_DELAY 60")));
                }
            }
            catch(Exception&)
            {
            }
        }
    }
    //------------------------------------------------------------------
    void SAL_CALL ODriverDelegator::commited( const ::com::sun::star::lang::EventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException)
    {
    }
    //------------------------------------------------------------------
    void SAL_CALL ODriverDelegator::preRevert( const ::com::sun::star::lang::EventObject& aEvent ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
    {
    }
    //------------------------------------------------------------------
    void SAL_CALL ODriverDelegator::reverted( const ::com::sun::star::lang::EventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException)
    {
    }
    //------------------------------------------------------------------
    //------------------------------------------------------------------
//........................................................................
}   // namespace connectivity
//........................................................................

