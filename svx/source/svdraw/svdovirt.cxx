/*************************************************************************
 *
 *  $RCSfile: svdovirt.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2003-11-24 16:59:57 $
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

#include "svdovirt.hxx"
#include "xpool.hxx"
#include "svdxout.hxx"
#include "svdtrans.hxx"
#include "svdio.hxx"
#include "svdetc.hxx"
#include "svdhdl.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////

sdr::properties::BaseProperties& SdrVirtObj::GetProperties() const
{
    return rRefObj.GetProperties();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEINIT1(SdrVirtObj,SdrObject);

SdrVirtObj::SdrVirtObj(SdrObject& rNewObj):
    rRefObj(rNewObj)
{
    bVirtObj=TRUE; // Ja, ich bin ein virtuelles Objekt
    rRefObj.AddReference(*this);
    bClosedObj=rRefObj.IsClosedObj();
}

SdrVirtObj::SdrVirtObj(SdrObject& rNewObj, const Point& rAnchorPos):
    rRefObj(rNewObj)
{
    aAnchor=rAnchorPos;
    bVirtObj=TRUE; // Ja, ich bin ein virtuelles Objekt
    rRefObj.AddReference(*this);
    bClosedObj=rRefObj.IsClosedObj();
}

SdrVirtObj::~SdrVirtObj()
{
    rRefObj.DelReference(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const SdrObject& SdrVirtObj::GetReferencedObj() const
{
    return rRefObj;
}

SdrObject& SdrVirtObj::ReferencedObj()
{
    return rRefObj;
}

void __EXPORT SdrVirtObj::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
{
    bClosedObj=rRefObj.IsClosedObj();
    SetRectsDirty(); // hier noch Optimieren.

    // Only a repaint here, rRefObj may have changed and broadcasts
    ActionChanged();
    // BroadcastObjectChange();
}

void SdrVirtObj::NbcSetAnchorPos(const Point& rAnchorPos)
{
    aAnchor=rAnchorPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrVirtObj::SetModel(SdrModel* pNewModel)
{
    SdrObject::SetModel(pNewModel);
    rRefObj.SetModel(pNewModel);
}

void SdrVirtObj::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
{
    rRefObj.TakeObjInfo(rInfo);
}

UINT32 SdrVirtObj::GetObjInventor() const
{
    return rRefObj.GetObjInventor();
}

UINT16 SdrVirtObj::GetObjIdentifier() const
{
    return rRefObj.GetObjIdentifier();
}

SdrObjList* SdrVirtObj::GetSubList() const
{
    return rRefObj.GetSubList();
}

const Rectangle& SdrVirtObj::GetCurrentBoundRect() const
{
    ((SdrVirtObj*)this)->aOutRect=rRefObj.GetCurrentBoundRect(); // Hier noch optimieren
    ((SdrVirtObj*)this)->aOutRect+=aAnchor;
    return aOutRect;
}

const Rectangle& SdrVirtObj::GetLastBoundRect() const
{
    ((SdrVirtObj*)this)->aOutRect=rRefObj.GetLastBoundRect(); // Hier noch optimieren
    ((SdrVirtObj*)this)->aOutRect+=aAnchor;
    return aOutRect;
}

void SdrVirtObj::RecalcBoundRect()
{
    aOutRect=rRefObj.GetCurrentBoundRect();
    aOutRect+=aAnchor;
}

void SdrVirtObj::SetChanged()
{
    SdrObject::SetChanged();
}

sal_Bool SdrVirtObj::DoPaintObject(ExtOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec) const
{
    Point aOfs(rOut.GetOffset());
    rOut.SetOffset(aOfs+aAnchor);
    sal_Bool bRet(rRefObj.DoPaintObject(rOut, rInfoRec));
    rOut.SetOffset(aOfs);
    return bRet;
}

SdrObject* SdrVirtObj::CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const
{
    Point aPnt(rPnt-aAnchor);
    FASTBOOL bRet=rRefObj.CheckHit(aPnt,nTol,pVisiLayer)!=NULL;
    return bRet ? (SdrObject*)this : NULL;
}

SdrObject* SdrVirtObj::Clone() const
{
    SdrObject* pObj=new SdrVirtObj(((SdrVirtObj*)this)->rRefObj); // Nur eine weitere Referenz
    return pObj;
}

void SdrVirtObj::operator=(const SdrObject& rObj)
{   // ???anderes Objekt referenzieren???
    SdrObject::operator=(rObj);
    aAnchor=((SdrVirtObj&)rObj).aAnchor;
}

void SdrVirtObj::TakeObjNameSingul(XubString& rName) const
{
    rRefObj.TakeObjNameSingul(rName);
    rName.Insert(sal_Unicode('['), 0);
    rName += sal_Unicode(']');

    String aName( GetName() );
    if(aName.Len())
    {
        rName += sal_Unicode(' ');
        rName += sal_Unicode('\'');
        rName += aName;
        rName += sal_Unicode('\'');
    }
}

void SdrVirtObj::TakeObjNamePlural(XubString& rName) const
{
    rRefObj.TakeObjNamePlural(rName);
    rName.Insert(sal_Unicode('['), 0);
    rName += sal_Unicode(']');
}

void operator +=(PolyPolygon& rPoly, const Point& rOfs)
{
    if (rOfs.X()!=0 || rOfs.Y()!=0) {
        USHORT i,j;
        for (j=0; j<rPoly.Count(); j++) {
            Polygon aP1(rPoly.GetObject(j));
            for (i=0; i<aP1.GetSize(); i++) {
                 aP1[i]+=rOfs;
            }
            rPoly.Replace(aP1,j);
        }
    }
}

void SdrVirtObj::TakeXorPoly(XPolyPolygon& rPoly, FASTBOOL bDetail) const
{
    rRefObj.TakeXorPoly(rPoly,bDetail);
    rPoly.Move(aAnchor.X(),aAnchor.Y());
}

//#110094#-12
//void SdrVirtObj::TakeContour(XPolyPolygon& rXPoly, SdrContourType eType) const
//{
//}

////////////////////////////////////////////////////////////////////////////////////////////////////

USHORT SdrVirtObj::GetHdlCount() const
{
    return rRefObj.GetHdlCount();
}

SdrHdl* SdrVirtObj::GetHdl(USHORT nHdlNum) const
{
    SdrHdl* pHdl=rRefObj.GetHdl(nHdlNum);
    Point aP(pHdl->GetPos()+aAnchor);
    pHdl->SetPos(aP);
    return pHdl;
}

USHORT SdrVirtObj::GetPlusHdlCount(const SdrHdl& rHdl) const
{
    return rRefObj.GetPlusHdlCount(rHdl);
}

SdrHdl* SdrVirtObj::GetPlusHdl(const SdrHdl& rHdl, USHORT nPlNum) const
{
    SdrHdl* pHdl=rRefObj.GetPlusHdl(rHdl,nPlNum);
    pHdl->SetPos(pHdl->GetPos()+aAnchor);
    return pHdl;
}

void SdrVirtObj::AddToHdlList(SdrHdlList& rHdlList) const
{
    SdrObject::AddToHdlList(rHdlList);
}

FASTBOOL SdrVirtObj::HasSpecialDrag() const
{
    return rRefObj.HasSpecialDrag();
}

FASTBOOL SdrVirtObj::BegDrag(SdrDragStat& rDrag) const
{
    return rRefObj.BegDrag(rDrag);
}

FASTBOOL SdrVirtObj::MovDrag(SdrDragStat& rDrag) const
{
    return rRefObj.MovDrag(rDrag);
}

FASTBOOL SdrVirtObj::EndDrag(SdrDragStat& rDrag)
{
    return rRefObj.EndDrag(rDrag);
}

void SdrVirtObj::BrkDrag(SdrDragStat& rDrag) const
{
    rRefObj.BrkDrag(rDrag);
}

void SdrVirtObj::TakeDragPoly(const SdrDragStat& rDrag, XPolyPolygon& rXPP) const
{
    rRefObj.TakeDragPoly(rDrag,rXPP);
    // Offset handlen !!!!!! fehlt noch !!!!!!!
}

XubString SdrVirtObj::GetDragComment(const SdrDragStat& rDrag, FASTBOOL bUndoDragComment, FASTBOOL bCreateComment) const
{
    return rRefObj.GetDragComment(rDrag,bUndoDragComment,bCreateComment);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FASTBOOL SdrVirtObj::BegCreate(SdrDragStat& rStat)
{
    return rRefObj.BegCreate(rStat);
}

FASTBOOL SdrVirtObj::MovCreate(SdrDragStat& rStat)
{
    return rRefObj.MovCreate(rStat);
}

FASTBOOL SdrVirtObj::EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd)
{
    return rRefObj.EndCreate(rStat,eCmd);
}

FASTBOOL SdrVirtObj::BckCreate(SdrDragStat& rStat)
{
    return rRefObj.BckCreate(rStat);
}

void SdrVirtObj::BrkCreate(SdrDragStat& rStat)
{
    rRefObj.BrkCreate(rStat);
}

void SdrVirtObj::TakeCreatePoly(const SdrDragStat& rDrag, XPolyPolygon& rXPP) const
{
    rRefObj.TakeCreatePoly(rDrag,rXPP);
    // Offset handlen !!!!!! fehlt noch !!!!!!!
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrVirtObj::NbcMove(const Size& rSiz)
{
    MovePoint(aAnchor,rSiz);
    SetRectsDirty();
}

void SdrVirtObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    rRefObj.NbcResize(rRef-aAnchor,xFact,yFact);
    SetRectsDirty();
}

void SdrVirtObj::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
{
    rRefObj.NbcRotate(rRef-aAnchor,nWink,sn,cs);
    SetRectsDirty();
}

void SdrVirtObj::NbcMirror(const Point& rRef1, const Point& rRef2)
{
    rRefObj.NbcMirror(rRef1-aAnchor,rRef2-aAnchor);
    SetRectsDirty();
}

void SdrVirtObj::NbcShear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    rRefObj.NbcShear(rRef-aAnchor,nWink,tn,bVShear);
    SetRectsDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrVirtObj::Move(const Size& rSiz)
{
    if (rSiz.Width()!=0 || rSiz.Height()!=0) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        // #110094#-14 SendRepaintBroadcast();
        NbcMove(rSiz);
        SetChanged();
        BroadcastObjectChange();
        SendUserCall(SDRUSERCALL_MOVEONLY,aBoundRect0);
    }
}

void SdrVirtObj::Resize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    if (xFact.GetNumerator()!=xFact.GetDenominator() || yFact.GetNumerator()!=yFact.GetDenominator()) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        rRefObj.Resize(rRef-aAnchor,xFact,yFact);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
    }
}

void SdrVirtObj::Rotate(const Point& rRef, long nWink, double sn, double cs)
{
    if (nWink!=0) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        rRefObj.Rotate(rRef-aAnchor,nWink,sn,cs);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
    }
}

void SdrVirtObj::Mirror(const Point& rRef1, const Point& rRef2)
{
    Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
    rRefObj.Mirror(rRef1-aAnchor,rRef2-aAnchor);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
}

void SdrVirtObj::Shear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    if (nWink!=0) {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        rRefObj.Shear(rRef-aAnchor,nWink,tn,bVShear);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrVirtObj::RecalcSnapRect()
{
    aSnapRect=rRefObj.GetSnapRect();
    aSnapRect+=aAnchor;
}

const Rectangle& SdrVirtObj::GetSnapRect() const
{
    ((SdrVirtObj*)this)->aSnapRect=rRefObj.GetSnapRect();
    ((SdrVirtObj*)this)->aSnapRect+=aAnchor;
    return aSnapRect;
}

void SdrVirtObj::SetSnapRect(const Rectangle& rRect)
{
    {
        Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
        Rectangle aR(rRect);
        aR-=aAnchor;
        rRefObj.SetSnapRect(aR);
        SetRectsDirty();
        SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
    }
}

void SdrVirtObj::NbcSetSnapRect(const Rectangle& rRect)
{
    Rectangle aR(rRect);
    aR-=aAnchor;
    SetRectsDirty();
    rRefObj.NbcSetSnapRect(aR);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const Rectangle& SdrVirtObj::GetLogicRect() const
{
    ((SdrVirtObj*)this)->aSnapRect=rRefObj.GetLogicRect();  // !!! Missbrauch von aSnapRect !!!
    ((SdrVirtObj*)this)->aSnapRect+=aAnchor;                // Wenns mal Aerger gibt, muss ein weiteres Rectangle Member her (oder Heap)
    return aSnapRect;
}

void SdrVirtObj::SetLogicRect(const Rectangle& rRect)
{
    Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
    Rectangle aR(rRect);
    aR-=aAnchor;
    rRefObj.SetLogicRect(aR);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
}

void SdrVirtObj::NbcSetLogicRect(const Rectangle& rRect)
{
    Rectangle aR(rRect);
    aR-=aAnchor;
    SetRectsDirty();
    rRefObj.NbcSetLogicRect(aR);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

long SdrVirtObj::GetRotateAngle() const
{
    return rRefObj.GetRotateAngle();
}

long SdrVirtObj::GetShearAngle(FASTBOOL bVertical) const
{
    return rRefObj.GetShearAngle(bVertical);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

USHORT SdrVirtObj::GetSnapPointCount() const
{
    return rRefObj.GetSnapPointCount();
}

Point SdrVirtObj::GetSnapPoint(USHORT i) const
{
    Point aP(rRefObj.GetSnapPoint(i));
    aP+=aAnchor;
    return aP;
}

FASTBOOL SdrVirtObj::IsPolyObj() const
{
    return rRefObj.IsPolyObj();
}

USHORT SdrVirtObj::GetPointCount() const
{
    return rRefObj.GetPointCount();
}

const Point& SdrVirtObj::GetPoint(USHORT i) const
{
    ((SdrVirtObj*)this)->aHack=rRefObj.GetPoint(i);
    ((SdrVirtObj*)this)->aHack+=aAnchor;
    return aHack;
}

void SdrVirtObj::NbcSetPoint(const Point& rPnt, USHORT i)
{
    Point aP(rPnt);
    aP-=aAnchor;
    rRefObj.SetPoint(aP,i);
    SetRectsDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrObjGeoData* SdrVirtObj::NewGeoData() const
{
    return rRefObj.NewGeoData();
}

void SdrVirtObj::SaveGeoData(SdrObjGeoData& rGeo) const
{
    rRefObj.SaveGeoData(rGeo);
}

void SdrVirtObj::RestGeoData(const SdrObjGeoData& rGeo)
{
    rRefObj.RestGeoData(rGeo);
    SetRectsDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrObjGeoData* SdrVirtObj::GetGeoData() const
{
    return rRefObj.GetGeoData();
}

void SdrVirtObj::SetGeoData(const SdrObjGeoData& rGeo)
{
    Rectangle aBoundRect0; if (pUserCall!=NULL) aBoundRect0=GetLastBoundRect();
    rRefObj.SetGeoData(rGeo);
    SetRectsDirty();
    SendUserCall(SDRUSERCALL_RESIZE,aBoundRect0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrVirtObj::NbcReformatText()
{
    rRefObj.NbcReformatText();
}

void SdrVirtObj::ReformatText()
{
    rRefObj.ReformatText();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FASTBOOL SdrVirtObj::HasMacro() const
{
    return rRefObj.HasMacro();
}

SdrObject* SdrVirtObj::CheckMacroHit(const SdrObjMacroHitRec& rRec) const
{
    return rRefObj.CheckMacroHit(rRec); // Todo: Positionsversatz
}

Pointer SdrVirtObj::GetMacroPointer(const SdrObjMacroHitRec& rRec) const
{
    return rRefObj.GetMacroPointer(rRec); // Todo: Positionsversatz
}

void SdrVirtObj::PaintMacro(ExtOutputDevice& rXOut, const Rectangle& rDirtyRect, const SdrObjMacroHitRec& rRec) const
{
    rRefObj.PaintMacro(rXOut,rDirtyRect,rRec); // Todo: Positionsversatz
}

FASTBOOL SdrVirtObj::DoMacro(const SdrObjMacroHitRec& rRec)
{
    return rRefObj.DoMacro(rRec); // Todo: Positionsversatz
}

XubString SdrVirtObj::GetMacroPopupComment(const SdrObjMacroHitRec& rRec) const
{
    return rRefObj.GetMacroPopupComment(rRec); // Todo: Positionsversatz
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrVirtObj::WriteData(SvStream& rOut) const
{
    SdrDownCompat aCompat(rOut,STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
#ifdef DBG_UTIL
    aCompat.SetID("SdrVirtObj");
#endif
    // fehlende Implementation
    rOut<<aAnchor;
}

void SdrVirtObj::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
{
    if (rIn.GetError()!=0) return;
    if (rHead.GetVersion()>=4) {
        SdrDownCompat aCompat(rIn,STREAM_READ); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
#ifdef DBG_UTIL
        aCompat.SetID("SdrVirtObj");
#endif
        // fehlende Implementation
        rIn>>aAnchor;
    } else {
        rIn>>aAnchor;
    }
}

void SdrVirtObj::AfterRead()
{
    // fehlende Implementation
}

const Point SdrVirtObj::GetOffset() const
{
    return Point(0,0);
}
