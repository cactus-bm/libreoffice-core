/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: KDriver.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 02:52:25 $
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

#include "KDriver.hxx"

#ifndef CONNECTIVITY_KAB_KDEINIT_H
#include "KDEInit.h"
#endif
#ifndef _CONNECTIVITY_KAB_CONNECTION_HXX_
#include "KConnection.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_SDB_SQLCONTEXT_HPP_
#include <com/sun/star/sdb/SQLContext.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_NULLPOINTEREXCEPTION_HPP_
#include <com/sun/star/lang/NullPointerException.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif
/** === end UNO includes === **/

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef TOOLS_DIAGNOSE_EX_H
#include <tools/diagnose_ex.h>
#endif

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdb;
using namespace com::sun::star::frame;
using namespace connectivity::kab;

// =======================================================================
// = KabImplModule
// =======================================================================
// --------------------------------------------------------------------------------
KabImplModule::KabImplModule( const Reference< XMultiServiceFactory >& _rxFactory )
    :m_xORB(_rxFactory)
    ,m_bAttemptedLoadModule(false)
    ,m_bAttemptedInitialize(false)
    ,m_hConnectorModule(NULL)
    ,m_pConnectionFactoryFunc(NULL)
    ,m_pApplicationInitFunc(NULL)
    ,m_pApplicationShutdownFunc(NULL)
    ,m_pKDEVersionCheckFunc(NULL)
{
    if ( !m_xORB.is() )
        throw NullPointerException();
}

// --------------------------------------------------------------------------------
bool KabImplModule::isKDEPresent()
{
    if ( !impl_loadModule() )
        return false;

    return true;
}

// --------------------------------------------------------------------------------
KabImplModule::KDEVersionType KabImplModule::matchKDEVersion()
{
    OSL_PRECOND( m_pKDEVersionCheckFunc, "KabImplModule::matchKDEVersion: module not loaded!" );

    int nVersionInfo = (*m_pKDEVersionCheckFunc)();
    if ( nVersionInfo < 0 )
        return eTooOld;
    if ( nVersionInfo > 0 )
        return eToNew;
    return eSupported;
}

// --------------------------------------------------------------------------------
namespace
{
    template< typename FUNCTION >
    void lcl_getFunctionFromModuleOrUnload( oslModule& _rModule, const sal_Char* _pAsciiSymbolName, FUNCTION& _rFunction )
    {
        _rFunction = NULL;
        if ( _rModule )
        {
            //
            const ::rtl::OUString sSymbolName = ::rtl::OUString::createFromAscii( _pAsciiSymbolName );
            _rFunction = (FUNCTION)( osl_getSymbol( _rModule, sSymbolName.pData ) );

            if ( !_rFunction )
            {   // did not find the symbol
                OSL_ENSURE( false, ::rtl::OString( "lcl_getFunctionFromModuleOrUnload: could not find the symbol " ) + ::rtl::OString( _pAsciiSymbolName ) );
                osl_unloadModule( _rModule );
                _rModule = NULL;
            }
        }
    }
}

// --------------------------------------------------------------------------------
bool KabImplModule::impl_loadModule()
{
    if ( m_bAttemptedLoadModule )
        return ( m_hConnectorModule != NULL );
    m_bAttemptedLoadModule = true;

    OSL_ENSURE( !m_hConnectorModule && !m_pConnectionFactoryFunc && !m_pApplicationInitFunc && !m_pApplicationShutdownFunc && !m_pKDEVersionCheckFunc,
        "KabImplModule::impl_loadModule: inconsistence: inconsistency (never attempted load before, but some values already set)!");

    const ::rtl::OUString sModuleName = ::rtl::OUString::createFromAscii( SAL_MODULENAME( "kabdrv1" ) );
    m_hConnectorModule = osl_loadModule( sModuleName.pData, SAL_LOADMODULE_NOW );   // LAZY! #i61335#
    OSL_ENSURE( m_hConnectorModule, "KabImplModule::impl_loadModule: could not load the implementation library!" );
    if ( !m_hConnectorModule )
        return false;

    lcl_getFunctionFromModuleOrUnload( m_hConnectorModule, "createKabConnection",   m_pConnectionFactoryFunc );
    lcl_getFunctionFromModuleOrUnload( m_hConnectorModule, "initKApplication",      m_pApplicationInitFunc );
    lcl_getFunctionFromModuleOrUnload( m_hConnectorModule, "shutdownKApplication",  m_pApplicationShutdownFunc );
    lcl_getFunctionFromModuleOrUnload( m_hConnectorModule, "matchKDEVersion",       m_pKDEVersionCheckFunc );

    if ( !m_hConnectorModule )
        // one of the symbols did not exist
        throw RuntimeException();

    return true;
}

// --------------------------------------------------------------------------------
void KabImplModule::impl_unloadModule()
{
    OSL_PRECOND( m_hConnectorModule != NULL, "KabImplModule::impl_unloadModule: no module!" );

    osl_unloadModule( m_hConnectorModule );
    m_hConnectorModule = NULL;

    m_pConnectionFactoryFunc = NULL;
    m_pApplicationInitFunc = NULL;
    m_pApplicationShutdownFunc = NULL;
    m_pKDEVersionCheckFunc = NULL;

    m_bAttemptedLoadModule = false;
}

// --------------------------------------------------------------------------------
void KabImplModule::init()
{
    if ( !impl_loadModule() )
        impl_throwNoKdeException();

    // if we're not running on a supported version, throw
    KabImplModule::KDEVersionType eKDEVersion = matchKDEVersion();

    if ( eKDEVersion == eTooOld )
        impl_throwKdeTooOldException();

    if ( ( eKDEVersion == eToNew ) && !impl_doAllowNewKDEVersion() )
        impl_throwKdeTooNewException();

    if ( !m_bAttemptedInitialize )
    {
        m_bAttemptedInitialize = true;
        (*m_pApplicationInitFunc)();
    }
}

// --------------------------------------------------------------------------------
bool KabImplModule::impl_doAllowNewKDEVersion()
{
    try
    {
        Reference< XMultiServiceFactory > xConfigProvider(
            m_xORB->createInstance( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.configuration.ConfigurationProvider" ) ) ),
            UNO_QUERY_THROW );
        Sequence< Any > aCreationArgs(1);
        aCreationArgs[0] <<= PropertyValue(
                                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "nodepath" ) ),
                                0,
                                makeAny( KabDriver::impl_getConfigurationSettingsPath() ),
                                PropertyState_DIRECT_VALUE );
        Reference< XPropertySet > xSettings( xConfigProvider->createInstanceWithArguments(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.configuration.ConfigurationAccess" ) ),
                aCreationArgs ),
            UNO_QUERY_THROW );

        sal_Bool bDisableCheck = sal_False;
        xSettings->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DisableKDEMaximumVersionCheck" ) ) ) >>= bDisableCheck;

        return bDisableCheck != sal_False;
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    return false;
}

// --------------------------------------------------------------------------------
void KabImplModule::impl_throwNoKdeException()
{
    impl_throwGenericSQLException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "No suitable KDE installation was found." ) ) );
}

// --------------------------------------------------------------------------------
void KabImplModule::impl_throwKdeTooOldException()
{
    ::rtl::OUStringBuffer aMessage;
    aMessage.appendAscii( "KDE version " );
    aMessage.append( (sal_Int32)MIN_KDE_VERSION_MAJOR );
    aMessage.append( (sal_Unicode)'.' );
    aMessage.append( (sal_Int32)MIN_KDE_VERSION_MINOR );
    aMessage.appendAscii( " or higher is required to access the KDE Address Book." );

    impl_throwGenericSQLException( aMessage.makeStringAndClear() );
}

// --------------------------------------------------------------------------------
void KabImplModule::impl_throwGenericSQLException( const ::rtl::OUString& _rMessage )
{
    SQLException aError;
    aError.Message = _rMessage;
    aError.SQLState = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "S1000" ) );
    aError.ErrorCode = 0;
    throw aError;
}

// --------------------------------------------------------------------------------
void KabImplModule::impl_throwKdeTooNewException()
{
    ::rtl::OUStringBuffer aMessage;
    aMessage.appendAscii( "The found KDE version is too new. Only KDE up to version " );
    aMessage.append( (sal_Int32)MAX_KDE_VERSION_MAJOR );
    aMessage.append( (sal_Unicode)'.' );
    aMessage.append( (sal_Int32)MAX_KDE_VERSION_MINOR );
    aMessage.appendAscii( " is known to work with this product.\n" );

    SQLException aError;
    aError.Message = aMessage.makeStringAndClear();
    aError.SQLState = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "S1000" ) );
    aError.ErrorCode = 0;

    SQLContext aDetails;
    aMessage.appendAscii( "If you are sure that your KDE version works, " );
    aMessage.appendAscii( "you might execute the following Basic macro to disable this version check:\n\n" );

    aMessage.appendAscii( "Sub disableKDEMaxVersionCheck\n" );
    aMessage.appendAscii( "  BasicLibraries.LoadLibrary( \"Tools\" )\n" );

    aMessage.appendAscii( "  Dim configNode as Object\n" );
    aMessage.appendAscii( "  configNode = GetRegistryKeyContent( \"" );
    aMessage.append( KabDriver::impl_getConfigurationSettingsPath() );
    aMessage.appendAscii( "\", true )\n" );

    aMessage.appendAscii( "  configNode.DisableKDEMaximumVersionCheck = TRUE\n" );
    aMessage.appendAscii( "  configNode.commitChanges\n" );
    aMessage.appendAscii( "End Sub\n" );

    aDetails.Message = aMessage.makeStringAndClear();

    aError.NextException <<= aDetails;

    throw aError;
}

// --------------------------------------------------------------------------------
KabConnection* KabImplModule::createConnection( KabDriver* _pDriver ) const
{
    OSL_PRECOND( m_hConnectorModule, "KabImplModule::createConnection: not initialized!" );

    void* pUntypedConnection = (*m_pConnectionFactoryFunc)( _pDriver );
    if ( !pUntypedConnection )
        throw RuntimeException();

    return static_cast< KabConnection* >( pUntypedConnection );
}

// --------------------------------------------------------------------------------
void KabImplModule::shutdown()
{
    if ( !m_hConnectorModule )
        return;

    (*m_pApplicationShutdownFunc)();
    m_bAttemptedInitialize = false;

    impl_unloadModule();
}

// =======================================================================
// = KabDriver
// =======================================================================
KabDriver::KabDriver(
    const Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory)
    : KDriver_BASE(m_aMutex),
      m_xMSFactory(_rxFactory),
      m_aImplModule(_rxFactory)
{
    if ( !m_xMSFactory.is() )
        throw NullPointerException();

    osl_incrementInterlockedCount( &m_refCount );
    try
    {
        Reference< XDesktop > xDesktop(
            m_xMSFactory->createInstance( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ) ) ),
            UNO_QUERY_THROW );
        xDesktop->addTerminateListener( this );
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    osl_decrementInterlockedCount( &m_refCount );
}
// --------------------------------------------------------------------------------
void KabDriver::disposing()
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

    WeakComponentImplHelperBase::disposing();
}
// static ServiceInfo
//------------------------------------------------------------------------------
rtl::OUString KabDriver::getImplementationName_Static(  ) throw(RuntimeException)
{
    return rtl::OUString::createFromAscii( impl_getAsciiImplementationName() );
}
//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > KabDriver::getSupportedServiceNames_Static(  ) throw (RuntimeException)
{
    // which service is supported
    // for more information @see com.sun.star.sdbc.Driver
    Sequence< ::rtl::OUString > aSNS( 1 );
    aSNS[0] = ::rtl::OUString::createFromAscii("com.sun.star.sdbc.Driver");

    return aSNS;
}
//------------------------------------------------------------------
::rtl::OUString SAL_CALL KabDriver::getImplementationName(  ) throw(RuntimeException)
{
    return getImplementationName_Static();
}
//------------------------------------------------------------------
sal_Bool SAL_CALL KabDriver::supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException)
{
    Sequence< ::rtl::OUString > aSupported(getSupportedServiceNames());
    const ::rtl::OUString* pSupported = aSupported.getConstArray();
    const ::rtl::OUString* pEnd = pSupported + aSupported.getLength();

    while (pSupported != pEnd && !pSupported->equals(_rServiceName))
        ++pSupported;
    return pSupported != pEnd;
}
//------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL KabDriver::getSupportedServiceNames(  ) throw(RuntimeException)
{
    return getSupportedServiceNames_Static();
}
// --------------------------------------------------------------------------------
Reference< XConnection > SAL_CALL KabDriver::connect( const ::rtl::OUString& url, const Sequence< PropertyValue >& info ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);

    m_aImplModule.init();

    // create a new connection with the given properties and append it to our vector
    KabConnection* pConnection = m_aImplModule.createConnection( this );
    OSL_POSTCOND( pConnection, "KabDriver::connect: no connection has been created by the factory!" );

    // by definition, the factory function returned an object which was acquired once
    Reference< XConnection > xConnection = pConnection;
    pConnection->release();

    // late constructor call which can throw exception and allows a correct dtor call when so
    pConnection->construct( url, info );

    // remember it
    m_xConnections.push_back( WeakReferenceHelper( *pConnection ) );

    return xConnection;
}
// --------------------------------------------------------------------------------
sal_Bool SAL_CALL KabDriver::acceptsURL( const ::rtl::OUString& url )
        throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);

    if ( !m_aImplModule.isKDEPresent() )
        return sal_False;

    // here we have to look whether we support this URL format
    return (!url.compareTo(::rtl::OUString::createFromAscii("sdbc:address:kab:"), 16));
}
// --------------------------------------------------------------------------------
Sequence< DriverPropertyInfo > SAL_CALL KabDriver::getPropertyInfo( const ::rtl::OUString&, const Sequence< PropertyValue >& ) throw(SQLException, RuntimeException)
{
    // if you have something special to say, return it here :-)
    return Sequence< DriverPropertyInfo >();
}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL KabDriver::getMajorVersion(  ) throw(RuntimeException)
{
    return KAB_DRIVER_VERSION_MAJOR;
}
// --------------------------------------------------------------------------------
sal_Int32 SAL_CALL KabDriver::getMinorVersion(  ) throw(RuntimeException)
{
    return KAB_DRIVER_VERSION_MINOR;
}
// --------------------------------------------------------------------------------
void SAL_CALL KabDriver::queryTermination( const EventObject& ) throw (TerminationVetoException, RuntimeException)
{
    // nothing to do, nothing to veto
}
// --------------------------------------------------------------------------------
void SAL_CALL KabDriver::notifyTermination( const EventObject& ) throw (RuntimeException)
{
    m_aImplModule.shutdown();
}
// --------------------------------------------------------------------------------
void SAL_CALL KabDriver::disposing( const EventObject& ) throw (RuntimeException)
{
    // not interested in (this is the disposing of the desktop, if any)
}
// --------------------------------------------------------------------------------
const sal_Char* KabDriver::impl_getAsciiImplementationName()
{
    return "com.sun.star.comp.sdbc.kab.Driver";
        // this name is referenced in the configuration and in the kab.xml
        // Please be careful when changing it.
}
// --------------------------------------------------------------------------------
::rtl::OUString KabDriver::impl_getConfigurationSettingsPath()
{
    ::rtl::OUStringBuffer aPath;
    aPath.appendAscii( "/org.openoffice.Office.DataAccess/DriverSettings/" );
    aPath.appendAscii( "com.sun.star.comp.sdbc.kab.Driver" );
    return aPath.makeStringAndClear();
}
// --------------------------------------------------------------------------------
Reference< XInterface >  SAL_CALL KabDriver::Create( const Reference< XMultiServiceFactory >& _rxFactory ) throw( Exception )
{
    return *(new KabDriver(_rxFactory));
}

