/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuspell.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-12-14 17:04:36 $
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

#pragma hdrstop

#include "fuspell.hxx"

#include <svx/outliner.hxx>
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif

#include <sfx2/viewfrm.hxx>

#ifndef SD_FU_POOR_HXX
#include "fupoor.hxx"
#endif
#ifndef SD_OUTLINER_HXX
#include "Outliner.hxx"
#endif
#include "drawdoc.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_OUTLINE_VIEW_SHELL_HXX
#include "OutlineViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif

#include "app.hrc"

class SfxRequest;

namespace sd {

USHORT SidArraySpell[] = {
                SID_DRAWINGMODE,
                SID_OUTLINEMODE,
                SID_DIAMODE,
                SID_NOTESMODE,
                SID_HANDOUTMODE,
                0 };

TYPEINIT1( FuSpell, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuSpell::FuSpell (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDoc,
    SfxRequest& rReq )
    : FuPoor(pViewSh, pWin, pView, pDoc, rReq),
      pSdOutliner(NULL),
      bOwnOutliner(FALSE)
{
}

FunctionReference FuSpell::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq )
{
    FunctionReference xFunc( new FuSpell( pViewSh, pWin, pView, pDoc, rReq ) );
    xFunc->DoExecute(rReq);
    return xFunc;
}

void FuSpell::DoExecute( SfxRequest& rReq )
{
    pViewShell->GetViewFrame()->GetBindings().Invalidate( SidArraySpell );

    if ( pViewShell->ISA(DrawViewShell) )
    {
        bOwnOutliner = TRUE;
        pSdOutliner = new ::sd::Outliner( pDoc, OUTLINERMODE_TEXTOBJECT );
    }
    else if ( pViewShell->ISA(OutlineViewShell) )
    {
        bOwnOutliner = FALSE;
        pSdOutliner = pDoc->GetOutliner();
    }

    if (pSdOutliner)
       pSdOutliner->PrepareSpelling();
}



/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

FuSpell::~FuSpell()
{
    pDocSh->GetViewShell()->GetViewFrame()->GetBindings().Invalidate( SidArraySpell );

    if (pSdOutliner)
        pSdOutliner->EndSpelling();

    if (bOwnOutliner)
        delete pSdOutliner;
}

/*************************************************************************
|*
|* Pruefung starten
|*
\************************************************************************/

void FuSpell::StartSpelling()
{
    // Get current main view shell.
    ViewShellBase* pBase (ViewShellBase::GetViewShellBase (
        pDocSh->GetViewShell()->GetViewFrame()));
    if (pBase != NULL)
        pViewShell = pBase->GetMainViewShell ();
    else
        pViewShell = NULL;
    if (pViewShell != NULL)
    {
        if ( pSdOutliner && pViewShell->ISA(DrawViewShell) && !bOwnOutliner )
        {
            pSdOutliner->EndSpelling();

            bOwnOutliner = TRUE;
            pSdOutliner = new ::sd::Outliner( pDoc, OUTLINERMODE_TEXTOBJECT );
            pSdOutliner->PrepareSpelling();
        }
        else if ( pSdOutliner && pViewShell->ISA(OutlineViewShell) && bOwnOutliner )
        {
            pSdOutliner->EndSpelling();
            delete pSdOutliner;

            bOwnOutliner = FALSE;
            pSdOutliner = pDoc->GetOutliner();
            pSdOutliner->PrepareSpelling();
        }

        if (pSdOutliner)
            pSdOutliner->StartSpelling();
    }
}



} // end of namespace sd
