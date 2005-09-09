/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: FormShellManager.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:57:37 $
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

#include "FormShellManager.hxx"

#include "ViewShell.hxx"
#include "PaneManager.hxx"
#include "ViewShellBase.hxx"
#include "ObjectBarManager.hxx"
#include "Window.hxx"
#include <svx/fmshell.hxx>

namespace sd {


FormShellManager::FormShellManager (ViewShellBase& rBase)
    : mrBase(rBase),
      meStackPosition(SP_UNKNOWN)
{
    // Register at the PaneManager to be informed about changes in the
    // center pane.
    mrBase.GetPaneManager().AddEventListener (
        LINK(
            this,
            FormShellManager,
            PaneManagerEventHandler));

    RegisterAtCenterPane();
}




FormShellManager::~FormShellManager (void)
{
    UnregisterAtCenterPane();

    // Unregister at the PaneManager.
    mrBase.GetPaneManager().RemoveEventListener (
        LINK(
            this,
            FormShellManager,
            PaneManagerEventHandler));
}




void FormShellManager::RegisterAtCenterPane (void)
{
    do
    {
        ViewShell* pShell = mrBase.GetMainViewShell();
        if (pShell == NULL)
            break;

        ::Window* pWindow = pShell->GetActiveWindow();
        if (pWindow == NULL)
            break;

        // Register at the window to get informed when to move the form
        // shell to the bottom of the shell stack.
        pWindow->AddEventListener(
            LINK(
                this,
                FormShellManager,
                WindowEventHandler));

        // Register at the form shell to get informed when to move the shell
        // to the top of the shell stack.
        FmFormShell* pFormShell = static_cast<FmFormShell*>(
            pShell->GetObjectBarManager().GetObjectBar(RID_FORMLAYER_TOOLBOX));
        if (pFormShell == NULL)
            break;
        pFormShell->SetControlActivationHandler(
            LINK(
                this,
                FormShellManager,
                FormControlActivated));

        // Move the form shell to the correct position.
        if (meStackPosition == SP_ABOVE_VIEW_SHELL)
            pShell->GetObjectBarManager().MoveToTop(RID_FORMLAYER_TOOLBOX);
        else
            pShell->GetObjectBarManager().MoveBelowShell(RID_FORMLAYER_TOOLBOX);
    }
    while (false);
}




void FormShellManager::UnregisterAtCenterPane (void)
{
    do
    {
        ViewShell* pShell = mrBase.GetMainViewShell();
        if (pShell == NULL)
            break;

        ::Window* pWindow = pShell->GetActiveWindow();
        if (pWindow == NULL)
            break;

        // Unregister from the window.
        pWindow->RemoveEventListener(
            LINK(
                this,
                FormShellManager,
                WindowEventHandler));

        // Unregister form at the form shell.
        FmFormShell* pFormShell = static_cast<FmFormShell*>(
            pShell->GetObjectBarManager().GetObjectBar(RID_FORMLAYER_TOOLBOX));
        if (pFormShell == NULL)
            break;
        pFormShell->SetControlActivationHandler(Link());
    }
    while (false);
}




IMPL_LINK(FormShellManager, FormControlActivated, FmFormShell*, EMPTYARG)
{
    // The form shell has been actived.  To give it priority in reacting to
    // slot calls the form shell is moved to the top of the object bar shell
    // stack.
    ViewShell* pShell = mrBase.GetMainViewShell();
    if (pShell!=NULL && meStackPosition!=SP_ABOVE_VIEW_SHELL)
    {
        pShell->GetObjectBarManager().MoveToTop (RID_FORMLAYER_TOOLBOX);
        meStackPosition = SP_ABOVE_VIEW_SHELL;
    }

    return 0;
}




IMPL_LINK(FormShellManager, PaneManagerEventHandler, PaneManagerEvent*, pEvent)
{
    if (pEvent->mePane == PaneManager::PT_CENTER)
    {
        switch (pEvent->meEventId)
        {
            case PaneManagerEvent::EID_VIEW_SHELL_REMOVED:
                UnregisterAtCenterPane();
                break;

            case PaneManagerEvent::EID_VIEW_SHELL_ADDED:
                RegisterAtCenterPane();
                break;
        }
    }

    return 0;
}




IMPL_LINK(FormShellManager, WindowEventHandler, VclWindowEvent*, pEvent)
{
    if (pEvent != NULL)
    {
        ::Window* pEventWindow
            = static_cast<VclWindowEvent*>(pEvent)->GetWindow();

        switch (pEvent->GetId())
        {
            case VCLEVENT_WINDOW_GETFOCUS:
            {
                // The window of the center pane got the focus.  Therefore
                // the form shell is moved to the bottom of the object bar
                // stack.
                ViewShell* pShell = mrBase.GetMainViewShell();
                if (pShell!=NULL && meStackPosition!=SP_BELOW_VIEW_SHELL)
                {
                    pShell->GetObjectBarManager().MoveBelowShell (
                        RID_FORMLAYER_TOOLBOX);
                    meStackPosition = SP_BELOW_VIEW_SHELL;
                }
            }
            break;

            case VCLEVENT_WINDOW_LOSEFOCUS:
                // We follow the sloppy focus policy.  Losing the focus is
                // ignored.  We wait for the window gets the focus again or
                // the form shell is focused.  The later, however, is
                // notified over the FormControlActivated handler, not this
                // one.
                break;
        }
    }

    return 0;
}


} // end of namespace sd
