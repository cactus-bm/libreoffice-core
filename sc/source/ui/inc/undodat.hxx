/*************************************************************************
 *
 *  $RCSfile: undodat.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2004-07-23 10:53:18 $
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

#ifndef SC_UNDODAT_HXX
#define SC_UNDODAT_HXX

#ifndef SC_UNDOBASE_HXX
#include "undobase.hxx"
#endif

#ifndef SC_RANGEUTL_HXX
#include "rangeutl.hxx"     // ScArea
#endif

#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"     // ScRangeListRef
#endif

#ifndef SC_MARKDATA_HXX
#include "markdata.hxx"
#endif

#ifndef SC_SORTPARAM_HXX
#include "sortparam.hxx"
#endif

#ifndef SC_PIVOT_HXX
#include "pivot.hxx"
#endif

class ScDocShell;
class ScDocument;
class ScOutlineTable;
class ScRangeName;
class ScDBData;
class ScDBCollection;
class ScDPObject;
class SdrUndoAction;

//----------------------------------------------------------------------------

class ScUndoDoOutline: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoDoOutline( ScDocShell* pNewDocShell,
                            SCCOLROW nNewStart, SCCOLROW nNewEnd, SCTAB nNewTab,
                            ScDocument* pNewUndoDoc, BOOL bNewColumns,
                            USHORT nNewLevel, USHORT nNewEntry, BOOL bNewShow );
    virtual         ~ScUndoDoOutline();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    SCCOLROW        nStart;
    SCCOLROW        nEnd;
    SCTAB           nTab;
    ScDocument*     pUndoDoc;
    BOOL            bColumns;
    USHORT          nLevel;
    USHORT          nEntry;
    BOOL            bShow;
};


class ScUndoMakeOutline: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoMakeOutline( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScOutlineTable* pNewUndoTab,
                            BOOL bNewColumns, BOOL bNewMake );
    virtual         ~ScUndoMakeOutline();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScOutlineTable* pUndoTable;
    BOOL            bColumns;
    BOOL            bMake;
};


class ScUndoOutlineLevel: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoOutlineLevel( ScDocShell* pNewDocShell,
                            SCCOLROW nNewStart, SCCOLROW nNewEnd, SCTAB nNewTab,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                            BOOL bNewColumns, USHORT nNewLevel );
    virtual         ~ScUndoOutlineLevel();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    SCCOLROW        nStart;
    SCCOLROW        nEnd;
    SCTAB           nTab;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
    BOOL            bColumns;
    USHORT          nLevel;
};


class ScUndoOutlineBlock: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoOutlineBlock( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                            BOOL bNewShow );
    virtual         ~ScUndoOutlineBlock();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
    BOOL            bShow;
};


class ScUndoRemoveAllOutlines: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoRemoveAllOutlines( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab );
    virtual         ~ScUndoRemoveAllOutlines();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
};


class ScUndoAutoOutline: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoAutoOutline( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab );
    virtual         ~ScUndoAutoOutline();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
};


class ScUndoSubTotals: public ScDBFuncUndo
{
public:
                    TYPEINFO();
                    ScUndoSubTotals( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScSubTotalParam& rNewParam, SCROW nNewEndY,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
//                          ScDBData* pNewData,
                            ScRangeName* pNewUndoRange, ScDBCollection* pNewUndoDB );
    virtual         ~ScUndoSubTotals();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    SCTAB           nTab;
    ScSubTotalParam aParam;                         // Original uebergebener Parameter
    SCROW           nNewEndRow;                     // Ergebnis-Groesse
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
//  ScDBData*       pUndoDBData;
    ScRangeName*    pUndoRange;
    ScDBCollection* pUndoDB;
};


class ScUndoSort: public ScDBFuncUndo
{
public:
                    TYPEINFO();
                    ScUndoSort( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScSortParam& rParam,
                            BOOL bQuery, ScDocument* pNewUndoDoc,
                            ScDBCollection* pNewUndoDB, const ScRange* pDest = NULL );
    virtual         ~ScUndoSort();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    SCTAB           nTab;
    ScSortParam     aSortParam;
    BOOL            bRepeatQuery;
    ScDocument*     pUndoDoc;
    ScDBCollection* pUndoDB;                // wegen Quell- und Zielbereich
    BOOL            bDestArea;
    ScRange         aDestRange;
};


class ScUndoQuery: public ScDBFuncUndo
{
public:
                    TYPEINFO();
                    ScUndoQuery( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScQueryParam& rParam, ScDocument* pNewUndoDoc,
                            ScDBCollection* pNewUndoDB, const ScRange* pOld,
                            BOOL bSize, const ScRange* pAdvSrc );
    virtual         ~ScUndoQuery();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    SdrUndoAction*  pDrawUndo;
    SCTAB           nTab;
    ScQueryParam    aQueryParam;
    ScDocument*     pUndoDoc;
    ScDBCollection* pUndoDB;                // wegen Quell- und Zielbereich
    ScRange         aOldDest;
    ScRange         aAdvSource;
    BOOL            bIsAdvanced;
    BOOL            bDestArea;
    BOOL            bDoSize;
};


class ScUndoAutoFilter: public ScDBFuncUndo
{
private:
    String          aDBName;
    BOOL            bFilterSet;

    void            DoChange( BOOL bUndo );

public:
                    TYPEINFO();
                    ScUndoAutoFilter( ScDocShell* pNewDocShell, const ScRange& rRange,
                            const String& rName, BOOL bSet );
    virtual         ~ScUndoAutoFilter();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;
};


class ScUndoDBData: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoDBData( ScDocShell* pNewDocShell,
                            ScDBCollection* pNewUndoColl, ScDBCollection* pNewRedoColl );
    virtual         ~ScUndoDBData();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScDBCollection* pUndoColl;
    ScDBCollection* pRedoColl;
};


class ScUndoImportData: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoImportData( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            const ScImportParam& rParam, SCCOL nNewEndX, SCROW nNewEndY,
                            SCCOL nNewFormula,
                            ScDocument* pNewUndoDoc, ScDocument* pNewRedoDoc,
                            ScDBData* pNewUndoData, ScDBData* pNewRedoData );
    virtual         ~ScUndoImportData();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    SCTAB           nTab;
    ScImportParam   aImportParam;
    SCCOL           nEndCol;
    SCROW           nEndRow;
    ScDocument*     pUndoDoc;
    ScDocument*     pRedoDoc;
    ScDBData*       pUndoDBData;
    ScDBData*       pRedoDBData;
    SCCOL           nFormulaCols;
    BOOL            bRedoFilled;
};


class ScUndoRepeatDB: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoRepeatDB( ScDocShell* pNewDocShell, SCTAB nNewTab,
                            SCCOL nStartX, SCROW nStartY, SCCOL nEndX, SCROW nEndY,
                            SCROW nResultEndRow, SCCOL nCurX, SCROW nCurY,
                            ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                            ScRangeName* pNewUndoRange, ScDBCollection* pNewUndoDB,
                            const ScRange* pOldQ, const ScRange* pNewQ );
    virtual         ~ScUndoRepeatDB();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScAddress       aBlockStart;
    ScAddress       aBlockEnd;
    SCROW           nNewEndRow;
    ScAddress       aCursorPos;
    ScDocument*     pUndoDoc;
    ScOutlineTable* pUndoTable;
    ScRangeName*    pUndoRange;
    ScDBCollection* pUndoDB;
    ScRange         aOldQuery;
    ScRange         aNewQuery;
    BOOL            bQuerySize;
};


class ScUndoPivot: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoPivot( ScDocShell* pNewDocShell,
                            const ScArea& rOld, const ScArea& rNew,
                            ScDocument* pOldDoc, ScDocument* pNewDoc,
                            const ScPivot* pOldPivot, const ScPivot* pNewPivot );
    virtual         ~ScUndoPivot();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScArea          aOldArea;
    ScArea          aNewArea;
    ScDocument*     pOldUndoDoc;
    ScDocument*     pNewUndoDoc;
    ScPivotParam    aOldParam;              // fuer Redo
    ScQueryParam    aOldQuery;
    ScArea          aOldSrc;
    ScPivotParam    aNewParam;              // fuer Undo in Collection
    ScQueryParam    aNewQuery;
    ScArea          aNewSrc;
    String          aOldName;
    String          aOldTag;
    String          aNewName;
    String          aNewTag;
};


class ScUndoDataPilot: public ScSimpleUndo
{
public:
                    TYPEINFO();
                    ScUndoDataPilot( ScDocShell* pNewDocShell,
                            ScDocument* pOldDoc, ScDocument* pNewDoc,
                            const ScDPObject* pOldObj, const ScDPObject* pNewObj,
                            BOOL bMove );
    virtual         ~ScUndoDataPilot();

    virtual void    Undo();
    virtual void    Redo();
    virtual void    Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL    CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String  GetComment() const;

private:
    ScDocument*     pOldUndoDoc;
    ScDocument*     pNewUndoDoc;
    ScDPObject*     pOldDPObject;
    ScDPObject*     pNewDPObject;
    BOOL            bAllowMove;
};


class ScUndoConsolidate: public ScSimpleUndo
{
public:
                        TYPEINFO();
                        ScUndoConsolidate( ScDocShell* pNewDocShell,
                                const ScArea& rArea, const ScConsolidateParam& rPar,
                                ScDocument* pNewUndoDoc, BOOL bReference,
                                SCROW nInsCount, ScOutlineTable* pTab,
                                ScDBData* pData );
    virtual             ~ScUndoConsolidate();

    virtual void        Undo();
    virtual void        Redo();
    virtual void        Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL        CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String      GetComment() const;

private:
    ScArea              aDestArea;
    ScDocument*         pUndoDoc;
    ScConsolidateParam  aParam;
    BOOL                bInsRef;
    SCSIZE              nInsertCount;
    ScOutlineTable*     pUndoTab;
    ScDBData*           pUndoData;
};


class ScUndoChartData: public ScSimpleUndo
{
public:
                        TYPEINFO();
                        ScUndoChartData( ScDocShell* pNewDocShell,
                                const String& rName, const ScRange& rNew,
                                BOOL bColHdr, BOOL bRowHdr, BOOL bAdd );
                        ScUndoChartData( ScDocShell* pNewDocShell,
                                const String& rName, const ScRangeListRef& rNew,
                                BOOL bColHdr, BOOL bRowHdr, BOOL bAdd );
    virtual             ~ScUndoChartData();

    virtual void        Undo();
    virtual void        Redo();
    virtual void        Repeat(SfxRepeatTarget& rTarget);
    virtual BOOL        CanRepeat(SfxRepeatTarget& rTarget) const;

    virtual String      GetComment() const;

private:
    String              aChartName;
    ScRangeListRef      aOldRangeListRef;
    BOOL                bOldColHeaders;
    BOOL                bOldRowHeaders;
//  ScRange             aNewRange;
    ScRangeListRef      aNewRangeListRef;
    BOOL                bNewColHeaders;
    BOOL                bNewRowHeaders;
    BOOL                bAddRange;

    void                Init();
};




#endif

