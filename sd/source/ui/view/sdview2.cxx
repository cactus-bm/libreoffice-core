/*************************************************************************
 *
 *  $RCSfile: sdview2.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: ka $ $Date: 2001-11-23 14:12:30 $
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

#pragma hdrstop


#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif
#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _SVDETC_HXX //autogen
#include <svx/svdetc.hxx>
#endif
#ifndef _IPOBJ_HXX //autogen
#include <so3/ipobj.hxx>
#endif
#ifndef _SVDOOLE2_HXX //autogen
#include <svx/svdoole2.hxx>
#endif
#ifndef _SVDOGRAF_HXX //autogen
#include <svx/svdograf.hxx>
#endif
#ifndef _SV_GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _SVX_XEXCH_HXX //autogen
#include <svx/xexch.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX //autogen
#include <sfx2/childwin.hxx>
#endif
#ifndef _SVDUNDO_HXX //autogen
#include <svx/svdundo.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _URLBMK_HXX //autogen
#include <svtools/urlbmk.hxx>
#endif
#ifndef _URLBMK_HXX //autogen
#include <svtools/urlbmk.hxx>
#endif
#ifndef _OUTLINER_HXX //autogen
#include <svx/outliner.hxx>
#endif
#ifndef _SVX_XFLCLIT_HXX
#include <svx/xflclit.hxx>
#endif
#include <svx/dbexch.hrc>
#ifndef _SOT_FORMATS_HXX //autogen
#include <sot/formats.hxx>
#endif
#ifndef _MyEDITENG_HXX //autogen
#include <svx/editeng.hxx>
#endif

#include "navigatr.hxx"
#include "anminfo.hxx"
#include "strings.hrc"
#include "sdview.hxx"
#include "sdxfer.hxx"
#include "sdresid.hxx"
#include "sdmod.hxx"
#include "drviewsh.hxx"
#include "docshell.hxx"
#include "fudraw.hxx"
#include "drawdoc.hxx"
#include "sdwindow.hxx"
#include "sdpage.hxx"
#include "unoaprms.hxx"
#include "drawview.hxx"
#include "slidview.hxx"

#ifndef SO2_DECL_SVINPLACEOBJECT_DEFINED
#define SO2_DECL_SVINPLACEOBJECT_DEFINED
SO2_DECL_REF(SvInPlaceObject)
#endif
#ifndef SO2_DECL_SVSTORAGE_DEFINED
#define SO2_DECL_SVSTORAGE_DEFINED
SO2_DECL_REF(SvStorage)
#endif

using namespace ::com::sun::star;

// ------------------------
// - SdNavigatorDropEvent -
// ------------------------

struct SdNavigatorDropEvent : public ExecuteDropEvent
{
    DropTargetHelper&       mrTargetHelper;
    SdWindow*               mpTargetWindow;
    USHORT                  mnPage;
    USHORT                  mnLayer;
    const SdDrawDocShell*   mpNavigatorDragDocShell;

                SdNavigatorDropEvent( const ExecuteDropEvent& rEvt, DropTargetHelper& rTargetHelper,
                                      SdWindow* pTargetWindow, USHORT nPage, USHORT nLayer, const SdDrawDocShell* pNavigatorDragDocShell ) :
                    ExecuteDropEvent( rEvt ),
                    mrTargetHelper( rTargetHelper ),
                    mpTargetWindow( pTargetWindow ),
                    mnPage( nPage ),
                    mnLayer( nLayer ),
                    mpNavigatorDragDocShell( pNavigatorDragDocShell ) {}
};

// -----------------------------------------------------------------------------

::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > SdView::CreateClipboardDataObject( SdView* pWorkView, Window& rWindow )
{
    // since SdTransferable::CopyToClipboard is called, this
    // dynamically created object ist destroyed automatically
    SdTransferable* pTransferable = new SdTransferable( pDoc, NULL, FALSE );
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > xRet( pTransferable );

    SD_MOD()->pTransferClip = pTransferable;

    pDoc->CreatingDataObj( pTransferable );
    pTransferable->SetWorkDocument( (SdDrawDocument*) GetAllMarkedModel() );
    pDoc->CreatingDataObj( NULL );

    const Rectangle                 aMarkRect( GetAllMarkedRect() );
    TransferableObjectDescriptor    aObjDesc;
    String                          aDisplayName;
    SdrOle2Obj*                     pSdrOleObj = NULL;
    SdrPageView*                    pPgView = GetPageViewPvNum( 0 );
    SdPage*                         pOldPage = pPgView ? ( (SdPage*) pPgView->GetPage() ) : NULL;
    SdPage*                         pNewPage = (SdPage*) pTransferable->GetWorkDocument()->GetPage( 0 );

    if( pOldPage )
    {
        pNewPage->SetSize( pOldPage->GetSize() );
        pNewPage->SetLayoutName( pOldPage->GetLayoutName() );
    }

    if( aMark.GetMarkCount() == 1 )
    {
        SdrObject* pObj = aMark.GetMark(0)->GetObj();

        if( pObj && pObj->ISA(SdrOle2Obj) && ((SdrOle2Obj*) pObj)->GetObjRef().Is() )
            pSdrOleObj = (SdrOle2Obj*) pObj;
    }

    if( pSdrOleObj )
        pSdrOleObj->GetObjRef()->FillTransferableObjectDescriptor( aObjDesc );
    else
        pTransferable->GetWorkDocument()->GetDocSh()->FillTransferableObjectDescriptor( aObjDesc );

    if( pDocSh )
        aObjDesc.maDisplayName = pDocSh->GetMedium()->GetURLObject().GetURLNoPass();

    aObjDesc.maSize = aMarkRect.GetSize();

    pTransferable->SetStartPos( aMarkRect.TopLeft() );
    pTransferable->SetObjectDescriptor( aObjDesc );
    pTransferable->CopyToClipboard( pViewSh->GetActiveWindow() );

    return xRet;
}

// -----------------------------------------------------------------------------

::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > SdView::CreateDragDataObject( SdView* pWorkView, Window& rWindow, const Point& rDragPos )
{
    SdTransferable* pTransferable = new SdTransferable( pDoc, pWorkView, FALSE );
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > xRet( pTransferable );

    SD_MOD()->pTransferDrag = pTransferable;

    TransferableObjectDescriptor    aObjDesc;
    String                          aDisplayName;
    SdrOle2Obj*                     pSdrOleObj = NULL;

    if( aMark.GetMarkCount() == 1 )
    {
        SdrObject* pObj = aMark.GetMark( 0 )->GetObj();

        if( pObj && pObj->ISA( SdrOle2Obj ) && ( (SdrOle2Obj*) pObj )->GetObjRef().Is() )
            pSdrOleObj = (SdrOle2Obj*) pObj;
    }

    if( pDocSh )
        aDisplayName = pDocSh->GetMedium()->GetURLObject().GetURLNoPass();

    if( pSdrOleObj )
        pSdrOleObj->GetObjRef()->FillTransferableObjectDescriptor( aObjDesc );
    else
        pDocSh->FillTransferableObjectDescriptor( aObjDesc );

    aObjDesc.maSize = GetAllMarkedRect().GetSize();
    aObjDesc.maDragStartPos = rDragPos;
    aObjDesc.maDisplayName = aDisplayName;
    aObjDesc.mbCanLink = FALSE;

    pTransferable->SetStartPos( rDragPos );
    pTransferable->SetObjectDescriptor( aObjDesc );
    pTransferable->StartDrag( &rWindow, DND_ACTION_COPYMOVE | DND_ACTION_LINK );

    return xRet;
}

// -----------------------------------------------------------------------------

::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > SdView::CreateSelectionDataObject( SdView* pWorkView, Window& rWindow )
{
    SdTransferable*                 pTransferable = new SdTransferable( pDoc, pWorkView, TRUE );
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > xRet( pTransferable );
    TransferableObjectDescriptor    aObjDesc;
    const Rectangle                 aMarkRect( GetAllMarkedRect() );
    String                          aDisplayName;

    SD_MOD()->pTransferSelection = pTransferable;

    if( pDocSh )
    {
        aDisplayName = pDocSh->GetMedium()->GetURLObject().GetURLNoPass();
        pDocSh->FillTransferableObjectDescriptor( aObjDesc );
    }

    aObjDesc.maSize = aMarkRect.GetSize();

    pTransferable->SetStartPos( aMarkRect.TopLeft() );
    pTransferable->SetObjectDescriptor( aObjDesc );
    pTransferable->CopyToSelection( &rWindow );

    return xRet;
}

// -----------------------------------------------------------------------------

void SdView::UpdateSelectionClipboard( BOOL bForceDeselect )
{
    if( pViewSh && pViewSh->GetActiveWindow() )
    {
        if( !bForceDeselect && GetMarkList().GetMarkCount() )
            CreateSelectionDataObject( this, *pViewSh->GetActiveWindow() );
        else if( SD_MOD()->pTransferSelection && ( SD_MOD()->pTransferSelection->GetView() == this ) )
        {
            TransferableHelper::ClearSelection( pViewSh->GetActiveWindow() );
            SD_MOD()->pTransferSelection = NULL;
        }
    }
}

// -----------------------------------------------------------------------------

void SdView::DoCut(Window* pWindow)
{
    const OutlinerView* pOLV = GetTextEditOutlinerView();

    if( pOLV )
        ( (OutlinerView*) pOLV)->Cut();
    else if( HasMarkedObj() )
    {
        String aStr( SdResId(STR_UNDO_CUT) );

        DoCopy();
        BegUndo( ( aStr += sal_Unicode(' ') ) += aMark.GetMarkDescription() );
        DeleteMarked();
        EndUndo();
    }
}

// -----------------------------------------------------------------------------

void SdView::DoCopy(Window* pWindow)
{
    const OutlinerView* pOLV = GetTextEditOutlinerView();

    if( pOLV )
        ( (OutlinerView*) pOLV)->Copy();
    else if( HasMarkedObj() )
    {
        BrkAction();
        CreateClipboardDataObject( this, *pWindow );
    }
}

// -----------------------------------------------------------------------------

void SdView::DoPaste( Window* pWindow )
{
    const OutlinerView* pOLV = GetTextEditOutlinerView();

    if( pOLV )
    {
        ( (OutlinerView*) pOLV)->PasteSpecial();

        SdrObject*  pObj = GetTextEditObject();
        SdPage*     pPage = (SdPage*)( pObj ? pObj->GetPage() : NULL );
        Outliner*   pOutliner = pOLV->GetOutliner();

        if( pOutliner)
        {
            if( pObj && pPage && pPage->GetPresObjKind(pObj) == PRESOBJ_TITLE )
            {
                // remove all hard linebreaks from the title
                if( pOutliner && pOutliner->GetParagraphCount() > 1 )
                {
                    BOOL bOldUpdateMode = pOutliner->GetUpdateMode();

                    pOutliner->SetUpdateMode( FALSE );

                    const EditEngine& rEdit = pOutliner->GetEditEngine();
                    const int nParaCount = rEdit.GetParagraphCount();

                    for( int nPara = nParaCount - 2; nPara >= 0; nPara-- )
                    {
                        const int nParaLen = rEdit.GetTextLen( nPara );
                        pOutliner->QuickDelete( ESelection( nPara, nParaLen, nPara+1, 0 ) );
                        pOutliner->QuickInsertLineBreak( ESelection( nPara, nParaLen, nPara, nParaLen ) );
                    }

                    DBG_ASSERT( rEdit.GetParagraphCount() <= 1, "Titelobjekt contains hard line breaks" );
                    pOutliner->SetUpdateMode(bOldUpdateMode);
                }
            }

            if( !pDoc->IsChanged() )
            {
                if( pOutliner && pOutliner->IsModified() )
                    pDoc->SetChanged( TRUE );
            }
        }
    }
    else
    {
        TransferableDataHelper aDataHelper( TransferableDataHelper::CreateFromSystemClipboard( pViewSh->GetActiveWindow() ) );

        if( aDataHelper.GetTransferable().is() )
        {
            Point       aPos;
            sal_Int8    nDnDAction = DND_ACTION_COPY;

            if( pWindow )
                aPos = pWindow->PixelToLogic( Rectangle( aPos, pWindow->GetOutputSizePixel() ).Center() );

            if( !InsertData( aDataHelper, aPos, nDnDAction, FALSE ) )
            {
                SdDrawViewShell* pDrViewSh = (SdDrawViewShell*) pDocSh->GetViewShell();

                if( pDrViewSh )
                {
                    String          aEmptyStr;
                    INetBookmark    aINetBookmark( aEmptyStr, aEmptyStr );

                    if( ( aDataHelper.HasFormat( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK ) &&
                          aDataHelper.GetINetBookmark( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK, aINetBookmark ) ) ||
                        ( aDataHelper.HasFormat( SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR ) &&
                          aDataHelper.GetINetBookmark( SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR, aINetBookmark ) ) ||
                        ( aDataHelper.HasFormat( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR ) &&
                          aDataHelper.GetINetBookmark( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR, aINetBookmark ) ) )
                    {
                        pDrViewSh->InsertURLField( aINetBookmark.GetURL(), aINetBookmark.GetDescription(), aEmptyStr, NULL );
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

void SdView::StartDrag( const Point& rStartPos, Window* pWindow )
{
    if( HasMarkedObj() && IsAction() && pViewSh && pWindow )
    {
        BrkAction();

        if( IsTextEdit() )
            EndTextEdit();

        SdViewShell* pViewShell= pDocSh->GetViewShell();

        if( pViewShell && pViewShell->ISA( SdDrawViewShell ) )
        {
            FuPoor* pFunc = ( (SdDrawViewShell*) pViewShell )->GetActualFunction();

            if( pFunc && pFunc->ISA( FuDraw ) )
                ( (FuDraw*) pFunc)->ForcePointer( NULL );
        }

        pDragSrcMarkList = new SdrMarkList( aMark );
        nDragSrcPgNum = GetPageViewPvNum(0)->GetPage()->GetPageNum();

        String aStr( SdResId(STR_UNDO_DRAGDROP) );
        aStr += sal_Unicode(' ');
        aStr += pDragSrcMarkList->GetMarkDescription();
        BegUndo(aStr);
        CreateDragDataObject( this, *pWindow, rStartPos );
    }
}

// -----------------------------------------------------------------------------

void SdView::DragFinished( sal_Int8 nDropAction )
{
    SdTransferable* pDragTransferable = SD_MOD()->pTransferDrag;

    if( pDragTransferable )
        pDragTransferable->SetView( NULL );

    if( ( nDropAction & DND_ACTION_MOVE ) && !pDragTransferable->IsInternalMove() &&
        !IsPresObjSelected() && pDragSrcMarkList && pDragSrcMarkList->GetMarkCount() )
    {
        pDragSrcMarkList->ForceSort();
        BegUndo();

        ULONG nm, nAnz = pDragSrcMarkList->GetMarkCount();

         for( nm = nAnz; nm>0; )
        {
            nm--;
            SdrMark* pM=pDragSrcMarkList->GetMark(nm);
            AddUndo(new SdrUndoDelObj(*pM->GetObj()));
        }

        pDragSrcMarkList->GetMark(0)->GetObj()->GetOrdNum();

        for (nm=nAnz; nm>0;)
        {
            nm--;
            SdrMark* pM=pDragSrcMarkList->GetMark(nm);
            SdrObject* pObj=pM->GetObj();
            UINT32 nOrdNum=pObj->GetOrdNumDirect();
            SdrObject* pChkObj = pObj->GetPage()->RemoveObject(nOrdNum);
            DBG_ASSERT(pChkObj==pObj,"DeleteMarked(MarkList): pChkObj!=pObj beim RemoveObject()");
        }

        EndUndo();
    }

    if( pDragTransferable )
        pDragTransferable->SetInternalMove( FALSE );

    EndUndo();
    nDragSrcPgNum = SDRPAGE_NOTFOUND;
    delete pDragSrcMarkList, pDragSrcMarkList = NULL;
}

// -----------------------------------------------------------------------------

sal_Int8 SdView::AcceptDrop( const AcceptDropEvent& rEvt, DropTargetHelper& rTargetHelper,
                             SdWindow* pTargetWindow, USHORT nPage, USHORT nLayer )
{
    String          aLayerName( GetActiveLayer() );
    SdrPageView*    pPV = GetPageViewPvNum(0);
    sal_Int8        nDropAction = rEvt.mnAction;
    sal_Int8        nRet = DND_ACTION_NONE;

    if( nLayer != SDRLAYER_NOTFOUND )
    {
        SdrLayerAdmin& rLayerAdmin = pDoc->GetLayerAdmin();
        aLayerName = rLayerAdmin.GetLayerPerID(nLayer)->GetName();
    }

    if( bIsDropAllowed && !pPV->IsLayerLocked( aLayerName ) && pPV->IsLayerVisible( aLayerName ) )
    {
        const OutlinerView* pOLV = GetTextEditOutlinerView();
        BOOL                bIsInsideOutlinerView = FALSE;

        if( pOLV )
        {
            Rectangle aRect( pOLV->GetOutputArea() );

            if (aMark.GetMarkCount() == 1)
            {
                SdrMark* pMark = aMark.GetMark(0);
                SdrObject* pObj = pMark->GetObj();
                aRect.Union( pObj->GetLogicRect() );
            }

            if( aRect.IsInside( pOLV->GetWindow()->PixelToLogic( rEvt.maPosPixel ) ) )
            {
                bIsInsideOutlinerView = TRUE;
            }
        }

        if( !bIsInsideOutlinerView )
        {
            SdTransferable* pDragTransferable = SD_MOD()->pTransferDrag;

            if( pDragTransferable )
            {
                const SdView* pSourceView = pDragTransferable->GetView();

                if( pSourceView )
                {
                    if( !( nDropAction & DND_ACTION_LINK ) ||
                        pSourceView->GetDocSh()->GetMedium()->GetName().Len() ||
                        pDragTransferable->IsPageTransferable() )
                    {
                        nRet = nDropAction;
                    }
                }
                else if( pDragTransferable->IsPageTransferable() )
                    nRet = nDropAction;
            }
            else
            {
                const BOOL  bDrawing = rTargetHelper.IsDropFormatSupported( SOT_FORMATSTR_ID_DRAWING );
                const BOOL  bGraphic = rTargetHelper.IsDropFormatSupported( SOT_FORMATSTR_ID_SVXB );
                const BOOL  bMtf = rTargetHelper.IsDropFormatSupported( FORMAT_GDIMETAFILE );
                const BOOL  bBitmap = rTargetHelper.IsDropFormatSupported( FORMAT_BITMAP );
                BOOL        bBookmark = rTargetHelper.IsDropFormatSupported( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK );
                BOOL        bXFillExchange = rTargetHelper.IsDropFormatSupported( SOT_FORMATSTR_ID_XFA );

                // check handle insert
                if( !nRet && ( bXFillExchange && ( SDRDRAG_GRADIENT == GetDragMode() ) || ( SDRDRAG_TRANSPARENCE == GetDragMode() ) ) )
                {
                    const SdrHdlList& rHdlList = GetHdlList();

                    for( sal_uInt32 n = 0; n < rHdlList.GetHdlCount(); n++ )
                    {
                        SdrHdl* pIAOHandle = rHdlList.GetHdl( n );

                        if( pIAOHandle && ( HDL_COLR == pIAOHandle->GetKind() ) )
                        {
                            const B2dIAOGroup& rIAOGroup = pIAOHandle->GetIAOGroup();

                            if( rIAOGroup.IsHit( rEvt.maPosPixel ) )
                            {
                                nRet = nDropAction;
                                static_cast< SdrHdlColor* >( pIAOHandle )->SetSize( SDR_HANDLE_COLOR_SIZE_SELECTED );
                            }
                            else
                                static_cast< SdrHdlColor* >( pIAOHandle )->SetSize( SDR_HANDLE_COLOR_SIZE_NORMAL );
                        }
                    }

                    RefreshAllIAOManagers();
                }

                // check object insert
                if( !nRet && ( bXFillExchange || ( ( bDrawing || bGraphic || bMtf || bBitmap || bBookmark ) && ( nDropAction & DND_ACTION_LINK ) ) ) )
                {
                    SdrObject*      pPickObj = NULL;
                    SdrPageView*    pPV = NULL;
                    SdWindow*       pWindow = pViewSh->GetActiveWindow();
                    USHORT          nHitLog = (USHORT) pWindow->PixelToLogic( Size( HITPIX, 0 ) ).Width();
                    Point           aPos( pWindow->PixelToLogic( rEvt.maPosPixel ) );
                    const BOOL      bHasPickObj = PickObj( aPos, pPickObj, pPV );
                    BOOL            bIsPresTarget = FALSE;

                    if( bHasPickObj && pPickObj && ( pPickObj->IsEmptyPresObj() || pPickObj->GetUserCall() ) )
                    {
                        SdPage* pPage = (SdPage*) pPickObj->GetPage();

                        if( pPage && pPage->IsMasterPage() )
                            bIsPresTarget = pPage->GetPresObjList()->GetPos( pPickObj ) != LIST_ENTRY_NOTFOUND;
                    }

                    if( bHasPickObj && !bIsPresTarget &&
                        ( !pPickObj->ISA( SdrGrafObj ) || bGraphic || bMtf || bBitmap || ( bXFillExchange && !pPickObj->ISA( SdrGrafObj ) && !pPickObj->ISA( SdrOle2Obj ) ) ) )
                    {
                        if( !pDropMarker )
                            pDropMarker = new SdrViewUserMarker(this);

                        if( pDropMarkerObj != pPickObj )
                        {
                            pDropMarkerObj = pPickObj;
                            pDropMarker->SetXPolyPolygon( pDropMarkerObj, GetPageViewPvNum( 0 ) );
                            pDropMarker->Show();
                        }

                        nRet = nDropAction;
                    }
                    else
                        bXFillExchange = FALSE;
                }

                // check normal insert
                if( !nRet )
                {
                    const BOOL  bSBAFormat = rTargetHelper.IsDropFormatSupported( SOT_FORMATSTR_ID_SVX_FORMFIELDEXCH );
                    const BOOL  bEditEngine = rTargetHelper.IsDropFormatSupported( SOT_FORMATSTR_ID_EDITENGINE );
                    const BOOL  bString = rTargetHelper.IsDropFormatSupported( FORMAT_STRING );
                    const BOOL  bRTF = rTargetHelper.IsDropFormatSupported( FORMAT_RTF );
                    const BOOL  bFile = rTargetHelper.IsDropFormatSupported( FORMAT_FILE );

                    if( pDropMarker )
                    {
                        pDropMarker->Hide();
                        pDropMarkerObj = NULL;
                    }

                    if( bBookmark && bFile && ( nDropAction & DND_ACTION_MOVE ) && ( !pViewSh || pViewSh->GetSlideShow() ) )
                        bBookmark = FALSE;

                    if( bDrawing || bGraphic || bMtf || bBitmap || bBookmark || bFile || bXFillExchange || bSBAFormat || bEditEngine || bString || bRTF )
                        nRet = nDropAction;
                }
            }
        }
    }

    // destroy drop marker if this is a leaving event
    if( rEvt.mbLeaving && pDropMarker )
    {
        pDropMarker->Hide();
        delete pDropMarker, pDropMarker = NULL;
        pDropMarkerObj = NULL;
    }

    return nRet;
}

// -----------------------------------------------------------------------------

sal_Int8 SdView::ExecuteDrop( const ExecuteDropEvent& rEvt, DropTargetHelper& rTargetHelper,
                              SdWindow* pTargetWindow, USHORT nPage, USHORT nLayer )
{
    SdrPageView*    pPV = GetPageViewPvNum( 0 );
    String          aActiveLayer = GetActiveLayer();
    sal_Int8        nDropAction = rEvt.mnAction;
    sal_Int8        nRet = DND_ACTION_NONE;

    // destroy drop marker if it is shown
    if( pDropMarker )
    {
        pDropMarker->Hide();
        delete pDropMarker, pDropMarker = NULL;
        pDropMarkerObj = NULL;
    }

    if( !pPV->IsLayerLocked( aActiveLayer ) )
    {
        const OutlinerView* pOLV = GetTextEditOutlinerView();
        BOOL                bIsInsideOutlinerView = FALSE;

        if( pOLV )
        {
            Rectangle aRect( pOLV->GetOutputArea() );

            if( aMark.GetMarkCount() == 1 )
            {
                SdrMark* pMark = aMark.GetMark(0);
                SdrObject* pObj = pMark->GetObj();
                aRect.Union( pObj->GetLogicRect() );
            }

            Point aPos( pOLV->GetWindow()->PixelToLogic( rEvt.maPosPixel ) );

            if( aRect.IsInside( aPos ) )
            {
                bIsInsideOutlinerView = TRUE;
            }
        }

        if( !bIsInsideOutlinerView )
        {
            Point                   aPos;
            SdDrawViewShell*        pDrViewSh = (SdDrawViewShell*) pDocSh->GetViewShell();
            SdrPage*                pPage = NULL;
            TransferableDataHelper  aDataHelper( rEvt.maDropEvent.Transferable );

            if( pTargetWindow )
                aPos = pTargetWindow->PixelToLogic( rEvt.maPosPixel );

            // handle insert?
            if( !nRet && ( SDRDRAG_GRADIENT == GetDragMode() ) || ( SDRDRAG_TRANSPARENCE == GetDragMode() ) && aDataHelper.HasFormat( SOT_FORMATSTR_ID_XFA ) )
            {
                const SdrHdlList& rHdlList = GetHdlList();

                for( sal_uInt32 n = 0; !nRet && n < rHdlList.GetHdlCount(); n++ )
                {
                    SdrHdl* pIAOHandle = rHdlList.GetHdl( n );

                    if( pIAOHandle && ( HDL_COLR == pIAOHandle->GetKind() ) )
                    {
                        const B2dIAOGroup& rIAOGroup = pIAOHandle->GetIAOGroup();

                        if( rIAOGroup.IsHit( rEvt.maPosPixel ) )
                        {
                            SotStorageStreamRef xStm;

                            if( aDataHelper.GetSotStorageStream( SOT_FORMATSTR_ID_XFA, xStm ) && xStm.Is() )
                            {
                                XFillExchangeData aFillData( XFillAttrSetItem( &pDoc->GetPool() ) );

                                *xStm >> aFillData;
                                const Color aColor( ( (XFillColorItem&) aFillData.GetXFillAttrSetItem()->GetItemSet().Get( XATTR_FILLCOLOR ) ).GetValue() );
                                static_cast< SdrHdlColor* >( pIAOHandle )->SetColor( aColor, TRUE );
                                nRet = nDropAction;
                            }
                        }
                    }
                }
            }

            // standard insert?
            if( !nRet && InsertData( aDataHelper, aPos, nDropAction, TRUE, 0, nPage, nLayer ) )
                nRet = nDropAction;

            // special insert?
            if( !nRet && pViewSh )
            {
                String          aTmpString1, aTmpString2;
                INetBookmark    aINetBookmark( aTmpString1, aTmpString2 );

                // insert bookmark
                if( aDataHelper.HasFormat( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK ) &&
                    aDataHelper.GetINetBookmark( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK, aINetBookmark ) )
                {
                    const NavigatorDragType eDragType = SD_MOD()->GetCurrentNavigatorDragType();

                    if( eDragType == NAVIGATOR_DRAGTYPE_LINK || eDragType == NAVIGATOR_DRAGTYPE_EMBEDDED )
                    {
                        // insert bookmark from own navigator (handled async. due to possible message box )
                        Application::PostUserEvent( LINK( this, SdView, ExecuteNavigatorDrop ),
                                                    new SdNavigatorDropEvent( rEvt, rTargetHelper, pTargetWindow,
                                                                              nPage, nLayer,
                                                                              SD_MOD()->GetCurrentNavigatorDragDocShell() ) );
                        nRet = nDropAction;
                    }
                    else
                    {
                        SdrObject*      pPickObj = NULL;
                        SdrPageView*    pPV = NULL;
                        SdWindow*       pWindow = pViewSh->GetActiveWindow();
                        USHORT          nHitLog = USHORT(pWindow->PixelToLogic(Size(HITPIX,0)).Width());

                        if( PickObj( aPos, pPickObj, pPV ) )
                        {
                            // insert as clip action => jump
                            String              aBookmark( aINetBookmark.GetURL() );
                            SdAnimationInfo*    pInfo = pDoc->GetAnimationInfo( pPickObj );
                            BOOL                bCreated = FALSE;

                            if( aBookmark.Len() )
                            {
                                presentation::ClickAction eClickAction = presentation::ClickAction_DOCUMENT;
                                String aDocName( aBookmark.GetToken( 0, '#' ) );

                                if( pDocSh->GetMedium()->GetName() == aDocName || pDocSh->GetName() == aDocName )
                                {
                                    // Interner Sprung -> nur "#Bookmark" verwenden
                                    aBookmark = aBookmark.GetToken( 1, '#' );
                                    eClickAction = presentation::ClickAction_BOOKMARK;
                                }

                                if( !pInfo )
                                {
                                    pInfo = new SdAnimationInfo( pDoc );
                                    pPickObj->InsertUserData( pInfo );
                                    bCreated = TRUE;
                                }

                                // Undo-Action mit alten und neuen Groessen erzeugen
                                SdAnimationPrmsUndoAction* pAction = new SdAnimationPrmsUndoAction(pDoc, pPickObj, bCreated);
                                pAction->SetActive(pInfo->bActive, pInfo->bActive);
                                pAction->SetEffect(pInfo->eEffect, pInfo->eEffect);
                                pAction->SetTextEffect(pInfo->eTextEffect, pInfo->eTextEffect);
                                pAction->SetSpeed(pInfo->eSpeed, pInfo->eSpeed);
                                pAction->SetDim(pInfo->bDimPrevious, pInfo->bDimPrevious);
                                pAction->SetDimColor(pInfo->aDimColor, pInfo->aDimColor);
                                pAction->SetDimHide(pInfo->bDimHide, pInfo->bDimHide);
                                pAction->SetSoundOn(pInfo->bSoundOn, pInfo->bSoundOn);
                                pAction->SetSound(pInfo->aSoundFile, pInfo->aSoundFile);
                                pAction->SetPlayFull(pInfo->bPlayFull, pInfo->bPlayFull);
                                pAction->SetPathObj(pInfo->pPathObj, pInfo->pPathObj);
                                pAction->SetClickAction(pInfo->eClickAction, eClickAction);
                                pAction->SetBookmark(pInfo->aBookmark, aBookmark);
                                pAction->SetInvisibleInPres(pInfo->bInvisibleInPresentation, TRUE);
                                pAction->SetVerb(pInfo->nVerb, pInfo->nVerb);
                                pAction->SetSecondEffect(pInfo->eSecondEffect, pInfo->eSecondEffect);
                                pAction->SetSecondSpeed(pInfo->eSecondSpeed, pInfo->eSecondSpeed);
                                pAction->SetSecondSoundOn(pInfo->bSecondSoundOn, pInfo->bSecondSoundOn);
                                pAction->SetSecondPlayFull(pInfo->bSecondPlayFull, pInfo->bSecondPlayFull);

                                String aString(SdResId(STR_UNDO_ANIMATION));
                                pAction->SetComment(aString);
                                pDocSh->GetUndoManager()->AddUndoAction(pAction);
                                pInfo->eClickAction = eClickAction;
                                pInfo->aBookmark = aBookmark;
                                pDoc->SetChanged();

                                nRet = nDropAction;
                            }
                        }
                        else if( pViewSh->ISA( SdDrawViewShell ) )
                        {
                            // insert as normal URL button
                            ( (SdDrawViewShell*) pViewSh )->InsertURLButton( aINetBookmark.GetURL(), aINetBookmark.GetDescription(), String(), &aPos );
                            nRet = nDropAction;
                        }
                    }
                }
            }
        }
    }

    return nRet;
}

// -----------------------------------------------------------------------------

IMPL_LINK( SdView, ExecuteNavigatorDrop, SdNavigatorDropEvent*, pSdNavigatorDropEvent )
{
    TransferableDataHelper  aDataHelper( pSdNavigatorDropEvent->maDropEvent.Transferable );
    INetBookmark            aINetBookmark;

    if( pSdNavigatorDropEvent->mpNavigatorDragDocShell &&
        aDataHelper.GetINetBookmark( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK, aINetBookmark ) )
    {
        Point   aPos;
        List    aBookmarkList;
        String  aBookmark;
        String  aFile;
        SdPage* pPage = (SdPage*) GetPageViewPvNum( 0 )->GetPage();
        USHORT  nPgPos = 0xFFFF;

        if( pSdNavigatorDropEvent->mpTargetWindow )
            aPos = pSdNavigatorDropEvent->mpTargetWindow->PixelToLogic( pSdNavigatorDropEvent->maPosPixel );

        aFile = aINetBookmark.GetURL().GetToken( 0, '#' );
        aBookmark = aINetBookmark.GetURL().GetToken( 1, '#' );
        aBookmarkList.Insert( &aBookmark );

        if( !pPage->IsMasterPage() )
        {
            if( pPage->GetPageKind() == PK_STANDARD )
                nPgPos = pPage->GetPageNum() + 2;
            else if( pPage->GetPageKind() == PK_NOTES )
                nPgPos = pPage->GetPageNum() + 1;
        }

        // Um zu gewaehrleisten, dass alle Seitennamen eindeutig sind, werden
        // die einzufuegenden geprueft und gegebenenfalls in einer Ersatzliste
        // aufgenommen (bNameOK == FALSE -> Benutzer hat abgebrochen)
        List*   pExchangeList = NULL;

        // #93240# swapped order: GetExchangeList() ends drop, which in turn resets GetCurrentNavigatorDragType.
        BOOL    bLink = ( SD_MOD()->GetCurrentNavigatorDragType() == NAVIGATOR_DRAGTYPE_LINK ? TRUE : FALSE );
        BOOL    bNameOK = GetExchangeList( pExchangeList, &aBookmarkList, 2 );
        BOOL    bReplace = FALSE;

        // Da man hier nicht weiss, ob es sich um eine Seite oder ein Objekt handelt,
        // wird eine Liste sowohl mit Seiten, als auch mit Objekten gefuellt.
        // Sollten Seitennamen und Objektnamen identisch sein gibt es hier natuerlich Probleme !!!
        if( bNameOK )
        {
            pDoc->InsertBookmark( &aBookmarkList, pExchangeList,
                                  bLink, bReplace, nPgPos, FALSE,
                                  (SdDrawDocShell*) pSdNavigatorDropEvent->mpNavigatorDragDocShell,
                                  TRUE, &aPos );
        }

        // Loeschen der ExchangeList
        if( pExchangeList )
        {
            for( void* p = pExchangeList->First(); p; p = pExchangeList->Next() )
                delete (String*) p;

            delete pExchangeList;
        }
    }

    delete pSdNavigatorDropEvent;

    return 0;
}

/*************************************************************************
|*
|* Rueckgabeparameter:
|* pExchangeList == NULL -> Namen sind alle eindeutig
|* bNameOK == FALSE -> Benutzer hat abgebrochen
|* nType == 0 -> Seiten
|* nType == 1 -> Objekte
|* nType == 2 -> Seiten + Objekte
|*
\************************************************************************/

BOOL SdView::GetExchangeList( List*& rpExchangeList, List* pBookmarkList, USHORT nType )
{
    DBG_ASSERT( !rpExchangeList, "ExchangeList muss NULL sein!");

    BOOL bListIdentical = TRUE; // BookmarkList und ExchangeList sind gleich
    BOOL bNameOK = TRUE;        // Name ist eindeutig

    rpExchangeList = new List();

    if( pBookmarkList )
    {
        String* pString = (String*) pBookmarkList->First();
        while( pString && bNameOK )
        {
            String* pNewName = new String( *pString );
            if( nType == 0  || nType == 2 )
                bNameOK = pDocSh->CheckPageName( pViewSh->GetWindow(), *pNewName );
            if( bNameOK && (nType == 1  || nType == 2) )
                bNameOK = pDocSh->CheckObjectName( pViewSh->GetWindow(), *pNewName );

            if( bListIdentical )
                bListIdentical = ( *pString == *pNewName );

            rpExchangeList->Insert( pNewName, LIST_APPEND );
            pString = (String*) pBookmarkList->Next();
        }
    }

    // ExchangeList ist mit BookmarkList identisch
    if( rpExchangeList && bListIdentical )
    {
        String* pString = (String*) rpExchangeList->First();
        while( pString )
        {
            delete pString;
            pString = (String*) rpExchangeList->Next();
        }
        delete rpExchangeList;
        rpExchangeList = NULL;
    }

    return( bNameOK );
}
