/*************************************************************************
 *
 *  $RCSfile: drwtrans.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: hr $ $Date: 2004-08-03 13:27:53 $
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

// INCLUDE ---------------------------------------------------------------

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/form/FormButtonType.hpp>

#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif

#include <svx/unomodel.hxx>

#include <sot/storage.hxx>
#include <so3/svstor.hxx>
#include <vcl/virdev.hxx>
#include <svx/fmglob.hxx>
#include <svx/svditer.hxx>
#include <svx/svdograf.hxx>
#include <svx/svdoole2.hxx>
#include <svx/svdouno.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdxcgv.hxx>
#include <sfx2/docfile.hxx>
#include <svtools/itempool.hxx>
#include <svtools/urlbmk.hxx>
#include <tools/urlobj.hxx>
#include <vos/mutex.hxx>

#include "drwtrans.hxx"
#include "docsh.hxx"
#include "drwlayer.hxx"
#include "drawview.hxx"
#include "viewdata.hxx"
#include "scmod.hxx"

// #108584#
#include "scitems.hxx"

// #108584#
#ifndef _EEITEM_HXX
#include <svx/eeitem.hxx>
#endif

// #108584#
#ifndef _SVX_FHGTITEM_HXX
#include <svx/fhgtitem.hxx>
#endif

using namespace com::sun::star;

// -----------------------------------------------------------------------

#define SCDRAWTRANS_TYPE_EMBOBJ         1
#define SCDRAWTRANS_TYPE_DRAWMODEL      2

// -----------------------------------------------------------------------

// -----------------------------------------------------------------------

ScDrawTransferObj::ScDrawTransferObj( SdrModel* pClipModel, ScDocShell* pContainerShell,
                                        const TransferableObjectDescriptor& rDesc ) :
    aObjDesc( rDesc ),
    pModel( pClipModel ),
    pBookmark( NULL ),
    bGraphic( FALSE ),
    bGrIsBit( FALSE ),
    bOleObj( FALSE ),
    pDragSourceView( NULL ),
    nDragSourceFlags( 0 ),
    bDragWasInternal( FALSE ),
    nSourceDocID( 0 )
{
    //
    //  check what kind of objects are contained
    //

    SdrPage* pPage = pModel->GetPage(0);
    if (pPage)
    {
        SdrObjListIter aIter( *pPage, IM_FLAT );
        SdrObject* pObject = aIter.Next();
        if (pObject && !aIter.Next())               // exactly one object?
        {
            //
            //  OLE object
            //

            UINT16 nSdrObjKind = pObject->GetObjIdentifier();
            if (nSdrObjKind == OBJ_OLE2)
            {
                bOleObj = TRUE;
                // aOleData is initialized later
            }

            //
            //  Graphic object
            //

            if (nSdrObjKind == OBJ_GRAF)
            {
                bGraphic = TRUE;
                if ( ((SdrGrafObj*)pObject)->GetGraphic().GetType() == GRAPHIC_BITMAP )
                    bGrIsBit = TRUE;
            }

            //
            //  URL button
            //

            SdrUnoObj* pUnoCtrl = PTR_CAST(SdrUnoObj, pObject);
            if (pUnoCtrl && FmFormInventor == pUnoCtrl->GetObjInventor())
            {
                uno::Reference<awt::XControlModel> xControlModel = pUnoCtrl->GetUnoControlModel();
                DBG_ASSERT( xControlModel.is(), "uno control without model" );
                if ( xControlModel.is() )
                {
                    uno::Reference< beans::XPropertySet > xPropSet( xControlModel, uno::UNO_QUERY );
                    uno::Reference< beans::XPropertySetInfo > xInfo = xPropSet->getPropertySetInfo();

                    rtl::OUString sPropButtonType = rtl::OUString::createFromAscii( "ButtonType" );
                    rtl::OUString sPropTargetURL  = rtl::OUString::createFromAscii( "TargetURL" );
                    rtl::OUString sPropLabel      = rtl::OUString::createFromAscii( "Label" );

                    if(xInfo->hasPropertyByName( sPropButtonType ))
                    {
                        uno::Any aAny = xPropSet->getPropertyValue( sPropButtonType );
                        form::FormButtonType eTmp;
                        if ( (aAny >>= eTmp) && eTmp == form::FormButtonType_URL )
                        {
                            // URL
                            if(xInfo->hasPropertyByName( sPropTargetURL ))
                            {
                                aAny = xPropSet->getPropertyValue( sPropTargetURL );
                                rtl::OUString sTmp;
                                if ( (aAny >>= sTmp) && sTmp.getLength() )
                                {
                                    String aUrl = sTmp;
                                    String aAbs;
                                    const SfxMedium* pMedium;
                                    if (pContainerShell && (pMedium = pContainerShell->GetMedium()))
                                    {
                                        bool bWasAbs = true;
                                        aAbs = pMedium->GetURLObject().smartRel2Abs( aUrl, bWasAbs ).
                                                    GetMainURL(INetURLObject::NO_DECODE);
                                        // full path as stored INetBookmark must be encoded
                                    }
                                    else
                                        aAbs = aUrl;

                                    // Label
                                    String aLabel;
                                    if(xInfo->hasPropertyByName( sPropLabel ))
                                    {
                                        aAny = xPropSet->getPropertyValue( sPropLabel );
                                        if ( (aAny >>= sTmp) && sTmp.getLength() )
                                        {
                                            aLabel = String(sTmp);
                                        }
                                    }
                                    pBookmark = new INetBookmark( aAbs, aLabel );
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //
    //  get size for object descriptor
    //

    SdrExchangeView aView(pModel);
    Point aPos;
    SdrPageView* pPv = aView.ShowPagePgNum(0,aPos);
    aView.MarkAll(pPv);
    aSrcSize = aView.GetAllMarkedRect().GetSize();
    aObjDesc.maSize = aSrcSize;

    //
    // remember a unique ID of the source document
    //
    if ( pContainerShell )
    {
        const ScDocument* pDoc = pContainerShell->GetDocument();
        if ( pDoc )
            nSourceDocID = pDoc->GetDocumentID();
    }
}

ScDrawTransferObj::~ScDrawTransferObj()
{
    Application::GetSolarMutex().acquire();     //! ???

    ScModule* pScMod = SC_MOD();
    if ( pScMod->GetClipData().pDrawClipboard == this )
    {
        DBG_ERROR("ScDrawTransferObj wasn't released");
        pScMod->SetClipObject( NULL, NULL );
    }
    if ( pScMod->GetDragData().pDrawTransfer == this )
    {
        DBG_ERROR("ScDrawTransferObj wasn't released");
        pScMod->ResetDragObject();
    }

    aOleData = TransferableDataHelper();        // clear before releasing the mutex
    aDocShellRef.Clear();

    delete pModel;
    aDrawPersistRef.Clear();                    // after the model

    delete pBookmark;
    delete pDragSourceView;

    Application::GetSolarMutex().release();     //! ???
}

// static
ScDrawTransferObj* ScDrawTransferObj::GetOwnClipboard( Window* )
{
    ScDrawTransferObj* pObj = SC_MOD()->GetClipData().pDrawClipboard;
    return pObj;
}

BOOL lcl_HasOnlyControls( SdrModel* pModel )
{
    BOOL bOnlyControls = FALSE;         // default if there are no objects

    if ( pModel )
    {
        SdrPage* pPage = pModel->GetPage(0);
        if (pPage)
        {
            SdrObjListIter aIter( *pPage, IM_DEEPNOGROUPS );
            SdrObject* pObj = aIter.Next();
            if ( pObj )
            {
                bOnlyControls = TRUE;   // only set if there are any objects at all
                while ( pObj )
                {
                    if (!pObj->ISA(SdrUnoObj))
                    {
                        bOnlyControls = FALSE;
                        break;
                    }
                    pObj = aIter.Next();
                }
            }
        }
    }

    return bOnlyControls;
}

void ScDrawTransferObj::AddSupportedFormats()
{
    if ( bGrIsBit )             // single bitmap graphic
    {
        AddFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR );
        AddFormat( SOT_FORMATSTR_ID_SVXB );
        AddFormat( SOT_FORMAT_BITMAP );
        AddFormat( SOT_FORMAT_GDIMETAFILE );
    }
    else if ( bGraphic )        // other graphic
    {
        // #i25616#
        AddFormat( SOT_FORMATSTR_ID_DRAWING );

        AddFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR );
        AddFormat( SOT_FORMATSTR_ID_SVXB );
        AddFormat( SOT_FORMAT_GDIMETAFILE );
        AddFormat( SOT_FORMAT_BITMAP );
    }
    else if ( pBookmark )       // url button
    {
//      AddFormat( SOT_FORMATSTR_ID_EMBED_SOURCE );
        AddFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR );
        AddFormat( SOT_FORMATSTR_ID_SOLK );
        AddFormat( SOT_FORMAT_STRING );
        AddFormat( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR );
        AddFormat( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK );
        AddFormat( SOT_FORMATSTR_ID_DRAWING );
    }
    else if ( bOleObj )         // single OLE object
    {
        AddFormat( SOT_FORMATSTR_ID_EMBED_SOURCE );
        AddFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR );
        AddFormat( SOT_FORMAT_GDIMETAFILE );

        if ( !aOleData.GetTransferable().is() )
        {
            SvInPlaceObjectRef xIPObj = GetSingleObject();
            if ( xIPObj.Is() )
                aOleData = TransferableDataHelper( xIPObj->CreateTransferableSnapshot() );
        }
        if ( aOleData.GetTransferable().is() )
        {
            //  get format list from object snapshot
            //  (this must be after inserting the default formats!)

            DataFlavorExVector              aVector( aOleData.GetDataFlavorExVector() );
            DataFlavorExVector::iterator    aIter( aVector.begin() ), aEnd( aVector.end() );

            while( aIter != aEnd )
                AddFormat( *aIter++ );
        }
    }
    else                        // any drawing objects
    {
        AddFormat( SOT_FORMATSTR_ID_EMBED_SOURCE );
        AddFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR );
        AddFormat( SOT_FORMATSTR_ID_DRAWING );

        // #103556# leave out bitmap and metafile if there are only controls
        if ( !lcl_HasOnlyControls( pModel ) )
        {
            AddFormat( SOT_FORMAT_BITMAP );
            AddFormat( SOT_FORMAT_GDIMETAFILE );
        }
    }

//  if( pImageMap )
//      AddFormat( SOT_FORMATSTR_ID_SVIM );
}

sal_Bool ScDrawTransferObj::GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor )
{
    sal_Bool bOK = sal_False;

    if ( bOleObj )
    {
        if ( !aOleData.GetTransferable().is() )
        {
            SvInPlaceObjectRef xIPObj = GetSingleObject();
            if ( xIPObj.Is() )
                aOleData = TransferableDataHelper( xIPObj->CreateTransferableSnapshot() );
        }

        if( aOleData.GetTransferable().is() && aOleData.HasFormat( rFlavor ) )
        {
            ULONG nOldSwapMode;

            if( pModel )
            {
                nOldSwapMode = pModel->GetSwapGraphicsMode();
                pModel->SetSwapGraphicsMode( SDR_SWAPGRAPHICSMODE_PURGE );
            }

            bOK = SetAny( aOleData.GetAny( rFlavor ), rFlavor );

            if( pModel )
                pModel->SetSwapGraphicsMode( nOldSwapMode );

            return bOK;
        }
    }

    sal_uInt32 nFormat = SotExchange::GetFormat( rFlavor );
    if( HasFormat( nFormat ) )
    {
        if ( nFormat == SOT_FORMATSTR_ID_LINKSRCDESCRIPTOR || nFormat == SOT_FORMATSTR_ID_OBJECTDESCRIPTOR )
        {
            if ( bOleObj )              // single OLE object
            {
                SvInPlaceObjectRef xIPObj = GetSingleObject();
                if ( xIPObj.Is() )
                    xIPObj->FillTransferableObjectDescriptor( aObjDesc );
            }

            bOK = SetTransferableObjectDescriptor( aObjDesc, rFlavor );
        }
        else if ( nFormat == SOT_FORMATSTR_ID_DRAWING )
        {
            bOK = SetObject( pModel, SCDRAWTRANS_TYPE_DRAWMODEL, rFlavor );
        }
        else if ( nFormat == SOT_FORMAT_BITMAP || nFormat == SOT_FORMAT_GDIMETAFILE )
        {
            SdrExchangeView aView( pModel );
            Point aPos;
            SdrPageView* pPv = aView.ShowPagePgNum( 0, aPos );
            DBG_ASSERT( pPv, "pPv not there..." );
            aView.MarkAll( pPv );
            if ( nFormat == SOT_FORMAT_GDIMETAFILE )
                bOK = SetGDIMetaFile( aView.GetAllMarkedMetaFile( TRUE ), rFlavor );
            else
                bOK = SetBitmap( aView.GetAllMarkedBitmap( TRUE ), rFlavor );
        }
        else if ( nFormat == SOT_FORMATSTR_ID_SVXB )
        {
            // only enabled for single graphics object

            SdrPage* pPage = pModel->GetPage(0);
            if (pPage)
            {
                SdrObjListIter aIter( *pPage, IM_FLAT );
                SdrObject* pObject = aIter.Next();
                if (pObject && pObject->GetObjIdentifier() == OBJ_GRAF)
                {
                    SdrGrafObj* pGraphObj = (SdrGrafObj*) pObject;
                    bOK = SetGraphic( pGraphObj->GetGraphic(), rFlavor );
                }
            }
        }
        else if ( nFormat == SOT_FORMATSTR_ID_EMBED_SOURCE )
        {
            if ( bOleObj )              // single OLE object
            {
                SvInPlaceObjectRef xIPObj = GetSingleObject();
                if ( xIPObj.Is() )
                {
                    SvEmbeddedObject* pEmbObj = xIPObj;
                    bOK = SetObject( pEmbObj, SCDRAWTRANS_TYPE_EMBOBJ, rFlavor );
                }
            }
            else                        // create object from contents
            {
                InitDocShell();         // set aDocShellRef

                SvEmbeddedObject* pEmbObj = aDocShellRef;
                bOK = SetObject( pEmbObj, SCDRAWTRANS_TYPE_EMBOBJ, rFlavor );
            }
        }
        else if( pBookmark )
        {
            bOK = SetINetBookmark( *pBookmark, rFlavor );
        }
    }
    return bOK;
}

sal_Bool ScDrawTransferObj::WriteObject( SotStorageStreamRef& rxOStm, void* pUserObject, sal_uInt32 nUserObjectId,
                                        const ::com::sun::star::datatransfer::DataFlavor& rFlavor )
{
    // called from SetObject, put data into stream

    sal_Bool bRet = sal_False;
    switch (nUserObjectId)
    {
        case SCDRAWTRANS_TYPE_DRAWMODEL:
            {
                SdrModel* pDrawModel = (SdrModel*)pUserObject;
                pDrawModel->SetStreamingSdrModel(TRUE);
                rxOStm->SetBufferSize( 0xff00 );

                // #108584#
                // for the changed pool defaults from drawing layer pool set those
                // attributes as hard attributes to preserve them for saving
                const SfxItemPool& rItemPool = pModel->GetItemPool();
                const SvxFontHeightItem& rDefaultFontHeight = (const SvxFontHeightItem&)rItemPool.GetDefaultItem(EE_CHAR_FONTHEIGHT);

                // SW should have no MasterPages
                DBG_ASSERT(0L == pModel->GetMasterPageCount(), "SW with MasterPages (!)");

                for(sal_uInt16 a(0); a < pModel->GetPageCount(); a++)
                {
                    const SdrPage* pPage = pModel->GetPage(a);
                    SdrObjListIter aIter(*pPage, IM_DEEPNOGROUPS);

                    while(aIter.IsMore())
                    {
                        SdrObject* pObj = aIter.Next();
                        const SvxFontHeightItem& rItem = (const SvxFontHeightItem&)pObj->GetMergedItem(EE_CHAR_FONTHEIGHT);

                        if(rItem.GetHeight() == rDefaultFontHeight.GetHeight())
                        {
                            pObj->SetMergedItem(rDefaultFontHeight);
                        }
                    }
                }

                {
                    com::sun::star::uno::Reference<com::sun::star::io::XOutputStream> xDocOut( new utl::OOutputStreamWrapper( *rxOStm ) );
                    if( SvxDrawingLayerExport( pDrawModel, xDocOut ) )
                        rxOStm->Commit();
                }
                pDrawModel->SetStreamingSdrModel(FALSE);
                bRet = ( rxOStm->GetError() == ERRCODE_NONE );
            }
            break;

        case SCDRAWTRANS_TYPE_EMBOBJ:
            {
                SvEmbeddedObject* pEmbObj = (SvEmbeddedObject*)pUserObject;

                SvStorageRef xWorkStore( new SvStorage( TRUE, *rxOStm ) );
                rxOStm->SetBufferSize( 0xff00 );

                // write document storage
                pEmbObj->SetupStorage( xWorkStore );
                bRet = pEmbObj->DoSaveAs( xWorkStore );
                pEmbObj->DoSaveCompleted();
                xWorkStore->Commit();
                rxOStm->Commit();

                bRet = ( rxOStm->GetError() == ERRCODE_NONE );
            }
            break;

        default:
            DBG_ERROR("unknown object id");
    }
    return bRet;
}

void ScDrawTransferObj::ObjectReleased()
{
    ScModule* pScMod = SC_MOD();
    if ( pScMod->GetClipData().pDrawClipboard == this )
        pScMod->SetClipObject( NULL, NULL );

    TransferableHelper::ObjectReleased();
}

void ScDrawTransferObj::DragFinished( sal_Int8 nDropAction )
{
    if ( nDropAction == DND_ACTION_MOVE && !bDragWasInternal && !(nDragSourceFlags & SC_DROP_NAVIGATOR) )
    {
        //  move: delete source objects

        if ( pDragSourceView )
            pDragSourceView->DeleteMarked();
    }

    ScModule* pScMod = SC_MOD();
    if ( pScMod->GetDragData().pDrawTransfer == this )
        pScMod->ResetDragObject();

    DELETEZ( pDragSourceView );

    TransferableHelper::DragFinished( nDropAction );
}

void ScDrawTransferObj::SetDrawPersist( const SvEmbeddedObjectRef& rRef )
{
    aDrawPersistRef = rRef;
}

void lcl_InitMarks( SdrMarkView& rDest, const SdrMarkView& rSource, SCTAB nTab )
{
    rDest.ShowPagePgNum( static_cast<sal_uInt16>(static_cast<sal_Int16>(nTab)), Point() );
    SdrPageView* pDestPV = rDest.GetPageViewPvNum(0);
    DBG_ASSERT(pDestPV,"PageView ?");

    const SdrMarkList& rMarkList = rSource.GetMarkedObjectList();
    ULONG nCount = rMarkList.GetMarkCount();
    for (ULONG i=0; i<nCount; i++)
    {
        SdrMark* pMark = rMarkList.GetMark(i);
        SdrObject* pObj = pMark->GetObj();

        rDest.MarkObj(pObj, pDestPV);
    }
}

void ScDrawTransferObj::SetDragSource( ScDrawView* pView )
{
    DELETEZ( pDragSourceView );
    pDragSourceView = new SdrView( pView->GetModel() );
    lcl_InitMarks( *pDragSourceView, *pView, pView->GetTab() );

    //! add as listener with document, delete pDragSourceView if document gone
}

void ScDrawTransferObj::SetDragSourceObj( SdrObject* pObj, SCTAB nTab )
{
    DELETEZ( pDragSourceView );
    pDragSourceView = new SdrView( pObj->GetModel() );
    pDragSourceView->ShowPagePgNum( static_cast<sal_uInt16>(static_cast<sal_Int16>(nTab)), Point() );
    SdrPageView* pPV = pDragSourceView->GetPageViewPvNum(0);
    pDragSourceView->MarkObj(pObj, pPV);

    //! add as listener with document, delete pDragSourceView if document gone
}

void ScDrawTransferObj::SetDragSourceFlags( USHORT nFlags )
{
    nDragSourceFlags = nFlags;
}

void ScDrawTransferObj::SetDragWasInternal()
{
    bDragWasInternal = TRUE;
}

SvInPlaceObjectRef ScDrawTransferObj::GetSingleObject()
{
    //  if single OLE object was copied, get its object

    SdrPage* pPage = pModel->GetPage(0);
    if (pPage)
    {
        SdrObjListIter aIter( *pPage, IM_FLAT );
        SdrObject* pObject = aIter.Next();
        if (pObject && pObject->GetObjIdentifier() == OBJ_OLE2)
        {
            SdrOle2Obj* pOleObj = (SdrOle2Obj*) pObject;
            return pOleObj->GetObjRef();
        }
    }

    return SvInPlaceObjectRef();
}

//
//  initialize aDocShellRef with a live document from the ClipDoc
//

void ScDrawTransferObj::InitDocShell()
{
    if ( !aDocShellRef.Is() )
    {
        ScDocShell* pDocSh = new ScDocShell;
        aDocShellRef = pDocSh;      // ref must be there before InitNew

        pDocSh->DoInitNew(NULL);

        ScDocument* pDestDoc = pDocSh->GetDocument();
        pDestDoc->InitDrawLayer( pDocSh );

        SdrModel* pDestModel = pDestDoc->GetDrawLayer();
        SdrExchangeView aDestView( pDestModel );
        Point aPos;
        aDestView.ShowPagePgNum( 0, aPos );
        aDestView.Paste( *pModel, Point( aSrcSize.Width()/2, aSrcSize.Height()/2 ) );

        // put objects to right layer (see ScViewFunc::PasteDataFormat for SOT_FORMATSTR_ID_DRAWING)

        SdrPage* pPage = pDestModel->GetPage(0);
        if (pPage)
        {
            SdrObjListIter aIter( *pPage, IM_DEEPWITHGROUPS );
            SdrObject* pObject = aIter.Next();
            while (pObject)
            {
                if ( pObject->ISA(SdrUnoObj) )
                    pObject->NbcSetLayer(SC_LAYER_CONTROLS);
                else
                    pObject->NbcSetLayer(SC_LAYER_FRONT);
                pObject = aIter.Next();
            }
        }

        Point aTmpPoint;
        Rectangle aDestArea( aTmpPoint, aSrcSize );
        pDocSh->SetVisArea( aDestArea );

        ScViewOptions aViewOpt( pDestDoc->GetViewOptions() );
        aViewOpt.SetOption( VOPT_GRID, FALSE );
        pDestDoc->SetViewOptions( aViewOpt );

        ScViewData aViewData( pDocSh, NULL );
        aViewData.SetTabNo( 0 );
        aViewData.SetScreen( aDestArea );
        aViewData.SetCurX( 0 );
        aViewData.SetCurY( 0 );
        pDocSh->UpdateOle(&aViewData, TRUE);
    }
}


