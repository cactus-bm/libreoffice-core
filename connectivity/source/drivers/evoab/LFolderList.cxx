/*************************************************************************
 *
 *  $RCSfile: LFolderList.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-16 17:25:12 $
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

#include <ctype.h>
#include <algorithm>

#ifndef _CONNECTIVITY_EVOAB_LEVOABFOLDERLIST_HXX_
#include "LFolderList.hxx"
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
#ifndef _CONNECTIVITY_EVOAB_LCONNECTION_HXX_
#include "LConnection.hxx"
#endif
#ifndef _CONNECTIVITY_EVOAB_LCOLUMNS_HXX_
#include "LColumns.hxx"
#endif
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif
#ifndef _CONFIG_HXX
#include <tools/config.hxx>
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
#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#include <stdio.h>      //sprintf
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _COMPHELPER_NUMBERS_HXX_
#include <comphelper/numbers.hxx>
#endif
#ifndef _CONNECTIVITY_EVOAB_LDRIVER_HXX_
#include "LDriver.hxx"
#endif
#ifndef _COM_SUN_STAR_UTIL_NUMBERFORMAT_HPP_
#include <com/sun/star/util/NumberFormat.hpp>
#endif
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _ISOLANG_HXX
#include <tools/isolang.hxx>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include "connectivity/dbconversion.hxx"
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _UNTOOLS_UCBSTREAMHELPER_HXX
#include <unotools/ucbstreamhelper.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include "connectivity/dbexception.hxx"
#endif
#ifndef CONNECTIVITY_EVOAB_DEBUG_HELPER_HXX
#include "LDebug.hxx"
#endif

using namespace ::comphelper;
using namespace connectivity;
using namespace connectivity::evoab;
using namespace connectivity::file;
using namespace ucb;
using namespace ::cppu;
using namespace utl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

// -------------------------------------------------------------------------
void OEvoabFolderList::fillColumns()
{
    BOOL bRead = TRUE;

    OEvoabString aHeaderLine;
    OEvoabConnection* pConnection = (OEvoabConnection*)m_pConnection;

    // read first row
    OEvoabString aFirstLine;

    bRead = m_pFileStream->ReadByteStringLine(aFirstLine,pConnection->getTextEncoding());

    while(bRead && !aFirstLine.Len())
    {
        bRead = m_pFileStream->ReadByteStringLine(aFirstLine,pConnection->getTextEncoding());
    }
    // use first row as headerline because we need the number of columns
    aHeaderLine = aFirstLine;

    // column count
    xub_StrLen nFieldCount = aHeaderLine.GetTokenCount(pConnection->getFieldDelimiter(),pConnection->getStringDelimiter());

    if(!m_aColumns.isValid())
        m_aColumns = new OSQLColumns();
    else
        m_aColumns->clear();

    m_aTypes.clear();
    m_aPrecisions.clear();
    m_aScales.clear();
    // reserve some space
    m_aColumns->reserve(nFieldCount);
    m_aTypes.reserve(nFieldCount);
    m_aPrecisions.reserve(nFieldCount);
    m_aScales.reserve(nFieldCount);

    sal_Bool bCase = getConnection()->getMetaData()->storesMixedCaseQuotedIdentifiers();
    // read description
    sal_Unicode cDecimalDelimiter  = pConnection->getDecimalDelimiter();
    sal_Unicode cThousandDelimiter = pConnection->getThousandDelimiter();
    String aColumnName;
    ::rtl::OUString aTypeName;
    ::comphelper::UStringMixEqual aCase(bCase);
    xub_StrLen nStartPosHeaderLine = 0; // use for eficient way to get the tokens
    xub_StrLen nStartPosFirstLine = 0; // use for eficient way to get the tokens
    xub_StrLen nStartPosFirstLine2 = 0;
    for (xub_StrLen i = 0; i < nFieldCount; i++)
    {

        // no column name so ...
        aColumnName = 'C';
        aColumnName += String::CreateFromInt32(i+1);

        sal_Int32 eType;
        UINT16 nPrecision = 0;
        UINT16 nScale = 0;

        BOOL bNumeric = FALSE;
        double nRes=0.0;
        ULONG  nIndex = 0;

        // first without fielddelimiter
        String aField;
        aFirstLine.GetTokenSpecial(aField,nStartPosFirstLine,pConnection->getFieldDelimiter(),'\0');
        if (aField.Len() == 0 ||
            (pConnection->getStringDelimiter() && pConnection->getStringDelimiter() == aField.GetChar(0)))
        {
            bNumeric = FALSE;
        }
        else
        {
            String aField2;
            if ( pConnection->getStringDelimiter() != '\0' )
                aFirstLine.GetTokenSpecial(aField2,nStartPosFirstLine2,pConnection->getFieldDelimiter(),pConnection->getStringDelimiter());
            else
                aField2 = aField;

            if (aField2.Len() == 0)
            {
                bNumeric = FALSE;
            }
            else
            {
                bNumeric = TRUE;
                xub_StrLen nDot = 0;
                for (xub_StrLen j = 0; j < aField2.Len(); j++)
                {
                    sal_Unicode c = aField2.GetChar(j);
                    // nur Ziffern und Dezimalpunkt und Tausender-Trennzeichen?
                    if ((!cDecimalDelimiter || c != cDecimalDelimiter) &&
                        (!cThousandDelimiter || c != cThousandDelimiter) &&
                        !isdigit(c))
                    {
                        bNumeric = FALSE;
                        break;
                    }
                    if (cDecimalDelimiter && c == cDecimalDelimiter)
                    {
                        nPrecision = 15; // we have an decimal value
                        nScale = 2;
                        nDot++;
                    }
                }

                if (nDot > 1) // if there is more than one dot it isn't a number
                    bNumeric = FALSE;
                if (bNumeric && cThousandDelimiter)
                {
                    // Ist der Trenner richtig angegeben?
                    String aValue = aField2.GetToken(0,cDecimalDelimiter);
                    for (sal_Int32 j = aValue.Len() - 4; j >= 0; j -= 4)
                    {
                        sal_Unicode c = aValue.GetChar(j);
                        // nur Ziffern und Dezimalpunkt und Tausender-Trennzeichen?
                        if (c == cThousandDelimiter && j)
                            continue;
                        else
                        {
                            bNumeric = FALSE;
                            break;
                        }
                    }
                }

                // jetzt koennte es noch ein Datumsfeld sein
                if (!bNumeric)
                {
                    try
                    {
                        nIndex = m_xNumberFormatter->detectNumberFormat(::com::sun::star::util::NumberFormat::ALL,aField2);
                    }
                    catch(Exception&)
                    {
                    }
                }
            }
        }

        sal_Int32 nFlags = 0;
        if (bNumeric)
        {
            if (cDecimalDelimiter)
            {
                if(nPrecision)
                {
                    eType = DataType::DECIMAL;
                    aTypeName = ::rtl::OUString::createFromAscii("DECIMAL");
                }
                else
                {
                    eType = DataType::DOUBLE;
                    aTypeName = ::rtl::OUString::createFromAscii("DOUBLE");
                }
            }
            else
                eType = DataType::INTEGER;
            nFlags = ColumnSearch::BASIC;
        }
        else
        {

            switch (comphelper::getNumberFormatType(m_xNumberFormatter,nIndex))
            {
                case NUMBERFORMAT_DATE:
                    eType = DataType::DATE;
                    aTypeName = ::rtl::OUString::createFromAscii("DATE");
                    break;
                case NUMBERFORMAT_DATETIME:
                    eType = DataType::TIMESTAMP;
                    aTypeName = ::rtl::OUString::createFromAscii("TIMESTAMP");
                    break;
                case NUMBERFORMAT_TIME:
                    eType = DataType::TIME;
                    aTypeName = ::rtl::OUString::createFromAscii("TIME");
                    break;
                default:
                    eType = DataType::VARCHAR;
                    nPrecision = 0; // nyi: Daten koennen aber laenger sein!
                    nScale = 0;
                    aTypeName = ::rtl::OUString::createFromAscii("VARCHAR");
            };
            nFlags |= ColumnSearch::CHAR;
        }

        // check if the columname already exists
        String aAlias(aColumnName);
        OSQLColumns::const_iterator aFind = connectivity::find(m_aColumns->begin(),m_aColumns->end(),aAlias,aCase);
        sal_Int32 nExprCnt = 0;
        while(aFind != m_aColumns->end())
        {
            (aAlias = aColumnName) += String::CreateFromInt32(++nExprCnt);
            aFind = connectivity::find(m_aColumns->begin(),m_aColumns->end(),aAlias,aCase);
        }

        sdbcx::OColumn* pColumn = new sdbcx::OColumn(aAlias,aTypeName,::rtl::OUString(),
                                                ColumnValue::NULLABLE,
                                                nPrecision,
                                                nScale,
                                                eType,
                                                sal_False,
                                                sal_False,
                                                sal_False,
                                                bCase);
        Reference< XPropertySet> xCol = pColumn;
        m_aColumns->push_back(xCol);
        m_aTypes.push_back(eType);
        m_aPrecisions.push_back(nPrecision);
        m_aScales.push_back(nScale);
    }
    m_pFileStream->Seek(STREAM_SEEK_TO_BEGIN);
}
// -------------------------------------------------------------------------
OEvoabFolderList::OEvoabFolderList(OEvoabConnection* _pConnection)
{

}
// -------------------------------------------------------------------------
DBG_NAME( OEvoabFolderList );
OEvoabFolderList::OEvoabFolderList( OEvoabConnection* _pConnection,
                    const ::rtl::OUString& _Name,
                    const ::rtl::OUString& _Type
                )
    :m_pConnection(_pConnection)
    ,m_nFilePos(0)
    ,m_pFileStream(NULL)
    ,m_bIsNull(sal_False)
{
    DBG_CTOR( OEvoabFolderList, NULL );
    m_aColumns = new OSQLColumns();

    EVO_TRACE_STRING("OEvoabFolderList::constructor::_Name = %s\n", _Name );
    EVO_TRACE_STRING("OEvoabFolderList::constructor::_Type = %s\n", _Type );
    construct();
}
// -----------------------------------------------------------------------------
void OEvoabFolderList::construct()
{
    Any aValue = ConfigManager::GetDirectConfigProperty(ConfigManager::LOCALE);
    LanguageType eLanguage = ConvertIsoStringToLanguage(comphelper::getString(aValue),'-');

    String sLanguage, sCountry;
    ConvertLanguageToIsoNames(eLanguage, sLanguage, sCountry);
    ::com::sun::star::lang::Locale aAppLocale(sLanguage,sCountry,rtl::OUString());
    Sequence< ::com::sun::star::uno::Any > aArg(1);
    aArg[0] <<= aAppLocale;

    Reference< ::com::sun::star::util::XNumberFormatsSupplier >  xSupplier(m_pConnection->getDriver()->getFactory()->createInstanceWithArguments(::rtl::OUString::createFromAscii("com.sun.star.util.NumberFormatsSupplier"),aArg),UNO_QUERY);
    m_xNumberFormatter = Reference< ::com::sun::star::util::XNumberFormatter >(m_pConnection->getDriver()->getFactory()->createInstance(::rtl::OUString::createFromAscii("com.sun.star.util.NumberFormatter")),UNO_QUERY);
    m_xNumberFormatter->attachNumberFormatsSupplier(xSupplier);

    INetURLObject aURL;
    aURL.SetURL(m_pConnection->getDriver()->getEvoFolderListFileURL());

    String aFileName = aURL.GetMainURL(INetURLObject::NO_DECODE);

    EVO_TRACE_STRING("OJ::construct()::aFileName = %s\n", aFileName );
    m_pFileStream = createStream_simpleError( aFileName,STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYWRITE);

    if(!m_pFileStream)
        m_pFileStream = createStream_simpleError( aFileName,STREAM_READ | STREAM_NOCREATE | STREAM_SHARE_DENYNONE);

    if(m_pFileStream)
    {
        m_pFileStream->Seek(STREAM_SEEK_TO_END);
        sal_Int32 nSize = m_pFileStream->Tell();
        m_pFileStream->Seek(STREAM_SEEK_TO_BEGIN);

        // Buffersize abhaengig von der Filegroesse
        m_pFileStream->SetBufferSize(nSize > 1000000 ? 32768 :
                                    nSize > 100000  ? 16384 :
                                    nSize > 10000   ? 4096  : 1024);
        OSL_TRACE("OEvoabFolderList::construct()::m_pFileStream->Tell() = %d\n", nSize );

        fillColumns();
    }
}


//------------------------------------------------------------------
sal_Bool OEvoabFolderList::fetchRow(OValueRow _rRow,const OSQLColumns & _rCols)
{
    //OSL_TRACE("OEvoabFolderList::fetchRow()::entered in" );

    (*_rRow)[0] = m_nFilePos;   // the "bookmark"
    //OSL_TRACE("OEvoabFolderList::fetchRow()::after *_rRow" );

    OEvoabConnection* pConnection = (OEvoabConnection*)m_pConnection;
    sal_Int32 nByteOffset = 1;
    // Felder:
    xub_StrLen nStartPos = 0;
    String aStr;
    OSQLColumns::const_iterator aIter = _rCols.begin();
    for (sal_Int32 i = 0; aIter != _rCols.end();++aIter, ++i)
    {
        m_aCurrentLine.GetTokenSpecial(aStr,nStartPos,pConnection->getFieldDelimiter(),pConnection->getStringDelimiter());
        //OSL_TRACE("OEvoabFolderList::fetchRow()::aStr = %s\n", ((OUtoCStr(::rtl::OUString(aStr))) ? (OUtoCStr(::rtl::OUString(aStr))):("NULL")) );

        if (aStr.Len() == 0)
            (*_rRow)[i+1].setNull();
        else
        {
            // length depending on the data type
            sal_Int32   nLen    = m_aPrecisions[i];
            sal_Int32   nType   = m_aTypes[i];
            switch(nType)
            {
                case DataType::TIMESTAMP:
                case DataType::DATE:
                case DataType::TIME:
                {
                    double nRes = 0.0;
                    try
                    {
                        nRes = m_xNumberFormatter->convertStringToNumber(::com::sun::star::util::NumberFormat::ALL,aStr);
                        Reference<XPropertySet> xProp(m_xNumberFormatter->getNumberFormatsSupplier()->getNumberFormatSettings(),UNO_QUERY);
                        com::sun::star::util::Date aDate;
                        xProp->getPropertyValue(::rtl::OUString::createFromAscii("NullDate")) >>= aDate;

                        switch(nType)
                        {
                            case DataType::DATE:
                                (*_rRow)[i+1] = ::dbtools::DBTypeConversion::toDouble(::dbtools::DBTypeConversion::toDate(nRes,aDate));
                                break;
                            case DataType::TIMESTAMP:
                                (*_rRow)[i+1] = ::dbtools::DBTypeConversion::toDouble(::dbtools::DBTypeConversion::toDateTime(nRes,aDate));
                                break;
                            default:
                                (*_rRow)[i+1] = ::dbtools::DBTypeConversion::toDouble(::dbtools::DBTypeConversion::toTime(nRes));
                        }
                    }
                    catch(Exception&)
                    {
                        (*_rRow)[i+1].setNull();
                    }
                }   break;
                case DataType::DOUBLE:
                case DataType::INTEGER:
                case DataType::DECIMAL:             // #99178# OJ
                case DataType::NUMERIC:
                {
                    sal_Unicode cDecimalDelimiter = pConnection->getDecimalDelimiter();
                    sal_Unicode cThousandDelimiter = pConnection->getThousandDelimiter();
                    String aStrConverted;

                    OSL_ENSURE(cDecimalDelimiter && nType != DataType::INTEGER ||
                               !cDecimalDelimiter && nType == DataType::INTEGER,
                               "FalscherTyp");

                    // In Standard-Notation (DezimalPUNKT ohne Tausender-Komma) umwandeln:
                    for (xub_StrLen j = 0; j < aStr.Len(); ++j)
                    {
                        if (cDecimalDelimiter && aStr.GetChar(j) == cDecimalDelimiter)
                            aStrConverted += '.';
                        else if ( aStr.GetChar(j) == '.' ) // special case, if decimal seperator isn't '.' we have to vut the string after it
                            break; // #99189# OJ
                        else if (cThousandDelimiter && aStr.GetChar(j) == cThousandDelimiter)
                        {
                            // weglassen
                        }
                        else
                            aStrConverted += aStr.GetChar(j) ;
                    }
                    double nVal = ::rtl::math::stringToDouble(aStrConverted.GetBuffer(),',','.',NULL,NULL);

                    // #99178# OJ
                    if ( DataType::DECIMAL == nType || DataType::NUMERIC == nType )
                        (*_rRow)[i+1] = String::CreateFromDouble(nVal);
                    else
                        (*_rRow)[i+1] = nVal;
                } break;

                default:
                {
                    // Wert als String in Variable der Row uebernehmen
                    (*_rRow)[i+1] = aStr;
                }
                break;
            }
        }
    }
    return sal_True;
}


//------------------------------------------------------------------
sal_Bool OEvoabFolderList::seekRow(IResultSetHelper::Movement eCursorPosition)
{
    //OSL_TRACE("OEvoabFolderList::seekRow()::(before SeekRow)m_aCurrentLine = %d\n", ((OUtoCStr(::rtl::OUString(m_aCurrentLine))) ? (OUtoCStr(::rtl::OUString(m_aCurrentLine))):("NULL")) );

    if ( !m_pFileStream )
        return sal_False;
    OEvoabConnection* pConnection = (OEvoabConnection*)m_pConnection;
    // ----------------------------------------------------------
    // Positionierung vorbereiten:
    //OSL_TRACE("OEvoabFolderList::seekRow()::(before SeekRow,m_pFileStriam Exist)m_aCurrentLine = %d\n", ((OUtoCStr(::rtl::OUString(m_aCurrentLine))) ? (OUtoCStr(::rtl::OUString(m_aCurrentLine))):("NULL")) );

    switch(eCursorPosition)
    {
        case IResultSetHelper::FIRST:
            m_nFilePos = 0;
            // run through

        case IResultSetHelper::NEXT:
            m_pFileStream->Seek(m_nFilePos);

            if (m_pFileStream->IsEof())
            {
                OSL_TRACE( "OEvoabFolderList::seekRow: EOF /before/ reading the line." );
                return sal_False;
            }

            m_pFileStream->ReadByteStringLine(m_aCurrentLine,pConnection->getTextEncoding());
            if (m_pFileStream->IsEof())
            {
                OSL_TRACE( "OEvoabFolderList::seekRow: EOF /after/ reading the line." );
                if ( !m_aCurrentLine.Len() )
                {
                    OSL_TRACE( "OEvoabFolderList::seekRow: empty line read." );
                    return sal_False;
                }
            }
            m_nFilePos = m_pFileStream->Tell();
            break;

        default:
            OSL_ENSURE( sal_False, "OEvoabFolderList::seekRow: unsupported positioning!" );
            break;
    }

    //OSL_TRACE("OEvoabFolderList::seekRow()::(after SeekRow)m_aCurrentLine = %d\n", ((OUtoCStr(::rtl::OUString(m_aCurrentLine))) ? (OUtoCStr(::rtl::OUString(m_aCurrentLine))):("NULL")) );

    return sal_True;
}
// -----------------------------------------------------------------------------
SvStream* OEvoabFolderList::createStream_simpleError( const String& _rFileName, StreamMode _eOpenMode)
{
    utl::UcbLockBytesHandler* p_null_dummy=NULL;
    SvStream* pReturn = ::utl::UcbStreamHelper::CreateStream( _rFileName, _eOpenMode, p_null_dummy);
    if (pReturn && (ERRCODE_NONE != pReturn->GetErrorCode()))
    {
        delete pReturn;
        pReturn = NULL;
    }
    return pReturn;
}
// -----------------------------------------------------------------------------
const ORowSetValue& OEvoabFolderList::getValue(sal_Int32 _nColumnIndex ) throw(::com::sun::star::sdbc::SQLException)
{
    checkIndex( _nColumnIndex );

    m_bIsNull = (*m_aRow)[_nColumnIndex].isNull();
    return (*m_aRow)[_nColumnIndex];
}
// -----------------------------------------------------------------------------
void OEvoabFolderList::checkIndex(sal_Int32 _nColumnIndex ) throw(::com::sun::star::sdbc::SQLException)
{
    if (   _nColumnIndex <= 0 || _nColumnIndex >= (sal_Int32)m_aRow->size() )
//        ::dbtools::throwInvalidIndexException();
            ;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabFolderList::getString( sal_Int32 _nColumnIndex ) throw(SQLException, RuntimeException)
{
    return getValue(_nColumnIndex);
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OEvoabFolderList::getInt( sal_Int32 _nColumnIndex ) throw(SQLException, RuntimeException)
{
    return getValue( _nColumnIndex );
}
// -----------------------------------------------------------------------------
void OEvoabFolderList::initializeRow(sal_Int32 _nColumnCount)
{
    if(!m_aRow.isValid())
    {
        m_aRow  = new OValueVector(_nColumnCount);
        (*m_aRow)[0].setBound(sal_True);
        ::std::for_each(m_aRow->begin()+1,m_aRow->end(),TSetBound(sal_False));
    }
    //OSL_TRACE("OEvoabFolderList::initializeRow()::_nColumnCount = %d\n", _nColumnCount);

}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OEvoabFolderList::first(  ) throw(SQLException, RuntimeException)
{
    sal_Bool bSuccess = seekRow(IResultSetHelper::FIRST);

    EVO_TRACE_STRING("OEvoabFolderList::first(): returning %s\n", ::rtl::OUString::valueOf(bSuccess) );
    return bSuccess;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OEvoabFolderList::next(  ) throw(SQLException, RuntimeException)
{
    sal_Bool bSuccess = seekRow(IResultSetHelper::NEXT);

    EVO_TRACE_STRING("OEvoabFolderList::next(): returning %s\n", ::rtl::OUString::valueOf(bSuccess) );
    return bSuccess;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL OEvoabFolderList::getRow(  ) throw(SQLException, RuntimeException)
{
    sal_Bool bRet = fetchRow(m_aRow,getTableColumns().getBody());
    EVO_TRACE_STRING("OEvoabFolderList::getRow()::fetchRow() = %s\n", ::rtl::OUString::valueOf(bRet) );

    return bRet;
}
