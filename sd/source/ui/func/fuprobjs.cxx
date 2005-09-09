/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuprobjs.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 04:49:35 $
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

#include "fuprobjs.hxx"

#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif
#ifndef _OUTLINER_HXX
#include <svx/outliner.hxx>
#endif
#ifndef _SFXSMPLHINT_HXX //autogen
#include <svtools/smplhint.hxx>
#endif

#pragma hdrstop

#include "app.hrc"
#include "res_bmp.hrc"
#include "strings.hrc"
#include "glob.hrc"
#include "prltempl.hrc"

#include "sdresid.hxx"
#include "drawdoc.hxx"
#ifndef SD_OUTLINE_VIEW_SHELL_HX
#include "OutlineViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "glob.hxx"
#include "prlayout.hxx"
//CHINA001 #include "prltempl.hxx"
#include "unchss.hxx"
#include "sdabstdlg.hxx" //CHINA001
namespace sd {

TYPEINIT1( FuPresentationObjects, FuPoor );


/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuPresentationObjects::FuPresentationObjects (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDoc,
    SfxRequest& rReq)
     : FuPoor(pViewSh, pWin, pView, pDoc, rReq)
{
    // ergibt die Selektion ein eindeutiges Praesentationslayout?
    // wenn nicht, duerfen die Vorlagen nicht bearbeitet werden
    SfxItemSet aSet(pDoc->GetItemPool(), SID_STATUS_LAYOUT, SID_STATUS_LAYOUT);
    static_cast<OutlineViewShell*>(pViewSh)->GetStatusBarState( aSet );
    String aLayoutName = (((SfxStringItem&)aSet.Get(SID_STATUS_LAYOUT)).GetValue());
    DBG_ASSERT(aLayoutName.Len(), "Layout unbestimmt");

    BOOL    bUnique = FALSE;
    USHORT  nDepth, nTmp;
    OutlineView* pOlView = static_cast<OutlineView*>(
        static_cast<OutlineViewShell*>(pViewSh)->GetView());
    OutlinerView* pOutlinerView = pOlView->GetViewByWindow( (Window*) pWin );
    ::Outliner* pOutl = pOutlinerView->GetOutliner();
    List* pList = pOutlinerView->CreateSelectionList();
    Paragraph* pPara = (Paragraph*)pList->First();
    nDepth = pOutl->GetDepth((USHORT)pOutl->GetAbsPos( pPara ) );
    while( pPara )
    {
        nTmp = pOutl->GetDepth((USHORT) pOutl->GetAbsPos( pPara ) );

        if( nDepth != nTmp )
        {
            bUnique = FALSE;
            break;
        }
        bUnique = TRUE;

        pPara = (Paragraph*) pList->Next();
    }

    if( bUnique )
    {
        String aStyleName = aLayoutName;
        aStyleName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( SD_LT_SEPARATOR ) );
        USHORT nDlgId = TAB_PRES_LAYOUT_TEMPLATE_3;
        PresentationObjects ePO;

        if( nDepth == 0 )
        {
            ePO = PO_TITLE;
            String aStr(SdResId( STR_LAYOUT_TITLE ));
            aStyleName.Append( aStr );
        }
        else
        {
            ePO = (PresentationObjects) ( PO_OUTLINE_1 + nDepth - 1 );
            String aStr(SdResId( STR_LAYOUT_OUTLINE ));
            aStyleName.Append( aStr );
            aStyleName.Append( sal_Unicode(' ') );
            aStyleName.Append( UniString::CreateFromInt32( nDepth ) );
        }

        SfxStyleSheetBasePool* pStyleSheetPool = pDocSh->GetStyleSheetPool();
        SfxStyleSheetBase* pStyleSheet = pStyleSheetPool->Find( aStyleName,
                            (SfxStyleFamily) SD_LT_FAMILY );
        DBG_ASSERT(pStyleSheet, "StyleSheet nicht gefunden");

        if( pStyleSheet )
        {
            SfxStyleSheetBase& rStyleSheet = *pStyleSheet;

//CHINA001          SdPresLayoutTemplateDlg* pDlg = new SdPresLayoutTemplateDlg( pDocSh, NULL,
//CHINA001          SdResId( nDlgId ), rStyleSheet, ePO, pStyleSheetPool );
            SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
            DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
            SfxAbstractTabDialog* pDlg = pFact->CreateSdPresLayoutTemplateDlg(ResId( TAB_PRES_LAYOUT_TEMPLATE ), pDocSh, NULL,
                                                SdResId( nDlgId ), rStyleSheet, ePO, pStyleSheetPool );
            DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
            if( pDlg->Execute() == RET_OK )
            {
                const SfxItemSet* pOutSet = pDlg->GetOutputItemSet();
                // Undo-Action
                StyleSheetUndoAction* pAction = new StyleSheetUndoAction
                                                (pDoc, (SfxStyleSheet*)pStyleSheet,
                                                    pOutSet);
                pDocSh->GetUndoManager()->AddUndoAction(pAction);

                pStyleSheet->GetItemSet().Put( *pOutSet );
                ( (SfxStyleSheet*) pStyleSheet )->Broadcast( SfxSimpleHint( SFX_HINT_DATACHANGED ) );
            }
            delete( pDlg );
        }
    }
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

FuPresentationObjects::~FuPresentationObjects()
{
}



} // end of namespace sd
