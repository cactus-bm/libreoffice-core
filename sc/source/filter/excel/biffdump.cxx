/*************************************************************************
 *
 *  $RCSfile: biffdump.cxx,v $
 *
 *  $Revision: 1.37 $
 *
 *  last change: $Author: dr $ $Date: 2002-04-10 14:57:56 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _BIFFDUMP_HXX
#include "biffdump.hxx"
#endif

#ifdef DEBUGGING

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
#ifndef _SC_XCLTOOLS_HXX
#include "XclTools.hxx"
#endif
#ifndef _FLTTOOLS_HXX
#include "flttools.hxx"
#endif
#ifndef _FLTPRGRS_HXX
#include "fltprgrs.hxx"
#endif
#ifndef _IMP_OP_HXX
#include "imp_op.hxx"
#endif

#define GETSTR(s)       ByteString( s, RTL_TEXTENCODING_MS_1252 )

static ByteString       aDefaultName;

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

extern const sal_Char*  pUserNamesStreamName;
extern const sal_Char*  pRevLogStreamName;

Biff8RecDumper          __aDummyBiff8RecDumperInstance();


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
    ultoa( n, p, 10 );

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
    ltoa( n, p, 10 );

    r += p;
}


inline static void __AddDec( ByteString& r, INT16 n )
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
    sal_Char    p[ 256 ];
    sprintf( p, "%.15G", f );
    r += p;
}


static inline void __AddRK( ByteString& rString, sal_uInt32 nRKValue )
{
    __AddDouble( rString, XclTools::GetDoubleFromRK( nRKValue ) );
}


inline static void __Add16p16( ByteString& r, UINT32 n )
{
    __AddDouble( r, double(n) / 65536.0 );
}


static void __AddRef( ByteString& r, const UINT16 nC, const UINT16 nR )
{
    ScTripel    aRef( nC, nR, 0 );
    r += GETSTR( aRef.GetColRowString() );
}


static void __AddCellHead( ByteString& r, const UINT16 nC, const UINT16 nR, const UINT16 nXF )
{
    __AddRef( r, (UINT8) nC, nR );
    r += " (XF=";
    __AddDec( r, nXF );
    r += ')';
}



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
        case 0:     p = "date";         break;
        case 1:     p = "numeric";      break;
        case 2:     p = "sequence";     break;
        case 3:     p = "text";         break;
        default:    p = pU;
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
        // C-1, R-1 = one column left/row up
        // C+1, R+1 = one column right/row down
        // C,   R   = same column/row
        // C=B, R=2 = absolute column B/row 2
        t += "[C";
        if( bColRel )
        {
            if( nRelCol > 0 )
                t += '+';
            if( nRelCol )
                __AddDec( t, (INT16)nRelCol );
        }
        else
        {
            t += '=';
            t += GETSTR( ::ColToAlpha( nCol ) );
        }
        t += "|R";

        if( bRowRel )
        {
            if( nRelRow > 0 )
                t += "+";
            if( nRelRow )
                __AddDec( t, nRelRow );
        }
        else
        {
            t += '=';
            __AddDec( t, (UINT16)(nRow + 1) );
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


static BOOL AddUNICODEString( ByteString& rStr, XclImpStream& rStrm, const BOOL b16BitLen = TRUE, UINT16 nLen = 0 )
{
    BOOL bRet = TRUE;

    if( !nLen )
        nLen = b16BitLen ? rStrm.ReaduInt16() : rStrm.ReaduInt8();
    UINT8 nGrbit = rStrm.ReaduInt8();

    UINT32  nExtLen;
    UINT16  nCrun;
    BOOL    b16Bit, bFarEast, bRichString;
    ULONG   nSeek = rStrm.ReadUniStringExtHeader( b16Bit, bRichString, bFarEast, nCrun, nExtLen, nGrbit );

    rStr += "[l=";
    __AddDec( rStr, nLen );
    rStr += ", f=";
    __AddHex( rStr, nGrbit );
    rStr += " (";
    rStr += b16Bit ? "16-Bit" : "8-Bit";

    if( bRichString && bFarEast )
        rStr += ", Rich, FarEast";
    else if( bRichString && !bFarEast )
        rStr += ", Rich";
    else if ( !bRichString && bFarEast )
        rStr += ", FarEast";
    rStr += ")]: \'";

    ByteString aData( rStrm.ReadRawUniString( nLen, b16Bit ), RTL_TEXTENCODING_MS_1252 );

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
        rStr += " Bytes)";
    }
    if( nExtLen )
    {
        rStr += " + ";
        __AddDec( rStr, nExtLen );
        rStr += " Byte extended info";
    }

    if( nSeek > 2048 )
    {
        rStr += " (Gruetze!)";
        bRet = FALSE;
    }

    rStrm.SkipUniStringExtData( nSeek );

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
#define ADDCOLROW(c,r)          __AddRef( t, c, r )
#define PRINT()                 Print( t )
#define PreDump(LEN)            {rIn.PushPosition();ContDump(LEN);rIn.PopPosition();}
#define ADDCELLHEAD()           {UINT16 nR,nC,nX;rIn>>nR>>nC>>nX;__AddCellHead(t,nC,nR,nX);}
#define STARTFLAG()             {ADDTEXT( "flags (" ); __AddHex( t, __nFlags ); ADDTEXT( "):" );}
#define ADDFLAG(mask,text)      {if( __nFlags & mask ) t += text;}
#define ADDRESERVED(mask)       ADDFLAG(mask," !RESERVED!")


UINT16 Biff8RecDumper::DumpXF( XclImpStream& rIn, const sal_Char* pPre )
{
    ByteString      t;
    const sal_Char* p;
    UINT16          nW4, nW6, nW8, nW10, nW12, nW14, nW16, nW22;
    UINT32          nL18;

    rIn >> nW4 >> nW6 >> nW8 >> nW10 >> nW12 >> nW14 >> nW16 >> nL18 >> nW22;

    LINESTART();
    ADDTEXT( "--4- --6- --8- -10- -12- -14- -16- ---18--- -22-" );
    PRINT();
    LINESTART();
    __AddPureHex( t, nW4 );
    ADDTEXT( "-" );
    __AddPureHex( t, nW6 );
    ADDTEXT( "-" );
    __AddPureHex( t, nW8 );
    ADDTEXT( "-" );
    __AddPureHex( t, nW10 );
    ADDTEXT( "-" );
    __AddPureHex( t, nW12 );
    ADDTEXT( "-" );
    __AddPureHex( t, nW14 );
    ADDTEXT( "-" );
    __AddPureHex( t, nW16 );
    ADDTEXT( "-" );
    __AddPureHex( t, nL18 );
    ADDTEXT( "-" );
    __AddPureHex( t, nW22);
    PRINT();

    LINESTART();
    if( nW8 & 0x0004 )
        ADDTEXT( "Style XF" );
    else
        ADDTEXT( "Cell XF " );
    ADDTEXT( "                               ( #" );
    __AddDec( t, nXFCount );
    nXFCount++;
    ADDTEXT( " )" );
    PRINT();

    LINESTART();                // pXFD->SetFont( nW4 );
    ADDTEXT( "Font: " );
    __AddDec( t, nW4 );
    PRINT();

    LINESTART();                // pXFD->SetValueFormat( pValueFormBuffer->GetValueFormat( nW6 ) );
    ADDTEXT( "Format: " );
    __AddDec( t, nW6 );
    PRINT();

    LINESTART();
    ADDTEXT( "Flags: " );
    if( nW8 & 0x0001 )      // pXFD->SetLocked( ( BOOL ) ( nW8 & 0x0001 ) );
        ADDTEXT( " Locked" );
    if( nW8 & 0x0002 )      // pXFD->SetHidden( ( BOOL ) ( nW8 & 0x0002 ) );
        ADDTEXT( " Hidden" );
    if( nW10 & 0x0008 )     // pXFD->SetWrap( EWT_Wrap );
        ADDTEXT( " Wrap" );
    if( nW10 & 0x0080 )
        ADDTEXT( " JustLast" );
    if( nW12 & 0x0020 )     // pXFD->Merge();
        ADDTEXT( " Merge" );
    PRINT();


    LINESTART();                // pXFD->SetParent( nW8 >> 4 );
    ADDTEXT( "Parent: " );
    __AddDec( t, ( UINT32 ) ( nW8 >> 4 ) );
    PRINT();

    LINESTART();                // pXFD->SetAlign( ( ExcHorizAlign ) ( nW10 & 0x0007 ) );
    ADDTEXT( "Alignment: " );
    switch( nW10 & 0x0007 )
    {
        case 0:     p = "general";                  break;
        case 1:     p = "left";                     break;
        case 2:     p = "center";                   break;
        case 3:     p = "right";                    break;
        case 4:     p = "fill";                     break;
        case 5:     p = "justify";                  break;
        case 6:     p = "center across selection";  break;
        default:    p = "ERROR";
    }
    ADDTEXT( p );
    PRINT();

    LINESTART();                // pXFD->SetAlign( ( ExcVertAlign ) ( ( nW10 & 0x0070 ) >> 4 ) );
    ADDTEXT( "Vertical Alignment: " );
    switch( ( nW10 & 0x0070 ) >> 4 )
    {
        case 0:     p = "top";                      break;
        case 1:     p = "center";                   break;
        case 2:     p = "bottom";                   break;
        case 3:     p = "justify";                  break;
        default:    p = "ERROR";
    }
    ADDTEXT( p );
    PRINT();

    LINESTART();                // pXFD->SetTextOrient( ( UINT8 ) ( nW10 >> 8 ) );
    ADDTEXT( "Rotation: " );
    __AddDec( t, ( UINT8 ) ( nW10 >> 8 ) );
    PRINT();

    LINESTART();                // pXFD->SetIndent( nW12 & 0x000F );
    ADDTEXT( "Indent: " );
    __AddDec( t, ( UINT32 ) ( nW12 & 0x000F ) );
    PRINT();

    LINESTART();                // aBorder.nLeftLine = ( BYTE ) nW14 & 0x000F;
    ADDTEXT( "Left Line:   " );
    __AddDec( t, ( BYTE ) ( nW14 & 0x000F ) );
    ADDTEXT( ", c= " );
    __AddDec( t, ( UINT32 ) ( nW16 & 0x007F ) );            // aBorder.nLeftColor = nW16 & 0x007F;
    PRINT();

    nW14 >>= 4;
    LINESTART();                // aBorder.nRightLine = ( BYTE ) nW14 & 0x000F;
    ADDTEXT( "Right Line:  " );
    __AddDec( t, ( BYTE ) ( nW14 & 0x000F ) );
    ADDTEXT( ", c= " );
    __AddDec( t, ( UINT32 ) ( ( nW16 >> 7 ) & 0x007F ) );   // aBorder.nRightColor = ( nW16 >> 7 ) & 0x007F;
    PRINT();

    nW14 >>= 4;
    LINESTART();                // aBorder.nTopLine = ( BYTE ) nW14 & 0x000F;
    ADDTEXT( "Top Line:    " );
    __AddDec( t, ( BYTE ) ( nW14 & 0x000F ) );
    ADDTEXT( ", c= " );
    __AddDec( t, ( UINT16 ) ( nL18 & 0x007F ) );    // aBorder.nTopColor = ( UINT16 ) ( nL18 & 0x007F );
    PRINT();

    nW14 >>= 4;
    LINESTART();                // aBorder.nBottomLine = ( BYTE ) nW14 & 0x000F;
    ADDTEXT( "Bottom Line: " );
    __AddDec( t, ( BYTE ) ( nW14 & 0x000F ) );
    ADDTEXT( ", c= " );
    __AddDec( t, ( UINT16 ) ( ( nL18 >> 7 ) & 0x007F ) );   // aBorder.nBottomColor = ( UINT16 ) ( ( nL18 >> 7 ) & 0x007F );
    PRINT();

    LINESTART();
    ADDTEXT( "Fill Pattern: p=" );
    __AddDec( t, ( BYTE ) ( nL18 >> 26 ) ); // aFill.nPattern = ( BYTE ) ( nL18 >> 26 );
    ADDTEXT( "  fc=" );
    __AddDec( t, ( UINT32 ) ( nW22 & 0x007F ) );                // aFill.nForeColor = nW22 & 0x007F;
    ADDTEXT( "  bc=" );
    __AddDec( t, ( UINT32 ) ( ( nW22 & 0x3F80 ) >> 7 ) );       // aFill.nBackColor = ( nW22 & 0x3F80 ) >> 7;
    PRINT();

    return 20;  // Laenge des Recs
}

// 16 bit pseudo password
void Biff8RecDumper::DumpValidPassword( XclImpStream& rIn, const sal_Char* pPre )
{
    ByteString  t;
    UINT16 nPasswd;
    rIn >> nPasswd;
    LINESTART();
    __AddHex( t, nPasswd );
    if( nPasswd )
    {
        ByteString  sPasswd;
        ByteString  sDummy;
        UINT16  nLen = 9;
        UINT16  nDummy;
        UINT16  nNewChar;

        nPasswd ^= 0xCE4B;
        nDummy = nPasswd;
        ADDTEXT( "   without mask: " );
        __AddHex( t, nPasswd );
        sPasswd.Erase();
        while( !(nDummy & 0x8000) && nLen )
        {
            nLen--;
            nDummy <<= 1;
        }
        if( !nLen ) nLen = 2;
        if( (nLen ^ nPasswd) & 0x0001 ) nLen++;
        if( nLen == 9 )
        {
            nLen = 10;
            nPasswd ^= 0x8001;
        }
        nPasswd ^= nLen;
        if( nLen < 9 ) nPasswd <<= (8 - nLen);
        for( UINT16 iChar = nLen; iChar > 0; iChar-- )
        {
            switch( iChar )
            {
                case 10:
                    nNewChar = (nPasswd & 0xC000) | 0x0400;
                    nPasswd ^= nNewChar;
                    nNewChar >>= 2;
                    break;
                case 9:
                    nNewChar = 0x4200;
                    nPasswd ^= nNewChar;
                    nNewChar >>= 1;
                    break;
                case 1:
                    nNewChar = nPasswd & 0xFF00;
                    break;
                default:
                    nNewChar = (nPasswd & 0xE000) ^ 0x2000;
                    if( !nNewChar ) nNewChar = (nPasswd & 0xF000) ^ 0x1800;
                    if( nNewChar == 0x6000 ) nNewChar = 0x6100;
                    nPasswd ^= nNewChar;
                    nPasswd <<= 1;
                    break;
            }
            nNewChar >>= 8;
            nNewChar &= 0x00FF;
            sDummy = sPasswd;
            sPasswd = (sal_Char) nNewChar;
            sPasswd += sDummy;
        }
        ADDTEXT( "   valid password: \"" );
        t += sPasswd;
        ADDTEXT( "\"" );
    }
    PRINT();
}


void __AddGUID( ByteString& rStr, XclImpStream& rIn )
{
    UINT16 nIndex;
    for( nIndex = 0; nIndex < 4; nIndex++ )
        __AddPureHex( rStr, Read1( rIn ) );
    rStr += "-";
    for( nIndex = 0; nIndex < 2; nIndex++ )
        __AddPureHex( rStr, Read1( rIn ) );
    rStr += "-";
    for( nIndex = 0; nIndex < 2; nIndex++ )
        __AddPureHex( rStr, Read1( rIn ) );
    rStr += "-";
    for( nIndex = 0; nIndex < 4; nIndex++ )
        __AddPureHex( rStr, Read1( rIn ) );
    rStr += "-";
    for( nIndex = 0; nIndex < 4; nIndex++ )
        __AddPureHex( rStr, Read1( rIn ) );
}


void Biff8RecDumper::RecDump( BOOL bSubStream )
{
    const sal_Char*     p;
    BOOL                bDec = FALSE;
    ByteString          aTmp;
    UINT16              __nFlags;
    const UINT16        nR = pIn->GetRecNum();
    const ByteString*   pName = GetName( nR );

    // set CONTINUE handling mode
    switch( nR )
    {
        case 0x000A:        // EOF
        case 0x003C:        // CONT
        case 0x005D:        // OBJ
        case 0x00EC:        // MSODRAWING
        case 0x01B6:        // TXO
            pIn->InitializeRecord( FALSE );
        break;
        default:
            pIn->InitializeRecord( bReadContRecs );
    }
    const ULONG         nL = pIn->GetRecLen();

    switch( nR )
    {
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
            __AddHex( aT, UINT32(pIn->GetStreamPos() - 2 * sizeof(UINT16)) );
            aT += ':';
        }

        Print( aT );
    }

    if( HasModeNameOnly( nR ) )
        ;
    else if( HasModeHex( nR ) || !bBIFF8 )
        ContDump( nL );
    else if( nMaxBodyLines && nL )
    {
        XclImpStream& rIn = *pIn;

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
                ADDTEXT( "   (" );                    __AddRef( t, nC1, (UINT16)nR1 );
                ADDTEXT( ":" );                       __AddRef( t, nC2-1, (UINT16)nR2-1 );
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
                    ADDFLAG( 0x01, " fAlwaysCalc" );
                    ADDFLAG( 0x02, " fCalcOnLoad" );
                    ADDFLAG( 0x08, " fShrFmla" );
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
            }
            break;
            case 0x0013:        // PASSWORD
                DumpValidPassword( rIn, pPre );
                break;
            case 0x17:
            {
                UINT16  n;
                rIn >> n;
                ADDTEXT( "# of XTI: " );
                __AddDec( t, n );
                PRINT();
                UINT16  nSB, nF, nL;
                while( n )
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
            break;
            case 0x18:
            case 0x0218:        // NAME
            {
                UINT8   nNameText, n8;
                UINT16  nNameDef, n16;

                PreDump( nL );
                rIn >> __nFlags >> n8 >> nNameText >> nNameDef >> n16 >> n16 >> n8 >> n8 >> n8 >> n8;
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, " fHidden" );
                ADDFLAG( 0x0002, " fFunc" );
                ADDFLAG( 0x0004, " fVBProc" );
                ADDFLAG( 0x0008, " fProc" );
                ADDFLAG( 0x0010, " fCalcExp" );
                ADDFLAG( 0x0020, " fBuiltIn" );
                ADDFLAG( 0x1000, " fBig" );
                ADDRESERVED( 0xE000 );
                ADDTEXT( "   Fn grp index: " );
                __AddDec( t, (UINT16)((__nFlags & 0x0FC0) >> 6) );
                PRINT();

                ByteString  sTemp;
                LINESTART();
                ADDTEXT( "NAMETEXT[" );
                __AddDec( t, nNameText );
                ADDTEXT( "] = " );
                if( nNameText )
                    sTemp = GETSTR( rIn.ReadUniString( nNameText ) );
                else
                    rIn >> n8;
                if( (__nFlags & 0x0020) && (nNameText == 1) )
                {
                    UINT8 nAutoname = (UINT8)(sTemp.GetChar( 0 ));
                    ADDTEXT( "(" );
                    __AddHex( t, nAutoname );
                    ADDTEXT( ") " );
                    switch( nAutoname )
                    {
                        case 0x00:  ADDTEXT( "Consolidate_Area" );  break;
                        case 0x01:  ADDTEXT( "Auto_Open" );         break;
                        case 0x02:  ADDTEXT( "Auto_Close" );        break;
                        case 0x03:  ADDTEXT( "Extract" );           break;
                        case 0x04:  ADDTEXT( "Database" );          break;
                        case 0x05:  ADDTEXT( "Criteria" );          break;
                        case 0x06:  ADDTEXT( "Print_Area" );        break;
                        case 0x07:  ADDTEXT( "Print_Titles" );      break;
                        case 0x08:  ADDTEXT( "Recorder" );          break;
                        case 0x09:  ADDTEXT( "Data_Form" );         break;
                        case 0x0A:  ADDTEXT( "Auto_Activate" );     break;
                        case 0x0B:  ADDTEXT( "Auto_Deactivate" );   break;
                        case 0x0C:  ADDTEXT( "Sheet_Title" );       break;
                        case 0x0D:  ADDTEXT( "AutoFilter" );        break;
                        default:    ADDTEXT( pU );
                    }
                }
                else
                {
                    ADDTEXT( "\"" );
                    t += sTemp;
                    ADDTEXT( "\"" );
                }
                PRINT();
                LINESTART();
                t += "NAMEDEF[";
                __AddDec( t, nNameDef );
                t += "] (formula?)";
                PRINT();
                if ( nNameDef && rIn.GetRecLeft() > 0 )
                    FormulaDump( ((rIn.GetRecLeft() < nNameDef) ? UINT16(rIn.GetRecLeft()) : nNameDef), FT_RangeName );
            }
            break;
            case 0x001D:        // SELECTION - list of selections
            {
                ADDTEXT( "pane: " );                ADDDEC( 1 );
                ADDTEXT( "   active cell: " );
                UINT16 nR, nC;
                rIn >> nR >> nC;
                __AddRef( t, nC, nR );
                ADDTEXT( "   active index: " );     ADDDEC( 2 );
                ADDTEXT( "   ref count: " );
                UINT16 nCount;
                rIn >> nCount;
                __AddDec( t, nCount );
                PRINT();
                for( UINT16 nIndex = 0; nIndex < nCount; nIndex++ )
                {
                    LINESTART();
                    UINT16 nR1, nR2;
                    UINT8 nC1, nC2;
                    rIn >> nR1 >> nR2 >> nC1 >> nC2;
                    ADDTEXT( "ref#" );      __AddDec( t, nIndex, 3 );
                    ADDTEXT( ": " );        __AddRef( t, nC1, nR1 );
                    ADDTEXT( ":" );         __AddRef( t, nC2, nR2 );
                    PRINT();
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
                ADDFLAG( 0x0002, " fItalic" );
                ADDFLAG( 0x0008, " fStrikeout" );
                ADDFLAG( 0x0010, " fOutline" );
                ADDFLAG( 0x0020, " fShadow" );
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
                ADDTEXT( "   reserved: " );     ADDHEX( 1 );
                ADDTEXT( "   " );
                AddUNICODEString( t, rIn, FALSE );
                PRINT();
            }
            break;
            case 0x51:
            {
                UINT16  nR1, nR2;
                UINT8   nC1, nC2;
                rIn >> nR1 >> nR2 >> nC1 >> nC2;
                ADDCOLROW( nC1, nR1 );
                ADDTEXT( " : " );
                ADDCOLROW( nC2, nR2 );
                PRINT();
                LINESTART();
                ADDTEXT( "workbook: " );
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
                    ADDFLAG( 0x0001, " fReadOnlyRec" );
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
                ADDFLAG( 0x0001, " fHidden" );
                ADDTEXT( " outlnlev=" );
                __AddDec( t, (UINT16)((__nFlags & 0x0700) >> 8) );
                ADDFLAG( 0x1000, " fCollapsed" );
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
                __AddRK( t, Read4( rIn ) );
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
                    ADDFLAG( 0x0001, " fShowAutoBreaks" );
                    ADDFLAG( 0x0010, " fDialog" );
                    ADDFLAG( 0x0020, " fApplyStyles" );
                    ADDFLAG( 0x0040, " fRowSumsBelow" );
                    ADDFLAG( 0x0080, " fColSumsBelow" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & (0xCD00 | 0x320E) )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x0100, " fFitToPage" );
                    ADDFLAG( 0x0400, " fDispRowGuts" );
                    ADDFLAG( 0x0800, " fDispColGuts" );
                    ADDFLAG( 0x4000, " fAee" );
                    ADDFLAG( 0x8000, " fAfe" );
                    ADDRESERVED( 0x320E );
                    PRINT();
                }
                if( !__nFlags )
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
                ADDFLAG( 0x0003, " fJoin" );
                ADDFLAG( 0x0004, " fSimpleEq1" );
                ADDFLAG( 0x0008, " fSimpleEq2" );
                ADDFLAG( 0x0010, " fTop10" );
                ADDFLAG( 0x0020, " fTop" );
                ADDFLAG( 0x0040, " fPercent" );
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
                            __AddRK( sTemp[ nF ], Read4( rIn ) );
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
                    ADDFLAG( 0x0001, " fRowGrand" );
                    ADDFLAG( 0x0002, " fColGrand" );
                    ADDFLAG( 0x0008, " fAutoFormat" );
                    ADDFLAG( 0x0010, " fWidthHeightAuto" );
                    ADDFLAG( 0x0020, " fFontAuto" );
                    ADDFLAG( 0x0040, " fAlignAuto" );
                    ADDFLAG( 0x0080, " fBorderAuto" );
                    ADDFLAG( 0x0100, " fPatternAuto" );
                    ADDFLAG( 0x0200, " fNumberAuto" );
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
                    ADDFLAG( 0x0001, " row" );
                    ADDFLAG( 0x0002, " col" );
                    ADDFLAG( 0x0004, " page" );
                    ADDFLAG( 0x0008, " data" );
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
                    ADDFLAG( 0x0001, " Default" );
                    ADDFLAG( 0x0002, " Sum" );
                    ADDFLAG( 0x0004, " Counta" );
                    ADDFLAG( 0x0008, " Average" );
                    ADDFLAG( 0x0010, " Max" );
                    ADDFLAG( 0x0020, " Min" );
                    ADDFLAG( 0x0040, " Product" );
                    ADDFLAG( 0x0080, " Count" );
                    ADDFLAG( 0x0100, " Stdev" );
                    ADDFLAG( 0x0200, " Stddevp" );
                    ADDFLAG( 0x0400, " Var" );
                    ADDFLAG( 0x0800, " Varp" );
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
                else
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
                    ADDFLAG( 0x01, " fHidden" );
                    ADDFLAG( 0x02, " fHideDetail" );
                    ADDFLAG( 0x04, " fFormula" );
                    ADDFLAG( 0x08, " fMissing" );
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
                    ADDFLAG( 0x0001, " fMultiDataName" );
                    ADDFLAG( 0x0200, " fSub" );
                    ADDFLAG( 0x0400, " fBlock" );
                    ADDFLAG( 0x0800, " fGrand" );
                    ADDFLAG( 0x1000, " fMultiDataOnAxis" );
                    ADDFLAG( 0x2000, " fBlankLine" );       // undocumented
                    ADDFLAG( 0x4000, " fHideDetail" );      // undocumented
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
                UINT32 nRK;
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
                    ADDFLAG( 0x0001, " fInIndexList" );
                    ADDFLAG( 0x0002, " fNotInList" );
                    ADDFLAG( 0x0200, " fLongIndex" );
                    ADDTEXT( "   data type: " );
                    __nFlags &= 0x0DFC;
                    switch( __nFlags )
                    {
                        case 0x0480:    ADDTEXT( "string" ); break;
                        case 0x0520:    ADDTEXT( "double (fraction)" ); break;
                        case 0x0560:    ADDTEXT( "double (int only)" ); break;
                        case 0x05A0:    ADDTEXT( "string & double (fraction)" ); break;
                        case 0x05E0:    ADDTEXT( "string & double (int only)" ); break;
                        case 0x0900:    ADDTEXT( "date" ); break;
                        case 0x0D00:    ADDTEXT( "date & (any) double" ); break;
                        case 0x0D80:    ADDTEXT( "date & string (& double?)" ); break;
                        default:        ADDTEXT( pU );
                    }
                    PRINT();
                    LINESTART();
                    ADDTEXT( pPre );
                    ADDTEXT( "unknown: " );             ADDHEX( 2 );
                    ADDTEXT( "   unknown: " );          ADDHEX( 2 );
                    ADDTEXT( "   item count #1: " );    ADDDEC( 2 );
                    PRINT();
                    LINESTART();
                    ADDTEXT( pPre );
                    ADDTEXT( "unknown: " );             ADDHEX( 2 );
                    ADDTEXT( "   unknown: " );          ADDHEX( 2 );
                    ADDTEXT( "   item count #2: " );    ADDDEC( 2 );
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
                        ADDTEXT( "name: " );
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
                for( UINT16 iIndex = 0; iIndex < rIn.GetRecLen(); iIndex++ )
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
            case 0x00CD:        // SXSTRING - ByteString
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
            case 0x00D5:        // SXIDSTM - pivot table cache stream id
            {
                LINESTART();
                UINT16      nStrId = Read2( rIn );
                ADDTEXT( "Stream ID: " );
                __AddHex( t, nStrId );
                PRINT();
                DumpPivotCache( nStrId );
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
                EscherDump( nL );
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
                UINT32 __nFlags = Read4( rIn );
                STARTFLAG();
                if( __nFlags & 0x009F )
                {
                    ADDFLAG( 0x0001, " fShowAllItems" );
                    ADDFLAG( 0x0002, " fDragToRow" );
                    ADDFLAG( 0x0004, " fDragToColumn" );
                    ADDFLAG( 0x0008, " fDragToPage" );
                    ADDFLAG( 0x0010, " fDragToHide" );
                    ADDFLAG( 0x0080, " fServerBased" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0xBF00 )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x0200, " fAutoSort" );
                    ADDFLAG( 0x0400, " fAscendSort" );
                    ADDFLAG( 0x0800, " fAutoShow" );
                    ADDFLAG( 0x1000, " fAscendShow" );
                    ADDFLAG( 0x2000, " fCalculatedField" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0x00FF4000 )
                {
                    ADDTEXT( "   " );                               // Layout flags:
                    ADDFLAG( 0x00004000, " fLONewPage" );           // undocumented
                    ADDFLAG( 0x00200000, " fLOReport" );            // undocumented
                    ADDFLAG( 0x00400000, " fLOBlankLine" );         // undocumented
                    ADDFLAG( 0x00800000, " fLOSubTotalTop" );       // undocumented
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0xFF000060 )
                {
                    ADDTEXT( "    !RESERVED!" );
                    PRINT();
                }
                if( !__nFlags )
                    PRINT();
                LINESTART();
                ADDTEXT( "reserved = " );
                __AddHex( t, Read1( rIn ) );
                ADDTEXT( "  citmShow = " );
                ADDDEC( 1 );
                ADDTEXT( "  isxdiSort = " );
                ADDHEX( 2 );
                ADDTEXT( "  isxdiShow = " );
                ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "number format = " );
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
                ADDFLAG( 0x0001, " fAuto" );
                ADDRESERVED( 0xFFFE );
                UINT16 nCol1, nRow1, nCol2, nRow2;
                rIn >> nRow1 >> nRow2 >> nCol1 >> nCol2;
                ADDTEXT( "   range: " );        __AddRef( t, nCol1, nRow1 );
                ADDTEXT( ":" );                 __AddRef( t, nCol2, nRow2 );
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
                ADDTEXT( "   address: " );      __AddRef( t, nCol, nRow );
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
                            __AddRK( t, Read4( rIn ) );
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
                            __AddRK( t, Read4( rIn ) );
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
                ADDTEXT( " " );                         __AddRef( t, nCol11, nRow11 );
                ADDTEXT( ":" );                         __AddRef( t, nCol12, nRow12 );
                ADDTEXT( "   to: tab=" );               __AddDec( t, nTab2 );
                ADDTEXT( " " );                         __AddRef( t, nCol21, nRow21 );
                ADDTEXT( ":" );                         __AddRef( t, nCol22, nRow22 );
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
            case 0x0196:
            {
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
                ADDFLAG( 0x0001, " fDsplFormulaBar" );
                ADDFLAG( 0x0002, " fDsplStatus" );
                ADDFLAG( 0x0004, " fNoteOff" );
                ADDFLAG( 0x0008, " fDsplHScroll" );
                ADDFLAG( 0x0010, " fDsplVScroll" );
                ADDFLAG( 0x0020, " fBotAdornment" );
                ADDFLAG( 0x0040, " fZoom" );
                ADDFLAG( 0x0080, " fShowPlaceHld" );
                ADDFLAG( 0x0100, " fHideAll" );
                if( !(__nFlags && 0x0180) )
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
                    ADDFLAG( 0x00000001, " fShowPgBrk" );
                    ADDFLAG( 0x00000002, " fDsplForml" );
                    ADDFLAG( 0x00000004, " fDsplGrid" );
                    ADDFLAG( 0x00000008, " fDsplRCHead" );
                    ADDFLAG( 0x00000010, " fDsplGuts" );
                    ADDFLAG( 0x00000020, " fDsplZeros" );
                    ADDFLAG( 0x00000040, " fPrintHorC" );
                    ADDFLAG( 0x00000080, " fPrintVerC" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0x00007F00 )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x00000100, " fPrintRCHead" );
                    ADDFLAG( 0x00000200, " fPrintGrid" );
                    ADDFLAG( 0x00000400, " fFitToPage" );
                    ADDFLAG( 0x00000800, " fPrintArea" );
                    ADDFLAG( 0x00001000, " fOnePrintArea" );
                    ADDFLAG( 0x00002000, " fFilter" );
                    ADDFLAG( 0x00004000, " fAutoFilter" );
                    PRINT();
                    LINESTART();
                }
                if( __nFlags & 0xFFF80000 )
                {
                    ADDTEXT( "   " );
                    ADDFLAG( 0x00020000, " fSplitV" );
                    ADDFLAG( 0x00040000, " fSplitH" );
                    ADDFLAG( 0x00180000, " fHiddenRow" );
                    ADDFLAG( 0x00200000, " fHiddenCol" );
                    ADDFLAG( 0x01000000, " fChartSize" );
                    ADDFLAG( 0x02000000, " fFilterUnique" );
                    ADDFLAG( 0x04000000, " fLayoutView" );
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
                ADDFLAG( 0x0001, " fTitles" );
                ADDFLAG( 0x0002, " fRowNums" );
                ADDFLAG( 0x0004, " fDisRefr" );
                ADDFLAG( 0x0080, " fFill" );
                ADDFLAG( 0x0100, " fAutoFmt" );
                ADDFLAG( 0x0400, " fDisEdit" );
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
                PreDump( nL );

                INT32   nCntDwn = nL;
                UINT16  nNum;
                LINESTART();
                ADDTEXT( "# of formats: " );
                rIn >> nNum;
                __AddDec( t, nNum );
                rIn >> __nFlags;
                ADDTEXT( "   " );
                STARTFLAG();
                ADDFLAG( 0x0001, " fToughRecalc" );
                ADDRESERVED( 0xFFFE );
                PRINT();
                LINESTART();
                ADDTEXT( "range: " );
                UINT16  nR1, nR2, nC1, nC2;
                rIn >> nR1 >> nR2 >> nC1 >> nC2;
                nCntDwn -= 12;
                ADDCOLROW( nC1, nR1 );
                if( nC1 != nC2 || nR1 != nR2 )
                {
                    ADDTEXT( ":" );
                    ADDCOLROW( nC2, nR2 );
                }
                PRINT();
                UINT16  nC = 0;
                UINT16  nMac;
                while( nCntDwn >= 10 )
                {
                    LINESTART();
                    ADDTEXT( "SQREF #" );
                    __AddDec( t, nC );
                    ADDTEXT( " [" );
                    rIn >> nMac;
                    nCntDwn -= 2;
                    __AddDec( t, nMac );
                    ADDTEXT( "]" );
                    PRINT();
                    if( nMac )
                    {
                        LINESTART();
                        ADDTEXT( pPre );
                        while( nMac && nCntDwn >= 8 )
                        {
                            rIn >> nR1 >> nR2 >> nC1 >> nC2;
                            nCntDwn -= 8;
                            ADDCOLROW( nC1, nR1 );
                            if( nC1 != nC2 || nR1 != nR2 )
                            {
                                ADDTEXT( ":" );
                                ADDCOLROW( nC2, nR2 );
                            }
                            nMac--;
                            if( nMac )
                                ADDTEXT( " - " );
                        }
                        PRINT();
                    }

                    nC++;
                }
                if( nCntDwn > 0 )
                    ContDump( nCntDwn );
            }
            break;
            case 0x01B1:        // CF - conditional format
            {
                UINT8   nCcf, nCp;
                UINT16  nCce1, nCce2;
                ULONG   nStartPos = rIn.GetRecPos();
                rIn >> nCcf >> nCp >> nCce1 >> nCce2;
                LINESTART();
                ADDTEXT( "type (" );
                __AddHex( t, nCcf );
                ADDTEXT( "): " );
                switch( nCcf )
                {
                    case 0x01:  p = "cond";         break;
                    case 0x02:  p = "form";         break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                ADDTEXT( "   operator (" );
                __AddHex( t, nCp );
                ADDTEXT( "): " );
                switch( nCp )
                {
                    case 0x00:  p = "-/-";          break;
                    case 0x01:  p = "between";      break;
                    case 0x02:  p = "not between";  break;
                    case 0x03:  p = "=";            break;
                    case 0x04:  p = "!=";           break;
                    case 0x05:  p = ">";            break;
                    case 0x06:  p = "<";            break;
                    case 0x07:  p = ">=";           break;
                    case 0x08:  p = "<=";           break;
                    default:    p = NULL;
                }
                if( p )
                    ADDTEXT( p );
                else
                    __AddHex( t, nCp );
                PRINT();
                LINESTART();
                ADDTEXT( "cce1: " );
                __AddDec( t, nCce1 );
                ADDTEXT( "  cce2: " );
                __AddDec( t, nCce2 );
                PRINT();

                ULONG nPreForm = rIn.GetRecLeft() - nCce1 - nCce2;
                if( nPreForm > 0 )
                {
                    LINESTART();
                    ADDTEXT( "len rgbdxf: " );
                    __AddDec( t, nPreForm );
                    PRINT();

                    ULONG nPosPreForm = rIn.GetRecPos();
                    ContDump( nPreForm );

                    rIn.Seek( nPosPreForm );    // start
                    UINT32  nFlags = Read4( rIn );
                    IGNORE( 2 );
                    BOOL    bFont = TRUEBOOL( nFlags & 0x04000000 );
                    BOOL    bLine = TRUEBOOL( nFlags & 0x10000000 );
                    BOOL    bPatt = TRUEBOOL( nFlags & 0x20000000 );

                    LINESTART();
                    ADDTEXT( "start = " );
                    __AddPureBin( t, nFlags );
                    PRINT();

                    if( bFont )
                    {
                        LINESTART();
                        ADDTEXT( "- FONT -   [ 118 ]" );
                        PRINT();

                        LINESTART();
                        ADDTEXT( "const:     [  20 ]" );
                        PRINT();
                        ContDump( 20 );

                        LINESTART();
                        ADDTEXT( "1. var:    [  44 ]" );
                        PRINT();
                        ContDump( 44 );

                        LINESTART();
                        ADDTEXT( "const:     [   4 ]" );
                        PRINT();
                        ContDump( 4 );

                        LINESTART();
                        ADDTEXT( "2. var:    [  33 ]" );
                        PRINT();
                        ContDump( 33 );

                        LINESTART();
                        ADDTEXT( "const:     [  17 ]" );
                        PRINT();
                        ContDump( 17 );
                    }

                    if( bLine )
                    {
                        LINESTART();
                        ADDTEXT( "- BORDER - [   8 ]" );
                        PRINT();
                        ContDump( 8 );
                    }

                    if( bPatt )
                    {
                        LINESTART();
                        ADDTEXT( "- PATTERN -[   4 ]" );
                        PRINT();
                        LINESTART();
                        UINT16 nVal;
                        rIn >> nVal;
                        ADDTEXT( "Pattern(" );      __AddHex( t, nVal );
                        ADDTEXT( "): " );           __AddDec( t, (UINT16)((nVal >> 10) & 0x003F) );
                        rIn >> nVal;
                        ADDTEXT( "   Colors(" );    __AddHex( t, nVal );
                        ADDTEXT( "): fg=" );        __AddDec( t, (UINT16)((nVal >> 7) & 0x007F) );
                        ADDTEXT( " bg=" );          __AddDec( t, (UINT16)(nVal & 0x007F) );
                        PRINT();
                    }

                    rIn.Seek( nPosPreForm + nPreForm );
                }

                if( rIn.GetRecLeft() >= nCce1 )
                {
                    LINESTART();
                    ADDTEXT( "form1:" );
                    PRINT();
                    FormulaDump( nCce1, FT_RangeName );
                }
                if( rIn.GetRecLeft() >= nCce2 )
                {
                    LINESTART();
                    ADDTEXT( "form2:" );
                    PRINT();
                    FormulaDump( nCce2, FT_RangeName );
                }
            }
            break;
            case 0x01B2:    // DVAL - header of DV recs
            {
                PreDump( nL );

                rIn >> __nFlags;
                LINESTART();
                STARTFLAG();
                ADDTEXT( " (" );
                __AddPureBin( t, __nFlags );
                ADDTEXT( ")" );
                ADDTEXT( "):" );
                ADDFLAG( 0x0001, " fWnClosed" );
                ADDFLAG( 0x0002, " fWnPinned" );
                ADDFLAG( 0x0004, " fCached" );
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
            case 0x01BE:    // DV - data validation record
            {
                PreDump( nL );
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
                    ADDFLAG( 0x00000080, " fStrLookup" );
                    ADDFLAG( 0x00000100, " fAllowBlank" );
                    ADDFLAG( 0x00000200, " fSuppressCombo" );
                    ADDFLAG( 0x00040000, " fShowInputMsg" );
                    ADDFLAG( 0x00080000, " fShowErrorMsg" );
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
                UINT32 n1, n2;
                PreDump( nL );

                LINESTART();
                PRINT();
                ADDTEXT( "Row1 = " );       ADDDEC( 2 );
                ADDTEXT( "  Row2 = " );     ADDDEC( 2 );
                ADDTEXT( "  Col1 = " );     ADDDEC( 2 );
                ADDTEXT( "  Col2 = " );     ADDDEC( 2 );
                PRINT();

                ContDump( 20 );

                UINT32 __nFlags = Read4( rIn );
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x00000001, " fBody" );
                ADDFLAG( 0x00000002, " fAbs" );
                ADDFLAG( 0x00000014, " fDescr" );
                ADDFLAG( 0x00000008, " fMark" );
                ADDFLAG( 0x00000100, " fNetwork" );
                ADDRESERVED( 0xFFFFFEE0 );
                PRINT();

                //description
                if( __nFlags & 0x00000014 )
                {
                    LINESTART();
                    rIn >> n1;
                    ADDTEXT( "## Description ##  len: " );
                    __AddDec( t, n1 );
                    PRINT();
                    ContDump( n1 << 1 );
                }

                // network path
                if( __nFlags & 0x00000100 )
                {
                    LINESTART();
                    rIn >> n1;
                    ADDTEXT( "## Network path ##  len: " );
                    __AddDec( t, n1 );
                    PRINT();
                    ContDump( n1 << 1 );
                }
                // file link or URL
                else if( __nFlags & 0x00000001 )
                {
                    LINESTART();
                    rIn >> n1;
                    LINESTART();
                    ADDTEXT( "## identifier ## " );
                    __AddHex( t, n1 );
                    PRINT();
                    switch( n1 )
                    {
                        case 0x00000303:    // file
                        {
                            ContDump( 12 );
                            LINESTART();
                            ADDTEXT( "## File link ##  up level: " );
                            ADDDEC( 2 );
                            ADDTEXT( "   len (8-bit String): " );
                            rIn >> n2;
                            __AddDec( t, n2 );
                            PRINT();
                            ContDump( n2 );
                            ContDump( 24 );
                            rIn >> n2;
                            LINESTART();
                            ADDTEXT( "Bytes left: " );
                            __AddDec( t, n2 );
                            PRINT();
                            if( !n2 ) break;
                            rIn >> n2;
                            LINESTART();
                            ADDTEXT( "link byte count: " );
                            __AddDec( t, n2 );
                            ADDTEXT( "   unknown: " );
                            ADDHEX( 2 );
                            PRINT();
                            ContDump( n2 );
                        }
                        break;
                        case 0x79EAC9E0:    // URL
                        {
                            ContDump( 12 );
                            rIn >> n2;
                            LINESTART();
                            ADDTEXT( "## URL ##  byte count: " );
                            __AddDec( t, n2 );
                            PRINT();
                            ContDump( n2 );
                        }
                        break;
                        default:
                        {
                            LINESTART();
                            ADDTEXT( "!! UNKNOWN ID !!" );
                            PRINT();
                        }
                        break;
                    }
                }

                // text mark
                if( __nFlags & 0x00000008 )
                {
                    rIn >> n1;
                    LINESTART();
                    ADDTEXT( "## Text mark ##  len: " );
                    __AddDec( t, n1 );
                    PRINT();
                    ContDump( n1 << 1 );
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
                ADDFLAG( 0x0010, " fCollapsed" );
                ADDFLAG( 0x0020, " fRowHeightZero" );
                ADDFLAG( 0x0040, " fUnsynced" );
                ADDFLAG( 0x0080, " fGhostDirty" );
                ADDRESERVED( 0xFF08 );
                PRINT();
                UINT16 nXF;
                rIn >> nXF;
                LINESTART();
                ADDTEXT( "ix to XF: "  );       __AddDec( t, (UINT16)(nXF & 0x0FFF) );
                ADDTEXT( "   add. flags(" );    __AddHex( t, nXF );
                ADDTEXT( "):" );
                ADDFLAG( 0x1000, " fExAsc" );
                ADDFLAG( 0x2000, " fExDsc" );
                ADDRESERVED( 0xC000 );
                PRINT();
            }
            break;
            case 0x0221:
            {
                UINT16  nR1, nR2;
                UINT8   nC1, nC2;
                rIn >> nR1 >> nR2 >> nC1 >> nC2 >> __nFlags;
                LINESTART();
                ADDTEXT( "range: " );
                ADDCOLROW( nC1, nR1 );
                ADDTEXT( " - " );
                ADDCOLROW( nC2, nR2 );
                PRINT();
                LINESTART();
                STARTFLAG();
                ADDFLAG( 0x0001, " fAlwaysCalc" );
                ADDFLAG( 0x0002, " fCalcOnLoad" );
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
                ADDFLAG( 0x0001, " fUnsynced" );
                ADDFLAG( 0x0002, " fRowHtZero" );
                ADDFLAG( 0x0004, " fExAsc" );
                ADDFLAG( 0x0008, " fExDsc" );
                ADDRESERVED( 0xFFF0 );
                PRINT();
                LINESTART();
                ADDTEXT( "default row height: " );
                ADDDEC( 2 );
                PRINT();
            }
            break;
            case 0x027E:
            {
                ADDCELLHEAD();
                ADDTEXT( "   val = " );
                __AddRK( t, Read4( rIn ) );
                PRINT();
            }
            break;
            case 0x04BC:
            {
                UINT16  nR1, nR2;
                UINT8   nC1, nC2;
                LINESTART();
                rIn >> nR1 >> nR2 >> nC1 >> nC2;
                __AddRef( t, nC1, nR1 );
                ADDTEXT( " - " );
                __AddRef( t, nC2, nR2 );
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
                ADDFLAG( 0x0001, " fImportPRE" );
                ADDFLAG( 0x0002, " fIgnoreSep" );
                ADDFLAG( 0x0004, " fUseSetting" );
                ADDFLAG( 0x0010, " fIgnoreDate" );
                ADDFLAG( 0x0020, " fWhatIsIt?" );
                ADDRESERVED( 0xFFC8 );
                PRINT();
                LINESTART();
                rIn >> __nFlags;
                STARTFLAG();
                ADDFLAG( 0x0002, " fTables" );
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
                LINESTART();
                ADDTEXT( "version number:      " );
                ADDHEX( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "substream type:      " );
                UINT16  n;
                rIn >> n;
                __AddHex( t, n );
                ADDTEXT( " (" );
                switch( n )
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
                    ADDFLAG( 0x00000001, " fWin" );
                    ADDFLAG( 0x00000002, " fRisc" );
                    ADDFLAG( 0x00000004, " fBeta" );
                    ADDFLAG( 0x00000008, " fWinAny" );
                    ADDFLAG( 0x00000010, " fMacAny" );
                    ADDFLAG( 0x00000020, " fBetaAny" );
                    ADDFLAG( 0x00000100, " fRiscAny" );
                    ADDFLAG( 0xFFFFE0C0, " fXxxxxxx" );
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
                UINT16  nSdtX, nSdtY, nCValx, nCValy, nSdtBSize, nCValBSize;
                rIn >> nSdtX >> nSdtY >> nCValx >> nCValy >> nSdtBSize >> nCValBSize;
                LINESTART();
                ADDTEXT( "categories: " );
                ADDTEXT( GetSeriesType( nSdtX ) );
                ADDTEXT( "     values: " );
                ADDTEXT( GetSeriesType( nSdtY ) );
                PRINT();
                LINESTART();
                ADDTEXT( "Count of categories = " );
                __AddDec( t, nCValx );
                ADDTEXT( "       Count of values = " );
                __AddDec( t, nCValy );
                PRINT();
                LINESTART();
                ADDTEXT( "Type of data in Bubble size series: " );
                const sal_Char* p = GetSeriesType( nSdtBSize );
                ADDTEXT( p );
                if ( p == pU )
                    __AddHex( t, nSdtBSize );
                ADDTEXT( "    count = " );
                __AddDec( t, nCValBSize );
                PRINT();
            }
                break;
            case 0x1006:        // ChartDataformat
            {
                UINT16  n;
                LINESTART();
                rIn >> n;
                ADDTEXT( "Point number = " );
                __AddDec( t, n );
                if( n == 0xFFFF )
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
                    ADDFLAG( 0x0001, " fAuto" );
                    ADDFLAG( 0x0004, " fDrawTick" );
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
                    ADDFLAG( 0x0001, " fAuto" );
                    ADDFLAG( 0x0010, " fNoBackg" );
                    ADDFLAG( 0x0020, " fNoFore" );
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
                    ADDFLAG( 0x01, " fAuto" );
                    ADDFLAG( 0x02, " fInvertNeg" );
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
                    ADDFLAG( 0x0001, " fVaried" );
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
                    ADDFLAG( 0x01, " fAutoPosition" );
                    ADDFLAG( 0x02, " fAutoSeries" );
                    ADDFLAG( 0x04, " fAutoPosX" );
                    ADDFLAG( 0x08, " fAutoPosY" );
                    ADDFLAG( 0x10, " fVert" );
                    ADDFLAG( 0x20, " fWasDataTable" );
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
                    ADDFLAG( 0x0001, " fTranspose" );
                    ADDFLAG( 0x0002, " fStacked" );
                    ADDFLAG( 0x0004, " f100" );
                    ADDFLAG( 0x0008, " fHasShadow" );
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
                LINESTART();
                switch( Read2( rIn ) )
                {
                    case 0:     p = "category axis or x axis on a scatter chart";   break;
                    case 1:     p = "value axis";                                   break;
                    case 2:     p = "series axis";                                  break;
                    default:    p = pU;
                }
                ADDTEXT( p );
                PRINT();
                ContDump( 16 );
            }
                break;
            case 0x101E:        // ChartTick
            {
                LINESTART();
                switch( Read1( rIn ) )
                {
                    case 0:     p = "invisible";                break;
                    case 1:     p = "inside of axis line";      break;
                    case 2:     p = "outside of axis line";     break;
                    case 3:     p = "cross axis line";          break;
                    default:    p = pU;
                }
                ADDTEXT( "major = " );
                ADDTEXT( p );
                switch( Read1( rIn ) )
                {
                    case 0:     p = "invisible";                break;
                    case 1:     p = "inside of axis line";      break;
                    case 2:     p = "outside of axis line";     break;
                    case 3:     p = "cross axis line";          break;
                    default:    p = pU;
                }
                ADDTEXT( "    minor = " );
                ADDTEXT( p );
                PRINT();
                LINESTART();
                switch( Read1( rIn ) )
                {
                    case 0:     p = "invisible";                break;
                    case 1:     p = "low end";                  break;
                    case 2:     p = "high end";                 break;
                    case 3:     p = "next to axis";             break;
                    default:    p = pU;
                }
                ADDTEXT( "position = " );
                ADDTEXT( p );
                switch( Read1( rIn ) )
                {
                    case 1:     p = "transparent";              break;
                    case 2:     p = "opaque";                   break;
                    default:    p = pU;
                }
                ADDTEXT( "    mode = " );
                ADDTEXT( p );
                ADDTEXT( "    text col = " );
                ADDTEXT( GetRGB( Read4( rIn ) ) );
                PRINT();
                LINESTART();
                ADDTEXT( "reserved = " );
                ADDHEX( 4 ); t += ' ';
                ADDHEX( 4 ); t += ' ';
                ADDHEX( 4 ); t += ' ';
                ADDHEX( 4 );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, " fAutoCol" );
                    ADDFLAG( 0x0002, " fAutoBack" );
                    ADDFLAG( 0x0020, " fAutoRot" );
                    PRINT();
                }
                LINESTART();
                ADDTEXT( "color index = " );
                ADDDEC( 2 );
                ADDTEXT( "   rotation = " );
                ADDDEC( 2 );
                PRINT();
            }
                break;
            case 0x101F:        // ChartValuerange
            {
                LINESTART();
                ADDTEXT( "min val = " );        ADDDOUBLE();
                ADDTEXT( "  max val = " );      ADDDOUBLE();
                PRINT();
                LINESTART();
                ADDTEXT( "major incr = " );     ADDDOUBLE();
                ADDTEXT( "  minor incr = " );   ADDDOUBLE();
                PRINT();
                LINESTART();
                ADDTEXT( "cross val = " );      ADDDOUBLE();
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, " fAutoMin" );
                    ADDFLAG( 0x0002, " fAutoMax" );
                    ADDFLAG( 0x0004, " fAutoMajor" );
                    ADDFLAG( 0x0008, " fAutoMinor" );
                    ADDFLAG( 0x0010, " fAutoCross" );
                    ADDFLAG( 0x0020, " fLogScale" );
                    ADDFLAG( 0x0040, " fReverse" );
                    ADDFLAG( 0x0080, " fMaxCross" );
                    PRINT();
                }
            }
                break;
            case 0x1020:        // ChartCatserrange
            {
                LINESTART();
                ADDTEXT( "Value axis / category crossing point: " );
                ADDDEC( 2 );
                PRINT();
                LINESTART();
                ADDTEXT( "label freq = " );
                ADDDEC( 2 );
                ADDTEXT( "  mark freq = " );
                ADDDEC( 2 );
                PRINT();
                rIn >> __nFlags;
                if( __nFlags )
                {
                    LINESTART();
                    STARTFLAG();
                    ADDFLAG( 0x0001, " fBetween" );
                    ADDFLAG( 0x0002, " fMaxCross" );
                    ADDFLAG( 0x0004, " fReverse" );
                    PRINT();
                }
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
                    ADDFLAG( 0x0001, " fAutoColor" );
                    ADDFLAG( 0x0002, " fShowKey" );
                    ADDFLAG( 0x0004, " fShowValue" );
                    ADDFLAG( 0x0008, " fVert" );
                    ADDFLAG( 0x0010, " fAutoText" );
                    ADDFLAG( 0x0020, " fGenerated" );
                    ADDFLAG( 0x0040, " fDeleted" );
                    ADDFLAG( 0x0080, " fAutoMode" );
                    ADDFLAG( 0x0800, " fShLabPct" );
                    ADDFLAG( 0x1000, " fShowPct" );
                    ADDFLAG( 0x2000, " fShowBubbleSizes" );
                    ADDFLAG( 0x4000, " fShowLabel" );
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
            case 0x1027:        // ChartObjectLink
                ContDump( nL );
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
                    ADDFLAG( 0x01, " fAutoSize" );
                    ADDFLAG( 0x02, " fAutoPosition" );
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
                ContDump( nL );
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
            case 0x104A:        // ChartSerparent
                ContDump( nL );
                break;
            case 0x104B:        // ChartSerauxtrend
                ContDump( nL );
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
                    case 0:     p = "use default categories";   break;
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
                    ADDFLAG( 0x01, " fCustomIfmt" );
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
            case 0x105B:        // ChartSerauxerrbar
                ContDump( nL );
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
                    ADDFLAG( 0x0001, " fAutiMin" );
                    ADDFLAG( 0x0002, " fAutoMax" );
                    ADDFLAG( 0x0004, " fAutoMajor" );
                    ADDFLAG( 0x0008, " fAutoMinor" );
                    ADDFLAG( 0x0010, " fDateAxis" );
                    ADDFLAG( 0x0020, " fAutoBase" );
                    ADDFLAG( 0x0040, " fAutoCross" );
                    ADDFLAG( 0x0080, " fAutoDate" );
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
                ContDump( nL );
                break;
            case 0x1067:        // ChartBoppcustom
                ContDump( nL );
                break;
            default:
                ContDump( nL );
        }

    }

    if( bDec )
        pLevelPre -= nLevelInc;
}


void Biff8RecDumper::DumpSubStream( SvStorage* pStorage, const sal_Char* pStreamName )
{
    ByteString sOutput;

    if( !pStorage )
    {
        sOutput = "-- no storage available --";
        Print( sOutput );
        DBG_ERROR( "Biff8RecDumper::DumpSubStream - storage missing" );
        return;
    }

    String sName;
    sName.AppendAscii( pStreamName );
    if( !pStorage->IsContained( sName ) || !pStorage->IsStream( sName ) )
        return;

    SvStream* pStream = pStorage->OpenStream( sName, STREAM_STD_READ );

    if( !pStream )
    {
        sOutput = "-- no stream available --";
        Print( sOutput );
        DBG_ERROR( "Biff8RecDumper::DumpSubStream - no stream available" );
        return;
    }

    pStream->Seek( STREAM_SEEK_TO_END );
    if( pStream->Tell() == ~((ULONG)0) )
    {
        sOutput = "-- no stream available --";
        Print( sOutput );
        DBG_ERROR( "Biff8RecDumper::DumpSubStream - no stream available" );
        delete pStream;
        return;
    }

    sOutput = "-- substream dump --";
    Print( sOutput );
    sOutput = "Stream name: ";
    sOutput += pStreamName;
    Print( sOutput );

    XclImpStream* pOldStream = pIn;
    CharSet eCharSet = RTL_TEXTENCODING_MS_1252;
    pIn = new XclImpStream( *pStream, &eCharSet );
    XclImpStream& rIn = *pIn;
    rIn.SetWarningMode( bWarnings );

    // -- dump from here --
    UINT16  nId;
    BOOL bLoop = TRUE;

    while( bLoop && rIn.StartNextRecord() )
    {
        nId = rIn.GetRecNum();
        if( HasModeDump( nId ) )
            RecDump( TRUE );

        bLoop = (nId != 0x000A);
    }

    sOutput = "-- end of stream --\n";
    Print( sOutput );
    delete pIn;
    delete pStream;
    pIn = pOldStream;
}


void Biff8RecDumper::DumpPivotCache( const UINT16 nStrId )
{
    ByteString sByteStrName;
    __AddPureHex( sByteStrName, nStrId );
    DumpSubStream( pPivotCache, sByteStrName.GetBuffer() );
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

void Biff8RecDumper::EscherDump( const ULONG nMaxLen )
{
#if 0
// if an entire hex block is needed
    ULONG nPos = pIn->Tell();
    ContDump( nMaxLen );
    pIn->Seek( nPos );
#endif

    ULONG           n = nMaxLen;
    UINT16          nPre, nR;
    UINT32          nL;
    const sal_Char* p;
    ByteString      aT;
    UINT16          nDumpSize;

    aT += pLevelPre;

    while( n > 0 )
    {
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
        else if ( nR == 0xF010 && 0x12 <= n && 0x12 <= nL )
        {   // ClientAnchor
            ULONG nP = pIn->GetRecPos();
            UINT16 n16;

            aT = "    Flag: ";
            *pIn >> n16;
            __AddHex( aT, n16 );
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
        else if ( nR == 0xF00B )
        {   // OPT
            while ( nL >= 6 && n >= 6 )
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
                    while ( n32 && n > 0 )
                    {
                        nDumpSize = (n32 > (UINT32) n) ? (UINT16) n : (UINT16) n32;
                        ContDump( nDumpSize );
                        nL -= nDumpSize;
                        n -= nDumpSize;
                        n32 -= nDumpSize;
                    }
                }
            }
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
                            UINT16 __nFlags = Read2( rIn );
                            if ( __nFlags )
                            {
                                ADDTEXT( "   " );
                                STARTFLAG();
                                ADDFLAG( 0x0008, " fAsSymbol" );
                                ADDFLAG( 0x0002, " fLinked" );
                                ADDFLAG( 0x0001, " f1???" );
                            }
                        }
                        break;
                        case 0x0009 :       // ftPictFmla
                        {
                            ADDTEXT( "    Document type " );
                            UINT16 nFmlaLen;
                            rIn >> nFmlaLen;
                            if ( sizeof(nFmlaLen) + nFmlaLen == nL )
                            {
                                ADDTEXT( "linked\n    OLE stream: LNK........ (ID in EXTERNNAME of SUPBOOK)\n    XTI: " );
                                IGNORE(7);
                                ADDHEX(2);
                                ADDTEXT( "   Externname: " );
                                ADDHEX(2);
                                IGNORE(3);      // MAY be right
                            }
                            else
                            {
                                ADDTEXT( "embedded " );
                                const UINT16 nStringOffset = 14;    // MAY be right
                                rIn.Seek( nPos1 + nStringOffset );
                                INT32 nBytesLeft = nL - nStringOffset;
                                ULONG nPos3 = rIn.GetRecPos();
                                AddUNICODEString( t, rIn );
                                nBytesLeft -= (rIn.GetRecPos() - nPos3);
                                ADDTEXT( '\n' );
                                if ( nBytesLeft < 4 )
                                    ADDTEXT( "    >> ByteString OVERRUN <<\n" );
                                else if ( nBytesLeft == 5 )
                                {
                                    ADDTEXT( "    pad byte " );
                                    ADDHEX(1);
                                    ADDTEXT( '\n' );
                                }
                                else if ( nBytesLeft == 4 )
                                    ADDTEXT( "    no pad byte\n" );
                                else
                                    ADDTEXT( "    oops.. bytes left?!?\n" );

                                ADDTEXT( "    OLE stream: MBD" );
                                rIn.Seek( nPos1 + sizeof(nFmlaLen) + nFmlaLen );
                                UINT32 nOleId;
                                rIn >> nOleId;
                                __AddPureHex( t, nOleId );
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


#undef  ADDFLAG
#undef  LINESTART
#undef  IGNORE
#undef  ADDHEX
#undef  ADDDEC
#undef  ADDTEXT
#undef  ADDCOLROW
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
                aT += *p;
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


void Biff8RecDumper::ContDumpStream( SvStream& rStrm, const ULONG nL )
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

        rStrm.Read( pB, nInL );

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
                aT += *p;
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



//_____________________________________________________________________________
// Formula dumper

/// Name and parameter count of an Excel function.
struct XclDumpFunc
{
    const sal_Char*             pName;          /// Name of the function.
    sal_uInt16                  nIndex;         /// Excel built-in function index.
    sal_uInt16                  nParam;         /// Parameter count for fixed functions.
};

// ! some of the function names are wrong
const XclDumpFunc pFuncData[] =
{
    { "COUNT",            0,    0 },
    { "IF",               1,    0 },
    { "ISNV",             2,    1 },
    { "ISERROR",          3,    1 },
    { "SUM",              4,    0 },
    { "AVERAGE",          5,    0 },
    { "MIN",              6,    0 },
    { "MAX",              7,    0 },
    { "ROW",              8,    0 },
    { "COLUMN",           9,    0 },
    { "NOVALUE",          10,   0 },
    { "NBW",              11,   0 },
    { "STDEV",            12,   0 },
    { "CURRENCY",         13,   0 },
    { "FIXED",            14,   0 },
    { "SIN",              15,   1 },
    { "COS",              16,   1 },
    { "TAN",              17,   1 },
    { "ARCTAN",           18,   1 },
    { "PI",               19,   0 },
    { "SQRT",             20,   1 },
    { "EXP",              21,   1 },
    { "LN",               22,   1 },
    { "LOG10",            23,   1 },
    { "ABS",              24,   1 },
    { "INT",              25,   1 },
    { "PLUSMINUS",        26,   1 },
    { "ROUND",            27,   2 },
    { "LOOKUP",           28,   0 },
    { "INDEX",            29,   0 },
    { "REPT",             30,   2 },
    { "MID",              31,   3 },
    { "LEN",              32,   1 },
    { "VALUE",            33,   1 },
    { "TRUE",             34,   0 },
    { "FALSE",            35,   0 },
    { "AND",              36,   0 },
    { "OR",               37,   0 },
    { "NOT",              38,   1 },
    { "MOD",              39,   2 },
    { "DBCOUNT",          40,   3 },
    { "DBSUM",            41,   3 },
    { "DBAVERAGE",        42,   3 },
    { "DBMIN",            43,   3 },
    { "DBMAX",            44,   3 },
    { "DBSTDDEV",         45,   3 },
    { "VAR",              46,   0 },
    { "DBVAR",            47,   3 },
    { "TEXT",             48,   2 },
    { "RGP",              49,   0 },
    { "TREND",            50,   0 },
    { "RKP",              51,   0 },
    { "GROWTH",           52,   0 },
    { "BW",               56,   0 },
    { "ZW",               57,   0 },
    { "ZZR",              58,   0 },
    { "RMZ",              59,   0 },
    { "ZINS",             60,   0 },
    { "MIRR",             61,   3 },
    { "IRR",              62,   0 },
    { "RANDOM",           63,   0 },
    { "MATCH",            64,   0 },
    { "DATE",             65,   3 },
    { "TIME",             66,   3 },
    { "DAY",              67,   1 },
    { "MONTH",            68,   1 },
    { "YEAR",             69,   1 },
    { "DAYOFWEEK",        70,   0 },
    { "HOUR",             71,   1 },
    { "MIN",              72,   1 },
    { "SEC",              73,   1 },
    { "NOW",              74,   0 },
    { "AREAS",            75,   1 },
    { "ROWS",             76,   1 },
    { "COLUMNS",          77,   1 },
    { "OFFSET",           78,   0 },
    { "SEARCH",           82,   0 },
    { "TRANSPOSE",        83,   1 },
    { "TYPE",             86,   1 },
    { "ARCTAN2",          97,   2 },
    { "ARCSIN",           98,   1 },
    { "ARCCOS",           99,   1 },
    { "CHOSE",            100,  0 },
    { "HLOOKUP",          101,  0 },
    { "VLOOKUP",          102,  0 },
    { "ISREF",            105,  1 },
    { "LOG",              109,  0 },
    { "CHAR",             111,  1 },
    { "LOWER",            112,  1 },
    { "UPPER",            113,  1 },
    { "PROPPER",          114,  1 },
    { "LEFT",             115,  0 },
    { "RIGHT",            116,  0 },
    { "EXACT",            117,  2 },
    { "TRIM",             118,  1 },
    { "REPLACE",          119,  4 },
    { "SUBSTITUTE",       120,  0 },
    { "CODE",             121,  1 },
    { "FIND",             124,  0 },
    { "CELL",             125,  0 },
    { "ISERR",            126,  1 },
    { "ISSTRING",         127,  1 },
    { "ISVALUE",          128,  1 },
    { "ISEMPTY",          129,  1 },
    { "T",                130,  1 },
    { "N",                131,  1 },
    { "DATEVALUE",        140,  1 },
    { "TIMEVALUE",        141,  1 },
    { "LIA",              142,  3 },
    { "DIA",              143,  4 },
    { "GDA",              144,  0 },
    { "INDIRECT",         148,  0 },
    { "CLEAN",            162,  1 },
    { "MATDET",           163,  1 },
    { "MATINV",           164,  1 },
    { "MATMULT",          165,  2 },
    { "ZINSZ",            167,  0 },
    { "KAPZ",             168,  0 },
    { "COUNT2",           169,  0 },
    { "PRODUCT",          183,  0 },
    { "FACT",             184,  1 },
    { "DBPRODUCT",        189,  3 },
    { "ISNONSTRING",      190,  1 },
    { "STDEVP",           193,  0 },
    { "VARP",             194,  0 },
    { "DBSTDDEVP",        195,  3 },
    { "DBVARP",           196,  3 },
    { "TRUNC",            197,  0 },
    { "ISLOGICAL",        198,  1 },
    { "DBCOUNT2",         199,  3 },
    { "ROUNDUP",          212,  2 },
    { "ROUNDDOWN",        213,  2 },
    { "RANK",             216,  0 },
    { "ADDRESS",          219,  0 },
    { "GETDIFFDATE360",   220,  0 },
    { "TODAY",            221,  0 },
    { "VBD",              222,  0 },
    { "MEDIAN",           227,  0 },
    { "SUMPRODUCT",       228,  0 },
    { "SINHYP",           229,  1 },
    { "COSHYP",           230,  1 },
    { "TANHYP",           231,  1 },
    { "ARCSINHYP",        232,  1 },
    { "ARCCOSHYP",        233,  1 },
    { "ARCTANHYP",        234,  1 },
    { "DBGET",            235,  3 },
    { "GDA2",             247,  0 },
    { "FREQUENCY",        252,  2 },
    { "ERRORTYPE",        261,  1 },
    { "AVEDEV",           269,  0 },
    { "BETADIST",         270,  0 },
    { "GAMMALN",          271,  1 },
    { "BETAINV",          272,  0 },
    { "BINOMDIST",        273,  4 },
    { "CHIDIST",          274,  2 },
    { "CHIINV",           275,  2 },
    { "KOMBIN",           276,  2 },
    { "CONFIDENCE",       277,  3 },
    { "KRITBINOM",        278,  3 },
    { "EVEN",             279,  1 },
    { "EXPDIST",          280,  3 },
    { "FDIST",            281,  3 },
    { "FINV",             282,  3 },
    { "FISHER",           283,  1 },
    { "FISHERINV",        284,  1 },
    { "FLOOR",            285,  2 },
    { "GAMMADIST",        286,  4 },
    { "GAMMAINV",         287,  3 },
    { "CEIL",             288,  2 },
    { "HYPGEOMDIST",      289,  4 },
    { "LOGNORMDIST",      290,  3 },
    { "LOGINV",           291,  3 },
    { "NEGBINOMVERT",     292,  3 },
    { "NORMDIST",         293,  4 },
    { "STDNORMDIST",      294,  1 },
    { "NORMINV",          295,  3 },
    { "SNORMINV",         296,  1 },
    { "STANDARD",         297,  3 },
    { "ODD",              298,  1 },
    { "VARIATIONEN",      299,  2 },
    { "POISSONDIST",      300,  3 },
    { "TDIST",            301,  3 },
    { "WEIBULL",          302,  4 },
    { "SUMXMY2",          303,  2 },
    { "SUMX2MY2",         304,  2 },
    { "SUMX2DY2",         305,  2 },
    { "CHITEST",          306,  2 },
    { "CORREL",           307,  2 },
    { "COVAR",            308,  2 },
    { "FORECAST",         309,  3 },
    { "FTEST",            310,  2 },
    { "INTERCEPT",        311,  2 },
    { "PEARSON",          312,  2 },
    { "RSQ",              313,  2 },
    { "STEYX",            314,  2 },
    { "SLOPE",            315,  2 },
    { "TTEST",            316,  4 },
    { "PROB",             317,  0 },
    { "DEVSQ",            318,  0 },
    { "GEOMEAN",          319,  0 },
    { "HARMEAN",          320,  0 },
    { "SUMSQ",            321,  0 },
    { "KURT",             322,  0 },
    { "SCHIEFE",          323,  0 },
    { "ZTEST",            324,  0 },
    { "LARGE",            325,  2 },
    { "SMALL",            326,  2 },
    { "QUARTILE",         327,  2 },
    { "PERCENTILE",       328,  2 },
    { "PERCENTRANK",      329,  0 },
    { "MODALVALUE",       330,  0 },
    { "TRIMMEAN",         331,  2 },
    { "TINV",             332,  2 },
    { "CONCAT",           336,  0 },
    { "POWER",            337,  2 },
    { "RAD",              342,  1 },
    { "DEG",              343,  1 },
    { "SUBTOTAL",         344,  0 },
    { "SUMIF",            345,  0 },
    { "COUNTIF",          346,  2 },
    { "COUNTEMPTYCELLS",  347,  1 },
    { "ROMAN",            354,  0 },
    { "EXTERNAL",         255,  0 },
    { "ISPMT",            350,  4 },
    { "AVERAGEA",         361,  0 },
    { "MAXA",             362,  0 },
    { "MINA",             363,  0 },
    { "STDEVPA",          364,  0 },
    { "VARPA",            365,  0 },
    { "STDEVA",           366,  0 },
    { "VARA",             367,  0 }
};

const XclDumpFunc* lcl_GetFuncData( sal_uInt16 nIndex )
{
    const XclDumpFunc* pFirst = pFuncData;
    const XclDumpFunc* pLast = pFuncData + sizeof( pFuncData ) / sizeof( XclDumpFunc ) - 1;
    while( pFirst <= pLast )
    {
        const XclDumpFunc* pCurr = pFirst + (pLast - pFirst) / 2;
        if( pCurr->nIndex > nIndex )
            pLast = pCurr - 1;
        else if( pCurr->nIndex < nIndex )
            pFirst = pCurr + 1;
        else
            return pCurr;
    }
    return NULL;
}



/// Stack to create a human readable formula from UPN.
class XclDumpFormulaStack : private ScfObjStack< ByteString >
{
public:
                                ScfObjStack< ByteString >::Top;

    void                        PushOperand( const ByteString& rName );
    void                        PushOperand( const sal_Char* pName );
    void                        PushUnary( const sal_Char* pOperator, sal_Bool bInFront = sal_True );
    void                        PushBinary( const sal_Char* pOperator );
    void                        PushFunction( const ByteString& rName, sal_uInt16 nParam );
};

void XclDumpFormulaStack::PushOperand( const ByteString& rName )
{
    Push( new ByteString( rName ) );
}

void XclDumpFormulaStack::PushOperand( const sal_Char* pName )
{
    Push( new ByteString( pName ) );
}

void XclDumpFormulaStack::PushUnary( const sal_Char* pOperator, sal_Bool bInFront )
{
    if( !Count() ) return;
    ByteString* pOp = Top();
    pOp->Insert( pOperator, bInFront ? 0 : pOp->Len() );
}

void XclDumpFormulaStack::PushBinary( const sal_Char* pOperator )
{
    if( Count() < 2 ) return;
    // second operand is on top
    ByteString* pSecond = Pop();
    ByteString* pFirst = Top();
    *pFirst += pOperator;
    *pFirst += *pSecond;
    delete pSecond;
}

void XclDumpFormulaStack::PushFunction( const ByteString& rName, sal_uInt16 nParam )
{
    if( Count() < nParam ) return;
    ByteString* pNew = new ByteString( ')' );
    for( sal_uInt16 nIndex = 0; nIndex < nParam; ++nIndex )
    {
        if( nIndex ) pNew->Insert( ';', 0 );
        ByteString* pOp = Pop();
        pNew->Insert( *pOp, 0 );
        delete pOp;
    }
    pNew->Insert( '(', 0 );
    pNew->Insert( rName, 0 );
    Push( pNew );
}



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



void lcl_StartToken( ByteString& rString, sal_uInt8 nToken, const sal_Char* pTokenName )
{
    rString.Erase();
    rString += "    ";
    __AddHex( rString, nToken );
    rString += " ptg";
    rString += pTokenName;
    rString.Expand( 24, ' ' );
}

void lcl_StartTokenClass( ByteString& rString, sal_uInt8 nToken, const sal_Char* pTokenName )
{
    ByteString aToken( pTokenName );
    switch( nToken & 0xE0 )
    {
        case 0x20:  aToken += 'R';  break;
        case 0x40:  aToken += 'V';  break;
        case 0x60:  aToken += 'A';  break;
        default:    aToken += '?';
    }
    lcl_StartToken( rString, nToken, aToken.GetBuffer() );
}

void Biff8RecDumper::FormulaDump( const UINT16 nL, const FORMULA_TYPE eFT )
{
    if( !nL )
        return;

    const ULONG             nAfterPos = pIn->GetRecPos() + nL;
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
#define STARTTOKEN( name )      lcl_StartToken( t, nOp, name )
#define STARTTOKENCLASS( name ) lcl_StartTokenClass( t, nOp, name )

    while( ( pIn->GetRecPos() < nAfterPos ) && !bError )
    {
        *pIn >> nOp;
        bPrinted = sal_False;
        aOperand.Erase();

        switch( nOp )   //                              Buch Seite:
        {           //                                      SDK4 SDK5
            case 0x01: // Array Formula                         [325    ]
                    // Array Formula or Shared Formula       [    277]
                STARTTOKEN( "Exp" );
                t += "array formula or shared formula";
                pIn->Ignore( 4 );
                aStack.PushOperand( "ARRAY()" );
            break;
            case 0x02: // Data Table                            [325 277]
                STARTTOKEN( "Tbl" );
                t += "multiple operation";
                pIn->Ignore( 4 );
                aStack.PushOperand( "MULTIPLE.OPERATIONS()" );
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
            case 0x10: // Union                                 [314 265]
                STARTTOKEN( "Union" );
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
                aStack.PushUnary( "%", sal_False );
            break;
            case 0x15: // Parenthesis                           [326 278]
                STARTTOKEN( "Paren" );
                // simulate two unary operators to enclose operand
                aStack.PushUnary( "(" );
                aStack.PushUnary( ")", sal_False );
            break;
            case 0x16: // Missing Argument                      [314 266]
                STARTTOKEN( "MissArg" );
                aStack.PushUnary( "" );
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
                aStack.PushOperand( aOperand );
            }
            break;
            case 0x18:
            {
                STARTTOKEN( "NatFmla" );
                UINT8   nEptg;
                *pIn >> nEptg;

#define D(name,size,ext,type)   {t+="eptg ";__AddDec(t,(UINT16)nEptg);t+=": ";              \
                                t+=name;t+=" [";__AddDec(t,(UINT16)size);t+="] ";t+=type;   \
                                if(ext)t+=" + ext";PRINTTOKEN();ContDump(size);aStack.PushOperand(name);}
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

                if( nOpt & 0x04 )
                {// nFakt -> Bytes oder Words ueberlesen    AttrChoose
                    t += " AttrChoose";
                    nData++;
                    pIn->Ignore( nData * nFakt );
                }
                else if( nOpt & 0x10 )                      // AttrSum
                {
                    t += " AttrSum";
                    aStack.PushFunction( "SUM", 1 );
                }
                else
                    t += " AttrMISC";
            }
            break;
            case 0x1C: // Error Value                           [314 266]
            {
                STARTTOKEN( "Err" );
                sal_uInt8 nErr = pIn->ReaduInt8();
                t += "error value (";           __AddHex( t, nErr );
                t += ") ";
                t += lcl_GetErrorString( nErr );
                aStack.PushOperand( lcl_GetErrorString( nErr ) );
            }
            break;
            case 0x1D: // Boolean                               [315 266]
                STARTTOKEN( "Bool" );
                aOperand += pIn->ReaduInt8() ? "FALSE" : "TRUE";
                t += aOperand;
                aStack.PushOperand( aOperand );
            break;
            case 0x1E: // Integer                               [315 266]
                STARTTOKEN( "Int" );
                __AddDec( aOperand, pIn->ReaduInt16() );
                t += aOperand;
                aStack.PushOperand( aOperand );
            break;
            case 0x1F: // Number                                [315 266]
                STARTTOKEN( "Num" );
                __AddDouble( aOperand, pIn->ReadDouble() );
                t += aOperand;
                aStack.PushOperand( aOperand );
            break;
            case 0x20: // Array Constant                        [317 268]
            case 0x40:
            case 0x60:
                STARTTOKENCLASS( "Array" );
                pIn->Ignore( 7 );
                aStack.PushOperand( "ConstArray" );
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
                aStack.PushFunction( aOperand, pFunc ? pFunc->nParam : 0 );
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
                aStack.PushFunction( aOperand, nAnz & 0x7F );
            }
            break;
            case 0x23: // Name                                  [318 269]
            case 0x43:
            case 0x63:
            {
                STARTTOKENCLASS( "Name" );
                __AddDec( aOperand, pIn->ReaduInt16() );
                t += "internal name: index=";
                t += aOperand;
                pIn->Ignore( 2 );
                aOperand.Insert( "NAME(", 0 ).Append( ')' );
                aStack.PushOperand( aOperand );
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
                aStack.PushOperand( aOperand );
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
                aStack.PushOperand( aOperand );
            }
            break;
            case 0x26: // Constant Reference Subexpression      [321 271]
            case 0x46:
            case 0x66:
                STARTTOKENCLASS( "MemArea" );
                pIn->Ignore( 6 );
                aStack.PushOperand( "MemArea" );
            break;
            case 0x27: // Erroneous Constant Reference Subexpr. [322 272]
            case 0x47:
            case 0x67:
                STARTTOKENCLASS( "MemErr" );
                pIn->Ignore( 6 );
            break;
            case 0x28: // Incomplete Constant Reference Subexpr.[331 281]
            case 0x48:
            case 0x68:
                STARTTOKENCLASS( "MemNoMem" );
                pIn->Ignore( 6 );
            break;
            case 0x29: // Variable Reference Subexpression      [331 281]
            case 0x49:
            case 0x69:
                STARTTOKENCLASS( "MemFunc" );
                pIn->Ignore( 2 );
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
                aStack.PushOperand( aOperand );
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
                aStack.PushOperand( aOperand );
            }
            break;
            case 0x2E: // Reference Subexpression Within a Name [332 282]
            case 0x4E:
            case 0x6E:
                STARTTOKENCLASS( "MemAreaN" );
                pIn->Ignore( 2 );
            break;
            case 0x2F: // Incomplete Reference Subexpression... [332 282]
            case 0x4F:
            case 0x6F:
                STARTTOKENCLASS( "MemNoMemN" );
                pIn->Ignore( 2 );
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
                aStack.PushOperand( aOperand );
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
                aStack.PushOperand( aOperand );
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
                aStack.PushOperand( aOperand );
            }
            break;
            default:
                STARTTOKEN( "unknown" );
                bError = TRUE;
        }
        PRINTTOKEN();
    }
    t.Erase();
    t += "  Formula = ";
    t += aStack.Top() ? *aStack.Top() : "ERROR IN STACK";
    Print( t );
    pIn->Seek( nAfterPos );
}


void Biff8RecDumper::ControlsDump( SvStream& rIn )
{
    if( !pDumpStream )
        return;

    rIn.Seek( STREAM_SEEK_TO_END );
    ULONG nLen = rIn.Tell();
    rIn.Seek( STREAM_SEEK_TO_BEGIN );

    if( nLen == ~0UL )
        return;

    *pDumpStream << "### start Ctls stream ###\n";
    while( nLen )
    {
        UINT16  nPart = ( nLen >= 1024 )? 1024 : ( UINT16 ) nLen;
        ContDumpStream( rIn, nPart );
        nLen -= nPart;
    }
    *pDumpStream << "\n### end Ctls stream ###\n";
}


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
    const sal_Char*     pPathSep = "\\";        // only DOS-style implemented
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
                                AddError( nLine, "Unknwon command", aCommand );
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
                                AddError( nLine, "Unknwon command", aCommand );
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
                            AddError( nLine, "Unknwon command", aCommand );
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
                            AddError( nLine, "Unknwon command", aCommand );
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
        case Skipdump:      bSkip = TRUE;           break;
        case SkipOffset:    bSkipOffset = TRUE;     break;
        case ReadContRecs:  bReadContRecs = TRUE;   break;
        case NoWarnings:    bWarnings = FALSE;      break;
        case Contload:      bEndLoading = TRUE;     break;
        case BlankLine:     bBlankLine = TRUE;      break;
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
                    SfxObjectShell*     pShell = pExcRoot->pDoc->GetDocumentShell();
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

    if( pOutName )
    {
        pOutName->EraseLeadingChars( ' ' );
        pOutName->EraseTrailingChars( ' ' );
        pOutName->EraseLeadingChars( '\t' );
        pOutName->EraseTrailingChars( '\t' );

        pDumpStream = new SvFileStream( String::CreateFromAscii( pOutName->GetBuffer() ), STREAM_WRITE|STREAM_SHARE_DENYWRITE );

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


Biff8RecDumper::Biff8RecDumper( RootData& rRootData, BOOL _bBIFF8 ) :
    ExcRoot( &rRootData ),
    bBIFF8( _bBIFF8 )
{
    nXFCount = 0;
    nFontIndex = 0;
    nInstances++;
    pPivotCache = NULL;

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
    bEndLoading = bSkip = bSkipOffset = bReadContRecs = bBlankLine = FALSE;
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
        pPivotCache = pExcRoot->pPivotCacheStorage;

        if( pTitle )
            *pDumpStream << pTitle->GetBuffer();

        pIn = &r;
        r.StoreUserPosition();
        r.SetWarningMode( bWarnings );

        FilterProgressBar*  pPrgrsBar = new FilterProgressBar( r );

        while( r.StartNextRecord() )
        {
            pPrgrsBar->Progress();

            if( HasModeDump( r.GetRecNum() ) )
                RecDump();
        }

        *pDumpStream << '\n';

        pIn = NULL;
        delete pPrgrsBar;

        r.SeekUserPosition();
        r.SetWarningMode( TRUE );

        pPivotCache = NULL;

        // dump substreams
        if( pExcRoot->pRootStorage )
        {
            DumpSubStream( pExcRoot->pRootStorage, pUserNamesStreamName );
            DumpSubStream( pExcRoot->pRootStorage, pRevLogStreamName );

            SvStorageStream*    pContrIn = pExcRoot->pRootStorage->OpenStream( _STRINGCONST( "Ctls" ), STREAM_STD_READ );
            if( pContrIn )
                ControlsDump( *pContrIn );
        }
    }

    return !bEndLoading;
}

#undef Read1
#undef Read2
#undef Read4
#undef Read8

#endif

