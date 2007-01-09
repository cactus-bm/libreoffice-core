/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drviews5.cxx,v $
 *
 *  $Revision: 1.49 $
 *
 *  last change: $Author: vg $ $Date: 2007-01-09 11:36:26 $
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
#include "precompiled_sd.hxx"

#include "DrawViewShell.hxx"
#ifndef SD_PRESENTATION_VIEW_SHELL_HXX
#include "PresentationViewShell.hxx"
#endif

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
#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _SVX_FMSHELL_HXX //autogen
#include <svx/fmshell.hxx>
#endif
#include <svx/eeitem.hxx>
#ifndef INCLUDED_SVTOOLS_COLORCFG_HXX
#include <svtools/colorcfg.hxx>
#endif
#ifndef _SD_ACCESSIBILITY_ACCESSIBLE_DRAW_DOCUMENT_VIEW_HXX
#include "AccessibleDrawDocumentView.hxx"
#endif

#include <sfx2/viewfrm.hxx>
#include "LayerTabBar.hxx"

#include "strings.hrc"
#include "res_bmp.hrc"
#include "glob.hrc"
#include "app.hrc"

#include "optsitem.hxx"
#include "app.hxx"
#ifndef SD_FRAME_VIEW_HXX
#include "FrameView.hxx"
#endif
#include "sdattr.hxx"
#ifndef SD_FU_TEXT_HXX
#include "futext.hxx"
#endif
#include "sdpage.hxx"
#include "stlpool.hxx"
#include "prntopts.hxx"
#include "sdresid.hxx"
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#ifndef SD_SHOW_WINDOW_HXX
#include "ShowWindow.hxx"
#endif
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#ifndef SD_OUTLINER_HXX
#include "Outliner.hxx"
#endif
#ifndef SD_CLIENT_HXX
#include "Client.hxx"
#endif
#ifndef _SD_SLIDESHOW_HXX
#include "slideshow.hxx"
#endif
#include "unokywds.hxx"
#ifndef SD_UNO_DRAW_VIEW_HXX
#include "SdUnoDrawView.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_FORM_SHELL_MANAGER_HXX
#include "FormShellManager.hxx"
#endif
#include "LayerDialogContent.hxx"
#include "DrawController.hxx"

namespace sd {

static const int TABCONTROL_INITIAL_SIZE = 350;
static const int PAPER_SHADOW_EXT_PIXEL = 2;


/*************************************************************************
|*
|* Wird gerufen, wenn sich das Model aendert
|*
\************************************************************************/

void DrawViewShell::ModelHasChanged()
{
    Invalidate();
    // Damit der Navigator auch einen aktuellen Status bekommt
    GetViewFrame()->GetBindings().Invalidate( SID_NAVIGATOR_STATE, TRUE, FALSE );

    //Update3DWindow();
    SfxBoolItem aItem( SID_3D_STATE, TRUE );
    GetViewFrame()->GetDispatcher()->Execute(
        SID_3D_STATE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

    // jetzt den von der Drawing Engine neu erzeugten TextEditOutliner
    // initialisieren
    ::Outliner* pOutliner     = mpDrawView->GetTextEditOutliner();
    if (pOutliner)
    {
        SfxStyleSheetPool* pSPool = (SfxStyleSheetPool*) GetDocSh()->GetStyleSheetPool();
        pOutliner->SetStyleSheetPool(pSPool);
    }
}

void DrawViewShell::Resize (const Point &rPos, const Size &rSize)
{
    if (!mpSlideShow || !mpSlideShow->isFullScreen())
    {
        ViewShell::Resize(rPos, rSize);

        if ( GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        {
            SetZoomRect( GetDocSh()->GetVisArea(ASPECT_CONTENT) );
        }
    }

    if(mpSlideShow != NULL)
        mpSlideShow->resize( rSize );
}

void DrawViewShell::ArrangeGUIElements (void)
{
    // Retrieve the current size (thickness) of the scroll bars.  That is
    // the width of the vertical and the height of the horizontal scroll
    // bar.
    int nScrollBarSize =
        GetParentWindow()->GetSettings().GetStyleSettings().GetScrollBarSize();
    maScrBarWH = Size (nScrollBarSize, nScrollBarSize);

    Point aHPos = maViewPos;
    aHPos.Y() += maViewSize.Height();


    ViewShell::ArrangeGUIElements ();

    maTabControl.Hide();

    OSL_ASSERT (GetViewShell()!=NULL);
    Client* pIPClient = static_cast<Client*>(GetViewShell()->GetIPClient());
    BOOL bClientActive = FALSE;
    if ( pIPClient && pIPClient->IsObjectInPlaceActive() )
        bClientActive = TRUE;

    BOOL bInPlaceActive = GetViewFrame()->GetFrame()->IsInPlace();

    if ( mbZoomOnPage && !bInPlaceActive && !bClientActive )
    {
        // bei Split immer erstes Fenster resizen
        //af pWindow = mpContentWindow.get();
        SfxRequest aReq(SID_SIZE_PAGE, 0, GetDoc()->GetItemPool());
        ExecuteSlot( aReq );
    }
}

/*************************************************************************
|*
|* Daten der FrameView auf die aktuelle View uebertragen
|*
\************************************************************************/

void DrawViewShell::ReadFrameViewData(FrameView* pView)
{
    ModifyGuard aGuard( GetDoc() );

    // Diese Option wird am Model eingestellt
    GetDoc()->SetPickThroughTransparentTextFrames(
             SD_MOD()->GetSdOptions(GetDoc()->GetDocumentType())->IsPickThrough());

    // Initialisierungen der Zeichen-(Bildschirm-)Attribute
    if (HasRuler() != pView->HasRuler())
        SetRuler( pView->HasRuler() );

    if (mpDrawView->GetGridCoarse() != pView->GetGridCoarse())
        mpDrawView->SetGridCoarse( pView->GetGridCoarse() );

    if (mpDrawView->GetGridFine() != pView->GetGridFine())
        mpDrawView->SetGridFine( pView->GetGridFine() );

    if (mpDrawView->GetSnapGridWidthX() != pView->GetSnapGridWidthX() || mpDrawView->GetSnapGridWidthY() != pView->GetSnapGridWidthY())
        mpDrawView->SetSnapGridWidth(pView->GetSnapGridWidthX(), pView->GetSnapGridWidthY());

    if (mpDrawView->IsGridVisible() != pView->IsGridVisible())
        mpDrawView->SetGridVisible( pView->IsGridVisible() );

    if (mpDrawView->IsGridFront() != pView->IsGridFront())
        mpDrawView->SetGridFront( pView->IsGridFront() );

    if (mpDrawView->GetSnapAngle() != pView->GetSnapAngle())
        mpDrawView->SetSnapAngle( pView->GetSnapAngle() );

    if (mpDrawView->IsGridSnap() !=  pView->IsGridSnap() )
        mpDrawView->SetGridSnap( pView->IsGridSnap() );

    if (mpDrawView->IsBordSnap() !=  pView->IsBordSnap() )
        mpDrawView->SetBordSnap( pView->IsBordSnap() );

    if (mpDrawView->IsHlplSnap() !=  pView->IsHlplSnap() )
        mpDrawView->SetHlplSnap( pView->IsHlplSnap() );

    if (mpDrawView->IsOFrmSnap() !=  pView->IsOFrmSnap() )
        mpDrawView->SetOFrmSnap( pView->IsOFrmSnap() );

    if (mpDrawView->IsOPntSnap() !=  pView->IsOPntSnap() )
        mpDrawView->SetOPntSnap( pView->IsOPntSnap() );

    if (mpDrawView->IsOConSnap() !=  pView->IsOConSnap() )
        mpDrawView->SetOConSnap( pView->IsOConSnap() );

    if (mpDrawView->IsHlplVisible() != pView->IsHlplVisible() )
        mpDrawView->SetHlplVisible( pView->IsHlplVisible() );

    if (mpDrawView->IsDragStripes() != pView->IsDragStripes() )
        mpDrawView->SetDragStripes( pView->IsDragStripes() );

    if (mpDrawView->IsPlusHandlesAlwaysVisible() != pView->IsPlusHandlesAlwaysVisible() )
        mpDrawView->SetPlusHandlesAlwaysVisible( pView->IsPlusHandlesAlwaysVisible() );

    if (mpDrawView->GetSnapMagneticPixel() != pView->GetSnapMagneticPixel() )
        mpDrawView->SetSnapMagneticPixel( pView->GetSnapMagneticPixel() );

    if (mpDrawView->IsMarkedHitMovesAlways() != pView->IsMarkedHitMovesAlways() )
        mpDrawView->SetMarkedHitMovesAlways( pView->IsMarkedHitMovesAlways() );

    if (mpDrawView->IsMoveOnlyDragging() != pView->IsMoveOnlyDragging() )
        mpDrawView->SetMoveOnlyDragging( pView->IsMoveOnlyDragging() );

//    mpDrawView->SetSlantButShear( pView->IsSlantButShear() );

    if (mpDrawView->IsNoDragXorPolys() != pView->IsNoDragXorPolys() )
        mpDrawView->SetNoDragXorPolys( pView->IsNoDragXorPolys() );

    if (mpDrawView->IsCrookNoContortion() != pView->IsCrookNoContortion() )
        mpDrawView->SetCrookNoContortion( pView->IsCrookNoContortion() );

    if (mpDrawView->IsAngleSnapEnabled() != pView->IsAngleSnapEnabled() )
        mpDrawView->SetAngleSnapEnabled( pView->IsAngleSnapEnabled() );

    if (mpDrawView->IsBigOrtho() != pView->IsBigOrtho() )
        mpDrawView->SetBigOrtho( pView->IsBigOrtho() );

    if (mpDrawView->IsOrtho() != pView->IsOrtho() )
        mpDrawView->SetOrtho( pView->IsOrtho() );

    if (mpDrawView->GetEliminatePolyPointLimitAngle() != pView->GetEliminatePolyPointLimitAngle() )
        mpDrawView->SetEliminatePolyPointLimitAngle( pView->GetEliminatePolyPointLimitAngle() );

    if (mpDrawView->IsEliminatePolyPoints() != pView->IsEliminatePolyPoints() )
        mpDrawView->SetEliminatePolyPoints( pView->IsEliminatePolyPoints() );

    if (mpDrawView->IsLineDraft() != pView->IsLineDraft() )
        mpDrawView->SetLineDraft( pView->IsLineDraft() );

    if (mpDrawView->IsFillDraft() != pView->IsFillDraft() )
    {
// #110094#-7
//      mpDrawView->ReleaseMasterPagePaintCache();
        mpDrawView->SetFillDraft( pView->IsFillDraft() );
    }

    if (mpDrawView->IsTextDraft() != pView->IsTextDraft() )
        mpDrawView->SetTextDraft( pView->IsTextDraft() );

    if (mpDrawView->IsGrafDraft() != pView->IsGrafDraft() )
        mpDrawView->SetGrafDraft( pView->IsGrafDraft() );

    if (mpDrawView->IsSolidMarkHdl() != pView->IsSolidMarkHdl() )
        mpDrawView->SetSolidMarkHdl( pView->IsSolidMarkHdl() );

    if (mpDrawView->IsSolidDragging() != pView->IsSolidDragging() )
        mpDrawView->SetSolidDragging( pView->IsSolidDragging() );

    if (mpDrawView->IsQuickTextEditMode() != pView->IsQuickEdit())
        mpDrawView->SetQuickTextEditMode( pView->IsQuickEdit() );

    // #i26631#
    if (mpDrawView->IsMasterPagePaintCaching() != pView->IsMasterPagePaintCaching())
        mpDrawView->SetMasterPagePaintCaching( pView->IsMasterPagePaintCaching() );

    // Definition:
    //  grosse Handles: 9
    //  kleine Handles: 7
    USHORT nTmp = mpDrawView->GetMarkHdlSizePixel();
    //DBG_ASSERT(nTmp != 7, "HandleSize != 7 oder 9");
    if( nTmp == 9 && !pView->IsBigHandles() )
        mpDrawView->SetMarkHdlSizePixel( 7 );
    else if( nTmp == 7 && pView->IsBigHandles() )
        mpDrawView->SetMarkHdlSizePixel( 9 );


    SdrPageView* pPageView = mpDrawView->GetSdrPageView();
    if (pPageView)
    {
        if ( pPageView->GetVisibleLayers() != pView->GetVisibleLayers() )
            pPageView->SetVisibleLayers( pView->GetVisibleLayers() );

        if ( pPageView->GetPrintableLayers() != pView->GetPrintableLayers() )
            pPageView->SetPrintableLayers( pView->GetPrintableLayers() );

        if ( pPageView->GetLockedLayers() != pView->GetLockedLayers() )
            pPageView->SetLockedLayers( pView->GetLockedLayers() );

        if (mePageKind == PK_NOTES)
        {
            if (pPageView->GetHelpLines() != pView->GetNotesHelpLines())
                pPageView->SetHelpLines( pView->GetNotesHelpLines() );
        }
        else if (mePageKind == PK_HANDOUT)
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

    if ( mpDrawView->GetActiveLayer() != pView->GetActiveLayer() )
        mpDrawView->SetActiveLayer( pView->GetActiveLayer() );

    USHORT nSelectedPage = 0;

    if (mePageKind != PK_HANDOUT)
    {
        nSelectedPage = pView->GetSelectedPage();
    }

    EditMode eNewEditMode = pView->GetViewShEditMode(mePageKind);
    BOOL bNewLayerMode = pView->IsLayerMode();
    ChangeEditMode(eNewEditMode, bNewLayerMode);
    SwitchPage(nSelectedPage);

    // DrawMode fuer 'Normales' Fenster wiederherstellen
    if(GetActiveWindow()->GetDrawMode() != pView->GetDrawMode())
      GetActiveWindow()->SetDrawMode(pView->GetDrawMode());

    if ( mpDrawView->IsDesignMode() != pView->IsDesignMode() )
    {
        SfxBoolItem aDesignModeItem( SID_FM_DESIGN_MODE, pView->IsDesignMode() );
        GetViewFrame()->GetDispatcher()->Execute( SID_FM_DESIGN_MODE, SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_RECORD, &aDesignModeItem, 0L );
    }

    // Muss am Ende gerufen werden, da ein WriteFrameViewData() ausgeloest wird
    if (mpDrawView->IsFrameDragSingles() != pView->IsFrameDragSingles() )
        mpDrawView->SetFrameDragSingles( pView->IsFrameDragSingles() );
}

/*************************************************************************
|*
|* Daten der aktuellen View auf die FrameView uebertragen
|*
\************************************************************************/

void DrawViewShell::WriteFrameViewData()
{
    // Zeichen-(Bildschirm-)Attribute an FrameView merken
    mpFrameView->SetRuler( HasRuler() );
    mpFrameView->SetGridCoarse( mpDrawView->GetGridCoarse() );
    mpFrameView->SetGridFine( mpDrawView->GetGridFine() );
    mpFrameView->SetSnapGridWidth(mpDrawView->GetSnapGridWidthX(), mpDrawView->GetSnapGridWidthY());
    mpFrameView->SetGridVisible( mpDrawView->IsGridVisible() );
    mpFrameView->SetGridFront( mpDrawView->IsGridFront() );
    mpFrameView->SetSnapAngle( mpDrawView->GetSnapAngle() );
    mpFrameView->SetGridSnap( mpDrawView->IsGridSnap() );
    mpFrameView->SetBordSnap( mpDrawView->IsBordSnap() );
    mpFrameView->SetHlplSnap( mpDrawView->IsHlplSnap() );
    mpFrameView->SetOFrmSnap( mpDrawView->IsOFrmSnap() );
    mpFrameView->SetOPntSnap( mpDrawView->IsOPntSnap() );
    mpFrameView->SetOConSnap( mpDrawView->IsOConSnap() );
    mpFrameView->SetHlplVisible( mpDrawView->IsHlplVisible() );
    mpFrameView->SetDragStripes( mpDrawView->IsDragStripes() );
    mpFrameView->SetPlusHandlesAlwaysVisible( mpDrawView->IsPlusHandlesAlwaysVisible() );
    mpFrameView->SetFrameDragSingles( mpDrawView->IsFrameDragSingles() );
    mpFrameView->SetMarkedHitMovesAlways( mpDrawView->IsMarkedHitMovesAlways() );
    mpFrameView->SetMoveOnlyDragging( mpDrawView->IsMoveOnlyDragging() );
    mpFrameView->SetNoDragXorPolys( mpDrawView->IsNoDragXorPolys() );
    mpFrameView->SetCrookNoContortion( mpDrawView->IsCrookNoContortion() );
    mpFrameView->SetBigOrtho( mpDrawView->IsBigOrtho() );
    mpFrameView->SetEliminatePolyPointLimitAngle( mpDrawView->GetEliminatePolyPointLimitAngle() );
    mpFrameView->SetEliminatePolyPoints( mpDrawView->IsEliminatePolyPoints() );

    mpFrameView->SetLineDraft( mpDrawView->IsLineDraft() );
    mpFrameView->SetFillDraft( mpDrawView->IsFillDraft() );
    mpFrameView->SetTextDraft( mpDrawView->IsTextDraft() );
    mpFrameView->SetGrafDraft( mpDrawView->IsGrafDraft() );
    mpFrameView->SetSolidMarkHdl( mpDrawView->IsSolidMarkHdl() );
    mpFrameView->SetSolidDragging( mpDrawView->IsSolidDragging() );
    mpFrameView->SetQuickEdit( mpDrawView->IsQuickTextEditMode() );

    mpFrameView->SetDesignMode( mpDrawView->IsDesignMode() );

    Size aVisSizePixel = GetActiveWindow()->GetOutputSizePixel();
    Rectangle aVisArea = GetActiveWindow()->PixelToLogic( Rectangle( Point(0,0), aVisSizePixel) );
    mpFrameView->SetVisArea(aVisArea);

    if( mePageKind == PK_HANDOUT )
        mpFrameView->SetSelectedPage(0);
    else
    {
        mpFrameView->SetSelectedPage( maTabControl.GetCurPageId() - 1 );
    }

    mpFrameView->SetViewShEditMode(meEditMode, mePageKind);
    mpFrameView->SetLayerMode(IsLayerModeActive());

    SdrPageView* pPageView = mpDrawView->GetSdrPageView();

    if (pPageView)
    {
        if ( mpFrameView->GetVisibleLayers() != pPageView->GetVisibleLayers() )
            mpFrameView->SetVisibleLayers( pPageView->GetVisibleLayers() );

        if ( mpFrameView->GetPrintableLayers() != pPageView->GetPrintableLayers() )
            mpFrameView->SetPrintableLayers( pPageView->GetPrintableLayers() );

        if ( mpFrameView->GetLockedLayers() != pPageView->GetLockedLayers() )
            mpFrameView->SetLockedLayers( pPageView->GetLockedLayers() );

        if (mePageKind == PK_NOTES)
        {
            mpFrameView->SetNotesHelpLines( pPageView->GetHelpLines() );
        }
        else if (mePageKind == PK_HANDOUT)
        {
            mpFrameView->SetHandoutHelpLines( pPageView->GetHelpLines() );
        }
        else
        {
            mpFrameView->SetStandardHelpLines( pPageView->GetHelpLines() );
        }
    }

    if ( mpFrameView->GetActiveLayer() != mpDrawView->GetActiveLayer() )
        mpFrameView->SetActiveLayer( mpDrawView->GetActiveLayer() );

    // DrawMode fuer 'Normales' Fenster merken
    if(mpFrameView->GetDrawMode() != GetActiveWindow()->GetDrawMode())
      mpFrameView->SetDrawMode(GetActiveWindow()->GetDrawMode());
}



/*************************************************************************
|*
|* Paint-Methode: das Ereignis wird vom Fenster pWin an
|* die Viewshell und die aktuelle Funktion weitergeleitet
|*
|* Anmerkung: pWin==NULL, wenn Paint() vom ShowWindow gerufen wird!
|*
\************************************************************************/

void DrawViewShell::Paint(const Rectangle& rRect, ::sd::Window* pWin)
{
    // #103834# Fill var FillColor here to have it available on later call
    svtools::ColorConfig aColorConfig;
    Color aFillColor;

    if(DOCUMENT_TYPE_IMPRESS == GetDoc()->GetDocumentType())
    {
        aFillColor = Color( aColorConfig.GetColorValue( svtools::APPBACKGROUND ).nColor );
    }
    else
    {
        aFillColor = Color( aColorConfig.GetColorValue( svtools::DOCCOLOR ).nColor );
    }

    /* #97517#  This is done before each text edit, so why not do it before every paint.
                The default language is only used if the outliner only contains one
                character in a symbol font */
    GetDoc()->GetDrawOutliner( NULL ).SetDefaultLanguage( GetDoc()->GetLanguage( EE_CHAR_LANGUAGE ) );

    // #103834# Set Application Background color for usage in SdrPaintView(s)
    mpDrawView->SetApplicationBackgroundColor(aFillColor);

    /* #97517#  This is done before each text edit, so why not do it before every paint.
                The default language is only used if the outliner only contains one
                character in a symbol font */
    GetDoc()->GetDrawOutliner( NULL ).SetDefaultLanguage( Application::GetSettings().GetLanguage() );

    mpDrawView->CompleteRedraw( pWin, Region( rRect ) );

    if( pWin )
    {
        if( GetDocSh()->GetDocShellFunction().is() )
            GetDocSh()->GetDocShellFunction()->Paint( rRect, pWin );

        if( HasCurrentFunction() )
            GetCurrentFunction()->Paint( rRect, pWin );
    }
}

/*************************************************************************
|*
|* Zoom-Faktor fuer InPlace einstellen
|*
\************************************************************************/

void DrawViewShell::SetZoomFactor(const Fraction& rZoomX, const Fraction& rZoomY)
{
    ViewShell::SetZoomFactor(rZoomX, rZoomY);
    mbZoomOnPage = FALSE;
    Point aOrigin = GetActiveWindow()->GetViewOrigin();
    GetActiveWindow()->SetWinViewPos(aOrigin);
}

/*************************************************************************
|*
|* Optimale Groesse zurueckgeben
|*
\************************************************************************/

Size DrawViewShell::GetOptimalSizePixel() const
{
    Size aSize;

    SdrPageView* pPV = mpDrawView->GetSdrPageView();
    if (pPV)
    {
        SdPage* pPage = (SdPage*) pPV->GetPage();

        if (pPage)
        {
            if (!mbZoomOnPage)
            {
                // Gegenwaertigen MapMode beruecksichtigen
                aSize = GetActiveWindow()->LogicToPixel( pPage->GetSize() );
            }
            else
            {
                // 1:1 Darstellung
                MapMode aMapMode(MAP_100TH_MM);
                aSize = GetActiveWindow()->LogicToPixel( pPage->GetSize(), aMapMode );
                const_cast< DrawViewShell* >(this)->mbZoomOnPage = TRUE;
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

void DrawViewShell::HidePage()
{
    FmFormShell* pFormShell = GetViewShellBase().GetFormShellManager().GetFormShell();
    if (pFormShell != NULL)
        pFormShell->PrepareClose (FALSE);
}



void DrawViewShell::WriteUserDataSequence ( ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
    WriteFrameViewData();

    ViewShell::WriteUserDataSequence( rSequence, bBrowse );

    const sal_Int32 nIndex = rSequence.getLength();
    rSequence.realloc( nIndex + 1 );
    rSequence[nIndex].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( sUNO_View_ZoomOnPage ) );
    rSequence[nIndex].Value <<= (sal_Bool)mbZoomOnPage;
}

void DrawViewShell::ReadUserDataSequence ( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
    WriteFrameViewData();

    ViewShell::ReadUserDataSequence( rSequence, bBrowse );

    const sal_Int32 nLength = rSequence.getLength();
    const com::sun::star::beans::PropertyValue *pValue = rSequence.getConstArray();
    for (sal_Int16 i = 0 ; i < nLength; i++, pValue++ )
    {
        if (pValue->Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( sUNO_View_ZoomOnPage ) ) )
        {
            sal_Bool bZoomPage = sal_False;
            if( pValue->Value >>= bZoomPage )
            {
                mbZoomOnPage = bZoomPage;
            }
        }
    }

    if( mpFrameView->GetPageKind() != mePageKind )
    {
        mePageKind = mpFrameView->GetPageKind();

        if (mePageKind == PK_NOTES)
        {
            SetHelpId( SID_NOTESMODE );
            GetActiveWindow()->SetHelpId( SID_NOTESMODE );
            GetActiveWindow()->SetUniqueId( SID_NOTESMODE );
        }
        else if (mePageKind == PK_HANDOUT)
        {
            SetHelpId( SID_HANDOUTMODE );
            GetActiveWindow()->SetHelpId( SID_HANDOUTMODE );
            GetActiveWindow()->SetUniqueId( SID_HANDOUTMODE );
        }
        else
        {
            SetHelpId( SD_IF_SDDRAWVIEWSHELL );
            GetActiveWindow()->SetHelpId( SD_IF_SDDRAWVIEWSHELL );
            GetActiveWindow()->SetUniqueId( SD_IF_SDDRAWVIEWSHELL );
        }
    }

    ReadFrameViewData( mpFrameView );

    if( !mbZoomOnPage )
    {
        const Rectangle aVisArea( mpFrameView->GetVisArea() );

        if ( GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        {
            GetDocSh()->SetVisArea(aVisArea);
        }

        VisAreaChanged(aVisArea);

        ::sd::View* pView = GetView();

        if (pView)
        {
            pView->VisAreaChanged(GetActiveWindow());
        }

        SetZoomRect(aVisArea);
    }

    ChangeEditMode (meEditMode, ! IsLayerModeActive());
    ChangeEditMode (meEditMode, ! IsLayerModeActive());
}

void DrawViewShell::VisAreaChanged(const Rectangle& rRect)
{
    ViewShell::VisAreaChanged( rRect );

    DrawController& rController = GetViewShellBase().GetDrawController();
    rController.FireVisAreaChanged (rRect);
}




/** If there is a valid controller then create a new instance of
    <type>AccessibleDrawDocumentView</type>.  Otherwise delegate this call
    to the base class to return a default object (probably an empty
    reference).
*/
::com::sun::star::uno::Reference<
    ::com::sun::star::accessibility::XAccessible>
    DrawViewShell::CreateAccessibleDocumentView (::sd::Window* pWindow)
{
    if (GetViewShellBase().GetController() != NULL)
    {
        accessibility::AccessibleDrawDocumentView* pDocumentView =
            new accessibility::AccessibleDrawDocumentView (
                pWindow,
                this,
                GetViewShellBase().GetController(),
                pWindow->GetAccessibleParentWindow()->GetAccessible());
        pDocumentView->Init();
        return ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>
            (static_cast< ::com::sun::star::uno::XWeak*>(pDocumentView),
                ::com::sun::star::uno::UNO_QUERY);
    }
    else
    {
        OSL_TRACE ("DrawViewShell::CreateAccessibleDocumentView: no controller");
        return ViewShell::CreateAccessibleDocumentView (pWindow);
    }
}




int DrawViewShell::GetActiveTabLayerIndex (void) const
{
    const LayerTabBar* pBar
        = const_cast<DrawViewShell*>(this)->GetLayerTabControl ();
    if (pBar != NULL)
        return pBar->GetPagePos (pBar->GetCurPageId());
    else
        return -1;
}




void DrawViewShell::SetActiveTabLayerIndex (int nIndex)
{
    LayerTabBar* pBar = GetLayerTabControl ();
    if (pBar != NULL)
    {
        // Ignore invalid indices silently.
        if (nIndex>=0 && nIndex<pBar->GetPageCount())
        {
            // Tell the draw view and the tab control of the new active layer.
            mpDrawView->SetActiveLayer (pBar->GetPageText (pBar->GetPageId ((USHORT)nIndex)));
            pBar->SetCurPageId (pBar->GetPageId ((USHORT)nIndex));
        }
    }
}




TabControl* DrawViewShell::GetPageTabControl (void)
{
    return &maTabControl;
}




LayerTabBar* DrawViewShell::GetLayerTabControl (void)
{
    return mpLayerTabBar.get();
}




int DrawViewShell::GetTabLayerCount (void) const
{
    const LayerTabBar* pBar
        = const_cast<DrawViewShell*>(this)->GetLayerTabControl ();
    if (pBar != NULL)
        return pBar->GetPageCount();
    else
        return 0;
}


} // end of namespace sd
