/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pivot2.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 18:28:25 $
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

#ifdef PCH
#include "core_pch.hxx"
#endif

#pragma hdrstop

#pragma optimize("",off)
#pragma optimize("q",off) // p-code off

// INCLUDE ---------------------------------------------------------------

#include "scitems.hxx"
#include <svx/boxitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/algitem.hxx>
#ifndef _UNOTOOLS_TRANSLITERATIONWRAPPER_HXX
#include <unotools/transliterationwrapper.hxx>
#endif

#include "globstr.hrc"
#include "subtotal.hxx"
#include "rangeutl.hxx"
#include "attrib.hxx"
#include "patattr.hxx"
#include "docpool.hxx"
#include "document.hxx"
#include "userlist.hxx"
#include "pivot.hxx"
#include "rechead.hxx"
#include "errorcodes.hxx"                           // fuer errNoValue
#include "refupdat.hxx"
#include "stlpool.hxx"
#include "stlsheet.hxx"

using ::com::sun::star::sheet::DataPilotFieldReference;

// STATIC DATA -----------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Hilfsmethoden von ScPivot
//--------------------------------------------------------------------------------------------------

void ScPivot::SetFrame(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2, USHORT nWidth)
{
    if (pDoc->pTab[nDestTab])
    {
        SvxBorderLine aLine;
        aLine.SetOutWidth(nWidth);
        SvxBoxItem aBox;
        aBox.SetLine(&aLine, BOX_LINE_LEFT);
        aBox.SetLine(&aLine, BOX_LINE_TOP);
        aBox.SetLine(&aLine, BOX_LINE_RIGHT);
        aBox.SetLine(&aLine, BOX_LINE_BOTTOM);
        SvxBoxInfoItem aBoxInfo;
        aBoxInfo.SetValid(VALID_HORI,FALSE);
        aBoxInfo.SetValid(VALID_VERT,FALSE);
        aBoxInfo.SetValid(VALID_DISTANCE,FALSE);
        pDoc->pTab[nDestTab]->ApplyBlockFrame(&aBox, &aBoxInfo, nCol1, nRow1, nCol2, nRow2);
    }
}

void ScPivot::SetFrameHor(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    if (pDoc->pTab[nDestTab])
    {
        SvxBorderLine aLine;
        aLine.SetOutWidth(20);
        SvxBoxItem aBox;
        aBox.SetLine(&aLine, BOX_LINE_LEFT);
        aBox.SetLine(&aLine, BOX_LINE_TOP);
        aBox.SetLine(&aLine, BOX_LINE_RIGHT);
        aBox.SetLine(&aLine, BOX_LINE_BOTTOM);
        SvxBoxInfoItem aBoxInfo;
        aBoxInfo.SetValid(VALID_VERT,FALSE);
        aBoxInfo.SetValid(VALID_DISTANCE,FALSE);
        aBoxInfo.SetLine(&aLine, BOXINFO_LINE_HORI);
        pDoc->pTab[nDestTab]->ApplyBlockFrame(&aBox, &aBoxInfo, nCol1, nRow1, nCol2, nRow2);
    }
}

void ScPivot::SetFrameVer(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    if (pDoc->pTab[nDestTab])
    {
        SvxBorderLine aLine;
        aLine.SetOutWidth(20);
        SvxBoxItem aBox;
        aBox.SetLine(&aLine, BOX_LINE_LEFT);
        aBox.SetLine(&aLine, BOX_LINE_TOP);
        aBox.SetLine(&aLine, BOX_LINE_RIGHT);
        aBox.SetLine(&aLine, BOX_LINE_BOTTOM);
        SvxBoxInfoItem aBoxInfo;
        aBoxInfo.SetValid(VALID_HORI,FALSE);
        aBoxInfo.SetValid(VALID_DISTANCE,FALSE);
        aBoxInfo.SetLine(&aLine, BOXINFO_LINE_VERT);
        pDoc->pTab[nDestTab]->ApplyBlockFrame(&aBox, &aBoxInfo, nCol1, nRow1, nCol2, nRow2);
    }
}

void ScPivot::SetFontBold(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    if (pDoc->pTab[nDestTab])
    {
        ScPatternAttr aPattern( pDoc->GetPool() );
        aPattern.GetItemSet().Put( SvxWeightItem( WEIGHT_BOLD ) );
        pDoc->pTab[nDestTab]->ApplyPatternArea(nCol1, nRow1, nCol2, nRow2, aPattern);
    }
}

void ScPivot::SetJustifyLeft(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    if (pDoc->pTab[nDestTab])
    {
        ScPatternAttr aPattern( pDoc->GetPool() );
        aPattern.GetItemSet().Put( SvxHorJustifyItem( SVX_HOR_JUSTIFY_LEFT ) );
        pDoc->pTab[nDestTab]->ApplyPatternArea(nCol1, nRow1, nCol2, nRow2, aPattern);
    }
}

void ScPivot::SetJustifyRight(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    if (pDoc->pTab[nDestTab])
    {
        ScPatternAttr aPattern( pDoc->GetPool() );
        aPattern.GetItemSet().Put( SvxHorJustifyItem( SVX_HOR_JUSTIFY_RIGHT ) );
        pDoc->pTab[nDestTab]->ApplyPatternArea(nCol1, nRow1, nCol2, nRow2, aPattern);
    }
}

void ScPivot::SetButton(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    if (pDoc->pTab[nDestTab])
    {
        ScPatternAttr aPattern( pDoc->GetPool() );
        aPattern.GetItemSet().Put( ScMergeFlagAttr(SC_MF_BUTTON) );
        pDoc->pTab[nDestTab]->ApplyPatternArea(nCol1, nRow1, nCol2, nRow2, aPattern);
    }
}

void ScPivot::SetStyle(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2, USHORT nId)
{
    if ( nCol1 > nCol2 || nRow1 > nRow2 )
        return;                                 //  Falls Bereiche leer sind

    USHORT nStringId = 0;
    switch (nId)
    {
        case PIVOT_STYLE_INNER:     nStringId = STR_PIVOT_STYLE_INNER;      break;
        case PIVOT_STYLE_RESULT:    nStringId = STR_PIVOT_STYLE_RESULT;     break;
        case PIVOT_STYLE_CATEGORY:  nStringId = STR_PIVOT_STYLE_CATEGORY;   break;
        case PIVOT_STYLE_TITLE:     nStringId = STR_PIVOT_STYLE_TITLE;      break;
        case PIVOT_STYLE_FIELDNAME: nStringId = STR_PIVOT_STYLE_FIELDNAME;  break;
        case PIVOT_STYLE_TOP:       nStringId = STR_PIVOT_STYLE_TOP;        break;
        default:
            DBG_ERROR("falsche ID bei ScPivot::SetStyle");
            return;
    }
    String aStyleName = ScGlobal::GetRscString(nStringId);

    ScStyleSheetPool* pStlPool = pDoc->GetStyleSheetPool();
    ScStyleSheet* pStyle = (ScStyleSheet*) pStlPool->Find( aStyleName, SFX_STYLE_FAMILY_PARA );
    if (!pStyle)
    {
        //  neu anlegen

        pStyle = (ScStyleSheet*) &pStlPool->Make( aStyleName, SFX_STYLE_FAMILY_PARA,
                                                    SFXSTYLEBIT_USERDEF );
        pStyle->SetParent( ScGlobal::GetRscString(STR_STYLENAME_STANDARD) );
        SfxItemSet& rSet = pStyle->GetItemSet();
        if ( nId==PIVOT_STYLE_RESULT || nId==PIVOT_STYLE_TITLE )
            rSet.Put( SvxWeightItem( WEIGHT_BOLD ) );
        if ( nId==PIVOT_STYLE_CATEGORY || nId==PIVOT_STYLE_TITLE )
            rSet.Put( SvxHorJustifyItem( SVX_HOR_JUSTIFY_LEFT ) );
    }

    pDoc->pTab[nDestTab]->ApplyStyleArea( nCol1, nRow1, nCol2, nRow2, *pStyle );
}

void ScPivot::SetValue(SCCOL nCol, SCROW nRow, const SubTotal& rTotal, USHORT nFunc)
{
    if ( rTotal.Valid( nFunc ) == 1)
        pDoc->SetValue(nCol, nRow, nDestTab, rTotal.Result( nFunc ));
    else if ( rTotal.Valid( nFunc ) == 0)
        pDoc->SetError(nCol, nRow, nDestTab, errNoValue);
}

//--------------------------------------------------------------------------------------------------

void ScPivot::GetParam( ScPivotParam& rParam, ScQueryParam& rQuery, ScArea& rSrcArea ) const
{
    SCSIZE nCount;
    SCCOL nDummyCol;
    SCROW nDummyRow;
    GetDestArea( rParam.nCol,rParam.nRow, nDummyCol,nDummyRow, rParam.nTab );

    // Row und Col in der Bedeutung vertauscht:
    GetRowFields( rParam.aColArr, nCount );
    rParam.nColCount = nCount;
    GetColFields( rParam.aRowArr, nCount );
    rParam.nRowCount = nCount;
    GetDataFields( rParam.aDataArr, nCount );
    rParam.nDataCount = nCount;

    rParam.bIgnoreEmptyRows  = GetIgnoreEmpty();
    rParam.bDetectCategories = GetDetectCat();
    rParam.bMakeTotalCol     = GetMakeTotalCol();
    rParam.bMakeTotalRow     = GetMakeTotalRow();

    GetQuery(rQuery);
    GetSrcArea( rSrcArea.nColStart, rSrcArea.nRowStart,
                        rSrcArea.nColEnd, rSrcArea.nRowEnd, rSrcArea.nTab );
}

void ScPivot::SetParam( const ScPivotParam& rParam, const ScQueryParam& rQuery,
                            const ScArea& rSrcArea )
{
    SetQuery( rQuery );
    SetHeader( TRUE );
    SetSrcArea( rSrcArea.nColStart, rSrcArea.nRowStart,
                          rSrcArea.nColEnd, rSrcArea.nRowEnd, rSrcArea.nTab );
    SetDestPos( rParam.nCol, rParam.nRow, rParam.nTab );
    SetIgnoreEmpty( rParam.bIgnoreEmptyRows );
    SetDetectCat( rParam.bDetectCategories );
    SetMakeTotalCol( rParam.bMakeTotalCol );
    SetMakeTotalRow( rParam.bMakeTotalRow );

    // Row und Col in der Bedeutung vertauscht:
    SetRowFields( rParam.aColArr, rParam.nColCount );
    SetColFields( rParam.aRowArr, rParam.nRowCount );
    SetDataFields( rParam.aDataArr, rParam.nDataCount );
}

DataObject* ScPivot::Clone() const
{
    return new ScPivot(*this);
}

//--------------------------------------------------------------------------------------------------
// PivotStrCollection
//--------------------------------------------------------------------------------------------------

DataObject* PivotStrCollection::Clone() const
{
    return new PivotStrCollection(*this);
}

short PivotStrCollection::Compare(DataObject* pKey1, DataObject* pKey2) const
{
    DBG_ASSERT(pKey1&&pKey2,"0-Zeiger bei PivotStrCollection::Compare");

    short nResult = 0;

    TypedStrData& rData1 = (TypedStrData&)*pKey1;
    TypedStrData& rData2 = (TypedStrData&)*pKey2;

    if ( rData1.nStrType > rData2.nStrType )
        nResult = 1;
    else if ( rData1.nStrType < rData2.nStrType )
        nResult = -1;
    else if ( !rData1.nStrType /* && !rData2.nStrType */ )
    {
        // Zahlen vergleichen:

        if ( rData1.nValue == rData2.nValue )
            nResult = 0;
        else if ( rData1.nValue < rData2.nValue )
            nResult = -1;
        else
            nResult = 1;
    }
    else /* if ( rData1.nStrType && rData2.nStrType ) */
    {
        // Strings vergleichen:

        if (pUserData)
            nResult = pUserData->ICompare(rData1.aStrValue, rData2.aStrValue);
        else
        {
            nResult = (short) ScGlobal::pTransliteration->compareString(
                rData1.aStrValue, rData2.aStrValue );
        }
    }

    return nResult;
}

USHORT PivotStrCollection::GetIndex(TypedStrData* pData) const
{
    USHORT nIndex = 0;
    if (!Search(pData, nIndex))
        nIndex = 0;
    return nIndex;
}

//--------------------------------------------------------------------------------------------------
// PivotCollection
//--------------------------------------------------------------------------------------------------

String ScPivotCollection::CreateNewName( USHORT nMin ) const
{
    String aBase( RTL_CONSTASCII_USTRINGPARAM("DataPilot") );
    //! from Resource?

    for (USHORT nAdd=0; nAdd<=nCount; nAdd++)   //  nCount+1 Versuche
    {
        String aNewName = aBase;
        aNewName += String::CreateFromInt32( nMin + nAdd );
        BOOL bFound = FALSE;
        for (USHORT i=0; i<nCount && !bFound; i++)
            if (((ScPivot*)pItems[i])->GetName() == aNewName)
                bFound = TRUE;
        if (!bFound)
            return aNewName;            // freien Namen gefunden
    }
    return String();                    // sollte nicht vorkommen
}

ScPivot* ScPivotCollection::GetPivotAtCursor(SCCOL nCol, SCROW nRow, SCTAB nTab) const
{
    if (pItems)
    {
        for (USHORT i = 0; i < nCount; i++)
            if (((ScPivot*)pItems[i])->IsPivotAtCursor(nCol, nRow, nTab))
            {
                return (ScPivot*)pItems[i];
            }
    }
    return NULL;
}

BOOL ScPivotCollection::Load(SvStream& rStream)
{
    BOOL bSuccess = TRUE;
    USHORT nNewCount, i;
    FreeAll();

    ScMultipleReadHeader aHdr( rStream );

    rStream >> nNewCount;
    for (i=0; i<nNewCount && bSuccess; i++)
    {
        ScPivot* pPivot = new ScPivot( pDoc );
        if (pPivot)
        {
            bSuccess = pPivot->Load(rStream, aHdr);
            Insert( pPivot );
        }
        else
            bSuccess = FALSE;
    }

    //  fuer alte Dateien: eindeutige Namen vergeben

    if (bSuccess)
        for (i=0; i<nCount; i++)
            if (!((const ScPivot*)At(i))->GetName().Len())
                ((ScPivot*)At(i))->SetName( CreateNewName() );

    return bSuccess;
}

BOOL ScPivotCollection::Store(SvStream& rStream) const
{
    BOOL bSuccess = TRUE;

    ScMultipleWriteHeader aHdr( rStream );

    rStream << nCount;

    for (USHORT i=0; i<nCount && bSuccess; i++)
        bSuccess = ((const ScPivot*)At(i))->Store( rStream, aHdr );

    return bSuccess;
}

void ScPivotCollection::UpdateReference(UpdateRefMode eUpdateRefMode,
                                SCCOL nCol1, SCROW nRow1, SCTAB nTab1,
                                SCCOL nCol2, SCROW nRow2, SCTAB nTab2,
                                SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    for (USHORT i=0; i<nCount; i++)
    {
        SCCOL theCol1;
        SCROW theRow1;
        SCTAB theTab1;
        SCCOL theCol2;
        SCROW theRow2;
        SCTAB theTab2;
        ScRefUpdateRes eRes;
        ScPivot* pPivot = (ScPivot*)pItems[i];

        //  Source

        pPivot->GetSrcArea( theCol1, theRow1, theCol2, theRow2, theTab1 );
        theTab2 = theTab1;

        eRes = ScRefUpdate::Update( pDoc, eUpdateRefMode,
                                                nCol1,nRow1,nTab1, nCol2,nRow2,nTab2, nDx,nDy,nDz,
                                                theCol1,theRow1,theTab1, theCol2,theRow2,theTab2 );

        if (eRes != UR_NOTHING)
            pPivot->MoveSrcArea( theCol1, theRow1, theTab1 );

        //  Dest

        pPivot->GetDestArea( theCol1, theRow1, theCol2, theRow2, theTab1 );
        theTab2 = theTab1;

        eRes = ScRefUpdate::Update( pDoc, eUpdateRefMode,
                                                nCol1,nRow1,nTab1, nCol2,nRow2,nTab2, nDx,nDy,nDz,
                                                theCol1,theRow1,theTab1, theCol2,theRow2,theTab2 );

        if (eRes != UR_NOTHING)
            pPivot->MoveDestArea( theCol1, theRow1, theTab1 );
    }
}

void ScPivotCollection::UpdateGrow( const ScRange& rArea, SCCOL nGrowX, SCROW nGrowY )
{
    //  nur Quell-Bereich

    for (USHORT i=0; i<nCount; i++)
    {
        ScPivot* pPivot = (ScPivot*)pItems[i];
        ScRange aSrc = pPivot->GetSrcArea();
        ScRefUpdateRes eRes = ScRefUpdate::DoGrow( rArea, nGrowX, nGrowY, aSrc );
        if (eRes != UR_NOTHING)
            pPivot->ExtendSrcArea( aSrc.aEnd.Col(), aSrc.aEnd.Row() );
    }
}

BOOL ScPivotCollection::operator==(const ScPivotCollection& rCmp) const
{
    if (nCount != rCmp.nCount)
        return FALSE;

    if (!nCount)
        return TRUE;            // beide leer - nicht erst die Param's anlegen!

    ScPivotParam aMyParam, aCmpParam;
    ScQueryParam aMyQuery, aCmpQuery;
    ScArea aMyArea, aCmpArea;

    for (USHORT i=0; i<nCount; i++)
    {
        ScPivot* pMyPivot = (ScPivot*)pItems[i];
        pMyPivot->GetParam( aMyParam, aMyQuery, aMyArea );
        ScPivot* pCmpPivot = (ScPivot*)rCmp.pItems[i];
        pCmpPivot->GetParam( aCmpParam, aCmpQuery, aCmpArea );
        if (!( aMyArea==aCmpArea && aMyParam==aCmpParam && aMyQuery==aCmpQuery ))
            return FALSE;
    }

    return TRUE;
}

DataObject* ScPivotCollection::Clone() const
{
    return new ScPivotCollection(*this);
}

// ============================================================================

LabelData::LabelData( const String& rName, short nCol, bool bIsValue ) :
    maName( rName ),
    mnCol( nCol ),
    mnFuncMask( PIVOT_FUNC_NONE ),
    mnUsedHier( 0 ),
    mbShowAll( false ),
    mbIsValue( bIsValue )
{
}

// ============================================================================

ScDPFuncData::ScDPFuncData( short nCol, USHORT nFuncMask ) :
    mnCol( nCol ),
    mnFuncMask( nFuncMask )
{
}

ScDPFuncData::ScDPFuncData( short nCol, USHORT nFuncMask, const DataPilotFieldReference& rFieldRef ) :
    mnCol( nCol ),
    mnFuncMask( nFuncMask ),
    maFieldRef( rFieldRef )
{
}

// ============================================================================

