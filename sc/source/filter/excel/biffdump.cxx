/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: biffdump.cxx,v $
 *
 *  $Revision: 1.88 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 19:44:04 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"

#ifndef SC_BIFFDUMP_HXX
#include "biffdump.hxx"
#endif

#if EXC_INCL_DUMPER

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>
#endif
#ifndef _SOT_STORINFO_HXX
#include <sot/storinfo.hxx>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtl/math.hxx>

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_FPROGRESSBAR_HXX
#include "fprogressbar.hxx"
#endif
#ifndef SC_XLPIVOT_HXX
#include "xlpivot.hxx"
#endif
#ifndef SC_XICONTENT_HXX
#include "xicontent.hxx"
#endif

#ifndef _IMP_OP_HXX
#include "imp_op.hxx"
#endif

#define GETSTR(s)       ByteString( s, RTL_TEXTENCODING_MS_1252 )

static const sal_Char*  __pHexPrefix = "0x";
static const sal_Char*  __pBinPrefix = "0b";
static const sal_Char*  pU = "UNKNOWN ";

const sal_Char*         Biff8RecDumper::pLevelPreString = "                                            ";
const sal_Char*         Biff8RecDumper::pLevelPreStringNT = pLevelPreString + strlen( pLevelPreString );
UINT32                  Biff8RecDumper::nInstances = 0;
sal_Char*               Biff8RecDumper::pBlankLine = NULL;
const UINT16            Biff8RecDumper::nLenBlankLine = 255;
const UINT16            Biff8RecDumper::nRecCnt = 0x2020;
UINT8*                  Biff8RecDumper::pCharType = NULL;
UINT8*                  Biff8RecDumper::pCharVal = NULL;

static const UINT16     nLevelInc = 1;

static UINT16           nXFCount = 0;

static UINT16           nSXLISize[2] = {0, 0};      // array size for SXLI records [rows/cols]
static UINT16           nSXLIIndex = 0;             // current index for SXLI records

// ============================================================================

namespace {

// decimal --------------------------------------------------------------------

inline void lclAppendDec( ByteString& rStr, sal_uInt8 nData )
{
    rStr.Append( ByteString::CreateFromInt32( nData ) );
}

inline void lclAppendDec( ByteString& rStr, sal_Int8 nData )
{
    rStr.Append( ByteString::CreateFromInt32( nData ) );
}

inline void lclAppendDec( ByteString& rStr, sal_uInt16 nData )
{
    rStr.Append( ByteString::CreateFromInt32( nData ) );
}

inline void lclAppendDec( ByteString& rStr, sal_Int16 nData )
{
    rStr.Append( ByteString::CreateFromInt32( nData ) );
}

inline void lclAppendDec( ByteString& rStr, sal_uInt32 nData )
{
    rStr.Append( ByteString::CreateFromInt64( nData ) );
}

inline void lclAppendDec( ByteString& rStr, sal_Int32 nData )
{
    rStr.Append( ByteString::CreateFromInt32( nData ) );
}

inline void lclAppendDec( ByteString& rStr, double fData )
{
    rStr.Append( ByteString( ::rtl::math::doubleToString( fData, rtl_math_StringFormat_G, 15, '.', true ) ) );
}

// hexadecimal ----------------------------------------------------------------

void lclAppendHex( ByteString& rStr, sal_uInt8 nData, bool bPrefix = true )
{
    static const sal_Char spcHexDigits[] = "0123456789ABCDEF";
    static const ByteString saPrefix( "0x" );

    if( bPrefix )
        rStr.Append( saPrefix );
    rStr.Append( spcHexDigits[ (nData >> 4) & 0x0F ] ).Append( spcHexDigits[ nData & 0x0F ] );
}

inline void lclAppendHex( ByteString& rStr, sal_Int8 nData, bool bPrefix = true )
{
    lclAppendHex( rStr, static_cast< sal_uInt8 >( nData ), bPrefix );
}

void lclAppendHex( ByteString& rStr, sal_uInt16 nData, bool bPrefix = true )
{
    lclAppendHex( rStr, static_cast< sal_uInt8 >( nData >> 8 ), bPrefix );
    lclAppendHex( rStr, static_cast< sal_uInt8 >( nData ), false );
}

inline void lclAppendHex( ByteString& rStr, sal_Int16 nData, bool bPrefix = true )
{
    lclAppendHex( rStr, static_cast< sal_uInt16 >( nData ), bPrefix );
}

void lclAppendHex( ByteString& rStr, sal_uInt32 nData, bool bPrefix = true )
{
    lclAppendHex( rStr, static_cast< sal_uInt16 >( nData >> 16 ), bPrefix );
    lclAppendHex( rStr, static_cast< sal_uInt16 >( nData ), false );
}

inline void lclAppendHex( ByteString& rStr, sal_Int32 nData, bool bPrefix = true )
{
    lclAppendHex( rStr, static_cast< sal_uInt32 >( nData ), bPrefix );
}

inline void lclAppendHex( ByteString& rStr, double fData, bool bPrefix = true )
{
    const sal_uInt32* pnData = reinterpret_cast< const sal_uInt32* >( &fData );
    lclAppendHex( rStr, pnData[ 0 ], bPrefix );
    lclAppendHex( rStr, pnData[ 1 ], false );
}

// others ---------------------------------------------------------------------

void lclAppendGuid( ByteString& rStr, const XclGuid& rGuid )
{
    lclAppendHex( rStr, SVBT32ToUInt32( rGuid.mpnData ), false );
    rStr.Append( '-' );
    lclAppendHex( rStr, SVBT16ToShort( rGuid.mpnData + 4 ), false );
    rStr.Append( '-' );
    lclAppendHex( rStr, SVBT16ToShort( rGuid.mpnData + 6 ), false );
    rStr.Append( '-' );
    lclAppendHex( rStr, rGuid.mpnData[ 8 ], false );
    lclAppendHex( rStr, rGuid.mpnData[ 9 ], false );
    rStr.Append( '-' );
    lclAppendHex( rStr, rGuid.mpnData[ 10 ], false );
    lclAppendHex( rStr, rGuid.mpnData[ 11 ], false );
    lclAppendHex( rStr, rGuid.mpnData[ 12 ], false );
    lclAppendHex( rStr, rGuid.mpnData[ 13 ], false );
    lclAppendHex( rStr, rGuid.mpnData[ 14 ], false );
    lclAppendHex( rStr, rGuid.mpnData[ 15 ], false );
}

} // namespace

// ============================================================================

static void __AddHexNibble( ByteString& r, UINT8 nVal )
{
    const sal_Char  pH[] = "0123456789ABCDEF";

    nVal &= 0x0F;

    r += pH[ nVal ];
}


static void __AddPureHex( ByteString& r, UINT8 nVal )
{
    __AddHexNibble( r, nVal >> 4 );
    __AddHexNibble( r, nVal );
}


static void __AddHex( ByteString& r, UINT8 nVal )
{
    r += __pHexPrefix;
    __AddHexNibble( r, nVal >> 4 );
    __AddHexNibble( r, nVal );
}


static void __AddPureHex( ByteString& r, UINT16 nVal )
{
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 12 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 8 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 4 ) );
    __AddHexNibble( r, ( UINT8 ) nVal );
}


static void __AddHex( ByteString& r, UINT16 nVal )
{
    r += __pHexPrefix;
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 12 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 8 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 4 ) );
    __AddHexNibble( r, ( UINT8 ) nVal );
}


static void __AddPureHex( ByteString& r, UINT32 nVal )
{
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 28 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 24 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 20 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 16 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 12 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 8 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 4 ) );
    __AddHexNibble( r, ( UINT8 ) nVal );
}


static void __AddHex( ByteString& r, UINT32 nVal )
{
    r += __pHexPrefix;
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 28 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 24 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 20 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 16 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 12 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 8 ) );
    __AddHexNibble( r, ( UINT8 ) ( nVal >> 4 ) );
    __AddHexNibble( r, ( UINT8 ) nVal );
}


static void __AddHex( ByteString& r, INT32 nVal )
{
    __AddHex( r, (UINT32) nVal );
}


static void __AddPureBinNibble( ByteString& r, UINT8 nVal )
{
    nVal <<= 4;
    for( int n = 4 ; n ; n-- )
    {
        r += ( nVal & 0x80 )? "1" : "0";
        nVal <<= 1;
    }
}


static void __AddPureBin( ByteString& r, UINT8 nVal )
{
    __AddPureBinNibble( r, nVal >> 4 );
    r += " ";
    __AddPureBinNibble( r, nVal );
}


static void __AddPureBin( ByteString& r, UINT16 nVal )
{
    const sal_Char*     pIn = "  ";
    __AddPureBin( r, ( UINT8 ) ( nVal >> 8 ) );
    r += pIn;
    __AddPureBin( r, ( UINT8 ) nVal );
}


static void __AddPureBin( ByteString& r, UINT32 nVal )
{
    const sal_Char*     pIn = "  ";
    __AddPureBin( r, ( UINT8 ) ( nVal >> 24 ) );
    r += pIn;
    __AddPureBin( r, ( UINT8 ) ( nVal >> 16 ) );
    r += pIn;
    __AddPureBin( r, ( UINT8 ) ( nVal >> 8 ) );
    r += pIn;
    __AddPureBin( r, ( UINT8 ) nVal );
}


inline static void __AddDec( ByteString& r, UINT32 n )
{
    sal_Char    p[ 32 ];
    sprintf( p, "%u", n );  // #100211# - checked
    r += p;
}


inline static void __AddDec( ByteString& r, UINT16 n )
{
    __AddDec( r, ( UINT32 ) n );
}


inline static void __AddDec( ByteString& r, UINT8 n )
{
    __AddDec( r, ( UINT32 ) n );
}


inline static void __AddDec( ByteString& r, INT32 n )
{
    sal_Char    p[ 32 ];
    sprintf( p, "%d", n );  // #100211# - checked
    r += p;
}


inline static void __AddDec( ByteString& r, INT16 n )
{
    __AddDec( r, ( INT32 ) n );
}

inline static void __AddDec( ByteString& r, sal_Int8 n )
{
    __AddDec( r, ( INT32 ) n );
}


static void __AddDec( ByteString& r, UINT32 nVal, UINT16 nNumOfDig, sal_Char c = ' ' )
{
    ByteString  t;
    ByteString  aVal;
    __AddDec( aVal, nVal );
    if( nNumOfDig > (UINT16) aVal.Len() )
        t.Fill( nNumOfDig - (UINT16) aVal.Len(), c );
    r += t;
    r += aVal;
}


inline static void __AddDec1616( ByteString& r, UINT32 n )
{
    __AddDec( r, (UINT16)(n >> 16) );
    r += '.';
    __AddDec( r, (UINT16)(n & 0xFFFF) );
}


static void __AddDouble( ByteString& r, const double f )
{
    r += ByteString( ::rtl::math::doubleToString( f, rtl_math_StringFormat_G, 15, '.', TRUE ) );
}


static inline void __AddRK( ByteString& rString, sal_Int32 nRKValue )
{
    __AddDouble( rString, XclTools::GetDoubleFromRK( nRKValue ) );
}


inline static void __Add16p16( ByteString& r, UINT32 n )
{
    __AddDouble( r, double(n) / 65536.0 );
}


static void lcl_AddRef( ByteString& rStr, sal_uInt16 nCol, sal_uInt16 nRow )
{
    ScAddress aRef( static_cast< SCCOL >( nCol ), static_cast< SCROW >( nRow ), 0 );
    rStr.Append( GETSTR( aRef.GetColRowString() ) );
}


static void lcl_AddRangeRef( ByteString& rStr, sal_uInt16 nCol1, sal_uInt16 nRow1, sal_uInt16 nCol2, sal_uInt16 nRow2 )
{
    lcl_AddRef( rStr, nCol1, nRow1 );
    if( (nCol1 != nCol2) || (nRow1 != nRow2) )
    {
        rStr.Append( ':' );
        lcl_AddRef( rStr, nCol2, nRow2 );
    }
}


static void __AddCellHead( ByteString& r, const UINT16 nC, const UINT16 nR, const UINT16 nXF )
{
    lcl_AddRef( r, (UINT8) nC, nR );
    r += " (XF=";
    __AddDec( r, nXF );
    r += ')';
}


inline static void lcl_AddFlag(
        ByteString& rString, bool bFlag, const sal_Char* pcTrue = "true", const sal_Char* pcFalse = "false" )
{
    rString += (bFlag ? pcTrue : pcFalse);
}

inline static void lcl_AddOnOff( ByteString& rString, bool bFlag )
{
    lcl_AddFlag( rString, bFlag, "on", "off" );
}


static void lcl_AddEnum(
        ByteString& rString, long nValue, const sal_Char* const ppcEnums[], long nSize,
        const sal_Char* pcDefault = 0, long nOffset = 0 )
{
    nValue -= nOffset;
    const sal_Char* pcText = "!unknown!";
    if( (0 <= nValue) && (nValue < nSize) && ppcEnums[ nValue ] )
        pcText = ppcEnums[ nValue ];
    else if( pcDefault )
        pcText = pcDefault;
    if( *pcText )   // nonempty string
        rString.Append( " (" ).Append( pcText ).Append( ')' );
}


namespace {

void lclDumpString( SvStream& rOutStrm, const ByteString& rData )
{
    ByteString aOutStr;
    xub_StrLen nIdx = 0;
    for( ; (nIdx < rData.Len()) && (aOutStr.Len() < 80); ++nIdx )
    {
        sal_Char cChar = rData.GetChar( nIdx );
        if( 32 <= cChar )
            aOutStr.Append( cChar );
        else
        {
            aOutStr.Append( '<' );
            __AddHex( aOutStr, static_cast< sal_uInt8 >( cChar ) );
            aOutStr.Append( '>' );
        }
    }
    rOutStrm << aOutStr.GetBuffer();
    if( nIdx < rData.Len() )
        rOutStrm << "<...>";
}

void lclDumpStringValue( SvStream& rOutStrm, const ByteString& rName, const ByteString& rData )
{
    rOutStrm << rName.GetBuffer() << "='";
    lclDumpString( rOutStrm, rData );
    rOutStrm << '\'';
}

void lclDumpString( SvStream& rOutStrm, const String& rData )
{
    ByteString aOutStr;
    xub_StrLen nIdx = 0;
    for( ; (nIdx < rData.Len()) && (aOutStr.Len() < 80); ++nIdx )
    {
        sal_Unicode cChar = rData.GetChar( nIdx );
        if( (32 <= cChar) && (cChar <= 255) )
            aOutStr.Append( static_cast< sal_Char >( cChar ) );
        else
        {
            aOutStr.Append( '<' );
            if( cChar < 256 )
                __AddHex( aOutStr, static_cast< sal_uInt8 >( cChar ) );
            else
                __AddHex( aOutStr, static_cast< sal_uInt16 >( cChar ) );
            aOutStr.Append( '>' );
        }
    }
    rOutStrm << aOutStr.GetBuffer();
    if( nIdx < rData.Len() )
        rOutStrm << "<...>";
}

void lclDumpStringValue( SvStream& rOutStrm, const ByteString& rName, const String& rData )
{
    rOutStrm << rName.GetBuffer() << "='";
    lclDumpString( rOutStrm, rData );
    rOutStrm << '\'';
}

} // namespace


IdRangeList::~IdRangeList()
{
    Clear();
}


void IdRangeList::Clear( void )
{
    IdRange*    p = ( IdRange* ) First();

    while( p )
    {
        delete p;
        p = ( IdRange* ) Next();
    }

    List::Clear();
}

// ============================================================================
//
//  H E L P E R   O B J E C T S
//
// ============================================================================

namespace {

// ----------------------------------------------------------------------------

class XclDumpStreamHeader
{
public:
    explicit            XclDumpStreamHeader( SvStream& rInStrm, SvStream& rOutStrm, const String& rStrmName, const String& rStrmPath );
                        ~XclDumpStreamHeader();
    inline ULONG        GetStreamLen() const { return mnStrmLen; }

private:
    SvStream&           mrOutStrm;
    String              maStrmName;
    String              maStrmPath;
    ByteString          maSeparator;
    ULONG               mnStrmLen;
};

XclDumpStreamHeader::XclDumpStreamHeader( SvStream& rInStrm, SvStream& rOutStrm, const String& rStrmName, const String& rStrmPath ) :
    mrOutStrm( rOutStrm ),
    maStrmName( rStrmName ),
    maStrmPath( rStrmPath ),
    mnStrmLen( 0 )
{
    maSeparator.Assign( '+' ).Expand( 78, '-' );

    rInStrm.Seek( STREAM_SEEK_TO_END );
    mnStrmLen = rInStrm.Tell();
    rInStrm.Seek( STREAM_SEEK_TO_BEGIN );

    ByteString aLine;
    lclAppendDec( aLine, mnStrmLen );

    mrOutStrm << maSeparator.GetBuffer() << "\n";
    mrOutStrm << "| STREAM-BEGIN\n";
    mrOutStrm << "|   ";
    lclDumpStringValue( mrOutStrm, "stream-name", maStrmName );
    mrOutStrm << "\n|   ";
    lclDumpStringValue( mrOutStrm, "stream-path", maStrmPath );
    mrOutStrm << "\n|   stream-len=" << aLine.GetBuffer() << "\n";
    mrOutStrm << "|\n\n";
}

XclDumpStreamHeader::~XclDumpStreamHeader()
{
    mrOutStrm << "|\n";
    mrOutStrm << "|   ";
    lclDumpStringValue( mrOutStrm, "stream-name", maStrmName );
    mrOutStrm << "\n|   ";
    lclDumpStringValue( mrOutStrm, "stream-path", maStrmPath );
    mrOutStrm << "\n";
    mrOutStrm << "| STREAM-END\n";
    mrOutStrm << maSeparator.GetBuffer() << "\n\n";
}

// ----------------------------------------------------------------------------

class XclDumpStorageHeader
{
public:
    explicit            XclDumpStorageHeader( SotStorage& rInStrg, SvStream& rOutStrm, const String& rStrgPath );
                        ~XclDumpStorageHeader();

private:
    SvStream&           mrOutStrm;
    String              maStrgName;
    String              maStrgPath;
    ByteString          maSeparator;
};

XclDumpStorageHeader::XclDumpStorageHeader( SotStorage& rInStrg, SvStream& rOutStrm, const String& rStrgPath ) :
    mrOutStrm( rOutStrm ),
    maStrgName( rInStrg.GetName() ),
    maStrgPath( rStrgPath )
{
    maSeparator.Assign( "++" ).Expand( 78, '=' );

    mrOutStrm << maSeparator.GetBuffer() << "\n";
    mrOutStrm << "|| STORAGE-BEGIN\n";
    mrOutStrm << "||   ";
    lclDumpStringValue( mrOutStrm, "storage-name", maStrgName );
    mrOutStrm << "\n||   ";
    lclDumpStringValue( mrOutStrm, "storage-path", maStrgPath );
    mrOutStrm << "\n";

    SvStorageInfoList aInfoList;
    rInStrg.FillInfoList( &aInfoList );
    ByteString aLine;
    lclAppendDec( aLine, aInfoList.Count() );
    mrOutStrm << "||   directory-size=" << aLine.GetBuffer() << "\n";

    for( ULONG nInfo = 0; nInfo < aInfoList.Count(); ++nInfo )
    {
        SvStorageInfo& rInfo = aInfoList.GetObject( nInfo );
        mrOutStrm << "||     type=";
        if( rInfo.IsStream() )
            mrOutStrm << "stream   ";
        else if( rInfo.IsStorage() )
            mrOutStrm << "storage  ";
        else
            mrOutStrm << "unknown  ";
        lclDumpStringValue( mrOutStrm, "name", rInfo.GetName() );
        mrOutStrm << "\n";
    }

    mrOutStrm << "||\n\n";
}

XclDumpStorageHeader::~XclDumpStorageHeader()
{
    mrOutStrm << "||\n";
    mrOutStrm << "||   ";
    lclDumpStringValue( mrOutStrm, "storage-name", maStrgName );
    mrOutStrm << "\n||   ";
    lclDumpStringValue( mrOutStrm, "storage-path", maStrgPath );
    mrOutStrm << "\n";
    mrOutStrm << "|| STORAGE-END\n";
    mrOutStrm << maSeparator.GetBuffer() << "\n\n";
}

// ----------------------------------------------------------------------------

}

// ============================================================================
//
// ============================================================================

void Biff8RecDumper::Print( const ByteString& r )
{
    DBG_ASSERT( pDumpStream, "-Biff8RecDumper::Print(): Stream is wech!" );
    *pDumpStream << '\n' << pLevelPre;
    pDumpStream->Write( r.GetBuffer(), r.Len() );
}


void Biff8RecDumper::Print( const sal_Char* p )
{
    DBG_ASSERT( pDumpStream, "-Biff8RecDumper::Print(): Stream is wech!" );
    DBG_ASSERT( p, "-Biff8RecDumper::Print(): ByteString is wech!" );

    *pDumpStream << '\n' << pLevelPre << p;
}


static const sal_Char* GetSeriesType( const UINT16 n )
{
    const sal_Char* p;

    switch( n )
    {
        case 0:     p = "(date)    "; break;
        case 1:     p = "(numeric) "; break;
        case 2:     p = "(sequence)"; break;
        case 3:     p = "(text)    "; break;
        default:    p = "(unknown) ";
    }

    return p;
}


static const sal_Char* GetLineType( const UINT16 n )
{
    const sal_Char* p;

    switch( n )
    {
        case 0:     p = "solid";                break;
        case 1:     p = "dash";                 break;
        case 2:     p = "dot";                  break;
        case 3:     p = "dash-dot";             break;
        case 4:     p = "dash-dot-dot";         break;
        case 5:     p = "none";                 break;
        case 6:     p = "dark gray pattern";    break;
        case 7:     p = "medium gray pattern";  break;
        case 8:     p = "light gray pattern";   break;
        default:    p = pU;
    }

    return p;
}


static ByteString GetRGB( const UINT32 n )
{
    ByteString  s;

    s += "R";
    __AddDec( s, ( UINT8 ) n );
    s += " G";
    __AddDec( s, ( UINT8 ) ( n >> 8 ) );
    s += " B";
    __AddDec( s, ( UINT8 ) ( n >> 16 ) );

    return s;
}


static void AddRef( ByteString& t, UINT16 nRow, UINT16 nC, BOOL bName, UINT16 nTab = 0xFFFF )
{
    BOOL bColRel = ( nC & 0x4000 ) != 0;
    BOOL bRowRel = ( nC & 0x8000 ) != 0;
    UINT8 nCol = (UINT8) nC;
    INT8 nRelCol = (INT8) nCol;
    INT16 nRelRow = (INT16) nRow;

    if( nTab < 0xFFFF )
    {
        t += "XTI(";
        __AddDec( t, nTab );
        t += ")!";
    }

    if( bName )
    {
        // dump relative: [Column|Row]
        // [C-1,R-1] = one column left, one row up
        // [C+1,R+1] = one column right, one row down
        // [C,R]     = same column/row
        // [C=B,R=2] = absolute column B/row 2
        t += "[C";
        if( bColRel )
        {
            if( nRelCol > 0 )
                t += '+';
            if( nRelCol != 0 )
                __AddDec( t, (INT16)nRelCol );
        }
        else
        {
            t += '=';
            t += GETSTR( ::ColToAlpha( nCol ) );
        }

        t += ",R";
        if( bRowRel )
        {
            if( nRelRow > 0 )
                t += "+";
            if( nRelRow != 0 )
                __AddDec( t, nRelRow );
        }
        else
        {
            t += '=';
            __AddDec( t, (INT32)nRow + 1 );
        }
        t += ']';
    }
    else
    {
        if( !bColRel )
            t += '$';
        t += GETSTR( ::ColToAlpha( nCol ) );
        if( !bRowRel )
            t += '$';
        __AddDec( t, (UINT16)(nRow + 1) );
    }
}

static void AddRangeRef( ByteString& t, UINT16 nRow1, UINT16 nC1, UINT16 nRow2, UINT16 nC2, BOOL bName, UINT16 nTab = 0xFFFF )
{
    AddRef( t, nRow1, nC1, bName, nTab );
    if( (nRow1 != nRow2) || (nC1 != nC2) )
    {
        t += ':';
        AddRef( t, nRow2, nC2, bName );
    }
}



static void AddString( ByteString& t, XclImpStream& r, const UINT16 nLen )
{
    if( nLen )
    {
        UINT16  n = nLen;
        UINT8   c;
        while( n )
        {
            r >> c;
            if( c < ' ' )
            {
                t += '<';
                __AddHex( t, c );
                t += '>';
            }
            else
                t += ( sal_Char ) c;
            n--;
        }
    }
}


static BOOL AddUNICODEString( ByteString& rStr, XclImpStream& rStrm, const BOOL b16BitLen = TRUE, UINT16 nLen = 0, ByteString* pRawName = 0 )
{
    BOOL bRet = TRUE;

    if( !nLen )
        nLen = b16BitLen ? rStrm.ReaduInt16() : rStrm.ReaduInt8();
    UINT8 nGrbit = rStrm.ReaduInt8();

    UINT32  nExtLen;
    UINT16  nCrun;
    bool    b16Bit, bFarEast, bRichString;
    ULONG   nSeek = rStrm.ReadUniStringExtHeader( b16Bit, bRichString, bFarEast, nCrun, nExtLen, nGrbit );

    rStr += "(l=";
    __AddDec( rStr, nLen );
    rStr += " f=";
    __AddHex( rStr, nGrbit );
    rStr += " ";
    rStr += b16Bit ? "16-Bit" : "8-Bit";

    if( bRichString && bFarEast )
        rStr += " rich far-east";
    else if( bRichString && !bFarEast )
        rStr += " rich";
    else if ( !bRichString && bFarEast )
        rStr += " far-east";
    rStr += ") '";

    ByteString aData( rStrm.ReadRawUniString( nLen, b16Bit ), RTL_TEXTENCODING_MS_1252 );
    if( pRawName ) *pRawName = aData;

    xub_StrLen nIndex = 0;
    while( (nIndex < aData.Len()) && (nIndex < 255) )
    {
        UINT8 nChar = (UINT8)aData.GetChar( nIndex );
        if( nChar < ' ' )
        {
            ByteString aIns( '<' );
            __AddHex( aIns, nChar );
            aIns += '>';
            aData.Erase( nIndex, 1 ).Insert( aIns, nIndex );
            nIndex += 5;
        }
        nIndex++;
    }

    rStr += aData.Copy( 0, 255 );
    rStr += '\'';
    if( aData.Len() > 255 )
        rStr += "...";

    if( nCrun )
    {
        rStr += " + ";
        __AddDec( rStr, nCrun );
        rStr += " format blocks (";
        nCrun *= 4;
        __AddDec( rStr, nCrun );
        rStr += " bytes)";
        rStrm.Ignore( nCrun );
    }
    if( nExtLen )
    {
        rStr += " + ";
        __AddDec( rStr, nExtLen );
        rStr += " byte extended:";
        for( sal_uInt32 nIdx = 0; rStrm.IsValid() && (nIdx < nExtLen); ++nIdx )
        {
            rStr.Append( ' ' );
            __AddPureHex( rStr, rStrm.ReaduInt8() );
        }
    }

    return bRet;
}


DUMP_ERR::~DUMP_ERR()
{
    if( pHint )
        delete pHint;
}




#define Read1(rIn)              (rIn).ReaduInt8()
#define Read2(rIn)              (rIn).ReaduInt16()
#define Read4(rIn)              (rIn).ReaduInt32()
#define Read8(rIn)              (rIn).ReadDouble()
#define LINESTART()             {t.Erase();t+=pPre;}
#define IGNORE(n)               rIn.Ignore(n)
#define ADDBIN(n)               __AddBin( t, Read##n( rIn ) )
#define ADDHEX(n)               __AddHex( t, Read##n( rIn ) )
#define ADDDEC(n)               __AddDec( t, Read##n( rIn ) )
#define ADDDOUBLE()             __AddDouble( t, rIn.ReadDouble() )
#define ADD16P16()              __Add16p16( t, Read4( rIn ) )
#define ADDTEXT(T)              t += T
#define PRINT()                 Print( t )
#define PreDump(LEN)            {rIn.PushPosition();ContDump(LEN);rIn.PopPosition();}
#define ADDCELLHEAD()           {UINT16 nR,nC,nX;rIn>>nR>>nC>>nX;__AddCellHead(t,nC,nR,nX);}
#define STARTFLAG()             {ADDTEXT( "flags=" ); __AddHex( t, __nFlags ); ADDTEXT( " " );}
#define ADDFLAG(mask,text)      {if( __nFlags & mask ) t.Append( ' ' ).Append( text );}
#define ADDRESERVED(mask)       ADDFLAG(mask,"!RESERVED!")


UINT16 Biff8RecDumper::DumpXF( XclImpStream& rStrm, const sal_Char* pPre )
{
    ByteString t;   // "t" needed by macros

    sal_uInt32 nBorder1, nBorder2;
    sal_uInt16 nFont, nNumFmt, nTypeProt, nAlign, nMiscAttrib, nArea, __nFlags, nTmp;
    rStrm >> nFont >> nNumFmt >> nTypeProt >> nAlign >> nMiscAttrib >> nBorder1 >> nBorder2 >> nArea;
    bool bCell = !::get_flag( nTypeProt, EXC_XF_STYLE );

    // XF type/parent
    LINESTART();
    ::extract_value( nTmp, nTypeProt, 4, 12 );
    ADDTEXT( "index=#" );           __AddDec( t, nXFCount++ );
    ADDTEXT( "   type=" );          lcl_AddFlag( t, bCell, "cell", "style" );
    ADDTEXT( "   parent-xf=#" );    __AddDec( t, nTmp );
    PRINT();

    // attribute used flags
    LINESTART();
    ::extract_value( __nFlags, nMiscAttrib, 10, 6 );
    if( !bCell ) __nFlags ^= 0x3F;  // in style XFs a 0 means used
    ADDTEXT( "used " ); STARTFLAG();
    ADDFLAG( EXC_XF_DIFF_VALFMT, "numfmt" );
    ADDFLAG( EXC_XF_DIFF_FONT, "font" );
    ADDFLAG( EXC_XF_DIFF_ALIGN, "align" );
    ADDFLAG( EXC_XF_DIFF_BORDER, "border" );
    ADDFLAG( EXC_XF_DIFF_AREA, "area" );
    ADDFLAG( EXC_XF_DIFF_PROT, "prot" );
    PRINT();

    // cell protection/font/number format
    LINESTART();
    ADDTEXT( "cell-lock=" );        lcl_AddOnOff( t, ::get_flag( nTypeProt, EXC_XF_LOCKED ) );
    ADDTEXT( "   hidden=" );        lcl_AddOnOff( t, ::get_flag( nTypeProt, EXC_XF_HIDDEN ) );
    ADDTEXT( "   font=" );          __AddDec( t, nFont );
    ADDTEXT( "   num-fmt=" );       __AddDec( t, nNumFmt );
    PRINT();

    // alignment
    LINESTART();
    ::extract_value( nTmp, nAlign, 0, 3 );
    ADDTEXT( "hor-align=" );        __AddDec( t, nTmp );
    ADDTEXT( " (" );
    switch( nTmp )
    {
        case EXC_XF_HOR_GENERAL:    ADDTEXT( "general" );   break;
        case EXC_XF_HOR_LEFT:       ADDTEXT( "left" );      break;
        case EXC_XF_HOR_CENTER:     ADDTEXT( "center" );    break;
        case EXC_XF_HOR_RIGHT:      ADDTEXT( "right" );     break;
        case EXC_XF_HOR_FILL:       ADDTEXT( "fill" );      break;
        case EXC_XF_HOR_JUSTIFY:    ADDTEXT( "justify" );   break;
        case EXC_XF_HOR_CENTER_AS:  ADDTEXT( "center-as" ); break;
        case EXC_XF_HOR_DISTRIB:    ADDTEXT( "distrib" );   break;
        default:                    ADDTEXT( "!unknown!" );
    };
    ::extract_value( nTmp, nAlign, 4, 3 );
    ADDTEXT( ")   ver-align=" );    __AddDec( t, nTmp );
    ADDTEXT( " (" );
    switch( nTmp )
    {
        case EXC_XF_VER_TOP:        ADDTEXT( "top" );       break;
        case EXC_XF_VER_CENTER:     ADDTEXT( "center" );    break;
        case EXC_XF_VER_BOTTOM:     ADDTEXT( "bottom" );    break;
        case EXC_XF_VER_JUSTIFY:    ADDTEXT( "justify" );   break;
        case EXC_XF_VER_DISTRIB:    ADDTEXT( "distrib" );   break;
        default:                    ADDTEXT( "!unknown!" );
    };
    ADDTEXT( ")   text-wrap=" );    lcl_AddOnOff( t, ::get_flag( nAlign, EXC_XF_LINEBREAK ) );
    PRINT();

    LINESTART();
    ::extract_value( nTmp, nAlign, 8, 8 );
    ADDTEXT( "rotation=" );         __AddDec( t, nTmp );
    ADDTEXT( " (" );
    if( nTmp < 91 )
        { __AddDec( t, nTmp ); ADDTEXT( "\xB0" ); }
    else if( nTmp < 181 )
        { __AddDec( t, static_cast< sal_Int32 >( 90 - nTmp ) ); ADDTEXT( "\xB0" ); }
    else if( nTmp == EXC_ROT_STACKED )
        { ADDTEXT( "stacked" ); }
    else
        { ADDTEXT( "!unknown!" ); }
    ::extract_value( nTmp, nMiscAttrib, 0, 4 );
    ADDTEXT( ")   indent=" );       __AddDec( t, nTmp );
    ADDTEXT( "   shrink=" );        lcl_AddOnOff( t, ::get_flag( nMiscAttrib, EXC_XF8_SHRINK ) );
    ::extract_value( nTmp, nMiscAttrib, 6, 2 );
    ADDTEXT( "   text-dir=" );      __AddDec( t, nTmp );
    ADDTEXT( " (" );
    switch( nTmp )
    {
        case EXC_XF_TEXTDIR_CONTEXT:    ADDTEXT( "context" );   break;
        case EXC_XF_TEXTDIR_LTR:        ADDTEXT( "ltr" );       break;
        case EXC_XF_TEXTDIR_RTL:        ADDTEXT( "rtl" );       break;
        default:                        ADDTEXT( "!unknown!" );
    };
    ADDTEXT( ")" );
    PRINT();

    // border/area
    LINESTART();
    ::extract_value( nTmp, nBorder1, 0, 4 );
    ADDTEXT( "left-line=" );        __AddDec( t, nTmp );
    ::extract_value( nTmp, nBorder1, 16, 7 );
    ADDTEXT( " color=" );           __AddDec( t, nTmp );
    ::extract_value( nTmp, nBorder1, 4, 4 );
    ADDTEXT( "   right-line =" );   __AddDec( t, nTmp );
    ::extract_value( nTmp, nBorder1, 23, 7 );
    ADDTEXT( " color=" );           __AddDec( t, nTmp );
    PRINT();
    LINESTART();
    ::extract_value( nTmp, nBorder1, 8, 4 );
    ADDTEXT( "top-line =" );        __AddDec( t, nTmp );
    ::extract_value( nTmp, nBorder2, 0, 7 );
    ADDTEXT( " color=" );           __AddDec( t, nTmp );
    ::extract_value( nTmp, nBorder1, 12, 4 );
    ADDTEXT( "   bottom-line=" );   __AddDec( t, nTmp );
    ::extract_value( nTmp, nBorder2, 7, 7 );
    ADDTEXT( " color=" );           __AddDec( t, nTmp );
    PRINT();
    LINESTART();
    ::extract_value( nTmp, nBorder2, 21, 4 );
    ADDTEXT( "diag-line=" );        __AddDec( t, nTmp );
    ::extract_value( nTmp, nBorder2, 14, 7 );
    ADDTEXT( " color=" );           __AddDec( t, nTmp );
    ADDTEXT( "   diag-tl-to-br=" ); lcl_AddOnOff( t, ::get_flag( nBorder1, 0x40000000UL ) );
    ADDTEXT( "   diag-bl-to-tr=" ); lcl_AddOnOff( t, ::get_flag( nBorder1, 0x80000000UL ) );
    PRINT();
    LINESTART();
    ::extract_value( nTmp, nBorder2, 26, 6 );
    ADDTEXT( "area-pattern=" );     __AddDec( t, nTmp );
    ::extract_value( nTmp, nArea, 0, 7 );
    ADDTEXT( "   fore-color=" );    __AddDec( t, nTmp );
    ::extract_value( nTmp, nArea, 7, 7 );
    ADDTEXT( "   back-color=" );    __AddDec( t, nTmp );
    PRINT();

    return 0;
}

void Biff8RecDumper::DumpValidPassword( XclImpStream& rIn, const sal_Char* pPre )
{
    ByteString  t;
    UINT16 nHash;
    rIn >> nHash;
    LINESTART();
    ADDTEXT( "hash=" );
    __AddHex( t, nHash );
    if( nHash )
    {
        ByteString  sPasswd;
        ByteString  sDummy;
        UINT16  nLen = 9;
        UINT16  nDummy;
        UINT16  nNewChar;

        nHash ^= 0xCE4B;
        nDummy = nHash;
        ADDTEXT( "   without-mask=" );
        __AddHex( t, nHash );
        while( !(nDummy & 0x8000) && nLen )
        {
            nLen--;
            nDummy <<= 1;
        }
        if( !nLen ) nLen = 2;
        if( (nLen ^ nHash) & 0x0001 ) nLen++;
        if( nLen == 9 )
        {
            nLen = 10;
            nHash ^= 0x8001;
        }
        nHash ^= nLen;
        if( nLen < 9 ) nHash <<= (8 - nLen);
        for( UINT16 iChar = nLen; iChar > 0; iChar-- )
        {
            switch( iChar )
            {
                case 10:
                    nNewChar = (nHash & 0xC000) | 0x0400;
                    nHash ^= nNewChar;
                    nNewChar >>= 2;
                    break;
                case 9:
                    nNewChar = 0x4200;
                    nHash ^= nNewChar;
                    nNewChar >>= 1;
                    break;
                case 1:
                    nNewChar = nHash & 0xFF00;
                    break;
                default:
                    nNewChar = (nHash & 0xE000) ^ 0x2000;
                    if( !nNewChar ) nNewChar = (nHash & 0xF000) ^ 0x1800;
                    if( nNewChar == 0x6000 ) nNewChar = 0x6100;
                    nHash ^= nNewChar;
                    nHash <<= 1;
                    break;
            }
            nNewChar >>= 8;
            nNewChar &= 0x00FF;
            sDummy = sPasswd;
            sPasswd = (sal_Char) nNewChar;
            sPasswd += sDummy;
        }
        ADDTEXT( "   valid-password='" );
        t += sPasswd;
        ADDTEXT( "'" );
    }
    PRINT();
}


void __AddGUID( ByteString& rStr, XclImpStream& rIn )
{
    UINT16 nIndex;
    __AddPureHex( rStr, Read4( rIn ) );
    rStr += "-";
    __AddPureHex( rStr, Read2( rIn ) );
    rStr += "-";
    __AddPureHex( rStr, Read2( rIn ) );
    rStr += "-";
    // last 2 parts byte for byte
    for( nIndex = 0; nIndex < 2; nIndex++ )
        __AddPureHex( rStr, Read1( rIn ) );
    rStr += "-";
    for( nIndex = 0; nIndex < 6; nIndex++ )
        __AddPureHex( rStr, Read1( rIn ) );
}


void Biff8RecDumper::PreDumpDecrypted( ULONG nL )
{
    if( !nL ) return;

    ByteString t;
    const sal_Char* pPre = (pLevelPre > pLevelPreString) ? pLevelPre - 1 : pLevelPre;

    LINESTART();
    ADDTEXT( "*** encrypted ***" );
    PRINT();
    pIn->DisableDecryption();
    pIn->Seek( EXC_REC_SEEK_TO_BEGIN );
    ContDump( nL );

    if( pIn->HasValidDecrypter() )
    {
        LINESTART();
        ADDTEXT( "*** decrypted ***" );
        PRINT();
        pIn->EnableDecryption();
        pIn->Seek( EXC_REC_SEEK_TO_BEGIN );
        ContDump( nL );
    }
}


void Biff8RecDumper::RecDump( BOOL bSubStream )
{
    const sal_Char*     p;
    BOOL                bDec = FALSE;
    ByteString          aTmp;
    UINT16              __nFlags;
    const UINT16        nR = pIn->GetRecId();
    const ByteString*   pName = GetName( nR );

    // set CONTINUE handling mode
    switch( nR )
    {
        case 0x000A:        // EOF
        case 0x003C:        // CONT
        case 0x005D:        // OBJ
        case 0x00EC:        // MSODRAWING
        case 0x01B6:        // TXO
            pIn->ResetRecord( false );
        break;
        case 0x1066:        // CHGELFRAME
            pIn->ResetRecord( bReadContRecs, 0x1066 );
        break;
        default:
            pIn->ResetRecord( bReadContRecs );
    }
    const ULONG         nL = pIn->GetRecSize();

    switch( nR )
    {
        case 0x0009:
        case 0x0209:
        case 0x0409:
        case 0x0809:
            nLevelCnt = 0;
            break;
        case 0x1033:
            if( pName )
                aTmp = *pName;
            else
                aTmp = "BEGIN ";
            bDec = ( pLevelPre + nLevelInc ) >= pLevelPreString;
            nLevelCnt++;
            aTmp.Append( ByteString::CreateFromInt32( nLevelCnt ) );
            pName = &aTmp;
            break;
        case 0x1034:
            if( pName )
                aTmp = *pName;
            else
                aTmp = "END ";
            if( ( pLevelPre + nLevelInc ) <= pLevelPreStringNT )
                pLevelPre += nLevelInc;
            if( nLevelCnt )
            {
                aTmp.Append( ByteString::CreateFromInt32( nLevelCnt ) );
                nLevelCnt--;
            }
            else
                aTmp += "#LEVEL ERROR#";
            pName = &aTmp;
            break;
    }

    ByteString      aT;
    ByteString&     t = aT;
    const sal_Char* pPre = (pLevelPre > pLevelPreString) ? pLevelPre - 1 : pLevelPre;

    if( nR || nL )      // skip dummy-zero DIMENSIONS at eof
    {
        if( bBlankLine )
            *pDumpStream << '\n';

        aT += pLevelPre;
        __AddHex( aT, nR );

        if( pName )
        {
            aT += " (";
            aT += *pName;
            aT += ") [";
        }
        else
            aT += " [";

        if( bReadContRecs )
            __AddHex( aT, nL );
        else
            __AddHex( aT, (UINT16)nL );
        aT += "]";
        if( !bSkipOffset )
        {
            aT += " :";
            __AddHex( aT, UINT32(pIn->GetSvStreamPos() - 2 * sizeof(UINT16)) );
            aT += ':';
        }

        Print( aT );
    }

    if( HasModeNameOnly( nR ) )
        ;
    else if( HasModeHex( nR ) || !bBIFF8 )
    {
        if( bEncrypted )
            PreDumpDecrypted( nL );
        else
            ContDump( nL );
    }
    else if( nMaxBodyLines && nL )
    {
        XclImpStream& rIn = *pIn;

        if( bEncrypted )
        {
            PreDumpDecrypted( nL );
            LINESTART();
            ADDTEXT( "*** contents ***" );
            PRINT();
            pIn->Seek( EXC_REC_SEEK_TO_BEGIN );
        }

        LINESTART();

        switch( nR )
        {
            case 0x0000:        // DIMENSIONS - used area
            case 0x0200:
            {
                LINESTART();
                UINT32 nR1, nR2;
                UINT16 nC1, nC2;
                rIn >> nR1 >> nR2 >> nC1 >> nC2;
                ADDTEXT( "first row: " );             __AddHex( t, nR1 );
                ADDTEXT( "   last row+1: " );         __AddHex( t, nR2 );
                ADDTEXT( "   first col: " );          __AddHex( t, nC1 );
                ADDTEXT( "   last col+1: " );         __AddHex( t, nC2 );
                ADDTEXT( "   (" );                    lcl_AddRangeRef( t, nC1, (UINT16)nR1, nC2-1, (UINT16)nR2-1 );
                ADDTEXT( ")" );
                PRINT();
            }
            break;
            case 0x06:
            {
                ADDCELLHEAD();
                PRINT();
                LINESTART();
                ADDTEXT( "val = " );
                ADDDOUBLE();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    ADDTEXT( "   " );
                    STARTFLAG();
                    ADDFLAG( 0x01, "fAlwaysCalc" );
                    ADDFLAG( 0x02, "fCalcOnLoad" );
                    ADDFLAG( 0x08, "fShrFmla" );
                }
                PRINT();
                LINESTART();
                UINT16  n;
                ADDTEXT( "chn = " );
                ADDHEX( 4 );
                rIn >> n;
                ADDTEXT( "    cce = " );
                __AddDec( t, n );
                PRINT();
                FormulaDump( n, FT_CellFormula );
                if( rIn.GetRecLeft() > 0 )
                {
                    LINESTART();
                    ADDTEXT( "additional formula data" );
                    PRINT();
                    ContDump( rIn.GetRecLeft() );
                }
            }
            break;
            case 0x0013:        // PASSWORD
                DumpValidPassword( rIn, pPre );
            break;
            case 0x0014:        // HEADER
            case 0x0015:        // FOOTER
                if( rIn.GetRecLeft() )
                    AddUNICODEString( t, rIn );
                PRINT();
            break;
            case 0x17:
            {
                if( mnSubstream == EXC_BOF_WORKSPACE )
                {
                    ADDTEXT( "filename=" );
                    AddUNICODEString( t, rIn );
                    PRINT();
                }
                else
                {
                    UINT16  n;
                    rIn >> n;
                    ADDTEXT( "# of XTI: " );
                    __AddDec( t, n );
                    PRINT();
                    UINT16  nSB, nF, nL;
                    while( n && rIn.IsValid() )
                    {
                        LINESTART();
                        rIn >> nSB >> nF >> nL;
                        ADDTEXT( "Supbook = " );
                        __AddDec( t, nSB );
                        ADDTEXT( "    Tab = " );
                        __AddDec( t, nF );
                        ADDTEXT( " ... " );
                        __AddDec( t, nL );
                        PRINT();
                        n--;
                    }
                }
            }
            break;
            case 0x0018:
            case 0x0218:        // NAME
            {
                sal_uInt8 nKey, nNameLen, nMenuLen, nDescrLen, nHelpLen, nStatusLen;
                sal_uInt16 nFmlaSize, nRes, nTab;

                rIn >> __nFlags >> nKey >> nNameLen >> nFmlaSize >> nRes >> nTab >> nMenuLen >> nDescrLen >> nHelpLen >> nStatusLen;
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, "fHidden" );
                ADDFLAG( 0x0002, "fFunc" );
                ADDFLAG( 0x0004, "fVBProc" );
                ADDFLAG( 0x0008, "fProc" );
                ADDFLAG( 0x0010, "fCalcExp" );
                ADDFLAG( 0x0020, "fBuiltIn" );
                ADDFLAG( 0x1000, "fBig" );
                ADDRESERVED( 0xE000 );
                ADDTEXT( "   func-group-idx=" );
                __AddDec( t, (UINT16)((__nFlags & 0x0FC0) >> 6) );
                ADDTEXT( "   shortcut=" );      __AddHex( t, nKey );
                PRINT();

                LINESTART();
                ADDTEXT( "fmla-size=" );        __AddDec( t, nFmlaSize );
                ADDTEXT( "   reserved=" );      __AddHex( t, nRes );
                ADDTEXT( "   tab=" );           __AddDec( t, nTab );
                if( !nTab ) ADDTEXT( " (global)" );
                PRINT();

                LINESTART();
                sal_uInt16 nBuiltIn = 0;
                bool bBuiltIn = (nNameLen == 1) && (__nFlags & 0x0020);
                if( bBuiltIn )
                {
                    rIn.PushPosition();
                    sal_uInt8 nStrFlags;
                    rIn >> nStrFlags;
                    nBuiltIn = (nStrFlags & 1) ? rIn.ReaduInt16() : rIn.ReaduInt8();
                    rIn.PopPosition();
                }
                ADDTEXT( "name=" );
                ByteString aName;
                AddUNICODEString( t, rIn, false, nNameLen, &aName );
                if( bBuiltIn )
                {
                    static const sal_Char* const ppcNames[] = {
                        "Consolidate_Area", "Auto_Open", "Auto_Close", "Extract", "Database",
                        "Criteria", "Print_Area", "Print_Titles", "Recorder", "Data_Form",
                        "Auto_Activate", "Auto_Deactivate", "Sheet_Title", "_FilterDatabase" };
                    lcl_AddEnum( t, nBuiltIn, ppcNames, STATIC_TABLE_SIZE( ppcNames ) );
                    if( (0 <= nBuiltIn) && (nBuiltIn < STATIC_TABLE_SIZE( ppcNames )) )
                        aName.Assign( ppcNames[ nBuiltIn ] );
                }
                maNames.push_back( aName );
                PRINT();

                if( nFmlaSize && (rIn.GetRecLeft() > 0) )
                {
                    LINESTART();
                    ADDTEXT( "name-definition=" );
                    PRINT();
                    FormulaDump( nFmlaSize, FT_RangeName );
                }
                if( nMenuLen )
                {
                    LINESTART();
                    ADDTEXT( "menu-text=" );
                    AddUNICODEString( t, rIn, false, nMenuLen );
                    PRINT();
                }
                if( nDescrLen )
                {
                    LINESTART();
                    ADDTEXT( "descr-text=" );
                    AddUNICODEString( t, rIn, false, nDescrLen );
                    PRINT();
                }
                if( nHelpLen )
                {
                    LINESTART();
                    ADDTEXT( "help-text=" );
                    AddUNICODEString( t, rIn, false, nHelpLen );
                    PRINT();
                }
                if( nStatusLen )
                {
                    LINESTART();
                    ADDTEXT( "status-text=" );
                    AddUNICODEString( t, rIn, false, nStatusLen );
                    PRINT();
                }
            }
            break;
            case 0x001D:        // SELECTION - list of selections
            {
                ADDTEXT( "pane: " );                ADDDEC( 1 );
                ADDTEXT( "   active cell: " );
                UINT16 nR, nC;
                rIn >> nR >> nC;
                lcl_AddRef( t, nC, nR );
                ADDTEXT( "   active index: " );     ADDDEC( 2 );
                ADDTEXT( "   ref count: " );
                UINT16 nCount;
                rIn >> nCount;
                __AddDec( t, nCount );
                PRINT();
                for( UINT16 nIndex = 0; nIndex < nCount && rIn.IsValid(); nIndex++ )
                {
                    LINESTART();
                    UINT16 nR1, nR2;
                    UINT8 nC1, nC2;
                    rIn >> nR1 >> nR2 >> nC1 >> nC2;
                    ADDTEXT( "ref#" );      __AddDec( t, nIndex, 3 );
                    ADDTEXT( ": " );        lcl_AddRangeRef( t, nC1, nR1, nC2, nR2 );
                    PRINT();
                }
            }
            break;
            case 0x0023:        // EXTERNNAME
            {
                PreDump( rIn.GetRecSize() );
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0001, "builtin" );
                ADDFLAG( 0x0002, "automatic" );
                ADDFLAG( 0x0004, "wantcliprepr" );
                ADDFLAG( 0x0008, "DDEstddocname" );
                ADDFLAG( 0x0010, "OLE" );
                ADDRESERVED( 0x8000 );
                sal_uInt16 nClip = (__nFlags & 0x7FE0) >> 5;
                ADDTEXT( "   clip-format=" );       __AddHex( t, nClip );
                PRINT(); LINESTART();
                bool bFormula = false, bArray = false;
                if( (__nFlags & 0x0001) || !(__nFlags & 0x7FFE) )
                {
                    ADDTEXT( "type=external name" );
                    ADDTEXT( "   table=" );             ADDDEC( 2 );
                    ADDTEXT( "   reserved=" );          ADDHEX( 2 );
                    bFormula = true;
                }
                else if( __nFlags & 0x0010 )
                {
                    ADDTEXT( "type=OLE" );
                    ADDTEXT( "   stream-id=" );         ADDHEX( 4 );
                }
                else
                {
                    ADDTEXT( "type=DDE" );
                    ADDTEXT( "   reserved=" );          ADDHEX( 4 );
                    bArray = true;
                }
                PRINT(); LINESTART();
                ADDTEXT( "name=" );                 AddUNICODEString( t, rIn, FALSE );
                PRINT();
                if( rIn.GetRecLeft() > 0 )
                {
                    LINESTART();
                    if( bFormula )
                    {
                        sal_uInt16 nSize = rIn.ReaduInt16();
                        ADDTEXT( "formula (size=" );    __AddDec( t, nSize );
                        ADDTEXT( "):" );
                        PRINT();
                        FormulaDump( nSize, FT_RangeName );
                    }
                    else if( bArray && (rIn.GetRecLeft() >= 3) )
                    {
                        LINESTART();
                        ADDTEXT( "constant array   width=" );   ADDDEC( 1 );
                        ADDTEXT( "   height=" );                ADDDEC( 2 );
                        PRINT();
                        while( rIn.GetRecLeft() > 0 )
                        {
                            sal_uInt8 nType = rIn.ReaduInt8();
                            LINESTART();
                            ADDTEXT( "type=" );                 __AddHex( t, nType );
                            ADDTEXT( " (" );
                            switch( nType )
                            {
                                case 0x00:
                                    ADDTEXT( "empty)    reserved=" );   ADDHEX( 4 );
                                    ADDTEXT( " " );                     ADDHEX( 4 );
                                break;
                                case 0x01:
                                    ADDTEXT( "double)   value=" );      ADDDOUBLE();
                                break;
                                case 0x02:
                                    ADDTEXT( "string)   text=" );       AddUNICODEString( t, rIn );
                                break;
                                case 0x04:
                                    ADDTEXT( "bool)     value=" );      lcl_AddFlag( t, rIn.ReaduInt8() );
                                    ADDTEXT( "   reserved=" );          ADDHEX( 1 );
                                    ADDTEXT( " " );                     ADDHEX( 2 );
                                    ADDTEXT( " " );                     ADDHEX( 4 );
                                break;
                                case 0x10:
                                    ADDTEXT( "error)    code=" );        ADDHEX( 1 );
                                    ADDTEXT( "   reserved=" );          ADDHEX( 1 );
                                    ADDTEXT( " " );                     ADDHEX( 2 );
                                    ADDTEXT( " " );                     ADDHEX( 4 );
                                break;
                                default:
                                    ADDTEXT( "!unknown!)" );
                                    rIn.Ignore( 8 );
                            }
                            PRINT();
                        }
                    }
                    if( rIn.GetRecLeft() > 0 )
                    {
                        LINESTART();
                        ADDTEXT( "additional data:" );
                        PRINT();
                        ContDump( rIn.GetRecLeft() );
                    }
                }
            }
            break;
            case 0x0026:
            case 0x0027:
            case 0x0028:
            case 0x0029:
                LINESTART();
                ADDDOUBLE();
                PRINT();
            break;
            case 0x002F:        // FILEPASS
            {
                LINESTART();
                sal_uInt16 nType;
                rIn >> nType;
                ADDTEXT( "encrypt-type=" );     __AddHex( t, nType );
                ADDTEXT( " (" );
                switch( nType )
                {
                    case 0x0000:
                    {
                        ADDTEXT( "BIFF2-BIFF7 XOR)   key=" );
                        ADDHEX( 2 );
                        ADDTEXT( "   hash=" );
                        ADDHEX( 2 );
                        PRINT();
                    }
                    break;

                    case 0x0001:
                    {
                        ADDTEXT( "BIFF8 standard/strong)" );
                        PRINT();
                        LINESTART();
                        ADDTEXT( "reserved=" );     ADDHEX( 2 );
                        sal_uInt16 nMode;
                        rIn >> nMode;
                        ADDTEXT( "   mode=" );      __AddHex( t, nMode );
                        ADDTEXT( " (" );
                        switch( nMode )
                        {
                            case 0x0001:
                            {
                                ADDTEXT( "BIFF8 standard)" );
                                PRINT();
                                LINESTART();
                                ADDTEXT( "document-id=..." );
                                PRINT();
                                ContDump( 16 );
                                LINESTART();
                                ADDTEXT( "salt-data=..." );
                                PRINT();
                                ContDump( 16 );
                                LINESTART();
                                ADDTEXT( "salt-hash=..." );
                                PRINT();
                                ContDump( 16 );
                            }
                            break;
                            case 0x0002:
                            {
                                ADDTEXT( "BIFF8X strong)   flags=" );
                                ADDHEX( 4 );
                                PRINT();
                                LINESTART();
                                ADDTEXT( "info-size=" );        ADDHEX( 4 );
                                ADDTEXT( "    flags=" );        ADDHEX( 4 );
                                ADDTEXT( "    unknown=" );      ADDHEX( 4 );
                                PRINT();
                                LINESTART();
                                ADDTEXT( "stream-crypt-id=" );  ADDHEX( 4 );
                                ADDTEXT( "   hash-algo-id=" );  ADDHEX( 4 );
                                ADDTEXT( "   hash-key-len=" );  ADDDEC( 4 );
                                PRINT();
                                LINESTART();
                                ADDTEXT( "crypt-prov-type=" );  ADDHEX( 4 );
                                ADDTEXT( "   unknown=" );       ADDHEX( 4 );
                                ADDTEXT( "   unknown=" );       ADDHEX( 4 );
                                PRINT();
                                LINESTART();
                                ADDTEXT( "crypt-provider-name='" );
                                sal_uInt16 nChar;
                                do
                                {
                                    rIn >> nChar;
                                    if( nChar )
                                        t += (sal_Char)(((32 <= nChar) && (nChar <=127)) ? nChar : '.');
                                }
                                while( nChar );
                                ADDTEXT( "'" );
                                PRINT();
                                LINESTART();
                                sal_uInt32 nLen;
                                rIn >> nLen;
                                ADDTEXT( "*** document-id ***   len=" );   __AddHex( t, nLen );
                                PRINT();
                                ContDump( nLen );
                                LINESTART();
                                ADDTEXT( "*** salt-data ***   len=" );   __AddHex( t, nLen );
                                PRINT();
                                ContDump( nLen );
                                LINESTART();
                                rIn >> nLen;
                                ADDTEXT( "*** salt-hash ***   len=" );   __AddHex( t, nLen );
                                PRINT();
                                ContDump( nLen );
                            }
                            break;
                            default:
                            {
                                ADDTEXT( "!unknown!)" );
                                PRINT();
                                ContDump( rIn.GetRecLeft() );
                            }
                        }
                    }
                    break;

                    default:
                    {
                        ADDTEXT( "!unknown!)" );
                        PRINT();
                        ContDump( rIn.GetRecLeft() );
                    }
                }
            }
            break;
            case 0x0031:        // FONT
            case 0x0231:
            {
                LINESTART();
                ADDTEXT( "(index=" );           __AddDec( t, nFontIndex );
                ADDTEXT( ")   " );
                nFontIndex++; if( nFontIndex == 4 ) nFontIndex++;
                ADDTEXT( "height: " );          ADDDEC( 2 );
                ADDTEXT( "/20pt   " );
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0002, "fItalic" );
                ADDFLAG( 0x0008, "fStrikeout" );
                ADDFLAG( 0x0010, "fOutline" );
                ADDFLAG( 0x0020, "fShadow" );
                ADDRESERVED( 0xFFC5 );
                PRINT();
                LINESTART();
                ADDTEXT( "color: " );           ADDDEC( 2 );
                ADDTEXT( "   boldness: " );     ADDDEC( 2 );
                ADDTEXT( "   sub/sup: " );      ADDDEC( 2 );
                ADDTEXT( "   underline: " );    ADDHEX( 1 );
                PRINT();
                LINESTART();
                ADDTEXT( "family: " );          ADDDEC( 1 );
                ADDTEXT( "   charset: " );      ADDDEC( 1 );
//               ADDTEXT( "   reserved: " );     ADDHEX( 1 );
                rIn.Ignore( 1 );
                ADDTEXT( "   " );
                AddUNICODEString( t, rIn, FALSE );
                PRINT();
            }
            break;
            case 0x003D:        // WINDOW1
            {
                LINESTART();
                ADDTEXT( "pos-x=" );            ADDDEC( 2 );
                ADDTEXT( "   pos-y=" );         ADDDEC( 2 );
                ADDTEXT( "   width=" );         ADDDEC( 2 );
                ADDTEXT( "   height=" );        ADDDEC( 2 );
                PRINT();
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0001, "hide-window" );
                ADDFLAG( 0x0002, "min-window" );
                ADDFLAG( 0x0008, "show-hscroll" );
                ADDFLAG( 0x0010, "show-vscroll" );
                ADDFLAG( 0x0020, "show-tabbar" );
                ADDRESERVED( 0xFFC4 );
                PRINT();
                LINESTART();
                ADDTEXT( "active-tab=" );       ADDDEC( 2 );
                ADDTEXT( "   first-vis-tab=" ); ADDDEC( 2 );
                ADDTEXT( "   selected-tabs=" ); ADDDEC( 2 );
                ADDTEXT( "   tabbar-width=" );  ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x0041:        // PANE
            {
                LINESTART();
                ADDTEXT( "vert-split-pos=" );   ADDDEC( 2 );
                ADDTEXT( "   hor-split-pos=" ); ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "first-row=" );        ADDDEC( 2 );
                ADDTEXT( "   first-col=" );     ADDDEC( 2 );
                ADDTEXT( "   active-pane=" );   ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x0042:        // CODEPAGE
            {
                LINESTART();
                sal_uInt16 nCodePage = rIn.ReaduInt16();
                ADDTEXT( "codepage=" );         __AddHex( t, nCodePage );
                ADDTEXT( " (" );                __AddDec( t, nCodePage );
                ADDTEXT( ")" );
                PRINT();
            }
            break;
            case 0x004D:        // PLS
            {
                LINESTART();
                static const sal_Char* const ppcTypes[] = { "Win", "Mac" };
                sal_uInt16 nType = rIn.ReaduInt16();
                ADDTEXT( "environment=" );
                lcl_AddEnum( t, nType, ppcTypes, STATIC_TABLE_SIZE( ppcTypes ) );
                PRINT();
                if( nType == 0 )
                {
                    String aData;
                    sal_uInt32 __nFlags;
                    LINESTART();
                    rIn.SetNulSubstChar( '\0' );
                    aData = rIn.ReadRawUniString( 32, true );
                    ADDTEXT( "device-name='" );         ADDTEXT( GETSTR( aData ) );
                    ADDTEXT( "'" );
                    PRINT(); LINESTART();
                    ADDTEXT( "spec-version=" );         ADDDEC( 2 );
                    ADDTEXT( "   driver-version=" );    ADDDEC( 2 );
                    sal_uInt16 nOwnSize, nPrvSize;
                    rIn >> nOwnSize >> nPrvSize;
                    ADDTEXT( "   own-size=" );          __AddDec( t, nOwnSize );
                    ADDTEXT( "   prv-size=" );          __AddDec( t, nPrvSize );
                    PRINT(); LINESTART();
                    rIn >> __nFlags;
                    STARTFLAG();
                    ADDFLAG( 0x00000001, "orient" );
                    ADDFLAG( 0x00000002, "paper-size" );
                    ADDFLAG( 0x00000004, "paper-height" );
                    ADDFLAG( 0x00000008, "paper-width" );
                    ADDFLAG( 0x00000010, "scale" );
                    ADDFLAG( 0x00000100, "copies" );
                    ADDRESERVED( 0xE0000080 );
                    PRINT(); LINESTART();
                    static const sal_Char* const ppcOrient[] = { 0, "portrait", "landsc" };
                    sal_uInt16 nOrient = rIn.ReaduInt16();
                    ADDTEXT( "orientation=" );
                    lcl_AddEnum( t, nOrient, ppcOrient, STATIC_TABLE_SIZE( ppcOrient ) );
                    ADDTEXT( "   paper-size=" );        ADDDEC( 2 );
                    ADDTEXT( "   paper-width=" );       ADDDEC( 2 );
                    ADDTEXT( "   paper-height=" );      ADDDEC( 2 );
                    PRINT(); LINESTART();
                    ADDTEXT( "scale=" );                ADDDEC( 2 );
                    ADDTEXT( "   copies=" );            ADDDEC( 2 );
                    PRINT();
                    if( nOwnSize > 88 )
                    {
                        LINESTART(); ADDTEXT( "additional data:" ); PRINT();
                        ContDump( nOwnSize - 88 );
                    }
                    if( nPrvSize > 0 )
                    {
                        LINESTART(); ADDTEXT( "private data:" ); PRINT();
                        ContDump( nPrvSize );
                    }
                }
                if( rIn.GetRecLeft() > 0 )
                {
                    LINESTART(); ADDTEXT( "unknown data:" ); PRINT();
                    ContDump( rIn.GetRecLeft() );
                }
            }
            break;
            case 0x51:
            {
                UINT16  nR1, nR2;
                UINT8   nC1, nC2;
                rIn >> nR1 >> nR2 >> nC1 >> nC2;
                lcl_AddRangeRef( t, nC1, nR1, nC2, nR2 );
                PRINT();
                LINESTART();
                ADDTEXT( "workbook: " );
                AddUNICODEString( t, rIn, TRUE );
                PRINT();
            }
            break;
            case 0x0052:    // DCONNAME
            {
                ADDTEXT( "name=" );
                AddUNICODEString( t, rIn, TRUE );
                PRINT();
                LINESTART();
                ADDTEXT( "sheet=" );
                AddUNICODEString( t, rIn, TRUE );
                PRINT();
            }
            break;
            case 0x5B:          // FILESHARING
            {
                PreDump( nL );
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fReadOnlyRec" );
                    PRINT();
                }
                DumpValidPassword( rIn, pPre );
            }
            break;
            case 0x5D:
                ObjDump( nL );
                break;
            case 0x007D:        // COLINFO - col range info
            {
                LINESTART();
                ADDTEXT( "col range: " );
                ADDDEC( 2 );
                ADDTEXT( "-" );
                ADDDEC( 2 );
                ADDTEXT( "   width: " );
                ADDDEC( 2 );
                ADDTEXT( "/256 charwidth   ix to XF: " );
                ADDDEC( 2 );
                PRINT();
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, "fHidden" );
                ADDTEXT( " outlnlev=" );
                __AddDec( t, (UINT16)((__nFlags & 0x0700) >> 8) );
                ADDFLAG( 0x1000, "fCollapsed" );
                ADDRESERVED( 0xE8FE );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved: " );
                ADDHEX( 1 );
                PRINT();
            }
            break;
            case 0x7E:
            {
//              LINESTART();
                ADDCELLHEAD();
                ADDTEXT( "   val = " );
                __AddRK( t, rIn.ReadInt32() );
                PRINT();
            }
            break;
            case 0x0080:        // GUTS - row & col gutters for outlines
            {
                LINESTART();
                ADDTEXT( "size row gutter: " );
                __AddDec( t, Read2( rIn ), 5 );
                ADDTEXT( "   |   size col gutter: " );
                __AddDec( t, Read2( rIn ), 5 );
                PRINT();
                LINESTART();
                ADDTEXT( "max outline lev: " );
                __AddDec( t, Read2( rIn ), 5 );
                ADDTEXT( "   |   max outline lev: " );
                __AddDec( t, Read2( rIn ), 5 );
                PRINT();
            }
            break;
            case 0x0081:        // WSBOOL - additional workspace info
            {
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                if( __nFlags & 0x00F1 )
                {
                    ADDFLAG( 0x0001, "fShowAutoBreaks" );
                    ADDFLAG( 0x0010, "fDialog" );
                    ADDFLAG( 0x0020, "fApplyStyles" );
                    ADDFLAG( 0x0040, "fRowSumsBelow" );
                    ADDFLAG( 0x0080, "fColSumsBelow" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & (0xCD00 | 0x320E) )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x0100, "fFitToPage" );
                    ADDFLAG( 0x0400, "fDispRowGuts" );
                    ADDFLAG( 0x0800, "fDispColGuts" );
                    ADDFLAG( 0x4000, "fAee" );
                    ADDFLAG( 0x8000, "fAfe" );
                    ADDRESERVED( 0x320E );
                    PRINT();
                }
                if( !__nFlags )
                    PRINT();
            }
            break;
            case 0x008C:        // COUNTRY
            {
                LINESTART();
                ADDTEXT( "ui-country=" );           ADDDEC( 2 );
                ADDTEXT( "   doc-country=" );       ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x92:      // PALETTE
            {
                UINT16 nColCnt;
                rIn >> nColCnt;
                LINESTART();
                ADDTEXT( "count: " );
                __AddDec( t, nColCnt );
                PRINT();
                LINESTART();
                for( UINT16 nCol = 0; nCol < nColCnt; nCol++ )
                {
                    __AddDec( t, nCol, 2 );
                    ADDTEXT( "=" );
                    ADDHEX( 4 );
                    ADDTEXT( "   " );
                    if( (nCol % 5 == 4) || (nCol == nColCnt - 1) )
                    {
                        PRINT();
                        LINESTART();
                    }
                }
            }
            break;
            case 0x9D:      // AUTOFILTERINFO -- count of drop-down arrows
            {
                LINESTART();
                ADDTEXT( "count of drop-down arrows: " );
                ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x9E:      // AUTOFILTER -- autofilter settings
            {
                UINT8       nType;
                UINT8       nCompare;
                ByteString  sTemp[ 2 ];
                UINT16      nLen[ 2 ] = { 0, 0 };
                UINT8       nF;
                LINESTART();
                ADDTEXT( "count: " );
                ADDDEC( 2 );
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0003, "fJoin" );
                ADDFLAG( 0x0004, "fSimpleEq1" );
                ADDFLAG( 0x0008, "fSimpleEq2" );
                ADDFLAG( 0x0010, "fTop10" );
                ADDFLAG( 0x0020, "fTop" );
                ADDFLAG( 0x0040, "fPercent" );
                PRINT();
                LINESTART();
                if( __nFlags & 0x0003 )
                    ADDTEXT( "(custom conditions are OR-ed" );
                else
                    ADDTEXT( "(custom conditions are AND-ed" );
                if( __nFlags & 0x0010 )
                {
                    if( __nFlags & 0x0020 )
                        ADDTEXT( "; show top " );
                    else
                        ADDTEXT( "; show bottom " );
                    __AddDec( t, (UINT16)(__nFlags >> 7) );
                    if( __nFlags & 0x0040 )
                        ADDTEXT( " percent" );
                    else
                        ADDTEXT( " items" );
                }
                ADDTEXT( ")" );
                PRINT();
                for( nF = 0; nF < 2; nF++ )
                {
                    LINESTART();
                    __AddDec( t, (UINT16)(nF + 1) );
                    ADDTEXT( ". Filter: " );
                    rIn >> nType >> nCompare;
                    switch( nType )
                    {
                        case 0x00:  ADDTEXT( "not used " );         break;
                        case 0x02:  ADDTEXT( "RK " );               break;
                        case 0x04:  ADDTEXT( "double " );           break;
                        case 0x06:  ADDTEXT( "string " );           break;
                        case 0x08:  ADDTEXT( "bool/err " );         break;
                        case 0x0A:  ADDTEXT( "show nothing " );     break;
                        case 0x0C:  ADDTEXT( "all blanks " );       break;
                        case 0x0E:  ADDTEXT( "all non-blanks " );   break;
                        default:
                            ADDTEXT( "unknown (" );
                            __AddHex( t, nType );
                            ADDTEXT( ") " );
                    }
                    switch( nCompare )
                    {
                        case 0x01:  ADDTEXT( "< " );    break;
                        case 0x02:  ADDTEXT( "= " );    break;
                        case 0x03:  ADDTEXT( "<= " );   break;
                        case 0x04:  ADDTEXT( "> " );    break;
                        case 0x05:  ADDTEXT( "<> " );   break;
                        case 0x06:  ADDTEXT( ">= " );   break;
                        default:    if( nCompare ) __AddHex( t, nCompare );
                    }
                    sTemp[ nF ] = t;
                    switch( nType )
                    {
                        case 0x02:
                            __AddRK( sTemp[ nF ], rIn.ReadInt32() );
                            IGNORE( 4 );
                            break;
                        case 0x04:
                            __AddDouble( sTemp[ nF ], Read8( rIn ) );
                            break;
                        case 0x06:
                            IGNORE( 4 );
                            nLen[ nF ] = Read1( rIn );
                            IGNORE( 3 );
                            break;
                        case 0x08:
                            __AddHex( sTemp[ nF ], Read1( rIn ) );
                            sTemp[ nF ] += " ";
                            __AddHex( sTemp[ nF ], Read1( rIn ) );
                            IGNORE( 6 );
                            break;
                        default:
                            IGNORE( 8 );
                    }
                }
                for( nF = 0; nF < 2; nF++ )
                {
                    t = sTemp[ nF ];
                    if( nLen[ nF ] )
                        AddUNICODEString( t, rIn, TRUE, nLen[ nF ] );
                    PRINT();
                }
            }
            break;
            case 0xA0:
            {
                UINT16  nN, nD;
                rIn >> nN >> nD;
                LINESTART();
                ADDTEXT( "Window Zoom Magnification = " );
                __AddDec( t, nN );
                ADDTEXT( "/" );
                __AddDec( t, nD );
                PRINT();
            }
            break;
            case 0x00A1:    // SETUP
            {
                LINESTART();
                ADDTEXT( "paper size: " );          ADDDEC( 2 );
                ADDTEXT( "   scaling: " );          ADDDEC( 2 );
                ADDTEXT( "   start page: " );       ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "fit to width: " );        ADDDEC( 2 );
                ADDTEXT( "   fit to height: " );    ADDDEC( 2 );
                PRINT();
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0001, "fLeftRight" );
                ADDFLAG( 0x0002, "fPortrait" );
                ADDFLAG( 0x0004, "fNoPrintSettings" );
                ADDFLAG( 0x0008, "fMonochrom" );
                ADDFLAG( 0x0010, "fDraft" );
                ADDFLAG( 0x0020, "fNotes" );
                ADDFLAG( 0x0040, "fNoOrientation" );
                ADDFLAG( 0x0080, "fCustomNumber" );
                PRINT();
                LINESTART();
                ADDTEXT( "Print res: " );           ADDDEC( 2 );
                ADDTEXT( "   vert print res: " );   ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "header margin: " );       ADDDOUBLE();
                ADDTEXT( "   footer margin: " );    ADDDOUBLE();
                ADDTEXT( "   copies: " );           ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0xAF:
            {
                UINT16  nCref;
                UINT8   nLocked, nHidden, nName, nComment, nNameUser;
                rIn >> nCref >> nLocked >> nHidden >> nName >> nComment >> nNameUser;
                LINESTART();
                ADDTEXT( "Changing Cells = " );
                __AddDec( t, nCref );
                if( nLocked )
                    ADDTEXT( "  fLocked" );
                if( nHidden )
                    ADDTEXT( "  fHidden" );
                PRINT();
                LINESTART();
                ADDTEXT( "Name = " );
                __AddDec( t, nName );
                ADDTEXT( "    Comment = " );
                __AddDec( t, nComment );
                ADDTEXT( "    Name User = " );
                __AddDec( t, nNameUser );
                PRINT();
                ContDump( rIn.GetRecLeft() );
            }
            break;
            case 0xB0:      // SXVIEW
            {
                UINT16  nColFirst, nColLast, nRowFirst, nRowLast;
                rIn >> nRowFirst >> nRowLast >> nColFirst >> nColLast;
                nSXLIIndex = 0;     // new pivot table
                LINESTART();
                ADDTEXT( "PivotTable: " );          __AddDec( t, nColFirst );
                ADDTEXT( " / " );                   __AddDec( t, nRowFirst );
                ADDTEXT( " - " );                   __AddDec( t, nColLast );
                ADDTEXT( " / " );                   __AddDec( t, nRowLast );
                PRINT();
                LINESTART();
                ADDTEXT( "1st Head: " );            ADDDEC( 2 );
                rIn >> nRowFirst;
                ADDTEXT( "    First Data: " );      ADDDEC( 2 );
                ADDTEXT( " / " );                   __AddDec( t, nRowFirst );
                PRINT();
                LINESTART();
                ADDTEXT( "Cache index: " );         ADDDEC( 2 );
                ADDTEXT( "    reserved: " );        ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "axis 4 data: " );         ADDDEC( 2 );
                ADDTEXT( "     pos 4 Data: " );     ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "num of fields: " );       ADDDEC( 2 );
                rIn >> nSXLISize[0] >> nSXLISize[1];
                ADDTEXT( "     ...row fields: " );  __AddDec( t, nSXLISize[0] );
                ADDTEXT( "     ...col fields: " );  __AddDec( t, nSXLISize[1] );
                PRINT();
                LINESTART();
                ADDTEXT( "num of page fields: " );  ADDDEC( 2 );
                ADDTEXT( "     ...data fields: " ); ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "data rows: " );           ADDDEC( 2 );
                ADDTEXT( "     data cols: " );      ADDDEC( 2 );
                rIn >> __nFlags;
                PRINT();
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fRowGrand" );
                    ADDFLAG( 0x0002, "fColGrand" );
                    ADDFLAG( 0x0008, "fAutoFormat" );
                    ADDFLAG( 0x0010, "fWidthHeightAuto" );
                    ADDFLAG( 0x0020, "fFontAuto" );
                    ADDFLAG( 0x0040, "fAlignAuto" );
                    ADDFLAG( 0x0080, "fBorderAuto" );
                    ADDFLAG( 0x0100, "fPatternAuto" );
                    ADDFLAG( 0x0200, "fNumberAuto" );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "index 2 PivotTable autoform: " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                UINT16  nTableLen = Read2( rIn );
                UINT16  nDataLen = Read2( rIn );
                ADDTEXT( "PivotTable name: " );
                if( nTableLen )
                    AddUNICODEString( t, rIn, TRUE, nTableLen );
                else
                    ADDTEXT( "-/-" );
                PRINT();
                LINESTART();
                ADDTEXT( "data field name: " );
                if( nDataLen )
                    AddUNICODEString( t, rIn, TRUE, nDataLen );
                else
                    ADDTEXT( "-/-" );
                PRINT();
            }
            break;
            case 0xB1:                                                          // SXVD
            {
                rIn >> __nFlags;
                LINESTART();
                ADDTEXT( "Axis (" );
                __AddHex( t, __nFlags );
                ADDTEXT( "):" );
                if( __nFlags )
                {
                    ADDFLAG( 0x0001, "row" );
                    ADDFLAG( 0x0002, "col" );
                    ADDFLAG( 0x0004, "page" );
                    ADDFLAG( 0x0008, "data" );
                }
                else
                    ADDTEXT( " no axis" );
                ADDTEXT( "   num of att. subtotals: " );
                ADDDEC( 2 );
                PRINT();
                rIn >> __nFlags;
                LINESTART();
                ADDTEXT( "subtotal type(" );
                __AddHex( t, __nFlags );
                ADDTEXT( "):" );
                if( __nFlags )
                {
                    ADDFLAG( 0x0001, "Default" );
                    ADDFLAG( 0x0002, "Sum" );
                    ADDFLAG( 0x0004, "Counta" );
                    ADDFLAG( 0x0008, "Average" );
                    ADDFLAG( 0x0010, "Max" );
                    ADDFLAG( 0x0020, "Min" );
                    ADDFLAG( 0x0040, "Product" );
                    ADDFLAG( 0x0080, "Count" );
                    ADDFLAG( 0x0100, "Stdev" );
                    ADDFLAG( 0x0200, "Stddevp" );
                    ADDFLAG( 0x0400, "Var" );
                    ADDFLAG( 0x0800, "Varp" );
                }
                else
                    ADDTEXT( " none" );
                PRINT();
                LINESTART();
                ADDTEXT( "num of items: " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "Name: " );
                UINT16      nLen = Read2( rIn );
                if( nLen == 0xFFFF )
                    ADDTEXT( "<name in cache>" );
                else if( nLen )
                    AddUNICODEString( t, rIn, TRUE, nLen );
                PRINT();
            }
            break;
            case 0xB2:                                                          // SXVI
            {
                UINT16  nType, nCache;
                rIn >> nType >> __nFlags >> nCache;
                LINESTART();
                switch( nType )
                {
                    case 0xFE:  p = "Page";         break;
                    case 0xFF:  p = "Null";         break;
                    case 0x00:  p = "Data";         break;
                    case 0x01:  p = "Default";      break;
                    case 0x02:  p = "SUM";          break;
                    case 0x03:  p = "COUNT";        break;
                    case 0x04:  p = "AVERAGE";      break;
                    case 0x05:  p = "MAX";          break;
                    case 0x06:  p = "MIN";          break;
                    case 0x07:  p = "PRODUCT";      break;
                    case 0x08:  p = "COUNTA";       break;
                    case 0x09:  p = "STDEV";        break;
                    case 0x0A:  p = "STDEVP";       break;
                    case 0x0B:  p = "VAR";          break;
                    case 0x0C:  p = "VARP";         break;
                    case 0x0D:  p = "Grand total";  break;
                    default:    p = pU;
                }
                ADDTEXT( "Type (" );
                __AddHex( t, nType );
                ADDTEXT( "): " );
                ADDTEXT( p );
                ADDTEXT( "    iCache: " );
                __AddDec( t, nCache );
                PRINT();
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x01, "fHidden" );
                    ADDFLAG( 0x02, "fHideDetail" );
                    ADDFLAG( 0x04, "fFormula" );
                    ADDFLAG( 0x08, "fMissing" );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "Name: " );
                UINT16  nCch = Read2( rIn );
                if( nCch == 0xFFFF )
                    ADDTEXT( "<name in cache>" );
                else if( nCch )
                    AddUNICODEString( t, rIn, TRUE, nCch );
                else
                    ADDTEXT( "<empty string>" );
                PRINT();
            }
            break;
            case 0xB4:                                                          // SXIVD
            {
                const UINT16    nBrkNum = 5;
                UINT16          nBrk = nBrkNum;
                UINT16          nSize = (UINT16)(nL / 2);
                LINESTART();
                for( UINT16 i = 0; i < nSize; i++ )
                {
                    ADDHEX( 2 );
                    nBrk--;
                    if( nBrk )
                        ADDTEXT( "  " );
                    else
                    {
                        PRINT();
                        LINESTART();
                        nBrk = nBrkNum;
                    }
                }
                if( nBrk < nBrkNum ) PRINT();
            }
            break;
            case 0xB5:                                      // SXLI - pivot table line item array
            {
                UINT16              nIdent;
                UINT16              nType;
                UINT16              nMaxInd;
                const sal_Char*     pInd = "   ";
                const sal_Char*     pType[] = {
                    "Data", "Default", "SUM", "COUNT","AVERAGE",
                    "MAX", "MIN", "PRODUCT", "COUNTA", "STDEV",
                    "STDEVP", "VAR", "VARP", "Grand total",
                    "Blank line" };                         // undocumented
                while( rIn.GetRecLeft() > 0 )
                {
                    rIn >> nIdent >> nType >> nMaxInd >> __nFlags;
                    LINESTART();
                    ADDTEXT( "# of ident. items: " );
                    __AddDec( t, nIdent );
                    ADDTEXT( "   Type (" );
                    __AddHex( t, nType );
                    ADDTEXT( "): " );
                    if( nType > 0x0E )
                        p = pU;
                    else
                        p = pType[ nType ];
                    ADDTEXT( p );
                    ADDTEXT( "   relevant indexes: " );
                    __AddDec( t, nMaxInd );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pInd );
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fMultiDataName" );
                    ADDFLAG( 0x0200, "fSub" );
                    ADDFLAG( 0x0400, "fBlock" );
                    ADDFLAG( 0x0800, "fGrand" );
                    ADDFLAG( 0x1000, "fMultiDataOnAxis" );
                    ADDFLAG( 0x2000, "fBlankLine" );        // undocumented
                    ADDFLAG( 0x4000, "fHideDetail" );       // undocumented
                    ADDRESERVED( 0x8000 );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pInd );
                    ADDTEXT( "index to data field: " );
                    __AddDec( t, (UINT16) ( (__nFlags & 0x01FE) >> 1 ) );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pInd );
                    ADDTEXT( "array of " );
                    __AddDec( t, nSXLISize[nSXLIIndex] );
                    ADDTEXT( " indexes (^ are ident., * are irrelevant):" );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pInd );
                    ADDTEXT( "  " );
                    const UINT16    nBrkNum = 5;
                    UINT16          nBrk = nBrkNum;
                    for( UINT16 i = 0; i < nSXLISize[nSXLIIndex]; i++ )
                    {
                        __AddDec( t, Read2( rIn ), 7 );
                        if( i < nIdent )
                            ADDTEXT( "^" );
                        else if( i < nMaxInd )
                            ADDTEXT( " " );
                        else
                            ADDTEXT( "*" );
                        nBrk--;
                        if( !nBrk )
                        {
                            PRINT();
                            LINESTART();
                            ADDTEXT( pInd );
                            ADDTEXT( "  " );
                            nBrk = nBrkNum;
                        }
                    }
                    if( nBrk < nBrkNum )
                        PRINT();
                }
                nSXLIIndex = 1 - nSXLIIndex;
            }
            break;
            case 0xB6:      // SXPI - pivot table page item(s)
            {
                UINT16  nArrayCnt = (UINT16)(nL / 6);
                LINESTART();
                __AddDec( t, nArrayCnt );
                ADDTEXT( " page items:" );
                PRINT();
                for( UINT16 iArray = 0; iArray < nArrayCnt; iArray++ )
                {
                    LINESTART();
                    ADDTEXT( "index SXVD: " );
                    __AddDec( t, Read2( rIn ), 3 );
                    ADDTEXT( "   index SXVI: " );
                    UINT16 nSXVIInd;
                    rIn >> nSXVIInd;
                    __AddDec( t, nSXVIInd, 5 );
                    if( nSXVIInd == 32765 )
                        ADDTEXT( " (All items)   Obj ID: " );
                    else
                        ADDTEXT( "               Obj ID: " );
                    ADDHEX( 2 );
                    PRINT();
                }
            }
            break;
            case 0xBD:
            {
                UINT16  nC, nR, nXF;
                INT32 nRK;
                UINT16  n = (UINT16)((nL - 4) / 6);

                rIn >> nR >> nC;
                while( n )
                {
                    rIn >> nXF >> nRK;
                    LINESTART();
                    __AddCellHead( t, nC, nR, nXF );
                    ADDTEXT( "   val = " );
                    __AddRK( t, nRK );
                    PRINT();
                    nC++;
                    n--;
                }
            }
            break;
            case 0xBE:
            {
                LINESTART();
                ADDCELLHEAD();
                PRINT();
                LINESTART();
                ADDTEXT( "next XFs: " );
                UINT16  n = (UINT16)((nL - 6) / 2);
                while( n )
                {
                    __AddDec( t, Read2( rIn ) );
                    n--;
                    if( n )
                        ADDTEXT( ' ' );
                }
                PRINT();
            }
            break;
            case 0x00C5:        // SXDI
            {
                LINESTART();
                ADDTEXT( "Field: " );
                ADDDEC( 2 );
                UINT16  nFunc = Read2( rIn );
                ADDTEXT( "   aggregation func (" );
                __AddHex( t, nFunc );
                ADDTEXT( "): " );
                const sal_Char*     pFunc[] = { "Sum", "Count", "Average", "Max", "Min",
                                            "Product", "Count Nums", "StdDev", "StdDevp", "Var",
                                            "Varp" };
                p = (nFunc > 0x0A) ? pU : pFunc[ nFunc ];
                ADDTEXT( p );
                ADDTEXT( "   display format (" );
                const sal_Char*     pDispl[] = {
                    "Normal", "Difference", "Percentage of", "Percentage difference from", "Running total in",
                    "Percentage of row", "Percentage of column", "Percentage of total", "Index" };
                UINT16  nDispl = Read2( rIn );
                __AddHex( t, nDispl );
                ADDTEXT( "): " );
                p = (nDispl > 0x08) ? pU : pDispl[ nDispl ];
                ADDTEXT( p );
                PRINT();
                LINESTART();
                ADDTEXT( "ind. to SXVD: " );        ADDDEC( 2 );
                ADDTEXT( "   ind. to SXVI: " );     ADDDEC( 2 );
                ADDTEXT( "   num format: " );       ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "name: " );
                UINT16  nCch = Read2( rIn );
                if( nCch == 0xFFFF )
                    ADDTEXT( "<name in cache>" );
                else if( nCch )
                    AddUNICODEString( t, rIn, TRUE, nCch );
                else
                    ADDTEXT( "<empty string>" );
                PRINT();
            }
            break;
            case 0x00C6:        // SXDB - cache info
            {
                ADDTEXT( "number of recs: " );  ADDDEC( 4 );
                ADDTEXT( "   stream id: " );    ADDHEX( 2 );
                ADDTEXT( "   flags: " );        ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "DB block recs: " );   ADDDEC( 2 );
                ADDTEXT( "   base fields: " );  ADDDEC( 2 );
                ADDTEXT( "   all fields: " );   ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved: " );        ADDHEX( 2 );
                ADDTEXT( "   type: " );         ADDHEX( 2 );
                ADDTEXT( "   changed by:" );
                PRINT();
                LINESTART();
                AddUNICODEString( t, rIn );
                PRINT();
            }
            break;
            case 0x00C7:        // SXFIELD - Pivot Field
            {
                nItemCnt = 0;

                ADDTEXT( "#" );
                __AddDec( t, nFieldCnt, 3 );
                nFieldCnt++;
                ADDTEXT( " (pivot field): " );
                if( rIn.GetRecLeft() < 14 )
                {
                    ADDTEXT( "<break in pivot field start>" );
                    PRINT();
                }
                else
                {
                    PRINT();
                    LINESTART();
                    ADDTEXT( pPre );
                    rIn >> __nFlags;
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fOrigItems" );
                    ADDFLAG( 0x0002, "fPostponed" );
                    ADDFLAG( 0x0004, "fCalculated" );
                    ADDFLAG( 0x0008, "fGroupChild" );
                    ADDFLAG( 0x0010, "fNumGroup" );
                    ADDFLAG( 0x0200, "fLongIndex" );
                    ADDFLAG( 0x1000, "f1000?" );
                    ADDFLAG( 0x8000, "f8000?" );
                    ADDRESERVED( 0x6000 );
                    ADDTEXT( "   data-type=" );
                    __AddHex( t, static_cast< sal_uInt16 >( __nFlags & 0x0DE0 ) );
                    ADDTEXT( "=" );
                    switch( __nFlags & 0x0DE0 )
                    {
                        case 0x0000:    ADDTEXT( "spc" );               break;
                        case 0x0480:    ADDTEXT( "str" );               break;
                        case 0x0520:    ADDTEXT( "int[+dbl]" );         break;
                        case 0x0560:    ADDTEXT( "dbl" );               break;
                        case 0x05A0:    ADDTEXT( "str+int[+dbl]" );     break;
                        case 0x05E0:    ADDTEXT( "str+dbl" );           break;
                        case 0x0900:    ADDTEXT( "dat" );               break;
                        case 0x0D00:    ADDTEXT( "dat+int/dbl" );       break;
                        case 0x0D80:    ADDTEXT( "dat+str[+int/dbl]" ); break;
                        default:        ADDTEXT( pU );
                    }
                    PRINT();
                    LINESTART();
                    ADDTEXT( pPre );
                    ADDTEXT( "group-subfield=" );           ADDDEC( 2 );
                    ADDTEXT( "   group-basefield=" );       ADDDEC( 2 );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pPre );
                    ADDTEXT( "item-count=" );               ADDDEC( 2 );
                    ADDTEXT( "   group-item-count=" );      ADDDEC( 2 );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pPre );
                    ADDTEXT( "base-item-count=" );          ADDDEC( 2 );
                    ADDTEXT( "   source-item-count=" );     ADDDEC( 2 );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pPre );
                    if( rIn.GetRecLeft() < 3 )
                    {
                        ADDTEXT( "<break in pivot field name>" );
                        PRINT();
                    }
                    else
                    {
                        ADDTEXT( "name=" );
                        AddUNICODEString( t, rIn );
                        PRINT();
                    }
                }
            }
            break;
            case 0x00C8:        // SXINDEXLIST - indexes to source data
            {
                ADDTEXT( "#" );
                __AddDec( t, nTabIndexCnt, 3 );
                nTabIndexCnt++;
                ADDTEXT( " (index list):" );
                for( UINT16 iIndex = 0; iIndex < rIn.GetRecSize(); iIndex++ )
                {
                    ADDTEXT( " " );
                    ADDHEX( 1 );
                }
                PRINT();
            }
                break;
            case 0x00C9:        // SXDOUBLE - cache entry: double value
            {
                ADDTEXT( "#" );
                __AddDec( t, nItemCnt, 3 );
                ADDTEXT( " (double): " );
                nItemCnt++;
                ADDTEXT( "  " );
                ADDDOUBLE();
                PRINT();
            }
                break;
            case 0x00CA:        // SXBOOLEAN - cache entry: boolean value
            {
                ADDTEXT( "#" );
                __AddDec( t, nItemCnt, 3 );
                ADDTEXT( " (boolean): " );
                nItemCnt++;
                ADDTEXT( "  " );
                lcl_AddFlag( t, rIn.ReaduInt16() != 0 );
                PRINT();
            }
            break;
            case 0x00CB:        // SXERROR - cache entry: error code
            {
                ADDTEXT( "#" );
                __AddDec( t, nItemCnt, 3 );
                ADDTEXT( " (error): " );
                nItemCnt++;
                ADDTEXT( "  " );
                ADDHEX( 2 );
                PRINT();
            }
            break;
            case 0x00CC:        // SXINTEGER - signed 16bit integer
            {
                ADDTEXT( "#" );
                __AddDec( t, nItemCnt, 3 );
                ADDTEXT( " (integer): " );
                nItemCnt++;
                ADDTEXT( "  " );
                ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x00CD:        // SXSTRING - String
            {
                if( bSubStream )
                {
                    ADDTEXT( "#" );
                    __AddDec( t, nItemCnt, 3 );
                    ADDTEXT( " (string): " );
                    nItemCnt++;
                }
                AddUNICODEString( t, rIn );
                PRINT();
            }
                break;
            case 0x00CE:        // SXDATETIME - date & time special format
            {
                ADDTEXT( "#" );
                __AddDec( t, nItemCnt, 3 );
                ADDTEXT( " (date/time): " );
                nItemCnt++;
                UINT8   nDay, nHour, nMin, nSec;
                UINT16  nYear, nMonth;
                rIn >> nYear >> nMonth >> nDay >> nHour >> nMin >> nSec;
                if( nDay )
                {
                    __AddDec( t, nDay );
                    ADDTEXT( "." );
                    __AddDec( t, nMonth );
                    ADDTEXT( "." );
                    __AddDec( t, nYear );
                    ADDTEXT( " " );
                }
                __AddDec( t, nHour, 2, '0' );
                ADDTEXT( ":" );
                __AddDec( t, nMin, 2, '0' );
                ADDTEXT( ":" );
                __AddDec( t, nSec, 2, '0' );
                PRINT();
            }
            break;
            case 0x00CF:        // SXEMPTY - cache entry: empty
            {
                ADDTEXT( "#" );
                __AddDec( t, nItemCnt, 3 );
                ADDTEXT( " (empty): " );
                nItemCnt++;
                PRINT();
            }
            break;
            case 0x00D5:        // SXIDSTM - pivot table cache stream id
            {
                LINESTART();
                UINT16      nStrId = Read2( rIn );
                ADDTEXT( "Stream ID: " );
                __AddHex( t, nStrId );
                PRINT();
                DumpRecordStream( OpenStorage( EXC_STORAGE_PTCACHE ), ScfTools::GetHexStr( nStrId ), EMPTY_STRING );
            }
            break;
            case 0x00D8:        // SXNUMGROUP - numerical grouping in pivot cache field
            {
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0001, "fAutoMin" );
                ADDFLAG( 0x0002, "fAutoMax" );
                ADDTEXT( "   data-type=" );
                switch( (__nFlags & 0x003C) >> 2 )
                {
                    case 0x0001:    ADDTEXT( "seconds" );   break;
                    case 0x0002:    ADDTEXT( "minutes" );   break;
                    case 0x0003:    ADDTEXT( "hours" );     break;
                    case 0x0004:    ADDTEXT( "days" );      break;
                    case 0x0005:    ADDTEXT( "months" );    break;
                    case 0x0006:    ADDTEXT( "quarters" );  break;
                    case 0x0007:    ADDTEXT( "years" );     break;
                    case 0x0008:    ADDTEXT( "numeric" );   break;
                    default:        ADDTEXT( pU );
                }
                (__nFlags &= 0xFFC0) >>= 6;
                ADDTEXT( "   remaining=" );         __AddHex( t, __nFlags );
                ADDTEXT( "=" );                     __AddDec( t, __nFlags );
                PRINT();
            }
            break;
            case 0xE0:
                DumpXF( rIn, pPre );
                break;
            case 0xE3:
            {
                LINESTART();
                ADDTEXT( "view source (" );
                UINT16  n = Read2( rIn );
                __AddHex( t, n );
                ADDTEXT( "): " );
                switch( n )
                {
                    case 0x01:  p = "M$ Excel list or database";        break;
                    case 0x02:  p = "external data source";             break;
                    case 0x04:  p = "multiple consolidation ranges";    break;
                    case 0x08:  p = "pivot table";                      break;
                    case 0x10:  p = "scenario manager summary report";  break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                PRINT();
            }
            break;
            case 0x00E5:        // CELLMERGING
            {
                UINT16 nCount, nInd;
                UINT16 nRow1, nRow2, nCol1, nCol2;
                rIn >> nCount;
                LINESTART();
                ADDTEXT( "Count: " );
                __AddDec( t, nCount );
                PRINT();
                LINESTART();
                for( nInd = 0; nInd < 3; nInd++ )
                    ADDTEXT( " row - row  / col-col | " );
                PRINT();
                LINESTART();
                if( (ULONG)(nCount * 8 + 2) == nL )
                {
                    for( nInd = 0; nInd < nCount; nInd++ )
                    {
                        rIn >> nRow1 >> nRow2 >> nCol1 >> nCol2;
                        __AddDec( t, nRow1, 5 );
                        ADDTEXT( "-" );
                        __AddDec( t, nRow2, 5 );
                        ADDTEXT( " / " );
                        __AddDec( t, nCol1, 3 );
                        ADDTEXT( "-" );
                        __AddDec( t, nCol2, 3 );
                        ADDTEXT( " | " );
                        if( (nInd % 3 == 2) || (nInd == nCount - 1) )
                        {
                            PRINT();
                            LINESTART();
                        }
                    }
                }
                else
                {
                    LINESTART();
                    ADDTEXT( "<Wrong record length!>" );
                    PRINT();
                }
            }
            break;
            case 0xEB:
            case 0xEC:
            case 0xED:
                EscherDump( nL, true );
            break;
            case 0x00F6:        // SXNAME
            {
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0002, "fNameErr" );
                ADDRESERVED( 0xFFFD );
                ADDTEXT( "   field=" );             ADDDEC( 2 );
                PRINT();
                LINESTART();
                sal_Int16 nFunc;
                rIn >> nFunc;
                ADDTEXT( "function=" );          __AddHex( t, (INT32)nFunc );
                static const sal_Char* const ppcFuncs[] = {
                    "none", 0, "sum", "counta", "count", "average", "max", "min",
                    "product", "stdev", "stdevp", "var", "varp" };
                lcl_AddEnum( t, nFunc, ppcFuncs, STATIC_TABLE_SIZE( ppcFuncs ), 0, -1 );
                ADDTEXT( "   SXPAIR-count=" );      ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x00F9:        // SXFMLA
            {
                LINESTART();
                sal_uInt16 nSize;
                rIn >> nSize;
                ADDTEXT( "formula-size=" );         __AddDec( t, nSize );
                ADDTEXT( "   SXNAME-count=" );      ADDDEC( 2 );
                PRINT();
                FormulaDump( nSize, FT_RangeName );
            }
            break;
            case 0xFC:
            {
                UINT16  nCnt = 0;
                BOOL    bOK = TRUE;
                ContDump( 8 );

                while( bOK && (rIn.GetRecLeft() > 0) )
                {
                    LINESTART();
                    __AddDec( t, nCnt );
                    ADDTEXT( ": " );
                    bOK = AddUNICODEString( t, rIn );
                    PRINT();
                    nCnt++;
                }
            }
            break;
            case 0xFD:
            {
                LINESTART();
                ADDCELLHEAD();
                ADDTEXT( "  sst = " );
                ADDDEC(4);
                PRINT();
            }
            break;
            case 0x0100:        // SXVDEX
            {
                LINESTART();
                sal_uInt32 __nFlags = Read4( rIn );
                STARTFLAG();
                if( __nFlags & 0x0000009F )
                {
                    ADDFLAG( 0x00000001, "fShowAllItems" );
                    ADDFLAG( 0x00000002, "fDragToRow" );
                    ADDFLAG( 0x00000004, "fDragToColumn" );
                    ADDFLAG( 0x00000008, "fDragToPage" );
                    ADDFLAG( 0x00000010, "fDragToHide" );
                    ADDFLAG( 0x00000080, "fServerBased" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0x00007E00 )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x00000200, "fAutoSort" );
                    ADDFLAG( 0x00000400, "fAscendSort" );
                    ADDFLAG( 0x00000800, "fAutoShow" );
                    ADDFLAG( 0x00001000, "fAscendShow" );
                    ADDFLAG( 0x00002000, "fCalculatedField" );
                    ADDFLAG( 0x00004000, "fLONewPage" );            // undocumented
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0xFFE00000 )
                {
                    ADDTEXT( "   " );                               // Layout flags:
                    ADDFLAG( 0x00200000, "fLOReport" );             // undocumented
                    ADDFLAG( 0x00400000, "fLOBlankLine" );          // undocumented
                    ADDFLAG( 0x00800000, "fLOSubTotalTop" );        // undocumented
                    ADDTEXT( " show-items=" );  __AddDec( t, sal_uInt32( __nFlags >> 24 ) );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0x001F8160 )
                {
                    ADDTEXT( "    !RESERVED!" );
                    PRINT();
                }
                if( !__nFlags )
                    PRINT();
                LINESTART();
                ADDTEXT( "   sort-field=" );
                ADDDEC( 2 );
                ADDTEXT( "   show-field=" );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "format=" );
                UINT16  n = Read2( rIn );
                if( n )
                    __AddDec( t, n );
                else
                    ADDTEXT( "none" );
                PRINT();
            }
            break;
            case 0x0122:        // SXDBEX - ext. cache info
            {
                ADDTEXT( "last changed: " );        ADDDOUBLE();
                ADDTEXT( "   SXFORMULA recs: " );   ADDDEC( 4 );
                PRINT();
            }
            break;
            case 0x0138:        // CHTRINFO - change tracking info
            {
                rIn.DisableDecryption();
                ADDTEXT( "14 bytes of unknown data..." );
                PRINT();
                ContDump( 14 );
                LINESTART();
                ADDTEXT( "16 bytes unknown identification:" );
                PRINT();
                ContDump( 16 );
                LINESTART();
                ADDTEXT( "unknown: " );         ADDHEX( 2 );
                ADDTEXT( "   user: " );
                if( rIn.GetRecLeft() > 3 )
                    AddUNICODEString( t, rIn );
                PRINT();
                LINESTART();
                __AddDec( t, (UINT16)(rIn.GetRecLeft() - 10) );
                ADDTEXT( " bytes of unknown data..." );
                PRINT();
                ContDump( rIn.GetRecLeft() - 10 );
                LINESTART();
                ADDTEXT( "date/time: " );       ADDDEC( 2 );
                ADDTEXT( "-" );                 ADDDEC( 1 );
                ADDTEXT( "-" );                 ADDDEC( 1 );
                ADDTEXT( " " );                 ADDDEC( 1 );
                ADDTEXT( ":" );                 ADDDEC( 1 );
                ADDTEXT( ":" );                 ADDDEC( 1 );
                ADDTEXT( "   unknown: " );      ADDHEX( 1 );
                ADDTEXT( " " );                 ADDHEX( 2 );
                PRINT();
            }
            break;
            case 0x0137:        // CHTRINSERT - change tracking: insert/remove
            {
                ADDTEXT( "len: " );             ADDDEC( 4 );
                ADDTEXT( "   index: " );        ADDDEC( 4 );
                ADDTEXT( "   op: " );
                UINT16 nOp;
                rIn >> nOp;
                switch( nOp )
                {
                    case 0x0000:    ADDTEXT( "insert row" );    break;
                    case 0x0001:    ADDTEXT( "insert column" ); break;
                    case 0x0002:    ADDTEXT( "delete row" );    break;
                    case 0x0003:    ADDTEXT( "delete column" ); break;
                    default:
                        __AddHex( t, nOp );
                        ADDTEXT( " *UNKNOWN*" );
                }
                ADDTEXT( "   accept: " );       ADDHEX( 2 );
                ADDTEXT( "   tab: " );          ADDDEC( 2 );
                PRINT();
                LINESTART();
                UINT16 __nFlags = Read2( rIn );
                STARTFLAG();
                ADDFLAG( 0x0001, "fAuto" );
                ADDRESERVED( 0xFFFE );
                UINT16 nCol1, nRow1, nCol2, nRow2;
                rIn >> nRow1 >> nRow2 >> nCol1 >> nCol2;
                ADDTEXT( "   range: " );        lcl_AddRangeRef( t, nCol1, nRow1, nCol2, nRow2 );
                ADDTEXT( "   unknown: " );      ADDHEX( 4 );
                PRINT();
            }
            break;
            case 0x013B:        // CHTRCELLCONTENT: change tracking: changed cell
            {
                PreDump( nL );
                ADDTEXT( "len: " );             ADDDEC( 4 );
                ADDTEXT( "   index: " );        ADDDEC( 4 );
                ADDTEXT( "   opcode: " );       ADDHEX( 2 );
                ADDTEXT( "   accept: " );       ADDHEX( 2 );
                ADDTEXT( "   tab: " );          ADDDEC( 2 );
                PRINT();
                LINESTART();
                UINT16 nChg, nOldType, nNewType;
                rIn >> nChg;
                nOldType = (nChg & 0x0038) >> 3;
                nNewType = nChg & 0x0007;
                ADDTEXT( "change (" );          __AddHex( t, nChg );
                ADDTEXT( "): " );
                switch( nOldType )
                {
                    case 0x0000:    ADDTEXT( "empty->" );   break;
                    case 0x0001:    ADDTEXT( "RK->" );      break;
                    case 0x0002:    ADDTEXT( "double->" );  break;
                    case 0x0003:    ADDTEXT( "string->" );  break;
                    case 0x0004:    ADDTEXT( "bool->" );    break;
                    case 0x0005:    ADDTEXT( "formula->" ); break;
                    default:        ADDTEXT( "*UNKNOWN*->" );
                }
                switch( nNewType )
                {
                    case 0x0000:    ADDTEXT( "empty" );     break;
                    case 0x0001:    ADDTEXT( "RK" );        break;
                    case 0x0002:    ADDTEXT( "double" );    break;
                    case 0x0003:    ADDTEXT( "string" );    break;
                    case 0x0004:    ADDTEXT( "bool" );      break;
                    case 0x0005:    ADDTEXT( "formula" );   break;
                    default:        ADDTEXT( "*UNKNOWN*" );
                }
                UINT16 nFormatData = (nChg & 0xFF00);
                if( (nFormatData == 0x1100) || (nFormatData == 0x1300) )
                    ADDTEXT( "; contains add. data" );
                ADDTEXT( "   format: " );       ADDHEX( 2 );
                UINT16 nCol, nRow;
                rIn >> nRow >> nCol;
                ADDTEXT( "   address: " );      lcl_AddRef( t, nCol, nRow );
                PRINT();
                LINESTART();
                UINT16 nOldLen;
                rIn >> nOldLen;
                ADDTEXT( "old value len: " );   __AddHex( t, nOldLen );
                if( nOldType == 0x0003 )
                    nOldLen >>= 1;
                ADDTEXT( "   unknown: " );      ADDHEX( 4 );
                PRINT();
                UINT16 nCount = 0;
                switch( nFormatData )
                {
                    case 0x1100:    nCount = 8; break;
                    case 0x1300:    nCount = 4; break;
                }
                if( nCount )
                {
                    LINESTART();
                    ADDTEXT( "additional format data:" );
                    for( UINT16 nIndex = 0; nIndex < nCount; nIndex ++ )
                    {
                        ADDTEXT( " " );
                        ADDHEX( 2 );
                    }
                    PRINT();
                }
                if( nOldType )
                {
                    LINESTART();
                    ADDTEXT( "old value: " );
                    switch( nOldType )
                    {
                        case 0x0001:
                            __AddRK( t, rIn.ReadInt32() );
                            PRINT();
                        break;
                        case 0x0002:
                            ADDDOUBLE();
                            PRINT();
                        break;
                        case 0x0003:
                            AddUNICODEString( t, rIn );
                            PRINT();
                        break;
                        case 0x0004:
                            if( Read2( rIn ) )
                                ADDTEXT( "true" );
                            else
                                ADDTEXT( "false" );
                            PRINT();
                        break;
                        case 0x0005:
                        {
                            PRINT();
                            UINT16 nLen;
                            rIn >> nLen;
                            FormulaDump( nLen, FT_CellFormula );
                            IGNORE( 1 );
                        }
                        break;
                    }
                }
                if( nNewType )
                {
                    LINESTART();
                    ADDTEXT( "new value: " );
                    switch( nNewType )
                    {
                        case 0x0001:
                            __AddRK( t, rIn.ReadInt32() );
                            PRINT();
                        break;
                        case 0x0002:
                            ADDDOUBLE();
                            PRINT();
                        break;
                        case 0x0003:
                            AddUNICODEString( t, rIn );
                            PRINT();
                        break;
                        case 0x0004:
                            if( Read2( rIn ) )
                                ADDTEXT( "true" );
                            else
                                ADDTEXT( "false" );
                            PRINT();
                        break;
                        case 0x0005:
                        {
                            PRINT();
                            UINT16 nLen;
                            rIn >> nLen;
                            FormulaDump( nLen, FT_CellFormula );
                            IGNORE( 1 );
                        }
                        break;
                    }
                }
                if( rIn.GetRecLeft() > 0 )
                {
                    LINESTART();
                    ADDTEXT( "*UNKNOWN* data:" );
                    PRINT();
                    PreDump( rIn.GetRecLeft() );
                }
            }
            break;
            case 0x013D:        // TABID
            {
                ADDTEXT( "tab ids:" );
                while( rIn.GetRecLeft() )
                {
                    ADDTEXT( " " );
                    ADDDEC( 2 );
                }
                PRINT();
            }
            break;
            case 0x0140:        // CHTRMOVE - change tracking: moved range
            {
                ADDTEXT( "len: " );             ADDDEC( 4 );
                ADDTEXT( "   index: " );        ADDDEC( 4 );
                ADDTEXT( "   opcode: " );       ADDHEX( 2 );
                ADDTEXT( "   accept: " );       ADDHEX( 2 );
                PRINT();
                UINT16 nTab1, nTab2;
                UINT16 nCol11, nCol12, nCol21, nCol22;
                UINT16 nRow11, nRow12, nRow21, nRow22;
                rIn >> nTab2 >> nRow11 >> nRow12 >> nCol11 >> nCol12 >> nRow21 >> nRow22 >> nCol21 >> nCol22 >> nTab1;
                LINESTART();
                ADDTEXT( "move range from: tab=" );     __AddDec( t, nTab1 );
                ADDTEXT( " " );                         lcl_AddRangeRef( t, nCol11, nRow11, nCol12, nRow12 );
                ADDTEXT( "   to: tab=" );               __AddDec( t, nTab2 );
                ADDTEXT( " " );                         lcl_AddRangeRef( t, nCol21, nRow21, nCol22, nRow22 );
                ADDTEXT( "   unknown: " );              ADDHEX( 4 );
                PRINT();
            }
            break;
            case 0x014D:        // CHTRINSERTTAB - change tracking: insert tab
            {
                ADDTEXT( "len: " );             ADDDEC( 4 );
                ADDTEXT( "   index: " );        ADDDEC( 4 );
                ADDTEXT( "   opcode: " );       ADDHEX( 2 );
                ADDTEXT( "   accept: " );       ADDHEX( 2 );
                ADDTEXT( "   tab: " );          ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "unknown: " );         ADDHEX( 4 );
                ADDTEXT( "   table name: " );
                AddUNICODEString( t, rIn );
                PRINT();
                LINESTART();
                __AddDec( t, rIn.GetRecLeft() );
                ADDTEXT( " bytes of unknown data:" );
                PRINT();
                ContDump( rIn.GetRecLeft() );
            }
            break;
            case 0x015F:    // LABELRANGES
            {
                UINT16 nCnt, nR1, nR2, nC1, nC2;
                rIn >> nCnt;
                ADDTEXT( "row headers: " );     __AddDec( t, nCnt );
                PRINT();
                while( nCnt-- )
                {
                    rIn >> nR1 >> nR2 >> nC1 >> nC2;
                    LINESTART();
                    AddRangeRef( t, nR1, nC1 | 0xC000, nR2, nC2 | 0xC000, FALSE );
                    PRINT();
                }
                rIn >> nCnt;
                LINESTART();
                ADDTEXT( "column headers: " );  __AddDec( t, nCnt );
                PRINT();
                while( nCnt-- )
                {
                    rIn >> nR1 >> nR2 >> nC1 >> nC2;
                    LINESTART();
                    AddRangeRef( t, nR1, nC1 | 0xC000, nR2, nC2 | 0xC000, FALSE );
                    PRINT();
                }
            }
            break;
            case 0x0193:
            {
                ADDTEXT( "unknown: " );         ADDHEX( 4 );
                PRINT();
                LINESTART();
                ADDTEXT( "16 bytes unknown identification:" );
                PRINT();
                ContDump( 16 );
                LINESTART();
                ADDTEXT( "date/time: " );       ADDDEC( 2 );
                ADDTEXT( "-" );                 ADDDEC( 1 );
                ADDTEXT( "-" );                 ADDDEC( 1 );
                ADDTEXT( " " );                 ADDDEC( 1 );
                ADDTEXT( ":" );                 ADDDEC( 1 );
                ADDTEXT( ":" );                 ADDDEC( 1 );
                ADDTEXT( "   unknown: " );      ADDHEX( 1 );
                PRINT();
                LINESTART();
                ADDTEXT( "user: " );
                if( rIn.GetRecLeft() > 3 )
                    AddUNICODEString( t, rIn );
                PRINT();
            }
            break;
            case 0x0194:
            {
                rIn.DisableDecryption();
                ADDTEXT( "unknown: " );         ADDHEX( 4 );
                ADDTEXT( "   date/time: " );    ADDDEC( 2 );
                ADDTEXT( "-" );                 ADDDEC( 1 );
                ADDTEXT( "-" );                 ADDDEC( 1 );
                ADDTEXT( " " );                 ADDDEC( 1 );
                ADDTEXT( ":" );                 ADDDEC( 1 );
                ADDTEXT( ":" );                 ADDDEC( 1 );
                ADDTEXT( "   unknown: " );      ADDHEX( 1 );
                PRINT();
                LINESTART();
                ADDTEXT( "user: " );
                if( rIn.GetRecLeft() > 3 )
                    AddUNICODEString( t, rIn );
                PRINT();
                LINESTART();
                __AddDec( t, rIn.GetRecLeft() );
                ADDTEXT( " bytes of unknown data:" );
                PRINT();
                ContDump( rIn.GetRecLeft() );
            }
            break;
            case 0x0195:
                rIn.DisableDecryption();
                ContDump( nL );
            break;
            case 0x0196:
            {
                rIn.DisableDecryption();
                ADDTEXT( "unknown: " );             ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "16 bytes unknown identification:" );
                PRINT();
                ContDump( 16 );
                LINESTART();
                ADDTEXT( "16 bytes unknown identification:" );
                PRINT();
                ContDump( 16 );
                LINESTART();
                ADDTEXT( "count of changes: " );    ADDDEC( 2 );
                ADDTEXT( "   " );
                __AddDec( t, rIn.GetRecLeft() );
                ADDTEXT( " bytes of unknown data:" );
                PRINT();
                ContDump( rIn.GetRecLeft() );
            }
            break;
            case 0x01A9:        // USERBVIEW
            {
                LINESTART();
                ADDTEXT( "view id: " );     ADDHEX( 4 );
                ADDTEXT( "   tab id: " );   ADDDEC( 4 );
                ADDTEXT( "   guid: " );     __AddGUID( t, rIn );
                PRINT();
                LINESTART();
                ADDTEXT( "window x: " );    ADDDEC( 4 );
                ADDTEXT( "   y: " );        ADDDEC( 4 );
                ADDTEXT( "   width: " );    ADDDEC( 4 );
                ADDTEXT( "   height: " );   ADDDEC( 4 );
                ADDTEXT( "   ratio: " );    ADDDEC( 2 );
                PRINT();
                LINESTART();
                UINT16 __nFlags = Read2( rIn );
                STARTFLAG();
                ADDFLAG( 0x0001, "fDsplFormulaBar" );
                ADDFLAG( 0x0002, "fDsplStatus" );
                ADDFLAG( 0x0004, "fNoteOff" );
                ADDFLAG( 0x0008, "fDsplHScroll" );
                ADDFLAG( 0x0010, "fDsplVScroll" );
                ADDFLAG( 0x0020, "fBotAdornment" );
                ADDFLAG( 0x0040, "fZoom" );
                ADDFLAG( 0x0080, "fShowPlaceHld" );
                ADDFLAG( 0x0100, "fHideAll" );
                if( !(__nFlags & 0x0180) )
                    ADDTEXT( " fShowAll" );
                PRINT();
                LINESTART();
                ADDTEXT( "flags2: " );          ADDHEX( 2 );
                ADDTEXT( "   merge int: " );    ADDDEC( 2 );
                ADDTEXT( "   reserved: " );     ADDHEX( 2 );
                PRINT();
                if( rIn.GetRecLeft() > 3 )
                {
                    LINESTART();
                    ADDTEXT( "name: " );
                    AddUNICODEString( t, rIn );
                    PRINT();
                }
            }
            break;
            case 0x01AA:        // USERSVIEWBEGIN
            {
                LINESTART();
                ADDTEXT( "guid: " );        __AddGUID( t, rIn );
                ADDTEXT( "   tab id: " );   ADDDEC( 4 );
                PRINT();
                LINESTART();
                ADDTEXT( "wscale: " );      ADDDEC( 4 );
                ADDTEXT( "   icolor: " );   ADDDEC( 4 );
                ADDTEXT( "   pane: " );     ADDDEC( 4 );
                PRINT();
                LINESTART();
                UINT32 __nFlags = Read4( rIn );
                STARTFLAG();
                if( __nFlags & 0x000000FF )
                {
                    ADDFLAG( 0x00000001, "fShowPgBrk" );
                    ADDFLAG( 0x00000002, "fDsplForml" );
                    ADDFLAG( 0x00000004, "fDsplGrid" );
                    ADDFLAG( 0x00000008, "fDsplRCHead" );
                    ADDFLAG( 0x00000010, "fDsplGuts" );
                    ADDFLAG( 0x00000020, "fDsplZeros" );
                    ADDFLAG( 0x00000040, "fPrintHorC" );
                    ADDFLAG( 0x00000080, "fPrintVerC" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0x00007F00 )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x00000100, "fPrintRCHead" );
                    ADDFLAG( 0x00000200, "fPrintGrid" );
                    ADDFLAG( 0x00000400, "fFitToPage" );
                    ADDFLAG( 0x00000800, "fPrintArea" );
                    ADDFLAG( 0x00001000, "fOnePrintArea" );
                    ADDFLAG( 0x00002000, "fFilter" );
                    ADDFLAG( 0x00004000, "fAutoFilter" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0xFFF80000 )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x00020000, "fSplitV" );
                    ADDFLAG( 0x00040000, "fSplitH" );
                    ADDFLAG( 0x00180000, "fHiddenRow" );
                    ADDFLAG( 0x00200000, "fHiddenCol" );
                    ADDFLAG( 0x01000000, "fChartSize" );
                    ADDFLAG( 0x02000000, "fFilterUnique" );
                    ADDFLAG( 0x04000000, "fLayoutView" );
                    ADDRESERVED( 0xF8C18000 );
                    PRINT();
                    LINESTART();
                }
                if( !__nFlags )
                    PRINT();
                ADDTEXT( "visible: " );             ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "pane pos vert: " );       ADDDOUBLE();
                ADDTEXT( "   hor: " );              ADDDOUBLE();
                ADDTEXT( "   1st vis right: " );    ADDDEC( 2 );
                ADDTEXT( "   bott: " );             ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x01AB:        // USERSVIEWEND
            {
                LINESTART();
                ADDTEXT( "settings are valid: " );  ADDHEX( 2 );
                PRINT();
            }
            break;
            case 0x01AD:        // QSI - web query range
            {
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0001, "fTitles" );
                ADDFLAG( 0x0002, "fRowNums" );
                ADDFLAG( 0x0004, "fDisRefr" );
                ADDFLAG( 0x0080, "fFill" );
                ADDFLAG( 0x0100, "fAutoFmt" );
                ADDFLAG( 0x0400, "fDisEdit" );
                ADDRESERVED( 0xFA78 );
                PRINT();
                LINESTART();
                ADDTEXT( "AutoFmt: " );             ADDDEC( 2 );
                ADDTEXT( "   AutoFmtAttr: " );      ADDHEX( 2 );
                ADDTEXT( "   reserved: " );         ADDHEX( 4 );
                PRINT();
                LINESTART();
                ADDTEXT( "name: " );
                AddUNICODEString( t, rIn );
                PRINT();
            }
            break;
            case 0x01AE:
            {
                LINESTART();
                ADDTEXT( "# of tabs: " );
                UINT16  nTabs;
                rIn >> nTabs;
                __AddDec( t, nTabs );
                rIn.PushPosition();
                PRINT();
                LINESTART();
                if( nL <= (ULONG)(2 + 2 * nTabs) )
                {
                    ADDTEXT( "----- shortened record -----" );
                    PRINT();

                    rIn.PopPosition();
                    ContDump( nL - 2 );
                }
                else
                {
                    rIn.RejectPosition();
                    ADDTEXT( "file name: " );
                    AddUNICODEString( t, rIn );
                    PRINT();
                    while( nTabs )
                    {
                        LINESTART();
                        ADDTEXT( "  " );
                        AddUNICODEString( t, rIn );
                        PRINT();
                        nTabs--;
                    }
                }

            }
            break;
            case 0x01B0:        // CONDFMT
            {
                LINESTART();
                ADDTEXT( "cf-count=" );         ADDDEC( 2 );
                rIn >> __nFlags;
                ADDTEXT( "   " );
                STARTFLAG();
                ADDFLAG( 0x0001, "tough-recalc" );
                ADDRESERVED( 0xFFFE );
                PRINT();
                LINESTART();
                sal_uInt16 nR1, nR2, nC1, nC2, nCount;
                rIn >> nR1 >> nR2 >> nC1 >> nC2 >> nCount;
                ADDTEXT( "max-range=" );        lcl_AddRangeRef( t, nC1, nR1, nC2, nR2 );
                ADDTEXT( "   range-count=" );   __AddDec( t, nCount );
                PRINT();

                for( sal_uInt16 nRange = 0; rIn.IsValid() && (nRange < nCount); ++nRange )
                {
                    if( !(nRange % 4) )
                    {
                        LINESTART();
                        ADDTEXT( pPre );
                    }
                    rIn >> nR1 >> nR2 >> nC1 >> nC2;
                    ByteString aRef;
                    lcl_AddRangeRef( aRef, nC1, nR1, nC2, nR2 );
                    aRef.Expand( 16, ' ' );
                    ADDTEXT( aRef );
                    if( (nRange % 4 == 3) || (nRange + 1 == nCount) )
                        PRINT();
                }
            }
            break;
            case 0x01B1:        // CF - conditional format
            {
                sal_uInt8 nType, nOp;
                sal_uInt16 nSize1, nSize2;
                sal_uInt32 nFlags;
                rIn >> nType >> nOp >> nSize1 >> nSize2 >> nFlags;
                LINESTART();
                ADDTEXT( "type=" );                 __AddHex( t, nType );
                ADDTEXT( " (" );
                switch( nType )
                {
                    case 0x01:  ADDTEXT( "compare" );   break;
                    case 0x02:  ADDTEXT( "formula" );    break;
                    default:    ADDTEXT( "!unknown!" );
                }
                ADDTEXT( ")   operator=" );         __AddHex( t, nOp );
                ADDTEXT( " (" );
                switch( nOp )
                {
                    case 0x00:  ADDTEXT( "none" );          break;
                    case 0x01:  ADDTEXT( "between" );       break;
                    case 0x02:  ADDTEXT( "not-between" );   break;
                    case 0x03:  ADDTEXT( "equal" );         break;
                    case 0x04:  ADDTEXT( "not-equal" );     break;
                    case 0x05:  ADDTEXT( "greater" );       break;
                    case 0x06:  ADDTEXT( "less" );          break;
                    case 0x07:  ADDTEXT( "greater-eq" );    break;
                    case 0x08:  ADDTEXT( "less-eq" );       break;
                    default:    ADDTEXT( "!unknown!" );
                }
                ADDTEXT( ")" );
                PRINT();
                LINESTART();
                ADDTEXT( "formula-size-1=" );       __AddDec( t, nSize1 );
                ADDTEXT( "   formula-size-2=" );    __AddDec( t, nSize2 );
                PRINT();
                LINESTART();
                sal_uInt32 __nFlags = nFlags;
                STARTFLAG();
                __nFlags = ~__nFlags;
                ADDFLAG( 0x00000400, "bord-lft" );
                ADDFLAG( 0x00000800, "bord-rgt" );
                ADDFLAG( 0x00001000, "bord-top" );
                ADDFLAG( 0x00002000, "bord-bot" );
                ADDFLAG( 0x00010000, "patt-style" );
                ADDFLAG( 0x00020000, "patt-fgcol" );
                ADDFLAG( 0x00040000, "patt-bgcol" );
                __nFlags = ~__nFlags;
                ADDFLAG( 0x04000000, "font" );
                ADDFLAG( 0x10000000, "bord" );
                ADDFLAG( 0x20000000, "patt" );
                ADDRESERVED( 0xCBC00000 );
                PRINT();
                LINESTART();
                ADDTEXT( "unknown=" );              ADDHEX( 2 );
                PRINT();
                if( nFlags & 0x04000000 )
                {
                    LINESTART(); ADDTEXT( "*** FONT ***" ); PRINT();
                    ContDump( 64 );
                    LINESTART();
                    ADDTEXT( "height=" );           ADDHEX( 4 );
                    rIn >> __nFlags;
                    ADDTEXT( "   style-" );
                    STARTFLAG();
                    ADDFLAG( 0x00000002, "italic" );
                    ADDFLAG( 0x00000080, "strikeout" );
                    ADDRESERVED( 0xFFFFFF7D );
                    ADDTEXT( "   weight=" );        ADDDEC( 2 );
                    PRINT();
                    LINESTART();
                    sal_uInt16 nEsc;
                    rIn >> nEsc;
                    ADDTEXT( "escapement=" );       __AddDec( t, nEsc );
                    ADDTEXT( " (" );
                    switch( nEsc )
                    {
                        case 0x0000:    ADDTEXT( "none" );  break;
                        case 0x0001:    ADDTEXT( "super" ); break;
                        case 0x0002:    ADDTEXT( "sub" );   break;
                        default:        ADDTEXT( "!unknown!" );
                    }
                    sal_uInt8 nUnd;
                    rIn >> nUnd;
                    ADDTEXT( ")   underline=" );    __AddDec( t, nUnd );
                    ADDTEXT( " (" );
                    switch( nUnd )
                    {
                        case 0x00:  ADDTEXT( "none" );      break;
                        case 0x01:  ADDTEXT( "single" );    break;
                        case 0x02:  ADDTEXT( "double" );    break;
                        default:    ADDTEXT( "!unknown!" );
                    }
                    ADDTEXT( ")   unknown=" );      ADDHEX( 1 );
                    ADDTEXT( " " );                 ADDHEX( 1 );
                    ADDTEXT( " " );                 ADDHEX( 1 );
                    PRINT();
                    LINESTART();
                    ADDTEXT( "color=" );            ADDHEX( 4 );
                    ADDTEXT( "   unknown=" );       ADDHEX( 4 );
                    rIn >> __nFlags;
                    ADDTEXT( "   used-" );
                    STARTFLAG();
                    __nFlags = ~__nFlags;
                    ADDFLAG( 0x00000002, "italic" );
                    ADDFLAG( 0x00000080, "strikeout" );
                    __nFlags = ~__nFlags;
                    ADDRESERVED( 0xFFFFFF65 );
                    PRINT();
                    LINESTART();
                    ADDTEXT( "escape-def=" );       ADDHEX( 4 );
                    ADDTEXT( "   underl-def=" );    ADDHEX( 4 );
                    PRINT();
                    ContDump( 18 );
                }
                if( nFlags & 0x10000000 )
                {
                    LINESTART(); ADDTEXT( "*** BORDER ***" ); PRINT();
                    sal_uInt16 nLine;
                    sal_uInt32 nColor;
                    rIn >> nLine >> nColor;
                    LINESTART();
                    ADDTEXT( "line-style=" );       __AddHex( t, nLine );
                    ADDTEXT( " (lft=" );            __AddDec( t, (sal_uInt16)(nLine & 0x000F) );
                    ADDTEXT( " rgt=" );             __AddDec( t, (sal_uInt16)((nLine & 0x00F0) >> 4) );
                    ADDTEXT( " top=" );             __AddDec( t, (sal_uInt16)((nLine & 0x0F00) >> 8) );
                    ADDTEXT( " bot=" );             __AddDec( t, (sal_uInt16)((nLine & 0xF000) >> 12) );
                    ADDTEXT( ")" );
                    PRINT();
                    LINESTART();
                    ADDTEXT( "line-color=" );       __AddHex( t, nColor );
                    ADDTEXT( " (lft=" );            __AddDec( t, (sal_uInt16)(nColor & 0x0000007F) );
                    ADDTEXT( " rgt=" );             __AddDec( t, (sal_uInt16)((nColor & 0x00003F80) >> 7) );
                    ADDTEXT( " top=" );             __AddDec( t, (sal_uInt16)((nColor & 0x007F0000) >> 16) );
                    ADDTEXT( " bot=" );             __AddDec( t, (sal_uInt16)((nColor & 0x3F800000) >> 23) );
                    ADDTEXT( ")   unknown=" );      ADDHEX( 2 );
                    PRINT();
                }
                if( nFlags & 0x20000000 )
                {
                    LINESTART(); ADDTEXT( "*** AREA ***" ); PRINT();
                    sal_uInt16 nPatt, nColor;
                    rIn >> nPatt >> nColor;
                    LINESTART();
                    ADDTEXT( "pattern=" );          __AddHex( t, nPatt );
                    ADDTEXT( " (" );                __AddDec( t, (sal_uInt16)((nPatt & 0xFC00) >> 10) );
                    ADDTEXT( ")   color=" );        __AddHex( t, nColor );
                    ADDTEXT( " (fg=" );             __AddDec( t, (sal_uInt16)(nColor & 0x007F) );
                    ADDTEXT( " bg=" );              __AddDec( t, (sal_uInt16)((nColor & 0x3F80) >> 7) );
                    ADDTEXT( ")" );
                    PRINT();
                }
                if( rIn.IsValid() && nSize1 && (rIn.GetRecLeft() >= nSize1) )
                {
                    LINESTART(); ADDTEXT( "*** FORMULA 1 ***" ); PRINT();
                    FormulaDump( nSize1, FT_RangeName );
                }
                if( rIn.IsValid() && nSize2 && (rIn.GetRecLeft() >= nSize2) )
                {
                    LINESTART(); ADDTEXT( "*** FORMULA 2 ***" ); PRINT();
                    FormulaDump( nSize2, FT_RangeName );
                }
            }
            break;
            case 0x01B2:    // DVAL - header of DV recs
            {
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDTEXT( " (" );
                __AddPureBin( t, __nFlags );
                ADDTEXT( ")" );
                ADDTEXT( "):" );
                ADDFLAG( 0x0001, "fWnClosed" );
                ADDFLAG( 0x0002, "fWnPinned" );
                ADDFLAG( 0x0004, "fCached" );
                PRINT();
                LINESTART();
                ADDTEXT( "input window: " );
                ADDHEX( 4 );
                ADDTEXT( " / " );
                ADDHEX( 4 );
                PRINT();
                LINESTART();
                ADDTEXT( "object id: " );
                ADDHEX( 4 );
                PRINT();
                LINESTART();
                ADDTEXT( "num of DV recs: " );
                ADDDEC( 4 );
                PRINT();
            }
            break;
            case 0x01B6:    // TXO - text box
            {
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                switch( __nFlags & 0x000E )
                {
                    case 0x0002:    ADDTEXT( " h-left" );   break;
                    case 0x0004:    ADDTEXT( " h-center" ); break;
                    case 0x0006:    ADDTEXT( " h-right" );  break;
                    case 0x0008:    ADDTEXT( " h-block" );  break;
                    default:        ADDTEXT( " *h-unknown*" );
                }
                switch( __nFlags & 0x0070 )
                {
                    case 0x0010:    ADDTEXT( " v-top" );    break;
                    case 0x0020:    ADDTEXT( " v-center" ); break;
                    case 0x0030:    ADDTEXT( " v-bottom" ); break;
                    case 0x0040:    ADDTEXT( " v-block" );  break;
                    default:        ADDTEXT( " *v-unknown*" );
                }
                ADDFLAG( 0x0200, "lock-text" );
                ADDRESERVED( 0xFD81 );
                ADDTEXT( "   orient=" );
                sal_uInt16 nOrient = rIn.ReaduInt16();
                __AddDec( t, nOrient );
                ADDTEXT( " (" );
                switch( nOrient )
                {
                    case 0:     ADDTEXT( "no-rot" );        break;
                    case 1:     ADDTEXT( "stacked" );       break;
                    case 2:     ADDTEXT( "90\xB0 ccw" );    break;
                    case 3:     ADDTEXT( "90\xB0 cw" );     break;
                    default:    ADDTEXT( "!unknown!" );
                }
                ADDTEXT( ")" );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved=" );
                ADDHEX( 2 ); ADDTEXT( " " ); ADDHEX( 2 ); ADDTEXT( " " ); ADDHEX( 2 );
                ADDTEXT( "   text-len=" );      ADDDEC( 2 );
                ADDTEXT( "   format-size=" );   ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved=" );      ADDHEX( 2 );
                sal_uInt16 nLinkSize = rIn.ReaduInt16();
                ADDTEXT( "   link-size=" );     __AddDec( t, nLinkSize );
                PRINT();
                if( nLinkSize > 0 )
                {
                    LINESTART();
                    sal_uInt16 nFmlaSize = rIn.ReaduInt16();
                    ADDTEXT( "fmla-size=" );        __AddDec( t, nFmlaSize );
                    ADDTEXT( "   reserved=" );      ADDHEX( 4 );
                    PRINT();
                    FormulaDump( nFmlaSize, FT_CellFormula );
                }
            }
            break;
            case 0x01BE:    // DV - data validation record
            {
                UINT32  __nFlags;
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDTEXT( " (" );
                __AddPureBin( t, __nFlags );
                ADDTEXT( ")" );
                PRINT();
                LINESTART();
                if( __nFlags )
                {
                    ADDTEXT( "  " );
                    ADDFLAG( 0x00000080, "fStrLookup" );
                    ADDFLAG( 0x00000100, "fAllowBlank" );
                    ADDFLAG( 0x00000200, "fSuppressCombo" );
                    ADDFLAG( 0x00040000, "fShowInputMsg" );
                    ADDFLAG( 0x00080000, "fShowErrorMsg" );
                }
                PRINT();
                LINESTART();
                ADDTEXT( "error style: " );
                const char*     pErrStyle[] = { "stop", "warning", "info", "4" };
                ADDTEXT( pErrStyle[ ( __nFlags >> 4 ) & 0x03 ] );
                PRINT();
                LINESTART();
                const char*     pValType[] =
                {
                    "all", "integer", "decimal", "list", "date", "time", "text len", "user",
                    "8", "9", "A", "B", "C", "D", "E", "F"
                };
                LINESTART();
                ADDTEXT( "validation type: " );
                ADDTEXT( pValType[ __nFlags & 0x0000000F ] );
                PRINT();

                const char*     pOpType[] =
                {
                    "between", "not between", "equal", "not equal",
                    "greater", "less", "greater or equal", "less or equal",
                    "8", "9", "A", "B", "C", "D", "E", "F"
                };
                LINESTART();
                ADDTEXT( "operator type: " );
                ADDTEXT( pOpType[ ( __nFlags >> 20 ) & 0x0000000F ] );
                PRINT();

                LINESTART();
                ADDTEXT( "Prompt Title:   " );
                AddUNICODEString( t, rIn );
                PRINT();
                LINESTART();
                ADDTEXT( "Error Title:    " );
                AddUNICODEString( t, rIn );
                PRINT();
                LINESTART();
                ADDTEXT( "Prompt Message: " );
                AddUNICODEString( t, rIn );
                PRINT();
                LINESTART();
                ADDTEXT( "Error Message:  " );
                AddUNICODEString( t, rIn );
                PRINT();
                if( rIn.GetRecLeft() > 8 )
                {
                    UINT16      nLen;
                    rIn >> nLen;
                    LINESTART();
                    ADDTEXT( "Len1: " );
                    __AddDec( t, nLen );
                    ADDTEXT( "   (unknown1: " );
                    ADDHEX( 2 );
                    ADDTEXT( ")" );
                    PRINT();
                    FormulaDump( nLen, FT_RangeName );

                    rIn >> nLen;
                    LINESTART();
                    ADDTEXT( "Len2: " );
                    __AddDec( t, nLen );
                    ADDTEXT( "   (unknown2: " );
                    ADDHEX( 2 );
                    ADDTEXT( ")" );
                    PRINT();
                    FormulaDump( nLen, FT_RangeName );

                    LINESTART();
                    ADDTEXT( "range count: " );
                    ADDHEX( 2 );
                    PRINT();
                }

                while( rIn.GetRecLeft() >= 8 )
                {
                    // Row-Row / Col-Col
                    UINT16  nR1, nR2, nC1, nC2;
                    rIn >> nR1 >> nR2 >> nC1 >> nC2;
                    LINESTART();
                    AddRangeRef( t, nR1, nC1 | 0xC000, nR2, nC2 | 0xC000, FALSE );
                    PRINT();
                }
            }
            break;
            case 0x01B8:        // HLINK
            {
                PreDump( nL );

                UINT32 n1, n2;
                LINESTART();
                PRINT();
                UINT16 nR1, nR2, nC1, nC2;
                rIn >> nR1 >> nR2 >> nC1 >> nC2;
                ADDTEXT( "Cellrange=" );
                lcl_AddRangeRef( t, nC1, nR1, nC2, nR2 );
                PRINT();
                LINESTART();
                ADDTEXT( "GUID StdLink=" );     __AddGUID( t, rIn );
                PRINT();
                LINESTART();
                ADDTEXT( "        must=79EAC9D0-BAF9-11CE-8C82-00AA004BA90B" );
                PRINT();
                LINESTART();
                ADDTEXT( "unknown=" );          ADDHEX( 4 );
                PRINT();
                UINT32 __nFlags = Read4( rIn );
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x00000001, "fBody" );
                ADDFLAG( 0x00000002, "fAbs" );
                ADDFLAG( 0x00000014, "fDescr" );
                ADDFLAG( 0x00000008, "fMark" );
                ADDFLAG( 0x00000080, "fFrame" );
                ADDFLAG( 0x00000100, "fUNC" );
                ADDRESERVED( 0xFFFFFE60 );
                PRINT();

                //description
                String aData;
                if( __nFlags & 0x00000014 )
                {
                    LINESTART();
                    rIn >> n1;
                    ADDTEXT( "## Description ##  [l=" );
                    __AddDec( t, n1 );
                    ADDTEXT( "]: '" );
                    aData = rIn.ReadRawUniString( (USHORT)(n1 - 1), TRUE );
                    t += GETSTR( aData );
                    ADDTEXT( "<" ); ADDHEX( 2 ); ADDTEXT( ">'" ); // trailing zero
                    PRINT();
                }

                // frame name
                if( __nFlags & 0x00000080 )
                {
                    LINESTART();
                    rIn >> n1;
                    ADDTEXT( "## Frame ## [l=" );
                    __AddDec( t, n1 );
                    ADDTEXT( "]: '" );
                    aData = rIn.ReadRawUniString( (USHORT)(n1 - 1), TRUE );
                    t += GETSTR( aData );
                    ADDTEXT( "<" ); ADDHEX( 2 ); ADDTEXT( ">'" ); // trailing zero
                    PRINT();
                }

                // network path
                if( __nFlags & 0x00000100 )
                {
                    LINESTART();
                    rIn >> n1;
                    ADDTEXT( "## UNC ## [l=" );
                    __AddDec( t, n1 );
                    ADDTEXT( "]: '" );
                    aData = rIn.ReadRawUniString( (USHORT)(n1 - 1), TRUE );
                    t += GETSTR( aData );
                    ADDTEXT( "<" ); ADDHEX( 2 ); ADDTEXT( ">'" ); // trailing zero
                    PRINT();
                }

                // file link or URL
                else if( __nFlags & 0x00000001 )
                {
                    rIn.PushPosition();
                    rIn >> n1;
                    rIn.PopPosition();
                    LINESTART();
                    ADDTEXT( "## Content GUID ## " );
                    __AddGUID( t, rIn );
                    switch( n1 )
                    {
                        case 0x00000303:    // file
                        {
                            ADDTEXT( " File Moniker" );
                            PRINT();
                            LINESTART();
                            ADDTEXT( "              must=00000303-0000-0000-C000-000000000046" );
                            PRINT();
                            LINESTART();
                            ADDTEXT( "## File link ##  up level=" );
                            ADDDEC( 2 );
                            rIn >> n2;
                            ADDTEXT( "   [l=" );        __AddDec( t, n2 );
                            ADDTEXT( ", 8-Bit]: '" );
                            aData = rIn.ReadRawByteString( (USHORT)(n2 - 1) );
                            t += GETSTR( aData );
                            ADDTEXT( "<" ); ADDHEX( 1 ); ADDTEXT( ">'" ); // trailing zero
                            PRINT();
                            ContDump( 24 );
                            rIn >> n2;
                            LINESTART();
                            ADDTEXT( "bytes left=" );  __AddDec( t, n2 );
                            if( n2 )
                            {
                                rIn >> n2;
                                LINESTART();
                                ADDTEXT( "   string byte count=" );
                                __AddDec( t, n2 );
                                ADDTEXT( "   unknown=" );
                                ADDHEX( 2 );
                                PRINT();
                                LINESTART();
                                ADDTEXT( "[l=" );
                                __AddDec( t, n2 / 2 );
                                ADDTEXT( "]: '" );
                                aData = rIn.ReadRawUniString( (USHORT)n2, TRUE );
                                t += GETSTR( aData );
                                ADDTEXT( "'" );
                            }
                            PRINT();
                        }
                        break;
                        case 0x79EAC9E0:    // URL
                        {
                            ADDTEXT( " URL Moniker" );
                            PRINT();
                            LINESTART();
                            ADDTEXT( "              must=79EAC9E0-BAF9-11CE-8C82-00AA004BA90B" );
                            PRINT();
                            rIn >> n2;
                            LINESTART();
                            ADDTEXT( "## URL ##  string byte count=" );
                            __AddDec( t, n2 );
                            PRINT();
                            LINESTART();
                            ADDTEXT( "[l=" );
                            __AddDec( t, n2 / 2 );
                            ADDTEXT( "]: '" );
                            aData = rIn.ReadRawUniString( (USHORT)(n2 / 2 - 1), TRUE );
                            t += GETSTR( aData );
                            ADDTEXT( "<" ); ADDHEX( 2 ); ADDTEXT( ">'" ); // trailing zero
                            PRINT();
                        }
                        break;
                        default:
                        {
                            ADDTEXT( " (!!UNKNOWN!!)" );
                            PRINT();
                        }
                        break;
                    }
                }

                // text mark
                if( __nFlags & 0x00000008 )
                {
                    LINESTART();
                    rIn >> n1;
                    ADDTEXT( "## Text mark ##  [l=" );
                    __AddDec( t, n1 );
                    ADDTEXT( "]: '" );
                    aData = rIn.ReadRawUniString( (USHORT)(n1 - 1), TRUE );
                    t += GETSTR( aData );
                    ADDTEXT( "<" ); ADDHEX( 2 ); ADDTEXT( ">'" ); // trailing zero
                    PRINT();
                }
            }
            break;
            case 0x01BB:        // SXFDBTYPE - SQL data type
            {
                ADDTEXT( "SQL data type: " );       ADDHEX( 2 );
                PRINT();
            }
            break;
            case 0x0201:
            {
                LINESTART();
                ADDCELLHEAD();
                PRINT();
            }
            break;
            case 0x0203:
            {
                LINESTART();
                ADDCELLHEAD();
                ADDTEXT( "   val = " );
                ADDDOUBLE();
                PRINT();
            }
            break;
            case 0x0205:
            {
                LINESTART();
                ADDCELLHEAD();
                ADDTEXT( "   val = " );     ADDHEX( 1 );
                ADDTEXT( "   type = " );    ADDDEC( 1 );
                PRINT();
            }
            break;
            case 0x0208:        // ROW - row info
            {
                LINESTART();
                ADDTEXT( "row #: " );               ADDDEC( 2 );
                ADDTEXT( "   def. cols: " );        ADDDEC( 2 );
                ADDTEXT( "-" );                     ADDDEC( 2 );
                ADDTEXT( "   ht: " );               ADDDEC( 2 );
                ADDTEXT( "   reserved: " );         ADDHEX( 4 );
                PRINT();
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDTEXT( " outlnlev=" );
                __AddDec( t, (UINT16)(__nFlags & 0x0007) );
                ADDFLAG( 0x0010, "fCollapsed" );
                ADDFLAG( 0x0020, "fRowHeightZero" );
                ADDFLAG( 0x0040, "fUnsynced" );
                ADDFLAG( 0x0080, "fGhostDirty" );
                ADDRESERVED( 0xFF08 );
                PRINT();
                UINT16 nXF;
                rIn >> nXF;
                LINESTART();
                ADDTEXT( "ix to XF: "  );       __AddDec( t, (UINT16)(nXF & 0x0FFF) );
                ADDTEXT( "   add. flags(" );    __AddHex( t, nXF );
                ADDTEXT( "):" );
                ADDFLAG( 0x1000, "fExAsc" );
                ADDFLAG( 0x2000, "fExDsc" );
                ADDRESERVED( 0xC000 );
                PRINT();
            }
            break;
            case 0x0021:    // ARRAY
            case 0x0221:
            {
                UINT16  nR1, nR2;
                UINT8   nC1, nC2;
                rIn >> nR1 >> nR2 >> nC1 >> nC2 >> __nFlags;
                LINESTART();
                ADDTEXT( "range: " );
                lcl_AddRangeRef( t, nC1, nR1, nC2, nR2 );
                PRINT();
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, "fAlwaysCalc" );
                ADDFLAG( 0x0002, "fCalcOnLoad" );
                ADDRESERVED( 0xFFFC );
                PRINT();
                LINESTART();
                ADDTEXT( "chn = " );
                ADDHEX( 4 );
                UINT16  n;
                rIn >> n;
                ADDTEXT( "    cce = " );
                __AddDec( t, n );
                PRINT();
                FormulaDump( n, FT_SharedFormula );
            }
            break;
            case 0x0225:        // DEFAULTROWHEIGHT - height & flags
            {
                rIn >> __nFlags;
                LINESTART();
                ADDTEXT( "default row " );
                STARTFLAG();
                ADDFLAG( 0x0001, "fUnsynced" );
                ADDFLAG( 0x0002, "fRowHtZero" );
                ADDFLAG( 0x0004, "fExAsc" );
                ADDFLAG( 0x0008, "fExDsc" );
                ADDRESERVED( 0xFFF0 );
                PRINT();
                LINESTART();
                ADDTEXT( "default row height: " );
                ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x023E:        // WINDOW2
            {
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0001, "show-formulas" );
                ADDFLAG( 0x0002, "show-grid" );
                ADDFLAG( 0x0004, "show-headers" );
                ADDFLAG( 0x0008, "frozen" );
                ADDFLAG( 0x0010, "show-zero" );
                ADDFLAG( 0x0020, "auto-grid-color" );
                ADDFLAG( 0x0040, "right-to-left" );
                ADDFLAG( 0x0080, "show-outline" );
                ADDFLAG( 0x0100, "remove-splits" );
                ADDFLAG( 0x0200, "sheet-selected" );
                ADDFLAG( 0x0400, "sheet-visible" );
                ADDFLAG( 0x0800, "show-pagebreak" );
                ADDRESERVED( 0xF000 );
                PRINT();
                LINESTART();
                ADDTEXT( "first-row=" );        ADDDEC( 2 );
                ADDTEXT( "   first-col=" );     ADDDEC( 2 );
                ADDTEXT( "   grid-color=" );    ADDDEC( 2 );
                ADDTEXT( "   reserved=" );      ADDHEX( 2 );
                PRINT();
                // reallife: WINDOW2 in charts do not have teh following fields
                if( rIn.GetRecLeft() >= 8 )
                {
                    LINESTART();
                    ADDTEXT( "pagebreak-zoom=" );   ADDDEC( 2 );
                    ADDTEXT( "%   view-zoom=" );    ADDDEC( 2 );
                    ADDTEXT( "%   reserved=" );     ADDHEX( 4 );
                    PRINT();
                }
            }
            break;
            case 0x027E:
            {
                ADDCELLHEAD();
                ADDTEXT( "   val = " );
                __AddRK( t, rIn.ReadInt32() );
                PRINT();
            }
            break;
            case 0x0293:        // STYLE
            {
                LINESTART();
                sal_uInt16 nXF;
                rIn >> nXF;
                ADDTEXT( "xf-ref=" );           __AddHex( t, nXF );
                ADDTEXT( " (xf=#" );            __AddDec( t, static_cast< sal_uInt16 >( nXF & EXC_STYLE_XFMASK ) );
                if( ::get_flag( nXF, EXC_STYLE_BUILTIN ) )
                {
                    sal_uInt8 nStyleId, nLevel;
                    rIn >> nStyleId >> nLevel;
                    ADDTEXT( " builtin)   style-id=" ); __AddDec( t, nStyleId );
                    ADDTEXT( " (" );
                    static const sal_Char* ppcStyles[] = {
                        "Normal", "RowLevel", "ColLevel", "Comma", "Currency",
                        "Percent", "Comma_0", "Currency_0",
                        "Hyperlink", "Followed_Hyperlink" };
                    if( nStyleId < STATIC_TABLE_SIZE( ppcStyles ) )
                        ADDTEXT( ppcStyles[ nStyleId ] );
                    else
                        ADDTEXT( "!unknown!" );
                    ADDTEXT( ")   outline-level=" );    __AddDec( t, nLevel );
                }
                else
                {
                    ADDTEXT( ")   name=" );
                    AddUNICODEString( t, rIn );
                }
                PRINT();
            }
            break;
            case 0x041E:
            {
                LINESTART();
                ADDTEXT( "Index: " );           ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "Format: " );       AddUNICODEString( t, rIn );
                PRINT();
            }
            break;
            case 0x04BC:
            {
                UINT16  nR1, nR2;
                UINT8   nC1, nC2;
                LINESTART();
                rIn >> nR1 >> nR2 >> nC1 >> nC2;
                lcl_AddRangeRef( t, nC1, nR1, nC2, nR2 );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved = " );
                __AddPureHex( t, Read2( rIn ) );
                UINT16  n;
                rIn >> n;
                ADDTEXT( "    cce = " );
                __AddDec( t, n );
                PRINT();
                FormulaDump( n, FT_SharedFormula );
            }
            break;
            case 0x0803:        // WEBQRYSETTINGS - web query: options
            {
                UINT16 nCnt;
                LINESTART();
                ADDTEXT( "repeated recnum: " );
                ADDHEX( 2 );
                ADDTEXT( "   unknown:" );
                for( nCnt = 0; nCnt < 3; nCnt++ )
                {
                    ADDTEXT( " " );
                    ADDHEX( 2 );
                }
                PRINT();
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0001, "fImportPRE" );
                ADDFLAG( 0x0002, "fIgnoreSep" );
                ADDFLAG( 0x0004, "fUseSetting" );
                ADDFLAG( 0x0010, "fIgnoreDate" );
                ADDFLAG( 0x0020, "fWhatIsIt?" );
                ADDRESERVED( 0xFFC8 );
                PRINT();
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0002, "fTables" );
                ADDRESERVED( 0xFFFD );
                PRINT();
                LINESTART();
                ADDTEXT( "unknown: " );             ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "refresh: " );             ADDDEC( 2 );
                ADDTEXT( "   unknown: " );          ADDHEX( 2 );
                ADDTEXT( " " );                     ADDHEX( 2 );
                PRINT();
            }
            break;
            case 0x0804:        // WEBQRYTABLES - web query: selected tables
            {
                LINESTART();
                ADDTEXT( "repeated recnum: " );     ADDHEX( 2 );
                ADDTEXT( "   unknown: " );          ADDHEX( 2 );
                PRINT();
                if( nL > 6 )
                {
                    LINESTART();
                    ADDTEXT( "text: " );
                    AddUNICODEString( t, rIn );
                    PRINT();
                }
            }
            break;
            case 0x0809:        // BOF
            {
                rIn.DisableDecryption();
                LINESTART();
                ADDTEXT( "version number:      " );
                ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "substream type:      " );
                rIn >> mnSubstream;
                __AddHex( t, mnSubstream );
                ADDTEXT( " (" );
                switch( mnSubstream )
                {
                    case 0x0005:    p = "Workbook globals";             break;
                    case 0x0006:    p = "Visual Basic module";          break;
                    case 0x0010:    p = "Worksheet or dialog sheet";    break;
                    case 0x0020:    p = "Chart";                        break;
                    case 0x0040:    p = "MS 4.0 Macro";                 break;
                    case 0x0100:    p = "Workspace file";               break;
                    default:        p = pU;
                }
                ADDTEXT( p );
                ADDTEXT( ")" );
                PRINT();
                LINESTART();
                UINT16  n;
                rIn >> n;
                ADDTEXT( "build identifier:    ");
                __AddHex( t, n );
                ADDTEXT( " (=" );
                __AddDec( t, n );
                ADDTEXT( ")" );
                PRINT();

                LINESTART();
                rIn >> n;
                ADDTEXT( "build year:          ");
                __AddHex( t, n );
                ADDTEXT( " (=" );
                __AddDec( t, n );
                ADDTEXT( ")" );
                PRINT();

                UINT32  __nFlags;
                rIn >> __nFlags;
                LINESTART();
                ADDTEXT( "file history " );
                STARTFLAG();
                if( __nFlags )
                {
                    ADDFLAG( 0x00000001, "fWin" );
                    ADDFLAG( 0x00000002, "fRisc" );
                    ADDFLAG( 0x00000004, "fBeta" );
                    ADDFLAG( 0x00000008, "fWinAny" );
                    ADDFLAG( 0x00000010, "fMacAny" );
                    ADDFLAG( 0x00000020, "fBetaAny" );
                    ADDFLAG( 0x00000100, "fRiscAny" );
                    ADDRESERVED( 0xFFFFE0C0 );
                }
                PRINT();

                LINESTART();
                ADDTEXT( "lowest BIFF version: ");
                ADDHEX( 4 );
                PRINT();
            }
            break;
            case 0x1002:        // ChartChart
            {
                LINESTART();
                ADDTEXT( "Pos = " );
                ADD16P16();
                ADDTEXT( " / " );
                ADD16P16();
                ADDTEXT( "     Size = " );
                ADD16P16();
                ADDTEXT( " / " );
                ADD16P16();
                PRINT();
            }
                break;
            case 0x1003:        // ChartSeries
            {
                sal_uInt16 nCatType, nValType, nCatCnt, nValCnt, nBubType, nBubCnt;
                rIn >> nCatType >> nValType >> nCatCnt >> nValCnt >> nBubType >> nBubCnt;
                LINESTART();
                ADDTEXT( "category-type=" );    __AddDec( t, nCatType );
                ADDTEXT( " " );                 ADDTEXT( GetSeriesType( nCatType ) );
                ADDTEXT( "   count=" );         __AddDec( t, nCatCnt );
                PRINT();
                LINESTART();
                ADDTEXT( "   value-type=" );    __AddDec( t, nValType );
                ADDTEXT( " " );                 ADDTEXT( GetSeriesType( nValType ) );
                ADDTEXT( "   count=" );         __AddDec( t, nValCnt );
                PRINT();
                LINESTART();
                ADDTEXT( "  bubble-type=" );    __AddDec( t, nBubType );
                ADDTEXT( " " );                 ADDTEXT( GetSeriesType( nBubType ) );
                ADDTEXT( "   count=" );         __AddDec( t, nBubCnt );
                PRINT();
            }
            break;
            case 0x1006:        // ChartDataformat
            {
                INT16   n;
                LINESTART();
                rIn >> n;
                ADDTEXT( "Point number = " );
                __AddDec( t, n );
                if( n == -1 )
                    ADDTEXT( " (entire series)" );
                PRINT();
                LINESTART();
                ADDTEXT( "Series index = " );
                ADDDEC( 2 );
                ADDTEXT( "   Series number = " );
                ADDDEC( 2 );
                if( Read2( rIn ) & 0x01 )
                    ADDTEXT( "  (fXL4iss)" );
                PRINT();
            }
                break;
            case 0x1007:        // ChartLineform
            {
                LINESTART();
                ADDTEXT( "Color = " );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                ADDTEXT( "   Pattern : " );
                ADDTEXT( GetLineType( Read2( rIn ) ) );
                switch( ( INT16 ) Read2( rIn ) )
                {
                    case -1:    p = "hairline";         break;
                    case 0:     p = "narrow (single)";  break;
                    case 1:     p = "medium (double)";  break;
                    case 2:     p = "wide (triple)";    break;
                    default:    p = pU;
                }
                ADDTEXT( ", " );
                ADDTEXT( p );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fAuto" );
                    ADDFLAG( 0x0004, "fDrawTick" );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "color index = " );
                ADDDEC( 2 );
                PRINT();
            }
                break;
            case 0x1009:        // ChartMarkerformat
            {
                UINT16  n;
                LINESTART();
                ADDTEXT( "Fore = " );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                ADDTEXT( "  Back = " );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                rIn >> n;
                switch( n )
                {
                    case 0:     p = "no marker";        break;
                    case 1:     p = "square";           break;
                    case 2:     p = "diamond";          break;
                    case 3:     p = "triangle";         break;
                    case 4:     p = "X";                break;
                    case 5:     p = "star";             break;
                    case 6:     p = "Dow-Jones";        break;
                    case 7:     p = "std deviation";    break;
                    case 8:     p = "circle";           break;
                    case 9:     p = "plus sign";        break;
                    default:    p = pU;
                }
                ADDTEXT( "  Type = " );
                ADDTEXT( p );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fAuto" );
                    ADDFLAG( 0x0010, "fNoBackg" );
                    ADDFLAG( 0x0020, "fNoFore" );
                    ADDRESERVED( 0xFFCE );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "border color = " );
                ADDDEC( 2 );
                ADDTEXT( "   fill color = " );
                ADDDEC( 2 );
                ADDTEXT( "   size = " );
                ADDDEC(4);
                PRINT();
            }
                break;
            case 0x100A:        // ChartAreaformat
            {
                LINESTART();
                ADDTEXT( "Fore = " );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                ADDTEXT( "  Back = " );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                ADDTEXT( "  Pattern = " );
                UINT16 n;
                rIn >> n >> __nFlags;
                __AddDec( t, n );
                ADDTEXT( " (" );
                __AddHex( t, n );
                ADDTEXT( ")" );
                PRINT();
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x01, "fAuto" );
                    ADDFLAG( 0x02, "fInvertNeg" );
                    PRINT();
                }
            }
                LINESTART();
                ADDTEXT( "color index fore = " );
                ADDDEC( 2 );
                ADDTEXT( ", back = " );
                ADDDEC( 2 );
                PRINT();
                break;
            case 0x100B:        // ChartPieformat
                LINESTART();
                ADDDEC( 2 );
                ADDTEXT( "%" );
                PRINT();
                break;
            case 0x100C:        // ChartAttachedlabel
                ContDump( nL );
                break;
            case 0x100D:        // ChartSeriestext
                ContDump( nL );
                break;
            case 0x1014:        // ChartChartformat
            {
                ContDump( 16 );
                LINESTART();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fVaried" );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "drawing order = " );
                ADDDEC( 2 );
                PRINT();
            }
                break;
            case 0x1015:        // ChartLegend
            {
                UINT32  nX, nY, nDx, nDy;
                UINT8   nWType, nWSpacing;
                rIn >> nX >> nY >> nDx >> nDy >> nWType >> nWSpacing >> __nFlags;
                LINESTART();
                __AddDec( t, nX );
                ADDTEXT( " / " );
                __AddDec( t, nY );
                ADDTEXT( "  [" );
                __AddDec( t, nDx );
                ADDTEXT( " / " );
                __AddDec( t, nDy );
                ADDTEXT( "]" );
                PRINT();
                LINESTART();
                ADDTEXT( "  Type: " );
                switch( nWType )
                {
                    case 0:     p = "bottom";                               break;
                    case 1:     p = "corner";                               break;
                    case 2:     p = "top";                                  break;
                    case 3:     p = "right";                                break;
                    case 4:     p = "left";                                 break;
                    case 7:     p = "not docked or inside the plot area";   break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                ADDTEXT( "   Spacing: " );
                switch( nWSpacing )
                {
                    case 0:     p = "close";        break;
                    case 1:     p = "medium";       break;
                    case 2:     p = "open";         break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                PRINT();
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x01, "fAutoPosition" );
                    ADDFLAG( 0x02, "fAutoSeries" );
                    ADDFLAG( 0x04, "fAutoPosX" );
                    ADDFLAG( 0x08, "fAutoPosY" );
                    ADDFLAG( 0x10, "fVert" );
                    ADDFLAG( 0x20, "fWasDataTable" );
                    PRINT();
                }
            }
                break;
            case 0x1016:        // ChartSerieslist
                ContDump( nL );
                break;
            case 0x1017:        // ChartBar
            {
                LINESTART();
                ADDTEXT( "space betw. bars = " );
                ADDDEC( 2 );
                ADDTEXT( "  space betw. cat = " );
                ADDDEC( 2 );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fTranspose" );
                    ADDFLAG( 0x0002, "fStacked" );
                    ADDFLAG( 0x0004, "f100" );
                    ADDFLAG( 0x0008, "fHasShadow" );
                    PRINT();
                }
            }
                break;
            case 0x1018:        // ChartLine
                ContDump( nL );
                break;
            case 0x1019:        // ChartPie
                ContDump( nL );
                break;
            case 0x101A:        // ChartArea
                ContDump( nL );
                break;
            case 0x101C:        // ChartLine
                ContDump( nL );
                break;
            case 0x101D:        // ChartAxis
            {
                static const sal_Char* const ppcIds[] = { "x-axis", "y-axis", "z-axis" };
                LINESTART();
                sal_uInt16 nAxisId = rIn.ReaduInt16();
                ADDTEXT( "axis-id=" );          __AddDec( t, nAxisId );
                lcl_AddEnum( t, nAxisId, ppcIds, STATIC_TABLE_SIZE( ppcIds ) );
                PRINT();
                ContDump( 16 );
            }
                break;
            case 0x101E:        // CHTICK
            {
                static const sal_Char* const ppcTickMode[] = { "off", "inside", "outside", "cross" };
                static const sal_Char* const ppcTickPos[] = { "off", "low", "high", "next-to-axis" };
                static const sal_Char* const ppcText[] = { "transparent", "opaque" };
                LINESTART();
                sal_uInt8 nMajor, nMinor, nPos, nText;
                rIn >> nMajor >> nMinor >> nPos >> nText;
                ADDTEXT( "major=" );            __AddDec( t, nMajor );
                lcl_AddEnum( t, nMajor, ppcTickMode, STATIC_TABLE_SIZE( ppcTickMode ) );
                ADDTEXT( "   minor=" );         __AddDec( t, nMinor );
                lcl_AddEnum( t, nMinor, ppcTickMode, STATIC_TABLE_SIZE( ppcTickMode ) );
                PRINT();
                LINESTART();
                ADDTEXT( "position=" );         __AddDec( t, nPos );
                lcl_AddEnum( t, nPos, ppcTickPos, STATIC_TABLE_SIZE( ppcTickPos ) );
                ADDTEXT( "   text-mode=" );     __AddDec( t, nText );
                lcl_AddEnum( t, nText, ppcText, STATIC_TABLE_SIZE( ppcText ) );
                ADDTEXT( "    text-color=" );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved=" );         ADDHEX( 4 );
                ADDTEXT( "," );                 ADDHEX( 4 );
                ADDTEXT( "," );                 ADDHEX( 4 );
                ADDTEXT( "," );                 ADDHEX( 4 );
                PRINT();
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, "fAutoCol" );
                ADDFLAG( 0x0002, "fAutoBack" );
                ADDFLAG( 0x0020, "fAutoRot" );
                ADDRESERVED( 0xFFDC );
                PRINT();
                LINESTART();
                ADDTEXT( "color=" );            ADDDEC( 2 );
                ADDTEXT( "   rotation=" );      ADDDEC( 2 );
                PRINT();
            }
                break;
            case 0x101F:        // CHVALUERANGE
            {
                LINESTART();
                ADDTEXT( "min=" );              ADDDOUBLE();
                ADDTEXT( "   max=" );           ADDDOUBLE();
                ADDTEXT( "   major=" );         ADDDOUBLE();
                ADDTEXT( "   minor=" );         ADDDOUBLE();
                ADDTEXT( "   axis-cross=" );    ADDDOUBLE();
                PRINT();
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, "fAutoMin" );
                ADDFLAG( 0x0002, "fAutoMax" );
                ADDFLAG( 0x0004, "fAutoMajor" );
                ADDFLAG( 0x0008, "fAutoMinor" );
                ADDFLAG( 0x0010, "fAutoCross" );
                ADDFLAG( 0x0020, "fLogScale" );
                ADDFLAG( 0x0040, "fReverse" );
                ADDFLAG( 0x0080, "fMaxCross" );
                ADDRESERVED( 0xFF00 );
                PRINT();
            }
                break;
            case 0x1020:        // CHEXTRANGE
            {
                LINESTART();
                ADDTEXT( "axis-cross=" );       ADDDEC( 2 );
                ADDTEXT( "   label-freq=" );    ADDDEC( 2 );
                ADDTEXT( "   mark-freq=" );     ADDDEC( 2 );
                PRINT();
                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, "fBetween" );
                ADDFLAG( 0x0002, "fMaxCross" );
                ADDFLAG( 0x0004, "fReverse" );
                ADDRESERVED( 0xFFF8 );
                PRINT();
            }
                break;
            case 0x1021:        // ChartAxislineformat
            {
                LINESTART();
                switch( Read2( rIn ) )
                {
                    case 0:     p = "axis line itself";     break;
                    case 1:     p = "major grid line";      break;
                    case 2:     p = "minor grid line";      break;
                    case 3:     p = "walls or floor";       break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                PRINT();
            }
                break;
            case 0x1022:        // CHARTFORMATLINK
                ContDump( nL );
                break;
            case 0x1024:        // ChartDefaulttext
                ContDump( nL );
                break;
            case 0x1025:        // ChartText
            {
                LINESTART();
                UINT8   nAt, nVat;
                UINT16  nBkgMode;
                INT32   nX, nY, nDx, nDy;
                UINT16  nGrbit2, nIcvText;
                INT16   nTrot;
                rIn >> nAt >> nVat >> nBkgMode;
                ADDTEXT( "h = " );
                switch( nAt )
                {
                    case 1:     p = "left";         break;
                    case 2:     p = "center";       break;
                    case 3:     p = "right";        break;
                    case 4:     p = "justify";      break;
                    case 5:     p = "distribute";   break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                ADDTEXT( "   v = " );
                switch( nVat )
                {
                    case 1:     p = "top";          break;
                    case 2:     p = "center";       break;
                    case 3:     p = "bottom";       break;
                    case 4:     p = "justify";      break;
                    case 5:     p = "distribute";   break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                ADDTEXT( "   mode = " );
                if( nBkgMode == 1 )
                    ADDTEXT( "transparent" );
                else if( nBkgMode == 2 )
                    ADDTEXT( "opaque" );
                else
                    ADDTEXT( pU );
                PRINT();
                LINESTART();
                ADDTEXT( "color = " );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                rIn >> nX >> nY >> nDx >> nDy >> __nFlags >> nIcvText >> nGrbit2 >> nTrot;
                ADDTEXT( "   pos[size] = " );
                __AddDec( t, nX );
                ADDTEXT( " / " );
                __AddDec( t, nY );
                ADDTEXT( "  [" );
                __AddDec( t, nDx );
                ADDTEXT( " / " );
                __AddDec( t, nDy );
                ADDTEXT( "]" );
                PRINT();
                LINESTART();
                ADDTEXT( "   (pos[size] = " );
                __AddHex( t, nX );
                ADDTEXT( " / " );
                __AddHex( t, nY );
                ADDTEXT( "  [" );
                __AddHex( t, nDx );
                ADDTEXT( " / " );
                __AddHex( t, nDy );
                ADDTEXT( "])" );
                PRINT();
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fAutoColor" );
                    ADDFLAG( 0x0002, "fShowKey" );
                    ADDFLAG( 0x0004, "fShowValue" );
                    ADDFLAG( 0x0008, "fVert" );
                    ADDFLAG( 0x0010, "fAutoText" );
                    ADDFLAG( 0x0020, "fGenerated" );
                    ADDFLAG( 0x0040, "fDeleted" );
                    ADDFLAG( 0x0080, "fAutoMode" );
                    ADDFLAG( 0x0800, "fShLabPct" );
                    ADDFLAG( 0x1000, "fShowPct" );
                    ADDFLAG( 0x2000, "fShowBubbleSizes" );
                    ADDFLAG( 0x4000, "fShowLabel" );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "rot = " );
                switch( __nFlags & 0x0700 )
                {
                    case 0x0000:    p = "no rotation";              break;
                    case 0x0100:    p = "top to bottom, upright";   break;
                    case 0x0200:    p = "90 deg counterclockwise";  break;
                    case 0x0300:    p = "90 deg clockwise";         break;
                    default:        p = pU;
                }
                ADDTEXT( p );
                ADDTEXT( "   trot = " );
                __AddDec( t, nTrot );
                PRINT();
                LINESTART();
                ADDTEXT( "text color = " );
                __AddDec( t, nIcvText );
                PRINT();
                LINESTART();
                ADDTEXT( "placement: " );
                switch( nGrbit2 & 0x000F )
                {
                    case 0:     p = "0 default";        break;
                    case 1:     p = "outside";          break;
                    case 2:     p = "inside";           break;
                    case 3:     p = "center";           break;
                    case 4:     p = "axis";             break;
                    case 5:     p = "above";            break;
                    case 6:     p = "below";            break;
                    case 7:     p = "left";             break;
                    case 8:     p = "right";            break;
                    case 9:     p = "auto";             break;
                    case 10:    p = "moved";            break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                PRINT();
            }
                break;
            case 0x1026:        // ChartFontx
                ContDump( nL );
                break;
            case 0x1027:        // CHOBJECTLINK
            {
                static const sal_Char* const ppcObjLink[] = { 0, "title", "y-axis", "x-axis", "data", "legend", "none", "z-axis" };
                LINESTART();
                sal_uInt16 nObjLink;
                rIn >> nObjLink;
                ADDTEXT( "link=" );             __AddDec( t, nObjLink );
                lcl_AddEnum( t, nObjLink, ppcObjLink, STATIC_TABLE_SIZE( ppcObjLink ) );
                ADDTEXT( "   series=" );        ADDDEC( 2 );
                ADDTEXT( "   point=" );         ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x1032:        // ChartFrame
            {
                LINESTART();
                switch( Read2( rIn ) )
                {
                    case 0:     p = "no border";    break;
                    case 1:
                    case 2:
                    case 3:     p = "reserved";     break;
                    case 4:     p = "with shadow";  break;
                    default:    p = pU;
                }
                ADDTEXT( "Frame type: " );
                ADDTEXT( p );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x01, "fAutoSize" );
                    ADDFLAG( 0x02, "fAutoPosition" );
                    PRINT();
                }
            }
                break;
            case 0x1035:        // ChartPlotarea
                ContDump( nL );
                break;
            case 0x103A:        // Chart3D
                ContDump( nL );
                break;
            case 0x103C:        // ChartPicf
                LINESTART();
                ADDTEXT( "bmp-mode=" );             ADDDEC( 2 );
                ADDTEXT( "   format=" );            ADDDEC( 2 );
                ADDTEXT( "   flags=" );             ADDHEX( 2 );
                ADDTEXT( "   scale=" );             ADDDOUBLE();
                PRINT();
                break;
            case 0x103D:        // ChartDropbar
                ContDump( nL );
                break;
            case 0x103E:        // ChartRadar
                ContDump( nL );
                break;
            case 0x103F:        // ChartSurface
                ContDump( nL );
                break;
            case 0x1040:        // ChartRadararea
                ContDump( nL );
                break;
            case 0x1041:        // ChartAxisparent
            {
                LINESTART();
                switch( Read2( rIn ) )
                {
                    case 0:     p = "main";             break;
                    case 1:     p = "secondary";        break;
                    default:    p = pU;
                }
                ADDTEXT( "Index: " );
                ADDTEXT( p );
                ADDTEXT( "    " );
                ADDDEC(4);
                ADDTEXT( '/' );
                ADDDEC(4);
                ADDTEXT( " [" );
                ADDDEC(4);
                ADDTEXT( '/' );
                ADDDEC(4);
                ADDTEXT( ']' );
                PRINT();
            }
                break;
            case 0x1043:        // ChartLegendxn
                ContDump( nL );
                break;
            case 0x1044:        // ChartShtprops
                ContDump( nL );
                break;
            case 0x1045:        // ChartSertocrt
                ContDump( nL );
                break;
            case 0x1046:        // ChartAxesused
            {
                LINESTART();
                ADDTEXT( "Used : " );
                ADDDEC( 2 );
                PRINT();
            }
                break;
            case 0x1048:        // ChartSbaseref
                ContDump( nL );
                break;
            case 0x104A:        // CHSERPARENT
                LINESTART();
                ADDTEXT( "parent-index=" );     ADDDEC( 2 );
                ADDTEXT( " (one-based)" );
                PRINT();
            break;
            case 0x104B:        // CHSERTRENDLINE
            {
                static const sal_Char* const ppcType[] =
                    { "poynomial", "exponential", "logarithmic", "power", "moving-avg" };
                sal_uInt8 nType;
                rIn >> nType;
                LINESTART();
                ADDTEXT( "line-type=" );        __AddDec( t, nType );
                lcl_AddEnum( t, nType, ppcType, STATIC_TABLE_SIZE( ppcType ) );
                ADDTEXT( "   order=" );         ADDDEC( 1 );
                ADDTEXT( "   intercept=" );     ADDDOUBLE();
                PRINT();
                LINESTART();
                ADDTEXT( "show-equation=" );    ADDDEC( 1 );
                ADDTEXT( "   show-r-sqr=" );    ADDDEC( 1 );
                PRINT();
                LINESTART();
                ADDTEXT( "forecast-for=" );     ADDDOUBLE();
                ADDTEXT( "   forecast-back=" ); ADDDOUBLE();
                PRINT();
            }
            break;
            case 0x104E:        // ChartIfmt
                ContDump( nL );
                break;
            case 0x104F:        // ChartPos
            {
                LINESTART();
                UINT16  nMdTopLt, nMdBotRt;
                INT32   nX, nY, nDx, nDy;
                rIn >> nMdTopLt >> nMdBotRt >> nX >> nY >> nDx >> nDy;
                ADDTEXT( "TopLr = " );
                __AddDec( t, nMdTopLt );
                ADDTEXT( "   BotRt = " );
                __AddDec( t, nMdBotRt );
                PRINT();
                LINESTART();
                __AddDec( t, nX );
                ADDTEXT( " / " );
                __AddDec( t, nY );
                ADDTEXT( "  [" );
                __AddDec( t, nDx );
                ADDTEXT( " / " );
                __AddDec( t, nDy );
                ADDTEXT( "]" );
                PRINT();
            }
                break;
            case 0x1050:        // ChartAlruns
                ContDump( nL );
                break;
            case 0x1051:        // AI
            {
                LINESTART();
                UINT8 n8 = Read1( rIn );
                switch( n8 )
                {
                    case 0:     p = "title or text";        break;
                    case 1:     p = "values";               break;
                    case 2:     p = "categories";           break;
                    case 3:     p = "bubble sizes";         break;
                    default:    p = pU;
                }
                ADDTEXT( "Link index identifier: " );
                ADDTEXT( p );
                if ( p == pU )
                    __AddHex( t, n8 );
                PRINT();
                LINESTART();
                switch( Read1( rIn ) )
                {
                    case 0:     p = "default categories";       break;
                    case 1:     p = "text or value";            break;
                    case 2:     p = "linked to worksheet";      break;
                    case 3:     p = "not used (HaHaHa...)";     break;
                    case 4:     p = "error reported";           break;
                    default:    p = pU;
                }
                ADDTEXT( "Refernce type: " );
                ADDTEXT( p );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x01, "fCustomIfmt" );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "Numberformat = " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                UINT16  n;
                rIn >> n;
                ADDTEXT( "Formula [" );
                __AddDec( t, n );
                ADDTEXT( "]: ---------------- " );
                PRINT();
                FormulaDump( n, FT_CellFormula );
            }
                break;
            case 0x105B:        // CHSERERRORBAR
            {
                static const sal_Char* const ppcType[] = { 0, "x-plus", "x-minus", "y-plus", "y-minus" };
                static const sal_Char* const ppcSource[] = { 0, "percent", "fixed", "std-dev", "custom", "std-error" };
                static const sal_Char* const ppcLineEnd[] = { "blank", "t-shape" };
                sal_uInt8 nType, nSource, nLineEnd;
                rIn >> nType >> nSource >> nLineEnd;
                LINESTART();
                ADDTEXT( "bar-type=" );         __AddDec( t, nType );
                lcl_AddEnum( t, nType, ppcType, STATIC_TABLE_SIZE( ppcType ) );
                ADDTEXT( "   value-source=" );  __AddDec( t, nSource );
                lcl_AddEnum( t, nSource, ppcSource, STATIC_TABLE_SIZE( ppcSource ) );
                ADDTEXT( "   line-end=" );      __AddDec( t, nLineEnd );
                lcl_AddEnum( t, nLineEnd, ppcLineEnd, STATIC_TABLE_SIZE( ppcLineEnd ) );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved=" );         ADDHEX( 1 );
                ADDTEXT( "   value=" );         ADDDOUBLE();
                ADDTEXT( "   ref-count=" );     ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x105D:        // ChartSerfmt
                ContDump( nL );
                break;
            case 0x105F:        // Chart3DDataForm
            {
                UINT8 nGround, nTop;
                nGround = Read1( rIn );
                nTop    = Read1( rIn );
                UINT16 nStyle = ((UINT16)nGround << 8) | nTop;
                LINESTART();
                ADDTEXT( "3D - format (" );
                __AddHex( t, nGround );
                ADDTEXT( " " );
                __AddHex( t, nTop );
                ADDTEXT( "): " );
                switch( nStyle )
                {
                    case 0x0000:    ADDTEXT( "bar" ); break;
                    case 0x0001:    ADDTEXT( "pyramid" ); break;
                    case 0x0002:    ADDTEXT( "pyramid, cut" ); break;
                    case 0x0100:    ADDTEXT( "cylinder" ); break;
                    case 0x0101:    ADDTEXT( "cone" ); break;
                    case 0x0102:    ADDTEXT( "cone, cut" ); break;
                    default:        ADDTEXT( pU );
                }
                PRINT();
            }
                break;
            case 0x1060:        // ChartFbi
            {
                LINESTART();
                ADDTEXT( "dmixBasis = " );
                ADDDEC( 2 );
                ADDTEXT( "  dmiyBasis = " );
                ADDDEC( 2 );
                ADDTEXT( "  twpHeightBasis = " );
                ADDDEC( 2 );
                ADDTEXT( "  scab = " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "ifnt = " );
                ADDDEC( 2 );
                PRINT();
            }
                break;
            case 0x1061:        // ChartBoppop
                ContDump( nL );
                break;
            case 0x1062:        // ChartAxcext
            {
                LINESTART();
                ADDTEXT( "cat on axis: " );
                ADDDEC( 2 );
                ADDTEXT( " ... " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "major unit: val = " );
                ADDDEC( 2 );
                ADDTEXT( "  units = " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "minor unit: val = " );
                ADDDEC( 2 );
                ADDTEXT( "  units = " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "base unit = " );
                ADDDEC( 2 );
                ADDTEXT( "   crossing point of val axis = " );
                ADDDEC( 2 );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, "fAutoMin" );
                    ADDFLAG( 0x0002, "fAutoMax" );
                    ADDFLAG( 0x0004, "fAutoMajor" );
                    ADDFLAG( 0x0008, "fAutoMinor" );
                    ADDFLAG( 0x0010, "fDateAxis" );
                    ADDFLAG( 0x0020, "fAutoBase" );
                    ADDFLAG( 0x0040, "fAutoCross" );
                    ADDFLAG( 0x0080, "fAutoDate" );
                    PRINT();
                }
            }
                break;
            case 0x1063:        // ChartDat
                ContDump( nL );
                break;
            case 0x1064:        // ChartPlotgrowth
            {
                UINT32  nDx, nDy;
                rIn >> nDx >> nDy;
                LINESTART();
                ADDTEXT( "dxPlotGrowth = " );
                __Add16p16( t, nDx );
                ADDTEXT( " (" );
                __AddHex( t, nDx );
                ADDTEXT( ")    dyPlotGrowth = " );
                __Add16p16( t, nDy );
                ADDTEXT( " (" );
                __AddHex( t, nDy );
                ADDTEXT( ")" );
                PRINT();
            }
                break;
            case 0x1065:        // ChartSiindex
                ContDump( nL );
                break;
            case 0x1066:        // ChartGelframe
                EscherDump( nL, false );
                break;
            case 0x1067:        // ChartBoppcustom
                ContDump( nL );
                break;
            default:
                if( !bEncrypted )
                    ContDump( nL );
        }

    }

    if( nR == EXC_ID_FILEPASS )
    {
        bEncrypted = true;
        pIn->Seek( EXC_REC_SEEK_TO_BEGIN );
        bool bValid = (XclImpDecryptHelper::ReadFilepass( *pIn ) == ERRCODE_NONE);
        LINESTART();
        ADDTEXT( "decrypter=" );    lcl_AddOnOff( t, bValid );
        PRINT();
        bBIFF8 = bBIFF8 && bValid;  // dump BIFF8 hex only on invalid decrypter
    }

    if( bDec )
        pLevelPre -= nLevelInc;
}


static const sal_Char* GetBlipType( UINT8 n )
{
    switch ( n )
    {
        case 0 :
            return " ERROR";
        break;
        case 1 :
            return " UNKNOWN";
        break;
        case 2 :
            return " EMF";
        break;
        case 3 :
            return " WMF";
        break;
        case 4 :
            return " PICT";
        break;
        case 5 :
            return " JPEG";
        break;
        case 6 :
            return " PNG";
        break;
        case 7 :
            return " DIB";
        break;
        default:
            if ( n < 32 )
                return " NotKnown";
            else
                return " Client";
    }
}

void Biff8RecDumper::EscherDump( const ULONG nMaxLen, bool bDumpOffset )
{
    ULONG           n = nMaxLen;
    UINT16          nPre, nR;
    UINT32          nL;
    const sal_Char* p;
    ByteString      aT;
    UINT16          nDumpSize;

    aT += pLevelPre;

    ULONG nStartPos = pIn->GetSvStreamPos();
    while( pIn->IsValid() && (n > 0) )
    {
        ULONG nCurrPos = pIn->GetSvStreamPos();
        *pIn >> nPre >> nR >> nL;
        n -= sizeof( nPre ) + sizeof( nR ) + sizeof( nL );

        switch( nR )
        {
            case 0xF000:    p = "MsofbtDggContainer";       break;
            case 0xF006:    p = "MsofbtDgg";                break;
            case 0xF016:    p = "MsofbtCLSID";              break;
            case 0xF00B:    p = "MsofbtOPT";                break;
            case 0xF11A:    p = "MsofbtColorMRU";           break;
            case 0xF11E:    p = "MsofbtSplitMenuColors";    break;
            case 0xF001:    p = "MsofbtBstoreContainer";    break;
            case 0xF007:    p = "MsofbtBSE";                break;
            case 0xF002:    p = "MsofbtDgContainer";        break;
            case 0xF008:    p = "MsofbtDg";                 break;
            case 0xF118:    p = "MsofbtRegroupItem";        break;
            case 0xF120:    p = "MsofbtColorScheme";        break;
            case 0xF003:    p = "MsofbtSpgrContainer";      break;
            case 0xF004:    p = "MsofbtSpContainer";        break;
            case 0xF009:    p = "MsofbtSpgr";               break;
            case 0xF00A:    p = "MsofbtSp";                 break;
            case 0xF00C:    p = "MsofbtTextbox";            break;
            case 0xF00D:    p = "MsofbtClientTextbox";      break;
            case 0xF00E:    p = "MsofbtAnchor";             break;
            case 0xF00F:    p = "MsofbtChildAnchor";        break;
            case 0xF010:    p = "MsofbtClientAnchor";       break;
            case 0xF011:    p = "MsofbtClientData";         break;
            case 0xF11F:    p = "MsofbtOleObject";          break;
            case 0xF11D:    p = "MsofbtDeletedPspl";        break;
            case 0xF005:    p = "MsofbtSolverContainer";    break;
            case 0xF012:    p = "MsofbtConnectorRule";      break;
            case 0xF013:    p = "MsofbtAlignRule";          break;
            case 0xF014:    p = "MsofbtArcRule";            break;
            case 0xF015:    p = "MsofbtClientRule";         break;
            case 0xF017:    p = "MsofbtCalloutRule";        break;
            case 0xF119:    p = "MsofbtSelection";          break;
            case 0xF122:    p = "MsofbtUDefProp";           break;
            default:
                if( nR >= 0xF018 && nR <= 0xF117 )
                    p = "MsofbtBLIP";
                else if ( ( nPre & 0x000F ) == 0x000F )
                    p = "UNKNOWN container";
                else
                    p = "UNKNOWN ID";
        }

        aT += "  ";
        __AddHex( aT, nR );
        ((aT += "  ") += p) += " [";
        __AddHex( aT, nPre );
        aT += ',';
        __AddHex( aT, nL );
        aT += "]  instance: ";
        __AddDec( aT, (UINT16)(nPre >> 4) );
        if( bDumpOffset )
        {
            aT.Append( "   pos=" );
            __AddHex( aT, static_cast< sal_uInt32 >( mnEscherPos + nCurrPos - nStartPos ) );
        }
        Print( aT );

        if ( nR == 0xF007 && 36 <= n && 36 <= nL )
        {   // BSE, FBSE
            ULONG nP = pIn->GetRecPos();
            UINT8 n8;
            UINT16 n16;
            UINT32 n32;

            aT = "    btWin32: ";
            *pIn >> n8;
            __AddHex( aT, n8 );
            aT += GetBlipType( n8 );
            aT += "  btMacOS: ";
            *pIn >> n8;
            __AddHex( aT, n8 );
            aT += GetBlipType( n8 );
            Print( aT );

            aT = "    rgbUid:";
            Print( aT );
            ContDump( 16 );

            aT = "    tag: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            Print( aT );

            aT = "    size: ";
            *pIn >> n32;
            __AddHex( aT, n32 );
            Print( aT );

            aT = "    cRef: ";
            *pIn >> n32;
            __AddHex( aT, n32 );
            Print( aT );

            aT = "    offs: ";
            *pIn >> n32;
            __AddHex( aT, n32 );
            Print( aT );

            aT = "    usage: ";
            *pIn >> n8;
            __AddHex( aT, n8 );
            aT += "  cbName: ";
            *pIn >> n8;
            __AddHex( aT, n8 );
            aT += "  unused2: ";
            *pIn >> n8;
            __AddHex( aT, n8 );
            aT += "  unused3: ";
            *pIn >> n8;
            __AddHex( aT, n8 );
            Print( aT );

            n -= pIn->GetRecPos() - nP;
            nL = 0;     // loop to MsofbtBLIP
        }
        else if ( nR == 0xF00F && 0x10 <= n && 0x10 <= nL )
        {   // ChildAnchor
            ULONG nP = pIn->GetRecPos();
            sal_Int32 n32;

            aT.Assign( "    pos1=" );
            *pIn >> n32;    lclAppendDec( aT, n32 );
            aT.Append( "  pos2=" );
            *pIn >> n32;    lclAppendDec( aT, n32 );
            aT.Append( "  pos3=" );
            *pIn >> n32;    lclAppendDec( aT, n32 );
            aT.Append( "  pos4=" );
            *pIn >> n32;    lclAppendDec( aT, n32 );
            Print( aT );

            ULONG nC = pIn->GetRecPos() - nP;
            n -= nC;
            nL -= nC;
        }
        else if ( nR == 0xF010 && 0x12 <= n && 0x12 <= nL )
        {   // ClientAnchor
            ULONG nP = pIn->GetRecPos();
            UINT16 n16;

            aT = "    Flag: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            if( n16 & 0x0001 ) aT += " -fixedpos";
            if( n16 & 0x0002 ) aT += " -fixedsize";
            Print( aT );

            aT = "    Col1: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            aT += "  dX1: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            aT += "  Row1: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            aT += "  dY1: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            Print( aT );

            aT = "    Col2: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            aT += "  dX2: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            aT += "  Row2: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            aT += "  dY2: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
            Print( aT );

            ULONG nC = pIn->GetRecPos() - nP;
            n -= nC;
            nL -= nC;
        }
        else if ( nR == 0xF00A )
        {
            sal_uInt32 nId, nFlags;
            *pIn >> nId >> nFlags;
            aT.Assign( "    shape-id=" );
            __AddHex( aT, nId );
            aT.Append( "   flags=" );
            __AddHex( aT, nFlags );
            Print( aT );
            nL -= 8; n -= 8;
        }
        else if ( nR == 0xF00B || nR == 0xF122 )
        {   // OPT
            sal_uInt32 nComplex = 0;
            while ( nL >= 6 + nComplex && n >= 6 + nComplex )
            {
                UINT16 n16;
                UINT32 n32;
                *pIn >> n16 >> n32;
                nL -= 6; n -= 6;
                aT = "    ";
                __AddHex( aT, n16 );
                aT += " (";
                __AddDec( aT, n16 & 0x3FFF, 5 );
                if ( (n16 & 0x8000) == 0 )
                {
                    if ( n16 & 0x4000 )
                        aT += ", fBlipID";
                    aT += ")  ";
                    __AddHex( aT, n32 );
                    if ( (n16 & 0x4000) == 0 )
                    {
                        aT += " (";
                        __AddDec1616( aT, n32 );
                        aT += ')';
                    }
                    Print( aT );
                }
                else
                {
                    aT += ", fComplex)  ";
                    __AddHex( aT, n32 );
                    Print( aT );
                    nComplex += n32;
                }
            }
            // complex property data
            while ( nComplex && n > 0 )
            {
                sal_uInt32 nDumpSize = (nComplex > n) ? n : nComplex;
                ContDump( nDumpSize );
                nComplex -= nDumpSize;
                nL -= nDumpSize;
                n -= nDumpSize;
            }
        }
        else if ( nR == 0xF012 )
        {
            aT = "    Connector rule: ";    __AddDec( aT, pIn->ReaduInt32() );
            aT += "   ShapeID A: ";         __AddHex( aT, pIn->ReaduInt32() );
            aT += "   ShapeID B: ";         __AddHex( aT, pIn->ReaduInt32() );
            Print( aT );
            aT = "    ShapeID connector: "; __AddHex( aT, pIn->ReaduInt32() );
            aT += "   Connect pt A: ";      __AddHex( aT, pIn->ReaduInt32() );
            aT += "   Connect pt B: ";      __AddHex( aT, pIn->ReaduInt32() );
            Print( aT );
            nL -= 24; n -= 24;
        }

        if( ( nPre & 0x000F ) == 0x000F && n >= 0 )
        {   // Container
            if ( nL <= (UINT32) n )
                Print( "            completed within" );
            else
                Print( "            continued elsewhere" );
        }
        else if( n >= 0 )
            // -> 0x0000 ... 0x0FFF
        {
            nDumpSize = ( ( UINT32 ) nL > ( UINT32 ) n )? ( UINT16 ) n : ( UINT16 ) nL;

            if( nDumpSize )
            {
                ContDump( nDumpSize );
                n -= nDumpSize;
            }
        }
        else
            Print( " >> OVERRUN <<" );

        aT.Erase();
    }
    if( bDumpOffset )
        mnEscherPos += nMaxLen;
}


void Biff8RecDumper::ObjDump( const ULONG nMaxLen )
{
#if 0
// if an entire hex block is needed
    pIn->PushPosition();
    ContDump( nMaxLen );
    pIn->PopPosition();
#endif

    ULONG           n = nMaxLen;
    UINT16          nR, nL;
    const sal_Char* p;
    ByteString      t;
    XclImpStream&   rIn = *pIn;
    UINT16          nDumpSize;
    sal_uInt16      nObjFlags = 0;

    t += pLevelPre;

    while( n > 0 )
    {
        rIn >> nR >> nL;
        n -= sizeof( nR ) + sizeof( nL );

        BOOL bDetails = FALSE;
        switch( nR )
        {
            case 0x0000:    p = "ftEnd";        break;
            case 0x0001:    p = "(Reserved)";   break;
            case 0x0002:    p = "(Reserved)";   break;
            case 0x0003:    p = "(Reserved)";   break;
            case 0x0004:    p = "ftMacro";      break;
            case 0x0005:    p = "ftButton";     break;
            case 0x0006:    p = "ftGmo";        break;
            case 0x0007:    p = "ftCf";         break;
            case 0x0008:    p = "ftPioGrbit";   bDetails = TRUE;    break;
            case 0x0009:    p = "ftPictFmla";   bDetails = TRUE;    break;
            case 0x000A:    p = "ftCbls";       break;
            case 0x000B:    p = "ftRbo";        break;
            case 0x000C:    p = "ftSbs";        break;
            case 0x000D:    p = "ftNts";        break;
            case 0x000E:    p = "ftSbsFmla";    break;
            case 0x000F:    p = "ftGboData";    break;
            case 0x0010:    p = "ftEdoData";    break;
            case 0x0011:    p = "ftRboData";    break;
            case 0x0012:    p = "ftCblsData";   break;
            case 0x0013:    p = "ftLbsData";    break;
            case 0x0014:    p = "ftCblsFmla";   break;
            case 0x0015:    p = "ftCmo";        bDetails = TRUE;    break;
            default:
                    p = "UNKNOWN ID";
        }

        t += "  ";
        __AddHex( t, nR );
        t += "  [";
        __AddHex( t, nL );
        (t += "]  ") += p;
        Print( t );

        if( n >= 0 )
        {
            nDumpSize = ( ( UINT32 ) nL > ( UINT32 ) n )? ( UINT16 ) n : ( UINT16 ) nL;

            if( nDumpSize )
            {
                ULONG nPos1 = (bDetails ? rIn.GetRecPos() : 0);
                ContDump( nDumpSize );
                n -= nDumpSize;
                if ( bDetails )
                {
                    ULONG nPos2 = rIn.GetRecPos();
                    rIn.Seek( nPos1 );
                    t.Erase();
                    switch ( nR )
                    {
                        case 0x0008 :       // ftPioGrbit
                        {
                            rIn >> nObjFlags;
                            UINT16 __nFlags = nObjFlags;
                            if ( __nFlags )
                            {
                                ADDTEXT( "   " );
                                STARTFLAG();
                                ADDFLAG( 0x0001, "man-size" );
                                ADDFLAG( 0x0002, "linked" );
                                ADDFLAG( 0x0008, "symbol" );
                                ADDFLAG( 0x0010, "control" );
                                ADDFLAG( 0x0020, "ctls-stream" );
                                ADDFLAG( 0x0200, "autoload" );
                                ADDRESERVED( 0xFDC4 );
                            }
                        }
                        break;
                        case 0x0009 :       // ftPictFmla
                        {
                            ADDTEXT( "    Document type " );
                            UINT16 nFmlaLen;
                            rIn >> nFmlaLen;
                            if( nObjFlags & 0x0002 )
                            {
                                ADDTEXT( "linked\n    OLE stream: LNK??? (from EXTERNNAME)   " );
                                rIn >> nFmlaLen;
                                ADDTEXT( "    unknown=" ); ADDHEX( 4 );
                                PRINT();
                                t.Erase();
                                FormulaDump( nFmlaLen, FT_CellFormula );
                            }
                            else
                            {
                                ADDTEXT( "embedded " );
                                const UINT16 nStringOffset = 14;    // MAY be right
                                rIn.Seek( nPos1 + nStringOffset );
                                INT32 nBytesLeft = nL - nStringOffset;
                                UINT16 nStrLen = rIn.ReaduInt16();
                                ULONG nPos3 = rIn.GetRecPos();
                                if( nStrLen )
                                    AddUNICODEString( t, rIn, TRUE, nStrLen );
                                nBytesLeft -= (rIn.GetRecPos() - nPos3);
                                ADDTEXT( '\n' );
                                if ( nBytesLeft < 4 )
                                    ADDTEXT( "    >> ByteString OVERRUN <<\n" );

                                rIn.Seek( nPos1 + sizeof(nFmlaLen) + nFmlaLen );
                                if( nObjFlags & 0x0020 )
                                {
                                    sal_uInt32 nStrmStart, nStrmLen;
                                    rIn >> nStrmStart >> nStrmLen;
                                    ADDTEXT( "    'Ctls' stream   start=" );
                                    __AddHex( t, nStrmStart );
                                    ADDTEXT( "   size=" );
                                    __AddHex( t, nStrmLen );
                                    maCtlsPosMap[ nStrmStart ] = nStrmLen;
                                }
                                else
                                {
                                    ADDTEXT( "    OLE storage name: MBD" );
                                    __AddPureHex( t, rIn.ReaduInt32() );
                                }
                            }
                        }
                        break;
                        case 0x0015 :       // ftCmo
                        {
                            UINT16 nType, nId;
                            rIn >> nType >> nId;
                            ADDTEXT( "    Object ID " );
                            __AddHex( t, nId );
                            switch ( nType )
                            {
                                case 0x0000 :   p = "Group";        break;
                                case 0x0001 :   p = "Line";         break;
                                case 0x0002 :   p = "Rectangle";    break;
                                case 0x0003 :   p = "Oval";         break;
                                case 0x0004 :   p = "Arc";          break;
                                case 0x0005 :   p = "Chart";        break;
                                case 0x0006 :   p = "Text";         break;
                                case 0x0007 :   p = "Button";       break;
                                case 0x0008 :   p = "Picture";      break;
                                case 0x0009 :   p = "Polygon";      break;
                                case 0x000a :   p = "(Reserved)";   break;
                                case 0x000b :   p = "Check box";    break;
                                case 0x000c :   p = "Option button";break;
                                case 0x000d :   p = "Edit box";     break;
                                case 0x000e :   p = "Label";        break;
                                case 0x000f :   p = "Dialog box";   break;
                                case 0x0010 :   p = "Spinner";      break;
                                case 0x0011 :   p = "Scroll bar";   break;
                                case 0x0012 :   p = "List box";     break;
                                case 0x0013 :   p = "Group box";    break;
                                case 0x0014 :   p = "Combo box";    break;
                                case 0x0015 :   p = "(Reserved)";   break;
                                case 0x0016 :   p = "(Reserved)";   break;
                                case 0x0017 :   p = "(Reserved)";   break;
                                case 0x0018 :   p = "(Reserved)";   break;
                                case 0x0019 :   p = "Comment";      break;
                                case 0x001a :   p = "(Reserved)";   break;
                                case 0x001b :   p = "(Reserved)";   break;
                                case 0x001c :   p = "(Reserved)";   break;
                                case 0x001d :   p = "(Reserved)";   break;
                                case 0x001e :   p = "Microsoft Office drawing"; break;
                                default:
                                    p = "UNKNOWN";
                            }
                            ADDTEXT( ", type " );
                            __AddHex( t, nType );
                            ADDTEXT( ' ' );
                            ADDTEXT( p );
                        }
                        break;
                    }
                    if ( t.Len() )
                        PRINT();
                    rIn.Seek( nPos2 );
                }
            }
        }
        else
            Print( " >> OVERRUN <<" );

        t.Erase();
    }
}


#undef  LINESTART
#undef  IGNORE
#undef  ADDHEX
#undef  ADDDEC
#undef  PRINT
#undef  PreDump
#undef  ADDCELLHEAD

void Biff8RecDumper::ContDump( const ULONG nL )
{
    UINT32          nC = nMaxBodyLines;
    UINT32          n = nL;
    UINT32          nInL, nTmp;
    UINT8*          pB = new UINT8[ nL ];
    UINT8*          p;
    const UINT16    nLineLen = 16;
    UINT16          nCharCnt;
    BOOL            bPart;
    ByteString      aT;

    aT += pLevelPre;

    while( n && nC )
    {
        bPart = n < nLineLen;
        nInL = bPart? n : nLineLen;
        n -= nInL;

        pIn->Read( pB, nInL );

        // als Hex-Codes
        nTmp = nInL;
        p = pB;
        nCharCnt = 0;
        while( nTmp )
        {
            if( nCharCnt == nLineLen / 2 )
                aT += ' ';

            nCharCnt++;

            aT += ' ';
            __AddPureHex( aT, *p );
            p++;

            nTmp--;
        }

        if( bPart )
            aT += GetBlanks( ( UINT16 ) ( ( nLineLen - nInL ) * 3 ) );

        // als chars

        aT += "    ";
        if( nInL < 9 )
            aT += ' ';

        nTmp = nInL;
        p = pB;
        nCharCnt = 0;
        while( nTmp )
        {
            if( nCharCnt == nLineLen / 2 )
                aT += ' ';

            nCharCnt++;

            if( IsPrintable( *p ) )
                aT += static_cast< sal_Char >( *p );
            else
                aT += '.';

            p++;

            nTmp--;
        }

        Print( aT );
        aT.Erase();
        aT += pLevelPre;

        nC--;
    }

    delete[] pB;
}


// Formula dumper =============================================================

/** Name and parameter count of an Excel function. */
struct XclDumpFunc
{
    const sal_Char*             pName;          /// Name of the function.
    sal_uInt16                  nParam;         /// Parameter count for fixed functions.
};

static const XclDumpFunc pFuncData[] =
{
/*  0*/ { "COUNT",                  0 },
        { "IF",                     0 },
        { "ISNA",                   1 },
        { "ISERROR",                1 },
        { "SUM",                    0 },
/*  5*/ { "AVERAGE",                0 },
        { "MIN",                    0 },
        { "MAX",                    0 },
        { "ROW",                    0 },
        { "COLUMN",                 0 },
/* 10*/ { "NA",                     0 },
        { "NPV",                    0 },
        { "STDEV",                  0 },
        { "DOLLAR",                 0 },
        { "FIXED",                  0 },
/* 15*/ { "SIN",                    1 },
        { "COS",                    1 },
        { "TAN",                    1 },
        { "ATAN",                   1 },
        { "PI",                     0 },
/* 20*/ { "SQRT",                   1 },
        { "EXP",                    1 },
        { "LN",                     1 },
        { "LOG10",                  1 },
        { "ABS",                    1 },
/* 25*/ { "INT",                    1 },
        { "SIGN",                   1 },
        { "ROUND",                  2 },
        { "LOOKUP",                 0 },
        { "INDEX",                  0 },
/* 30*/ { "REPT",                   2 },
        { "MID",                    3 },
        { "LEN",                    1 },
        { "VALUE",                  1 },
        { "TRUE",                   0 },
/* 35*/ { "FALSE",                  0 },
        { "AND",                    0 },
        { "OR",                     0 },
        { "NOT",                    1 },
        { "MOD",                    2 },
/* 40*/ { "DCOUNT",                 3 },
        { "DSUM",                   3 },
        { "DAVERAGE",               3 },
        { "DMIN",                   3 },
        { "DMAX",                   3 },
/* 45*/ { "DSTDEV",                 3 },
        { "VAR",                    0 },
        { "DVAR",                   3 },
        { "TEXT",                   2 },
        { "LINEST",                 0 },
/* 50*/ { "TREND",                  0 },
        { "LOGEST",                 0 },
        { "GROWTH",                 0 },
        { "GOTO" },                         // macro/internal
        { "HALT" },                         // macro/internal
/* 55*/ { "RETURN" },                       // macro/internal
        { "PV",                     0 },
        { "FV",                     0 },
        { "NPER",                   0 },
        { "PMT",                    0 },
/* 60*/ { "RATE",                   0 },
        { "MIRR",                   3 },
        { "IRR",                    0 },
        { "RAND",                   0 },
        { "MATCH",                  0 },
/* 65*/ { "DATE",                   3 },
        { "TIME",                   3 },
        { "DAY",                    1 },
        { "MONTH",                  1 },
        { "YEAR",                   1 },
/* 70*/ { "WEEKDAY",                0 },
        { "HOUR",                   1 },
        { "MINUTE",                 1 },
        { "SECOND",                 1 },
        { "NOW",                    0 },
/* 75*/ { "AREAS",                  1 },
        { "ROWS",                   1 },
        { "COLUMNS",                1 },
        { "OFFSET",                 0 },
        { "ABSREF" },                       // macro/internal
/* 80*/ { "RELREF" },                       // macro/internal
        { "ARGUMENT" },                     // macro/internal
        { "SEARCH",                 0 },
        { "TRANSPOSE",              1 },
        { "ERROR" },                        // macro/internal
/* 85*/ { "STEP" },                         // macro/internal
        { "TYPE",                   1 },
        { "ECHO" },                         // macro/internal
        { "SET.NAME" },                     // macro/internal
        { "CALLER" },                       // macro/internal
/* 90*/ { "DEREF" },                        // macro/internal
        { "WINDOWS" },                      // macro/internal
        { "SERIES" },                       // macro/internal
        { "DOCUMENTS" },                    // macro/internal
        { "ACTIVE.CELL" },                  // macro/internal
/* 95*/ { "SELECTION" },                    // macro/internal
        { "RESULT" },                       // macro/internal
        { "ATAN2",                  2 },
        { "ASIN",                   1 },
        { "ACOS",                   1 },
/*100*/ { "CHOOSE",                 0 },
        { "HLOOKUP",                0 },
        { "VLOOKUP",                0 },
        { "LINKS" },                        // macro/internal
        { "INPUT" },                        // macro/internal
/*105*/ { "ISREF",                  1 },
        { "GET.FORMULA" },                  // macro/internal
        { "GET.NAME" },                     // macro/internal
        { "SET.VALUE",  2 },                // macro/internal
        { "LOG",                    0 },
/*110*/ { "EXEC" },                         // macro/internal
        { "CHAR",                   1 },
        { "LOWER",                  1 },
        { "UPPER",                  1 },
        { "PROPER",                 1 },
/*115*/ { "LEFT",                   0 },
        { "RIGHT",                  0 },
        { "EXACT",                  2 },
        { "TRIM",                   1 },
        { "REPLACE",                4 },
/*120*/ { "SUBSTITUTE",             0 },
        { "CODE",                   1 },
        { "NAMES" },                        // macro/internal
        { "DIRECTORY" },                    // macro/internal
        { "FIND",                   0 },
/*125*/ { "CELL",                   0 },
        { "ISERR",                  1 },
        { "ISTEXT",                 1 },
        { "ISNUMBER",               1 },
        { "ISBLANK",                1 },
/*130*/ { "T",                      1 },
        { "N",                      1 },
        { "FOPEN" },                        // macro/internal
        { "FCLOSE" },                       // macro/internal
        { "FSIZE" },                        // macro/internal
/*135*/ { "FREADLN" },                      // macro/internal
        { "FREAD" },                        // macro/internal
        { "FWRITELN" },                     // macro/internal
        { "FWRITE" },                       // macro/internal
        { "FPOS" },                         // macro/internal
/*140*/ { "DATEVALUE",              1 },
        { "TIMEVALUE",              1 },
        { "SLN",                    3 },
        { "SYD",                    4 },
        { "DDB",                    0 },
/*145*/ { "GET.DEF" },                      // macro/internal
        { "REFTEXT" },                      // macro/internal
        { "TEXTREF" },                      // macro/internal
        { "INDIRECT",               0 },
        { "REGISTER" },                     // macro/internal
/*150*/ { "CALL" },
        { "ADD.BAR" },                      // macro/internal
        { "ADD.MENU" },                     // macro/internal
        { "ADD.COMMAND" },                  // macro/internal
        { "ENABLE.COMMAND" },               // macro/internal
/*155*/ { "CHECK.COMMAND" },                // macro/internal
        { "RENAME.COMMAND" },               // macro/internal
        { "SHOW.BAR" },                     // macro/internal
        { "DELETE.MENU" },                  // macro/internal
        { "DELETE.COMMAND" },               // macro/internal
/*160*/ { "GET.CHART.ITEM" },               // macro/internal
        { "DIALOG.BOX" },                   // macro/internal
        { "CLEAN",                  1 },
        { "MDETERM",                1 },
        { "MINVERSE",               1 },
/*165*/ { "MMULT",                  2 },
        { "FILES" },                        // macro/internal
        { "IPMT",                   0 },
        { "PPMT",                   0 },
        { "COUNTA",                 0 },
/*170*/ { "CANCEL.KEY" },                   // macro/internal
        { "FOR" },                          // macro/internal
        { "WHILE" },                        // macro/internal
        { "BREAK" },                        // macro/internal
        { "NEXT" },                         // macro/internal
/*175*/ { "INITIATE" },                     // macro/internal
        { "REQUEST" },                      // macro/internal
        { "POKE" },                         // macro/internal
        { "EXECUTE" },                      // macro/internal
        { "TERMINATE" },                    // macro/internal
/*180*/ { "RESTART" },                      // macro/internal
        { "HELP" },                         // macro/internal
        { "GET.BAR" },                      // macro/internal
        { "PRODUCT",                0 },
        { "FACT",                   1 },
/*185*/ { "GET.CELL" },                     // macro/internal
        { "GET.WORKSPACE" },                // macro/internal
        { "GET.WINDOW" },                   // macro/internal
        { "GET.DOCUMENT" },                 // macro/internal
        { "DPRODUCT",               3 },
/*190*/ { "ISNONTEXT",              1 },
        { "GET.NOTE" },                     // macro/internal
        { "NOTE" },                         // macro/internal
        { "STDEVP",                 0 },
        { "VARP",                   0 },
/*195*/ { "DSTDDEVP",               3 },
        { "DVARP",                  3 },
        { "TRUNC",                  0 },
        { "ISLOGICAL",              1 },
        { "DBCOUNTA",               3 },
/*200*/ { "DELETE.BAR" },                   // macro/internal
        { "UNREGISTER" },                   // macro/internal
        { "202" },                          // not used
        { "203" },                          // not used
        { "USDOLLAR" },
/*205*/ { "FINDB" },
        { "SEARCHB" },
        { "REPLACEB",               4 },
        { "LEFTB" },
        { "RIGHTB" },
/*210*/ { "MIDB",                   3 },
        { "LENB",                   1 },
        { "ROUNDUP",                2 },
        { "ROUNDDOWN",              2 },
        { "ASC",                    1 },
/*215*/ { "DBSC",                   1 },
        { "RANK",                   0 },
        { "217" },                          // not used
        { "218" },                          // not used
        { "ADDRESS",                0 },
/*220*/ { "DAYS360",                0 },
        { "TODAY",                  0 },
        { "VDB",                    0 },
        { "ELSE" },                         // macro/internal
        { "ELSE.IF" },                      // macro/internal
/*225*/ { "END.IF" },                       // macro/internal
        { "FOR.CELL" },                     // macro/internal
        { "MEDIAN",                 0 },
        { "SUMPRODUCT",             0 },
        { "SINH",                   1 },
/*230*/ { "COSH",                   1 },
        { "TANH",                   1 },
        { "ASINH",                  1 },
        { "ACOSH",                  1 },
        { "ATANH",                  1 },
/*235*/ { "DGET",                   3 },
        { "CREATE.OBJECT" },                // macro/internal
        { "VOLATILE" },                     // macro/internal
        { "LAST.ERROR" },                   // macro/internal
        { "CUSTOM.UNDO" },                  // macro/internal
/*240*/ { "CUSTOM.REPEAT" },                // macro/internal
        { "FORMULA.CONVERT" },              // macro/internal
        { "GET.LINK.INFO" },                // macro/internal
        { "TEXT.BOX" },                     // macro/internal
        { "INFO",                   1 },
/*245*/ { "GROUP" },                        // macro/internal
        { "GET.OBJECT" },                   // macro/internal
        { "DB",                     0 },
        { "PAUSE" },                        // macro/internal
        { "249" },                          // not used
/*250*/ { "250" },                          // not used
        { "RESUME" },                       // macro/internal
        { "FREQUENCY",              2 },
        { "ADD.TOOLBAR" },                  // macro/internal
        { "DELETE.TOOLBAR" },               // macro/internal
/*255*/ { "EXTERN.CALL" },                  // macro/internal
        { "RESET.TOOLBAR" },                // macro/internal
        { "EVALUATE" },                     // macro/internal
        { "GET.TOOLBAR" },                  // macro/internal
        { "GET.TOOL" },                     // macro/internal
/*260*/ { "SPELLING.CHECK" },               // macro/internal
        { "ERROR.TYPE",             1 },
        { "APP.TITLE" },                    // macro/internal
        { "WINDOW.TITLE" },                 // macro/internal
        { "SAVE.TOOLBAR" },                 // macro/internal
/*265*/ { "ENABLE.TOOL" },                  // macro/internal
        { "PRESS.TOOL" },                   // macro/internal
        { "REGISTER.ID" },                  // macro/internal
        { "GET.WORKBOOK" },                 // macro/internal
        { "AVEDEV",                 0 },
/*270*/ { "BETADIST",               0 },
        { "GAMMALN",                1 },
        { "BETAINV",                0 },
        { "BINOMDIST",              4 },
        { "CHIDIST",                2 },
/*275*/ { "CHIINV",                 2 },
        { "COMBIN",                 2 },
        { "CONFIDENCE",             3 },
        { "CRITBINOM",              3 },
        { "EVEN",                   1 },
/*280*/ { "EXPONDIST",              3 },
        { "FDIST",                  3 },
        { "FINV",                   3 },
        { "FISHER",                 1 },
        { "FISHERINV",              1 },
/*285*/ { "FLOOR",                  2 },
        { "GAMMADIST",              4 },
        { "GAMMAINV",               3 },
        { "CEILING",                2 },
        { "HYPGEOMDIST",            4 },
/*290*/ { "LOGNORMDIST",            3 },
        { "LOGINV",                 3 },
        { "NEGBINOMDIST",           3 },
        { "NORMDIST",               4 },
        { "NORMSDIST",              1 },
/*295*/ { "NORMINV",                3 },
        { "NORMSINV",               1 },
        { "STANDARDIZE",            3 },
        { "ODD",                    1 },
        { "PERMUT",                 2 },
/*300*/ { "POISSON",                3 },
        { "TDIST",                  3 },
        { "WEIBULL",                4 },
        { "SUMXMY2",                2 },
        { "SUMX2MY2",               2 },
/*305*/ { "SUMX2PY2",               2 },
        { "CHITEST",                2 },
        { "CORREL",                 2 },
        { "COVAR",                  2 },
        { "FORECAST",               3 },
/*310*/ { "FTEST",                  2 },
        { "INTERCEPT",              2 },
        { "PEARSON",                2 },
        { "RSQ",                    2 },
        { "STEYX",                  2 },
/*315*/ { "SLOPE",                  2 },
        { "TTEST",                  4 },
        { "PROB",                   0 },
        { "DEVSQ",                  0 },
        { "GEOMEAN",                0 },
/*320*/ { "HARMEAN",                0 },
        { "SUMSQ",                  0 },
        { "KURT",                   0 },
        { "SKEW",                   0 },
        { "ZTEST",                  0 },
/*325*/ { "LARGE",                  2 },
        { "SMALL",                  2 },
        { "QUARTILE",               2 },
        { "PERCENTILE",             2 },
        { "PERCENTRANK",            0 },
/*330*/ { "MODE",                   0 },
        { "TRIMMEAN",               2 },
        { "TINV",                   2 },
        { "333" },                          // not used
        { "MOVIE.COMMAND" },                // macro/internal
/*335*/ { "GET.MOVIE" },                    // macro/internal
        { "CONCATENATE",            0 },
        { "POWER",                  2 },
        { "PIVOT.ADD.DATA" },               // macro/internal
        { "GET.PIVOT.TABLE" },              // macro/internal
/*340*/ { "GET.PIVOT.FIELD" },              // macro/internal
        { "GET.PIVOT.ITEM" },               // macro/internal
        { "RADIANS",                1 },
        { "DEGREES",                1 },
        { "SUBTOTAL",               0 },
/*345*/ { "SUMIF",                  0 },
        { "COUNTIF",                2 },
        { "COUNTBLANK",             1 },
        { "SCENARIO.GET" },                 // macro/internal
        { "OPTIONS.LISTS.GET" },            // macro/internal
/*350*/ { "ISPMT",                  4 },
        { "DATEDIF",                3 },
        { "DATESTRING",             1 },
        { "NUMBERSTRING",           2 },
        { "ROMAN",                  0 },
/*355*/ { "OPEN.DIALOG" },                  // macro/internal
        { "SAVE.DIALOG" },                  // macro/internal
        { "VIEW.GET" },                     // macro/internal
        { "GETPIVOTDATA",           0 },
        { "HYPERLINK",              2 },
/*360*/ { "PHONETIC",               1 },
        { "AVERAGEA",               0 },
        { "MAXA",                   0 },
        { "MINA",                   0 },
        { "STDEVPA",                0 },
/*365*/ { "VARPA",                  0 },
        { "STDEVA",                 0 },
        { "VARA",                   0 },
        { "BAHTTEXT",               1 },
        { "THAIDAYOFWEEK",          1 },
/*370*/ { "THAIDIGIT",              1 },
        { "THAIMONTHOFYEAR",        1 },
        { "THAINUMSOUND",           1 },
        { "THAINUMSTRING",          1 },
        { "THAISTRINGLENGTH",       1 },
/*375*/ { "ISTHAIDIGIT",            1 },
        { "ROUNDBAHTDOWN",          1 },
        { "ROUNDBAHTUP",            1 },
        { "THAIYEAR",               1 },
        { "RTD" },
/*380*/ { "ISHYPERLINK",            1 }
};

const XclDumpFunc* lcl_GetFuncData( sal_uInt16 nIndex )
{
    return (nIndex < STATIC_TABLE_SIZE( pFuncData )) ? (pFuncData + nIndex) : NULL;
}

// ----------------------------------------------------------------------------

/** Stack helper to create a human readable formula string from UPN. */
class XclDumpFormulaStackImpl : private ScfDelStack< ByteString >
{
public:
    void                        PushOperand( const ByteString& rOperand );
    void                        PushUnary( const ByteString& rOperator, bool bInFront );
    void                        PushBinary( const ByteString& rOperator );
    void                        PushFunction( const ByteString& rFuncName, sal_uInt16 nParamCount );
    inline ByteString           GetString() const { return Top() ? *Top() : ByteString(); }
};

void XclDumpFormulaStackImpl::PushOperand( const ByteString& rOperand )
{
    Push( new ByteString( rOperand ) );
}

void XclDumpFormulaStackImpl::PushUnary( const ByteString& rOperator, bool bInFront )
{
    if( !Count() ) return;
    ByteString* pOp = Top();
    pOp->Insert( rOperator, bInFront ? 0 : pOp->Len() );
}

void XclDumpFormulaStackImpl::PushBinary( const ByteString& rOperator )
{
    if( Count() < 2 ) return;
    // second operand is on top
    ByteString* pSecond = Pop();
    ByteString* pFirst = Top();
    *pFirst += rOperator;
    *pFirst += *pSecond;
    delete pSecond;
}

void XclDumpFormulaStackImpl::PushFunction( const ByteString& rFuncName, sal_uInt16 nParamCount )
{
    if( Count() < nParamCount ) return;
    ByteString* pNew = new ByteString( ')' );
    for( sal_uInt16 nIndex = 0; nIndex < nParamCount; ++nIndex )
    {
        if( nIndex ) pNew->Insert( ';', 0 );
        ByteString* pOp = Pop();
        pNew->Insert( *pOp, 0 );
        delete pOp;
    }
    pNew->Insert( '(', 0 );
    pNew->Insert( rFuncName, 0 );
    Push( pNew );
}

// ----------------------------------------------------------------------------

/** Stack to create a human readable formula and token class overview from UPN. */
class XclDumpFormulaStack
{
public:
    void                        PushOperand( const ByteString& rOperand, sal_uInt8 nToken );
    inline void                 PushOperand( const sal_Char* pcOperand, sal_uInt8 nToken )
                                    { PushOperand( ByteString( pcOperand ), nToken ); }

    void                        PushUnary( const ByteString& rOperator, bool bInFront = true );
    inline void                 PushUnary( const sal_Char* pcOperator, bool bInFront = true )
                                    { PushUnary( ByteString( pcOperator ), bInFront ); }

    void                        PushBinary( const ByteString& rOperator );
    inline void                 PushBinary( const sal_Char* pcOperator )
                                    { PushBinary( ByteString( pcOperator ) ); }

    void                        PushFunction( const ByteString& rFuncName, sal_uInt16 nParamCount, sal_uInt8 nToken );
    inline void                 PushFunction( const sal_Char* pcFuncName, sal_uInt16 nParamCount, sal_uInt8 nToken )
                                    { PushFunction( ByteString( pcFuncName ), nParamCount, nToken ); }

    inline ByteString           GetFormula() const { return maFmla.GetString(); }
    inline ByteString           GetTokenClasses() const { return maClass.GetString(); }

    static ByteString           GetTokenClass( sal_uInt8 nToken );

private:
    void                        PushTokenClass( sal_uInt8 nToken );

    XclDumpFormulaStackImpl     maFmla;
    XclDumpFormulaStackImpl     maClass;
};

void XclDumpFormulaStack::PushOperand( const ByteString& rOperand, sal_uInt8 nToken )
{
    maFmla.PushOperand( rOperand );
    maClass.PushOperand( GetTokenClass( nToken ) );
}

void XclDumpFormulaStack::PushUnary( const ByteString& rOperator, bool bInFront )
{
    maFmla.PushUnary( rOperator, bInFront );
    maClass.PushUnary( rOperator, bInFront );
}

void XclDumpFormulaStack::PushBinary( const ByteString& rOperator )
{
    maFmla.PushBinary( rOperator );
    maClass.PushBinary( rOperator );
}

void XclDumpFormulaStack::PushFunction( const ByteString& rFuncName, sal_uInt16 nParamCount, sal_uInt8 nToken )
{
    maFmla.PushFunction( rFuncName, nParamCount );
    maClass.PushFunction( GetTokenClass( nToken ), nParamCount );
}

ByteString XclDumpFormulaStack::GetTokenClass( sal_uInt8 nToken )
{
    sal_Char cClass = 'B';
    switch( nToken & 0xE0 )
    {
        case 0x20:  cClass = 'R';   break;
        case 0x40:  cClass = 'V';   break;
        case 0x60:  cClass = 'A';   break;
    }
    return ByteString( cClass );
}

// ----------------------------------------------------------------------------

const sal_Char* lcl_GetErrorString( sal_uInt8 nErr )
{
    switch( nErr )
    {
        case 0x00:  return "#NULL!";
        case 0x07:  return "#DIV/0!";
        case 0x0F:  return "#VALUE!";
        case 0x17:  return "#REF!";
        case 0x1D:  return "#NAME?";
        case 0x24:  return "#NUM!";
        case 0x2A:  return "#N/A!";
    }
    return "!unknown!";
}



void lcl_StartToken( ByteString& rString, sal_uInt16 nPos, sal_uInt8 nToken, const sal_Char* pTokenName )
{
    rString.Erase();
    rString.Append( "    " );
    __AddHex( rString, nPos );
    rString.Append( "   " );
    __AddHex( rString, nToken );
    rString.Append( " t" ).Append( pTokenName ).Expand( 33, ' ' );
}

void lcl_StartTokenClass( ByteString& rString, sal_uInt16 nPos, sal_uInt8 nToken, const sal_Char* pTokenName )
{
    ByteString aToken( pTokenName );
    aToken.Append( XclDumpFormulaStack::GetTokenClass( nToken ) );
    lcl_StartToken( rString, nPos, nToken, aToken.GetBuffer() );
}

void Biff8RecDumper::FormulaDump( const UINT16 nL, const FORMULA_TYPE eFT )
{
    if( !nL )
        return;

    sal_uInt32              nStartPos = pIn->GetRecPos();
    const sal_uInt32        nAfterPos = nStartPos + nL;
    const sal_Char*         pPre = "    ";
    const sal_Char*         pInfix = "  ";

    BYTE                    nOp;
    ByteString              t, aOperand;
    BOOL                    bError = FALSE;
    const BOOL              bRangeName = eFT == FT_RangeName;
    const BOOL              bSharedFormula = eFT == FT_SharedFormula;
    const BOOL              bRNorSF = bRangeName || bSharedFormula;

    XclDumpFormulaStack         aStack;
    sal_Bool                    bPrinted = sal_True;

#define PRINTTOKEN()            { if( !bPrinted ) Print( t ); bPrinted = sal_True; }
#define STARTTOKEN( name )      lcl_StartToken( t, static_cast< sal_uInt16 >( pIn->GetRecPos() - nStartPos - 1 ), nOp, name )
#define STARTTOKENCLASS( name ) lcl_StartTokenClass( t, static_cast< sal_uInt16 >( pIn->GetRecPos() - nStartPos - 1 ), nOp, name )

    while( pIn->IsValid() && ( pIn->GetRecPos() < nAfterPos ) && !bError )
    {
        *pIn >> nOp;
        bPrinted = sal_False;
        aOperand.Erase();

        switch( nOp )   //                              Buch Seite:
        {           //                                      SDK4 SDK5
            case 0x01: // Array Formula                         [325    ]
            {
                STARTTOKEN( "Exp" );
                sal_uInt16 nRow, nCol;
                *pIn >> nRow >> nCol;
                t += "array formula or shared formula, base-address=";
                lcl_AddRef( t, nCol, nRow );
                aStack.PushFunction( "ARRAY", 0, nOp );
            }
            break;
            case 0x02: // Data Table                            [325 277]
            {
                STARTTOKEN( "Tbl" );
                sal_uInt16 nRow, nCol;
                *pIn >> nRow >> nCol;
                t += "multiple operation, base-address=";
                lcl_AddRef( t, nCol, nRow );
                aStack.PushFunction( "MULTIPLE.OPERATIONS", 0, nOp );
            }
            break;
            case 0x03: // Addition                              [312 264]
                STARTTOKEN( "Add" );
                aStack.PushBinary( "+" );
            break;
            case 0x04: // Subtraction                           [313 264]
                STARTTOKEN( "Sub" );
                aStack.PushBinary( "-" );
            break;
            case 0x05: // Multiplication                        [313 264]
                STARTTOKEN( "Mul" );
                aStack.PushBinary( "*" );
            break;
            case 0x06: // Division                              [313 264]
                STARTTOKEN( "Div" );
                aStack.PushBinary( "/" );
            break;
            case 0x07: // Exponetiation                         [313 265]
                STARTTOKEN( "Power" );
                aStack.PushBinary( "^" );
            break;
            case 0x08: // Concatenation                         [313 265]
                STARTTOKEN( "Concat" );
                aStack.PushBinary( "&" );
            break;
            case 0x09: // Less Than                             [313 265]
                STARTTOKEN( "LT" );
                aStack.PushBinary( "<" );
            break;
            case 0x0A: // Less Than or Equal                    [313 265]
                STARTTOKEN( "LE" );
                aStack.PushBinary( "<=" );
            break;
            case 0x0B: // Equal                                 [313 265]
                STARTTOKEN( "EQ" );
                aStack.PushBinary( "=" );
            break;
            case 0x0C: // Greater Than or Equal                 [313 265]
                STARTTOKEN( "GE" );
                aStack.PushBinary( ">=" );
            break;
            case 0x0D: // Greater Than                          [313 265]
                STARTTOKEN( "GT" );
                aStack.PushBinary( ">" );
            break;
            case 0x0E: // Not Equal                             [313 265]
                STARTTOKEN( "NE" );
                aStack.PushBinary( "<>" );
            break;
            case 0x0F: // Intersection                          [314 265]
                STARTTOKEN( "Isect" );
                aStack.PushBinary( " " );
            break;
            case 0x10: // List                                  [314 265]
                STARTTOKEN( "List" );
                aStack.PushBinary( ";" );
            break;
            case 0x11: // Range                                 [314 265]
                STARTTOKEN( "Range" );
                aStack.PushBinary( ":" );
            break;
            case 0x12: // Unary Plus                            [312 264]
                STARTTOKEN( "Uplus" );
                aStack.PushUnary( "+" );
            break;
            case 0x13: // Unary Minus                           [312 264]
                STARTTOKEN( "Uminus" );
                aStack.PushUnary( "-" );
            break;
            case 0x14: // Percent Sign                          [312 264]
                STARTTOKEN( "Percent" );
                aStack.PushUnary( "%", false );
            break;
            case 0x15: // Parenthesis                           [326 278]
                STARTTOKEN( "Paren" );
                // simulate two unary operators to enclose operand
                aStack.PushUnary( "(" );
                aStack.PushUnary( ")", false );
            break;
            case 0x16: // Missing Argument                      [314 266]
                STARTTOKEN( "MissArg" );
                aStack.PushOperand( "~", nOp );
            break;
            case 0x17: // ByteString Constant                       [314 266]
            {
                STARTTOKEN( "Str" );

                UINT8   nLen;
                *pIn >> nLen;

                t += "string   [len=";
                __AddDec( t, nLen );
                t += "] ";

                aOperand += '"';
                if( nLen )
                    aOperand += GETSTR( pIn->ReadUniString( nLen ) );
                else
                    pIn->Ignore( 1 );
                aOperand += '"';
                t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x18:
            {
                STARTTOKEN( "Extended" );
                UINT8   nEptg;
                *pIn >> nEptg;

#define D(name,size,ext,type)   {t+="eptg ";__AddDec(t,(UINT16)nEptg);t+=": ";              \
                                t+=name;t+=" [";__AddDec(t,(UINT16)size);t+="] ";t+=type;   \
                                if(ext)t+=" + ext";PRINTTOKEN();ContDump(size);aStack.PushOperand(name,nOp);}
                switch( nEptg )
                {                           //  name        size    ext     type
                    case 0x00:              //  res
                        D( "res", 0, 0, "" );
                        break;
                    case 0x01:              //  Lel         4       -       err
                        D( "Lel", 4, 0, "err" );
                        break;
                    case 0x02:              //  Rw          4       -       ref
                        D( "Rw", 4, 0, "ref" );
                        break;
                    case 0x03:              //  Col         4       -       ref
                        D( "Col", 4, 0, "ref" );
                        break;
                    case 0x04:              //  res
                    case 0x05:              //  res
                        D( "res", 0, 0, "" );
                        break;
                    case 0x06:              //  RwV         4       -       val
                        D( "RwV", 4, 0, "val" );
                        break;
                    case 0x07:              //  ColV        4       -       val
                        D( "ColV", 4, 0, "val" );
                        break;
                    case 0x08:              //  res
                    case 0x09:              //  res
                        D( "res", 0, 0, "" );
                        break;
                    case 0x0A:              //  Radical     13      -       ref
                        D( "Radical", 13, 0, "ref" );
                        break;
                    case 0x0B:              //  RadicalS    13      x       ref
                        D( "RadicalS", 13, 1, "ref" );
                        break;
                    case 0x0C:              //  RwS         4       x       ref
                        D( "RwS", 4, 1, "ref" );
                        break;
                    case 0x0D:              //  ColS        4       x       ref
                        D( "ColS", 4, 1, "ref" );
                        break;
                    case 0x0E:              //  RwSV        4       x       val
                        D( "RwSV", 4, 1, "val" );
                        break;
                    case 0x0F:              //  ColSV       4       x       val
                        D( "ColSV", 4, 1, "val" );
                        break;
                    case 0x10:              //  RadicalLel  4       -       err
                        D( "RadicalLel", 4, 0, "err" );
                        break;
                    case 0x11:              //  res
                    case 0x12:              //  res
                    case 0x13:              //  res
                    case 0x14:              //  res
                    case 0x15:              //  res
                    case 0x16:              //  res
                    case 0x17:              //  res
                    case 0x18:              //  res
                        D( "res", 0, 0, "" );
                        break;
                    case 0x19:              //  invalid values
                    case 0x1A:              //  invalid values
                        D( "invalid vals", 0, 0, "" );
                        break;
                    case 0x1B:              //  res
                    case 0x1C:              //  res
                        D( "res", 0, 0, "" );
                        break;
                    case 0x1D:              //  SxName      4       -       val
                        D( "SxName", 4, 0, "val" );
                        break;
                    case 0x1E:              //  res
                        D( "res", 0, 0, "" );
                        break;
                    default:
                        D( "!unknown!", 0, 0, "" );
                }
#undef  D
            }
            break;
            case 0x19: // Special Attribute                     [327 279]
            {
                STARTTOKEN( "Attr" );
                UINT16 nData, nFakt;
                BYTE nOpt;

                *pIn >> nOpt >> nData;
                nFakt = 2;

                t += "flags=";              __AddHex( t, nOpt );

                if( nOpt & 0x01 ) t += " volatile";
                if( nOpt & 0x02 ) t += " if";
                if( nOpt & 0x04 ) t += " choose";
                if( nOpt & 0x08 ) t += " skip";
                if( nOpt & 0x10 ) t += " sum";
                if( nOpt & 0x20 ) t += " assignment";
                if( nOpt & 0x40 ) t += " space";
                if( nOpt & 0x80 ) t += " unknown";

                if( nOpt & 0x02 )
                {
                    t += "   skip-to-false=";
                    __AddHex( t, nData );
                }
                if( nOpt & 0x04 )
                {
                    t += "   count=";
                    __AddDec( t, nData );
                    t += "   skip=";
                    for( sal_uInt16 nIdx = 0; nIdx <= nData; ++nIdx )
                    {
                        if( nIdx ) t += ',';
                        __AddDec( t, pIn->ReaduInt16() );
                    }
                }
                if( nOpt & 0x08 )
                {
                    t += "   skip=";
                    __AddHex( t, nData );
                    t += " (";
                    __AddDec( t, sal_uInt8( nData + 1 ) );
                    t += " bytes)";
                }
                if( nOpt & 0x10 )
                    aStack.PushFunction( "ATTRSUM", 1, nOp );
                if( nOpt & 0x40 )
                {
                    t += "   type=";
                    __AddDec( t, static_cast< sal_uInt8 >( nData ) );
                    t += "   count=";
                    __AddDec( t, static_cast< sal_uInt8 >( nData >> 8 ) );
                }
            }
            break;
            case 0x1C: // Error Value                           [314 266]
            {
                STARTTOKEN( "Err" );
                sal_uInt8 nErr = pIn->ReaduInt8();
                t += "error value (";           __AddHex( t, nErr );
                t += ") ";
                t += lcl_GetErrorString( nErr );
                aStack.PushOperand( lcl_GetErrorString( nErr ), nOp );
            }
            break;
            case 0x1D: // Boolean                               [315 266]
                STARTTOKEN( "Bool" );
                aOperand += pIn->ReaduInt8() ? "TRUE" : "FALSE";
                t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            break;
            case 0x1E: // Integer                               [315 266]
                STARTTOKEN( "Int" );
                __AddDec( aOperand, pIn->ReaduInt16() );
                t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            break;
            case 0x1F: // Number                                [315 266]
                STARTTOKEN( "Num" );
                __AddDouble( aOperand, pIn->ReadDouble() );
                t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            break;
            case 0x20: // Array Constant                        [317 268]
            case 0x40:
            case 0x60:
                STARTTOKENCLASS( "Array" );
                pIn->Ignore( 7 );
                aStack.PushOperand( "{ConstArray}", nOp );
            break;
            case 0x21: // Function, Fixed Number of Arguments   [333 282]
            case 0x41:
            case 0x61:
            {
                STARTTOKENCLASS( "Func" );
                UINT16 nInd = pIn->ReaduInt16();
                const XclDumpFunc* pFunc = lcl_GetFuncData( nInd );
                aOperand += pFunc ? pFunc->pName : "!unknown!";

                t += "fix function: index=";    __AddHex( t, nInd );
                t += " (";                      t += aOperand;
                t += ')';
                aStack.PushFunction( aOperand, pFunc ? pFunc->nParam : 0, nOp );
            }
            break;
            case 0x22: // Function, Variable Number of Arg.     [333 283]
            case 0x42:
            case 0x62:
            {
                STARTTOKENCLASS( "FuncVar" );
                BYTE nAnz;
                UINT16 nInd;
                *pIn >> nAnz >> nInd;
                const XclDumpFunc* pFunc = lcl_GetFuncData( nInd & 0x7FFF );
                aOperand += pFunc ? pFunc->pName : "!unknown!";

                t += "var function: index=";    __AddHex( t, nInd );
                t += " (";                      t += aOperand;
                t += ")   param count=";        __AddHex( t, nAnz );
                t += " (";                      __AddDec( t, (UINT8)(nAnz & 0x7F) );
                t += ')';
                if( nAnz & 0x8000 )
                    t += "   cmd-equiv.";
                aStack.PushFunction( aOperand, nAnz & 0x7F, nOp );
            }
            break;
            case 0x23: // Name                                  [318 269]
            case 0x43:
            case 0x63:
            {
                STARTTOKENCLASS( "Name" );
                sal_uInt16 nNameIdx = pIn->ReaduInt16();
                __AddDec( aOperand, nNameIdx );
                t += "internal name: index=";
                t += aOperand;
                pIn->Ignore( 2 );
                if( (0 < nNameIdx) && (nNameIdx <= maNames.size()) )
                    aOperand = maNames[ nNameIdx - 1 ];
                else
                    aOperand.Insert( "NAME(", 0 ).Append( ')' );
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x24: // Cell Reference                        [319 270]
            case 0x44:
            case 0x64:
            case 0x2A: // Deleted Cell Reference                [323 273]
            case 0x4A:
            case 0x6A:
            {
                if( (nOp & 0x1F) == 0x04 )
                    STARTTOKENCLASS( "Ref" );
                else
                    STARTTOKENCLASS( "RefErr" );

                UINT16 nCol, nRow;
                *pIn >> nRow >> nCol;
                AddRef( aOperand, nRow, nCol, bRangeName );
                t += "2D cell ref   C/R=";      __AddHex( t, nCol );
                t += '/';                       __AddHex( t, nRow );
                t += ' ';                       t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x25: // Area Reference                        [320 270]
            case 0x45:
            case 0x65:
            case 0x2B: // Deleted Area Refernce                 [323 273]
            case 0x4B:
            case 0x6B:
            {
                if( (nOp & 0x1F) == 0x05 )
                    STARTTOKENCLASS( "Area" );
                else
                    STARTTOKENCLASS( "AreaErr" );

                UINT16 nRowFirst, nRowLast, nColFirst, nColLast;
                *pIn >> nRowFirst >> nRowLast >> nColFirst >> nColLast;
                AddRangeRef( aOperand, nRowFirst, nColFirst, nRowLast, nColLast, bRangeName );
                t += "2D area ref   C/R:C/R=";  __AddHex( t, nColFirst );
                t += '/';                       __AddHex( t, nRowFirst );
                t += ':';                       __AddHex( t, nColLast );
                t += '/';                       __AddHex( t, nRowLast );
                t += ' ';                       t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x26: // Constant Reference Subexpression      [321 271]
            case 0x46:
            case 0x66:
            {
                STARTTOKENCLASS( "MemArea" );
                sal_uInt32 nRes;
                sal_uInt16 nSize;
                *pIn >> nRes >> nSize;
                t += "reserved=";               __AddHex( t, nRes );
                t += "   size=";                __AddDec( t, nSize );
            }
            break;
            case 0x27: // Erroneous Constant Reference Subexpr. [322 272]
            case 0x47:
            case 0x67:
            {
                STARTTOKENCLASS( "MemErr" );
                sal_uInt32 nRes;
                sal_uInt16 nSize;
                *pIn >> nRes >> nSize;
                t += "reserved=";               __AddHex( t, nRes );
                t += "   size=";                __AddDec( t, nSize );
            }
            break;
            case 0x28: // Incomplete Constant Reference Subexpr.[331 281]
            case 0x48:
            case 0x68:
            {
                STARTTOKENCLASS( "MemNoMem" );
                sal_uInt32 nRes;
                sal_uInt16 nSize;
                *pIn >> nRes >> nSize;
                t += "reserved=";               __AddHex( t, nRes );
                t += "   size=";                __AddDec( t, nSize );
            }
            break;
            case 0x29: // Variable Reference Subexpression      [331 281]
            case 0x49:
            case 0x69:
            {
                STARTTOKENCLASS( "MemFunc" );
                sal_uInt16 nSize;
                *pIn >> nSize;
                t += "size=";                   __AddDec( t, nSize );
            }
            break;
            case 0x2C: // Cell Reference Within a Name/ShrdFmla [323 273]
            case 0x4C:
            case 0x6C:
            {
                STARTTOKENCLASS( "RefN" );
                UINT16 nRow, nCol;
                *pIn >> nRow >> nCol;
                AddRef( aOperand, nRow, nCol, bRNorSF );
                t += "2D cell ref in name   C/R=";      __AddHex( t, nCol );
                t += '/';                               __AddHex( t, nRow );
                t += ' ';                               t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x2D: // Area Reference Within a Name/ShrdFmla [324 274]
            case 0x4D:
            case 0x6D:
            {
                STARTTOKENCLASS( "AreaN" );
                UINT16 nRowFirst, nRowLast, nColFirst, nColLast;
                *pIn >> nRowFirst >> nRowLast >> nColFirst >> nColLast;
                AddRangeRef( aOperand, nRowFirst, nColFirst, nRowLast, nColLast, bRNorSF );
                t += "2D area ref in name   C/R:C/R";   __AddHex( t, nColFirst );
                t += '/';                               __AddHex( t, nRowFirst );
                t += ':';                               __AddHex( t, nColLast );
                t += '/';                               __AddHex( t, nRowLast );
                t += ' ';                               t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x2E: // Reference Subexpression Within a Name [332 282]
            case 0x4E:
            case 0x6E:
            {
                STARTTOKENCLASS( "MemAreaN" );
                sal_uInt16 nSize;
                *pIn >> nSize;
                t += "size=";                   __AddDec( t, nSize );
            }
            break;
            case 0x2F: // Incomplete Reference Subexpression... [332 282]
            case 0x4F:
            case 0x6F:
            {
                STARTTOKENCLASS( "MemNoMemN" );
                sal_uInt16 nSize;
                *pIn >> nSize;
                t += "size=";                   __AddDec( t, nSize );
            }
            break;
            case 0x39: // Name or External Name                 [    275]
            case 0x59:
            case 0x79:
            {
                STARTTOKENCLASS( "NameX" );
                UINT16 nXti, nName;
                *pIn >> nXti >> nName;
                pIn->Ignore( 2 );
                t += "external name:   XTI=";   __AddDec( t, nXti );
                t += "   name index=";          __AddDec( t, nName );
                aOperand += "EXTNAME(XTI(";
                __AddDec( aOperand, nXti );
                aOperand += "),";
                __AddDec( aOperand, nName );
                aOperand += ')';
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x3A: // 3-D Cell Reference                    [    275]
            case 0x5A:
            case 0x7A:
            case 0x3C: // Deleted 3-D Cell Reference            [    277]
            case 0x5C:
            case 0x7C:
            {
                if( (nOp & 0x1F) == 0x1A )
                    STARTTOKENCLASS( "Ref3d" );
                else
                    STARTTOKENCLASS( "Ref3dErr" );

                UINT16 nXti, nRow, nCol;
                *pIn >> nXti >> nRow >> nCol;
                AddRef( aOperand, nRow, nCol, bRangeName, nXti );
                t += "3D cell ref   Xti!C/R=";  __AddHex( t, nXti );
                t += '!';                       __AddHex( t, nCol );
                t += '/';                       __AddHex( t, nRow );
                t += ' ';                       t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            case 0x3B: // 3-D Area Reference                    [    276]
            case 0x5B:
            case 0x7B:
            case 0x3D: // Deleted 3-D Area Reference            [    277]
            case 0x5D:
            case 0x7D:
            {
                if( (nOp & 0x1F) == 0x1B )
                    STARTTOKENCLASS( "Area3d" );
                else
                    STARTTOKENCLASS( "Area3dErr" );

                UINT16 nXti, nRow1, nCol1, nRow2, nCol2;
                *pIn >> nXti >> nRow1 >> nRow2 >> nCol1 >> nCol2;
                AddRangeRef( aOperand, nRow1, nCol1, nRow2, nCol2, bRangeName, nXti );
                t += "3D area ref   Xti!C/R:C/R=";      __AddHex( t, nXti );
                t += '!';                               __AddHex( t, nCol1 );
                t += '/';                               __AddHex( t, nRow1 );
                t += ':';                               __AddHex( t, nCol2 );
                t += '/';                               __AddHex( t, nRow2 );
                t += ' ';                               t += aOperand;
                aStack.PushOperand( aOperand, nOp );
            }
            break;
            default:
                STARTTOKEN( "unknown" );
                bError = TRUE;
        }
        PRINTTOKEN();
    }
    t.Assign( "  Formula = " );
    if( aStack.GetFormula().Len() ) t += aStack.GetFormula(); else t += "ERROR IN STACK";
    Print( t );
    t.Assign( "  Tokencl = " ).Append( aStack.GetTokenClasses() );
    Print( t );
    pIn->Seek( nAfterPos );
}


// ============================================================================
//
//  S T R E A M   C O N T E N T S
//
// ============================================================================

void Biff8RecDumper::DumpBinary( SvStream& rInStrm, ULONG nSize )
{
    ULONG nStrmPos = rInStrm.Tell();
    rInStrm.Seek( STREAM_SEEK_TO_END );
    ULONG nStrmLen = rInStrm.Tell();
    rInStrm.Seek( nStrmPos );
    ULONG nDumpEnd = (nSize == STREAM_SEEK_TO_END) ? nStrmLen : ::std::min( nStrmPos + nSize, nStrmLen );

    const ULONG LINE_SIZE = 16;
    sal_uInt8 pnData[ LINE_SIZE ];

    while( rInStrm.Tell() < nDumpEnd )
    {
        ByteString aBinLine;
        ByteString aTextLine;

        ULONG nLineLen = ::std::min( nDumpEnd - rInStrm.Tell(), LINE_SIZE );
        rInStrm.Read( pnData, nLineLen );

        for( sal_uInt8 *pnByte = pnData, *pnEnd = pnData + nLineLen; pnByte != pnEnd; ++pnByte )
        {
            if( pnByte - pnData == LINE_SIZE / 2 )
            {
                aBinLine.Append( ' ' );
                aTextLine.Append( ' ' );
            }
            __AddPureHex( aBinLine, *pnByte );
            aBinLine.Append( ' ' );
            aTextLine.Append( static_cast< sal_Char >( IsPrintable( *pnByte ) ? *pnByte : '.' ) );
        }

        aBinLine.Expand( LINE_SIZE * 3 + 3, ' ' );
        (*pDumpStream) << aBinLine.GetBuffer() << aTextLine.GetBuffer() << "\n";
    }
}

// ============================================================================
//
//  F O R M   C O N T R O L S
//
// ============================================================================

namespace {

// little helpers -------------------------------------------------------------

/** Import from bytestream. */
SvStream& operator>>( SvStream& rStrm, XclGuid& rGuid )
{
    rStrm.Read( rGuid.mpnData, 16 );
    return rStrm;
}

/** Output guid into text stream. */
SvStream& operator<<( SvStream& rStrm, const XclGuid& rGuid )
{
    ByteString aOut;
    lclAppendGuid( aOut, rGuid );
    return rStrm << aOut.GetBuffer();
}

void lclAlignStream( SvStream& rInStrm, ULONG nStartPos, ULONG nDataWidth )
{
    rInStrm.SeekRel( nDataWidth - 1 - (rInStrm.Tell() - nStartPos + nDataWidth - 1) % nDataWidth );
}

// control types --------------------------------------------------------------

const sal_uInt16 EXC_CTRL_PAGE          = 0x0007;
const sal_uInt16 EXC_CTRL_IMAGE         = 0x000C;
const sal_uInt16 EXC_CTRL_FRAME         = 0x000E;
const sal_uInt16 EXC_CTRL_SPINBUTTON    = 0x0010;
const sal_uInt16 EXC_CTRL_PUSHBUTTON    = 0x0011;
const sal_uInt16 EXC_CTRL_TABSTRIP      = 0x0012;
const sal_uInt16 EXC_CTRL_LABEL         = 0x0015;
const sal_uInt16 EXC_CTRL_TEXTBOX       = 0x0017;
const sal_uInt16 EXC_CTRL_LISTBOX       = 0x0018;
const sal_uInt16 EXC_CTRL_COMBOBOX      = 0x0019;
const sal_uInt16 EXC_CTRL_CHECKBOX      = 0x001A;
const sal_uInt16 EXC_CTRL_OPTIONBUTTON  = 0x001B;
const sal_uInt16 EXC_CTRL_TOGGLEBUTTON  = 0x001C;
const sal_uInt16 EXC_CTRL_SCROLLBAR     = 0x002F;
const sal_uInt16 EXC_CTRL_MULTIPAGE     = 0x0039;
const sal_uInt16 EXC_CTRL_REFEDIT       = 0x8000;
const sal_uInt16 EXC_CTRL_FONTDATA      = 0xFFF0;   // internal use only
const sal_uInt16 EXC_CTRL_USERFORM      = 0xFFF1;   // internal use only
const sal_uInt16 EXC_CTRL_ADDDATA       = 0xFFF2;   // internal use only
const sal_uInt16 EXC_CTRL_FRAMECHILD    = 0xFFF3;   // internal use only
const sal_uInt16 EXC_CTRL_UNKNOWN       = 0xFFFF;   // internal use only

const sal_uInt16 EXC_CTRL_RECORD_ID     = 0x0000;
const sal_uInt16 EXC_CTRL_CLIENT_ID     = 0x0200;
const sal_uInt16 EXC_CTRL_CONTAINER_ID  = 0x0400;

// control names --------------------------------------------------------------

struct XclDumpControlInfo
{
    sal_uInt16          mnType;
    const sal_Char*     mpcName;
    sal_uInt16          mnId;
};

static const XclDumpControlInfo spControlInfos[] =
{
    { EXC_CTRL_PAGE,            "Page",         EXC_CTRL_CONTAINER_ID },
    { EXC_CTRL_IMAGE,           "Image",        EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_FRAME,           "Frame",        EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_SPINBUTTON,      "Spin",         EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_PUSHBUTTON,      "PushButton",   EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_TABSTRIP,        "TabStrip",     EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_LABEL,           "Label",        EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_TEXTBOX,         "TextBox",      EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_LISTBOX,         "ListBox",      EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_COMBOBOX,        "ComboBox",     EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_CHECKBOX,        "CheckBox",     EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_OPTIONBUTTON,    "OptionButton", EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_TOGGLEBUTTON,    "ToggleButton", EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_SCROLLBAR,       "ScrollBar",    EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_MULTIPAGE,       "MultiPage",    EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_REFEDIT,         "RefEdit",      EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_FONTDATA,        "FontData",     EXC_CTRL_CLIENT_ID    },
    { EXC_CTRL_USERFORM,        "UserForm",     EXC_CTRL_CONTAINER_ID },
    { EXC_CTRL_ADDDATA,         "AddData",      EXC_CTRL_RECORD_ID    },
    { EXC_CTRL_FRAMECHILD,      "FrameChild",   EXC_CTRL_RECORD_ID    }
};

typedef ::std::map< sal_uInt16, const XclDumpControlInfo* > XclDumpControlInfoMap;
typedef ScfRef< XclDumpControlInfoMap >                     XclDumpControlInfoMapRef;

XclDumpControlInfoMapRef lclCreateControlInfoMap()
{
    XclDumpControlInfoMapRef xMap( new XclDumpControlInfoMap );
    for( const XclDumpControlInfo *pIt = spControlInfos, *pEnd = STATIC_TABLE_END( spControlInfos ); pIt != pEnd; ++pIt )
        (*xMap)[ pIt->mnType ] = pIt;
    return xMap;
}

const XclDumpControlInfoMap& lclGetControlInfoMap()
{
    static const XclDumpControlInfoMapRef sxMap = lclCreateControlInfoMap();
    return *sxMap;
}

void lclAppendControlType( ByteString& rStr, sal_uInt16 nCtrlType )
{
    const XclDumpControlInfoMap& rMap = lclGetControlInfoMap();
    XclDumpControlInfoMap::const_iterator aIt = rMap.find( nCtrlType );
    rStr.Append( (aIt == rMap.end()) ? "*UNKNOWN*" : aIt->second->mpcName );
}

void lclDumpControlType( SvStream& rOutStrm, sal_uInt16 nCtrlType )
{
    ByteString aTitle( "type=" );
    lclAppendHex( aTitle, nCtrlType );
    aTitle.Append( " (" );
    lclAppendControlType( aTitle, nCtrlType );
    aTitle.Append( ')' );
    rOutStrm << aTitle.GetBuffer();
}

sal_uInt16 lclDumpControlHeader( SvStream& rInStrm, SvStream& rOutStrm, sal_uInt16 nCtrlType )
{
    lclDumpControlType( rOutStrm, nCtrlType );
    sal_uInt16 nId, nSize;
    rInStrm >> nId >> nSize;
    ByteString aLine( "  id=" );    lclAppendHex( aLine, nId );
    const XclDumpControlInfoMap& rMap = lclGetControlInfoMap();
    XclDumpControlInfoMap::const_iterator aIt = rMap.find( nCtrlType );
    bool bValid = (aIt != rMap.end()) && (aIt->second->mnId == nId);
    aLine.Append( bValid ? " (valid)" : " (invalid)" );
    aLine.Append( "  size=" );      lclAppendHex( aLine, nSize );
    rOutStrm << aLine.GetBuffer() << "\n";
    return nSize;
}

// control GUIDs --------------------------------------------------------------

struct XclDumpControlGuid
{
    sal_uInt16          mnType;
    sal_uInt32          mnGuidData1;
    sal_uInt16          mnGuidData2;
    sal_uInt16          mnGuidData3;
    sal_uInt8           mnGuidData41;
    sal_uInt8           mnGuidData42;
    sal_uInt8           mnGuidData43;
    sal_uInt8           mnGuidData44;
    sal_uInt8           mnGuidData45;
    sal_uInt8           mnGuidData46;
    sal_uInt8           mnGuidData47;
    sal_uInt8           mnGuidData48;
};

static const XclDumpControlGuid spControlGuids[] =
{
    { EXC_CTRL_PUSHBUTTON,      0xD7053240, 0xCE69, 0x11CD, 0xA7, 0x77, 0x00, 0xDD, 0x01, 0x14, 0x3C, 0x57 },
    { EXC_CTRL_TOGGLEBUTTON,    0x8BD21D60, 0xEC42, 0x11CE, 0x9E, 0x0D, 0x00, 0xAA, 0x00, 0x60, 0x02, 0xF3 },
    { EXC_CTRL_CHECKBOX,        0x8BD21D40, 0xEC42, 0x11CE, 0x9E, 0x0D, 0x00, 0xAA, 0x00, 0x60, 0x02, 0xF3 },
    { EXC_CTRL_OPTIONBUTTON,    0x8BD21D50, 0xEC42, 0x11CE, 0x9E, 0x0D, 0x00, 0xAA, 0x00, 0x60, 0x02, 0xF3 },
    { EXC_CTRL_LABEL,           0x978C9E23, 0xD4B0, 0x11CE, 0xBF, 0x2D, 0x00, 0xAA, 0x00, 0x3F, 0x40, 0xD0 },
    { EXC_CTRL_TEXTBOX,         0x8BD21D10, 0xEC42, 0x11CE, 0x9E, 0x0D, 0x00, 0xAA, 0x00, 0x60, 0x02, 0xF3 },
    { EXC_CTRL_LISTBOX,         0x8BD21D20, 0xEC42, 0x11CE, 0x9E, 0x0D, 0x00, 0xAA, 0x00, 0x60, 0x02, 0xF3 },
    { EXC_CTRL_COMBOBOX,        0x8BD21D30, 0xEC42, 0x11CE, 0x9E, 0x0D, 0x00, 0xAA, 0x00, 0x60, 0x02, 0xF3 },
    { EXC_CTRL_SPINBUTTON,      0x79176FB0, 0xB7F2, 0x11CE, 0x97, 0xEF, 0x00, 0xAA, 0x00, 0x6D, 0x27, 0x76 },
    { EXC_CTRL_SCROLLBAR,       0xDFD181E0, 0x5E2F, 0x11CE, 0xA4, 0x49, 0x00, 0xAA, 0x00, 0x4A, 0x80, 0x3D },
    { EXC_CTRL_IMAGE,           0x4C599241, 0x6926, 0x101B, 0x99, 0x92, 0x00, 0x00, 0x0B, 0x65, 0xC6, 0xF9 }
};

typedef ::std::map< XclGuid, sal_uInt16 >   XclDumpControlGuidMap;
typedef ScfRef< XclDumpControlGuidMap >     XclDumpControlGuidMapRef;

XclDumpControlGuidMapRef lclCreateControlGuidMap()
{
    XclDumpControlGuidMapRef xMap( new XclDumpControlGuidMap );
    for( const XclDumpControlGuid *pIt = spControlGuids, *pEnd = STATIC_TABLE_END( spControlGuids ); pIt != pEnd; ++pIt )
    {
        XclGuid aGuid( pIt->mnGuidData1, pIt->mnGuidData2, pIt->mnGuidData3,
            pIt->mnGuidData41, pIt->mnGuidData42, pIt->mnGuidData43, pIt->mnGuidData44,
            pIt->mnGuidData45, pIt->mnGuidData46, pIt->mnGuidData47, pIt->mnGuidData48 );
        (*xMap)[ aGuid ] = pIt->mnType;
    }
    return xMap;
}

const XclDumpControlGuidMap& lclGetControlGuidMap()
{
    static const XclDumpControlGuidMapRef sxMap = lclCreateControlGuidMap();
    return *sxMap;
}

sal_uInt16 lclDumpControlGuid( SvStream& rInStrm, SvStream& rOutStrm )
{
    XclGuid aGuid;
    rInStrm >> aGuid;
    const XclDumpControlGuidMap& rMap = lclGetControlGuidMap();
    XclDumpControlGuidMap::const_iterator aIt = rMap.find( aGuid );
    sal_uInt16 nCtrlType = (aIt == rMap.end()) ? EXC_CTRL_UNKNOWN : aIt->second;
    rOutStrm << "guid=" << aGuid;
    return nCtrlType;
};

// other guids ----------------------------------------------------------------

static const XclGuid saStdFontGuid( 0x0BE35203, 0x8F91, 0x11CE, 0x9D, 0xE3, 0x00, 0xAA, 0x00, 0x4B, 0xB8, 0x51 );
static const XclGuid saStdPicGuid(  0x0BE35204, 0x8F91, 0x11CE, 0x9D, 0xE3, 0x00, 0xAA, 0x00, 0x4B, 0xB8, 0x51 );\

// ----------------------------------------------------------------------------

} // namespace

// *** yet some other ugly macros for the specials of form control dumping ***

// align the instream
#define EXC_CTRLDUMP_ALIGN_INSTRM( val ) lclAlignStream( rInStrm, nStartPos, val )
// push the string to outstream
#define EXC_CTRLDUMP_PRINT() { if( t.Len() ) { rOutStrm << t.GetBuffer() << '\n'; t.Erase(); } }

// implementation, don't use
#define IMPL_EXC_CTRLDUMP_PLAIN_VALUE( type, func, text )    { type n; rInStrm >> n; t.Append( "  " text "=" ); func( t, n ); EXC_CTRLDUMP_PRINT(); }
#define IMPL_EXC_CTRLDUMP_VALUE( type, func, text )          { EXC_CTRLDUMP_ALIGN_INSTRM( sizeof( type ) ); IMPL_EXC_CTRLDUMP_PLAIN_VALUE( type, func, text ); }
#define IMPL_EXC_CTRLDUMP_PLAIN_VAR( var, mask, func, text ) { rInStrm >> var; var &= (mask); t.Append( "  " text "=" ); func( t, var ); EXC_CTRLDUMP_PRINT(); }
#define IMPL_EXC_CTRLDUMP_VAR( var, mask, func, text )       { EXC_CTRLDUMP_ALIGN_INSTRM( sizeof( var ) ); IMPL_EXC_CTRLDUMP_PLAIN_VAR( var, mask, func, text ); }

// read a value from stream (no stream alignment)
#define EXC_CTRLDUMP_PLAIN_HEX4( text ) IMPL_EXC_CTRLDUMP_PLAIN_VALUE( sal_uInt32, lclAppendHex, text )
#define EXC_CTRLDUMP_PLAIN_DEC4( text ) IMPL_EXC_CTRLDUMP_PLAIN_VALUE( sal_Int32,  lclAppendDec, text )
#define EXC_CTRLDUMP_PLAIN_HEX2( text ) IMPL_EXC_CTRLDUMP_PLAIN_VALUE( sal_uInt16, lclAppendHex, text )
#define EXC_CTRLDUMP_PLAIN_DEC2( text ) IMPL_EXC_CTRLDUMP_PLAIN_VALUE( sal_Int16,  lclAppendDec, text )
#define EXC_CTRLDUMP_PLAIN_HEX1( text ) IMPL_EXC_CTRLDUMP_PLAIN_VALUE( sal_uInt8,  lclAppendHex, text )
#define EXC_CTRLDUMP_PLAIN_DEC1( text ) IMPL_EXC_CTRLDUMP_PLAIN_VALUE( sal_Int8,   lclAppendDec, text )
// read a value from stream (with stream alignment)
#define EXC_CTRLDUMP_HEX4( text ) IMPL_EXC_CTRLDUMP_VALUE( sal_uInt32, lclAppendHex, text )
#define EXC_CTRLDUMP_DEC4( text ) IMPL_EXC_CTRLDUMP_VALUE( sal_Int32,  lclAppendDec, text )
#define EXC_CTRLDUMP_HEX2( text ) IMPL_EXC_CTRLDUMP_VALUE( sal_uInt16, lclAppendHex, text )
#define EXC_CTRLDUMP_DEC2( text ) IMPL_EXC_CTRLDUMP_VALUE( sal_Int16,  lclAppendDec, text )
#define EXC_CTRLDUMP_HEX1( text ) IMPL_EXC_CTRLDUMP_VALUE( sal_uInt8,  lclAppendHex, text )
#define EXC_CTRLDUMP_DEC1( text ) IMPL_EXC_CTRLDUMP_VALUE( sal_Int8,   lclAppendDec, text )
// read a value from stream into existing variable (no stream alignment)
#define EXC_CTRLDUMP_PLAIN_HEXVAR( var, text )           IMPL_EXC_CTRLDUMP_PLAIN_VAR( var, ~0,   lclAppendHex, text )
#define EXC_CTRLDUMP_PLAIN_DECVAR( var, text )           IMPL_EXC_CTRLDUMP_PLAIN_VAR( var, ~0,   lclAppendDec, text )
#define EXC_CTRLDUMP_PLAIN_HEXVARMASK( var, mask, text ) IMPL_EXC_CTRLDUMP_PLAIN_VAR( var, mask, lclAppendHex, text )
#define EXC_CTRLDUMP_PLAIN_DECVARMASK( var, mask, text ) IMPL_EXC_CTRLDUMP_PLAIN_VAR( var, mask, lclAppendDec, text )
// read a value from stream into existing variable (with stream alignment)
#define EXC_CTRLDUMP_HEXVAR( var, text )           IMPL_EXC_CTRLDUMP_VAR( var, ~0,   lclAppendHex, text )
#define EXC_CTRLDUMP_DECVAR( var, text )           IMPL_EXC_CTRLDUMP_VAR( var, ~0,   lclAppendDec, text )
#define EXC_CTRLDUMP_HEXVARMASK( var, mask, text ) IMPL_EXC_CTRLDUMP_VAR( var, mask, lclAppendHex, text )
#define EXC_CTRLDUMP_DECVARMASK( var, mask, text ) IMPL_EXC_CTRLDUMP_VAR( var, mask, lclAppendDec, text )
// read flag fields
#define EXC_CTRLDUMP_PLAIN_STARTOPTFLAG( text, doread, defaults )\
{                                                           \
    nFlags = defaults;                                      \
    t.Append( "  " text );                                  \
    if( doread )                                            \
        rInStrm >> nFlags;                                  \
    else                                                    \
        t.Append( "-defaulted" );                           \
    t.Append( '=' );                                        \
    lclAppendHex( t, nFlags );                              \
}
#define EXC_CTRLDUMP_STARTOPTFLAG( text, doread, defaults ) { if( doread ) { EXC_CTRLDUMP_ALIGN_INSTRM( sizeof( nFlags ) ); } EXC_CTRLDUMP_PLAIN_STARTOPTFLAG( text, doread, defaults ) }
#define EXC_CTRLDUMP_PLAIN_STARTFLAG( text )                EXC_CTRLDUMP_PLAIN_STARTOPTFLAG( text, true, 0 )
#define EXC_CTRLDUMP_STARTFLAG( text )                      EXC_CTRLDUMP_STARTOPTFLAG( text, true, 0 )
#define EXC_CTRLDUMP_ADDFLAG( flag, text )                  { if( nFlags & (flag) ) t.Append( " " text ); }
#define EXC_CTRLDUMP_ADDFLAGVALUE( start, width, text )     { sal_uInt32 nValue; ::extract_value( nValue, nFlags, start, width ); t.Append( " " text "=" ); lclAppendDec( t, nValue ); }
#define EXC_CTRLDUMP_ENDFLAG( reserved )                    { if( nFlags & (reserved) ) { t.Append( " ?" ); lclAppendHex( t, static_cast< sal_uInt32 >( nFlags & (reserved) ) ); } EXC_CTRLDUMP_PRINT(); }
// read coordinates
#define EXC_CTRLDUMP_COORD2( text ) { EXC_CTRLDUMP_ALIGN_INSTRM( 4 ); EXC_CTRLDUMP_DEC2( text "-x" ); EXC_CTRLDUMP_DEC2( text "-y" ); }
#define EXC_CTRLDUMP_COORD4( text ) { EXC_CTRLDUMP_DEC4( text "-x" ); EXC_CTRLDUMP_DEC4( text "-y" ); }
#define EXC_CTRLDUMP_SIZE4( text )  { EXC_CTRLDUMP_DEC4( text "-width" ); EXC_CTRLDUMP_DEC4( text "-height" ); }
// read guid
#define EXC_CTRLDUMP_PLAIN_GUID( text ) IMPL_EXC_CTRLDUMP_PLAIN_VALUE( XclGuid, lclAppendGuid, text )
#define EXC_CTRLDUMP_GUID( text )       { EXC_CTRLDUMP_ALIGN_INSTRM( 4 ); EXC_CTRLDUMP_PLAIN_GUID( text ); }
// read control type
#define EXC_CTRLDUMP_CTRLTYPE( var, text )                  \
{                                                           \
    EXC_CTRLDUMP_ALIGN_INSTRM( 2 );                         \
    rInStrm >> var;                                         \
    t.Assign( "  " text "=" ); lclAppendHex( t, var );      \
    t.Append( " (" ); lclAppendControlType( t, var );       \
    t.Append( ')' );                                        \
    EXC_CTRLDUMP_PRINT();                                   \
}
// read character array, add to string, but do not print
#define EXC_CTRLDUMP_RAWSTRING( var )                       \
{                                                           \
    t.Append( "='" );                                       \
    if( var )                                               \
    {                                                       \
        EXC_CTRLDUMP_ALIGN_INSTRM( 4 );                     \
        ULONG nNextPos = rInStrm.Tell() + (var);            \
        if( var > 128 ) var = 128;                          \
        sal_Char pc[ 129 ];                                 \
        rInStrm.Read( pc, var ); pc[ var ] = '\0';          \
        t.Append( pc );                                     \
        rInStrm.Seek( nNextPos );                           \
    }                                                       \
    t.Append( '\'' );                                       \
}
// read a string
#define EXC_CTRLDUMP_STRING( var, text )                    \
{                                                           \
    t.Append( "  " text );                                  \
    EXC_CTRLDUMP_RAWSTRING( var );                          \
    EXC_CTRLDUMP_PRINT();                                   \
}
// read an array of strings
#define EXC_CTRLDUMP_STRINGARRAY( total, count, text )      \
{                                                           \
    ULONG nNextPos = rInStrm.Tell() + (total);              \
    for( sal_uInt32 nIdx = 0; (nIdx < (count)) && (rInStrm.Tell() < nNextPos); ++nIdx )\
    {                                                       \
        EXC_CTRLDUMP_ALIGN_INSTRM( 4 );                     \
        sal_uInt32 nLen;                                    \
        rInStrm >> nLen;                                    \
        nLen &= 0x7FFFFFFF;                                 \
        t.Append( "  " text "[" );                          \
        lclAppendDec( t, nIdx + 1 );                        \
        t.Append( ']' );                                    \
        EXC_CTRLDUMP_RAWSTRING( nLen );                     \
    }                                                       \
    EXC_CTRLDUMP_PRINT();                                   \
    rInStrm.Seek( nNextPos );                               \
}
// read embedded font data
#define EXC_CTRLDUMP_FONT( var, text )                      \
if( var )                                                   \
{                                                           \
    EXC_CTRLDUMP_PRINT();                                   \
    XclGuid aGuid; rInStrm >> aGuid;                        \
    rOutStrm << "embedded-font-guid=" << aGuid;             \
    if( aGuid == saStdFontGuid )                            \
    {                                                       \
        rOutStrm << " (StdFont)\n";                         \
        EXC_CTRLDUMP_PLAIN_HEX1( "unknown1" );              \
        EXC_CTRLDUMP_PLAIN_DEC1( "script-type" );           \
        EXC_CTRLDUMP_PLAIN_HEX1( "unknown2" );              \
        sal_uInt8 nFlags;                                   \
        EXC_CTRLDUMP_PLAIN_STARTFLAG( "font-style-flags" ); \
        EXC_CTRLDUMP_ADDFLAG( 0x02, "italic" );             \
        EXC_CTRLDUMP_ADDFLAG( 0x04, "underline" );          \
        EXC_CTRLDUMP_ADDFLAG( 0x08, "strikeout" );          \
        EXC_CTRLDUMP_ENDFLAG( 0xF1 );                       \
        EXC_CTRLDUMP_PLAIN_DEC2( "font-weight" );           \
        EXC_CTRLDUMP_PLAIN_DEC4( "font-size" );             \
/* font-size := pt*10000 + (1-((pt+1)%3))*2500 */           \
        sal_uInt8 nLen;                                     \
        EXC_CTRLDUMP_PLAIN_DECVAR( nLen, "font-name-len" ); \
        sal_Char* p = new sal_Char[ nLen + 1 ];             \
        rInStrm.Read( p, nLen ); p[ nLen ] = '\0';          \
        t.Append( "  font-name='" ).Append( p ).Append( '\'' );\
        delete [] p;                                        \
        EXC_CTRLDUMP_PRINT();                               \
    }                                                       \
    else                                                    \
        rOutStrm << " (*UNKNOWN*)\n";                       \
}
// read image data
#define EXC_CTRLDUMP_IMAGE( var, text )                     \
if( var )                                                   \
{                                                           \
    EXC_CTRLDUMP_PRINT();                                   \
    XclGuid aGuid; rInStrm >> aGuid;                        \
    rOutStrm << "embedded-" text "-guid=" << aGuid;         \
    if( aGuid == saStdPicGuid )                             \
    {                                                       \
        rOutStrm << " (StdPict)\n";                         \
        EXC_CTRLDUMP_PLAIN_HEX2( "u1" );                    \
        EXC_CTRLDUMP_PLAIN_HEX2( "u2" );                    \
        sal_uInt32 nLen;                                    \
        EXC_CTRLDUMP_PLAIN_DECVAR( nLen, text "-len" );     \
        rInStrm.SeekRel( nLen );                            \
    }                                                       \
    else                                                    \
        rOutStrm << " (*UNKNOWN*)\n";                       \
}
// hex dump remaining or unknown data
#define EXC_CTRLDUMP_REMAINING( nextpos )                   \
{                                                           \
    EXC_CTRLDUMP_ALIGN_INSTRM( 4 );                         \
    if( rInStrm.Tell() < (nextpos) )                        \
    {                                                       \
        rOutStrm << "  unknown-data=\n";                    \
        DumpBinary( rInStrm, ::std::min< ULONG >( (nextpos) - rInStrm.Tell(), 1024 ) );\
    }                                                       \
    rInStrm.Seek( nextpos );                                \
}

// *** macros end ***

void Biff8RecDumper::DumpControlContents( SvStream& rInStrm, sal_uInt16 nCtrlType )
{
    SvStream& rOutStrm = *pDumpStream;

    sal_uInt16 nSize = lclDumpControlHeader( rInStrm, rOutStrm, nCtrlType );
    if( nSize > 0 )
    {
        ULONG nStartPos = rInStrm.Tell();   // for stream alignment macro
        ByteString t;                       // "t" needed for macros
        sal_uInt32 nFlags = 0;              // "nFlags" needed for macros

        bool bHasFontData = false;
        sal_uInt32 nNameLen = 0;
        sal_uInt32 nCaptionLen = 0;
        sal_uInt32 nValueLen = 0;
        sal_uInt32 nGroupNameLen = 0;
        sal_uInt32 nTagLen = 0;
        sal_uInt32 nTipLen = 0;
        sal_uInt32 nCtrlSrcLen = 0;
        sal_uInt16 nPic = 0;
        sal_uInt16 nIcon = 0;
        sal_uInt16 nFont = 0;

        switch( nCtrlType )
        {
            case EXC_CTRL_PUSHBUTTON:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x0001, "forecolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x0002, "backcolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x0004, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x0008, "caption" );
                EXC_CTRLDUMP_ADDFLAG( 0x0010, "picpos" );
                EXC_CTRLDUMP_ADDFLAG( 0x0020, "size" );
                EXC_CTRLDUMP_ADDFLAG( 0x0040, "mouseptr" );
                EXC_CTRLDUMP_ADDFLAG( 0x0080, "pic" );
                EXC_CTRLDUMP_ADDFLAG( 0x0100, "accel" );
                EXC_CTRLDUMP_ADDFLAG( 0x0200, "notakefocus" );
                EXC_CTRLDUMP_ADDFLAG( 0x0400, "icon" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFF800 );
                sal_uInt32 nCtrlFlags = nFlags;

                if( nCtrlFlags & 0x0001 ) EXC_CTRLDUMP_HEX4( "forecolor" );
                if( nCtrlFlags & 0x0002 ) EXC_CTRLDUMP_HEX4( "backcolor" );

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x0004, 0x0000001B );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "enabled" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "locked" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "opaque" );
                EXC_CTRLDUMP_ADDFLAG( 0x00800000, "wordwrap" );
                EXC_CTRLDUMP_ADDFLAG( 0x10000000, "autosize" );
                EXC_CTRLDUMP_ENDFLAG( 0xEF7FFFF1 );

                if( nCtrlFlags & 0x0008 ) EXC_CTRLDUMP_DECVARMASK( nCaptionLen, 0x7FFFFFFF, "caption-len" );
                if( nCtrlFlags & 0x0010 ) EXC_CTRLDUMP_COORD2( "picpos" );
                if( nCtrlFlags & 0x0040 ) EXC_CTRLDUMP_DEC1( "mouseptr" );
                if( nCtrlFlags & 0x0080 ) EXC_CTRLDUMP_HEXVAR( nPic, "pic" );
                if( nCtrlFlags & 0x0100 ) EXC_CTRLDUMP_HEX2( "accel" );
                if( nCtrlFlags & 0x0400 ) EXC_CTRLDUMP_HEXVAR( nIcon, "icon" );

                if( nCtrlFlags & 0x0008 ) EXC_CTRLDUMP_STRING( nCaptionLen, "caption" );
                if( nCtrlFlags & 0x0020 ) EXC_CTRLDUMP_SIZE4( "size" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                EXC_CTRLDUMP_IMAGE( nPic, "pic" );
                EXC_CTRLDUMP_IMAGE( nIcon, "icon" );
                bHasFontData = true;
            }
            break;

            case EXC_CTRL_TOGGLEBUTTON:
            case EXC_CTRL_CHECKBOX:
            case EXC_CTRL_OPTIONBUTTON:
            case EXC_CTRL_TEXTBOX:
            case EXC_CTRL_LISTBOX:
            case EXC_CTRL_COMBOBOX:
            case EXC_CTRL_REFEDIT:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000001, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "backcolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "forecolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "maxlen" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000010, "borderstyle" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000020, "scrollbars" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000040, "style" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000080, "mouseptr" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000100, "size" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000200, "passwordchar" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000400, "listwidth" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000800, "boundcol" );
                EXC_CTRLDUMP_ADDFLAG( 0x00001000, "textcol" );
                EXC_CTRLDUMP_ADDFLAG( 0x00002000, "colcount" );
                EXC_CTRLDUMP_ADDFLAG( 0x00004000, "listrows" );
                EXC_CTRLDUMP_ADDFLAG( 0x00008000, "colwidth?" );
                EXC_CTRLDUMP_ADDFLAG( 0x00010000, "matchentry" );
                EXC_CTRLDUMP_ADDFLAG( 0x00020000, "liststyle" );
                EXC_CTRLDUMP_ADDFLAG( 0x00040000, "showdropbtn" );
                EXC_CTRLDUMP_ADDFLAG( 0x00100000, "dropbtnstyle" );
                EXC_CTRLDUMP_ADDFLAG( 0x00200000, "multistate" );
                EXC_CTRLDUMP_ADDFLAG( 0x00400000, "value" );
                EXC_CTRLDUMP_ADDFLAG( 0x00800000, "caption" );
                EXC_CTRLDUMP_ADDFLAG( 0x01000000, "picpos" );
                EXC_CTRLDUMP_ADDFLAG( 0x02000000, "bordercolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x04000000, "specialeff" );
                EXC_CTRLDUMP_ADDFLAG( 0x08000000, "icon" );
                EXC_CTRLDUMP_ADDFLAG( 0x10000000, "pic" );
                EXC_CTRLDUMP_ADDFLAG( 0x20000000, "accel" );
                EXC_CTRLDUMP_ENDFLAG( 0x40080000 ); // 0x80000000 always set?
                sal_uInt32 nCtrlFlags = nFlags;

                EXC_CTRLDUMP_STARTFLAG( "2nd-content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000001, "groupname" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFFFFE );
                sal_uInt32 nCtrlFlags2 = nFlags;

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x00000001, 0x2C80081B );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "enabled" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "locked" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "opaque" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000400, "colheads" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000800, "intheight" );
                EXC_CTRLDUMP_ADDFLAG( 0x00001000, "matchreq" );
                EXC_CTRLDUMP_ADDFLAG( 0x00002000, "align" );
                EXC_CTRLDUMP_ADDFLAGVALUE( 15, 4, "ime-mode" );
                EXC_CTRLDUMP_ADDFLAG( 0x00080000, "dragbehav" );
                EXC_CTRLDUMP_ADDFLAG( 0x00100000, "enterkeybehav" );
                EXC_CTRLDUMP_ADDFLAG( 0x00200000, "enterfieldbehav" );
                EXC_CTRLDUMP_ADDFLAG( 0x00400000, "tabkeybehav" );
                EXC_CTRLDUMP_ADDFLAG( 0x00800000, "wordwrap" );
                EXC_CTRLDUMP_ADDFLAG( 0x04000000, "selmargin" );
                EXC_CTRLDUMP_ADDFLAG( 0x08000000, "autowordsel" );
                EXC_CTRLDUMP_ADDFLAG( 0x10000000, "autosize" );
                EXC_CTRLDUMP_ADDFLAG( 0x20000000, "hidesel" );
                EXC_CTRLDUMP_ADDFLAG( 0x40000000, "autotab" );
                EXC_CTRLDUMP_ADDFLAG( 0x80000000, "multiline" );
                EXC_CTRLDUMP_ENDFLAG( 0x030043F1 );

                if( nCtrlFlags & 0x00000002 ) EXC_CTRLDUMP_HEX4( "backcolor" );
                if( nCtrlFlags & 0x00000004 ) EXC_CTRLDUMP_HEX4( "forecolor" );
                if( nCtrlFlags & 0x00000008 ) EXC_CTRLDUMP_DEC4( "maxlen" );
                if( nCtrlFlags & 0x00000010 ) EXC_CTRLDUMP_DEC1( "borderstyle" );
                if( nCtrlFlags & 0x00000020 ) EXC_CTRLDUMP_DEC1( "scrollbars" );
                if( nCtrlFlags & 0x00000040 ) EXC_CTRLDUMP_DEC1( "style" );
                if( nCtrlFlags & 0x00000080 ) EXC_CTRLDUMP_DEC1( "mouseptr" );
                if( nCtrlFlags & 0x00000200 ) EXC_CTRLDUMP_HEX2( "passwordchar" );
                if( nCtrlFlags & 0x00000400 ) EXC_CTRLDUMP_DEC4( "listwidth" );
                if( nCtrlFlags & 0x00000800 ) EXC_CTRLDUMP_DEC2( "boundcol" );
                if( nCtrlFlags & 0x00001000 ) EXC_CTRLDUMP_DEC2( "textcol" );
                if( nCtrlFlags & 0x00002000 ) EXC_CTRLDUMP_DEC2( "colcount" );
                if( nCtrlFlags & 0x00004000 ) EXC_CTRLDUMP_DEC2( "listrows" );
                if( nCtrlFlags & 0x00008000 ) EXC_CTRLDUMP_DEC2( "colwidth?" );
                if( nCtrlFlags & 0x00010000 ) EXC_CTRLDUMP_DEC1( "matchentry" );
                if( nCtrlFlags & 0x00020000 ) EXC_CTRLDUMP_DEC1( "liststyle" );
                if( nCtrlFlags & 0x00040000 ) EXC_CTRLDUMP_DEC1( "showdropbtn" );
                if( nCtrlFlags & 0x00100000 ) EXC_CTRLDUMP_DEC1( "dropbtnstyle" );
                if( nCtrlFlags & 0x00200000 ) EXC_CTRLDUMP_DEC1( "multistate" );
                if( nCtrlFlags & 0x00400000 ) EXC_CTRLDUMP_DECVARMASK( nValueLen, 0x7FFFFFFF, "value-len" );
                if( nCtrlFlags & 0x00800000 ) EXC_CTRLDUMP_DECVARMASK( nCaptionLen, 0x7FFFFFFF, "caption-len" );
                if( nCtrlFlags & 0x01000000 ) EXC_CTRLDUMP_COORD2( "picpos" );
                if( nCtrlFlags & 0x02000000 ) EXC_CTRLDUMP_HEX4( "bordercolor" );
                if( nCtrlFlags & 0x04000000 ) EXC_CTRLDUMP_DEC4( "specialeff" );
                if( nCtrlFlags & 0x08000000 ) EXC_CTRLDUMP_HEXVAR( nIcon, "icon" );
                if( nCtrlFlags & 0x10000000 ) EXC_CTRLDUMP_HEXVAR( nPic, "pic" );
                if( nCtrlFlags & 0x20000000 ) EXC_CTRLDUMP_HEX1( "accel" );
                if( nCtrlFlags2 & 0x00000001 ) EXC_CTRLDUMP_DECVARMASK( nGroupNameLen, 0x7FFFFFFF, "groupname-len" );

                if( nCtrlFlags & 0x00000100 ) EXC_CTRLDUMP_SIZE4( "size" );
                if( nCtrlFlags & 0x00400000 ) EXC_CTRLDUMP_STRING( nValueLen, "value" );
                if( nCtrlFlags & 0x00800000 ) EXC_CTRLDUMP_STRING( nCaptionLen, "caption" );
                if( nCtrlFlags2 & 0x00000001 ) EXC_CTRLDUMP_STRING( nGroupNameLen, "groupname" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                EXC_CTRLDUMP_IMAGE( nIcon, "icon" );
                EXC_CTRLDUMP_IMAGE( nPic, "pic" );
                bHasFontData = true;
            }
            break;

            case EXC_CTRL_LABEL:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x0001, "forecolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x0002, "backcolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x0004, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x0008, "caption" );
                EXC_CTRLDUMP_ADDFLAG( 0x0010, "picpos" );
                EXC_CTRLDUMP_ADDFLAG( 0x0020, "size" );
                EXC_CTRLDUMP_ADDFLAG( 0x0040, "mouseptr" );
                EXC_CTRLDUMP_ADDFLAG( 0x0080, "bordercolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x0100, "borderstyle" );
                EXC_CTRLDUMP_ADDFLAG( 0x0200, "specialeff" );
                EXC_CTRLDUMP_ADDFLAG( 0x0400, "pic" );
                EXC_CTRLDUMP_ADDFLAG( 0x0800, "accel" );
                EXC_CTRLDUMP_ADDFLAG( 0x1000, "icon" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFE000 );
                sal_uInt32 nCtrlFlags = nFlags;

                if( nCtrlFlags & 0x0001 ) EXC_CTRLDUMP_HEX4( "forecolor" );
                if( nCtrlFlags & 0x0002 ) EXC_CTRLDUMP_HEX4( "backcolor" );

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x0004, 0x0080001B );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "enabled" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "locked" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "opaque" );
                EXC_CTRLDUMP_ADDFLAG( 0x00800000, "wordwrap" );
                EXC_CTRLDUMP_ADDFLAG( 0x10000000, "autosize" );
                EXC_CTRLDUMP_ENDFLAG( 0xEF7FFFF0 ); // 0x00000001 always set?

                if( nCtrlFlags & 0x0008 ) EXC_CTRLDUMP_DECVARMASK( nCaptionLen, 0x7FFFFFFF, "caption-len" );
                if( nCtrlFlags & 0x0010 ) EXC_CTRLDUMP_COORD2( "picpos" );
                if( nCtrlFlags & 0x0040 ) EXC_CTRLDUMP_DEC1( "mouseptr" );
                if( nCtrlFlags & 0x0080 ) EXC_CTRLDUMP_HEX4( "bordercolor" );
                if( nCtrlFlags & 0x0100 ) EXC_CTRLDUMP_HEX2( "borderstyle" );
                if( nCtrlFlags & 0x0200 ) EXC_CTRLDUMP_HEX2( "specialeff" );
                if( nCtrlFlags & 0x0400 ) EXC_CTRLDUMP_HEXVAR( nPic, "pic" );
                if( nCtrlFlags & 0x0800 ) EXC_CTRLDUMP_HEX2( "accel" );
                if( nCtrlFlags & 0x1000 ) EXC_CTRLDUMP_HEXVAR( nIcon, "icon" );

                if( nCtrlFlags & 0x0008 ) EXC_CTRLDUMP_STRING( nCaptionLen, "caption" );
                if( nCtrlFlags & 0x0020 ) EXC_CTRLDUMP_SIZE4( "size" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                EXC_CTRLDUMP_IMAGE( nPic, "pic" );
                EXC_CTRLDUMP_IMAGE( nIcon, "icon" );
                bHasFontData = true;
            }
            break;

            case EXC_CTRL_SPINBUTTON:
            case EXC_CTRL_SCROLLBAR:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000001, "forecolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "backcolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "size" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000010, "mouseptr" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000100, "unknown1" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000200, "unknown2" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000400, "unknown3" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000020, "min" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000040, "max" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000080, "value" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000800, "step" );
                EXC_CTRLDUMP_ADDFLAG( 0x00001000, "page-step" );
                EXC_CTRLDUMP_ADDFLAG( 0x00002000, "orient" );
                EXC_CTRLDUMP_ADDFLAG( 0x00004000, "prop-thumb" );
                EXC_CTRLDUMP_ADDFLAG( 0x00008000, "delay" );
                EXC_CTRLDUMP_ADDFLAG( 0x00010000, "icon" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFE0000 );
                sal_uInt32 nCtrlFlags = nFlags;

                if( nCtrlFlags & 0x00000001 ) EXC_CTRLDUMP_HEX4( "forecolor" );
                if( nCtrlFlags & 0x00000002 ) EXC_CTRLDUMP_HEX4( "backcolor" );

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x00000004, 0x0000001B );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "enabled" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "locked" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "opaque" );
                EXC_CTRLDUMP_ADDFLAG( 0x10000000, "autosize" );
                EXC_CTRLDUMP_ENDFLAG( 0xEFFFFFF1 );

                if( nCtrlFlags & 0x00000010 ) EXC_CTRLDUMP_DEC1( "mouseptr" );
                if( nCtrlFlags & 0x00000020 ) EXC_CTRLDUMP_DEC4( "min" );
                if( nCtrlFlags & 0x00000040 ) EXC_CTRLDUMP_DEC4( "max" );
                if( nCtrlFlags & 0x00000080 ) EXC_CTRLDUMP_DEC4( "value" );
                if( nCtrlFlags & 0x00000100 ) EXC_CTRLDUMP_HEX4( "unknown1" );
                if( nCtrlFlags & 0x00000200 ) EXC_CTRLDUMP_HEX4( "unknown2" );
                if( nCtrlFlags & 0x00000400 ) EXC_CTRLDUMP_HEX4( "unknown3" );
                if( nCtrlFlags & 0x00000800 ) EXC_CTRLDUMP_DEC4( "step" );
                if( nCtrlFlags & 0x00001000 ) EXC_CTRLDUMP_DEC4( "page-step" );
                if( nCtrlFlags & 0x00002000 ) EXC_CTRLDUMP_DEC4( "orient" );
                if( nCtrlFlags & 0x00004000 ) EXC_CTRLDUMP_DEC4( "prop-thumb" );
                if( nCtrlFlags & 0x00008000 ) EXC_CTRLDUMP_DEC4( "delay" );
                if( nCtrlFlags & 0x00010000 ) EXC_CTRLDUMP_HEXVAR( nIcon, "icon" );

                if( nCtrlFlags & 0x00000008 ) EXC_CTRLDUMP_SIZE4( "size" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                EXC_CTRLDUMP_IMAGE( nIcon, "icon" );
            }
            break;

            case EXC_CTRL_IMAGE:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x0004, "autosize" );
                EXC_CTRLDUMP_ADDFLAG( 0x0008, "bordercolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x0010, "backcolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x0020, "borderstyle" );
                EXC_CTRLDUMP_ADDFLAG( 0x0040, "mouseptr" );
                EXC_CTRLDUMP_ADDFLAG( 0x0080, "picsizemode" );
                EXC_CTRLDUMP_ADDFLAG( 0x0100, "speceffect" );
                EXC_CTRLDUMP_ADDFLAG( 0x0200, "size" );
                EXC_CTRLDUMP_ADDFLAG( 0x0400, "pic" );
                EXC_CTRLDUMP_ADDFLAG( 0x0800, "picalign" );
                EXC_CTRLDUMP_ADDFLAG( 0x1000, "pictiling" );
                EXC_CTRLDUMP_ADDFLAG( 0x2000, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x4000, "icon" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFF8003 );
                sal_uInt32 nCtrlFlags = nFlags;

                if( nCtrlFlags & 0x0008 ) EXC_CTRLDUMP_HEX4( "bordercolor" );
                if( nCtrlFlags & 0x0010 ) EXC_CTRLDUMP_HEX4( "backcolor" );
                if( nCtrlFlags & 0x0020 ) EXC_CTRLDUMP_HEX1( "borderstyle" );
                if( nCtrlFlags & 0x0040 ) EXC_CTRLDUMP_DEC1( "mouseptr" );
                if( nCtrlFlags & 0x0080 ) EXC_CTRLDUMP_DEC1( "picsizemode" );
                if( nCtrlFlags & 0x0100 ) EXC_CTRLDUMP_HEX1( "speceffect" );
                if( nCtrlFlags & 0x0400 ) EXC_CTRLDUMP_HEXVAR( nPic, "pic" );
                if( nCtrlFlags & 0x0800 ) EXC_CTRLDUMP_HEX1( "picalign" );

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x2000, 0x0000001B );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "enabled" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "opaque" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFFFF5 );

                if( nCtrlFlags & 0x4000 ) EXC_CTRLDUMP_HEXVAR( nIcon, "icon" );

                if( nCtrlFlags & 0x0200 ) EXC_CTRLDUMP_SIZE4( "size" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                EXC_CTRLDUMP_IMAGE( nPic, "pic" );
                EXC_CTRLDUMP_IMAGE( nIcon, "icon" );
            }
            break;

            case EXC_CTRL_TABSTRIP:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000001, "selected-tab" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "backcolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "forecolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000010, "size" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000020, "caption-arr-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000040, "mouseptr" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000100, "taborientation" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000200, "tabstyle" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000400, "multirow" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000800, "fixed-width" );
                EXC_CTRLDUMP_ADDFLAG( 0x00001000, "fixed-height" );
                EXC_CTRLDUMP_ADDFLAG( 0x00008000, "infotip-arr-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x00020000, "id-arr-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x00040000, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x00080000, "last-id" );
                EXC_CTRLDUMP_ADDFLAG( 0x00200000, "unknown-arr-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x00400000, "tab-count" );
                EXC_CTRLDUMP_ADDFLAG( 0x00800000, "shortcut-arr-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x01000000, "icon" );
                EXC_CTRLDUMP_ENDFLAG( 0xFE116088 );
                sal_uInt32 nCtrlFlags = nFlags;

                sal_uInt32 nTabCount = 0;
                sal_uInt32 nInfoArrLen = 0;
                sal_uInt32 nIdArrLen = 0;
                sal_uInt32 nUnknownArrLen = 0;
                sal_uInt32 nShortArrLen = 0;

                if( nCtrlFlags & 0x00000001 ) EXC_CTRLDUMP_DEC4( "selected-tab" );      // size ok?
                if( nCtrlFlags & 0x00000002 ) EXC_CTRLDUMP_HEX4( "backcolor" );
                if( nCtrlFlags & 0x00000004 ) EXC_CTRLDUMP_HEX4( "forecolor" );
                if( nCtrlFlags & 0x00000020 ) EXC_CTRLDUMP_HEXVAR( nCaptionLen, "caption-arr-len" );
                if( nCtrlFlags & 0x00000040 ) EXC_CTRLDUMP_DEC1( "mouseptr" );          // size ok?
                if( nCtrlFlags & 0x00000100 ) EXC_CTRLDUMP_DEC4( "taborientation" );    // size ok?
                if( nCtrlFlags & 0x00000200 ) EXC_CTRLDUMP_DEC4( "tabstyle" );          // size ok?
                if( nCtrlFlags & 0x00000800 ) EXC_CTRLDUMP_DEC4( "fixed-width" );
                if( nCtrlFlags & 0x00001000 ) EXC_CTRLDUMP_DEC4( "fixed-height" );
                if( nCtrlFlags & 0x00008000 ) EXC_CTRLDUMP_HEXVAR( nTipLen, "infotip-arr-len" );
                if( nCtrlFlags & 0x00020000 ) EXC_CTRLDUMP_HEXVAR( nIdArrLen, "id-arr-len" );

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x00040000, 0x0000001B );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "enabled" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFFFFD );

                if( nCtrlFlags & 0x00080000 ) EXC_CTRLDUMP_DEC4( "last-id" );
                if( nCtrlFlags & 0x00200000 ) EXC_CTRLDUMP_HEXVAR( nUnknownArrLen, "unknown-arr-len" );
                if( nCtrlFlags & 0x00400000 ) EXC_CTRLDUMP_DECVAR( nTabCount, "tab-count" );
                if( nCtrlFlags & 0x00800000 ) EXC_CTRLDUMP_HEXVAR( nShortArrLen, "shortcut-arr-len" );
                if( nCtrlFlags & 0x01000000 ) EXC_CTRLDUMP_HEXVAR( nIcon, "icon" );

                if( nCtrlFlags & 0x00000010 ) EXC_CTRLDUMP_SIZE4( "size" );
                if( nCtrlFlags & 0x00000020 ) EXC_CTRLDUMP_STRINGARRAY( nCaptionLen, nTabCount, "caption" );
                if( nCtrlFlags & 0x00008000 ) EXC_CTRLDUMP_STRINGARRAY( nTipLen, nTabCount, "infotip" );
                if( nCtrlFlags & 0x00020000 ) EXC_CTRLDUMP_STRINGARRAY( nIdArrLen, nTabCount, "id" );
                if( nCtrlFlags & 0x00200000 ) EXC_CTRLDUMP_STRINGARRAY( nUnknownArrLen, nTabCount, "unknown" );
                if( nCtrlFlags & 0x00800000 ) EXC_CTRLDUMP_STRINGARRAY( nShortArrLen, nTabCount, "shortcut" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                EXC_CTRLDUMP_IMAGE( nIcon, "icon" );
                bHasFontData = true;
            }
            break;

            case EXC_CTRL_USERFORM:
            case EXC_CTRL_FRAME:
            case EXC_CTRL_MULTIPAGE:
            case EXC_CTRL_PAGE:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "backcolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000004, "forecolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000008, "last-id" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000040, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000080, "borderstyle" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000100, "mouseptr" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000200, "scrollbars" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000400, "size" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000800, "scrollsize" );
                EXC_CTRLDUMP_ADDFLAG( 0x00001000, "scrollpos" );
                EXC_CTRLDUMP_ADDFLAG( 0x00008000, "icon" );
                EXC_CTRLDUMP_ADDFLAG( 0x00010000, "cycle" );
                EXC_CTRLDUMP_ADDFLAG( 0x00020000, "speceffect" );
                EXC_CTRLDUMP_ADDFLAG( 0x00040000, "bordercolor" );
                EXC_CTRLDUMP_ADDFLAG( 0x00080000, "caption" );
                EXC_CTRLDUMP_ADDFLAG( 0x00100000, "font" );
                EXC_CTRLDUMP_ADDFLAG( 0x00200000, "pic" );
                EXC_CTRLDUMP_ADDFLAG( 0x00400000, "zoom" );
                EXC_CTRLDUMP_ADDFLAG( 0x00800000, "picalign" );
                EXC_CTRLDUMP_ADDFLAG( 0x01000000, "pictiling" );
                EXC_CTRLDUMP_ADDFLAG( 0x02000000, "picsizemode" );
                EXC_CTRLDUMP_ADDFLAG( 0x04000000, "typeinfover" );
                EXC_CTRLDUMP_ADDFLAG( 0x08000000, "drawbuffer" );
                EXC_CTRLDUMP_ENDFLAG( 0xF0006031 );
                sal_uInt32 nCtrlFlags = nFlags;

                if( nCtrlFlags & 0x00000002 ) EXC_CTRLDUMP_HEX4( "backcolor" );
                if( nCtrlFlags & 0x00000004 ) EXC_CTRLDUMP_HEX4( "forecolor" );
                if( nCtrlFlags & 0x00000008 ) EXC_CTRLDUMP_DEC4( "last-id" );

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x00000040, 0x00000002 );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "enabled" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFFFFD );

                if( nCtrlFlags & 0x00000080 ) EXC_CTRLDUMP_HEX1( "borderstyle" );
                if( nCtrlFlags & 0x00000100 ) EXC_CTRLDUMP_DEC1( "mouseptr" );
                if( nCtrlFlags & 0x00000200 ) EXC_CTRLDUMP_HEX1( "scrollbars" );
                if( nCtrlFlags & 0x00008000 ) EXC_CTRLDUMP_HEXVAR( nIcon, "icon" );
                if( nCtrlFlags & 0x00010000 ) EXC_CTRLDUMP_DEC1( "cycle" );
                if( nCtrlFlags & 0x00020000 ) EXC_CTRLDUMP_HEX1( "speceffect" );
                if( nCtrlFlags & 0x00040000 ) EXC_CTRLDUMP_HEX4( "bordercolor" );
                if( nCtrlFlags & 0x00080000 ) EXC_CTRLDUMP_DECVARMASK( nCaptionLen, 0x7FFFFFFF, "caption-len" );
                if( nCtrlFlags & 0x00100000 ) EXC_CTRLDUMP_HEXVAR( nFont, "font" );
                if( nCtrlFlags & 0x00200000 ) EXC_CTRLDUMP_HEXVAR( nPic, "pic" );
                if( nCtrlFlags & 0x00400000 ) EXC_CTRLDUMP_DEC4( "zoom" );
                if( nCtrlFlags & 0x00800000 ) EXC_CTRLDUMP_HEX1( "picalign" );
                if( nCtrlFlags & 0x02000000 ) EXC_CTRLDUMP_DEC1( "picsizemode" );
                if( nCtrlFlags & 0x04000000 ) EXC_CTRLDUMP_DEC4( "typeinfover" );
                if( nCtrlFlags & 0x08000000 ) EXC_CTRLDUMP_DEC4( "drawbuffer" );

                if( nCtrlFlags & 0x00000400 ) EXC_CTRLDUMP_SIZE4( "size" );
                if( nCtrlFlags & 0x00000800 ) EXC_CTRLDUMP_SIZE4( "scrollsize" );
                if( nCtrlFlags & 0x00001000 ) EXC_CTRLDUMP_COORD4( "scrollpos" );
                if( nCtrlFlags & 0x00080000 ) EXC_CTRLDUMP_STRING( nCaptionLen, "caption" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                EXC_CTRLDUMP_FONT( nFont, "font" );
                EXC_CTRLDUMP_IMAGE( nIcon, "icon" );
                EXC_CTRLDUMP_IMAGE( nPic, "pic" );
            }
            break;

            case EXC_CTRL_FONTDATA:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x01, "font-name" );
                EXC_CTRLDUMP_ADDFLAG( 0x02, "font-style" );
                EXC_CTRLDUMP_ADDFLAG( 0x04, "font-size" );
                EXC_CTRLDUMP_ADDFLAG( 0x10, "language-id" );
                EXC_CTRLDUMP_ADDFLAG( 0x40, "align" );
                EXC_CTRLDUMP_ADDFLAG( 0x80, "font-weight" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFFF08 ); // 0x20 always set?
                sal_uInt32 nCtrlFlags = nFlags;

                if( nCtrlFlags & 0x0001 ) EXC_CTRLDUMP_DECVARMASK( nNameLen, 0x7FFFFFFF, "font-name-len" );

                EXC_CTRLDUMP_STARTOPTFLAG( "font-style-flags", nCtrlFlags & 0x0002, 0x40000000 );
                EXC_CTRLDUMP_ADDFLAG( 0x0001, "bold" );
                EXC_CTRLDUMP_ADDFLAG( 0x0002, "italic" );
                EXC_CTRLDUMP_ADDFLAG( 0x0004, "underline" );
                EXC_CTRLDUMP_ADDFLAG( 0x0008, "strikeout" );
                EXC_CTRLDUMP_ADDFLAG( 0x2000, "sunken" );
                EXC_CTRLDUMP_ENDFLAG( 0xBFFFDFF0 ); // 0x40000000 always set?

                if( nCtrlFlags & 0x0004 ) EXC_CTRLDUMP_DEC4( "font-size" );
                if( nCtrlFlags & 0x0010 ) EXC_CTRLDUMP_HEX2( "language-id" );
                if( nCtrlFlags & 0x0040 )
                {
                    EXC_CTRLDUMP_ALIGN_INSTRM( 2 );
                    sal_uInt16 nAlign; rInStrm >> nAlign;
                    t += "  align="; lclAppendDec( t, nAlign );
                    switch( nAlign )
                    {
                        case 1: t += "=left";   break;
                        case 2: t += "=right";  break;
                        case 3: t += "=center"; break;
                        default: t += "=!unknown!";
                    }
                    EXC_CTRLDUMP_PRINT();
                }
                if( nCtrlFlags & 0x0080 ) EXC_CTRLDUMP_DEC2( "font-weight" );

                if( nCtrlFlags & 0x0001 ) EXC_CTRLDUMP_STRING( nNameLen, "font-name" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );
            }
            break;

            case EXC_CTRL_ADDDATA:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x0001, "guid1" );
                EXC_CTRLDUMP_ADDFLAG( 0x0002, "guid2" );
                EXC_CTRLDUMP_ADDFLAG( 0x0008, "guid4" );
                EXC_CTRLDUMP_ADDFLAG( 0x0040, "unknown1" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFFFB4 );
                sal_uInt32 nCtrlFlags = nFlags;

                if( nCtrlFlags & 0x0040 ) EXC_CTRLDUMP_HEX4( "unknown1" );

                if( nCtrlFlags & 0x0001 ) EXC_CTRLDUMP_GUID( "guid1" );
                if( nCtrlFlags & 0x0002 ) EXC_CTRLDUMP_GUID( "guid2" );
                if( nCtrlFlags & 0x0008 ) EXC_CTRLDUMP_GUID( "guid4" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );
            }
            break;

            case EXC_CTRL_FRAMECHILD:
            {
                EXC_CTRLDUMP_STARTFLAG( "content-flags" );
                EXC_CTRLDUMP_ADDFLAG( 0x0001, "name-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x0002, "tag-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x0004, "storage-id" );
                EXC_CTRLDUMP_ADDFLAG( 0x0008, "helpcontext-id" );
                EXC_CTRLDUMP_ADDFLAG( 0x0010, "option" );
                EXC_CTRLDUMP_ADDFLAG( 0x0020, "substream-len" );
                EXC_CTRLDUMP_ADDFLAG( 0x0040, "tabpos" );
                EXC_CTRLDUMP_ADDFLAG( 0x0080, "type" );
                EXC_CTRLDUMP_ADDFLAG( 0x0100, "pos" );
                EXC_CTRLDUMP_ADDFLAG( 0x0800, "tiptext" );
                EXC_CTRLDUMP_ADDFLAG( 0x2000, "ctrl-source" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFFD600 );
                sal_uInt32 nCtrlFlags = nFlags;

                sal_uInt32 nStorageId = 0;
                sal_uInt32 nSubStrmLen = 0;
                sal_uInt16 nChildType = EXC_CTRL_UNKNOWN;

                if( nCtrlFlags & 0x0001 ) EXC_CTRLDUMP_DECVARMASK( nNameLen, 0x7FFFFFFF, "name-len" );
                if( nCtrlFlags & 0x0002 ) EXC_CTRLDUMP_DECVARMASK( nTagLen, 0x7FFFFFFF, "tag-len" );
                if( nCtrlFlags & 0x0004 ) EXC_CTRLDUMP_PLAIN_DECVAR( nStorageId, "storage-id" );
                if( nCtrlFlags & 0x0008 ) EXC_CTRLDUMP_PLAIN_DEC4( "helpcontext-id" );

                EXC_CTRLDUMP_STARTOPTFLAG( "option-flags", nCtrlFlags & 0x0010, 0x00000033 );
                EXC_CTRLDUMP_ADDFLAG( 0x00000001, "tabstop" );
                EXC_CTRLDUMP_ADDFLAG( 0x00000002, "visible" );
                EXC_CTRLDUMP_ADDFLAG( 0x00040000, "container" );
                EXC_CTRLDUMP_ENDFLAG( 0xFFFBFFFC );

                if( nCtrlFlags & 0x0020 ) EXC_CTRLDUMP_HEXVAR( nSubStrmLen, "substream-len" );
                if( nCtrlFlags & 0x0040 ) EXC_CTRLDUMP_DEC2( "tabpos" );
                if( nCtrlFlags & 0x0080 ) EXC_CTRLDUMP_CTRLTYPE( nChildType, "type" );
                if( nCtrlFlags & 0x0800 ) EXC_CTRLDUMP_DECVARMASK( nTipLen, 0x7FFFFFFF, "infotip-len" );
                if( nCtrlFlags & 0x2000 ) EXC_CTRLDUMP_DECVARMASK( nCtrlSrcLen, 0x7FFFFFFF, "ctrl-source-len" );

                if( nCtrlFlags & 0x0001 ) EXC_CTRLDUMP_STRING( nNameLen, "name" );
                if( nCtrlFlags & 0x0002 ) EXC_CTRLDUMP_STRING( nTagLen, "tag" );
                if( nCtrlFlags & 0x0100 ) EXC_CTRLDUMP_COORD4( "pos" );
                if( nCtrlFlags & 0x0800 ) EXC_CTRLDUMP_STRING( nTipLen, "infotip" );
                if( nCtrlFlags & 0x2000 ) EXC_CTRLDUMP_STRING( nCtrlSrcLen, "ctrl-source" );
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );

                if( (nCtrlFlags & 0x0080) && (nChildType != EXC_CTRL_UNKNOWN) )
                {
                    if( (nFlags & 0x00040000) && (nStorageId > 0) )
                        maCtrlStorages.push_back( XclDumpCtrlPortion( nStorageId, nChildType ) );
                    if( (nCtrlFlags & 0x0020) && (nSubStrmLen > 0) )
                        maCtrlPortions.push_back( XclDumpCtrlPortion( nSubStrmLen, nChildType ) );
                }
            }
            break;

            default:
                EXC_CTRLDUMP_REMAINING( nStartPos + nSize );
        }

        // font data
        if( bHasFontData )
            DumpControlContents( rInStrm, EXC_CTRL_FONTDATA );
    }
}

// ============================================================================
//
//  S T R E A M S
//
// ============================================================================

void Biff8RecDumper::DumpBinaryStream( SotStorageRef xStrg, const String& rStrmName, const String& rStrgPath )
{
    SotStorageStreamRef xInStrm = OpenStream( xStrg, rStrmName );
    if( !xInStrm || !pDumpStream ) return;

    XclDumpStreamHeader aStrmHeader( *xInStrm, *pDumpStream, rStrmName, rStrgPath );
    DumpBinary( *xInStrm );
    (*pDumpStream) << "\n";
}

void Biff8RecDumper::DumpTextStream( SotStorageRef xStrg, const String& rStrmName, const String& rStrgPath )
{
    SotStorageStreamRef xInStrm = OpenStream( xStrg, rStrmName );
    if( !xInStrm || !pDumpStream ) return;

    XclDumpStreamHeader aStrmHeader( *xInStrm, *pDumpStream, rStrmName, rStrgPath );
    while( xInStrm->Tell() < aStrmHeader.GetStreamLen() )
    {
        ByteString aLine;
        xInStrm->ReadLine( aLine );
        lclDumpString( *pDumpStream, aLine );
        (*pDumpStream) << "\n";
    }
    (*pDumpStream) << "\n";
}

void Biff8RecDumper::DumpRecordStream( SotStorageRef xStrg, const String& rStrmName, const String& rStrgPath )
{
    SotStorageStreamRef xInStrm = OpenStream( xStrg, rStrmName );
    if( !xInStrm || !pDumpStream ) return;

    XclDumpStreamHeader aStrmHeader( *xInStrm, *pDumpStream, rStrmName, rStrgPath );

    XclImpStream* pOldStream = pIn;
    pIn = new XclImpStream( *xInStrm, GetRoot() );
    XclImpStream& rIn = *pIn;
    if( pOldStream )
        rIn.CopyDecrypterFrom( *pOldStream );

    // -- dump from here --
    UINT16  nId;
    BOOL bLoop = TRUE;

    while( bLoop && rIn.StartNextRecord() )
    {
        nId = rIn.GetRecId();
        if( HasModeDump( nId ) )
            RecDump( TRUE );

        bLoop = (nId != 0x000A);
    }

    delete pIn;
    pIn = pOldStream;
}

void Biff8RecDumper::DumpCtlsStream()
{
    SotStorageStreamRef xInStrm = OpenStream( EXC_STREAM_CTLS );
    if( !xInStrm || !pDumpStream ) return;

    SvStream& rInStrm = *xInStrm;
    SvStream& rOutStrm = *pDumpStream;
    XclDumpStreamHeader aStrmHeader( rInStrm, rOutStrm, EXC_STREAM_CTLS, EMPTY_STRING );

    for( StrmPortionMap::const_iterator aIt = maCtlsPosMap.begin(), aEnd = maCtlsPosMap.end(); aIt != aEnd; ++aIt )
    {
        ULONG nCtrlPos = static_cast< ULONG >( aIt->first );
        ULONG nCtrlEnd = nCtrlPos + static_cast< ULONG >( aIt->second );
        if( nCtrlEnd <= aStrmHeader.GetStreamLen() )
        {
            // stream position
            ULONG nStartPos = nCtrlPos;     // for stream alignment macro
            rInStrm.Seek( nStartPos );

            ByteString t( "\npos=" );   __AddHex( t, aIt->first );
            t.Append( "  len=" );       __AddHex( t, aIt->second );
            rOutStrm << t.GetBuffer() << "  ";

            // control type
            sal_uInt16 nCtrlType = lclDumpControlGuid( rInStrm, rOutStrm );
            rOutStrm << "\n";

            // control contents
            if( nCtrlType != EXC_CTRL_UNKNOWN )
                DumpControlContents( rInStrm, nCtrlType );

            // remaining unknown data
            EXC_CTRLDUMP_REMAINING( nCtrlEnd );
            rOutStrm << "\n";
        }
    }
}

void Biff8RecDumper::DumpControlFrameStream( SotStorageRef xInStrg, sal_uInt16 nCtrlType, const String& rStrgPath )
{
    static const String saStrmName( sal_Unicode( 'f' ) );

    SotStorageStreamRef xInStrm = OpenStream( xInStrg, saStrmName );
    if( !xInStrm || !pDumpStream ) return;

    SvStream& rInStrm = *xInStrm;
    SvStream& rOutStrm = *pDumpStream;
    XclDumpStreamHeader aStrmHeader( rInStrm, rOutStrm, saStrmName, rStrgPath );

    if( aStrmHeader.GetStreamLen() > 0 )
    {
        ByteString t;               // "t" needed for macros
        sal_uInt32 nFlags = 0;      // "nFlags" needed for macros

        rOutStrm << "header-record\n";
        DumpControlContents( rInStrm, nCtrlType );
        rOutStrm << "\n";

        if( nCtrlType == EXC_CTRL_USERFORM )
        {
            rOutStrm << "add-records\n";
            sal_uInt16 nAddCount;
            EXC_CTRLDUMP_PLAIN_DECVAR( nAddCount, "count" );
            EXC_CTRLDUMP_PRINT();
            rOutStrm << "\n";

            for( sal_uInt16 nAdd = 0; (nAdd < nAddCount) && (rInStrm.Tell() < aStrmHeader.GetStreamLen()); ++nAdd )
            {
                DumpControlContents( rInStrm, EXC_CTRL_ADDDATA );
                rOutStrm << "\n";
            }
        }

        rOutStrm << "children-records\n";
        sal_uInt32 nRecCount, nTotalSize;
        EXC_CTRLDUMP_PLAIN_DECVAR( nRecCount, "count" );
        EXC_CTRLDUMP_PLAIN_HEXVAR( nTotalSize, "total-size" );
        if( nTotalSize > 0 )
        {
            EXC_CTRLDUMP_PLAIN_HEX4( "header-unknown" );
            rOutStrm << "\n";

            for( sal_uInt32 nRec = 0; (nRec < nRecCount) && (rInStrm.Tell() < aStrmHeader.GetStreamLen()); ++nRec )
            {
                DumpControlContents( rInStrm, EXC_CTRL_FRAMECHILD );
                rOutStrm << "\n";
            }
        }
        else
            rOutStrm << "\n";

        if( rInStrm.Tell() < aStrmHeader.GetStreamLen() )
        {
            rOutStrm << "remaining=\n";
            DumpBinary( rInStrm );
            rOutStrm << "\n";
        }
    }
}

void Biff8RecDumper::DumpControlObjectsStream( SotStorageRef xInStrg, const String& rStrgPath )
{
    static const String saStrmName( sal_Unicode( 'o' ) );

    SotStorageStreamRef xInStrm = OpenStream( xInStrg, saStrmName );
    if( !xInStrm || !pDumpStream ) return;

    SvStream& rInStrm = *xInStrm;
    SvStream& rOutStrm = *pDumpStream;
    XclDumpStreamHeader aStrmHeader( rInStrm, rOutStrm, saStrmName, rStrgPath );

    if( aStrmHeader.GetStreamLen() > 0 )
    {
        ULONG nStrmPos = 0;
        for( XclDumpCtrlPortionVec::const_iterator aIt = maCtrlPortions.begin(), aEnd = maCtrlPortions.end(); aIt != aEnd; ++aIt )
        {
            rInStrm.Seek( nStrmPos );
            DumpControlContents( rInStrm, aIt->second );
            rOutStrm << "\n";
            nStrmPos += aIt->first;
        }

        if( rInStrm.Tell() < aStrmHeader.GetStreamLen() )
        {
            rOutStrm << "remaining=\n";
            DumpBinary( rInStrm );
            rOutStrm << "\n";
        }
    }
}

// ============================================================================
//
//  S T O R A G E S
//
// ============================================================================

void Biff8RecDumper::DumpAnyStorage( SotStorageRef xParentStrg, const String& rStrgName, const String& rStrgPath )
{
    SotStorageRef xInStrg = OpenStorage( xParentStrg, rStrgName );
    if( !xInStrg || !pDumpStream ) return;

    XclDumpStorageHeader aStrgHeader( *xInStrg, *pDumpStream, rStrgPath );
}

void Biff8RecDumper::DumpUserFormStorage( SotStorageRef xParentStrg, const String& rStrgName, sal_uInt16 nCtrlType, const String& rStrgPath )
{
    SotStorageRef xInStrg = OpenStorage( xParentStrg, rStrgName );
    if( !xInStrg || !pDumpStream ) return;

    XclDumpStorageHeader aStrgHeader( *xInStrg, *pDumpStream, rStrgPath );

    // streams
    maCtrlStorages.clear();
    maCtrlPortions.clear();
    DumpControlFrameStream( xInStrg, nCtrlType, rStrgPath );
    DumpControlObjectsStream( xInStrg, rStrgPath );
    DumpTextStream( xInStrg, CREATE_STRING( "\003VBFrame" ), rStrgPath );

    // frame substorages
    XclDumpCtrlPortionVec aCtrlStorages( maCtrlStorages );  // make local copy, maCtrlStorages is reused in loop
    for( XclDumpCtrlPortionVec::const_iterator aIt = aCtrlStorages.begin(), aEnd = aCtrlStorages.end(); aIt != aEnd; ++aIt )
    {
        sal_uInt32 nStorageId = aIt->first;
        String aSubName( sal_Unicode( 'i' ) );
        if( nStorageId < 10 )
            aSubName.Append( sal_Unicode( '0' ) );
        aSubName.Append( String::CreateFromInt32( static_cast< sal_Int32 >( nStorageId ) ) );

        String aPath( rStrgPath );
        aPath.Append( sal_Unicode( '/' ) ).Append( rStrgName );

        DumpUserFormStorage( xInStrg, aSubName, aIt->second, aPath );
    }
}

void Biff8RecDumper::DumpVbaProjectStorage()
{
    SotStorageRef xInStrg = OpenStorage( EXC_STORAGE_VBA_PROJECT );
    if( !xInStrg || !pDumpStream ) return;

    XclDumpStorageHeader aStrgHeader( *xInStrg, *pDumpStream, EMPTY_STRING );
    // PROJECT substream
    DumpTextStream( xInStrg, CREATE_STRING( "PROJECT" ), EXC_STORAGE_VBA_PROJECT );
    // VBA storage
    DumpAnyStorage( xInStrg, EXC_STORAGE_VBA, EXC_STORAGE_VBA_PROJECT );
    // user forms
    SvStorageInfoList aInfoList;
    xInStrg->FillInfoList( &aInfoList );
    for( ULONG nInfo = 0; nInfo < aInfoList.Count(); ++nInfo )
    {
        SvStorageInfo& rInfo = aInfoList.GetObject( nInfo );
        if( rInfo.IsStorage() && (rInfo.GetName() != EXC_STORAGE_VBA) )
            DumpUserFormStorage( xInStrg, rInfo.GetName(), EXC_CTRL_USERFORM, EXC_STORAGE_VBA_PROJECT );
    }
}

// ============================================================================
//
// ============================================================================

const sal_Char* Biff8RecDumper::GetBlanks( const UINT16 nNumOfBlanks )
{
    DBG_ASSERT( pBlankLine, "-Biff8RecDumper::GetBlanks(): nicht so schnell mein Freund!" );
    DBG_ASSERT( nNumOfBlanks <= nLenBlankLine,
        "+Biff8RecDumper::GetBlanks(): So viel kannst Du nicht von mir verlangen!" );

    return pBlankLine + ( ( nNumOfBlanks <= nLenBlankLine )? ( nLenBlankLine - nNumOfBlanks ) : 0 );
}


BOOL Biff8RecDumper::IsLineEnd( const sal_Char c, sal_Char& rNext, SvStream& rIn, INT32& rLeft )
{
    static const sal_Char   cNL = '\n';
    static const sal_Char   cRET = '\r';

    if( IsEndOfLine( c ) )
    {
        sal_Char    cDouble = ( c == cNL )? cRET : cNL;

        if( rNext == cDouble )
        {
            rIn >> rNext;
            rLeft--;
        }

        return TRUE;
    }
    else
        return FALSE;
}


_KEYWORD Biff8RecDumper::GetKeyType( const ByteString& r )
{
    ByteString      t( r );
    _KEYWORD    e;

    t.ToUpperAscii();

    if( t == "SKIPDUMP" )
        e = Skipdump;
    else if( t == "SKIPOFFSET" )
        e = SkipOffset;
    else if( t == "READCONTRECS" )
        e = ReadContRecs;
    else if( t == "NOWARNINGS" )
        e = NoWarnings;
    else if( t == "CONTLOAD" )
        e = Contload;
    else if( t == "BLANKLINE" )
        e = BlankLine;
    else if( t == "EXPORTBOOKSTREAM" )
        e = ExportBookStream;
    else if( t == "PARSEP" )
        e = Parsep;
    else if( t == "MAXBODYLINES" )
        e = Maxbodylines;
    else if( t == "INCLUDE" || t == "+" )
        e = Include;
    else if( t == "EXCLUDE" || t == "-" )
        e = Exclude;
    else if( t == "HEX" || t == "H" )
        e = Hex;
    else if( t == "BODY" || t == "B" )
        e = Body;
    else if( t == "NAMEONLY" || t =="N" )
        e = NameOnly;
    else if( t == "COMMENT" )
        e = Comment;
    else if( t == "OUTPUT" )
        e = Output;
    else if( t == "TITLE" )
        e = Title;
    else if( t == "CLEARFILE" )
        e = ClearFile;
    else
        e = KW_Unknown;

    return e;
}


void Biff8RecDumper::Init( void )
{
    bClearFile = FALSE;

    if( pDumpModes || ppRecNames )
        return;

    const sal_Char*     pDefName = "biffrecdumper.ini";
    const sal_Char*     pIniKey = "BIFFRECDUMPERINI";
    const sal_Char      pPathSep[] = { SAL_PATHDELIMITER, '\0' };
    ByteString          aIniName;

    SvFileStream*       pIn = NULL;

    // first try: search for biffrecdumper.ini in dir, specified  in %HOME%
    const sal_Char*     pHome = getenv( "HOME" );
    if( pHome )
    {
        aIniName = pHome;
        aIniName.EraseTrailingChars( *pPathSep );
        aIniName += pPathSep;
        aIniName += pDefName;

        pIn = CreateInStream( aIniName.GetBuffer() );
    }

    if( !pIn )
    {   // %HOME% not set or %HOME%\biffrecdumper.ini could not be opened
        const sal_Char* pIni = getenv( pIniKey );
        if( pIni )
        {
            pIn = CreateInStream( pIni );
            if( !pIn )
            {
                AddError( 0, "Could not open ini file", ByteString( pIni ) );
                bEndLoading = TRUE;     // zur Sicherheit....
            }
        }
    }

    if( pIn )
    {
        pIn->Seek( STREAM_SEEK_TO_END );

        const UINT32    nStreamLen = pIn->Tell();

        if( nStreamLen <= 1 )
        {
            pIn->Close();
            delete pIn;
            return;
        }

        pIn->Seek( STREAM_SEEK_TO_BEGIN );

        pDumpModes = new UINT16[ nRecCnt ];
        ppRecNames = new ByteString*[ nRecCnt ];

        memset( pDumpModes, 0x00, sizeof( UINT16 ) * nRecCnt );
        memset( ppRecNames, 0x00, sizeof( ByteString* )* nRecCnt );

        enum STATE  { PreLine, InCommand, PostCommand, InId, PostEqu,
                        InVal, InName, InInExClude, InBodyMode, PostSepIEC,
                        PostSepBM, InComment };

        STATE       eAct = PreLine;
        sal_Char    c;
        sal_Char    cNext;
        INT32       n = ( INT32 ) nStreamLen;
        ByteString  aCommand;
        ByteString  aVal;
        ByteString  aInExClude;
        ByteString  aHexBody;
        UINT32      nLine = 1;
        BOOL        bCommand;

        *pIn >> c >> cNext;

        while( n > 0 )
        {
            n--;

            switch( eAct )
            {
                case PreLine:
                    if( IsAlphaNum( c ) )
                    {
                        if( IsNum( c ) )
                        {
                            eAct = InId;
                            bCommand = FALSE;
                        }
                        else
                        {
                            eAct = InCommand;
                            bCommand = TRUE;
                        }
                        aCommand.Erase();
                        aCommand += c;
                    }
                    else if( c == cComm1 && cNext == cComm2 )
                        eAct = InComment;
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                        nLine++;
                    else if( !IsBlank( c ) )
                    {
                        AddError( nLine, "Unexpected command or id" );
                        n = 0;
                    }
                    break;
                case InCommand:
                    if( c == '=' )
                    {
                        eAct = PostEqu;
                        aVal.Erase();
                    }
                    else if( IsAlpha( c ) )
                        aCommand += c;
                    else if( IsBlank( c ) )
                        eAct = PostCommand;
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecCommand( nLine, aCommand ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                            n = 0;
                    }
                    else if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecCommand( nLine, aCommand ) )
                            eAct = InComment;
                        else
                        {
                            AddError( nLine, "Invalid command before comment", aCommand );
                            n = 0;
                        }
                    }
                    else
                    {
                        aCommand += '>';
                        aCommand += c;
                        aCommand += '<';
                        AddError( nLine, "Invalid character in command or id", aCommand );
                        n = 0;
                    }
                    break;
                case PostCommand:
                    if( c == '=' )
                    {
                        eAct = PostEqu;
                        aVal.Erase();
                    }
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecCommand( nLine, aCommand ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                            n = 0;
                    }
                    else if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecCommand( nLine, aCommand ) )
                            eAct = InComment;
                        else
                        {
                            AddError( nLine, "Invalid command before comment", aCommand );
                            n = 0;
                        }
                    }
                    else if( !IsBlank( c ) )
                    {
                        AddError( nLine, "Expecting blank or \'=\' following the command", aCommand );
                        n = 0;
                    }
                    break;
                case InId:
                    if( c == '=' )
                    {
                        eAct = PostEqu;
                        aVal.Erase();
                    }
                    else if( IsAlphaNum( c ) )
                        aCommand += c;
                    else if( IsBlank( c ) )
                        eAct = PostCommand;
                    else if( ( c == cComm1 && cNext == cComm2 ) || IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        AddError( nLine, "Unexpected end of record id", aCommand );
                        eAct = InComment;
                    }
                    else
                    {
                        AddError( nLine, "Unexpected sal_Char in record id", aCommand );
                        n = 0;
                    }
                    break;
                case PostEqu:
                    if( bCommand )
                    {   // Command
                        if( c == cComm1 && cNext == cComm2 )
                        {
                            if( ExecCommand( nLine, aCommand ) )
                                eAct = InComment;
                            else
                            {
                                AddError( nLine, "Unknown command", aCommand );
                                n = 0;
                            }
                        }
                        else if( IsLineEnd( c, cNext, *pIn, n ) )
                        {
                            if( ExecCommand( nLine, aCommand ) )
                            {
                                eAct = PreLine;
                                nLine++;
                            }
                            else
                            {
                                AddError( nLine, "Unknown command", aCommand );
                                n = 0;
                            }
                        }
                        else if( !bCommand && c == cParSep )
                            eAct = PostSepIEC;
                        else if( !IsBlank( c ) )
                        {
                            aVal.Erase();
                            aVal += c;
                            eAct = InVal;
                        }
                    }
                    else
                    {   // Set
                        if( ( c == cComm1 && cNext == cComm2 ) || IsLineEnd( c, cNext, *pIn, n ) )
                        {
                            AddError( nLine, "No mode set for record", aCommand );
                            n = 0;
                        }
                        else if( !bCommand && c == cParSep )
                            eAct = PostSepIEC;
                        else
                        {
                            aVal.Erase();
                            aVal += c;
                            eAct = InName;
                            aInExClude.Erase();
                            aHexBody.Erase();
                        }
                    }
                    break;
                case InVal:
                    if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecCommand( nLine, aCommand, &aVal ) )
                            eAct = InComment;
                        else
                        {
                            AddError( nLine, "Unknown command", aCommand );
                            n = 0;
                        }
                    }
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecCommand( nLine, aCommand, &aVal ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                        {
                            AddError( nLine, "Unknown command", aCommand );
                            n = 0;
                        }
                    }
                    else
                        aVal += c;
                    break;
                case InName:
                    if( c == cParSep )
                        eAct = PostSepIEC;
                    else if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, NULL, NULL ) )
                            eAct = InComment;
                        else
                            n = 0;
                    }
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, NULL, NULL ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                            n = 0;
                    }
                    else
                        aVal += c;
                    break;
                case InInExClude:
                    if( c == cParSep )
                        eAct = PostSepBM;
                    else if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, &aInExClude, NULL ) )
                            eAct = InComment;
                        else
                            n = 0;
                    }
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, &aInExClude, NULL ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                            n = 0;
                    }
                    else
                        aInExClude += c;
                    break;
                case InBodyMode:
                    if( c == cParSep )
                    {
                        AddError( nLine, "Only 3 parameter possible" );
                        n = 0;
                    }
                    else if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, &aInExClude, &aHexBody ) )
                            eAct = InComment;
                        else
                            n = 0;
                    }
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, &aInExClude, &aHexBody ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                            n = 0;
                    }
                    else
                        aInExClude += c;
                    break;
                case PostSepIEC:
                    if( c == cParSep )
                        eAct = PostSepBM;
                    else if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, NULL, NULL ) )
                            eAct = InComment;
                        else
                            n = 0;
                    }
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, NULL, NULL ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                            n = 0;
                    }
                    else if( !IsBlank( c ) )
                    {
                        aInExClude += c;
                        eAct = InInExClude;
                    }
                    break;
                case PostSepBM:
                    if( c == cParSep )
                        eAct = PostSepBM;
                    else if( c == cComm1 && cNext == cComm2 )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, &aInExClude, NULL ) )
                            eAct = InComment;
                        else
                            n = 0;
                    }
                    else if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        if( ExecSetVal( nLine, aCommand, &aVal, &aInExClude, NULL ) )
                        {
                            eAct = PreLine;
                            nLine++;
                        }
                        else
                            n = 0;
                    }
                    else if( !IsBlank( c ) )
                    {
                        aHexBody += c;
                        eAct = InBodyMode;
                    }
                    break;
                case InComment:
                    if( IsLineEnd( c, cNext, *pIn, n ) )
                    {
                        eAct = PreLine;
                        nLine++;
                    }
                    break;
                default:
                    n = 0;
            }

            c = cNext;
            *pIn >> cNext;
        }

        pIn->Close();
        delete pIn;
    }
}


BOOL Biff8RecDumper::ExecCommand( const UINT32 nL, const ByteString& r, const ByteString* pVal )
{
    _KEYWORD                        e = GetKeyType( r );
    BOOL                            b = TRUE;
    const UINT32                    nValLen = ( pVal )? pVal->Len() : 0;
    BOOL                            bSet;
    UINT16                          nMode;

    switch( e )
    {
        case Skipdump:          bSkip = TRUE;               break;
        case SkipOffset:        bSkipOffset = TRUE;         break;
        case ReadContRecs:      bReadContRecs = TRUE;       break;
        case NoWarnings:        bWarnings = FALSE;          break;
        case Contload:          bEndLoading = TRUE;         break;
        case BlankLine:         bBlankLine = TRUE;          break;
        case ExportBookStream:  bExportBookStream = TRUE;   break;
        case Parsep:
            if( nValLen == 0 )
            {
                AddError( nL, "No separator found" );
                b = FALSE;
            }
            else if( nValLen == 1 )
                cParSep = *( pVal->GetBuffer() );
            else
            {
                AddError( nL, "More than 1 sal_Char is not valid for separator", *pVal );
                b = FALSE;
            }
            break;
        case Maxbodylines:
            if( nValLen )
            {
                UINT32              n = GetVal( *pVal );

                if( n == 0xFFFFFFFF )
                {
                    AddError( nL, "Syntax error in value of body lines" );
                    b = FALSE;
                }
                else
                    nMaxBodyLines = n;
            }
            else
            {
                AddError( nL, "No value specified for number of body lines" );
                b = FALSE;
            }
            break;
        case Include:
            bSet = FALSE;
            nMode = MODE_SKIP;
            goto _common;
        case Exclude:
            bSet = TRUE;
            nMode = MODE_SKIP;
            goto _common;
        case Hex:
            bSet = TRUE;
            nMode = MODE_HEX;
            goto _common;
        case Body:
            bSet = FALSE;
            nMode = MODE_HEX;
            goto _common;
        case NameOnly:
            bSet = TRUE;
            nMode = MODE_NAMEONLY;
            goto _common;
_common:
            if( pVal )
            {
                IdRangeList         aRL;
                if( FillIdRangeList( nL, aRL, *pVal ) )
                {
                    const IdRange*  p = aRL.First();
                    if( bSet )
                    {
                        while( p )
                        { SetFlag( p->nFirst, p->nLast, nMode );    p = aRL.Next(); }
                    }
                    else
                    {
                        while( p )
                        { ClrFlag( p->nFirst, p->nLast, nMode );    p = aRL.Next(); }
                    }
                }
                else
                    b = FALSE;
            }
            break;
        case Comment:
            if( pVal && pVal->Len() >= 2 )
            {
                cComm1 = pVal->GetChar( 0 );
                cComm2 = pVal->GetChar( 1 );
            }
            break;
        case Output:
            CopyStrpOnStrp( pOutName, pVal );
            if( pOutName )
            {
                if( *pOutName == "*" )
                {
                    ByteString          aDefault( "DefaultDumpName.txt" );
                    SfxObjectShell*     pShell = GetDocShell();
                    if( pShell )
                    {
                        SfxMedium*      pMedium = pShell->GetMedium();
                        if( pMedium )
                        {
                            const ByteString    aTextExtension( ".txt" );
                            aDefault = GETSTR( pMedium->GetPhysicalName() );
                            xub_StrLen      nStrLen = aDefault.Len();
                            if( nStrLen >= 4 )
                            {
                                ByteString  aTmp( aDefault );
                                aTmp.ToLowerAscii();
                                if( aTmp.Copy( nStrLen -4 ) == ".xls" )
                                    aDefault.Replace( nStrLen - 4, aTextExtension.Len(), aTextExtension );
                                else
                                    aDefault += aTextExtension;
                            }
                            else
                                aDefault += aTextExtension;
                        }
                    }

                    *pOutName = aDefault;
                }
                CreateOutStream( nL );
            }
            else if( pDumpStream )
            {
                pDumpStream->Close();
                DELANDNULL( pDumpStream );
            }
            break;
        case Title:
            CopyStrpOnStrp( pTitle, pVal );
            break;
        case ClearFile:
            bClearFile = TRUE;
            break;
        default:
            AddError( nL, "Unknown command", r );
            b = FALSE;
    }

    return b;
}


BOOL Biff8RecDumper::ExecSetVal( const UINT32 nL, const ByteString& rId, const ByteString* pName,
                                const ByteString* pIEC, const ByteString* pHB )
{
    UINT32  _nId = GetVal( rId );

    if( _nId == 0xFFFFFFFF )
    {
        AddError( nL, "Syntax error in record id", rId );
        return FALSE;
    }
    else if( _nId >= ( UINT32 ) nRecCnt )
    {
        AddError( nL, "Record id to high", rId );
        return FALSE;
    }

    UINT16  nId = ( UINT16 ) _nId;

    if( pName && pName->Len() )
        SetName( nId, *pName );

    _KEYWORD        e;
    UINT16          n = GetMode( nId );

    if( pIEC && pIEC->Len() )
    {
        e = GetKeyType( *pIEC );

        if( e == Include )
            n &= ~MODE_SKIP;
        else if( e == Exclude )
            n |= MODE_SKIP;
        else
        {
            AddError( nL, "Invalid key for in-/exclude", *pIEC );
            return FALSE;
        }
    }

    if( pHB && pHB->Len() )
    {
        e = GetKeyType( *pHB );

        if( e == NameOnly )
            n |= MODE_NAMEONLY;
        else
        {
            n &= ~MODE_NAMEONLY;
            if( e == Hex )
                n |= MODE_HEX;
            else if( e == Body )
                n &= ~MODE_HEX;
            else
            {
                AddError( nL, "Invalid key for hex/body/name only", *pIEC );
                return FALSE;
            }
        }
    }

    SetMode( nId, n );
    return TRUE;
}


void Biff8RecDumper::SetFlag( const UINT16 nF, const UINT16 nL, const UINT16 nFl )
{
    UINT16  n       = ( nF < nL )? nF : nL;
    UINT16  nLast   = ( nF < nL )? nL : nF;

    if( nLast >= nRecCnt )
        nLast = nRecCnt - 1;

    while( n <= nLast )
    {
        pDumpModes[ n ] |= nFl;
        n++;
    }
}


void Biff8RecDumper::ClrFlag( const UINT16 nF, const UINT16 nL, const UINT16 nFlags )
{
    UINT16  n       = ( nF < nL )? nF : nL;
    UINT16  nLast   = ( nF < nL )? nL : nF;
    UINT16  nFl     = ~nFlags;

    if( nLast >= nRecCnt )
        nLast = nRecCnt - 1;

    while( n <= nLast )
    {
        pDumpModes[ n ] &= nFl;
        n++;
    }
}


void Biff8RecDumper::SetName( const UINT16 n, ByteString* p )
{
    if( n < nRecCnt )
        CopyStrpOnStrp( ppRecNames[ n ], p );
}


UINT32 Biff8RecDumper::GetVal( const ByteString& r )
{
    const sal_Char* p = r.GetBuffer();
    sal_Char        c = *p;
    UINT32          n;
    const UINT32    nLimitDec = 0xFFFFFFFF / 10;
    const UINT32    nLimitHex = 0xFFFFFFFF / 16;
    BOOL            bError = FALSE;

    enum STATE  { First, Dec, MaybeHex, Hex };

    STATE           e = First;

    while( c )
    {
        switch( e )
        {
            case First:
                if( IsNum( c ) )
                {
                    if( c == '0' )
                        e = MaybeHex;
                    else
                    {
                        e = Dec;
                        n = GetVal( c );
                    }
                }
                else
                    bError = TRUE;
                break;
            case Dec:
                if( n < nLimitDec && IsNum( c ) )
                {
                    n *= 10;
                    n += GetVal( c );
                }
                else
                    bError = TRUE;
                break;
            case MaybeHex:
                if( c == 'x' || c == 'X' )
                {
                    e = Hex;
                    n = 0;
                }
                else if( IsNum( c ) )
                {
                    n = GetVal( c );
                    e = Dec;
                }
                else
                    bError = TRUE;
                break;
            case Hex:
                if( n < nLimitHex && IsHex( c ) )
                {
                    n *= 16;
                    n += GetVal( c );
                }
                else
                    bError = TRUE;
                break;
        }

        if( bError )
            c = 0x00;
        else
        {
            p++;
            c = *p;
        }
    }

    if( bError )
        return 0xFFFFFFFF;
    else
        return n;
}


BOOL Biff8RecDumper::FillIdRangeList( const UINT32 nL, IdRangeList& rRL, const ByteString& rV )
{
    rRL.Clear();

    const sal_Char*     pSyntErr = "Syntax error in number (list)";

#define SYNTERR(ch) AddError( ((UINT32)(b=FALSE))+nL, pSyntErr, ch? ((((t+=">")+=ByteString::CreateFromInt32(ch))+="<")) : t )
#define NEWVAL(_n)  _n=(n>=(UINT32)nRecCnt)?nRecCnt-1:(UINT16)n
#define ORDERN()    {if(n1>n2){UINT16 nT=n1;n1=n2;n2=nT;}}

    const sal_Char*     p = rV.GetBuffer();
    sal_Char            c = *p;
    const sal_Char      cSep = cParSep;
    const sal_Char      cCont = '.';
    const sal_Char      cAll = '*';
    ByteString          t;
    UINT16              n1, n2;
    UINT32              n;
    BOOL                b = TRUE;
    BOOL                bExp2 = FALSE;

    enum STATE  { PreFirst, InFirst, PostFirst, InCont, PreLast, InLast, PostLast };
    STATE           e = PreFirst;

    while( c )
    {
        switch( e )
        {
            case PreFirst:
                if( IsAlphaNum( c ) )
                {
                    t += c;
                    e = InFirst;
                }
                else if( c == cAll )
                {
                    rRL.Append( 0, nRecCnt - 1 );
                    e = PostLast;
                }
                else if( !IsBlank( c ) )
                    SYNTERR( c );
                break;
            case InFirst:
                if( c == cSep || c == cCont || IsBlank( c ) )
                {
                    n = GetVal( t );
                    if( n == 0xFFFFFFFF )
                        SYNTERR( 0 );
                    else
                        NEWVAL( n1 );

                    t.Erase();

                    if( c == cSep )
                    {
                        rRL.Append( n1, n1 );
                        e = PreFirst;
                    }
                    else if( c == cCont )
                    {
                        bExp2 = TRUE;
                        e = InCont;
                    }
                    else
                        e = PostFirst;
                }
                else if( IsAlphaNum( c ) )
                    t += c;
                else
                    SYNTERR( c );
                break;
            case PostFirst:
                if( c == cCont )
                {
                    e = InCont;
                    bExp2 = TRUE;
                }
                else if( c == cSep )
                {
                    n = GetVal( t );
                    if( n == 0xFFFFFFFF )
                        SYNTERR( 0 );
                    else
                        NEWVAL( n1 );

                    e = PreFirst;
                }
                else if( !IsBlank( c ) )
                    SYNTERR( c );
                break;
            case InCont:
                if( IsAlphaNum( c ) )
                {
                    t += c;
                    e = InLast;
                }
                else if( IsBlank( c ) )
                    e = PreLast;
                else if( c == cSep || c == cAll )
                {
                    rRL.Append( n1, nRecCnt - 1 );
                    bExp2 = FALSE;
                    e = PreFirst;
                }
                else if( c != cCont )
                    SYNTERR( c );
                break;
            case PreLast:
                if( IsAlphaNum( c ) )
                {
                    t += c;
                    e = InLast;
                }
                else if( !IsBlank( c ) )
                    SYNTERR( c );
                break;
                break;
            case InLast:
                if( c == cSep || IsBlank( c ) )
                {
                    n = GetVal( t );
                    if( n == 0xFFFFFFFF )
                        SYNTERR( 0 );
                    else
                        NEWVAL( n2 );

                    t.Erase();
                    ORDERN();
                    rRL.Append( n1, n2 );
                    bExp2 = FALSE;
                    e = ( c == cSep )? PreFirst : PostLast;
                }
                else if( IsAlphaNum( c ) )
                    t += c;
                else
                    SYNTERR( c );
                break;
            case PostLast:
                if( c == cSep )
                {
                    e = PreFirst;
                    bExp2 = FALSE;
                }
                else if( !IsBlank( c ) )
                    SYNTERR( c );
                break;
        }

        if( b )
        {
            p++;
            c = *p;
        }
        else
            c = 0x00;
    }

    if( bExp2 )
    {
        if( t.Len() )
        {
            n = GetVal( t );
            if( n == 0xFFFFFFFF )
                SYNTERR( 0 );
            else
                NEWVAL( n2 );
            ORDERN();
        }
        else
            n2 = nRecCnt - 1;

        if( b )
            rRL.Append( n1, n2 );
    }

#undef  SYNTERR
#undef  NEWVAL
#undef  ORDERN

    return b;
}


BOOL Biff8RecDumper::CreateOutStream( const UINT32 n )
{
    const BOOL bWithErr = ( n != 0xFFFFFFFF );

    if( pDumpStream )
    {
        pDumpStream->Close();
        DELANDNULL( pDumpStream );
    }

    if( !bSkip && pOutName )
    {
        pOutName->EraseLeadingChars( ' ' );
        pOutName->EraseTrailingChars( ' ' );
        pOutName->EraseLeadingChars( '\t' );
        pOutName->EraseTrailingChars( '\t' );

        pDumpStream = new SvFileStream( String::CreateFromAscii( pOutName->GetBuffer() ), STREAM_WRITE|STREAM_SHARE_DENYWRITE|(bClearFile?STREAM_TRUNC:0) );

        if( pDumpStream->IsOpen() )
        {
            pDumpStream->Seek( bClearFile? STREAM_SEEK_TO_BEGIN : STREAM_SEEK_TO_END );
            return TRUE;
        }
        else
        {
            DELANDNULL( pDumpStream );
        }
    }

    return FALSE;
}


SvFileStream* Biff8RecDumper::CreateInStream( const sal_Char* pN )
{
    SvFileStream*   p = new SvFileStream( String::CreateFromAscii( pN ), STREAM_READ|STREAM_SHARE_DENYWRITE );

    if( p->IsOpen() )
        return p;
    else
    {
        delete p;
        return NULL;
    }
}


SvFileStream* Biff8RecDumper::CreateInStream( const sal_Char* pP, const sal_Char* pN )
{
    ByteString      t( pP );
    const sal_Char  c = t.GetChar( t.Len() - 1 );

    if( c != '\\' )
        t += '\\';
    else if( c != '/' )
        t += '/';

    t += pN;

    return CreateInStream( t.GetBuffer() );
}


void Biff8RecDumper::AddError( const UINT32 n, const ByteString& rT, const ByteString& rH )
{
    DUMP_ERR*   p = new DUMP_ERR( n, rT, rH );

    if( pFirst )
        pLast->pNext = p;
    else
        pFirst = p;

    pLast = p;
}


Biff8RecDumper::Biff8RecDumper( const XclImpRoot& rRoot, BOOL _bBIFF8 ) :
    XclImpRoot( rRoot ),
    bBIFF8( _bBIFF8 ),
    bEncrypted( false ),
    mnEscherPos( 0 )
{
    nXFCount = 0;
    nFontIndex = 0;
    nInstances++;
    mnSubstream = EXC_BOF_UNKNOWN;

    if( !pCharType )
    {
        pCharType = new UINT8[ 256 ];
        memset( pCharType, 0x00, 256 );

        pCharVal = new UINT8[ 256 ];
        memset( pCharVal, 0x00, 256 );

        sal_Char    c;
        UINT8       n;

#define __TYPE(n)   pCharType[(UINT8)n]
#define __VAL(n)    pCharVal[(UINT8)n]

        __TYPE( '\n' ) = CT_EOL;
        __TYPE( '\r' ) = CT_EOL;

        __TYPE( ' ' ) = CT_BLANK;
        __TYPE( '\t' ) = CT_BLANK;

        for( c = '0', n = 0 ; c <= '9' ; c++, n++ )
        {
            __TYPE( c ) = CT_NUM|CT_HEX|CT_ALPHANUM;
            __VAL( c ) = n;
        }
        for( c = 'a', n = 10 ; c <= 'f' ; c++, n++ )
        {
            __TYPE( c ) = CT_HEX|CT_ALPHA|CT_ALPHANUM|CT_LOWERALPHA;
            __VAL( c ) = n;
        }
        for( c = 'g' ; c <= 'z' ; c++ )
            __TYPE( c ) = CT_ALPHA|CT_ALPHANUM|CT_LOWERALPHA;
        for( c = 'A', n = 10 ; c <= 'F' ; c++, n++ )
        {
            __TYPE( c ) = CT_HEX|CT_ALPHA|CT_ALPHANUM|CT_UPPERALPHA;
            __VAL( c ) = n;
        }
        for( c = 'G' ; c <= 'Z' ; c++ )
            __TYPE( c ) = CT_ALPHA|CT_ALPHANUM|CT_UPPERALPHA;

#undef __TYPE
#undef __VAL
    }

    pDumpStream = NULL;

    if( !pBlankLine )
    {
        pBlankLine = new sal_Char[ nLenBlankLine + 1 ];
        memset( pBlankLine, ' ', nLenBlankLine );
        pBlankLine[ nLenBlankLine ] = 0x00;
    }

    pTitle = NULL;
    pOutName = NULL;
    pLevelPre = pLevelPreStringNT;

    nMaxBodyLines = 1024;
    bEndLoading = bSkip = bSkipOffset = bReadContRecs = bBlankLine = bExportBookStream = FALSE;
    bWarnings = TRUE;

    pDumpModes = NULL;
    ppRecNames = NULL;

    pFirst = pLast = pAct = NULL;

    cParSep = ',';
    cComm1 = cComm2 = '/';

    nFieldCnt = nItemCnt = nTabIndexCnt = 0;

    Init();
}


Biff8RecDumper::~Biff8RecDumper()
{
    if( pDumpStream )
    {
        pDumpStream->Close();
        delete pDumpStream;
    }

    if( nInstances )
    {
        nInstances--;
        if( !nInstances )
        {
            delete[] pBlankLine;
            pBlankLine = NULL;

            delete[] pCharType;
            pCharType = NULL;

            delete[] pCharVal;
            pCharVal = NULL;
        }
    }

    if( pDumpModes )
        delete[] pDumpModes;

    if( ppRecNames )
    {
        ByteString**        pp = ppRecNames;
        UINT16          n = nRecCnt;
        while( n )
        {
            if( *pp )
                delete *pp;
            pp++;
            n--;
        }
        delete[] ppRecNames;
    }

    if( pTitle )
        delete pTitle;

    if( pOutName )
        delete pOutName;

    DUMP_ERR*           p = pFirst;
    DUMP_ERR*           pD;
    if( p )
    {
        pD = p;
        p = p->pNext;
        delete pD;
    }
}


BOOL Biff8RecDumper::Dump( XclImpStream& r )
{
    const DUMP_ERR*     pErr = FirstErr();

    if( pErr )
    {
        if( pDumpStream )
        {
            SvFileStream&   rOut = *pDumpStream;
            while( pErr )
            {
                rOut << "\nError";
                if( pErr->nLine )
                {
                    ByteString  t;
                    t += ByteString::CreateFromInt32( pErr->nLine );
                    rOut << " at line " << t.GetBuffer();
                }

                rOut << ": " << pErr->aText.GetBuffer();

                if( pErr->pHint )
                    rOut << " (" << pErr->pHint->GetBuffer() << ')';
                pErr = NextErr();
            }
            rOut << '\n';
        }
    }
    else if( pDumpStream && !bSkip )
    {
        SvStream& rOutStrm = *pDumpStream;

        if( bExportBookStream && pOutName )
        {
            ByteString aBookOutName( *pOutName, 0, pOutName->Len() - 4 );
            aBookOutName.Append( "_book.xls" );
            SvFileStream aBook( String::CreateFromAscii( aBookOutName.GetBuffer() ), STREAM_WRITE|STREAM_SHARE_DENYWRITE|STREAM_TRUNC );
            if( aBook.IsOpen() )
            {
                const sal_uInt32 nBufLen = 0xFFFF;
                sal_uInt8 pBuffer[ nBufLen ];
                r.StoreGlobalPosition();
                while( r.StartNextRecord() )
                {
                    r.ResetRecord( false );
                    sal_uInt16 nRecSize = (sal_uInt16) Min( r.GetRecSize(), nBufLen );
                    aBook << r.GetRecId() << nRecSize;
                    r.Read( pBuffer, nRecSize );
                    aBook.Write( pBuffer, nRecSize );
                }
                r.SeekGlobalPosition();
            }
        }

        if( pTitle )
            rOutStrm << pTitle->GetBuffer();

        pIn = &r;
        r.StoreGlobalPosition();

        ::std::auto_ptr< XclDumpStorageHeader > xStrgHerader;
        SotStorageRef xRootStrg = GetRootStorage();
        if( xRootStrg.Is() )
            xStrgHerader.reset( new XclDumpStorageHeader( *xRootStrg, rOutStrm, EMPTY_STRING ) );

        ::std::auto_ptr< ScfProgressBar > xProgress( new ScfProgressBar(
            GetDocShell(), String( RTL_CONSTASCII_USTRINGPARAM( "Dumper" ) ) ) );
        sal_Int32 nStreamSeg = xProgress->AddSegment( r.GetSvStreamSize() );
        xProgress->ActivateSegment( nStreamSeg );

        while( r.StartNextRecord() )
        {
            xProgress->ProgressAbs( r.GetSvStreamPos() );
            if( HasModeDump( r.GetRecId() ) )
                RecDump();
        }

        rOutStrm << "\n\n";

        pIn = NULL;
        xProgress.reset();

        r.SeekGlobalPosition();

        // dump substreams
        if( xRootStrg.Is() )
        {
            pIn = NULL;
            bool bOldEncr = bEncrypted;
            bEncrypted = false;
            DumpRecordStream( xRootStrg, EXC_STREAM_USERNAMES, EMPTY_STRING );

            pIn = &r;
            bEncrypted = bOldEncr;
            DumpRecordStream( xRootStrg, EXC_STREAM_REVLOG, EMPTY_STRING );

            pIn = NULL;

            DumpCtlsStream();
            DumpVbaProjectStorage();
        }
    }

    return !bEndLoading;
}

#undef Read1
#undef Read2
#undef Read4
#undef Read8

#endif

