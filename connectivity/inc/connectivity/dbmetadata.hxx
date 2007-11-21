/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dbmetadata.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: ihi $ $Date: 2007-11-21 14:57:02 $
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

#ifndef CONNECTIVITY_INC_CONNECTIVITY_DBMETADATA_HXX
#define CONNECTIVITY_INC_CONNECTIVITY_DBMETADATA_HXX

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
/** === end UNO includes === **/

#include <memory>

//........................................................................
namespace dbtools
{
//........................................................................

    //====================================================================
    //= DatabaseMetaData
    //====================================================================
    struct DatabaseMetaData_Impl;
    /** encapsulates meta data about a database/connection which cannot be obtained
        from the usual XDatabaseMetaData result set.

        Meta data perhaps isn't really the right term ... Some of the methods
        in this class involved heuristics, some are just a convenient wrapper
        around more complex ways to obtain the same information.

        @todo
            Once CWS dba30 is integrated, we could easily add all the meta data
            which is part of the "Info" property of a data source.
    */
    class DatabaseMetaData
    {
    private:
        ::std::auto_ptr< DatabaseMetaData_Impl >    m_pImpl;

    public:
        DatabaseMetaData();
        /** constructs a DatabaseMetaData instance
            @param  _rxConnection
                is the connection whose meta data you're interested in.
                Note that some of the information provided by this class can only be obtained
                if this connection denotes an application-level connection, i.e. supports
                the com.sun.star.sdb.Connection service.

            @throws ::com::sun::star::lang::IllegalArgumentException
                if the given connection is not <NULL/>, but the XDatabaseMetaData provided by it
                are <NULL/>
            @throws ::com::sun::star::sdbc::SQLException
                if obtaining the meta data from the connection throws an SQLException
            @throws ::com::sun::star::uno::RuntimeException
                if obtaining the meta data from the connection throws an RuntimeException
        */
        DatabaseMetaData(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _connection );
        DatabaseMetaData( const DatabaseMetaData& _copyFrom );
        DatabaseMetaData& operator=( const DatabaseMetaData& _copyFrom );

        ~DatabaseMetaData();

    public:
        /** determines whether or not the instances is based on a valid connection

            As long as this method returns true<TRUE/>, you should expect all other
            methods throwing an SQLException when called.
        */
        bool    isConnected() const;

        /** resets the instance so that it's based on a new connection
        */
        inline  void    reset( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _connection )
        {
            *this = DatabaseMetaData( _connection );
        }

        /// wraps XDatabaseMetaData::getIdentifierQuoteString
        const ::rtl::OUString&  getIdentifierQuoteString() const;

        /// wraps XDatabaseMetaData::getCatalogSeparator
        const ::rtl::OUString&  getCatalogSeparator() const;

        /** determines whether the database supports sub queries in the FROM part
            of a SELECT clause are supported.
            @throws ::com::sun::star::sdbc::SQLException
                with SQLState 08003 (connection does not exist) if the instances was
                default-constructed and does not have a connection, yet.
        */
        bool supportsSubqueriesInFrom() const;

        /** determines whether names in the database should be restricted to SQL-92 identifiers

            Effectively, this method checks the EnableSQL92Check property of the data source settings,
            if present.
        */
        bool restrictIdentifiersToSQL92() const;

        /** determines whether when generating SQL statements, an AS keyword should be generated
            before a correlation name.

            E.g., it determines whether <code>SELECT * FROM table AS correlation_name</code> or
            <code>SELECT * FROM table correlation_name</code> is generated.
        */
        bool generateASBeforeCorrelationName() const;

        /** determines the syntax to use for boolean comparison predicates

            @see ::com::sun::star::sdb::BooleanComparisonMode
        */
        sal_Int32
            getBooleanComparisonMode() const;

        /**
        *
        * \return <TRUE/> when relations are supported, otherwise <FALSE/>
        */
        bool supportsRelations() const;
    };

//........................................................................
} // namespace dbtools
//........................................................................

#endif // CONNECTIVITY_INC_CONNECTIVITY_DBMETADATA_HXX
