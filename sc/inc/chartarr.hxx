/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: chartarr.hxx,v $
 * $Revision: 1.7 $
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

#ifndef SC_CHARTARR_HXX
#define SC_CHARTARR_HXX

// -----------------------------------------------------------------------

#include "collect.hxx"
#include "rangelst.hxx"
#include "chartpos.hxx"

class ScAddress;
class Table;
class ScDocument;
class ScMultipleReadHeader;


// ScMemChart is a stripped-down SchMemChart from old chart,
// used only to transport a rectangular data array for the UNO API,
// contains only column/row header text and data values.

class ScMemChart
{
    short           nRowCnt;
    short           nColCnt;
    double*         pData;
    String*         pColText;
    String*         pRowText;

    ScMemChart(const ScMemChart& rMemChart);      // not implemented

public:
    ScMemChart(short nCols, short nRows);
    ~ScMemChart();

    short GetColCount() const { return nColCnt; }
    short GetRowCount() const { return nRowCnt; }
    const String& GetColText(short nCol) const { return pColText[nCol]; }
    const String& GetRowText(short nRow) const { return pRowText[nRow]; }
    double GetData(short nCol, short nRow) const { return pData[nCol * nRowCnt + nRow]; }
    void SetData(short nCol, short nRow, const double& rVal) { pData[nCol * nRowCnt + nRow] = rVal; }
    void SetColText(short nCol, const String& rText) { pColText[nCol] = rText; }
    void SetRowText(short nRow, const String& rText) { pRowText[nRow] = rText; }
};


class ScChartArray : public DataObject              // nur noch Parameter-Struct
{
    String      aName;
    ScDocument* pDocument;
    ScChartPositioner aPositioner;
    BOOL        bValid;             // fuer Erzeugung aus SchMemChart

private:
    ScMemChart* CreateMemChartSingle();
    ScMemChart* CreateMemChartMulti();
public:
    ScChartArray( ScDocument* pDoc, SCTAB nTab,
                    SCCOL nStartColP, SCROW nStartRowP,
                    SCCOL nEndColP, SCROW nEndRowP,
                    const String& rChartName );
    ScChartArray( ScDocument* pDoc, const ScRangeListRef& rRangeList,
                    const String& rChartName );
    ScChartArray( const ScChartArray& rArr );
    ScChartArray( ScDocument* pDoc, SvStream& rStream, ScMultipleReadHeader& rHdr );

    virtual ~ScChartArray();
    virtual DataObject* Clone() const;

    const ScRangeListRef&   GetRangeList() const { return aPositioner.GetRangeList(); }
    void    SetRangeList( const ScRangeListRef& rNew ) { aPositioner.SetRangeList(rNew); }
    void    SetRangeList( const ScRange& rNew ) { aPositioner.SetRangeList(rNew); }
    void    AddToRangeList( const ScRange& rRange ) { aPositioner.AddToRangeList(rRange); }
    void    AddToRangeList( const ScRangeListRef& rAdd ) { aPositioner.AddToRangeList(rAdd); }
    const   ScChartPositionMap* GetPositionMap() { return aPositioner.GetPositionMap(); }

    void    SetHeaders(BOOL bCol, BOOL bRow) { aPositioner.SetHeaders(bCol, bRow); }
    BOOL    HasColHeaders() const            { return aPositioner.HasColHeaders(); }
    BOOL    HasRowHeaders() const            { return aPositioner.HasRowHeaders(); }
    BOOL    IsValid() const                  { return bValid; }
    void    SetName(const String& rNew)      { aName = rNew; }
    const String& GetName() const            { return aName; }

    BOOL    IsAtCursor(const ScAddress& rPos) const { return aPositioner.IsAtCursor(rPos); }

    BOOL    operator==(const ScChartArray& rCmp) const;

    ScMemChart* CreateMemChart();
};

class ScChartCollection : public Collection
{
public:
    ScChartCollection() : Collection( 4,4 ) {}
    ScChartCollection( const ScChartCollection& rColl ):
            Collection( rColl ) {}

    virtual DataObject* Clone() const;
    ScChartArray*       operator[](USHORT nIndex) const
                        { return (ScChartArray*)At(nIndex); }

    BOOL    operator==(const ScChartCollection& rCmp) const;

    BOOL    Load( ScDocument* pDoc, SvStream& rStream );
};



#endif

