/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: LTable.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 05:47:56 $
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

#ifndef _CONNECTIVITY_EVOAB_LTABLE_HXX_
#define _CONNECTIVITY_EVOAB_LTABLE_HXX_

#ifndef _CONNECTIVITY_FILE_TABLE_HXX_
#include "file/FTable.hxx"
#endif
#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/sdbcx/VColumn.hxx"
#endif
#ifndef _CONNECTIVITY_COMMONTOOLS_HXX_
#include "connectivity/CommonTools.hxx"
#endif
#ifndef _URLOBJ_HXX //autogen wg. INetURLObject
#include <tools/urlobj.hxx>
#endif
#include <vector>

namespace connectivity
{
    namespace evoab
    {
        //==================================================================
        // Ableitung von String mit ueberladenen GetToken/GetTokenCount-Methoden
        // Speziell fuer FLAT FILE-Format: Strings koennen gequotet sein
        //==================================================================
        class OEvoabString : public String
        {
        public:
            OEvoabString(){}

            xub_StrLen  GetTokenCount( sal_Unicode cTok = ';', sal_Unicode cStrDel = '\0' ) const;
            void        GetTokenSpecial( String& _rStr,xub_StrLen& nStartPos, sal_Unicode cTok = ';', sal_Unicode cStrDel = '\0' ) const;
        };


        typedef file::OFileTable OEvoabTable_BASE;
        class OEvoabConnection;

        typedef ::std::map< ::rtl::OUString,
                        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed>, comphelper::UStringMixLess > OContainer;

        class OEvoabTable : public OEvoabTable_BASE
        {
        private:
            // maps a row postion to a file position
            ::std::map<sal_Int32,sal_Int32> m_aRowToFilePos;
            ::std::vector<sal_Int32>        m_aTypes;       // holds all type for columns just to avoid to ask the propertyset
            ::std::vector<sal_Int32>        m_aPrecisions;  // same as aboth
            ::std::vector<sal_Int32>        m_aScales;
            OEvoabString                    m_aCurrentLine;
            ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter > m_xNumberFormatter;
            sal_Int32                       m_nRowPos;
            sal_Int32                       m_nMaxRowCount; // will be set if stream is once eof
            ::std::vector< ::rtl::OUString> m_aColumnRawNames;
            sal_Bool    setColumnAliases();
            void        fillColumns();
            sal_Bool checkHeaderLine();
            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XFastPropertySet> isUniqueByColumnName(const ::rtl::OUString& _rColName);

        public:
            virtual void refreshColumns();
            virtual void refreshIndexes();

        public:
            //  DECLARE_CTY_DEFAULTS( OFlatTable_BASE);
            OEvoabTable( sdbcx::OCollection* _pTables,OEvoabConnection* _pConnection);
            OEvoabTable( sdbcx::OCollection* _pTables,OEvoabConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description = ::rtl::OUString(),
                    const ::rtl::OUString& _SchemaName = ::rtl::OUString(),
                    const ::rtl::OUString& _CatalogName = ::rtl::OUString()
                );

            void construct(); // can throw any exception

            virtual sal_Bool seekRow(IResultSetHelper::Movement eCursorPosition, sal_Int32 nOffset, sal_Int32& nCurPos);
            virtual sal_Bool fetchRow(OValueRefRow& _rRow,const OSQLColumns& _rCols, sal_Bool bIsTable,sal_Bool bRetrieveData);

            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL disposing(void);

            // com::sun::star::lang::XUnoTunnel
            virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);
            static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();

            String getEntry();
        };
    }
}
#endif // _CONNECTIVITY_EVOAB_LTABLE_HXX_

