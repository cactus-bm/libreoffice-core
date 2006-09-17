/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdcrtv.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 05:47:53 $
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
#include "precompiled_svx.hxx"

#include "svdcrtv.hxx"
#include "xattr.hxx"
#include "svdxout.hxx"
#include "svdundo.hxx"
#include "svdocapt.hxx" // Spezialbehandlung: Nach dem Create transparente Fuellung
#include "svdcrtmt.hxx"
#include "svdvmark.hxx"
#include "svdoedge.hxx"
#include "svdpagv.hxx"
#include "svdpage.hxx"
#include "svdetc.hxx"

#ifndef _E3D_SCENE3D_HXX
#include "scene3d.hxx"
#endif

#ifndef _E3D_VIEW3D_HXX
#include "view3d.hxx"
#endif

// #116425#
#ifndef _SDR_CONTACT_OBJECTCONTACTOFOBJLISTPAINTER_HXX
#include <svx/sdr/contact/objectcontactofobjlistpainter.hxx>
#endif

// #116425#
#ifndef _SDR_CONTACT_DISPLAYINFO_HXX
#include <svx/sdr/contact/displayinfo.hxx>
#endif

#define XOR_CREATE_PEN          PEN_SOLID

////////////////////////////////////////////////////////////////////////////////////////////////////

class ImpSdrConnectMarker: public SdrViewUserMarker
{
    const SdrObject*            pAktObj;
    const SdrPageView*          pAktPV;

    virtual void Draw(OutputDevice* pOut, FASTBOOL bHiding, FASTBOOL bNoSaveDC);

public:
    ImpSdrConnectMarker(SdrCreateView* _pView): SdrViewUserMarker(_pView),pAktObj(NULL),pAktPV(NULL) {}
    ~ImpSdrConnectMarker() {}
    void SetTargetObject(const SdrObject* pObj);
}; // svdvmark

void ImpSdrConnectMarker::Draw(OutputDevice* pOut, FASTBOOL bHiding, FASTBOOL bNoSaveDC)
{
    SdrViewUserMarker::Draw(pOut,bHiding,bNoSaveDC);

    const SdrCreateView* pView2=(SdrCreateView*)GetView();
    const SdrObject* pObj=pAktObj;
    const SdrPageView* pPV=pAktPV;

    if (pObj!=NULL && pView2!=NULL && pOut!=NULL)
    {
        RasterOp eRop0=pOut->GetRasterOp();
        BOOL bMap0=pOut->IsMapModeEnabled();
        pOut->SetRasterOp(ROP_INVERT);

        Color aOldLineColor, aOldFillColor;

        if (!bNoSaveDC)
        {
            aOldLineColor = pOut->GetLineColor();
            aOldFillColor = pOut->GetFillColor();
        }
        Point aPvOfs; if (pPV!=NULL) aPvOfs=pPV->GetOffset();
        if (pView2->IsAutoVertexConnectors())
        {
            for (USHORT i=0; i<4; i++) {
                SdrGluePoint aGluePoint(pObj->GetVertexGluePoint(i));
                aGluePoint.Draw(*pOut,pObj);
            }
        }
        if (pView2->IsAutoCornerConnectors())
        {
            for (USHORT i=0; i<4; i++)
            {
                SdrGluePoint aGluePoint(pObj->GetVertexGluePoint(i));
                aGluePoint.Draw(*pOut,pObj);
            }
        }
        if (!bNoSaveDC)
        {
            pOut->SetLineColor( aOldLineColor );
            if ( aOldFillColor != pOut->GetFillColor() )
                pOut->SetFillColor( aOldFillColor );
        }
        pOut->SetRasterOp(eRop0);
        pOut->EnableMapMode(bMap0);
    }
}

void ImpSdrConnectMarker::SetTargetObject(const SdrObject* pObj)
{
    if (pAktObj!=pObj) {
        BOOL bVisible2=IsVisible();
        if (bVisible2) Hide();
        pAktObj=pObj;
        if (bVisible2) Show();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   @@@@  @@@@@  @@@@@  @@@@  @@@@@@ @@@@@  @@ @@ @@ @@@@@ @@   @@
//  @@  @@ @@  @@ @@    @@  @@   @@   @@     @@ @@ @@ @@    @@   @@
//  @@     @@  @@ @@    @@  @@   @@   @@     @@ @@ @@ @@    @@ @ @@
//  @@     @@@@@  @@@@  @@@@@@   @@   @@@@   @@@@@ @@ @@@@  @@@@@@@
//  @@     @@  @@ @@    @@  @@   @@   @@      @@@  @@ @@    @@@@@@@
//  @@  @@ @@  @@ @@    @@  @@   @@   @@      @@@  @@ @@    @@@ @@@
//   @@@@  @@  @@ @@@@@ @@  @@   @@   @@@@@    @   @@ @@@@@ @@   @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrCreateView::ImpClearVars()
{
    nAktInvent=SdrInventor;
    nAktIdent=OBJ_NONE;
    pCurrentLibObj=NULL;
    bCurrentLibObjMoveNoResize=FALSE;
    bCurrentLibObjSetDefAttr=FALSE;
    bCurrentLibObjSetDefLayer=FALSE;
    pLibObjDragMeth=NULL;
    pAktCreate=NULL;
    pCreatePV=NULL;
    bAutoTextEdit=FALSE;
    b1stPointAsCenter=FALSE;
    aAktCreatePointer=Pointer(POINTER_CROSS);
    bUseIncompatiblePathCreateInterface=FALSE;
    bAutoClosePolys=TRUE;
    nAutoCloseDistPix=5;
    nFreeHandMinDistPix=10;

    pConnectMarker=new ImpSdrConnectMarker(this);
    pConnectMarker->SetLineWidth(2);
    pConnectMarker->SetAnimateDelay(500);
    pConnectMarker->SetAnimateCount(3);
    pConnectMarker->SetAnimateSpeed(100);
    pConnectMarker->SetAnimateToggle(TRUE);
}

void SdrCreateView::ImpMakeCreateAttr()
{
}

SdrCreateView::SdrCreateView(SdrModel* pModel1, OutputDevice* pOut):
    SdrDragView(pModel1,pOut)
{
    ImpClearVars();
    ImpMakeCreateAttr();
}

SdrCreateView::SdrCreateView(SdrModel* pModel1, XOutputDevice* _pXOut):
    SdrDragView(pModel1,_pXOut)
{
    ImpClearVars();
    ImpMakeCreateAttr();
}

SdrCreateView::~SdrCreateView()
{
    delete pConnectMarker;
    delete pCurrentLibObj;
    delete pAktCreate;
}

void SdrCreateView::ImpDelCreateAttr()
{
}

BOOL SdrCreateView::IsAction() const
{
    return SdrDragView::IsAction() || pAktCreate!=NULL;
}

void SdrCreateView::MovAction(const Point& rPnt)
{
    SdrDragView::MovAction(rPnt);
    if (pAktCreate!=NULL) {
        MovCreateObj(rPnt);
    }
}

void SdrCreateView::EndAction()
{
    if (pAktCreate!=NULL) EndCreateObj(SDRCREATE_FORCEEND);
    SdrDragView::EndAction();
}

void SdrCreateView::BckAction()
{
    if (pAktCreate!=NULL) BckCreateObj();
    SdrDragView::BckAction();
}

void SdrCreateView::BrkAction()
{
    SdrDragView::BrkAction();
    BrkCreateObj();
}

void SdrCreateView::TakeActionRect(Rectangle& rRect) const
{
    if (pAktCreate!=NULL) {
        rRect=aDragStat.GetActionRect();
        if (rRect.IsEmpty()) {
            rRect=Rectangle(aDragStat.GetPrev(),aDragStat.GetNow());
        }
    } else {
        SdrDragView::TakeActionRect(rRect);
    }
}

void SdrCreateView::ToggleShownXor(OutputDevice* pOut, const Region* pRegion) const
{
    SdrDragView::ToggleShownXor(pOut,pRegion);
    if (pAktCreate!=NULL && aDragStat.IsShown()) {
        DrawCreateObj(pOut,TRUE);
    }
}

BOOL SdrCreateView::CheckEdgeMode()
{
    UINT32 nInv=nAktInvent;
    UINT16 nIdn=nAktIdent;
    if (pAktCreate!=NULL) {
        nInv=pAktCreate->GetObjInventor();
        nIdn=pAktCreate->GetObjIdentifier();
        // wird vom EdgeObj gemanaged
        if (nAktInvent==SdrInventor && nAktIdent==OBJ_EDGE) return FALSE;
    }
    if (!IsCreateMode() || nAktInvent!=SdrInventor || nAktIdent!=OBJ_EDGE || pCurrentLibObj!=NULL) {
        if (pConnectMarker->IsVisible()) pConnectMarker->Hide();
        pConnectMarker->SetTargetObject(NULL);
        return FALSE;
    } else {
        // TRUE heisst: MouseMove soll Connect checken
        return !IsAction();
    }
}

void SdrCreateView::SetConnectMarker(const SdrObjConnection& rCon, const SdrPageView& rPV)
{
    if (rCon.pObj!=NULL) {
        Rectangle aNewRect;
        USHORT nNewDist=0;
        if (rCon.bBestConn || rCon.bBestVertex) {
            aNewRect=rCon.pObj->GetCurrentBoundRect();
            nNewDist=2;
        } else {
            nNewDist=aHdl.GetHdlSize()+2;
            SdrGluePoint aGP;
            rCon.TakeGluePoint(aGP,TRUE);
            Point aPt(aGP.GetPos());
            aNewRect=Rectangle(aPt,aPt);
        }
        Point aPvOfs(rPV.GetOffset());
        aNewRect.Move(aPvOfs.X(),aPvOfs.Y());
        if (!pConnectMarker->IsVisible() ||
            pConnectMarker->GetRectangle()==NULL ||
            *pConnectMarker->GetRectangle()!=aNewRect ||
            pConnectMarker->GetPixelDistance()!=nNewDist)
        {
            pConnectMarker->Hide();
            pConnectMarker->SetRectangle(aNewRect);
            pConnectMarker->SetPixelDistance(nNewDist);
            pConnectMarker->SetAnimate(TRUE);
            pConnectMarker->SetTargetObject(rCon.pObj);
            pConnectMarker->Show();
        }
    } else {
        HideConnectMarker();
    }
}

void SdrCreateView::HideConnectMarker()
{
    pConnectMarker->Hide();
    pConnectMarker->SetTargetObject(NULL);
    pConnectMarker->SetAnimate(FALSE);
}

BOOL SdrCreateView::MouseMove(const MouseEvent& rMEvt, Window* pWin)
{
    if (CheckEdgeMode() && pWin!=NULL) {
        Point aPos(pWin->PixelToLogic(rMEvt.GetPosPixel()));
        SdrPageView* pPV=GetPageView(aPos);
        if (pPV!=NULL) {
            // Defaultete Hit-Toleranz bei IsMarkedHit() mal aendern !!!!
            BOOL bMarkHit=HitHandle(aPos,*pWin)!=NULL || IsMarkedObjHit(aPos);
            SdrObjConnection aCon;
            Point aPvOfs(pPV->GetOffset());
            aPos-=aPvOfs;
            if (!bMarkHit) SdrEdgeObj::ImpFindConnector(aPos,*pPV,aCon,NULL,pWin);
            SetConnectMarker(aCon,*pPV);
        }
    }
    return SdrDragView::MouseMove(rMEvt,pWin);
}

BOOL SdrCreateView::IsTextTool() const
{
    return eEditMode==SDREDITMODE_CREATE && pCurrentLibObj==NULL && nAktInvent==SdrInventor && (nAktIdent==OBJ_TEXT || nAktIdent==OBJ_TEXTEXT || nAktIdent==OBJ_TITLETEXT || nAktIdent==OBJ_OUTLINETEXT);
}

BOOL SdrCreateView::IsEdgeTool() const
{
    return eEditMode==SDREDITMODE_CREATE && pCurrentLibObj==NULL && nAktInvent==SdrInventor && (nAktIdent==OBJ_EDGE);
}

BOOL SdrCreateView::IsMeasureTool() const
{
    return eEditMode==SDREDITMODE_CREATE && pCurrentLibObj==NULL && nAktInvent==SdrInventor && (nAktIdent==OBJ_MEASURE);
}

void SdrCreateView::SetCurrentObj(UINT16 nIdent, UINT32 nInvent)
{
    if (pCurrentLibObj!=NULL) {
        delete pCurrentLibObj;
        pCurrentLibObj=NULL;
    }
    if (nAktInvent!=nInvent || nAktIdent!=nIdent) {
        nAktInvent=nInvent;
        nAktIdent=nIdent;
        SdrObject* pObj = SdrObjFactory::MakeNewObject(nInvent,nIdent,NULL,NULL);

        if(pObj)
        {
            // Auf pers. Wunsch von Marco:
            // Mauszeiger bei Textwerkzeug immer I-Beam. Fadenkreuz
            // mit kleinem I-Beam erst bai MouseButtonDown
            if(IsTextTool())
            {
                // #81944# AW: Here the correct pointer needs to be used
                // if the default is set to vertical writing
                aAktCreatePointer = POINTER_TEXT;
            }
            else
                aAktCreatePointer = pObj->GetCreatePointer();

            delete pObj;
        }
        else
        {
            aAktCreatePointer = Pointer(POINTER_CROSS);
        }
    }

    CheckEdgeMode();
    ImpSetGlueVisible3(IsEdgeTool());
}

void SdrCreateView::SetCurrentLibObj(SdrObject* pObj, BOOL bMoveNoResize, BOOL bSetDefAttr, BOOL bSetDefLayer)
{
    if (pCurrentLibObj!=NULL && pObj!=pCurrentLibObj) delete pCurrentLibObj;
    pCurrentLibObj=pObj;
    bCurrentLibObjMoveNoResize=bMoveNoResize;
    bCurrentLibObjSetDefAttr=bSetDefAttr;
    bCurrentLibObjSetDefLayer=bSetDefLayer;
    if (pCurrentLibObj!=NULL) {
        pCurrentLibObj->SetModel(pMod);
    }
    aAktCreatePointer=Pointer(POINTER_CROSS);
    nAktInvent=SdrInventor;
    nAktIdent=OBJ_NONE;
    pConnectMarker->Hide();
    ImpSetGlueVisible3(FALSE);
}

BOOL SdrCreateView::ImpBegCreateObj(UINT32 nInvent, UINT16 nIdent, const Point& rPnt, OutputDevice* pOut,
    short nMinMov, SdrPageView* pPV, const Rectangle& rLogRect)
{
    BOOL bRet=FALSE;
    SetSolidDraggingNow(IsSolidDragging());
    SetSolidDraggingCheck(FALSE);
    UnmarkAllObj();
    BrkAction();
    pConnectMarker->Hide();
    DBG_ASSERT(pLibObjDragMeth==NULL,"SdrCreateView::ImpBegCreateObj(): pLibObjDragMeth!=NULL");
    pLibObjDragMeth=NULL;
    if (pPV!=NULL) pCreatePV=pPV;
    else pCreatePV=GetPageView(rPnt);
    if (pCreatePV!=NULL) { // ansonsten keine Seite angemeldet!
        String aLay(aAktLayer);

        if(nInvent == SdrInventor && nIdent == OBJ_MEASURE && aMeasureLayer.Len())
        {
            aLay = aMeasureLayer;
        }

        SdrLayerID nLayer=pCreatePV->GetPage()->GetLayerAdmin().GetLayerID(aLay,TRUE);
        if (nLayer==SDRLAYER_NOTFOUND) nLayer=0;
        if (!pCreatePV->GetLockedLayers().IsSet(nLayer) && pCreatePV->GetVisibleLayers().IsSet(nLayer)) {
            pAktCreate=SdrObjFactory::MakeNewObject(nInvent,nIdent,pCreatePV->GetPage(),pMod);
            Point aPnt(rPnt-pCreatePV->GetOffset());
            if (nAktInvent!=SdrInventor || (nAktIdent!=USHORT(OBJ_EDGE) &&
                                            nAktIdent!=USHORT(OBJ_FREELINE) &&
                                            nAktIdent!=USHORT(OBJ_FREEFILL) )) { // Kein Fang fuer Edge und Freihand!
                aPnt=GetSnapPos(aPnt,pCreatePV);
            }
            if (pAktCreate!=NULL) {
                BOOL bStartEdit=FALSE; // nach Ende von Create automatisch TextEdit starten
                if (pDefaultStyleSheet!=NULL) pAktCreate->NbcSetStyleSheet(pDefaultStyleSheet, sal_False);

                // #101618# SW uses a naked SdrObject for frame construction. Normally, such an
                // object should not be created. Since it is possible to use it as a helper
                // object (e.g. in letting the user define an area with the interactive
                // construction) at least no items should be set at that object.
                if(nInvent != SdrInventor || nIdent != OBJ_NONE)
                {
                    pAktCreate->SetMergedItemSet(aDefaultAttr);
                }

                if (HAS_BASE(SdrCaptionObj,pAktCreate))
                {
                    SfxItemSet aSet(pMod->GetItemPool());
                    aSet.Put(XFillColorItem(String(),Color(COL_WHITE))); // Falls einer auf Solid umschaltet
                    aSet.Put(XFillStyleItem(XFILL_NONE));

                    pAktCreate->SetMergedItemSet(aSet);

                    bStartEdit=TRUE;
                }
                if (nInvent==SdrInventor && (nIdent==OBJ_TEXT || nIdent==OBJ_TEXTEXT ||
                    nIdent==OBJ_TITLETEXT || nIdent==OBJ_OUTLINETEXT))
                {
                    // Fuer alle Textrahmen default keinen Hintergrund und keine Umrandung
                    SfxItemSet aSet(pMod->GetItemPool());
                    aSet.Put(XFillColorItem(String(),Color(COL_WHITE))); // Falls einer auf Solid umschaltet
                    aSet.Put(XFillStyleItem(XFILL_NONE));
                    aSet.Put(XLineColorItem(String(),Color(COL_BLACK))); // Falls einer auf Solid umschaltet
                    aSet.Put(XLineStyleItem(XLINE_NONE));

                    pAktCreate->SetMergedItemSet(aSet);

                    bStartEdit=TRUE;
                }
                if (!rLogRect.IsEmpty()) pAktCreate->NbcSetLogicRect(rLogRect);

                // #90129# make sure drag start point is inside WorkArea
                const Rectangle& rWorkArea = ((SdrDragView*)this)->GetWorkArea();

                if(!rWorkArea.IsEmpty())
                {
                    if(aPnt.X() < rWorkArea.Left())
                    {
                        aPnt.X() = rWorkArea.Left();
                    }

                    if(aPnt.X() > rWorkArea.Right())
                    {
                        aPnt.X() = rWorkArea.Right();
                    }

                    if(aPnt.Y() < rWorkArea.Top())
                    {
                        aPnt.Y() = rWorkArea.Top();
                    }

                    if(aPnt.Y() > rWorkArea.Bottom())
                    {
                        aPnt.Y() = rWorkArea.Bottom();
                    }
                }

                aDragStat.Reset(aPnt);
                aDragStat.SetView((SdrView*)this);
                aDragStat.SetPageView(pCreatePV);
                aDragStat.SetMinMove(ImpGetMinMovLogic(nMinMov,pOut));
                pDragWin=pOut;
                if (pAktCreate->BegCreate(aDragStat)) {
                    ShowCreateObj(pOut,TRUE);
                    bRet=TRUE;
                } else {
                    delete pAktCreate;
                    pAktCreate=NULL;
                    pCreatePV=NULL;
                }
            }
        }
    }
    return bRet;
}

BOOL SdrCreateView::BegCreateObj(const Point& rPnt, OutputDevice* pOut, short nMinMov, SdrPageView* pPV)
{
    if ( !pCurrentLibObj )
        return ImpBegCreateObj(nAktInvent,nAktIdent,rPnt,pOut,nMinMov,pPV,Rectangle());
    else
        return BegCreateLibObj(rPnt,pCurrentLibObj->Clone(),bCurrentLibObjMoveNoResize,
                               bCurrentLibObjSetDefAttr,bCurrentLibObjSetDefLayer,pOut,nMinMov,pPV);
}

BOOL SdrCreateView::BegCreateCaptionObj(const Point& rPnt, const Size& rObjSiz,
    OutputDevice* pOut, short nMinMov, SdrPageView* pPV)
{
    return ImpBegCreateObj(SdrInventor,OBJ_CAPTION,rPnt,pOut,nMinMov,pPV,
                           Rectangle(rPnt,Size(rObjSiz.Width()+1,rObjSiz.Height()+1)));
}

BOOL SdrCreateView::BegCreateLibObj(const Point& rPnt, SdrObject* pObj, BOOL bMoveNoResize,
    BOOL bSetDefAttr, BOOL bSetDefLayer,
    OutputDevice* pOut, short nMinMov, SdrPageView* pPV)
{
    SetSolidDraggingNow(FALSE); // noch kein SolidDragging bei LibObjs
    BOOL bRet=FALSE;
    UnmarkAllObj();
    BrkAction();
    pConnectMarker->Hide();
    DBG_ASSERT(pLibObjDragMeth==NULL,"SdrCreateView::BegCreateLibObj(): pLibObjDragMeth!=NULL");
    pLibObjDragMeth=NULL;
    if (pPV!=NULL) pCreatePV=pPV;
    else pCreatePV=GetPageView(rPnt);
    if (pObj!=NULL && pCreatePV!=NULL) {
        bRet=TRUE;
        pObj->SetModel(pMod);
        if (bSetDefAttr) {
            if (pDefaultStyleSheet!=NULL) pObj->NbcSetStyleSheet(pDefaultStyleSheet, sal_False);

            pObj->SetMergedItemSet(aDefaultAttr);
        }
        if (bSetDefLayer) {
            SdrLayerID nLayer=pCreatePV->GetPage()->GetLayerAdmin().GetLayerID(aAktLayer,TRUE);
            if (nLayer==SDRLAYER_NOTFOUND) nLayer=0;
            if (pCreatePV->GetLockedLayers().IsSet(nLayer) || !pCreatePV->GetVisibleLayers().IsSet(nLayer)) {
                bRet=FALSE; // Layer gesperrt oder nicht sichtbar!
            }
        }
        if (bRet) {
            pAktCreate=pObj;
            Point aPnt(rPnt-pCreatePV->GetOffset());
            aPnt=GetSnapPos(aPnt,pCreatePV);

            // #90129# make sure drag start point is inside WorkArea
            const Rectangle& rWorkArea = ((SdrDragView*)this)->GetWorkArea();

            if(!rWorkArea.IsEmpty())
            {
                if(aPnt.X() < rWorkArea.Left())
                {
                    aPnt.X() = rWorkArea.Left();
                }

                if(aPnt.X() > rWorkArea.Right())
                {
                    aPnt.X() = rWorkArea.Right();
                }

                if(aPnt.Y() < rWorkArea.Top())
                {
                    aPnt.Y() = rWorkArea.Top();
                }

                if(aPnt.Y() > rWorkArea.Bottom())
                {
                    aPnt.Y() = rWorkArea.Bottom();
                }
            }

            aDragStat.Reset(aPnt);
            aDragStat.SetView((SdrView*)this);
            aDragStat.SetPageView(pCreatePV);
            aDragStat.SetMinMove(ImpGetMinMovLogic(nMinMov,pOut));
            pDragWin=pOut;
            if (bMoveNoResize) {
                pLibObjDragMeth=new ImpSdrCreateLibObjMove(*this);
            } else {
                pLibObjDragMeth=new ImpSdrCreateLibObjResize(*this);
            }
            bRet=pLibObjDragMeth->Beg();
        }
    }
    if (!bRet) {
        if (pObj!=NULL) delete pObj;
        if (pLibObjDragMeth!=NULL) { delete pLibObjDragMeth; pLibObjDragMeth=NULL; }
        pAktCreate=NULL;
        pCreatePV=NULL;
    }
    return bRet;
}

void SdrCreateView::MovCreateObj(const Point& rPnt)
{
    if (pAktCreate!=NULL) {
        if (IsSolidDraggingNow() && !IsSolidDraggingCheck()) {
            // Z.B. fuer Fill+Linelose Textrahmen bei SolidDragging
            SetSolidDraggingCheck(TRUE);
            const SfxItemSet& rSet = pAktCreate->GetMergedItemSet();
            XFillStyle eFill=((XFillStyleItem&)(rSet.Get(XATTR_FILLSTYLE))).GetValue();
            XLineStyle eLine=((XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue();
            if (eLine==XLINE_NONE && eFill==XFILL_NONE) {
                SetSolidDraggingNow(FALSE);
            }
        }
        Point aPnt(rPnt-pCreatePV->GetOffset());
        if (!aDragStat.IsNoSnap()) {
            aPnt=GetSnapPos(aPnt,pCreatePV);
        }
        if (IsOrtho()) {
            if (aDragStat.IsOrtho8Possible()) OrthoDistance8(aDragStat.GetPrev(),aPnt,IsBigOrtho());
            else if (aDragStat.IsOrtho4Possible()) OrthoDistance4(aDragStat.GetPrev(),aPnt,IsBigOrtho());
        }

        // #77734# If the drag point was limited and Ortho is active, do
        // the small ortho correction (reduction) -> last parameter to FALSE.
        sal_Bool bDidLimit(ImpLimitToWorkArea(aPnt,pCreatePV));
        if(bDidLimit && IsOrtho())
        {
            if(aDragStat.IsOrtho8Possible())
                OrthoDistance8(aDragStat.GetPrev(), aPnt, FALSE);
            else if(aDragStat.IsOrtho4Possible())
                OrthoDistance4(aDragStat.GetPrev(), aPnt, FALSE);
        }

        if (aPnt==aDragStat.GetNow()) return;
        if (pLibObjDragMeth==NULL) {
            BOOL bMerk=aDragStat.IsMinMoved();
            if (aDragStat.CheckMinMoved(aPnt)) {
                Rectangle aBound;
                if (IsSolidDraggingNow()) aBound=pAktCreate->GetCurrentBoundRect();
                XPolyPolygon aXPP1;
                if (!bMerk) aDragStat.NextPoint();
                aDragStat.NextMove(aPnt);
                pAktCreate->MovCreate(aDragStat);
                pAktCreate->TakeCreatePoly(aDragStat,aXPP1);
                XPolyPolygon aXPP2(aXPP1); // kopieren, weil wird in DrawCreateDiff geaendert
                if (!IsSolidDraggingNow()) {
                    DrawCreateObjDiff(pCreatePV->DragPoly(),aXPP1);
                }
                pCreatePV->DragPoly()=aXPP2;
                if (IsSolidDraggingNow()) {
                    aBound.Union(pAktCreate->GetCurrentBoundRect());
                    // #116425#
                    // Do not add the in-creation object, this would cause a RePaint
                    //
                    //SdrObjList* pOL=pCreatePV->GetObjList();
                    //SdrInsertReason aReason(SDRREASON_VIEWCALL);
                    //pOL->NbcInsertObject(pAktCreate,CONTAINER_APPEND,&aReason);

                    Point aPvOfs(pCreatePV->GetOffset());
                    USHORT nAnz=pDragWin==NULL ? GetWinCount() : 1;
                    for (USHORT i=0; i<nAnz; i++) {
                        USHORT nWinNum=SDRVIEWWIN_NOTFOUND;
                        OutputDevice* pOut=pDragWin;
                        if (pOut==NULL) {
                            nWinNum=i;
                            pOut=GetWin(nWinNum);
                        } else {
                            nWinNum=aWinList.Find(pOut);
                        }

                        // #116425#
                        // Do not do a complete RePaint into a VDev
                        //
                        //VirtualDevice aVDev(*pOut);
                        //Size a2Pix(pOut->PixelToLogic(Size(2,2)));
                        //MapMode aMap(pOut->GetMapMode());
                        //aVDev.SetMapMode(aMap);
                        //
                        // #109585#
                        //Color aMixedColor = pCreatePV->GetApplicationBackgroundColor();
                        //
                        //aVDev.SetBackground( Wallpaper( aMixedColor ) );
                        //aVDev.SetOutputSize(pOut->GetOutputSize());
                        Rectangle aDirtyArea(aBound);
                        aDirtyArea.Move(aPvOfs.X(),aPvOfs.Y());
                        //CompleteRedraw(&aVDev,aDirtyArea);

                        // #116425#
                        // Do direct paint of dirty area
                        CompleteRedraw(pOut, aDirtyArea);

                        // #116425#
                        // paint in-creation object over it using a ObjectContactOfObjListPainter
                        {
                            sdr::contact::SdrObjectVector aObjectVector;
                            aObjectVector.push_back(pAktCreate);

                            sdr::contact::ObjectContactOfObjListPainter aPainter(aObjectVector);
                            sdr::contact::DisplayInfo aDisplayInfo;
                            XOutputDevice aExtOut(pOut);
                            SdrPaintInfoRec aInfoRec;

                            aDisplayInfo.SetExtendedOutputDevice(&aExtOut);
                            aDisplayInfo.SetPaintInfoRec(&aInfoRec);
                            aDisplayInfo.SetOutputDevice(pOut);

                            // keep draw hierarchy up-to-date
                            aPainter.PreProcessDisplay(aDisplayInfo);

                            // do processing
                            aPainter.ProcessDisplay(aDisplayInfo);

                            // prepare delete
                            aPainter.PrepareDelete();
                        }

                        if (nWinNum!=SDRVIEWWIN_NOTFOUND) {
                            if (IsShownXorVisibleWinNum(nWinNum)) {
                                //ToggleShownXor(&aVDev,NULL);
                                ToggleShownXor(pOut, 0L);
                            }
                        }

                        // #116425#
                        // Do not remove the object and do not copy from some VDev
                        //
                        //Point aCopyOfs(aDirtyArea.TopLeft());
                        //aCopyOfs.X()-=a2Pix.Width();
                        //aCopyOfs.Y()-=a2Pix.Height();
                        //Size aCopySize(aBound.Right()-aBound.Left(),aBound.Bottom()-aBound.Top());
                        //aCopySize.Width()+=2*a2Pix.Width();
                        //aCopySize.Height()+=2*a2Pix.Height();
                        //pOut->DrawOutDev(aCopyOfs,aCopySize,aCopyOfs,aCopySize,aVDev);
                    }
                    //pOL->NbcRemoveObject(pOL->GetObjCount()-1);
                    // Die Page brauchen die Objekte
                    // hier mal bitte eine Optimierung vornehmen
                    //pAktCreate->SetPage(pCreatePV->GetPage());
                }
            }
        } else {
            pLibObjDragMeth->Mov(aPnt);
        }
    }
}

BOOL SdrCreateView::EndCreateObj(SdrCreateCmd eCmd)
{
    BOOL bRet=FALSE;
    SdrObject* pObjMerk=pAktCreate;
    SdrPageView* pPVMerk=pCreatePV;
    if (pAktCreate!=NULL) {
        if (pLibObjDragMeth==NULL) {
            ULONG nAnz=aDragStat.GetPointAnz();
            if (nAnz<=1 && eCmd==SDRCREATE_FORCEEND) {
                BrkCreateObj(); // Objekte mit nur einem Punkt gibt's nicht (zumindest noch nicht)
                return FALSE; // FALSE=Event nicht ausgewertet
            }
            BOOL bPntsEq=nAnz>1;
            ULONG i=1;
            Point aP0=aDragStat.GetPoint(0);
            while (bPntsEq && i<nAnz) { bPntsEq=aP0==aDragStat.GetPoint(i); i++; }
            if (pAktCreate->EndCreate(aDragStat,eCmd)) {
                if (!IsSolidDraggingNow()) HideCreateObj(pDragWin,TRUE);
                if (!bPntsEq) { // sonst Brk, weil alle Punkte gleich sind.
                    SdrObject* pObj=pAktCreate;
                    pAktCreate=NULL;
                    SdrLayerAdmin& rAd=pCreatePV->GetPage()->GetLayerAdmin();
                    SdrLayerID nLayer=rAd.GetLayerID(aAktLayer,TRUE);
                    if (nLayer==SDRLAYER_NOTFOUND) nLayer=0;
                    pObj->SetLayer(nLayer);

                    // #i37462# Always insert with broadcasting (leads to ActionChanged() here),
                    // else the refreshes will be wrong
                    InsertObject(pObj, *pCreatePV);

                    pCreatePV=NULL;
                    bRet=TRUE; // TRUE=Event ausgewertet
                } else {
                    BrkCreateObj();
                }
            } else { // Mehr Punkte
                if (eCmd==SDRCREATE_FORCEEND || // nix da, Ende erzwungen
                    nAnz==0 ||                             // keine Punkte da (kann eigentlich nicht vorkommen)
                    (nAnz<=1 && !aDragStat.IsMinMoved())) { // MinMove nicht erfuellt
                    BrkCreateObj();
                } else {
                    XPolyPolygon aXPP1;
                    pAktCreate->TakeCreatePoly(aDragStat,aXPP1);
                    XPolyPolygon aXPP2(aXPP1); // kopieren, weil wird in DrawCreateDiff geaendert
                    if (!IsSolidDraggingNow()) DrawCreateObjDiff(pCreatePV->DragPoly(),aXPP1);
                    pCreatePV->DragPoly()=aXPP2;
                    aDragStat.ResetMinMoved(); // NextPoint gibt's bei MovCreateObj()
                    bRet=TRUE;
                }
            }
            if (bRet && pObjMerk!=NULL && IsTextEditAfterCreate()) {
                SdrTextObj* pText=PTR_CAST(SdrTextObj,pObjMerk);
                if (pText!=NULL && pText->IsTextFrame()) {
                    BegTextEdit(pText,pPVMerk,(Window*)NULL,TRUE,(SdrOutliner*)NULL,(OutlinerView*)NULL);
                }
            }
        } else {
            if (pLibObjDragMeth->End(FALSE)) {
                pAktCreate=NULL;
                pCreatePV=NULL;
                SdrLayerAdmin& rAd=pPVMerk->GetPage()->GetLayerAdmin();
                SdrLayerID nLayer=rAd.GetLayerID(aAktLayer,TRUE);
                if (nLayer==SDRLAYER_NOTFOUND) nLayer=0;
                pObjMerk->SetLayer(nLayer);

                // #83403# recognize creation of a new 3D object inside a 3D scene
                BOOL bSceneIntoScene(FALSE);

                if(pObjMerk
                    && pObjMerk->ISA(E3dScene)
                    && pPVMerk
                    && pPVMerk->GetAktGroup()
                    && pPVMerk->GetAktGroup()->ISA(E3dScene))
                {
                    BOOL bDidInsert = ((E3dView*)this)->ImpCloneAll3DObjectsToDestScene(
                        (E3dScene*)pObjMerk, (E3dScene*)pPVMerk->GetAktGroup(), Point(0, 0));
                    if(bDidInsert)
                    {
                        // delete object, it's content is cloned and inserted
                        delete pAktCreate;
                        bRet = FALSE;
                        bSceneIntoScene = TRUE;
                    }
                }

                if(!bSceneIntoScene)
                {
                    // do the same as before
                    InsertObject(pObjMerk,*pPVMerk);
                }

                bRet=TRUE;
            } else {
                delete pAktCreate;
                bRet=FALSE;
            }
            delete pLibObjDragMeth;
            pLibObjDragMeth=NULL;
            pAktCreate=NULL;
            pCreatePV=NULL;
        }
        if (pAktCreate==NULL) {
            SetSolidDraggingNow(FALSE);
            SetSolidDraggingCheck(FALSE);
        }
    }
    return bRet;
}

void SdrCreateView::BckCreateObj()
{
    if (pAktCreate!=NULL) {
        if (aDragStat.GetPointAnz()<=2 || pLibObjDragMeth!=NULL) {
            BrkCreateObj();
        } else {
            HideCreateObj(pDragWin,TRUE);
            aDragStat.PrevPoint();
            if (pAktCreate->BckCreate(aDragStat)) {
                ShowCreateObj(pDragWin,TRUE);
            } else {
                BrkCreateObj();
            }
        }
    }
}

void SdrCreateView::BrkCreateObj()
{
    if (pAktCreate!=NULL) {
        if (pLibObjDragMeth==NULL) {
            if (!IsSolidDraggingNow()) {
                HideCreateObj(pDragWin,TRUE);
            } else {
                Rectangle aBound(pAktCreate->GetCurrentBoundRect());
                Point aPvOfs(pCreatePV->GetOffset());
                aBound.Move(aPvOfs.X(),aPvOfs.Y());
                InvalidateAllWin(aBound);
            }
            pAktCreate->BrkCreate(aDragStat);
        } else {
            pLibObjDragMeth->Brk();
            delete pLibObjDragMeth;
            pLibObjDragMeth=NULL;
        }
        delete pAktCreate;
        pAktCreate=NULL;
        pCreatePV=NULL;
        SetSolidDraggingNow(FALSE);
        SetSolidDraggingCheck(FALSE);
    }
}

void SdrCreateView::DrawCreateObjDiff(XPolyPolygon& rXPP0, XPolyPolygon& rXPP1)
{
    if (IsSolidDraggingNow()) return;
    USHORT nPolyAnz0=rXPP0.Count();
    USHORT nPolyAnz1=rXPP1.Count();
    USHORT nMinPolyAnz=Min(nPolyAnz0,nPolyAnz1);
    USHORT nPolyNum;
    for (nPolyNum=nMinPolyAnz; nPolyNum>0;) { // unveraenderte Polygone entfernen
        nPolyNum--;
        if (rXPP0[nPolyNum]==rXPP1[nPolyNum]) {
            rXPP0.Remove(nPolyNum);
            rXPP1.Remove(nPolyNum);
        }
    }
    nPolyAnz0=rXPP0.Count();
    nPolyAnz1=rXPP1.Count();
    nMinPolyAnz=Min(nPolyAnz0,nPolyAnz1);
    BOOL bNeedDot=FALSE;
    for (nPolyNum=nMinPolyAnz; nPolyNum>0;) { // unveraenderte Polygonteile entfernen
        nPolyNum--;
        const XPolygon& rXP0=rXPP0[nPolyNum];
        const XPolygon& rXP1=rXPP1[nPolyNum];
        USHORT nPntAnz0=rXP0.GetPointCount();
        USHORT nPntAnz1=rXP1.GetPointCount();
        USHORT nMinPntAnz=Min(nPntAnz0,nPntAnz1);
        USHORT nEquCnt0=0;
        USHORT nEquCnt1=0;
        for (USHORT nPntNum=0; nPntNum<nMinPntAnz; nPntNum++) {
            if (rXP0[nPntNum]==rXP1[nPntNum]) {
                nEquCnt0=nEquCnt1;
                BOOL bCtrl0=(nPntNum<nMinPntAnz) && rXP0.IsControl(nPntNum+1);
                BOOL bCtrl1=(nPntNum<nMinPntAnz) && rXP1.IsControl(nPntNum+1);
                if (bCtrl0==bCtrl1) {
                    nEquCnt1=nPntNum+1;
                    if (bCtrl0) {
                        nPntNum++;
                        if (rXP0[nPntNum]==rXP1[nPntNum]) {
                            nPntNum++;
                            if (rXP0[nPntNum]==rXP1[nPntNum]) {
                                nEquCnt1=nPntNum+1;
                            } else nPntNum=nMinPntAnz; // sowas wie break.
                        } else nPntNum=nMinPntAnz; // sowas wie break.
                    }
                } else nPntNum=nMinPntAnz; // sowas wie break.
            } else nPntNum=nMinPntAnz; // sowas wie break.
        }
        if (nEquCnt0!=0) {
            rXPP0[nPolyNum].Remove(0,nEquCnt0);
            rXPP1[nPolyNum].Remove(0,nEquCnt0);
#ifndef MAC // anderes Polygonpainting beim Mac
            if (nPolyNum==nMinPolyAnz-1 &&
                (rXPP0[nPolyNum].GetPointCount()<=1 ||
                 rXPP1[nPolyNum].GetPointCount()<=1)) bNeedDot=TRUE;
#endif
        }
    }
    aDragStat.SetShown(TRUE);
    USHORT nOutNum=0;
    do {
        Window* pO= (Window*)pDragWin;
        if (pO==NULL) {
            pO=(Window*)GetWin(nOutNum);
            nOutNum++;
        }
        if (pO!=NULL) {
            ImpSdrHdcMerk aHDCMerk(*pO,SDRHDC_SAVEALL,bRestoreColors);
            RasterOp eRop0=pO->GetRasterOp();
            pO->SetRasterOp(ROP_INVERT);
            pXOut->SetOutDev(pO);

            Color aBlackColor( COL_BLACK );
            Color aTranspColor( COL_TRANSPARENT );
            pXOut->OverrideLineColor( aBlackColor );
            pXOut->OverrideFillColor( aTranspColor );
            pXOut->SetOffset(pCreatePV->GetOffset());
            USHORT nAnz=rXPP0.Count();
            USHORT i;
            for (i=0; i<nAnz; i++) {
                pXOut->DrawXPolyLine(rXPP0[i]);
            }
            nAnz=rXPP1.Count();
            for (i=0; i<nAnz; i++) {
                pXOut->DrawXPolyLine(rXPP1[i]);
                if (bNeedDot && nPolyNum==nMinPolyAnz-1) {
                    Point aPt(rXPP1[i][0]);
                    //pO->InvertRect(Rectangle(aPt,aPt));
                    pO->Invert( Rectangle( aPt, aPt ) );
                }
            }
            pXOut->SetOffset(Point(0,0));
            pO->SetRasterOp(eRop0);
            if (bRestoreColors) aHDCMerk.Restore(*pO);
        }
    } while (pDragWin==NULL && nOutNum<GetWinCount());
}

void SdrCreateView::DrawCreateObj(OutputDevice* pOut, BOOL /*bFull*/) const
{
    if (IsSolidDraggingNow()) return;
    if (IsCreateObj()) {
        USHORT i=0;
        do {
            OutputDevice* pO=pOut;
            if (pO==NULL) {
                pO=GetWin(i);
                i++;
            }
            if (pO!=NULL) {
                ImpSdrHdcMerk aHDCMerk(*pO,SDRHDC_SAVEALL,bRestoreColors);
                RasterOp eRop0=pO->GetRasterOp();
                pO->SetRasterOp(ROP_INVERT);
                pXOut->SetOutDev(pO);
                Color aBlackColor( COL_BLACK );
                Color aTranspColor( COL_TRANSPARENT );
                pXOut->OverrideLineColor( aBlackColor );
                pXOut->OverrideFillColor( aTranspColor );
                pXOut->SetOffset(pCreatePV->GetOffset());
                XPolyPolygon aXPP(pCreatePV->DragPoly());
                USHORT nAnz=aXPP.Count();
                for (USHORT j=0; j<nAnz; j++) {
                    pXOut->DrawXPolyLine(aXPP[j]);
                }
                pXOut->SetOffset(Point(0,0));
                pO->SetRasterOp(eRop0);
                if (bRestoreColors) aHDCMerk.Restore(*pO);
            }
        } while (pOut==NULL && i<GetWinCount());
    }
}

void SdrCreateView::ShowCreateObj(OutputDevice* pOut, BOOL bFull)
{
    if (IsCreateObj() && !aDragStat.IsShown()) {
        XPolyPolygon aXPP;
        if (pLibObjDragMeth==NULL) {
            pAktCreate->TakeCreatePoly(aDragStat,pCreatePV->DragPoly());
        }
        DrawCreateObj(pOut,bFull);
        aDragStat.SetShown(TRUE);

        // #i3705# set shown state at views
        if(pOut)
        {
            sal_uInt16 nw(aWinList.Find(pOut));

            if(nw < GetWinCount() && SDRVIEWWIN_NOTFOUND != nw)
            {
                if(!IsShownXorVisibleWinNum(nw))
                {
                    SetShownXorVisible(nw, TRUE);
                }
            }
        }
    }
}

void SdrCreateView::HideCreateObj(OutputDevice* pOut, BOOL bFull)
{
    if (IsCreateObj() && aDragStat.IsShown()) {
        DrawCreateObj(pOut,bFull);
        aDragStat.SetShown(FALSE);

        // #i3705# clear shown state at views
        if(pOut)
        {
            sal_uInt16 nw(aWinList.Find(pOut));

            if(nw < GetWinCount() && SDRVIEWWIN_NOTFOUND != nw)
            {
                if(IsShownXorVisibleWinNum(nw))
                {
                    SetShownXorVisible(nw, FALSE);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/* new interface src537 */
BOOL SdrCreateView::GetAttributes(SfxItemSet& rTargetSet, BOOL bOnlyHardAttr) const
{
    if(pAktCreate)
    {
        rTargetSet.Put(pAktCreate->GetMergedItemSet());
        return TRUE;
    }
    else
    {
        return SdrDragView::GetAttributes(rTargetSet, bOnlyHardAttr);
    }
}

BOOL SdrCreateView::SetAttributes(const SfxItemSet& rSet, BOOL bReplaceAll)
{
    if(pAktCreate)
    {
        //pAktCreate->SetItemSetAndBroadcast(rSet, bReplaceAll);
        pAktCreate->SetMergedItemSetAndBroadcast(rSet, bReplaceAll);

        return TRUE;
    }
    else
    {
        return SdrDragView::SetAttributes(rSet,bReplaceAll);
    }
}

SfxStyleSheet* SdrCreateView::GetStyleSheet() const // SfxStyleSheet* SdrCreateView::GetStyleSheet(BOOL& rOk) const
{
    if (pAktCreate!=NULL) {
        //rOk=TRUE;
        return pAktCreate->GetStyleSheet();
    } else {
        return SdrDragView::GetStyleSheet(); // SdrDragView::GetStyleSheet(rOk);
    }
}

BOOL SdrCreateView::SetStyleSheet(SfxStyleSheet* pStyleSheet, BOOL bDontRemoveHardAttr)
{
    if (pAktCreate!=NULL) {
        pAktCreate->SetStyleSheet(pStyleSheet,bDontRemoveHardAttr);
        return TRUE;
    } else {
        return SdrDragView::SetStyleSheet(pStyleSheet,bDontRemoveHardAttr);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//BFS01void SdrCreateView::WriteRecords(SvStream& rOut) const
//BFS01{
//BFS01 SdrDragView::WriteRecords(rOut);
//BFS01 {
//BFS01     SdrNamedSubRecord aSubRecord(rOut,STREAM_WRITE,SdrInventor,SDRIORECNAME_VIEWCROBJECT);
//BFS01     rOut<<nAktInvent;
//BFS01     rOut<<nAktIdent;
//BFS01 } {
//BFS01     SdrNamedSubRecord aSubRecord(rOut,STREAM_WRITE,SdrInventor,SDRIORECNAME_VIEWCRFLAGS);
//BFS01     rOut<<BOOL(b1stPointAsCenter);
//BFS01 } {
//BFS01     // in der CreateView (statt ObjEditView) weil sonst inkompatibel.
//BFS01     SdrNamedSubRecord aSubRecord(rOut,STREAM_WRITE,SdrInventor,SDRIORECNAME_VIEWTEXTEDIT);
//BFS01     rOut<<BOOL(bQuickTextEditMode);
//BFS01 } {
//BFS01     // in der CreateView (statt ObjEditView) weil sonst inkompatibel.
//BFS01     SdrNamedSubRecord aSubRecord(rOut,STREAM_WRITE,SdrInventor,SDRIORECNAME_VIEWMACRO);
//BFS01     rOut<<BOOL(bMacroMode);
//BFS01 }
//BFS01}

//BFS01BOOL SdrCreateView::ReadRecord(const SdrIOHeader& rViewHead,
//BFS01 const SdrNamedSubRecord& rSubHead,
//BFS01 SvStream& rIn)
//BFS01{
//BFS01 BOOL bRet=FALSE;
//BFS01 if (rSubHead.GetInventor()==SdrInventor) {
//BFS01     bRet=TRUE;
//BFS01     switch (rSubHead.GetIdentifier()) {
//BFS01         case SDRIORECNAME_VIEWCROBJECT: {
//BFS01             UINT32 nInvent;
//BFS01             UINT16 nIdent;
//BFS01             rIn>>nInvent;
//BFS01             rIn>>nIdent;
//BFS01             SetCurrentObj(nIdent,nInvent);
//BFS01         } break;
//BFS01         case SDRIORECNAME_VIEWCRFLAGS: {
//BFS01             BOOL bTmp; rIn>>bTmp; b1stPointAsCenter=bTmp;
//BFS01         } break;
//BFS01         case SDRIORECNAME_VIEWTEXTEDIT: {
//BFS01             // in der CreateView (statt ObjEditView) weil sonst inkompatibel.
//BFS01             BOOL bTmp; rIn>>bTmp; bQuickTextEditMode=bTmp;
//BFS01         } break;
//BFS01         case SDRIORECNAME_VIEWMACRO: {
//BFS01             // in der CreateView (statt ObjEditView) weil sonst inkompatibel.
//BFS01             BOOL bTmp; rIn>>bTmp; bMacroMode=bTmp;
//BFS01         } break;
//BFS01         default: bRet=FALSE;
//BFS01     }
//BFS01 }
//BFS01 if (!bRet) bRet=SdrDragView::ReadRecord(rViewHead,rSubHead,rIn);
//BFS01 return bRet;
//BFS01}

