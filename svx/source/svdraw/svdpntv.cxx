/*************************************************************************
 *
 *  $RCSfile: svdpntv.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-28 16:31:51 $
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

#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROL_HPP_
#include <com/sun/star/awt/XControl.hpp>
#endif

#include "svdpntv.hxx"
#include "editdata.hxx"
#include "svdmrkv.hxx"
#include "svdxout.hxx"
#include "svdpagv.hxx"
#include "svdpage.hxx"
#include "svdmodel.hxx"
#include "svdvmark.hxx"
#include "svdio.hxx"
#include "svdundo.hxx"
#include "svdview.hxx"
#include "svdglue.hxx"
#include "svdobj.hxx"
#include "svdograf.hxx"
#include "svdattrx.hxx"
#include "svdibrow.hxx"
#include "svditer.hxx"
#include "svdouno.hxx"

#ifndef _SVX_XLNCLIT_HXX
#include "xlnclit.hxx"
#endif
#ifndef _SVX_XFLCLIT_HXX
#include "xflclit.hxx"
#endif

#ifndef _EEITEM_HXX //autogen
#include <eeitem.hxx>
#endif

#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif

#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif

#ifndef _SFXITEMITER_HXX //autogen
#include <svtools/itemiter.hxx>
#endif

#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif

#ifndef _B2D_MBMP_HXX
#include <goodies/b2dmbmp.hxx>
#endif

#ifndef _B2D_MTRI_HXX
#include <goodies/b2dmtri.hxx>
#endif

#ifndef _GRFMGR_HXX
#include <goodies/grfmgr.hxx>
#endif

// #i38135#
#ifndef _SDR_CONTACT_OBJECTCONTACT_HXX
#include <svx/sdr/contact/objectcontact.hxx>
#endif

#ifndef _SDR_ANIMATION_OBJECTANIMATOR_HXX
#include <svx/sdr/animation/objectanimator.hxx>
#endif

#ifndef _SDR_CONTACT_VIEWCONTACT_HXX
#include <svx/sdr/contact/viewcontact.hxx>
#endif

using namespace ::rtl;
using namespace ::com::sun::star;

////////////////////////////////////////////////////////////////////////////////////////////////////

void RollingRect::DrawHor(OutputDevice& rOut, long x1, long x2, long y, BOOL bMov) const
{
    USHORT nLen2=nLen*2;
    BOOL bBck=x1>x2;
    long nOfs=nIdx;
    if (bBck) {
        long nTmp;
        nTmp=x1-x2+1+nLen2+nLen;
        nTmp%=nLen2;
        nOfs=nLen2-nOfs+nTmp;
        nTmp=x1; x1=x2; x2=nTmp;
    }
    while (nOfs>0) nOfs-=nLen2;
    long a1,a2,a;
    a=x1+nOfs;
    while (a<=x2+1) {
        a1=a;
        a2=a+(nLen-1);
        if (bMov) {
            if (!bBck) a2++; else a1--;
            if (a1>=x1 && a1<=x2) ((Window&)rOut).Invert(Rectangle(a1,y,a1,y));
            if (a2>=x1 && a2<=x2) ((Window&)rOut).Invert(Rectangle(a2,y,a2,y));
        } else {
            if (a1<x1) a1=x1;
            if (a2>x2) a2=x2;
            if (a1<=a2) ((Window&)rOut).Invert(Rectangle(a1,y,a2,y));
        }
        a+=nLen2;
    }
}

void RollingRect::DrawVer(OutputDevice& rOut, long x, long y1, long y2, BOOL bMov) const
{
    USHORT nLen2=nLen*2;
    BOOL bBck=y1>y2;
    long nOfs=nIdx;
    if (bBck) {
        long nTmp;
        nTmp=y1-y2+1+nLen2+nLen;
        nTmp%=nLen2;
        nOfs=nLen2-nOfs+nTmp;
        nTmp=y1; y1=y2; y2=nTmp;
    }
    while (nOfs>0) nOfs-=nLen2;
    long a1,a2,a;
    a=y1+nOfs;
    while (a<=y2+1) {
        a1=a;
        a2=a+nLen-1;
        if (bMov) {
            if (!bBck) a2++; else a1--;
            if (a1>=y1 && a1<=y2) ((Window&)rOut).Invert(Rectangle(x,a1,x,a1));
            if (a2>=y1 && a2<=y2) ((Window&)rOut).Invert(Rectangle(x,a2,x,a2));
        } else {
            if (a1<y1) a1=y1;
            if (a2>y2) a2=y2;
            if (a1<=a2) ((Window&)rOut).Invert(Rectangle(x,a1,x,a2));
        }
        a+=nLen2;
    }
}

void RollingRect::DrawRect(OutputDevice& rOut, BOOL bMov) const
{
    BOOL bMap0=rOut.IsMapModeEnabled();
    Point aPt1(rOut.LogicToPixel(aP1));
    Point aPt2(rOut.LogicToPixel(aP2));
    rOut.EnableMapMode(FALSE);
    Size aS(rOut.GetOutputSizePixel());
    long dx=Abs(aPt2.X()-aPt1.X());
    long dy=Abs(aPt2.Y()-aPt1.Y());
    if (IsStripes()) {
        DrawHor(rOut,aPt1.X(),0,aPt1.Y(),bMov);
        DrawHor(rOut,aPt2.X(),aS.Width(),aPt1.Y(),bMov);
        if (dy!=0) {
            DrawHor(rOut,aPt1.X(),0,aPt2.Y(),bMov);
            DrawHor(rOut,aPt2.X(),aS.Width(),aPt2.Y(),bMov);
        }
        DrawVer(rOut,aPt1.X(),aPt1.Y(),0,bMov);
        DrawVer(rOut,aPt1.X(),aPt2.Y(),aS.Height(),bMov);
        if (dx!=0) {
            DrawVer(rOut,aPt2.X(),aPt1.Y(),0,bMov);
            DrawVer(rOut,aPt2.X(),aPt2.Y(),aS.Height(),bMov);
        }
    } else if (IsCrossHair()) {
        DrawHor(rOut,aPt1.X(),0,aPt1.Y(),bMov);
        DrawHor(rOut,aPt1.X(),aS.Width(),aPt1.Y(),bMov);
        DrawVer(rOut,aPt1.X(),aPt1.Y(),0,bMov);
        DrawVer(rOut,aPt1.X(),aPt1.Y(),aS.Height(),bMov);
    } else {
        if (dx!=0) {
            DrawHor(rOut,aPt1.X(),aPt2.X(),aPt1.Y(),bMov);
            if (dy>0) DrawHor(rOut,aPt2.X(),aPt1.X(),aPt2.Y(),bMov);
        }
        if (dy!=0 || (dy==1 && dx>1)) {
            DrawVer(rOut,aPt1.X(),aPt1.Y(),aPt2.Y(),bMov);
            if (dx>0) DrawVer(rOut,aPt2.X(),aPt2.Y(),aPt1.Y(),bMov);
        }
    }
    rOut.EnableMapMode(bMap0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FrameAnimator::FrameAnimator(SdrView& rNewView):
    rView(rNewView)
{
    aTim.SetTimeoutHdl(LINK(this,FrameAnimator,Hdl));
    aTim.SetTimeout(50);
    pOut=NULL;
}

void FrameAnimator::Start() const
{
    ((FrameAnimator*)this)->aTim.Start();
}

void FrameAnimator::Stop() const
{
    ((FrameAnimator*)this)->aTim.Stop();
}

IMPL_LINK(FrameAnimator,Hdl,AutoTimer*,pTim)
{
    if (rView.aDragStat.IsShown()) {
        USHORT i=0;
        do {
            OutputDevice* pO=rView.pDragWin;
            if (pO==NULL) {
                pO=rView.GetWin(i);
                i++;
            }
            if (pO!=NULL) {
                RollIt(*pO,FALSE);
            }
        } while (pOut==NULL && i<rView.GetWinCount());
        IncRollIdx();
    }
    return 0;
}

void FrameAnimator::Invert(OutputDevice* pNewOut) const
{
    USHORT i=0;
    ((FrameAnimator*)this)->pOut=pNewOut;
    do {
        OutputDevice* pO=pNewOut;
        if (pO==NULL) {
            pO=rView.GetWin(i);
            i++;
        }
        if (pO!=NULL) {
            RollingRect::Invert(*pO);
        }
    } while (pOut==NULL && i<rView.GetWinCount());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrViewWinRec::SdrViewWinRec(OutputDevice* pW)
:   pWin(pW),
    bXorVisible(FALSE)
{
}

SdrViewWinRec::~SdrViewWinRec()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrViewWinList::Clear()
{
    USHORT nAnz=GetCount();
    for (USHORT i=0; i<nAnz; i++) {
        delete GetObject(i);
    }
    aList.Clear();
}

USHORT SdrViewWinList::Find(OutputDevice* pW) const
{
    USHORT nAnz=GetCount();
    USHORT nRet=SDRVIEWWIN_NOTFOUND;
    for (USHORT nNum=0; nNum<nAnz && nRet==SDRVIEWWIN_NOTFOUND; nNum++) {
        if (GetObject(nNum)->pWin==pW) nRet=nNum;
    }
    return nRet;
}

//------------------------------------------------------------------------

TYPEINIT1( SvxViewHint, SfxSimpleHint );

SvxViewHint::SvxViewHint( ULONG _nId ) :
    SfxSimpleHint( _nId )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@   @@@@  @@ @@  @@ @@@@@@  @@ @@ @@ @@@@@ @@   @@
//  @@  @@ @@  @@ @@ @@@ @@   @@    @@ @@ @@ @@    @@   @@
//  @@  @@ @@  @@ @@ @@@@@@   @@    @@ @@ @@ @@    @@ @ @@
//  @@@@@  @@@@@@ @@ @@@@@@   @@    @@@@@ @@ @@@@  @@@@@@@
//  @@     @@  @@ @@ @@ @@@   @@     @@@  @@ @@    @@@@@@@
//  @@     @@  @@ @@ @@  @@   @@     @@@  @@ @@    @@@ @@@
//  @@     @@  @@ @@ @@  @@   @@      @   @@ @@@@@ @@   @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEINIT2(SdrPaintView,SfxListener,SfxRepeatTarget);

DBG_NAME(SdrPaintView);

void SdrPaintView::ImpClearVars()
{
    pXOut=NULL;
    bForeignXOut=FALSE;
    pItemBrowser=NULL;
    // bLayerSortedRedraw=FALSE;
    bPageVisible=TRUE;
    bPageBorderVisible=TRUE;
    bBordVisible=TRUE;
    bGridVisible=TRUE;
    bGridFront  =FALSE;
    bHlplVisible=TRUE;
    bHlplFront  =TRUE;
    bGlueVisible=FALSE;
    bGlueVisible2=FALSE;
    bGlueVisible3=FALSE;
    bGlueVisible4=FALSE;
    bSwapAsynchron=FALSE;
    bPrintPreview=FALSE;
    mbLineDraft=FALSE;
    mbFillDraft=FALSE;
    mbGrafDraft=FALSE;
    mbHideGrafDraft=FALSE;
    mbTextDraft=FALSE;
    mbLineDraftPrn=FALSE;
    mbFillDraftPrn=FALSE;
    mbGrafDraftPrn=FALSE;
    mbTextDraftPrn=FALSE;

    eAnimationMode = SDR_ANIMATION_ANIMATE;
    bAnimationPause = FALSE;

    nHitTolPix=2;
    nMinMovPix=3;
    nHitTolLog=0;
    nMinMovLog=0;
    pActualOutDev=NULL;

    bPageTwice=FALSE;
    pDragWin=NULL;
    bRestoreColors=TRUE;
    pDefaultStyleSheet=NULL;
    bEncircle=FALSE;
    bSomeObjChgdFlag=FALSE;

    nGraphicManagerDrawMode = GRFMGR_DRAW_STANDARD;

    aComeBackTimer.SetTimeout(1);
    aComeBackTimer.SetTimeoutHdl(LINK(this,SdrPaintView,ImpComeBackHdl));

    // #111097#
    maAfterPaintTimer.SetTimeout(1);
    maAfterPaintTimer.SetTimeoutHdl(LINK(this,SdrPaintView,ImpAfterPaintHdl));

    aUserMarkerAnimator.SetTimeout(50);
    aUserMarkerAnimator.SetTimeoutHdl(LINK(this,SdrPaintView,ImpUserMarkerAnimatorHdl));

    String aNam;    // System::GetUserName() just return an empty string

    if (pMod)
        SetDefaultStyleSheet(pMod->GetDefaultStyleSheet(), TRUE);

    aNam.ToUpperAscii();

    maGridColor = Color( COL_BLACK );
}


SdrPaintView::SdrPaintView(SdrModel* pModel1, OutputDevice* pOut):
    aPagV(1024,16,16),
    aAni(*(SdrView*)this),
    aDefaultAttr(pModel1->GetItemPool()),
    aUserMarkers(1024,16,16),
    mbBufferedOutputAllowed(sal_False),
    mbPagePaintingAllowed(sal_True)
{
    DBG_CTOR(SdrPaintView,NULL);
    pMod=pModel1;
    ImpClearVars();
    if (pOut!=NULL) AddWin(pOut);
    pXOut=new XOutputDevice(pOut);

    // Flag zur Visualisierung von Gruppen
    bVisualizeEnteredGroup = TRUE;

    StartListening( maColorConfig );
    onChangeColorConfig();
}

SdrPaintView::SdrPaintView(SdrModel* pModel1, XOutputDevice* pExtOut):
    aPagV(1024,16,16),
    aAni(*(SdrView*)this),
    aDefaultAttr(pModel1->GetItemPool()),
    aUserMarkers(1024,16,16),
    mbBufferedOutputAllowed(sal_False),
    mbPagePaintingAllowed(sal_True)
{
    DBG_CTOR(SdrPaintView,NULL);
    pMod=pModel1;
    ImpClearVars();
    pMod=pModel1;
    if (pExtOut!=NULL) {
        bForeignXOut=TRUE;
        pXOut=pExtOut;
        OutputDevice* pO=pXOut->GetOutDev();
        if (pO!=NULL) AddWin(pO);
    } else {
        pXOut=new XOutputDevice(NULL);
    }

    // Flag zur Visualisierung von Gruppen
    bVisualizeEnteredGroup = TRUE;

    StartListening( maColorConfig );
    onChangeColorConfig();
}

SdrPaintView::~SdrPaintView()
{
    DBG_DTOR(SdrPaintView,NULL);

    // #111097#
    maAfterPaintTimer.Stop();

    EndListening( maColorConfig );

    ClearAll();
    if (!bForeignXOut && pXOut!=NULL) {
        delete pXOut;
    }
#ifndef SVX_LIGHT
    if (pItemBrowser!=NULL) {
        delete pItemBrowser;
    }
#endif
    USHORT nAnz=ImpGetUserMarkerCount();
    for (USHORT nNum=0; nNum<nAnz; nNum++) {
        SdrViewUserMarker* pUM=ImpGetUserMarker(nNum);
        pUM->pView=NULL; // Weil's mich gleich nichtmehr gibt.
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void __EXPORT SdrPaintView::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
{
    BOOL bObjChg=!bSomeObjChgdFlag; // TRUE= auswerten fuer ComeBack-Timer
    if (bObjChg) {
        SdrHint* pSdrHint=PTR_CAST(SdrHint,&rHint);
        if (pSdrHint!=NULL) {
            SdrHintKind eKind=pSdrHint->GetKind();
            if (eKind==HINT_OBJCHG || eKind==HINT_OBJINSERTED || eKind==HINT_OBJREMOVED) {
                if (bObjChg) {
                    bSomeObjChgdFlag=TRUE;
                    aComeBackTimer.Start();

                    // #i32773#
                    // Get rid of handles early
                    // off for testing...// ForceInvalidateMarkHandles();
                }
            }
            if (eKind==HINT_PAGEORDERCHG) {
                const SdrPage* pPg=pSdrHint->GetPage();
                if (!pPg->IsInserted()) { // aha, Seite geloescht: also hiden
                    USHORT nAnz=GetPageViewCount();
                    USHORT nv;
                    for (nv=nAnz; nv>0;) {
                        nv--;
                        SdrPageView* pPV=GetPageViewPvNum(nv);
                        if (pPV->GetPage()==pPg) {
                            HidePagePvNum(nv);
                        }
                    }
                }
            }
        }
    }

    if( rHint.ISA( SfxSimpleHint ) && ( (SfxSimpleHint&) rHint ).GetId() == SFX_HINT_COLORS_CHANGED )
    {
        onChangeColorConfig();
        InvalidateAllWin();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IMPL_LINK_INLINE_START(SdrPaintView,ImpComeBackHdl,Timer*,pTimer)
{
    if (bSomeObjChgdFlag) {
        bSomeObjChgdFlag=FALSE;
        ModelHasChanged();
    }
    return 0;
}

IMPL_LINK_INLINE_END(SdrPaintView,ImpComeBackHdl,Timer*,pTimer)

// #111097#
IMPL_LINK(SdrPaintView,ImpAfterPaintHdl,Timer*,pTimer)
{
    // refresh outside of paint to get the handles displayed correctly. This
    // will be removed and also the AfterPaintTimer as soon as the handles will
    // be handled with the normal paints.
    RefreshAllIAOManagers();
    return 0;
}

void SdrPaintView::FlushComeBackTimer() const
{
    if (bSomeObjChgdFlag) {
        // casting auf nonconst
        ((SdrPaintView*)this)->ImpComeBackHdl(&((SdrPaintView*)this)->aComeBackTimer);
        ((SdrPaintView*)this)->aComeBackTimer.Stop();
    }
}

void SdrPaintView::ModelHasChanged()
{
    // Auch alle PageViews benachrichtigen
    USHORT nv;
    USHORT nAnz;
    nAnz=GetPageViewCount();
    for (nv=nAnz; nv>0;) {
        nv--;
        SdrPageView* pPV=GetPageViewPvNum(nv);
        if (!pPV->GetPage()->IsInserted()) {
            HidePage(pPV);
        }
    }
    nAnz=GetPageViewCount();
    for (nv=0; nv<nAnz; nv++) {
        SdrPageView* pPV=GetPageViewPvNum(nv);
        pPV->ModelHasChanged();
    }
#ifndef SVX_LIGHT
    if (pItemBrowser!=NULL) pItemBrowser->SetDirty();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IMPL_LINK_INLINE_START(SdrPaintView,ImpUserMarkerAnimatorHdl,AutoTimer*,pTimer)
{
    USHORT nAnz=ImpGetUserMarkerCount();
    for (USHORT nNum=0; nNum<nAnz; nNum++) {
        SdrViewUserMarker* pUM=ImpGetUserMarker(nNum);
        if (pUM->IsAnimate() && pUM->IsVisible()) pUM->DoAnimateOneStep();
    }
    return 0;
}

IMPL_LINK_INLINE_END(SdrPaintView,ImpUserMarkerAnimatorHdl,AutoTimer*,pTimer)

void SdrPaintView::ImpInsertUserMarker(SdrViewUserMarker* pMarker)
{
    aUserMarkers.Insert(pMarker);
    ImpCheckMarkerAnimator();
}

void SdrPaintView::ImpRemoveUserMarker(SdrViewUserMarker* pMarker)
{
    aUserMarkers.Remove(pMarker);
    ImpCheckMarkerAnimator();
}

void SdrPaintView::ImpCheckMarkerAnimator()
{
    BOOL bNeed=FALSE;
    USHORT nAnz=ImpGetUserMarkerCount();
    for (USHORT nNum=0; nNum<nAnz && !bNeed; nNum++) {
        SdrViewUserMarker* pUM=ImpGetUserMarker(nNum);
        bNeed=pUM->IsAnimate();
    }
    if (bNeed) aUserMarkerAnimator.Start();
    else aUserMarkerAnimator.Stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SdrPaintView::IsAction() const
{
    return IsEncirclement();
}

void SdrPaintView::MovAction(const Point& rPnt)
{
    if (IsEncirclement()) {
        MovEncirclement(rPnt);
    }
}

void SdrPaintView::EndAction()
{
    if (IsEncirclement()) EndEncirclement();
}

void SdrPaintView::BckAction()
{
    BrkEncirclement();
}

void SdrPaintView::BrkAction()
{
    BrkEncirclement();
}

void SdrPaintView::TakeActionRect(Rectangle& rRect) const
{
    if (IsEncirclement()) {
        rRect=Rectangle(aDragStat.GetStart(),aDragStat.GetNow());
    }
}

void SdrPaintView::ToggleShownXor(OutputDevice* pOut, const Region* pRegion) const
{
    if (IsEncirclement() && aDragStat.IsShown()) {
        DrawEncirclement(pOut);
    }
    USHORT nAnz=ImpGetUserMarkerCount();
    for (USHORT nNum=0; nNum<nAnz; nNum++) {
        SdrViewUserMarker* pUM=ImpGetUserMarker(nNum);
        if (pUM->IsVisible()) pUM->Draw(pOut,FALSE,!bRestoreColors); // den 3. Parameter hier noch richtig setzen !!!!!
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

USHORT SdrPaintView::ImpGetMinMovLogic(short nMinMov, const OutputDevice* pOut) const
{
    if (nMinMov>=0) return USHORT(nMinMov);
    if (pOut==NULL) pOut=GetWin(0);
    if (pOut!=NULL) {
        return short(-pOut->PixelToLogic(Size(nMinMov,0)).Width());
    } else {
        return 0;
    }
}

USHORT SdrPaintView::ImpGetHitTolLogic(short nHitTol, const OutputDevice* pOut) const
{
    if (nHitTol>=0) return USHORT(nHitTol);
    if (pOut==NULL) pOut=GetWin(0);
    if (pOut!=NULL) {
        return short(-pOut->PixelToLogic(Size(nHitTol,0)).Width());
    } else {
        return 0;
    }
}

void SdrPaintView::TheresNewMapMode()
{
    if (pActualOutDev!=NULL) {
        nHitTolLog=(USHORT)((OutputDevice*)pActualOutDev)->PixelToLogic(Size(nHitTolPix,0)).Width();
        nMinMovLog=(USHORT)((OutputDevice*)pActualOutDev)->PixelToLogic(Size(nMinMovPix,0)).Width();
    }
}

void SdrPaintView::SetActualWin(const OutputDevice* pWin)
{
    pActualOutDev=pWin;
    TheresNewMapMode();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrPaintView::BegEncirclement(const Point& rPnt, OutputDevice* pOut, short nMinMov)
{
    BrkAction();
    aDragStat.Reset(rPnt);
    aDragStat.SetMinMove(ImpGetMinMovLogic(nMinMov,pOut));
    if (nMinMov==0) aDragStat.SetMinMoved();
    aAni.Reset();
    aDragStat.NextPoint();
    pDragWin=pOut;
    bEncircle=TRUE;
    if (aDragStat.IsMinMoved()) ShowEncirclement(pOut);
}

void SdrPaintView::MovEncirclement(const Point& rPnt)
{
    if (IsEncirclement()) {
        if (aDragStat.IsMinMoved()) HideEncirclement(pDragWin);
        aDragStat.NextMove(rPnt);
        if (aDragStat.CheckMinMoved(rPnt)) ShowEncirclement(pDragWin);
    }
}

Rectangle SdrPaintView::EndEncirclement(BOOL bNoJustify)
{
    if (IsEncirclement() && aDragStat.IsMinMoved()) {
        HideEncirclement(pDragWin);
        bEncircle=FALSE;
        Rectangle aRect(aDragStat.GetStart(),aDragStat.GetNow());
        if (!bNoJustify) aRect.Justify();
        return aRect;
    }
    bEncircle=FALSE;
    return Rectangle();
}

void SdrPaintView::BrkEncirclement()
{
    if (IsEncirclement()) {
        HideEncirclement(pDragWin);
        bEncircle=FALSE;
    }
}

void SdrPaintView::ShowEncirclement(OutputDevice* pOut)
{
    if (IsEncirclement() && !aDragStat.IsShown()) {
        DrawEncirclement(pOut);
        aDragStat.SetShown(TRUE);
        aAni.Start();
    }
}

void SdrPaintView::HideEncirclement(OutputDevice* pOut)
{
    if (IsEncirclement() && aDragStat.IsShown()) {
        aAni.Stop();
        DrawEncirclement(pOut);
        aDragStat.SetShown(FALSE);
    }
}

void SdrPaintView::DrawEncirclement(OutputDevice* pOut) const
{
    if (IsEncirclement()) {
        aAni.SetP1(aDragStat.GetStart());
        aAni.SetP2(aDragStat.GetNow());
        aAni.Invert(pOut);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrPaintView::ShowShownXor(OutputDevice* pOut, BOOL bShow)
{
    USHORT nAnz=GetWinCount();
    USHORT nw=0;
    BOOL bWeiter=TRUE;
    do {
        if (pOut!=NULL) {
            nw=aWinList.Find(pOut);
            bWeiter=FALSE;
        }
        if (nw<nAnz && nw!=SDRVIEWWIN_NOTFOUND) {
            BOOL bOn=IsShownXorVisibleWinNum(nw);
            if (bOn!=bShow) {
                ToggleShownXor(GetWin(nw),NULL);
                SetShownXorVisible(nw,bShow);
            }
        } else bWeiter=FALSE;
    } while (bWeiter);
}

BOOL SdrPaintView::IsShownXorVisible(OutputDevice* pOut) const
{
    USHORT nPos=aWinList.Find(pOut);
    if (nPos!=SDRVIEWWIN_NOTFOUND) {
        return IsShownXorVisibleWinNum(nPos);
    } else {
        return FALSE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrPaintView::ClearPageViews()
{
    BrkAction();
    for (USHORT nv=0; nv<GetPageViewCount(); nv++) {
        SdrPageView* pPV=GetPageViewPvNum(nv);
        InvalidateAllWin(pPV->GetPageRect());
        delete pPV;
    }
    aPagV.Clear();
}

void SdrPaintView::Clear()
{
    ClearPageViews();
    // ClearHideViews();
}

void SdrPaintView::ClearAll()
{
    ClearPageViews();
}

SdrPageView* SdrPaintView::ShowPage(SdrPage* pPage, const Point& rOffs)
{
    SdrPageView* pPV=NULL;
    if (pPage!=NULL) {
        SdrPageView* pTmpPV=NULL;
        if (!bPageTwice) pTmpPV=GetPageView(pPage); // Evtl. jede Seite nur einmal!
        if (pTmpPV==NULL) {
            pPV=new SdrPageView(pPage,rOffs,*((SdrView*)this));
            if (pPV!=NULL) {
                aPagV.Insert(pPV,CONTAINER_APPEND);
                pPV->Show();
            }
        }
    }
    return pPV;
}

SdrPageView* SdrPaintView::ShowPagePgNum(USHORT nPgNum, const Point& rOffs)
{
    return ShowPage(pMod->GetPage(nPgNum),rOffs);
}

SdrPageView* SdrPaintView::ShowMasterPagePgNum(USHORT nPgNum, const Point& rOffs)
{
    return ShowPage(pMod->GetMasterPage(nPgNum),rOffs);
}

void SdrPaintView::HidePage(SdrPageView* pPV)
{
    if (pPV!=NULL) {
        ULONG nPos=aPagV.GetPos(pPV);
        if (nPos!=CONTAINER_ENTRY_NOTFOUND) {
            aPagV.Remove(nPos);
            pPV->Hide();
            delete pPV;
        }
    }
}

void SdrPaintView::HidePagePgNum(USHORT nPgNum)
{
    HidePage(pMod->GetPage(nPgNum));
}

void SdrPaintView::HideAllPages()
{
    while (GetPageViewCount()>0) HidePagePvNum(0);
}

void SdrPaintView::SetPagePos(SdrPageView* pPV, const Point& rOffs)
{
    if (pPV!=NULL) {
        pPV->SetOffset(rOffs);
    }
}

void SdrPaintView::SetPagePosPgNum(USHORT nPgNum, const Point& rOffs)
{
    SetPagePos(pMod->GetPage(nPgNum),rOffs);
}

SdrPageView* SdrPaintView::GetPageView(const SdrPage* pPage) const
{
    if (pPage==NULL) return NULL;
    BOOL bWeiter=TRUE;
    SdrPageView* pPV=NULL;
    for (USHORT i=0; i<GetPageViewCount() && bWeiter; i++) {
        pPV=GetPageViewPvNum(i);
        bWeiter=(pPV->GetPage()!=pPage);
    }
    if (bWeiter) return NULL;
    else return pPV;
}

SdrPageView* SdrPaintView::GetPageViewPgNum(USHORT nPgNum) const
{
    return GetPageView(pMod->GetPage(nPgNum));
}

SdrPageView* SdrPaintView::GetPageView(const Point& rPnt) const
{
    SdrPageView* pHit=NULL;
    SdrPageView* pBest=GetPageViewPvNum(0);
    ULONG        nBest=0xFFFFFFFF;

    for (USHORT i=GetPageViewCount(); i>0 && pHit==NULL;) {
        i--;
        SdrPageView* pPV=GetPageViewPvNum(i);
        Rectangle aRect(pPV->GetPageRect());
        if (aRect.IsInside(rPnt)) {
            pHit=pPV;
            pBest=pHit;
        } else {
            ULONG dx=0,dy=0;
            if (rPnt.X()<aRect.Left  ()) dx=ULONG(aRect.Left  ()-rPnt.X());
            if (rPnt.Y()<aRect.Top   ()) dy=ULONG(aRect.Top   ()-rPnt.Y());
            if (rPnt.X()>aRect.Right ()) dx=ULONG(rPnt.X()-aRect.Left  ());
            if (rPnt.Y()>aRect.Bottom()) dy=ULONG(rPnt.Y()-aRect.Bottom());
            ULONG nDist=dx+dy;
            if (nDist<nBest) {
                nBest=nDist;
                pBest=pPV;
            }
        }
    }
    return pBest;
}

USHORT SdrPaintView::GetPageViewNum(const SdrPageView* pPV) const
{
    if (pPV==NULL) return 0xFFFF;
    ULONG nNum=aPagV.GetPos(pPV);
    if (nNum==CONTAINER_ENTRY_NOTFOUND) nNum=0xFFFF;
    return USHORT(nNum);
}

SdrPageView* SdrPaintView::HitPage(const Point& rPnt) const
{
    SdrPageView* pHit=NULL;
    for (USHORT i=GetPageViewCount(); i>0 && pHit==NULL;) {
        i--;
        SdrPageView* pPV=GetPageViewPvNum(i);
        if (pPV->GetPageRect().IsInside(rPnt)) pHit=pPV;
    }
    return pHit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrPaintView::AddWin(OutputDevice* pWin1)
{
    SdrViewWinRec* pWinRec=new SdrViewWinRec(pWin1);
    pWinRec->bXorVisible=TRUE; // Normalerweise ist Xor nicht unterdrueckt
    aWinList.Insert(pWinRec);
    for (USHORT i=0; i<GetPageViewCount(); i++)
    {
        // GetPageViewPvNum(i)->AddWin(pWin1);
        DBG_ASSERT(pWin1, "SdrPaintView::AddWin with empty window (?)");
        GetPageViewPvNum(i)->AddWindowToPageView(*pWin1);
    }
#ifndef SVX_LIGHT
    if (pItemBrowser!=NULL) pItemBrowser->ForceParent();
#endif
}

void SdrPaintView::DelWin(OutputDevice* pWin1)
{
    USHORT nPos=aWinList.Find(pWin1);
    if (nPos!=SDRVIEWWIN_NOTFOUND) {
        for (USHORT i=0; i<GetPageViewCount(); i++)
        {
            // GetPageViewPvNum(i)->DelWin(pWin1);
            DBG_ASSERT(pWin1, "SdrPaintView::DelWin with empty window (?)");
            GetPageViewPvNum(i)->DeleteWindowFromPageView(*pWin1);
        }
        aWinList.Delete(nPos);
    }
#ifndef SVX_LIGHT
    if (pItemBrowser!=NULL) pItemBrowser->ForceParent();
#endif
}

Rectangle SdrPaintView::GetVisibleArea( USHORT nNum )
{
    OutputDevice* pWin = GetWin(nNum);

    if( pWin )
    {
        // get visible area
        Size aVisSizePixel( pWin->GetOutputSizePixel() );
        return Rectangle( pWin->PixelToLogic(Rectangle(Point(0,0), aVisSizePixel)) );
    }

    return Rectangle();
}

void SdrPaintView::SetLayerVisible(const XubString& rName, BOOL bShow)
{
    USHORT i;
    for (i=0; i<GetPageViewCount(); i++) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        pPV->SetLayerVisible(rName,bShow);
    }
    InvalidateAllWin();
}

TRISTATE SdrPaintView::IsLayerVisible(const XubString& rName) const
{
    TRISTATE nRet=FALSE;
    USHORT i=0;
    BOOL b1st=TRUE;
    while (i<GetPageViewCount() && nRet!=FUZZY) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        BOOL bOn=pPV->IsLayerVisible(rName);
        if (b1st) {
            nRet=bOn;
            b1st=FALSE;
        } else {
            if (nRet!=bOn) nRet=FUZZY;
        }
        i++;
    }
    return nRet;
}

void SdrPaintView::SetAllLayersVisible(BOOL bShow)
{
    USHORT i;
    for (i=0; i<GetPageViewCount(); i++) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        pPV->SetAllLayersVisible(bShow);
    }
    InvalidateAllWin();
}

void SdrPaintView::SetLayerLocked(const XubString& rName, BOOL bLock)
{
    USHORT i;
    for (i=0; i<GetPageViewCount(); i++) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        pPV->SetLayerLocked(rName,bLock);
    }
}

TRISTATE SdrPaintView::IsLayerLocked(const XubString& rName) const
{
    TRISTATE nRet=FALSE;
    USHORT i=0;
    BOOL b1st=TRUE;
    while (i<GetPageViewCount() && nRet!=FUZZY) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        BOOL bLock=pPV->IsLayerLocked(rName);
        if (b1st) {
            nRet=bLock;
            b1st=FALSE;
        } else {
            if (nRet!=bLock) nRet=FUZZY;
        }
        i++;
    }
    return nRet;
}

void SdrPaintView::SetAllLayersLocked(BOOL bLock)
{
    USHORT i;
    for (i=0; i<GetPageViewCount(); i++) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        pPV->SetAllLayersLocked(bLock);
    }
}

void SdrPaintView::SetLayerPrintable(const XubString& rName, BOOL bPrn)
{
    USHORT i;
    for (i=0; i<GetPageViewCount(); i++) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        pPV->SetLayerPrintable(rName,bPrn);
    }
}

TRISTATE SdrPaintView::IsLayerPrintable(const XubString& rName) const
{
    TRISTATE nRet=FALSE;
    USHORT i=0;
    BOOL b1st=TRUE;
    while (i<GetPageViewCount() && nRet!=FUZZY) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        BOOL bPrn=pPV->IsLayerPrintable(rName);
        if (b1st) {
            nRet=bPrn;
            b1st=FALSE;
        } else {
            if (nRet!=bPrn) nRet=FUZZY;
        }
        i++;
    }
    return nRet;
}

void SdrPaintView::SetAllLayersPrintable(BOOL bPrn)
{
    for (USHORT nv=0; nv<GetPageViewCount(); nv++) {
        SdrPageView* pPV=GetPageViewPvNum(nv);
        pPV->SetAllLayersPrintable(bPrn);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrPaintView::CompleteRedraw(OutputDevice* pOut, const Region& rReg, USHORT nPaintMode,
    ::sdr::contact::ViewObjectContactRedirector* pRedirector)
{
    for (USHORT i=0; i<GetPageViewCount(); i++) {
        SdrPageView* pPV=GetPageViewPvNum(i);
        pPV->CompleteRedraw(pOut, rReg, nPaintMode, pRedirector);
    }
    USHORT nWinNum=aWinList.Find(pOut);
    if (nWinNum!=SDRVIEWWIN_NOTFOUND) {
        if (IsShownXorVisibleWinNum(nWinNum)) { // Durch Invalidate zerstoerte Handles wiederherstellen
            OutputDevice* pOut=GetWin(nWinNum);
            if (pOut!=NULL && pOut->GetOutDevType()!=OUTDEV_PRINTER) {
                ToggleShownXor(pOut,&rReg);
            }
        }
    }

    // #111097#
    RestartAfterPaintTimer();
}

B2dIAOManager* SdrPaintView::GetIAOManager(OutputDevice* pOut) const
{
    if(pOut)
    {
        for(sal_uInt16 a(0); a < GetPageViewCount(); a++)
        {
            SdrPageView* pPageView = GetPageViewPvNum(a);

            for(sal_uInt32 b(0L); b < pPageView->WindowCount(); b++)
            {
                // const SdrPageViewWinRec& rPageViewWinRec = rPageViewWinList[b];
                const SdrPageViewWindow& rPageViewWindow = *pPageView->GetWindow(b);
                OutputDevice& rOutDev = rPageViewWindow.GetOutputDevice();

                if(&rOutDev == pOut)
                {
                    return rPageViewWindow.GetIAOManager();
                }
            }
        }
    }

    return NULL;
}

B2dIAOManager* SdrPaintView::GetFirstIAOManager() const
{
    if(GetPageViewCount())
    {
        SdrPageView* pPageView = GetPageViewPvNum(0);

        if(pPageView->WindowCount())
        {
            const SdrPageViewWindow& rPageViewWindow = *pPageView->GetWindow(0);
            return rPageViewWindow.GetIAOManager();
        }
    }

    return 0L;
}

void SdrPaintView::RefreshAllIAOManagers() const
{
    for(sal_uInt16 a(0); a < GetPageViewCount(); a++)
    {
        SdrPageView* pPageView = GetPageViewPvNum(a);

        for(sal_uInt32 b(0L); b < pPageView->WindowCount(); b++)
        {
            const SdrPageViewWindow& rPageViewWindow = *pPageView->GetWindow(b);

            if(rPageViewWindow.GetIAOManager())
            {
                rPageViewWindow.GetIAOManager()->UpdateDisplay();
            }
        }
    }
}

BOOL SdrPaintView::KeyInput(const KeyEvent& rKEvt, Window* pWin)
{
    return FALSE;
}

// #111097#
void SdrPaintView::RestartAfterPaintTimer()
{
    maAfterPaintTimer.Start();
}

void SdrPaintView::GlueInvalidate() const
{
    USHORT nPvAnz=GetPageViewCount();
    USHORT nWinAnz=GetWinCount();
    for (USHORT nWinNum=0; nWinNum<nWinAnz; nWinNum++) {
        OutputDevice* pOut=GetWin(nWinNum);
        if (pOut->GetOutDevType()==OUTDEV_WINDOW) {
            pXOut->SetOutDev(pOut);
            for (USHORT nPvNum=0; nPvNum<nPvAnz; nPvNum++) {
                const SdrPageView* pPV=GetPageViewPvNum(nPvNum);
                const SdrObjList* pOL=pPV->GetObjList();
                pXOut->SetOffset(pPV->GetOffset());
                ULONG nObjAnz=pOL->GetObjCount();
                for (ULONG nObjNum=0; nObjNum<nObjAnz; nObjNum++) {
                    const SdrObject* pObj=pOL->GetObj(nObjNum);
                    const SdrGluePointList* pGPL=pObj->GetGluePointList();
                    if (pGPL!=NULL && pGPL->GetCount()!=0) {
                        pGPL->Invalidate(*(Window*)pOut,pObj);
                    }
                }
            }
        }
        pXOut->SetOffset(Point(0,0));
    }
}

void SdrPaintView::InvalidateAllWin()
{
    for (USHORT i=0; i<GetWinCount(); i++) {
        OutputDevice* pOut=GetWin(i);
        if (pOut->GetOutDevType()==OUTDEV_WINDOW) {
            InvalidateOneWin(*(Window*)pOut);
        }
    }
}

void SdrPaintView::InvalidateAllWin(const Rectangle& rRect, BOOL bPlus1Pix)
{
    USHORT nWinAnz=GetWinCount();
    for (USHORT i=0; i<nWinAnz; i++) {
        OutputDevice* pOut=GetWin(i);
        if (pOut->GetOutDevType()==OUTDEV_WINDOW) {
            Rectangle aRect(rRect);
            if (bPlus1Pix) {
                Size aPixSiz(1,1);
                Size aSiz(pOut->PixelToLogic(aPixSiz));
                aRect.Left  ()-=aSiz.Width();
                aRect.Top   ()-=aSiz.Height();
                aRect.Right ()+=aSiz.Width();
                aRect.Bottom()+=aSiz.Height();
            }
            Point aOrg(pOut->GetMapMode().GetOrigin());
            aOrg.X()=-aOrg.X(); aOrg.Y()=-aOrg.Y();
            Rectangle aOutRect(aOrg,pOut->GetOutputSize());
            if (aRect.IsOver(aOutRect)) {
                InvalidateOneWin(*(Window*)pOut,aRect);
            }
        }
    }
}

void SdrPaintView::InvalidateOneWin(Window& rWin)
{
    // #111096#
    // do not erase background, that causes flicker (!)
    rWin.Invalidate(INVALIDATE_NOERASE);
}

void SdrPaintView::InvalidateOneWin(Window& rWin, const Rectangle& rRect)
{
    // #111096#
    // do not erase background, that causes flicker (!)
    rWin.Invalidate(rRect, INVALIDATE_NOERASE);
}

void SdrPaintView::LeaveOneGroup()
{
    for (USHORT nv=0; nv<GetPageViewCount(); nv++) {
        SdrPageView* pPV=GetPageViewPvNum(nv);
        pPV->LeaveOneGroup();
    }
}

void SdrPaintView::LeaveAllGroup()
{
    for (USHORT nv=0; nv<GetPageViewCount(); nv++) {
        SdrPageView* pPV=GetPageViewPvNum(nv);
        pPV->LeaveAllGroup();
    }
}

BOOL SdrPaintView::IsGroupEntered() const
{
    BOOL bRet=FALSE;
    USHORT nPVAnz=GetPageViewCount();
    for (USHORT nv=0; nv<nPVAnz && !bRet; nv++) {
        SdrPageView* pPV=GetPageViewPvNum(nv);
        if (pPV->GetEnteredLevel()!=0) bRet=TRUE;
    }
    return bRet;
}

void SdrPaintView::SetNotPersistDefaultAttr(const SfxItemSet& rAttr, BOOL bReplaceAll)
{
    // bReplaceAll hat hier keinerlei Wirkung
    BOOL bMeasure=ISA(SdrView) && ((SdrView*)this)->IsMeasureTool();
    const SfxPoolItem *pPoolItem=NULL;
    if (rAttr.GetItemState(SDRATTR_LAYERID,TRUE,&pPoolItem)==SFX_ITEM_SET) {
        SdrLayerID nLayerId=((const SdrLayerIdItem*)pPoolItem)->GetValue();
        const SdrLayer* pLayer=pMod->GetLayerAdmin().GetLayerPerID(nLayerId);
        if (pLayer!=NULL) {
            if (bMeasure) aMeasureLayer=pLayer->GetName();
            else aAktLayer=pLayer->GetName();
        }
    }
    if (rAttr.GetItemState(SDRATTR_LAYERNAME,TRUE,&pPoolItem)==SFX_ITEM_SET) {
        if (bMeasure) aMeasureLayer=((const SdrLayerNameItem*)pPoolItem)->GetValue();
        else aAktLayer=((const SdrLayerNameItem*)pPoolItem)->GetValue();
    }
}

void SdrPaintView::MergeNotPersistDefaultAttr(SfxItemSet& rAttr, BOOL bOnlyHardAttr) const
{
    // bOnlyHardAttr hat hier keinerlei Wirkung
    BOOL bMeasure=ISA(SdrView) && ((SdrView*)this)->IsMeasureTool();
    const XubString& aNam=bMeasure?aMeasureLayer:aAktLayer;
    rAttr.Put(SdrLayerNameItem(aNam));
    SdrLayerID nLayer=pMod->GetLayerAdmin().GetLayerID(aNam,TRUE);
    if (nLayer!=SDRLAYER_NOTFOUND) {
        rAttr.Put(SdrLayerIdItem(nLayer));
    }
}

void SdrPaintView::SetDefaultAttr(const SfxItemSet& rAttr, BOOL bReplaceAll)
{
#ifdef DBG_UTIL
    {
        BOOL bHasEEFeatureItems=FALSE;
        SfxItemIter aIter(rAttr);
        const SfxPoolItem* pItem=aIter.FirstItem();
        while (!bHasEEFeatureItems && pItem!=NULL) {
            if (!IsInvalidItem(pItem)) {
                USHORT nW=pItem->Which();
                if (nW>=EE_FEATURE_START && nW<=EE_FEATURE_END) bHasEEFeatureItems=TRUE;
            }
            pItem=aIter.NextItem();
        }

        if(bHasEEFeatureItems)
        {
            String aMessage;
            aMessage.AppendAscii("SdrPaintView::SetDefaultAttr(): Das setzen von EE_FEATURE-Items an der SdrView macht keinen Sinn! Es fuehrt nur zu Overhead und nicht mehr lesbaren Dokumenten.");
            InfoBox(NULL, aMessage).Execute();
        }
    }
#endif
    if (bReplaceAll) aDefaultAttr.Set(rAttr);
    else aDefaultAttr.Put(rAttr,FALSE); // FALSE= InvalidItems nicht als Default, sondern als "Loecher" betrachten
    SetNotPersistDefaultAttr(rAttr,bReplaceAll);
#ifndef SVX_LIGHT
    if (pItemBrowser!=NULL) pItemBrowser->SetDirty();
#endif
}

void SdrPaintView::SetDefaultStyleSheet(SfxStyleSheet* pStyleSheet, BOOL bDontRemoveHardAttr)
{
    pDefaultStyleSheet=pStyleSheet;
    if (pStyleSheet!=NULL && !bDontRemoveHardAttr) {
        SfxWhichIter aIter(pStyleSheet->GetItemSet());
        USHORT nWhich=aIter.FirstWhich();
        while (nWhich!=0) {
            if (pStyleSheet->GetItemSet().GetItemState(nWhich,TRUE)==SFX_ITEM_SET) {
                aDefaultAttr.ClearItem(nWhich);
            }
            nWhich=aIter.NextWhich();
        }
    }
#ifndef SVX_LIGHT
    if (pItemBrowser!=NULL) pItemBrowser->SetDirty();
#endif
}

/* new interface src537 */
BOOL SdrPaintView::GetAttributes(SfxItemSet& rTargetSet, BOOL bOnlyHardAttr) const
{
    if(bOnlyHardAttr || !pDefaultStyleSheet)
    {
        rTargetSet.Put(aDefaultAttr, FALSE);
    }
    else
    {
        // sonst DefStyleSheet dazumergen
        rTargetSet.Put(pDefaultStyleSheet->GetItemSet(), FALSE);
        rTargetSet.Put(aDefaultAttr, FALSE);
    }
    MergeNotPersistDefaultAttr(rTargetSet, bOnlyHardAttr);
    return TRUE;
}

BOOL SdrPaintView::SetAttributes(const SfxItemSet& rSet, BOOL bReplaceAll)
{
    SetDefaultAttr(rSet,bReplaceAll);
    return TRUE;
}

SfxStyleSheet* SdrPaintView::GetStyleSheet(BOOL& rOk) const
{
    rOk=TRUE;
    return GetDefaultStyleSheet();
}

BOOL SdrPaintView::SetStyleSheet(SfxStyleSheet* pStyleSheet, BOOL bDontRemoveHardAttr)
{
    SetDefaultStyleSheet(pStyleSheet,bDontRemoveHardAttr);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrPaintView::ShowItemBrowser(BOOL bShow)
{
#ifndef SVX_LIGHT
    if (bShow) {
        if (pItemBrowser==NULL) {
            pItemBrowser=new SdrItemBrowser(*(SdrView*)this);
            pItemBrowser->SetFloatingMode(TRUE);
        }
        pItemBrowser->Show();
        pItemBrowser->GrabFocus();
    } else {
        if (pItemBrowser!=NULL) {
            pItemBrowser->Hide();
            delete pItemBrowser;
            pItemBrowser=NULL;
        }
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//BFS01void SdrPaintView::WriteRecords(SvStream& rOut) const
//BFS01{
//BFS01 {
//BFS01     SdrNamedSubRecord aSubRecord(rOut,STREAM_WRITE,SdrInventor,SDRIORECNAME_VIEWPAGEVIEWS);
//BFS01     USHORT nv;
//BFS01     for (nv=0; nv<GetPageViewCount(); nv++) {
//BFS01         SdrPageView* pPV=GetPageViewPvNum(nv);
//BFS01         if (pPV->GetPage()->IsInserted()) {
//BFS01             rOut<<*pPV;
//BFS01         }
//BFS01     }
//BFS01 } {
//BFS01     SdrNamedSubRecord aSubRecord(rOut,STREAM_WRITE,SdrInventor,SDRIORECNAME_VIEWVISIELEM);
//BFS01     rOut<<BOOL(sal_False /*bLayerSortedRedraw*/);
//BFS01     rOut<<BOOL(bPageVisible);
//BFS01     rOut<<BOOL(bBordVisible);
//BFS01     rOut<<BOOL(bGridVisible);
//BFS01     rOut<<BOOL(bGridFront);
//BFS01     rOut<<BOOL(bHlplVisible);
//BFS01     rOut<<BOOL(bHlplFront);
//BFS01     rOut<<BOOL(bGlueVisible);
//BFS01     rOut<<aGridBig;
//BFS01     rOut<<aGridFin;
//BFS01     rOut<<aGridWdtX;
//BFS01     rOut<<aGridWdtY;
//BFS01     rOut<<aGridSubdiv;
//BFS01 } {
//BFS01     SdrNamedSubRecord aSubRecord(rOut,STREAM_WRITE,SdrInventor,SDRIORECNAME_VIEWAKTLAYER);
//BFS01
//BFS01     // UNICODE: rOut << aAktLayer;
//BFS01     rOut.WriteByteString(aAktLayer);
//BFS01
//BFS01     // UNICODE: rOut << aMeasureLayer;
//BFS01     rOut.WriteByteString(aMeasureLayer);
//BFS01 }
//BFS01}

////////////////////////////////////////////////////////////////////////////////////////////////////

//BFS01BOOL SdrPaintView::ReadRecord(const SdrIOHeader& rViewHead,
//BFS01 const SdrNamedSubRecord& rSubHead,
//BFS01 SvStream& rIn)
//BFS01{
//BFS01 BOOL bRet=FALSE;
//BFS01 if (rSubHead.GetInventor()==SdrInventor) {
//BFS01     bRet=TRUE;
//BFS01     switch (rSubHead.GetIdentifier()) {
//BFS01         case SDRIORECNAME_VIEWPAGEVIEWS: {
//BFS01             while (rSubHead.GetBytesLeft()>0 && rIn.GetError()==0 && !rIn.IsEof()) {
//BFS01                 SdrPageView* pPV=new SdrPageView(NULL,Point(),*(SdrView*)this);
//BFS01                 rIn>>*pPV;
//BFS01                 if (pPV->GetPage()!=NULL) {
//BFS01                     if (pPV->IsVisible()) {
//BFS01                         aPagV.Insert(pPV,CONTAINER_APPEND);
//BFS01                     } else {
//BFS01                         // aPagHide.Insert(pPV,CONTAINER_APPEND);
//BFS01                         delete pPV;
//BFS01                     }
//BFS01                 } else {
//BFS01                     DBG_ERROR("SdrPaintView::ReadRecord(): Seite der PageView nicht gefunden");
//BFS01                     delete pPV;
//BFS01                 }
//BFS01             }
//BFS01         } break;
//BFS01
//BFS01         case SDRIORECNAME_VIEWVISIELEM:
//BFS01         {
//BFS01             BOOL bTemp;
//BFS01
//BFS01             rIn>>bTemp; // bLayerSortedRedraw=bTemp;
//BFS01             rIn>>bTemp; bPageVisible      =bTemp;
//BFS01             rIn>>bTemp; bBordVisible      =bTemp;
//BFS01             rIn>>bTemp; bGridVisible      =bTemp;
//BFS01             rIn>>bTemp; bGridFront        =bTemp;
//BFS01             rIn>>bTemp; bHlplVisible      =bTemp;
//BFS01             rIn>>bTemp; bHlplFront        =bTemp;
//BFS01             rIn>>bTemp; bGlueVisible      =bTemp;
//BFS01             rIn>>aGridBig;
//BFS01             rIn>>aGridFin;
//BFS01
//BFS01             if(rSubHead.GetBytesLeft() > 0)
//BFS01             {
//BFS01                 rIn >> aGridWdtX;
//BFS01                 rIn >> aGridWdtY;
//BFS01                 rIn >> aGridSubdiv;
//BFS01             }
//BFS01             break;
//BFS01         }
//BFS01         case SDRIORECNAME_VIEWAKTLAYER:
//BFS01         {
//BFS01             // UNICODE: rIn >> aAktLayer;
//BFS01             rIn.ReadByteString(aAktLayer);
//BFS01
//BFS01             if(rSubHead.GetBytesLeft() > 0)
//BFS01             {
//BFS01                 // UNICODE: rIn >> aMeasureLayer;
//BFS01                 rIn.ReadByteString(aMeasureLayer);
//BFS01             }
//BFS01             break;
//BFS01         }
//BFS01
//BFS01         default:
//BFS01             bRet = FALSE;
//BFS01     }
//BFS01 }
//BFS01 return bRet;
//BFS01}

void SdrPaintView::MakeVisible(const Rectangle& rRect, Window& rWin)
{
    MapMode aMap(rWin.GetMapMode());
    Size aActualSize(rWin.GetOutputSize());

    if( aActualSize.Height() > 0 && aActualSize.Width() > 0 )
    {
        Size aNewSize(rRect.GetSize());
        BOOL bNewScale=FALSE;
        BOOL bNeedMoreX=aNewSize.Width()>aActualSize.Width();
        BOOL bNeedMoreY=aNewSize.Height()>aActualSize.Height();
        if (bNeedMoreX || bNeedMoreY)
        {
            bNewScale=TRUE;
            // Neuen MapMode (Size+Org) setzen und dabei alles invalidieren
            Fraction aXFact(aNewSize.Width(),aActualSize.Width());
            Fraction aYFact(aNewSize.Height(),aActualSize.Height());
            if (aYFact>aXFact) aXFact=aYFact;
            aXFact*=aMap.GetScaleX();
            aXFact.ReduceInaccurate(10); // Um Ueberlaeufe und BigInt-Mapping zu vermeiden
            aMap.SetScaleX(aXFact);
            aMap.SetScaleY(aYFact);
            rWin.SetMapMode(aMap);
            aActualSize=rWin.GetOutputSize();
        }
        Point aOrg(aMap.GetOrigin());
        long dx=0,dy=0;
        long l=-aOrg.X();
        long r=-aOrg.X()+aActualSize.Width()-1;
        long o=-aOrg.Y();
        long u=-aOrg.Y()+aActualSize.Height()-1;
        if (l>rRect.Left()) dx=rRect.Left()-l;
        else if (r<rRect.Right()) dx=rRect.Right()-r;
        if (o>rRect.Top()) dy=rRect.Top()-o;
        else if (u<rRect.Bottom()) dy=rRect.Bottom()-u;
        aMap.SetOrigin(Point(aOrg.X()-dx,aOrg.Y()-dy));
        if (!bNewScale) {
            if (dx!=0 || dy!=0) {
                BOOL bXor=IsShownXorVisible(&rWin);
                if (bXor) HideShownXor(&rWin);
                rWin.Scroll(-dx,-dy);
                rWin.SetMapMode(aMap);
                rWin.Update();
                if (bXor) ShowShownXor(&rWin);
            }
        } else {
            rWin.SetMapMode(aMap);
            InvalidateOneWin(rWin);
        }
    }
}

void SdrPaintView::DoConnect(SdrOle2Obj* pOleObj)
{
}

void SdrPaintView::SetAnimationEnabled( BOOL bEnable )
{
    SetAnimationMode( bEnable ? SDR_ANIMATION_ANIMATE : SDR_ANIMATION_DISABLE );
}

void SdrPaintView::SetAnimationPause( BOOL bSet )
{
    if(bAnimationPause != bSet)
    {
        bAnimationPause = bSet;

        for(sal_uInt16 a(0); a < GetPageViewCount(); a++)
        {
            SdrPageView& rPageView = *GetPageViewPvNum(a);

            for(sal_uInt32 b(0L); b < rPageView.WindowCount(); b++)
            {
                const SdrPageViewWindow& rPageViewWindow = *(rPageView.GetWindow(b));
                sdr::contact::ObjectContact& rObjectContact = rPageViewWindow.GetObjectContact();

                if(rObjectContact.HasObjectAnimator())
                {
                    sdr::animation::ObjectAnimator& rAnimator = rObjectContact.GetObjectAnimator();

                    if(rAnimator.IsPaused() != bSet)
                    {
                        rAnimator.SetPaused(bSet);
                    }
                }
            }
        }
    }
}

void SdrPaintView::SetAnimationMode( const SdrAnimationMode eMode )
{
    eAnimationMode = eMode;
}

void SdrPaintView::VisAreaChanged(const OutputDevice* pOut)
{
    USHORT nCount = GetPageViewCount();

    for (USHORT i = 0; i < nCount; i++)
    {
        SdrPageView* pPV = GetPageViewPvNum(i);

        if (pOut)
        {
            // const SdrPageViewWindows& rPageViewWindows = pPV->GetPageViewWindows();
            SdrPageViewWindow* pWindow = pPV->FindWindow(*((OutputDevice*)pOut));

            if(pWindow)
            {
                VisAreaChanged(*pWindow);
            }
        }
        else
        {
            // const SdrPageViewWindows& rPageViewWindows = pPV->GetPageViewWindows();

            for(sal_uInt32 a(0L); a < pPV->WindowCount(); a++)
            {
                VisAreaChanged(*pPV->GetWindow(a));
            }
        }
    }
}

void SdrPaintView::VisAreaChanged(const SdrPageViewWindow& rWindow)
{
    // notify SfxListener
    Broadcast(SvxViewHint(SVX_HINT_VIEWCHANGED));

    // notify UNO objects
    if(rWindow.GetControlContainerRef().is())
    {
        const SdrUnoControlList& rCList = rWindow.GetControlList();

        for (ULONG i = 0; i < rCList.GetCount(); i++)
        {
            const SdrUnoControlRec& rControlRec = rCList[ (sal_uInt16) i];
            uno::Reference< awt::XControl > xUnoControl = rControlRec.GetControl();

            if (xUnoControl.is())
            {
                uno::Reference< awt::XWindow > xVclComponent( xUnoControl, uno::UNO_QUERY );

                if (xVclComponent.is() && rControlRec.GetUnoObj())
                {
                    Rectangle aRect = rControlRec.GetUnoObj()->GetLogicRect();
                    OutputDevice& rOut = rWindow.GetOutputDevice();
                    Point aPixPos(rOut.LogicToPixel(aRect.TopLeft()));
                    Size aPixSize(rOut.LogicToPixel(aRect.GetSize()));

                    xVclComponent->setPosSize(aPixPos.X(), aPixPos.Y(),
                        aPixSize.Width(), aPixSize.Height(), awt::PosSize::POSSIZE);
                }
            }
        }
    }
}

const svtools::ColorConfig& SdrPaintView::getColorConfig() const
{
    return maColorConfig;
}

void SdrPaintView::onChangeColorConfig()
{
    SetGridColor( Color( maColorConfig.GetColorValue( svtools::DRAWGRID ).nColor ) );
}

void SdrPaintView::SetGridColor( Color aColor )
{
    maGridColor = aColor;
}

Color SdrPaintView::GetGridColor() const
{
    return maGridColor;
}

// #103834# Set background color for svx at SdrPageViews
void SdrPaintView::SetApplicationBackgroundColor(Color aBackgroundColor)
{
    for(sal_uInt16 a(0); a < GetPageViewCount(); a++)
    {
        SdrPageView* pPageView = GetPageViewPvNum(a);
        pPageView->SetApplicationBackgroundColor(aBackgroundColor);
    }
}

// #103911# Set document color for svx at SdrPageViews
void SdrPaintView::SetApplicationDocumentColor(Color aDocumentColor)
{
    for(sal_uInt16 a(0); a < GetPageViewCount(); a++)
    {
        SdrPageView* pPageView = GetPageViewPvNum(a);
        pPageView->SetApplicationDocumentColor(aDocumentColor);
    }
}

// declaration extracted from svdedxv.cxx
#define SPOTCOUNT   5

Color SdrPaintView::CalcBackgroundColor( const Rectangle& rArea,
                                         const SetOfByte& rVisibleLayers,
                                         const SdrPage&   rCurrPage ) const
{
    // code extracted from SdrObjEditView::ImpGetTextEditBackgroundColor
    svtools::ColorConfig aColorConfig;
    Color aBackground(aColorConfig.GetColorValue(svtools::DOCCOLOR).nColor);

    // #98988# test if we are in High contrast mode; if yes, take
    // application background color
    // #10049# wrong, always use svtools::DOCCOLOR as default and use document settings if
    //         not hc mode
    const StyleSettings& rStyleSettings = Application::GetSettings().GetStyleSettings();

    if(!rStyleSettings.GetHighContrastMode())
    {
        // Ok, dann eben die Page durchsuchen!
        Point aSpotPos[SPOTCOUNT];
        Color aSpotColor[SPOTCOUNT];
        ULONG nHeight( rArea.GetSize().Height() );
        ULONG nWidth( rArea.GetSize().Width() );
        ULONG nWidth14  = nWidth / 4;
        ULONG nHeight14 = nHeight / 4;
        ULONG nWidth34  = ( 3 * nWidth ) / 4;
        ULONG nHeight34 = ( 3 * nHeight ) / 4;

        USHORT i;
        for ( i = 0; i < SPOTCOUNT; i++ )
        {
            // Es wird anhand von fuenf Spots die Farbe untersucht
            switch ( i )
            {
                case 0 :
                {
                    // Center-Spot
                    aSpotPos[i] = rArea.Center();
                }
                break;

                case 1 :
                {
                    // TopLeft-Spot
                    aSpotPos[i] = rArea.TopLeft();
                    aSpotPos[i].X() += nWidth14;
                    aSpotPos[i].Y() += nHeight14;
                }
                break;

                case 2 :
                {
                    // TopRight-Spot
                    aSpotPos[i] = rArea.TopLeft();
                    aSpotPos[i].X() += nWidth34;
                    aSpotPos[i].Y() += nHeight14;
                }
                break;

                case 3 :
                {
                    // BottomLeft-Spot
                    aSpotPos[i] = rArea.TopLeft();
                    aSpotPos[i].X() += nWidth14;
                    aSpotPos[i].Y() += nHeight34;
                }
                break;

                case 4 :
                {
                    // BottomRight-Spot
                    aSpotPos[i] = rArea.TopLeft();
                    aSpotPos[i].X() += nWidth34;
                    aSpotPos[i].Y() += nHeight34;
                }
                break;

            }

            aSpotColor[i] = Color( COL_WHITE );
            rCurrPage.GetFillColor(aSpotPos[i], rVisibleLayers, /*bLayerSortedRedraw,*/ aSpotColor[i]);
        }

        USHORT aMatch[SPOTCOUNT];

        for ( i = 0; i < SPOTCOUNT; i++ )
        {
            // Wurden gleiche Spot-Farben gefuden?
            aMatch[i] = 0;

            for ( USHORT j = 0; j < SPOTCOUNT; j++ )
            {
                if( j != i )
                {
                    if( aSpotColor[i] == aSpotColor[j] )
                    {
                        aMatch[i]++;
                    }
                }
            }
        }

        // Das hoechste Gewicht hat der Spot in der Mitte
        aBackground = aSpotColor[0];

        for ( USHORT nMatchCount = SPOTCOUNT - 1; nMatchCount > 1; nMatchCount-- )
        {
            // Welche Spot-Farbe wurde am haeufigsten gefunden?
            for ( USHORT i = 0; i < SPOTCOUNT; i++ )
            {
                if( aMatch[i] == nMatchCount )
                {
                    aBackground = aSpotColor[i];
                    nMatchCount = 1;   // Abbruch auch der aeusseren for-Schleife
                    break;
                }
            }
        }
    }

    return aBackground;
}

// #114898#
sal_Bool SdrPaintView::IsBufferedOutputAllowed() const
{
    return mbBufferedOutputAllowed;
}

// #114898#
void SdrPaintView::SetBufferedOutputAllowed(sal_Bool bNew)
{
    if(bNew != mbBufferedOutputAllowed)
    {
        mbBufferedOutputAllowed = bNew;
    }
}

//BFS09
sal_Bool SdrPaintView::IsPagePaintingAllowed() const
{
    return mbPagePaintingAllowed;
}

//BFS09
void SdrPaintView::SetPagePaintingAllowed(sal_Bool bNew)
{
    if(bNew != mbPagePaintingAllowed)
    {
        mbPagePaintingAllowed = bNew;
    }
}

// #i32773#
// empty implementation, nothing known about handles here
void SdrPaintView::ForceInvalidateMarkHandles()
{
}

// #i38135# Sets the timer for Object animations and restarts.
void SdrPaintView::SetAnimationTimer(sal_uInt32 nTime)
{
    SdrPageView* pPageView = GetPageViewPvNum(0);

    if(pPageView)
    {
        // first, reset all timers at all windows to 0L
        for(sal_uInt32 a(0L); a < pPageView->WindowCount(); a++)
        {
            const SdrPageViewWindow& rPageViewWindow = *pPageView->GetWindow(a);
            sdr::contact::ObjectContact& rObjectContact = rPageViewWindow.GetObjectContact();

            if(rObjectContact.HasObjectAnimator())
            {
                sdr::animation::ObjectAnimator& rAnimator = rObjectContact.GetObjectAnimator();
                rAnimator.SetTime(nTime);
            }
        }
    }
}

// eof
