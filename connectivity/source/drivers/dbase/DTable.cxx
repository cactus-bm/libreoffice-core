/*************************************************************************
 *
 *  $RCSfile: DTable.cxx,v $
 *
 *  $Revision: 1.59 $
 *
 *  last change: $Author: fs $ $Date: 2001-08-06 07:41:14 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
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
#include "dbase/DTable.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_COLUMNVALUE_HPP_
#include <com/sun/star/sdbc/ColumnValue.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENTACCESS_HPP_
#include <com/sun/star/ucb/XContentAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SQLC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _SV_CONVERTER_HXX_
#include <svtools/converter.hxx>
#endif
#ifndef _CONNECTIVITY_DBASE_DCONNECTION_HXX_
#include "dbase/DConnection.hxx"
#endif
#ifndef _CONNECTIVITY_DBASE_COLUMNS_HXX_
#include "dbase/DColumns.hxx"
#endif
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif
#ifndef _CONFIG_HXX //autogen
#include <vcl/config.hxx>
#endif
#ifndef _CONNECTIVITY_DBASE_INDEX_HXX_
#include "dbase/DIndex.hxx"
#endif
#ifndef _CONNECTIVITY_DBASE_INDEXES_HXX_
#include "dbase/DIndexes.hxx"
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _INTN_HXX //autogen
#include <tools/intn.hxx>
#endif
#ifndef _ZFORLIST_HXX //autogen
#include <svtools/zforlist.hxx>
#endif
#ifndef _SOLMATH_HXX //autogen wg. SolarMath
#include <tools/solmath.hxx>
#endif
#include <stdio.h>      //sprintf
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include <connectivity/dbconversion.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _UNOTOOLS_TEMPFILE_HXX
#include <unotools/tempfile.hxx>
#endif
#ifndef _UNOTOOLS_UCBHELPER_HXX
#include <unotools/ucbhelper.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _CONNECTIVITY_SDBCX_COLUMN_HXX_
#include "connectivity/PColumn.hxx"
#endif

using namespace ::comphelper;
using namespace connectivity;
using namespace connectivity::sdbcx;
using namespace connectivity::dbase;
using namespace connectivity::file;
using namespace ::ucb;
using namespace ::utl;
using namespace ::cppu;
using namespace ::dbtools;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

// -------------------------------------------------------------------------
void ODbaseTable::readHeader()
{
    sal_Bool bError = sal_False;
    m_pFileStream->RefreshBuffer(); // sicherstellen, dass die Kopfinformationen tatsaechlich neu gelesen werden
    m_pFileStream->Seek(STREAM_SEEK_TO_BEGIN);

    BYTE nType=0;
    (*m_pFileStream) >> nType;
    if(ERRCODE_NONE != m_pFileStream->GetErrorCode())
        throwInvalidDbaseFormat();

    m_pFileStream->Read((char*)(&m_aHeader.db_aedat), 3*sizeof(BYTE));
    if(ERRCODE_NONE != m_pFileStream->GetErrorCode())
        throwInvalidDbaseFormat();
    (*m_pFileStream) >> m_aHeader.db_anz;
    if(ERRCODE_NONE != m_pFileStream->GetErrorCode())
        throwInvalidDbaseFormat();
    (*m_pFileStream) >> m_aHeader.db_kopf;
    if(ERRCODE_NONE != m_pFileStream->GetErrorCode())
        throwInvalidDbaseFormat();
    (*m_pFileStream) >> m_aHeader.db_slng;
    if(ERRCODE_NONE != m_pFileStream->GetErrorCode())
        throwInvalidDbaseFormat();
    m_pFileStream->Read((char*)(&m_aHeader.db_frei), 20*sizeof(BYTE));
    if(ERRCODE_NONE != m_pFileStream->GetErrorCode())
        throwInvalidDbaseFormat();

    if (m_aHeader.db_anz  < 0 ||
        m_aHeader.db_kopf <= 0 ||
        m_aHeader.db_slng <= 0 ||
        ((m_aHeader.db_kopf - 1) / 32 - 1) <= 0) // anzahl felder
    {
        // no dbase file
        throwInvalidDbaseFormat();
    }
    else
    {
        // Konsistenzpruefung des Header:
        m_aHeader.db_typ = (DBFType)nType;
        switch (m_aHeader.db_typ)
        {
            case dBaseIII:
            case dBaseIV:
            case dBaseV:
            case dBaseFS:
            case dBaseFSMemo:
            case dBaseIVMemoSQL:
            case dBaseIIIMemo:
            case dBaseIVMemo:
            case FoxProMemo:
                m_pFileStream->SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);
                break;
            default:
            {
                throwInvalidDbaseFormat();
            }
        }
    }
}
// -------------------------------------------------------------------------
void ODbaseTable::fillColumns()
{
    m_pFileStream->Seek(STREAM_SEEK_TO_BEGIN);
    m_pFileStream->Seek(32L);

    if(!m_aColumns.isValid())
        m_aColumns = new OSQLColumns();
    else
        m_aColumns->clear();

    m_aTypes.clear();
    m_aPrecisions.clear();
    m_aScales.clear();

    // Anzahl Felder:
    sal_Int32 nFieldCount = (m_aHeader.db_kopf - 1) / 32 - 1;

    String aStrFieldName;aStrFieldName.AssignAscii("Column");
    sal_Int32 nFieldCnt = 0;
    ::rtl::OUString aTypeName;

    for (sal_Int32 i = 0; i < nFieldCount; i++)
    {
        DBFColumn aDBFColumn;
        m_pFileStream->Read((char*)&aDBFColumn, sizeof(aDBFColumn));

        String aColumnName((const char *)aDBFColumn.db_fnm,getConnection()->getTextEncoding());

        sal_Int32 nPrecision = aDBFColumn.db_flng;
        sal_Int32 eType;

        switch (aDBFColumn.db_typ)
        {
            case 'C':
                eType = DataType::VARCHAR;
                aTypeName = ::rtl::OUString::createFromAscii("VARCHAR");
                break;
            case 'F':
            case 'N':
                eType = DataType::DECIMAL;
                aTypeName = ::rtl::OUString::createFromAscii("DECIMAL");

                // Bei numerischen Feldern werden zwei Zeichen mehr geschrieben, als die Precision der Spaltenbeschreibung eigentlich
                // angibt, um Platz fuer das eventuelle Vorzeichen und das Komma zu haben. Das muss ich jetzt aber wieder rausrechnen.
                nPrecision = SvDbaseConverter::ConvertPrecisionToOdbc(nPrecision,aDBFColumn.db_dez);
                    // leider gilt das eben Gesagte nicht fuer aeltere Versionen ....
                    ;
                break;
            case 'L':
                eType = DataType::BIT;
                aTypeName = ::rtl::OUString::createFromAscii("BIT");
                break;
            case 'D':
                eType = DataType::DATE;
                aTypeName = ::rtl::OUString::createFromAscii("DATE");
                break;
            case 'M':
                eType = DataType::LONGVARCHAR;
                aTypeName = ::rtl::OUString::createFromAscii("LONGVARCHAR");
                nPrecision = 0;
                break;
            default:
                aTypeName = ::rtl::OUString::createFromAscii("OTHER");
                eType = DataType::OTHER;

        }

        sal_Int32 nFlags = 0;
        switch (aDBFColumn.db_typ)
        {
            case 'C':
            case 'D':
            case 'L':   nFlags = ColumnSearch::FULL; break;
            case 'F':
            case 'N':   nFlags = ColumnSearch::BASIC; break;
            case 'M':   nFlags = ColumnSearch::CHAR; break;
            default:
                        nFlags = ColumnSearch::NONE;

        }

        sdbcx::OColumn* pColumn = new sdbcx::OColumn(aColumnName,aTypeName,::rtl::OUString(),
                                                ColumnValue::NULLABLE,nPrecision,aDBFColumn.db_dez,eType,sal_False,sal_False,sal_False,
                                                getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers());
        Reference< XPropertySet> xCol = pColumn;
        m_aColumns->push_back(xCol);
        m_aTypes.push_back(eType);
        m_aPrecisions.push_back(nPrecision);
        m_aScales.push_back(aDBFColumn.db_dez);
    }
}
// -------------------------------------------------------------------------
ODbaseTable::ODbaseTable(ODbaseConnection* _pConnection)
        :ODbaseTable_BASE(_pConnection)
        ,m_pMemoStream(NULL)
        ,m_bWriteableMemo(sal_False)
{
    // initialize the header
    m_aHeader.db_typ    = dBaseIII;
    m_aHeader.db_anz    = 0;
    m_aHeader.db_kopf   = 0;
    m_aHeader.db_slng   = 0;
}
// -------------------------------------------------------------------------
ODbaseTable::ODbaseTable(ODbaseConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description ,
                    const ::rtl::OUString& _SchemaName,
                    const ::rtl::OUString& _CatalogName
                ) : ODbaseTable_BASE(_pConnection,_Name,
                                  _Type,
                                  _Description,
                                  _SchemaName,
                                  _CatalogName)
                ,m_pMemoStream(NULL)
                ,m_bWriteableMemo(sal_False)
{
}

// -----------------------------------------------------------------------------
void ODbaseTable::construct()
{
    // initialize the header
    m_aHeader.db_typ    = dBaseIII;
    m_aHeader.db_anz    = 0;
    m_aHeader.db_kopf   = 0;
    m_aHeader.db_slng   = 0;

    String sFileName(getEntry());

    INetURLObject aURL;
    aURL.SetURL(sFileName);

    OSL_ENSURE(aURL.getExtension() == m_pConnection->getExtension(),
        "ODbaseTable::ODbaseTable: invalid extension!");
        // getEntry is expected to ensure the corect file name

    m_pFileStream = createStream_simpleError( sFileName, STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYWRITE);

    if (!(m_bWriteable = (NULL != m_pFileStream)))
        m_pFileStream = createStream_simpleError( sFileName, STREAM_READ | STREAM_NOCREATE | STREAM_SHARE_DENYNONE);

    if(m_pFileStream)
    {
        readHeader();
        if (HasMemoFields())
        {
            // Memo-Dateinamen bilden (.DBT):
            // nyi: Unschoen fuer Unix und Mac!

            if (m_aHeader.db_typ == FoxProMemo) // foxpro uses another extension
                aURL.SetExtension(String::CreateFromAscii("fpt"));
            else
                aURL.SetExtension(String::CreateFromAscii("dbt"));

            // Wenn die Memodatei nicht gefunden wird, werden die Daten trotzdem angezeigt
            // allerdings koennen keine Updates durchgefuehrt werden
            // jedoch die Operation wird ausgefuehrt
            m_pMemoStream = createStream_simpleError( aURL.GetMainURL(INetURLObject::NO_DECODE), STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYWRITE);
            if (!(m_bWriteableMemo = (NULL != m_pMemoStream)))
                m_pMemoStream = createStream_simpleError( aURL.GetMainURL(INetURLObject::NO_DECODE), STREAM_READ | STREAM_NOCREATE | STREAM_SHARE_DENYNONE);
            if (m_pMemoStream)
                ReadMemoHeader();
        }
        //  if(!m_pColumns && (!m_aColumns.isValid() || !m_aColumns->size()))
        fillColumns();

        m_pFileStream->Seek(STREAM_SEEK_TO_END);
        UINT32 nFileSize = m_pFileStream->Tell();
        m_pFileStream->Seek(STREAM_SEEK_TO_BEGIN);

        // Buffersize abhaengig von der Filegroesse
        m_pFileStream->SetBufferSize(nFileSize > 1000000 ? 32768 :
                                  nFileSize > 100000 ? 16384 :
                                  nFileSize > 10000 ? 4096 : 1024);

        if (m_pMemoStream)
        {
            // Puffer genau auf Laenge eines Satzes stellen
            m_pMemoStream->Seek(STREAM_SEEK_TO_END);
            nFileSize = m_pMemoStream->Tell();
            m_pMemoStream->Seek(STREAM_SEEK_TO_BEGIN);

            // Buffersize abhaengig von der Filegroesse
            m_pMemoStream->SetBufferSize(nFileSize > 1000000 ? 32768 :
                                          nFileSize > 100000 ? 16384 :
                                          nFileSize > 10000 ? 4096 :
                                          m_aMemoHeader.db_size);
        }

        AllocBuffer();
    }
}
//------------------------------------------------------------------
BOOL ODbaseTable::ReadMemoHeader()
{
    m_pMemoStream->SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);
    m_pMemoStream->RefreshBuffer();         // sicherstellen das die Kopfinformationen tatsaechlich neu gelesen werden
    m_pMemoStream->Seek(0L);

    (*m_pMemoStream) >> m_aMemoHeader.db_next;
    switch (m_aHeader.db_typ)
    {
        case dBaseIIIMemo:  // dBase III: feste Blockgr��e
        case dBaseIVMemo:
            // manchmal wird aber auch dBase3 dBase4 Memo zugeordnet
            m_pMemoStream->Seek(20L);
            (*m_pMemoStream) >> m_aMemoHeader.db_size;
            if (m_aMemoHeader.db_size > 1 && m_aMemoHeader.db_size != 512)  // 1 steht auch fuer dBase 3
                m_aMemoHeader.db_typ  = MemodBaseIV;
            else if (m_aMemoHeader.db_size > 1 && m_aMemoHeader.db_size == 512)
            {
                // nun gibt es noch manche Dateien, die verwenden eine G��enangabe,
                // sind aber dennoch dBase Dateien
                char sHeader[4];
                m_pMemoStream->Seek(m_aMemoHeader.db_size);
                m_pMemoStream->Read(sHeader,4);

                if ((m_pMemoStream->GetErrorCode() != ERRCODE_NONE) || ((BYTE)sHeader[0]) != 0xFF || ((BYTE)sHeader[1]) != 0xFF || ((BYTE)sHeader[2]) != 0x08)
                    m_aMemoHeader.db_typ  = MemodBaseIII;
                else
                    m_aMemoHeader.db_typ  = MemodBaseIV;
            }
            else
            {
                m_aMemoHeader.db_typ  = MemodBaseIII;
                m_aMemoHeader.db_size = 512;
            }
            break;
        case FoxProMemo:
            m_aMemoHeader.db_typ    = MemoFoxPro;
            m_pMemoStream->Seek(6L);
            m_pMemoStream->SetNumberFormatInt(NUMBERFORMAT_INT_BIGENDIAN);
            (*m_pMemoStream) >> m_aMemoHeader.db_size;
    }
    return TRUE;
}
// -------------------------------------------------------------------------
String ODbaseTable::getEntry()
{
    ::rtl::OUString aURL;
    try
    {
        Reference< XResultSet > xDir = m_pConnection->getDir()->getStaticResultSet();
        Reference< XRow> xRow(xDir,UNO_QUERY);
        ::rtl::OUString sName;
        ::rtl::OUString sExt;
        ::rtl::OUString sNeededExt(m_pConnection->getExtension());
        sal_Int32 nExtLen = sNeededExt.getLength();
        sal_Int32 nExtLenWithSep = nExtLen + 1;
        xDir->beforeFirst();
        while(xDir->next())
        {
            sName = xRow->getString(1);

            // cut the extension
            sExt = sName.copy(sName.getLength() - nExtLen);
            sName = sName.copy(0, sName.getLength() - nExtLenWithSep);

            // name and extension have to coincide
            if ((sName == m_Name) && (sExt == sNeededExt))
            {
                Reference< XContentAccess > xContentAccess( xDir, UNO_QUERY );
                aURL = xContentAccess->queryContentIdentifierString();
                break;
            }
        }
        xDir->beforeFirst(); // move back to before first record
    }
    catch(Exception&)
    {
        OSL_ASSERT(0);
    }
    return aURL;
}
// -------------------------------------------------------------------------
void ODbaseTable::refreshColumns()
{
    ::osl::MutexGuard aGuard( m_aMutex );

    TStringVector aVector;

    for(OSQLColumns::const_iterator aIter = m_aColumns->begin();aIter != m_aColumns->end();++aIter)
        aVector.push_back(Reference< XNamed>(*aIter,UNO_QUERY)->getName());

    if(m_pColumns)
        m_pColumns->reFill(aVector);
    else
        m_pColumns  = new ODbaseColumns(this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void ODbaseTable::refreshIndexes()
{
    TStringVector aVector;
    if(m_pFileStream && (!m_pIndexes || m_pIndexes->getCount() == 0))
    {
        INetURLObject aURL;
        aURL.SetURL(getEntry());

        aURL.setExtension(String::CreateFromAscii("inf"));
        Config aInfFile(aURL.getFSysPath(INetURLObject::FSYS_DETECT));
        aInfFile.SetGroup(dBASE_III_GROUP);
        USHORT nKeyCnt = aInfFile.GetKeyCount();
        ByteString aKeyName;
        ByteString aIndexName;

        for (USHORT nKey = 0,nPos=0; nKey < nKeyCnt; nKey++)
        {
            // Verweist der Key auf ein Indexfile?...
            aKeyName = aInfFile.GetKeyName( nKey );
            //...wenn ja, Indexliste der Tabelle hinzufuegen
            if (aKeyName.Copy(0,3) == ByteString("NDX") )
            {
                aIndexName = aInfFile.ReadKey(aKeyName);
                aURL.setName(String(aIndexName,getConnection()->getTextEncoding()));
                try
                {
                    Content aCnt(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());
                    if (aCnt.isDocument())
                    {
                        aVector.push_back(aURL.getBase());
                    }
                }
                catch(Exception&) // a execption is thrown when no file exists
                {
                }
            }
        }
    }
    if(m_pIndexes)
        m_pIndexes->reFill(aVector);
    else
        m_pIndexes  = new ODbaseIndexes(this,m_aMutex,aVector);
}

// -------------------------------------------------------------------------
void SAL_CALL ODbaseTable::disposing(void)
{
    OFileTable::disposing();
    ::osl::MutexGuard aGuard(m_aMutex);
    m_aColumns = NULL;
}
// -------------------------------------------------------------------------
Sequence< Type > SAL_CALL ODbaseTable::getTypes(  ) throw(RuntimeException)
{
    Sequence< Type > aTypes = OTable_TYPEDEF::getTypes();
    Sequence< Type > aRet(aTypes.getLength());
    const Type* pBegin = aTypes.getConstArray();
    const Type* pEnd = pBegin + aTypes.getLength();
    sal_Int32 i=0;
    for(;pBegin != pEnd;++pBegin)
    {
        if(!(*pBegin == ::getCppuType((const Reference<XKeysSupplier>*)0)   ||
            //  *pBegin == ::getCppuType((const Reference<XAlterTable>*)0)  ||
            *pBegin == ::getCppuType((const Reference<XDataDescriptorFactory>*)0)))
        {
            aRet.getArray()[i++] = *pBegin;
        }
    }
    aRet.getArray()[i] = ::getCppuType( (const Reference< ::com::sun::star::lang::XUnoTunnel > *)0 );

    return aRet;
}

// -------------------------------------------------------------------------
Any SAL_CALL ODbaseTable::queryInterface( const Type & rType ) throw(RuntimeException)
{
    if( rType == ::getCppuType((const Reference<XKeysSupplier>*)0)  ||
        //  rType == ::getCppuType((const Reference<XAlterTable>*)0)    ||
        rType == ::getCppuType((const Reference<XDataDescriptorFactory>*)0))
        return Any();

    Any aRet = ::cppu::queryInterface(rType,static_cast< ::com::sun::star::lang::XUnoTunnel*> (this));
    if(!aRet.hasValue())
        aRet = OTable_TYPEDEF::queryInterface(rType);

    return aRet;
}

//--------------------------------------------------------------------------
Sequence< sal_Int8 > ODbaseTable::getUnoTunnelImplementationId()
{
    static ::cppu::OImplementationId * pId = 0;
    if (! pId)
    {
        ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
        if (! pId)
        {
            static ::cppu::OImplementationId aId;
            pId = &aId;
        }
    }
    return pId->getImplementationId();
}

// com::sun::star::lang::XUnoTunnel
//------------------------------------------------------------------
sal_Int64 ODbaseTable::getSomething( const Sequence< sal_Int8 > & rId ) throw (RuntimeException)
{
    if (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
        return (sal_Int64)this;

    return ODbaseTable_BASE::getSomething(rId);
}
//------------------------------------------------------------------
sal_Bool ODbaseTable::fetchRow(OValueRow _rRow,const OSQLColumns & _rCols, sal_Bool _bUseTableDefs,sal_Bool bRetrieveData)
{
    // Einlesen der Daten
    BOOL bIsCurRecordDeleted = ((char)m_pBuffer[0] == '*') ? TRUE : sal_False;

    // only read the bookmark

    // Satz als geloescht markieren
    //  rRow.setState(bIsCurRecordDeleted ? ROW_DELETED : ROW_CLEAN );
    _rRow->setDeleted(bIsCurRecordDeleted);
    (*_rRow)[0] = m_nFilePos;

    if (!bRetrieveData)
        return TRUE;

    sal_Int32 nByteOffset = 1;
    // Felder:
    OSQLColumns::const_iterator aIter = _rCols.begin();
    for (sal_Int32 i = 1; aIter != _rCols.end();++aIter, i++)
    {
        //  pVal = (*_rRow)[i].getBodyPtr();
        Reference< XPropertySet> xColumn = *aIter;

        ::rtl::OUString aName;
        xColumn->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aName;
        // Laengen je nach Datentyp:
        sal_Int32 nLen;
        sal_Int32 nType;
        if(_bUseTableDefs)
        {
            nLen    = m_aPrecisions[i-1];
            nType   = m_aTypes[i-1];
        }
        else
        {
            xColumn->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION))  >>= nLen;
            xColumn->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))       >>= nType;
        }
        switch(nType)
        {
            case DataType::DATE:        nLen = 8; break;
            case DataType::DECIMAL:
                if(_bUseTableDefs)
                    nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,m_aScales[i-1]);
                else
                    nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,getINT32(xColumn->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))));
                break;  // das Vorzeichen und das Komma
            case DataType::BIT:         nLen = 1; break;
            case DataType::LONGVARCHAR: nLen = 10; break;
            case DataType::OTHER:
                nByteOffset += nLen;
                continue;
            default:                    break;
        }

        // Ist die Variable ueberhaupt gebunden?
        if (!(*_rRow)[i].isBound())
        {
            // Nein - naechstes Feld.
            nByteOffset += nLen;
            continue;
        }

        char *pData = (char *) (m_pBuffer + nByteOffset);

        //  (*_rRow)[i].setType(nType);

        if (nType == DataType::CHAR || nType == DataType::VARCHAR)
        {
            char cLast = pData[nLen];
            pData[nLen] = 0;
            String aStr(pData,(xub_StrLen)nLen,getConnection()->getTextEncoding());
            aStr.EraseTrailingChars();

            if (!aStr.Len())                // keine StringLaenge, dann NULL
                (*_rRow)[i].setNull();
            else
            {
                ::rtl::OUString aStr2(aStr);
                (*_rRow)[i] = aStr2;
            }
            pData[nLen] = cLast;
        }
        else
        {
            // Falls Nul-Zeichen im String enthalten sind, in Blanks umwandeln!
            for (sal_Int32 k = 0; k < nLen; k++)
            {
                if (pData[k] == '\0')
                    pData[k] = ' ';
            }

            String aStr(pData, (xub_StrLen)nLen,getConnection()->getTextEncoding());        // Spaces am Anfang und am Ende entfernen:
            aStr.EraseLeadingChars();
            aStr.EraseTrailingChars();

            if (!aStr.Len())
            {
                nByteOffset += nLen;
                (*_rRow)[i].setNull();  // keine Werte -> fertig
                continue;
            }

            switch (nType)
            {
                case DataType::DATE:
                {
                    if (aStr.Len() != nLen)
                    {
                        (*_rRow)[i].setNull();
                        break;
                    }
                    sal_uInt16  nYear   = (sal_uInt16)aStr.Copy( 0, 4 ).ToInt32();
                    sal_uInt16  nMonth  = (sal_uInt16)aStr.Copy( 4, 2 ).ToInt32();
                    sal_uInt16  nDay    = (sal_uInt16)aStr.Copy( 6, 2 ).ToInt32();

                    ::com::sun::star::util::Date aDate(nDay,nMonth,nYear);
                    (*_rRow)[i] = aDate;
                }
                break;
                case DataType::DECIMAL:
                    (*_rRow)[i] = aStr;
                    //  pVal->setDouble(SdbTools::ToDouble(aStr));
                break;
                case DataType::BIT:
                {
                    BOOL b;
                    switch (* ((const char *)pData))
                    {
                        case 'T':
                        case 'Y':
                        case 'J':   b = TRUE; break;
                        default:    b = sal_False; break;
                    }
                    (*_rRow)[i] = b;
                    //  pVal->setDouble(b);
                }
                break;
                case DataType::LONGVARCHAR:
                {
                    long nBlockNo = aStr.ToInt32(); // Blocknummer lesen
                    if (nBlockNo > 0 && m_pMemoStream) // Daten aus Memo-Datei lesen, nur wenn
                    {
                        if (!ReadMemo(nBlockNo, (*_rRow)[i]))
                            break;
                    }
                    else
                        (*_rRow)[i].setNull();
                }   break;
                default:
                    OSL_ASSERT("Falscher Type");
            }
            (*_rRow)[i].setTypeKind(nType);
        }

//      if (aStatus.IsError())
//          break;
        // Und weiter ...
        nByteOffset += nLen;
    }
    return sal_True;
}
//------------------------------------------------------------------
// -------------------------------------------------------------------------
void ODbaseTable::FileClose()
{
    ::osl::MutexGuard aGuard(m_aMutex);
    // falls noch nicht alles geschrieben wurde
    if (m_pMemoStream && m_pMemoStream->IsWritable())
        m_pMemoStream->Flush();

    delete m_pMemoStream;
    m_pMemoStream = NULL;

    ODbaseTable_BASE::FileClose();
}
// -------------------------------------------------------------------------
BOOL ODbaseTable::CreateImpl()
{
    OSL_ENSURE(!m_pFileStream, "SequenceError");

    INetURLObject aURL;
    aURL.SetSmartProtocol(INET_PROT_FILE);
    String aName = getEntry();
    if(!aName.Len())
    {
        ::rtl::OUString aIdent = m_pConnection->getContent()->getIdentifier()->getContentIdentifier();
        aIdent += ::rtl::OUString::createFromAscii("/");
        aIdent += m_Name;
        aName = aIdent.getStr();
    }
    aURL.SetURL(aName);

    if(aURL.getExtension() != m_pConnection->getExtension())
        aURL.setExtension(m_pConnection->getExtension());

    Content aContent(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());
    try
    {
        if (aContent.isDocument())
        {
            // Hack fuer Bug #30609 , nur wenn das File existiert und die Laenge > 0 gibt es einen Fehler
            SvStream* pFileStream = createStream_simpleError( aURL.GetMainURL(INetURLObject::NO_DECODE),STREAM_READ);

            if (pFileStream && pFileStream->Seek(STREAM_SEEK_TO_END))
            {
                //  aStatus.SetError(ERRCODE_IO_ALREADYEXISTS,TABLE,aFile.GetFull());
                return sal_False;
            }
            delete pFileStream;
        }
    }
    catch(Exception&) // a execption is thrown when no file exists
    {
    }

    BOOL bMemoFile = sal_False;

    sal_Bool bOk = CreateFile(aURL, bMemoFile);

    FileClose();

    if (!bOk)
    {
        aContent.executeCommand( rtl::OUString::createFromAscii( "delete" ),bool2any( sal_True ) );
        return sal_False;
    }

    if (bMemoFile)
    {
        String aExt = aURL.getExtension();
        aURL.setExtension(String::CreateFromAscii("dbt"));                      // extension for memo file
        Content aMemo1Content(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());

        sal_Bool bMemoAlreadyExists = sal_False;
        try
        {
            bMemoAlreadyExists = aMemo1Content.isDocument();
        }
        catch(Exception&) // a execption is thrown when no file exists
        {
        }
        if (bMemoAlreadyExists)
        {
            //  aStatus.SetError(ERRCODE_IO_ALREADYEXISTS,MEMO,aFile.GetFull());
            aURL.setExtension(aExt);      // kill dbf file
            try
            {
                Content aMemoContent(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());
                aMemoContent.executeCommand( rtl::OUString::createFromAscii( "delete" ),bool2any( sal_True ) );
            }
            catch(const Exception&)
            {
                ::rtl::OUString sMessage = ::rtl::OUString::createFromAscii("[StarOffice Base dbase] The memo file '");
                sMessage += aName;
                sMessage += ::rtl::OUString::createFromAscii(" already exists.");
                throwGenericSQLException(sMessage, static_cast<XNamed*>(this));
            }
        }
        if (!CreateMemoFile(aURL))
        {
            aURL.setExtension(aExt);      // kill dbf file
            Content aMemoContent(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());
            aMemoContent.executeCommand( rtl::OUString::createFromAscii( "delete" ),bool2any( sal_True ) );
            return sal_False;
        }
        m_aHeader.db_typ = dBaseIIIMemo;
    }
    else
        m_aHeader.db_typ = dBaseIII;

//  if (GetDBFConnection()->GetShowDeleted())
//      nPrivileges = SDB_PR_READ | SDB_PR_INSERT | SDB_PR_UPDATE |
//          SDB_PR_ALTER | SDB_PR_DROP;
//  else
        //  nPrivileges = SDB_PR_READ | SDB_PR_INSERT | SDB_PR_UPDATE |
            //  SDB_PR_DELETE | SDB_PR_ALTER | SDB_PR_DROP;

    return TRUE;
}

//------------------------------------------------------------------
// erzeugt grunds�tzlich dBase IV Datei Format
BOOL ODbaseTable::CreateFile(const INetURLObject& aFile, BOOL& bCreateMemo)
{
    bCreateMemo = sal_False;
    Date aDate;                                     // aktuelles Datum

    m_pFileStream = createStream_simpleError( aFile.GetMainURL(INetURLObject::NO_DECODE),STREAM_READWRITE | STREAM_SHARE_DENYWRITE | STREAM_TRUNC );

    if (!m_pFileStream)
        return sal_False;

    char aBuffer[21];               // write buffer
    memset(aBuffer,0,sizeof(aBuffer));

    m_pFileStream->Seek(0L);
    (*m_pFileStream) << (BYTE) dBaseIII;                                                                // dBase format
    (*m_pFileStream) << (BYTE) (aDate.GetYear() % 100);                 // aktuelles Datum


    (*m_pFileStream) << (BYTE) aDate.GetMonth();
    (*m_pFileStream) << (BYTE) aDate.GetDay();
    (*m_pFileStream) << 0L;                                                                                                     // Anzahl der Datens�tze
    (*m_pFileStream) << (USHORT)((m_pColumns->getCount()+1) * 32 + 1);                // Kopfinformationen,
                                                                        // pColumns erh�lt immer eine Spalte mehr
    (*m_pFileStream) << (USHORT) 0;                                                                                     // Satzl�nge wird sp�ter bestimmt
    m_pFileStream->Write(aBuffer, 20);

    USHORT nRecLength = 1;                                                                                          // L�nge 1 f�r deleted flag
    sal_Int32  nMaxFieldLength = m_pConnection->getMetaData()->getMaxColumnNameLength();
    Reference<XIndexAccess> xColumns(getColumns(),UNO_QUERY);

    ::rtl::OUString aName;
    Reference<XPropertySet> xCol;
    for(sal_Int32 i=0;i<xColumns->getCount();++i)
    {
        ::cppu::extractInterface(xCol,xColumns->getByIndex(i));
        OSL_ENSURE(xCol.is(),"This should be a column!");

        char  cTyp;

        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aName;

        if (aName.getLength() > nMaxFieldLength)
        {
            ::rtl::OUString sMsg = ::rtl::OUString::createFromAscii("Invalid column name length for column: ");
            sMsg += aName;
            sMsg += ::rtl::OUString::createFromAscii("!");
            throw SQLException(sMsg,*this,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());
        }

        ByteString aCol(aName.getStr(), getConnection()->getTextEncoding());
        (*m_pFileStream) << aCol.GetBuffer();
        m_pFileStream->Write(aBuffer, 11 - aCol.Len());

        switch (getINT32(xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))))
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
                cTyp = 'C';
                break;
            case DataType::TINYINT:
            case DataType::SMALLINT:
            case DataType::INTEGER:
            case DataType::BIGINT:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::REAL:
            case DataType::DOUBLE:
                cTyp = 'N';                             // nur dBase 3 format
                break;
            case DataType::DATE:
                cTyp = 'D';
                break;
            case DataType::BIT:
                cTyp = 'L';
                break;
            case DataType::LONGVARBINARY:
            case DataType::LONGVARCHAR:
                cTyp = 'M';
                break;
            default:
                {
                    ::rtl::OUString sMsg = ::rtl::OUString::createFromAscii("Invalid column type for column: ");
                    sMsg += aName;
                    sMsg += ::rtl::OUString::createFromAscii("!");
                    throw SQLException(sMsg,*this,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());
                }
        }

        (*m_pFileStream) << cTyp;
        m_pFileStream->Write(aBuffer, 4);

        sal_Int32 nPrecision = 0;
        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION)) >>= nPrecision;
        sal_Int32 nScale = 0;
        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE)) >>= nScale;

        switch(cTyp)
        {
            case 'C':
                OSL_ENSURE(nPrecision < 255, "ODbaseTable::Create: Column zu lang!");
                if (nPrecision > 254)
                {
                    ::rtl::OUString sMsg = ::rtl::OUString::createFromAscii("Invalid precision for column: ");
                    sMsg += aName;
                    sMsg += ::rtl::OUString::createFromAscii("!");
                    throw SQLException(sMsg,*this,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());
                }
                (*m_pFileStream) << (BYTE) Min((ULONG)nPrecision, 255UL);      //Feldl�nge
                nRecLength += (USHORT)Min((ULONG)nPrecision, 255UL);
                (*m_pFileStream) << (BYTE)0;                                                                //Nachkommastellen
                break;
            case 'F':
            case 'N':
                OSL_ENSURE(nPrecision >=  nScale,
                           "ODbaseTable::Create: Feldl�nge mu� gr��er Nachkommastellen sein!");
                if (nPrecision <  nScale)
                {
                    ::rtl::OUString sMsg = ::rtl::OUString::createFromAscii("Precision is less than scale for column: ");
                    sMsg += aName;
                    sMsg += ::rtl::OUString::createFromAscii("!");
                    throw SQLException(sMsg,*this,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());
                    break;
                }
                if (getBOOL(xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISCURRENCY)))) // Currency wird gesondert behandelt
                {
                    (*m_pFileStream) << (BYTE)10;          // Standard Laenge
                    (*m_pFileStream) << (BYTE)4;
                    nRecLength += 10;
                }
                else
                {
                    sal_Int32 nPrec = SvDbaseConverter::ConvertPrecisionToDbase(nPrecision,nScale);

                    (*m_pFileStream) << (BYTE)( nPrec);
                    (*m_pFileStream) << (BYTE)nScale;
                    nRecLength += (USHORT)nPrec;
                }
                break;
            case 'L':
                (*m_pFileStream) << (BYTE)1;
                (*m_pFileStream) << (BYTE)0;
                nRecLength++;
                break;
            case 'D':
                (*m_pFileStream) << (BYTE)8;
                (*m_pFileStream) << (BYTE)0;
                nRecLength += 8;
                break;
            case 'M':
                bCreateMemo = TRUE;
                (*m_pFileStream) << (BYTE)10;
                (*m_pFileStream) << (BYTE)0;
                nRecLength += 10;
                break;
            default:
                {
                    ::rtl::OUString sMsg = ::rtl::OUString::createFromAscii("Invalid column type for column: ");
                    sMsg += aName;
                    sMsg += ::rtl::OUString::createFromAscii("!");
                    throw SQLException(sMsg,*this,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());
                }
        }
        m_pFileStream->Write(aBuffer, 14);
    }

    (*m_pFileStream) << (BYTE)0x0d;                                     // kopf ende
    m_pFileStream->Seek(10L);
    (*m_pFileStream) << nRecLength;                                     // satzl�nge nachtr�glich eintragen

    if (bCreateMemo)
    {
        m_pFileStream->Seek(0L);
        (*m_pFileStream) << (BYTE) dBaseIIIMemo;
    }
    return TRUE;
}

//------------------------------------------------------------------
// erzeugt grunds�tzlich dBase III Datei Format
BOOL ODbaseTable::CreateMemoFile(const INetURLObject& aFile)
{
    // Makro zum Filehandling f�rs Erzeugen von Tabellen
    m_pMemoStream = createStream_simpleError( aFile.GetMainURL(INetURLObject::NO_DECODE),STREAM_READWRITE | STREAM_SHARE_DENYWRITE);

    if (!m_pMemoStream)
        return sal_False;

    char aBuffer[512];              // write buffer
    memset(aBuffer,0,sizeof(aBuffer));

#ifdef WIN
    m_pMemoStream->Seek(0L);
    for (UINT16 i = 0; i < 512; i++)
    {
        (*m_pMemoStream) << BYTE(0);
    }
#else
    m_pMemoStream->SetFiller('\0');
    m_pMemoStream->SetStreamSize(512);
#endif

    m_pMemoStream->Seek(0L);
    (*m_pMemoStream) << long(1);                  // Zeiger auf ersten freien Block

    m_pMemoStream->Flush();
    delete m_pMemoStream;
    m_pMemoStream = NULL;
    return TRUE;
}
//------------------------------------------------------------------
BOOL ODbaseTable::DropImpl()
{
    FileClose();

    INetURLObject aURL;
    aURL.SetURL(getEntry());

    BOOL bDropped = FALSE;

    if(bDropped = ::utl::UCBContentHelper::Kill(aURL.GetMainURL(INetURLObject::NO_DECODE)))
    {
        if (HasMemoFields())
        {  // delete the memo fields
            aURL.setExtension(String::CreateFromAscii("dbt"));
            bDropped = ::utl::UCBContentHelper::Kill(aURL.GetMainURL(INetURLObject::NO_DECODE));
        }

        if(bDropped)
        {
            // now delete all indices
            refreshIndexes(); // look for indexes which must be deleted as well
            if(m_pIndexes)
            {
                sal_Int32 nCount = m_pIndexes->getCount(),
                       i      = 0;
                while (i < nCount)
                {
                    m_pIndexes->dropByIndex(i);
                }
            }
            //  aFile.SetBase(m_Name);
            aURL.setExtension(String::CreateFromAscii("inf"));

            // as the inf file does not necessarily exist, we aren't allowed to use UCBContentHelper::Kill
            // 89711 - 16.07.2001 - frank.schoenheit@sun.com
            try
            {
                ::ucb::Content aDeleteContent( aURL.GetMainURL( INetURLObject::NO_DECODE ), Reference< ::com::sun::star::ucb::XCommandEnvironment > () );
                aDeleteContent.executeCommand( ::rtl::OUString::createFromAscii( "delete" ), makeAny( sal_Bool( sal_True ) ) );
            }
            catch(Exception&)
            {
                // silently ignore this ....
            }
        }
    }
    if(!bDropped)
    {// we couldn't drop the table so we have to reopen it
        construct();
        if(m_pColumns)
            m_pColumns->refresh();
    }
    return bDropped;
}
//------------------------------------------------------------------
BOOL ODbaseTable::InsertRow(OValueVector& rRow, BOOL bFlush,const Reference<XIndexAccess>& _xCols)
{
    // Buffer mit Leerzeichen f�llen
    AllocBuffer();
    memset(m_pBuffer, ' ', m_aHeader.db_slng);

    // Gesamte neue Row uebernehmen:
    // ... und am Ende als neuen Record hinzufuegen:
    UINT32 nTempPos = m_nFilePos,
           nFileSize,
           nMemoFileSize;

    BOOL bInsertRow;
    m_nFilePos = (ULONG)m_aHeader.db_anz + 1;
    if (bInsertRow = UpdateBuffer(rRow,NULL,_xCols))
    {
        m_pFileStream->Seek(STREAM_SEEK_TO_END);
        nFileSize = m_pFileStream->Tell();

        if (HasMemoFields() && m_pMemoStream)
        {
            m_pMemoStream->Seek(STREAM_SEEK_TO_END);
            nMemoFileSize = m_pMemoStream->Tell();
        }

        if (!WriteBuffer())
        {
            m_pFileStream->SetStreamSize(nFileSize);                // alte Gr��e restaurieren

            if (HasMemoFields() && m_pMemoStream)
                m_pMemoStream->SetStreamSize(nMemoFileSize);    // alte Gr��e restaurieren
            m_nFilePos = nTempPos;                              // Fileposition restaurieren
        }
        else
        {
            // Anzahl Datensaetze im Header erhoehen:
            m_pFileStream->Seek( 4L );
            (*m_pFileStream) << (m_aHeader.db_anz + 1);

            // beim AppendOnly kein Flush!
            if (bFlush)
                m_pFileStream->Flush();

            // bei Erfolg # erh�hen
            m_aHeader.db_anz++;
            rRow[0] = m_nFilePos;                               // BOOKmark setzen
            m_nFilePos = nTempPos;
        }
    }
    else
        m_nFilePos = nTempPos;

    return bInsertRow;;
}

//------------------------------------------------------------------
BOOL ODbaseTable::UpdateRow(OValueVector& rRow, OValueRow pOrgRow,const Reference<XIndexAccess>& _xCols)
{
    // Buffer mit Leerzeichen f�llen
    AllocBuffer();

    // Auf gewuenschten Record positionieren:
    long nPos = m_aHeader.db_kopf + (long)(m_nFilePos-1) * m_aHeader.db_slng;
    m_pFileStream->Seek(nPos);
    m_pFileStream->Read((char*)m_pBuffer, m_aHeader.db_slng);

    UINT32 nMemoFileSize;
    if (HasMemoFields() && m_pMemoStream)
    {
        m_pMemoStream->Seek(STREAM_SEEK_TO_END);
        nMemoFileSize = m_pMemoStream->Tell();
    }
    if (!UpdateBuffer(rRow, pOrgRow,_xCols) || !WriteBuffer())
    {
        if (HasMemoFields() && m_pMemoStream)
            m_pMemoStream->SetStreamSize(nMemoFileSize);    // alte Gr��e restaurieren
    }
    else
    {
        m_pFileStream->Flush();
    }
    return sal_True;
}

//------------------------------------------------------------------
BOOL ODbaseTable::DeleteRow(const OSQLColumns& _rCols)
{
    // Einfach das Loesch-Flag setzen (egal, ob es schon gesetzt war
    // oder nicht):
    // Auf gewuenschten Record positionieren:
    long nPos = m_aHeader.db_kopf + (long)(m_nFilePos-1) * m_aHeader.db_slng;
    m_pFileStream->Seek(nPos);

    OValueRow aRow = new OValueVector(_rCols.size());

    if (!fetchRow(aRow,_rCols,TRUE,TRUE))
        return sal_False;

    Reference<XPropertySet> xCol;
    ::rtl::OUString aColName;
    ::comphelper::UStringMixEqual aCase(isCaseSensitive());
    for (USHORT i = 0; i < m_pColumns->getCount(); i++)
    {
        ::cppu::extractInterface(xCol,m_pColumns->getByIndex(i));
        OSL_ENSURE(xCol.is(),"ODbaseTable::DeleteRow column is null!");

        if(xCol.is())
        {
            xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;
            Reference<XPropertySet> xIndex = isUniqueByColumnName(aColName);
            if (xIndex.is())
            {
                Reference<XUnoTunnel> xTunnel(xIndex,UNO_QUERY);
                OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
                ODbaseIndex* pIndex = (ODbaseIndex*)xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId());
                OSL_ENSURE(pIndex,"ODbaseTable::DeleteRow: No Index returned!");

                OSQLColumns::const_iterator aIter = _rCols.begin();
                for(;aIter != _rCols.end();++aIter,++nPos)
                {
                    if(aCase(getString((*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_REALNAME))),aColName))
                        break;
                }
                if (aIter == _rCols.end())
                    continue;

                pIndex->Delete(m_nFilePos,(*aRow)[nPos]);
            }
        }
    }

    m_pFileStream->Seek(nPos);
    (*m_pFileStream) << (BYTE)'*'; // mark the row in the table as deleted
    m_pFileStream->Flush();
    return sal_True;;
}
// -------------------------------------------------------------------------
Reference<XPropertySet> ODbaseTable::isUniqueByColumnName(const ::rtl::OUString& _rColName)
{
    if(!m_pIndexes)
        refreshIndexes();
    Reference<XPropertySet> xIndex;
    for(sal_Int32 i=0;i<m_pIndexes->getCount();++i)
    {
        ::cppu::extractInterface(xIndex,m_pIndexes->getByIndex(i));
        if(xIndex.is() && getBOOL(xIndex->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISUNIQUE))))
        {
            Reference<XNameAccess> xCols(Reference<XColumnsSupplier>(xIndex,UNO_QUERY)->getColumns());
            if(xCols->hasByName(_rColName))
                return xIndex;

        }
    }
    return Reference<XPropertySet>();
}
//------------------------------------------------------------------
double toDouble(const ByteString& rString)
{
    int nErrno;
    return SolarMath::StringToDouble(UniString(rString,gsl_getSystemTextEncoding()).GetBuffer(),',','.',nErrno);
}

//------------------------------------------------------------------
BOOL ODbaseTable::UpdateBuffer(OValueVector& rRow, OValueRow pOrgRow,const Reference<XIndexAccess>& _xCols)
{
    sal_Int32 nByteOffset  = 1;

    // Felder aktualisieren:
    Reference<XPropertySet> xCol;
    Reference<XPropertySet> xIndex;
    USHORT i;
    ::rtl::OUString aColName;
    ::std::vector< Reference<XPropertySet> > aIndexedCols(m_pColumns->getCount());

    ::comphelper::UStringMixEqual aCase(isCaseSensitive());

    // first search a key that exist already in the table
    for (i = 0; i < m_pColumns->getCount(); i++)
    {
        m_pColumns->getByIndex(i) >>= xCol;
        OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;

        sal_Int32 nPos = 0;
        for(;nPos<_xCols->getCount();++nPos)
        {
            Reference<XPropertySet> xFindCol;
            ::cppu::extractInterface(xFindCol,_xCols->getByIndex(nPos));
            OSL_ENSURE(xFindCol.is(),"ODbaseTable::UpdateBuffer column is null!");
            if(aCase(getString(xFindCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))),aColName))
                break;
        }
        if (nPos >= _xCols->getCount())
            continue;

        ++nPos;
        xIndex = isUniqueByColumnName(aColName);
        aIndexedCols[i] = xIndex;
        if (xIndex.is())
        {
            // first check if the value is different to the old one and when if it conform to the index
            if(pOrgRow.isValid() && (rRow[nPos].isNull() || rRow[nPos] == (*pOrgRow)[nPos]))
                continue;
            else
            {
                //  ODbVariantRef xVar = (pVal == NULL) ? new ODbVariant() : pVal;
                Reference<XUnoTunnel> xTunnel(xIndex,UNO_QUERY);
                OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
                ODbaseIndex* pIndex = (ODbaseIndex*)xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId());
                OSL_ENSURE(pIndex,"ODbaseTable::UpdateBuffer: No Index returned!");

                if (pIndex->Find(0,rRow[nPos]))
                {
                    // es existiert kein eindeutiger Wert
                    ::rtl::OUString sMessage = ::rtl::OUString::createFromAscii("Duplicate value found in column \"");
                    sMessage += aColName;
                    sMessage += ::rtl::OUString::createFromAscii( "\"!");
                    throw SQLException(sMessage,*this,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());
                }
            }
        }
    }

    // when we are here there is no double key in the table

    for (i = 0; i < m_pColumns->getCount(); i++)
    {
        m_pColumns->getByIndex(i) >>= xCol;
        OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;

        // Laengen je nach Datentyp:
        sal_Int32 nLen;
        sal_Int32 nType;
        nLen    = m_aPrecisions[i];
        nType   = m_aTypes[i];

        switch (nType)
        {
            case DataType::DATE:        nLen = 8; break;
            case DataType::DECIMAL:
                nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,m_aScales[i]);
                break;  // das Vorzeichen und das Komma
            case DataType::BIT:         nLen = 1; break;
            case DataType::LONGVARCHAR: nLen = 10; break;
            default:                    break;

        }

        sal_Int32 nPos = 0;
        for(;nPos<_xCols->getCount();++nPos)
        {
            Reference<XPropertySet> xFindCol;
            ::cppu::extractInterface(xFindCol,_xCols->getByIndex(nPos));
            if(aCase(getString(xFindCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))),aColName))
                break;
        }

        if (nPos >= _xCols->getCount())
        {
            nByteOffset += nLen;
            continue;
        }

        ++nPos; // the row values start at 1
        // Ist die Variable ueberhaupt gebunden?
        if (!rRow[nPos].isBound() )
        {
            // Nein - naechstes Feld.
            nByteOffset += nLen;
            continue;
        }
        if (aIndexedCols[i].is())
        {
            Reference<XUnoTunnel> xTunnel(aIndexedCols[i],UNO_QUERY);
            OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
            ODbaseIndex* pIndex = (ODbaseIndex*)xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId());
            OSL_ENSURE(pIndex,"ODbaseTable::UpdateBuffer: No Index returned!");
            // Update !!
            if (pOrgRow.isValid() && !rRow[nPos].isNull() )//&& pVal->isModified())
                pIndex->Update(m_nFilePos,(*pOrgRow)[nPos],rRow[nPos]);
            else
                pIndex->Insert(m_nFilePos,rRow[nPos]);
        }



        char* pData = (char *)(m_pBuffer + nByteOffset);
        if (rRow[nPos].isNull())
        {
            memset(pData,' ',nLen); // Zuruecksetzen auf NULL
            nByteOffset += nLen;
            continue;
        }
        try
        {
            switch (nType)
            {
                case DataType::DATE:
                {
                    ::com::sun::star::util::Date aDate;
                    if(rRow[nPos].getTypeKind() == DataType::DOUBLE)
                        aDate = ::dbtools::DBTypeConversion::toDate(rRow[nPos].getDouble());
                    else
                        aDate = rRow[nPos];
                    char s[9];
                    sprintf(s,"%04d%02d%02d",
                        (int)aDate.Year,
                        (int)aDate.Month,
                        (int)aDate.Day);

                    // Genau 8 Byte kopieren:
                    strncpy(pData,s,sizeof s - 1);
                } break;
                case DataType::DECIMAL:
                {
                    memset(pData,' ',nLen); // Zuruecksetzen auf NULL

                    double n = rRow[nPos];

                    int nPrecision      = (int)m_aPrecisions[i];
                    int nScale          = (int)getINT32(xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE)));
                    // ein const_cast, da GetFormatPrecision am SvNumberFormat nicht const ist, obwohl es das eigentlich
                    // sein koennte und muesste

                    String aString;
                    SolarMath::DoubleToString(aString,n,'F',nScale,'.');
                    ByteString aDefaultValue(aString, getConnection()->getTextEncoding());
                    BOOL bValidLength   = sal_False;
                    if (aDefaultValue.Len() <= nLen)
                    {
                        strncpy(pData,aDefaultValue.GetBuffer(),nLen);
                        // write the resulting double back
                        rRow[nPos] = toDouble(aDefaultValue);
                        bValidLength = TRUE;
                    }
                    if (!bValidLength)
                    {
                        String sError;
                        sError.AppendAscii("The ");
                        sError += aColName.getStr();
                        sError.AppendAscii(" column has been defined as a \"Decimal\" type, the max. length is ");
                        sError += String::CreateFromInt32(nPrecision);
                        sError.AppendAscii(" characters (with ");
                        sError += String::CreateFromInt32(nScale);
                        sError.AppendAscii(" decimal places).\n\nThe specified value is longer than the number of digits allowed.");
                        throwGenericSQLException(sError, static_cast<XNamed*>(this));
                    }
                } break;
                case DataType::BIT:
                    *pData = rRow[nPos].getBool() ? 'T' : 'F';
                    break;
                case DataType::LONGVARCHAR:
                {
                    char cNext = pData[nLen]; // merken und temporaer durch 0 ersetzen
                    pData[nLen] = '\0';       // das geht, da der Puffer immer ein Zeichen groesser ist ...

                    ULONG nBlockNo = strtol((const char *)pData,NULL,10);   // Blocknummer lesen

                    // Naechstes Anfangszeichen wieder restaurieren:
                    pData[nLen] = cNext;
                    if (!m_pMemoStream || !WriteMemo(rRow[nPos], nBlockNo))
                        break;

                    ByteString aStr;
                    ByteString aBlock(ByteString::CreateFromInt32(nBlockNo));
                    aStr.Expand(nLen - aBlock.Len(), '0');
                    aStr += aBlock;
                    aStr.Convert(gsl_getSystemTextEncoding(),getConnection()->getTextEncoding());
                    // Zeichen kopieren:
                    memset(pData,' ',nLen); // Zuruecksetzen auf NULL
                    memcpy(pData, aStr.GetBuffer(), nLen);
                }   break;
                default:
                {
                    memset(pData,' ',nLen); // Zuruecksetzen auf NULL
                    ByteString aStr(rRow[nPos].getString().getStr(),getConnection()->getTextEncoding());
                    // Zeichen kopieren:
                    memcpy(pData, aStr.GetBuffer(), min(nLen,(sal_Int32)aStr.Len()));
                }   break;
            }
        }
        catch ( Exception& )
        {
            ::rtl::OUString sMsg = ::rtl::OUString::createFromAscii("Invalid value for column: ");
            sMsg += aColName;
            sMsg += ::rtl::OUString::createFromAscii("!");
            throw SQLException(sMsg,*this,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());
        }
        // Und weiter ...
        nByteOffset += nLen;
    }
    return sal_True;
}
// -----------------------------------------------------------------------------
// XAlterTable
void SAL_CALL ODbaseTable::alterColumnByName( const ::rtl::OUString& colName, const Reference< XPropertySet >& descriptor ) throw(SQLException, NoSuchElementException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OTableDescriptor_BASE::rBHelper.bDisposed);


    Reference<XDataDescriptorFactory> xOldColumn;
    m_pColumns->getByName(colName) >>= xOldColumn;

    alterColumn(m_pColumns->findColumn(colName)-1,descriptor,xOldColumn);
}
// -------------------------------------------------------------------------
void SAL_CALL ODbaseTable::alterColumnByIndex( sal_Int32 index, const Reference< XPropertySet >& descriptor ) throw(SQLException, ::com::sun::star::lang::IndexOutOfBoundsException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OTableDescriptor_BASE::rBHelper.bDisposed);

    if(index < 0 || index >= m_pColumns->getCount())
        throw IndexOutOfBoundsException(::rtl::OUString::valueOf(index),*this);

    Reference<XDataDescriptorFactory> xOldColumn;
    m_pColumns->getByIndex(index) >>= xOldColumn;
    alterColumn(index,descriptor,xOldColumn);
}
// -----------------------------------------------------------------------------
void ODbaseTable::alterColumn(sal_Int32 index,
                              const Reference< XPropertySet >& descriptor ,
                              const Reference< XDataDescriptorFactory >& xOldColumn )
{
    if(index < 0 || index >= m_pColumns->getCount())
        throw IndexOutOfBoundsException(::rtl::OUString::valueOf(index),*this);

    ODbaseTable* pNewTable = NULL;
    try
    {
        OSL_ENSURE(descriptor.is(),"ODbaseTable::alterColumn: descriptor can not be null!");
        // creates a copy of the the original column and copy all properties from descriptor in xCopyColumn
        Reference<XPropertySet> xCopyColumn;
        if(xOldColumn.is())
            xCopyColumn = xOldColumn->createDataDescriptor();
        else
            xCopyColumn = new OColumn(getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers());

        ::comphelper::copyProperties(descriptor,xCopyColumn);

        // creates a temp file

        String sTempName = createTempFile();

        pNewTable = new ODbaseTable(static_cast<ODbaseConnection*>(m_pConnection));
        Reference<XPropertySet> xHoldTable = pNewTable;
        pNewTable->setPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME),makeAny(::rtl::OUString(sTempName)));
        Reference<XAppend> xAppend(pNewTable->getColumns(),UNO_QUERY);
        OSL_ENSURE(xAppend.is(),"ODbaseTable::alterColumn: No XAppend interface!");

        // copy the structure
        sal_Int32 i=0;
        for(;i < index;++i)
        {
            Reference<XPropertySet> xProp;
            m_pColumns->getByIndex(i) >>= xProp;
            Reference<XDataDescriptorFactory> xColumn(xProp,UNO_QUERY);
            Reference<XPropertySet> xCpy;
            if(xColumn.is())
                xCpy = xColumn->createDataDescriptor();
            else
            {
                xCpy = new OColumn(getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers());
                ::comphelper::copyProperties(xProp,xCpy);
            }
            xAppend->appendByDescriptor(xCpy);
        }
        ++i; // now insert our new column
        xAppend->appendByDescriptor(xCopyColumn);

        for(;i < m_pColumns->getCount();++i)
        {
            Reference<XPropertySet> xProp;
            m_pColumns->getByIndex(i) >>= xProp;
            Reference<XDataDescriptorFactory> xColumn(xProp,UNO_QUERY);
            Reference<XPropertySet> xCpy;
            if(xColumn.is())
                xCpy = xColumn->createDataDescriptor();
            else
            {
                xCpy = new OColumn(getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers());
                ::comphelper::copyProperties(xProp,xCpy);
            }
            xAppend->appendByDescriptor(xCpy);
        }

        // construct the new table
        if(!pNewTable->CreateImpl())
        {
            delete pNewTable;
            return;
        }
        pNewTable->construct();

        // copy the data
        copyData(pNewTable,0);

        // now drop the old one
        if(DropImpl())
        {
            // rename the new one to the old one
            pNewTable->rename(m_Name);
            // release the temp file
            pNewTable = NULL;
            ::comphelper::disposeComponent(xHoldTable);
        }
        else
            delete pNewTable;
        FileClose();
        construct();
        if(m_pColumns)
            m_pColumns->refresh();

    }
    catch(const SQLException&)
    {
        delete pNewTable;
        throw;
    }
    catch(const Exception&)
    {
        OSL_ENSURE(0,"ODbaseTable::alterColumn: Exception occured!");
        delete pNewTable;
        throw;
    }
}
// -------------------------------------------------------------------------
void SAL_CALL ODbaseTable::rename( const ::rtl::OUString& newName ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OTableDescriptor_BASE::rBHelper.bDisposed);


    FileClose();
    String aName = getEntry();
    if(!aName.Len())
    {
        ::rtl::OUString aIdent = m_pConnection->getContent()->getIdentifier()->getContentIdentifier();
        aIdent += ::rtl::OUString::createFromAscii("/");
        aIdent += m_Name;
        aName = aIdent;
    }
    INetURLObject aURL;
    aURL.SetURL(aName);

    if(aURL.getExtension() != m_pConnection->getExtension())
        aURL.setExtension(m_pConnection->getExtension());
    String sNewName(newName);
    sNewName.AppendAscii(".");
    sNewName += m_pConnection->getExtension();

    try
    {
        String sOldName = aURL.GetMainURL(INetURLObject::NO_DECODE);
        //  ::utl::UCBContentHelper::MoveTo(sOldName,sNewName);
        Content aContent(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());
        aContent.setPropertyValue( rtl::OUString::createFromAscii( "Title" ),makeAny( ::rtl::OUString(sNewName) ) );
    }
    catch(Exception&)
    {
        throw ElementExistException();
    }
    m_Name = newName;
    construct();
    if(m_pColumns)
        m_pColumns->refresh();
}
// -----------------------------------------------------------------------------
void ODbaseTable::addColumn(const Reference< XPropertySet >& _xNewColumn)
{
    String sTempName = createTempFile();

    ODbaseTable* pNewTable = new ODbaseTable(static_cast<ODbaseConnection*>(m_pConnection));
    pNewTable->acquire();
    pNewTable->setPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME),makeAny(::rtl::OUString(sTempName)));
    {
        Reference<XAppend> xAppend(pNewTable->getColumns(),UNO_QUERY);
        sal_Bool bCase = getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers();
        // copy the structure
        for(sal_Int32 i=0;i < m_pColumns->getCount();++i)
        {
            Reference<XPropertySet> xProp;
            m_pColumns->getByIndex(i) >>= xProp;
            Reference<XDataDescriptorFactory> xColumn(xProp,UNO_QUERY);
            Reference<XPropertySet> xCpy;
            if(xColumn.is())
                xCpy = xColumn->createDataDescriptor();
            else
            {
                xCpy = new OColumn(bCase);
                ::comphelper::copyProperties(xProp,xCpy);
            }

            xAppend->appendByDescriptor(xCpy);
        }
        Reference<XPropertySet> xCpy = new OColumn(bCase);
        ::comphelper::copyProperties(_xNewColumn,xCpy);
        xAppend->appendByDescriptor(xCpy);
    }

    // construct the new table
    if(!pNewTable->CreateImpl())
    {
        delete pNewTable;
        throw SQLException();
    }
    BOOL bAlreadyDroped = FALSE;
    try
    {
        pNewTable->construct();
        // copy the data
        copyData(pNewTable,pNewTable->m_pColumns->getCount());
        // drop the old table
        if(DropImpl())
        {
            bAlreadyDroped = TRUE;
            pNewTable->rename(m_Name);
            // release the temp file
            pNewTable->release();
        }
        else
            delete pNewTable;

        FileClose();
        construct();
        if(m_pColumns)
            m_pColumns->refresh();
    }
    catch(const SQLException&)
    {
        // here we know that the old table wasn't droped before
        if(!bAlreadyDroped)
            delete pNewTable;

        throw;
    }
}
// -----------------------------------------------------------------------------
void ODbaseTable::dropColumn(sal_Int32 _nPos)
{
    String sTempName = createTempFile();

    ODbaseTable* pNewTable = new ODbaseTable(static_cast<ODbaseConnection*>(m_pConnection));
    pNewTable->acquire();
    pNewTable->setPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME),makeAny(::rtl::OUString(sTempName)));
    {
        Reference<XAppend> xAppend(pNewTable->getColumns(),UNO_QUERY);
        sal_Bool bCase = getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers();
        // copy the structure
        for(sal_Int32 i=0;i < m_pColumns->getCount();++i)
        {
            if(_nPos != i)
            {
                Reference<XPropertySet> xProp;
                m_pColumns->getByIndex(i) >>= xProp;
                Reference<XDataDescriptorFactory> xColumn(xProp,UNO_QUERY);
                Reference<XPropertySet> xCpy;
                if(xColumn.is())
                    xCpy = xColumn->createDataDescriptor();
                else
                {
                    xCpy = new OColumn(bCase);
                    ::comphelper::copyProperties(xProp,xCpy);
                }
                xAppend->appendByDescriptor(xCpy);
            }
        }
    }

    // construct the new table
    if(!pNewTable->CreateImpl())
    {
        delete pNewTable;
        throw SQLException();
    }
    pNewTable->construct();
    // copy the data
    copyData(pNewTable,_nPos);
    // drop the old table
    if(DropImpl())
    {
        pNewTable->rename(m_Name);
        // release the temp file
        pNewTable->release();
    }
    else
        delete pNewTable;


    FileClose();
    construct();
    if(m_pColumns)
        m_pColumns->refresh();
}
// -----------------------------------------------------------------------------
String ODbaseTable::createTempFile()
{
    ::rtl::OUString aIdent = m_pConnection->getContent()->getIdentifier()->getContentIdentifier();
    aIdent += ::rtl::OUString::createFromAscii("/");
    String sTempName(aIdent);
    String sExt;
    sExt.AssignAscii(".");
    sExt += m_pConnection->getExtension();

    String sName(m_Name);
    TempFile aTempFile(sName,&sExt,&sTempName);
    if(!aTempFile.IsValid())
        throw SQLException(::rtl::OUString::createFromAscii("Error while alter table!"),NULL,OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_HY0000),1000,Any());

    INetURLObject aURL;
    aURL.SetSmartProtocol(INET_PROT_FILE);
    aURL.SetURL(aTempFile.GetURL());

    String sNewName(aURL.getName());
    sNewName.Erase(sNewName.Len() - sExt.Len());
    return sNewName;
}
// -----------------------------------------------------------------------------
void ODbaseTable::copyData(ODbaseTable* _pNewTable,sal_Int32 _nPos)
{
    sal_Int32 nPos = _nPos + 1; // +1 because we always have the bookmark clumn as well
    OValueRow aRow = new OValueVector(m_pColumns->getCount());
    OValueRow aInsertRow;
    if(nPos)
    {
        aInsertRow = new OValueVector(_pNewTable->m_pColumns->getCount());
        for(OValueVector::iterator aInsertIter = aInsertRow->begin(); aInsertIter != aInsertRow->end();++aInsertIter)
            aInsertIter->setBound(sal_True);
    }
    else
        aInsertRow = aRow;

    // we only have to bind the values which we need to copy into the new table
    for(OValueVector::iterator aIter = aRow->begin(); aIter != aRow->end();++aIter)
        aIter->setBound(sal_True);
    if(nPos && (nPos < (sal_Int32)aRow->size()))
        (*aRow)[nPos].setBound(sal_False);


    sal_Bool bOk = sal_True;
    sal_Int32 nCurPos;
    for(sal_uInt32 nRowPos = 0; nRowPos < m_aHeader.db_anz;++nRowPos)
    {
        if(bOk = seekRow(FILE_BOOKMARK,nRowPos+1,nCurPos))
        {
            if(bOk = fetchRow(aRow,m_aColumns.getBody(),sal_True,sal_True))
            {
                // special handling when pos == 0 then we don't have to distinguish between the two rows
                if(nPos)
                {
                    aIter = aRow->begin()+1;
                    sal_Int32 nCount = 1;
                    for(OValueVector::iterator aInsertIter = aInsertRow->begin()+1; aIter != aRow->end() && aInsertIter != aInsertRow->end();++aIter,++nCount)
                    {
                        if(nPos != nCount)
                        {
                            *aInsertIter = *aIter;
                            ++aInsertIter;
                        }
                    }
                }
                bOk = _pNewTable->InsertRow(*aInsertRow,sal_True,_pNewTable->m_pColumns);
                OSL_ENSURE(bOk,"Row could not be inserted!");
            }
            else
                OSL_ENSURE(bOk,"Row could not be fetched!");
        }
        else
        {
            OSL_ASSERT(0);
        }
    }
}
// -----------------------------------------------------------------------------
void ODbaseTable::throwInvalidDbaseFormat()
{
    // no dbase file
    ::rtl::OUString sMessage = ::rtl::OUString::createFromAscii("[StarOffice Base dbase] The file '");
    sMessage += getEntry();
    sMessage += ::rtl::OUString::createFromAscii(" is an invalid (or unrecognized) dBase file.");
    throwGenericSQLException(sMessage, static_cast<XNamed*>(this));
}
// -----------------------------------------------------------------------------
