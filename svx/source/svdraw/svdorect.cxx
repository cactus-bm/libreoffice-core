/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdorect.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 16:43:42 $
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

#include "svdorect.hxx"
#include <math.h>
#include <stdlib.h>
#include "xpool.hxx"
#include "xpoly.hxx"
#include "svdxout.hxx"
#include "svdattr.hxx"
#include "svdpool.hxx"
#include "svdtouch.hxx"
#include "svdtrans.hxx"
#include "svdio.hxx"
#include "svdetc.hxx"
#include "svddrag.hxx"
#include "svdmodel.hxx"
#include "svdpage.hxx"
#include "svdocapt.hxx" // fuer Import von SdrFileVersion 2
#include "svdpagv.hxx" // fuer
#include "svdview.hxx" // das
#include "svdundo.hxx" // Macro-Beispiel
#include "svdopath.hxx"
#include "svdglob.hxx"  // Stringcache
#include "svdstr.hrc"   // Objektname

#ifndef _SVX_XFLCLIT_HXX //autogen
#include <xflclit.hxx>
#endif

#ifndef _SVX_XLNCLIT_HXX //autogen
#include <xlnclit.hxx>
#endif

#ifndef _SVX_XLNWTIT_HXX //autogen
#include <xlnwtit.hxx>
#endif

#ifndef _SVX_SVDOIMP_HXX
#include "svdoimp.hxx"
#endif

#ifndef _SDR_PROPERTIES_RECTANGLEPROPERTIES_HXX
#include <svx/sdr/properties/rectangleproperties.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

sdr::properties::BaseProperties* SdrRectObj::CreateObjectSpecificProperties()
{
    return new sdr::properties::RectangleProperties(*this);
}

//////////////////////////////////////////////////////////////////////////////

TYPEINIT1(SdrRectObj,SdrTextObj);

SdrRectObj::SdrRectObj():
    pXPoly(NULL),
    bXPolyIsLine(FALSE)
{
    bClosedObj=TRUE;
}

SdrRectObj::SdrRectObj(const Rectangle& rRect):
    SdrTextObj(rRect),
    pXPoly(NULL),
    bXPolyIsLine(FALSE)
{
    bClosedObj=TRUE;
}

SdrRectObj::SdrRectObj(SdrObjKind eNewTextKind):
    SdrTextObj(eNewTextKind),
    pXPoly(NULL),
    bXPolyIsLine(FALSE)
{
    DBG_ASSERT(eTextKind==OBJ_TEXT || eTextKind==OBJ_TEXTEXT ||
               eTextKind==OBJ_OUTLINETEXT || eTextKind==OBJ_TITLETEXT,
               "SdrRectObj::SdrRectObj(SdrObjKind) ist nur fuer Textrahmen gedacht");
    bClosedObj=TRUE;
}

SdrRectObj::SdrRectObj(SdrObjKind eNewTextKind, const Rectangle& rRect):
    SdrTextObj(eNewTextKind,rRect),
    pXPoly(NULL),
    bXPolyIsLine(FALSE)
{
    DBG_ASSERT(eTextKind==OBJ_TEXT || eTextKind==OBJ_TEXTEXT ||
               eTextKind==OBJ_OUTLINETEXT || eTextKind==OBJ_TITLETEXT,
               "SdrRectObj::SdrRectObj(SdrObjKind,...) ist nur fuer Textrahmen gedacht");
    bClosedObj=TRUE;
}

SdrRectObj::SdrRectObj(SdrObjKind eNewTextKind, const Rectangle& rNewRect, SvStream& rInput, const String& rBaseURL, USHORT eFormat):
    SdrTextObj(eNewTextKind,rNewRect,rInput,rBaseURL,eFormat),
    pXPoly(NULL),
    bXPolyIsLine(FALSE)
{
    DBG_ASSERT(eTextKind==OBJ_TEXT || eTextKind==OBJ_TEXTEXT ||
               eTextKind==OBJ_OUTLINETEXT || eTextKind==OBJ_TITLETEXT,
               "SdrRectObj::SdrRectObj(SdrObjKind,...) ist nur fuer Textrahmen gedacht");
    bClosedObj=TRUE;
}

SdrRectObj::~SdrRectObj()
{
    if (pXPoly!=NULL) delete pXPoly;
}

void SdrRectObj::SetXPolyDirty()
{
    if (pXPoly!=NULL) {
        delete pXPoly;
        pXPoly=NULL;
    }
}

FASTBOOL SdrRectObj::PaintNeedsXPoly(long nEckRad) const
{
    FASTBOOL bNeed=aGeo.nDrehWink!=0 || aGeo.nShearWink!=0 || nEckRad!=0;
    return bNeed;
}

XPolygon SdrRectObj::ImpCalcXPoly(const Rectangle& rRect1, long nRad1, FASTBOOL bContour) const
{
    bContour=TRUE; // am 14.1.97 wg. Umstellung TakeContour ueber Mtf und Paint. Joe.
    XPolygon aXPoly(rRect1,nRad1,nRad1);
    if (bContour) {
        unsigned nPointAnz=aXPoly.GetPointCount();
        XPolygon aNeuPoly(nPointAnz+1);
        unsigned nShift=nPointAnz-2;
        if (nRad1!=0) nShift=nPointAnz-5;
        unsigned j=nShift;
        for (unsigned i=1; i<nPointAnz; i++) {
            aNeuPoly[i]=aXPoly[j];
            aNeuPoly.SetFlags(i,aXPoly.GetFlags(j));
            j++;
            if (j>=nPointAnz) j=1;
        }
        aNeuPoly[0]=rRect1.BottomCenter();
        aNeuPoly[nPointAnz]=aNeuPoly[0];
        aXPoly=aNeuPoly;
    }
    // Die Winkelangaben beziehen sich immer auf die linke obere Ecke von !aRect!
    if (aGeo.nShearWink!=0) ShearXPoly(aXPoly,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotateXPoly(aXPoly,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    return aXPoly;
}

void SdrRectObj::RecalcXPoly()
{
    pXPoly=new XPolygon(ImpCalcXPoly(aRect,GetEckenradius()));
}

const XPolygon& SdrRectObj::GetXPoly() const
{
    if (pXPoly==NULL) ((SdrRectObj*)this)->RecalcXPoly();
    return *pXPoly;
}

void SdrRectObj::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
{
    FASTBOOL bNoTextFrame=!IsTextFrame();
    rInfo.bResizeFreeAllowed=bNoTextFrame || aGeo.nDrehWink%9000==0;
    rInfo.bResizePropAllowed=TRUE;
    rInfo.bRotateFreeAllowed=TRUE;
    rInfo.bRotate90Allowed  =TRUE;
    rInfo.bMirrorFreeAllowed=bNoTextFrame;
    rInfo.bMirror45Allowed  =bNoTextFrame;
    rInfo.bMirror90Allowed  =bNoTextFrame;

    // allow transparence
    rInfo.bTransparenceAllowed = TRUE;

    // gradient depends on fillstyle
    XFillStyle eFillStyle = ((XFillStyleItem&)(GetObjectItem(XATTR_FILLSTYLE))).GetValue();
    rInfo.bGradientAllowed = (eFillStyle == XFILL_GRADIENT);

    rInfo.bShearAllowed     =bNoTextFrame;
    rInfo.bEdgeRadiusAllowed=TRUE;

    FASTBOOL bCanConv=!HasText() || ImpCanConvTextToCurve();
    if (bCanConv && !bNoTextFrame && !HasText()) {
        bCanConv=HasFill() || HasLine();
    }
    rInfo.bCanConvToPath    =bCanConv;
    rInfo.bCanConvToPoly    =bCanConv;
    rInfo.bCanConvToContour = (rInfo.bCanConvToPoly || LineGeometryUsageIsNecessary());
}

UINT16 SdrRectObj::GetObjIdentifier() const
{
    if (IsTextFrame()) return UINT16(eTextKind);
    else return UINT16(OBJ_RECT);
}

void SdrRectObj::RecalcBoundRect()
{
    aOutRect=GetSnapRect();
    long nLineWdt=ImpGetLineWdt();

    // #i25616#
    if(!LineIsOutsideGeometry())
    {
        nLineWdt++; nLineWdt/=2;
    }

    if (nLineWdt!=0) {
        long a=nLineWdt;
        if ((aGeo.nDrehWink!=0 || aGeo.nShearWink!=0) && GetEckenradius()==0) {
            a*=2; // doppelt, wegen evtl. spitzen Ecken
        }
        aOutRect.Left  ()-=a;
        aOutRect.Top   ()-=a;
        aOutRect.Right ()+=a;
        aOutRect.Bottom()+=a;
    }
    ImpAddShadowToBoundRect();
    ImpAddTextToBoundRect();
}

void SdrRectObj::TakeUnrotatedSnapRect(Rectangle& rRect) const
{
    rRect=aRect;
    if (aGeo.nShearWink!=0) {
        long nDst=Round((aRect.Bottom()-aRect.Top())*aGeo.nTan);
        if (aGeo.nShearWink>0) {
            Point aRef(rRect.TopLeft());
            rRect.Left()-=nDst;
            Point aTmpPt(rRect.TopLeft());
            RotatePoint(aTmpPt,aRef,aGeo.nSin,aGeo.nCos);
            aTmpPt-=rRect.TopLeft();
            rRect.Move(aTmpPt.X(),aTmpPt.Y());
        } else {
            rRect.Right()-=nDst;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// #i25616#

void SdrRectObj::ImpDoPaintRectObjShadow(XOutputDevice& rXOut, const SdrPaintInfoRec& rInfoRec,
    sal_Bool bPaintFill, sal_Bool bPaintLine) const
{
    const sal_Bool bHideContour(IsHideContour());
    const SfxItemSet& rSet = GetObjectItemSet();
    SfxItemSet aShadowSet(rSet);

    if(!bHideContour && ImpSetShadowAttributes(rSet, aShadowSet))
    {
        const sal_Bool bIsFillDraft(0 != (rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTFILL));

        // perepare ItemSet to avoid old XOut line drawing
        SfxItemSet aEmptySet(*rSet.GetPool());
        aEmptySet.Put(XLineStyleItem(XLINE_NONE));
        aEmptySet.Put(XFillStyleItem(XFILL_NONE));

        if( bIsFillDraft )
        {
            rXOut.SetFillAttr(aEmptySet);
        }
        else
        {
            rXOut.SetFillAttr(aShadowSet);
        }

        sal_uInt32 nXDist(((SdrShadowXDistItem&)(rSet.Get(SDRATTR_SHADOWXDIST))).GetValue());
        sal_uInt32 nYDist(((SdrShadowYDistItem&)(rSet.Get(SDRATTR_SHADOWYDIST))).GetValue());

        // avoid shadow line drawing in XOut
        rXOut.SetLineAttr(aEmptySet);

        if(bPaintFill)
        {
            // #100127# Output original geometry for metafiles
            ImpGraphicFill aFill( *this, rXOut, aShadowSet, true );
            const sal_Int32 nEckRad(GetEckenradius());

            if (PaintNeedsXPoly(nEckRad))
            {
                XPolygon aX(GetXPoly());
                aX.Move(nXDist,nYDist);
                rXOut.DrawXPolygon(aX);
            }
            else
            {
                Rectangle aR(aRect);
                aR.Move(nXDist,nYDist);
                rXOut.DrawRect(aR,USHORT(2*nEckRad),USHORT(2*nEckRad));
            }
        }

        if(bPaintLine)
        {
            // prepare line geometry
            const sal_Bool bIsLineDraft(0 != (rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTLINE));

            // #b4899532# if not filled but fill draft, avoid object being invisible in using
            // a hair linestyle and COL_LIGHTGRAY
            SfxItemSet aItemSet(rSet);
            if(bIsFillDraft && XLINE_NONE == ((const XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue())
            {
                ImpPrepareLocalItemSetForDraftLine(aItemSet);
            }

            // prepare line geometry
            ::std::auto_ptr< SdrLineGeometry > pLineGeometry(ImpPrepareLineGeometry(rXOut, aItemSet, bIsLineDraft));

            // new shadow line drawing
            if( pLineGeometry.get() )
            {
                // draw the line geometry
                ImpDrawShadowLineGeometry(rXOut, aItemSet, *pLineGeometry);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// #i25616#

void SdrRectObj::ImpDoPaintRectObj(XOutputDevice& rXOut, const SdrPaintInfoRec& rInfoRec,
    sal_Bool bPaintFill, sal_Bool bPaintLine) const
{
    const sal_Bool bHideContour(IsHideContour());

    if(!bHideContour)
    {
        // prepare ItemSet of this object
        const SfxItemSet& rSet = GetObjectItemSet();

        // perepare ItemSet to avoid old XOut line drawing
        SfxItemSet aEmptySet(*rSet.GetPool());
        aEmptySet.Put(XLineStyleItem(XLINE_NONE));
        aEmptySet.Put(XFillStyleItem(XFILL_NONE));

        // Before here the LineAttr were set: if(pLineAttr) rXOut.SetLineAttr(*pLineAttr);
        rXOut.SetLineAttr(aEmptySet);

        const sal_Bool bIsFillDraft(0 != (rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTFILL));
        rXOut.SetFillAttr( bIsFillDraft ? aEmptySet : rSet );

        if (!bHideContour && bPaintFill)
        {
            // #100127# Output original geometry for metafiles
            ImpGraphicFill aFill( *this, rXOut, bIsFillDraft ? aEmptySet : rSet );
            const sal_Int32 nEckRad(GetEckenradius());

            if (PaintNeedsXPoly(nEckRad))
            {
                rXOut.DrawXPolygon(GetXPoly());
            }
            else
            {
                DBG_ASSERT(nEckRad==0,"SdrRectObj::DoPaintObject(): XOut.DrawRect() unterstuetz kein Eckenradius!");
                rXOut.DrawRect(aRect/*,USHORT(2*nEckRad),USHORT(2*nEckRad)*/);
            }
        }

        DBG_ASSERT(aRect.GetWidth()>1 && aRect.GetHeight()>1,"SdrRectObj::DoPaintObject(): Rect hat Nullgroesse (oder negativ)!");

        // Own line drawing
        if( !bHideContour && bPaintLine)
        {
            // prepare line geometry
            const sal_Bool bIsLineDraft(0 != (rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTLINE));

            // #b4899532# if not filled but fill draft, avoid object being invisible in using
            // a hair linestyle and COL_LIGHTGRAY
            SfxItemSet aItemSet(rSet);
            if(bIsFillDraft && XLINE_NONE == ((const XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue())
            {
                ImpPrepareLocalItemSetForDraftLine(aItemSet);
            }

            // prepare line geometry
            ::std::auto_ptr< SdrLineGeometry > pLineGeometry( ImpPrepareLineGeometry(rXOut, aItemSet, bIsLineDraft) );

            if( pLineGeometry.get() )
            {
                // draw the line geometry
                ImpDrawColorLineGeometry(rXOut, aItemSet, *pLineGeometry);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

sal_Bool SdrRectObj::DoPaintObject(XOutputDevice& rXOut, const SdrPaintInfoRec& rInfoRec) const
{
    if (bTextFrame && aGeo.nShearWink!=0) {
        DBG_WARNING("Shearwinkel vom TextFrame innerhalb von SdrRectObj::DoPaintObject() auf 0 gesetzt");
        ((SdrRectObj*)this)->ImpCheckShear();
        ((SdrRectObj*)this)->SetRectsDirty();
    }

    sal_Bool bOk(sal_True);

    // draw shadow
    ImpDoPaintRectObjShadow(rXOut, rInfoRec, sal_True, sal_True);

    // draw geometry
    ImpDoPaintRectObj(rXOut, rInfoRec, sal_True, sal_True);

    // draw text
    if(HasText() && !LineIsOutsideGeometry())
    {
        bOk = SdrTextObj::DoPaintObject(rXOut,rInfoRec);
    }

    return bOk;
}

SdrObject* SdrRectObj::ImpCheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer, FASTBOOL bForceFilled, FASTBOOL bForceTol) const
{
    if (pVisiLayer!=NULL && !pVisiLayer->IsSet(nLayerId)) return NULL;
    INT32 nMyTol=nTol;
    FASTBOOL bFilled=bForceFilled || HasFill();
    FASTBOOL bPickThrough=pModel!=NULL && pModel->IsPickThroughTransparentTextFrames();
    if (bTextFrame && !bPickThrough) bFilled=TRUE;
    FASTBOOL bLine=HasLine();

    INT32 nWdt=bLine ? ImpGetLineWdt() :0; // Halbe Strichstaerke

    // #i25616#
    if(nWdt && !LineIsOutsideGeometry())
    {
        nWdt /= 2;
    }

    long nBoundWdt=aRect.GetWidth()-1;
    long nBoundHgt=aRect.GetHeight()-1;
    if (bFilled && nBoundWdt>short(nTol) && nBoundHgt>short(nTol) && Abs(aGeo.nShearWink)<=4500) {
        if (!bForceTol && !bTextFrame ) nMyTol=0; // Keine Toleranz noetig hier
    }
    if (nWdt>nMyTol && (!bTextFrame || pEdtOutl==NULL)) nMyTol=nWdt; // Bei dicker Umrandung keine Toleranz noetig, ausser wenn bei TextEdit
    Rectangle aR(aRect);
    if (nMyTol!=0 && bFilled) {
        aR.Left  ()-=nMyTol;
        aR.Top   ()-=nMyTol;
        aR.Right ()+=nMyTol;
        aR.Bottom()+=nMyTol;
    }

    if (bFilled || bLine || bTextFrame) { // Bei TextFrame so tun, alsob Linie da
        unsigned nCnt=0;
        INT32 nXShad=0,nYShad=0;
        long nEckRad=/*bTextFrame ? 0 :*/ GetEckenradius();
        do { // 1 Durchlauf, bei Schatten 2 Durchlaeufe.
            if (nCnt!=0) aR.Move(nXShad,nYShad);
            if (aGeo.nDrehWink!=0 || aGeo.nShearWink!=0 || nEckRad!=0 || !bFilled) {
                Polygon aPol(aR);
                if (nEckRad!=0) {
                    INT32 nRad=nEckRad;
                    if (bFilled) nRad+=nMyTol; // um korrekt zu sein ...
                    XPolygon aXPoly(ImpCalcXPoly(aR,nRad));
//BFS09                 aPol=XOutCreatePolygon(aXPoly,NULL);
                    aPol=XOutCreatePolygon(aXPoly);
                } else {
                    if (aGeo.nShearWink!=0) ShearPoly(aPol,aRect.TopLeft(),aGeo.nTan);
                    if (aGeo.nDrehWink!=0) RotatePoly(aPol,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
                }
                if (bFilled) {
                    if (IsPointInsidePoly(aPol,rPnt)) return (SdrObject*)this;
                } else {
                    Rectangle aTouchRect(rPnt.X()-nMyTol,rPnt.Y()-nMyTol,rPnt.X()+nMyTol,rPnt.Y()+nMyTol);
                    if (IsRectTouchesLine(aPol,aTouchRect)) return (SdrObject*)this;
                }
            } else {
                if (aR.IsInside(rPnt)) return (SdrObject*)this;
            }
        } while (nCnt++==0 && ImpGetShadowDist(nXShad,nYShad));
    }
    FASTBOOL bCheckText=TRUE;
    if (bCheckText && HasText() && (!bTextFrame || bPickThrough)) {
        return SdrTextObj::CheckHit(rPnt,nTol,pVisiLayer);
    }
    return NULL;
}

SdrObject* SdrRectObj::CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const
{
    return ImpCheckHit(rPnt,nTol,pVisiLayer,FALSE/*,bTextFrame*/);
}

void SdrRectObj::TakeObjNameSingul(XubString& rName) const
{
    if (IsTextFrame())
    {
        SdrTextObj::TakeObjNameSingul(rName);
    }
    else
    {
        USHORT nResId=STR_ObjNameSingulRECT;
        if (aGeo.nShearWink!=0) {
            nResId+=4;  // Parallelogramm oder Raute
            // Raute ist nicht, weil Shear die vertikalen Kanten verlaengert!
            // Wenn Zeit ist, werde ich das mal berechnen.
        } else {
            if (aRect.GetWidth()==aRect.GetHeight()) nResId+=2; // Quadrat
        }
        if (GetEckenradius()!=0) nResId+=8; // abgerundet
        rName=ImpGetResStr(nResId);

        String aName( GetName() );
        if(aName.Len())
        {
            rName += sal_Unicode(' ');
            rName += sal_Unicode('\'');
            rName += aName;
            rName += sal_Unicode('\'');
        }
    }
}

void SdrRectObj::TakeObjNamePlural(XubString& rName) const
{
    if (IsTextFrame()) SdrTextObj::TakeObjNamePlural(rName);
    else {
        USHORT nResId=STR_ObjNamePluralRECT;
        if (aGeo.nShearWink!=0) {
            nResId+=4;  // Parallelogramm oder Raute
        } else {
            if (aRect.GetWidth()==aRect.GetHeight()) nResId+=2; // Quadrat
        }
        if (GetEckenradius()!=0) nResId+=8; // abgerundet
        rName=ImpGetResStr(nResId);
    }
}

void SdrRectObj::operator=(const SdrObject& rObj)
{
    SdrTextObj::operator=(rObj);
}

void SdrRectObj::TakeXorPoly(XPolyPolygon& rPoly, FASTBOOL /*bDetail*/) const
{
    rPoly=XPolyPolygon(ImpCalcXPoly(aRect,GetEckenradius()));
}

void SdrRectObj::TakeContour(XPolyPolygon& rPoly) const
{
    SdrTextObj::TakeContour(rPoly);
}

//#110094#-12
//void SdrRectObj::TakeContour(XPolyPolygon& rXPoly, SdrContourType eType) const
//{
//}

void SdrRectObj::RecalcSnapRect()
{
    long nEckRad=GetEckenradius();
    if ((aGeo.nDrehWink!=0 || aGeo.nShearWink!=0) && nEckRad!=0) {
        maSnapRect=GetXPoly().GetBoundRect();
    } else {
        SdrTextObj::RecalcSnapRect();
    }
}

void SdrRectObj::NbcSetSnapRect(const Rectangle& rRect)
{
    SdrTextObj::NbcSetSnapRect(rRect);
    SetXPolyDirty();
}

void SdrRectObj::NbcSetLogicRect(const Rectangle& rRect)
{
    SdrTextObj::NbcSetLogicRect(rRect);
    SetXPolyDirty();
}

USHORT SdrRectObj::GetHdlCount() const
{
    return 9;
}

SdrHdl* SdrRectObj::GetHdl(USHORT nHdlNum) const
{
    SdrHdl* pH=NULL;
    Point aPnt;
    SdrHdlKind eKind=HDL_MOVE;
    switch (nHdlNum) {
        case 0: {
            long a=GetEckenradius();
            long b=Max(aRect.GetWidth(),aRect.GetHeight())/2; // Wird aufgerundet, da GetWidth() eins draufaddiert
            if (a>b) a=b;
            if (a<0) a=0;
            aPnt=aRect.TopLeft();
            aPnt.X()+=a;
            eKind=HDL_CIRC;
        } break; // Eckenradius
        case 1: aPnt=aRect.TopLeft();      eKind=HDL_UPLFT; break; // Oben links
        case 2: aPnt=aRect.TopCenter();    eKind=HDL_UPPER; break; // Oben
        case 3: aPnt=aRect.TopRight();     eKind=HDL_UPRGT; break; // Oben rechts
        case 4: aPnt=aRect.LeftCenter();   eKind=HDL_LEFT ; break; // Links
        case 5: aPnt=aRect.RightCenter();  eKind=HDL_RIGHT; break; // Rechts
        case 6: aPnt=aRect.BottomLeft();   eKind=HDL_LWLFT; break; // Unten links
        case 7: aPnt=aRect.BottomCenter(); eKind=HDL_LOWER; break; // Unten
        case 8: aPnt=aRect.BottomRight();  eKind=HDL_LWRGT; break; // Unten rechts
    }
    if (aGeo.nShearWink!=0) ShearPoint(aPnt,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotatePoint(aPnt,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    if (eKind!=HDL_MOVE) {
        pH=new SdrHdl(aPnt,eKind);
        pH->SetObj((SdrObject*)this);
        pH->SetDrehWink(aGeo.nDrehWink);
    }
    return pH;
}

FASTBOOL SdrRectObj::HasSpecialDrag() const
{
    return TRUE;
}

FASTBOOL SdrRectObj::BegDrag(SdrDragStat& rDrag) const
{
    FASTBOOL bRad=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    if (bRad) {
        rDrag.SetEndDragChangesAttributes(TRUE);
        return TRUE;
    } else {
        return SdrTextObj::BegDrag(rDrag);
    }
}

FASTBOOL SdrRectObj::MovDrag(SdrDragStat& rDrag) const
{
    FASTBOOL bRad=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    if (bRad) {
        return TRUE;
    } else {
        return SdrTextObj::MovDrag(rDrag);
    }
}

FASTBOOL SdrRectObj::EndDrag(SdrDragStat& rDrag)
{
    FASTBOOL bRad=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    if (bRad) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        // #110094#-14 SendRepaintBroadcast();
        Point aPt(rDrag.GetNow());
        if (aGeo.nDrehWink!=0) RotatePoint(aPt,aRect.TopLeft(),-aGeo.nSin,aGeo.nCos); // -sin fuer Umkehrung
        // Shear nicht noetig, da Pt auf einer Linie mit dem RefPt (LiOb Ecke des Rect)
        long nRad=aPt.X()-aRect.Left();
        if (nRad<0) nRad=0;
        long nAltRad=GetEckenradius();
        if (nRad!=nAltRad) NbcSetEckenradius(nRad);
        SetChanged();
        SetRectsDirty();
        SetXPolyDirty();
        BroadcastObjectChange();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
        return TRUE;
    } else {
        return SdrTextObj::EndDrag(rDrag);
    }
}

void SdrRectObj::BrkDrag(SdrDragStat& rDrag) const
{
    FASTBOOL bRad=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    if (bRad) {
    } else {
        SdrTextObj::BrkDrag(rDrag);
    }
}

XubString SdrRectObj::GetDragComment(const SdrDragStat& rDrag, FASTBOOL bUndoDragComment, FASTBOOL bCreateComment) const
{
    if(bCreateComment)
        return String();

    BOOL bRad(rDrag.GetHdl() && rDrag.GetHdl()->GetKind() == HDL_CIRC);

    if(bRad)
    {
        Point aPt(rDrag.GetNow());

        // -sin fuer Umkehrung
        if(aGeo.nDrehWink)
            RotatePoint(aPt, aRect.TopLeft(), -aGeo.nSin, aGeo.nCos);

        INT32 nRad(aPt.X() - aRect.Left());

        if(nRad < 0)
            nRad = 0;

        XubString aStr;

        ImpTakeDescriptionStr(STR_DragRectEckRad, aStr);
        aStr.AppendAscii(" (");
        aStr += GetMetrStr(nRad);
        aStr += sal_Unicode(')');

        return aStr;
    }
    else
    {
        return SdrTextObj::GetDragComment(rDrag, bUndoDragComment, FALSE);
    }
}

void SdrRectObj::TakeDragPoly(const SdrDragStat& rDrag, XPolyPolygon& rXPP) const
{
    rXPP.Clear();
    FASTBOOL bRad=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    //FASTBOOL bRectSiz=!bRad;
    if (bRad) {
        Point aPt(rDrag.GetNow());
        if (aGeo.nDrehWink!=0) RotatePoint(aPt,aRect.TopLeft(),-aGeo.nSin,aGeo.nCos); // -sin fuer Umkehrung
        // Shear nicht noetig, da Pt auf einer Linie mit dem RefPt (LiOb Ecke des Rect)
        long nRad=aPt.X()-aRect.Left();
        if (nRad<0) nRad=0;
        rXPP.Insert(ImpCalcXPoly(aRect,nRad));
    } else {
        rXPP.Insert(ImpCalcXPoly(ImpDragCalcRect(rDrag),GetEckenradius()));
    }
}

void SdrRectObj::TakeCreatePoly(const SdrDragStat& rDrag, XPolyPolygon& rXPP) const
{
    Rectangle aRect1;
    rDrag.TakeCreateRect(aRect1);
    aRect1.Justify();
    rXPP=XPolyPolygon(ImpCalcXPoly(aRect1,GetEckenradius()));
}

Pointer SdrRectObj::GetCreatePointer() const
{
    if (IsTextFrame()) return Pointer(POINTER_DRAW_TEXT);
    return Pointer(POINTER_DRAW_RECT);
}

void SdrRectObj::NbcMove(const Size& rSiz)
{
    SdrTextObj::NbcMove(rSiz);
    SetXPolyDirty();
}

void SdrRectObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    SdrTextObj::NbcResize(rRef,xFact,yFact);
    SetXPolyDirty();
}

void SdrRectObj::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
{
    SdrTextObj::NbcRotate(rRef,nWink,sn,cs);
    SetXPolyDirty();
}

void SdrRectObj::NbcShear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    SdrTextObj::NbcShear(rRef,nWink,tn,bVShear);
    SetXPolyDirty();
}

void SdrRectObj::NbcMirror(const Point& rRef1, const Point& rRef2)
{
    SdrTextObj::NbcMirror(rRef1,rRef2);
    SetXPolyDirty();
}

FASTBOOL SdrRectObj::DoMacro(const SdrObjMacroHitRec& rRec)
{
    return SdrTextObj::DoMacro(rRec);
}

XubString SdrRectObj::GetMacroPopupComment(const SdrObjMacroHitRec& rRec) const
{
    return SdrTextObj::GetMacroPopupComment(rRec);
}

SdrGluePoint SdrRectObj::GetVertexGluePoint(USHORT nPosNum) const
{
    INT32 nWdt = ImpGetLineWdt(); // #i25616# ((XLineWidthItem&)(GetObjectItem(XATTR_LINEWIDTH))).GetValue();

    // #i25616#
    if(!LineIsOutsideGeometry())
    {
        nWdt++;
        nWdt /= 2;
    }

    Point aPt;
    switch (nPosNum) {
        case 0: aPt=aRect.TopCenter();    aPt.Y()-=nWdt; break;
        case 1: aPt=aRect.RightCenter();  aPt.X()+=nWdt; break;
        case 2: aPt=aRect.BottomCenter(); aPt.Y()+=nWdt; break;
        case 3: aPt=aRect.LeftCenter();   aPt.X()-=nWdt; break;
    }
    if (aGeo.nShearWink!=0) ShearPoint(aPt,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotatePoint(aPt,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    aPt-=GetSnapRect().Center();
    SdrGluePoint aGP(aPt);
    aGP.SetPercent(FALSE);
    return aGP;
}

SdrGluePoint SdrRectObj::GetCornerGluePoint(USHORT nPosNum) const
{
    INT32 nWdt = ImpGetLineWdt(); // #i25616# ((XLineWidthItem&)(GetObjectItem(XATTR_LINEWIDTH))).GetValue();

    // #i25616#
    if(!LineIsOutsideGeometry())
    {
        nWdt++;
        nWdt /= 2;
    }

    Point aPt;
    switch (nPosNum) {
        case 0: aPt=aRect.TopLeft();     aPt.X()-=nWdt; aPt.Y()-=nWdt; break;
        case 1: aPt=aRect.TopRight();    aPt.X()+=nWdt; aPt.Y()-=nWdt; break;
        case 2: aPt=aRect.BottomRight(); aPt.X()+=nWdt; aPt.Y()+=nWdt; break;
        case 3: aPt=aRect.BottomLeft();  aPt.X()-=nWdt; aPt.Y()+=nWdt; break;
    }
    if (aGeo.nShearWink!=0) ShearPoint(aPt,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotatePoint(aPt,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    aPt-=GetSnapRect().Center();
    SdrGluePoint aGP(aPt);
    aGP.SetPercent(FALSE);
    return aGP;
}

SdrObject* SdrRectObj::DoConvertToPolyObj(BOOL bBezier) const
{
    XPolygon aXP(ImpCalcXPoly(aRect,GetEckenradius()));
    { // #40608# Nur Uebergangsweise bis zum neuen TakeContour()
        aXP.Remove(0,1);
        aXP[aXP.GetPointCount()-1]=aXP[0];
    }
    SdrObject* pRet=NULL;
    if (!IsTextFrame() || HasFill() || HasLine()) {
        pRet=ImpConvertMakeObj(XPolyPolygon(aXP),TRUE,bBezier);
    }
    pRet=ImpConvertAddText(pRet,bBezier);
    return pRet;
}

void SdrRectObj::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
{
    SdrTextObj::SFX_NOTIFY(rBC,rBCType,rHint,rHintType);
    SetXPolyDirty(); // wg. Eckenradius
}

// #109872#
//SdrObjGeoData* SdrRectObj::NewGeoData() const
//{ // etwas umstaendlicher, damit's vielleicht unter Chicago durchgeht
//  SdrObjGeoData* pGeo=new SdrRectObjGeoData;
//  return pGeo;
//}

// #109872#
//void SdrRectObj::SaveGeoData(SdrObjGeoData& rGeo) const
//{
//  SdrTextObj::SaveGeoData(rGeo);
//  SdrRectObjGeoData& rRGeo=(SdrRectObjGeoData&)rGeo;
//  rRGeo.nEckRad=GetEckenradius();
//}

void SdrRectObj::RestGeoData(const SdrObjGeoData& rGeo)
{
    SdrTextObj::RestGeoData(rGeo);
// #109872#
//  SdrRectObjGeoData& rRGeo=(SdrRectObjGeoData&)rGeo;
//  long nAltRad=GetEckenradius();
//  if (rRGeo.nEckRad!=nAltRad) NbcSetEckenradius(rRGeo.nEckRad);
    SetXPolyDirty();
}

//BFS01void SdrRectObj::WriteData(SvStream& rOut) const
//BFS01{
//BFS01 SdrTextObj::WriteData(rOut);
//BFS01 SdrDownCompat aCompat(rOut,STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
//BFS01#ifdef DBG_UTIL
//BFS01 aCompat.SetID("SdrRectObj");
//BFS01#endif
//BFS01}

//BFS01void SdrRectObj::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
//BFS01{
//BFS01 if (rIn.GetError()!=0) return;
//BFS01 SdrTextObj::ReadData(rHead,rIn);
//BFS01 if (IsTextFrame() && rHead.GetVersion()<3 && !HAS_BASE(SdrCaptionObj,this)) {
//BFS01     // Bis einschl. Version 2 wurden Textrahmen mit SdrTextObj dargestellt, ausser CaptionObj
//BFS01     SfxItemPool* pPool=GetItemPool();
//BFS01     if (pPool!=NULL) {
//BFS01         // Umrandung und Hintergrund des importierten Textrahmens ausschalten
//BFS01         SfxItemSet aSet(*pPool);
//BFS01         aSet.Put(XFillColorItem(String(),Color(COL_WHITE))); // Falls einer auf Solid umschaltet
//BFS01         aSet.Put(XFillStyleItem(XFILL_NONE));
//BFS01         aSet.Put(XLineColorItem(String(),Color(COL_BLACK))); // Falls einer auf Solid umschaltet
//BFS01         aSet.Put(XLineStyleItem(XLINE_NONE));
//BFS01
//BFS01         SetObjectItemSet(aSet);
//BFS01     }
//BFS01 } else {
//BFS01     SdrDownCompat aCompat(rIn,STREAM_READ); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
//BFS01#ifdef DBG_UTIL
//BFS01     aCompat.SetID("SdrRectObj");
//BFS01#endif
//BFS01     if (rHead.GetVersion()<=5) {
//BFS01         long nEckRad;
//BFS01         rIn>>nEckRad;
//BFS01         long nAltRad=GetEckenradius();
//BFS01         if (nEckRad!=nAltRad) NbcSetEckenradius(nEckRad);
//BFS01     }
//BFS01 }
//BFS01 SetXPolyDirty();
//BFS01}

// eof
