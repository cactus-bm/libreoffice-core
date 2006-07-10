/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: compiler.cxx,v $
 *
 *  $Revision: 1.60 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-10 12:32:16 $
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

// INCLUDE ---------------------------------------------------------------

#include <sfx2/app.hxx>
#include <sfx2/objsh.hxx>
#include <basic/sbmeth.hxx>
#include <basic/sbstar.hxx>
#include <svtools/zforlist.hxx>
#include <tools/rcid.h>
#include <tools/rc.hxx>
#include <tools/solar.h>
#include <unotools/charclass.hxx>
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _UNOTOOLS_TRANSLITERATIONWRAPPER_HXX
#include <unotools/transliterationwrapper.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "compiler.hxx"
#include "rangenam.hxx"
#include "dbcolect.hxx"
#include "document.hxx"
#include "callform.hxx"
#include "addincol.hxx"
#include "refupdat.hxx"
#include "scresid.hxx"
#include "sc.hrc"
#include "globstr.hrc"
#include "cell.hxx"
#include "dociter.hxx"
#include "docoptio.hxx"
#include "errorcodes.hxx"
#include "parclass.hxx"
#include "autonamecache.hxx"


String* ScCompiler::pSymbolTableNative = NULL;
String* ScCompiler::pSymbolTableEnglish = NULL;
USHORT  ScCompiler::nAnzStrings = 0;
ULONG* ScCompiler::pCharTable = 0;
ScOpCodeHashMap* ScCompiler::pSymbolHashMapNative = NULL;
ScOpCodeHashMap* ScCompiler::pSymbolHashMapEnglish = NULL;
CharClass* ScCompiler::pCharClassEnglish = NULL;

enum ScanState
{
    ssGetChar,
    ssGetBool,
    ssGetString,
    ssSkipString,
    ssGetIdent,
    ssStop
};

struct ScArrayStack
{
    ScArrayStack* pNext;
    ScTokenArray* pArr;
    BOOL bTemp;
};

static sal_Char* pInternal[ 5 ] = { "GAME", "SPEW", "TTT", "STARCALCTEAM", "ANTWORT" };


/////////////////////////////////////////////////////////////////////////

short lcl_GetRetFormat( OpCode eOpCode )
{
    switch (eOpCode)
    {
        case ocEqual:
        case ocNotEqual:
        case ocLess:
        case ocGreater:
        case ocLessEqual:
        case ocGreaterEqual:
        case ocAnd:
        case ocOr:
        case ocNot:
        case ocTrue:
        case ocFalse:
        case ocIsEmpty:
        case ocIsString:
        case ocIsNonString:
        case ocIsLogical:
        case ocIsRef:
        case ocIsValue:
        case ocIsFormula:
        case ocIsNV:
        case ocIsErr:
        case ocIsError:
        case ocIsEven:
        case ocIsOdd:
        case ocExact:
            return NUMBERFORMAT_LOGICAL;
        case ocGetActDate:
        case ocGetDate:
        case ocEasterSunday :
            return NUMBERFORMAT_DATE;
        case ocGetActTime:
            return NUMBERFORMAT_DATETIME;
        case ocGetTime:
            return NUMBERFORMAT_TIME;
        case ocNBW:
        case ocBW:
        case ocDIA:
        case ocGDA:
        case ocGDA2:
        case ocVBD:
        case ocLIA:
        case ocRMZ:
        case ocZW:
        case ocZinsZ:
        case ocKapz:
        case ocKumZinsZ:
        case ocKumKapZ:
            return NUMBERFORMAT_CURRENCY;
        case ocZins:
        case ocIKV:
        case ocMIRR:
        case ocZGZ:
        case ocEffektiv:
        case ocNominal:
        case ocPercentSign:
            return NUMBERFORMAT_PERCENT;
//      case ocSum:
//      case ocSumSQ:
//      case ocProduct:
//      case ocAverage:
//          return -1;
        default:
            return NUMBERFORMAT_NUMBER;
    }
    return NUMBERFORMAT_NUMBER;
}

/////////////////////////////////////////////////////////////////////////

class ScOpCodeList : public Resource        // temp object for resource
{
public:
    ScOpCodeList( USHORT, String[], ScOpCodeHashMap& );
};

ScOpCodeList::ScOpCodeList( USHORT nRID, String pSymbolTable[], ScOpCodeHashMap& rHashMap )
        :
        Resource( ScResId( nRID ) )
{
    for (USHORT i = 0; i <= SC_OPCODE_LAST_OPCODE_ID; i++)
    {
        ScResId aRes(i);
        aRes.SetRT(RSC_STRING);
        if (IsAvailableRes(aRes))
        {
            pSymbolTable[i] = aRes;
            rHashMap.insert( ScOpCodeHashMap::value_type( pSymbolTable[i], (OpCode) i ) );
        }
    }
    FreeResource();
}


class ScCompilerRecursionGuard
{
private:
            short&              rRecursion;
public:
                                ScCompilerRecursionGuard( short& rRec )
                                    : rRecursion( rRec ) { ++rRecursion; }
                                ~ScCompilerRecursionGuard() { --rRecursion; }
};


void ScCompiler::Init()
{
    pSymbolTableNative = new String[SC_OPCODE_LAST_OPCODE_ID+1];
    pSymbolHashMapNative = new ScOpCodeHashMap( SC_OPCODE_LAST_OPCODE_ID+1 );
    ScOpCodeList aOpCodeListNative( RID_SC_FUNCTION_NAMES, pSymbolTableNative,
            *pSymbolHashMapNative );
    nAnzStrings = SC_OPCODE_LAST_OPCODE_ID+1;

    pCharTable = new ULONG [128];
    USHORT i;
    for (i = 0; i < 128; i++)
        pCharTable[i] = SC_COMPILER_C_ILLEGAL;
    /*   */     pCharTable[32] = SC_COMPILER_C_CHAR_DONTCARE | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* ! */     pCharTable[33] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* " */     pCharTable[34] = SC_COMPILER_C_CHAR_STRING | SC_COMPILER_C_STRING_SEP;
    /* # */     pCharTable[35] = SC_COMPILER_C_WORD_SEP;
    /* $ */     pCharTable[36] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT;
    /* % */     pCharTable[37] = SC_COMPILER_C_VALUE;
    /* & */     pCharTable[38] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* ' */     pCharTable[39] = SC_COMPILER_C_NAME_SEP;
    /* ( */     pCharTable[40] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* ) */     pCharTable[41] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* * */     pCharTable[42] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* + */     pCharTable[43] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_EXP | SC_COMPILER_C_VALUE_SIGN;
    /* , */     pCharTable[44] = SC_COMPILER_C_CHAR_VALUE | SC_COMPILER_C_VALUE;
    /* - */     pCharTable[45] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_EXP | SC_COMPILER_C_VALUE_SIGN;
    /* . */     pCharTable[46] = SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_VALUE | SC_COMPILER_C_VALUE | SC_COMPILER_C_IDENT;
    /* / */     pCharTable[47] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    for (i = 48; i < 58; i++)
    /* 0-9 */   pCharTable[i] = SC_COMPILER_C_CHAR_VALUE | SC_COMPILER_C_WORD | SC_COMPILER_C_VALUE | SC_COMPILER_C_VALUE_EXP | SC_COMPILER_C_VALUE_VALUE | SC_COMPILER_C_IDENT;
    /* : */     pCharTable[58] = SC_COMPILER_C_WORD | SC_COMPILER_C_IDENT;
    /* ; */     pCharTable[59] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* < */     pCharTable[60] = SC_COMPILER_C_CHAR_BOOL | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* = */     pCharTable[61] = SC_COMPILER_C_CHAR | SC_COMPILER_C_BOOL | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* > */     pCharTable[62] = SC_COMPILER_C_CHAR_BOOL | SC_COMPILER_C_BOOL | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* ? */     pCharTable[63] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD;
    /* @ */     // FREI
    for (i = 65; i < 91; i++)
    /* A-Z */   pCharTable[i] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT;
    /* [ */     // FREI
    /* \ */     // FREI
    /* ] */     // FREI
    /* ^ */     pCharTable[94] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    /* _ */     pCharTable[95] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT;
    /* ` */     // FREI
    for (i = 97; i < 123; i++)
    /* a-z */   pCharTable[i] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT;
    /* { */     // FREI
    /* | */     // FREI
    /* } */     // FREI
    /* ~ */     // FREI
    /* 127 */   // FREI
}

void ScCompiler::DeInit()
{
    if (pSymbolTableNative)
    {
        delete [] pSymbolTableNative;
        pSymbolTableNative = NULL;
    }
    if (pSymbolTableEnglish)
    {
        delete [] pSymbolTableEnglish;
        pSymbolTableEnglish = NULL;
    }
    if ( pSymbolHashMapNative )
    {
        pSymbolHashMapNative->clear();
        delete pSymbolHashMapNative;
        pSymbolHashMapNative = NULL;
    }
    if ( pSymbolHashMapEnglish )
    {
        pSymbolHashMapEnglish->clear();
        delete pSymbolHashMapEnglish;
        pSymbolHashMapEnglish = NULL;
    }
    if (pCharClassEnglish)
    {
        delete pCharClassEnglish;
        pCharClassEnglish = NULL;
    }
    delete [] pCharTable;
    pCharTable = NULL;
}

void ScCompiler::SetCompileEnglish( BOOL bCompileEnglish )
{
    if ( bCompileEnglish )
    {
        if ( !pSymbolTableEnglish )
        {
            pSymbolTableEnglish = new String[SC_OPCODE_LAST_OPCODE_ID+1];
            pSymbolHashMapEnglish = new ScOpCodeHashMap( SC_OPCODE_LAST_OPCODE_ID+1 );
            ScOpCodeList aOpCodeListEnglish( RID_SC_FUNCTION_NAMES_ENGLISH,
                pSymbolTableEnglish, *pSymbolHashMapEnglish );
        }
        if (!pCharClassEnglish)
        {
            ::com::sun::star::lang::Locale aLocale(
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "en")),
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "US")),
                    ::rtl::OUString());
            pCharClassEnglish = new CharClass(
                    ::comphelper::getProcessServiceFactory(), aLocale);
        }
        pSymbolTable = pSymbolTableEnglish;
        pSymbolHashMap = pSymbolHashMapEnglish;
        pCharClass = pCharClassEnglish;
    }
    else
    {
        pSymbolTable = pSymbolTableNative;
        pSymbolHashMap = pSymbolHashMapNative;
        pCharClass = ScGlobal::pCharClass;
    }
}

//-----------------------------------------------------------------------------

ScCompiler::ScCompiler( ScDocument* pDocument, const ScAddress& rPos,
                        const ScTokenArray& rArr )
        :
        aPos( rPos ),
        pSymbolTable( pSymbolTableNative ),
        pSymbolHashMap( pSymbolHashMapNative ),
        pCharClass( ScGlobal::pCharClass ),
        nRecursion(0),
        bAutoCorrect( FALSE ),
        bCorrected( FALSE ),
        bCompileForFAP( FALSE ),
        bIgnoreErrors( FALSE ),
        bCompileXML( FALSE ),
        bImportXML ( FALSE )
{
    if (!nAnzStrings)
        Init();
    pArr = (ScTokenArray*) &rArr;
    pDoc = pDocument;
    nMaxTab = pDoc->GetTableCount() - 1;
    pStack = NULL;
    nNumFmt = NUMBERFORMAT_UNDEFINED;
}

ScCompiler::ScCompiler(ScDocument* pDocument, const ScAddress& rPos )
        :
        aPos( rPos ),
        pSymbolTable( pSymbolTableNative ),
        pSymbolHashMap( pSymbolHashMapNative ),
        pCharClass( ScGlobal::pCharClass ),
        nRecursion(0),
        bAutoCorrect( FALSE ),
        bCorrected( FALSE ),
        bCompileForFAP( FALSE ),
        bIgnoreErrors( FALSE ),
        bCompileXML( FALSE ),
        bImportXML ( FALSE )
{
    if (!nAnzStrings)
        Init();
    pDoc = pDocument;
    nMaxTab = pDoc ? pDoc->GetTableCount() - 1 : 0;
    pStack = NULL;
    nNumFmt = NUMBERFORMAT_UNDEFINED;
}


String ScCompiler::MakeColStr( SCCOL nCol )
{
    if ( !ValidCol( nCol) )
        return ScGlobal::GetRscString(STR_NO_REF_TABLE);
    else
        return ::ColToAlpha( nCol);
}

void ScCompiler::MakeColStr( rtl::OUStringBuffer& rBuffer, SCCOL nCol )
{
    if ( !ValidCol( nCol) )
        rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
    else
        ::ColToAlpha( rBuffer, nCol);
}

String ScCompiler::MakeRowStr( SCROW nRow )
{
    if ( !ValidRow(nRow) )
        return ScGlobal::GetRscString(STR_NO_REF_TABLE);
    else
        return String::CreateFromInt32( nRow + 1 );
}

void ScCompiler::MakeRowStr( rtl::OUStringBuffer& rBuffer, SCROW nRow )
{
    if ( !ValidRow(nRow) )
        rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
    else
        rBuffer.append(sal_Int32(nRow + 1));
}

String ScCompiler::MakeTabStr( SCTAB nTab, String& aDoc )
{
    String aString;
    if (!pDoc->GetName(nTab, aString))
        aString = ScGlobal::GetRscString(STR_NO_REF_TABLE);
    else
    {
        if ( aString.GetChar(0) == '\'' )
        {   // "'Doc'#Tab"
            xub_StrLen nPos, nLen = 1;
            while( (nPos = aString.Search( '\'', nLen )) != STRING_NOTFOUND )
                nLen = nPos + 1;
            if ( aString.GetChar(nLen) == SC_COMPILER_FILE_TAB_SEP )
            {
                aDoc = aString.Copy( 0, nLen + 1 );
                aString.Erase( 0, nLen + 1 );
                aDoc = INetURLObject::decode( aDoc, INET_HEX_ESCAPE,
                    INetURLObject::DECODE_UNAMBIGUOUS );
            }
            else
                aDoc.Erase();
        }
        else
            aDoc.Erase();
        CheckTabQuotes( aString );
    }
    aString += '.';
    return aString;
}

void ScCompiler::CheckTabQuotes( String& rString )
{
    register const sal_Unicode* p = rString.GetBuffer();
    register const sal_Unicode* const pEnd = p + rString.Len();
    while ( p < pEnd )
    {
        if( !IsWordChar( *p ) )
        {
            rString.Insert( '\'', 0 );
            rString += '\'';
            return ;
        }
        p++;
    }
    if ( CharClass::isAsciiNumeric( rString ) )
    {
        rString.Insert( '\'', 0 );
        rString += '\'';
    }
}

String ScCompiler::MakeRefStr( ComplRefData& rRef, BOOL bSingleRef )
{
    rtl::OUStringBuffer aBuffer;
    MakeRefStr( aBuffer, rRef, bSingleRef );
    return String( aBuffer );
}

void ScCompiler::MakeRefStr( rtl::OUStringBuffer& rBuffer, ComplRefData& rRef, BOOL bSingleRef )
{
    if (bCompileXML)
        rBuffer.append(sal_Unicode('['));
    ComplRefData aRef( rRef );
    // In case absolute/relative positions weren't separately available:
    // transform relative to absolute!
//  AdjustReference( aRef.Ref1 );
//  if( !bSingleRef )
//      AdjustReference( aRef.Ref2 );
    aRef.Ref1.CalcAbsIfRel( aPos );
    if( !bSingleRef )
        aRef.Ref2.CalcAbsIfRel( aPos );
    if( aRef.Ref1.IsFlag3D() )
    {
        if (aRef.Ref1.IsTabDeleted())
        {
            if (!aRef.Ref1.IsTabRel())
                rBuffer.append(sal_Unicode('$'));
            rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
            rBuffer.append(sal_Unicode('.'));
        }
        else
        {
            String aDoc;
            String aRefStr( MakeTabStr( aRef.Ref1.nTab, aDoc ) );
            rBuffer.append(aDoc);
            if (!aRef.Ref1.IsTabRel()) rBuffer.append(sal_Unicode('$'));
            rBuffer.append(aRefStr);
        }
    }
    else if (bCompileXML)
        rBuffer.append(sal_Unicode('.'));
    if (!aRef.Ref1.IsColRel())
        rBuffer.append(sal_Unicode('$'));
    if ( aRef.Ref1.IsColDeleted() )
        rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
    else
        MakeColStr(rBuffer, aRef.Ref1.nCol );
    if (!aRef.Ref1.IsRowRel())
        rBuffer.append(sal_Unicode('$'));
    if ( aRef.Ref1.IsRowDeleted() )
        rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
    else
        MakeRowStr( rBuffer, aRef.Ref1.nRow );
    if (!bSingleRef)
    {
        rBuffer.append(sal_Unicode(':'));
        if (aRef.Ref2.IsFlag3D() || aRef.Ref2.nTab != aRef.Ref1.nTab)
        {
            if (aRef.Ref2.IsTabDeleted())
            {
                if (!aRef.Ref2.IsTabRel())
                    rBuffer.append(sal_Unicode('$'));
                rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
                rBuffer.append(sal_Unicode('.'));
            }
            else
            {
                String aDoc;
                String aRefStr( MakeTabStr( aRef.Ref2.nTab, aDoc ) );
                rBuffer.append(aDoc);
                if (!aRef.Ref2.IsTabRel()) rBuffer.append(sal_Unicode('$'));
                rBuffer.append(aRefStr);
            }
        }
        else if (bCompileXML)
            rBuffer.append(sal_Unicode('.'));
        if (!aRef.Ref2.IsColRel())
            rBuffer.append(sal_Unicode('$'));
        if ( aRef.Ref2.IsColDeleted() )
            rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
        else
            MakeColStr( rBuffer, aRef.Ref2.nCol );
        if (!aRef.Ref2.IsRowRel())
            rBuffer.append(sal_Unicode('$'));
        if ( aRef.Ref2.IsRowDeleted() )
            rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
        else
            MakeRowStr( rBuffer, aRef.Ref2.nRow );
    }
    if (bCompileXML)
        rBuffer.append(sal_Unicode(']'));
}

//---------------------------------------------------------------------------

void ScCompiler::SetError(USHORT nError)
{
    if( !pArr->GetError() )
        pArr->nError = nError;
}


sal_Unicode* lcl_UnicodeStrNCpy( sal_Unicode* pDst, const sal_Unicode* pSrc, xub_StrLen nMax )
{
    const sal_Unicode* const pStop = pDst + nMax;
    while ( *pSrc && pDst < pStop )
    {
        *pDst++ = *pSrc++;
    }
    *pDst = 0;
    return pDst;
}


//---------------------------------------------------------------------------
// NextSymbol
//---------------------------------------------------------------------------
// Zerlegt die Formel in einzelne Symbole fuer die weitere
// Verarbeitung (Turing-Maschine).
//---------------------------------------------------------------------------
// Ausgangs Zustand = GetChar
//---------------+-------------------+-----------------------+---------------
// Alter Zustand | gelesenes Zeichen | Aktion                | Neuer Zustand
//---------------+-------------------+-----------------------+---------------
// GetChar       | ;()+-*/^=&        | Symbol=Zeichen        | Stop
//               | <>                | Symbol=Zeichen        | GetBool
//               | $ Buchstabe       | Symbol=Zeichen        | GetWord
//               | Ziffer            | Symbol=Zeichen        | GetValue
//               | "                 | Keine                 | GetString
//               | Sonst             | Keine                 | GetChar
//---------------+-------------------+-----------------------+---------------
// GetBool       | =>                | Symbol=Symbol+Zeichen | Stop
//               | Sonst             | Dec(CharPos)          | Stop
//---------------+-------------------+-----------------------+---------------
// GetWord       | SepSymbol         | Dec(CharPos)          | Stop
//               | ()+-*/^=<>&~      |                       |
//               | Leerzeichen       | Dec(CharPos)          | Stop
//               | $_:.              |                       |
//               | Buchstabe,Ziffer  | Symbol=Symbol+Zeichen | GetWord
//               | Sonst             | Fehler                | Stop
//---------------|-------------------+-----------------------+---------------
// GetValue      | ;()*/^=<>&        |                       |
//               | Leerzeichen       | Dec(CharPos)          | Stop
//               | Ziffer E+-%,.     | Symbol=Symbol+Zeichen | GetValue
//               | Sonst             | Fehler                | Stop
//---------------+-------------------+-----------------------+---------------
// GetString     | "                 | Keine                 | Stop
//               | Sonst             | Symbol=Symbol+Zeichen | GetString
//---------------+-------------------+-----------------------+---------------

xub_StrLen ScCompiler::NextSymbol()
{
    cSymbol[MAXSTRLEN-1] = 0;       // Stopper
    sal_Unicode* pSym = cSymbol;
    const sal_Unicode* const pStart = aFormula.GetBuffer();
    const sal_Unicode* pSrc = pStart + nSrcPos;
    BOOL bi18n = FALSE;
    sal_Unicode c = *pSrc;
    sal_Unicode cLast = 0;
    BOOL bQuote = FALSE;
    ScanState eState = ssGetChar;
    xub_StrLen nSpaces = 0;
    // try to parse simple tokens before calling i18n parser
    while ((c != 0) && (eState != ssStop) )
    {
        pSrc++;
        ULONG nMask = GetCharTableFlags( c );
        switch (eState)
        {
            case ssGetChar :
            {
                if( nMask & SC_COMPILER_C_CHAR )
                {
                    *pSym++ = c;
                    eState = ssStop;
                }
                else if( nMask & SC_COMPILER_C_CHAR_BOOL )
                {
                    *pSym++ = c;
                    eState = ssGetBool;
                }
                else if( nMask & SC_COMPILER_C_CHAR_STRING )
                {
                    *pSym++ = c;
                    eState = ssGetString;
                }
                else if( nMask & SC_COMPILER_C_CHAR_DONTCARE )
                {
                    nSpaces++;
                }
                else if( nMask & SC_COMPILER_C_CHAR_IDENT )
                {   // try to get a simple ASCII identifier before calling
                    // i18n, to gain performance during import
                    *pSym++ = c;
                    eState = ssGetIdent;
                }
                else
                {
                    bi18n = TRUE;
                    eState = ssStop;
                }
            }
            break;
            case ssGetIdent:
            {
                if ( nMask & SC_COMPILER_C_IDENT )
                {   // this catches also $Sheet1.A1:A$2, for example
                    if( pSym == &cSymbol[ MAXSTRLEN-1 ] )
                    {
                        SetError(errStringOverflow);
                        eState = ssStop;
                    }
                    else
                        *pSym++ = c;
                }
                else if ( 128 <= c || '\'' == c )
                {   // High values need reparsing with i18n,
                    // single quoted $'sheet' names too (otherwise we'd had to
                    // implement everything twice).
                    pSrc = pStart + nSrcPos + nSpaces;
                    pSym = cSymbol;
                    c = *pSrc;
                    bi18n = TRUE;
                    eState = ssStop;
                }
                else
                {
                    pSrc--;
                    eState = ssStop;
                }
            }
            break;
            case ssGetBool :
            {
                if( nMask & SC_COMPILER_C_BOOL )
                {
                    *pSym++ = c;
                    eState = ssStop;
                }
                else
                {
                    pSrc--;
                    eState = ssStop;
                }
            }
            break;
            case ssGetString :
            {
                if( nMask & SC_COMPILER_C_STRING_SEP )
                {
                    if ( !bQuote )
                    {
                        if ( *pSrc == '"' )
                            bQuote = TRUE;      // "" => literal "
                        else
                            eState = ssStop;
                    }
                    else
                        bQuote = FALSE;
                }
                if ( !bQuote )
                {
                    if( pSym == &cSymbol[ MAXSTRLEN-1 ] )
                    {
                        SetError(errStringOverflow);
                        eState = ssSkipString;
                    }
                    else
                        *pSym++ = c;
                }
            }
            break;
            case ssSkipString:
                if( nMask & SC_COMPILER_C_STRING_SEP )
                    eState = ssStop;
                break;
            case ssStop:
                ;   // nothing, prevent warning
                break;
        }
        cLast = c;
        c = *pSrc;
    }
    if ( bi18n )
    {
        using namespace ::com::sun::star::i18n;
        nSrcPos += nSpaces;
        sal_Int32 nStartFlags = KParseTokens::ANY_LETTER_OR_NUMBER |
            KParseTokens::ASC_UNDERSCORE | KParseTokens::ASC_DOLLAR;
        sal_Int32 nContFlags = nStartFlags | KParseTokens::ASC_DOT |
            KParseTokens::ASC_COLON;
        // '?' allowed in range names because of Xcl :-/
        static const String aAddAllowed( '?' );
        String aSymbol;
        USHORT nErr = 0;
        do
        {
            bi18n = FALSE;
            // special case  $'sheetname'
            if ( pStart[nSrcPos] == '$' && pStart[nSrcPos+1] == '\'' )
                aSymbol += pStart[nSrcPos++];

            ParseResult aRes = pCharClass->parseAnyToken( aFormula,
                nSrcPos, nStartFlags, aAddAllowed, nContFlags, aAddAllowed );

            if ( !aRes.TokenType )
                SetError( nErr = errIllegalChar );      // parsed chars as string
            if ( aRes.EndPos <= nSrcPos )
            {   // ?!?
                SetError( nErr = errIllegalChar );
                nSrcPos = aFormula.Len();
                aSymbol.Erase();
            }
            else
            {
                aSymbol.Append( pStart + nSrcPos, (xub_StrLen)aRes.EndPos - nSrcPos );
                nSrcPos = (xub_StrLen) aRes.EndPos;
                if ( aRes.TokenType & KParseType::SINGLE_QUOTE_NAME )
                {   // special cases  'sheetname'.  'filename'#
                    c = pStart[nSrcPos];
                    bi18n = (c == '.' || c == SC_COMPILER_FILE_TAB_SEP);
                    if ( bi18n )
                        aSymbol += pStart[nSrcPos++];
                }
                else if ( aRes.TokenType & KParseType::IDENTNAME )
                {   // special cases  reference:[$]'sheetname'
                    c = aSymbol.GetChar( aSymbol.Len()-1 );
                    bi18n = ((c == ':' || c == '$') && pStart[nSrcPos] == '\'');
                }
            }
        } while ( bi18n && !nErr );
        xub_StrLen nLen = aSymbol.Len();
        if ( nLen >= MAXSTRLEN )
        {
            SetError( errStringOverflow );
            nLen = MAXSTRLEN-1;
        }
        lcl_UnicodeStrNCpy( cSymbol, aSymbol.GetBuffer(), nLen );
    }
    else
    {
        nSrcPos = pSrc - pStart;
        *pSym = 0;
    }
    if ( bAutoCorrect )
        aCorrectedSymbol = cSymbol;
    return nSpaces;
}

//---------------------------------------------------------------------------
// Convert symbol to token
//---------------------------------------------------------------------------

BOOL ScCompiler::IsOpCode( const String& rName )
{
    ScOpCodeHashMap::const_iterator iLook( pSymbolHashMap->find( rName ) );
    BOOL bFound = (iLook != pSymbolHashMap->end());
    if (bFound)
    {
        ScRawToken aToken;
        aToken.SetOpCode( iLook->second );
        pRawToken = aToken.Clone();
    }
    else
    {
        USHORT nIndex;
        bFound = ScGlobal::GetFuncCollection()->SearchFunc(cSymbol, nIndex);
        if( bFound )
        {
            ScRawToken aToken;
            aToken.SetExternal( cSymbol );
            pRawToken = aToken.Clone();
        }
        else
        {
            // bLocalFirst=FALSE for english
            String aIntName = ScGlobal::GetAddInCollection()->
                    FindFunction( rName, ( pSymbolTable != pSymbolTableEnglish ) );
            if (aIntName.Len())
            {
                ScRawToken aToken;
                aToken.SetExternal( aIntName.GetBuffer() );     // international name
                pRawToken = aToken.Clone();
                bFound = TRUE;
            }
        }
    }
    if ( bFound && pRawToken->GetOpCode() == ocSub &&
            (eLastOp == ocOpen || eLastOp == ocSep || eLastOp == ocNegSub ||
             (eLastOp > ocEndDiv && eLastOp < ocEndBinOp)))
        pRawToken->NewOpCode( ocNegSub );
        //! if ocNegSub had ForceArray we'd have to set it here
    return bFound;
}

BOOL ScCompiler::IsOpCode2( const String& rName )
{
    BOOL bFound = FALSE;
    USHORT i;

    for( i = ocInternalBegin; i <= ocInternalEnd && !bFound; i++ )
        bFound = rName.EqualsAscii( pInternal[ i-ocInternalBegin ] );

    if (bFound)
    {
        ScRawToken aToken;
        aToken.SetOpCode( (OpCode) --i );
        pRawToken = aToken.Clone();
    }
    return bFound;
}

BOOL ScCompiler::IsValue( const String& rSym )
{
    double fVal;
    sal_uInt32 nIndex = ( pSymbolTable == pSymbolTableEnglish ?
        pDoc->GetFormatTable()->GetStandardIndex( LANGUAGE_ENGLISH_US ) : 0 );
//  ULONG nIndex = 0;
////    ULONG nIndex = pDoc->GetFormatTable()->GetStandardIndex(ScGlobal::eLnge);
    if (pDoc->GetFormatTable()->IsNumberFormat( rSym, nIndex, fVal ) )
    {
        USHORT nType = pDoc->GetFormatTable()->GetType(nIndex);
        const sal_Unicode* p = aFormula.GetBuffer() + nSrcPos;
        while( *p == ' ' )
            p++;
        if ( *p == '(' && nType == NUMBERFORMAT_LOGICAL)
            return FALSE;
        else if( aFormula.GetChar(nSrcPos) == '.' )
            // numerical sheet name?
            return FALSE;
        else
        {
            if( nType == NUMBERFORMAT_TEXT )
                // HACK: number too big!
                SetError( errIllegalArgument );
            ScRawToken aToken;
            aToken.SetDouble( fVal );
            pRawToken = aToken.Clone();
            return TRUE;
        }
    }
    else
        return FALSE;
}

BOOL ScCompiler::IsString()
{
    register const sal_Unicode* p = cSymbol;
    while ( *p )
        p++;
    xub_StrLen nLen = p - cSymbol - 1;
    BOOL bQuote = ((cSymbol[0] == '"') && (cSymbol[nLen] == '"'));
    if ((bQuote ? nLen-2 : nLen) > MAXSTRLEN-1)
    {
        SetError(errStringOverflow);
        return FALSE;
    }
    if ( bQuote )
    {
        cSymbol[nLen] = '\0';
        ScRawToken aToken;
        aToken.SetString( cSymbol+1 );
        pRawToken = aToken.Clone();
        return TRUE;
    }
    return FALSE;
}

BOOL ScCompiler::IsReference( const String& rName )
{
    // Has to be called before IsValue
    sal_Unicode ch1 = rName.GetChar(0);
    sal_Unicode cDecSep = ( pSymbolTable == pSymbolTableEnglish ? '.' :
        ScGlobal::pLocaleData->getNumDecimalSep().GetChar(0) );
    if ( ch1 == cDecSep )
        return FALSE;
    // Who was that imbecile introducing '.' as the sheet name separator!?!
    if ( CharClass::isAsciiNumeric( ch1 ) )
    {   // Numerical sheet name is valid.
        // But English 1.E2 or 1.E+2 is value 100, 1.E-2 is 0.01
        // Don't create a #REF! of values.
        const xub_StrLen nPos = rName.Search( '.' );
        if ( nPos == STRING_NOTFOUND )
            return FALSE;
        sal_Unicode const * const pTabSep = rName.GetBuffer() + nPos;
        sal_Unicode ch2 = pTabSep[1];   // maybe a column identifier
        if ( !(ch2 == '$' || CharClass::isAsciiAlpha( ch2 )) )
            return FALSE;
        if ( cDecSep == '.' && (ch2 == 'E' || ch2 == 'e')   // E + - digit
                && (GetCharTableFlags( pTabSep[2] ) & SC_COMPILER_C_VALUE_EXP) )
        {   // #91053#
            // If it is an 1.E2 expression check if "1" is an existent sheet
            // name. If so, a desired value 1.E2 would have to be entered as
            // 1E2 or 1.0E2 or 1.E+2, sorry. Another possibility would be to
            // require numerical sheet names always being entered quoted, which
            // is not desirable (too many 1999, 2000, 2001 sheets in use).
            // Furthermore, XML files created with versions prior to SRC640e
            // wouldn't contain the quotes added by MakeTabStr()/CheckTabQuotes()
            // and would produce wrong formulas if the conditions here are met.
            // If you can live with these restrictions you may remove the
            // check and return an unconditional FALSE.
            String aTabName( rName.Copy( 0, nPos ) );
            SCTAB nTab;
            if ( !pDoc->GetTable( aTabName, nTab ) )
                return FALSE;
            // If sheet "1" exists and the expression is 1.E+2 continue as
            // usual, the ScRange/ScAddress parser will take care of it.
        }
    }
    ScRange aRange( aPos, aPos );
    USHORT nFlags = aRange.Parse( rName, pDoc );
    if( nFlags & SCA_VALID )
    {
        ScRawToken aToken;
        ComplRefData aRef;
        aRef.InitRange( aRange );
        aRef.Ref1.SetColRel( (nFlags & SCA_COL_ABSOLUTE) == 0 );
        aRef.Ref1.SetRowRel( (nFlags & SCA_ROW_ABSOLUTE) == 0 );
        aRef.Ref1.SetTabRel( (nFlags & SCA_TAB_ABSOLUTE) == 0 );
        if ( !(nFlags & SCA_VALID_TAB) )
            aRef.Ref1.SetTabDeleted( TRUE );        // #REF!
        aRef.Ref1.SetFlag3D( ( nFlags & SCA_TAB_3D ) != 0 );
        aRef.Ref2.SetColRel( (nFlags & SCA_COL2_ABSOLUTE) == 0 );
        aRef.Ref2.SetRowRel( (nFlags & SCA_ROW2_ABSOLUTE) == 0 );
        aRef.Ref2.SetTabRel( (nFlags & SCA_TAB2_ABSOLUTE) == 0 );
        if ( !(nFlags & SCA_VALID_TAB2) )
            aRef.Ref2.SetTabDeleted( TRUE );        // #REF!
        aRef.Ref2.SetFlag3D( ( nFlags & SCA_TAB2_3D ) != 0 );
        aRef.CalcRelFromAbs( aPos );
        aToken.SetDoubleReference( aRef );
        pRawToken = aToken.Clone();
    }
    else
    {
        ScAddress aAddr( aPos );
        nFlags = aAddr.Parse( rName, pDoc );
        // Something must be valid in order to recognize Sheet1.blah or blah.a1
        // as a (wrong) reference.
        if( nFlags & ( SCA_VALID_COL|SCA_VALID_ROW|SCA_VALID_TAB ) )
        {
            ScRawToken aToken;
            SingleRefData aRef;
            aRef.InitAddress( aAddr );
            aRef.SetColRel( (nFlags & SCA_COL_ABSOLUTE) == 0 );
            aRef.SetRowRel( (nFlags & SCA_ROW_ABSOLUTE) == 0 );
            aRef.SetTabRel( (nFlags & SCA_TAB_ABSOLUTE) == 0 );
            aRef.SetFlag3D( ( nFlags & SCA_TAB_3D ) != 0 );
            // the reference is really invalid
            if( !( nFlags & SCA_VALID ) )
            {
                if( !( nFlags & SCA_VALID_COL ) )
                    aRef.nCol = MAXCOL+1;
                if( !( nFlags & SCA_VALID_ROW ) )
                    aRef.nRow = MAXROW+1;
                if( !( nFlags & SCA_VALID_TAB ) )
                    aRef.nTab = MAXTAB+3;
                nFlags |= SCA_VALID;
            }
            aRef.CalcRelFromAbs( aPos );
            aToken.SetSingleReference( aRef );
            pRawToken = aToken.Clone();
        }
    }
    return ( nFlags & SCA_VALID ) != 0;
}

BOOL ScCompiler::IsMacro( const String& rName )
{
    StarBASIC* pObj = 0;
    SfxObjectShell* pDocSh = pDoc->GetDocumentShell();

    SfxApplication* pSfxApp = SFX_APP();
    pSfxApp->EnterBasicCall();              // initialize document's BASIC

    if( pDocSh )//XXX
        pObj = pDocSh->GetBasic();
    else
        pObj = pSfxApp->GetBasic();

    SbxMethod* pMeth = (SbxMethod*) pObj->Find( rName, SbxCLASS_METHOD );
    if( !pMeth )
    {
        pSfxApp->LeaveBasicCall();
        return FALSE;
    }
    // It really should be a BASIC function!
    if( pMeth->GetType() == SbxVOID
     || ( pMeth->IsFixed() && pMeth->GetType() == SbxEMPTY )
     || !pMeth->ISA(SbMethod) )
    {
        pSfxApp->LeaveBasicCall();
        return FALSE;
    }
    ScRawToken aToken;
    aToken.SetExternal( rName.GetBuffer() );
    aToken.eOp = ocMacro;
    pRawToken = aToken.Clone();
    pSfxApp->LeaveBasicCall();
    return TRUE;
}

BOOL ScCompiler::IsNamedRange( const String& rUpperName )
{
    // IsNamedRange is called only from NextNewToken, with an upper-case string

    USHORT n;
    ScRangeName* pRangeName = pDoc->GetRangeName();
    if (pRangeName->SearchNameUpper( rUpperName, n ) )
    {
        ScRangeData* pData = (*pRangeName)[n];
        ScRawToken aToken;
        aToken.SetName( pData->GetIndex() );
        pRawToken = aToken.Clone();
        return TRUE;
    }
    else
        return FALSE;
}

BOOL ScCompiler::IsDBRange( const String& rName )
{
    USHORT n;
    ScDBCollection* pDBColl = pDoc->GetDBCollection();
    if (pDBColl->SearchName( rName, n ) )
    {
        ScDBData* pData = (*pDBColl)[n];
        ScRawToken aToken;
        aToken.SetName( pData->GetIndex() );
        aToken.eOp = ocDBArea;
        pRawToken = aToken.Clone();
        return TRUE;
    }
    else
        return FALSE;
}

BOOL ScCompiler::IsColRowName( const String& rName )
{
    BOOL bInList = FALSE;
    BOOL bFound = FALSE;
    SingleRefData aRef;
    String aName( rName );
    DeQuote( aName );
    SCTAB nThisTab = aPos.Tab();
    for ( short jThisTab = 1; jThisTab >= 0 && !bInList; jThisTab-- )
    {   // #50300# first check ranges on this sheet, in case of duplicated names
        for ( short jRow=0; jRow<2 && !bInList; jRow++ )
        {
            ScRangePairList* pRL;
            if ( !jRow )
                pRL = pDoc->GetColNameRanges();
            else
                pRL = pDoc->GetRowNameRanges();
            for ( ScRangePair* pR = pRL->First(); pR && !bInList; pR = pRL->Next() )
            {
                const ScRange& rNameRange = pR->GetRange(0);
                if ( jThisTab && !(rNameRange.aStart.Tab() <= nThisTab &&
                        nThisTab <= rNameRange.aEnd.Tab()) )
                    continue;   // for
                ScCellIterator aIter( pDoc, rNameRange );
                for ( ScBaseCell* pCell = aIter.GetFirst(); pCell && !bInList;
                        pCell = aIter.GetNext() )
                {
                    // Don't crash if cell (via CompileNameFormula) encounters
                    // a formula cell without code and
                    // HasStringData/Interpret/Compile is executed and all that
                    // recursive..
                    // Furthermore, *this* cell won't be touched, since no RPN exists yet.
                    CellType eType = pCell->GetCellType();
                    BOOL bOk = (eType == CELLTYPE_FORMULA ?
                        ((ScFormulaCell*)pCell)->GetCode()->GetCodeLen() > 0
                        && ((ScFormulaCell*)pCell)->aPos != aPos    // noIter
                        : TRUE );
                    if ( bOk && pCell->HasStringData() )
                    {
                        String aStr;
                        switch ( eType )
                        {
                            case CELLTYPE_STRING:
                                ((ScStringCell*)pCell)->GetString( aStr );
                            break;
                            case CELLTYPE_FORMULA:
                                ((ScFormulaCell*)pCell)->GetString( aStr );
                            break;
                            case CELLTYPE_EDIT:
                                ((ScEditCell*)pCell)->GetString( aStr );
                            break;
                            case CELLTYPE_NONE:
                            case CELLTYPE_VALUE:
                            case CELLTYPE_NOTE:
                            case CELLTYPE_SYMBOLS:
                            case CELLTYPE_DESTROYED:
                                ;   // nothing, prevent compiler warning
                            break;
                        }
                        if ( ScGlobal::pTransliteration->isEqual( aStr, aName ) )
                        {
                            aRef.InitFlags();
                            aRef.nCol = aIter.GetCol();
                            aRef.nRow = aIter.GetRow();
                            aRef.nTab = aIter.GetTab();
                            if ( !jRow )
                                aRef.SetColRel( TRUE );     // ColName
                            else
                                aRef.SetRowRel( TRUE );     // RowName
                            aRef.CalcRelFromAbs( aPos );
                            bInList = bFound = TRUE;
                        }
                    }
                }
            }
        }
    }
    if ( !bInList && pDoc->GetDocOptions().IsLookUpColRowNames() )
    {   // search in current sheet
        long nDistance, nMax;
        long nMyCol = (long) aPos.Col();
        long nMyRow = (long) aPos.Row();
        BOOL bTwo = FALSE;
        ScAddress aOne( 0, 0, aPos.Tab() );
        ScAddress aTwo( MAXCOL, MAXROW, aPos.Tab() );

        ScAutoNameCache* pNameCache = pDoc->GetAutoNameCache();
        if ( pNameCache )
        {
            //  #b6355215# use GetNameOccurences to collect all positions of aName on the sheet
            //  (only once), similar to the outer part of the loop in the "else" branch.

            const ScAutoNameAddresses& rAddresses = pNameCache->GetNameOccurences( aName, aPos.Tab() );

            //  Loop through the found positions, similar to the inner part of the loop in the "else" branch.
            //  The order of addresses in the vector is the same as from ScCellIterator.

            ScAutoNameAddresses::const_iterator aEnd(rAddresses.end());
            for ( ScAutoNameAddresses::const_iterator aAdrIter(rAddresses.begin()); aAdrIter != aEnd; ++aAdrIter )
            {
                ScAddress aAddress( *aAdrIter );        // cell address with an equal string

                if ( bFound )
                {   // stop if everything else is further away
                    if ( nMax < (long)aAddress.Col() )
                        break;      // aIter
                }
                if ( aAddress != aPos )
                {
                    // same treatment as in isEqual case below

                    SCCOL nCol = aAddress.Col();
                    SCROW nRow = aAddress.Row();
                    long nC = nMyCol - nCol;
                    long nR = nMyRow - nRow;
                    if ( bFound )
                    {
                        long nD = nC * nC + nR * nR;
                        if ( nD < nDistance )
                        {
                            if ( nC < 0 || nR < 0 )
                            {   // right or below
                                bTwo = TRUE;
                                aTwo.Set( nCol, nRow, aAddress.Tab() );
                                nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                                nDistance = nD;
                            }
                            else if ( !(nRow < aOne.Row() && nMyRow >= (long)aOne.Row()) )
                            {
                                // upper left, only if not further up than the
                                // current entry and nMyRow is below (CellIter
                                // runs column-wise)
                                bTwo = FALSE;
                                aOne.Set( nCol, nRow, aAddress.Tab() );
                                nMax = Max( nMyCol + nC, nMyRow + nR );
                                nDistance = nD;
                            }
                        }
                    }
                    else
                    {
                        aOne.Set( nCol, nRow, aAddress.Tab() );
                        nDistance = nC * nC + nR * nR;
                        nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                    }
                    bFound = TRUE;
                }
            }
        }
        else
        {
            ScCellIterator aIter( pDoc, ScRange( aOne, aTwo ) );
            for ( ScBaseCell* pCell = aIter.GetFirst(); pCell; pCell = aIter.GetNext() )
            {
                if ( bFound )
                {   // stop if everything else is further away
                    if ( nMax < (long)aIter.GetCol() )
                        break;      // aIter
                }
                CellType eType = pCell->GetCellType();
                BOOL bOk = (eType == CELLTYPE_FORMULA ?
                    ((ScFormulaCell*)pCell)->GetCode()->GetCodeLen() > 0
                    && ((ScFormulaCell*)pCell)->aPos != aPos    // noIter
                    : TRUE );
                if ( bOk && pCell->HasStringData() )
                {
                    String aStr;
                    switch ( eType )
                    {
                        case CELLTYPE_STRING:
                            ((ScStringCell*)pCell)->GetString( aStr );
                        break;
                        case CELLTYPE_FORMULA:
                            ((ScFormulaCell*)pCell)->GetString( aStr );
                        break;
                        case CELLTYPE_EDIT:
                            ((ScEditCell*)pCell)->GetString( aStr );
                        break;
                        case CELLTYPE_NONE:
                        case CELLTYPE_VALUE:
                        case CELLTYPE_NOTE:
                        case CELLTYPE_SYMBOLS:
                        case CELLTYPE_DESTROYED:
                            ;   // nothing, prevent compiler warning
                        break;
                    }
                    if ( ScGlobal::pTransliteration->isEqual( aStr, aName ) )
                    {
                        SCCOL nCol = aIter.GetCol();
                        SCROW nRow = aIter.GetRow();
                        long nC = nMyCol - nCol;
                        long nR = nMyRow - nRow;
                        if ( bFound )
                        {
                            long nD = nC * nC + nR * nR;
                            if ( nD < nDistance )
                            {
                                if ( nC < 0 || nR < 0 )
                                {   // right or below
                                    bTwo = TRUE;
                                    aTwo.Set( nCol, nRow, aIter.GetTab() );
                                    nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                                    nDistance = nD;
                                }
                                else if ( !(nRow < aOne.Row() && nMyRow >= (long)aOne.Row()) )
                                {
                                    // upper left, only if not further up than the
                                    // current entry and nMyRow is below (CellIter
                                    // runs column-wise)
                                    bTwo = FALSE;
                                    aOne.Set( nCol, nRow, aIter.GetTab() );
                                    nMax = Max( nMyCol + nC, nMyRow + nR );
                                    nDistance = nD;
                                }
                            }
                        }
                        else
                        {
                            aOne.Set( nCol, nRow, aIter.GetTab() );
                            nDistance = nC * nC + nR * nR;
                            nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                        }
                        bFound = TRUE;
                    }
                }
            }
        }

        if ( bFound )
        {
            ScAddress aAdr;
            if ( bTwo )
            {
                if ( nMyCol >= (long)aOne.Col() && nMyRow >= (long)aOne.Row() )
                    aAdr = aOne;        // upper left takes precedence
                else
                {
                    if ( nMyCol < (long)aOne.Col() )
                    {   // two to the right
                        if ( nMyRow >= (long)aTwo.Row() )
                            aAdr = aTwo;        // directly right
                        else
                            aAdr = aOne;
                    }
                    else
                    {   // two below or below and right, take the nearest
                        long nC1 = nMyCol - aOne.Col();
                        long nR1 = nMyRow - aOne.Row();
                        long nC2 = nMyCol - aTwo.Col();
                        long nR2 = nMyRow - aTwo.Row();
                        if ( nC1 * nC1 + nR1 * nR1 <= nC2 * nC2 + nR2 * nR2 )
                            aAdr = aOne;
                        else
                            aAdr = aTwo;
                    }
                }
            }
            else
                aAdr = aOne;
            aRef.InitAddress( aAdr );
            if ( (aRef.nRow != MAXROW && pDoc->HasStringData(
                    aRef.nCol, aRef.nRow + 1, aRef.nTab ))
              || (aRef.nRow != 0 && pDoc->HasStringData(
                    aRef.nCol, aRef.nRow - 1, aRef.nTab )) )
                aRef.SetRowRel( TRUE );     // RowName
            else
                aRef.SetColRel( TRUE );     // ColName
            aRef.CalcRelFromAbs( aPos );
        }
    }
    if ( bFound )
    {
        ScRawToken aToken;
        aToken.SetSingleReference( aRef );
        aToken.eOp = ocColRowName;
        pRawToken = aToken.Clone();
        return TRUE;
    }
    else
        return FALSE;
}

//---------------------------------------------------------------------------

void ScCompiler::AutoCorrectParsedSymbol()
{
    xub_StrLen nPos = aCorrectedSymbol.Len();
    if ( nPos )
    {
        nPos--;
        const sal_Unicode cQuote = '\"';
        const sal_Unicode cx = 'x';
        const sal_Unicode cX = 'X';
        sal_Unicode c1 = aCorrectedSymbol.GetChar( 0 );
        sal_Unicode c2 = aCorrectedSymbol.GetChar( nPos );
        if ( c1 == cQuote && c2 != cQuote  )
        {   // "...
            // What's not a word doesn't belong to it.
            // Don't be pedantic: c < 128 should be sufficient here.
            while ( nPos && ((aCorrectedSymbol.GetChar(nPos) < 128) &&
                    ((GetCharTableFlags( aCorrectedSymbol.GetChar(nPos) ) &
                    (SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_DONTCARE)) == 0)) )
                nPos--;
            if ( nPos == MAXSTRLEN - 2 )
                aCorrectedSymbol.SetChar( nPos, cQuote );   // '"' the 255th character
            else
                aCorrectedSymbol.Insert( cQuote, nPos + 1 );
            bCorrected = TRUE;
        }
        else if ( c1 != cQuote && c2 == cQuote )
        {   // ..."
            aCorrectedSymbol.Insert( cQuote, 0 );
            bCorrected = TRUE;
        }
        else if ( nPos == 0 && (c1 == cx || c1 == cX) )
        {   // x => *
            aCorrectedSymbol = pSymbolTable[ocMul];
            bCorrected = TRUE;
        }
        else if ( (GetCharTableFlags( c1 ) & SC_COMPILER_C_CHAR_VALUE)
               && (GetCharTableFlags( c2 ) & SC_COMPILER_C_CHAR_VALUE) )
        {
            xub_StrLen nXcount;
            if ( (nXcount = aCorrectedSymbol.GetTokenCount( cx )) > 1 )
            {   // x => *
                xub_StrLen nIndex = 0;
                sal_Unicode c = pSymbolTable[ocMul].GetChar(0);
                while ( (nIndex = aCorrectedSymbol.SearchAndReplace(
                        cx, c, nIndex )) != STRING_NOTFOUND )
                    nIndex++;
                bCorrected = TRUE;
            }
            if ( (nXcount = aCorrectedSymbol.GetTokenCount( cX )) > 1 )
            {   // X => *
                xub_StrLen nIndex = 0;
                sal_Unicode c = pSymbolTable[ocMul].GetChar(0);
                while ( (nIndex = aCorrectedSymbol.SearchAndReplace(
                        cX, c, nIndex )) != STRING_NOTFOUND )
                    nIndex++;
                bCorrected = TRUE;
            }
        }
        else
        {
            String aSymbol( aCorrectedSymbol );
            String aDoc;
            xub_StrLen nPos;
            if ( aSymbol.GetChar(0) == '\''
              && ((nPos = aSymbol.SearchAscii( "'#" )) != STRING_NOTFOUND) )
            {   // Split off 'Doc'#, may be d:\... or whatever
                aDoc = aSymbol.Copy( 0, nPos + 2 );
                aSymbol.Erase( 0, nPos + 2 );
            }
            xub_StrLen nRefs = aSymbol.GetTokenCount( ':' );
            BOOL bColons;
            if ( nRefs > 2 )
            {   // duplicated or too many ':'? B:2::C10 => B2:C10
                bColons = TRUE;
                xub_StrLen nIndex = 0;
                String aTmp1( aSymbol.GetToken( 0, ':', nIndex ) );
                xub_StrLen nLen1 = aTmp1.Len();
                String aSym, aTmp2;
                BOOL bLastAlp, bNextNum;
                bLastAlp = bNextNum = TRUE;
                xub_StrLen nStrip = 0;
                xub_StrLen nCount = nRefs;
                for ( xub_StrLen j=1; j<nCount; j++ )
                {
                    aTmp2 = aSymbol.GetToken( 0, ':', nIndex );
                    xub_StrLen nLen2 = aTmp2.Len();
                    if ( nLen1 || nLen2 )
                    {
                        if ( nLen1 )
                        {
                            aSym += aTmp1;
                            bLastAlp = CharClass::isAsciiAlpha( aTmp1 );
                        }
                        if ( nLen2 )
                        {
                            bNextNum = CharClass::isAsciiNumeric( aTmp2 );
                            if ( bLastAlp == bNextNum && nStrip < 1 )
                            {
                                // Must be alternating number/string, only
                                // strip within a reference.
                                nRefs--;
                                nStrip++;
                            }
                            else
                            {
                                xub_StrLen nSymLen = aSym.Len();
                                if ( nSymLen
                                  && (aSym.GetChar( nSymLen - 1 ) != ':') )
                                    aSym += ':';
                                nStrip = 0;
                            }
                            bLastAlp = !bNextNum;
                        }
                        else
                        {   // ::
                            nRefs--;
                            if ( nLen1 )
                            {   // B10::C10 ? append ':' on next round
                                if ( !bLastAlp && !CharClass::isAsciiNumeric( aTmp1 ) )
                                    nStrip++;
                            }
                            bNextNum = !bLastAlp;
                        }
                        aTmp1 = aTmp2;
                        nLen1 = nLen2;
                    }
                    else
                        nRefs--;
                }
                aSymbol = aSym;
                aSymbol += aTmp1;
            }
            else
                bColons = FALSE;
            if ( nRefs && nRefs <= 2 )
            {   // reference twisted? 4A => A4 etc.
                String aTab[2], aRef[2];
                if ( nRefs == 2 )
                {
                    aRef[0] = aSymbol.GetToken( 0, ':' );
                    aRef[1] = aSymbol.GetToken( 1, ':' );
                }
                else
                    aRef[0] = aSymbol;

                BOOL bChanged = FALSE;
                BOOL bOk = TRUE;
                USHORT nMask = SCA_VALID | SCA_VALID_COL | SCA_VALID_ROW;
                for ( int j=0; j<nRefs; j++ )
                {
                    xub_StrLen nTmp = 0;
                    xub_StrLen nPos = STRING_NOTFOUND;
                    while ( (nTmp = aRef[j].Search( '.', nTmp )) != STRING_NOTFOUND )
                        nPos = nTmp++;      // the last one counts
                    if ( nPos != STRING_NOTFOUND )
                    {
                        aTab[j] = aRef[j].Copy( 0, nPos + 1 );  // with '.'
                        aRef[j].Erase( 0, nPos + 1 );
                    }
                    String aOld( aRef[j] );
                    String aStr2;
                    const sal_Unicode* p = aRef[j].GetBuffer();
                    while ( *p && CharClass::isAsciiNumeric( *p ) )
                        aStr2 += *p++;
                    aRef[j] = String( p );
                    aRef[j] += aStr2;
                    if ( bColons || aRef[j] != aOld )
                    {
                        bChanged = TRUE;
                        ScAddress aAdr;
                        bOk &= ((aAdr.Parse( aRef[j], pDoc ) & nMask) == nMask);
                    }
                }
                if ( bChanged && bOk )
                {
                    aCorrectedSymbol = aDoc;
                    aCorrectedSymbol += aTab[0];
                    aCorrectedSymbol += aRef[0];
                    if ( nRefs == 2 )
                    {
                        aCorrectedSymbol += ':';
                        aCorrectedSymbol += aTab[1];
                        aCorrectedSymbol += aRef[1];
                    }
                    bCorrected = TRUE;
                }
            }
        }
    }
}

BOOL ScCompiler::NextNewToken()
{
    xub_StrLen nSpaces = NextSymbol();
    ScRawToken aToken;
    if( cSymbol[0] )
    {
        if( nSpaces )
        {
            aToken.SetOpCode( ocSpaces );
            aToken.sbyte.cByte = (BYTE) ( nSpaces > 255 ? 255 : nSpaces );
            if( !pArr->AddToken( aToken ) )
            {
                SetError(errCodeOverflow); return FALSE;
            }
        }
        if ( (cSymbol[0] == '#' || cSymbol[0] == '$') && cSymbol[1] == 0 &&
                !bAutoCorrect )
        {   // #101100# special case to speed up broken [$]#REF documents
            String aBad( aFormula.Copy( nSrcPos-1 ) );
            eLastOp = pArr->AddBad( aBad )->GetOpCode();
            return FALSE;
        }
        if( !IsString() )
        {
            BOOL bMayBeFuncName;
            if ( cSymbol[0] < 128 )
                bMayBeFuncName = CharClass::isAsciiAlpha( cSymbol[0] );
            else
            {
                String aTmpStr( cSymbol[0] );
                bMayBeFuncName = ScGlobal::pCharClass->isLetter( aTmpStr, 0 );
            }
            if ( bMayBeFuncName )
            {   // a function name must be followed by a parenthesis
                const sal_Unicode* p = aFormula.GetBuffer() + nSrcPos;
                while( *p == ' ' )
                    p++;
                bMayBeFuncName = ( *p == '(' );
            }
            else
                bMayBeFuncName = TRUE;      // operators and other opcodes

            String aOrg( cSymbol ); // preserve file names in IsReference()
            String aUpper( ScGlobal::pCharClass->upper( aOrg ) );
            // Column 'DM' ("Deutsche Mark", German currency) couldn't be
            // referred to => IsReference() before IsValue().
            // #42016# Italian ARCTAN.2 resulted in #REF! => IsOpcode() before
            // IsReference().
            if ( !(bMayBeFuncName && IsOpCode( aUpper ))
              && !IsReference( aOrg )
              && !IsValue( aUpper )
              && !IsNamedRange( aUpper )
              && !IsDBRange( aUpper )
              && !IsColRowName( aUpper )
              && !(bMayBeFuncName && IsMacro( aUpper ))
              && !(bMayBeFuncName && IsOpCode2( aUpper )) )
            {
                SetError( errNoName );
                if ( bAutoCorrect )
                {   // provide single token information and continue
                    ScGlobal::pCharClass->toLower( aUpper );
                    aToken.SetString( aUpper.GetBuffer() );
                    aToken.NewOpCode( ocBad );
                    pRawToken = aToken.Clone();
                    AutoCorrectParsedSymbol();
                }
                else
                {   // we don't need single token information, just a bad formula
                    String aBad( aFormula.Copy( nSrcPos - aOrg.Len() ) );
                    eLastOp = pArr->AddBad( aBad )->GetOpCode();
                    return FALSE;
                }
            }
        }
        return TRUE;
    }
    else
        return FALSE;
}

ScTokenArray* ScCompiler::CompileString( const String& rFormula )
{
    ScTokenArray aArr;
    pArr = &aArr;
    aFormula = rFormula;
    aFormula.EraseLeadingChars();
    aFormula.EraseTrailingChars();
    nSrcPos = 0;
    bCorrected = FALSE;
    if ( bAutoCorrect )
    {
        aCorrectedFormula.Erase();
        aCorrectedSymbol.Erase();
    }
    BYTE nForced = 0;   // ==formula forces recalc even if cell is not visible
    if( aFormula.GetChar(nSrcPos) == '=' )
    {
        nSrcPos++;
        nForced++;
        if ( bAutoCorrect )
            aCorrectedFormula += '=';
    }
    if( aFormula.GetChar(nSrcPos) == '=' )
    {
        nSrcPos++;
        nForced++;
        if ( bAutoCorrect )
            aCorrectedFormula += '=';
    }
    short nBrackets = 0;
    eLastOp = ocOpen;
    while( NextNewToken() )
    {
        if( pRawToken->GetOpCode() == ocOpen )
            nBrackets++;
        else if( pRawToken->GetOpCode() == ocClose )
        {
            if( !nBrackets )
            {
                SetError( errPairExpected );
                if ( bAutoCorrect )
                {
                    bCorrected = TRUE;
                    aCorrectedSymbol.Erase();
                }
            }
            else
                nBrackets--;
        }
        if( !pArr->Add( pRawToken->CreateToken() ) )
        {
            SetError(errCodeOverflow); break;
        }
        eLastOp = pRawToken->GetOpCode();
        if ( bAutoCorrect )
            aCorrectedFormula += aCorrectedSymbol;
    }
    if ( eLastOp != ocBad )
    {
        // With ocBad the remaining formula is a string, too many parentheses
        // would be shown.
        ScByteToken aToken( ocClose );
        while( nBrackets-- )
        {
            if( !pArr->AddToken( aToken ) )
            {
                SetError(errCodeOverflow); break;
            }
            if ( bAutoCorrect )
                aCorrectedFormula += pSymbolTable[ocClose];
        }
    }
    if ( nForced >= 2 )
        pArr->SetRecalcModeForced();
    // remember pArr, in case a subsequent CompileTokenArray() is executed.
    return pArr = new ScTokenArray( aArr );
}

void ScCompiler::PushTokenArray( ScTokenArray* pa, BOOL bTemp )
{
    if ( bAutoCorrect && !pStack )
    {   // #61426# don't merge stacked subroutine code into entered formula
        aCorrectedFormula += aCorrectedSymbol;
        aCorrectedSymbol.Erase();
    }
    ScArrayStack* p = new ScArrayStack;
    p->pNext      = pStack;
    p->pArr       = pArr;
    p->bTemp      = bTemp;
    pStack        = p;
    pArr          = pa;
}

void ScCompiler::PopTokenArray()
{
    if( pStack )
    {
        ScArrayStack* p = pStack;
        pStack = p->pNext;
        p->pArr->nRefs += pArr->nRefs;
        // obtain special RecalcMode from SharedFormula
        if ( pArr->IsRecalcModeAlways() )
            p->pArr->SetRecalcModeAlways();
        else if ( !pArr->IsRecalcModeNormal() && p->pArr->IsRecalcModeNormal() )
            p->pArr->SetMaskedRecalcMode( pArr->GetRecalcMode() );
        p->pArr->SetCombinedBitsRecalcMode( pArr->GetRecalcMode() );
        if( p->bTemp )
            delete pArr;
        pArr = p->pArr;
        delete p;
    }
}

BOOL ScCompiler::GetToken()
{
    static const short nRecursionMax = 42;
    ScCompilerRecursionGuard aRecursionGuard( nRecursion );
    if ( nRecursion > nRecursionMax )
    {
        SetError( errStackOverflow );
        pToken = new ScByteToken( ocStop );
        return FALSE;
    }
    if ( bAutoCorrect && !pStack )
    {   // #61426# don't merge stacked subroutine code into entered formula
        aCorrectedFormula += aCorrectedSymbol;
        aCorrectedSymbol.Erase();
    }
    BOOL bStop = FALSE;
    if( pArr->GetError() && !bIgnoreErrors )
        bStop = TRUE;
    else
    {
        short nWasColRowName;
        if ( pArr->nIndex
          && pArr->pCode[ pArr->nIndex-1 ]->GetOpCode() == ocColRowName )
             nWasColRowName = 1;
        else
             nWasColRowName = 0;
        pToken = pArr->Next();
        while( pToken && pToken->GetOpCode() == ocSpaces )
        {
            if ( nWasColRowName )
                nWasColRowName++;
            if ( bAutoCorrect && !pStack )
                CreateStringFromToken( aCorrectedFormula, pToken, FALSE );
            pToken = pArr->Next();
        }
        if ( bAutoCorrect && !pStack && pToken )
            CreateStringFromToken( aCorrectedSymbol, pToken, FALSE );
        if( !pToken )
        {
            if( pStack )
            {
                PopTokenArray();
                return GetToken();
            }
            else
                bStop = TRUE;
        }
        else
        {
            if ( nWasColRowName >= 2 && pToken->GetOpCode() == ocColRowName )
            {   // aus einem ocSpaces ein ocIntersect im RPN machen
                pToken = new ScByteToken( ocIntersect );
                pArr->nIndex--;     // ganz schweinisch..
            }
        }
    }
    if( bStop )
    {
        pToken = new ScByteToken( ocStop );
        return FALSE;
    }
    if( pToken->GetOpCode() == ocSubTotal )
        glSubTotal = TRUE;
    else if( pToken->GetOpCode() == ocName )
    {
        ScRangeData* pRangeData = pDoc->GetRangeName()->FindIndex( pToken->GetIndex() );
        if (pRangeData)
        {
            USHORT nErr = pRangeData->GetErrCode();
            if( nErr )
                SetError( errNoName );
            else if ( !bCompileForFAP )
            {
                ScTokenArray* pNew;
                // #35168# put named formula into parentheses.
                // #37680# But only if there aren't any yet, parenthetical
                // ocSep doesn't work, e.g. SUM((...;...))
                // or if not directly between ocSep/parenthesis,
                // e.g. SUM(...;(...;...)) no, SUM(...;(...)*3) yes,
                // in short: if it isn't a self-contained expression.
                ScToken* p1 = pArr->PeekPrevNoSpaces();
                ScToken* p2 = pArr->PeekNextNoSpaces();
                OpCode eOp1 = (p1 ? p1->GetOpCode() : ocSep);
                OpCode eOp2 = (p2 ? p2->GetOpCode() : ocSep);
                BOOL bBorder1 = (eOp1 == ocSep || eOp1 == ocOpen);
                BOOL bBorder2 = (eOp2 == ocSep || eOp2 == ocClose);
                BOOL bAddPair = !(bBorder1 && bBorder2);
                if ( bAddPair )
                {
                    pNew = new ScTokenArray;
                    pNew->AddOpCode( ocClose );
                    PushTokenArray( pNew, TRUE );
                    pNew->Reset();
                }
                pNew = pRangeData->GetCode()->Clone();
                PushTokenArray( pNew, TRUE );
                if( pRangeData->HasReferences() )
                {
                    SetRelNameReference();
                    MoveRelWrap();
                }
                pNew->Reset();
                if ( bAddPair )
                {
                    pNew = new ScTokenArray;
                    pNew->AddOpCode( ocOpen );
                    PushTokenArray( pNew, TRUE );
                    pNew->Reset();
                }
                return GetToken();
            }
        }
        else
            SetError(errNoName);
    }
    else if( pToken->GetOpCode() == ocColRowName )
    {
        SingleRefData& rRef = pToken->GetSingleRef();
        rRef.CalcAbsIfRel( aPos );
        if ( !rRef.Valid() )
        {
            SetError( errNoRef );
            return TRUE;
        }
        SCCOL nCol = rRef.nCol;
        SCROW nRow = rRef.nRow;
        SCTAB nTab = rRef.nTab;
        ScAddress aLook( nCol, nRow, nTab );
        BOOL bColName = rRef.IsColRel();
        SCCOL nMyCol = aPos.Col();
        SCROW nMyRow = aPos.Row();
        BOOL bInList = FALSE;
        BOOL bValidName = FALSE;
        ScRangePairList* pRL = (bColName ?
            pDoc->GetColNameRanges() : pDoc->GetRowNameRanges());
        ScRange aRange;
        for ( ScRangePair* pR = pRL->First(); pR; pR = pRL->Next() )
        {
            if ( pR->GetRange(0).In( aLook ) )
            {
                bInList = bValidName = TRUE;
                aRange = pR->GetRange(1);
                if ( bColName )
                {
                    aRange.aStart.SetCol( nCol );
                    aRange.aEnd.SetCol( nCol );
                }
                else
                {
                    aRange.aStart.SetRow( nRow );
                    aRange.aEnd.SetRow( nRow );
                }
                break;  // for
            }
        }
        if ( !bInList && pDoc->GetDocOptions().IsLookUpColRowNames() )
        {   // automagically or created by copying and NamePos isn't in list
            BOOL bString = pDoc->HasStringData( nCol, nRow, nTab );
            if ( !bString && !pDoc->GetCell( aLook ) )
                bString = TRUE;     // empty cell is ok
            if ( bString )
            {   //! coresponds with ScInterpreter::ScColRowNameAuto()
                bValidName = TRUE;
                if ( bColName )
                {   // ColName
                    SCROW nStartRow = nRow + 1;
                    if ( nStartRow > MAXROW )
                        nStartRow = MAXROW;
                    SCROW nMaxRow = MAXROW;
                    if ( nMyCol == nCol )
                    {   // formula cell in same column
                        if ( nMyRow == nStartRow )
                        {   // take remainder under name cell
                            nStartRow++;
                            if ( nStartRow > MAXROW )
                                nStartRow = MAXROW;
                        }
                        else if ( nMyRow > nStartRow )
                        {   // from name cell down to formula cell
                            nMaxRow = nMyRow - 1;
                        }
                    }
                    for ( ScRangePair* pR = pRL->First(); pR; pR = pRL->Next() )
                    {   // next defined ColNameRange below limits row
                        const ScRange& rRange = pR->GetRange(1);
                        if ( rRange.aStart.Col() <= nCol && nCol <= rRange.aEnd.Col() )
                        {   // identical column range
                            SCROW nTmp = rRange.aStart.Row();
                            if ( nStartRow < nTmp && nTmp <= nMaxRow )
                                nMaxRow = nTmp - 1;
                        }
                    }
                    aRange.aStart.Set( nCol, nStartRow, nTab );
                    aRange.aEnd.Set( nCol, nMaxRow, nTab );
                }
                else
                {   // RowName
                    SCCOL nStartCol = nCol + 1;
                    if ( nStartCol > MAXCOL )
                        nStartCol = MAXCOL;
                    SCCOL nMaxCol = MAXCOL;
                    if ( nMyRow == nRow )
                    {   // formula cell in same row
                        if ( nMyCol == nStartCol )
                        {   // take remainder right from name cell
                            nStartCol++;
                            if ( nStartCol > MAXCOL )
                                nStartCol = MAXCOL;
                        }
                        else if ( nMyCol > nStartCol )
                        {   // from name cell right to formula cell
                            nMaxCol = nMyCol - 1;
                        }
                    }
                    for ( ScRangePair* pR = pRL->First(); pR; pR = pRL->Next() )
                    {   // next defined RowNameRange to the right limits column
                        const ScRange& rRange = pR->GetRange(1);
                        if ( rRange.aStart.Row() <= nRow && nRow <= rRange.aEnd.Row() )
                        {   // gleicher Row Bereich
                            SCCOL nTmp = rRange.aStart.Col();
                            if ( nStartCol < nTmp && nTmp <= nMaxCol )
                                nMaxCol = nTmp - 1;
                        }
                    }
                    aRange.aStart.Set( nStartCol, nRow, nTab );
                    aRange.aEnd.Set( nMaxCol, nRow, nTab );
                }
            }
        }
        if ( bValidName )
        {
            // And now the magic to distinguish between a range and a single
            // cell thereof, which is picked position-dependent of the formula
            // cell. If a direct neighbor is a binary operator (ocAdd, ...) a
            // SingleRef matching the column/row of the formula cell is
            // generated. A ocColRowName or ocIntersect as a neighbor results
            // in a range. Special case: if label is valid for a single cell, a
            // position independent SingleRef is generated.
            BOOL bSingle = (aRange.aStart == aRange.aEnd);
            BOOL bFound;
            if ( bSingle )
                bFound = TRUE;
            else
            {
                ScToken* p1 = pArr->PeekPrevNoSpaces();
                ScToken* p2 = pArr->PeekNextNoSpaces();
                // begin/end of a formula => single
                OpCode eOp1 = p1 ? p1->GetOpCode() : ocAdd;
                OpCode eOp2 = p2 ? p2->GetOpCode() : ocAdd;
                if ( eOp1 != ocColRowName && eOp1 != ocIntersect
                    && eOp2 != ocColRowName && eOp2 != ocIntersect )
                {
                    if ( (ocEndDiv < eOp1 && eOp1 < ocEndBinOp)
                        || (ocEndDiv < eOp2 && eOp2 < ocEndBinOp) )
                        bSingle = TRUE;
                }
                if ( bSingle )
                {   // column and/or row must match range
                    if ( bColName )
                    {
                        bFound = (aRange.aStart.Row() <= nMyRow
                            && nMyRow <= aRange.aEnd.Row());
                        if ( bFound )
                            aRange.aStart.SetRow( nMyRow );
                    }
                    else
                    {
                        bFound = (aRange.aStart.Col() <= nMyCol
                            && nMyCol <= aRange.aEnd.Col());
                        if ( bFound )
                            aRange.aStart.SetCol( nMyCol );
                    }
                }
                else
                    bFound = TRUE;
            }
            if ( !bFound )
                SetError(errNoRef);
            else if ( !bCompileForFAP )
            {
                ScTokenArray* pNew = new ScTokenArray;
                if ( bSingle )
                {
                    SingleRefData aRefData;
                    aRefData.InitAddress( aRange.aStart );
                    if ( bColName )
                        aRefData.SetColRel( TRUE );
                    else
                        aRefData.SetRowRel( TRUE );
                    aRefData.CalcRelFromAbs( aPos );
                    pNew->AddSingleReference( aRefData );
                }
                else
                {
                    ComplRefData aRefData;
                    aRefData.InitRange( aRange );
                    if ( bColName )
                    {
                        aRefData.Ref1.SetColRel( TRUE );
                        aRefData.Ref2.SetColRel( TRUE );
                    }
                    else
                    {
                        aRefData.Ref1.SetRowRel( TRUE );
                        aRefData.Ref2.SetRowRel( TRUE );
                    }
                    aRefData.CalcRelFromAbs( aPos );
                    if ( bInList )
                        pNew->AddDoubleReference( aRefData );
                    else
                    {   // automagically
                        pNew->Add( new ScDoubleRefToken( ocColRowNameAuto,
                            aRefData ) );
                    }
                }
                PushTokenArray( pNew, TRUE );
                pNew->Reset();
                return GetToken();
            }
        }
        else
            SetError(errNoName);
    }
    else if( pToken->GetOpCode() == ocDBArea )
    {
        ScDBData* pDBData = pDoc->GetDBCollection()->FindIndex( pToken->GetIndex() );
        if ( !pDBData )
            SetError(errNoName);
        else if ( !bCompileForFAP )
        {
            ComplRefData aRefData;
            aRefData.InitFlags();
            pDBData->GetArea(   (SCTAB&) aRefData.Ref1.nTab,
                                (SCCOL&) aRefData.Ref1.nCol,
                                (SCROW&) aRefData.Ref1.nRow,
                                (SCCOL&) aRefData.Ref2.nCol,
                                (SCROW&) aRefData.Ref2.nRow);
            aRefData.Ref2.nTab    = aRefData.Ref1.nTab;
            aRefData.CalcRelFromAbs( aPos );
            ScTokenArray* pNew = new ScTokenArray;
            pNew->AddDoubleReference( aRefData );
            PushTokenArray( pNew, TRUE );
            pNew->Reset();
            return GetToken();
        }
    }
    else if( pToken->GetType() == svSingleRef )
    {
//      if (!pDoc->HasTable( pToken->aRef.Ref1.nTab ) )
//          SetError(errNoRef);
        pArr->nRefs++;
    }
    else if( pToken->GetType() == svDoubleRef )
    {
//      if (!pDoc->HasTable( pToken->aRef.Ref1.nTab ) ||
//          !pDoc->HasTable( pToken->aRef.Ref2.nTab ))
//          SetError(errNoRef);
        pArr->nRefs++;
    }
    return TRUE;
}

OpCode ScCompiler::NextToken()
{
    if( !GetToken() )
        return ocStop;
    OpCode eOp = pToken->GetOpCode();
    // #38815# CompileTokenArray() with error reset results in error again
    if ( eOp == ocBad )
        SetError( errNoName );
    // There must be an operator before a push
    if ( (eOp == ocPush || eOp == ocColRowNameAuto) &&
            !( (eLastOp == ocOpen) || (eLastOp == ocSep) ||
                ((eLastOp > ocEndDiv) && (eLastOp < ocEndUnOp))) )
        SetError(errOperatorExpected);
    // Operator and Plus => operator
    if (eOp == ocAdd && (eLastOp == ocOpen || eLastOp == ocSep ||
                (eLastOp > ocEndDiv && eLastOp < ocEndUnOp)))
        eOp = NextToken();
    else
    {
        // Before an operator there must not be another operator, with the
        // exception of AND and OR.
        if ( eOp != ocAnd && eOp != ocOr
          && ( eOp > ocEndDiv && eOp < ocEndBinOp )
          && ( eLastOp == ocOpen || eLastOp == ocSep
            || (eLastOp > ocEndDiv && eLastOp < ocEndUnOp)) )
        {
            SetError(errVariableExpected);
            if ( bAutoCorrect && !pStack )
            {
                if ( eOp == eLastOp || eLastOp == ocOpen )
                {   // throw away duplicated operator
                    aCorrectedSymbol.Erase();
                    bCorrected = TRUE;
                }
                else
                {
                    xub_StrLen nPos = aCorrectedFormula.Len();
                    if ( nPos )
                    {
                        nPos--;
                        sal_Unicode c = aCorrectedFormula.GetChar( nPos );
                        switch ( eOp )
                        {   // swap operators
                            case ocGreater:
                                if ( c == pSymbolTable[ocEqual].GetChar(0) )
                                {   // >= instead of =>
                                    aCorrectedFormula.SetChar( nPos,
                                        pSymbolTable[ocGreater].GetChar(0) );
                                    aCorrectedSymbol = c;
                                    bCorrected = TRUE;
                                }
                            break;
                            case ocLess:
                                if ( c == pSymbolTable[ocEqual].GetChar(0) )
                                {   // <= instead of =<
                                    aCorrectedFormula.SetChar( nPos,
                                        pSymbolTable[ocLess].GetChar(0) );
                                    aCorrectedSymbol = c;
                                    bCorrected = TRUE;
                                }
                                else if ( c == pSymbolTable[ocGreater].GetChar(0) )
                                {   // <> instead of ><
                                    aCorrectedFormula.SetChar( nPos,
                                        pSymbolTable[ocLess].GetChar(0) );
                                    aCorrectedSymbol = c;
                                    bCorrected = TRUE;
                                }
                            break;
                            case ocMul:
                                if ( c == pSymbolTable[ocSub].GetChar(0) )
                                {   // *- instead of -*
                                    aCorrectedFormula.SetChar( nPos,
                                        pSymbolTable[ocMul].GetChar(0) );
                                    aCorrectedSymbol = c;
                                    bCorrected = TRUE;
                                }
                            break;
                            case ocDiv:
                                if ( c == pSymbolTable[ocSub].GetChar(0) )
                                {   // /- instead of -/
                                    aCorrectedFormula.SetChar( nPos,
                                        pSymbolTable[ocDiv].GetChar(0) );
                                    aCorrectedSymbol = c;
                                    bCorrected = TRUE;
                                }
                            break;
                            default:
                                ;   // nothing
                        }
                    }
                }
            }
        }
        eLastOp = eOp;
    }
    return eOp;
}

//---------------------------------------------------------------------------

BOOL ScCompiler::CompileTokenArray()
{
    glSubTotal = FALSE;
    bCorrected = FALSE;
    if( !pArr->nError || bIgnoreErrors )
    {
        if ( bAutoCorrect )
        {
            aCorrectedFormula.Erase();
            aCorrectedSymbol.Erase();
        }
        pArr->nRefs = 0;    // count from start
        pArr->DelRPN();
        pStack = NULL;
        ScToken* pData[ MAXCODE ];
        pCode = pData;
        BOOL bWasForced = pArr->IsRecalcModeForced();
        if ( bWasForced )
        {
            if ( bAutoCorrect )
                aCorrectedFormula = '=';
        }
        pArr->ClearRecalcMode();
        pArr->Reset();
        eLastOp = ocOpen;
        pc = 0;
        NextToken();
        Expression();

        USHORT nErrorBeforePop = pArr->nError;

        while( pStack )
            PopTokenArray();
        if( pc )
        {
            pArr->pRPN = new ScToken*[ pc ];
            pArr->nRPN = pc;
            memcpy( pArr->pRPN, pData, pc * sizeof( ScToken* ) );
        }

        if( !pArr->nError && nErrorBeforePop )
            pArr->nError = nErrorBeforePop;     // once an error, always an error

        if( pArr->nError && !bIgnoreErrors )
    {
            pArr->DelRPN();
            pArr->SetHyperLink(FALSE);
    }

        if ( bWasForced )
            pArr->SetRecalcModeForced();
    }
    if( nNumFmt == NUMBERFORMAT_UNDEFINED )
        nNumFmt = NUMBERFORMAT_NUMBER;
    return glSubTotal;
}

//---------------------------------------------------------------------------
// Append token to RPN code
//---------------------------------------------------------------------------

void ScCompiler::PutCode( ScTokenRef& p )
{
    if( pc >= MAXCODE-1 )
    {
        if ( pc == MAXCODE-1 )
        {
            p = new ScByteToken( ocStop );
            p->IncRef();
            *pCode++ = p;
            ++pc;
        }
        SetError(errCodeOverflow);
        return;
    }
    if( pArr->GetError() && !bCompileForFAP )
        return;
    ForceArrayOperator( p, pCurrentFactorToken);
    p->IncRef();
    *pCode++ = p;
    pc++;
}

//---------------------------------------------------------------------------
// RPN creation by recursion
//---------------------------------------------------------------------------

void ScCompiler::Factor()
{
    if ( pArr->GetError() && !bIgnoreErrors )
        return;

    CurrentFactor pFacToken( this );

    OpCode eOp = pToken->GetOpCode();
    if( eOp == ocPush || eOp == ocColRowNameAuto || eOp == ocMatRef ||
            eOp == ocDBArea
            || (bCompileForFAP && ((eOp == ocName) || (eOp == ocDBArea)
            || (eOp == ocColRowName) || (eOp == ocBad)))
        )
    {
        PutCode( pToken );
        eOp = NextToken();
        if( eOp == ocOpen )
        {
            // PUSH( is an error that may be caused by an unknown function.
            SetError(
                ( pToken->GetType() == svString
               || pToken->GetType() == svSingleRef )
               ? errNoName : errOperatorExpected );
            if ( bAutoCorrect && !pStack )
            {   // assume multiplication
                aCorrectedFormula += pSymbolTable[ocMul];
                bCorrected = TRUE;
                NextToken();
                eOp = Expression();
                if( eOp != ocClose )
                    SetError(errPairExpected);
                else
                    eOp = NextToken();
            }
        }
    }
    else if( eOp == ocOpen )
    {
        NextToken();
        eOp = Expression();
        if( eOp != ocClose )
            SetError(errPairExpected);
        else
            eOp = NextToken();
    }
    else
    {
        if( nNumFmt == NUMBERFORMAT_UNDEFINED )
            nNumFmt = lcl_GetRetFormat( eOp );
        // Functions that have to be always recalculated
        switch( eOp )
        {
            // no parameters:
            case ocRandom:
            case ocGetActDate:
            case ocGetActTime:
            // one parameter:
            case ocFormula:
            case ocInfo:
            // more than one parameters:
                // ocIndirect otherwise would have to do StopListening and
                // StartListening on a reference for every interpreted value.
            case ocIndirect:
                // ocOffset results in indirect references.
            case ocOffset:
                pArr->SetRecalcModeAlways();
            break;
                // Functions recalculated on every document load.
                // Don't use SetRecalcModeOnLoad() which would override
                // ModeAlways.
            case ocConvert :
                pArr->AddRecalcMode( RECALCMODE_ONLOAD );
            break;
                // If the referred cell is moved the value changes.
            case ocColumn :
            case ocRow :
                // ocCell needs recalc on move for some possible type values.
            case ocCell :
                pArr->SetRecalcModeOnRefMove();
            break;
            case ocHyperLink :
                pArr->SetHyperLink(TRUE);
            break;
            default:
                ;   // nothing
        }
        if( eOp > ocEndUnOp && eOp < ocEndNoPar)
        {
            pFacToken = pToken;
            eOp = NextToken();
            if (eOp != ocOpen)
            {
                SetError(errPairExpected);
                PutCode( pFacToken );
            }
            else
            {
                eOp = NextToken();
                if (eOp != ocClose)
                    SetError(errPairExpected);
                PutCode(pFacToken);
                eOp = NextToken();
            }
        }
        // special cases NOT() and NEG()
        else if( eOp == ocNot || eOp == ocNeg
              || ( eOp > ocEndNoPar && eOp < ocEnd1Par) )
        {
            pFacToken = pToken;
            eOp = NextToken();
            if( nNumFmt == NUMBERFORMAT_UNDEFINED && eOp == ocNot )
                nNumFmt = NUMBERFORMAT_LOGICAL;
            if (eOp == ocOpen)
            {
                NextToken();
                eOp = Expression();
            }
            else
                SetError(errPairExpected);
            if (eOp != ocClose)
                SetError(errPairExpected);
            else if ( !pArr->GetError() )
                pFacToken->SetByte( 1 );
            PutCode( pFacToken );
            eOp = NextToken();
        }
        else if ((eOp > ocEnd1Par && eOp < ocEnd2Par)
                || eOp == ocExternal
                || eOp == ocMacro
                || eOp == ocAnd
                || eOp == ocOr
                || ( eOp >= ocInternalBegin && eOp <= ocInternalEnd )
                || (bCompileForFAP && ((eOp == ocIf) || (eOp == ocChose)))
            )
        {
            OpCode eFuncOp = eOp;
            pFacToken = pToken;
            eOp = NextToken();
            BOOL bNoParam = FALSE;
            BOOL bNoPair = FALSE;
            BYTE nMultiAreaSep = 0;
            if (eOp == ocOpen)
            {
                eOp = NextToken();
                if ( eFuncOp == ocIndex && eOp == ocOpen )
                {   // multiple areas
                    BYTE SepCount = 0;
                    do
                    {
                        eOp = NextToken();
                        if ( eOp != ocClose )
                        {
                            SepCount++;
                            eOp = Expression();
                        }
                    } while ( (eOp == ocSep) && (!pArr->GetError() || bIgnoreErrors) );
                    if ( eOp != ocClose )
                        SetError(errPairExpected);
                    eOp = NextToken();
                    if ( eOp == ocSep )
                    {
                        nMultiAreaSep = 1;
                        eOp = NextToken();
                    }
                    if ( SepCount == 0 )
                    {
                        if ( eOp == ocClose )
                            bNoParam = TRUE;
                    }
                    else
                    {
                        ScTokenRef xTemp = new ScByteToken( ocPush, SepCount );
                        PutCode( xTemp );
                        if ( eOp != ocClose )
                            eOp = Expression();
                    }
                }
                else
                {
                    if (eOp == ocClose)
                        bNoParam = TRUE;
                    else
                        eOp = Expression();
                }
            }
            else
            {
                if (pFacToken->GetOpCode() == ocMultiArea)
                {
                    bNoPair = TRUE;
                    NextToken();
                    eOp = Expression();
                }
                else
                    SetError(errPairExpected);
            }
            BYTE SepCount = nMultiAreaSep;
            if( !bNoParam )
            {
                SepCount++;
                while ( (eOp == ocSep) && (!pArr->GetError() || bIgnoreErrors) )
                {
                    SepCount++;
                    NextToken();
                    eOp = Expression();
                }
            }
            if ((eOp != ocClose) && !bNoPair )
                SetError(errPairExpected);
            else
                eOp = NextToken();
            // Jumps are just normal functions for the FunctionAutoPilot tree view
            if ( bCompileForFAP && pFacToken->GetType() == svJump )
                pFacToken = new ScFAPToken( pFacToken->GetOpCode(), SepCount, pFacToken );
            else
                pFacToken->SetByte( SepCount );
            PutCode( pFacToken );
        }
        else if (eOp == ocIf || eOp == ocChose)
        {
            // the PC counters are -1
            pFacToken = pToken;
            if ( eOp == ocIf )
                pFacToken->GetJump()[ 0 ] = 3;  // if, else, behind
            else
                pFacToken->GetJump()[ 0 ] = MAXJUMPCOUNT+1;
            eOp = NextToken();
            if (eOp == ocOpen)
            {
                NextToken();
                eOp = Expression();
            }
            else
                SetError(errPairExpected);
            short nJumpCount = 0;
            PutCode( pFacToken );
            // #36253# during AutoCorrect (since pArr->nError is ignored) an
            // unlimited ocIf would crash because ScRawToken::Clone() allocates
            // the JumpBuffer according to nJump[0]*2+2, which is 3*2+2 on
            // ocIf.
            const short nJumpMax =
                (pFacToken->GetOpCode() == ocIf ? 3 : MAXJUMPCOUNT);
            while ( (nJumpCount < (MAXJUMPCOUNT - 1)) && (eOp == ocSep)
                    && (!pArr->GetError() || bIgnoreErrors) )
            {
                if ( ++nJumpCount <= nJumpMax )
                    pFacToken->GetJump()[nJumpCount] = pc-1;
                NextToken();
                eOp = Expression();
                // ocSep or ocClose terminate the subexpression
                PutCode( pToken );
            }
            if (eOp != ocClose)
                SetError(errPairExpected);
            else
            {
                eOp = NextToken();
                // always limit to nJumpMax, no arbitrary overwrites
                if ( ++nJumpCount <= nJumpMax )
                    pFacToken->GetJump()[ nJumpCount ] = pc-1;
                if ((pFacToken->GetOpCode() == ocIf && (nJumpCount > 3)) ||
                                 (nJumpCount >= MAXJUMPCOUNT))
                    SetError(errIllegalParameter);
                else
                    pFacToken->GetJump()[ 0 ] = nJumpCount;
            }
        }
        else if ( eOp == ocBad )
        {
            SetError( errNoName );
        }
        else if ( eOp == ocClose )
        {
            SetError( errParameterExpected );
        }
        else if ( eOp == ocMissing )
        {   // #84460# May occur if imported from Xcl.
            // The real value for missing parameters depends on the function
            // where it is used, interpreter would have to handle this.
            // If it does remove this error case here, that could also be the
            // time to generate ocMissing in between subsequent ocSep.
            // Xcl import should map missings to values if possible.
            SetError( errParameterExpected );
        }
        else if ( eOp == ocSep )
        {   // Subsequent ocSep
            SetError( errParameterExpected );
            if ( bAutoCorrect && !pStack )
            {
                aCorrectedSymbol.Erase();
                bCorrected = TRUE;
            }
        }
        else
        {
            SetError( errUnknownToken );
            if ( bAutoCorrect && !pStack )
            {
                if ( eOp == ocStop )
                {   // trailing operator w/o operand
                    xub_StrLen nLen = aCorrectedFormula.Len();
                    if ( nLen )
                        aCorrectedFormula.Erase( nLen - 1 );
                    aCorrectedSymbol.Erase();
                    bCorrected = TRUE;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------

void ScCompiler::UnionCutLine()
{
    Factor();
    while (pToken->GetOpCode() == ocIntersect)
    {
        ScTokenRef p = pToken;
        NextToken();
        Factor();
        PutCode(p);
    }
}

//---------------------------------------------------------------------------

void ScCompiler::UnaryLine()
{
    if( pToken->GetOpCode() == ocAdd )
        GetToken();
    else if ( pToken->GetOpCode() > ocNot && pToken->GetOpCode() < ocEndUnOp )
    {
        ScTokenRef p = pToken;
        NextToken();
        UnaryLine();
        PutCode( p );
    }
    else
        UnionCutLine();
}

//---------------------------------------------------------------------------

void ScCompiler::PostOpLine()
{
    UnaryLine();
    while ( pToken->GetOpCode() == ocPercentSign )
    {   // this operator _follows_ its operand
        PutCode( pToken );
        NextToken();
    }
}

//---------------------------------------------------------------------------

void ScCompiler::PowLine()
{
    PostOpLine();
    while (pToken->GetOpCode() == ocPow)
    {
        ScTokenRef p = pToken;
        NextToken();
        PostOpLine();
        PutCode(p);
    }
}

//---------------------------------------------------------------------------

void ScCompiler::MulDivLine()
{
    PowLine();
    while (pToken->GetOpCode() == ocMul || pToken->GetOpCode() == ocDiv)
    {
        ScTokenRef p = pToken;
        NextToken();
        PowLine();
        PutCode(p);
    }
}

//---------------------------------------------------------------------------

void ScCompiler::AddSubLine()
{
    MulDivLine();
    while (pToken->GetOpCode() == ocAdd || pToken->GetOpCode() == ocSub)
    {
        ScTokenRef p = pToken;
        NextToken();
        MulDivLine();
        PutCode(p);
    }
}

//---------------------------------------------------------------------------

void ScCompiler::ConcatLine()
{
    AddSubLine();
    while (pToken->GetOpCode() == ocAmpersand)
    {
        ScTokenRef p = pToken;
        NextToken();
        AddSubLine();
        PutCode(p);
    }
}

//---------------------------------------------------------------------------

void ScCompiler::CompareLine()
{
    ConcatLine();
    while (pToken->GetOpCode() >= ocEqual && pToken->GetOpCode() <= ocGreaterEqual)
    {
        ScTokenRef p = pToken;
        NextToken();
        ConcatLine();
        PutCode(p);
    }
}

//---------------------------------------------------------------------------

void ScCompiler::NotLine()
{
    CompareLine();
    while (pToken->GetOpCode() == ocNot)
    {
        ScTokenRef p = pToken;
        NextToken();
        CompareLine();
        PutCode(p);
    }
}

//---------------------------------------------------------------------------

OpCode ScCompiler::Expression()
{
    static const short nRecursionMax = 42;
    ScCompilerRecursionGuard aRecursionGuard( nRecursion );
    if ( nRecursion > nRecursionMax )
    {
        SetError( errStackOverflow );
        return ocStop;      //! generate token instead?
    }
    NotLine();
    while (pToken->GetOpCode() == ocAnd || pToken->GetOpCode() == ocOr)
    {
        ScTokenRef p = pToken;
        pToken->SetByte( 2 );       // 2 parameters!
        NextToken();
        NotLine();
        PutCode(p);
    }
    return pToken->GetOpCode();
}

//-----------------------------------------------------------------------------

BOOL ScCompiler::HasModifiedRange()
{
    pArr->Reset();
    for ( ScToken* t = pArr->Next(); t; t = pArr->Next() )
    {
        OpCode eOpCode = t->GetOpCode();
        if ( eOpCode == ocName )
        {
            ScRangeData* pRangeData = pDoc->GetRangeName()->FindIndex(t->GetIndex());

            if (pRangeData && pRangeData->IsModified())
                return TRUE;
        }
        else if ( eOpCode == ocDBArea )
        {
            ScDBData* pDBData = pDoc->GetDBCollection()->FindIndex(t->GetIndex());

            if (pDBData && pDBData->IsModified())
                return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------

template< typename T, typename S >
S lcl_adjval( S& n, T pos, T max, BOOL bRel )
{
    max++;
    if( bRel )
        n += pos;
    if( n < 0 )
        n += max;
    else if( n >= max )
        n -= max;
    if( bRel )
        n -= pos;
    return n;
}

void ScCompiler::AdjustReference( SingleRefData& r )
{
    if( r.IsColRel() )
        r.nCol = lcl_adjval( r.nCol, aPos.Col(), MAXCOL, r.IsColRel() );
    if( r.IsRowRel() )
        r.nRow = lcl_adjval( r.nRow, aPos.Row(), MAXROW, r.IsRowRel() );
    if( r.IsTabRel() )
        r.nTab = lcl_adjval( r.nTab, aPos.Tab(), static_cast<SCTAB>(nMaxTab), r.IsTabRel() );
}

// reference of named range with relative references

void ScCompiler::SetRelNameReference()
{
    pArr->Reset();
    for( ScToken* t = pArr->GetNextReference(); t;
                  t = pArr->GetNextReference() )
    {
        SingleRefData& rRef1 = t->GetSingleRef();
        if ( rRef1.IsColRel() || rRef1.IsRowRel() || rRef1.IsTabRel() )
            rRef1.SetRelName( TRUE );
        if ( t->GetType() == svDoubleRef )
        {
            SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
            if ( rRef2.IsColRel() || rRef2.IsRowRel() || rRef2.IsTabRel() )
                rRef2.SetRelName( TRUE );
        }
    }
}

// Wrap-adjust relative references of a RangeName to current position,
// don't call for other token arrays!
void ScCompiler::MoveRelWrap()
{
    pArr->Reset();
    for( ScToken* t = pArr->GetNextReference(); t;
                  t = pArr->GetNextReference() )
    {
        if ( t->GetType() == svSingleRef )
            ScRefUpdate::MoveRelWrap( pDoc, aPos, SingleDoubleRefModifier( t->GetSingleRef() ).Ref() );
        else
            ScRefUpdate::MoveRelWrap( pDoc, aPos, t->GetDoubleRef() );
    }
}

// static
// Wrap-adjust relative references of a RangeName to current position,
// don't call for other token arrays!
void ScCompiler::MoveRelWrap( ScTokenArray& rArr, ScDocument* pDoc,
            const ScAddress& rPos )
{
    rArr.Reset();
    for( ScToken* t = rArr.GetNextReference(); t;
                  t = rArr.GetNextReference() )
    {
        if ( t->GetType() == svSingleRef )
            ScRefUpdate::MoveRelWrap( pDoc, rPos, SingleDoubleRefModifier( t->GetSingleRef() ).Ref() );
        else
            ScRefUpdate::MoveRelWrap( pDoc, rPos, t->GetDoubleRef() );
    }
}

ScRangeData* ScCompiler::UpdateReference(UpdateRefMode eUpdateRefMode,
                                 const ScAddress& rOldPos, const ScRange& r,
                                 SCsCOL nDx, SCsROW nDy, SCsTAB nDz,
                                 BOOL& rChanged)
{
    rChanged = FALSE;
    if ( eUpdateRefMode == URM_COPY )
    {   // Normally nothing has to be done here since RelRefs are used, also
        // SharedFormulas don't need any special handling.
        // #67383# But ColRowName tokens pointing to a ColRow header which was
        // copied along with this formula need to be updated to point to the
        // copied header instead of the old position's new intersection.
        ScToken* t;
        pArr->Reset();
        for( t = pArr->GetNextColRowName(); t;
             t = pArr->GetNextColRowName() )
        {
            SingleRefData& rRef = t->GetSingleRef();
            rRef.CalcAbsIfRel( rOldPos );
            ScAddress aNewRef( rRef.nCol + nDx, rRef.nRow + nDy, rRef.nTab + nDz );
            if ( r.In( aNewRef ) )
            {   // yes, this is URM_MOVE
                if ( ScRefUpdate::Update( pDoc, URM_MOVE, aPos,
                        r, nDx, nDy, nDz,
                        SingleDoubleRefModifier( rRef ).Ref() )
                        != UR_NOTHING
                    )
                    rChanged = TRUE;
            }
        }
        return NULL;
    }
    else
    {
/*
 * Set SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE to 1 if we wanted to preserve as
 * many shared formulas as possible instead of replacing them with direct code.
 * Note that this may produce shared formula usage Excel doesn't understand,
 * which would have to be adapted for in the export filter. Advisable as a long
 * term goal, since it could decrease memory footprint.
 */
#define SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE 0
        ScRangeData* pRangeData = NULL;
        ScToken* t;
        pArr->Reset();
        for( t = pArr->GetNextReferenceOrName(); t;
             t = pArr->GetNextReferenceOrName() )
        {
            if( t->GetOpCode() == ocName )
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex( t->GetIndex() );
                if (pName && pName->HasType(RT_SHAREDMOD))
                {
                    pRangeData = pName;     // maybe need a replacement of shared with own code
#if ! SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                    rChanged = TRUE;
#endif
                }
            }
            else if( t->GetType() != svIndex )  // it may be a DB area!!!
            {
                t->CalcAbsIfRel( rOldPos );
                if ( t->GetType() == svSingleRef )
                {
                    if ( ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                            r, nDx, nDy, nDz,
                            SingleDoubleRefModifier( t->GetSingleRef() ).Ref() )
                            != UR_NOTHING
                        )
                        rChanged = TRUE;
                }
                else
                {
                    if ( ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                                r, nDx, nDy, nDz, t->GetDoubleRef() )
                            != UR_NOTHING
                        )
                        rChanged = TRUE;
                }
            }
        }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
        BOOL bEasyShared, bPosInRange;
        if ( !pRangeData )
            bEasyShared = bPosInRange = FALSE;
        else
        {
            bEasyShared = TRUE;
            bPosInRange = r.In( eUpdateRefMode == URM_MOVE ? aPos : rOldPos );
        }
#endif
        pArr->Reset();
        for( t = pArr->GetNextReferenceRPN(); t;
             t = pArr->GetNextReferenceRPN() )
        {
            if ( t->GetRef() != 1 )
            {
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                bEasyShared = FALSE;
#endif
            }
            else
            {   // if nRefCnt>1 it's already updated in token code
                if ( t->GetType() == svSingleRef )
                {
                    SingleRefData& rRef = t->GetSingleRef();
                    SingleDoubleRefModifier aMod( rRef );
                    if ( rRef.IsRelName() )
                    {
                        ScRefUpdate::MoveRelWrap( pDoc, aPos, aMod.Ref() );
                        rChanged = TRUE;
                    }
                    else
                    {
                        aMod.Ref().CalcAbsIfRel( rOldPos );
                        if ( ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                                    r, nDx, nDy, nDz, aMod.Ref() )
                                != UR_NOTHING
                            )
                            rChanged = TRUE;
                    }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                    if ( bEasyShared )
                    {
                        const SingleRefData& rSRD = aMod.Ref().Ref1;
                        ScAddress aRef( rSRD.nCol, rSRD.nRow, rSRD.nTab );
                        if ( r.In( aRef ) != bPosInRange )
                            bEasyShared = FALSE;
                    }
#endif
                }
                else
                {
                    ComplRefData& rRef = t->GetDoubleRef();
                    if ( rRef.Ref1.IsRelName() || rRef.Ref2.IsRelName() )
                    {
                        ScRefUpdate::MoveRelWrap( pDoc, aPos, rRef );
                        rChanged = TRUE;
                    }
                    else
                    {
                        if ( ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                                    r, nDx, nDy, nDz, rRef )
                                != UR_NOTHING
                            )
                            rChanged = TRUE;
                    }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                    if ( bEasyShared )
                    {
                        ScRange aRef( rRef.Ref1.nCol, rRef.Ref1.nRow,
                                rRef.Ref1.nTab, rRef.Ref2.nCol, rRef.Ref2.nRow,
                                rRef.Ref2.nTab );
                        if ( r.In( aRef ) != bPosInRange )
                            bEasyShared = FALSE;
                    }
#endif
                }
            }
        }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
        if ( pRangeData )
        {
            if ( bEasyShared )
                pRangeData = 0;
            else
                rChanged = TRUE;
        }
#endif
#undef SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
        return pRangeData;
    }
}

BOOL ScCompiler::UpdateNameReference(UpdateRefMode eUpdateRefMode,
                                     const ScRange& r,
                                     SCsCOL nDx, SCsROW nDy, SCsTAB nDz,
                                     BOOL& rChanged, BOOL bSharedFormula)
{
    BOOL bRelRef = FALSE;   // set if relative reference
    rChanged = FALSE;
    pArr->Reset();
    for( ScToken* t = pArr->GetNextReference(); t;
                  t = pArr->GetNextReference() )
    {
        SingleDoubleRefModifier aMod( *t );
        ComplRefData& rRef = aMod.Ref();
        bRelRef = rRef.Ref1.IsColRel() || rRef.Ref1.IsRowRel() ||
            rRef.Ref1.IsTabRel();
        if (!bRelRef && t->GetType() == svDoubleRef)
            bRelRef = rRef.Ref2.IsColRel() || rRef.Ref2.IsRowRel() ||
                rRef.Ref2.IsTabRel();
        bool bUpdate;
        if (bSharedFormula)
        {
            bUpdate = !rRef.Ref1.IsColRel() || !rRef.Ref1.IsRowRel() ||
                !rRef.Ref1.IsTabRel();
            if (bUpdate)
                rRef.Ref1.CalcAbsIfRel( aPos);
            if (t->GetType() == svDoubleRef)
            {
                if (!bUpdate)
                    bUpdate = !rRef.Ref2.IsColRel() || !rRef.Ref2.IsRowRel() ||
                        !rRef.Ref2.IsTabRel();
                if (bUpdate)
                    rRef.Ref2.CalcAbsIfRel( aPos);
            }
        }
        else
        {
            bUpdate = !bRelRef;
        }
        if (bUpdate)
        {
            if (ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos, r,
                        nDx, nDy, nDz, rRef, ScRefUpdate::ABSOLUTE)
                    != UR_NOTHING )
                rChanged = TRUE;
        }
    }
    return bRelRef;
}


void ScCompiler::UpdateSharedFormulaReference( UpdateRefMode eUpdateRefMode,
                                  const ScAddress& rOldPos, const ScRange& r,
                                  SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    if ( eUpdateRefMode == URM_COPY )
        return ;
    else
    {
        ScToken* t;
        pArr->Reset();
        for( t = pArr->GetNextReference(); t;
             t = pArr->GetNextReference() )
        {
            if( t->GetType() != svIndex )   // it may be a DB area!!!
            {
                t->CalcAbsIfRel( rOldPos );
                // Absolute references have been already adjusted in the named
                // shared formula itself prior to breaking the shared formula
                // and calling this function. Don't readjust them again.
                SingleDoubleRefModifier aMod( *t );
                ComplRefData& rRef = aMod.Ref();
                ComplRefData aBkp = rRef;
                ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                                            r, nDx, nDy, nDz, rRef );
                // restore absolute parts
                if ( !aBkp.Ref1.IsColRel() )
                {
                    rRef.Ref1.nCol = aBkp.Ref1.nCol;
                    rRef.Ref1.nRelCol = aBkp.Ref1.nRelCol;
                    rRef.Ref1.SetColDeleted( aBkp.Ref1.IsColDeleted() );
                }
                if ( !aBkp.Ref1.IsRowRel() )
                {
                    rRef.Ref1.nRow = aBkp.Ref1.nRow;
                    rRef.Ref1.nRelRow = aBkp.Ref1.nRelRow;
                    rRef.Ref1.SetRowDeleted( aBkp.Ref1.IsRowDeleted() );
                }
                if ( !aBkp.Ref1.IsTabRel() )
                {
                    rRef.Ref1.nTab = aBkp.Ref1.nTab;
                    rRef.Ref1.nRelTab = aBkp.Ref1.nRelTab;
                    rRef.Ref1.SetTabDeleted( aBkp.Ref1.IsTabDeleted() );
                }
                if ( t->GetType() == svDoubleRef )
                {
                    if ( !aBkp.Ref2.IsColRel() )
                    {
                        rRef.Ref2.nCol = aBkp.Ref2.nCol;
                        rRef.Ref2.nRelCol = aBkp.Ref2.nRelCol;
                        rRef.Ref2.SetColDeleted( aBkp.Ref2.IsColDeleted() );
                    }
                    if ( !aBkp.Ref2.IsRowRel() )
                    {
                        rRef.Ref2.nRow = aBkp.Ref2.nRow;
                        rRef.Ref2.nRelRow = aBkp.Ref2.nRelRow;
                        rRef.Ref2.SetRowDeleted( aBkp.Ref2.IsRowDeleted() );
                    }
                    if ( !aBkp.Ref2.IsTabRel() )
                    {
                        rRef.Ref2.nTab = aBkp.Ref2.nTab;
                        rRef.Ref2.nRelTab = aBkp.Ref2.nRelTab;
                        rRef.Ref2.SetTabDeleted( aBkp.Ref2.IsTabDeleted() );
                    }
                }
            }
        }
    }
}


ScRangeData* ScCompiler::UpdateInsertTab( SCTAB nTable, BOOL bIsName )
{
    ScRangeData* pRangeData = NULL;
    SCTAB nPosTab = aPos.Tab();     // _after_ incremented!
    SCTAB nOldPosTab = ((nPosTab > nTable) ? (nPosTab - 1) : nPosTab);
    BOOL bIsRel = FALSE;
    ScToken* t;
    pArr->Reset();
    if (bIsName)
        t = pArr->GetNextReference();
    else
        t = pArr->GetNextReferenceOrName();
    while( t )
    {
        if( t->GetOpCode() == ocName )
        {
            if (!bIsName)
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex(t->GetIndex());
                if (pName && pName->HasType(RT_SHAREDMOD))
                    pRangeData = pName;
            }
        }
        else if( t->GetType() != svIndex )  // it may be a DB area!!!
        {
            if ( !(bIsName && t->GetSingleRef().IsTabRel()) )
            {   // of names only adjust absolute references
                SingleRefData& rRef = t->GetSingleRef();
                if ( rRef.IsTabRel() )
                {
                    rRef.nTab = rRef.nRelTab + nOldPosTab;
                    if ( rRef.nTab < 0 )
                        rRef.nTab += pDoc->GetTableCount();  // was a wrap
                }
                if (nTable <= rRef.nTab)
                    ++rRef.nTab;
                rRef.nRelTab = rRef.nTab - nPosTab;
            }
            else
                bIsRel = TRUE;
            if ( t->GetType() == svDoubleRef )
            {
                if ( !(bIsName && t->GetDoubleRef().Ref2.IsTabRel()) )
                {   // of names only adjust absolute references
                    SingleRefData& rRef = t->GetDoubleRef().Ref2;
                    if ( rRef.IsTabRel() )
                    {
                        rRef.nTab = rRef.nRelTab + nOldPosTab;
                        if ( rRef.nTab < 0 )
                            rRef.nTab += pDoc->GetTableCount();  // was a wrap
                    }
                    if (nTable <= rRef.nTab)
                        ++rRef.nTab;
                    rRef.nRelTab = rRef.nTab - nPosTab;
                }
                else
                    bIsRel = TRUE;
            }
            if ( bIsName && bIsRel )
                pRangeData = (ScRangeData*) this;   // not dereferenced in rangenam
        }
        if (bIsName)
            t = pArr->GetNextReference();
        else
            t = pArr->GetNextReferenceOrName();
    }
    if ( !bIsName )
    {
        pArr->Reset();
        for ( t = pArr->GetNextReferenceRPN(); t;
              t = pArr->GetNextReferenceRPN() )
        {
            if ( t->GetRef() == 1 )
            {
                SingleRefData& rRef1 = t->GetSingleRef();
                if ( !(rRef1.IsRelName() && rRef1.IsTabRel()) )
                {   // of names only adjust absolute references
                    if ( rRef1.IsTabRel() )
                    {
                        rRef1.nTab = rRef1.nRelTab + nOldPosTab;
                        if ( rRef1.nTab < 0 )
                            rRef1.nTab += pDoc->GetTableCount();  // was a wrap
                    }
                    if (nTable <= rRef1.nTab)
                        ++rRef1.nTab;
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                }
                if ( t->GetType() == svDoubleRef )
                {
                    SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                    if ( !(rRef2.IsRelName() && rRef2.IsTabRel()) )
                    {   // of names only adjust absolute references
                        if ( rRef2.IsTabRel() )
                        {
                            rRef2.nTab = rRef2.nRelTab + nOldPosTab;
                            if ( rRef2.nTab < 0 )
                                rRef2.nTab += pDoc->GetTableCount();  // was a wrap
                        }
                        if (nTable <= rRef2.nTab)
                            ++rRef2.nTab;
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                }
            }
        }
    }
    return pRangeData;
}

ScRangeData* ScCompiler::UpdateDeleteTab(SCTAB nTable, BOOL bIsMove, BOOL bIsName,
                                 BOOL& rChanged)
{
    ScRangeData* pRangeData = NULL;
    SCTAB nTab, nTab2;
    SCTAB nPosTab = aPos.Tab();          // _after_ decremented!
    SCTAB nOldPosTab = ((nPosTab >= nTable) ? (nPosTab + 1) : nPosTab);
    rChanged = FALSE;
    BOOL bIsRel = FALSE;
    ScToken* t;
    pArr->Reset();
    if (bIsName)
        t = pArr->GetNextReference();
    else
        t = pArr->GetNextReferenceOrName();
    while( t )
    {
        if( t->GetOpCode() == ocName )
        {
            if (!bIsName)
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex(t->GetIndex());
                if (pName && pName->HasType(RT_SHAREDMOD))
                    pRangeData = pName;
            }
            rChanged = TRUE;
        }
        else if( t->GetType() != svIndex )  // it may be a DB area!!!
        {
            if ( !(bIsName && t->GetSingleRef().IsTabRel()) )
            {   // of names only adjust absolute references
                SingleRefData& rRef = t->GetSingleRef();
                if ( rRef.IsTabRel() )
                    nTab = rRef.nTab = rRef.nRelTab + nOldPosTab;
                else
                    nTab = rRef.nTab;
                if ( nTable < nTab )
                {
                    rRef.nTab = nTab - 1;
                    rChanged = TRUE;
                }
                else if ( nTable == nTab )
                {
                    if ( t->GetType() == svDoubleRef )
                    {
                        SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                        if ( rRef2.IsTabRel() )
                            nTab2 = rRef2.nRelTab + nOldPosTab;
                        else
                            nTab2 = rRef2.nTab;
                        if ( nTab == nTab2
                          || (nTab+1) >= pDoc->GetTableCount() )
                        {
                            rRef.nTab = MAXTAB+1;
                            rRef.SetTabDeleted( TRUE );
                        }
                        // else: nTab later points to what's nTable+1 now
                        // => area shrunk
                    }
                    else
                    {
                        rRef.nTab = MAXTAB+1;
                        rRef.SetTabDeleted( TRUE );
                    }
                    rChanged = TRUE;
                }
                rRef.nRelTab = rRef.nTab - nPosTab;
            }
            else
                bIsRel = TRUE;
            if ( t->GetType() == svDoubleRef )
            {
                if ( !(bIsName && t->GetDoubleRef().Ref2.IsTabRel()) )
                {   // of names only adjust absolute references
                    SingleRefData& rRef = t->GetDoubleRef().Ref2;
                    if ( rRef.IsTabRel() )
                        nTab = rRef.nTab = rRef.nRelTab + nOldPosTab;
                    else
                        nTab = rRef.nTab;
                    if ( nTable < nTab )
                    {
                        rRef.nTab = nTab - 1;
                        rChanged = TRUE;
                    }
                    else if ( nTable == nTab )
                    {
                        if ( !t->GetDoubleRef().Ref1.IsTabDeleted() )
                            rRef.nTab = nTab - 1;   // shrink area
                        else
                        {
                            rRef.nTab = MAXTAB+1;
                            rRef.SetTabDeleted( TRUE );
                        }
                        rChanged = TRUE;
                    }
                    rRef.nRelTab = rRef.nTab - nPosTab;
                }
                else
                    bIsRel = TRUE;
            }
            if ( bIsName && bIsRel )
                pRangeData = (ScRangeData*) this;   // not dereferenced in rangenam
        }
        if (bIsName)
            t = pArr->GetNextReference();
        else
            t = pArr->GetNextReferenceOrName();
    }
    if ( !bIsName )
    {
        pArr->Reset();
        for ( t = pArr->GetNextReferenceRPN(); t;
              t = pArr->GetNextReferenceRPN() )
        {
            if ( t->GetRef() == 1 )
            {
                SingleRefData& rRef1 = t->GetSingleRef();
                if ( !(rRef1.IsRelName() && rRef1.IsTabRel()) )
                {   // of names only adjust absolute references
                    if ( rRef1.IsTabRel() )
                        nTab = rRef1.nTab = rRef1.nRelTab + nOldPosTab;
                    else
                        nTab = rRef1.nTab;
                    if ( nTable < nTab )
                    {
                        rRef1.nTab = nTab - 1;
                        rChanged = TRUE;
                    }
                    else if ( nTable == nTab )
                    {
                        if ( t->GetType() == svDoubleRef )
                        {
                            SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                            if ( rRef2.IsTabRel() )
                                nTab2 = rRef2.nRelTab + nOldPosTab;
                            else
                                nTab2 = rRef2.nTab;
                            if ( nTab == nTab2
                              || (nTab+1) >= pDoc->GetTableCount() )
                            {
                                rRef1.nTab = MAXTAB+1;
                                rRef1.SetTabDeleted( TRUE );
                            }
                            // else: nTab later points to what's nTable+1 now
                            // => area shrunk
                        }
                        else
                        {
                            rRef1.nTab = MAXTAB+1;
                            rRef1.SetTabDeleted( TRUE );
                        }
                        rChanged = TRUE;
                    }
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                }
                if ( t->GetType() == svDoubleRef )
                {
                    SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                    if ( !(rRef2.IsRelName() && rRef2.IsTabRel()) )
                    {   // of names only adjust absolute references
                        if ( rRef2.IsTabRel() )
                            nTab = rRef2.nTab = rRef2.nRelTab + nOldPosTab;
                        else
                            nTab = rRef2.nTab;
                        if ( nTable < nTab )
                        {
                            rRef2.nTab = nTab - 1;
                            rChanged = TRUE;
                        }
                        else if ( nTable == nTab )
                        {
                            if ( !rRef1.IsTabDeleted() )
                                rRef2.nTab = nTab - 1;  // shrink area
                            else
                            {
                                rRef2.nTab = MAXTAB+1;
                                rRef2.SetTabDeleted( TRUE );
                            }
                            rChanged = TRUE;
                        }
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                }
            }
        }
    }
    return pRangeData;
}

// aPos.Tab() must be already adjusted!
ScRangeData* ScCompiler::UpdateMoveTab( SCTAB nOldTab, SCTAB nNewTab,
        BOOL bIsName )
{
    ScRangeData* pRangeData = NULL;
    SCsTAB nTab;

    SCTAB nStart, nEnd;
    short nDir;                         // direction in which others move
    if ( nOldTab < nNewTab )
    {
        nDir = -1;
        nStart = nOldTab;
        nEnd = nNewTab;
    }
    else
    {
        nDir = 1;
        nStart = nNewTab;
        nEnd = nOldTab;
    }
    SCTAB nPosTab = aPos.Tab();        // current sheet
    SCTAB nOldPosTab;                  // previously it was this one
    if ( nPosTab == nNewTab )
        nOldPosTab = nOldTab;           // look, it's me!
    else if ( nPosTab < nStart || nEnd < nPosTab )
        nOldPosTab = nPosTab;           // wasn't moved
    else
        nOldPosTab = nPosTab - nDir;    // moved by one

    BOOL bIsRel = FALSE;
    ScToken* t;
    pArr->Reset();
    if (bIsName)
        t = pArr->GetNextReference();
    else
        t = pArr->GetNextReferenceOrName();
    while( t )
    {
        if( t->GetOpCode() == ocName )
        {
            if (!bIsName)
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex(t->GetIndex());
                if (pName && pName->HasType(RT_SHAREDMOD))
                    pRangeData = pName;
            }
        }
        else if( t->GetType() != svIndex )  // it may be a DB area!!!
        {
            SingleRefData& rRef1 = t->GetSingleRef();
            if ( !(bIsName && rRef1.IsTabRel()) )
            {   // of names only adjust absolute references
                if ( rRef1.IsTabRel() )
                    nTab = rRef1.nRelTab + nOldPosTab;
                else
                    nTab = rRef1.nTab;
                if ( nTab == nOldTab )
                    rRef1.nTab = nNewTab;
                else if ( nStart <= nTab && nTab <= nEnd )
                    rRef1.nTab = nTab + nDir;
                rRef1.nRelTab = rRef1.nTab - nPosTab;
            }
            else
                bIsRel = TRUE;
            if ( t->GetType() == svDoubleRef )
            {
                SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                if ( !(bIsName && rRef2.IsTabRel()) )
                {   // of names only adjust absolute references
                    if ( rRef2.IsTabRel() )
                        nTab = rRef2.nRelTab + nOldPosTab;
                    else
                        nTab = rRef2.nTab;
                    if ( nTab == nOldTab )
                        rRef2.nTab = nNewTab;
                    else if ( nStart <= nTab && nTab <= nEnd )
                        rRef2.nTab = nTab + nDir;
                    rRef2.nRelTab = rRef2.nTab - nPosTab;
                }
                else
                    bIsRel = TRUE;
                SCsTAB nTab1, nTab2;
                if ( rRef1.IsTabRel() )
                    nTab1 = rRef1.nRelTab + nPosTab;
                else
                    nTab1 = rRef1.nTab;
                if ( rRef2.IsTabRel() )
                    nTab2 = rRef2.nRelTab + nPosTab;
                else
                    nTab2 = rRef1.nTab;
                if ( nTab2 < nTab1 )
                {   // PutInOrder
                    rRef1.nTab = nTab2;
                    rRef2.nTab = nTab1;
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                    rRef2.nRelTab = rRef2.nTab - nPosTab;
                }
            }
            if ( bIsName && bIsRel )
                pRangeData = (ScRangeData*) this;   // not dereferenced in rangenam
        }
        if (bIsName)
            t = pArr->GetNextReference();
        else
            t = pArr->GetNextReferenceOrName();
    }
    if ( !bIsName )
    {
        SCsTAB nMaxTabMod = (SCsTAB) pDoc->GetTableCount();
        pArr->Reset();
        for ( t = pArr->GetNextReferenceRPN(); t;
              t = pArr->GetNextReferenceRPN() )
        {
            if ( t->GetRef() == 1 )
            {
                SingleRefData& rRef1 = t->GetSingleRef();
                if ( rRef1.IsRelName() && rRef1.IsTabRel() )
                {   // possibly wrap RelName, like lcl_MoveItWrap in refupdat.cxx
                    nTab = rRef1.nRelTab + nPosTab;
                    if ( nTab < 0 )
                        nTab += nMaxTabMod;
                    else if ( nTab > nMaxTab )
                        nTab -= nMaxTabMod;
                    rRef1.nRelTab = nTab - nPosTab;
                }
                else
                {
                    if ( rRef1.IsTabRel() )
                        nTab = rRef1.nRelTab + nOldPosTab;
                    else
                        nTab = rRef1.nTab;
                    if ( nTab == nOldTab )
                        rRef1.nTab = nNewTab;
                    else if ( nStart <= nTab && nTab <= nEnd )
                        rRef1.nTab = nTab + nDir;
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                }
                if( t->GetType() == svDoubleRef )
                {
                    SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                    if ( rRef2.IsRelName() && rRef2.IsTabRel() )
                    {   // possibly wrap RelName, like lcl_MoveItWrap in refupdat.cxx
                        nTab = rRef2.nRelTab + nPosTab;
                        if ( nTab < 0 )
                            nTab += nMaxTabMod;
                        else if ( nTab > nMaxTab )
                            nTab -= nMaxTabMod;
                        rRef2.nRelTab = nTab - nPosTab;
                    }
                    else
                    {
                        if ( rRef2.IsTabRel() )
                            nTab = rRef2.nRelTab + nOldPosTab;
                        else
                            nTab = rRef2.nTab;
                        if ( nTab == nOldTab )
                            rRef2.nTab = nNewTab;
                        else if ( nStart <= nTab && nTab <= nEnd )
                            rRef2.nTab = nTab + nDir;
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                    SCsTAB nTab1, nTab2;
                    if ( rRef1.IsTabRel() )
                        nTab1 = rRef1.nRelTab + nPosTab;
                    else
                        nTab1 = rRef1.nTab;
                    if ( rRef2.IsTabRel() )
                        nTab2 = rRef2.nRelTab + nPosTab;
                    else
                        nTab2 = rRef1.nTab;
                    if ( nTab2 < nTab1 )
                    {   // PutInOrder
                        rRef1.nTab = nTab2;
                        rRef2.nTab = nTab1;
                        rRef1.nRelTab = rRef1.nTab - nPosTab;
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                }
            }
        }
    }
    return pRangeData;
}


const String& ScCompiler::GetStringFromOpCode( OpCode eOpCode, bool bEnglish )
{
    if( static_cast< USHORT >( eOpCode ) < nAnzStrings )
        return bEnglish ? pSymbolTableEnglish[ eOpCode ] : pSymbolTableNative[ eOpCode ];
    return EMPTY_STRING;
}

ScToken* ScCompiler::CreateStringFromToken( String& rFormula, ScToken* pToken,
        BOOL bAllowArrAdvance )
{
    rtl::OUStringBuffer aBuffer;
    ScToken* p = CreateStringFromToken( aBuffer, pToken, bAllowArrAdvance );
    rFormula += aBuffer;
    return p;
}

ScToken* ScCompiler::CreateStringFromToken( rtl::OUStringBuffer& rBuffer, ScToken* pToken,
        BOOL bAllowArrAdvance )
{
    BOOL bNext = TRUE;
    BOOL bSpaces = FALSE;
    ScToken* t = pToken;
    OpCode eOp = t->GetOpCode();
    if( eOp >= ocAnd && eOp <= ocOr )
    {
        // AND, OR infix?
        if ( bAllowArrAdvance )
            t = pArr->Next();
        else
            t = pArr->PeekNext();
        bNext = FALSE;
        bSpaces = ( !t || t->GetOpCode() != ocOpen );
    }
    if( bSpaces )
        rBuffer.append(sal_Unicode(' '));

    if( eOp == ocSpaces )
    {   // most times it's just one blank
        BYTE n = t->GetByte();
        for ( BYTE j=0; j<n; ++j )
        {
            rBuffer.append(sal_Unicode(' '));
        }
    }
    else if( eOp >= ocInternalBegin && eOp <= ocInternalEnd )
        rBuffer.appendAscii( pInternal[ eOp - ocInternalBegin ] );
    else if( (USHORT) eOp < nAnzStrings)        // Keyword:
        rBuffer.append(pSymbolTable[eOp]);
    else
    {
        DBG_ERROR("Unbekannter OpCode");
        rBuffer.append(ScGlobal::GetRscString(STR_NO_NAME_REF));
    }
    if( bNext ) switch( t->GetType() )
    {
        case svDouble:
        {
            if ( pSymbolTable == pSymbolTableEnglish )
            {   // Don't go via number formatter, slows down XML export
                // significantly because on every formula the number formatter
                // has to switch to/from English/native language.
                ::rtl::math::doubleToUStringBuffer( rBuffer, t->GetDouble(),
                        rtl_math_StringFormat_Automatic,
                        rtl_math_DecimalPlaces_Max, '.', TRUE );
            }
            else
            {
                ::rtl::math::doubleToUStringBuffer( rBuffer, t->GetDouble(),
                        rtl_math_StringFormat_Automatic,
                        rtl_math_DecimalPlaces_Max,
                        ScGlobal::pLocaleData->getNumDecimalSep().GetChar(0),
                        TRUE );
            }
        }
        break;
        case svString:
            if( eOp == ocBad )
                rBuffer.append(t->GetString());
            else
            {
                if (bImportXML)
                    rBuffer.append(t->GetString());
                else
                {
                    rBuffer.append(sal_Unicode('"'));
                    if ( ScGlobal::UnicodeStrChr( t->GetString().GetBuffer(), '"' ) == NULL )
                        rBuffer.append(t->GetString());
                    else
                    {
                        String aStr( t->GetString() );
                        xub_StrLen nPos = 0;
                        while ( (nPos = aStr.Search( '"', nPos)) != STRING_NOTFOUND )
                        {
                            aStr.Insert( '"', nPos );
                            nPos += 2;
                        }
                        rBuffer.append(aStr);
                    }
                    rBuffer.append(sal_Unicode('"'));
                }
            }
            break;
        case svSingleRef:
        {
            SingleRefData& rRef = t->GetSingleRef();
            ComplRefData aRef;
            aRef.Ref1 = aRef.Ref2 = rRef;
            if ( eOp == ocColRowName )
            {
                rRef.CalcAbsIfRel( aPos );
                if ( pDoc->HasStringData( rRef.nCol, rRef.nRow, rRef.nTab ) )
                {
                    String aStr;
                    pDoc->GetString( rRef.nCol, rRef.nRow, rRef.nTab, aStr );
                    EnQuote( aStr );
                    rBuffer.append(aStr);
                }
                else
                {
                    rBuffer.append(ScGlobal::GetRscString(STR_NO_NAME_REF));
                    MakeRefStr( rBuffer, aRef, TRUE );
                }
            }
            else
                MakeRefStr( rBuffer, aRef, TRUE );
        }
            break;
        case svDoubleRef:
            MakeRefStr( rBuffer, t->GetDoubleRef(), FALSE );
            break;
        case svIndex:
        {
            rtl::OUStringBuffer aBuffer;
            switch ( eOp )
            {
                case ocName:
                {
                    ScRangeData* pData = pDoc->GetRangeName()->FindIndex(t->GetIndex());
                    if (pData)
                    {
                        if (pData->HasType(RT_SHARED))
                            pData->UpdateSymbol( aBuffer, aPos,
                                        pSymbolTable == pSymbolTableEnglish,
                                        bCompileXML );
                        else
                            aBuffer.append(pData->GetName());
                    }
                }
                break;
                case ocDBArea:
                {
                    ScDBData* pDBData = pDoc->GetDBCollection()->FindIndex(t->GetIndex());
                    if (pDBData)
                        aBuffer.append(pDBData->GetName());
                }
                break;
                default:
                    ;   // nothing
            }
            if ( aBuffer.getLength() )
                rBuffer.append(aBuffer);
            else
                rBuffer.append(ScGlobal::GetRscString(STR_NO_NAME_REF));
            break;
        }
        case svExternal:
        {
            //  show translated name of StarOne AddIns
            String aAddIn( t->GetExternal() );
            if ( pSymbolTable != pSymbolTableEnglish )
                ScGlobal::GetAddInCollection()->LocalizeString( aAddIn );
            rBuffer.append(aAddIn);
        }
            break;
        case svByte:
        case svJump:
        case svFAP:
        case svMissing:
            break;      // Opcodes
        default:
            DBG_ERROR("ScCompiler:: GetStringFromToken errUnknownVariable");
    }                                           // of switch
    if( bSpaces )
        rBuffer.append(sal_Unicode(' '));
    if ( bAllowArrAdvance )
    {
        if( bNext )
            t = pArr->Next();
        return t;
    }
    return pToken;
}

void ScCompiler::CreateStringFromTokenArray( String& rFormula )
{
    rtl::OUStringBuffer aBuffer( pArr->GetLen() * 2 );
    CreateStringFromTokenArray( aBuffer );
    rFormula = aBuffer;
}

void ScCompiler::CreateStringFromTokenArray( rtl::OUStringBuffer& rBuffer )
{
    rBuffer.setLength(0);
    if( !pArr->GetLen() )
        return;

    // at least one char per token, plus some are references, some are function names
    rBuffer.ensureCapacity( pArr->GetLen() * 2 );

    if ( pArr->IsRecalcModeForced() )
        rBuffer.append(sal_Unicode('='));
    ScToken* t = pArr->First();
    while( t )
        t = CreateStringFromToken( rBuffer, t, TRUE );
}

BOOL ScCompiler::EnQuote( String& rStr )
{
    sal_Int32 nType = ScGlobal::pCharClass->getStringType( rStr, 0, rStr.Len() );
    if ( !CharClass::isNumericType( nType )
            && CharClass::isAlphaNumericType( nType ) )
        return FALSE;
    xub_StrLen nPos = 0;
    while ( (nPos = rStr.Search( '\'', nPos)) != STRING_NOTFOUND )
    {
        rStr.Insert( '\\', nPos );
        nPos += 2;
    }
    rStr.Insert( '\'', 0 );
    rStr += '\'';
    return TRUE;
}


BOOL ScCompiler::DeQuote( String& rStr )
{
    xub_StrLen nLen = rStr.Len();
    if ( nLen > 1 && rStr.GetChar(0) == '\'' && rStr.GetChar( nLen-1 ) == '\'' )
    {
        rStr.Erase( nLen-1, 1 );
        rStr.Erase( 0, 1 );
        xub_StrLen nPos = 0;
        while ( (nPos = rStr.SearchAscii( "\\\'", nPos)) != STRING_NOTFOUND )
        {
            rStr.Erase( nPos, 1 );
            ++nPos;
        }
        return TRUE;
    }
    return FALSE;
}


String* GetScCompilerpSymbolTableNative() //CHINA001
{
    return ScCompiler::pSymbolTableNative;
}
