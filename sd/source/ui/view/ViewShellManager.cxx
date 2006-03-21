/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ViewShellManager.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:41:15 $
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

#include "ViewShellManager.hxx"

#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#include "Window.hxx"
#ifndef SD_DRAW_DOC_SHELL_HXX
#include "DrawDocShell.hxx"
#endif
#include "FormShellManager.hxx"

#include <sfx2/dispatch.hxx>
#include <svx/svxids.hrc>
#include <svx/fmshell.hxx>

#include <hash_map>

#undef VERBOSE

namespace sd {

namespace {

/** The ShellDescriptor template class is used to store view shells or sub
    shells together with their ids and the factory that was used to create
    the shell.  The shell pointer may be NULL.  In that case the shell is
    created on demand.
*/
template<class Shell>
class ShellDescriptor {
public:
    Shell* mpShell;
    ShellId mnId;
    ViewShellManager::SharedShellFactory mpFactory;
    ShellDescriptor () : mpShell(NULL), mnId(0), mpFactory() {}
    ShellDescriptor (Shell* pShell, ShellId nId) : mpShell(pShell), mnId(nId), mpFactory() {}
    ShellDescriptor (const ShellDescriptor& rDescriptor)
        : mpShell(rDescriptor.mpShell), mnId(rDescriptor.mnId), mpFactory(rDescriptor.mpFactory) {}
    ShellDescriptor& operator= (const ShellDescriptor& rDescriptor)
    { mpShell = rDescriptor.mpShell; mnId = rDescriptor.mnId; mpFactory = rDescriptor.mpFactory;
        return *this; }
};




/** This functor can be used to search for a shell in an STL container when the
    shell pointer is given.
*/
template<class Shell>
class IsShell : public ::std::unary_function<ShellDescriptor<Shell>,bool>
{
public:
    IsShell (const Shell* pShell) : mpShell(pShell) {}
    bool operator() (const ShellDescriptor<Shell>& rDescriptor)
    { return rDescriptor.mpShell == mpShell; }
private:
    const Shell* mpShell;
};




/** This functor can be used to search for a shell in an STL container when the
    id of the shell is given.
*/
template<class Shell>
class IsId : public ::std::unary_function<ShellDescriptor<Shell>,bool>
{
public:
    IsId (ShellId nId) : mnId(nId) {}
    bool operator() (const ShellDescriptor<Shell>& rDescriptor)
    { return rDescriptor.mnId == mnId; }
private:
    ShellId mnId;
};

} // end of anonymous namespace




class ViewShellManager::Implementation
{
public:
    Implementation (
        ViewShellManager& rManager,
        ViewShellBase& rBase);
    ~Implementation (void);

    void AddShellFactory (
        ViewShell* pViewShell,
        const SharedShellFactory& rpFactory);
    void RemoveShellFactory (
        ViewShell* pViewShell,
        const SharedShellFactory& rpFactory);
    ViewShell* ActivateViewShell (
        ShellId nId,
        ::Window* pParentWindow,
        FrameView* pFrameView);
    void DeactivateViewShell (const ViewShell& rShell);
    void SetFormShell (const ViewShell* pViewShell, FmFormShell* pFormShell, bool bAbove);
    void ActivateSubShell (const ViewShell& rParentShell, ShellId nId);
    void DeactivateSubShell (const ViewShell& rParentShell, ShellId nId);
    void DeactivateAllSubShells (const ViewShell& rViewShell);
    void MoveSubShellToTop (const ViewShell& rParentShell, ShellId nId);
    void MoveToTop (const ViewShell& rParentShell);
    SfxShell* GetShell (ShellId nId) const;
    SfxShell* GetTopShell (void) const;
    ShellId GetShellId (const SfxShell* pShell) const;
    void ReplaceUndoManager (SfxUndoManager* pManager, SfxUndoManager* pReplacement);
    void Shutdown (void);
    void InvalidateAllSubShells (const ViewShell* pParentShell);

    /** Remove all shells from the SFX stack above and including the given
        shell.
    */
    void TakeShellsFromStack (const SfxShell* pShell);

    class UpdateLock
    {
    public:
        UpdateLock (Implementation& rImpl) : mrImpl(rImpl) {mrImpl.LockUpdate();}
        ~UpdateLock (void) {mrImpl.UnlockUpdate();};
    private:
        Implementation& mrImpl;
    };



    /** Prevent updates of the shell stack.  While the sub shell manager is
        locked it will update its internal data structures but not alter the
        shell stack.  Use this method when there are several modifications
        to the shell stack to prevent multiple rebuilds of the shell stack
        and resulting broadcasts.
    */
    void LockUpdate (void);

    /** Allow updates of the shell stack.  This method has to be called the
        same number of times as LockUpdate() to really allow a rebuild of
        the shell stack.
    */
    void UnlockUpdate (void);

private:
    ViewShellBase& mrBase;
    mutable ::osl::Mutex maMutex;

    class ViewShellHash{public: size_t operator()(const ViewShell* p) const { return (size_t)p;} };
    typedef ::std::hash_multimap<const ViewShell*,SharedShellFactory,ViewShellHash>
        FactoryList;
    FactoryList maShellFactories;

    /** List of the active view shells.  In order to create gather all shells
        to put on the shell stack each view shell in this list is asked for
        its sub-shells (typically toolbars).
    */
    typedef ::std::list<ShellDescriptor<ViewShell> > ActiveShellList;
    ActiveShellList maActiveViewShells;

    typedef ::std::list<ShellDescriptor<SfxShell> > SubShellSubList;
    typedef ::std::hash_map<const ViewShell*,SubShellSubList,ViewShellHash> SubShellList;
    SubShellList maActiveSubShells;

    /** In this member we remember what shells we have pushed on the shell
        stack.
    */
    typedef ::std::vector<SfxShell*> ShellStack;

    int mnUpdateLockCount;

    /** When this flag is set then the main view shell is always kept at the
        top of the shell stack.
    */
    bool mbKeepMainViewShellOnTop;

    /** The UpdateShellStack() method can be called recursively.  This flag
        is used to communicate between different levels of invocation: if
        the stack has been updated in an inner call the outer call can (has
        to) stop and return immediately.
    */
    bool mbShellStackIsUpToDate;

    SfxShell* mpFormShell;
    const ViewShell* mpFormShellParent;
    bool mbFormShellAboveParent;

    SfxShell* mpTopShell;

    void GatherActiveShells (ShellStack& rShellList);

    void UpdateShellStack (void);

    void CreateShells (void);

    /** This method rebuilds the stack of shells that are stacked upon the
        view shell base.
    */
    void CreateTargetStack (ShellStack& rStack) const;

    DECL_LINK(WindowEventHandler, VclWindowEvent*);

    void DumpActiveShell (const ActiveShellList& rList);
    void DumpShellStack (const ShellStack& rStack);
    void DumpSfxShellStack (void);

    /** To be called before a shell is taken fom the SFX shell stack.  This
        method deactivates an active text editing to avoid problems with
        undo managers.
        Afterwards the Deactivate() of the shell is called.
    */
    void Deactivate (SfxShell* pShell);

    ShellDescriptor<ViewShell> CreateViewShell (
        ShellId nShellId,
        ::Window* pParentWindow,
        FrameView* pFrameView);
    ShellDescriptor<SfxShell> CreateSubShell (
        ViewShell* pViewShell,
        ShellId nShellId,
        ::Window* pParentWindow,
        FrameView* pFrameView);
    void DestroyViewShell (const ShellDescriptor<ViewShell>& rDescriptor);
    void DestroySubShell (
        const ViewShell& rViewShell,
        const ShellDescriptor<SfxShell>& rDescriptor);
};




//===== ViewShellManager ======================================================

ViewShellManager::ViewShellManager (ViewShellBase& rBase)
    : mpImpl(new Implementation(*this,rBase)),
      mbValid(true)
{
}




ViewShellManager::~ViewShellManager (void)
{
}




void ViewShellManager::SetViewShellFactory (const SharedShellFactory& rpFactory)
{
    if (mbValid)
        mpImpl->AddShellFactory(NULL, rpFactory);
}




void ViewShellManager::AddSubShellFactory (
    ViewShell* pViewShell,
    const SharedShellFactory& rpFactory)
{
    if (mbValid)
        mpImpl->AddShellFactory(pViewShell, rpFactory);
}




void ViewShellManager::RemoveSubShellFactory (
    ViewShell* pViewShell,
    const SharedShellFactory& rpFactory)
{
    if (mbValid)
        mpImpl->RemoveShellFactory(pViewShell, rpFactory);
}




ViewShell* ViewShellManager::ActivateViewShell (
    ShellId nShellId,
    ::Window* pParentWindow,
    FrameView* pFrameView)
{
    if (mbValid)
        return mpImpl->ActivateViewShell(nShellId,pParentWindow,pFrameView);
    else
        return NULL;
}




void ViewShellManager::DeactivateViewShell (const ViewShell* pShell)
{
    if (mbValid && pShell!=NULL)
        mpImpl->DeactivateViewShell(*pShell);
}




void ViewShellManager::MoveSubShellToTop (
    const ViewShell& rParentShell,
    ShellId nId)
{
    if (mbValid)
        mpImpl->MoveSubShellToTop(rParentShell, nId);
}




void ViewShellManager::SetFormShell (
    const ViewShell* pParentShell,
    FmFormShell* pFormShell,
    bool bAbove)
{
    if (mbValid)
        mpImpl->SetFormShell(pParentShell,pFormShell,bAbove);
}




void ViewShellManager::ActivateSubShell (const ViewShell& rViewShell, ShellId nId)
{
    if (mbValid)
        mpImpl->ActivateSubShell(rViewShell,nId);
}




void ViewShellManager::DeactivateSubShell (const ViewShell& rViewShell, ShellId nId)
{
    if (mbValid)
        mpImpl->DeactivateSubShell(rViewShell,nId);
}




void ViewShellManager::DeactivateAllSubShells (const ViewShell& rViewShell)
{
    if (mbValid)
        mpImpl->DeactivateAllSubShells(rViewShell);
}




void ViewShellManager::InvalidateAllSubShells (ViewShell* pViewShell)
{
    if (mbValid)
        mpImpl->InvalidateAllSubShells(pViewShell);
}




void ViewShellManager::InvalidateShellStack (const SfxShell* pShell)
{
    if (mbValid)
        mpImpl->TakeShellsFromStack(pShell);
}




void ViewShellManager::MoveToTop (const ViewShell& rParentShell)
{
    if (mbValid)
        mpImpl->MoveToTop(rParentShell);
}




SfxShell* ViewShellManager::GetShell (ShellId nId) const
{
    if (mbValid)
        return mpImpl->GetShell(nId);
    else
        return NULL;
}




SfxShell* ViewShellManager::GetTopShell (void) const
{
    if (mbValid)
        return mpImpl->GetTopShell();
    else
        return NULL;
}




ShellId ViewShellManager::GetShellId (const SfxShell* pShell) const
{
    if (mbValid)
        return mpImpl->GetShellId(pShell);
    else
        return snInvalidShellId;
}




void ViewShellManager::ReplaceUndoManager (SfxUndoManager* pManager, SfxUndoManager* pReplacement)
{
    if (mbValid)
        mpImpl->ReplaceUndoManager(pManager,pReplacement);
}




void ViewShellManager::Shutdown (void)
{
    if (mbValid)
    {
        mpImpl->Shutdown();
        mbValid = false;
    }
}



void ViewShellManager::LockUpdate (void)
{
    mpImpl->LockUpdate();
}




void ViewShellManager::UnlockUpdate (void)
{
    mpImpl->UnlockUpdate();
}




//===== ViewShellManager::Implementation ======================================

ViewShellManager::Implementation::Implementation (
    ViewShellManager& rManager,
    ViewShellBase& rBase)
    : mrBase(rBase),
      maMutex(),
      maShellFactories(),
      maActiveViewShells(),
      mnUpdateLockCount(0),
      mbKeepMainViewShellOnTop(false),
      mbShellStackIsUpToDate(true),
      mpFormShell(NULL),
      mpFormShellParent(NULL),
      mbFormShellAboveParent(true),
      mpTopShell(NULL)
{
}




ViewShellManager::Implementation::~Implementation (void)
{
    Shutdown();
}




void ViewShellManager::Implementation::AddShellFactory (
    ViewShell* pViewShell,
    const SharedShellFactory& rpFactory)
{
    bool bAlreadyAdded (false);

    // Check that the given factory has not already been added.
    ::std::pair<FactoryList::iterator,FactoryList::iterator> aRange(
        maShellFactories.equal_range(pViewShell));
    for (FactoryList::const_iterator iFactory=aRange.first; iFactory!=aRange.second; ++iFactory)
        if (iFactory->second == rpFactory)
        {
            bAlreadyAdded = true;
            break;
        }

    // Add the factory if it is not already present.
    if ( ! bAlreadyAdded)
        maShellFactories.insert(FactoryList::value_type(pViewShell, rpFactory));
}




void ViewShellManager::Implementation::RemoveShellFactory (
    ViewShell* pViewShell,
    const SharedShellFactory& rpFactory)
{
    ::std::pair<FactoryList::iterator,FactoryList::iterator> aRange(
        maShellFactories.equal_range(pViewShell));
    for (FactoryList::iterator iFactory=aRange.first; iFactory!=aRange.second; ++iFactory)
        if (iFactory->second == rpFactory)
        {
            maShellFactories.erase(iFactory);
            break;
        }
}




ViewShell* ViewShellManager::Implementation::ActivateViewShell (
    ShellId nShellId,
    ::Window* pParentWindow,
    FrameView* pFrameView)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Create a new shell or recycle on in the cache.
    ShellDescriptor<ViewShell> aDescriptor (CreateViewShell(nShellId, pParentWindow, pFrameView));

    // Put shell on top of the active view shells.
    if (aDescriptor.mpShell != NULL)
    {
        // Determine where to put the view shell on the stack.  By default
        // it is put on top of the stack.  When the view shell of the center
        // pane is to be kept top most and the new view shell is not
        // displayed in the center pane then it is inserted at the position
        // one below the top.
        ActiveShellList::iterator iInsertPosition (maActiveViewShells.begin());
        if (iInsertPosition != maActiveViewShells.end()
            && mbKeepMainViewShellOnTop
            && ! aDescriptor.mpShell->IsMainViewShell()
            && iInsertPosition->mpShell->IsMainViewShell())
        {
            ++iInsertPosition;
        }
        maActiveViewShells.insert(
            iInsertPosition,
            aDescriptor);
    }

    return aDescriptor.mpShell;
}




void ViewShellManager::Implementation::DeactivateViewShell (const ViewShell& rShell)
{
    ::osl::MutexGuard aGuard (maMutex);

    ActiveShellList::iterator iShell (::std::find_if (
        maActiveViewShells.begin(),
        maActiveViewShells.end(),
        IsShell<ViewShell>(&rShell)));
    if (iShell != maActiveViewShells.end())
    {
        UpdateLock aLocker (*this);

        ShellDescriptor<ViewShell> aDescriptor(*iShell);
        mrBase.GetDocShell()->Disconnect(aDescriptor.mpShell);
        maActiveViewShells.erase(iShell);
        TakeShellsFromStack(aDescriptor.mpShell);

        // Deactivate sub shells.
        SubShellList::iterator iList (maActiveSubShells.find(&rShell));
        if (iList != maActiveSubShells.end())
        {
            SubShellSubList& rList (iList->second);
            while ( ! rList.empty())
                DeactivateSubShell(rShell, rList.front().mnId);
        }

        DestroyViewShell(aDescriptor);
    }
}




void ViewShellManager::Implementation::ActivateSubShell (
    const ViewShell& rParentShell,
    ShellId nId)
{
    ::osl::MutexGuard aGuard (maMutex);

    do
    {
        // Check that the given view shell is active.
        ActiveShellList::iterator iShell (::std::find_if (
            maActiveViewShells.begin(),
            maActiveViewShells.end(),
            IsShell<ViewShell>(&rParentShell)));
        if (iShell == maActiveViewShells.end())
            break;

        // Create the sub shell list if it does not yet exist.
        SubShellList::iterator iList (maActiveSubShells.find(&rParentShell));
        if (iList == maActiveSubShells.end())
            iList = maActiveSubShells.insert(
                SubShellList::value_type(&rParentShell,SubShellSubList())).first;

        // Do not activate an object bar that is already active.  Requesting
        // this is not exactly an error but may be an indication of one.
        SubShellSubList& rList (iList->second);
        if (::std::find_if(rList.begin(),rList.end(), IsId<SfxShell>(nId)) != rList.end())
            break;

        // Add just the id of the sub shell. The actual shell is created
        // later in CreateShells().
        UpdateLock aLock (*this);
        rList.push_back(ShellDescriptor<SfxShell>(NULL, nId));
    }
    while (false);
}




void ViewShellManager::Implementation::DeactivateSubShell (
    const ViewShell& rParentShell,
    ShellId nId)
{
    ::osl::MutexGuard aGuard (maMutex);

    do
    {
        // Check that the given view shell is active.
        SubShellList::iterator iList (maActiveSubShells.find(&rParentShell));
        if (iList == maActiveSubShells.end())
            break;

        // Look up the sub shell.
        SubShellSubList& rList (iList->second);
        SubShellSubList::iterator iShell (
            ::std::find_if(rList.begin(),rList.end(), IsId<SfxShell>(nId)));
        if (iShell == rList.end())
            break;
        SfxShell* pShell = iShell->mpShell;
        if (pShell == NULL)
            break;

        UpdateLock aLock (*this);

        ShellDescriptor<SfxShell> aDescriptor(*iShell);
        // Remove the sub shell from both the internal structure as well as the
        // SFX shell stack above and including the sub shell.
        rList.erase(iShell);
        TakeShellsFromStack(pShell);

        DestroySubShell(rParentShell, aDescriptor);
    }
    while (false);
}




void ViewShellManager::Implementation::MoveSubShellToTop (
    const ViewShell& rParentShell,
    ShellId nId)
{
    SubShellList::iterator iList (maActiveSubShells.find(&rParentShell));
    if (iList != maActiveSubShells.end())
    {
        // Look up the sub shell.
        SubShellSubList& rList (iList->second);
        SubShellSubList::iterator iShell (
            ::std::find_if(rList.begin(),rList.end(), IsId<SfxShell>(nId)));
        if (iShell!=rList.end() && iShell!=rList.begin())
        {
            SubShellSubList::value_type aEntry (*iShell);
            rList.erase(iShell);
            rList.push_front(aEntry);
        }
    }
    else
    {
        // Ignore this call when there are no sub shells for the given
        // parent shell.  We could remember the sub shell to move to the top
        // but we do not.  Do call this method at a later time instead.
    }
}



void ViewShellManager::Implementation::MoveToTop (const ViewShell& rShell)
{
    ::osl::MutexGuard aGuard (maMutex);

    ActiveShellList::iterator iShell (::std::find_if (
        maActiveViewShells.begin(),
        maActiveViewShells.end(),
        IsShell<ViewShell>(&rShell)));
    bool bMove = true;
    if (iShell != maActiveViewShells.end())
    {
        // Is the shell already at the top of the stack?  We have to keep
        // the case in mind that mbKeepMainViewShellOnTop is true.  Shells
        // that are not the main view shell are placed on the second-to-top
        // position in this case.
            if (iShell == maActiveViewShells.begin()
            && (iShell->mpShell->IsMainViewShell() || ! mbKeepMainViewShellOnTop))
        {
            // The shell is at the top position and is either a) the main
            // view shell or b) another shell but the main view shell is not
            // kept at the top position.  We do not have to move the shell.
            bMove = false;
        }
        else if (iShell == ++maActiveViewShells.begin()
            && ! iShell->mpShell->IsMainViewShell()
            && mbKeepMainViewShellOnTop)
        {
            // The shell is a the second-to-top position, not the main view
            // shell and the main view shell is kept at the top position.
            // Therefore we do not have to move the shell.
            bMove = false;
        }
    }
    else
    {
        // The shell is not on the stack.  Therefore it can not be moved.
        // We could insert it but we don't.  Use ActivateViewShell() for
        // that.
        bMove = false;
    }

    // When the shell is not at the right position it is removed from the
    // internal list of shells and inserted at the correct position.
    if (bMove)
    {
        UpdateLock aLock (*this);

        ShellDescriptor<ViewShell> aDescriptor(*iShell);

        TakeShellsFromStack(&rShell);
        maActiveViewShells.erase(iShell);

        // Find out whether to insert at the top or one below.
        ActiveShellList::iterator aInsertPosition (maActiveViewShells.begin());
        if (mbKeepMainViewShellOnTop && ! aDescriptor.mpShell->IsMainViewShell())
        {
            if (maActiveViewShells.back().mpShell->IsMainViewShell())
                aInsertPosition++;
        }

        maActiveViewShells.insert(aInsertPosition, aDescriptor);
    }
}




SfxShell* ViewShellManager::Implementation::GetShell (ShellId nId) const
{
    ::osl::MutexGuard aGuard (maMutex);

    SfxShell* pShell = NULL;

    // First search the active view shells.
    ActiveShellList::const_iterator iShell (
        ::std::find_if (
        maActiveViewShells.begin(),
        maActiveViewShells.end(),
        IsId<ViewShell>(nId)));
    if (iShell != maActiveViewShells.end())
        pShell = iShell->mpShell;
    else
    {
        // Now search the active sub shells of every active view shell.
        SubShellList::const_iterator iList;
        for (iList=maActiveSubShells.begin(); iList!=maActiveSubShells.end(); ++iList)
        {
            const SubShellSubList& rList (iList->second);
            SubShellSubList::const_iterator iSubShell(
                ::std::find_if(rList.begin(),rList.end(), IsId<SfxShell>(nId)));
            if (iSubShell != rList.end())
            {
                pShell = iSubShell->mpShell;
                break;
            }
        }
    }

    return pShell;
}




SfxShell* ViewShellManager::Implementation::GetTopShell (void) const
{
    OSL_ASSERT(mpTopShell == mrBase.GetSubShell(0));
    return mpTopShell;
}




ShellId ViewShellManager::Implementation::GetShellId (const SfxShell* pShell) const
{
    ::osl::MutexGuard aGuard (maMutex);

    ShellId nId = snInvalidShellId;

    // First search the active view shells.
    const ViewShell* pViewShell = dynamic_cast<const ViewShell*>(pShell);
    if (pShell != NULL)
    {
        ActiveShellList::const_iterator iShell (
            ::std::find_if (
                maActiveViewShells.begin(),
                maActiveViewShells.end(),
                IsShell<ViewShell>(pViewShell)));
        if (iShell != maActiveViewShells.end())
            nId = iShell->mnId;
    }
    if (nId == snInvalidShellId)
    {
        // Now search the active sub shells of every active view shell.
        SubShellList::const_iterator iList;
        for (iList=maActiveSubShells.begin(); iList!=maActiveSubShells.end(); ++iList)
        {
            const SubShellSubList& rList (iList->second);
            SubShellSubList::const_iterator iSubShell(
                ::std::find_if(rList.begin(),rList.end(), IsShell<SfxShell>(pShell)));
            if (iSubShell != rList.end())
            {
                nId = iSubShell->mnId;
                break;
            }
        }
    }

    return nId;
}




void ViewShellManager::Implementation::ReplaceUndoManager (
    SfxUndoManager* pManager,
    SfxUndoManager* pReplacement)
{
    for (int i=0; ;++i)
    {
        SfxShell* pShell = mrBase.GetSubShell(i);
        if (pShell == NULL)
            break;
        if (pShell->GetUndoManager() == pManager)
            pShell->SetUndoManager(pReplacement);
    }
}






void ViewShellManager::Implementation::LockUpdate (void)
{
    mnUpdateLockCount++;
}




void ViewShellManager::Implementation::UnlockUpdate (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    mnUpdateLockCount--;
    if (mnUpdateLockCount < 0)
    {
        // This should not happen.
        OSL_ASSERT (mnUpdateLockCount>=0);
        mnUpdateLockCount = 0;
    }
    if (mnUpdateLockCount == 0)
        UpdateShellStack();
}




/** Update the SFX shell stack (the portion that is visible to us) so that
    it matches the internal shell stack.  This is done in six steps:
    1. Create the missing view shells and sub shells.
    2. Set up the internal shell stack.
    3. Get the SFX shell stack.
    4. Find the lowest shell in which the two stacks differ.
    5. Remove all shells above and including that shell from the SFX stack.
    6. Push all shells of the internal stack on the SFX shell stack that are
    not already present on the later.
*/
void ViewShellManager::Implementation::UpdateShellStack (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Remember the undo manager from the top-most shell on the stack.
    SfxShell* pTopMostShell = mrBase.GetSubShell(0);
    SfxUndoManager* pUndoManager = (pTopMostShell!=NULL)
        ? pTopMostShell->GetUndoManager()
        : NULL;

    // 1. Create the missing shells.
    CreateShells();


    // 2. Create the internal target stack.
    ShellStack aTargetStack;
    CreateTargetStack(aTargetStack);


    // 3. Get SFX shell stack.
    ShellStack aSfxShellStack;
    USHORT nIndex (0);
    while (mrBase.GetSubShell(nIndex)!=NULL)
        ++nIndex;
    aSfxShellStack.reserve(nIndex);
    while (nIndex-- > 0)
        aSfxShellStack.push_back(mrBase.GetSubShell(nIndex));


#ifdef VERBOSE
    OSL_TRACE("Current SFX Stack");
    DumpShellStack(aSfxShellStack);
    OSL_TRACE("Target Stack");
    DumpShellStack(aTargetStack);
#endif


    // 4. Find the lowest shell in which the two stacks differ.
    ShellStack::const_iterator iSfxShell (aSfxShellStack.begin());
    ShellStack::iterator iTargetShell (aTargetStack.begin());
    while (iSfxShell != aSfxShellStack.end()
        && iTargetShell!=aTargetStack.end()
        && (*iSfxShell)==(*iTargetShell))
    {
        ++iSfxShell;
        ++iTargetShell;
    }


    // 5. Remove all shells above and including the differing shell from the
    // SFX stack starting with the shell on top of the stack.
    while (iSfxShell != aSfxShellStack.end())
    {
        SfxShell* pShell = aSfxShellStack.back();
        aSfxShellStack.pop_back();
#ifdef VERBOSE
        OSL_TRACE("removing shell %p from stack", pShell);
#endif
        mrBase.RemoveSubShell(pShell);
    }


    // 6. Push shells from the given stack onto the SFX stack.
    mbShellStackIsUpToDate = false;
    while (iTargetShell != aTargetStack.end())
    {
#ifdef VERBOSE
        OSL_TRACE("pushing shell %p on stack", *iTargetShell);
#endif
        mrBase.AddSubShell(**iTargetShell);
        ++iTargetShell;

        // The pushing of the shell on to the shell stack may have lead to
        // another invocation of this method.  In this case we have to abort
        // pushing shells on the stack and return immediately.
        if (mbShellStackIsUpToDate)
            break;
    }
    if (mrBase.GetDispatcher() != NULL)
        mrBase.GetDispatcher()->Flush();

    // Update the pointer to the top-most shell and set its undo manager
    // to the one of the previous top-most shell.
    mpTopShell = mrBase.GetSubShell(0);
    if (mpTopShell!=NULL && pUndoManager!=NULL && mpTopShell->GetUndoManager()==NULL)
        mpTopShell->SetUndoManager(pUndoManager);

    // Finally tell an invocation of this method on a higher level that it can (has
    // to) abort and return immediately.
    mbShellStackIsUpToDate = true;

#ifdef VERBOSE
    OSL_TRACE("New current stack");
    DumpSfxShellStack();
#endif
}




void ViewShellManager::Implementation::TakeShellsFromStack (const SfxShell* pShell)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Remember the undo manager from the top-most shell on the stack.
    SfxShell* pTopMostShell = mrBase.GetSubShell(0);
    SfxUndoManager* pUndoManager = (pTopMostShell!=NULL)
        ? pTopMostShell->GetUndoManager()
        : NULL;

#ifdef VERBOSE
    OSL_TRACE("TakeShellsFromStack(%p)", pShell);
    DumpSfxShellStack();
#endif

    // 0.Make sure that the given shell is on the stack.  This is a
    // preparation for the following assertion.
    for (USHORT nIndex=0; true; nIndex++)
    {
        SfxShell* pShellOnStack = mrBase.GetSubShell(nIndex);
        if (pShellOnStack == NULL)
        {
            // Set pShell to NULL to indicate the following code that the
            // shell is not on the stack.
            pShell = NULL;
            break;
        }
        else if (pShellOnStack == pShell)
            break;
    }

    if (pShell != NULL)
    {
        // 1. Deactivate our shells on the stack before they are removed so
        // that during the Deactivation() calls the stack is still intact.
        for (USHORT nIndex=0; true; nIndex++)
        {
            SfxShell* pShellOnStack = mrBase.GetSubShell(nIndex);
            Deactivate(pShellOnStack);
            if (pShellOnStack == pShell)
                break;
        }

        // 2. Remove the shells from the stack.
        while (true)
        {
            SfxShell* pShellOnStack = mrBase.GetSubShell(0);
#ifdef VERBOSE
            OSL_TRACE("removing shell %p from stack", pShellOnStack);
#endif
            mrBase.RemoveSubShell(pShellOnStack);
            if (pShellOnStack == pShell)
                break;
        }

        // 3. Update the stack.
        if (mrBase.GetDispatcher() != NULL)
            mrBase.GetDispatcher()->Flush();

        // Update the pointer to the top-most shell and set its undo manager
        // to the one of the previous top-most shell.
        mpTopShell = mrBase.GetSubShell(0);
        if (mpTopShell!=NULL && pUndoManager!=NULL && mpTopShell->GetUndoManager()==NULL)
            mpTopShell->SetUndoManager(pUndoManager);
    }

#ifdef VERBOSE
    OSL_TRACE("Sfx shell stack is:");
    DumpSfxShellStack();
#endif
}




void ViewShellManager::Implementation::CreateShells (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Iterate over all view shells.
    ShellStack aShellStack;
    ActiveShellList::reverse_iterator iShell;
    for (iShell=maActiveViewShells.rbegin(); iShell!=maActiveViewShells.rend(); ++iShell)
    {
        // Get the list of associated sub shells.
        SubShellList::iterator iList (maActiveSubShells.find(iShell->mpShell));
        if (iList != maActiveSubShells.end())
        {
            SubShellSubList& rList (iList->second);

            // Iterate over all sub shells of the current view shell.
            SubShellSubList::iterator iSubShell;
            for (iSubShell=rList.begin(); iSubShell!=rList.end(); ++iSubShell)
            {
                if (iSubShell->mpShell == NULL)
                {
                    *iSubShell = CreateSubShell(iShell->mpShell,iSubShell->mnId,NULL,NULL);
                }
            }
        }
   }
}




void ViewShellManager::Implementation::CreateTargetStack (ShellStack& rStack) const
{
    // Create a local stack of the shells that are to push on the shell
    // stack.  We can thus safly create the required shells wile still
    // having a valid shell stack.
    for (ActiveShellList::const_reverse_iterator iViewShell (maActiveViewShells.rbegin());
         iViewShell != maActiveViewShells.rend();
         ++iViewShell)
    {
        // Possibly place the form shell below the current view shell.
        if ( ! mbFormShellAboveParent
            && mpFormShell!=NULL
            && iViewShell->mpShell==mpFormShellParent)
        {
            rStack.push_back(mpFormShell);
        }

        // Put the view shell itself on the local stack.
        rStack.push_back (iViewShell->mpShell);

        // Possibly place the form shell above the current view shell.
        if (mbFormShellAboveParent
            && mpFormShell!=NULL
            && iViewShell->mpShell==mpFormShellParent)
        {
            rStack.push_back(mpFormShell);
        }

        // Add all other sub shells.
        SubShellList::const_iterator iList (maActiveSubShells.find(iViewShell->mpShell));
        if (iList != maActiveSubShells.end())
        {
            const SubShellSubList& rList (iList->second);
            SubShellSubList::const_reverse_iterator iSubShell;
            for (iSubShell=rList.rbegin(); iSubShell!=rList.rend(); ++iSubShell)
                if (iSubShell->mpShell != mpFormShell)
                    rStack.push_back(iSubShell->mpShell);
        }
    }
}




IMPL_LINK(ViewShellManager::Implementation, WindowEventHandler, VclWindowEvent*, pEvent)
{
    if (pEvent != NULL)
    {
        ::Window* pEventWindow
            = static_cast<VclWindowEvent*>(pEvent)->GetWindow();

        switch (pEvent->GetId())
        {
            case VCLEVENT_WINDOW_GETFOCUS:
            {
                for (ActiveShellList::iterator aI(maActiveViewShells.begin());
                     aI!=maActiveViewShells.end();
                     aI++)
                {
                    if (pEventWindow == static_cast< ::Window*>(
                        aI->mpShell->GetActiveWindow()))
                    {
                        MoveToTop(*aI->mpShell);
                        break;
                    }
                }
            }
            break;

            case VCLEVENT_WINDOW_LOSEFOCUS:
                break;
        }
    }
    return TRUE;
}




ShellDescriptor<ViewShell> ViewShellManager::Implementation::CreateViewShell (
    ShellId nShellId,
    ::Window* pParentWindow,
    FrameView* pFrameView)
{
    ShellDescriptor<ViewShell> aResult;

    if (pParentWindow != NULL)
    {
        ShellDescriptor<SfxShell> aDescriptor (
            CreateSubShell(NULL,nShellId,pParentWindow,pFrameView));
        aResult.mpShell = dynamic_cast<ViewShell*>(aDescriptor.mpShell);
        aResult.mpFactory = aDescriptor.mpFactory;
        aResult.mnId = aDescriptor.mnId;

        // Register as window listener so that the shells of the current
        // window can be moved to the top of the shell stack.
        if (aResult.mpShell != NULL)
        {
            ::Window* pWindow = aResult.mpShell->GetActiveWindow();
            if (pWindow != NULL)
                pWindow->AddEventListener(
                    LINK(this, ViewShellManager::Implementation, WindowEventHandler));
            else
            {
                DBG_ASSERT (aResult.mpShell->GetActiveWindow()!=NULL,
                    "ViewShellManager::ActivateViewShell: "
                    "new view shell has no active window");
            }
        }
    }

    return aResult;
}




ShellDescriptor<SfxShell> ViewShellManager::Implementation::CreateSubShell (
    ViewShell* pParentShell,
    ShellId nShellId,
    ::Window* pParentWindow,
    FrameView* pFrameView)
{
    ::osl::MutexGuard aGuard (maMutex);
    ShellDescriptor<SfxShell> aResult;

    // Look up the factories for the parent shell.
    ::std::pair<FactoryList::iterator,FactoryList::iterator> aRange(
        maShellFactories.equal_range(pParentShell));

    // Try all factories to create the shell.
    for (FactoryList::const_iterator iFactory=aRange.first; iFactory!=aRange.second; ++iFactory)
    {
        SharedShellFactory pFactory = iFactory->second;
        if (pFactory != NULL)
            aResult.mpShell = pFactory->CreateShell(nShellId, pParentWindow, pFrameView);

        // Exit the loop when the shell has been successfully created.
        if (aResult.mpShell != NULL)
        {
            aResult.mpFactory = pFactory;
            aResult.mnId = nShellId;
            break;
        }
    }

    return aResult;
}




void ViewShellManager::Implementation::DestroyViewShell (
    const ShellDescriptor<ViewShell>& rDescriptor)
{
    OSL_ASSERT(rDescriptor.mpShell != NULL);

    rDescriptor.mpShell->GetActiveWindow()->RemoveEventListener(
        LINK(this, ViewShellManager::Implementation, WindowEventHandler));

    // Destroy the sub shell factories.
    ::std::pair<FactoryList::iterator,FactoryList::iterator> aRange(
        maShellFactories.equal_range(rDescriptor.mpShell));
    if (aRange.first != maShellFactories.end())
        maShellFactories.erase(aRange.first, aRange.second);

    // Release the shell.
    OSL_ASSERT(rDescriptor.mpFactory.get() != NULL);
    rDescriptor.mpFactory->ReleaseShell(rDescriptor.mpShell);
}




void ViewShellManager::Implementation::DestroySubShell (
    const ViewShell& rParentShell,
    const ShellDescriptor<SfxShell>& rDescriptor)
{
    OSL_ASSERT(rDescriptor.mpFactory.get() != NULL);
    rDescriptor.mpFactory->ReleaseShell(rDescriptor.mpShell);
}




void ViewShellManager::Implementation::InvalidateAllSubShells (const ViewShell* pParentShell)
{
    ::osl::MutexGuard aGuard (maMutex);

    SubShellList::iterator iList (maActiveSubShells.find(pParentShell));
    if (iList != maActiveSubShells.end())
    {
        SubShellSubList& rList (iList->second);
        SubShellSubList::iterator iShell;
        for (iShell=rList.begin(); iShell!=rList.end(); ++iShell)
            if (iShell->mpShell != NULL)
                iShell->mpShell->Invalidate();
    }
}




void ViewShellManager::Implementation::Shutdown (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    // Take stacked shells from stack.
    if ( ! maActiveViewShells.empty())
    {
        UpdateLock aLock (*this);

        while ( ! maActiveViewShells.empty())
        {
            DeactivateViewShell(*maActiveViewShells.front().mpShell);
        }
    }
    mrBase.RemoveSubShell (NULL);

    maShellFactories.clear();
}




void ViewShellManager::Implementation::DumpActiveShell (const ActiveShellList& rList)
{
    for (ActiveShellList::const_iterator aI=rList.begin(); aI!=rList.end(); ++aI)
        OSL_TRACE ("    %d %p", aI->mnId, aI->mpShell);
}




void ViewShellManager::Implementation::DumpShellStack (const ShellStack& rStack)
{
    ShellStack::const_reverse_iterator iEntry;
    for (iEntry=rStack.rbegin(); iEntry!=rStack.rend(); ++iEntry)
        if (*iEntry != NULL)
            OSL_TRACE ("    %p : %s",
                *iEntry,
                ::rtl::OUStringToOString((*iEntry)->GetName(),RTL_TEXTENCODING_UTF8).getStr());
        else
            OSL_TRACE("     null");
}




void ViewShellManager::Implementation::DumpSfxShellStack (void)
{
    ShellStack aSfxShellStack;
    USHORT nIndex (0);
    while (mrBase.GetSubShell(nIndex)!=NULL)
        ++nIndex;
    aSfxShellStack.reserve(nIndex);
    while (nIndex-- > 0)
        aSfxShellStack.push_back(mrBase.GetSubShell(nIndex));
    DumpShellStack(aSfxShellStack);
}




void ViewShellManager::Implementation::Deactivate (SfxShell* pShell)
{
    OSL_ASSERT(pShell!=NULL);

    // We have to end a text edit for view shells that are to be taken from
    // the shell stack.
    ViewShell* pViewShell = dynamic_cast<ViewShell*>(pShell);
    if (pViewShell != NULL)
    {
        sd::View* pView = pViewShell->GetView();
        if (pView!=NULL && pView->IsTextEdit())
        {
            pView->EndTextEdit();
            pView->UnmarkAll();
            pViewShell->GetViewFrame()->GetDispatcher()->Execute(
                SID_OBJECT_SELECT,
                SFX_CALLMODE_ASYNCHRON);
        }
    }

    // Now we can deactivate the shell.
    pShell->Deactivate(TRUE);
}




void ViewShellManager::Implementation::SetFormShell (
    const ViewShell* pFormShellParent,
    FmFormShell* pFormShell,
    bool bFormShellAboveParent)
{
    ::osl::MutexGuard aGuard (maMutex);

    mpFormShellParent = pFormShellParent;
    mpFormShell = pFormShell;
    mbFormShellAboveParent = bFormShellAboveParent;
}




void ViewShellManager::Implementation::DeactivateAllSubShells (
    const ViewShell& rViewShell)
{
    ::osl::MutexGuard aGuard (maMutex);

    SubShellList::iterator iList (maActiveSubShells.find(&rViewShell));
    if (iList != maActiveSubShells.end())
    {
        SubShellSubList& rList (iList->second);
        UpdateLock aLock (*this);
        while ( ! rList.empty())
            DeactivateSubShell(rViewShell, rList.front().mnId);
    }
}




} // end of namespace sd

