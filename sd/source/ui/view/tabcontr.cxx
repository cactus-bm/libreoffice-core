/*************************************************************************
 *
 *  $RCSfile: tabcontr.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: af $ $Date: 2002-11-04 14:47:19 $
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

#ifndef _SVDLAYER_HXX
#include <svx/svdlayer.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif

#pragma hdrstop

#include "sdattr.hxx"
#include "app.hxx"
#include "tabcontr.hxx"
#include "app.hrc"
#include "glob.hrc"
#include "res_bmp.hrc"
#include "drviewsh.hxx"
#include "grviewsh.hxx"
#include "helpids.h"
#include "sdview.hxx"
#include "sdpage.hxx"
#include "drawdoc.hxx"
#include "sdwindow.hxx"
#include "unmodpg.hxx"
#include "docshell.hxx"
#include "sdresid.hxx"


#define SWITCH_TIMEOUT  20

// -----------------------------------------
// - SdTabControl::SdPageObjsTransferable -
// -----------------------------------------

SdTabControl::SdTabControlTransferable::~SdTabControlTransferable()
{
}

// -----------------------------------------------------------------------------

void SdTabControl::SdTabControlTransferable::AddSupportedFormats()
{
    AddFormat( SOT_FORMATSTR_ID_STARDRAW_TABBAR );
}

// -----------------------------------------------------------------------------

sal_Bool SdTabControl::SdTabControlTransferable::GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor )
{
    return sal_False;
}

// -----------------------------------------------------------------------------

void SdTabControl::SdTabControlTransferable::DragFinished( sal_Int8 nDropAction )
{
    mrParent.DragFinished( nDropAction );
}

/*************************************************************************
|*
|* Standard-Konstruktor
|*
\************************************************************************/

SdTabControl::SdTabControl(SdDrawViewShell* pViewSh, Window* pParent) :
    TabBar( pParent, WinBits( WB_BORDER | WB_3DLOOK | WB_SCROLL | WB_SIZEABLE | WB_DRAG) ),
    DragSourceHelper( this ),
    DropTargetHelper( this ),
    pDrViewSh(pViewSh),
    bInternalMove(FALSE)
{
    EnableEditMode();
    SetSizePixel(Size(0, 0));
    SetMaxPageWidth( 150 );
    SetHelpId( HID_SD_TABBAR_PAGES );
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

SdTabControl::~SdTabControl()
{
}

/*************************************************************************
|*
\************************************************************************/

void SdTabControl::Select()
{
    SfxDispatcher* pDispatcher = pDrViewSh->GetViewFrame()->GetDispatcher();
    pDispatcher->Execute(SID_SWITCHPAGE, SFX_CALLMODE_ASYNCHRON |
                            SFX_CALLMODE_RECORD);
}

/*************************************************************************
|*
\************************************************************************/

void  SdTabControl::MouseButtonDown(const MouseEvent& rMEvt)
{
    // eine saubere linke Maustaste ohne verwaessernde Modifier (damit koennte
    //ja das Kontextmenue gemeint sein)
    if ( rMEvt.IsLeft() && !rMEvt.IsMod1() && !rMEvt.IsMod2() && !rMEvt.IsShift())
    {
        Point aPos = PixelToLogic( rMEvt.GetPosPixel() );
        USHORT aPageId = GetPageId(aPos);

        if (aPageId == 0)
        {
            SfxDispatcher* pDispatcher = pDrViewSh->GetViewFrame()->GetDispatcher();

            pDispatcher->Execute(SID_INSERTPAGE_QUICK,
                                SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_RECORD);
        }
    }

    // A single left click with pressed control key on a tab page first
    // switches to that page before the usual handling (copying with drag
    // and drop) takes place.
    else if (rMEvt.IsLeft() && rMEvt.IsMod1() && !rMEvt.IsMod2() && !rMEvt.IsShift())
    {
        pDrViewSh->SwitchPage (GetPageId (rMEvt.GetPosPixel()) - 1);
    }


    TabBar::MouseButtonDown(rMEvt);
}

/*************************************************************************
|*
\************************************************************************/

void SdTabControl::DoubleClick()
{
    if (GetCurPageId() != 0)
    {
        SfxDispatcher* pDispatcher = pDrViewSh->GetViewFrame()->GetDispatcher();
        pDispatcher->Execute( SID_MODIFYPAGE,
                        SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_RECORD );
    }
}

/*************************************************************************
|*
|* StartDrag-Request
|*
\************************************************************************/

void SdTabControl::StartDrag( sal_Int8 nAction, const Point& rPosPixel )
{
    bInternalMove = TRUE;

    // object is delete by reference mechanismn
    ( new SdTabControl::SdTabControlTransferable( *this ) )->StartDrag( this, DND_ACTION_COPYMOVE );
}

/*************************************************************************
|*
|* DragFinished
|*
\************************************************************************/

void SdTabControl::DragFinished( sal_Int8 nDropAction )
{
    bInternalMove = FALSE;
}

/*************************************************************************
|*
|* AcceptDrop-Event
|*
\************************************************************************/

sal_Int8 SdTabControl::AcceptDrop( const AcceptDropEvent& rEvt )
{
    sal_Int8 nRet = DND_ACTION_NONE;

    if( rEvt.mbLeaving )
        EndSwitchPage();

    if( !pDrViewSh->GetDocSh()->IsReadOnly() )
    {
        SdDrawDocument* pDoc = pDrViewSh->GetDoc();
        Point           aPos( rEvt.maPosPixel );

        if( bInternalMove )
        {
            if( rEvt.mbLeaving || ( pDrViewSh->GetEditMode() == EM_MASTERPAGE ) )
                HideDropPos();
            else
            {
                ShowDropPos( aPos );
                nRet = rEvt.mnAction;
            }
        }
        else
        {
            HideDropPos();

            USHORT nPageId = GetPageId( aPos ) - 1;

            if( ( nPageId >= 0 ) && pDoc->GetPage( nPageId ) )
            {
                SdWindow* pWindow = NULL;

                nRet = pDrViewSh->AcceptDrop( rEvt, *this, NULL, nPageId, SDRLAYER_NOTFOUND );
                SwitchPage( aPos );
            }
        }
    }

    return nRet;
}

/*************************************************************************
|*
|* ExecuteDrop-Event
|*
\************************************************************************/

sal_Int8 SdTabControl::ExecuteDrop( const ExecuteDropEvent& rEvt )
{
    SdDrawDocument* pDoc = pDrViewSh->GetDoc();
    Point           aPos( rEvt.maPosPixel );
    sal_Int8        nRet = DND_ACTION_NONE;

    if( bInternalMove )
    {
        USHORT nPageId = ShowDropPos( aPos ) - 1;

        switch (rEvt.mnAction)
        {
            case DND_ACTION_MOVE:
                if( pDrViewSh->IsSwitchPageAllowed() && pDoc->MovePages( nPageId ) )
                {
                    SfxDispatcher* pDispatcher = pDrViewSh->GetViewFrame()->GetDispatcher();
                    pDispatcher->Execute(SID_SWITCHPAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
                }
                break;

            case DND_ACTION_COPY:
            {
                // Copying the selected page to the place that rEvt points
                // takes place in three steps:
                // 1. Create a copy of the selected page.  This copy will
                // lie directly behind the selected page.
                // 2. Move the copy to the desired place.
                // 3. Select the copy.
                if (pDrViewSh->IsSwitchPageAllowed())
                {
                    // 1. Create a copy.
                    USHORT nPageNumOfCopy = pDoc->DuplicatePage (GetCurPageId() - 1);
                    // 2. Move page.  For this first switch to the copy:
                    // MovePages operates on the currently selected page(s).
                    pDrViewSh->SwitchPage (nPageNumOfCopy);
                    // Adapt target page id when necessary, i.e. page copy
                    // has been inserted in front of the target page.
                    USHORT nPageNum = nPageId;
                    if ((nPageNumOfCopy <= nPageNum) && (nPageNum != (USHORT)-1))
                        nPageNum += 1;
                    if (pDoc->MovePages(nPageNum))
                    {
                        // 3. Switch to the copy that has been moved to its
                        // final destination.  Use an asynchron slot call to
                        // be executed after the still pending ones.
                        if (nPageNumOfCopy >= nPageNum || (nPageNum == (USHORT)-1))
                            nPageNum += 1;
                        SetCurPageId (GetPageId(nPageNum));
                        SfxDispatcher* pDispatcher = pDrViewSh->GetViewFrame()->GetDispatcher();
                        pDispatcher->Execute(SID_SWITCHPAGE,
                            SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
                    }
                }

                break;
            }
        }

        nRet = rEvt.mnAction;
    }
    else
    {
        USHORT nPageId = GetPageId( aPos ) - 1;

        if( ( nPageId >= 0 ) && pDoc->GetPage( nPageId ) )
        {
            nRet = pDrViewSh->ExecuteDrop( rEvt, *this, NULL, nPageId, SDRLAYER_NOTFOUND );
        }
    }

    HideDropPos();
    EndSwitchPage();

    return nRet;
}

/*************************************************************************
|*
\************************************************************************/

void SdTabControl::Command(const CommandEvent& rCEvt)
{
    USHORT nCmd = rCEvt.GetCommand();

    if ( nCmd == COMMAND_CONTEXTMENU )
    {
        BOOL bGraphicShell = pDrViewSh->ISA( SdGraphicViewShell );
        USHORT nResId = bGraphicShell ? RID_GRAPHIC_PAGETAB_POPUP :
                                        RID_DRAW_PAGETAB_POPUP;
        SfxDispatcher* pDispatcher = pDrViewSh->GetViewFrame()->GetDispatcher();
        pDispatcher->ExecutePopup( SdResId( nResId ) );
    }
}

/*************************************************************************
|*
\************************************************************************/

long SdTabControl::StartRenaming()
{
    BOOL bOK = FALSE;

    if (pDrViewSh->GetPageKind() == PK_STANDARD)
    {
        bOK = TRUE;

        SdView* pView = pDrViewSh->GetView();

        if ( pView->IsTextEdit() )
            pView->EndTextEdit();
    }

    return( bOK );
}

/*************************************************************************
|*
\************************************************************************/

long SdTabControl::AllowRenaming()
{
    BOOL bOK = TRUE;

    String aNewName( GetEditText() );
    String aCompareName( GetPageText( GetEditPageId() ) );

    if( aCompareName != aNewName )
    {
        // Seite umbenennen
        if( pDrViewSh->GetDocSh()->CheckPageName( this, aNewName ) )
        {
            SetEditText( aNewName );
            EndRenaming();
        }
        else
        {
            bOK = FALSE;
        }
    }
    return( bOK );
}

/*************************************************************************
|*
\************************************************************************/

void SdTabControl::EndRenaming()
{
    if( !IsEditModeCanceled() )
    {
        SdView* pView = pDrViewSh->GetView();
        SdDrawDocument* pDoc = pView->GetDoc();
        String aNewName( GetEditText() );

        if (pDrViewSh->GetEditMode() == EM_PAGE)
        {
            // Seite umbenennen
            SdView* pView = pDrViewSh->GetView();
            SdDrawDocument* pDoc = pView->GetDoc();
            PageKind ePageKind = pDrViewSh->GetPageKind();
            SdPage* pActualPage = pDoc->GetSdPage(GetEditPageId() - 1, ePageKind);

            SdPage* pUndoPage = pActualPage;
            SdrLayerAdmin& rLayerAdmin = pDoc->GetLayerAdmin();
            BYTE aBckgrnd = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRND)), FALSE);
            BYTE aBckgrndObj = rLayerAdmin.GetLayerID(String(SdResId(STR_LAYER_BCKGRNDOBJ)), FALSE);
            SetOfByte aVisibleLayers = pActualPage->GetMasterPageVisibleLayers(0);

            // #67720#
            SfxUndoManager* pManager = pDoc->GetDocSh()->GetUndoManager();
            ModifyPageUndoAction* pAction = new ModifyPageUndoAction(
                pManager, pDoc, pUndoPage, aNewName, pActualPage->GetAutoLayout(),
                aVisibleLayers.IsSet(aBckgrnd),
                aVisibleLayers.IsSet(aBckgrndObj));
            pManager->AddUndoAction(pAction);

            pActualPage->SetName(aNewName);
            aNewName = pActualPage->GetName();

            if (ePageKind == PK_STANDARD)
            {
                SdPage* pNotesPage = pDoc->GetSdPage(GetEditPageId() - 1, PK_NOTES);
                pNotesPage->SetName(aNewName);
            }
        }
        else
        {
            // MasterPage umbenennen -> LayoutTemplate umbenennen
            SdPage* pActualPage = pDoc->GetMasterSdPage(GetEditPageId() - 1, pDrViewSh->GetPageKind());
            pDoc->RenameLayoutTemplate(pActualPage->GetLayoutName(), aNewName);
            aNewName = pActualPage->GetName();
        }

        // user edited page names may be changed by the page so update control
        SetPageText( GetEditPageId(), aNewName );

        pDoc->SetChanged(TRUE);

        // Damit der Navigator das mitbekommt
        SfxBoolItem aItem(SID_NAVIGATOR_INIT, TRUE);
        pDrViewSh->GetViewFrame()->GetDispatcher()->Execute(
            SID_NAVIGATOR_INIT, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );
    }
}


/*************************************************************************
|*
\************************************************************************/

void SdTabControl::ActivatePage()
{
    if ( /*IsInSwitching && */ pDrViewSh->IsSwitchPageAllowed() )
    {
        SfxDispatcher* pDispatcher = pDrViewSh->GetViewFrame()->GetDispatcher();
        pDispatcher->Execute(SID_SWITCHPAGE,
                             SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
    }
}


/*************************************************************************
|*
\************************************************************************/

long SdTabControl::DeactivatePage()
{
    return pDrViewSh->IsSwitchPageAllowed();
}




void SdTabControl::SendActivatePageEvent (void)
{
    CallEventListeners (VCLEVENT_TABBAR_PAGEACTIVATED,
        reinterpret_cast<void*>(GetCurPageId()));
}




void SdTabControl::SendDeactivatePageEvent (void)
{
    CallEventListeners (VCLEVENT_TABBAR_PAGEDEACTIVATED,
        reinterpret_cast<void*>(GetCurPageId()));
}
