/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdocirc.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: vg $ $Date: 2006-11-21 16:56:09 $
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

#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif

#ifndef _BIGINT_HXX //autogen
#include <tools/bigint.hxx>
#endif

#ifndef _SVX_XLNWTIT_HXX //autogen
#include <xlnwtit.hxx>
#endif

#ifndef _SVX_XLNEDWIT_HXX //autogen
#include <xlnedwit.hxx>
#endif

#ifndef _SVX_XLNSTWIT_HXX //autogen
#include <xlnstwit.hxx>
#endif

#ifndef _SVX_XLNSTIT_HXX //autogen
#include <xlnstit.hxx>
#endif

#ifndef _SVX_XLNEDIT_HXX //autogen
#include <xlnedit.hxx>
#endif

#include "svdocirc.hxx"
#include <math.h>
#include "xpool.hxx"
#include "svdtouch.hxx"
#include "svdxout.hxx"
#include "svdattr.hxx"
#include "svdpool.hxx"
#include "svdattrx.hxx"
#include "svdtrans.hxx"
#include "svdetc.hxx"
#include "svddrag.hxx"
#include "svdmodel.hxx"
#include "svdpage.hxx"
#include "svdopath.hxx" // fuer die Objektkonvertierung
#include "svdview.hxx"  // Zum Draggen (Ortho)
#include "svdglob.hxx"   // StringCache
#include "svdstr.hrc"    // Objektname

#ifndef _EEITEM_HXX
#include "eeitem.hxx"
#endif

#ifndef _SVX_SVDOIMP_HXX
#include "svdoimp.hxx"
#endif

#ifndef _SDR_PROPERTIES_CIRCLEPROPERTIES_HXX
#include <svx/sdr/properties/circleproperties.hxx>
#endif

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

void SetWinkPnt(const Rectangle& rR, long nWink, Point& rPnt)
{
    Point aCenter(rR.Center());
    long nWdt=rR.Right()-rR.Left();
    long nHgt=rR.Bottom()-rR.Top();
    long nMaxRad=((nWdt>nHgt ? nWdt : nHgt)+1) /2;
    double a;
    a=nWink*nPi180;
    rPnt=Point(Round(cos(a)*nMaxRad),-Round(sin(a)*nMaxRad));
    if (nWdt==0) rPnt.X()=0;
    if (nHgt==0) rPnt.Y()=0;
    if (nWdt!=nHgt) {
        if (nWdt>nHgt) {
            if (nWdt!=0) {
                // eventuelle Ueberlaeufe bei sehr grossen Objekten abfangen (Bug 23384)
                if (Abs(nHgt)>32767 || Abs(rPnt.Y())>32767) {
                    rPnt.Y()=BigMulDiv(rPnt.Y(),nHgt,nWdt);
                } else {
                    rPnt.Y()=rPnt.Y()*nHgt/nWdt;
                }
            }
        } else {
            if (nHgt!=0) {
                // eventuelle Ueberlaeufe bei sehr grossen Objekten abfangen (Bug 23384)
                if (Abs(nWdt)>32767 || Abs(rPnt.X())>32767) {
                    rPnt.X()=BigMulDiv(rPnt.X(),nWdt,nHgt);
                } else {
                    rPnt.X()=rPnt.X()*nWdt/nHgt;
                }
            }
        }
    }
    rPnt+=aCenter;
}

//////////////////////////////////////////////////////////////////////////////

sdr::properties::BaseProperties* SdrCircObj::CreateObjectSpecificProperties()
{
    return new sdr::properties::CircleProperties(*this);
}

//////////////////////////////////////////////////////////////////////////////

TYPEINIT1(SdrCircObj,SdrRectObj);

SdrCircObj::SdrCircObj(SdrObjKind eNewKind)
{
    mbPolygonIsLine = false;
    nStartWink=0;
    nEndWink=36000;
    eKind=eNewKind;
    bClosedObj=eNewKind!=OBJ_CARC;
}

SdrCircObj::SdrCircObj(SdrObjKind eNewKind, const Rectangle& rRect):
    SdrRectObj(rRect)
{
    mbPolygonIsLine = false;
    nStartWink=0;
    nEndWink=36000;
    eKind=eNewKind;
    bClosedObj=eNewKind!=OBJ_CARC;
}

SdrCircObj::SdrCircObj(SdrObjKind eNewKind, const Rectangle& rRect, long nNewStartWink, long nNewEndWink):
    SdrRectObj(rRect)
{
    mbPolygonIsLine = false;
    long nWinkDif=nNewEndWink-nNewStartWink;
    nStartWink=NormAngle360(nNewStartWink);
    nEndWink=NormAngle360(nNewEndWink);
    if (nWinkDif==36000) nEndWink+=nWinkDif; // Vollkreis
    eKind=eNewKind;
    bClosedObj=eNewKind!=OBJ_CARC;
}

SdrCircObj::~SdrCircObj()
{
}

void SdrCircObj::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
{
    FASTBOOL bCanConv=!HasText() || ImpCanConvTextToCurve();
    rInfo.bEdgeRadiusAllowed    = FALSE;
    rInfo.bCanConvToPath=bCanConv;
    rInfo.bCanConvToPoly=bCanConv;
    rInfo.bCanConvToContour = !IsFontwork() && (rInfo.bCanConvToPoly || LineGeometryUsageIsNecessary());
}

UINT16 SdrCircObj::GetObjIdentifier() const
{
    return UINT16(eKind);
}

FASTBOOL SdrCircObj::PaintNeedsXPolyCirc() const
{
    // XPoly ist notwendig fuer alle gedrehten Ellipsenobjekte,
    // fuer alle Kreis- und Ellipsenabschnitte
    // und wenn nicht WIN dann (erstmal) auch fuer Kreis-/Ellipsenausschnitte
    // und Kreis-/Ellipsenboegen (wg. Genauigkeit)
    FASTBOOL bNeed=aGeo.nDrehWink!=0 || aGeo.nShearWink!=0 || eKind==OBJ_CCUT;
#ifndef WIN
    // Wenn nicht Win, dann fuer alle ausser Vollkreis (erstmal!!!)
    if (eKind!=OBJ_CIRC) bNeed=TRUE;
#endif

    const SfxItemSet& rSet = GetObjectItemSet();
    if(!bNeed)
    {
        // XPoly ist notwendig fuer alles was nicht LineSolid oder LineNone ist
        XLineStyle eLine = ((XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue();
        bNeed = eLine != XLINE_NONE && eLine != XLINE_SOLID;

        // XPoly ist notwendig fuer dicke Linien
        if(!bNeed && eLine != XLINE_NONE)
            bNeed = ((XLineWidthItem&)(rSet.Get(XATTR_LINEWIDTH))).GetValue() != 0;

        // XPoly ist notwendig fuer Kreisboegen mit Linienenden
        if(!bNeed && eKind == OBJ_CARC)
        {
            // Linienanfang ist da, wenn StartPolygon und StartWidth!=0
            bNeed=((XLineStartItem&)(rSet.Get(XATTR_LINESTART))).GetLineStartValue().count() != 0L &&
                  ((XLineStartWidthItem&)(rSet.Get(XATTR_LINESTARTWIDTH))).GetValue() != 0;

            if(!bNeed)
            {
                // Linienende ist da, wenn EndPolygon und EndWidth!=0
                bNeed = ((XLineEndItem&)(rSet.Get(XATTR_LINEEND))).GetLineEndValue().count() != 0L &&
                        ((XLineEndWidthItem&)(rSet.Get(XATTR_LINEENDWIDTH))).GetValue() != 0;
            }
        }
    }

    // XPoly ist notwendig, wenn Fill !=None und !=Solid
    if(!bNeed && eKind != OBJ_CARC)
    {
        XFillStyle eFill=((XFillStyleItem&)(rSet.Get(XATTR_FILLSTYLE))).GetValue();
        bNeed = eFill != XFILL_NONE && eFill != XFILL_SOLID;
    }

    if(!bNeed && eKind != OBJ_CIRC && nStartWink == nEndWink)
        bNeed=TRUE; // Weil sonst Vollkreis gemalt wird

    return bNeed;
}

XPolygon SdrCircObj::ImpCalcXPolyCirc(const Rectangle& rRect1, long nStart, long nEnd, FASTBOOL bContour) const
{
    bContour=TRUE; // am 14.1.97 wg. Umstellung TakeContour ueber Mtf und Paint. Joe.
    long rx=rRect1.GetWidth()/2;  // Da GetWidth()/GetHeight() jeweils 1
    long ry=rRect1.GetHeight()/2; // draufaddieren wird korrekt gerundet.
    long a=0,e=3600;
    if (eKind!=OBJ_CIRC) {
        a=nStart/10;
        e=nEnd/10;
        if (bContour) {
            // Drehrichtung umkehren, damit Richtungssinn genauso wie Rechteck
            rx=-rx;
            a=1800-a; if (a<0) a+=3600;
            e=1800-e; if (e<0) e+=3600;
            long nTmp=a;
            a=e;
            e=nTmp;
        }
    } else {
        if (bContour) {
            long nTmp=rx;
            rx=ry;
            ry=nTmp;
            // und auch noch die Drehrichtung aendern
            ry=-ry;
        }
    }
    ((SdrCircObj*)this)->mbPolygonIsLine=eKind==OBJ_CARC;
    const bool bClose(eKind==OBJ_CIRC /*|| eKind==OBJ_SECT*/);
    XPolygon aXPoly(rRect1.Center(),rx,ry,USHORT(a),USHORT(e),bClose);
    if (eKind!=OBJ_CIRC && nStart==nEnd) {
        if (eKind==OBJ_SECT) {
            Point aMerk(aXPoly[0]);
            aXPoly=XPolygon(2);
            aXPoly[0]=rRect1.Center();
            aXPoly[1]=aMerk;
        } else {
            aXPoly=XPolygon();
        }
    }
    if (eKind==OBJ_SECT) { // Der Sektor soll Start/Ende im Zentrum haben
        // Polygon um einen Punkt rotieren (Punkte im Array verschieben)
        //unsigned nPointAnz=aXPoly.GetPointCount();
        aXPoly.Insert(0,rRect1.Center(),XPOLY_NORMAL);
        aXPoly[aXPoly.GetPointCount()]=rRect1.Center();
    }
    // Der Kreis soll Anfang und Ende im unteren Scheitelpunkt haben!
    if (bContour && eKind==OBJ_CIRC) RotateXPoly(aXPoly,rRect1.Center(),-1.0,0.0);
    // Die Winkelangaben beziehen sich immer auf die linke obere Ecke von !aRect!
    if (aGeo.nShearWink!=0) ShearXPoly(aXPoly,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotateXPoly(aXPoly,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    return aXPoly;
}

void SdrCircObj::RecalcXPoly()
{
    mpXPoly = new XPolygon(ImpCalcXPolyCirc(aRect,nStartWink,nEndWink));
}

void SdrCircObj::RecalcBoundRect()
{
    SetWinkPnt(aRect,nStartWink,aPnt1);
    SetWinkPnt(aRect,nEndWink,aPnt2);
    bBoundRectDirty=FALSE;
    aOutRect=GetSnapRect();
    long nLineWdt=ImpGetLineWdt();
    nLineWdt++; nLineWdt/=2;
    if (nLineWdt!=0) {
        long nWink=nEndWink-nStartWink;
        if (nWink<0) nWink+=36000;
        if (eKind==OBJ_SECT && nWink<18000) nLineWdt*=2; // doppelt, wegen evtl. spitzen Ecken
        if (eKind==OBJ_CCUT && nWink<18000) nLineWdt*=2; // doppelt, wegen evtl. spitzen Ecken
    }
    if (eKind==OBJ_CARC) { // ggf. Linienenden beruecksichtigen
        long nLEndWdt=ImpGetLineEndAdd();
        if (nLEndWdt>nLineWdt) nLineWdt=nLEndWdt;
    }

    if(ImpAddLineGeomteryForMiteredLines())
    {
        nLineWdt = 0;
    }

    if (nLineWdt!=0) {
        aOutRect.Left  ()-=nLineWdt;
        aOutRect.Top   ()-=nLineWdt;
        aOutRect.Right ()+=nLineWdt;
        aOutRect.Bottom()+=nLineWdt;
    }
    ImpAddShadowToBoundRect();
    ImpAddTextToBoundRect();
}

sal_Bool SdrCircObj::DoPaintObject(XOutputDevice& rXOut, const SdrPaintInfoRec& rInfoRec) const
{
    // #110094#-16 Moved to ViewContactOfSdrObj::ShouldPaintObject(..)
    //// Hidden objects on masterpages, draw nothing
    //if((rInfoRec.nPaintMode & SDRPAINTMODE_MASTERPAGE) && bNotVisibleAsMaster)
    //  return TRUE;

    bool bHideContour(IsHideContour());
    bool bIsLineDraft(0 != (rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTLINE));
    bool bIsFillDraft(0 != (rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTFILL));

    // prepare ItemSet of this object
    const SfxItemSet& rSet = GetObjectItemSet();

    // perepare ItemSet to avoid old XOut line drawing
    SfxItemSet aEmptySet(*rSet.GetPool());
    aEmptySet.Put(XLineStyleItem(XLINE_NONE));
    aEmptySet.Put(XFillStyleItem(XFILL_NONE));

    // #b4899532# if not filled but fill draft, avoid object being invisible in using
    // a hair linestyle and COL_LIGHTGRAY
    SfxItemSet aItemSet(rSet);
    if(bIsFillDraft && XLINE_NONE == ((const XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue())
    {
        ImpPrepareLocalItemSetForDraftLine(aItemSet);
    }

    // #103692# prepare ItemSet for shadow fill attributes
    SfxItemSet aShadowSet(aItemSet);

    // prepare line geometry
    ::std::auto_ptr< SdrLineGeometry > pLineGeometry( ImpPrepareLineGeometry(rXOut, aItemSet, bIsLineDraft) );

    // Shadows
    if(!bHideContour && ImpSetShadowAttributes(aItemSet, aShadowSet))
    {
        if( eKind==OBJ_CARC || bIsFillDraft )
            rXOut.SetFillAttr(aEmptySet);
        else
            rXOut.SetFillAttr(aShadowSet);

        UINT32 nXDist=((SdrShadowXDistItem&)(aItemSet.Get(SDRATTR_SHADOWXDIST))).GetValue();
        UINT32 nYDist=((SdrShadowYDistItem&)(aItemSet.Get(SDRATTR_SHADOWYDIST))).GetValue();

        // avoid shadow line drawing in XOut
        rXOut.SetLineAttr(aEmptySet);

        if (PaintNeedsXPolyCirc())
        {
            if( !mbPolygonIsLine )
            {
                XPolygon aX(GetXPoly()); // In dieser Reihenfolge, damit bXPolyIsLine gueltig ist.
                aX.Move(nXDist,nYDist);

                // #100127# Output original geometry for metafiles
                ImpGraphicFill aFill( *this, rXOut, aShadowSet, true );

                rXOut.DrawPolygon(aX.getB2DPolygon());
            }
        } else {
            // #100127# Output original geometry for metafiles
            ImpGraphicFill aFill( *this, rXOut, aShadowSet, true );

            Rectangle aR(aRect);
            aR.Move(nXDist,nYDist);
            if (eKind==OBJ_CIRC) {
                rXOut.DrawEllipse(aR);
            } else {
                GetCurrentBoundRect(); // fuer aPnt1,aPnt2
                Point aTmpPt1(aPnt1);
                Point aTmpPt2(aPnt2);
                aTmpPt1.X()+=nXDist;
                aTmpPt1.Y()+=nYDist;
                aTmpPt2.X()+=nXDist;
                aTmpPt2.Y()+=nYDist;
                switch (eKind) {
                    case OBJ_SECT: rXOut.DrawPie(aR,aTmpPt1,aTmpPt2); break;
                    case OBJ_CARC: rXOut.DrawArc(aR,aTmpPt1,aTmpPt2); break;
                    case OBJ_CCUT: DBG_ERROR("SdrCircObj::DoPaintObject(): ein Kreisabschnitt muss immer mit XPoly gepaintet werden"); break;
                    default: break;
                }
            }
        }

        // new shadow line drawing
        if( pLineGeometry.get() )
        {
            // draw the line geometry
            ImpDrawShadowLineGeometry(rXOut, aItemSet, *pLineGeometry);
        }
    }

    // Before here the LineAttr were set: if(pLineAttr) rXOut.SetLineAttr(*pLineAttr);
    rXOut.SetLineAttr(aEmptySet);

    rXOut.SetFillAttr( bIsFillDraft ? aEmptySet : aItemSet );

    if (!bHideContour) {
        if (PaintNeedsXPolyCirc())
        {
            if( !mbPolygonIsLine )
            {
                const XPolygon& rXP=GetXPoly(); // In dieser Reihenfolge, damit bXPolyIsLine gueltig ist.

                // #100127# Output original geometry for metafiles
                ImpGraphicFill aFill( *this, rXOut, bIsFillDraft ? aEmptySet : aItemSet );

                rXOut.DrawPolygon(rXP.getB2DPolygon());
            }
        } else {
            // #100127# Output original geometry for metafiles
            ImpGraphicFill aFill( *this, rXOut, bIsFillDraft ? aEmptySet : aItemSet );

            if (eKind==OBJ_CIRC) {
                rXOut.DrawEllipse(aRect);
            } else {
                GetCurrentBoundRect(); // fuer aPnt1,aPnt2
                switch (eKind) {
                    case OBJ_SECT: rXOut.DrawPie(aRect,aPnt1,aPnt2); break;
                    case OBJ_CARC: rXOut.DrawArc(aRect,aPnt1,aPnt2); break;
                    case OBJ_CCUT: DBG_ERROR("SdrCircObj::DoPaintObject(): ein Kreisabschnitt muss immer mit XPoly gepaintet werden"); break;
                    default: break;
                }
            }
        }
    }

    // Own line drawing
    if(!bHideContour && pLineGeometry.get() )
    {
        // draw the line geometry
        ImpDrawColorLineGeometry(rXOut, aItemSet, *pLineGeometry);
    }

    bool bOk=true;
    if (HasText()) {
        bOk=SdrTextObj::DoPaintObject(rXOut,rInfoRec);
    }

    return bOk;
}

SdrObject* SdrCircObj::CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const
{
    if(pVisiLayer && !pVisiLayer->IsSet(sal::static_int_cast< sal_uInt8 >(GetLayer())))
    {
        return NULL;
    }

    Point aPt(rPnt);
    Point aZero;
    aPt.X()-=aRect.Left();
    aPt.Y()-=aRect.Top();

    INT32 nMyTol=nTol;
    FASTBOOL bFilled=eKind!=OBJ_CARC && (bTextFrame || HasFill());

    INT32 nWdt=ImpGetLineWdt()/2; // Halbe Strichstaerke
    long nBoundWdt=aRect.GetWidth()-1;
    long nBoundHgt=aRect.GetHeight()-1;
    if (eKind==OBJ_SECT) {
        long nTmpWink=NormAngle360(nEndWink-nStartWink);
        if (nTmpWink<9000) {
            nBoundWdt=0;
            nBoundHgt=0;
        } else if (nTmpWink<27000) {
            nBoundWdt/=2;
            nBoundHgt/=2;
        }
    }
    if (bFilled && nBoundWdt>short(nTol) && nBoundHgt>short(nTol) && Abs(aGeo.nShearWink)<=4500) nMyTol=0; // Keine Toleranz noetig hier
    if (nWdt>nMyTol) nMyTol=nWdt; // Bei dicker Umrandung keine Toleranz noetig

    // Den uebergebenen Punkt auf den gedrehten, geshearten Kreis transformieren
    // Unrotate:
    if (aGeo.nDrehWink!=0) RotatePoint(aPt,aZero,-aGeo.nSin,aGeo.nCos); // -sin fuer Umkehrung
    // Unshear:
    if (aGeo.nShearWink!=0) ShearPoint(aPt,aZero,-aGeo.nTan); // -tan fuer Umkehrung

    long nXRad=aRect.GetWidth()/2;  if (nXRad<1) nXRad=1;
    long nYRad=aRect.GetHeight()/2; if (nYRad<1) nYRad=1;

    // Die wirklichen Radien fuer spaeter merken
    long nXRadReal=nXRad;
    long nYRadReal=nYRad;
    aPt.X()-=nXRad;
    aPt.Y()-=nYRad;
    Point aPtNoStretch(aPt);

    if (nXRad>nYRad) {
        aPt.Y()=BigMulDiv(aPt.Y(),nXRad,nYRad);
        // Da die Strichstaerke bei Ellipsen ueberall gleich ist:
        if (Abs(aPt.X())<Abs(aPt.Y())) {
            nMyTol=BigMulDiv(nMyTol,nXRad,nYRad);
        }
        nYRad=nXRad;
    }
    if (nYRad>nXRad) {
        aPt.X()=BigMulDiv(aPt.X(),nYRad,nXRad);
        // Da die Strichstaerke bei Ellipsen ueberall gleich ist:
        if (Abs(aPt.Y())<Abs(aPt.X())) {
            nMyTol=BigMulDiv(nMyTol,nYRad,nXRad);
        }
        nXRad=nYRad;
    }

    // Die BigInts haben bei *= leider ein Vorzeichenproblem (a*=a;)
    // (SV250A), deshalb hier soviele Instanzen. (JOE)
    long nAussen=nXRad+nMyTol;
    BigInt nBigTmpA(nAussen);
    BigInt nAusRadQ(nBigTmpA*nBigTmpA);
    long nInnen=nXRad-nMyTol; if (nInnen<=0) nInnen=0;
    BigInt nBigTmpI(nInnen);
    // wird sonst nicht benoetigt, ggf. BugMul sparen:
    BigInt nInnRadQ((!bFilled && nInnen!=0) ? nBigTmpI*nBigTmpI : nBigTmpI);

    // Radius von aPt berechnen
    BigInt nBigTmpX(aPt.X());
    BigInt nBigTmpY(aPt.Y());
    BigInt nPntRadQ(nBigTmpX*nBigTmpX+nBigTmpY*nBigTmpY);

    sal_Bool bRet(sal_False);
    if (nPntRadQ<=nAusRadQ) { // sonst ausserhalb
        if (nInnen==0) bRet = sal_True;
        else if (eKind==OBJ_CIRC) { // Vollkreis
            if (bFilled) bRet = sal_True;
            else if (nPntRadQ>=nInnRadQ) bRet = sal_True;
        } else { // Teilkreise
            long nWink=NormAngle360(GetAngle(aPt));
            long a=nStartWink;
            long e=nEndWink;
            if (e<a) e+=36000;
            if (nWink<a) nWink+=36000;
            if (nWink>=a && nWink<=e) {
                if (bFilled) bRet = sal_True;
                else if (nPntRadQ>=nInnRadQ) bRet = sal_True;
            }
            if (!bRet) {
                Rectangle aR(aPtNoStretch.X()-nMyTol,aPtNoStretch.Y()-nMyTol,
                             aPtNoStretch.X()+nMyTol,aPtNoStretch.Y()+nMyTol);
                Point aP1(aPnt1);
                aP1.X()-=aRect.Left()+nXRadReal;
                aP1.Y()-=aRect.Top()+nYRadReal;
                Point aP2(aPnt2);
                aP2.X()-=aRect.Left()+nXRadReal;
                aP2.Y()-=aRect.Top()+nYRadReal;
                if (eKind==OBJ_SECT) { // Kreissektor: nur noch die beiden Strecken testen
                    bRet=IsRectTouchesLine(aZero,aP1,aR) || IsRectTouchesLine(aZero,aP2,aR);
                }
                if (eKind==OBJ_CCUT) { // Kreisabschnitt noch die Sehne und die MaeuseEcke (Dreieck) testen
                    if (IsRectTouchesLine(aP1,aP2,aR)) bRet = sal_True; // die Sehne
                    else if (bFilled) { // und nun die Maeusescke
                        const Polygon aPoly(basegfx::tools::adaptiveSubdivideByAngle(GetXPoly().getB2DPolygon()));
                        bRet=IsPointInsidePoly(aPoly,rPnt);
                    }
                }
            }
        }
    }
    if (!bRet && HasText()) bRet=SdrTextObj::CheckHit(rPnt,nTol,pVisiLayer)!=NULL;
    return bRet ? (SdrObject*)this : NULL;
}

void SdrCircObj::TakeObjNameSingul(XubString& rName) const
{
    USHORT nID=STR_ObjNameSingulCIRC;
    if (aRect.GetWidth()==aRect.GetHeight() && aGeo.nShearWink==0) {
        switch (eKind) {
            case OBJ_CIRC: nID=STR_ObjNameSingulCIRC; break;
            case OBJ_SECT: nID=STR_ObjNameSingulSECT; break;
            case OBJ_CARC: nID=STR_ObjNameSingulCARC; break;
            case OBJ_CCUT: nID=STR_ObjNameSingulCCUT; break;
            default: break;
        }
    } else {
        switch (eKind) {
            case OBJ_CIRC: nID=STR_ObjNameSingulCIRCE; break;
            case OBJ_SECT: nID=STR_ObjNameSingulSECTE; break;
            case OBJ_CARC: nID=STR_ObjNameSingulCARCE; break;
            case OBJ_CCUT: nID=STR_ObjNameSingulCCUTE; break;
            default: break;
        }
    }
    rName=ImpGetResStr(nID);

    String aName( GetName() );
    if(aName.Len())
    {
        rName += sal_Unicode(' ');
        rName += sal_Unicode('\'');
        rName += aName;
        rName += sal_Unicode('\'');
    }
}

void SdrCircObj::TakeObjNamePlural(XubString& rName) const
{
    USHORT nID=STR_ObjNamePluralCIRC;
    if (aRect.GetWidth()==aRect.GetHeight() && aGeo.nShearWink==0) {
        switch (eKind) {
            case OBJ_CIRC: nID=STR_ObjNamePluralCIRC; break;
            case OBJ_SECT: nID=STR_ObjNamePluralSECT; break;
            case OBJ_CARC: nID=STR_ObjNamePluralCARC; break;
            case OBJ_CCUT: nID=STR_ObjNamePluralCCUT; break;
            default: break;
        }
    } else {
        switch (eKind) {
            case OBJ_CIRC: nID=STR_ObjNamePluralCIRCE; break;
            case OBJ_SECT: nID=STR_ObjNamePluralSECTE; break;
            case OBJ_CARC: nID=STR_ObjNamePluralCARCE; break;
            case OBJ_CCUT: nID=STR_ObjNamePluralCCUTE; break;
            default: break;
        }
    }
    rName=ImpGetResStr(nID);
}

void SdrCircObj::operator=(const SdrObject& rObj)
{
    SdrRectObj::operator=(rObj);

    nStartWink = ((SdrCircObj&)rObj).nStartWink;
    nEndWink = ((SdrCircObj&)rObj).nEndWink;
    aPnt1 = ((SdrCircObj&)rObj).aPnt1;
    aPnt2 = ((SdrCircObj&)rObj).aPnt2;
}

basegfx::B2DPolyPolygon SdrCircObj::TakeXorPoly(sal_Bool /*bDetail*/) const
{
    basegfx::B2DPolyPolygon aRetval;

    XPolygon aP(ImpCalcXPolyCirc(aRect,nStartWink,nEndWink));
    if (!mbPolygonIsLine) { // Polygon schliessen
        USHORT n=aP.GetPointCount();
        Point aPnt(aP[0]);
        aP[n]=aPnt;
    }

    aRetval.append(aP.getB2DPolygon());

    return aRetval;
}

struct ImpCircUser : public SdrDragStatUserData
{
    Rectangle                   aR;
    Point                       aCenter;
    Point                       aRadius;
    Point                       aP1;
    Point                       aP2;
    long                        nMaxRad;
    long                        nHgt;
    long                        nWdt;
    long                        nStart;
    long                        nEnd;
    long                        nWink;
    FASTBOOL                    bRight; // noch nicht implementiert

public:
    ImpCircUser()
    :   nMaxRad(0),
        nHgt(0),
        nWdt(0),
        nStart(0),
        nEnd(0),
        bRight(FALSE)
    {}
    void SetCreateParams(SdrDragStat& rStat);
};

sal_uInt32 SdrCircObj::GetHdlCount() const
{
    if(OBJ_CIRC != eKind)
    {
        return 10L;
    }
    else
    {
        return 8L;
    }
}

SdrHdl* SdrCircObj::GetHdl(sal_uInt32 nHdlNum) const
{
    if (eKind==OBJ_CIRC)
        nHdlNum += 2L; // Keine Winkelhandles fuer den Vollkreis
    SdrHdl* pH=NULL;
    Point aPnt;
    SdrHdlKind eLocalKind=HDL_MOVE;
    sal_uInt32 nPNum(0L);
    switch (nHdlNum) {
        case 0: aPnt=aPnt1; eLocalKind=HDL_CIRC; nPNum=1L; break; // StartWink
        case 1: aPnt=aPnt2; eLocalKind=HDL_CIRC; nPNum=2L; break; // EndWink
        case 2: aPnt=aRect.TopLeft();      eLocalKind=HDL_UPLFT; break; // Oben links
        case 3: aPnt=aRect.TopCenter();    eLocalKind=HDL_UPPER; break; // Oben
        case 4: aPnt=aRect.TopRight();     eLocalKind=HDL_UPRGT; break; // Oben rechts
        case 5: aPnt=aRect.LeftCenter();   eLocalKind=HDL_LEFT ; break; // Links
        case 6: aPnt=aRect.RightCenter();  eLocalKind=HDL_RIGHT; break; // Rechts
        case 7: aPnt=aRect.BottomLeft();   eLocalKind=HDL_LWLFT; break; // Unten links
        case 8: aPnt=aRect.BottomCenter(); eLocalKind=HDL_LOWER; break; // Unten
        case 9: aPnt=aRect.BottomRight();  eLocalKind=HDL_LWRGT; break; // Unten rechts
    }
    if (aGeo.nShearWink!=0) ShearPoint(aPnt,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotatePoint(aPnt,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    if (eLocalKind!=HDL_MOVE) {
        pH=new SdrHdl(aPnt,eLocalKind);
        pH->SetPointNum(nPNum);
        pH->SetObj((SdrObject*)this);
        pH->SetDrehWink(aGeo.nDrehWink);
    }
    return pH;
}

FASTBOOL SdrCircObj::HasSpecialDrag() const
{
    return TRUE;
}

FASTBOOL SdrCircObj::BegDrag(SdrDragStat& rDrag) const
{
    FASTBOOL bWink=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    FASTBOOL bOk=bWink;
    if (bWink) {
        ImpCircUser* pNewUser = new ImpCircUser;
        pNewUser->nWink = 0;
        rDrag.SetUser(pNewUser);
        if (rDrag.GetHdl()->GetPointNum()==1 || rDrag.GetHdl()->GetPointNum()==2) {
            rDrag.SetNoSnap(TRUE);
        }
    } else {
        bOk=SdrTextObj::BegDrag(rDrag);
    }
    return bOk;
}

FASTBOOL SdrCircObj::MovDrag(SdrDragStat& rDrag) const
{
    FASTBOOL bWink=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    if (bWink) {
        Point aPt(rDrag.GetNow());
        // Unrotate:
        if (aGeo.nDrehWink!=0) RotatePoint(aPt,aRect.TopLeft(),-aGeo.nSin,aGeo.nCos); // -sin fuer Umkehrung
        // Unshear:
        if (aGeo.nShearWink!=0) ShearPoint(aPt,aRect.TopLeft(),-aGeo.nTan); // -tan fuer Umkehrung
        aPt-=aRect.Center();
        long nWdt=aRect.Right()-aRect.Left();
        long nHgt=aRect.Bottom()-aRect.Top();
        if (nWdt>=nHgt) {
            aPt.Y()=BigMulDiv(aPt.Y(),nWdt,nHgt);
        } else {
            aPt.X()=BigMulDiv(aPt.X(),nHgt,nWdt);
        }
        long nWink=NormAngle360(GetAngle(aPt));
        if (rDrag.GetView()!=NULL && rDrag.GetView()->IsAngleSnapEnabled()) {
            long nSA=rDrag.GetView()->GetSnapAngle();
            if (nSA!=0) { // Winkelfang
                nWink+=nSA/2;
                nWink/=nSA;
                nWink*=nSA;
                nWink=NormAngle360(nWink);
            }
        }

        ImpCircUser* pUserData = (ImpCircUser*)rDrag.GetUser();

        if(pUserData && pUserData->nWink != nWink)
        {
            pUserData->nWink = nWink;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    } else {
        return SdrTextObj::MovDrag(rDrag);
    }
}

FASTBOOL SdrCircObj::EndDrag(SdrDragStat& rDrag)
{
    FASTBOOL bWink=rDrag.GetHdl()!=NULL && rDrag.GetHdl()->GetKind()==HDL_CIRC;
    if (bWink) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        long nWink=*((long*)(rDrag.GetUser()));
        if (rDrag.GetHdl()->GetPointNum()==1) nStartWink=nWink;
        if (rDrag.GetHdl()->GetPointNum()==2) nEndWink=nWink;
        SetRectsDirty();
        SetXPolyDirty();
        ImpSetCircInfoToAttr();
        SetChanged();
        BroadcastObjectChange();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
        return TRUE;
    } else {
        return SdrTextObj::EndDrag(rDrag);
    }
}

void SdrCircObj::BrkDrag(SdrDragStat& rDrag) const
{
    SdrTextObj::BrkDrag(rDrag);
}

XubString SdrCircObj::GetDragComment(const SdrDragStat& rDrag, FASTBOOL bUndoDragComment, FASTBOOL bCreateComment) const
{
    if(bCreateComment)
    {
        XubString aStr;
        ImpTakeDescriptionStr(STR_ViewCreateObj, aStr);
        UINT32 nPntAnz(rDrag.GetPointAnz());

        if(eKind != OBJ_CIRC && nPntAnz > 2)
        {
            ImpCircUser* pU = (ImpCircUser*)rDrag.GetUser();
            INT32 nWink;

            aStr.AppendAscii(" (");

            if(nPntAnz == 3)
                nWink = pU->nStart;
            else
                nWink = pU->nEnd;

            aStr += GetWinkStr(nWink,FALSE);
            aStr += sal_Unicode(')');
        }
        return aStr;
    }

    BOOL bWink(rDrag.GetHdl() && rDrag.GetHdl()->GetKind() == HDL_CIRC);

    if(bWink)
    {
        XubString aStr;
        ImpCircUser* pUserData = (ImpCircUser*)rDrag.GetUser();
        const sal_Int32 nWink(pUserData ? pUserData->nWink : 0L);

        ImpTakeDescriptionStr(STR_DragCircAngle, aStr);
        aStr.AppendAscii(" (");
        aStr += GetWinkStr(nWink,FALSE);
        aStr += sal_Unicode(')');

        return aStr;
    }
    else
    {
        return SdrTextObj::GetDragComment(rDrag, bUndoDragComment, FALSE);
    }
}

basegfx::B2DPolyPolygon SdrCircObj::TakeDragPoly(const SdrDragStat& rDrag) const
{
    const bool bWink(rDrag.GetHdl() && HDL_CIRC == rDrag.GetHdl()->GetKind());
    long a(nStartWink);
    long e(nEndWink);

    if(bWink)
    {
        long nWink=*((long*)(rDrag.GetUser()));
        if (rDrag.GetHdl()->GetPointNum()==1) a=nWink;
        else e=nWink;
    }

    Rectangle aTmpRect(bWink ? aRect : ImpDragCalcRect(rDrag));
    XPolygon aXP(ImpCalcXPolyCirc(aTmpRect, a, e));

    if(!mbPolygonIsLine)
    {
        // Polygon schliessen
        USHORT n=aXP.GetPointCount();
        Point aPnt(aXP[0]);
        aXP[n]=aPnt;
    }

    XPolyPolygon aXPP;
    aXPP.Insert(aXP);

    return aXPP.getB2DPolyPolygon();
}

void ImpCircUser::SetCreateParams(SdrDragStat& rStat)
{
    rStat.TakeCreateRect(aR);
    aR.Justify();
    aCenter=aR.Center();
    nWdt=aR.Right()-aR.Left();
    nHgt=aR.Bottom()-aR.Top();
    nMaxRad=((nWdt>nHgt ? nWdt : nHgt)+1) /2;
    nStart=0;
    nEnd=36000;
    if (rStat.GetPointAnz()>2) {
        Point aP(rStat.GetPoint(2)-aCenter);
        if (nWdt==0) aP.X()=0;
        if (nHgt==0) aP.Y()=0;
        if (nWdt>=nHgt) {
            if (nHgt!=0) aP.Y()=aP.Y()*nWdt/nHgt;
        } else {
            if (nWdt!=0) aP.X()=aP.X()*nHgt/nWdt;
        }
        nStart=NormAngle360(GetAngle(aP));
        if (rStat.GetView()!=NULL && rStat.GetView()->IsAngleSnapEnabled()) {
            long nSA=rStat.GetView()->GetSnapAngle();
            if (nSA!=0) { // Winkelfang
                nStart+=nSA/2;
                nStart/=nSA;
                nStart*=nSA;
                nStart=NormAngle360(nStart);
            }
        }
        SetWinkPnt(aR,nStart,aP1);
        nEnd=nStart;
        aP2=aP1;
    } else aP1=aCenter;
    if (rStat.GetPointAnz()>3) {
        Point aP(rStat.GetPoint(3)-aCenter);
        if (nWdt>=nHgt) {
            aP.Y()=BigMulDiv(aP.Y(),nWdt,nHgt);
        } else {
            aP.X()=BigMulDiv(aP.X(),nHgt,nWdt);
        }
        nEnd=NormAngle360(GetAngle(aP));
        if (rStat.GetView()!=NULL && rStat.GetView()->IsAngleSnapEnabled()) {
            long nSA=rStat.GetView()->GetSnapAngle();
            if (nSA!=0) { // Winkelfang
                nEnd+=nSA/2;
                nEnd/=nSA;
                nEnd*=nSA;
                nEnd=NormAngle360(nEnd);
            }
        }
        SetWinkPnt(aR,nEnd,aP2);
    } else aP2=aCenter;
}

void SdrCircObj::ImpSetCreateParams(SdrDragStat& rStat) const
{
    ImpCircUser* pU=(ImpCircUser*)rStat.GetUser();
    if (pU==NULL) {
        pU=new ImpCircUser;
        rStat.SetUser(pU);
    }
    pU->SetCreateParams(rStat);
}

FASTBOOL SdrCircObj::BegCreate(SdrDragStat& rStat)
{
    rStat.SetOrtho4Possible();
    Rectangle aRect1(rStat.GetStart(), rStat.GetNow());
    aRect1.Justify();
    rStat.SetActionRect(aRect1);
    aRect = aRect1;
    ImpSetCreateParams(rStat);
    return TRUE;
}

FASTBOOL SdrCircObj::MovCreate(SdrDragStat& rStat)
{
    ImpSetCreateParams(rStat);
    ImpCircUser* pU=(ImpCircUser*)rStat.GetUser();
    rStat.SetActionRect(pU->aR);
    aRect=pU->aR; // fuer ObjName
    ImpJustifyRect(aRect);
    nStartWink=pU->nStart;
    nEndWink=pU->nEnd;
    bBoundRectDirty=TRUE;
    bSnapRectDirty=TRUE;
    SetXPolyDirty();
    return TRUE;
}

FASTBOOL SdrCircObj::EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd)
{
    ImpSetCreateParams(rStat);
    ImpCircUser* pU=(ImpCircUser*)rStat.GetUser();
    FASTBOOL bRet=FALSE;
    if (eCmd==SDRCREATE_FORCEEND && rStat.GetPointAnz()<4) eKind=OBJ_CIRC;
    if (eKind==OBJ_CIRC) {
        bRet=rStat.GetPointAnz()>=2;
        if (bRet) {
            aRect=pU->aR;
            ImpJustifyRect(aRect);
        }
    } else {
        rStat.SetNoSnap(rStat.GetPointAnz()>=2);
        rStat.SetOrtho4Possible(rStat.GetPointAnz()<2);
        bRet=rStat.GetPointAnz()>=4;
        if (bRet) {
            aRect=pU->aR;
            ImpJustifyRect(aRect);
            nStartWink=pU->nStart;
            nEndWink=pU->nEnd;
        }
    }
    bClosedObj=eKind!=OBJ_CARC;
    SetRectsDirty();
    SetXPolyDirty();
    ImpSetCircInfoToAttr();
    if (bRet) {
        delete pU;
        rStat.SetUser(NULL);
    }
    return bRet;
}

void SdrCircObj::BrkCreate(SdrDragStat& rStat)
{
    ImpCircUser* pU=(ImpCircUser*)rStat.GetUser();
    delete pU;
    rStat.SetUser(NULL);
}

FASTBOOL SdrCircObj::BckCreate(SdrDragStat& rStat)
{
    rStat.SetNoSnap(rStat.GetPointAnz()>=3);
    rStat.SetOrtho4Possible(rStat.GetPointAnz()<3);
    return eKind!=OBJ_CIRC;
}

basegfx::B2DPolyPolygon SdrCircObj::TakeCreatePoly(const SdrDragStat& rDrag) const
{
    basegfx::B2DPolyPolygon aRetval;
    ImpCircUser* pU = (ImpCircUser*)rDrag.GetUser();

    if(OBJ_CIRC == eKind || rDrag.GetPointAnz() < 4L)
    {
        aRetval.append(XPolygon(pU->aCenter,pU->aR.GetWidth()/2,pU->aR.GetHeight()/2).getB2DPolygon());

        if(3L == rDrag.GetPointAnz())
        {
            basegfx::B2DPolygon aNew;
            aNew.append(basegfx::B2DPoint(pU->aCenter.X(), pU->aCenter.Y()));
            aNew.append(basegfx::B2DPoint(pU->aP1.X(), pU->aP1.Y()));
            aRetval.append(aNew);
        }
    }
    else
    {
        basegfx::B2DPolygon aNew(XPolygon(ImpCalcXPolyCirc(pU->aR,pU->nStart,pU->nEnd)).getB2DPolygon());

        if(!mbPolygonIsLine)
        {
            // Polygon schliessen
            aNew.setClosed(true);
        }

        aRetval.append(aNew);
    }

    return aRetval;
}

Pointer SdrCircObj::GetCreatePointer() const
{
    switch (eKind) {
        case OBJ_CIRC: return Pointer(POINTER_DRAW_ELLIPSE);
        case OBJ_SECT: return Pointer(POINTER_DRAW_PIE);
        case OBJ_CARC: return Pointer(POINTER_DRAW_ARC);
        case OBJ_CCUT: return Pointer(POINTER_DRAW_CIRCLECUT);
        default: break;
    } // switch
    return Pointer(POINTER_CROSS);
}

void SdrCircObj::NbcMove(const Size& aSiz)
{
    MoveRect(aRect,aSiz);
    MoveRect(aOutRect,aSiz);
    MoveRect(maSnapRect,aSiz);
    MovePoint(aPnt1,aSiz);
    MovePoint(aPnt2,aSiz);
    SetXPolyDirty();
    SetRectsDirty(sal_True);
}

void SdrCircObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    long nWink0=aGeo.nDrehWink;
    FASTBOOL bNoShearRota=(aGeo.nDrehWink==0 && aGeo.nShearWink==0);
    SdrTextObj::NbcResize(rRef,xFact,yFact);
    bNoShearRota|=(aGeo.nDrehWink==0 && aGeo.nShearWink==0);
    if (eKind!=OBJ_CIRC) {
        FASTBOOL bXMirr=(xFact.GetNumerator()<0) != (xFact.GetDenominator()<0);
        FASTBOOL bYMirr=(yFact.GetNumerator()<0) != (yFact.GetDenominator()<0);
        if (bXMirr || bYMirr) {
            // bei bXMirr!=bYMirr muessten eigentlich noch die beiden
            // Linienende vertauscht werden. Das ist jedoch mal wieder
            // schlecht (wg. zwangslaeufiger harter Formatierung).
            // Alternativ koennte ein bMirrored-Flag eingefuehrt werden
            // (Vielleicht ja mal grundsaetzlich, auch fuer gepiegelten Text, ...).
            long nS0=nStartWink;
            long nE0=nEndWink;
            if (bNoShearRota) {
                // Das RectObj spiegelt bei VMirror bereits durch durch 180deg Drehung.
                if (! (bXMirr && bYMirr)) {
                    long nTmp=nS0;
                    nS0=18000-nE0;
                    nE0=18000-nTmp;
                }
            } else { // Spiegeln fuer verzerrte Ellipsen
                if (bXMirr!=bYMirr) {
                    nS0+=nWink0;
                    nE0+=nWink0;
                    if (bXMirr) {
                        long nTmp=nS0;
                        nS0=18000-nE0;
                        nE0=18000-nTmp;
                    }
                    if (bYMirr) {
                        long nTmp=nS0;
                        nS0=-nE0;
                        nE0=-nTmp;
                    }
                    nS0-=aGeo.nDrehWink;
                    nE0-=aGeo.nDrehWink;
                }
            }
            long nWinkDif=nE0-nS0;
            nStartWink=NormAngle360(nS0);
            nEndWink  =NormAngle360(nE0);
            if (nWinkDif==36000) nEndWink+=nWinkDif; // Vollkreis
        }
    }
    SetXPolyDirty();
    ImpSetCircInfoToAttr();
}

void SdrCircObj::NbcShear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    SdrTextObj::NbcShear(rRef,nWink,tn,bVShear);
    SetXPolyDirty();
    ImpSetCircInfoToAttr();
}

void SdrCircObj::NbcMirror(const Point& rRef1, const Point& rRef2)
{
    //long nWink0=aGeo.nDrehWink;
    FASTBOOL bFreeMirr=eKind!=OBJ_CIRC;
    Point aTmpPt1;
    Point aTmpPt2;
    if (bFreeMirr) { // bei freier Spiegelachse einige Vorbereitungen Treffen
        Point aCenter(aRect.Center());
        long nWdt=aRect.GetWidth()-1;
        long nHgt=aRect.GetHeight()-1;
        long nMaxRad=((nWdt>nHgt ? nWdt : nHgt)+1) /2;
        double a;
        // Startpunkt
        a=nStartWink*nPi180;
        aTmpPt1=Point(Round(cos(a)*nMaxRad),-Round(sin(a)*nMaxRad));
        if (nWdt==0) aTmpPt1.X()=0;
        if (nHgt==0) aTmpPt1.Y()=0;
        aTmpPt1+=aCenter;
        // Endpunkt
        a=nEndWink*nPi180;
        aTmpPt2=Point(Round(cos(a)*nMaxRad),-Round(sin(a)*nMaxRad));
        if (nWdt==0) aTmpPt2.X()=0;
        if (nHgt==0) aTmpPt2.Y()=0;
        aTmpPt2+=aCenter;
        if (aGeo.nDrehWink!=0) {
            RotatePoint(aTmpPt1,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
            RotatePoint(aTmpPt2,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
        }
        if (aGeo.nShearWink!=0) {
            ShearPoint(aTmpPt1,aRect.TopLeft(),aGeo.nTan);
            ShearPoint(aTmpPt2,aRect.TopLeft(),aGeo.nTan);
        }
    }
    SdrTextObj::NbcMirror(rRef1,rRef2);
    if (eKind!=OBJ_CIRC) { // Anpassung von Start- und Endwinkel
        MirrorPoint(aTmpPt1,rRef1,rRef2);
        MirrorPoint(aTmpPt2,rRef1,rRef2);
        // Unrotate:
        if (aGeo.nDrehWink!=0) {
            RotatePoint(aTmpPt1,aRect.TopLeft(),-aGeo.nSin,aGeo.nCos); // -sin fuer Umkehrung
            RotatePoint(aTmpPt2,aRect.TopLeft(),-aGeo.nSin,aGeo.nCos); // -sin fuer Umkehrung
        }
        // Unshear:
        if (aGeo.nShearWink!=0) {
            ShearPoint(aTmpPt1,aRect.TopLeft(),-aGeo.nTan); // -tan fuer Umkehrung
            ShearPoint(aTmpPt2,aRect.TopLeft(),-aGeo.nTan); // -tan fuer Umkehrung
        }
        Point aCenter(aRect.Center());
        aTmpPt1-=aCenter;
        aTmpPt2-=aCenter;
        // Weil gespiegelt sind die Winkel nun auch noch vertauscht
        nStartWink=GetAngle(aTmpPt2);
        nEndWink  =GetAngle(aTmpPt1);
        long nWinkDif=nEndWink-nStartWink;
        nStartWink=NormAngle360(nStartWink);
        nEndWink  =NormAngle360(nEndWink);
        if (nWinkDif==36000) nEndWink+=nWinkDif; // Vollkreis
    }
    SetXPolyDirty();
    ImpSetCircInfoToAttr();
}

SdrObjGeoData* SdrCircObj::NewGeoData() const
{
    return new SdrCircObjGeoData;
}

void SdrCircObj::SaveGeoData(SdrObjGeoData& rGeo) const
{
    SdrRectObj::SaveGeoData(rGeo);
    SdrCircObjGeoData& rCGeo=(SdrCircObjGeoData&)rGeo;
    rCGeo.nStartWink=nStartWink;
    rCGeo.nEndWink  =nEndWink;
}

void SdrCircObj::RestGeoData(const SdrObjGeoData& rGeo)
{
    SdrRectObj::RestGeoData(rGeo);
    SdrCircObjGeoData& rCGeo=(SdrCircObjGeoData&)rGeo;
    nStartWink=rCGeo.nStartWink;
    nEndWink  =rCGeo.nEndWink;
    SetXPolyDirty();
    ImpSetCircInfoToAttr();
}

void Union(Rectangle& rR, const Point& rP)
{
    if (rP.X()<rR.Left  ()) rR.Left  ()=rP.X();
    if (rP.X()>rR.Right ()) rR.Right ()=rP.X();
    if (rP.Y()<rR.Top   ()) rR.Top   ()=rP.Y();
    if (rP.Y()>rR.Bottom()) rR.Bottom()=rP.Y();
}

void SdrCircObj::TakeUnrotatedSnapRect(Rectangle& rRect) const
{
    rRect=aRect;
    if (eKind!=OBJ_CIRC) {
        SetWinkPnt(rRect,nStartWink,((SdrCircObj*)(this))->aPnt1);
        SetWinkPnt(rRect,nEndWink  ,((SdrCircObj*)(this))->aPnt2);
        long a=nStartWink;
        long e=nEndWink;
        rRect.Left  ()=aRect.Right();
        rRect.Right ()=aRect.Left();
        rRect.Top   ()=aRect.Bottom();
        rRect.Bottom()=aRect.Top();
        Union(rRect,aPnt1);
        Union(rRect,aPnt2);
        if ((a<=18000 && e>=18000) || (a>e && (a<=18000 || e>=18000))) {
            Union(rRect,aRect.LeftCenter());
        }
        if ((a<=27000 && e>=27000) || (a>e && (a<=27000 || e>=27000))) {
            Union(rRect,aRect.BottomCenter());
        }
        if (a>e) {
            Union(rRect,aRect.RightCenter());
        }
        if ((a<=9000 && e>=9000) || (a>e && (a<=9000 || e>=9000))) {
            Union(rRect,aRect.TopCenter());
        }
        if (eKind==OBJ_SECT) {
            Union(rRect,aRect.Center());
        }
        if (aGeo.nDrehWink!=0) {
            Point aDst(rRect.TopLeft());
            aDst-=aRect.TopLeft();
            Point aDst0(aDst);
            RotatePoint(aDst,Point(),aGeo.nSin,aGeo.nCos);
            aDst-=aDst0;
            rRect.Move(aDst.X(),aDst.Y());
        }
    }
    if (aGeo.nShearWink!=0) {
        long nDst=Round((rRect.Bottom()-rRect.Top())*aGeo.nTan);
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

void SdrCircObj::RecalcSnapRect()
{
    if (PaintNeedsXPolyCirc()) {
        maSnapRect=GetXPoly().GetBoundRect();
    } else {
        TakeUnrotatedSnapRect(maSnapRect);
    }
}

void SdrCircObj::NbcSetSnapRect(const Rectangle& rRect)
{
    if (aGeo.nDrehWink!=0 || aGeo.nShearWink!=0 || eKind!=OBJ_CIRC) {
        Rectangle aSR0(GetSnapRect());
        long nWdt0=aSR0.Right()-aSR0.Left();
        long nHgt0=aSR0.Bottom()-aSR0.Top();
        long nWdt1=rRect.Right()-rRect.Left();
        long nHgt1=rRect.Bottom()-rRect.Top();
        NbcResize(maSnapRect.TopLeft(),Fraction(nWdt1,nWdt0),Fraction(nHgt1,nHgt0));
        NbcMove(Size(rRect.Left()-aSR0.Left(),rRect.Top()-aSR0.Top()));
    } else {
        aRect=rRect;
        ImpJustifyRect(aRect);
    }
    SetRectsDirty();
    SetXPolyDirty();
    ImpSetCircInfoToAttr();
}

sal_uInt32 SdrCircObj::GetSnapPointCount() const
{
    if (eKind==OBJ_CIRC) {
        return 1L;
    } else {
        return 3L;
    }
}

Point SdrCircObj::GetSnapPoint(sal_uInt32 i) const
{
    switch (i) {
        case 1 : return aPnt1;
        case 2 : return aPnt2;
        default: return aRect.Center();
    }
}

void __EXPORT SdrCircObj::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
{
    SetXPolyDirty();
    SdrRectObj::SFX_NOTIFY(rBC,rBCType,rHint,rHintType);
    ImpSetAttrToCircInfo();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrCircObj::ImpSetAttrToCircInfo()
{
    const SfxItemSet& rSet = GetObjectItemSet();
    SdrCircKind eNewKindA = ((SdrCircKindItem&)rSet.Get(SDRATTR_CIRCKIND)).GetValue();
    SdrObjKind eNewKind = eKind;

    if(eNewKindA == SDRCIRC_FULL)
        eNewKind = OBJ_CIRC;
    else if(eNewKindA == SDRCIRC_SECT)
        eNewKind = OBJ_SECT;
    else if(eNewKindA == SDRCIRC_ARC)
        eNewKind = OBJ_CARC;
    else if(eNewKindA == SDRCIRC_CUT)
        eNewKind = OBJ_CCUT;

    sal_Int32 nNewStart = ((SdrCircStartAngleItem&)rSet.Get(SDRATTR_CIRCSTARTANGLE)).GetValue();
    sal_Int32 nNewEnd = ((SdrCircEndAngleItem&)rSet.Get(SDRATTR_CIRCENDANGLE)).GetValue();

    BOOL bKindChg = eKind != eNewKind;
    BOOL bWinkChg = nNewStart != nStartWink || nNewEnd != nEndWink;

    if(bKindChg || bWinkChg)
    {
        eKind = eNewKind;
        nStartWink = nNewStart;
        nEndWink = nNewEnd;

        if(bKindChg || (eKind != OBJ_CIRC && bWinkChg))
        {
            SetXPolyDirty();
            SetRectsDirty();
        }
    }
}

void SdrCircObj::ImpSetCircInfoToAttr()
{
    SdrCircKind eNewKindA = SDRCIRC_FULL;
    const SfxItemSet& rSet = GetObjectItemSet();

    if(eKind == OBJ_SECT)
        eNewKindA = SDRCIRC_SECT;
    else if(eKind == OBJ_CARC)
        eNewKindA = SDRCIRC_ARC;
    else if(eKind == OBJ_CCUT)
        eNewKindA = SDRCIRC_CUT;

    SdrCircKind eOldKindA = ((SdrCircKindItem&)rSet.Get(SDRATTR_CIRCKIND)).GetValue();
    sal_Int32 nOldStartWink = ((SdrCircStartAngleItem&)rSet.Get(SDRATTR_CIRCSTARTANGLE)).GetValue();
    sal_Int32 nOldEndWink = ((SdrCircEndAngleItem&)rSet.Get(SDRATTR_CIRCENDANGLE)).GetValue();

    if(eNewKindA != eOldKindA || nStartWink != nOldStartWink || nEndWink != nOldEndWink)
    {
        // #81921# since SetItem() implicitly calls ImpSetAttrToCircInfo()
        // setting the item directly is necessary here.
        if(eNewKindA != eOldKindA)
        {
            GetProperties().SetObjectItemDirect(SdrCircKindItem(eNewKindA));
        }

        if(nStartWink != nOldStartWink)
        {
            GetProperties().SetObjectItemDirect(SdrCircStartAngleItem(nStartWink));
        }

        if(nEndWink != nOldEndWink)
        {
            GetProperties().SetObjectItemDirect(SdrCircEndAngleItem(nEndWink));
        }

        SetXPolyDirty();
        ImpSetAttrToCircInfo();
    }
}

SdrObject* SdrCircObj::DoConvertToPolyObj(BOOL bBezier) const
{
    const sal_Bool bFill(OBJ_CARC == eKind ? sal_False : sal_True);
    basegfx::B2DPolyPolygon aPolyPolygon;
    aPolyPolygon.append(ImpCalcXPolyCirc(aRect, nStartWink, nEndWink).getB2DPolygon());
    SdrObject* pRet = ImpConvertMakeObj(aPolyPolygon, bFill, bBezier);
    pRet = ImpConvertAddText(pRet, bBezier);

    return pRet;
}

//////////////////////////////////////////////////////////////////////////////
// eof
