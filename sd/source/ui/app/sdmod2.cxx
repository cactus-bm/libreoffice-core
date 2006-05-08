/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdmod2.cxx,v $
 *
 *  $Revision: 1.46 $
 *
 *  last change: $Author: hr $ $Date: 2006-05-08 15:30:08 $
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

#ifndef _EEITEM_HXX //autogen
#include <svx/eeitem.hxx>
#endif

#define ITEMID_FIELD    EE_FEATURE_FIELD

#ifndef _SVX_FLDITEM_HXX //autogen
#include <svx/flditem.hxx>
#endif
#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _INETHIST_HXX //autogen
#include <svtools/inethist.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX //autogen
#include <svtools/poolitem.hxx>
#endif
#ifndef _SFXFLAGITEM_HXX //autogen
#include <svtools/flagitem.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif

#include <svx/svdfield.hxx>
#include <svx/editstat.hxx>
#include <svx/editeng.hxx>

#define ITEMID_SPELLCHECK   0
#include <svx/dialogs.hrc>

#ifndef _SVDOTEXT_HXX
#include <svx/svdotext.hxx>
#endif
#ifndef _SVDPAGV_HXX
#include <svx/svdpagv.hxx>
#endif
#ifndef _SVDOPAGE_HXX
#include <svx/svdopage.hxx>
#endif

#include <sfx2/sfxdlg.hxx>

#pragma hdrstop

#ifndef _SDR_CONTACT_DISPLAYINFO_HXX
#include <svx/sdr/contact/displayinfo.hxx>
#endif

#define _SD_DLL                 // fuer SD_MOD()
#include "sdmod.hxx"
#include "sddll.hxx"
#include "app.hrc"
#include "glob.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_FRAMW_VIEW_HXX
#include "FrameView.hxx"
#endif
#include "sdattr.hxx"
#include "tpoption.hrc"
#include "optsitem.hxx"
#ifndef SD_DRAW_DOC_SHELL_HXX
#include "DrawDocShell.hxx"
#endif
#include "drawdoc.hxx"
#ifndef SD_OUTLINER_HXX
#include "Outliner.hxx"
#endif
#include "sdresid.hxx"
#include "pres.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_OUTLINE_VIEW_SHELL_HXX
#include "OutlineViewShell.hxx"
#endif
#ifndef SD_OUTLINE_VIEW_HXX
#include "OutlineView.hxx"
#endif
#include "ViewShellBase.hxx"
#include "sdpage.hxx"
#include "sdxfer.hxx"
#include "sdabstdlg.hxx"
#include "tpoption.hrc"
#include "prntopts.hrc"
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif

/** retrieves the page that is currently painted. This will only be the master page
    if the current drawn view only shows the master page*/
static SdPage* GetCurrentPage( sd::ViewShell* pViewSh, EditFieldInfo* pInfo, bool& bMasterView )
{
    if( !pInfo )
        return 0;

    bMasterView = false;
    SdPage* pPage = dynamic_cast< SdPage* >( pInfo->GetSdrPage() );
    SdrOutliner* pOutliner = dynamic_cast< SdrOutliner* >( pInfo->GetOutliner() );

    // special case, someone already set the current page on the EditFieldInfo
    // This is used from the svx::UnoGraphicsExporter f.e.
    if( pPage )
    {
        bMasterView = false;
        return pPage;
    }

    // first try to check if we are inside the outline view
    sd::OutlineView* pSdView = NULL;
    if( pViewSh && pViewSh->ISA(sd::OutlineViewShell))
        pSdView = static_cast<sd::OutlineView*> (static_cast<sd::OutlineViewShell*>(pViewSh)->GetView());

    if (pSdView != NULL && (pOutliner ==  pSdView->GetOutliner()))
    {
        // outline mode
        int nPgNum = 0;
        Outliner* pOutl = pSdView->GetOutliner();
        long nPos = pInfo->GetPara();
        ULONG nParaPos = 0;

        for( Paragraph* pPara = pOutl->GetParagraph( 0 ); pPara && nPos >= 0; pPara = pOutl->GetParagraph( ++nParaPos ), nPos-- )
        {
            if( pOutl->GetDepth( (USHORT) nParaPos ) == 0 )
                nPgNum++;
        }

        pPage = pViewSh->GetDoc()->GetSdPage( nPgNum, PK_STANDARD );
    }
    else
    {
        // if not, get the current drawn page from the page view

        // draw mode, slide mode and preview
        const SdrPageView* pPV = NULL;

        // first, we try to geht the current page view from the SdrPaintInfoRec
        if( pOutliner && pOutliner->GetPaintInfoRec() )
        {
            const SdrPaintInfoRec* pRec = pOutliner->GetPaintInfoRec();

            if( pOutliner->GetTextObj() )
            {
                SdrPage* pPage = pOutliner->GetTextObj()->GetPage();
                bMasterView = ((pPage == NULL) || pPage->IsMasterPage()) && ((pRec->nPaintMode & SDRPAINTMODE_MASTERPAGE) == 0);
            }

            pPV = pRec->pPV;
        }

        // if this failed, we use the viewshell
        if( (pPV == 0) && pViewSh && (pViewSh->GetDoc()) )
            pPV = pViewSh->GetDoc()->GetPaintingPageView();

        if(pPV)
        {
            const SdrPage* pSdrPage = NULL;
            // get the current page from the current painting display info
            if( pPV->GetCurrentPaintingDisplayInfo() )
            {
                pSdrPage = pPV->GetCurrentPaintingDisplayInfo()->GetProcessedPage();
            }
            else
            {
                // if this is not available, get the current page from the page view
                pSdrPage = pPV->GetPage();
            }
            pPage = dynamic_cast< SdPage * >( const_cast< SdrPage* >( pSdrPage ) );

            // if all else failed, geht the current page from the object that is
            // currently formated from the document
            if( pPage == 0 )
            {
                const SdrTextObj* pTextObj = (pViewSh && pViewSh->GetDoc()) ? pViewSh->GetDoc()->GetFormattingTextObj() : NULL;

                if( pTextObj )
                    pPage = dynamic_cast< SdPage* >( pTextObj->GetPage() );
            }

            bMasterView = pPage && pPage->IsMasterPage();
        }
    }

    return pPage;
}

/*************************************************************************
|*
|* Link fuer CalcFieldValue des Outliners
|*
\************************************************************************/

IMPL_LINK(SdModule, CalcFieldValueHdl, EditFieldInfo*, pInfo)
{
    if (pInfo)
    {
        const SvxFieldData* pField = pInfo->GetField().GetField();
        ::sd::DrawDocShell*     pDocShell = NULL;
        SdDrawDocument* pDoc = 0;

        if( pInfo->GetOutliner() )
        {
            const SdrTextObj* pTextObj = static_cast< SdrOutliner* >( pInfo->GetOutliner() )->GetTextObj();

            if( pTextObj )
                pDoc = dynamic_cast< SdDrawDocument* >( pTextObj->GetModel() );

            if( pDoc )
                pDocShell = pDoc->GetDocSh();
        }

        if( !pDocShell )
            pDocShell = dynamic_cast< ::sd::DrawDocShell *>( SfxObjectShell::Current() );

        const SvxDateField* pDateField = 0;
        const SvxExtTimeField* pExtTimeField = 0;
        const SvxExtFileField* pExtFileField = 0;
        const SvxAuthorField* pAuthorField = 0;
        const SvxURLField* pURLField = 0;

        if( pDateField = dynamic_cast< const SvxDateField* >(pField) )
        {
            LanguageType eLang = pInfo->GetOutliner()->GetLanguage( pInfo->GetPara(), pInfo->GetPos() );
            pInfo->SetRepresentation( pDateField->GetFormatted( *GetNumberFormatter(), eLang ) );
        }
        else if( pExtTimeField = dynamic_cast< const SvxExtTimeField *>(pField) )
        {
            LanguageType eLang = pInfo->GetOutliner()->GetLanguage( pInfo->GetPara(), pInfo->GetPos() );
            pInfo->SetRepresentation( pExtTimeField->GetFormatted( *GetNumberFormatter(), eLang ) );
        }
        else if( pExtFileField = dynamic_cast< const SvxExtFileField * >(pField) )
        {
            if( pDocShell && (pExtFileField->GetType() != SVXFILETYPE_FIX) )
            {
                String aName;
                if( pDocShell->HasName() )
                    aName = pDocShell->GetMedium()->GetName();
                else
                    aName = pDocShell->GetName();

                const_cast< SvxExtFileField* >(pExtFileField)->SetFile( aName );
            }
            pInfo->SetRepresentation( pExtFileField->GetFormatted() );

        }
        else if( pAuthorField = dynamic_cast< const SvxAuthorField* >( pField )  )
        {
            if( pAuthorField->GetType() != SVXAUTHORTYPE_FIX )
            {
                SvtUserOptions aUserOptions;
                SvxAuthorField aAuthorField(
                        aUserOptions.GetFirstName(), aUserOptions.GetLastName(), aUserOptions.GetID(),
                        pAuthorField->GetType(), pAuthorField->GetFormat() );

                *(const_cast< SvxAuthorField* >(pAuthorField)) = aAuthorField;
            }
            pInfo->SetRepresentation( pAuthorField->GetFormatted() );

        }
        else if( dynamic_cast< const SvxPageField*  >(pField) )
        {
            String aRepresentation;
            aRepresentation += sal_Unicode( ' ' );

            ::sd::ViewShell* pViewSh = pDocShell ? pDocShell->GetViewShell() : NULL;
            if(pViewSh == NULL)
            {
                ::sd::ViewShellBase* pBase = PTR_CAST(::sd::ViewShellBase, SfxViewShell::Current());
                if(pBase)
                    pViewSh = pBase->GetMainViewShell();
            }
            if( !pDoc && pViewSh )
                pDoc = pViewSh->GetDoc();

            bool bMasterView;
            SdPage* pPage = GetCurrentPage( pViewSh, pInfo, bMasterView );

            if( pPage && pDoc && !bMasterView )
            {
                int nPgNum;

                if( (pPage->GetPageKind() == PK_HANDOUT) && pViewSh )
                {
                    nPgNum = pViewSh->GetPrintedHandoutPageNum();
                }
                else
                {
                    nPgNum = (pPage->GetPageNum() - 1) / 2 + 1;
                }
                aRepresentation = pDoc->CreatePageNumValue(nPgNum);
            }
            else
            {
                static String aNumberText( SdResId( STR_FIELD_PLACEHOLDER_NUMBER ) );
                aRepresentation = aNumberText;
            }

            pInfo->SetRepresentation( aRepresentation );
        }
        else if( pURLField = dynamic_cast< const SvxURLField* >(pField) )
        {
            switch ( pURLField->GetFormat() )
            {
                case SVXURLFORMAT_APPDEFAULT: //!!! einstellbar an App???
                case SVXURLFORMAT_REPR:
                    pInfo->SetRepresentation( pURLField->GetRepresentation() );
                    break;

                case SVXURLFORMAT_URL:
                    pInfo->SetRepresentation( pURLField->GetURL() );
                    break;
            }

            String aURL = pURLField->GetURL();

            svtools::ColorConfig aConfig;
            svtools::ColorConfigEntry eEntry =
                INetURLHistory::GetOrCreate()->QueryUrl( aURL ) ? svtools::LINKSVISITED : svtools::LINKS;
            pInfo->SetTxtColor( aConfig.GetColorValue(eEntry).nColor );
        }
        else if ( dynamic_cast< const SdrMeasureField* >(pField))
        {
            pInfo->ClearFldColor();
        }
        else
        {
            String aRepresentation;

            bool bHeaderField = dynamic_cast< const SvxHeaderField* >( pField ) != 0;
            bool bFooterField = !bHeaderField && (dynamic_cast< const SvxFooterField* >( pField ) != 0 );
            bool bDateTimeField = !bHeaderField && !bFooterField && (dynamic_cast< const SvxDateTimeField* >( pField ) != 0);

            if( bHeaderField || bFooterField || bDateTimeField )
            {
                sd::ViewShell* pViewSh = pDocShell ? pDocShell->GetViewShell() : NULL;
                bool bMasterView = false;
                SdPage* pPage = GetCurrentPage( pViewSh, pInfo, bMasterView );

                if( (pPage == NULL) || bMasterView )
                {
                    if( bHeaderField )
                    {
                        static String aHeaderStr( SdResId( STR_FIELD_PLACEHOLDER_HEADER ) );
                        aRepresentation = aHeaderStr;
                    }
                    else if (bFooterField )
                    {
                        static String aFooterStr( SdResId( STR_FIELD_PLACEHOLDER_FOOTER ) );
                        aRepresentation = aFooterStr;
                    }
                    else if (bDateTimeField )
                    {
                        static String aDateTimeStr( SdResId( STR_FIELD_PLACEHOLDER_DATETIME ) );
                        aRepresentation = aDateTimeStr;
                    }
                }
                else
                {
                    const sd::HeaderFooterSettings &rSettings = pPage->getHeaderFooterSettings();

                    if( bHeaderField )
                    {
                        aRepresentation = rSettings.maHeaderText;
                    }
                    else if( bFooterField )
                    {
                        aRepresentation = rSettings.maFooterText;
                    }
                    else if( bDateTimeField )
                    {
                        if( rSettings.mbDateTimeIsFixed )
                        {
                            aRepresentation = rSettings.maDateTimeText;
                        }
                        else
                        {
                            Date aDate;
                            Time aTime;
                            LanguageType eLang = pInfo->GetOutliner()->GetLanguage( pInfo->GetPara(), pInfo->GetPos() );
                            aRepresentation = SvxDateTimeField::GetFormatted( aDate, aTime, (SvxDateFormat)rSettings.meDateTimeFormat, *GetNumberFormatter(), eLang );
                        }
                    }
                }
            }
            else
            {
                DBG_ERROR("sd::SdModule::CalcFieldValueHdl(), unknown field type!");
            }

            if( aRepresentation.Len() == 0 )                // TODO: Edit engine doesn't handle empty fields?
                aRepresentation += sal_Unicode( ' ' );
            pInfo->SetRepresentation( aRepresentation );
        }
    }

    return(0);
}



/*************************************************************************
|*
|* virt. Methoden fuer Optionendialog
|*
\************************************************************************/
SfxItemSet*  SdModule::CreateItemSet( USHORT nSlot )
{
    ::sd::FrameView* pFrameView = NULL;
    ::sd::DrawDocShell* pDocSh = PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current() );
    SdDrawDocument* pDoc = NULL;

    // Hier wird der DocType vom Optionsdialog gesetzt (nicht Dokument!)
    DocumentType eDocType = DOCUMENT_TYPE_IMPRESS;
    if( nSlot == SID_SD_GRAPHIC_OPTIONS )
        eDocType = DOCUMENT_TYPE_DRAW;

    ::sd::ViewShell* pViewShell = NULL;

    if (pDocSh)
    {
        pDoc = pDocSh->GetDoc();

        // Wenn der Optionsdialog zum Dokumenttyp identisch ist,
        // kann auch die FrameView mit uebergeben werden:
        if( pDoc && eDocType == pDoc->GetDocumentType() )
            pFrameView = pDocSh->GetFrameView();

        pViewShell = pDocSh->GetViewShell();
        if (pViewShell != NULL)
            pViewShell->WriteFrameViewData();
    }

    SdOptions* pOptions = GetSdOptions(eDocType);

    // Pool hat standardmaessig MapUnit Twips (Baeh!)
    SfxItemPool& rPool = GetPool();
    rPool.SetDefaultMetric( SFX_MAPUNIT_100TH_MM );

    SfxItemSet*  pRet = new SfxItemSet( rPool,
                        SID_ATTR_METRIC, SID_ATTR_METRIC,
                        SID_ATTR_DEFTABSTOP, SID_ATTR_DEFTABSTOP,

                        ATTR_OPTIONS_LAYOUT, ATTR_OPTIONS_LAYOUT,
                        ATTR_OPTIONS_CONTENTS, ATTR_OPTIONS_CONTENTS,
                        ATTR_OPTIONS_MISC, ATTR_OPTIONS_MISC,

                        ATTR_OPTIONS_SNAP, ATTR_OPTIONS_SNAP,

                        ATTR_OPTIONS_SCALE_START, ATTR_OPTIONS_SCALE_END,

                        ATTR_OPTIONS_PRINT, ATTR_OPTIONS_PRINT,

                        SID_ATTR_GRID_OPTIONS, SID_ATTR_GRID_OPTIONS,
                        0 );

    // TP_OPTIONS_LAYOUT:
    pRet->Put( SdOptionsLayoutItem( ATTR_OPTIONS_LAYOUT, pOptions, pFrameView ) );

    UINT16 nDefTab = 0;
    if( pFrameView)
        nDefTab = pDoc->GetDefaultTabulator();
    else
        nDefTab = pOptions->GetDefTab();
    pRet->Put( SfxUInt16Item( SID_ATTR_DEFTABSTOP, nDefTab ) );

    UINT16 nMetric = 0xffff;
    if( pFrameView)
        nMetric = pDoc->GetUIUnit();
    else
        nMetric = pOptions->GetMetric();

    if( nMetric == 0xffff )
        nMetric = GetModuleFieldUnit();

    pRet->Put( SfxUInt16Item( SID_ATTR_METRIC, nMetric ) );

    // TP_OPTIONS_CONTENTS:
    pRet->Put( SdOptionsContentsItem( ATTR_OPTIONS_CONTENTS, pOptions, pFrameView ) );

    // TP_OPTIONS_MISC:
    SdOptionsMiscItem aSdOptionsMiscItem( ATTR_OPTIONS_MISC, pOptions, pFrameView );
    if ( pFrameView )
    {
        aSdOptionsMiscItem.SetSummationOfParagraphs( pDoc->IsSummationOfParagraphs() );
        aSdOptionsMiscItem.SetPrinterIndependentLayout (
            (USHORT)pDoc->GetPrinterIndependentLayout());
    }
    pRet->Put( aSdOptionsMiscItem );


    // TP_OPTIONS_SNAP:
    pRet->Put( SdOptionsSnapItem( ATTR_OPTIONS_SNAP, pOptions, pFrameView ) );

    // TP_SCALE:
    UINT32 nW = 10L;
    UINT32 nH = 10L;
    INT32  nX;
    INT32  nY;
    if( pDocSh )
    {
        SdrPage* pPage = (SdrPage*) pDoc->GetSdPage(0, PK_STANDARD);
        Size aSize(pPage->GetSize());
        nW = aSize.Width();
        nH = aSize.Height();
    }

    if(pFrameView)
    {
        const Fraction& rFraction =  pDoc->GetUIScale();
        nX=rFraction.GetNumerator();
        nY=rFraction.GetDenominator();
    }
    else
    {
        // Optionen aus Configdatei holen
        pOptions->GetScale( nX, nY );
    }

    pRet->Put( SfxInt32Item( ATTR_OPTIONS_SCALE_X, nX ) );
    pRet->Put( SfxInt32Item( ATTR_OPTIONS_SCALE_Y, nY ) );
    pRet->Put( SfxUInt32Item( ATTR_OPTIONS_SCALE_WIDTH, nW ) );
    pRet->Put( SfxUInt32Item( ATTR_OPTIONS_SCALE_HEIGHT, nH ) );


    // TP_OPTIONS_PRINT:
    pRet->Put( SdOptionsPrintItem( ATTR_OPTIONS_PRINT, pOptions ) );

    // RID_SVXPAGE_GRID:
    pRet->Put( SdOptionsGridItem( SID_ATTR_GRID_OPTIONS, pOptions ) );

    return pRet;
}
void SdModule::ApplyItemSet( USHORT nSlot, const SfxItemSet& rSet )
{
    const SfxPoolItem*  pItem = NULL;
    BOOL bNewDefTab = FALSE;
    BOOL bNewPrintOptions = FALSE;
    BOOL bMiscOptions = FALSE;

    ::sd::FrameView* pFrameView = NULL;
    ::sd::DrawDocShell* pDocSh = PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current() );
    SdDrawDocument* pDoc = NULL;
    // Hier wird der DocType vom Optionsdialog gesetzt (nicht Dokument!)
    DocumentType eDocType = DOCUMENT_TYPE_IMPRESS;
    if( nSlot == SID_SD_GRAPHIC_OPTIONS )
        eDocType = DOCUMENT_TYPE_DRAW;

    ::sd::ViewShell* pViewShell = NULL;

    if (pDocSh)
    {
        pDoc = pDocSh->GetDoc();

        // Wenn der Optionsdialog zum Dokumenttyp identisch ist,
        // kann auch die FrameView mit uebergeben werden:
        if( pDoc && eDocType == pDoc->GetDocumentType() )
            pFrameView = pDocSh->GetFrameView();

        pViewShell = pDocSh->GetViewShell();
        if (pViewShell != NULL)
            pViewShell->WriteFrameViewData();
    }
    SdOptions* pOptions = GetSdOptions(eDocType);
    // Raster
    if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_GRID_OPTIONS ,
                            FALSE, (const SfxPoolItem**) &pItem ))
    {
        const SdOptionsGridItem* pGridItem = (SdOptionsGridItem*) pItem;
        pGridItem->SetOptions( pOptions );
    }

    // Layout
    const SdOptionsLayoutItem* pLayoutItem = NULL;
    if( SFX_ITEM_SET == rSet.GetItemState( ATTR_OPTIONS_LAYOUT,
                            FALSE, (const SfxPoolItem**) &pLayoutItem ))
    {
        pLayoutItem->SetOptions( pOptions );
    }

    // Metric
    if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_METRIC, FALSE, &pItem ) )
    {
        if( pDoc && eDocType == pDoc->GetDocumentType() )
            PutItem( *pItem );
        pOptions->SetMetric( ( (SfxUInt16Item*) pItem )->GetValue() );
    }
    UINT16 nDefTab = pOptions->GetDefTab();
    // Default-Tabulator
    if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_DEFTABSTOP, FALSE, &pItem ) )
    {
        nDefTab = ( (SfxUInt16Item*) pItem )->GetValue();
        pOptions->SetDefTab( nDefTab );

        bNewDefTab = TRUE;
    }

    // Massstab
    if( SFX_ITEM_SET == rSet.GetItemState( ATTR_OPTIONS_SCALE_X, FALSE, &pItem ) )
    {
        INT32 nX = ( (SfxInt32Item*) pItem )->GetValue();
        if( SFX_ITEM_SET == rSet.GetItemState( ATTR_OPTIONS_SCALE_Y, FALSE, &pItem ) )
        {
            INT32 nY = ( (SfxInt32Item*) pItem )->GetValue();
            pOptions->SetScale( nX, nY );

            // #92067# Apply to document only if doc type match
            if( pDocSh && pDoc && eDocType == pDoc->GetDocumentType() )
            {
                pDoc->SetUIScale( Fraction( nX, nY ) );
                if( pViewShell )
                    pViewShell->SetRuler( pViewShell->HasRuler() );
            }
        }
    }

    // Contents (Inhalte)
    const SdOptionsContentsItem* pContentsItem = NULL;
    if( SFX_ITEM_SET == rSet.GetItemState( ATTR_OPTIONS_CONTENTS,
                            FALSE, (const SfxPoolItem**) &pContentsItem ))
    {
        pContentsItem->SetOptions( pOptions );
    }

    // Misc (Sonstiges)
    const SdOptionsMiscItem* pMiscItem = NULL;
    if( SFX_ITEM_SET == rSet.GetItemState( ATTR_OPTIONS_MISC,
                            FALSE, (const SfxPoolItem**) &pMiscItem ))
    {
        pMiscItem->SetOptions( pOptions );
        bMiscOptions = TRUE;
    }

    // Fangen/Einrasten
    const SdOptionsSnapItem* pSnapItem = NULL;
    if( SFX_ITEM_SET == rSet.GetItemState( ATTR_OPTIONS_SNAP,
                            FALSE, (const SfxPoolItem**) &pSnapItem ))
    {
        pSnapItem->SetOptions( pOptions );
    }

    SfxItemSet aPrintSet( GetPool(),
                    SID_PRINTER_NOTFOUND_WARN,  SID_PRINTER_NOTFOUND_WARN,
                    SID_PRINTER_CHANGESTODOC,   SID_PRINTER_CHANGESTODOC,
                    ATTR_OPTIONS_PRINT,         ATTR_OPTIONS_PRINT,
                    0 );

    // Drucken
    const SdOptionsPrintItem* pPrintItem = NULL;
    if( SFX_ITEM_SET == rSet.GetItemState( ATTR_OPTIONS_PRINT,
                            FALSE, (const SfxPoolItem**) &pPrintItem ))
    {
        pPrintItem->SetOptions( pOptions );

        // PrintOptionsSet setzen
        SdOptionsPrintItem aPrintItem( ATTR_OPTIONS_PRINT, pOptions );
        SfxFlagItem aFlagItem( SID_PRINTER_CHANGESTODOC );
        USHORT      nFlags = 0;

        nFlags =  (aPrintItem.IsWarningSize() ? SFX_PRINTER_CHG_SIZE : 0) |
                (aPrintItem.IsWarningOrientation() ? SFX_PRINTER_CHG_ORIENTATION : 0);
        aFlagItem.SetValue( nFlags );

        aPrintSet.Put( aPrintItem );
        aPrintSet.Put( SfxBoolItem( SID_PRINTER_NOTFOUND_WARN, aPrintItem.IsWarningPrinter() ) );
        aPrintSet.Put( aFlagItem );

        bNewPrintOptions = TRUE;
    }

    // Nur, wenn auch der Dokumenttyp uebereinstimmt...
    if( pDocSh && pDoc && eDocType == pDoc->GetDocumentType() )
    {
        if( bNewPrintOptions )
        {
            pDocSh->GetPrinter(TRUE)->SetOptions( aPrintSet );
        }

        // Am Model den DefTab setzen
        if( bNewDefTab )
        {
            SdDrawDocument* pDocument = pDocSh->GetDoc();
            pDocument->SetDefaultTabulator( nDefTab );

            ::sd::Outliner* pOutl = pDocument->GetOutliner( FALSE );
            if( pOutl )
                pOutl->SetDefTab( nDefTab );

            ::sd::Outliner* pInternalOutl = pDocument->GetInternalOutliner( FALSE );
            if( pInternalOutl )
                pInternalOutl->SetDefTab( nDefTab );
        }
        if ( bMiscOptions )
        {
            pDoc->SetSummationOfParagraphs( pMiscItem->IsSummationOfParagraphs() );
            sal_uInt32 nSum = pMiscItem->IsSummationOfParagraphs() ? EE_CNTRL_ULSPACESUMMATION : 0;
            sal_uInt32 nCntrl;

            SdDrawDocument* pDocument = pDocSh->GetDoc();
            SdrOutliner& rOutl = pDocument->GetDrawOutliner( FALSE );
            nCntrl = rOutl.GetControlWord() &~ EE_CNTRL_ULSPACESUMMATION;
            rOutl.SetControlWord( nCntrl | nSum );
            ::sd::Outliner* pOutl = pDocument->GetOutliner( FALSE );
            if( pOutl )
            {
                nCntrl = pOutl->GetControlWord() &~ EE_CNTRL_ULSPACESUMMATION;
                pOutl->SetControlWord( nCntrl | nSum );
            }
            pOutl = pDocument->GetInternalOutliner( FALSE );
            if( pOutl )
            {
                nCntrl = pOutl->GetControlWord() &~ EE_CNTRL_ULSPACESUMMATION;
                pOutl->SetControlWord( nCntrl | nSum );
            }

            // Set printer independent layout mode.
            if( pDoc->GetPrinterIndependentLayout() != pMiscItem->GetPrinterIndependentLayout() )
                pDoc->SetPrinterIndependentLayout (pMiscItem->GetPrinterIndependentLayout());
        }
    }

    pOptions->StoreConfig();

    // Nur, wenn auch der Dokumenttyp uebereinstimmt...
    if( pDocSh && pDoc && eDocType == pDoc->GetDocumentType() )
    {
        FieldUnit eUIUnit = (FieldUnit) pOptions->GetMetric();
        pDoc->SetUIUnit(eUIUnit);

        if (pViewShell)
        {
            // #74495# make sure no one is in text edit mode, cause there
            // are some pointers remembered else (!)
            if(pViewShell->GetView())
                pViewShell->GetView()->EndTextEdit();

            ::sd::FrameView* pFrameView = pViewShell->GetFrameView();
            pFrameView->Update(pOptions);
            pViewShell->ReadFrameViewData(pFrameView);
            pViewShell->SetUIUnit(eUIUnit);
            pViewShell->SetDefTabHRuler( nDefTab );
        }
    }

    if( pViewShell && pViewShell->GetViewFrame() )
        pViewShell->GetViewFrame()->GetBindings().InvalidateAll( TRUE );
}

SfxTabPage* SdModule::CreateTabPage( USHORT nId, Window* pParent, const SfxItemSet& rSet )
{
    SfxTabPage* pRet = NULL;
    SfxAllItemSet aSet(*(rSet.GetPool())); //CHINA001
    SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
    DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
    switch(nId)
    {
        case SID_SD_TP_CONTENTS:
        case SID_SI_TP_CONTENTS:
        { //add by CHINA001
            //CHINA001 pRet = SdTpOptionsContents::Create(pParent, rSet);
            ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( TP_OPTIONS_CONTENTS );
            DBG_ASSERT(fnCreatePage, "SdAbstractDialogFactory fail!");//CHINA001
            pRet = (*fnCreatePage)( pParent, rSet );
        }
        break;
        case SID_SD_TP_SNAP:
        case SID_SI_TP_SNAP:
        { //add by CHINA001
            //CHINA001 pRet = SdTpOptionsSnap::Create(pParent, rSet);
            ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( TP_OPTIONS_SNAP );
            DBG_ASSERT(fnCreatePage, "SdAbstractDialogFactory fail!");//CHINA001
            pRet = (*fnCreatePage)( pParent, rSet );
        }
        break;
        case SID_SD_TP_PRINT:
        case SID_SI_TP_PRINT:
        { //add by CHINA001
            //CHINA001 pRet = SdPrintOptions::Create(pParent, rSet);
            ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( TP_PRINT_OPTIONS );
            DBG_ASSERT(fnCreatePage, "SdAbstractDialogFactory fail!");//CHINA001
            pRet = (*fnCreatePage)( pParent, rSet );
            if(SID_SD_TP_PRINT == nId)
                //CHINA001 ( (SdPrintOptions*) pRet )->SetDrawMode();
                aSet.Put (SfxUInt32Item(SID_SDMODE_FLAG,SD_DRAW_MODE));
            pRet->PageCreated(aSet);
        }
        break;
        case SID_SI_TP_MISC:
        case SID_SD_TP_MISC:
        { //add by CHINA001
            //CHINA001 pRet = SdTpOptionsMisc::Create(pParent, rSet);
            ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( TP_OPTIONS_MISC );
            DBG_ASSERT(fnCreatePage, "SdAbstractDialogFactory fail!");//CHINA001
            pRet = (*fnCreatePage)( pParent, rSet );
            if(SID_SD_TP_MISC == nId)
                //CHINA001 ( (SdTpOptionsMisc*) pRet )->SetDrawMode();
                aSet.Put (SfxUInt32Item(SID_SDMODE_FLAG,SD_DRAW_MODE));
            else
                //CHINA001 ( (SdTpOptionsMisc*) pRet )->SetImpressMode();
                aSet.Put (SfxUInt32Item(SID_SDMODE_FLAG,SD_IMPRESS_MODE));
            pRet->PageCreated(aSet);
        }
        break;
        case RID_OFA_TP_INTERNATIONAL_SD:
        case RID_OFA_TP_INTERNATIONAL_IMPR:
        case RID_SVXPAGE_TEXTANIMATION :
        {
            SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();
            if ( pFact )
            {
                ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( nId );
                if ( fnCreatePage )
                    pRet = (*fnCreatePage)( pParent, rSet );
            }
        }
        break;
    }

    DBG_ASSERT( pRet, "SdModule::CreateTabPage(): no valid ID for TabPage!" );

    return pRet;
}
