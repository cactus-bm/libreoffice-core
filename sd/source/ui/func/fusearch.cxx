/*************************************************************************
 *
 *  $RCSfile: fusearch.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-27 10:57:51 $
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

#define ITEMID_SEARCH           SID_SEARCH_ITEM
#include <svx/svxids.hrc>
#include <svx/srchitem.hxx>
#include <svx/srchdlg.hxx>
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif

#include "fupoor.hxx"
#include "fusearch.hxx"
#include "fuspell.hxx"  // wegen SidArraySpell[]
#include "sdwindow.hxx"
#include "drawdoc.hxx"
#include "app.hrc"
#include "app.hxx"
#include "sdview.hxx"
#include "sdoutl.hxx"
#include "drviewsh.hxx"
#include "outlnvsh.hxx"

class SdViewShell;
class SfxRequest;


TYPEINIT1( FuSearch, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuSearch::FuSearch( SdViewShell* pViewSh, SdWindow* pWin, SdView* pView,
                  SdDrawDocument* pDoc, SfxRequest& rReq )
       : FuPoor(pViewSh, pWin, pView, pDoc, rReq),
    pSdOutliner(NULL),
    bOwnOutliner(FALSE)
{
    pViewShell->GetViewFrame()->GetBindings().Invalidate( SidArraySpell );

    if ( pViewShell->ISA(SdDrawViewShell) )
    {
        bOwnOutliner = TRUE;
        pSdOutliner = new SdOutliner( pDoc, OUTLINERMODE_TEXTOBJECT );
    }
    else if ( pViewShell->ISA(SdOutlineViewShell) )
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

FuSearch::~FuSearch()
{
    if ( ! pDocSh->IsInDestruction())
        pDocSh->GetViewShell()->GetViewFrame()->GetBindings().Invalidate( SidArraySpell );

    if (pSdOutliner)
        pSdOutliner->EndSpelling();

    if (bOwnOutliner)
        delete pSdOutliner;
}


/*************************************************************************
|*
|* Suchen&Ersetzen
|*
\************************************************************************/

void FuSearch::SearchAndReplace( const SvxSearchItem* pSearchItem )
{
    pViewShell = PTR_CAST( SdViewShell, SfxViewShell::Current() );

    if( pViewShell )
    {
        if ( pSdOutliner && pViewShell->ISA(SdDrawViewShell) && !bOwnOutliner )
        {
            pSdOutliner->EndSpelling();

            bOwnOutliner = TRUE;
            pSdOutliner = new SdOutliner( pDoc, OUTLINERMODE_TEXTOBJECT );
            pSdOutliner->PrepareSpelling();
        }
        else if ( pSdOutliner && pViewShell->ISA(SdOutlineViewShell) && bOwnOutliner )
        {
            pSdOutliner->EndSpelling();
            delete pSdOutliner;

            bOwnOutliner = FALSE;
            pSdOutliner = pDoc->GetOutliner();
            pSdOutliner->PrepareSpelling();
        }

        if (pSdOutliner)
        {
            BOOL bEndSpelling = pSdOutliner->StartSearchAndReplace(pSearchItem);

            if (bEndSpelling)
            {
                pSdOutliner->EndSpelling();
                pSdOutliner->PrepareSpelling();
            }
        }
    }
}



