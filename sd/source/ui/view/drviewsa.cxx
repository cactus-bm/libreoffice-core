/*************************************************************************
 *
 *  $RCSfile: drviewsa.cxx,v $
 *
 *  $Revision: 1.23 $
 *
 *  last change: $Author: obo $ $Date: 2004-07-06 12:26:48 $
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

#define ITEMID_SIZE 0

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _SVX_SIZEITEM
#include <svx/sizeitem.hxx>
#endif
#ifndef _SVDLAYER_HXX
#include <svx/svdlayer.hxx>
#endif
#ifndef _SVX_ZOOMITEM
#include <svx/zoomitem.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _SFXPTITEM_HXX //autogen
#include <svtools/ptitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _XPOLY_HXX //autogen
#include <svx/xpoly.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVDOPATH_HXX //autogen
#include <svx/svdopath.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif

#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif

#ifndef _SVX_DIALOGS_HRC
#include <svx/dialogs.hrc>
#endif
#ifndef _SVX_EXTRUSION_BAR_HXX
#include <svx/extrusionbar.hxx>
#endif
#ifndef _SVX_FONTWORK_BAR_HXX
#include <svx/fontworkbar.hxx>
#endif

#pragma hdrstop

#ifndef _SVX_FMSHELL_HXX            // XXX nur temp (dg)
#include <svx/fmshell.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _CLIPLISTENER_HXX
#include <svtools/cliplistener.hxx>
#endif
#ifndef _SVX_FLOAT3D_HXX
#include <svx/float3d.hxx>
#endif

#include "app.hrc"
#include "helpids.h"
#include "strings.hrc"
#include "res_bmp.hrc"
#include "sdpage.hxx"
#ifndef SD_FRAME_VIEW_HXX
#include "FrameView.hxx"
#endif
#include "drawdoc.hxx"
#include "sdresid.hxx"
#include "DrawDocShell.hxx"
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#ifndef SD_FU_POOR_HXX
#include "fupoor.hxx"
#endif
#ifndef SD_FU_SELECTION_HXX
#include "fusel.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_BEZIER_OBJECT_BAR_HXX
#include "BezierObjectBar.hxx"
#endif
#ifndef SD_GLUE_POINTS_OBJECT_BAR_HXX
#include "GluePointsObjectBar.hxx"
#endif
#ifndef SD_TEXT_OBJECT_BAR_HXX
#include "TextObjectBar.hxx"
#endif
#ifndef SD_GRAPHIC_OBJECT_BAR_HXX
#include "GraphicObjectBar.hxx"
#endif
#ifndef SD_IMPRESS_OBJECT_BAR_HXX
#include "ImpressObjectBar.hxx"
#endif
#ifndef SD_DRAW_OBJECT_BAR_HXX
#include "DrawObjectBar.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#ifndef SD_OBJECT_BAR_MANAGER_HXX
#include "ObjectBarManager.hxx"
#endif
#include "SdUnoDrawView.hxx"
#include "FormShellManager.hxx"

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sd {

BOOL DrawViewShell::bPipette = FALSE;

// Use magenta as transparency color instead of the standard gray that is
// imported from vcl/image.hxx
#undef IMAGE_STDBTN_COLOR
#undef IMAGE_STDBTN_COLOR_HC
#define IMAGE_STDBTN_COLOR Color(0xff,0x00,0xff)
#define IMAGE_STDBTN_COLOR_HC Color(0xff,0x00,0xff)

// ------------------------
// - ScannerEventListener -
// ------------------------

class ScannerEventListener : public ::cppu::WeakImplHelper1< ::com::sun::star::lang::XEventListener >
{
private:

    DrawViewShell*      mpParent;

public:

                            ScannerEventListener( DrawViewShell* pParent ) : mpParent( pParent )  {};
                            ~ScannerEventListener();

    // XEventListener
    virtual void SAL_CALL   disposing( const ::com::sun::star::lang::EventObject& rEventObject ) throw (::com::sun::star::uno::RuntimeException);

    void                    ParentDestroyed() { mpParent = NULL; }
};

// -----------------------------------------------------------------------------

ScannerEventListener::~ScannerEventListener()
{
}

// -----------------------------------------------------------------------------

void SAL_CALL ScannerEventListener::disposing( const ::com::sun::star::lang::EventObject& rEventObject ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mpParent )
        mpParent->ScannerEvent( rEventObject );
}

/*************************************************************************
|*
|* Standard-Konstruktor
|*
\************************************************************************/

DrawViewShell::DrawViewShell (
    SfxViewFrame* pFrame,
    ViewShellBase& rViewShellBase,
    PageKind ePageKind,
    FrameView* pFrameViewArgument)
    : ViewShell (pFrame, rViewShellBase),
      aTabControl(this, &pFrame->GetWindow()),
      aLayerTab(this, &pFrame->GetWindow()),
      aPageBtn(&pFrame->GetWindow(),
          WB_3DLOOK | WB_RECTSTYLE | WB_SMALLSTYLE | WB_NOPOINTERFOCUS),
      aMasterPageBtn(&pFrame->GetWindow(),
          WB_3DLOOK | WB_RECTSTYLE | WB_SMALLSTYLE | WB_NOPOINTERFOCUS ),
      aLayerBtn(&pFrame->GetWindow(),
          WB_3DLOOK | WB_RECTSTYLE | WB_SMALLSTYLE | WB_NOPOINTERFOCUS ),
      pActualPage(NULL),
      bLayerMode(FALSE),
      pXPolygon (0),
      nPolygonIndex (0),
      bLineError (FALSE),
      bLastWasLineTo (FALSE),
      bLastWasMoveTo (FALSE),
      bLastWasBezierTo (FALSE),
      bMousePosFreezed (FALSE),
      nLastSlot(0),
      bReadOnly(GetDocSh()->IsReadOnly()),
      bInEffectAssignment(FALSE),
      pSlotArray( NULL ),
      pClipEvtLstnr(NULL),
      bPastePossible(FALSE),
      mpFormShellManager(NULL)
{
    if (pFrameViewArgument != NULL)
        pFrameView = pFrameViewArgument;
    else
        pFrameView = new FrameView(GetDoc());
    Construct(GetDocSh(), ePageKind);
}

/*************************************************************************
|*
|* Copy-Konstruktor
|*
\************************************************************************/

DrawViewShell::DrawViewShell (
    SfxViewFrame* pFrame,
    const DrawViewShell& rShell)
    : ViewShell(pFrame, rShell),
      aTabControl(this, &pFrame->GetWindow()),
      aLayerTab(this, &pFrame->GetWindow()),
      aPageBtn(&pFrame->GetWindow(),
          WB_3DLOOK | WB_RECTSTYLE | WB_SMALLSTYLE | WB_NOPOINTERFOCUS ),
      aMasterPageBtn(&pFrame->GetWindow(),
          WB_3DLOOK | WB_RECTSTYLE | WB_SMALLSTYLE | WB_NOPOINTERFOCUS ),
      aLayerBtn(&pFrame->GetWindow(),
          WB_3DLOOK | WB_RECTSTYLE | WB_SMALLSTYLE | WB_NOPOINTERFOCUS ),
      pActualPage(NULL),
      bLayerMode(FALSE),
      pXPolygon (0),
      nPolygonIndex (0),
      bLineError (FALSE),
      bLastWasLineTo (FALSE),
      bLastWasMoveTo (FALSE),
      bLastWasBezierTo (FALSE),
      bMousePosFreezed (FALSE),
      nLastSlot(0),
      bReadOnly(GetDocSh()->IsReadOnly()),
      bInEffectAssignment(FALSE),
      pSlotArray( NULL ),
      pClipEvtLstnr(NULL),
      bPastePossible(FALSE),
      mpFormShellManager(NULL)
{
    pFrameView = new FrameView(GetDoc());
    Construct (GetDocSh(), PK_STANDARD);
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

DrawViewShell::~DrawViewShell()
{
    SfxViewShell* pViewShell = GetViewShell();
    OSL_ASSERT (pViewShell!=NULL);

    mpFormShellManager.reset();

    if( mxScannerListener.is() )
        static_cast< ScannerEventListener* >( mxScannerListener.get() )->ParentDestroyed();

    // #96642# Remove references to items within Svx3DWin
    // (maybe do a listening sometime in Svx3DWin)
    USHORT nId = Svx3DChildWindow::GetChildWindowId();
    SfxChildWindow* pWindow = GetViewFrame() ? GetViewFrame()->GetChildWindow(nId) : NULL;
    if(pWindow)
    {
        Svx3DWin* p3DWin = static_cast< Svx3DWin* > (pWindow->GetWindow());
        if(p3DWin)
            p3DWin->DocumentReload();
    }

    EndListening (*GetDocSh());

    if (pFuSlideShow)
    {
        // SlideShow ggf. beenden
        GetViewFrame()->GetDispatcher()->Execute(SID_PRESENTATION_END,
                    SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_RECORD );
    }

    if (pFuActual)
    {
        if (pFuOld == pFuActual)
            pFuOld = NULL;

        pFuActual->Deactivate();
        FuPoor* pFunction = pFuActual;
        pFuActual = NULL;
        delete pFunction;
    }

    if (pFuOld)
    {
        delete pFuOld;
        pFuOld = NULL;
    }

    SdPage* pPage;
    USHORT nSelectedPage = 0;
    USHORT aPageCnt = GetDoc()->GetSdPageCount(ePageKind);

    for (USHORT i = 0; i < aPageCnt; i++)
    {
        pPage = GetDoc()->GetSdPage(i, ePageKind);

        if (pPage == pActualPage)
        {
            GetDoc()->SetSelected(pPage, TRUE);
        }
        else
        {
            GetDoc()->SetSelected(pPage, FALSE);
        }
    }

    /*af
    pViewShell->RemoveSubShell();

    // Umschaltung der ObjectBarShells unterdruecken, sonst versucht die
    // sterbende Funktion eventuell die ObjectBars zu wechseln. Die
    // entsprechende Shell ist aber schon vom SFX vom Dispatcher-Stack
    // genommen worden.
    bObjectBarSwitchEnabled = FALSE;
    */

    if ( pClipEvtLstnr )
    {
        pClipEvtLstnr->AddRemoveListener( GetActiveWindow(), FALSE );
        pClipEvtLstnr->ClearCallbackLink();     // #103849# prevent callback if another thread is waiting
        pClipEvtLstnr->release();
    }

    delete pDrView;
    pViewShell->SetWindow(NULL);

    pFrameView->Disconnect();
    delete pXPolygon;
    delete [] pSlotArray;
}

/*************************************************************************
|*
|* gemeinsamer Initialisierungsanteil der beiden Konstruktoren
|*
\************************************************************************/

void DrawViewShell::Construct(DrawDocShell* pDocSh, PageKind eInitialPageKind)
{
    pFrameView->Connect();

    SfxViewShell* pViewShell = GetViewShell();
    OSL_ASSERT (pViewShell!=NULL);

    // Array fuer Slot-/ImageMapping:
    // Gerader Eintrag: Haupt-/ToolboxSlot
    // Ungerader Eintrag: gemappter Slot
    // Achtung: Anpassen von GetIdBySubId() !!!
    // Reihenfolge (insbesondere Zoom) darf nicht geaendert werden !!!
    pSlotArray = new USHORT[ SLOTARRAY_COUNT ];
    pSlotArray[ 0 ]  = SID_OBJECT_CHOOSE_MODE;
    pSlotArray[ 1 ]  = SID_OBJECT_ROTATE;
    pSlotArray[ 2 ]  = SID_OBJECT_ALIGN;
    pSlotArray[ 3 ]  = SID_OBJECT_ALIGN_LEFT;
    pSlotArray[ 4 ]  = SID_ZOOM_TOOLBOX;
    pSlotArray[ 5 ]  = SID_ZOOM_TOOLBOX;
    pSlotArray[ 6 ]  = SID_DRAWTBX_TEXT;
    pSlotArray[ 7 ]  = SID_ATTR_CHAR;
    pSlotArray[ 8 ]  = SID_DRAWTBX_RECTANGLES;
    pSlotArray[ 9 ]  = SID_DRAW_RECT;
    pSlotArray[ 10 ] = SID_DRAWTBX_ELLIPSES;
    pSlotArray[ 11 ] = SID_DRAW_ELLIPSE;
    pSlotArray[ 12 ] = SID_DRAWTBX_LINES;
    pSlotArray[ 13 ] = SID_DRAW_FREELINE_NOFILL;
    pSlotArray[ 14 ] = SID_DRAWTBX_3D_OBJECTS;
    pSlotArray[ 15 ] = SID_3D_CUBE;
    pSlotArray[ 16 ] = SID_DRAWTBX_INSERT;
    pSlotArray[ 17 ] = SID_INSERT_DIAGRAM;
    pSlotArray[ 18 ] = SID_POSITION;
    pSlotArray[ 19 ] = SID_FRAME_TO_TOP;
    pSlotArray[ 20 ] = SID_DRAWTBX_CONNECTORS;
    pSlotArray[ 21 ] = SID_TOOL_CONNECTOR;
    pSlotArray[ 22 ] = SID_DRAWTBX_ARROWS;
    pSlotArray[ 23 ] = SID_LINE_ARROW_END;

    SetPool( &GetDoc()->GetPool() );

    GetDoc()->CreateFirstPages();

    pDrView = new DrawView(pDocSh, pWindow, this);
    pView = pDrView;             // Pointer der Basisklasse ViewShell
    pDrView->SetSwapAsynchron(TRUE); // Asynchrones Laden von Graphiken

    ePageKind = eInitialPageKind; //af pFrameView->GetPageKind();
    eEditMode = EM_PAGE;
    DocumentType eDocType = GetDoc()->GetDocumentType(); // RTTI fasst hier noch nicht
    switch (ePageKind)
    {
        case PK_STANDARD:
            meShellType = ST_IMPRESS;
            break;

        case PK_NOTES:
            meShellType = ST_NOTES;
            break;

        case PK_HANDOUT:
            meShellType = ST_HANDOUT;
            break;
    }

    // Create the object bars and register them at the sub shell
    // manager.
    ObjectBarManager& rObjectBarManager (GetObjectBarManager());
    // The id for the main object bars is the same.
    if (eDocType == DOCUMENT_TYPE_DRAW)
        rObjectBarManager.RegisterObjectBar (
            RID_DRAW_OBJ_TOOLBOX,
            ::std::auto_ptr<SfxShell>(new DrawObjectBar(this, pDrView)));
    else
        rObjectBarManager.RegisterObjectBar (
            RID_DRAW_OBJ_TOOLBOX,
            ::std::auto_ptr<SfxShell>(new ImpressObjectBar(this,pDrView)));

    rObjectBarManager.RegisterObjectBar (
        RID_BEZIER_TOOLBOX,
        ::std::auto_ptr<SfxShell>(new BezierObjectBar(this, pDrView)));

    rObjectBarManager.RegisterObjectBar (
        RID_GLUEPOINTS_TOOLBOX,
        ::std::auto_ptr<SfxShell>(new GluePointsObjectBar(this, pDrView)));

    rObjectBarManager.RegisterObjectBar (
        RID_DRAW_TEXT_TOOLBOX,
        ::std::auto_ptr<SfxShell>(new TextObjectBar(
            this, GetDoc()->GetPool(), pDrView)));

    ::std::auto_ptr<SfxShell> pFormShell(new FmFormShell(pViewShell, pDrView));
    rObjectBarManager.RegisterObjectBar (RID_FORMLAYER_TOOLBOX, pFormShell);

    rObjectBarManager.RegisterObjectBar (
        RID_DRAW_GRAF_TOOLBOX,
        ::std::auto_ptr<SfxShell>(new GraphicObjectBar(this, pDrView)));

    rObjectBarManager.RegisterObjectBar (
        RID_SVX_EXTRUSION_BAR,
        ::std::auto_ptr<SfxShell>(new svx::ExtrusionBar( pViewShell )));

    rObjectBarManager.RegisterObjectBar (
        RID_SVX_FONTWORK_BAR,
        ::std::auto_ptr<SfxShell>(new svx::FontworkBar( pViewShell )));

    // Activate the relevant object bars.
    rObjectBarManager.PushObjectBar (RID_FORMLAYER_TOOLBOX);
    rObjectBarManager.PushObjectBar (RID_SVX_EXTRUSION_BAR);
    rObjectBarManager.PushObjectBar (RID_DRAW_OBJ_TOOLBOX);

    Size aPageSize = GetDoc()->GetSdPage(0, ePageKind)->GetSize();
    Point aPageOrg = Point(aPageSize.Width(), aPageSize.Height() / 2);
    Size aViewSize = Size(aPageSize.Width() * 3, aPageSize.Height() * 2);
    InitWindows(aPageOrg, aViewSize, Point(-1, -1));

    Point aVisAreaPos;

    if ( pDocSh->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
    {
        aVisAreaPos = pDocSh->GetVisArea(ASPECT_CONTENT).TopLeft();
    }

    pDrView->SetWorkArea(Rectangle(Point() - aVisAreaPos - aPageOrg, aViewSize));

    // Objekte koennen max. so gross wie die ViewSize werden
    GetDoc()->SetMaxObjSize(aViewSize);

    // Split-Handler fuer TabControls
    aTabControl.SetSplitHdl( LINK( this, DrawViewShell, TabSplitHdl ) );
    aLayerTab.SetSplitHdl( LINK( this, DrawViewShell, TabSplitHdl ) );

    aPageBtn.SetModeImage( Image( Bitmap( SdResId( BMP_TAB_PAGEMODE ) ), IMAGE_STDBTN_COLOR ) );
    aPageBtn.SetModeImage( Image( Bitmap( SdResId( BMP_TAB_PAGEMODE_H ) ), IMAGE_STDBTN_COLOR_HC ), BMP_COLOR_HIGHCONTRAST );
    aPageBtn.SetClickHdl(LINK(this, DrawViewShell, TabModeBtnHdl));
    aPageBtn.SetQuickHelpText( String( SdResId( STR_PAGEMODE ) ) );
    aPageBtn.SetHelpId( HID_SD_BTN_PAGE );
    aPageBtn.SetAccessibleName (String(SdResId(STR_PAGEMODE)));
    aPageBtn.Show();

    aMasterPageBtn.SetModeImage( Image( Bitmap( SdResId( BMP_TAB_MASTERPAGE ) ), IMAGE_STDBTN_COLOR ) );
    aMasterPageBtn.SetModeImage( Image( Bitmap( SdResId( BMP_TAB_MASTERPAGE_H ) ), IMAGE_STDBTN_COLOR_HC ), BMP_COLOR_HIGHCONTRAST );
    aMasterPageBtn.SetClickHdl(LINK(this, DrawViewShell, TabModeBtnHdl));
    aMasterPageBtn.SetQuickHelpText( String( SdResId( STR_MASTERPAGEMODE ) ) );
    aMasterPageBtn.SetHelpId( HID_SD_BTN_MASTERPAGE );
    aMasterPageBtn.SetAccessibleName (String(SdResId(STR_MASTERPAGEMODE)));
    aMasterPageBtn.Show();

    aLayerBtn.SetModeImage( Image( Bitmap( SdResId( BMP_TAB_LAYERMODE ) ), IMAGE_STDBTN_COLOR ) );
    aLayerBtn.SetModeImage( Image( Bitmap( SdResId( BMP_TAB_LAYERMODE_H ) ), IMAGE_STDBTN_COLOR_HC ), BMP_COLOR_HIGHCONTRAST );
    aLayerBtn.SetClickHdl(LINK(this, DrawViewShell, TabModeBtnHdl));
    aLayerBtn.SetQuickHelpText( String( SdResId( STR_LAYERMODE ) ) );
    aLayerBtn.SetHelpId( HID_SD_BTN_LAYER );
    aLayerBtn.SetAccessibleName (String(SdResId(STR_LAYERMODE)));
    aLayerBtn.Show();

    // Damit der richtige EditMode von der FrameView komplett eingestellt
    // werden kann, wird hier ein aktuell anderer gewaehlt (kleiner Trick)
    if (pFrameView->GetViewShEditMode(ePageKind) == EM_PAGE)
    {
        eEditMode = EM_MASTERPAGE;
    }
    else
    {
        eEditMode = EM_PAGE;
    }

    // Einstellungen der FrameView uebernehmen
    ReadFrameViewData(pFrameView);

    if( eDocType == DOCUMENT_TYPE_DRAW )
    {
        SetHelpId( SD_IF_SDGRAPHICVIEWSHELL );
        pWindow->SetHelpId( SD_IF_SDGRAPHICVIEWSHELL );
        pWindow->SetUniqueId( SD_IF_SDGRAPHICVIEWSHELL );
    }
    else
    {
        if (ePageKind == PK_NOTES)
        {
            aNotesBtn.Check(TRUE);
            aLayerBtn.Disable();
            SetHelpId( SID_NOTESMODE );
            pWindow->SetHelpId( SID_NOTESMODE );
            pWindow->SetUniqueId( SID_NOTESMODE );

            // AutoLayouts muessen erzeugt sein
            GetDoc()->StopWorkStartupDelay();
        }
        else if (ePageKind == PK_HANDOUT)
        {
            aHandoutBtn.Check(TRUE);
            aPageBtn.Disable();
            aLayerBtn.Disable();
            SetHelpId( SID_HANDOUTMODE );
            pWindow->SetHelpId( SID_HANDOUTMODE );
            pWindow->SetUniqueId( SID_HANDOUTMODE );

            // AutoLayouts muessen erzeugt sein
            GetDoc()->StopWorkStartupDelay();
        }
        else
        {
            aDrawBtn.Check(TRUE);
            SetHelpId( SD_IF_SDDRAWVIEWSHELL );
            pWindow->SetHelpId( SD_IF_SDDRAWVIEWSHELL );
            pWindow->SetUniqueId( SD_IF_SDDRAWVIEWSHELL );
        }
    }

    // Selektionsfunktion starten
    SfxRequest aReq(SID_OBJECT_SELECT, 0, GetDoc()->GetItemPool());
    FuPermanent(aReq);
    pDrView->SetFrameDragSingles(TRUE);

    if (pDocSh->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED)
    {
        bZoomOnPage = FALSE;
    }
    else
    {
        bZoomOnPage = TRUE;
    }

    bIsRulerDrag = FALSE;

    String aName( RTL_CONSTASCII_USTRINGPARAM( "DrawView" ));
    SetName (aName);

    if (pFrameView->GetPresentationViewShellId() != SID_VIEWSHELL0)
    {
        // Save the type of the previous view shell and restore it after
        // starting the presentation function: This is done in the name of
        // that view shell, so setting this view shell as the previous one,
        // as it is done in FuSpport would lead to switching back to the
        // wrong view shell, after the presentation has ended.
        ViewShell::ShellType ePreviousType (
            pFrameView->GetPreviousViewShellType());

        // Die Praesentation ist aus einer anderen ViewShell angewaehlt worden,
        // daraufhin wird zum Zeichentisch (SID_VIEWSHELL0) verzweigt und
        // die Praesentation wird gestartet.
        SfxAllItemSet aSet(GetDoc()->GetItemPool());
        SfxRequest aReq(pFrameView->GetSlotId(), 0, aSet);
        FuSupport(aReq);

        pFrameView->SetPreviousViewShellType (ePreviousType);
    }

    nLockCount = 0UL;

    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );

    if( xMgr.is() )
    {
        mxScannerManager = ::com::sun::star::uno::Reference< ::com::sun::star::scanner::XScannerManager >(
                           xMgr->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.scanner.ScannerManager" ) ),
                           ::com::sun::star::uno::UNO_QUERY );

        if( mxScannerManager.is() )
        {
            mxScannerListener = ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >(
                                static_cast< ::cppu::OWeakObject* >( new ScannerEventListener( this ) ),
                                ::com::sun::star::uno::UNO_QUERY );
        }
    }

    mpController = new SdUnoDrawView (GetViewShellBase(), *this, *pView);
    mxController = static_cast<XWeak*>(mpController);

    DestroyPolygons ();
    pXPolygon = new XPolygon;
    CreateBorder();
}



void DrawViewShell::Init (void)
{
    ViewShell::Init ();
    StartListening (*GetDocSh());
    mpFormShellManager = ::std::auto_ptr<FormShellManager>(
        new FormShellManager (GetViewShellBase()));
}



/*************************************************************************
|*
|* pruefe ob linienzuege gezeichnet werden muessen
|*
\************************************************************************/

/*
    linienzuege werden ueber makros als folge von
        MoveTo (x, y)
        LineTo (x, y)   [oder BezierTo (x, y)]
        LineTo (x, y)
            :
    dargestellt. einen endbefehl fuer die linienzuege
    gibt es nicht, also muessen alle befehle in den
    requests nach LineTo (BezierTo) abgetestet und die
    punktparameter gesammelt werden.
    der erste nicht-LineTo fuehrt dann dazu, dass aus
    den gesammelten punkten der linienzug erzeugt wird
*/

void DrawViewShell::CheckLineTo (SfxRequest& rReq)
{
    if (rReq.IsAPI ())
        if ((rReq.GetSlot () == SID_LINETO) || (rReq.GetSlot () == SID_BEZIERTO))
        {
            if ((bLastWasLineTo && (rReq.GetSlot () == SID_LINETO)) ||
                (bLastWasBezierTo && (rReq.GetSlot () == SID_BEZIERTO)) ||
                bLastWasMoveTo)
            {
                const SfxItemSet* pArgs = rReq.GetArgs ();

                if (pArgs)
                    if (pArgs->Count () == 2)
                    {
                        SFX_REQUEST_ARG (rReq, pMouseStartX, SfxUInt32Item, ID_VAL_MOUSEEND_X, FALSE);
                        SFX_REQUEST_ARG (rReq, pMouseStartY, SfxUInt32Item, ID_VAL_MOUSEEND_Y, FALSE);

                        Point aTempPoint (pMouseStartX->GetValue (), pMouseStartY->GetValue ());
                        if (nPolygonIndex < 30) pXPolygon->Insert (nPolygonIndex ++, aTempPoint, XPOLY_NORMAL);

                        bLastWasLineTo   = (rReq.GetSlot () == SID_LINETO);
                        bLastWasBezierTo = (rReq.GetSlot () == SID_BEZIERTO);
                        bLastWasMoveTo   = FALSE;
                    }
                    else DestroyPolygons ();
                else DestroyPolygons ();
            }
            else DestroyPolygons ();
        }
        else
        {
            if (bLastWasLineTo || bLastWasBezierTo)
            {
                SdrPageView *pPV = pDrView->GetPageViewPvNum (0);

                pDrView->InsertObject (new SdrPathObj (bLastWasLineTo
                                                           ? OBJ_PLIN
                                                           : OBJ_PATHLINE, *pXPolygon), *pPV, SDRINSERT_SETDEFLAYER);
                if (bLastWasBezierTo) pDrView->ConvertMarkedToPathObj(FALSE);
                DestroyPolygons ();
            }

            if (rReq.GetSlot () == SID_MOVETO)
            {
                const SfxItemSet* pArgs = rReq.GetArgs ();

                if (pArgs)
                    if (pArgs->Count () == 2)
                    {
                        SFX_REQUEST_ARG (rReq, pMouseStartX, SfxUInt32Item, ID_VAL_MOUSESTART_X, FALSE);
                        SFX_REQUEST_ARG (rReq, pMouseStartY, SfxUInt32Item, ID_VAL_MOUSESTART_Y, FALSE);

                        nPolygonIndex = 0;
                        Point aTempPoint (pMouseStartX->GetValue (), pMouseStartY->GetValue ());
                        pXPolygon->Insert (nPolygonIndex ++, aTempPoint, XPOLY_NORMAL);

                        bLastWasMoveTo = TRUE;
                    }
                    else DestroyPolygons ();
                else DestroyPolygons ();
            }
            else bLastWasMoveTo = FALSE;
        }

    rReq.Ignore ();
}

/*************************************************************************
|*
|* loesche alle polygone, wenn noetig
|*
\************************************************************************/

void DrawViewShell::DestroyPolygons ()
{
    if (nPolygonIndex)
    {
        if (pXPolygon) delete pXPolygon;
        pXPolygon      = new XPolygon;
        nPolygonIndex  = 0;
        bLineError     =
        bLastWasLineTo =
        bLastWasBezierTo =
        bLastWasMoveTo = FALSE;
    }
}

/*************************************************************************
|*
|* veraendere die seitemparameter, wenn SID_PAGESIZE oder SID_PAGEMARGIN
|*
\************************************************************************/

void DrawViewShell::SetupPage (Size &rSize,
                                 long nLeft,
                                 long nRight,
                                 long nUpper,
                                 long nLower,
                                 BOOL bSize,
                                 BOOL bMargin,
                                 BOOL bScaleAll)
{
    USHORT nPageCnt = GetDoc()->GetMasterSdPageCount(ePageKind);
    USHORT i;

    for (i = 0; i < nPageCnt; i++)
    {
        /**********************************************************************
        * Erst alle MasterPages bearbeiten
        **********************************************************************/
        SdPage *pPage = GetDoc()->GetMasterSdPage(i, ePageKind);

        if( pPage )
        {
            const SfxPoolItem *pPoolItem = NULL;

            if( bSize )
            {
                Rectangle aBorderRect(nLeft, nUpper, nRight, nLower);
                pPage->ScaleObjects(rSize, aBorderRect, bScaleAll);
                pPage->SetSize(rSize);

            }
            if( bMargin )
            {
                pPage->SetLftBorder(nLeft);
                pPage->SetRgtBorder(nRight);
                pPage->SetUppBorder(nUpper);
                pPage->SetLwrBorder(nLower);
            }

            if ( ePageKind == PK_STANDARD )
            {
                GetDoc()->GetMasterSdPage(i, PK_NOTES)->CreateTitleAndLayout();
            }

            pPage->CreateTitleAndLayout();
        }
    }

    nPageCnt = GetDoc()->GetSdPageCount(ePageKind);

    for (i = 0; i < nPageCnt; i++)
    {
        /**********************************************************************
        * Danach alle Pages bearbeiten
        **********************************************************************/
        SdPage *pPage = GetDoc()->GetSdPage(i, ePageKind);

        if( pPage )
        {
            if( bSize )
            {
                Rectangle aBorderRect(nLeft, nUpper, nRight, nLower);
                pPage->ScaleObjects(rSize, aBorderRect, bScaleAll);
                pPage->SetSize(rSize);
            }
            if( bMargin )
            {
                pPage->SetLftBorder(nLeft);
                pPage->SetRgtBorder(nRight);
                pPage->SetUppBorder(nUpper);
                pPage->SetLwrBorder(nLower);
            }

            if ( ePageKind == PK_STANDARD )
            {
                SdPage* pNotesPage = GetDoc()->GetSdPage(i, PK_NOTES);
                pNotesPage->SetAutoLayout( pNotesPage->GetAutoLayout() );
            }

            pPage->SetAutoLayout( pPage->GetAutoLayout() );
        }
    }

    if ( ePageKind == PK_STANDARD )
    {
        SdPage* pHandoutPage = GetDoc()->GetSdPage(0, PK_HANDOUT);
        pHandoutPage->CreateTitleAndLayout(TRUE);
    }

    long nWidth = pActualPage->GetSize().Width();
    long nHeight = pActualPage->GetSize().Height();

    Point aPageOrg = Point(nWidth, nHeight / 2);
    Size aViewSize = Size(nWidth * 3, nHeight * 2);

    InitWindows(aPageOrg, aViewSize, Point(-1, -1), TRUE);
//    GetView()->SetWorkArea(Rectangle(Point(0,0) - aPageOrg, aViewSize));

    Point aVisAreaPos;

    if ( GetDocSh()->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
    {
        aVisAreaPos = GetDocSh()->GetVisArea(ASPECT_CONTENT).TopLeft();
    }

    GetView()->SetWorkArea(Rectangle(Point() - aVisAreaPos - aPageOrg, aViewSize));

    UpdateScrollBars();

    Point aNewOrigin(pActualPage->GetLftBorder(), pActualPage->GetUppBorder());
    GetView()->GetPageViewPvNum(0)->SetPageOrigin(aNewOrigin);

    GetViewFrame()->GetBindings().Invalidate(SID_RULER_NULL_OFFSET);

    // auf (neue) Seitengroesse zoomen
    GetViewFrame()->GetDispatcher()->Execute(SID_SIZE_PAGE,
                        SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
}

/*************************************************************************
|*
|* Statuswerte der Statusbar zurueckgeben
|*
\************************************************************************/

void DrawViewShell::GetStatusBarState(SfxItemSet& rSet)
{
    // Zoom-Item
    // Hier sollte der entsprechende Wert (Optimal ?, Seitenbreite oder
    // Seite) mit Hilfe des ZoomItems weitergegeben werden !!!
    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ATTR_ZOOM ) )
    {
        if (GetDocSh()->IsUIActive() || pFuSlideShow)
        {
            rSet.DisableItem( SID_ATTR_ZOOM );
        }
        else
        {
            SvxZoomItem* pZoomItem;
            UINT16 nZoom = (UINT16) pWindow->GetZoom();

            if( bZoomOnPage )
                pZoomItem = new SvxZoomItem( SVX_ZOOM_WHOLEPAGE, nZoom );
            else
                pZoomItem = new SvxZoomItem( SVX_ZOOM_PERCENT, nZoom );

            // Bereich einschraenken
            USHORT nZoomValues = SVX_ZOOM_ENABLE_ALL;
            SdrPageView* pPageView = pDrView->GetPageViewPvNum( 0 );

            if( ( pPageView && pPageView->GetObjList()->GetObjCount() == 0 ) )
                // || ( pDrView->GetMarkList().GetMarkCount() == 0 ) )
            {
                nZoomValues &= ~SVX_ZOOM_ENABLE_OPTIMAL;
            }

            pZoomItem->SetValueSet( nZoomValues );
            rSet.Put( *pZoomItem );
            delete pZoomItem;
        }
    }

    Point aPos = pWindow->PixelToLogic(aMousePos);
    pDrView->GetPageViewPvNum(0)->LogicToPagePos(aPos);
    Fraction aUIScale(GetDoc()->GetUIScale());
    aPos.X() = Fraction(aPos.X()) / aUIScale;
    aPos.Y() = Fraction(aPos.Y()) / aUIScale;

    // Position- und Groesse-Items
    if ( pDrView->IsAction() )
    {
        Rectangle aRect;
        pDrView->TakeActionRect( aRect );

        if ( aRect.IsEmpty() )
            rSet.Put( SfxPointItem(SID_ATTR_POSITION, aPos) );
        else
        {
            pDrView->GetPageViewPvNum(0)->LogicToPagePos(aRect);
            aPos = aRect.TopLeft();
            aPos.X() = Fraction(aPos.X()) / aUIScale;
            aPos.Y() = Fraction(aPos.Y()) / aUIScale;
            rSet.Put( SfxPointItem( SID_ATTR_POSITION, aPos) );
            Size aSize( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() );
            aSize.Height() = Fraction(aSize.Height()) / aUIScale;
            aSize.Width()  = Fraction(aSize.Width())  / aUIScale;
            rSet.Put( SvxSizeItem( SID_ATTR_SIZE, aSize) );
        }
    }
    else
    {
        if ( pDrView->HasMarkedObj() )
        {
            Rectangle aRect = pDrView->GetAllMarkedRect();
            pDrView->GetPageViewPvNum(0)->LogicToPagePos(aRect);

            // Show the position of the selected shape(s)
            Point aShapePosition (aRect.TopLeft());
            aShapePosition.X() = Fraction(aShapePosition.X()) / aUIScale;
            aShapePosition.Y() = Fraction(aShapePosition.Y()) / aUIScale;
            rSet.Put (SfxPointItem(SID_ATTR_POSITION, aShapePosition));

            Size aSize( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() );
            aSize.Height() = Fraction(aSize.Height()) / aUIScale;
            aSize.Width()  = Fraction(aSize.Width())  / aUIScale;
            rSet.Put( SvxSizeItem( SID_ATTR_SIZE, aSize) );
        }
        else
        {
            rSet.Put( SfxPointItem(SID_ATTR_POSITION, aPos) );
            rSet.Put( SvxSizeItem( SID_ATTR_SIZE, Size( 0, 0 ) ) );
        }
    }

    // Display of current page and layer.
    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_STATUS_PAGE ) )
    {
        // Allways show the slide/page number.
        String aString (SdResId( STR_SD_PAGE ));
        aString += sal_Unicode(' ');
        aString += UniString::CreateFromInt32( aTabControl.GetCurPageId() );
        aString.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " / " ));
        aString += UniString::CreateFromInt32( GetDoc()->GetSdPageCount( ePageKind ) );

        // If in layer mode additionally show the layer that contains all
        // selected shapes of the page.  If the shapes are distributed on
        // more than one layer, no layer name is shown.
        if( bLayerMode )
        {
            SdrLayerAdmin& rLayerAdmin = GetDoc()->GetLayerAdmin();
            SdrLayerID nLayer, nOldLayer;
            SdrLayer*  pLayer = NULL;
            SdrObject* pObj = NULL;
            const SdrMarkList& rMarkList = pDrView->GetMarkList();
            ULONG nMarkCount = rMarkList.GetMarkCount();
            FASTBOOL bOneLayer = TRUE;

            // Use the first ten selected shapes as a (hopefully
            // representative) sample of all shapes of the current page.
            // Detect whether they belong to the same layer.
            for( ULONG j = 0; j < nMarkCount && bOneLayer && j < 10; j++ )
            {
                pObj = rMarkList.GetMark( j )->GetObj();
                if( pObj )
                {
                    nLayer = pObj->GetLayer();

                    if( j != 0 && nLayer != nOldLayer )
                        bOneLayer = FALSE;

                    nOldLayer = nLayer;
                }
            }

            // Append the layer name to the current page number.
            if( bOneLayer && nMarkCount )
            {
                pLayer = rLayerAdmin.GetLayerPerID( nLayer );
                if( pLayer )
                {
                    aString.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " (" ));
                    aString += pLayer->GetName();
                    aString += sal_Unicode(')');
                }
            }
        }

        rSet.Put (SfxStringItem (SID_STATUS_PAGE, aString));
    }
    // Layout
    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_STATUS_LAYOUT ) )
    {
        String aString = pActualPage->GetLayoutName();
        aString.Erase( aString.SearchAscii( SD_LT_SEPARATOR ) );
        rSet.Put( SfxStringItem( SID_STATUS_LAYOUT, aString ) );
    }
}



void DrawViewShell::Notify (SfxBroadcaster& rBC, const SfxHint& rHint)
{

    SfxSimpleHint* pSimple = PTR_CAST(SfxSimpleHint, &rHint);
    if (pSimple!=NULL && pSimple->GetId()==SFX_HINT_MODECHANGED)
    {
        // Change to selection when turning on read-only mode.
        if (GetDocSh()->IsReadOnly()
            && pFuActual!=NULL
            && !pFuActual->ISA(FuSelection))
        {
            SfxRequest aReq(SID_OBJECT_SELECT, 0, GetDoc()->GetItemPool());
            FuPermanent(aReq);
        }

        // Turn on design mode when document is not read-only.
        if (GetDocSh()->IsReadOnly() != bReadOnly )
        {
            bReadOnly = GetDocSh()->IsReadOnly();

            SfxBoolItem aItem( SID_FM_DESIGN_MODE, !bReadOnly );
            GetViewFrame()->GetDispatcher()->Execute( SID_FM_DESIGN_MODE,
                SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );
        }
    }

}


} // end of namespace sd
