/*************************************************************************
 *
 *  $RCSfile: ftpresultsetbase.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: abi $ $Date: 2002-07-31 15:13:35 $
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
#ifndef _FTP_FTPRESULTSETBASE_HXX_
#define _FTP_FTPRESULTSETBASE_HXX_

#include <vector>
#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTACCESS_HPP_
#include <com/sun/star/ucb/XContentAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCLOSEABLE_HPP_
#include <com/sun/star/sdbc/XCloseable.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETMETADATASUPPLIER_HPP_
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_NUMBEREDSORTINGINFO_HPP_
#include <com/sun/star/ucb/NumberedSortingInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTPROVIDER_HPP_
#include <com/sun/star/ucb/XContentProvider.hpp>
#endif
#ifndef  _COM_SUN_STAR_UCB_XCONTENTIDENTIFIER_HPP_
#include <com/sun/star/ucb/XContentIdentifier.hpp>
#endif
#ifndef  _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTY_HPP_
#include <com/sun/star/beans/Property.hpp>
#endif


namespace ftp {

    class ResultSetBase
        : public cppu::OWeakObject,
          public com::sun::star::lang::XComponent,
          public com::sun::star::sdbc::XRow,
          public com::sun::star::sdbc::XResultSet,
          public com::sun::star::sdbc::XCloseable,
          public com::sun::star::sdbc::XResultSetMetaDataSupplier,
          public com::sun::star::beans::XPropertySet,
          public com::sun::star::ucb::XContentAccess
    {
    public:

        ResultSetBase(const com::sun::star::uno::Reference<
                      com::sun::star::lang::XMultiServiceFactory >&  xMSF,
                      const com::sun::star::uno::Reference<
                      com::sun::star::ucb::XContentProvider >&  xProvider,
                      sal_Int32 nOpenMode,
                      const com::sun::star::uno::Sequence<
                      com::sun::star::beans::Property >& seq,
                      const com::sun::star::uno::Sequence<
                      com::sun::star::ucb::NumberedSortingInfo >& seqSort);

        virtual ~ResultSetBase();

        // XInterface
        virtual com::sun::star::uno::Any SAL_CALL
        queryInterface(
            const com::sun::star::uno::Type& aType )
            throw( com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        acquire(
            void )
            throw();

        virtual void SAL_CALL
        release(
            void )
            throw();

        // XComponent
        virtual void SAL_CALL
        dispose(
            void )
            throw( com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL
        addEventListener(
            const com::sun::star::uno::Reference<
            com::sun::star::lang::XEventListener >& xListener )
            throw( com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL
        removeEventListener( const com::sun::star::uno::Reference<
                             com::sun::star::lang::XEventListener >& aListener )
            throw( com::sun::star::uno::RuntimeException );


        // XRow
        virtual sal_Bool SAL_CALL
        wasNull(
            void )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException )
        {
            if( 0<= m_nRow && m_nRow < m_aItems.size() )
                m_nWasNull = m_aItems[m_nRow]->wasNull();
            else
                m_nWasNull = true;
            return m_nWasNull;
        }

        virtual rtl::OUString SAL_CALL
        getString(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getString( columnIndex );
            else
                return rtl::OUString();
        }

        virtual sal_Bool SAL_CALL
        getBoolean(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getBoolean( columnIndex );
            else
                return false;
        }

        virtual sal_Int8 SAL_CALL
        getByte(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getByte( columnIndex );
            else
                return sal_Int8( 0 );
        }

        virtual sal_Int16 SAL_CALL
        getShort(
            sal_Int32 columnIndex )
            throw(
                com::sun::star::sdbc::SQLException,
                com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getShort( columnIndex );
            else
                return sal_Int16( 0 );
        }

        virtual sal_Int32 SAL_CALL
        getInt(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException )
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getInt( columnIndex );
            else
                return sal_Int32( 0 );
        }

        virtual sal_Int64 SAL_CALL
        getLong(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getLong( columnIndex );
            else
                return sal_Int64( 0 );
        }

        virtual float SAL_CALL
        getFloat(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException )
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getFloat( columnIndex );
            else
                return float( 0 );
        }

        virtual double SAL_CALL
        getDouble(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException )
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getDouble( columnIndex );
            else
                return double( 0 );
        }

        virtual com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL
        getBytes(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException )
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getBytes( columnIndex );
            else
                return com::sun::star::uno::Sequence< sal_Int8 >();
        }

        virtual com::sun::star::util::Date SAL_CALL
        getDate(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getDate( columnIndex );
            else
                return com::sun::star::util::Date();
        }

        virtual com::sun::star::util::Time SAL_CALL
        getTime(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getTime( columnIndex );
            else
                return com::sun::star::util::Time();
        }

        virtual com::sun::star::util::DateTime SAL_CALL
        getTimestamp(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getTimestamp( columnIndex );
            else
                return com::sun::star::util::DateTime();
        }


        virtual com::sun::star::uno::Reference<
        com::sun::star::io::XInputStream > SAL_CALL
        getBinaryStream(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getBinaryStream( columnIndex );
            else
                return com::sun::star::uno::Reference<
                    com::sun::star::io::XInputStream >();
        }

        virtual com::sun::star::uno::Reference<
        com::sun::star::io::XInputStream > SAL_CALL
        getCharacterStream(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getCharacterStream( columnIndex );
            else
                return com::sun::star::uno::Reference<
                    com::sun::star::io::XInputStream >();
        }

        virtual com::sun::star::uno::Any SAL_CALL
        getObject(
            sal_Int32 columnIndex,
            const com::sun::star::uno::Reference<
            com::sun::star::container::XNameAccess >& typeMap )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getObject( columnIndex,typeMap );
            else
                return com::sun::star::uno::Any();
        }

        virtual com::sun::star::uno::Reference<
        com::sun::star::sdbc::XRef > SAL_CALL
        getRef(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getRef( columnIndex );
            else
                return com::sun::star::uno::Reference< com::sun::star::sdbc::XRef >();
        }

        virtual com::sun::star::uno::Reference<
        com::sun::star::sdbc::XBlob > SAL_CALL
        getBlob(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getBlob( columnIndex );
            else
                return com::sun::star::uno::Reference< com::sun::star::sdbc::XBlob >();
        }

        virtual com::sun::star::uno::Reference<
        com::sun::star::sdbc::XClob > SAL_CALL
        getClob(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getClob( columnIndex );
            else
                return com::sun::star::uno::Reference<
                    com::sun::star::sdbc::XClob >();
        }

        virtual com::sun::star::uno::Reference<
        com::sun::star::sdbc::XArray > SAL_CALL
        getArray(
            sal_Int32 columnIndex )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException)
        {
            if( 0 <= m_nRow && m_nRow < m_aItems.size() )
                return m_aItems[m_nRow]->getArray( columnIndex );
            else
                return com::sun::star::uno::Reference<
                    com::sun::star::sdbc::XArray >();
        }


        // XResultSet

        virtual sal_Bool SAL_CALL
        next(
            void )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        isBeforeFirst(
            void )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        isAfterLast(
            void )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        isFirst(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        isLast(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        beforeFirst(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        afterLast(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        first(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        last(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Int32 SAL_CALL
        getRow(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        absolute(
            sal_Int32 row )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        relative(
            sal_Int32 rows )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        previous(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        refreshRow(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        rowUpdated(
            void )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        rowInserted(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL
        rowDeleted(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);


        virtual  com::sun::star::uno::Reference<
        com::sun::star::uno::XInterface > SAL_CALL
        getStatement(
            void  )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        // XCloseable

        virtual void SAL_CALL
        close(
            void )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);

        // XContentAccess

        virtual rtl::OUString SAL_CALL
        queryContentIdentifierString(
            void )
            throw( com::sun::star::uno::RuntimeException );

        virtual com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentIdentifier > SAL_CALL
        queryContentIdentifier(
            void )
            throw( com::sun::star::uno::RuntimeException );

        virtual com::sun::star::uno::Reference<
        com::sun::star::ucb::XContent > SAL_CALL
        queryContent(
            void )
            throw( com::sun::star::uno::RuntimeException );

        // XResultSetMetaDataSupplier
        virtual com::sun::star::uno::Reference<
        com::sun::star::sdbc::XResultSetMetaData > SAL_CALL
        getMetaData(
            void )
            throw( com::sun::star::sdbc::SQLException,
                   com::sun::star::uno::RuntimeException);


        // XPropertySet
        virtual com::sun::star::uno::Reference<
        com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo()
            throw( com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL setPropertyValue(
            const rtl::OUString& aPropertyName,
            const com::sun::star::uno::Any& aValue )
            throw( com::sun::star::beans::UnknownPropertyException,
                   com::sun::star::beans::PropertyVetoException,
                   com::sun::star::lang::IllegalArgumentException,
                   com::sun::star::lang::WrappedTargetException,
                   com::sun::star::uno::RuntimeException);

        virtual com::sun::star::uno::Any SAL_CALL
        getPropertyValue(
            const rtl::OUString& PropertyName )
            throw( com::sun::star::beans::UnknownPropertyException,
                   com::sun::star::lang::WrappedTargetException,
                   com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        addPropertyChangeListener(
            const rtl::OUString& aPropertyName,
            const com::sun::star::uno::Reference<
            com::sun::star::beans::XPropertyChangeListener >& xListener )
            throw( com::sun::star::beans::UnknownPropertyException,
                   com::sun::star::lang::WrappedTargetException,
                   com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        removePropertyChangeListener(
            const rtl::OUString& aPropertyName,
            const com::sun::star::uno::Reference<
            com::sun::star::beans::XPropertyChangeListener >& aListener )
            throw( com::sun::star::beans::UnknownPropertyException,
                   com::sun::star::lang::WrappedTargetException,
                   com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        addVetoableChangeListener(
            const rtl::OUString& PropertyName,
            const com::sun::star::uno::Reference<
            com::sun::star::beans::XVetoableChangeListener >& aListener )
            throw( com::sun::star::beans::UnknownPropertyException,
                   com::sun::star::lang::WrappedTargetException,
                   com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL removeVetoableChangeListener(
            const rtl::OUString& PropertyName,
            const com::sun::star::uno::Reference<
            com::sun::star::beans::XVetoableChangeListener >& aListener )
            throw( com::sun::star::beans::UnknownPropertyException,
                   com::sun::star::lang::WrappedTargetException,
                   com::sun::star::uno::RuntimeException);

    protected:

        com::sun::star::uno::Reference<
        com::sun::star::lang::XMultiServiceFactory >  m_xMSF;
        com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentProvider >  m_xProvider;
        sal_uInt32                          m_nRow;
        sal_Bool                            m_nWasNull;
        sal_Int32                           m_nOpenMode;
        sal_Bool                            m_bRowCountFinal;

        typedef std::vector< com::sun::star::uno::Reference<
        com::sun::star::ucb::XContentIdentifier > > IdentSet;
        typedef std::vector< com::sun::star::uno::Reference<
        com::sun::star::sdbc::XRow > >              ItemSet;
        typedef std::vector< rtl::OUString >
        PathSet;

        IdentSet                            m_aIdents;
        ItemSet                             m_aItems;
        PathSet                             m_aPath;

        com::sun::star::uno::Sequence<
        com::sun::star::beans::Property >           m_sProperty;
        com::sun::star::uno::Sequence<
        com::sun::star::ucb::NumberedSortingInfo >  m_sSortingInfo;

        osl::Mutex                          m_aMutex;
        cppu::OInterfaceContainerHelper*    m_pDisposeEventListeners;

        cppu::OInterfaceContainerHelper*    m_pRowCountListeners;
        cppu::OInterfaceContainerHelper*    m_pIsFinalListeners;
    };


} // end namespace fileaccess


#endif
