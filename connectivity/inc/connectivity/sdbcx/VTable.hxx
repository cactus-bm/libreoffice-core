/*************************************************************************
 *
 *  $RCSfile: VTable.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: oj $ $Date: 2001-04-30 10:13:37 $
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

#ifndef _CONNECTIVITY_SDBCX_TABLE_HXX_
#define _CONNECTIVITY_SDBCX_TABLE_HXX_

#ifndef _COM_SUN_STAR_SDBCX_XDATADESCRIPTORFACTORY_HPP_
#include <com/sun/star/sdbcx/XDataDescriptorFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XINDEXESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XIndexesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XRENAME_HPP_
#include <com/sun/star/sdbcx/XRename.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XALTERTABLE_HPP_
#include <com/sun/star/sdbcx/XAlterTable.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XKEYSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XKeysSupplier.hpp>
#endif
#ifndef COMPHELPER_IDPROPERTYARRAYUSAGEHELPER_HXX
#include <comphelper/IdPropArrayHelper.hxx>
#endif
#ifndef _CPPUHELPER_COMPBASE4_HXX_
#include <cppuhelper/compbase4.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif
#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _CONNECTIVITY_SDBCX_IREFRESHABLE_HXX_
#include "connectivity/sdbcx/IRefreshable.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_DESCRIPTOR_HXX_
#include "connectivity/sdbcx/VDescriptor.hxx"
#endif
#ifndef _CONNECTIVITY_COMMONTOOLS_HXX_
#include "connectivity/CommonTools.hxx"
#endif

namespace connectivity
{
    namespace sdbcx
    {

        class OTable;
        class OCollection;

        typedef ::cppu::WeakComponentImplHelper4<   ::com::sun::star::sdbcx::XColumnsSupplier,
                                                    ::com::sun::star::sdbcx::XKeysSupplier,
                                                    ::com::sun::star::container::XNamed,
                                                    ::com::sun::star::lang::XServiceInfo> OTableDescriptor_BASE;

        typedef ::cppu::ImplHelper4<                ::com::sun::star::sdbcx::XDataDescriptorFactory,
                                                    ::com::sun::star::sdbcx::XIndexesSupplier,
                                                    ::com::sun::star::sdbcx::XRename,
                                                    ::com::sun::star::sdbcx::XAlterTable > OTable_BASE;

        typedef ::comphelper::OIdPropertyArrayUsageHelper<OTable> OTable_PROP;


        class OTable :           public comphelper::OBaseMutex,
                                 public OTable_BASE,
                                 public OTableDescriptor_BASE,
                                 public IRefreshableColumns,
                                 public OTable_PROP,
                                 public ODescriptor
        {
        protected:
            ::rtl::OUString m_CatalogName;
            ::rtl::OUString m_SchemaName;
            ::rtl::OUString m_Description;
            ::rtl::OUString m_Type;

            OCollection*    m_pKeys;
            OCollection*    m_pColumns;
            OCollection*    m_pIndexes;

            using OTableDescriptor_BASE::rBHelper;

            // OPropertyArrayUsageHelper
            virtual ::cppu::IPropertyArrayHelper* createArrayHelper(sal_Int32 _nId ) const;
            // OPropertySetHelper
            virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();
        public:
            OTable(sal_Bool _bCase);
            OTable( sal_Bool _bCase,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description = ::rtl::OUString(),
                    const ::rtl::OUString& _SchemaName  = ::rtl::OUString(),
                    const ::rtl::OUString& _CatalogName = ::rtl::OUString());

            virtual ~OTable();

            DECLARE_SERVICE_INFO();
            //XInterface
            virtual void    SAL_CALL acquire() throw(::com::sun::star::uno::RuntimeException);
            virtual void    SAL_CALL release() throw(::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);

            // ODescriptor
            virtual void construct();
            virtual void refreshColumns();
            virtual void refreshKeys();
            virtual void refreshIndexes();
            // ::cppu::OComponentHelper
            virtual void SAL_CALL disposing(void);
            // XPropertySet
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
            // XColumnsSupplier
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getColumns(  ) throw(::com::sun::star::uno::RuntimeException);
            // XKeysSupplier
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL getKeys(  ) throw(::com::sun::star::uno::RuntimeException);
            // XNamed
            virtual ::rtl::OUString SAL_CALL getName() throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setName( const ::rtl::OUString& aName ) throw(::com::sun::star::uno::RuntimeException);
            // XDataDescriptorFactory
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL createDataDescriptor( void ) throw(::com::sun::star::uno::RuntimeException);
            // XIndexesSupplier
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getIndexes(  ) throw(::com::sun::star::uno::RuntimeException);
            // XRename
            virtual void SAL_CALL rename( const ::rtl::OUString& newName ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException);
            // XAlterTable
            virtual void SAL_CALL alterColumnByName( const ::rtl::OUString& colName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL alterColumnByIndex( sal_Int32 index, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        };
    }
}

#endif // _CONNECTIVITY_SDBCX_TABLE_HXX_

