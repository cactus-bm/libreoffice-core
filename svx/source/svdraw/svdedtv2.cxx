/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdedtv2.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 16:36:05 $
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

#include "svdedtv.hxx"
#include "outliner.hxx"
#include "svdundo.hxx"
#include "svdogrp.hxx"   // fuer's Gruppieren
#include "svdovirt.hxx"  // fuer VirtualObject-Bundling (Writer)
#include "svdopath.hxx"  // fuer CombineObjects
#include "svdpage.hxx"
#include "svdpagv.hxx"
#include "svditer.hxx"
#include "svdograf.hxx"  // fuer Possibilities
#include "svdoole2.hxx"  // und Mtf-Import
#include "svdstr.hrc"   // Namen aus der Resource
#include "svdglob.hxx"  // StringCache
#include "svdfmtf.hxx"
#include "svdetc.hxx"

#ifndef _BASEDLGS_HXX
#include <sfx2/basedlgs.hxx>
#endif

//CHINA001 #include "dstribut.hxx"

#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif

#ifndef _OUTLOBJ_HXX //autogen
#include <outlobj.hxx>
#endif

#ifndef _EEITEM_HXX
#include "eeitem.hxx"
#endif

#ifndef _POLY3D_HXX
#include "poly3d.hxx"
#endif

#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif

#ifndef _BGFX_POLYPOLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#endif

#include "svxdlg.hxx" //CHINA001
#include <dialogs.hrc> //CHINA001

// #i37011#
#ifndef _SVDOASHP_HXX
#include "svdoashp.hxx"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@ @@@@@  @@ @@@@@@  @@ @@ @@ @@@@@ @@   @@
//  @@    @@  @@ @@   @@    @@ @@ @@ @@    @@   @@
//  @@    @@  @@ @@   @@    @@ @@ @@ @@    @@ @ @@
//  @@@@  @@  @@ @@   @@    @@@@@ @@ @@@@  @@@@@@@
//  @@    @@  @@ @@   @@     @@@  @@ @@    @@@@@@@
//  @@    @@  @@ @@   @@     @@@  @@ @@    @@@ @@@
//  @@@@@ @@@@@  @@   @@      @   @@ @@@@@ @@   @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrEditView::ImpBundleVirtObjOfMarkList()
{
  // ... fehlende Implementation
}

SdrObject* SdrEditView::GetMaxToTopObj(SdrObject* /*pObj*/) const
{
  return NULL;
}

SdrObject* SdrEditView::GetMaxToBtmObj(SdrObject* /*pObj*/) const
{
  return NULL;
}

void SdrEditView::ObjOrderChanged(SdrObject* /*pObj*/, ULONG /*nOldPos*/, ULONG /*nNewPos*/)
{
}

void SdrEditView::MovMarkedToTop()
{
    ULONG nAnz=GetMarkedObjectCount();
    if (nAnz!=0) {
        BegUndo(ImpGetResStr(STR_EditMovToTop),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_MOVTOTOP);
        SortMarkedObjects();
        ULONG nm;
        for (nm=0; nm<nAnz; nm++) { // Ordnums muessen alle stimmen!
            GetMarkedObjectByIndex(nm)->GetOrdNum();
        }
        // --> OD 2004-08-24 #110810#
//      BOOL bNeedBundle=FALSE;
        // <--
        BOOL bChg=FALSE;
        SdrObjList* pOL0=NULL;
        ULONG nNewPos=0;
        for (nm=nAnz; nm>0;) {
            nm--;
            SdrMark* pM=GetSdrMarkByIndex(nm);
            SdrObject* pObj=pM->GetObj();
            SdrObjList* pOL=pObj->GetObjList();
            if (pOL!=pOL0) {
                nNewPos=ULONG(pOL->GetObjCount()-1);
                pOL0=pOL;
            }
            ULONG nNowPos=pObj->GetOrdNumDirect();
            const Rectangle& rBR=pObj->GetCurrentBoundRect();
            ULONG nCmpPos=nNowPos+1;
            SdrObject* pMaxObj=GetMaxToTopObj(pObj);
            if (pMaxObj!=NULL) {
                ULONG nMaxPos=pMaxObj->GetOrdNum();
                if (nMaxPos!=0) nMaxPos--;
                if (nNewPos>nMaxPos) nNewPos=nMaxPos; // diesen nicht ueberholen.
                if (nNewPos<nNowPos) nNewPos=nNowPos; // aber dabei auch nicht in die falsche Richtung schieben
            }
            BOOL bEnd=FALSE;
            while (nCmpPos<nNewPos && !bEnd) {
                SdrObject* pCmpObj=pOL->GetObj(nCmpPos);
                if (pCmpObj==NULL) {
                    DBG_ERROR("MovMarkedToTop(): Vergleichsobjekt nicht gefunden");
                    bEnd=TRUE;
                } else if (pCmpObj==pMaxObj) {
                    nNewPos=nCmpPos;
                    nNewPos--;
                    bEnd=TRUE;
                } else if (rBR.IsOver(pCmpObj->GetCurrentBoundRect())) {
                    nNewPos=nCmpPos;
                    bEnd=TRUE;
                } else nCmpPos++;
            }
            if (nNowPos!=nNewPos) {
                // --> OD 2004-08-24 #110810#
//              if (bBundleVirtObj) {
//                  SdrVirtObj* pV0=PTR_CAST(SdrVirtObj,pObj);
//                  if (pV0!=NULL) bNeedBundle=TRUE;
//                  do { // nicht zwischen virtuelle Objekte draengeln
//                      SdrObject* pV1Tmp=pOL->GetObj(nNewPos);
//                      SdrObject* pV2Tmp=pOL->GetObj(nNewPos+1);
//                      SdrVirtObj* pV1=PTR_CAST(SdrVirtObj,pV1Tmp);
//                      SdrVirtObj* pV2=PTR_CAST(SdrVirtObj,pV2Tmp);
//                      // Zwischen VirtObj mit gleichem Master wie ich darf ich schon
//                      if (pV0!=NULL && pV1!=NULL && &pV0->GetReferencedObj()==&pV1->GetReferencedObj()) break;
//                      // Zwischen andere nicht
//                      if (pV1==NULL || pV2==NULL || &pV1->GetReferencedObj()!=&pV2->GetReferencedObj()) break;
//                      nNewPos++;
//                  } while (TRUE);
//              }
                // <--
                bChg=TRUE;
                pOL->SetObjectOrdNum(nNowPos,nNewPos);
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoObjectOrdNum(*pObj,nNowPos,nNewPos));
                ObjOrderChanged(pObj,nNowPos,nNewPos);
            }
            nNewPos--;
        }
        // --> OD 2004-08-24 #110810#
//      if (bNeedBundle) ImpBundleVirtObjOfMarkList();
        // <--
        EndUndo();
        if (bChg) MarkListHasChanged();
    }
}

void SdrEditView::MovMarkedToBtm()
{
    ULONG nAnz=GetMarkedObjectCount();
    if (nAnz!=0) {
        BegUndo(ImpGetResStr(STR_EditMovToBtm),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_MOVTOBTM);
        SortMarkedObjects();
        ULONG nm;
        for (nm=0; nm<nAnz; nm++) { // Ordnums muessen alle stimmen!
            GetMarkedObjectByIndex(nm)->GetOrdNum();
        }
        // --> OD 2004-08-24 #110810#
//      BOOL bNeedBundle=FALSE;
        // <--
        BOOL bChg=FALSE;
        SdrObjList* pOL0=NULL;
        ULONG nNewPos=0;
        for (nm=0; nm<nAnz; nm++) {
            SdrMark* pM=GetSdrMarkByIndex(nm);
            SdrObject* pObj=pM->GetObj();
            SdrObjList* pOL=pObj->GetObjList();
            if (pOL!=pOL0) {
                nNewPos=0;
                pOL0=pOL;
            }
            ULONG nNowPos=pObj->GetOrdNumDirect();
            const Rectangle& rBR=pObj->GetCurrentBoundRect();
            ULONG nCmpPos=nNowPos; if (nCmpPos>0) nCmpPos--;
            SdrObject* pMaxObj=GetMaxToBtmObj(pObj);
            if (pMaxObj!=NULL) {
                ULONG nMinPos=pMaxObj->GetOrdNum()+1;
                if (nNewPos<nMinPos) nNewPos=nMinPos; // diesen nicht ueberholen.
                if (nNewPos>nNowPos) nNewPos=nNowPos; // aber dabei auch nicht in die falsche Richtung schieben
            }
            BOOL bEnd=FALSE;
            // nNewPos ist an dieser Stelle noch die maximale Position,
            // an der das Obj hinruecken darf, ohne seinen Vorgaenger
            // (Mehrfachselektion) zu ueberholen.
            while (nCmpPos>nNewPos && !bEnd) {
                SdrObject* pCmpObj=pOL->GetObj(nCmpPos);
                if (pCmpObj==NULL) {
                    DBG_ERROR("MovMarkedToBtm(): Vergleichsobjekt nicht gefunden");
                    bEnd=TRUE;
                } else if (pCmpObj==pMaxObj) {
                    nNewPos=nCmpPos;
                    nNewPos++;
                    bEnd=TRUE;
                } else if (rBR.IsOver(pCmpObj->GetCurrentBoundRect())) {
                    nNewPos=nCmpPos;
                    bEnd=TRUE;
                } else nCmpPos--;
            }
            if (nNowPos!=nNewPos) {
                // --> OD 2004-08-24 #110810#
//              if (bBundleVirtObj) {
//                  SdrVirtObj* pV0=PTR_CAST(SdrVirtObj,pObj);
//                  if (pV0!=NULL) bNeedBundle=TRUE;
//                  do { // nicht zwischen virtuelle Objekte draengeln
//                      if (nNewPos==0) break;
//                      SdrObject* pV1Tmp=pOL->GetObj(nNewPos);
//                      SdrObject* pV2Tmp=pOL->GetObj(ULONG(nNewPos-1));
//                      SdrVirtObj* pV1=PTR_CAST(SdrVirtObj,pV1Tmp);
//                      SdrVirtObj* pV2=PTR_CAST(SdrVirtObj,pV2Tmp);
//                      // Zwischen VirtObj mit gleichem Master wie ich darf ich schon
//                      if (pV0!=NULL && pV1!=NULL && &pV0->GetReferencedObj()==&pV1->GetReferencedObj()) break;
//                      // Zwischen andere nicht
//                      if (pV1==NULL || pV2==NULL || &pV1->GetReferencedObj()!=&pV2->GetReferencedObj()) break;
//                      nNewPos--;
//                  } while (TRUE);
//              }
                // <--
                bChg=TRUE;
                pOL->SetObjectOrdNum(nNowPos,nNewPos);
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoObjectOrdNum(*pObj,nNowPos,nNewPos));
                ObjOrderChanged(pObj,nNowPos,nNewPos);
            }
            nNewPos++;
        }
        // --> OD 2004-08-24 #110810#
//      if (bNeedBundle) ImpBundleVirtObjOfMarkList();
        // <--
        EndUndo();
        if (bChg) MarkListHasChanged();
    }
}

void SdrEditView::PutMarkedToTop()
{
    PutMarkedInFrontOfObj(NULL);
}

void SdrEditView::PutMarkedInFrontOfObj(const SdrObject* pRefObj)
{
    ULONG nAnz=GetMarkedObjectCount();
    if (nAnz!=0) {
        BegUndo(ImpGetResStr(STR_EditPutToTop),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_PUTTOTOP);
        SortMarkedObjects();
        if (pRefObj!=NULL) {
            // Damit "Vor das Objekt" auch funktioniert wenn die
            // markierten Objekte bereits vor dem Objekt stehen
            ULONG nRefMark=TryToFindMarkedObject(pRefObj);
            SdrMark aRefMark;
            if (nRefMark!=CONTAINER_ENTRY_NOTFOUND)
            {
                aRefMark=*GetSdrMarkByIndex(nRefMark);
                GetMarkedObjectListWriteAccess().DeleteMark(nRefMark);
            }
            PutMarkedToBtm();
            if (nRefMark!=CONTAINER_ENTRY_NOTFOUND)
            {
                GetMarkedObjectListWriteAccess().InsertEntry(aRefMark);
                SortMarkedObjects();
            }
        }
        ULONG nm;
        for (nm=0; nm<nAnz; nm++) { // Ordnums muessen alle stimmen!
            GetMarkedObjectByIndex(nm)->GetOrdNum();
        }
        // --> OD 2004-08-24 #110810#
//      BOOL bNeedBundle=FALSE;
        // <--
        BOOL bChg=FALSE;
        SdrObjList* pOL0=NULL;
        ULONG nNewPos=0;
        for (nm=nAnz; nm>0;) {
            nm--;
            SdrMark* pM=GetSdrMarkByIndex(nm);
            SdrObject* pObj=pM->GetObj();
            if (pObj!=pRefObj) {
                SdrObjList* pOL=pObj->GetObjList();
                if (pOL!=pOL0) {
                    nNewPos=ULONG(pOL->GetObjCount()-1);
                    pOL0=pOL;
                }
                ULONG nNowPos=pObj->GetOrdNumDirect();
                SdrObject* pMaxObj=GetMaxToTopObj(pObj);
                if (pMaxObj!=NULL) {
                    ULONG nMaxOrd=pMaxObj->GetOrdNum(); // geht leider nicht anders
                    if (nMaxOrd>0) nMaxOrd--;
                    if (nNewPos>nMaxOrd) nNewPos=nMaxOrd; // nicht ueberholen.
                    if (nNewPos<nNowPos) nNewPos=nNowPos; // aber dabei auch nicht in die falsche Richtung schieben
                }
                if (pRefObj!=NULL) {
                    if (pRefObj->GetObjList()==pObj->GetObjList()) {
                        ULONG nMaxOrd=pRefObj->GetOrdNum(); // geht leider nicht anders
                        if (nNewPos>nMaxOrd) nNewPos=nMaxOrd; // nicht ueberholen.
                        if (nNewPos<nNowPos) nNewPos=nNowPos; // aber dabei auch nicht in die falsche Richtung schieben
                    } else {
                        nNewPos=nNowPos; // andere PageView, also nicht veraendern
                    }
                }
                if (nNowPos!=nNewPos) {
                    // --> OD 2004-08-24 #110810#
//                  if (bBundleVirtObj && HAS_BASE(SdrVirtObj,pObj)) bNeedBundle=TRUE;
                    // <--
                    bChg=TRUE;
                    pOL->SetObjectOrdNum(nNowPos,nNewPos);
                    AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoObjectOrdNum(*pObj,nNowPos,nNewPos));
                    ObjOrderChanged(pObj,nNowPos,nNewPos);
                }
                nNewPos--;
            } // if (pObj!=pRefObj)
        } // for-Schleife ueber alle Markierten Objekte
        // --> OD 2004-08-24 #110810#
//      if (bNeedBundle) ImpBundleVirtObjOfMarkList();
        // <--
        EndUndo();
        if (bChg) MarkListHasChanged();
    }
}

void SdrEditView::PutMarkedToBtm()
{
    PutMarkedBehindObj(NULL);
}

void SdrEditView::PutMarkedBehindObj(const SdrObject* pRefObj)
{
    ULONG nAnz=GetMarkedObjectCount();
    if (nAnz!=0) {
        BegUndo(ImpGetResStr(STR_EditPutToBtm),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_PUTTOBTM);
        SortMarkedObjects();
        if (pRefObj!=NULL) {
            // Damit "Hinter das Objekt" auch funktioniert wenn die
            // markierten Objekte bereits hinter dem Objekt stehen
            ULONG nRefMark=TryToFindMarkedObject(pRefObj);
            SdrMark aRefMark;
            if (nRefMark!=CONTAINER_ENTRY_NOTFOUND)
            {
                aRefMark=*GetSdrMarkByIndex(nRefMark);
                GetMarkedObjectListWriteAccess().DeleteMark(nRefMark);
            }
            PutMarkedToTop();
            if (nRefMark!=CONTAINER_ENTRY_NOTFOUND)
            {
                GetMarkedObjectListWriteAccess().InsertEntry(aRefMark);
                SortMarkedObjects();
            }
        }
        ULONG nm;
        for (nm=0; nm<nAnz; nm++) { // Ordnums muessen alle stimmen!
            GetMarkedObjectByIndex(nm)->GetOrdNum();
        }
        // --> OD 2004-08-24 #110810#
//      BOOL bNeedBundle=FALSE;
        // <--
        BOOL bChg=FALSE;
        SdrObjList* pOL0=NULL;
        ULONG nNewPos=0;
        for (nm=0; nm<nAnz; nm++) {
            SdrMark* pM=GetSdrMarkByIndex(nm);
            SdrObject* pObj=pM->GetObj();
            if (pObj!=pRefObj) {
                SdrObjList* pOL=pObj->GetObjList();
                if (pOL!=pOL0) {
                    nNewPos=0;
                    pOL0=pOL;
                }
                ULONG nNowPos=pObj->GetOrdNumDirect();
                SdrObject* pMinObj=GetMaxToBtmObj(pObj);
                if (pMinObj!=NULL) {
                    ULONG nMinOrd=pMinObj->GetOrdNum()+1; // geht leider nicht anders
                    if (nNewPos<nMinOrd) nNewPos=nMinOrd; // nicht ueberholen.
                    if (nNewPos>nNowPos) nNewPos=nNowPos; // aber dabei auch nicht in die falsche Richtung schieben
                }
                if (pRefObj!=NULL) {
                    if (pRefObj->GetObjList()==pObj->GetObjList()) {
                        ULONG nMinOrd=pRefObj->GetOrdNum(); // geht leider nicht anders
                        if (nNewPos<nMinOrd) nNewPos=nMinOrd; // nicht ueberholen.
                        if (nNewPos>nNowPos) nNewPos=nNowPos; // aber dabei auch nicht in die falsche Richtung schieben
                    } else {
                        nNewPos=nNowPos; // andere PageView, also nicht veraendern
                    }
                }
                if (nNowPos!=nNewPos) {
                    // --> OD 2004-08-24 #110810#
//                  if (bBundleVirtObj && !bNeedBundle && HAS_BASE(SdrVirtObj,pObj)) bNeedBundle=TRUE;
                    // <--
                    bChg=TRUE;
                    pOL->SetObjectOrdNum(nNowPos,nNewPos);
                    AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoObjectOrdNum(*pObj,nNowPos,nNewPos));
                    ObjOrderChanged(pObj,nNowPos,nNewPos);
                }
                nNewPos++;
            } // if (pObj!=pRefObj)
        } // for-Schleife ueber alle markierten Objekte
        // --> OD 2004-08-24 #110810#
//      if (bNeedBundle) ImpBundleVirtObjOfMarkList();
        // <--
        EndUndo();
        if (bChg) MarkListHasChanged();
    }
}

void SdrEditView::ReverseOrderOfMarked()
{
    SortMarkedObjects();
    ULONG nMarkAnz=GetMarkedObjectCount();
    if (nMarkAnz>0) {
        //BOOL bNeedBundle=FALSE;
        BOOL bChg=FALSE;
        BegUndo(ImpGetResStr(STR_EditRevOrder),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_REVORDER);
        ULONG a=0;
        do {
            // Markierung ueber mehrere PageViews berueksichtigen
            ULONG b=a+1;
            while (b<nMarkAnz && GetSdrPageViewOfMarkedByIndex(b) == GetSdrPageViewOfMarkedByIndex(a)) b++;
            b--;
            SdrObjList* pOL=GetSdrPageViewOfMarkedByIndex(a)->GetObjList();
            ULONG c=b;
            if (a<c) { // Sicherstellen, dass die OrdNums nicht Dirty sind
                GetMarkedObjectByIndex(a)->GetOrdNum();
            }
            while (a<c) {
                SdrObject* pObj1=GetMarkedObjectByIndex(a);
                SdrObject* pObj2=GetMarkedObjectByIndex(c);
                ULONG nOrd1=pObj1->GetOrdNumDirect();
                ULONG nOrd2=pObj2->GetOrdNumDirect();
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoObjectOrdNum(*pObj1,nOrd1,nOrd2));
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoObjectOrdNum(*pObj2,nOrd2-1,nOrd1));
                pOL->SetObjectOrdNum(nOrd1,nOrd2);
                // Obj 2 ist um eine Position nach vorn gerutscht, deshalb nun nOrd2-1
                pOL->SetObjectOrdNum(nOrd2-1,nOrd1);
                // Verwendung von Replace statt SetOrdNum wg. Performance (Neuberechnung der Ordnums)
                a++; c--;
                bChg=TRUE;
                // --> OD 2004-08-24 #110810#
//              if (bBundleVirtObj && !bNeedBundle &&
//                  (HAS_BASE(SdrVirtObj,pObj1) || HAS_BASE(SdrVirtObj,pObj2))) bNeedBundle=TRUE;
                // <--
            }
            a=b+1;
        } while (a<nMarkAnz);
        // --> OD 2004-08-24 #110810#
//      if (bNeedBundle) ImpBundleVirtObjOfMarkList();
        // <--
        EndUndo();
        if (bChg) MarkListHasChanged();
    }
}

void SdrEditView::ImpCheckToTopBtmPossible()
{
    ULONG nAnz=GetMarkedObjectCount();
    if (nAnz==0) return;
    if (nAnz==1) { // Sonderbehandlung fuer Einzelmarkierung
        SdrObject* pObj=GetMarkedObjectByIndex(0);
        SdrObjList* pOL=pObj->GetObjList();
        ULONG nMax=pOL->GetObjCount();
        ULONG nMin=0;
        ULONG nObjNum=pObj->GetOrdNum();
        SdrObject* pRestrict=GetMaxToTopObj(pObj);
        if (pRestrict!=NULL) {
            ULONG nRestrict=pRestrict->GetOrdNum();
            if (nRestrict<nMax) nMax=nRestrict;
        }
        pRestrict=GetMaxToBtmObj(pObj);
        if (pRestrict!=NULL) {
            ULONG nRestrict=pRestrict->GetOrdNum();
            if (nRestrict>nMin) nMin=nRestrict;
        }
        bToTopPossible=nObjNum<ULONG(nMax-1);
        bToBtmPossible=nObjNum>nMin;
    } else { // Mehrfachselektion
        ULONG nm=0;
        SdrObjList* pOL0=NULL;
        long nPos0=-1;
        while (!bToBtmPossible && nm<nAnz) { // 'nach hinten' checken
            SdrObject* pObj=GetMarkedObjectByIndex(nm);
            SdrObjList* pOL=pObj->GetObjList();
            if (pOL!=pOL0) {
                nPos0=-1;
                pOL0=pOL;
            }
            ULONG nPos=pObj->GetOrdNum();
            bToBtmPossible=nPos>ULONG(nPos0+1);
            nPos0=long(nPos);
            nm++;
        }
        nm=nAnz;
        pOL0=NULL;
        nPos0=0x7FFFFFFF;
        while (!bToTopPossible && nm>0) { // 'nach vorn' checken
            nm--;
            SdrObject* pObj=GetMarkedObjectByIndex(nm);
            SdrObjList* pOL=pObj->GetObjList();
            if (pOL!=pOL0) {
                nPos0=pOL->GetObjCount();
                pOL0=pOL;
            }
            ULONG nPos=pObj->GetOrdNum();
            bToTopPossible=nPos+1<ULONG(nPos0);
            nPos0=nPos;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   @@@@    @@@@   @@   @@  @@@@@   @@  @@  @@  @@@@@
//  @@  @@  @@  @@  @@@ @@@  @@  @@  @@  @@@ @@  @@
//  @@      @@  @@  @@@@@@@  @@  @@  @@  @@@@@@  @@
//  @@      @@  @@  @@@@@@@  @@@@@   @@  @@@@@@  @@@@
//  @@      @@  @@  @@ @ @@  @@  @@  @@  @@ @@@  @@
//  @@  @@  @@  @@  @@   @@  @@  @@  @@  @@  @@  @@
//   @@@@    @@@@   @@   @@  @@@@@   @@  @@  @@  @@@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrEditView::ImpCopyAttributes(const SdrObject* pSource, SdrObject* pDest) const
{
    if (pSource!=NULL) {
        SdrObjList* pOL=pSource->GetSubList();
        if (pOL!=NULL && !pSource->Is3DObj()) { // erstes Nichtgruppenobjekt aus der Gruppe holen
            SdrObjListIter aIter(*pOL,IM_DEEPNOGROUPS);
            pSource=aIter.Next();
        }
    }

    if(pSource && pDest)
    {
        SfxItemSet aSet(pMod->GetItemPool(),
            SDRATTR_START,              SDRATTR_NOTPERSIST_FIRST-1,
            SDRATTR_NOTPERSIST_LAST+1,  SDRATTR_END,
            EE_ITEMS_START,             EE_ITEMS_END,
            0, 0); // #52757#, #52762#

        aSet.Put(pSource->GetMergedItemSet());

        pDest->ClearMergedItem();
        pDest->SetMergedItemSet(aSet);

        pDest->NbcSetLayer(pSource->GetLayer());
        pDest->NbcSetStyleSheet(pSource->GetStyleSheet(), sal_True);
    }
}

BOOL SdrEditView::ImpCanConvertForCombine1(const SdrObject* pObj) const
{
    // #69711 : new condition IsLine() to be able to combine simple Lines
    BOOL bIsLine = FALSE;
    const SdrPathObj* pPath=PTR_CAST(SdrPathObj,pObj);
    if (pPath!=NULL)
        bIsLine = pPath->IsLine();

    SdrObjTransformInfoRec aInfo;
    pObj->TakeObjInfo(aInfo);
    return ( aInfo.bCanConvToPath || aInfo.bCanConvToPoly || bIsLine );
}

BOOL SdrEditView::ImpCanConvertForCombine(const SdrObject* pObj) const
{
    SdrObjList* pOL=pObj->GetSubList();
    if (pOL!=NULL && !pObj->Is3DObj()) {
        SdrObjListIter aIter(*pOL,IM_DEEPNOGROUPS);
        while (aIter.IsMore()) {
            SdrObject* pObj1=aIter.Next();
            // Es muessen alle Member einer Gruppe konvertierbar sein
            if (!ImpCanConvertForCombine1(pObj1)) return FALSE;
        }
    } else {
        if (!ImpCanConvertForCombine1(pObj)) return FALSE;
    }
    return TRUE;
}

XPolyPolygon SdrEditView::ImpGetXPolyPoly1(const SdrObject* pObj, BOOL bCombine) const
{
    XPolyPolygon aRet;
    SdrPathObj* pPath=PTR_CAST(SdrPathObj,pObj);
    if (bCombine && pPath!=NULL && pObj->GetOutlinerParaObject()==NULL) {
        aRet=pPath->GetPathPoly();
    } else {
        SdrObject* pConvObj=pObj->ConvertToPolyObj(bCombine, FALSE);
        if (pConvObj!=NULL) {
            SdrObjList* pOL=pConvObj->GetSubList();
            if (pOL!=NULL) {
                SdrObjListIter aIter(*pOL,IM_DEEPNOGROUPS);
                while (aIter.IsMore() && !bCombineError) {
                    SdrObject* pObj1=aIter.Next();
                    pPath=PTR_CAST(SdrPathObj,pObj1);
                    if (pPath!=NULL) {
                        if ((ULONG)aRet.Count()+(ULONG)pPath->GetPathPoly().Count()<=0xFFFF) {
                            aRet.Insert(pPath->GetPathPoly());
                        } else {
                            ((SdrEditView*)this)->bCombineError=TRUE;
                        }
                    }
                }
            } else {
                pPath=PTR_CAST(SdrPathObj,pConvObj);
                if (pPath!=NULL) {
                    aRet=pPath->GetPathPoly();
                }
            }
            delete pConvObj;
        }
    }
    return aRet;
}

XPolyPolygon SdrEditView::ImpGetXPolyPoly(const SdrObject* pObj, BOOL bCombine) const
{
    XPolyPolygon aRet;
    SdrObjList* pOL=pObj->GetSubList();
    if (pOL!=NULL && !pObj->Is3DObj()) {
        SdrObjListIter aIter(*pOL,IM_DEEPNOGROUPS);
        while (aIter.IsMore() && !bCombineError) {
            SdrObject* pObj1=aIter.Next();
            XPolyPolygon aXPP(ImpGetXPolyPoly1(pObj1, bCombine));
            if ((ULONG)aRet.Count()+(ULONG)aXPP.Count()<=0xFFFF) {
                aRet.Insert(aXPP);
            } else {
                ((SdrEditView*)this)->bCombineError=TRUE;
            }
        }
    } else {
        aRet=ImpGetXPolyPoly1(pObj, bCombine);
    }
    return aRet;
}

void SdrEditView::ImpCombineToSinglePoly(XPolyPolygon& rXPP, long nJoinTol) const
{
    USHORT nPolyCount=rXPP.Count();
    if (nPolyCount>=2) {
        XPolygon aXP(rXPP[0]);
        if (aXP.GetPointCount()!=0) {
            aXP.SetFlags(0,XPOLY_NORMAL);
            aXP.SetFlags(USHORT(aXP.GetPointCount()-1),XPOLY_NORMAL);
        }
        for (USHORT i=1; i<nPolyCount && !bCombineError; i++) {
            USHORT nDstPointCount=aXP.GetPointCount();
            const XPolygon rSrcPoly=rXPP[i];
            USHORT nSrcPointCount=rSrcPoly.GetPointCount();
            if ((ULONG)nSrcPointCount+(ULONG)nDstPointCount>XPOLY_MAXPOINTS) {
                ((SdrEditView*)this)->bCombineError=TRUE;
            } else {
                if (nDstPointCount==0 || nSrcPointCount==0) {
                    aXP.Insert(nDstPointCount,rSrcPoly);
                } else {
                    Point aDstPt0(aXP[0]);
                    Point aDstPt1(aXP[USHORT(nDstPointCount-1)]);
                    Point aSrcPt0(rSrcPoly[0]);
                    Point aSrcPt1(rSrcPoly[USHORT(nSrcPointCount-1)]);
                    long n00=Abs(aSrcPt0.X()-aDstPt0.X())+Abs(aSrcPt0.Y()-aDstPt0.Y());
                    long n10=Abs(aSrcPt1.X()-aDstPt0.X())+Abs(aSrcPt1.Y()-aDstPt0.Y());
                    long n01=Abs(aSrcPt0.X()-aDstPt1.X())+Abs(aSrcPt0.Y()-aDstPt1.Y());
                    long n11=Abs(aSrcPt1.X()-aDstPt1.X())+Abs(aSrcPt1.Y()-aDstPt1.Y());
                    BOOL bAppend=(n01<=n00 && n01<=n10) || (n11<=n00 && n11<=n10);
                    BOOL bRevers=bAppend ? n11<n01 : n00<n10;
                    USHORT nDstPos=bAppend ? nDstPointCount : 0;
                    BOOL b1st=TRUE;
                    if (bRevers) {
                        for (USHORT nSrcPos=nSrcPointCount; nSrcPos>0;) {
                            nSrcPos--;
                            BOOL bNoIns=FALSE;
                            if (b1st) bNoIns=(bAppend ? n11 : n00) <=nJoinTol;
                            if (!bNoIns) {
                                aXP.Insert(nDstPos,rSrcPoly[nSrcPos],rSrcPoly.GetFlags(nSrcPos));
                                if (bAppend) nDstPos++;
                            }
                            b1st=FALSE;
                        }
                    } else {
                        for (USHORT nSrcPos=0; nSrcPos<nSrcPointCount; nSrcPos++) {
                            BOOL bNoIns=FALSE;
                            if (b1st) bNoIns=(bAppend ? n01 : n10) <=nJoinTol;
                            if (!bNoIns) {
                                aXP.Insert(nDstPos,rSrcPoly[nSrcPos],rSrcPoly.GetFlags(nSrcPos));
                                if (bAppend) nDstPos++;
                            }
                            b1st=FALSE;
                        }
                    }
                }
                if (aXP.GetPointCount()!=0) {
                    aXP.SetFlags(0,XPOLY_NORMAL);
                    aXP.SetFlags(USHORT(aXP.GetPointCount()-1),XPOLY_NORMAL);
                }
            }
        }
        if (!bCombineError) {
            rXPP.Clear();
            rXPP.Insert(aXP,0);
        }
    }
}

// for distribution dialog function
struct ImpDistributeEntry
{
    SdrObject*                  mpObj;
    INT32                       mnPos;
    INT32                       mnLength;
};

DECLARE_LIST(ImpDistributeEntryList, ImpDistributeEntry*)

void SdrEditView::DistributeMarkedObjects()
{
#ifndef SVX_LIGHT

    UINT32 nMark(GetMarkedObjectCount());

    if(nMark > 2)
    {
        SfxItemSet aNewAttr(pMod->GetItemPool());
        //CHINA001 SvxDistributeDialog* pDlg = new SvxDistributeDialog(NULL, aNewAttr);
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        if(pFact)
        {
            AbstractSvxDistributeDialog *pDlg = pFact->CreateSvxDistributeDialog(NULL, aNewAttr, ResId(RID_SVXPAGE_DISTRIBUTE));
            DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001

            UINT16 nResult = pDlg->Execute();

            if(nResult == RET_OK)
            {
                SvxDistributeHorizontal eHor = pDlg->GetDistributeHor();
                SvxDistributeVertical eVer = pDlg->GetDistributeVer();
                ImpDistributeEntryList aEntryList;
                UINT32 a, nInsPos, nFullLength;

                BegUndo();

                if(eHor != SvxDistributeHorizontalNone)
                {
                    // build sorted entry list
                    nFullLength = 0L;

                    for(a=0;a<nMark;a++)
                    {
                        SdrMark* pMark = GetSdrMarkByIndex(a);
                        ImpDistributeEntry* pNew = new ImpDistributeEntry;

                        pNew->mpObj = pMark->GetObj();
                        nInsPos = 0;

                        switch(eHor)
                        {
                            case SvxDistributeHorizontalLeft:
                            {
                                pNew->mnPos = pNew->mpObj->GetSnapRect().Left();
                                break;
                            }
                            case SvxDistributeHorizontalCenter:
                            {
                                pNew->mnPos = (pNew->mpObj->GetSnapRect().Right() + pNew->mpObj->GetSnapRect().Left()) / 2;
                                break;
                            }
                            case SvxDistributeHorizontalDistance:
                            {
                                pNew->mnLength = pNew->mpObj->GetSnapRect().GetWidth() + 1;
                                nFullLength += pNew->mnLength;
                                pNew->mnPos = (pNew->mpObj->GetSnapRect().Right() + pNew->mpObj->GetSnapRect().Left()) / 2;
                                break;
                            }
                            case SvxDistributeHorizontalRight:
                            {
                                pNew->mnPos = pNew->mpObj->GetSnapRect().Right();
                                break;
                            }
                            default: break;
                        }

                        while(nInsPos < aEntryList.Count() && aEntryList.GetObject(nInsPos)->mnPos < pNew->mnPos)
                            nInsPos++;

                        aEntryList.Insert(pNew, nInsPos);
                    }

                    if(eHor == SvxDistributeHorizontalDistance)
                    {
                        // calc room in-between
                        INT32 nWidth = GetAllMarkedBoundRect().GetWidth() + 1;
                        double fStepWidth = ((double)nWidth - (double)nFullLength) / (double)(aEntryList.Count() - 1);
                        double fStepStart = (double)aEntryList.GetObject(0)->mnPos;
                        fStepStart += fStepWidth + (double)((aEntryList.GetObject(0)->mnLength + aEntryList.GetObject(1)->mnLength) / 2);

                        // move entries 1..n-1
                        for(a=1;a<aEntryList.Count()-1;a++)
                        {
                            ImpDistributeEntry* pCurr = aEntryList.GetObject(a);
                            ImpDistributeEntry* pNext = aEntryList.GetObject(a+1);
                            INT32 nDelta = (INT32)(fStepStart + 0.5) - pCurr->mnPos;
                            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoGeoObject(*pCurr->mpObj));
                            pCurr->mpObj->Move(Size(nDelta, 0));
                            fStepStart += fStepWidth + (double)((pCurr->mnLength + pNext->mnLength) / 2);
                        }
                    }
                    else
                    {
                        // calc distances
                        INT32 nWidth = aEntryList.GetObject(aEntryList.Count() - 1)->mnPos - aEntryList.GetObject(0)->mnPos;
                        double fStepWidth = (double)nWidth / (double)(aEntryList.Count() - 1);
                        double fStepStart = (double)aEntryList.GetObject(0)->mnPos;
                        fStepStart += fStepWidth;

                        // move entries 1..n-1
                        for(a=1;a<aEntryList.Count()-1;a++)
                        {
                            ImpDistributeEntry* pCurr = aEntryList.GetObject(a);
                            INT32 nDelta = (INT32)(fStepStart + 0.5) - pCurr->mnPos;
                            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoGeoObject(*pCurr->mpObj));
                            pCurr->mpObj->Move(Size(nDelta, 0));
                            fStepStart += fStepWidth;
                        }
                    }

                    // clear list
                    while(aEntryList.Count())
                        delete aEntryList.Remove((ULONG)0L);
                }

                if(eVer != SvxDistributeVerticalNone)
                {
                    // build sorted entry list
                    nFullLength = 0L;

                    for(a=0;a<nMark;a++)
                    {
                        SdrMark* pMark = GetSdrMarkByIndex(a);
                        ImpDistributeEntry* pNew = new ImpDistributeEntry;

                        pNew->mpObj = pMark->GetObj();
                        nInsPos = 0;

                        switch(eVer)
                        {
                            case SvxDistributeVerticalTop:
                            {
                                pNew->mnPos = pNew->mpObj->GetSnapRect().Top();
                                break;
                            }
                            case SvxDistributeVerticalCenter:
                            {
                                pNew->mnPos = (pNew->mpObj->GetSnapRect().Bottom() + pNew->mpObj->GetSnapRect().Top()) / 2;
                                break;
                            }
                            case SvxDistributeVerticalDistance:
                            {
                                pNew->mnLength = pNew->mpObj->GetSnapRect().GetHeight() + 1;
                                nFullLength += pNew->mnLength;
                                pNew->mnPos = (pNew->mpObj->GetSnapRect().Bottom() + pNew->mpObj->GetSnapRect().Top()) / 2;
                                break;
                            }
                            case SvxDistributeVerticalBottom:
                            {
                                pNew->mnPos = pNew->mpObj->GetSnapRect().Bottom();
                                break;
                            }
                            default: break;
                        }

                        while(nInsPos < aEntryList.Count() && aEntryList.GetObject(nInsPos)->mnPos < pNew->mnPos)
                            nInsPos++;

                        aEntryList.Insert(pNew, nInsPos);
                    }

                    if(eVer == SvxDistributeVerticalDistance)
                    {
                        // calc room in-between
                        INT32 nHeight = GetAllMarkedBoundRect().GetHeight() + 1;
                        double fStepWidth = ((double)nHeight - (double)nFullLength) / (double)(aEntryList.Count() - 1);
                        double fStepStart = (double)aEntryList.GetObject(0)->mnPos;
                        fStepStart += fStepWidth + (double)((aEntryList.GetObject(0)->mnLength + aEntryList.GetObject(1)->mnLength) / 2);

                        // move entries 1..n-1
                        for(a=1;a<aEntryList.Count()-1;a++)
                        {
                            ImpDistributeEntry* pCurr = aEntryList.GetObject(a);
                            ImpDistributeEntry* pNext = aEntryList.GetObject(a+1);
                            INT32 nDelta = (INT32)(fStepStart + 0.5) - pCurr->mnPos;
                            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoGeoObject(*pCurr->mpObj));
                            pCurr->mpObj->Move(Size(0, nDelta));
                            fStepStart += fStepWidth + (double)((pCurr->mnLength + pNext->mnLength) / 2);
                        }
                    }
                    else
                    {
                        // calc distances
                        INT32 nHeight = aEntryList.GetObject(aEntryList.Count() - 1)->mnPos - aEntryList.GetObject(0)->mnPos;
                        double fStepWidth = (double)nHeight / (double)(aEntryList.Count() - 1);
                        double fStepStart = (double)aEntryList.GetObject(0)->mnPos;
                        fStepStart += fStepWidth;

                        // move entries 1..n-1
                        for(a=1;a<aEntryList.Count()-1;a++)
                        {
                            ImpDistributeEntry* pCurr = aEntryList.GetObject(a);
                            INT32 nDelta = (INT32)(fStepStart + 0.5) - pCurr->mnPos;
                            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoGeoObject(*pCurr->mpObj));
                            pCurr->mpObj->Move(Size(0, nDelta));
                            fStepStart += fStepWidth;
                        }
                    }

                    // clear list
                    while(aEntryList.Count())
                        delete aEntryList.Remove((ULONG)0L);
                }

                // UNDO-Comment and end of UNDO
                SetUndoComment(ImpGetResStr(STR_DistributeMarkedObjects));
                EndUndo();
            }

            delete(pDlg);
        }
    }
#endif
}

void SdrEditView::MergeMarkedObjects(SdrMergeMode eMode)
{
    SdrMarkList aRemove;
    SortMarkedObjects();
    BegUndo();

    UINT32 nInsPos=0xFFFFFFFF;
    //UINT32 nAnz=GetMarkedObjectCount();
    const SdrObject* pAttrObj = NULL;

    //PolyPolygon3D aMergePolyPolygonA;
    //PolyPolygon3D aMergePolyPolygonB;
    ::basegfx::B2DPolyPolygon aMergePolyPolygonA;
    ::basegfx::B2DPolyPolygon aMergePolyPolygonB;

    SdrObjList* pInsOL = NULL;
    SdrPageView* pInsPV = NULL;
    BOOL bFirstObjectComplete(FALSE);

    // make sure selected objects are contour objects

    // since now ::basegfx::tools::adaptiveSubdivide() is used, it is no longer
    // necessary to use ConvertMarkedToPolyObj which will subdivide curves using the old
    // mechanisms. In a next step the polygon clipper will even be able to clip curves...
    // ConvertMarkedToPolyObj(TRUE);
    ConvertMarkedToPathObj(sal_True);

    for(UINT32 a=0;a<GetMarkedObjectCount();a++)
    {
        SdrMark* pM = GetSdrMarkByIndex(a);
        SdrObject* pObj = pM->GetObj();

        if(ImpCanConvertForCombine(pObj))
        {
            if(!pAttrObj)
                pAttrObj = pObj;

            nInsPos = pObj->GetOrdNum() + 1;
            pInsPV = pM->GetPageView();
            pInsOL = pObj->GetObjList();

            // get the polygons; do NOT use ImpGetXPolyPoly here - it does fetch all
            // the polygons, but they are all put in one single XPolyPolygon so that
            // no SetDirections() on single objects can be made. This would lead to
            // wrong results.
            SdrObjList* pObjectList = pObj->GetSubList();
            //Vector3D aCommonNormal(0.0, 0.0, 1.0);

            if(pObjectList)
            {
                SdrObjListIter aIter(*pObjectList, IM_DEEPWITHGROUPS);

                while(aIter.IsMore())
                {
                    SdrObject* pCandidate = aIter.Next();
                    SdrPathObj* pPathObj = PTR_CAST(SdrPathObj, pCandidate);
                    if(pPathObj)
                    {
                        //PolyPolygon3D aTmpPoly3D(pPathObj->GetPathPoly());
                        //aTmpPoly3D.SetDirections(aCommonNormal);
                        ::basegfx::B2DPolyPolygon aTmpPoly(pPathObj->GetPathPoly().getB2DPolyPolygon());

                        if(aTmpPoly.areControlPointsUsed())
                        {
                            aTmpPoly = ::basegfx::tools::adaptiveSubdivideByAngle(aTmpPoly);
                        }

                        // #i37009#
                        aTmpPoly = ::basegfx::tools::correctOrientations(aTmpPoly);

                        if(!bFirstObjectComplete)
                        {
                            //aMergePolyPolygonA.Insert(aTmpPoly3D);
                            aMergePolyPolygonA.append(aTmpPoly);
                        }
                        else
                        {
                            //aMergePolyPolygonB.Insert(aTmpPoly3D);
                            aMergePolyPolygonB.append(aTmpPoly);
                        }
                    }
                }
            }
            else
            {
                SdrPathObj* pPathObj = PTR_CAST(SdrPathObj, pObj);
                if(pPathObj)
                {
                    //PolyPolygon3D aTmpPoly3D(pPathObj->GetPathPoly());
                    //aTmpPoly3D.SetDirections(aCommonNormal);
                    ::basegfx::B2DPolyPolygon aTmpPoly(pPathObj->GetPathPoly().getB2DPolyPolygon());

                    if(aTmpPoly.areControlPointsUsed())
                    {
                        aTmpPoly = ::basegfx::tools::adaptiveSubdivideByAngle(aTmpPoly);
                    }

                    // #i37009#
                    aTmpPoly = ::basegfx::tools::correctOrientations(aTmpPoly);

                    if(!bFirstObjectComplete)
                    {
                        //aMergePolyPolygonA.Insert(aTmpPoly3D);
                        aMergePolyPolygonA.append(aTmpPoly);
                    }
                    else
                    {
                        //aMergePolyPolygonB.Insert(aTmpPoly3D);
                        aMergePolyPolygonB.append(aTmpPoly);
                    }
                }
            }

            // was there something added to the first poly?
            //if(!bFirstObjectComplete && aMergePolyPolygonA.Count())
            if(!bFirstObjectComplete && aMergePolyPolygonA.count())
            {
                bFirstObjectComplete = TRUE;
            }

            // move object to temporary delete list
            aRemove.InsertEntry(SdrMark(pObj, pM->GetPageView()));
        }
    }

    switch(eMode)
    {
        case SDR_MERGE_MERGE:
        {
            // simple merge all contained parts (OR)
            //aMergePolyPolygonA.Insert(aMergePolyPolygonB);
            //aMergePolyPolygonA.Merge(FALSE);
            aMergePolyPolygonA.append(aMergePolyPolygonB);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonA, sal_False);
            aMergePolyPolygonA = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonA);
            aMergePolyPolygonA = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonA, sal_True);

            break;
        }
        case SDR_MERGE_SUBSTRACT:
        {
            // take selected poly 2..n (is in Polygon B), merge them, flipdirections
            // and merge with poly 1
            //aMergePolyPolygonA.Merge(FALSE);
            //aMergePolyPolygonB.Merge(FALSE);
            //aMergePolyPolygonB.FlipDirections();
            //aMergePolyPolygonA.Insert(aMergePolyPolygonB);
            //aMergePolyPolygonA.Merge(FALSE);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonA, sal_False);
            aMergePolyPolygonA = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonA);
            aMergePolyPolygonA = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonA, sal_True);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonB, sal_False);
            aMergePolyPolygonB = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonB);
            aMergePolyPolygonB = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonB, sal_True);
            aMergePolyPolygonB.flip();
            aMergePolyPolygonA.append(aMergePolyPolygonB);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonA, sal_False);
            aMergePolyPolygonA = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonA);
            aMergePolyPolygonA = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonA, sal_True);

            // #72995# one more call to resolve self intersections which
            // may have been built by substracting (see bug)
            //aMergePolyPolygonA.Merge(FALSE);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonA, sal_False);
            aMergePolyPolygonA = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonA);
            aMergePolyPolygonA = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonA, sal_True);

            break;
        }
        case SDR_MERGE_INTERSECT:
        {
            // cut poly 1 against polys 2..n (AND)
            //aMergePolyPolygonA.Merge(FALSE);
            //aMergePolyPolygonB.Merge(FALSE);
            //aMergePolyPolygonA.Insert(aMergePolyPolygonB);
            //aMergePolyPolygonA.Merge(FALSE, TRUE);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonA, sal_False);
            aMergePolyPolygonA = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonA);
            aMergePolyPolygonA = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonA, sal_True);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonB, sal_False);
            aMergePolyPolygonB = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonB);
            aMergePolyPolygonB = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonB, sal_True);
            aMergePolyPolygonA.append(aMergePolyPolygonB);
//BFS08         ::basegfx::tools::removeIntersections(aMergePolyPolygonA, sal_False, sal_True);
            aMergePolyPolygonA = ::basegfx::tools::removeAllIntersections(aMergePolyPolygonA);
            aMergePolyPolygonA = ::basegfx::tools::removeNeutralPolygons(aMergePolyPolygonA, sal_False);

            break;
        }
    }

    // XPolyPolygon aXPP = aMergePolyPolygonA.GetXPolyPolygon();
    XPolyPolygon aXPP(aMergePolyPolygonA);
    SdrPathObj* pPath = new SdrPathObj(OBJ_PATHFILL, aXPP);
    ImpCopyAttributes(pAttrObj, pPath);
    SdrInsertReason aReason(SDRREASON_VIEWCALL, pAttrObj);
    pInsOL->InsertObject(pPath, nInsPos, &aReason);
    AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoNewObject(*pPath));
    MarkObj(pPath, pInsPV, FALSE, TRUE);

    aRemove.ForceSort();
    switch(eMode)
    {
        case SDR_MERGE_MERGE:
        {
            SetUndoComment(
                ImpGetResStr(STR_EditMergeMergePoly),
                aRemove.GetMarkDescription());
            break;
        }
        case SDR_MERGE_SUBSTRACT:
        {
            SetUndoComment(
                ImpGetResStr(STR_EditMergeSubstractPoly),
                aRemove.GetMarkDescription());
            break;
        }
        case SDR_MERGE_INTERSECT:
        {
            SetUndoComment(
                ImpGetResStr(STR_EditMergeIntersectPoly),
                aRemove.GetMarkDescription());
            break;
        }
    }
    DeleteMarkedList(aRemove);

    EndUndo();
}

BOOL SdrEditView::CombineMarkedObjects(BOOL bNoPolyPoly)
{
    // #105899# Start of Combine-Undo put to front, else ConvertMarkedToPolyObj would
    // create a 2nd Undo-action and Undo-Comment.

    // Undo-String will be set later
    BegUndo(String(), String(),
        bNoPolyPoly ? SDRREPFUNC_OBJ_COMBINE_ONEPOLY : SDRREPFUNC_OBJ_COMBINE_POLYPOLY);

    // #105899# First, guarantee that all objects are converted to polyobjects,
    // especially for SdrGrafObj with bitmap filling this is necessary to not
    // loose the bitmap filling.

    // #i12392#
    // ConvertMarkedToPolyObj was too strong here, it will loose quality and
    // information when curve objects are combined. This can be replaced by
    // using ConvertMarkedToPathObj without changing the previous fix.

    // #i21250#
    // Instead of simply passing sal_True as LineToArea, use bNoPolyPoly as info
    // if this command is a 'Combine' or a 'Connect' command. On Connect it's sal_True.
    // To not concert line segments with a set line width to polygons in that case,
    // use this info. Do not convert LineToArea on Connect commands.
    // ConvertMarkedToPathObj(!bNoPolyPoly);

    // #114310#
    // This is used for Combine and Connect. In no case it is necessary to force
    // the content to curve, but it is also not good to force to polygons. Thus,
    // curve is the less information loosing one. Remember: This place is not
    // used for merge.
    // LineToArea is never necessary, both commands are able to take over the
    // set line style and to display it correctly. Thus, i will use a
    // ConvertMarkedToPathObj with a sal_False in any case. Only drawback is that
    // simple polygons will be changed to curves, but with no information loss.
    ConvertMarkedToPathObj(sal_False /* bLineToArea */);

    // continue as before
    bCombineError = FALSE;
    XPolyPolygon aXPP;
    SdrObjList* pAktOL = NULL;
    // Temporaere Marklist
    SdrMarkList aRemoveMerker;

    SortMarkedObjects();
    ULONG nInsPos=0xFFFFFFFF;
    SdrObjList* pInsOL=NULL;
    SdrPageView* pInsPV=NULL;
    ULONG nAktPointCount=0; // Gesamtpunkteanzahl aller Teilpolygone des PolyPolygons
    ULONG nm;
    ULONG nAnz=GetMarkedObjectCount();
    const SdrObject* pAttrObj=NULL;
    for (nm=nAnz; nm>0 && !bCombineError;) {
        nm--;
        SdrMark* pM=GetSdrMarkByIndex(nm);
        SdrObject* pObj=pM->GetObj();
        SdrObjList* pThisOL=pObj->GetObjList();
        if (pAktOL!=pThisOL) {
            pAktOL=pThisOL;
        }
        if (ImpCanConvertForCombine(pObj)) {
            pAttrObj=pObj; // Obj merken fuer Attribute kopieren
            XPolyPolygon aTmpXPP(ImpGetXPolyPoly(pObj, TRUE));
            ULONG nAktPolyCount=aXPP.Count();
            ULONG nTmpPolyCount=aTmpXPP.Count();
            ULONG nTmpPointCount=0;
            for (ULONG nTmpPolyNum=0; nTmpPolyNum<nTmpPolyCount; nTmpPolyNum++) {
                nTmpPointCount+=aTmpXPP[(USHORT)nTmpPolyNum].GetPointCount();
            }
            if (nAktPolyCount+nTmpPolyCount<=0xFFFF && nAktPointCount+nTmpPointCount<=XPOLY_MAXPOINTS) {
                aXPP.Insert(aTmpXPP,0);
                nAktPointCount+=nTmpPointCount;
            } else {
                bCombineError=TRUE;
            }
            if (pInsOL==NULL) {
                nInsPos=pObj->GetOrdNum()+1;
                pInsPV=pM->GetPageView();
                pInsOL=pObj->GetObjList();
            }
            aRemoveMerker.InsertEntry(SdrMark(pObj,pM->GetPageView()));
        }
    }
    long nJoinTol=10;
    if (bNoPolyPoly && !bCombineError) ImpCombineToSinglePoly(aXPP,nJoinTol);
    USHORT nPolyCount=aXPP.Count();
    if (nPolyCount!=0 && !bCombineError) {
        SdrObjKind eKind=OBJ_PATHFILL;
        if (nPolyCount>1) {
            // Polygone ggf. schliessen (Polylines,...)
            for (USHORT i=0; i<nPolyCount; i++) {
                const XPolygon& rXP=aXPP.GetObject(i);
                USHORT nPointCount=rXP.GetPointCount();
                USHORT nMaxPoint=USHORT(nPointCount-1);
                if (nPointCount>0 && rXP[0]!=rXP[nMaxPoint]) { // nicht geschlossen?
                    // XPolygon um einen Punkt erweitern (zum schliessen)
                    aXPP[i][nPointCount]=aXPP[i][0];
                }
            }
        } else { // auf Polyline Checken
            const XPolygon& rXP=aXPP.GetObject(0);
            USHORT nPointCount=rXP.GetPointCount();
            USHORT nMaxPoint=USHORT(nPointCount-1);
            if (nPointCount<=2) eKind=OBJ_PATHLINE;
            else {
                const Point& rPt0=rXP[0];
                const Point& rPt1=rXP[nMaxPoint];
                if (rPt0!=rPt1) { // nicht geschlossen?
                    long nDist=Abs(rPt0.X()-rPt1.X())+Abs(rPt0.Y()-rPt1.Y());
                    if (nDist<=nJoinTol) {
                        long x=(rPt0.X()+rPt1.X()+1)/2;
                        long y=(rPt0.Y()+rPt1.Y()+1)/2;
                        Point aMitte(x,y);
                        aXPP[0][0]=aMitte;
                        aXPP[0][nMaxPoint]=aMitte;
                    } else {
                        eKind=OBJ_PATHLINE;
                    }
                }
            }
        }

        SdrPathObj* pPath=new SdrPathObj(eKind,aXPP);
        // Attribute des untersten Objekts
        ImpCopyAttributes(pAttrObj,pPath);

        // #100408# If LineStyle of pAttrObj is XLINE_NONE force to XLINE_SOLID to make visible.
        const XLineStyle eLineStyle = ((const XLineStyleItem&)pAttrObj->GetMergedItem(XATTR_LINESTYLE)).GetValue();
        const XFillStyle eFillStyle = ((const XFillStyleItem&)pAttrObj->GetMergedItem(XATTR_FILLSTYLE)).GetValue();

        // #110635#
        // Take fill style/closed state of pAttrObj in account when deciding to change the line style
        sal_Bool bIsClosedPathObj(pAttrObj->ISA(SdrPathObj) && ((SdrPathObj*)pAttrObj)->IsClosed());

        if(XLINE_NONE == eLineStyle && (XFILL_NONE == eFillStyle || !bIsClosedPathObj))
        {
            pPath->SetMergedItem(XLineStyleItem(XLINE_SOLID));
        }

        SdrInsertReason aReason(SDRREASON_VIEWCALL,pAttrObj);
        pInsOL->InsertObject(pPath,nInsPos,&aReason);
        AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoNewObject(*pPath));

        // #111111#
        // Here was a severe error: Without UnmarkAllObj, the new object was marked
        // additionally to the two ones which are deleted below. As long as those are
        // in the UNDO there is no problem, but as soon as they get deleted, the
        // MarkList will contain deleted objects -> GPF.
        UnmarkAllObj(pInsPV);
        MarkObj(pPath, pInsPV, FALSE, TRUE);
    }
    if (!bCombineError) {
        // UndoComment aus den tatsaechlich verwendeten Objekten zusammenbauen
        aRemoveMerker.ForceSort(); // wichtig fuer Remove (s.u.)
        SetUndoComment(ImpGetResStr(bNoPolyPoly?STR_EditCombine_OnePoly:STR_EditCombine_PolyPoly),aRemoveMerker.GetMarkDescription());
        // die tatsaechlich verwendeten Objekten aus der Liste entfernen
        DeleteMarkedList(aRemoveMerker);
    }
    EndUndo();
    return !bCombineError;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@   @@   @@@@   @@   @@   @@@@   @@  @@  @@@@@@  @@     @@@@@
//  @@  @@  @@  @@  @@  @@@ @@@  @@  @@  @@@ @@    @@    @@     @@
//  @@  @@  @@  @@      @@@@@@@  @@  @@  @@@@@@    @@    @@     @@
//  @@  @@  @@   @@@@   @@@@@@@  @@@@@@  @@@@@@    @@    @@     @@@@
//  @@  @@  @@      @@  @@ @ @@  @@  @@  @@ @@@    @@    @@     @@
//  @@  @@  @@  @@  @@  @@   @@  @@  @@  @@  @@    @@    @@     @@
//  @@@@@   @@   @@@@   @@   @@  @@  @@  @@  @@    @@    @@@@@  @@@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SdrEditView::ImpCanDismantle(const XPolyPolygon& rXPP, BOOL bMakeLines) const
{
    BOOL bCan=FALSE;
    USHORT nPolyAnz=rXPP.Count();
    // Dismantle macht erst Sinn ab 2 Polygone im PolyPolygon
    if (nPolyAnz>=2) bCan=TRUE;
    else if (bMakeLines && nPolyAnz==1) { // oder ab 2 Linien bzw. 2 Segmenten im Polygon
        const XPolygon& rXP=rXPP[0];
        USHORT nPointAnz=rXP.GetPointCount();
        if (nPointAnz>4) bCan=TRUE;
        else if (nPointAnz>2) { // sonst ist es nur eine Linie
            if (!rXP.IsControl(1)) bCan=TRUE; // sonst ist es nun ein Beziersegment
        }
    }
    return bCan;
}

BOOL SdrEditView::ImpCanDismantle(const SdrObject* pObj, BOOL bMakeLines) const
{
    BOOL bOtherObjs=FALSE;    // TRUE=andere Objekte ausser PathObj's vorhanden
    BOOL bMin1PolyPoly=FALSE; // TRUE=mind. 1 PolyPolygon mit mehr als ein Polygon vorhanden
    SdrObjList* pOL=pObj->GetSubList();
    if (pOL!=NULL) {
        // Aha, Gruppenobjekt. Also alle Member ansehen.
        // Alle muessen PathObjs sein !
        SdrObjListIter aIter(*pOL,IM_DEEPNOGROUPS);
        while (aIter.IsMore() && !bOtherObjs) {
            const SdrObject* pObj1=aIter.Next();
            const SdrPathObj* pPath=PTR_CAST(SdrPathObj,pObj1);
            if (pPath!=NULL) {
                if (ImpCanDismantle(pPath->GetPathPoly(),bMakeLines)) bMin1PolyPoly=TRUE;
                SdrObjTransformInfoRec aInfo;
                pObj1->TakeObjInfo(aInfo);
                if (!aInfo.bCanConvToPath) bOtherObjs=TRUE; // Passiert z.B. im Falle Fontwork (Joe, 28-11-95)
            } else {
                bOtherObjs=TRUE;
            }
        }
    }
    else
    {
        const SdrPathObj* pPath = PTR_CAST(SdrPathObj, pObj);
        const SdrObjCustomShape* pCustomShape = PTR_CAST(SdrObjCustomShape, pObj);

        // #i37011#
        if(pPath)
        {
            if (ImpCanDismantle(pPath->GetPathPoly(),bMakeLines)) bMin1PolyPoly=TRUE;
            SdrObjTransformInfoRec aInfo;
            pObj->TakeObjInfo(aInfo);
            // #69711 : new condition IsLine() to be able to break simple Lines
            if (!(aInfo.bCanConvToPath || aInfo.bCanConvToPoly) && !pPath->IsLine() ) bOtherObjs=TRUE; // Passiert z.B. im Falle Fontwork (Joe, 28-11-95)
        }
        else if(pCustomShape)
        {
            if(bMakeLines)
            {
                // allow break command
                bMin1PolyPoly = TRUE;
            }
        }
        else
        {
            bOtherObjs = TRUE;
        }
    }
    return bMin1PolyPoly && !bOtherObjs;
}

void SdrEditView::ImpDismantleOneObject(const SdrObject* pObj, SdrObjList& rOL, ULONG& rPos, SdrPageView* pPV, BOOL bMakeLines)
{
    const SdrPathObj* pSrcPath = PTR_CAST(SdrPathObj, pObj);
    const SdrObjCustomShape* pCustomShape = PTR_CAST(SdrObjCustomShape, pObj);

    // #i37011#
    if(pSrcPath)
    {
        SdrObject* pLast=NULL; // // fuer die Zuweisung des OutlinerParaObject
        const XPolyPolygon& rXPP=pSrcPath->GetPathPoly();
        USHORT nPolyAnz=rXPP.Count();
        for (USHORT i=0; i<nPolyAnz; i++) {
            const XPolygon& rAktXP=rXPP.GetObject(i);
            const XPolygon* pXP=&rAktXP;
            XPolygon aTmpPoly;
            USHORT nPointAnz=rAktXP.GetPointCount();
            if (nPointAnz<2) bMakeLines=FALSE;
            for (USHORT nPoint=0; nPoint<nPointAnz; nPoint++) {
                SdrObjKind eKind=OBJ_PATHFILL;
                if (bMakeLines) {
                    eKind=OBJ_PLIN;
                    pXP=&aTmpPoly;
                    aTmpPoly.SetSize(2);
                    aTmpPoly[0]=rAktXP[nPoint];
                    aTmpPoly.SetFlags(0,XPOLY_NORMAL);
                    aTmpPoly[1]=rAktXP[nPoint+1];
                    aTmpPoly.SetFlags(1,rAktXP.GetFlags(nPoint+1));
                    if (aTmpPoly.IsControl(1) && nPoint+3<nPointAnz) { // Aha, Beziersegment
                        aTmpPoly[2]=rAktXP[nPoint+2];
                        aTmpPoly.SetFlags(2,rAktXP.GetFlags(nPoint+2));
                        aTmpPoly[3]=rAktXP[nPoint+3];
                        aTmpPoly.SetFlags(3,rAktXP.GetFlags(nPoint+3));
                        nPoint+=2;
                        eKind=OBJ_PATHLINE;
                    }
                    aTmpPoly.SetFlags(USHORT(aTmpPoly.GetPointCount()-1),XPOLY_NORMAL);
                    if (nPoint>=USHORT(nPointAnz-2)) nPoint=nPointAnz;
                } else {
                    nPoint=nPointAnz;
                    eKind=SdrObjKind(pSrcPath->GetObjIdentifier());
                }
                SdrPathObj* pPath=new SdrPathObj(eKind,XPolyPolygon(*pXP));
                ImpCopyAttributes(pSrcPath,pPath);
                pLast=pPath;
                SdrInsertReason aReason(SDRREASON_VIEWCALL,pSrcPath);
                rOL.InsertObject(pPath,rPos,&aReason);
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoNewObject(*pPath,TRUE));
                MarkObj(pPath,pPV,FALSE,TRUE);
                rPos++;
            }
        }
        if (pLast!=NULL && pSrcPath->GetOutlinerParaObject()!=NULL)
            pLast->SetOutlinerParaObject(pSrcPath->GetOutlinerParaObject()->Clone());
    }
    else if(pCustomShape)
    {
        if(bMakeLines)
        {
            // break up custom shape
            const SdrObject* pReplacement = pCustomShape->GetSdrObjectFromCustomShape();

            if(pReplacement)
            {
                SdrObject* pCandidate = pReplacement->Clone();
                DBG_ASSERT(pCandidate, "SdrEditView::ImpDismantleOneObject: Could not clone SdrObject (!)");
                pCandidate->SetModel(pCustomShape->GetModel());

                if(((SdrShadowItem&)pCustomShape->GetMergedItem(SDRATTR_SHADOW)).GetValue())
                {
                    if(pReplacement->ISA(SdrObjGroup))
                    {
                        pCandidate->SetMergedItem(SdrShadowItem(sal_True));
                    }
                }

                SdrInsertReason aReason(SDRREASON_VIEWCALL, pCustomShape);
                rOL.InsertObject(pCandidate, rPos, &aReason);
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoNewObject(*pCandidate, true));
                MarkObj(pCandidate, pPV, FALSE, TRUE);

                if(pCustomShape->HasText() && !pCustomShape->IsTextPath())
                {
                    // #i37011# also create a text object and add at rPos + 1
                    SdrTextObj* pTextObj = (SdrTextObj*)SdrObjFactory::MakeNewObject(
                        pCustomShape->GetObjInventor(), OBJ_TEXT, 0L, pCustomShape->GetModel());

                    // Copy text content
                    OutlinerParaObject* pParaObj = pCustomShape->GetOutlinerParaObject();
                    if(pParaObj)
                    {
                        pTextObj->NbcSetOutlinerParaObject(pParaObj->Clone());
                    }

                    // copy all attributes
                    SfxItemSet aTargetItemSet(pCustomShape->GetMergedItemSet());

                    // clear fill and line style
                    aTargetItemSet.Put(XLineStyleItem(XLINE_NONE));
                    aTargetItemSet.Put(XFillStyleItem(XFILL_NONE));

                    // get the text bounds and set at text object
                    Rectangle aTextBounds = pCustomShape->GetSnapRect();
                    if(pCustomShape->GetTextBounds(aTextBounds))
                    {
                        pTextObj->SetSnapRect(aTextBounds);
                    }

                    // if rotated, copy GeoStat, too.
                    const GeoStat& rSourceGeo = pCustomShape->GetGeoStat();
                    if(rSourceGeo.nDrehWink)
                    {
                        pTextObj->NbcRotate(
                            pCustomShape->GetSnapRect().Center(), rSourceGeo.nDrehWink,
                            rSourceGeo.nSin, rSourceGeo.nCos);
                    }

                    // set modified ItemSet at text object
                    pTextObj->SetMergedItemSet(aTargetItemSet);

                    // insert object
                    rOL.InsertObject(pTextObj, rPos + 1, &aReason);
                    AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoNewObject(*pTextObj, true));
                    MarkObj(pTextObj, pPV, FALSE, TRUE);
                }
            }
        }
    }
}

void SdrEditView::DismantleMarkedObjects(BOOL bMakeLines)
{
    //UINT32 nCnt(0);
    // Temporaere Marklist
    SdrMarkList aRemoveMerker;

    SortMarkedObjects();
    // Der Comment wird spaeter zusammengebaut
    BegUndo(String(), String(),
        bMakeLines ? SDRREPFUNC_OBJ_DISMANTLE_LINES : SDRREPFUNC_OBJ_DISMANTLE_POLYS);

    ULONG nm;
    ULONG nAnz=GetMarkedObjectCount();
    SdrObjList* pOL0=NULL;
    for (nm=nAnz; nm>0;) {
        nm--;
        SdrMark* pM=GetSdrMarkByIndex(nm);
        SdrObject* pObj=pM->GetObj();
        SdrPageView* pPV=pM->GetPageView();
        SdrObjList* pOL=pObj->GetObjList();
        if (pOL!=pOL0) { pOL0=pOL; pObj->GetOrdNum(); } // sicherstellen, dass OrdNums stimmen!
        if (ImpCanDismantle(pObj,bMakeLines)) {
            aRemoveMerker.InsertEntry(SdrMark(pObj,pM->GetPageView()));
            ULONG nPos0=pObj->GetOrdNumDirect();
            ULONG nPos=nPos0+1;
            SdrObjList* pSubList=pObj->GetSubList();
            if (pSubList!=NULL && !pObj->Is3DObj()) {
                SdrObjListIter aIter(*pSubList,IM_DEEPNOGROUPS);
                while (aIter.IsMore()) {
                    const SdrObject* pObj1=aIter.Next();
                    ImpDismantleOneObject(pObj1,*pOL,nPos,pPV,bMakeLines);
                }
            } else {
                ImpDismantleOneObject(pObj,*pOL,nPos,pPV,bMakeLines);
            }
            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoDeleteObject(*pObj,TRUE));
            pOL->RemoveObject(nPos0);
        }
    }
    // UndoComment aus den tatsaechlich verwendeten Objekten zusammenbauen
    SetUndoComment(ImpGetResStr(bMakeLines?STR_EditDismantle_Lines:STR_EditDismantle_Polys),aRemoveMerker.GetMarkDescription());
    // die tatsaechlich verwendeten Objekten aus der Liste entfernen
    EndUndo();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   #### ####   ###  #   # ####
//  #     #   # #   # #   # #   #
//  #  ## ####  #   # #   # ####
//  #   # #   # #   # #   # #
//   #### #   #  ###   ###  #
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrEditView::GroupMarked(const SdrObject* pUserGrp)
{
    if (AreObjectsMarked()) {
        BegUndo(ImpGetResStr(STR_EditGroup),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_GROUP);
        SortMarkedObjects();
        ULONG nAnz=GetMarkedObjectCount();
        for (ULONG nm=nAnz; nm>0;) { // UndoActions fuer alle betroffenen Objekte anlegen
            nm--;
            SdrMark* pM=GetSdrMarkByIndex(nm);
            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoRemoveObject(*pM->GetObj()));
        }

        SdrMarkList aNewMark;
        for (USHORT nv=0; nv<GetPageViewCount(); nv++) {
            SdrPageView* pPV=GetPageViewPvNum(nv);
            SdrObjList* pAktLst=pPV->GetObjList();
            SdrObjList* pSrcLst=pAktLst;
            SdrObjList* pSrcLst0=pSrcLst;
            SdrPage*    pPage=pPV->GetPage();
            // sicherstellen, dass die OrdNums stimmen
            if (pSrcLst->IsObjOrdNumsDirty()) pSrcLst->RecalcObjOrdNums();
            SdrObject*  pGrp=NULL;
            SdrObject*  pRefObj=NULL; // Referenz fuer InsertReason (-> rumankern im Writer)
            SdrObject*  pRefObj1=NULL; // Referenz fuer InsertReason (-> rumankern im Writer)
            SdrObjList* pDstLst=NULL;
            // Falls alle markierten Objekte aus Fremden Obj-Listen
            // kommen, kommt das Gruppenobjekt an das Ende der Liste.
            ULONG       nInsPos=pSrcLst->GetObjCount();
            BOOL    bNeedInsPos=TRUE;
            for (ULONG nm=GetMarkedObjectCount(); nm>0;) {
                nm--;
                SdrMark* pM=GetSdrMarkByIndex(nm);
                if (pM->GetPageView()==pPV) {
                    if (pGrp==NULL) {
                        if (pUserGrp!=NULL) pGrp=pUserGrp->Clone();
                        if (pGrp==NULL) pGrp=new SdrObjGroup;
                        pDstLst=pGrp->GetSubList();
                        DBG_ASSERT(pDstLst!=NULL,"Angebliches Gruppenobjekt liefert keine Objektliste");
                    }
                    SdrObject* pObj=pM->GetObj();
                    pSrcLst=pObj->GetObjList();
                    if (pSrcLst!=pSrcLst0) {
                        if (pSrcLst->IsObjOrdNumsDirty()) pSrcLst->RecalcObjOrdNums();
                    }
                    BOOL bForeignList=pSrcLst!=pAktLst;
                    BOOL bGrouped=pSrcLst!=pPage;
                    if (!bForeignList && bNeedInsPos) {
                        nInsPos=pObj->GetOrdNum(); // ua, damit sind alle ObjOrdNum der Page gesetzt
                        nInsPos++;
                        bNeedInsPos=FALSE;
                    }
                    pSrcLst->RemoveObject(pObj->GetOrdNumDirect());
                    if (!bForeignList) nInsPos--; // InsertPos korregieren
                    SdrInsertReason aReason(SDRREASON_VIEWCALL);
                    pDstLst->InsertObject(pObj,0,&aReason);
                    GetMarkedObjectListWriteAccess().DeleteMark(nm);
                    if (pRefObj1==NULL) pRefObj1=pObj; // Das oberste sichtbare Objekt
                    if (!bGrouped) {
                        if (pRefObj==NULL) pRefObj=pObj; // Das oberste sichtbare nicht gruppierte Objekt
                    }
                    pSrcLst0=pSrcLst;
                }
            }
            if (pRefObj==NULL) pRefObj=pRefObj1;
            if (pGrp!=NULL) {
                aNewMark.InsertEntry(SdrMark(pGrp,pPV));
                ULONG nAnz2=pDstLst->GetObjCount();
                SdrInsertReason aReason(SDRREASON_VIEWCALL,pRefObj);
                pAktLst->InsertObject(pGrp,nInsPos,&aReason);
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoNewObject(*pGrp,true)); // Kein Recalc!
                for (ULONG no=0; no<nAnz2; no++) {
                    AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoInsertObject(*pDstLst->GetObj(no)));
                }
            }
        }
        GetMarkedObjectListWriteAccess().Merge(aNewMark);
        MarkListHasChanged();
        EndUndo();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  #   # #   #  #### ####   ###  #   # ####
//  #   # ##  # #     #   # #   # #   # #   #
//  #   # # # # #  ## ####  #   # #   # ####
//  #   # #  ## #   # #   # #   # #   # #
//   ###  #   #  #### #   #  ###   ###  #
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrEditView::UnGroupMarked()
{
    SdrMarkList aNewMark;

    BegUndo(String(), String(), SDRREPFUNC_OBJ_UNGROUP);
    ULONG nCount=0;
    XubString aName1;
    XubString aName;
    BOOL bNameOk=FALSE;
    for (ULONG nm=GetMarkedObjectCount(); nm>0;) {
        nm--;
        SdrMark* pM=GetSdrMarkByIndex(nm);
        SdrObject* pGrp=pM->GetObj();
        SdrObjList* pSrcLst=pGrp->GetSubList();
        if (pSrcLst!=NULL) {
            nCount++;
            if (nCount==1) {
                pGrp->TakeObjNameSingul(aName);  // Bezeichnung der Gruppe holen
                pGrp->TakeObjNamePlural(aName1); // Bezeichnung der Gruppe holen
                bNameOk=TRUE;
            } else {
                if (nCount==2) aName=aName1; // Pluralname setzen
                if (bNameOk) {
                    XubString aStr;
                    pGrp->TakeObjNamePlural(aStr); // Bezeichnung der Gruppe holen

                    if(!aStr.Equals(aName))
                        bNameOk = FALSE;
                }
            }
            ULONG nDstCnt=pGrp->GetOrdNum();
            SdrObjList* pDstLst=pM->GetPageView()->GetObjList();

            // FIRST move contained objects to parent of group, so that
            // the contained objects are NOT migrated to the UNDO-ItemPool
            // when AddUndo(new SdrUndoDelObj(*pGrp)) is called.
            ULONG nAnz=pSrcLst->GetObjCount();
            ULONG no;
            for (no=nAnz; no>0;) {
                no--;
                SdrObject* pObj=pSrcLst->GetObj(no);
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoRemoveObject(*pObj));
            }
            for (no=0; no<nAnz; no++) {
                SdrObject* pObj=pSrcLst->RemoveObject(0);
                SdrInsertReason aReason(SDRREASON_VIEWCALL,pGrp);
                pDstLst->InsertObject(pObj,nDstCnt,&aReason);
                AddUndo( GetModel()->GetSdrUndoFactory().CreateUndoInsertObject(*pObj,true));
                nDstCnt++;
                // Kein SortCheck beim einfuegen in die MarkList, denn das
                // wuerde wg. pObj->GetOrdNum() jedesmal ein RecalcOrdNums()
                // provozieren:
                aNewMark.InsertEntry(SdrMark(pObj,pM->GetPageView()),FALSE);
            }

            // Now it is safe to add the delete-UNDO which trigers the
            // MigrateItemPool now only for itself, not for the subobjects.
            // nDstCnt is right, because previous inserts move group
            // object deeper and increase nDstCnt.
            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoDeleteObject(*pGrp));
            pDstLst->RemoveObject(nDstCnt);

            GetMarkedObjectListWriteAccess().DeleteMark(nm);
        }
    }
    if (nCount!=0)
    {
        if (!bNameOk) aName=ImpGetResStr(STR_ObjNamePluralGRUP); // Oberbegriff Gruppenobjekte verwenden, wenn verschiedene Objekte.
        SetUndoComment(ImpGetResStr(STR_EditUngroup),aName);
    }
    EndUndo();
    if (nCount!=0)
    {
        GetMarkedObjectListWriteAccess().Merge(aNewMark,TRUE); // Durch das obige Einsortieren ist aNewMark genau verkehrtherum
        MarkListHasChanged();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   ###   ###  #   # #   # ##### ####  #####   #####  ###    ####   ###  #  #   #
//  #   # #   # ##  # #   # #     #   #   #       #   #   #   #   # #   # #   # #
//  #     #   # # # # #   # ####  ####    #       #   #   #   ####  #   # #    #
//  #   # #   # #  ##  # #  #     #   #   #       #   #   #   #     #   # #    #
//   ###   ###  #   #   #   ##### #   #   #       #    ###    #      ###  #### #
//
////////////////////////////////////////////////////////////////////////////////////////////////////

SdrObject* SdrEditView::ImpConvertOneObj(SdrObject* pObj, BOOL bPath, BOOL bLineToArea)
{
    SdrObject* pNewObj = pObj->ConvertToPolyObj(bPath, bLineToArea);
    if (pNewObj!=NULL) {
        SdrObjList* pOL=pObj->GetObjList();
        DBG_ASSERT(pOL!=NULL,"ConvertTo: Obj liefert keine ObjList");
        if (pOL!=NULL) {
            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoReplaceObject(*pObj,*pNewObj));
            pOL->ReplaceObject(pNewObj,pObj->GetOrdNum());
        }
    }
    return pNewObj;
}

void SdrEditView::ImpConvertTo(BOOL bPath, BOOL bLineToArea)
{
    BOOL bMrkChg=FALSE;
    BOOL bModChg=FALSE;
    if (AreObjectsMarked()) {
        ULONG nMarkAnz=GetMarkedObjectCount();
        USHORT nDscrID=0;
        if(bLineToArea)
        {
            if(nMarkAnz == 1)
                nDscrID = STR_EditConvToContour;
            else
                nDscrID = STR_EditConvToContours;

            BegUndo(ImpGetResStr(nDscrID), GetDescriptionOfMarkedObjects());
        }
        else
        {
            if (bPath) {
                if (nMarkAnz==1) nDscrID=STR_EditConvToCurve;
                else nDscrID=STR_EditConvToCurves;
                BegUndo(ImpGetResStr(nDscrID),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_CONVERTTOPATH);
            } else {
                if (nMarkAnz==1) nDscrID=STR_EditConvToPoly;
                else nDscrID=STR_EditConvToPolys;
                BegUndo(ImpGetResStr(nDscrID),GetDescriptionOfMarkedObjects(),SDRREPFUNC_OBJ_CONVERTTOPOLY);
            }
        }
        for (ULONG nm=nMarkAnz; nm>0;) {
            nm--;
            SdrMark* pM=GetSdrMarkByIndex(nm);
            SdrObject* pObj=pM->GetObj();
            SdrPageView* pPV=pM->GetPageView();
            if (pObj->IsGroupObject() && !pObj->Is3DObj()) {
                SdrObject* pGrp=pObj;
                SdrObjListIter aIter(*pGrp,IM_DEEPNOGROUPS);
                while (aIter.IsMore()) {
                    pObj=aIter.Next();
                    if (ImpConvertOneObj(pObj,bPath,bLineToArea)) bModChg=TRUE;
                }
            } else {
                SdrObject* pNewObj=ImpConvertOneObj(pObj,bPath,bLineToArea);
                if (pNewObj!=NULL) {
                    bModChg=TRUE;
                    bMrkChg=TRUE;
                    GetMarkedObjectListWriteAccess().ReplaceMark(SdrMark(pNewObj,pPV),nm);
                }
            }
        }
        EndUndo();
        if (bMrkChg) AdjustMarkHdl();
        if (bMrkChg) MarkListHasChanged();
    }
}

void SdrEditView::ConvertMarkedToPathObj(BOOL bLineToArea)
{
    ImpConvertTo(TRUE, bLineToArea);
}

void SdrEditView::ConvertMarkedToPolyObj(BOOL bLineToArea)
{
    ImpConvertTo(FALSE, bLineToArea);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  #   # ##### #####  ###  ##### # #    #####      # #   # ####   ###  ####  #####
//  ## ## #       #   #   # #     # #    #          # ## ## #   # #   # #   #   #
//  # # # ####    #   ##### ###   # #    ####  ###  # # # # ####  #   # ####    #
//  #   # #       #   #   # #     # #    #          # #   # #     #   # #   #   #
//  #   # #####   #   #   # #     # #### #####      # #   # #      ###  #   #   #
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrEditView::DoImportMarkedMtf(SvdProgressInfo *pProgrInfo)
{
    BegUndo(String(), String(), SDRREPFUNC_OBJ_IMPORTMTF);
    SortMarkedObjects();
    SdrMarkList aForTheDescription;
    SdrMarkList aNewMarked;
    ULONG nAnz=GetMarkedObjectCount();

    for (ULONG nm=nAnz; nm>0;)
    { // Undo Objekte fuer alle neuen Objekte erzeugen
        // zwischen den Metafiles auf Abbruch testen
        if( pProgrInfo != NULL )
        {
            pProgrInfo->SetNextObject();
            if(!pProgrInfo->ReportActions(0))
                break;
        }

        nm--;
        SdrMark*     pM=GetSdrMarkByIndex(nm);
        SdrObject*   pObj=pM->GetObj();
        SdrPageView* pPV=pM->GetPageView();
        SdrObjList*  pOL=pObj->GetObjList();
        ULONG        nInsPos=pObj->GetOrdNum()+1;
        SdrGrafObj*  pGraf=PTR_CAST(SdrGrafObj,pObj);
        SdrOle2Obj*  pOle2=PTR_CAST(SdrOle2Obj,pObj);
        ULONG        nInsAnz=0;
        if (pGraf!=NULL && pGraf->HasGDIMetaFile())
        {
            ImpSdrGDIMetaFileImport aFilter(*pMod);
            aFilter.SetScaleRect(pGraf->GetSnapRect());
            aFilter.SetLayer(pObj->GetLayer());
            nInsAnz=aFilter.DoImport(pGraf->GetTransformedGraphic().GetGDIMetaFile(),*pOL,nInsPos,pProgrInfo);
        }
        if ( pOle2!=NULL && pOle2->GetGraphic() )
        {
            //const GDIMetaFile* pMtf=pOle2->GetGDIMetaFile();
            ImpSdrGDIMetaFileImport aFilter(*pMod);
            aFilter.SetScaleRect(pOle2->GetLogicRect());
            aFilter.SetLayer(pObj->GetLayer());
            nInsAnz=aFilter.DoImport(pOle2->GetGraphic()->GetGDIMetaFile(),*pOL,nInsPos,pProgrInfo);
        }
        if (nInsAnz!=0) {
            ULONG nObj=nInsPos;
            for (ULONG i=0; i<nInsAnz; i++) {
                AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoNewObject(*pOL->GetObj(nObj)));

                // Neue MarkList pflegen
                SdrMark aNewMark(pOL->GetObj(nObj), pPV);
                aNewMarked.InsertEntry(aNewMark);

                nObj++;
            }
            aForTheDescription.InsertEntry(*pM);
            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoDeleteObject(*pObj));

            // Objekt aus selektion loesen und loeschen
            GetMarkedObjectListWriteAccess().DeleteMark(TryToFindMarkedObject(pObj));
            pOL->RemoveObject(nInsPos-1);
        }
    }

    // MarkObj... fehlt... jetzt nicht mehr (AW)
    if(aNewMarked.GetMarkCount())
    {
        // Neue Selektion bilden
        for(sal_uInt32 a(0); a < aNewMarked.GetMarkCount(); a++)
        {
            GetMarkedObjectListWriteAccess().InsertEntry(*aNewMarked.GetMark(a));
        }

        SortMarkedObjects();
    }

    SetUndoComment(ImpGetResStr(STR_EditImportMtf),aForTheDescription.GetMarkDescription());
    EndUndo();
}

