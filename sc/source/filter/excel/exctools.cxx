/*************************************************************************
 *
 *  $RCSfile: exctools.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: dr $ $Date: 2000-12-18 14:25:25 $
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

//------------------------------------------------------------------------

#include "scitems.hxx"
#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD

#include <svx/editdata.hxx>
#include <svx/editeng.hxx>
#include <svx/editobj.hxx>
#include <svx/editstat.hxx>
#include <vcl/color.hxx>
#include <so3/svstor.hxx>

#include <string.h>
#include <math.h>

#include "document.hxx"
#include "patattr.hxx"
#include "attrib.hxx"
#include "globstr.hrc"
#include "scextopt.hxx"
#include "olinetab.hxx"
#include "progress.hxx"
#include "editutil.hxx"

#include "imp_op.hxx"
#include "root.hxx"
#include "xfbuff.hxx"
#include "vfbuff.hxx"
#include "fontbuff.hxx"
#include "excimp8.hxx"
#include "scerrors.hxx"
#include "otlnbuff.hxx"
#include "fltprgrs.hxx"
#include "excsst.hxx"
#include "flttools.hxx"
#include "spstring.hxx"
#include "excrecds.hxx"
#include "xcl97rec.hxx"

#ifndef _SC_XCLIMPPIVOTTABLES_HXX
#include "XclImpPivotTables.hxx"
#endif
#ifndef _SC_XCLEXPPIVOTTABLES_HXX
#include "XclExpPivotTables.hxx"
#endif

using namespace com::sun::star;

extern double ReadLongDouble( SvStream& rStr );

// - ALLGEMEINE ----------------------------------------------------------

#ifdef _MSC_VER
#if _MSC_VER <= 800
#undef __SIMPLE_FUNC
#define __SIMPLE_FUNC
#endif
#endif

double ReadLongDouble( SvStream& rStr )

#ifdef __SIMPLE_FUNC
// ========== nur fuer VC 1.5 oder kleiner ===============================
{
    long double fRet;
    rStr.Read( &fRet, 10 );
    return ( double ) fRet;
}
#undef __SIMPLE_FUNC
#else
// ========== ausfuehrliche Version fuer'n Rest ==========================
{
    register long double    lfDouble = 0.0;
    register long double    lfFakt = 256.0;
    BYTE                    pDouble10[ 10 ];

    rStr.Read( pDouble10, 10 );         // Intel-10 in pDouble10

    lfDouble = ( long double ) pDouble10[ 7 ];  // Byte 7
    lfDouble *= lfFakt;                         // Byte 6
    lfDouble += ( long double ) pDouble10[ 6 ];
    lfDouble *= lfFakt;                         // Byte 5
    lfDouble += ( long double ) pDouble10[ 5 ];
    lfDouble *= lfFakt;                         // Byte 4
    lfDouble += ( long double ) pDouble10[ 4 ];
    lfDouble *= lfFakt;                         // Byte 3
    lfDouble += ( long double ) pDouble10[ 3 ];
    lfDouble *= lfFakt;                         // Byte 2
    lfDouble += ( long double ) pDouble10[ 2 ];
    lfDouble *= lfFakt;                         // Byte 1
    lfDouble += ( long double ) pDouble10[ 1 ];
    lfDouble *= lfFakt;                         // Byte 0
    lfDouble += ( long double ) pDouble10[ 0 ];

    //  Bei 0.0 sind alle Bits 0, und pow(2.0,-16446) kommt mit dem
    //  CSet Compiler nicht so gut
    if( lfDouble != 0.0 )
    {
        // Exponent
        register INT32          nExp;
        nExp = ( pDouble10[ 9 ] & 0x7F );
        nExp <<= 8;
        nExp += pDouble10[ 8 ];
        nExp -= 16446;

        lfDouble *= pow( 2.0, ( double ) nExp );
    }

    // Sign
    if( pDouble10[ 9 ] & 0x80 )
        lfDouble *= ( long double ) -1.0;

    return ( double ) lfDouble;
/*

" M a p p i n g - G u i d e "

10-Byte Intel

77777777 77666666 66665555 55555544 44444444 33333333 33222222 22221111 1111110  00000000       x10
98765432 10987654 32109876 54321098 76543210 98765432 10987654 32109876 54321098 76543210   Bit-# insgesamt
9      9 8      8 7      7 6      6 5      5 4      4 3      3 2      2 1      1 0      0   Byte-#
76543210 76543210 76543210 76543210 76543210 76543210 76543210 76543210 76543210 76543210   Bit-# im Byte
SEEEEEEE EEEEEEEE IMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM   Gruppe
01111110 00000000 06665555 55555544 44444444 33333333 33222222 22221111 11111100 00000000       x10
14321098 76543210 02109876 54321098 76543210 98765432 10987654 32109876 54321098 76543210   Bit in Gruppe
*/
}

#endif


ScEditEngineDefaulter& RootData::GetEdEng( void )
{
    if( !pEdEng )
    {
        ScEditEngineDefaulter* p = new ScEditEngineDefaulter( pDoc->GetEnginePool() );

        p->SetRefMapMode( MAP_100TH_MM );
        p->SetEditTextObjectPool( pDoc->GetEditPool() );
        p->SetUpdateMode( FALSE );
        p->EnableUndo( FALSE );
        p->SetControlWord( ( p->GetControlWord() & ~EE_CNTRL_ALLOWBIGOBJS ) );

        pEdEng = p;
    }

    return *pEdEng;
}


ScEditEngineDefaulter& RootData::GetEdEngForHF( void )
{
    if( !pEdEngHF )
    {
        //  can't use document's edit engine pool here,
        //  because pool must have twips as default metric
        ScEditEngineDefaulter* p = new ScHeaderEditEngine( EditEngine::CreatePool(), TRUE );

        p->SetRefMapMode( MAP_TWIP );       // headers/footers are in twips
        p->SetUpdateMode( FALSE );
        p->EnableUndo( FALSE );
        p->SetControlWord( ( p->GetControlWord() & ~EE_CNTRL_ALLOWBIGOBJS ) );

        pEdEngHF = p;
    }

    return *pEdEngHF;
}


String RootData::GetCondFormStyleName( const UINT16 n )
{
    if( n <= nLastCond )
        nCondRangeCnt++;

    String  t( RTL_CONSTASCII_STRINGPARAM( "CndFrmStyl_" ) );

    t += UniString::CreateFromInt32( nCondRangeCnt );
    t.AppendAscii( "_" );
    t += UniString::CreateFromInt32( n );

    nLastCond = n;

    return t;
}


RootData::RootData( void )
{
    fColScale = fRowScale = 1.0;
    pDoc = NULL;
    pFormTable = NULL;
    pScRangeName = NULL;
    pColor = NULL;
    pXF_Buffer = NULL;
    pFontBuffer = NULL;
    eDefLanguage = ScGlobal::eLnge;
    eDateiTyp = eHauptDateiTyp = eGlobalDateiTyp = BiffX;
    pExtSheetBuff = NULL;
    pTabNameBuff = NULL;
    pRNameBuff = NULL;
    pShrfmlaBuff = NULL;
    pExtNameBuff = NULL;
    pAktTab = NULL;
    pCharset = NULL;
    pExtDocOpt = NULL;
    pProgress = NULL;

    pEdEng = pEdEngHF = NULL;

    bCellCut = FALSE;
    bBreakSharedFormula = TRUE;
    bDefaultPage = bChartTab = FALSE;

    pXtiBuffer = NULL;
    pSupbookBuffer = NULL;
    pCurrSupbook = NULL;
    pImpTabIdBuffer = NULL;

    pRootStorage = pPivotCacheStorage = /*pCtrlStorage = */NULL;
    pImpPivotCacheList = NULL;

    nCondRangeCnt = 0;
    nLastCond = 0;

    pRootStorage = NULL;
    pTabBuffer = NULL;
    pTabId = NULL;
    pUserBViewList = NULL;
    pNameList = NULL;
    pPalette2 = NULL;
    pFontRecs = NULL;
    pExtSheetCntAndRecs = NULL;
    nRowMax = 0;
    nRangeNameIndex = 1;

    pSstRecs = NULL;
    pExternsheetRecs = NULL;
    pObjRecs = NULL;
    pNoteRecs = NULL;
    pEscher = NULL;

    pPivotCacheList = NULL;

    bWriteVBAStorage = FALSE;
    nCodenames = 0;

    pStyleSheet = NULL;
    pStyleSheetItemSet = NULL;

    pLastHlink = NULL;
    bStoreRel = FALSE;
    pBasePath = NULL;
}


RootData::~RootData()
{
    delete pFontBuffer;
    delete pColor;
    delete pExtSheetBuff;
    delete pTabNameBuff;
    delete pRNameBuff;
    delete pShrfmlaBuff;
    delete pExtNameBuff;
    delete pXF_Buffer;

    if( pTabBuffer )
        delete pTabBuffer;
    if( pEdEng )
        delete pEdEng;
    if( pEdEngHF )
        delete pEdEngHF;
    if( pExtSheetCntAndRecs )
        delete pExtSheetCntAndRecs;

    if( pXtiBuffer )
        delete pXtiBuffer;
    if( pSupbookBuffer )
        delete pSupbookBuffer;
    if( pImpTabIdBuffer )
        delete pImpTabIdBuffer;

    if( pImpPivotCacheList )
        delete pImpPivotCacheList;
    if( pPivotCacheList )
        delete pPivotCacheList;

    if( pLastHlink )
        delete pLastHlink;
    if( pBasePath )
        delete pBasePath;

    if( pExtDocOpt )
        delete pExtDocOpt;

//  if( pCtrlStorage )
//      delete pCtrlStorage;
}




ImportTyp::ImportTyp( SvStream& aStream, ScDocument* pDoc, CharSet eQ ): aIn( aStream )
{
    eQuellChar = eQ;
    pD = pDoc;

    pExtOpt = NULL;
}


ImportTyp::~ImportTyp()
{
    ScExtDocOptions*    p = pD->GetExtDocOptions();
    if( p )
    {
        if( pExtOpt )
        {
            *p = *pExtOpt;
            delete pExtOpt;
        }
    }
    else
        pD->SetExtDocOptions( pExtOpt );
}


FltError ImportTyp::Read()
{
    return eERR_INTERN;
}


ScExtDocOptions &ImportTyp::GetExtOpt( void )
{
    if( !pExtOpt )
    {
        pExtOpt = new ScExtDocOptions;
        ScExtDocOptions*    pOrg = pD->GetExtDocOptions();
        if( pOrg )
            *pExtOpt = *pOrg;
    }
    return *pExtOpt;
}




OutlineBuffer::OutlineBuffer( USHORT nNewSize )
{
    DBG_ASSERT( nNewSize > 0, "-OutlineBuffer::Ctor: nNewSize == 0!" );

    nSize = nNewSize + 1;
    pLevel = new BYTE[ nSize ];
    pOuted = new BOOL[ nSize ];
    pHidden = new BOOL[ nSize ];
    pOutlineArray = NULL;

    Reset();
}


OutlineBuffer::~OutlineBuffer()
{
    delete[] pLevel;
    delete[] pOuted;
    delete[] pHidden;
}


void OutlineBuffer::SetLevel( USHORT nIndex, BYTE nVal, BOOL bOuted, BOOL bHidden )
{
    if( nIndex < nSize )
    {
        pLevel[ nIndex ] = nVal;
        pOuted[ nIndex ] = bOuted;
        pHidden[ nIndex ] = bHidden;

        if( nIndex > nLast )
            nLast = nIndex;
        if( nVal > nMaxLevel )
            nMaxLevel = nVal;
    }
}


void OutlineBuffer::SetOuted( USHORT nIndex )
{
    if( nIndex < nSize )
    {
        pOuted[ nIndex ] = TRUE;
        if( nIndex > nLast )
            nLast = nIndex;
    }
}


void OutlineBuffer::SetOuted( USHORT nIndex, BOOL bOuted )
{
    if( nIndex < nSize )
    {
        pOuted[ nIndex ] = bOuted;
        if( nIndex > nLast )
            nLast = nIndex;
    }
}


void OutlineBuffer::SetOutlineArray( ScOutlineArray* pOArray )
{
    pOutlineArray = pOArray;
}


// transtorm xcl-outline into SC-outline
void OutlineBuffer::MakeScOutline( void )
{
    if( !pOutlineArray || !HasOutline() )
        return;

    const UINT16    nNumLev         = 8;
    BOOL            bPreOutedLevel  = FALSE;
    BYTE            nCurrLevel      = 0;
    UINT16          nC;
    BOOL            bMakeHidden[ nNumLev ];
    BOOL            bMakeVisible[ nNumLev + 1 ];

    for( nC = 0; nC < nNumLev; nC++ )
        bMakeHidden[ nC ] = FALSE;
    for( nC = 0; nC <= nNumLev; nC++ )
        bMakeVisible[ nC ] = TRUE;
    if( nLast < (nSize - 1) )
        nLast++;

    // search for hidden attributes at end of level, move them to begin
    if( bButtonNormal )
    {
        for( BYTE nWorkLevel = 1; nWorkLevel <= nMaxLevel; nWorkLevel++ )
        {
            UINT16  nStartPos;
            BYTE    nCurrLevel  = 0;
            BYTE    nPrevLevel  = 0;

            for( UINT16 nC = 0 ; nC <= nLast ; nC++ )
            {
                nPrevLevel = nCurrLevel;
                nCurrLevel = pLevel[ nC ];
                if( (nPrevLevel < nWorkLevel) && (nCurrLevel >= nWorkLevel) )
                    nStartPos = nC;
                else if( (nPrevLevel >= nWorkLevel) && (nCurrLevel < nWorkLevel) )
                {
                    if( pOuted[ nC ] && pHidden[ nStartPos ] )
                    {
                        if( nStartPos )
                            pOuted[ nStartPos - 1 ] = TRUE;
                        else
                            bPreOutedLevel = TRUE;
                        pOuted[ nC ] = FALSE;
                    }
                }
            }
        }
    }
    else
        bPreOutedLevel = pHidden[ 0 ];

    // generate SC outlines
    UINT16  nPrevC;
    UINT16  nStart[ nNumLev ];
    BOOL    bDummy;
    BOOL    bPrevOuted  = bPreOutedLevel;
    BOOL    bCurrHidden = FALSE;
    BOOL    bPrevHidden = FALSE;

    for( nC = 0; nC <= nLast; nC++ )
    {
        BYTE nWorkLevel = pLevel[ nC ];

        nPrevC      = nC ? nC - 1 : 0;
        bPrevHidden = bCurrHidden;
        bCurrHidden = pHidden[ nC ];

        // open new levels
        while( nWorkLevel > nCurrLevel )
        {
            nCurrLevel++;
            bMakeHidden[ nCurrLevel ] = bPrevOuted;
            bMakeVisible[ nCurrLevel + 1 ] =
                bMakeVisible[ nCurrLevel ] && !bMakeHidden[ nCurrLevel ];
            nStart[ nCurrLevel ] = nC;
        }
        // close levels
        while( nWorkLevel < nCurrLevel )
        {
            BOOL bLastLevel     = (nWorkLevel == (nCurrLevel - 1));
            BOOL bRealHidden    = (bMakeHidden[ nCurrLevel ] && bPrevHidden );
            BOOL bRealVisible   = (bMakeVisible[ nCurrLevel ] ||
                                    (!bCurrHidden && bLastLevel));

            pOutlineArray->Insert( nStart[ nCurrLevel ], nPrevC , bDummy,
                bRealHidden, bRealVisible );
            nCurrLevel--;
        }

        bPrevOuted = pOuted[ nC ];
    }
}


void OutlineBuffer::SetLevelRange( USHORT nF, USHORT nL, BYTE nVal,
                                    BOOL bOuted, BOOL bHidden )
{
    DBG_ASSERT( nF <= nL, "+OutlineBuffer::SetLevelRange(): Last < First!" );

    if( nL < nSize )
    {
        if( nL > nLast )
            nLast = nL;

        BYTE*   pLevelCount;
        BYTE*   pLast;
        BOOL*   pOutedCount;
        BOOL*   pHiddenCount;

        pLevelCount = &pLevel[ nF ];
        pLast = &pLevel[ nL ];
        pOutedCount = &pOuted[ nF ];
        pHiddenCount = &pHidden[ nF ];

        while( pLevelCount <= pLast )
        {
            *( pLevelCount++ ) = nVal;
            *( pOutedCount++ ) = bOuted;
            *( pHiddenCount++ ) = bHidden;
        }

        if( nVal > nMaxLevel )
            nMaxLevel = nVal;
    }
}


void OutlineBuffer::Reset( void )
{
    for( USHORT nC = 0 ; nC < nSize ; nC++  )
    {
        pLevel[ nC ] = 0;
        pOuted[ nC ] = pHidden[ nC ] = FALSE;
    }
    nLast = 0;
    nMaxLevel = 0;
}


void OutlineBuffer::Reset( ScOutlineArray *pOArray )
{
    for( USHORT nC = 0 ; nC < nSize ; nC++  )
    {
        pLevel[ nC ] = 0;
        pOuted[ nC ] = pHidden[ nC ] = FALSE;
    }
    pOutlineArray = pOArray;
}




INT32   FilterProgressBar::nInstances = 0;


FilterProgressBar::FilterProgressBar( SvStream& rStr ) : pStr( &rStr )
{
    nInstances++;

    const UINT32    nCurPos = rStr.Tell();

    UINT32          nEndPos = rStr.Seek( STREAM_SEEK_TO_END );
    nCnt = 0;

    if( nInstances == 1 )
        pPrgrs = new ScProgress( NULL, ScGlobal::GetRscString( STR_LOAD_DOC ), nEndPos - nCurPos );
    else
        pPrgrs = NULL;

    if( pPrgrs )
        pPrgrs->SetState( nCurPos );

    rStr.Seek( nCurPos );
}


FilterProgressBar::~FilterProgressBar()
{
    nInstances--;

    if( pPrgrs )
        delete pPrgrs;
}


void FilterProgressBar::Progress( void )
{
    if( pPrgrs )
    {
        if( pStr )
            pPrgrs->SetState( pStr->Tell() );
        else
        {
            nCnt++;
            pPrgrs->SetState( nCnt );
        }
    }
}


UINT32 FilterProgressBar::GetStreamLen( void ) const
{
    if( pStr )
    {
        const UINT32    nCurPos = pStr->Tell();

        UINT32          nEndPos = pStr->Seek( STREAM_SEEK_TO_END );

        pStr->Seek( nCurPos );

        return nEndPos;
    }
    else
        return 0;
}


void FilterProgressBar::StartPostLoadProgress( const UINT32 nObj )
{
    if( pPrgrs )
    {
        pStr = NULL;
        nCnt = 0;

        delete pPrgrs;
        if( nObj )
        {
            pPrgrs = new ScProgress( NULL,
                    ScGlobal::GetRscString( STR_PROGRESS_CALCULATING ), nObj );
        }
        else
            pPrgrs = NULL;

    }
}




ShStrTabEntry::ShStrTabEntry( const String& r ) : aString( r )
{
}


ShStrTabEntry::~ShStrTabEntry()
{
}


BOOL ShStrTabEntry::HasFormats( void ) const
{
    return FALSE;
}


EditTextObject* ShStrTabEntry::CreateEditTextObject( ScEditEngineDefaulter&, FontBuffer& ) const
{
    return NULL;
}




struct ShStrTabFormData
{
    EditTextObject*     pEdTxtObj;
    UINT16*             pForms;
    UINT16              nFormCnt;

                        ShStrTabFormData( SvStream& rIn, INT32& rBytesLeft, UINT16 nFormCnt );
                            // wenn Ctor fehlschlaegt, ist pForms NULL!
                        ~ShStrTabFormData();
};


ShStrTabFormData::ShStrTabFormData( SvStream& r, INT32& rBytesLeft, UINT16 n )
{
    pEdTxtObj = NULL;
    if( n )
    {
        pForms = new UINT16[ n * 2 ];
        nFormCnt = n;

        rBytesLeft -= n * 4;
        UINT16*     p = pForms;

        while( n )
        {
            r >> *p;
            p++;
            r >> *p;
            p++;
            n--;
        }
    }
    else
        pForms = NULL;
}


ShStrTabFormData::~ShStrTabFormData()
{
    if( pEdTxtObj )
        delete pEdTxtObj;

    if( pForms )
        delete[] pForms;
}




ShStrTabFormEntry::ShStrTabFormEntry( const String& r, SvStream& rIn, INT32& rBytesLeft, const UINT16 nFormCnt ) :
    ShStrTabEntry( r )
{
    pData = new ShStrTabFormData( rIn, rBytesLeft, nFormCnt );
    if( !pData->pForms )
    {
        delete pData;
        pData = NULL;
    }
}


ShStrTabFormEntry::~ShStrTabFormEntry()
{
    if( pData )
        delete pData;
}


BOOL ShStrTabFormEntry::HasFormats( void ) const
{
    return pData && pData->pForms;
}




#define READFORM()      nChar = *pRead; pRead++; nFont = *pRead; pRead++; nAnzFrms--;


EditTextObject* ShStrTabFormEntry::CreateEditTextObject( ScEditEngineDefaulter& rEdEng, FontBuffer& rFB ) const
{
    if( !pData || !pData->pForms )
        return NULL;

    if( !pData->pEdTxtObj )
    {
        rEdEng.SetText( aString );

        SfxItemSet          aItemSet( rEdEng.GetEmptyItemSet() );

        UINT16              nChar, nFont;
        const sal_Unicode*  pAktChar = aString.GetBuffer();
        sal_Unicode         cAkt = *pAktChar;
        UINT16              nCnt = 0;
        const UINT16*       pRead = pData->pForms;
        UINT32              nAnzFrms = pData->nFormCnt;

        READFORM();

        ESelection          aSel( 0, 0 );

        while( cAkt )
        {
            if( nCnt >= nChar )
            {// neuer Item-Set
                rEdEng.QuickSetAttribs( aItemSet, aSel );

                aItemSet.ClearItem( 0 );

                rFB.Fill( nFont, aItemSet, FALSE );
                if( nAnzFrms )
                {
                    READFORM();
                }
                else
                    nChar = 0xFFFF;

                aSel.nStartPara = aSel.nEndPara;
                aSel.nStartPos = aSel.nEndPos;
            }

            if( cAkt == '\n' )
            {// new Paragraph
                aSel.nEndPara++;
                aSel.nEndPos = 0;
            }
            else
                aSel.nEndPos++;

            pAktChar++;
            cAkt = *pAktChar;
            nCnt++;
        }

        // letzten ItemSet setzten
        rEdEng.QuickSetAttribs( aItemSet, aSel );

        pData->pEdTxtObj = rEdEng.CreateTextObject();
    }

    return pData->pEdTxtObj->Clone();
}




SharedStringTable::SharedStringTable( void )
{
}


SharedStringTable::~SharedStringTable()
{
    Clear();
}


void SharedStringTable::Append( ShStrTabEntry* p )
{
    List::Insert( p, LIST_APPEND );
}


void SharedStringTable::Clear( void )
{
    ShStrTabEntry*      p = ( ShStrTabEntry* ) List::First();

    while( p )
    {
        delete p;
        p = ( ShStrTabEntry* ) List::Next();
    }

    List::Clear();
}




CharSet GetSystemCharSet( void )
{
    return gsl_getSystemTextEncoding();
}



ShStrTabEntry* CreateUnicodeEntry( SvStream& rIn, INT32& nLeft, CharSet eSrc, const UINT16 nPreLen,
                                    UINT32List* pCutPosList )
{
    UINT16              nCch;
    UINT8               nGrbit;
    ShStrTabEntry*      pRet;

    if( nPreLen )
        nCch = nPreLen;
    else
    {
        rIn >> nCch;
        nLeft -= 2;
    }
    rIn >> nGrbit;
    nLeft--;

    const BOOL  b8Bit = ( nGrbit & 0x01 ) == 0x00;
    const BOOL  bRich = ( nGrbit & 0x08 ) != 0x00;
    const BOOL  bFarEast = ( nGrbit & 0x04 ) != 0x00;

    UINT16      nCrun;
    UINT32      nExtInf;

    if( bRich )
    {
        rIn >> nCrun;
        nLeft -= sizeof( nCrun );
    }
    else
        nCrun = 0;

    if( bFarEast )
    {
        rIn >> nExtInf;
        nLeft -= sizeof( nExtInf );
    }
    else
        nExtInf = 0;

    if( pCutPosList )
    {
        if( bRich )
            pRet = new ShStrTabFormEntry(
                            ReadString( rIn, nLeft, nCch, b8Bit, *pCutPosList, eSrc ), rIn, nLeft, nCrun );
        else
            pRet = new ShStrTabEntry( ReadString( rIn, nLeft, nCch, b8Bit, *pCutPosList, eSrc ) );
    }
    else
    {
        if( bRich )
            pRet = new ShStrTabFormEntry( ReadString( rIn, nLeft, nCch, b8Bit, eSrc ), rIn, nLeft, nCrun );
        else
            pRet = new ShStrTabEntry( ReadString( rIn, nLeft, nCch, b8Bit, eSrc ) );
    }

    if( nExtInf )
    {
        rIn.SeekRel( nExtInf );
        nLeft -= nExtInf;
    }

    return pRet;
}




String ReadUnicodeString( SvStream& rIn, INT32& nLeft, CharSet eSrc, const UINT16 nPreLen )
{
    UINT16      nCch;
    UINT8       nGrbit;

    if( nPreLen )
        nCch = nPreLen;
    else
    {
        rIn >> nCch;
        nLeft -= 2;
    }

    rIn >> nGrbit;
    nLeft--;

    const BOOL  b8Bit = ( nGrbit & 0x01 ) == 0x00;
    const BOOL  bRich = ( nGrbit & 0x08 ) != 0x00;
    const BOOL  bFarEast = ( nGrbit & 0x04 ) != 0x00;

    UINT16      nCrun;
    UINT32      nExtInf;

    if( bRich )
    {
        rIn >> nCrun;
        nLeft -= sizeof( nCrun );
    }
    else
        nCrun = 0;

    if( bFarEast )
    {
        rIn >> nExtInf;
        nLeft -= sizeof( nExtInf );
    }
    else
        nExtInf = 0;

    String      aRet = ReadString( rIn, nLeft, nCch, b8Bit, eSrc );

    UINT32      nSeek = nCrun * 4 + nExtInf;
    if( nSeek )
    {
        rIn.SeekRel( nSeek );
        nLeft -= nSeek;
    }

    return aRet;
}




void SkipUnicodeString( SvStream& rIn, INT32& nLeft, const UINT16 nPreLen )
{
    UINT16      nCch;
    UINT8       nGrbit;

    if( nPreLen )
        nCch = nPreLen;
    else
    {
        rIn >> nCch;
        nLeft -= 2;
    }

    rIn >> nGrbit;
    nLeft--;

    const BOOL  b8Bit = ( nGrbit & 0x01 ) == 0x00;
    const BOOL  bRich = ( nGrbit & 0x08 ) != 0x00;
    const BOOL  bFarEast = ( nGrbit & 0x04 ) != 0x00;

    UINT16      nCrun;
    UINT32      nExtInf;

    if( bRich )
    {
        rIn >> nCrun;
        nLeft -= sizeof( nCrun );
    }
    else
        nCrun = 0;

    if( bFarEast )
    {
        rIn >> nExtInf;
        nLeft -= sizeof( nExtInf );
    }
    else
        nExtInf = 0;

    SkipString( rIn, nLeft, nCch, b8Bit );


    UINT32      nSeek = nCrun * 4 + nExtInf;
    if( nSeek )
    {
        rIn.SeekRel( nSeek );
        nLeft -= nSeek;
    }
}




INT16 lcl_ReadExternSheetChar( SvStream& aIn, INT32& nBytesLeft, UINT16& nCharsLeft, BOOL b16Bit )
{
    INT16       nRetVal;
    sal_Char    cChar;

    if( b16Bit )
    {
        aIn >> nRetVal;
        nBytesLeft -= 2;
    }
    else
    {
        aIn >> cChar;
        nRetVal = (INT16) cChar;
        nBytesLeft--;
    }
    nCharsLeft--;
    return nRetVal;
}




// reads 8 bit and 16 bit strings
void EncodeExternSheet( SvStream& aIn, String& aFile, String& aTabName,
                        INT32& nBytesLeft, BOOL& rbSWbk, UINT16 nLen, BOOL b16Bit )
{
    enum        State{ S_INIT, S_PATH, S_FILENAME, S_TABNAME };
    INT16       cEnc;
    State       eS = S_INIT;

    rbSWbk = FALSE;
    while( nLen )
    {
        cEnc = lcl_ReadExternSheetChar( aIn, nBytesLeft, nLen, b16Bit );
        switch( eS )
        {
            case S_INIT:
                switch( cEnc )
                {
                    case 0x00:                          // empty
                    case 0x01:                  break;  // encode
                    case 0x02:                          // self
                    case 0x03:  rbSWbk = TRUE;  break;  // encode & self
                    default:    aFile += ( sal_Unicode ) cEnc;
                }
                eS = S_PATH;
                break;
            case S_PATH:
                switch( cEnc )
                {
                    case 0x01:
                        if( nLen > 1 )
                        {
                            cEnc = lcl_ReadExternSheetChar( aIn, nBytesLeft, nLen, b16Bit );
                            if( cEnc == '@' )
                                aFile.AppendAscii( "//" );
                            else
                            {
                                aFile += ( sal_Unicode ) cEnc;
                                aFile.AppendAscii( ":\\" );
                            }
                        }
                        else
                            aFile.AppendAscii( "<NULL-DRIVE!>" );
                        break;
                    case 0x02:
                    case 0x03:  aFile.AppendAscii( "\\" );      break;
                    case 0x04:  aFile.AppendAscii( "..\\") ;    break;
                    case 0x05:
                        if( nLen )
                        {
                            INT16 nVolLen = lcl_ReadExternSheetChar( aIn, nBytesLeft, nLen, b16Bit );
                            if( nVolLen )
                            {
                                while( nVolLen )
                                {
                                    cEnc = lcl_ReadExternSheetChar( aIn, nBytesLeft, nLen, b16Bit );
                                    aFile += ( sal_Unicode ) cEnc;
                                    nVolLen--;
                                }
                            }
                            else
                                aFile.AppendAscii( "<EMPTY MAC-LONG-VOLUME>" );

                            aFile.AppendAscii( ":" );
                        }
                        else
                            aFile.AppendAscii( "<ERROR IN MAC-LONG-VOLUME>" );
                        break;
                    case 0x06:  aFile.AppendAscii( "<Startup Dir>:" );      break;
                    case 0x07:  aFile.AppendAscii( "<Alt Startup Dir>:" );  break;
                    case 0x08:  aFile.AppendAscii( "<Library>" );           break;
                    case '[':   eS = S_FILENAME;                            break;  // Start von FileName
                    default:    aFile += ( sal_Unicode ) cEnc;
                }
                break;
            case S_FILENAME:
                switch( cEnc )
                {
                    case ']':   eS = S_TABNAME;     break;  // Ende FileName
                    default:    aFile += ( sal_Unicode ) cEnc;
                }
                break;
            case S_TABNAME:     aTabName += ( sal_Unicode ) cEnc;   break;
        }
    }
}




void EncodeExternSheetUnicode( SvStream& aIn, String& aFile, String& aTabName, INT32& nBytesLeft,
                              BOOL& rbSWbk, UINT16 nPreLen )
{
    UINT16              nCch;
    UINT8               nGrbit;
    String              aRet;

    if( nPreLen )
        nCch = nPreLen;
    else
    {
        aIn >> nCch;
        nBytesLeft -= 2;
    }

    aIn >> nGrbit;
    nBytesLeft--;

    UINT16 b16Bit = (nGrbit & 0x01);

    if( nGrbit & 0x04 )
    {
        // extended String Far-East-Version
        UINT32      nExtRstLen;
        UINT8       nEnc;

        aIn >> nExtRstLen >> nEnc;
        nBytesLeft -= sizeof( nExtRstLen ) + sizeof( nEnc );
        EncodeExternSheet( aIn, aFile, aTabName, nBytesLeft, rbSWbk, nCch, b16Bit );
        aIn.SeekRel( nExtRstLen );
        nBytesLeft -= nExtRstLen;
    }
    else if( nGrbit & 0x08 )
    {
        // extended String Rich-String-Version
        UINT32      nTmp;
        UINT16      nCrun;

        aIn >> nCrun;
        nBytesLeft -= sizeof( nCrun );
        EncodeExternSheet( aIn, aFile, aTabName, nBytesLeft, rbSWbk, nCch, b16Bit );
        nTmp = nCrun * 4;
        aIn.SeekRel( nTmp );
        nBytesLeft -= nTmp;
    }
    else
        EncodeExternSheet( aIn, aFile, aTabName, nBytesLeft, rbSWbk, nCch, b16Bit );
}




long CalcX( const UINT16 nT, const UINT16 nC, const UINT16 nOC, const double f, ScDocument* pD )
{
    return ( long ) (
        ( ( double ) pD->GetColOffset( nC, nT )
            + ( double ) pD->GetColWidth( nC, nT ) * ( ( double ) nOC / 1024.0 ) )
        * f );
}




long CalcY( const UINT16 nT, const UINT16 nR, const UINT16 nOR, const double f, ScDocument* pD )
{
    return ( long ) (
        ( ( double ) pD->GetRowOffset( nR, nT )
            + ( double ) pD->GetRowHeight( nR, nT ) * ( ( double ) nOR / 256.0 ) )
        * f );
}



BOOL GetExcRKValue( double fValue, INT32& rRKValue )
{
    double fFrac, fInt;

    // integer
    fFrac = modf( fValue, &fInt );
    if( (fFrac == 0.0) && (fInt >= -536870912.0) && (fInt <= 536870911.0) ) // 2^29 =  536870912
    {
        rRKValue = (INT32) fInt;
        rRKValue <<= 2;
        rRKValue |= EXC_RK_INT;
        return TRUE;
    }

    // integer/100
    fFrac = modf( fValue * 100.0, &fInt );
    if( (fFrac == 0.0) && (fInt >= -536870912.0) && (fInt <= 536870911.0) )
    {
        rRKValue = ( INT32 ) fInt;
        rRKValue <<= 2;
        rRKValue |= EXC_RK_INT100;
        return TRUE;
    }

    // double
    return FALSE;
}




ExcScenarioCell::ExcScenarioCell( const UINT16 nC, const UINT16 nR ) : nCol( nC ), nRow( nR )
{
}


void ExcScenarioCell::SetValue( const String& r )
{
    aValue = r;
}




#define EXCSCAPPEND(EXCSCCELL)  (List::Insert(EXCSCCELL,LIST_APPEND))
#define EXCSCFIRST()            ((ExcScenarioCell*)List::First())
#define EXCSCNEXT()             ((ExcScenarioCell*)List::Next())


ExcScenario::ExcScenario( SvStream& rIn, const RootData& rR ) : nTab( *rR.pAktTab )
{
    const CharSet   eSrc = *rR.pCharset;

    UINT16          nCref;
    UINT8           nName, nComment;
    INT32           nD = 0x7FFFFFFF;

    rIn >> nCref;
    rIn.SeekRel( 2 );
    rIn >> nName >> nComment;
    rIn.SeekRel( 1 );       // statt nUser!

    if( nName )
        pName = new String( ::ReadUnicodeString( rIn, nD, eSrc, nName ) );
    else
    {
        pName = new String( RTL_CONSTASCII_STRINGPARAM( "Scenery" ) );
        rIn.SeekRel( 1 );
    }

    pUserName = new String( ::ReadUnicodeString( rIn, nD, eSrc ) );

    if( nComment )
        pComment = new String( ::ReadUnicodeString( rIn, nD, eSrc, 0 ) );
    else
        pComment = new String( EMPTY_STRING );

    UINT16          n = nCref;
    UINT16          nC, nR;
    while( n )
    {
        rIn >> nR >> nC;

        EXCSCAPPEND( new ExcScenarioCell( nC, nR ) );

        n--;
    }

    n = nCref;
    ExcScenarioCell*    p = EXCSCFIRST();
    while( p )
    {
        p->SetValue( ::ReadUnicodeString( rIn, nD, eSrc ) );

        p = EXCSCNEXT();
    }
}


ExcScenario::~ExcScenario()
{
    ExcScenarioCell*    p = EXCSCFIRST();

    while( p )
    {
        delete p;
        p = EXCSCNEXT();
    }

    if( pName )
        delete pName;
    if( pComment )
        delete pComment;
    if( pUserName )
        delete pUserName;
}


void ExcScenario::Apply( ScDocument& r, const BOOL bLast )
{
    UINT32              nNumCells = List::Count();

    ExcScenarioCell*    p = EXCSCFIRST();
    String              aSzenName( *pName );
    r.CreateValidTabName( aSzenName );
    UINT16              nNewTab = nTab + 1;

    if( !r.InsertTab( nNewTab, aSzenName ) )
        return;

    r.SetScenario( nNewTab, TRUE );
    r.SetScenarioData( nNewTab, *pComment, COL_LIGHTGRAY, SC_SCENARIO_SHOWFRAME|SC_SCENARIO_COPYALL );

    while( p )
    {
        UINT16          nCol = p->nCol;
        UINT16          nRow = p->nRow;
        String          aVal = p->GetValue();

        r.ApplyFlagsTab( nCol, nRow, nCol, nRow, nNewTab, SC_MF_SCENARIO );

        r.SetString( nCol, nRow, nNewTab, aVal );

        p = EXCSCNEXT();
    }

    if( bLast )
        r.SetActiveScenario( nNewTab, TRUE );
}




ExcScenarioList::~ExcScenarioList()
{
    ExcScenario*    p = _First();

    while( p )
    {
        delete p;
        p = _Next();
    }
}


void ExcScenarioList::Apply( ScDocument& r )
{
    ExcScenario*    p = _Last();
    UINT16          n = ( UINT16 ) Count();

    while( p )
    {
        n--;
        p->Apply( r, ( BOOL ) ( n == nLastScenario ) );
        p = _Prev();
    }
}

//_________________________________________________________

String ScGetHexStr( UINT16 nValue )
{
    static const sal_Char   pH[] = "0123456789ABCDEF";
    String                  aStrName;

    aStrName += pH[ nValue >> 12 ];
    aStrName += pH[ (nValue >> 8) & 0x000F ];
    aStrName += pH[ (nValue >> 4) & 0x000F ];
    aStrName += pH[ nValue & 0x000F ];

    return aStrName;
}

//_________________________________________________________

const sal_Char* GetExcBuiltInName( sal_Unicode n )
{
    const sal_Char* p[] = {
        "BuiltIn_Consolidate_Area","BuiltIn_Auto_Open","BuiltIn_Auto_Close",
        "BuiltIn_Extract","BuiltIn_Database","BuiltIn_Criteria","BuiltIn_Print_Area",
        "BuiltIn_Print_Titles","BuiltIn_Recorder","BuiltIn_Data_Form",
        "BuiltIn_Auto_Activate","BuiltIn_Auto_Deactivate","BuiltIn_SheetTitle",
        "BuiltIn_AutoFilter","BuiltIn_UNKNOWN" };

    if( n < 0x00 || n > EXC_BUILTIN_UNKNOWN )
        n = EXC_BUILTIN_UNKNOWN;

    return p[ n ];
}

