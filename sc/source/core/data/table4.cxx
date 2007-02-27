/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: table4.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: vg $ $Date: 2007-02-27 12:09:54 $
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

// System - Includes -----------------------------------------------------



#ifdef WNT
#pragma optimize("",off)
                                        // sonst Absturz Win beim Fuellen
#endif

// INCLUDE ---------------------------------------------------------------

#include "scitems.hxx"
#include <svx/algitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/brshitem.hxx>
#include <svx/cntritem.hxx>
#include <svx/colritem.hxx>
#include <svx/crsditem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/fontitem.hxx>
#include <svx/langitem.hxx>
#include <svx/postitem.hxx>
#include <svx/shdditem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/wghtitem.hxx>
#ifndef _SVX_ROTMODIT_HXX //autogen
#include <svx/rotmodit.hxx>
#endif
#include <svx/editobj.hxx>
#include <svx/editeng.hxx>
#include <svx/eeitem.hxx>
#include <svx/escpitem.hxx>
#include <svtools/zforlist.hxx>
#include <vcl/keycodes.hxx>
#include <rtl/math.hxx>
#include <unotools/charclass.hxx>
#include <math.h>

#include "attrib.hxx"
#include "patattr.hxx"
#include "cell.hxx"
#include "table.hxx"
#include "globstr.hrc"
#include "global.hxx"
#include "document.hxx"
#include "autoform.hxx"
#include "userlist.hxx"
#include "zforauto.hxx"
#include "subtotal.hxx"
#include "errorcodes.hxx"
#include "rangenam.hxx"
#include "docpool.hxx"
#include "progress.hxx"

// STATIC DATA -----------------------------------------------------------

#define _D_MAX_LONG_  (double) 0x7fffffff

extern USHORT nScFillModeMouseModifier;     // global.cxx

// -----------------------------------------------------------------------

short lcl_DecompValueString( String& aValue, sal_Int32& nVal, USHORT* pMinDigits = NULL )
{
    if ( !aValue.Len() )
    {
        nVal = 0;
        return 0;
    }
    const sal_Unicode* p = aValue.GetBuffer();
    xub_StrLen nNeg = 0;
    xub_StrLen nNum = 0;
    if ( p[nNum] == '-' )
        nNum = nNeg = 1;
    while ( p[nNum] && CharClass::isAsciiNumeric( p[nNum] ) )
        nNum++;
    if ( nNum > nNeg )
    {   // number at the beginning
        nVal = aValue.Copy( 0, nNum ).ToInt32();
        //  #60893# any number with a leading zero sets the minimum number of digits
        if ( p[nNeg] == '0' && pMinDigits && ( nNum - nNeg > *pMinDigits ) )
            *pMinDigits = nNum - nNeg;
        aValue.Erase( 0, nNum );
        return -1;
    }
    else
    {
        nNeg = 0;
        xub_StrLen nEnd = nNum = aValue.Len() - 1;
        while ( nNum && CharClass::isAsciiNumeric( p[nNum] ) )
            nNum--;
        if ( p[nNum] == '-' )
        {
            nNum--;
            nNeg = 1;
        }
        if ( nNum < nEnd - nNeg )
        {   // number at the end
            nVal = aValue.Copy( nNum + 1 ).ToInt32();
            //  #60893# any number with a leading zero sets the minimum number of digits
            if ( p[nNum+1+nNeg] == '0' && pMinDigits && ( nEnd - nNum - nNeg > *pMinDigits ) )
                *pMinDigits = nEnd - nNum - nNeg;
            aValue.Erase( nNum + 1 );
            return 1;
        }
    }
    nVal = 0;
    return 0;
}

String lcl_ValueString( sal_Int32 nValue, USHORT nMinDigits )
{
    if ( nMinDigits <= 1 )
        return String::CreateFromInt32( nValue );           // simple case...
    else
    {
        String aStr = String::CreateFromInt32( Abs( nValue ) );
        if ( aStr.Len() < nMinDigits )
        {
            String aZero;
            aZero.Fill( nMinDigits - aStr.Len(), '0' );
            aStr.Insert( aZero, 0 );
        }
        //  nMinDigits doesn't include the '-' sign -> add after inserting zeros
        if ( nValue < 0 )
            aStr.Insert( '-', 0 );
        return aStr;
    }
}

static ScBaseCell * lcl_getSuffixCell( ScDocument* pDocument, sal_Int32 nValue,
        USHORT nDigits, const String& rSuffix, CellType eCellType,
        BOOL bIsOrdinalSuffix )
{
    String aValue( lcl_ValueString( nValue, nDigits ));
    if (!bIsOrdinalSuffix)
        return new ScStringCell( aValue += rSuffix);

    String aOrdinalSuffix( ScGlobal::GetOrdinalSuffix( nValue));
    if (eCellType != CELLTYPE_EDIT)
        return new ScStringCell( aValue += aOrdinalSuffix);

    EditEngine aEngine( pDocument->GetEnginePool() );
    SfxItemSet aAttr = aEngine.GetEmptyItemSet();
    aAttr.Put( SvxEscapementItem( SVX_ESCAPEMENT_SUPERSCRIPT, EE_CHAR_ESCAPEMENT));
    aEngine.SetText( aValue );
    aEngine.QuickInsertText( aOrdinalSuffix, ESelection( 0, aValue.Len(), 0,
                aValue.Len() + aOrdinalSuffix.Len()));
    aEngine.QuickSetAttribs( aAttr, ESelection( 0, aValue.Len(), 0, aValue.Len() +
                aOrdinalSuffix.Len()));
    return new ScEditCell( aEngine.CreateTextObject(), pDocument, NULL );
}

void ScTable::FillAnalyse( SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2,
                            FillCmd& rCmd, FillDateCmd& rDateCmd,
                            double& rInc, USHORT& rMinDigits,
                            ScUserListData*& rListData, USHORT& rListIndex)
{
    DBG_ASSERT( nCol1==nCol2 || nRow1==nRow2, "FillAnalyse: falscher Bereich" );

    rInc = 0.0;
    rMinDigits = 0;
    rListData = NULL;
    rCmd = FILL_SIMPLE;
    if ( nScFillModeMouseModifier & KEY_MOD1 )
        return ;        // Ctrl-Taste: Copy

    SCCOL nAddX;
    SCROW nAddY;
    SCSIZE nCount;
    if (nCol1 == nCol2)
    {
        nAddX = 0;
        nAddY = 1;
        nCount = static_cast<SCSIZE>(nRow2 - nRow1 + 1);
    }
    else
    {
        nAddX = 1;
        nAddY = 0;
        nCount = static_cast<SCSIZE>(nCol2 - nCol1 + 1);
    }

    SCCOL nCol = nCol1;
    SCROW nRow = nRow1;

    ScBaseCell* pFirstCell = GetCell( nCol, nRow );
    CellType eCellType = pFirstCell ? pFirstCell->GetCellType() : CELLTYPE_NONE;

    if (eCellType == CELLTYPE_VALUE)
    {
        UINT32 nFormat = ((const SfxUInt32Item*)GetAttr(nCol,nRow,ATTR_VALUE_FORMAT))->GetValue();
        BOOL bDate = ( pDocument->GetFormatTable()->GetType(nFormat) == NUMBERFORMAT_DATE );
        if (bDate)
        {
            if (nCount > 1)
            {
                long nCmpInc = 0;
                double nVal;
                Date aNullDate = *pDocument->GetFormatTable()->GetNullDate();
                Date aDate1 = aNullDate;
                nVal = ((ScValueCell*)pFirstCell)->GetValue();
                aDate1 += (long)nVal;
                Date aDate2 = aNullDate;
                nVal = GetValue(nCol+nAddX, nRow+nAddY);
                aDate2 += (long)nVal;
                if ( aDate1 != aDate2 )
                {
                    FillDateCmd eType;
                    long nDDiff = aDate2.GetDay()   - (long) aDate1.GetDay();
                    long nMDiff = aDate2.GetMonth() - (long) aDate1.GetMonth();
                    long nYDiff = aDate2.GetYear()  - (long) aDate1.GetYear();
                    if ( nDDiff )
                    {
                        eType = FILL_DAY;
                        nCmpInc = aDate2 - aDate1;
                    }
                    else
                    {
                        eType = FILL_MONTH;
                        nCmpInc = nMDiff + 12 * nYDiff;
                    }

                    nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
                    nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
                    BOOL bVal = TRUE;
                    for (USHORT i=1; i<nCount && bVal; i++)
                    {
                        ScBaseCell* pCell = GetCell(nCol,nRow);
                        if (pCell && pCell->GetCellType() == CELLTYPE_VALUE)
                        {
                            nVal = ((ScValueCell*)pCell)->GetValue();
                            aDate2 = aNullDate + (long) nVal;
                            if ( eType == FILL_DAY )
                            {
                                if ( aDate2-aDate1 != nCmpInc )
                                    bVal = FALSE;
                            }
                            else
                            {
                                nDDiff = aDate2.GetDay()   - (long) aDate1.GetDay();
                                nMDiff = aDate2.GetMonth() - (long) aDate1.GetMonth();
                                nYDiff = aDate2.GetYear()  - (long) aDate1.GetYear();
                                if (nDDiff || ( nMDiff + 12 * nYDiff != nCmpInc ))
                                    bVal = FALSE;
                            }
                            aDate1 = aDate2;
                            nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
                            nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
                        }
                        else
                            bVal = FALSE;   // #50965# kein Datum passt auch nicht
                    }
                    if (bVal)
                    {
                        if ( eType == FILL_MONTH && ( nCmpInc % 12 == 0 ) )
                        {
                            eType = FILL_YEAR;
                            nCmpInc /= 12;
                        }
                        rCmd = FILL_DATE;
                        rDateCmd = eType;
                        rInc = nCmpInc;
                    }
                }
            }
            else                            // einzelnes Datum -> Tage hochzaehlen
            {
                rCmd = FILL_DATE;
                rDateCmd = FILL_DAY;
                rInc = 1.0;
            }
        }
        else
        {
            if (nCount > 1)
            {
                double nVal1 = ((ScValueCell*)pFirstCell)->GetValue();
                double nVal2 = GetValue(nCol+nAddX, nRow+nAddY);
                rInc = nVal2 - nVal1;
                nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
                nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
                BOOL bVal = TRUE;
                for (USHORT i=1; i<nCount && bVal; i++)
                {
                    ScBaseCell* pCell = GetCell(nCol,nRow);
                    if (pCell && pCell->GetCellType() == CELLTYPE_VALUE)
                    {
                        nVal2 = ((ScValueCell*)pCell)->GetValue();
                        double nDiff = nVal2 - nVal1;
                        if ( !::rtl::math::approxEqual( nDiff, rInc ) )
                            bVal = FALSE;
                        nVal1 = nVal2;
                    }
                    else
                        bVal = FALSE;
                    nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
                    nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
                }
                if (bVal)
                    rCmd = FILL_LINEAR;
            }
        }
    }
    else if (eCellType == CELLTYPE_STRING || eCellType == CELLTYPE_EDIT)
    {
        String aStr;
        GetString(nCol, nRow, aStr);
        rListData = (ScUserListData*)(ScGlobal::GetUserList()->GetData(aStr));
        if (rListData)
        {
            rListData->GetSubIndex(aStr, rListIndex);
            nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
            nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
            for (USHORT i=1; i<nCount && rListData; i++)
            {
                GetString(nCol, nRow, aStr);
                if (!rListData->GetSubIndex(aStr, rListIndex))
                    rListData = NULL;
                nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
                nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
            }
        }
        else if ( nCount > 1 )
        {
            //  pass rMinDigits to all DecompValueString calls
            //  -> longest number defines rMinDigits

            sal_Int32 nVal1;
            short nFlag1 = lcl_DecompValueString( aStr, nVal1, &rMinDigits );
            if ( nFlag1 )
            {
                sal_Int32 nVal2;
                GetString( nCol+nAddX, nRow+nAddY, aStr );
                short nFlag2 = lcl_DecompValueString( aStr, nVal2, &rMinDigits );
                if ( nFlag1 == nFlag2 )
                {
                    rInc = (double)nVal2 - (double)nVal1;
                    nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
                    nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
                    BOOL bVal = TRUE;
                    for (USHORT i=1; i<nCount && bVal; i++)
                    {
                        ScBaseCell* pCell = GetCell(nCol,nRow);
                        CellType eType = pCell ? pCell->GetCellType() : CELLTYPE_NONE;
                        if ( eType == CELLTYPE_STRING || eType == CELLTYPE_EDIT )
                        {
                            if ( eType == CELLTYPE_STRING )
                                ((ScStringCell*)pCell)->GetString( aStr );
                            else
                                ((ScEditCell*)pCell)->GetString( aStr );
                            nFlag2 = lcl_DecompValueString( aStr, nVal2, &rMinDigits );
                            if ( nFlag1 == nFlag2 )
                            {
                                double nDiff = (double)nVal2 - (double)nVal1;
                                if ( !::rtl::math::approxEqual( nDiff, rInc ) )
                                    bVal = FALSE;
                                nVal1 = nVal2;
                            }
                            else
                                bVal = FALSE;
                        }
                        else
                            bVal = FALSE;
                        nCol = sal::static_int_cast<SCCOL>( nCol + nAddX );
                        nRow = sal::static_int_cast<SCROW>( nRow + nAddY );
                    }
                    if (bVal)
                        rCmd = FILL_LINEAR;
                }
            }
        }
        else
        {
            //  call DecompValueString to set rMinDigits
            sal_Int32 nDummy;
            lcl_DecompValueString( aStr, nDummy, &rMinDigits );
        }
    }
}

void ScTable::FillFormula(ULONG& /* nFormulaCounter */, BOOL /* bFirst */, ScFormulaCell* pSrcCell,
                          SCCOL nDestCol, SCROW nDestRow, BOOL bLast )
{
/*  USHORT nTokArrLen = pSrcCell->GetTokenArrayLen();
    if ( nTokArrLen > 15 )                          // mehr als =A1 oder =67
    {
        ScRangeName* pRangeName = pDocument->GetRangeName();
        String aName("___SC_");                     // Wird dieser String veraendert,
                                                    // auch in document2 EraseNonUsed...
                                                    // mitaendern!!
        aName += pRangeName->GetSharedMaxIndex() + 1;
        aName += '_';
        aName += nFormulaCounter;
        nFormulaCounter++;
        if (bFirst)
        {
            ScRangeData *pAktRange = new ScRangeData(
                            pDocument, aName, pSrcCell->GetTokenArray(), nTokArrLen,
                            pSrcCell->GetCol(), pSrcCell->GetRow(), nTab ,RT_SHARED);
            if (!pRangeName->Insert( pAktRange ))
                delete pAktRange;
            else
                bSharedNameInserted = TRUE;
        }
        USHORT nIndex;
        pRangeName->SearchName(aName, nIndex);
        if (!pRangeName)
        {
            DBG_ERROR("ScTable::FillFormula: Falscher Name");
            return;
        }
        nIndex = ((ScRangeData*) ((*pRangeName)[nIndex]))->GetIndex();
        ScTokenArray aArr;
        aArr.AddName(nIndex);
        aArr.AddOpCode(ocStop);
        ScFormulaCell* pDestCell = new ScFormulaCell
            (pDocument, ScAddress( nDestCol, nDestRow, nTab ), aArr );
        aCol[nDestCol].Insert(nDestRow, pDestCell);
    }
    else
*/  {
        pDocument->SetNoListening( TRUE );  // noch falsche Referenzen
        ScFormulaCell* pDestCell = (ScFormulaCell*) pSrcCell->Clone( pDocument,
            ScAddress( nDestCol, nDestRow, nTab ), TRUE );
        aCol[nDestCol].Insert(nDestRow, pDestCell);
#if 0
// mit RelRefs unnoetig
        ScAddress aAddr( nDestCol, nDestRow, nTab );
        pDestCell->UpdateReference(URM_COPY,
                         ScRange( aAddr, aAddr ),
                         nDestCol - pSrcCell->aPos.Col(),
                         nDestRow - pSrcCell->aPos.Row(), 0);
#endif
        if ( bLast && pDestCell->GetMatrixFlag() )
        {
            ScAddress aOrg;
            if ( pDestCell->GetMatrixOrigin( aOrg ) )
            {
                if ( nDestCol >= aOrg.Col() && nDestRow >= aOrg.Row() )
                {
                    ScBaseCell* pOrgCell = pDocument->GetCell( aOrg );
                    if ( pOrgCell && pOrgCell->GetCellType() == CELLTYPE_FORMULA
                      && ((ScFormulaCell*)pOrgCell)->GetMatrixFlag() == MM_FORMULA )
                    {
                        ((ScFormulaCell*)pOrgCell)->SetMatColsRows(
                            nDestCol - aOrg.Col() + 1,
                            nDestRow - aOrg.Row() + 1 );
                    }
                    else
                    {
                        DBG_ERRORFILE( "FillFormula: MatrixOrigin keine Formelzelle mit MM_FORMULA" );
                    }
                }
                else
                {
                    DBG_ERRORFILE( "FillFormula: MatrixOrigin rechts unten" );
                }
            }
            else
            {
                DBG_ERRORFILE( "FillFormula: kein MatrixOrigin" );
            }
        }
        pDocument->SetNoListening( FALSE );
        pDestCell->StartListeningTo( pDocument );
    }
}

void ScTable::FillAuto( SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2,
                        ULONG nFillCount, FillDir eFillDir, ScProgress& rProgress )
{
    if ( (nFillCount == 0) || !ValidColRow(nCol1, nRow1) || !ValidColRow(nCol2, nRow2) )
        return;

    //
    //  Richtung auswerten
    //

    BOOL bVertical = (eFillDir == FILL_TO_BOTTOM || eFillDir == FILL_TO_TOP);
    BOOL bPositive = (eFillDir == FILL_TO_BOTTOM || eFillDir == FILL_TO_RIGHT);

    ULONG nCol = 0;
    ULONG nRow = 0;
    ULONG& rInner = bVertical ? nRow : nCol;        // Schleifenvariablen
    ULONG& rOuter = bVertical ? nCol : nRow;
    ULONG nOStart;
    ULONG nOEnd;
    ULONG nIStart;
    ULONG nIEnd;
    ULONG nISrcStart;
    ULONG nISrcEnd;

    if (bVertical)
    {
        nOStart = nCol1;
        nOEnd = nCol2;
        if (bPositive)
        {
            nISrcStart = nRow1;
            nISrcEnd = nRow2;
            nIStart = nRow2 + 1;
            nIEnd = nRow2 + nFillCount;
        }
        else
        {
            nISrcStart = nRow2;
            nISrcEnd = nRow1;
            nIStart = nRow1 - 1;
            nIEnd = nRow1 - nFillCount;
        }
    }
    else
    {
        nOStart = nRow1;
        nOEnd = nRow2;
        if (bPositive)
        {
            nISrcStart = nCol1;
            nISrcEnd = nCol2;
            nIStart = nCol2 + 1;
            nIEnd = nCol2 + nFillCount;
        }
        else
        {
            nISrcStart = nCol2;
            nISrcEnd = nCol1;
            nIStart = nCol1 - 1;
            nIEnd = nCol1 - nFillCount;
        }
    }
    ULONG nIMin = nIStart;
    ULONG nIMax = nIEnd;
    PutInOrder(nIMin,nIMax);
    if (bVertical)
        DeleteArea(nCol1, static_cast<SCROW>(nIMin), nCol2, static_cast<SCROW>(nIMax), IDF_ALL);
    else
        DeleteArea(static_cast<SCCOL>(nIMin), nRow1, static_cast<SCCOL>(nIMax), nRow2, IDF_ALL);

    ULONG nProgress = rProgress.GetState();

    //
    //  ausfuehren
    //

    ULONG nActFormCnt = 0;
    for (rOuter = nOStart; rOuter <= nOEnd; rOuter++)
    {
        ULONG nMaxFormCnt = 0;                      // fuer Formeln

        //  Attributierung uebertragen

        const ScPatternAttr* pSrcPattern = NULL;
        ULONG nAtSrc = nISrcStart;
        ScPatternAttr* pNewPattern = NULL;
        BOOL bGetPattern = TRUE;
        rInner = nIStart;
        while (true)        // #i53728# with "for (;;)" old solaris/x86 compiler mis-optimizes
        {
            const ScStyleSheet* pStyleSheet = NULL;
            if ( bGetPattern )
            {
                if ( pNewPattern )
                    delete pNewPattern;
                if (bVertical)      // rInner&:=nRow, rOuter&:=nCol
                    pSrcPattern = aCol[nCol].GetPattern(static_cast<SCROW>(nAtSrc));
                else                // rInner&:=nCol, rOuter&:=nRow
                    pSrcPattern = aCol[nAtSrc].GetPattern(static_cast<SCROW>(nRow));
                bGetPattern = FALSE;
                pStyleSheet = pSrcPattern->GetStyleSheet();
                //  Merge/Mergeflag nicht uebernehmen,
                const SfxItemSet& rSet = pSrcPattern->GetItemSet();
                if ( rSet.GetItemState(ATTR_MERGE, FALSE) == SFX_ITEM_SET
                  || rSet.GetItemState(ATTR_MERGE_FLAG, FALSE) == SFX_ITEM_SET )
                {
                    pNewPattern = new ScPatternAttr( *pSrcPattern );
                    SfxItemSet& rNewSet = pNewPattern->GetItemSet();
                    rNewSet.ClearItem(ATTR_MERGE);
                    rNewSet.ClearItem(ATTR_MERGE_FLAG);
                }
                else
                    pNewPattern = NULL;
            }

            if ( bVertical && nISrcStart == nISrcEnd )
            {
                //  Attribute komplett am Stueck setzen
                if (pNewPattern || pSrcPattern != pDocument->GetDefPattern())
                {
                    //  Default steht schon da (DeleteArea)
                    SCROW nY1 = static_cast<SCROW>(Min( nIStart, nIEnd ));
                    SCROW nY2 = static_cast<SCROW>(Max( nIStart, nIEnd ));
                    if ( pStyleSheet )
                        aCol[nCol].ApplyStyleArea( nY1, nY2, *pStyleSheet );
                    if ( pNewPattern )
                        aCol[nCol].ApplyPatternArea( nY1, nY2, *pNewPattern );
                    else
                        aCol[nCol].ApplyPatternArea( nY1, nY2, *pSrcPattern );
                }
                break;      // Schleife abbrechen
            }

            if ( pSrcPattern != aCol[nCol].GetPattern( static_cast<SCROW>(nRow) ) )
            {
                //  Vorlage auch uebernehmen
                //! am AttrArray mit ApplyPattern zusammenfassen ??
                if ( pStyleSheet )
                    aCol[nCol].ApplyStyle( static_cast<SCROW>(nRow), *pStyleSheet );

                //  ApplyPattern statt SetPattern um alte MergeFlags stehenzulassen
                if ( pNewPattern )
                    aCol[nCol].ApplyPattern( static_cast<SCROW>(nRow), *pNewPattern );
                else
                    aCol[nCol].ApplyPattern( static_cast<SCROW>(nRow), *pSrcPattern );
            }

            if (nAtSrc==nISrcEnd)
            {
                if ( nAtSrc != nISrcStart )
                {   // mehr als eine Source-Zelle
                    nAtSrc = nISrcStart;
                    bGetPattern = TRUE;
                }
            }
            else if (bPositive)
            {
                ++nAtSrc;
                bGetPattern = TRUE;
            }
            else
            {
                --nAtSrc;
                bGetPattern = TRUE;
            }

            if (rInner == nIEnd) break;
            if (bPositive) ++rInner; else --rInner;
        }
        if ( pNewPattern )
            delete pNewPattern;

        //  Analyse

        FillCmd eFillCmd;
        FillDateCmd eDateCmd;
        double nInc;
        USHORT nMinDigits;
        ScUserListData* pListData = NULL;
        USHORT nListIndex;
        if (bVertical)
            FillAnalyse(static_cast<SCCOL>(nCol),nRow1,
                    static_cast<SCCOL>(nCol),nRow2, eFillCmd,eDateCmd,
                    nInc,nMinDigits, pListData,nListIndex);
        else
            FillAnalyse(nCol1,static_cast<SCROW>(nRow),
                    nCol2,static_cast<SCROW>(nRow), eFillCmd,eDateCmd,
                    nInc,nMinDigits, pListData,nListIndex);

        if (bVertical)
            aCol[nCol].Resize( aCol[nCol].GetCellCount() + nFillCount );

        if (pListData)
        {
            USHORT nListCount = pListData->GetSubCount();
            if ( !bPositive )
            {
                //  nListIndex auf FillAnalyse zeigt auf den letzten Eintrag -> anpassen
                ULONG nSub = nISrcStart - nISrcEnd;
                for (ULONG i=0; i<nSub; i++)
                {
                    if (nListIndex == 0) nListIndex = nListCount;
                    --nListIndex;
                }
            }

            rInner = nIStart;
            while (true)        // #i53728# with "for (;;)" old solaris/x86 compiler mis-optimizes
            {
                if (bPositive)
                {
                    ++nListIndex;
                    if (nListIndex >= nListCount) nListIndex = 0;
                }
                else
                {
                    if (nListIndex == 0) nListIndex = nListCount;
                    --nListIndex;
                }
                aCol[nCol].Insert(static_cast<SCROW>(nRow), new ScStringCell(pListData->GetSubStr(nListIndex)));

                if (rInner == nIEnd) break;
                if (bPositive) ++rInner; else --rInner;
            }
            nProgress += nIMax - nIMin + 1;
            rProgress.SetStateOnPercent( nProgress );
        }
        else if (eFillCmd == FILL_SIMPLE)           // Auffuellen mit Muster
        {
            ULONG nSource = nISrcStart;
            double nDelta;
            if ( nScFillModeMouseModifier & KEY_MOD1 )
                nDelta = 0.0;
            else if ( bPositive )
                nDelta = 1.0;
            else
                nDelta = -1.0;
            double nVal = 0.0;
            ULONG nFormulaCounter = nActFormCnt;
            BOOL bFirst = TRUE;
            BOOL bGetCell = TRUE;
            USHORT nCellDigits = 0;
            short nHeadNoneTail = 0;
            sal_Int32 nStringValue = 0;
            String aValue;
            ScBaseCell* pSrcCell = NULL;
            CellType eCellType = CELLTYPE_NONE;
            BOOL bIsOrdinalSuffix = FALSE;

            rInner = nIStart;
            while (true)        // #i53728# with "for (;;)" old solaris/x86 compiler mis-optimizes
            {
                if ( bGetCell )
                {
                    if (bVertical)      // rInner&:=nRow, rOuter&:=nCol
                        pSrcCell = aCol[nCol].GetCell( static_cast<SCROW>(nSource) );
                    else                // rInner&:=nCol, rOuter&:=nRow
                        pSrcCell = aCol[nSource].GetCell( static_cast<SCROW>(nRow) );
                    bGetCell = FALSE;
                    if ( pSrcCell )
                    {
                        eCellType = pSrcCell->GetCellType();
                        switch ( eCellType )
                        {
                            case CELLTYPE_VALUE:
                                nVal = ((ScValueCell*)pSrcCell)->GetValue();
                            break;
                            case CELLTYPE_STRING:
                            case CELLTYPE_EDIT:
                                if ( eCellType == CELLTYPE_STRING )
                                    ((ScStringCell*)pSrcCell)->GetString( aValue );
                                else
                                    ((ScEditCell*)pSrcCell)->GetString( aValue );
                                if ( !(nScFillModeMouseModifier & KEY_MOD1) )
                                {
                                    nCellDigits = 0;    // look at each source cell individually
                                    nHeadNoneTail = lcl_DecompValueString(
                                        aValue, nStringValue, &nCellDigits );

                                    bIsOrdinalSuffix = aValue.Equals(
                                            ScGlobal::GetOrdinalSuffix( nStringValue));
                                }
                            break;
                            default:
                            {
                                // added to avoid warnings
                            }
                        }
                    }
                    else
                        eCellType = CELLTYPE_NONE;
                }
                switch (eCellType)
                {
                    case CELLTYPE_VALUE:
                        aCol[nCol].Insert(static_cast<SCROW>(nRow), new ScValueCell(nVal + nDelta));
                        break;
                    case CELLTYPE_STRING:
                    case CELLTYPE_EDIT:
                        if ( nHeadNoneTail )
                        {
                            // #i48009# with the "nStringValue+(long)nDelta" expression within the
                            // lcl_ValueString calls, gcc 3.4.1 makes wrong optimizations (ok in 3.4.3),
                            // so nNextValue is now calculated ahead.
                            sal_Int32 nNextValue = nStringValue+(sal_Int32)nDelta;

                            String aStr;
                            if ( nHeadNoneTail < 0 )
                            {
                                aCol[nCol].Insert( static_cast<SCROW>(nRow),
                                        lcl_getSuffixCell( pDocument,
                                            nNextValue, nCellDigits, aValue,
                                            eCellType, bIsOrdinalSuffix));
                            }
                            else
                            {
                                aStr = aValue;
                                aStr += lcl_ValueString( nNextValue, nCellDigits );
                                aCol[nCol].Insert( static_cast<SCROW>(nRow),
                                        new ScStringCell( aStr));
                            }
                        }
                        else
                        {
                            switch ( eCellType )
                            {
                                case CELLTYPE_STRING:
                                    aCol[nCol].Insert(static_cast<SCROW>(nRow),
                                        new ScStringCell(*(ScStringCell*)pSrcCell, pDocument));
                                break;
                                case CELLTYPE_EDIT:
                                    aCol[nCol].Insert(static_cast<SCROW>(nRow),
                                        new ScEditCell( *(ScEditCell*)pSrcCell,
                                            pDocument ));
                                break;
                                default:
                                {
                                    // added to avoid warnings
                                }
                            }
                        }
                        break;
                    case CELLTYPE_FORMULA :
                        FillFormula( nFormulaCounter, bFirst,
                                (ScFormulaCell*) pSrcCell,
                                static_cast<SCCOL>(nCol),
                                static_cast<SCROW>(nRow), (rInner == nIEnd) );
                        if (nFormulaCounter - nActFormCnt > nMaxFormCnt)
                            nMaxFormCnt = nFormulaCounter - nActFormCnt;
                        break;
                    default:
                    {
                        // added to avoid warnings
                    }
                }

                if (nSource==nISrcEnd)
                {
                    if ( nSource != nISrcStart )
                    {   // mehr als eine Source-Zelle
                        nSource = nISrcStart;
                        bGetCell = TRUE;
                    }
                    if ( !(nScFillModeMouseModifier & KEY_MOD1) )
                    {
                        if ( bPositive )
                            nDelta += 1.0;
                        else
                            nDelta -= 1.0;
                    }
                    nFormulaCounter = nActFormCnt;
                    bFirst = FALSE;
                }
                else if (bPositive)
                {
                    ++nSource;
                    bGetCell = TRUE;
                }
                else
                {
                    --nSource;
                    bGetCell = TRUE;
                }

                //  Progress in der inneren Schleife nur bei teuren Zellen,
                //  und auch dann nicht fuer jede einzelne

                ++nProgress;
                if ( eCellType == CELLTYPE_FORMULA || eCellType == CELLTYPE_EDIT )
                    rProgress.SetStateOnPercent( nProgress );

                if (rInner == nIEnd) break;
                if (bPositive) ++rInner; else --rInner;
            }
            rProgress.SetStateOnPercent( nProgress );
        }
        else
        {
            if (!bPositive)
                nInc = -nInc;
            double nEndVal = (nInc>=0.0) ? MAXDOUBLE : -MAXDOUBLE;
            if (bVertical)
                FillSeries( static_cast<SCCOL>(nCol), nRow1,
                        static_cast<SCCOL>(nCol), nRow2, nFillCount, eFillDir,
                        eFillCmd, eDateCmd, nInc, nEndVal, nMinDigits, FALSE,
                        rProgress );
            else
                FillSeries( nCol1, static_cast<SCROW>(nRow), nCol2,
                        static_cast<SCROW>(nRow), nFillCount, eFillDir,
                        eFillCmd, eDateCmd, nInc, nEndVal, nMinDigits, FALSE,
                        rProgress );
            nProgress = rProgress.GetState();
        }

        nActFormCnt += nMaxFormCnt;
    }
}

String ScTable::GetAutoFillPreview( const ScRange& rSource, SCCOL nEndX, SCROW nEndY )
{
    String aValue;

    SCCOL nCol1 = rSource.aStart.Col();
    SCROW nRow1 = rSource.aStart.Row();
    SCCOL nCol2 = rSource.aEnd.Col();
    SCROW nRow2 = rSource.aEnd.Row();
    BOOL bOk = TRUE;
    long nIndex = 0;
    ULONG nSrcCount = 0;
    FillDir eFillDir = FILL_TO_BOTTOM;
    if ( nEndX == nCol2 && nEndY == nRow2 )     // leer
        bOk = FALSE;
    else if ( nEndX == nCol2 )                  // nach oben/unten
    {
        nEndX = nCol2 = nCol1;                  // nur erste Spalte ansehen
        nSrcCount = nRow2 - nRow1 + 1;
        nIndex = ((long)nEndY) - nRow1;         // kann negativ werden
        if ( nEndY >= nRow1 )
            eFillDir = FILL_TO_BOTTOM;
        else
            eFillDir = FILL_TO_TOP;
    }
    else if ( nEndY == nRow2 )                  // nach links/rechts
    {
        nEndY = nRow2 = nRow1;                  // nur erste Zeile ansehen
        nSrcCount = nCol2 - nCol1 + 1;
        nIndex = ((long)nEndX) - nCol1;         // kann negativ werden
        if ( nEndX >= nCol1 )
            eFillDir = FILL_TO_RIGHT;
        else
            eFillDir = FILL_TO_LEFT;
    }
    else                                        // Richtung nicht eindeutig
        bOk = FALSE;

    if ( bOk )
    {
        FillCmd eFillCmd;
        FillDateCmd eDateCmd;
        double nInc;
        USHORT nMinDigits;
        ScUserListData* pListData = NULL;
        USHORT nListIndex;

        FillAnalyse(nCol1,nRow1, nCol2,nRow2, eFillCmd,eDateCmd, nInc,nMinDigits, pListData,nListIndex);

        if ( pListData )                            // benutzerdefinierte Liste
        {
            USHORT nListCount = pListData->GetSubCount();
            if ( nListCount )
            {
                ULONG nSub = nSrcCount - 1; //  nListIndex ist vom letzten Source-Eintrag
                while ( nIndex < sal::static_int_cast<long>(nSub) )
                    nIndex += nListCount;
                ULONG nPos = ( nListIndex + nIndex - nSub ) % nListCount;
                aValue = pListData->GetSubStr(sal::static_int_cast<USHORT>(nPos));
            }
        }
        else if ( eFillCmd == FILL_SIMPLE )         // Auffuellen mit Muster
        {
            long nPosIndex = nIndex;
            while ( nPosIndex < 0 )
                nPosIndex += nSrcCount;
            ULONG nPos = nPosIndex % nSrcCount;
            SCCOL nSrcX = nCol1;
            SCROW nSrcY = nRow1;
            if ( eFillDir == FILL_TO_TOP || eFillDir == FILL_TO_BOTTOM )
                nSrcY = sal::static_int_cast<SCROW>( nSrcY + static_cast<SCROW>(nPos) );
            else
                nSrcX = sal::static_int_cast<SCCOL>( nSrcX + static_cast<SCCOL>(nPos) );

            ScBaseCell* pCell = GetCell( nSrcX, nSrcY );
            if ( pCell )
            {
                sal_Int32 nDelta;
                if (nIndex >= 0)
                    nDelta = nIndex / nSrcCount;
                else
                    nDelta = ( nIndex - nSrcCount + 1 ) / nSrcCount;    // -1 -> -1

                CellType eType = pCell->GetCellType();
                switch ( eType )
                {
                    case CELLTYPE_STRING:
                    case CELLTYPE_EDIT:
                    {
                        if ( eType == CELLTYPE_STRING )
                            ((ScStringCell*)pCell)->GetString( aValue );
                        else
                            ((ScEditCell*)pCell)->GetString( aValue );
                        if ( !(nScFillModeMouseModifier & KEY_MOD1) )
                        {
                            sal_Int32 nVal;
                            USHORT nCellDigits = 0; // look at each source cell individually
                            short nFlag = lcl_DecompValueString( aValue, nVal, &nCellDigits );
                            if ( nFlag < 0 )
                            {
                                if (aValue.Equals( ScGlobal::GetOrdinalSuffix( nVal)))
                                    aValue = ScGlobal::GetOrdinalSuffix( nVal + nDelta);

                                aValue.Insert( lcl_ValueString( nVal + nDelta, nCellDigits ), 0 );
                            }
                            else if ( nFlag > 0 )
                                aValue += lcl_ValueString( nVal + nDelta, nCellDigits );
                        }
                    }
                    break;
                    case CELLTYPE_VALUE:
                    {
                        //  dabei kann's keinen Ueberlauf geben...
                        double nVal = ((ScValueCell*)pCell)->GetValue();
                        if ( !(nScFillModeMouseModifier & KEY_MOD1) )
                            nVal += (double) nDelta;

                        Color* pColor;
                        ULONG nNumFmt = GetNumberFormat( nSrcX, nSrcY );
                        pDocument->GetFormatTable()->
                            GetOutputString( nVal, nNumFmt, aValue, &pColor );
                    }
                    break;
                    //  Formeln nicht
                    default:
                    {
                        // added to avoid warnings
                    }
                }
            }
        }
        else if ( eFillCmd == FILL_LINEAR || eFillCmd == FILL_DATE )        // Werte
        {
            BOOL bValueOk;
            double nStart;
            sal_Int32 nVal = 0;
            short nHeadNoneTail = 0;
            ScBaseCell* pCell = GetCell( nCol1, nRow1 );
            if ( pCell )
            {
                CellType eType = pCell->GetCellType();
                switch ( eType )
                {
                    case CELLTYPE_STRING:
                    case CELLTYPE_EDIT:
                    {
                        if ( eType == CELLTYPE_STRING )
                            ((ScStringCell*)pCell)->GetString( aValue );
                        else
                            ((ScEditCell*)pCell)->GetString( aValue );
                        nHeadNoneTail = lcl_DecompValueString( aValue, nVal );
                        if ( nHeadNoneTail )
                            nStart = (double)nVal;
                        else
                            nStart = 0.0;
                    }
                    break;
                    case CELLTYPE_VALUE:
                        nStart = ((ScValueCell*)pCell)->GetValue();
                    break;
                    case CELLTYPE_FORMULA:
                        nStart = ((ScFormulaCell*)pCell)->GetValue();
                    break;
                    default:
                        nStart = 0.0;
                }
            }
            else
                nStart = 0.0;
            if ( eFillCmd == FILL_LINEAR )
            {
                double nAdd = nInc;
                bValueOk = ( SubTotal::SafeMult( nAdd, (double) nIndex ) &&
                             SubTotal::SafePlus( nStart, nAdd ) );
            }
            else        // Datum
            {
                bValueOk = TRUE;
                USHORT nDayOfMonth = 0;
                if ( nIndex < 0 )
                {
                    nIndex = -nIndex;
                    nInc = -nInc;
                }
                for (long i=0; i<nIndex; i++)
                    IncDate( nStart, nDayOfMonth, nInc, eDateCmd );
            }

            if (bValueOk)
            {
                if ( nHeadNoneTail )
                {
                    if ( nHeadNoneTail < 0 )
                    {
                        if (aValue.Equals( ScGlobal::GetOrdinalSuffix( nVal)))
                            aValue = ScGlobal::GetOrdinalSuffix( (sal_Int32)nStart );

                        aValue.Insert( lcl_ValueString( (sal_Int32)nStart, nMinDigits ), 0 );
                    }
                    else
                        aValue += lcl_ValueString( (sal_Int32)nStart, nMinDigits );
                }
                else
                {
                    //! Zahlformat je nach Index holen?
                    Color* pColor;
                    ULONG nNumFmt = GetNumberFormat( nCol1, nRow1 );
                    pDocument->GetFormatTable()->
                        GetOutputString( nStart, nNumFmt, aValue, &pColor );
                }
            }
        }
        else
            DBG_ERROR("GetAutoFillPreview: falscher Modus");
    }

    return aValue;
}

void ScTable::IncDate(double& rVal, USHORT& nDayOfMonth, double nStep, FillDateCmd eCmd)
{
    if (eCmd == FILL_DAY)
    {
        rVal += nStep;
        return;
    }

    // class Date Grenzen
    const USHORT nMinYear = 1583;
    const USHORT nMaxYear = 9956;

    long nInc = (long) nStep;       // nach oben/unten begrenzen ?
    Date aNullDate = *pDocument->GetFormatTable()->GetNullDate();
    Date aDate = aNullDate;
    aDate += (long)rVal;
    switch (eCmd)
    {
        case FILL_WEEKDAY:
            {
                aDate += nInc;
                DayOfWeek eWeekDay = aDate.GetDayOfWeek();
                if (nInc >= 0)
                {
                    if (eWeekDay == SATURDAY)
                        aDate += 2;
                    else if (eWeekDay == SUNDAY)
                        aDate += 1;
                }
                else
                {
                    if (eWeekDay == SATURDAY)
                        aDate -= 1;
                    else if (eWeekDay == SUNDAY)
                        aDate -= 2;
                }
            }
            break;
        case FILL_MONTH:
            {
                if ( nDayOfMonth == 0 )
                    nDayOfMonth = aDate.GetDay();       // init
                long nMonth = aDate.GetMonth();
                long nYear = aDate.GetYear();

                nMonth += nInc;

                if (nInc >= 0)
                {
                    if (nMonth > 12)
                    {
                        long nYAdd = (nMonth-1) / 12;
                        nMonth -= nYAdd * 12;
                        nYear += nYAdd;
                    }
                }
                else
                {
                    if (nMonth < 1)
                    {
                        long nYAdd = 1 - nMonth / 12;       // positiv
                        nMonth += nYAdd * 12;
                        nYear -= nYAdd;
                    }
                }

                if ( nYear < nMinYear )
                    aDate = Date( 1,1, nMinYear );
                else if ( nYear > nMaxYear )
                    aDate = Date( 31,12, nMaxYear );
                else
                {
                    aDate.SetMonth((USHORT) nMonth);
                    aDate.SetYear((USHORT) nYear);
                    if ( nDayOfMonth > 28 )
                        aDate.SetDay( Min( aDate.GetDaysInMonth(), nDayOfMonth ) );
                }
            }
            break;
        case FILL_YEAR:
            {
                long nYear = aDate.GetYear();
                nYear += nInc;
                if ( nYear < nMinYear )
                    aDate = Date( 1,1, nMinYear );
                else if ( nYear > nMaxYear )
                    aDate = Date( 31,12, nMaxYear );
                else
                    aDate.SetYear((USHORT) nYear);
            }
            break;
        default:
        {
            // added to avoid warnings
        }
    }

    rVal = aDate - aNullDate;
}

void ScTable::FillSeries( SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2,
                    ULONG nFillCount, FillDir eFillDir, FillCmd eFillCmd, FillDateCmd eFillDateCmd,
                    double nStepValue, double nMaxValue, USHORT nArgMinDigits,
                    BOOL bAttribs, ScProgress& rProgress )
{
    //
    //  Richtung auswerten
    //

    BOOL bVertical = (eFillDir == FILL_TO_BOTTOM || eFillDir == FILL_TO_TOP);
    BOOL bPositive = (eFillDir == FILL_TO_BOTTOM || eFillDir == FILL_TO_RIGHT);

    ULONG nCol = 0;
    ULONG nRow = 0;
    ULONG& rInner = bVertical ? nRow : nCol;        // Schleifenvariablen
    ULONG& rOuter = bVertical ? nCol : nRow;
    ULONG nOStart;
    ULONG nOEnd;
    ULONG nIStart;
    ULONG nIEnd;
    ULONG nISource;

    if (bVertical)
    {
        nFillCount += (nRow2 - nRow1);
        if (nFillCount == 0)
            return;
        nOStart = nCol1;
        nOEnd = nCol2;
        if (bPositive)
        {
            nISource = nRow1;
            nIStart = nRow1 + 1;
            nIEnd = nRow1 + nFillCount;
        }
        else
        {
            nISource = nRow2;
            nIStart = nRow2 - 1;
            nIEnd = nRow2 - nFillCount;
        }
    }
    else
    {
        nFillCount += (nCol2 - nCol1);
        if (nFillCount == 0)
            return;
        nOStart = nRow1;
        nOEnd = nRow2;
        if (bPositive)
        {
            nISource = nCol1;
            nIStart = nCol1 + 1;
            nIEnd = nCol1 + nFillCount;
        }
        else
        {
            nISource = nCol2;
            nIStart = nCol2 - 1;
            nIEnd = nCol2 - nFillCount;
        }
    }

    ULONG nIMin = nIStart;
    ULONG nIMax = nIEnd;
    PutInOrder(nIMin,nIMax);
    USHORT nDel = bAttribs ? IDF_ALL : IDF_CONTENTS;
    if (bVertical)
        DeleteArea(nCol1, static_cast<SCROW>(nIMin), nCol2, static_cast<SCROW>(nIMax), nDel);
    else
        DeleteArea(static_cast<SCCOL>(nIMin), nRow1, static_cast<SCCOL>(nIMax), nRow2, nDel);

    ULONG nProgress = rProgress.GetState();

    //
    //  ausfuehren
    //

    ULONG nActFormCnt = 0;
    for (rOuter = nOStart; rOuter <= nOEnd; rOuter++)
    {
        BOOL bFirst = TRUE;
        rInner = nISource;
        ScBaseCell* pSrcCell = aCol[nCol].GetCell(static_cast<SCROW>(nRow));

        if (bVertical && bAttribs)
            aCol[nCol].Resize( aCol[nCol].GetCellCount() + nFillCount );

        if (bAttribs)
        {
            const ScPatternAttr* pSrcPattern = aCol[nCol].GetPattern(static_cast<SCROW>(nRow));
            if (bVertical)
                aCol[nCol].SetPatternArea( static_cast<SCROW>(nIMin),
                        static_cast<SCROW>(nIMax), *pSrcPattern, TRUE );
            else
                for (SCCOL nAtCol = static_cast<SCCOL>(nIMin); nAtCol <= sal::static_int_cast<SCCOL>(nIMax); nAtCol++)
                    aCol[nAtCol].SetPattern(static_cast<SCROW>(nRow), *pSrcPattern, TRUE);
        }

        if (pSrcCell)
        {
            CellType eCellType = pSrcCell->GetCellType();

            if (eFillCmd == FILL_SIMPLE)                // kopieren
            {
                if (eCellType == CELLTYPE_FORMULA)
                {
                    for (rInner = nIMin; rInner <= nIMax; rInner++)
                    {
                        ULONG nInd = nActFormCnt;
                        FillFormula(nInd, bFirst, (ScFormulaCell*)pSrcCell,
                            static_cast<SCCOL>(nCol), nRow, (rInner == nIEnd) );
                        bFirst = FALSE;
                        rProgress.SetStateOnPercent( ++nProgress );
                    }
                }
                else if (eCellType != CELLTYPE_NOTE)
                {
                    for (rInner = nIMin; rInner <= nIMax; rInner++)
                    {
                        aCol[nCol].Insert(static_cast<SCROW>(nRow), pSrcCell->Clone(pDocument));
                    }
                    nProgress += nIMax - nIMin + 1;
                    rProgress.SetStateOnPercent( nProgress );
                }
            }
            else if (eCellType == CELLTYPE_VALUE || eCellType == CELLTYPE_FORMULA)
            {
                double nStartVal;
                if (eCellType == CELLTYPE_VALUE)
                    nStartVal = ((ScValueCell*)pSrcCell)->GetValue();
                else
                    nStartVal = ((ScFormulaCell*)pSrcCell)->GetValue();
                double nVal = nStartVal;
                long nIndex = 0;

                BOOL bError = FALSE;
                BOOL bOverflow = FALSE;

                USHORT nDayOfMonth = 0;
                rInner = nIStart;
                while (true)        // #i53728# with "for (;;)" old solaris/x86 compiler mis-optimizes
                {
                    if (!bError && !bOverflow)
                    {
                        switch (eFillCmd)
                        {
                            case FILL_LINEAR:
                                {
                                    //  #86365# use multiplication instead of repeated addition
                                    //  to avoid accumulating rounding errors
                                    nVal = nStartVal;
                                    double nAdd = nStepValue;
                                    if ( !SubTotal::SafeMult( nAdd, (double) ++nIndex ) ||
                                         !SubTotal::SafePlus( nVal, nAdd ) )
                                        bError = TRUE;
                                }
                                break;
                            case FILL_GROWTH:
                                if (!SubTotal::SafeMult(nVal, nStepValue))
                                    bError = TRUE;
                                break;
                            case FILL_DATE:
                                if (fabs(nVal) > _D_MAX_LONG_)
                                    bError = TRUE;
                                else
                                    IncDate(nVal, nDayOfMonth, nStepValue, eFillDateCmd);
                                break;
                            default:
                            {
                                // added to avoid warnings
                            }
                        }

                        if (nStepValue >= 0)
                        {
                            if (nVal > nMaxValue)           // Zielwert erreicht?
                            {
                                nVal = nMaxValue;
                                bOverflow = TRUE;
                            }
                        }
                        else
                        {
                            if (nVal < nMaxValue)
                            {
                                nVal = nMaxValue;
                                bOverflow = TRUE;
                            }
                        }
                    }

                    if (bError)
                        aCol[nCol].SetError(static_cast<SCROW>(nRow), errNoValue);
                    else if (!bOverflow)
                        aCol[nCol].SetValue(static_cast<SCROW>(nRow), nVal);

                    if (rInner == nIEnd) break;
                    if (bPositive) ++rInner; else --rInner;
                }
                nProgress += nIMax - nIMin + 1;
                rProgress.SetStateOnPercent( nProgress );
            }
            else if (eCellType == CELLTYPE_STRING || eCellType == CELLTYPE_EDIT)
            {
                if ( nStepValue >= 0 )
                {
                    if ( nMaxValue >= (double)LONG_MAX )
                        nMaxValue = (double)LONG_MAX - 1;
                }
                else
                {
                    if ( nMaxValue <= (double)LONG_MIN )
                        nMaxValue = (double)LONG_MIN + 1;
                }
                String aValue;
                if (eCellType == CELLTYPE_STRING)
                    ((ScStringCell*)pSrcCell)->GetString( aValue );
                else
                    ((ScEditCell*)pSrcCell)->GetString( aValue );
                sal_Int32 nStringValue;
                USHORT nMinDigits = nArgMinDigits;
                short nHeadNoneTail = lcl_DecompValueString( aValue, nStringValue, &nMinDigits );
                if ( nHeadNoneTail )
                {
                    double nStartVal = (double)nStringValue;
                    double nVal = nStartVal;
                    long nIndex = 0;
                    BOOL bError = FALSE;
                    BOOL bOverflow = FALSE;

                    BOOL bIsOrdinalSuffix = aValue.Equals( ScGlobal::GetOrdinalSuffix(
                                (sal_Int32)nStartVal));

                    rInner = nIStart;
                    while (true)        // #i53728# with "for (;;)" old solaris/x86 compiler mis-optimizes
                    {
                        if (!bError && !bOverflow)
                        {
                            switch (eFillCmd)
                            {
                                case FILL_LINEAR:
                                    {
                                        //  #86365# use multiplication instead of repeated addition
                                        //  to avoid accumulating rounding errors
                                        nVal = nStartVal;
                                        double nAdd = nStepValue;
                                        if ( !SubTotal::SafeMult( nAdd, (double) ++nIndex ) ||
                                             !SubTotal::SafePlus( nVal, nAdd ) )
                                            bError = TRUE;
                                    }
                                    break;
                                case FILL_GROWTH:
                                    if (!SubTotal::SafeMult(nVal, nStepValue))
                                        bError = TRUE;
                                    break;
                                default:
                                {
                                    // added to avoid warnings
                                }
                            }

                            if (nStepValue >= 0)
                            {
                                if (nVal > nMaxValue)           // Zielwert erreicht?
                                {
                                    nVal = nMaxValue;
                                    bOverflow = TRUE;
                                }
                            }
                            else
                            {
                                if (nVal < nMaxValue)
                                {
                                    nVal = nMaxValue;
                                    bOverflow = TRUE;
                                }
                            }
                        }

                        if (bError)
                            aCol[nCol].SetError(static_cast<SCROW>(nRow), errNoValue);
                        else if (!bOverflow)
                        {
                            nStringValue = (sal_Int32)nVal;
                            String aStr;
                            if ( nHeadNoneTail < 0 )
                            {
                                aCol[nCol].Insert( static_cast<SCROW>(nRow),
                                        lcl_getSuffixCell( pDocument,
                                            nStringValue, nMinDigits, aValue,
                                            eCellType, bIsOrdinalSuffix ));
                            }
                            else
                            {
                                aStr = aValue;
                                aStr += lcl_ValueString( nStringValue, nMinDigits );
                                ScStringCell* pCell = new ScStringCell( aStr );
                                aCol[nCol].Insert( static_cast<SCROW>(nRow), pCell );
                            }
                        }

                        if (rInner == nIEnd) break;
                        if (bPositive) ++rInner; else --rInner;
                    }
                }
                nProgress += nIMax - nIMin + 1;
                rProgress.SetStateOnPercent( nProgress );
            }
        }
        else
        {
            nProgress += nIMax - nIMin + 1;
            rProgress.SetStateOnPercent( nProgress );
        }
        ++nActFormCnt;
    }
}

void ScTable::Fill( SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2,
                    ULONG nFillCount, FillDir eFillDir, FillCmd eFillCmd, FillDateCmd eFillDateCmd,
                    double nStepValue, double nMaxValue)
{
    ULONG nProgCount;
    if (eFillDir == FILL_TO_BOTTOM || eFillDir == FILL_TO_TOP)
        nProgCount = nCol2 - nCol1 + 1;
    else
        nProgCount = nRow2 - nRow1 + 1;
    nProgCount *= nFillCount;
    ScProgress aProgress( pDocument->GetDocumentShell(),
                            ScGlobal::GetRscString(STR_FILL_SERIES_PROGRESS), nProgCount );

    bSharedNameInserted = FALSE;

    if (eFillCmd == FILL_AUTO)
        FillAuto(nCol1, nRow1, nCol2, nRow2, nFillCount, eFillDir, aProgress);
    else
        FillSeries(nCol1, nRow1, nCol2, nRow2, nFillCount, eFillDir,
                    eFillCmd, eFillDateCmd, nStepValue, nMaxValue, 0, TRUE, aProgress);

    if (bSharedNameInserted)                        // Wurde Shared-Name eingefuegt?
        pDocument->GetRangeName()->SetSharedMaxIndex(
            pDocument->GetRangeName()->GetSharedMaxIndex()+1);  // dann hochzaehlen
}


void ScTable::AutoFormatArea(SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow,
                                const ScPatternAttr& rAttr, USHORT nFormatNo)
{
    ScAutoFormat* pAutoFormat = ScGlobal::GetAutoFormat();
    if (pAutoFormat)
    {
        ScAutoFormatData* pData = (*pAutoFormat)[nFormatNo];
        if (pData)
        {
//          ScPatternAttr aPattern(pDocument->GetPool());
//            pData->FillToItemSet(nIndex, aPattern.GetItemSet(), *pDocument);
            ApplyPatternArea(nStartCol, nStartRow, nEndCol, nEndRow, rAttr);
        }
    }
}

void ScTable::AutoFormat( SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow,
                            USHORT nFormatNo )
{
    if (ValidColRow(nStartCol, nStartRow) && ValidColRow(nEndCol, nEndRow))
    {
        ScAutoFormat* pAutoFormat = ScGlobal::GetAutoFormat();
        if (pAutoFormat)
        {
            ScAutoFormatData* pData = (*pAutoFormat)[nFormatNo];
            if (pData)
            {
                ScPatternAttr* pPatternAttrs[16];
                for (sal_uInt8 i = 0; i < 16; ++i)
                {
                    pPatternAttrs[i] = new ScPatternAttr(pDocument->GetPool());
                    pData->FillToItemSet(i, pPatternAttrs[i]->GetItemSet(), *pDocument);
                }

                SCCOL nCol = nStartCol;
                SCROW nRow = nStartRow;
                USHORT nIndex = 0;
                // Linke obere Ecke
                AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                // Linke Spalte
                if (pData->IsEqualData(4, 8))
                    AutoFormatArea(nStartCol, nStartRow + 1, nStartCol, nEndRow - 1, *pPatternAttrs[4], nFormatNo);
                else
                {
                    nIndex = 4;
                    for (nRow = nStartRow + 1; nRow < nEndRow; nRow++)
                    {
                        AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                        if (nIndex == 4)
                            nIndex = 8;
                        else
                            nIndex = 4;
                    }
                }
                // Linke untere Ecke
                nRow = nEndRow;
                nIndex = 12;
                AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                // Rechte obere Ecke
                nCol = nEndCol;
                nRow = nStartRow;
                nIndex = 3;
                AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                // Rechte Spalte
                if (pData->IsEqualData(7, 11))
                    AutoFormatArea(nEndCol, nStartRow + 1, nEndCol, nEndRow - 1, *pPatternAttrs[7], nFormatNo);
                else
                {
                    nIndex = 7;
                    for (nRow = nStartRow + 1; nRow < nEndRow; nRow++)
                    {
                        AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                        if (nIndex == 7)
                            nIndex = 11;
                        else
                            nIndex = 7;
                    }
                }
                // Rechte untere Ecke
                nRow = nEndRow;
                nIndex = 15;
                AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                nRow = nStartRow;
                nIndex = 1;
                for (nCol = nStartCol + 1; nCol < nEndCol; nCol++)
                {
                    AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                    if (nIndex == 1)
                        nIndex = 2;
                    else
                        nIndex = 1;
                }
                // Untere Zeile
                nRow = nEndRow;
                nIndex = 13;
                for (nCol = nStartCol + 1; nCol < nEndCol; nCol++)
                {
                    AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                    if (nIndex == 13)
                        nIndex = 14;
                    else
                        nIndex = 13;
                }
                // Boddy
                if ((pData->IsEqualData(5, 6)) && (pData->IsEqualData(9, 10)) && (pData->IsEqualData(5, 9)))
                    AutoFormatArea(nStartCol + 1, nStartRow + 1, nEndCol-1, nEndRow - 1, *pPatternAttrs[5], nFormatNo);
                else
                {
                    if ((pData->IsEqualData(5, 9)) && (pData->IsEqualData(6, 10)))
                    {
                        nIndex = 5;
                        for (nCol = nStartCol + 1; nCol < nEndCol; nCol++)
                        {
                            AutoFormatArea(nCol, nStartRow + 1, nCol, nEndRow - 1, *pPatternAttrs[nIndex], nFormatNo);
                            if (nIndex == 5)
                                nIndex = 6;
                            else
                                nIndex = 5;
                        }
                    }
                    else
                    {
                        nIndex = 5;
                        for (nCol = nStartCol + 1; nCol < nEndCol; nCol++)
                        {
                            for (nRow = nStartRow + 1; nRow < nEndRow; nRow++)
                            {
                                AutoFormatArea(nCol, nRow, nCol, nRow, *pPatternAttrs[nIndex], nFormatNo);
                                if ((nIndex == 5) || (nIndex == 9))
                                {
                                    if (nIndex == 5)
                                        nIndex = 9;
                                    else
                                        nIndex = 5;
                                }
                                else
                                {
                                    if (nIndex == 6)
                                        nIndex = 10;
                                    else
                                        nIndex = 6;
                                }
                            } // for nRow
                            if ((nIndex == 5) || (nIndex == 9))
                                nIndex = 6;
                            else
                                nIndex = 5;
                        } // for nCol
                    } // if not equal Column
                } // if not all equal

                for (sal_uInt8 j = 0; j < 16; ++j)
                    delete pPatternAttrs[j];
            } // if AutoFormatData != NULL
        } // if AutoFormat != NULL
    } // if ValidColRow
}

void ScTable::GetAutoFormatAttr(SCCOL nCol, SCROW nRow, USHORT nIndex, ScAutoFormatData& rData)
{
    UINT32 nFormatIndex = GetNumberFormat( nCol, nRow );
    ScNumFormatAbbrev   aNumFormat( nFormatIndex, *pDocument->GetFormatTable() );
    rData.GetFromItemSet( nIndex, GetPattern( nCol, nRow )->GetItemSet(), aNumFormat );
}

#define LF_LEFT         1
#define LF_TOP          2
#define LF_RIGHT        4
#define LF_BOTTOM       8
#define LF_ALL          (LF_LEFT | LF_TOP | LF_RIGHT | LF_BOTTOM)

void ScTable::GetAutoFormatFrame(SCCOL nCol, SCROW nRow, USHORT nFlags, USHORT nIndex, ScAutoFormatData& rData)
{
    const SvxBoxItem* pTheBox = (SvxBoxItem*)GetAttr(nCol, nRow, ATTR_BORDER);
    const SvxBoxItem* pLeftBox = (SvxBoxItem*)GetAttr(nCol - 1, nRow, ATTR_BORDER);
    const SvxBoxItem* pTopBox = (SvxBoxItem*)GetAttr(nCol, nRow - 1, ATTR_BORDER);
    const SvxBoxItem* pRightBox = (SvxBoxItem*)GetAttr(nCol + 1, nRow, ATTR_BORDER);
    const SvxBoxItem* pBottomBox = (SvxBoxItem*)GetAttr(nCol, nRow + 1, ATTR_BORDER);

    SvxBoxItem aBox;
    if (nFlags & LF_LEFT)
    {
        if (pLeftBox)
        {
            if (HasPriority(pTheBox->GetLeft(), pLeftBox->GetRight()))
                aBox.SetLine(pTheBox->GetLeft(), BOX_LINE_LEFT);
            else
                aBox.SetLine(pLeftBox->GetRight(), BOX_LINE_LEFT);
        }
        else
            aBox.SetLine(pTheBox->GetLeft(), BOX_LINE_LEFT);
    }
    if (nFlags & LF_TOP)
    {
        if (pTopBox)
        {
            if (HasPriority(pTheBox->GetTop(), pTopBox->GetBottom()))
                aBox.SetLine(pTheBox->GetTop(), BOX_LINE_TOP);
            else
                aBox.SetLine(pTopBox->GetBottom(), BOX_LINE_TOP);
        }
        else
            aBox.SetLine(pTheBox->GetTop(), BOX_LINE_TOP);
    }
    if (nFlags & LF_RIGHT)
    {
        if (pRightBox)
        {
            if (HasPriority(pTheBox->GetRight(), pRightBox->GetLeft()))
                aBox.SetLine(pTheBox->GetRight(), BOX_LINE_RIGHT);
            else
                aBox.SetLine(pRightBox->GetLeft(), BOX_LINE_RIGHT);
        }
        else
            aBox.SetLine(pTheBox->GetRight(), BOX_LINE_RIGHT);
    }
    if (nFlags & LF_BOTTOM)
    {
        if (pBottomBox)
        {
            if (HasPriority(pTheBox->GetBottom(), pBottomBox->GetTop()))
                aBox.SetLine(pTheBox->GetBottom(), BOX_LINE_BOTTOM);
            else
                aBox.SetLine(pBottomBox->GetTop(), BOX_LINE_BOTTOM);
        }
        else
            aBox.SetLine(pTheBox->GetBottom(), BOX_LINE_BOTTOM);
    }
    rData.PutItem( nIndex, aBox );
}

void ScTable::GetAutoFormatData(SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow, ScAutoFormatData& rData)
{
    if (ValidColRow(nStartCol, nStartRow) && ValidColRow(nEndCol, nEndRow))
    {
        if ((nEndCol - nStartCol >= 3) && (nEndRow - nStartRow >= 3))
        {
            // Linke obere Ecke
            GetAutoFormatAttr(nStartCol, nStartRow, 0, rData);
            GetAutoFormatFrame(nStartCol, nStartRow, LF_ALL, 0, rData);
            // Linke Spalte
            GetAutoFormatAttr(nStartCol, nStartRow + 1, 4, rData);
            GetAutoFormatAttr(nStartCol, nStartRow + 2, 8, rData);
            GetAutoFormatFrame(nStartCol, nStartRow + 1, LF_LEFT | LF_RIGHT | LF_BOTTOM, 4, rData);
            if (nEndRow - nStartRow >= 4)
                GetAutoFormatFrame(nStartCol, nStartRow + 2, LF_LEFT | LF_RIGHT | LF_BOTTOM, 8, rData);
            else
                rData.CopyItem( 8, 4, ATTR_BORDER );
            // Linke untere Ecke
            GetAutoFormatAttr(nStartCol, nEndRow, 12, rData);
            GetAutoFormatFrame(nStartCol, nEndRow, LF_ALL, 12, rData);
            // Rechte obere Ecke
            GetAutoFormatAttr(nEndCol, nStartRow, 3, rData);
            GetAutoFormatFrame(nEndCol, nStartRow, LF_ALL, 3, rData);
            // Rechte Spalte
            GetAutoFormatAttr(nEndCol, nStartRow + 1, 7, rData);
            GetAutoFormatAttr(nEndCol, nStartRow + 2, 11, rData);
            GetAutoFormatFrame(nEndCol, nStartRow + 1, LF_LEFT | LF_RIGHT | LF_BOTTOM, 7, rData);
            if (nEndRow - nStartRow >= 4)
                GetAutoFormatFrame(nEndCol, nStartRow + 2, LF_LEFT | LF_RIGHT | LF_BOTTOM, 11, rData);
            else
                rData.CopyItem( 11, 7, ATTR_BORDER );
            // Rechte untere Ecke
            GetAutoFormatAttr(nEndCol, nEndRow, 15, rData);
            GetAutoFormatFrame(nEndCol, nEndRow, LF_ALL, 15, rData);
            // Ober Zeile
            GetAutoFormatAttr(nStartCol + 1, nStartRow, 1, rData);
            GetAutoFormatAttr(nStartCol + 2, nStartRow, 2, rData);
            GetAutoFormatFrame(nStartCol + 1, nStartRow, LF_TOP | LF_BOTTOM | LF_RIGHT, 1, rData);
            if (nEndCol - nStartCol >= 4)
                GetAutoFormatFrame(nStartCol + 2, nStartRow, LF_TOP | LF_BOTTOM | LF_RIGHT, 2, rData);
            else
                rData.CopyItem( 2, 1, ATTR_BORDER );
            // Untere Zeile
            GetAutoFormatAttr(nStartCol + 1, nEndRow, 13, rData);
            GetAutoFormatAttr(nStartCol + 2, nEndRow, 14, rData);
            GetAutoFormatFrame(nStartCol + 1, nEndRow, LF_TOP | LF_BOTTOM | LF_RIGHT, 13, rData);
            if (nEndCol - nStartCol >= 4)
                GetAutoFormatFrame(nStartCol + 2, nEndRow, LF_TOP | LF_BOTTOM | LF_RIGHT, 14, rData);
            else
                rData.CopyItem( 14, 13, ATTR_BORDER );
            // Body
            GetAutoFormatAttr(nStartCol + 1, nStartRow + 1, 5, rData);
            GetAutoFormatAttr(nStartCol + 2, nStartRow + 1, 6, rData);
            GetAutoFormatAttr(nStartCol + 1, nStartRow + 2, 9, rData);
            GetAutoFormatAttr(nStartCol + 2, nStartRow + 2, 10, rData);
            GetAutoFormatFrame(nStartCol + 1, nStartRow + 1, LF_RIGHT | LF_BOTTOM, 5, rData);
            if ((nEndCol - nStartCol >= 4) && (nEndRow - nStartRow >= 4))
            {
                GetAutoFormatFrame(nStartCol + 2, nStartRow + 1, LF_RIGHT | LF_BOTTOM, 6, rData);
                GetAutoFormatFrame(nStartCol + 1, nStartRow + 2, LF_RIGHT | LF_BOTTOM, 9, rData);
                GetAutoFormatFrame(nStartCol + 2, nStartRow + 2, LF_RIGHT | LF_BOTTOM, 10, rData);
            }
            else
            {
                rData.CopyItem( 6, 5, ATTR_BORDER );
                rData.CopyItem( 9, 5, ATTR_BORDER );
                rData.CopyItem( 10, 5, ATTR_BORDER );
            }
        }
    }
}

void ScTable::SetError( SCCOL nCol, SCROW nRow, USHORT nError)
{
    if (ValidColRow(nCol, nRow))
        aCol[nCol].SetError( nRow, nError );
}

void ScTable::UpdateInsertTabAbs(SCTAB nTable)
{
    for (SCCOL i=0; i <= MAXCOL; i++)
        aCol[i].UpdateInsertTabAbs(nTable);
}

USHORT ScTable::GetErrorData( SCCOL nCol, SCROW nRow ) const
{
    if (ValidColRow(nCol,nRow))
        return aCol[nCol].GetErrorData( nRow );
    else
        return 0;
}

BOOL ScTable::GetNextSpellingCell(SCCOL& rCol, SCROW& rRow, BOOL bInSel,
                                    const ScMarkData& rMark) const
{
    if (rRow == MAXROW+2)                       // Tabellenende
    {
        rRow = 0;
        rCol = 0;
    }
    else
    {
        rRow++;
        if (rRow == MAXROW+1)
        {
            rCol++;
            rRow = 0;
        }
    }
    if (rCol == MAXCOL+1)
        return TRUE;
    else
    {
        BOOL bStop = FALSE;
        while (!bStop)
        {
            if (ValidCol(rCol))
            {
                bStop = aCol[rCol].GetNextSpellingCell(rRow, bInSel, rMark);
                if (bStop)
                    return TRUE;
                else /*if (rRow == MAXROW+1) */
                {
                    rCol++;
                    rRow = 0;
                }
            }
            else
                return TRUE;
        }
    }
    return FALSE;
}

void ScTable::RemoveAutoSpellObj()
{
    for (SCCOL i=0; i <= MAXCOL; i++)
        aCol[i].RemoveAutoSpellObj();
}

BOOL ScTable::TestTabRefAbs(SCTAB nTable)
{
    BOOL bRet = FALSE;
    for (SCCOL i=0; i <= MAXCOL; i++)
        if (aCol[i].TestTabRefAbs(nTable))
            bRet = TRUE;
    return bRet;
}

void ScTable::CompileDBFormula()
{
    for (SCCOL i=0; i<=MAXCOL; i++) aCol[i].CompileDBFormula();
}

void ScTable::CompileDBFormula( BOOL bCreateFormulaString )
{
    for (SCCOL i=0; i<=MAXCOL; i++) aCol[i].CompileDBFormula( bCreateFormulaString );
}

void ScTable::CompileNameFormula( BOOL bCreateFormulaString )
{
    for (SCCOL i=0; i<=MAXCOL; i++) aCol[i].CompileNameFormula( bCreateFormulaString );
}

void ScTable::CompileColRowNameFormula()
{
    for (SCCOL i=0; i<=MAXCOL; i++) aCol[i].CompileColRowNameFormula();
}






