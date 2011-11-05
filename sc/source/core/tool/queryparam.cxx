/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
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

// INCLUDE ---------------------------------------------------------------

#include "queryparam.hxx"
#include <unotools/textsearch.hxx>

using ::std::vector;

namespace {

const SCSIZE MAXQUERY = 8;

}

ScQueryEntry::ScQueryEntry() :
    bDoQuery(false),
    bQueryByString(false),
    bQueryByDate(false),
    nField(0),
    eOp(SC_EQUAL),
    eConnect(SC_AND),
    pStr(new String),
    nVal(0.0),
    pSearchParam(NULL),
    pSearchText(NULL)
{
}

ScQueryEntry::ScQueryEntry(const ScQueryEntry& r) :
    bDoQuery(r.bDoQuery),
    bQueryByString(r.bQueryByString),
    bQueryByDate(r.bQueryByDate),
    nField(r.nField),
    eOp(r.eOp),
    eConnect(r.eConnect),
    pStr(new String(*r.pStr)),
    nVal(r.nVal),
    pSearchParam(NULL),
    pSearchText(NULL)
{
}

ScQueryEntry::~ScQueryEntry()
{
    delete pStr;
    delete pSearchParam;
    delete pSearchText;
}

ScQueryEntry& ScQueryEntry::operator=( const ScQueryEntry& r )
{
    bDoQuery        = r.bDoQuery;
    bQueryByString  = r.bQueryByString;
    bQueryByDate    = r.bQueryByDate;
    eOp             = r.eOp;
    eConnect        = r.eConnect;
    nField          = r.nField;
    nVal            = r.nVal;
    *pStr           = *r.pStr;

    delete pSearchParam;
    delete pSearchText;
    pSearchParam    = NULL;
    pSearchText     = NULL;

    return *this;
}

bool ScQueryEntry::MatchByString(const rtl::OUString& rStr) const
{
    return rStr.equals(*pStr);
}

void ScQueryEntry::SetQueryString(const rtl::OUString& rStr)
{
    *pStr = String(rStr);
}

rtl::OUString ScQueryEntry::GetQueryString() const
{
    return *pStr;
}

void ScQueryEntry::Clear()
{
    bDoQuery        = false;
    bQueryByString  = false;
    bQueryByDate    = false;
    eOp             = SC_EQUAL;
    eConnect        = SC_AND;
    nField          = 0;
    nVal            = 0.0;
    pStr->Erase();

    delete pSearchParam;
    delete pSearchText;
    pSearchParam    = NULL;
    pSearchText     = NULL;
}

bool ScQueryEntry::operator==( const ScQueryEntry& r ) const
{
    return bDoQuery         == r.bDoQuery
        && bQueryByString   == r.bQueryByString
        && bQueryByDate     == r.bQueryByDate
        && eOp              == r.eOp
        && eConnect         == r.eConnect
        && nField           == r.nField
        && nVal             == r.nVal
        && *pStr            == *r.pStr;
    //! pSearchParam und pSearchText nicht vergleichen
}

utl::TextSearch* ScQueryEntry::GetSearchTextPtr( bool bCaseSens ) const
{
    if ( !pSearchParam )
    {
        pSearchParam = new utl::SearchParam( *pStr, utl::SearchParam::SRCH_REGEXP,
            bCaseSens, false, false );
        pSearchText = new utl::TextSearch( *pSearchParam, *ScGlobal::pCharClass );
    }
    return pSearchText;
}

// ============================================================================

ScQueryParamBase::ScQueryParamBase() :
    maEntries(MAXQUERY)
{
}

ScQueryParamBase::ScQueryParamBase(const ScQueryParamBase& r) :
    bHasHeader(r.bHasHeader), bByRow(r.bByRow), bInplace(r.bInplace), bCaseSens(r.bCaseSens),
    bRegExp(r.bRegExp), bDuplicate(r.bDuplicate), bMixedComparison(r.bMixedComparison),
    maEntries(r.maEntries)
{
}

ScQueryParamBase::~ScQueryParamBase()
{
}

bool ScQueryParamBase::IsValidFieldIndex() const
{
    return true;
}

SCSIZE ScQueryParamBase::GetEntryCount() const
{
    return maEntries.size();
}

const ScQueryEntry& ScQueryParamBase::GetEntry(SCSIZE n) const
{
    return maEntries[n];
}

ScQueryEntry& ScQueryParamBase::GetEntry(SCSIZE n)
{
    return maEntries[n];
}

void ScQueryParamBase::Resize(SCSIZE nNew)
{
    if ( nNew < MAXQUERY )
        nNew = MAXQUERY;                // never less than MAXQUERY

    vector<ScQueryEntry> aNewEntries(nNew);
    SCSIZE nCopy = ::std::min(maEntries.size(), nNew);
    for (SCSIZE i=0; i<nCopy; i++)
        aNewEntries[i] = maEntries[i];

    maEntries.swap(aNewEntries);
}

void ScQueryParamBase::DeleteQuery( SCSIZE nPos )
{
    if (nPos >= maEntries.size())
        return;

    size_t n = maEntries.size();
    vector<ScQueryEntry> aNewEntries;
    aNewEntries.reserve(n);
    for (size_t i = 0; i < n; ++i)
        if (i != nPos)
            aNewEntries.push_back(maEntries[i]);

    // Don't forget to append an empty entry to make up for the removed one.
    // The size of the entries is not supposed to change.
    aNewEntries.push_back(ScQueryEntry());

    maEntries.swap(aNewEntries);
}

void ScQueryParamBase::FillInExcelSyntax(String& aCellStr, SCSIZE nIndex)
{
    if (aCellStr.Len() > 0)
    {
        if ( nIndex >= maEntries.size() )
            Resize( nIndex+1 );

        ScQueryEntry& rEntry = GetEntry(nIndex);

        rEntry.bDoQuery = sal_True;
        // Operatoren herausfiltern
        if (aCellStr.GetChar(0) == '<')
        {
            if (aCellStr.GetChar(1) == '>')
            {
                *rEntry.pStr = aCellStr.Copy(2);
                rEntry.eOp   = SC_NOT_EQUAL;
            }
            else if (aCellStr.GetChar(1) == '=')
            {
                *rEntry.pStr = aCellStr.Copy(2);
                rEntry.eOp   = SC_LESS_EQUAL;
            }
            else
            {
                *rEntry.pStr = aCellStr.Copy(1);
                rEntry.eOp   = SC_LESS;
            }
        }
        else if (aCellStr.GetChar(0) == '>')
        {
            if (aCellStr.GetChar(1) == '=')
            {
                *rEntry.pStr = aCellStr.Copy(2);
                rEntry.eOp   = SC_GREATER_EQUAL;
            }
            else
            {
                *rEntry.pStr = aCellStr.Copy(1);
                rEntry.eOp   = SC_GREATER;
            }
        }
        else
        {
            if (aCellStr.GetChar(0) == '=')
                *rEntry.pStr = aCellStr.Copy(1);
            else
                *rEntry.pStr = aCellStr;
            rEntry.eOp = SC_EQUAL;
        }
    }
}

// ============================================================================

ScQueryParamTable::ScQueryParamTable()
{
}

ScQueryParamTable::ScQueryParamTable(const ScQueryParamTable& r) :
    nCol1(r.nCol1),nRow1(r.nRow1),nCol2(r.nCol2),nRow2(r.nRow2),nTab(r.nTab)
{
}

ScQueryParamTable::~ScQueryParamTable()
{
}

// ============================================================================

ScQueryParam::ScQueryParam() :
    ScQueryParamBase(),
    ScQueryParamTable(),
    bDestPers(true),
    nDestTab(0),
    nDestCol(0),
    nDestRow(0)
{
    Clear();
}

//------------------------------------------------------------------------

ScQueryParam::ScQueryParam( const ScQueryParam& r ) :
    ScQueryParamBase(r),
    ScQueryParamTable(r),
    bDestPers(r.bDestPers), nDestTab(r.nDestTab), nDestCol(r.nDestCol), nDestRow(r.nDestRow)
{
}

ScQueryParam::ScQueryParam( const ScDBQueryParamInternal& r ) :
    ScQueryParamBase(r),
    ScQueryParamTable(r),
    bDestPers(true),
    nDestTab(0),
    nDestCol(0),
    nDestRow(0)
{
}


//------------------------------------------------------------------------

ScQueryParam::~ScQueryParam()
{
}

//------------------------------------------------------------------------

void ScQueryParam::Clear()
{
    nCol1=nCol2 = 0;
    nRow1=nRow2 = 0;
    nTab = SCTAB_MAX;
    bHasHeader = bCaseSens = bRegExp = bMixedComparison = false;
    bInplace = bByRow = bDuplicate = sal_True;

    std::vector<ScQueryEntry> aNewEntries(MAXQUERY);
    maEntries.swap(aNewEntries);

    ClearDestParams();
}

void ScQueryParam::ClearDestParams()
{
    bDestPers = true;
    nDestTab = 0;
    nDestCol = 0;
    nDestRow = 0;
}

//------------------------------------------------------------------------

ScQueryParam& ScQueryParam::operator=( const ScQueryParam& r )
{
    nCol1       = r.nCol1;
    nRow1       = r.nRow1;
    nCol2       = r.nCol2;
    nRow2       = r.nRow2;
    nTab        = r.nTab;
    nDestTab    = r.nDestTab;
    nDestCol    = r.nDestCol;
    nDestRow    = r.nDestRow;
    bHasHeader  = r.bHasHeader;
    bInplace    = r.bInplace;
    bCaseSens   = r.bCaseSens;
    bRegExp     = r.bRegExp;
    bMixedComparison = r.bMixedComparison;
    bDuplicate  = r.bDuplicate;
    bByRow      = r.bByRow;
    bDestPers   = r.bDestPers;

    maEntries = r.maEntries;

    return *this;
}

//------------------------------------------------------------------------

bool ScQueryParam::operator==( const ScQueryParam& rOther ) const
{
    bool bEqual = false;

    // Anzahl der Queries gleich?
    SCSIZE nUsed      = 0;
    SCSIZE nOtherUsed = 0;
    SCSIZE nEntryCount = GetEntryCount();
    SCSIZE nOtherEntryCount = rOther.GetEntryCount();

    while ( nUsed<nEntryCount && maEntries[nUsed].bDoQuery ) ++nUsed;
    while ( nOtherUsed<nOtherEntryCount && rOther.maEntries[nOtherUsed].bDoQuery )
        ++nOtherUsed;

    if (   (nUsed       == nOtherUsed)
        && (nCol1       == rOther.nCol1)
        && (nRow1       == rOther.nRow1)
        && (nCol2       == rOther.nCol2)
        && (nRow2       == rOther.nRow2)
        && (nTab        == rOther.nTab)
        && (bHasHeader  == rOther.bHasHeader)
        && (bByRow      == rOther.bByRow)
        && (bInplace    == rOther.bInplace)
        && (bCaseSens   == rOther.bCaseSens)
        && (bRegExp     == rOther.bRegExp)
        && (bMixedComparison == rOther.bMixedComparison)
        && (bDuplicate  == rOther.bDuplicate)
        && (bDestPers   == rOther.bDestPers)
        && (nDestTab    == rOther.nDestTab)
        && (nDestCol    == rOther.nDestCol)
        && (nDestRow    == rOther.nDestRow) )
    {
        bEqual = true;
        for ( SCSIZE i=0; i<nUsed && bEqual; i++ )
            bEqual = maEntries[i] == rOther.maEntries[i];
    }
    return bEqual;
}

//------------------------------------------------------------------------

void ScQueryParam::MoveToDest()
{
    if (!bInplace)
    {
        SCsCOL nDifX = ((SCsCOL) nDestCol) - ((SCsCOL) nCol1);
        SCsROW nDifY = ((SCsROW) nDestRow) - ((SCsROW) nRow1);
        SCsTAB nDifZ = ((SCsTAB) nDestTab) - ((SCsTAB) nTab);

        nCol1 = sal::static_int_cast<SCCOL>( nCol1 + nDifX );
        nRow1 = sal::static_int_cast<SCROW>( nRow1 + nDifY );
        nCol2 = sal::static_int_cast<SCCOL>( nCol2 + nDifX );
        nRow2 = sal::static_int_cast<SCROW>( nRow2 + nDifY );
        nTab  = sal::static_int_cast<SCTAB>( nTab  + nDifZ );
        size_t n = maEntries.size();
        for (size_t i=0; i<n; i++)
            maEntries[i].nField += nDifX;

        bInplace = sal_True;
    }
    else
    {
        OSL_FAIL("MoveToDest, bInplace == TRUE");
    }
}

// ============================================================================

ScDBQueryParamBase::ScDBQueryParamBase(DataType eType) :
    ScQueryParamBase(),
    mnField(-1),
    mbSkipString(true),
    meType(eType)
{
}

ScDBQueryParamBase::~ScDBQueryParamBase()
{
}

ScDBQueryParamBase::DataType ScDBQueryParamBase::GetType() const
{
    return meType;
}

// ============================================================================

ScDBQueryParamInternal::ScDBQueryParamInternal() :
    ScDBQueryParamBase(ScDBQueryParamBase::INTERNAL),
    ScQueryParamTable()
{
}

ScDBQueryParamInternal::~ScDBQueryParamInternal()
{
}

bool ScDBQueryParamInternal::IsValidFieldIndex() const
{
    return nCol1 <= mnField && mnField <= nCol2;
}

// ============================================================================

ScDBQueryParamMatrix::ScDBQueryParamMatrix() :
    ScDBQueryParamBase(ScDBQueryParamBase::MATRIX)
{
}

bool ScDBQueryParamMatrix::IsValidFieldIndex() const
{
    SCSIZE nC, nR;
    mpMatrix->GetDimensions(nC, nR);
    return 0 <= mnField && mnField <= static_cast<SCCOL>(nC);
}

ScDBQueryParamMatrix::~ScDBQueryParamMatrix()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
