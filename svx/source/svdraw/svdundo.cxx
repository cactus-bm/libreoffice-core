/*************************************************************************
 *
 *  $RCSfile: svdundo.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-27 15:04:41 $
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

#include "svdundo.hxx"
#include "svditext.hxx"
#include "svdotext.hxx"
#include "svdobj.hxx"
#include "svdpage.hxx"
#include "svdlayer.hxx"
#include "svdmodel.hxx"
#include "svdview.hxx"
#include "svdstr.hrc"   // Namen aus der Resource
#include "svdglob.hxx"  // StringCache
#include "scene3d.hxx"

#ifndef _OUTLOBJ_HXX //autogen
#include <outlobj.hxx>
#endif

// #i11426#
#ifndef _SVDOGRP_HXX
#include <svdogrp.hxx>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEINIT1(SdrUndoAction,SfxUndoAction);

BOOL SdrUndoAction::CanRepeat(SfxRepeatTarget& rView) const
{
    SdrView* pV=PTR_CAST(SdrView,&rView);
    if (pV!=NULL) return CanSdrRepeat(*pV);
    return FALSE;
}

void SdrUndoAction::Repeat(SfxRepeatTarget& rView)
{
    SdrView* pV=PTR_CAST(SdrView,&rView);
    if (pV!=NULL) SdrRepeat(*pV);
    DBG_ASSERT(pV!=NULL,"Repeat: Uebergebenes SfxRepeatTarget ist keine SdrView");
}

XubString SdrUndoAction::GetRepeatComment(SfxRepeatTarget& rView) const
{
    SdrView* pV=PTR_CAST(SdrView,&rView);
    if (pV!=NULL) return GetSdrRepeatComment(*pV);
    return String();
}

FASTBOOL SdrUndoAction::CanSdrRepeat(SdrView& rView) const
{
    return FALSE;
}

void SdrUndoAction::SdrRepeat(SdrView& rView)
{
}

XubString SdrUndoAction::GetSdrRepeatComment(SdrView& rView) const
{
    return String();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoGroup::SdrUndoGroup(SdrModel& rNewMod)
:   SdrUndoAction(rNewMod),
    aBuf(1024,32,32),
    eFunction(SDRREPFUNC_OBJ_NONE)      /*#72642#*/
{}

SdrUndoGroup::SdrUndoGroup(SdrModel& rNewMod,const String& rStr)
:   SdrUndoAction(rNewMod),
    aBuf(1024,32,32),
    aComment(rStr),
    eFunction(SDRREPFUNC_OBJ_NONE)
{}

SdrUndoGroup::~SdrUndoGroup()
{
    Clear();
}

void SdrUndoGroup::Clear()
{
    for (ULONG nu=0; nu<GetActionCount(); nu++) {
        SdrUndoAction* pAct=GetAction(nu);
        delete pAct;
    }
    aBuf.Clear();
}

void SdrUndoGroup::AddAction(SdrUndoAction* pAct)
{
    aBuf.Insert(pAct,CONTAINER_APPEND);
}

void SdrUndoGroup::Undo()
{
    for (ULONG nu=GetActionCount(); nu>0;) {
        nu--;
        SdrUndoAction* pAct=GetAction(nu);
        pAct->Undo();
    }
}

void SdrUndoGroup::Redo()
{
    for (ULONG nu=0; nu<GetActionCount(); nu++) {
        SdrUndoAction* pAct=GetAction(nu);
        pAct->Redo();
    }
}

XubString SdrUndoGroup::GetComment() const
{
    XubString aRet(aComment);
    sal_Char aSearchText[] = "%O";
    String aSearchString(aSearchText, sizeof(aSearchText-1));

    aRet.SearchAndReplace(aSearchString, aObjDescription);

    return aRet;
}

FASTBOOL SdrUndoGroup::CanSdrRepeat(SdrView& rView) const
{
    switch (eFunction) {
        case SDRREPFUNC_OBJ_NONE            :  return FALSE;
        case SDRREPFUNC_OBJ_DELETE          :  return rView.HasMarkedObj();
        case SDRREPFUNC_OBJ_COMBINE_POLYPOLY:  return rView.IsCombinePossible(FALSE);
        case SDRREPFUNC_OBJ_COMBINE_ONEPOLY :  return rView.IsCombinePossible(TRUE);
        case SDRREPFUNC_OBJ_DISMANTLE_POLYS :  return rView.IsDismantlePossible(FALSE);
        case SDRREPFUNC_OBJ_DISMANTLE_LINES :  return rView.IsDismantlePossible(TRUE);
        case SDRREPFUNC_OBJ_CONVERTTOPOLY   :  return rView.IsConvertToPolyObjPossible(FALSE);
        case SDRREPFUNC_OBJ_CONVERTTOPATH   :  return rView.IsConvertToPathObjPossible(FALSE);
        case SDRREPFUNC_OBJ_GROUP           :  return rView.IsGroupPossible();
        case SDRREPFUNC_OBJ_UNGROUP         :  return rView.IsUnGroupPossible();
        case SDRREPFUNC_OBJ_PUTTOTOP        :  return rView.IsToTopPossible();
        case SDRREPFUNC_OBJ_PUTTOBTM        :  return rView.IsToBtmPossible();
        case SDRREPFUNC_OBJ_MOVTOTOP        :  return rView.IsToTopPossible();
        case SDRREPFUNC_OBJ_MOVTOBTM        :  return rView.IsToBtmPossible();
        case SDRREPFUNC_OBJ_REVORDER        :  return rView.IsReverseOrderPossible();
        case SDRREPFUNC_OBJ_IMPORTMTF       :  return rView.IsImportMtfPossible();
    } // switch
    return FALSE;
}

void SdrUndoGroup::SdrRepeat(SdrView& rView)
{
    switch (eFunction) {
        case SDRREPFUNC_OBJ_NONE            :  break;
        case SDRREPFUNC_OBJ_DELETE          :  rView.DeleteMarked();                break;
        case SDRREPFUNC_OBJ_COMBINE_POLYPOLY:  rView.CombineMarkedObjects(FALSE);   break;
        case SDRREPFUNC_OBJ_COMBINE_ONEPOLY :  rView.CombineMarkedObjects(TRUE);    break;
        case SDRREPFUNC_OBJ_DISMANTLE_POLYS :  rView.DismantleMarkedObjects(FALSE); break;
        case SDRREPFUNC_OBJ_DISMANTLE_LINES :  rView.DismantleMarkedObjects(TRUE);  break;
        case SDRREPFUNC_OBJ_CONVERTTOPOLY   :  rView.ConvertMarkedToPolyObj(FALSE); break;
        case SDRREPFUNC_OBJ_CONVERTTOPATH   :  rView.ConvertMarkedToPathObj(FALSE); break;
        case SDRREPFUNC_OBJ_GROUP           :  rView.GroupMarked();                 break;
        case SDRREPFUNC_OBJ_UNGROUP         :  rView.UnGroupMarked();               break;
        case SDRREPFUNC_OBJ_PUTTOTOP        :  rView.PutMarkedToTop();              break;
        case SDRREPFUNC_OBJ_PUTTOBTM        :  rView.PutMarkedToBtm();              break;
        case SDRREPFUNC_OBJ_MOVTOTOP        :  rView.MovMarkedToTop();              break;
        case SDRREPFUNC_OBJ_MOVTOBTM        :  rView.MovMarkedToBtm();              break;
        case SDRREPFUNC_OBJ_REVORDER        :  rView.ReverseOrderOfMarked();        break;
        case SDRREPFUNC_OBJ_IMPORTMTF       :  rView.DoImportMarkedMtf();           break;
    } // switch
}

XubString SdrUndoGroup::GetSdrRepeatComment(SdrView& rView) const
{
    XubString aRet(aComment);
    sal_Char aSearchText[] = "%O";
    String aSearchString(aSearchText, sizeof(aSearchText-1));

    aRet.SearchAndReplace(aSearchString, ImpGetResStr(STR_ObjNameSingulPlural));

    return aRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   @@@@   @@@@@   @@@@@@  @@@@@   @@@@   @@@@@@   @@@@
//  @@  @@  @@  @@      @@  @@     @@  @@    @@    @@  @@
//  @@  @@  @@  @@      @@  @@     @@        @@    @@
//  @@  @@  @@@@@       @@  @@@@   @@        @@     @@@@
//  @@  @@  @@  @@      @@  @@     @@        @@        @@
//  @@  @@  @@  @@  @@  @@  @@     @@  @@    @@    @@  @@
//   @@@@   @@@@@    @@@@   @@@@@   @@@@     @@     @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoObj::SdrUndoObj(SdrObject& rNewObj):
    SdrUndoAction(*rNewObj.GetModel()),
    pObj(&rNewObj)
{
}

void SdrUndoObj::ImpTakeDescriptionStr(USHORT nStrCacheID, XubString& rStr, FASTBOOL bRepeat) const
{
    rStr = ImpGetResStr(nStrCacheID);
    sal_Char aSearchText[] = "%O";
    String aSearchString(aSearchText, sizeof(aSearchText-1));

    xub_StrLen nPos = rStr.Search(aSearchString);

    if(nPos != STRING_NOTFOUND)
    {
        rStr.Erase(nPos, 2);

        if(bRepeat)
        {
            rStr.Insert(ImpGetResStr(STR_ObjNameSingulPlural), nPos);
        }
        else
        {
            XubString aStr;

            pObj->TakeObjNameSingul(aStr);
            rStr.Insert(aStr, nPos);
        }
    }
}

// #94278# common call method for evtl. page change when UNDO/REDO
// is triggered
void SdrUndoObj::ImpShowPageOfThisObject()
{
    if(pObj && pObj->IsInserted() && pObj->GetPage() && pObj->GetModel())
    {
        SdrHint aHint(HINT_SWITCHTOPAGE);

        aHint.SetObject(pObj);
        aHint.SetPage(pObj->GetPage());

        pObj->GetModel()->Broadcast(aHint);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoAttrObj::SdrUndoAttrObj(SdrObject& rNewObj, FASTBOOL bStyleSheet1, FASTBOOL bSaveText):
    SdrUndoObj(rNewObj),
    pUndoSet(NULL),
    pRedoSet(NULL),
    pRepeatSet(NULL),
    pUndoStyleSheet(NULL),
    pRedoStyleSheet(NULL),
    pRepeatStyleSheet(NULL),
    pTextUndo(NULL),

    // #i8508#
    pTextRedo(NULL),

    pUndoGroup(NULL),
    bHaveToTakeRedoSet(TRUE)
{
    bStyleSheet = bStyleSheet1;
    pUndoSet = rNewObj.CreateNewItemSet(*((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()));
    pRedoSet = rNewObj.CreateNewItemSet(*((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()));

    SdrObjList* pOL = rNewObj.GetSubList();
    BOOL bIsGroup(pOL!=NULL && pOL->GetObjCount());
    BOOL bIs3DScene(bIsGroup && pObj->ISA(E3dScene));

    if(bIsGroup)
    {
        // Aha, Gruppenobjekt
        pUndoGroup = new SdrUndoGroup(*pObj->GetModel());
        sal_uInt32 nObjAnz(pOL->GetObjCount());

        for(sal_uInt32 nObjNum(0); nObjNum < nObjAnz; nObjNum++)
        {
            pUndoGroup->AddAction(
                new SdrUndoAttrObj(*pOL->GetObj(nObjNum), bStyleSheet1));
        }
    }

    if(!bIsGroup || bIs3DScene)
    {
        pUndoSet->Put(pObj->GetItemSet());

        if(bStyleSheet)
            pUndoStyleSheet = pObj->GetStyleSheet();

        if(bSaveText)
        {
            pTextUndo = pObj->GetOutlinerParaObject();
            if(pTextUndo)
                pTextUndo = pTextUndo->Clone();
        }
    }
}

SdrUndoAttrObj::~SdrUndoAttrObj()
{
    if(pUndoSet)
        delete pUndoSet;
    if(pRedoSet)
        delete pRedoSet;
    if(pRepeatSet)
        delete pRepeatSet;
    if(pUndoGroup)
        delete pUndoGroup;
    if(pTextUndo)
        delete pTextUndo;

    // #i8508#
    if(pTextRedo)
        delete pTextRedo;
}

void SdrUndoAttrObj::SetRepeatAttr(const SfxItemSet& rSet)
{
    if(pRepeatSet)
        delete pRepeatSet;

    pRepeatSet = pObj->CreateNewItemSet(*((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()));

    pRepeatSet->Put(rSet);
}

void SdrUndoAttrObj::Undo()
{
    BOOL bIs3DScene(pObj && pObj->ISA(E3dScene));

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    if(!pUndoGroup || bIs3DScene)
    {
        if(bHaveToTakeRedoSet)
        {
            bHaveToTakeRedoSet = FALSE;
            pRedoSet->Put(pObj->GetItemSet());
            if(bStyleSheet)
                pRedoStyleSheet=pObj->GetStyleSheet();

            if(pTextUndo)
            {
                // #i8508#
                pTextRedo = pObj->GetOutlinerParaObject();

                if(pTextRedo)
                    pTextRedo = pTextRedo->Clone();
            }
        }

        if(bStyleSheet)
        {
            pRedoStyleSheet = pObj->GetStyleSheet();
            pObj->SetStyleSheet(pUndoStyleSheet, TRUE);
        }

        SdrBroadcastItemChange aItemChange(*pObj);

        // #105122# Since ClearItem sets back everything to normal
        // it also sets fit-to-size text to non-fit-to-size text and
        // switches on autogrowheight (the default). That may lead to
        // loosing the geometry size info for the object when it is
        // re-layouted from AdjustTextFrameWidthAndHeight(). This makes
        // rescuing the size of the object necessary.
        const Rectangle aSnapRect = pObj->GetSnapRect();

        pObj->ClearItem();
        pObj->SetItemSet(*pUndoSet);

        // #105122# Restore prev size here when it was changed.
        if(aSnapRect != pObj->GetSnapRect())
        {
            pObj->NbcSetSnapRect(aSnapRect);
        }

        pObj->BroadcastItemChange(aItemChange);

        if(pTextUndo)
        {
            pObj->SetOutlinerParaObject(pTextUndo->Clone());
        }
    }

    if(pUndoGroup)
    {
        pUndoGroup->Undo();
    }
}

void SdrUndoAttrObj::Redo()
{
    BOOL bIs3DScene(pObj && pObj->ISA(E3dScene));

    if(!pUndoGroup || bIs3DScene)
    {
        if(bStyleSheet)
        {
            pUndoStyleSheet = pObj->GetStyleSheet();
            pObj->SetStyleSheet(pRedoStyleSheet, TRUE);
        }

        SdrBroadcastItemChange aItemChange(*pObj);

        // #105122#
        const Rectangle aSnapRect = pObj->GetSnapRect();

        pObj->ClearItem();
        pObj->SetItemSet(*pRedoSet);

        // #105122# Restore prev size here when it was changed.
        if(aSnapRect != pObj->GetSnapRect())
        {
            pObj->NbcSetSnapRect(aSnapRect);
        }

        pObj->BroadcastItemChange(aItemChange);

        // #i8508#
        if(pTextRedo)
        {
            pObj->SetOutlinerParaObject(pTextRedo->Clone());
        }
    }

    if(pUndoGroup)
    {
        pUndoGroup->Redo();
    }

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

XubString SdrUndoAttrObj::GetComment() const
{
    XubString aStr;

    if(bStyleSheet)
    {
        ImpTakeDescriptionStr(STR_EditSetStylesheet, aStr);
    }
    else
    {
        ImpTakeDescriptionStr(STR_EditSetAttributes, aStr);
    }

    return aStr;
}

void SdrUndoAttrObj::SdrRepeat(SdrView& rView)
{
    if(pRepeatSet)
    {
        rView.SetAttrToMarked(*pRepeatSet, FALSE);
    }
}

FASTBOOL SdrUndoAttrObj::CanSdrRepeat(SdrView& rView) const
{
    return (pRepeatSet!=0L && rView.HasMarkedObj());
}

XubString SdrUndoAttrObj::GetSdrRepeatComment(SdrView& rView) const
{
    XubString aStr;

    if(bStyleSheet)
    {
        ImpTakeDescriptionStr(STR_EditSetStylesheet, aStr, TRUE);
    }
    else
    {
        ImpTakeDescriptionStr(STR_EditSetAttributes, aStr, TRUE);
    }

    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoMoveObj::Undo()
{
    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    pObj->Move(Size(-aDistance.Width(),-aDistance.Height()));
}

void SdrUndoMoveObj::Redo()
{
    pObj->Move(Size(aDistance.Width(),aDistance.Height()));

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

XubString SdrUndoMoveObj::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_EditMove,aStr);
    return aStr;
}

void SdrUndoMoveObj::SdrRepeat(SdrView& rView)
{
    rView.MoveMarkedObj(aDistance);
}

FASTBOOL SdrUndoMoveObj::CanSdrRepeat(SdrView& rView) const
{
    return rView.HasMarkedObj();
}

XubString SdrUndoMoveObj::GetSdrRepeatComment(SdrView& rView) const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_EditMove,aStr,TRUE);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoGeoObj::SdrUndoGeoObj(SdrObject& rNewObj):
    SdrUndoObj(rNewObj),
    pUndoGeo(NULL),
    pRedoGeo(NULL),
    pUndoGroup(NULL)
{
    SdrObjList* pOL=rNewObj.GetSubList();
    if (pOL!=NULL && pOL->GetObjCount() && !rNewObj.ISA(E3dScene))
    {
        // Aha, Gruppenobjekt
        // AW: Aber keine 3D-Szene, dann nur fuer die Szene selbst den Undo anlegen
        pUndoGroup=new SdrUndoGroup(*pObj->GetModel());
        ULONG nObjAnz=pOL->GetObjCount();
        for (ULONG nObjNum=0; nObjNum<nObjAnz; nObjNum++) {
            pUndoGroup->AddAction(new SdrUndoGeoObj(*pOL->GetObj(nObjNum)));
        }
    } else {
        pUndoGeo=pObj->GetGeoData();
    }
}

SdrUndoGeoObj::~SdrUndoGeoObj()
{
    if (pUndoGeo!=NULL) delete pUndoGeo;
    if (pRedoGeo!=NULL) delete pRedoGeo;
    if (pUndoGroup!=NULL) delete pUndoGroup;
}

void SdrUndoGeoObj::Undo()
{
    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    if(pUndoGroup)
    {
        // #97172#
        pObj->SendRepaintBroadcast();

        pUndoGroup->Undo();

        // #97172#
        pObj->SendRepaintBroadcast();
    }
    else
    {
        if (pRedoGeo!=NULL) delete pRedoGeo;
        pRedoGeo=pObj->GetGeoData();
        pObj->SetGeoData(*pUndoGeo);
    }
}

void SdrUndoGeoObj::Redo()
{
    if(pUndoGroup)
    {
        // #97172#
        pObj->SendRepaintBroadcast();

        pUndoGroup->Redo();

        // #97172#
        pObj->SendRepaintBroadcast();
    }
    else
    {
        if (pUndoGeo!=NULL) delete pUndoGeo;
        pUndoGeo=pObj->GetGeoData();
        pObj->SetGeoData(*pRedoGeo);
    }

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

XubString SdrUndoGeoObj::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_DragMethObjOwn,aStr);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoObjList::SdrUndoObjList(SdrObject& rNewObj, FASTBOOL bOrdNumDirect):
    pView(NULL),
    pPageView(NULL),
    SdrUndoObj(rNewObj),
    bOwner(FALSE)
{
    pObjList=pObj->GetObjList();
    if (bOrdNumDirect) {
        nOrdNum=pObj->GetOrdNumDirect();
    } else {
        nOrdNum=pObj->GetOrdNum();
    }
}

SdrUndoObjList::~SdrUndoObjList()
{
    if (pObj!=NULL && IsOwner())
    {
        // Attribute muessen wieder in den regulaeren Pool
        SetOwner(FALSE);

        // nun loeschen
        delete pObj;
    }
}

void SdrUndoObjList::SetOwner(BOOL bNew)
{
    if(bNew != bOwner)
    {
        // Besitzuebergang des Objektes. Hier muss auch die Speicherung der
        // Items des Objektes zwischen dem allgemeinen Pool und dem Pool des
        // Undo-Managers wechseln
        if(bNew)
        {
            pObj->MigrateItemPool(&rMod.GetItemPool(), ((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()));
        }
        else
        {
            pObj->MigrateItemPool(((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()), &rMod.GetItemPool());
            pObj->SetStyleSheet(pObj->GetStyleSheet(), TRUE);
        }

        // umsetzen
        bOwner = bNew;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoRemoveObj::Undo()
{
    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    DBG_ASSERT(!pObj->IsInserted(),"UndoRemoveObj: pObj ist bereits Inserted");
    if (!pObj->IsInserted())
    {
        // #i11426#
        // For UNDOs in Calc/Writer it is necessary to adapt the anchor
        // pos of the target object.
        Point aOwnerAnchorPos(0, 0);

        if(pObjList
            && pObjList->GetOwnerObj()
            && pObjList->GetOwnerObj()->ISA(SdrObjGroup))
        {
            aOwnerAnchorPos = pObjList->GetOwnerObj()->GetAnchorPos();
        }

        SdrInsertReason aReason(SDRREASON_UNDO);
        pObjList->InsertObject(pObj,nOrdNum,&aReason);

        // #i11426#
        if(aOwnerAnchorPos.X() || aOwnerAnchorPos.Y())
        {
            pObj->NbcSetAnchorPos(aOwnerAnchorPos);
        }

        if(pObjList->GetOwnerObj() && pObjList->GetOwnerObj()->ISA(E3dObject) && pObj->ISA(E3dObject))
        {
            E3dScene* pScene = ((E3dObject*)pObjList->GetOwnerObj())->GetScene();
            if(pScene)
                pScene->CorrectSceneDimensions();
        }
    }
}

void SdrUndoRemoveObj::Redo()
{
    DBG_ASSERT(pObj->IsInserted(),"RedoRemoveObj: pObj ist nicht Inserted");
    if (pObj->IsInserted()) {
        SdrObject* pChkObj=pObjList->RemoveObject(nOrdNum);
        DBG_ASSERT(pChkObj==pObj,"RedoRemoveObj: RemoveObjNum!=pObj");
        if(pObjList->GetOwnerObj() && pObjList->GetOwnerObj()->ISA(E3dObject) && pObj->ISA(E3dObject))
        {
            E3dScene* pScene = ((E3dObject*)pObjList->GetOwnerObj())->GetScene();
            if(pScene)
                pScene->CorrectSceneDimensions();
        }
    }

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoInsertObj::Undo()
{
    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    DBG_ASSERT(pObj->IsInserted(),"UndoInsertObj: pObj ist nicht Inserted");
    if (pObj->IsInserted()) {
        SdrObject* pChkObj=pObjList->RemoveObject(nOrdNum);
        DBG_ASSERT(pChkObj==pObj,"UndoInsertObj: RemoveObjNum!=pObj");
        if(pObjList->GetOwnerObj() && pObjList->GetOwnerObj()->ISA(E3dObject) && pObj->ISA(E3dObject))
        {
            E3dScene* pScene = ((E3dObject*)pObjList->GetOwnerObj())->GetScene();
            if(pScene)
                pScene->CorrectSceneDimensions();
        }
    }
}

void SdrUndoInsertObj::Redo()
{
    DBG_ASSERT(!pObj->IsInserted(),"RedoInsertObj: pObj ist bereits Inserted");
    if (!pObj->IsInserted()) {
        SdrInsertReason aReason(SDRREASON_UNDO);
        pObjList->InsertObject(pObj,nOrdNum,&aReason);
        if(pObjList->GetOwnerObj() && pObjList->GetOwnerObj()->ISA(E3dObject) && pObj->ISA(E3dObject))
        {
            E3dScene* pScene = ((E3dObject*)pObjList->GetOwnerObj())->GetScene();
            if(pScene)
                pScene->CorrectSceneDimensions();
        }
    }

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoDelObj::Undo()
{
    SdrUndoRemoveObj::Undo();
    DBG_ASSERT(IsOwner(),"UndoDeleteObj: pObj gehoert nicht der UndoAction");
    SetOwner(FALSE);
}

void SdrUndoDelObj::Redo()
{
    SdrUndoRemoveObj::Redo();
    DBG_ASSERT(!IsOwner(),"RedoDeleteObj: pObj gehoert bereits der UndoAction");
    SetOwner(TRUE);
}

XubString SdrUndoDelObj::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_EditDelete,aStr);
    return aStr;
}

void SdrUndoDelObj::SdrRepeat(SdrView& rView)
{
    rView.DeleteMarked();
}

FASTBOOL SdrUndoDelObj::CanSdrRepeat(SdrView& rView) const
{
    return rView.HasMarkedObj();
}

XubString SdrUndoDelObj::GetSdrRepeatComment(SdrView& rView) const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_EditDelete,aStr,TRUE);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoNewObj::Undo()
{
    SdrUndoInsertObj::Undo();
    DBG_ASSERT(!IsOwner(),"RedoNewObj: pObj gehoert bereits der UndoAction");
    SetOwner(TRUE);
}

void SdrUndoNewObj::Redo()
{
    SdrUndoInsertObj::Redo();
    DBG_ASSERT(IsOwner(),"RedoNewObj: pObj gehoert nicht der UndoAction");
    SetOwner(FALSE);
}

XubString SdrUndoNewObj::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoInsertObj,aStr);
    return aStr;
}

SdrUndoReplaceObj::SdrUndoReplaceObj(SdrObject& rOldObj1, SdrObject& rNewObj1, FASTBOOL bOrdNumDirect):
    SdrUndoObj(rOldObj1),
    pNewObj(&rNewObj1),
    bOldOwner(FALSE),
    bNewOwner(FALSE)
{
    SetOldOwner(TRUE);

    pObjList=pObj->GetObjList();
    if (bOrdNumDirect) {
        nOrdNum=pObj->GetOrdNumDirect();
    } else {
        nOrdNum=pObj->GetOrdNum();
    }
}

SdrUndoReplaceObj::~SdrUndoReplaceObj()
{
    if (pObj!=NULL && IsOldOwner())
    {
        // Attribute muessen wieder in den regulaeren Pool
        SetOldOwner(FALSE);

        // nun loeschen
        delete pObj;
    }
    if (pNewObj!=NULL && IsNewOwner())
    {
        // Attribute muessen wieder in den regulaeren Pool
        SetNewOwner(FALSE);

        // nun loeschen
        delete pNewObj;
    }
}

void SdrUndoReplaceObj::Undo()
{
    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    if (IsOldOwner() && !IsNewOwner()) {
        DBG_ASSERT(!pObj->IsInserted(),"SdrUndoReplaceObj::Undo(): Altes Objekt ist bereits inserted!");
        DBG_ASSERT(pNewObj->IsInserted(),"SdrUndoReplaceObj::Undo(): Neues Objekt ist nicht inserted!");
        SetOldOwner(FALSE);
        SetNewOwner(TRUE);
        SdrObject* pCompObj=pObjList->ReplaceObject(pObj,nOrdNum);
    } else {
        DBG_ERROR("SdrUndoReplaceObj::Undo(): IsMine-Flags stehen verkehrt. Doppelter Undo-Aufruf?");
    }
}

void SdrUndoReplaceObj::Redo()
{
    if (!IsOldOwner() && IsNewOwner()) {
        DBG_ASSERT(!pNewObj->IsInserted(),"SdrUndoReplaceObj::Redo(): Neues Objekt ist bereits inserted!");
        DBG_ASSERT(pObj->IsInserted(),"SdrUndoReplaceObj::Redo(): Altes Objekt ist nicht inserted!");
        SetOldOwner(TRUE);
        SetNewOwner(FALSE);
        SdrObject* pCompObj=pObjList->ReplaceObject(pNewObj,nOrdNum);
    } else {
        DBG_ERROR("SdrUndoReplaceObj::Redo(): IsMine-Flags stehen verkehrt. Doppelter Redo-Aufruf?");
    }

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

void SdrUndoReplaceObj::SetNewOwner(BOOL bNew)
{
    if(bNew != bNewOwner)
    {
        // Besitzuebergang des Objektes. Hier muss auch die Speicherung der
        // Items des Objektes zwischen dem allgemeinen Pool und dem Pool des
        // Undo-Managers wechseln
        if(bNew)
        {
            pNewObj->MigrateItemPool(&rMod.GetItemPool(), ((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()));
        }
        else
        {
            pNewObj->MigrateItemPool(((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()), &rMod.GetItemPool());
        }

        // umsetzen
        bNewOwner = bNew;
    }
}

void SdrUndoReplaceObj::SetOldOwner(BOOL bNew)
{
    if(bNew != bOldOwner)
    {
        // Besitzuebergang des Objektes. Hier muss auch die Speicherung der
        // Items des Objektes zwischen dem allgemeinen Pool und dem Pool des
        // Undo-Managers wechseln
        if(bNew)
        {
            pObj->MigrateItemPool(&rMod.GetItemPool(), ((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()));
        }
        else
        {
            pObj->MigrateItemPool(((SfxItemPool*)SdrObject::GetGlobalDrawObjectItemPool()), &rMod.GetItemPool());
        }

        // umsetzen
        bOldOwner = bNew;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XubString SdrUndoCopyObj::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoCopyObj,aStr);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoObjOrdNum::SdrUndoObjOrdNum(SdrObject& rNewObj, UINT32 nOldOrdNum1, UINT32 nNewOrdNum1):
    SdrUndoObj(rNewObj),
    nOldOrdNum(nOldOrdNum1),
    nNewOrdNum(nNewOrdNum1)
{
}

void SdrUndoObjOrdNum::Undo()
{
    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    SdrObjList* pOL=pObj->GetObjList();
    if (pOL==NULL) {
        DBG_ERROR("UndoObjOrdNum: pObj hat keine ObjList");
        return;
    }
    pOL->SetObjectOrdNum(nNewOrdNum,nOldOrdNum);
}

void SdrUndoObjOrdNum::Redo()
{
    SdrObjList* pOL=pObj->GetObjList();
    if (pOL==NULL) {
        DBG_ERROR("RedoObjOrdNum: pObj hat keine ObjList");
        return;
    }
    pOL->SetObjectOrdNum(nOldOrdNum,nNewOrdNum);

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

XubString SdrUndoObjOrdNum::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoObjOrdNum,aStr);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoObjSetText::SdrUndoObjSetText(SdrObject& rNewObj):
    SdrUndoObj(rNewObj),
    pOldText(NULL),
    pNewText(NULL),
    bNewTextAvailable(FALSE),
    bEmptyPresObj(FALSE)
{
    pOldText=rNewObj.GetOutlinerParaObject();
    bEmptyPresObj = rNewObj.IsEmptyPresObj();
    if (pOldText!=NULL)
        pOldText=pOldText->Clone();
}

SdrUndoObjSetText::~SdrUndoObjSetText()
{
    if ( pOldText )
        delete pOldText;
    if ( pNewText )
        delete pNewText;
}

void SdrUndoObjSetText::AfterSetText()
{
    if (!bNewTextAvailable)
    {
        pNewText=pObj->GetOutlinerParaObject();
        if (pNewText!=NULL) pNewText=pNewText->Clone();
        bNewTextAvailable=TRUE;
    }
}

void SdrUndoObjSetText::Undo()
{
    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();

    // alten Text sichern fuer Redo
    if (!bNewTextAvailable)
        AfterSetText();
    // Text fuer Undo kopieren, denn SetOutlinerParaObject() ist Eigentumsuebereignung
    OutlinerParaObject* pText1=pOldText;
    if (pText1!=NULL)
        pText1=pText1->Clone();
    pObj->SetOutlinerParaObject(pText1);
    pObj->SetEmptyPresObj( bEmptyPresObj );
}

void SdrUndoObjSetText::Redo()
{
    // Text fuer Undo kopieren, denn SetOutlinerParaObject() ist Eigentumsuebereignung
    OutlinerParaObject* pText1=pNewText;
    if (pText1!=NULL) pText1=pText1->Clone();
    pObj->SetOutlinerParaObject(pText1);

    // #94278# Trigger PageChangeCall
    ImpShowPageOfThisObject();
}

XubString SdrUndoObjSetText::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoObjSetText,aStr);
    return aStr;
}

XubString SdrUndoObjSetText::GetSdrRepeatComment(SdrView& rView) const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoObjSetText,aStr);
    return aStr;
}

void SdrUndoObjSetText::SdrRepeat(SdrView& rView)
{
    if (bNewTextAvailable && rView.HasMarkedObj()) {
        const SdrMarkList& rML=rView.GetMarkList();
        XubString aStr;
        ImpTakeDescriptionStr(STR_UndoObjSetText,aStr);
        rView.BegUndo(aStr);
        ULONG nAnz=rML.GetMarkCount();
        for (ULONG nm=0; nm<nAnz; nm++) {
            SdrObject* pObj=rML.GetMark(nm)->GetObj();
            SdrTextObj* pTextObj=PTR_CAST(SdrTextObj,pObj);
            if (pTextObj!=NULL) {
                rView.AddUndo(new SdrUndoObjSetText(*pTextObj));
                OutlinerParaObject* pText1=pNewText;
                if (pText1!=NULL) pText1=pText1->Clone();
                pTextObj->SetOutlinerParaObject(pText1);
            }
        }
        rView.EndUndo();
    }
}

FASTBOOL SdrUndoObjSetText::CanSdrRepeat(SdrView& rView) const
{
    FASTBOOL bOk=FALSE;
    if (bNewTextAvailable && rView.HasMarkedObj()) {
        bOk=TRUE;
    }
    return bOk;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@      @@@@   @@  @@  @@@@@  @@@@@
//  @@     @@  @@  @@  @@  @@     @@  @@
//  @@     @@  @@  @@  @@  @@     @@  @@
//  @@     @@@@@@   @@@@   @@@@   @@@@@
//  @@     @@  @@    @@    @@     @@  @@
//  @@     @@  @@    @@    @@     @@  @@
//  @@@@@  @@  @@    @@    @@@@@  @@  @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoLayer::SdrUndoLayer(USHORT nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel):
    SdrUndoAction(rNewModel),
    pLayer(rNewLayerAdmin.GetLayer(nLayerNum)),
    pLayerAdmin(&rNewLayerAdmin),
    nNum(nLayerNum),
    bItsMine(FALSE)
{
}

SdrUndoLayer::~SdrUndoLayer()
{
    if (bItsMine) {
        delete pLayer;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoNewLayer::Undo()
{
    DBG_ASSERT(!bItsMine,"SdrUndoNewLayer::Undo(): Layer gehoert bereits der UndoAction");
    bItsMine=TRUE;
    SdrLayer* pCmpLayer=pLayerAdmin->RemoveLayer(nNum);
    DBG_ASSERT(pCmpLayer==pLayer,"SdrUndoNewLayer::Undo(): Removter Layer ist != pLayer");
}

void SdrUndoNewLayer::Redo()
{
    DBG_ASSERT(bItsMine,"SdrUndoNewLayer::Undo(): Layer gehoert nicht der UndoAction");
    bItsMine=FALSE;
    pLayerAdmin->InsertLayer(pLayer,nNum);
}

XubString SdrUndoNewLayer::GetComment() const
{
    return ImpGetResStr(STR_UndoNewLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoDelLayer::Undo()
{
    DBG_ASSERT(bItsMine,"SdrUndoDelLayer::Undo(): Layer gehoert nicht der UndoAction");
    bItsMine=FALSE;
    pLayerAdmin->InsertLayer(pLayer,nNum);
}

void SdrUndoDelLayer::Redo()
{
    DBG_ASSERT(!bItsMine,"SdrUndoDelLayer::Undo(): Layer gehoert bereits der UndoAction");
    bItsMine=TRUE;
    SdrLayer* pCmpLayer=pLayerAdmin->RemoveLayer(nNum);
    DBG_ASSERT(pCmpLayer==pLayer,"SdrUndoDelLayer::Redo(): Removter Layer ist != pLayer");
}

XubString SdrUndoDelLayer::GetComment() const
{
    return ImpGetResStr(STR_UndoDelLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoMoveLayer::Undo()
{
    SdrLayer* pCmpLayer=pLayerAdmin->RemoveLayer(nNeuPos);
    DBG_ASSERT(pCmpLayer==pLayer,"SdrUndoMoveLayer::Undo(): Removter Layer ist != pLayer");
    pLayerAdmin->InsertLayer(pLayer,nNum);
}

void SdrUndoMoveLayer::Redo()
{
    SdrLayer* pCmpLayer=pLayerAdmin->RemoveLayer(nNum);
    DBG_ASSERT(pCmpLayer==pLayer,"SdrUndoMoveLayer::Redo(): Removter Layer ist != pLayer");
    pLayerAdmin->InsertLayer(pLayer,nNeuPos);
}

XubString SdrUndoMoveLayer::GetComment() const
{
    return ImpGetResStr(STR_UndoMovLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@    @@@@    @@@@   @@@@@   @@@@
//  @@  @@  @@  @@  @@  @@  @@     @@  @@
//  @@  @@  @@  @@  @@      @@     @@
//  @@@@@   @@@@@@  @@ @@@  @@@@    @@@@
//  @@      @@  @@  @@  @@  @@         @@
//  @@      @@  @@  @@  @@  @@     @@  @@
//  @@      @@  @@   @@@@@  @@@@@   @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoPage::SdrUndoPage(SdrPage& rNewPg):
    SdrUndoAction(*rNewPg.GetModel()), pPage(&rNewPg)
{ }

void SdrUndoPage::ImpInsertPage(USHORT nNum)
{
    DBG_ASSERT(!pPage->IsInserted(),"SdrUndoPage::ImpInsertPage(): pPage ist bereits Inserted");
    if (!pPage->IsInserted()) {
        if (pPage->IsMasterPage()) {
            rMod.InsertMasterPage(pPage,nNum);
        } else {
            rMod.InsertPage(pPage,nNum);
        }
    }
}

void SdrUndoPage::ImpRemovePage(USHORT nNum)
{
    DBG_ASSERT(pPage->IsInserted(),"SdrUndoPage::ImpRemovePage(): pPage ist nicht Inserted");
    if (pPage->IsInserted()) {
        SdrPage* pChkPg=NULL;
        if (pPage->IsMasterPage()) {
            pChkPg=rMod.RemoveMasterPage(nNum);
        } else {
            pChkPg=rMod.RemovePage(nNum);
        }
        DBG_ASSERT(pChkPg==pPage,"SdrUndoPage::ImpRemovePage(): RemovePage!=pPage");
    }
}

void SdrUndoPage::ImpMovePage(USHORT nOldNum, USHORT nNewNum)
{
    DBG_ASSERT(pPage->IsInserted(),"SdrUndoPage::ImpMovePage(): pPage ist nicht Inserted");
    if (pPage->IsInserted()) {
        if (pPage->IsMasterPage()) {
            rMod.MoveMasterPage(nOldNum,nNewNum);
        } else {
            rMod.MovePage(nOldNum,nNewNum);
        }
    }
}

void SdrUndoPage::ImpTakeDescriptionStr(USHORT nStrCacheID, XubString& rStr, USHORT n, FASTBOOL bRepeat) const
{
    rStr=ImpGetResStr(nStrCacheID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoPageList::SdrUndoPageList(SdrPage& rNewPg):
    SdrUndoPage(rNewPg),
    bItsMine(FALSE)
{
    nPageNum=rNewPg.GetPageNum();
}

SdrUndoPageList::~SdrUndoPageList()
{
    if(bItsMine && pPage)
    {
        delete pPage;
        pPage = 0L;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoDelPage::SdrUndoDelPage(SdrPage& rNewPg):
    SdrUndoPageList(rNewPg),
    pUndoGroup(NULL)
{
    bItsMine=TRUE;
    // Und nun ggf. die MasterPage-Beziehungen merken
    if (pPage->IsMasterPage()) {
        USHORT nMasterPageNum=pPage->GetPageNum();
        USHORT nPageAnz=rMod.GetPageCount();
        for (USHORT nPageNum=0; nPageNum<nPageAnz; nPageNum++) {
            SdrPage* pDrawPage=rMod.GetPage(nPageNum);
            USHORT nMasterAnz=pDrawPage->GetMasterPageCount();
            for (USHORT nMasterNum=nMasterAnz; nMasterNum>0;) { // Rueckwaerts, da die Beziehungen auch rueckwaerts entfernt werden
                nMasterNum--;
                USHORT nReferencedMaster=pDrawPage->GetMasterPageNum(nMasterNum);
                if (nReferencedMaster==nMasterPageNum) { // Aha, betroffen
                    if (pUndoGroup==NULL) {
                        pUndoGroup=new SdrUndoGroup(rMod);
                    }
                    pUndoGroup->AddAction(new SdrUndoPageRemoveMasterPage(*pDrawPage,nMasterNum));
                }
            }
        }
    }
}

SdrUndoDelPage::~SdrUndoDelPage()
{
    if (pUndoGroup!=NULL) {
        delete pUndoGroup;
    }
}

void SdrUndoDelPage::Undo()
{
    ImpInsertPage(nPageNum);
    if (pUndoGroup!=NULL) { // MasterPage-Beziehungen wiederherstellen
        pUndoGroup->Undo();
    }
    DBG_ASSERT(bItsMine,"UndoDeletePage: pPage gehoert nicht der UndoAction");
    bItsMine=FALSE;
}

void SdrUndoDelPage::Redo()
{
    ImpRemovePage(nPageNum);
    // Die MasterPage-Beziehungen werden ggf. von selbst geloesst
    DBG_ASSERT(!bItsMine,"RedoDeletePage: pPage gehoert bereits der UndoAction");
    bItsMine=TRUE;
}

XubString SdrUndoDelPage::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoDelPage,aStr,0,FALSE);
    return aStr;
}

XubString SdrUndoDelPage::GetSdrRepeatComment(SdrView& rView) const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoDelPage,aStr,0,FALSE);
    return aStr;
}

void SdrUndoDelPage::SdrRepeat(SdrView& rView)
{
}

FASTBOOL SdrUndoDelPage::CanSdrRepeat(SdrView& rView) const
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoNewPage::Undo()
{
    ImpRemovePage(nPageNum);
    DBG_ASSERT(!bItsMine,"UndoNewPage: pPage gehoert bereits der UndoAction");
    bItsMine=TRUE;
}

void SdrUndoNewPage::Redo()
{
    ImpInsertPage(nPageNum);
    DBG_ASSERT(bItsMine,"RedoNewPage: pPage gehoert nicht der UndoAction");
    bItsMine=FALSE;
}

XubString SdrUndoNewPage::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoNewPage,aStr,0,FALSE);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

XubString SdrUndoCopyPage::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoCopPage,aStr,0,FALSE);
    return aStr;
}

XubString SdrUndoCopyPage::GetSdrRepeatComment(SdrView& rView) const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoCopPage,aStr,0,FALSE);
    return aStr;
}

void SdrUndoCopyPage::SdrRepeat(SdrView& rView)
{

}

FASTBOOL SdrUndoCopyPage::CanSdrRepeat(SdrView& rView) const
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoSetPageNum::Undo()
{
    ImpMovePage(nNewPageNum,nOldPageNum);
}

void SdrUndoSetPageNum::Redo()
{
    ImpMovePage(nOldPageNum,nNewPageNum);
}

XubString SdrUndoSetPageNum::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoMovPage,aStr,0,FALSE);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@   @@  @@@@   @@@@  @@@@@@ @@@@@ @@@@@   @@@@@   @@@@   @@@@  @@@@@  @@@@
//  @@@ @@@ @@  @@ @@  @@   @@   @@    @@  @@  @@  @@ @@  @@ @@  @@ @@    @@  @@
//  @@@@@@@ @@  @@ @@       @@   @@    @@  @@  @@  @@ @@  @@ @@     @@    @@
//  @@@@@@@ @@@@@@  @@@@    @@   @@@@  @@@@@   @@@@@  @@@@@@ @@ @@@ @@@@   @@@@
//  @@ @ @@ @@  @@     @@   @@   @@    @@  @@  @@     @@  @@ @@  @@ @@        @@
//  @@   @@ @@  @@ @@  @@   @@   @@    @@  @@  @@     @@  @@ @@  @@ @@    @@  @@
//  @@   @@ @@  @@  @@@@    @@   @@@@@ @@  @@  @@     @@  @@  @@@@@ @@@@@  @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

SdrUndoPageMasterPage::SdrUndoPageMasterPage(SdrPage& rNewPg, USHORT nMasterDescriptorNum):
        SdrUndoPage(rNewPg), pMasterDescriptor(NULL),
        pNewMasterDescriptor(NULL), nMasterNum(0), nNewMasterNum(0)
{
    nMasterNum=nMasterDescriptorNum;
    SdrMasterPageDescriptor* pDscr=&rNewPg.GetMasterPageDescriptor(nMasterDescriptorNum);
    if (pDscr!=NULL) { // den betroffenen MasterPageDescriptor kopieren
        pMasterDescriptor=new SdrMasterPageDescriptor(*pDscr);
    } else {
#ifdef DBGUTIL
        String aMsg("SdrUndoPageMasterPage::Ctor(): Descriptornummer "); aMsg+=nMasterNum;
        aMsg+="\nKein MasterPageDescriptor an dieser Position gefunden.";
        DBG_ERROR(aMsg.GetStr());
#endif
    }
}

SdrUndoPageMasterPage::~SdrUndoPageMasterPage()
{
    if (pMasterDescriptor!=NULL) delete pMasterDescriptor;
    if (pNewMasterDescriptor!=NULL) delete pNewMasterDescriptor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoPageInsertMasterPage::Undo()
{
#ifdef DBGUTIL
    String aMsg("SdrUndoPageInsertMasterPage::Undo(): Descriptornummer "); aMsg+=nMasterNum;
    if (pMasterDescriptor!=NULL) {
        aMsg+=" (MasterPage-Nummer im gemerkten Descriptor ist ";
        aMsg+=pMasterDescriptor->GetPageNum();
        aMsg+=')';
    } else {
        aMsg+=" (kein Descriptor gemerkt)";
    }
    aMsg+="\n";
    if (nMasterNum>=pPage->GetMasterPageCount()) {
        aMsg+="An der Seite sind nur ";
        aMsg+=pPage->GetMasterPageCount();
        aMsg+=" Descriptoren vorhanden.";
        DBG_ERROR(aMsg.GetStr());
    } else if (pMasterDescriptor!=NULL && *pMasterDescriptor!=*pPage->GetMasterPageDescriptor(nMasterNum)) {
        aMsg+="Aktueller und gemerkter Descriptor enthalten unterschiedliche MasterPage-Nummern";
        DBG_ERROR(aMsg.GetStr());
    }
#endif
    pPage->RemoveMasterPage(nMasterNum);
}

void SdrUndoPageInsertMasterPage::Redo()
{
    if (pMasterDescriptor!=NULL) {
        pPage->InsertMasterPage(*pMasterDescriptor,nMasterNum);
    } else {
#ifdef DBGUTIL
        String aMsg("SdrUndoPageInsertMasterPage::Redo(): Descriptornummer "); aMsg+=nMasterNum;
        aMsg+="Kein MasterPageDescriptor gemerkt.";
        DBG_ERROR(aMsg.GetStr());
#endif
    }
}

XubString SdrUndoPageInsertMasterPage::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoNewPageMasterDscr,aStr,0,FALSE);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoPageRemoveMasterPage::Undo()
{
    if (pMasterDescriptor!=NULL) {
        pPage->InsertMasterPage(*pMasterDescriptor,nMasterNum);
    } else {
#ifdef DBGUTIL
        String aMsg("SdrUndoPageRemoveMasterPage::Undo(): Descriptornummer "); aMsg+=nMasterNum;
        aMsg+="Kein MasterPageDescriptor gemerkt.";
        DBG_ERROR(aMsg.GetStr());
#endif
    }
}

void SdrUndoPageRemoveMasterPage::Redo()
{
#ifdef DBGUTIL
    String aMsg("SdrUndoPageRemoveMasterPage::Redo(): Descriptornummer "); aMsg+=nMasterNum;
    if (pMasterDescriptor!=NULL) {
        aMsg+=" (MasterPage-Nummer im gemerkten Descriptor ist ";
        aMsg+=pMasterDescriptor->GetPageNum();
        aMsg+=')';
    } else {
        aMsg+=" (kein Descriptor gemerkt)";
    }
    aMsg+="\n";
    if (nMasterNum>=pPage->GetMasterPageCount()) {
        aMsg+="An der Seite sind nur ";
        aMsg+=pPage->GetMasterPageCount();
        aMsg+=" Descriptoren vorhanden.";
        DBG_ERROR(aMsg.GetStr());
    } else if (pMasterDescriptor!=NULL && *pMasterDescriptor!=*pPage->GetMasterPageDescriptor(nMasterNum)) {
        aMsg+="Aktueller und gemerkter Descriptor enthalten unterschiedliche MasterPage-Nummern";
        DBG_ERROR(aMsg.GetStr());
    }
#endif
    pPage->RemoveMasterPage(nMasterNum);
}

XubString SdrUndoPageRemoveMasterPage::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoDelPageMasterDscr,aStr,0,FALSE);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoPageMoveMasterPage::Undo()
{
    pPage->MoveMasterPage(nNewMasterNum,nMasterNum);
}

void SdrUndoPageMoveMasterPage::Redo()
{
    pPage->MoveMasterPage(nMasterNum,nNewMasterNum);
}

XubString SdrUndoPageMoveMasterPage::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoMovPageMasterDscr,aStr,0,FALSE);
    return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrUndoPageChangeMasterPage::Undo()
{
    if (pNewMasterDescriptor==NULL) {
        SdrMasterPageDescriptor* pDscr=&pPage->GetMasterPageDescriptor(nMasterNum);
        if (pDscr!=NULL) { // den neuen MasterPageDescriptor merken fuer Redo
            pNewMasterDescriptor=new SdrMasterPageDescriptor(*pDscr);
        } else {
#ifdef DBGUTIL
            String aMsg("SdrUndoPageChangeMasterPage::Undo(): Descriptornummer "); aMsg+=nMasterNum;
            aMsg+="\nKein MasterPageDescriptor an dieser Position gefunden.";
            DBG_ERROR(aMsg.GetStr());
#endif
        }
    }
    if (pMasterDescriptor!=NULL) {
        pPage->SetMasterPageDescriptor(*pMasterDescriptor,nMasterNum);
    }
}

void SdrUndoPageChangeMasterPage::Redo()
{
    if (pNewMasterDescriptor!=NULL) {
        pPage->SetMasterPageDescriptor(*pNewMasterDescriptor,nMasterNum);
    }
}

XubString SdrUndoPageChangeMasterPage::GetComment() const
{
    XubString aStr;
    ImpTakeDescriptionStr(STR_UndoChgPageMasterDscr,aStr,0,FALSE);
    return aStr;
}

