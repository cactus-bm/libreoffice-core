/*************************************************************************
 *
 *  $RCSfile: drviews5.cxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-24 17:06:22 $
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

#ifndef _OUTLINER_HXX
#include <svx/outliner.hxx>
#endif
#ifndef _SVXIDS_HXX
#include <svx/svxids.hrc>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _SV_SCRBAR_HXX //autogen
#include <vcl/scrbar.hxx>
#endif
#ifndef _SV_POLY_HXX //autogen
#include <vcl/poly.hxx>
#endif
#ifndef _SVX_FMSHELL_HXX //autogen
#include <svx/fmshell.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_COLORCFG_HXX
#include <svtools/colorcfg.hxx>
#endif
#ifndef _SD_ACCESSIBILITY_ACCESSIBLE_DRAW_DOCUMENT_VIEW_HXX
#include "AccessibleDrawDocumentView.hxx"
#endif


#include "strings.hrc"
#include "res_bmp.hrc"
#include "glob.hrc"
#include "app.hrc"

#include "optsitem.hxx"
#include "app.hxx"
#include "frmview.hxx"
#include "sdattr.hxx"
#include "futext.hxx"
#include "sdpage.hxx"
#include "stlpool.hxx"
#include "prntopts.hxx"
#include "sdresid.hxx"
#include "sdwindow.hxx"
#include "drawview.hxx"
#include "drawdoc.hxx"
#include "docshell.hxx"
#include "drviewsh.hxx"
#include "sdoutl.hxx"
#include "sdclient.hxx"
#include "fuslshow.hxx"
#include "unokywds.hxx"
#include "SdUnoDrawView.hxx"

#define TABCONTROL_INITIAL_SIZE     350
#define PAPER_SHADOW_EXT_PIXEL        2

/*************************************************************************
|*
|* Wird gerufen, wenn sich das Model aendert
|*
\************************************************************************/

void SdDrawViewShell::ModelHasChanged()
{
    Invalidate();
    // Damit der Navigator auch einen aktuellen Status bekommt
    GetViewFrame()->GetBindings().Invalidate( SID_NAVIGATOR_STATE, TRUE, FALSE );

    // Damit das Effekte-Window die Reihenfolge updaten kann
    UpdateEffectWindow();

    //Update3DWindow();
    SfxBoolItem aItem( SID_3D_STATE, TRUE );
    GetViewFrame()->GetDispatcher()->Execute(
        SID_3D_STATE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

    // jetzt den von der Drawing Engine neu erzeugten TextEditOutliner
    // initialisieren
    Outliner* pOutliner       = pDrView->GetTextEditOutliner();
    if (pOutliner)
    {
        SfxStyleSheetPool* pSPool = (SfxStyleSheetPool*) pDocSh->GetStyleSheetPool();
        pOutliner->SetStyleSheetPool(pSPool);
    }
}

/*************************************************************************
|*
|* Ersatz fuer AdjustPosSizePixel ab Sfx 248a
|*
\************************************************************************/

void SdDrawViewShell::InnerResizePixel(const Point &rPos, const Size &rSize)
{
    if (!pFuSlideShow)
    {
        // Da die innere Groesse vorgegeben wird, ist ein Zoomen auf
        // Seitenbreite nicht erlaubt
        bZoomOnPage = FALSE;

        SdViewShell::InnerResizePixel(rPos, rSize);

        SetZoomRect( pDocSh->GetVisArea(ASPECT_CONTENT) );
    }
}

/*************************************************************************
|*
|* Ersatz fuer AdjustPosSizePixel ab Sfx 248a
|*
\************************************************************************/

void SdDrawViewShell::OuterResizePixel(const Point &rPos, const Size &rSize)
{
    if (!pFuSlideShow || !pFuSlideShow->IsFullScreen())
    {
        SdViewShell::OuterResizePixel(rPos, rSize);

        if ( pDocSh->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        {
            SetZoomRect( pDocSh->GetVisArea(ASPECT_CONTENT) );
        }
    }

    FuSlideShow* pSlideShow = ( pFuSlideShow ? pFuSlideShow : ( pDrView ? pDrView->GetSlideShow() : NULL ) );

    if( pSlideShow && !pSlideShow->IsFullScreen())
        pSlideShow->Resize( rSize );
}


/** This call is simply delegated to the base class.
*/
void SdDrawViewShell::AdjustPosSizePixel(const Point &rNewPos,
                                         const Size &rNewSize)
{
    SdViewShell::AdjustPosSizePixel (rNewPos, rNewSize);
}



void SdDrawViewShell::ArrangeGUIElements ()
{
    Size aTabSize = aTabControl.GetSizePixel();

    // Retrieve the current size (thickness) of the scroll bars.  That is
    // the width of the vertical and the height of the horizontal scroll
    // bar.
    int nScrollBarSize = GetViewFrame()->GetWindow().GetSettings().GetStyleSettings().GetScrollBarSize();
    aScrBarWH = Size (nScrollBarSize, nScrollBarSize);

    if( 0 == aTabSize.Width() )
    {
        if( 0.0 == pFrameView->GetTabCtrlPercent() )
            aTabSize.Width() = TABCONTROL_INITIAL_SIZE;
        else
        {
            const Size aFrameSize( GetViewFrame()->GetWindow().GetOutputSizePixel() );
            aTabSize.Width() = FRound( aFrameSize.Width() * pFrameView->GetTabCtrlPercent() );
        }
    }

    aTabSize.Height() = aScrBarWH.Height();
    Point aHPos = aViewPos;
    aHPos.Y() += aViewSize.Height() - aTabSize.Height();

    Size aBtnSize(aScrBarWH);
    aPageBtn.SetPosSizePixel(aHPos, aBtnSize);
    aHPos.X() += aBtnSize.Width();
    aMasterPageBtn.SetPosSizePixel(aHPos, aBtnSize);
    aHPos.X() += aBtnSize.Width();
    aLayerBtn.SetPosSizePixel(aHPos, aBtnSize);
    aHPos.X() += aBtnSize.Width();

    aTabControl.SetSizePixel(aTabSize);

    SdViewShell::ArrangeGUIElements ();

    aTabSize.Width() = pHScrlArray[0]->GetPosPixel().X() - aHPos.X();
    if ( aTabSize.Width() < 0 )
        aTabSize.Width() = 0;

    aTabControl.SetPosSizePixel(aHPos, aTabSize);
    aLayerTab.SetPosSizePixel(aHPos, aTabSize);

    SdClient*   pIPClient = (SdClient*)GetIPClient();
    BOOL bClientActive = FALSE;
    if ( pIPClient && pIPClient->IsInPlaceActive() )
        bClientActive = TRUE;

    BOOL bInPlaceActive = pDocSh->GetProtocol().IsInPlaceActive();

//        if ( bZoomOnPage && pDocSh->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
    if ( bZoomOnPage && !bInPlaceActive && !bClientActive )
    {
        // bei Split immer erstes Fenster resizen
        pWindow = pWinArray[0][0];
        SfxRequest aReq(SID_SIZE_PAGE, 0, pDoc->GetItemPool());
        ExecuteSlot( aReq );
    }
}

/*************************************************************************
|*
|* Daten der FrameView auf die aktuelle View uebertragen
|*
\************************************************************************/

void SdDrawViewShell::ReadFrameViewData(FrameView* pView)
{
    BOOL bChanged = pDoc->IsChanged();      // merken

    // Diese Option wird am Model eingestellt
    pDoc->SetPickThroughTransparentTextFrames(
             SD_MOD()->GetSdOptions(pDoc->GetDocumentType())->IsPickThrough());

    // Initialisierungen der Zeichen-(Bildschirm-)Attribute
    if (HasRuler() != pView->HasRuler())
        SetRuler( pView->HasRuler() );

    if (pDrView->GetGridCoarse() != pView->GetGridCoarse())
        pDrView->SetGridCoarse( pView->GetGridCoarse() );

    if (pDrView->GetGridFine() != pView->GetGridFine())
        pDrView->SetGridFine( pView->GetGridFine() );

    if (pDrView->GetSnapGrid() != pView->GetSnapGrid())
        pDrView->SetSnapGrid( pView->GetSnapGrid() );

    if (pDrView->GetSnapGridWidthX() != pView->GetSnapGridWidthX() || pDrView->GetSnapGridWidthY() != pView->GetSnapGridWidthY())
        pDrView->SetSnapGridWidth(pView->GetSnapGridWidthX(), pView->GetSnapGridWidthY());

    if (pDrView->IsGridVisible() != pView->IsGridVisible())
        pDrView->SetGridVisible( pView->IsGridVisible() );

    if (pDrView->IsGridFront() != pView->IsGridFront())
        pDrView->SetGridFront( pView->IsGridFront() );

    if (pDrView->GetSnapAngle() != pView->GetSnapAngle())
        pDrView->SetSnapAngle( pView->GetSnapAngle() );

    if (pDrView->IsGridSnap() !=  pView->IsGridSnap() )
        pDrView->SetGridSnap( pView->IsGridSnap() );

    if (pDrView->IsBordSnap() !=  pView->IsBordSnap() )
        pDrView->SetBordSnap( pView->IsBordSnap() );

    if (pDrView->IsHlplSnap() !=  pView->IsHlplSnap() )
        pDrView->SetHlplSnap( pView->IsHlplSnap() );

    if (pDrView->IsOFrmSnap() !=  pView->IsOFrmSnap() )
        pDrView->SetOFrmSnap( pView->IsOFrmSnap() );

    if (pDrView->IsOPntSnap() !=  pView->IsOPntSnap() )
        pDrView->SetOPntSnap( pView->IsOPntSnap() );

    if (pDrView->IsOConSnap() !=  pView->IsOConSnap() )
        pDrView->SetOConSnap( pView->IsOConSnap() );

    if (pDrView->IsHlplVisible() != pView->IsHlplVisible() )
        pDrView->SetHlplVisible( pView->IsHlplVisible() );

    if (pDrView->IsDragStripes() != pView->IsDragStripes() )
        pDrView->SetDragStripes( pView->IsDragStripes() );

    if (pDrView->IsPlusHandlesAlwaysVisible() != pView->IsPlusHandlesAlwaysVisible() )
        pDrView->SetPlusHandlesAlwaysVisible( pView->IsPlusHandlesAlwaysVisible() );

    if (pDrView->GetSnapMagneticPixel() != pView->GetSnapMagneticPixel() )
        pDrView->SetSnapMagneticPixel( pView->GetSnapMagneticPixel() );

    if (pDrView->IsMarkedHitMovesAlways() != pView->IsMarkedHitMovesAlways() )
        pDrView->SetMarkedHitMovesAlways( pView->IsMarkedHitMovesAlways() );

    if (pDrView->IsMoveOnlyDragging() != pView->IsMoveOnlyDragging() )
        pDrView->SetMoveOnlyDragging( pView->IsMoveOnlyDragging() );

//    pDrView->SetSlantButShear( pView->IsSlantButShear() );

    if (pDrView->IsNoDragXorPolys() != pView->IsNoDragXorPolys() )
        pDrView->SetNoDragXorPolys( pView->IsNoDragXorPolys() );

    if (pDrView->IsCrookNoContortion() != pView->IsCrookNoContortion() )
        pDrView->SetCrookNoContortion( pView->IsCrookNoContortion() );

    if (pDrView->IsAngleSnapEnabled() != pView->IsAngleSnapEnabled() )
        pDrView->SetAngleSnapEnabled( pView->IsAngleSnapEnabled() );

    if (pDrView->IsBigOrtho() != pView->IsBigOrtho() )
        pDrView->SetBigOrtho( pView->IsBigOrtho() );

    if (pDrView->IsOrtho() != pView->IsOrtho() )
        pDrView->SetOrtho( pView->IsOrtho() );

    if (pDrView->GetEliminatePolyPointLimitAngle() != pView->GetEliminatePolyPointLimitAngle() )
        pDrView->SetEliminatePolyPointLimitAngle( pView->GetEliminatePolyPointLimitAngle() );

    if (pDrView->IsEliminatePolyPoints() != pView->IsEliminatePolyPoints() )
        pDrView->SetEliminatePolyPoints( pView->IsEliminatePolyPoints() );

    if (pDrView->IsLineDraft() != pView->IsLineDraft() )
        pDrView->SetLineDraft( pView->IsLineDraft() );

    if (pDrView->IsFillDraft() != pView->IsFillDraft() )
    {
        pDrView->ReleaseMasterPagePaintCache();
        pDrView->SetFillDraft( pView->IsFillDraft() );
    }

    if (pDrView->IsTextDraft() != pView->IsTextDraft() )
        pDrView->SetTextDraft( pView->IsTextDraft() );

    if (pDrView->IsGrafDraft() != pView->IsGrafDraft() )
        pDrView->SetGrafDraft( pView->IsGrafDraft() );

    if (pDrView->IsSolidMarkHdl() != pView->IsSolidMarkHdl() )
        pDrView->SetSolidMarkHdl( pView->IsSolidMarkHdl() );

    if (pDrView->IsSolidDragging() != pView->IsSolidDragging() )
        pDrView->SetSolidDragging( pView->IsSolidDragging() );

    if (pDrView->IsQuickTextEditMode() != pView->IsQuickEdit())
        pDrView->SetQuickTextEditMode( pView->IsQuickEdit() );

    // Definition:
    //  grosse Handles: 9
    //  kleine Handles: 7
    USHORT nTmp = pDrView->GetMarkHdlSizePixel();
    //DBG_ASSERT(nTmp != 7, "HandleSize != 7 oder 9");
    if( nTmp == 9 && !pView->IsBigHandles() )
        pDrView->SetMarkHdlSizePixel( 7 );
    else if( nTmp == 7 && pView->IsBigHandles() )
        pDrView->SetMarkHdlSizePixel( 9 );


    SdrPageView* pPageView = pDrView->GetPageViewPvNum(0);

    if (pPageView)
    {
        if ( pPageView->GetVisibleLayers() != pView->GetVisibleLayers() )
            pPageView->SetVisibleLayers( pView->GetVisibleLayers() );

        if ( pPageView->GetPrintableLayers() != pView->GetPrintableLayers() )
            pPageView->SetPrintableLayers( pView->GetPrintableLayers() );

        if ( pPageView->GetLockedLayers() != pView->GetLockedLayers() )
            pPageView->SetLockedLayers( pView->GetLockedLayers() );

        if (ePageKind == PK_NOTES)
        {
            if (pPageView->GetHelpLines() != pView->GetNotesHelpLines())
                pPageView->SetHelpLines( pView->GetNotesHelpLines() );
        }
        else if (ePageKind == PK_HANDOUT)
        {
            if (pPageView->GetHelpLines() != pView->GetHandoutHelpLines())
                pPageView->SetHelpLines( pView->GetHandoutHelpLines() );
        }
        else
        {
            if (pPageView->GetHelpLines() != pView->GetStandardHelpLines())
                pPageView->SetHelpLines( pView->GetStandardHelpLines() );
        }
    }

    if ( pDrView->GetActiveLayer() != pView->GetActiveLayer() )
        pDrView->SetActiveLayer( pView->GetActiveLayer() );

    USHORT nSelectedPage = 0;

    if (ePageKind != PK_HANDOUT)
    {
        nSelectedPage = pView->GetSelectedPage();
    }

    EditMode eNewEditMode = pView->GetViewShEditMode(ePageKind);
    BOOL bNewLayerMode = pView->IsLayerMode();
    ChangeEditMode(eNewEditMode, bNewLayerMode);
    SwitchPage(nSelectedPage);

    // DrawMode fuer 'Normales' Fenster wiederherstellen
    if(pWindow->GetDrawMode() != pView->GetDrawMode())
      pWindow->SetDrawMode(pView->GetDrawMode());

    if ( pDrView->IsDesignMode() != pView->IsDesignMode() )
    {
        SfxBoolItem aDesignModeItem( SID_FM_DESIGN_MODE, pView->IsDesignMode() );
        GetViewFrame()->GetDispatcher()->Execute( SID_FM_DESIGN_MODE, SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_RECORD, &aDesignModeItem, 0L );
    }

    // Muss am Ende gerufen werden, da ein WriteFrameViewData() ausgeloest wird
    if (pDrView->IsFrameDragSingles() != pView->IsFrameDragSingles() )
        pDrView->SetFrameDragSingles( pView->IsFrameDragSingles() );

    pDoc->SetChanged( bChanged );
}

/*************************************************************************
|*
|* Daten der aktuellen View auf die FrameView uebertragen
|*
\************************************************************************/

void SdDrawViewShell::WriteFrameViewData()
{
    // Zeichen-(Bildschirm-)Attribute an FrameView merken
    pFrameView->SetRuler( HasRuler() );
    pFrameView->SetGridCoarse( pDrView->GetGridCoarse() );
    pFrameView->SetGridFine( pDrView->GetGridFine() );
    pFrameView->SetSnapGrid( pDrView->GetSnapGrid() );
    pFrameView->SetSnapGridWidth(pDrView->GetSnapGridWidthX(), pDrView->GetSnapGridWidthY());
    pFrameView->SetGridVisible( pDrView->IsGridVisible() );
    pFrameView->SetGridFront( pDrView->IsGridFront() );
    pFrameView->SetSnapAngle( pDrView->GetSnapAngle() );
    pFrameView->SetGridSnap( pDrView->IsGridSnap() );
    pFrameView->SetBordSnap( pDrView->IsBordSnap() );
    pFrameView->SetHlplSnap( pDrView->IsHlplSnap() );
    pFrameView->SetOFrmSnap( pDrView->IsOFrmSnap() );
    pFrameView->SetOPntSnap( pDrView->IsOPntSnap() );
    pFrameView->SetOConSnap( pDrView->IsOConSnap() );
    pFrameView->SetHlplVisible( pDrView->IsHlplVisible() );
    pFrameView->SetDragStripes( pDrView->IsDragStripes() );
    pFrameView->SetPlusHandlesAlwaysVisible( pDrView->IsPlusHandlesAlwaysVisible() );
    pFrameView->SetFrameDragSingles( pDrView->IsFrameDragSingles() );
    pFrameView->SetMarkedHitMovesAlways( pDrView->IsMarkedHitMovesAlways() );
    pFrameView->SetMoveOnlyDragging( pDrView->IsMoveOnlyDragging() );
    pFrameView->SetNoDragXorPolys( pDrView->IsNoDragXorPolys() );
    pFrameView->SetCrookNoContortion( pDrView->IsCrookNoContortion() );
    pFrameView->SetBigOrtho( pDrView->IsBigOrtho() );
    pFrameView->SetEliminatePolyPointLimitAngle( pDrView->GetEliminatePolyPointLimitAngle() );
    pFrameView->SetEliminatePolyPoints( pDrView->IsEliminatePolyPoints() );

    pFrameView->SetLineDraft( pDrView->IsLineDraft() );
    pFrameView->SetFillDraft( pDrView->IsFillDraft() );
    pFrameView->SetTextDraft( pDrView->IsTextDraft() );
    pFrameView->SetGrafDraft( pDrView->IsGrafDraft() );
    pFrameView->SetSolidMarkHdl( pDrView->IsSolidMarkHdl() );
    pFrameView->SetSolidDragging( pDrView->IsSolidDragging() );
    pFrameView->SetQuickEdit( pDrView->IsQuickTextEditMode() );

    pFrameView->SetDesignMode( pDrView->IsDesignMode() );

    Size aVisSizePixel = pWindow->GetOutputSizePixel();
    Rectangle aVisArea = pWindow->PixelToLogic( Rectangle( Point(0,0), aVisSizePixel) );
    pFrameView->SetVisArea(aVisArea);

    if( ePageKind == PK_HANDOUT )
        pFrameView->SetSelectedPage(0);
    else
    {
        if( pFuSlideShow && ( pFuSlideShow->GetAnimationMode() == ANIMATIONMODE_SHOW ) )
            pFrameView->SetSelectedPage( pFuSlideShow->GetCurrentPage() );
        else
            pFrameView->SetSelectedPage( aTabControl.GetCurPageId() - 1 );
    }

    pFrameView->SetViewShEditMode(eEditMode, ePageKind);
    pFrameView->SetLayerMode(bLayerMode);

    SdrPageView* pPageView = pDrView->GetPageViewPvNum(0);

    if (pPageView)
    {
        if ( pFrameView->GetVisibleLayers() != pPageView->GetVisibleLayers() )
            pFrameView->SetVisibleLayers( pPageView->GetVisibleLayers() );

        if ( pFrameView->GetPrintableLayers() != pPageView->GetPrintableLayers() )
            pFrameView->SetPrintableLayers( pPageView->GetPrintableLayers() );

        if ( pFrameView->GetLockedLayers() != pPageView->GetLockedLayers() )
            pFrameView->SetLockedLayers( pPageView->GetLockedLayers() );

        if (ePageKind == PK_NOTES)
        {
            pFrameView->SetNotesHelpLines( pPageView->GetHelpLines() );
        }
        else if (ePageKind == PK_HANDOUT)
        {
            pFrameView->SetHandoutHelpLines( pPageView->GetHelpLines() );
        }
        else
        {
            pFrameView->SetStandardHelpLines( pPageView->GetHelpLines() );
        }
    }

    if ( pFrameView->GetActiveLayer() != pDrView->GetActiveLayer() )
        pFrameView->SetActiveLayer( pDrView->GetActiveLayer() );

    // DrawMode fuer 'Normales' Fenster merken
    if(pFrameView->GetDrawMode() != pWindow->GetDrawMode())
      pFrameView->SetDrawMode(pWindow->GetDrawMode());

    // remember tabcontrol extent as percentage of whole frame width
    const Size aFrameSize( GetViewFrame()->GetWindow().GetOutputSizePixel() );

    if( aFrameSize.Width() )
        pFrameView->SetTabCtrlPercent( (double) aTabControl.GetSizePixel().Width() / aFrameSize.Width() );
    else
        pFrameView->SetTabCtrlPercent( 0.0 );
}



/*************************************************************************
|*
|* Paint-Methode: das Ereignis wird vom Fenster pWin an
|* die Viewshell und die aktuelle Funktion weitergeleitet
|*
|* Anmerkung: pWin==NULL, wenn Paint() vom ShowWindow gerufen wird!
|*
\************************************************************************/

void SdDrawViewShell::Paint(const Rectangle& rRect, SdWindow* pWin)
{
    // #103834# Fill var FillColor here to have it available on later call
    svtools::ColorConfig aColorConfig;
    Color aFillColor;

    if(DOCUMENT_TYPE_IMPRESS == pDoc->GetDocumentType())
    {
        aFillColor = Color( aColorConfig.GetColorValue( svtools::APPBACKGROUND ).nColor );
    }
    else
    {
        aFillColor = Color( aColorConfig.GetColorValue( svtools::DOCCOLOR ).nColor );
    }

    if( pWin )
    {
        if( !pFuSlideShow || pWin != (SdWindow*) pFuSlideShow->GetShowWindow() )
        {
            const SdrPageView*  pPageView = pDrView->GetPageViewPvNum( 0 );
            const Color         aOldLineColor( pWin->GetLineColor() );
            const Color         aOldFillColor( pWin->GetFillColor() );
            const ULONG         nOldDrawMode( pWin->GetDrawMode() );
            const BOOL          bOldMap = pWin->IsMapModeEnabled();
            Point               aNullPoint;
            const Rectangle     aOutputRect( aNullPoint, pWin->GetOutputSizePixel() );
            Rectangle           aOutputPaperRect, aPaperRect;

            if( pPageView )
                aPaperRect = pPageView->GetPageRect();
            else
                aPaperRect = Rectangle( aNullPoint, pActualPage->GetSize() );

            ( aOutputPaperRect = aPaperRect = pWin->LogicToPixel( aPaperRect ) ).Intersection( aOutputRect );

            pWin->EnableMapMode( FALSE );
            pWin->SetDrawMode( DRAWMODE_DEFAULT );
            pWin->SetLineColor();
            pWin->SetFillColor( aFillColor );

            if( aOutputPaperRect.IsEmpty() )
                pWin->DrawRect( aOutputRect );
            else
            {
                const Color aShadowColor( aColorConfig.GetColorValue( svtools::FONTCOLOR ).nColor );
                PolyPolygon aPolyPoly( 2 );
                Rectangle   aOutputRectExt( aOutputRect );

                aOutputRectExt.Left()--;
                aOutputRectExt.Top()--;
                aOutputRectExt.Right()++;
                aOutputRectExt.Bottom()++;

                aPolyPoly.Insert( aOutputRectExt );
                aPolyPoly.Insert( aOutputPaperRect );

                // draw paper
                pWin->DrawPolyPolygon( aPolyPoly );

                 // draw shadow
                pWin->SetFillColor( aShadowColor );
                pWin->DrawRect( Rectangle( Point( aPaperRect.Right() + 1, aPaperRect.Top() + PAPER_SHADOW_EXT_PIXEL ),
                                           Size( PAPER_SHADOW_EXT_PIXEL, aPaperRect.GetHeight() ) ) );
                pWin->DrawRect( Rectangle( Point( aPaperRect.Left() + PAPER_SHADOW_EXT_PIXEL, aPaperRect.Bottom() + 1 ),
                                           Size( aPaperRect.GetWidth(), PAPER_SHADOW_EXT_PIXEL ) ) );
            }

            pWin->SetFillColor( aOldFillColor );
            pWin->SetLineColor( aOldLineColor );
            pWin->SetDrawMode( nOldDrawMode );
            pWin->EnableMapMode( bOldMap );
        }
    }

    /* #97517#  This is done before each text edit, so why not do it before every paint.
                The default language is only used if the outliner only contains one
                character in a symbol font */
    pDoc->GetDrawOutliner( NULL ).SetDefaultLanguage( pDoc->GetLanguage( EE_CHAR_LANGUAGE ) );

    // #103834# Set Application Background color for usage in SdrPaintView(s)
    pDrView->SetApplicationBackgroundColor(aFillColor);

    pDrView->InitRedraw( pWin, Region( rRect ) );

    if( pWin )
    {
        if( pDocSh->GetActualFunction() )
            pDocSh->GetActualFunction()->Paint( rRect, pWin );

        if( pFuActual )
            pFuActual->Paint( rRect, pWin );
    }
}

/*************************************************************************
|*
|* Zoom-Faktor fuer InPlace einstellen
|*
\************************************************************************/

void SdDrawViewShell::SetZoomFactor(const Fraction& rZoomX, const Fraction& rZoomY)
{
    SdViewShell::SetZoomFactor(rZoomX, rZoomY);
    bZoomOnPage = FALSE;
    Point aOrigin = pWindow->GetViewOrigin();
    pWindow->SetWinViewPos(aOrigin);
}

/*************************************************************************
|*
|* Optimale Groesse zurueckgeben
|*
\************************************************************************/

Size SdDrawViewShell::GetOptimalSizePixel() const
{
    Size aSize;

    SdrPageView* pPV = pDrView->GetPageViewPvNum(0);

    if (pPV)
    {
        SdPage* pPage = (SdPage*) pPV->GetPage();

        if (pPage)
        {
            if (!bZoomOnPage)
            {
                // Gegenwaertigen MapMode beruecksichtigen
                aSize = pWindow->LogicToPixel( pPage->GetSize() );
            }
            else
            {
                // 1:1 Darstellung
                MapMode aMapMode(MAP_100TH_MM);
                aSize = pWindow->LogicToPixel( pPage->GetSize(), aMapMode );
                ( (SdDrawViewShell*) this)->bZoomOnPage = TRUE;
            }
        }
    }

    return(aSize);
}


/*************************************************************************
|*
|* Seite wird gehided
|*
\************************************************************************/

void SdDrawViewShell::HidePage(SdrPageView* pPV)
{
    FmFormShell* pShell = (FmFormShell*) aShellTable.Get(RID_FORMLAYER_TOOLBOX);

    if (pShell)
        pShell->PrepareClose(FALSE);
}



void SdDrawViewShell::WriteUserDataSequence ( ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
    WriteFrameViewData();

    SdViewShell::WriteUserDataSequence( rSequence, bBrowse );

    const sal_Int32 nIndex = rSequence.getLength();
    rSequence.realloc( nIndex + 1 );
    rSequence[nIndex].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( sUNO_View_ZoomOnPage ) );
    rSequence[nIndex].Value <<= (sal_Bool)bZoomOnPage;
}

void SdDrawViewShell::ReadUserDataSequence ( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
    WriteFrameViewData();

    SdViewShell::ReadUserDataSequence( rSequence, bBrowse );

    const sal_Int32 nLength = rSequence.getLength();
    const com::sun::star::beans::PropertyValue *pValue = rSequence.getConstArray();
    for (sal_Int16 i = 0 ; i < nLength; i++, pValue++ )
    {
        if (pValue->Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( sUNO_View_ZoomOnPage ) ) )
        {
            sal_Bool bZoomPage;
            if( pValue->Value >>= bZoomPage )
            {
                bZoomOnPage = bZoomPage;
            }
        }
    }

    if( pFrameView->GetPageKind() != ePageKind )
    {
        ePageKind = pFrameView->GetPageKind();

        if (ePageKind == PK_NOTES)
        {
            // SdDrawViewShell buttons
            aLayerBtn.Disable();

            // SdViewShell buttons
            aDrawBtn.Check(FALSE);
            aOutlineBtn.Check(FALSE);
            aSlideBtn.Check(FALSE);
            aHandoutBtn.Check(FALSE);
            aPresentationBtn.Check(FALSE);
            aNotesBtn.Check(TRUE);

            SetHelpId( SID_NOTESMODE );
            pWindow->SetHelpId( SID_NOTESMODE );
            pWindow->SetUniqueId( SID_NOTESMODE );
        }
        else if (ePageKind == PK_HANDOUT)
        {
            // SdDrawViewShell buttons
            aPageBtn.Disable();
            aLayerBtn.Disable();

            // SdViewShell buttons
            aDrawBtn.Check(FALSE);
            aOutlineBtn.Check(FALSE);
            aSlideBtn.Check(FALSE);
            aPresentationBtn.Check(FALSE);
            aNotesBtn.Check(FALSE);
            aHandoutBtn.Check(TRUE);

            SetHelpId( SID_HANDOUTMODE );
            pWindow->SetHelpId( SID_HANDOUTMODE );
            pWindow->SetUniqueId( SID_HANDOUTMODE );
        }
        else
        {
            // SdViewShell buttons
            aOutlineBtn.Check(FALSE);
            aSlideBtn.Check(FALSE);
            aPresentationBtn.Check(FALSE);
            aNotesBtn.Check(FALSE);
            aHandoutBtn.Check(FALSE);
            aDrawBtn.Check(TRUE);

            SetHelpId( SD_IF_SDDRAWVIEWSHELL );
            pWindow->SetHelpId( SD_IF_SDDRAWVIEWSHELL );
            pWindow->SetUniqueId( SD_IF_SDDRAWVIEWSHELL );
        }
    }

    ReadFrameViewData( pFrameView );

    if( !bZoomOnPage )
    {
        const Rectangle aVisArea( pFrameView->GetVisArea() );

        if ( pDocSh->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        {
            pDocSh->SetVisArea(aVisArea);
        }

        VisAreaChanged(aVisArea);

        SdView* pView = GetView();

        if (pView)
        {
            pView->VisAreaChanged(pWindow);
        }

        SetZoomRect(aVisArea);
    }

    ChangeEditMode( eEditMode, !bLayerMode );
    ChangeEditMode( eEditMode, !bLayerMode );

}

void SdDrawViewShell::VisAreaChanged(const Rectangle& rRect)
{
    SdViewShell::VisAreaChanged( rRect );

    if( pController )
    {
        pController->fireVisAreaChanged( rRect );
    }
}




/** If there is a valid controller then create a new instance of
    <type>AccessibleDrawDocumentView</type>.  Otherwise delegate this call
    to the base class to return a default object (probably an empty
    reference).
*/
::com::sun::star::uno::Reference<
    ::com::sun::star::accessibility::XAccessible>
    SdDrawViewShell::CreateAccessibleDocumentView (SdWindow* pWindow)
{
    if (GetController() != NULL)
    {
        accessibility::AccessibleDrawDocumentView* pDocumentView =
            new accessibility::AccessibleDrawDocumentView (
                pWindow,
                this,
                GetController(),
                pWindow->GetAccessibleParentWindow()->GetAccessible());
        pDocumentView->Init();
        return ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>
            (static_cast< ::com::sun::star::uno::XWeak*>(pDocumentView),
                ::com::sun::star::uno::UNO_QUERY);
    }
    else
    {
        OSL_TRACE ("SdDrawViewShell::CreateAccessibleDocumentView: no controller");
        return SdViewShell::CreateAccessibleDocumentView (pWindow);
    }
}




int SdDrawViewShell::GetTabLayerCount (void) const
{
    return aLayerTab.GetPageCount();

}




int SdDrawViewShell::GetActiveTabLayerIndex (void) const
{
    return aLayerTab.GetPagePos (aLayerTab.GetCurPageId());
}




void SdDrawViewShell::SetActiveTabLayerIndex (int nIndex)
{
    // Ignore invalid indices silently.
    if (nIndex>=0 && nIndex<aLayerTab.GetPageCount())
    {
        // Tell the draw view and the tab control of the new active layer.
        pDrView->SetActiveLayer (aLayerTab.GetPageText (aLayerTab.GetPageId (nIndex)));
        aLayerTab.SetCurPageId (aLayerTab.GetPageId (nIndex));
   }
}




SdTabControl* SdDrawViewShell::GetPageTabControl (void)
{
    return &aTabControl;
}




SdLayerTab* SdDrawViewShell::GetLayerTabControl (void)
{
    return &aLayerTab;
}
