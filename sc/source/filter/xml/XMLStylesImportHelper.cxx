/*************************************************************************
 *
 *  $RCSfile: XMLStylesImportHelper.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: sab $ $Date: 2001-05-28 12:06:17 $
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

// INCLUDE ---------------------------------------------------------------

#ifndef _SC_XMLSTYLESIMPORTHELPER_HXX
#include "XMLStylesImportHelper.hxx"
#endif
#ifndef SC_XMLIMPRT_HXX
#include "xmlimprt.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _COM_SUN_STAR_UTIL_NUMBERFORMAT_HPP_
#include <com/sun/star/util/NumberFormat.hpp>
#endif

using namespace com::sun::star;

ScMyStyleRanges::ScMyStyleRanges()
    :
    pTextList(NULL),
    pNumberList(NULL),
    pTimeList(NULL),
    pDateTimeList(NULL),
    pPercentList(NULL),
    pLogicalList(NULL),
    pCurrencyList(NULL)
{
}

ScMyStyleRanges::~ScMyStyleRanges()
{
    if (pTextList)
        delete pTextList;
    if (pNumberList)
        delete pNumberList;
    if (pTimeList)
        delete pTimeList;
    if (pDateTimeList)
        delete pDateTimeList;
    if (pPercentList)
        delete pPercentList;
    if (pLogicalList)
        delete pLogicalList;
    if (pCurrencyList)
        delete pCurrencyList;
}

void ScMyStyleRanges::AddRange(const ScRange& rRange, const sal_Int16 nType)
{
    switch (nType)
    {
        case util::NumberFormat::NUMBER:
        {
            if (!pNumberList)
                pNumberList = new ScRangeList();
            pNumberList->Join(rRange);
        }
        break;
        case util::NumberFormat::TEXT:
        {
            if (!pTextList)
                pTextList = new ScRangeList();
            pTextList->Join(rRange);
        }
        break;
        case util::NumberFormat::TIME:
        {
            if (!pTimeList)
                pTimeList = new ScRangeList();
            pTimeList->Join(rRange);
        }
        break;
        case util::NumberFormat::DATETIME:
        {
            if (!pDateTimeList)
                pDateTimeList = new ScRangeList();
            pDateTimeList->Join(rRange);
        }
        break;
        case util::NumberFormat::PERCENT:
        {
            if (!pPercentList)
                pPercentList = new ScRangeList();
            pPercentList->Join(rRange);
        }
        break;
        case util::NumberFormat::LOGICAL:
        {
            if (!pLogicalList)
                pLogicalList = new ScRangeList();
            pLogicalList->Join(rRange);
        }
        break;
        default:
        {
            DBG_ERROR("wrong type");
        }
        break;
    }
}

void ScMyStyleRanges::AddCurrencyRange(const ScRange& rRange, const rtl::OUString& rCurrency)
{
    if (!pCurrencyList)
        pCurrencyList = new ScMyCurrencyStylesSet();
    ScMyCurrencyStyle aStyle;
    aStyle.sCurrency = rCurrency;
    ScMyCurrencyStylesSet::iterator aItr = pCurrencyList->find(aStyle);
    if (aItr == pCurrencyList->end())
    {
        std::pair<ScMyCurrencyStylesSet::iterator, bool> aPair = pCurrencyList->insert(aStyle);
        if (aPair.second)
        {
            aItr = aPair.first;
            aItr->xRanges->Join(rRange);
        }
    }
    else
        aItr->xRanges->Join(rRange);
}

void ScMyStyleRanges::InsertColRow(const ScRange& rRange, const sal_Int16 nDx, const sal_Int16 nDy,
        const sal_Int16 nDz, ScDocument* pDoc)
{
    UpdateRefMode aRefMode = URM_INSDEL;
    if (pNumberList)
        pNumberList->UpdateReference(aRefMode, pDoc, rRange, nDx, nDy, nDz);
    if (pTextList)
        pTextList->UpdateReference(aRefMode, pDoc, rRange, nDx, nDy, nDz);
    if (pTimeList)
        pTimeList->UpdateReference(aRefMode, pDoc, rRange, nDx, nDy, nDz);
    if (pDateTimeList)
        pDateTimeList->UpdateReference(aRefMode, pDoc, rRange, nDx, nDy, nDz);
    if (pPercentList)
        pPercentList->UpdateReference(aRefMode, pDoc, rRange, nDx, nDy, nDz);
    if (pLogicalList)
        pLogicalList->UpdateReference(aRefMode, pDoc, rRange, nDx, nDy, nDz);
    if (pCurrencyList)
    {
        ScMyCurrencyStylesSet::iterator aItr = pCurrencyList->begin();
        while (aItr != pCurrencyList->end())
        {
            aItr->xRanges->UpdateReference(aRefMode, pDoc, rRange, nDx, nDy, nDz);
        }
    }
}

void ScMyStyleRanges::InsertRow(const sal_Int32 nRow, const sal_Int32 nTab, ScDocument* pDoc)
{
    InsertColRow(ScRange(0, static_cast<sal_uInt16>(nRow), static_cast<sal_uInt16>(nTab),
        MAXCOL, MAXROW, static_cast<sal_uInt16>(nTab)), 0, 1, 0, pDoc);
}

void ScMyStyleRanges::InsertCol(const sal_Int32 nCol, const sal_Int32 nTab, ScDocument* pDoc)
{
    InsertColRow(ScRange(static_cast<sal_uInt16>(nCol), 0, static_cast<sal_uInt16>(nTab),
        MAXCOL, MAXROW, static_cast<sal_uInt16>(nTab)), 1, 0, 0, pDoc);
}

void ScMyStyleRanges::SetStylesToRanges(ScRangeList* pList,
    const rtl::OUString& rStyleName, const sal_Int16 nCellType,
    const rtl::OUString& rCurrency, ScXMLImport& rImport)
{
    sal_Int32 nCount(pList->Count());
    for (sal_Int32 i = 0; i < nCount; i++)
        rImport.SetStyleToRange(*pList->GetObject(i), rStyleName, nCellType, rCurrency);
}

void ScMyStyleRanges::SetStylesToRanges(ScRangeListRef xList,
    const rtl::OUString& rStyleName, const sal_Int16 nCellType,
    const rtl::OUString& rCurrency, ScXMLImport& rImport)
{
    sal_Int32 nCount(xList->Count());
    for (sal_Int32 i = 0; i < nCount; i++)
        rImport.SetStyleToRange(*xList->GetObject(i), rStyleName, nCellType, rCurrency);
}

void ScMyStyleRanges::SetStylesToRanges(const rtl::OUString& rStyleName, ScXMLImport& rImport)
{
    rtl::OUString sEmpty;
    if (pNumberList)
        SetStylesToRanges(pNumberList, rStyleName, util::NumberFormat::NUMBER, sEmpty, rImport);
    if (pTextList)
        SetStylesToRanges(pTextList, rStyleName, util::NumberFormat::TEXT, sEmpty, rImport);
    if (pTimeList)
        SetStylesToRanges(pTimeList, rStyleName, util::NumberFormat::TIME, sEmpty, rImport);
    if (pDateTimeList)
        SetStylesToRanges(pDateTimeList, rStyleName, util::NumberFormat::DATETIME, sEmpty, rImport);
    if (pPercentList)
        SetStylesToRanges(pPercentList, rStyleName, util::NumberFormat::PERCENT, sEmpty, rImport);
    if (pLogicalList)
        SetStylesToRanges(pLogicalList, rStyleName, util::NumberFormat::LOGICAL, sEmpty, rImport);
    if (pCurrencyList)
    {
        ScMyCurrencyStylesSet::iterator aItr = pCurrencyList->begin();
        while (aItr != pCurrencyList->end())
        {
            SetStylesToRanges(aItr->xRanges, rStyleName, util::NumberFormat::CURRENCY, aItr->sCurrency, rImport);
            aItr++;
        }
    }
}

//----------------------------------------------------------------------------

ScMyStylesImportHelper::ScMyStylesImportHelper(const rtl::OUString& rPrefix)
    :
    aCellStyles(),
    aColDefaultStyles(),
    sPrefix(rPrefix)
{
    aRowDefaultStyle = aCellStyles.end();
}

ScMyStylesImportHelper::~ScMyStylesImportHelper()
{
}

void ScMyStylesImportHelper::ResetAttributes()
{
    sStyleName = sEmpty;
    sCurrency = sEmpty;
    nCellType = 0;
}

ScMyStylesSet::iterator ScMyStylesImportHelper::GetIterator(const rtl::OUString& sStyleName)
{
    ScMyStyle aStyle;
    aStyle.sStyleName = sStyleName;
    ScMyStylesSet::iterator aItr = aCellStyles.find(aStyle);
    if (aItr == aCellStyles.end())
    {
        std::pair<ScMyStylesSet::iterator, bool> aPair = aCellStyles.insert(aStyle);
        if (aPair.second)
            aItr = aPair.first;
        else
        {
            DBG_ERROR("not possible to insert style");
            return aCellStyles.end();
        }
    }
    return aItr;
}

void ScMyStylesImportHelper::AddDefaultRange(const ScRange& rRange)
{
    DBG_ASSERT(aRowDefaultStyle != aCellStyles.end(), "no row default style")
    if (!aRowDefaultStyle->sStyleName.getLength())
    {
        sal_Int32 nStartCol(rRange.aStart.Col());
        sal_Int32 nEndCol(rRange.aEnd.Col());
        ScMyStylesSet::iterator aPrevItr = aColDefaultStyles[nStartCol];
        DBG_ASSERT(aColDefaultStyles.size() > static_cast<sal_uInt32>(nEndCol), "to much columns");
        for (sal_Int32 i = nStartCol + 1; i <= nEndCol; i++)
        {
            if (aPrevItr != aColDefaultStyles[i])
            {
                DBG_ASSERT(aPrevItr != aCellStyles.end(), "no column default style")
                ScRange aRange(rRange);
                aRange.aStart.SetCol(static_cast<sal_uInt16>(nStartCol));
                aRange.aEnd.SetCol(i - 1);
                sStyleName = aPrevItr->sStyleName;
                AddSingleRange(aRange);
                nStartCol = i;
                aPrevItr = aColDefaultStyles[i];
            }
        }
        DBG_ASSERT(aPrevItr != aCellStyles.end(), "no column default style")
        ScRange aRange(rRange);
        aRange.aStart.SetCol(static_cast<sal_uInt16>(nStartCol));
        sStyleName = aPrevItr->sStyleName;
        AddSingleRange(aRange);
    }
    else
    {
        sStyleName = aRowDefaultStyle->sStyleName;
        AddSingleRange(rRange);
    }
}

void ScMyStylesImportHelper::AddSingleRange(const ScRange& rRange)
{
    ScMyStylesSet::iterator aItr = GetIterator(sStyleName);
    if (aItr != aCellStyles.end())
    {
        if (nCellType != util::NumberFormat::CURRENCY)
            aItr->xRanges->AddRange(rRange, nCellType);
        else
            aItr->xRanges->AddCurrencyRange(rRange, sCurrency);
    }
}

void ScMyStylesImportHelper::AddColumnStyle(const rtl::OUString& sStyleName, const sal_Int32 nColumn, const sal_Int32 nRepeat)
{
    DBG_ASSERT(static_cast<sal_uInt32>(nColumn) == aColDefaultStyles.size(), "some columns are absent");
    ScMyStylesSet::iterator aItr = GetIterator(sStyleName);
    DBG_ASSERT(aItr != aCellStyles.end(), "no column default style")
    for (sal_Int32 i = 0; i < nRepeat; i++)
        aColDefaultStyles.push_back(aItr);
}

void ScMyStylesImportHelper::SetRowStyle(const rtl::OUString& sStyleName)
{
    aRowDefaultStyle = GetIterator(sStyleName);
}

void ScMyStylesImportHelper::SetAttributes(const rtl::OUString& sStyleName,
    const rtl::OUString& sCurrency, const sal_Int16 nCellType)
{
    this->sStyleName = sStyleName;
    this->sCurrency = sCurrency;
    this->nCellType = nCellType;
}

void ScMyStylesImportHelper::AddRange(const ScRange& rRange)
{
    if (sStyleName.getLength())
        AddSingleRange(rRange);
    else
        AddDefaultRange(rRange);
    ResetAttributes();
}

void ScMyStylesImportHelper::AddRange(const com::sun::star::table::CellRangeAddress& rRange)
{
    ScRange aScRange( static_cast<USHORT>(rRange.StartColumn), static_cast<USHORT>(rRange.StartRow), rRange.Sheet,
        static_cast<USHORT>(rRange.EndColumn), static_cast<USHORT>(rRange.EndRow), rRange.Sheet );
    AddRange(aScRange);
}

void ScMyStylesImportHelper::AddCell(const ScAddress& rAddress)
{
    ScRange aScRange( rAddress, rAddress );
    AddRange(aScRange);
}

void ScMyStylesImportHelper::AddCell(const com::sun::star::table::CellAddress& rAddress)
{
    ScAddress aScAddress( static_cast<USHORT>(rAddress.Column), static_cast<USHORT>(rAddress.Row), rAddress.Sheet );
    ScRange aScRange( aScAddress, aScAddress );
    AddRange(aScRange);
}

void ScMyStylesImportHelper::InsertRow(const sal_Int32 nRow, const sal_Int32 nTab, ScDocument* pDoc)
{
    ScMyStylesSet::iterator aItr = aCellStyles.begin();
    while (aItr != aCellStyles.end())
    {
        aItr->xRanges->InsertRow(nRow, nTab, pDoc);
        aItr++;
    }
}

void ScMyStylesImportHelper::InsertCol(const sal_Int32 nCol, const sal_Int32 nTab, ScDocument* pDoc)
{
    ScMyStylesSet::iterator aItr = aCellStyles.begin();
    while (aItr != aCellStyles.end())
    {
        aItr->xRanges->InsertCol(nCol, nTab, pDoc);
        aItr++;
    }
}

void ScMyStylesImportHelper::SetStylesToRanges(ScXMLImport& rImport)
{
    ScMyStylesSet::iterator aItr = aCellStyles.begin();
    while (aItr != aCellStyles.end())
    {
        aItr->xRanges->SetStylesToRanges(aItr->sStyleName, rImport);
        aItr++;
    }
    aCellStyles.clear();
    aColDefaultStyles.clear();
}

