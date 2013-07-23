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

#ifndef CONNECTIVITY_FIREBIRD_STATEMENT_BASE_HXX
#define CONNECTIVITY_FIREBIRD_STATEMENT_BASE_HXX

#include "FConnection.hxx"

#include <ibase.h>

#include <connectivity/OSubComponent.hxx>
#include <cppuhelper/compbase4.hxx>
#include <list>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/sdbc/SQLWarning.hpp>
#include <com/sun/star/sdbc/XBatchExecution.hpp>
#include <com/sun/star/sdbc/XCloseable.hpp>
#include <com/sun/star/sdbc/XMultipleResults.hpp>
#include <com/sun/star/sdbc/XStatement.hpp>
#include <com/sun/star/sdbc/XWarningsSupplier.hpp>
#include <com/sun/star/util/XCancellable.hpp>

namespace connectivity
{
    namespace firebird
    {

        typedef ::cppu::WeakComponentImplHelper4<   ::com::sun::star::sdbc::XWarningsSupplier,
                                                    ::com::sun::star::util::XCancellable,
                                                    ::com::sun::star::sdbc::XCloseable,
                                                    ::com::sun::star::sdbc::XMultipleResults> OStatementCommonBase_Base;

        class OStatementCommonBase  :   public  OStatementCommonBase_Base,
                                        public  ::cppu::OPropertySetHelper,
                                        public  OPropertyArrayUsageHelper<OStatementCommonBase>

        {
        protected:
            ::com::sun::star::uno::WeakReference< ::com::sun::star::sdbc::XResultSet>    m_xResultSet;   // The last ResultSet created
            //  for this Statement

            ::std::list< ::rtl::OUString>               m_aBatchList;

            OConnection*                                m_pConnection;

            ISC_STATUS_ARRAY                            m_statusVector;
        protected:

            void disposeResultSet();

            // OPropertyArrayUsageHelper
            virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;
            // OPropertySetHelper
            using OPropertySetHelper::getFastPropertyValue;
            virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();
            virtual sal_Bool SAL_CALL convertFastPropertyValue(
                                                                ::com::sun::star::uno::Any & rConvertedValue,
                                                                ::com::sun::star::uno::Any & rOldValue,
                                                                sal_Int32 nHandle,
                                                                const ::com::sun::star::uno::Any& rValue )
                                                            throw (::com::sun::star::lang::IllegalArgumentException);
            virtual void SAL_CALL setFastPropertyValue_NoBroadcast(
                                                                sal_Int32 nHandle,
                                                                const ::com::sun::star::uno::Any& rValue)   throw (::com::sun::star::uno::Exception);
            virtual void SAL_CALL getFastPropertyValue(
                                                                ::com::sun::star::uno::Any& rValue,
                                                                sal_Int32 nHandle) const;
            virtual ~OStatementCommonBase();
            int prepareAndDescribeStatement(const OUString& sqlIn,
                                                  isc_stmt_handle& aStatementHandle,
                                                  XSQLDA*& pOutSqlda,
                                                  XSQLDA* pInSqlda=0);
            ::rtl::OUString sanitizeSqlString(const OUString& sqlIn);
        public:

            ::cppu::OBroadcastHelper& rBHelper;
            OStatementCommonBase(OConnection* _pConnection);
            using OStatementCommonBase_Base::operator ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >;

            // OComponentHelper
            virtual void SAL_CALL disposing(void){OStatementCommonBase_Base::disposing();}
            // XInterface
            virtual void SAL_CALL release() throw();
            virtual void SAL_CALL acquire() throw();
            // XInterface
            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);

            // XPropertySet
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);

            // XWarningsSupplier - UNSUPPORTED
            virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL clearWarnings(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            // XMultipleResults - UNSUPPORTED
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL getResultSet(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Int32 SAL_CALL getUpdateCount(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Bool SAL_CALL getMoreResults(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);

            // XCancellable
            virtual void SAL_CALL cancel(  ) throw(::com::sun::star::uno::RuntimeException);
            // XCloseable
            virtual void SAL_CALL close(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);

            // other methods
            OConnection* getOwnConnection() const { return m_pConnection;}

        };
    }
}

#endif //CONNECTIVITY_FIREBIRD_STATEMENT_BASE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
