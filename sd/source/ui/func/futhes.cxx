/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: futhes.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 15:32:11 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sd.hxx"


#include "futhes.hxx"

#include <tools/pstm.hxx>
#include <svx/outliner.hxx>
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVDOBJ_HXX //autogen
#include <svx/svdobj.hxx>
#endif
#ifndef _SVDOTEXT_HXX //autogen
#include <svx/svdotext.hxx>
#endif

#ifndef _EEITEM_HXX
#include <svx/eeitem.hxx>
#endif

#include <svx/dialogs.hrc>
#include <svx/svxerr.hxx>
#include <svx/dialmgr.hxx>

#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#include "app.hrc"
#include "strings.hrc"
#include "drawdoc.hxx"
#include "app.hxx"
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#ifndef SD_OUTLINER_HXX
#include "Outliner.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_OUTLINE_VIEW_SHELL_HXX
#include "OutlineViewShell.hxx"
#endif
#ifndef SD_WINDOW_SHELL_HXX
#include "Window.hxx"
#endif
#include "sdresid.hxx"

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::linguistic2;

class SfxRequest;

namespace sd {

TYPEINIT1( FuThesaurus, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuThesaurus::FuThesaurus( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView,
                  SdDrawDocument* pDoc, SfxRequest& rReq )
       : FuPoor(pViewSh, pWin, pView, pDoc, rReq)
{
}

FunctionReference FuThesaurus::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq )
{
    FunctionReference xFunc( new FuThesaurus( pViewSh, pWin, pView, pDoc, rReq ) );
    xFunc->DoExecute(rReq);
    return xFunc;
}

void FuThesaurus::DoExecute( SfxRequest& )
{
    SfxErrorContext aContext(ERRCTX_SVX_LINGU_THESAURUS, String(),
                             mpWindow, RID_SVXERRCTX, &DIALOG_MGR() );

    if( mpViewShell && mpViewShell->ISA(DrawViewShell) )
    {
        SdrTextObj* pTextObj = NULL;

        if ( mpView->AreObjectsMarked() )
        {
            const SdrMarkList& rMarkList = mpView->GetMarkedObjectList();

            if ( rMarkList.GetMarkCount() == 1 )
            {
                SdrMark* pMark = rMarkList.GetMark(0);
                SdrObject* pObj = pMark->GetMarkedSdrObj();

                if ( pObj->ISA(SdrTextObj) )
                {
                    pTextObj = (SdrTextObj*) pObj;
                }
            }
        }

        ::Outliner* pOutliner = mpView->GetTextEditOutliner();
        const OutlinerView* pOutlView = mpView->GetTextEditOutlinerView();

        if ( pTextObj && pOutliner && pOutlView )
        {
            if ( !pOutliner->GetSpeller().is() )
            {
                Reference< XSpellChecker1 > xSpellChecker( LinguMgr::GetSpellChecker() );
                if ( xSpellChecker.is() )
                    pOutliner->SetSpeller( xSpellChecker );

                Reference< XHyphenator > xHyphenator( LinguMgr::GetHyphenator() );
                if( xHyphenator.is() )
                    pOutliner->SetHyphenator( xHyphenator );

                pOutliner->SetDefaultLanguage( mpDoc->GetLanguage( EE_CHAR_LANGUAGE ) );
            }

            EESpellState eState = ( (OutlinerView*) pOutlView)->StartThesaurus();
            DBG_ASSERT(eState != EE_SPELL_NOSPELLER, "No SpellChecker");

            if (eState == EE_SPELL_NOLANGUAGE)
            {
                ErrorBox(mpWindow, WB_OK, String(SdResId(STR_NOLANGUAGE))).Execute();
            }
        }
    }
    else if ( mpViewShell->ISA(OutlineViewShell) )
    {
        Outliner* pOutliner = mpDoc->GetOutliner();
        OutlinerView* pOutlView = pOutliner->GetView(0);

        if ( !pOutliner->GetSpeller().is() )
        {
            Reference< XSpellChecker1 > xSpellChecker( LinguMgr::GetSpellChecker() );
            if ( xSpellChecker.is() )
                pOutliner->SetSpeller( xSpellChecker );

            Reference< XHyphenator > xHyphenator( LinguMgr::GetHyphenator() );
            if( xHyphenator.is() )
                pOutliner->SetHyphenator( xHyphenator );

            pOutliner->SetDefaultLanguage( mpDoc->GetLanguage( EE_CHAR_LANGUAGE ) );
        }

        EESpellState eState = pOutlView->StartThesaurus();
        DBG_ASSERT(eState != EE_SPELL_NOSPELLER, "No SpellChecker");

        if (eState == EE_SPELL_NOLANGUAGE)
        {
            ErrorBox(mpWindow, WB_OK, String(SdResId(STR_NOLANGUAGE))).Execute();
        }
    }
}

} // end of namespace sd
