/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: navigatr.cxx,v $
 *
 *  $Revision: 1.36 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 12:42:23 $
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

#include <sfx2/viewfrm.hxx>

#ifndef _SFXDOCKWIN_HXX
#include <sfx2/dockwin.hxx>
#endif

#include "pres.hxx"
#include "navigatr.hxx"
//#include "navichld.hxx"
#include "navigatr.hrc"
#include "pgjump.hxx"
#include "app.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "sdresid.hxx"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef _SD_SLIDESHOW_HXX
#include "slideshow.hxx"
#endif
#include "helpids.h"



/*************************************************************************
|*  SdNavigatorWin - FloatingWindow
\************************************************************************/

SdNavigatorWin::SdNavigatorWin(
    ::Window* pParent,
    ::sd::NavigatorChildWindow* pChWinCtxt,
    const SdResId& rSdResId,
    SfxBindings* pInBindings )
:   ::Window( pParent, rSdResId )
,   maToolbox        ( this, SdResId( 1 ) )
,   maTlbObjects( this, SdResId( TLB_OBJECTS ) )
,   maLbDocs         ( this, SdResId( LB_DOCS ) )
,   mpChildWinContext( pChWinCtxt )
,   mbDocImported   ( FALSE )
    // Bei Aenderung des DragTypes: SelectionMode der TLB anpassen!
,   meDragType      ( NAVIGATOR_DRAGTYPE_EMBEDDED )
,   mpBindings      ( pInBindings )
,   maImageList     ( SdResId( IL_NAVIGATR ) )
,   maImageListH    ( SdResId( ILH_NAVIGATR ) )
{
    maTlbObjects.SetViewFrame( mpBindings->GetDispatcher()->GetFrame() );

    FreeResource();

    mpNavigatorCtrlItem = new SdNavigatorControllerItem( SID_NAVIGATOR_STATE, this, mpBindings );
    mpPageNameCtrlItem = new SdPageNameControllerItem( SID_NAVIGATOR_PAGENAME, this, mpBindings );
    mpDocList = new List();

    ApplyImageList(); // load images *before* calculating sizes to get something useful !!!

    Size aTbxSize( maToolbox.CalcWindowSizePixel() );
    maToolbox.SetOutputSizePixel( aTbxSize );
    maToolbox.SetSelectHdl( LINK( this, SdNavigatorWin, SelectToolboxHdl ) );
    maToolbox.SetClickHdl( LINK( this, SdNavigatorWin, ClickToolboxHdl ) );
    maToolbox.SetDropdownClickHdl( LINK(this, SdNavigatorWin, DropdownClickToolBoxHdl) );
    maToolbox.SetItemBits( TBI_DRAGTYPE, maToolbox.GetItemBits( TBI_DRAGTYPE ) | TIB_DROPDOWNONLY );

    // TreeListBox
    // set position below toolbox
    long nListboxYPos = maToolbox.GetPosPixel().Y() + maToolbox.GetSizePixel().Height() + 4;
    maTlbObjects.SetPosSizePixel( 0, nListboxYPos, 0, 0, WINDOW_POSSIZE_Y );
    maTlbObjects.SetDoubleClickHdl( LINK( this, SdNavigatorWin, ClickObjectHdl ) );
    maTlbObjects.SetSelectionMode( SINGLE_SELECTION );
    // set focus to listbox, otherwise it is in the toolbox which is only useful
    // for keyboard navigation
    maTlbObjects.GrabFocus();

    // DragTypeListBox
    maLbDocs.SetSelectHdl( LINK( this, SdNavigatorWin, SelectDocumentHdl ) );
    // set position below treelistbox
    nListboxYPos = maTlbObjects.GetPosPixel().Y() + maTlbObjects.GetSizePixel().Height() + 4;
    maLbDocs.SetPosSizePixel( 0, nListboxYPos, 0, 0, WINDOW_POSSIZE_Y );

    // set min outputsize after all sizes are known
    long nFullHeight = nListboxYPos + maLbDocs.GetSizePixel().Height() + 4;
    maSize = GetOutputSizePixel();
    if( maSize.Height() < nFullHeight )
    {
        maSize.Height() = nFullHeight;
        SetOutputSizePixel( maSize );
    }
    maMinSize = maSize;
    long nMinWidth = 2*maToolbox.GetPosPixel().X() + aTbxSize.Width(); // never clip the toolbox
    if( nMinWidth > maMinSize.Width() )
        maMinSize.Width() = nMinWidth;
    maMinSize.Height() -= 40;
    ((SfxDockingWindow*)GetParent())->SetMinOutputSizePixel( maMinSize );

    // InitTlb; Wird ueber Slot initiiert
    SfxBoolItem aItem( SID_NAVIGATOR_INIT, TRUE );
    mpBindings->GetDispatcher()->Execute(
        SID_NAVIGATOR_INIT, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

}

// -----------------------------------------------------------------------

SdNavigatorWin::~SdNavigatorWin()
{
    delete mpNavigatorCtrlItem;
    delete mpPageNameCtrlItem;

    // Liste der DocInfos loeschen
    long nCount = mpDocList->Count();
    while( nCount-- )
        delete (NavDocInfo*) mpDocList->Remove( (ULONG)0 );

    delete mpDocList;
}

// -----------------------------------------------------------------------

void SdNavigatorWin::InitTreeLB( const SdDrawDocument* pDoc )
{
    SdDrawDocument* pNonConstDoc = (SdDrawDocument*) pDoc; // const as const can...
    ::sd::DrawDocShell* pDocShell = pNonConstDoc->GetDocSh();
    String aDocShName( pDocShell->GetName() );

    if( !maTlbObjects.IsEqualToDoc( pDoc ) )
    {
        String aDocName = pDocShell->GetMedium()->GetName();
        maTlbObjects.Clear();
        maTlbObjects.Fill( pDoc, (BOOL) FALSE, aDocName ); // Nur normale Seiten

        RefreshDocumentLB();
        maLbDocs.SelectEntry( aDocShName );
    }
    else
    {
        maLbDocs.SetNoSelection();
        maLbDocs.SelectEntry( aDocShName );

// auskommentiert um 30246 zu fixen
//        if( maLbDocs.GetSelectEntryCount() == 0 )
        {
            RefreshDocumentLB();
            maLbDocs.SelectEntry( aDocShName );
        }
    }

    ::sd::ViewShell* pViewShell = pDocShell->GetViewShell();
    SfxViewFrame* pViewFrame = ( ( pViewShell && pViewShell->GetViewFrame() ) ? pViewShell->GetViewFrame() : SfxViewFrame::Current() );
    if( pViewFrame )
        pViewFrame->GetBindings().Invalidate(SID_NAVIGATOR_PAGENAME, TRUE, TRUE);
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
    NavigatorDragType   eDT = meDragType;
    NavDocInfo*         pInfo = GetDocInfo();

    if( ( eDT == NAVIGATOR_DRAGTYPE_LINK ) && ( ( pInfo && !pInfo->HasName() ) || !maTlbObjects.IsLinkableSelected() ) )
        eDT = NAVIGATOR_DRAGTYPE_NONE;

    return( eDT );
}

// -----------------------------------------------------------------------


IMPL_LINK( SdNavigatorWin, SelectToolboxHdl, void *, EMPTYARG )
{
    USHORT nId = maToolbox.GetCurItemId();
    USHORT nSId = 0;
    PageJump ePage = PAGE_NONE;

    switch( nId )
    {
        case TBI_PEN:
        {
            if( nId == TBI_PEN )
            {
                nSId = SID_NAVIGATOR_PEN;
            }

            if( nSId > 0 )
            {
                SfxBoolItem aItem( nSId, TRUE );
                mpBindings->GetDispatcher()->Execute(
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
                SfxUInt16Item aItem( SID_NAVIGATOR_PAGE, (UINT16)ePage );
                mpBindings->GetDispatcher()->Execute(
                    SID_NAVIGATOR_PAGE, SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD, &aItem, 0L );
            }
        }
        break;
    }
    return 0;
}

// -----------------------------------------------------------------------


IMPL_LINK( SdNavigatorWin, ClickToolboxHdl, ToolBox*, EMPTYARG )
{
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SdNavigatorWin, DropdownClickToolBoxHdl, ToolBox*, pBox )
{
    USHORT nId = maToolbox.GetCurItemId();

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

            if( ( pInfo && !pInfo->HasName() ) || !maTlbObjects.IsLinkableSelected() )
            {
                pMenu->EnableItem( NAVIGATOR_DRAGTYPE_LINK, FALSE );
                pMenu->EnableItem( NAVIGATOR_DRAGTYPE_URL, FALSE );
                meDragType = NAVIGATOR_DRAGTYPE_EMBEDDED;
            }

            pMenu->CheckItem( (UINT16)meDragType );
            pMenu->SetSelectHdl( LINK( this, SdNavigatorWin, MenuSelectHdl ) );

            pMenu->Execute( this, maToolbox.GetItemRect( nId ), POPUPMENU_EXECUTE_DOWN );
            pBox->EndSelection();
            delete pMenu;
            //pBox->Invalidate();
        }
        break;
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SdNavigatorWin, ClickObjectHdl, void *, EMPTYARG )
{
    if( !mbDocImported || maLbDocs.GetSelectEntryPos() != 0 )
    {
        NavDocInfo* pInfo = GetDocInfo();

        // Nur wenn es sich um das aktive Fenster handelt, wird
        // auf die Seite gesprungen
        if( pInfo && pInfo->IsActive() )
        {
            String aStr( maTlbObjects.GetSelectEntry() );

            if( aStr.Len() > 0 )
            {
                SfxStringItem aItem( SID_NAVIGATOR_OBJECT, aStr );
                mpBindings->GetDispatcher()->Execute(
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

IMPL_LINK( SdNavigatorWin, SelectDocumentHdl, void *, EMPTYARG )
{
    String aStrLb = maLbDocs.GetSelectEntry();
    long   nPos = maLbDocs.GetSelectEntryPos();
    BOOL   bFound = FALSE;
    ::sd::DrawDocShell* pDocShell = NULL;
    NavDocInfo* pInfo = GetDocInfo();

    // Handelt es sich um ein gedragtes Objekt?
    if( mbDocImported && nPos == 0 )
    {
        // Dokument in TLB aufbauen
        InsertFile( aStrLb );
    }
    else if (pInfo)
    {
        pDocShell = pInfo->mpDocShell;

        bFound = TRUE;
    }

    if( bFound )
    {
        SdDrawDocument* pDoc = pDocShell->GetDoc();
        if( !maTlbObjects.IsEqualToDoc( pDoc ) )
        {
            SdDrawDocument* pNonConstDoc = (SdDrawDocument*) pDoc; // const as const can...
            ::sd::DrawDocShell* pNCDocShell = pNonConstDoc->GetDocSh();
            String aDocName = pNCDocShell->GetMedium()->GetName();
            maTlbObjects.Clear();
            maTlbObjects.Fill( pDoc, (BOOL) FALSE, aDocName ); // Nur normale Seiten
        }
    }

    // Pruefen, ob Link oder URL moeglich ist
    if( ( pInfo && !pInfo->HasName() ) || !maTlbObjects.IsLinkableSelected() || ( meDragType != NAVIGATOR_DRAGTYPE_EMBEDDED ) )
    {
        meDragType = NAVIGATOR_DRAGTYPE_EMBEDDED;
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
        if( meDragType != eDT )
        {
            meDragType = eDT;
            SetDragImage();

            if( meDragType == NAVIGATOR_DRAGTYPE_URL )
            {
                // Fix, um Endlosschleife zu unterbinden
                if( maTlbObjects.GetSelectionCount() > 1 )
                    maTlbObjects.SelectAll( FALSE );

                maTlbObjects.SetSelectionMode( SINGLE_SELECTION );
            }
            else
                maTlbObjects.SetSelectionMode( MULTIPLE_SELECTION );
        }
    }
    return( 0 );
}

// -----------------------------------------------------------------------

void SdNavigatorWin::Resize()
{
    Size aWinSize( GetOutputSizePixel() );
    if( aWinSize.Height() >= maMinSize.Height() )
        //aWinSize.Width() >= maMinSize.Width() )
    {
        Size aDiffSize;
        aDiffSize.Width() = aWinSize.Width() - maSize.Width();
        aDiffSize.Height() = aWinSize.Height() - maSize.Height();

        // Umgroessern der Toolbox
        Size aObjSize( maToolbox.GetOutputSizePixel() );
        aObjSize.Width() += aDiffSize.Width();
        maToolbox.SetOutputSizePixel( aObjSize );

        // Umgroessern der TreeLB
        aObjSize = maTlbObjects.Control::GetOutputSizePixel();
        aObjSize.Width() += aDiffSize.Width();
        aObjSize.Height() += aDiffSize.Height();
        maTlbObjects.SetOutputSizePixel( aObjSize );

        Point aPt( 0, aDiffSize.Height() );

        // Verschieben der anderen Controls (DocumentLB)
        maLbDocs.Hide();
        aObjSize = maLbDocs.GetOutputSizePixel();
        aObjSize.Width() += aDiffSize.Width();
        maLbDocs.SetPosPixel( maLbDocs.GetPosPixel() + aPt );
        maLbDocs.SetOutputSizePixel( aObjSize );
        maLbDocs.Show();

        maSize = aWinSize;
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
        maDropFileName = aFileName;
    }
    else
    {
        /**********************************************************************
        * Hineingedraggtes Dokument anzeigen
        **********************************************************************/
        const SfxFilter* pFilter = NULL;
        ErrCode nErr = 0;

        if (aFileName != maDropFileName)
        {
            SfxMedium aMed(aFileName, (STREAM_READ | STREAM_SHARE_DENYNONE), FALSE);
            SfxFilterMatcher aMatch( String::CreateFromAscii("simpress") );
            aMed.UseInteractionHandler( TRUE );
            nErr = aMatch.GuessFilter(aMed, &pFilter);
        }

        if ((pFilter && !nErr) || aFileName == maDropFileName)
        {
            // Das Medium muss ggf. mit READ/WRITE geoeffnet werden, daher wird
            // ersteinmal nachgeschaut, ob es einen Storage enthaelt
            SfxMedium* pMedium = new SfxMedium( aFileName,
                                                STREAM_READ | STREAM_NOCREATE,
                                                TRUE);                // Download

            if (pMedium->IsStorage())
            {
                // Jetzt modusabhaengig:
                // maTlbObjects.SetSelectionMode(MULTIPLE_SELECTION);
                // Eigentuemeruebergabe von pMedium;
                SdDrawDocument* pDropDoc = maTlbObjects.GetBookmarkDoc(pMedium);

                if (pDropDoc)
                {
                    maTlbObjects.Clear();
                    maDropFileName = aFileName;

                    if( !maTlbObjects.IsEqualToDoc( pDropDoc ) )
                    {
                        // Nur normale Seiten
                        maTlbObjects.Fill(pDropDoc, (BOOL) FALSE, maDropFileName);
                        RefreshDocumentLB( &maDropFileName );
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
        if( mbDocImported )
            maLbDocs.RemoveEntry( 0 );

        maLbDocs.InsertEntry( *pDocName, 0 );
        mbDocImported = TRUE;
    }
    else
    {
        nPos = maLbDocs.GetSelectEntryPos();
        if( nPos == LISTBOX_ENTRY_NOTFOUND )
            nPos = 0;

        String aStr;
        if( mbDocImported )
            aStr = maLbDocs.GetEntry( 0 );

        maLbDocs.Clear();

        // Liste der DocInfos loeschen
        long nCount = mpDocList->Count();
        while( nCount-- )
            delete (NavDocInfo*) mpDocList->Remove( (ULONG)0 );

        if( mbDocImported )
            maLbDocs.InsertEntry( aStr, 0 );

        ::sd::DrawDocShell* pCurrentDocShell =
              PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current() );
        SfxObjectShell* pSfxDocShell = SfxObjectShell::GetFirst(0, FALSE);
        while( pSfxDocShell )
        {
            ::sd::DrawDocShell* pDocShell = PTR_CAST(::sd::DrawDocShell, pSfxDocShell );
            if( pDocShell  && !pDocShell->IsInDestruction() && ( pDocShell->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED ) )
            {
                NavDocInfo* pInfo = new NavDocInfo();
                pInfo->mpDocShell = pDocShell;

                aStr = pDocShell->GetMedium()->GetName();
                if( aStr.Len() )
                    pInfo->SetName();
                else
                    pInfo->SetName( FALSE );
                // z.Z. wird wieder der Name der Shell genommen (also ohne Pfad)
                // da Koose es als Fehler ansieht, wenn er Pfad in URL-Notation
                // angezeigt wird!
                aStr = pDocShell->GetName();

                maLbDocs.InsertEntry( aStr, LISTBOX_APPEND );

                //
                if( pDocShell == pCurrentDocShell )
                    pInfo->SetActive();
                else
                    pInfo->SetActive( FALSE );

                mpDocList->Insert( pInfo, LIST_APPEND );
            }
            pSfxDocShell = SfxObjectShell::GetNext( *pSfxDocShell, 0 , FALSE );
        }
    }
    maLbDocs.SelectEntryPos( nPos );
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
    long nPos = maLbDocs.GetSelectEntryPos();

    if( mbDocImported )
    {
        if( nPos == 0 )
        {
            return( NULL );
        }
        nPos--;
    }

    NavDocInfo* pInfo = (NavDocInfo*)mpDocList->GetObject( nPos );

    return( pInfo );
}

/*************************************************************************
|*
|* PreNotify
|*
\************************************************************************/

long SdNavigatorWin::Notify(NotifyEvent& rNEvt)
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
                ::sd::ViewShellBase* pBase =
                    ::sd::ViewShellBase::GetViewShellBase(
                        mpBindings->GetDispatcher()->GetFrame());
                if( pBase )
                    pBase->StopPresentation();
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
            ::sd::ViewShellBase* pBase =
                ::sd::ViewShellBase::GetViewShellBase(
                    mpBindings->GetDispatcher()->GetFrame());
            ::sd::ViewShell* pViewShell = pBase->GetMainViewShell().get();
            if (pViewShell != NULL)
            {
                ::sd::Slideshow* pSlideShow = pViewShell->GetSlideShow();

                if(pSlideShow)
                {
                    nOK = TRUE;
                    pSlideShow->stopShow();
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
    maToolbox.SetItemImage( TBI_DRAGTYPE, maToolbox.GetImageList().GetImage( GetDragTypeSdResId( meDragType, TRUE ) ) );
}

void SdNavigatorWin::ApplyImageList()
{
    const bool bHighContrast = GetDisplayBackground().GetColor().IsDark() != 0;

    maToolbox.SetImageList( bHighContrast ? maImageListH : maImageList );

    SetDragImage();
}



/*************************************************************************
|*
|* ControllerItem fuer Navigator
|*
\************************************************************************/

SdNavigatorControllerItem::SdNavigatorControllerItem( USHORT _nId,
                                SdNavigatorWin* pNavWin,
                                SfxBindings*    _pBindings) :
    SfxControllerItem( _nId, *_pBindings ),
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

        // Stift
        if( nState & NAVBTN_PEN_ENABLED &&
            !pNavigatorWin->maToolbox.IsItemEnabled( TBI_PEN ) )
            pNavigatorWin->maToolbox.EnableItem( TBI_PEN );
        if( nState & NAVBTN_PEN_DISABLED &&
            pNavigatorWin->maToolbox.IsItemEnabled( TBI_PEN ) )
            pNavigatorWin->maToolbox.EnableItem( TBI_PEN, FALSE );
        if( nState & NAVBTN_PEN_CHECKED &&
            !pNavigatorWin->maToolbox.IsItemChecked( TBI_PEN ) )
            pNavigatorWin->maToolbox.CheckItem( TBI_PEN );
        if( nState & NAVBTN_PEN_UNCHECKED &&
            pNavigatorWin->maToolbox.IsItemChecked( TBI_PEN ) )
            pNavigatorWin->maToolbox.CheckItem( TBI_PEN, FALSE );

        // Nur wenn Doc in LB das Aktive ist
        NavDocInfo* pInfo = pNavigatorWin->GetDocInfo();
        if( pInfo && pInfo->IsActive() )
        {
            // First
            if( nState & NAVBTN_FIRST_ENABLED &&
                !pNavigatorWin->maToolbox.IsItemEnabled( TBI_FIRST ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_FIRST );
            if( nState & NAVBTN_FIRST_DISABLED &&
                pNavigatorWin->maToolbox.IsItemEnabled( TBI_FIRST ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_FIRST, FALSE );

            // Prev
            if( nState & NAVBTN_PREV_ENABLED &&
                !pNavigatorWin->maToolbox.IsItemEnabled( TBI_PREVIOUS ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_PREVIOUS );
            if( nState & NAVBTN_PREV_DISABLED &&
                pNavigatorWin->maToolbox.IsItemEnabled( TBI_PREVIOUS ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_PREVIOUS, FALSE );

            // Last
            if( nState & NAVBTN_LAST_ENABLED &&
                !pNavigatorWin->maToolbox.IsItemEnabled( TBI_LAST ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_LAST );
            if( nState & NAVBTN_LAST_DISABLED &&
                pNavigatorWin->maToolbox.IsItemEnabled( TBI_LAST ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_LAST, FALSE );

            // Next
            if( nState & NAVBTN_NEXT_ENABLED &&
                !pNavigatorWin->maToolbox.IsItemEnabled( TBI_NEXT ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_NEXT );
            if( nState & NAVBTN_NEXT_DISABLED &&
                pNavigatorWin->maToolbox.IsItemEnabled( TBI_NEXT ) )
                pNavigatorWin->maToolbox.EnableItem( TBI_NEXT, FALSE );

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

SdPageNameControllerItem::SdPageNameControllerItem( USHORT _nId,
                                SdNavigatorWin* pNavWin,
                                SfxBindings*    _pBindings) :
    SfxControllerItem( _nId, *_pBindings ),
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

            if( !pNavigatorWin->maTlbObjects.HasSelectedChilds( aPageName ) )
            {
                if( pNavigatorWin->maTlbObjects.GetSelectionMode() == MULTIPLE_SELECTION )
                {
                    // Weil sonst immer dazuselektiert wird
                    pNavigatorWin->maTlbObjects.SelectAll( FALSE );
                }
                pNavigatorWin->maTlbObjects.SelectEntry( aPageName );
            }
        }
    }
}
