/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: DTable.cxx,v $
 * $Revision: 1.107.30.2 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_connectivity.hxx"
#include "dbase/DTable.hxx"
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/ucb/XContentAccess.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <svtools/converter.hxx>
#include "dbase/DConnection.hxx"
#include "dbase/DColumns.hxx"
#include <osl/thread.h>
#include <tools/config.hxx>
#include "dbase/DIndex.hxx"
#include "dbase/DIndexes.hxx"
#include <comphelper/sequence.hxx>
#include <svtools/zforlist.hxx>
#include <rtl/math.hxx>
#include <stdio.h>      //sprintf
#include <ucbhelper/content.hxx>
#include <comphelper/extract.hxx>
#include <connectivity/dbexception.hxx>
#include <connectivity/dbconversion.hxx>
#include <com/sun/star/lang/DisposedException.hpp>
#include <comphelper/property.hxx>
#include <unotools/tempfile.hxx>
#include <unotools/ucbhelper.hxx>
#include <comphelper/types.hxx>
#include <cppuhelper/exc_hlp.hxx>
#include "connectivity/PColumn.hxx"
#include "connectivity/dbtools.hxx"
#include "connectivity/FValue.hxx"
#include "connectivity/dbconversion.hxx"
#include "resource/dbase_res.hrc"
#include <rtl/logfile.hxx>

#include <algorithm>

using namespace ::comphelper;
using namespace connectivity;
using namespace connectivity::sdbcx;
using namespace connectivity::dbase;
using namespace connectivity::file;
using namespace ::ucbhelper;
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

// stored as the Field Descriptor terminator
#define FIELD_DESCRIPTOR_TERMINATOR 0x0D
#define DBF_EOL                     0x1A

//==================================================================
namespace
{
//==================================================================
sal_Int32 lcl_getFileSize(SvStream& _rStream)
{
    sal_Int32 nFileSize = 0;
    _rStream.Seek(STREAM_SEEK_TO_END);
    _rStream.SeekRel(-1);
    char cEOL;
    _rStream >> cEOL;
    nFileSize = _rStream.Tell();
    if ( cEOL == DBF_EOL )
        nFileSize -= 1;
    return nFileSize;
}
//==================================================================
}
//==================================================================

// -------------------------------------------------------------------------
void ODbaseTable::readHeader()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::readHeader" );
    OSL_ENSURE(m_pFileStream,"No Stream available!");
    if(!m_pFileStream)
        return;
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

    if ( ( ( m_aHeader.db_kopf - 1 ) / 32 - 1 ) <= 0 ) // anzahl felder
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
            case VisualFoxPro:
            case dBaseFS:
            case dBaseFSMemo:
            case dBaseIVMemoSQL:
            case dBaseIIIMemo:
            case FoxProMemo:
                m_pFileStream->SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);
                if ( m_aHeader.db_frei[17] != 0x00
                    && !m_aHeader.db_frei[18] && !m_aHeader.db_frei[19] && getConnection()->isTextEncodingDefaulted() )
                {
                    switch(m_aHeader.db_frei[17])
                    {
                        case 0x01: m_eEncoding = RTL_TEXTENCODING_IBM_437; break;       // DOS USA  code page 437
                        case 0x02: m_eEncoding = RTL_TEXTENCODING_IBM_850; break;       // DOS Multilingual code page 850
                        case 0x03: m_eEncoding = RTL_TEXTENCODING_MS_1252; break;       // Windows ANSI code page 1252
                        case 0x04: m_eEncoding = RTL_TEXTENCODING_APPLE_ROMAN; break;   // Standard Macintosh
                        case 0x64: m_eEncoding = RTL_TEXTENCODING_IBM_852; break;       // EE MS-DOS    code page 852
                        case 0x65: m_eEncoding = RTL_TEXTENCODING_IBM_865; break;       // Nordic MS-DOS    code page 865
                        case 0x66: m_eEncoding = RTL_TEXTENCODING_IBM_866; break;       // Russian MS-DOS   code page 866
                        case 0x67: m_eEncoding = RTL_TEXTENCODING_IBM_861; break;       // Icelandic MS-DOS
                        //case 0x68: m_eEncoding = ; break;     // Kamenicky (Czech) MS-DOS
                        //case 0x69: m_eEncoding = ; break;     // Mazovia (Polish) MS-DOS
                        case 0x6A: m_eEncoding = RTL_TEXTENCODING_IBM_737; break;       // Greek MS-DOS (437G)
                        case 0x6B: m_eEncoding = RTL_TEXTENCODING_IBM_857; break;       // Turkish MS-DOS
                        case 0x96: m_eEncoding = RTL_TEXTENCODING_APPLE_CYRILLIC; break;    // Russian Macintosh
                        case 0x97: m_eEncoding = RTL_TEXTENCODING_APPLE_CENTEURO; break;    // Eastern European Macintosh
                        case 0x98: m_eEncoding = RTL_TEXTENCODING_APPLE_GREEK; break;   // Greek Macintosh
                        case 0xC8: m_eEncoding = RTL_TEXTENCODING_MS_1250; break;       // Windows EE   code page 1250
                        case 0xC9: m_eEncoding = RTL_TEXTENCODING_MS_1251; break;       // Russian Windows
                        case 0xCA: m_eEncoding = RTL_TEXTENCODING_MS_1254; break;       // Turkish Windows
                        case 0xCB: m_eEncoding = RTL_TEXTENCODING_MS_1253; break;       // Greek Windows
                        default:
                            break;
                    }
                }
                break;
            case dBaseIVMemo:
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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::fillColumns" );
    m_pFileStream->Seek(STREAM_SEEK_TO_BEGIN);
    m_pFileStream->Seek(32L);

    if(!m_aColumns.isValid())
        m_aColumns = new OSQLColumns();
    else
        m_aColumns->get().clear();

    m_aTypes.clear();
    m_aPrecisions.clear();
    m_aScales.clear();

    // Anzahl Felder:
    const sal_Int32 nFieldCount = (m_aHeader.db_kopf - 1) / 32 - 1;
    OSL_ENSURE(nFieldCount,"No columns in table!");

    m_aColumns->get().reserve(nFieldCount);
    m_aTypes.reserve(nFieldCount);
    m_aPrecisions.reserve(nFieldCount);
    m_aScales.reserve(nFieldCount);

    String aStrFieldName;
    aStrFieldName.AssignAscii("Column");
    ::rtl::OUString aTypeName;
    static const ::rtl::OUString sVARCHAR(RTL_CONSTASCII_USTRINGPARAM("VARCHAR"));
    const sal_Bool bCase = getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers();

    sal_Int32 i = 0;
    for (; i < nFieldCount; i++)
    {
        DBFColumn aDBFColumn;
        m_pFileStream->Read((char*)&aDBFColumn, sizeof(aDBFColumn));
        if ( FIELD_DESCRIPTOR_TERMINATOR == aDBFColumn.db_fnm[0] ) // 0x0D stored as the Field Descriptor terminator.
            break;

        const String aColumnName((const char *)aDBFColumn.db_fnm,m_eEncoding);

        sal_Int32 nPrecision = aDBFColumn.db_flng;
        sal_Int32 eType;

        switch (aDBFColumn.db_typ)
        {
            case 'C':
                eType = DataType::VARCHAR;
                aTypeName = sVARCHAR;
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
                nPrecision = 65535;
                break;
            default:
                aTypeName = ::rtl::OUString::createFromAscii("OTHER");
                eType = DataType::OTHER;

        }

//      sal_Int32 nFlags = 0;
//      switch (aDBFColumn.db_typ)
//      {
//          case 'C':
//          case 'D':
//          case 'L':   nFlags = ColumnSearch::FULL; break;
//          case 'F':
//          case 'N':   nFlags = ColumnSearch::BASIC; break;
//          case 'M':   nFlags = ColumnSearch::CHAR; break;
//          default:
//                      nFlags = ColumnSearch::NONE;
//
//      }

        m_aTypes.push_back(eType);
        m_aPrecisions.push_back(nPrecision);
        m_aScales.push_back(aDBFColumn.db_dez);

        Reference< XPropertySet> xCol = new sdbcx::OColumn(aColumnName,
                                                    aTypeName,
                                                    ::rtl::OUString(),
                                                    ColumnValue::NULLABLE,
                                                    nPrecision,
                                                    aDBFColumn.db_dez,
                                                    eType,
                                                    sal_False,
                                                    sal_False,
                                                    sal_False,
                                                    bCase);
        m_aColumns->get().push_back(xCol);
    } // for (; i < nFieldCount; i++)
    OSL_ENSURE(i,"No columns in table!");
}
// -------------------------------------------------------------------------
ODbaseTable::ODbaseTable(sdbcx::OCollection* _pTables,ODbaseConnection* _pConnection)
        :ODbaseTable_BASE(_pTables,_pConnection)
        ,m_pMemoStream(NULL)
        ,m_bWriteableMemo(sal_False)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::ODbaseTable" );
    // initialize the header
    m_aHeader.db_typ    = dBaseIII;
    m_aHeader.db_anz    = 0;
    m_aHeader.db_kopf   = 0;
    m_aHeader.db_slng   = 0;
    m_eEncoding = getConnection()->getTextEncoding();
}
// -------------------------------------------------------------------------
ODbaseTable::ODbaseTable(sdbcx::OCollection* _pTables,ODbaseConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type,
                    const ::rtl::OUString& _Description ,
                    const ::rtl::OUString& _SchemaName,
                    const ::rtl::OUString& _CatalogName
                ) : ODbaseTable_BASE(_pTables,_pConnection,_Name,
                                  _Type,
                                  _Description,
                                  _SchemaName,
                                  _CatalogName)
                ,m_pMemoStream(NULL)
                ,m_bWriteableMemo(sal_False)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::ODbaseTable" );
    m_eEncoding = getConnection()->getTextEncoding();
}

// -----------------------------------------------------------------------------
void ODbaseTable::construct()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::construct" );
    // initialize the header
    m_aHeader.db_typ    = dBaseIII;
    m_aHeader.db_anz    = 0;
    m_aHeader.db_kopf   = 0;
    m_aHeader.db_slng   = 0;
    m_aMemoHeader.db_size = 0;

    String sFileName(getEntry(m_pConnection,m_Name));

    INetURLObject aURL;
    aURL.SetURL(sFileName);

    OSL_ENSURE( m_pConnection->matchesExtension( aURL.getExtension() ),
        "ODbaseTable::ODbaseTable: invalid extension!");
        // getEntry is expected to ensure the corect file name

    m_pFileStream = createStream_simpleError( sFileName, STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYWRITE);
    m_bWriteable = ( m_pFileStream != NULL );

    if ( !m_pFileStream )
    {
        m_bWriteable = sal_False;
        m_pFileStream = createStream_simpleError( sFileName, STREAM_READ | STREAM_NOCREATE | STREAM_SHARE_DENYNONE);
    }

    if(m_pFileStream)
    {
        readHeader();
        if (HasMemoFields())
        {
            // Memo-Dateinamen bilden (.DBT):
            // nyi: Unschoen fuer Unix und Mac!

            if ( m_aHeader.db_typ == FoxProMemo || VisualFoxPro == m_aHeader.db_typ ) // foxpro uses another extension
                aURL.SetExtension(String::CreateFromAscii("fpt"));
            else
                aURL.SetExtension(String::CreateFromAscii("dbt"));

            // Wenn die Memodatei nicht gefunden wird, werden die Daten trotzdem angezeigt
            // allerdings koennen keine Updates durchgefuehrt werden
            // jedoch die Operation wird ausgefuehrt
            m_pMemoStream = createStream_simpleError( aURL.GetMainURL(INetURLObject::NO_DECODE), STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYWRITE);
            if ( !m_pMemoStream )
            {
                m_bWriteableMemo = sal_False;
                m_pMemoStream = createStream_simpleError( aURL.GetMainURL(INetURLObject::NO_DECODE), STREAM_READ | STREAM_NOCREATE | STREAM_SHARE_DENYNONE);
            }
            if (m_pMemoStream)
                ReadMemoHeader();
        }
        //  if(!m_pColumns && (!m_aColumns.isValid() || !m_aColumns->size()))
        fillColumns();

        UINT32 nFileSize = lcl_getFileSize(*m_pFileStream);
        m_pFileStream->Seek(STREAM_SEEK_TO_BEGIN);
        if ( m_aHeader.db_anz == 0 && ((nFileSize-m_aHeader.db_kopf)/m_aHeader.db_slng) > 0) // seems to be empty or someone wrote bullshit into the dbase file
            m_aHeader.db_anz = ((nFileSize-m_aHeader.db_kopf)/m_aHeader.db_slng);

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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::ReadMemoHeader" );
    m_pMemoStream->SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);
    m_pMemoStream->RefreshBuffer();         // sicherstellen das die Kopfinformationen tatsaechlich neu gelesen werden
    m_pMemoStream->Seek(0L);

    (*m_pMemoStream) >> m_aMemoHeader.db_next;
    switch (m_aHeader.db_typ)
    {
        case dBaseIIIMemo:  // dBase III: feste Blockgroesse
        case dBaseIVMemo:
            // manchmal wird aber auch dBase3 dBase4 Memo zugeordnet
            m_pMemoStream->Seek(20L);
            (*m_pMemoStream) >> m_aMemoHeader.db_size;
            if (m_aMemoHeader.db_size > 1 && m_aMemoHeader.db_size != 512)  // 1 steht auch fuer dBase 3
                m_aMemoHeader.db_typ  = MemodBaseIV;
            else if (m_aMemoHeader.db_size > 1 && m_aMemoHeader.db_size == 512)
            {
                // nun gibt es noch manche Dateien, die verwenden eine Groessenangabe,
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
        case VisualFoxPro:
        case FoxProMemo:
            m_aMemoHeader.db_typ    = MemoFoxPro;
            m_pMemoStream->Seek(6L);
            m_pMemoStream->SetNumberFormatInt(NUMBERFORMAT_INT_BIGENDIAN);
            (*m_pMemoStream) >> m_aMemoHeader.db_size;
            break;
        default:
            OSL_ENSURE( false, "ODbaseTable::ReadMemoHeader: unsupported memo type!" );
            break;
    }
    return TRUE;
}
// -------------------------------------------------------------------------
String ODbaseTable::getEntry(OConnection* _pConnection,const ::rtl::OUString& _sName )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::getEntry" );
    ::rtl::OUString sURL;
    try
    {
        Reference< XResultSet > xDir = _pConnection->getDir()->getStaticResultSet();
        Reference< XRow> xRow(xDir,UNO_QUERY);
        ::rtl::OUString sName;
        ::rtl::OUString sExt;
        INetURLObject aURL;
        static const ::rtl::OUString s_sSeparator(RTL_CONSTASCII_USTRINGPARAM("/"));
        xDir->beforeFirst();
        while(xDir->next())
        {
            sName = xRow->getString(1);
            aURL.SetSmartProtocol(INET_PROT_FILE);
            String sUrl = _pConnection->getURL() +  s_sSeparator + sName;
            aURL.SetSmartURL( sUrl );

            // cut the extension
            sExt = aURL.getExtension();

            // name and extension have to coincide
            if ( _pConnection->matchesExtension( sExt ) )
            {
                sName = sName.replaceAt(sName.getLength()-(sExt.getLength()+1),sExt.getLength()+1,::rtl::OUString());
                if ( sName == _sName )
                {
                    Reference< XContentAccess > xContentAccess( xDir, UNO_QUERY );
                    sURL = xContentAccess->queryContentIdentifierString();
                    break;
                }
            }
        }
        xDir->beforeFirst(); // move back to before first record
    }
    catch(Exception&)
    {
        OSL_ASSERT(0);
    }
    return sURL;
}
// -------------------------------------------------------------------------
void ODbaseTable::refreshColumns()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::refreshColumns" );
    ::osl::MutexGuard aGuard( m_aMutex );

    TStringVector aVector;
    aVector.reserve(m_aColumns->get().size());

    for(OSQLColumns::Vector::const_iterator aIter = m_aColumns->get().begin();aIter != m_aColumns->get().end();++aIter)
        aVector.push_back(Reference< XNamed>(*aIter,UNO_QUERY)->getName());

    if(m_pColumns)
        m_pColumns->reFill(aVector);
    else
        m_pColumns  = new ODbaseColumns(this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void ODbaseTable::refreshIndexes()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::refreshIndexes" );
    TStringVector aVector;
    if(m_pFileStream && (!m_pIndexes || m_pIndexes->getCount() == 0))
    {
        INetURLObject aURL;
        aURL.SetURL(getEntry(m_pConnection,m_Name));

        aURL.setExtension(String::CreateFromAscii("inf"));
        Config aInfFile(aURL.getFSysPath(INetURLObject::FSYS_DETECT));
        aInfFile.SetGroup(dBASE_III_GROUP);
        USHORT nKeyCnt = aInfFile.GetKeyCount();
        ByteString aKeyName;
        ByteString aIndexName;

        for (USHORT nKey = 0; nKey < nKeyCnt; nKey++)
        {
            // Verweist der Key auf ein Indexfile?...
            aKeyName = aInfFile.GetKeyName( nKey );
            //...wenn ja, Indexliste der Tabelle hinzufuegen
            if (aKeyName.Copy(0,3) == ByteString("NDX") )
            {
                aIndexName = aInfFile.ReadKey(aKeyName);
                aURL.setName(String(aIndexName,m_eEncoding));
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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::disposing" );
    OFileTable::disposing();
    ::osl::MutexGuard aGuard(m_aMutex);
    m_aColumns = NULL;
}
// -------------------------------------------------------------------------
Sequence< Type > SAL_CALL ODbaseTable::getTypes(  ) throw(RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::getTypes" );
    Sequence< Type > aTypes = OTable_TYPEDEF::getTypes();
    ::std::vector<Type> aOwnTypes;
    aOwnTypes.reserve(aTypes.getLength());

    const Type* pBegin = aTypes.getConstArray();
    const Type* pEnd = pBegin + aTypes.getLength();
    for(;pBegin != pEnd;++pBegin)
    {
        if(!(*pBegin == ::getCppuType((const Reference<XKeysSupplier>*)0)   ||
            //  *pBegin == ::getCppuType((const Reference<XAlterTable>*)0)  ||
            *pBegin == ::getCppuType((const Reference<XDataDescriptorFactory>*)0)))
        {
            aOwnTypes.push_back(*pBegin);
        }
    }
    aOwnTypes.push_back(::getCppuType( (const Reference< ::com::sun::star::lang::XUnoTunnel > *)0 ));
    Type *pTypes = aOwnTypes.empty() ? 0 : &aOwnTypes[0];
    return Sequence< Type >(pTypes, aOwnTypes.size());
}

// -------------------------------------------------------------------------
Any SAL_CALL ODbaseTable::queryInterface( const Type & rType ) throw(RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::queryInterface" );
    if( rType == ::getCppuType((const Reference<XKeysSupplier>*)0)  ||
        rType == ::getCppuType((const Reference<XDataDescriptorFactory>*)0))
        return Any();

    Any aRet = OTable_TYPEDEF::queryInterface(rType);
    return aRet.hasValue() ? aRet : ::cppu::queryInterface(rType,static_cast< ::com::sun::star::lang::XUnoTunnel*> (this));
}

//--------------------------------------------------------------------------
Sequence< sal_Int8 > ODbaseTable::getUnoTunnelImplementationId()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::getUnoTunnelImplementationId" );
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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::getSomething" );
    return (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
                ? reinterpret_cast< sal_Int64 >( this )
                : ODbaseTable_BASE::getSomething(rId);
}
//------------------------------------------------------------------
sal_Bool ODbaseTable::fetchRow(OValueRefRow& _rRow,const OSQLColumns & _rCols, sal_Bool _bUseTableDefs,sal_Bool bRetrieveData)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::fetchRow" );
    // Einlesen der Daten
    BOOL bIsCurRecordDeleted = ((char)m_pBuffer[0] == '*') ? TRUE : sal_False;

    // only read the bookmark

    // Satz als geloescht markieren
    //  rRow.setState(bIsCurRecordDeleted ? ROW_DELETED : ROW_CLEAN );
    _rRow->setDeleted(bIsCurRecordDeleted);
    *(_rRow->get())[0] = m_nFilePos;

    if (!bRetrieveData)
        return TRUE;

    sal_Size nByteOffset = 1;
    // Felder:
    OSQLColumns::Vector::const_iterator aIter = _rCols.get().begin();
    OSQLColumns::Vector::const_iterator aEnd  = _rCols.get().end();
    const sal_Size nCount = _rRow->get().size();
    for (sal_Size i = 1; aIter != aEnd && nByteOffset <= m_nBufferSize && i < nCount;++aIter, i++)
    {
        // Laengen je nach Datentyp:
        sal_Int32 nLen = 0;
        sal_Int32 nType = 0;
        if(_bUseTableDefs)
        {
            nLen    = m_aPrecisions[i-1];
            nType   = m_aTypes[i-1];
        }
        else
        {
            (*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION)) >>= nLen;
            (*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))      >>= nType;
        }
        switch(nType)
        {
            case DataType::DATE:        nLen = 8; break;
            case DataType::DECIMAL:
                if(_bUseTableDefs)
                    nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,m_aScales[i-1]);
                else
                    nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,getINT32((*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))));
                break;  // das Vorzeichen und das Komma
            case DataType::BIT:         nLen = 1; break;
            case DataType::LONGVARCHAR: nLen = 10; break;
            case DataType::OTHER:
                nByteOffset += nLen;
                continue;
        }

        // Ist die Variable ueberhaupt gebunden?
        if ( !(_rRow->get())[i]->isBound() )
        {
            // Nein - naechstes Feld.
            nByteOffset += nLen;
            OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
            continue;
        }

        char *pData = (char *) (m_pBuffer + nByteOffset);

        //  (*_rRow)[i].setType(nType);

        if (nType == DataType::CHAR || nType == DataType::VARCHAR)
        {
            char cLast = pData[nLen];
            pData[nLen] = 0;
            String aStr(pData,(xub_StrLen)nLen,m_eEncoding);
            aStr.EraseTrailingChars();

            if ( aStr.Len() )
                *(_rRow->get())[i] = ::rtl::OUString(aStr);
            else// keine StringLaenge, dann NULL
                (_rRow->get())[i]->setNull();

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

            String aStr(pData, (xub_StrLen)nLen,m_eEncoding);       // Spaces am Anfang und am Ende entfernen:
            aStr.EraseLeadingChars();
            aStr.EraseTrailingChars();

            if (!aStr.Len())
            {
                nByteOffset += nLen;
                (_rRow->get())[i]->setNull();   // keine Werte -> fertig
                continue;
            }

            switch (nType)
            {
                case DataType::DATE:
                {
                    if (aStr.Len() != nLen)
                    {
                        (_rRow->get())[i]->setNull();
                        break;
                    }
                    const sal_uInt16  nYear   = (sal_uInt16)aStr.Copy( 0, 4 ).ToInt32();
                    const sal_uInt16  nMonth  = (sal_uInt16)aStr.Copy( 4, 2 ).ToInt32();
                    const sal_uInt16  nDay    = (sal_uInt16)aStr.Copy( 6, 2 ).ToInt32();

                    const ::com::sun::star::util::Date aDate(nDay,nMonth,nYear);
                    *(_rRow->get())[i] = aDate;
                }
                break;
                case DataType::DECIMAL:
                    *(_rRow->get())[i] = ORowSetValue(aStr);
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
                        default:    b = FALSE; break;
                    }
                    *(_rRow->get())[i] = b;
                    //  pVal->setDouble(b);
                }
                break;
                case DataType::LONGVARCHAR:
                {
                    const long nBlockNo = aStr.ToInt32();   // Blocknummer lesen
                    if (nBlockNo > 0 && m_pMemoStream) // Daten aus Memo-Datei lesen, nur wenn
                    {
                        if ( !ReadMemo(nBlockNo, (_rRow->get())[i]->get()) )
                            break;
                    }
                    else
                        (_rRow->get())[i]->setNull();
                }   break;
                default:
                    OSL_ASSERT("Falscher Type");
            }
            (_rRow->get())[i]->setTypeKind(nType);
        }

//      if (aStatus.IsError())
//          break;
        // Und weiter ...
        nByteOffset += nLen;
        OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
    }
    return sal_True;
}
//------------------------------------------------------------------
// -------------------------------------------------------------------------
void ODbaseTable::FileClose()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::FileClose" );
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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::CreateImpl" );
    OSL_ENSURE(!m_pFileStream, "SequenceError");

    if ( m_pConnection->isCheckEnabled() && ::dbtools::convertName2SQLName(m_Name,::rtl::OUString()) != m_Name )
    {
        const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                STR_SQL_NAME_ERROR,
                "$name$", m_Name
             ) );
        ::dbtools::throwGenericSQLException( sError, *this );
    }

    INetURLObject aURL;
    aURL.SetSmartProtocol(INET_PROT_FILE);
    String aName = getEntry(m_pConnection,m_Name);
    if(!aName.Len())
    {
        ::rtl::OUString aIdent = m_pConnection->getContent()->getIdentifier()->getContentIdentifier();
        if ( aIdent.lastIndexOf('/') != (aIdent.getLength()-1) )
            aIdent += ::rtl::OUString::createFromAscii("/");
        aIdent += m_Name;
        aName = aIdent.getStr();
    }
    aURL.SetURL(aName);

    if ( !m_pConnection->matchesExtension( aURL.getExtension() ) )
        aURL.setExtension(m_pConnection->getExtension());

    try
    {
        Content aContent(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());
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
        try
        {
            Content aContent(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());
            aContent.executeCommand( rtl::OUString::createFromAscii( "delete" ),bool2any( sal_True ) );
        }
        catch(Exception&) // a execption is thrown when no file exists
        {
        }
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

                const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                        STR_COULD_NOT_DELETE_FILE,
                        "$name$", aName
                     ) );
                ::dbtools::throwGenericSQLException( sError, *this );
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

    return TRUE;
}
// -----------------------------------------------------------------------------
void ODbaseTable::throwInvalidColumnType(const sal_uInt16 _nErrorId,const ::rtl::OUString& _sColumnName)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::throwInvalidColumnType" );
    try
    {
        // we have to drop the file because it is corrupted now
        DropImpl();
    }
    catch(const Exception&)
    {
    }

    const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
            _nErrorId,
            "$columnname$", _sColumnName
         ) );
    ::dbtools::throwGenericSQLException( sError, *this );
}
//------------------------------------------------------------------
// erzeugt grundsaetzlich dBase IV Datei Format
BOOL ODbaseTable::CreateFile(const INetURLObject& aFile, BOOL& bCreateMemo)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::CreateFile" );
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
    (*m_pFileStream) << 0L;                                                                                                     // Anzahl der Datensaetze
    (*m_pFileStream) << (USHORT)((m_pColumns->getCount()+1) * 32 + 1);                // Kopfinformationen,
                                                                        // pColumns erhaelt immer eine Spalte mehr
    (*m_pFileStream) << (USHORT) 0;                                                                                     // Satzlaenge wird spaeter bestimmt
    m_pFileStream->Write(aBuffer, 20);

    USHORT nRecLength = 1;                                                                                          // Laenge 1 fuer deleted flag
    sal_Int32  nMaxFieldLength = m_pConnection->getMetaData()->getMaxColumnNameLength();
    Reference<XIndexAccess> xColumns(getColumns(),UNO_QUERY);

    ::rtl::OUString aName;
    Reference<XPropertySet> xCol;
    try
    {
        for(sal_Int32 i=0;i<xColumns->getCount();++i)
        {
            ::cppu::extractInterface(xCol,xColumns->getByIndex(i));
            OSL_ENSURE(xCol.is(),"This should be a column!");

            char cTyp( 'C' );

            xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aName;

            ::rtl::OString aCol;
            if ( DBTypeConversion::convertUnicodeString( aName, aCol, m_eEncoding ) > nMaxFieldLength)
            {
                throwInvalidColumnType( STR_INVALID_COLUMN_NAME_LENGTH, aName );
            }

            (*m_pFileStream) << aCol.getStr();
            m_pFileStream->Write(aBuffer, 11 - aCol.getLength());

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
                        throwInvalidColumnType(STR_INVALID_COLUMN_TYPE, aName);
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
                        throwInvalidColumnType(STR_INVALID_COLUMN_PRECISION, aName);
                    }
                    (*m_pFileStream) << (BYTE) Min((ULONG)nPrecision, 255UL);      //Feldlaenge
                    nRecLength = nRecLength + (USHORT)::std::min((USHORT)nPrecision, (USHORT)255UL);
                    (*m_pFileStream) << (BYTE)0;                                                                //Nachkommastellen
                    break;
                case 'F':
                case 'N':
                    OSL_ENSURE(nPrecision >=  nScale,
                            "ODbaseTable::Create: Feldlaenge muss groesser Nachkommastellen sein!");
                    if (nPrecision <  nScale)
                    {
                        throwInvalidColumnType(STR_INVALID_PRECISION_SCALE, aName);
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
                        nRecLength = nRecLength + (USHORT)nPrec;
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
                    throwInvalidColumnType(STR_INVALID_COLUMN_TYPE, aName);
            }
            m_pFileStream->Write(aBuffer, 14);
        }

        (*m_pFileStream) << (BYTE)FIELD_DESCRIPTOR_TERMINATOR;              // kopf ende
        (*m_pFileStream) << (char)DBF_EOL;
        m_pFileStream->Seek(10L);
        (*m_pFileStream) << nRecLength;                                     // Satzlaenge nachtraeglich eintragen

        if (bCreateMemo)
        {
            m_pFileStream->Seek(0L);
            (*m_pFileStream) << (BYTE) dBaseIIIMemo;
        } // if (bCreateMemo)
    }
    catch ( const Exception& e )
    {
        (void)e;

        try
        {
            // we have to drop the file because it is corrupted now
            DropImpl();
        }
        catch(const Exception&) { }
        throw;
    }
    return TRUE;
}

//------------------------------------------------------------------
// erzeugt grundsaetzlich dBase III Datei Format
BOOL ODbaseTable::CreateMemoFile(const INetURLObject& aFile)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::CreateMemoFile" );
    // Makro zum Filehandling fuers Erzeugen von Tabellen
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
BOOL ODbaseTable::Drop_Static(const ::rtl::OUString& _sUrl,sal_Bool _bHasMemoFields,OCollection* _pIndexes )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::Drop_Static" );
    INetURLObject aURL;
    aURL.SetURL(_sUrl);

    BOOL bDropped = ::utl::UCBContentHelper::Kill(aURL.GetMainURL(INetURLObject::NO_DECODE));

    if(bDropped)
    {
        if (_bHasMemoFields)
        {  // delete the memo fields
            aURL.setExtension(String::CreateFromAscii("dbt"));
            bDropped = ::utl::UCBContentHelper::Kill(aURL.GetMainURL(INetURLObject::NO_DECODE));
        }

        if(bDropped)
        {
            if(_pIndexes)
            {
                try
                {
                    sal_Int32 i = _pIndexes->getCount();
                    while (i)
                    {
                        _pIndexes->dropByIndex(--i);
                    }
                }
                catch(SQLException)
                {
                }
            }
            //  aFile.SetBase(m_Name);
            aURL.setExtension(String::CreateFromAscii("inf"));

            // as the inf file does not necessarily exist, we aren't allowed to use UCBContentHelper::Kill
            // 89711 - 16.07.2001 - frank.schoenheit@sun.com
            try
            {
                ::ucbhelper::Content aDeleteContent( aURL.GetMainURL( INetURLObject::NO_DECODE ), Reference< XCommandEnvironment > () );
                aDeleteContent.executeCommand( ::rtl::OUString::createFromAscii( "delete" ), makeAny( sal_Bool( sal_True ) ) );
            }
            catch(Exception&)
            {
                // silently ignore this ....
            }
        }
    }
    return bDropped;
}
// -----------------------------------------------------------------------------
BOOL ODbaseTable::DropImpl()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::DropImpl" );
    FileClose();

    if(!m_pIndexes)
        refreshIndexes(); // look for indexes which must be deleted as well

    BOOL bDropped = Drop_Static(getEntry(m_pConnection,m_Name),HasMemoFields(),m_pIndexes);
    if(!bDropped)
    {// we couldn't drop the table so we have to reopen it
        construct();
        if(m_pColumns)
            m_pColumns->refresh();
    }
    return bDropped;
}

//------------------------------------------------------------------
BOOL ODbaseTable::InsertRow(OValueRefVector& rRow, BOOL bFlush,const Reference<XIndexAccess>& _xCols)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::InsertRow" );
    // Buffer mit Leerzeichen fuellen
    AllocBuffer();
    memset(m_pBuffer, ' ', m_aHeader.db_slng);

    // Gesamte neue Row uebernehmen:
    // ... und am Ende als neuen Record hinzufuegen:
    UINT32 nTempPos = m_nFilePos,
           nFileSize = 0,
           nMemoFileSize = 0;

    m_nFilePos = (ULONG)m_aHeader.db_anz + 1;
    BOOL bInsertRow = UpdateBuffer( rRow, NULL, _xCols );
    if ( bInsertRow )
    {
        nFileSize = lcl_getFileSize(*m_pFileStream);

        if (HasMemoFields() && m_pMemoStream)
        {
            m_pMemoStream->Seek(STREAM_SEEK_TO_END);
            nMemoFileSize = m_pMemoStream->Tell();
        }

        if (!WriteBuffer())
        {
            m_pFileStream->SetStreamSize(nFileSize);                // alte Groesse restaurieren

            if (HasMemoFields() && m_pMemoStream)
                m_pMemoStream->SetStreamSize(nMemoFileSize);    // alte Groesse restaurieren
            m_nFilePos = nTempPos;                              // Fileposition restaurieren
        }
        else
        {
            (*m_pFileStream) << (char)DBF_EOL; // write EOL
            // Anzahl Datensaetze im Header erhoehen:
            m_pFileStream->Seek( 4L );
            (*m_pFileStream) << (m_aHeader.db_anz + 1);

            // beim AppendOnly kein Flush!
            if (bFlush)
                m_pFileStream->Flush();

            // bei Erfolg # erhoehen
            m_aHeader.db_anz++;
            *rRow.get()[0] = m_nFilePos;                                // BOOKmark setzen
            m_nFilePos = nTempPos;
        }
    }
    else
        m_nFilePos = nTempPos;

    return bInsertRow;
}

//------------------------------------------------------------------
BOOL ODbaseTable::UpdateRow(OValueRefVector& rRow, OValueRefRow& pOrgRow,const Reference<XIndexAccess>& _xCols)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::UpdateRow" );
    // Buffer mit Leerzeichen fuellen
    AllocBuffer();

    // Auf gewuenschten Record positionieren:
    long nPos = m_aHeader.db_kopf + (long)(m_nFilePos-1) * m_aHeader.db_slng;
    m_pFileStream->Seek(nPos);
    m_pFileStream->Read((char*)m_pBuffer, m_aHeader.db_slng);

    UINT32 nMemoFileSize( 0 );
    if (HasMemoFields() && m_pMemoStream)
    {
        m_pMemoStream->Seek(STREAM_SEEK_TO_END);
        nMemoFileSize = m_pMemoStream->Tell();
    }
    if (!UpdateBuffer(rRow, pOrgRow,_xCols) || !WriteBuffer())
    {
        if (HasMemoFields() && m_pMemoStream)
            m_pMemoStream->SetStreamSize(nMemoFileSize);    // alte Groesse restaurieren
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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::DeleteRow" );
    // Einfach das Loesch-Flag setzen (egal, ob es schon gesetzt war
    // oder nicht):
    // Auf gewuenschten Record positionieren:
    long nFilePos = m_aHeader.db_kopf + (long)(m_nFilePos-1) * m_aHeader.db_slng;
    m_pFileStream->Seek(nFilePos);

    OValueRefRow aRow = new OValueRefVector(_rCols.get().size());

    if (!fetchRow(aRow,_rCols,TRUE,TRUE))
        return sal_False;

    Reference<XPropertySet> xCol;
    ::rtl::OUString aColName;
    ::comphelper::UStringMixEqual aCase(isCaseSensitive());
    for (USHORT i = 0; i < m_pColumns->getCount(); i++)
    {
        Reference<XPropertySet> xIndex = isUniqueByColumnName(i);
        if (xIndex.is())
        {
            ::cppu::extractInterface(xCol,m_pColumns->getByIndex(i));
            OSL_ENSURE(xCol.is(),"ODbaseTable::DeleteRow column is null!");
            if(xCol.is())
            {
                xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;

                Reference<XUnoTunnel> xTunnel(xIndex,UNO_QUERY);
                OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
                ODbaseIndex* pIndex = reinterpret_cast< ODbaseIndex* >( xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId()) );
                OSL_ENSURE(pIndex,"ODbaseTable::DeleteRow: No Index returned!");

                OSQLColumns::Vector::const_iterator aIter = _rCols.get().begin();
                sal_Int32 nPos = 1;
                for(;aIter != _rCols.get().end();++aIter,++nPos)
                {
                    if(aCase(getString((*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_REALNAME))),aColName))
                        break;
                }
                if (aIter == _rCols.get().end())
                    continue;

                pIndex->Delete(m_nFilePos,*(aRow->get())[nPos]);
            }
        }
    }

    m_pFileStream->Seek(nFilePos);
    (*m_pFileStream) << (BYTE)'*'; // mark the row in the table as deleted
    m_pFileStream->Flush();
    return sal_True;
}
// -------------------------------------------------------------------------
Reference<XPropertySet> ODbaseTable::isUniqueByColumnName(sal_Int32 _nColumnPos)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::isUniqueByColumnName" );
    if(!m_pIndexes)
        refreshIndexes();
    if(m_pIndexes->hasElements())
    {
        Reference<XPropertySet> xCol;
        m_pColumns->getByIndex(_nColumnPos) >>= xCol;
        OSL_ENSURE(xCol.is(),"ODbaseTable::isUniqueByColumnName column is null!");
        ::rtl::OUString sColName;
        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= sColName;

        Reference<XPropertySet> xIndex;
        for(sal_Int32 i=0;i<m_pIndexes->getCount();++i)
        {
            ::cppu::extractInterface(xIndex,m_pIndexes->getByIndex(i));
            if(xIndex.is() && getBOOL(xIndex->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISUNIQUE))))
            {
                Reference<XNameAccess> xCols(Reference<XColumnsSupplier>(xIndex,UNO_QUERY)->getColumns());
                if(xCols->hasByName(sColName))
                    return xIndex;

            }
        }
    }
    return Reference<XPropertySet>();
}
//------------------------------------------------------------------
double toDouble(const ByteString& rString)
{
    return ::rtl::math::stringToDouble( rString, '.', ',', NULL, NULL );
}

//------------------------------------------------------------------
BOOL ODbaseTable::UpdateBuffer(OValueRefVector& rRow, OValueRefRow pOrgRow,const Reference<XIndexAccess>& _xCols)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::UpdateBuffer" );
    OSL_ENSURE(m_pBuffer,"Buffer is NULL!");
    if ( !m_pBuffer )
        return FALSE;
    sal_Int32 nByteOffset  = 1;

    // Felder aktualisieren:
    Reference<XPropertySet> xCol;
    Reference<XPropertySet> xIndex;
    USHORT i;
    ::rtl::OUString aColName;
    sal_Int32 nColumnCount = m_pColumns->getCount();
    ::std::vector< Reference<XPropertySet> > aIndexedCols(nColumnCount);

    ::comphelper::UStringMixEqual aCase(isCaseSensitive());

    Reference<XIndexAccess> xColumns = m_pColumns;
    // first search a key that exist already in the table
    for (i = 0; i < nColumnCount; ++i)
    {
        sal_Int32 nPos = i;
        if(_xCols != xColumns)
        {
            m_pColumns->getByIndex(i) >>= xCol;
            OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
            xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;

            for(nPos = 0;nPos<_xCols->getCount();++nPos)
            {
                Reference<XPropertySet> xFindCol;
                ::cppu::extractInterface(xFindCol,_xCols->getByIndex(nPos));
                OSL_ENSURE(xFindCol.is(),"ODbaseTable::UpdateBuffer column is null!");
                if(aCase(getString(xFindCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))),aColName))
                    break;
            }
            if (nPos >= _xCols->getCount())
                continue;
        }

        ++nPos;
        xIndex = isUniqueByColumnName(i);
        aIndexedCols[i] = xIndex;
        if (xIndex.is())
        {
            // first check if the value is different to the old one and when if it conform to the index
            if(pOrgRow.isValid() && (rRow.get()[nPos]->getValue().isNull() || rRow.get()[nPos] == (pOrgRow->get())[nPos]))
                continue;
            else
            {
                //  ODbVariantRef xVar = (pVal == NULL) ? new ODbVariant() : pVal;
                Reference<XUnoTunnel> xTunnel(xIndex,UNO_QUERY);
                OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
                ODbaseIndex* pIndex = reinterpret_cast< ODbaseIndex* >( xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId()) );
                OSL_ENSURE(pIndex,"ODbaseTable::UpdateBuffer: No Index returned!");

                if (pIndex->Find(0,*rRow.get()[nPos]))
                {
                    // es existiert kein eindeutiger Wert
                    if ( !aColName.getLength() )
                    {
                        m_pColumns->getByIndex(i) >>= xCol;
                        OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
                        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;
                        xCol = NULL;
                    } // if ( !aColName.getLength() )
                    throwInvalidColumnType(STR_DUPLICATE_VALUE_IN_COLUMN,aColName);
                }
            }
        }
    }

    // when we are here there is no double key in the table

    for (i = 0; i < nColumnCount && nByteOffset <= m_nBufferSize ; ++i)
    {
        // Laengen je nach Datentyp:
        OSL_ENSURE(i < m_aPrecisions.size(),"Illegal index!");
        sal_Int32 nLen = 0;
        sal_Int32 nType = 0;
        sal_Int32 nScale = 0;
        if ( i < m_aPrecisions.size() )
        {
            nLen    = m_aPrecisions[i];
            nType   = m_aTypes[i];
            nScale  = m_aScales[i];
        }
        else
        {
            m_pColumns->getByIndex(i) >>= xCol;
            if ( xCol.is() )
            {
                xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION)) >>= nLen;
                xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))      >>= nType;
                xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))     >>= nScale;
            }
        }

        switch (nType)
        {
            case DataType::DATE:        nLen = 8; break;
            case DataType::DECIMAL:
                nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,nScale);
                break;  // das Vorzeichen und das Komma
            case DataType::BIT:         nLen = 1; break;
            case DataType::LONGVARCHAR: nLen = 10; break;
            default:                    break;

        } // switch (nType)

        sal_Int32 nPos = i;
        if(_xCols != xColumns)
        {
            m_pColumns->getByIndex(i) >>= xCol;
            OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
            xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;
            for(nPos = 0;nPos<_xCols->getCount();++nPos)
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
        }



        ++nPos; // the row values start at 1
        // Ist die Variable ueberhaupt gebunden?
        if ( !rRow.get()[nPos]->isBound() )
        {
            // Nein - naechstes Feld.
            nByteOffset += nLen;
            continue;
        }
        if (aIndexedCols[i].is())
        {
            Reference<XUnoTunnel> xTunnel(aIndexedCols[i],UNO_QUERY);
            OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
            ODbaseIndex* pIndex = reinterpret_cast< ODbaseIndex* >( xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId()) );
            OSL_ENSURE(pIndex,"ODbaseTable::UpdateBuffer: No Index returned!");
            // Update !!
            if (pOrgRow.isValid() && !rRow.get()[nPos]->getValue().isNull() )//&& pVal->isModified())
                pIndex->Update(m_nFilePos,*(pOrgRow->get())[nPos],*rRow.get()[nPos]);
            else
                pIndex->Insert(m_nFilePos,*rRow.get()[nPos]);
        }

        char* pData = (char *)(m_pBuffer + nByteOffset);
        if (rRow.get()[nPos]->getValue().isNull())
        {
            memset(pData,' ',nLen); // Zuruecksetzen auf NULL
            nByteOffset += nLen;
            OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
            continue;
        }

        sal_Bool bHadError = sal_False;
        try
        {
            switch (nType)
            {
                case DataType::DATE:
                {
                    ::com::sun::star::util::Date aDate;
                    if(rRow.get()[nPos]->getValue().getTypeKind() == DataType::DOUBLE)
                        aDate = ::dbtools::DBTypeConversion::toDate(rRow.get()[nPos]->getValue().getDouble());
                    else
                        aDate = rRow.get()[nPos]->getValue();
                    char s[9];
                    snprintf(s,
                        sizeof(s),
                        "%04d%02d%02d",
                        (int)aDate.Year,
                        (int)aDate.Month,
                        (int)aDate.Day);

                    // Genau 8 Byte kopieren:
                    strncpy(pData,s,sizeof s - 1);
                } break;
                case DataType::DECIMAL:
                {
                    memset(pData,' ',nLen); // Zuruecksetzen auf NULL

                    const double n = rRow.get()[nPos]->getValue();

                    // ein const_cast, da GetFormatPrecision am SvNumberFormat nicht const ist, obwohl es das eigentlich
                    // sein koennte und muesste

                    const ByteString aDefaultValue( ::rtl::math::doubleToString( n, rtl_math_StringFormat_F, nScale, '.', NULL, 0));
                    BOOL bValidLength  = aDefaultValue.Len() <= nLen;
                    if ( bValidLength )
                    {
                        strncpy(pData,aDefaultValue.GetBuffer(),nLen);
                        // write the resulting double back
                        *rRow.get()[nPos] = toDouble(aDefaultValue);
                    }
                    else
                    {
                        m_pColumns->getByIndex(i) >>= xCol;
                        OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
                        xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;
                        ::std::list< ::std::pair<const sal_Char* , ::rtl::OUString > > aStringToSubstitutes;
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$columnname$", aColName));
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$precision$", String::CreateFromInt32(nLen)));
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$scale$", String::CreateFromInt32(nScale)));
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$value$", ::rtl::OStringToOUString(aDefaultValue,RTL_TEXTENCODING_UTF8)));

                        const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                                STR_INVALID_COLUMN_DECIMAL_VALUE
                                ,aStringToSubstitutes
                             ) );
                        ::dbtools::throwGenericSQLException( sError, *this );
                    }
                } break;
                case DataType::BIT:
                    *pData = rRow.get()[nPos]->getValue().getBool() ? 'T' : 'F';
                    break;
                case DataType::LONGVARCHAR:
                {
                    char cNext = pData[nLen]; // merken und temporaer durch 0 ersetzen
                    pData[nLen] = '\0';       // das geht, da der Puffer immer ein Zeichen groesser ist ...

                    ULONG nBlockNo = strtol((const char *)pData,NULL,10);   // Blocknummer lesen

                    // Naechstes Anfangszeichen wieder restaurieren:
                    pData[nLen] = cNext;
                    if (!m_pMemoStream || !WriteMemo(rRow.get()[nPos]->get(), nBlockNo))
                        break;

                    ByteString aStr;
                    ByteString aBlock(ByteString::CreateFromInt32(nBlockNo));
                    aStr.Expand(static_cast<sal_uInt16>(nLen - aBlock.Len()), '0' );
                    aStr += aBlock;
                    // Zeichen kopieren:
                    memset(pData,' ',nLen); // Zuruecksetzen auf NULL
                    memcpy(pData, aStr.GetBuffer(), nLen);
                }   break;
                default:
                {
                    memset(pData,' ',nLen); // Zuruecksetzen auf NULL

                    ::rtl::OUString sStringToWrite( rRow.get()[nPos]->getValue().getString() );

                    // convert the string, using the connection's encoding
                    ::rtl::OString sEncoded;

                    DBTypeConversion::convertUnicodeStringToLength( sStringToWrite, sEncoded, nLen, m_eEncoding );
                    memcpy( pData, sEncoded.getStr(), sEncoded.getLength() );

                }
                break;
            }
        }
        catch( SQLException&  )
        {
            throw;
        }
        catch ( Exception& ) { bHadError = sal_True; }

        if ( bHadError )
        {
            m_pColumns->getByIndex(i) >>= xCol;
            OSL_ENSURE( xCol.is(), "ODbaseTable::UpdateBuffer column is null!" );
            if ( xCol.is() )
                xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;

            const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                    STR_INVALID_COLUMN_VALUE,
                    "$columnname$", aColName
                 ) );
            ::dbtools::throwGenericSQLException( sError, *this );
        }
        // Und weiter ...
        nByteOffset += nLen;
        OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
    }
    return sal_True;
}

// -----------------------------------------------------------------------------
BOOL ODbaseTable::WriteMemo(ORowSetValue& aVariable, ULONG& rBlockNr)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::WriteMemo" );
    // wird die BlockNr 0 vorgegeben, wird der block ans Ende gehaengt

    BYTE nHeader[4];

    ::rtl::OUString sStringToWrite( aVariable.getString() );
    ::rtl::OString aStr;
    ULONG nSize = DBTypeConversion::convertUnicodeString( sStringToWrite, aStr, m_eEncoding );

    // Anhaengen oder ueberschreiben
    BOOL bAppend = rBlockNr == 0;

    if (!bAppend)
    {
        switch (m_aMemoHeader.db_typ)
        {
            case MemodBaseIII: // dBase III-Memofeld, endet mit 2 * Ctrl-Z
                bAppend = nSize > (512 - 2);
                break;
            case MemoFoxPro:
            case MemodBaseIV: // dBase IV-Memofeld mit Laengenangabe
            {
                char sHeader[4];
                m_pMemoStream->Seek(rBlockNr * m_aMemoHeader.db_size);
                m_pMemoStream->SeekRel(4L);
                m_pMemoStream->Read(sHeader,4);

                ULONG nOldSize;
                if (m_aMemoHeader.db_typ == MemoFoxPro)
                    nOldSize = ((((unsigned char)sHeader[0]) * 256 +
                                 (unsigned char)sHeader[1]) * 256 +
                                 (unsigned char)sHeader[2]) * 256 +
                                 (unsigned char)sHeader[3];
                else
                    nOldSize = ((((unsigned char)sHeader[3]) * 256 +
                                 (unsigned char)sHeader[2]) * 256 +
                                 (unsigned char)sHeader[1]) * 256 +
                                 (unsigned char)sHeader[0]  - 8;

                // passt die neue Laenge in die belegten Bloecke
                ULONG nUsedBlocks = ((nSize + 8) / m_aMemoHeader.db_size) + (((nSize + 8) % m_aMemoHeader.db_size > 0) ? 1 : 0),
                      nOldUsedBlocks = ((nOldSize + 8) / m_aMemoHeader.db_size) + (((nOldSize + 8) % m_aMemoHeader.db_size > 0) ? 1 : 0);
                bAppend = nUsedBlocks > nOldUsedBlocks;
            }
        }
    }

    if (bAppend)
    {
        ULONG nStreamSize = m_pMemoStream->Seek(STREAM_SEEK_TO_END);
        // letzten block auffuellen
        rBlockNr = (nStreamSize / m_aMemoHeader.db_size) + ((nStreamSize % m_aMemoHeader.db_size) > 0 ? 1 : 0);

        m_pMemoStream->SetStreamSize(rBlockNr * m_aMemoHeader.db_size);
        m_pMemoStream->Seek(STREAM_SEEK_TO_END);
    }
    else
    {
        m_pMemoStream->Seek(rBlockNr * m_aMemoHeader.db_size);
    }

    switch (m_aMemoHeader.db_typ)
    {
        case MemodBaseIII: // dBase III-Memofeld, endet mit Ctrl-Z
        {
            const char cEOF = (char) DBF_EOL;
            nSize++;

//          if (pData)
//          {
//              m_pMemoStream->Write((const char*) pData->getConstArray(), pData->getLength());
//          }
//          else
//          {
                m_pMemoStream->Write( aStr.getStr(), aStr.getLength() );
            //  }

            (*m_pMemoStream) << cEOF << cEOF;
        } break;
        case MemoFoxPro:
        case MemodBaseIV: // dBase IV-Memofeld mit Laengenangabe
        {
            (*m_pMemoStream) << (BYTE)0xFF
                                         << (BYTE)0xFF
                                         << (BYTE)0x08;

            UINT32 nWriteSize = nSize;
            if (m_aMemoHeader.db_typ == MemoFoxPro)
            {
                (*m_pMemoStream) << (BYTE) 0x01; // ((pData = NULL) ? 0x01 : 0x00);
                for (int i = 4; i > 0; nWriteSize >>= 8)
                    nHeader[--i] = (BYTE) (nWriteSize % 256);
            }
            else
            {
                (*m_pMemoStream) << (BYTE) 0x00;
                nWriteSize += 8;
                for (int i = 0; i < 4; nWriteSize >>= 8)
                    nHeader[i++] = (BYTE) (nWriteSize % 256);
            }

            m_pMemoStream->Write(nHeader,4);
//          if (pData)
//          {
//              m_pMemoStream->Write((const char*) pData->getConstArray(), pData->getLength());
//          }
//          else
//          {
                m_pMemoStream->Write( aStr.getStr(), aStr.getLength() );
            //  }
            m_pMemoStream->Flush();
        }
    }


    // Schreiben der neuen Blocknummer
    if (bAppend)
    {
        ULONG nStreamSize = m_pMemoStream->Seek(STREAM_SEEK_TO_END);
        m_aMemoHeader.db_next = (nStreamSize / m_aMemoHeader.db_size) + ((nStreamSize % m_aMemoHeader.db_size) > 0 ? 1 : 0);

        // Schreiben der neuen Blocknummer
        m_pMemoStream->Seek(0L);
        (*m_pMemoStream) << m_aMemoHeader.db_next;
        m_pMemoStream->Flush();
    }
    return sal_True;
}

// -----------------------------------------------------------------------------
// XAlterTable
void SAL_CALL ODbaseTable::alterColumnByName( const ::rtl::OUString& colName, const Reference< XPropertySet >& descriptor ) throw(SQLException, NoSuchElementException, RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::alterColumnByName" );
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OTableDescriptor_BASE::rBHelper.bDisposed);


    Reference<XDataDescriptorFactory> xOldColumn;
    m_pColumns->getByName(colName) >>= xOldColumn;

    alterColumn(m_pColumns->findColumn(colName)-1,descriptor,xOldColumn);
}
// -------------------------------------------------------------------------
void SAL_CALL ODbaseTable::alterColumnByIndex( sal_Int32 index, const Reference< XPropertySet >& descriptor ) throw(SQLException, ::com::sun::star::lang::IndexOutOfBoundsException, RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::alterColumnByIndex" );
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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::alterColumn" );
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

        pNewTable = new ODbaseTable(m_pTables,static_cast<ODbaseConnection*>(m_pConnection));
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
                xCpy = new OColumn(getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers());
            ::comphelper::copyProperties(xProp,xCpy);
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
                xCpy = new OColumn(getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers());
            ::comphelper::copyProperties(xProp,xCpy);
            xAppend->appendByDescriptor(xCpy);
        }

        // construct the new table
        if(!pNewTable->CreateImpl())
        {
            const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                    STR_COLUMN_NOT_ALTERABLE,
                    "$columnname$", ::comphelper::getString(descriptor->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)))
                 ) );
            ::dbtools::throwGenericSQLException( sError, *this );
        }

        pNewTable->construct();

        // copy the data
        copyData(pNewTable,0);

        // now drop the old one
        if( DropImpl() ) // we don't want to delete the memo columns too
        {
            // rename the new one to the old one
            pNewTable->renameImpl(m_Name);
            // release the temp file
            pNewTable = NULL;
            ::comphelper::disposeComponent(xHoldTable);
        }
        else
        {
            pNewTable = NULL;
        }
        FileClose();
        construct();
        if(m_pColumns)
            m_pColumns->refresh();

    }
    catch(const SQLException&)
    {
        throw;
    }
    catch(const Exception&)
    {
        OSL_ENSURE(0,"ODbaseTable::alterColumn: Exception occured!");
        throw;
    }
}
// -----------------------------------------------------------------------------
Reference< XDatabaseMetaData> ODbaseTable::getMetaData() const
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::getMetaData" );
    return getConnection()->getMetaData();
}
// -------------------------------------------------------------------------
void SAL_CALL ODbaseTable::rename( const ::rtl::OUString& newName ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::rename" );
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OTableDescriptor_BASE::rBHelper.bDisposed);
    if(m_pTables && m_pTables->hasByName(newName))
        throw ElementExistException(newName,*this);


    renameImpl(newName);

    ODbaseTable_BASE::rename(newName);

    construct();
    if(m_pColumns)
        m_pColumns->refresh();
}
namespace
{
    void renameFile(OConnection* _pConenction,const ::rtl::OUString& oldName,
                    const ::rtl::OUString& newName,const String& _sExtension)
    {
        String aName = ODbaseTable::getEntry(_pConenction,oldName);
        if(!aName.Len())
        {
            ::rtl::OUString aIdent = _pConenction->getContent()->getIdentifier()->getContentIdentifier();
            if ( aIdent.lastIndexOf('/') != (aIdent.getLength()-1) )
                aIdent += ::rtl::OUString::createFromAscii("/");
            aIdent += oldName;
            aName = aIdent;
        }
        INetURLObject aURL;
        aURL.SetURL(aName);

        aURL.setExtension( _sExtension );
        String sNewName(newName);
        sNewName.AppendAscii(".");
        sNewName += _sExtension;

        try
        {
            Content aContent(aURL.GetMainURL(INetURLObject::NO_DECODE),Reference<XCommandEnvironment>());

            Sequence< PropertyValue > aProps( 1 );
            aProps[0].Name      = ::rtl::OUString::createFromAscii("Title");
            aProps[0].Handle    = -1; // n/a
            aProps[0].Value     = makeAny( ::rtl::OUString(sNewName) );
            Sequence< Any > aValues;
            aContent.executeCommand( rtl::OUString::createFromAscii( "setPropertyValues" ),makeAny(aProps) ) >>= aValues;
            if(aValues.getLength() && aValues[0].hasValue())
                throw Exception();
        }
        catch(Exception&)
        {
            throw ElementExistException(newName,NULL);
        }
    }
}
// -------------------------------------------------------------------------
void SAL_CALL ODbaseTable::renameImpl( const ::rtl::OUString& newName ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::getEntry" );
    ::osl::MutexGuard aGuard(m_aMutex);

    FileClose();


    renameFile(m_pConnection,m_Name,newName,m_pConnection->getExtension());
    if ( HasMemoFields() )
    {  // delete the memo fields
        String sExt = String::CreateFromAscii("dbt");
        renameFile(m_pConnection,m_Name,newName,sExt);
    }
}
// -----------------------------------------------------------------------------
void ODbaseTable::addColumn(const Reference< XPropertySet >& _xNewColumn)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::addColumn" );
    String sTempName = createTempFile();

    ODbaseTable* pNewTable = new ODbaseTable(m_pTables,static_cast<ODbaseConnection*>(m_pConnection));
    Reference< XPropertySet > xHold = pNewTable;
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
        const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                STR_COLUMN_NOT_ADDABLE,
                "$columnname$", ::comphelper::getString(_xNewColumn->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)))
             ) );
        ::dbtools::throwGenericSQLException( sError, *this );
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
            pNewTable->renameImpl(m_Name);
            // release the temp file
        }
        xHold = pNewTable = NULL;

        FileClose();
        construct();
        if(m_pColumns)
            m_pColumns->refresh();
    }
    catch(const SQLException&)
    {
        // here we know that the old table wasn't droped before
        if(!bAlreadyDroped)
            xHold = pNewTable = NULL;

        throw;
    }
}
// -----------------------------------------------------------------------------
void ODbaseTable::dropColumn(sal_Int32 _nPos)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::dropColumn" );
    String sTempName = createTempFile();

    ODbaseTable* pNewTable = new ODbaseTable(m_pTables,static_cast<ODbaseConnection*>(m_pConnection));
    Reference< XPropertySet > xHold = pNewTable;
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
        xHold = pNewTable = NULL;
        const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                STR_COLUMN_NOT_DROP,
                "$position$", ::rtl::OUString::valueOf(_nPos)
             ) );
        ::dbtools::throwGenericSQLException( sError, *this );
    }
    pNewTable->construct();
    // copy the data
    copyData(pNewTable,_nPos);
    // drop the old table
    if(DropImpl())
        pNewTable->renameImpl(m_Name);
        // release the temp file

    xHold = pNewTable = NULL;

    FileClose();
    construct();
}
// -----------------------------------------------------------------------------
String ODbaseTable::createTempFile()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::createTempFile" );
    ::rtl::OUString aIdent = m_pConnection->getContent()->getIdentifier()->getContentIdentifier();
    if ( aIdent.lastIndexOf('/') != (aIdent.getLength()-1) )
        aIdent += ::rtl::OUString::createFromAscii("/");
    String sTempName(aIdent);
    String sExt;
    sExt.AssignAscii(".");
    sExt += m_pConnection->getExtension();

    String sName(m_Name);
    TempFile aTempFile(sName,&sExt,&sTempName);
    if(!aTempFile.IsValid())
        getConnection()->throwGenericSQLException(STR_COULD_NOT_ALTER_TABLE,*this);

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
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::copyData" );
    sal_Int32 nPos = _nPos + 1; // +1 because we always have the bookmark clumn as well
    OValueRefRow aRow = new OValueRefVector(m_pColumns->getCount());
    OValueRefRow aInsertRow;
    if(_nPos)
    {
        aInsertRow = new OValueRefVector(_pNewTable->m_pColumns->getCount());
        ::std::for_each(aInsertRow->get().begin(),aInsertRow->get().end(),TSetRefBound(sal_True));
    }
    else
        aInsertRow = aRow;

    // we only have to bind the values which we need to copy into the new table
    ::std::for_each(aRow->get().begin(),aRow->get().end(),TSetRefBound(sal_True));
    if(_nPos && (_nPos < (sal_Int32)aRow->get().size()))
        (aRow->get())[nPos]->setBound(sal_False);


    sal_Bool bOk = sal_True;
    sal_Int32 nCurPos;
    OValueRefVector::Vector::iterator aIter;
    for(sal_uInt32 nRowPos = 0; nRowPos < m_aHeader.db_anz;++nRowPos)
    {
        bOk = seekRow( IResultSetHelper::BOOKMARK, nRowPos+1, nCurPos );
        if ( bOk )
        {
            bOk = fetchRow( aRow, m_aColumns.getBody(), sal_True, sal_True);
            if ( bOk && !aRow->isDeleted() ) // copy only not deleted rows
            {
                // special handling when pos == 0 then we don't have to distinguish between the two rows
                if(_nPos)
                {
                    aIter = aRow->get().begin()+1;
                    sal_Int32 nCount = 1;
                    for(OValueRefVector::Vector::iterator aInsertIter = aInsertRow->get().begin()+1; aIter != aRow->get().end() && aInsertIter != aInsertRow->get().end();++aIter,++nCount)
                    {
                        if(nPos != nCount)
                        {
                            (*aInsertIter)->setValue( (*aIter)->getValue() );
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
    } // for(sal_uInt32 nRowPos = 0; nRowPos < m_aHeader.db_anz;++nRowPos)
}
// -----------------------------------------------------------------------------
void ODbaseTable::throwInvalidDbaseFormat()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::throwInvalidDbaseFormat" );
    FileClose();
    // no dbase file

    const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                STR_SQL_NAME_ERROR,
                "$filename$", getEntry(m_pConnection,m_Name)
             ) );
    ::dbtools::throwGenericSQLException( sError, *this );
}
// -----------------------------------------------------------------------------
void ODbaseTable::refreshHeader()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::refreshHeader" );
    if ( m_aHeader.db_anz == 0 )
        readHeader();
}
//------------------------------------------------------------------
sal_Bool ODbaseTable::seekRow(IResultSetHelper::Movement eCursorPosition, sal_Int32 nOffset, sal_Int32& nCurPos)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::seekRow" );
    // ----------------------------------------------------------
    // Positionierung vorbereiten:
    OSL_ENSURE(m_pFileStream,"ODbaseTable::seekRow: FileStream is NULL!");

    sal_uInt32  nNumberOfRecords = (sal_uInt32)m_aHeader.db_anz;
    sal_uInt32 nTempPos = m_nFilePos;
    m_nFilePos = nCurPos;

    switch(eCursorPosition)
    {
        case IResultSetHelper::NEXT:
            ++m_nFilePos;
            break;
        case IResultSetHelper::PRIOR:
            if (m_nFilePos > 0)
                --m_nFilePos;
            break;
        case IResultSetHelper::FIRST:
            m_nFilePos = 1;
            break;
        case IResultSetHelper::LAST:
            m_nFilePos = nNumberOfRecords;
            break;
        case IResultSetHelper::RELATIVE:
            m_nFilePos = (((sal_Int32)m_nFilePos) + nOffset < 0) ? 0L
                            : (sal_uInt32)(((sal_Int32)m_nFilePos) + nOffset);
            break;
        case IResultSetHelper::ABSOLUTE:
        case IResultSetHelper::BOOKMARK:
            m_nFilePos = (sal_uInt32)nOffset;
            break;
    }

    if (m_nFilePos > (sal_Int32)nNumberOfRecords)
        m_nFilePos = (sal_Int32)nNumberOfRecords + 1;

    if (m_nFilePos == 0 || m_nFilePos == (sal_Int32)nNumberOfRecords + 1)
        goto Error;
    else
    {
        sal_uInt16 nEntryLen = m_aHeader.db_slng;

        OSL_ENSURE(m_nFilePos >= 1,"SdbDBFCursor::FileFetchRow: ungueltige Record-Position");
        sal_Int32 nPos = m_aHeader.db_kopf + (sal_Int32)(m_nFilePos-1) * nEntryLen;

        ULONG nLen = m_pFileStream->Seek(nPos);
        if (m_pFileStream->GetError() != ERRCODE_NONE)
            goto Error;

        nLen = m_pFileStream->Read((char*)m_pBuffer, nEntryLen);
        if (m_pFileStream->GetError() != ERRCODE_NONE)
            goto Error;
    }
    goto End;

Error:
    switch(eCursorPosition)
    {
        case IResultSetHelper::PRIOR:
        case IResultSetHelper::FIRST:
            m_nFilePos = 0;
            break;
        case IResultSetHelper::LAST:
        case IResultSetHelper::NEXT:
        case IResultSetHelper::ABSOLUTE:
        case IResultSetHelper::RELATIVE:
            if (nOffset > 0)
                m_nFilePos = nNumberOfRecords + 1;
            else if (nOffset < 0)
                m_nFilePos = 0;
            break;
        case IResultSetHelper::BOOKMARK:
            m_nFilePos = nTempPos;   // vorherige Position
    }
    //  aStatus.Set(SDB_STAT_NO_DATA_FOUND);
    return sal_False;

End:
    nCurPos = m_nFilePos;
    return sal_True;
}
// -----------------------------------------------------------------------------
BOOL ODbaseTable::ReadMemo(ULONG nBlockNo, ORowSetValue& aVariable)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::ReadMemo" );
    BOOL bIsText = TRUE;
    //  SdbConnection* pConnection = GetConnection();

    m_pMemoStream->Seek(nBlockNo * m_aMemoHeader.db_size);
    switch (m_aMemoHeader.db_typ)
    {
        case MemodBaseIII: // dBase III-Memofeld, endet mit Ctrl-Z
        {
            const char cEOF = (char) DBF_EOL;
            ByteString aBStr;
            static char aBuf[514];
            aBuf[512] = 0;          // sonst kann der Zufall uebel mitspielen
            BOOL bReady = sal_False;

            do
            {
                m_pMemoStream->Read(&aBuf,512);

                USHORT i = 0;
                while (aBuf[i] != cEOF && ++i < 512)
                    ;
                bReady = aBuf[i] == cEOF;

                aBuf[i] = 0;
                aBStr += aBuf;

            } while (!bReady && !m_pMemoStream->IsEof() && aBStr.Len() < STRING_MAXLEN);

            ::rtl::OUString aStr(aBStr.GetBuffer(), aBStr.Len(),m_eEncoding);
            aVariable = aStr;

        } break;
        case MemoFoxPro:
        case MemodBaseIV: // dBase IV-Memofeld mit Laengenangabe
        {
            char sHeader[4];
            m_pMemoStream->Read(sHeader,4);
            // Foxpro stores text and binary data
            if (m_aMemoHeader.db_typ == MemoFoxPro)
            {
                if (((BYTE)sHeader[0]) != 0 || ((BYTE)sHeader[1]) != 0 || ((BYTE)sHeader[2]) != 0)
                {
//                  String aText = String(SdbResId(STR_STAT_IResultSetHelper::INVALID));
//                  aText.SearchAndReplace(String::CreateFromAscii("%%d"),m_pMemoStream->GetFileName());
//                  aText.SearchAndReplace(String::CreateFromAscii("%%t"),aStatus.TypeToString(MEMO));
//                  aStatus.Set(SDB_STAT_ERROR,
//                          String::CreateFromAscii("01000"),
//                          aStatus.CreateErrorMessage(aText),
//                          0, String() );
                    return sal_False;
                }

                bIsText = sHeader[3] != 0;
            }
            else if (((BYTE)sHeader[0]) != 0xFF || ((BYTE)sHeader[1]) != 0xFF || ((BYTE)sHeader[2]) != 0x08)
            {
//              String aText = String(SdbResId(STR_STAT_IResultSetHelper::INVALID));
//              aText.SearchAndReplace(String::CreateFromAscii("%%d"),m_pMemoStream->GetFileName());
//              aText.SearchAndReplace(String::CreateFromAscii("%%t"),aStatus.TypeToString(MEMO));
//              aStatus.Set(SDB_STAT_ERROR,
//                      String::CreateFromAscii("01000"),
//                      aStatus.CreateErrorMessage(aText),
//                      0, String() );
                return sal_False;
            }

            sal_uInt32 nLength(0);
            (*m_pMemoStream) >> nLength;

            if (m_aMemoHeader.db_typ == MemodBaseIV)
                nLength -= 8;

            //  char cChar;
            ::rtl::OUString aStr;
            while ( nLength > STRING_MAXLEN )
            {
                ByteString aBStr;
                aBStr.Expand(STRING_MAXLEN);
                m_pMemoStream->Read(aBStr.AllocBuffer(STRING_MAXLEN),STRING_MAXLEN);
                aStr += ::rtl::OUString(aBStr.GetBuffer(),aBStr.Len(), m_eEncoding);
                nLength -= STRING_MAXLEN;
            }
            if ( nLength > 0 )
            {
                ByteString aBStr;
                aBStr.Expand(static_cast<xub_StrLen>(nLength));
                m_pMemoStream->Read(aBStr.AllocBuffer(static_cast<xub_StrLen>(nLength)),nLength);
                //  aBStr.ReleaseBufferAccess();

                aStr += ::rtl::OUString(aBStr.GetBuffer(),aBStr.Len(), m_eEncoding);

            }
            if ( aStr.getLength() )
                aVariable = aStr;
        }
    }
    return sal_True;
}
// -----------------------------------------------------------------------------
void ODbaseTable::AllocBuffer()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::AllocBuffer" );
    UINT16 nSize = m_aHeader.db_slng;
    OSL_ENSURE(nSize > 0, "Size too small");

    if (m_nBufferSize != nSize)
    {
        delete m_pBuffer;
        m_pBuffer = NULL;
    }

    // Falls noch kein Puffer vorhanden: allozieren:
    if (m_pBuffer == NULL && nSize)
    {
        m_nBufferSize = nSize;
        m_pBuffer       = new BYTE[m_nBufferSize+1];
    }
}
// -----------------------------------------------------------------------------
BOOL ODbaseTable::WriteBuffer()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::WriteBuffer" );
    OSL_ENSURE(m_nFilePos >= 1,"SdbDBFCursor::FileFetchRow: ungueltige Record-Position");

    // Auf gewuenschten Record positionieren:
    long nPos = m_aHeader.db_kopf + (long)(m_nFilePos-1) * m_aHeader.db_slng;
    m_pFileStream->Seek(nPos);
    return m_pFileStream->Write((char*) m_pBuffer, m_aHeader.db_slng) > 0;
}
// -----------------------------------------------------------------------------
sal_Int32 ODbaseTable::getCurrentLastPos() const
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::getCurrentLastPos" );
    return m_aHeader.db_anz;
}
