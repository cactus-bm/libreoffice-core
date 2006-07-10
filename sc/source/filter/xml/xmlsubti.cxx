/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlsubti.cxx,v $
 *
 *  $Revision: 1.45 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-10 12:44:24 $
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
#include "filt_pch.hxx"
#endif

#pragma hdrstop

// INCLUDE ---------------------------------------------------------------

#ifndef SC_XMLSUBTI_HXX
#include "xmlsubti.hxx"
#endif
#include "global.hxx"
#include "xmlstyli.hxx"
#include "xmlimprt.hxx"
#include "document.hxx"
#ifndef _SC_XMLCONVERTER_HXX
#include "XMLConverter.hxx"
#endif
#ifndef SC_DOCUNO_HXX
#include "docuno.hxx"
#endif
#ifndef _SC_XMLSTYLESIMPORTHELPER_HXX
#include "XMLStylesImportHelper.hxx"
#endif

#include <xmloff/xmltkmap.hxx>
#include <xmloff/nmspmap.hxx>
#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif
#ifndef _XMLOFF_XMLERROR_HXX
#include <xmloff/xmlerror.hxx>
#endif

#ifndef _COM_SUN_STAR_SHEET_XSPREADSHEETDOCUMENT_HPP_
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMERGEABLE_HPP_
#include <com/sun/star/util/XMergeable.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XSHEETCELLRANGE_HPP_
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XCELLRANGEADDRESSABLE_HPP_
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_CELLINSERTMODE_HPP_
#include <com/sun/star/sheet/CellInsertMode.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XCELLRANGEMOVEMENT_HPP_
#include <com/sun/star/sheet/XCellRangeMovement.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XPROTECTABLE_HPP_
#include <com/sun/star/util/XProtectable.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XARRAYFORMULARANGE_HPP_
#include <com/sun/star/sheet/XArrayFormulaRange.hpp>
#endif

//------------------------------------------------------------------

using namespace com::sun::star;

ScMyTableData::ScMyTableData(sal_Int32 nSheet, sal_Int32 nCol, sal_Int32 nRow)
    :   nColsPerCol(nDefaultColCount, 1),
        nRealCols(nDefaultColCount + 1, 0),
        nRowsPerRow(nDefaultRowCount, 1),
        nRealRows(nDefaultRowCount + 1, 0),
        nChangedCols()
{
    aTableCellPos.Sheet = nSheet;
    aTableCellPos.Column = nCol;
    aTableCellPos.Row = nRow;

    for (sal_Int32 i = 0; i < 3; ++i)
        nRealCols[i] = i;
    for (sal_Int32 j = 0; j < 3; ++j)
        nRealRows[j] = j;

    nSpannedCols = 1;
    nColCount = 0;
    nSubTableSpanned = 1;
}

ScMyTableData::~ScMyTableData()
{
}

void ScMyTableData::AddRow()
{
    ++aTableCellPos.Row;
    if (static_cast<sal_uInt32>(aTableCellPos.Row) >= nRowsPerRow.size())
    {
        nRowsPerRow.resize(nRowsPerRow.size() + nDefaultRowCount, 1);
        nRealRows.resize(nRowsPerRow.size() + nDefaultRowCount + 1, 0);
    }
    nRealRows[aTableCellPos.Row + 1] = nRealRows[aTableCellPos.Row] + nRowsPerRow[aTableCellPos.Row];
}

void ScMyTableData::AddColumn()
{
    ++aTableCellPos.Column;
    if (static_cast<sal_uInt32>(aTableCellPos.Column) >= nColsPerCol.size())
    {
        nColsPerCol.resize(nColsPerCol.size() + nDefaultColCount, 1);
        nRealCols.resize(nColsPerCol.size() + nDefaultColCount + 1, 0);
    }
    nRealCols[aTableCellPos.Column + 1] = nRealCols[aTableCellPos.Column] + nColsPerCol[aTableCellPos.Column];
}

sal_Int32 ScMyTableData::FindNextCol(const sal_Int32 nIndex) const
{
    sal_Int32 i = nIndex;
    while(nRealCols[i] < 0)
        ++i;
    return nRealCols[i];
}

sal_Int32 ScMyTableData::GetRealCols(const sal_Int32 nIndex, const sal_Bool bIsNormal) const
{
    return (nIndex < 0) ? 0 : nRealCols[nIndex];
}

sal_Int32 ScMyTableData::GetChangedCols(const sal_Int32 nFromIndex, const sal_Int32 nToIndex) const
{
    ScMysalIntList::const_iterator i(nChangedCols.begin());
    ScMysalIntList::const_iterator endi(nChangedCols.end());
    while ((i != endi) && ((*i < nToIndex) && !(*i >= nFromIndex)))
        ++i;
    if (i == endi)
        return -1;
    else
        if ((*i >= nFromIndex) && (*i < nToIndex))
            return *i;
        else
            return -1;
}

void ScMyTableData::SetChangedCols(const sal_Int32 nValue)
{
    ScMysalIntList::iterator i(nChangedCols.begin());
    ScMysalIntList::iterator endi(nChangedCols.end());
    while ((i != endi) && (*i < nValue))
    {
        sal_Int32 nTemp = *i;
        ++i;
    }
    if ((i == endi) || (*i != nValue))
        nChangedCols.insert(i, nValue);
}

/*******************************************************************************************************************************/

ScMyTables::ScMyTables(ScXMLImport& rTempImport)
    : rImport(rTempImport),
    nTableCount( 0 ),
    nCurrentSheet( -1 ),
    nCurrentDrawPage( -1 ),
    nCurrentXShapes( -1 ),
    aResizeShapes(rTempImport),
    nCurrentColStylePos(0)
{
    aTableVec.resize(nDefaultTabCount, NULL);
}

ScMyTables::~ScMyTables()
{
    ScMyTableData* pTable;
    while (nTableCount > 0)
    {
        pTable = aTableVec[nTableCount - 1];
        delete pTable;
        aTableVec[nTableCount - 1] = NULL;
        --nTableCount;
    }
}

void ScMyTables::NewSheet(const rtl::OUString& sTableName, const rtl::OUString& sStyleName,
                        const sal_Bool bTempProtection, const rtl::OUString& sTempPassword)
{
    if (rImport.GetModel().is())
    {
        nCurrentColStylePos = 0;
        sCurrentSheetName = sTableName;
        ScMyTableData* aTable;
        while (nTableCount > 0)
        {
            aTable = aTableVec[nTableCount - 1];
            delete aTable;
            aTableVec[nTableCount - 1] = NULL;
            --nTableCount;
        }
        ++nCurrentSheet;

        bProtection = bTempProtection;
        sPassword = sTempPassword;
        uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( rImport.GetModel(), uno::UNO_QUERY );
        if ( xSpreadDoc.is() )
        {
            uno::Reference <sheet::XSpreadsheets> xSheets(xSpreadDoc->getSheets());
            if (xSheets.is())
            {
                if (nCurrentSheet > 0)
                {
                    try
                    {
                        xSheets->insertNewByName(sTableName, nCurrentSheet);
                    }
                    catch ( uno::RuntimeException& )
                    {
                        ScDocument *pDoc = ScXMLConverter::GetScDocument(rImport.GetModel());
                        if (pDoc)
                        {
                            rImport.LockSolarMutex();
                            String sTabName(String::CreateFromAscii("Table"));
                            pDoc->CreateValidTabName(sTabName);
                            rtl::OUString sOUTabName(sTabName);
                            xSheets->insertNewByName(sOUTabName, nCurrentSheet);
                            rImport.UnlockSolarMutex();
                        }
                    }
                }
                uno::Reference <container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
                if ( xIndex.is() )
                {
                    xCurrentSheet.set(xIndex->getByIndex(nCurrentSheet), uno::UNO_QUERY);
                    if ( xCurrentSheet.is() )
                    {
                        xCurrentCellRange.set(xCurrentSheet, uno::UNO_QUERY);
                        if (!(nCurrentSheet > 0))
                        {
                            uno::Reference < container::XNamed > xNamed(xCurrentSheet, uno::UNO_QUERY );
                            if ( xNamed.is() )
                                try
                                {
                                    xNamed->setName(sTableName);
                                }
                                catch ( uno::RuntimeException& )
                                {
                                    ScDocument *pDoc = ScXMLConverter::GetScDocument(rImport.GetModel());
                                    if (pDoc)
                                    {
                                        rImport.LockSolarMutex();
                                        String sTabName(String::CreateFromAscii("Table"));
                                        pDoc->CreateValidTabName(sTabName);
                                        rtl::OUString sOUTabName(sTabName);
                                        xNamed->setName(sOUTabName);
                                        rImport.UnlockSolarMutex();
                                    }
                                }
                        }
                        rImport.SetTableStyle(sStyleName);

                        if ( sStyleName.getLength() )
                        {
                            // #i57869# All table style properties for all sheets are now applied here,
                            // before importing the contents.
                            // This is needed for the background color.
                            // Sheet visibility has special handling in ScDocFunc::SetTableVisible to
                            // allow hiding the first sheet.
                            // RTL layout is only remembered, not actually applied, so the shapes can
                            // be loaded before mirroring.

                            uno::Reference <beans::XPropertySet> xProperties(xCurrentSheet, uno::UNO_QUERY);
                            if (xProperties.is())
                            {
                                XMLTableStylesContext *pStyles = (XMLTableStylesContext *)rImport.GetAutoStyles();
                                if (pStyles)
                                {
                                    XMLTableStyleContext* pStyle = (XMLTableStyleContext *)pStyles->FindStyleChildContext(
                                        XML_STYLE_FAMILY_TABLE_TABLE, sStyleName, sal_True);
                                    if (pStyle)
                                        pStyle->FillPropertySet(xProperties);
                                }
                            }
                        }
                    }

                }
            }
        }
    }

    NewTable(1);
}

sal_Bool ScMyTables::IsMerged (const uno::Reference <table::XCellRange>& xCellRange, const sal_Int32 nCol, const sal_Int32 nRow,
                            table::CellRangeAddress& aCellAddress) const
{
    uno::Reference <util::XMergeable> xMergeable (xCellRange->getCellRangeByPosition(nCol,nRow,nCol,nRow), uno::UNO_QUERY);
    if (xMergeable.is())
    {
        uno::Reference<sheet::XSheetCellRange> xMergeSheetCellRange (xMergeable, uno::UNO_QUERY);
        uno::Reference<sheet::XSpreadsheet> xTable(xMergeSheetCellRange->getSpreadsheet());
        uno::Reference<sheet::XSheetCellCursor> xMergeSheetCursor(xTable->createCursorByRange(xMergeSheetCellRange));
        if (xMergeSheetCursor.is())
        {
            xMergeSheetCursor->collapseToMergedArea();
            uno::Reference<sheet::XCellRangeAddressable> xMergeCellAddress (xMergeSheetCursor, uno::UNO_QUERY);
            if (xMergeCellAddress.is())
            {
                aCellAddress = xMergeCellAddress->getRangeAddress();
                if (aCellAddress.StartColumn == nCol && aCellAddress.EndColumn == nCol &&
                    aCellAddress.StartRow == nRow && aCellAddress.EndRow == nRow)
                    return sal_False;
                else
                    return sal_True;
            }
        }
    }
    return sal_False;
}

void ScMyTables::UnMerge()
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        if (IsMerged(xCurrentCellRange, GetRealCellPos().Column, GetRealCellPos().Row, aCellAddress))
        {
            //unmerge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(sal_False);
        }
    }
}

void ScMyTables::DoMerge(sal_Int32 nCount)
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        if (IsMerged(xCurrentCellRange, GetRealCellPos().Column, GetRealCellPos().Row, aCellAddress))
        {
            //unmerge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(sal_False);
        }

        //merge
        uno::Reference <table::XCellRange> xMergeCellRange;
        if (nCount == -1)
            xMergeCellRange.set(xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.EndColumn
                                                    + aTableVec[nTableCount - 1]->GetColsPerCol(aTableVec[nTableCount - 1]->GetColumn()) - 1,
                                                    aCellAddress.EndRow
                                                    + aTableVec[nTableCount - 1]->GetRowsPerRow(aTableVec[nTableCount - 1]->GetRow()) - 1));
        else
            xMergeCellRange.set(xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.StartColumn
                                                    + nCount - 1,
                                                    aCellAddress.EndRow));
        uno::Reference <util::XMergeable> xMergeable (xMergeCellRange, uno::UNO_QUERY);
        if (xMergeable.is())
            xMergeable->merge(sal_True);
    }
}

void ScMyTables::InsertRow()
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        sal_Int32 nRow(GetRealCellPos().Row);
        for (sal_Int32 j = 0; j < GetRealCellPos().Column - aTableVec[nTableCount - 1]->GetColumn() - 1; ++j)
        {
            if (IsMerged(xCurrentCellRange, j, nRow - 1, aCellAddress))
            {
                //unmerge
                uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                        aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
                if (xMergeable.is())
                    xMergeable->merge(sal_False);
            }

            //merge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                    aCellAddress.EndColumn, aCellAddress.EndRow + 1), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(sal_True);
            j += aCellAddress.EndColumn - aCellAddress.StartColumn;
        }
        rImport.GetStylesImportHelper()->InsertRow(nRow, nCurrentSheet, rImport.GetDocument());
    }
}

void ScMyTables::NewRow()
{
    if (nTableCount > 1)
        if (aTableVec[nTableCount - 1]->GetRealRows(aTableVec[nTableCount - 1]->GetRow()) >
            aTableVec[nTableCount - 2]->GetRowsPerRow(aTableVec[nTableCount - 2]->GetRow()) - 1)
        {
            if (GetRealCellPos().Column > 0)
                InsertRow();
            for (sal_Int16 i = nTableCount - 1; i > 0; i--)
            {
                sal_Int32 nRow = aTableVec[i - 1]->GetRow();
                aTableVec[i - 1]->SetRowsPerRow(nRow,
                    aTableVec[i - 1]->GetRowsPerRow(nRow) + 1);
                aTableVec[i - 1]->SetRealRows(nRow + 1,
                    aTableVec[i - 1]->GetRealRows(nRow)
                    + aTableVec[i - 1]->GetRowsPerRow(nRow));
            }
        }
}

void ScMyTables::AddRow()
{
    aTableVec[nTableCount - 1]->AddRow();
    aTableVec[nTableCount - 1]->SetFirstColumn();
    sal_Int32 nRow = aTableVec[nTableCount - 1]->GetRow();
    if (nRow > 0)
        NewRow();
    aTableVec[nTableCount - 1]->SetRealRows(nRow + 1,
        aTableVec[nTableCount - 1]->GetRealRows(nRow)
        + aTableVec[nTableCount - 1]->GetRowsPerRow(nRow));
}

void ScMyTables::SetRowStyle(const rtl::OUString& rCellStyleName)
{
    rImport.GetStylesImportHelper()->SetRowStyle(rCellStyleName);
}

void ScMyTables::CloseRow()
{
    sal_Int32 nToMerge;
    sal_Int32 nSpannedCols(aTableVec[nTableCount - 1]->GetSpannedCols());
    sal_Int32 nColCount(aTableVec[nTableCount - 1]->GetColCount());
    sal_Int32 nCol(aTableVec[nTableCount - 1]->GetColumn());
    sal_Int32 nColsPerCol(aTableVec[nTableCount - 1]->GetColsPerCol(nCol));
    if (nSpannedCols > nColCount)
        nToMerge = aTableVec[nTableCount - 1]->GetChangedCols(nCol, nCol + nColsPerCol + nSpannedCols - nColCount);
    else
        nToMerge = aTableVec[nTableCount - 1]->GetChangedCols(nCol, nCol + nColsPerCol);
    if ((nToMerge > nCol) && (aTableVec[nTableCount - 1]->GetSubTableSpanned() == 1))
        DoMerge(nColsPerCol + aTableVec[nTableCount - 1]->GetColsPerCol(nToMerge) - 1);
}


void ScMyTables::InsertColumn()
{
    if ( xCurrentCellRange.is() )
    {
        table::CellRangeAddress aCellAddress;
        sal_Int32 nCol(GetRealCellPos().Column);
        for (sal_Int32 j = 0; j <= GetRealCellPos().Row - aTableVec[nTableCount - 1]->GetRow() - 1; ++j)
        {
            table::CellRangeAddress aTempCellAddress;
            if (IsMerged(xCurrentCellRange, nCol - 1, j, aCellAddress))
            {
                //unmerge
                uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                        aCellAddress.EndColumn, aCellAddress.EndRow), uno::UNO_QUERY);
                if (xMergeable.is())
                    xMergeable->merge(sal_False);
                aTempCellAddress = aCellAddress;
                aTempCellAddress.StartColumn = aTempCellAddress.EndColumn + 1;
                aTempCellAddress.EndColumn = aTempCellAddress.StartColumn;
            }
            else
            {
                aTempCellAddress = aCellAddress;
                aTempCellAddress.StartColumn += 1;
                aTempCellAddress.EndColumn = aTempCellAddress.StartColumn;
            }

            //insert Cell
            sheet::CellInsertMode aCellInsertMode(sheet::CellInsertMode_RIGHT);
            uno::Reference <sheet::XCellRangeMovement>  xCellRangeMovement (xCurrentSheet, uno::UNO_QUERY);
            xCellRangeMovement->insertCells(aTempCellAddress, aCellInsertMode);

            //merge
            uno::Reference <util::XMergeable> xMergeable (xCurrentCellRange->getCellRangeByPosition(aCellAddress.StartColumn, aCellAddress.StartRow,
                                                        aCellAddress.EndColumn + 1, aCellAddress.EndRow), uno::UNO_QUERY);
            if (xMergeable.is())
                xMergeable->merge(sal_True);
            j += aCellAddress.EndRow - aCellAddress.StartRow;
        }
        rImport.GetStylesImportHelper()->InsertCol(nCol, nCurrentSheet, rImport.GetDocument());
    }
}

void ScMyTables::NewColumn(sal_Bool bIsCovered)
{
    if (!bIsCovered)
    {
        sal_Int32 nColCount(aTableVec[nTableCount - 1]->GetColCount());
        sal_Int32 nSpannedCols(aTableVec[nTableCount - 1]->GetSpannedCols());
        if ( (nSpannedCols > nColCount) &&
            (aTableVec[nTableCount - 1]->GetRow() == 0) &&
            (aTableVec[nTableCount - 1]->GetColumn() == 0) )
        {
            if (nColCount > 0)
            {
                sal_Int32 FirstColsSpanned(nSpannedCols / nColCount);
                sal_Int32 LastColSpanned(FirstColsSpanned
                    + (nSpannedCols % nColCount));
                for (sal_Int32 i = 0; i < nColCount - 1; ++i)
                {
                    aTableVec[nTableCount - 1]->SetColsPerCol(i, FirstColsSpanned);
                    aTableVec[nTableCount - 1]->SetRealCols(i + 1,
                        aTableVec[nTableCount - 1]->GetRealCols(i)
                        + FirstColsSpanned);
                }
                aTableVec[nTableCount - 1]->SetColsPerCol(nColCount - 1, LastColSpanned);
                aTableVec[nTableCount - 1]->SetRealCols(nColCount - 1 + 1,
                    aTableVec[nTableCount - 1]->GetRealCols(nColCount - 1)
                    + LastColSpanned);
            }
        }
        sal_Int32 nTemp(aTableVec[nTableCount - 1]->GetRealCols(aTableVec[nTableCount - 1]->GetColumn()));
        if (aTableVec[nTableCount - 1]->GetRealCols(aTableVec[nTableCount - 1]->GetColumn()) > nSpannedCols - 1)
        {
            if ( aTableVec[nTableCount - 1]->GetRow() == 0)
            {
                InsertColumn();
                for (sal_Int16 i = nTableCount - 1; i > 0; i--)
                {
                    sal_Int32 nColPos = aTableVec[i - 1]->GetColumn() +
                        aTableVec[i]->GetSpannedCols() - 1;
                    aTableVec[i - 1]->SetColsPerCol(nColPos,
                        aTableVec[i - 1]->GetColsPerCol(nColPos) +
                        aTableVec[nTableCount - 1]->GetColsPerCol(aTableVec[nTableCount - 1]->GetColumn()));
                    aTableVec[i - 1]->SetRealCols(nColPos + 1,
                        aTableVec[i - 1]->GetRealCols(nColPos)
                        + aTableVec[i - 1]->GetColsPerCol(nColPos));
                    aTableVec[i - 1]->SetChangedCols(nColPos);
                }
            }
        }
    }
}

void ScMyTables::AddColumn(sal_Bool bIsCovered)
{
    aTableVec[nTableCount - 1]->AddColumn();
    if (aTableVec[nTableCount - 1]->GetSubTableSpanned() > 1)
        aTableVec[nTableCount - 1]->SetSubTableSpanned(aTableVec[nTableCount - 1]->GetSubTableSpanned() - 1);
    else
    {
        NewColumn(bIsCovered);
    //  if (!bIsCovered)
            aTableVec[nTableCount - 1]->SetRealCols(aTableVec[nTableCount - 1]->GetColumn() + 1,
                aTableVec[nTableCount - 1]->GetRealCols(aTableVec[nTableCount - 1]->GetColumn())
                + aTableVec[nTableCount - 1]->GetColsPerCol(aTableVec[nTableCount - 1]->GetColumn()));
        if ((!bIsCovered) || (bIsCovered &&
            (aTableVec[nTableCount - 1]->GetColsPerCol(aTableVec[nTableCount - 1]->GetColumn()) > 1)))
        {
            if ((aTableVec[nTableCount - 1]->GetRowsPerRow(aTableVec[nTableCount - 1]->GetRow()) > 1) ||
                (aTableVec[nTableCount - 1]->GetColsPerCol(aTableVec[nTableCount - 1]->GetColumn()) > 1))
                DoMerge();
        }
    }
}

void ScMyTables::NewTable(sal_Int32 nTempSpannedCols)
{
    ++nTableCount;
    if (static_cast<sal_uInt32>(nTableCount) >= aTableVec.size())
        aTableVec.resize(aTableVec.size() + nDefaultTabCount);
    ScMyTableData* aTable(new ScMyTableData(nCurrentSheet));
    if (nTableCount > 1)
    {
        sal_Int32 nCol(aTableVec[nTableCount - 2]->GetColumn());
        sal_Int32 nColCount(aTableVec[nTableCount - 2]->GetColCount());
        sal_Int32 nColsPerCol(aTableVec[nTableCount - 2]->GetColsPerCol(nCol));
        sal_Int32 nSpannedCols(aTableVec[nTableCount - 2]->GetSpannedCols());
        sal_Int32 nTemp(nSpannedCols - nColCount);
        sal_Int32 nTemp2(nCol - (nColCount - 1));
        if ((nTemp > 0) && (nTemp2 == 0))
            nTempSpannedCols *= (nTemp + 1);
        else
            if (nColsPerCol > 1)
                nTempSpannedCols *= nColsPerCol;

        sal_Int32 nToMerge;
        if (nSpannedCols > nColCount)
            nToMerge = aTableVec[nTableCount - 2]->GetChangedCols(nCol, nCol + nColsPerCol + nSpannedCols - nColCount);
        else
            nToMerge = aTableVec[nTableCount - 2]->GetChangedCols(nCol, nCol + nColsPerCol);
        if (nToMerge > nCol)
            nTempSpannedCols += nToMerge;
    }
    aTable->SetSpannedCols(nTempSpannedCols);
    aTableVec[nTableCount - 1] = aTable;
    if (nTableCount > 1)
    {
        aTableVec[nTableCount - 2]->SetSubTableSpanned(aTable->GetSpannedCols());
        UnMerge();
    }
}

void ScMyTables::UpdateRowHeights()
{
    if (rImport.GetModel().is())
    {
        rImport.LockSolarMutex();
        // update automatic row heights
        SCTAB nTableCount(rImport.GetDocument() ? rImport.GetDocument()->GetTableCount() : 0);
        for (SCTAB i = 0; i < nTableCount; ++i)
            ScModelObj::getImplementation(rImport.GetModel())->AdjustRowHeight( 0, MAXROW, i );
        rImport.UnlockSolarMutex();
    }
}

void ScMyTables::DeleteTable()
{
    rImport.LockSolarMutex();

    nCurrentColStylePos = 0;
    if (nTableCount > 0)
    {
        ScMyTableData* aTable = aTableVec[nTableCount - 1];
        delete aTable;
        aTableVec[nTableCount - 1] = NULL;
        nTableCount--;
    }
    if (nTableCount == 0) // only set the styles if all subtables are importet and the table is finished
    {
        rImport.GetStylesImportHelper()->SetStylesToRanges();
        rImport.SetStylesToRangesFinished();
    }

    //#i48793#; has to be set before protection
    if (!aMatrixRangeList.empty())
    {
        ScMyMatrixRangeList::iterator aItr = aMatrixRangeList.begin();
        ScMyMatrixRangeList::iterator aEndItr = aMatrixRangeList.end();
        while(aItr != aEndItr)
        {
            SetMatrix(aItr->aRange, aItr->sFormula);
            ++aItr;
        }
        aMatrixRangeList.clear();
    }

    if (rImport.GetDocument() && bProtection)
    {
        uno::Sequence<sal_Int8> aPass;
        SvXMLUnitConverter::decodeBase64(aPass, sPassword);
        rImport.GetDocument()->SetTabProtection(static_cast<SCTAB>(nCurrentSheet), bProtection, aPass);
        /*uno::Reference <util::XProtectable> xProtectable(xCurrentSheet, uno::UNO_QUERY);
        if (xProtectable.is())
        {
            rtl::OUString sKey;
            xProtectable->protect(sKey);
        }*/
    }

    rImport.UnlockSolarMutex();

    //#95582#; find out whether it was possible to set the sheet name
    // test it here, because if it is a linked table the name is changed by importing
    // the linking informations
    uno::Reference < container::XNamed > xNamed(xCurrentSheet, uno::UNO_QUERY );
    if ( xNamed.is() )
    {
        rtl::OUString sCurrentName(xNamed->getName());
        if (sCurrentName != sCurrentSheetName && rImport.GetDocument())
        {
            rImport.GetDocument()->RenameTab( static_cast<SCTAB>(nCurrentSheet),
                sCurrentSheetName, sal_False, sal_True);

/*          rtl::OUString sErrorMessage(RTL_CONSTASCII_USTRINGPARAM("Could not create a table with the name "));
            sErrorMessage += sCurrentSheetName;
            sErrorMessage += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(". The new name is "));
            sErrorMessage += sCurrentName;
            uno::Sequence<rtl::OUString> aSeq(1);
            aSeq[0] = sErrorMessage;
            uno::Reference<xml::sax::XLocator> xLocator;
            rImport.SetError(XMLERROR_API | XMLERROR_FLAG_ERROR, aSeq, rtl::OUString(), xLocator);*/
        }
    }
}

table::CellAddress ScMyTables::GetRealCellPos()
{
    sal_Int32 nRow(0);
    sal_Int32 nCol(0);
    for (sal_Int32 i = 1; i <= nTableCount; ++i)
    {
        nCol += aTableVec[i - 1]->GetRealCols(aTableVec[i - 1]->GetColumn());
        nRow += aTableVec[i - 1]->GetRealRows(aTableVec[i - 1]->GetRow());
    }
    aRealCellPos.Row = nRow;
    aRealCellPos.Column = nCol;
    aRealCellPos.Sheet = nCurrentSheet;
    return aRealCellPos;
}

void ScMyTables::AddColCount(sal_Int32 nTempColCount)
{
    aTableVec[nTableCount - 1]->SetColCount(aTableVec[nTableCount - 1]->GetColCount() + nTempColCount);
}

void ScMyTables::AddColStyle(const sal_Int32 nRepeat, const rtl::OUString& rCellStyleName)
{
    DBG_ASSERT(nTableCount == 1, "not possible to use default styles on columns in subtables");
    rImport.GetStylesImportHelper()->AddColumnStyle(rCellStyleName, nCurrentColStylePos, nRepeat);
    nCurrentColStylePos += nRepeat;
}

uno::Reference< drawing::XDrawPage > ScMyTables::GetCurrentXDrawPage()
{
    if( (nCurrentSheet != nCurrentDrawPage) || !xDrawPage.is() )
    {
        uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier( xCurrentSheet, uno::UNO_QUERY );
        if( xDrawPageSupplier.is() )
            xDrawPage.set(xDrawPageSupplier->getDrawPage());
        nCurrentDrawPage = nCurrentSheet;
    }
    return xDrawPage;
}

uno::Reference< drawing::XShapes > ScMyTables::GetCurrentXShapes()
{
    if( (nCurrentSheet != nCurrentXShapes) || !xShapes.is() )
    {
        xShapes.set(GetCurrentXDrawPage(), uno::UNO_QUERY);
        rImport.GetShapeImport()->startPage(xShapes);
        rImport.GetShapeImport()->pushGroupForSorting ( xShapes );
        nCurrentXShapes = nCurrentSheet;
        return xShapes;
    }
    else
        return xShapes;
}

sal_Bool ScMyTables::HasDrawPage()
{
    return !((nCurrentSheet != nCurrentDrawPage) || !xDrawPage.is());
}

sal_Bool ScMyTables::HasXShapes()
{
    return !((nCurrentSheet != nCurrentXShapes) || !xShapes.is());
}

void ScMyTables::AddShape(uno::Reference <drawing::XShape>& rShape,
    rtl::OUString* pRangeList,
    table::CellAddress& rStartAddress, table::CellAddress& rEndAddress,
    sal_Int32 nEndX, sal_Int32 nEndY)
{
    aResizeShapes.AddShape(rShape, pRangeList, rStartAddress, rEndAddress, nEndX, nEndY);
}

void ScMyTables::AddMatrixRange(sal_Int32 nStartColumn, sal_Int32 nStartRow, sal_Int32 nEndColumn, sal_Int32 nEndRow, const rtl::OUString& rFormula)
{
    DBG_ASSERT(nEndRow >= nStartRow, "wrong row order");
    DBG_ASSERT(nEndColumn >= nStartColumn, "wrong column order");
    table::CellRangeAddress aRange;
    aRange.StartColumn = nStartColumn;
    aRange.StartRow = nStartRow;
    aRange.EndColumn = nEndColumn;
    aRange.EndRow = nEndRow;
    aRange.Sheet = nCurrentSheet;
    ScMatrixRange aMRange(aRange, rFormula);
    aMatrixRangeList.push_back(aMRange);
}

sal_Bool ScMyTables::IsPartOfMatrix(sal_Int32 nColumn, sal_Int32 nRow)
{
    sal_Bool bResult(sal_False);
    if (!aMatrixRangeList.empty())
    {
        ScMyMatrixRangeList::iterator aItr(aMatrixRangeList.begin());
        ScMyMatrixRangeList::iterator aEndItr(aMatrixRangeList.end());
        sal_Bool bReady(sal_False);
        while(!bReady && aItr != aEndItr)
        {
            if (nCurrentSheet > aItr->aRange.Sheet)
            {
                DBG_ERROR("should never hapen, because the list should be cleared in DeleteTable");
                aItr = aMatrixRangeList.erase(aItr);
            }
            else if ((nRow > aItr->aRange.EndRow) && (nColumn > aItr->aRange.EndColumn))
            {
                SetMatrix(aItr->aRange, aItr->sFormula);
                aItr = aMatrixRangeList.erase(aItr);
            }
            else if (nColumn < aItr->aRange.StartColumn)
                bReady = sal_True;
            else if (nColumn >= aItr->aRange.StartColumn && nColumn <= aItr->aRange.EndColumn && nRow >= aItr->aRange.StartRow && nRow <= aItr->aRange.EndRow)
            {
                bReady = sal_True;
                bResult = sal_True;
            }
            else
                ++aItr;
        }
    }
    return bResult;
}

void ScMyTables::SetMatrix(const table::CellRangeAddress& rRange, const rtl::OUString& rFormula)
{
    uno::Reference <table::XCellRange> xMatrixCellRange(
        GetCurrentXCellRange()->getCellRangeByPosition(rRange.StartColumn, rRange.StartRow,
                    rRange.EndColumn, rRange.EndRow));
    if (xMatrixCellRange.is())
    {
        uno::Reference <sheet::XArrayFormulaRange> xArrayFormulaRange(xMatrixCellRange, uno::UNO_QUERY);
        if (xArrayFormulaRange.is())
            xArrayFormulaRange->setArrayFormula(rFormula);
    }
}
