/*************************************************************************
 *
 *  $RCSfile: MasterPageContainer.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-29 14:35:21 $
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
#include "MasterPageContainer.hxx"

#include "TemplateScanner.hxx"
#include "strings.hrc"
#include <algorithm>
#include <vector>
#include <queue>

#include <comphelper/processfactory.hxx>
#include "unomodel.hxx"
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGES_HPP_
#include <com/sun/star/drawing/XDrawPages.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XSTREAM_HPP_
#include <com/sun/star/io/XStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#include <unotools/ucbstreamhelper.hxx>
#include <vcl/pngread.hxx>
#include <tools/urlobj.hxx>
#include <sfx2/app.hxx>
#include <svx/svdpage.hxx>
#include "DrawDocShell.hxx"
#include "drawdoc.hxx"
#include "sdpage.hxx"
#include <svtools/itemset.hxx>
#include <svtools/eitem.hxx>
#include "stlpool.hxx"
#include "unmovss.hxx"
#include "sdresid.hxx"
#include "tools/IdleDetection.hxx"
#include "pres.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

//#define RENDER_MASTER_PAGES_WITH_PRESENTATION_OBJECTS

namespace {

class PreviewCreationRequest
{
public:
    ::sd::toolpanel::controls::MasterPageContainer::Token maToken;
    int mnWidth;
    Link maCallback;
    void* mpUserData;
    PreviewCreationRequest (
        ::sd::toolpanel::controls::MasterPageContainer::Token aToken,
        int nWidth,
        const Link& rCallback,
        void* pUserData)
        : maToken(aToken),
          mnWidth(nWidth),
          maCallback(rCallback),
          mpUserData(pUserData)
    {}
};


/** A collection of data that is stored for every master page in the
    container.
*/
class MasterPageDescriptor
{
public:
    MasterPageDescriptor (
        const String& rURL,
        const String& rPageName,
        const String& rStyleName,
        SdPage* pMasterPage,
        SdPage* pSlide,
        const Image& rPreview,
        ::sd::toolpanel::controls::MasterPageContainer::Token aToken
        )
        : msURL(rURL),
          msPageName(rPageName),
          msStyleName(rStyleName),
          mpMasterPage(pMasterPage),
          mpSlide(pSlide),
          maPreview(rPreview),
          maToken(aToken)
    {}
    MasterPageDescriptor (const MasterPageDescriptor& rDescriptor)
        : msURL(rDescriptor.msURL),
          msPageName(rDescriptor.msPageName),
          msStyleName(rDescriptor.msStyleName),
          mpMasterPage(rDescriptor.mpMasterPage),
          mpSlide(rDescriptor.mpSlide),
          maPreview(rDescriptor.maPreview),
          maToken(rDescriptor.maToken)
    {}
    ~MasterPageDescriptor (void)
    {}

    String msURL;
    /// Taken from the title of the template file.
    String msPageName;
    /// Taken from the master page object.
    String msStyleName;
    /// The actual master page.
    SdPage* mpMasterPage;
    /// A slide that uses the master page.
    SdPage* mpSlide;
    /// Previews of the master page.  May be empty.
    Image maPreview;            // original preview from file
    Image maScaledPreview;      // scaled size for display
    ::sd::toolpanel::controls::MasterPageContainer::Token maToken;
};

typedef ::std::vector<MasterPageDescriptor> MasterPageContainerType;

class URLComparator { public:
    String msURL;
    URLComparator (const String& sURL) : msURL(sURL) {};
    bool operator() (const MasterPageContainerType::value_type& rDescriptor)
    { return rDescriptor.msURL.CompareTo(msURL)==0; }
};
class PageNameComparator { public:
    String msPageName;
    PageNameComparator (const String& sPageName) : msPageName(sPageName) {};
    bool operator() (const MasterPageContainerType::value_type& rDescriptor)
    { return rDescriptor.msPageName.CompareTo(msPageName)==0; }
};
class StyleNameComparator { public:
    String msStyleName;
    StyleNameComparator (const String& sStyleName) : msStyleName(sStyleName) {};
    bool operator() (const MasterPageContainerType::value_type& rDescriptor)
    { return rDescriptor.msStyleName.CompareTo(msStyleName)==0; }
};
class PageObjectComparator { public:
    const SdPage* mpMasterPage;
    PageObjectComparator (const SdPage* pPageObject)
        : mpMasterPage(pPageObject) {};
    bool operator() (const MasterPageContainerType::value_type& rDescriptor)
    { return rDescriptor.mpMasterPage==mpMasterPage; }
};
class AllComparator { public:
    String msURL;
    String msPageName;
    String msStyleName;
    const SdPage* mpMasterPage;
    AllComparator (
        const String& sURL,
        const String& sPageName,
        const String& sStyleName,
        const SdPage* pPageObject)
        : msURL(sURL),
          msPageName(sPageName),
          msStyleName(sStyleName),
          mpMasterPage(pPageObject)
    {}
    bool operator() (const MasterPageContainerType::value_type& rDescriptor)
    {
        return
            (msURL.Len()>0
                &&rDescriptor.msURL.CompareTo(msURL)==0)
            || (msPageName.Len()>0
                && rDescriptor.msPageName.CompareTo(msPageName)==0)
            || (msStyleName.Len()>0
                && rDescriptor.msStyleName.CompareTo(msStyleName)==0)
            || (mpMasterPage!=NULL
                && rDescriptor.mpMasterPage==mpMasterPage);
    }
};


} // end of anonymous namespace




namespace sd { namespace toolpanel { namespace controls {


/** Inner implementation class of the MasterPageContainer.  For
    documentation of undocumented methods and members please look in
    MasterPageContainer at method or member with the same name.
*/
class MasterPageContainer::Implementation
{
public:
    static MasterPageContainer* mpInstance;

    MasterPageContainerType maContainer;

    Implementation (void);
    ~Implementation (void);

    void AddPreviewCreationRequest (
        Token aToken,
         int nWidth,
        const Link& rCallback,
        void* pUserData);

    SdPage* GetPageObjectForToken (
        MasterPageContainer::Token aToken,
        bool bLoad);
    SdPage* GetSlideForToken (
        MasterPageContainer::Token aToken,
        bool bLoad);
    SdPage* CopyMasterPageToLocalDocument (SdPage* pMasterPage);
    Token PutMasterPage (
        const String& sURL,
        const String& sPageName,
        const String& sStyleName,
        SdPage* pMasterPage,
        Image aPreview);
    Image GetPreviewForToken (
        Token aToken,
        int nWidth);
    Image GetPreviewForToken (
        Token aToken,
        int nWidth,
        const Link& rCallback,
        void* pUserData);
    void RemoveCallback (const Link& rCallback);
    Image GetPreviewForPage (
        SdPage* pPage,
        int nWidth);

private:
    Timer maDelayedPreviewCreationTimer;

    /** Use a vector for the queue so that we can iterate over all
        elements and remove those that became invalid.
    */
    typedef ::std::queue<PreviewCreationRequest> RequestQueue;
    RequestQueue maRequestQueue;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::frame::XModel> mxModel;
    SdDrawDocument* mpDocument;
    PreviewRenderer maPreviewRenderer;

    /// The time to wait (in milliseconds) between the creation of previews.
    static const int DELAYED_CREATION_TIMEOUT=250;
    /// The time to wait when the system is not idle.
    static const int DELAYED_CREATION_TIMEOUT_WHEN_NOT_IDLE=1000;

    DECL_LINK(DelayedPreviewCreation, Timer *);
    ::sd::DrawDocShell* LoadDocument (
        const String& sFileName,
        SfxObjectShellLock& rxDocumentShell);
    SdPage* AddMasterPage (
        SdDrawDocument* pTargetDocument,
        SdPage* pMasterPage);
    void ProvideStyles (
        SdDrawDocument* pSourceDocument,
        SdDrawDocument* pTargetDocument,
        SdPage* pPage);
    SdPage* GetSlideForMasterPage (SdPage* pMasterPage);

    /** Retrieve the preview of the document specified by the given URL.
    */
    BitmapEx LoadPreviewFromURL (const ::rtl::OUString& aURL);

    /** Fetch previews of the master pages of all relevant templates,
        styles, and layouts and put them in the container.
    */
    void FillContainer (void);

    /** This function is used by FillContainer() to add the master page of
        one of the templates.  It tries to load the thumbnail bitmap from
        the file.  If that is not present the preview remains empty save a
        short text indicating what happened.
    */
    void AddTemplate (
        const String& rsPath,
        const String& rsTitle);

    /** Also used by FillContainer() to filter out those templates among
        those provided by the ucb that can be handled by us, i.e. Impress
        templates, layouts, styles.
        @param rsFileName
            The full file name, possibly a URL, that is used to recognize
            whether a template can be handled by this class.
    */
    bool FileFilter (const String& rsFileName);
};




//===== MasterPageContainer ===================================================

MasterPageContainer* MasterPageContainer::Implementation::mpInstance = NULL;
static const MasterPageContainer::Token NIL_TOKEN (-1);

MasterPageContainer& MasterPageContainer::Instance (void)
{
    if (Implementation::mpInstance == NULL)
    {
        ::osl::GetGlobalMutex aMutexFunctor;
        ::osl::MutexGuard aGuard (aMutexFunctor());
        if (Implementation::mpInstance == NULL)
        {
            MasterPageContainer* pInstance = new MasterPageContainer ();
            pInstance->LateInit();
            SdGlobalResourceContainer::Instance().AddResource (
                ::std::auto_ptr<SdGlobalResource>(pInstance));
            OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
            Implementation::mpInstance = pInstance;
        }
    }
    else
        OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();

    DBG_ASSERT (Implementation::mpInstance!=NULL,
        "MasterPageContainer::Instance(): instance is NULL");
    return *Implementation::mpInstance;
}




MasterPageContainer::MasterPageContainer (void)
    : mpImpl(new Implementation())
{
}




void MasterPageContainer::LateInit (void)
{
}




MasterPageContainer::~MasterPageContainer (void)
{
}




MasterPageContainer::Token MasterPageContainer::PutMasterPage (
    const String& sURL,
    const String& sPageName,
    const String& sStyleName,
    SdPage* pMasterPage,
    Image aPreview)
{
    return mpImpl->PutMasterPage (
        sURL,
        sPageName,
        sStyleName,
        pMasterPage,
        aPreview);
}




int MasterPageContainer::GetTokenCount (void)
{
    return mpImpl->maContainer.size();
}




MasterPageContainer::Token MasterPageContainer::GetTokenForIndex (int nIndex)
{
    Token aResult (NIL_TOKEN);
    if (nIndex>=0 && nIndex<GetTokenCount())
        aResult = mpImpl->maContainer[nIndex].maToken;
    return aResult;
}




MasterPageContainer::Token MasterPageContainer::GetTokenForURL (
    const String& sURL)
{
    Token aResult (NIL_TOKEN);
    if (sURL.Len() > 0)
    {
        MasterPageContainerType::iterator aEntry (
            ::std::find_if (
                mpImpl->maContainer.begin(),
                mpImpl->maContainer.end(),
                URLComparator(sURL)));
        if (aEntry != mpImpl->maContainer.end())
            aResult = aEntry->maToken;
    }
    return aResult;
}




MasterPageContainer::Token MasterPageContainer::GetTokenForPageName (
    const String& sPageName)
{
    Token aResult (NIL_TOKEN);
    if (sPageName.Len() > 0)
    {
        MasterPageContainerType::iterator aEntry (
            ::std::find_if (
                mpImpl->maContainer.begin(),
                mpImpl->maContainer.end(),
                PageNameComparator(sPageName)));
        if (aEntry != mpImpl->maContainer.end())
            return aEntry->maToken;
    }
    return aResult;
}




MasterPageContainer::Token MasterPageContainer::GetTokenForStyleName (const String& sStyleName)
{
    Token aResult (NIL_TOKEN);
    if (sStyleName.Len() > 0)
    {
        MasterPageContainerType::iterator aEntry (
            ::std::find_if (
                mpImpl->maContainer.begin(),
                mpImpl->maContainer.end(),
                StyleNameComparator(sStyleName)));
        if (aEntry != mpImpl->maContainer.end())
            return aEntry->maToken;
    }
    return aResult;
}




MasterPageContainer::Token MasterPageContainer::GetTokenForPageObject (
    const SdPage* pPage)
{
    Token aResult (NIL_TOKEN);
    if (pPage != NULL)
    {
        MasterPageContainerType::iterator aEntry (
            ::std::find_if (
                mpImpl->maContainer.begin(),
                mpImpl->maContainer.end(),
                PageObjectComparator(pPage)));
        if (aEntry != mpImpl->maContainer.end())
            return aEntry->maToken;
    }
    return aResult;
}




String MasterPageContainer::GetURLForToken (
    MasterPageContainer::Token aToken)
{
    if (aToken>=0 && (unsigned)aToken<mpImpl->maContainer.size())
        return mpImpl->maContainer[aToken].msURL;
    else
        return String();
}




String MasterPageContainer::GetPageNameForToken (
    MasterPageContainer::Token aToken)
{
    if (aToken>=0 && (unsigned)aToken<mpImpl->maContainer.size())
        return mpImpl->maContainer[aToken].msPageName;
    else
        return String();
}




String MasterPageContainer::GetStyleNameForToken (
    MasterPageContainer::Token aToken)
{
    if (aToken>=0 && (unsigned)aToken<mpImpl->maContainer.size())
        return mpImpl->maContainer[aToken].msStyleName;
    else
        return String();
}




SdPage* MasterPageContainer::GetPageObjectForToken (
    MasterPageContainer::Token aToken,
    bool bLoad)
{
    return mpImpl->GetPageObjectForToken (aToken, bLoad);
}




SdPage* MasterPageContainer::GetSlideForToken (
    MasterPageContainer::Token aToken,
    bool bLoad)
{
    return mpImpl->GetSlideForToken (aToken, bLoad);
}




Image MasterPageContainer::GetPreviewForToken (
    MasterPageContainer::Token aToken,
    int nWidth)
{
    return mpImpl->GetPreviewForToken (aToken, nWidth);
}




Image MasterPageContainer::GetPreviewForToken (
    MasterPageContainer::Token aToken,
    int nWidth,
    const Link& rCallback,
    void* pUserData)
{
    return mpImpl->GetPreviewForToken (
            aToken,
            nWidth,
            rCallback,
            pUserData);
}




Image MasterPageContainer::GetPreviewForPage (
    SdPage* pPage,
    int nWidth)
{
    return mpImpl->GetPreviewForPage(pPage, nWidth);
}




void MasterPageContainer::RemoveCallback (const Link& rCallback)
{
    mpImpl->RemoveCallback (rCallback);
}




//==== Implementation ================================================

MasterPageContainer::Implementation::Implementation (void)
    : maContainer(),
      maDelayedPreviewCreationTimer(),
      maRequestQueue(),
      mxModel(NULL),
      mpDocument(NULL),
      maPreviewRenderer()
{
    try
    {
        // Use the API to create a new document.

        // Get the desktop a s service factory.
        ::rtl::OUString sDesktopServiceName (
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop"));
        uno::Reference<frame::XComponentLoader> xDesktop (
            ::comphelper::getProcessServiceFactory()->createInstance(
                sDesktopServiceName),
            uno::UNO_QUERY);

        // Create a new model.
        ::rtl::OUString sModelServiceName (
            RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.presentation.PresentationDocument"));
        mxModel = uno::Reference<frame::XModel>(
            ::comphelper::getProcessServiceFactory()->createInstance(
                sModelServiceName),
            uno::UNO_QUERY);

        // Initialize it.
        uno::Reference<frame::XLoadable> xLoadable (mxModel,uno::UNO_QUERY);
        if (xLoadable.is())
            xLoadable->initNew();

        // Use its tunnel to get a pointer to its core implementation.
        uno::Reference<lang::XUnoTunnel> xUnoTunnel (mxModel, uno::UNO_QUERY);
        if (xUnoTunnel.is())
        {
            mpDocument = reinterpret_cast<SdXImpressDocument*>(
                xUnoTunnel->getSomething(
                    SdXImpressDocument::getUnoTunnelId()))->GetDoc();
        }

        // Create slides that are used to render previews of empty pages.
        uno::Reference<drawing::XDrawPagesSupplier> xSlideSupplier (
            mxModel, uno::UNO_QUERY);
        if (xSlideSupplier.is())
        {
            uno::Reference<drawing::XDrawPages> xSlides (
                xSlideSupplier->getDrawPages(), uno::UNO_QUERY);
            if (xSlides.is())
            {
                xSlides->insertNewByIndex (0);
                xSlides->insertNewByIndex (1);
            }
        }
    }
    catch (...)
    {
        mxModel = NULL;
        mpDocument = NULL;
    }

    // Set up the timer for the delayed creation of preview bitmaps.
    maDelayedPreviewCreationTimer.SetTimeout (DELAYED_CREATION_TIMEOUT);
    maDelayedPreviewCreationTimer.SetTimeoutHdl (
        LINK(this,Implementation,DelayedPreviewCreation));

    FillContainer();
}




MasterPageContainer::Implementation::~Implementation (void)
{
    maDelayedPreviewCreationTimer.Stop();
    while ( ! maRequestQueue.empty())
        maRequestQueue.pop();

   uno::Reference<util::XCloseable> xCloseable (mxModel, uno::UNO_QUERY);
    if (xCloseable.is())
    {
        try
        {
            xCloseable->close(true);
        }
        catch (::com::sun::star::util::CloseVetoException aException)
        {
        }
    }
    mxModel = NULL;
}




void MasterPageContainer::Implementation::AddPreviewCreationRequest (
    ::sd::toolpanel::controls::MasterPageContainer::Token aToken,
    int nWidth,
    const Link& rCallback,
    void* pUserData)
{
    maRequestQueue.push (PreviewCreationRequest(
        aToken, nWidth, rCallback, pUserData));
    maDelayedPreviewCreationTimer.Start();
}




IMPL_LINK(MasterPageContainer::Implementation,
    DelayedPreviewCreation,
    Timer*, pTimer)
{
    bool bIsShowingFullScreenShow (true);

    while (maRequestQueue.size() > 0)
    {
        // First check whether the system is idle.
        sal_Int32 nIdleState (tools::IdleDetection::GetIdleState());
        if (nIdleState != tools::IdleDetection::IDET_IDLE)
        {
            if ((nIdleState&tools::IdleDetection::IDET_FULL_SCREEN_SHOW_ACTIVE) != 0)
                bIsShowingFullScreenShow = true;
            break;
        }

        PreviewCreationRequest aRequest (maRequestQueue.front());
        maRequestQueue.pop();

        if (aRequest.maToken>=0
            && (unsigned)aRequest.maToken<maContainer.size())
        {
            String sURL (maContainer[aRequest.maToken].msURL);
            // Store bitmap in original size so that both preview sizes have
            // access to the full resolution.
            // #i42576# read from file only if preview not avilable yet
            if ( maContainer[aRequest.maToken].maPreview.GetSizePixel().Width() == 0 )
                maContainer[aRequest.maToken].maPreview = LoadPreviewFromURL (sURL);
            aRequest.maCallback.Call (aRequest.mpUserData);
        }
    }

    if (maRequestQueue.size() > 0)
    {
        if (bIsShowingFullScreenShow)
            maDelayedPreviewCreationTimer.SetTimeout (DELAYED_CREATION_TIMEOUT_WHEN_NOT_IDLE);
        else
            maDelayedPreviewCreationTimer.SetTimeout (DELAYED_CREATION_TIMEOUT);
        pTimer->Start();
    }

    return 0;
}




MasterPageContainer::Token MasterPageContainer::Implementation::PutMasterPage (
    const String& sURL,
    const String& sPageName,
    const String& sStyleName,
    SdPage* pMasterPage,
    Image aPreview)
{
    Token aResult (NIL_TOKEN);

    MasterPageContainerType::iterator aEntry (
        ::std::find_if (
            maContainer.begin(),
            maContainer.end(),
            AllComparator(sURL,sPageName,sStyleName,pMasterPage)));
    if (aEntry == maContainer.end())
    {
        bool bIgnore = false;
        SdPage* pLocalMasterPage = NULL;
        SdPage* pLocalSlide = NULL;
        if (pMasterPage != NULL)
        {
            // When a master page is given then copy it to the local document.
            pLocalMasterPage = CopyMasterPageToLocalDocument (pMasterPage);
            pLocalSlide = GetSlideForMasterPage(pLocalMasterPage);
            bIgnore = (pLocalMasterPage==NULL || pLocalSlide==NULL);
        }
        else
            // When no master page is given then at least the URL has to be
            // there.
            bIgnore = (sURL.Len() == 0);

        if ( ! bIgnore)
        {
            aResult = maContainer.size();
            maContainer.push_back (MasterPageDescriptor (
                sURL,
                sPageName,
                sStyleName,
                pLocalMasterPage,
                pLocalSlide,
                aPreview,
                aResult));
        }
    }
    else
    {
        // Update the existing entry.
        if (aEntry->msURL.Len() == 0)
            aEntry->msURL = sURL;
        if (aEntry->msPageName.Len() == 0)
            aEntry->msPageName = sPageName;
        if (aEntry->msStyleName.Len() == 0)
            aEntry->msStyleName = sStyleName;
        if (aEntry->mpMasterPage == NULL)
            aEntry->mpMasterPage = pMasterPage;
        Size aImageSize (aEntry->maPreview.GetSizePixel());
        if (aImageSize.Width()<=0 || aImageSize.Height()<=0)
            aEntry->maPreview = aPreview;
        aResult = aEntry->maToken;
    }

    return aResult;
}




SdPage* MasterPageContainer::Implementation::GetPageObjectForToken (
    MasterPageContainer::Token aToken,
    bool bLoad)
{
    SdPage* pPageObject = NULL;
    if (aToken>=0 && (unsigned)aToken<maContainer.size())
    {
        pPageObject = maContainer[aToken].mpMasterPage;

        if (pPageObject == NULL && bLoad)
        {
            SfxObjectShellLock xDocumentShell (NULL);
            ::sd::DrawDocShell* pDocumentShell = NULL;
            try
            {
                pDocumentShell = LoadDocument (
                    maContainer[aToken].msURL,
                    xDocumentShell);
                if (pDocumentShell != NULL)
                {
                    SdDrawDocument* pDocument = pDocumentShell->GetDoc();
                    SdPage* pPage = pDocument->GetMasterSdPage(0, PK_STANDARD);
                    pPageObject = CopyMasterPageToLocalDocument (pPage);
                    maContainer[aToken].mpMasterPage = pPageObject;
                    maContainer[aToken].mpSlide
                        = GetSlideForMasterPage(pPageObject);
                    maContainer[aToken].msStyleName = pPageObject->GetName();
                    // Delete an existing substitution. The next request for
                    // a preview will create the real one.
                    maContainer[aToken].maPreview = Image();
                    //AF delete pDocumentShell;
                }
            }
            catch (...)
            {
                pPageObject = NULL;
                OSL_TRACE ("caught general exception");
            }
        }
    }
    return pPageObject;
}




SdPage* MasterPageContainer::Implementation::GetSlideForToken (
    Token aToken,
    bool bLoad=true)
{
    SdPage* pSlide = NULL;
    if (aToken>=0 && (unsigned)aToken<maContainer.size())
        pSlide = maContainer[aToken].mpSlide;

    if (pSlide==NULL && bLoad)
    {
        GetPageObjectForToken (aToken, bLoad);
        pSlide = maContainer[aToken].mpSlide;
    }

    return pSlide;
}




DrawDocShell* MasterPageContainer::Implementation::LoadDocument (
    const String& sFileName,
    SfxObjectShellLock& rxDocumentShell)
{
    SfxApplication* pSfxApp = SFX_APP();
    SfxItemSet* pSet = new SfxAllItemSet (pSfxApp->GetPool());
    pSet->Put (SfxBoolItem (SID_TEMPLATE, TRUE));
    pSet->Put (SfxBoolItem (SID_PREVIEW, TRUE));
    if (pSfxApp->LoadTemplate (rxDocumentShell, sFileName, TRUE, pSet))
    {
        rxDocumentShell = NULL;
    }
    SfxObjectShell* pShell = rxDocumentShell;
    return PTR_CAST(DrawDocShell,pShell);
}




SdPage* MasterPageContainer::Implementation::CopyMasterPageToLocalDocument (
    SdPage* pMasterPage)
{
    SdPage* pNewMasterPage = NULL;

    do
    {
        if (pMasterPage == NULL)
            break;

        // Check the presence of the target document.
        if (mpDocument == NULL)
            break;
        // Check the presence of the source document.
        SdDrawDocument* pSourceDocument = static_cast<SdDrawDocument*>(
            pMasterPage->GetModel());
        if (pSourceDocument == NULL)
            break;

        // Test if the master pages of both the slide and its notes page are
        // present.  This is not the case when we are called during the
        // creation of the slide master page because then the notes master
        // page is not there.
        USHORT nSourceMasterPageCount = pSourceDocument->GetMasterPageCount();
        if (nSourceMasterPageCount%2 == 0)
            // There should be 1 handout page + n slide masters + n notes
            // masters = 2*n+1.  An even value indicates that a new slide
            // master but not yet the notes master has been inserted.
            break;
        USHORT nIndex = pMasterPage->GetPageNum();
        if (nSourceMasterPageCount <= nIndex+1)
            break;
        // Get the slide master page.
        if (pMasterPage != static_cast<SdPage*>(
            pSourceDocument->GetMasterPage(nIndex)))
            break;
        // Get the notes master page.
        SdPage* pNotesMasterPage = static_cast<SdPage*>(
            pSourceDocument->GetMasterPage(nIndex+1));
        if (pNotesMasterPage == NULL)
            break;


        // Check if a master page with the same name as that of the given
        // master page already exists.
        bool bPageExists (false);
        USHORT nMasterPageCount(mpDocument->GetMasterSdPageCount(PK_STANDARD));
        for (USHORT nIndex=0; nIndex<nMasterPageCount; nIndex++)
        {
            SdPage* pCandidate = static_cast<SdPage*>(
                mpDocument->GetMasterSdPage (nIndex, PK_STANDARD));
            if (pMasterPage!=NULL
                && pCandidate->GetName().CompareTo(pMasterPage->GetName())==0)
            {
                bPageExists = true;
                pNewMasterPage = pCandidate;
                break;
            }
        }
        if (bPageExists)
            break;

        // Create a new slide (and its notes page.)
        uno::Reference<drawing::XDrawPagesSupplier> xSlideSupplier (
            mxModel, uno::UNO_QUERY);
        if ( ! xSlideSupplier.is())
            break;
        uno::Reference<drawing::XDrawPages> xSlides (
            xSlideSupplier->getDrawPages(), uno::UNO_QUERY);
        if ( ! xSlides.is())
            break;
        xSlides->insertNewByIndex (xSlides->getCount());

        // Set a layout.
        SdPage* pSlide = mpDocument->GetSdPage(
            mpDocument->GetSdPageCount(PK_STANDARD)-1,
            PK_STANDARD);
        if (pSlide == NULL)
            break;

        // Create a copy of the master page and the associated notes
        // master page and insert them into our document.
        pNewMasterPage = AddMasterPage(mpDocument, pMasterPage);
        if (pNewMasterPage==NULL)
            break;
        SdPage* pNewNotesMasterPage
            = AddMasterPage(mpDocument, pNotesMasterPage);
        if (pNewNotesMasterPage==NULL)
            break;

        // Make the connection from the new slide to the master page
        // (and do the same for the notes page.)
        mpDocument->SetMasterPage (
            mpDocument->GetSdPageCount(PK_STANDARD)-1,
            pNewMasterPage->GetName(),
            mpDocument,
            FALSE, // Connect the new master page with the new slide but
                   // do not modify other (master) pages.
            TRUE);
    }
    while (false);

    // We are not interested in any automatisms for our modified internal
    // document.
    mpDocument->SetChanged (sal_False);

    return pNewMasterPage;
}




SdPage* MasterPageContainer::Implementation::AddMasterPage (
    SdDrawDocument* pTargetDocument,
    SdPage* pMasterPage)
{
    SdPage* pClonedMasterPage = NULL;

    if (pTargetDocument!=NULL && pMasterPage!=NULL)
    {
        try
        {
            // Duplicate the master page.
            pClonedMasterPage = static_cast<SdPage*>(pMasterPage->Clone());

            // Copy the necessary styles.
            SdDrawDocument* pSourceDocument
                = static_cast<SdDrawDocument*>(pMasterPage->GetModel());
            ProvideStyles (pSourceDocument,
                pTargetDocument, pClonedMasterPage);

            // Now that the styles are available we can insert the cloned
            // master page.
            pTargetDocument->InsertMasterPage (pClonedMasterPage);
        }
        catch (Exception& rException)
        {
            pClonedMasterPage = NULL;
            OSL_TRACE("caught exception while adding master page: %s",
                ::rtl::OUStringToOString(rException.Message,
                    RTL_TEXTENCODING_UTF8).getStr());
        }
        catch (::std::exception rException)
        {
            pClonedMasterPage = NULL;
            OSL_TRACE ("caught general exception");
        }
        catch (...)
        {
            pClonedMasterPage = NULL;
            OSL_TRACE ("caught general exception");
        }
    }

    return pClonedMasterPage;
}




void MasterPageContainer::Implementation::ProvideStyles (
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




Image MasterPageContainer::Implementation::GetPreviewForToken (
    MasterPageContainer::Token aToken,
    int nWidth)
{
    Image aPreview;
    if (aToken>=0 && (unsigned)aToken<maContainer.size())
    {
        aPreview = maContainer[aToken].maPreview;
        if (aPreview.GetSizePixel().Width() == nWidth)
        {
            // The existing preview already has the right size so use that.
        }
        else if (maContainer[aToken].mpMasterPage!=NULL)
        {
            // We have the page in memory so we can render it in the desired
            // size.
            aPreview = maPreviewRenderer.RenderPage(
#ifdef RENDER_MASTER_PAGES_WITH_PRESENTATION_OBJECTS
                maContainer[aToken].mpMasterPage,
#else
                maContainer[aToken].mpSlide,
#endif
                nWidth,
                String::CreateFromAscii(""));
            maContainer[aToken].maScaledPreview = aPreview;
        }
        else if (nWidth>0 && aPreview.GetSizePixel().Width()>0)
        {
            // #i42576# scale preview only if required and cache it
            if( maContainer[aToken].maScaledPreview.GetSizePixel().Width() != nWidth )
            {
                maContainer[aToken].maScaledPreview = maPreviewRenderer.ScaleBitmap (
                    aPreview.GetBitmapEx(),
                    nWidth);
            }
            aPreview = maContainer[aToken].maScaledPreview;
        }
        else if (maContainer[aToken].msURL.Len() > 0)
        {
            // We have a URL.  Try to load the thumbnail from that file.
            BitmapEx aBitmap (LoadPreviewFromURL (maContainer[aToken].msURL));
            maContainer[aToken].maPreview = aBitmap;
            aPreview = maPreviewRenderer.ScaleBitmap (aBitmap, nWidth);
        }

        if (aPreview.GetSizePixel().Width()==0
            || aPreview.GetSizePixel().Height()==0)
        {
            // All else failed so create an empty preview with a text that
            // tells the user that there is no preview available.
            static Image sSubstPreview;
            if( nWidth != sSubstPreview.GetSizePixel().Width() )
            {
                sSubstPreview = maPreviewRenderer.RenderPage(
                    mpDocument->GetSdPage(0, PK_STANDARD),
                    nWidth,
                    SdResId(STR_TASKPANEL_NOT_AVAILABLE_SUBSTITUTION));
            }
            maContainer[aToken].maPreview = sSubstPreview;
            maContainer[aToken].maScaledPreview = sSubstPreview;
            aPreview = sSubstPreview;
        }
    }

    return aPreview;
}




Image MasterPageContainer::Implementation::GetPreviewForToken (
    MasterPageContainer::Token aToken,
    int nWidth,
    const Link& rCallback,
    void* pUserData)
{
    Image aPreview;
    SdPage* pPage = NULL;
    if (aToken>=0 && (unsigned)aToken<maContainer.size())
    {
        aPreview = maContainer[aToken].maPreview;
        pPage = maContainer[aToken].mpMasterPage;
    }

    bool bShowSubstitution = false;
    Size aSize (aPreview.GetSizePixel());
    if (aPreview.GetSizePixel().Width() == nWidth)
    {
        // The existing preview already has the right size so use that.
        bShowSubstitution = false;
    }
    else if (pPage != NULL)
    {
        // We have the page in memory so we can render it in the desired
        // size.
        aPreview = maPreviewRenderer.RenderPage(
#ifdef RENDER_MASTER_PAGES_WITH_PRESENTATION_OBJECTS
                maContainer[aToken].mpMasterPage,
#else
                maContainer[aToken].mpSlide,
#endif
                nWidth,
                String::CreateFromAscii(""));
        maContainer[aToken].maScaledPreview = aPreview;
        bShowSubstitution = false;
    }
    else if (nWidth>0 && aPreview.GetSizePixel().Width()>0)
    {
        // We already have a preview so we scale that to the desired size.
        // #i42576# scale preview only if required and cache it
        if( maContainer[aToken].maScaledPreview.GetSizePixel().Width() != nWidth )
        {
            maContainer[aToken].maScaledPreview = maPreviewRenderer.ScaleBitmap (
                aPreview.GetBitmapEx(),
                nWidth);
        }
        aPreview = maContainer[aToken].maScaledPreview;
        bShowSubstitution = false;
    }
    else
    {
        // The page object has to be loaded.  That takes so long that we
        // do it asynchronously.
        AddPreviewCreationRequest(
            aToken,
            nWidth,
            rCallback,
            pUserData);
        bShowSubstitution = true;
    }

    if (bShowSubstitution)
    {
        static Image sSubstPreview;
        if( nWidth != sSubstPreview.GetSizePixel().Width() )
        {
            // #i42576# cache substitution bitmap
            sSubstPreview = maPreviewRenderer.RenderPage(
                mpDocument->GetSdPage(0, PK_STANDARD),
                nWidth,
                SdResId(STR_TASKPANEL_PREPARING_PREVIEW_SUBSTITUTION));
        }
        aPreview = sSubstPreview;
    }

    return aPreview;
}




Image MasterPageContainer::Implementation::GetPreviewForPage (
    SdPage* pPage,
    int nWidth)
{
    return maPreviewRenderer.RenderPage(
        pPage,
        nWidth,
        String::CreateFromAscii(""));
}




void MasterPageContainer::Implementation::RemoveCallback (
    const Link& rCallback)
{
    maDelayedPreviewCreationTimer.Stop();

    // Remove all entries from the queue for the given callback.

    // Move all entries to the temporary queue that will not call the
    // specified callback.
    RequestQueue aTemporaryQueue;
    while ( ! maRequestQueue.empty())
    {
        PreviewCreationRequest aRequest (maRequestQueue.front());
        maRequestQueue.pop();
        if (aRequest.maCallback != rCallback)
            aTemporaryQueue.push (aRequest);
    }

    // Move the entries back to the queue member to reverse the order
    // of the temprorary queue.
    while ( ! aTemporaryQueue.empty())
    {
        maRequestQueue.push (aTemporaryQueue.front());
        aTemporaryQueue.pop();
    }

    // Start the timer again when there are entries left.
    if ( ! maRequestQueue.empty())
        maDelayedPreviewCreationTimer.Start();
}




SdPage* MasterPageContainer::Implementation::GetSlideForMasterPage (
    SdPage* pMasterPage)
{
    SdPage* pCandidate = NULL;

    // Iterate over all pages and check if it references the given master
    // page.
    if (mpDocument->GetSdPageCount(PK_STANDARD) > 0)
    {
        // In most cases a new slide has just been inserted so start with
        // the last page.
        USHORT nPageIndex (mpDocument->GetSdPageCount(PK_STANDARD)-1);
        bool bFound (false);
        while ( ! bFound)
        {
            pCandidate = mpDocument->GetSdPage(
                nPageIndex,
                PK_STANDARD);
            if (pCandidate != NULL)
            {
                if (static_cast<SdPage*>(&pCandidate->TRG_GetMasterPage())
                    == pMasterPage)
                {
                    bFound = true;
                    break;
                }
            }

            if (nPageIndex == 0)
                break;
            else
                nPageIndex --;
        }

        // If no page was found that refernced the given master page reset
        // the pointer that is returned.
        if ( ! bFound)
            pCandidate = NULL;
    }

    return pCandidate;
}




BitmapEx MasterPageContainer::Implementation::LoadPreviewFromURL (
    const ::rtl::OUString& aURL)
{
    uno::Reference<io::XInputStream> xIStream;

    uno::Reference< lang::XMultiServiceFactory > xServiceManager (
        ::comphelper::getProcessServiceFactory());
    if (xServiceManager.is())
    {
        try
        {
            uno::Reference<lang::XSingleServiceFactory> xStorageFactory(
                xServiceManager->createInstance(
                    ::rtl::OUString::createFromAscii(
                        "com.sun.star.embed.StorageFactory")),
                uno::UNO_QUERY);

            if (xStorageFactory.is())
            {
                uno::Sequence<uno::Any> aArgs (2);
                aArgs[0] <<= aURL;
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
                                ::rtl::OUString::createFromAscii("Thumbnails"),
                                embed::ElementModes::READ));
                        if (xStorage.is())
                        {
                            uno::Reference<io::XStream> xThumbnailCopy (
                                xStorage->cloneStreamElement(
                                    ::rtl::OUString::createFromAscii(
                                        "thumbnail.png")));
                            if (xThumbnailCopy.is())
                                xIStream = xThumbnailCopy->getInputStream();
                        }
                    }
                }
                catch (uno::Exception& rException)
                {
                    OSL_TRACE (
                        "caught exception while trying to access Thumbnail/thumbnail.png of %s: %s",
                        ::rtl::OUStringToOString(aURL,
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
                            xDocStorage->openStorageElement(
                                ::rtl::OUString::createFromAscii("Thumbnail"),
                                embed::ElementModes::READ));
                        if (xStorage.is())
                        {
                            uno::Reference<io::XStream> xThumbnailCopy (
                                xStorage->cloneStreamElement(
                                    ::rtl::OUString::createFromAscii(
                                        "thumbnail.png")));
                            if (xThumbnailCopy.is())
                                xIStream = xThumbnailCopy->getInputStream();
                        }
                    }
                }
                catch (uno::Exception& rException)
                {
                    OSL_TRACE (
                        "caught exception while trying to access Thumbnails/thumbnail.png of %s: %s",
                        ::rtl::OUStringToOString(aURL,
                            RTL_TEXTENCODING_UTF8).getStr(),
                        ::rtl::OUStringToOString(rException.Message,
                            RTL_TEXTENCODING_UTF8).getStr());
                }
            }
        }
        catch (uno::Exception& rException)
        {
            OSL_TRACE (
                "caught exception while trying to access tumbnail of %s: %s",
                ::rtl::OUStringToOString(aURL,
                    RTL_TEXTENCODING_UTF8).getStr(),
                ::rtl::OUStringToOString(rException.Message,
                    RTL_TEXTENCODING_UTF8).getStr());
        }
    }

    BitmapEx aThumbnail;
    if (xIStream.is())
    {
        ::std::auto_ptr<SvStream> pStream (
            ::utl::UcbStreamHelper::CreateStream (xIStream));
        ::vcl::PNGReader aReader (*pStream);
        aThumbnail = aReader.Read ();
    }
    return aThumbnail;
}




void MasterPageContainer::Implementation::FillContainer (void)
{
    //add a default master page
    if(mpDocument)
    {
        Image aPreview;
        String aEmpty;
        Token aResult = maContainer.size();
        USHORT nDefaultPageIndex = 1;
        SdPage* pLocalSlide = mpDocument->GetSdPage(nDefaultPageIndex, PK_STANDARD);
        SdPage* pLocalMasterPage = NULL;
        if (pLocalSlide != NULL && pLocalSlide->TRG_HasMasterPage())
            pLocalMasterPage = static_cast<SdPage*>(&pLocalSlide->TRG_GetMasterPage());

        if (pLocalMasterPage!=NULL)
        {
            // Assign the title,text layout to the slide so that its preview
            // is consistent with that of the tumbnails of the other master
            // pages.
            pLocalSlide->SetAutoLayout(AUTOLAYOUT_ENUM, TRUE);

            maContainer.push_back (MasterPageDescriptor (
                aEmpty/*sURL*/,
                pLocalMasterPage->GetName()/*sPageName*/,
                pLocalMasterPage->GetName()/*rStyleName*/,
                pLocalMasterPage,
                pLocalSlide,
                aPreview,
                aResult));
        }
    }

    //add all master pages from the template directories
    TemplateScanner aScanner;
    aScanner.Scan ();

    // Move all relevant entries to a local list.
    ::std::vector<TemplateDir*>::const_iterator iDirectory;
    ::std::vector<TemplateDir*>& aDirectories (aScanner.GetFolderList());
    for (iDirectory=aDirectories.begin();
         iDirectory!=aDirectories.end();
         ++iDirectory)
    {
        ::std::vector<TemplateEntry*>::iterator iEntry;
        ::std::vector<TemplateEntry*>& aEntries ((*iDirectory)->m_aEntries);
        for (iEntry=aEntries.begin(); iEntry!=aEntries.end(); ++iEntry)
        {
            if (FileFilter ((*iEntry)->m_aPath))
                AddTemplate ((*iEntry)->m_aPath, (*iEntry)->m_aTitle);
            delete *iEntry;
        }
        aEntries.clear();
    }
}




void MasterPageContainer::Implementation::AddTemplate (
    const String& rsPath,
    const String& rsTitle)
{
    PutMasterPage (
        rsPath,
        rsTitle,
        String(),
        NULL,
        Image());
}




bool MasterPageContainer::Implementation::FileFilter (const String& rsFileName)
{
    return rsFileName.SearchAscii ("presnt")!=STRING_NOTFOUND
        || rsFileName.SearchAscii ("layout")!=STRING_NOTFOUND;
}

} } } // end of namespace ::sd::toolpanel::controls
