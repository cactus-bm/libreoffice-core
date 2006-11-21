/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdocapt.cxx,v $
 *
 *  $Revision: 1.26 $
 *
 *  last change: $Author: vg $ $Date: 2006-11-21 16:56:03 $
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

#ifndef _BIGINT_HXX //autogen
#include <tools/bigint.hxx>
#endif

#ifndef _SVX_XLNWTIT_HXX //autogen
#include <xlnwtit.hxx>
#endif

#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif

#include "svdocapt.hxx"
#include "xpool.hxx"
#include "xpoly.hxx"
#include "svdattrx.hxx"
#include "svdpool.hxx"
#include "svdxout.hxx"
#include "svdetc.hxx"
#include "svdtrans.hxx"
#include "svdtouch.hxx"
#include "svdhdl.hxx"
#include "svddrag.hxx"
#include "svdmodel.hxx"
#include "svdview.hxx"   // fuer RectSnap
#include "svdglob.hxx"   // StringCache
#include "svdstr.hrc"    // Objektname
#include "svdogrp.hxx"
#include "svdpage.hxx"

#ifndef _SVX_XFLHTIT_HXX
#include <xflhtit.hxx>
#endif

#ifndef _SVX_XFLCLIT_HXX
#include <xflclit.hxx>
#endif

#ifndef _SVX_XFLTRIT_HXX
#include <xfltrit.hxx>
#endif

#ifndef _EEITEM_HXX
#include "eeitem.hxx"
#endif

#ifndef _SDR_PROPERTIES_CAPTIONPROPERTIES_HXX
#include <svx/sdr/properties/captionproperties.hxx>
#endif

// #i32599#
#ifndef _SV_SALBTYPE_HXX
#include <vcl/salbtype.hxx>     // FRound
#endif

#ifndef _BGFX_TUPLE_B2DTUPLE_HXX
#include <basegfx/tuple/b2dtuple.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif

#ifndef _BGFX_RANGE_B2DRANGE_HXX
#include <basegfx/range/b2drange.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif

// #i32599#
inline double ImplTwipsToMM(double fVal) { return (fVal * (127.0 / 72.0)); }
inline double ImplMMToTwips(double fVal) { return (fVal * (72.0 / 127.0)); }

////////////////////////////////////////////////////////////////////////////////////////////////////

enum EscDir {LKS,RTS,OBN,UNT};

class ImpCaptParams
{
public:
    SdrCaptionType              eType;
    long                        nAngle;
    long                        nGap;
    long                        nEscRel;
    long                        nEscAbs;
    long                        nLineLen;
    SdrCaptionEscDir            eEscDir;
    FASTBOOL                    bFitLineLen;
    FASTBOOL                    bEscRel;
    FASTBOOL                    bFixedAngle;

public:
    ImpCaptParams()
    {
        eType      =SDRCAPT_TYPE3;
        bFixedAngle=FALSE;
        nAngle     =4500;
        nGap       =0;
        eEscDir    =SDRCAPT_ESCHORIZONTAL;
        bEscRel    =TRUE;
        nEscRel    =5000;
        nEscAbs    =0;
        nLineLen   =0;
        bFitLineLen=TRUE;
    }
    void CalcEscPos(const Point& rTail, const Rectangle& rRect, Point& rPt, EscDir& rDir) const;
};

void ImpCaptParams::CalcEscPos(const Point& rTailPt, const Rectangle& rRect, Point& rPt, EscDir& rDir) const
{
    Point aTl(rTailPt); // lokal kopieren wg. Performance
    long nX,nY;
    if (bEscRel) {
        nX=rRect.Right()-rRect.Left();
        nX=BigMulDiv(nX,nEscRel,10000);
        nY=rRect.Bottom()-rRect.Top();
        nY=BigMulDiv(nY,nEscRel,10000);
    } else {
        nX=nEscAbs;
        nY=nEscAbs;
    }
    nX+=rRect.Left();
    nY+=rRect.Top();
    Point  aBestPt;
    EscDir eBestDir=LKS;
    FASTBOOL bTryH=eEscDir==SDRCAPT_ESCBESTFIT;
    if (!bTryH) {
        if (eType!=SDRCAPT_TYPE1) {
            bTryH=eEscDir==SDRCAPT_ESCHORIZONTAL;
        } else {
            bTryH=eEscDir==SDRCAPT_ESCVERTICAL;
        }
    }
    FASTBOOL bTryV=eEscDir==SDRCAPT_ESCBESTFIT;
    if (!bTryV) {
        if (eType!=SDRCAPT_TYPE1) {
            bTryV=eEscDir==SDRCAPT_ESCVERTICAL;
        } else {
            bTryV=eEscDir==SDRCAPT_ESCHORIZONTAL;
        }
    }

    if (bTryH) {
        Point aLft(rRect.Left()-nGap,nY);
        Point aRgt(rRect.Right()+nGap,nY);
        FASTBOOL bLft=(aTl.X()-aLft.X()<aRgt.X()-aTl.X());
        if (bLft) {
            eBestDir=LKS;
            aBestPt=aLft;
        } else {
            eBestDir=RTS;
            aBestPt=aRgt;
        }
    }
    if (bTryV) {
        Point aTop(nX,rRect.Top()-nGap);
        Point aBtm(nX,rRect.Bottom()+nGap);
        FASTBOOL bTop=(aTl.Y()-aTop.Y()<aBtm.Y()-aTl.Y());
        Point aBest2;
        EscDir eBest2;
        if (bTop) {
            eBest2=OBN;
            aBest2=aTop;
        } else {
            eBest2=UNT;
            aBest2=aBtm;
        }
        FASTBOOL bTakeIt=eEscDir!=SDRCAPT_ESCBESTFIT;
        if (!bTakeIt) {
            BigInt aHorX(aBestPt.X()-aTl.X()); aHorX*=aHorX;
            BigInt aHorY(aBestPt.Y()-aTl.Y()); aHorY*=aHorY;
            BigInt aVerX(aBest2.X()-aTl.X());  aVerX*=aVerX;
            BigInt aVerY(aBest2.Y()-aTl.Y());  aVerY*=aVerY;
            if (eType!=SDRCAPT_TYPE1) {
                bTakeIt=aVerX+aVerY<aHorX+aHorY;
            } else {
                bTakeIt=aVerX+aVerY>=aHorX+aHorY;
            }
        }
        if (bTakeIt) {
            aBestPt=aBest2;
            eBestDir=eBest2;
        }
    }
    rPt=aBestPt;
    rDir=eBestDir;
}

//////////////////////////////////////////////////////////////////////////////

sdr::properties::BaseProperties* SdrCaptionObj::CreateObjectSpecificProperties()
{
    return new sdr::properties::CaptionProperties(*this);
}

//////////////////////////////////////////////////////////////////////////////

TYPEINIT1(SdrCaptionObj,SdrRectObj);

SdrCaptionObj::SdrCaptionObj():
    SdrRectObj(OBJ_TEXT),
    aTailPoly(3),  // Default Groesse: 3 Punkte = 2 Linien
    mbSpecialTextBoxShadow(FALSE),
    mbFixedTail(FALSE)
{
}

SdrCaptionObj::SdrCaptionObj(const Rectangle& rRect):
    SdrRectObj(OBJ_TEXT,rRect),
    aTailPoly(3),  // Default Groesse: 3 Punkte = 2 Linien
    mbSpecialTextBoxShadow(FALSE),
    mbFixedTail(FALSE)
{
}

SdrCaptionObj::SdrCaptionObj(const Rectangle& rRect, const Point& rTail):
    SdrRectObj(OBJ_TEXT,rRect),
    aTailPoly(3),  // Default Groesse: 3 Punkte = 2 Linien
    mbSpecialTextBoxShadow(FALSE),
    mbFixedTail(FALSE)
{
    aTailPoly[0]=maFixedTailPos=rTail;
}

SdrCaptionObj::~SdrCaptionObj()
{
}

sal_Bool SdrCaptionObj::DoPaintObject(XOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec) const
{
    // special shadow paint for calc
    if(mbSpecialTextBoxShadow)
    {
        const SfxItemSet& rSet = GetObjectItemSet();
        sal_uInt32 nXDist = ((SdrShadowXDistItem&)(rSet.Get(SDRATTR_SHADOWXDIST))).GetValue();
        sal_uInt32 nYDist = ((SdrShadowYDistItem&)(rSet.Get(SDRATTR_SHADOWYDIST))).GetValue();
        const SdrShadowColorItem& rShadColItem = ((SdrShadowColorItem&)(rSet.Get(SDRATTR_SHADOWCOLOR)));
        Color aShadCol(rShadColItem.GetColorValue());
        sal_uInt16 nTransp = ((SdrShadowTransparenceItem&)(rSet.Get(SDRATTR_SHADOWTRANSPARENCE))).GetValue();
        XFillStyle eStyle = ((XFillStyleItem&)(rSet.Get(XATTR_FILLSTYLE))).GetValue();

        SfxItemSet aSet(rSet);
        // #99001# Hide lines for special calc shadow
        aSet.Put(XLineStyleItem(XLINE_NONE));

        if(eStyle == XFILL_HATCH) // #41666#
        {
            XHatch aHatch = ((XFillHatchItem&)(rSet.Get(XATTR_FILLHATCH))).GetHatchValue();
            aHatch.SetColor(aShadCol);
            aSet.Put(XFillHatchItem(String(),aHatch));
        }
        else
        {
            if(eStyle != XFILL_NONE && eStyle != XFILL_SOLID)
            {
                // also fuer Gradient und Bitmap
                aSet.Put(XFillStyleItem(XFILL_SOLID));
            }

            aSet.Put(XFillColorItem(String(),aShadCol));
            aSet.Put(XFillTransparenceItem(nTransp));
        }

        rOut.SetFillAttr(aSet);
        // #99001# Hide lines for special calc shadow
        rOut.SetLineAttr(aSet);

        sal_Int32 nEckRad(GetEckenradius());
        if(PaintNeedsXPoly(nEckRad))
        {
            XPolygon aX(GetXPoly());
            aX.Move(nXDist,nYDist);
            rOut.DrawPolygon(aX.getB2DPolygon());
        }
        else
        {
            Rectangle aR(aRect);
            aR.Move(nXDist,nYDist);
            rOut.DrawRect(aR,USHORT(2*nEckRad),USHORT(2*nEckRad));
        }
    }

    // call parent for normal paint
    return SdrRectObj::DoPaintObject(rOut, rInfoRec);
}

void SdrCaptionObj::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
{
    rInfo.bRotateFreeAllowed=FALSE;
    rInfo.bRotate90Allowed  =FALSE;
    rInfo.bMirrorFreeAllowed=FALSE;
    rInfo.bMirror45Allowed  =FALSE;
    rInfo.bMirror90Allowed  =FALSE;
    rInfo.bTransparenceAllowed = FALSE;
    rInfo.bGradientAllowed = FALSE;
    rInfo.bShearAllowed     =FALSE;
    rInfo.bEdgeRadiusAllowed=FALSE;
    rInfo.bCanConvToPath    =TRUE;
    rInfo.bCanConvToPoly    =TRUE;
    rInfo.bCanConvToPathLineToArea=FALSE;
    rInfo.bCanConvToPolyLineToArea=FALSE;
    rInfo.bCanConvToContour = (rInfo.bCanConvToPoly || LineGeometryUsageIsNecessary());
}

UINT16 SdrCaptionObj::GetObjIdentifier() const
{
    return UINT16(OBJ_CAPTION);
}

void SdrCaptionObj::RecalcBoundRect()
{
    aOutRect=GetSnapRect();

    // #i32599#
    // Add BoundRect of TailPlygon here, not in RecalcSnapRect()
    aOutRect.Union(aTailPoly.GetBoundRect());

    long nLineWdt=ImpGetLineWdt();
    nLineWdt++; nLineWdt/=2;
    long nLEndWdt=ImpGetLineEndAdd();
    if (nLEndWdt>nLineWdt) nLineWdt=nLEndWdt;
    if (nLineWdt!=0) {
        aOutRect.Left  ()-=nLineWdt;
        aOutRect.Top   ()-=nLineWdt;
        aOutRect.Right ()+=nLineWdt;
        aOutRect.Bottom()+=nLineWdt;
    }
    ImpAddShadowToBoundRect();
    ImpAddTextToBoundRect();
}

SdrObject* SdrCaptionObj::CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const
{
    if(pVisiLayer && !pVisiLayer->IsSet(sal::static_int_cast< sal_uInt8 >(GetLayer())))
    {
        return NULL;
    }

    sal_Bool bHit(SdrRectObj::CheckHit(rPnt,nTol,pVisiLayer) != NULL);
    if (!bHit) {
        INT32 nMyTol=nTol;
        INT32 nWdt = ((XLineWidthItem&)(GetObjectItem(XATTR_LINEWIDTH))).GetValue();
        nWdt++;
        nWdt /= 2;

        if (nWdt>nMyTol) nMyTol=nWdt; // Bei dicker Linie keine Toleranz noetig
        Rectangle aR(rPnt,rPnt);
        aR.Left()  -=nMyTol;
        aR.Right() +=nMyTol;
        aR.Top()   -=nMyTol;
        aR.Bottom()+=nMyTol;
        bHit = IsRectTouchesLine(aTailPoly,aR);
    }
    return bHit ? (SdrObject*)this : NULL;
}

void SdrCaptionObj::operator=(const SdrObject& rObj)
{
    SdrRectObj::operator=(rObj);
    aTailPoly=((SdrCaptionObj&)rObj).aTailPoly;
}

void SdrCaptionObj::TakeObjNameSingul(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNameSingulCAPTION);

    String aName( GetName() );
    if(aName.Len())
    {
        rName += sal_Unicode(' ');
        rName += sal_Unicode('\'');
        rName += aName;
        rName += sal_Unicode('\'');
    }
}

void SdrCaptionObj::TakeObjNamePlural(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNamePluralCAPTION);
}

basegfx::B2DPolyPolygon SdrCaptionObj::TakeXorPoly(sal_Bool bDetail) const
{
    basegfx::B2DPolyPolygon aPolyPoly(SdrRectObj::TakeXorPoly(bDetail));
    aPolyPoly.append(aTailPoly.getB2DPolygon());

    return aPolyPoly;
}

sal_uInt32 SdrCaptionObj::GetHdlCount() const
{
    sal_uInt32 nAnz1(SdrRectObj::GetHdlCount());
    // sal_uInt32 nAnz2(aTailPoly.GetSize());
    // Derzeit ist nur das Draggen des Schwanzendes implementiert
    return nAnz1 + 1L;
}

SdrHdl* SdrCaptionObj::GetHdl(sal_uInt32 nHdlNum) const
{
    const sal_uInt32 nRectHdlAnz(SdrRectObj::GetHdlCount());

    if(nHdlNum < nRectHdlAnz)
    {
        return SdrRectObj::GetHdl(nHdlNum);
    }
    else
    {
        sal_uInt32 nPntNum(nHdlNum);
        nPntNum -= nRectHdlAnz;

        if(nPntNum < aTailPoly.GetSize())
        {
            SdrHdl* pHdl = new SdrHdl(aTailPoly.GetPoint((sal_uInt16)nPntNum), HDL_POLY);
            pHdl->SetPolyNum(1L);
            pHdl->SetPointNum(nPntNum);
            return pHdl;
        }
        else
        {
            return 0L;
        }
    }
}

FASTBOOL SdrCaptionObj::HasSpecialDrag() const
{
    return TRUE;
}

FASTBOOL SdrCaptionObj::BegDrag(SdrDragStat& rDrag) const
{
    const SdrHdl* pHdl=rDrag.GetHdl();

    // #109992#
    // If this is a CaptionObj, set the flags bEndDragChangesAttributes
    // and bEndDragChangesGeoAndAttributes to create an undo action which
    // contains geo and attr changes. Joe seems to have added this as a fix
    // for a similar occurring problem.
    rDrag.SetEndDragChangesAttributes(TRUE);
    rDrag.SetEndDragChangesGeoAndAttributes(TRUE);

    if (pHdl!=NULL && pHdl->GetPolyNum()==0) {
        return SdrRectObj::BegDrag(rDrag);
    } else {
        rDrag.SetOrtho8Possible(TRUE);
        if (pHdl==NULL) {
            if (bMovProt) return FALSE; // Position geschuetzt
            rDrag.SetNoSnap(TRUE); // Snap mache ich in diesem Fall selbst (RectSnap)
            rDrag.SetActionRect(aRect);
            Point aHit(rDrag.GetStart());
            if (SdrRectObj::CheckHit(aHit,0,NULL)!=NULL) return TRUE;
            else return FALSE;
        } else {
            return (pHdl->GetPolyNum()==1) && (pHdl->GetPointNum()==0);
        }
    }
}

FASTBOOL SdrCaptionObj::MovDrag(SdrDragStat& rDrag) const
{
    const SdrHdl* pHdl=rDrag.GetHdl();
    if (pHdl!=NULL && pHdl->GetPolyNum()==0) {
        return SdrRectObj::MovDrag(rDrag);
    } else {
        SdrView* pView=rDrag.GetView();
        SdrPageView* pPV=rDrag.GetPageView();
        Rectangle aR(aRect);
        aR.Move(rDrag.GetDX(),rDrag.GetDY());
        if (pView!=NULL && pPV!=NULL && pView->IsSnapEnabled()) { // RectSnap
            long nDX=0,nDY=0;
            pView->SnapRect(aR,pPV,nDX,nDY);
            rDrag.Now().X()+=nDX;
            rDrag.Now().Y()+=nDY;
            aR.Move(nDX,nDY);
        }
        rDrag.SetActionRect(aR);
        return TRUE;
    }
}

FASTBOOL SdrCaptionObj::EndDrag(SdrDragStat& rDrag)
{
    Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
    const SdrHdl* pHdl=rDrag.GetHdl();
    if (pHdl!=NULL && pHdl->GetPolyNum()==0) {
        FASTBOOL bRet=SdrRectObj::EndDrag(rDrag);
        ImpRecalcTail();

        // Here only redraw wanted
        ActionChanged();
        // BroadcastObjectChange();

        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
        return bRet;
    } else {
        // #110094#-14 SendRepaintBroadcast();
        Point aDelt(rDrag.GetNow()-rDrag.GetStart());
        if (pHdl==NULL) { // Rect verschoben
            aRect.Move(aDelt.X(),aDelt.Y());
        } else {          // Schwanz verschoben
            aTailPoly[0]+=aDelt;
        }
        ImpRecalcTail();
        SetChanged();
        BroadcastObjectChange();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
        return TRUE;
    }
}

void SdrCaptionObj::BrkDrag(SdrDragStat& rDrag) const
{
    const SdrHdl* pHdl=rDrag.GetHdl();
    if (pHdl!=NULL && pHdl->GetPolyNum()==0) {
        SdrRectObj::BrkDrag(rDrag);
    } else {
    }
}

XubString SdrCaptionObj::GetDragComment(const SdrDragStat& rDrag, FASTBOOL bUndoDragComment, FASTBOOL bCreateComment) const
{
    if (bCreateComment) return String();
    const SdrHdl* pHdl=rDrag.GetHdl();
    if (pHdl!=NULL && pHdl->GetPolyNum()==0) {
        return SdrRectObj::GetDragComment(rDrag,bUndoDragComment,FALSE);
    } else {
        XubString aStr;
        if (pHdl==NULL) {
            ImpTakeDescriptionStr(STR_DragCaptFram,aStr);
        } else {
            ImpTakeDescriptionStr(STR_DragCaptTail,aStr);
        }
        return aStr;
    }
}

basegfx::B2DPolyPolygon SdrCaptionObj::TakeDragPoly(const SdrDragStat& rDrag) const
{
    const SdrHdl* pHdl = rDrag.GetHdl();
    const bool bRad(rDrag.GetHdl() && HDL_CIRC == rDrag.GetHdl()->GetKind());
    const bool bRectSiz(pHdl && 0L == pHdl->GetPolyNum());

    if(bRad)
    {
        return SdrRectObj::TakeDragPoly(rDrag);
    }
    else
    {
        XPolyPolygon aXPP;

        Point aDelt(rDrag.GetNow()-rDrag.GetStart());
        Polygon aTmpPoly(aTailPoly);
        Rectangle aTmpRect;
        if (bRectSiz) aTmpRect=ImpDragCalcRect(rDrag);
        else aTmpRect=aRect;
        ImpCaptParams aPara;
        ImpGetCaptParams(aPara);
        if (!bRectSiz) {
            if (pHdl==NULL) { // Rect verschieben
                aTmpRect.Move(aDelt.X(),aDelt.Y());
            } else {          // Schwanz verschieben
                aTmpPoly[0]+=aDelt;
            }
        }
        ImpCalcTail(aPara,aTmpPoly,aTmpRect);
        aXPP.Insert(ImpCalcXPoly(aTmpRect,GetEckenradius()));
        aXPP.Insert(XPolygon(aTmpPoly));

        return aXPP.getB2DPolyPolygon();
    }
}

void SdrCaptionObj::ImpGetCaptParams(ImpCaptParams& rPara) const
{
    const SfxItemSet& rSet = GetObjectItemSet();
    rPara.eType      =((SdrCaptionTypeItem&)      (rSet.Get(SDRATTR_CAPTIONTYPE      ))).GetValue();
    rPara.bFixedAngle=((SdrCaptionFixedAngleItem&)(rSet.Get(SDRATTR_CAPTIONANGLE     ))).GetValue();
    rPara.nAngle     =((SdrCaptionAngleItem&)     (rSet.Get(SDRATTR_CAPTIONFIXEDANGLE))).GetValue();
    rPara.nGap       =((SdrCaptionGapItem&)       (rSet.Get(SDRATTR_CAPTIONGAP       ))).GetValue();
    rPara.eEscDir    =((SdrCaptionEscDirItem&)    (rSet.Get(SDRATTR_CAPTIONESCDIR    ))).GetValue();
    rPara.bEscRel    =((SdrCaptionEscIsRelItem&)  (rSet.Get(SDRATTR_CAPTIONESCISREL  ))).GetValue();
    rPara.nEscRel    =((SdrCaptionEscRelItem&)    (rSet.Get(SDRATTR_CAPTIONESCREL    ))).GetValue();
    rPara.nEscAbs    =((SdrCaptionEscAbsItem&)    (rSet.Get(SDRATTR_CAPTIONESCABS    ))).GetValue();
    rPara.nLineLen   =((SdrCaptionLineLenItem&)   (rSet.Get(SDRATTR_CAPTIONLINELEN   ))).GetValue();
    rPara.bFitLineLen=((SdrCaptionFitLineLenItem&)(rSet.Get(SDRATTR_CAPTIONFITLINELEN))).GetValue();
}

void SdrCaptionObj::ImpRecalcTail()
{
    ImpCaptParams aPara;
    ImpGetCaptParams(aPara);
    ImpCalcTail(aPara,aTailPoly,aRect);
    SetRectsDirty();
    SetXPolyDirty();
}

// #i35971#
// SdrCaptionObj::ImpCalcTail1 does move the object(!). What a hack.
// I really wonder why this had not triggered problems before. I am
// sure there are some places where SetTailPos() is called at least
// twice or SetSnapRect after it again just to work around this.
// Changed this method to not do that.
// Also found why this has been done: For interactive dragging of the
// tail end pos for SDRCAPT_TYPE1. This sure was the simplest method
// to achieve this, for the cost to make a whole group of const methods
// of this object implicitly chainging the object's position.
void SdrCaptionObj::ImpCalcTail1(const ImpCaptParams& rPara, Polygon& rPoly, Rectangle& rRect) const
{
    Polygon aPol(2);
    Point aTl(rPoly[0]);

    aPol[0] = aTl;
    aPol[1] = aTl;

    EscDir eEscDir;
    Point aEscPos;

    rPara.CalcEscPos(aTl, rRect, aEscPos, eEscDir);
    aPol[1] = aEscPos;

    if(eEscDir==LKS || eEscDir==RTS)
    {
        aPol[0].X() = aEscPos.X();
    }
    else
    {
        aPol[0].Y() = aEscPos.Y();
    }

    rPoly = aPol;
}

void SdrCaptionObj::ImpCalcTail2(const ImpCaptParams& rPara, Polygon& rPoly, Rectangle& rRect) const
{ // Gap/EscDir/EscPos/Angle
    Polygon aPol(2);
    Point aTl(rPoly[0]);
    aPol[0]=aTl;

    EscDir eEscDir;
    Point aEscPos;
    rPara.CalcEscPos(aTl,rRect,aEscPos,eEscDir);
    aPol[1]=aEscPos;

    if (!rPara.bFixedAngle) {
        // fehlende Implementation
    }
    rPoly=aPol;
}

void SdrCaptionObj::ImpCalcTail3(const ImpCaptParams& rPara, Polygon& rPoly, Rectangle& rRect) const
{ // Gap/EscDir/EscPos/Angle/LineLen
    Polygon aPol(3);
    Point aTl(rPoly[0]);
    aPol[0]=aTl;

    EscDir eEscDir;
    Point aEscPos;
    rPara.CalcEscPos(aTl,rRect,aEscPos,eEscDir);
    aPol[1]=aEscPos;
    aPol[2]=aEscPos;

    if (eEscDir==LKS || eEscDir==RTS) {
        if (rPara.bFitLineLen) {
            aPol[1].X()=(aTl.X()+aEscPos.X())/2;
        } else {
            if (eEscDir==LKS) aPol[1].X()-=rPara.nLineLen;
            else aPol[1].X()+=rPara.nLineLen;
        }
    } else {
        if (rPara.bFitLineLen) {
            aPol[1].Y()=(aTl.Y()+aEscPos.Y())/2;
        } else {
            if (eEscDir==OBN) aPol[1].Y()-=rPara.nLineLen;
            else aPol[1].Y()+=rPara.nLineLen;
        }
    }
    if (!rPara.bFixedAngle) {
        // fehlende Implementation
    }
    rPoly=aPol;
}

void SdrCaptionObj::ImpCalcTail4(const ImpCaptParams& rPara, Polygon& rPoly, Rectangle& rRect) const
{
    ImpCalcTail3(rPara,rPoly,rRect);
}

void SdrCaptionObj::ImpCalcTail(const ImpCaptParams& rPara, Polygon& rPoly, Rectangle& rRect) const
{
    switch (rPara.eType) {
        case SDRCAPT_TYPE1: ImpCalcTail1(rPara,rPoly,rRect); break;
        case SDRCAPT_TYPE2: ImpCalcTail2(rPara,rPoly,rRect); break;
        case SDRCAPT_TYPE3: ImpCalcTail3(rPara,rPoly,rRect); break;
        case SDRCAPT_TYPE4: ImpCalcTail4(rPara,rPoly,rRect); break;
    }
}

FASTBOOL SdrCaptionObj::BegCreate(SdrDragStat& rStat)
{
    if (aRect.IsEmpty()) return FALSE; // Create z.Zt. nur mit vorgegebenen Rect

    ImpCaptParams aPara;
    ImpGetCaptParams(aPara);
    aRect.SetPos(rStat.GetNow());
    aTailPoly[0]=rStat.GetStart();
    ImpCalcTail(aPara,aTailPoly,aRect);
    rStat.SetActionRect(aRect);
    return TRUE;
}

FASTBOOL SdrCaptionObj::MovCreate(SdrDragStat& rStat)
{
    ImpCaptParams aPara;
    ImpGetCaptParams(aPara);
    aRect.SetPos(rStat.GetNow());
    ImpCalcTail(aPara,aTailPoly,aRect);
    rStat.SetActionRect(aRect);
    bBoundRectDirty=TRUE;
    bSnapRectDirty=TRUE;
    return TRUE;
}

FASTBOOL SdrCaptionObj::EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd)
{
    ImpCaptParams aPara;
    ImpGetCaptParams(aPara);
    aRect.SetPos(rStat.GetNow());
    ImpCalcTail(aPara,aTailPoly,aRect);
    SetRectsDirty();
    return (eCmd==SDRCREATE_FORCEEND || rStat.GetPointAnz()>=2);
}

FASTBOOL SdrCaptionObj::BckCreate(SdrDragStat& /*rStat*/)
{
    return FALSE;
}

void SdrCaptionObj::BrkCreate(SdrDragStat& /*rStat*/)
{
}

basegfx::B2DPolyPolygon SdrCaptionObj::TakeCreatePoly(const SdrDragStat& /*rDrag*/) const
{
    basegfx::B2DPolyPolygon aRetval;
    const basegfx::B2DRange aRange(aRect.Left(), aRect.Top(), aRect.Right(), aRect.Bottom());
    aRetval.append(basegfx::tools::createPolygonFromRect(aRange));
    aRetval.append(aTailPoly.getB2DPolygon());
    return aRetval;
}

Pointer SdrCaptionObj::GetCreatePointer() const
{
    return Pointer(POINTER_DRAW_CAPTION);
}

void SdrCaptionObj::NbcMove(const Size& rSiz)
{
    SdrRectObj::NbcMove(rSiz);
    MovePoly(aTailPoly,rSiz);
    if(mbFixedTail)
        SetTailPos(GetFixedTailPos());
}

void SdrCaptionObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    SdrRectObj::NbcResize(rRef,xFact,yFact);
    ResizePoly(aTailPoly,rRef,xFact,yFact);
    ImpRecalcTail();
    if(mbFixedTail)
        SetTailPos(GetFixedTailPos());
}

void SdrCaptionObj::NbcSetRelativePos(const Point& rPnt)
{
    Point aRelPos0(aTailPoly.GetPoint(0)-aAnchor);
    Size aSiz(rPnt.X()-aRelPos0.X(),rPnt.Y()-aRelPos0.Y());
    NbcMove(aSiz); // Der ruft auch das SetRectsDirty()
}

Point SdrCaptionObj::GetRelativePos() const
{
    return aTailPoly.GetPoint(0)-aAnchor;
}

void SdrCaptionObj::NbcSetAnchorPos(const Point& rPnt)
{
    SdrRectObj::NbcSetAnchorPos(rPnt);
    // !!!!! fehlende Impl.
}

const Point& SdrCaptionObj::GetAnchorPos() const
{
    // !!!!! fehlende Impl.
    return SdrRectObj::GetAnchorPos();
}

void SdrCaptionObj::RecalcSnapRect()
{
    SdrRectObj::RecalcSnapRect();
    // #i32599#
    // Add BoundRect of TailPlygon in RecalcBoundRect(), not here
    // maSnapRect.Union(aTailPoly.GetBoundRect());
    // !!!!! fehlende Impl.
}

const Rectangle& SdrCaptionObj::GetSnapRect() const
{
    return SdrRectObj::GetSnapRect();
}

void SdrCaptionObj::NbcSetSnapRect(const Rectangle& rRect)
{
    // #i32599#
    // Move back to see the rectangle of the underlying SdrRectObj
    // as the SnapRect, without the TailPos. That simplifies SnapRect
    // handling again, if not allows it at all...
    SdrRectObj::NbcSetSnapRect(rRect);
}

const Rectangle& SdrCaptionObj::GetLogicRect() const
{
    return aRect;
}

void SdrCaptionObj::NbcSetLogicRect(const Rectangle& rRect)
{
    SdrRectObj::NbcSetLogicRect(rRect);
    ImpRecalcTail();
}

const Point& SdrCaptionObj::GetTailPos() const
{
    return aTailPoly[0];
}

void SdrCaptionObj::SetTailPos(const Point& rPos)
{
    if (aTailPoly.GetSize()==0 || aTailPoly[0]!=rPos) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        // #110094#-14 SendRepaintBroadcast();
        NbcSetTailPos(rPos);
        SetChanged();
        BroadcastObjectChange();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
    }
}

void SdrCaptionObj::NbcSetTailPos(const Point& rPos)
{
    aTailPoly[0]=rPos;
    ImpRecalcTail();
}

sal_uInt32 SdrCaptionObj::GetSnapPointCount() const
{
    // !!!!! fehlende Impl.
    return 0L;
}

Point SdrCaptionObj::GetSnapPoint(sal_uInt32 /*i*/) const
{
    // !!!!! fehlende Impl.
    return Point(0,0);
}

void SdrCaptionObj::SetModel(SdrModel* pNewModel)
{
    SdrRectObj::SetModel(pNewModel);
    ImpRecalcTail();
}

void SdrCaptionObj::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
{
    SdrRectObj::SFX_NOTIFY(rBC,rBCType,rHint,rHintType);
    ImpRecalcTail();
}

SdrObjGeoData* SdrCaptionObj::NewGeoData() const
{
    return new SdrCaptObjGeoData;
}

void SdrCaptionObj::SaveGeoData(SdrObjGeoData& rGeo) const
{
    SdrRectObj::SaveGeoData(rGeo);
    SdrCaptObjGeoData& rCGeo=(SdrCaptObjGeoData&)rGeo;
    rCGeo.aTailPoly=aTailPoly;
}

void SdrCaptionObj::RestGeoData(const SdrObjGeoData& rGeo)
{
    SdrRectObj::RestGeoData(rGeo);
    SdrCaptObjGeoData& rCGeo=(SdrCaptObjGeoData&)rGeo;
    aTailPoly=rCGeo.aTailPoly;
}

SdrObject* SdrCaptionObj::DoConvertToPolyObj(BOOL bBezier) const
{ // #42334# - Convert implementiert
    SdrObject* pRect=SdrRectObj::DoConvertToPolyObj(bBezier);
    SdrObject* pTail = ImpConvertMakeObj(basegfx::B2DPolyPolygon(aTailPoly.getB2DPolygon()), sal_False, bBezier);
    SdrObject* pRet=(pTail!=NULL) ? pTail : pRect;
    if (pTail!=NULL && pRect!=NULL) {
        FASTBOOL bInsRect=TRUE;
        FASTBOOL bInsTail=TRUE;
        SdrObjList* pOL=pTail->GetSubList();
        if (pOL!=NULL) { pRet=pRect; bInsTail=FALSE; }
        if (pOL==NULL) pOL=pRect->GetSubList();
        if (pOL!=NULL) { pRet=pRect; bInsRect=FALSE; }
        if (pOL==NULL) {
            SdrObjGroup* pGrp=new SdrObjGroup;
            pOL=pGrp->GetSubList();
            pRet=pGrp;
        }
        if (bInsRect) pOL->NbcInsertObject(pRect);
        if (bInsTail) pOL->NbcInsertObject(pTail,0);
    }
    return pRet;
}

// #i32599#
// Add own implementation for TRSetBaseGeometry to handle TailPos over changes.
void SdrCaptionObj::TRSetBaseGeometry(const basegfx::B2DHomMatrix& rMatrix, const basegfx::B2DPolyPolygon& /*rPolyPolygon*/)
{
    // break up matrix
    basegfx::B2DTuple aScale;
    basegfx::B2DTuple aTranslate;
    double fRotate, fShearX;
    rMatrix.decompose(aScale, aTranslate, fRotate, fShearX);

    // force metric to pool metric
    SfxMapUnit eMapUnit = pModel->GetItemPool().GetMetric(0);
    if(eMapUnit != SFX_MAPUNIT_100TH_MM)
    {
        switch(eMapUnit)
        {
            case SFX_MAPUNIT_TWIP :
            {
                // position
                aTranslate.setX(ImplMMToTwips(aTranslate.getX()));
                aTranslate.setY(ImplMMToTwips(aTranslate.getY()));

                // size
                aScale.setX(ImplMMToTwips(aScale.getX()));
                aScale.setY(ImplMMToTwips(aScale.getY()));

                break;
            }
            default:
            {
                DBG_ERROR("TRSetBaseGeometry: Missing unit translation to PoolMetric!");
            }
        }
    }

    // if anchor is used, make position relative to it
    if( pModel->IsWriter() )
    {
        if(GetAnchorPos().X() || GetAnchorPos().Y())
        {
            aTranslate += basegfx::B2DTuple(GetAnchorPos().X(), GetAnchorPos().Y());
        }
    }

    // build BaseRect
    Point aPoint(FRound(aTranslate.getX()), FRound(aTranslate.getY()));
    Rectangle aBaseRect(aPoint, Size(FRound(aScale.getX()), FRound(aScale.getY())));

    // set BaseRect, but rescue TailPos over this call
    const Point aTailPoint = GetTailPos();
    SetSnapRect(aBaseRect);
    SetTailPos(aTailPoint);
    ImpRecalcTail();
}

// eof
