/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdview2.cxx,v $
 *
 *  $Revision: 1.55 $
 *
 *  last change: $Author: kz $ $Date: 2006-12-12 19:20:22 $
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


#include "View.hxx"

#include <vector>

#ifndef _COM_SUN_STAR_EMBED_XEMBEDPERSIST_HPP_
#include <com/sun/star/embed/XEmbedPersist.hpp>
#endif

#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif
#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVDETC_HXX //autogen
#include <svx/svdetc.hxx>
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
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
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
#ifndef _SVDITER_HXX
#include <svx/svditer.hxx>
#endif
#ifndef _E3D_OBJ3D_HXX
#include <svx/obj3d.hxx>
#endif
#ifndef _E3D_SCENE3D_HXX
#include <svx/scene3d.hxx>
#endif

#include <sfx2/objsh.hxx>
#include <svtools/embedtransfer.hxx>

#include "navigatr.hxx"
#include "anminfo.hxx"
#include "strings.hrc"
#include "glob.hrc"
#include "sdxfer.hxx"
#include "sdresid.hxx"
#include "sdmod.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#include "DrawDocShell.hxx"
#ifndef SD_FU_DRAW_HXX
#include "fudraw.hxx"
#endif
#include "drawdoc.hxx"
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "sdpage.hxx"
#include "unoaprms.hxx"
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#ifndef SD_SLIDE_VIEW_HXX
#include "SlideView.hxx"
#endif
#include "helpids.h"

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

namespace sd {

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
    ::sd::Window* mpTargetWindow;
    USHORT                  mnPage;
    USHORT                  mnLayer;

    SdNavigatorDropEvent (
        const ExecuteDropEvent& rEvt,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow,
        USHORT nPage,
        USHORT nLayer )
        : ExecuteDropEvent( rEvt ),
          mrTargetHelper( rTargetHelper ),
          mpTargetWindow( pTargetWindow ),
          mnPage( nPage ),
          mnLayer( nLayer )
    {}
};

// -----------------------------------------------------------------------------

::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > View::CreateClipboardDataObject( View*, ::Window& )
{
    // since SdTransferable::CopyToClipboard is called, this
    // dynamically created object ist destroyed automatically
    SdTransferable* pTransferable = new SdTransferable( mpDoc, NULL, FALSE );
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > xRet( pTransferable );

    SD_MOD()->pTransferClip = pTransferable;

    mpDoc->CreatingDataObj( pTransferable );
    pTransferable->SetWorkDocument( (SdDrawDocument*) GetAllMarkedModel() );
    mpDoc->CreatingDataObj( NULL );

    const Rectangle                 aMarkRect( GetAllMarkedRect() );
    TransferableObjectDescriptor    aObjDesc;
    String                          aDisplayName;
    SdrOle2Obj*                     pSdrOleObj = NULL;
    SdrPageView*                    pPgView = GetSdrPageView();
    SdPage*                         pOldPage = pPgView ? ( (SdPage*) pPgView->GetPage() ) : NULL;
    SdPage*                         pNewPage = (SdPage*) pTransferable->GetWorkDocument()->GetPage( 0 );

    if( pOldPage )
    {
        pNewPage->SetSize( pOldPage->GetSize() );
        pNewPage->SetLayoutName( pOldPage->GetLayoutName() );
    }

    if( GetMarkedObjectCount() == 1 )
    {
        SdrObject* pObj = GetMarkedObjectByIndex(0);

        if( pObj && pObj->ISA(SdrOle2Obj) && ((SdrOle2Obj*) pObj)->GetObjRef().is() )
        {
            // If object has no persistence it must be copied as part of the document
            try
            {
                uno::Reference< embed::XEmbedPersist > xPersObj( ((SdrOle2Obj*)pObj)->GetObjRef(), uno::UNO_QUERY );
                if ( xPersObj.is() && xPersObj->hasEntry() )
                     pSdrOleObj = (SdrOle2Obj*) pObj;
            }
            catch( uno::Exception& )
            {}
        }
    }

    if( pSdrOleObj )
        SvEmbedTransferHelper::FillTransferableObjectDescriptor( aObjDesc, pSdrOleObj->GetObjRef(), pSdrOleObj->GetGraphic(), pSdrOleObj->GetAspect() );
    else
        pTransferable->GetWorkDocument()->GetDocSh()->FillTransferableObjectDescriptor( aObjDesc );

    if( mpDocSh )
        aObjDesc.maDisplayName = mpDocSh->GetMedium()->GetURLObject().GetURLNoPass();

    aObjDesc.maSize = aMarkRect.GetSize();

    pTransferable->SetStartPos( aMarkRect.TopLeft() );
    pTransferable->SetObjectDescriptor( aObjDesc );
    pTransferable->CopyToClipboard( mpViewSh->GetActiveWindow() );

    return xRet;
}

// -----------------------------------------------------------------------------

::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > View::CreateDragDataObject( View* pWorkView, ::Window& rWindow, const Point& rDragPos )
{
    SdTransferable* pTransferable = new SdTransferable( mpDoc, pWorkView, FALSE );
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > xRet( pTransferable );

    SD_MOD()->pTransferDrag = pTransferable;

    TransferableObjectDescriptor    aObjDesc;
    String                          aDisplayName;
    SdrOle2Obj*                     pSdrOleObj = NULL;

    if( GetMarkedObjectCount() == 1 )
    {
        SdrObject* pObj = GetMarkedObjectByIndex( 0 );

        if( pObj && pObj->ISA( SdrOle2Obj ) && ( (SdrOle2Obj*) pObj )->GetObjRef().is() )
        {
            // If object has no persistence it must be copied as part of the document
            try
            {
                uno::Reference< embed::XEmbedPersist > xPersObj( ((SdrOle2Obj*)pObj)->GetObjRef(), uno::UNO_QUERY );
                if ( xPersObj.is() && xPersObj->hasEntry() )
                     pSdrOleObj = (SdrOle2Obj*) pObj;
            }
            catch( uno::Exception& )
            {}
        }
    }

    if( mpDocSh )
        aDisplayName = mpDocSh->GetMedium()->GetURLObject().GetURLNoPass();

    if( pSdrOleObj )
        SvEmbedTransferHelper::FillTransferableObjectDescriptor( aObjDesc, pSdrOleObj->GetObjRef(), pSdrOleObj->GetGraphic(), pSdrOleObj->GetAspect() );
    else
        mpDocSh->FillTransferableObjectDescriptor( aObjDesc );

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

::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > View::CreateSelectionDataObject( View* pWorkView, ::Window& rWindow )
{
    SdTransferable*                 pTransferable = new SdTransferable( mpDoc, pWorkView, TRUE );
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > xRet( pTransferable );
    TransferableObjectDescriptor    aObjDesc;
    const Rectangle                 aMarkRect( GetAllMarkedRect() );
    String                          aDisplayName;

    SD_MOD()->pTransferSelection = pTransferable;

    if( mpDocSh )
    {
        aDisplayName = mpDocSh->GetMedium()->GetURLObject().GetURLNoPass();
        mpDocSh->FillTransferableObjectDescriptor( aObjDesc );
    }

    aObjDesc.maSize = aMarkRect.GetSize();

    pTransferable->SetStartPos( aMarkRect.TopLeft() );
    pTransferable->SetObjectDescriptor( aObjDesc );
    pTransferable->CopyToSelection( &rWindow );

    return xRet;
}

// -----------------------------------------------------------------------------

void View::UpdateSelectionClipboard( BOOL bForceDeselect )
{
    if( mpViewSh && mpViewSh->GetActiveWindow() )
    {
        if( !bForceDeselect && GetMarkedObjectList().GetMarkCount() )
            CreateSelectionDataObject( this, *mpViewSh->GetActiveWindow() );
        else if( SD_MOD()->pTransferSelection && ( SD_MOD()->pTransferSelection->GetView() == this ) )
        {
            TransferableHelper::ClearSelection( mpViewSh->GetActiveWindow() );
            SD_MOD()->pTransferSelection = NULL;
        }
    }
}

// -----------------------------------------------------------------------------

void View::DoCut(::Window* )
{
    const OutlinerView* pOLV = GetTextEditOutlinerView();

    if( pOLV )
        ( (OutlinerView*) pOLV)->Cut();
    else if( AreObjectsMarked() )
    {
        String aStr( SdResId(STR_UNDO_CUT) );

        DoCopy();
        BegUndo( ( aStr += sal_Unicode(' ') ) += GetDescriptionOfMarkedObjects() );
        DeleteMarked();
        EndUndo();
    }
}

// -----------------------------------------------------------------------------

void View::DoCopy (::Window* pWindow)
{
    const OutlinerView* pOLV = GetTextEditOutlinerView();

    if( pOLV )
        ( (OutlinerView*) pOLV)->Copy();
    else if( AreObjectsMarked() )
    {
        BrkAction();
        CreateClipboardDataObject( this, *pWindow );
    }
}

// -----------------------------------------------------------------------------

void View::DoPaste (::Window* pWindow)
{
    const OutlinerView* pOLV = GetTextEditOutlinerView();

    if( pOLV )
    {
        ( (OutlinerView*) pOLV)->PasteSpecial();

        SdrObject*  pObj = GetTextEditObject();
        SdPage*     pPage = (SdPage*)( pObj ? pObj->GetPage() : NULL );
        ::Outliner* pOutliner = pOLV->GetOutliner();

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
                        const USHORT nParaLen = (USHORT)rEdit.GetTextLen( (USHORT)nPara );
                        pOutliner->QuickDelete( ESelection( (USHORT)nPara, nParaLen, (USHORT)nPara+1, 0 ) );
                        pOutliner->QuickInsertLineBreak( ESelection( (USHORT)nPara, nParaLen, (USHORT)nPara, nParaLen ) );
                    }

                    DBG_ASSERT( rEdit.GetParagraphCount() <= 1, "Titelobjekt contains hard line breaks" );
                    pOutliner->SetUpdateMode(bOldUpdateMode);
                }
            }

            if( !mpDoc->IsChanged() )
            {
                if( pOutliner && pOutliner->IsModified() )
                    mpDoc->SetChanged( TRUE );
            }
        }
    }
    else
    {
        TransferableDataHelper aDataHelper( TransferableDataHelper::CreateFromSystemClipboard( mpViewSh->GetActiveWindow() ) );

        if( aDataHelper.GetTransferable().is() )
        {
            Point       aPos;
            sal_Int8    nDnDAction = DND_ACTION_COPY;

            if( pWindow )
                aPos = pWindow->PixelToLogic( Rectangle( aPos, pWindow->GetOutputSizePixel() ).Center() );

            DrawViewShell* pDrViewSh = (DrawViewShell*) mpDocSh->GetViewShell();

            if (pDrViewSh != NULL)
            {
                if( !InsertData( aDataHelper, aPos, nDnDAction, FALSE ) )
                {
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

void View::StartDrag( const Point& rStartPos, ::Window* pWindow )
{
    if( AreObjectsMarked() && IsAction() && mpViewSh && pWindow && !mpDragSrcMarkList )
    {
        BrkAction();

        if( IsTextEdit() )
            SdrEndTextEdit();

        DrawViewShell* pDrawViewShell = dynamic_cast< DrawViewShell* >( mpDocSh ? mpDocSh->GetViewShell() : 0 );

        if( pDrawViewShell )
        {
            FunctionReference xFunction( pDrawViewShell->GetCurrentFunction() );

            if( xFunction.is() && xFunction->ISA( FuDraw ) )
                static_cast<FuDraw*>(xFunction.get())->ForcePointer( NULL );
        }

        mpDragSrcMarkList = new SdrMarkList(GetMarkedObjectList());
        mnDragSrcPgNum = GetSdrPageView()->GetPage()->GetPageNum();

        String aStr( SdResId(STR_UNDO_DRAGDROP) );
        aStr += sal_Unicode(' ');
        aStr += mpDragSrcMarkList->GetMarkDescription();
        BegUndo(aStr);
        CreateDragDataObject( this, *pWindow, rStartPos );
    }
}

// -----------------------------------------------------------------------------

void View::DragFinished( sal_Int8 nDropAction )
{
    SdTransferable* pDragTransferable = SD_MOD()->pTransferDrag;

    if( pDragTransferable )
        pDragTransferable->SetView( NULL );

    if( ( nDropAction & DND_ACTION_MOVE ) &&
        pDragTransferable && !pDragTransferable->IsInternalMove() &&
        mpDragSrcMarkList && mpDragSrcMarkList->GetMarkCount() &&
        !IsPresObjSelected() )
    {
        mpDragSrcMarkList->ForceSort();
        BegUndo();

        ULONG nm, nAnz = mpDragSrcMarkList->GetMarkCount();

         for( nm = nAnz; nm>0; )
        {
            nm--;
            SdrMark* pM=mpDragSrcMarkList->GetMark(nm);
            AddUndo(mpDoc->GetSdrUndoFactory().CreateUndoDeleteObject(*pM->GetMarkedSdrObj()));
        }

        mpDragSrcMarkList->GetMark(0)->GetMarkedSdrObj()->GetOrdNum();

        for (nm=nAnz; nm>0;)
        {
            nm--;
            SdrMark* pM=mpDragSrcMarkList->GetMark(nm);
            SdrObject* pObj=pM->GetMarkedSdrObj();
            UINT32 nOrdNum=pObj->GetOrdNumDirect();

            if( pObj && pObj->GetPage() )
            {
#ifdef DBG_UTIL
                SdrObject* pChkObj =
#endif
                    pObj->GetPage()->RemoveObject(nOrdNum);
                DBG_ASSERT(pChkObj==pObj,"pChkObj!=pObj beim RemoveObject()");
            }
        }

        EndUndo();
    }

    if( pDragTransferable )
        pDragTransferable->SetInternalMove( FALSE );

    EndUndo();
    mnDragSrcPgNum = SDRPAGE_NOTFOUND;
    delete mpDragSrcMarkList;
    mpDragSrcMarkList = NULL;
}

// -----------------------------------------------------------------------------

sal_Int8 View::AcceptDrop( const AcceptDropEvent& rEvt, DropTargetHelper& rTargetHelper,
                             ::sd::Window*, USHORT, USHORT nLayer )
{
    String          aLayerName( GetActiveLayer() );
    SdrPageView*    pPV = GetSdrPageView();
    sal_Int8        nDropAction = rEvt.mnAction;
    sal_Int8        nRet = DND_ACTION_NONE;

    if( nLayer != SDRLAYER_NOTFOUND )
    {
        SdrLayerAdmin& rLayerAdmin = mpDoc->GetLayerAdmin();
        aLayerName = rLayerAdmin.GetLayerPerID(nLayer)->GetName();
    }

    if( mbIsDropAllowed && !pPV->IsLayerLocked( aLayerName ) && pPV->IsLayerVisible( aLayerName ) )
    {
        const OutlinerView* pOLV = GetTextEditOutlinerView();
        BOOL                bIsInsideOutlinerView = FALSE;

        if( pOLV )
        {
            Rectangle aRect( pOLV->GetOutputArea() );

            if (GetMarkedObjectCount() == 1)
            {
                SdrMark* pMark = GetSdrMarkByIndex(0);
                SdrObject* pObj = pMark->GetMarkedSdrObj();
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
                const View* pSourceView = pDragTransferable->GetView();

                if( pDragTransferable->IsPageTransferable() )
                {
                    nRet = DND_ACTION_COPY;
                }
                else if( pSourceView )
                {
                    if( !( nDropAction & DND_ACTION_LINK ) ||
                        pSourceView->GetDocSh()->GetMedium()->GetName().Len() )
                    {
                        nRet = nDropAction;
                    }
                }
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
                            if(pIAOHandle->getOverlayObjectList().isHitPixel(rEvt.maPosPixel))
                            {
                                nRet = nDropAction;
                                static_cast< SdrHdlColor* >( pIAOHandle )->SetSize( SDR_HANDLE_COLOR_SIZE_SELECTED );
                            }
                            else
                            {
                                static_cast< SdrHdlColor* >( pIAOHandle )->SetSize( SDR_HANDLE_COLOR_SIZE_NORMAL );
                            }

                            //OLMconst B2dIAOGroup& rIAOGroup = pIAOHandle->GetIAOGroup();
                            //OLMif( rIAOGroup.IsHit( rEvt.maPosPixel ) )
                            //OLM{
                            //OLM    nRet = nDropAction;
                            //OLM    static_cast< SdrHdlColor* >( pIAOHandle )->SetSize( SDR_HANDLE_COLOR_SIZE_SELECTED );
                            //OLM}
                            //OLMelse
                            //OLM    static_cast< SdrHdlColor* >( pIAOHandle )->SetSize( SDR_HANDLE_COLOR_SIZE_NORMAL );
                        }
                    }

                    //OLMRefreshAllIAOManagers();
                }

                // check object insert
                if( !nRet && ( bXFillExchange || ( ( bDrawing || bGraphic || bMtf || bBitmap || bBookmark ) && ( nDropAction & DND_ACTION_LINK ) ) ) )
                {
                    SdrObject*      pPickObj = NULL;
                    SdrPageView*    pPageView = NULL;
                    ::sd::Window* pWindow = mpViewSh->GetActiveWindow();
                    Point           aPos( pWindow->PixelToLogic( rEvt.maPosPixel ) );
                    const BOOL      bHasPickObj = PickObj( aPos, pPickObj, pPageView );
                    BOOL            bIsPresTarget = FALSE;

                    if( bHasPickObj && pPickObj && ( pPickObj->IsEmptyPresObj() || pPickObj->GetUserCall() ) )
                    {
                        SdPage* pPage = (SdPage*) pPickObj->GetPage();

                        if( pPage && pPage->IsMasterPage() )
                            bIsPresTarget = pPage->IsPresObj( pPickObj );
                    }

                    if( bHasPickObj && !bIsPresTarget &&
                        ( !pPickObj->ISA( SdrGrafObj ) || bGraphic || bMtf || bBitmap || ( bXFillExchange && !pPickObj->ISA( SdrGrafObj ) && !pPickObj->ISA( SdrOle2Obj ) ) ) )
                    {
                        if( mpDropMarkerObj != pPickObj )
                        {
                            mpDropMarkerObj = pPickObj;
                            ImplClearDrawDropMarker();

                            if(mpDropMarkerObj)
                            {
                                mpDropMarker = new SdrDropMarkerOverlay(*this, *mpDropMarkerObj);
                            }
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

                    if( mpDropMarker )
                    {
                        ImplClearDrawDropMarker();
                        mpDropMarkerObj = NULL;
                    }

                    if( bBookmark && bFile && ( nDropAction & DND_ACTION_MOVE ) && ( !mpViewSh || mpViewSh->GetSlideShow() ) )
                        bBookmark = FALSE;

                    if( bDrawing || bGraphic || bMtf || bBitmap || bBookmark || bFile || bXFillExchange || bSBAFormat || bEditEngine || bString || bRTF )
                        nRet = nDropAction;
                }
            }
        }
    }

    // destroy drop marker if this is a leaving event
    if( rEvt.mbLeaving && mpDropMarker )
    {
        ImplClearDrawDropMarker();
        mpDropMarkerObj = NULL;
    }

    return nRet;
}

// -----------------------------------------------------------------------------

sal_Int8 View::ExecuteDrop( const ExecuteDropEvent& rEvt, DropTargetHelper& rTargetHelper,
                              ::sd::Window* pTargetWindow, USHORT nPage, USHORT nLayer )
{
    SdrPageView*    pPV = GetSdrPageView();
    String          aActiveLayer = GetActiveLayer();
    sal_Int8        nDropAction = rEvt.mnAction;
    sal_Int8        nRet = DND_ACTION_NONE;

    // destroy drop marker if it is shown
    if( mpDropMarker )
    {
        ImplClearDrawDropMarker();
        mpDropMarkerObj = NULL;
    }

    if( !pPV->IsLayerLocked( aActiveLayer ) )
    {
        const OutlinerView* pOLV = GetTextEditOutlinerView();
        BOOL                bIsInsideOutlinerView = FALSE;

        if( pOLV )
        {
            Rectangle aRect( pOLV->GetOutputArea() );

            if( GetMarkedObjectCount() == 1 )
            {
                SdrMark* pMark = GetSdrMarkByIndex(0);
                SdrObject* pObj = pMark->GetMarkedSdrObj();
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
                        if(pIAOHandle->getOverlayObjectList().isHitPixel(rEvt.maPosPixel))
                        {
                            SotStorageStreamRef xStm;

                            if( aDataHelper.GetSotStorageStream( SOT_FORMATSTR_ID_XFA, xStm ) && xStm.Is() )
                            {
                                XFillExchangeData aFillData( XFillAttrSetItem( &mpDoc->GetPool() ) );

                                *xStm >> aFillData;
                                const Color aColor( ( (XFillColorItem&) aFillData.GetXFillAttrSetItem()->GetItemSet().Get( XATTR_FILLCOLOR ) ).GetColorValue() );
                                static_cast< SdrHdlColor* >( pIAOHandle )->SetColor( aColor, TRUE );
                                nRet = nDropAction;
                            }
                        }

                        //OLMconst B2dIAOGroup& rIAOGroup = pIAOHandle->GetIAOGroup();
                        //OLMif( rIAOGroup.IsHit( rEvt.maPosPixel ) )
                        //OLM{
                        //OLM   SotStorageStreamRef xStm;
                        //OLM   if( aDataHelper.GetSotStorageStream( SOT_FORMATSTR_ID_XFA, xStm ) && xStm.Is() )
                        //OLM    {
                        //OLM        XFillExchangeData aFillData( XFillAttrSetItem( &pDoc->GetPool() ) );
                        //OLM       *xStm >> aFillData;
                        //OLM        const Color aColor( ( (XFillColorItem&) aFillData.GetXFillAttrSetItem()->GetItemSet().Get( XATTR_FILLCOLOR ) ).GetValue() );
                        //OLM       static_cast< SdrHdlColor* >( pIAOHandle )->SetColor( aColor, TRUE );
                        //OLM        nRet = nDropAction;
                        //OLM    }
                        //OLM}
                    }
                }
            }

            // standard insert?
            if( !nRet && InsertData( aDataHelper, aPos, nDropAction, TRUE, 0, nPage, nLayer ) )
                nRet = nDropAction;

            // special insert?
            if( !nRet && mpViewSh )
            {
                String          aTmpString1, aTmpString2;
                INetBookmark    aINetBookmark( aTmpString1, aTmpString2 );

                // insert bookmark
                if( aDataHelper.HasFormat( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK ) &&
                    aDataHelper.GetINetBookmark( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK, aINetBookmark ) )
                {
                    SdPageObjsTLB::SdPageObjsTransferable* pPageObjsTransferable = SdPageObjsTLB::SdPageObjsTransferable::getImplementation( aDataHelper.GetXTransferable() );

                    if( pPageObjsTransferable &&
                        ( NAVIGATOR_DRAGTYPE_LINK == pPageObjsTransferable->GetDragType() ||
                          NAVIGATOR_DRAGTYPE_EMBEDDED == pPageObjsTransferable->GetDragType() ) )
                    {
                        // insert bookmark from own navigator (handled async. due to possible message box )
                        Application::PostUserEvent( LINK( this, View, ExecuteNavigatorDrop ),
                                                    new SdNavigatorDropEvent( rEvt, rTargetHelper, pTargetWindow,
                                                                              nPage, nLayer ) );
                        nRet = nDropAction;
                    }
                    else
                    {
                        SdrObject*      pPickObj = NULL;
                        SdrPageView*    pPageView = NULL;

                        if( PickObj( aPos, pPickObj, pPageView ) )
                        {
                            // insert as clip action => jump
                            rtl::OUString       aBookmark( aINetBookmark.GetURL() );
                            SdAnimationInfo*    pInfo = mpDoc->GetAnimationInfo( pPickObj );
                            BOOL                bCreated = FALSE;

                            if( aBookmark.getLength() )
                            {
                                presentation::ClickAction eClickAction = presentation::ClickAction_DOCUMENT;

                                sal_Int32 nIndex = aBookmark.indexOf( (sal_Unicode)'#' );
                                if( nIndex != -1 )
                                {
                                    const String aDocName( aBookmark.copy( 0, nIndex ) );

                                    if( mpDocSh->GetMedium()->GetName() == aDocName || mpDocSh->GetName() == aDocName )
                                    {
                                        // internal jump, only use the part after and including '#'
                                        eClickAction = presentation::ClickAction_BOOKMARK;
                                        aBookmark = aBookmark.copy( nIndex+1 );
                                    }
                                }

                                if( !pInfo )
                                {
                                    pInfo = SdDrawDocument::GetShapeUserData( *pPickObj, true );
                                    bCreated = TRUE;
                                }

                                // Undo-Action mit alten und neuen Groessen erzeugen
                                SdAnimationPrmsUndoAction* pAction = new SdAnimationPrmsUndoAction(mpDoc, pPickObj, bCreated);
                                pAction->SetActive(pInfo->mbActive, pInfo->mbActive);
                                pAction->SetEffect(pInfo->meEffect, pInfo->meEffect);
                                pAction->SetTextEffect(pInfo->meTextEffect, pInfo->meTextEffect);
                                pAction->SetSpeed(pInfo->meSpeed, pInfo->meSpeed);
                                pAction->SetDim(pInfo->mbDimPrevious, pInfo->mbDimPrevious);
                                pAction->SetDimColor(pInfo->maDimColor, pInfo->maDimColor);
                                pAction->SetDimHide(pInfo->mbDimHide, pInfo->mbDimHide);
                                pAction->SetSoundOn(pInfo->mbSoundOn, pInfo->mbSoundOn);
                                pAction->SetSound(pInfo->maSoundFile, pInfo->maSoundFile);
                                pAction->SetPlayFull(pInfo->mbPlayFull, pInfo->mbPlayFull);
                                pAction->SetPathObj(pInfo->mpPathObj, pInfo->mpPathObj);
                                pAction->SetClickAction(pInfo->meClickAction, eClickAction);
                                pAction->SetBookmark(pInfo->maBookmark, aBookmark);
//                              pAction->SetInvisibleInPres(pInfo->mbInvisibleInPresentation, TRUE);
                                pAction->SetVerb(pInfo->mnVerb, pInfo->mnVerb);
                                pAction->SetSecondEffect(pInfo->meSecondEffect, pInfo->meSecondEffect);
                                pAction->SetSecondSpeed(pInfo->meSecondSpeed, pInfo->meSecondSpeed);
                                pAction->SetSecondSoundOn(pInfo->mbSecondSoundOn, pInfo->mbSecondSoundOn);
                                pAction->SetSecondPlayFull(pInfo->mbSecondPlayFull, pInfo->mbSecondPlayFull);

                                String aString(SdResId(STR_UNDO_ANIMATION));
                                pAction->SetComment(aString);
                                mpDocSh->GetUndoManager()->AddUndoAction(pAction);
                                pInfo->meClickAction = eClickAction;
                                pInfo->maBookmark = aBookmark;
                                mpDoc->SetChanged();

                                nRet = nDropAction;
                            }
                        }
                        else if( mpViewSh->ISA( DrawViewShell ) )
                        {
                            // insert as normal URL button
                            ( (DrawViewShell*) mpViewSh )->InsertURLButton( aINetBookmark.GetURL(), aINetBookmark.GetDescription(), String(), &aPos );
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

IMPL_LINK( View, ExecuteNavigatorDrop, SdNavigatorDropEvent*, pSdNavigatorDropEvent )
{
    TransferableDataHelper                  aDataHelper( pSdNavigatorDropEvent->maDropEvent.Transferable );
    SdPageObjsTLB::SdPageObjsTransferable*  pPageObjsTransferable = SdPageObjsTLB::SdPageObjsTransferable::getImplementation( aDataHelper.GetXTransferable() );
    INetBookmark                            aINetBookmark;

    if( pPageObjsTransferable && aDataHelper.GetINetBookmark( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK, aINetBookmark ) )
    {
        Point   aPos;
        List    aBookmarkList;
        String  aBookmark;
        SdPage* pPage = (SdPage*) GetSdrPageView()->GetPage();
        USHORT  nPgPos = 0xFFFF;

        if( pSdNavigatorDropEvent->mpTargetWindow )
            aPos = pSdNavigatorDropEvent->mpTargetWindow->PixelToLogic( pSdNavigatorDropEvent->maPosPixel );

        const rtl::OUString aURL( aINetBookmark.GetURL() );
        sal_Int32 nIndex = aURL.indexOf( (sal_Unicode)'#' );
        if( nIndex != -1 )
            aBookmark = aURL.copy( nIndex+1 );
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
        BOOL    bLink = ( NAVIGATOR_DRAGTYPE_LINK == pPageObjsTransferable->GetDragType()  ? TRUE : FALSE );
        BOOL    bNameOK = GetExchangeList( pExchangeList, &aBookmarkList, 2 );
        BOOL    bReplace = FALSE;

        // Da man hier nicht weiss, ob es sich um eine Seite oder ein Objekt handelt,
        // wird eine Liste sowohl mit Seiten, als auch mit Objekten gefuellt.
        // Sollten Seitennamen und Objektnamen identisch sein gibt es hier natuerlich Probleme !!!
        if( bNameOK )
        {
            mpDoc->InsertBookmark( &aBookmarkList, pExchangeList,
                                  bLink, bReplace, nPgPos, FALSE,
                                  &pPageObjsTransferable->GetDocShell(),
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

BOOL View::GetExchangeList( List*& rpExchangeList, List* pBookmarkList, USHORT nType )
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
                bNameOK = mpDocSh->CheckPageName (
                    mpViewSh->GetActiveWindow(), *pNewName);

            if( bNameOK && ( nType == 1  || nType == 2 ) )
            {
                if( mpDoc->GetObj( *pNewName ) )
                {
                    String          aTitle( SdResId( STR_TITLE_NAMEGROUP ) );
                    String          aDesc( SdResId( STR_DESC_NAMEGROUP ) );
                    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                    AbstractSvxNameDialog* pDlg = pFact ? pFact->CreateSvxNameDialog( mpViewSh->GetActiveWindow(), *pNewName, aDesc, ResId(RID_SVXDLG_NAME) ) : 0;
                    if( pDlg )
                    {
                        pDlg->SetEditHelpId( HID_SD_NAMEDIALOG_OBJECT );

                        bNameOK = FALSE;
                        pDlg->SetText( aTitle );

                        while( !bNameOK && pDlg->Execute() == RET_OK )
                        {
                            pDlg->GetName( *pNewName );

                            if( !mpDoc->GetObj( *pNewName ) )
                                bNameOK = TRUE;
                        }

                        delete pDlg;
                    }
                }
            }

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

typedef std::vector< std::pair< sal_uInt32, sal_uInt32 > > PathSurrogateVector;
typedef std::vector< SdrObject* > SdrObjectVector;

void ImplProcessObjectList(SdrObject* pObj, SdrObjectVector& rVector )
{
    sal_Bool bIsGroup(pObj->IsGroupObject());
    if(bIsGroup && pObj->ISA(E3dObject) && !pObj->ISA(E3dScene))
        bIsGroup = sal_False;

    rVector.push_back( pObj );

    if(bIsGroup)
    {
        SdrObjList* pObjList = pObj->GetSubList();
        sal_uInt32 a;
        for( a = 0; a < pObjList->GetObjCount(); a++)
            ImplProcessObjectList(pObjList->GetObj(a), rVector);
    }
}

SdrModel* View::GetMarkedObjModel() const
{
    return FmFormView::GetMarkedObjModel();;
}

BOOL View::Paste(const SdrModel& rMod, const Point& rPos, SdrObjList* pLst /* =NULL */, UINT32 nOptions /* =0 */)
{
    return FmFormView::Paste( rMod, rPos, pLst,nOptions );;
}

} // end of namespace sd
