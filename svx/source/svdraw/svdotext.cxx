/*************************************************************************
 *
 *  $RCSfile: svdotext.cxx,v $
 *
 *  $Revision: 1.51 $
 *
 *  last change: $Author: thb $ $Date: 2002-07-31 09:34:36 $
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

#include "svdotext.hxx"
#include "svditext.hxx"
#include "svdpagv.hxx"  // fuer Abfrage im Paint, ob das
#include "svdview.hxx"  // Objekt gerade editiert wird
#include "svdpage.hxx"  // und fuer AnimationHandler (Laufschrift)
#include "svdxout.hxx"
#include "svdtouch.hxx"
#include "svdio.hxx"
#include "svdetc.hxx"
#include "svdoutl.hxx"
#include "svdscrol.hxx"  // fuer Laufschrift
#include "svdmodel.hxx"  // OutlinerDefaults
#include "svdglob.hxx"  // Stringcache
#include "svdstr.hrc"   // Objektname
#include "svdtxhdl.hxx"  // DrawTextToPath
#include "writingmodeitem.hxx"

#ifndef _SVX_COLORCFG_HXX
#include "colorcfg.hxx"
#endif

#ifndef _EEITEM_HXX //autogen
#include <eeitem.hxx>
#endif

#ifndef _EDITSTAT_HXX //autogen
#include <editstat.hxx>
#endif

#ifndef _OUTLOBJ_HXX //autogen
#include <outlobj.hxx>
#endif

#ifndef _EDITOBJ_HXX //autogen
#include <editobj.hxx>
#endif

#ifndef _OUTLINER_HXX //autogen
#include "outliner.hxx"
#endif

#ifndef _SVX_FHGTITEM_HXX //autogen
#include "fhgtitem.hxx"
#endif

#ifndef _SFXITEMPOOL_HXX //autogen
#include <svtools/itempool.hxx>
#endif

#ifndef _SVX_ADJITEM_HXX //autogen
#include "adjitem.hxx"
#endif
#include "flditem.hxx"

#ifndef _SVX_XFTOUIT_HXX
#include "xftouit.hxx"
#endif

#ifndef _SV_SALBTYPE_HXX
#include <vcl/salbtype.hxx>     // FRound
#endif

#ifndef _SVX_XFLGRIT_HXX
#include "xflgrit.hxx"
#endif

/***********************************************************************
* Macros fuer Umrechnung Twips<->100tel mm                             *
***********************************************************************/
#define TWIPS_TO_MM(val) ((val * 127 + 36) / 72)
#define MM_TO_TWIPS(val) ((val * 72 + 63) / 127)

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@@ @@@@@ @@   @@ @@@@@@  @@@@  @@@@@  @@@@@@
//    @@   @@    @@@ @@@   @@   @@  @@ @@  @@     @@
//    @@   @@     @@@@@    @@   @@  @@ @@  @@     @@
//    @@   @@@@    @@@     @@   @@  @@ @@@@@      @@
//    @@   @@     @@@@@    @@   @@  @@ @@  @@     @@
//    @@   @@    @@@ @@@   @@   @@  @@ @@  @@ @@  @@
//    @@   @@@@@ @@   @@   @@    @@@@  @@@@@   @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEINIT1(SdrTextObj,SdrAttrObj);

SdrTextObj::SdrTextObj():
    eTextKind(OBJ_TEXT),
    pOutlinerParaObject(NULL),
    pEdtOutl(NULL),
    pFormTextBoundRect(NULL)
{
    bTextSizeDirty=FALSE;
    bPortionInfoChecked=FALSE;
    bTextFrame=FALSE;
    bNoShear=FALSE;
    bNoRotate=FALSE;
    bNoMirror=FALSE;
    bDisableAutoWidthOnDragging=FALSE;
}

SdrTextObj::SdrTextObj(const Rectangle& rNewRect):
    aRect(rNewRect),
    eTextKind(OBJ_TEXT),
    pOutlinerParaObject(NULL),
    pEdtOutl(NULL),
    pFormTextBoundRect(NULL)
{
    bTextSizeDirty=FALSE;
    bPortionInfoChecked=FALSE;
    bTextFrame=FALSE;
    bNoShear=FALSE;
    bNoRotate=FALSE;
    bNoMirror=FALSE;
    bDisableAutoWidthOnDragging=FALSE;
    ImpJustifyRect(aRect);
}

SdrTextObj::SdrTextObj(SdrObjKind eNewTextKind):
    eTextKind(eNewTextKind),
    pOutlinerParaObject(NULL),
    pEdtOutl(NULL),
    pFormTextBoundRect(NULL)
{
    bTextSizeDirty=FALSE;
    bTextFrame=TRUE;
    bNoShear=TRUE;
    bNoRotate=FALSE;
    bNoMirror=TRUE;
    bPortionInfoChecked=FALSE;
    bDisableAutoWidthOnDragging=FALSE;
}

SdrTextObj::SdrTextObj(SdrObjKind eNewTextKind, const Rectangle& rNewRect):
    aRect(rNewRect),
    eTextKind(eNewTextKind),
    pOutlinerParaObject(NULL),
    pEdtOutl(NULL),
    pFormTextBoundRect(NULL)
{
    bTextSizeDirty=FALSE;
    bTextFrame=TRUE;
    bNoShear=TRUE;
    bNoRotate=FALSE;
    bNoMirror=TRUE;
    bPortionInfoChecked=FALSE;
    bDisableAutoWidthOnDragging=FALSE;
    ImpJustifyRect(aRect);
}

SdrTextObj::SdrTextObj(SdrObjKind eNewTextKind, const Rectangle& rNewRect, SvStream& rInput, USHORT eFormat):
    aRect(rNewRect),
    eTextKind(eNewTextKind),
    pOutlinerParaObject(NULL),
    pEdtOutl(NULL),
    pFormTextBoundRect(NULL)
{
    bTextSizeDirty=FALSE;
    bTextFrame=TRUE;
    bNoShear=TRUE;
    bNoRotate=FALSE;
    bNoMirror=TRUE;
    bPortionInfoChecked=FALSE;
    bDisableAutoWidthOnDragging=FALSE;
    ImpJustifyRect(aRect);

    NbcSetText(rInput,eFormat);
}

SdrTextObj::~SdrTextObj()
{
    if( pModel )
    {
        SdrOutliner& rOutl = pModel->GetHitTestOutliner();
        if( rOutl.GetTextObj() == this )
            rOutl.SetTextObj( NULL );
    }

    if (pOutlinerParaObject!=NULL) {
        delete pOutlinerParaObject;
    }
    if (pFormTextBoundRect!=NULL) {
        delete pFormTextBoundRect;
    }
    ImpLinkAbmeldung();
}

void SdrTextObj::FitFrameToTextSize()
{
    DBG_ASSERT(pModel!=NULL,"SdrTextObj::FitFrameToTextSize(): pModel=NULL!")
    ImpJustifyRect(aRect);
    if (pOutlinerParaObject!=NULL && pModel!=NULL) {
        SdrOutliner& rOutliner=ImpGetDrawOutliner();
        rOutliner.SetPaperSize(Size(aRect.Right()-aRect.Left(),aRect.Bottom()-aRect.Top()));
        rOutliner.SetUpdateMode(TRUE);
        rOutliner.SetText(*pOutlinerParaObject);
        Rectangle aTextRect;
        Size aNewSize(rOutliner.CalcTextSize());
        rOutliner.Clear();
        aNewSize.Width()++; // wegen evtl. Rundungsfehler
        aNewSize.Width()+=GetTextLeftDistance()+GetTextRightDistance();
        aNewSize.Height()+=GetTextUpperDistance()+GetTextLowerDistance();
        Rectangle aNewRect(aRect);
        aNewRect.SetSize(aNewSize);
        ImpJustifyRect(aNewRect);
        if (aNewRect!=aRect) {
            SetLogicRect(aNewRect);
        }
    }
}

void SdrTextObj::NbcSetText(const XubString& rStr)
{
    SdrOutliner& rOutliner=ImpGetDrawOutliner();
    rOutliner.SetStyleSheet( 0, GetStyleSheet());
    OutputDevice* pRef1=rOutliner.GetRefDevice();
    rOutliner.SetUpdateMode(TRUE);
    rOutliner.SetText(rStr,rOutliner.GetParagraph( 0 ));
    OutlinerParaObject* pNewText=rOutliner.CreateParaObject();
    Size aSiz(rOutliner.CalcTextSize());
    OutputDevice* pRef2=rOutliner.GetRefDevice();
    rOutliner.Clear();
    NbcSetOutlinerParaObject(pNewText);
    aTextSize=aSiz;
    bTextSizeDirty=FALSE;
}

void SdrTextObj::SetText(const XubString& rStr)
{
    Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetBoundRect();
    SendRepaintBroadcast();
    NbcSetText(rStr);
    SetChanged();
    SendRepaintBroadcast();
    if (GetBoundRect()!=aBoundRect0) {
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
    }
}

void SdrTextObj::NbcSetText(SvStream& rInput, USHORT eFormat)
{
    SdrOutliner& rOutliner=ImpGetDrawOutliner();
    rOutliner.SetStyleSheet( 0, GetStyleSheet());
    rOutliner.Read(rInput,eFormat);
    OutlinerParaObject* pNewText=rOutliner.CreateParaObject();
    rOutliner.SetUpdateMode(TRUE);
    Size aSiz(rOutliner.CalcTextSize());
    rOutliner.Clear();
    NbcSetOutlinerParaObject(pNewText);
    aTextSize=aSiz;
    bTextSizeDirty=FALSE;
}

void SdrTextObj::SetText(SvStream& rInput, USHORT eFormat)
{
    Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetBoundRect();
    SendRepaintBroadcast();
    NbcSetText(rInput,eFormat);
    SetChanged();
    SendRepaintBroadcast();
    if (GetBoundRect()!=aBoundRect0) {
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
    }
}

const Size& SdrTextObj::GetTextSize() const
{
    if (bTextSizeDirty) {
        Size aSiz;
        if (pOutlinerParaObject!=NULL) {
            SdrOutliner& rOutliner=ImpGetDrawOutliner();
            rOutliner.SetText(*pOutlinerParaObject);
            rOutliner.SetUpdateMode(TRUE);
            aSiz=rOutliner.CalcTextSize();
            rOutliner.Clear();
        }
        // 2x casting auf nonconst
        ((SdrTextObj*)this)->aTextSize=aSiz;
        ((SdrTextObj*)this)->bTextSizeDirty=FALSE;
    }
    return aTextSize;
}

FASTBOOL SdrTextObj::IsAutoGrowHeight() const
{
    if(!bTextFrame)
        return FALSE; // AutoGrow nur bei TextFrames

    const SfxItemSet& rSet = GetItemSet();
    BOOL bRet = ((SdrTextAutoGrowHeightItem&)(rSet.Get(SDRATTR_TEXT_AUTOGROWHEIGHT))).GetValue();

    if(bRet)
    {
        SdrTextAniKind eAniKind = ((SdrTextAniKindItem&)(rSet.Get(SDRATTR_TEXT_ANIKIND))).GetValue();

        if(eAniKind == SDRTEXTANI_SCROLL || eAniKind == SDRTEXTANI_ALTERNATE || eAniKind == SDRTEXTANI_SLIDE)
        {
            SdrTextAniDirection eDirection = ((SdrTextAniDirectionItem&)(rSet.Get(SDRATTR_TEXT_ANIDIRECTION))).GetValue();

            if(eDirection == SDRTEXTANI_UP || eDirection == SDRTEXTANI_DOWN)
            {
                bRet = FALSE;
            }
        }
    }
    return bRet;
}

FASTBOOL SdrTextObj::IsAutoGrowWidth() const
{
    if(!bTextFrame)
        return FALSE; // AutoGrow nur bei TextFrames

    const SfxItemSet& rSet = GetItemSet();
    BOOL bRet = ((SdrTextAutoGrowHeightItem&)(rSet.Get(SDRATTR_TEXT_AUTOGROWWIDTH))).GetValue();

    if(bRet)
    {
        SdrTextAniKind eAniKind = ((SdrTextAniKindItem&)(rSet.Get(SDRATTR_TEXT_ANIKIND))).GetValue();

        if(eAniKind == SDRTEXTANI_SCROLL || eAniKind == SDRTEXTANI_ALTERNATE || eAniKind == SDRTEXTANI_SLIDE)
        {
            SdrTextAniDirection eDirection = ((SdrTextAniDirectionItem&)(rSet.Get(SDRATTR_TEXT_ANIDIRECTION))).GetValue();

            if(eDirection == SDRTEXTANI_LEFT || eDirection == SDRTEXTANI_RIGHT)
            {
                bRet = FALSE;
            }
        }
    }
    return bRet;
}

SdrTextHorzAdjust SdrTextObj::GetTextHorizontalAdjust() const
{
    if(IsContourTextFrame())
        return SDRTEXTHORZADJUST_BLOCK;

    const SfxItemSet& rSet = GetItemSet();
    SdrTextHorzAdjust eRet = ((SdrTextHorzAdjustItem&)(rSet.Get(SDRATTR_TEXT_HORZADJUST))).GetValue();

    if(eRet == SDRTEXTHORZADJUST_BLOCK)
    {
        SdrTextAniKind eAniKind = ((SdrTextAniKindItem&)(rSet.Get(SDRATTR_TEXT_ANIKIND))).GetValue();

        if(eAniKind == SDRTEXTANI_SCROLL || eAniKind == SDRTEXTANI_ALTERNATE || eAniKind == SDRTEXTANI_SLIDE)
        {
            SdrTextAniDirection eDirection = ((SdrTextAniDirectionItem&)(rSet.Get(SDRATTR_TEXT_ANIDIRECTION))).GetValue();

            if(eDirection == SDRTEXTANI_LEFT || eDirection == SDRTEXTANI_RIGHT)
            {
                eRet = SDRTEXTHORZADJUST_LEFT;
            }
        }
    }
    return eRet;
} // defaults: BLOCK fuer Textrahmen, CENTER fuer beschriftete Grafikobjekte

SdrTextVertAdjust SdrTextObj::GetTextVerticalAdjust() const
{
    if(IsContourTextFrame())
        return SDRTEXTVERTADJUST_TOP;

    SdrTextVertAdjust eRet = ((SdrTextVertAdjustItem&)(GetItem(SDRATTR_TEXT_VERTADJUST))).GetValue();
    return eRet;
} // defaults: TOP fuer Textrahmen, CENTER fuer beschriftete Grafikobjekte

void SdrTextObj::ImpJustifyRect(Rectangle& rRect) const
{
    if (!rRect.IsEmpty()) {
        rRect.Justify();
        if (rRect.Left()==rRect.Right()) rRect.Right()++;
        if (rRect.Top()==rRect.Bottom()) rRect.Bottom()++;
    }
}

void SdrTextObj::ImpCheckShear()
{
    if (bNoShear && aGeo.nShearWink!=0) {
        aGeo.nShearWink=0;
        aGeo.nTan=0;
    }
}

void SdrTextObj::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
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
    XFillStyle eFillStyle = ((XFillStyleItem&)(GetItem(XATTR_FILLSTYLE))).GetValue();
    rInfo.bGradientAllowed = (eFillStyle == XFILL_GRADIENT);
    rInfo.bShearAllowed     =bNoTextFrame;
    rInfo.bEdgeRadiusAllowed=TRUE;
    FASTBOOL bCanConv=ImpCanConvTextToCurve();
    rInfo.bCanConvToPath    =bCanConv;
    rInfo.bCanConvToPoly    =bCanConv;
    rInfo.bCanConvToPathLineToArea=bCanConv;
    rInfo.bCanConvToPolyLineToArea=bCanConv;
    rInfo.bCanConvToContour = (rInfo.bCanConvToPoly || LineGeometryUsageIsNecessary());
}

UINT16 SdrTextObj::GetObjIdentifier() const
{
    return USHORT(eTextKind);
}

FASTBOOL SdrTextObj::HasEditText() const
{
    FASTBOOL bRet=FALSE;
    if (pEdtOutl!=NULL) {
        Paragraph* p1stPara=pEdtOutl->GetParagraph( 0 );
        ULONG nParaAnz=pEdtOutl->GetParagraphCount();
        if (p1stPara==NULL) nParaAnz=0;
        if (nParaAnz==1) { // bei nur einem Para nachsehen ob da ueberhaupt was drin steht
            XubString aStr(pEdtOutl->GetText(p1stPara));

            // Aha, steht nix drin!
            if(!aStr.Len())
                nParaAnz = 0;
        }
        bRet=nParaAnz!=0;
    }
    return bRet;
}

void SdrTextObj::SetPage(SdrPage* pNewPage)
{
    FASTBOOL bRemove=pNewPage==NULL && pPage!=NULL;
    FASTBOOL bInsert=pNewPage!=NULL && pPage==NULL;
    FASTBOOL bLinked=IsLinkedText();

    if (bLinked && bRemove) {
        ImpLinkAbmeldung();
    }

    SdrAttrObj::SetPage(pNewPage);

    if (bLinked && bInsert) {
        ImpLinkAnmeldung();
    }
}

void SdrTextObj::SetModel(SdrModel* pNewModel)
{
    const SfxItemSet& rSet = GetItemSet();
    SdrModel* pOldModel=pModel;
    BOOL bLinked=IsLinkedText();
    BOOL bChg=pNewModel!=pModel;
    BOOL bHgtSet = rSet.GetItemState(EE_CHAR_FONTHEIGHT, TRUE) == SFX_ITEM_SET;
    if (bLinked && bChg) {
        ImpLinkAbmeldung();
    }

    SdrAttrObj::SetModel(pNewModel);

    if (bChg && pOutlinerParaObject!=NULL && pOldModel!=NULL && pNewModel!=NULL) {
        MapUnit aOldUnit(pOldModel->GetScaleUnit());
        MapUnit aNewUnit(pNewModel->GetScaleUnit());
        FASTBOOL bScaleUnitChanged=aNewUnit!=aOldUnit;
        SetTextSizeDirty();
        // und nun dem OutlinerParaObject einen neuen Pool verpassen
        // !!! Hier muss noch DefTab und RefDevice der beiden Models
        // !!! verglichen werden und dann ggf. AutoGrow zuschlagen !!!
        // !!! fehlende Implementation !!!
        ULONG nOldFontHgt=pOldModel->GetDefaultFontHeight();
        ULONG nNewFontHgt=pNewModel->GetDefaultFontHeight();
        BOOL bDefHgtChanged=nNewFontHgt!=nOldFontHgt;
        BOOL bSetHgtItem=bDefHgtChanged && !bHgtSet;
        if (bSetHgtItem) { // #32665#
            // zunaechst das HeightItem festklopfen, damit
            // 1. Es eben bestehen bleibt und
            // 2. DoStretchChars vom richtigen Wert ausgeht
            SetItem(SvxFontHeightItem(nOldFontHgt));
        }
        // erst jetzt den Outliner holen, etc. damit obiges SetAttr auch wirkt
        SdrOutliner& rOutliner=ImpGetDrawOutliner();
        rOutliner.SetText(*pOutlinerParaObject);
        delete pOutlinerParaObject;
        pOutlinerParaObject=NULL;
        if (bScaleUnitChanged) {
            Fraction aMetricFactor=GetMapFactor(aOldUnit,aNewUnit).X();

            // Funktioniert nicht richtig:
            // Geht am Outliner leider nur in %
            // double nPercFloat=double(aMetricFactor)*100+0.5;
            // USHORT nPerc=(USHORT)nPercFloat;
            // rOutliner.DoStretchChars(100,nPerc);

            if (bSetHgtItem) {
                // Und nun noch das Rahmenattribut korregieren
                nOldFontHgt=BigMulDiv(nOldFontHgt,aMetricFactor.GetNumerator(),aMetricFactor.GetDenominator());
                SetItem(SvxFontHeightItem(nOldFontHgt));
            }
        }
        SetOutlinerParaObject(rOutliner.CreateParaObject()); // #34494#
        pOutlinerParaObject->ClearPortionInfo();
        bPortionInfoChecked=FALSE;
        rOutliner.Clear();
        //ImpSetTextStyleSheetListeners();
    }

    if (bLinked && bChg) {
        ImpLinkAnmeldung();
    }
}

FASTBOOL SdrTextObj::NbcSetEckenradius(long nRad)
{
    SetItem(SdrEckenradiusItem(nRad));
    return TRUE;
}

FASTBOOL SdrTextObj::NbcSetAutoGrowHeight(FASTBOOL bAuto)
{
    if(bTextFrame)
    {
        SetItem(SdrTextAutoGrowHeightItem(bAuto));
        return TRUE;
    }
    return FALSE;
}

FASTBOOL SdrTextObj::NbcSetMinTextFrameHeight(long nHgt)
{
    if(bTextFrame)
    {
        SetItem(SdrTextMinFrameHeightItem(nHgt));

        // #84974# use bDisableAutoWidthOnDragging as
        // bDisableAutoHeightOnDragging if vertical.
        if(IsVerticalWriting() && bDisableAutoWidthOnDragging)
        {
            bDisableAutoWidthOnDragging = FALSE;
            SetItem(SdrTextAutoGrowHeightItem(FALSE));
        }

        return TRUE;
    }
    return FALSE;
}

FASTBOOL SdrTextObj::NbcSetMaxTextFrameHeight(long nHgt)
{
    if(bTextFrame)
    {
        SetItem(SdrTextMaxFrameHeightItem(nHgt));
        return TRUE;
    }
    return FALSE;
}

FASTBOOL SdrTextObj::NbcSetAutoGrowWidth(FASTBOOL bAuto)
{
    if(bTextFrame)
    {
        SetItem(SdrTextAutoGrowWidthItem(bAuto));
        return TRUE;
    }
    return FALSE;
}

FASTBOOL SdrTextObj::NbcSetMinTextFrameWidth(long nWdt)
{
    if(bTextFrame)
    {
        SetItem(SdrTextMinFrameWidthItem(nWdt));

        // #84974# use bDisableAutoWidthOnDragging only
        // when not vertical.
        if(!IsVerticalWriting() && bDisableAutoWidthOnDragging)
        {
            bDisableAutoWidthOnDragging = FALSE;
            SetItem(SdrTextAutoGrowWidthItem(FALSE));
        }

        return TRUE;
    }
    return FALSE;
}

FASTBOOL SdrTextObj::NbcSetMaxTextFrameWidth(long nWdt)
{
    if(bTextFrame)
    {
        SetItem(SdrTextMaxFrameWidthItem(nWdt));
        return TRUE;
    }
    return FALSE;
}

FASTBOOL SdrTextObj::NbcSetFitToSize(SdrFitToSizeType eFit)
{
    if(bTextFrame)
    {
        SetItem(SdrTextFitToSizeTypeItem(eFit));
        return TRUE;
    }
    return FALSE;
}

void SdrTextObj::ImpSetContourPolygon( SdrOutliner& rOutliner, Rectangle& rAnchorRect, BOOL bLineWidth ) const
{

    XPolyPolygon aXorXPP;
    TakeXorPoly(aXorXPP, FALSE);
    if (aGeo.nDrehWink!=0) { // Unrotate!
        RotateXPoly(aXorXPP,rAnchorRect.TopLeft(),-aGeo.nSin,aGeo.nCos);
    }
    Point aRef(rAnchorRect.TopLeft());
    aXorXPP.Move(-aRef.X(),-aRef.Y());

    XPolyPolygon* pContourXPP = NULL;

    if( bLineWidth )
    {
        // Strichstaerke beruecksichtigen
        // Beim Hittest muss das unterbleiben (Performance!)
        pContourXPP = new XPolyPolygon();

        // #86258# test if shadow needs to be avoided for TakeContour()
        const SfxItemSet& rSet = GetItemSet();
        sal_Bool bShadowOn = ((SdrShadowItem&)(rSet.Get(SDRATTR_SHADOW))).GetValue();

        if(bShadowOn)
        {
            // #86258# force shadow off
            SdrObject* pCopy = Clone();
            pCopy->SetItem(SdrShadowItem(FALSE));
            pCopy->TakeContour(*pContourXPP);
            delete pCopy;
        }
        else
        {
            TakeContour(*pContourXPP);
        }

        if (aGeo.nDrehWink!=0)  // Unrotate!
            RotateXPoly(*pContourXPP,rAnchorRect.TopLeft(),-aGeo.nSin,aGeo.nCos);
        pContourXPP->Move(-aRef.X(),-aRef.Y());
    }

    rOutliner.SetPolygon(aXorXPP, pContourXPP);
}

void SdrTextObj::TakeUnrotatedSnapRect(Rectangle& rRect) const
{
    rRect=aRect;
}

void SdrTextObj::TakeTextAnchorRect(Rectangle& rAnchorRect) const
{
    long nLeftDist=GetTextLeftDistance();
    long nRightDist=GetTextRightDistance();
    long nUpperDist=GetTextUpperDistance();
    long nLowerDist=GetTextLowerDistance();
    Rectangle aAnkRect(aRect); // Rect innerhalb dem geankert wird
    FASTBOOL bFrame=IsTextFrame();
    if (!bFrame) {
        TakeUnrotatedSnapRect(aAnkRect);
    }
    Point aRotateRef(aAnkRect.TopLeft());
    aAnkRect.Left()+=nLeftDist;
    aAnkRect.Top()+=nUpperDist;
    aAnkRect.Right()-=nRightDist;
    aAnkRect.Bottom()-=nLowerDist;
    if (bFrame) {
        // !!! hier noch etwas verfeinern !!!
        if (aAnkRect.GetWidth()<2) aAnkRect.Right()=aAnkRect.Left()+1;   // Mindestgroesse 2
        if (aAnkRect.GetHeight()<2) aAnkRect.Bottom()=aAnkRect.Top()+1;  // Mindestgroesse 2
    }
    if (aGeo.nDrehWink!=0) {
        Point aTmpPt(aAnkRect.TopLeft());
        RotatePoint(aTmpPt,aRotateRef,aGeo.nSin,aGeo.nCos);
        aTmpPt-=aAnkRect.TopLeft();
        aAnkRect.Move(aTmpPt.X(),aTmpPt.Y());
    }
    rAnchorRect=aAnkRect;
}

void SdrTextObj::TakeTextRect( SdrOutliner& rOutliner, Rectangle& rTextRect, FASTBOOL bNoEditText,
                               Rectangle* pAnchorRect, BOOL bLineWidth ) const
{
    Rectangle aAnkRect; // Rect innerhalb dem geankert wird
    TakeTextAnchorRect(aAnkRect);
    SdrTextVertAdjust eVAdj=GetTextVerticalAdjust();
    SdrTextHorzAdjust eHAdj=GetTextHorizontalAdjust();
    SdrTextAniKind      eAniKind=GetTextAniKind();
    SdrTextAniDirection eAniDirection=GetTextAniDirection();

    SdrFitToSizeType eFit=GetFitToSize();
    FASTBOOL bFitToSize=(eFit==SDRTEXTFIT_PROPORTIONAL || eFit==SDRTEXTFIT_ALLLINES);
    FASTBOOL bContourFrame=IsContourTextFrame();

    FASTBOOL bFrame=IsTextFrame();
    ULONG nStat0=rOutliner.GetControlWord();
    Size aNullSize;
    if (!bContourFrame)
    {
        rOutliner.SetControlWord(nStat0|EE_CNTRL_AUTOPAGESIZE);
        rOutliner.SetMinAutoPaperSize(aNullSize);
        rOutliner.SetMaxAutoPaperSize(Size(1000000,1000000));
    }

    if (!bFitToSize && !bContourFrame)
    {
        long nAnkWdt=aAnkRect.GetWidth();
        long nAnkHgt=aAnkRect.GetHeight();
        if (bFrame)
        {
            long nWdt=nAnkWdt;
            long nHgt=nAnkHgt;
            if (eAniKind==SDRTEXTANI_SCROLL || eAniKind==SDRTEXTANI_ALTERNATE || eAniKind==SDRTEXTANI_SLIDE)
            {
                // Grenzenlose Papiergroesse fuer Laufschrift
                if (eAniDirection==SDRTEXTANI_LEFT || eAniDirection==SDRTEXTANI_RIGHT) nWdt=1000000;
                if (eAniDirection==SDRTEXTANI_UP || eAniDirection==SDRTEXTANI_DOWN) nHgt=1000000;
            }
            rOutliner.SetMaxAutoPaperSize(Size(nWdt,nHgt));
        }

        // #100801# MinAutoPaperSize needs always to be set completely
        // when Verical
        if(IsVerticalWriting())
        {
            rOutliner.SetMinAutoPaperSize(Size(nAnkWdt, nAnkHgt));
        }

        if(SDRTEXTHORZADJUST_BLOCK == eHAdj)
        {
            // #89459#
            if(!IsVerticalWriting())
            {
                rOutliner.SetMinAutoPaperSize(Size(nAnkWdt, 0));
            }
        }
    }

    rOutliner.SetPaperSize(aNullSize);
    if (bContourFrame)
        ImpSetContourPolygon( rOutliner, aAnkRect, bLineWidth );

    // Text in den Outliner stecken - ggf. den aus dem EditOutliner
    OutlinerParaObject* pPara=pOutlinerParaObject;
    if (pEdtOutl && !bNoEditText)
        pPara=pEdtOutl->CreateParaObject();

    if (pPara)
    {
        BOOL bHitTest = FALSE;
        if( pModel )
            bHitTest = &pModel->GetHitTestOutliner() == &rOutliner;

        const SdrTextObj* pTestObj = rOutliner.GetTextObj();
        if( !pTestObj || !bHitTest || pTestObj != this ||
            pTestObj->GetOutlinerParaObject() != pOutlinerParaObject )
        {
            if( bHitTest )
                rOutliner.SetTextObj( this );

            rOutliner.SetUpdateMode(TRUE);
            rOutliner.SetText(*pPara);
        }
    }
    else
    {
//      rOutliner.Clear();
        rOutliner.SetTextObj( NULL );
    }

    if (pEdtOutl && !bNoEditText && pPara)
        delete pPara;

    rOutliner.SetUpdateMode(TRUE);
    rOutliner.SetControlWord(nStat0);

    if (!bPortionInfoChecked)
    {
        // Optimierung: ggf. BigTextObject erzeugen
        ((SdrTextObj*)this)->bPortionInfoChecked=TRUE;
        if (pOutlinerParaObject!=NULL && rOutliner.ShouldCreateBigTextObject())
            ((SdrTextObj*)this)->pOutlinerParaObject=rOutliner.CreateParaObject();
    }

    Point aTextPos(aAnkRect.TopLeft());
    Size aTextSiz(rOutliner.GetPaperSize()); // GetPaperSize() hat etwas Toleranz drauf, oder?

    if (eHAdj==SDRTEXTHORZADJUST_CENTER || eHAdj==SDRTEXTHORZADJUST_RIGHT)
    {
        long nFreeWdt=aAnkRect.GetWidth()-aTextSiz.Width();
        if (eHAdj==SDRTEXTHORZADJUST_CENTER)
            aTextPos.X()+=nFreeWdt/2;
        if (eHAdj==SDRTEXTHORZADJUST_RIGHT)
            aTextPos.X()+=nFreeWdt;
    }
    if (eVAdj==SDRTEXTVERTADJUST_CENTER || eVAdj==SDRTEXTVERTADJUST_BOTTOM)
    {
        long nFreeHgt=aAnkRect.GetHeight()-aTextSiz.Height();
        if (eVAdj==SDRTEXTVERTADJUST_CENTER)
            aTextPos.Y()+=nFreeHgt/2;
        if (eVAdj==SDRTEXTVERTADJUST_BOTTOM)
            aTextPos.Y()+=nFreeHgt;
    }
    if (aGeo.nDrehWink!=0)
        RotatePoint(aTextPos,aAnkRect.TopLeft(),aGeo.nSin,aGeo.nCos);

    if (pAnchorRect)
        *pAnchorRect=aAnkRect;

    // rTextRect ist bei ContourFrame in einigen Faellen nicht korrekt
    rTextRect=Rectangle(aTextPos,aTextSiz);
    if (bContourFrame)
        rTextRect=aAnkRect;
}

OutlinerParaObject* SdrTextObj::GetEditOutlinerParaObject() const
{
    OutlinerParaObject* pPara=NULL;
    if (pEdtOutl!=NULL) { // Wird gerade editiert, also das ParaObject aus dem aktiven Editor verwenden
        Paragraph* p1stPara=pEdtOutl->GetParagraph( 0 );
        ULONG nParaAnz=pEdtOutl->GetParagraphCount();
        if (nParaAnz==1 && p1stPara!=NULL) { // bei nur einem Para nachsehen ob da ueberhaupt was drin steht
            XubString aStr(pEdtOutl->GetText(p1stPara));

            // Aha, steht nix drin!
            if(!aStr.Len())
                nParaAnz = 0;
        }
        if (p1stPara!=NULL && nParaAnz!=0) {
            pPara = pEdtOutl->CreateParaObject(0, (sal_uInt16)nParaAnz);
        }
    }
    return pPara;
}

void SdrTextObj::ImpSetCharStretching(SdrOutliner& rOutliner, const Rectangle& rTextRect, const Rectangle& rAnchorRect, Fraction& rFitXKorreg) const
{
    OutputDevice* pOut = rOutliner.GetRefDevice();
    BOOL bNoStretching(FALSE);

    if(pOut && pOut->GetOutDevType() == OUTDEV_PRINTER)
    {
        // #35762#: Checken ob CharStretching ueberhaupt moeglich
        GDIMetaFile* pMtf = pOut->GetConnectMetaFile();
        UniString aTestString(sal_Unicode('J'));

        if(pMtf && (!pMtf->IsRecord() || pMtf->IsPause()))
            pMtf = NULL;

        if(pMtf)
            pMtf->Pause(TRUE);

        Font aFontMerk(pOut->GetFont());
        Font aTmpFont( OutputDevice::GetDefaultFont( DEFAULTFONT_SERIF, LANGUAGE_SYSTEM, DEFAULTFONT_FLAGS_ONLYONE ) );

        aTmpFont.SetSize(Size(0,100));
        pOut->SetFont(aTmpFont);
        Size aSize1(pOut->GetTextWidth(aTestString), pOut->GetTextHeight());
        aTmpFont.SetSize(Size(800,100));
        pOut->SetFont(aTmpFont);
        Size aSize2(pOut->GetTextWidth(aTestString), pOut->GetTextHeight());
        pOut->SetFont(aFontMerk);

        if(pMtf)
            pMtf->Pause(FALSE);

        bNoStretching = (aSize1 == aSize2);

#ifdef WNT
        // #35762# Windows vergroessert bei Size(100,500) den Font proportional
        // Und das finden wir nicht so schoen.
        if(aSize2.Height() >= aSize1.Height() * 2)
        {
            bNoStretching = TRUE;
        }
#endif
    }
    unsigned nLoopCount=0;
    FASTBOOL bNoMoreLoop=FALSE;
    long nXDiff0=0x7FFFFFFF;
    long nWantWdt=rAnchorRect.Right()-rAnchorRect.Left();
    long nIsWdt=rTextRect.Right()-rTextRect.Left();
    if (nIsWdt==0) nIsWdt=1;

    long nWantHgt=rAnchorRect.Bottom()-rAnchorRect.Top();
    long nIsHgt=rTextRect.Bottom()-rTextRect.Top();
    if (nIsHgt==0) nIsHgt=1;

    long nXTolPl=nWantWdt/100; // Toleranz +1%
    long nXTolMi=nWantWdt/25;  // Toleranz -4%
    long nXKorr =nWantWdt/20;  // Korrekturmasstab 5%

    long nX=(nWantWdt*100) /nIsWdt; // X-Stretching berechnen
    long nY=(nWantHgt*100) /nIsHgt; // Y-Stretching berechnen
    FASTBOOL bChkX=TRUE;
    FASTBOOL bChkY=TRUE;
    if (bNoStretching) { // #35762# evtl. nur proportional moeglich
        if (nX>nY) { nX=nY; bChkX=FALSE; }
        else { nY=nX; bChkY=FALSE; }
    }

    while (nLoopCount<5 && !bNoMoreLoop) {
        if (nX<0) nX=-nX;
        if (nX<1) { nX=1; bNoMoreLoop=TRUE; }
        if (nX>65535) { nX=65535; bNoMoreLoop=TRUE; }

        if (nY<0) nY=-nY;
        if (nY<1) { nY=1; bNoMoreLoop=TRUE; }
        if (nY>65535) { nY=65535; bNoMoreLoop=TRUE; }

        // exception, there is no text yet (horizontal case)
        if(nIsWdt <= 1)
        {
            nX = nY;
            bNoMoreLoop = TRUE;
        }

        // #87877# exception, there is no text yet (vertical case)
        if(nIsHgt <= 1)
        {
            nY = nX;
            bNoMoreLoop = TRUE;
        }

        rOutliner.SetGlobalCharStretching((USHORT)nX,(USHORT)nY);
        nLoopCount++;
        Size aSiz(rOutliner.CalcTextSize());
        long nXDiff=aSiz.Width()-nWantWdt;
        rFitXKorreg=Fraction(nWantWdt,aSiz.Width());
        if (((nXDiff>=nXTolMi || !bChkX) && nXDiff<=nXTolPl) || nXDiff==nXDiff0/*&& Abs(nYDiff)<=nYTol*/) {
            bNoMoreLoop=TRUE;
        } else {
            // Stretchingfaktoren korregieren
            long nMul=nWantWdt;
            long nDiv=aSiz.Width();
            if (Abs(nXDiff)<=2*nXKorr) {
                if (nMul>nDiv) nDiv+=(nMul-nDiv)/2; // und zwar nur um die haelfte des berechneten
                else nMul+=(nDiv-nMul)/2;           // weil die EE ja eh wieder falsch rechnet
            }
            nX=nX*nMul/nDiv;
            if (bNoStretching) nY=nX;
        }
        nXDiff0=nXDiff;
    }
}

FASTBOOL SdrTextObj::Paint(ExtOutputDevice& rXOut, const SdrPaintInfoRec& rInfoRec) const
{
    // Hidden objects on masterpages, draw nothing
    if((rInfoRec.nPaintMode & SDRPAINTMODE_MASTERPAGE) && bNotVisibleAsMaster)
        return TRUE;

    FASTBOOL bOk=TRUE;
    FASTBOOL bPrinter=rXOut.GetOutDev()->GetOutDevType()==OUTDEV_PRINTER;

    if (bPrinter && bEmptyPresObj)
        return bOk; // Leere Praesentationsobjekte nicht drucken!

    if (!bPrinter && pEdtOutl!=NULL && rInfoRec.pPV!=NULL &&
        rInfoRec.pPV->GetView().GetTextEditObject()==(SdrObject*)this)
        return bOk; // Textobjekt wird gerade editiert in der auffordernen View

    if ((rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTTEXT) ==0)
    {
        if (pOutlinerParaObject!=NULL || (pEdtOutl!=NULL && HasEditText()))
        {
            SdrOutliner& rOutliner=ImpGetDrawOutliner();

            {
                SvtAccessibilityOptions aOptions;
                rOutliner.ForceAutoColor( aOptions.GetIsAutomaticFontColor() );
            }

            FASTBOOL bContourFrame=IsContourTextFrame();
            if (IsFontwork() && !bContourFrame)
            { // FontWork
                if (pModel!=NULL)
                {
                    rOutliner.SetUpdateMode(TRUE); // hier kann ggf. noch optimiert werden !!!
                    ImpTextPortionHandler aTPHandler(rOutliner,*this);

                    // #78478# to have the outline color in XOutputDevice::ImpDrawFormText(...)
                    // SetLineAttr(...) needs to be called if the outline item is set
                    const SfxItemSet& rSet = GetItemSet();
                    BOOL bFormTextOutline = ((XFormTextOutlineItem&)(rSet.Get(XATTR_FORMTXTOUTLINE))).GetValue();

                    if(bFormTextOutline)
                        rXOut.SetLineAttr(rSet);

                    rXOut.SetTextAttr(rSet);

                    aTPHandler.DrawTextToPath(rXOut); // drucken bei aktivem Textedit fehlt hier
                    rOutliner.Clear();
                }
            }
            else
            {
                // sonst kein Fontwork
                // hier findet das richtige Painten des Textes statt

                Rectangle aTextRect;
                Rectangle aAnchorRect;
                Rectangle aPaintRect;
                Fraction aFitXKorreg(1,1);

                // #101029#: Extracted Outliner setup to ImpSetupDrawOutlinerForPaint
                ImpSetupDrawOutlinerForPaint( bContourFrame, rOutliner, aTextRect, aAnchorRect, aPaintRect, aFitXKorreg );

                FASTBOOL bAnimated=GetTextAniKind()!=SDRTEXTANI_NONE;
                OutputDevice* pOutDev=rXOut.GetOutDev();

                // #98825# Text animation is initialized and strted from this Paint()
                // no one seems to use StartTextAnimation(...) or StopTextAnimation(...)
                // at all. Thus, I need to stop text animation here when necessary.
                sal_Bool bDoNotPaintAnimatedText =
                    bPrinter || (rInfoRec.nPaintMode & SDRPAINTMODE_ANILIKEPRN) !=0 || !bAnimated;

                if(!bDoNotPaintAnimatedText)
                {
                    // if animated text is disabled, do not start it here...
                    if(0L != rInfoRec.pPV)
                    {
                        const SdrView& rTargetView = rInfoRec.pPV->GetView();
                        const SvtAccessibilityOptions& rOpt = ((SdrView&)rTargetView).getAccessibilityOptions();
                        sal_Bool bIsAllowedAnimatedText = rOpt.GetIsAllowAnimatedText();

                        if(!bIsAllowedAnimatedText)
                        {
                            bDoNotPaintAnimatedText = sal_True;

                            if(pPlusData && pPlusData->pAnimator)
                            {
                                pPlusData->pAnimator->Stop();
                                delete pPlusData->pAnimator;
                                pPlusData->pAnimator = 0L;
                            }
                        }
                    }
                }

                if(bDoNotPaintAnimatedText)
                {
                    if (!bAnimated && pPlusData!=NULL && pPlusData->pAnimator!=NULL)
                    {
                        delete pPlusData->pAnimator;
                        pPlusData->pAnimator=NULL;
                    }
                    if (aGeo.nDrehWink!=0)
                    {
                        // #49328# bei AutoGrowHeight()=TRUE nicht mehr clippen
                        FASTBOOL bNeedClip=(bTextFrame && !IsAutoGrowHeight()) || bContourFrame;
                        // ClipRegion setzen. Das macht Malte bei gedrehter Ausgabe naemlich nicht!
                        FASTBOOL bMtf=pOutDev->GetConnectMetaFile()!=NULL;
                        // Clipping merken
                        FASTBOOL bClip0=pOutDev->IsClipRegion();
                        Region   aClip0(pOutDev->GetClipRegion());
                        if (bNeedClip)
                        {
                            if (bMtf) pOutDev->Push();
                            // Neues Clipping setzen
                            Rectangle aClipRect(aPaintRect);
                            if (bPrinter)
                            { // #42520#: Bei HP-Druckern fehlt sonst oefter der letzte Buchstabe einer Zeile
                                Size a1Pix(pOutDev->PixelToLogic(Size(1,1)));
                                aClipRect.Top()-=a1Pix.Width();
                                aClipRect.Left()-=a1Pix.Height();
                                aClipRect.Right()+=a1Pix.Width();
                                aClipRect.Bottom()+=a1Pix.Height();
                            }
                            Polygon aClipPoly(aClipRect);
                            RotatePoly(aClipPoly,aPaintRect.TopLeft(),aGeo.nSin,aGeo.nCos);
                            // Intersect geht leider nicht, weil Poly statt Rect
                            pOutDev->SetClipRegion(aClipPoly);
                            if (bClip0)
                            {
                                // Aber wenn's vorher nur ein Rechteck war, dann
                                // intersecte ich mein Poly nun mit diesem
                                pOutDev->IntersectClipRegion(aClip0.GetBoundRect());
                            }
                        }
                        // Textausgabe
                        rOutliner.Draw(pOutDev,aPaintRect.TopLeft(),(short)(aGeo.nDrehWink/10));
                        if (bNeedClip)
                        {
                            // Clipping restaurieren
                            if (bMtf)
                                pOutDev->Pop();
                            else
                            {
                                if (bClip0)
                                    pOutDev->SetClipRegion(aClip0);
                                else
                                    pOutDev->SetClipRegion();
                            }
                        }
                    }
                    else
                    {
                        if(IsVerticalWriting())
                        {
                            // new try for #82826#
                            if(aAnchorRect.GetWidth() > aPaintRect.GetWidth())
                            {
                                aPaintRect = Rectangle(
                                    aPaintRect.Right() - aAnchorRect.GetWidth(), aPaintRect.Top(),
                                    aPaintRect.Right(), aPaintRect.Bottom());
                            }

                            // #91744# for vertical writing the original fix #82826#
                            // needs to be taken out.
                            rOutliner.Draw(pOutDev, aPaintRect);
                        }
                        else
                        {
                            // new try for #82826#
                            if(aAnchorRect.GetHeight() > aPaintRect.GetHeight())
                            {
                                aPaintRect = Rectangle(
                                    aPaintRect.Left(), aPaintRect.Top(),
                                    aPaintRect.Right(), aPaintRect.Top() + aAnchorRect.GetHeight());
                            }

                            // #91809# for horizontal writing the original fix #82826#
                            // needs to be taken out, too.
                            rOutliner.Draw(pOutDev, aPaintRect);

                            // #82826# for correct preview of outliner views
                            //// rOutliner.Draw(pOutDev,aPaintRect);
                            //if(aPaintRect.Top() > aAnchorRect.Top())
                            //  rOutliner.Draw(pOutDev, aPaintRect);
                            //else
                            //  rOutliner.Draw(pOutDev, aAnchorRect);
                        }
                    }
                }
                else
                {
                    ImpPaintAnimatedText(*rXOut.GetOutDev(),rXOut.GetOffset(),rOutliner,aAnchorRect,aPaintRect,rInfoRec);
                }

                rOutliner.Clear();
            }
        } // if (pPara!=NULL)
        if (bEmptyPresObj)
        {
            // leere Praesentationsobjekte bekommen einen grauen Rahmen
            svx::ColorConfig aColorConfig;
            svx::ColorConfigValue aColor( aColorConfig.GetColorValue( svx::OBJECTBOUNDARIES ) );

            if( aColor.bIsVisible )
            {
                rXOut.GetOutDev()->SetFillColor();
                rXOut.GetOutDev()->SetLineColor( aColor.nColor );

                if (aGeo.nDrehWink!=0 || aGeo.nShearWink!=0)
                {
                    Polygon aPoly(aRect);
                    if (aGeo.nShearWink!=0)
                        ShearPoly(aPoly,aRect.TopLeft(),aGeo.nTan);

                    if (aGeo.nDrehWink!=0)
                        RotatePoly(aPoly,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);

                    rXOut.GetOutDev()->DrawPolyLine(aPoly);
                }
                else
                {
                    rXOut.GetOutDev()->DrawRect(aRect);
                }
            }
        } // if pOutlParaObj!=NULL
    }
    else
    { // sonst SDRPAINTMODE_DRAFTTEXT
        FASTBOOL bFill=HasFill();
        FASTBOOL bLine=HasLine();
        FASTBOOL bHide=IsFontwork() && IsHideContour() && pFormTextBoundRect!=NULL;

        if (bHide)
        {
            bFill=FALSE;
            bLine=FALSE;
        }

        if ((bTextFrame && !bLine && !bFill) || bHide)
        {
            OutputDevice* pOut=rXOut.GetOutDev();
            Polygon aPoly;

            if (bHide)
                aPoly=Polygon(*pFormTextBoundRect);
            else
                aPoly=Rect2Poly(aRect,aGeo);

            if (aPoly.GetSize()>=4)
            {
                pOut->SetLineColor(Color(COL_BLACK));
                pOut->DrawPolyLine(aPoly);
                pOut->DrawLine(aPoly[0],aPoly[2]);
                pOut->DrawLine(aPoly[1],aPoly[3]);
            }
        }
    }
    return bOk;
}

// Geht z.Zt. nur wenn das Obj schon wenigstens einmal gepaintet wurde
// Denn dann ist der MtfAnimator initiallisiert
void SdrTextObj::StartTextAnimation(OutputDevice* pOutDev, const Point& rOffset, long nExtraData)
{
    if (GetTextAniKind()!=SDRTEXTANI_NONE && ImpGetMtfAnimator()!=NULL) {
        ImpSdrMtfAnimator* pAnimator=ImpForceMtfAnimator();
        pAnimator->Start(*pOutDev,rOffset,nExtraData);
    }
}

void SdrTextObj::StopTextAnimation(OutputDevice* pOutDev, long nExtraData)
{
    if (pPlusData!=NULL && pPlusData->pAnimator!=NULL) {
        ImpSdrMtfAnimator* pAnimator=pPlusData->pAnimator;
        for (ULONG nInfoNum=pAnimator->GetInfoCount(); nInfoNum>0;) {
            nInfoNum--;
            ImpMtfAnimationInfo* pInfo=pAnimator->GetInfo(nInfoNum);
            if (pInfo->nExtraData==nExtraData &&
                (pOutDev==NULL || pInfo->pOutDev==pOutDev)) pAnimator->RemoveInfo(nInfoNum);
        }
    }
}

FASTBOOL SdrTextObj::ImpPaintAnimatedText(OutputDevice& rOut, const Point& rOffset,
    SdrOutliner& rOutliner, const Rectangle& rAnchorRect, const Rectangle& rPaintRect,
    const SdrPaintInfoRec& rInfoRec) const
{
    SdrTextAniKind eAniKind = GetTextAniKind();
    FASTBOOL bBlink = eAniKind == SDRTEXTANI_BLINK;
    const SfxItemSet& rSet = GetItemSet();
    SdrTextAniDirection eDirection = ((SdrTextAniDirectionItem&)(rSet.Get(SDRATTR_TEXT_ANIDIRECTION))).GetValue();
    ImpSdrMtfAnimator* pAnimator = ((SdrTextObj*)this)->ImpForceMtfAnimator();
    pAnimator->SetAnimationNotifyHdl(LINK(this,SdrTextObj,ImpAnimationHdl));

    // erstmal checken, ob nicht schon laeuft
    ImpMtfAnimationInfo* pRunningInfo = NULL;

    for(ULONG nInfoNum = pAnimator->GetInfoCount(); nInfoNum>0 && pRunningInfo==NULL;) {
        nInfoNum--;
        ImpMtfAnimationInfo* pInfo=pAnimator->GetInfo(nInfoNum);
        if (pInfo->pPageView==rInfoRec.pPV && pInfo->pOutDev==&rOut) {
            if (!pInfo->bBackSaved) pRunningInfo=pInfo;
            else {
                MapMode aMap1(pInfo->aBackground.GetMapMode());
                MapMode aMap2(rOut.GetMapMode());
                if (aMap1.GetMapUnit()==aMap2.GetMapUnit() &&
                    aMap1.GetScaleX()==aMap2.GetScaleX() &&
                    aMap1.GetScaleY()==aMap2.GetScaleY()) pRunningInfo=pInfo;
            }
        }
    }
    if (pRunningInfo!=NULL) {
        if (pRunningInfo->bBackSaved) {
            // Teilinvalidierung des Hintergrunds
            Rectangle aClipRect(rInfoRec.aDirtyRect);
            if (rOut.GetMapMode().GetMapUnit()==MAP_TWIP) {
                HACK(Bei Teilinvalidierung von Laufschrift im Writer 1 Pixel korregieren);
                Size a1Pix(rOut.PixelToLogic(Size(1,1)));
                aClipRect.Top()+=a1Pix.Width();
                aClipRect.Left()+=a1Pix.Height();
                aClipRect.Right()-=a1Pix.Width();
                aClipRect.Bottom()-=a1Pix.Height();
            }
            Region aRegion(aClipRect);
            pRunningInfo->SaveBackground(*pAnimator,&aRegion);
            pRunningInfo->Paint(*pAnimator,rOut);
        }
        return TRUE;
    }
    pAnimator->SetAttributes(rSet);

    Point aRotateRef(rAnchorRect.TopLeft());
    // Die Drehreferenz ist bei allen Rechtecken jeweils das eigene TopLeft()
    // (rPaintRect,rAnchorRect). Ich verschiebe nun aPaintRect so, dass fuer
    // beide die Drehreferenz rAnchorRect.TopLeft() gilt.
    Rectangle aPaintRect(rPaintRect);
    if (GetTextHorizontalAdjust()==SDRTEXTHORZADJUST_BLOCK &&
        GetFitToSize()!=SDRTEXTFIT_NONE) {
        // Bei den Default-Textrahmen muss ich erstmal die laengste Zeile berechnen
        // Das gibt allerdings Probleme bei Absatzformatierungen Center, Rechts
        // und evtl. auch bei Blocksatz.
        Size aSiz(rOutliner.CalcTextSize());
        aPaintRect.Right()=aPaintRect.Left()+aSiz.Width();
        aPaintRect.Bottom()=aPaintRect.Top()+aSiz.Height();
    }

    // PaintRect auf's AnchorRect ausrichten (den selben Drehpunkt fuer alle)
    if (aGeo.nDrehWink!=0) {
        if (aPaintRect.TopLeft()!=aRotateRef) {
            Point aTmpPt(aPaintRect.TopLeft());
            RotatePoint(aTmpPt,aRotateRef,-aGeo.nSin,aGeo.nCos); // -sin=zurueckdrehen
            aTmpPt-=aPaintRect.TopLeft();
            aPaintRect.Move(aTmpPt.X(),aTmpPt.Y());
        }
    }
    Rectangle aScrollFrameRect(aPaintRect);
    if (!bBlink) {
        if (eDirection==SDRTEXTANI_LEFT || eDirection==SDRTEXTANI_RIGHT) {
            aScrollFrameRect.Left()=rAnchorRect.Left();
            aScrollFrameRect.Right()=rAnchorRect.Right();
        }
        if (eDirection==SDRTEXTANI_UP || eDirection==SDRTEXTANI_DOWN) {
            aScrollFrameRect.Top()=rAnchorRect.Top();
            aScrollFrameRect.Bottom()=rAnchorRect.Bottom();
        }
    }
    Rectangle aAnimationBoundRect(aScrollFrameRect);
    if (aGeo.nDrehWink!=0) {
        // Das aAnimationBoundRect ist das BoundRect des gedrehten aScrollFrameRect.
        // Fuer diesen Bereich muss spaeter der Hintergrund gesichert werden, ...
        Polygon aPoly(aAnimationBoundRect);
        RotatePoly(aPoly,aRotateRef,aGeo.nSin,aGeo.nCos);
        aAnimationBoundRect=aPoly.GetBoundRect();
    }
    // ClipRegion am pAnimator setzen, falls erforderlich
    if (bBlink) pAnimator->SetClipRegion(); // kein Clipping erforderlich
    else {
        if (aGeo.nDrehWink==0) pAnimator->SetClipRegion(aScrollFrameRect); // RectClipping
        else {
            Polygon aPoly(aScrollFrameRect);
            RotatePoly(aPoly,aRotateRef,aGeo.nSin,aGeo.nCos);
            pAnimator->SetClipRegion(aPoly); // PolyClipping
        }
    }
    Rectangle aAnimationBoundRectPlus(aAnimationBoundRect); // Das ist das AnimationBoundRect +PvOfs
    aAnimationBoundRectPlus.Move(rOffset.X(),rOffset.Y());

    { // Nun das Metafile erzeugen
        GDIMetaFile* pMtf=new GDIMetaFile;
        VirtualDevice aBlackHole;
        aBlackHole.EnableOutput(FALSE);
        pMtf->Record(&aBlackHole);

        //AW vertical writing
        // For the outliner::Draw method which takes a start position
        // the upper right position must be offered for vertical writing.
        // Update: MT changed the method to take the same position as normal,
        // so I changed this back.
        Point aPaintPos = rPaintRect.TopLeft();

        //aPaintPos-=aAnimationBoundRect.TopLeft();
        // Die RedLines der Online-Rechtschreibpruefung werden via DrawPixel
        // gemalt. Das ist bei Laufschrift, besonders unter OS/2, viel zu langsam.
        // Deshalb schalte ich diese hier bei der Metafileaufzeichnung temporaer ab.
        ULONG nStat0=rOutliner.GetControlWord();
        rOutliner.SetControlWord(nStat0|EE_CNTRL_NOREDLINES);
        rOutliner.Draw(&aBlackHole,aPaintPos,(short)(aGeo.nDrehWink/10));
        rOutliner.SetControlWord(nStat0);
        pMtf->Stop();
        pMtf->WindStart();
        pAnimator->SetGDIMetaFile(pMtf);
    }
    pAnimator->SetOutputRect(aAnimationBoundRect);
    pAnimator->SetScrollFrameRect(aScrollFrameRect);
    pAnimator->SetMtfFrameRect(aPaintRect);
    pAnimator->SetRotateRef(aRotateRef);
    pAnimator->SetRotateAngle(aGeo.nDrehWink);

    ImpMtfAnimationInfo* pInfo=pAnimator->Start(rOut,rOffset);
    // Nun noch die PageView setzen (fuer Xor)
    pInfo->pPageView=rInfoRec.pPV;
    return TRUE;
}

void SdrTextObj::RecalcBoundRect()
{
    aOutRect=GetSnapRect();
}

void SdrTextObj::ImpAddTextToBoundRect()
{
    if (pOutlinerParaObject!=NULL) {
        if (IsContourTextFrame()) return;
        if (IsFontwork()) {
            if (pModel!=NULL) {
                VirtualDevice aVD;
                ExtOutputDevice aXOut(&aVD);
                SdrOutliner& rOutl=ImpGetDrawOutliner();
                rOutl.SetUpdateMode(TRUE);
                ImpTextPortionHandler aTPHandler(rOutl,*this);

                aXOut.SetTextAttr(GetItemSet());

                aTPHandler.DrawTextToPath(aXOut,FALSE);
                if (pFormTextBoundRect==NULL) pFormTextBoundRect=new Rectangle;
                *pFormTextBoundRect=aTPHandler.GetFormTextBoundRect();
                aOutRect.Union(*pFormTextBoundRect);
            }
        } else { // Ansonsten Text im Zeichenobjekt zentriert
            if (pFormTextBoundRect!=NULL) {
                delete pFormTextBoundRect;
                pFormTextBoundRect=NULL;
            }
            FASTBOOL bCheckText=TRUE;
            if (bTextFrame) {
                bCheckText=GetTextLeftDistance ()<0 ||
                           GetTextRightDistance()<0 ||
                           GetTextUpperDistance()<0 ||
                           GetTextLowerDistance()<0 ||
                           (GetEckenradius()>0 && aGeo.nDrehWink!=0);
            }
            if (bCheckText) {
                SdrOutliner& rOutliner=ImpGetDrawOutliner();
                Rectangle aTextRect;
                Rectangle aAnchorRect;
                TakeTextRect(rOutliner,aTextRect,TRUE,&aAnchorRect); // EditText ignorieren!
                SdrFitToSizeType eFit=GetFitToSize();
                FASTBOOL bFitToSize=(eFit==SDRTEXTFIT_PROPORTIONAL || eFit==SDRTEXTFIT_ALLLINES);
                if (bFitToSize) aTextRect=aAnchorRect;
                rOutliner.Clear();
                if (aGeo.nDrehWink!=0) {
                    Polygon aPol(aTextRect);
                    if (aGeo.nDrehWink!=0) RotatePoly(aPol,aTextRect.TopLeft(),aGeo.nSin,aGeo.nCos);
                    aOutRect.Union(aPol.GetBoundRect());
                } else {
                    aOutRect.Union(aTextRect);
                }
            }
        }
    }
}

SdrObject* SdrTextObj::CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const
{
    if (!bTextFrame && pOutlinerParaObject==NULL) return NULL;
    if (pVisiLayer!=NULL && !pVisiLayer->IsSet(nLayerId)) return NULL;
    INT32 nMyTol=nTol;
    FASTBOOL bFontwork=IsFontwork();
    SdrFitToSizeType eFit=GetFitToSize();
    FASTBOOL bFitToSize=(eFit==SDRTEXTFIT_PROPORTIONAL || eFit==SDRTEXTFIT_ALLLINES);
    Rectangle aR(aRect);
    Rectangle aAnchor(aR);
    Rectangle aTextRect(aR);
    SdrOutliner* pOutliner = NULL;
    pOutliner = &pModel->GetHitTestOutliner();

    if (bFontwork) {
        if (pFormTextBoundRect!=NULL) aR=*pFormTextBoundRect;
        else aR=GetBoundRect();
    }
    else
    {
        TakeTextRect( *pOutliner, aTextRect, FALSE, &aAnchor, FALSE ); // EditText nicht mehr ignorieren! TRUE); // EditText ignorieren!

        if (bFitToSize)
            aR=aAnchor;
        else
            aR=aTextRect;
    }
    if (aR.GetWidth()-1>short(nTol) && aR.GetHeight()-1>short(nTol)) nMyTol=0; // Keine Toleranz noetig hier
    if (nMyTol!=0) {
        aR.Left  ()-=nMyTol;
        aR.Top   ()-=nMyTol;
        aR.Right ()+=nMyTol;
        aR.Bottom()+=nMyTol;
    }
    FASTBOOL bRet=FALSE;
    if (bFontwork) {
        bRet=aR.IsInside(rPnt);
    } else {
        if (aGeo.nDrehWink!=0) {
            Polygon aPol(aR);
            RotatePoly(aPol,aR.TopLeft(),aGeo.nSin,aGeo.nCos);
            bRet=IsPointInsidePoly(aPol,rPnt);
        } else {
            bRet=aR.IsInside(rPnt);
        }
        if (bRet) { // und nun noch checken, ob wirklich Buchstaben getroffen sind
            // Featurewunsch zur 4.0
            // Zunaechst meine Dok-Koordinaten in EE-Dok-Koordinaten umwandeln.
            Point aPt(rPnt); aPt-=aR.TopLeft();
            if (bFitToSize) { // #38214#: FitToSize berueksichtigen
                Fraction aX(aTextRect.GetWidth()-1,aAnchor.GetWidth()-1);
                Fraction aY(aTextRect.GetHeight()-1,aAnchor.GetHeight()-1);
                ResizePoint(aPt,Point(),aX,aY);
            }
            if (aGeo.nDrehWink!=0) RotatePoint(aPt,Point(),-aGeo.nSin,aGeo.nCos); // -sin fuer Unrotate
            // Und nun im EE-Dok auf Buchstabensuche gehen
            long nHitTol = 2000;
            OutputDevice* pRef = pOutliner->GetRefDevice();
            if( pRef )
                nHitTol = pRef->LogicToLogic( nHitTol, MAP_100TH_MM, pRef->GetMapMode().GetMapUnit() );

            bRet = pOutliner->IsTextPos( aPt, (sal_uInt16)nHitTol );
        }
    }

    return bRet ? (SdrObject*)this : NULL;
}

void SdrTextObj::TakeObjNameSingul(XubString& rName) const
{
    XubString aStr;

    switch(eTextKind)
    {
        case OBJ_OUTLINETEXT:
        {
            aStr = ImpGetResStr(STR_ObjNameSingulOUTLINETEXT);
            break;
        }

        case OBJ_TITLETEXT  :
        {
            aStr = ImpGetResStr(STR_ObjNameSingulTITLETEXT);
            break;
        }

        default:
        {
            if(IsLinkedText())
                aStr = ImpGetResStr(STR_ObjNameSingulTEXTLNK);
            else
                aStr = ImpGetResStr(STR_ObjNameSingulTEXT);
            break;
        }
    }

    if(pOutlinerParaObject && eTextKind != OBJ_OUTLINETEXT)
    {
        // Macht bei OUTLINETEXT wohl derzeit noch etwas Probleme
        XubString aStr2(pOutlinerParaObject->GetTextObject().GetText(0));
        aStr2.EraseLeadingChars();

        // #69446# avoid non expanded text portions in object name
        // (second condition is new)
        if(aStr2.Len() && aStr2.Search(sal_Unicode(255)) == STRING_NOTFOUND)
        {
            // #76681# space between ResStr and content text
            aStr += sal_Unicode(' ');

            aStr += sal_Unicode('\'');

            if(aStr2.Len() > 10)
            {
                aStr2.Erase(8);
                aStr2.AppendAscii("...", 3);
            }

            aStr += aStr2;
            aStr += sal_Unicode('\'');
        }
    }

    rName = aStr;

    String aName( GetName() );
    if(aName.Len())
    {
        rName += sal_Unicode(' ');
        rName += sal_Unicode('\'');
        rName += aName;
        rName += sal_Unicode('\'');
    }

}

void SdrTextObj::TakeObjNamePlural(XubString& rName) const
{
    switch (eTextKind) {
        case OBJ_OUTLINETEXT: rName=ImpGetResStr(STR_ObjNamePluralOUTLINETEXT); break;
        case OBJ_TITLETEXT  : rName=ImpGetResStr(STR_ObjNamePluralTITLETEXT);   break;
        default: {
            if (IsLinkedText()) {
                rName=ImpGetResStr(STR_ObjNamePluralTEXTLNK);
            } else {
                rName=ImpGetResStr(STR_ObjNamePluralTEXT);
            }
        } break;
    } // switch
}

void SdrTextObj::operator=(const SdrObject& rObj)
{
    SdrAttrObj::operator=(rObj);
    const SdrTextObj* pText=PTR_CAST(SdrTextObj,&rObj);
    if (pText!=NULL) {
        aRect     =pText->aRect;
        aGeo      =pText->aGeo;
        eTextKind =pText->eTextKind;
        bTextFrame=pText->bTextFrame;
        aTextSize=pText->aTextSize;
        bTextSizeDirty=pText->bTextSizeDirty;

        // #101776# Not all of the necessary parameters were copied yet.
        bNoShear = pText->bNoShear;
        bNoRotate = pText->bNoRotate;
        bNoMirror = pText->bNoMirror;
        bDisableAutoWidthOnDragging = pText->bDisableAutoWidthOnDragging;

        if (pOutlinerParaObject!=NULL) delete pOutlinerParaObject;
        if (pText->HasText()) {
            const Outliner* pEO=pText->pEdtOutl;
            if (pEO!=NULL) {
                pOutlinerParaObject=pEO->CreateParaObject();
            } else {
                pOutlinerParaObject=pText->pOutlinerParaObject->Clone();
            }
        } else {
            pOutlinerParaObject=NULL;
        }
        ImpSetTextStyleSheetListeners();
    }
}

void SdrTextObj::TakeXorPoly(XPolyPolygon& rPoly, FASTBOOL bDetail) const
{
    Polygon aPol(aRect);
    if (aGeo.nShearWink!=0) ShearPoly(aPol,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotatePoly(aPol,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    rPoly=XPolyPolygon(XPolygon(aPol));
}

void SdrTextObj::TakeContour(XPolyPolygon& rPoly) const
{
    SdrAttrObj::TakeContour(rPoly);

    // #80328# using Clone()-Paint() strategy inside TakeContour() leaves a destroyed
    // SdrObject as pointer in DrawOutliner. Set *this again in fetching the outliner
    // in every case
    SdrOutliner& rOutliner=ImpGetDrawOutliner();

    // und nun noch ggf. das BoundRect des Textes dazu
    if (pOutlinerParaObject!=NULL && !IsFontwork() && !IsContourTextFrame()) {
        Rectangle aAnchor;
        Rectangle aR;
        TakeTextRect(rOutliner,aR,FALSE,&aAnchor);
        rOutliner.Clear();
        SdrFitToSizeType eFit=GetFitToSize();
        FASTBOOL bFitToSize=(eFit==SDRTEXTFIT_PROPORTIONAL || eFit==SDRTEXTFIT_ALLLINES);
        if (bFitToSize) aR=aAnchor;
        Polygon aPol(aR);
        if (aGeo.nDrehWink!=0) RotatePoly(aPol,aR.TopLeft(),aGeo.nSin,aGeo.nCos);
        rPoly.Insert(XPolygon(aPol));
    }
}

void SdrTextObj::TakeContour(XPolyPolygon& rXPoly, SdrContourType eType) const
{
}

void SdrTextObj::RecalcSnapRect()
{
    if (aGeo.nDrehWink!=0 || aGeo.nShearWink!=0) {
        Polygon aPol(aRect);
        if (aGeo.nShearWink!=0) ShearPoly(aPol,aRect.TopLeft(),aGeo.nTan);
        if (aGeo.nDrehWink!=0) RotatePoly(aPol,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
        maSnapRect=aPol.GetBoundRect();
    } else {
        maSnapRect=aRect;
    }
}

USHORT SdrTextObj::GetSnapPointCount() const
{
    return 4;
}

Point SdrTextObj::GetSnapPoint(USHORT i) const
{
    Point aP;
    switch (i) {
        case 0: aP=aRect.TopLeft(); break;
        case 1: aP=aRect.TopRight(); break;
        case 2: aP=aRect.BottomLeft(); break;
        case 3: aP=aRect.BottomRight(); break;
        default: aP=aRect.Center(); break;
    }
    if (aGeo.nShearWink!=0) ShearPoint(aP,aRect.TopLeft(),aGeo.nTan);
    if (aGeo.nDrehWink!=0) RotatePoint(aP,aRect.TopLeft(),aGeo.nSin,aGeo.nCos);
    return aP;
}

void SdrTextObj::ImpCheckMasterCachable()
{
    bNotMasterCachable=FALSE;
    if (!bNotVisibleAsMaster && pOutlinerParaObject!=NULL && pOutlinerParaObject->IsEditDoc()) {
        const EditTextObject& rText=pOutlinerParaObject->GetTextObject();
        bNotMasterCachable=rText.HasField(SvxPageField::StaticType());
    }
}

// #101029#: Extracted from ImpGetDrawOutliner()
void SdrTextObj::ImpInitDrawOutliner( SdrOutliner& rOutl ) const
{
    rOutl.SetUpdateMode(FALSE);
    USHORT nOutlinerMode = OUTLINERMODE_OUTLINEOBJECT;
    if ( !IsOutlText() )
        nOutlinerMode = OUTLINERMODE_TEXTOBJECT;
    rOutl.Init( nOutlinerMode );

    rOutl.SetGlobalCharStretching(100,100);
    ULONG nStat=rOutl.GetControlWord();
    nStat&=~(EE_CNTRL_STRETCHING|EE_CNTRL_AUTOPAGESIZE);
    rOutl.SetControlWord(nStat);
    Size aNullSize;
    Size aMaxSize(100000,100000);
    rOutl.SetMinAutoPaperSize(aNullSize);
    rOutl.SetMaxAutoPaperSize(aMaxSize);
    rOutl.SetPaperSize(aMaxSize);
    rOutl.ClearPolygon();
}

SdrOutliner& SdrTextObj::ImpGetDrawOutliner() const
{
    SdrOutliner& rOutl=pModel->GetDrawOutliner(this);

    // #101029#: Code extracted to ImpInitDrawOutliner()
    ImpInitDrawOutliner( rOutl );

    return rOutl;
}

// #101029#: Extracted from Paint()
void SdrTextObj::ImpSetupDrawOutlinerForPaint( FASTBOOL         bContourFrame,
                                               SdrOutliner&     rOutliner,
                                               Rectangle&       rTextRect,
                                               Rectangle&       rAnchorRect,
                                               Rectangle&       rPaintRect,
                                               Fraction&        rFitXKorreg ) const
{
    if (!bContourFrame)
    {
        // FitToSize erstmal nicht mit ContourFrame
        SdrFitToSizeType eFit=GetFitToSize();
        if (eFit==SDRTEXTFIT_PROPORTIONAL || eFit==SDRTEXTFIT_ALLLINES)
        {
            ULONG nStat=rOutliner.GetControlWord();
            nStat|=EE_CNTRL_STRETCHING|EE_CNTRL_AUTOPAGESIZE;
            rOutliner.SetControlWord(nStat);
        }
    }

    TakeTextRect(rOutliner, rTextRect, FALSE, &rAnchorRect);
    rPaintRect = rTextRect;

    if (!bContourFrame)
    {
        // FitToSize erstmal nicht mit ContourFrame
        SdrFitToSizeType eFit=GetFitToSize();
        if (eFit==SDRTEXTFIT_PROPORTIONAL || eFit==SDRTEXTFIT_ALLLINES)
        {
            ImpSetCharStretching(rOutliner,rTextRect,rAnchorRect,rFitXKorreg);
            rPaintRect=rAnchorRect;
        }
    }
}

void SdrTextObj::SetupOutlinerFormatting( SdrOutliner& rOutl, Rectangle& rPaintRect ) const
{
    Rectangle aTextRect;
    Rectangle aAnchorRect;
    Fraction aFitXKorreg(1,1);

    FASTBOOL bContourFrame=IsContourTextFrame();

    ImpInitDrawOutliner( rOutl );
    ImpSetupDrawOutlinerForPaint( bContourFrame, rOutl, aTextRect, aAnchorRect, rPaintRect, aFitXKorreg );

    if( GetModel() )
    {
        MapMode aMapMode(GetModel()->GetScaleUnit(), Point(0,0),
                         GetModel()->GetScaleFraction(),
                         GetModel()->GetScaleFraction());
        rOutl.SetRefMapMode(aMapMode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// pre- and postprocessing for objects for saving

void SdrTextObj::PreSave()
{
    // call parent
    SdrAttrObj::PreSave();

    // Prepare OutlinerParaObjects for storing
    OutlinerParaObject* pParaObj = GetOutlinerParaObject();
    if(pParaObj && GetModel())
        pParaObj->PrepareStore((SfxStyleSheetPool*)GetModel()->GetStyleSheetPool());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

OutlinerParaObject* SdrTextObj::GetOutlinerParaObject() const
{
    return pOutlinerParaObject;
}

void SdrTextObj::NbcSetOutlinerParaObject(OutlinerParaObject* pTextObject)
{
    if( pModel )
    {
        // Update HitTestOutliner
        const SdrTextObj* pTestObj = pModel->GetHitTestOutliner().GetTextObj();
        if( pTestObj && pTestObj->GetOutlinerParaObject() == pOutlinerParaObject )
            pModel->GetHitTestOutliner().SetTextObj( NULL );
    }

    if (pOutlinerParaObject!=NULL) {
        delete pOutlinerParaObject;
        pOutlinerParaObject=NULL;
    }
    pOutlinerParaObject=pTextObject;

    if( pOutlinerParaObject )
    {
        ImpForceItemSet();
        mpObjectItemSet->Put( SvxWritingModeItem( pOutlinerParaObject->IsVertical() ? com::sun::star::text::WritingMode_TB_RL : com::sun::star::text::WritingMode_LR_TB ) );
    }

    SetTextSizeDirty();
    bPortionInfoChecked=FALSE;
    if (IsTextFrame() && (IsAutoGrowHeight() || IsAutoGrowWidth())) { // Textrahmen anpassen!
        NbcAdjustTextFrameWidthAndHeight();
    }
    if (!IsTextFrame()) {
        // Das SnapRect behaelt seine Groesse bei
        bBoundRectDirty=TRUE;
        SetRectsDirty(TRUE);
    }
    ImpSetTextStyleSheetListeners();
    ImpCheckMasterCachable();
}

void SdrTextObj::NbcReformatText()
{
    if (pOutlinerParaObject!=NULL) {
        bPortionInfoChecked=FALSE;
        pOutlinerParaObject->ClearPortionInfo();
        if (bTextFrame) {
            NbcAdjustTextFrameWidthAndHeight();
        } else {
            // Das SnapRect behaelt seine Groesse bei
            bBoundRectDirty=TRUE;
            SetRectsDirty(TRUE);
        }
        SetTextSizeDirty();
    }
}

void SdrTextObj::ReformatText()
{
    if (pOutlinerParaObject!=NULL) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetBoundRect();
        SendRepaintBroadcast();
        NbcReformatText();
        SetChanged();
        SendRepaintBroadcast();
        if (GetBoundRect()!=aBoundRect0) {
            SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
        }
    }
}

void SdrTextObj::RestartAnimation(SdrPageView* pPageView) const
{
    FASTBOOL bAnimated=GetTextAniKind()!=SDRTEXTANI_NONE;
    if (bAnimated) {
        ImpSdrMtfAnimator* pAnimator=((SdrTextObj*)this)->ImpGetMtfAnimator();
        if (pAnimator!=NULL) {
            if (pPageView==NULL) {
                pAnimator->Stop();
            } else {
                for (ULONG nInfoNum=pAnimator->GetInfoCount(); nInfoNum>0;) {
                    nInfoNum--;
                    ImpMtfAnimationInfo* pInfo=pAnimator->GetInfo(nInfoNum);
                    if (pInfo->pPageView==pPageView) {
                        pAnimator->RemoveInfo(nInfoNum);
                    }
                }
            }
        }
    }
}

SdrObjGeoData* SdrTextObj::NewGeoData() const
{
    return new SdrTextObjGeoData;
}

void SdrTextObj::SaveGeoData(SdrObjGeoData& rGeo) const
{
    SdrAttrObj::SaveGeoData(rGeo);
    SdrTextObjGeoData& rTGeo=(SdrTextObjGeoData&)rGeo;
    rTGeo.aRect  =aRect;
    rTGeo.aGeo   =aGeo;
}

void SdrTextObj::RestGeoData(const SdrObjGeoData& rGeo)
{ // RectsDirty wird von SdrObject gerufen
    SdrAttrObj::RestGeoData(rGeo);
    SdrTextObjGeoData& rTGeo=(SdrTextObjGeoData&)rGeo;
    aRect  =rTGeo.aRect;
    aGeo   =rTGeo.aGeo;
    SetTextSizeDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// I/O
////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrTextObj::WriteData(SvStream& rOut) const
{
    SdrAttrObj::WriteData(rOut);
    SdrDownCompat aCompat(rOut,STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
#ifdef DBG_UTIL
    aCompat.SetID("SdrTextObj");
#endif
    rOut<<BYTE(eTextKind);
    rOut<<aRect;
    rOut<<INT32(aGeo.nDrehWink);
    rOut<<INT32(aGeo.nShearWink);

    // Wird gerade editiert, also das ParaObject aus dem aktiven Editor verwenden
    // Das war frueher. Jetzt wird beim Speichern sowas aehnliches wie EndTextEdit gemacht! #43095#
    if (pEdtOutl!=NULL) {
        // #43095#
        OutlinerParaObject* pPara=GetEditOutlinerParaObject();
        // casting auf nicht-const
        ((SdrTextObj*)this)->SetOutlinerParaObject(pPara);

        // #91254# put text to object and set EmptyPresObj to FALSE
        if(pPara && IsEmptyPresObj())
            ((SdrTextObj*)this)->SetEmptyPresObj(FALSE);
    }
    OutlinerParaObject* pPara=pOutlinerParaObject;

    BOOL bOutlinerParaObjectValid=pPara!=NULL;
    rOut<<bOutlinerParaObjectValid;

    if (bOutlinerParaObjectValid)
    {
        SdrDownCompat aTextCompat(rOut,STREAM_WRITE); // Ab V11 eingepackt
#ifdef DBG_UTIL
        aTextCompat.SetID("SdrTextObj(OutlinerParaObject)");
#endif
        pPara->Store(rOut); // neues Store am Outliner ab SV303
        pPara->FinishStore();
    }

    // Ab FileVersion 10 wird das TextBoundRect gestreamt
    BOOL bFormTextBoundRectValid=pFormTextBoundRect!=NULL;
    rOut<<bFormTextBoundRectValid;
    if (bFormTextBoundRectValid) {
        rOut<<*pFormTextBoundRect;
    }
}

void SdrTextObj::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
{
    if (rIn.GetError()!=0) return;
    if (pOutlinerParaObject!=NULL) {
        delete pOutlinerParaObject;
        pOutlinerParaObject=NULL;
    }

    SdrAttrObj::ReadData(rHead,rIn);
    SdrDownCompat aCompat(rIn,STREAM_READ); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
#ifdef DBG_UTIL
    aCompat.SetID("SdrTextObj");
#endif
    BYTE nTmp;
    rIn>>nTmp;
    eTextKind=SdrObjKind(nTmp);
    rIn>>aRect;
    INT32 n32;
    rIn>>n32; aGeo.nDrehWink=n32;
    rIn>>n32; aGeo.nShearWink=n32;
    aGeo.RecalcSinCos();
    aGeo.RecalcTan();
    //rIn>>aText;
    if (rHead.GetVersion()<=5 && IsOutlText()) { // Das war bis zu diesem Zeitpunkt nicht gespeichert
        NbcSetAutoGrowHeight(FALSE);
    }

    BOOL bOutlinerParaObjectValid=FALSE;
    rIn>>bOutlinerParaObjectValid;
    if (bOutlinerParaObjectValid)
    {
        SfxItemPool* pOutlPool=pModel!=NULL ? &pModel->GetItemPool() : NULL;
        if (rHead.GetVersion()>=11) {
            SdrDownCompat aTextCompat(rIn,STREAM_READ); // ab V11 eingepackt
#ifdef DBG_UTIL
            aTextCompat.SetID("SdrTextObj(OutlinerParaObject)");
#endif
            pOutlinerParaObject=OutlinerParaObject::Create(rIn,pOutlPool);
        } else {
            pOutlinerParaObject=OutlinerParaObject::Create(rIn,pOutlPool);
        }
    }

    if( pOutlinerParaObject )
    {
        if( pOutlinerParaObject->GetOutlinerMode() == OUTLINERMODE_DONTKNOW )
        {
            if( eTextKind == OBJ_TITLETEXT )
                pOutlinerParaObject->SetOutlinerMode( OUTLINERMODE_TITLEOBJECT );
            else if( eTextKind == OBJ_OUTLINETEXT )
                pOutlinerParaObject->SetOutlinerMode( OUTLINERMODE_OUTLINEOBJECT );
            else
                pOutlinerParaObject->SetOutlinerMode( OUTLINERMODE_TEXTOBJECT );
        }

        if( pOutlinerParaObject->IsVertical() )
        {
            ImpForceItemSet();
            mpObjectItemSet->Put( SvxWritingModeItem( com::sun::star::text::WritingMode_TB_RL ) );
        }
    }

    if (rHead.GetVersion()>=10) {
        // Ab FileVersion 10 wird das TextBoundRect gestreamt
        BOOL bFormTextBoundRectValid=FALSE;
        rIn>>bFormTextBoundRectValid;
        if (bFormTextBoundRectValid) {
            if (pFormTextBoundRect==NULL) pFormTextBoundRect=new Rectangle;
            rIn>>*pFormTextBoundRect;
        }
    }

    if(rHead.GetVersion() < 12 && !bTextFrame)
    {
        mpObjectItemSet->Put(SdrTextHorzAdjustItem(SDRTEXTHORZADJUST_CENTER));
        mpObjectItemSet->Put(SdrTextVertAdjustItem(SDRTEXTVERTADJUST_CENTER));
        mpObjectItemSet->Put(SvxAdjustItem(SVX_ADJUST_CENTER));
    }

    if (bTextFrame && pOutlinerParaObject!=NULL)
        NbcAdjustTextFrameWidthAndHeight();

    if ( pOutlinerParaObject &&
         pOutlinerParaObject->GetTextObject().GetVersion() < 500 &&
         !pOutlinerParaObject->IsEditDoc() )
    {
        pOutlinerParaObject->MergeParaAttribs( GetItemSet() );
    }

    // #84529# correct gradient rotation for 5.2 and earlier
    if(aGeo.nDrehWink != 0 && rHead.GetVersion() <= 16)
    {
        XFillStyle eStyle = ((const XFillStyleItem&)GetItem(XATTR_FILLSTYLE)).GetValue();
        if(XFILL_GRADIENT == eStyle)
        {
            XFillGradientItem aItem = (XFillGradientItem&)GetItem(XATTR_FILLGRADIENT);
            XGradient aGradient = aItem.GetValue();

            // calc new angle. aGeo.nDrehWink is 1/100th degree, aGradient.GetAngle()
            // is 1/10th degree. Match this.
            sal_Int32 nNewAngle = ((aGeo.nDrehWink + (aGradient.GetAngle() * 10)) + 5) / 10;

            while(nNewAngle < 0)
                nNewAngle += 3600;

            while(nNewAngle >= 3600)
                nNewAngle -= 3600;

            // create new item and set
            aGradient.SetAngle(nNewAngle);
            aItem.SetValue(aGradient);
            SetItem(aItem);
        }
    }

    ImpSetTextStyleSheetListeners();
    SetTextSizeDirty();
    ImpCheckMasterCachable();
}

IMPL_LINK(SdrTextObj,ImpAnimationHdl,ImpSdrMtfAnimator*,pAnimator)
{
    // Aehnliche Implementation am Grafikobjekt: svdograf.cxx, SdrGrafObj

    // Wenn wir nicht mehr da sind, stoppen wir natuerlich alles
    // und kehren gleich zurueck
    if (!bInserted || pPage==NULL || pModel==NULL) {
        pAnimator->Stop();
        return 0;
    }

    // Alle Extra-Data auf 0 setzen, wenn keine andere ExtraData
    // ausser der eigenen (1) gesetzt;
    // groesser als 1 bedeutet zum. beim GrafObj, dass die Animation
    // von aussen gestartet wurde, z.B. von der DiaShow.
    ULONG nInfoNum;
    for (nInfoNum=pAnimator->GetInfoCount(); nInfoNum>0;) {
        nInfoNum--;
        ImpMtfAnimationInfo* pInfo=pAnimator->GetInfo(nInfoNum);
        if (pInfo->nExtraData==1L) pInfo->nExtraData=0L;
    }

    USHORT   nPageNum=pPage->GetPageNum();
    FASTBOOL bMaster=pPage->IsMasterPage() && !bNotVisibleAsMaster;
    USHORT   nLsAnz=pModel->GetListenerCount();

    for (USHORT nLsNum=0; nLsNum<nLsAnz; nLsNum++) {
        SfxListener* pLs=pModel->GetListener(nLsNum);
        SdrObjEditView* pView=PTR_CAST(SdrObjEditView,pLs);
        if (pView!=NULL) {
            FASTBOOL bDis=!pView->IsAnimationEnabled();
            FASTBOOL bMrk=pView->IsObjMarked(this);
            FASTBOOL bEdt=pView->GetTextEditObject()==this;
            USHORT nPvAnz=pView->GetPageViewCount();
            for (USHORT nPvNum=0; nPvNum<nPvAnz; nPvNum++) {
                SdrPageView* pPV=pView->GetPageViewPvNum(nPvNum);
                SdrPage* pPg=pPV->GetPage();
                if (pPV->GetVisibleLayers().IsSet(nLayerID)) {
                    FASTBOOL bJa=pPg==pPage;
                    if (!bJa && bMaster && !pPg->IsMasterPage()) {
                        USHORT nMasterAnz=pPg->GetMasterPageCount();
                        for  (USHORT nMasterNum=0; nMasterNum<nMasterAnz && !bJa; nMasterNum++) {
                            const SdrMasterPageDescriptor& rMPD=pPg->GetMasterPageDescriptor(nMasterNum);
                            bJa=nPageNum==rMPD.GetPageNum() && rMPD.GetVisibleLayers().IsSet(nLayerID);
                        }
                    }
                    if (bJa) {
                        USHORT nOutAnz=pView->GetWinCount();
                        for (USHORT nOutNum=0; nOutNum<nOutAnz; nOutNum++) {
                            OutputDevice* pOut=pView->GetWin(nOutNum);
                            if (pOut->GetOutDevType()==OUTDEV_WINDOW)
                            {
                                Point                   aPvOfs( pPV->GetOffset() );
                                ULONG                   nPos = pAnimator->FindInfo(*pOut,aPvOfs,0);
                                ImpMtfAnimationInfo*    pInfo = NULL;
                                const BOOL              bPause = ( bMrk || bEdt || bDis );

                                if( nPos != CONTAINER_ENTRY_NOTFOUND )
                                {
                                    pInfo = pAnimator->GetInfo(nPos);

                                    if( ( pInfo->pPageView && pInfo->pPageView != pPV ) && pInfo->nExtraData == 0L )
                                        pInfo = NULL;
                                }
                                else if( !bPause )
                                {
                                    // Falls kein Record gefunden, wird ein neuer erzeugt
                                    // Das passiert z.B., wenn das Obj auf einer MasterPage liegt
                                    // und diese mittels MasterPagePaintCache angezeigt wurde.
                                    pInfo=pAnimator->Start(*pOut,aPvOfs);
                                }

                                if( pInfo )
                                {
                                    // Flag am gefundenen bzw. neuen Objekt setzen
                                    // ( Info soll _nicht_ geloescht werden )
                                    if( pInfo->nExtraData == 0L )
                                        pInfo->nExtraData = 1L;

                                    pInfo->bPause = bPause;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Alle Objekte mit nicht gesetztem Flag loeschen
    for (nInfoNum=pAnimator->GetInfoCount(); nInfoNum>0;) {
        nInfoNum--;
        ImpMtfAnimationInfo* pInfo=pAnimator->GetInfo(nInfoNum);
        if (pInfo->nExtraData==0L) {
            pAnimator->RemoveInfo(nInfoNum);
        }
        if (pInfo->nExtraData==1L) pInfo->nExtraData=0L;
    }
    return 0;
}

void SdrTextObj::SetTextAnimationSupervisor( OutputDevice* pDisplayDev, BOOL bObjSupervises )
{
    ImpSdrMtfAnimator* pAnimator = ImpGetMtfAnimator();

    if( GetTextAniKind() != SDRTEXTANI_NONE && pAnimator )
    {
        for( ULONG nInfoNum = pAnimator->GetInfoCount(); nInfoNum > 0; )
        {
            ImpMtfAnimationInfo* pInfo = pAnimator->GetInfo( --nInfoNum );

            if( pInfo->pOutDev == pDisplayDev )
            {
                pInfo->nExtraData = ( bObjSupervises ? 1L : (long) this );

                if( !bObjSupervises )
                    pInfo->bPause = FALSE;
            }
        }
    }
}

SdrFitToSizeType SdrTextObj::GetFitToSize() const
{
    SdrFitToSizeType eType = SDRTEXTFIT_NONE;

    if(!IsAutoGrowWidth())
        eType = ((SdrTextFitToSizeTypeItem&)(GetItem(SDRATTR_TEXT_FITTOSIZE))).GetValue();

    return eType;
}

void SdrTextObj::ForceOutlinerParaObject()
{
    if( pOutlinerParaObject == NULL )
    {
        USHORT nOutlMode = OUTLINERMODE_TEXTOBJECT;
        if( IsTextFrame() && eTextKind == OBJ_OUTLINETEXT )
            nOutlMode = OUTLINERMODE_OUTLINEOBJECT;

        Outliner* pOutliner = SdrMakeOutliner( nOutlMode, pModel );
        if( pOutliner )
        {
            Outliner& aDrawOutliner = pModel->GetDrawOutliner();
            pOutliner->SetCalcFieldValueHdl( aDrawOutliner.GetCalcFieldValueHdl() );

            pOutliner->SetStyleSheet( 0, GetStyleSheet());
            OutlinerParaObject* pOutlinerParaObject = pOutliner->CreateParaObject();
            SetOutlinerParaObject( pOutlinerParaObject );

            delete pOutliner;
        }
    }
}

BOOL SdrTextObj::IsVerticalWriting() const
{
    // #89459#
    if(pOutlinerParaObject)
        return pOutlinerParaObject->IsVertical();
    if(pEdtOutl)
        return pEdtOutl->IsVertical();
    return FALSE;
}

void SdrTextObj::SetVerticalWriting( BOOL bVertical )
{
    ForceOutlinerParaObject();

    DBG_ASSERT( pOutlinerParaObject, "SdrTextObj::SetVerticalWriting() without OutlinerParaObject!" );
    if( pOutlinerParaObject )
    {
        if(pOutlinerParaObject->IsVertical() != bVertical)
        {
            // get item settings
            const SfxItemSet& rSet = GetItemSet();
            sal_Bool bAutoGrowWidth = ((SdrTextAutoGrowWidthItem&)rSet.Get(SDRATTR_TEXT_AUTOGROWWIDTH)).GetValue();
            sal_Bool bAutoGrowHeight = ((SdrTextAutoGrowHeightItem&)rSet.Get(SDRATTR_TEXT_AUTOGROWHEIGHT)).GetValue();

            // rescue object size
            Rectangle aObjectRect = GetSnapRect();

            // prepare ItemSet to set exchanged width and height items
            SfxItemSet aNewSet(*rSet.GetPool(),
                SDRATTR_TEXT_AUTOGROWHEIGHT, SDRATTR_TEXT_AUTOGROWHEIGHT,
                SDRATTR_TEXT_AUTOGROWWIDTH, SDRATTR_TEXT_AUTOGROWWIDTH,
                0, 0);
            aNewSet.Put(rSet);
            aNewSet.Put(SdrTextAutoGrowWidthItem(bAutoGrowHeight));
            aNewSet.Put(SdrTextAutoGrowHeightItem(bAutoGrowWidth));
            SetItemSet(aNewSet);

            // set ParaObject orientation accordingly
            pOutlinerParaObject->SetVertical(bVertical);

            // restore object size
            SetSnapRect(aObjectRect);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// transformation interface for StarOfficeAPI. This implements support for
// homogen 3x3 matrices containing the transformation of the SdrObject. At the
// moment it contains a shearX, rotation and translation, but for setting all linear
// transforms like Scale, ShearX, ShearY, Rotate and Translate are supported.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
// gets base transformation and rectangle of object. If it's an SdrPathObj it fills the PolyPolygon
// with the base geometry and returns TRUE. Otherwise it returns FALSE.
BOOL SdrTextObj::TRGetBaseGeometry(Matrix3D& rMat, XPolyPolygon& rPolyPolygon) const
{
    // get turn and shear
    double fRotate = (aGeo.nDrehWink / 100.0) * F_PI180;
    double fShear = (aGeo.nShearWink / 100.0) * F_PI180;

    // get aRect, this is the unrotated snaprect
    Rectangle aRectangle(aRect);

    // fill other values
    Vector2D aScale((double)aRectangle.GetWidth(), (double)aRectangle.GetHeight());
    Vector2D aTranslate((double)aRectangle.Left(), (double)aRectangle.Top());

    // position maybe relative to anchorpos, convert
    if( pModel->IsWriter() )
    {
        if(GetAnchorPos().X() != 0 || GetAnchorPos().Y() != 0)
            aTranslate -= Vector2D(GetAnchorPos().X(), GetAnchorPos().Y());
    }

    // force MapUnit to 100th mm
    SfxMapUnit eMapUnit = pModel->GetItemPool().GetMetric(0);
    if(eMapUnit != SFX_MAPUNIT_100TH_MM)
    {
        switch(eMapUnit)
        {
            case SFX_MAPUNIT_TWIP :
            {
                // position
                aTranslate.X() = TWIPS_TO_MM(aTranslate.X());
                aTranslate.Y() = TWIPS_TO_MM(aTranslate.Y());

                // size
                aScale.X() = TWIPS_TO_MM(aScale.X());
                aScale.Y() = TWIPS_TO_MM(aScale.Y());

                break;
            }
            default:
            {
                DBG_ERROR("TRGetBaseGeometry: Missing unit translation to 100th mm!");
            }
        }
    }

    // build matrix
    rMat.Identity();
    if(aScale.X() != 1.0 || aScale.Y() != 1.0)
        rMat.Scale(aScale.X(), aScale.Y());
    if(fShear != 0.0)
        rMat.ShearX(tan(fShear));
    if(fRotate != 0.0)
        rMat.Rotate(fRotate);
    if(aTranslate.X() != 0.0 || aTranslate.Y() != 0.0)
        rMat.Translate(aTranslate.X(), aTranslate.Y());

    return FALSE;
}

// sets the base geometry of the object using infos contained in the homogen 3x3 matrix.
// If it's an SdrPathObj it will use the provided geometry information. The Polygon has
// to use (0,0) as upper left and will be scaled to the given size in the matrix.
void SdrTextObj::TRSetBaseGeometry(const Matrix3D& rMat, const XPolyPolygon& rPolyPolygon)
{
    // break up matrix
    Vector2D aScale, aTranslate;
    double fShear, fRotate;
    rMat.DecomposeAndCorrect(aScale, fShear, fRotate, aTranslate);

    // reset object shear and rotations
    aGeo.nDrehWink = 0;
    aGeo.RecalcSinCos();
    aGeo.nShearWink = 0;
    aGeo.RecalcTan();

    // force metric to pool metric
    SfxMapUnit eMapUnit = pModel->GetItemPool().GetMetric(0);
    if(eMapUnit != SFX_MAPUNIT_100TH_MM)
    {
        switch(eMapUnit)
        {
            case SFX_MAPUNIT_TWIP :
            {
                // position
                aTranslate.X() = MM_TO_TWIPS(aTranslate.X());
                aTranslate.Y() = MM_TO_TWIPS(aTranslate.Y());

                // size
                aScale.X() = MM_TO_TWIPS(aScale.X());
                aScale.Y() = MM_TO_TWIPS(aScale.Y());

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
        if(GetAnchorPos().X() != 0 || GetAnchorPos().Y() != 0)
            aTranslate -= Vector2D(GetAnchorPos().X(), GetAnchorPos().Y());
    }

    // build and set BaseRect (use scale)
    Point aPoint = Point();
    Size  aSize(FRound(aScale.X()), FRound(aScale.Y()));
    Rectangle aBaseRect(aPoint, aSize);
    SetSnapRect(aBaseRect);

    // shear?
    if(fShear != 0.0)
    {
        GeoStat aGeoStat;
        aGeoStat.nShearWink = FRound((atan(fShear) / F_PI180) * 100.0);
        aGeoStat.RecalcTan();
        Shear(Point(), aGeoStat.nShearWink, aGeoStat.nTan, FALSE);
    }

    // rotation?
    if(fRotate != 0.0)
    {
        GeoStat aGeoStat;
        aGeoStat.nDrehWink = FRound((fRotate / F_PI180) * 100.0);
        aGeoStat.RecalcSinCos();
        Rotate(Point(), aGeoStat.nDrehWink, aGeoStat.nSin, aGeoStat.nCos);
    }

    // translate?
    if(aTranslate.X() != 0.0 || aTranslate.Y() != 0.0)
    {
        Move(Size(
            (sal_Int32)FRound(aTranslate.X()),
            (sal_Int32)FRound(aTranslate.Y())));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Konzept des TextObjekts:
// ~~~~~~~~~~~~~~~~~~~~~~~~
// Attribute/Varianten:
// - BOOL Textrahmen / beschriftetes Zeichenobjekt
// - BOOL FontWork                 (wenn nicht Textrahmen und nicht ContourTextFrame)
// - BOOL ContourTextFrame         (wenn nicht Textrahmen und nicht Fontwork)
// - long Drehwinkel               (wenn nicht FontWork)
// - long Textrahmenabstaende      (wenn nicht FontWork)
// - BOOL FitToSize                (wenn nicht FontWork)
// - BOOL AutoGrowingWidth/Height  (wenn nicht FitToSize und nicht FontWork)
// - long Min/MaxFrameWidth/Height (wenn AutoGrowingWidth/Height)
// - enum Horizontale Textverankerung Links,Mitte,Rechts,Block,Stretch(ni)
// - enum Vertikale Textverankerung Oben,Mitte,Unten,Block,Stretch(ni)
// - enum Laufschrift              (wenn nicht FontWork)
//
// Jedes abgeleitete Objekt ist entweder ein Textrahmen (bTextFrame=TRUE)
// oder ein beschriftetes Zeichenobjekt (bTextFrame=FALSE).
//
// Defaultverankerung von Textrahmen:
//   SDRTEXTHORZADJUST_BLOCK, SDRTEXTVERTADJUST_TOP
//   = statische Pooldefaults
// Defaultverankerung von beschrifteten Zeichenobjekten:
//   SDRTEXTHORZADJUST_CENTER, SDRTEXTVERTADJUST_CENTER
//   durch harte Attributierung von SdrAttrObj
//
// Jedes vom SdrTextObj abgeleitete Objekt muss ein "UnrotatedSnapRect"
// (->TakeUnrotatedSnapRect()) liefern (Drehreferenz ist TopLeft dieses
// Rechtecks (aGeo.nDrehWink)), welches die Grundlage der Textverankerung
// bildet. Von diesem werden dann ringsum die Textrahmenabstaende abgezogen;
// das Ergebnis ist der Ankerbereich (->TakeTextAnchorRect()). Innerhalb
// dieses Bereichs wird dann in Abhaengigkeit von der horizontalen und
// vertikalen Ausrichtung (SdrTextVertAdjust,SdrTextHorzAdjust) der Ankerpunkt
// sowie der Ausgabebereich bestimmt. Bei beschrifteten Grafikobjekten kann
// der Ausgabebereich durchaus groesser als der Ankerbereich werden, bei
// Textrahmen ist er stets kleiner oder gleich (ausser bei negativen Textrahmen-
// abstaenden).
//
// FitToSize hat Prioritaet vor Textverankerung und AutoGrowHeight/Width. Der
// Ausgabebereich ist bei FitToSize immer genau der Ankerbereich. Weiterhin
// gibt es bei FitToSize keinen automatischen Zeilenumbruch.
//
// ContourTextFrame:
// - long Drehwinkel
// - long Textrahmenabstaende         spaeter vielleicht
// - BOOL FitToSize                   spaeter vielleicht
// - BOOL AutoGrowingWidth/Height     viel spaeter vielleicht
// - long Min/MaxFrameWidth/Height    viel spaeter vielleicht
// - enum Horizontale Textverankerung spaeter vielleicht, erstmal Links, Absatz zentr.
// - enum Vertikale Textverankerung   spaeter vielleicht, erstmal oben
// - enum Laufschrift                 spaeter vielleicht (evtl. sogar mit korrektem Clipping)
//
// Bei Aenderungen zu beachten:
// - Paint
// - HitTest
// - RecalcBoundRect
// - ConvertToPoly
// - Edit
// - Drucken,Speichern, Paint in Nachbarview waerend Edit
// - ModelChanged (z.B. durch NachbarView oder Lineale) waerend Edit
// - FillColorChanged waerend Edit
// - uvm...
//
/////////////////////////////////////////////////////////////////////////////////////////////////

