/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drbezob.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: kz $ $Date: 2006-12-12 19:10:41 $
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

#include "BezierObjectBar.hxx"

#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#ifndef _SFXMSG_HXX
#include <sfx2/msg.hxx>
#endif

#include <sfx2/viewfrm.hxx>
#include <sfx2/objface.hxx>

#ifndef _SVXIDS_HRC
#include <svx/svxids.hrc>
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVDOPATH_HXX //autogen
#include <svx/svdopath.hxx>
#endif
#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif
#ifndef _SVDUNDO_HXX //autogen
#include <svx/svdundo.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif


#ifndef SD_RESID_HXX
#include "sdresid.hxx"
#endif



#include "res_bmp.hrc"
#include "glob.hrc"
#include "strings.hrc"

#include "DrawDocShell.hxx"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#include "drawdoc.hxx"
#include "sdresid.hxx"
#ifndef SD_FU_SELECTION_HXX
#include "fusel.hxx"
#endif
#ifndef SD_FU_CONSTRUCT_BEZIER_HXX
#include "fuconbez.hxx"
#endif


using namespace sd;
#define BezierObjectBar
#include "sdslots.hxx"

namespace sd {

/*************************************************************************
|*
|* Standardinterface deklarieren (Die Slotmap darf nicht leer sein)
|*
\************************************************************************/


SFX_DECL_TYPE(13);


SFX_IMPL_INTERFACE(BezierObjectBar, ::SfxShell, SdResId(STR_BEZIEROBJECTBARSHELL))
{
}

TYPEINIT1(BezierObjectBar, ::SfxShell);

/*************************************************************************
|*
|* Standard-Konstruktor
|*
\************************************************************************/

BezierObjectBar::BezierObjectBar(
    ViewShell* pSdViewShell,
    ::sd::View* pSdView)
    : SfxShell(pSdViewShell->GetViewShell()),
      mpView(pSdView),
      mpViewSh(pSdViewShell)
{
    DrawDocShell* pDocShell = mpViewSh->GetDocSh();
    SetPool(&pDocShell->GetPool());
    SetUndoManager(pDocShell->GetUndoManager());
    SetRepeatTarget(mpView);

    SetHelpId( SD_IF_SDDRAWBEZIEROBJECTBAR );
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

BezierObjectBar::~BezierObjectBar()
{
    SetRepeatTarget(NULL);
}


/*************************************************************************
|*
|* Status der Attribut-Items
|*
\************************************************************************/

void BezierObjectBar::GetAttrState(SfxItemSet& rSet)
{
    SfxItemSet aAttrSet( mpView->GetDoc()->GetPool() );
    mpView->GetAttributes( aAttrSet );
    rSet.Put(aAttrSet, FALSE); // <- FALSE, damit DontCare-Status uebernommen wird

    FunctionReference xFunc( mpViewSh->GetCurrentFunction() );

    if(xFunc.is())
    {
        if(xFunc->ISA(FuSelection))
        {
            USHORT nEditMode = static_cast<FuSelection*>(xFunc.get())->GetEditMode();
            rSet.Put(SfxBoolItem(nEditMode, TRUE));
        }
        else if (xFunc->ISA(FuConstructBezierPolygon))
        {
            USHORT nEditMode = static_cast<FuConstructBezierPolygon*>(xFunc.get())->GetEditMode();
            rSet.Put(SfxBoolItem(nEditMode, TRUE));
        }
    }

    if (!mpView->IsRipUpAtMarkedPointsPossible())
    {
        rSet.DisableItem(SID_BEZIER_CUTLINE);
    }
    if (!mpView->IsDeleteMarkedPointsPossible())
    {
        rSet.DisableItem(SID_BEZIER_DELETE);
    }
    if (!mpView->IsSetMarkedSegmentsKindPossible())
    {
        rSet.DisableItem(SID_BEZIER_CONVERT);
    }
    else
    {
        SdrPathSegmentKind eSegm = mpView->GetMarkedSegmentsKind();
        switch (eSegm)
        {
            case SDRPATHSEGMENT_DONTCARE: rSet.InvalidateItem(SID_BEZIER_CONVERT); break;
            case SDRPATHSEGMENT_LINE    : rSet.Put(SfxBoolItem(SID_BEZIER_CONVERT,FALSE)); break; // Button reingedrueckt = Kurve
            case SDRPATHSEGMENT_CURVE   : rSet.Put(SfxBoolItem(SID_BEZIER_CONVERT,TRUE));  break;
            default: break;
        }
    }
    if (!mpView->IsSetMarkedPointsSmoothPossible())
    {
        rSet.DisableItem(SID_BEZIER_EDGE);
        rSet.DisableItem(SID_BEZIER_SMOOTH);
        rSet.DisableItem(SID_BEZIER_SYMMTR);
    }
    else
    {
        SdrPathSmoothKind eSmooth = mpView->GetMarkedPointsSmooth();
        switch (eSmooth)
        {
            case SDRPATHSMOOTH_DONTCARE  : break;
            case SDRPATHSMOOTH_ANGULAR   : rSet.Put(SfxBoolItem(SID_BEZIER_EDGE,  TRUE)); break;
            case SDRPATHSMOOTH_ASYMMETRIC: rSet.Put(SfxBoolItem(SID_BEZIER_SMOOTH,TRUE)); break;
            case SDRPATHSMOOTH_SYMMETRIC : rSet.Put(SfxBoolItem(SID_BEZIER_SYMMTR,TRUE)); break;
        }
    }
    if (!mpView->IsOpenCloseMarkedObjectsPossible())
    {
        rSet.DisableItem(SID_BEZIER_CLOSE);
    }
    else
    {
        SdrObjClosedKind eClose = mpView->GetMarkedObjectsClosedState();
        switch (eClose)
        {
            case SDROBJCLOSED_DONTCARE: rSet.InvalidateItem(SID_BEZIER_CLOSE); break;
            case SDROBJCLOSED_OPEN    : rSet.Put(SfxBoolItem(SID_BEZIER_CLOSE,FALSE)); break;
            case SDROBJCLOSED_CLOSED  : rSet.Put(SfxBoolItem(SID_BEZIER_CLOSE,TRUE)); break;
            default: break;
        }
    }

    rSet.Put(SfxBoolItem(SID_BEZIER_ELIMINATE_POINTS, mpView->IsEliminatePolyPoints()));
}


/*************************************************************************
|*
|* Bearbeitung der SfxRequests
|*
\************************************************************************/

void BezierObjectBar::Execute(SfxRequest& rReq)
{
    USHORT nSId = rReq.GetSlot();

    switch (nSId)
    {
        case SID_BEZIER_CUTLINE:
        case SID_BEZIER_CONVERT:
        case SID_BEZIER_DELETE:
        case SID_BEZIER_EDGE:
        case SID_BEZIER_SMOOTH:
        case SID_BEZIER_SYMMTR:
        case SID_BEZIER_CLOSE:
        {
            const SdrMarkList& rMarkList = mpView->GetMarkedObjectList();

            if (rMarkList.GetMark(0) && !mpView->IsAction())
            {
                switch (nSId)
                {
                    case SID_BEZIER_DELETE:
                        mpView->DeleteMarkedPoints();
                        break;

                    case SID_BEZIER_CUTLINE:
                        mpView->RipUpAtMarkedPoints();
                        break;

                    case SID_BEZIER_CONVERT:
                    {
                        mpView->SetMarkedSegmentsKind(SDRPATHSEGMENT_TOGGLE);
                        break;
                    }

                    case SID_BEZIER_EDGE:
                    case SID_BEZIER_SMOOTH:
                    case SID_BEZIER_SYMMTR:
                    {
                        SdrPathSmoothKind eKind;

                        switch (nSId)
                        {
                            default:
                            case SID_BEZIER_EDGE:   eKind = SDRPATHSMOOTH_ANGULAR; break;
                            case SID_BEZIER_SMOOTH: eKind = SDRPATHSMOOTH_ASYMMETRIC; break;
                            case SID_BEZIER_SYMMTR: eKind = SDRPATHSMOOTH_SYMMETRIC; break;
                        }

                        mpView->SetMarkedPointsSmooth(eKind);
                        break;
                    }

                    case SID_BEZIER_CLOSE:
                    {
                        SdrPathObj* pPathObj = (SdrPathObj*) rMarkList.GetMark(0)->GetMarkedSdrObj();
                        mpView->BegUndo(String(SdResId(STR_UNDO_BEZCLOSE)));
                        mpView->UnmarkAllPoints();
                        mpView->AddUndo(mpView->GetModel()->GetSdrUndoFactory().CreateUndoGeoObject(*pPathObj));
                        pPathObj->ToggleClosed();
                        mpView->EndUndo();
                        break;
                    }
                }
            }

            if ( !mpView->AreObjectsMarked() )
                mpViewSh->GetViewFrame()->GetDispatcher()->Execute(SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);

            rReq.Ignore();
        }
        break;

        case SID_BEZIER_ELIMINATE_POINTS:
        {
            mpView->SetEliminatePolyPoints(!mpView->IsEliminatePolyPoints());
            Invalidate(SID_BEZIER_ELIMINATE_POINTS);
            rReq.Done();
        }
        break;

        case SID_BEZIER_MOVE:
        case SID_BEZIER_INSERT:
        {
            FunctionReference xFunc( mpViewSh->GetCurrentFunction() );

            if(xFunc.is())
            {
                if(xFunc->ISA(FuSelection))
                {
                    static_cast<FuSelection*>(xFunc.get())->SetEditMode(rReq.GetSlot());
                }
                else if(xFunc->ISA(FuConstructBezierPolygon))
                {
                    static_cast<FuConstructBezierPolygon*>(xFunc.get())->SetEditMode(rReq.GetSlot());
                }
            }

            rReq.Ignore ();
        }
        break;

        default:
        break;
    }

    Invalidate();
}


} // end of namespace sd
