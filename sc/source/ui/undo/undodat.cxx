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

// INCLUDE ---------------------------------------------------------------

#include <sfx2/app.hxx>

#include "undodat.hxx"
#include "undoutil.hxx"
#include "undoolk.hxx"
#include "document.hxx"
#include "docsh.hxx"
#include "tabvwsh.hxx"
#include "olinetab.hxx"
#include "dbcolect.hxx"
#include "rangenam.hxx"
#include "pivot.hxx"
#include "globstr.hrc"
#include "global.hxx"
#include "target.hxx"
#include "chartarr.hxx"
#include "dbdocfun.hxx"
#include "olinefun.hxx"
#include "dpobject.hxx"
#include "attrib.hxx"
#include "hints.hxx"
#include "sc.hrc"
#include "chgtrack.hxx"  // Amelia Wang
#include "refundo.hxx"  // Amelia Wang

// -----------------------------------------------------------------------

TYPEINIT1(ScUndoDoOutline,          ScSimpleUndo);
TYPEINIT1(ScUndoMakeOutline,        ScSimpleUndo);
TYPEINIT1(ScUndoOutlineLevel,       ScSimpleUndo);
TYPEINIT1(ScUndoOutlineBlock,       ScSimpleUndo);
TYPEINIT1(ScUndoRemoveAllOutlines,  ScSimpleUndo);
TYPEINIT1(ScUndoAutoOutline,        ScSimpleUndo);
TYPEINIT1(ScUndoSubTotals,          ScDBFuncUndo);
TYPEINIT1(ScUndoSort,               ScDBFuncUndo);
TYPEINIT1(ScUndoQuery,              ScDBFuncUndo);
TYPEINIT1(ScUndoAutoFilter,         ScDBFuncUndo);
TYPEINIT1(ScUndoDBData,             ScSimpleUndo);
TYPEINIT1(ScUndoImportData,         ScSimpleUndo);
TYPEINIT1(ScUndoRepeatDB,           ScSimpleUndo);
TYPEINIT1(ScUndoDataPilot,          ScSimpleUndo);
TYPEINIT1(ScUndoConsolidate,        ScSimpleUndo);
TYPEINIT1(ScUndoChartData,          ScSimpleUndo);
TYPEINIT1(ScUndoDataForm,           SfxUndoAction);    // amelia

// -----------------------------------------------------------------------


//
//      Outline-Gruppen ein- oder ausblenden
//

ScUndoDoOutline::ScUndoDoOutline( ScDocShell* pNewDocShell,
                            SCCOLROW nNewStart, SCCOLROW nNewEnd, SCTAB nNewTab,
                            ScDocument* pNewUndoDoc, BOOL bNewColumns,
                            USHORT nNewLevel, USHORT nNewEntry, BOOL bNewShow ) :
    ScSimpleUndo( pNewDocShell ),
    nStart( nNewStart ),
    nEnd( nNewEnd ),
    nTab( nNewTab ),
    pUndoDoc( pNewUndoDoc ),
    bColumns( bNewColumns ),
    nLevel( nNewLevel ),
    nEntry( nNewEntry ),
    bShow( bNewShow )
{
}

__EXPORT ScUndoDoOutline::~ScUndoDoOutline()
{
    delete pUndoDoc;
}

String __EXPORT ScUndoDoOutline::GetComment() const
{   // Detail einblenden" "Detail ausblenden"
    return bShow ?
        ScGlobal::GetRscString( STR_UNDO_DOOUTLINE ) :
        ScGlobal::GetRscString( STR_UNDO_REDOOUTLINE );
}

void __EXPORT ScUndoDoOutline::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    //  Tabelle muss vorher umgeschaltet sein (#46952#) !!!

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    //  inverse Funktion ausfuehren

    if (bShow)
        pViewShell->HideOutline( bColumns, nLevel, nEntry, FALSE, FALSE );
    else
        pViewShell->ShowOutline( bColumns, nLevel, nEntry, FALSE, FALSE );

    //  Original Spalten-/Zeilenstatus

    if (bColumns)
        pUndoDoc->CopyToDocument( static_cast<SCCOL>(nStart), 0, nTab,
                static_cast<SCCOL>(nEnd), MAXROW, nTab, IDF_NONE, FALSE, pDoc);
    else
        pUndoDoc->CopyToDocument( 0, nStart, nTab, MAXCOL, nEnd, nTab, IDF_NONE, FALSE, pDoc );

    pViewShell->UpdateScrollBars();

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP);

    EndUndo();
}

void __EXPORT ScUndoDoOutline::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    //  Tabelle muss vorher umgeschaltet sein (#46952#) !!!

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    if (bShow)
        pViewShell->ShowOutline( bColumns, nLevel, nEntry, FALSE );
    else
        pViewShell->HideOutline( bColumns, nLevel, nEntry, FALSE );

    EndRedo();
}

void __EXPORT ScUndoDoOutline::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL __EXPORT ScUndoDoOutline::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;                       // geht nicht
}

//
//      Outline-Gruppen erzeugen oder loeschen
//

ScUndoMakeOutline::ScUndoMakeOutline( ScDocShell* pNewDocShell,
                            SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                            SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                            ScOutlineTable* pNewUndoTab, BOOL bNewColumns, BOOL bNewMake ) :
    ScSimpleUndo( pNewDocShell ),
    aBlockStart( nStartX, nStartY, nStartZ ),
    aBlockEnd( nEndX, nEndY, nEndZ ),
    pUndoTable( pNewUndoTab ),
    bColumns( bNewColumns ),
    bMake( bNewMake )
{
}

__EXPORT ScUndoMakeOutline::~ScUndoMakeOutline()
{
    delete pUndoTable;
}

String __EXPORT ScUndoMakeOutline::GetComment() const
{   // "Gruppierung" "Gruppierung aufheben"
    return bMake ?
        ScGlobal::GetRscString( STR_UNDO_MAKEOUTLINE ) :
        ScGlobal::GetRscString( STR_UNDO_REMAKEOUTLINE );
}

void __EXPORT ScUndoMakeOutline::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    SCTAB nTab = aBlockStart.Tab();

    ScUndoUtil::MarkSimpleBlock( pDocShell, aBlockStart, aBlockEnd );

    pDoc->SetOutlineTable( nTab, pUndoTable );

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP|PAINT_SIZE);

    EndUndo();
}

void __EXPORT ScUndoMakeOutline::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    ScUndoUtil::MarkSimpleBlock( pDocShell, aBlockStart, aBlockEnd );

    if (bMake)
        pViewShell->MakeOutline( bColumns, FALSE );
    else
        pViewShell->RemoveOutline( bColumns, FALSE );

    pDocShell->PostPaint(0,0,aBlockStart.Tab(),MAXCOL,MAXROW,aBlockEnd.Tab(),PAINT_GRID);

    EndRedo();
}

void __EXPORT ScUndoMakeOutline::Repeat(SfxRepeatTarget& rTarget)
{
    if (rTarget.ISA(ScTabViewTarget))
    {
        ScTabViewShell& rViewShell = *((ScTabViewTarget&)rTarget).GetViewShell();

        if (bMake)
            rViewShell.MakeOutline( bColumns, TRUE );
        else
            rViewShell.RemoveOutline( bColumns, TRUE );
    }
}

BOOL __EXPORT ScUndoMakeOutline::CanRepeat(SfxRepeatTarget& rTarget) const
{
    return (rTarget.ISA(ScTabViewTarget));
}

//
//      Outline-Ebene auswaehlen
//

ScUndoOutlineLevel::ScUndoOutlineLevel( ScDocShell* pNewDocShell,
                        SCCOLROW nNewStart, SCCOLROW nNewEnd, SCTAB nNewTab,
                        ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                        BOOL bNewColumns, USHORT nNewLevel ) :
    ScSimpleUndo( pNewDocShell ),
    nStart( nNewStart ),
    nEnd( nNewEnd ),
    nTab( nNewTab ),
    pUndoDoc( pNewUndoDoc ),
    pUndoTable( pNewUndoTab ),
    bColumns( bNewColumns ),
    nLevel( nNewLevel )
{
}

__EXPORT ScUndoOutlineLevel::~ScUndoOutlineLevel()
{
    delete pUndoDoc;
    delete pUndoTable;
}

String __EXPORT ScUndoOutlineLevel::GetComment() const
{   // "Gliederungsebene auswaehlen";
    return ScGlobal::GetRscString( STR_UNDO_OUTLINELEVEL );
}

void __EXPORT ScUndoOutlineLevel::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    //  Original Outline-Table

    pDoc->SetOutlineTable( nTab, pUndoTable );

    //  Original Spalten-/Zeilenstatus

    if (bColumns)
        pUndoDoc->CopyToDocument( static_cast<SCCOL>(nStart), 0, nTab,
                static_cast<SCCOL>(nEnd), MAXROW, nTab, IDF_NONE, FALSE, pDoc);
    else
        pUndoDoc->CopyToDocument( 0, nStart, nTab, MAXCOL, nEnd, nTab, IDF_NONE, FALSE, pDoc );

    pDoc->UpdatePageBreaks( nTab );

    pViewShell->UpdateScrollBars();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP);

    EndUndo();
}

void __EXPORT ScUndoOutlineLevel::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    //  Tabelle muss vorher umgeschaltet sein (#46952#) !!!

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pViewShell->SelectLevel( bColumns, nLevel, FALSE );

    EndRedo();
}

void __EXPORT ScUndoOutlineLevel::Repeat(SfxRepeatTarget& rTarget)
{
    if (rTarget.ISA(ScTabViewTarget))
        ((ScTabViewTarget&)rTarget).GetViewShell()->SelectLevel( bColumns, nLevel, TRUE );
}

BOOL __EXPORT ScUndoOutlineLevel::CanRepeat(SfxRepeatTarget& rTarget) const
{
    return (rTarget.ISA(ScTabViewTarget));
}

//
//      Outline ueber Blockmarken ein- oder ausblenden
//

ScUndoOutlineBlock::ScUndoOutlineBlock( ScDocShell* pNewDocShell,
                        SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                        SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                        ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab, BOOL bNewShow ) :
    ScSimpleUndo( pNewDocShell ),
    aBlockStart( nStartX, nStartY, nStartZ ),
    aBlockEnd( nEndX, nEndY, nEndZ ),
    pUndoDoc( pNewUndoDoc ),
    pUndoTable( pNewUndoTab ),
    bShow( bNewShow )
{
}

__EXPORT ScUndoOutlineBlock::~ScUndoOutlineBlock()
{
    delete pUndoDoc;
    delete pUndoTable;
}

String __EXPORT ScUndoOutlineBlock::GetComment() const
{   // "Detail einblenden" "Detail ausblenden"
    return bShow ?
        ScGlobal::GetRscString( STR_UNDO_DOOUTLINEBLK ) :
        ScGlobal::GetRscString( STR_UNDO_REDOOUTLINEBLK );
}

void __EXPORT ScUndoOutlineBlock::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    SCTAB nTab = aBlockStart.Tab();

    //  Original Outline-Table

    pDoc->SetOutlineTable( nTab, pUndoTable );

    //  Original Spalten-/Zeilenstatus

    SCCOLROW    nStartCol = aBlockStart.Col();
    SCCOLROW    nEndCol = aBlockEnd.Col();
    SCCOLROW    nStartRow = aBlockStart.Row();
    SCCOLROW    nEndRow = aBlockEnd.Row();

    if (!bShow)
    {                               //  Groesse des ausgeblendeten Blocks
        USHORT nLevel;
        pUndoTable->GetColArray()->FindTouchedLevel( nStartCol, nEndCol, nLevel );
        pUndoTable->GetColArray()->ExtendBlock( nLevel, nStartCol, nEndCol );
        pUndoTable->GetRowArray()->FindTouchedLevel( nStartRow, nEndRow, nLevel );
        pUndoTable->GetRowArray()->ExtendBlock( nLevel, nStartRow, nEndRow );
    }

    pUndoDoc->CopyToDocument( static_cast<SCCOL>(nStartCol), 0, nTab,
            static_cast<SCCOL>(nEndCol), MAXROW, nTab, IDF_NONE, FALSE, pDoc );
    pUndoDoc->CopyToDocument( 0, nStartRow, nTab, MAXCOL, nEndRow, nTab, IDF_NONE, FALSE, pDoc );

    pDoc->UpdatePageBreaks( nTab );

    pViewShell->UpdateScrollBars();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP);

    EndUndo();
}

void __EXPORT ScUndoOutlineBlock::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    ScUndoUtil::MarkSimpleBlock( pDocShell, aBlockStart, aBlockEnd );
    if (bShow)
        pViewShell->ShowMarkedOutlines( FALSE );
    else
        pViewShell->HideMarkedOutlines( FALSE );

    EndRedo();
}

void __EXPORT ScUndoOutlineBlock::Repeat(SfxRepeatTarget& rTarget)
{
    if (rTarget.ISA(ScTabViewTarget))
    {
        ScTabViewShell& rViewShell = *((ScTabViewTarget&)rTarget).GetViewShell();

        if (bShow)
            rViewShell.ShowMarkedOutlines( TRUE );
        else
            rViewShell.HideMarkedOutlines( TRUE );
    }
}

BOOL __EXPORT ScUndoOutlineBlock::CanRepeat(SfxRepeatTarget& rTarget) const
{
    return (rTarget.ISA(ScTabViewTarget));
}

//
//      alle Outlines loeschen
//

ScUndoRemoveAllOutlines::ScUndoRemoveAllOutlines( ScDocShell* pNewDocShell,
                                    SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                                    SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                                    ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab ) :
    ScSimpleUndo( pNewDocShell ),
    aBlockStart( nStartX, nStartY, nStartZ ),
    aBlockEnd( nEndX, nEndY, nEndZ ),
    pUndoDoc( pNewUndoDoc ),
    pUndoTable( pNewUndoTab )
{
}

__EXPORT ScUndoRemoveAllOutlines::~ScUndoRemoveAllOutlines()
{
    delete pUndoDoc;
    delete pUndoTable;
}

String __EXPORT ScUndoRemoveAllOutlines::GetComment() const
{   // "Gliederung entfernen"
    return ScGlobal::GetRscString( STR_UNDO_REMOVEALLOTLNS );
}

void __EXPORT ScUndoRemoveAllOutlines::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    SCTAB nTab = aBlockStart.Tab();

    //  Original Outline-Table

    pDoc->SetOutlineTable( nTab, pUndoTable );

    //  Original Spalten-/Zeilenstatus

    SCCOL   nStartCol = aBlockStart.Col();
    SCCOL   nEndCol = aBlockEnd.Col();
    SCROW   nStartRow = aBlockStart.Row();
    SCROW   nEndRow = aBlockEnd.Row();

    pUndoDoc->CopyToDocument( nStartCol, 0, nTab, nEndCol, MAXROW, nTab, IDF_NONE, FALSE, pDoc );
    pUndoDoc->CopyToDocument( 0, nStartRow, nTab, MAXCOL, nEndRow, nTab, IDF_NONE, FALSE, pDoc );

    pDoc->UpdatePageBreaks( nTab );

    pViewShell->UpdateScrollBars();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP|PAINT_SIZE);

    EndUndo();
}

void __EXPORT ScUndoRemoveAllOutlines::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    //  Tabelle muss vorher umgeschaltet sein (#46952#) !!!

    SCTAB nTab = aBlockStart.Tab();
    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pViewShell->RemoveAllOutlines( FALSE );

    EndRedo();
}

void __EXPORT ScUndoRemoveAllOutlines::Repeat(SfxRepeatTarget& rTarget)
{
    if (rTarget.ISA(ScTabViewTarget))
        ((ScTabViewTarget&)rTarget).GetViewShell()->RemoveAllOutlines( TRUE );
}

BOOL __EXPORT ScUndoRemoveAllOutlines::CanRepeat(SfxRepeatTarget& rTarget) const
{
    return (rTarget.ISA(ScTabViewTarget));
}

//
//      Auto-Outline
//

ScUndoAutoOutline::ScUndoAutoOutline( ScDocShell* pNewDocShell,
                                    SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                                    SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                                    ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab ) :
    ScSimpleUndo( pNewDocShell ),
    aBlockStart( nStartX, nStartY, nStartZ ),
    aBlockEnd( nEndX, nEndY, nEndZ ),
    pUndoDoc( pNewUndoDoc ),
    pUndoTable( pNewUndoTab )
{
}

__EXPORT ScUndoAutoOutline::~ScUndoAutoOutline()
{
    delete pUndoDoc;
    delete pUndoTable;
}

String __EXPORT ScUndoAutoOutline::GetComment() const
{   // "Auto-Gliederung"
    return ScGlobal::GetRscString( STR_UNDO_AUTOOUTLINE );
}

void __EXPORT ScUndoAutoOutline::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    SCTAB nTab = aBlockStart.Tab();

    //  Original Outline-Table

    pDoc->SetOutlineTable( nTab, pUndoTable );

    //  Original Spalten-/Zeilenstatus

    if (pUndoDoc && pUndoTable)
    {
        SCCOLROW nStartCol;
        SCCOLROW nStartRow;
        SCCOLROW nEndCol;
        SCCOLROW nEndRow;
        pUndoTable->GetColArray()->GetRange( nStartCol, nEndCol );
        pUndoTable->GetRowArray()->GetRange( nStartRow, nEndRow );

        pUndoDoc->CopyToDocument( static_cast<SCCOL>(nStartCol), 0, nTab,
                static_cast<SCCOL>(nEndCol), MAXROW, nTab, IDF_NONE, FALSE,
                pDoc);
        pUndoDoc->CopyToDocument( 0, nStartRow, nTab, MAXCOL, nEndRow, nTab, IDF_NONE, FALSE, pDoc );

        pViewShell->UpdateScrollBars();
    }

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP|PAINT_SIZE);

    EndUndo();
}

void __EXPORT ScUndoAutoOutline::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    SCTAB nTab = aBlockStart.Tab();
    if (pViewShell)
    {
        //  Tabelle muss vorher umgeschaltet sein (#46952#) !!!

        SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
        if ( nVisTab != nTab )
            pViewShell->SetTabNo( nTab );
    }

    ScRange aRange( aBlockStart.Col(), aBlockStart.Row(), nTab,
                    aBlockEnd.Col(),   aBlockEnd.Row(),   nTab );
    ScOutlineDocFunc aFunc( *pDocShell );
    aFunc.AutoOutline( aRange, FALSE, FALSE );

    //  auf der View markieren
    //  Wenn's beim Aufruf eine Mehrfachselektion war, ist es jetzt der
    //  umschliessende Bereich...

    if (pViewShell)
        pViewShell->MarkRange( aRange );

    EndRedo();
}

void __EXPORT ScUndoAutoOutline::Repeat(SfxRepeatTarget& rTarget)
{
    if (rTarget.ISA(ScTabViewTarget))
        ((ScTabViewTarget&)rTarget).GetViewShell()->AutoOutline( TRUE );
}

BOOL __EXPORT ScUndoAutoOutline::CanRepeat(SfxRepeatTarget& rTarget) const
{
    return (rTarget.ISA(ScTabViewTarget));
}

//
//      Zwischenergebnisse
//

ScUndoSubTotals::ScUndoSubTotals( ScDocShell* pNewDocShell, SCTAB nNewTab,
                                const ScSubTotalParam& rNewParam, SCROW nNewEndY,
                                ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                                ScRangeName* pNewUndoRange, ScDBCollection* pNewUndoDB ) :
    ScDBFuncUndo( pNewDocShell, ScRange( rNewParam.nCol1, rNewParam.nRow1, nNewTab,
                                         rNewParam.nCol2, rNewParam.nRow2, nNewTab ) ),
    nTab( nNewTab ),
    aParam( rNewParam ),
    nNewEndRow( nNewEndY ),
    pUndoDoc( pNewUndoDoc ),
    pUndoTable( pNewUndoTab ),
    pUndoRange( pNewUndoRange ),
    pUndoDB( pNewUndoDB )
{
}

__EXPORT ScUndoSubTotals::~ScUndoSubTotals()
{
    delete pUndoDoc;
    delete pUndoTable;
    delete pUndoRange;
    delete pUndoDB;
}

String __EXPORT ScUndoSubTotals::GetComment() const
{   // "Teilergebnisse"
    return ScGlobal::GetRscString( STR_UNDO_SUBTOTALS );
}

void __EXPORT ScUndoSubTotals::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    //  um einzelnen DB-Bereich anzupassen
/*  ScDBData* pOldDBData = ScUndoUtil::GetOldDBData( pUndoDBData, pDoc, nTab,
                                        aParam.nCol1, aParam.nRow1, aParam.nCol2, nNewEndRow );
*/

    if (nNewEndRow > aParam.nRow2)
    {
        pDoc->DeleteRow( 0,nTab, MAXCOL,nTab, aParam.nRow2+1, static_cast<SCSIZE>(nNewEndRow-aParam.nRow2) );
    }
    else if (nNewEndRow < aParam.nRow2)
    {
        pDoc->InsertRow( 0,nTab, MAXCOL,nTab, nNewEndRow+1, static_cast<SCSIZE>(aParam.nRow2-nNewEndRow) );
    }


    //  Original Outline-Table

    pDoc->SetOutlineTable( nTab, pUndoTable );

    //  Original Spalten-/Zeilenstatus

    if (pUndoDoc && pUndoTable)
    {
        SCCOLROW nStartCol;
        SCCOLROW nStartRow;
        SCCOLROW nEndCol;
        SCCOLROW nEndRow;
        pUndoTable->GetColArray()->GetRange( nStartCol, nEndCol );
        pUndoTable->GetRowArray()->GetRange( nStartRow, nEndRow );

        pUndoDoc->CopyToDocument( static_cast<SCCOL>(nStartCol), 0, nTab,
                static_cast<SCCOL>(nEndCol), MAXROW, nTab, IDF_NONE, FALSE,
                pDoc);
        pUndoDoc->CopyToDocument( 0, nStartRow, nTab, MAXCOL, nEndRow, nTab, IDF_NONE, FALSE, pDoc );

        pViewShell->UpdateScrollBars();
    }

    //  Original-Daten & Referenzen

    ScUndoUtil::MarkSimpleBlock( pDocShell, 0, aParam.nRow1+1, nTab,
                                            MAXCOL, aParam.nRow2, nTab );

    pDoc->DeleteAreaTab( 0,aParam.nRow1+1, MAXCOL,aParam.nRow2, nTab, IDF_ALL );

    pUndoDoc->CopyToDocument( 0, aParam.nRow1+1, nTab, MAXCOL, aParam.nRow2, nTab,
                                                            IDF_NONE, FALSE, pDoc );    // Flags
    pUndoDoc->UndoToDocument( 0, aParam.nRow1+1, nTab, MAXCOL, aParam.nRow2, nTab,
                                                            IDF_ALL, FALSE, pDoc );

    ScUndoUtil::MarkSimpleBlock( pDocShell, aParam.nCol1,aParam.nRow1,nTab,
                                            aParam.nCol2,aParam.nRow2,nTab );

/*  if (pUndoDBData)
        *pOldDBData = *pUndoDBData;
*/
    if (pUndoRange)
        pDoc->SetRangeName( new ScRangeName( *pUndoRange ) );
    if (pUndoDB)
        pDoc->SetDBCollection( new ScDBCollection( *pUndoDB ), TRUE );

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP|PAINT_SIZE);
    pDocShell->PostDataChanged();

    EndUndo();
}

void __EXPORT ScUndoSubTotals::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    ScUndoUtil::MarkSimpleBlock( pDocShell, aParam.nCol1,aParam.nRow1,nTab,
                                            aParam.nCol2,aParam.nRow2,nTab );
    pViewShell->DoSubTotals( aParam, FALSE );

    EndRedo();
}

void __EXPORT ScUndoSubTotals::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL __EXPORT ScUndoSubTotals::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;                       // geht nicht wegen Spaltennummern
}

//
//      Sortieren
//

ScUndoSort::ScUndoSort( ScDocShell* pNewDocShell,
                        SCTAB nNewTab, const ScSortParam& rParam,
                        BOOL bQuery, ScDocument* pNewUndoDoc, ScDBCollection* pNewUndoDB,
                        const ScRange* pDest ) :
    ScDBFuncUndo( pNewDocShell, ScRange( rParam.nCol1, rParam.nRow1, nNewTab,
                                         rParam.nCol2, rParam.nRow2, nNewTab ) ),
    nTab( nNewTab ),
    aSortParam( rParam ),
    bRepeatQuery( bQuery ),
    pUndoDoc( pNewUndoDoc ),
    pUndoDB( pNewUndoDB ),
    bDestArea( FALSE )
{
    if ( pDest )
    {
        bDestArea = TRUE;
        aDestRange = *pDest;
    }
}

__EXPORT ScUndoSort::~ScUndoSort()
{
    delete pUndoDoc;
    delete pUndoDB;
}

String __EXPORT ScUndoSort::GetComment() const
{
    return ScGlobal::GetRscString( STR_UNDO_SORT );
}

void __EXPORT ScUndoSort::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    SCCOL nStartCol = aSortParam.nCol1;
    SCROW nStartRow = aSortParam.nRow1;
    SCCOL nEndCol    = aSortParam.nCol2;
    SCROW nEndRow    = aSortParam.nRow2;
    SCTAB nSortTab  = nTab;
    if ( !aSortParam.bInplace )
    {
        nStartCol = aSortParam.nDestCol;
        nStartRow = aSortParam.nDestRow;
        nEndCol   = nStartCol + ( aSortParam.nCol2 - aSortParam.nCol1 );
        nEndRow   = nStartRow + ( aSortParam.nRow2 - aSortParam.nRow1 );
        nSortTab  = aSortParam.nDestTab;
    }

    ScUndoUtil::MarkSimpleBlock( pDocShell, nStartCol, nStartRow, nSortTab,
                                 nEndCol, nEndRow, nSortTab );

    // do not delete/copy note captions, they are handled in drawing undo (ScDBFuncUndo::mpDrawUndo)
    pDoc->DeleteAreaTab( nStartCol,nStartRow, nEndCol,nEndRow, nSortTab, IDF_ALL|IDF_NOCAPTIONS );
    pUndoDoc->CopyToDocument( nStartCol, nStartRow, nSortTab, nEndCol, nEndRow, nSortTab,
                                IDF_ALL|IDF_NOCAPTIONS, FALSE, pDoc );

    if (bDestArea)
    {
        // do not delete/copy note captions, they are handled in drawing undo (ScDBFuncUndo::mpDrawUndo)
        pDoc->DeleteAreaTab( aDestRange, IDF_ALL|IDF_NOCAPTIONS );
        pUndoDoc->CopyToDocument( aDestRange, IDF_ALL|IDF_NOCAPTIONS, FALSE, pDoc );
    }

    //  Zeilenhoehen immer (wegen automatischer Anpassung)
    //! auf ScBlockUndo umstellen
//  if (bRepeatQuery)
        pUndoDoc->CopyToDocument( 0, nStartRow, nSortTab, MAXCOL, nEndRow, nSortTab,
                                IDF_NONE, FALSE, pDoc );

    if (pUndoDB)
        pDoc->SetDBCollection( new ScDBCollection( *pUndoDB ), TRUE );

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nSortTab )
        pViewShell->SetTabNo( nSortTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP|PAINT_SIZE);
    pDocShell->PostDataChanged();

    EndUndo();
}

void __EXPORT ScUndoSort::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

//  pViewShell->DoneBlockMode();
//  pViewShell->InitOwnBlockMode();
//  pViewShell->GetViewData()->GetMarkData() = aMarkData;   // CopyMarksTo

    pViewShell->MarkRange( ScRange( aSortParam.nCol1, aSortParam.nRow1, nTab,
                                      aSortParam.nCol2, aSortParam.nRow2, nTab ) );

    pViewShell->Sort( aSortParam, FALSE );

    //  Quellbereich painten wegen Markierung
    if ( !aSortParam.bInplace )
        pDocShell->PostPaint( aSortParam.nCol1, aSortParam.nRow1, nTab,
                              aSortParam.nCol2, aSortParam.nRow2, nTab, PAINT_GRID );

    EndRedo();
}

void __EXPORT ScUndoSort::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL __EXPORT ScUndoSort::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;                       // geht nicht wegen Spaltennummern
}

//
//      Filtern
//

ScUndoQuery::ScUndoQuery( ScDocShell* pNewDocShell, SCTAB nNewTab, const ScQueryParam& rParam,
                            ScDocument* pNewUndoDoc, ScDBCollection* pNewUndoDB,
                            const ScRange* pOld, BOOL bSize, const ScRange* pAdvSrc ) :
    ScDBFuncUndo( pNewDocShell, ScRange( rParam.nCol1, rParam.nRow1, nNewTab,
                                         rParam.nCol2, rParam.nRow2, nNewTab ) ),
    pDrawUndo( NULL ),
    nTab( nNewTab ),
    aQueryParam( rParam ),
    pUndoDoc( pNewUndoDoc ),
//  pUndoDBData( pNewData )
    pUndoDB( pNewUndoDB ),
    bIsAdvanced( FALSE ),
    bDestArea( FALSE ),
    bDoSize( bSize )
{
    if ( pOld )
    {
        bDestArea = TRUE;
        aOldDest = *pOld;
    }
    if ( pAdvSrc )
    {
        bIsAdvanced = TRUE;
        aAdvSource = *pAdvSrc;
    }

    pDrawUndo = GetSdrUndoAction( pDocShell->GetDocument() );
}

__EXPORT ScUndoQuery::~ScUndoQuery()
{
    delete pUndoDoc;
//  delete pUndoDBData;
    delete pUndoDB;
    DeleteSdrUndoAction( pDrawUndo );
}

String __EXPORT ScUndoQuery::GetComment() const
{   // "Filtern";
    return ScGlobal::GetRscString( STR_UNDO_QUERY );
}

void __EXPORT ScUndoQuery::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    BOOL bCopy = !aQueryParam.bInplace;
    SCCOL nDestEndCol = 0;
    SCROW nDestEndRow = 0;
    if (bCopy)
    {
        nDestEndCol = aQueryParam.nDestCol + ( aQueryParam.nCol2-aQueryParam.nCol1 );
        nDestEndRow = aQueryParam.nDestRow + ( aQueryParam.nRow2-aQueryParam.nRow1 );

        ScDBData* pData = pDoc->GetDBAtCursor( aQueryParam.nDestCol, aQueryParam.nDestRow,
                                                aQueryParam.nDestTab, TRUE );
        if (pData)
        {
            ScRange aNewDest;
            pData->GetArea( aNewDest );
            nDestEndCol = aNewDest.aEnd.Col();
            nDestEndRow = aNewDest.aEnd.Row();
        }

        if ( bDoSize && bDestArea )
        {
            //  aDestRange ist der alte Bereich
            pDoc->FitBlock( ScRange(
                                aQueryParam.nDestCol, aQueryParam.nDestRow, aQueryParam.nDestTab,
                                nDestEndCol, nDestEndRow, aQueryParam.nDestTab ),
                            aOldDest );
        }

        ScUndoUtil::MarkSimpleBlock( pDocShell,
                                    aQueryParam.nDestCol, aQueryParam.nDestRow, aQueryParam.nDestTab,
                                    nDestEndCol, nDestEndRow, aQueryParam.nDestTab );
        pDoc->DeleteAreaTab( aQueryParam.nDestCol, aQueryParam.nDestRow,
                            nDestEndCol, nDestEndRow, aQueryParam.nDestTab, IDF_ALL );

        pViewShell->DoneBlockMode();

        pUndoDoc->CopyToDocument( aQueryParam.nDestCol, aQueryParam.nDestRow, aQueryParam.nDestTab,
                                    nDestEndCol, nDestEndRow, aQueryParam.nDestTab,
                                    IDF_ALL, FALSE, pDoc );
        //  Attribute werden immer mitkopiert (#49287#)

        //  Rest von altem Bereich
        if ( bDestArea && !bDoSize )
        {
            pDoc->DeleteAreaTab( aOldDest, IDF_ALL );
            pUndoDoc->CopyToDocument( aOldDest, IDF_ALL, FALSE, pDoc );
        }
    }
    else
        pUndoDoc->CopyToDocument( 0, aQueryParam.nRow1, nTab, MAXCOL, aQueryParam.nRow2, nTab,
                                        IDF_NONE, FALSE, pDoc );

    if (pUndoDB)
        pDoc->SetDBCollection( new ScDBCollection( *pUndoDB ), TRUE );

    if (!bCopy)
    {
        pDoc->InvalidatePageBreaks(nTab);
        pDoc->UpdatePageBreaks( nTab );
    }

    ScRange aDirtyRange( 0 , aQueryParam.nRow1, nTab,
        MAXCOL, aQueryParam.nRow2, nTab );
    pDoc->SetDirty( aDirtyRange );

    DoSdrUndoAction( pDrawUndo, pDoc );

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

        //  Paint

    if (bCopy)
    {
        SCCOL nEndX = nDestEndCol;
        SCROW nEndY = nDestEndRow;
        if (bDestArea)
        {
            if ( aOldDest.aEnd.Col() > nEndX )
                nEndX = aOldDest.aEnd.Col();
            if ( aOldDest.aEnd.Row() > nEndY )
                nEndY = aOldDest.aEnd.Row();
        }
        if (bDoSize)
            nEndY = MAXROW;
        pDocShell->PostPaint( aQueryParam.nDestCol, aQueryParam.nDestRow, aQueryParam.nDestTab,
                                    nEndX, nEndY, aQueryParam.nDestTab, PAINT_GRID );
    }
    else
        pDocShell->PostPaint( 0, aQueryParam.nRow1, nTab, MAXCOL, MAXROW, nTab,
                                                    PAINT_GRID | PAINT_LEFT );
    pDocShell->PostDataChanged();

    EndUndo();
}

void __EXPORT ScUndoQuery::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    if ( bIsAdvanced )
        pViewShell->Query( aQueryParam, &aAdvSource, FALSE );
    else
        pViewShell->Query( aQueryParam, NULL, FALSE );

    EndRedo();
}

void __EXPORT ScUndoQuery::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL __EXPORT ScUndoQuery::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;                       // geht nicht wegen Spaltennummern
}

//
//      Show or hide AutoFilter buttons (doesn't include filter settings)
//

ScUndoAutoFilter::ScUndoAutoFilter( ScDocShell* pNewDocShell, const ScRange& rRange,
                                    const String& rName, BOOL bSet ) :
    ScDBFuncUndo( pNewDocShell, rRange ),
    aDBName( rName ),
    bFilterSet( bSet )
{
}

ScUndoAutoFilter::~ScUndoAutoFilter()
{
}

String ScUndoAutoFilter::GetComment() const
{
    return ScGlobal::GetRscString( STR_UNDO_QUERY );    // same as ScUndoQuery
}

void ScUndoAutoFilter::DoChange( BOOL bUndo )
{
    BOOL bNewFilter = bUndo ? !bFilterSet : bFilterSet;

    USHORT nIndex;
    ScDocument* pDoc = pDocShell->GetDocument();
    ScDBCollection* pColl = pDoc->GetDBCollection();
    if ( pColl->SearchName( aDBName, nIndex ) )
    {
        ScDBData* pDBData = (*pColl)[nIndex];
        pDBData->SetAutoFilter( bNewFilter );

        SCCOL nRangeX1;
        SCROW nRangeY1;
        SCCOL nRangeX2;
        SCROW nRangeY2;
        SCTAB nRangeTab;
        pDBData->GetArea( nRangeTab, nRangeX1, nRangeY1, nRangeX2, nRangeY2 );

        if ( bNewFilter )
            pDoc->ApplyFlagsTab( nRangeX1, nRangeY1, nRangeX2, nRangeY1, nRangeTab, SC_MF_AUTO );
        else
            pDoc->RemoveFlagsTab( nRangeX1, nRangeY1, nRangeX2, nRangeY1, nRangeTab, SC_MF_AUTO );

        pDocShell->PostPaint( nRangeX1, nRangeY1, nRangeTab, nRangeX2, nRangeY1, nRangeTab, PAINT_GRID );
    }
}

void ScUndoAutoFilter::Undo()
{
    BeginUndo();
    DoChange( TRUE );
    EndUndo();
}

void ScUndoAutoFilter::Redo()
{
    BeginRedo();
    DoChange( FALSE );
    EndRedo();
}

void ScUndoAutoFilter::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL ScUndoAutoFilter::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;
}

//
//      Datenbankbereiche aendern (Dialog)
//

ScUndoDBData::ScUndoDBData( ScDocShell* pNewDocShell,
                            ScDBCollection* pNewUndoColl, ScDBCollection* pNewRedoColl ) :
    ScSimpleUndo( pNewDocShell ),
    pUndoColl( pNewUndoColl ),
    pRedoColl( pNewRedoColl )
{
}

__EXPORT ScUndoDBData::~ScUndoDBData()
{
    delete pUndoColl;
    delete pRedoColl;
}

String __EXPORT ScUndoDBData::GetComment() const
{   // "Datenbankbereiche aendern";
    return ScGlobal::GetRscString( STR_UNDO_DBDATA );
}

void __EXPORT ScUndoDBData::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();

    BOOL bOldAutoCalc = pDoc->GetAutoCalc();
    pDoc->SetAutoCalc( FALSE );         // unnoetige Berechnungen vermeiden
    pDoc->CompileDBFormula( TRUE );     // CreateFormulaString
    pDoc->SetDBCollection( new ScDBCollection(*pUndoColl), TRUE );
    pDoc->CompileDBFormula( FALSE );    // CompileFormulaString
    pDoc->SetAutoCalc( bOldAutoCalc );

    SFX_APP()->Broadcast( SfxSimpleHint( SC_HINT_DBAREAS_CHANGED ) );

    EndUndo();
}

void __EXPORT ScUndoDBData::Redo()
{
    BeginRedo();

    ScDocument* pDoc = pDocShell->GetDocument();

    BOOL bOldAutoCalc = pDoc->GetAutoCalc();
    pDoc->SetAutoCalc( FALSE );         // unnoetige Berechnungen vermeiden
    pDoc->CompileDBFormula( TRUE );     // CreateFormulaString
    pDoc->SetDBCollection( new ScDBCollection(*pRedoColl), TRUE );
    pDoc->CompileDBFormula( FALSE );    // CompileFormulaString
    pDoc->SetAutoCalc( bOldAutoCalc );

    SFX_APP()->Broadcast( SfxSimpleHint( SC_HINT_DBAREAS_CHANGED ) );

    EndRedo();
}

void __EXPORT ScUndoDBData::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL __EXPORT ScUndoDBData::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;                       // geht nicht
}

//
//      Import
//

ScUndoImportData::ScUndoImportData( ScDocShell* pNewDocShell, SCTAB nNewTab,
                                const ScImportParam& rParam, SCCOL nNewEndX, SCROW nNewEndY,
                                SCCOL nNewFormula,
                                ScDocument* pNewUndoDoc, ScDocument* pNewRedoDoc,
                                ScDBData* pNewUndoData, ScDBData* pNewRedoData ) :
    ScSimpleUndo( pNewDocShell ),
    nTab( nNewTab ),
    aImportParam( rParam ),
    nEndCol( nNewEndX ),
    nEndRow( nNewEndY ),
    pUndoDoc( pNewUndoDoc ),
    pRedoDoc( pNewRedoDoc ),
    pUndoDBData( pNewUndoData ),
    pRedoDBData( pNewRedoData ),
    nFormulaCols( nNewFormula ),
    bRedoFilled( FALSE )
{
    // redo doc doesn't contain imported data (but everything else)
}

__EXPORT ScUndoImportData::~ScUndoImportData()
{
    delete pUndoDoc;
    delete pRedoDoc;
    delete pUndoDBData;
    delete pRedoDBData;
}

String __EXPORT ScUndoImportData::GetComment() const
{   // "Importieren";
    return ScGlobal::GetRscString( STR_UNDO_IMPORTDATA );
}

void __EXPORT ScUndoImportData::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    ScUndoUtil::MarkSimpleBlock( pDocShell, aImportParam.nCol1,aImportParam.nRow1,nTab,
                                                        nEndCol,nEndRow,nTab );

    SCTAB nTable;
    SCCOL nCol1, nCol2;
    SCROW nRow1, nRow2;
    ScDBData* pCurrentData = NULL;
    if (pUndoDBData && pRedoDBData)
    {
        pRedoDBData->GetArea( nTable, nCol1, nRow1, nCol2, nRow2 );
        pCurrentData = ScUndoUtil::GetOldDBData( pRedoDBData, pDoc, nTab,
                                                    nCol1, nRow1, nCol2, nRow2 );

        if ( !bRedoFilled )
        {
            //  read redo data from document at first undo
            //  imported data is deleted later anyway,
            //  so now delete each column after copying to save memory (#41216#)

            BOOL bOldAutoCalc = pDoc->GetAutoCalc();
            pDoc->SetAutoCalc( FALSE );             // outside of the loop
            for (SCCOL nCopyCol = nCol1; nCopyCol <= nCol2; nCopyCol++)
            {
                pDoc->CopyToDocument( nCopyCol,nRow1,nTab, nCopyCol,nRow2,nTab,
                                        IDF_CONTENTS & ~IDF_NOTE, FALSE, pRedoDoc );
                pDoc->DeleteAreaTab( nCopyCol,nRow1, nCopyCol,nRow2, nTab, IDF_CONTENTS & ~IDF_NOTE );
                pDoc->DoColResize( nTab, nCopyCol, nCopyCol, 0 );
            }
            pDoc->SetAutoCalc( bOldAutoCalc );
            bRedoFilled = TRUE;
        }
    }
    BOOL bMoveCells = pUndoDBData && pRedoDBData &&
                        pRedoDBData->IsDoSize();        // in alt und neu gleich
    if (bMoveCells)
    {
        //  Undo: erst die neuen Daten loeschen, dann FitBlock rueckwaerts

        ScRange aOld, aNew;
        pUndoDBData->GetArea( aOld );
        pRedoDBData->GetArea( aNew );

        pDoc->DeleteAreaTab( aNew.aStart.Col(), aNew.aStart.Row(),
                                aNew.aEnd.Col(), aNew.aEnd.Row(), nTab, IDF_ALL & ~IDF_NOTE );

        aOld.aEnd.SetCol( aOld.aEnd.Col() + nFormulaCols );     // FitBlock auch fuer Formeln
        aNew.aEnd.SetCol( aNew.aEnd.Col() + nFormulaCols );
        pDoc->FitBlock( aNew, aOld, FALSE );                    // rueckwaerts
    }
    else
        pDoc->DeleteAreaTab( aImportParam.nCol1,aImportParam.nRow1,
                                nEndCol,nEndRow, nTab, IDF_ALL & ~IDF_NOTE );

    pUndoDoc->CopyToDocument( aImportParam.nCol1,aImportParam.nRow1,nTab,
                                nEndCol+nFormulaCols,nEndRow,nTab,
                                IDF_ALL & ~IDF_NOTE, FALSE, pDoc );

    if (pCurrentData)
    {
        *pCurrentData = *pUndoDBData;

        pUndoDBData->GetArea( nTable, nCol1, nRow1, nCol2, nRow2 );
        ScUndoUtil::MarkSimpleBlock( pDocShell, nCol1, nRow1, nTable, nCol2, nRow2, nTable );
    }

// erack! it's broadcasted
//  pDoc->SetDirty();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    if (bMoveCells)
        pDocShell->PostPaint( 0,0,nTab, MAXCOL,MAXROW,nTab, PAINT_GRID );
    else
        pDocShell->PostPaint( aImportParam.nCol1,aImportParam.nRow1,nTab,
                                nEndCol,nEndRow,nTab, PAINT_GRID );
    pDocShell->PostDataChanged();

    EndUndo();
}

void __EXPORT ScUndoImportData::Redo()
{
    BeginRedo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    ScUndoUtil::MarkSimpleBlock( pDocShell, aImportParam.nCol1,aImportParam.nRow1,nTab,
                                                        nEndCol,nEndRow,nTab );

    SCTAB nTable;
    SCCOL nCol1, nCol2;
    SCROW nRow1, nRow2;
    ScDBData* pCurrentData = NULL;
    if (pUndoDBData && pRedoDBData)
    {
        pUndoDBData->GetArea( nTable, nCol1, nRow1, nCol2, nRow2 );
        pCurrentData = ScUndoUtil::GetOldDBData( pUndoDBData, pDoc, nTab,
                                                    nCol1, nRow1, nCol2, nRow2 );
    }
    BOOL bMoveCells = pUndoDBData && pRedoDBData &&
                        pRedoDBData->IsDoSize();        // in alt und neu gleich
    if (bMoveCells)
    {
        //  Redo: FitBlock, dann Daten loeschen (noetig fuer CopyToDocument)

        ScRange aOld, aNew;
        pUndoDBData->GetArea( aOld );
        pRedoDBData->GetArea( aNew );

        aOld.aEnd.SetCol( aOld.aEnd.Col() + nFormulaCols );     // FitBlock auch fuer Formeln
        aNew.aEnd.SetCol( aNew.aEnd.Col() + nFormulaCols );
        pDoc->FitBlock( aOld, aNew );

        pDoc->DeleteAreaTab( aNew.aStart.Col(), aNew.aStart.Row(),
                                aNew.aEnd.Col(), aNew.aEnd.Row(), nTab, IDF_ALL & ~IDF_NOTE );

        pRedoDoc->CopyToDocument( aNew, IDF_ALL & ~IDF_NOTE, FALSE, pDoc );        // incl. Formeln
    }
    else
    {
        pDoc->DeleteAreaTab( aImportParam.nCol1,aImportParam.nRow1,
                                nEndCol,nEndRow, nTab, IDF_ALL & ~IDF_NOTE );
        pRedoDoc->CopyToDocument( aImportParam.nCol1,aImportParam.nRow1,nTab,
                                nEndCol,nEndRow,nTab, IDF_ALL & ~IDF_NOTE, FALSE, pDoc );
    }

    if (pCurrentData)
    {
        *pCurrentData = *pRedoDBData;

        pRedoDBData->GetArea( nTable, nCol1, nRow1, nCol2, nRow2 );
        ScUndoUtil::MarkSimpleBlock( pDocShell, nCol1, nRow1, nTable, nCol2, nRow2, nTable );
    }

// erack! it's broadcasted
//  pDoc->SetDirty();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    if (bMoveCells)
        pDocShell->PostPaint( 0,0,nTab, MAXCOL,MAXROW,nTab, PAINT_GRID );
    else
        pDocShell->PostPaint( aImportParam.nCol1,aImportParam.nRow1,nTab,
                                nEndCol,nEndRow,nTab, PAINT_GRID );
    pDocShell->PostDataChanged();

    EndRedo();
}

void __EXPORT ScUndoImportData::Repeat(SfxRepeatTarget& rTarget)
{
    if (rTarget.ISA(ScTabViewTarget))
    {
        ScTabViewShell& rViewShell = *((ScTabViewTarget&)rTarget).GetViewShell();

        SCTAB nDummy;
        ScImportParam aNewParam(aImportParam);
        ScDBData* pDBData = rViewShell.GetDBData();
        pDBData->GetArea( nDummy, aNewParam.nCol1,aNewParam.nRow1, aNewParam.nCol2,aNewParam.nRow2 );

        rViewShell.ImportData( aNewParam );
    }
}

BOOL __EXPORT ScUndoImportData::CanRepeat(SfxRepeatTarget& rTarget) const
{
    //  Repeat nur fuer Import per DB-Bereich, dann ist pUndoDBData gesetzt

    if (pUndoDBData)
        return (rTarget.ISA(ScTabViewTarget));
    else
        return FALSE;       // Adressbuch
}

//
//      Operationen wiederholen
//

ScUndoRepeatDB::ScUndoRepeatDB( ScDocShell* pNewDocShell, SCTAB nNewTab,
                                SCCOL nStartX, SCROW nStartY, SCCOL nEndX, SCROW nEndY,
                                SCROW nResultEndRow, SCCOL nCurX, SCROW nCurY,
                                ScDocument* pNewUndoDoc, ScOutlineTable* pNewUndoTab,
                                ScRangeName* pNewUndoRange, ScDBCollection* pNewUndoDB,
                                const ScRange* pOldQ, const ScRange* pNewQ ) :
    ScSimpleUndo( pNewDocShell ),
    aBlockStart( nStartX,nStartY,nNewTab ),
    aBlockEnd( nEndX,nEndY,nNewTab ),
    nNewEndRow( nResultEndRow ),
    aCursorPos( nCurX,nCurY,nNewTab ),
    pUndoDoc( pNewUndoDoc ),
    pUndoTable( pNewUndoTab ),
    pUndoRange( pNewUndoRange ),
    pUndoDB( pNewUndoDB ),
    bQuerySize( FALSE )
{
    if ( pOldQ && pNewQ )
    {
        aOldQuery = *pOldQ;
        aNewQuery = *pNewQ;
        bQuerySize = TRUE;;
    }
}

__EXPORT ScUndoRepeatDB::~ScUndoRepeatDB()
{
    delete pUndoDoc;
    delete pUndoTable;
    delete pUndoRange;
    delete pUndoDB;
}

String __EXPORT ScUndoRepeatDB::GetComment() const
{   // "Wiederholen";       //! bessere Beschreibung!
    return ScGlobal::GetRscString( STR_UNDO_REPEATDB );
}

void __EXPORT ScUndoRepeatDB::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    SCTAB nTab = aBlockStart.Tab();

    if (bQuerySize)
    {
        pDoc->FitBlock( aNewQuery, aOldQuery, FALSE );

        if ( aNewQuery.aEnd.Col() == aOldQuery.aEnd.Col() )
        {
            SCCOL nFormulaCols = 0;
            SCCOL nCol = aOldQuery.aEnd.Col() + 1;
            SCROW nRow = aOldQuery.aStart.Row() + 1;        //! Header testen
            while ( nCol <= MAXCOL &&
                    pDoc->GetCellType(ScAddress( nCol, nRow, nTab )) == CELLTYPE_FORMULA )
                ++nCol, ++nFormulaCols;

            if ( nFormulaCols > 0 )
            {
                ScRange aOldForm = aOldQuery;
                aOldForm.aStart.SetCol( aOldQuery.aEnd.Col() + 1 );
                aOldForm.aEnd.SetCol( aOldQuery.aEnd.Col() + nFormulaCols );
                ScRange aNewForm = aOldForm;
                aNewForm.aEnd.SetRow( aNewQuery.aEnd.Row() );
                pDoc->FitBlock( aNewForm, aOldForm, FALSE );
            }
        }
    }

    //!     Daten von Filter in anderen Bereich fehlen noch !!!!!!!!!!!!!!!!!

    if (nNewEndRow > aBlockEnd.Row())
    {
        pDoc->DeleteRow( 0,nTab, MAXCOL,nTab, aBlockEnd.Row()+1, static_cast<SCSIZE>(nNewEndRow-aBlockEnd.Row()) );
    }
    else if (nNewEndRow < aBlockEnd.Row())
    {
        pDoc->InsertRow( 0,nTab, MAXCOL,nTab, nNewEndRow+1, static_cast<SCSIZE>(nNewEndRow-aBlockEnd.Row()) );
    }

    //  Original Outline-Table

    pDoc->SetOutlineTable( nTab, pUndoTable );

    //  Original Spalten-/Zeilenstatus

    if (pUndoDoc && pUndoTable)
    {
        SCCOLROW nStartCol;
        SCCOLROW nStartRow;
        SCCOLROW nEndCol;
        SCCOLROW nEndRow;
        pUndoTable->GetColArray()->GetRange( nStartCol, nEndCol );
        pUndoTable->GetRowArray()->GetRange( nStartRow, nEndRow );

        pUndoDoc->CopyToDocument( static_cast<SCCOL>(nStartCol), 0, nTab,
                static_cast<SCCOL>(nEndCol), MAXROW, nTab, IDF_NONE, FALSE,
                pDoc );
        pUndoDoc->CopyToDocument( 0, nStartRow, nTab, MAXCOL, nEndRow, nTab, IDF_NONE, FALSE, pDoc );

        pViewShell->UpdateScrollBars();
    }

    //  Original-Daten & Referenzen

    ScUndoUtil::MarkSimpleBlock( pDocShell, 0, aBlockStart.Row(), nTab,
                                            MAXCOL, aBlockEnd.Row(), nTab );
    pDoc->DeleteAreaTab( 0, aBlockStart.Row(),
                            MAXCOL, aBlockEnd.Row(), nTab, IDF_ALL );

    pUndoDoc->CopyToDocument( 0, aBlockStart.Row(), nTab, MAXCOL, aBlockEnd.Row(), nTab,
                                                            IDF_NONE, FALSE, pDoc );            // Flags
    pUndoDoc->UndoToDocument( 0, aBlockStart.Row(), nTab, MAXCOL, aBlockEnd.Row(), nTab,
                                                            IDF_ALL, FALSE, pDoc );

    ScUndoUtil::MarkSimpleBlock( pDocShell, aBlockStart.Col(),aBlockStart.Row(),nTab,
                                            aBlockEnd.Col(),aBlockEnd.Row(),nTab );

    if (pUndoRange)
        pDoc->SetRangeName( new ScRangeName( *pUndoRange ) );
    if (pUndoDB)
        pDoc->SetDBCollection( new ScDBCollection( *pUndoDB ), TRUE );

// erack! it's broadcasted
//  pDoc->SetDirty();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    pDocShell->PostPaint(0,0,nTab,MAXCOL,MAXROW,nTab,PAINT_GRID|PAINT_LEFT|PAINT_TOP|PAINT_SIZE);
    pDocShell->PostDataChanged();

    EndUndo();
}

void __EXPORT ScUndoRepeatDB::Redo()
{
    BeginRedo();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    SCTAB nTab = aBlockStart.Tab();

    SCTAB nVisTab = pViewShell->GetViewData()->GetTabNo();
    if ( nVisTab != nTab )
        pViewShell->SetTabNo( nTab );

    ScUndoUtil::MarkSimpleBlock( pDocShell, aBlockStart.Col(),aBlockStart.Row(),nTab,
                                            aBlockEnd.Col(),aBlockEnd.Row(),nTab );
    pViewShell->SetCursor( aCursorPos.Col(), aCursorPos.Row() );

    pViewShell->RepeatDB( FALSE );

    EndRedo();
}

void __EXPORT ScUndoRepeatDB::Repeat(SfxRepeatTarget& rTarget)
{
    if (rTarget.ISA(ScTabViewTarget))
        ((ScTabViewTarget&)rTarget).GetViewShell()->RepeatDB( TRUE );
}

BOOL __EXPORT ScUndoRepeatDB::CanRepeat(SfxRepeatTarget& rTarget) const
{
    return (rTarget.ISA(ScTabViewTarget));
}

//
//      data pilot
//

ScUndoDataPilot::ScUndoDataPilot( ScDocShell* pNewDocShell,
                            ScDocument* pOldDoc, ScDocument* pNewDoc,
                            const ScDPObject* pOldObj, const ScDPObject* pNewObj, BOOL bMove ) :
    ScSimpleUndo( pNewDocShell ),
    pOldUndoDoc( pOldDoc ),
    pNewUndoDoc( pNewDoc ),
    pOldDPObject( NULL ),
    pNewDPObject( NULL ),
    bAllowMove( bMove )
{
    if (pOldObj)
        pOldDPObject = new ScDPObject( *pOldObj );
    if (pNewObj)
        pNewDPObject = new ScDPObject( *pNewObj );
}

__EXPORT ScUndoDataPilot::~ScUndoDataPilot()
{
    delete pOldDPObject;
    delete pNewDPObject;
    delete pOldUndoDoc;
    delete pNewUndoDoc;
}

String __EXPORT ScUndoDataPilot::GetComment() const
{
    USHORT nIndex;
    if ( pOldUndoDoc && pNewUndoDoc )
        nIndex = STR_UNDO_PIVOT_MODIFY;
    else if ( pNewUndoDoc )
        nIndex = STR_UNDO_PIVOT_NEW;
    else
        nIndex = STR_UNDO_PIVOT_DELETE;

    return ScGlobal::GetRscString( nIndex );
}

void __EXPORT ScUndoDataPilot::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();

    ScRange aOldRange;
    ScRange aNewRange;

    if ( pNewDPObject && pNewUndoDoc )
    {
        aNewRange = pNewDPObject->GetOutRange();
        pDoc->DeleteAreaTab( aNewRange, IDF_ALL );
        pNewUndoDoc->CopyToDocument( aNewRange, IDF_ALL, FALSE, pDoc );
    }
    if ( pOldDPObject && pOldUndoDoc )
    {
        aOldRange = pOldDPObject->GetOutRange();
        pDoc->DeleteAreaTab( aOldRange, IDF_ALL );
        pOldUndoDoc->CopyToDocument( aOldRange, IDF_ALL, FALSE, pDoc );
    }

    //  update objects in collection

    if ( pNewDPObject )
    {
        //  find updated object
        //! find by name!

        ScDPObject* pDocObj = pDoc->GetDPAtCursor(
                            aNewRange.aStart.Col(), aNewRange.aStart.Row(), aNewRange.aStart.Tab() );
        DBG_ASSERT(pDocObj, "DPObject not found");
        if (pDocObj)
        {
            if ( pOldDPObject )
            {
                //  restore old settings
                pOldDPObject->WriteSourceDataTo( *pDocObj );
                ScDPSaveData* pData = pOldDPObject->GetSaveData();
                if (pData)
                    pDocObj->SetSaveData(*pData);
                pDocObj->SetOutRange( pOldDPObject->GetOutRange() );
                pOldDPObject->WriteTempDataTo( *pDocObj );
            }
            else
            {
                //  delete inserted object
                pDoc->GetDPCollection()->FreeTable(pDocObj);
            }
        }
    }
    else if ( pOldDPObject )
    {
        //  re-insert deleted object

        ScDPObject* pDestObj = new ScDPObject( *pOldDPObject );
        pDestObj->SetAlive(TRUE);
        if ( !pDoc->GetDPCollection()->InsertNewTable(pDestObj) )
        {
            DBG_ERROR("cannot insert DPObject");
            DELETEZ( pDestObj );
        }
    }

    if (pNewUndoDoc)
        pDocShell->PostPaint( aNewRange, PAINT_GRID, SC_PF_LINES );
    if (pOldUndoDoc)
        pDocShell->PostPaint( aOldRange, PAINT_GRID, SC_PF_LINES );
    pDocShell->PostDataChanged();

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    if (pViewShell)
    {
        //! set current sheet
    }

    if (pNewDPObject)
    {
        // notify API objects
        pDoc->BroadcastUno( ScDataPilotModifiedHint( pNewDPObject->GetName() ) );
    }

    EndUndo();
}

void __EXPORT ScUndoDataPilot::Redo()
{
    BeginRedo();

    //! copy output data instead of repeating the change,
    //! in case external data have changed!

    ScDocument* pDoc = pDocShell->GetDocument();

    ScDPObject* pSourceObj = NULL;
    if ( pOldDPObject )
    {
        //  find object to modify
        //! find by name!

        ScRange aOldRange = pOldDPObject->GetOutRange();
        pSourceObj = pDoc->GetDPAtCursor(
                        aOldRange.aStart.Col(), aOldRange.aStart.Row(), aOldRange.aStart.Tab() );
        DBG_ASSERT(pSourceObj, "DPObject not found");
    }

    ScDBDocFunc aFunc( *pDocShell );
    aFunc.DataPilotUpdate( pSourceObj, pNewDPObject, FALSE, FALSE, bAllowMove );    // no new undo action

    EndRedo();
}

void __EXPORT ScUndoDataPilot::Repeat(SfxRepeatTarget& /* rTarget */)
{
    //! allow deletion
}

BOOL __EXPORT ScUndoDataPilot::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    //! allow deletion
    return FALSE;
}


//
//      Konsolidieren
//

ScUndoConsolidate::ScUndoConsolidate( ScDocShell* pNewDocShell, const ScArea& rArea,
                    const ScConsolidateParam& rPar, ScDocument* pNewUndoDoc,
                    BOOL bReference, SCROW nInsCount, ScOutlineTable* pTab,
                    ScDBData* pData ) :
    ScSimpleUndo( pNewDocShell ),
    aDestArea( rArea ),
    pUndoDoc( pNewUndoDoc ),
    aParam( rPar ),
    bInsRef( bReference ),
    nInsertCount( nInsCount ),
    pUndoTab( pTab ),
    pUndoData( pData )
{
}

__EXPORT ScUndoConsolidate::~ScUndoConsolidate()
{
    delete pUndoDoc;
    delete pUndoTab;
    delete pUndoData;
}

String __EXPORT ScUndoConsolidate::GetComment() const
{
    return ScGlobal::GetRscString( STR_UNDO_CONSOLIDATE );
}

void __EXPORT ScUndoConsolidate::Undo()
{
    BeginUndo();

    ScDocument* pDoc = pDocShell->GetDocument();
    SCTAB nTab = aDestArea.nTab;

    ScRange aOldRange;
    if (pUndoData)
        pUndoData->GetArea(aOldRange);

    if (bInsRef)
    {
        //  Zeilen loeschen
        pDoc->DeleteRow( 0,nTab, MAXCOL,nTab, aDestArea.nRowStart, nInsertCount );

        //  Outlines
        pDoc->SetOutlineTable( nTab, pUndoTab );

        //  Zeilenstatus
        pUndoDoc->CopyToDocument( 0,0,nTab, MAXCOL,MAXROW,nTab, IDF_NONE, FALSE, pDoc );

        //  Daten & Referenzen
        pDoc->DeleteAreaTab( 0,aDestArea.nRowStart, MAXCOL,aDestArea.nRowEnd, nTab, IDF_ALL );
        pUndoDoc->UndoToDocument( 0, aDestArea.nRowStart, nTab,
                                    MAXCOL, aDestArea.nRowEnd, nTab,
                                    IDF_ALL, FALSE, pDoc );

        //  Original-Bereich
        if (pUndoData)
        {
            pDoc->DeleteAreaTab(aOldRange, IDF_ALL);
            pUndoDoc->CopyToDocument(aOldRange, IDF_ALL, FALSE, pDoc);
        }

        pDocShell->PostPaint( 0,aDestArea.nRowStart,nTab, MAXCOL,MAXROW,nTab,
                                PAINT_GRID | PAINT_LEFT | PAINT_SIZE );
    }
    else
    {
        pDoc->DeleteAreaTab( aDestArea.nColStart,aDestArea.nRowStart,
                                aDestArea.nColEnd,aDestArea.nRowEnd, nTab, IDF_ALL );
        pUndoDoc->CopyToDocument( aDestArea.nColStart, aDestArea.nRowStart, nTab,
                                    aDestArea.nColEnd, aDestArea.nRowEnd, nTab,
                                    IDF_ALL, FALSE, pDoc );

        //  Original-Bereich
        if (pUndoData)
        {
            pDoc->DeleteAreaTab(aOldRange, IDF_ALL);
            pUndoDoc->CopyToDocument(aOldRange, IDF_ALL, FALSE, pDoc);
        }

        SCCOL nEndX = aDestArea.nColEnd;
        SCROW nEndY = aDestArea.nRowEnd;
        if ( pUndoData )
        {
            if ( aOldRange.aEnd.Col() > nEndX )
                nEndX = aOldRange.aEnd.Col();
            if ( aOldRange.aEnd.Row() > nEndY )
                nEndY = aOldRange.aEnd.Row();
        }
        pDocShell->PostPaint( aDestArea.nColStart, aDestArea.nRowStart, nTab,
                                    nEndX, nEndY, nTab, PAINT_GRID );
    }

    //  DB-Bereich wieder anpassen
    if (pUndoData)
    {
        ScDBCollection* pColl = pDoc->GetDBCollection();
        if (pColl)
        {
            USHORT nIndex;
            if (pColl->SearchName( pUndoData->GetName(), nIndex ))
            {
                ScDBData* pDocData = (*pColl)[nIndex];
                if (pDocData)
                    *pDocData = *pUndoData;
            }
            else
            {
                DBG_ERROR("alte DB-Daten nicht gefunden");
            }
        }
    }

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    if (pViewShell)
    {
        SCTAB nViewTab = pViewShell->GetViewData()->GetTabNo();
        if ( nViewTab != nTab )
            pViewShell->SetTabNo( nTab );
    }

    EndUndo();
}

void __EXPORT ScUndoConsolidate::Redo()
{
    BeginRedo();

    pDocShell->DoConsolidate( aParam, FALSE );

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
    if (pViewShell)
    {
        SCTAB nViewTab = pViewShell->GetViewData()->GetTabNo();
        if ( nViewTab != aParam.nTab )
            pViewShell->SetTabNo( aParam.nTab );
    }

    EndRedo();
}

void __EXPORT ScUndoConsolidate::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL __EXPORT ScUndoConsolidate::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;
}


//
//      Quell-Daten von Chart aendern
//

void ScUndoChartData::Init()
{
    ScDocument* pDoc = pDocShell->GetDocument();
    aOldRangeListRef = new ScRangeList;
    pDoc->GetOldChartParameters( aChartName, *aOldRangeListRef, bOldColHeaders, bOldRowHeaders );
}

ScUndoChartData::ScUndoChartData( ScDocShell* pNewDocShell, const String& rName,
                                    const ScRange& rNew, BOOL bColHdr, BOOL bRowHdr,
                                    BOOL bAdd ) :
    ScSimpleUndo( pNewDocShell ),
    aChartName( rName ),
    bNewColHeaders( bColHdr ),
    bNewRowHeaders( bRowHdr ),
    bAddRange( bAdd )
{
    aNewRangeListRef = new ScRangeList;
    aNewRangeListRef->Append( rNew );

    Init();
}

ScUndoChartData::ScUndoChartData( ScDocShell* pNewDocShell, const String& rName,
                                    const ScRangeListRef& rNew, BOOL bColHdr, BOOL bRowHdr,
                                    BOOL bAdd ) :
    ScSimpleUndo( pNewDocShell ),
    aChartName( rName ),
    aNewRangeListRef( rNew ),
    bNewColHeaders( bColHdr ),
    bNewRowHeaders( bRowHdr ),
    bAddRange( bAdd )
{
    Init();
}

__EXPORT ScUndoChartData::~ScUndoChartData()
{
}

String __EXPORT ScUndoChartData::GetComment() const
{
    return ScGlobal::GetRscString( STR_UNDO_CHARTDATA );
}

void __EXPORT ScUndoChartData::Undo()
{
    BeginUndo();

    pDocShell->GetDocument()->UpdateChartArea( aChartName, aOldRangeListRef,
                                bOldColHeaders, bOldRowHeaders, FALSE );

    EndUndo();
}

void __EXPORT ScUndoChartData::Redo()
{
    BeginRedo();

    pDocShell->GetDocument()->UpdateChartArea( aChartName, aNewRangeListRef,
                                bNewColHeaders, bNewRowHeaders, bAddRange );

    EndRedo();
}

void __EXPORT ScUndoChartData::Repeat(SfxRepeatTarget& /* rTarget */)
{
}

BOOL __EXPORT ScUndoChartData::CanRepeat(SfxRepeatTarget& /* rTarget */) const
{
    return FALSE;
}

// Amelia Wang
ScUndoDataForm::ScUndoDataForm( ScDocShell* pNewDocShell,
                                SCCOL nStartX, SCROW nStartY, SCTAB nStartZ,
                                SCCOL nEndX, SCROW nEndY, SCTAB nEndZ,
                                const ScMarkData& rMark,
                                ScDocument* pNewUndoDoc, ScDocument* pNewRedoDoc,
                                USHORT nNewFlags,
                                ScRefUndoData* pRefData,
                                void* /*pFill1*/, void* /*pFill2*/, void* /*pFill3*/,
                                BOOL bRedoIsFilled ) :
        ScBlockUndo( pNewDocShell, ScRange( nStartX, nStartY, nStartZ, nEndX, nEndY, nEndZ ), SC_UNDO_SIMPLE ),
        aMarkData( rMark ),
        pUndoDoc( pNewUndoDoc ),
        pRedoDoc( pNewRedoDoc ),
        nFlags( nNewFlags ),
        pRefUndoData( pRefData ),
        pRefRedoData( NULL ),
        bRedoFilled( bRedoIsFilled )
{
        //      pFill1,pFill2,pFill3 are there so the ctor calls for simple paste (without cutting)
        //      don't have to be changed and branched for 641.
        //      They can be removed later.

        if ( !aMarkData.IsMarked() )                            // no cell marked:
                aMarkData.SetMarkArea( aBlockRange );   //  mark paste block

        if ( pRefUndoData )
                pRefUndoData->DeleteUnchanged( pDocShell->GetDocument() );

        SetChangeTrack();
}

ScUndoDataForm::~ScUndoDataForm()
{
        delete pUndoDoc;
        delete pRedoDoc;
        delete pRefUndoData;
        delete pRefRedoData;
}

String ScUndoDataForm::GetComment() const
{
        return ScGlobal::GetRscString( STR_UNDO_PASTE );
}

void ScUndoDataForm::SetChangeTrack()
{
        ScChangeTrack* pChangeTrack = pDocShell->GetDocument()->GetChangeTrack();
        if ( pChangeTrack && (nFlags & IDF_CONTENTS) )
                pChangeTrack->AppendContentRange( aBlockRange, pUndoDoc,
                        nStartChangeAction, nEndChangeAction, SC_CACM_PASTE );
        else
                nStartChangeAction = nEndChangeAction = 0;
}


void ScUndoDataForm::Undo()
{
        BeginUndo();
        DoChange( TRUE );
        ShowTable( aBlockRange );
        EndUndo();
        SFX_APP()->Broadcast( SfxSimpleHint( SC_HINT_AREALINKS_CHANGED ) );
}

void ScUndoDataForm::Redo()
{
        BeginRedo();
        ScDocument* pDoc = pDocShell->GetDocument();
        EnableDrawAdjust( pDoc, FALSE );                                //! include in ScBlockUndo?
        DoChange( FALSE );
        EnableDrawAdjust( pDoc, TRUE );                                 //! include in ScBlockUndo?
        EndRedo();
        SFX_APP()->Broadcast( SfxSimpleHint( SC_HINT_AREALINKS_CHANGED ) );
}

void ScUndoDataForm::Repeat(SfxRepeatTarget& /*rTarget*/)
{
}

BOOL ScUndoDataForm::CanRepeat(SfxRepeatTarget& rTarget) const
{
        return (rTarget.ISA(ScTabViewTarget));
}

void ScUndoDataForm::DoChange( const BOOL bUndo )
{
    ScDocument* pDoc = pDocShell->GetDocument();

    //      RefUndoData for redo is created before first undo
    //      (with DeleteUnchanged after the DoUndo call)
    BOOL bCreateRedoData = ( bUndo && pRefUndoData && !pRefRedoData );
    if ( bCreateRedoData )
            pRefRedoData = new ScRefUndoData( pDoc );

    ScRefUndoData* pWorkRefData = bUndo ? pRefUndoData : pRefRedoData;

    //      fuer Undo immer alle oder keine Inhalte sichern
    USHORT nUndoFlags = IDF_NONE;
    if (nFlags & IDF_CONTENTS)
            nUndoFlags |= IDF_CONTENTS;
    if (nFlags & IDF_ATTRIB)
            nUndoFlags |= IDF_ATTRIB;

    BOOL bPaintAll = FALSE;

    ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();

    // marking is in ScBlockUndo...
    //ScUndoUtil::MarkSimpleBlock( pDocShell, aBlockRange );

    SCTAB nTabCount = pDoc->GetTableCount();
    if ( bUndo && !bRedoFilled )
    {
        if (!pRedoDoc)
        {
            BOOL bColInfo = ( aBlockRange.aStart.Row()==0 && aBlockRange.aEnd.Row()==MAXROW );
            BOOL bRowInfo = ( aBlockRange.aStart.Col()==0 && aBlockRange.aEnd.Col()==MAXCOL );

            pRedoDoc = new ScDocument( SCDOCMODE_UNDO );
            pRedoDoc->InitUndoSelected( pDoc, aMarkData, bColInfo, bRowInfo );
        }
        //  read "redo" data from the document in the first undo
            //  all sheets - CopyToDocument skips those that don't exist in pRedoDoc
        ScRange aCopyRange = aBlockRange;
        aCopyRange.aStart.SetTab(0);
        aCopyRange.aEnd.SetTab(nTabCount-1);
        pDoc->CopyToDocument( aCopyRange, 1, FALSE, pRedoDoc );
        bRedoFilled = TRUE;
    }

    USHORT nExtFlags = 0;
    pDocShell->UpdatePaintExt( nExtFlags, aBlockRange );

    for ( sal_uInt16 i=0; i <= ( aBlockRange.aEnd.Col() - aBlockRange.aStart.Col() ); i++ )
    {
        String aOldString;
        pUndoDoc->GetString( aBlockRange.aStart.Col()+i , aBlockRange.aStart.Row() , aBlockRange.aStart.Tab() , aOldString );
        pDoc->SetString( aBlockRange.aStart.Col()+i , aBlockRange.aStart.Row() , aBlockRange.aStart.Tab() , aOldString );
    }

    //ScRange aTabSelectRange = aBlockRange;

    if (pWorkRefData)
    {
        pWorkRefData->DoUndo( pDoc, TRUE );             // TRUE = bSetChartRangeLists for SetChartListenerCollection
        if ( pDoc->RefreshAutoFilter( 0,0, MAXCOL,MAXROW, aBlockRange.aStart.Tab() ) )
            bPaintAll = TRUE;
    }

    if ( bCreateRedoData && pRefRedoData )
            pRefRedoData->DeleteUnchanged( pDoc );

    if ( bUndo )
    {
        ScChangeTrack* pChangeTrack = pDoc->GetChangeTrack();
        if ( pChangeTrack )
            pChangeTrack->Undo( nStartChangeAction, nEndChangeAction );
    }
    else
        SetChangeTrack();

    ScRange aDrawRange( aBlockRange );
    pDoc->ExtendMerge( aDrawRange, TRUE );      // only needed for single sheet (text/rtf etc.)
    USHORT nPaint = PAINT_GRID;
    if (bPaintAll)
    {
        aDrawRange.aStart.SetCol(0);
        aDrawRange.aStart.SetRow(0);
        aDrawRange.aEnd.SetCol(MAXCOL);
        aDrawRange.aEnd.SetRow(MAXROW);
        nPaint |= PAINT_TOP | PAINT_LEFT;
/*A*/   if (pViewShell)
            pViewShell->AdjustBlockHeight(FALSE);
    }
    else
    {
        if ( aBlockRange.aStart.Row() == 0 && aBlockRange.aEnd.Row() == MAXROW )        // ganze Spalte
        {
            nPaint |= PAINT_TOP;
            aDrawRange.aEnd.SetCol(MAXCOL);
        }
        if ( aBlockRange.aStart.Col() == 0 && aBlockRange.aEnd.Col() == MAXCOL )        // ganze Zeile
        {
            nPaint |= PAINT_LEFT;
            aDrawRange.aEnd.SetRow(MAXROW);
        }
/*A*/   if ((pViewShell) && pViewShell->AdjustBlockHeight(FALSE))
        {
            aDrawRange.aStart.SetCol(0);
            aDrawRange.aStart.SetRow(0);
            aDrawRange.aEnd.SetCol(MAXCOL);
            aDrawRange.aEnd.SetRow(MAXROW);
            nPaint |= PAINT_LEFT;
        }
        pDocShell->UpdatePaintExt( nExtFlags, aDrawRange );
    }

    if ( !bUndo )                               //      draw redo after updating row heights
        RedoSdrUndoAction( pDrawUndo );                 //!     include in ScBlockUndo?

    pDocShell->PostPaint( aDrawRange, nPaint, nExtFlags );

    pDocShell->PostDataChanged();
    if (pViewShell)
        pViewShell->CellContentChanged();
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
