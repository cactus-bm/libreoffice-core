/*************************************************************************
 *
 *  $RCSfile: navigatr.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-27 10:57:42 $
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

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _UNOTOOLS_LOCALFILEHELPER_HXX
#include <unotools/localfilehelper.hxx>
#endif
#ifndef _SFXIMGMGR_HXX //autogen
#include <sfx2/imgmgr.hxx>
#endif
#ifndef _SFX_FCONTNR_HXX //autogen
#include <sfx2/fcontnr.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX //autogen
#include <sfx2/docfilt.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#include <svx/svxids.hrc>
#ifndef _SV_MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif

#ifndef _SFXDOCKWIN_HXX
#include <sfx2/dockwin.hxx>
#endif

#include "pres.hxx"
#include "navigatr.hxx"
#include "navichld.hxx"
#include "navigatr.hrc"
#include "pgjump.hxx"
#include "app.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"
#include "drawdoc.hxx"
#include "docshell.hxx"
#include "sdresid.hxx"
#include "viewshel.hxx"
#include "drviewsh.hxx"
#include "fuslshow.hxx"
#include "helpids.h"


SFX_IMPL_CHILDWINDOWCONTEXT( SdNavigatorChildWindow, SID_NAVIGATOR )

/*************************************************************************
|*  SdNavigatorWin - FloatingWindow
\************************************************************************/

SdNavigatorWin::SdNavigatorWin( Window* pParent,
                SdNavigatorChildWindow* pChWinCtxt,
                const SdResId& rSdResId, SfxBindings* pInBindings ) :
        Window          ( pParent, rSdResId ),
        aToolbox        ( this, SdResId( 1 ) ),
        aTlbObjects     ( this, SdResId( TLB_OBJECTS ), TRUE ),
        aLbDocs         ( this, SdResId( LB_DOCS ) ),
        pBindings       ( pInBindings ),
        pChildWinContext( pChWinCtxt ),
        maImageList     ( SdResId( IL_NAVIGATR ) ),
        maImageListH    ( SdResId( ILH_NAVIGATR ) ),
        // Bei Aenderung des DragTypes: SelectionMode der TLB anpassen!
        eDragType       ( NAVIGATOR_DRAGTYPE_EMBEDDED ),
        bDocImported    ( FALSE )
{
    aTlbObjects.SetViewFrame( pBindings->GetDispatcher()->GetFrame() );

    FreeResource();

    pNavigatorCtrlItem = new SdNavigatorControllerItem( SID_NAVIGATOR_STATE, this, pBindings );
    pPageNameCtrlItem = new SdPageNameControllerItem( SID_NAVIGATOR_PAGENAME, this, pBindings );
    pDocList = new List();

    aSize = GetOutputSizePixel();
    // Da es jetzt kein SetMinOutputSizePixel mehr gibt:
    aMinSize = aSize;
    aMinSize.Height() -= 40;
    ((SfxDockingWindow*)GetParent())->SetMinOutputSizePixel( aMinSize );

    // ToolBox (Groesse anpassen funktioniert sowieso nicht!)
    Size aTbxSize( aToolbox.CalcWindowSizePixel() );
    aTbxSize.Width() = aSize.Width() - 6; //Rand
    aTbxSize.Height() += 1;  // AF: Work arround for #105145#
    aToolbox.SetOutputSizePixel( aTbxSize );
    aToolbox.SetSelectHdl( LINK( this, SdNavigatorWin, SelectToolboxHdl ) );
    aToolbox.SetClickHdl( LINK( this, SdNavigatorWin, ClickToolboxHdl ) );
    pBindings->GetImageManager()->RegisterToolBox( &aToolbox, SFX_TOOLBOX_CHANGEOUTSTYLE );

    // TreeListBox
    aTlbObjects.SetDoubleClickHdl( LINK( this, SdNavigatorWin, ClickObjectHdl ) );
    aTlbObjects.SetSelectionMode( SINGLE_SELECTION );

    // DragTypeListBox
    aLbDocs.SetSelectHdl( LINK( this, SdNavigatorWin, SelectDocumentHdl ) );

    // InitTlb; Wird ueber Slot initiiert
    SfxBoolItem aItem( SID_NAVIGATOR_INIT, TRUE );
    pBindings->GetDispatcher()->Execute(
        SID_NAVIGATOR_INIT, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

    ApplyImageList();
}

// -----------------------------------------------------------------------

SdNavigatorWin::~SdNavigatorWin()
{
    pBindings->GetImageManager()->ReleaseToolBox( &aToolbox );

    delete pNavigatorCtrlItem;
    delete pPageNameCtrlItem;

    // Liste der DocInfos loeschen
    long nCount = pDocList->Count();
    while( nCount-- )
        delete (NavDocInfo*) pDocList->Remove( (ULONG)0 );

    delete pDocList;
}

// -----------------------------------------------------------------------

void SdNavigatorWin::InitTreeLB( const SdDrawDocument* pDoc )
{
    SdDrawDocument* pNonConstDoc = (SdDrawDocument*) pDoc; // const as const can...
    SdDrawDocShell* pDocShell = pNonConstDoc->GetDocSh();
    String aDocShName( pDocShell->GetName() );

    if( !aTlbObjects.IsEqualToDoc( pDoc ) )
    {
        String aDocName = pDocShell->GetMedium()->GetName();
        aTlbObjects.Clear();
        aTlbObjects.Fill( pDoc, (BOOL) FALSE, aDocName ); // Nur normale Seiten

        RefreshDocumentLB();
        aLbDocs.SelectEntry( aDocShName );
    }
    else
    {
        aLbDocs.SetNoSelection();
        aLbDocs.SelectEntry( aDocShName );

// auskommentiert um 30246 zu fixen
//        if( aLbDocs.GetSelectEntryCount() == 0 )
        {
            RefreshDocumentLB();
            aLbDocs.SelectEntry( aDocShName );
        }
    }

    SdViewShell* pViewShell = pDocShell->GetViewShell();

    ( ( pViewShell && pViewShell->GetViewFrame() ) ? pViewShell->GetViewFrame() : SfxViewFrame::Current() )->
        GetBindings().Invalidate(SID_NAVIGATOR_PAGENAME, TRUE, TRUE);
}

/*************************************************************************
|*
|* DragType wird in Abhaengigkeit davon gesetzt, ob ein Drag ueberhaupt
|* moeglich ist. Graphiken duerfen beispielsweise unter gewissen Umstaenden
|* nicht gedragt werden (#31038#).
|*
\************************************************************************/

NavigatorDragType SdNavigatorWin::GetNavigatorDragType()
{
    NavigatorDragType   eDT = eDragType;
    NavDocInfo*         pInfo = GetDocInfo();

    if( ( eDT == NAVIGATOR_DRAGTYPE_LINK ) && ( ( pInfo && !pInfo->HasName() ) || !aTlbObjects.IsLinkableSelected() ) )
        eDT = NAVIGATOR_DRAGTYPE_NONE;

    return( eDT );
}

// -----------------------------------------------------------------------


IMPL_LINK( SdNavigatorWin, SelectToolboxHdl, void *, EMPTYARG )
{
    USHORT nId = aToolbox.GetCurItemId();
    USHORT nSId = 0;
    PageJump ePage = PAGE_NONE;

    switch( nId )
    {
        case TBI_LIVE:
        case TBI_PEN:
        {
            if( nId == TBI_LIVE )
            {
                nSId = SID_LIVE_PRESENTATION;
                aToolbox.EnableItem( TBI_PEN, FALSE );
            }
            else if( nId == TBI_PEN )
            {
                nSId = SID_NAVIGATOR_PEN;
                aToolbox.EnableItem( TBI_LIVE, FALSE );
            }

            if( nSId > 0 )
            {
                SfxBoolItem aItem( nSId, TRUE );
                pBindings->GetDispatcher()->Execute(
                    nSId, SFX_CALLMODE_SLOT |SFX_CALLMODE_RECORD, &aItem, 0L );
            }
        }
        break;

        case TBI_FIRST:
        case TBI_PREVIOUS:
        case TBI_NEXT:
        case TBI_LAST:
        {
            if( nId == TBI_FIRST )
                ePage = PAGE_FIRST;
            else if( nId == TBI_PREVIOUS )
                ePage = PAGE_PREVIOUS;
            else if( nId == TBI_NEXT )
                ePage = PAGE_NEXT;
            else if( nId == TBI_LAST )
                ePage = PAGE_LAST;

            if( ePage != PAGE_NONE )
            {
                SfxUInt16Item aItem( SID_NAVIGATOR_PAGE, ePage );
                pBindings->GetDispatcher()->Execute(
                    SID_NAVIGATOR_PAGE, SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD, &aItem, 0L );
            }
        }
        break;
    }
    return 0;
}

// -----------------------------------------------------------------------


IMPL_LINK( SdNavigatorWin, ClickToolboxHdl, ToolBox*, pBox )
{
    USHORT nId = aToolbox.GetCurItemId();

    switch( nId )
    {
        case TBI_DRAGTYPE:
        {
            // Popup-Menu wird in Abhaengigkeit davon erzeugt, ob Dokument
            // gespeichert ist oder nicht
            PopupMenu *pMenu = new PopupMenu;

            for( USHORT nID = NAVIGATOR_DRAGTYPE_URL;
                 nID < NAVIGATOR_DRAGTYPE_COUNT;
                 nID++ )
            {
                USHORT nRId = GetDragTypeSdResId( (NavigatorDragType)nID );
                if( nRId > 0 )
                {
                    pMenu->InsertItem( nID, String( SdResId( nRId ) ) );
                    pMenu->SetHelpId( nID, HID_SD_NAVIGATOR_MENU1 +
                                            nID - NAVIGATOR_DRAGTYPE_URL );
                }

            }
            NavDocInfo* pInfo = GetDocInfo();

            if( ( pInfo && !pInfo->HasName() ) || !aTlbObjects.IsLinkableSelected() )
            {
                pMenu->EnableItem( NAVIGATOR_DRAGTYPE_LINK, FALSE );
                pMenu->EnableItem( NAVIGATOR_DRAGTYPE_URL, FALSE );
                eDragType = NAVIGATOR_DRAGTYPE_EMBEDDED;
            }

            pMenu->CheckItem( eDragType );
            pMenu->SetSelectHdl( LINK( this, SdNavigatorWin, MenuSelectHdl ) );

            pMenu->Execute( this, aToolbox.GetItemRect( nId ).BottomLeft() );
            pBox->EndSelection();
            delete pMenu;
            //pBox->Invalidate();
        }
        break;
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SdNavigatorWin, ClickObjectHdl, void *, p )
{
    if( !bDocImported || aLbDocs.GetSelectEntryPos() != 0 )
    {
        NavDocInfo* pInfo = GetDocInfo();

        // Nur wenn es sich um das aktive Fenster handelt, wird
        // auf die Seite gesprungen
        if( pInfo && pInfo->IsActive() )
        {
            String aStr( aTlbObjects.GetSelectEntry() );

            if( aStr.Len() > 0 )
            {
                SfxStringItem aItem( SID_NAVIGATOR_OBJECT, aStr );
                pBindings->GetDispatcher()->Execute(
                    SID_NAVIGATOR_OBJECT, SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD, &aItem, 0L );

                // #98821# moved here from SetGetFocusHdl. Reset the
                // focus only if something has been selected in the
                // document.
                SfxViewShell* pCurSh = SfxViewShell::Current();

                if ( pCurSh )
                {
                    Window* pShellWnd = pCurSh->GetWindow();
                    if ( pShellWnd )
                        pShellWnd->GrabFocus();
                }
            }
        }
    }
    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( SdNavigatorWin, SelectDocumentHdl, void *, p )
{
    String aStrLb = aLbDocs.GetSelectEntry();
    long   nPos = aLbDocs.GetSelectEntryPos();
    BOOL   bFound = FALSE;
    SdDrawDocShell* pDocShell = NULL;
    NavDocInfo* pInfo = GetDocInfo();

    // Handelt es sich um ein gedragtes Objekt?
    if( bDocImported && nPos == 0 )
    {
        // Dokument in TLB aufbauen
        InsertFile( aStrLb );
    }
    else if (pInfo)
    {
        pDocShell = pInfo->pDocShell;

        bFound = TRUE;
    }

    if( bFound )
    {
        /* z.Z. wird das Dokument nicht mehr aktiviert
        SdViewShell* pViewShell = pDocShell->GetViewShell();
        if( pViewShell )
            pViewShell->GetViewFrame()->GetWindow().ToTop();
        */
        SdDrawDocument* pDoc = pDocShell->GetDoc();
        if( !aTlbObjects.IsEqualToDoc( pDoc ) )
        {
            SdDrawDocument* pNonConstDoc = (SdDrawDocument*) pDoc; // const as const can...
            SdDrawDocShell* pDocShell = pNonConstDoc->GetDocSh();
            String aDocName = pDocShell->GetMedium()->GetName();
            aTlbObjects.Clear();
            aTlbObjects.Fill( pDoc, (BOOL) FALSE, aDocName ); // Nur normale Seiten
        }
    }

    // Pruefen, ob Link oder URL moeglich ist
    if( ( pInfo && !pInfo->HasName() ) || !aTlbObjects.IsLinkableSelected() || ( eDragType != NAVIGATOR_DRAGTYPE_EMBEDDED ) )
    {
        eDragType = NAVIGATOR_DRAGTYPE_EMBEDDED;
        SetDragImage();
    }

    return( 0L );
}

/*************************************************************************
|*
|* DrageType wird gesetzt und Image wird entspr. gesetzt.
|* Sollte Handler mit NULL gerufen werden, so wird der Default (URL) gesetzt.
|*
\************************************************************************/

IMPL_LINK( SdNavigatorWin, MenuSelectHdl, Menu *, pMenu )
{
    USHORT nMenuId;
    if( pMenu )
        nMenuId = pMenu->GetCurItemId();
    else
        nMenuId = NAVIGATOR_DRAGTYPE_URL;

    if( nMenuId != USHRT_MAX ) // Notwendig ?
    {
        NavigatorDragType eDT = (NavigatorDragType) nMenuId;
        if( eDragType != eDT )
        {
            eDragType = eDT;
            SetDragImage();

            if( eDragType == NAVIGATOR_DRAGTYPE_URL )
            {
                // Fix, um Endlosschleife zu unterbinden
                if( aTlbObjects.GetSelectionCount() > 1 )
                    aTlbObjects.SelectAll( FALSE );

                aTlbObjects.SetSelectionMode( SINGLE_SELECTION );
            }
            else
                aTlbObjects.SetSelectionMode( MULTIPLE_SELECTION );
        }
    }
    return( 0 );
}

// -----------------------------------------------------------------------

void SdNavigatorWin::Resize()
{
    Size aWinSize( GetOutputSizePixel() );
    if( aWinSize.Height() >= aMinSize.Height() )
        //aWinSize.Width() >= aMinSize.Width() )
    {
        Size aDiffSize;
        aDiffSize.Width() = aWinSize.Width() - aSize.Width();
        aDiffSize.Height() = aWinSize.Height() - aSize.Height();

        // Umgroessern der Toolbox
        Size aObjSize( aToolbox.GetOutputSizePixel() );
        aObjSize.Width() += aDiffSize.Width();
        aToolbox.SetOutputSizePixel( aObjSize );

        // Umgroessern der TreeLB
        aObjSize = aTlbObjects.Control::GetOutputSizePixel();
        aObjSize.Width() += aDiffSize.Width();
        aObjSize.Height() += aDiffSize.Height();
        aTlbObjects.SetOutputSizePixel( aObjSize );

        Point aPt( 0, aDiffSize.Height() );

        // Verschieben der anderen Controls (DocumentLB)
        aLbDocs.Hide();
        aObjSize = aLbDocs.GetOutputSizePixel();
        aObjSize.Width() += aDiffSize.Width();
        aLbDocs.SetPosPixel( aLbDocs.GetPosPixel() + aPt );
        aLbDocs.SetOutputSizePixel( aObjSize );
        aLbDocs.Show();

        aSize = aWinSize;
    }
    Window::Resize();
}

// -----------------------------------------------------------------------

BOOL SdNavigatorWin::InsertFile(const String& rFileName)
{
    INetURLObject   aURL( rFileName );
    BOOL            bReturn = TRUE;

    if( aURL.GetProtocol() == INET_PROT_NOT_VALID )
    {
        String aURLStr;
        ::utl::LocalFileHelper::ConvertPhysicalNameToURL( rFileName, aURLStr );
        aURL = INetURLObject( aURLStr );
    }

    // get adjusted FileName
    String aFileName( aURL.GetMainURL( INetURLObject::NO_DECODE ) );

    if (!aFileName.Len())
    {
        /**********************************************************************
        * Wieder aktuelles Dokument anzeigen
        **********************************************************************/
        aDropFileName = aFileName;
    }
    else
    {
        /**********************************************************************
        * Hineingedraggtes Dokument anzeigen
        **********************************************************************/
        const SfxFilter* pFilter = NULL;
        ErrCode nErr = 0;

        if (aFileName != aDropFileName)
        {
            SfxMedium aMed(aFileName, (STREAM_READ | STREAM_SHARE_DENYNONE), FALSE);
            nErr = SFX_APP()->GetFilterMatcher().GuessFilter(aMed, &pFilter);

            if (pFilter->GetFilterContainer() !=
                SdDrawDocShell::Factory().GetFilterContainer())
            {
                pFilter = NULL;
            }
        }

        if ((pFilter && !nErr) || aFileName == aDropFileName)
        {
            // Das Medium muss ggf. mit READ/WRITE geoeffnet werden, daher wird
            // ersteinmal nachgeschaut, ob es einen Storage enthaelt
            SfxMedium* pMedium = new SfxMedium( aFileName,
                                                STREAM_READ | STREAM_NOCREATE,
                                                TRUE);                // Download

            if (pMedium->IsStorage())
            {
                // Jetzt modusabhaengig:
                // aTlbObjects.SetSelectionMode(MULTIPLE_SELECTION);
                // Eigentuemeruebergabe von pMedium;
                SdDrawDocument* pDropDoc = aTlbObjects.GetBookmarkDoc(pMedium);

                if (pDropDoc)
                {
                    aTlbObjects.Clear();
                    aDropFileName = aFileName;

                    if( !aTlbObjects.IsEqualToDoc( pDropDoc ) )
                    {
                        // Nur normale Seiten
                        aTlbObjects.Fill(pDropDoc, (BOOL) FALSE, aDropFileName);
                        RefreshDocumentLB( &aDropFileName );
                    }
                }
            }
            else
            {
                bReturn = FALSE;
                delete pMedium;
            }
        }
        else
        {
            bReturn = FALSE;
        }
    }

    return (bReturn);
}

// -----------------------------------------------------------------------

void SdNavigatorWin::RefreshDocumentLB( const String* pDocName )
{
    USHORT nPos = 0;

    if( pDocName )
    {
        if( bDocImported )
            aLbDocs.RemoveEntry( 0 );

        aLbDocs.InsertEntry( *pDocName, 0 );
        bDocImported = TRUE;
    }
    else
    {
        nPos = aLbDocs.GetSelectEntryPos();
        if( nPos == LISTBOX_ENTRY_NOTFOUND )
            nPos = 0;

        String aStr;
        if( bDocImported )
            aStr = aLbDocs.GetEntry( 0 );

        aLbDocs.Clear();

        // Liste der DocInfos loeschen
        long nCount = pDocList->Count();
        while( nCount-- )
            delete (NavDocInfo*) pDocList->Remove( (ULONG)0 );

        if( bDocImported )
            aLbDocs.InsertEntry( aStr, 0 );

        SdDrawDocShell* pCurrentDocShell = PTR_CAST( SdDrawDocShell, SfxObjectShell::Current() );
        SfxObjectShell* pSfxDocShell = SfxObjectShell::GetFirst(0, FALSE);
        while( pSfxDocShell )
        {
            SdDrawDocShell* pDocShell = PTR_CAST( SdDrawDocShell, pSfxDocShell );
            if( pDocShell  && !pDocShell->IsInDestruction() && ( pDocShell->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED ) )
            {
                NavDocInfo* pInfo = new NavDocInfo();
                pInfo->pDocShell = pDocShell;

                aStr = pDocShell->GetMedium()->GetName();
                if( aStr.Len() )
                    pInfo->SetName();
                else
                    pInfo->SetName( FALSE );
                // z.Z. wird wieder der Name der Shell genommen (also ohne Pfad)
                // da Koose es als Fehler ansieht, wenn er Pfad in URL-Notation
                // angezeigt wird!
                aStr = pDocShell->GetName();

                aLbDocs.InsertEntry( aStr, LISTBOX_APPEND );

                //
                if( pDocShell == pCurrentDocShell )
                    pInfo->SetActive();
                else
                    pInfo->SetActive( FALSE );

                pDocList->Insert( pInfo, LIST_APPEND );
            }
            pSfxDocShell = SfxObjectShell::GetNext( *pSfxDocShell, 0 , FALSE );
        }
    }
    aLbDocs.SelectEntryPos( nPos );
}

//------------------------------------------------------------------------

USHORT SdNavigatorWin::GetDragTypeSdResId( NavigatorDragType eDT, BOOL bImage )
{
    switch( eDT )
    {
        case NAVIGATOR_DRAGTYPE_NONE:
                return( bImage ? 0 : STR_NONE );
        case NAVIGATOR_DRAGTYPE_URL:
                return( bImage ? TBI_HYPERLINK : STR_DRAGTYPE_URL );
        case NAVIGATOR_DRAGTYPE_EMBEDDED:
                return( bImage ? TBI_EMBEDDED : STR_DRAGTYPE_EMBEDDED );
        case NAVIGATOR_DRAGTYPE_LINK:
                return( bImage ? TBI_LINK : STR_DRAGTYPE_LINK );
        default: DBG_ERROR( "Keine Resource fuer DragType vorhanden!" );
    }
    return( 0 );
}

//------------------------------------------------------------------------

NavDocInfo* SdNavigatorWin::GetDocInfo()
{
    long nPos = aLbDocs.GetSelectEntryPos();

    if( bDocImported )
    {
        if( nPos == 0 )
        {
            return( NULL );
        }
        nPos--;
    }

    NavDocInfo* pInfo = (NavDocInfo*)pDocList->GetObject( nPos );

    return( pInfo );
}

/*************************************************************************
|*
|* PreParentNotify
|*
\************************************************************************/

long SdNavigatorWin::ParentNotify(NotifyEvent& rNEvt)
{
    const KeyEvent* pKEvt = rNEvt.GetKeyEvent();
    long            nOK = FALSE;

    if( pKEvt )
    {
        if( KEY_ESCAPE == pKEvt->GetKeyCode().GetCode() )
        {
            if( SdPageObjsTLB::IsInDrag() )
            {
                // during drag'n'drop we just stop the drag but do not close the navigator
                nOK = TRUE;
            }
            else
            {
                SdViewShell* pViewShell = (SdViewShell*) pBindings->GetDispatcher()->GetFrame()->GetViewShell();

                if( pViewShell )
                {
                    FuSlideShow* pFuSlideShow = pViewShell->GetSlideShow();

                    if( pFuSlideShow )
                    {
                        nOK = TRUE;
                        pFuSlideShow->Terminate();
                    }
                }
            }
        }
    }

    if( !nOK )
        nOK = Window::Notify( rNEvt );

    return( nOK );
}


/*************************************************************************
|*
|* KeyInput: ESCAPE abfangen, um Show zu beenden
|*
\************************************************************************/

void SdNavigatorWin::KeyInput( const KeyEvent& rKEvt )
{
    long nOK = FALSE;

    if (rKEvt.GetKeyCode().GetCode() == KEY_ESCAPE)
    {
        if( SdPageObjsTLB::IsInDrag() )
        {
            // during drag'n'drop we just stop the drag but do not close the navigator
            nOK = TRUE;
        }
        else
        {
            SdViewShell* pViewShell = (SdViewShell*) pBindings->GetDispatcher()->GetFrame()->GetViewShell();

            if (pViewShell)
            {
                FuSlideShow* pFuSlideShow = pViewShell->GetSlideShow();

                if (pFuSlideShow && !pFuSlideShow->IsLivePresentation())
                {
                    // Im Native-Mode soll ESC die Pr�sentation beenden
                    nOK = TRUE;
                    pFuSlideShow->Terminate();
                }
            }
        }
    }

    if (!nOK)
    {
        Window::KeyInput(rKEvt);
    }
}

void SdNavigatorWin::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            ApplyImageList();

    Window::DataChanged( rDCEvt );
}

void SdNavigatorWin::SetDragImage()
{
    aToolbox.SetItemImage( TBI_DRAGTYPE, aToolbox.GetImageList().GetImage( GetDragTypeSdResId( eDragType, TRUE ) ) );
}

void SdNavigatorWin::ApplyImageList()
{
    const bool bHighContrast = GetDisplayBackground().GetColor().IsDark() != 0;

    aToolbox.SetImageList( bHighContrast ? maImageListH : maImageList );

    SetDragImage();
}


/*************************************************************************
|*
|* Ableitung vom SfxChildWindowContext als "Behaelter" fuer Animator
|*
\************************************************************************/

SdNavigatorChildWindow::SdNavigatorChildWindow( Window* pParent,
                                                         USHORT nId,
                                                         SfxBindings* pBindings,
                                                         SfxChildWinInfo* pInfo ) :
    SfxChildWindowContext( nId )
{
    SdNavigatorWin* pNavWin = new SdNavigatorWin( pParent, this,
                                        SdResId( FLT_NAVIGATOR ), pBindings );

    SetWindow( pNavWin );
    //pWindow = pNavWin;

    //eChildAlignment = SFX_ALIGN_NOALIGNMENT;
    //pNavWin->Initialize( pInfo );
}

/*************************************************************************
|*
|* ControllerItem fuer Navigator
|*
\************************************************************************/

SdNavigatorControllerItem::SdNavigatorControllerItem( USHORT nId,
                                SdNavigatorWin* pNavWin,
                                SfxBindings*    pBindings) :
    SfxControllerItem( nId, *pBindings ),
    pNavigatorWin( pNavWin )
{
}

// -----------------------------------------------------------------------

void SdNavigatorControllerItem::StateChanged( USHORT nSId,
                        SfxItemState eState, const SfxPoolItem* pItem )
{
    if( eState >= SFX_ITEM_AVAILABLE && nSId == SID_NAVIGATOR_STATE )
    {
        const SfxUInt32Item* pStateItem = PTR_CAST( SfxUInt32Item, pItem );
        DBG_ASSERT( pStateItem, "SfxUInt16Item erwartet");
        UINT32 nState = pStateItem->GetValue();

        // Live
        if( nState & NAVBTN_LIVE_ENABLED &&
            !pNavigatorWin->aToolbox.IsItemEnabled( TBI_LIVE ) )
            pNavigatorWin->aToolbox.EnableItem( TBI_LIVE );
        if( nState & NAVBTN_LIVE_DISABLED &&
            pNavigatorWin->aToolbox.IsItemEnabled( TBI_LIVE ) )
            pNavigatorWin->aToolbox.EnableItem( TBI_LIVE, FALSE );
        if( nState & NAVBTN_LIVE_CHECKED &&
            !pNavigatorWin->aToolbox.IsItemChecked( TBI_LIVE ) )
            pNavigatorWin->aToolbox.CheckItem( TBI_LIVE );
        if( nState & NAVBTN_LIVE_UNCHECKED &&
            pNavigatorWin->aToolbox.IsItemChecked( TBI_LIVE ) )
            pNavigatorWin->aToolbox.CheckItem( TBI_LIVE, FALSE );

        // Stift
        if( nState & NAVBTN_PEN_ENABLED &&
            !pNavigatorWin->aToolbox.IsItemEnabled( TBI_PEN ) )
            pNavigatorWin->aToolbox.EnableItem( TBI_PEN );
        if( nState & NAVBTN_PEN_DISABLED &&
            pNavigatorWin->aToolbox.IsItemEnabled( TBI_PEN ) )
            pNavigatorWin->aToolbox.EnableItem( TBI_PEN, FALSE );
        if( nState & NAVBTN_PEN_CHECKED &&
            !pNavigatorWin->aToolbox.IsItemChecked( TBI_PEN ) )
            pNavigatorWin->aToolbox.CheckItem( TBI_PEN );
        if( nState & NAVBTN_PEN_UNCHECKED &&
            pNavigatorWin->aToolbox.IsItemChecked( TBI_PEN ) )
            pNavigatorWin->aToolbox.CheckItem( TBI_PEN, FALSE );

        // Nur wenn Doc in LB das Aktive ist
        NavDocInfo* pInfo = pNavigatorWin->GetDocInfo();
        if( pInfo && pInfo->IsActive() )
        {
            // First
            if( nState & NAVBTN_FIRST_ENABLED &&
                !pNavigatorWin->aToolbox.IsItemEnabled( TBI_FIRST ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_FIRST );
            if( nState & NAVBTN_FIRST_DISABLED &&
                pNavigatorWin->aToolbox.IsItemEnabled( TBI_FIRST ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_FIRST, FALSE );

            // Prev
            if( nState & NAVBTN_PREV_ENABLED &&
                !pNavigatorWin->aToolbox.IsItemEnabled( TBI_PREVIOUS ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_PREVIOUS );
            if( nState & NAVBTN_PREV_DISABLED &&
                pNavigatorWin->aToolbox.IsItemEnabled( TBI_PREVIOUS ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_PREVIOUS, FALSE );

            // Last
            if( nState & NAVBTN_LAST_ENABLED &&
                !pNavigatorWin->aToolbox.IsItemEnabled( TBI_LAST ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_LAST );
            if( nState & NAVBTN_LAST_DISABLED &&
                pNavigatorWin->aToolbox.IsItemEnabled( TBI_LAST ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_LAST, FALSE );

            // Next
            if( nState & NAVBTN_NEXT_ENABLED &&
                !pNavigatorWin->aToolbox.IsItemEnabled( TBI_NEXT ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_NEXT );
            if( nState & NAVBTN_NEXT_DISABLED &&
                pNavigatorWin->aToolbox.IsItemEnabled( TBI_NEXT ) )
                pNavigatorWin->aToolbox.EnableItem( TBI_NEXT, FALSE );

            if( nState & NAVTLB_UPDATE )
            {
                // InitTlb; Wird ueber Slot initiiert
                SfxBoolItem aItem( SID_NAVIGATOR_INIT, TRUE );
                GetBindings().GetDispatcher()->Execute(
                    SID_NAVIGATOR_INIT, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );
            }
        }
    }
}

/*************************************************************************
|*
|* ControllerItem fuer Navigator, um die Seite in der TreeLB anzuzeigen
|*
\************************************************************************/

SdPageNameControllerItem::SdPageNameControllerItem( USHORT nId,
                                SdNavigatorWin* pNavWin,
                                SfxBindings*    pBindings) :
    SfxControllerItem( nId, *pBindings ),
    pNavigatorWin( pNavWin )
{
}

// -----------------------------------------------------------------------

void SdPageNameControllerItem::StateChanged( USHORT nSId,
                        SfxItemState eState, const SfxPoolItem* pItem )
{
    if( eState >= SFX_ITEM_AVAILABLE && nSId == SID_NAVIGATOR_PAGENAME )
    {
        // Nur wenn Doc in LB das Aktive ist
        NavDocInfo* pInfo = pNavigatorWin->GetDocInfo();
        if( pInfo && pInfo->IsActive() )
        {
            const SfxStringItem* pStateItem = PTR_CAST( SfxStringItem, pItem );
            DBG_ASSERT( pStateItem, "SfxStringItem erwartet");
            String aPageName = pStateItem->GetValue();

            if( !pNavigatorWin->aTlbObjects.HasSelectedChilds( aPageName ) )
            {
                if( pNavigatorWin->aTlbObjects.GetSelectionMode() == MULTIPLE_SELECTION )
                {
                    // Weil sonst immer dazuselektiert wird
                    pNavigatorWin->aTlbObjects.SelectAll( FALSE );
                }
                pNavigatorWin->aTlbObjects.SelectEntry( aPageName );
            }
        }
    }
}



