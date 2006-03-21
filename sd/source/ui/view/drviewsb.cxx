/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drviewsb.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:44:51 $
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

#ifndef _COM_SUN_STAR_UI_DIALOGS_XEXECUTABLEDIALOG_HPP_
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _SVDLAYER_HXX
#include <svx/svdlayer.hxx>
#endif
#ifndef _SVXIDS_HXX
#include <svx/svxids.hrc>
#endif
#define ITEMID_HYPERLINK    SID_HYPERLINK_SETLINK

#include <svx/hyprlink.hxx>
#include <sfx2/viewfrm.hxx>

#ifndef _SVX_HLNKITEM_HXX
#include <svx/hlnkitem.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _EEITEM_HXX //autogen
#include <svx/eeitem.hxx>
#endif
#define ITEMID_FIELD    EE_FEATURE_FIELD
#ifndef _FLDITEM_HXX
#include <svx/flditem.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVDORECT_HXX //autogen
#include <svx/svdorect.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SB_SBSTAR_HXX //autogen
#include <basic/sbstar.hxx>
#endif
#ifndef _SB_SBERRORS_HXX //autogen
#include <basic/sberrors.hxx>
#endif
#ifndef _SVX_FMSHELL_HXX
#include <svx/fmshell.hxx>
#endif
//CHINA001 #ifndef _SVX_DLG_NAME_HXX
//CHINA001 #include <svx/dlgname.hxx>
//CHINA001 #endif
#include <svx/svxdlg.hxx> //CHINA001
#include <svx/dialogs.hrc> //CHINA001
#pragma hdrstop

#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif

#include "app.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"
#include "glob.hrc"

#ifndef SD_OUTLINER_HXX
#include "Outliner.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "app.hxx"
#include "sdattr.hxx"
//CHINA001 #include "ins_page.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "sdresid.hxx"
#include "sdpage.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
//CHINA001 #include "dlgfield.hxx"
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#include "unmodpg.hxx"
#include "undolayer.hxx"
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_FORM_SHELL_MANAGER_HXX
#include "FormShellManager.hxx"
#endif
#include "LayerTabBar.hxx"
#include "sdabstdlg.hxx" //CHINA001
#include "dlgfield.hrc" //CHINA001
#include "ins_page.hrc" //CHINA001
#include "SlideSorterViewShell.hxx"
#include "controller/SlideSorterController.hxx"

#define RET_DELETE  100 //CHINA001

namespace sd {

/*************************************************************************
|*
|* SfxRequests fuer temporaere Funktionen
|*
\************************************************************************/

void DrawViewShell::FuTemp02(SfxRequest& rReq)
{
    USHORT nSId = rReq.GetSlot();
    switch( nSId )
    {
        case SID_INSERTLAYER:
        {
            if ( pDrView->IsTextEdit() )
            {
                pDrView->EndTextEdit();
            }

            SdrLayerAdmin& rLayerAdmin = GetDoc()->GetLayerAdmin();
            USHORT nLayerCnt = rLayerAdmin.GetLayerCount();
            USHORT nLayer = nLayerCnt - 2 + 1;
            String aLayerName ( SdResId(STR_LAYER) );
            aLayerName += String::CreateFromInt32( (sal_Int32)nLayer );
            BOOL bIsVisible;
            BOOL bIsLocked;
            BOOL bIsPrintable;

            const SfxItemSet* pArgs = rReq.GetArgs();

            if (! pArgs)
            {
                SfxItemSet aNewAttr( GetDoc()->GetPool(), ATTR_LAYER_START, ATTR_LAYER_END );

                aNewAttr.Put( SdAttrLayerName( aLayerName ) );
                aNewAttr.Put( SdAttrLayerVisible() );
                aNewAttr.Put( SdAttrLayerPrintable() );
                aNewAttr.Put( SdAttrLayerLocked() );
                aNewAttr.Put( SdAttrLayerThisPage() );

                //CHINA001 SdInsertLayerDlg* pDlg = new SdInsertLayerDlg( NULL, aNewAttr );
                SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
                DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
                AbstractSdInsertLayerDlg* pDlg = pFact->CreateSdInsertLayerDlg(ResId( DLG_INSERT_LAYER ), NULL, aNewAttr, TRUE, String( SdResId( STR_INSERTLAYER ) ) );
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                pDlg->SetHelpId( SID_INSERTLAYER );

                // Ueberpruefung auf schon vorhandene Namen
                BOOL bLoop = TRUE;
                while( bLoop && pDlg->Execute() == RET_OK )
                {
                    pDlg->GetAttr( aNewAttr );
                    aLayerName   = ((SdAttrLayerName &) aNewAttr.Get (ATTR_LAYER_NAME)).GetValue ();

                    if( rLayerAdmin.GetLayer( aLayerName, FALSE )
                        || aLayerName.Len()==0 )
                    {
                        // Name ist schon vorhanden
                        WarningBox aWarningBox (
                            GetParentWindow(),
                            WinBits( WB_OK ),
                            String(SdResId( STR_WARN_NAME_DUPLICATE)));
                        aWarningBox.Execute();
                    }
                    else
                        bLoop = FALSE;
                }
                if( bLoop ) // wurde abgebrochen
                {
                    delete pDlg;

                    Cancel();
                    rReq.Ignore ();
                    break;
                }
                else
                {
                    //pDlg->GetAttr( aNewAttr );
                    //aLayerName     = ((SdAttrLayerName &) aNewAttr.Get (ATTR_LAYER_NAME)).GetValue ();
                    bIsVisible   = ((SdAttrLayerVisible &) aNewAttr.Get (ATTR_LAYER_VISIBLE)).GetValue ();
                    bIsLocked    = ((SdAttrLayerLocked &) aNewAttr.Get (ATTR_LAYER_LOCKED)).GetValue () ;
                    bIsPrintable = ((SdAttrLayerPrintable &) aNewAttr.Get (ATTR_LAYER_PRINTABLE)).GetValue () ;

                    delete pDlg;
                }
            }
            else if (pArgs->Count () != 4)
                 {
                     StarBASIC::FatalError (SbERR_WRONG_ARGS);
                     Cancel();
                     rReq.Ignore ();
                     break;
                 }
                 else
                 {
                     SFX_REQUEST_ARG (rReq, pLayerName, SfxStringItem, ID_VAL_LAYERNAME, FALSE);
                     SFX_REQUEST_ARG (rReq, pIsVisible, SfxBoolItem, ID_VAL_ISVISIBLE, FALSE);
                     SFX_REQUEST_ARG (rReq, pIsLocked, SfxBoolItem, ID_VAL_ISLOCKED, FALSE);
                     SFX_REQUEST_ARG (rReq, pIsPrintable, SfxBoolItem, ID_VAL_ISPRINTABLE, FALSE);

                     aLayerName   = pLayerName->GetValue ();
                     bIsVisible   = pIsVisible->GetValue ();
                     bIsLocked    = pIsLocked->GetValue ();
                     bIsPrintable = pIsPrintable->GetValue ();
                 }

            String aPrevLayer = pDrView->GetActiveLayer();
            String aName;
            SdrLayer* pLayer;
            USHORT nPrevLayer = 0;
            nLayerCnt = rLayerAdmin.GetLayerCount();

            for ( nLayer = 0; nLayer < nLayerCnt; nLayer++ )
            {
                pLayer = rLayerAdmin.GetLayer(nLayer);
                aName = pLayer->GetName();

                if ( aPrevLayer == aName )
                {
//                    nPrevLayer = nLayer;
                    nPrevLayer = Max(nLayer, (USHORT) 4);
                }
            }

            SdrLayer* pNewLayer = pDrView->InsertNewLayer(aLayerName, nPrevLayer + 1);

            pDrView->SetLayerVisible( aLayerName, bIsVisible );
            pDrView->SetLayerLocked( aLayerName, bIsLocked);
            pDrView->SetLayerPrintable(aLayerName, bIsPrintable);

            pDrView->SetActiveLayer(aLayerName);

            ResetActualLayer();

            GetDoc()->SetChanged(TRUE);

            GetViewFrame()->GetDispatcher()->Execute(SID_SWITCHLAYER,
                    SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);

            Cancel();
            rReq.Done ();
        }
        break;

        case SID_MODIFYLAYER:
        {
            if ( pDrView->IsTextEdit() )
            {
                pDrView->EndTextEdit();
            }

            SdrLayerAdmin& rLayerAdmin = GetDoc()->GetLayerAdmin();
            USHORT nCurPage = GetLayerTabControl()->GetCurPageId();
            String aLayerName = GetLayerTabControl()->GetPageText(nCurPage);
            String aOldLayerName( aLayerName );
            SdrLayer* pLayer = rLayerAdmin.GetLayer(aLayerName, FALSE);
            const SfxItemSet* pArgs = rReq.GetArgs();

            BOOL bIsVisible;
            BOOL bIsLocked;
            BOOL bIsPrintable;

             // darf der Layer geloescht werden ?
            BOOL bDelete;

            String aLayoutLayer ( SdResId(STR_LAYER_LAYOUT) );
            String aControlsLayer ( SdResId(STR_LAYER_CONTROLS) );
            String aMeasureLinesLayer ( SdResId(STR_LAYER_MEASURELINES) );
            String aBackgroundLayer( SdResId(STR_LAYER_BCKGRND) );
            String aBackgroundObjLayer( SdResId(STR_LAYER_BCKGRNDOBJ) );

            if( aLayerName == aLayoutLayer       || aLayerName == aControlsLayer ||
                aLayerName == aMeasureLinesLayer ||
                aLayerName == aBackgroundLayer   || aLayerName == aBackgroundObjLayer )
            {
                bDelete = FALSE;
            }
            else
                bDelete = TRUE;

            if (! pArgs)
            {
                SfxItemSet aNewAttr( GetDoc()->GetPool(), ATTR_LAYER_START, ATTR_LAYER_END );

                aNewAttr.Put( SdAttrLayerName( aLayerName ) );
                aNewAttr.Put( SdAttrLayerVisible( pDrView->IsLayerVisible(aLayerName) ) );
                aNewAttr.Put( SdAttrLayerLocked( pDrView->IsLayerLocked(aLayerName) ) );
                aNewAttr.Put( SdAttrLayerPrintable( pDrView->IsLayerPrintable(aLayerName) ) );
                aNewAttr.Put( SdAttrLayerThisPage() );

//CHINA001              SdInsertLayerDlg* pDlg = new SdInsertLayerDlg( NULL,
//CHINA001              aNewAttr, bDelete, String( SdResId( STR_MODIFYLAYER ) ) );
                SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
                DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
                AbstractSdInsertLayerDlg* pDlg = pFact->CreateSdInsertLayerDlg(ResId( DLG_INSERT_LAYER ), NULL,
                                                aNewAttr, bDelete, String( SdResId( STR_MODIFYLAYER ) ) );
                DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                pDlg->SetHelpId( SID_MODIFYLAYER );

                // Ueberpruefung auf schon vorhandene Namen
                BOOL    bLoop = TRUE;
                USHORT  nRet;
                String  aOldLayerName( aLayerName );
                while( bLoop && ( nRet = pDlg->Execute() == RET_OK ) )
                {
                    pDlg->GetAttr( aNewAttr );
                    aLayerName   = ((SdAttrLayerName &) aNewAttr.Get (ATTR_LAYER_NAME)).GetValue ();

                    if( (rLayerAdmin.GetLayer( aLayerName, FALSE ) &&
                         aLayerName != aOldLayerName) || aLayerName.Len()==0 )
                    {
                        // Name ist schon vorhanden
                        WarningBox aWarningBox (
                            GetParentWindow(),
                            WinBits( WB_OK ),
                            String( SdResId( STR_WARN_NAME_DUPLICATE)));
                        aWarningBox.Execute();
                    }
                    else
                        bLoop = FALSE;
                }
                switch (nRet)
                {
                    case RET_OK :
                        //pDlg->GetAttr( aNewAttr );
                        //aLayerName   = ((SdAttrLayerName &) aNewAttr.Get (ATTR_LAYER_NAME)).GetValue ();
                        bIsVisible   = ((const SdAttrLayerVisible &) aNewAttr.Get (ATTR_LAYER_VISIBLE)).GetValue ();
                        bIsLocked    = ((const SdAttrLayerLocked &) aNewAttr.Get (ATTR_LAYER_LOCKED)).GetValue ();
                        bIsPrintable = ((const SdAttrLayerLocked &) aNewAttr.Get (ATTR_LAYER_PRINTABLE)).GetValue ();

                        delete pDlg;
                        break;

                    case RET_DELETE :
                        pDrView->DeleteLayer (((SdAttrLayerName &) aNewAttr.Get (ATTR_LAYER_NAME)).GetValue ());
                        GetLayerTabControl()->RemovePage(nCurPage);

                    default :
                        delete pDlg;
                        rReq.Ignore ();
                        Cancel ();
                        return;
                }
            }
            else if (pArgs->Count () == 4)
            {
                SFX_REQUEST_ARG (rReq, pLayerName, SfxStringItem, ID_VAL_LAYERNAME, FALSE);
                SFX_REQUEST_ARG (rReq, pIsVisible, SfxBoolItem, ID_VAL_ISVISIBLE, FALSE);
                SFX_REQUEST_ARG (rReq, pIsLocked, SfxBoolItem, ID_VAL_ISLOCKED, FALSE);
                SFX_REQUEST_ARG (rReq, pIsPrintable, SfxBoolItem, ID_VAL_ISPRINTABLE, FALSE);

                aLayerName   = pLayerName->GetValue ();
                bIsVisible   = pIsVisible->GetValue ();
                bIsLocked     = pIsLocked->GetValue ();
                bIsPrintable = pIsPrintable->GetValue ();
            }
            else
            {
                StarBASIC::FatalError (SbERR_WRONG_ARGS);
                Cancel ();
                rReq.Ignore ();
                break;
            }

            SfxUndoManager* pManager = GetDoc()->GetDocSh()->GetUndoManager();
            SdLayerModifyUndoAction* pAction = new SdLayerModifyUndoAction(
                GetDoc(),
                pLayer,
                // old values
                aOldLayerName,
                pDrView->IsLayerVisible(aOldLayerName),
                pDrView->IsLayerLocked(aOldLayerName),
                pDrView->IsLayerPrintable(aOldLayerName),
                // new values
                aLayerName,
                bIsVisible,
                bIsLocked,
                bIsPrintable
                );
            pManager->AddUndoAction( pAction );

            ModifyLayer( pLayer, aLayerName, bIsVisible, bIsLocked, bIsPrintable );

            Cancel();
            rReq.Done ();
        }
        break;

        case SID_RENAMELAYER:
        {
            if ( pDrView->IsTextEdit() )
            {
                pDrView->EndTextEdit();
            }

            GetLayerTabControl()->StartEditMode(
                GetLayerTabControl()->GetCurPageId() );

            Cancel();
            rReq.Ignore ();
        }
        break;

        case SID_EDIT_HYPERLINK :
        {
            GetViewFrame()->GetDispatcher()->Execute( SID_HYPERLINK_DIALOG );

            Cancel();
            rReq.Done ();
        }
        break;

        case SID_OPEN_HYPERLINK:
        {
            OutlinerView* pOutView = pDrView->GetTextEditOutlinerView();
            if ( pOutView )
            {
                const SvxFieldItem* pFieldItem = pOutView->GetFieldAtSelection();
                if ( pFieldItem )
                {
                    const SvxFieldData* pField = pFieldItem->GetField();
                    if( pField && pField->ISA( SvxURLField ) )
                    {
                        const SvxURLField* pURLField = static_cast< const SvxURLField* >( pField );

                        SfxStringItem aUrl( SID_FILE_NAME, pURLField->GetURL() );
                        SfxStringItem aTarget( SID_TARGETNAME, pURLField->GetTargetFrame() );

                        String aReferName;
                        SfxViewFrame* pFrame = GetViewFrame();
                        SfxMedium* pMed = pFrame->GetObjectShell()->GetMedium();
                        if (pMed)
                            aReferName = pMed->GetName();

                        SfxFrameItem aFrm( SID_DOCFRAME, pFrame );
                        SfxStringItem aReferer( SID_REFERER, aReferName );

                        SfxBoolItem aNewView( SID_OPEN_NEW_VIEW, FALSE );
                        SfxBoolItem aBrowsing( SID_BROWSE, TRUE );

                        SfxViewFrame* pViewFrm = SfxViewFrame::Current();
                        if (pViewFrm)
                            pViewFrm->GetDispatcher()->Execute( SID_OPENDOC,
                                                        SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD,
                                                        &aUrl, &aTarget,
                                                        &aFrm, &aReferer,
                                                        &aNewView, &aBrowsing,
                                                        0L );
                    }
                }
            }
            Cancel();
            rReq.Done ();
        }
        break;

        case SID_HYPERLINK_SETLINK:
        {
            const SfxItemSet* pReqArgs = rReq.GetArgs();

            if (pReqArgs)
            {
                SvxHyperlinkItem* pHLItem =
                (SvxHyperlinkItem*) &pReqArgs->Get(ITEMID_HYPERLINK);

                if (pHLItem->GetInsertMode() == HLINK_FIELD)
                {
                    InsertURLField(pHLItem->GetURL(), pHLItem->GetName(),
                                   pHLItem->GetTargetFrame(), NULL);
                }
                else if (pHLItem->GetInsertMode() == HLINK_BUTTON)
                {
                    InsertURLButton(pHLItem->GetURL(), pHLItem->GetName(),
                                    pHLItem->GetTargetFrame(), NULL);
                }
                else if (pHLItem->GetInsertMode() == HLINK_DEFAULT)
                {
                    OutlinerView* pOlView = pDrView->GetTextEditOutlinerView();

                    if (pOlView)
                    {
                        InsertURLField(pHLItem->GetURL(), pHLItem->GetName(),
                                       pHLItem->GetTargetFrame(), NULL);
                    }
                    else
                    {
                        InsertURLButton(pHLItem->GetURL(), pHLItem->GetName(),
                                        pHLItem->GetTargetFrame(), NULL);
                    }
                }
            }

            Cancel();
            rReq.Ignore ();
        }
        break;

        case SID_INSERT_FLD_DATE_FIX:
        case SID_INSERT_FLD_DATE_VAR:
        case SID_INSERT_FLD_TIME_FIX:
        case SID_INSERT_FLD_TIME_VAR:
        case SID_INSERT_FLD_AUTHOR:
        case SID_INSERT_FLD_PAGE:
        case SID_INSERT_FLD_FILE:
        {
            USHORT nMul = 1;
            SvxFieldItem* pFieldItem;

            switch( nSId )
            {
                case SID_INSERT_FLD_DATE_FIX:
                    pFieldItem = new SvxFieldItem(
                        SvxDateField( Date(), SVXDATETYPE_FIX ) );
                break;

                case SID_INSERT_FLD_DATE_VAR:
                    pFieldItem = new SvxFieldItem( SvxDateField() );
                break;

                case SID_INSERT_FLD_TIME_FIX:
                    pFieldItem = new SvxFieldItem(
                        SvxExtTimeField( Time(), SVXTIMETYPE_FIX ) );
                break;

                case SID_INSERT_FLD_TIME_VAR:
                    pFieldItem = new SvxFieldItem( SvxExtTimeField() );
                break;

                case SID_INSERT_FLD_AUTHOR:
                {
                    SvtUserOptions aUserOptions;
                    pFieldItem = new SvxFieldItem(
                            SvxAuthorField(
                                aUserOptions.GetFirstName(), aUserOptions.GetLastName(), aUserOptions.GetID() ) );
                }
                break;

                case SID_INSERT_FLD_PAGE:
                {
                    pFieldItem = new SvxFieldItem( SvxPageField() );
                    nMul = 3;
                }
                break;

                case SID_INSERT_FLD_FILE:
                {
                    String aName;
                    if( GetDocSh()->HasName() )
                        aName = GetDocSh()->GetMedium()->GetName();
                    //else
                    //  aName = GetDocSh()->GetName();
                    pFieldItem = new SvxFieldItem( SvxExtFileField( aName ) );
                }
                break;
            }

            OutlinerView* pOLV = pDrView->GetTextEditOutlinerView();

            if( pOLV )
            {
                const SvxFieldItem* pOldFldItem = pOLV->GetFieldAtSelection();

                if( pOldFldItem && ( pOldFldItem->GetField()->ISA( SvxURLField ) ||
                                    pOldFldItem->GetField()->ISA( SvxDateField ) ||
                                    pOldFldItem->GetField()->ISA( SvxTimeField ) ||
                                    pOldFldItem->GetField()->ISA( SvxExtTimeField ) ||
                                    pOldFldItem->GetField()->ISA( SvxExtFileField ) ||
                                    pOldFldItem->GetField()->ISA( SvxAuthorField ) ||
                                    pOldFldItem->GetField()->ISA( SvxPageField ) ) )
                {
                    // Feld selektieren, so dass es beim Insert geloescht wird
                    ESelection aSel = pOLV->GetSelection();
                    if( aSel.nStartPos == aSel.nEndPos )
                        aSel.nEndPos++;
                    pOLV->SetSelection( aSel );
                }

                pOLV->InsertField( *pFieldItem );
            }
            else
            {
                Outliner* pOutl = GetDoc()->GetInternalOutliner();
                pOutl->Init( OUTLINERMODE_TEXTOBJECT );
                USHORT nOutlMode = pOutl->GetMode();
                USHORT nMinDepth = pOutl->GetMinDepth();
                pOutl->SetMinDepth( 0 );
                pOutl->SetStyleSheet( 0, NULL );
                pOutl->QuickInsertField( *pFieldItem, ESelection() );
                OutlinerParaObject* pOutlParaObject = pOutl->CreateParaObject();

                SdrRectObj* pRectObj = new SdrRectObj( OBJ_TEXT );
                pRectObj->SetMergedItem(SdrTextAutoGrowWidthItem(TRUE));

                pOutl->UpdateFields();
                pOutl->SetUpdateMode( TRUE );
                Size aSize( pOutl->CalcTextSize() );
                aSize.Width() *= nMul;
                pOutl->SetUpdateMode( FALSE );

                Point aPos;
                Rectangle aRect( aPos, GetActiveWindow()->GetOutputSizePixel() );
                aPos = aRect.Center();
                aPos = GetActiveWindow()->PixelToLogic(aPos);
                aPos.X() -= aSize.Width() / 2;
                aPos.Y() -= aSize.Height() / 2;

                Rectangle aLogicRect(aPos, aSize);
                pRectObj->SetLogicRect(aLogicRect);
                pRectObj->SetOutlinerParaObject( pOutlParaObject );
                pDrView->InsertObject(pRectObj, *pDrView->GetPageViewPvNum(0));
                pOutl->Init( nOutlMode );
                pOutl->SetMinDepth( nMinDepth );
            }

            delete pFieldItem;

            Cancel();
            rReq.Ignore ();
        }
        break;

        case SID_MODIFY_FIELD:
        {
            OutlinerView* pOLV = pDrView->GetTextEditOutlinerView();

            if( pOLV )
            {
                const SvxFieldItem* pFldItem = pOLV->GetFieldAtSelection();

                if( pFldItem && (pFldItem->GetField()->ISA( SvxDateField ) ||
                                 pFldItem->GetField()->ISA( SvxAuthorField ) ||
                                 pFldItem->GetField()->ISA( SvxExtFileField ) ||
                                 pFldItem->GetField()->ISA( SvxExtTimeField ) ) )
                {
                    // Dialog...
                    //CHINA001 SdModifyFieldDlg aDlg( pWindow, pFldItem->GetField(), pOLV->GetAttribs() );
                    SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
                    DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
                    AbstractSdModifyFieldDlg* pDlg = pFact->CreateSdModifyFieldDlg(ResId( DLG_FIELD_MODIFY ), GetActiveWindow(), pFldItem->GetField(), pOLV->GetAttribs() );
                    DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
                    if( pDlg->Execute() == RET_OK ) //CHINA001 if( aDlg.Execute() == RET_OK )
                    {
                        // #108538#
                        // To make a correct SetAttribs() call at the utlinerView
                        // it is necessary to split the actions here
                        SvxFieldData* pField = pDlg->GetField(); //CHINA001 SvxFieldData* pField = aDlg.GetField();
                        ESelection aSel = pOLV->GetSelection();
                        sal_Bool bSelectionWasModified(sal_False);

                        if( pField )
                        {
                            SvxFieldItem aFieldItem( *pField );

                            if( aSel.nStartPos == aSel.nEndPos )
                            {
                                bSelectionWasModified = sal_True;
                                aSel.nEndPos++;
                                pOLV->SetSelection( aSel );
                            }

                            pOLV->InsertField( aFieldItem );

                            // #108538# select again for eventual SetAttribs call
                            pOLV->SetSelection( aSel );
                        }

                        SfxItemSet aSet( pDlg->GetItemSet() ); //CHINA001 SfxItemSet aSet( aDlg.GetItemSet() );

                        if( aSet.Count() )
                        {
                            pOLV->SetAttribs( aSet );

                            ::Outliner* pOutliner = pOLV->GetOutliner();
                            if( pOutliner )
                                pOutliner->UpdateFields();
                        }

                        if(pField)
                        {
                            // #108538# restore selection to original
                            if(bSelectionWasModified)
                            {
                                aSel.nEndPos--;
                                pOLV->SetSelection( aSel );
                            }

                            delete pField;
                        }
                    }
                    delete pDlg; //add by CHINA001
                }
            }

            Cancel();
            rReq.Ignore ();
        }
        break;

        case SID_OPEN_XML_FILTERSETTINGS:
        {
            try
            {
                com::sun::star::uno::Reference < ::com::sun::star::ui::dialogs::XExecutableDialog > xDialog(::comphelper::getProcessServiceFactory()->createInstance(rtl::OUString::createFromAscii("com.sun.star.comp.ui.XSLTFilterDialog")), com::sun::star::uno::UNO_QUERY);
                if( xDialog.is() )
                {
                    xDialog->execute();
                }
            }
            catch( ::com::sun::star::uno::RuntimeException& )
            {
            }

            Cancel();
            rReq.Ignore ();
        }
        break;

        default:
        {
            // switch Anweisung wegen CLOOKS aufgeteilt. Alle case-Anweisungen die
            // eine Fu???? -Funktion aufrufen, sind in die Methode FuTemp03 (drviewsb)
            // gewandert.
            FuTemp03(rReq);
        }
        break;
    };
};

bool DrawViewShell::RenameSlide( USHORT nPageId, const String & rName  )
{
    BOOL   bOutDummy;
    if( GetDoc()->GetPageByName( rName, bOutDummy ) != SDRPAGE_NOTFOUND )
        return false;

    SdPage* pPageToRename = NULL;
    PageKind ePageKind = GetPageKind();

    if( GetEditMode() == EM_PAGE )
    {
        pPageToRename = GetDoc()->GetSdPage( nPageId - 1, ePageKind );

        // Undo
        SdPage* pUndoPage = pPageToRename;
        SdrLayerAdmin &  rLayerAdmin = GetDoc()->GetLayerAdmin();
        BYTE nBackground = rLayerAdmin.GetLayerID( String( SdResId( STR_LAYER_BCKGRND )), FALSE );
        BYTE nBgObj = rLayerAdmin.GetLayerID( String( SdResId( STR_LAYER_BCKGRNDOBJ )), FALSE );
        SetOfByte aVisibleLayers = pActualPage->TRG_GetMasterPageVisibleLayers();

        // (#67720#)
        SfxUndoManager* pManager = GetDoc()->GetDocSh()->GetUndoManager();
        ModifyPageUndoAction* pAction = new ModifyPageUndoAction(
            pManager, GetDoc(), pUndoPage, rName, pUndoPage->GetAutoLayout(),
            aVisibleLayers.IsSet( nBackground ),
            aVisibleLayers.IsSet( nBgObj ));
        pManager->AddUndoAction( pAction );

        // rename
        pPageToRename->SetName( rName );

        if( ePageKind == PK_STANDARD )
        {
            // also rename notes-page
            SdPage* pNotesPage = GetDoc()->GetSdPage( nPageId - 1, PK_NOTES );
            pNotesPage->SetName( rName );
        }
    }
    else
    {
        // rename MasterPage -> rename LayoutTemplate
        pPageToRename = GetDoc()->GetMasterSdPage( nPageId - 1, ePageKind );
        GetDoc()->RenameLayoutTemplate( pPageToRename->GetLayoutName(), rName );
    }

    bool bSuccess = ( FALSE != rName.Equals( pPageToRename->GetName()));

    if( bSuccess )
    {
        // user edited page names may be changed by the page so update control
        aTabControl.SetPageText( nPageId, rName );

        // set document to modified state
        GetDoc()->SetChanged( TRUE );

        // inform navigator about change
        SfxBoolItem aItem( SID_NAVIGATOR_INIT, TRUE );
        GetViewFrame()->GetDispatcher()->Execute(
            SID_NAVIGATOR_INIT, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

        // Tell the slide sorter about the name change (necessary for
        // accessibility.)
        slidesorter::SlideSorterViewShell* pSlideSorter
            = slidesorter::SlideSorterViewShell::GetSlideSorter(GetViewShellBase());
        if (pSlideSorter != NULL)
        {
            pSlideSorter->GetSlideSorterController().PageNameHasChanged(nPageId-1, rName);
        }
    }

    return bSuccess;
}

IMPL_LINK( DrawViewShell, RenameSlideHdl, AbstractSvxNameDialog*, pDialog )
{
    if( ! pDialog )
        return 0;

    String aNewName;
    pDialog->GetName( aNewName );

    SdPage* pCurrentPage = GetDoc()->GetSdPage( aTabControl.GetCurPageId() - 1, GetPageKind() );

    return pCurrentPage && ( aNewName.Equals( pCurrentPage->GetName() ) || GetDocSh()->IsNewPageNameValid( aNewName ) );
}




void DrawViewShell::ModifyLayer (
    SdrLayer* pLayer,
    String& rLayerName,
    bool bIsVisible,
    bool bIsLocked,
    bool bIsPrintable)
{
    if( pLayer )
    {

        const USHORT nPageCount = GetLayerTabControl()->GetPageCount();
        USHORT nCurPage = 0;
        USHORT nPos;
        for( nPos = 0; nPos < nPageCount; nPos++ )
        {
            USHORT nId = GetLayerTabControl()->GetPageId( nPos );
            if( pLayer->GetName() == GetLayerTabControl()->GetPageText( nId ) )
            {
                nCurPage = nId;
                break;
            }
        }

        pLayer->SetName( rLayerName );
        pDrView->SetLayerVisible( rLayerName, bIsVisible );
        pDrView->SetLayerLocked( rLayerName, bIsLocked);
        pDrView->SetLayerPrintable(rLayerName, bIsPrintable);

        GetDoc()->SetChanged(TRUE);

        GetLayerTabControl()->SetPageText(nCurPage, rLayerName);

        TabBarPageBits nBits = 0;

        if (!bIsVisible)
        {
            // Unsichtbare Layer werden anders dargestellt
            nBits = TPB_SPECIAL;
        }

        GetLayerTabControl()->SetPageBits(nCurPage, nBits);

        GetViewFrame()->GetDispatcher()->Execute(
            SID_SWITCHLAYER,
            SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);

        // Call Invalidate at the form shell.
        FmFormShell* pFormShell = GetViewShellBase().GetFormShellManager().GetFormShell();
        if (pFormShell != NULL)
            pFormShell->Invalidate();
    }
}

} // end of namespace sd
