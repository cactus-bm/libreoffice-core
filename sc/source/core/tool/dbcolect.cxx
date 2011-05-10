/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "precompiled_sc.hxx"


#include <tools/debug.hxx>
#include <unotools/transliterationwrapper.hxx>

#include "dbcolect.hxx"
#include "global.hxx"
#include "globalnames.hxx"
#include "refupdat.hxx"
#include "rechead.hxx"
#include "document.hxx"
#include "queryparam.hxx"
#include "globstr.hrc"
#include "subtotalparam.hxx"

#include <memory>

using ::std::unary_function;
using ::std::for_each;
using ::std::find_if;
using ::std::remove_if;

//---------------------------------------------------------------------------------------

ScDBData::ScDBData( const ::rtl::OUString& rName,
                    SCTAB nTab,
                    SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2,
                    bool bByR, bool bHasH) :
    aName       (rName),
    nTable      (nTab),
    nStartCol   (nCol1),
    nStartRow   (nRow1),
    nEndCol     (nCol2),
    nEndRow     (nRow2),
    bByRow      (bByR),
    bHasHeader  (bHasH),
    bDoSize     (false),
    bKeepFmt    (false),
    bStripData  (false),
    bIsAdvanced (false),
    bDBSelection(false),
    nIndex      (0),
    bAutoFilter (false),
    bModified   (false)
{
    ScSortParam aSortParam;
    ScQueryParam aQueryParam;
    ScImportParam aImportParam;

    SetSortParam( aSortParam );
    SetQueryParam( aQueryParam );
    SetImportParam( aImportParam );
}

ScDBData::ScDBData( const ScDBData& rData ) :
    ScDataObject(),
    ScRefreshTimer      ( rData ),
    aName               (rData.aName),
    nTable              (rData.nTable),
    nStartCol           (rData.nStartCol),
    nStartRow           (rData.nStartRow),
    nEndCol             (rData.nEndCol),
    nEndRow             (rData.nEndRow),
    bByRow              (rData.bByRow),
    bHasHeader          (rData.bHasHeader),
    bDoSize             (rData.bDoSize),
    bKeepFmt            (rData.bKeepFmt),
    bStripData          (rData.bStripData),
    bSortCaseSens       (rData.bSortCaseSens),
    bSortNaturalSort    (rData.bSortNaturalSort),
    bIncludePattern     (rData.bIncludePattern),
    bSortInplace        (rData.bSortInplace),
    bSortUserDef        (rData.bSortUserDef),
    nSortUserIndex      (rData.nSortUserIndex),
    nSortDestTab        (rData.nSortDestTab),
    nSortDestCol        (rData.nSortDestCol),
    nSortDestRow        (rData.nSortDestRow),
    aSortLocale         (rData.aSortLocale),
    aSortAlgorithm      (rData.aSortAlgorithm),
    bIsAdvanced         (rData.bIsAdvanced),
    aAdvSource          (rData.aAdvSource),
    maQueryParam        (rData.maQueryParam),
    maSubTotal          (rData.maSubTotal),
    maImportParam       (rData.maImportParam),
    bDBSelection        (rData.bDBSelection),
    nIndex              (rData.nIndex),
    bAutoFilter         (rData.bAutoFilter),
    bModified           (rData.bModified)
{
    sal_uInt16 i;

    for (i=0; i<MAXSORT; i++)
    {
        bDoSort[i]      = rData.bDoSort[i];
        nSortField[i]   = rData.nSortField[i];
        bAscending[i]   = rData.bAscending[i];
    }
}

ScDBData& ScDBData::operator= (const ScDBData& rData)
{
    sal_uInt16 i;

    ScRefreshTimer::operator=( rData );
    aName               = rData.aName;
    nTable              = rData.nTable;
    nStartCol           = rData.nStartCol;
    nStartRow           = rData.nStartRow;
    nEndCol             = rData.nEndCol;
    nEndRow             = rData.nEndRow;
    bByRow              = rData.bByRow;
    bHasHeader          = rData.bHasHeader;
    bDoSize             = rData.bDoSize;
    bKeepFmt            = rData.bKeepFmt;
    bStripData          = rData.bStripData;
    bSortCaseSens       = rData.bSortCaseSens;
    bSortNaturalSort    = rData.bSortNaturalSort;
    bIncludePattern     = rData.bIncludePattern;
    bSortInplace        = rData.bSortInplace;
    nSortDestTab        = rData.nSortDestTab;
    nSortDestCol        = rData.nSortDestCol;
    nSortDestRow        = rData.nSortDestRow;
    bSortUserDef        = rData.bSortUserDef;
    nSortUserIndex      = rData.nSortUserIndex;
    aSortLocale         = rData.aSortLocale;
    aSortAlgorithm      = rData.aSortAlgorithm;
    bIsAdvanced         = rData.bIsAdvanced;
    maQueryParam        = rData.maQueryParam;
    maSubTotal          = rData.maSubTotal;
    maImportParam       = rData.maImportParam;
    aAdvSource          = rData.aAdvSource;
    bDBSelection        = rData.bDBSelection;
    nIndex              = rData.nIndex;
    bAutoFilter         = rData.bAutoFilter;

    for (i=0; i<MAXSORT; i++)
    {
        bDoSort[i]      = rData.bDoSort[i];
        nSortField[i]   = rData.nSortField[i];
        bAscending[i]   = rData.bAscending[i];
    }

    return *this;
}

SCTAB ScDBData::GetTable() const
{
    return nTable;
}

bool ScDBData::operator== (const ScDBData& rData) const
{
    //  Daten, die nicht in den Params sind

    if ( nTable     != rData.nTable     ||
         bDoSize    != rData.bDoSize    ||
         bKeepFmt   != rData.bKeepFmt   ||
         bIsAdvanced!= rData.bIsAdvanced||
         bStripData != rData.bStripData ||
//       SAB: I think this should be here, but I don't want to break something
//         bAutoFilter!= rData.bAutoFilter||
         ScRefreshTimer::operator!=( rData )
        )
        return false;

    if ( bIsAdvanced && aAdvSource != rData.aAdvSource )
        return false;

    ScSortParam aSort1, aSort2;
    GetSortParam(aSort1);
    rData.GetSortParam(aSort2);
    if (!(aSort1 == aSort2))
        return false;

    ScQueryParam aQuery1, aQuery2;
    GetQueryParam(aQuery1);
    rData.GetQueryParam(aQuery2);
    if (!(aQuery1 == aQuery2))
        return false;

    ScSubTotalParam aSubTotal1, aSubTotal2;
    GetSubTotalParam(aSubTotal1);
    rData.GetSubTotalParam(aSubTotal2);
    if (!(aSubTotal1 == aSubTotal2))
        return false;

    ScImportParam aImport1, aImport2;
    GetImportParam(aImport1);
    rData.GetImportParam(aImport2);
    if (!(aImport1 == aImport2))
        return false;

    return true;
}

ScDBData::~ScDBData()
{
    StopRefreshTimer();
}


::rtl::OUString ScDBData::GetSourceString() const
{
    ::rtl::OUStringBuffer aBuf;
    if (maImportParam.bImport)
    {
        aBuf.append(maImportParam.aDBName);
        aBuf.append(sal_Unicode('/'));
        aBuf.append(maImportParam.aStatement);
    }
    return aBuf.makeStringAndClear();
}

::rtl::OUString ScDBData::GetOperations() const
{
    ::rtl::OUStringBuffer aBuf;
    if (maQueryParam.GetEntryCount())
    {
        const ScQueryEntry& rEntry = maQueryParam.GetEntry(0);
        if (rEntry.bDoQuery)
            aBuf.append(ScGlobal::GetRscString(STR_OPERATION_FILTER));
    }

    if (bDoSort[0])
    {
        if (aBuf.getLength())
            aBuf.appendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
        aBuf.append(ScGlobal::GetRscString(STR_OPERATION_SORT));
    }

    if (maSubTotal.bGroupActive[0] && !maSubTotal.bRemoveOnly)
    {
        if (aBuf.getLength())
            aBuf.appendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
        aBuf.append(ScGlobal::GetRscString(STR_OPERATION_SUBTOTAL));
    }

    if (!aBuf.getLength())
        aBuf.append(ScGlobal::GetRscString(STR_OPERATION_NONE));

    return aBuf.makeStringAndClear();
}

void ScDBData::GetArea(SCTAB& rTab, SCCOL& rCol1, SCROW& rRow1, SCCOL& rCol2, SCROW& rRow2) const
{
    rTab  = nTable;
    rCol1 = nStartCol;
    rRow1 = nStartRow;
    rCol2 = nEndCol;
    rRow2 = nEndRow;
}

void ScDBData::GetArea(ScRange& rRange) const
{
    SCROW nNewEndRow = nEndRow;
    rRange = ScRange( nStartCol, nStartRow, nTable, nEndCol, nNewEndRow, nTable );
}

void ScDBData::SetArea(SCTAB nTab, SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    nTable  = nTab;
    nStartCol = nCol1;
    nStartRow = nRow1;
    nEndCol   = nCol2;
    nEndRow   = nRow2;
}

void ScDBData::MoveTo(SCTAB nTab, SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2)
{
    sal_uInt16 i;
    long nDifX = ((long) nCol1) - ((long) nStartCol);
    long nDifY = ((long) nRow1) - ((long) nStartRow);

    long nSortDif = bByRow ? nDifX : nDifY;
    long nSortEnd = bByRow ? static_cast<long>(nCol2) : static_cast<long>(nRow2);

    for (i=0; i<MAXSORT; i++)
    {
        nSortField[i] += nSortDif;
        if (nSortField[i] > nSortEnd)
        {
            nSortField[i] = 0;
            bDoSort[i]    = false;
        }
    }

    if (maQueryParam.GetEntryCount() < MAXQUERY)
        maQueryParam.Resize(MAXQUERY);
    for (i=0; i<MAXQUERY; i++)
    {
        ScQueryEntry& rEntry = maQueryParam.GetEntry(i);
        rEntry.nField += nDifX;
        if (rEntry.nField > nCol2)
        {
            rEntry.nField = 0;
            rEntry.bDoQuery = false;
        }
    }
    for (i=0; i<MAXSUBTOTAL; i++)
    {
        maSubTotal.nField[i] = sal::static_int_cast<SCCOL>( maSubTotal.nField[i] + nDifX );
        if (maSubTotal.nField[i] > nCol2)
        {
            maSubTotal.nField[i] = 0;
            maSubTotal.bGroupActive[i] = false;
        }
    }

    SetArea( nTab, nCol1, nRow1, nCol2, nRow2 );
}

void ScDBData::GetSortParam( ScSortParam& rSortParam ) const
{
    rSortParam.nCol1 = nStartCol;
    rSortParam.nRow1 = nStartRow;
    rSortParam.nCol2 = nEndCol;
    rSortParam.nRow2 = nEndRow;
    rSortParam.bByRow = bByRow;
    rSortParam.bHasHeader = bHasHeader;
    rSortParam.bCaseSens = bSortCaseSens;
    rSortParam.bNaturalSort = bSortNaturalSort;
    rSortParam.bInplace = bSortInplace;
    rSortParam.nDestTab = nSortDestTab;
    rSortParam.nDestCol = nSortDestCol;
    rSortParam.nDestRow = nSortDestRow;
    rSortParam.bIncludePattern = bIncludePattern;
    rSortParam.bUserDef = bSortUserDef;
    rSortParam.nUserIndex = nSortUserIndex;
    for (sal_uInt16 i=0; i<MAXSORT; i++)
    {
        rSortParam.bDoSort[i]    = bDoSort[i];
        rSortParam.nField[i]     = nSortField[i];
        rSortParam.bAscending[i] = bAscending[i];
    }
    rSortParam.aCollatorLocale = aSortLocale;
    rSortParam.aCollatorAlgorithm = aSortAlgorithm;
}

void ScDBData::SetSortParam( const ScSortParam& rSortParam )
{
    bSortCaseSens = rSortParam.bCaseSens;
    bSortNaturalSort = rSortParam.bNaturalSort;
    bIncludePattern = rSortParam.bIncludePattern;
    bSortInplace = rSortParam.bInplace;
    nSortDestTab = rSortParam.nDestTab;
    nSortDestCol = rSortParam.nDestCol;
    nSortDestRow = rSortParam.nDestRow;
    bSortUserDef = rSortParam.bUserDef;
    nSortUserIndex = rSortParam.nUserIndex;
    for (sal_uInt16 i=0; i<MAXSORT; i++)
    {
        bDoSort[i]    = rSortParam.bDoSort[i];
        nSortField[i] = rSortParam.nField[i];
        bAscending[i] = rSortParam.bAscending[i];
    }
    aSortLocale = rSortParam.aCollatorLocale;
    aSortAlgorithm = rSortParam.aCollatorAlgorithm;

    //#98317#; set the orientation
    bByRow = rSortParam.bByRow;
}

void ScDBData::GetQueryParam( ScQueryParam& rQueryParam ) const
{
    rQueryParam = maQueryParam;
    rQueryParam.nCol1 = nStartCol;
    rQueryParam.nRow1 = nStartRow;
    rQueryParam.nCol2 = nEndCol;
    rQueryParam.nRow2 = nEndRow;
    rQueryParam.nTab  = nTable;
    rQueryParam.bByRow = bByRow;
    rQueryParam.bHasHeader = bHasHeader;
}

void ScDBData::SetQueryParam(const ScQueryParam& rQueryParam)
{
    DBG_ASSERT( rQueryParam.GetEntryCount() <= MAXQUERY ||
                !rQueryParam.GetEntry(MAXQUERY).bDoQuery,
                "zuviele Eintraege bei ScDBData::SetQueryParam" );

    maQueryParam = rQueryParam;

    //  set bIsAdvanced to false for everything that is not from the
    //  advanced filter dialog
    bIsAdvanced = false;
}

void ScDBData::SetAdvancedQuerySource(const ScRange* pSource)
{
    if (pSource)
    {
        aAdvSource = *pSource;
        bIsAdvanced = true;
    }
    else
        bIsAdvanced = false;
}

bool ScDBData::GetAdvancedQuerySource(ScRange& rSource) const
{
    rSource = aAdvSource;
    return bIsAdvanced;
}

void ScDBData::GetSubTotalParam(ScSubTotalParam& rSubTotalParam) const
{
    rSubTotalParam = maSubTotal;

    // Share the data range with the parent db data.  The range in the subtotal
    // param struct is not used.
    rSubTotalParam.nCol1 = nStartCol;
    rSubTotalParam.nRow1 = nStartRow;
    rSubTotalParam.nCol2 = nEndCol;
    rSubTotalParam.nRow2 = nEndRow;
}

void ScDBData::SetSubTotalParam(const ScSubTotalParam& rSubTotalParam)
{
    maSubTotal = rSubTotalParam;
}

void ScDBData::GetImportParam(ScImportParam& rImportParam) const
{
    rImportParam = maImportParam;
    // set the range.
    rImportParam.nCol1 = nStartCol;
    rImportParam.nRow1 = nStartRow;
    rImportParam.nCol2 = nEndCol;
    rImportParam.nRow2 = nEndRow;
}

void ScDBData::SetImportParam(const ScImportParam& rImportParam)
{
    // the range is ignored.
    maImportParam = rImportParam;
}

bool ScDBData::IsDBAtCursor(SCCOL nCol, SCROW nRow, SCTAB nTab, bool bStartOnly) const
{
    if (nTab == nTable)
    {
        if ( bStartOnly )
            return ( nCol == nStartCol && nRow == nStartRow );
        else
            return ( nCol >= nStartCol && nCol <= nEndCol &&
                     nRow >= nStartRow && nRow <= nEndRow );
    }

    return false;
}

bool ScDBData::IsDBAtArea(SCTAB nTab, SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2) const
{
    return (bool)((nTab == nTable)
                    && (nCol1 == nStartCol) && (nRow1 == nStartRow)
                    && (nCol2 == nEndCol) && (nRow2 == nEndRow));
}

bool ScDBData::HasQueryParam() const
{
    if (!maQueryParam.GetEntryCount())
        return false;

    return maQueryParam.GetEntry(0).bDoQuery;
}

ScDataObject*   ScDBData::Clone() const
{
    return new ScDBData(*this);
}

void ScDBData::UpdateMoveTab(SCTAB nOldPos, SCTAB nNewPos)
{
        ScRange aRange;
        GetArea( aRange );
        SCTAB nTab = aRange.aStart.Tab();               // hat nur eine Tabelle

        //  anpassen wie die aktuelle Tabelle bei ScTablesHint (tabvwsh5.cxx)

        if ( nTab == nOldPos )                          // verschobene Tabelle
            nTab = nNewPos;
        else if ( nOldPos < nNewPos )                   // nach hinten verschoben
        {
            if ( nTab > nOldPos && nTab <= nNewPos )    // nachrueckender Bereich
                --nTab;
        }
        else                                            // nach vorne verschoben
        {
            if ( nTab >= nNewPos && nTab < nOldPos )    // nachrueckender Bereich
                ++nTab;
        }

        bool bChanged = ( nTab != aRange.aStart.Tab() );
        if (bChanged)
            SetArea( nTab, aRange.aStart.Col(), aRange.aStart.Row(),
                                    aRange.aEnd.Col(),aRange.aEnd .Row() );

        //  MoveTo ist nicht noetig, wenn nur die Tabelle geaendert ist

        SetModified(bChanged);

}

void ScDBData::UpdateReference(ScDocument* pDoc, UpdateRefMode eUpdateRefMode,
                                SCCOL nCol1, SCROW nRow1, SCTAB nTab1,
                                SCCOL nCol2, SCROW nRow2, SCTAB nTab2,
                                SCsCOL nDx, SCsROW nDy, SCsTAB nDz)
{
    SCCOL theCol1;
    SCROW theRow1;
    SCTAB theTab1;
    SCCOL theCol2;
    SCROW theRow2;
    SCTAB theTab2;
    GetArea( theTab1, theCol1, theRow1, theCol2, theRow2 );
    theTab2 = theTab1;

    bool bDoUpdate = ScRefUpdate::Update( pDoc, eUpdateRefMode,
                                            nCol1,nRow1,nTab1, nCol2,nRow2,nTab2, nDx,nDy,nDz,
                                            theCol1,theRow1,theTab1, theCol2,theRow2,theTab2 ) != UR_NOTHING;
    if (bDoUpdate)
        MoveTo( theTab1, theCol1, theRow1, theCol2, theRow2 );

    ScRange aRangeAdvSource;
    if ( GetAdvancedQuerySource(aRangeAdvSource) )
    {
        aRangeAdvSource.GetVars( theCol1,theRow1,theTab1, theCol2,theRow2,theTab2 );
        if ( ScRefUpdate::Update( pDoc, eUpdateRefMode,
                                    nCol1,nRow1,nTab1, nCol2,nRow2,nTab2, nDx,nDy,nDz,
                                    theCol1,theRow1,theTab1, theCol2,theRow2,theTab2 ) )
        {
            aRangeAdvSource.aStart.Set( theCol1,theRow1,theTab1 );
            aRangeAdvSource.aEnd.Set( theCol2,theRow2,theTab2 );
            SetAdvancedQuerySource( &aRangeAdvSource );

            bDoUpdate = true;       // DBData is modified
        }
    }

    SetModified(bDoUpdate);

    //!     Testen, ob mitten aus dem Bereich geloescht/eingefuegt wurde !!!
}

void ScDBData::ExtendDataArea(ScDocument* pDoc)
{
    // Extend the DB area to include data rows immediately below.
    SCCOL nCol1a = nStartCol, nCol2a = nEndCol;
    SCROW nRow1a = nStartRow, nRow2a = nEndRow;
    pDoc->GetDataArea(nTable, nCol1a, nRow1a, nCol2a, nRow2a, false, false);
    nEndRow = nRow2a;
}

namespace {

class FindByTable : public ::std::unary_function<ScDBData, bool>
{
    SCTAB mnTab;
public:
    FindByTable(SCTAB nTab) : mnTab(nTab) {}

    bool operator() (const ScDBData& r) const
    {
        ScRange aRange;
        r.GetArea(aRange);
        return aRange.aStart.Tab() == mnTab;
    }
};

class UpdateRefFunc : public unary_function<ScDBData, void>
{
    ScDocument* mpDoc;
    UpdateRefMode meMode;
    SCCOL mnCol1;
    SCROW mnRow1;
    SCTAB mnTab1;
    SCCOL mnCol2;
    SCROW mnRow2;
    SCTAB mnTab2;
    SCsCOL mnDx;
    SCsROW mnDy;
    SCsTAB mnDz;

public:
    UpdateRefFunc(ScDocument* pDoc, UpdateRefMode eMode,
                    SCCOL nCol1, SCROW nRow1, SCTAB nTab1,
                    SCCOL nCol2, SCROW nRow2, SCTAB nTab2,
                    SCsCOL nDx, SCsROW nDy, SCsTAB nDz) :
        mpDoc(pDoc), meMode(eMode),
        mnCol1(nCol1), mnRow1(nRow1), mnTab1(nTab1),
        mnCol2(nCol2), mnRow2(nRow2), mnTab2(nTab2),
        mnDx(nDx), mnDy(nDy), mnDz(nDz) {}

    void operator() (ScDBData& r)
    {
        r.UpdateReference(mpDoc, meMode, mnCol1, mnRow1, mnTab1, mnCol2, mnRow2, mnTab2, mnDx, mnDy, mnDz);
    }
};

class UpdateMoveTabFunc : public unary_function<ScDBData, void>
{
    SCTAB mnOldTab;
    SCTAB mnNewTab;
public:
    UpdateMoveTabFunc(SCTAB nOld, SCTAB nNew) : mnOldTab(nOld), mnNewTab(nNew) {}
    void operator() (ScDBData& r)
    {
        r.UpdateMoveTab(mnOldTab, mnNewTab);
    }
};

class FindByCursor : public unary_function<ScDBData, bool>
{
    SCCOL mnCol;
    SCROW mnRow;
    SCTAB mnTab;
    bool mbStartOnly;
public:
    FindByCursor(SCCOL nCol, SCROW nRow, SCTAB nTab, bool bStartOnly) :
        mnCol(nCol), mnRow(nRow), mnTab(nTab), mbStartOnly(bStartOnly) {}

    bool operator() (const ScDBData& r)
    {
        return r.IsDBAtCursor(mnCol, mnRow, mnTab, mbStartOnly);
    }
};

class FindByRange : public unary_function<ScDBData, bool>
{
    const ScRange& mrRange;
public:
    FindByRange(const ScRange& rRange) : mrRange(rRange) {}

    bool operator() (const ScDBData& r)
    {
        return r.IsDBAtArea(
            mrRange.aStart.Tab(), mrRange.aStart.Col(), mrRange.aStart.Row(), mrRange.aEnd.Col(), mrRange.aEnd.Row());
    }
};

}

ScDBCollection::ScDBCollection(const ScDBCollection& r) :
    ScSortedCollection(r), pDoc(r.pDoc), nEntryIndex(r.nEntryIndex), maAnonDBs(r.maAnonDBs)
{}

short ScDBCollection::Compare(ScDataObject* pKey1, ScDataObject* pKey2) const
{
    const String& rStr1 = ((ScDBData*)pKey1)->GetName();
    const String& rStr2 = ((ScDBData*)pKey2)->GetName();
    return (short) ScGlobal::GetpTransliteration()->compareString( rStr1, rStr2 );
}

//  IsEqual - alles gleich

sal_Bool ScDBCollection::IsEqual(ScDataObject* pKey1, ScDataObject* pKey2) const
{
    return *(ScDBData*)pKey1 == *(ScDBData*)pKey2;
}

ScDBData* ScDBCollection::GetDBAtCursor(SCCOL nCol, SCROW nRow, SCTAB nTab, sal_Bool bStartOnly) const
{
    ScDBData* pNoNameData = pDoc->GetAnonymousDBData(nTab);
    if (pItems)
    {
        for (sal_uInt16 i = 0; i < nCount; i++)
        {
            if (((ScDBData*)pItems[i])->IsDBAtCursor(nCol, nRow, nTab, bStartOnly))
            {
                ScDBData* pDB = (ScDBData*)pItems[i];
                return pDB; //return AnonymousDBData only if nothing else was found
            }
        }
    }
    if (pNoNameData)
        if (pNoNameData->IsDBAtCursor(nCol,nRow,nTab,bStartOnly))
            return pNoNameData;

    // Check the anonymous db ranges.
    const ScDBData* pData = findAnonAtCursor(nCol, nRow, nTab, bStartOnly);
    if (pData)
        return const_cast<ScDBData*>(pData);

    return NULL;
}

ScDBData* ScDBCollection::GetDBAtArea(SCTAB nTab, SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2) const
{
    ScDBData* pNoNameData = pDoc->GetAnonymousDBData(nTab);
    if (pItems)
    {
        for (sal_uInt16 i = 0; i < nCount; i++)
            if (((ScDBData*)pItems[i])->IsDBAtArea(nTab, nCol1, nRow1, nCol2, nRow2))
            {
                ScDBData* pDB = (ScDBData*)pItems[i];
                return pDB; //return AnonymousDBData only if nothing else was found
            }
    }
    if (pNoNameData)
        if (pNoNameData->IsDBAtArea(nTab, nCol1, nRow1, nCol2, nRow2))
            return pNoNameData;

    // Check the anonymous db ranges.
    ScRange aRange(nCol1, nRow1, nTab, nCol2, nRow2, nTab);
    const ScDBData* pData = findAnonByRange(aRange);
    if (pData)
        return const_cast<ScDBData*>(pData);

    return NULL;
}

ScDBData* ScDBCollection::GetFilterDBAtTable(SCTAB nTab) const
{
    ScDBData* pDataEmpty = NULL;
    if (pItems)
    {
        for (sal_uInt16 i = 0; i < nCount; i++)
        {
            ScDBData* pDBTemp = (ScDBData*)pItems[i];
            if ( pDBTemp->GetTable() == nTab )
            {
                sal_Bool bFilter = pDBTemp->HasAutoFilter() || pDBTemp->HasQueryParam();

                if ( bFilter )
                    return pDBTemp;
            }
        }
    }

    return pDataEmpty;
}

sal_Bool ScDBCollection::SearchName( const String& rName, sal_uInt16& rIndex ) const
{
    if (rtl::OUString(rName)==rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(STR_DB_LOCAL_NONAME)))
        DBG_ASSERT(false,"search for noname string");
    ScDBData aDataObj( rName, 0,0,0,0,0 );
    return Search( &aDataObj, rIndex );
}

void ScDBCollection::DeleteOnTab( SCTAB nTab )
{
    sal_uInt16 nPos = 0;
    while ( nPos < nCount )
    {
        // look for output positions on the deleted sheet

        SCCOL nEntryCol1, nEntryCol2;
        SCROW nEntryRow1, nEntryRow2;
        SCTAB nEntryTab;
        static_cast<const ScDBData*>(At(nPos))->GetArea( nEntryTab, nEntryCol1, nEntryRow1, nEntryCol2, nEntryRow2 );
        if ( nEntryTab == nTab )
            AtFree(nPos);
        else
            ++nPos;
    }

    remove_if(maAnonDBs.begin(), maAnonDBs.end(), FindByTable(nTab));
}

void ScDBCollection::UpdateReference(UpdateRefMode eUpdateRefMode,
                                SCCOL nCol1, SCROW nRow1, SCTAB nTab1,
                                SCCOL nCol2, SCROW nRow2, SCTAB nTab2,
                                SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    for (sal_uInt16 i=0; i<nCount; i++)
    {
        ((ScDBData*)pItems[i])->UpdateReference(
            pDoc, eUpdateRefMode,
            nCol1, nRow1, nTab1, nCol2, nRow2, nTab2, nDx, nDy, nDz);
    }
    ScDBData* pData = pDoc->GetAnonymousDBData(nTab1);
    if (pData)
    {
        if (nTab1 == nTab2 && nDz == 0)
        {
            pData->UpdateReference(
                pDoc, eUpdateRefMode,
                nCol1, nRow1, nTab1, nCol2, nRow2, nTab2, nDx, nDy, nDz);
        }
        else
        {
            //this will perhabs break undo
        }
    }

    UpdateRefFunc func(pDoc, eUpdateRefMode, nCol1, nRow1, nTab1, nCol2, nRow2, nTab2, nDx, nDy, nDz);
    for_each(maAnonDBs.begin(), maAnonDBs.end(), func);
}


void ScDBCollection::UpdateMoveTab( SCTAB nOldPos, SCTAB nNewPos )
{
    //  wenn nOldPos vor nNewPos liegt, ist nNewPos schon angepasst

    for (sal_uInt16 i=0; i<nCount; i++)
    {
        ScDBData* pData = (ScDBData*)pItems[i];
        pData->UpdateMoveTab(nOldPos, nNewPos);
    }

    UpdateMoveTabFunc func(nOldPos, nNewPos);
    for_each(maAnonDBs.begin(), maAnonDBs.end(), func);
}


ScDBData* ScDBCollection::FindIndex(sal_uInt16 nIndex)
{
    sal_uInt16 i = 0;
    while (i < nCount)
    {
        if ((*this)[i]->GetIndex() == nIndex)
            return (*this)[i];
        i++;
    }
    return NULL;
}

sal_Bool ScDBCollection::Insert(ScDataObject* pScDataObject)
{
    ScDBData* pData = (ScDBData*) pScDataObject;
    if (!pData->GetIndex())     // schon gesetzt?
        pData->SetIndex(nEntryIndex++);
    sal_Bool bInserted = ScSortedCollection::Insert(pScDataObject);
    if ( bInserted && pData->HasImportParam() && !pData->HasImportSelection() )
    {
        pData->SetRefreshHandler( GetRefreshHandler() );
        pData->SetRefreshControl( pDoc->GetRefreshTimerControlAddress() );
    }
    return bInserted;
}

ScDBData* ScDBCollection::GetDBNearCursor(SCCOL nCol, SCROW nRow, SCTAB nTab )
{
    ScDBData* pNearData = NULL;
    SCTAB nAreaTab;
    SCCOL nStartCol, nEndCol;
    SCROW nStartRow, nEndRow;
    for (sal_uInt16 i = 0; i < nCount; i++)
    {
        ScDBData* pDB = (ScDBData*)pItems[i];
        pDB->GetArea( nAreaTab, nStartCol, nStartRow, nEndCol, nEndRow );
        if ( nTab == nAreaTab && nCol+1 >= nStartCol && nCol <= nEndCol+1 &&
                                 nRow+1 >= nStartRow && nRow <= nEndRow+1 )
        {
            if ( nCol < nStartCol || nCol > nEndCol || nRow < nStartRow || nRow > nEndRow )
            {
                if (!pNearData)
                    pNearData = pDB;    // ersten angrenzenden Bereich merken
            }
            else
                return pDB;             // nicht "unbenannt" und Cursor steht wirklich drin
        }
    }
    if (pNearData)
        return pNearData;               // angrenzender, wenn nichts direkt getroffen
    return pDoc->GetAnonymousDBData(nTab);                  // "unbenannt" nur zurueck, wenn sonst nichts gefunden
}

const ScDBData* ScDBCollection::findAnonAtCursor(SCCOL nCol, SCROW nRow, SCTAB nTab, bool bStartOnly) const
{
    AnonDBsType::const_iterator itr = find_if(
        maAnonDBs.begin(), maAnonDBs.end(), FindByCursor(nCol, nRow, nTab, bStartOnly));
    return itr == maAnonDBs.end() ? NULL : &(*itr);
}

const ScDBData* ScDBCollection::findAnonByRange(const ScRange& rRange) const
{
    AnonDBsType::const_iterator itr = find_if(
        maAnonDBs.begin(), maAnonDBs.end(), FindByRange(rRange));
    return itr == maAnonDBs.end() ? NULL : &(*itr);
}

ScDBData* ScDBCollection::getAnonByRange(const ScRange& rRange)
{
    const ScDBData* pData = findAnonByRange(rRange);
    if (!pData)
    {
        // Insert a new db data.  They all have identical names.
        rtl::OUString aName(RTL_CONSTASCII_USTRINGPARAM(STR_DB_GLOBAL_NONAME));
        ::std::auto_ptr<ScDBData> pNew(new ScDBData(
            aName, rRange.aStart.Tab(), rRange.aStart.Col(), rRange.aStart.Row(),
            rRange.aEnd.Col(), rRange.aEnd.Row(), true, false));
        pData = pNew.get();
        maAnonDBs.push_back(pNew);
    }
    return const_cast<ScDBData*>(pData);
}

void ScDBCollection::insertAnonRange(ScDBData* pData)
{
    rtl::OUString aName(RTL_CONSTASCII_USTRINGPARAM(STR_DB_GLOBAL_NONAME));
    ::std::auto_ptr<ScDBData> pNew(pData);
    maAnonDBs.push_back(pNew);
}

const ScDBCollection::AnonDBsType& ScDBCollection::getAnonRanges() const
{
    return maAnonDBs;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
