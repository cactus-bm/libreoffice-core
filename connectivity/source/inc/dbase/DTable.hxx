/*************************************************************************
 *
 *  $RCSfile: DTable.hxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: oj $ $Date: 2001-04-30 10:09:03 $
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

#ifndef _CONNECTIVITY_DBASE_TABLE_HXX_
#define _CONNECTIVITY_DBASE_TABLE_HXX_

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


namespace connectivity
{
    namespace dbase
    {
        typedef file::OFileTable ODbaseTable_BASE;
        class ODbaseConnection;

        typedef ::std::map< ::rtl::OUString,
                        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed>, comphelper::UStringMixLess > OContainer;

        class ODbaseTable : public ODbaseTable_BASE
        {
            // der Typ einer dBase datei wird mit dem ersten Byte bestimmt
        public:
            enum DBFType  { dBaseIII = 0x03,
                            dBaseIV  = 0x04,
                            dBaseV   = 0x05,
                            dBaseFS  = 0x43,
                            dBaseFSMemo  = 0xB3,
                            dBaseIIIMemo = 0x83,
                            dBaseIVMemo = 0x8B,
                            dBaseIVMemoSQL = 0x8E,
                            FoxProMemo = 0xF5
                          };
            enum DBFMemoType {  MemodBaseIII = 0,
                                MemodBaseIV,
                                MemoFoxPro
                            };

        private:
            struct DBFHeader {                       /* Kopfsatz-Struktur            */
                                DBFType db_typ;                         /* Dateityp                     */
                                BYTE    db_aedat[3];                    /* Datum der letzen Aenderung   */
                                                                        /* JJ MM TT                     */
                                ULONG   db_anz;                         /* Anzahl der Saetze            */
                                USHORT  db_kopf;                        /* laenge Kopfsatz-Struktur     */
                                USHORT  db_slng;                        /* laenge der Daten-Saetze      */
                                BYTE    db_frei[20];                    /* reserviert                   */
                            };
            struct DBFColumn {                       /* Feldbezeichner               */
                                BYTE    db_fnm[11];                     /* Feldname                     */
                                BYTE    db_typ;                         /* Feldtyp                      */
                                ULONG   db_adr;                         /* Feldadresse                  */
                                BYTE    db_flng;                        /* Feldlaenge                   */
                                BYTE    db_dez;                         /* Dezimalstellen fuer N        */
                                BYTE    db_frei2[14];                   /* reserviert                   */
                            };
            struct DBFMemoHeader
                            {
                                DBFMemoType db_typ;                     /* Dateityp                     */
                                UINT32  db_next;                        /* n�chster freier Block        */
                                USHORT  db_size;                        /* Blockgr��e: dBase 3 fest     */
                            };

            ::std::vector<sal_Int32> m_aTypes;      // holds all type for columns just to avoid to ask the propertyset
            ::std::vector<sal_Int32> m_aPrecisions; // same as aboth
            ::std::vector<sal_Int32> m_aScales;
            DBFHeader       m_aHeader;
            DBFMemoHeader   m_aMemoHeader;
            SvStream*       m_pMemoStream;
            sal_Bool        m_bWriteableMemo;

            void alterColumn(sal_Int32 index,
                             const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& descriptor ,
                             const ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XDataDescriptorFactory>& xOldColumn );
            void readHeader();
            void fillColumns();
            String createTempFile();
            void copyData(ODbaseTable* _pNewTable);
            BOOL CreateFile(const INetURLObject& aFile, BOOL& bCreateMemo);
            BOOL CreateMemoFile(const INetURLObject& aFile);
            BOOL HasMemoFields() const { return m_aHeader.db_typ > dBaseIV;}
            BOOL ReadMemoHeader();
            BOOL ReadMemo(ULONG nBlockNo, ORowSetValue& aVariable);

            BOOL WriteMemo(ORowSetValue& aVariable, ULONG& rBlockNr);
            BOOL WriteBuffer();
            BOOL UpdateBuffer(OValueVector& rRow, OValueRow pOrgRow,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);
            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> isUniqueByColumnName(const ::rtl::OUString& _rColName);
            void AllocBuffer();

        protected:
            virtual void FileClose();
//          using ::connectivity::sdbcx::OTableDescriptor_BASE::rBHelper;

        public:
            virtual void refreshColumns();
            virtual void refreshIndexes();

        public:
            ODbaseTable( ODbaseConnection* _pConnection);
            ODbaseTable( ODbaseConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description = ::rtl::OUString(),
                    const ::rtl::OUString& _SchemaName = ::rtl::OUString(),
                    const ::rtl::OUString& _CatalogName = ::rtl::OUString()
                );

            void construct(); // can throw any exception

            virtual sal_Int32 getCurrentLastPos() const;
            virtual sal_Bool seekRow(FilePosition eCursorPosition, sal_Int32 nOffset, sal_Int32& nCurPos);
            virtual sal_Bool fetchRow(OValueRow _rRow,const OSQLColumns& _rCols, sal_Bool _bUseTableDefs,sal_Bool bRetrieveData);

            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL disposing(void);

            // com::sun::star::lang::XUnoTunnel
            virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);
            static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();
            // XAlterTable
            virtual void SAL_CALL alterColumnByName( const ::rtl::OUString& colName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL alterColumnByIndex( sal_Int32 index, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
            // XRename
            virtual void SAL_CALL rename( const ::rtl::OUString& newName ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException);

            BOOL    DropImpl();
            BOOL    CreateImpl();
            String  getEntry();

            virtual BOOL InsertRow(OValueVector& rRow, BOOL bFlush,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);
            virtual BOOL DeleteRow(const OSQLColumns& _rCols);
            virtual BOOL UpdateRow(OValueVector& rRow, OValueRow pOrgRow,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess>& _xCols);

            virtual void addColumn(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& descriptor);
        };
    }
}
#endif // _CONNECTIVITY_DBASE_TABLE_HXX_

