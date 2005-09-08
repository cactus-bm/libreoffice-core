/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: MABTable.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 07:31:37 $
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

#ifndef _CONNECTIVITY_MAB_TABLE_HXX_
#define _CONNECTIVITY_MAB_TABLE_HXX_

#ifndef _CONNECTIVITY_FILE_TABLE_HXX_
#include "file/FTable.hxx"
#endif
#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif

#include <mozaddressbook/MABQuery.hxx>

#ifdef DARREN_WORK
namespace com { namespace sun { namespace star { namespace sheet {
    class XSpreadsheet;
} } } }

namespace com { namespace sun { namespace star { namespace util {
    class XNumberFormats;
} } } }

#endif /* DARREN_WORK */

namespace connectivity
{
    namespace mozaddressbook
    {
        typedef file::OFileTable OMozabTable_BASE;
        class OMozabConnection;

        typedef ::std::map< ::rtl::OUString,
                        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed>,
                        comphelper::UStringMixLess > OContainer;
        static sal_Int32 const m_nNR_OF_FIELDS = 37;

        class OMozabTable : public OMozabTable_BASE
        {
        private:
            //static sal_Int32 const m_nNR_OF_FIELDS = 37;
            const OMozabColumnAlias & m_aColumnAlias;
            OMozabQuery             *m_xQuery;
            ::std::vector<sal_Int32> m_aTypes;      // holds all type for columns just to avoid to ask the propertyset
            ::std::vector<sal_Int32> m_aPrecisions; // same as aboth
            ::std::vector<sal_Int32> m_aScales;
#ifdef DARREN_WORK
            ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > m_xSheet;
            ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormats > m_xFormats;
            ::Date m_aNullDate;
#endif /* DARREN_WORK */
            ::std::vector< ::rtl::OUString> m_aAttributeStrings;

            void fillColumns();

            BOOL WriteBuffer();
            BOOL UpdateBuffer(connectivity::OValueVector& rRow, connectivity::OValueRow pOrgRow,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);
            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> isUniqueByColumnName(const ::rtl::OUString& _rColName);
            OValueRow                               m_aParameterRow;

            sal_Int32   sParamIndex;

            inline void resetParameters()
                          { sParamIndex = 0; }

            void parseParameter( const OSQLParseNode* pNode, rtl::OUString& rMatchString );

        protected:
            virtual void FileClose();

            // special ctor is needed for clone
            OMozabTable(const OMozabTable* _pTable);
        public:
            virtual void refreshColumns();
            virtual void refreshIndexes();

        public:
            //  DECLARE_CTY_DEFAULTS( OMozabTable_BASE);
            OMozabTable( sdbcx::OCollection* _pTables,OMozabConnection* _pConnection);
            OMozabTable( sdbcx::OCollection* _pTables,OMozabConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description = ::rtl::OUString(),
                    const ::rtl::OUString& _SchemaName = ::rtl::OUString(),
                    const ::rtl::OUString& _CatalogName = ::rtl::OUString()
                );
            ~OMozabTable( );

            virtual sal_Int32 getCurrentLastPos() const;
            virtual sal_Bool seekRow(IResultSetHelper::Movement eCursorPosition, sal_Int32 nOffset, sal_Int32& nCurPos);
            virtual sal_Bool fetchRow(connectivity::OValueRow _rRow,const OSQLColumns& _rCols, sal_Bool _bUseTableDefs,sal_Bool bRetrieveData);

            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL disposing(void);

            // com::sun::star::lang::XUnoTunnel
            virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);
            static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();

            BOOL DropImpl();
            BOOL CreateImpl();

            virtual BOOL InsertRow(connectivity::OValueVector& rRow, BOOL bFlush,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);
            virtual BOOL DeleteRow(const OSQLColumns& _rCols);
            virtual BOOL UpdateRow(connectivity::OValueVector& rRow, connectivity::OValueRow pOrgRow,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);

            void fillRowData( connectivity::OSQLParseTreeIterator& _aSQLIterator );

            void analyseWhereClause( const OSQLParseNode*          parseTree,
                            ::std::vector< ::rtl::OUString >       &matchItems,
                            ::std::vector< OMozabQuery::eSqlOppr > &matchOper,
                            ::std::vector< ::rtl::OUString >       &matchValues,
                            connectivity::OSQLParseTreeIterator&   aSQLIterator);

            // returns a copy of this object
            OMozabTable* clone() const;
            const OMozabColumnAlias & getColumnAlias() const
                          { return (m_aColumnAlias); }

            void setParameterRow(const OValueRow& _rParaRow)
                          { m_aParameterRow = _rParaRow; }

        };
    }
}

#endif // _CONNECTIVITY_MAB_TABLE_HXX_

