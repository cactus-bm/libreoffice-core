/*************************************************************************
 *
 *  $RCSfile: XMLStylesExportHelper.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: sab $ $Date: 2000-11-10 17:17:59 $
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

#ifndef _SC_XMLSTYLESEXPORTHELPER_HXX
#define _SC_XMLSTYLESEXPORTHELPER_HXX

#include <vector>

#ifndef _COM_SUN_STAR_TABLE_CELLRANGEADDRESS_HPP_
#include <com/sun/star/table/CellRangeAddress.hpp>
#endif

struct ScMyRowFormatRange
{
    com::sun::star::table::CellRangeAddress aRangeAddress;
    sal_Int32   nStartColumn;
    sal_Int32   nRepeatColumns;
    sal_Int32   nRepeatRows;
    sal_Int32   nIndex;
    sal_Bool    bIsAutoStyle;

    ScMyRowFormatRange();
};

typedef std::vector<ScMyRowFormatRange> ScMyRowFormatRangesVec;

class ScRowFormatRanges
{
    ScMyRowFormatRangesVec      aRowFormatRanges;

public:
    ScRowFormatRanges();
    ~ScRowFormatRanges();

    void Clear();
    void AddRange(const ScMyRowFormatRange& aFormatRange);
    sal_Bool GetNext(ScMyRowFormatRange& aFormatRange);
    sal_Int32 GetMaxRows();
    sal_Int32 GetSize();
    void Sort();
};

typedef std::vector<rtl::OUString*>     ScMyOUStringVec;

struct ScMyFormatRange
{
    com::sun::star::table::CellRangeAddress aRangeAddress;
    sal_Int32                               nStyleNameIndex;
    sal_Bool                                bIsAutoStyle;

    ScMyFormatRange();
};

typedef std::vector<ScMyFormatRange>            ScMyFormatRangeAddresses;
typedef std::vector<ScMyFormatRangeAddresses*>  ScMyFormatRangeVectorVec;

class ScFormatRangeStyles
{
    ScMyFormatRangeVectorVec    aTables;
    ScMyOUStringVec             aStyleNames;
    ScMyOUStringVec             aAutoStyleNames;

public:
    ScFormatRangeStyles();
    ~ScFormatRangeStyles();

    void AddNewTable(const sal_Int16 nTable);
    sal_Int32 AddStyleName(rtl::OUString* pString, const sal_Bool bIsAutoStyle = sal_True);
    sal_Int32 GetIndexOfStyleName(const rtl::OUString& rString, const rtl::OUString& rPrefix, sal_Bool& bIsAutoStyle);
    sal_Int32 GetStyleNameIndex(const sal_Int16 nTable, const sal_Int32 nColumn, const sal_Int32 nRow, sal_Bool& bIsAutoStyle);
    void GetFormatRanges(const sal_Int32 nStartColumn, const sal_Int32 nEndColumn, const sal_Int32 nRow,
                    const sal_Int16 nTable, ScRowFormatRanges& aFormatRanges);
    void AddRangeStyleName(const com::sun::star::table::CellRangeAddress aCellRangeAddress, const sal_Int32 nStringIndex, const sal_Bool bIsAutoStyle);
    rtl::OUString* GetStyleName(const sal_Int16 nTable, const sal_Int32 nColumn, const sal_Int32 nRow);
    rtl::OUString* GetStyleNameByIndex(const sal_Int32 nIndex, const sal_Bool bIsAutoStyle);
    void Sort();
};

typedef std::vector<sal_Int32>          ScMysalInt32Vec;
typedef std::vector<ScMysalInt32Vec>    ScMyVectorVec;

class ScColumnRowStyles
{
    ScMyVectorVec               aTables;
    ScMyOUStringVec             aStyleNames;

public:
    ScColumnRowStyles();
    ~ScColumnRowStyles();

    void AddNewTable(const sal_Int16 nTable, const sal_Int32 nFields);
    sal_Int32 AddStyleName(rtl::OUString* pString);
    sal_Int32 GetIndexOfStyleName(const rtl::OUString& rString, const rtl::OUString& rPrefix);
    sal_Int32 GetStyleNameIndex(const sal_Int16 nTable, const sal_Int32 nField);
    void AddFieldStyleName(const sal_Int16 nTable, const sal_Int32 nField, const sal_Int32 nStringIndex);
    rtl::OUString* GetStyleName(const sal_Int16 nTable, const sal_Int32 nField);
    rtl::OUString* GetStyleNameByIndex(const sal_Int32 nIndex);
};

#endif

