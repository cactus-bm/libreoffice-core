/*************************************************************************
 *
 *  $RCSfile: OutlinerIterator.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-01-20 12:39:07 $
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

#include "OutlinerIterator.hxx"
#include "OutlinerIteratorImpl.hxx"

#ifndef _SVDITER_HXX
#include <svx/svditer.hxx>
#endif
#ifndef _SFXDISPATCH_HXX
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif

#ifndef SD_OUTLINER_HXX
#include "Outliner.hxx"
#endif

#include "drawdoc.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#include "sdpage.hxx"
#ifndef SD_FRAME_VIEW
#include "FrameView.hxx"
#endif
#include "DrawDocShell.hxx"
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif

namespace sd { namespace outliner {


//===== IteratorPosition ======================================================

IteratorPosition::IteratorPosition (void)
    : mpObject(NULL),mnPageIndex(-1),//mpView(NULL),
      mePageKind(PK_STANDARD), meEditMode(EM_PAGE)
{}

IteratorPosition::IteratorPosition (const IteratorPosition& aPosition)
    : mpObject(aPosition.mpObject),
      mnPageIndex(aPosition.mnPageIndex),
      //      mpView(aPosition.mpView),
      mePageKind(aPosition.mePageKind),
      meEditMode(aPosition.meEditMode)
{}

IteratorPosition::IteratorPosition (SdrObject* pObject, sal_Int32 nPageIndex,// ::sd::View* pView,
    PageKind ePageKind, EditMode eEditMode)
    : mpObject(pObject),
      mnPageIndex(nPageIndex),
      //      mpView(pView),
      mePageKind(ePageKind),
      meEditMode(eEditMode)
{}

IteratorPosition::~IteratorPosition (void)
{}

IteratorPosition& IteratorPosition::operator= (const IteratorPosition& aPosition)
{
    mpObject = aPosition.mpObject;
    mnPageIndex = aPosition.mnPageIndex;
    //    mpView = aPosition.mpView;
    mePageKind = aPosition.mePageKind;
    meEditMode = aPosition.meEditMode;
    return *this;
}

bool IteratorPosition::operator== (const IteratorPosition& aPosition) const
{
    return mpObject == aPosition.mpObject
        && mnPageIndex == aPosition.mnPageIndex
        //        && mpView == aPosition.mpView
        && mePageKind == aPosition.mePageKind
        && meEditMode == aPosition.meEditMode;
}




//===== Iterator ==============================================================

Iterator::Iterator (void)
{
    mpIterator = NULL;
}

Iterator::Iterator (IteratorImplBase* pObject)
{
    mpIterator = pObject;
}

Iterator::Iterator (const IteratorImplBase& rObject)
{
    mpIterator = rObject.Clone();
}

Iterator::~Iterator (void)
{
    delete mpIterator;
}

Iterator& Iterator::operator= (const Iterator& rIterator)
{
    if (this != &rIterator)
    {
        delete mpIterator;
        if (rIterator.mpIterator != NULL)
            mpIterator = rIterator.mpIterator->Clone();
        else
            mpIterator = NULL;
    }
    return *this;
}

const IteratorPosition& Iterator::operator* () const
{
    DBG_ASSERT (mpIterator!=NULL, "::sd::outliner::Iterator::operator* : missing implementation object");
    return mpIterator->GetPosition();
}

Iterator& Iterator::operator++ ()
{
    if (mpIterator!=NULL)
        mpIterator->GotoNextObject();
    return *this;
}

Iterator Iterator::operator++ (int)
{
    Iterator aTmp = *this;
    if (mpIterator!=NULL)
        mpIterator->GotoNextObject();
    return aTmp;
}

bool Iterator::operator== (const Iterator& rIterator)
{
    if (mpIterator == NULL || rIterator.mpIterator==NULL)
        return mpIterator == rIterator.mpIterator;
    else
        return *mpIterator == *rIterator.mpIterator;
}

bool Iterator::operator!= (const Iterator& rIterator)
{
    return ! operator==(rIterator);
}




void Iterator::Reverse (void)
{
    if (mpIterator != NULL)
        mpIterator->Reverse();
}




//===== IteratorFactory =======================================================

OutlinerContainer::OutlinerContainer (Outliner* pOutliner)
    : mpOutliner(pOutliner)
{}

Iterator OutlinerContainer::begin (void)
{
    return CreateIterator (BEGIN);
}

Iterator OutlinerContainer::end (void)
{
    return CreateIterator (END);
}

Iterator OutlinerContainer::current (void)
{
    return CreateIterator (CURRENT);
}



Iterator OutlinerContainer::CreateIterator (IteratorLocation aLocation)
{
    // Decide on certain features of the outliner which kind of iterator to
    // use.
    if (mpOutliner->mbRestrictSearchToSelection)
        // There is a selection.  Search only in this.
        return CreateSelectionIterator (
            mpOutliner->maMarkListCopy,
            mpOutliner->mpDrawDocument,
            static_cast<DrawViewShell*>(mpOutliner->mpViewShell),
            mpOutliner->mbDirectionIsForward,
            aLocation);
    else
        // Search in the whole document.
        return CreateDocumentIterator (
            mpOutliner->mpDrawDocument,
            static_cast<DrawViewShell*>(mpOutliner->mpViewShell),
            mpOutliner->mbDirectionIsForward,
            aLocation);
}




Iterator OutlinerContainer::CreateSelectionIterator (
    const ::std::vector<SdrObject*>& rObjectList,
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward,
    IteratorLocation aLocation)
{
    sal_Int32 nObjectIndex;

    if (bDirectionIsForward)
        switch (aLocation)
        {
            case CURRENT:
            case BEGIN:
            default:
                nObjectIndex = 0;
                break;
            case END:
                nObjectIndex = rObjectList.size();
                break;
        }
    else
        switch (aLocation)
        {
            case CURRENT:
            case BEGIN:
            default:
                nObjectIndex = rObjectList.size()-1;
                break;
            case END:
                nObjectIndex = -1;
                break;
        }

    return Iterator (new SelectionIteratorImpl (
        rObjectList, nObjectIndex, pDocument, pViewShell, bDirectionIsForward));
}

Iterator OutlinerContainer::CreateViewIterator (
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward,
    IteratorLocation aLocation)
{
    sal_Int32 nPageIndex = GetPageIndex (pDocument, pViewShell,
        pViewShell->GetPageKind(), pViewShell->GetEditMode(),
        bDirectionIsForward, aLocation);

    return Iterator (new ViewIteratorImpl (
        nPageIndex, pDocument, pViewShell, bDirectionIsForward));
}

Iterator OutlinerContainer::CreateDocumentIterator (
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward,
    IteratorLocation aLocation)
{
    PageKind ePageKind;
    EditMode eEditMode;

    if (bDirectionIsForward)
        switch (aLocation)
        {
            case BEGIN:
            default:
                ePageKind = PK_STANDARD;
                eEditMode = EM_PAGE;
                break;
            case END:
                ePageKind = PK_HANDOUT;
                eEditMode = EM_MASTERPAGE;
                break;
            case CURRENT:
                ePageKind = pViewShell->GetPageKind();
                eEditMode = pViewShell->GetEditMode();
                break;
        }
    else
        switch (aLocation)
        {
            case BEGIN:
            default:
                ePageKind = PK_HANDOUT;
                eEditMode = EM_MASTERPAGE;
                break;
            case END:
                ePageKind = PK_STANDARD;
                eEditMode = EM_PAGE;
                break;
            case CURRENT:
                ePageKind = pViewShell->GetPageKind();
                eEditMode = pViewShell->GetEditMode();
                break;
        }

    sal_Int32 nPageIndex = GetPageIndex (pDocument, pViewShell,
        ePageKind, eEditMode, bDirectionIsForward, aLocation);

    return Iterator (
        new DocumentIteratorImpl (nPageIndex, ePageKind, eEditMode,
            pDocument, pViewShell, bDirectionIsForward));
}

sal_Int32 OutlinerContainer::GetPageIndex (
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    PageKind ePageKind,
    EditMode eEditMode,
    bool bDirectionIsForward,
    IteratorLocation aLocation)
{
    sal_Int32 nPageIndex;
    sal_Int32 nPageCount;

    switch (eEditMode)
    {
        case EM_PAGE:
            nPageCount = pDocument->GetSdPageCount (ePageKind);
            break;
        case EM_MASTERPAGE:
            nPageCount = pDocument->GetMasterSdPageCount(ePageKind);
            break;
        default:
            nPageCount = 0;
    }

    if (bDirectionIsForward)
        switch (aLocation)
        {
            case CURRENT:
                nPageIndex = pViewShell->GetCurPageId() - 1;
                break;
            case BEGIN:
            default:
                nPageIndex = 0;
                break;
            case END:
                nPageIndex = nPageCount;
                break;
        }
    else
        switch (aLocation)
        {
            case CURRENT:
                nPageIndex = pViewShell->GetCurPageId() - 1;
                break;
            case BEGIN:
            default:
                nPageIndex = nPageCount-1;
                break;
            case END:
                nPageIndex = -1;
                break;
        }

    return nPageIndex;
}




//===== IteratorImplBase ====================================================

IteratorImplBase::IteratorImplBase (
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward)
    : mpDocument (pDocument),
      mpViewShell (pViewShell),
      maPosition(),
      mbDirectionIsForward (bDirectionIsForward)
{
    maPosition.mePageKind = pViewShell->GetPageKind();
    maPosition.meEditMode = pViewShell->GetEditMode();
}

IteratorImplBase::IteratorImplBase (
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward,
    PageKind ePageKind,
    EditMode eEditMode)
    : mpDocument (pDocument),
      mpViewShell (pViewShell),
      maPosition(),
      mbDirectionIsForward (bDirectionIsForward)
{
    maPosition.mePageKind = ePageKind;
    maPosition.meEditMode = eEditMode;
}

IteratorImplBase::~IteratorImplBase (void)
{}

bool IteratorImplBase::operator== (const IteratorImplBase& rIterator) const
{
    return maPosition == rIterator.maPosition;
}

bool IteratorImplBase::IsEqual (const IteratorImplBase& rIterator, IteratorType aType) const
{
    // When this method is executed instead of the ones from derived classes
    // then the argument is of another type then the object itself.  In this
    // just compare the position objects.
    return maPosition == rIterator.maPosition;
}

const IteratorPosition& IteratorImplBase::GetPosition (void)
{
    return maPosition;
}




IteratorImplBase* IteratorImplBase::Clone (IteratorImplBase* pObject) const
{
    if (pObject != NULL)
    {
        pObject->maPosition = maPosition;
        pObject->mpDocument = mpDocument;
        pObject->mpViewShell = mpViewShell;
        pObject->mbDirectionIsForward = mbDirectionIsForward;
    }
    return pObject;
}



void IteratorImplBase::Reverse (void)
{
    mbDirectionIsForward = ! mbDirectionIsForward;
}



//===== SelectionIteratorImpl ===========================================

SelectionIteratorImpl::SelectionIteratorImpl (
    const ::std::vector<SdrObject*>& rObjectList,
    sal_Int32 nObjectIndex,
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward)
    : IteratorImplBase (pDocument, pViewShell, bDirectionIsForward),
      mrObjectList(rObjectList),
      mnObjectIndex(nObjectIndex)
{}

SelectionIteratorImpl::~SelectionIteratorImpl (void)
{}

IteratorImplBase* SelectionIteratorImpl::Clone (IteratorImplBase* pObject) const
{
    SelectionIteratorImpl* pIterator = static_cast<SelectionIteratorImpl*>(pObject);
    if (pIterator == NULL)
        pIterator = new SelectionIteratorImpl (
            mrObjectList, mnObjectIndex, mpDocument, mpViewShell, mbDirectionIsForward);
    return pIterator;
}


void SelectionIteratorImpl::GotoNextObject (void)
{
    if (mbDirectionIsForward)
        ++mnObjectIndex;
    else
        --mnObjectIndex;
}


const IteratorPosition& SelectionIteratorImpl::GetPosition (void)
{
    maPosition.mpObject = mrObjectList.at(mnObjectIndex);

    return maPosition;
}


bool SelectionIteratorImpl::operator== (const IteratorImplBase& rIterator) const
{
    return rIterator.IsEqual (*this, SELECTION);
}


bool SelectionIteratorImpl::IsEqual (
    const IteratorImplBase& rIterator,
    IteratorType aType) const
{
    if (aType == SELECTION)
    {
        const SelectionIteratorImpl* pSelectionIterator =
            static_cast<const SelectionIteratorImpl*>(&rIterator);
        return mpDocument == pSelectionIterator->mpDocument
            && mnObjectIndex == pSelectionIterator->mnObjectIndex;
    }
    else
        return false;
}




//===== ViewIteratorImpl ================================================

ViewIteratorImpl::ViewIteratorImpl (
    sal_Int32 nPageIndex,
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward)
    : IteratorImplBase (pDocument, pViewShell, bDirectionIsForward),
      mbPageChangeOccured(false),
      mpPage(NULL),
      mpObjectIterator(NULL)
{
    SetPage (nPageIndex);
}




ViewIteratorImpl::ViewIteratorImpl (
    sal_Int32 nPageIndex,
    SdDrawDocument* pDocument,
    DrawViewShell* pViewShell,
    bool bDirectionIsForward,
    PageKind ePageKind,
    EditMode eEditMode)
    : IteratorImplBase (pDocument, pViewShell, bDirectionIsForward, ePageKind, eEditMode),
      mbPageChangeOccured(false),
      mpPage(NULL),
      mpObjectIterator(NULL)
{
    SetPage (nPageIndex);
}




ViewIteratorImpl::~ViewIteratorImpl (void)
{
}




IteratorImplBase* ViewIteratorImpl::Clone (IteratorImplBase* pObject) const
{

    ViewIteratorImpl* pIterator = static_cast<ViewIteratorImpl*>(pObject);
    if (pIterator == NULL)
        pIterator = new ViewIteratorImpl (
            maPosition.mnPageIndex, mpDocument, mpViewShell, mbDirectionIsForward);

    IteratorImplBase::Clone (pObject);

    if (mpObjectIterator != NULL)
    {
        pIterator->mpObjectIterator = new SdrObjListIter(
            *mpPage, IM_DEEPNOGROUPS, !mbDirectionIsForward);

        // No direct way to set the object iterator to the current object.
        pIterator->maPosition.mpObject = NULL;
        while (pIterator->mpObjectIterator->IsMore()
            && pIterator->maPosition.mpObject!=maPosition.mpObject)
            pIterator->maPosition.mpObject = pIterator->mpObjectIterator->Next();
    }
    else
        pIterator->mpObjectIterator = NULL;

    return pIterator;
}




void ViewIteratorImpl::GotoNextObject (void)
{
    if (mpObjectIterator != NULL && mpObjectIterator->IsMore())
        maPosition.mpObject = mpObjectIterator->Next();
    else
        maPosition.mpObject = NULL;

    if (maPosition.mpObject == NULL)
    {
        if (mbDirectionIsForward)
            SetPage (maPosition.mnPageIndex+1);
        else
            SetPage (maPosition.mnPageIndex-1);

        if (mpPage != NULL)
            mpObjectIterator = new SdrObjListIter(*mpPage, IM_DEEPNOGROUPS, !mbDirectionIsForward);
        if (mpObjectIterator!=NULL && mpObjectIterator->IsMore())
            maPosition.mpObject = mpObjectIterator->Next();
        else
            maPosition.mpObject = NULL;
    }
}




void ViewIteratorImpl::SetPage (sal_Int32 nPageIndex)
{
    mbPageChangeOccured = (maPosition.mnPageIndex!=nPageIndex);
    if (mbPageChangeOccured)
    {
        maPosition.mnPageIndex = nPageIndex;

        // Get page pointer.
        if (nPageIndex >= 0)
        {
            if (maPosition.meEditMode == EM_PAGE)
                mpPage = mpDocument->GetSdPage ((USHORT)nPageIndex, maPosition.mePageKind);
            else
                mpPage = mpDocument->GetMasterSdPage ((USHORT)nPageIndex, maPosition.mePageKind);
        }
        else
            mpPage = NULL;
    }

    // Set up object list iterator.
    if (mpPage != NULL)
        mpObjectIterator = new SdrObjListIter(*mpPage, IM_DEEPNOGROUPS, ! mbDirectionIsForward);
    else
        mpObjectIterator = NULL;

    // Get object pointer.
    if (mpObjectIterator!=NULL && mpObjectIterator->IsMore())
        maPosition.mpObject = mpObjectIterator->Next();
    else
        maPosition.mpObject = NULL;
}




void ViewIteratorImpl::Reverse (void)
{
    IteratorImplBase::Reverse ();

    // Create reversed object list iterator.
    if (mpObjectIterator != NULL)
        delete mpObjectIterator;
    if (mpPage != NULL)
        mpObjectIterator = new SdrObjListIter(*mpPage, IM_DEEPNOGROUPS, ! mbDirectionIsForward);
    else
        mpObjectIterator = NULL;

    // Move iterator to the current object.
    SdrObject* pObject = maPosition.mpObject;
    maPosition.mpObject = NULL;
    while (mpObjectIterator->IsMore() && maPosition.mpObject!=pObject)
        maPosition.mpObject = mpObjectIterator->Next();
}




//===== DocumentIteratorImpl ============================================

DocumentIteratorImpl::DocumentIteratorImpl (
    sal_Int32 nPageIndex,
    PageKind ePageKind, EditMode eEditMode,
    SdDrawDocument* pDocument, DrawViewShell* pViewShell,
    bool bDirectionIsForward)
    : ViewIteratorImpl (nPageIndex, pDocument, pViewShell, bDirectionIsForward,
        ePageKind, eEditMode)
{
    if (eEditMode == EM_PAGE)
        mnPageCount = pDocument->GetSdPageCount (ePageKind);
    else
        mnPageCount = pDocument->GetMasterSdPageCount(ePageKind);
}




DocumentIteratorImpl::~DocumentIteratorImpl (void)
{}




IteratorImplBase* DocumentIteratorImpl::Clone (IteratorImplBase* pObject) const
{
    DocumentIteratorImpl* pIterator = static_cast<DocumentIteratorImpl*>(pObject);
    if (pIterator == NULL)
        pIterator = new DocumentIteratorImpl (
            maPosition.mnPageIndex, maPosition.mePageKind, maPosition.meEditMode,
            mpDocument, mpViewShell, mbDirectionIsForward);
    // Finish the cloning.
    return ViewIteratorImpl::Clone (pIterator);
}




void DocumentIteratorImpl::GotoNextObject (void)
{
    bool bSetToOnePastLastPage = false;
    bool bViewChanged = false;

    ViewIteratorImpl::GotoNextObject();

    if (mbDirectionIsForward)
    {
        if (maPosition.mnPageIndex >= mnPageCount)
        {
            // Switch to master page.
            if (maPosition.meEditMode == EM_PAGE)
            {
                maPosition.meEditMode = EM_MASTERPAGE;
                SetPage (0);
            }

            // Switch to next view mode.
            else
            {
                if (maPosition.mePageKind == PK_HANDOUT)
                    // Not really necessary but makes things more clear.
                    bSetToOnePastLastPage = true;
                else
                {
                    maPosition.meEditMode = EM_PAGE;
                    if (maPosition.mePageKind == PK_STANDARD)
                        maPosition.mePageKind = PK_NOTES;
                    else if (maPosition.mePageKind == PK_NOTES)
                        maPosition.mePageKind = PK_HANDOUT;
                    SetPage (0);
                }
            }
            bViewChanged = true;
        }
    }
    else
        if (maPosition.mnPageIndex < 0)
        {
            // Switch from master pages to draw pages.
            if (maPosition.meEditMode == EM_MASTERPAGE)
            {
                maPosition.meEditMode = EM_PAGE;
                bSetToOnePastLastPage = true;
            }

            // Switch to previous view mode.
            else
            {
                if (maPosition.mePageKind == PK_STANDARD)
                    SetPage (-1);
                else
                {
                    maPosition.meEditMode = EM_MASTERPAGE;
                    if (maPosition.mePageKind == PK_HANDOUT)
                        maPosition.mePageKind = PK_NOTES;
                    else if (maPosition.mePageKind == PK_NOTES)
                        maPosition.mePageKind = PK_STANDARD;
                    bSetToOnePastLastPage = true;
                }
            }
            bViewChanged = true;
        }

    if (bViewChanged)
    {
        // Get new page count;
        sal_Int32 nPageCount;
        if (maPosition.meEditMode == EM_PAGE)
            nPageCount = mpDocument->GetSdPageCount (maPosition.mePageKind);
        else
            nPageCount = mpDocument->GetMasterSdPageCount(maPosition.mePageKind);

        // Now that we know the number of pages we can set the current page index.
        if (bSetToOnePastLastPage)
            SetPage (nPageCount);
    }
}


} } // end of namespace ::sd::outliner
