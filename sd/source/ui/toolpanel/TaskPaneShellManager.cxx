/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TaskPaneShellManager.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:31:53 $
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

#include "TaskPaneShellManager.hxx"

#include "ViewShellManager.hxx"
#include <osl/diagnose.h>
#include <vcl/window.hxx>

#include <algorithm>

namespace sd { namespace toolpanel {

TaskPaneShellManager::TaskPaneShellManager (
    ViewShellManager& rViewShellManager,
    const ViewShell& rViewShell)
    : mrViewShellManager(rViewShellManager),
      mrViewShell(rViewShell),
      maSubShells()
{
}




TaskPaneShellManager::~TaskPaneShellManager (void)
{
    while ( ! maSubShells.empty())
        RemoveSubShell(maSubShells.begin()->second.mpShell);
}




SfxShell* TaskPaneShellManager::CreateShell (
    ShellId nId,
    ::Window* pParentWindow,
    FrameView* pFrameView)
{
    SubShells::const_iterator iShell (maSubShells.find(nId));
    if (iShell != maSubShells.end())
        return iShell->second.mpShell;
    else
        return NULL;
}




void TaskPaneShellManager::ReleaseShell (SfxShell* pShell)
{
    // Nothing to do.
}




void TaskPaneShellManager::AddSubShell (
    ShellId nId,
    SfxShell* pShell,
    ::Window* pWindow)
{
    if (pShell != NULL)
    {
        maSubShells[nId] = ShellDescriptor(pShell,pWindow);
        if (pWindow != NULL)
        {
            pWindow->AddEventListener(LINK(this,TaskPaneShellManager,WindowCallback));
            if (pWindow->IsReallyVisible())
                mrViewShellManager.ActivateSubShell(mrViewShell, nId);
        }
        else
            mrViewShellManager.ActivateSubShell(mrViewShell, nId);
    }
}




void TaskPaneShellManager::RemoveSubShell (const SfxShell* pShell)
{
    if (pShell != NULL)
    {
        SubShells::iterator iShell;
        for (iShell=maSubShells.begin(); iShell!=maSubShells.end(); ++iShell)
            if (iShell->second.mpShell == pShell)
            {
                if (iShell->second.mpWindow != NULL)
                    iShell->second.mpWindow->RemoveEventListener(
                        LINK(this,TaskPaneShellManager,WindowCallback));
                mrViewShellManager.DeactivateSubShell(mrViewShell,iShell->first);
                maSubShells.erase(iShell);
                break;
            }
    }
}




void TaskPaneShellManager::MoveToTop (SfxShell* pShell)
{
    SubShells::const_iterator iShell;
    for (iShell=maSubShells.begin(); iShell!=maSubShells.end(); ++iShell)
        if (iShell->second.mpShell == pShell)
        {
            ViewShellManager::UpdateLock aLocker (mrViewShellManager);
            mrViewShellManager.MoveSubShellToTop(mrViewShell,iShell->first);
            mrViewShellManager.MoveToTop(mrViewShell);
            break;
        }
}




IMPL_LINK(TaskPaneShellManager, WindowCallback, VclWindowEvent*, pEvent)
{
    if (pEvent != NULL)
    {
        SubShells::const_iterator iShell;
        ::Window* pWindow = pEvent->GetWindow();
        for (iShell=maSubShells.begin(); iShell!=maSubShells.end(); ++iShell)
            if (iShell->second.mpWindow == pWindow)
                break;
        if (iShell != maSubShells.end())
            switch (pEvent->GetId())
            {
                case VCLEVENT_WINDOW_SHOW:
                    mrViewShellManager.ActivateSubShell(mrViewShell,iShell->first);
                    break;

                case VCLEVENT_WINDOW_HIDE:
                    // Do not activate the sub shell.  This leads to
                    // problems with shapes currently being in text edit
                    // mode: Deactivating the shell leads to leaving the
                    // text editing mode.
                    // mrViewShellManager.DeactivateSubShell(mrViewShell,iShell->first);
                    break;
            }
    }

    return 0;
}


} } // end of namespace ::sd::toolpanel
