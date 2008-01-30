/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: FConnection.cxx,v $
 *
 *  $Revision: 1.49 $
 *
 *  last change: $Author: rt $ $Date: 2008-01-30 07:51:57 $
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

#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _CONNECTIVITY_FILE_OCONNECTION_HXX_
#include "file/FConnection.hxx"
#endif
#ifndef _CONNECTIVITY_FILE_ODATABASEMETADATA_HXX_
#include "file/FDatabaseMetaData.hxx"
#endif
#ifndef _CONNECTIVITY_FILE_ODRIVER_HXX_
#include "file/FDriver.hxx"
#endif
#ifndef _CONNECTIVITY_FILE_OSTATEMENT_HXX_
#include "file/FStatement.hxx"
#endif
#ifndef _CONNECTIVITY_FILE_OPREPAREDSTATEMENT_HXX_
#include "file/FPreparedStatement.hxx"
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
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENT_HPP_
#include <com/sun/star/ucb/XContent.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTIDENTIFIER_HPP_
#include <com/sun/star/ucb/XContentIdentifier.hpp>
#endif
#ifndef _URLOBJ_HXX //autogen wg. INetURLObject
#include <tools/urlobj.hxx>
#endif
#ifndef _CONNECTIVITY_FILE_CATALOG_HXX_
#include "file/FCatalog.hxx"
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef _DBHELPER_DBCHARSET_HXX_
#include <connectivity/dbcharset.hxx>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif
#ifndef _OSL_NLSUPPORT_H_
#include <osl/nlsupport.h>
#endif

using namespace connectivity::file;
using namespace dbtools;
//------------------------------------------------------------------------------
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;
using namespace com::sun::star::container;
using namespace com::sun::star::ucb;
using namespace ::ucbhelper;
using rtl::OUString;
typedef connectivity::OMetaConnection OConnection_BASE;
// --------------------------------------------------------------------------------
OConnection::OConnection(OFileDriver*   _pDriver)
                         : OSubComponent<OConnection, OConnection_BASE>((::cppu::OWeakObject*)_pDriver, this)
                         ,m_pDriver(_pDriver)
                         ,m_bClosed(sal_False)
                         ,m_bShowDeleted(sal_False)
                         ,m_bCaseSensitiveExtension( sal_True )
                         ,m_bCheckSQL92(sal_False)
{
    m_nTextEncoding = RTL_TEXTENCODING_DONTKNOW;
}
//-----------------------------------------------------------------------------
OConnection::~OConnection()
{
    if(!isClosed(  ))
        close();
}
//-----------------------------------------------------------------------------
void SAL_CALL OConnection::release() throw()
{
    relase_ChildImpl();
}

//-----------------------------------------------------------------------------
sal_Bool OConnection::matchesExtension( const String& _rExt ) const
{
    if ( isCaseSensitveExtension() )
        return ( getExtension() == _rExt );

    String sMyExtension( getExtension() );
    sMyExtension.ToLowerAscii();
    String sExt( _rExt );
    sExt.ToLowerAscii();

    return sMyExtension == sExt;
}

//-----------------------------------------------------------------------------
void OConnection::construct(const ::rtl::OUString& url,const Sequence< PropertyValue >& info)  throw(SQLException)
{
    osl_incrementInterlockedCount( &m_refCount );

    {
        sal_Int32 nLen = url.indexOf(':');
        nLen = url.indexOf(':',nLen+1);
        ::rtl::OUString aDSN(url.copy(nLen+1)),aUID,aPWD;

        String aFileName = aDSN;
        INetURLObject aURL;
        aURL.SetSmartProtocol(INET_PROT_FILE);
        {
            SvtPathOptions aPathOptions;
            aFileName = aPathOptions.SubstituteVariable(aFileName);
        }
        aURL.SetSmartURL(aFileName);

        setURL(aURL.GetMainURL(INetURLObject::NO_DECODE));
    }

    ::rtl::OUString aExt;
    const PropertyValue *pBegin  = info.getConstArray();
    const PropertyValue *pEnd    = pBegin + info.getLength();
    for(;pBegin != pEnd;++pBegin)
    {
        if(0 == pBegin->Name.compareToAscii("Extension"))
            OSL_VERIFY( pBegin->Value >>= aExt );
        else if(0 == pBegin->Name.compareToAscii("CharSet"))
        {
            ::rtl::OUString sIanaName;
            OSL_VERIFY( pBegin->Value >>= sIanaName );

            ::dbtools::OCharsetMap aLookupIanaName;
            ::dbtools::OCharsetMap::const_iterator aLookup = aLookupIanaName.find(sIanaName, ::dbtools::OCharsetMap::IANA());
            if (aLookup != aLookupIanaName.end())
                m_nTextEncoding = (*aLookup).getEncoding();
            else
                m_nTextEncoding = RTL_TEXTENCODING_DONTKNOW;
        }
        else if (0 == pBegin->Name.compareToAscii("ShowDeleted"))
        {
            OSL_VERIFY( pBegin->Value >>= m_bShowDeleted );
        }
        else if (0 == pBegin->Name.compareToAscii("EnableSQL92Check"))
        {
            pBegin->Value >>= m_bCheckSQL92;
        }
    } // for(;pBegin != pEnd;++pBegin)

    if ( m_nTextEncoding == RTL_TEXTENCODING_DONTKNOW )
    {
        //m_nTextEncoding = osl_getTextEncodingFromLocale(NULL);
        m_nTextEncoding = osl_getThreadTextEncoding();
    }

    if ( aExt.getLength() )
        m_aFilenameExtension = aExt;

    try
    {
        ::ucbhelper::Content aFile;
        try
        {
            aFile = ::ucbhelper::Content(getURL(),Reference< XCommandEnvironment >());
        }
        catch(ContentCreationException& e)
        {
            throwUrlNotValid(getURL(),e.Message);
        }

        // set fields to fetch
        Sequence< OUString > aProps(1);
        OUString* pProps = aProps.getArray();
        pProps[ 0 ] = OUString::createFromAscii( "Title" );

        try
        {
            if (aFile.isFolder())
            {
                m_xDir = aFile.createDynamicCursor(aProps, ::ucbhelper::INCLUDE_DOCUMENTS_ONLY );
                m_xContent = aFile.get();
            }
            else if (aFile.isDocument())
            {
                Reference<XContent> xParent(Reference<XChild>(aFile.get(),UNO_QUERY)->getParent(),UNO_QUERY);
                Reference<XContentIdentifier> xIdent = xParent->getIdentifier();
                m_xContent = xParent;

                ::ucbhelper::Content aParent(xIdent->getContentIdentifier(),Reference< XCommandEnvironment >());
                m_xDir = aParent.createDynamicCursor(aProps, ::ucbhelper::INCLUDE_DOCUMENTS_ONLY );
            }
            else
            {
                OSL_ENSURE(0,"OConnection::construct: ::ucbhelper::Content isn't a folde nor a document! How that?!");
                throw SQLException();
            }
        }
        catch(Exception& e) // a execption is thrown when no file exists
        {
            throwUrlNotValid(getURL(),e.Message);
        }
        if(!m_xDir.is() || !m_xContent.is())
            throwUrlNotValid(getURL(),::rtl::OUString());

        if (m_aFilenameExtension.Search('*') != STRING_NOTFOUND || m_aFilenameExtension.Search('?') != STRING_NOTFOUND)
            throw SQLException();
    }
    catch(const Exception&)
    {
        osl_decrementInterlockedCount( &m_refCount );
        throw;
    }

    osl_decrementInterlockedCount( &m_refCount );
}
// XServiceInfo
// --------------------------------------------------------------------------------
IMPLEMENT_SERVICE_INFO(OConnection, "com.sun.star.sdbc.drivers.file.Connection", "com.sun.star.sdbc.Connection")

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


    OPreparedStatement* pStmt = new OPreparedStatement(this);
    Reference< XPreparedStatement > xHoldAlive = pStmt;
    pStmt->construct(sql);
    m_aStatements.push_back(WeakReferenceHelper(*pStmt));
    return pStmt;
}
// --------------------------------------------------------------------------------
Reference< XPreparedStatement > SAL_CALL OConnection::prepareCall( const ::rtl::OUString& /*sql*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XConnection::prepareCall", *this );
    return NULL;
}
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OConnection::nativeSQL( const ::rtl::OUString& sql ) throw(SQLException, RuntimeException)
{
    return sql;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setAutoCommit( sal_Bool autoCommit ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);

    m_bAutoCommit = autoCommit;
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL OConnection::getAutoCommit(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);

    return m_bAutoCommit;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::commit(  ) throw(SQLException, RuntimeException)
{
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::rollback(  ) throw(SQLException, RuntimeException)
{
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
        xMetaData = new ODatabaseMetaData(this);
        m_xMetaData = xMetaData;
    }

    return xMetaData;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setReadOnly( sal_Bool readOnly ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    m_bReadOnly = readOnly;
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL OConnection::isReadOnly(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    return m_bReadOnly;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setCatalog( const ::rtl::OUString& /*catalog*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XConnection::setCatalog", *this );
}
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OConnection::getCatalog(  ) throw(SQLException, RuntimeException)
{
    return ::rtl::OUString();
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setTransactionIsolation( sal_Int32 /*level*/ ) throw(SQLException, RuntimeException)
{
    throwFeatureNotImplementedException( "XConnection::setTransactionIsolation", *this );
}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL OConnection::getTransactionIsolation(  ) throw(SQLException, RuntimeException)
{
    return 0;
}
// --------------------------------------------------------------------------------
Reference< XNameAccess > SAL_CALL OConnection::getTypeMap(  ) throw(SQLException, RuntimeException)
{
    return NULL;
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setTypeMap( const Reference< XNameAccess >& /*typeMap*/ ) throw(SQLException, RuntimeException)
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
//------------------------------------------------------------------------------
void OConnection::disposing()
{
    ::osl::MutexGuard aGuard(m_aMutex);
    OConnection_BASE::disposing();

    m_bClosed   = sal_True;
    m_xDir      = NULL;
    m_xContent  = NULL;
    m_xCatalog  = WeakReference< XTablesSupplier>();

    dispose_ChildImpl();
}
//------------------------------------------------------------------------------
Reference< XTablesSupplier > OConnection::createCatalog()
{
    ::osl::MutexGuard aGuard( m_aMutex );
    Reference< XTablesSupplier > xTab = m_xCatalog;
    if(!xTab.is())
    {
        xTab = new OFileCatalog(this);
        m_xCatalog = xTab;
    }
    return xTab;
}
// -----------------------------------------------------------------------------
Reference< XDynamicResultSet > OConnection::getDir() const
{
    Reference<XDynamicResultSet> xContent;
    Sequence< ::rtl::OUString > aProps(1);
    ::rtl::OUString* pProps = aProps.getArray();
    pProps[ 0 ] = ::rtl::OUString::createFromAscii( "Title" );
    try
    {
        Reference<XContentIdentifier> xIdent = getContent()->getIdentifier();
        ::ucbhelper::Content aParent(xIdent->getContentIdentifier(),Reference< XCommandEnvironment >());
        xContent = aParent.createDynamicCursor(aProps, ::ucbhelper::INCLUDE_DOCUMENTS_ONLY );
    }
    catch(Exception&)
    {
    }
    return xContent;
}
// -----------------------------------------------------------------------------
sal_Int64 SAL_CALL OConnection::getSomething( const Sequence< sal_Int8 >& rId ) throw (RuntimeException)
{
    return (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
        ? reinterpret_cast< sal_Int64 >( this )
        : (sal_Int64)0;
}
// -----------------------------------------------------------------------------
Sequence< sal_Int8 > OConnection::getUnoTunnelImplementationId()
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
// -----------------------------------------------------------------------------
void OConnection::throwUrlNotValid(const ::rtl::OUString & _rsUrl,const ::rtl::OUString & _rsMessage)
{
    SQLException aError;
    aError.Message = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unable to create a content for the URL given."));
    aError.SQLState = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000"));
    aError.ErrorCode = 0;
    aError.Context = static_cast< XConnection* >(this);
    SQLException aUrlError;
    if(_rsUrl.getLength())
    {
        aUrlError.Message = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Invalid URL: "));
        aUrlError.Message += _rsUrl;
    }
    else
        aUrlError.Message = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("No URL supplied!"));

    aError.NextException <<= aUrlError;
    if (_rsMessage.getLength())
        aUrlError.NextException <<= SQLException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("UCB message: ")) += _rsMessage, aError.Context, ::rtl::OUString(), 0, Any());

    throw aError;
}
// -----------------------------------------------------------------------------



