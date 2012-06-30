/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright 2012 LibreOffice contributors.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "templatedlg.hxx"

#include <comphelper/processfactory.hxx>
#include <sfx2/filedlghelper.hxx>
#include <sfx2/sfxresid.hxx>
#include <sfx2/templatefolderview.hxx>
#include <sfx2/templatefolderviewitem.hxx>
#include <sfx2/templateviewitem.hxx>
#include <sfx2/thumbnailviewitem.hxx>
#include <tools/urlobj.hxx>
#include <unotools/moduleoptions.hxx>
#include <vcl/edit.hxx>
#include <vcl/toolbox.hxx>

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>

#include "doc.hrc"
#include "templatedlg.hrc"

#define ITEM_MAX_WIDTH 192
#define ITEM_MAX_HEIGHT 192
#define ITEM_PADDING 5
#define ITEM_SPACE 20
#define ITEM_MAX_TEXT_LENGTH 20
#define THUMBNAIL_MAX_HEIGHT 128

#define INIT_FOLDER_COLS 3
#define INIT_FOLDER_LINES 2

#define PADDING_TOOLBAR_VIEW    15
#define PADDING_DLG_BORDER      10

using namespace ::com::sun::star;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::frame;

void lcl_createTemplate(uno::Reference<XComponentLoader> xDesktop, const FILTER_APPLICATION eApp);

// Sort by name in ascending order
class SortView_Name
{
public:

    bool operator() (const ThumbnailViewItem *pItem1, const ThumbnailViewItem *pItem2)
    {
        return (pItem1->maText.compareTo(pItem2->maText) < 0);
    }
};

SfxTemplateManagerDlg::SfxTemplateManagerDlg (Window *parent)
    : ModalDialog(parent, SfxResId(DLG_TEMPLATE_MANAGER)),
      aButtonAll(this,SfxResId(BTN_SELECT_ALL)),
      aButtonDocs(this,SfxResId(BTN_SELECT_DOCS)),
      aButtonPresents(this,SfxResId(BTN_SELECT_PRESENTATIONS)),
      aButtonSheets(this,SfxResId(BTN_SELECT_SHEETS)),
      aButtonDraws(this,SfxResId(BTN_SELECT_DRAWS)),
      maButtonClose(this,SfxResId(BTN_TEMPLATE_CLOSE)),
      maButtonSelMode(this,SfxResId(BTN_SELECTION_MODE)),
      mpSearchEdit(new Edit(this,WB_HIDE | WB_BORDER)),
      mpViewBar( new ToolBox(this, SfxResId(TBX_ACTION_VIEW))),
      mpActionBar( new ToolBox(this, SfxResId(TBX_ACTION_ACTION))),
      mpTemplateBar( new ToolBox(this, SfxResId(TBX_ACTION_TEMPLATES))),
      maView(new TemplateFolderView(this,SfxResId(TEMPLATE_VIEW))),
      mnSelectionCount(0),
      mxDesktop(comphelper::getProcessServiceFactory()->createInstance( "com.sun.star.frame.Desktop" ),uno::UNO_QUERY )
{
    maButtonSelMode.SetStyle(maButtonSelMode.GetStyle() | WB_TOGGLE);

    // Create popup menus
    mpCreateMenu = new PopupMenu;
    mpCreateMenu->InsertItem(MNI_CREATE_TEXT,SfxResId(STR_CREATE_TEXT).toString());
    mpCreateMenu->InsertItem(MNI_CREATE_SHEET,SfxResId(STR_CREATE_SHEET).toString());
    mpCreateMenu->InsertItem(MNI_CREATE_PRESENT,SfxResId(STR_CREATE_PRESENT).toString());
    mpCreateMenu->InsertItem(MNI_CREATE_DRAW,SfxResId(STR_CREATE_DRAW).toString());
    mpCreateMenu->SetSelectHdl(LINK(this, SfxTemplateManagerDlg, MenuSelectHdl));

    mpActionMenu = new PopupMenu;
    mpActionMenu->InsertItem(MNI_ACTION_SORT_NAME,SfxResId(STR_ACTION_SORT_NAME).toString());
    mpActionMenu->SetSelectHdl(LINK(this,SfxTemplateManagerDlg,MenuSelectHdl));

    mpMoveMenu = new PopupMenu;
    mpMoveMenu->InsertItem(MNI_MOVE_NEW,SfxResId(STR_MOVE_NEW).toString());
    mpMoveMenu->SetSelectHdl(LINK(this,SfxTemplateManagerDlg,MoveMenuSelectHdl));

    Size aWinSize = GetOutputSize();

    // Calculate thumbnail view minimum size
    Size aThumbSize = maView->CalcWindowSizePixel(INIT_FOLDER_COLS,INIT_FOLDER_LINES,
                                                  ITEM_MAX_WIDTH,ITEM_MAX_HEIGHT,ITEM_SPACE);

    if (aWinSize.getWidth() < aThumbSize.getWidth() + 2*PADDING_DLG_BORDER)
        aWinSize.setWidth(aThumbSize.getWidth() + 2*PADDING_DLG_BORDER);

    // Calculate toolboxs size and positions
    Size aViewSize = mpViewBar->CalcMinimumWindowSizePixel();
    Size aActionSize = mpActionBar->CalcMinimumWindowSizePixel();
    Size aTemplateSize = mpTemplateBar->CalcMinimumWindowSizePixel();

    aActionSize.setWidth(2*aActionSize.getWidth());
    aViewSize.setWidth(aWinSize.getWidth()-aActionSize.getWidth()-mpViewBar->GetPosPixel().X());
    aTemplateSize.setWidth(aWinSize.getWidth());

    Point aActionPos = mpActionBar->GetPosPixel();
    aActionPos.setX(aWinSize.getWidth() - aActionSize.getWidth());

    mpViewBar->SetSizePixel(aViewSize);
    mpActionBar->SetPosSizePixel(aActionPos,aActionSize);
    mpTemplateBar->SetSizePixel(aTemplateSize);

    // Set toolbox styles
    mpViewBar->SetButtonType(BUTTON_SYMBOLTEXT);
    mpTemplateBar->SetButtonType(BUTTON_SYMBOLTEXT);

    // Set toolbox button bits
    mpViewBar->SetItemBits(TBI_TEMPLATE_CREATE, TIB_DROPDOWNONLY);
    mpActionBar->SetItemBits(TBI_TEMPLATE_ACTION, TIB_DROPDOWNONLY);
    mpTemplateBar->SetItemBits(TBI_TEMPLATE_MOVE,TIB_DROPDOWNONLY);

    // Set toolbox handlers
    mpViewBar->SetClickHdl(LINK(this,SfxTemplateManagerDlg,TBXViewHdl));
    mpViewBar->SetDropdownClickHdl(LINK(this,SfxTemplateManagerDlg,TBXDropdownHdl));
    mpActionBar->SetClickHdl(LINK(this,SfxTemplateManagerDlg,TBXActionHdl));
    mpActionBar->SetDropdownClickHdl(LINK(this,SfxTemplateManagerDlg,TBXDropdownHdl));
    mpTemplateBar->SetClickHdl(LINK(this,SfxTemplateManagerDlg,TBXTemplateHdl));
    mpTemplateBar->SetDropdownClickHdl(LINK(this,SfxTemplateManagerDlg,TBXDropdownHdl));

    // Set view position below toolbox
    Point aViewPos = maView->GetPosPixel();
    aViewPos.setY(aActionPos.Y() + aActionSize.getHeight() + PADDING_TOOLBAR_VIEW);
    aViewPos.setX((aWinSize.getWidth() - aThumbSize.getWidth())/2);     // Center the view
    maView->SetPosPixel(aViewPos);

    if (aWinSize.getHeight() < aViewPos.getY() + aThumbSize.getHeight() + PADDING_DLG_BORDER)
        aWinSize.setHeight(aViewPos.getY() + aThumbSize.getHeight() + PADDING_DLG_BORDER);

    // Set search box position and size
    Size aSearchSize = mpSearchEdit->CalcMinimumSize();
    aSearchSize.setWidth(aWinSize.getWidth() - 2*PADDING_DLG_BORDER);

    mpSearchEdit->SetSizePixel(aSearchSize);
    mpSearchEdit->SetPosPixel(Point(PADDING_DLG_BORDER,aActionPos.Y()+aActionSize.getHeight()));

    maView->SetStyle(WB_TABSTOP);
    maView->SetColor(GetBackground().GetColor());
    maView->SetSizePixel(aThumbSize);
    maView->setItemMaxTextLength(ITEM_MAX_TEXT_LENGTH);

    maView->setItemDimensions(ITEM_MAX_WIDTH,THUMBNAIL_MAX_HEIGHT,
                              ITEM_MAX_HEIGHT-THUMBNAIL_MAX_HEIGHT,
                              ITEM_PADDING);

    maView->setItemStateHdl(LINK(this,SfxTemplateManagerDlg,TVFolderStateHdl));
    maView->setTemplateStateHdl(LINK(this,SfxTemplateManagerDlg,TVTemplateStateHdl));
    maView->setOverlayDblClickHdl(LINK(this,SfxTemplateManagerDlg,OpenTemplateHdl));

    // Set OK button position
    Point aBtnPos;
    Size aBtnSize = maButtonClose.GetSizePixel();
    aBtnPos.setX(aWinSize.getWidth() - PADDING_DLG_BORDER - aBtnSize.getWidth());
    aBtnPos.setY(aViewPos.getY()+aThumbSize.getHeight() + PADDING_TOOLBAR_VIEW);
    maButtonClose.SetPosPixel(aBtnPos);

    if (aWinSize.getHeight() != aBtnPos.getY() + aBtnSize.getHeight() + PADDING_DLG_BORDER )
        aWinSize.setHeight(aBtnPos.getY() + aBtnSize.getHeight() + PADDING_DLG_BORDER);

    aButtonAll.SetClickHdl(LINK(this,SfxTemplateManagerDlg,ViewAllHdl));
    aButtonDocs.SetClickHdl(LINK(this,SfxTemplateManagerDlg,ViewDocsHdl));
    aButtonPresents.SetClickHdl(LINK(this,SfxTemplateManagerDlg,ViewPresentsHdl));
    aButtonSheets.SetClickHdl(LINK(this,SfxTemplateManagerDlg,ViewSheetsHdl));
    aButtonDraws.SetClickHdl(LINK(this,SfxTemplateManagerDlg,ViewDrawsHdl));
    maButtonClose.SetClickHdl(LINK(this,SfxTemplateManagerDlg,CloseHdl));
    maButtonSelMode.SetClickHdl(LINK(this,SfxTemplateManagerDlg,OnClickSelectionMode));

    // Set dialog to correct dimensions
    SetSizePixel(aWinSize);

    mpViewBar->Show();
    mpActionBar->Show();

    maView->Populate();
    maView->Show();

    FreeResource();
}

SfxTemplateManagerDlg::~SfxTemplateManagerDlg ()
{
    delete mpSearchEdit;
    delete mpViewBar;
    delete mpActionBar;
    delete mpTemplateBar;
    delete maView;
    delete mpCreateMenu;
    delete mpActionMenu;
    delete mpMoveMenu;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,ViewAllHdl)
{
    maView->filterTemplatesByApp(FILTER_APP_NONE);
    return 0;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,ViewDocsHdl)
{
    maView->filterTemplatesByApp(FILTER_APP_WRITER);
    return 0;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,ViewPresentsHdl)
{
    maView->filterTemplatesByApp(FILTER_APP_IMPRESS);
    return 0;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,ViewSheetsHdl)
{
    maView->filterTemplatesByApp(FILTER_APP_CALC);
    return 0;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,ViewDrawsHdl)
{
    maView->filterTemplatesByApp(FILTER_APP_DRAW);
    return 0;
}

void SfxTemplateManagerDlg::MouseButtonDown( const MouseEvent& rMEvt )
{
    if (!maView->GetActiveClipRegion().IsInside(rMEvt.GetPosPixel()) && maView->isOverlayVisible())
        maView->showOverlay(false);
}

IMPL_LINK_NOARG (SfxTemplateManagerDlg, CloseHdl)
{
    Close();
    return 0;
}

IMPL_LINK (SfxTemplateManagerDlg, OnClickSelectionMode, ImageButton*, pButton)
{
    maView->setSelectionMode(pButton->GetState() == STATE_CHECK);
    return 0;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,TBXViewHdl)
{
    switch(mpViewBar->GetCurItemId())
    {
    case TBI_TEMPLATE_IMPORT:
        OnTemplateImport();
        break;
    default:
        break;
    }

    return 0;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,TBXActionHdl)
{
    switch(mpActionBar->GetCurItemId())
    {
    case TBI_TEMPLATE_SEARCH:
        OnTemplateSearch();
        break;
    default:
        break;
    }

    return 0;
}

IMPL_LINK_NOARG(SfxTemplateManagerDlg,TBXTemplateHdl)
{
    switch(mpTemplateBar->GetCurItemId())
    {
    case TBI_TEMPLATE_EDIT:
        OnTemplateEdit();
        break;
    case TBI_TEMPLATE_PROPERTIES:
        OnTemplateProperties();
        break;
    case TBI_TEMPLATE_MOVE:
        OnTemplateMove();
        break;
    case TBI_TEMPLATE_DELETE:
        OnTemplateDelete();
        break;
    default:
        break;
    }

    return 0;
}

IMPL_LINK(SfxTemplateManagerDlg, TBXDropdownHdl, ToolBox*, pBox)
{
    const sal_uInt16 nCurItemId = pBox->GetCurItemId();

    switch(nCurItemId)
    {
    case TBI_TEMPLATE_CREATE:
        pBox->SetItemDown( nCurItemId, true );

        mpCreateMenu->Execute(pBox,pBox->GetItemRect(TBI_TEMPLATE_CREATE),
                              POPUPMENU_EXECUTE_DOWN);

        pBox->SetItemDown( nCurItemId, false );
        pBox->EndSelection();
        pBox->Invalidate();
        break;
    case TBI_TEMPLATE_ACTION:
        pBox->SetItemDown( nCurItemId, true );

        mpActionMenu->Execute(pBox,pBox->GetItemRect(TBI_TEMPLATE_ACTION),
                              POPUPMENU_EXECUTE_DOWN);

        pBox->SetItemDown( nCurItemId, false );
        pBox->EndSelection();
        pBox->Invalidate();
        break;
    case TBI_TEMPLATE_MOVE:
        pBox->SetItemDown( nCurItemId, true );

        mpMoveMenu->Execute(pBox,pBox->GetItemRect(TBI_TEMPLATE_MOVE),
                            POPUPMENU_EXECUTE_DOWN);

        pBox->SetItemDown( nCurItemId, false );
        pBox->EndSelection();
        pBox->Invalidate();
        break;
    default:
        break;
    }

    return 0;
}

IMPL_LINK(SfxTemplateManagerDlg, TVFolderStateHdl, const ThumbnailViewItem*, pItem)
{
    if (pItem->isSelected())
    {
        maSelFolders.insert(pItem);
    }
    else
    {
        maSelFolders.erase(pItem);
    }

    return 0;
}

IMPL_LINK(SfxTemplateManagerDlg, TVTemplateStateHdl, const ThumbnailViewItem*, pItem)
{
    if (pItem->isSelected())
    {
        if (maSelTemplates.empty())
        {
            mpViewBar->Show(false);
            mpActionBar->Show(false);
            mpTemplateBar->Show();
        }

        maSelTemplates.insert(pItem);
    }
    else
    {
        if (maSelTemplates.find(pItem) != maSelTemplates.end())
        {
            maSelTemplates.erase(pItem);

            if (maSelTemplates.empty())
            {
                mpTemplateBar->Show(false);
                mpViewBar->Show();
                mpActionBar->Show();
            }
        }
    }

    return 0;
}

IMPL_LINK(SfxTemplateManagerDlg, MenuSelectHdl, Menu*, pMenu)
{
    sal_uInt16 nMenuId = pMenu->GetCurItemId();

    switch(nMenuId)
    {
    case MNI_CREATE_TEXT:
        lcl_createTemplate(mxDesktop,FILTER_APP_WRITER);
        break;
    case MNI_CREATE_SHEET:
        lcl_createTemplate(mxDesktop,FILTER_APP_CALC);
        break;
    case MNI_CREATE_PRESENT:
        lcl_createTemplate(mxDesktop,FILTER_APP_IMPRESS);
        break;
    case MNI_CREATE_DRAW:
        lcl_createTemplate(mxDesktop,FILTER_APP_DRAW);
        break;
    case MNI_ACTION_SORT_NAME:
        if (maView->isOverlayVisible())
            maView->sortOverlayItems(SortView_Name());
        else
            maView->sortItems(SortView_Name());
        break;
    default:
        break;
    }

    return 0;
}

IMPL_LINK(SfxTemplateManagerDlg, MoveMenuSelectHdl, Menu*, pMenu)
{
    sal_uInt16 nMenuId = pMenu->GetCurItemId();

    if (nMenuId == MNI_MOVE_NEW)
    {
    }
    else
    {
    }

    return 0;
}

IMPL_LINK(SfxTemplateManagerDlg, OpenTemplateHdl, ThumbnailViewItem*, pItem)
{
    uno::Sequence< PropertyValue > aArgs(1);
    aArgs[0].Name = "AsTemplate";
    aArgs[0].Value <<= sal_True;

    TemplateViewItem *pTemplateItem = static_cast<TemplateViewItem*>(pItem);

    try
    {
        mxDesktop->loadComponentFromURL(pTemplateItem->getPath(),rtl::OUString("_blank"), 0, aArgs );
    }
    catch( const uno::Exception& )
    {
    }

    Close();

    return 0;
}

void SfxTemplateManagerDlg::OnTemplateImport ()
{
    sal_Int16 nDialogType =
        com::sun::star::ui::dialogs::TemplateDescription::FILEOPEN_SIMPLE;

    sfx2::FileDialogHelper aFileDlg(nDialogType, SFXWB_MULTISELECTION);

    // add "All" filter
    aFileDlg.AddFilter( String(SfxResId( STR_SFX_FILTERNAME_ALL) ),
                        DEFINE_CONST_UNICODE(FILEDIALOG_FILTER_ALL) );

    // add template filter
    rtl::OUString sFilterExt;
    rtl::OUString sFilterName( SfxResId( STR_TEMPLATE_FILTER ).toString() );

    // add filters of modules which are installed
    SvtModuleOptions aModuleOpt;
    if ( aModuleOpt.IsModuleInstalled( SvtModuleOptions::E_SWRITER ) )
        sFilterExt += "*.ott;*.stw;*.oth";

    if ( aModuleOpt.IsModuleInstalled( SvtModuleOptions::E_SCALC ) )
    {
        if ( !sFilterExt.isEmpty() )
            sFilterExt += ";";

        sFilterExt += "*.ots;*.stc";
    }

    if ( aModuleOpt.IsModuleInstalled( SvtModuleOptions::E_SIMPRESS ) )
    {
        if ( !sFilterExt.isEmpty() )
            sFilterExt += ";";

        sFilterExt += "*.otp;*.sti";
    }

    if ( aModuleOpt.IsModuleInstalled( SvtModuleOptions::E_SDRAW ) )
    {
        if ( !sFilterExt.isEmpty() )
            sFilterExt += ";";

        sFilterExt += "*.otg;*.std";
    }

    if ( !sFilterExt.isEmpty() )
        sFilterExt += ";";

    sFilterExt += "*.vor";

    sFilterName += " (";
    sFilterName += sFilterExt;
    sFilterName += ")";

    aFileDlg.AddFilter( sFilterName, sFilterExt );
    aFileDlg.SetCurrentFilter( sFilterName );

    ErrCode nCode = aFileDlg.Execute();

    if ( nCode == ERRCODE_NONE )
    {
        com::sun::star::uno::Sequence< ::rtl::OUString > aFiles = aFileDlg.GetSelectedFiles();

        if (aFiles.hasElements())
        {
            std::set<const ThumbnailViewItem*>::const_iterator pIter;
            for (pIter = maSelFolders.begin(); pIter != maSelFolders.end(); ++pIter)
            {
                TemplateFolderViewItem *pFolder = (TemplateFolderViewItem*)(*pIter);

                for (size_t i = 0, n = aFiles.getLength(); i < n; ++i)
                    maView->copyFrom(pFolder,aFiles[i]);
            }
        }
    }
}

void SfxTemplateManagerDlg::OnTemplateSearch ()
{
    Point aPos = maView->GetPosPixel();
    bool bVisible = mpSearchEdit->IsVisible();
    Size aWinSize = GetSizePixel();
    long nEditHeight = mpSearchEdit->GetSizePixel().getHeight();

    if (bVisible)
    {
        aWinSize.setHeight(aWinSize.getHeight() - nEditHeight );
        aPos.setY(aPos.getY() - nEditHeight );
        mpActionBar->SetItemState(TBI_TEMPLATE_SEARCH,STATE_NOCHECK);
    }
    else
    {
        aWinSize.setHeight(aWinSize.getHeight() + nEditHeight );
        aPos.setY(aPos.getY() + nEditHeight );
        mpActionBar->SetItemState(TBI_TEMPLATE_SEARCH,STATE_CHECK);
    }

    SetSizePixel(aWinSize);
    maView->SetPosPixel(aPos);
    mpSearchEdit->Show(!bVisible);
}

void SfxTemplateManagerDlg::OnTemplateEdit ()
{
    uno::Sequence< PropertyValue > aArgs(1);
    aArgs[0].Name = "AsTemplate";
    aArgs[0].Value <<= sal_False;

    uno::Reference< XStorable > xStorable;
    std::set<const ThumbnailViewItem*>::const_iterator pIter;
    for (pIter = maSelTemplates.begin(); pIter != maSelTemplates.end(); ++pIter)
    {
        const TemplateViewItem *pItem = static_cast<const TemplateViewItem*>(*pIter);

        try
        {
            xStorable = uno::Reference< XStorable >(
                        mxDesktop->loadComponentFromURL(pItem->getPath(),rtl::OUString("_blank"), 0, aArgs ),
                        uno::UNO_QUERY );
        }
        catch( const uno::Exception& )
        {
        }
    }
}

void SfxTemplateManagerDlg::OnTemplateProperties ()
{
}

void SfxTemplateManagerDlg::OnTemplateMove ()
{
}

void SfxTemplateManagerDlg::OnTemplateDelete ()
{
    std::set<const ThumbnailViewItem*>::const_iterator pIter;
    for (pIter = maSelTemplates.begin(); pIter != maSelTemplates.end();)
    {
        if (maView->removeTemplate((*pIter)->mnId))
            maSelTemplates.erase(pIter++);
        else
            ++pIter;
    }
}

void lcl_createTemplate(uno::Reference< com::sun::star::frame::XComponentLoader > xDesktop,
                        const FILTER_APPLICATION eApp)
{
    rtl::OUString aURL;

    switch(eApp)
    {
    case FILTER_APP_WRITER:
        aURL = "private:factory/swriter";
        break;
    case FILTER_APP_CALC:
        aURL = "private:factory/scalc";
        break;
    case FILTER_APP_IMPRESS:
        aURL = "private:factory/simpress";
        break;
    case FILTER_APP_DRAW:
        aURL = "private:factory/sdraw";
        break;
    default:
        break;
    }

    if (!aURL.isEmpty())
    {
        uno::Sequence<PropertyValue> aArgs;
        xDesktop->loadComponentFromURL(aURL,rtl::OUString("_blank"), 0, aArgs );
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
