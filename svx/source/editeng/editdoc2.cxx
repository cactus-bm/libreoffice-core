/*************************************************************************
 *
 *  $RCSfile: editdoc2.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: mt $ $Date: 2002-07-12 13:31:13 $
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

#include <eeng_pch.hxx>

#pragma hdrstop

#ifndef _SFXSMPLHINT_HXX //autogen
#include <svtools/smplhint.hxx>
#endif
#include <tools/rtti.hxx>
#include <lspcitem.hxx>
#include <adjitem.hxx>
#include <tstpitem.hxx>

#include <editdoc.hxx>
#include <impedit.hxx>
#include <editdbg.hxx>

#include <numitem.hxx>

#include "akrnitem.hxx"
#include "cntritem.hxx"
#include "colritem.hxx"
#include "crsditem.hxx"
#include "escpitem.hxx"
#include "fhgtitem.hxx"
#include "fontitem.hxx"
#include "kernitem.hxx"
#include "lrspitem.hxx"
#include "postitem.hxx"
#include "shdditem.hxx"
#include "udlnitem.hxx"
#include "ulspitem.hxx"
#include "wghtitem.hxx"
#include "wrlmitem.hxx"
#include <charscaleitem.hxx>

#include <vcl/svapp.hxx>    // Fuer AppWindow...

DBG_NAME( EE_ParaPortion );

SV_IMPL_VARARR( CharPosArray, long );

/*

BOOL EditStyleSheet::HasStyleAsAnyParent( SfxStyleSheet& rStyle )
{
    if ( GetParent() == rStyle.GetName() )
        return TRUE;

    if ( GetParent().Len() && ( GetParent() != GetName() ) )
    {
        EditStyleSheet* pS = (EditStyleSheet*)GetPool().Find( GetParent(), rStyle.GetFamily() );
        if ( pS )
            return pS->HasStyleAsAnyParent( rStyle );
    }
    return FALSE;
}

*/

// -------------------------------------------------------------------------
// class TextPortionList
// -------------------------------------------------------------------------
TextPortionList::TextPortionList()
{
}

TextPortionList::~TextPortionList()
{
    Reset();
}

void TextPortionList::Reset()
{
    for ( USHORT nPortion = 0; nPortion < Count(); nPortion++ )
        delete GetObject( nPortion );
    Remove( 0, Count() );
}

void TextPortionList::DeleteFromPortion( USHORT nDelFrom )
{
    DBG_ASSERT( ( nDelFrom < Count() ) || ( (nDelFrom == 0) && (Count() == 0) ), "DeleteFromPortion: Out of range" );
    for ( USHORT nP = nDelFrom; nP < Count(); nP++ )
        delete GetObject( nP );
    Remove( nDelFrom, Count()-nDelFrom );
}

USHORT TextPortionList::FindPortion( USHORT nCharPos, USHORT& nPortionStart, BOOL bPreferStartingPortion )
{
    // Bei nCharPos an Portion-Grenze wird die linke Portion gefunden
    USHORT nTmpPos = 0;
    for ( USHORT nPortion = 0; nPortion < Count(); nPortion++ )
    {
        TextPortion* pPortion = GetObject( nPortion );
        nTmpPos += pPortion->GetLen();
        if ( nTmpPos >= nCharPos )
        {
            // take this one if we don't prefer the starting portion, or if it's the last one
            if ( ( nTmpPos != nCharPos ) || !bPreferStartingPortion || ( nPortion == Count() - 1 ) )
            {
                nPortionStart = nTmpPos - pPortion->GetLen();
                return nPortion;
            }
        }
    }
    DBG_ERROR( "FindPortion: Nicht gefunden!" );
    return ( Count() - 1 );
}

USHORT TextPortionList::GetStartPos( USHORT nPortion )
{
    USHORT nPos = 0;
    for ( USHORT n = 0; n < nPortion; n++ )
    {
        TextPortion* pPortion = GetObject( n );
        nPos += pPortion->GetLen();
    }
    return nPos;
}


// -------------------------------------------------------------------------
// class ExtraPortionInfo
// -------------------------------------------------------------------------

ExtraPortionInfo::ExtraPortionInfo()
{
    nOrgWidth = 0;
    nWidthFullCompression = 0;
    nMaxCompression100thPercent = 0;
    nAsianCompressionTypes = 0;
    nPortionOffsetX = 0;
    bFirstCharIsRightPunktuation = FALSE;
    bCompressed = FALSE;
    pOrgDXArray = NULL;
}

ExtraPortionInfo::~ExtraPortionInfo()
{
    delete[] pOrgDXArray;
}

void ExtraPortionInfo::SaveOrgDXArray( const long* pDXArray, USHORT nLen )
{
    delete[] pOrgDXArray;
    pOrgDXArray = new long[nLen];
    memcpy( pOrgDXArray, pDXArray, nLen*sizeof(long) );
}

void ExtraPortionInfo::DestroyOrgDXArray()
{
    delete[] pOrgDXArray;
    pOrgDXArray = NULL;
}


// -------------------------------------------------------------------------
// class ParaPortion
// -------------------------------------------------------------------------
ParaPortion::ParaPortion( ContentNode* pN )
{
    DBG_CTOR( EE_ParaPortion, 0 );

    pNode               = pN;
    bInvalid            = TRUE;
    bVisible            = TRUE;
    bSimple             = FALSE;
    bForceRepaint       = FALSE;
    nInvalidPosStart    = 0;
    nInvalidDiff        = 0;
    nHeight             = 0;
    nFirstLineOffset    = 0;
    nBulletX            = 0;
}

ParaPortion::~ParaPortion()
{
    DBG_DTOR( EE_ParaPortion, 0 );
}

void ParaPortion::MarkInvalid( USHORT nStart, short nDiff )
{
    if ( bInvalid == FALSE )
    {
//      nInvalidPosEnd = nStart;    // ??? => CreateLines
        nInvalidPosStart = ( nDiff >= 0 ) ? nStart : ( nStart + nDiff );
        nInvalidDiff = nDiff;
    }
    else
    {
        // Einfaches hintereinander tippen
        if ( ( nDiff > 0 ) && ( nInvalidDiff > 0 ) &&
             ( ( nInvalidPosStart+nInvalidDiff ) == nStart ) )
        {
            nInvalidDiff += nDiff;
        }
        // Einfaches hintereinander loeschen
        else if ( ( nDiff < 0 ) && ( nInvalidDiff < 0 ) && ( nInvalidPosStart == nStart ) )
        {
            nInvalidPosStart += nDiff;
            nInvalidDiff += nDiff;
        }
        else
        {
//          nInvalidPosEnd = pNode->Len();
            DBG_ASSERT( ( nDiff >= 0 ) || ( (nStart+nDiff) >= 0 ), "MarkInvalid: Diff out of Range" );
            nInvalidPosStart = Min( nInvalidPosStart, (USHORT) ( nDiff < 0 ? nStart+nDiff : nDiff ) );
            nInvalidDiff = 0;
            bSimple = FALSE;
        }
    }
    bInvalid = TRUE;
    aScriptInfos.Remove( 0, aScriptInfos.Count() );
    aWritingDirectionInfos.Remove( 0, aWritingDirectionInfos.Count() );
//  aExtraCharInfos.Remove( 0, aExtraCharInfos.Count() );
}

void ParaPortion::MarkSelectionInvalid( USHORT nStart, USHORT nEnd )
{
    if ( bInvalid == FALSE )
    {
        nInvalidPosStart = nStart;
//      nInvalidPosEnd = nEnd;
    }
    else
    {
        nInvalidPosStart = Min( nInvalidPosStart, nStart );
//      nInvalidPosEnd = pNode->Len();
    }
    nInvalidDiff = 0;
    bInvalid = TRUE;
    bSimple = FALSE;
    aScriptInfos.Remove( 0, aScriptInfos.Count() );
    aWritingDirectionInfos.Remove( 0, aWritingDirectionInfos.Count() );
//  aExtraCharInfos.Remove( 0, aExtraCharInfos.Count() );
}

void ParaPortion::AdjustBlocks( EditLine* pLine, long nRemainingSpace )
{
    DBG_ASSERT( nRemainingSpace > 0, "AdjustBlocks: Etwas zuwenig..." );
    DBG_ASSERT( pLine, "AdjustBlocks: Zeile ?!" );
    if ( ( nRemainingSpace < 0 ) || pLine->IsEmpty() )
        return ;

//  USHORTs aBlanks;
    const USHORT nFirstChar = pLine->GetStart();
    const USHORT nLastChar = pLine->GetEnd() -1;    // Last zeigt dahinter

    DBG_ASSERT( nLastChar < pNode->Len(), "AdjustBlocks: Out of range!" );

    // Blanks suchen:
    USHORT nBlanks = 0;
    USHORT nChar;
    for ( nChar = nFirstChar; nChar <= nLastChar; nChar++ )
    {
        if ( pNode->GetChar(nChar) == ' ' )
            nBlanks++;
    }

    if ( !nBlanks )
        return;

    // Wenn das letzte Zeichen ein Blank ist, will ich es nicht haben!
    // Die Breite muss auf die Blocker davor verteilt werden...
    // Aber nicht, wenn es das einzige ist
    if ( ( pNode->GetChar( nLastChar ) == ' ' ) && ( nBlanks > 1 ) )
    {
        nBlanks--;
        USHORT nPortionStart, nPortion;
        nPortion = GetTextPortions().FindPortion( nLastChar+1, nPortionStart );
        TextPortion* pLastPortion = GetTextPortions()[ nPortion ];
        long nRealWidth = pLine->GetCharPosArray()[nLastChar-nFirstChar];
        long nBlankWidth = nRealWidth;
        if ( nLastChar > nPortionStart )
            nBlankWidth -= pLine->GetCharPosArray()[nLastChar-nFirstChar-1];
        // Evtl. ist das Blank schon in ImpBreakLine abgezogen worden:
        if ( nRealWidth == pLastPortion->GetSize().Width() )
        {
            // Beim letzten Zeichen muss die Portion hinter dem Blank aufhoeren
            // => Korrektur vereinfachen:
            DBG_ASSERT( ( nPortionStart + pLastPortion->GetLen() ) == ( nLastChar+1 ), "Blank doch nicht am Portion-Ende?!" );
            pLastPortion->GetSize().Width() -= nBlankWidth;
            nRemainingSpace += nBlankWidth;
        }
        pLine->GetCharPosArray()[nLastChar-nFirstChar] -= nBlankWidth;
    }

    const long nMore4Everyone = nRemainingSpace / nBlanks;
    long nSomeExtraSpace = nRemainingSpace - nMore4Everyone*nBlanks;

    DBG_ASSERT( nSomeExtraSpace < (long)nBlanks, "AdjustBlocks: ExtraSpace zu gross" );
    DBG_ASSERT( nSomeExtraSpace >= 0, "AdjustBlocks: ExtraSpace < 0 " );

    // Die Positionen im Array und die Portion-Breiten korrigieren:
    // Letztes Zeichen wird schon nicht mehr beachtet...
    for ( nChar = nFirstChar; nChar < nLastChar; nChar++ )
    {
        if ( pNode->GetChar( nChar ) == ' ' )
        {
            USHORT nPortionStart, nPortion;
            nPortion = GetTextPortions().FindPortion( nChar, nPortionStart );
            TextPortion* pLastPortion = GetTextPortions()[ nPortion ];

            // Die Breite der Portion:
            pLastPortion->GetSize().Width() += nMore4Everyone;
            if ( nSomeExtraSpace )
                pLastPortion->GetSize().Width()++;

            // Die Zeichenpositionen ab dem Blank:
            USHORT nPortionEnd = nPortionStart + pLastPortion->GetLen();
            for ( USHORT n = nChar; n < nPortionEnd; n++ )
            {
                pLine->GetCharPosArray()[n-nFirstChar] += nMore4Everyone;
                if ( nSomeExtraSpace )
                    pLine->GetCharPosArray()[n-nFirstChar]++;
            }

            if ( nSomeExtraSpace )
                nSomeExtraSpace--;
        }
    }
}

USHORT ParaPortion::GetLineNumber( USHORT nIndex )
{
    DBG_ASSERTWARNING( aLineList.Count(), "Leere ParaPortion in GetLine!" );
    DBG_ASSERT( bVisible, "Wozu GetLine() bei einem unsichtbaren Absatz?" );

    for ( USHORT nLine = 0; nLine < aLineList.Count(); nLine++ )
    {
        if ( aLineList[nLine]->IsIn( nIndex ) )
            return nLine;
    }

    // Dann sollte es am Ende der letzten Zeile sein!
    DBG_ASSERT( nIndex == aLineList[ aLineList.Count() - 1 ]->GetEnd(), "Index voll daneben!" );
    return (aLineList.Count()-1);
}

void ParaPortion::SetVisible( BOOL bMakeVisible )
{
    bVisible = bMakeVisible;
}

void ParaPortion::CorrectValuesBehindLastFormattedLine( USHORT nLastFormattedLine )
{
    USHORT nLines = aLineList.Count();
    DBG_ASSERT( nLines, "CorrectPortionNumbersFromLine: Leere Portion?" );
    if ( nLastFormattedLine < ( nLines - 1 ) )
    {
        const EditLine* pLastFormatted = aLineList[ nLastFormattedLine ];
        const EditLine* pUnformatted = aLineList[ nLastFormattedLine+1 ];
        short nPortionDiff = pUnformatted->GetStartPortion() - pLastFormatted->GetEndPortion();
        short nTextDiff = pUnformatted->GetStart() - pLastFormatted->GetEnd();
        nTextDiff++;    // LastFormatted->GetEnd() war incl. => 1 zuviel abgezogen!

        // Die erste unformatierte muss genau eine Portion hinter der letzten der
        // formatierten beginnen:
        // Wenn in der geaenderten Zeile eine Portion gesplittet wurde,
        // kann nLastEnd > nNextStart sein!
        short nPDiff = -( nPortionDiff-1 );
        short nTDiff = -( nTextDiff-1 );
        if ( nPDiff || nTDiff )
        {
            for ( USHORT nL = nLastFormattedLine+1; nL < nLines; nL++ )
            {
                EditLine* pLine = aLineList[ nL ];

                pLine->GetStartPortion() += nPDiff;
                pLine->GetEndPortion() += nPDiff;

                pLine->GetStart() += nTDiff;
                pLine->GetEnd() += nTDiff;

                pLine->SetValid();
            }
        }
    }
    DBG_ASSERT( aLineList[ aLineList.Count()-1 ]->GetEnd() == pNode->Len(), "CorrectLines: Ende stimmt nicht!" );
}

// -------------------------------------------------------------------------
// class ParaPortionList
// -------------------------------------------------------------------------
ParaPortionList::ParaPortionList()
{
}

ParaPortionList::~ParaPortionList()
{
    Reset();
}

void ParaPortionList::Reset()
{
    for ( USHORT nPortion = 0; nPortion < Count(); nPortion++ )
        delete GetObject( nPortion );
    Remove( 0, Count() );
}

long ParaPortionList::GetYOffset( ParaPortion* pPPortion )
{
    long nHeight = 0;
    for ( USHORT nPortion = 0; nPortion < Count(); nPortion++ )
    {
        ParaPortion* pTmpPortion = GetObject(nPortion);
        if ( pTmpPortion == pPPortion )
            return nHeight;
        nHeight += pTmpPortion->GetHeight();
    }
    DBG_ERROR( "GetYOffset: Portion nicht gefunden" );
    return nHeight;
}

USHORT ParaPortionList::FindParagraph( long nYOffset )
{
    long nY = 0;
    for ( USHORT nPortion = 0; nPortion < Count(); nPortion++ )
    {
        nY += GetObject(nPortion)->GetHeight(); // sollte auch bei !bVisible richtig sein!
        if ( nY > nYOffset )
            return nPortion;
    }
    return 0xFFFF;  // solte mal ueber EE_PARA_NOT_FOUND erreicht werden!
}

void ParaPortionList::DbgCheck( EditDoc& rDoc )
{
#ifdef DBG_UTIL
    DBG_ASSERT( Count() == rDoc.Count(), "ParaPortionList::DbgCheck() - Count() ungleich!" );
    for ( USHORT i = 0; i < Count(); i++ )
    {
        DBG_ASSERT( SaveGetObject(i), "ParaPortionList::DbgCheck() - Null-Pointer in Liste!" );
        DBG_ASSERT( GetObject(i)->GetNode(), "ParaPortionList::DbgCheck() - Null-Pointer in Liste(2)!" );
        DBG_ASSERT( GetObject(i)->GetNode() == rDoc.GetObject(i), "ParaPortionList::DbgCheck() - Eintraege kreuzen sich!" );
    }
#endif
}


ContentAttribsInfo::ContentAttribsInfo( const SfxItemSet& rParaAttribs ) :
        aPrevParaAttribs( rParaAttribs)
{
}


void ConvertItem( SfxPoolItem& rPoolItem, MapUnit eSourceUnit, MapUnit eDestUnit )
{
    DBG_ASSERT( eSourceUnit != eDestUnit, "ConvertItem - Why?!" );

    switch ( rPoolItem.Which() )
    {
        case EE_PARA_LRSPACE:
        {
            DBG_ASSERT( rPoolItem.IsA( TYPE( SvxLRSpaceItem ) ), "ConvertItem: Ungueltiges Item!" );
            SvxLRSpaceItem& rItem = (SvxLRSpaceItem&)rPoolItem;
            rItem.SetTxtFirstLineOfst( OutputDevice::LogicToLogic( rItem.GetTxtFirstLineOfst(), eSourceUnit, eDestUnit ) );
            rItem.SetTxtLeft( OutputDevice::LogicToLogic( rItem.GetTxtLeft(), eSourceUnit, eDestUnit ) );
//          rItem.SetLeft( OutputDevice::LogicToLogic( rItem.GetLeft(), eSourceUnit, eDestUnit ) ); // #96298# SetLeft manipulates nTxtLeft!
            rItem.SetRight( OutputDevice::LogicToLogic( rItem.GetRight(), eSourceUnit, eDestUnit ) );
        }
        break;
        case EE_PARA_ULSPACE:
        {
            DBG_ASSERT( rPoolItem.IsA( TYPE( SvxULSpaceItem ) ), "ConvertItem: Ungueltiges Item!" );
            SvxULSpaceItem& rItem = (SvxULSpaceItem&)rPoolItem;
            rItem.SetUpper( OutputDevice::LogicToLogic( rItem.GetUpper(), eSourceUnit, eDestUnit ) );
            rItem.SetLower( OutputDevice::LogicToLogic( rItem.GetLower(), eSourceUnit, eDestUnit ) );
        }
        break;
        case EE_PARA_SBL:
        {
            DBG_ASSERT( rPoolItem.IsA( TYPE( SvxLineSpacingItem ) ), "ConvertItem: Ungueltiges Item!" );
            SvxLineSpacingItem& rItem = (SvxLineSpacingItem&)rPoolItem;
            // #96298# SetLineHeight changes also eLineSpace!
            if ( rItem.GetLineSpaceRule() == SVX_LINE_SPACE_MIN )
                rItem.SetLineHeight( OutputDevice::LogicToLogic( rItem.GetLineHeight(), eSourceUnit, eDestUnit ) );
        }
        break;
        case EE_PARA_TABS:
        {
            DBG_ASSERT( rPoolItem.IsA( TYPE( SvxTabStopItem ) ), "ConvertItem: Ungueltiges Item!" );
            SvxTabStopItem& rItem = (SvxTabStopItem&)rPoolItem;
            SvxTabStopItem aNewItem( EE_PARA_TABS );
            for ( USHORT i = 0; i < rItem.Count(); i++ )
            {
                const SvxTabStop& rTab = rItem[i];
                SvxTabStop aNewStop( OutputDevice::LogicToLogic( rTab.GetTabPos(), eSourceUnit, eDestUnit ), rTab.GetAdjustment(), rTab.GetDecimal(), rTab.GetFill() );
                aNewItem.Insert( aNewStop );
            }
            rItem = aNewItem;
        }
        break;
        case EE_CHAR_FONTHEIGHT:
        case EE_CHAR_FONTHEIGHT_CJK:
        case EE_CHAR_FONTHEIGHT_CTL:
        {
            DBG_ASSERT( rPoolItem.IsA( TYPE( SvxFontHeightItem ) ), "ConvertItem: Ungueltiges Item!" );
            SvxFontHeightItem& rItem = (SvxFontHeightItem&)rPoolItem;
            rItem.SetHeight( OutputDevice::LogicToLogic( rItem.GetHeight(), eSourceUnit, eDestUnit ) );
        }
        break;
    }
}

void ConvertAndPutItems( SfxItemSet& rDest, const SfxItemSet& rSource, const MapUnit* pSourceUnit, const MapUnit* pDestUnit )
{
    const SfxItemPool* pSourcePool = rSource.GetPool();
    const SfxItemPool* pDestPool = rDest.GetPool();

    for ( USHORT nWhich = EE_PARA_START; nWhich <= EE_CHAR_END; nWhich++ )
    {
        // Wenn moeglich ueber SlotID gehen...

        USHORT nSourceWhich = nWhich;
        USHORT nSlot = pDestPool->GetTrueSlotId( nWhich );
        if ( nSlot )
        {
            USHORT nW = pSourcePool->GetTrueWhich( nSlot );
            if ( nW )
                nSourceWhich = nW;
        }

        if ( rSource.GetItemState( nSourceWhich, FALSE ) == SFX_ITEM_ON )
        {
            MapUnit eSourceUnit = pSourceUnit ? *pSourceUnit : (MapUnit)pSourcePool->GetMetric( nSourceWhich );
            MapUnit eDestUnit = pDestUnit ? *pDestUnit : (MapUnit)pDestPool->GetMetric( nWhich );
            if ( eSourceUnit != eDestUnit )
            {
                SfxPoolItem* pItem = rSource.Get( nSourceWhich ).Clone();
//              pItem->SetWhich( nWhich );
                ConvertItem( *pItem, eSourceUnit, eDestUnit );
                rDest.Put( *pItem, nWhich );
                delete pItem;
            }
            else
            {
                rDest.Put( rSource.Get( nSourceWhich ), nWhich );
            }
        }
        else
        {
            // MT 3.3.99: Waere so eigentlich richtig, aber schon seit Jahren nicht so...
//          rDest.ClearItem( nWhich );
        }
    }
}

