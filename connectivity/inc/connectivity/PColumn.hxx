/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PColumn.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 04:57:57 $
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
#ifndef _CONNECTIVITY_PCOLUMN_HXX_
#define _CONNECTIVITY_PCOLUMN_HXX_

#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/sdbcx/VColumn.hxx"
#endif

namespace connectivity
{
    namespace parse
    {
        class OParseColumn;

        typedef sdbcx::OColumn OParseColumn_BASE;
        typedef ::comphelper::OIdPropertyArrayUsageHelper<OParseColumn> OParseColumn_PROP;

        class OParseColumn :    public OParseColumn_BASE,
                                public OParseColumn_PROP
        {
            ::rtl::OUString m_aRealName;
            ::rtl::OUString m_aTableName;
            sal_Bool        m_bFunction;
            sal_Bool        m_bDbasePrecisionChanged;
            sal_Bool        m_bAggregateFunction;
        protected:
            virtual ::cppu::IPropertyArrayHelper* createArrayHelper( sal_Int32 _nId) const;
            virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

            virtual ~OParseColumn();
        public:
            OParseColumn(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _xColumn,sal_Bool _bCase);
            OParseColumn(const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _TypeName,
                    const ::rtl::OUString& _DefaultValue,
                    sal_Int32       _IsNullable,
                    sal_Int32       _Precision,
                    sal_Int32       _Scale,
                    sal_Int32       _Type,
                    sal_Bool        _IsAutoIncrement,
                    sal_Bool        _IsCurrency,
                    sal_Bool        _bCase);

            virtual void construct();

            void setRealName(const ::rtl::OUString& _rName)  { m_aRealName  = _rName; }
            void setTableName(const ::rtl::OUString& _rName) { m_aTableName = _rName; }
            void setFunction(sal_Bool _bFunction)            { m_bFunction  = _bFunction; }
            void setAggregateFunction(sal_Bool _bFunction)   { m_bAggregateFunction = _bFunction; }
            void setDbasePrecisionChanged(sal_Bool _bDbasePrecisionChanged) { m_bDbasePrecisionChanged = _bDbasePrecisionChanged; }

            ::rtl::OUString getRealName() const { return  m_aRealName; }
            ::rtl::OUString getTableName() const { return  m_aTableName; }
            sal_Bool        getFunction() const { return  m_bFunction; }
            sal_Bool        getDbasePrecisionChanged()  const { return  m_bDbasePrecisionChanged; }
        };

        class OOrderColumn;

        typedef sdbcx::OColumn OOrderColumn_BASE;
        typedef ::comphelper::OIdPropertyArrayUsageHelper<OOrderColumn> OOrderColumn_PROP;

        class OOrderColumn :    public OOrderColumn_BASE,
                                public OOrderColumn_PROP
        {
            sal_Bool        m_bAscending;
            sal_Bool        m_bOrder;
        protected:
            virtual ::cppu::IPropertyArrayHelper* createArrayHelper( sal_Int32 _nId) const;
            virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

            virtual ~OOrderColumn();
        public:
            OOrderColumn(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _xColumn,sal_Bool _bCase,sal_Bool _bAscending);
            OOrderColumn(const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _TypeName,
                    const ::rtl::OUString& _DefaultValue,
                    sal_Int32       _IsNullable,
                    sal_Int32       _Precision,
                    sal_Int32       _Scale,
                    sal_Int32       _Type,
                    sal_Bool        _IsAutoIncrement,
                    sal_Bool        _IsCurrency,
                    sal_Bool        _bCase
                    ,sal_Bool _bAscending);

            virtual void construct();

            virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);
        };
    }
}

#endif //_CONNECTIVITY_PCOLUMN_HXX_

