/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuconbez.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-18 10:45:55 $
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

#include <com/sun/star/presentation/EffectNodeType.hpp>

#include "fuconbez.hxx"

#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif
#ifndef _SVDOPATH_HXX //autogen
#include <svx/svdopath.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVDOBJ_HXX //autogen
#include <svx/svdobj.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif


#include <svx/svxids.hrc>

#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif

#include "app.hrc"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#ifndef SD_TOOL_BAR_MANAGER_HXX
#include "ToolBarManager.hxx"
#endif
#include "drawdoc.hxx"
#include "res_bmp.hrc"

#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif

#include "CustomAnimationEffect.hxx"

using namespace ::com::sun::star::uno;

namespace sd {

TYPEINIT1( FuConstructBezierPolygon, FuConstruct );


/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuConstructBezierPolygon::FuConstructBezierPolygon (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDoc,
    SfxRequest& rReq)
    : FuConstruct(pViewSh, pWin, pView, pDoc, rReq),
      nEditMode(SID_BEZIER_MOVE)
{
}

FunctionReference FuConstructBezierPolygon::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq, bool bPermanent )
{
    FuConstructBezierPolygon* pFunc;
    FunctionReference xFunc( pFunc = new FuConstructBezierPolygon( pViewSh, pWin, pView, pDoc, rReq ) );
    xFunc->DoExecute(rReq);
    pFunc->SetPermanent(bPermanent);
    return xFunc;
}

void FuConstructBezierPolygon::DoExecute( SfxRequest& rReq )
{
    FuConstruct::DoExecute( rReq );

    const SfxItemSet* pArgs = rReq.GetArgs();
    if( pArgs )
    {
        const SfxPoolItem*  pPoolItem = NULL;
        if( SFX_ITEM_SET == pArgs->GetItemState( SID_ADD_MOTION_PATH, TRUE, &pPoolItem ) )
            maTargets = ( ( const SfxUnoAnyItem* ) pPoolItem )->GetValue();
    }
}

/*************************************************************************
|*
|* MouseButtonDown-event
|*
\************************************************************************/

BOOL FuConstructBezierPolygon::MouseButtonDown(const MouseEvent& rMEvt)
{
    BOOL bReturn = FuConstruct::MouseButtonDown(rMEvt);

    SdrViewEvent aVEvt;
    SdrHitKind eHit = mpView->PickAnything(rMEvt, SDRMOUSEBUTTONDOWN, aVEvt);

    if (eHit == SDRHIT_HANDLE || rMEvt.IsMod1())
    {
        mpView->SetEditMode(SDREDITMODE_EDIT);
    }
    else
    {
        mpView->SetEditMode(SDREDITMODE_CREATE);
    }

    if (aVEvt.eEvent == SDREVENT_BEGTEXTEDIT)
    {
        // Texteingabe hier nicht zulassen
        aVEvt.eEvent = SDREVENT_BEGDRAGOBJ;
        mpView->EnableExtendedMouseEventDispatcher(FALSE);
    }
    else
    {
        mpView->EnableExtendedMouseEventDispatcher(TRUE);
    }

    if (eHit == SDRHIT_MARKEDOBJECT && nEditMode == SID_BEZIER_INSERT)
    {
        /******************************************************************
        * Klebepunkt einfuegen
        ******************************************************************/
        mpView->BegInsObjPoint(aMDPos, rMEvt.IsMod1());
    }
    else
    {
        mpView->MouseButtonDown(rMEvt, mpWindow);

        SdrObject* pObj = mpView->GetCreateObj();

        if (pObj)
        {
            SfxItemSet aAttr(mpDoc->GetPool());
            SetStyleSheet(aAttr, pObj);
            pObj->SetMergedItemSet(aAttr);
        }
    }

    return(bReturn);
}

/*************************************************************************
|*
|* MouseMove-event
|*
\************************************************************************/

BOOL FuConstructBezierPolygon::MouseMove(const MouseEvent& rMEvt)
{
    BOOL bReturn = FuConstruct::MouseMove(rMEvt);
    return(bReturn);
}

/*************************************************************************
|*
|* MouseButtonUp-event
|*
\************************************************************************/

BOOL FuConstructBezierPolygon::MouseButtonUp(const MouseEvent& rMEvt )
{
    BOOL bReturn = FALSE;
    BOOL bCreated = FALSE;

    SdrViewEvent aVEvt;
    mpView->PickAnything(rMEvt, SDRMOUSEBUTTONUP, aVEvt);

    ULONG nCount = mpView->GetSdrPageView()->GetObjList()->GetObjCount();

    if (mpView->IsInsObjPoint())
    {
        mpView->EndInsObjPoint(SDRCREATE_FORCEEND);
    }
    else
    {
        mpView->MouseButtonUp(rMEvt, mpWindow);
    }

    if (aVEvt.eEvent == SDREVENT_ENDCREATE)
    {
        bReturn = TRUE;

        if (nCount == (mpView->GetSdrPageView()->GetObjList()->GetObjCount() - 1))
        {
            bCreated = TRUE;
        }

        // Trick, um FuDraw::DoubleClick nicht auszuloesen
        bMBDown = FALSE;

    }

    bReturn = FuConstruct::MouseButtonUp(rMEvt) || bReturn;

    bool bDeleted = false;
    if( bCreated && maTargets.hasValue() )
    {
        SdrPathObj* pPathObj = dynamic_cast< SdrPathObj* >( mpView->GetSdrPageView()->GetObjList()->GetObj( nCount ) );
        SdPage* pPage = dynamic_cast< SdPage* >( pPathObj ? pPathObj->GetPage() : 0 );
        if( pPage )
        {
            boost::shared_ptr< sd::MainSequence > pMainSequence( pPage->getMainSequence() );
            if( pMainSequence.get() )
            {
                Sequence< Any > aTargets;
                maTargets >>= aTargets;

                sal_Int32 nTCount = aTargets.getLength();
                if( nTCount > 1 )
                {
                    const Any* pTarget = aTargets.getConstArray();
                    double fDuration = 0.0;
                    *pTarget++ >>= fDuration;
                    bool bFirst = true;
                    while( --nTCount )
                    {
                        CustomAnimationEffectPtr pCreated( pMainSequence->append( *pPathObj, *pTarget++, fDuration ) );
                        if( bFirst )
                            bFirst = false;
                        else
                            pCreated->setNodeType( ::com::sun::star::presentation::EffectNodeType::WITH_PREVIOUS );
                    }
                }
            }
        }
        mpView->DeleteMarked();
        bDeleted = true;
    }

    if ((!bPermanent && bCreated) || bDeleted)
    {
        mpViewShell->GetViewFrame()->GetDispatcher()->Execute(SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);
    }

    return(bReturn);
}

/*************************************************************************
|*
|* Tastaturereignisse bearbeiten
|*
|* Wird ein KeyEvent bearbeitet, so ist der Return-Wert TRUE, andernfalls
|* FALSE.
|*
\************************************************************************/

BOOL FuConstructBezierPolygon::KeyInput(const KeyEvent& rKEvt)
{
    BOOL bReturn = FuConstruct::KeyInput(rKEvt);

    return(bReturn);
}

/*************************************************************************
|*
|* Function aktivieren
|*
\************************************************************************/

void FuConstructBezierPolygon::Activate()
{
    mpView->EnableExtendedMouseEventDispatcher(TRUE);

    SdrObjKind eKind;

    switch (nSlotId)
    {
        case SID_DRAW_POLYGON_NOFILL:
        case SID_DRAW_XPOLYGON_NOFILL:
        {
            eKind = OBJ_PLIN;
        }
        break;

        case SID_DRAW_POLYGON:
        case SID_DRAW_XPOLYGON:
        {
            eKind = OBJ_POLY;
        }
        break;

        case SID_DRAW_BEZIER_NOFILL:
        {
            eKind = OBJ_PATHLINE;
        }
        break;

        case SID_DRAW_BEZIER_FILL:
        {
            eKind = OBJ_PATHFILL;
        }
        break;

        case SID_DRAW_FREELINE_NOFILL:
        {
            eKind = OBJ_FREELINE;
        }
        break;

        case SID_DRAW_FREELINE:
        {
            eKind = OBJ_FREEFILL;
        }
        break;

        default:
        {
            eKind = OBJ_PATHLINE;
        }
        break;
    }

    mpView->SetCurrentObj((UINT16)eKind);

    FuConstruct::Activate();
}

/*************************************************************************
|*
|* Function deaktivieren
|*
\************************************************************************/

void FuConstructBezierPolygon::Deactivate()
{
    mpView->EnableExtendedMouseEventDispatcher(FALSE);

    FuConstruct::Deactivate();
}


/*************************************************************************
|*
|* Selektion hat sich geaendert
|*
\************************************************************************/

void FuConstructBezierPolygon::SelectionHasChanged()
{
    FuDraw::SelectionHasChanged();

    mpViewShell->GetViewShellBase().GetToolBarManager()->SelectionHasChanged(
        *mpViewShell,
        *mpView);
}



/*************************************************************************
|*
|* Aktuellen Bezier-Editmodus setzen
|*
\************************************************************************/

void FuConstructBezierPolygon::SetEditMode(USHORT nMode)
{
    nEditMode = nMode;
    ForcePointer();

    SfxBindings& rBindings = mpViewShell->GetViewFrame()->GetBindings();
    rBindings.Invalidate(SID_BEZIER_MOVE);
    rBindings.Invalidate(SID_BEZIER_INSERT);
}

// #97016#
SdrObject* FuConstructBezierPolygon::CreateDefaultObject(const sal_uInt16 nID, const Rectangle& rRectangle)
{
    // case SID_DRAW_POLYGON:
    // case SID_DRAW_POLYGON_NOFILL:
    // case SID_DRAW_XPOLYGON:
    // case SID_DRAW_XPOLYGON_NOFILL:
    // case SID_DRAW_FREELINE:
    // case SID_DRAW_FREELINE_NOFILL:
    // case SID_DRAW_BEZIER_FILL:          // BASIC
    // case SID_DRAW_BEZIER_NOFILL:        // BASIC

    SdrObject* pObj = SdrObjFactory::MakeNewObject(
        mpView->GetCurrentObjInventor(), mpView->GetCurrentObjIdentifier(),
        0L, mpDoc);

    if(pObj)
    {
        if(pObj->ISA(SdrPathObj))
        {
            basegfx::B2DPolyPolygon aPoly;

            switch(nID)
            {
                case SID_DRAW_BEZIER_FILL:
                {
                    const sal_Int32 nWdt(rRectangle.GetWidth() / 2);
                    const sal_Int32 nHgt(rRectangle.GetHeight() / 2);
                    const basegfx::B2DPolygon aInnerPoly(basegfx::tools::createPolygonFromEllipse(basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Center().Y()), nWdt, nHgt));

                    aPoly.append(aInnerPoly);
                    break;
                }
                case SID_DRAW_BEZIER_NOFILL:
                {
                    basegfx::B2DPolygon aInnerPoly;

                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left(), rRectangle.Bottom()));

                    const basegfx::B2DPoint aCenterBottom(rRectangle.Center().X(), rRectangle.Bottom());
                    aInnerPoly.appendBezierSegment(
                        aCenterBottom,
                        aCenterBottom,
                        basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Center().Y()));

                    const basegfx::B2DPoint aCenterTop(rRectangle.Center().X(), rRectangle.Top());
                    aInnerPoly.appendBezierSegment(
                        aCenterTop,
                        aCenterTop,
                        basegfx::B2DPoint(rRectangle.Right(), rRectangle.Top()));

                    aPoly.append(aInnerPoly);
                    break;
                }
                case SID_DRAW_FREELINE:
                case SID_DRAW_FREELINE_NOFILL:
                {
                    basegfx::B2DPolygon aInnerPoly;

                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left(), rRectangle.Bottom()));

                    aInnerPoly.appendBezierSegment(
                        basegfx::B2DPoint(rRectangle.Left(), rRectangle.Top()),
                        basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Top()),
                        basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Center().Y()));

                    aInnerPoly.appendBezierSegment(
                        basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Bottom()),
                        basegfx::B2DPoint(rRectangle.Right(), rRectangle.Bottom()),
                        basegfx::B2DPoint(rRectangle.Right(), rRectangle.Top()));

                    if(SID_DRAW_FREELINE == nID)
                    {
                        aInnerPoly.append(basegfx::B2DPoint(rRectangle.Right(), rRectangle.Bottom()));
                    }
                    else
                    {
                        aInnerPoly.setClosed(true);
                    }

                    aPoly.append(aInnerPoly);
                    break;
                }
                case SID_DRAW_XPOLYGON:
                case SID_DRAW_XPOLYGON_NOFILL:
                {
                    basegfx::B2DPolygon aInnerPoly;

                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left(), rRectangle.Bottom()));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left(), rRectangle.Top()));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Top()));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Center().Y()));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Right(), rRectangle.Center().Y()));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Right(), rRectangle.Bottom()));

                    if(SID_DRAW_XPOLYGON_NOFILL == nID)
                    {
                        aInnerPoly.append(basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Bottom()));
                    }
                    else
                    {
                        aInnerPoly.setClosed(true);
                    }

                    aPoly.append(aInnerPoly);
                    break;
                }
                case SID_DRAW_POLYGON:
                case SID_DRAW_POLYGON_NOFILL:
                {
                    basegfx::B2DPolygon aInnerPoly;
                    const sal_Int32 nWdt(rRectangle.GetWidth());
                    const sal_Int32 nHgt(rRectangle.GetHeight());

                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left(), rRectangle.Bottom()));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left() + (nWdt * 30) / 100, rRectangle.Top() + (nHgt * 70) / 100));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left(), rRectangle.Top() + (nHgt * 15) / 100));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left() + (nWdt * 65) / 100, rRectangle.Top()));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left() + nWdt, rRectangle.Top() + (nHgt * 30) / 100));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left() + (nWdt * 80) / 100, rRectangle.Top() + (nHgt * 50) / 100));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Left() + (nWdt * 80) / 100, rRectangle.Top() + (nHgt * 75) / 100));
                    aInnerPoly.append(basegfx::B2DPoint(rRectangle.Bottom(), rRectangle.Right()));

                    if(SID_DRAW_POLYGON_NOFILL == nID)
                    {
                        aInnerPoly.append(basegfx::B2DPoint(rRectangle.Center().X(), rRectangle.Bottom()));
                    }
                    else
                    {
                        aInnerPoly.setClosed(true);
                    }

                    aPoly.append(aInnerPoly);
                    break;
                }
            }

            ((SdrPathObj*)pObj)->SetPathPoly(aPoly);
        }
        else
        {
            DBG_ERROR("Object is NO path object");
        }

        pObj->SetLogicRect(rRectangle);
    }

    return pObj;
}

} // end of namespace sd

// eof
