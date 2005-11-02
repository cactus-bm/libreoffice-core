/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: editsh.cxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: kz $ $Date: 2005-11-02 17:39:08 $
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

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

//------------------------------------------------------------------

#include "scitems.hxx"
#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD

#include <svx/clipfmtitem.hxx>
#include <svx/svxdlg.hxx>
#include <svx/cntritem.hxx>
//CHINA001 #include <svx/chardlg.hxx>
#include <svx/crsditem.hxx>
#include <svx/editeng.hxx>
#include <svx/editview.hxx>
#include <svx/escpitem.hxx>
#include <svx/flditem.hxx>
#include <svx/fontitem.hxx>
#include <svx/hlnkitem.hxx>
#include <svx/postitem.hxx>
#include <svx/scripttypeitem.hxx>
#include <svx/shdditem.hxx>
#include <svx/srchitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/wghtitem.hxx>
#include <sfx2/basedlgs.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/msg.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/request.hxx>
#include <sfx2/viewfrm.hxx>
#include <sot/exchange.hxx>
#include <svtools/cjkoptions.hxx>
#include <svtools/cliplistener.hxx>
#include <svtools/whiter.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/sound.hxx>
#include <sot/formats.hxx>
#include <svtools/transfer.hxx>

#define _EDITSH_CXX
#include "editsh.hxx"

#include "scresid.hxx"
#include "global.hxx"
#include "sc.hrc"
#include "scmod.hxx"
#include "inputhdl.hxx"
#include "viewutil.hxx"
#include "viewdata.hxx"
#include "document.hxx"
//CHINA001 #include "namepast.hxx"
#include "reffind.hxx"
#include "tabvwsh.hxx"
//CHINA001 #include "textdlgs.hxx"
#include "editutil.hxx"
#include "globstr.hrc"

#define ScEditShell
#include "scslots.hxx"

#include "scui_def.hxx" //CHINA001
#include "scabstdlg.hxx" //CHINA001
TYPEINIT1( ScEditShell, SfxShell );

SFX_IMPL_INTERFACE(ScEditShell, SfxShell, ScResId(SCSTR_EDITSHELL))
{
    SFX_POPUPMENU_REGISTRATION( ScResId(RID_POPUP_EDIT) );
    SFX_OBJECTMENU_REGISTRATION( SID_OBJECTMENU0, ScResId(RID_OBJECTMENU_EDIT) );
}


ScEditShell::ScEditShell(EditView* pView, ScViewData* pData) :
    pEditView       (pView),
    pViewData       (pData),
    pClipEvtLstnr   (NULL),
    bPastePossible  (FALSE),
    bIsInsertMode   (TRUE)
{
    SetPool( pEditView->GetEditEngine()->GetEmptyItemSet().GetPool() );
    SetUndoManager( &pEditView->GetEditEngine()->GetUndoManager() );
    SetName(String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("EditCell")));
}

ScEditShell::~ScEditShell()
{
    if ( pClipEvtLstnr )
    {
        pClipEvtLstnr->AddRemoveListener( pViewData->GetActiveWin(), FALSE );
        pClipEvtLstnr->release();
    }
}

ScInputHandler* ScEditShell::GetMyInputHdl()
{
    return SC_MOD()->GetInputHdl( pViewData->GetViewShell() );
}

void ScEditShell::SetEditView(EditView* pView)
{
    pEditView = pView;
    pEditView->SetInsertMode( bIsInsertMode );
    SetPool( pEditView->GetEditEngine()->GetEmptyItemSet().GetPool() );
    SetUndoManager( &pEditView->GetEditEngine()->GetUndoManager() );
}

void lcl_RemoveAttribs( EditView& rEditView )
{
    ScEditEngineDefaulter* pEngine = static_cast<ScEditEngineDefaulter*>(rEditView.GetEditEngine());

    BOOL bOld = pEngine->GetUpdateMode();
    pEngine->SetUpdateMode(FALSE);

    String aName = ScGlobal::GetRscString( STR_UNDO_DELETECONTENTS );
    pEngine->GetUndoManager().EnterListAction( aName, aName );

    rEditView.RemoveAttribs(TRUE);
    pEngine->RepeatDefaults();      // #97226# paragraph attributes from cell formats must be preserved

    pEngine->GetUndoManager().LeaveListAction();

    pEngine->SetUpdateMode(bOld);
}

void ScEditShell::Execute( SfxRequest& rReq )
{
    const SfxItemSet*   pReqArgs    = rReq.GetArgs();
    USHORT              nSlot       = rReq.GetSlot();
    SfxBindings&        rBindings   = pViewData->GetBindings();

    ScInputHandler* pHdl = GetMyInputHdl();
    DBG_ASSERT(pHdl,"kein ScInputHandler");

    EditView* pTopView   = pHdl->GetTopView();      // hat Eingabezeile den Focus?
    EditView* pTableView = pHdl->GetTableView();

    DBG_ASSERT(pTableView,"keine EditView :-(");

    EditEngine* pEngine = pTableView->GetEditEngine();

    pHdl->DataChanging();
    BOOL bSetSelIsRef = FALSE;

    switch ( nSlot )
    {
        case FID_INS_CELL_CONTENTS: // Insert-Taste, weil als Acc definiert
            bIsInsertMode = !pTableView->IsInsertMode();
            pTableView->SetInsertMode( bIsInsertMode );
            if (pTopView)
                pTopView->SetInsertMode( bIsInsertMode );
            rBindings.Invalidate( SID_ATTR_INSERT );
            break;

        case SID_ATTR_INSERT:
            if ( pReqArgs )
            {
                bIsInsertMode = ((const SfxBoolItem&)pReqArgs->Get(nSlot)).GetValue();
                pTableView->SetInsertMode( bIsInsertMode );
                if (pTopView)
                    pTopView->SetInsertMode( bIsInsertMode );
                rBindings.Invalidate( SID_ATTR_INSERT );
            }
            break;

        case SID_COPY:
            pTableView->Copy();
            break;

        case SID_CUT:
            pTableView->Cut();
            if (pTopView)
                pTopView->DeleteSelected();
            break;

        case SID_PASTE:
            pTableView->PasteSpecial();
            if (pTopView)
                pTopView->Paste();
            break;

        case SID_DELETE:
            pTableView->DeleteSelected();
            if (pTopView)
                pTopView->DeleteSelected();
            break;

        case SID_CELL_FORMAT_RESET:                 // "Standard"
            lcl_RemoveAttribs( *pTableView );
            if ( pTopView )
                lcl_RemoveAttribs( *pTopView );
            break;

        case SID_CLIPBOARD_FORMAT_ITEMS:
            {
                ULONG nFormat = 0;
                const SfxPoolItem* pItem;
                if ( pReqArgs &&
                     pReqArgs->GetItemState(nSlot, TRUE, &pItem) == SFX_ITEM_SET &&
                     pItem->ISA(SfxUInt32Item) )
                {
                    nFormat = ((const SfxUInt32Item*)pItem)->GetValue();
                }

                if ( nFormat )
                {
                    if (SOT_FORMAT_STRING == nFormat)
                        pTableView->Paste();
                    else
                        pTableView->PasteSpecial();

                    if (pTopView)
                        pTopView->Paste();
                }
            }
            break;

        case FID_PASTE_CONTENTS:
            {
                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                SfxAbstractPasteDialog* pDlg = pFact->CreatePasteDialog( pViewData->GetDialogParent() );
                ULONG nFormat = 0;
                if ( pDlg )
                {
                    pDlg->Insert( SOT_FORMAT_STRING, EMPTY_STRING );
                    pDlg->Insert( SOT_FORMAT_RTF,    EMPTY_STRING );

                    TransferableDataHelper aDataHelper(
                        TransferableDataHelper::CreateFromSystemClipboard( pViewData->GetActiveWin() ) );

                    nFormat = pDlg->GetFormat( aDataHelper.GetTransferable() );
                    DELETEZ(pDlg);
                }

                // while the dialog was open, edit mode may have been stopped
                if (!SC_MOD()->IsInputMode())
                {
                    Sound::Beep();
                    return;
                }

                if (nFormat > 0)
                {
                    if (SOT_FORMAT_STRING == nFormat)
                        pTableView->Paste();
                    else
                        pTableView->PasteSpecial();

                    if (pTopView)
                        pTopView->Paste();
                }

                if (pTopView)
                    pTopView->GetWindow()->GrabFocus();
            }
            break;

        case SID_SELECTALL:
            {
                USHORT nPar = pEngine->GetParagraphCount();
                if (nPar)
                {
                    xub_StrLen nLen = pEngine->GetTextLen(nPar-1);
                    pTableView->SetSelection(ESelection(0,0,nPar-1,nLen));
                    if (pTopView)
                        pTopView->SetSelection(ESelection(0,0,nPar-1,nLen));
                }
            }
            break;

        case SID_CHARMAP:
            {
                USHORT nScript = pTableView->GetSelectedScriptType();
                USHORT nFontWhich = ( nScript == SCRIPTTYPE_ASIAN ) ? EE_CHAR_FONTINFO_CJK :
                                ( ( nScript == SCRIPTTYPE_COMPLEX ) ? EE_CHAR_FONTINFO_CTL :
                                                                        EE_CHAR_FONTINFO );
                const SvxFontItem& rItem = (const SvxFontItem&)
                            pTableView->GetAttribs().Get(nFontWhich);

                String aString;
                SvxFontItem aNewItem( EE_CHAR_FONTINFO );

                BOOL bOk = ScViewUtil::ExecuteCharMap( rItem, aNewItem, aString );

                // while the dialog was open, edit mode may have been stopped
                if (!SC_MOD()->IsInputMode())
                {
                    Sound::Beep();
                    return;
                }

                if (bOk)
                {
                    //  if string contains WEAK characters, set all fonts
                    BYTE nScript;
                    ScDocument* pDoc = pViewData->GetDocument();
                    if ( pDoc->HasStringWeakCharacters( aString ) )
                        nScript = SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN | SCRIPTTYPE_COMPLEX;
                    else
                        nScript = pDoc->GetStringScriptType( aString );

                    SfxItemSet aSet( pTableView->GetEmptyItemSet() );
                    SvxScriptSetItem aSetItem( SID_ATTR_CHAR_FONT, GetPool() );
                    aSetItem.PutItemForScriptType( nScript, aNewItem );
                    aSet.Put( aSetItem.GetItemSet(), FALSE );

                    //  SetAttribs an der View selektiert ein Wort, wenn nichts selektiert ist
                    pTableView->GetEditEngine()->QuickSetAttribs( aSet, pTableView->GetSelection() );
                    pTableView->InsertText(aString);
                    if (pTopView)
                        pTopView->InsertText(aString);
                }

                if (pTopView)
                    pTopView->GetWindow()->GrabFocus();
            }
            break;

        case FID_INSERT_NAME:
            {
                ScDocument*     pDoc = pViewData->GetDocument();
                //CHINA001 ScNamePasteDlg* pDlg = new ScNamePasteDlg( pViewData->GetDialogParent(),
                //CHINA001                              pDoc->GetRangeName(), FALSE );
                                                // "Liste" disablen
                ScAbstractDialogFactory* pFact = ScAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "ScAbstractFactory create fail!");//CHINA001

                AbstractScNamePasteDlg* pDlg = pFact->CreateScNamePasteDlg( pViewData->GetDialogParent(), pDoc->GetRangeName(), ResId(RID_SCDLG_NAMES_PASTE), FALSE );
                DBG_ASSERT(pDlg, "Dialog create fail!");//CHINA001
                short nRet = pDlg->Execute();
                // pDlg is needed below

                // while the dialog was open, edit mode may have been stopped
                if (!SC_MOD()->IsInputMode())
                {
                    Sound::Beep();
                    delete pDlg;
                    return;
                }

                if ( nRet == BTN_PASTE_NAME )
                {
                    String aName = pDlg->GetSelectedName();
                    pTableView->InsertText(aName);
                    if (pTopView)
                        pTopView->InsertText(aName);
                }
                delete pDlg;

                if (pTopView)
                    pTopView->GetWindow()->GrabFocus();
            }
            break;

        case SID_CHAR_DLG:
            {
                SfxItemSet aAttrs( pTableView->GetAttribs() );

                SfxObjectShell* pObjSh = pViewData->GetSfxDocShell();

                //CHINA001 ScCharDlg* pDlg = new ScCharDlg( pViewData->GetDialogParent(), &aAttrs, pObjSh );
                ScAbstractDialogFactory* pFact = ScAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "ScAbstractFactory create fail!");//CHINA001

                SfxAbstractTabDialog* pDlg = pFact->CreateScCharDlg( pViewData->GetDialogParent(), &aAttrs,
                                                                     pObjSh,ResId(RID_SCDLG_CHAR) );
                DBG_ASSERT(pDlg, "Dialog create fail!");//CHINA001
                short nRet = pDlg->Execute();
                // pDlg is needed below

                // while the dialog was open, edit mode may have been stopped
                if (!SC_MOD()->IsInputMode())
                {
                    Sound::Beep();
                    delete pDlg;
                    return;
                }

                if ( nRet == RET_OK )
                {
                    const SfxItemSet* pOut = pDlg->GetOutputItemSet();
                    pTableView->SetAttribs( *pOut );
                }
                delete pDlg;
            }
            break;

        case SID_TOGGLE_REL:
            {
                BOOL bOk = FALSE;
                if (pEngine->GetParagraphCount() == 1)
                {
                    String aText = pEngine->GetText();
                    ESelection aSel = pEditView->GetSelection();    // aktuelle View

                    ScRefFinder aFinder( aText, pViewData->GetDocument() );
                    aFinder.ToggleRel( aSel.nStartPos, aSel.nEndPos );
                    if (aFinder.GetFound())
                    {
                        String aNew = aFinder.GetText();
                        ESelection aSel( 0,aFinder.GetSelStart(), 0,aFinder.GetSelEnd() );
                        pEngine->SetText( aNew );
                        pTableView->SetSelection( aSel );
                        if ( pTopView )
                        {
                            pTopView->GetEditEngine()->SetText( aNew );
                            pTopView->SetSelection( aSel );
                        }
                        bOk = TRUE;

                        //  Referenz wird selektiert -> beim Tippen nicht ueberschreiben
                        bSetSelIsRef = TRUE;
                    }
                }
                if (!bOk)
                    Sound::Beep();              // keine Referenzen oder mehrere Absaetze
            }
            break;

        case SID_HYPERLINK_SETLINK:
            if( pReqArgs )
            {
                const SfxPoolItem* pItem;
                if ( pReqArgs->GetItemState( SID_HYPERLINK_SETLINK, TRUE, &pItem ) == SFX_ITEM_SET )
                {
                    const SvxHyperlinkItem* pHyper = (const SvxHyperlinkItem*) pItem;
                    const String& rName     = pHyper->GetName();
                    const String& rURL      = pHyper->GetURL();
                    const String& rTarget   = pHyper->GetTargetFrame();
                    SvxLinkInsertMode eMode = pHyper->GetInsertMode();

                    BOOL bDone = FALSE;
                    if ( eMode == HLINK_DEFAULT || eMode == HLINK_FIELD )
                    {
                        const SvxURLField* pURLField = GetURLField();
                        if ( pURLField )
                        {
                            //  altes Feld selektieren

                            ESelection aSel = pTableView->GetSelection();
                            aSel.Adjust();
                            aSel.nEndPara = aSel.nStartPara;
                            aSel.nEndPos = aSel.nStartPos + 1;
                            pTableView->SetSelection( aSel );

                            //  neues Feld einfuegen

                            SvxURLField aURLField( rURL, rName, SVXURLFORMAT_REPR );
                            aURLField.SetTargetFrame( rTarget );
                            SvxFieldItem aURLItem( aURLField );
                            pTableView->InsertField( aURLItem );
                            pTableView->SetSelection( aSel );       // select inserted field

                            //  #57254# jetzt doch auch Felder in der Top-View

                            if ( pTopView )
                            {
                                aSel = pTopView->GetSelection();
                                aSel.nEndPara = aSel.nStartPara;
                                aSel.nEndPos = aSel.nStartPos + 1;
                                pTopView->SetSelection( aSel );
                                pTopView->InsertField( aURLItem );
                                pTopView->SetSelection( aSel );     // select inserted field
                            }

                            bDone = TRUE;
                        }
                    }

                    if (!bDone)
                    {
                        pViewData->GetViewShell()->
                            InsertURL( rName, rURL, rTarget, (USHORT) eMode );

                        //  InsertURL an der ViewShell schaltet bei "Button"
                        //  die EditShell ab, darum sofort return

                        return;
                    }
                }
            }
            break;

            case SID_OPEN_HYPERLINK:
                {
                    const SvxURLField* pURLField = GetURLField();
                    if ( pURLField )
                        ScGlobal::OpenURL( pURLField->GetURL(), pURLField->GetTargetFrame() );
                    return;
                }
                break;
    }

    pHdl->DataChanged();
    if (bSetSelIsRef)
        pHdl->SetSelIsRef(TRUE);
}

void lcl_DisableAll( SfxItemSet& rSet )    // disable all slots
{
    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    while (nWhich)
    {
        rSet.DisableItem( nWhich );
        nWhich = aIter.NextWhich();
    }
}

void __EXPORT ScEditShell::GetState( SfxItemSet& rSet )
{
    // #125326# When deactivating the view, edit mode is stopped, but the EditShell is left active
    // (a shell can't be removed from within Deactivate). In that state, the EditView isn't inserted
    // into the EditEngine, so it can have an invalid selection and must not be used.
    if ( !pViewData->HasEditView( pViewData->GetActivePart() ) )
    {
        lcl_DisableAll( rSet );
        return;
    }

    ScInputHandler* pHdl = GetMyInputHdl();
    EditView* pActiveView = pHdl ? pHdl->GetActiveView() : pEditView;

    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    while (nWhich)
    {
        switch (nWhich)
        {
            case SID_ATTR_INSERT:   // Statuszeile
                {
                    if ( pActiveView )
                        rSet.Put( SfxBoolItem( nWhich, pActiveView->IsInsertMode() ) );
                    else
                        rSet.Put( SfxBoolItem( nWhich, 42 ) );
                }
                break;

            case SID_HYPERLINK_GETLINK:
                {
                    SvxHyperlinkItem aHLinkItem;
                    const SvxURLField* pURLField = GetURLField();
                    if ( pURLField )
                    {
                        aHLinkItem.SetName( pURLField->GetRepresentation() );
                        aHLinkItem.SetURL( pURLField->GetURL() );
                        aHLinkItem.SetTargetFrame( pURLField->GetTargetFrame() );
                    }
                    else if ( pActiveView )
                    {
                        // use selected text as name for urls
                        String sReturn = pActiveView->GetSelected();
                        sReturn.Erase(255);
                        sReturn.EraseTrailingChars();
                        aHLinkItem.SetName(sReturn);
                    }
                    rSet.Put(aHLinkItem);
                }
                break;

            case SID_OPEN_HYPERLINK:
                {
                    if ( !GetURLField() )
                        rSet.DisableItem( nWhich );
                }
                break;

            case SID_TRANSLITERATE_HALFWIDTH:
            case SID_TRANSLITERATE_FULLWIDTH:
            case SID_TRANSLITERATE_HIRAGANA:
            case SID_TRANSLITERATE_KATAGANA:
                {
                    // SvtCJKOptions is ref-counted - can be constructed every time
                    SvtCJKOptions aCJKOptions;
                    if (!aCJKOptions.IsChangeCaseMapEnabled())
                    {
                        pViewData->GetBindings().SetVisibleState( nWhich, sal_False );
                        rSet.DisableItem( nWhich );
                    }
                    else
                        pViewData->GetBindings().SetVisibleState( nWhich, sal_True );
                }
                break;
        }
        nWhich = aIter.NextWhich();
    }
}

const SvxURLField* ScEditShell::GetURLField()
{
    ScInputHandler* pHdl = GetMyInputHdl();
    EditView* pActiveView = pHdl ? pHdl->GetActiveView() : pEditView;
    if ( pActiveView )
    {
        const SvxFieldItem* pFieldItem = pActiveView->GetFieldAtSelection();
        if (pFieldItem)
        {
            const SvxFieldData* pField = pFieldItem->GetField();
            if ( pField && pField->ISA(SvxURLField) )
                return (const SvxURLField*)pField;
        }
    }

    return NULL;
}

IMPL_LINK( ScEditShell, ClipboardChanged, TransferableDataHelper*, pDataHelper )
{
    if ( pDataHelper )
    {
        bPastePossible = ( pDataHelper->HasFormat( SOT_FORMAT_STRING ) || pDataHelper->HasFormat( SOT_FORMAT_RTF ) );

        SfxBindings& rBindings = pViewData->GetBindings();
        rBindings.Invalidate( SID_PASTE );
        rBindings.Invalidate( FID_PASTE_CONTENTS );
        rBindings.Invalidate( SID_CLIPBOARD_FORMAT_ITEMS );
    }
    return 0;
}

void __EXPORT ScEditShell::GetClipState( SfxItemSet& rSet )
{
    if ( !pClipEvtLstnr )
    {
        // create listener
        pClipEvtLstnr = new TransferableClipboardListener( LINK( this, ScEditShell, ClipboardChanged ) );
        pClipEvtLstnr->acquire();
        Window* pWin = pViewData->GetActiveWin();
        pClipEvtLstnr->AddRemoveListener( pWin, TRUE );

        // get initial state
        TransferableDataHelper aDataHelper( TransferableDataHelper::CreateFromSystemClipboard( pViewData->GetActiveWin() ) );
        bPastePossible = ( aDataHelper.HasFormat( SOT_FORMAT_STRING ) || aDataHelper.HasFormat( SOT_FORMAT_RTF ) );
    }

    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    while (nWhich)
    {
        switch (nWhich)
        {
            case SID_PASTE:
            case FID_PASTE_CONTENTS:
                if( !bPastePossible )
                    rSet.DisableItem( nWhich );
                break;
            case SID_CLIPBOARD_FORMAT_ITEMS:
                if( bPastePossible )
                {
                    SvxClipboardFmtItem aFormats( SID_CLIPBOARD_FORMAT_ITEMS );
                    TransferableDataHelper aDataHelper(
                            TransferableDataHelper::CreateFromSystemClipboard( pViewData->GetActiveWin() ) );

                    if ( aDataHelper.HasFormat( SOT_FORMAT_STRING ) )
                        aFormats.AddClipbrdFormat( SOT_FORMAT_STRING );
                    if ( aDataHelper.HasFormat( SOT_FORMAT_RTF ) )
                        aFormats.AddClipbrdFormat( SOT_FORMAT_RTF );

                    rSet.Put( aFormats );
                }
                else
                    rSet.DisableItem( nWhich );
                break;
        }
        nWhich = aIter.NextWhich();
    }
}

void lcl_InvalidateUnder( SfxBindings& rBindings )
{
    rBindings.Invalidate( SID_ATTR_CHAR_UNDERLINE );
    rBindings.Invalidate( SID_ULINE_VAL_NONE );
    rBindings.Invalidate( SID_ULINE_VAL_SINGLE );
    rBindings.Invalidate( SID_ULINE_VAL_DOUBLE );
    rBindings.Invalidate( SID_ULINE_VAL_DOTTED );
}

void ScEditShell::ExecuteAttr(SfxRequest& rReq)
{
    SfxItemSet          aSet( pEditView->GetEmptyItemSet() );
    SfxBindings&        rBindings   = pViewData->GetBindings();
    const SfxItemSet*   pArgs       = rReq.GetArgs();
    USHORT              nSlot       = rReq.GetSlot();

    switch ( nSlot )
    {
        case SID_ATTR_CHAR_FONTHEIGHT:
        case SID_ATTR_CHAR_FONT:
            {
                if (pArgs)
                {
                    USHORT nScript = pEditView->GetSelectedScriptType();
                    if (nScript == 0) nScript = ScGlobal::GetDefaultScriptType();

                    SfxItemPool& rPool = GetPool();
                    SvxScriptSetItem aSetItem( nSlot, rPool );
                    USHORT nWhich = rPool.GetWhich( nSlot );
                    aSetItem.PutItemForScriptType( nScript, pArgs->Get( nWhich ) );

                    aSet.Put( aSetItem.GetItemSet(), FALSE );
                }
            }
            break;

        case SID_ATTR_CHAR_COLOR:
            {
                if (pArgs)
                {
                    aSet.Put( pArgs->Get( pArgs->GetPool()->GetWhich( nSlot ) ) );
                    rBindings.Invalidate( nSlot );
                }
            }
            break;

        //  Toggles

        case SID_ATTR_CHAR_WEIGHT:
            {
                USHORT nScript = pEditView->GetSelectedScriptType();
                if (nScript == 0) nScript = ScGlobal::GetDefaultScriptType();
                SfxItemPool& rPool = GetPool();

                BOOL bOld = FALSE;
                SvxScriptSetItem aOldSetItem( nSlot, rPool );
                aOldSetItem.GetItemSet().Put( pEditView->GetAttribs(), FALSE );
                const SfxPoolItem* pCore = aOldSetItem.GetItemOfScript( nScript );
                if ( pCore && ((const SvxWeightItem*)pCore)->GetWeight() > WEIGHT_NORMAL )
                    bOld = TRUE;

                SvxScriptSetItem aSetItem( nSlot, rPool );
                aSetItem.PutItemForScriptType( nScript,
                            SvxWeightItem( bOld ? WEIGHT_NORMAL : WEIGHT_BOLD, EE_CHAR_WEIGHT ) );
                aSet.Put( aSetItem.GetItemSet(), FALSE );

                rBindings.Invalidate( nSlot );
            }
            break;

        case SID_ATTR_CHAR_POSTURE:
            {
                USHORT nScript = pEditView->GetSelectedScriptType();
                if (nScript == 0) nScript = ScGlobal::GetDefaultScriptType();
                SfxItemPool& rPool = GetPool();

                BOOL bOld = FALSE;
                SvxScriptSetItem aOldSetItem( nSlot, rPool );
                aOldSetItem.GetItemSet().Put( pEditView->GetAttribs(), FALSE );
                const SfxPoolItem* pCore = aOldSetItem.GetItemOfScript( nScript );
                if ( pCore && ((const SvxPostureItem*)pCore)->GetValue() != ITALIC_NONE )
                    bOld = TRUE;

                SvxScriptSetItem aSetItem( nSlot, rPool );
                aSetItem.PutItemForScriptType( nScript,
                            SvxPostureItem( bOld ? ITALIC_NONE : ITALIC_NORMAL, EE_CHAR_ITALIC ) );
                aSet.Put( aSetItem.GetItemSet(), FALSE );

                rBindings.Invalidate( nSlot );
            }
            break;

        case SID_ULINE_VAL_NONE:
            aSet.Put( SvxUnderlineItem( UNDERLINE_NONE, EE_CHAR_UNDERLINE ) );
            lcl_InvalidateUnder( rBindings );
            break;

        case SID_ATTR_CHAR_UNDERLINE:   // Toggles
        case SID_ULINE_VAL_SINGLE:
        case SID_ULINE_VAL_DOUBLE:
        case SID_ULINE_VAL_DOTTED:
            {
                FontUnderline eOld = ((const SvxUnderlineItem&) pEditView->
                                    GetAttribs().Get(EE_CHAR_UNDERLINE)).GetUnderline();
                FontUnderline eNew = eOld;
                switch (nSlot)
                {
                    case SID_ATTR_CHAR_UNDERLINE:
                        eNew = ( eOld != UNDERLINE_NONE ) ? UNDERLINE_NONE : UNDERLINE_SINGLE;
                        break;
                    case SID_ULINE_VAL_SINGLE:
                        eNew = ( eOld == UNDERLINE_SINGLE ) ? UNDERLINE_NONE : UNDERLINE_SINGLE;
                        break;
                    case SID_ULINE_VAL_DOUBLE:
                        eNew = ( eOld == UNDERLINE_DOUBLE ) ? UNDERLINE_NONE : UNDERLINE_DOUBLE;
                        break;
                    case SID_ULINE_VAL_DOTTED:
                        eNew = ( eOld == UNDERLINE_DOTTED ) ? UNDERLINE_NONE : UNDERLINE_DOTTED;
                        break;
                }
                aSet.Put( SvxUnderlineItem( eNew, EE_CHAR_UNDERLINE ) );
                lcl_InvalidateUnder( rBindings );
            }
            break;

        case SID_ATTR_CHAR_STRIKEOUT:
            {
                BOOL bOld = ((const SvxCrossedOutItem&)pEditView->GetAttribs().
                                Get(EE_CHAR_STRIKEOUT)).GetValue() != STRIKEOUT_NONE;
                aSet.Put( SvxCrossedOutItem( bOld ? STRIKEOUT_NONE : STRIKEOUT_SINGLE, EE_CHAR_STRIKEOUT ) );
                rBindings.Invalidate( nSlot );
            }
            break;

        case SID_ATTR_CHAR_SHADOWED:
            {
                BOOL bOld = ((const SvxShadowedItem&)pEditView->GetAttribs().
                                Get(EE_CHAR_SHADOW)).GetValue();
                aSet.Put( SvxShadowedItem( !bOld, EE_CHAR_SHADOW ) );
                rBindings.Invalidate( nSlot );
            }
            break;

        case SID_ATTR_CHAR_CONTOUR:
            {
                BOOL bOld = ((const SvxContourItem&)pEditView->GetAttribs().
                                Get(EE_CHAR_OUTLINE)).GetValue();
                aSet.Put( SvxContourItem( !bOld, EE_CHAR_OUTLINE ) );
                rBindings.Invalidate( nSlot );
            }
            break;

        case SID_SET_SUPER_SCRIPT:
            {
                SvxEscapement eOld = (SvxEscapement) ((const SvxEscapementItem&)
                        pEditView->GetAttribs().Get(EE_CHAR_ESCAPEMENT)).GetEnumValue();
                SvxEscapement eNew = (eOld == SVX_ESCAPEMENT_SUPERSCRIPT) ?
                                        SVX_ESCAPEMENT_OFF : SVX_ESCAPEMENT_SUPERSCRIPT;
                aSet.Put( SvxEscapementItem( eNew, EE_CHAR_ESCAPEMENT ) );
                rBindings.Invalidate( nSlot );
            }
            break;
        case SID_SET_SUB_SCRIPT:
            {
                SvxEscapement eOld = (SvxEscapement) ((const SvxEscapementItem&)
                        pEditView->GetAttribs().Get(EE_CHAR_ESCAPEMENT)).GetEnumValue();
                SvxEscapement eNew = (eOld == SVX_ESCAPEMENT_SUBSCRIPT) ?
                                        SVX_ESCAPEMENT_OFF : SVX_ESCAPEMENT_SUBSCRIPT;
                aSet.Put( SvxEscapementItem( eNew, EE_CHAR_ESCAPEMENT ) );
                rBindings.Invalidate( nSlot );
            }
            break;
    }

    //
    //  anwenden
    //

    EditEngine* pEngine = pEditView->GetEditEngine();
    BOOL bOld = pEngine->GetUpdateMode();
    pEngine->SetUpdateMode(FALSE);

    pEditView->SetAttribs( aSet );

    pEngine->SetUpdateMode(bOld);
    pEditView->Invalidate();

    ScInputHandler* pHdl = GetMyInputHdl();
    pHdl->SetModified();

    rReq.Done();
}

void ScEditShell::GetAttrState(SfxItemSet &rSet)
{
    if ( !pViewData->HasEditView( pViewData->GetActivePart() ) )    // #125326#
    {
        lcl_DisableAll( rSet );
        return;
    }

    SfxItemSet aAttribs = pEditView->GetAttribs();
    rSet.Put( aAttribs );

    //  choose font info according to selection script type

    USHORT nScript = pEditView->GetSelectedScriptType();
    if (nScript == 0) nScript = ScGlobal::GetDefaultScriptType();

    if ( rSet.GetItemState( EE_CHAR_FONTINFO ) != SFX_ITEM_UNKNOWN )
        ScViewUtil::PutItemScript( rSet, aAttribs, EE_CHAR_FONTINFO, nScript );
    if ( rSet.GetItemState( EE_CHAR_FONTHEIGHT ) != SFX_ITEM_UNKNOWN )
        ScViewUtil::PutItemScript( rSet, aAttribs, EE_CHAR_FONTHEIGHT, nScript );
    if ( rSet.GetItemState( EE_CHAR_WEIGHT ) != SFX_ITEM_UNKNOWN )
        ScViewUtil::PutItemScript( rSet, aAttribs, EE_CHAR_WEIGHT, nScript );
    if ( rSet.GetItemState( EE_CHAR_ITALIC ) != SFX_ITEM_UNKNOWN )
        ScViewUtil::PutItemScript( rSet, aAttribs, EE_CHAR_ITALIC, nScript );

    //  Unterstreichung

    SfxItemState eState = aAttribs.GetItemState( EE_CHAR_UNDERLINE, TRUE );
    if ( eState == SFX_ITEM_DONTCARE )
    {
        rSet.InvalidateItem( SID_ULINE_VAL_NONE );
        rSet.InvalidateItem( SID_ULINE_VAL_SINGLE );
        rSet.InvalidateItem( SID_ULINE_VAL_DOUBLE );
        rSet.InvalidateItem( SID_ULINE_VAL_DOTTED );
    }
    else
    {
        FontUnderline eUnderline = ((const SvxUnderlineItem&)
                    aAttribs.Get(EE_CHAR_UNDERLINE)).GetUnderline();
        USHORT nId = SID_ULINE_VAL_NONE;
        switch (eUnderline)
        {
            case UNDERLINE_SINGLE:  nId = SID_ULINE_VAL_SINGLE; break;
            case UNDERLINE_DOUBLE:  nId = SID_ULINE_VAL_DOUBLE; break;
            case UNDERLINE_DOTTED:  nId = SID_ULINE_VAL_DOTTED; break;
            default:
                break;
        }
        rSet.Put( SfxBoolItem( nId, TRUE ) );
    }

    //! Testen, ob Klammer-Hervorhebung aktiv ist !!!!
    ScInputHandler* pHdl = GetMyInputHdl();
    if ( pHdl && pHdl->IsFormulaMode() )
        rSet.ClearItem( EE_CHAR_WEIGHT );   // hervorgehobene Klammern hier nicht
}

String ScEditShell::GetSelectionText( BOOL bWholeWord )
{
    String aStrSelection;

    if ( pViewData->HasEditView( pViewData->GetActivePart() ) )    // #125326#
    {
        if ( bWholeWord )
        {
            EditEngine* pEngine = pEditView->GetEditEngine();
            ESelection  aSel = pEditView->GetSelection();
            String      aStrCurrentDelimiters = pEngine->GetWordDelimiters();

            pEngine->SetWordDelimiters( String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM(" .,;\"'")) );
            aStrSelection = pEngine->GetWord( aSel.nEndPara, aSel.nEndPos );
            pEngine->SetWordDelimiters( aStrCurrentDelimiters );
        }
        else
        {
            aStrSelection = pEditView->GetSelected();
        }
    }

    return aStrSelection;
}

void ScEditShell::ExecuteUndo(SfxRequest& rReq)
{
    //  #81733# Undo must be handled here because it's called for both EditViews

    ScInputHandler* pHdl = GetMyInputHdl();
    DBG_ASSERT(pHdl,"no ScInputHandler");
    EditView* pTopView   = pHdl->GetTopView();
    EditView* pTableView = pHdl->GetTableView();
    DBG_ASSERT(pTableView,"no EditView");

    pHdl->DataChanging();

    const SfxItemSet* pReqArgs = rReq.GetArgs();
    USHORT nSlot = rReq.GetSlot();
    switch ( nSlot )
    {
        case SID_UNDO:
        case SID_REDO:
            {
                BOOL bIsUndo = ( nSlot == SID_UNDO );

                USHORT nCount = 1;
                const SfxPoolItem* pItem;
                if ( pReqArgs && pReqArgs->GetItemState( nSlot, TRUE, &pItem ) == SFX_ITEM_SET )
                    nCount = ((const SfxUInt16Item*)pItem)->GetValue();

                for (USHORT i=0; i<nCount; i++)
                {
                    if ( bIsUndo )
                    {
                        pTableView->Undo();
                        if (pTopView)
                            pTopView->Undo();
                    }
                    else
                    {
                        pTableView->Redo();
                        if (pTopView)
                            pTopView->Redo();
                    }
                }
            }
            break;
    }
    pViewData->GetBindings().InvalidateAll(FALSE);

    pHdl->DataChanged();
}

void ScEditShell::GetUndoState(SfxItemSet &rSet)
{
    //  Undo state is taken from normal ViewFrame state function

    SfxViewFrame* pViewFrm = pViewData->GetViewShell()->GetViewFrame();
    if ( pViewFrm && GetUndoManager() )
    {
        SfxWhichIter aIter(rSet);
        USHORT nWhich = aIter.FirstWhich();
        while( nWhich )
        {
            pViewFrm->GetSlotState( nWhich, NULL, &rSet );
            nWhich = aIter.NextWhich();
        }
    }

    //  disable if no action in input line EditView

    ScInputHandler* pHdl = GetMyInputHdl();
    DBG_ASSERT(pHdl,"no ScInputHandler");
    EditView* pTopView = pHdl->GetTopView();
    if (pTopView)
    {
        SfxUndoManager& rTopMgr = pTopView->GetEditEngine()->GetUndoManager();
        if ( rTopMgr.GetUndoActionCount() == 0 )
            rSet.DisableItem( SID_UNDO );
        if ( rTopMgr.GetRedoActionCount() == 0 )
            rSet.DisableItem( SID_REDO );
    }
}

void ScEditShell::ExecuteTrans( SfxRequest& rReq )
{
    sal_Int32 nType = ScViewUtil::GetTransliterationType( rReq.GetSlot() );
    if ( nType )
    {
        ScInputHandler* pHdl = GetMyInputHdl();
        DBG_ASSERT( pHdl, "no ScInputHandler" );

        EditView* pTopView   = pHdl->GetTopView();
        EditView* pTableView = pHdl->GetTableView();
        DBG_ASSERT( pTableView, "no EditView" );

        EditEngine* pEngine = pTableView->GetEditEngine();
        pHdl->DataChanging();

        pTableView->TransliterateText( nType );
        if (pTopView)
            pTopView->TransliterateText( nType );

        pHdl->DataChanged();
    }
}

