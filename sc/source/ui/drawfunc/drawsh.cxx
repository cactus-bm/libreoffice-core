/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drawsh.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-06 12:42:27 $
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

#include <svx/svxdlg.hxx> //CHINA001
#include <svx/dialogs.hrc> //CHINA001

#include "scitems.hxx"

#include <svx/eeitem.hxx>
#include <svx/fontwork.hxx>
//#include <svx/labdlg.hxx> CHINA001
#include <svx/srchitem.hxx>
#include <svx/tabarea.hxx>
#include <svx/tabline.hxx>
//CHINA001 #include <svx/transfrm.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/request.hxx>
#include <sfx2/dispatch.hxx>
#include <svtools/whiter.hxx>
#include <vcl/msgbox.hxx>

#include "drawsh.hxx"
#include "drwlayer.hxx"
#include "sc.hrc"
#include "viewdata.hxx"
#include "document.hxx"
#include "docpool.hxx"
#include "drawview.hxx"
#include "scresid.hxx"

#ifndef _SVDOBJ_HXX //autogen
#include <svx/svdobj.hxx>
#endif
//add header of cui CHINA001
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
#include <svx/drawitem.hxx>
#include <svx/xtable.hxx>

#define ScDrawShell
#include "scslots.hxx"

#include "userdat.hxx"
#include <sfx2/objsh.hxx>
#include <sfx2/macropg.hxx>
#include <svtools/macitem.hxx>
#include <com/sun/star/util/XModifiable.hpp>

//------------------------------------------------------------------

TYPEINIT1( ScDrawShell, SfxShell );

SFX_IMPL_INTERFACE(ScDrawShell, SfxShell, ScResId(SCSTR_DRAWSHELL) )
{
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT|SFX_VISIBILITY_STANDARD|SFX_VISIBILITY_SERVER,
                                ScResId(RID_DRAW_OBJECTBAR) );
    SFX_POPUPMENU_REGISTRATION( ScResId(RID_POPUP_DRAW) );
    SFX_CHILDWINDOW_REGISTRATION( SvxFontWorkChildWindow::GetChildWindowId() );
}


// abschalten der nicht erwuenschten Acceleratoren:

void ScDrawShell::StateDisableItems( SfxItemSet &rSet )
{
    SfxWhichIter aIter(rSet);
    USHORT nWhich = aIter.FirstWhich();

    while (nWhich)
    {
        rSet.DisableItem( nWhich );
        nWhich = aIter.NextWhich();
    }
}

void lcl_setModified( SfxObjectShell*  pShell )
{
    if ( pShell )
    {
        com::sun::star::uno::Reference< com::sun::star::util::XModifiable > xModif( pShell->GetModel(), com::sun::star::uno::UNO_QUERY );
        if ( xModif.is() )
            xModif->setModified( sal_True );
    }
}

void ScDrawShell::ExecDrawAttr( SfxRequest& rReq )
{
    USHORT              nSlot       = rReq.GetSlot();
    Window*             pWin        = pViewData->GetActiveWin();
//  SfxViewFrame*       pViewFrame  = SfxViewShell::Current()->GetViewFrame(); //!!! koennte knallen
    ScDrawView*         pView       = pViewData->GetScDrawView();
    SdrModel*           pDoc        = pViewData->GetDocument()->GetDrawLayer();

    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    ULONG nMarkCount = rMarkList.GetMarkCount();
    SdrObject* pSingleSelectedObj = NULL;
    if ( nMarkCount > 0 )
        pSingleSelectedObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();

    switch ( nSlot )
    {
        case SID_ASSIGNMACRO:
            {
                if ( pSingleSelectedObj )
                    ExecuteMacroAssign( pSingleSelectedObj, pWin );
            }
            break;

        case SID_TEXT_STANDARD: // Harte Textattributierung loeschen
            {
                SfxItemSet aEmptyAttr(GetPool(), EE_ITEMS_START, EE_ITEMS_END);
                pView->SetAttributes(aEmptyAttr, TRUE);
            }
            break;

        case SID_ATTR_LINE_STYLE:
        case SID_ATTR_LINEEND_STYLE:
        case SID_ATTR_LINE_DASH:
        case SID_ATTR_LINE_WIDTH:
        case SID_ATTR_LINE_COLOR:
        case SID_ATTR_FILL_STYLE:
        case SID_ATTR_FILL_COLOR:
        case SID_ATTR_FILL_GRADIENT:
        case SID_ATTR_FILL_HATCH:
        case SID_ATTR_FILL_BITMAP:

        // #i25616#
        case SID_ATTR_FILL_SHADOW:
            {
                // Wenn ToolBar vertikal :
                if ( !rReq.GetArgs() )
                {
                    switch ( nSlot )
                    {
                        case SID_ATTR_LINE_STYLE:
                        case SID_ATTR_LINE_DASH:
                        case SID_ATTR_LINE_WIDTH:
                        case SID_ATTR_LINE_COLOR:
                            ExecuteLineDlg( rReq );
                            break;

                        case SID_ATTR_FILL_STYLE:
                        case SID_ATTR_FILL_COLOR:
                        case SID_ATTR_FILL_GRADIENT:
                        case SID_ATTR_FILL_HATCH:
                        case SID_ATTR_FILL_BITMAP:

                        // #i25616#
                        case SID_ATTR_FILL_SHADOW:

                            ExecuteAreaDlg( rReq );
                            break;

                        default:
                            break;
                    }

                    //=====
                    return;
                    //=====
                }

                if( pView->AreObjectsMarked() )
                    pView->SetAttrToMarked( *rReq.GetArgs(), FALSE );
                else
                    pView->SetDefaultAttr( *rReq.GetArgs(), FALSE);
                pView->StoreCaptionAttribs();
                pView->InvalidateAttribs();
            }
            break;

        case SID_ATTRIBUTES_LINE:
            ExecuteLineDlg( rReq );
            break;

        case SID_ATTRIBUTES_AREA:
            ExecuteAreaDlg( rReq );
            break;

        case SID_DRAWTEXT_ATTR_DLG:
            ExecuteTextAttrDlg( rReq );
            break;

#ifdef ISSUE66550_HLINK_FOR_SHAPES
        case SID_DRAW_HLINK_EDIT:
            if ( pSingleSelectedObj )
                pViewData->GetDispatcher().Execute( SID_HYPERLINK_DIALOG );
            break;

        case SID_DRAW_HLINK_DELETE:
            if ( pSingleSelectedObj )
                SetHlinkForObject( pSingleSelectedObj, rtl::OUString() );
            break;

        case SID_OPEN_HYPERLINK:
            if ( nMarkCount == 1 )
            {
                SdrObject* pObj = rMarkList.GetMark( 0 )->GetMarkedSdrObj();
                if ( pObj->IsGroupObject() )
                {
                    SdrPageView* pPV = 0;
                    SdrObject* pHit = 0;
                    if ( pView->PickObj( pWin->PixelToLogic( pViewData->GetMousePosPixel() ), pHit, pPV, SDRSEARCH_DEEP ) )
                        pObj = pHit;
                }

                ScMacroInfo* pInfo = ScDrawLayer::GetMacroInfo( pObj );
                if ( pInfo && (pInfo->GetHlink().getLength() > 0) )
                    ScGlobal::OpenURL( pInfo->GetHlink(), String::EmptyString() );
            }
            break;
#endif

        case SID_ATTR_TRANSFORM:
            {
                if ( pView->AreObjectsMarked() )
                {
                    const SfxItemSet* pArgs = rReq.GetArgs();

                    if( !pArgs )
                    {
                        // const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
                        if( rMarkList.GetMark(0) != 0 )
                        {
                            SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
                            if( pObj->GetObjIdentifier() == OBJ_CAPTION )
                            {
                                // --------- Itemset fuer Caption --------
                                SfxItemSet aNewAttr(pDoc->GetItemPool());
                                pView->GetAttributes(aNewAttr);
                                // --------- Itemset fuer Groesse und Position --------
                                SfxItemSet aNewGeoAttr(pView->GetGeoAttrFromMarked());

                                //SvxCaptionTabDialog* pDlg = new SvxCaptionTabDialog(pWin, pView);
                                //change for cui CHINA001
                                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                                if ( pFact )
                                {
                                    SfxAbstractTabDialog *pDlg = pFact->CreateCaptionDialog( pWin, pView, RID_SVXDLG_CAPTION );

                                    const USHORT* pRange = pDlg->GetInputRanges( *aNewAttr.GetPool() );
                                    SfxItemSet aCombSet( *aNewAttr.GetPool(), pRange );
                                    aCombSet.Put( aNewAttr );
                                    aCombSet.Put( aNewGeoAttr );
                                    pDlg->SetInputSet( &aCombSet );

                                    if (pDlg->Execute() == RET_OK)
                                    {
                                        rReq.Done(*(pDlg->GetOutputItemSet()));
                                        pView->SetAttributes(*pDlg->GetOutputItemSet());
                                        pView->SetGeoAttrToMarked(*pDlg->GetOutputItemSet());
                                        pView->StoreCaptionDimensions();
                                    }

                                    delete pDlg;
                                }// change for cui
                            }
                            else
                            {
                                SfxItemSet aNewAttr(pView->GetGeoAttrFromMarked());
                                //CHINA001 SvxTransformTabDialog* pDlg = new SvxTransformTabDialog(pWin, &aNewAttr, pView);
                                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                                if(pFact)
                                {
                                    SfxAbstractTabDialog* pDlg = pFact->CreateSvxTransformTabDialog( pWin, &aNewAttr,pView, RID_SVXDLG_TRANSFORM );
                                    DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                                    if (pDlg->Execute() == RET_OK)
                                    {
                                        rReq.Done(*(pDlg->GetOutputItemSet()));
                                        pView->SetGeoAttrToMarked(*pDlg->GetOutputItemSet());
                                    }
                                    delete pDlg;
                                }
                            }
                        }


                    }
                    else
                        pView->SetGeoAttrToMarked( *pArgs );
                }
            }
            break;

        default:
            break;
    }
}

void ScDrawShell::ExecuteMacroAssign( SdrObject* pObj, Window* pWin )
{
    SvxMacroItem aItem ( SFX_APP()->GetPool().GetWhich( SID_ATTR_MACROITEM ) );
    ScMacroInfo* pInfo = ScDrawLayer::GetMacroInfo( pObj, TRUE );
    if ( pInfo->GetMacro().getLength() > 0 )
    {
        SvxMacroTableDtor aTab;
        String sMacro(  pInfo->GetMacro() );
        aTab.Insert( SFX_EVENT_MOUSECLICK_OBJECT, new SvxMacro( sMacro, String() ) );
        aItem.SetMacroTable( aTab );
    }
    // create empty itemset for macro-dlg
    SfxItemSet* pItemSet = new SfxItemSet(SFX_APP()->GetPool(), SID_ATTR_MACROITEM, SID_ATTR_MACROITEM );
    pItemSet->Put ( aItem, SID_ATTR_MACROITEM );

    SfxMacroAssignDlg aDlg( pWin, *pItemSet );
    SfxMacroTabPage *pMacroPage = (SfxMacroTabPage*) aDlg.GetTabPage();
    pMacroPage->AddEvent( ScResId(RID_SCSTR_ONCLICK),
        SFX_EVENT_MOUSECLICK_OBJECT);

    if ( RET_OK == aDlg.Execute() )
    {
        const SfxItemSet* pOutSet = aDlg.GetOutputItemSet();
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pOutSet->GetItemState( SID_ATTR_MACROITEM, FALSE, &pItem ))
        {
            rtl::OUString sMacro;
            SvxMacro* pMacro = ((SvxMacroItem*)pItem)->GetMacroTable().Get( SFX_EVENT_MOUSECLICK_OBJECT );
            if ( pMacro )
            {
                if ( pObj->IsGroupObject() )
                {
                    SdrObjList* pOL = pObj->GetSubList();
                    ULONG nObj = pOL->GetObjCount();
                    for ( ULONG index=0; index<nObj; ++index )
                    {
                        pInfo = ScDrawLayer::GetMacroInfo( pOL->GetObj(index), TRUE );
                        pInfo->SetMacro( pMacro->GetMacName() );
                    }
                }
                else
                    pInfo->SetMacro( pMacro->GetMacName() );
                lcl_setModified( GetObjectShell() );
            }
        }
    }

    delete pItemSet;
}

void ScDrawShell::ExecuteLineDlg( SfxRequest& rReq, USHORT nTabPage )
{
    ScDrawView*         pView       = pViewData->GetScDrawView();
    BOOL                bHasMarked  = pView->AreObjectsMarked();
    const SdrObject*    pObj        = NULL;
    const SdrMarkList&  rMarkList   = pView->GetMarkedObjectList();

    if( rMarkList.GetMarkCount() == 1 )
        pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();

    SfxItemSet  aNewAttr( pView->GetDefaultAttr() );
    if( bHasMarked )
        pView->MergeAttrFromMarked( aNewAttr, FALSE );

//CHINA001  SvxLineTabDialog* pDlg
//CHINA001  = new SvxLineTabDialog( pViewData->GetDialogParent(),
//CHINA001  &aNewAttr,
//CHINA001  pViewData->GetDocument()->GetDrawLayer(),
//CHINA001  pObj,
//CHINA001  bHasMarked );
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        DBG_ASSERT(pFact, "Dialogdiet Factory fail!");//CHINA001
        SfxAbstractTabDialog * pDlg = pFact->CreateSvxLineTabDialog( pViewData->GetDialogParent(),
                    &aNewAttr,
                pViewData->GetDocument()->GetDrawLayer(),
                RID_SVXDLG_LINE,
                pObj,
                bHasMarked);
        DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
    if ( nTabPage != 0xffff )
        pDlg->SetCurPageId( nTabPage );

    if ( pDlg->Execute() == RET_OK )
    {
        if( bHasMarked )
            pView->SetAttrToMarked( *pDlg->GetOutputItemSet(), FALSE );
        else
            pView->SetDefaultAttr( *pDlg->GetOutputItemSet(), FALSE );

        pView->StoreCaptionAttribs();
        pView->InvalidateAttribs();
        rReq.Done();
    }

    delete pDlg;
}

void ScDrawShell::ExecuteAreaDlg( SfxRequest& rReq, USHORT nTabPage )
{
    ScDrawView* pView       = pViewData->GetScDrawView();
    BOOL        bHasMarked  = pView->AreObjectsMarked();

    SfxItemSet  aNewAttr( pView->GetDefaultAttr() );
    if( bHasMarked )
        pView->MergeAttrFromMarked( aNewAttr, FALSE );

    //CHINA001 SvxAreaTabDialog* pDlg
    //CHINA001  = new SvxAreaTabDialog( pViewData->GetDialogParent(),
//CHINA001                              &aNewAttr,
//CHINA001                              pViewData->GetDocument()->GetDrawLayer(),
//CHINA001                              pView );

    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
    DBG_ASSERT(pFact, "Dialogdiet Factory fail!");//CHINA001
    AbstractSvxAreaTabDialog * pDlg = pFact->CreateSvxAreaTabDialog( pViewData->GetDialogParent(),
                                                                    &aNewAttr,
                                                            pViewData->GetDocument()->GetDrawLayer(),
                                                            RID_SVXDLG_AREA,
                                                            pView);
    DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001

    // #i74099# by default, the dialog deletes the current color table if a different one is loaded
    // (see SwDrawShell::ExecDrawDlg)
    const SvxColorTableItem* pColorItem =
        static_cast<const SvxColorTableItem*>( pViewData->GetSfxDocShell()->GetItem(SID_COLOR_TABLE) );
    if (pColorItem->GetColorTable() == XColorTable::GetStdColorTable())
        pDlg->DontDeleteColorTable();

    if ( nTabPage != 0xffff )
        pDlg->SetCurPageId( nTabPage );

    if ( pDlg->Execute() == RET_OK )
    {
        if( bHasMarked )
            pView->SetAttrToMarked( *pDlg->GetOutputItemSet(), FALSE );
        else
            pView->SetDefaultAttr( *pDlg->GetOutputItemSet(), FALSE );

        pView->StoreCaptionAttribs();
        pView->InvalidateAttribs();
        rReq.Done();
    }

    delete pDlg;
}

void ScDrawShell::ExecuteTextAttrDlg( SfxRequest& rReq, USHORT /* nTabPage */ )
{
    ScDrawView* pView       = pViewData->GetScDrawView();
    BOOL        bHasMarked  = pView->AreObjectsMarked();
    SfxItemSet  aNewAttr    ( pView->GetDefaultAttr() );

    if( bHasMarked )
        pView->MergeAttrFromMarked( aNewAttr, FALSE );

    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
    SfxAbstractTabDialog *pDlg = pFact->CreateTextTabDialog( pViewData->GetDialogParent(), &aNewAttr, RID_SVXDLG_TEXT, pView );

    USHORT nResult = pDlg->Execute();

    if ( RET_OK == nResult )
    {
        if ( bHasMarked )
            pView->SetAttributes( *pDlg->GetOutputItemSet() );
        else
            pView->SetDefaultAttr( *pDlg->GetOutputItemSet(), FALSE );

        pView->StoreCaptionAttribs();
        pView->InvalidateAttribs();
        rReq.Done();
    }
    delete( pDlg );
}

#ifdef ISSUE66550_HLINK_FOR_SHAPES
void ScDrawShell::SetHlinkForObject( SdrObject* pObj, const rtl::OUString& rHlnk )
{
    if ( pObj )
    {
        ScMacroInfo* pInfo = ScDrawLayer::GetMacroInfo( pObj, TRUE );
        pInfo->SetHlink( rHlnk );
        lcl_setModified( GetObjectShell() );
    }
}
#endif

