/*************************************************************************
 *
 *  $RCSfile: FTable.hxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: oj $ $Date: 2002-07-05 08:07:50 $
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

#ifndef _CONNECTIVITY_FILE_TABLE_HXX_
#define _CONNECTIVITY_FILE_TABLE_HXX_

#ifndef _CONNECTIVITY_SDBCX_TABLE_HXX_
#include "connectivity/sdbcx/VTable.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATABASEMETADATA_HPP_
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _CONNECTIVITY_FILE_BCONNECTION_HXX_
#include "file/FConnection.hxx"
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _CONNECTIVITY_FILE_VALUE_HXX_
#include "connectivity/FValue.hxx"
#endif
#ifndef CONNECTIVITY_TRESULTSETHELPER_HXX
#include "TResultSetHelper.hxx"
#endif

namespace connectivity
{
    namespace file
    {
        typedef connectivity::sdbcx::OTable OTable_TYPEDEF;

        class OFileTable :  public OTable_TYPEDEF
        {
        protected:
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >   m_xMetaData;
            OConnection*                                        m_pConnection;
            SvStream*                                           m_pFileStream;
            ::vos::ORef<OSQLColumns>                            m_aColumns;
            sal_Int32                                           m_nFilePos;                 // aktuelle IResultSetHelper::Movement
            sal_uInt8*                                          m_pBuffer;
            sal_uInt16                                          m_nBufferSize;  // Groesse des ReadBuffer, wenn pBuffer != NULL
            sal_Bool                                            m_bWriteable;   // svstream cann't say if we are writeable
                                                                                // so we have to

            virtual void FileClose();
            virtual ~OFileTable( );
        public:
            virtual void refreshColumns();
            virtual void refreshKeys();
            virtual void refreshIndexes();
        public:
            OFileTable( sdbcx::OCollection* _pTables,OConnection* _pConnection);
            OFileTable( sdbcx::OCollection* _pTables,OConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description = ::rtl::OUString(),
                    const ::rtl::OUString& _SchemaName = ::rtl::OUString(),
                    const ::rtl::OUString& _CatalogName = ::rtl::OUString()
                );

            //XInterface
            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL acquire() throw();
            virtual void SAL_CALL release() throw();
            // ::cppu::OComponentHelper
            virtual void SAL_CALL disposing(void);

            OConnection* getConnection() const { return m_pConnection;}
            virtual sal_Int32 getCurrentLastPos() const {return -1;}

            virtual sal_Bool seekRow(IResultSetHelper::Movement eCursorPosition, sal_Int32 nOffset, sal_Int32& nCurPos) = 0;
            virtual sal_Bool fetchRow(OValueRow _rRow,const OSQLColumns& _rCols, sal_Bool _bUseTableDefs,sal_Bool bRetrieveData) = 0;

            ::vos::ORef<OSQLColumns> getTableColumns() const {return m_aColumns;}
            virtual BOOL InsertRow(OValueVector& rRow, BOOL bFlush,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);
            virtual BOOL DeleteRow(const OSQLColumns& _rCols);
            virtual BOOL UpdateRow(OValueVector& rRow, OValueRow pOrgRow,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);
            virtual void addColumn(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& descriptor);
            virtual void dropColumn(sal_Int32 _nPos);
            // refresh the header of file based tables to see changes done by someone
            virtual void refreshHeader();

            ::rtl::OUString SAL_CALL getName() throw() { return m_Name; }

            ::rtl::OUString getSchema() { return m_SchemaName; }
            sal_Bool isReadOnly() const { return !m_bWriteable; }
                // m_pFileStream && !m_pFileStream->IsWritable(); }
            // com::sun::star::lang::XUnoTunnel
            virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);
            static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();


            sal_Int32 getFilePos() const { return m_nFilePos; }

        public:
            // helper

            // creates a stream using ::utl::UcbStreamHelper::CreateStream, but the error is simplified
            // (NULL or non-NULL is returned)
            static SvStream* createStream_simpleError( const String& _rFileName, StreamMode _eOpenMode);
        };
    }
}
#endif // _CONNECTIVITY_FILE_TABLE_HXX_

