/*************************************************************************
 *
 *  $RCSfile: xmlexprt.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: sab $ $Date: 2000-09-22 11:24:05 $
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

#include <xmloff/nmspmap.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/xmlkywd.hxx>
#include <xmloff/xmlmetae.hxx>
#include <xmloff/xmlexppr.hxx>
#include <xmloff/styleexp.hxx>
#include <xmloff/families.hxx>
#include <xmloff/xmluconv.hxx>
#include <xmloff/numehelp.hxx>
#ifndef _XMLOFF_XMLNUMFE_HXX
#include <xmloff/xmlnumfe.hxx>
#endif

#include <tools/lang.hxx>
#include <tools/solmath.hxx>
#include <tools/date.hxx>
#include <tools/intn.hxx>
#include <svtools/zforlist.hxx>
#include <sfx2/objsh.hxx>
#include <rtl/ustrbuf.hxx>
#include <vector>

#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XUsedAreaCursor.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/table/CellContentType.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/util/NumberFormat.hpp>
#include <com/sun/star/sheet/XNamedRanges.hpp>
#include <com/sun/star/sheet/XNamedRange.hpp>
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#include <com/sun/star/sheet/NamedRangeFlag.hpp>
#include <com/sun/star/sheet/XDatabaseRanges.hpp>
#include <com/sun/star/sheet/XDatabaseRange.hpp>
#include <com/sun/star/sheet/XSheetFilterDescriptor.hpp>
#include <com/sun/star/sheet/DataImportMode.hpp>
#include <com/sun/star/util/SortField.hpp>
#include <com/sun/star/sheet/XSubTotalField.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/sheet/CellFlags.hpp>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>

#include "cellsuno.hxx"
#include "xmlexprt.hxx"
#include "document.hxx"
#include "cell.hxx"
#include "scitems.hxx"
#include "attrib.hxx"
#include "docuno.hxx"
#include "rangenam.hxx"
#include "globstr.hrc"
#include "dbcolect.hxx"
#include "global.hxx"
#include "dpobject.hxx"
#include "dpsave.hxx"
#include "dpshttab.hxx"
#include "dpsdbtab.hxx"
#include "dociter.hxx"
#include "patattr.hxx"

const sal_Int8 SC_MAXDIGITSCOUNT_TIME = 11;

#define SC_NUMBERFORMAT "NumberFormat"
#define SC_TYPE "Type"
#define SC_CELLSTYLE "CellStyle"
#define SC_NAMEDRANGES "NamedRanges"
#define SC_DATABASERANGES "DatabaseRanges"
#define SC_KEEPFORMATS "KeepFormats"
#define SC_MOVECELLS "MoveCells"
#define SC_STRIPDATA "StripData"
#define SC_CONTAINSHEADER "ContainsHeader"
#define SC_ORIENTATION "Orientation"
#define SC_DATABASENAME "DatabaseName"
#define SC_SOURCEOBJECT "SourceObject"
#define SC_SOURCETYPE "SourceType"
#define SC_NATIVE "Native"
#define SC_BINDFORMATSTOCONTENT "BindFormatstoContent"
#define SC_COPYOUTPUTDATA "CopyOutputData"
#define SC_ISCASESENSITIVE "IsCaseSensitive"
#define SC_ISUSERLISTENABLED "IsUserListEnabled"
#define SC_OUTPUTPOSITION "OutputPosition"
#define SC_USERLISTINDEX "UserListIndex"
#define SC_SORTFIELDS "SortFields"
#define SC_USERLIST "UserList"
#define SC_INSERTPAGEBREAKS "InsertPageBreaks"
#define SC_SORTASCENDING "SortAscending"
#define SC_ENABLEUSERSORTLIST "EnableUserSortList"
#define SC_USERSORTLISTINDEX "UserSortListIndex"
#define SC_SKIPDUPLICATES "SkipDuplicates"
#define SC_USEREGULAREXPRESSIONS "UseRegularExpressions"
#define SC_STANDARDFORMAT "StandardFormat"
#define SC_ISVISIBLE "IsVisible"
#define SC_OPTIMALWIDTH "OptimalWidth"
#define SC_OPTIMALHEIGHT "OptimalHeight"
#define SC_ISFILTERED "IsFiltered"

#define SC_DEFAULT_TABLE_COUNT 3

using namespace rtl;
using namespace com::sun::star;

 // -----------------------------------------------------------------------

ScMyRowFormatRange::ScMyRowFormatRange()
    : nStartColumn(0),
    nRepeatColumns(0),
    nRepeatRows(0),
    nIndex(-1),
    bIsAutoStyle(sal_True)
{
}

ScRowFormatRanges::ScRowFormatRanges()
    : aRowFormatRanges()
{
}

ScRowFormatRanges::~ScRowFormatRanges()
{
}

void ScRowFormatRanges::Clear()
{
    aRowFormatRanges.clear();
}

void ScRowFormatRanges::AddRange(const ScMyRowFormatRange& aFormatRange)
{
    aRowFormatRanges.push_back(aFormatRange);
}

sal_Bool ScRowFormatRanges::GetNext(ScMyRowFormatRange& aFormatRange)
{
    ScMyRowFormatRangesVec::iterator aItr = aRowFormatRanges.begin();
    if (aItr != aRowFormatRanges.end())
    {
        aFormatRange = (*aItr);
        aRowFormatRanges.erase(aItr);
        return sal_True;
    }
    return sal_False;
}

sal_Int32 ScRowFormatRanges::GetMaxRows()
{
    ScMyRowFormatRangesVec::iterator aItr = aRowFormatRanges.begin();
    sal_Int32 nMaxRows = MAXROW + 1;
    if (aItr != aRowFormatRanges.end())
        while (aItr != aRowFormatRanges.end())
        {
            if ((*aItr).nRepeatRows < nMaxRows)
                nMaxRows = (*aItr).nRepeatRows;
            aItr++;
        }
    else
        nMaxRows = 0;
    return nMaxRows;
}

sal_Bool LessRowFormatRange (const ScMyRowFormatRange& aRange1, const ScMyRowFormatRange& aRange2)
{
    return (aRange1.nStartColumn < aRange2.nStartColumn);
}

void ScRowFormatRanges::Sort()
{
    std::sort(aRowFormatRanges.begin(), aRowFormatRanges.end(), LessRowFormatRange);
}

// ============================================================================
ScMyFormatRange::ScMyFormatRange()
    : nStyleNameIndex(-1),
    bIsAutoStyle(sal_True)
{
}

ScFormatRangeStyles::ScFormatRangeStyles()
    : aTables(),
    aStyleNames(),
    aAutoStyleNames()
{
}

ScFormatRangeStyles::~ScFormatRangeStyles()
{
    ScMyOUStringVec::iterator i = aStyleNames.begin();
    while (i != aStyleNames.end())
    {
        delete *i;
        i++;
    }
    ScMyFormatRangeVectorVec::iterator j = aTables.begin();
    while (j != aTables.end())
    {
        delete *j;
        j++;
    }
}

void ScFormatRangeStyles::AddNewTable(const sal_Int16 nTable)
{
    sal_Int16 nSize = aTables.size() - 1;
    if (nTable > nSize)
        for (sal_Int16 i = nSize; i < nTable; i++)
        {
            ScMyFormatRangeAddresses* aRangeAddresses = new ScMyFormatRangeAddresses;
            aTables.push_back(aRangeAddresses);
        }
}

sal_Int32 ScFormatRangeStyles::AddStyleName(rtl::OUString* pString, const sal_Bool bIsAutoStyle)
{
    if (bIsAutoStyle)
    {
        aAutoStyleNames.push_back(pString);
        return aAutoStyleNames.size() - 1;
    }
    else
    {
        sal_Int32 nCount = aStyleNames.size();
        sal_Bool bFound(sal_False);
        sal_Int32 i = nCount - 1;
        while ((i >= 0) && (!bFound))
        {
            if (aStyleNames.at(i)->equals(*pString))
                bFound = sal_True;
            else
                i--;
        }
        if (bFound)
            return i;
        else
        {
            aStyleNames.push_back(pString);
            return aStyleNames.size() - 1;
        }
    }
}

sal_Int32 ScFormatRangeStyles::GetIndexOfStyleName(const rtl::OUString& rString, const rtl::OUString& rPrefix, sal_Bool& bIsAutoStyle)
{
    sal_Int16 nPrefixLength = rPrefix.getLength();
    rtl::OUString sTemp = rString.copy(nPrefixLength);
    sal_Int32 nIndex = sTemp.toInt32();
    if (aAutoStyleNames.at(nIndex - 1)->equals(rString))
    {
        bIsAutoStyle = sal_True;
        return nIndex - 1;
    }
    else
    {
        sal_Int32 i = 0;
        sal_Bool bFound(sal_False);
        while (!bFound && i < aStyleNames.size())
        {
            if (aStyleNames.at(i)->equals(rString))
                bFound = sal_True;
            else
                i++;
        }
        if (bFound)
        {
            bIsAutoStyle = sal_False;
            return i;
        }
        else
        {
            i = 0;
            while (!bFound && i < aAutoStyleNames.size())
            {
                if (aAutoStyleNames.at(i)->equals(rString))
                    bFound = sal_True;
                else
                    i++;
            }
            if (bFound)
            {
                bIsAutoStyle = sal_True;
                return i;
            }
            else
                return -1;
        }
    }
}

sal_Int32 ScFormatRangeStyles::GetStyleNameIndex(const sal_Int16 nTable, const sal_Int32 nColumn, const sal_Int32 nRow, sal_Bool& bIsAutoStyle)
{
    ScMyFormatRangeAddresses* pFormatRanges = aTables[nTable];
    ScMyFormatRangeAddresses::iterator aItr = pFormatRanges->begin();
    while (aItr != pFormatRanges->end())
    {
        if (((*aItr).aRangeAddress.StartColumn <= nColumn) &&
            ((*aItr).aRangeAddress.EndColumn >= nColumn) &&
            ((*aItr).aRangeAddress.StartRow <= nRow) &&
            ((*aItr).aRangeAddress.EndRow >= nRow))
        {
            bIsAutoStyle = (*aItr).bIsAutoStyle;
            return (*aItr).nStyleNameIndex;
        }
        else
        {
            if ((*aItr).aRangeAddress.EndRow < nRow)
                aItr = pFormatRanges->erase(aItr);
            else
                aItr++;
        }
    }
    return -1;
}
void ScFormatRangeStyles::GetFormatRanges(const sal_Int32 nStartColumn, const sal_Int32 nEndColumn, const sal_Int32 nRow,
                    const sal_Int16 nTable, ScRowFormatRanges& aFormatRanges)
{
    sal_Int32 nTotalColumns = nEndColumn - nStartColumn + 1;
    ScMyFormatRangeAddresses* pFormatRanges = aTables[nTable];
    ScMyFormatRangeAddresses::iterator aItr = pFormatRanges->begin();
    sal_Int32 nColumns = 0;
    while (aItr != pFormatRanges->end() && nColumns < nTotalColumns)
    {
        if (((*aItr).aRangeAddress.StartRow <= nRow) &&
            ((*aItr).aRangeAddress.EndRow >= nRow))
        {
            if ((((*aItr).aRangeAddress.StartColumn <= nStartColumn) &&
                ((*aItr).aRangeAddress.EndColumn >= nStartColumn)) ||
                (((*aItr).aRangeAddress.StartColumn <= nEndColumn) &&
                ((*aItr).aRangeAddress.EndColumn >= nEndColumn)) ||
                (((*aItr).aRangeAddress.StartColumn >= nStartColumn) &&
                ((*aItr).aRangeAddress.EndColumn <= nEndColumn)))
            {
                ScMyRowFormatRange aRange;
                aRange.nIndex = (*aItr).nStyleNameIndex;
                aRange.bIsAutoStyle = (*aItr).bIsAutoStyle;
                if (((*aItr).aRangeAddress.StartColumn < nStartColumn) &&
                    ((*aItr).aRangeAddress.EndColumn >= nStartColumn))
                {
                    if ((*aItr).aRangeAddress.EndColumn >= nEndColumn)
                        aRange.nRepeatColumns = nTotalColumns;
                    else
                        aRange.nRepeatColumns = (*aItr).aRangeAddress.EndColumn - nStartColumn + 1;
                    aRange.nStartColumn = nStartColumn;
                }
                else if (((*aItr).aRangeAddress.StartColumn >= nStartColumn) &&
                    ((*aItr).aRangeAddress.EndColumn <= nEndColumn))
                {
                    aRange.nRepeatColumns = (*aItr).aRangeAddress.EndColumn - (*aItr).aRangeAddress.StartColumn + 1;
                    aRange.nStartColumn = (*aItr).aRangeAddress.StartColumn;
                }
                else if (((*aItr).aRangeAddress.StartColumn >= nStartColumn) &&
                    ((*aItr).aRangeAddress.StartColumn <= nEndColumn) &&
                    ((*aItr).aRangeAddress.EndColumn > nEndColumn))
                {
                    aRange.nRepeatColumns = nEndColumn - (*aItr).aRangeAddress.StartColumn + 1;
                    aRange.nStartColumn = (*aItr).aRangeAddress.StartColumn;
                }
                aRange.nRepeatRows = (*aItr).aRangeAddress.EndRow - nRow + 1;
                aFormatRanges.AddRange(aRange);
                nColumns += aRange.nRepeatColumns;
            }
            aItr++;
        }
        else
            if((*aItr).aRangeAddress.EndRow < nRow)
                aItr = pFormatRanges->erase(aItr);
            else
                aItr++;
    }
    aFormatRanges.Sort();
}

void ScFormatRangeStyles::AddRangeStyleName(const table::CellRangeAddress aCellRangeAddress, const sal_Int32 nStringIndex, const sal_Bool bIsAutoStyle)
{
    ScMyFormatRange aFormatRange;
    aFormatRange.aRangeAddress = aCellRangeAddress;
    aFormatRange.nStyleNameIndex = nStringIndex;
    aFormatRange.bIsAutoStyle = bIsAutoStyle;
    ScMyFormatRangeAddresses* pFormatRanges = aTables[aCellRangeAddress.Sheet];
    pFormatRanges->push_back(aFormatRange);
}

rtl::OUString* ScFormatRangeStyles::GetStyleName(const sal_Int16 nTable, const sal_Int32 nColumn, const sal_Int32 nRow)
{
    sal_Bool bIsAutoStyle;
    sal_Int32 nIndex = GetStyleNameIndex(nTable, nColumn, nRow, bIsAutoStyle);
    if (bIsAutoStyle)
        return aAutoStyleNames.at(nIndex);
    else
        return aStyleNames.at(nIndex);
}

rtl::OUString* ScFormatRangeStyles::GetStyleNameByIndex(const sal_Int32 nIndex, const sal_Bool bIsAutoStyle)
{
    if (bIsAutoStyle)
        return aAutoStyleNames[nIndex];
    else
        return aStyleNames[nIndex];
}

sal_Bool LessFormatRange(const ScMyFormatRange& aRange1, const ScMyFormatRange& aRange2)
{
    if (aRange1.aRangeAddress.StartRow < aRange2.aRangeAddress.StartRow)
        return sal_True;
    else
        if (aRange1.aRangeAddress.StartRow == aRange2.aRangeAddress.StartRow)
            if (aRange1.aRangeAddress.StartColumn < aRange2.aRangeAddress.StartColumn)
                return sal_True;
            else
                return sal_False;
        else
            return sal_False;
}

void ScFormatRangeStyles::Sort()
{
    sal_Int16 nTables = aTables.size();
    for (sal_Int16 i = 0; i < nTables; i++)
    {
        ScMyFormatRangeAddresses* aFormatRanges = aTables[i];
        sal_Int32 nFormatRangesCount = aFormatRanges->size();
        if (nFormatRangesCount > 1)
            std::sort(aFormatRanges->begin(), aFormatRanges->end(), LessFormatRange);
    }
}

//===========================================================================

ScColumnRowStyles::ScColumnRowStyles()
    : aTables(),
    aStyleNames()
{
}

ScColumnRowStyles::~ScColumnRowStyles()
{
    ScMyOUStringVec::iterator i = aStyleNames.begin();
    while (i != aStyleNames.end())
    {
        delete *i;
        i++;
    }
}

void ScColumnRowStyles::AddNewTable(const sal_Int16 nTable, const sal_Int32 nFields)
{
    sal_Int16 nSize = aTables.size() - 1;
    if (nTable > nSize)
        for (sal_Int32 i = nSize; i < nTable; i++)
        {
            ScMysalInt32Vec aFieldsVec(nFields + 1, -1);
            aTables.push_back(aFieldsVec);
        }
}

sal_Int32 ScColumnRowStyles::AddStyleName(rtl::OUString* pString)
{
    aStyleNames.push_back(pString);
    return aStyleNames.size() - 1;
}

sal_Int32 ScColumnRowStyles::GetIndexOfStyleName(const rtl::OUString& rString, const rtl::OUString& rPrefix)
{
    sal_Int16 nPrefixLength = rPrefix.getLength();
    rtl::OUString sTemp = rString.copy(nPrefixLength);
    sal_Int32 nIndex = sTemp.toInt32();
    if (aStyleNames.at(nIndex - 1)->equals(rString))
        return nIndex - 1;
    else
    {
        sal_Int32 i = 0;
        sal_Bool bFound(sal_False);
        while (!bFound && i < aStyleNames.size())
        {
            if (aStyleNames.at(i)->equals(rString))
                bFound = sal_True;
            else
                i++;
        }
        if (bFound)
            return i;
        else
            return -1;
    }
}

sal_Int32 ScColumnRowStyles::GetStyleNameIndex(const sal_Int16 nTable, const sal_Int32 nField)
{
    if (nField < aTables[nTable].size())
        return aTables[nTable].at(nField);
    else
        return aTables[nTable].at(aTables[nTable].size() - 1);
}

void ScColumnRowStyles::AddFieldStyleName(const sal_Int16 nTable, const sal_Int32 nField, const sal_Int32 nStringIndex)
{
    aTables[nTable].at(nField) = nStringIndex;
}

rtl::OUString* ScColumnRowStyles::GetStyleName(const sal_Int16 nTable, const sal_Int32 nField)
{
    return aStyleNames[aTables[nTable].at(nField)];
}

rtl::OUString* ScColumnRowStyles::GetStyleNameByIndex(const sal_Int32 nIndex)
{
    return aStyleNames[nIndex];
}

//==============================================================================

ScShapesContainer::ScShapesContainer()
    : aDrawPages()
{
}

ScShapesContainer::~ScShapesContainer()
{
}

void ScShapesContainer::AddNewTable()
{
    ScMyShapes aShapes;
    aDrawPages.push_back(aShapes);
}

void ScShapesContainer::AddNewShape(const sal_Int16 nTable, const ScMyShape& aShape)
{
    aDrawPages.at(nTable).push_back(aShape);
}

sal_Bool ScShapesContainer::HasShape(const sal_Int16 nTable, const sal_Int32 nColumn, const sal_Int32 nRow)
{
    sal_Int32 nShapes = aDrawPages.at(nTable).size();
    sal_Int32 i = 0;
    sal_Bool bFound(sal_False);
    sal_Bool bExist(sal_True);
    while (i < nShapes && !bFound && bExist)
    {
        if (aDrawPages[nTable][i].aAddress.Col() == nColumn &&
            aDrawPages[nTable][i].aAddress.Row() == nRow)
            bFound = sal_True;
        else
        {
            if (aDrawPages[nTable][i].aAddress.Col() <= nColumn &&
                aDrawPages[nTable][i].aAddress.Row() == nRow)
                i++;
            else
                bExist = sal_False;
        }
    }
    return bFound;
}

sal_Bool ScShapesContainer::GetNextShape(const sal_Int16 nTable, ScMyShape& aShape)
{
    ScMyShapes::iterator aItr = aDrawPages.at(nTable).begin();
    if (aItr != aDrawPages[nTable].end())
    {
        aShape = *aItr;
        aDrawPages[nTable].erase(aItr);
        return sal_True;
    }
    return sal_False;
}


sal_Bool LessShape(const ScMyShape& aShape1, const ScMyShape& aShape2)
{
    if (aShape1.aAddress.Row() < aShape2.aAddress.Row())
        return sal_True;
    else
        if (aShape1.aAddress.Row() == aShape2.aAddress.Row())
            if (aShape1.aAddress.Col() < aShape1.aAddress.Col())
                return sal_True;
            else
                return sal_False;
        else
            return sal_False;
}

void ScShapesContainer::Sort()
{
    sal_Int16 nTables = aDrawPages.size();
    for (sal_Int16 i = 0; i < nTables; i++)
    {
        ScMyShapes aShapes = aDrawPages[i];
        sal_Int32 nShapesCount = aShapes.size();
        if (nShapesCount > 1)
            std::sort(aShapes.begin(), aShapes.end(), LessShape);
    }
}

//==============================================================================

sal_Bool LessMyRange(const ScMyRange& aRange1, const ScMyRange& aRange2)
{
    if ((aRange1.aCellRange.StartColumn < aRange2.aCellRange.StartColumn) && (aRange1.aCellRange.StartRow == aRange2.aCellRange.StartRow))
        return sal_True;
    else
        if (aRange1.aCellRange.StartRow != aRange2.aCellRange.StartRow)
            if (aRange1.aCellRange.StartRow < aRange2.aCellRange.StartRow)
                return sal_True;
            else
                return sal_False;
        else
            return sal_False;
}

ScMyMergedCells::ScMyMergedCells()
    : aTables()
{
}

ScMyMergedCells::~ScMyMergedCells()
{
}

void ScMyMergedCells::AddNewTable()
{
    ScMyMergedRanges aMergedRanges;
    aTables.push_back(aMergedRanges);
}

void ScMyMergedCells::AddRange(const sal_Int16 nTable, const com::sun::star::table::CellRangeAddress aMergedRange)
{
    sal_Int32 nRows = aMergedRange.EndRow - aMergedRange.StartRow;
    sal_Int32 nFirstRow = aMergedRange.StartRow;
    ScMyRange aRange;
    aRange.bIsFirst = sal_True;
    aRange.aCellRange = aMergedRange;
    aRange.aCellRange.EndRow = nFirstRow;
    aRange.nRows = nRows + 1;
    aTables.at(nTable).push_back(aRange);
    sal_Int32 i = 1;
    while (i <= nRows)
    {
        aRange.bIsFirst = sal_False;
        aRange.nRows = 0;
        aRange.aCellRange = aMergedRange;
        aRange.aCellRange.StartRow = nFirstRow + i;
        aRange.aCellRange.EndRow = nFirstRow + i;
        aTables[nTable].push_back(aRange);
        i++;
    }
}

sal_Bool ScMyMergedCells::GetNextMergedRange(const sal_Int16 nTable, ScMyRange& aMyRange)
{
    ScMyMergedRanges::iterator aItr = aTables.at(nTable).begin();
    if (aItr != aTables[nTable].end())
    {
        aMyRange = (*aItr);
        aTables[nTable].erase(aItr);
        return sal_True;
    }
    return sal_False;
}

void ScMyMergedCells::SortAndRemoveDoublets()
{
    ScMyMergedRangesVec::iterator aItr = aTables.begin();
    while (aItr != aTables.end())
    {
        std::sort((*aItr).begin(), (*aItr).end(), LessMyRange);
        ScMyMergedRanges::iterator aItr2_old = (*aItr).begin();
        ScMyMergedRanges::iterator aItr2 = aItr2_old;
        while(aItr2 != (*aItr).end())
        {
            if (aItr2 != aItr2_old)
            {
                if (((*aItr2).aCellRange.StartColumn == (*aItr2_old).aCellRange.StartColumn) &&
                    ((*aItr2).aCellRange.StartRow == (*aItr2_old).aCellRange.StartRow))
                    aItr2 = (*aItr).erase(aItr2);
                else
                {
                    aItr2_old = aItr2;
                    aItr2++;
                }
            }
            else
                aItr2++;
        }
        aItr++;
    }
}

//==============================================================================

sal_Bool LessRange(const table::CellRangeAddress& aRange1, const table::CellRangeAddress& aRange2)
{
    if ((aRange1.StartColumn < aRange2.StartColumn) && (aRange1.StartRow == aRange2.StartRow))
        return sal_True;
    else
        if (aRange1.StartRow != aRange2.StartRow)
            if (aRange1.StartRow < aRange2.StartRow)
                return sal_True;
            else
                return sal_False;
        else
            return sal_False;
}

ScMyEmptyDatabaseRanges::ScMyEmptyDatabaseRanges(const sal_Int16 nTables)
    : aTables(nTables)
{
}

ScMyEmptyDatabaseRanges::~ScMyEmptyDatabaseRanges()
{
}

void ScMyEmptyDatabaseRanges::AddNewEmptyDatabaseRange(const com::sun::star::table::CellRangeAddress& aCellRange)
{
    sal_Int16 nTable = aCellRange.Sheet;
    ScMyEmptyDatabaseRangesVec::iterator aItr = aTables.at(nTable).begin();
    sal_Int32 nRows = aCellRange.EndRow - aCellRange.StartRow;
    sal_Int32 nFirstRow = aCellRange.StartRow;
    table::CellRangeAddress aRange;
    aRange = aCellRange;
    aRange.EndRow = nFirstRow;
    aTables.at(nTable).push_back(aRange);
    sal_Int32 i = 1;
    while (i <= nRows)
    {
        aRange = aCellRange;
        aRange.StartRow = nFirstRow + i;
        aRange.EndRow = nFirstRow + i;
        aTables[nTable].push_back(aRange);
        i++;
    }
}

sal_Bool ScMyEmptyDatabaseRanges::GetNextEmptyDatabaseRange(const sal_Int16 nTable, com::sun::star::table::CellRangeAddress& aCellRange)
{
    ScMyEmptyDatabaseRangesVec::iterator aItr = aTables.at(nTable).begin();
    if (aItr != aTables[nTable].end())
    {
        aCellRange = (*aItr);
        aTables[nTable].erase(aItr);
        return sal_True;
    }
    return sal_False;
}

void ScMyEmptyDatabaseRanges::Sort()
{
    ScMyEmptyDatabaseRangesVecVec::iterator aItr = aTables.begin();
    while (aItr != aTables.end())
    {
        std::sort((*aItr).begin(), (*aItr).end(), LessRange);
        aItr++;
    }
}

//==============================================================================

void ScMyNotEmptyCellsIterator::HasAnnotation(ScMyCell& aCell)
{
    aCell.bHasAnnotation = sal_False;
    if (xTable.is())
    {
        uno::Reference<table::XCellRange> xCellRange(xTable, uno::UNO_QUERY);
        if (xCellRange.is())
        {
            aCell.xCell = xCellRange->getCellByPosition(aCell.aCellAddress.Column, aCell.aCellAddress.Row);
            if (aCell.xCell.is())
            {
                uno::Reference<sheet::XSheetAnnotationAnchor> xSheetAnnotationAnchor(aCell.xCell, uno::UNO_QUERY);
                if (xSheetAnnotationAnchor.is())
                {
                    uno::Reference <sheet::XSheetAnnotation> xSheetAnnotation = xSheetAnnotationAnchor->getAnnotation();
                    uno::Reference<text::XSimpleText> xSimpleText(xSheetAnnotation, uno::UNO_QUERY);
                    if (xSheetAnnotation.is() && xSimpleText.is())
                    {
                        rtl::OUString sText = xSimpleText->getString();
                        if (sText.getLength())
                            aCell.bHasAnnotation = sal_True;
                    }
                }
            }
        }
    }
}

ScMyNotEmptyCellsIterator::ScMyNotEmptyCellsIterator(ScXMLExport& rTempXMLExport)
    : rExport(rTempXMLExport),
    nCurrentTable(-1),
    bHasShapes(sal_False),
    bHasShape(sal_False),
    bHasEmptyDatabaseRanges(sal_False),
    bIsEmptyDatabaseRange(sal_False),
    bHasMergedCells(sal_False),
    bIsMergedBase(sal_False),
    bIsCovered(sal_False),
    bHasAnnotation(sal_False),
    bIsMatrixBase(sal_False),
    bIsMatrixCovered(sal_False),
    bShapeWasWritten(sal_True)
{
    pCellItr = NULL;
}

ScMyNotEmptyCellsIterator::~ScMyNotEmptyCellsIterator()
{
    if (pCellItr)
        delete pCellItr;
}

void ScMyNotEmptyCellsIterator::SetShapes(ScShapesContainer* pTempShapes)
{
    pShapes = pTempShapes;
}

void ScMyNotEmptyCellsIterator::SetEmptyDatabaseRanges(ScMyEmptyDatabaseRanges* pTempEmptyDatabaseRanges)
{
    pEmptyDatabaseRanges = pTempEmptyDatabaseRanges;
}

void ScMyNotEmptyCellsIterator::SetMergedCells(ScMyMergedCells* pTempMergedCells)
{
    pMergedCells = pTempMergedCells;
}

void ScMyNotEmptyCellsIterator::SetCurrentTable(const sal_Int32 nTable)
{
    if (nCurrentTable != nTable)
    {
        nCurrentTable = nTable;
        if (!pShapes->GetNextShape(nCurrentTable, aCurrentShape))
            bHasShapes = sal_False;
        else
            bHasShapes = sal_True;
        if (!pEmptyDatabaseRanges->GetNextEmptyDatabaseRange(nCurrentTable, aNextEmptyCells))
            bHasEmptyDatabaseRanges = sal_False;
        else
            bHasEmptyDatabaseRanges = sal_True;
        if (!pMergedCells->GetNextMergedRange(nCurrentTable, aNextMergedCells))
            bHasMergedCells = sal_False;
        else
            bHasMergedCells = sal_True;
        if (pCellItr)
            delete pCellItr;
        pCellItr = new ScHorizontalCellIterator(rExport.GetDocument(), nCurrentTable, 0, 0,
            rExport.GetLastColumn(nCurrentTable), rExport.GetLastRow(nCurrentTable));
        uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( rExport.GetModel(), uno::UNO_QUERY );
        if ( xSpreadDoc.is() )
        {
            uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
            uno::Reference<container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
            if ( xIndex.is() )
            {
                sal_Int32 nTableCount = xIndex->getCount();
                if(nCurrentTable < nTableCount)
                {
                    uno::Any aTable = xIndex->getByIndex(nCurrentTable);
                    aTable>>=xTable;
                }
            }
        }
    }
}

/*void ScMyNotEmptyCellsIterator::GetNextShape()
{
    if (!pShapes->GetNextShape(nCurrentTable, aCurrentShape))
        bHasShapes = sal_False;
    else
        bHasShapes = sal_True;
}*/

sal_Bool ScMyNotEmptyCellsIterator::GetNext(ScMyCell& aCell)
{
    sal_Bool bFoundCell(sal_False);
    sal_uInt32 nCellCol, nCellRow;
    sal_uInt16 nTempCol, nTempRow;
    table::CellRangeAddress aMergeRange;
    if (pCellItr->ReturnNext(nTempCol, nTempRow))
    {
        nCellCol = nTempCol;
        nCellRow = nTempRow;
    }
    else
    {
        nCellCol = MAXCOL + 1;
        nCellRow = MAXROW + 1;
    }
    if (bHasMergedCells)
    {
        if (((nCellCol >= aNextMergedCells.aCellRange.StartColumn) &&
            (nCellRow == aNextMergedCells.aCellRange.StartRow)) ||
            (nCellRow > aNextMergedCells.aCellRange.StartRow))
        {
            if (nCellCol > aNextMergedCells.aCellRange.StartColumn)
            {
                nCellCol = aNextMergedCells.aCellRange.StartColumn;
            }
            if (nCellRow > aNextMergedCells.aCellRange.StartRow)
            {
                nCellRow = aNextMergedCells.aCellRange.StartRow;
                nCellCol = aNextMergedCells.aCellRange.StartColumn;
            }
            if (aNextMergedCells.bIsFirst)
            {
                bIsMergedBase = sal_True;
                bIsCovered = sal_False;
                aMergeRange.StartColumn = nCellCol;
                aMergeRange.EndColumn = aNextMergedCells.aCellRange.EndColumn;
                aMergeRange.StartRow = nCellRow;
                aMergeRange.EndRow = nCellRow + aNextMergedCells.nRows - 1;
            }
            else
            {
                bIsMergedBase = sal_False;
                bIsCovered = sal_True;
            }
        }
        else
        {
            bIsMergedBase = sal_False;
            bIsCovered = sal_False;
        }
    }
    else
    {
        bIsMergedBase = sal_False;
        bIsCovered = sal_False;
    }
    if (bHasShapes && bShapeWasWritten)
    {
        if (((nCellCol >= aCurrentShape.aAddress.Col()) &&
            (nCellRow == aCurrentShape.aAddress.Row())) ||
            (nCellRow > aCurrentShape.aAddress.Row()))
        {
            bHasShape = sal_True;
            bShapeWasWritten = sal_False;
            if (nCellCol > aCurrentShape.aAddress.Col())
            {
                nCellCol = aCurrentShape.aAddress.Col();
                bIsMergedBase = sal_False;
                bIsCovered = sal_False;
            }
            if (nCellRow > aCurrentShape.aAddress.Row())
            {
                nCellRow = aCurrentShape.aAddress.Row();
                nCellCol = aCurrentShape.aAddress.Col();
                bIsMergedBase = sal_False;
                bIsCovered = sal_False;
            }
        }
        else
            bHasShape = sal_False;
    }
    else
        bHasShape = sal_False;
    while (bHasEmptyDatabaseRanges &&
        (((nCellCol > aNextEmptyCells.EndColumn) &&
        (nCellRow == aNextEmptyCells.EndRow)) ||
        (nCellRow > aNextEmptyCells.EndRow)))
        if (!pEmptyDatabaseRanges->GetNextEmptyDatabaseRange(nCurrentTable, aNextEmptyCells))
            bHasEmptyDatabaseRanges = sal_False;
        else
            bHasEmptyDatabaseRanges = sal_True;
    if (bHasEmptyDatabaseRanges)
    {
        if ((nCellCol >= aNextEmptyCells.StartColumn) &&
            (nCellCol <= aNextEmptyCells.EndColumn) &&
            (nCellRow == aNextEmptyCells.StartRow))
            bIsEmptyDatabaseRange = sal_True;
        else
            bIsEmptyDatabaseRange = sal_False;
    }
    else
        bIsEmptyDatabaseRange = sal_False;
    if (bIsEmptyDatabaseRange)
        if (aNextEmptyCells.StartColumn + nCellCol < aNextEmptyCells.EndColumn)
            aNextEmptyCells.StartColumn += nCellCol + 1;
        else
            bHasEmptyDatabaseRanges = pEmptyDatabaseRanges->GetNextEmptyDatabaseRange(nCurrentTable, aNextEmptyCells);
    if(bIsMergedBase || bIsCovered)
        if (aNextMergedCells.aCellRange.StartColumn < aNextMergedCells.aCellRange.EndColumn)
        {
            aNextMergedCells.aCellRange.StartColumn++;
            aNextMergedCells.bIsFirst = sal_False;
        }
        else
            bHasMergedCells = pMergedCells->GetNextMergedRange(nCurrentTable, aNextMergedCells);
//  if(bHasShape)
//      bHasShapes = pShapes->GetNextShape(nCurrentTable, aCurrentShape);
    if (nCellCol < MAXCOL + 1 && nCellRow < MAXROW + 1)
    {
        bFoundCell = sal_True;
        aCell.aCellAddress.Sheet = nCurrentTable;
        aCell.aCellAddress.Column = nCellCol;
        aCell.aCellAddress.Row = nCellRow;
        aCell.bHasShape = bHasShape;
        aCell.bIsMergedBase = bIsMergedBase;
        aCell.bIsCovered = bIsCovered;
        aCell.aMergeRange = aMergeRange;
        aCell.bIsMatrixCovered = sal_False;
        aCell.bIsMatrixBase = sal_False;
        aCell.aShape = aCurrentShape;
        if (xTable.is())
        {
            uno::Reference<table::XCellRange> xCellRange(xTable, uno::UNO_QUERY);
            if (xCellRange.is())
            {
                if (rExport.IsMatrix(xCellRange, xTable, nCellCol, nCellRow, aCell.aMatrixRange, aCell.bIsMatrixBase))
                    aCell.bIsMatrixCovered = !aCell.bIsMatrixBase;
            }
        }
        HasAnnotation(aCell);
        aCurrentCell = aCell;
    }
    if ((nTempCol == nCellCol) && (nTempRow == nCellRow))
        pCellItr->GetNext(nTempCol, nTempRow);
    return bFoundCell;
}

//==============================================================================

void ScXMLExport::SetLastColumn(const sal_Int32 nTable, const sal_Int32 nCol)
{
    if(nCol > nLastColumns[nTable]) nLastColumns[nTable] = nCol;
}

sal_Int32 ScXMLExport::GetLastColumn(const sal_Int32 nTable)
{
    return nLastColumns[nTable];
}

void ScXMLExport::SetLastRow(const sal_Int32 nTable, const sal_Int32 nRow)
{
    if(nRow > nLastRows[nTable]) nLastRows[nTable] = nRow;
}

sal_Int32 ScXMLExport::GetLastRow(const sal_Int32 nTable)
{
    return nLastRows[nTable];
}


sal_Int16 ScXMLExport::GetFieldUnit()
{
    com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet> xProperties(
                utl::getProcessServiceFactory()->createInstance(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sheet.GlobalSheetSettings" )) ),
                com::sun::star::uno::UNO_QUERY);
    if (xProperties.is())
    {
        com::sun::star::uno::Any aAny = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Metric")));
        sal_Int16 nFieldUnit;
        if (aAny >>= nFieldUnit)
            return nFieldUnit;
    }
    return 0;
}

ScDocument* ScXMLExport::GetDocument()
{
    if (!pDoc)
    {
        uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
        if ( xSpreadDoc.is() )
        {
            ScModelObj* pDocObj = ScModelObj::getImplementation( xSpreadDoc );
            if ( pDocObj )
            {
                ScDocument* pDoc = pDocObj->GetDocument();
                if (pDoc)
                {
                    return pDoc;
                }
                else
                    return NULL;
            }
            return NULL;

        }
        return NULL;
    }
    else
        return pDoc;
}

ScXMLExport::ScXMLExport( const uno::Reference <frame::XModel>& xTempModel, const NAMESPACE_RTL(OUString)& rFileName,
                        const uno::Reference<xml::sax::XDocumentHandler>& rHandler,
                        sal_Bool bShowProgr ) :
SvXMLExport( rFileName, rHandler, xTempModel, GetFieldUnit() ),
               xModel(xTempModel),
//  rDoc(rD),
//    mpUnitConv(0),
//  pParaItemMapper( 0 ),
//  pTableItemMapper( 0 ),
//  pAutoStylePool( new SwXMLAutoStylePool ),
//  pListElements( 0 ),
//  pExportedLists( 0 ),
//  pTableLines( 0 ),
//  bExportWholeDoc( bExpWholeDoc ),
//  bExportFirstTableOnly( bExpFirstTableOnly ),
    pDoc(NULL),
    mbShowProgress( bShowProgr ),
//  pScAutoStylePool(new SvXMLAutoStylePoolP),
    pScPropHdlFactory(0L),
    pCellStylesPropertySetMapper(0L),
    pPageStylesPropertySetMapper(0L),
    pColumnStylesPropertySetMapper(0L),
    pRowStylesPropertySetMapper(0L),
    pTableStylesPropertySetMapper(0L),
    nLastColumns(SC_DEFAULT_TABLE_COUNT, 0),
    nLastRows(SC_DEFAULT_TABLE_COUNT, 0),
    aColumnStyles(),
    aRowStyles(),
    aCellStyles(),
    aShapesContainer(),
    aMergedCells(),
    xChartExportMapper(new ScExportMapper()),
    nOpenRow(-1),
    aRowFormatRanges(),
    nCurrentTable(0),
    aTableStyles(),
    pCellsItr(NULL)
{
    pDoc = GetDocument();
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
    if ( xSpreadDoc.is() )
    {
        uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
        uno::Reference<container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
        if ( xIndex.is() )
        {
            sal_Int32 nTableCount = xIndex->getCount();
            if (nTableCount > SC_DEFAULT_TABLE_COUNT)
            {
                nLastColumns.resize(nTableCount, 0);
                nLastRows.resize(nTableCount, 0);
            }
        }
    }

    pScPropHdlFactory = new XMLScPropHdlFactory;
    if(pScPropHdlFactory)
    {
        // set lock to avoid deletion
        pScPropHdlFactory->acquire();

        // build one ref
        const UniReference< XMLPropertyHandlerFactory > aFactoryRef = pScPropHdlFactory;

        // construct PropertySetMapper
        pCellStylesPropertySetMapper = new XMLCellStylesPropertySetMapper((XMLPropertyMapEntry*)aXMLScCellStylesProperties, aFactoryRef);
        if(pCellStylesPropertySetMapper)
        {
            // set lock to avoid deletion
            pCellStylesPropertySetMapper->acquire();
        }
        pPageStylesPropertySetMapper = new XMLPageStylesPropertySetMapper((XMLPropertyMapEntry*)aXMLScPageStylesProperties, aFactoryRef);
        if(pPageStylesPropertySetMapper)
        {
            // set lock to avoid deletion
            pPageStylesPropertySetMapper->acquire();
        }
        pColumnStylesPropertySetMapper = new XMLColumnStylesPropertySetMapper((XMLPropertyMapEntry*)aXMLScColumnStylesProperties, aFactoryRef);
        if(pColumnStylesPropertySetMapper)
        {
            // set lock to avoid deletion
            pColumnStylesPropertySetMapper->acquire();
        }
        pRowStylesPropertySetMapper = new XMLRowStylesPropertySetMapper((XMLPropertyMapEntry*)aXMLScRowStylesProperties, aFactoryRef);
        if(pRowStylesPropertySetMapper)
        {
            // set lock to avoid deletion
            pRowStylesPropertySetMapper->acquire();
        }
        pTableStylesPropertySetMapper = new XMLTableStylesPropertySetMapper((XMLPropertyMapEntry*)aXMLScTableStylesProperties, aFactoryRef);
        if(pTableStylesPropertySetMapper)
        {
            // set lock to avoid deletion
            pTableStylesPropertySetMapper->acquire();
        }
    }
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_CELL, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_CELL_STYLES_NAME)),
        pCellStylesPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_CELL_STYLES_PREFIX)));
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_PAGE_STYLES, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_PAGE_STYLES_NAME)),
        pPageStylesPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_PAGE_STYLES_PREFIX)));
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_COLUMN, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_COLUMN_STYLES_NAME)),
        pColumnStylesPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_COLUMN_STYLES_PREFIX)));
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_ROW, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_ROW_STYLES_NAME)),
        pRowStylesPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_ROW_STYLES_PREFIX)));
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_TABLE_STYLES_NAME)),
        pTableStylesPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_TABLE_STYLES_PREFIX)));
}


ScXMLExport::~ScXMLExport()
{
//  delete pAutoStylePool;

//  if( bShowProgress )
//      ::EndProgress( pDoc->GetDocShell() );

//  _FinitItemExport();
//  _FinitNumRuleExport();
    if (pScPropHdlFactory)
    {
        pScPropHdlFactory->release();
        pScPropHdlFactory = 0L;
    }
    if (pCellStylesPropertySetMapper)
    {
        pCellStylesPropertySetMapper->release();
        pCellStylesPropertySetMapper = 0L;
    }
    if (pPageStylesPropertySetMapper)
    {
        pPageStylesPropertySetMapper->release();
        pPageStylesPropertySetMapper = 0L;
    }
    if (pColumnStylesPropertySetMapper)
    {
        pColumnStylesPropertySetMapper->release();
        pColumnStylesPropertySetMapper = 0L;
    }
    if (pRowStylesPropertySetMapper)
    {
        pRowStylesPropertySetMapper->release();
        pRowStylesPropertySetMapper = 0L;
    }
    if (pTableStylesPropertySetMapper)
    {
        pTableStylesPropertySetMapper->release();
        pTableStylesPropertySetMapper = 0L;
    }
//  if (pScAutoStylePool)
//      delete pScAutoStylePool;
}

table::CellRangeAddress ScXMLExport::GetEndAddress(uno::Reference<sheet::XSpreadsheet>& xTable,const sal_Int16 nTable)
{
    table::CellRangeAddress aCellAddress;
    uno::Reference<sheet::XSheetCellCursor> xCursor = xTable->createCursor();
    uno::Reference<sheet::XUsedAreaCursor> xUsedArea (xCursor, uno::UNO_QUERY);
    uno::Reference<sheet::XCellRangeAddressable> xCellAddress (xCursor, uno::UNO_QUERY);
    if (xUsedArea.is() && xCellAddress.is())
    {
        xUsedArea->gotoEndOfUsedArea(sal_True);
        aCellAddress = xCellAddress->getRangeAddress();
    }
    return aCellAddress;
}

ScMyEmptyDatabaseRanges ScXMLExport::GetEmptyDatabaseRanges(const sal_Int16 nTableCount)
{
    ScMyEmptyDatabaseRanges aSkipRanges(nTableCount);
    sal_Int32 nSkipRangesCount = 0;
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
    if ( xSpreadDoc.is() )
    {
        uno::Reference <beans::XPropertySet> xPropertySet (xSpreadDoc, uno::UNO_QUERY);
        if (xPropertySet.is())
        {
            uno::Any aDatabaseRanges = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_DATABASERANGES)));
            uno::Reference <sheet::XDatabaseRanges> xDatabaseRanges;
            CheckAttrList();
            if (aDatabaseRanges >>= xDatabaseRanges)
            {
                uno::Sequence <rtl::OUString> aRanges = xDatabaseRanges->getElementNames();
                sal_Int32 nDatabaseRangesCount = aRanges.getLength();
                for (sal_Int32 i = 0; i < nDatabaseRangesCount; i++)
                {
                    rtl::OUString sDatabaseRangeName = aRanges[i];
                    uno::Any aDatabaseRange = xDatabaseRanges->getByName(sDatabaseRangeName);
                    uno::Reference <sheet::XDatabaseRange> xDatabaseRange;
                    if (aDatabaseRange >>= xDatabaseRange)
                    {
                        uno::Reference <beans::XPropertySet> xDatabaseRangePropertySet (xDatabaseRange, uno::UNO_QUERY);
                        if (xDatabaseRangePropertySet.is())
                        {
                            uno::Any aStripDataProperty = xDatabaseRangePropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_STRIPDATA)));
                            sal_Bool bStripData = sal_False;
                            if (aStripDataProperty >>= bStripData)
                                if (bStripData)
                                {
                                    uno::Sequence <beans::PropertyValue> aImportProperties = xDatabaseRange->getImportDescriptor();
                                    sal_Int32 nLength = aImportProperties.getLength();
                                    sheet::DataImportMode nSourceType = sheet::DataImportMode_NONE;
                                    for (sal_Int32 j = 0; j < nLength; j++)
                                        if (aImportProperties[j].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_SOURCETYPE)))
                                        {
                                            uno::Any aSourceType = aImportProperties[j].Value;
                                            aSourceType >>= nSourceType;
                                        }
                                    if (nSourceType != sheet::DataImportMode_NONE)
                                        aSkipRanges.AddNewEmptyDatabaseRange(xDatabaseRange->getDataArea());
                                }
                        }
                    }
                }
                if (nSkipRangesCount > 1)
                    aSkipRanges.Sort();
            }
        }
    }
    return aSkipRanges;
}

sal_Bool ScXMLExport::GetxCurrentShapes(uno::Reference<container::XIndexAccess>& xShapes)
{
    uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xCurrentTable, uno::UNO_QUERY);
    if (xDrawPageSupplier.is())
    {
        uno::Reference<drawing::XDrawPage> xDrawPage = xDrawPageSupplier->getDrawPage();
        if (xDrawPage.is())
        {
            uno::Reference<container::XIndexAccess> xShapesIndex (xDrawPage, uno::UNO_QUERY);
            if (xShapesIndex.is())
            {
                xShapes = xShapesIndex;
                return sal_True;
            }
        }
    }
    return sal_False;
}

void ScXMLExport::ExportColumns(const sal_Int16 nTable)
{
    sal_Int32 nColsRepeated = 0;
    rtl::OUString sParent;
    sal_Int32 nIndex;
    sal_Bool bPrevIsVisible = sal_True;
    sal_Int32 nPrevIndex = -1;
    uno::Reference<table::XColumnRowRange> xColumnRowRange (xCurrentTable, uno::UNO_QUERY);
    if (xColumnRowRange.is())
    {
        uno::Reference<table::XTableColumns> xTableColumns = xColumnRowRange->getColumns();
        if (xTableColumns.is())
        {
            for (sal_Int32 nColumn = 0; nColumn <= GetLastColumn(nTable); nColumn++)
            {
                CheckAttrList();
                uno::Any aColumn = xTableColumns->getByIndex(nColumn);
                uno::Reference<table::XCellRange> xTableColumn;
                if (aColumn >>= xTableColumn)
                {
                    uno::Reference <beans::XPropertySet> xColumnProperties(xTableColumn, uno::UNO_QUERY);
                    if (xColumnProperties.is())
                    {
                        nIndex = aColumnStyles.GetStyleNameIndex(nTable, nColumn);

                        uno::Any aAny = xColumnProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ISVISIBLE)));
                        sal_Bool bIsVisible(sal_True);
                        aAny >>= bIsVisible;
                        if (nColumn == 0)
                        {
                            bPrevIsVisible = bIsVisible;
                            nPrevIndex = nIndex;
                        }
                        if ((bIsVisible == bPrevIsVisible) && (nIndex == nPrevIndex))
                        {
                            nColsRepeated++;
                        }
                        else
                        {
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *aColumnStyles.GetStyleNameByIndex(nPrevIndex));
                            if (!bPrevIsVisible)
                                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_collapse);
                            if (nColsRepeated > 1)
                            {
                                OUString sOUEndCol = OUString::valueOf(static_cast <sal_Int32> (nColsRepeated));
                                AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, sOUEndCol);
                            }
                            SvXMLElementExport aElemR(*this, XML_NAMESPACE_TABLE, sXML_table_column, sal_True, sal_True);
                            bPrevIsVisible = bIsVisible;
                            nPrevIndex = nIndex;
                            nColsRepeated = 1;
                        }
                    }
                }
            }
            CheckAttrList();
            AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *aColumnStyles.GetStyleNameByIndex(nPrevIndex));
            if (!bPrevIsVisible)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_collapse);
            if (nColsRepeated > 1)
            {
                OUString sOUEndCol = OUString::valueOf(static_cast <sal_Int32> (nColsRepeated));
                AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, sOUEndCol);
            }
            SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_table_column, sal_True, sal_True);
        }
    }
}

void ScXMLExport::WriteRowContent()
{
    ScMyRowFormatRange aRange;
    sal_Int32 nIndex = -1;
    sal_Int32 nCols = 0;
    sal_Bool bIsAutoStyle(sal_True);
    sal_Bool bIsFirst(sal_True);
    while (aRowFormatRanges.GetNext(aRange))
    {
        if (bIsFirst)
        {
            nIndex = aRange.nIndex;
            bIsAutoStyle = aRange.bIsAutoStyle;
            nCols = aRange.nRepeatColumns;
            bIsFirst = sal_False;
        }
        else
        {
            if (aRange.nIndex == nIndex && aRange.bIsAutoStyle == bIsAutoStyle)
                nCols += aRange.nRepeatColumns;
            else
            {
                AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *aCellStyles.GetStyleNameByIndex(nIndex, bIsAutoStyle));
                if (nCols > 1)
                {
                    rtl::OUStringBuffer aBuf;
                    GetMM100UnitConverter().convertNumber(aBuf, nCols);
                    AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, aBuf.makeStringAndClear());
                }
                SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_table_cell, sal_True, sal_True);
                nIndex = aRange.nIndex;
                bIsAutoStyle = aRange.bIsAutoStyle;
                nCols = aRange.nRepeatColumns;
            }
        }
    }
    if (!bIsFirst)
    {
        AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *aCellStyles.GetStyleNameByIndex(nIndex, bIsAutoStyle));
        if (nCols > 1)
        {
            rtl::OUStringBuffer aBuf;
            GetMM100UnitConverter().convertNumber(aBuf, nCols);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, aBuf.makeStringAndClear());
        }
        SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_table_cell, sal_True, sal_True);
    }
}

void ScXMLExport::OpenNewRow(const sal_Int32 nIndex, const sal_Int8 nFlag, const sal_Int32 nEqualRows)
{
    AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *aRowStyles.GetStyleNameByIndex(nIndex));
    if (nFlag)
        if (nFlag & CR_HIDDEN)
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_collapse);
        else
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_filter);
    if (nEqualRows > 1)
    {
        rtl::OUStringBuffer aBuf;
        GetMM100UnitConverter().convertNumber(aBuf, nEqualRows);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_rows_repeated, aBuf.makeStringAndClear());
    }
    rtl::OUString aName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_row)));
    GetDocHandler()->ignorableWhitespace(sWS);
    GetDocHandler()->startElement( aName, GetXAttrList());
    ClearAttrList();
}

void ScXMLExport::OpenAndCloseRow(const sal_Int32 nIndex, const sal_Int8 nFlag, const sal_Int32 nEqualRows)
{
    AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *aRowStyles.GetStyleNameByIndex(nIndex));
    if (nFlag)
        if (nFlag & CR_HIDDEN)
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_collapse);
        else
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_filter);
    if (nEqualRows > 1)
    {
        rtl::OUStringBuffer aBuf;
        GetMM100UnitConverter().convertNumber(aBuf, nEqualRows);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_rows_repeated, aBuf.makeStringAndClear());
    }
     SvXMLElementExport aElemR(*this, XML_NAMESPACE_TABLE, sXML_table_row, sal_True, sal_True);
    WriteRowContent();
    aRowFormatRanges.Clear();
}

void ScXMLExport::OpenRow(const sal_Int16 nTable, const sal_Int32 nStartRow, const sal_Int32 nRepeatRow)
{
    if (nRepeatRow > 1)
    {
        sal_Int32 nPrevIndex, nIndex;
        sal_Int8 nPrevFlag, nFlag;
        sal_Int32 nEqualRows = 1;
        for (sal_Int32 nRow = nStartRow; nRow < nStartRow + nRepeatRow; nRow++)
        {
            if (nRow == nStartRow)
            {
                nPrevIndex = aRowStyles.GetStyleNameIndex(nTable, nRow);
                nPrevFlag = (pDoc->GetRowFlags(nRow, nTable)) & (CR_HIDDEN | CR_FILTERED);
            }
            else
            {
                nIndex = aRowStyles.GetStyleNameIndex(nTable, nRow);
                nFlag = (pDoc->GetRowFlags(nRow, nTable)) & (CR_HIDDEN | CR_FILTERED);
                if (nIndex == nPrevIndex && nFlag == nPrevFlag)
                    nEqualRows++;
                else
                {
                    OpenAndCloseRow(nPrevIndex, nPrevFlag, nEqualRows);
                    nEqualRows = 1;
                    nPrevIndex = nIndex;
                    nPrevFlag = nFlag;
                }
            }
        }
        OpenNewRow(nPrevIndex, nPrevFlag, nEqualRows);
    }
    else
    {
        sal_Int32 nIndex = aRowStyles.GetStyleNameIndex(nTable, nStartRow);
        sal_Int8 nFlag = (pDoc->GetRowFlags(nStartRow, nTable)) & (CR_HIDDEN | CR_FILTERED);
        OpenNewRow(nIndex, nFlag, 1);
    }
    nOpenRow = nStartRow + nRepeatRow - 1;
}

void ScXMLExport::CloseRow()
{
    if (nOpenRow > -1)
    {
        rtl::OUString aName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_row)));
        GetDocHandler()->ignorableWhitespace(sWS);
        GetDocHandler()->endElement(aName);
    }
    nOpenRow = -1;
}

void ScXMLExport::ExportFormatRanges(const sal_Int32 nStartCol, const sal_Int32 nStartRow,
    const sal_Int32 nEndCol, const sal_Int32 nEndRow, const sal_Int16 nSheet)
{
    aRowFormatRanges.Clear();
    if (nStartRow == nEndRow)
    {
        aCellStyles.GetFormatRanges(nStartCol, nEndCol, nStartRow, nSheet, aRowFormatRanges);
        if (nOpenRow == - 1)
            OpenRow(nSheet, nStartRow, 1);
        WriteRowContent();
        aRowFormatRanges.Clear();
    }
    else
    {
        if (nOpenRow > -1)
        {
            aCellStyles.GetFormatRanges(nStartCol, GetLastColumn(nSheet), nStartRow, nSheet, aRowFormatRanges);
            WriteRowContent();
            CloseRow();
            sal_Int32 nRows = 1;
            sal_Int32 nTotalRows = nEndRow - nStartRow + 1 - 1;
            while (nRows < nTotalRows)
            {
                aRowFormatRanges.Clear();
                aCellStyles.GetFormatRanges(0, GetLastColumn(nSheet), nStartRow + nRows, nSheet, aRowFormatRanges);
                sal_Int32 nMaxRows = aRowFormatRanges.GetMaxRows();
                if (nMaxRows >= nTotalRows - nRows)
                {
                    OpenRow(nSheet, nStartRow + nRows, nTotalRows - nRows);
                    nRows += nTotalRows - nRows;
                }
                else
                {
                    OpenRow(nSheet, nStartRow + nRows, nMaxRows);
                    nRows += nMaxRows;
                }
                WriteRowContent();
                CloseRow();
            }
            if (nTotalRows == 1)
                CloseRow();
            OpenRow(nSheet, nEndRow, 1);
            aRowFormatRanges.Clear();
            aCellStyles.GetFormatRanges(0, nEndCol, nEndRow, nSheet, aRowFormatRanges);
            WriteRowContent();
        }
        else
        {
            sal_Int32 nRows = 0;
            sal_Int32 nTotalRows = nEndRow - nStartRow + 1 - 1;
            while (nRows < nTotalRows)
            {
                aRowFormatRanges.Clear();
                aCellStyles.GetFormatRanges(0, GetLastColumn(nSheet), nStartRow + nRows, nSheet, aRowFormatRanges);
                sal_Int32 nMaxRows = aRowFormatRanges.GetMaxRows();
                if (nMaxRows >= nTotalRows - nRows)
                {
                    OpenRow(nSheet, nStartRow + nRows, nTotalRows - nRows);
                    nRows += nTotalRows - nRows;
                }
                else
                {
                    OpenRow(nSheet, nStartRow + nRows, nMaxRows);
                    nRows += nMaxRows;
                }
                WriteRowContent();
                CloseRow();
            }
            OpenRow(nSheet, nEndRow, 1);
            aRowFormatRanges.Clear();
            aCellStyles.GetFormatRanges(0, nEndCol, nEndRow, nSheet, aRowFormatRanges);
            WriteRowContent();
        }
    }
}

void ScXMLExport::_ExportContent()
{
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
    if ( xSpreadDoc.is() )
    {
        uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
        uno::Reference<container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
        if ( xIndex.is() )
        {
            sal_Int32 nTableCount = xIndex->getCount();
            aCellStyles.Sort();
            aShapesContainer.Sort();
            aMergedCells.SortAndRemoveDoublets();
            ScMyNotEmptyCellsIterator aCellsItr(*this);
            pCellsItr = &aCellsItr;
            aCellsItr.SetShapes(&aShapesContainer);
            ScMyEmptyDatabaseRanges aEmptyRanges = GetEmptyDatabaseRanges(nTableCount);
            aCellsItr.SetEmptyDatabaseRanges(&aEmptyRanges);
            aCellsItr.SetMergedCells(&aMergedCells);
            for (sal_Int32 nTable = 0; nTable < nTableCount; nTable++)
            {
                uno::Any aTable = xIndex->getByIndex(nTable);
                uno::Reference<sheet::XSpreadsheet> xTable;
                if (aTable>>=xTable)
                {
                    xCurrentTable = xTable;
                    uno::Reference<container::XNamed> xName (xTable, uno::UNO_QUERY );
                    if ( xName.is() )
                    {
                        nCurrentTable = nTable;
                        OUString sOUTableName = xName->getName();
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_name, sOUTableName);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, aTableStyles[nTable]);
                        uno::Reference<util::XProtectable> xProtectable (xTable, uno::UNO_QUERY);
                        if (xProtectable.is())
                            if (xProtectable->isProtected())
                                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_use_cell_protection, sXML_true);
                        SvXMLElementExport aElemT(*this, XML_NAMESPACE_TABLE, sXML_table, sal_True, sal_True);
                        CheckAttrList();
                        table::CellRangeAddress aRange = GetEndAddress(xTable, nTable);
                        SetLastColumn(nTable, aRange.EndColumn);
                        SetLastRow(nTable, aRange.EndRow);
                        aCellsItr.SetCurrentTable(nTable);
                        GetxCurrentShapes(xCurrentShapes);
                        ExportColumns(nTable);
                        sal_Bool bIsFirst(sal_True);
                        sal_Int32 nEqualCells(0);
                        ScMyCell aCell;
                        ScMyCell aPrevCell;
                        while(aCellsItr.GetNext(aCell))
                        {
                            if (bIsFirst)
                            {
                                ExportFormatRanges(0, 0, aCell.aCellAddress.Column - 1, aCell.aCellAddress.Row, nTable);
                                aPrevCell = aCell;
                                bIsFirst = sal_False;
                            }
                            else
                            {
                                if ((aPrevCell.aCellAddress.Row == aCell.aCellAddress.Row) &&
                                    (aPrevCell.aCellAddress.Column + nEqualCells + 1 == aCell.aCellAddress.Column))
                                {
                                    if(IsCellEqual(aPrevCell, aCell))
                                        nEqualCells++;
                                    else
                                    {
                                        SetRepeatAttribute(nEqualCells);
                                        WriteCell(aPrevCell);
/*                                      if (aPrevCell.bHasShape)
                                            aCellsItr.GetNextShape();*/
                                        nEqualCells = 0;
                                        aPrevCell = aCell;
                                    }
                                }
                                else
                                {
                                    SetRepeatAttribute(nEqualCells);
                                    WriteCell(aPrevCell);
/*                                  if (aPrevCell.bHasShape)
                                        aCellsItr.GetNextShape();*/
                                    ExportFormatRanges(aPrevCell.aCellAddress.Column + nEqualCells + 1, aPrevCell.aCellAddress.Row,
                                        aCell.aCellAddress.Column - 1, aCell.aCellAddress.Row, nTable);
                                    nEqualCells = 0;
                                    aPrevCell = aCell;
                                }
                            }
                        }
                        if (!bIsFirst)
                        {
                            SetRepeatAttribute(nEqualCells);
                            WriteCell(aPrevCell);
                            ExportFormatRanges(aPrevCell.aCellAddress.Column + nEqualCells + 1, aPrevCell.aCellAddress.Row,
                                GetLastColumn(nTable), GetLastRow(nTable), nTable);
                        }
                        else
                            ExportFormatRanges(0, 0, GetLastColumn(nTable), GetLastRow(nTable), nTable);
                        CloseRow();
                        nEqualCells = 0;
                    }
                }
            }
        }
        WriteNamedExpressions(xSpreadDoc);
        WriteDatabaseRanges(xSpreadDoc);
        WriteDataPilots(xSpreadDoc);
    }
}

void ScXMLExport::_ExportStyles( sal_Bool bUsed )
{
    uno::Reference <style::XStyleFamiliesSupplier> xStyleFamiliesSupplier (xModel, uno::UNO_QUERY);
    if (xStyleFamiliesSupplier.is())
    {
        uno::Reference <container::XNameAccess> aStylesFamilies = xStyleFamiliesSupplier->getStyleFamilies();
        if (aStylesFamilies.is())
        {
            uno::Any aStyleFamily = aStylesFamilies->getByName(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CellStyles")));
            uno::Reference <container::XIndexAccess> aCellStyles;
            if (aStyleFamily >>= aCellStyles)
            {
                sal_Int32 nCount = aCellStyles->getCount();
                for (sal_Int32 i = 0; i < nCount; i++)
                {
                    uno::Any aCellStyle = aCellStyles->getByIndex(i);
                    uno::Reference <beans::XPropertySet> xCellProperties;
                    if (aCellStyle >>= xCellProperties)
                    {
                        uno::Any aNumberFormat = xCellProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_NUMBERFORMAT)));
                        sal_Int32 nNumberFormat;
                        if (aNumberFormat >>= nNumberFormat)
                        {
                            addDataStyle(nNumberFormat);
                        }
                    }
                }
            }
        }
    }
    exportDataStyles();

    const UniReference< XMLPropertySetMapper > aCellStylesMapperRef = pCellStylesPropertySetMapper;
    ScXMLExportPropertyMapper* aCellStylesExpPropMapper = new ScXMLExportPropertyMapper(aCellStylesMapperRef);
    ScXMLStyleExport aStylesExp(*this, rtl::OUString(), GetAutoStylePool().get());
    aStylesExp.exportStyleFamily(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CellStyles")),
        XML_STYLE_FAMILY_TABLE_CELL_STYLES_NAME, aCellStylesExpPropMapper, FALSE, XML_STYLE_FAMILY_TABLE_CELL);

    const UniReference< XMLPropertySetMapper > aPageStylesMapperRef = pPageStylesPropertySetMapper;
    ScXMLExportPropertyMapper* aPageStylesExpPropMapper = new ScXMLExportPropertyMapper(aPageStylesMapperRef);
    aStylesExp.exportStyleFamily(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PageStyles")),
        XML_STYLE_FAMILY_TABLE_PAGE_STYLES_NAME, aPageStylesExpPropMapper, FALSE, XML_STYLE_FAMILY_TABLE_PAGE_STYLES);
}

void ScXMLExport::_ExportAutoStyles()
{
    rtl::OUString SC_SCOLUMNPREFIX(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_COLUMN_STYLES_PREFIX));
    rtl::OUString SC_SROWPREFIX(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_ROW_STYLES_PREFIX));
    rtl::OUString SC_SCELLPREFIX(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_CELL_STYLES_PREFIX));
    GetChartExport()->setTableAddressMapper(xChartExportMapper);
    GetAutoStylePool()->ClearEntries();
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
    if ( xSpreadDoc.is() )
    {
        uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
        uno::Reference<container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
        if ( xIndex.is() )
        {
            sal_Int32 nTableCount = xIndex->getCount();
            aCellStyles.AddNewTable(nTableCount - 1);
            for (sal_Int32 nTable = 0; nTable < nTableCount; nTable++)
            {
                uno::Any aTable = xIndex->getByIndex(nTable);
                uno::Reference<sheet::XSpreadsheet> xTable;
                if (aTable>>=xTable)
                {
                    aShapesContainer.AddNewTable();
                    uno::Reference<beans::XPropertySet> xTableProperties(xTable, uno::UNO_QUERY);
                    if (xTableProperties.is())
                    {
                        std::vector<XMLPropertyState> xPropStates = pTableStylesPropertySetMapper->Filter(xTableProperties);
                        if(xPropStates.size())
                        {
                            rtl::OUString sParent;
                            rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_TABLE, sParent, xPropStates);
                            if (!sName.len())
                            {
                                sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_TABLE, sParent, xPropStates);
                            }
                            aTableStyles.push_back(sName);
                        }
                    }
                    uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xTable, uno::UNO_QUERY);
                    if (xDrawPageSupplier.is())
                    {
                        uno::Reference<drawing::XDrawPage> xDrawPage = xDrawPageSupplier->getDrawPage();
                        if (xDrawPage.is())
                        {
                            uno::Reference<container::XIndexAccess> xShapesIndex (xDrawPage, uno::UNO_QUERY);
                            if (xShapesIndex.is())
                            {
                                sal_Int32 nShapesCount = xShapesIndex->getCount();
                                for (sal_Int32 nShape = 0; nShape < nShapesCount; nShape++)
                                {
                                    uno::Any aShape = xShapesIndex->getByIndex(nShape);
                                    uno::Reference<drawing::XShape> xShape;
                                    if (aShape >>= xShape)
                                    {
                                        GetShapeExport()->collectShapeAutoStyles(xShape);
                                        if (pDoc)
                                        {
                                            awt::Point aPoint = xShape->getPosition();
                                            awt::Size aSize = xShape->getSize();
                                            Rectangle aRectangle(aPoint.X, aPoint.Y, aPoint.X + aSize.Width, aPoint.Y + aSize.Height);
                                            ScRange aRange = pDoc->GetRange(nTable, aRectangle);
                                            ScMyShape aMyShape;
                                            aMyShape.aAddress = aRange.aStart;
                                            aMyShape.nIndex = nShape;
                                            aShapesContainer.AddNewShape(nTable, aMyShape);
                                            SetLastColumn(nTable, aRange.aStart.Col());
                                            SetLastRow(nTable, aRange.aStart.Row());
                                        }
                                    }
                                }
                            }
                        }
                    }
                    uno::Reference<sheet::XCellFormatRangesSupplier> xCellFormatRanges ( xTable, uno::UNO_QUERY );
                    if ( xCellFormatRanges.is() )
                    {
                        uno::Reference<container::XIndexAccess> xFormatRangesIndex = xCellFormatRanges->getCellFormatRanges();
                        if (xFormatRangesIndex.is())
                        {
                            sal_Int32 nFormatRangesCount = xFormatRangesIndex->getCount();
                            for (sal_Int32 nFormatRange = 0; nFormatRange < nFormatRangesCount; nFormatRange++)
                            {
                                aMergedCells.AddNewTable();
                                uno::Any aFormatRange = xFormatRangesIndex->getByIndex(nFormatRange);
                                uno::Reference<table::XCellRange> xCellRange;
                                if (aFormatRange >>= xCellRange)
                                {
                                    uno::Reference <beans::XPropertySet> xProperties (xCellRange, uno::UNO_QUERY);
                                    if (xProperties.is())
                                    {
                                        uno::Any aNumberFormat = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_NUMBERFORMAT)));
                                        sal_Int32 nNumberFormat;
                                        if (aNumberFormat >>= nNumberFormat)
                                        {
                                            addDataStyle(nNumberFormat);
                                        }
                                        uno::Any aStyle = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_CELLSTYLE)));
                                        rtl::OUString sStyleName;
                                        if (aStyle >>= sStyleName)
                                        {
                                            std::vector< XMLPropertyState > xPropStates = pCellStylesPropertySetMapper->Filter( xProperties );
                                            if (xPropStates.size())
                                            {
                                                sal_Int32 nIndex;
                                                rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_CELL, sStyleName, xPropStates);
                                                sal_Bool bIsAutoStyle = sal_True;
                                                if (!sName.len())
                                                {
                                                    sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_CELL, sStyleName, xPropStates);
                                                    rtl::OUString* pTemp = new rtl::OUString(sName);
                                                    nIndex = aCellStyles.AddStyleName(pTemp);
                                                }
                                                else
                                                    nIndex = aCellStyles.GetIndexOfStyleName(sName, SC_SCELLPREFIX, bIsAutoStyle);
                                                uno::Reference <sheet::XCellRangeAddressable> xCellRangeAddressable(xCellRange, uno::UNO_QUERY);
                                                if (xCellRangeAddressable.is())
                                                {
                                                    table::CellRangeAddress aRangeAddress = xCellRangeAddressable->getRangeAddress();
                                                    SetLastColumn(nTable, aRangeAddress.EndColumn);
                                                    SetLastRow(nTable, aRangeAddress.EndRow);
                                                    aCellStyles.AddRangeStyleName(aRangeAddress, nIndex, bIsAutoStyle);
                                                }
                                                uno::Reference<sheet::XSheetCellRange> xSheetCellRange(xCellRange, uno::UNO_QUERY);
                                                if (xSheetCellRange.is())
                                                {
                                                    uno::Reference<sheet::XSheetCellCursor> xCursor = xTable->createCursorByRange(xSheetCellRange);
                                                    if(xCursor.is())
                                                    {
                                                        uno::Reference<sheet::XCellRangeAddressable> xCellAddress (xCursor, uno::UNO_QUERY);
                                                        table::CellRangeAddress aCellAddress = xCellAddress->getRangeAddress();
                                                        xCursor->collapseToMergedArea();
                                                        table::CellRangeAddress aCellAddress2 = xCellAddress->getRangeAddress();
                                                        if (aCellAddress2.EndColumn > aCellAddress.EndColumn ||
                                                            aCellAddress2.EndRow > aCellAddress.EndRow)
                                                        {
                                                            SetLastColumn(nTable, aCellAddress2.EndColumn);
                                                            SetLastRow(nTable, aCellAddress2.EndRow);
                                                            aMergedCells.AddRange(nTable, aCellAddress2);
                                                        }
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                uno::Reference<sheet::XSheetCellRange> xSheetCellRange(xCellRange, uno::UNO_QUERY);
                                                if (xSheetCellRange.is())
                                                {
                                                    uno::Reference<sheet::XSheetCellCursor> xCursor = xTable->createCursorByRange(xSheetCellRange);
                                                    if(xCursor.is())
                                                    {
                                                        uno::Reference<sheet::XCellRangeAddressable> xCellAddress (xCursor, uno::UNO_QUERY);
                                                        table::CellRangeAddress aCellAddress = xCellAddress->getRangeAddress();
                                                        xCursor->collapseToMergedArea();
                                                        table::CellRangeAddress aCellAddress2 = xCellAddress->getRangeAddress();
                                                        if (aCellAddress2.EndColumn > aCellAddress.EndColumn ||
                                                            aCellAddress2.EndRow > aCellAddress.EndRow)
                                                        {
                                                            SetLastColumn(nTable, aCellAddress2.EndColumn);
                                                            SetLastRow(nTable, aCellAddress2.EndRow);
                                                            aMergedCells.AddRange(nTable, aCellAddress2);
                                                        }
                                                        rtl::OUString* pTemp = new rtl::OUString(sStyleName);
                                                        sal_Int32 nIndex = aCellStyles.AddStyleName(pTemp, sal_False);
                                                        aCellStyles.AddRangeStyleName(aCellAddress, nIndex, sal_False);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery (xTable, uno::UNO_QUERY);
                    uno::Reference<table::XColumnRowRange> xColumnRowRange (xTable, uno::UNO_QUERY);
                    if (xColumnRowRange.is())
                    {
                        if (pDoc)
                        {
                            uno::Reference<table::XTableColumns> xTableColumns = xColumnRowRange->getColumns();
                            if (xTableColumns.is())
                            {
                                sal_Int32 nColumns = pDoc->GetLastFlaggedCol(nTable);
                                SetLastColumn(nTable, nColumns);
                                table::CellRangeAddress aCellAddress = GetEndAddress(xTable, nTable);
                                if (aCellAddress.EndColumn > nColumns)
                                {
                                    nColumns++;
                                    aColumnStyles.AddNewTable(nTable, aCellAddress.EndColumn);
                                }
                                else
                                    aColumnStyles.AddNewTable(nTable, nColumns);
                                for (sal_Int32 nColumn = 0; nColumn <= nColumns; nColumn++)
                                {
                                    uno::Any aColumn = xTableColumns->getByIndex(nColumn);
                                    uno::Reference<table::XCellRange> xTableColumn;
                                    if (aColumn >>= xTableColumn)
                                    {
                                        uno::Reference <beans::XPropertySet> xColumnProperties(xTableColumn, uno::UNO_QUERY);
                                        if (xColumnProperties.is())
                                        {
                                            std::vector<XMLPropertyState> xPropStates = pColumnStylesPropertySetMapper->Filter(xColumnProperties);
                                            if(xPropStates.size())
                                            {
                                                sal_Int32 nIndex;
                                                rtl::OUString sParent;
                                                rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_COLUMN, sParent, xPropStates);
                                                if (!sName.len())
                                                {
                                                    sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_COLUMN, sParent, xPropStates);
                                                    rtl::OUString* pTemp = new rtl::OUString(sName);
                                                    nIndex = aColumnStyles.AddStyleName(pTemp);
                                                }
                                                else
                                                    nIndex = aColumnStyles.GetIndexOfStyleName(sName, SC_SCOLUMNPREFIX);
                                                aColumnStyles.AddFieldStyleName(nTable, nColumn, nIndex);
                                            }
                                        }
                                    }
                                }
                                if (aCellAddress.EndColumn > nColumns)
                                {
                                    sal_Int32 nIndex = aColumnStyles.GetStyleNameIndex(nTable, nColumns);
                                    for (sal_Int32 i = nColumns + 1; i <= aCellAddress.EndColumn; i++)
                                        aColumnStyles.AddFieldStyleName(nTable, i, nIndex);
                                }
                            }
                            uno::Reference<table::XTableRows> xTableRows = xColumnRowRange->getRows();
                            if (xTableRows.is())
                            {
                                sal_Int32 nRows = pDoc->GetLastFlaggedRow(nTable);
                                SetLastRow(nTable, nRows);
                                table::CellRangeAddress aCellAddress = GetEndAddress(xTable, nTable);
                                if (aCellAddress.EndRow > nRows)
                                {
                                    nRows++;
                                    aRowStyles.AddNewTable(nTable, aCellAddress.EndRow);
                                }
                                else
                                    aRowStyles.AddNewTable(nTable, nRows);
                                for (sal_Int32 nRow = 0; nRow <= nRows; nRow++)
                                {
                                    uno::Any aRow = xTableRows->getByIndex(nRow);
                                    uno::Reference<table::XCellRange> xTableRow;
                                    if (aRow >>= xTableRow)
                                    {
                                        uno::Reference <beans::XPropertySet> xRowProperties(xTableRow, uno::UNO_QUERY);
                                        if(xRowProperties.is())
                                        {
                                            std::vector<XMLPropertyState> xPropStates = pRowStylesPropertySetMapper->Filter(xRowProperties);
                                            if(xPropStates.size())
                                            {
                                                sal_Int32 nIndex;
                                                rtl::OUString sParent;
                                                rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_ROW, sParent, xPropStates);
                                                if (!sName.len())
                                                {
                                                    sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_ROW, sParent, xPropStates);
                                                    rtl::OUString* pTemp = new rtl::OUString(sName);
                                                    nIndex = aRowStyles.AddStyleName(pTemp);
                                                }
                                                else
                                                    nIndex = aRowStyles.GetIndexOfStyleName(sName, SC_SROWPREFIX);
                                                aRowStyles.AddFieldStyleName(nTable, nRow, nIndex);
                                            }
                                        }
                                    }
                                }
                                if (aCellAddress.EndRow > nRows)
                                {
                                    sal_Int32 nIndex = aRowStyles.GetStyleNameIndex(nTable, nRows);
                                    for (sal_Int32 i = nRows + 1; i <= aCellAddress.EndRow; i++)
                                        aRowStyles.AddFieldStyleName(nTable, i, nIndex);
                                }
                            }
                        }
                    }
                    if (xCellRangesQuery.is())
                    {
                        uno::Reference<sheet::XSheetCellRanges> xSheetCellRanges = xCellRangesQuery->queryContentCells(sheet::CellFlags::STRING);
                        if (xSheetCellRanges.is())
                        {
                            uno::Reference<container::XEnumerationAccess> xCellsAccess = xSheetCellRanges->getCells();
                            if (xCellsAccess.is())
                            {
                                uno::Reference<container::XEnumeration> xCells = xCellsAccess->createEnumeration();
                                if (xCells.is())
                                {
                                    while (xCells->hasMoreElements())
                                    {
                                        uno::Any aCell = xCells->nextElement();
                                        uno::Reference<table::XCell> xCell;
                                        if (aCell >>= xCell)
                                        {
                                            if (IsEditCell(xCell))
                                            {
                                                uno::Reference<text::XText> xText(xCell, uno::UNO_QUERY);
                                                if (xText.is())
                                                {
                                                    GetTextParagraphExport()->collectTextAutoStyles(xText);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            const UniReference< XMLPropertySetMapper > aColumnStylesMapperRef = pColumnStylesPropertySetMapper;
            ScXMLExportPropertyMapper* aColumnStylesExpPropMapper = new ScXMLExportPropertyMapper(aColumnStylesMapperRef);
            GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_COLUMN,
                *aColumnStylesExpPropMapper, GetDocHandler(), GetMM100UnitConverter(),
                GetNamespaceMap());
            const UniReference< XMLPropertySetMapper > aRowStylesMapperRef = pRowStylesPropertySetMapper;
            ScXMLExportPropertyMapper* aRowStylesExpPropMapper = new ScXMLExportPropertyMapper(aRowStylesMapperRef);
            GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_ROW,
                *aRowStylesExpPropMapper, GetDocHandler(), GetMM100UnitConverter(),
                GetNamespaceMap());
            const UniReference< XMLPropertySetMapper > aTableStylesMapperRef = pTableStylesPropertySetMapper;
            ScXMLExportPropertyMapper* aTableStylesExpPropMapper = new ScXMLExportPropertyMapper(aTableStylesMapperRef);
            GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_TABLE,
                *aTableStylesExpPropMapper, GetDocHandler(), GetMM100UnitConverter(),
                GetNamespaceMap());
            exportAutoDataStyles();
            const UniReference< XMLPropertySetMapper > aCellStylesMapperRef = pCellStylesPropertySetMapper;
            ScXMLExportPropertyMapper* aCellStylesExpPropMapper = new ScXMLExportPropertyMapper(aCellStylesMapperRef);
            GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_CELL,
                *aCellStylesExpPropMapper, GetDocHandler(), GetMM100UnitConverter(),
                GetNamespaceMap());
            GetTextParagraphExport()->exportTextAutoStyles();
            GetShapeExport()->exportAutoStyles();
            GetChartExport()->exportAutoStyles();
        }
    }
}

void ScXMLExport::_ExportMasterStyles()
{
}

sal_Bool ScXMLExport::IsMerged (const uno::Reference <table::XCellRange>& xCellRange, const sal_Int32 nCol, const sal_Int32 nRow,
                            table::CellRangeAddress& aCellAddress) const
{
    uno::Reference <table::XCellRange> xMergeCellRange = xCellRange->getCellRangeByPosition(nCol,nRow,nCol,nRow);
    uno::Reference <util::XMergeable> xMergeable (xMergeCellRange, uno::UNO_QUERY);
    if (xMergeable.is())
        if (xMergeable->getIsMerged())
        {
            uno::Reference<sheet::XSheetCellRange> xMergeSheetCellRange (xMergeCellRange, uno::UNO_QUERY);
            uno::Reference<sheet::XSpreadsheet> xTable = xMergeSheetCellRange->getSpreadsheet();
            uno::Reference<sheet::XSheetCellCursor> xMergeSheetCursor = xTable->createCursorByRange(xMergeSheetCellRange);
            if (xMergeSheetCursor.is())
            {
                xMergeSheetCursor->collapseToMergedArea();
                uno::Reference<sheet::XCellRangeAddressable> xMergeCellAddress (xMergeSheetCursor, uno::UNO_QUERY);
                if (xMergeCellAddress.is())
                {
                    aCellAddress = xMergeCellAddress->getRangeAddress();
                    return sal_True;
                }
            }
        }
    return sal_False;
}

sal_Bool ScXMLExport::IsMatrix (const uno::Reference <table::XCellRange>& xCellRange,
                            const uno::Reference <sheet::XSpreadsheet>& xTable,
                            const sal_Int32 nCol, const sal_Int32 nRow,
                            table::CellRangeAddress& aCellAddress, sal_Bool& bIsFirst) const
{
    bIsFirst = sal_False;
    uno::Reference <table::XCellRange> xMatrixCellRange = xCellRange->getCellRangeByPosition(nCol,nRow,nCol,nRow);
    uno::Reference <sheet::XArrayFormulaRange> xArrayFormulaRange (xMatrixCellRange, uno::UNO_QUERY);
    if (xMatrixCellRange.is() && xArrayFormulaRange.is())
    {
        rtl::OUString sArrayFormula = xArrayFormulaRange->getArrayFormula();
        if (sArrayFormula.getLength())
        {
            uno::Reference<sheet::XSheetCellRange> xMatrixSheetCellRange (xMatrixCellRange, uno::UNO_QUERY);
            if (xMatrixSheetCellRange.is())
            {
                uno::Reference<sheet::XSheetCellCursor> xMatrixSheetCursor = xTable->createCursorByRange(xMatrixSheetCellRange);
                if (xMatrixSheetCursor.is())
                {
                    xMatrixSheetCursor->collapseToCurrentArray();
                    uno::Reference<sheet::XCellRangeAddressable> xMatrixCellAddress (xMatrixSheetCursor, uno::UNO_QUERY);
                    if (xMatrixCellAddress.is())
                    {
                        aCellAddress = xMatrixCellAddress->getRangeAddress();
                        if ((aCellAddress.StartColumn == nCol && aCellAddress.StartRow == nRow) &&
                            (aCellAddress.EndColumn > nCol || aCellAddress.EndRow > nRow))
                        {
                            bIsFirst = sal_True;
                            return sal_True;
                        }
                        else if (aCellAddress.StartColumn != nCol || aCellAddress.StartRow != nRow ||
                            aCellAddress.EndColumn != nCol || aCellAddress.EndRow != nRow)
                            return sal_True;
                        else
                        {
                            bIsFirst = sal_True;
                            return sal_True;
                        }
                    }
                }
            }
        }
    }
    return sal_False;
}

/*sal_Bool ScXMLExport::GetCell (const uno::Reference <table::XCellRange>& xCellRange,
                                   const sal_Int32 nCol, const sal_Int32 nRow, uno::Reference <table::XCell>& xTempCell) const
{
    xTempCell = xCellRange->getCellByPosition(nCol, nRow);
    return xTempCell.is();
}*/

sal_Bool ScXMLExport::GetCellText (const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell,
        rtl::OUString& sOUTemp) const
{
    uno::Reference <text::XText> xText (xCell, uno::UNO_QUERY);
    if (xText.is())
    {
        sOUTemp = xText->getString();
        return sal_True;
    }
    return sal_False;
}

sal_Int16 ScXMLExport::GetCellType(const sal_Int32 nNumberFormat, sal_Bool& bIsStandard)
{
    uno::Reference <util::XNumberFormatsSupplier> xNumberFormatsSupplier = GetNumberFormatsSupplier();
    if (xNumberFormatsSupplier.is())
    {
        uno::Reference <util::XNumberFormats> xNumberFormats = xNumberFormatsSupplier->getNumberFormats();
        if (xNumberFormats.is())
        {
            uno::Reference <beans::XPropertySet> xNumberPropertySet = xNumberFormats->getByKey(nNumberFormat);
            uno::Any aIsStandardFormat = xNumberPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_STANDARDFORMAT)));
            aIsStandardFormat >>= bIsStandard;
            uno::Any aNumberFormat = xNumberPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_TYPE)));
            sal_Int16 nNumberFormat;
            if ( aNumberFormat >>= nNumberFormat )
            {
                return nNumberFormat;
            }
        }
    }
    return 0;
}

sal_Int32 ScXMLExport::GetCellNumberFormat(const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell) const
{
/*  uno::Reference <util::XNumberFormatsSupplier> xNumberFormatsSupplier = GetNumberFormatsSupplier();
    if (xNumberFormatsSupplier.is())
    {
        uno::Reference <util::XNumberFormats> xNumberFormats = xNumberFormatsSupplier->getNumberFormats();
        if (xNumberFormats.is())
        {*/
            uno::Reference <beans::XPropertySet> xPropertySet (xCell, uno::UNO_QUERY);
            if (xPropertySet.is())
            {
                uno::Any aNumberFormatPropertyKey = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_NUMBERFORMAT)));
                sal_Int32 nNumberFormatPropertyKey;
                if ( aNumberFormatPropertyKey>>=nNumberFormatPropertyKey )
                {
                    return nNumberFormatPropertyKey;
                }
            }
/*      }
    }*/
    return 0;
}

sal_Bool ScXMLExport::GetCellStyleNameIndex(const ScMyCell& aCell, sal_Int32& nStyleNameIndex, sal_Bool& bIsAutoStyle)
{
//  uno::Reference <beans::XPropertySet> xProperties (xCell, uno::UNO_QUERY);
//  if (xProperties.is())
//  {
//      uno::Any aStyle = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_CELLSTYLE)));
//      rtl::OUString sStyleName;
//      if (aStyle >>= sStyleName)
//      {
            //std::vector< XMLPropertyState > xPropStates = pCellStylesPropertySetMapper->Filter( xProperties );
            //rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_CELL, sStyleName, xPropStates);
            sal_Int32 nIndex = aCellStyles.GetStyleNameIndex(aCell.aCellAddress.Sheet, aCell.aCellAddress.Column, aCell.aCellAddress.Row, bIsAutoStyle);
            if (nIndex > -1)
            {
                nStyleNameIndex = nIndex;
                return sal_True;
            }
            /*else
            {
                rtl::OUString* pTemp = new rtl::OUString(sStyleName);
                nStyleNameIndex = aCellStyles.AddStyleName(pTemp, sal_False);
                return sal_True;
            }*/
//      }
//  }
    return sal_False;
}

void ScXMLExport::WriteCell (const ScMyCell& aCell)
{
    sal_Int32 nIndex;
    sal_Bool bIsAutoStyle;
    if (GetCellStyleNameIndex(aCell, nIndex, bIsAutoStyle))
        AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *aCellStyles.GetStyleNameByIndex(nIndex, bIsAutoStyle));
    sal_Bool bIsMatrix(aCell.bIsMatrixBase || aCell.bIsMatrixCovered);
    sal_Bool bIsFirstMatrixCell(aCell.bIsMatrixBase);
    if (bIsFirstMatrixCell)
    {
        sal_Int32 nColumns = aCell.aMatrixRange.EndColumn - aCell.aMatrixRange.StartColumn + 1;
        sal_Int32 nRows = aCell.aMatrixRange.EndRow - aCell.aMatrixRange.StartRow + 1;
        rtl::OUStringBuffer sColumns;
        rtl::OUStringBuffer sRows;
        SvXMLUnitConverter::convertNumber(sColumns, nColumns);
        SvXMLUnitConverter::convertNumber(sRows, nRows);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_matrix_columns_spanned, sColumns.makeStringAndClear());
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_matrix_rows_spanned, sRows.makeStringAndClear());
    }
    table::CellContentType xCellType = aCell.xCell->getType();
    sal_Bool bIsEmpty = sal_False;
    switch (xCellType)
    {
    case table::CellContentType_EMPTY :
        {
            bIsEmpty = sal_True;
        }
        break;
    case table::CellContentType_VALUE :
        {
            XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(
                *this, GetCellNumberFormat(aCell.xCell), aCell.xCell->getValue(), XML_NAMESPACE_TABLE);
        }
        break;
    case table::CellContentType_TEXT :
        {
            rtl::OUString sValue;
            if (GetCellText(aCell.xCell, sValue))
                XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(
                    *this, aCell.xCell->getFormula(), sValue, XML_NAMESPACE_TABLE);
        }
        break;
    case table::CellContentType_FORMULA :
        {
            String sFormula;
            ScCellObj* pCellObj = (ScCellObj*) ScCellRangesBase::getImplementation( aCell.xCell );
            if ( pCellObj )
            {
                ScBaseCell* pBaseCell = pCellObj->GetDocument()->GetCell(pCellObj->GetPosition());
                ScFormulaCell* pFormulaCell = (ScFormulaCell*) pBaseCell;
                if (pBaseCell && pBaseCell->GetCellType() == CELLTYPE_FORMULA)
                {
                    if (!bIsMatrix || (bIsMatrix && bIsFirstMatrixCell))
                    {
                        pFormulaCell->GetEnglishFormula(sFormula, sal_True);
                        rtl::OUString sOUFormula(sFormula);
                        if (!bIsMatrix)
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_formula, sOUFormula);
                        else
                        {
                            rtl::OUString sMatrixFormula = sOUFormula.copy(1, sOUFormula.getLength() - 2);
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_formula, sMatrixFormula);
                        }
                    }
                    if (pFormulaCell->IsValue())
                    {
                        sal_Bool bIsStandard = sal_True;
                        GetCellType(GetCellNumberFormat(aCell.xCell), bIsStandard);
                        if (bIsStandard)
                        {
                            uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
                            if ( xSpreadDoc.is() )
                            {
                                if (pDoc)
                                {
                                    pFormulaCell->GetStandardFormat(*pDoc->GetFormatTable(), 0);
                                    XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(
                                        *this, pFormulaCell->GetStandardFormat(*pDoc->GetFormatTable(), 0),
                                        aCell.xCell->getValue(), XML_NAMESPACE_TABLE);
                                }
                            }
                        }
                        else
                            XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(*this,
                                GetCellNumberFormat(aCell.xCell), aCell.xCell->getValue(), XML_NAMESPACE_TABLE);
                    }
                    else
                    {
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_value_type, sXML_string);
                        rtl::OUString sValue;
                        if (GetCellText(aCell.xCell, sValue))
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_string_value, sValue);
                    }
                }
            }
        }
        break;
    }
    if (aCell.bIsCovered)
    {
        SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_covered_table_cell, sal_True, sal_True);
        CheckAttrList();
        if (aCell.bHasAnnotation)
            WriteAnnotation(aCell.xCell);
        {
            if (!bIsEmpty)
            {
                if (IsEditCell(aCell.xCell))
                {
                    uno::Reference<text::XText> xText(aCell.xCell, uno::UNO_QUERY);
                    if ( xText.is())
                        GetTextParagraphExport()->exportText(xText);
                }
                else
                {
                    SvXMLElementExport aElemC(*this, XML_NAMESPACE_TEXT, sXML_p, sal_True, sal_False);
                    OUString sOUText;
                    if (GetCellText(aCell.xCell, sOUText))
                        GetDocHandler()->characters(sOUText);
                }
            }
        }
        if (aCell.bHasShape)
            WriteShapes(aCell.xCell, aCell);
    }
    else
    {
        if (aCell.bIsMergedBase)
        {
            sal_Int32 nColumns = aCell.aMergeRange.EndColumn - aCell.aMergeRange.StartColumn + 1;
            sal_Int32 nRows = aCell.aMergeRange.EndRow - aCell.aMergeRange.StartRow + 1;
            rtl::OUStringBuffer sColumns;
            rtl::OUStringBuffer sRows;
            SvXMLUnitConverter::convertNumber(sColumns, nColumns);
            SvXMLUnitConverter::convertNumber(sRows, nRows);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_spanned, sColumns.makeStringAndClear());
            AddAttribute(XML_NAMESPACE_TABLE, sXML_number_rows_spanned, sRows.makeStringAndClear());
        }
        SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_table_cell, sal_True, sal_True);
        CheckAttrList();
        if (aCell.bHasAnnotation)
            WriteAnnotation(aCell.xCell);
        {
            if (!bIsEmpty)
            {
                if (IsEditCell(aCell.xCell))
                {
                    uno::Reference<text::XText> xText(aCell.xCell, uno::UNO_QUERY);
                    OUString sOUText = xText->getString();
                    if ( xText.is())
                        GetTextParagraphExport()->exportText(xText);
                }
                else
                {
                    SvXMLElementExport aElemC(*this, XML_NAMESPACE_TEXT, sXML_p, sal_True, sal_False);
                    rtl::OUString sOUText;
                      if (GetCellText(aCell.xCell, sOUText))
                        GetDocHandler()->characters(sOUText);
                }
            }
        }
        if (aCell.bHasShape)
            WriteShapes(aCell.xCell, aCell);
    }
}

void ScXMLExport::WriteShapes(const uno::Reference<table::XCell>& xCell, const ScMyCell& aCell)
{
    if (xCurrentShapes.is())
    {
        sal_Int32 nIndex = aCell.aShape.nIndex;
        if (nIndex > -1)
        {
            if (pDoc)
            {
                ScMyShape aShape = aCell.aShape;
                awt::Point aPoint;
                Rectangle aRec = pDoc->GetMMRect(aCell.aCellAddress.Column, aCell.aCellAddress.Row,
                    aCell.aCellAddress.Column, aCell.aCellAddress.Row, aCell.aCellAddress.Sheet);
                aPoint.X = aRec.Left();
                aPoint.Y = aRec.Top();
                awt::Point* pPoint = &aPoint;
                while ((nIndex > -1) &&
                    (aShape.aAddress.Col() == aCell.aCellAddress.Column) &&
                    (aShape.aAddress.Row() == aCell.aCellAddress.Row) &&
                    (aShape.aAddress.Tab() == aCell.aCellAddress.Sheet))
                {
                    uno::Any aAny = xCurrentShapes->getByIndex(nIndex);
                    uno::Reference<drawing::XShape> xShape;
                    if (aAny >>= xShape)
                        GetShapeExport()->exportShape(xShape/*, pPoint*/);
                    if (aShapesContainer.GetNextShape(aCell.aCellAddress.Sheet, aShape))
                        nIndex = aShape.nIndex;
                    else
                        nIndex = -1;
                }
                if (nIndex > -1)
                    pCellsItr->SetCurrentShape(aShape);
                else
                    pCellsItr->SetHasShape(sal_False);
            }
        }
    }
}

void ScXMLExport::WriteAnnotation(const uno::Reference<table::XCell>& xCell)
{
    uno::Reference<sheet::XSheetAnnotationAnchor> xSheetAnnotationAnchor(xCell, uno::UNO_QUERY);
    if (xSheetAnnotationAnchor.is())
    {
        uno::Reference <sheet::XSheetAnnotation> xSheetAnnotation = xSheetAnnotationAnchor->getAnnotation();
        uno::Reference<text::XSimpleText> xSimpleText(xSheetAnnotation, uno::UNO_QUERY);
        if (xSheetAnnotation.is() && xSimpleText.is())
        {
            rtl::OUString sText = xSimpleText->getString();
            if (sText.getLength())
            {
                AddAttribute(XML_NAMESPACE_OFFICE, sXML_author, xSheetAnnotation->getAuthor());
                String aDate(xSheetAnnotation->getDate());
                if (pDoc)
                {
                    SvNumberFormatter* pNumForm = pDoc->GetFormatTable();
                    double fDate;
                    sal_uInt32 nfIndex = pNumForm->GetFormatIndex(NF_DATE_SYS_DDMMYYYY, LANGUAGE_SYSTEM);
                    if (pNumForm->IsNumberFormat(aDate, nfIndex, fDate))
                    {
                        rtl::OUStringBuffer sBuf;
                        GetMM100UnitConverter().convertDateTime(sBuf, fDate);
                        AddAttribute(XML_NAMESPACE_OFFICE, sXML_create_date, sBuf.makeStringAndClear());
                    }
                }
                if (!xSheetAnnotation->getIsVisible())
                    AddAttributeASCII(XML_NAMESPACE_OFFICE, sXML_display, sXML_false);
                SvXMLElementExport aElemA(*this, XML_NAMESPACE_OFFICE, sXML_annotation, sal_True, sal_False);
                GetDocHandler()->characters(sText);
            }
        }
        CheckAttrList();
    }
}

void ScXMLExport::SetRepeatAttribute (const sal_Int32 nEqualCellCount)
{
    if (nEqualCellCount > 0)
    {
        sal_Int32 nTemp = nEqualCellCount + 1;
        OUString sOUEqualCellCount = OUString::valueOf(nTemp);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, sOUEqualCellCount);
    }
}

sal_Bool ScXMLExport::IsCellTypeEqual (const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell1,
        const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell2) const
{
    return (xCell1->getType() == xCell2->getType());
}

sal_Bool ScXMLExport::IsEditCell(const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell) const
{
    ScCellObj* pCellObj = (ScCellObj*) ScCellRangesBase::getImplementation( xCell );
    if ( pCellObj )
    {
        ScBaseCell* pBaseCell = pCellObj->GetDocument()->GetCell(pCellObj->GetPosition());
        if (pBaseCell)
        {
            if (pBaseCell->GetCellType() == CELLTYPE_EDIT)
                return sal_True;
            else
                return sal_False;
        }
    }
    return sal_True;
}

sal_Bool ScXMLExport::IsAnnotationEqual(const uno::Reference<table::XCell>& xCell1,
                                        const uno::Reference<table::XCell>& xCell2)
{
    uno::Reference<sheet::XSheetAnnotationAnchor> xSheetAnnotationAnchor1(xCell1, uno::UNO_QUERY);
    uno::Reference<sheet::XSheetAnnotationAnchor> xSheetAnnotationAnchor2(xCell2, uno::UNO_QUERY);
    if (xSheetAnnotationAnchor1.is() && xSheetAnnotationAnchor2.is())
    {
        uno::Reference <sheet::XSheetAnnotation> xSheetAnnotation1 = xSheetAnnotationAnchor1->getAnnotation();
        uno::Reference <sheet::XSheetAnnotation> xSheetAnnotation2 = xSheetAnnotationAnchor2->getAnnotation();
        uno::Reference<text::XSimpleText> xSimpleText1(xSheetAnnotation1, uno::UNO_QUERY);
        uno::Reference<text::XSimpleText> xSimpleText2(xSheetAnnotation2, uno::UNO_QUERY);
        if (xSheetAnnotation1.is() && xSimpleText1.is() &&
            xSheetAnnotation2.is() && xSimpleText2.is())
        {
            rtl::OUString sText1 = xSimpleText1->getString();
            rtl::OUString sText2 = xSimpleText2->getString();
            sal_Int32 nLength1 = sText1.getLength();
            sal_Int32 nLength2 = sText2.getLength();
            if (nLength1 && nLength2)
                if (sText1 == sText2 &&
                    xSheetAnnotation1->getAuthor() == xSheetAnnotation2->getAuthor() &&
                    xSheetAnnotation1->getDate() == xSheetAnnotation2->getDate() &&
                    xSheetAnnotation1->getIsVisible() == xSheetAnnotation2->getIsVisible())
                    return sal_True;
                else
                    return sal_False;
            else
                if (nLength1 || nLength2)
                    return sal_False;
                else
                    return sal_True;
        }
    }
    return sal_False;
}

sal_Bool ScXMLExport::IsCellEqual (const ScMyCell& aCell1, const ScMyCell& aCell2)
{
    sal_Bool bIsEqual = sal_False;
    if( !aCell1.bIsMergedBase && !aCell2.bIsMergedBase &&
        aCell1.bIsCovered == aCell2.bIsCovered &&
        !aCell1.bIsMatrixBase && !aCell2.bIsMatrixBase &&
        aCell1.bIsMatrixCovered == aCell2.bIsMatrixCovered &&
        aCell1.bHasAnnotation == aCell2.bHasAnnotation &&
        !aCell1.bHasShape && !aCell2.bHasShape)
    {
        if (!aCell1.bHasAnnotation || (aCell1.bHasAnnotation && IsAnnotationEqual(aCell1.xCell, aCell2.xCell)))
        {
            sal_Int32 nIndex1, nIndex2;
            sal_Bool bIsAutoStyle1, bIsAutoStyle2;
            if (GetCellStyleNameIndex(aCell1, nIndex1, bIsAutoStyle1) &&
                GetCellStyleNameIndex(aCell2, nIndex2, bIsAutoStyle2))
            {
                if ((nIndex1 == nIndex2) && (bIsAutoStyle1 == bIsAutoStyle2) &&
                    IsCellTypeEqual(aCell1.xCell, aCell2.xCell))
                {
                    table::CellContentType eCellType = aCell1.xCell->getType();
                    switch ( eCellType )
                    {
                    case table::CellContentType_EMPTY :
                        {
                            bIsEqual = sal_True;
                        }
                        break;
                    case table::CellContentType_VALUE :
                        {
                            double fCell1 = aCell1.xCell->getValue();
                            double fCell2 = aCell2.xCell->getValue();
                            bIsEqual = (fCell1 == fCell2);
                        }
                        break;
                    case table::CellContentType_TEXT :
                        {
                            if (IsEditCell(aCell1.xCell) || IsEditCell(aCell2.xCell))
                                bIsEqual = sal_False;
                            else
                            {
                                OUString sOUCell1, sOUCell2;
                                if (GetCellText(aCell1.xCell, sOUCell1) && GetCellText(aCell2.xCell, sOUCell2))
                                {
                                    bIsEqual = (sOUCell1 == sOUCell2);
                                }
                                else
                                    bIsEqual = sal_False;
                            }
                        }
                        break;
                    case table::CellContentType_FORMULA :
                        {
                            bIsEqual = sal_False;
                        }
                        break;
                    default :
                        {
                            bIsEqual = sal_False;
                        }
                        break;
                    }
                }
            }
        }
    }
    return bIsEqual;
}

void ScXMLExport::GetStringFromRange(const ScRange& aRange, rtl::OUString& rString) const
{
    ScAddress aStartAddress(aRange.aStart.Col(), aRange.aStart.Row(), aRange.aStart.Tab());
    ScAddress aEndAddress(aRange.aEnd.Col(), aRange.aEnd.Row(), aRange.aEnd.Tab());
    String sStartAddress;
    String sEndAddress;
    aStartAddress.Format(sStartAddress, SCA_VALID | SCA_TAB_3D, pDoc);
    aEndAddress.Format(sEndAddress, SCA_VALID | SCA_TAB_3D, pDoc);
    rtl::OUString sOUStartAddress(sStartAddress);
    sOUStartAddress += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(":"));
    rtl::OUString sOUEndAddress(sEndAddress);
    sOUStartAddress += sOUEndAddress;
    rString = sOUStartAddress;
}

void ScXMLExport::GetStringFromRange(const table::CellRangeAddress& aRange, rtl::OUString& rString) const
{
    ScAddress aStartAddress(aRange.StartColumn, aRange.StartRow, aRange.Sheet);
    ScAddress aEndAddress(aRange.EndColumn, aRange.EndRow, aRange.Sheet);
    String sStartAddress;
    String sEndAddress;
    aStartAddress.Format(sStartAddress, SCA_VALID | SCA_TAB_3D, pDoc);
    aEndAddress.Format(sEndAddress, SCA_VALID | SCA_TAB_3D, pDoc);
    rtl::OUString sOUStartAddress(sStartAddress);
    sOUStartAddress += rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(":"));
    rtl::OUString sOUEndAddress(sEndAddress);
    sOUStartAddress += sOUEndAddress;
    rString = sOUStartAddress;
}

void ScXMLExport::GetStringOfFunction(const sal_Int32 nFunction, rtl::OUString& rString) const
{
    switch (nFunction)
    {
        case sheet::GeneralFunction_AUTO : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("auto")); break;
        case sheet::GeneralFunction_AVERAGE : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("average")); break;
        case sheet::GeneralFunction_COUNT : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("count")); break;
        case sheet::GeneralFunction_COUNTNUMS : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("countnums")); break;
        case sheet::GeneralFunction_MAX : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("max")); break;
        case sheet::GeneralFunction_MIN : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("min")); break;
        case sheet::GeneralFunction_NONE : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("none")); break;
        case sheet::GeneralFunction_PRODUCT : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("product")); break;
        case sheet::GeneralFunction_STDEV : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("stdev")); break;
        case sheet::GeneralFunction_STDEVP : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("stdevp")); break;
        case sheet::GeneralFunction_SUM : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("sum")); break;
        case sheet::GeneralFunction_VAR : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("var")); break;
        case sheet::GeneralFunction_VARP : rString = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("varp")); break;
    }
}

void ScXMLExport::WriteNamedExpressions(const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheetDocument>& xSpreadDoc)
{
    uno::Reference <beans::XPropertySet> xPropertySet (xSpreadDoc, uno::UNO_QUERY);
    if (xPropertySet.is())
    {
        uno::Any aNamedRanges = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_NAMEDRANGES)));
        uno::Reference <sheet::XNamedRanges> xNamedRanges;
        CheckAttrList();
        if (aNamedRanges >>= xNamedRanges)
        {
            uno::Sequence <rtl::OUString> aRangesNames = xNamedRanges->getElementNames();
            sal_Int32 nNamedRangesCount = aRangesNames.getLength();
            if (nNamedRangesCount > 0)
            {
                if (pDoc)
                {
                    ScRangeName* pNamedRanges = pDoc->GetRangeName();
                    SvXMLElementExport aElemNEs(*this, XML_NAMESPACE_TABLE, sXML_named_expressions, sal_True, sal_True);
                    for (sal_Int32 i = 0; i < nNamedRangesCount; i++)
                    {
                        CheckAttrList();
                        rtl::OUString sNamedRange = aRangesNames[i];
                        uno::Any aNamedRange = xNamedRanges->getByName(sNamedRange);
                        uno::Reference <sheet::XNamedRange> xNamedRange;
                        if (aNamedRange >>= xNamedRange)
                        {
                            uno::Reference <container::XNamed> xNamed (xNamedRange, uno::UNO_QUERY);
                            uno::Reference <sheet::XCellRangeReferrer> xCellRangeReferrer (xNamedRange, uno::UNO_QUERY);
                            if (xNamed.is() && xCellRangeReferrer.is())
                            {
                                rtl::OUString sOUName = xNamed->getName();
                                AddAttribute(XML_NAMESPACE_TABLE, sXML_name, sOUName);
                                sal_uInt16 nRangeIndex;
                                String sName(sOUName);
                                pNamedRanges->SearchName(sName, nRangeIndex);
                                ScRangeData* pNamedRange = (*pNamedRanges)[nRangeIndex]; //should get directly and not with ScDocument
                                String sContent(xNamedRange->getContent());
                                pNamedRange->GetEnglishSymbol(sContent, sal_True);
                                rtl::OUString sOUTempContent(sContent);
                                uno::Reference <table::XCellRange> xCellRange = xCellRangeReferrer->getReferredCells();
                                table::CellAddress aCellAddress = xNamedRange->getReferencePosition();
                                ScAddress aBaseCellAddress(aCellAddress.Column, aCellAddress.Row, aCellAddress.Sheet);
                                String sBaseCellAddress;
                                aBaseCellAddress.Format(sBaseCellAddress, SCA_ABS_3D, pDoc);
                                rtl::OUString sOUBaseCellAddress(sBaseCellAddress);
                                AddAttribute(XML_NAMESPACE_TABLE, sXML_base_cell_address, sOUBaseCellAddress);
                                if(xCellRange.is())
                                {
                                    rtl::OUString sOUContent = sOUTempContent.copy(1, sOUTempContent.getLength() - 2);
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_cell_range_address, sOUContent);
                                    sal_Int32 nRangeType = xNamedRange->getType();
                                    rtl::OUStringBuffer sBufferRangeType;
                                    if (nRangeType & sheet::NamedRangeFlag::COLUMN_HEADER == sheet::NamedRangeFlag::COLUMN_HEADER)
                                        sBufferRangeType.appendAscii(sXML_repeat_column);
                                    if (nRangeType & sheet::NamedRangeFlag::ROW_HEADER == sheet::NamedRangeFlag::ROW_HEADER)
                                    {
                                        if (sBufferRangeType.getLength() > 0)
                                            sBufferRangeType.appendAscii(" ");
                                        sBufferRangeType.appendAscii(sXML_repeat_row);
                                    }
                                    if (nRangeType & sheet::NamedRangeFlag::FILTER_CRITERIA == sheet::NamedRangeFlag::FILTER_CRITERIA)
                                    {
                                        if (sBufferRangeType.getLength() > 0)
                                            sBufferRangeType.appendAscii(" ");
                                        sBufferRangeType.appendAscii(sXML_filter);
                                    }
                                    if (nRangeType & sheet::NamedRangeFlag::PRINT_AREA == sheet::NamedRangeFlag::PRINT_AREA)
                                    {
                                        if (sBufferRangeType.getLength() > 0)
                                            sBufferRangeType.appendAscii(" ");
                                        sBufferRangeType.appendAscii(sXML_print_range);
                                    }
                                    rtl::OUString sRangeType = sBufferRangeType.makeStringAndClear();
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_range_usable_as, sRangeType);
                                    SvXMLElementExport aElemNR(*this, XML_NAMESPACE_TABLE, sXML_named_range, sal_True, sal_True);
                                }
                                else
                                {
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_expression, sOUTempContent);
                                    SvXMLElementExport aElemNE(*this, XML_NAMESPACE_TABLE, sXML_named_expression, sal_True, sal_True);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ScXMLExport::WriteImportDescriptor(const uno::Sequence <beans::PropertyValue> aImportDescriptor)
{
    sal_Int32 nProperties = aImportDescriptor.getLength();
    rtl::OUString sDatabaseName;
    rtl::OUString sSourceObject;
    sheet::DataImportMode nSourceType;
    sal_Bool bNative;
    for (sal_Int16 i = 0; i < nProperties; i++)
    {
        if (aImportDescriptor[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_DATABASENAME)))
        {
            uno::Any aDatabaseName = aImportDescriptor[i].Value;
            aDatabaseName >>= sDatabaseName;
        }
        else if (aImportDescriptor[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_SOURCEOBJECT)))
        {
            uno::Any aSourceObject = aImportDescriptor[i].Value;
            aSourceObject >>= sSourceObject;
        }
        else if (aImportDescriptor[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_SOURCETYPE)))
        {
            uno::Any aSourceType = aImportDescriptor[i].Value;
            aSourceType >>= nSourceType;
        }
        else if (aImportDescriptor[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_NATIVE)))
        {
            uno::Any aNative = aImportDescriptor[i].Value;
            aNative >>= bNative;
        }
    }
    switch (nSourceType)
    {
        case sheet::DataImportMode_NONE : break;
        case sheet::DataImportMode_QUERY :
        {
            AddAttribute(XML_NAMESPACE_TABLE, sXML_database_name, sDatabaseName);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_query_name, sSourceObject);
            SvXMLElementExport aElemID(*this, XML_NAMESPACE_TABLE, sXML_database_source_query, sal_True, sal_True);
            CheckAttrList();
        }
        break;
        case sheet::DataImportMode_TABLE :
        {
            AddAttribute(XML_NAMESPACE_TABLE, sXML_database_name, sDatabaseName);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_table_name, sSourceObject);
            SvXMLElementExport aElemID(*this, XML_NAMESPACE_TABLE, sXML_database_source_table, sal_True, sal_True);
            CheckAttrList();
        }
        break;
        case sheet::DataImportMode_SQL :
        {
            AddAttribute(XML_NAMESPACE_TABLE, sXML_database_name, sDatabaseName);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_sql_statement, sSourceObject);
            if (!bNative)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_parse_sql_statement, sXML_true);
            SvXMLElementExport aElemID(*this, XML_NAMESPACE_TABLE, sXML_database_source_sql, sal_True, sal_True);
            CheckAttrList();
        }
        break;
    }
}

rtl::OUString ScXMLExport::getOperatorXML(const sheet::FilterOperator aFilterOperator, const sal_Bool bUseRegularExpressions) const
{
    if (bUseRegularExpressions)
    {
        switch (aFilterOperator)
        {
            case sheet::FilterOperator_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_match));
                break;
            case sheet::FilterOperator_NOT_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_nomatch));
                break;
        }
    }
    else
    {
        switch (aFilterOperator)
        {
            case sheet::FilterOperator_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("="));
                break;
            case sheet::FilterOperator_NOT_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("!="));
                break;
            case sheet::FilterOperator_BOTTOM_PERCENT :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_bottom_percent));
                break;
            case sheet::FilterOperator_BOTTOM_VALUES :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_bottom_values));
                break;
            case sheet::FilterOperator_EMPTY :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_empty));
                break;
            case sheet::FilterOperator_GREATER :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(">"));
                break;
            case sheet::FilterOperator_GREATER_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(">="));
                break;
            case sheet::FilterOperator_LESS :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("<"));
                break;
            case sheet::FilterOperator_LESS_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("<="));
                break;
            case sheet::FilterOperator_NOT_EMPTY :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_noempty));
                break;
            case sheet::FilterOperator_TOP_PERCENT :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_top_percent));
                break;
            case sheet::FilterOperator_TOP_VALUES :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_top_values));
                break;
        }
    }
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("="));
}

void ScXMLExport::WriteCondition(const sheet::TableFilterField& aFilterField, sal_Bool bIsCaseSensitive, sal_Bool bUseRegularExpressions)
{
    AddAttribute(XML_NAMESPACE_TABLE, sXML_field_number, rtl::OUString::valueOf(aFilterField.Field));
    if (bIsCaseSensitive)
        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_case_sensitive, sXML_true);
    if (aFilterField.IsNumeric)
    {
        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_data_type, sXML_number);
        rtl::OUStringBuffer sBuffer;
        GetMM100UnitConverter().convertNumber(sBuffer, aFilterField.NumericValue);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_value, sBuffer.makeStringAndClear());
    }
    else
        AddAttribute(XML_NAMESPACE_TABLE, sXML_value, aFilterField.StringValue);
    AddAttribute(XML_NAMESPACE_TABLE, sXML_operator, getOperatorXML(aFilterField.Operator, bUseRegularExpressions));
    SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_filter_condition, sal_True, sal_True);
}

void ScXMLExport::WriteFilterDescriptor(const uno::Reference <sheet::XSheetFilterDescriptor>& xSheetFilterDescriptor, const rtl::OUString sDatabaseRangeName)
{
    uno::Sequence <sheet::TableFilterField> aTableFilterFields = xSheetFilterDescriptor->getFilterFields();
    sal_Int32 nTableFilterFields = aTableFilterFields.getLength();
    if (nTableFilterFields > 0)
    {
        uno::Reference <beans::XPropertySet> xPropertySet (xSheetFilterDescriptor, uno::UNO_QUERY);
        if (xPropertySet.is())
        {
            sal_Bool bCopyOutputData;
            uno::Any aCopyOutputData = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_COPYOUTPUTDATA)));
            if (aCopyOutputData >>= bCopyOutputData)
                if (bCopyOutputData)
                {
                    table::CellAddress aOutputPosition;
                    uno::Any aTempOutputPosition = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_OUTPUTPOSITION)));
                    if (aTempOutputPosition >>= aOutputPosition)
                    {
                        ScAddress aTempCellAddress(aOutputPosition.Column, aOutputPosition.Row, aOutputPosition.Sheet);
                        String sCellAddress;
                        aTempCellAddress.Format(sCellAddress, SCA_VALID | SCA_TAB_3D, pDoc);
                        rtl::OUString sOUCellAddress(sCellAddress);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_target_range_address, sOUCellAddress);
                    }
                }
            ScDBCollection* pDBCollection = pDoc->GetDBCollection();
            sal_uInt16 nIndex;
            pDBCollection->SearchName(sDatabaseRangeName, nIndex);
            ScDBData* pDBData = (*pDBCollection)[nIndex];
            ScRange aAdvSource;
            if (pDBData->GetAdvancedQuerySource(aAdvSource))
            {
                rtl::OUString sOUCellAddress;
                GetStringFromRange(aAdvSource, sOUCellAddress);
                AddAttribute(XML_NAMESPACE_TABLE, sXML_target_range_address, sOUCellAddress);
            }

            sal_Bool bSkipDuplicates;
            uno::Any aSkipDuplicates = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_SKIPDUPLICATES)));
            if (aSkipDuplicates >>= bSkipDuplicates)
                if (bSkipDuplicates)
                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_display_duplicates, sXML_false);
            SvXMLElementExport aElemF(*this, XML_NAMESPACE_TABLE, sXML_filter, sal_True, sal_True);
            CheckAttrList();
            sal_Bool bIsCaseSensitive = sal_False;
            uno::Any aIsCaseSensitive = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ISCASESENSITIVE)));
            aIsCaseSensitive >>= bIsCaseSensitive;
            sal_Bool bUseRegularExpressions = sal_False;
            uno::Any aUseRegularExpressions = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_USEREGULAREXPRESSIONS)));
            aUseRegularExpressions >>= bUseRegularExpressions;
            sal_Bool bAnd = sal_False;
            sal_Bool bOr = sal_False;
            for (sal_Int32 i = 1; i < nTableFilterFields; i++)
            {
                if (aTableFilterFields[i].Connection == sheet::FilterConnection_AND)
                    bAnd = sal_True;
                else
                    bOr = sal_True;
            }
            if (bOr && !bAnd)
            {
                SvXMLElementExport aElemOr(*this, XML_NAMESPACE_TABLE, sXML_filter_or, sal_True, sal_True);
                for (i = 0; i < nTableFilterFields; i++)
                {
                    WriteCondition(aTableFilterFields[i], bIsCaseSensitive, bUseRegularExpressions);
                }
            }
            else if (bAnd && !bOr)
            {
                SvXMLElementExport aElemAnd(*this, XML_NAMESPACE_TABLE, sXML_filter_and, sal_True, sal_True);
                for (i = 0; i < nTableFilterFields; i++)
                {
                    WriteCondition(aTableFilterFields[i], bIsCaseSensitive, bUseRegularExpressions);
                }
            }
            else if (nTableFilterFields  == 1)
            {
                WriteCondition(aTableFilterFields[0], bIsCaseSensitive, bUseRegularExpressions);
            }
            else
            {
                SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_filter_or, sal_True, sal_True);
                sheet::TableFilterField aPrevFilterField = aTableFilterFields[0];
                sheet::FilterConnection aConnection = aTableFilterFields[1].Connection;
                sal_Bool bOpenAndElement;
                rtl::OUString aName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_filter_and)));
                if (aConnection == sheet::FilterConnection_AND)
                {
                    GetDocHandler()->ignorableWhitespace(sWS);
                    GetDocHandler()->startElement( aName, GetXAttrList());
                    ClearAttrList();
                    bOpenAndElement = sal_True;
                }
                else
                    bOpenAndElement = sal_False;
                for (i = 1; i < nTableFilterFields; i++)
                {
                    if (aConnection != aTableFilterFields[i].Connection)
                    {
                        aConnection = aTableFilterFields[i].Connection;
                        if (aTableFilterFields[i].Connection == sheet::FilterConnection_AND)
                        {
                            GetDocHandler()->ignorableWhitespace(sWS);
                            GetDocHandler()->startElement( aName, GetXAttrList());
                            ClearAttrList();
                            bOpenAndElement = sal_True;
                            WriteCondition(aPrevFilterField, bIsCaseSensitive, bUseRegularExpressions);
                            aPrevFilterField = aTableFilterFields[i];
                            if (i == nTableFilterFields - 1)
                            {
                                WriteCondition(aPrevFilterField, bIsCaseSensitive, bUseRegularExpressions);
                                GetDocHandler()->ignorableWhitespace(sWS);
                                GetDocHandler()->endElement(aName);
                                bOpenAndElement = sal_False;
                            }
                        }
                        else
                        {
                            WriteCondition(aPrevFilterField, bIsCaseSensitive, bUseRegularExpressions);
                            aPrevFilterField = aTableFilterFields[i];
                            if (bOpenAndElement)
                            {
                                GetDocHandler()->ignorableWhitespace(sWS);
                                GetDocHandler()->endElement(aName);
                                bOpenAndElement = sal_False;
                            }
                            if (i == nTableFilterFields - 1)
                            {
                                WriteCondition(aPrevFilterField, bIsCaseSensitive, bUseRegularExpressions);
                            }
                        }
                    }
                    else
                    {
                        WriteCondition(aPrevFilterField, bIsCaseSensitive, bUseRegularExpressions);
                        aPrevFilterField = aTableFilterFields[i];
                        if (i == nTableFilterFields - 1)
                            WriteCondition(aPrevFilterField, bIsCaseSensitive, bUseRegularExpressions);
                    }
                }
            }
        }
    }
}

void ScXMLExport::WriteSortDescriptor(const uno::Sequence <beans::PropertyValue> aSortProperties)
{
    uno::Sequence <util::SortField> aSortFields;
    sal_Bool bBindFormatsToContent = sal_True;
    sal_Bool bCopyOutputData = sal_False;
    sal_Bool bIsCaseSensitive = sal_False;
    sal_Bool bIsUserListEnabled = sal_False;
    table::CellAddress aOutputPosition;
    sal_Int32 nUserListIndex;
    sal_Int32 nProperties = aSortProperties.getLength();
    for (sal_Int32 i = 0; i < nProperties; i++)
    {
        if (aSortProperties[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_BINDFORMATSTOCONTENT)))
        {
            uno::Any aBindFormatsToContent = aSortProperties[i].Value;
            aBindFormatsToContent >>= bBindFormatsToContent;
        }
        else if (aSortProperties[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_COPYOUTPUTDATA)))
        {
            uno::Any aCopyOutputData = aSortProperties[i].Value;
            aCopyOutputData >>= bCopyOutputData;
        }
        else if (aSortProperties[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ISCASESENSITIVE)))
        {
            uno::Any aIsCaseSensitive = aSortProperties[i].Value;
            aIsCaseSensitive >>= bIsCaseSensitive;
        }
        else if (aSortProperties[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ISUSERLISTENABLED)))
        {
            uno::Any aIsUserListEnabled = aSortProperties[i].Value;
            aIsUserListEnabled >>= bIsUserListEnabled;
        }
        else if (aSortProperties[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_OUTPUTPOSITION)))
        {
            uno::Any aTempOutputPosition = aSortProperties[i].Value;
            aTempOutputPosition >>= aOutputPosition;
        }
        else if (aSortProperties[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_USERLISTINDEX)))
        {
            uno::Any aUserListIndex = aSortProperties[i].Value;
            aUserListIndex >>= nUserListIndex;
        }
        else if (aSortProperties[i].Name == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_SORTFIELDS)))
        {
            uno::Any aTempSortFields = aSortProperties[i].Value;
            aTempSortFields >>= aSortFields;
        }
    }
    sal_Int32 nSortFields = aSortFields.getLength();
    if (nSortFields > 0)
    {
        if (!bBindFormatsToContent)
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_bind_styles_to_content, sXML_false);
        if (bCopyOutputData)
        {
            ScAddress aTempCellAddress(aOutputPosition.Column, aOutputPosition.Row, aOutputPosition.Sheet);
            String sCellAddress;
            aTempCellAddress.Format(sCellAddress, SCA_VALID | SCA_TAB_3D, pDoc);
            rtl::OUString sOUCellAddress(sCellAddress);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_target_range_address, sOUCellAddress);
        }
        if (bIsCaseSensitive)
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_case_sensitive, sXML_true);
        SvXMLElementExport aElemS(*this, XML_NAMESPACE_TABLE, sXML_sort, sal_True, sal_True);
        CheckAttrList();
        for (i = 0; i < nSortFields; i++)
        {
            AddAttribute(XML_NAMESPACE_TABLE, sXML_field_number, rtl::OUString::valueOf(aSortFields[i].Field));
            if (!aSortFields[i].SortAscending)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_order, sXML_descending);
            if (!bIsUserListEnabled)
            {
                switch (aSortFields[i].FieldType)
                {
                    case util::SortFieldType_ALPHANUMERIC :
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_data_type, sXML_text);
                    break;
                    case util::SortFieldType_AUTOMATIC :
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_data_type, sXML_automatic);
                    break;
                    case util::SortFieldType_NUMERIC :
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_data_type, sXML_number);
                    break;
                }
            }
            else
                AddAttribute(XML_NAMESPACE_TABLE, sXML_data_type, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_USERLIST)) + rtl::OUString::valueOf(nUserListIndex));
            SvXMLElementExport aElemSb(*this, XML_NAMESPACE_TABLE, sXML_sort_by, sal_True, sal_True);
            CheckAttrList();
        }
    }
}

void ScXMLExport::WriteSubTotalDescriptor(const com::sun::star::uno::Reference <com::sun::star::sheet::XSubTotalDescriptor> xSubTotalDescriptor, const rtl::OUString sDatabaseRangeName)
{
    uno::Reference <container::XIndexAccess> xIndexAccess (xSubTotalDescriptor, uno::UNO_QUERY);
    if (xIndexAccess.is())
    {
        sal_Int32 nSubTotalFields = xIndexAccess->getCount();
        if (nSubTotalFields > 0)
        {
            uno::Reference <beans::XPropertySet> xPropertySet (xSubTotalDescriptor, uno::UNO_QUERY);
            sal_Bool bEnableUserSortList = sal_False;
            sal_Bool bSortAscending = sal_True;
            sal_Int32 nUserSortListIndex = 0;
            if (xPropertySet.is())
            {
                sal_Bool bBindFormatsToContent;
                uno::Any aBindFormatsToContent = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_BINDFORMATSTOCONTENT)));
                if (aBindFormatsToContent >>= bBindFormatsToContent)
                    if (!bBindFormatsToContent)
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_bind_styles_to_content, sXML_false);
                sal_Bool bInsertPageBreaks;
                uno::Any aInsertPageBreaks = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_INSERTPAGEBREAKS)));
                if (aInsertPageBreaks >>= bInsertPageBreaks)
                    if (bInsertPageBreaks)
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_page_breaks_on_group_change, sXML_true);
                sal_Bool bIsCaseSensitive;
                uno::Any aIsCaseSensitive = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ISCASESENSITIVE)));
                if (aIsCaseSensitive >>= bIsCaseSensitive)
                    if (bIsCaseSensitive)
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_case_sensitive, sXML_true);
                uno::Any aSortAscending = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_SORTASCENDING)));
                aSortAscending >>= bSortAscending;
                uno::Any aEnabledUserSortList = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ENABLEUSERSORTLIST)));
                if (aEnabledUserSortList >>= bEnableUserSortList)
                    if (bEnableUserSortList)
                    {
                        uno::Any aUserSortListIndex = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_USERSORTLISTINDEX)));
                        aUserSortListIndex >>= nUserSortListIndex;
                    }
            }
            SvXMLElementExport aElemSTRs(*this, XML_NAMESPACE_TABLE, sXML_subtotal_rules, sal_True, sal_True);
            CheckAttrList();
            {
                ScDBCollection* pDBCollection = pDoc->GetDBCollection();
                sal_uInt16 nIndex;
                pDBCollection->SearchName(sDatabaseRangeName, nIndex);
                ScDBData* pDBData = (*pDBCollection)[nIndex];
                ScSubTotalParam aSubTotalParam;
                pDBData->GetSubTotalParam(aSubTotalParam);
                if (aSubTotalParam.bDoSort)
                {
                    if (!aSubTotalParam.bAscending)
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_order, sXML_descending);
                    if (aSubTotalParam.bUserDef)
                    {
                        rtl::OUString sUserList = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_USERLIST));
                        sUserList += rtl::OUString::valueOf(aSubTotalParam.nUserIndex);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_data_type, sUserList);
                    }
                    SvXMLElementExport aElemSGs(*this, XML_NAMESPACE_TABLE, sXML_sort_groups, sal_True, sal_True);
                    CheckAttrList();
                }
            }
            for (sal_Int32 i = 0; i < nSubTotalFields; i++)
            {
                uno::Reference <sheet::XSubTotalField> xSubTotalField;
                uno::Any aSubTotalField = xIndexAccess->getByIndex(i);
                if (aSubTotalField >>= xSubTotalField)
                {
                    sal_Int32 nGroupColumn = xSubTotalField->getGroupColumn();
                    AddAttribute(XML_NAMESPACE_TABLE, sXML_group_by_field_number, rtl::OUString::valueOf(nGroupColumn));
                    SvXMLElementExport aElemSTR(*this, XML_NAMESPACE_TABLE, sXML_subtotal_rule, sal_True, sal_True);
                    CheckAttrList();
                    uno::Sequence <sheet::SubTotalColumn> aSubTotalColumns = xSubTotalField->getSubTotalColumns();
                    sal_Int32 nSubTotalColumns = aSubTotalColumns.getLength();
                    for (sal_Int32 j = 0; j < nSubTotalColumns; j++)
                    {
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_field_number, rtl::OUString::valueOf(aSubTotalColumns[j].Column));
                        rtl::OUString sFunction;
                        GetStringOfFunction(aSubTotalColumns[j].Function, sFunction);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_function, sFunction);
                        SvXMLElementExport aElemSTF(*this, XML_NAMESPACE_TABLE, sXML_subtotal_field, sal_True, sal_True);
                        CheckAttrList();
                    }
                }
            }
        }
    }
}

void ScXMLExport::WriteDatabaseRanges(const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheetDocument>& xSpreadDoc)
{
    uno::Reference <beans::XPropertySet> xPropertySet (xSpreadDoc, uno::UNO_QUERY);
    if (xPropertySet.is())
    {
        uno::Any aDatabaseRanges = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_DATABASERANGES)));
        uno::Reference <sheet::XDatabaseRanges> xDatabaseRanges;
        CheckAttrList();
        if (aDatabaseRanges >>= xDatabaseRanges)
        {
            uno::Sequence <rtl::OUString> aRanges = xDatabaseRanges->getElementNames();
            sal_Int32 nDatabaseRangesCount = aRanges.getLength();
            if (nDatabaseRangesCount > 0)
            {
                SvXMLElementExport aElemDRs(*this, XML_NAMESPACE_TABLE, sXML_database_ranges, sal_True, sal_True);
                for (sal_Int32 i = 0; i < nDatabaseRangesCount; i++)
                {
                    rtl::OUString sDatabaseRangeName = aRanges[i];
                    uno::Any aDatabaseRange = xDatabaseRanges->getByName(sDatabaseRangeName);
                    uno::Reference <sheet::XDatabaseRange> xDatabaseRange;
                    if (aDatabaseRange >>= xDatabaseRange)
                    {
                        String sUnbenannt = ScGlobal::GetRscString(STR_DB_NONAME);
                        rtl::OUString sOUUnbenannt (sUnbenannt);
                        if (sOUUnbenannt != sDatabaseRangeName)
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_name, sDatabaseRangeName);
                        table::CellRangeAddress aRangeAddress = xDatabaseRange->getDataArea();
                        rtl::OUString sOUAddress;
                        GetStringFromRange(aRangeAddress, sOUAddress);
                        AddAttribute (XML_NAMESPACE_TABLE, sXML_target_range_address, sOUAddress);
                        ScDBCollection* pDBCollection = pDoc->GetDBCollection();
                        sal_uInt16 nIndex;
                        pDBCollection->SearchName(sDatabaseRangeName, nIndex);
                        ScDBData* pDBData = (*pDBCollection)[nIndex];
                        if (pDBData->HasImportSelection())
                            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_is_selection, sXML_true);
                        if (pDBData->HasAutoFilter())
                            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_display_filter_buttons, sXML_true);
                        uno::Reference <beans::XPropertySet> xPropertySetDatabaseRange (xDatabaseRange, uno::UNO_QUERY);
                        if (xPropertySetDatabaseRange.is())
                        {
                            uno::Any aKeepFormatsProperty = xPropertySetDatabaseRange->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_KEEPFORMATS)));
                            sal_Bool bKeepFormats = sal_False;
                            if (aKeepFormatsProperty >>= bKeepFormats)
                                if (bKeepFormats)
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_on_update_keep_styles, sXML_true);
                            uno::Any aMoveCellsProperty = xPropertySetDatabaseRange->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_MOVECELLS)));
                            sal_Bool bMoveCells = sal_False;
                            if (aMoveCellsProperty >>= bMoveCells)
                                if (bMoveCells)
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_on_update_keep_size, sXML_false);
                            uno::Any aStripDataProperty = xPropertySetDatabaseRange->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_STRIPDATA)));
                            sal_Bool bStripData = sal_False;
                            if (aStripDataProperty >>= bStripData)
                                if (bStripData)
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_has_persistent_data, sXML_false);
                        }
                        uno::Reference <sheet::XSheetFilterDescriptor> xSheetFilterDescriptor = xDatabaseRange->getFilterDescriptor();
                        if (xSheetFilterDescriptor.is())
                        {
                            uno::Reference <beans::XPropertySet> xFilterProperties (xSheetFilterDescriptor, uno::UNO_QUERY);
                            if (xFilterProperties.is())
                            {
                                uno::Any aContainsHeaderProperty = xFilterProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_CONTAINSHEADER)));
                                sal_Bool bContainsHeader = sal_True;
                                if (aContainsHeaderProperty >>= bContainsHeader)
                                    if (!bContainsHeader)
                                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_contains_header, sXML_false);
                                uno::Any aOrientationProperty = xFilterProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ORIENTATION)));
                                sal_Bool bOrientation = sal_False;
                                if (aOrientationProperty >>= bOrientation)
                                    if (bOrientation)
                                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_orientation, sXML_column);
                            }
                        }
                        SvXMLElementExport aElemDR(*this, XML_NAMESPACE_TABLE, sXML_database_range, sal_True, sal_True);
                        CheckAttrList();
                        WriteImportDescriptor(xDatabaseRange->getImportDescriptor());
                        if (xSheetFilterDescriptor.is())
                            WriteFilterDescriptor(xSheetFilterDescriptor, sDatabaseRangeName);
                        WriteSortDescriptor(xDatabaseRange->getSortDescriptor());
                        WriteSubTotalDescriptor(xDatabaseRange->getSubTotalDescriptor(), sDatabaseRangeName);
                    }
                }
            }
        }
    }

}

rtl::OUString ScXMLExport::getDPOperatorXML(const ScQueryOp aFilterOperator, const sal_Bool bUseRegularExpressions,
    const sal_Bool bIsString, const double dVal) const
{
    if (bUseRegularExpressions)
    {
        switch (aFilterOperator)
        {
            case SC_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_match));
                break;
            case SC_NOT_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_nomatch));
                break;
        }
    }
    else
    {
        switch (aFilterOperator)
        {
            case SC_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("="));
                break;
            case SC_NOT_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("!="));
                break;
            case SC_BOTPERC :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_bottom_percent));
                break;
            case SC_BOTVAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_bottom_values));
                break;
            case SC_GREATER :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(">"));
                break;
            case SC_GREATER_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(">="));
                break;
            case SC_LESS :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("<"));
                break;
            case SC_LESS_EQUAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("<="));
                break;
            case SC_TOPPERC :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_top_percent));
                break;
            case SC_TOPVAL :
                return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_top_values));
                break;
            default:
            {
                if (bIsString)
                {
                    if (dVal == SC_EMPTYFIELDS)
                        return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_empty));
                    else if (dVal == SC_NONEMPTYFIELDS)
                        return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_noempty));
                }
            }
        }
    }
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("="));
}

void ScXMLExport::WriteDPCondition(const ScQueryEntry& aQueryEntry, sal_Bool bIsCaseSensitive, sal_Bool bUseRegularExpressions)
{
    AddAttribute(XML_NAMESPACE_TABLE, sXML_field_number, rtl::OUString::valueOf(aQueryEntry.nField));
    if (bIsCaseSensitive)
        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_case_sensitive, sXML_true);
    if (aQueryEntry.bQueryByString)
    {
        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_data_type, sXML_number);
        rtl::OUStringBuffer sBuffer;
        GetMM100UnitConverter().convertNumber(sBuffer, aQueryEntry.nVal);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_value, sBuffer.makeStringAndClear());
    }
    else
        AddAttribute(XML_NAMESPACE_TABLE, sXML_value, rtl::OUString(*aQueryEntry.pStr));
    AddAttribute(XML_NAMESPACE_TABLE, sXML_operator, getDPOperatorXML(aQueryEntry.eOp, bUseRegularExpressions,
        aQueryEntry.bQueryByString, aQueryEntry.nVal));
    SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_filter_condition, sal_True, sal_True);
}

void ScXMLExport::WriteDPFilter(const ScQueryParam& aQueryParam)
{
    sal_Int16 nQueryEntryCount = aQueryParam.GetEntryCount();
    if (nQueryEntryCount > 0)
    {
        sal_Bool bAnd(sal_False);
        sal_Bool bOr(sal_False);
        sal_Bool bHasEntries(sal_True);
        sal_Int16 nEntries(0);
        for (sal_Int32 j = 1; (j < nQueryEntryCount) && bHasEntries; j++)
        {
            ScQueryEntry aEntry = aQueryParam.GetEntry(j);
            if (aEntry.bDoQuery)
            {
                nEntries++;
                if (aEntry.eConnect == SC_AND)
                    bAnd = sal_True;
                else
                    bOr = sal_True;
            }
            else
                bHasEntries = sal_False;
        }
        nQueryEntryCount = nEntries;
        if (nQueryEntryCount)
        {
            if (!aQueryParam.bInplace)
            {
                ScAddress aTargetAddress(aQueryParam.nDestCol, aQueryParam.nDestRow, aQueryParam.nDestTab);
                String sTargetAddress;
                aTargetAddress.Format(sTargetAddress, SCA_VALID | SCA_TAB_3D, pDoc);
                AddAttribute(XML_NAMESPACE_TABLE, sXML_target_range_address, rtl::OUString(sTargetAddress));
            }
            if(!((aQueryParam.nCol1 == aQueryParam.nCol2) && (aQueryParam.nRow1 == aQueryParam.nRow2) && (aQueryParam.nCol1 == aQueryParam.nRow1)
                && (aQueryParam.nCol1 == 0) && (aQueryParam.nTab == USHRT_MAX)))
            {
                ScRange aConditionRange(aQueryParam.nCol1, aQueryParam.nRow1, aQueryParam.nTab,
                    aQueryParam.nCol2, aQueryParam.nRow2, aQueryParam.nTab);
                rtl::OUString sConditionRange;
                GetStringFromRange(aConditionRange, sConditionRange);
                AddAttribute(XML_NAMESPACE_TABLE, sXML_condition_source_range_address, sConditionRange);
            }
            if (!aQueryParam.bDuplicate)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_display_duplicates, sXML_false);
            SvXMLElementExport aElemDPF(*this, XML_NAMESPACE_TABLE, sXML_filter, sal_True, sal_True);
            CheckAttrList();
            if (bOr && !bAnd)
            {
                SvXMLElementExport aElemOr(*this, XML_NAMESPACE_TABLE, sXML_filter_or, sal_True, sal_True);
                for (j = 0; j < nQueryEntryCount; j++)
                {
                    WriteDPCondition(aQueryParam.GetEntry(j), aQueryParam.bCaseSens, aQueryParam.bRegExp);
                }
            }
            else if (bAnd && !bOr)
            {
                SvXMLElementExport aElemAnd(*this, XML_NAMESPACE_TABLE, sXML_filter_and, sal_True, sal_True);
                for (j = 0; j < nQueryEntryCount; j++)
                {
                    WriteDPCondition(aQueryParam.GetEntry(j), aQueryParam.bCaseSens, aQueryParam.bRegExp);
                }
            }
            else if (nQueryEntryCount  == 1)
            {
                    WriteDPCondition(aQueryParam.GetEntry(0), aQueryParam.bCaseSens, aQueryParam.bRegExp);
            }
            else
            {
                SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_filter_or, sal_True, sal_True);
                ScQueryEntry aPrevFilterField = aQueryParam.GetEntry(0);
                ScQueryConnect aConnection = aQueryParam.GetEntry(1).eConnect;
                sal_Bool bOpenAndElement;
                rtl::OUString aName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_filter_and)));
                if (aConnection == SC_AND)
                {
                    GetDocHandler()->ignorableWhitespace(sWS);
                    GetDocHandler()->startElement( aName, GetXAttrList());
                    ClearAttrList();
                    bOpenAndElement = sal_True;
                }
                else
                    bOpenAndElement = sal_False;
                for (j = 1; j < nQueryEntryCount; j++)
                {
                    if (aConnection != aQueryParam.GetEntry(j).eConnect)
                    {
                        aConnection = aQueryParam.GetEntry(j).eConnect;
                        if (aQueryParam.GetEntry(j).eConnect == SC_AND)
                        {
                            GetDocHandler()->ignorableWhitespace(sWS);
                            GetDocHandler()->startElement( aName, GetXAttrList());
                            ClearAttrList();
                            bOpenAndElement = sal_True;
                            WriteDPCondition(aPrevFilterField, aQueryParam.bCaseSens, aQueryParam.bRegExp);
                            aPrevFilterField = aQueryParam.GetEntry(j);
                            if (j == nQueryEntryCount - 1)
                            {
                                WriteDPCondition(aPrevFilterField, aQueryParam.bCaseSens, aQueryParam.bRegExp);
                                GetDocHandler()->ignorableWhitespace(sWS);
                                GetDocHandler()->endElement(aName);
                                bOpenAndElement = sal_False;
                            }
                        }
                        else
                        {
                            WriteDPCondition(aPrevFilterField, aQueryParam.bCaseSens, aQueryParam.bRegExp);
                            aPrevFilterField = aQueryParam.GetEntry(j);
                            if (bOpenAndElement)
                            {
                                GetDocHandler()->ignorableWhitespace(sWS);
                                GetDocHandler()->endElement(aName);
                                bOpenAndElement = sal_False;
                            }
                            if (j == nQueryEntryCount - 1)
                            {
                                WriteDPCondition(aPrevFilterField, aQueryParam.bCaseSens, aQueryParam.bRegExp);
                            }
                        }
                    }
                    else
                    {
                        WriteDPCondition(aPrevFilterField, aQueryParam.bCaseSens, aQueryParam.bRegExp);
                        aPrevFilterField = aQueryParam.GetEntry(j);
                        if (j == nQueryEntryCount - 1)
                            WriteDPCondition(aPrevFilterField, aQueryParam.bCaseSens, aQueryParam.bRegExp);
                    }
                }
            }
        }
    }
}

void ScXMLExport::WriteDataPilots(const uno::Reference <sheet::XSpreadsheetDocument>& xSpreadDoc)
{
    if (pDoc)
    {
        ScDPCollection* pDPs = pDoc->GetDPCollection();
        if (pDPs)
        {
            sal_Int16 nDPCount = pDPs->GetCount();
            if (nDPCount > 0)
            {
                SvXMLElementExport aElemDPs(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_tables, sal_True, sal_True);
                CheckAttrList();
                for (sal_Int16 i = 0; i < nDPCount; i++)
                {
                    ScDPSaveData* pDPSave = (*pDPs)[i]->GetSaveData();
                    if (pDPSave)
                    {
                        ScRange aOutRange = (*pDPs)[i]->GetOutRange();
                        rtl::OUString sTargetRangeAddress;
                        GetStringFromRange(aOutRange, sTargetRangeAddress);
                        ScDocAttrIterator aAttrItr(pDoc, aOutRange.aStart.Tab(),
                            aOutRange.aStart.Col(), aOutRange.aStart.Row(),
                            aOutRange.aEnd.Col(), aOutRange.aEnd.Row());
                        sal_uInt16 nCol, nRow1, nRow2;
                        String sButtonList;
                        const ScPatternAttr* pAttr = aAttrItr.GetNext(nCol, nRow1, nRow2);
                        while (pAttr)
                        {
                            ScMergeFlagAttr& rItem = (ScMergeFlagAttr&)pAttr->GetItem(ATTR_MERGE_FLAG);
                            if (rItem.HasButton())
                            {
                                for (sal_Int32 nButtonRow = nRow1; nButtonRow <= nRow2; nButtonRow++)
                                {
                                    ScAddress aButtonAddr(nCol, nButtonRow, aOutRange.aStart.Tab());
                                    String sButtonAddr;
                                    aButtonAddr.Format(sButtonAddr, SCA_VALID | SCA_TAB_3D, pDoc);
                                    if (sButtonList.Len())
                                        sButtonList += ' ';
                                    sButtonList += sButtonAddr;
                                }
                            }
                            pAttr = aAttrItr.GetNext(nCol, nRow1, nRow2);
                        }
                        rtl::OUString sOUButtonList(sButtonList);
                        rtl::OUString sName((*pDPs)[i]->GetName());
                        rtl::OUString sApplicationData((*pDPs)[i]->GetTag());
                        sal_Bool bRowGrand = pDPSave->GetRowGrand();
                        sal_Bool bColumnGrand = pDPSave->GetColumnGrand();
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_name, sName);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_application_data, sApplicationData);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_target_range_address, sTargetRangeAddress);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_buttons, sOUButtonList);
                        if (!(bRowGrand && bColumnGrand))
                        {
                            if (bRowGrand)
                                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_grand_total, sXML_row);
                            else if (bColumnGrand)
                                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_grand_total, sXML_column);
                            else
                                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_grand_total, sXML_none);
                        }
                        if (pDPSave->GetIgnoreEmptyRows())
                            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_ignore_empty_rows, sXML_true);
                        if (pDPSave->GetRepeatIfEmpty())
                            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_identify_categories, sXML_true);
                        SvXMLElementExport aElemDP(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_table, sal_True, sal_True);
                        CheckAttrList();
                        if ((*pDPs)[i]->IsSheetData())
                        {
                            const ScSheetSourceDesc* pSheetSource = (*pDPs)[i]->GetSheetDesc();
                            rtl::OUString sCellRangeAddress;
                            GetStringFromRange(pSheetSource->aSourceRange, sCellRangeAddress);
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_cell_range_address, sCellRangeAddress);
                            SvXMLElementExport aElemSCR(*this, XML_NAMESPACE_TABLE, sXML_source_cell_range, sal_True, sal_True);
                            CheckAttrList();
                            WriteDPFilter(pSheetSource->aQueryParam);
                        }
                        else if ((*pDPs)[i]->IsImportData())
                        {
                            const ScImportSourceDesc* pImpSource = (*pDPs)[i]->GetImportSourceDesc();
                            switch (pImpSource->nType)
                            {
                                case sheet::DataImportMode_NONE : break;
                                case sheet::DataImportMode_QUERY :
                                {
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_database_name, rtl::OUString(pImpSource->aDBName));
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_query_name, rtl::OUString(pImpSource->aObject));
                                    SvXMLElementExport aElemID(*this, XML_NAMESPACE_TABLE, sXML_database_source_query, sal_True, sal_True);
                                    CheckAttrList();
                                }
                                break;
                                case sheet::DataImportMode_TABLE :
                                {
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_database_name, rtl::OUString(pImpSource->aDBName));
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_table_name, rtl::OUString(pImpSource->aObject));
                                    SvXMLElementExport aElemID(*this, XML_NAMESPACE_TABLE, sXML_database_source_table, sal_True, sal_True);
                                    CheckAttrList();
                                }
                                break;
                                case sheet::DataImportMode_SQL :
                                {
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_database_name, rtl::OUString(pImpSource->aDBName));
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_sql_statement, rtl::OUString(pImpSource->aObject));
                                    if (!pImpSource->bNative)
                                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_parse_sql_statement, sXML_true);
                                    SvXMLElementExport aElemID(*this, XML_NAMESPACE_TABLE, sXML_database_source_sql, sal_True, sal_True);
                                    CheckAttrList();
                                }
                                break;
                            }
                        }
                        else if ((*pDPs)[i]->IsServiceData())
                        {
                            const ScDPServiceDesc* pServSource = (*pDPs)[i]->GetDPServiceDesc();
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_name, rtl::OUString(pServSource->aServiceName));
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_source_name, rtl::OUString(pServSource->aParSource));
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_object_name, rtl::OUString(pServSource->aParName));
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_username, rtl::OUString(pServSource->aParUser));
                            // How to write the Passwort? We must know, whether the passwort shoulb be written encrypted and how or not
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_passwort, rtl::OUString(pServSource->aParPass));
                            SvXMLElementExport aElemSD(*this, XML_NAMESPACE_TABLE, sXML_source_service, sal_True, sal_True);
                            CheckAttrList();
                        }
                        List aDimensions = pDPSave->GetDimensions();
                        sal_Int32 nDimCount = aDimensions.Count();
                        for (sal_Int32 nDim = 0; nDim < nDimCount; nDim++)
                        {
                            ScDPSaveDimension* pDim = (ScDPSaveDimension*)aDimensions.GetObject(nDim);
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_source_field_name, rtl::OUString(pDim->GetName()));
                            if (pDim->IsDataLayout())
                                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_is_data_layout_field, sXML_true);
                            switch (pDim->GetOrientation())
                            {
                                case sheet::DataPilotFieldOrientation_COLUMN :
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_orientation, sXML_column); break;
                                case sheet::DataPilotFieldOrientation_DATA :
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_orientation, sXML_data); break;
                                case sheet::DataPilotFieldOrientation_HIDDEN :
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_orientation, sXML_hidden); break;
                                case sheet::DataPilotFieldOrientation_PAGE :
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_orientation, sXML_page); break;
                                case sheet::DataPilotFieldOrientation_ROW :
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_orientation, sXML_row); break;
                            }
                            if (pDim->GetUsedHierarchy() != 1)
                            {
                                rtl::OUStringBuffer sBuffer;
                                SvXMLUnitConverter::convertNumber(sBuffer, pDim->GetUsedHierarchy());
                                AddAttribute(XML_NAMESPACE_TABLE, sXML_used_hierarchy, sBuffer.makeStringAndClear());
                            }
                            rtl::OUString sFunction;
                            GetStringOfFunction(pDim->GetFunction(), sFunction);
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_function, sFunction);
                            SvXMLElementExport aElemDPF(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_field, sal_True, sal_True);
                            CheckAttrList();
                            {
                                rtl::OUStringBuffer sBuffer;
                                SvXMLUnitConverter::convertBool(sBuffer, pDim->GetShowEmpty());
                                AddAttribute(XML_NAMESPACE_TABLE, sXML_display_empty, sBuffer.makeStringAndClear());
                                SvXMLElementExport aElemDPL(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_level, sal_True, sal_True);
                                CheckAttrList();
                                sal_Int32 nSubTotalCount = pDim->GetSubTotalsCount();
                                if (nSubTotalCount > 0)
                                {
                                    SvXMLElementExport aElemSTs(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_subtotals, sal_True, sal_True);
                                    CheckAttrList();
                                    for (sal_Int32 nSubTotal = 0; nSubTotal < nSubTotalCount; nSubTotal++)
                                    {
                                        rtl::OUString sFunction;
                                        GetStringOfFunction(pDim->GetSubTotalFunc(nSubTotal), sFunction);
                                        AddAttribute(XML_NAMESPACE_TABLE, sXML_function, sFunction);
                                        SvXMLElementExport aElemST(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_subtotal, sal_True, sal_True);
                                    }
                                }
                                List aMembers = pDim->GetMembers();
                                sal_Int32 nMemberCount = aMembers.Count();
                                if (nMemberCount > 0)
                                {
                                    SvXMLElementExport aElemDPMs(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_members, sal_True, sal_True);
                                    CheckAttrList();
                                    for (sal_Int32 nMember = 0; nMember < nMemberCount; nMember++)
                                    {
                                        AddAttribute(XML_NAMESPACE_TABLE, sXML_name, rtl::OUString(((ScDPSaveMember*)aMembers.GetObject(nMember))->GetName()));
                                        rtl::OUStringBuffer sBuffer;
                                        SvXMLUnitConverter::convertBool(sBuffer, ((ScDPSaveMember*)aMembers.GetObject(nMember))->GetIsVisible());
                                        AddAttribute(XML_NAMESPACE_TABLE, sXML_display, sBuffer.makeStringAndClear());
                                        SvXMLUnitConverter::convertBool(sBuffer, ((ScDPSaveMember*)aMembers.GetObject(nMember))->GetShowDetails());
                                        AddAttribute(XML_NAMESPACE_TABLE, sXML_display_details, sBuffer.makeStringAndClear());
                                        SvXMLElementExport aElemDPM(*this, XML_NAMESPACE_TABLE, sXML_data_pilot_member, sal_True, sal_True);
                                        CheckAttrList();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SvXMLAutoStylePoolP* ScXMLExport::CreateAutoStylePool()
{
    return new ScXMLAutoStylePoolP(*this);
}
