/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SpellDialogChildWindow.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:50:18 $
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

#include "SpellDialogChildWindow.hxx"
#include <svx/svxids.hrc>
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>

namespace sd{

SFX_IMPL_CHILDWINDOW(SpellDialogChildWindow, SID_SPELL_DIALOG)
}

#include "ViewShell.hxx"
#include "ViewShellBase.hxx"
#include "DrawViewShell.hxx"
#include "OutlineViewShell.hxx"
#ifndef SD_OUTLINER_HXX
#include <Outliner.hxx>
#endif
#include "drawdoc.hxx"


namespace sd {

SpellDialogChildWindow::SpellDialogChildWindow (
    ::Window* pParent,
    USHORT nId,
    SfxBindings* pBindings,
    SfxChildWinInfo* pInfo)
    : ::svx::SpellDialogChildWindow (pParent, nId, pBindings, pInfo),
      mbOwnOutliner (false),
      mpSdOutliner (NULL)
{
    ProvideOutliner();
}




SpellDialogChildWindow::~SpellDialogChildWindow (void)
{
    if (mpSdOutliner != NULL)
        mpSdOutliner->EndSpelling();

    if (mbOwnOutliner)
        delete mpSdOutliner;
}








SfxChildWinInfo SpellDialogChildWindow::GetInfo (void) const
{
    return ::svx::SpellDialogChildWindow::GetInfo();
}




void SpellDialogChildWindow::InvalidateSpellDialog (void)
{
    ::svx::SpellDialogChildWindow::InvalidateSpellDialog();
}




::svx::SpellPortions SpellDialogChildWindow::GetNextWrongSentence (void)
{
    ::svx::SpellPortions aResult;

    if (mpSdOutliner != NULL)
    {
        ProvideOutliner();
        aResult = mpSdOutliner->GetNextSpellSentence ();
    }

    // Close the spell check dialog when there are no more sentences to
    // check.
    if (aResult.size() == 0)
    {
        SfxBoolItem aItem (SID_SPELL_DIALOG, FALSE);
        GetBindings().GetDispatcher()->Execute(
            SID_SPELL_DIALOG,
            SFX_CALLMODE_ASYNCHRON,
            &aItem,
            0L);
    }

    return aResult;
}




void SpellDialogChildWindow::ApplyChangedSentence (
    const ::svx::SpellPortions& rChanged)
{
    if (mpSdOutliner != NULL)
    {
        EditView* pEditView = NULL;
        OutlinerView* pOutlinerView = mpSdOutliner->GetView(0);
        if (pOutlinerView != NULL)
            mpSdOutliner->ApplyChangedSentence (
                pOutlinerView->GetEditView(),
                rChanged);
    }
}




void SpellDialogChildWindow::GetFocus (void)
{
    // In order to detect a cursor movement we could compare the
    // currently selected text shape with the one that was selected
    // when LoseFocus() was called the last time.
    // For the time being we instead rely on the DetectChange() method
    // in the SdOutliner class.
}




void SpellDialogChildWindow::LoseFocus()
{
}




void SpellDialogChildWindow::ProvideOutliner (void)
{
    ViewShellBase* pViewShellBase = PTR_CAST (ViewShellBase, SfxViewShell::Current());

    if (pViewShellBase != NULL)
    {
        ViewShell* pViewShell = pViewShellBase->GetMainViewShell();
        // If there already exists an outliner that has been created
        // for another view shell then destroy it first.
        if (mpSdOutliner != NULL)
            if ((pViewShell->ISA(DrawViewShell) && ! mbOwnOutliner)
                || (pViewShell->ISA(OutlineViewShell) && mbOwnOutliner))
            {
                mpSdOutliner->EndSpelling();
                if (mbOwnOutliner)
                    delete mpSdOutliner;
                mpSdOutliner = NULL;
            }

        // Now create/get an outliner if none is present.
        if (mpSdOutliner == NULL)
        {
            if (pViewShell->ISA(DrawViewShell))
            {
                // We need an outliner for the spell check so we have
                // to create one.
                mbOwnOutliner = true;
                mpSdOutliner = new Outliner (
                    pViewShell->GetDoc(),
                    OUTLINERMODE_TEXTOBJECT);
            }
            else if (pViewShell->ISA(OutlineViewShell))
            {
                // An outline view is already visible. The SdOutliner
                // will use it instead of creating its own.
                mbOwnOutliner = false;
                mpSdOutliner = pViewShell->GetDoc()->GetOutliner();
            }

            // Initialize spelling.
            if (mpSdOutliner != NULL)
            {
                mpSdOutliner->PrepareSpelling();
                mpSdOutliner->StartSpelling();
            }
        }
    }
}



} // end of namespace ::sd
