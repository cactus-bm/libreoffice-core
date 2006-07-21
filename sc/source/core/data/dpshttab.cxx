/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dpshttab.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-21 10:57:52 $
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



// INCLUDE --------------------------------------------------------------

#include <tools/debug.hxx>
#include <svtools/zforlist.hxx>

#include "dpshttab.hxx"
#include "document.hxx"
#include "collect.hxx"
#include "cell.hxx"
#include "globstr.hrc"

// -----------------------------------------------------------------------

class ScSheetDPData_Impl
{
public:
    ScDocument*     pDoc;
    ScRange         aRange;
    ScQueryParam    aQuery;
    long            nColCount;
    BOOL            bIgnoreEmptyRows;
    BOOL            bRepeatIfEmpty;
    TypedStrCollection**    ppStrings;
    BOOL*           pDateDim;
    SCROW           nNextRow;       // for iterator, within range

    ScSheetDPData_Impl() {}
};

// -----------------------------------------------------------------------

ScSheetDPData::ScSheetDPData( ScDocument* pD, const ScSheetSourceDesc& rDesc )
    : pSpecial(NULL)
{
    long nCount = rDesc.aSourceRange.aEnd.Col() - rDesc.aSourceRange.aStart.Col() + 1;
    pImpl = new ScSheetDPData_Impl;
    pImpl->pDoc = pD;
    pImpl->aRange = rDesc.aSourceRange;
    pImpl->aQuery = rDesc.aQueryParam;
    pImpl->bIgnoreEmptyRows = FALSE;
    pImpl->bRepeatIfEmpty = FALSE;
    pImpl->nColCount = nCount;
    pImpl->ppStrings = new TypedStrCollection*[nCount];
    pImpl->pDateDim = NULL;
    for (long i=0; i<nCount; i++)
        pImpl->ppStrings[i] = NULL;

    pImpl->nNextRow = pImpl->aRange.aStart.Row() + 1;

    SCSIZE nEntryCount(pImpl->aQuery.GetEntryCount());
    pSpecial = new BOOL[nEntryCount];
    for (SCSIZE j = 0; j < nEntryCount; ++j )
    {
        ScQueryEntry& rEntry = pImpl->aQuery.GetEntry(j);
        if (rEntry.bDoQuery)
        {
            pSpecial[j] = false;
            if (!rEntry.bQueryByString)
            {
                if (*rEntry.pStr == EMPTY_STRING &&
                   ((rEntry.nVal == SC_EMPTYFIELDS) || (rEntry.nVal == SC_NONEMPTYFIELDS)))
                    pSpecial[j] = true;
            }
            else
            {
                sal_uInt32 nIndex = 0;
                rEntry.bQueryByString =
                            !(pD->GetFormatTable()->
                                IsNumberFormat(*rEntry.pStr, nIndex, rEntry.nVal));
            }
        }
    }
}

ScSheetDPData::~ScSheetDPData()
{
    for (long i=0; i<pImpl->nColCount; i++)
        delete pImpl->ppStrings[i];
    delete[] pImpl->ppStrings;
    delete[] pImpl->pDateDim;
    delete pImpl;
    delete[] pSpecial;
}

void ScSheetDPData::DisposeData()
{
    for (long i=0; i<pImpl->nColCount; i++)
    {
        delete pImpl->ppStrings[i];
        pImpl->ppStrings[i] = NULL;
    }
}

long ScSheetDPData::GetColumnCount()
{
    return pImpl->nColCount;
}

BOOL lcl_HasQuery( const ScQueryParam& rParam )
{
    return rParam.GetEntryCount() > 0 &&
            rParam.GetEntry(0).bDoQuery;
}

const TypedStrCollection& ScSheetDPData::GetColumnEntries(long nColumn)
{
    DBG_ASSERT(nColumn>=0 && nColumn<pImpl->nColCount, "ScSheetDPData: wrong column");

    if (!pImpl->ppStrings[nColumn])
    {
        TypedStrCollection* pColl = new TypedStrCollection;

        //! document must have function to fill collection!!!
        String aDocStr;
        SCCOL nDocCol = (SCCOL)(pImpl->aRange.aStart.Col() + nColumn);
        SCTAB nDocTab = pImpl->aRange.aStart.Tab();
        SCROW nStartRow = pImpl->aRange.aStart.Row()+1; // start of data
        SCROW nEndRow = pImpl->aRange.aEnd.Row();
        SCCOL nStartCol = pImpl->aRange.aStart.Col();
        SCCOL nEndCol = pImpl->aRange.aEnd.Col();
        for (SCROW nRow = nStartRow; nRow <= nEndRow; nRow++)
        {
            if ( pImpl->bIgnoreEmptyRows &&
                    pImpl->pDoc->IsBlockEmpty( nDocTab, nStartCol, nRow, nEndCol, nRow ) )
            {
                //  ignore this (empty) row
                //! count and skip empty rows?
            }
            else if ( pImpl->bRepeatIfEmpty && nRow > nStartRow &&
                        !pImpl->pDoc->HasData( nDocCol, nRow, nDocTab ) )
            {
                //  ignore empty member (if it's not the first row)
            }
            else if ( lcl_HasQuery(pImpl->aQuery) &&
                        !pImpl->pDoc->ValidQuery( nRow, nDocTab, pImpl->aQuery, pSpecial ) )
            {
                //  this row is filtered out
            }
            else
            {
                TypedStrData* pNew;
                pImpl->pDoc->GetString( nDocCol, nRow, nDocTab, aDocStr );
                if ( pImpl->pDoc->HasValueData( nDocCol, nRow, nDocTab ) )
                {
                    double fVal = pImpl->pDoc->GetValue(ScAddress(nDocCol, nRow, nDocTab));
                    pNew = new TypedStrData( aDocStr, fVal, SC_STRTYPE_VALUE );
                }
                else
                    pNew = new TypedStrData( aDocStr );

                if (!pColl->Insert(pNew))
                    delete pNew;
            }
        }

        pImpl->ppStrings[nColumn] = pColl;
    }
    return *pImpl->ppStrings[nColumn];
}

String ScSheetDPData::getDimensionName(long nColumn)
{
    if (getIsDataLayoutDimension(nColumn))
    {
        //! different internal and display names?
        //return "Data";
        return ScGlobal::GetRscString(STR_PIVOT_DATA);
    }
    else if ( nColumn >= pImpl->nColCount )
    {
        DBG_ERROR("getDimensionName: invalid dimension");
        return String();
    }
    else
    {
        SCCOL nDocCol = (SCCOL)(pImpl->aRange.aStart.Col() + nColumn);
        SCROW nDocRow = pImpl->aRange.aStart.Row();
        SCTAB nDocTab = pImpl->aRange.aStart.Tab();
        String aDocStr;
        pImpl->pDoc->GetString( nDocCol, nDocRow, nDocTab, aDocStr );
        return aDocStr;
    }
}

BOOL lcl_HasDateFormat( ScDocument* pDoc, const ScRange& rRange )
{
    //! iterate formats in range?

    ScAddress aPos = rRange.aStart;
    aPos.SetRow( aPos.Row() + 1 );      // below title
    ULONG nFormat = pDoc->GetNumberFormat( aPos );
    SvNumberFormatter* pFormatter = pDoc->GetFormatTable();
    return ( pFormatter->GetType(nFormat) & NUMBERFORMAT_DATE ) != 0;
}

BOOL ScSheetDPData::IsDateDimension(long nDim)
{
    if (getIsDataLayoutDimension(nDim))
    {
        return FALSE;
    }
    else if ( nDim >= pImpl->nColCount )
    {
        DBG_ERROR("IsDateDimension: invalid dimension");
        return FALSE;
    }
    else
    {
        if (!pImpl->pDateDim)
        {
            pImpl->pDateDim = new BOOL[pImpl->nColCount];
            ScRange aTestRange = pImpl->aRange;
            for (long i=0; i<pImpl->nColCount; i++)
            {
                SCCOL nCol = (SCCOL)( pImpl->aRange.aStart.Col() + i );
                aTestRange.aStart.SetCol(nCol);
                aTestRange.aEnd.SetCol(nCol);
                pImpl->pDateDim[i] = lcl_HasDateFormat( pImpl->pDoc, aTestRange );
            }
        }
        return pImpl->pDateDim[nDim];
    }
}

UINT32 ScSheetDPData::GetNumberFormat(long nDim)
{
    if (getIsDataLayoutDimension(nDim))
    {
        return 0;
    }
    else if ( nDim >= pImpl->nColCount )
    {
        DBG_ERROR("GetNumberFormat: invalid dimension");
        return 0;
    }
    else
    {
        //  is queried only once per dimension from ScDPOutput -> no need to cache

        ScAddress aPos = pImpl->aRange.aStart;
        aPos.SetCol( aPos.Col() + nDim );
        aPos.SetRow( aPos.Row() + 1 );      // below title
        return pImpl->pDoc->GetNumberFormat( aPos );
    }
}

BOOL ScSheetDPData::getIsDataLayoutDimension(long nColumn)
{
    return ( nColumn == pImpl->nColCount );
}

void ScSheetDPData::SetEmptyFlags( BOOL bIgnoreEmptyRows, BOOL bRepeatIfEmpty )
{
    pImpl->bIgnoreEmptyRows = bIgnoreEmptyRows;
    pImpl->bRepeatIfEmpty   = bRepeatIfEmpty;
}

void ScSheetDPData::ResetIterator()
{
    pImpl->nNextRow = pImpl->aRange.aStart.Row() + 1;
}

void lcl_GetStringOrValue( ScDPItemData& rData, ScDocument* pDoc,
                            SCCOL nCol, SCROW nRow, SCTAB nTab,
                            BOOL bRepeatIfEmpty, SCROW nFirstDataRow )
{
    if ( bRepeatIfEmpty )
    {
        //  use first non-empty entry
        //! just keep old ItemData ????
        //! otherwise count empty cells and subtract!

        while ( !pDoc->HasData( nCol, nRow, nTab ) && nRow > nFirstDataRow )
            --nRow;
    }

    BOOL bVal = pDoc->HasValueData( nCol, nRow, nTab );
    rData.bHasValue = bVal;
    if (bVal)
        rData.fValue = pDoc->GetValue( ScAddress( nCol, nRow, nTab ) );
    else
        pDoc->GetString( nCol, nRow, nTab, rData.aString );
}

BOOL ScSheetDPData::GetNextRow( const ScDPTableIteratorParam& rParam )
{
    if ( pImpl->nNextRow > pImpl->aRange.aEnd.Row() )
        return FALSE;

    SCSIZE i;
    long nStartCol = pImpl->aRange.aStart.Col();
    SCTAB nDocTab = pImpl->aRange.aStart.Tab();
    SCROW nFirstDataRow = pImpl->aRange.aStart.Row() + 1;

    BOOL bFilteredOut;
    do
    {
        if ( pImpl->bIgnoreEmptyRows )
        {
            SCCOL nEndCol = pImpl->aRange.aEnd.Col();
            while ( pImpl->pDoc->IsBlockEmpty( nDocTab, (SCCOL)nStartCol, pImpl->nNextRow,
                                                nEndCol, pImpl->nNextRow ) )
            {
                ++pImpl->nNextRow;
                if ( pImpl->nNextRow > pImpl->aRange.aEnd.Row() )
                    return FALSE;
            }
        }

        bFilteredOut = ( lcl_HasQuery(pImpl->aQuery) &&
                !pImpl->pDoc->ValidQuery( pImpl->nNextRow, nDocTab, pImpl->aQuery, pSpecial ) );
        if ( bFilteredOut )
        {
            ++pImpl->nNextRow;
            if ( pImpl->nNextRow > pImpl->aRange.aEnd.Row() )
                return FALSE;
        }
    }
    while (bFilteredOut);

    //! use more efficient iterators

    for (i=0; i<rParam.nColCount; i++)
    {
        long nDim = rParam.pCols[i];
        if ( getIsDataLayoutDimension(nDim) )
            rParam.pColData[i].SetString( String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("x")) );
        else
            lcl_GetStringOrValue( rParam.pColData[i], pImpl->pDoc,
                                    (SCCOL)(nStartCol+nDim), pImpl->nNextRow, nDocTab,
                                    pImpl->bRepeatIfEmpty, nFirstDataRow );
    }

    for (i=0; i<rParam.nRowCount; i++)
    {
        long nDim = rParam.pRows[i];
        if ( getIsDataLayoutDimension(nDim) )
            rParam.pRowData[i].SetString( String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("x")) );
        else
            lcl_GetStringOrValue( rParam.pRowData[i], pImpl->pDoc,
                                    (SCCOL)(nStartCol+nDim), pImpl->nNextRow, nDocTab,
                                    pImpl->bRepeatIfEmpty, nFirstDataRow );
    }

    for (i=0; i<rParam.nPageCount; i++)
    {
        long nDim = rParam.pPages[i];
        if ( getIsDataLayoutDimension(nDim) )
            rParam.pPageData[i].SetString( String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("x")) );
        else
            lcl_GetStringOrValue( rParam.pPageData[i], pImpl->pDoc,
                                    (USHORT)(nStartCol+nDim), pImpl->nNextRow, nDocTab,
                                    pImpl->bRepeatIfEmpty, nFirstDataRow );
    }

    for (i=0; i<rParam.nDatCount; i++)
    {
        long nDim = rParam.pDats[i];
        ScAddress aPos( (SCCOL)(nStartCol+nDim), pImpl->nNextRow, nDocTab );
        ScBaseCell* pCell = pImpl->pDoc->GetCell( aPos );
        if ( !pCell || pCell->GetCellType() == CELLTYPE_NOTE )
            rParam.pValues[i].Set( 0.0, SC_VALTYPE_EMPTY );
        else if ( pCell->GetCellType() == CELLTYPE_FORMULA && ((ScFormulaCell*)pCell)->GetErrCode() )
            rParam.pValues[i].Set( 0.0, SC_VALTYPE_ERROR );
        else if ( pCell->HasValueData() )
        {
            //! GetValue method at BaseCell?
            DBG_ASSERT( pCell->GetCellType() == CELLTYPE_VALUE ||
                        pCell->GetCellType() == CELLTYPE_FORMULA, "wrong cell type" );
            double fVal;
            if ( pCell->GetCellType() == CELLTYPE_VALUE )
                fVal = ((ScValueCell*)pCell)->GetValue();
            else
                fVal = ((ScFormulaCell*)pCell)->GetValue();
            rParam.pValues[i].Set( fVal, SC_VALTYPE_VALUE );
        }
        else
            rParam.pValues[i].Set( 0.0, SC_VALTYPE_STRING );
    }

    pImpl->nNextRow++;
    return TRUE;
}

// -----------------------------------------------------------------------







