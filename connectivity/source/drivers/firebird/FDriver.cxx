/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  the BSD license.
 *
 *  Copyright 2000, 2010 Oracle and/or its affiliates.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Sun Microsystems, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************/

#include "FDriver.hxx"
#include "FConnection.hxx"
#include "connectivity/dbexception.hxx"
#include "resource/common_res.hrc"
#include "resource/hsqldb_res.hrc"
#include "resource/sharedresources.hxx"

#include <unotools/tempfile.hxx>

#include <comphelper/processfactory.hxx>
#include <unotools/ucbstreamhelper.hxx>

#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/io/TempFile.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/ucb/SimpleFileAccess.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess2.hpp>

using namespace com::sun::star;
using namespace com::sun::star::embed;
using namespace com::sun::star::io;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;

using namespace connectivity::firebird;

namespace connectivity
{
    namespace firebird
    {
        //------------------------------------------------------------------
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  SAL_CALL FirebirdDriver_CreateInstance(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory) throw( ::com::sun::star::uno::Exception )
        {
            SAL_INFO("connectivity.firebird", "=> ODriver_BASE::FirebirdDriver_CreateInstance()" );
            (void) _rxFactory;
            return *(new FirebirdDriver());
        }
    }
}
// --------------------------------------------------------------------------------
FirebirdDriver::FirebirdDriver()
    : ODriver_BASE(m_aMutex),
      mbIsEmbedded(false),
      mFilePath()
{
}
// --------------------------------------------------------------------------------
void FirebirdDriver::disposing()
{
    ::osl::MutexGuard aGuard(m_aMutex);

    // when driver will be destroied so all our connections have to be destroied as well
    for (OWeakRefArray::iterator i = m_xConnections.begin(); m_xConnections.end() != i; ++i)
    {
        Reference< XComponent > xComp(i->get(), UNO_QUERY);
        if (xComp.is())
            xComp->dispose();
    }
    m_xConnections.clear();

    ODriver_BASE::disposing();
}

// static ServiceInfo
//------------------------------------------------------------------------------
rtl::OUString FirebirdDriver::getImplementationName_Static(  ) throw(RuntimeException)
{
    return rtl::OUString("com.sun.star.comp.sdbc.FirebirdDriver");
        // this name is referenced in the configuration and in the firebird.xml
        // Please take care when changing it.
}
//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > FirebirdDriver::getSupportedServiceNames_Static(  ) throw (RuntimeException)
{
    // which service is supported
    // for more information @see com.sun.star.sdbc.Driver
    Sequence< ::rtl::OUString > aSNS( 1 );
    aSNS[0] = ::rtl::OUString("com.sun.star.sdbc.Driver");
    return aSNS;
}

//------------------------------------------------------------------
::rtl::OUString SAL_CALL FirebirdDriver::getImplementationName(  ) throw(RuntimeException)
{
    return getImplementationName_Static();
}

//------------------------------------------------------------------
sal_Bool SAL_CALL FirebirdDriver::supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException)
{
    Sequence< ::rtl::OUString > aSupported(getSupportedServiceNames());
    const ::rtl::OUString* pSupported = aSupported.getConstArray();
    const ::rtl::OUString* pEnd = pSupported + aSupported.getLength();
    for (;pSupported != pEnd && !pSupported->equals(_rServiceName); ++pSupported)
        ;

    return pSupported != pEnd;
}

//------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL FirebirdDriver::getSupportedServiceNames(  ) throw(RuntimeException)
{
    return getSupportedServiceNames_Static();
}

// --------------------------------------------------------------------------------
Reference< XConnection > SAL_CALL FirebirdDriver::connect( const ::rtl::OUString& url, const Sequence< PropertyValue >& info ) throw(SQLException, RuntimeException)
{
    Reference< XConnection > xConnection;
    bool bIsNewDatabase = false;
    if (url.equals("sdbc:embedded:firebird"))
    {
        mbIsEmbedded = true;
        Reference<XStorage> xStorage;
        const PropertyValue* pIter = info.getConstArray();
        const PropertyValue* pEnd = pIter + info.getLength();

        for (;pIter != pEnd; ++pIter)
        {
            if ( pIter->Name == "Storage" )
            {
                xStorage.set(pIter->Value,UNO_QUERY);
            }
        }

        if ( !xStorage.is() )
        {
            ::connectivity::SharedResources aResources;
            const OUString sMessage = aResources.getResourceString(STR_NO_STROAGE);
            ::dbtools::throwGenericSQLException(sMessage ,*this);
        }

        bIsNewDatabase = !xStorage->hasElements();

        const OUString sDBName( "firebird.fdb" ); // Location within .odb container
        mFilePath = utl::TempFile::CreateTempName() + ".fdb";

        SAL_INFO("connectivity.firebird", "Temporary .fdb location:  "
                    << OUStringToOString(mFilePath,RTL_TEXTENCODING_UTF8 ).getStr());
        if (!bIsNewDatabase)
        {
            SAL_INFO("connectivity.firebird", "Extracting .fdb from .odb" );
            if (!xStorage->isStreamElement(sDBName))
            {
                ::connectivity::SharedResources aResources;
                const OUString sMessage = aResources.getResourceString(STR_ERROR_NEW_VERSION);
                ::dbtools::throwGenericSQLException(sMessage ,*this);
            }

            Reference< XStream > xDBStream(xStorage->openStreamElement(sDBName,
                                                            ElementModes::READ));

            SAL_INFO("connectivity.firebird", ".fdb being written to "
                    << OUStringToOString(mFilePath,RTL_TEXTENCODING_UTF8 ).getStr());

            uno::Reference< ucb::XSimpleFileAccess2 > xFileAccess(
                    ucb::SimpleFileAccess::create( comphelper::getProcessComponentContext() ),
                                                                uno::UNO_QUERY );
            if ( !xFileAccess.is() )
            {
                // TODO: Error
                ::connectivity::SharedResources aResources;
                const OUString sMessage = aResources.getResourceString(STR_ERROR_NEW_VERSION);
                ::dbtools::throwGenericSQLException(sMessage ,*this);
            }
            try {
                xFileAccess->writeFile(mFilePath,xDBStream->getInputStream());
            }
            catch (...)
            {
                // TODO
            }
        }

        if (bIsNewDatabase)
        {
        }
        // Get DB properties from XML

    }

    SAL_INFO("connectivity.firebird", "=> ODriver_BASE::connect(), URL: " << url );

    ::osl::MutexGuard aGuard( m_aMutex );
    if (ODriver_BASE::rBHelper.bDisposed)
       throw DisposedException();

    if ( ! acceptsURL(url) )
        return NULL;

    bool bCreateNewFile = mbIsEmbedded&&bIsNewDatabase;

    // create a new connection with the given properties and append it to our vector
    OConnection* pCon = new OConnection(this);
    Reference< XConnection > xCon = pCon;   // important here because otherwise the connection could be deleted inside (refcount goes -> 0)
    pCon->construct(mFilePath,info,bCreateNewFile); // late constructor call which can throw exception and allows a correct dtor call when so
    m_xConnections.push_back(WeakReferenceHelper(*pCon));

    return xCon;
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL FirebirdDriver::acceptsURL( const ::rtl::OUString& url )
        throw(SQLException, RuntimeException)
{
    return url.equals("sdbc:embedded:firebird") || url.startsWith("sdbc:firebird:");
}
// --------------------------------------------------------------------------------
Sequence< DriverPropertyInfo > SAL_CALL FirebirdDriver::getPropertyInfo( const ::rtl::OUString& url, const Sequence< PropertyValue >& info ) throw(SQLException, RuntimeException)
{
    (void) info;
    if ( ! acceptsURL(url) )
    {
        ::connectivity::SharedResources aResources;
        const OUString sMessage = aResources.getResourceString(STR_URI_SYNTAX_ERROR);
        ::dbtools::throwGenericSQLException(sMessage ,*this);
    } // if ( ! acceptsURL(url) )

    // if you have somthing special to say, return it here :-)
    return Sequence< DriverPropertyInfo >();
}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL FirebirdDriver::getMajorVersion(  ) throw(RuntimeException)
{
    return 0; // depends on you
}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL FirebirdDriver::getMinorVersion(  ) throw(RuntimeException)
{
    return 1; // depends on you
}
// --------------------------------------------------------------------------------

//.........................................................................
namespace connectivity
{
    namespace firebird
    {
//.........................................................................

        void release(oslInterlockedCount& _refCount,
                     ::cppu::OBroadcastHelper& rBHelper,
                     ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _xInterface,
                     ::com::sun::star::lang::XComponent* _pObject)
        {
            if (osl_atomic_decrement( &_refCount ) == 0)
            {
                osl_atomic_increment( &_refCount );

                if (!rBHelper.bDisposed && !rBHelper.bInDispose)
                {
                    // remember the parent
                    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xParent;
                    {
                        ::osl::MutexGuard aGuard( rBHelper.rMutex );
                        xParent = _xInterface;
                        _xInterface = NULL;
                    }

                    // First dispose
                    _pObject->dispose();

                    // only the alive ref holds the object
                    OSL_ASSERT( _refCount == 1 );

                    // release the parent in the ~
                    if (xParent.is())
                    {
                        ::osl::MutexGuard aGuard( rBHelper.rMutex );
                        _xInterface = xParent;
                    }
                }
            }
            else
                osl_atomic_increment( &_refCount );
        }

        void checkDisposed(sal_Bool _bThrow) throw ( DisposedException )
        {
            if (_bThrow)
                throw DisposedException();

        }
//.........................................................................
    }
} // namespace connectivity
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
