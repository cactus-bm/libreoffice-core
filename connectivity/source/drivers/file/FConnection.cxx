/*************************************************************************
 *
 *  $RCSfile: FConnection.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-25 07:15:00 $
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

#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _CONNECTIVITY_FILE_OCONNECTION_HXX_
#include "file/FConnection.hxx"
#endif
#ifndef _CONNECTIVITY_FILE_ODATABASEMETADATA_HXX_
#include "file/FDatabaseMetaData.hxx"
#endif
#ifndef _CONNECTIVITY_RESOURCE_HRC_
#include "Resource.hrc"
#endif
#ifndef _CONNECTIVITY_MODULECONTEXT_HXX_
#include "ModuleContext.hxx"
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
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif
#ifndef _CONNECTIVITY_MODULECONTEXT_HXX_
#include "ModuleContext.hxx"
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
using namespace ::ucb;
using namespace rtl;
typedef connectivity::OMetaConnection OConnection_BASE;
// --------------------------------------------------------------------------------
OConnection::OConnection(OFileDriver*   _pDriver)
                         : OSubComponent<OConnection, OConnection_BASE>((::cppu::OWeakObject*)_pDriver, this)
                         ,m_pDriver(_pDriver)
                         ,m_bClosed(sal_False)
                         ,m_xMetaData(NULL)
{
    ModuleContext::AddRef();
}
//-----------------------------------------------------------------------------
OConnection::~OConnection()
{
    if(!isClosed(  ))
        close();
    ModuleContext::ReleaseRef();
}
//-----------------------------------------------------------------------------
void SAL_CALL OConnection::release() throw(RuntimeException)
{
    relase_ChildImpl();
}

//-----------------------------------------------------------------------------
void OConnection::construct(const ::rtl::OUString& url,const Sequence< PropertyValue >& info)  throw(SQLException)
{
    osl_incrementInterlockedCount( &m_refCount );

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


    //  String aFileName = aURL.PathToFileName();
    aFileName = aURL.GetMainURL();
    m_aURL = aFileName;

    if(!aFileName.Len())
        aFileName = aDSN;
    String aWildcard;

    ::rtl::OUString aExt;
    const PropertyValue *pBegin  = info.getConstArray();
    const PropertyValue *pEnd    = pBegin + info.getLength();
    for(;pBegin != pEnd;++pBegin)
    {
        if(0 == pBegin->Name.compareToAscii("Extension"))
            pBegin->Value >>= aExt;
        if(0 == pBegin->Name.compareToAscii("CharSet"))
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

    if(aExt.getLength())
        m_aFilenameExtension = aExt;

    ::ucb::Content aFile;
    try
    {
        aFile = ::ucb::Content(aFileName,Reference< ::com::sun::star::ucb::XCommandEnvironment >());
    }
    catch(ContentCreationException&e)
    {
        SQLException aError;
        aError.Message = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unable to create a content for the URL given."));
        aError.SQLState = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000"));
        aError.ErrorCode = 0;
        aError.Context = static_cast< XConnection* >(this);
        SQLException aUrlError;
        aUrlError.Message = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Invalid URL: "));
        aUrlError.Message += url;
        aError.NextException <<= aUrlError;
        if (e.Message.getLength())
        {
            aUrlError.NextException <<= SQLException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("UCB message: ")) += e.Message, aError.Context, ::rtl::OUString(), 0, Any());
        }
        throw aError;
    }

//  if (aFileStat.IsKind(FSYS_KIND_WILD))
//  {
//      m_aDirectoryName = (DirEntry(aFileName).GetPath()).GetFull();
//      m_aFilenameExtension = DirEntry(aFileName).GetExtension();
//
//      // nyi: Verschiedene Extensions (aufgrund des angegebenen Wildcard)
//      // (bisher keine Wildcard in der Extension moeglich!)
//      aWildcard = DirEntry(aFileName).GetName();
//  } else

    // set fields to fetch
    Sequence< OUString > aProps(1);
    OUString* pProps = aProps.getArray();
    pProps[ 0 ] = OUString::createFromAscii( "Title" );

    try
    {
        if (aFile.isFolder())
        {
            m_xDir = aFile.createDynamicCursor(aProps, ::ucb::INCLUDE_DOCUMENTS_ONLY );
            m_xContent = aFile.get();
        }
        else if (aFile.isDocument())
        {
            Reference<XContent> xParent(Reference<XChild>(aFile.get(),UNO_QUERY)->getParent(),UNO_QUERY);
            Reference<XContentIdentifier> xIdent = xParent->getIdentifier();
            m_xContent = xParent;

            ::ucb::Content aParent(xIdent->getContentIdentifier(),Reference< XCommandEnvironment >());
            m_xDir = aParent.createDynamicCursor(aProps, ::ucb::INCLUDE_DOCUMENTS_ONLY );
        }
        else
            throw SQLException();
    }
    catch(Exception& e) // a execption is thrown when no file exists
    {
        SQLException aError;
        aError.Message = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unable to create a content for the URL given."));
        aError.SQLState = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000"));
        aError.ErrorCode = 0;
        aError.Context = static_cast< XConnection* >(this);
        if (e.Message.getLength())
            aError.NextException <<= SQLException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("UCB message: ")) += e.Message, aError.Context, ::rtl::OUString(), 0, Any());
        throw aError;
    }
    if(!m_xDir.is() || !m_xContent.is())
        throw SQLException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unable to create a content for the URL given.")),
                     *this,
                     ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000")),
                     0,
                     Any());

    if (m_aFilenameExtension.Search('*') != STRING_NOTFOUND || m_aFilenameExtension.Search('?') != STRING_NOTFOUND)
        throw SQLException();

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
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);



}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::rollback(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);



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
void SAL_CALL OConnection::setCatalog( const ::rtl::OUString& catalog ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);

}
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OConnection::getCatalog(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    return ::rtl::OUString();
}
// --------------------------------------------------------------------------------
void SAL_CALL OConnection::setTransactionIsolation( sal_Int32 level ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL OConnection::getTransactionIsolation(  ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    checkDisposed(OConnection_BASE::rBHelper.bDisposed);


    return 0;
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

    m_bClosed   = sal_True;
    m_xMetaData = ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XDatabaseMetaData>();
    m_xDir      = NULL;
    m_xContent  = NULL;
    m_xCatalog  = ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbcx::XTablesSupplier>();

    dispose_ChildImpl();
    OConnection_BASE::disposing();
}
//------------------------------------------------------------------------------
::com::sun::star::uno::Reference< XTablesSupplier > OConnection::createCatalog()
{
    ::osl::MutexGuard aGuard( m_aMutex );
    Reference< XTablesSupplier > xTab = m_xCatalog;
    if(!m_xCatalog.get().is())
    {
        OFileCatalog *pCat = new OFileCatalog(this);
        xTab = pCat;
        m_xCatalog = xTab;
    }
    return xTab;
}
// -----------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::ucb::XDynamicResultSet > OConnection::getDir() const
{
    Reference<XDynamicResultSet> xContent;
    try
    {
        Reference<XContentIdentifier> xIdent = getContent()->getIdentifier();
        ::ucb::Content aParent(xIdent->getContentIdentifier(),Reference< XCommandEnvironment >());
        Sequence< ::rtl::OUString > aProps(1);
        ::rtl::OUString* pProps = aProps.getArray();
        pProps[ 0 ] = ::rtl::OUString::createFromAscii( "Title" );
        xContent = aParent.createDynamicCursor(aProps, ::ucb::INCLUDE_DOCUMENTS_ONLY );
    }
    catch(Exception&)
    {
    }
    return xContent;
}
// -----------------------------------------------------------------------------
sal_Int64 SAL_CALL OConnection::getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& rId ) throw (::com::sun::star::uno::RuntimeException)
{
    if (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
        return (sal_Int64)this;

    return 0;
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



