/*************************************************************************
 *
 *  $RCSfile: MasterPagesSelector.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-31 14:54:49 $
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

#include "MasterPagesSelector.hxx"

#include "MasterPageContainer.hxx"
#include "pres.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "sdpage.hxx"
#include "glob.hxx"
#include "glob.hrc"
#include "app.hrc"
#include "res_bmp.hrc"
#include "strings.hrc"
#include "showview.hxx"
#include "DrawViewShell.hxx"
#include "SlideSorterViewShell.hxx"
#include "PreviewValueSet.hxx"
#include "ViewShellBase.hxx"
#include "PaneManager.hxx"
#include "../TaskPaneShellManager.hxx"
#include "../TitledControl.hxx"
#include "../ControlContainer.hxx"
#include "controller/SlideSorterController.hxx"
#include "controller/SlsPageSelector.hxx"

#ifndef _SFXOBJFACE_HXX
#include <sfx2/objface.hxx>
#endif
#ifndef _SDRESID_HXX
#include "sdresid.hxx"
#endif
#ifndef _TEMPLATE_SCANNER_HXX
#include "TemplateScanner.hxx"
#endif
#ifndef _SD_DRAWVIEW_HXX
#include "drawview.hxx"
#endif
#ifndef _SV_IMAGE_HXX
#include <vcl/image.hxx>
#endif
#ifndef _SVTOOLS_LANGUAGEOPTIONS_HXX
#include <svtools/languageoptions.hxx>
#endif
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#include <sfx2/dispatch.hxx>
#include <sfx2/mnumgr.hxx>
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _SVX_DLGUTIL_HXX
#include <svx/dlgutil.hxx>
#endif
#ifndef _SVDPAGV_HXX
#include <svx/svdpagv.hxx>
#endif
#include <svx/svxids.hrc>
#include "FrameView.hxx"
#include "sdpage.hxx"
#include "stlpool.hxx"
#include "unmovss.hxx"
#include <sfx2/request.hxx>
#ifndef _SFXITEMPOOL_HXX //autogen
#include <svtools/itempool.hxx>
#endif

using namespace ::sd::toolpanel::controls;
#define MasterPagesSelector
#include "sdslots.hxx"

using namespace ::com::sun::star::text;

namespace {

typedef ::std::pair<int, ::sd::toolpanel::controls::MasterPageContainer::Token> UserData;

} //end of anonymous namespace



namespace sd { namespace toolpanel { namespace controls {

SFX_IMPL_INTERFACE(MasterPagesSelector, SfxShell,
    SdResId(STR_MASTERPAGESSELECTOR))
{
    SFX_POPUPMENU_REGISTRATION( SdResId(RID_TASKPANE_MASTERPAGESSELECTOR_POPUP) );
}

TYPEINIT1(MasterPagesSelector, SfxShell);


// The widths for the previews contain two pixels for the border that is
// painted arround the preview.
const int MasterPagesSelector::snSmallPreviewWidth = 72 + 2;
const int MasterPagesSelector::snLargePreviewWidth = 2*72 + 2;


MasterPagesSelector::MasterPagesSelector (
    TreeNode* pParent,
    SdDrawDocument& rDocument,
    ViewShellBase& rBase)
    : TreeNode (pParent),
      SfxShell(),
      mrDocument(rDocument),
      mpPageSet (new PreviewValueSet(pParent)),
      mnPreviewWidth (snSmallPreviewWidth),
      mrBase(rBase)
{
    mpPageSet->SetSelectHdl (LINK(this, MasterPagesSelector, ClickHandler));
    mpPageSet->SetRightMouseClickHandler (
        LINK(this, MasterPagesSelector, RightClickHandler));
    mpPageSet->SetPreviewWidth (mnPreviewWidth);

    SetPool (&rDocument.GetPool());

    mpPageSet->Show();
}




MasterPagesSelector::~MasterPagesSelector (void)
{
    MasterPageContainer::Instance().RemoveCallback (
    LINK(this,MasterPagesSelector,PreviewAvailableCallback));
    Clear();
    mpPageSet.reset();
    if (GetShellManager() != NULL)
        GetShellManager()->RemoveSubShell (this);
}




void MasterPagesSelector::LateInit (void)
{
    if (GetShellManager() != NULL)
        GetShellManager()->AddSubShell (this);
}




sal_Int32 MasterPagesSelector::GetPreferredWidth (sal_Int32 nHeight)
{
    return mpPageSet->GetPreferredWidth (nHeight);
}




sal_Int32 MasterPagesSelector::GetPreferredHeight (sal_Int32 nWidth)
{
    return mpPageSet->GetPreferredHeight (nWidth);
}




Size MasterPagesSelector::GetPreferredSize (void)
{
    int nPreferredWidth = GetPreferredWidth(
        mpPageSet->GetOutputSizePixel().Height());
    int nPreferredHeight = GetPreferredHeight(nPreferredWidth);
    return Size (nPreferredWidth, nPreferredHeight);

}




IMPL_LINK(MasterPagesSelector, ClickHandler, PreviewValueSet*, pValueSet)
{
    // We use the framework to assign the clicked-on master page because we
    // so use the same mechanism as the context menu does (where we do not
    // have the option to call the assignment method directly.)
    if (GetShellManager() != NULL)
        GetShellManager()->MoveToTop (this);
    GetDispatcher()->Execute (SID_TP_APPLY_TO_ALL_SLIDES);

    return 0;
}




IMPL_LINK(MasterPagesSelector, RightClickHandler, MouseEvent*, pEvent)
{
    mpPageSet->GrabFocus ();
    mpPageSet->ReleaseMouse();
    if (GetShellManager() != NULL)
        GetShellManager()->MoveToTop (this);
    if (GetDispatcher() != NULL)
    {
        USHORT nIndex = mpPageSet->GetItemId (pEvent->GetPosPixel());
        if (nIndex > 0)
        {
            mpPageSet->SelectItem (nIndex);
            GetDispatcher()->ExecutePopup(
                SdResId(RID_TASKPANE_MASTERPAGESSELECTOR_POPUP));
        }
    }
    return 0;
}




SdPage* MasterPagesSelector::GetSelectedMasterPage (void)
{
    SdPage* pMasterPage = NULL;
    //    USHORT nIndex = maPageSet.GetItemId (pEvent->GetPosPixel());
    USHORT nIndex = mpPageSet->GetSelectItemId();
    MasterPageContainer::Token* pToken
        = reinterpret_cast<MasterPageContainer::Token*>(
            mpPageSet->GetItemData (nIndex));
    if (pToken != NULL)
    {
        pMasterPage
            = MasterPageContainer::Instance().GetPageObjectForToken(*pToken);
        if (pMasterPage != NULL)
        {
            OSL_TRACE ("assigning master page %s",
                ::rtl::OUStringToOString(pMasterPage->GetName(),
                    RTL_TEXTENCODING_UTF8).getStr());

            // Just in case there is no proper preview in the value set copy
            // the preview from the MasterPageContainer to the ValueSet.
            mpPageSet->SetItemImage (nIndex,
                MasterPageContainer::Instance().GetPreviewForToken(
                    *pToken,
                    mnPreviewWidth));
        }
    }
    return pMasterPage;
}




void MasterPagesSelector::AssignMasterPageToAllSlides (SdPage* pMasterPage)
{
    do
    {
        if (pMasterPage == NULL)
            break;

        USHORT nPageCount = mrDocument.GetSdPageCount(PK_STANDARD);
        if (nPageCount == 0)
            break;

        // Get a list of selected pages.
        String sFullLayoutName (pMasterPage->GetLayoutName());
        ::std::vector<SdPage*> aPageList;
        for (USHORT nPageIndex=0; nPageIndex<nPageCount; nPageIndex++)
        {
            SdPage* pPage = mrDocument.GetSdPage (nPageIndex, PK_STANDARD);
            if (pPage != NULL
                && pPage->GetLayoutName().CompareTo(sFullLayoutName)!=0)
            {
                aPageList.push_back (pPage);
            }
        }

        AssignMasterPageToPageList (pMasterPage, aPageList);
    }
    while (false);
}




void MasterPagesSelector::AssignMasterPageToSelectedSlides (
    SdPage* pMasterPage)
{
    do
    {
        using namespace ::std;
        using namespace ::sd::slidesorter;
        using namespace ::sd::slidesorter::controller;

        if (pMasterPage == NULL)
            break;

        // Find a slide sorter to get the selection from.  When one is
        // displayed in the center pane use that.  Otherwise use the one in
        // the left pane.
        ViewShell* pSlideSorter = mrBase.GetPaneManager().GetViewShell (
            PaneManager::PT_CENTER);
        if (pSlideSorter->GetShellType() != ViewShell::ST_SLIDE_SORTER)
            pSlideSorter = mrBase.GetPaneManager().GetViewShell (
                PaneManager::PT_LEFT);
        if (pSlideSorter->GetShellType() != ViewShell::ST_SLIDE_SORTER)
            break;

        SlideSorterViewShell* pShell = static_cast<SlideSorterViewShell*>(
            pSlideSorter);
        PageSelector& rSelector (
            pShell->GetSlideSorterController().GetPageSelector());
        auto_ptr<PageSelector::PageSelection> pSelection (
            rSelector.GetPageSelection());
        {
            SlideSorterController::ModelChangeLock aLock (
                pShell->GetSlideSorterController());

            // Get a list of selected pages.
            vector<SdPage*> aSelectedPages;
            pShell->GetSelectedPages(aSelectedPages);
            if (aSelectedPages.size() == 0)
                break;

            AssignMasterPageToPageList (pMasterPage, aSelectedPages);
        }

        // Restore the previous selection.
        rSelector.SetPageSelection(*pSelection.get());

    }
    while (false);
}




void MasterPagesSelector::AssignMasterPageToPageList (
    SdPage* pMasterPage,
    const ::std::vector<SdPage*>& rPageList)
{
    do
    {
        if (pMasterPage == NULL)
            break;
        if (rPageList.size() == 0)
            break;

        // Make the layout name by stripping ouf the layout postfix from the
        // layout name of the given master page.
        String sFullLayoutName (pMasterPage->GetLayoutName());
        String sBaseLayoutName (sFullLayoutName);
        sBaseLayoutName.Erase (sBaseLayoutName.SearchAscii (SD_LT_SEPARATOR));

        // Test whether there are pages that are not already associated with
        // the master page.
        bool bAssignmentNecessary (false);
        ::std::vector<SdPage*>::const_iterator iPage;
        for (iPage=rPageList.begin();
             iPage!=rPageList.end();
             ++iPage)
        {
            if (*iPage != NULL
                && (*iPage)->GetLayoutName().CompareTo(sFullLayoutName)!=0)
            {
                bAssignmentNecessary = true;
                break;
            }
        }
        if ( ! bAssignmentNecessary)
            break;

        // Get notes master page.
        SdDrawDocument* pSourceDocument
            = static_cast<SdDrawDocument*>(pMasterPage->GetModel());
        SdPage* pNotesMasterPage = static_cast<SdPage*>(
            pSourceDocument->GetMasterPage (pMasterPage->GetPageNum()+1));
        if (pNotesMasterPage == NULL)
            break;

        // Determine the position where the new master pages are inserted.
        // By default they are inserted at the end.  When we assign to a
        // master page then insert after the last of the (selected) pages.
        USHORT nInsertionIndex = mrDocument.GetMasterPageCount();
        if (rPageList.front()->IsMasterPage())
        {
            nInsertionIndex = rPageList.back()->GetPageNum();
        }

        // Add copies of the master pages.
        SdPage* pClonedMasterPage = AddMasterPage (&mrDocument, pMasterPage, nInsertionIndex);
        AddMasterPage (&mrDocument, pNotesMasterPage, nInsertionIndex+1);

        // Assign the master pages to the selected pages.
        for (iPage=rPageList.begin();
             iPage!=rPageList.end();
             ++iPage)
        {
            AssignMasterPageToPage (
                pClonedMasterPage,
                sBaseLayoutName,
                *iPage);
        }
    }
    while (false);
}




/** In here we have to handle three cases:
    1. pPage is a normal slide.  We can use SetMasterPage to assign the
    master pages to it.
    2. pPage is a master page that is used by at least one slide.  We can
    assign the master page to these slides.
    3. pPage is a master page that is currently not used by any slide.
    We can delete that page and add copies of the given master pages
    instead.

    For points 2 and 3 where one master page A is assigned to another B we have
    to keep in mind that the master page that page A has already been
    inserted into the target document.
*/
void MasterPagesSelector::AssignMasterPageToPage (
    SdPage* pMasterPage,
    const String& rsBaseLayoutName,
    SdPage* pPage)
{
    // Leave early when the parameters are invalid.
    if (pPage == NULL || pMasterPage == NULL)
        return;

    if ( ! pPage->IsMasterPage())
    {
        // 1. Assign master pages to regular slide.
        mrDocument.SetMasterPage (
            (pPage->GetPageNum()-1)/2,
            rsBaseLayoutName,
            &mrDocument,
            FALSE,
            FALSE);
    }
    else
    {
        // Find first slide that uses the master page.
        SdPage* pSlide = NULL;
        USHORT nPageCount = mrDocument.GetSdPageCount(PK_STANDARD);
        for (USHORT nPage=0; nPage<nPageCount&&pSlide==NULL; nPage++)
        {
            SdrPage* pCandidate = mrDocument.GetSdPage(nPage,PK_STANDARD);
            if (pCandidate != NULL
                && pCandidate->TRG_HasMasterPage()
                && &(pCandidate->TRG_GetMasterPage()) == pPage)
            {
                pSlide = static_cast<SdPage*>(pCandidate);
            }
        }

        USHORT nIndex = pPage->GetPageNum();
        if (pSlide != NULL)
        {
            // 2. Assign the given master pages to the first slide that was
            // found above that uses the master page.
            mrDocument.SetMasterPage (
                (pSlide->GetPageNum()-1)/2,
                rsBaseLayoutName,
                &mrDocument,
                FALSE,
                FALSE);
        }
        else
        {
            // 3. Replace the master page A by a copy of the given master
            // page B.
            mrDocument.RemoveUnnessesaryMasterPages (
                pPage, FALSE);
        }
    }
}




SdPage* MasterPagesSelector::AddMasterPage (
    SdDrawDocument* pTargetDocument,
    SdPage* pMasterPage,
    USHORT nInsertionIndex)
{
    SdPage* pClonedMasterPage = NULL;

    if (pTargetDocument!=NULL && pMasterPage!=NULL)
    {
        // Duplicate the master page.
        pClonedMasterPage = static_cast<SdPage*>(pMasterPage->Clone());

        // Copy the necessary styles.
        SdDrawDocument* pSourceDocument
            = static_cast<SdDrawDocument*>(pMasterPage->GetModel());
        ProvideStyles (pSourceDocument, pTargetDocument, pClonedMasterPage);

        // Now that the styles are available we can insert the cloned master
        // page.
        pTargetDocument->InsertMasterPage (pClonedMasterPage, nInsertionIndex);

        // Adapt the size of the new master page to that of the pages in the
        // document.
        Size aNewSize (pTargetDocument->GetSdPage(0, PK_STANDARD)->GetSize());
        Rectangle aBorders (
            pClonedMasterPage->GetLftBorder(),
            pClonedMasterPage->GetUppBorder(),
            pClonedMasterPage->GetRgtBorder(),
            pClonedMasterPage->GetLwrBorder());
        pClonedMasterPage->ScaleObjects(aNewSize, aBorders, TRUE);
        pClonedMasterPage->SetSize(aNewSize);
        pClonedMasterPage->CreateTitleAndLayout(TRUE);
    }

    return pClonedMasterPage;
}




bool MasterPagesSelector::IsResizable (void)
{
    return false;
}




::Window* MasterPagesSelector::GetWindow (void)
{
    return mpPageSet.get();
}




sal_Int32 MasterPagesSelector::GetMinimumWidth (void)
{
    return mnPreviewWidth + 2*3;
}




void MasterPagesSelector::ProvideStyles (
    SdDrawDocument* pSourceDocument,
    SdDrawDocument* pTargetDocument,
    SdPage* pPage)
{
    // Get the layout name of the given page.
    String sLayoutName (pPage->GetLayoutName());
    sLayoutName.Erase (sLayoutName.SearchAscii (SD_LT_SEPARATOR));

    // Copy the style sheet from source to target document.
    SdStyleSheetPool* pSourceStyleSheetPool =
        static_cast<SdStyleSheetPool*>(pSourceDocument->GetStyleSheetPool());
    SdStyleSheetPool* pTargetStyleSheetPool =
        static_cast<SdStyleSheetPool*>(pTargetDocument->GetStyleSheetPool());
    List* pCreatedStyles = new List();
    pTargetStyleSheetPool->CopyLayoutSheets (
        sLayoutName,
        *pSourceStyleSheetPool,
        pCreatedStyles);

    // Add an undo action for the copied style sheets.
    if (pCreatedStyles->Count() > 0)
    {
         SfxUndoManager* pUndoManager
            = pTargetDocument->GetDocSh()->GetUndoManager();
       if (pUndoManager != NULL)
       {
           SdMoveStyleSheetsUndoAction* pMovStyles =
               new SdMoveStyleSheetsUndoAction (
                   pTargetDocument,
                   pCreatedStyles,
                   TRUE);
           pUndoManager->AddUndoAction (pMovStyles);
       }
    }
    else
    {
        delete pCreatedStyles;
    }
}





IMPL_LINK(MasterPagesSelector,PreviewAvailableCallback,void*,pUserData)
{
    UserData* pData = reinterpret_cast<UserData*>(pUserData);
    mpPageSet->SetItemImage (
        pData->first,
        MasterPageContainer::Instance().GetPreviewForToken (
            pData->second,
            mnPreviewWidth));
    delete pUserData;
    return 0;
}




void MasterPagesSelector::AddItemForPage (
    const String& sURL,
    const String& sPageName,
    SdPage* pMasterPage,
    Image aPreview,
    bool bCreatePreview)
{
    AddItemForToken (
        MasterPageContainer::Instance().PutMasterPage (
            sURL,
            sPageName,
            pMasterPage,
            aPreview));
}




void MasterPagesSelector::AddItemForToken (
    MasterPageContainer::Token aToken,
    bool bCreatePreview)
{
    if (aToken != MasterPageContainer::NIL_TOKEN)
    {
        int nIndex = mpPageSet->GetItemCount() + 1;
        Image aPreview;
        if (bCreatePreview)
            aPreview = MasterPageContainer::Instance().GetPreviewForToken(
                aToken,
                mnPreviewWidth,
                LINK(this,MasterPagesSelector,PreviewAvailableCallback),
                new UserData(nIndex, aToken));
        else
            aPreview = MasterPageContainer::Instance().GetPreviewForToken(
                aToken,
                mnPreviewWidth);
        mpPageSet->InsertItem (
            nIndex,
            aPreview,
            MasterPageContainer::Instance().GetPageNameForToken (aToken));
        mpPageSet->SetItemData (
            nIndex,
            new MasterPageContainer::Token(aToken));
    }
}




void MasterPagesSelector::FillPreviews (void)
{
    MasterPageContainer::Token* pToken;
    for (USHORT nIndex=1; nIndex<=mpPageSet->GetItemCount(); nIndex++)
    {
        pToken = reinterpret_cast<MasterPageContainer::Token*>(
            mpPageSet->GetItemData (nIndex));
        if (pToken != NULL)
            mpPageSet->SetItemImage (nIndex,
                MasterPageContainer::Instance().GetPreviewForToken (
                    *pToken,
                    mnPreviewWidth));
    }
    GetParentNode()->RequestResize();
}




void MasterPagesSelector::SetPreviewWidth (int nPreviewWidth)
{
    mpPageSet->SetPreviewWidth (nPreviewWidth);
    mnPreviewWidth = nPreviewWidth;
    FillPreviews();
}




void MasterPagesSelector::Execute (SfxRequest& rRequest)
{
    switch (rRequest.GetSlot())
    {
        case SID_TP_APPLY_TO_ALL_SLIDES:
            mrBase.SetBusyState (true);
            AssignMasterPageToAllSlides (GetSelectedMasterPage());
            mrBase.SetBusyState (false);
            break;

        case SID_TP_APPLY_TO_SELECTED_SLIDES:
            mrBase.SetBusyState (true);
            AssignMasterPageToSelectedSlides (GetSelectedMasterPage());
            mrBase.SetBusyState (false);
            break;

        case SID_TP_USE_FOR_NEW_PRESENTATIONS:
            DBG_ASSERT (false,
                "Using slides as default for new presentations"
                " is not yet implemented");
            break;

        case SID_TP_SHOW_SMALL_PREVIEW:
        case SID_TP_SHOW_LARGE_PREVIEW:
        {
            mrBase.SetBusyState (true);
            // The following iteration over all chidren of our father,
            // casting them to MasterPagesSelector and call
            // SetPreviewWidth() is a hack that may be replaced eventually
            // by making out parent SubToolPanel a special master page
            // control that is a shell and can process these slots itself.
            ControlContainer& rContainer (
                GetParentNode()->GetParentNode()->GetControlContainer());
            for (sal_uInt32 nIndex=0;
                 nIndex<rContainer.GetControlCount();
                 nIndex++)
            {
                TitledControl* pNode = static_cast<TitledControl*>(
                    rContainer.GetControl(nIndex));
                if (pNode == NULL)
                    continue;
                MasterPagesSelector* pSelector
                    = static_cast<MasterPagesSelector*>(pNode->GetControl());
                if (pSelector != NULL)
                {
                    pSelector->SetPreviewWidth (
                        rRequest.GetSlot()==SID_TP_SHOW_SMALL_PREVIEW
                        ? snSmallPreviewWidth
                        : snLargePreviewWidth);
                }
            }
            mrBase.SetBusyState (false);
            break;
        }
        break;

        case SID_CUT:
        case SID_COPY:
        case SID_PASTE:
            // Cut, copy, and paste are not supported and thus are ignored.
            break;
    }
}




void MasterPagesSelector::GetState (SfxItemSet& rItemSet)
{
    const int nCenter = (snLargePreviewWidth + snSmallPreviewWidth) / 2;
    if (mnPreviewWidth > nCenter)
        rItemSet.DisableItem (SID_TP_SHOW_LARGE_PREVIEW);
    else
        rItemSet.DisableItem (SID_TP_SHOW_SMALL_PREVIEW);

    // Cut and paste is not supported so do not show the menu entries.
    rItemSet.DisableItem (SID_CUT);
    rItemSet.DisableItem (SID_COPY);
    rItemSet.DisableItem (SID_PASTE);
}




void MasterPagesSelector::UpdateSelection (void)
{
}




void MasterPagesSelector::Fill (void)
{
}



void MasterPagesSelector::Clear (void)
{
    for (USHORT nIndex=1; nIndex<=mpPageSet->GetItemCount(); nIndex++)
    {
        delete reinterpret_cast<MasterPageContainer::Token*>(
            mpPageSet->GetItemData (nIndex));
    }
    mpPageSet->Clear ();
}

void MasterPagesSelector::SetSmartHelpId( const SmartId& aId, SmartIdUpdateMode aMode )
{
    mpPageSet->SetSmartHelpId( aId, aMode );
}

} } } // end of namespace ::sd::toolpanel::controls
