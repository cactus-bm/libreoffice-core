/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright 2012 LibreOffice contributors.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sfx2/templatefolderview.hxx>

#include <comphelper/processfactory.hxx>
#include <sfx2/doctempl.hxx>
#include <sfx2/templateview.hxx>
#include <sfx2/templateviewitem.hxx>
#include <sfx2/templatefolderviewitem.hxx>
#include <svl/inettype.hxx>
#include <svtools/imagemgr.hxx>
#include <tools/urlobj.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <vcl/pngread.hxx>

#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

BitmapEx lcl_fetchThumbnail (const rtl::OUString &msURL, int width, int height)
{
    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;

    // Load the thumbnail from a template document.
    uno::Reference<io::XInputStream> xIStream;

    uno::Reference< lang::XMultiServiceFactory > xServiceManager (
        ::comphelper::getProcessServiceFactory());
    if (xServiceManager.is())
    {
        try
        {
            uno::Reference<lang::XSingleServiceFactory> xStorageFactory(
                xServiceManager->createInstance( "com.sun.star.embed.StorageFactory"),
                uno::UNO_QUERY);

            if (xStorageFactory.is())
            {
                uno::Sequence<uno::Any> aArgs (2);
                aArgs[0] <<= msURL;
                aArgs[1] <<= embed::ElementModes::READ;
                uno::Reference<embed::XStorage> xDocStorage (
                    xStorageFactory->createInstanceWithArguments(aArgs),
                    uno::UNO_QUERY);

                try
                {
                    if (xDocStorage.is())
                    {
                        uno::Reference<embed::XStorage> xStorage (
                            xDocStorage->openStorageElement(
                                "Thumbnails",
                                embed::ElementModes::READ));
                        if (xStorage.is())
                        {
                            uno::Reference<io::XStream> xThumbnailCopy (
                                xStorage->cloneStreamElement("thumbnail.png"));
                            if (xThumbnailCopy.is())
                                xIStream = xThumbnailCopy->getInputStream();
                        }
                    }
                }
                catch (const uno::Exception& rException)
                {
                    OSL_TRACE (
                        "caught exception while trying to access Thumbnail/thumbnail.png of %s: %s",
                        ::rtl::OUStringToOString(msURL,
                            RTL_TEXTENCODING_UTF8).getStr(),
                        ::rtl::OUStringToOString(rException.Message,
                            RTL_TEXTENCODING_UTF8).getStr());
                }

                try
                {
                    // An (older) implementation had a bug - The storage
                    // name was "Thumbnail" instead of "Thumbnails".  The
                    // old name is still used as fallback but this code can
                    // be removed soon.
                    if ( ! xIStream.is())
                    {
                        uno::Reference<embed::XStorage> xStorage (
                            xDocStorage->openStorageElement( "Thumbnail",
                                embed::ElementModes::READ));
                        if (xStorage.is())
                        {
                            uno::Reference<io::XStream> xThumbnailCopy (
                                xStorage->cloneStreamElement("thumbnail.png"));
                            if (xThumbnailCopy.is())
                                xIStream = xThumbnailCopy->getInputStream();
                        }
                    }
                }
                catch (const uno::Exception& rException)
                {
                    OSL_TRACE (
                        "caught exception while trying to access Thumbnails/thumbnail.png of %s: %s",
                        ::rtl::OUStringToOString(msURL,
                            RTL_TEXTENCODING_UTF8).getStr(),
                        ::rtl::OUStringToOString(rException.Message,
                            RTL_TEXTENCODING_UTF8).getStr());
                }
            }
        }
        catch (const uno::Exception& rException)
        {
            OSL_TRACE (
                "caught exception while trying to access tuhmbnail of %s: %s",
                ::rtl::OUStringToOString(msURL,
                    RTL_TEXTENCODING_UTF8).getStr(),
                ::rtl::OUStringToOString(rException.Message,
                    RTL_TEXTENCODING_UTF8).getStr());
        }
    }

    // Extract the image from the stream.
    BitmapEx aThumbnail;
    if (xIStream.is())
    {
        ::std::auto_ptr<SvStream> pStream (
            ::utl::UcbStreamHelper::CreateStream (xIStream));
        ::vcl::PNGReader aReader (*pStream);
        aThumbnail = aReader.Read ();
    }

    aThumbnail.Scale(Size(width,height));

    // Note that the preview is returned without scaling it to the desired
    // width.  This gives the caller the chance to take advantage of a
    // possibly larger resolution then was asked for.
    return aThumbnail;
}

// Display template items depending on the generator application
class ViewFilter_Application
{
public:

    ViewFilter_Application (SfxDocumentTemplates *pDocTemplates, FILTER_APPLICATION App)
        : mApp(App), mpDocTemplates(pDocTemplates)
    {}

    bool operator () (const ThumbnailViewItem *pItem)
    {
        const TemplateViewItem *pTempItem = static_cast<const TemplateViewItem*>(pItem);

        if (mApp == FILTER_APP_WRITER)
        {
            return pTempItem->getFileType() == "OpenDocument Text" ||
                    pTempItem->getFileType() == "OpenDocument Text Template";
        }
        else if (mApp == FILTER_APP_CALC)
        {
            return pTempItem->getFileType() == "OpenDocument Spreadsheet" ||
                    pTempItem->getFileType() == "OpenDocument Spreadsheet Template";
        }
        else if (mApp == FILTER_APP_IMPRESS)
        {
            return pTempItem->getFileType() == "OpenDocument Presentation" ||
                    pTempItem->getFileType() == "OpenDocument Presentation Template";
        }
        else if (mApp == FILTER_APP_DRAW)
        {
            return pTempItem->getFileType() == "OpenDocument Drawing" ||
                    pTempItem->getFileType() == "OpenDocument Drawing Template";
        }

        return true;
    }

private:

    FILTER_APPLICATION mApp;
    SfxDocumentTemplates *mpDocTemplates;
};

class FolderFilter_Application
{
public:

    FolderFilter_Application ( SfxDocumentTemplates *pDocTemplates, FILTER_APPLICATION eApp)
        : meApp(eApp)
    {
        maFilterFunc = ViewFilter_Application(pDocTemplates,eApp);
    }

    bool operator () (const ThumbnailViewItem *pItem)
    {
        TemplateFolderViewItem *pFolderItem = (TemplateFolderViewItem*)pItem;

        std::vector<TemplateViewItem*> &rTemplates = pFolderItem->maTemplates;

        size_t nVisCount = 0;
        ThumbnailViewItem *pTemplateItem;

        // Clear thumbnails
        pFolderItem->maPreview1.Clear();
        pFolderItem->maPreview2.Clear();

        for (size_t i = 0, n = rTemplates.size(); i < n; ++i)
        {
            pTemplateItem = rTemplates[i];

            if (maFilterFunc(pTemplateItem))
            {
                ++nVisCount;

                // Update the thumbnails
                if (nVisCount == 1)
                    pFolderItem->maPreview1 = pTemplateItem->maPreview1;
                else if (nVisCount == 2)
                    pFolderItem->maPreview2 = pTemplateItem->maPreview1;
            }
        }

        return nVisCount;
    }

private:

    FILTER_APPLICATION meApp;
    boost::function<bool (const ThumbnailViewItem*)> maFilterFunc;
};

TemplateFolderView::TemplateFolderView ( Window* pParent, const ResId& rResId, bool bDisableTransientChildren)
    : ThumbnailView(pParent,rResId,bDisableTransientChildren),
      mbFilteredResults(false),
      meFilterOption(FILTER_APP_NONE),
      mpDocTemplates(new SfxDocumentTemplates),
      mpItemView(new TemplateView(this,mpDocTemplates))
{
    mpItemView->SetColor(Color(COL_WHITE));
    mpItemView->SetPosPixel(Point(0,0));
    mpItemView->SetSizePixel(GetOutputSizePixel());
    mpItemView->SetColCount(3);
    mpItemView->SetLineCount(2);

    mpItemView->setItemStateHdl(LINK(this,TemplateFolderView,TVTemplateStateHdl));
}

TemplateFolderView::~TemplateFolderView()
{
    delete mpItemView;
    delete mpDocTemplates;
}

void TemplateFolderView::Populate ()
{
    sal_uInt16 nCount = mpDocTemplates->GetRegionCount();
    for (sal_uInt16 i = 0; i < nCount; ++i)
    {
        rtl::OUString aRegionName(mpDocTemplates->GetFullRegionName(i));

        sal_uInt16 nEntries = mpDocTemplates->GetCount(i);

        if (nEntries)
        {
            TemplateFolderViewItem* pItem = new TemplateFolderViewItem( *this, this );
            pItem->mnId = i+1;
            pItem->maText = aRegionName;
            pItem->setSelectClickHdl(LINK(this,ThumbnailView,OnFolderSelected));

            /// Preview first 2 thumbnails for folder
            pItem->maPreview1 = lcl_fetchThumbnail(mpDocTemplates->GetPath(i,0),128,128);

            if ( nEntries > 2 )
                pItem->maPreview2 = lcl_fetchThumbnail(mpDocTemplates->GetPath(i,1),128,128);

            for (sal_uInt16 j = 0; j < nEntries; ++j)
            {
                rtl::OUString aURL = mpDocTemplates->GetPath(i,j);
                rtl::OUString aType = SvFileInformationManager::GetDescription(INetURLObject(aURL));

                TemplateViewItem *pTemplateItem = new TemplateViewItem(*mpItemView,mpItemView);
                pTemplateItem->mnId = j+1;
                pTemplateItem->maText = mpDocTemplates->GetName(i,j);
                pTemplateItem->setPath(aURL);
                pTemplateItem->setFileType(aType);
                pTemplateItem->maPreview1 = lcl_fetchThumbnail(aURL,128,128);

                pItem->maTemplates.push_back(pTemplateItem);
            }

            mItemList.push_back(pItem);
        }
    }

    CalculateItemPositions();

    if ( IsReallyVisible() && IsUpdateMode() )
        Invalidate();
}

bool TemplateFolderView::isOverlayVisible () const
{
    return mpItemView->IsVisible();
}

void TemplateFolderView::showOverlay (bool bVisible)
{
    mpItemView->Show(bVisible);

    // Clear items is the overlay is closed.
    if (!bVisible)
    {
        // Check if the folder view needs to be filtered
        if (mbFilteredResults)
        {
            filterItems(FolderFilter_Application(mpDocTemplates,meFilterOption));

            mbFilteredResults = false;
            meFilterOption = FILTER_APP_NONE;
        }

        mpItemView->Clear();

        setSelectionMode(mbSelectionMode);
    }
}

void TemplateFolderView::filterTemplatesByApp (const FILTER_APPLICATION &eApp)
{
    meFilterOption = eApp;

    if (mpItemView->IsVisible())
    {
        mbFilteredResults = true;
        mpItemView->filterItems(ViewFilter_Application(mpDocTemplates,eApp));
    }
    else
    {
        filterItems(FolderFilter_Application(mpDocTemplates,eApp));
    }
}

void TemplateFolderView::copyFrom (TemplateFolderViewItem *pItem, const rtl::OUString &rPath)
{
    sal_uInt16 nId = 0;
    sal_uInt16 nRegionId = pItem->mnId - 1;
    String aPath(rPath);

    if (!mItemList.empty())
        nId = (mItemList.back())->mnId+1;

    if (mpDocTemplates->CopyFrom(nRegionId,nId,aPath))
    {
        TemplateViewItem *pTemplate = new TemplateViewItem(*mpItemView,mpItemView);
        pTemplate->mnId = nId;
        pTemplate->maText = mpDocTemplates->GetName(nRegionId,nId);
        pTemplate->maPreview1 = lcl_fetchThumbnail(rPath,128,128);
        pTemplate->setPath(rPath);
        pTemplate->setFileType(SvFileInformationManager::GetDescription(INetURLObject(rPath)));

        pItem->maTemplates.push_back(pTemplate);
    }
}

void TemplateFolderView::OnSelectionMode (bool bMode)
{
    if (mpItemView->IsVisible())
    {
        mbSelectionMode = bMode;
        mpItemView->setSelectionMode(bMode);
    }
    else
        ThumbnailView::OnSelectionMode(bMode);
}

void TemplateFolderView::OnItemDblClicked (ThumbnailViewItem *pRegionItem)
{
    // Fill templates
    sal_uInt16 nRegionId = pRegionItem->mnId-1;

    mpItemView->setRegionId(nRegionId);
    mpItemView->InsertItems(static_cast<TemplateFolderViewItem*>(pRegionItem)->maTemplates);

    if (mbSelectionMode)
        mpItemView->setSelectionMode(true);

    if (meFilterOption != FILTER_APP_NONE)
        mpItemView->filterItems(ViewFilter_Application(mpDocTemplates,meFilterOption));

    mpItemView->Show();
}

IMPL_LINK(TemplateFolderView, TVTemplateStateHdl, const ThumbnailViewItem*, pItem)
{
    maTemplateStateHdl.Call((void*)pItem);
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */


