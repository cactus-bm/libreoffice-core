/*************************************************************************
 *
 *  $RCSfile: undopage.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 16:48:37 $
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

#pragma hdrstop

#include <svx/svxids.hrc>
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif

#include "sdpage.hxx"
#include "drviewsh.hxx"
#include "drawview.hxx"
#include "undopage.hxx"

#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif


TYPEINIT1(SdPageFormatUndoAction, SdUndoAction);
TYPEINIT1(SdPageLRUndoAction, SdUndoAction);
TYPEINIT1(SdPageULUndoAction, SdUndoAction);


/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

__EXPORT SdPageFormatUndoAction::~SdPageFormatUndoAction()
{
}

/*************************************************************************
|*
|* Undo()
|*
\************************************************************************/

void __EXPORT SdPageFormatUndoAction::Undo()
{
    Rectangle aOldBorderRect(nOldLeft, nOldUpper, nOldRight, nOldLower);
    pPage->ScaleObjects(aOldSize, aOldBorderRect, bNewScale);
    pPage->SetSize(aOldSize);
    pPage->SetLftBorder(nOldLeft);
    pPage->SetRgtBorder(nOldRight);
    pPage->SetUppBorder(nOldUpper);
    pPage->SetLwrBorder(nOldLower);
    pPage->SetOrientation(eOldOrientation);

    SfxViewShell* pViewShell = SfxViewShell::Current();

    if ( pViewShell->ISA(SdDrawViewShell) )
    {
        SdDrawViewShell* pDrViewShell = (SdDrawViewShell*) pViewShell;
        long nWidth = pPage->GetSize().Width();
        long nHeight = pPage->GetSize().Height();

        Point aPageOrg = Point(nWidth, nHeight / 2);
        Size aViewSize = Size(nWidth * 3, nHeight * 2);

        pDrViewShell->InitWindows(aPageOrg, aViewSize, Point(-1, -1), TRUE);
        pDrViewShell->GetView()->SetWorkArea(Rectangle(Point(0,0) - aPageOrg, aViewSize));

        pDrViewShell->UpdateScrollBars();
        pDrViewShell->GetView()->GetPageViewPvNum(0)->SetPageOrigin(Point(0,0));
        SFX_BINDINGS().Invalidate(SID_RULER_NULL_OFFSET);

        // auf (neue) Seitengroesse zoomen
        SFX_DISPATCHER().Execute(SID_SIZE_PAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
    }
}

/*************************************************************************
|*
|* Redo()
|*
\************************************************************************/

void __EXPORT SdPageFormatUndoAction::Redo()
{
    Rectangle aNewBorderRect(nNewLeft, nNewUpper, nNewRight, nNewLower);
    pPage->ScaleObjects(aNewSize, aNewBorderRect, bNewScale);
    pPage->SetSize(aNewSize);
    pPage->SetLftBorder(nNewLeft);
    pPage->SetRgtBorder(nNewRight);
    pPage->SetUppBorder(nNewUpper);
    pPage->SetLwrBorder(nNewLower);
    pPage->SetOrientation(eNewOrientation);

    SfxViewShell* pViewShell = SfxViewShell::Current();

    if ( pViewShell->ISA(SdDrawViewShell) )
    {
        SdDrawViewShell* pDrViewShell = (SdDrawViewShell*) pViewShell;
        long nWidth = pPage->GetSize().Width();
        long nHeight = pPage->GetSize().Height();

        Point aPageOrg = Point(nWidth, nHeight / 2);
        Size aViewSize = Size(nWidth * 3, nHeight * 2);

        pDrViewShell->InitWindows(aPageOrg, aViewSize, Point(-1, -1), TRUE);
        pDrViewShell->GetView()->SetWorkArea(Rectangle(Point(0,0) - aPageOrg, aViewSize));

        pDrViewShell->UpdateScrollBars();
        pDrViewShell->GetView()->GetPageViewPvNum(0)->SetPageOrigin(Point(0,0));
        SFX_BINDINGS().Invalidate(SID_RULER_NULL_OFFSET);

        // auf (neue) Seitengroesse zoomen
        SFX_DISPATCHER().Execute(SID_SIZE_PAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
    }
}

/*************************************************************************
|*
|* Repeat()
|*
\************************************************************************/

void __EXPORT SdPageFormatUndoAction::Repeat()
{
    Redo();
}

/*************************************************************************
|*
|* LR-Destruktor
|*
\************************************************************************/

__EXPORT SdPageLRUndoAction::~SdPageLRUndoAction()
{
}

/*************************************************************************
|*
|* LR-Undo()
|*
\************************************************************************/

void __EXPORT SdPageLRUndoAction::Undo()
{
    pPage->SetLftBorder(nOldLeft);
    pPage->SetRgtBorder(nOldRight);
}

/*************************************************************************
|*
|* LR-Redo()
|*
\************************************************************************/

void __EXPORT SdPageLRUndoAction::Redo()
{
    pPage->SetLftBorder(nNewLeft);
    pPage->SetRgtBorder(nNewRight);
}

/*************************************************************************
|*
|* LR-Repeat()
|*
\************************************************************************/

void __EXPORT SdPageLRUndoAction::Repeat()
{
    Redo();
}

/*************************************************************************
|*
|* UL-Destruktor
|*
\************************************************************************/

__EXPORT SdPageULUndoAction::~SdPageULUndoAction()
{
}

/*************************************************************************
|*
|* UL-Undo()
|*
\************************************************************************/

void __EXPORT SdPageULUndoAction::Undo()
{
    pPage->SetUppBorder(nOldUpper);
    pPage->SetLwrBorder(nOldLower);
}

/*************************************************************************
|*
|* UL-Redo()
|*
\************************************************************************/

void __EXPORT SdPageULUndoAction::Redo()
{
    pPage->SetUppBorder(nNewUpper);
    pPage->SetLwrBorder(nNewLower);
}

/*************************************************************************
|*
|* UL-Repeat()
|*
\************************************************************************/

void __EXPORT SdPageULUndoAction::Repeat()
{
    Redo();
}



