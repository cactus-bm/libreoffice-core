/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dbmetadata.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-26 14:28:09 $
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

#ifndef CONNECTIVITY_INC_CONNECTIVITY_DBMETADATA_HXX
#include <connectivity/dbmetadata.hxx>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATABASEMETADATA2_HPP_
#include <com/sun/star/sdbc/XDatabaseMetaData2.hpp>
#endif
/** === end UNO includes === **/

#include <tools/diagnose_ex.h>
#include <comphelper/namedvaluecollection.hxx>

#include <boost/optional.hpp>

//........................................................................
namespace dbtools
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::sdbc::XConnection;
    using ::com::sun::star::sdbc::XConnection;
    using ::com::sun::star::sdbc::XDatabaseMetaData;
    using ::com::sun::star::sdbc::XDatabaseMetaData2;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::container::XChild;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::beans::XPropertySet;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::beans::PropertyValue;
    using ::com::sun::star::beans::XPropertySetInfo;
    using ::com::sun::star::uno::UNO_QUERY;
    /** === end UNO using === **/

    //====================================================================
    //= DatabaseMetaData_Impl
    //====================================================================
    struct DatabaseMetaData_Impl
    {
        Reference< XConnection >        xConnection;
        Reference< XDatabaseMetaData >  xConnectionMetaData;

        ::boost::optional< ::rtl::OUString >    sCachedIdentifierQuoteString;
        ::boost::optional< ::rtl::OUString >    sCachedCatalogSeparator;
    };

    //--------------------------------------------------------------------
    namespace
    {
        //................................................................
        static void lcl_construct( DatabaseMetaData_Impl& _metaDataImpl, const Reference< XConnection >& _connection )
        {
            _metaDataImpl.xConnection = _connection;
            if ( !_metaDataImpl.xConnection.is() )
                return;

            _metaDataImpl.xConnectionMetaData = _connection->getMetaData();
            if ( !_metaDataImpl.xConnectionMetaData.is() )
                throw IllegalArgumentException();
        }

        //................................................................
        static void lcl_checkConnected( const DatabaseMetaData_Impl& _metaDataImpl )
        {
            if ( !_metaDataImpl.xConnection.is() || !_metaDataImpl.xConnectionMetaData.is() )
                throwSQLException( "not connected", SQL_CONNECTION_DOES_NOT_EXIST, NULL );
        }

        //................................................................
        static bool lcl_getConnectionSettings( const sal_Char* _asciiName, const DatabaseMetaData_Impl& _metaData, Any& _out_setting )
        {
            try
            {
                Reference< XChild > xConnectionAsChild( _metaData.xConnection, UNO_QUERY );
                if ( xConnectionAsChild.is() )
                {
                    Reference< XPropertySet > xDataSource( xConnectionAsChild->getParent(), UNO_QUERY_THROW );
                    Reference< XPropertySet > xDataSourceSettings(
                        xDataSource->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Settings" ) ) ),
                        UNO_QUERY_THROW );

                    _out_setting = xDataSourceSettings->getPropertyValue( ::rtl::OUString::createFromAscii( _asciiName ) );
                }
                else
                {
                    Reference< XDatabaseMetaData2 > xExtendedMetaData( _metaData.xConnectionMetaData, UNO_QUERY_THROW );
                    ::comphelper::NamedValueCollection aSettings( xExtendedMetaData->getConnectionInfo() );
                    _out_setting = aSettings.get( _asciiName );
                    return _out_setting.hasValue();
                }
                return true;
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }
            return false;
        }

        //................................................................
        static const ::rtl::OUString& lcl_getConnectionStringSetting(
            const DatabaseMetaData_Impl& _metaData, ::boost::optional< ::rtl::OUString >& _cachedSetting,
            ::rtl::OUString (SAL_CALL XDatabaseMetaData::*_getter)() )
        {
            lcl_checkConnected( _metaData );

            if ( !_cachedSetting )
            {
                try
                {
                    _cachedSetting.reset( (_metaData.xConnectionMetaData.get()->*_getter)() );
                }
                catch( const Exception& ) { DBG_UNHANDLED_EXCEPTION(); }
            }
            return *_cachedSetting;
        }
    }

    //====================================================================
    //= DatabaseMetaData
    //====================================================================
    //--------------------------------------------------------------------
    DatabaseMetaData::DatabaseMetaData()
        :m_pImpl( new DatabaseMetaData_Impl )
    {
    }

    //--------------------------------------------------------------------
    DatabaseMetaData::DatabaseMetaData( const Reference< XConnection >& _connection )
        :m_pImpl( new DatabaseMetaData_Impl )
    {
        lcl_construct( *m_pImpl, _connection );
    }

    //--------------------------------------------------------------------
    DatabaseMetaData::DatabaseMetaData( const DatabaseMetaData& _copyFrom )
        :m_pImpl( new DatabaseMetaData_Impl( *_copyFrom.m_pImpl ) )
    {
    }

    //--------------------------------------------------------------------
    DatabaseMetaData& DatabaseMetaData::operator=( const DatabaseMetaData& _copyFrom )
    {
        if ( this == &_copyFrom )
            return *this;

        m_pImpl.reset( new DatabaseMetaData_Impl( *_copyFrom.m_pImpl ) );
        return *this;
    }

    //--------------------------------------------------------------------
    DatabaseMetaData::~DatabaseMetaData()
    {
    }

    //--------------------------------------------------------------------
    bool DatabaseMetaData::isConnected() const
    {
        return m_pImpl->xConnection.is();
    }

    //--------------------------------------------------------------------
    bool DatabaseMetaData::supportsSubqueriesInFrom() const
    {
        lcl_checkConnected( *m_pImpl );

        bool supportsSubQueries = false;
        try
        {
            sal_Int32 maxTablesInselect = m_pImpl->xConnectionMetaData->getMaxTablesInSelect();
            supportsSubQueries = ( maxTablesInselect > 1 ) || ( maxTablesInselect == 0 );
            // TODO: is there a better way to determine this? The above is not really true. More precise,
            // it's a *very* generous heuristics ...
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return supportsSubQueries;
    }

    //--------------------------------------------------------------------
    const ::rtl::OUString&  DatabaseMetaData::getIdentifierQuoteString() const
    {
        return lcl_getConnectionStringSetting( *m_pImpl, m_pImpl->sCachedIdentifierQuoteString, &XDatabaseMetaData::getIdentifierQuoteString );
    }

    //--------------------------------------------------------------------
    const ::rtl::OUString&  DatabaseMetaData::getCatalogSeparator() const
    {
        return lcl_getConnectionStringSetting( *m_pImpl, m_pImpl->sCachedCatalogSeparator, &XDatabaseMetaData::getCatalogSeparator );
    }

    //--------------------------------------------------------------------
    bool DatabaseMetaData::restrictIdentifiersToSQL92() const
    {
        lcl_checkConnected( *m_pImpl );

        bool restrict( false );
        Any setting;
        if ( lcl_getConnectionSettings( "EnableSQL92Check", *m_pImpl, setting ) )
            OSL_VERIFY( setting >>= restrict );
        return restrict;
    }

    //--------------------------------------------------------------------
    bool DatabaseMetaData::generateASBeforeCorrelationName() const
    {
        bool doGenerate( true );
        Any setting;
        if ( lcl_getConnectionSettings( "GenerateASBeforeCorrelationName", *m_pImpl, setting ) )
            OSL_VERIFY( setting >>= doGenerate );
        return doGenerate;
    }

    //--------------------------------------------------------------------
    bool DatabaseMetaData::supportsRelations() const
    {
        lcl_checkConnected( *m_pImpl );
        bool bSupport = false;
        try
        {
            bSupport = m_pImpl->xConnectionMetaData->supportsIntegrityEnhancementFacility();
            if ( !bSupport )
            {
                const ::rtl::OUString url = m_pImpl->xConnectionMetaData->getURL();
                char pMySQL[] = "sdbc:mysql:";
                bSupport = url.matchAsciiL(pMySQL,(sizeof(pMySQL)/sizeof(pMySQL[0]))-1);
            }
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return bSupport;
    }

//........................................................................
} // namespace dbtools
//........................................................................

