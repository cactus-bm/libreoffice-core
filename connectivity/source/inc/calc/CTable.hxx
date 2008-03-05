/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: CTable.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-05 16:32:10 $
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

#ifndef _CONNECTIVITY_CALC_TABLE_HXX_
#define _CONNECTIVITY_CALC_TABLE_HXX_

#ifndef _CONNECTIVITY_FILE_TABLE_HXX_
#include "file/FTable.hxx"
#endif
#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif

namespace com { namespace sun { namespace star { namespace sheet {
    class XSpreadsheet;
    class XSpreadsheetDocument;
} } } }

namespace com { namespace sun { namespace star { namespace util {
    class XNumberFormats;
} } } }


namespace connectivity
{
    namespace calc
    {
        typedef file::OFileTable OCalcTable_BASE;
        class OCalcConnection;

        typedef ::std::map< ::rtl::OUString,
                        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed>,
                        comphelper::UStringMixLess > OContainer;

        class OCalcTable :  public OCalcTable_BASE
        {
        private:
            ::std::vector<sal_Int32> m_aTypes;      // holds all type for columns just to avoid to ask the propertyset
            ::std::vector<sal_Int32> m_aPrecisions; // same as aboth
            ::std::vector<sal_Int32> m_aScales;
            ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet >           m_xSheet;
            OCalcConnection* m_pConnection;
            sal_Int32 m_nStartCol;
            sal_Int32 m_nStartRow;
            sal_Int32 m_nDataCols;
            sal_Int32 m_nDataRows;
            sal_Bool m_bHasHeaders;
            ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormats > m_xFormats;
            ::Date m_aNullDate;

            void fillColumns();

        protected:
            virtual void FileClose();
        public:
            virtual void refreshColumns();
            virtual void refreshIndexes();

        public:
            OCalcTable( sdbcx::OCollection* _pTables,OCalcConnection* _pConnection);
            OCalcTable( sdbcx::OCollection* _pTables,OCalcConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description = ::rtl::OUString(),
                    const ::rtl::OUString& _SchemaName = ::rtl::OUString(),
                    const ::rtl::OUString& _CatalogName = ::rtl::OUString()
                );

            virtual sal_Int32 getCurrentLastPos() const;
            virtual sal_Bool seekRow(IResultSetHelper::Movement eCursorPosition, sal_Int32 nOffset, sal_Int32& nCurPos);
            virtual sal_Bool fetchRow(OValueRefRow& _rRow,const OSQLColumns& _rCols, sal_Bool _bUseTableDefs,sal_Bool bRetrieveData);

            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL disposing(void);

            // com::sun::star::lang::XUnoTunnel
            virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);
            static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();

            void construct();
        };
    }
}

#endif // _CONNECTIVITY_CALC_TABLE_HXX_

