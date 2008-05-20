/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dpcachetable.cxx,v $
 *
 * $Revision: 1.2 $
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

#include "dpcachetable.hxx"
#include "document.hxx"
#include "address.hxx"
#include "cell.hxx"
#include "dptabdat.hxx"
#include "dptabsrc.hxx"

#include <stdio.h>

#include <com/sun/star/i18n/LocaleDataItem.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <com/sun/star/sdbc/XResultSetMetaData.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/sheet/DataPilotFieldFilter.hpp>
#include <com/sun/star/sheet/DataPilotFieldGroupBy.hpp>

#include <memory>

using namespace ::com::sun::star;

using ::rtl::OUString;
using ::std::vector;
using ::std::set;
using ::std::hash_map;
using ::std::auto_ptr;
using ::com::sun::star::i18n::LocaleDataItem;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::com::sun::star::sheet::DataPilotFieldFilter;

static BOOL lcl_HasQueryEntry( const ScQueryParam& rParam )
{
    return rParam.GetEntryCount() > 0 &&
            rParam.GetEntry(0).bDoQuery;
}

ScSharedString::StringTable::StringTable() :
    mnStrCount(0)
{
    // empty string (ID = 0)
    maSharedStrings.push_back(String());
    maSharedStringIds.insert( SharedStrMap::value_type(String(), mnStrCount++) );
}

ScSharedString::StringTable::~StringTable()
{
}

sal_Int32 ScSharedString::StringTable::insertString(const String& aStr)
{
    SharedStrMap::const_iterator itr = maSharedStringIds.find(aStr),
        itrEnd = maSharedStringIds.end();

    if (itr == itrEnd)
    {
        // new string.
        maSharedStrings.push_back(aStr);
        maSharedStringIds.insert( SharedStrMap::value_type(aStr, mnStrCount) );
        return mnStrCount++;
    }

    // existing string.
    return itr->second;
}

sal_Int32 ScSharedString::StringTable::getStringId(const String& aStr)
{
    SharedStrMap::const_iterator itr = maSharedStringIds.find(aStr),
        itrEnd = maSharedStringIds.end();
    if (itr == itrEnd)
    {
        // string not found.
        return insertString(aStr);
    }
    return itr->second;
}

const String* ScSharedString::StringTable::getString(sal_Int32 nId) const
{
    if (nId >= mnStrCount)
        return NULL;

    return &maSharedStrings[nId];
}

// ----------------------------------------------------------------------------

// static
::osl::Mutex ScSharedString::maStrMutex;
ScSharedString::StringTable ScSharedString::maStringTable;

sal_Int32 ScSharedString::insertString(const String& aStr)
{
    ::osl::MutexGuard aGuard(maStrMutex);
    return maStringTable.insertString(aStr);
}

const String* ScSharedString::getString(sal_Int32 nId)
{
    ::osl::MutexGuard aGuard(maStrMutex);
    return maStringTable.getString(nId);
}

sal_Int32 ScSharedString::getStringId(const String& aStr)
{
    ::osl::MutexGuard aGuard(maStrMutex);
    return maStringTable.getStringId(aStr);
}

// ----------------------------------------------------------------------------

ScDPCacheTable::Cell::Cell() :
    mnCategoryRef(0),
    mnStrId(ScSharedString::EMPTY),
    mnType(SC_VALTYPE_EMPTY),
    mfValue(0.0),
    mbNumeric(false)
{
}

// ----------------------------------------------------------------------------

ScDPCacheTable::FilterItem::FilterItem() :
    mnMatchStrId(ScSharedString::EMPTY),
    mfValue(0.0),
    mbHasValue(false)
{
}

// ----------------------------------------------------------------------------

ScDPCacheTable::SingleFilter::SingleFilter()
{
}

ScDPCacheTable::SingleFilter::SingleFilter(sal_Int32 nMatchStrId, double fValue, bool bHasValue)
{
    maItem.mnMatchStrId = nMatchStrId;
    maItem.mfValue      = fValue;
    maItem.mbHasValue   = bHasValue;
}

bool ScDPCacheTable::SingleFilter::match(const Cell& rCell) const
{
    if (rCell.mnStrId != maItem.mnMatchStrId &&
        (!rCell.mbNumeric || rCell.mfValue != maItem.mfValue))
        return false;

    return true;
}

const String ScDPCacheTable::SingleFilter::getMatchString() const
{
    const String* pStr = ScSharedString::getString(maItem.mnMatchStrId);
    if (pStr)
        return *pStr;

    return String();
}

double ScDPCacheTable::SingleFilter::getMatchValue() const
{
    return maItem.mfValue;
}

bool ScDPCacheTable::SingleFilter::hasValue() const
{
    return maItem.mbHasValue;
}

// ----------------------------------------------------------------------------

ScDPCacheTable::GroupFilter::GroupFilter()
{
}

bool ScDPCacheTable::GroupFilter::match(const Cell& rCell) const
{
    vector<FilterItem>::const_iterator itrEnd = maItems.end();
    for (vector<FilterItem>::const_iterator itr = maItems.begin(); itr != itrEnd; ++itr)
    {
        bool bMatch = false;
        if (rCell.mbNumeric)
            bMatch = (itr->mfValue == rCell.mfValue);
        else
            bMatch = (itr->mnMatchStrId == rCell.mnStrId);

        if (bMatch)
            return true;
    }
    return false;
}

void ScDPCacheTable::GroupFilter::addMatchItem(const String& rStr, double fVal, bool bHasValue)
{
    sal_Int32 nStrId = ScSharedString::getStringId(rStr);
    FilterItem aItem;
    aItem.mnMatchStrId = nStrId;
    aItem.mfValue = fVal;
    aItem.mbHasValue = bHasValue;
    maItems.push_back(aItem);
}

// ----------------------------------------------------------------------------

ScDPCacheTable::Criterion::Criterion() :
    mnFieldIndex(-1),
    mpFilter(static_cast<FilterBase*>(NULL))
{
}

// ----------------------------------------------------------------------------

ScDPCacheTable::ScDPCacheTable()
{
}

ScDPCacheTable::~ScDPCacheTable()
{
}

sal_Int32 ScDPCacheTable::getHeaderSize() const
{
    return maHeader.size();
}

sal_Int32 ScDPCacheTable::getRowSize() const
{
    return maTable.size();
}

sal_Int32 ScDPCacheTable::getColSize() const
{
    return maTable.empty() ? 0 : maTable[0].size();
}

void ScDPCacheTable::fillTable(ScDocument* pDoc, const ScRange& rRange, const ScQueryParam& rQuery, BOOL* pSpecial,
                               bool bIgnoreEmptyRows)
{
    SCTAB nTab = rRange.aStart.Tab();
    SCCOL nStartCol = rRange.aStart.Col();
    SCROW nStartRow = rRange.aStart.Row();
    SCCOL nColCount = rRange.aEnd.Col() - rRange.aStart.Col() + 1;
    SCROW nRowCount = rRange.aEnd.Row() - rRange.aStart.Row() + 1;

    if (nRowCount <= 1 || nColCount <= 0)
        return;

    maTable.clear();
    maTable.reserve(nRowCount);
    maHeader.clear();
    maHeader.reserve(nColCount);
    maRowsVisible.clear();
    maRowsVisible.reserve(nRowCount);

    // Header row
    for (SCCOL nCol = 0; nCol < nColCount; ++nCol)
    {
        String aStr;
        pDoc->GetString(nCol + nStartCol, nStartRow, nTab, aStr);
        sal_Int32 nStrId = ScSharedString::insertString(aStr);
        maHeader.push_back(nStrId);
    }

    // Initialize field entries container.
    maFieldEntries.clear();
    maFieldEntries.reserve(nColCount);
    for (SCCOL nCol = 0; nCol < nColCount; ++nCol)
    {
        TypedStrCollectionPtr p(new TypedStrCollection);
        maFieldEntries.push_back(p);
    }

    vector<SCROW> aLastNonEmptyRows(nColCount, 0);

    // Data rows
    for (SCROW nRow = 1; nRow < nRowCount; ++nRow)
    {
        if ( lcl_HasQueryEntry(rQuery) && !pDoc->ValidQuery(nRow + nStartRow, nTab, rQuery, pSpecial) )
            // filtered out by standard filter.
            continue;

        if ( bIgnoreEmptyRows &&
             pDoc->IsBlockEmpty(nTab, nStartCol, nRow + nStartRow,
                                nStartCol + nColCount - 1, nRow + nStartRow) )
            // skip an empty row.
            continue;

        // Insert a new row into cache table.
        maRowsVisible.push_back(true);
        maTable.push_back( vector<Cell>() );
        maTable.back().reserve(nColCount);

        for (SCCOL nCol = 0; nCol < nColCount; ++nCol)
        {
            maTable.back().push_back( ScDPCacheTable::Cell() );
            Cell& rCell = maTable.back().back();
            rCell.mnCategoryRef = maTable.size()-1;

            if (nRow == 0 || pDoc->HasData(nStartCol + nCol, nStartRow + nRow, nTab))
                aLastNonEmptyRows[nCol] = maTable.size()-1;
            else
                rCell.mnCategoryRef = aLastNonEmptyRows[nCol];

            String aStr;
            pDoc->GetString(nStartCol + nCol, nStartRow + nRow, nTab, aStr);
            rCell.mnStrId = ScSharedString::insertString(aStr);
            rCell.mnType = SC_VALTYPE_STRING;
            rCell.mbNumeric = false;
            ScAddress aPos(nStartCol + nCol, nStartRow + nRow, nTab);
            getValueData(pDoc, aPos, rCell);

            TypedStrData* pNew;
            if (rCell.mbNumeric)
                pNew = new TypedStrData(aStr, rCell.mfValue, SC_STRTYPE_VALUE);
            else
                pNew = new TypedStrData(aStr);

            if (!maFieldEntries[nCol]->Insert(pNew))
                delete pNew;
        }
    }
}

void lcl_GetCellValue(const Reference<sdbc::XRow>& xRow, sal_Int32 nType, long nCol,
             const Date& rNullDate, ScDPCacheTable::Cell& rCell, String& rStr)
{
    short nNumType = NUMBERFORMAT_NUMBER;
    BOOL bEmptyFlag = FALSE;
    try
    {
        rStr = xRow->getString(nCol);
        rCell.mnStrId = ScSharedString::getStringId(rStr);
        rCell.mnType = SC_VALTYPE_STRING;

        switch (nType)
        {
            case sdbc::DataType::BIT:
            case sdbc::DataType::BOOLEAN:
            {
                nNumType = NUMBERFORMAT_LOGICAL;
                rCell.mfValue = xRow->getBoolean(nCol) ? 1 : 0;
                bEmptyFlag = (rCell.mfValue == 0.0 && xRow->wasNull());
                rCell.mbNumeric = true;
                rCell.mnType = SC_VALTYPE_VALUE;
            }
            break;

            case sdbc::DataType::TINYINT:
            case sdbc::DataType::SMALLINT:
            case sdbc::DataType::INTEGER:
            case sdbc::DataType::BIGINT:
            case sdbc::DataType::FLOAT:
            case sdbc::DataType::REAL:
            case sdbc::DataType::DOUBLE:
            case sdbc::DataType::NUMERIC:
            case sdbc::DataType::DECIMAL:
            {
                //! do the conversion here?
                rCell.mfValue = xRow->getDouble(nCol);
                bEmptyFlag = (rCell.mfValue == 0.0 && xRow->wasNull());
                rCell.mbNumeric = true;
                rCell.mnType = SC_VALTYPE_VALUE;
            }
            break;

            case sdbc::DataType::CHAR:
            case sdbc::DataType::VARCHAR:
            case sdbc::DataType::LONGVARCHAR:
                bEmptyFlag = (rStr.Len() == 0 && xRow->wasNull());
            break;

            case sdbc::DataType::DATE:
            {
                nNumType = NUMBERFORMAT_DATE;

                util::Date aDate = xRow->getDate(nCol);
                rCell.mfValue = Date(aDate.Day, aDate.Month, aDate.Year) - rNullDate;
                bEmptyFlag = xRow->wasNull();
                rCell.mbNumeric = true;
                rCell.mnType = SC_VALTYPE_VALUE;
            }
            break;

                //! case sdbc::DataType::TIME:
                //! case sdbc::DataType::TIMESTAMP:

            case sdbc::DataType::SQLNULL:
            case sdbc::DataType::BINARY:
            case sdbc::DataType::VARBINARY:
            case sdbc::DataType::LONGVARBINARY:
            default:
            break;
        }
    }
    catch (uno::Exception&)
    {
    }
}

void ScDPCacheTable::fillTable(const Reference<sdbc::XRowSet>& xRowSet, const Date& rNullDate)
{
    if (!xRowSet.is())
        // Dont' even waste time to go any further.
        return;

    try
    {
        Reference<sdbc::XResultSetMetaDataSupplier> xMetaSupp(xRowSet, UNO_QUERY_THROW);
        Reference<sdbc::XResultSetMetaData> xMeta = xMetaSupp->getMetaData();
        if (!xMeta.is())
            return;

        sal_Int32 nColCount = xMeta->getColumnCount();

        // Get column titles and types.
        vector<sal_Int32> aColTypes(nColCount);
        maHeader.clear();
        maHeader.reserve(nColCount);
        for (sal_Int32 nCol = 0; nCol < nColCount; ++nCol)
        {
            String aColTitle = xMeta->getColumnLabel(nCol+1);
            aColTypes[nCol]  = xMeta->getColumnType(nCol+1);
            maHeader.push_back( ScSharedString::getStringId(aColTitle) );
        }

        // Initialize field entries container.
        maFieldEntries.clear();
        maFieldEntries.reserve(nColCount);
        for (SCCOL nCol = 0; nCol < nColCount; ++nCol)
        {
            TypedStrCollectionPtr p(new TypedStrCollection);
            maFieldEntries.push_back(p);
        }

        // Now get the data rows.
        Reference<sdbc::XRow> xRow(xRowSet, UNO_QUERY_THROW);
        xRowSet->first();
        maTable.clear();
        maRowsVisible.clear();
        do
        {
            maRowsVisible.push_back(true);
            maTable.push_back( vector<Cell>() );
            maTable.back().reserve(nColCount);
            for (sal_Int32 nCol = 0; nCol < nColCount; ++nCol)
            {
                maTable.back().push_back( Cell() );
                Cell& rCell = maTable.back().back();
                String aStr;
                lcl_GetCellValue(xRow, aColTypes[nCol], nCol+1, rNullDate, rCell, aStr);

                TypedStrData* pNew;
                if (rCell.mbNumeric)
                    pNew = new TypedStrData(aStr, rCell.mfValue, SC_STRTYPE_VALUE);
                else
                    pNew = new TypedStrData(aStr);

                if (!maFieldEntries[nCol]->Insert(pNew))
                    delete pNew;
            }
        }
        while (xRowSet->next());

        xRowSet->beforeFirst();
    }
    catch (const Exception&)
    {
    }
}

bool ScDPCacheTable::isRowActive(sal_Int32 nRow) const
{
    if (nRow < 0 || static_cast<size_t>(nRow) >= maRowsVisible.size())
        // row index out of bound
        return false;

    return maRowsVisible[nRow];
}

void ScDPCacheTable::filterByPageDimension(const vector<ScDPDimension*>& rPageDims)
{
    sal_Int32 nRowSize = getRowSize();

    if (nRowSize != static_cast<sal_Int32>(maRowsVisible.size()))
    {
        fprintf(stdout, "ScDPCacheTable::filterByPageDimension: the sizes of the two tables differ.\n");fflush(stdout);
        return;
    }

    for (sal_Int32 nRow = 0; nRow < nRowSize; ++nRow)
    {
        maRowsVisible[nRow] = true;
        const vector<Cell>& rRow = maTable[nRow];
        vector<ScDPDimension*>::const_iterator itr = rPageDims.begin(), itrEnd = rPageDims.end();
        for (; itr != itrEnd; ++itr)
        {
            ScDPDimension* pDim = *itr;
            if (!pDim->HasSelectedPage())
                // 'show all' is selected.
                continue;

            ScDPCacheTable::Cell aDimCell = getSelectedDimension(pDim);

            sal_Int32 nCol = pDim->GetDimension();
            const Cell& rCell = rRow[nCol];
            if (aDimCell.mnStrId != rCell.mnStrId)
            {
                // Selected page dimension value does not match the current value.  Skip it.
                maRowsVisible[nRow] = false;
                break;
            }
        }
    }
}

const ::ScDPCacheTable::Cell* ScDPCacheTable::getCell(SCCOL nCol, SCROW nRow, bool bRepeatIfEmpty) const
{
    if ( nRow >= static_cast<SCROW>(maTable.size()) )
        return NULL;

    const vector<Cell>& rRow = maTable.at(nRow);
    if ( nCol < 0 || static_cast<size_t>(nCol) >= rRow.size() )
        return NULL;

    const Cell* pCell = &rRow.at(nCol);
    if (bRepeatIfEmpty && pCell && pCell->mnType == SC_VALTYPE_EMPTY)
        pCell = getCell(nCol, pCell->mnCategoryRef, false);

    return pCell;
}

const String* ScDPCacheTable::getFieldName(sal_Int32 nIndex) const
{
    if (nIndex >= static_cast<sal_Int32>(maHeader.size()))
        return NULL;

    return ScSharedString::getString(maHeader[nIndex]);
}

sal_Int32 ScDPCacheTable::getFieldIndex(const String& rStr) const
{
    sal_Int32 nStrId = ScSharedString::getStringId(rStr);
    if (nStrId < 0)
        // string not found.
        return nStrId;

    sal_Int32 n = maHeader.size();
    for (sal_Int32 i = 0; i < n; ++i)
    {
        if (maHeader[i] == nStrId)
            return i;
    }

    return -1;
}

const TypedStrCollection& ScDPCacheTable::getFieldEntries(sal_Int32 nIndex) const
{
    if (nIndex < 0 || static_cast<size_t>(nIndex) >= maFieldEntries.size())
    {
        // index out of bound.  Hopefully this code will never be reached.
        static const TypedStrCollection emptyCollection;
        return emptyCollection;
    }

    return *maFieldEntries[nIndex].get();
}

void ScDPCacheTable::filterTable(const vector<Criterion>& rCriteria, Sequence< Sequence<Any> >& rTabData, bool bRepeatIfEmpty)
{
    sal_Int32 nRowSize = getRowSize();
    sal_Int32 nColSize = getColSize();

    if (!nRowSize)
        // no data to filter.
        return;

    // Row first, then column.
    vector< Sequence<Any> > tableData;
    tableData.reserve(nRowSize+1);

    // Header first.
    Sequence<Any> headerRow(nColSize);
    for (sal_Int32 nCol = 0; nCol < nColSize; ++nCol)
    {
        OUString str;
        const String* pStr = ScSharedString::getString(maHeader[nCol]);
        if (pStr)
            str = *pStr;

        Any any;
        any <<= str;
        headerRow[nCol] = any;
    }
    tableData.push_back(headerRow);


    for (sal_Int32 nRow = 0; nRow < nRowSize; ++nRow)
    {
        if (!maRowsVisible[nRow])
            // This row is filtered out.
            continue;

        bool bRetainRow = true;

        vector<Criterion>::const_iterator itrEnd = rCriteria.end();
        for (vector<Criterion>::const_iterator itr = rCriteria.begin(); itr != itrEnd; ++itr)
        {
            if (itr->mnFieldIndex >= nColSize)
                // specified field is outside the source data columns.  Don't
                // use this criterion.
                continue;

            const Cell* pCell = getCell(itr->mnFieldIndex, nRow, bRepeatIfEmpty);
            if (!pCell)
            {
                // This should never happen, but just in case...
                bRetainRow = false;
                break;
            }

            if (!itr->mpFilter->match(*pCell))
            {
                bRetainRow = false;
                break;
            }
        }

        if (!bRetainRow)
            continue;

        // Insert this row into table.

        Sequence<Any> row(nColSize);
        for (sal_Int32 nCol = 0; nCol < nColSize; ++nCol)
        {
            Any any;
            const Cell* pCell = getCell(nCol, nRow, bRepeatIfEmpty);
            if (!pCell)
            {
                // This should never happen, but in case this happens, just
                // stick in an empty string.
                OUString str;
                any <<= str;
                row[nCol] = any;
                continue;
            }

            if (pCell->mbNumeric)
                any <<= pCell->mfValue;
            else
            {
                OUString str;
                const String* pStr = ScSharedString::getString(pCell->mnStrId);
                if (pStr)
                    str = *pStr;
                any <<= str;
            }
            row[nCol] = any;
        }
        tableData.push_back(row);
    }

    // convert vector to Seqeunce
    sal_Int32 nTabSize = static_cast<sal_Int32>(tableData.size());
    rTabData.realloc(nTabSize);
    for (sal_Int32 i = 0; i < nTabSize; ++i)
        rTabData[i] = tableData[i];
}

void ScDPCacheTable::clear()
{
    maTable.clear();
    maHeader.clear();
    maFieldEntries.clear();
    maRowsVisible.clear();
}

void ScDPCacheTable::swap(ScDPCacheTable& rOther)
{
    maTable.swap(rOther.maTable);
    maHeader.swap(rOther.maHeader);
    maFieldEntries.swap(rOther.maFieldEntries);
    maRowsVisible.swap(rOther.maRowsVisible);
}

bool ScDPCacheTable::empty() const
{
    return maTable.empty();
}

void ScDPCacheTable::getValueData(ScDocument* pDoc, const ScAddress& rPos, Cell& rCell)
{
    ScBaseCell* pCell = pDoc->GetCell(rPos);
    if (!pCell)
    {
        rCell.mnType = SC_VALTYPE_EMPTY;
        return;
    }

    CellType eType = pCell->GetCellType();
    if (eType == CELLTYPE_NOTE)
    {
        // note cell
        rCell.mnType = SC_VALTYPE_EMPTY;
        return;
    }

    if (eType == CELLTYPE_FORMULA && static_cast<ScFormulaCell*>(pCell)->GetErrCode())
    {
        // formula cell with error
        rCell.mnType = SC_VALTYPE_ERROR;
        return;
    }

    if ( pCell->HasValueData() )
    {
        if (eType == CELLTYPE_VALUE)
            // value cell
            rCell.mfValue = static_cast<ScValueCell*>(pCell)->GetValue();
        else if (eType == CELLTYPE_FORMULA)
            // formula cell
            rCell.mfValue = static_cast<ScFormulaCell*>(pCell)->GetValue();

        rCell.mbNumeric = true;
        rCell.mnType = SC_VALTYPE_VALUE;
    }
}

ScDPCacheTable::Cell ScDPCacheTable::getSelectedDimension(ScDPDimension* pDim) const
{
    const ScDPItemData& rData = pDim->GetSelectedData();
    Cell aCell;
    aCell.mfValue = rData.fValue;
    aCell.mbNumeric = rData.bHasValue;
    aCell.mnStrId = ScSharedString::getStringId(rData.aString);
    return aCell;
}
