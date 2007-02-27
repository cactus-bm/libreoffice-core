/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tabview3.cxx,v $
 *
 *  $Revision: 1.52 $
 *
 *  last change: $Author: vg $ $Date: 2007-02-27 13:57:52 $
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

// System - Includes -----------------------------------------------------



// INCLUDE ---------------------------------------------------------------
#include <rangelst.hxx>
#include "scitems.hxx"
#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD

#include <svx/brshitem.hxx>
#include <svx/editview.hxx>
#include <svx/fmshell.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/viewfrm.hxx>
#include <vcl/cursor.hxx>
#include <sch/schdll.hxx>       // ChartSelectionInfo
#include <sch/memchrt.hxx>

#include "tabview.hxx"
#include "tabvwsh.hxx"
#include "docsh.hxx"
#include "gridwin.hxx"
#include "olinewin.hxx"
#include "colrowba.hxx"
#include "tabcont.hxx"
#include "scmod.hxx"
#include "uiitems.hxx"
#include "sc.hrc"
#include "viewutil.hxx"
#include "editutil.hxx"
#include "inputhdl.hxx"
#include "inputwin.hxx"
#include "validat.hxx"
#include "hintwin.hxx"
#include "inputopt.hxx"
#include "rfindlst.hxx"
#include "hiranges.hxx"
#include "viewuno.hxx"
#include "chartarr.hxx"
#include "anyrefdg.hxx"
#include "dpobject.hxx"
#include "patattr.hxx"
#include "dociter.hxx"
#include "seltrans.hxx"
#include "fillinfo.hxx"
#include "AccessibilityHints.hxx"

using namespace com::sun::star;

// -----------------------------------------------------------------------

//  helper class for DoChartSelection

#define SC_BGCOLLECT_FIRST      0
#define SC_BGCOLLECT_FOUND      1
#define SC_BGCOLLECT_AMBIGUOUS  2

class ScBackgroundCollector
{
private:
    ScDocument* pDoc;
    BOOL        bTransparent;
    Color       aBackColor;
    USHORT      nMode;
public:
            ScBackgroundCollector( ScDocument* pD );
    void    AddRange( const ScRange& rRange );
    Color   GetHighlightColor() const;
};

ScBackgroundCollector::ScBackgroundCollector( ScDocument* pD ) :
    pDoc( pD ),
    bTransparent( FALSE ),
    nMode( SC_BGCOLLECT_FIRST )
{
}

void ScBackgroundCollector::AddRange( const ScRange& rRange )
{
    if ( nMode == SC_BGCOLLECT_AMBIGUOUS )
        return;                                 // nothing more to do

    ScDocAttrIterator aIter( pDoc, rRange.aStart.Tab(),
                            rRange.aStart.Col(), rRange.aStart.Row(),
                            rRange.aEnd.Col(), rRange.aEnd.Row() );
    SCCOL nCol;
    SCROW nRow1, nRow2;
    const ScPatternAttr* pPattern = aIter.GetNext( nCol, nRow1, nRow2 );
    while ( pPattern )
    {
        //! look at conditional formats?
        const Color& rAttrColor = ((const SvxBrushItem&)pPattern->GetItem(ATTR_BACKGROUND)).GetColor();
        BOOL bAttrTransp = ( rAttrColor.GetTransparency() != 0 );

        if ( nMode == SC_BGCOLLECT_FIRST )
        {
            //  just copy first background
            bTransparent = bAttrTransp;
            aBackColor   = rAttrColor;
            nMode = SC_BGCOLLECT_FOUND;
        }
        else if ( nMode == SC_BGCOLLECT_FOUND )
        {
            BOOL bEqual = ( bTransparent == bAttrTransp );
            if ( bEqual && !bTransparent )
                bEqual = ( aBackColor == rAttrColor );
            if ( !bEqual )
            {
                nMode = SC_BGCOLLECT_AMBIGUOUS;     // different backgrounds found
                return;                             // dont need to continue
            }
        }

        pPattern = aIter.GetNext( nCol, nRow1, nRow2 );
    }
}

Color ScBackgroundCollector::GetHighlightColor() const
{
    if ( nMode == SC_BGCOLLECT_FOUND && !bTransparent )
    {
        //  everything formatted with a single background color
        //  -> use contrasting color (blue or yellow)

        Color aBlue( COL_LIGHTBLUE );
        Color aYellow( COL_YELLOW );

        if ( aBackColor.GetColorError(aBlue) >= aBackColor.GetColorError(aYellow) )
            return aBlue;
        else
            return aYellow;
    }
    else
        return Color( COL_LIGHTBLUE );      // default for transparent or ambiguous background
}

// -----------------------------------------------------------------------

//
// ---  Public-Funktionen
//

void ScTabView::ClickCursor( SCCOL nPosX, SCROW nPosY, BOOL bControl )
{
    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();
    while (pDoc->IsHorOverlapped( nPosX, nPosY, nTab ))     //! ViewData !!!
        --nPosX;
    while (pDoc->IsVerOverlapped( nPosX, nPosY, nTab ))
        --nPosY;

    BOOL bRefMode = SC_MOD()->IsFormulaMode();

    if ( bRefMode )
    {
        DoneRefMode( FALSE );

        if (bControl)
            SC_MOD()->AddRefEntry();

        InitRefMode( nPosX, nPosY, nTab, SC_REFTYPE_REF );
    }
    else
    {
        DoneBlockMode( bControl );
        aViewData.ResetOldCursor();
        SetCursor( (SCCOL) nPosX, (SCROW) nPosY );
    }
}

void ScTabView::UpdateAutoFillMark()
{
    ScRange aMarkRange;
    BOOL bMarked = aViewData.GetSimpleArea( aMarkRange );       // single selection or cursor

    USHORT i;
    for (i=0; i<4; i++)
        if (pGridWin[i] && pGridWin[i]->IsVisible())
            pGridWin[i]->UpdateAutoFillMark( bMarked, aMarkRange );

    for (i=0; i<2; i++)
    {
        if (pColBar[i] && pColBar[i]->IsVisible())
            pColBar[i]->SetMark( bMarked, aMarkRange.aStart.Col(), aMarkRange.aEnd.Col() );
        if (pRowBar[i] && pRowBar[i]->IsVisible())
            pRowBar[i]->SetMark( bMarked, aMarkRange.aStart.Row(), aMarkRange.aEnd.Row() );
    }

    //  selection transfer object is checked together with AutoFill marks,
    //  because it has the same requirement of a single continuous block.
    CheckSelectionTransfer();   // update selection transfer object
}

void ScTabView::FakeButtonUp( ScSplitPos eWhich )
{
    if (pGridWin[eWhich])
        pGridWin[eWhich]->FakeButtonUp();
}

void ScTabView::HideAllCursors()
{
    for (USHORT i=0; i<4; i++)
        if (pGridWin[i])
            if (pGridWin[i]->IsVisible())
            {
                Cursor* pCur = pGridWin[i]->GetCursor();
                if (pCur)
                    if (pCur->IsVisible())
                        pCur->Hide();
                pGridWin[i]->HideCursor();
            }
}

void ScTabView::ShowAllCursors()
{
    for (USHORT i=0; i<4; i++)
        if (pGridWin[i])
            if (pGridWin[i]->IsVisible())
            {
                pGridWin[i]->ShowCursor();

                // #114409#
                pGridWin[i]->CursorChanged();
            }
}

void ScTabView::HideCursor()
{
    pGridWin[aViewData.GetActivePart()]->HideCursor();
}

void ScTabView::ShowCursor()
{
    pGridWin[aViewData.GetActivePart()]->ShowCursor();

    // #114409#
    pGridWin[aViewData.GetActivePart()]->CursorChanged();
}

void ScTabView::InvalidateAttribs()
{
    SfxBindings& rBindings = aViewData.GetBindings();

    rBindings.Invalidate( SID_STYLE_APPLY );
    rBindings.Invalidate( SID_STYLE_FAMILY2 );
    // StarCalc kennt nur Absatz- bzw. Zellformat-Vorlagen

    rBindings.Invalidate( SID_ATTR_CHAR_FONT );
    rBindings.Invalidate( SID_ATTR_CHAR_FONTHEIGHT );
    rBindings.Invalidate( SID_ATTR_CHAR_COLOR );

    rBindings.Invalidate( SID_ATTR_CHAR_WEIGHT );
    rBindings.Invalidate( SID_ATTR_CHAR_POSTURE );
    rBindings.Invalidate( SID_ATTR_CHAR_UNDERLINE );
    rBindings.Invalidate( SID_ULINE_VAL_NONE );
    rBindings.Invalidate( SID_ULINE_VAL_SINGLE );
    rBindings.Invalidate( SID_ULINE_VAL_DOUBLE );
    rBindings.Invalidate( SID_ULINE_VAL_DOTTED );

    rBindings.Invalidate( SID_ALIGNLEFT );
    rBindings.Invalidate( SID_ALIGNRIGHT );
    rBindings.Invalidate( SID_ALIGNBLOCK );
    rBindings.Invalidate( SID_ALIGNCENTERHOR );

    rBindings.Invalidate( SID_ALIGNTOP );
    rBindings.Invalidate( SID_ALIGNBOTTOM );
    rBindings.Invalidate( SID_ALIGNCENTERVER );

    rBindings.Invalidate( SID_BACKGROUND_COLOR );

    rBindings.Invalidate( SID_ATTR_ALIGN_LINEBREAK );
    rBindings.Invalidate( SID_NUMBER_FORMAT );

    rBindings.Invalidate( SID_TEXTDIRECTION_LEFT_TO_RIGHT );
    rBindings.Invalidate( SID_TEXTDIRECTION_TOP_TO_BOTTOM );
    rBindings.Invalidate( SID_ATTR_PARA_LEFT_TO_RIGHT );
    rBindings.Invalidate( SID_ATTR_PARA_RIGHT_TO_LEFT );

    // pseudo slots for Format menu
    rBindings.Invalidate( SID_ALIGN_ANY_HDEFAULT );
    rBindings.Invalidate( SID_ALIGN_ANY_LEFT );
    rBindings.Invalidate( SID_ALIGN_ANY_HCENTER );
    rBindings.Invalidate( SID_ALIGN_ANY_RIGHT );
    rBindings.Invalidate( SID_ALIGN_ANY_JUSTIFIED );
    rBindings.Invalidate( SID_ALIGN_ANY_VDEFAULT );
    rBindings.Invalidate( SID_ALIGN_ANY_TOP );
    rBindings.Invalidate( SID_ALIGN_ANY_VCENTER );
    rBindings.Invalidate( SID_ALIGN_ANY_BOTTOM );

//  rBindings.Invalidate( SID_RANGE_VALUE );
//  rBindings.Invalidate( SID_RANGE_FORMULA );
}

//      SetCursor - Cursor setzen, zeichnen, InputWin updaten
//                  oder Referenz verschicken
//      ohne Optimierung wegen BugId 29307

#ifdef WNT
#pragma optimize ( "", off )
#endif

void ScTabView::SetCursor( SCCOL nPosX, SCROW nPosY, BOOL bNew )
{
    SCCOL nOldX = aViewData.GetCurX();
    SCROW nOldY = aViewData.GetCurY();

    //  DeactivateIP nur noch bei MarkListHasChanged

    if ( nPosX != nOldX || nPosY != nOldY || bNew )
    {
        if ( aViewData.HasEditView(aViewData.GetActivePart()) &&
                        !SC_MOD()->IsFormulaMode() )                    // 23259 oder so
            UpdateInputLine();

        HideAllCursors();

        aViewData.SetCurX( nPosX );
        aViewData.SetCurY( nPosY );

        ShowAllCursors();

        CursorPosChanged();
    }
}

#ifdef WNT
#pragma optimize ( "", on )
#endif

void ScTabView::CheckSelectionTransfer()
{
    if ( aViewData.IsActive() )     // only for active view
    {
        ScModule* pScMod = SC_MOD();
        ScSelectionTransferObj* pOld = pScMod->GetSelectionTransfer();
        if ( pOld && pOld->GetView() == this && pOld->StillValid() )
        {
            // selection not changed - nothing to do
        }
        else
        {
            ScSelectionTransferObj* pNew = ScSelectionTransferObj::CreateFromView( this );
            if ( pNew )
            {
                //  create new selection

                if (pOld)
                    pOld->ForgetView();

                uno::Reference<datatransfer::XTransferable> xRef( pNew );
                pScMod->SetSelectionTransfer( pNew );
                pNew->CopyToSelection( GetActiveWin() );                    // may delete pOld
            }
            else if ( pOld && pOld->GetView() == this )
            {
                //  remove own selection

                pOld->ForgetView();
                pScMod->SetSelectionTransfer( NULL );
                TransferableHelper::ClearSelection( GetActiveWin() );       // may delete pOld
            }
            // else: selection from outside: leave unchanged
        }
    }
}

// Eingabezeile / Menues updaten
//  CursorPosChanged ruft SelectionChanged
//  SelectionChanged ruft CellContentChanged

void ScTabView::CellContentChanged()
{
    SfxBindings& rBindings = aViewData.GetBindings();

    rBindings.Invalidate( SID_ATTR_SIZE );      // -> Fehlermeldungen anzeigen
    rBindings.Invalidate( SID_THESAURUS );
    rBindings.Invalidate( SID_HYPERLINK_GETLINK );

    InvalidateAttribs();                    // Attribut-Updates
    TestHintWindow();                       // Eingabemeldung (Gueltigkeit)

    aViewData.GetViewShell()->UpdateInputHandler();
}

void ScTabView::SelectionChanged()
{
    SfxViewFrame* pViewFrame = aViewData.GetViewShell()->GetViewFrame();
    if (pViewFrame)
    {
        SfxFrame* pFrame = pViewFrame->GetFrame();
        if (pFrame)
        {
            uno::Reference<frame::XController> xController = pFrame->GetController();
            if (xController.is())
            {
                ScTabViewObj* pImp = ScTabViewObj::getImplementation( xController );
                if (pImp)
                    pImp->SelectionChanged();
            }
        }
    }

    UpdateAutoFillMark();   // also calls CheckSelectionTransfer

    SfxBindings& rBindings = aViewData.GetBindings();

    rBindings.Invalidate( SID_CURRENTCELL );    // -> Navigator
    rBindings.Invalidate( SID_AUTO_FILTER );    // -> Menue
    rBindings.Invalidate( FID_NOTE_VISIBLE );
    rBindings.Invalidate( SID_DELETE_NOTE );

        //  Funktionen, die evtl disabled werden muessen

    rBindings.Invalidate( FID_INS_ROWBRK );
    rBindings.Invalidate( FID_INS_COLBRK );
    rBindings.Invalidate( FID_DEL_ROWBRK );
    rBindings.Invalidate( FID_DEL_COLBRK );
    rBindings.Invalidate( FID_MERGE_ON );
    rBindings.Invalidate( FID_MERGE_OFF );
    rBindings.Invalidate( FID_MERGE_TOGGLE );
    rBindings.Invalidate( SID_AUTOFILTER_HIDE );
    rBindings.Invalidate( SID_UNFILTER );
//  rBindings.Invalidate( SID_IMPORT_DATA );        // jetzt wieder immer moeglich
    rBindings.Invalidate( SID_REIMPORT_DATA );
    rBindings.Invalidate( SID_REFRESH_DBAREA );
    rBindings.Invalidate( SID_OUTLINE_SHOW );
    rBindings.Invalidate( SID_OUTLINE_HIDE );
    rBindings.Invalidate( SID_OUTLINE_REMOVE );
    rBindings.Invalidate( FID_FILL_TO_BOTTOM );
    rBindings.Invalidate( FID_FILL_TO_RIGHT );
    rBindings.Invalidate( FID_FILL_TO_TOP );
    rBindings.Invalidate( FID_FILL_TO_LEFT );
    rBindings.Invalidate( FID_FILL_SERIES );
    rBindings.Invalidate( SID_SCENARIOS );
    rBindings.Invalidate( SID_AUTOFORMAT );
    rBindings.Invalidate( SID_OPENDLG_TABOP );
    rBindings.Invalidate( SID_DATA_SELECT );

    rBindings.Invalidate( SID_CUT );
    rBindings.Invalidate( SID_COPY );
    rBindings.Invalidate( SID_PASTE );
    rBindings.Invalidate( FID_PASTE_CONTENTS );

    rBindings.Invalidate( FID_INS_ROW );
    rBindings.Invalidate( FID_INS_COLUMN );
    rBindings.Invalidate( FID_INS_CELL );
    rBindings.Invalidate( FID_INS_CELLSDOWN );
    rBindings.Invalidate( FID_INS_CELLSRIGHT );

    rBindings.Invalidate( FID_CHG_COMMENT );

        //  nur wegen Zellschutz:

    rBindings.Invalidate( SID_CELL_FORMAT_RESET );
    rBindings.Invalidate( SID_DELETE );
    rBindings.Invalidate( SID_DELETE_CONTENTS );
    rBindings.Invalidate( FID_DELETE_CELL );
    rBindings.Invalidate( FID_CELL_FORMAT );
    rBindings.Invalidate( SID_ENABLE_HYPHENATION );
    rBindings.Invalidate( SID_INSERT_POSTIT );
    rBindings.Invalidate( SID_CHARMAP );
    rBindings.Invalidate( SID_OPENDLG_FUNCTION );
//  rBindings.Invalidate( FID_CONDITIONAL_FORMAT );
    rBindings.Invalidate( SID_OPENDLG_CONDFRMT );
    rBindings.Invalidate( FID_VALIDATION );
    rBindings.Invalidate( SID_EXTERNAL_SOURCE );

    if (aViewData.GetViewShell()->HasAccessibilityObjects())
        aViewData.GetViewShell()->BroadcastAccessibility(SfxSimpleHint(SC_HINT_ACC_CURSORCHANGED));

    CellContentChanged();
}

void ScTabView::CursorPosChanged()
{
    BOOL bRefMode = SC_MOD()->IsFormulaMode();
    if ( !bRefMode ) // Abfrage, damit RefMode bei Tabellenwechsel funktioniert
        aViewData.GetDocShell()->Broadcast( SfxSimpleHint( FID_KILLEDITVIEW ) );

    //  Broadcast, damit andere Views des Dokuments auch umschalten

    ScDocument* pDoc = aViewData.GetDocument();
    BOOL bPivot = ( NULL != pDoc->GetPivotAtCursor( aViewData.GetCurX(),
                                                    aViewData.GetCurY(),
                                                    aViewData.GetTabNo() ) ||
                    NULL != pDoc->GetDPAtCursor( aViewData.GetCurX(),
                                                    aViewData.GetCurY(),
                                                    aViewData.GetTabNo() ) );
    aViewData.GetViewShell()->SetPivotShell(bPivot);

    //  UpdateInputHandler jetzt in CellContentChanged

    SelectionChanged();

    aViewData.SetTabStartCol( SC_TABSTART_NONE );
}

void ScTabView::TestHintWindow()
{
    //  show input help window and list drop-down button for validity

    BOOL bListValButton = FALSE;
    ScAddress aListValPos;

    ScDocument* pDoc = aViewData.GetDocument();
    const SfxUInt32Item* pItem = (const SfxUInt32Item*)
                                        pDoc->GetAttr( aViewData.GetCurX(),
                                                       aViewData.GetCurY(),
                                                       aViewData.GetTabNo(),
                                                       ATTR_VALIDDATA );
    if ( pItem->GetValue() )
    {
        const ScValidationData* pData = pDoc->GetValidationEntry( pItem->GetValue() );
        DBG_ASSERT(pData,"ValidationData nicht gefunden");
        String aTitle, aMessage;
        if ( pData && pData->GetInput( aTitle, aMessage ) )
        {
            //! Abfrage, ob an gleicher Stelle !!!!

            DELETEZ(pInputHintWindow);

            ScSplitPos eWhich = aViewData.GetActivePart();
            Window* pWin = pGridWin[eWhich];
            SCCOL nCol = aViewData.GetCurX();
            SCROW nRow = aViewData.GetCurY();
            Point aPos = aViewData.GetScrPos( nCol, nRow, eWhich );
            Size aWinSize = pWin->GetOutputSizePixel();
            //  Cursor sichtbar?
            if ( nCol >= aViewData.GetPosX(WhichH(eWhich)) &&
                 nRow >= aViewData.GetPosY(WhichV(eWhich)) &&
                 aPos.X() < aWinSize.Width() && aPos.Y() < aWinSize.Height() )
            {
                aPos += pWin->GetPosPixel();                                // Position auf Frame
                long nSizeXPix;
                long nSizeYPix;
                aViewData.GetMergeSizePixel( nCol, nRow, nSizeXPix, nSizeYPix );

                // HintWindow anlegen, bestimmt seine Groesse selbst
                pInputHintWindow = new ScHintWindow( pFrameWin, aTitle, aMessage );
                Size aHintSize = pInputHintWindow->GetSizePixel();
                Size aFrameWinSize = pFrameWin->GetOutputSizePixel();

                // passende Position finden
                //  erster Versuch: unter dem Cursor
                Point aHintPos( aPos.X() + nSizeXPix / 2, aPos.Y() + nSizeYPix + 3 );
                if ( aHintPos.Y() + aHintSize.Height() > aFrameWinSize.Height() )
                {
                    // zweiter Versuch: rechts vom Cursor
                    aHintPos = Point( aPos.X() + nSizeXPix + 3, aPos.Y() + nSizeYPix / 2 );
                    if ( aHintPos.X() + aHintSize.Width() > aFrameWinSize.Width() )
                    {
                        // dritter Versuch: ueber dem Cursor
                        aHintPos = Point( aPos.X() + nSizeXPix / 2,
                                            aPos.Y() - aHintSize.Height() - 3 );
                        if ( aHintPos.Y() < 0 )
                        {
                            // oben und unten kein Platz - dann Default und abschneiden
                            aHintPos = Point( aPos.X() + nSizeXPix / 2, aPos.Y() + nSizeYPix + 3 );
                            aHintSize.Height() = aFrameWinSize.Height() - aHintPos.Y();
                            pInputHintWindow->SetSizePixel( aHintSize );
                        }
                    }
                }

                //  X anpassen
                if ( aHintPos.X() + aHintSize.Width() > aFrameWinSize.Width() )
                    aHintPos.X() = aFrameWinSize.Width() - aHintSize.Width();
                //  Y anpassen
                if ( aHintPos.Y() + aHintSize.Height() > aFrameWinSize.Height() )
                    aHintPos.Y() = aFrameWinSize.Height() - aHintSize.Height();

                pInputHintWindow->SetPosPixel( aHintPos );
                pInputHintWindow->ToTop();
                pInputHintWindow->Show();
            }
        }
        else
            DELETEZ(pInputHintWindow);

        // list drop-down button
        if ( pData && pData->HasSelectionList() )
        {
            aListValPos.Set( aViewData.GetCurX(), aViewData.GetCurY(), aViewData.GetTabNo() );
            bListValButton = TRUE;
        }
    }
    else
        DELETEZ(pInputHintWindow);

    for ( USHORT i=0; i<4; i++ )
        if ( pGridWin[i] && pGridWin[i]->IsVisible() )
            pGridWin[i]->UpdateListValPos( bListValButton, aListValPos );
}

void ScTabView::RemoveHintWindow()
{
    DELETEZ(pInputHintWindow);
}


// find window that should not be over the cursor
Window* lcl_GetCareWin(SfxViewFrame* pViewFrm)
{
    //! auch Spelling ??? (dann beim Aufruf Membervariable setzen)

    //  Suchen & Ersetzen
    if ( pViewFrm->HasChildWindow(SID_SEARCH_DLG) )
    {
        SfxChildWindow* pChild = pViewFrm->GetChildWindow(SID_SEARCH_DLG);
        if (pChild)
        {
            Window* pWin = pChild->GetWindow();
            if (pWin && pWin->IsVisible())
                return pWin;
        }
    }

    //  Aenderungen uebernehmen
    if ( pViewFrm->HasChildWindow(FID_CHG_ACCEPT) )
    {
        SfxChildWindow* pChild = pViewFrm->GetChildWindow(FID_CHG_ACCEPT);
        if (pChild)
        {
            Window* pWin = pChild->GetWindow();
            if (pWin && pWin->IsVisible())
                return pWin;
        }
    }

    return NULL;
}

    //
    //  Bildschirm an Cursorposition anpassen
    //

void ScTabView::AlignToCursor( SCsCOL nCurX, SCsROW nCurY, ScFollowMode eMode,
                                const ScSplitPos* pWhich )
{
    //
    //  aktiven Teil umschalten jetzt hier
    //

    ScSplitPos eActive = aViewData.GetActivePart();
    ScHSplitPos eActiveX = WhichH(eActive);
    ScVSplitPos eActiveY = WhichV(eActive);
    BOOL bHFix = (aViewData.GetHSplitMode() == SC_SPLIT_FIX);
    BOOL bVFix = (aViewData.GetVSplitMode() == SC_SPLIT_FIX);
    if (bHFix)
        if (eActiveX == SC_SPLIT_LEFT && nCurX >= (SCsCOL)aViewData.GetFixPosX())
        {
            ActivatePart( (eActiveY==SC_SPLIT_TOP) ? SC_SPLIT_TOPRIGHT : SC_SPLIT_BOTTOMRIGHT );
            eActiveX = SC_SPLIT_RIGHT;
        }
    if (bVFix)
        if (eActiveY == SC_SPLIT_TOP && nCurY >= (SCsROW)aViewData.GetFixPosY())
        {
            ActivatePart( (eActiveX==SC_SPLIT_LEFT) ? SC_SPLIT_BOTTOMLEFT : SC_SPLIT_BOTTOMRIGHT );
            eActiveY = SC_SPLIT_BOTTOM;
        }

    //
    //  eigentliches Align
    //

    if ( eMode != SC_FOLLOW_NONE )
    {
        ScSplitPos eAlign;
        if (pWhich)
            eAlign = *pWhich;
        else
            eAlign = aViewData.GetActivePart();
        ScHSplitPos eAlignX = WhichH(eAlign);
        ScVSplitPos eAlignY = WhichV(eAlign);

        SCsCOL nDeltaX = (SCsCOL) aViewData.GetPosX(eAlignX);
        SCsROW nDeltaY = (SCsROW) aViewData.GetPosY(eAlignY);
        SCsCOL nSizeX = (SCsCOL) aViewData.VisibleCellsX(eAlignX);
        SCsROW nSizeY = (SCsROW) aViewData.VisibleCellsY(eAlignY);

        long nCellSizeX;
        long nCellSizeY;
        if ( nCurX >= 0 && nCurY >= 0 )
            aViewData.GetMergeSizePixel( (SCCOL)nCurX, (SCROW)nCurY, nCellSizeX, nCellSizeY );
        else
            nCellSizeX = nCellSizeY = 0;
        Size aScrSize = aViewData.GetScrSize();
        long nSpaceX = ( aScrSize.Width()  - nCellSizeX ) / 2;
        long nSpaceY = ( aScrSize.Height() - nCellSizeY ) / 2;
        //  nSpaceY: desired start position of cell for FOLLOW_JUMP, modified if dialog interferes

        BOOL bForceNew = FALSE;     // force new calculation of JUMP position (vertical only)

        // VisibleCellsY == CellsAtY( GetPosY( eWhichY ), 1, eWhichY )

        //-------------------------------------------------------------------------------
        //  falls z.B. Suchen-Dialog offen ist, Cursor nicht hinter den Dialog stellen
        //  wenn moeglich, die Zeile mit dem Cursor oberhalb oder unterhalb des Dialogs

        //! nicht, wenn schon komplett sichtbar

        if ( eMode == SC_FOLLOW_JUMP )
        {
            Window* pCare = lcl_GetCareWin( aViewData.GetViewShell()->GetViewFrame() );
            if (pCare)
            {
                BOOL bLimit = FALSE;
                Rectangle aDlgPixel;
                Size aWinSize;
                Window* pWin = GetActiveWin();
                if (pWin)
                {
                    aDlgPixel = pCare->GetWindowExtentsRelative( pWin );
                    aWinSize = pWin->GetOutputSizePixel();
                    //  ueberdeckt der Dialog das GridWin?
                    if ( aDlgPixel.Right() >= 0 && aDlgPixel.Left() < aWinSize.Width() )
                    {
                        if ( nCurX < nDeltaX || nCurX >= nDeltaX+nSizeX ||
                             nCurY < nDeltaY || nCurY >= nDeltaY+nSizeY )
                            bLimit = TRUE;          // es wird sowieso gescrollt
                        else
                        {
                            //  Cursor ist auf dem Bildschirm
                            Point aStart = aViewData.GetScrPos( nCurX, nCurY, eAlign );
                            long nCSX, nCSY;
                            aViewData.GetMergeSizePixel( nCurX, nCurY, nCSX, nCSY );
                            Rectangle aCursor( aStart, Size( nCSX, nCSY ) );
                            if ( aCursor.IsOver( aDlgPixel ) )
                                bLimit = TRUE;      // Zelle vom Dialog ueberdeckt
                        }
                    }
                }

                if (bLimit)
                {
                    BOOL bBottom = FALSE;
                    long nTopSpace = aDlgPixel.Top();
                    long nBotSpace = aWinSize.Height() - aDlgPixel.Bottom();
                    if ( nBotSpace > 0 && nBotSpace > nTopSpace )
                    {
                        long nDlgBot = aDlgPixel.Bottom();
                        SCsCOL nWPosX;
                        SCsROW nWPosY;
                        aViewData.GetPosFromPixel( 0,nDlgBot, eAlign, nWPosX, nWPosY );
                        ++nWPosY;   // unter der letzten betroffenen Zelle

                        SCsROW nDiff = nWPosY - nDeltaY;
                        if ( nCurY >= nDiff )           // Pos. kann nicht negativ werden
                        {
                            nSpaceY = nDlgBot + ( nBotSpace - nCellSizeY ) / 2;
                            bBottom = TRUE;
                            bForceNew = TRUE;
                        }
                    }
                    if ( !bBottom && nTopSpace > 0 )
                    {
                        nSpaceY = ( nTopSpace - nCellSizeY ) / 2;
                        bForceNew = TRUE;
                    }
                }
            }
        }
        //-------------------------------------------------------------------------------

        SCsCOL nNewDeltaX = nDeltaX;
        SCsROW nNewDeltaY = nDeltaY;
        BOOL bDoLine = FALSE;

        switch (eMode)
        {
            case SC_FOLLOW_JUMP:
                if ( nCurX < nDeltaX || nCurX >= nDeltaX+nSizeX )
                {
                    nNewDeltaX = nCurX - static_cast<SCsCOL>(aViewData.CellsAtX( nCurX, -1, eAlignX, static_cast<USHORT>(nSpaceX) ));
                    if (nNewDeltaX < 0) nNewDeltaX = 0;
                    nSizeX = (SCsCOL) aViewData.CellsAtX( nNewDeltaX, 1, eAlignX );
                }
                if ( nCurY < nDeltaY || nCurY >= nDeltaY+nSizeY || bForceNew )
                {
                    nNewDeltaY = nCurY - static_cast<SCsROW>(aViewData.CellsAtY( nCurY, -1, eAlignY, static_cast<USHORT>(nSpaceY) ));
                    if (nNewDeltaY < 0) nNewDeltaY = 0;
                    nSizeY = (SCsROW) aViewData.CellsAtY( nNewDeltaY, 1, eAlignY );
                }
                bDoLine = TRUE;
                break;

            case SC_FOLLOW_LINE:
                bDoLine = TRUE;
                break;

            case SC_FOLLOW_FIX:
                if ( nCurX < nDeltaX || nCurX >= nDeltaX+nSizeX )
                {
                    nNewDeltaX = nDeltaX + nCurX - aViewData.GetCurX();
                    nSizeX = (SCsCOL) aViewData.CellsAtX( nNewDeltaX, 1, eAlignX );
                }
                if ( nCurY < nDeltaY || nCurY >= nDeltaY+nSizeY )
                {
                    nNewDeltaY = nDeltaY + nCurY - aViewData.GetCurY();
                    nSizeY = (SCsROW) aViewData.CellsAtY( nNewDeltaY, 1, eAlignY );
                }

                //  like old version of SC_FOLLOW_JUMP:

                if ( nCurX < nNewDeltaX || nCurX >= nNewDeltaX+nSizeX )
                {
                    nNewDeltaX = nCurX - (nSizeX / 2);
                    if (nNewDeltaX < 0) nNewDeltaY = 0;
                    nSizeX = (SCsCOL) aViewData.CellsAtX( nNewDeltaX, 1, eAlignX );
                }
                if ( nCurY < nNewDeltaY || nCurY >= nNewDeltaY+nSizeY )
                {
                    nNewDeltaY = nCurY - (nSizeY / 2);
                    if (nNewDeltaY < 0) nNewDeltaY = 0;
                    nSizeY = (SCsROW) aViewData.CellsAtY( nNewDeltaY, 1, eAlignY );
                }

                bDoLine = TRUE;
                break;

            case SC_FOLLOW_NONE:
                break;
            default:
                DBG_ERROR("Falscher Cursormodus");
                break;
        }

        if (bDoLine)
        {
            while ( nCurX >= nNewDeltaX+nSizeX )
            {
                nNewDeltaX = nCurX-nSizeX+1;
                ScDocument* pDoc = aViewData.GetDocument();
                SCTAB nTab = aViewData.GetTabNo();
                while ( nNewDeltaX < MAXCOL && !pDoc->GetColWidth( nNewDeltaX, nTab ) )
                    ++nNewDeltaX;
                nSizeX = (SCsCOL) aViewData.CellsAtX( nNewDeltaX, 1, eAlignX );
            }
            while ( nCurY >= nNewDeltaY+nSizeY )
            {
                nNewDeltaY = nCurY-nSizeY+1;
                ScDocument* pDoc = aViewData.GetDocument();
                SCTAB nTab = aViewData.GetTabNo();
                while ( nNewDeltaY < MAXROW && !pDoc->GetRowHeight( nNewDeltaY, nTab ) )
                    ++nNewDeltaY;
                nSizeY = (SCsROW) aViewData.CellsAtY( nNewDeltaY, 1, eAlignY );
            }
            if ( nCurX < nNewDeltaX ) nNewDeltaX = nCurX;
            if ( nCurY < nNewDeltaY ) nNewDeltaY = nCurY;
        }

        if ( nNewDeltaX != nDeltaX )
            nSizeX = (SCsCOL) aViewData.CellsAtX( nNewDeltaX, 1, eAlignX );
        if (nNewDeltaX+nSizeX-1 > MAXCOL) nNewDeltaX = MAXCOL-nSizeX+1;
        if (nNewDeltaX < 0) nNewDeltaX = 0;

        if ( nNewDeltaY != nDeltaY )
            nSizeY = (SCsROW) aViewData.CellsAtY( nNewDeltaY, 1, eAlignY );
        if (nNewDeltaY+nSizeY-1 > MAXROW) nNewDeltaY = MAXROW-nSizeY+1;
        if (nNewDeltaY < 0) nNewDeltaY = 0;

        if ( nNewDeltaX != nDeltaX ) ScrollX( nNewDeltaX - nDeltaX, eAlignX );
        if ( nNewDeltaY != nDeltaY ) ScrollY( nNewDeltaY - nDeltaY, eAlignY );
    }

    //
    //  nochmal aktiven Teil umschalten
    //

    if (bHFix)
        if (eActiveX == SC_SPLIT_RIGHT && nCurX < (SCsCOL)aViewData.GetFixPosX())
        {
            ActivatePart( (eActiveY==SC_SPLIT_TOP) ? SC_SPLIT_TOPLEFT : SC_SPLIT_BOTTOMLEFT );
            eActiveX = SC_SPLIT_LEFT;
        }
    if (bVFix)
        if (eActiveY == SC_SPLIT_BOTTOM && nCurY < (SCsROW)aViewData.GetFixPosY())
        {
            ActivatePart( (eActiveX==SC_SPLIT_LEFT) ? SC_SPLIT_TOPLEFT : SC_SPLIT_TOPRIGHT );
            eActiveY = SC_SPLIT_TOP;
        }
}

BOOL ScTabView::SelMouseButtonDown( const MouseEvent& rMEvt )
{
    BOOL bRet = FALSE;

    // #i3875# *Hack*
    BOOL bMod1Locked = aViewData.GetViewShell()->GetLockedModifiers() & KEY_MOD1 ? TRUE : FALSE;
    aViewData.SetSelCtrlMouseClick( rMEvt.IsMod1() || bMod1Locked );

    if ( pSelEngine )
    {
        bMoveIsShift = rMEvt.IsShift();
        bRet = pSelEngine->SelMouseButtonDown( rMEvt );
        bMoveIsShift = FALSE;
    }

    aViewData.SetSelCtrlMouseClick( FALSE ); // #i3875# *Hack*

    return bRet;
}

    //
    //  MoveCursor - mit Anpassung des Bildausschnitts
    //

void ScTabView::MoveCursorAbs( SCsCOL nCurX, SCsROW nCurY, ScFollowMode eMode,
                                BOOL bShift, BOOL bControl, BOOL bKeepOld, BOOL bKeepSel )
{
    if (!bKeepOld)
        aViewData.ResetOldCursor();

    if (nCurX < 0) nCurX = 0;
    if (nCurY < 0) nCurY = 0;
    if (nCurX > MAXCOL) nCurX = MAXCOL;
    if (nCurY > MAXROW) nCurY = MAXROW;

    HideAllCursors();

    if ( bShift && bNewStartIfMarking && IsBlockMode() )
    {
        //  used for ADD selection mode: start a new block from the cursor position
        DoneBlockMode( TRUE );
        InitBlockMode( aViewData.GetCurX(), aViewData.GetCurY(), aViewData.GetTabNo(), TRUE );
    }

        //  aktiven Teil umschalten jetzt in AlignToCursor

    AlignToCursor( nCurX, nCurY, eMode );
    //!     auf OS/2: SC_FOLLOW_JUMP statt SC_FOLLOW_LINE, um Nachlaufen zu verhindern ???

    if (bKeepSel)
        SetCursor( nCurX, nCurY );      // Markierung stehenlassen
    else
    {
        BOOL bSame = ( nCurX == aViewData.GetCurX() && nCurY == aViewData.GetCurY() );
        bMoveIsShift = bShift;
        pSelEngine->CursorPosChanging( bShift, bControl );
        bMoveIsShift = FALSE;
        aFunctionSet.SetCursorAtCell( nCurX, nCurY, FALSE );

        //  Wenn der Cursor nicht bewegt wurde, muss das SelectionChanged fuer das
        //  Aufheben der Selektion hier einzeln passieren:
        if (bSame)
            SelectionChanged();
    }

    ShowAllCursors();
}

void ScTabView::MoveCursorRel( SCsCOL nMovX, SCsROW nMovY, ScFollowMode eMode,
                                    BOOL bShift, BOOL bKeepSel )
{
    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();

    SCsCOL nOldX;
    SCsROW nOldY;
    SCsCOL nCurX;
    SCsROW nCurY;
    if ( aViewData.IsRefMode() )
    {
        nOldX = (SCsCOL) aViewData.GetRefEndX();
        nOldY = (SCsROW) aViewData.GetRefEndY();
        nCurX = nOldX + nMovX;
        nCurY = nOldY + nMovY;
    }
    else
    {
        nOldX = (SCsCOL) aViewData.GetCurX();
        nOldY = (SCsROW) aViewData.GetCurY();
        nCurX = (nMovX != 0) ? nOldX+nMovX : (SCsCOL) aViewData.GetOldCurX();
        nCurY = (nMovY != 0) ? nOldY+nMovY : (SCsROW) aViewData.GetOldCurY();
    }

    BOOL bHidden;
    aViewData.ResetOldCursor();

    if (nMovX != 0 && VALIDCOLROW(nCurX,nCurY))
    {
        BOOL bHFlip = FALSE;
        do
        {
            BYTE nColFlags = pDoc->GetColFlags( nCurX, nTab );
            bHidden = (nColFlags & CR_HIDDEN) || pDoc->IsHorOverlapped( nCurX, nCurY, nTab );
            if (bHidden)
            {
                if ( nCurX<=0 || nCurX>=MAXCOL )
                {
                    if (bHFlip)
                    {
                        nCurX = nOldX;
                        bHidden = FALSE;
                    }
                    else
                    {
                        nMovX = -nMovX;
                        if (nMovX > 0) ++nCurX; else --nCurX;       // zuruecknehmen
                        bHFlip = TRUE;
                    }
                }
                else
                    if (nMovX > 0) ++nCurX; else --nCurX;
            }
        }
        while (bHidden);
        if (pDoc->IsVerOverlapped( nCurX, nCurY, nTab ))
        {
            aViewData.SetOldCursor( nCurX,nCurY );
            while (pDoc->IsVerOverlapped( nCurX, nCurY, nTab ))
                --nCurY;
        }
    }

    if (nMovY != 0 && VALIDCOLROW(nCurX,nCurY))
    {
        BOOL bVFlip = FALSE;
        do
        {
            BYTE nRowFlags = pDoc->GetRowFlags( nCurY, nTab );
            bHidden = (nRowFlags & CR_HIDDEN) || pDoc->IsVerOverlapped( nCurX, nCurY, nTab );
            if (bHidden)
            {
                if ( nCurY<=0 || nCurY>=MAXROW )
                {
                    if (bVFlip)
                    {
                        nCurY = nOldY;
                        bHidden = FALSE;
                    }
                    else
                    {
                        nMovY = -nMovY;
                        if (nMovY > 0) ++nCurY; else --nCurY;       // zuruecknehmen
                        bVFlip = TRUE;
                    }
                }
                else
                    if (nMovY > 0) ++nCurY; else --nCurY;
            }
        }
        while (bHidden);
        if (pDoc->IsHorOverlapped( nCurX, nCurY, nTab ))
        {
            aViewData.SetOldCursor( nCurX,nCurY );
            while (pDoc->IsHorOverlapped( nCurX, nCurY, nTab ))
                --nCurX;
        }
    }

    MoveCursorAbs( nCurX, nCurY, eMode, bShift, FALSE, TRUE, bKeepSel );
}

void ScTabView::MoveCursorPage( SCsCOL nMovX, SCsROW nMovY, ScFollowMode eMode, BOOL bShift, BOOL bKeepSel )
{
    SCCOL nCurX;
    SCROW nCurY;
    aViewData.GetMoveCursor( nCurX,nCurY );

    ScSplitPos eWhich = aViewData.GetActivePart();
    ScHSplitPos eWhichX = WhichH( eWhich );
    ScVSplitPos eWhichY = WhichV( eWhich );

    SCsCOL nPageX;
    SCsROW nPageY;
    if (nMovX >= 0)
        nPageX = ((SCsCOL) aViewData.CellsAtX( nCurX, 1, eWhichX )) * nMovX;
    else
        nPageX = ((SCsCOL) aViewData.CellsAtX( nCurX, -1, eWhichX )) * nMovX;

    if (nMovY >= 0)
        nPageY = ((SCsROW) aViewData.CellsAtY( nCurY, 1, eWhichY )) * nMovY;
    else
        nPageY = ((SCsROW) aViewData.CellsAtY( nCurY, -1, eWhichY )) * nMovY;

    if (nMovX != 0 && nPageX == 0) nPageX = (nMovX>0) ? 1 : -1;
    if (nMovY != 0 && nPageY == 0) nPageY = (nMovY>0) ? 1 : -1;

    MoveCursorRel( nPageX, nPageY, eMode, bShift, bKeepSel );
}

void ScTabView::MoveCursorArea( SCsCOL nMovX, SCsROW nMovY, ScFollowMode eMode, BOOL bShift, BOOL bKeepSel )
{
    SCCOL nCurX;
    SCROW nCurY;
    aViewData.GetMoveCursor( nCurX,nCurY );
    SCCOL nNewX = nCurX;
    SCROW nNewY = nCurY;

    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();

    //  FindAreaPos kennt nur -1 oder 1 als Richtung

    SCsCOLROW i;
    if ( nMovX > 0 )
        for ( i=0; i<nMovX; i++ )
            pDoc->FindAreaPos( nNewX, nNewY, nTab,  1,  0 );
    if ( nMovX < 0 )
        for ( i=0; i<-nMovX; i++ )
            pDoc->FindAreaPos( nNewX, nNewY, nTab, -1,  0 );
    if ( nMovY > 0 )
        for ( i=0; i<nMovY; i++ )
            pDoc->FindAreaPos( nNewX, nNewY, nTab,  0,  1 );
    if ( nMovY < 0 )
        for ( i=0; i<-nMovY; i++ )
            pDoc->FindAreaPos( nNewX, nNewY, nTab,  0, -1 );

    if (eMode==SC_FOLLOW_JUMP)                  // unten/rechts nicht zuviel grau anzeigen
    {
        if (nMovX != 0 && nNewX == MAXCOL)
            eMode = SC_FOLLOW_LINE;
        if (nMovY != 0 && nNewY == MAXROW)
            eMode = SC_FOLLOW_LINE;
    }

    MoveCursorRel( ((SCsCOL)nNewX)-(SCsCOL)nCurX, ((SCsROW)nNewY)-(SCsROW)nCurY, eMode, bShift, bKeepSel );
}

void ScTabView::MoveCursorEnd( SCsCOL nMovX, SCsROW nMovY, ScFollowMode eMode, BOOL bShift, BOOL bKeepSel )
{
    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();

    SCCOL nCurX;
    SCROW nCurY;
    aViewData.GetMoveCursor( nCurX,nCurY );
    SCCOL nNewX = nCurX;
    SCROW nNewY = nCurY;

    SCCOL nUsedX = 0;
    SCROW nUsedY = 0;
    if ( nMovX > 0 || nMovY > 0 )
        pDoc->GetPrintArea( nTab, nUsedX, nUsedY );     // Ende holen

    if (nMovX<0)
        nNewX=0;
    else if (nMovX>0)
        nNewX=nUsedX;                                   // letzter benutzter Bereich

    if (nMovY<0)
        nNewY=0;
    else if (nMovY>0)
        nNewY=nUsedY;

    aViewData.ResetOldCursor();
    MoveCursorRel( ((SCsCOL)nNewX)-(SCsCOL)nCurX, ((SCsROW)nNewY)-(SCsROW)nCurY, eMode, bShift, bKeepSel );
}

void ScTabView::MoveCursorScreen( SCsCOL nMovX, SCsROW nMovY, ScFollowMode eMode, BOOL bShift )
{
    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();

    SCCOL nCurX;
    SCROW nCurY;
    aViewData.GetMoveCursor( nCurX,nCurY );
    SCCOL nNewX = nCurX;
    SCROW nNewY = nCurY;

    ScSplitPos eWhich = aViewData.GetActivePart();
    SCCOL nPosX = aViewData.GetPosX( WhichH(eWhich) );
    SCROW nPosY = aViewData.GetPosY( WhichV(eWhich) );

    SCCOL nAddX = aViewData.VisibleCellsX( WhichH(eWhich) );
    if (nAddX != 0)
        --nAddX;
    SCROW nAddY = aViewData.VisibleCellsY( WhichV(eWhich) );
    if (nAddY != 0)
        --nAddY;

    if (nMovX<0)
        nNewX=nPosX;
    else if (nMovX>0)
        nNewX=nPosX+nAddX;

    if (nMovY<0)
        nNewY=nPosY;
    else if (nMovY>0)
        nNewY=nPosY+nAddY;

//  aViewData.ResetOldCursor();
    aViewData.SetOldCursor( nNewX,nNewY );

    while (pDoc->IsHorOverlapped( nNewX, nNewY, nTab ))
        --nNewX;
    while (pDoc->IsVerOverlapped( nNewX, nNewY, nTab ))
        --nNewY;

    MoveCursorAbs( nNewX, nNewY, eMode, bShift, FALSE, TRUE );
}

void ScTabView::MoveCursorEnter( BOOL bShift )          // bShift -> hoch/runter
{
    const ScInputOptions& rOpt = SC_MOD()->GetInputOptions();
    if (!rOpt.GetMoveSelection())
    {
        aViewData.UpdateInputHandler(TRUE);
        return;
    }

    SCsCOL nMoveX = 0;
    SCsROW nMoveY = 0;
    switch ((ScDirection)rOpt.GetMoveDir())
    {
        case DIR_BOTTOM:
            nMoveY = bShift ? -1 : 1;
            break;
        case DIR_RIGHT:
            nMoveX = bShift ? -1 : 1;
            break;
        case DIR_TOP:
            nMoveY = bShift ? 1 : -1;
            break;
        case DIR_LEFT:
            nMoveX = bShift ? 1 : -1;
            break;
    }

    ScMarkData& rMark = aViewData.GetMarkData();
    if (rMark.IsMarked() || rMark.IsMultiMarked())
    {
        SCCOL nCurX;
        SCROW nCurY;
        aViewData.GetMoveCursor( nCurX,nCurY );
        SCCOL nNewX = nCurX;
        SCROW nNewY = nCurY;
        SCTAB nTab = aViewData.GetTabNo();

        ScDocument* pDoc = aViewData.GetDocument();
        pDoc->GetNextPos( nNewX,nNewY, nTab, nMoveX,nMoveY, TRUE,FALSE, rMark );

        MoveCursorRel( ((SCsCOL)nNewX)-(SCsCOL)nCurX, ((SCsROW)nNewY)-(SCsROW)nCurY,
                            SC_FOLLOW_LINE, FALSE, TRUE );

        //  update input line even if cursor was not moved
        if ( nNewX == nCurX && nNewY == nCurY )
            aViewData.UpdateInputHandler(TRUE);
    }
    else
    {
        if ( nMoveY != 0 && !nMoveX && rOpt.GetUseTabCol() )
        {
            //  nach Tab und Enter wieder zur Ausgangsspalte
            SCCOL nTabCol = aViewData.GetTabStartCol();
            if (nTabCol != SC_TABSTART_NONE)
            {
                SCCOL nCurX;
                SCROW nCurY;
                aViewData.GetMoveCursor( nCurX,nCurY );
                nMoveX = ((SCsCOL)nTabCol)-(SCsCOL)nCurX;
            }
        }

        MoveCursorRel( nMoveX,nMoveY, SC_FOLLOW_LINE, FALSE );
    }
}


BOOL ScTabView::MoveCursorKeyInput( const KeyEvent& rKeyEvent )
{
    const KeyCode& rKCode = rKeyEvent.GetKeyCode();

    enum { MOD_NONE, MOD_CTRL, MOD_ALT, MOD_BOTH } eModifier =
        rKCode.IsMod1() ?
            (rKCode.IsMod2() ? MOD_BOTH : MOD_CTRL) :
            (rKCode.IsMod2() ? MOD_ALT : MOD_NONE);

    BOOL bSel = rKCode.IsShift();
    USHORT nCode = rKCode.GetCode();

    // CURSOR keys
    SCsCOL nDX = 0;
    SCsROW nDY = 0;
    switch( nCode )
    {
        case KEY_LEFT:  nDX = -1;   break;
        case KEY_RIGHT: nDX = 1;    break;
        case KEY_UP:    nDY = -1;   break;
        case KEY_DOWN:  nDY = 1;    break;
    }
    if( nDX != 0 || nDY != 0 )
    {
        switch( eModifier )
        {
            case MOD_NONE:  MoveCursorRel( nDX, nDY, SC_FOLLOW_LINE, bSel );    break;
            case MOD_CTRL:  MoveCursorArea( nDX, nDY, SC_FOLLOW_JUMP, bSel );   break;
            default:
            {
                // added to avoid warnings
            }
        }
        // always TRUE to suppress changes of col/row size (ALT+CURSOR)
        return TRUE;
    }

    // PAGEUP/PAGEDOWN
    if( (nCode == KEY_PAGEUP) || (nCode == KEY_PAGEDOWN) )
    {
        nDX = (nCode == KEY_PAGEUP) ? -1 : 1;
        switch( eModifier )
        {
            case MOD_NONE:  MoveCursorPage( 0, static_cast<SCsCOLROW>(nDX), SC_FOLLOW_FIX, bSel );  break;
            case MOD_ALT:   MoveCursorPage( nDX, 0, SC_FOLLOW_FIX, bSel );  break;
            case MOD_CTRL:  SelectNextTab( nDX );                           break;
            default:
            {
                // added to avoid warnings
            }
        }
        return TRUE;
    }

    // HOME/END
    if( (nCode == KEY_HOME) || (nCode == KEY_END) )
    {
        nDX = (nCode == KEY_HOME) ? -1 : 1;
        ScFollowMode eMode = (nCode == KEY_HOME) ? SC_FOLLOW_LINE : SC_FOLLOW_JUMP;
        switch( eModifier )
        {
            case MOD_NONE:  MoveCursorEnd( nDX, 0, eMode, bSel );   break;
            case MOD_CTRL:  MoveCursorEnd( nDX, static_cast<SCsCOLROW>(nDX), eMode, bSel ); break;
            default:
            {
                // added to avoid warnings
            }
        }
        return TRUE;
    }

    return FALSE;
}


        // naechste/vorherige nicht geschuetzte Zelle
void ScTabView::FindNextUnprot( BOOL bShift, BOOL bInSelection )
{
    short nMove = bShift ? -1 : 1;

    ScMarkData& rMark = aViewData.GetMarkData();
    BOOL bMarked = bInSelection && (rMark.IsMarked() || rMark.IsMultiMarked());

    SCCOL nCurX;
    SCROW nCurY;
    aViewData.GetMoveCursor( nCurX,nCurY );
    SCCOL nNewX = nCurX;
    SCROW nNewY = nCurY;
    SCTAB nTab = aViewData.GetTabNo();

    ScDocument* pDoc = aViewData.GetDocument();
    pDoc->GetNextPos( nNewX,nNewY, nTab, nMove,0, bMarked,TRUE, rMark );

    SCCOL nTabCol = aViewData.GetTabStartCol();
    if ( nTabCol == SC_TABSTART_NONE )
        nTabCol = nCurX;                    // auf diese Spalte zurueck bei Enter

    MoveCursorRel( ((SCsCOL)nNewX)-(SCsCOL)nCurX, ((SCsROW)nNewY)-(SCsROW)nCurY,
                        SC_FOLLOW_LINE, FALSE, TRUE );

    //  in MoveCursorRel wird die TabCol zurueckgesetzt...
    aViewData.SetTabStartCol( nTabCol );
}

void ScTabView::MarkColumns()
{
    SCCOL nStartCol;
    SCCOL nEndCol;

    ScMarkData& rMark = aViewData.GetMarkData();
    if (rMark.IsMarked())
    {
        ScRange aMarkRange;
        rMark.GetMarkArea( aMarkRange );
        nStartCol = aMarkRange.aStart.Col();
        nEndCol = aMarkRange.aEnd.Col();
    }
    else
    {
        SCROW nDummy;
        aViewData.GetMoveCursor( nStartCol, nDummy );
        nEndCol=nStartCol;
    }

    SCTAB nTab = aViewData.GetTabNo();
    DoneBlockMode();
    InitBlockMode( nStartCol,0, nTab );
    MarkCursor( nEndCol,MAXROW, nTab );
    SelectionChanged();
}

void ScTabView::MarkRows()
{
    SCROW nStartRow;
    SCROW nEndRow;

    ScMarkData& rMark = aViewData.GetMarkData();
    if (rMark.IsMarked())
    {
        ScRange aMarkRange;
        rMark.GetMarkArea( aMarkRange );
        nStartRow = aMarkRange.aStart.Row();
        nEndRow = aMarkRange.aEnd.Row();
    }
    else
    {
        SCCOL nDummy;
        aViewData.GetMoveCursor( nDummy, nStartRow );
        nEndRow=nStartRow;
    }

    SCTAB nTab = aViewData.GetTabNo();
    DoneBlockMode();
    InitBlockMode( 0,nStartRow, nTab );
    MarkCursor( MAXCOL,nEndRow, nTab );
    SelectionChanged();
}

void ScTabView::MarkDataArea( BOOL bIncludeCursor )
{
    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();
    SCCOL nStartCol = aViewData.GetCurX();
    SCROW nStartRow = aViewData.GetCurY();
    SCCOL nEndCol = nStartCol;
    SCROW nEndRow = nStartRow;

    pDoc->GetDataArea( nTab, nStartCol, nStartRow, nEndCol, nEndRow, bIncludeCursor );

    HideAllCursors();
    DoneBlockMode();
    InitBlockMode( nStartCol, nStartRow, nTab );
    MarkCursor( nEndCol, nEndRow, nTab );
    ShowAllCursors();

    SelectionChanged();
}

void ScTabView::MarkMatrixFormula()
{
    ScDocument* pDoc = aViewData.GetDocument();
    ScAddress aCursor( aViewData.GetCurX(), aViewData.GetCurY(), aViewData.GetTabNo() );
    ScRange aMatrix;
    if ( pDoc->GetMatrixFormulaRange( aCursor, aMatrix ) )
    {
        MarkRange( aMatrix, FALSE );        // cursor is already within the range
    }
}

void ScTabView::MarkRange( const ScRange& rRange, BOOL bSetCursor, BOOL bContinue )
{
    SCTAB nTab = rRange.aStart.Tab();
    SetTabNo( nTab );

    HideAllCursors();
    DoneBlockMode( bContinue ); // bContinue==TRUE -> clear old mark
    if (bSetCursor)             // Wenn Cursor gesetzt wird, immer auch alignen
    {
        SCCOL nAlignX = rRange.aStart.Col();
        SCROW nAlignY = rRange.aStart.Row();
        if ( rRange.aStart.Col() == 0 && rRange.aEnd.Col() == MAXCOL )
            nAlignX = aViewData.GetPosX(WhichH(aViewData.GetActivePart()));
        if ( rRange.aStart.Row() == 0 && rRange.aEnd.Row() == MAXROW )
            nAlignY = aViewData.GetPosY(WhichV(aViewData.GetActivePart()));
        AlignToCursor( nAlignX, nAlignY, SC_FOLLOW_JUMP );
    }
    InitBlockMode( rRange.aStart.Col(), rRange.aStart.Row(), nTab );
    MarkCursor( rRange.aEnd.Col(), rRange.aEnd.Row(), nTab );
    if (bSetCursor)
    {
        SCCOL nPosX = rRange.aStart.Col();
        SCROW nPosY = rRange.aStart.Row();
        ScDocument* pDoc = aViewData.GetDocument();

        while (pDoc->IsHorOverlapped( nPosX, nPosY, nTab ))     //! ViewData !!!
            --nPosX;
        while (pDoc->IsVerOverlapped( nPosX, nPosY, nTab ))
            --nPosY;

        aViewData.ResetOldCursor();
        SetCursor( nPosX, nPosY );
    }
    ShowAllCursors();

    SelectionChanged();
}

void ScTabView::Unmark()
{
    ScMarkData& rMark = aViewData.GetMarkData();
    if ( rMark.IsMarked() || rMark.IsMultiMarked() )
    {
        SCCOL nCurX;
        SCROW nCurY;
        aViewData.GetMoveCursor( nCurX,nCurY );
        MoveCursorAbs( nCurX, nCurY, SC_FOLLOW_NONE, FALSE, FALSE );

        SelectionChanged();
    }
}

void ScTabView::SelectNextTab( short nDir, BOOL bExtendSelection )
{
    if (!nDir) return;
    DBG_ASSERT( nDir==-1 || nDir==1, "SelectNextTab: falscher Wert");

    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();
    if (nDir<0)
    {
        if (!nTab) return;
        --nTab;
        while (!pDoc->IsVisible(nTab))
        {
            if (!nTab) return;
            --nTab;
        }
    }
    else
    {
        SCTAB nCount = pDoc->GetTableCount();
        ++nTab;
        if (nTab >= nCount) return;
        while (!pDoc->IsVisible(nTab))
        {
            ++nTab;
            if (nTab >= nCount) return;
        }
    }

    SetTabNo( nTab, FALSE, bExtendSelection );
    PaintExtras();
}


//  SetTabNo    - angezeigte Tabelle

void ScTabView::SetTabNo( SCTAB nTab, BOOL bNew, BOOL bExtendSelection )
{
    if ( !ValidTab(nTab) )
    {
        DBG_ERROR("SetTabNo: falsche Tabelle");
        return;
    }

    if ( nTab != aViewData.GetTabNo() || bNew )
    {
        //  #57724# Die FormShell moechte vor dem Umschalten benachrichtigt werden
        FmFormShell* pFormSh = aViewData.GetViewShell()->GetFormShell();
        if (pFormSh)
        {
            BOOL bAllowed = sal::static_int_cast<BOOL>( pFormSh->PrepareClose( TRUE ) );
            if (!bAllowed)
            {
                //! Fehlermeldung? oder macht das die FormShell selber?
                //! Fehler-Flag zurueckgeben und Aktionen abbrechen

                return;     // Die FormShell sagt, es kann nicht umgeschaltet werden
            }
        }

                                        //  nicht InputEnterHandler wegen Referenzeingabe !

        ScDocument* pDoc = aViewData.GetDocument();
        pDoc->MakeTable( nTab );

        SCTAB nTabCount = pDoc->GetTableCount();
        SCTAB nOldPos = nTab;
        while (!pDoc->IsVisible(nTab))              // naechste sichtbare suchen
        {
            BOOL bUp = (nTab>=nOldPos);
            if (bUp)
            {
                ++nTab;
                if (nTab>=nTabCount)
                {
                    nTab = nOldPos;
                    bUp = FALSE;
                }
            }

            if (!bUp)
            {
                if (nTab != 0)
                    --nTab;
                else
                {
                    DBG_ERROR("keine sichtbare Tabelle");
                    pDoc->SetVisible( 0, TRUE );
                }
            }
        }

        // #i71490# Deselect drawing objects before changing the sheet number in view data,
        // so the handling of notes still has the sheet selected on which the notes are.
        DrawDeselectAll();

        BOOL bRefMode = SC_MOD()->IsFormulaMode();
        if ( !bRefMode ) // Abfrage, damit RefMode bei Tabellenwechsel funktioniert
        {
            DoneBlockMode();
            pSelEngine->Reset();                // reset all flags, including locked modifiers
            aViewData.SetRefTabNo( nTab );
        }

        ScSplitPos eOldActive = aViewData.GetActivePart();      // before switching
        BOOL bFocus = pGridWin[eOldActive]->HasFocus();

        aViewData.SetTabNo( nTab );
        //  UpdateShow noch vor SetCursor, damit UpdateAutoFillMark die richtigen
        //  Fenster findet (wird aus SetCursor gerufen)
        UpdateShow();
        aViewData.ResetOldCursor();
        SetCursor( aViewData.GetCurX(), aViewData.GetCurY(), TRUE );

        if ( bRefMode )     // evtl. EditView verstecken (nach aViewData.SetTabNo !)
        {
            for ( USHORT i=0; i<4; i++ )
                if ( pGridWin[i] )
                    if ( pGridWin[i]->IsVisible() )
                        pGridWin[i]->UpdateEditViewPos();
        }

        SfxBindings& rBindings = aViewData.GetBindings();
        ScMarkData& rMark = aViewData.GetMarkData();

        BOOL bSelectOneTable = FALSE;
        if (bExtendSelection)
        {
            // #i6327# if all tables are selected, a selection event (#i6330#) will deselect all
            BOOL bAllSelected = TRUE;
            for( SCTAB nSelTab = 0; bAllSelected && (nSelTab < nTabCount); ++nSelTab )
                bAllSelected = !pDoc->IsVisible( nSelTab ) || rMark.GetTableSelect( nSelTab );
            if( bAllSelected )
            {
                bExtendSelection = FALSE;
                bSelectOneTable = TRUE;
            }
        }
        else
        {
            // move from multi-selection to unselected table
            bSelectOneTable = !rMark.GetTableSelect( nTab );
        }

        if (bExtendSelection)
        {
            // #i6330# multi-selection with keyboard
            rMark.SelectTable( nTab, TRUE );
        }
        else if (bSelectOneTable)
        {
            rMark.SelectOneTable( nTab );
            rBindings.Invalidate( FID_FILL_TAB );
        }

        TabChanged();                                       // DrawView
        aViewData.GetViewShell()->WindowChanged();          // falls das aktive Fenster anders ist
        aViewData.GetViewShell()->DisconnectAllClients();   // wichtig fuer Floating Frames

        if ( bFocus && aViewData.GetActivePart() != eOldActive && !bRefMode )
            ActiveGrabFocus();      // grab focus to the pane that's active now

            //  Fixierungen

        BOOL bResize = FALSE;
        if ( aViewData.GetHSplitMode() == SC_SPLIT_FIX )
            if (aViewData.UpdateFixX())
                bResize = TRUE;
        if ( aViewData.GetVSplitMode() == SC_SPLIT_FIX )
            if (aViewData.UpdateFixY())
                bResize = TRUE;
        if (bResize)
            RepeatResize();
        InvalidateSplit();

        if ( aViewData.IsPagebreakMode() )
            UpdatePageBreakData();              //! asynchron ??

        //  #53551# Form-Layer muss den sichtbaren Ausschnitt der neuen Tabelle kennen
        //  dafuer muss hier schon der MapMode stimmen
        for (USHORT i=0; i<4; i++)
            if (pGridWin[i])
                pGridWin[i]->SetMapMode( pGridWin[i]->GetDrawMapMode() );
        SetNewVisArea();

        PaintGrid();
        PaintTop();
        PaintLeft();
        PaintExtras();

        DoResize( aBorderPos, aFrameSize );
        rBindings.Invalidate( SID_DELETE_PRINTAREA );   // Menue
        rBindings.Invalidate( FID_DEL_MANUALBREAKS );
        rBindings.Invalidate( FID_RESET_PRINTZOOM );
        rBindings.Invalidate( SID_STATUS_DOCPOS );      // Statusbar
        rBindings.Invalidate( SID_STATUS_PAGESTYLE );   // Statusbar
        rBindings.Invalidate( SID_CURRENTTAB );         // Navigator
        rBindings.Invalidate( SID_STYLE_FAMILY2 );  // Gestalter
        rBindings.Invalidate( SID_STYLE_FAMILY4 );  // Gestalter
        rBindings.Invalidate( SID_TABLES_COUNT );

        ScModule* pScMod = SC_MOD();

        if(pScMod->IsRefDialogOpen())
        {
            USHORT nCurRefDlgId=pScMod->GetCurRefDlgId();
            SfxViewFrame* pViewFrm = aViewData.GetViewShell()->GetViewFrame();
            SfxChildWindow* pChildWnd = pViewFrm->GetChildWindow( nCurRefDlgId );
            if ( pChildWnd )
            {
                ScAnyRefDlg* pRefDlg = (ScAnyRefDlg*)pChildWnd->GetWindow();
                pRefDlg->ViewShellChanged(NULL);
            }
        }
    }
}

//
//  Paint-Funktionen - nur fuer diese View
//

void ScTabView::MakeEditView( ScEditEngineDefaulter* pEngine, SCCOL nCol, SCROW nRow )
{
    DrawDeselectAll();

    if (pDrawView)
        DrawEnableAnim( FALSE );

    EditView* pSpellingView = aViewData.GetSpellingView();

    for (USHORT i=0; i<4; i++)
        if (pGridWin[i])
            if ( pGridWin[i]->IsVisible() && !aViewData.HasEditView((ScSplitPos)i) )
            {
                ScHSplitPos eHWhich = WhichH( (ScSplitPos) i );
                ScVSplitPos eVWhich = WhichV( (ScSplitPos) i );
                SCCOL nScrX = aViewData.GetPosX( eHWhich );
                SCROW nScrY = aViewData.GetPosY( eVWhich );

                BOOL bPosVisible =
                     ( nCol >= nScrX && nCol <= nScrX + aViewData.VisibleCellsX(eHWhich) + 1 &&
                       nRow >= nScrY && nRow <= nScrY + aViewData.VisibleCellsY(eVWhich) + 1 );

                //  #102421# for the active part, create edit view even if outside the visible area,
                //  so input isn't lost (and the edit view may be scrolled into the visible area)

                //  #i26433# during spelling, the spelling view must be active
                if ( bPosVisible || aViewData.GetActivePart() == (ScSplitPos) i ||
                     ( pSpellingView && aViewData.GetEditView((ScSplitPos) i) == pSpellingView ) )
                {
                    pGridWin[i]->HideCursor();

                    pGridWin[i]->DeleteCursorOverlay();
                    pGridWin[i]->DeleteAutoFillOverlay();

                    // flush OverlayManager before changing MapMode to text edit
                    pGridWin[i]->flushOverlayManager();

                    // MapMode must be set after HideCursor
                    pGridWin[i]->SetMapMode(aViewData.GetLogicMode());

                    aViewData.SetEditEngine( (ScSplitPos) i, pEngine, pGridWin[i], nCol, nRow );

                    if ( !bPosVisible )
                    {
                        //  move the edit view area to the real (possibly negative) position,
                        //  or hide if completely above or left of the window
                        pGridWin[i]->UpdateEditViewPos();
                    }
                }
            }

    if (aViewData.GetViewShell()->HasAccessibilityObjects())
        aViewData.GetViewShell()->BroadcastAccessibility(SfxSimpleHint(SC_HINT_ACC_ENTEREDITMODE));
}

void ScTabView::UpdateEditView()
{
    ScSplitPos eActive = aViewData.GetActivePart();
    for (USHORT i=0; i<4; i++)
        if (aViewData.HasEditView( (ScSplitPos) i ))
        {
            EditView* pEditView = aViewData.GetEditView( (ScSplitPos) i );
            aViewData.SetEditEngine( (ScSplitPos) i,
                static_cast<ScEditEngineDefaulter*>(pEditView->GetEditEngine()),
                pGridWin[i], GetViewData()->GetCurX(), GetViewData()->GetCurY() );
            if ( (ScSplitPos)i == eActive )
                pEditView->ShowCursor( FALSE );
        }
}

void ScTabView::KillEditView( BOOL bNoPaint )
{
    USHORT i;
    SCCOL nCol1 = aViewData.GetEditStartCol();
    SCROW nRow1 = aViewData.GetEditStartRow();
    SCCOL nCol2 = aViewData.GetEditEndCol();
    SCROW nRow2 = aViewData.GetEditEndRow();
    BOOL bPaint[4];
    BOOL bNotifyAcc(false);

    BOOL bExtended = nRow1 != nRow2;                    // Col wird sowieso bis zum Ende gezeichnet
    BOOL bAtCursor = nCol1 <= aViewData.GetCurX() &&
                     nCol2 >= aViewData.GetCurX() &&
                     nRow1 == aViewData.GetCurY();
    for (i=0; i<4; i++)
    {
        bPaint[i] = aViewData.HasEditView( (ScSplitPos) i );
        if (bPaint[i])
            bNotifyAcc = true;
    }

    // #108931#; notify accessibility before all things happen
    if ((bNotifyAcc) && (aViewData.GetViewShell()->HasAccessibilityObjects()))
        aViewData.GetViewShell()->BroadcastAccessibility(SfxSimpleHint(SC_HINT_ACC_LEAVEEDITMODE));

    aViewData.ResetEditView();
    for (i=0; i<4; i++)
        if (pGridWin[i] && bPaint[i])
            if (pGridWin[i]->IsVisible())
            {
                pGridWin[i]->ShowCursor();

                pGridWin[i]->SetMapMode(pGridWin[i]->GetDrawMapMode());

                // #i73567# the cell still has to be repainted
                if (bExtended || ( bAtCursor && !bNoPaint ))
                {
                    pGridWin[i]->Draw( nCol1, nRow1, nCol2, nRow2 );
                    pGridWin[i]->UpdateSelectionOverlay();
                }
            }

    if (pDrawView)
        DrawEnableAnim( TRUE );

        //  GrabFocus immer dann, wenn diese View aktiv ist und
        //  die Eingabezeile den Focus hat

    BOOL bGrabFocus = FALSE;
    if (aViewData.IsActive())
    {
        ScInputHandler* pInputHdl = SC_MOD()->GetInputHdl();
        if ( pInputHdl )
        {
            ScInputWindow* pInputWin = pInputHdl->GetInputWindow();
            if (pInputWin && pInputWin->IsInputActive())
                bGrabFocus = TRUE;
        }
    }

    if (bGrabFocus)
    {
//      So soll es gemacht werden, damit der Sfx es mitbekommt, klappt aber nicht:
//!     aViewData.GetViewShell()->GetViewFrame()->GetWindow().GrabFocus();
//      deshalb erstmal so:
        GetActiveWin()->GrabFocus();
    }

    //  Cursor-Abfrage erst nach GrabFocus

    for (i=0; i<4; i++)
        if (pGridWin[i] && pGridWin[i]->IsVisible())
        {
            Cursor* pCur = pGridWin[i]->GetCursor();
            if (pCur && pCur->IsVisible())
                pCur->Hide();

            if(bPaint[i])
            {
                pGridWin[i]->UpdateCursorOverlay();
                pGridWin[i]->UpdateAutoFillOverlay();
                // pGridWin[i]->UpdateAllOverlays();
            }
        }
}

void ScTabView::UpdateFormulas()
{
    if ( aViewData.GetDocument()->IsAutoCalcShellDisabled() )
        return ;

    USHORT i;
    for (i=0; i<4; i++)
        if (pGridWin[i])
            if (pGridWin[i]->IsVisible())
                pGridWin[i]->UpdateFormulas();

    if ( aViewData.IsPagebreakMode() )
        UpdatePageBreakData();              //! asynchron

    UpdateHeaderWidth();

    //  if in edit mode, adjust edit view area because widths/heights may have changed
    if ( aViewData.HasEditView( aViewData.GetActivePart() ) )
        UpdateEditView();
}

//  PaintCell - einzelne Zelle neu zeichnen

void ScTabView::PaintCell( SCCOL nCol, SCROW nRow, SCTAB nTab )
{
    if ( aViewData.GetTabNo() == nTab )
    {
        USHORT i;
        for (i=0; i<4; i++)
            if (pGridWin[i])
                if (pGridWin[i]->IsVisible())
                    pGridWin[i]->Draw( nCol, nRow, nCol, nRow );
    }
}

//  PaintArea -Block neu zeichnen

void ScTabView::PaintArea( SCCOL nStartCol, SCROW nStartRow, SCCOL nEndCol, SCROW nEndRow,
                            ScUpdateMode eMode )
{
    USHORT i;
    SCCOL nCol1;
    SCROW nRow1;
    SCCOL nCol2;
    SCROW nRow2;

    PutInOrder( nStartCol, nEndCol );
    PutInOrder( nStartRow, nEndRow );

    for (i=0; i<4; i++)
        if (pGridWin[i])
            if (pGridWin[i]->IsVisible())
            {
                ScHSplitPos eHWhich = WhichH( (ScSplitPos) i );
                ScVSplitPos eVWhich = WhichV( (ScSplitPos) i );
                BOOL bOut = FALSE;

                nCol1 = nStartCol;
                nRow1 = nStartRow;
                nCol2 = nEndCol;
                nRow2 = nEndRow;

                SCCOL nScrX = aViewData.GetPosX( eHWhich );
                SCROW nScrY = aViewData.GetPosY( eVWhich );
                if (nCol1 < nScrX) nCol1 = nScrX;
                if (nCol2 < nScrX)
                {
                    if ( eMode == SC_UPDATE_ALL )   // #91240# for UPDATE_ALL, paint anyway
                        nCol2 = nScrX;              // (because of extending strings to the right)
                    else
                        bOut = TRUE;                // completely outside the window
                }
                if (nRow1 < nScrY) nRow1 = nScrY;
                if (nRow2 < nScrY) bOut = TRUE;

                SCCOL nLastX = nScrX + aViewData.VisibleCellsX( eHWhich ) + 1;
                SCROW nLastY = nScrY + aViewData.VisibleCellsY( eVWhich ) + 1;
                if (nCol1 > nLastX) bOut = TRUE;
                if (nCol2 > nLastX) nCol2 = nLastX;
                if (nRow1 > nLastY) bOut = TRUE;
                if (nRow2 > nLastY) nRow2 = nLastY;

                if (!bOut)
                {
                    if ( eMode == SC_UPDATE_CHANGED )
                        pGridWin[i]->Draw( nCol1, nRow1, nCol2, nRow2, eMode );
                    else    // ALL oder MARKS
                    {
                        BOOL bLayoutRTL = aViewData.GetDocument()->IsLayoutRTL( aViewData.GetTabNo() );
                        long nLayoutSign = bLayoutRTL ? -1 : 1;

                        Point aStart = aViewData.GetScrPos( nCol1, nRow1, (ScSplitPos) i );
                        Point aEnd   = aViewData.GetScrPos( nCol2+1, nRow2+1, (ScSplitPos) i );
                        if ( eMode == SC_UPDATE_ALL )
                            aEnd.X() = bLayoutRTL ? 0 : (pGridWin[i]->GetOutputSizePixel().Width());
                        aEnd.X() -= nLayoutSign;
                        aEnd.Y() -= 1;

                        BOOL bShowChanges = TRUE;           //! ...
                        if (bShowChanges)
                        {
                            aStart.X() -= nLayoutSign;      // include change marks
                            aStart.Y() -= 1;
                        }

                        BOOL bMarkClipped = aViewData.GetOptions().GetOption( VOPT_CLIPMARKS );
                        if (bMarkClipped)
                        {
                            //  dazu muesste ScColumn::IsEmptyBlock optimiert werden
                            //  (auf Search() umstellen)
                            //!if ( nCol1 > 0 && !aViewData.GetDocument()->IsBlockEmpty(
                            //!                     aViewData.GetTabNo(),
                            //!                     0, nRow1, nCol1-1, nRow2 ) )
                            {
                                long nMarkPixel = (long)( SC_CLIPMARK_SIZE * aViewData.GetPPTX() );
                                aStart.X() -= nMarkPixel * nLayoutSign;
                                if (!bShowChanges)
                                    aStart.X() -= nLayoutSign;      // cell grid
                            }
                        }

                        pGridWin[i]->Invalidate( pGridWin[i]->PixelToLogic( Rectangle( aStart,aEnd ) ) );
                    }
                }
            }

    UpdateAllOverlays();
}

void ScTabView::PaintRangeFinder( long nNumber )
{
    ScInputHandler* pHdl = SC_MOD()->GetInputHdl( aViewData.GetViewShell() );
    if (pHdl)
    {
        ScRangeFindList* pRangeFinder = pHdl->GetRangeFindList();
        if ( pRangeFinder && pRangeFinder->GetDocName() == aViewData.GetDocShell()->GetTitle() )
        {
            BOOL bHide = pRangeFinder->IsHidden();

            SCTAB nTab = aViewData.GetTabNo();
            USHORT nCount = (USHORT)pRangeFinder->Count();
            for (USHORT i=0; i<nCount; i++)
                if ( nNumber < 0 || nNumber == i )
                {
                    ScRangeFindData* pData = pRangeFinder->GetObject(i);
                    if (pData)
                    {
                        ScRange aRef = pData->aRef;
                        aRef.Justify();                 // Justify fuer die Abfragen unten

                        if ( aRef.aStart == aRef.aEnd )     //! Tab ignorieren?
                            aViewData.GetDocument()->ExtendMerge(aRef);

                        if ( aRef.aStart.Tab() >= nTab && aRef.aEnd.Tab() <= nTab )
                        {
                            SCCOL nCol1 = aRef.aStart.Col();
                            SCROW nRow1 = aRef.aStart.Row();
                            SCCOL nCol2 = aRef.aEnd.Col();
                            SCROW nRow2 = aRef.aEnd.Row();
                            if ( bHide )
                            {
                                //  wegnehmen -> Repaint
                                //  SC_UPDATE_MARKS: Invalidate, nicht bis zum Zeilenende

                                BOOL bHiddenEdge = FALSE;
                                SCROW nTmp;
                                ScDocument* pDoc = aViewData.GetDocument();
                                while ( nCol1 > 0 && ( pDoc->GetColFlags( nCol1, nTab ) & CR_HIDDEN ) )
                                {
                                    --nCol1;
                                    bHiddenEdge = TRUE;
                                }
                                while ( nCol2 < MAXCOL && ( pDoc->GetColFlags( nCol2, nTab ) & CR_HIDDEN ) )
                                {
                                    ++nCol2;
                                    bHiddenEdge = TRUE;
                                }
                                nTmp = pDoc->GetRowFlagsArray( nTab).GetLastForCondition( 0, nRow1, CR_HIDDEN, 0);
                                if (!ValidRow(nTmp))
                                    nTmp = 0;
                                if (nTmp < nRow1)
                                {
                                    nRow1 = nTmp;
                                    bHiddenEdge = TRUE;
                                }
                                nTmp = pDoc->GetRowFlagsArray( nTab).GetFirstForCondition( nRow2, MAXROW, CR_HIDDEN, 0);
                                if (!ValidRow(nTmp))
                                    nTmp = MAXROW;
                                if (nTmp > nRow2)
                                {
                                    nRow2 = nTmp;
                                    bHiddenEdge = TRUE;
                                }

                                if ( nCol2 - nCol1 > 1 && nRow2 - nRow1 > 1 && !bHiddenEdge )
                                {
                                    //  nur an den Raendern entlang
                                    PaintArea( nCol1, nRow1, nCol2, nRow1, SC_UPDATE_MARKS );
                                    PaintArea( nCol1, nRow1+1, nCol1, nRow2-1, SC_UPDATE_MARKS );
                                    PaintArea( nCol2, nRow1+1, nCol2, nRow2-1, SC_UPDATE_MARKS );
                                    PaintArea( nCol1, nRow2, nCol2, nRow2, SC_UPDATE_MARKS );
                                }
                                else    // alles am Stueck
                                    PaintArea( nCol1, nRow1, nCol2, nRow2, SC_UPDATE_MARKS );
                            }
                            else
                            {
                                //  neuen Rahmen zeichnen

                                for (USHORT nWin=0; nWin<4; nWin++)
                                    if (pGridWin[nWin] && pGridWin[nWin]->IsVisible())
                                        pGridWin[nWin]->DrawRefMark( nCol1, nRow1, nCol2, nRow2,
                                                            Color( ScRangeFindList::GetColorName( i ) ),
                                                            TRUE );
                            }
                        }
                    }
                }
        }
    }
}

//  fuer Chart-Daten-Markierung

void ScTabView::AddHighlightRange( const ScRange& rRange, const Color& rColor )
{
    if (!pHighlightRanges)
        pHighlightRanges = new ScHighlightRanges;
    pHighlightRanges->Insert( new ScHighlightEntry( rRange, rColor ) );
    //! auf doppelte testen??

    SCTAB nTab = aViewData.GetTabNo();
    if ( nTab >= rRange.aStart.Tab() && nTab <= rRange.aEnd.Tab() )
        PaintArea( rRange.aStart.Col(), rRange.aStart.Row(),
                    rRange.aEnd.Col(), rRange.aEnd.Row(), SC_UPDATE_MARKS );
}

void ScTabView::ClearHighlightRanges()
{
    if (pHighlightRanges)
    {
        ScHighlightRanges* pTemp = pHighlightRanges;
        pHighlightRanges = NULL;    // Repaint ohne Highlight

        SCTAB nTab = aViewData.GetTabNo();
        ULONG nCount = pTemp->Count();
        for (ULONG i=0; i<nCount; i++)
        {
            ScHighlightEntry* pEntry = pTemp->GetObject( i );
            if (pEntry)
            {
                ScRange aRange = pEntry->aRef;
                if ( nTab >= aRange.aStart.Tab() && nTab <= aRange.aEnd.Tab() )
                    PaintArea( aRange.aStart.Col(), aRange.aStart.Row(),
                               aRange.aEnd.Col(), aRange.aEnd.Row(), SC_UPDATE_MARKS );
            }
        }
        delete pTemp;
    }
}

long ScTabView::DoChartSelection( ChartSelectionInfo &rInfo, const SchMemChart& rMemChart )
{
    long nFlags = rInfo.nSelection;
    if ( nFlags & CHART_SEL_QUERYSUPPORT )
        return nFlags & ( CHART_SEL_NONE | CHART_SEL_ALL   | CHART_SEL_COL |
                          CHART_SEL_ROW  | CHART_SEL_POINT | CHART_SEL_COLOR );

    long nRet = 0;
    ScChartArray aArr( aViewData.GetDocument(), rMemChart );
    if (aArr.IsValid())
    {
        if ( nFlags & CHART_SEL_NONE )
        {
            ClearHighlightRanges();
            nRet |= CHART_SEL_NONE;
        }

        Color aSelColor( COL_LIGHTBLUE );   // Default
        BOOL bManualColor = ( ( nFlags & CHART_SEL_COLOR ) != 0 );
        bManualColor = FALSE;                       //! Test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if ( bManualColor )
        {
            aSelColor = rInfo.aSelectionColor;
            nRet |= CHART_SEL_COLOR;
        }

        if ( nFlags & CHART_SEL_ALL )
        {
            ScRangeListRef xRanges = aArr.GetRangeList();
            if (xRanges.Is())
            {
                ULONG nCount = xRanges->Count();
                ULONG i;
                if (!bManualColor)
                {
                    ScBackgroundCollector aColl( aViewData.GetDocument() );
                    for (i=0; i<nCount; i++)
                        aColl.AddRange( *xRanges->GetObject(i) );
                    aSelColor = aColl.GetHighlightColor();
                }
                for (i=0; i<nCount; i++)
                    AddHighlightRange( *xRanges->GetObject(i), aSelColor );
                nRet |= CHART_SEL_ALL;
            }
        }

        if ( nFlags & ( CHART_SEL_COL | CHART_SEL_ROW | CHART_SEL_POINT ) )
        {
            //  irgendwelche Teile selektieren -> PositionMap holen
            const ScChartPositionMap* pPosMap = aArr.GetPositionMap();
            if (pPosMap)
            {
                if ( nFlags & CHART_SEL_COL )
                {
                    ScRangeListRef xRanges =
                        ((ScChartPositionMap*)pPosMap)->GetColRanges(
                            static_cast<SCSIZE>(rInfo.nCol));
                    if (xRanges.Is())
                    {
                        ULONG nCount = xRanges->Count();
                        ULONG i;
                        if (!bManualColor)
                        {
                            ScBackgroundCollector aColl( aViewData.GetDocument() );
                            for (i=0; i<nCount; i++)
                                aColl.AddRange( *xRanges->GetObject(i) );
                            aSelColor = aColl.GetHighlightColor();
                        }
                        for (i=0; i<nCount; i++)
                            AddHighlightRange( *xRanges->GetObject(i), aSelColor );
                        nRet |= CHART_SEL_COL;
                    }
                }
                if ( nFlags & CHART_SEL_ROW )
                {
                    ScRangeListRef xRanges =
                        ((ScChartPositionMap*)pPosMap)->GetRowRanges(
                            static_cast<SCSIZE>(rInfo.nRow));
                    if (xRanges.Is())
                    {
                        ULONG nCount = xRanges->Count();
                        ULONG i;
                        if (!bManualColor)
                        {
                            ScBackgroundCollector aColl( aViewData.GetDocument() );
                            for (i=0; i<nCount; i++)
                                aColl.AddRange( *xRanges->GetObject(i) );
                            aSelColor = aColl.GetHighlightColor();
                        }
                        for (i=0; i<nCount; i++)
                            AddHighlightRange( *xRanges->GetObject(i), aSelColor );
                        nRet |= CHART_SEL_ROW;
                    }
                }
                if ( nFlags & CHART_SEL_POINT )
                {
                    const ScAddress* pPos = pPosMap->GetPosition(
                            static_cast<SCSIZE>(rInfo.nCol),
                            static_cast<SCSIZE>(rInfo.nRow));

                    if (pPos)
                    {
                        if (!bManualColor)
                        {
                            ScBackgroundCollector aColl( aViewData.GetDocument() );
                            aColl.AddRange( ScRange(*pPos) );
                            aSelColor = aColl.GetHighlightColor();
                        }
                        AddHighlightRange( ScRange(*pPos), aSelColor );
                        nRet |= CHART_SEL_POINT;
                    }
                }
            }
        }
    }

    return nRet;
}

//  DrawDragRect - Drag&Drop-Rechteck zeichnen (XOR)

void ScTabView::DrawDragRect( SCCOL nStartX, SCROW nStartY, SCCOL nEndX, SCROW nEndY,
                                ScSplitPos ePos )
{
    if ( aViewData.GetHSplitMode() == SC_SPLIT_FIX || aViewData.GetVSplitMode() == SC_SPLIT_FIX )
    {
        for (USHORT  i=0; i<4; i++)
            if (pGridWin[i])
                if (pGridWin[i]->IsVisible())
                    pGridWin[i]->DrawDragRect( nStartX, nStartY, nEndX, nEndY );
    }
    else
        pGridWin[ePos]->DrawDragRect( nStartX, nStartY, nEndX, nEndY );
}

//  PaintGrid - Datenbereiche neu zeichnen

void ScTabView::PaintGrid()
{
    USHORT i;
    for (i=0; i<4; i++)
        if (pGridWin[i])
            if (pGridWin[i]->IsVisible())
                pGridWin[i]->Invalidate();
}

//  PaintTop - obere Kontrollelemente neu zeichnen

void ScTabView::PaintTop()
{
    USHORT i;
    for (i=0; i<2; i++)
    {
        if (pColBar[i])
            pColBar[i]->Invalidate();
        if (pColOutline[i])
            pColOutline[i]->Invalidate();
    }
}

void ScTabView::CreateAnchorHandles(SdrHdlList& rHdl, const ScAddress& rAddress)
{
    UINT16 i;

    for(i=0; i<4; i++)
    {
        if(pGridWin[i])
        {
            if(pGridWin[i]->IsVisible())
            {
                pGridWin[i]->CreateAnchorHandle(rHdl, rAddress);
            }
        }
    }
}

void ScTabView::PaintTopCol( SCCOL nCol )
{
    PaintTopArea( nCol, nCol );
}

void ScTabView::PaintTopArea( SCCOL nStartCol, SCCOL nEndCol )
{
        //  Pixel-Position der linken Kante

    if ( nStartCol < aViewData.GetPosX(SC_SPLIT_LEFT) ||
         nStartCol < aViewData.GetPosX(SC_SPLIT_RIGHT) )
        aViewData.RecalcPixPos();

        //  Fixierung anpassen (UpdateFixX setzt HSplitPos neu)

    if ( aViewData.GetHSplitMode() == SC_SPLIT_FIX && nStartCol < aViewData.GetFixPosX() )
        if (aViewData.UpdateFixX())
            RepeatResize();

        //  zeichnen

    if (nStartCol>0)
        --nStartCol;                //! allgemeiner ?

    BOOL bLayoutRTL = aViewData.GetDocument()->IsLayoutRTL( aViewData.GetTabNo() );
    long nLayoutSign = bLayoutRTL ? -1 : 1;

    for (USHORT i=0; i<2; i++)
    {
        ScHSplitPos eWhich = (ScHSplitPos) i;
        if (pColBar[eWhich])
        {
            Size aWinSize = pColBar[eWhich]->GetSizePixel();
            long nStartX = aViewData.GetScrPos( nStartCol, 0, eWhich ).X();
            long nEndX;
            if (nEndCol >= MAXCOL)
                nEndX = bLayoutRTL ? 0 : ( aWinSize.Width()-1 );
            else
                nEndX = aViewData.GetScrPos( nEndCol+1, 0, eWhich ).X() - nLayoutSign;
            pColBar[eWhich]->Invalidate(
                    Rectangle( nStartX, 0, nEndX, aWinSize.Height()-1 ) );
        }
        if (pColOutline[eWhich])
            pColOutline[eWhich]->Invalidate();
    }
}


//  PaintLeft - linke Kontrollelemente neu zeichnen

void ScTabView::PaintLeft()
{
    USHORT i;
    for (i=0; i<2; i++)
    {
        if (pRowBar[i])
            pRowBar[i]->Invalidate();
        if (pRowOutline[i])
            pRowOutline[i]->Invalidate();
    }
}

void ScTabView::PaintLeftRow( SCROW nRow )
{
    PaintLeftArea( nRow, nRow );
}

void ScTabView::PaintLeftArea( SCROW nStartRow, SCROW nEndRow )
{
        //  Pixel-Position der oberen Kante

    if ( nStartRow < aViewData.GetPosY(SC_SPLIT_TOP) ||
         nStartRow < aViewData.GetPosY(SC_SPLIT_BOTTOM) )
        aViewData.RecalcPixPos();

        //  Fixierung anpassen (UpdateFixY setzt VSplitPos neu)

    if ( aViewData.GetVSplitMode() == SC_SPLIT_FIX && nStartRow < aViewData.GetFixPosY() )
        if (aViewData.UpdateFixY())
            RepeatResize();

        //  zeichnen

    if (nStartRow>0)
        --nStartRow;

    for (USHORT i=0; i<2; i++)
    {
        ScVSplitPos eWhich = (ScVSplitPos) i;
        if (pRowBar[eWhich])
        {
            Size aWinSize = pRowBar[eWhich]->GetSizePixel();
            long nStartY = aViewData.GetScrPos( 0, nStartRow, eWhich ).Y();
            long nEndY;
            if (nEndRow >= MAXROW)
                nEndY = aWinSize.Height()-1;
            else
                nEndY = aViewData.GetScrPos( 0, nEndRow+1, eWhich ).Y() - 1;
            pRowBar[eWhich]->Invalidate(
                    Rectangle( 0, nStartY, aWinSize.Width()-1, nEndY ) );
        }
        if (pRowOutline[eWhich])
            pRowOutline[eWhich]->Invalidate();
    }
}

//  InvertBlockMark - Block invertieren

void ScTabView::InvertBlockMark(SCCOL nStartX, SCROW nStartY,
                                SCCOL nEndX, SCROW nEndY)
{
    if ( !aViewData.IsActive() )
        return;                                 // invertiert wird nur auf aktiver View

    PutInOrder( nStartX, nEndX );
    PutInOrder( nStartY, nEndY );

    ScMarkData& rMark = aViewData.GetMarkData();
    ScDocShell* pDocSh = aViewData.GetDocShell();
    ScDocument* pDoc = pDocSh->GetDocument();
    SCTAB nTab = aViewData.GetTabNo();

    if ( pDocSh->GetLockCount() )
    {
        //  if paint is locked, avoid repeated inverting
        //  add repaint areas to paint lock data instead
        pDocSh->PostPaint( nStartX,nStartY,nTab, nEndX,nEndY,nTab, PAINT_GRID );
        return;
    }

#ifdef MAC
    BOOL bSingle = TRUE;
#else
    BOOL bSingle = rMark.IsMultiMarked();
#endif

    BOOL bMerge = pDoc->HasAttrib( nStartX, nStartY, nTab, nEndX, nEndY, nTab,
                                    HASATTR_MERGED | HASATTR_OVERLAPPED );

    USHORT i;
    if ( bMerge || bSingle )
    {
        for (i=0; i<4; i++)
            if (pGridWin[i])
                if (pGridWin[i]->IsVisible())
                    pGridWin[i]->InvertSimple( nStartX, nStartY, nEndX, nEndY,
                                                bMerge, bBlockNeg );
    }
    else
    {
        for (i=0; i<4; i++)
            if (pGridWin[i])
                if (pGridWin[i]->IsVisible())
                {
                    ScSplitPos ePos = (ScSplitPos) i;
                    Point aStartPoint = aViewData.GetScrPos( nStartX, nStartY, ePos );
                    Point aEndPoint = aViewData.GetScrPos( nEndX+1, nEndY+1, ePos );
                    if ( pDoc->IsLayoutRTL( nTab ) )
                    {
                        long nTemp = aStartPoint.X();
                        aStartPoint.X() = aEndPoint.X() + 1;    // +1 - excluding start of nEndX+1
                        aEndPoint.X() = nTemp;
                    }
                    else
                        aEndPoint.X() -= 1;
                    aEndPoint.Y() -= 1;
                    if ( aEndPoint.X() >= aStartPoint.X() && aEndPoint.Y() >= aStartPoint.Y() )
                    {
                        MapMode aOld = pGridWin[ePos]->GetMapMode();
                        pGridWin[ePos]->SetMapMode(MAP_PIXEL);
                        pGridWin[ePos]->Invert( Rectangle(aStartPoint,aEndPoint), INVERT_HIGHLIGHT );
                        pGridWin[ePos]->SetMapMode(aOld);
                        pGridWin[ePos]->CheckInverted();
                    }
                }
    }

        //
        //  wenn Controls betroffen, neu malen
        //

    BOOL bHide = TRUE;                  // wird Teil der Markierung aufgehoben ?
    if (rMark.IsMarked())
    {
        ScRange aMarkRange;
        rMark.GetMarkArea( aMarkRange );
        if ( aMarkRange.aStart.Col() <= nStartX && aMarkRange.aEnd.Col() >= nEndX &&
             aMarkRange.aStart.Row() <= nStartY && aMarkRange.aEnd.Row() >= nEndY )
        {
            bHide = FALSE;              // der ganze Bereich ist markiert
        }
    }

    Rectangle aMMRect = pDoc->GetMMRect(nStartX,nStartY,nEndX,nEndY, nTab);
    BOOL bPaint = bHide && pDoc->HasControl( nTab, aMMRect );

    if (bPaint)
    {
        for (i=0; i<4; i++)
            if (pGridWin[i])
                if (pGridWin[i]->IsVisible())
                {
                    //  MapMode muss logischer (1/100mm) sein !!!
                    pDoc->InvalidateControls( pGridWin[i], nTab, aMMRect );
                    pGridWin[i]->Update();
                }
    }
}

BOOL ScTabView::PaintExtras()
{
    BOOL bRet = FALSE;
    ScDocument* pDoc = aViewData.GetDocument();
    SCTAB nTab = aViewData.GetTabNo();
    if (!pDoc->HasTable(nTab))                  // Tabelle geloescht ?
    {
        SCTAB nCount = pDoc->GetTableCount();
        aViewData.SetTabNo(nCount-1);
        bRet = TRUE;
    }
    pTabControl->UpdateStatus();                        // TRUE = active
    return bRet;
}

void ScTabView::RecalcPPT()
{
    //  called after changes that require the PPT values to be recalculated
    //  (currently from detective operations)

    double nOldX = aViewData.GetPPTX();
    double nOldY = aViewData.GetPPTY();

    Fraction aZoomX = aViewData.GetZoomX();
    Fraction aZoomY = aViewData.GetZoomY();
    aViewData.SetZoom( aZoomX, aZoomY );            // pre-calculate new PPT values

    BOOL bChangedX = ( aViewData.GetPPTX() != nOldX );
    BOOL bChangedY = ( aViewData.GetPPTY() != nOldY );
    if ( bChangedX || bChangedY )
    {
        //  call view SetZoom (including draw scale, split update etc)
        //  and paint only if values changed

        SetZoom( aZoomX, aZoomY );

        PaintGrid();
        if (bChangedX)
            PaintTop();
        if (bChangedY)
            PaintLeft();
    }
}

void ScTabView::ActivateView( BOOL bActivate, BOOL bFirst )
{
    if ( bActivate == aViewData.IsActive() && !bFirst )
    {
        //  keine Assertion mehr - kommt vor, wenn vorher im Drag&Drop
        //  auf ein anderes Dokument umgeschaltet wurde
        return;
    }

    // wird nur bei MDI-(De)Activate gerufen
    // aViewData.Activate hinten wegen Cursor-Show bei KillEditView
    //  Markierung nicht mehr loeschen - wenn an der ViewData Activate(FALSE) gesetzt ist,
    //  wird die Markierung nicht ausgegeben

    if (!bActivate)
    {
        ScModule* pScMod = SC_MOD();
        BOOL bRefMode = pScMod->IsFormulaMode();

            //  Referenzeingabe nicht abbrechen, um Referenzen auf
            //  andere Dokumente zuzulassen

        if (!bRefMode)
        {
            //pScMod->InputEnterHandler();

            //  #80843# pass view to GetInputHdl, this view may not be current anymore
            ScInputHandler* pHdl = SC_MOD()->GetInputHdl(aViewData.GetViewShell());
            if (pHdl)
                pHdl->EnterHandler();
        }
    }
    pTabControl->ActivateView(bActivate);
    PaintExtras();

    aViewData.Activate(bActivate);

    PaintBlock(FALSE);                  // Repaint, Markierung je nach Active-Status

    if (!bActivate)
        HideAllCursors();               // Cursor
    else if (!bFirst)
        ShowAllCursors();

    //HMHif (pDrawView)
    //HMH   DrawShowMarkHdl(bActivate);     // Drawing-Markierung

    if (bActivate)
    {
        if ( bFirst )
        {
            ScSplitPos eWin = aViewData.GetActivePart();
            DBG_ASSERT( pGridWin[eWin], "rottes Dokument, nicht alle SplitPos in GridWin" );
            if ( !pGridWin[eWin] )
            {
                eWin = SC_SPLIT_BOTTOMLEFT;
                if ( !pGridWin[eWin] )
                {
                    short i;
                    for ( i=0; i<4; i++ )
                    {
                        if ( pGridWin[i] )
                        {
                            eWin = (ScSplitPos) i;
                            break;  // for
                        }
                    }
                    DBG_ASSERT( i<4, "und BUMM" );
                }
                aViewData.SetActivePart( eWin );
            }
        }
        //  hier nicht mehr selber GrabFocus rufen!
        //  Wenn das Doc bearbeitet wird, ruft der Sfx selber GrabFocus am Fenster der Shell.
        //  Wenn es z.B. ein Mailbody ist, darf es den Focus nicht bekommen (Bug #43638#)

        UpdateInputContext();
    }
    else
        pGridWin[aViewData.GetActivePart()]->ClickExtern();
}

void ScTabView::ActivatePart( ScSplitPos eWhich )
{
    ScSplitPos eOld = aViewData.GetActivePart();
    if ( eOld != eWhich )
    {
        bInActivatePart = TRUE;

        BOOL bRefMode = SC_MOD()->IsFormulaMode();

        //  #40565# the HasEditView call during SetCursor would fail otherwise
        if ( aViewData.HasEditView(eOld) && !bRefMode )
            UpdateInputLine();

        ScHSplitPos eOldH = WhichH(eOld);
        ScVSplitPos eOldV = WhichV(eOld);
        ScHSplitPos eNewH = WhichH(eWhich);
        ScVSplitPos eNewV = WhichV(eWhich);
        BOOL bTopCap  = pColBar[eOldH] && pColBar[eOldH]->IsMouseCaptured();
        BOOL bLeftCap = pRowBar[eOldV] && pRowBar[eOldV]->IsMouseCaptured();

        BOOL bFocus = pGridWin[eOld]->HasFocus();
        BOOL bCapture = pGridWin[eOld]->IsMouseCaptured();
        if (bCapture)
            pGridWin[eOld]->ReleaseMouse();
        pGridWin[eOld]->ClickExtern();
        pGridWin[eOld]->HideCursor();
        pGridWin[eWhich]->HideCursor();
        aViewData.SetActivePart( eWhich );

        ScTabViewShell* pShell = aViewData.GetViewShell();
        pShell->WindowChanged();

        pSelEngine->SetWindow(pGridWin[eWhich]);
        pSelEngine->SetWhich(eWhich);
        pSelEngine->SetVisibleArea( Rectangle(Point(), pGridWin[eWhich]->GetOutputSizePixel()) );

        pGridWin[eOld]->MoveMouseStatus(*pGridWin[eWhich]);

        if ( bCapture || pGridWin[eWhich]->IsMouseCaptured() )
        {
            //  Tracking statt CaptureMouse, damit sauber abgebrochen werden kann
            //  (SelectionEngine ruft CaptureMouse beim SetWindow)
            //! Irgendwann sollte die SelectionEngine selber StartTracking rufen!?!
            pGridWin[eWhich]->ReleaseMouse();
            pGridWin[eWhich]->StartTracking();
        }

        if ( bTopCap && pColBar[eNewH] )
        {
            pColBar[eOldH]->SetIgnoreMove(TRUE);
            pColBar[eNewH]->SetIgnoreMove(FALSE);
            pHdrSelEng->SetWindow( pColBar[eNewH] );
            long nWidth = pColBar[eNewH]->GetOutputSizePixel().Width();
            pHdrSelEng->SetVisibleArea( Rectangle( 0, LONG_MIN, nWidth-1, LONG_MAX ) );
            pColBar[eNewH]->CaptureMouse();
        }
        if ( bLeftCap && pRowBar[eNewV] )
        {
            pRowBar[eOldV]->SetIgnoreMove(TRUE);
            pRowBar[eNewV]->SetIgnoreMove(FALSE);
            pHdrSelEng->SetWindow( pRowBar[eNewV] );
            long nHeight = pRowBar[eNewV]->GetOutputSizePixel().Height();
            pHdrSelEng->SetVisibleArea( Rectangle( LONG_MIN, 0, LONG_MAX, nHeight-1 ) );
            pRowBar[eNewV]->CaptureMouse();
        }
        aHdrFunc.SetWhich(eWhich);

        pGridWin[eOld]->ShowCursor();
        pGridWin[eWhich]->ShowCursor();

        //  #103823# don't switch ViewShell's active window during RefInput, because the focus
        //  might change, and subsequent SetReference calls wouldn't find the right EditView
        if ( !bRefMode )
            aViewData.GetViewShell()->SetWindow( pGridWin[eWhich] );

        if ( bFocus && !aViewData.IsAnyFillMode() && !bRefMode )
        {
            //  GrabFocus nur, wenn vorher das andere GridWindow den Focus hatte
            //  (z.B. wegen Suchen & Ersetzen)
//!         aViewData.GetViewShell()->GetViewFrame()->GetWindow().GrabFocus();
            pGridWin[eWhich]->GrabFocus();
        }

        bInActivatePart = FALSE;
    }
}

void ScTabView::HideListBox()
{
    for (USHORT i=0; i<4; i++)
        if (pGridWin[i])
            pGridWin[i]->ClickExtern();
}

void ScTabView::UpdateInputContext()
{
    ScGridWindow* pWin = pGridWin[aViewData.GetActivePart()];
    if (pWin)
        pWin->UpdateInputContext();
}

//  GetGridWidth - Breite eines Ausgabebereichs (fuer ViewData)

long ScTabView::GetGridWidth( ScHSplitPos eWhich )
{
    ScSplitPos eGridWhich = ( eWhich == SC_SPLIT_LEFT ) ? SC_SPLIT_BOTTOMLEFT : SC_SPLIT_BOTTOMRIGHT;
    if (pGridWin[eGridWhich])
        return pGridWin[eGridWhich]->GetSizePixel().Width();
    else
        return 0;
}

//  GetGridHeight - Hoehe eines Ausgabebereichs (fuer ViewData)

long ScTabView::GetGridHeight( ScVSplitPos eWhich )
{
    ScSplitPos eGridWhich = ( eWhich == SC_SPLIT_TOP ) ? SC_SPLIT_TOPLEFT : SC_SPLIT_BOTTOMLEFT;
    if (pGridWin[eGridWhich])
        return pGridWin[eGridWhich]->GetSizePixel().Height();
    else
        return 0;
}

void ScTabView::UpdateInputLine()
{
    SC_MOD()->InputEnterHandler();
}

void ScTabView::ZoomChanged()
{
    ScInputHandler* pHdl = SC_MOD()->GetInputHdl(aViewData.GetViewShell());
    if (pHdl)
        pHdl->SetRefScale( aViewData.GetZoomX(), aViewData.GetZoomY() );

    UpdateFixPos();

    UpdateScrollBars();

    //  VisArea...
    // AW: Discussed with NN if there is a reason that new map mode was only set for one window,
    // but is not. Setting only on one window causes the first repaint to have the old mapMode
    // in three of four views, so the overlay will save the wrong content e.g. when zooming out.
    // Changing to setting map mode at all windows.
    sal_uInt32 a;

    for(a = 0L; a < 4L; a++)
    {
        if(pGridWin[a])
        {
            pGridWin[a]->SetMapMode(pGridWin[a]->GetDrawMapMode());
        }
    }

    SetNewVisArea();

    /* the old code
    ScGridWindow* pWin = pGridWin[aViewData.GetActivePart()];
    if (pWin)
    {
        pWin->SetMapMode( pWin->GetDrawMapMode() ); // mit neuem Zoom
        SetNewVisArea();                            // benutzt den gesetzten MapMode
    } */

    InterpretVisible();     // #69343# have everything calculated before painting

    SfxBindings& rBindings = aViewData.GetBindings();
    rBindings.Invalidate( SID_ATTR_ZOOM );

    HideNoteMarker();

    // AW: To not change too much, use pWin here
    ScGridWindow* pWin = pGridWin[aViewData.GetActivePart()];

    if ( pWin && aViewData.HasEditView( aViewData.GetActivePart() ) )
    {
        // flush OverlayManager before changing the MapMode
        pWin->flushOverlayManager();

        //  #93650# make sure the EditView's position and size are updated
        //  with the right (logic, not drawing) MapMode
        pWin->SetMapMode( aViewData.GetLogicMode() );
        UpdateEditView();
    }
}

void ScTabView::CheckNeedsRepaint()
{
    USHORT i;
    for (i=0; i<4; i++)
        if ( pGridWin[i] && pGridWin[i]->IsVisible() )
            pGridWin[i]->CheckNeedsRepaint();
}





