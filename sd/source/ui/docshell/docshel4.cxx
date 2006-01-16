/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docshel4.cxx,v $
 *
 *  $Revision: 1.70 $
 *
 *  last change: $Author: obo $ $Date: 2006-01-16 15:19:26 $
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

#include "DrawDocShell.hxx"

#ifndef _COM_SUN_STAR_DOCUMENT_PRINTERINDEPENDENTLAYOUT_HPP_
#include <com/sun/star/document/PrinterIndependentLayout.hpp>
#endif

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _SFX_PROGRESS_HXX
#include <sfx2/progress.hxx>
#endif
#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif
#ifndef _SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#define ITEMID_FONTLIST     SID_ATTR_CHAR_FONTLIST
#ifndef _SVX_FLSTITEM_HXX
#include <svx/flstitem.hxx>
#endif
#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif
#ifndef _SFXFLAGITEM_HXX //autogen
#include <svtools/flagitem.hxx>
#endif
#include <sot/storage.hxx>
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX //autogen
#include <sfx2/docfilt.hxx>
#endif
#ifndef _DISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVDOTEXT_HXX //autogen
#include <svx/svdotext.hxx>
#endif
#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif
#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _CTRLTOOL_HXX //autogen
#include <svtools/ctrltool.hxx>
#endif
#ifndef _SFX_ECODE_HXX //autogen
#include <svtools/sfxecode.hxx>
#endif
#include <sot/clsids.hxx>
#ifndef _SOT_FORMATS_HXX //autogen
#include <sot/formats.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifdef TF_STARONE
#include "unomodel.hxx"
#endif

#include <svtools/fltrcfg.hxx>
#include <sfx2/frame.hxx>
#include <sfx2/viewfrm.hxx>

#ifndef _SVXMSBAS_HXX
#include <svx/svxmsbas.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_SAVEOPT_HXX
#include <svtools/saveopt.hxx>
#endif
#ifndef _SD_UNODRAWVIEW_HXX
#include "SdUnoDrawView.hxx"
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGE_HPP_
#include <com/sun/star/drawing/XDrawPage.hpp>
#endif

#pragma hdrstop

#include "app.hrc"
#include "glob.hrc"
#include "strings.hrc"
#include "strmname.h"
#ifndef SD_FRAMW_VIEW_HXX
#include "FrameView.hxx"
#endif
#include "optsitem.hxx"
#ifndef SD_OUTLINER_HXX
#include "Outliner.hxx"
#endif
#include "sdattr.hxx"
#include "drawdoc.hxx"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#include "app.hxx"
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#include "sdpage.hxx"
#include "sdresid.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_PANE_MANAGER_HXX
#include "PaneManager.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
//CHINA001 #include "new_foil.hxx"
#include "sdmod.hxx"
#ifndef SD_OUTLINE_VIEW_SHELL_HXX
#include "OutlineViewShell.hxx"
#endif
#include "sdxmlwrp.hxx"
#include "sdpptwrp.hxx"
#include "sdcgmfilter.hxx"
#include "sdgrffilter.hxx"
#include "sdbinfilter.hxx"
#include "sdhtmlfilter.hxx"

using namespace ::com::sun::star;

namespace sd {

/*************************************************************************
|*
|* SfxPrinter ggf. erzeugen und zurueckgeben
|*
\************************************************************************/

SfxPrinter* DrawDocShell::GetPrinter(BOOL bCreate)
{
    if (bCreate && !pPrinter)
    {
        // ItemSet mit speziellem Poolbereich anlegen
        SfxItemSet* pSet = new SfxItemSet( GetPool(),
                            SID_PRINTER_NOTFOUND_WARN,  SID_PRINTER_NOTFOUND_WARN,
                            SID_PRINTER_CHANGESTODOC,   SID_PRINTER_CHANGESTODOC,
                            ATTR_OPTIONS_PRINT,         ATTR_OPTIONS_PRINT,
                            0 );
        // PrintOptionsSet setzen
        SdOptionsPrintItem aPrintItem( ATTR_OPTIONS_PRINT,
                            SD_MOD()->GetSdOptions(pDoc->GetDocumentType()));
        SfxFlagItem aFlagItem( SID_PRINTER_CHANGESTODOC );
        USHORT      nFlags = 0;

        nFlags =  (aPrintItem.IsWarningSize() ? SFX_PRINTER_CHG_SIZE : 0) |
                (aPrintItem.IsWarningOrientation() ? SFX_PRINTER_CHG_ORIENTATION : 0);
        aFlagItem.SetValue( nFlags );

        pSet->Put( aPrintItem );
        pSet->Put( SfxBoolItem( SID_PRINTER_NOTFOUND_WARN, aPrintItem.IsWarningPrinter() ) );
        pSet->Put( aFlagItem );

        pPrinter = new SfxPrinter(pSet);
        bOwnPrinter = TRUE;

        // Ausgabequalitaet setzen
        UINT16 nQuality = aPrintItem.GetOutputQuality();

        ULONG nMode = DRAWMODE_DEFAULT;

        if( nQuality == 1 )
            nMode = DRAWMODE_GRAYLINE | DRAWMODE_GRAYFILL | DRAWMODE_BLACKTEXT | DRAWMODE_GRAYBITMAP | DRAWMODE_GRAYGRADIENT;
        else if( nQuality == 2 )
            nMode = DRAWMODE_BLACKLINE | DRAWMODE_BLACKTEXT | DRAWMODE_WHITEFILL | DRAWMODE_GRAYBITMAP | DRAWMODE_WHITEGRADIENT;

        pPrinter->SetDrawMode( nMode );

        MapMode aMM (pPrinter->GetMapMode());
        aMM.SetMapUnit(MAP_100TH_MM);
        pPrinter->SetMapMode(aMM);
        UpdateRefDevice();
    }
    return pPrinter;
}

/*************************************************************************
|*
|* neuen SfxPrinter setzen (Eigentuemeruebergang)
|*
\************************************************************************/

void DrawDocShell::SetPrinter(SfxPrinter *pNewPrinter)
{
    if ( pViewShell )
    {
        ::sd::View* pView = pViewShell->GetView();
        if ( pView->IsTextEdit() )
            pView->EndTextEdit();
    }

    if ( pPrinter && bOwnPrinter && (pPrinter != pNewPrinter) )
    {
        delete pPrinter;
    }

    pPrinter = pNewPrinter;
    bOwnPrinter = TRUE;

    delete pFontList;
    pFontList = new FontList( GetPrinter(TRUE), Application::GetDefaultDevice(), FALSE );
    SvxFontListItem aFontListItem( pFontList );
    PutItem( aFontListItem );
    UpdateRefDevice();
}

/*************************************************************************
|*
|*
|*
\************************************************************************/
Printer* DrawDocShell::GetDocumentPrinter()
{
    return GetPrinter(FALSE);
}

/*************************************************************************
|*
|*
|*
\************************************************************************/
void DrawDocShell::OnDocumentPrinterChanged(Printer* pNewPrinter)
{
    // if we already have a printer, see if its the same
    if( pPrinter )
    {
        // easy case
        if( pPrinter == pNewPrinter )
            return;

        // compare if its the same printer with the same job setup
        if( (pPrinter->GetName() == pNewPrinter->GetName()) &&
            (pPrinter->GetJobSetup() == pNewPrinter->GetJobSetup()))
            return;
    }

    //  if (pPrinter->IsA(SfxPrinter))
    {
        // Da kein RTTI verfuegbar, wird hart gecasted (...)
        SetPrinter((SfxPrinter*) pNewPrinter);

        // Printer gehoert dem Container
        bOwnPrinter = FALSE;
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/
void DrawDocShell::UpdateRefDevice()
{
    if( pDoc )
    {
        // Determine the device for which the output will be formatted.
        OutputDevice* pRefDevice = NULL;
        switch (pDoc->GetPrinterIndependentLayout())
        {
            case ::com::sun::star::document::PrinterIndependentLayout::DISABLED:
                pRefDevice = pPrinter;
                break;

            case ::com::sun::star::document::PrinterIndependentLayout::ENABLED:
                pRefDevice = SD_MOD()->GetVirtualRefDevice();
                break;

            default:
                // We are confronted with an invalid or un-implemented
                // layout mode.  Use the printer as formatting device
                // as a fall-back.
                DBG_ASSERT(false, "DrawDocShell::UpdateRefDevice(): Unexpected printer layout mode");

                pRefDevice = pPrinter;
                break;
        }
        pDoc->SetRefDevice( pRefDevice );

        ::sd::Outliner* pOutl = pDoc->GetOutliner( FALSE );

        if( pOutl )
            pOutl->SetRefDevice( pRefDevice );

        ::sd::Outliner* pInternalOutl = pDoc->GetInternalOutliner( FALSE );

        if( pInternalOutl )
            pInternalOutl->SetRefDevice( pRefDevice );
    }
}

/*************************************************************************
|*
|* InitNew, (Dokument wird neu erzeugt): Streams oeffnen
|*
\************************************************************************/

BOOL DrawDocShell::InitNew( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage )
{
    BOOL bRet = FALSE;

    bRet = SfxObjectShell::InitNew( xStorage );

    Rectangle aVisArea( Point(0, 0), Size(14100, 10000) );
    SetVisArea(aVisArea);

    if (bRet)
    {
        if( !bSdDataObj )
            pDoc->NewOrLoadCompleted(NEW_DOC);  // otherwise calling
                                                // NewOrLoadCompleted(NEW_LOADED) in
                                                // SdDrawDocument::AllocModel()
    }
    return bRet;
}

/*************************************************************************
|*
|* Load: Pools und Dokument laden
|*
\************************************************************************/

sal_Bool DrawDocShell::IsNewDocument() const
{
    return( mbNewDocument &&
            ( !GetMedium() || GetMedium()->GetURLObject().GetProtocol() == INET_PROT_NOT_VALID ) );
}

/*************************************************************************
|*
|* Load: Pools und Dokument laden
|*
\************************************************************************/

BOOL DrawDocShell::Load( SfxMedium& rMedium )
{
    mbNewDocument = sal_False;

    BOOL    bRet = FALSE;
    bool    bStartPresentation = false;
    ErrCode nError = ERRCODE_NONE;

    SfxItemSet* pSet = rMedium.GetItemSet();


    if( pSet )
    {
        if( (  SFX_ITEM_SET == pSet->GetItemState(SID_PREVIEW ) ) && ( (SfxBoolItem&) ( pSet->Get( SID_PREVIEW ) ) ).GetValue() )
        {
            pDoc->SetStarDrawPreviewMode( TRUE );
        }

        if( SFX_ITEM_SET == pSet->GetItemState(SID_DOC_STARTPRESENTATION)&&
            ( (SfxBoolItem&) ( pSet->Get( SID_DOC_STARTPRESENTATION ) ) ).GetValue() )
        {
            bStartPresentation = true;
            pDoc->SetStartWithPresentation( true );
        }
    }

    bRet = SfxObjectShell::Load( rMedium );
    if( bRet )
    {
        bRet = SdXMLFilter( rMedium, *this, sal_True, SDXMLMODE_Normal, SotStorage::GetVersion( rMedium.GetStorage() ) ).Import( nError );
    }

    if( bRet )
    {
        UpdateTablePointers();

        // #108451# If we're an embedded OLE object, use tight bounds
        // for our visArea. No point in showing the user lots of empty
        // space. Had to remove the check for empty VisArea below,
        // since XML load always sets a VisArea before.
        //TODO/LATER: looks a little bit strange!
        if( ( GetCreateMode() == SFX_CREATE_MODE_EMBEDDED ) && SfxObjectShell::GetVisArea( ASPECT_CONTENT ).IsEmpty() )
        {
            SdPage* pPage = pDoc->GetSdPage( 0, PK_STANDARD );

            if( pPage )
                SetVisArea( Rectangle( pPage->GetAllObjBoundRect() ) );
        }

        FinishedLoading( SFX_LOADED_ALL );
    }
    else
    {
        if( nError == ERRCODE_IO_BROKENPACKAGE )
            SetError( ERRCODE_IO_BROKENPACKAGE );

        // TODO/LATER: correct error handling?!
        //pStore->SetError( SVSTREAM_WRONGVERSION );
        else
            SetError( ERRCODE_ABORT );
    }

    // tell SFX to change viewshell when in preview mode
    if( IsPreview() || bStartPresentation )
    {
        SfxItemSet *pSet = GetMedium()->GetItemSet();
        if( pSet )
            pSet->Put( SfxUInt16Item( SID_VIEW_ID, bStartPresentation ? 1 : 5 ) );
    }

    return bRet;
}

/*************************************************************************
|*
|* LoadFrom: Inhalte fuer Organizer laden
|*
\************************************************************************/

BOOL DrawDocShell::LoadFrom( SfxMedium& rMedium )
{
    mbNewDocument = sal_False;

    WaitObject* pWait = NULL;
    if( pViewShell )
        pWait = new WaitObject( (Window*) pViewShell->GetActiveWindow() );

    BOOL bRet = FALSE;

        /*
        // #90691# return to old behaviour (before #80365#): construct own medium
        SfxMedium aMedium(xStorage);

        // #90691# for having a progress bar nonetheless for XML copy it
        // from the local DocShell medium (GetMedium()) to the constructed one
        SfxMedium* pLocalMedium = GetMedium();
        if(pLocalMedium)
        {
            SfxItemSet* pLocalItemSet = pLocalMedium->GetItemSet();
            SfxItemSet* pDestItemSet = aMedium.GetItemSet();

            if(pLocalItemSet && pDestItemSet)
            {
                const SfxUnoAnyItem* pItem = static_cast<
                    const SfxUnoAnyItem*>(
                        pLocalItemSet->GetItem(SID_PROGRESS_STATUSBAR_CONTROL));

                if(pItem)
                {
                    pDestItemSet->Put(*pItem);
                }
            }
        }                           */

        pDoc->NewOrLoadCompleted( NEW_DOC );
        pDoc->CreateFirstPages();
        pDoc->StopWorkStartupDelay();

        // TODO/LATER: nobody is interested in the error code?!
        ErrCode nError = ERRCODE_NONE;
        bRet = SdXMLFilter( rMedium, *this, sal_True, SDXMLMODE_Organizer, SotStorage::GetVersion( rMedium.GetStorage() ) ).Import( nError );


    // tell SFX to change viewshell when in preview mode
    if( IsPreview() )
    {
        SfxItemSet *pSet = GetMedium()->GetItemSet();

        if( pSet )
            pSet->Put( SfxUInt16Item( SID_VIEW_ID, 5 ) );
    }

    delete pWait;

    return bRet;
}

/*************************************************************************
|*
|* ConvertFrom: aus Fremdformat laden
|*
\************************************************************************/

BOOL DrawDocShell::ConvertFrom( SfxMedium& rMedium )
{
    mbNewDocument = sal_False;

    const String    aFilterName( rMedium.GetFilter()->GetFilterName() );
    BOOL            bRet = FALSE;

    SetWaitCursor( TRUE );

    if( aFilterName == pFilterPowerPoint97 || aFilterName == pFilterPowerPoint97Template)
    {
        pDoc->StopWorkStartupDelay();
        bRet = SdPPTFilter( rMedium, *this, sal_True ).Import();
    }
    else if (aFilterName.SearchAscii("impress8" )  != STRING_NOTFOUND ||
             aFilterName.SearchAscii("draw8")  != STRING_NOTFOUND )
    {
        // TODO/LATER: nobody is interested in the error code?!
        pDoc->CreateFirstPages();
        pDoc->StopWorkStartupDelay();
        ErrCode nError = ERRCODE_NONE;
        bRet = SdXMLFilter( rMedium, *this, sal_True ).Import( nError );

    }
    else if (aFilterName.SearchAscii("StarOffice XML (Draw)" )  != STRING_NOTFOUND || aFilterName.SearchAscii("StarOffice XML (Impress)")  != STRING_NOTFOUND )
    {
        // TODO/LATER: nobody is interested in the error code?!
        pDoc->CreateFirstPages();
        pDoc->StopWorkStartupDelay();
        ErrCode nError = ERRCODE_NONE;
        bRet = SdXMLFilter( rMedium, *this, sal_True, SDXMLMODE_Normal, SOFFICE_FILEFORMAT_60 ).Import( nError );
    }
    else if( aFilterName.EqualsAscii( "CGM - Computer Graphics Metafile" ) )
    {
        pDoc->CreateFirstPages();
        pDoc->StopWorkStartupDelay();
        bRet = SdCGMFilter( rMedium, *this, sal_True ).Import();
    }
    else
    {
        pDoc->CreateFirstPages();
        pDoc->StopWorkStartupDelay();
        bRet = SdGRFFilter( rMedium, *this, sal_True ).Import();
    }

    FinishedLoading( SFX_LOADED_MAINDOCUMENT | SFX_LOADED_IMAGES );

    // tell SFX to change viewshell when in preview mode
    if( IsPreview() )
    {
        SfxItemSet *pSet = GetMedium()->GetItemSet();

        if( pSet )
            pSet->Put( SfxUInt16Item( SID_VIEW_ID, 5 ) );
    }
    SetWaitCursor( FALSE );

    return bRet;
}

/*************************************************************************
|*
|* Save: Pools und Dokument in die offenen Streams schreiben
|*
\************************************************************************/

BOOL DrawDocShell::Save()
{
    pDoc->StopWorkStartupDelay();

    //TODO/LATER: why this?!
    if( GetCreateMode() == SFX_CREATE_MODE_STANDARD )
        SfxObjectShell::SetVisArea( Rectangle() );

    BOOL bRet = SfxObjectShell::Save();

    if( bRet )
    {
        // #86834# Call UpdateDocInfoForSave() before export
        UpdateDocInfoForSave();

        bRet = SdXMLFilter( *GetMedium(), *this, sal_True, SDXMLMODE_Normal, SotStorage::GetVersion( GetMedium()->GetStorage() ) ).Export();
    }

    return bRet;
}

/*************************************************************************
|*
|* SaveAs: Pools und Dokument in den angegebenen Storage sichern
|*
\************************************************************************/

BOOL DrawDocShell::SaveAs( SfxMedium& rMedium )
{
    pDoc->StopWorkStartupDelay();

    //TODO/LATER: why this?!
    if( GetCreateMode() == SFX_CREATE_MODE_STANDARD )
        SfxObjectShell::SetVisArea( Rectangle() );

    UINT32  nVBWarning = ERRCODE_NONE;
    BOOL    bRet = SfxObjectShell::SaveAs( rMedium );

    if( bRet )
    {
        // #86834# Call UpdateDocInfoForSave() before export
        UpdateDocInfoForSave();
        bRet = SdXMLFilter( rMedium, *this, sal_True, SDXMLMODE_Normal, SotStorage::GetVersion( rMedium.GetStorage() ) ).Export();
    }

    if( GetError() == ERRCODE_NONE )
        SetError( nVBWarning );

    return bRet;
}

/*************************************************************************
|*
|* ConvertTo: im Fremdformat speichern
|*
\************************************************************************/

BOOL DrawDocShell::ConvertTo( SfxMedium& rMedium )
{
    BOOL bRet = FALSE;

    if( pDoc->GetPageCount() )
    {
        const SfxFilter*    pMediumFilter = rMedium.GetFilter();
        const String        aTypeName( pMediumFilter->GetTypeName() );
        SdFilter*           pFilter = NULL;

        if( aTypeName.SearchAscii( "graphic_HTML" ) != STRING_NOTFOUND )
        {
            pFilter = new SdHTMLFilter( rMedium, *this, sal_True );
        }
        else if( aTypeName.SearchAscii( "MS_PowerPoint_97" ) != STRING_NOTFOUND )
        {
            pFilter = new SdPPTFilter( rMedium, *this, sal_True );
            ((SdPPTFilter*)pFilter)->PreSaveBasic();
        }
        else if ( aTypeName.SearchAscii( "CGM_Computer_Graphics_Metafile" ) != STRING_NOTFOUND )
        {
            pFilter = new SdCGMFilter( rMedium, *this, sal_True );
        }
        else if( ( aTypeName.SearchAscii( "draw8" ) != STRING_NOTFOUND ) ||
                 ( aTypeName.SearchAscii( "impress8" ) != STRING_NOTFOUND ) )
        {
            pFilter = new SdXMLFilter( rMedium, *this, sal_True );
            UpdateDocInfoForSave();
        }
        else if( ( aTypeName.SearchAscii( "StarOffice_XML_Impress" ) != STRING_NOTFOUND ) ||
                 ( aTypeName.SearchAscii( "StarOffice_XML_Draw" ) != STRING_NOTFOUND ) )
        {
            pFilter = new SdXMLFilter( rMedium, *this, sal_True, SDXMLMODE_Normal, SOFFICE_FILEFORMAT_60 );
            UpdateDocInfoForSave();
        }
        else
        {
            pFilter = new SdGRFFilter( rMedium, *this, sal_True );
        }

        if( pFilter )
        {
            const ULONG nOldSwapMode = pDoc->GetSwapGraphicsMode();

            pDoc->SetSwapGraphicsMode( SDR_SWAPGRAPHICSMODE_TEMP );

            if( !( bRet = pFilter->Export() ) )
                pDoc->SetSwapGraphicsMode( nOldSwapMode );

            delete pFilter;
        }
    }

    return  bRet;
}

/*************************************************************************
|*
|* SaveCompleted: die eigenen Streams wieder oeffnen, damit kein anderer
|*                                sie "besetzt"
|*
\************************************************************************/

BOOL DrawDocShell::SaveCompleted( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage )
{
    BOOL bRet = FALSE;

    if( SfxObjectShell::SaveCompleted(xStorage) )
    {
        pDoc->NbcSetChanged( FALSE );

        if( pViewShell )
        {
            if( pViewShell->ISA( OutlineViewShell ) )
                static_cast<OutlineView*>(pViewShell->GetView())
                    ->GetOutliner()->ClearModifyFlag();

            SdrOutliner* pOutl = pViewShell->GetView()->GetTextEditOutliner();
            if( pOutl )
            {
                SdrObject* pObj = pViewShell->GetView()->GetTextEditObject();
                if( pObj )
                    pObj->NbcSetOutlinerParaObject( pOutl->CreateParaObject() );

                pOutl->ClearModifyFlag();
            }
        }

        bRet = TRUE;

        SfxViewFrame* pFrame = ( pViewShell && pViewShell->GetViewFrame() ) ?
                               pViewShell->GetViewFrame() :
                               SfxViewFrame::Current();

        if( pFrame )
            pFrame->GetBindings().Invalidate( SID_NAVIGATOR_STATE, TRUE, FALSE );
    }
    return bRet;
}

/*************************************************************************
|*
|* Referenz auf Dokument
|*
\************************************************************************/

SdDrawDocument* DrawDocShell::GetDoc()
{
    return pDoc;
}

/*************************************************************************
|*
|* Referenz auf Dokument
|*
\************************************************************************/

SfxStyleSheetBasePool* DrawDocShell::GetStyleSheetPool()
{
    return( (SfxStyleSheetBasePool*) pDoc->GetStyleSheetPool() );
}

/*************************************************************************
|*
|* Sprung zu Bookmark
|*
\************************************************************************/

BOOL DrawDocShell::GotoBookmark(const String& rBookmark)
{
    OSL_TRACE("GotoBookmark %s",
        ::rtl::OUStringToOString(rBookmark, RTL_TEXTENCODING_UTF8).getStr());
    BOOL bFound = FALSE;

    if (pViewShell && pViewShell->ISA(DrawViewShell))
    {
        DrawViewShell* pDrViewSh = static_cast<DrawViewShell*>(pViewShell);

        String aBookmark( rBookmark );

        // Ist das Bookmark eine Seite?
        BOOL        bIsMasterPage;
        USHORT      nPgNum = pDoc->GetPageByName( aBookmark, bIsMasterPage );
        SdrObject*  pObj = NULL;

        if (nPgNum == SDRPAGE_NOTFOUND)
        {
            // Ist das Bookmark ein Objekt?
            pObj = pDoc->GetObj(aBookmark);

            if (pObj)
            {
                nPgNum = pObj->GetPage()->GetPageNum();
            }
        }

        if (nPgNum != SDRPAGE_NOTFOUND)
        {
            // Jump to the bookmarked page.  This is done in three steps.

            bFound = TRUE;
            SdPage* pPage;
            if (bIsMasterPage)
                pPage = (SdPage*) pDoc->GetMasterPage(nPgNum);
            else
                pPage = (SdPage*) pDoc->GetPage(nPgNum);

            // 1.) Change the view shell to the edit view, the notes view,
            // or the handout view.
            PageKind eNewPageKind = pPage->GetPageKind();

            if( (eNewPageKind != PK_STANDARD) && (pDoc->GetDocumentType() == DOCUMENT_TYPE_DRAW) )
                return FALSE;

            if (eNewPageKind != pDrViewSh->GetPageKind())
            {
                // Arbeitsbereich wechseln
                GetFrameView()->SetPageKind(eNewPageKind);
                if (pViewShell != NULL)
                {
                    ViewShell::ShellType eShellType(
                        pViewShell->GetShellType());
                    switch (eNewPageKind)
                    {
                        case PK_STANDARD:
                            eShellType = ViewShell::ST_IMPRESS;
                            break;
                        case PK_NOTES:
                            eShellType = ViewShell::ST_NOTES;
                            break;
                        case PK_HANDOUT:
                            eShellType = ViewShell::ST_HANDOUT;
                            break;
                        default:
                            break;
                    }
                    pViewShell->GetViewShellBase().GetPaneManager()
                        .RequestMainViewShellChange(
                            eShellType,
                            PaneManager::CM_SYNCHRONOUS);
                }
                // Die aktuelle ViewShell hat sich geaendert!
                pDrViewSh = static_cast<DrawViewShell*>(pViewShell);
            }

            // 2.) Set the edit mode to either the normal edit mode or the
            // master page mode.
            EditMode eNewEditMode = EM_PAGE;
            if( bIsMasterPage )
            {
                eNewEditMode = EM_MASTERPAGE;
            }

            if (eNewEditMode != pDrViewSh->GetEditMode())
            {
                // EditMode setzen
                pDrViewSh->ChangeEditMode(eNewEditMode, FALSE);
            }

            // 3.) Make the bookmarked page the current page.  This is done
            // by using the API because this takes care of all the little
            // things to be done.  Especially writing the view data to the
            // frame view (see bug #107803#).
            USHORT nSdPgNum = (nPgNum - 1) / 2;
            SdUnoDrawView* pUnoDrawView = static_cast<SdUnoDrawView*>(
                pDrViewSh->GetController());
            if (pUnoDrawView != NULL)
            {
                ::com::sun::star::uno::Reference<
                      ::com::sun::star::drawing::XDrawPage> xDrawPage (
                          pPage->getUnoPage(), ::com::sun::star::uno::UNO_QUERY);
                pUnoDrawView->setCurrentPage (xDrawPage);
            }
            else
            {
                // As a fall back switch to the page via the core.
                DBG_ASSERT (pUnoDrawView!=NULL,
                    "DrawDocShell::GotoBookmark: can't switch page via API");
                pDrViewSh->SwitchPage(nSdPgNum);
            }


            if (pObj)
            {
                // Objekt einblenden und selektieren
                pDrViewSh->MakeVisible(pObj->GetLogicRect(),
                                       *pDrViewSh->GetActiveWindow());
                pDrViewSh->GetView()->UnmarkAll();
                pDrViewSh->GetView()->MarkObj(pObj, pDrViewSh->GetView()->GetPageViewPvNum(0), FALSE);
            }
        }

        SfxBindings& rBindings = ( ( pViewShell && pViewShell->GetViewFrame() ) ?
                                 pViewShell->GetViewFrame() : SfxViewFrame::Current() )->GetBindings();

        rBindings.Invalidate(SID_NAVIGATOR_STATE, TRUE, FALSE);
        rBindings.Invalidate(SID_NAVIGATOR_PAGENAME);
    }

    return (bFound);
}

/*************************************************************************
|*
|* SaveAsOwnFormat: wenn es eine Dokumentvorlage werden soll,
|*
\************************************************************************/
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

BOOL DrawDocShell::SaveAsOwnFormat( SfxMedium& rMedium )
{

    const SfxFilter* pFilter = rMedium.GetFilter();

    if (pFilter->IsOwnTemplateFormat())
    {
        // jetzt die StarDraw-Spezialitaeten:
        // die Layoutvorlagen der ersten Seite werden mit dem jetzt
        // bekannten Layoutnamen versehen, die Layoutnamen der betroffenen
        // Masterpages und Seiten werden gesetzt;
        // alle Textobjekte der betroffenen Standard-, Notiz- und
        // Masterpages werden ueber die Namensaenderung informiert

        String aLayoutName;

        SfxStringItem* pLayoutItem;
        if( rMedium.GetItemSet()->GetItemState(SID_TEMPLATE_NAME, FALSE, (const SfxPoolItem**) & pLayoutItem ) == SFX_ITEM_SET )
        {
            aLayoutName = pLayoutItem->GetValue();
        }
        else
        {
            INetURLObject aURL( rMedium.GetName() );
            aURL.removeExtension();
            aLayoutName = aURL.getName();
        }

        if( aLayoutName.Len() )
        {
            String aOldPageLayoutName = pDoc->GetSdPage(0, PK_STANDARD)->GetLayoutName();
            pDoc->RenameLayoutTemplate(aOldPageLayoutName, aLayoutName);
        }
    }

    return SfxObjectShell::SaveAsOwnFormat(rMedium);
}

/*************************************************************************
|*
|* FillClass
|*
\************************************************************************/

void DrawDocShell::FillClass(SvGlobalName* pClassName,
                                        sal_uInt32*  pFormat,
                                        String* pAppName,
                                        String* pFullTypeName,
                                        String* pShortTypeName,
                                        sal_Int32 nFileFormat ) const
{
    if (nFileFormat == SOFFICE_FILEFORMAT_60)
    {
        if ( eDocType == DOCUMENT_TYPE_DRAW )
        {
                *pClassName = SvGlobalName(SO3_SDRAW_CLASSID_60);
                *pFormat = SOT_FORMATSTR_ID_STARDRAW_60;
                *pFullTypeName = String(SdResId(STR_GRAPHIC_DOCUMENT_FULLTYPE_60));
        }
        else
        {
                *pClassName = SvGlobalName(SO3_SIMPRESS_CLASSID_60);
                *pFormat = SOT_FORMATSTR_ID_STARIMPRESS_60;
                *pFullTypeName = String(SdResId(STR_IMPRESS_DOCUMENT_FULLTYPE_60));
        }
    }
    else if (nFileFormat == SOFFICE_FILEFORMAT_8)
    {
        if ( eDocType == DOCUMENT_TYPE_DRAW )
        {
                *pClassName = SvGlobalName(SO3_SDRAW_CLASSID_60);
                *pFormat = SOT_FORMATSTR_ID_STARDRAW_8;
                *pFullTypeName = String(RTL_CONSTASCII_USTRINGPARAM("Draw 8")); // HACK: method will be removed with new storage API
        }
        else
        {
                *pClassName = SvGlobalName(SO3_SIMPRESS_CLASSID_60);
                *pFormat = SOT_FORMATSTR_ID_STARIMPRESS_8;
                *pFullTypeName = String(RTL_CONSTASCII_USTRINGPARAM("Impress 8")); // HACK: method will be removed with new storage API
        }
    }

    *pShortTypeName = String(SdResId( (eDocType == DOCUMENT_TYPE_DRAW) ?
                                      STR_GRAPHIC_DOCUMENT : STR_IMPRESS_DOCUMENT ));
}

OutputDevice* DrawDocShell::GetDocumentRefDev (void)
{
    OutputDevice* pReferenceDevice = SfxObjectShell::GetDocumentRefDev ();
    // Only when our parent does not have a reference device then we return
    // our own.
    if (pReferenceDevice == NULL && pDoc != NULL)
        pReferenceDevice = pDoc->GetRefDevice ();
    return pReferenceDevice;
}

/** executes the SID_OPENDOC slot to let the framework open a document
    with the given URL and this document as a referer */
void DrawDocShell::OpenBookmark( const String& rBookmarkURL )
{
    SfxStringItem   aStrItem( SID_FILE_NAME, rBookmarkURL );
    SfxStringItem   aReferer( SID_REFERER, GetMedium()->GetName() );
    SfxBoolItem     aBrowseItem( SID_BROWSE, TRUE );

    ( pViewShell ? pViewShell->GetViewFrame() : SfxViewFrame::Current() )->GetDispatcher()->
        Execute(SID_OPENDOC, SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD, &aStrItem, &aBrowseItem, &aReferer, 0L );
}

} // end of namespace sd
