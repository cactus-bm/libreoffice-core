/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drviews2.cxx,v $
 *
 *  $Revision: 1.44 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-05 21:53:48 $
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

#include "DrawViewShell.hxx"
#include "ViewShellImplementation.hxx"

#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif

#ifndef _SVDOGRAF_HXX
#include <svx/svdograf.hxx>
#endif
#ifndef _SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _SVDUNDO_HXX //autogen
#include <svx/svdundo.hxx>
#endif
#ifndef _ZOOMITEM_HXX
#include <svx/zoomitem.hxx>
#endif
#ifndef _EDITDATA_HXX
#include <svx/editdata.hxx>
#endif
#ifndef _SB_SBERRORS_HXX //autogen
#include <basic/sberrors.hxx>
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
#ifndef SVX_XFILLIT0_HXX //autogen
#include <svx/xfillit0.hxx>
#endif
#ifndef _SVX_XFLCLIT_HXX //autogen
#include <svx/xflclit.hxx>
#endif
#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif
#ifndef _EEITEM_HXX //autogen
#include <svx/eeitem.hxx>
#endif
#ifndef _SB_SBSTAR_HXX //autogen
#include <basic/sbstar.hxx>
#endif

#define ITEMID_FIELD    EE_FEATURE_FIELD
#ifndef _SVX_FLDITEM_HXX //autogen
#include <svx/flditem.hxx>
#endif
#ifndef _SVX_XLINEIT0_HXX //autogen
#include <svx/xlineit0.hxx>
#endif
#ifndef SVX_XFILLIT0_HXX //autogen
#include <svx/xfillit0.hxx>
#endif

#ifndef _SDOUTL_HXX //autogen
#include <svx/svdoutl.hxx>
#endif

#ifndef _SVX_XLNWTIT_HXX
#include <svx/xlnwtit.hxx>
#endif
#ifndef _SVDOATTR_HXX //autogen
#include <svx/svdoattr.hxx>
#endif
#ifndef _SVX_XLNSTWIT_HXX
#include <svx/xlnstwit.hxx>
#endif
#ifndef _SDTMFITM_HXX //autogen
#include <svx/sdtmfitm.hxx>
#endif
#ifndef _SDTAGITM_HXX //autogen
#include <svx/sdtagitm.hxx>
#endif
#ifndef _SVX_XLNEDWIT_HXX
#include <svx/xlnedwit.hxx>
#endif
#ifndef _SVX_FONTWORK_BAR_HXX
#include <svx/fontworkbar.hxx>
#endif

#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>

#include <sfx2/viewfrm.hxx>

#ifndef _SD_SDGRFFILTER_HXX
#include "sdgrffilter.hxx"
#endif

#include "app.hrc"
#include "glob.hrc"
#include "helpids.h"
#include "sdattr.hxx"
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "ins_page.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "sdpage.hxx"
#ifndef SD_FU_SCALE_HXX
#include "fuscale.hxx"
#endif
#include "sdresid.hxx"
#ifndef SD_GRAPHIC_VIEW_SHELL_HXX
#include "GraphicViewShell.hxx"
#endif
#include "unmodpg.hxx"
#ifndef _SD_SLIDESHOW_HXX
#include "slideshow.hxx"
#endif
#ifndef SD_FU_VECTORIZE_HXX
#include "fuvect.hxx"
#endif
#include "stlpool.hxx"

// #90356#
#ifndef _SD_OPTSITEM_HXX
#include "optsitem.hxx"
#endif
#include "sdabstdlg.hxx"
#include "new_foil.hrc"

#ifndef _COM_SUN_STAR_DRAWING_XMASTERPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGES_HPP_
#include <com/sun/star/drawing/XDrawPages.hpp>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sd {

/*************************************************************************
|*
|* modal dialog for #90356#
|*
\************************************************************************/

class ImpUndoDeleteWarning : public ModalDialog
{
private:
    FixedImage      maImage;
    FixedText       maWarningFT;
    CheckBox        maDisableCB;
    OKButton        maYesBtn;
    CancelButton    maNoBtn;

public:
    ImpUndoDeleteWarning(Window* pParent);
    BOOL IsWarningDisabled() const { return maDisableCB.IsChecked(); }
};

ImpUndoDeleteWarning::ImpUndoDeleteWarning(Window* pParent)
:   ModalDialog(pParent, SdResId(RID_UNDO_DELETE_WARNING)),
    maImage(this, SdResId(IMG_UNDO_DELETE_WARNING)),
    maWarningFT(this, SdResId(FT_UNDO_DELETE_WARNING)),
    maDisableCB(this, SdResId(CB_UNDO_DELETE_DISABLE)),
    maYesBtn(this, SdResId(BTN_UNDO_DELETE_YES)),
    maNoBtn(this, SdResId(BTN_UNDO_DELETE_NO))
{
    FreeResource();

    SetHelpId( HID_SD_UNDODELETEWARNING_DLG );
    maDisableCB.SetHelpId( HID_SD_UNDODELETEWARNING_CBX );

    maYesBtn.SetText(Button::GetStandardText(BUTTON_YES));
    maNoBtn.SetText(Button::GetStandardText(BUTTON_NO));
    maImage.SetImage(WarningBox::GetStandardImage());

    // #93721# Set focus to YES-Button
    maYesBtn.GrabFocus();
}


/*************************************************************************
|*
|* SfxRequests fuer temporaere Funktionen
|*
\************************************************************************/

void DrawViewShell::FuTemporary(SfxRequest& rReq)
{
    // Waehrend einer Native-Diashow wird nichts ausgefuehrt!
    if(mpSlideShow &&
       rReq.GetSlot() != SID_NAVIGATOR)
        return;

    CheckLineTo (rReq);

    DeactivateCurrentFunction();

    USHORT nSId = rReq.GetSlot();

    // Slot wird gemapped (ToolboxImages/-Slots)
    MapSlot( nSId );

    switch ( nSId )
    {
        // Flaechen und Linien-Attribute:
        // Sollten (wie StateMethode) eine eigene
        // Execute-Methode besitzen
        case SID_ATTR_FILL_STYLE:
        case SID_ATTR_FILL_COLOR:
        case SID_ATTR_FILL_GRADIENT:
        case SID_ATTR_FILL_HATCH:
        case SID_ATTR_FILL_BITMAP:
        case SID_ATTR_FILL_SHADOW:

        case SID_ATTR_LINE_STYLE:
        case SID_ATTR_LINE_DASH:
        case SID_ATTR_LINE_WIDTH:
        case SID_ATTR_LINE_COLOR:
        case SID_ATTR_LINEEND_STYLE:

        case SID_ATTR_TEXT_FITTOSIZE:
        {
            if( rReq.GetArgs() )
            {
                BOOL bMergeUndo = FALSE;
                SfxUndoManager* pUndoMgr = GetDocSh()->GetUndoManager();

                // Anpassungen Start/EndWidth #63083#
                if(nSId == SID_ATTR_LINE_WIDTH)
                {
                    SdrObject* pObj = NULL;
                    const SdrMarkList& rMarkList = pDrView->GetMarkedObjectList();
                    ULONG nCount = rMarkList.GetMarkCount();

                    INT32 nNewLineWidth = ((const XLineWidthItem&)rReq.GetArgs()->Get(XATTR_LINEWIDTH)).GetValue();

                    for (ULONG i=0; i<nCount; i++)
                    {
                        SfxItemSet aAttr(GetDoc()->GetPool());
                        pObj = rMarkList.GetMark(i)->GetObj();
                        aAttr.Put(pObj->GetMergedItemSet());

                        INT32 nActLineWidth = ((const XLineWidthItem&)aAttr.Get(XATTR_LINEWIDTH)).GetValue();

                        if(nActLineWidth != nNewLineWidth)
                        {
                            BOOL bSetItemSet(FALSE);

                            // #86265# do this for SFX_ITEM_DEFAULT and for SFX_ITEM_SET
                            if(SFX_ITEM_DONTCARE != aAttr.GetItemState(XATTR_LINESTARTWIDTH))
                            {
                                INT32 nValAct = ((const XLineStartWidthItem&)aAttr.Get(XATTR_LINESTARTWIDTH)).GetValue();
                                INT32 nValNew = nValAct + (((nNewLineWidth - nActLineWidth) * 15) / 10);
                                if(nValNew < 0)
                                    nValNew = 0;
                                bSetItemSet = TRUE;
                                aAttr.Put(XLineStartWidthItem(nValNew));
                            }

                            // #86265# do this for SFX_ITEM_DEFAULT and for SFX_ITEM_SET
                            if(SFX_ITEM_DONTCARE != aAttr.GetItemState(XATTR_LINEENDWIDTH))
                            {
                                INT32 nValAct = ((const XLineEndWidthItem&)aAttr.Get(XATTR_LINEENDWIDTH)).GetValue();
                                INT32 nValNew = nValAct + (((nNewLineWidth - nActLineWidth) * 15) / 10);
                                if(nValNew < 0)
                                    nValNew = 0;
                                bSetItemSet = TRUE;
                                aAttr.Put(XLineEndWidthItem(nValNew));
                            }

                            if(bSetItemSet)
                                pObj->SetMergedItemSet(aAttr);
                        }
                    }
                }

                if (nSId == SID_ATTR_FILL_SHADOW)
                {
                    // Ggf. werden transparente Objekte wei?gefuellt
                    SdrObject* pObj = NULL;
                    const SdrMarkList& rMarkList = pDrView->GetMarkedObjectList();
                    ULONG nCount = rMarkList.GetMarkCount();

                    for (ULONG i=0; i<nCount; i++)
                    {
                        SfxItemSet aAttr(GetDoc()->GetPool());
                        pObj = rMarkList.GetMark(i)->GetObj();

                        // #i25616#
                        if(!pObj->ISA(SdrGrafObj))
                        {
                            aAttr.Put(pObj->GetMergedItemSet());

                            const XFillStyleItem& rFillStyle =
                            (const XFillStyleItem&) aAttr.Get(XATTR_FILLSTYLE);

                            if (rFillStyle.GetValue() == XFILL_NONE)
                            {
                                // Vorlage hat keine Fuellung,
                                // daher hart attributieren: Fuellung setzen
                                if (!bMergeUndo)
                                {
                                    bMergeUndo = TRUE;
                                    pUndoMgr->EnterListAction( String(), String() );
                                    pDrView->BegUndo();
                                }
                                pDrView->AddUndo(GetDoc()->GetSdrUndoFactory().CreateUndoAttrObject(*pObj));

                                aAttr.Put(XFillStyleItem(XFILL_SOLID));
                                aAttr.Put(XFillColorItem(String(), COL_WHITE));

                                pObj->SetMergedItemSet(aAttr);
                            }
                        }
                    }

                    if (bMergeUndo)
                    {
                        pDrView->EndUndo();
                    }
                }

                pDrView->SetAttributes(*rReq.GetArgs());

                if (bMergeUndo)
                {
                    pUndoMgr->LeaveListAction();
                }

                rReq.Done();
            }
            else
            {
                switch( rReq.GetSlot() )
                {
                    case SID_ATTR_FILL_SHADOW:
                    case SID_ATTR_FILL_STYLE:
                    case SID_ATTR_FILL_COLOR:
                    case SID_ATTR_FILL_GRADIENT:
                    case SID_ATTR_FILL_HATCH:
                    case SID_ATTR_FILL_BITMAP:
                        GetViewFrame()->GetDispatcher()->Execute( SID_ATTRIBUTES_AREA, SFX_CALLMODE_ASYNCHRON );
                        break;
                    case SID_ATTR_LINE_STYLE:
                    case SID_ATTR_LINE_DASH:
                    case SID_ATTR_LINE_WIDTH:
                    case SID_ATTR_LINE_COLOR:
                        GetViewFrame()->GetDispatcher()->Execute( SID_ATTRIBUTES_LINE, SFX_CALLMODE_ASYNCHRON );
                        break;
                    case SID_ATTR_TEXT_FITTOSIZE:
                        GetViewFrame()->GetDispatcher()->Execute( SID_TEXTATTR_DLG, SFX_CALLMODE_ASYNCHRON );
                        break;
                }
            }
            Cancel();
        }
        break;

        case SID_HYPHENATION:
        {
            // const SfxPoolItem* pItem = rReq.GetArg( SID_HYPHENATION );
            //  ^-- Soll so nicht benutzt werden (Defaults sind falsch) !
            SFX_REQUEST_ARG( rReq, pItem, SfxBoolItem, SID_HYPHENATION, FALSE);

            if( pItem )
            {
                SfxItemSet aSet( GetPool(), EE_PARA_HYPHENATE, EE_PARA_HYPHENATE );
                BOOL bValue = ( (const SfxBoolItem*) pItem)->GetValue();
                aSet.Put( SfxBoolItem( EE_PARA_HYPHENATE, bValue ) );
                pDrView->SetAttributes( aSet );
            }
            else // nur zum Test
            {
                DBG_ERROR(" Kein Wert fuer Silbentrennung!");
                SfxItemSet aSet( GetPool(), EE_PARA_HYPHENATE, EE_PARA_HYPHENATE );
                BOOL bValue = TRUE;
                aSet.Put( SfxBoolItem( EE_PARA_HYPHENATE, bValue ) );
                pDrView->SetAttributes( aSet );
            }
            rReq.Done();
            Cancel();
        }
        break;

        case SID_INSERTPAGE:
        case SID_INSERTPAGE_QUICK:
        case SID_DUPLICATE_PAGE:
        {
            SdPage* pNewPage = CreateOrDuplicatePage (rReq, ePageKind, GetActualPage());
            Cancel();
            if(HasCurrentFunction(SID_BEZIER_EDIT) )
                GetViewFrame()->GetDispatcher()->Execute(SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);
            if (pNewPage != NULL)
                SwitchPage((pNewPage->GetPageNum()-1)/2);
            rReq.Done ();
        }
        break;

        case SID_INSERT_MASTER_PAGE:
        {
            // Use the API to create a new page.
            Reference<drawing::XMasterPagesSupplier> xMasterPagesSupplier (
                GetDoc()->getUnoModel(), UNO_QUERY);
            if (xMasterPagesSupplier.is())
            {
                Reference<drawing::XDrawPages> xMasterPages (
                    xMasterPagesSupplier->getMasterPages());
                if (xMasterPages.is())
                {
                    USHORT nIndex = GetCurPageId();
                    xMasterPages->insertNewByIndex (nIndex);

                    // Create shapes for the default layout.
                    SdPage* pMasterPage = GetDoc()->GetMasterSdPage(
                        nIndex, PK_STANDARD);
                    pMasterPage->CreateTitleAndLayout (TRUE,TRUE);
                }
            }

            Cancel();
            if(HasCurrentFunction(SID_BEZIER_EDIT))
                GetViewFrame()->GetDispatcher()->Execute(
                    SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);
            rReq.Done ();
        }
        break;

        case SID_MODIFYPAGE:
        {
            if (ePageKind==PK_STANDARD || ePageKind==PK_NOTES ||
                (ePageKind==PK_HANDOUT && eEditMode==EM_MASTERPAGE) )
            {
                if ( pDrView->IsTextEdit() )
                {
                    pDrView->EndTextEdit();
                }
                USHORT nPage = aTabControl.GetCurPageId() - 1;
                pActualPage = GetDoc()->GetSdPage(nPage, ePageKind);
                mpImpl->ProcessModifyPageSlot (
                    rReq,
                    pActualPage,
                    ePageKind);
            }

            Cancel();
            rReq.Done ();
        }
        break;

        case SID_ASSIGN_LAYOUT:
        {
            if (ePageKind==PK_STANDARD
                || ePageKind==PK_NOTES
                || (ePageKind==PK_HANDOUT && eEditMode==EM_MASTERPAGE))
            {
                if ( pDrView->IsTextEdit() )
                    pDrView->EndTextEdit();

                SFX_REQUEST_ARG (rReq, pWhatPage, SfxUInt32Item, ID_VAL_WHATPAGE, FALSE);
                SFX_REQUEST_ARG (rReq, pWhatLayout, SfxUInt32Item, ID_VAL_WHATLAYOUT, FALSE);
                mpImpl->AssignLayout (
                    GetDoc()->GetSdPage((USHORT)pWhatPage->GetValue(), ePageKind),
                    (AutoLayout)pWhatLayout->GetValue());
            }
            Cancel();
            rReq.Done ();
        }
        break;

        case SID_RENAMEPAGE:
        case SID_RENAME_MASTER_PAGE:
        {
            if (ePageKind==PK_STANDARD || ePageKind==PK_NOTES )
            {
                if ( pDrView->IsTextEdit() )
                {
                    pDrView->EndTextEdit();
                }

                USHORT nPageId = aTabControl.GetCurPageId();
                SdPage* pCurrentPage = ( GetEditMode() == EM_PAGE )
                    ? GetDoc()->GetSdPage( nPageId - 1, GetPageKind() )
                    : GetDoc()->GetMasterSdPage( nPageId - 1, GetPageKind() );

                String aTitle( SdResId( STR_TITLE_RENAMESLIDE ) );
                String aDescr( SdResId( STR_DESC_RENAMESLIDE ) );
                String aPageName = pCurrentPage->GetName();

                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "Dialogdiet fail!");
                AbstractSvxNameDialog* aNameDlg = pFact->CreateSvxNameDialog( GetActiveWindow(), aPageName, aDescr, ResId(RID_SVXDLG_NAME) );
                DBG_ASSERT(aNameDlg, "Dialogdiet fail!");
                aNameDlg->SetText( aTitle );
                aNameDlg->SetCheckNameHdl( LINK( this, DrawViewShell, RenameSlideHdl ), true );
                aNameDlg->SetEditHelpId( HID_SD_NAMEDIALOG_PAGE );

                if( aNameDlg->Execute() == RET_OK )
                {
                    String aNewName;
                    aNameDlg->GetName( aNewName );
                    if( ! aNewName.Equals( aPageName ) )
                    {
                        bool bResult = RenameSlide( nPageId, aNewName );
                        DBG_ASSERT( bResult, "Couldn't rename slide" );
                    }
                }
                delete aNameDlg;
            }

            Cancel();
            rReq.Ignore ();
        }
        break;

        case SID_RENAMEPAGE_QUICK:
        {
            if (ePageKind==PK_STANDARD || ePageKind==PK_NOTES )
            {
                if ( pDrView->IsTextEdit() )
                {
                    pDrView->EndTextEdit();
                }

                aTabControl.StartEditMode( aTabControl.GetCurPageId() );
            }

            Cancel();
            rReq.Ignore ();
        }
        break;

        case SID_PAGESIZE :  // entweder dieses (kein menueeintrag o. ae. !!)
        {
            const SfxItemSet *pArgs = rReq.GetArgs ();

            if (pArgs)
                if (pArgs->Count () == 3)
                {
                    SFX_REQUEST_ARG (rReq, pWidth, SfxUInt32Item, ID_VAL_PAGEWIDTH, FALSE);
                    SFX_REQUEST_ARG (rReq, pHeight, SfxUInt32Item, ID_VAL_PAGEHEIGHT, FALSE);
                    SFX_REQUEST_ARG (rReq, pScaleAll, SfxBoolItem, ID_VAL_SCALEOBJECTS, FALSE);

                    Size aSize (pWidth->GetValue (), pHeight->GetValue ());

                    SetupPage (aSize, 0, 0, 0, 0, TRUE, FALSE, pScaleAll->GetValue ());
                    rReq.Ignore ();
                    break;
                }

            StarBASIC::FatalError (SbERR_WRONG_ARGS);
            rReq.Ignore ();
            break;
        }

        case SID_PAGEMARGIN :  // oder dieses (kein menueeintrag o. ae. !!)
        {
            const SfxItemSet *pArgs = rReq.GetArgs ();

            if (pArgs)
                if (pArgs->Count () == 5)
                {
                    SFX_REQUEST_ARG (rReq, pLeft, SfxUInt32Item, ID_VAL_PAGELEFT, FALSE);
                    SFX_REQUEST_ARG (rReq, pRight, SfxUInt32Item, ID_VAL_PAGERIGHT, FALSE);
                    SFX_REQUEST_ARG (rReq, pUpper, SfxUInt32Item, ID_VAL_PAGETOP, FALSE);
                    SFX_REQUEST_ARG (rReq, pLower, SfxUInt32Item, ID_VAL_PAGEBOTTOM, FALSE);
                    SFX_REQUEST_ARG (rReq, pScaleAll, SfxBoolItem, ID_VAL_SCALEOBJECTS, FALSE);

                    Size aEmptySize (0, 0);

                    SetupPage (aEmptySize, pLeft->GetValue (), pRight->GetValue (),
                               pUpper->GetValue (), pLower->GetValue (),
                               FALSE, TRUE, pScaleAll->GetValue ());
                    rReq.Ignore ();
                    break;
                }

            StarBASIC::FatalError (SbERR_WRONG_ARGS);
            rReq.Ignore ();
            break;
        }

        case SID_ZOOMING :  // kein Menueintrag, sondern aus dem Zoomdialog generiert
        {
            const SfxItemSet* pArgs = rReq.GetArgs();

            if (pArgs)
                if (pArgs->Count () == 1)
                {
                    SFX_REQUEST_ARG (rReq, pScale, SfxUInt32Item, ID_VAL_ZOOM, FALSE);
                    if (CHECK_RANGE (10, pScale->GetValue (), 1000))
                    {
                        SetZoom (pScale->GetValue ());

                        SfxBindings& rBindings = GetViewFrame()->GetBindings();
                        rBindings.Invalidate( SID_ATTR_ZOOM );
                        rBindings.Invalidate( SID_ZOOM_IN );
                        rBindings.Invalidate( SID_ZOOM_OUT );
                    }
                    else StarBASIC::FatalError (SbERR_BAD_PROP_VALUE);

                    rReq.Ignore ();
                    break;
                }

            StarBASIC::FatalError (SbERR_WRONG_ARGS);
            rReq.Ignore ();
            break;
        }

        case SID_ATTR_ZOOM:
        {
            const SfxItemSet* pArgs = rReq.GetArgs();
            bZoomOnPage = FALSE;

            if ( pArgs )
            {
                SvxZoomType eZT = ( ( const SvxZoomItem& ) pArgs->
                                            Get( SID_ATTR_ZOOM ) ).GetType();
                switch( eZT )
                {
                    case SVX_ZOOM_PERCENT:
                        SetZoom( (long) ( ( const SvxZoomItem& ) pArgs->
                                            Get( SID_ATTR_ZOOM ) ).GetValue() );
                        break;

                    case SVX_ZOOM_OPTIMAL:
                        GetViewFrame()->GetDispatcher()->Execute( SID_SIZE_ALL,
                                    SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
                        break;

                    case SVX_ZOOM_PAGEWIDTH:
                        GetViewFrame()->GetDispatcher()->Execute( SID_SIZE_PAGE_WIDTH,
                                    SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
                        break;

                    case SVX_ZOOM_WHOLEPAGE:
                        GetViewFrame()->GetDispatcher()->Execute( SID_SIZE_PAGE,
                                    SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
                        break;
                }
                rReq.Ignore ();
            }
            else
            {
                // hier den Zoom-Dialog oeffnen
                SetCurrentFunction( FuScale::Create( this, GetActiveWindow(), pDrView, GetDoc(), rReq ) );
            }
            Cancel();
        }
        break;

        case SID_CHANGEBEZIER:
        case SID_CHANGEPOLYGON:
            if ( pDrView->IsTextEdit() )
            {
                pDrView->EndTextEdit();
                GetViewFrame()->GetDispatcher()->Execute(SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);
            }

            if ( pDrView->IsPresObjSelected() )
            {
                ::sd::Window* pWindow = GetActiveWindow();
                InfoBox(pWindow, String(SdResId(STR_ACTION_NOTPOSSIBLE) ) ).Execute();
            }
            else
            {
                if( rReq.GetSlot() == SID_CHANGEBEZIER )
                {
                    WaitObject aWait( (Window*)GetActiveWindow() );
                    pDrView->ConvertMarkedToPathObj(FALSE);
                }
                else
                {
                    if( pDrView->IsVectorizeAllowed() )
                        SetCurrentFunction( FuVectorize::Create( this, GetActiveWindow(), pDrView, GetDoc(), rReq ) );
                    else
                    {
                        WaitObject aWait( (Window*)GetActiveWindow() );
                        pDrView->ConvertMarkedToPolyObj(FALSE);
                    }
                }

                Invalidate(SID_CHANGEBEZIER);
                Invalidate(SID_CHANGEPOLYGON);
            }
            Cancel();

            if( HasCurrentFunction(SID_BEZIER_EDIT) )
            {   // ggf. die richtige Editfunktion aktivieren
                GetViewFrame()->GetDispatcher()->Execute(SID_SWITCH_POINTEDIT,
                                        SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
            }
            rReq.Ignore ();
            break;

        case SID_CONVERT_TO_CONTOUR:
            if ( pDrView->IsTextEdit() )
            {
                pDrView->EndTextEdit();
                GetViewFrame()->GetDispatcher()->Execute(SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);
            }

            if ( pDrView->IsPresObjSelected() )
            {
                ::sd::Window* pWindow = GetActiveWindow();
                InfoBox(pWindow, String(SdResId(STR_ACTION_NOTPOSSIBLE) ) ).Execute();
            }
            else
            {
                WaitObject aWait( (Window*)GetActiveWindow() );
                pDrView->ConvertMarkedToPolyObj(TRUE);

                Invalidate(SID_CONVERT_TO_CONTOUR);
            }
            Cancel();

            rReq.Ignore ();
            break;

        case SID_CONVERT_TO_METAFILE:
        case SID_CONVERT_TO_BITMAP:
        {
            // End text edit mode when it is active because the metafile or
            // bitmap that will be created does not support it.
            if ( pDrView->IsTextEdit() )
            {
                pDrView->EndTextEdit();
                GetViewFrame()->GetDispatcher()->Execute(SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);
            }

            if ( pDrView->IsPresObjSelected(true,true,true) )
            {
                ::sd::Window* pWindow = GetActiveWindow();
                InfoBox(pWindow, String(SdResId(STR_ACTION_NOTPOSSIBLE) ) ).Execute();
            }
            else
            {
                WaitObject aWait( (Window*)GetActiveWindow() );

                // switch on undo for the next operations
                pDrView->BegUndo(
                    String(
                    SdResId (nSId==SID_CONVERT_TO_METAFILE ? STR_UNDO_CONVERT_TO_METAFILE : STR_UNDO_CONVERT_TO_BITMAP)));

                // create SdrGrafObj from metafile/bitmap
                Graphic aGraphic;
                switch (nSId)
                {
                    case SID_CONVERT_TO_METAFILE:
                    {
                        GDIMetaFile aMetaFile(pDrView->GetAllMarkedMetaFile ());
                        aGraphic = Graphic(aMetaFile);
                    }
                    break;
                    case SID_CONVERT_TO_BITMAP:
                    {
                        Bitmap aBitmap (pDrView->GetAllMarkedBitmap ());
                        aGraphic = Graphic(aBitmap);
                    }
                    break;
                }

                SdrGrafObj* pGraphicObj = new SdrGrafObj (aGraphic);

                // fit rectangle of graphic-object to  mark-rect
                Rectangle aAllMarkedRect;
                SdrMarkList aMarkList = pDrView->GetMarkedObjectList();
                for (int i=0; i < (int) aMarkList.GetMarkCount(); i++)
                {
                    aAllMarkedRect.Union ( aMarkList.GetMark(i)->GetObj()->GetCurrentBoundRect() );
                }
                pGraphicObj->SetLogicRect (aAllMarkedRect);

                // get page-view
                SdrPageView* pPageView = pDrView->GetMarkedObjectList().GetMark(0)->GetPageView();

                // delete marked objects
                pDrView->DeleteMarkedObj(); // #69979# delete the objects, not only the marked area

                // insert new object
                pDrView->InsertObject (pGraphicObj, *pPageView);

                // switch off undo
                pDrView->EndUndo();
            }
        }

        Cancel();

        rReq.Done ();
        break;

        case SID_SET_DEFAULT:
        {
            SfxItemSet* pSet = NULL;

            if (pDrView->IsTextEdit())
            {
                ::Outliner* pOutl = pDrView->GetTextEditOutliner();
                if (pOutl)
                {
                    pOutl->RemoveFields(TRUE, (TypeId) SvxURLField::StaticType());
                }

                pSet = new SfxItemSet( GetPool(), EE_ITEMS_START, EE_ITEMS_END );
                pDrView->SetAttributes( *pSet, TRUE );
            }
            else
            {
                const SdrMarkList& rMarkList = pDrView->GetMarkedObjectList();
                ULONG nCount = rMarkList.GetMarkCount();

                // In diese Liste werden fuer jedes Praesentationsobjekt ein SfxItemSet
                // der harten Attribute sowie der UserCall eingetragen, da diese beim nachfolgenden
                // pDrView->SetAttributes( *pSet, TRUE ) verloren gehen und spaeter restauriert
                // werden muessen
                List* pAttrList = new List();
                SdPage* pPresPage = (SdPage*) pDrView->GetPageViewPvNum(0)->GetPage();
                ULONG i;

                for ( i = 0; i < nCount; i++ )
                {
                    SdrObject* pObj = rMarkList.GetMark(i)->GetObj();

                    if( pPresPage->IsPresObj( pObj ) )
                    {
                        SfxItemSet* pSet = new SfxItemSet( GetDoc()->GetPool(), SDRATTR_TEXT_MINFRAMEHEIGHT, SDRATTR_TEXT_AUTOGROWHEIGHT, 0 );
                        pSet->Put(pObj->GetMergedItemSet());
                        pAttrList->Insert( pSet, LIST_APPEND );
                        pAttrList->Insert( pObj->GetUserCall(), LIST_APPEND );
                    }
                }

                pSet = new SfxItemSet( GetPool() );
                pDrView->SetAttributes( *pSet, TRUE );

                ULONG j = 0;

                for ( i = 0; i < nCount; i++ )
                {
                    SfxStyleSheet* pSheet = NULL;
                    SdrObject* pObj = rMarkList.GetMark(i)->GetObj();

                    if (pObj->GetObjIdentifier() == OBJ_TITLETEXT)
                    {
                        pSheet = pActualPage->GetStyleSheetForPresObj(PRESOBJ_TITLE);
                        if (pSheet)
                            pObj->SetStyleSheet(pSheet, FALSE);
                    }
                    else if(pObj->GetObjIdentifier() == OBJ_OUTLINETEXT)
                    {
                        for (USHORT nLevel = 1; nLevel < 10; nLevel++)
                        {
                            pSheet = pActualPage->GetStyleSheetForPresObj( PRESOBJ_OUTLINE );
                            DBG_ASSERT(pSheet, "Vorlage fuer Gliederungsobjekt nicht gefunden");
                            if (pSheet)
                            {
                                pObj->StartListening(*pSheet);

                                if( nLevel == 1 )
                                    // Textrahmen hoert auf StyleSheet der Ebene1
                                    pObj->NbcSetStyleSheet(pSheet, FALSE);

                            }
                        }
                    }

                    if( pPresPage->IsPresObj( pObj ) )
                    {
                        SfxItemSet* pSet = (SfxItemSet*) pAttrList->GetObject(j++);
                        SdrObjUserCall* pUserCall = (SdrObjUserCall*) pAttrList->GetObject(j++);

                        if ( pSet && pSet->GetItemState( SDRATTR_TEXT_MINFRAMEHEIGHT ) == SFX_ITEM_ON )
                        {
                            pObj->SetMergedItem(pSet->Get(SDRATTR_TEXT_MINFRAMEHEIGHT));
                        }

                        if ( pSet && pSet->GetItemState( SDRATTR_TEXT_AUTOGROWHEIGHT ) == SFX_ITEM_ON )
                        {
                            pObj->SetMergedItem(pSet->Get(SDRATTR_TEXT_AUTOGROWHEIGHT));
                        }

                        if( pUserCall )
                            pObj->SetUserCall( pUserCall );

                        delete pSet;
                    }
                }

                delete pAttrList;
            }

            delete pSet;
            Cancel();
        }
        break;

        case SID_DELETE_SNAPITEM:
        {
            SdrPageView* pPV;
            Point   aMPos = GetActiveWindow()->PixelToLogic( aMousePos );
            USHORT  nHitLog = (USHORT) GetActiveWindow()->PixelToLogic( Size(
                FuPoor::HITPIX, 0 ) ).Width();
            USHORT  nHelpLine;

            bMousePosFreezed = FALSE;

            if( pDrView->PickHelpLine( aMPos, nHitLog, *GetActiveWindow(), nHelpLine, pPV) )
            {
                pPV->DeleteHelpLine( nHelpLine );
            }
            Cancel();
            rReq.Ignore ();
        }
        break;

        case SID_DELETE_PAGE:
        case SID_DELETE_MASTER_PAGE:
            DeleteActualPage();
            Cancel();
            rReq.Ignore ();
        break;

        case SID_DELETE_LAYER:
            DeleteActualLayer();
            Cancel();
            rReq.Ignore ();
        break;

        case SID_ORIGINAL_SIZE:
            pDrView->SetMarkedOriginalSize();
            Cancel();
            rReq.Done();
        break;

        case SID_DRAW_FONTWORK:
        case SID_DRAW_FONTWORK_VERTICAL:
        {
            svx::FontworkBar::execute( mpView, rReq, GetViewFrame()->GetBindings() );       // SJ: can be removed  (I think)
            Cancel();
            rReq.Done();
        }
        break;

        case SID_SAVEGRAPHIC:
        {
            const SdrMarkList& rMarkList = pDrView->GetMarkedObjectList();
            if( rMarkList.GetMarkCount() == 1 )
            {
                SdrGrafObj *pGrafObj = dynamic_cast< SdrGrafObj* >( rMarkList.GetMark( 0 )->GetObj() );
                if(pGrafObj )
                {
                    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape( pGrafObj->getUnoShape(), com::sun::star::uno::UNO_QUERY );
                    SdGRFFilter::SaveGraphic( xShape );
                }
            }
            Cancel();
            rReq.Ignore();
        }
        break;

        default:
        {
            // switch Anweisung wegen CLOOKS aufgeteilt. Alle case-Anweisungen die
            // eine Fu???? -Funktion aufrufen, sind in die Methode FuTemp01 (drviews8)
            // gewandert.
            FuTemp01(rReq);
        }
        break;
    }

    if(HasCurrentFunction())
    {
        GetCurrentFunction()->Activate();
    }
}




/** This method consists basically of three parts:
    1. Process the arguments of the SFX request.
    2. Use the model to create a new page or duplicate an existing one.
    3. Update the tab control and switch to the new page.
*/
SdPage* DrawViewShell::CreateOrDuplicatePage (
    SfxRequest& rRequest,
    PageKind ePageKind,
    SdPage* pPage)
{
    SdPage* pNewPage = NULL;
    if (ePageKind == PK_STANDARD && eEditMode != EM_MASTERPAGE)
    {
        if ( pDrView->IsTextEdit() )
        {
            pDrView->EndTextEdit();
        }
        pNewPage = ViewShell::CreateOrDuplicatePage (rRequest, ePageKind, pPage);
    }
    return pNewPage;
}

} // end of namespace sd
