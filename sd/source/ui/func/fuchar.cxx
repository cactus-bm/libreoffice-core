/*************************************************************************
 *
 *  $RCSfile: fuchar.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2004-05-10 15:46:17 $
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

#include "fuchar.hxx"

#include <svx/editdata.hxx>
#include <svx/svxids.hrc>
#ifndef _EEITEM_HXX //autogen
#include <svx/eeitem.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif

//CHINA001 #include "dlg_char.hxx"
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#include "drawdoc.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#include "DrawDocShell.hxx"
#include "sdabstdlg.hxx" //CHINA001
#include "dlg_char.hrc" //CHINA001
namespace sd {

TYPEINIT1( FuChar, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuChar::FuChar (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDoc,
    SfxRequest& rReq)
    : FuPoor(pViewSh, pWin, pView, pDoc, rReq)
{
    const SfxItemSet* pArgs = rReq.GetArgs();

    if( !pArgs )
    {
        SfxItemSet aEditAttr( pDoc->GetPool() );
        pView->GetAttributes( aEditAttr );

        SfxItemSet aNewAttr( pViewSh->GetPool(),
                                EE_ITEMS_START, EE_ITEMS_END );
        aNewAttr.Put( aEditAttr, FALSE );

        //CHINA001 SdCharDlg* pDlg = new SdCharDlg( NULL, &aNewAttr, pDoc->GetDocSh() );
        SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
        DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
        SfxAbstractTabDialog* pDlg = pFact->CreateSdTabDialog(ResId( TAB_CHAR ), NULL, &aNewAttr, pDoc->GetDocSh() );
        DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
        USHORT nResult = pDlg->Execute();

        switch( nResult )
        {
            case RET_OK:
            {
                rReq.Done( *( pDlg->GetOutputItemSet() ) );

                pArgs = rReq.GetArgs();
            }
            break;

            default:
            {
                delete pDlg;
            }
            return; // Abbruch
        }
        delete( pDlg );
    }
    pView->SetAttributes(*pArgs);

    // invalidieren der Slots, die in der DrTxtObjBar auftauchen
    static USHORT SidArray[] = {
                    SID_ATTR_CHAR_FONT,
                    SID_ATTR_CHAR_POSTURE,
                    SID_ATTR_CHAR_WEIGHT,
                    SID_ATTR_CHAR_UNDERLINE,
                    SID_ATTR_CHAR_FONTHEIGHT,
                    SID_ATTR_CHAR_COLOR,
                    SID_SET_SUPER_SCRIPT,
                    SID_SET_SUB_SCRIPT,
                    0 };

    pViewShell->GetViewFrame()->GetBindings().Invalidate( SidArray );

    if( pDoc->GetOnlineSpell() )
    {
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pArgs->GetItemState(EE_CHAR_LANGUAGE, FALSE, &pItem ) ||
            SFX_ITEM_SET == pArgs->GetItemState(EE_CHAR_LANGUAGE_CJK, FALSE, &pItem ) ||
            SFX_ITEM_SET == pArgs->GetItemState(EE_CHAR_LANGUAGE_CTL, FALSE, &pItem ) )
        {
            pDoc->StopOnlineSpelling();
            pDoc->StartOnlineSpelling();
        }
    }
}

} // end of namespace sd
