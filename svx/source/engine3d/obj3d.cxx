/*************************************************************************
 *
 *  $RCSfile: obj3d.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: aw $ $Date: 2001-07-10 10:09:51 $
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

#define ITEMID_COLOR            SID_ATTR_3D_LIGHTCOLOR

#include "svdstr.hrc"
#include "svdglob.hxx"

#ifndef _SOT_DTRANS_HXX
#include <sot/dtrans.hxx>
#endif

#ifndef _SVDVIEW_HXX
#include "svdview.hxx"
#endif

#ifndef _SVDATTR_HXX
#include "svdattr.hxx"
#endif

#ifndef _SVDPAGE_HXX
#include "svdpage.hxx"
#endif

#ifndef _SVDIO_HXX
#include "svdio.hxx"
#endif

#ifndef _SVDMODEL_HXX
#include "svdmodel.hxx"
#endif

#ifndef _SVDITER_HXX
#include "svditer.hxx"
#endif

#ifndef _E3D_GLOBL3D_HXX
#include "globl3d.hxx"
#endif

#ifndef _CAMERA3D_HXX
#include "camera3d.hxx"
#endif

#ifndef _E3D_VOLMRK3D_HXX
#include "volmrk3d.hxx"
#endif

#ifndef _E3D_POLYOB3D_HXX
#include "polyob3d.hxx"
#endif

#ifndef _E3D_SCENE3D_HXX
#include "scene3d.hxx"
#endif

#ifndef _E3D_POLYSC3D_HXX
#include "polysc3d.hxx"
#endif

#ifndef _E3D_CUBE3D_HXX
#include "cube3d.hxx"
#endif

#ifndef _E3D_LATHE3D_HXX
#include "lathe3d.hxx"
#endif

#ifndef _E3D_SPHERE3D_HXX
#include "sphere3d.hxx"
#endif

#ifndef _E3D_EXTRUD3D_HXX
#include "extrud3d.hxx"
#endif

#ifndef _E3D_OBJ3D_HXX
#include "obj3d.hxx"
#endif

#ifndef _XOUTX_HXX
#include "xoutx.hxx"
#endif

#ifndef _XTABLE_HXX
#include "xtable.hxx"
#endif

#ifndef _SVX_XFLCLIT_HXX
#include "xflclit.hxx"
#endif

#ifndef _B3D_BASE3D_HXX
#include <goodies/base3d.hxx>
#endif

#ifndef _B3D_B3DTEX_HXX
#include <goodies/b3dtex.hxx>
#endif

#ifndef _SVX_XLNCLIT_HXX
#include "xlnclit.hxx"
#endif

#ifndef _SFXMETRICITEM_HXX
#include <svtools/metitem.hxx>
#endif

#ifndef _XTABLE_HXX
#include "xtable.hxx"
#endif

#ifndef _SVX_FILLITEM_HXX
#include "xfillit.hxx"
#endif

#ifndef _SVX_XLNWTIT_HXX
#include "xlnwtit.hxx"
#endif

#ifndef _SV_VIRDEV_HXX
#include <vcl/virdev.hxx>
#endif

#ifndef _SVDIO_HXX
#include "svdio.hxx"
#endif

#ifndef _SV_POLY_HXX
#include <vcl/poly.hxx>
#endif

#ifndef _B3D_B3DTRANS_HXX
#include "b3dtrans.hxx"
#endif

#ifndef _SVX_SVXIDS_HRC
#include "svxids.hrc"
#endif

#ifndef _SVX_COLRITEM_HXX
#include "colritem.hxx"
#endif

#ifndef _SVXE3DITEM_HXX
#include "e3ditem.hxx"
#endif

#ifndef _SVX_XLNTRIT_HXX
#include "xlntrit.hxx"
#endif

#ifndef _SVX_XFLTRIT_HXX
#include "xfltrit.hxx"
#endif

#ifndef _SVDPAGV_HXX
#include "svdpagv.hxx"
#endif

#ifndef _SV_GRADIENT_HXX
#include <vcl/gradient.hxx>
#endif

#ifndef _SV_METAACT_HXX
#include <vcl/metaact.hxx>
#endif

#ifndef _SVX3DITEMS_HXX
#include "svx3ditems.hxx"
#endif

#ifndef _SFX_WHITER_HXX
#include <svtools/whiter.hxx>
#endif

#ifndef _EEITEM_HXX
#include "eeitem.hxx"
#endif

#ifndef _SVX_XGRSCIT_HXX
#include "svx/xgrscit.hxx"
#endif

#define ITEMVALUE(ItemSet,Id,Cast)  ((const Cast&)(ItemSet).Get(Id)).GetValue()

/*************************************************************************
|*
|* Liste fuer 3D-Objekte
|*
\************************************************************************/

TYPEINIT1(E3dObjList, SdrObjList);

E3dObjList::E3dObjList(SdrModel* pNewModel, SdrPage* pNewPage, E3dObjList* pNewUpList)
:   SdrObjList(pNewModel, pNewPage, pNewUpList)
{
}

E3dObjList::E3dObjList(const E3dObjList& rSrcList)
:   SdrObjList(rSrcList)
{
}

E3dObjList::~E3dObjList()
{
}

void E3dObjList::NbcInsertObject(SdrObject* pObj, ULONG nPos, const SdrInsertReason* pReason)
{
    // Owner holen
    DBG_ASSERT(GetOwnerObj()->ISA(E3dObject), "AW: Einfuegen 3DObject in Parent != 3DObject");

    // Ist es ueberhaupt ein 3D-Objekt?
    if(pObj && pObj->ISA(E3dObject))
    {
        // Normales 3D Objekt, einfuegen mittels
        // call parent
        SdrObjList::NbcInsertObject(pObj, nPos, pReason);
    }
    else
    {
        // Kein 3D Objekt, fuege in Seite statt in Szene ein...
        GetOwnerObj()->GetPage()->InsertObject(pObj, nPos);
    }
}

SdrObject* E3dObjList::NbcRemoveObject(ULONG nObjNum)
{
    // Owner holen
    DBG_ASSERT(GetOwnerObj()->ISA(E3dObject), "AW: Entfernen 3DObject aus Parent != 3DObject");
    E3dObject* pOwner = (E3dObject*)GetOwnerObj();

    // call parent
    SdrObject* pRetval = SdrObjList::NbcRemoveObject(nObjNum);

    // FitSnapRectToBoundVol vorbereiten
    if(GetOwnerObj() && GetOwnerObj()->ISA(E3dScene))
        ((E3dScene*)GetOwnerObj())->CorrectSceneDimensions();

    return pRetval;
}

SdrObject* E3dObjList::RemoveObject(ULONG nObjNum)
{
    // Owner holen
    DBG_ASSERT(GetOwnerObj()->ISA(E3dObject), "AW: Entfernen 3DObject aus Parent != 3DObject");
    E3dObject* pOwner = (E3dObject*)GetOwnerObj();

    // call parent
    SdrObject* pRetval = SdrObjList::RemoveObject(nObjNum);

    // FitSnapRectToBoundVol vorbereiten
    if(GetOwnerObj() && GetOwnerObj()->ISA(E3dScene))
        ((E3dScene*)GetOwnerObj())->CorrectSceneDimensions();

    return pRetval;
}

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

TYPEINIT1(E3dObject, SdrAttrObj);

E3dObject::E3dObject() :
    nLogicalGroup(0),
    nObjTreeLevel(0),
    eDragDetail(E3DDETAIL_ONEBOX),
    nPartOfParent(0),
    bTfHasChanged(TRUE),
    bBoundVolValid(TRUE),
    bIsSelected(FALSE)
{
    bIs3DObj = TRUE;
    pSub = new E3dObjList(NULL, NULL);
    pSub->SetOwnerObj(this);
    pSub->SetListKind(SDROBJLIST_GROUPOBJ);
    bClosedObj = TRUE;
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

E3dObject::~E3dObject()
{
    delete pSub;
    pSub = NULL;
}

/*************************************************************************
|*
|* Selektions-Flag setzen
|*
\************************************************************************/

void E3dObject::SetSelected(BOOL bNew)
{
    // selbst setzen
    bIsSelected = bNew;

    // bei SubObjekten setzen
    for ( ULONG i = 0; i < pSub->GetObjCount(); i++ )
    {
        if(pSub->GetObj(i) && pSub->GetObj(i)->ISA(E3dObject))
            ((E3dObject*)pSub->GetObj(i))->SetSelected(bNew);
    }
}

/*************************************************************************
|*
|* Aufbrechen, default-Implementierungen
|*
\************************************************************************/

BOOL E3dObject::IsBreakObjPossible()
{
    return FALSE;
}

SdrAttrObj* E3dObject::GetBreakObj()
{
    return 0L;
}

/*************************************************************************
|*
|* SetRectsDirty muss ueber die lokale SdrSubList gehen
|*
\************************************************************************/

void E3dObject::SetRectsDirty(FASTBOOL bNotMyself)
{
    // call parent
    SdrAttrObj::SetRectsDirty(bNotMyself);

    // Eigene SubListe AUCH behandeln
    if(pSub && pSub->GetObjCount())
    {
        for (ULONG i = 0; i < pSub->GetObjCount(); i++)
        {
            SdrObject* pObj = pSub->GetObj(i);
            DBG_ASSERT(pObj->ISA(E3dObject), "AW: In E3dObject sind nur 3D-Objekte erlaubt!");
            ((E3dObject*)pObj)->SetRectsDirty(bNotMyself);
        }
    }
}

/*************************************************************************
|*
|* Inventor zurueckgeben
|*
\************************************************************************/

UINT32 E3dObject::GetObjInventor() const
{
    return E3dInventor;
}

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/

UINT16 E3dObject::GetObjIdentifier() const
{
    return E3D_OBJECT_ID;
}

/*************************************************************************
|*
|* Faehigkeiten des Objektes feststellen
|*
\************************************************************************/

void E3dObject::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
{
    rInfo.bResizeFreeAllowed    = TRUE;
    rInfo.bResizePropAllowed    = TRUE;
    rInfo.bRotateFreeAllowed    = TRUE;
    rInfo.bRotate90Allowed      = TRUE;
    rInfo.bMirrorFreeAllowed    = FALSE;
    rInfo.bMirror45Allowed      = FALSE;
    rInfo.bMirror90Allowed      = FALSE;
    rInfo.bShearAllowed         = FALSE;
    rInfo.bEdgeRadiusAllowed    = FALSE;
    rInfo.bCanConvToPath        = FALSE;

    // no transparence for 3d objects
    rInfo.bTransparenceAllowed = FALSE;

    // gradient depends on fillstyle
    // BM *** check if SetItem is NULL ***
    XFillStyle eFillStyle = ((XFillStyleItem&)(GetItem(XATTR_FILLSTYLE))).GetValue();
    rInfo.bGradientAllowed = (eFillStyle == XFILL_GRADIENT);

    // Umwandeln von 3D-Koerpern in Gruppe von Polygonen:
    //
    // Erst mal nicht moeglich, da die Erzeugung einer Gruppe von
    // 2D-Polygonen notwendig waere, die tiefensortiert werden muessten,
    // also bei Durchdringugnen auch gegeneinander geschnitten werden
    // muessten. Auch die Texturkoorinaten waeren ein ungeloestes
    // Problem.
    rInfo.bCanConvToPoly = FALSE;
    rInfo.bCanConvToContour = FALSE;

    rInfo.bCanConvToPathLineToArea = FALSE;
    rInfo.bCanConvToPolyLineToArea = FALSE;
}

/*************************************************************************
|*
|* Layer abfragen
|*
\************************************************************************/

SdrLayerID E3dObject::GetLayer() const
{
    FASTBOOL bFirst = TRUE;
    E3dObjList* pOL = pSub;
    ULONG       nObjCnt = pOL->GetObjCount();
    SdrLayerID  nLayer = SdrLayerID(nLayerID);

    for ( ULONG i = 0; i < nObjCnt; i++ )
    {
        SdrLayerID nObjLayer;
        if(pOL->GetObj(i)->ISA(E3dPolyObj))
            nObjLayer = SdrLayerID(nLayerID);
        else
            nObjLayer = pOL->GetObj(i)->GetLayer();

        if (bFirst)
        {
            nLayer = nObjLayer;
            bFirst = FALSE;
        }
        else if ( nObjLayer != nLayer )
            return 0;
    }
    return nLayer;
}

/*************************************************************************
|*
|* Layer setzen
|*
\************************************************************************/

void E3dObject::NbcSetLayer(SdrLayerID nLayer)
{
    SdrAttrObj::NbcSetLayer(nLayer);

    E3dObjList* pOL = pSub;
    ULONG nObjCnt = pOL->GetObjCount();
    ULONG i;
    for ( i = 0; i < nObjCnt; i++ )
        pOL->GetObj(i)->NbcSetLayer(nLayer);
}

/*************************************************************************
|*
|* ObjList auch an SubList setzen
|*
\************************************************************************/

void E3dObject::SetObjList(SdrObjList* pNewObjList)
{
    SdrObject::SetObjList(pNewObjList);
    pSub->SetUpList(pNewObjList);
}

/*************************************************************************
|*
|* Layer setzen
|*
\************************************************************************/

void E3dObject::SetPage(SdrPage* pNewPage)
{
    SdrAttrObj::SetPage(pNewPage);
    pSub->SetPage(pNewPage);
}

/*************************************************************************
|*
|* Layer setzen
|*
\************************************************************************/

void E3dObject::SetModel(SdrModel* pNewModel)
{
    SdrAttrObj::SetModel(pNewModel);
    pSub->SetModel(pNewModel);
}

/*************************************************************************
|*
|* resize object, used from old 2d interfaces, e.g. in Move/Scale dialog
|* (F4)
|*
\************************************************************************/

void E3dObject::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    // SdrAttrObj::NbcResize(rRef, xFact, yFact);

    // Bewegung in X,Y im Augkoordinatensystem
    E3dScene* pScene = GetScene();

    if(pScene)
    {
        // pos ermitteln
        B3dTransformationSet& rSet = pScene->GetCameraSet();
        Vector3D aScaleCenter((double)rRef.X(), (double)rRef.Y(), 32768.0);
        aScaleCenter = rSet.ViewToEyeCoor(aScaleCenter);

        // scale-faktoren holen
        double fScaleX = xFact;
        double fScaleY = yFact;

        // build transform
        Matrix4D mFullTransform(GetFullTransform());
        Matrix4D mTrans(mFullTransform);

        mTrans *= rSet.GetOrientation();
        mTrans.Translate(-aScaleCenter);
        mTrans.Scale(fScaleX, fScaleY, 1.0);
        mTrans.Translate(aScaleCenter);
        mTrans *= rSet.GetInvOrientation();
        mFullTransform.Invert();
        mTrans *= mFullTransform;

        // anwenden
        Matrix4D mObjTrans(GetTransform());
        mObjTrans *= mTrans;
        SetTransform(mObjTrans);

        // force new camera and SnapRect on scene, geometry may have really
        // changed
        pScene->CorrectSceneDimensions();
    }
}

/*************************************************************************
|*
|* Objekt verschieben in 2D, wird bei Cursortasten benoetigt
|*
\************************************************************************/

void E3dObject::NbcMove(const Size& rSize)
{
    // Bewegung in X,Y im Augkoordinatensystem
    E3dScene* pScene = GetScene();

    if(pScene)
    {
        // Abmessungen der Szene in 3D und 2D als Vergleich
        Rectangle aRect = pScene->GetSnapRect();

        // Transformation Weltkoordinaten bis eine VOR Objektkoordinaten holen
        Matrix4D mInvDispTransform;
        if(GetParentObj())
        {
            mInvDispTransform = GetParentObj()->GetFullTransform();
            mInvDispTransform.Invert();
        }

        // BoundVolume von Weltkoordinaten in Eye-Koordinaten
        B3dTransformationSet& rSet = pScene->GetCameraSet();
        const Volume3D& rVol = pScene->GetBoundVolume();
        Volume3D aEyeVol = rVol.GetTransformVolume(rSet.GetOrientation());

        // relativen Bewegungsvektor in Augkoordinaten bilden
        Vector3D aMove(
            (double)rSize.Width() * aEyeVol.GetWidth() / (double)aRect.GetWidth(),
            (double)-rSize.Height() * aEyeVol.GetHeight() / (double)aRect.GetHeight(),
            0.0);

        // Bewegungsvektor in lokale Koordinaten des Parents des Objektes
        Vector3D aPos;
        aMove = rSet.EyeToWorldCoor(aMove);
        aMove *= mInvDispTransform;
        aPos = rSet.EyeToWorldCoor(aPos);
        aPos *= mInvDispTransform;
        aMove = aMove - aPos;

        // Transformieren
        Translate(aMove);

        // force new camera and SnapRect on scene, geometry may have really
        // changed
        pScene->CorrectSceneDimensions();
    }
}

/*************************************************************************
|*
|* liefere die Sublist, aber nur dann, wenn darin Objekte enthalten sind !
|*
\************************************************************************/

SdrObjList* E3dObject::GetSubList() const
{
    return pSub;
}

/*************************************************************************
|*
|* Anzahl der Handles zurueckgeben
|*
\************************************************************************/

USHORT E3dObject::GetHdlCount() const
{
    // 8 Eckpunkte + 1 E3dVolumeMarker (= Wireframe-Darstellung)
    return 9;
}

/*************************************************************************
|*
|* Handle-Liste fuellen
|*
\************************************************************************/

void E3dObject::AddToHdlList(SdrHdlList& rHdlList) const
{
    XPolyPolygon     aXPP(12);
    XPolygon         aLine(2);
    E3dVolumeMarker* pVolMarker;
    USHORT           nPolyCnt;

    ((E3dObject*) this)->ImpCreateWireframePoly(aXPP, E3DDETAIL_ONEBOX);
    nPolyCnt = aXPP.Count();

    for ( USHORT i = 0; i < nPolyCnt; i += 3 )
    {
        rHdlList.AddHdl(new SdrHdl(aXPP[i][0], HDL_BWGT));
        rHdlList.AddHdl(new SdrHdl(aXPP[i][1], HDL_BWGT));
    }

    if ( nPolyCnt > 0 )
    {
        pVolMarker = new E3dVolumeMarker(aXPP);
        rHdlList.AddHdl(pVolMarker);
    }
}

/*************************************************************************
|*
\************************************************************************/

FASTBOOL E3dObject::HasSpecialDrag() const
{
    return TRUE;
}

/*************************************************************************
|*
|* Paint; wird z.Z. nicht benutzt, da das Paint ueber die
|* (2D-)Displayliste der Szene laeuft
|*
\************************************************************************/

void E3dObject::Paint3D(ExtOutputDevice& rOut, Base3D* pBase3D,
    const SdrPaintInfoRec& rInfoRec, UINT16 nDrawFlags)
{
    if(pSub && pSub->GetObjCount())
    {
        BOOL bWasNotActive = rInfoRec.bNotActive;
        BOOL bIsEnteredGroup(FALSE);

        if((rInfoRec.pPV && GetSubList() && rInfoRec.pPV->GetObjList() == GetSubList())
            || ((rInfoRec.nPaintMode & SDRPAINTMODE_MASTERPAGE) != 0))
        {
            bIsEnteredGroup = TRUE;
        }

        if(bIsEnteredGroup && bWasNotActive)
        {
            // auf aktive Elemente schalten
            ((SdrPaintInfoRec&)rInfoRec).bNotActive = FALSE;
        }

        for (ULONG i = 0; i < pSub->GetObjCount(); i++)
        {
            SdrObject* pObj = pSub->GetObj(i);
            DBG_ASSERT(pObj->ISA(E3dObject), "AW: In E3dObject sind nur 3D-Objekte erlaubt!");
            ((E3dObject*)pObj)->Paint3D(rOut, pBase3D, rInfoRec, nDrawFlags);
        }

        if(bIsEnteredGroup && bWasNotActive)
        {
            // Zurueck auf Ursprung, Zustand wieder verlassen
            ((SdrPaintInfoRec&)rInfoRec).bNotActive = TRUE;
        }
    }
}

/*************************************************************************
|*
|* Objekt als Kontur in das Polygon einfuegen
|*
\************************************************************************/

void E3dObject::TakeContour3D(XPolyPolygon& rPoly)
{
    if(pSub && pSub->GetObjCount())
    {
        for (ULONG i = 0; i < pSub->GetObjCount(); i++)
        {
            SdrObject* pObj = pSub->GetObj(i);
            DBG_ASSERT(pObj->ISA(E3dObject), "AW: In E3dObject sind nur 3D-Objekte erlaubt!");
            ((E3dObject*)pObj)->TakeContour3D(rPoly);
        }
    }
}

/*************************************************************************
|*
|* Schatten fuer 3D-Objekte zeichnen
|*
\************************************************************************/

void E3dObject::DrawShadows(Base3D *pBase3D,
    ExtOutputDevice& rXOut,
    const Rectangle& rBound, const Volume3D& rVolume,
    const SdrPaintInfoRec& rInfoRec)
{
    if(pSub && pSub->GetObjCount())
    {
        for (ULONG i = 0; i < pSub->GetObjCount(); i++)
        {
            SdrObject* pObj = pSub->GetObj(i);
            DBG_ASSERT(pObj->ISA(E3dObject), "AW: In E3dObject sind nur 3D-Objekte erlaubt!");
            ((E3dObject*)pObj)->DrawShadows(pBase3D, rXOut, rBound, rVolume, rInfoRec);
        }
    }
}

/*************************************************************************
|*
|* SnapRect berechnen
|*
\************************************************************************/

void E3dObject::RecalcSnapRect()
{
    maSnapRect = Rectangle();
    if(pSub && pSub->GetObjCount())
    {
        for (ULONG i = 0; i < pSub->GetObjCount(); i++)
        {
            SdrObject* pObj = pSub->GetObj(i);
            DBG_ASSERT(pObj->ISA(E3dObject), "AW: In E3dObject sind nur 3D-Objekte erlaubt!");
            Rectangle aSubRect = ((E3dObject*)pObj)->GetSnapRect();
            maSnapRect.Union(aSubRect);
        }
    }
}

/*************************************************************************
|*
|* BoundRect berechnen
|*
\************************************************************************/

void E3dObject::RecalcBoundRect()
{
    // BoundRect aus SnapRect berechnen
    aOutRect = GetSnapRect();

    if(pSub && pSub->GetObjCount())
    {
        for (ULONG i = 0; i < pSub->GetObjCount(); i++)
        {
            SdrObject* pObj = pSub->GetObj(i);
            DBG_ASSERT(pObj->ISA(E3dObject), "AW: In E3dObject sind nur 3D-Objekte erlaubt!");
            Rectangle aSubRect = ((E3dObject*)pObj)->GetBoundRect();
            aOutRect.Union(aSubRect);
        }
    }
}

/*************************************************************************
|*
|* Einfuegen eines 3D-Objekts an den Parent weitermelden, damit dieser
|* ggf. eine Sonderbehandlung fuer spezielle Objekte durchfuehren kann
|* (z.B. Light/Label in E3dScene)
|*
\************************************************************************/

void E3dObject::NewObjectInserted(const E3dObject* p3DObj)
{
    if(GetParentObj())
        GetParentObj()->NewObjectInserted(p3DObj);
}

/*************************************************************************
|*
|* Parent ueber Aenderung der Struktur (z.B. durch Transformation)
|* informieren; dabei wird das Objekt, in welchem die Aenderung
|* aufgetreten ist, uebergeben
|*
\************************************************************************/

void E3dObject::StructureChanged(const E3dObject* p3DObj)
{
    if ( GetParentObj() )
    {
        // Wenn sich im Child das BoundVolume geaendert hat, muessen
        // auch die der Parents angepasst werden
        if ( !p3DObj->bBoundVolValid )
            GetParentObj()->bBoundVolValid = FALSE;

        GetParentObj()->StructureChanged(p3DObj);
    }
}

/*************************************************************************
|*
|* 3D-Objekt einfuegen
|*
\************************************************************************/

void E3dObject::Insert3DObj(E3dObject* p3DObj)
{
    DBG_ASSERT(p3DObj, "Insert3DObj mit NULL-Zeiger!");
    p3DObj->SetObjTreeLevel(nObjTreeLevel + 1);
    SdrPage* pPg = pPage;
    pSub->InsertObject(p3DObj);
    pPage = pPg;
    bBoundVolValid = FALSE;
    NewObjectInserted(p3DObj);
    StructureChanged(this);
}

void E3dObject::Remove3DObj(E3dObject* p3DObj)
{
    DBG_ASSERT(p3DObj, "Remove3DObj mit NULL-Zeiger!");

    if(p3DObj->GetParentObj() == this)
    {
        SdrPage* pPg = pPage;
        pSub->RemoveObject(p3DObj->GetOrdNum());
        pPage = pPg;

        bBoundVolValid = FALSE;
        StructureChanged(this);
    }
}

/*************************************************************************
|*
|* Parent holen
|*
\************************************************************************/

E3dObject* E3dObject::GetParentObj() const
{
    E3dObject* pRetval = NULL;

    if(GetObjList()
        && GetObjList()->GetOwnerObj()
        && GetObjList()->GetOwnerObj()->ISA(E3dObject))
        pRetval = ((E3dObject*)GetObjList()->GetOwnerObj());
    return pRetval;
}

/*************************************************************************
|*
|* Uebergeordnetes Szenenobjekt bestimmen
|*
\************************************************************************/

E3dScene* E3dObject::GetScene() const
{
    if(GetParentObj())
        return GetParentObj()->GetScene();
    return NULL;
}

/*************************************************************************
|*
|* umschliessendes Volumen inklusive aller Kindobjekte berechnen
|*
\************************************************************************/

void E3dObject::RecalcBoundVolume()
{
    E3dObjList* pOL = pSub;
    ULONG nObjCnt = pOL->GetObjCount();

    if(nObjCnt)
    {
        aBoundVol = Volume3D();

        for (ULONG i = 0; i < nObjCnt; i++)
        {
            SdrObject* pObj = pOL->GetObj(i);

            DBG_ASSERT(pObj->ISA(E3dObject), "In E3dObject sind nur 3D-Objekte erlaubt!");
            // Bei den Kindobjekten auch die lokalen Transformationen
            // beruecksichtigen
            E3dObject* p3DObj = (E3dObject*) pObj;
            const Volume3D& rVol = p3DObj->GetBoundVolume();
            const Matrix4D& rTf  = p3DObj->GetTransform();
            aBoundVol.Union(rVol.GetTransformVolume(rTf));
        }

        aLocalBoundVol = aBoundVol;
    }
    else
    {
        aBoundVol = aLocalBoundVol;
    }

    bBoundVolValid = TRUE;
}

/*************************************************************************
|*
|* umschliessendes Volumen zurueckgeben und ggf. neu berechnen
|*
\************************************************************************/

const Volume3D& E3dObject::GetBoundVolume()
{
    if ( !bBoundVolValid )
        RecalcBoundVolume();

    if(!aBoundVol.IsValid())
        aBoundVol = Volume3D(Vector3D(), Vector3D());

    return aBoundVol;
}

/*************************************************************************
|*
|* Mittelpunkt liefern
|*
\************************************************************************/

Vector3D E3dObject::GetCenter()
{
    Volume3D aVolume = GetBoundVolume();
    return (aVolume.MaxVec() + aVolume.MinVec()) / 2.0;
}

/*************************************************************************
|*
|* Aederung des BoundVolumes an alle Kindobjekte weitergeben
|*
\************************************************************************/

void E3dObject::SetBoundVolInvalid()
{
    bBoundVolValid = FALSE;

    E3dObjList* pOL = pSub;
    ULONG nObjCnt = pOL->GetObjCount();

    for (ULONG i = 0; i < nObjCnt; i++)
    {
        SdrObject* pObj = pOL->GetObj(i);
        DBG_ASSERT(pObj->ISA(E3dObject), "In E3dObject sind nur 3D-Objekte erlaubt!");

        ((E3dObject*) pObj)->SetBoundVolInvalid();
    }
}

/*************************************************************************
|*
|* Aederung der Transformation an alle Kindobjekte weitergeben
|*
\************************************************************************/

void E3dObject::SetTransformChanged()
{
    bTfHasChanged = TRUE;
    bBoundVolValid = FALSE;

    E3dObjList* pOL = pSub;
    ULONG nObjCnt = pOL->GetObjCount();

    for (ULONG i = 0; i < nObjCnt; i++)
    {
        SdrObject* pObj = pOL->GetObj(i);
        DBG_ASSERT(pObj->ISA(E3dObject), "In E3dObject sind nur 3D-Objekte erlaubt!");

        ((E3dObject*) pObj)->SetTransformChanged();
    }
}

/*************************************************************************
|*
|* hierarchische Transformation ueber alle Parents bestimmen und mit
|* der uebergebenen Matrix verketten
|*
\************************************************************************/

void E3dObject::GetFullTransform(Matrix4D& rMatrix) const
{
    if ( bTfHasChanged )
    {
        rMatrix *= aTfMatrix;
        if ( GetParentObj() )
            GetParentObj()->GetFullTransform(rMatrix);
    }
    else
        rMatrix *= aFullTfMatrix;
}

/*************************************************************************
|*
|* hierarchische Transformation ueber alle Parents bestimmen, in
|* aFullTfMatrix ablegen und diese zurueckgeben
|*
\************************************************************************/

const Matrix4D& E3dObject::GetFullTransform()
{
    if ( bTfHasChanged )
    {
        aFullTfMatrix = aTfMatrix;

        if ( GetParentObj() )
            aFullTfMatrix *= GetParentObj()->GetFullTransform();

        bTfHasChanged = FALSE;
    }

    return aFullTfMatrix;
}

/*************************************************************************
|*
|* Transformationsmatrix abfragen
|*
\************************************************************************/

const Matrix4D& E3dObject::GetTransform() const
{
    return aTfMatrix;
}

/*************************************************************************
|*
|* Transformationsmatrix setzen
|*
\************************************************************************/

void E3dObject::NbcSetTransform(const Matrix4D& rMatrix)
{
    aTfMatrix = rMatrix;
    SetTransformChanged();
    StructureChanged(this);
}

/*************************************************************************
|*
|* Transformationsmatrix auf Einheitsmatrix zuruecksetzen
|*
\************************************************************************/

void E3dObject::NbcResetTransform()
{
    aTfMatrix.Identity();
    SetTransformChanged();
    StructureChanged(this);
}

/*************************************************************************
|*
|* Transformationsmatrix setzen mit Repaint-Broadcast
|*
\************************************************************************/

void E3dObject::SetTransform(const Matrix4D& rMatrix)
{
    SendRepaintBroadcast();
    NbcSetTransform(rMatrix);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/*************************************************************************
|*
|* Transformationsmatrix zuruecksetzen mit Repaint-Broadcast
|*
\************************************************************************/

void E3dObject::ResetTransform()
{
    SendRepaintBroadcast();
    NbcResetTransform();
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/*************************************************************************
|*
|* Translation
|*
\************************************************************************/

void E3dObject::NbcTranslate(const Vector3D& rTrans)
{
    aTfMatrix.Translate(rTrans);
    SetTransformChanged();
    StructureChanged(this);
}
/*************************************************************************
|*
|* Translation mit Repaint-Broadcast
|*
\************************************************************************/

void E3dObject::Translate(const Vector3D& rTrans)
{
    SendRepaintBroadcast();
    NbcTranslate(rTrans);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/*************************************************************************
|*
|* Skalierungen
|*
\************************************************************************/

void E3dObject::NbcScaleX(double fSx)
{
    aTfMatrix.ScaleX(fSx);
    SetTransformChanged();
    StructureChanged(this);
}

/************************************************************************/

void E3dObject::NbcScaleY(double fSy)
{
    aTfMatrix.ScaleY(fSy);
    SetTransformChanged();
    StructureChanged(this);
}

/************************************************************************/

void E3dObject::NbcScaleZ(double fSz)
{
    aTfMatrix.ScaleZ(fSz);
    SetTransformChanged();
    StructureChanged(this);
}

/************************************************************************/

void E3dObject::NbcScale(double fSx, double fSy, double fSz)
{
    aTfMatrix.Scale(fSx, fSy, fSz);
    SetTransformChanged();
    StructureChanged(this);
}

/*************************************************************************
|*
|* gleichmaessige Skalierung
|*
\************************************************************************/

void E3dObject::NbcScale(double fS)
{
    aTfMatrix.Scale(fS, fS, fS);
    SetTransformChanged();
    StructureChanged(this);
}

/*************************************************************************
|*
|* Skalierungen mit mit Repaint-Broadcast
|*
\************************************************************************/

void E3dObject::ScaleX(double fSx)
{
    SendRepaintBroadcast();
    NbcScaleX(fSx);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/************************************************************************/

void E3dObject::ScaleY(double fSy)
{
    SendRepaintBroadcast();
    NbcScaleY(fSy);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/************************************************************************/

void E3dObject::ScaleZ(double fSz)
{
    SendRepaintBroadcast();
    NbcScaleZ(fSz);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/************************************************************************/

void E3dObject::Scale(double fSx, double fSy, double fSz)
{
    SendRepaintBroadcast();
    NbcScale(fSx, fSy, fSz);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/************************************************************************/

void E3dObject::Scale(double fS)
{
    SendRepaintBroadcast();
    NbcScale(fS);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/*************************************************************************
|*
|* Rotationen mit Winkel in Radiant
|*
\************************************************************************/

void E3dObject::NbcRotateX(double fAng)
{
    aTfMatrix.RotateX(fAng);
    SetTransformChanged();
    StructureChanged(this);
}

/************************************************************************/

void E3dObject::NbcRotateY(double fAng)
{
    aTfMatrix.RotateY(fAng);
    SetTransformChanged();
    StructureChanged(this);
}

/************************************************************************/

void E3dObject::NbcRotateZ(double fAng)
{
    aTfMatrix.RotateZ(fAng);
    SetTransformChanged();
    StructureChanged(this);
}

/*************************************************************************
|*
|* Rotationen mit Repaint-Broadcast
|*
\************************************************************************/

void E3dObject::RotateX(double fAng)
{
    SendRepaintBroadcast();
    NbcRotateX(fAng);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/************************************************************************/

void E3dObject::RotateY(double fAng)
{
    SendRepaintBroadcast();
    NbcRotateY(fAng);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/************************************************************************/

void E3dObject::RotateZ(double fAng)
{
    SendRepaintBroadcast();
    NbcRotateZ(fAng);
    SetChanged();
    SendRepaintBroadcast();
    if (pUserCall != NULL) pUserCall->Changed(*this, SDRUSERCALL_RESIZE, Rectangle());
}

/*************************************************************************
|*
|* Objektbaum-Ebene des Objekts und aller Children setzen
|*
\************************************************************************/

void E3dObject::SetObjTreeLevel(USHORT nNewLevel)
{
    nObjTreeLevel = nNewLevel;
    nNewLevel++;

    E3dObjList* pOL = pSub;
    ULONG nObjCnt = pOL->GetObjCount();

    for (ULONG i = 0; i < nObjCnt; i++)
    {
        SdrObject* pObj = pOL->GetObj(i);
        DBG_ASSERT(pObj->ISA(E3dObject), "In E3dObject sind nur 3D-Objekte erlaubt!");

        ((E3dObject*) pObj)->SetObjTreeLevel(nNewLevel);
    }
}

/*************************************************************************
|*
|* logische Gruppe setzen
|*
\************************************************************************/

void E3dObject::SetLogicalGroup(USHORT nGroup)
{
    nLogicalGroup = nGroup;
}

/*************************************************************************
|*
|* Linien fuer die Wireframe-Darstellung des Objekts dem uebergebenen
|* Polygon3D hinzufuegen. Als default wird das BoundVolume verwendet.
|*
\************************************************************************/

void E3dObject::CreateWireframe(Polygon3D& rWirePoly, const Matrix4D* pTf,
    E3dDragDetail eDetail)
{
    if ( eDetail == E3DDETAIL_DEFAULT )
        eDetail = eDragDetail;

    if ( eDetail == E3DDETAIL_ALLBOXES || eDetail == E3DDETAIL_ALLLINES )
    {
        E3dObjList* pOL = pSub;
        ULONG nObjCnt = pOL->GetObjCount();

        for (ULONG i = 0; i < nObjCnt; i++)
        {
            E3dObject* pObj = (E3dObject*)pOL->GetObj(i);
            DBG_ASSERT(pObj->ISA(E3dObject), "In E3dObject sind nur 3D-Objekte erlaubt!");

            Matrix4D aLocalTf(pObj->GetTransform());
            if(pTf)
                aLocalTf *= *pTf;
            pObj->CreateWireframe(rWirePoly, &aLocalTf, eDetail);
        }

        if(eDetail == E3DDETAIL_ALLBOXES && nObjCnt != 1)
            GetBoundVolume().CreateWireframe(rWirePoly, pTf);
    }
    else
        GetBoundVolume().CreateWireframe(rWirePoly, pTf);
}

/*************************************************************************
|*
|* Get the name of the object (singular)
|*
\************************************************************************/

void E3dObject::TakeObjNameSingul(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNameSingulObj3d);
}

/*************************************************************************
|*
|* Get the name of the object (plural)
|*
\************************************************************************/

void E3dObject::TakeObjNamePlural(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNamePluralObj3d);
}

/*************************************************************************
|*
|* Wireframe-XPolyPolygon erzeugen
|*
\************************************************************************/

void E3dObject::ImpCreateWireframePoly(XPolyPolygon& rXPP,
    E3dDragDetail eDetail)
{
    // Neue Methode
    E3dScene* pScene = GetScene();
    Polygon3D aPoly3D(24, 240);
    XPolygon aLine(2);
    USHORT nPntCnt;

    // WireFrame herstellen
    CreateWireframe(aPoly3D, NULL, eDetail);
    nPntCnt = aPoly3D.GetPointCount();

    if(pScene)
    {
        // Maximas holen in DeviceKoordinaten
        Volume3D aVolume = pScene->FitInSnapRect();

        // Maximas fuer Abbildung verwenden
        pScene->GetCameraSet().SetDeviceVolume(aVolume, FALSE);
        Matrix4D mTransform = GetFullTransform();
        pScene->GetCameraSet().SetObjectTrans(mTransform);

        if ( nPntCnt > 1 )
        {
            Vector3D aVec;
            for ( USHORT i = 0; i < nPntCnt; i += 2 )
            {
                aVec = pScene->GetCameraSet().ObjectToViewCoor(aPoly3D[i]);
                aLine[0] = Point((long)(aVec.X() + 0.5), (long)(aVec.Y() + 0.5));

                aVec = pScene->GetCameraSet().ObjectToViewCoor(aPoly3D[i+1]);
                aLine[1] = Point((long)(aVec.X() + 0.5), (long)(aVec.Y() + 0.5));

                rXPP.Insert(aLine);
            }
        }
    }
}

/*************************************************************************
|*
|* Drag-Polygon zurueckgeben
|*
\************************************************************************/

void E3dObject::TakeXorPoly(XPolyPolygon& rXPP, FASTBOOL bDetail) const
{
    rXPP.Clear();
    // Const mal wieder weg, da evtl. das BoundVolume neu generiert wird
    static E3dDragDetail eDetail = E3DDETAIL_DEFAULT;
    ((E3dObject*) this)->ImpCreateWireframePoly(rXPP, eDetail);
}

/*************************************************************************
|*
|* Zuweisungsoperator
|*
\************************************************************************/

void E3dObject::operator=(const SdrObject& rObj)
{
    // erstmal alle Childs kopieren
    SdrAttrObj::operator=(rObj);

    const E3dObject& r3DObj = (const E3dObject&) rObj;
    if (r3DObj.GetSubList())
    {
        // feststellen, ob alle SubObjekte selektiert oder
        // deselektiert sind
        BOOL bAllSelected = TRUE;
        BOOL bNoneSelected = TRUE;
        UINT32 nObjCnt = r3DObj.GetSubList()->GetObjCount();

        ULONG i;
        for (i = 0; i < nObjCnt; i++)
        {
            SdrObject* pObj = r3DObj.GetSubList()->GetObj(i);
            if(pObj && pObj->ISA(E3dObject))
            {
                E3dObject* p3DObj = (E3dObject*)pObj;
                if(p3DObj->GetSelected())
                    bNoneSelected = FALSE;
                else
                    bAllSelected = FALSE;
            }
        }

        if(bAllSelected || bNoneSelected)
        {
            // Normales verhalten
            pSub->CopyObjects(*r3DObj.GetSubList());
        }
        else
        {
            // Spezielle SubListe aufstellen, kopieren
            SdrObjList aOwnSubList(*r3DObj.GetSubList());

            // Alle nicht selektierten Objekte rausschmeissen
            for(i = 0;i < aOwnSubList.GetObjCount();i++)
            {
                SdrObject* pObj = aOwnSubList.GetObj(i);
                if(pObj && pObj->ISA(E3dObject))
                {
                    E3dObject* p3DObj = (E3dObject*)pObj;
                    if(!p3DObj->GetSelected())
                    {
                        aOwnSubList.NbcRemoveObject(pObj->GetOrdNum());
                        i--;
                        delete pObj;
                    }
                }
            }

            // jetzt diese Liste kopieren
            pSub->CopyObjects(aOwnSubList);

            // Hier noch ein FitSnapRect einleiten
            if(ISA(E3dScene))
                ((E3dScene&)r3DObj).FitSnapRectToBoundVol();
        }
    }

    // BoundVol kann uebernommen werden, da die Childs auch kopiert werden
    bBoundVolValid  = r3DObj.bBoundVolValid;
    aBoundVol       = r3DObj.aBoundVol;
    aLocalBoundVol  = r3DObj.aLocalBoundVol;

    aTfMatrix       = r3DObj.aTfMatrix;
    nLogicalGroup   = r3DObj.nLogicalGroup;
    nObjTreeLevel   = r3DObj.nObjTreeLevel;
    nPartOfParent   = r3DObj.nPartOfParent;
    eDragDetail     = r3DObj.eDragDetail;

    // Da sich der Parent geaendert haben kann, Gesamttransformation beim
    // naechsten Mal auf jeden Fall neu bestimmen
    SetTransformChanged();

    // Selektionsstatus kopieren
    bIsSelected = r3DObj.bIsSelected;
}

//////////////////////////////////////////////////////////////////////////////
// ItemSet access

const SfxItemSet& E3dObject::GetItemSet() const
{
    // include Items of scene this object belongs to
    E3dScene* pScene = GetScene();
    if(pScene && pScene != this)
    {
        SfxItemSet& rSet = (SfxItemSet&)SdrAttrObj::GetItemSet();
        SfxItemSet aSet(*rSet.GetPool(), SDRATTR_3DSCENE_FIRST, SDRATTR_3DSCENE_LAST);
        aSet.Put(pScene->E3dObject::GetItemSet());
        rSet.Put(aSet);
    }

    return SdrAttrObj::GetItemSet();
}

SfxItemSet* E3dObject::CreateNewItemSet(SfxItemPool& rPool)
{
    // include ALL items, 2D and 3D
    return new SfxItemSet(rPool,
        // ranges from SdrAttrObj
        SDRATTR_START, SDRATTRSET_SHADOW,
        SDRATTRSET_OUTLINER, SDRATTRSET_MISC,

        // ranges for 3D (object and scene)
        SDRATTR_3D_FIRST, SDRATTR_3D_LAST,

        // outliner and end
        EE_ITEMS_START, EE_ITEMS_END,
        0, 0);
}

// private support routines for ItemSet access. NULL pointer means clear item.
void E3dObject::ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem)
{
    // propagate item changes to scene
    if(!nWhich || (nWhich >= SDRATTR_3DSCENE_FIRST && nWhich <= SDRATTR_3DSCENE_LAST))
    {
        E3dScene* pScene = GetScene();
        if(pScene && pScene != this)
            pScene->E3dObject::ItemChange(nWhich, pNewItem);
    }

    // call parent
    SdrAttrObj::ItemChange(nWhich, pNewItem);
}

void E3dObject::ItemSetChanged( const SfxItemSet& rSet )
{
    // call parent
    SdrAttrObj::ItemSetChanged( rSet );

    // local changes
    StructureChanged(this);
}

/*************************************************************************
|*
|* StyleSheet setzen
|*
\************************************************************************/

void E3dObject::NbcSetStyleSheet(SfxStyleSheet* pNewStyleSheet,
    FASTBOOL bDontRemoveHardAttr)
{
    // call parent
    SdrAttrObj::NbcSetStyleSheet(pNewStyleSheet, bDontRemoveHardAttr);

    E3dObjList* pOL = pSub;
    ULONG nObjCnt = pOL->GetObjCount();
    for ( ULONG i = 0; i < nObjCnt; i++ )
        pOL->GetObj(i)->NbcSetStyleSheet(pNewStyleSheet, bDontRemoveHardAttr);
}

/*************************************************************************
|*
|* Nur die Member des E3dObjekts in den Stream speichern
|* Dies wird direkt auch von E3dSphere gerufen um zu verhindern dass die
|* Subliste weggeschrieben wird. (FG)
|*
\************************************************************************/

void E3dObject::WriteOnlyOwnMembers(SvStream& rOut) const
{
    // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
    SdrDownCompat aCompat(rOut, STREAM_WRITE);
#ifdef DBG_UTIL
    aCompat.SetID("E3dObjectOwnMembers");
#endif

    rOut << aLocalBoundVol;

    Old_Matrix3D aMat3D;
    aMat3D = aTfMatrix;
    rOut << aMat3D;

    rOut << nLogicalGroup;
    rOut << nObjTreeLevel;
    rOut << nPartOfParent;
    rOut << UINT16(eDragDetail);
}

/*************************************************************************
|*
|* Objektdaten in Stream speichern
|*
\************************************************************************/

void E3dObject::WriteData(SvStream& rOut) const
{
    long position = rOut.Tell();
    SdrAttrObj::WriteData(rOut);
    position = rOut.Tell();

    // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
    SdrDownCompat aCompat(rOut, STREAM_WRITE);
#ifdef DBG_UTIL
    aCompat.SetID("E3dObject");
#endif

    position = rOut.Tell();
    pSub->Save(rOut);
    position = rOut.Tell();

    if (rOut.GetVersion() < 3560)
    {
        rOut << aLocalBoundVol;

        Old_Matrix3D aMat3D;
        aMat3D = aTfMatrix;
        rOut << aMat3D;

        rOut << nLogicalGroup;
        rOut << nObjTreeLevel;
        rOut << nPartOfParent;
        rOut << UINT16(eDragDetail);
    }
    else
    {
        WriteOnlyOwnMembers(rOut);
    }
    position = rOut.Tell();
}

/*************************************************************************
|*
|* Objektdaten aus Stream laden
|*
\************************************************************************/

void E3dObject::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
{
    long position = rIn.Tell();
    if (ImpCheckSubRecords (rHead, rIn))
    {
        position = rIn.Tell();
        SdrAttrObj::ReadData(rHead, rIn);
        position = rIn.Tell();
        // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
        SdrDownCompat aCompat(rIn, STREAM_READ);
#ifdef DBG_UTIL
        aCompat.SetID("E3dObject");
#endif
        pSub->Load(rIn, *pPage);

        position = rIn.Tell();
        if ((rIn.GetVersion() < 3560) || (rHead.GetVersion() <= 12))
        {
            UINT16  nTmp16;

            rIn >> aLocalBoundVol;

            Old_Matrix3D aMat3D;
            rIn >> aMat3D;
            aTfMatrix = Matrix4D(aMat3D);

            rIn >> nLogicalGroup;
            rIn >> nObjTreeLevel;
            rIn >> nPartOfParent;
            rIn >> nTmp16; eDragDetail = E3dDragDetail(nTmp16);
        }
        else
        {
            ReadOnlyOwnMembers(rHead, rIn);
        }
        position = rIn.Tell();

        // Wie ein veraendertes Objekt behandeln
        SetTransformChanged();
        StructureChanged(this);

        // BoundVolume muss neu berechnet werden
        bBoundVolValid = FALSE;

        // SnapRect auch
        bSnapRectDirty = TRUE;
    }
}

/*************************************************************************
|*
|* Nur die Daten des E3dObject aus Stream laden (nicht der Sublisten und
|* der Basisklassen). Wird von E3dSphere auch genutzt. (FileFormat-Optimierung)
|*
\************************************************************************/

void E3dObject::ReadOnlyOwnMembers(const SdrObjIOHeader& rHead, SvStream& rIn)
{
    // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
    SdrDownCompat aCompat(rIn, STREAM_READ);
#ifdef DBG_UTIL
    aCompat.SetID("E3dObjectOwnMembers");
#endif
    UINT16  nTmp16;

    rIn >> aLocalBoundVol;

    Old_Matrix3D aMat3D;
    rIn >> aMat3D;
    aTfMatrix = Matrix4D(aMat3D);

    rIn >> nLogicalGroup;
    rIn >> nObjTreeLevel;
    rIn >> nPartOfParent;
    rIn >> nTmp16; eDragDetail = E3dDragDetail(nTmp16);

    bBoundVolValid = FALSE;
}


/*************************************************************************
|*
|* nach dem Laden...
|*
\************************************************************************/

void E3dObject::AfterRead()
{
    SdrAttrObj::AfterRead();
    if (pSub)
        pSub->AfterRead();
}

/*************************************************************************
|*
|* erstelle neues GeoData-Objekt
|*
\************************************************************************/

SdrObjGeoData *E3dObject::NewGeoData() const
{
    // Theoretisch duerfen auch nur Szenen ihre GeoDatas erstellen und verwalten !!
    // AW: Dies stimmt nicht mehr, diese Stelle ist mit der neuen Engine OK!
    return new E3DObjGeoData;
}

/*************************************************************************
|*
|* uebergebe aktuelle werte an das GeoData-Objekt
|*
\************************************************************************/

void E3dObject::SaveGeoData(SdrObjGeoData& rGeo) const
{
    SdrAttrObj::SaveGeoData (rGeo);

    ((E3DObjGeoData &) rGeo).aLocalBoundVol  = aLocalBoundVol;
    ((E3DObjGeoData &) rGeo).aTfMatrix       = aTfMatrix;
}

/*************************************************************************
|*
|* uebernehme werte aus dem GeoData-Objekt
|*
\************************************************************************/

void E3dObject::RestGeoData(const SdrObjGeoData& rGeo)
{
    aLocalBoundVol = ((E3DObjGeoData &) rGeo).aLocalBoundVol;
    NbcSetTransform (((E3DObjGeoData &) rGeo).aTfMatrix);

    SdrAttrObj::RestGeoData (rGeo);
    GetScene()->FitSnapRectToBoundVol();
}

/*************************************************************************
|*
|* Pruefe, ob die SubRecords ok sind und mit der Factory gelesen werden
|* koennen.
|*
\************************************************************************/

BOOL E3dObject::ImpCheckSubRecords (const SdrObjIOHeader& rHead,
                                    SvStream&             rIn)
{
    BOOL bDoRead = FALSE;

    if ( rIn.GetError() == SVSTREAM_OK )
    {
        if (rHead.GetVersion () <= 12)
        {
            ULONG nPos0 = rIn.Tell();
            // Einen SubRecord ueberspringen (SdrObject)
            { SdrDownCompat aCompat(rIn,STREAM_READ); }
            // Nocheinen SubRecord ueberspringen (SdrAttrObj)
            { SdrDownCompat aCompat(rIn,STREAM_READ); }
            // Und nun muesste meiner kommen
            bDoRead = rHead.GetBytesLeft() != 0;
            rIn.Seek (nPos0); // FilePos wieder restaurieren
        }
        else
        {
            bDoRead = TRUE;
        }
    }

    return bDoRead;
}

/*************************************************************************
|*
|* Keine DefaultAttr, zu langsam
|*
\************************************************************************/

void E3dObject::ForceDefaultAttr()
{
}

/*************************************************************************
|*
|* Falls doch noch DefaultAttrs benoetigt werden
|*
\************************************************************************/

void E3dObject::ForceDefaultAttrAgain()
{
    SdrAttrObj::ForceDefaultAttr();
}

/*************************************************************************
|*
|* Rotation eines 3d-Koerpers
|*
\************************************************************************/
// 2D-rotation eines 3D-Koerpers, normalerweise macht das die Szene selbst
// Ist aber eine korrekte Implementierung, denn alles was passiert ist eine
// Rotation um die Achse die senkrecht auf dem Bildschirm steht und zwar
// unabhaengig davon, wie die Szene bisher gedreht worden ist.

void E3dObject::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
{
        // Also derzeit sind die Klebepunkte relativ zum aOutRect der Szene definiert. Vor dem Drehen
        // werden die Klebepunkte relativ zur Seite definiert. Sie nehmen an der Drehung der Szene noch nicht Teil
        // dafuer gibt es den
    SetGlueReallyAbsolute(TRUE);

    // SendRepaintBroadcast();
    double fWinkelInRad = nWink/100 * F_PI180;
    NbcRotateZ(fWinkelInRad);
    SetRectsDirty();    // Veranlasst eine Neuberechnung aller BoundRects
    NbcRotateGluePoints(rRef,nWink,sn,cs);  // Rotiert die Klebepunkte (die haben noch Koordinaten relativ
                                            // zum Urpsung des Blattes
    SetGlueReallyAbsolute(FALSE);  // ab jetzt sind sie wieder relativ zum BoundRect (also dem aOutRect definiert)
}

/*************************************************************************/

TYPEINIT1(E3dCompoundObject, E3dObject);

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

E3dCompoundObject::E3dCompoundObject() : E3dObject()
{
    // Defaults setzen
    E3dDefaultAttributes aDefault;
    SetDefaultAttributes(aDefault);

    bBytesLeft = FALSE;
    bCreateE3dPolyObj = FALSE;
    bGeometryValid = FALSE;
}

E3dCompoundObject::E3dCompoundObject(E3dDefaultAttributes& rDefault) : E3dObject()
{
    // Defaults setzen
    SetDefaultAttributes(rDefault);

    bBytesLeft = FALSE;
    bCreateE3dPolyObj = FALSE;
    bGeometryValid = FALSE;
}

void E3dCompoundObject::SetDefaultAttributes(E3dDefaultAttributes& rDefault)
{
    // Defaults setzen
    aMaterialAmbientColor = rDefault.GetDefaultAmbientColor();

    aBackMaterial = rDefault.GetDefaultBackMaterial();
    bCreateNormals = rDefault.GetDefaultCreateNormals();
    bCreateTexture = rDefault.GetDefaultCreateTexture();
    bUseDifferentBackMaterial = rDefault.GetDefaultUseDifferentBackMaterial();
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

E3dCompoundObject::~E3dCompoundObject ()
{
}

/*************************************************************************
|*
|* Start der Geometrieerzeugung ankuendigen
|*
\************************************************************************/

void E3dCompoundObject::StartCreateGeometry()
{
    // Geometriestart mitteilen
    aDisplayGeometry.StartDescription();

    // Lokales Volumen reset
    aLocalBoundVol = Volume3D();

    // Geometrie ist ab jetzt gueltig, um ein rekursives weiteres
    // Erzeugen zu verhindern
    bGeometryValid = TRUE;
}

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/

UINT16 E3dCompoundObject::GetObjIdentifier() const
{
    return E3D_COMPOUNDOBJ_ID;
}

/*************************************************************************
|*
|* Compounds brauchen Defaults
|*
\************************************************************************/

void E3dCompoundObject::ForceDefaultAttr()
{
    SdrAttrObj::ForceDefaultAttr();
}

/*************************************************************************
|*
|* SnapRect berechnen
|*
\************************************************************************/

void E3dCompoundObject::RecalcSnapRect()
{
    E3dScene* pScene = GetScene();
    if(pScene)
    {
        // Objekttransformation uebernehmen
        const Volume3D& rBoundVol = GetBoundVolume();
        maSnapRect = Rectangle();

        if(rBoundVol.IsValid())
        {
            const Matrix4D& rTrans = GetFullTransform();
            Vol3DPointIterator aIter(rBoundVol, &rTrans);
            Vector3D aTfVec;
            while ( aIter.Next(aTfVec) )
            {
                aTfVec = pScene->GetCameraSet().WorldToViewCoor(aTfVec);
                Point aPoint((long)(aTfVec.X() + 0.5), (long)(aTfVec.Y() + 0.5));
                maSnapRect.Union(Rectangle(aPoint, aPoint));
            }
        }
        bSnapRectDirty = FALSE;
    }
}

/*************************************************************************
|*
|* BoundRect berechnen und evtl. Schatten einbeziehen
|* Dazu muss ein eventueller Schatten des einzelnen 3D-Objektes
|* beruecksichtigt werden
|*
\************************************************************************/

void E3dCompoundObject::RecalcBoundRect()
{
    // BoundRect aus SnapRect berechnen
    aOutRect = GetSnapRect();

    E3dScene* pScene = GetScene();
    if(pScene)
    {
        // Schatten beruecksichtigen
        if(DoDrawShadow())
        {
            // ObjectTrans setzen
            Matrix4D mTransform = GetFullTransform();
            pScene->GetCameraSet().SetObjectTrans(mTransform);

            // Schattenpolygon holen
            PolyPolygon aShadowPoly;
            GetShadowPolygon(aShadowPoly);

            // Hinzufuegen
            aOutRect.Union(aShadowPoly.GetBoundRect());
        }

        // Linienbreite beruecksichtigen
        INT32 nLineWidth = ((const XLineWidthItem&)(GetItem(XATTR_LINEWIDTH))).GetValue();
        if(nLineWidth)
        {
            Rectangle aShadowRect = aOutRect;
            aShadowRect.Left() -= nLineWidth;
            aShadowRect.Right() += nLineWidth;
            aShadowRect.Top() -= nLineWidth;
            aShadowRect.Bottom() += nLineWidth;
            aOutRect.Union(aShadowRect);
        }
    }
}

/*************************************************************************
|*
|* BoundVolume holen. Falls die Geometrie ungueltig ist, diese neu
|* erzeugen und das BoundVol neu berechnen
|*
\************************************************************************/

const Volume3D& E3dCompoundObject::GetBoundVolume()
{
    // Geometrie aktuell?
    if(!bGeometryValid)
    {
        // Neu erzeugen und eine Neubestimmung des BoundVol erzwingen
        ReCreateGeometry();
        bBoundVolValid = FALSE;
    }

    // call parent
    return E3dObject::GetBoundVolume();
}

/*************************************************************************
|*
|* Rausschreiben der Datenmember eines E3dCompounds
|*
\************************************************************************/

void E3dCompoundObject::WriteData(SvStream& rOut) const
{
#ifdef E3D_STREAMING

    if (!aLocalBoundVol.IsValid() && aBoundVol.IsValid())
    {
        // Das aLocalBoundVol wird gespeichert.
        // Ist dieses ungueltig, so wird das aBoundVol genommen
        // (sollten beim E3dCompoundObject sowieso gleich sein)
        ((E3dCompoundObject*) this)->aLocalBoundVol = aBoundVol;
    }

    E3dObject::WriteData(rOut);
    if (rOut.GetVersion() < 3560)
    {
        // In diesem Fall passiert nichts, da vor der Version 4.0
        // also im Falle der Revision 3.1
    }
    else
    {
        SdrDownCompat aCompat(rOut, STREAM_WRITE);
#ifdef DBG_UTIL
        aCompat.SetID("E3dCompoundObject");
#endif
        rOut << BOOL(GetDoubleSided());
#endif

        // neue Parameter zur Geometrieerzeugung
        rOut << BOOL(bCreateNormals);
        rOut << BOOL(bCreateTexture);

        sal_uInt16 nVal = GetNormalsKind();
        rOut << BOOL(nVal > 0);
        rOut << BOOL(nVal > 1);

        nVal = GetTextureProjectionX();
        rOut << BOOL(nVal > 0);
        rOut << BOOL(nVal > 1);

        nVal = GetTextureProjectionY();
        rOut << BOOL(nVal > 0);
        rOut << BOOL(nVal > 1);

        rOut << BOOL(GetShadow3D());

        // neu al 384:
        rOut << GetMaterialAmbientColor();
        rOut << GetMaterialColor();
        rOut << GetMaterialSpecular();
        rOut << GetMaterialEmission();
        rOut << GetMaterialSpecularIntensity();

        aBackMaterial.WriteData(rOut);

        rOut << (UINT16)GetTextureKind();

        rOut << (UINT16)GetTextureMode();

        rOut << BOOL(GetNormalsInvert());

        // neu ab 534: (hat noch gefehlt)
        rOut << BOOL(GetTextureFilter());
    }
}

/*************************************************************************
|*
|* Einlesen der Datenmember eines E3dCompounds
|*
\************************************************************************/

void E3dCompoundObject::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
{
    if ( rIn.GetError() != SVSTREAM_OK )
        return;

    E3dObject::ReadData(rHead, rIn);

      // Vor der Filerevision 13 wurde das Objekt nie geschrieben.
      // auch kein Kompatibilitaetsrecord.
    if ((rHead.GetVersion() < 13) || (rIn.GetVersion() < 3560))
    {
        return;
    }

    SdrDownCompat aCompat(rIn, STREAM_READ);
#ifdef DBG_UTIL
    aCompat.SetID("E3dCompoundObject");
#endif

    bBytesLeft = FALSE;
    if (aCompat.GetBytesLeft () >= sizeof (BOOL))
    {
        BOOL bTmp, bTmp2;
        sal_uInt16 nTmp;

        rIn >> bTmp;
        mpObjectItemSet->Put(Svx3DDoubleSidedItem(bTmp));

        // neue Parameter zur Geometrieerzeugung
        if (aCompat.GetBytesLeft () >= sizeof (BOOL))
        {
            rIn >> bTmp;
            bCreateNormals = bTmp;

            rIn >> bTmp;
            bCreateTexture = bTmp;

            rIn >> bTmp;
            rIn >> bTmp2;
            if(bTmp == FALSE && bTmp2 == FALSE)
                nTmp = 0;
            else if(bTmp == TRUE && bTmp2 == FALSE)
                nTmp = 1;
            else
                nTmp = 2;
            mpObjectItemSet->Put(Svx3DNormalsKindItem(nTmp));

            rIn >> bTmp;
            rIn >> bTmp2;
            if(bTmp == FALSE && bTmp2 == FALSE)
                nTmp = 0;
            else if(bTmp == TRUE && bTmp2 == FALSE)
                nTmp = 1;
            else
                nTmp = 2;
            mpObjectItemSet->Put(Svx3DTextureProjectionXItem(nTmp));

            rIn >> bTmp;
            rIn >> bTmp2;
            if(bTmp == FALSE && bTmp2 == FALSE)
                nTmp = 0;
            else if(bTmp == TRUE && bTmp2 == FALSE)
                nTmp = 1;
            else
                nTmp = 2;
            mpObjectItemSet->Put(Svx3DTextureProjectionYItem(nTmp));

            rIn >> bTmp;
            mpObjectItemSet->Put(Svx3DShadow3DItem(bTmp));

            // Setze ein Flag fuer den Aufrufer, dass neues Format
            // zu lesen ist
            bBytesLeft = TRUE;
        }

        // neu al 384:
        if (aCompat.GetBytesLeft () >= sizeof (B3dMaterial))
        {
            UINT16 nTmp;

            Color aCol;

            rIn >> aCol;
            SetMaterialAmbientColor(aCol);

            rIn >> aCol;
            // do NOT use, this is the old 3D-Color(!)
            // SetItem(XFillColorItem(String(), aCol));

            rIn >> aCol;
            mpObjectItemSet->Put(Svx3DMaterialSpecularItem(aCol));

            rIn >> aCol;
            mpObjectItemSet->Put(Svx3DMaterialEmissionItem(aCol));

            rIn >> nTmp;
            mpObjectItemSet->Put(Svx3DMaterialSpecularIntensityItem(nTmp));

            aBackMaterial.ReadData(rIn);

            rIn >> nTmp;
            mpObjectItemSet->Put(Svx3DTextureKindItem(nTmp));

            rIn >> nTmp;
            mpObjectItemSet->Put(Svx3DTextureModeItem(nTmp));

            rIn >> bTmp;
            mpObjectItemSet->Put(Svx3DNormalsInvertItem(bTmp));
        }

        // neu ab 534: (hat noch gefehlt)
        if (aCompat.GetBytesLeft () >= sizeof (BOOL))
        {
            rIn >> bTmp;
            mpObjectItemSet->Put(Svx3DTextureFilterItem(bTmp));
        }
    }
}

/*************************************************************************
|*
|* Bitmaps fuer 3D-Darstellung von Gradients und Hatches holen
|*
\************************************************************************/

AlphaMask E3dCompoundObject::GetAlphaMask(const SfxItemSet& rSet, const Size& rSizePixel)
{
    const XFillFloatTransparenceItem& rFloatTrans = ((const XFillFloatTransparenceItem&)(rSet.Get(XATTR_FILLFLOATTRANSPARENCE)));
    VirtualDevice *pVD = new VirtualDevice();
    pVD->SetOutputSizePixel( rSizePixel );
    XOutputDevice *pXOut = new XOutputDevice( pVD );
    SfxItemSet aFillSet(*rSet.GetPool());

    XGradient aNewGradient(rFloatTrans.GetValue());

    Color aStartCol(aNewGradient.GetStartColor());
    if(aNewGradient.GetStartIntens() != 100)
    {
        double fFact = (double)aNewGradient.GetStartIntens() / 100.0;
        aStartCol = (B3dColor)aStartCol * fFact;
    }
    aNewGradient.SetStartColor(aStartCol);
    aNewGradient.SetStartIntens(100);

    Color aEndCol(aNewGradient.GetEndColor());
    if(aNewGradient.GetEndIntens() != 100)
    {
        double fFact = (double)aNewGradient.GetEndIntens() / 100.0;
        aEndCol = (B3dColor)aEndCol * fFact;
    }
    aNewGradient.SetEndColor(aEndCol);
    aNewGradient.SetEndIntens(100);

    aFillSet.Put( XFillStyleItem( XFILL_GRADIENT ) );
    aFillSet.Put( XFillGradientItem( String(), aNewGradient ) );
    aFillSet.Put( XGradientStepCountItem( aNewGradient.GetSteps() ) );
    pXOut->SetFillAttr( aFillSet );

    aFillSet.Put( XLineStyleItem( XLINE_NONE ) );
    pXOut->SetLineAttr( aFillSet );

    pXOut->DrawRect( Rectangle( Point(), rSizePixel ) );
    Bitmap aGradientBitmap = pVD->GetBitmap( Point(), rSizePixel );

    if( pVD )
        delete pVD;
    if( pXOut )
        delete pXOut;

    return AlphaMask(aGradientBitmap);
}

Bitmap E3dCompoundObject::GetGradientBitmap(const SfxItemSet& rSet)
{
    VirtualDevice *pVD = new VirtualDevice();
    Size aVDSize(256, 256);
    pVD->SetOutputSizePixel( aVDSize );
    XOutputDevice *pXOut = new XOutputDevice( pVD );
    SfxItemSet aFillSet(*rSet.GetPool());

    aFillSet.Put( XFillStyleItem( XFILL_GRADIENT ) );
    aFillSet.Put( rSet.Get(XATTR_FILLGRADIENT) );
    aFillSet.Put( rSet.Get(XATTR_GRADIENTSTEPCOUNT) );
    pXOut->SetFillAttr( aFillSet );

    aFillSet.Put( XLineStyleItem( XLINE_NONE ) );
    pXOut->SetLineAttr( aFillSet );

    pXOut->DrawRect( Rectangle( Point(), aVDSize ) );
    Bitmap aGradientBitmap = pVD->GetBitmap( Point(), aVDSize );

    if( pVD )
        delete pVD;
    if( pXOut )
        delete pXOut;

    return aGradientBitmap;
}

Bitmap E3dCompoundObject::GetHatchBitmap(const SfxItemSet& rSet)
{
    VirtualDevice *pVD = new VirtualDevice();
    const XFillHatchItem* pFillHatchItem = (XFillHatchItem*)&rSet.Get(XATTR_FILLHATCH);
    const XHatch& rHatch = pFillHatchItem->GetValue();
    long nDistance = rHatch.GetDistance(); // in 100stel mm
    double fAngle = double(rHatch.GetAngle()) * (F_PI180 / 10.0);

    double fMinX(0.0);
    double fMaxX(0.0);
    double fMinY(0.0);
    double fMaxY(0.0);

    // nDistance in X-Richtung
    double fX = cos(fAngle) * double(nDistance);
    double fY = sin(fAngle) * double(nDistance);

    if(fX < fMinX)
        fMinX = fX;
    if(fX > fMaxX)
        fMaxX = fX;

    if(fY < fMinY)
        fMinY = fY;
    if(fY > fMaxY)
        fMaxY = fY;

    // nDistance in Y-Richtung
    fX = cos(fAngle + F_PI2) * double(nDistance);
    fY = sin(fAngle + F_PI2) * double(nDistance);

    if(fX < fMinX)
        fMinX = fX;
    if(fX > fMaxX)
        fMaxX = fX;

    if(fY < fMinY)
        fMinY = fY;
    if(fY > fMaxY)
        fMaxY = fY;

    // nDistance in -X-Richtung
    fX = cos(fAngle + F_PI) * double(nDistance);
    fY = sin(fAngle + F_PI) * double(nDistance);

    if(fX < fMinX)
        fMinX = fX;
    if(fX > fMaxX)
        fMaxX = fX;

    if(fY < fMinY)
        fMinY = fY;
    if(fY > fMaxY)
        fMaxY = fY;

    // nDistance in -Y-Richtung
    fX = cos(fAngle + (F_PI + F_PI2)) * double(nDistance);
    fY = sin(fAngle + (F_PI + F_PI2)) * double(nDistance);

    if(fX < fMinX)
        fMinX = fX;
    if(fX > fMaxX)
        fMaxX = fX;

    if(fY < fMinY)
        fMinY = fY;
    if(fY > fMaxY)
        fMaxY = fY;

    long nDistanceX = long(fMaxX - fMinX);
    long nDistanceY = long(fMaxY - fMinY);

    // Bei Schraffuren in eine der 4 Himmelsrichtungen Ausdehnung halbieren
    if(rHatch.GetAngle() % 900 == 0)
    {
        nDistanceX /= 2;
        nDistanceY /= 2;
    }

    Size aVDSize(64, 64);
    pVD->SetOutputSizePixel( aVDSize );
    MapMode aMapMode(MAP_100TH_MM);
    pVD->SetMapMode(aMapMode);

    XOutputDevice *pXOut = new XOutputDevice( pVD );
    SfxItemSet aFillSet(*rSet.GetPool());

    aFillSet.Put( XFillStyleItem( XFILL_SOLID ) );
    aFillSet.Put( XFillColorItem( String(), RGB_Color( COL_WHITE ) ) );
    aFillSet.Put( XLineStyleItem( XLINE_NONE ) );
    pXOut->SetLineAttr( aFillSet );
    pXOut->SetFillAttr( aFillSet );
    pXOut->DrawRect( Rectangle( Point(), pVD->PixelToLogic(aVDSize) ) );

    aFillSet.Put( XFillStyleItem( XFILL_HATCH ) );
    aFillSet.Put( rSet.Get(XATTR_FILLHATCH) );
    pXOut->SetFillAttr( aFillSet );

    Size aLogicalSize = pVD->PixelToLogic(aVDSize);
    Fraction aFractionX(aLogicalSize.Width(), nDistanceX);
    Fraction aFractionY(aLogicalSize.Height(), nDistanceY);
    aMapMode.SetScaleX(aFractionX);
    aMapMode.SetScaleY(aFractionY);
    pVD->SetMapMode(aMapMode);
    pXOut->DrawRect( Rectangle( Point(), pVD->PixelToLogic(aVDSize) ) );

    Bitmap aHatchBitmap = pVD->GetBitmap( Point(), pVD->PixelToLogic(aVDSize) );

    if( pVD )
        delete pVD;
    if( pXOut )
        delete pXOut;

    return aHatchBitmap;
}

/*************************************************************************
|*
|* Give out simple line geometry
|*
\************************************************************************/

void E3dCompoundObject::GetLineGeometry(PolyPolygon3D& rLinePolyPolygon) const
{
    // use basic implementation here. Maybe optimized later.
    rLinePolyPolygon.Clear();
    B3dEntityBucket& rEntityBucket = ((E3dCompoundObject*)this)->GetDisplayGeometry().GetEntityBucket();
    GeometryIndexValueBucket& rIndexBucket = ((E3dCompoundObject*)this)->GetDisplayGeometry().GetIndexBucket();
    sal_uInt32 nPolyCounter(0);
    sal_uInt32 nEntityCounter(0);

    while(nPolyCounter < rIndexBucket.Count())
    {
        // next primitive
        sal_uInt32 nUpperBound(rIndexBucket[nPolyCounter++].GetIndex());
        Vector3D aLastPoint;

        BOOL bLastLineVisible = rEntityBucket[nUpperBound - 1].IsEdgeVisible();
        if(bLastLineVisible)
            aLastPoint = rEntityBucket[nUpperBound - 1].Point().GetVector3D();

        while(nEntityCounter < nUpperBound)
        {
            Vector3D aNewPoint = rEntityBucket[nEntityCounter].Point().GetVector3D();

            if(bLastLineVisible)
            {
                if(aLastPoint != aNewPoint)
                {
                    // fill polygon
                    Polygon3D aNewPoly(2);
                    aNewPoly[0] = aLastPoint;
                    aNewPoly[1] = aNewPoint;

                    // create line geometry for polygon in eye coor to
                    // have it always orthogonal to camera plane
                    rLinePolyPolygon.Insert(aNewPoly);
                }
            }

            bLastLineVisible = rEntityBucket[nEntityCounter++].IsEdgeVisible();
            aLastPoint = aNewPoint;
        }
    }
}

/*************************************************************************
|*
|* Geometrieerzeugung
|*
\************************************************************************/

void E3dCompoundObject::DestroyGeometry()
{
    // Alle Objekte in der Sub-Liste zerstoeren. Dies sind die
    // zur Visualisierung des Objektes verwendeten Hilfsobjekte
    pSub->Clear();
    delete pSub;
    pSub = new E3dObjList(NULL, NULL);
    pSub->SetOwnerObj(this);
    pSub->SetListKind(SDROBJLIST_GROUPOBJ);

    // Neue Geometrie zerstoeren
    aDisplayGeometry.Erase();

    // BoundVols resetten
    aLocalBoundVol  = Volume3D();
    bBoundVolValid = FALSE;
    StructureChanged(this);

    // Geometrie ist ungueltig
    bGeometryValid = FALSE;
}

void E3dCompoundObject::CreateGeometry()
{
    // Geometrie ist gueltig, um rekursion zu verhindern
    bGeometryValid = TRUE;

    // Eventuell entstandene Geometrie noch korrigieren
    // und Default -Normalen oder -Texturkoordinaten erzeugen
    if(bCreateNormals)
    {
        if(GetNormalsKind() > 1)
            GetDisplayGeometry().CreateDefaultNormalsSphere();
        if(GetNormalsInvert())
            GetDisplayGeometry().InvertNormals();
    }

    if(bCreateTexture)
    {
        GetDisplayGeometry().CreateDefaultTexture(
            ((GetTextureProjectionX() > 0) ? B3D_CREATE_DEFAULT_X : FALSE)
            |((GetTextureProjectionY() > 0) ? B3D_CREATE_DEFAULT_Y : FALSE),
            GetTextureProjectionX() > 1);
    }

    // Am Ende der Geometrieerzeugung das model an den erzeugten
    // PolyObj's setzen, d.h. beim ueberladen dieser Funktion
    // den parent am Ende rufen.
    if(bCreateE3dPolyObj)
        SetModel(pModel);

    // Das Ende der Geometrieerzeugung anzeigen
    aDisplayGeometry.EndDescription();
}

void E3dCompoundObject::ReCreateGeometry(BOOL bCreateOldGeometry)
{
    // Geometrie zerstoeren
    DestroyGeometry();

    // Flag fuer Geometrieerzeugung setzen
    bCreateE3dPolyObj = bCreateOldGeometry;

    // ... und neu erzeugen
    CreateGeometry();
}

/*************************************************************************
|*
|* Geometrieerzeugung
|*
\************************************************************************/

void E3dCompoundObject::AddGeometry(const PolyPolygon3D& rPolyPolygon3D,
    BOOL bHintIsComplex, BOOL bOutline)
{
    if(rPolyPolygon3D.Count())
    {
        // eventuell alte Geometrie erzeugen (z.B. zum speichern)
        if(bCreateE3dPolyObj)
        {
            E3dPolyObj* pObj = new E3dPolyObj(
                rPolyPolygon3D, GetDoubleSided(), TRUE);
            pObj->SetPartOfParent();
            Insert3DObj(pObj);
        }

        // neue Geometrie erzeugen
        for(USHORT a = 0; a < rPolyPolygon3D.Count(); a++ )
        {
            const Polygon3D& rPoly3D = rPolyPolygon3D[a];
            aDisplayGeometry.StartObject(bHintIsComplex, bOutline);
            for(USHORT b = 0; b < rPoly3D.GetPointCount(); b++ )
                aDisplayGeometry.AddEdge(rPoly3D[b]);
        }
        aDisplayGeometry.EndObject();

        // LocalBoundVolume pflegen
        aLocalBoundVol.Union(rPolyPolygon3D.GetPolySize());

        // Eigenes BoundVol nicht mehr gueltig
        SetBoundVolInvalid();
        SetRectsDirty();
    }
}

void E3dCompoundObject::AddGeometry(
    const PolyPolygon3D& rPolyPolygon3D,
    const PolyPolygon3D& rPolyNormal3D,
    BOOL bHintIsComplex, BOOL bOutline)
{
    if(rPolyPolygon3D.Count())
    {
        // eventuell alte Geometrie erzeugen (z.B. zum speichern)
        if(bCreateE3dPolyObj)
        {
            E3dPolyObj* pObj = new E3dPolyObj(
                rPolyPolygon3D, rPolyNormal3D, GetDoubleSided(), TRUE);
            pObj->SetPartOfParent();
            Insert3DObj(pObj);
        }

        // neue Geometrie erzeugen
        for(USHORT a = 0; a < rPolyPolygon3D.Count(); a++ )
        {
            const Polygon3D& rPoly3D = rPolyPolygon3D[a];
            const Polygon3D& rNormal3D = rPolyNormal3D[a];
            aDisplayGeometry.StartObject(bHintIsComplex, bOutline);
            for(USHORT b = 0; b < rPoly3D.GetPointCount(); b++ )
                aDisplayGeometry.AddEdge(rPoly3D[b], rNormal3D[b]);
        }
        aDisplayGeometry.EndObject();

        // LocalBoundVolume pflegen
        aLocalBoundVol.Union(rPolyPolygon3D.GetPolySize());

        // Eigenes BoundVol nicht mehr gueltig
        SetBoundVolInvalid();
        SetRectsDirty();
    }
}

void E3dCompoundObject::AddGeometry(
    const PolyPolygon3D& rPolyPolygon3D,
    const PolyPolygon3D& rPolyNormal3D,
    const PolyPolygon3D& rPolyTexture3D,
    BOOL bHintIsComplex, BOOL bOutline)
{
    if(rPolyPolygon3D.Count())
    {
        // eventuell alte Geometrie erzeugen (z.B. zum speichern)
        if(bCreateE3dPolyObj)
        {
            E3dPolyObj* pObj = new E3dPolyObj(
                rPolyPolygon3D, rPolyNormal3D,
                rPolyTexture3D, GetDoubleSided(), TRUE);
            pObj->SetPartOfParent();
            Insert3DObj(pObj);
        }

        // neue Geometrie erzeugen
        for(USHORT a = 0; a < rPolyPolygon3D.Count(); a++ )
        {
            const Polygon3D& rPoly3D = rPolyPolygon3D[a];
            const Polygon3D& rNormal3D = rPolyNormal3D[a];
            const Polygon3D& rTexture3D = rPolyTexture3D[a];
            aDisplayGeometry.StartObject(bHintIsComplex, bOutline);
            for(USHORT b = 0; b < rPoly3D.GetPointCount(); b++ )
                aDisplayGeometry.AddEdge(rPoly3D[b], rNormal3D[b], rTexture3D[b]);
        }
        aDisplayGeometry.EndObject();

        // LocalBoundVolume pflegen
        aLocalBoundVol.Union(rPolyPolygon3D.GetPolySize());

        // Eigenes BoundVol nicht mehr gueltig
        SetBoundVolInvalid();
        SetRectsDirty();
    }
}

/*************************************************************************
|*
|* Hilfsfunktionen zur Geometrieerzeugung
|*
\************************************************************************/

void E3dCompoundObject::RotatePoly(
    PolyPolygon3D& rPolyPolyRotate,
    Matrix4D& rRotMat)
{
    USHORT nPolyCnt = rPolyPolyRotate.Count();

    for(UINT16 a=0;a<nPolyCnt;a++)
    {
        Polygon3D& rPolyRotate = rPolyPolyRotate[a];
        USHORT nPntCnt = rPolyRotate.GetPointCount();

        for(UINT16 b=0;b<nPntCnt;b++)
            rPolyRotate[b] *= rRotMat;
    }
}

void E3dCompoundObject::GrowPoly(
    PolyPolygon3D& rPolyPolyGrow,
    PolyPolygon3D& rPolyPolyNormals,
    double fFactor)
{
    USHORT nPolyCnt = rPolyPolyGrow.Count();

    for(UINT16 a=0;a<nPolyCnt;a++)
    {
        Polygon3D& rPolyGrow = rPolyPolyGrow[a];
        const Polygon3D& rPolyNormals = rPolyPolyNormals[a];
        USHORT nPntCnt = rPolyGrow.GetPointCount();

        for(UINT16 b=0;b<nPntCnt;b++)
            rPolyGrow[b] += rPolyNormals[b] * fFactor;
    }
}

void E3dCompoundObject::AddNormals(
    PolyPolygon3D& rPolyPolyDest,
    const PolyPolygon3D& rPolyPolySource)
{
    USHORT nPolyCnt = rPolyPolyDest.Count();

    for(UINT16 a=0;a<nPolyCnt;a++)
    {
        Polygon3D& rPolyDest = rPolyPolyDest[a];
        const Polygon3D& rPolySource = rPolyPolySource[a];
        USHORT nPntCnt = rPolyDest.GetPointCount();

        for(UINT16 b=0;b<nPntCnt;b++)
        {
            rPolyDest[b] += rPolySource[b];
            rPolyDest[b].Normalize();
        }
    }
}

void E3dCompoundObject::ScalePoly(
    PolyPolygon3D& rPolyPolyScale,
    double fFactor)
{
    USHORT nPolyCnt = rPolyPolyScale.Count();
    Vector3D aMiddle = rPolyPolyScale.GetMiddle();

    for(UINT16 a=0;a<nPolyCnt;a++)
    {
        Polygon3D& rPolyScale = rPolyPolyScale[a];
        USHORT nPntCnt = rPolyScale.GetPointCount();

        for(UINT16 b=0;b<nPntCnt;b++)
            rPolyScale[b] = ((rPolyScale[b] - aMiddle) * fFactor) + aMiddle;
    }
}

void E3dCompoundObject::CreateFront(
    const PolyPolygon3D& rPolyPoly3D,
    const PolyPolygon3D& rFrontNormals,
    BOOL bCreateNormals,
    BOOL bCreateTexture)
{
    // Vorderseite
    if(bCreateNormals)
    {
        if(bCreateTexture)
        {
            // Polygon fuer die Textur erzeugen
            PolyPolygon3D aPolyTexture = rPolyPoly3D;
            Volume3D aSize = aPolyTexture.GetPolySize();
            Matrix4D aTrans;

            aTrans.Identity();
            aTrans.Translate(-aSize.MinVec());
            aPolyTexture.Transform(aTrans);

            double fFactorX(1.0), fFactorY(1.0), fFactorZ(1.0);

            if(aSize.GetWidth() != 0.0)
                fFactorX = 1.0 / aSize.GetWidth();

            if(aSize.GetHeight() != 0.0)
                fFactorY = 1.0 / aSize.GetHeight();

            if(aSize.GetDepth() != 0.0)
                fFactorZ = 1.0 / aSize.GetDepth();

            aTrans.Identity();
            aTrans.Scale(fFactorX, -fFactorY, fFactorZ);
            aTrans.Translate(Vector3D(0.0, 1.0, 0.0));
            aPolyTexture.Transform(aTrans);

            AddGeometry(rPolyPoly3D, rFrontNormals, aPolyTexture, TRUE);
        }
        else
            AddGeometry(rPolyPoly3D, rFrontNormals, TRUE);
    }
    else
        AddGeometry(rPolyPoly3D, TRUE);
}

void E3dCompoundObject::AddFrontNormals(
    const PolyPolygon3D& rPolyPoly3D,
    PolyPolygon3D& rNormalsFront,
    Vector3D &rOffset)
{
    Vector3D aFrontNormal = -rOffset;
    aFrontNormal.Normalize();
    USHORT nPolyCnt = rPolyPoly3D.Count();

    for(UINT16 a=0;a<nPolyCnt;a++)
    {
        const Polygon3D& rPoly3D = rPolyPoly3D[a];
        Polygon3D& rNormalPoly = rNormalsFront[a];
        USHORT nPntCnt = rPoly3D.GetPointCount();

        for(UINT16 b=0;b<nPntCnt;b++)
        {
            rNormalPoly[b] += aFrontNormal;
            rNormalPoly[b].Normalize();
        }
    }
}

void E3dCompoundObject::CreateBack(
    const PolyPolygon3D& rPolyPoly3D,
    const PolyPolygon3D& rBackNormals,
    BOOL bCreateNormals,
    BOOL bCreateTexture)
{
    // PolyPolygon umdrehen
    PolyPolygon3D aLocalPoly = rPolyPoly3D;
    aLocalPoly.FlipDirections();

    // Rueckseite
    if(bCreateNormals)
    {
        PolyPolygon3D aLocalNormals = rBackNormals;
        aLocalNormals.FlipDirections();
        if(bCreateTexture)
        {
            // Polygon fuer die Textur erzeugen
            PolyPolygon3D aPolyTexture(aLocalPoly);
            Volume3D aSize = aPolyTexture.GetPolySize();
            Matrix4D aTrans;

            aTrans.Identity();
            aTrans.Translate(-aSize.MinVec());
            aPolyTexture.Transform(aTrans);

            double fFactorX(1.0), fFactorY(1.0), fFactorZ(1.0);

            if(aSize.GetWidth() != 0.0)
                fFactorX = 1.0 / aSize.GetWidth();

            if(aSize.GetHeight() != 0.0)
                fFactorY = 1.0 / aSize.GetHeight();

            if(aSize.GetDepth() != 0.0)
                fFactorZ = 1.0 / aSize.GetDepth();

            aTrans.Identity();
            aTrans.Scale(fFactorX, -fFactorY, fFactorZ);
            aTrans.Translate(Vector3D(0.0, 1.0, 0.0));
            aPolyTexture.Transform(aTrans);

            AddGeometry(aLocalPoly, aLocalNormals, aPolyTexture, TRUE);
        }
        else
            AddGeometry(aLocalPoly, aLocalNormals, TRUE);
    }
    else
    {
        AddGeometry(aLocalPoly, TRUE);
    }
}

void E3dCompoundObject::AddBackNormals(
    const PolyPolygon3D& rPolyPoly3D,
    PolyPolygon3D& rNormalsBack,
    Vector3D& rOffset)
{
    Vector3D aBackNormal = rOffset;
    aBackNormal.Normalize();
    USHORT nPolyCnt = rPolyPoly3D.Count();

    for(UINT16 a=0;a<nPolyCnt;a++)
    {
        const Polygon3D& rPoly3D = rPolyPoly3D[a];
        Polygon3D& rNormalPoly = rNormalsBack[a];
        USHORT nPntCnt = rPoly3D.GetPointCount();

        for(UINT16 b=0;b<nPntCnt;b++)
        {
            rNormalPoly[b] += aBackNormal;
            rNormalPoly[b].Normalize();
        }
    }
}

void E3dCompoundObject::CreateInBetween(
    const PolyPolygon3D& rPolyPolyFront,
    const PolyPolygon3D& rPolyPolyBack,
    const PolyPolygon3D& rFrontNormals,
    const PolyPolygon3D& rBackNormals,
    BOOL bCreateNormals,
    double fSurroundFactor,
    double fTextureStart,
    double fTextureDepth,
    BOOL bRotateTexture90)
{
    USHORT nPolyCnt = rPolyPolyFront.Count();
    BOOL bCreateTexture = (fTextureDepth == 0.0) ? FALSE : TRUE;
    double fPolyLength, fPolyPos;
    USHORT nLastIndex;

    // Verbindungsstuecke
    if(bCreateNormals)
    {
        for(UINT16 a=0;a<nPolyCnt;a++)
        {
            const Polygon3D& rPoly3DFront = rPolyPolyFront[a];
            const Polygon3D& rPoly3DBack = rPolyPolyBack[a];

            const Polygon3D& rPolyNormalsFront = rFrontNormals[a];
            const Polygon3D& rPolyNormalsBack = rBackNormals[a];

            Polygon3D   aRect3D(4);
            Polygon3D   aNormal3D(4);
            Polygon3D   aTexture3D(4);
            USHORT nPntCnt = rPoly3DFront.GetPointCount();
            USHORT nPrefillIndex = rPoly3DFront.IsClosed() ? nPntCnt - 1 : 0;

            aRect3D[3] = rPoly3DFront[nPrefillIndex];
            aRect3D[2] = rPoly3DBack[nPrefillIndex];
            aNormal3D[3] = rPolyNormalsFront[nPrefillIndex];
            aNormal3D[2] = rPolyNormalsBack[nPrefillIndex];

            if(bCreateTexture)
            {
                fPolyLength = rPoly3DFront.GetLength();
                fPolyPos = 0.0;
                nLastIndex = rPoly3DFront.IsClosed() ? nPntCnt - 1 : 0;

                if(bRotateTexture90)
                {
                    // X,Y vertauschen
                    aTexture3D[3].X() = fTextureStart;
                    aTexture3D[3].Y() = (1.0 - fPolyPos) * fSurroundFactor;

                    aTexture3D[2].X() = fTextureStart + fTextureDepth;
                    aTexture3D[2].Y() = (1.0 - fPolyPos) * fSurroundFactor;
                }
                else
                {
                    aTexture3D[3].X() = fPolyPos * fSurroundFactor;
                    aTexture3D[3].Y() = fTextureStart;

                    aTexture3D[2].X() = fPolyPos * fSurroundFactor;
                    aTexture3D[2].Y() = fTextureStart + fTextureDepth;
                }
            }

            for (USHORT i = rPoly3DFront.IsClosed() ? 0 : 1; i < nPntCnt; i++)
            {
                aRect3D[0] = aRect3D[3];
                aRect3D[1] = aRect3D[2];

                aRect3D[3] = rPoly3DFront[i];
                aRect3D[2] = rPoly3DBack[i];

                aNormal3D[0] = aNormal3D[3];
                aNormal3D[1] = aNormal3D[2];

                aNormal3D[3] = rPolyNormalsFront[i];
                aNormal3D[2] = rPolyNormalsBack[i];

                if(bCreateTexture)
                {
                    // Texturkoordinaten ermitteln
                    Vector3D aPart = rPoly3DFront[i] - rPoly3DFront[nLastIndex];
                    fPolyPos += aPart.GetLength() / fPolyLength;
                    nLastIndex = i;

                    // Der Abschnitt am Polygon entspricht dem Teil
                    // von fPolyPos bis fPolyPos+fPartLength

                    aTexture3D[0] = aTexture3D[3];
                    aTexture3D[1] = aTexture3D[2];

                    if(bRotateTexture90)
                    {
                        // X,Y vertauschen
                        aTexture3D[3].X() = fTextureStart;
                        aTexture3D[3].Y() = (1.0 - fPolyPos) * fSurroundFactor;

                        aTexture3D[2].X() = fTextureStart + fTextureDepth;
                        aTexture3D[2].Y() = (1.0 - fPolyPos) * fSurroundFactor;
                    }
                    else
                    {
                        aTexture3D[3].X() = fPolyPos * fSurroundFactor;
                        aTexture3D[3].Y() = fTextureStart;

                        aTexture3D[2].X() = fPolyPos * fSurroundFactor;
                        aTexture3D[2].Y() = fTextureStart + fTextureDepth;
                    }

                    AddGeometry(aRect3D, aNormal3D, aTexture3D, FALSE);
                }
                else
                    AddGeometry(aRect3D, aNormal3D, FALSE);
            }
        }
    }
    else
    {
        for(UINT16 a=0;a<nPolyCnt;a++)
        {
            const Polygon3D& rPoly3DFront = rPolyPolyFront[a];
            const Polygon3D& rPoly3DBack = rPolyPolyBack[a];
            Polygon3D   aRect3D(4);
            USHORT nPntCnt = rPoly3DFront.GetPointCount();
            USHORT nPrefillIndex = rPoly3DFront.IsClosed() ? nPntCnt - 1 : 0;

            aRect3D[3] = rPoly3DFront[nPrefillIndex];
            aRect3D[2] = rPoly3DBack[nPrefillIndex];

            for (USHORT i = rPoly3DFront.IsClosed() ? 0 : 1; i < nPntCnt; i++)
            {
                aRect3D[0] = aRect3D[3];
                aRect3D[1] = aRect3D[2];

                aRect3D[3] = rPoly3DFront[i];
                aRect3D[2] = rPoly3DBack[i];

                AddGeometry(aRect3D, FALSE);
            }
        }
    }
}

void E3dCompoundObject::AddInBetweenNormals(
    const PolyPolygon3D& rPolyPolyFront,
    const PolyPolygon3D& rPolyPolyBack,
    PolyPolygon3D& rNormals,
    BOOL bSmoothed)
{
    USHORT nPolyCnt = rPolyPolyFront.Count();

    // Verbindungsstuecke
    for(UINT16 a=0;a<nPolyCnt;a++)
    {
        const Polygon3D& rPoly3DFront = rPolyPolyFront[a];
        const Polygon3D& rPoly3DBack = rPolyPolyBack[a];
        Polygon3D& rNormalPoly = rNormals[a];
        USHORT nPntCnt = rPoly3DFront.GetPointCount();

        if(rPoly3DBack.IsClosed())
        {
            Vector3D aNormal = (rPoly3DBack[nPntCnt - 1] - rPoly3DFront[nPntCnt - 1])
                      |(rPoly3DFront[0] - rPoly3DFront[nPntCnt - 1]);
            aNormal.Normalize();
            for (USHORT i = 0; i < nPntCnt; i++)
            {
                Vector3D aNextNormal = (rPoly3DBack[i] - rPoly3DFront[i])
                    |(rPoly3DFront[(i+1 == nPntCnt) ? 0 : i+1] - rPoly3DFront[i]);
                aNextNormal.Normalize();
                if(bSmoothed)
                {
                    Vector3D aMidNormal = aNormal + aNextNormal;
                    aMidNormal.Normalize();
                    rNormalPoly[i] += aMidNormal;
                }
                else
                    rNormalPoly[i] += aNormal;
                rNormalPoly[i].Normalize();
                aNormal = aNextNormal;
            }
        }
        else
        {
            Vector3D aNormal;
            if(rPoly3DBack[0] == rPoly3DFront[0])
            {
                aNormal = (rPoly3DBack[1] - rPoly3DFront[1])
                      |(rPoly3DFront[1] - rPoly3DFront[0]);
            }
            else
            {
                aNormal = (rPoly3DBack[0] - rPoly3DFront[0])
                      |(rPoly3DFront[1] - rPoly3DFront[0]);
            }
            aNormal.Normalize();
            rNormalPoly[0] += aNormal; rNormalPoly[0].Normalize();
            for (USHORT i = 1; i < nPntCnt; i++)
            {
                Vector3D aNextNormal;
                if(i+1 == nPntCnt)
                {
                    aNextNormal = aNormal;
                }
                else
                {
                    aNextNormal = (rPoly3DBack[i] - rPoly3DFront[i])
                        |(rPoly3DFront[i+1] - rPoly3DFront[i]);
                }
                aNextNormal.Normalize();
                if(bSmoothed)
                {
                    Vector3D aMidNormal = aNormal + aNextNormal;
                    aMidNormal.Normalize();
                    rNormalPoly[i] += aMidNormal;
                }
                else
                    rNormalPoly[i] += aNormal;
                rNormalPoly[i].Normalize();
                aNormal = aNextNormal;
            }
        }
    }
}

/*************************************************************************
|*
|* Copy-Operator
|*
\************************************************************************/

void E3dCompoundObject::operator=(const SdrObject& rObj)
{
    // erstmal alle Childs kopieren
    E3dObject::operator=(rObj);

    // weitere Parameter kopieren
    const E3dCompoundObject& r3DObj = (const E3dCompoundObject&) rObj;

    aDisplayGeometry = r3DObj.aDisplayGeometry;
    bCreateNormals = r3DObj.bCreateNormals;
    bCreateTexture = r3DObj.bCreateTexture;
    bGeometryValid = r3DObj.bGeometryValid;
    bBytesLeft = r3DObj.bBytesLeft;
    bCreateE3dPolyObj = r3DObj.bCreateE3dPolyObj;

    // neu ab 383:
    aMaterialAmbientColor = r3DObj.aMaterialAmbientColor;

    aBackMaterial = r3DObj.aBackMaterial;
    bUseDifferentBackMaterial = r3DObj.bUseDifferentBackMaterial;
}

/*************************************************************************
|*
|* Ausgabeparameter an 3D-Kontext setzen
|*
\************************************************************************/

void E3dCompoundObject::ImpSet3DParForFill(ExtOutputDevice& rOut, Base3D* pBase3D,
    BOOL& bDrawObject, UINT16 nDrawFlags, BOOL bGhosted, BOOL bIsFillDraft)
{
    if(bIsFillDraft)
    {
        bDrawObject = FALSE;
    }
    else
    {
        const SfxItemSet& rSet = GetItemSet();
        const XFillStyle eFillStyle = ((const XFillStyleItem&)(rSet.Get(XATTR_FILLSTYLE))).GetValue();

        if(eFillStyle == XFILL_NONE)
        {
            bDrawObject = FALSE;
        }
        else
        {
            sal_uInt16 nFillTrans = ((const XFillTransparenceItem&)(rSet.Get(XATTR_FILLTRANSPARENCE))).GetValue();
            const XFillFloatTransparenceItem& rFloatTrans = ((const XFillFloatTransparenceItem&)(rSet.Get(XATTR_FILLFLOATTRANSPARENCE)));
            BOOL bFillTransparence = (nFillTrans != 0);
            BOOL bFloatTransparence = rFloatTrans.IsEnabled();
            BOOL bAnyTransparence = (bFillTransparence || bFloatTransparence);
            BOOL bDrawTransparence = ((nDrawFlags & E3D_DRAWFLAG_TRANSPARENT) != 0);

            // force no fill transparence when float transparence
            if(bFloatTransparence)
            {
                bFillTransparence = FALSE;
                nFillTrans = 0;
            }

            if(bAnyTransparence != bDrawTransparence)
            {
                bDrawObject = FALSE;
            }
            else
            {
                // get base color
                Color aColorSolid = ((const XFillColorItem&) (rSet.Get(XATTR_FILLCOLOR))).GetValue();
                if(bGhosted)
                {
                    aColorSolid = Color(
                        (aColorSolid.GetRed() >> 1) + 0x80,
                        (aColorSolid.GetGreen() >> 1) + 0x80,
                        (aColorSolid.GetBlue() >> 1) + 0x80);
                }

                // prepare custom colors for linear transparency and black/white mode
                Color aColorSolidWithTransparency(aColorSolid);
                aColorSolidWithTransparency.SetTransparency((UINT8)(nFillTrans * 255 / 100));
                Color aColorWhite(COL_WHITE);
                Color aColorWhiteWithTransparency(COL_WHITE);
                aColorWhiteWithTransparency.SetTransparency((UINT8)(nFillTrans * 255 / 100));

                // set base materials (if no drawmode is set)
                pBase3D->SetMaterial(aColorWhite, Base3DMaterialAmbient);
                pBase3D->SetMaterial(aColorWhiteWithTransparency, Base3DMaterialDiffuse);
                pBase3D->SetMaterial(GetMaterialSpecular(), Base3DMaterialSpecular);
                pBase3D->SetMaterial(GetMaterialEmission(), Base3DMaterialEmission);
                pBase3D->SetShininess(GetMaterialSpecularIntensity());
                if(GetUseDifferentBackMaterial())
                {
                    pBase3D->SetMaterial(aColorWhite, Base3DMaterialAmbient, Base3DMaterialBack);
                    pBase3D->SetMaterial(aColorWhiteWithTransparency, Base3DMaterialDiffuse, Base3DMaterialBack);
                    pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialSpecular), Base3DMaterialSpecular, Base3DMaterialBack);
                    pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialEmission), Base3DMaterialEmission, Base3DMaterialBack);
                    pBase3D->SetShininess(aBackMaterial.GetShininess(), Base3DMaterialBack);
                }

                // prepare some more later used texture parameters
                B3dTexture* pTexture = NULL;
                Base3DTextureWrap eWrapX(Base3DTextureRepeat);
                Base3DTextureWrap eWrapY(Base3DTextureRepeat);
                Matrix4D mTexture;

                // now test the different draw modes and cases
                if((pBase3D->GetOutputDevice()->GetDrawMode() & DRAWMODE_WHITEFILL) != 0)
                {
                    // set material to black and white mode
                    pBase3D->SetMaterial(aColorWhite, Base3DMaterialAmbient);
                    pBase3D->SetMaterial(aColorWhiteWithTransparency, Base3DMaterialDiffuse);
                    if(GetUseDifferentBackMaterial())
                    {
                        pBase3D->SetMaterial(aColorWhite, Base3DMaterialAmbient, Base3DMaterialBack);
                        pBase3D->SetMaterial(aColorWhiteWithTransparency, Base3DMaterialDiffuse, Base3DMaterialBack);
                    }

                    // Color stays white, just set render mode
                    pBase3D->SetRenderMode(Base3DRenderFill);
                }
                else if(eFillStyle == XFILL_BITMAP)
                {
                    // bitmap fill, use bitmap texture from 2D defines
                    BitmapEx aBmpEx;

                    if(SFX_ITEM_SET == rSet.GetItemState(XATTR_FILLBITMAP, TRUE))
                    {
                        // EIndeutige Bitmap, benutze diese
                        aBmpEx = BitmapEx((((const XFillBitmapItem&) (rSet.Get(XATTR_FILLBITMAP))).GetValue()).GetBitmap());
                    }
                    else
                    {
                        // Keine eindeutige Bitmap. benutze default
                        //
                        // DIES IST EINE NOTLOESUNG, BIS MAN IRGENDWO AN DIE
                        // DEAULT-BITMAP RANKOMMT (IST VON KA IN VORBEREITUNG)
                        //
                        aBmpEx = BitmapEx(Bitmap(Size(4,4), 8));
                    }

                    // Texturattribute bilden
                    TextureAttributesBitmap aTexAttr(
                        bGhosted,
                        (void*)&rSet.Get(XATTR_FILLFLOATTRANSPARENCE),
                        aBmpEx.GetBitmap());
                    pTexture = pBase3D->ObtainTexture(aTexAttr);
                    if(!pTexture)
                    {
                        if(bGhosted)
                            aBmpEx.Adjust( 50 );

                        if(bFloatTransparence)
                            // add alpha channel to bitmap
                            aBmpEx = BitmapEx(aBmpEx.GetBitmap(), GetAlphaMask(rSet, aBmpEx.GetSizePixel()));

                        pTexture = pBase3D->ObtainTexture(aTexAttr, aBmpEx);
                    }

                    sal_uInt16 nOffX = ((const SfxUInt16Item&) (rSet.Get(XATTR_FILLBMP_TILEOFFSETX))).GetValue();
                    sal_uInt16 nOffY = ((const SfxUInt16Item&) (rSet.Get(XATTR_FILLBMP_TILEOFFSETY))).GetValue();
                    sal_uInt16 nOffPosX = ((const SfxUInt16Item&) (rSet.Get(XATTR_FILLBMP_POSOFFSETX))).GetValue();
                    sal_uInt16 nOffPosY = ((const SfxUInt16Item&) (rSet.Get(XATTR_FILLBMP_POSOFFSETY))).GetValue();
                    RECT_POINT eRectPoint = (RECT_POINT)((const SfxEnumItem&) (rSet.Get(XATTR_FILLBMP_POS))).GetValue();
                    BOOL bTile = ((const SfxBoolItem&) (rSet.Get(XATTR_FILLBMP_TILE))).GetValue();
                    BOOL bStretch = ((const SfxBoolItem&) (rSet.Get(XATTR_FILLBMP_STRETCH))).GetValue();
                    BOOL bLogSize = ((const SfxBoolItem&) (rSet.Get(XATTR_FILLBMP_SIZELOG))).GetValue();
                    Size aSize(
                        labs(((const SfxMetricItem&)(rSet.Get(XATTR_FILLBMP_SIZEX))).GetValue()),
                        labs(((const SfxMetricItem&)(rSet.Get(XATTR_FILLBMP_SIZEY))).GetValue()));
                    Vector3D aScaleVector(1.0, 1.0, 1.0);
                    Vector3D aTranslateVector(0.0, 0.0, 0.0);

                    // Groesse beachten, logische Groesse einer Kachel bestimmen
                    // erst mal in 1/100 mm
                    Size aLogicalSize = aBmpEx.GetPrefSize();
                    const Volume3D& rVol = GetBoundVolume();
                    if(aLogicalSize.Width() == 0 || aLogicalSize.Height() == 0)
                    {
                        // Keine logische Groesse, nimm Pixelgroesse
                        // und wandle diese um
                        aLogicalSize = Application::GetDefaultDevice()->PixelToLogic(aBmpEx.GetSizePixel(), MAP_100TH_MM);
                    }
                    else
                    {
                        aLogicalSize = OutputDevice::LogicToLogic(aLogicalSize, aBmpEx.GetPrefMapMode(), MAP_100TH_MM);
                    }

                    if(bLogSize)
                    {
                        // logische Groesse
                        if(aSize.Width() == 0 && aSize.Height() == 0)
                        {
                            // Originalgroesse benutzen, Original flag

                            // Um ein vernuenftiges Mapping bei defaults auch
                            // fuer 3D-Objekte zu erreichen, nimm die logische
                            // groesse einfach als groesser an
                            aLogicalSize.Width() /= 5; //10;
                            aLogicalSize.Height() /= 5; //10;
                        }
                        else
                        {
                            // Groesse in 100TH_MM in aSize, keine Flags
                            aLogicalSize = aSize;

                            // Um ein vernuenftiges Mapping bei defaults auch
                            // fuer 3D-Objekte zu erreichen, nimm die logische
                            // groesse einfach als groesser an
                            aLogicalSize.Width() /= 5; //10;
                            aLogicalSize.Height() /= 5; //10;
                        }
                    }
                    else
                    {
                        // relative Groesse
                        // 0..100 Prozent in aSize, relativ flag
                        aLogicalSize = Size(
                            (long)((rVol.GetWidth() * (double)aSize.Width() / 100.0) + 0.5),
                            (long)((rVol.GetHeight() * (double)aSize.Height() / 100.0) + 0.5));
                    }

                    // Skalieren
                    aScaleVector.X() = rVol.GetWidth() / (double)aLogicalSize.Width();
                    aScaleVector.Y() = rVol.GetHeight() / (double)aLogicalSize.Height();

                    if(bTile)
                    {
                        // Aneinandergefuegt drauflegen
                        double fLeftBound, fTopBound;

                        // Vertikal
                        if(eRectPoint == RP_LT || eRectPoint == RP_LM || eRectPoint == RP_LB)
                        {
                            // Links aligned starten
                            fLeftBound = 0.0;
                        }
                        else if(eRectPoint == RP_MT || eRectPoint == RP_MM || eRectPoint == RP_MB)
                        {
                            // Mittig
                            fLeftBound = (rVol.GetWidth() / 2.0)
                                - ((double)aLogicalSize.Width() / 2.0);
                        }
                        else
                        {
                            // Rechts aligned starten
                            fLeftBound = rVol.GetWidth()
                                - (double)aLogicalSize.Width();
                        }

                        // Horizontal
                        if(eRectPoint == RP_LT || eRectPoint == RP_MT || eRectPoint == RP_RT)
                        {
                            // Top aligned starten
                            fTopBound = 0.0;
                        }
                        else if(eRectPoint == RP_LM || eRectPoint == RP_MM || eRectPoint == RP_RM)
                        {
                            // Mittig
                            fTopBound = (rVol.GetHeight() / 2.0)
                                - ((double)aLogicalSize.Height() / 2.0);
                        }
                        else
                        {
                            // Bottom aligned starten
                            fTopBound = rVol.GetHeight()
                                - (double)aLogicalSize.Height();
                        }

                        // Verschieben
                        aTranslateVector.X() = fLeftBound;
                        aTranslateVector.Y() = fTopBound;

                        // Offset beachten
                        if(nOffPosX || nOffPosY)
                        {
                            aTranslateVector.X() += (double)aLogicalSize.Width() * ((double)nOffPosX / 100.0);
                            aTranslateVector.Y() += (double)aLogicalSize.Height() * ((double)nOffPosY / 100.0);
                        }
                    }
                    else
                    {
                        if(bStretch)
                        {
                            // 1x drauflegen, alles wie gehabt
                            // fertig
                            aScaleVector.X() = 1.0;
                            aScaleVector.Y() = 1.0;
                        }
                        else
                        {
                            // nur einmal benutzen
                            eWrapX = Base3DTextureSingle;
                            eWrapY = Base3DTextureSingle;

                            // Groesse beachten, zentriert anlegen
                            double fLeftBound = (rVol.GetWidth() / 2.0) - ((double)aLogicalSize.Width() / 2.0);
                            double fTopBound = (rVol.GetHeight() / 2.0) - ((double)aLogicalSize.Height() / 2.0);

                            // Verschieben
                            aTranslateVector.X() = fLeftBound;
                            aTranslateVector.Y() = fTopBound;
                        }
                    }

                    // TranslateVector anpassen
                    if(aTranslateVector.X())
                        aTranslateVector.X() /= -rVol.GetWidth();
                    if(aTranslateVector.Y())
                        aTranslateVector.Y() /= -rVol.GetHeight();

                    // Texturtransformation setzen
                    mTexture.Translate(aTranslateVector);
                    mTexture.Scale(aScaleVector);
                }
                else if(eFillStyle == XFILL_GRADIENT)
                {
                    // gradient fill. Create texture and set.
                    TextureAttributesGradient aTexAttr(
                        bGhosted,
                        (void*)&rSet.Get(XATTR_FILLFLOATTRANSPARENCE),
                        (void*)&rSet.Get(XATTR_FILLGRADIENT),
                        (void*)&rSet.Get(XATTR_GRADIENTSTEPCOUNT));

                    pTexture = pBase3D->ObtainTexture(aTexAttr);
                    if(!pTexture)
                    {
                        BitmapEx aBmpEx = BitmapEx(GetGradientBitmap(rSet));

                        if(bFloatTransparence)
                            // add alpha channel to bitmap
                            aBmpEx = BitmapEx(aBmpEx.GetBitmap(), GetAlphaMask(rSet, aBmpEx.GetSizePixel()));

                        if(bGhosted)
                            aBmpEx.Adjust( 50 );
                        pTexture = pBase3D->ObtainTexture(aTexAttr, aBmpEx);
                    }
                }
                else if(eFillStyle == XFILL_HATCH)
                {
                    // hatch fill. Create texture and set.
                    TextureAttributesHatch aTexAttr(
                        bGhosted,
                        (void*)&rSet.Get(XATTR_FILLFLOATTRANSPARENCE),
                        (void*)&rSet.Get(XATTR_FILLHATCH));

                    pTexture = pBase3D->ObtainTexture(aTexAttr);
                    if(!pTexture)
                    {
                        BitmapEx aBmpEx = GetHatchBitmap(rSet);

                        if(bFloatTransparence)
                            // add alpha channel to bitmap
                            aBmpEx = BitmapEx(aBmpEx.GetBitmap(), GetAlphaMask(rSet, aBmpEx.GetSizePixel()));

                        if(bGhosted)
                            aBmpEx.Adjust( 50 );
                        pTexture = pBase3D->ObtainTexture(aTexAttr, aBmpEx);
                    }

                    // set different texture transformation
                    mTexture.Scale(Vector3D(20.0, 20.0, 20.0));
                }
                else if(eFillStyle == XFILL_SOLID)
                {
                    if(bFloatTransparence)
                    {
                        // Texturattribute bilden
                        TextureAttributesColor aTexAttr(
                            bGhosted,
                            (void*)&rSet.Get(XATTR_FILLFLOATTRANSPARENCE),
                            aColorSolid);

                        pTexture = pBase3D->ObtainTexture(aTexAttr);
                        if(!pTexture)
                        {
                            // build single colored bitmap with draw color and add transparence bitmap
                            Size aSizeBitmap(128, 128);
                            Bitmap aForeground(aSizeBitmap, 24);
                            aForeground.Erase(aColorSolid);

                            if(bGhosted)
                                aForeground.Adjust( 50 );

                            // add alpha channel to bitmap
                            BitmapEx aBmpEx(aForeground, GetAlphaMask(rSet, aSizeBitmap));

                            pTexture = pBase3D->ObtainTexture(aTexAttr, aBmpEx);
                        }
                    }
                    else
                    {
                        // set material to base color
                        pBase3D->SetMaterial(aColorSolid, Base3DMaterialAmbient);
                        pBase3D->SetMaterial(aColorSolidWithTransparency, Base3DMaterialDiffuse);
                        if(GetUseDifferentBackMaterial())
                        {
                            pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialAmbient), Base3DMaterialAmbient, Base3DMaterialBack);
                            pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialDiffuse), Base3DMaterialDiffuse, Base3DMaterialBack);
                        }

                        // and at last, the render mode.
                        pBase3D->SetRenderMode(Base3DRenderFill);
                    }
                }
                else
                {
                    DBG_ERROR("unknown drawing mode (!)");
                }

                // use texture?
                if(pTexture)
                {
                    // set values for texture modes
                    pTexture->SetTextureKind(GetTextureKind());
                    pTexture->SetTextureMode(GetTextureMode());
                    pTexture->SetTextureFilter(GetTextureFilter() ? Base3DTextureLinear : Base3DTextureNearest);
                    pTexture->SetTextureWrapS(eWrapX);
                    pTexture->SetTextureWrapT(eWrapY);
                    pTexture->SetBlendColor(aColorSolid);
                    pTexture->SetTextureColor(aColorSolid);

                    // activate texture
                    pBase3D->SetActiveTexture(pTexture);
                    pBase3D->SetRenderMode(Base3DRenderFill);

                    // set texture transformation
                    GetScene()->GetCameraSet().SetTexture(mTexture);
                }
                else
                {
                    // switch it off.texture usage
                    pBase3D->SetActiveTexture();
                }
            }
        }
    }
}

void E3dCompoundObject::ImpSet3DParForLine(ExtOutputDevice& rOut, Base3D* pBase3D,
    BOOL& bDrawOutline, UINT16 nDrawFlags, BOOL bGhosted, BOOL bIsLineDraft)
{
    // do drawflags allow line drawing at all?
    const SfxItemSet& rSet = GetItemSet();
    sal_uInt16 nLineTransparence = ((const XLineTransparenceItem&)(rSet.Get(XATTR_LINETRANSPARENCE))).GetValue();
    BOOL bLineTransparence = (nLineTransparence != 0);
    BOOL bDrawTransparence = ((nDrawFlags & E3D_DRAWFLAG_TRANSPARENT) != 0);
    if(bLineTransparence != bDrawTransparence)
    {
        bDrawOutline = FALSE;
    }

    // if no linestyle, draw no outline
    XLineStyle aLineStyle(XLINE_NONE);
    if(bDrawOutline)
    {
        aLineStyle = ((const XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue();
        bDrawOutline = (aLineStyle != XLINE_NONE);
    }

    // special mode for black/white drawing
    // Linecolor is set to boack before (Base3d::SetColor())
    if((!bDrawOutline) && ((pBase3D->GetOutputDevice()->GetDrawMode() & DRAWMODE_WHITEFILL) != 0))
    {
        bDrawOutline = TRUE;
    }

    // does the outdev use linestyle?
    if(bDrawOutline && !rOut.GetIgnoreLineStyle())
    {
        Color aColorLine = ((const XLineColorItem&)(rSet.Get(XATTR_LINECOLOR))).GetValue();
        sal_Int32 nLineWidth = ((const XLineWidthItem&)(rSet.Get(XATTR_LINEWIDTH))).GetValue();

        if(nLineWidth && !bIsLineDraft)
        {
            Point aPnt(nLineWidth, 0);
            aPnt = pBase3D->GetOutputDevice()->LogicToPixel(aPnt) - pBase3D->GetOutputDevice()->LogicToPixel(Point());
            if(aPnt.X() <= 0)
                aPnt.X() = 1;
            pBase3D->SetLineWidth((double)aPnt.X());
        }
        else
        {
            pBase3D->SetLineWidth(1.0);
        }

        // Material setzen
        pBase3D->SetColor(aColorLine);
    }
}

void E3dCompoundObject::SetBase3DParams(ExtOutputDevice& rOut, Base3D* pBase3D,
    BOOL& bDrawObject, BOOL& bDrawOutline, UINT16 nDrawFlags, BOOL bGhosted,
    BOOL bIsLineDraft, BOOL bIsFillDraft)
{
    bDrawObject = ((nDrawFlags & E3D_DRAWFLAG_FILLED) != 0);
    if(bDrawObject)
        ImpSet3DParForFill(rOut, pBase3D, bDrawObject, nDrawFlags, bGhosted, bIsFillDraft);

    bDrawOutline = ((nDrawFlags & E3D_DRAWFLAG_OUTLINE) != 0);
    if(bDrawOutline)
        ImpSet3DParForLine(rOut, pBase3D, bDrawOutline, nDrawFlags, bGhosted, bIsLineDraft);

    // Set ObjectTrans if line or fill is still set (maybe retet by upper calls)
    if(bDrawObject || bDrawOutline)
    {
        Matrix4D mTransform = GetFullTransform();
        GetScene()->GetCameraSet().SetObjectTrans(mTransform);
        pBase3D->SetTransformationSet(&(GetScene()->GetCameraSet()));
    }
}

/*************************************************************************
|*
|* Hittest fuer 3D-Objekte, wird an Geometrie weitergegeben
|*
\************************************************************************/

SdrObject* E3dCompoundObject::CheckHit(const Point& rPnt, USHORT nTol,
    const SetOfByte* pVisiLayer) const
{
    E3dPolyScene* pScene = (E3dPolyScene*)GetScene();
    if(pScene)
    {
        // HitLine holen in ObjektKoordinaten
        // ObjectTrans setzen
        Matrix4D mTransform = ((E3dCompoundObject*)this)->GetFullTransform();
        pScene->GetCameraSet().SetObjectTrans(mTransform);

        // HitPoint Front und Back erzeugen und umrechnen
        Vector3D aFront(rPnt.X(), rPnt.Y(), 0.0);
        Vector3D aBack(rPnt.X(), rPnt.Y(), ZBUFFER_DEPTH_RANGE);
        aFront = pScene->GetCameraSet().ViewToObjectCoor(aFront);
        aBack = pScene->GetCameraSet().ViewToObjectCoor(aBack);

        const Volume3D& rBoundVol = ((E3dCompoundObject*)this)->GetBoundVolume();
        if(rBoundVol.IsValid())
        {
            double fXMax = aFront.X();
            double fXMin = aBack.X();

            if(fXMax < fXMin)
            {
                fXMax = aBack.X();
                fXMin = aFront.X();
            }

            if(rBoundVol.MinVec().X() <= fXMax && rBoundVol.MaxVec().X() >= fXMin)
            {
                double fYMax = aFront.Y();
                double fYMin = aBack.Y();

                if(fYMax < fYMin)
                {
                    fYMax = aBack.Y();
                    fYMin = aFront.Y();
                }

                if(rBoundVol.MinVec().Y() <= fYMax && rBoundVol.MaxVec().Y() >= fYMin)
                {
                    double fZMax = aFront.Z();
                    double fZMin = aBack.Z();

                    if(fZMax < fZMin)
                    {
                        fZMax = aBack.Z();
                        fZMin = aFront.Z();
                    }

                    if(rBoundVol.MinVec().Z() <= fZMax && rBoundVol.MaxVec().Z() >= fZMin)
                    {
                        // Geometrie herstellen
                        if(!bGeometryValid)
                            ((E3dCompoundObject*)this)->ReCreateGeometry();

                        // 3D Volumes schneiden sich, teste in der Geometrie
                        // auf Basis der Projektion weiter
                        if(((E3dCompoundObject*)this)->aDisplayGeometry.CheckHit(aFront, aBack, nTol) != -1L)
                            return ((E3dCompoundObject*)this);
                    }
                }
            }
        }
    }
    return NULL;
}

/*************************************************************************
|*
|* Geometrie des Objektes auf angegebenen Punkt zentrieren
|*
\************************************************************************/

void E3dCompoundObject::CenterObject(const Vector3D& rCenter)
{
    // Geometrie herstellen
    if(!bGeometryValid)
        ReCreateGeometry();

    Vector3D aOldCenter = aDisplayGeometry.GetCenter();
    Vector3D aMoveVector = rCenter - aOldCenter;
    Matrix4D aTransMat;

    aTransMat.Translate(aMoveVector);
    SetTransform(GetTransform() * aTransMat);
}

/*************************************************************************
|*
|* Schattenattribute holen
|*
\************************************************************************/

Color E3dCompoundObject::GetShadowColor()
{
    return ((SdrShadowColorItem&)(GetItem(SDRATTR_SHADOWCOLOR))).GetValue();
}

BOOL E3dCompoundObject::DrawShadowAsOutline()
{
    const SfxItemSet& rSet = GetItemSet();
    XFillStyle eFillStyle = ((XFillStyleItem&)(rSet.Get(XATTR_FILLSTYLE))).GetValue();
    XLineStyle eLineStyle = ((XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue();
    BOOL bFillAttrIsNone = eFillStyle == XFILL_NONE;
    BOOL bLineAttrIsNone = eLineStyle == XLINE_NONE;
    return (bFillAttrIsNone && !bLineAttrIsNone);
}

INT32 E3dCompoundObject::GetShadowXDistance()
{
    return (long)((SdrShadowXDistItem&)(GetItem(SDRATTR_SHADOWXDIST))).GetValue();
}

INT32 E3dCompoundObject::GetShadowYDistance()
{
    return (long)((SdrShadowYDistItem&)(GetItem(SDRATTR_SHADOWYDIST))).GetValue();
}

UINT16 E3dCompoundObject::GetShadowTransparence()
{
    return (UINT16)((SdrShadowTransparenceItem&)(GetItem(SDRATTR_SHADOWTRANSPARENCE))).GetValue();
}

BOOL E3dCompoundObject::DoDrawShadow()
{
    const SfxItemSet& rSet = GetItemSet();
    BOOL bRetval(FALSE);
    BOOL bShadOn = ((SdrShadowItem&)(rSet.Get(SDRATTR_SHADOW))).GetValue();

    if(bShadOn)
    {
        bRetval = TRUE;

        if(((XFillStyleItem&)(rSet.Get(XATTR_FILLSTYLE))).GetValue() == XFILL_NONE)
        {
            if(((XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue() == XLINE_NONE)
            {
                bRetval = FALSE;
            }
        }
    }
    return bRetval;
}

/*************************************************************************
|*
|* Objekt als WireFrame zeichnen
|*
\************************************************************************/

void E3dCompoundObject::DrawObjectWireframe(ExtOutputDevice& rXOut)
{
    UINT32 nPolyCounter = 0;
    UINT32 nEntityCounter = 0;
    UINT32 nUpperBound;
    Point aFirstPoint, aLastPoint, aNewPoint;
    B3dEntityBucket& rEntityBucket = GetDisplayGeometry().GetEntityBucket();
    GeometryIndexValueBucket& rIndexBucket = GetDisplayGeometry().GetIndexBucket();
    B3dTransformationSet& rSet = GetScene()->GetCameraSet();
    BOOL bDrawLine, bLastDrawLine;
    Vector3D aPoint;

    while(nPolyCounter < rIndexBucket.Count())
    {
        // Naechstes Primitiv
        nUpperBound = rIndexBucket[nPolyCounter++].GetIndex();
        bDrawLine = bLastDrawLine = rEntityBucket[nEntityCounter].IsEdgeVisible();
        aPoint = rSet.ObjectToViewCoor(rEntityBucket[nEntityCounter++].Point().GetVector3D());
        aFirstPoint.X() = (long)(aPoint.X() + 0.5);
        aFirstPoint.Y() = (long)(aPoint.Y() + 0.5);
        aLastPoint = aFirstPoint;

        // Polygon fuellen
        while(nEntityCounter < nUpperBound)
        {
            // Punkt holen und auf Weltkoordinaten umrechnen
            bDrawLine = rEntityBucket[nEntityCounter].IsEdgeVisible();
            aPoint = rSet.ObjectToViewCoor(rEntityBucket[nEntityCounter++].Point().GetVector3D());
            aNewPoint.X() = (long)(aPoint.X() + 0.5);
            aNewPoint.Y() = (long)(aPoint.Y() + 0.5);
            if(bLastDrawLine)
                rXOut.GetOutDev()->DrawLine(aLastPoint, aNewPoint);
            aLastPoint = aNewPoint;
            bLastDrawLine = bDrawLine;
        }

        // Polygon scliessen
        if(bLastDrawLine)
            rXOut.GetOutDev()->DrawLine(aLastPoint, aFirstPoint);
    }
}

/*************************************************************************
|*
|* Ein Segment fuer Extrude oder Lathe erzeugen
|*
\************************************************************************/

void E3dCompoundObject::CreateSegment(
    const PolyPolygon3D& rFront,        // vorderes Polygon
    const PolyPolygon3D& rBack,         // hinteres Polygon
    const PolyPolygon3D* pPrev,         // smooth uebergang zu Vorgaenger
    const PolyPolygon3D* pNext,         // smooth uebergang zu Nachfolger
    BOOL bCreateFront,                  // vorderen Deckel erzeugen
    BOOL bCreateBack,                   // hinteren Deckel erzeugen
    double fPercentDiag,                // Anteil des Deckels an der Tiefe
    BOOL bSmoothLeft,                   // Glaetten der umlaufenden Normalen links
    BOOL bSmoothRight,                  // Glaetten der umlaufenden Normalen rechts
    BOOL bSmoothFrontBack,              // Glaetten der Abschlussflaechen
    double fSurroundFactor,             // Wertebereich der Texturkoordinaten im Umlauf
    double fTextureStart,               // TexCoor ueber Extrude-Tiefe
    double fTextureDepth,               // TexCoor ueber Extrude-Tiefe
    BOOL bCreateTexture,
    BOOL bCreateNormals,
    BOOL bCharacterExtrude,             // FALSE=exakt, TRUE=ohne Ueberschneidungen
    BOOL bRotateTexture90               // Textur der Seitenflaechen um 90 Grad kippen
    )
{
    PolyPolygon3D aNormalsLeft, aNormalsRight;
    AddInBetweenNormals(rFront, rBack, aNormalsLeft, bSmoothLeft);
    AddInBetweenNormals(rFront, rBack, aNormalsRight, bSmoothRight);
    Vector3D aOffset = rBack.GetMiddle() - rFront.GetMiddle();

    // Ausnahmen: Nicht geschlossen
    if(!rFront.IsClosed())
    {
        bCreateFront = FALSE;
    }
    if(!rBack.IsClosed())
    {
        bCreateBack = FALSE;
    }

    // Ausnahmen: Einfache Linie
    if(rFront[0].GetPointCount() < 3 || (!bCreateFront && !bCreateBack))
    {
        fPercentDiag = 0.0;
    }

    if(fPercentDiag == 0.0)
    {
        // Ohne Schraegen, Vorderseite
        if(bCreateFront)
        {
            PolyPolygon3D aNormalsFront;
            AddFrontNormals(rFront, aNormalsFront, aOffset);

            if(!bSmoothFrontBack)
                CreateFront(rFront, aNormalsFront, bCreateNormals, bCreateTexture);
            if(bSmoothLeft)
                AddFrontNormals(rFront, aNormalsLeft, aOffset);
            if(bSmoothFrontBack)
                CreateFront(rFront, aNormalsLeft, bCreateNormals, bCreateTexture);
        }
        else
        {
            if(pPrev)
                AddInBetweenNormals(*pPrev, rFront, aNormalsLeft, bSmoothLeft);
        }

        // Ohne Schraegen, Rueckseite
        if(bCreateBack)
        {
            PolyPolygon3D aNormalsBack;
            AddBackNormals(rBack, aNormalsBack, aOffset);

            if(!bSmoothFrontBack)
                CreateBack(rBack, aNormalsBack, bCreateNormals, bCreateTexture);
            if(bSmoothRight)
                AddBackNormals(rBack, aNormalsRight, aOffset);
            if(bSmoothFrontBack)
                CreateBack(rBack, aNormalsRight, bCreateNormals, bCreateTexture);
        }
        else
        {
            if(pNext)
                AddInBetweenNormals(rBack, *pNext, aNormalsRight, bSmoothRight);
        }

        // eigentliches Zwischenstueck
        CreateInBetween(rFront, rBack,
            aNormalsLeft, aNormalsRight,
            bCreateNormals,
            fSurroundFactor,
            fTextureStart,
            fTextureDepth,
            bRotateTexture90);
    }
    else
    {
        // Mit Scraegen, Vorderseite
        PolyPolygon3D aLocalFront = rFront;
        PolyPolygon3D aLocalBack = rBack;
        double fExtrudeDepth, fDiagLen;
        double fTexMidStart = fTextureStart;
        double fTexMidDepth = fTextureDepth;

        if(bCreateFront || bCreateBack)
        {
            fExtrudeDepth = aOffset.GetLength();
            fDiagLen = fPercentDiag * fExtrudeDepth;
        }

        if(bCreateFront)
        {
            PolyPolygon3D aOuterFront;
            PolyPolygon3D aNormalsOuterFront;
            AddFrontNormals(aLocalFront, aNormalsOuterFront, aOffset);

            if(bCharacterExtrude)
            {
                // Polygon kopieren
                aOuterFront = aLocalFront;

                // notwendige Normalen erzeugen
                PolyPolygon3D aGrowDirection;
                AddInBetweenNormals(aLocalFront, aLocalBack, aGrowDirection, bSmoothLeft);

                // Groesse inneres Polygon merken
                Volume3D aOldSize(aLocalFront.GetPolySize());

                // Inneres Polygon vergroessern
                GrowPoly(aLocalFront, aGrowDirection, fDiagLen);

                // Inneres Polygon nach innen verschieben
                GrowPoly(aLocalFront, aNormalsOuterFront, -fDiagLen);

                // Neue Groesse inneres Polygon feststellen
                Volume3D aNewSize(aLocalFront.GetPolySize());

                // Skalierung feststellen (nur X,Y)
                Vector3D aScaleVec(aOldSize.GetWidth() / aNewSize.GetWidth(),
                    aOldSize.GetHeight() / aNewSize.GetHeight(),
                    1.0);

                // Transformation bilden
                Matrix4D aTransMat;
                aTransMat.Scale(aScaleVec);

                // aeusseres und inneres Polygon skalieren
                aLocalFront.Transform(aTransMat);
                aOuterFront.Transform(aTransMat);

                // Neue Groesse aktualisieren
                aNewSize = aLocalFront.GetPolySize();

                // Translation feststellen
                Vector3D aTransVec(aOldSize.MinVec().X() - aNewSize.MinVec().X(),
                    aOldSize.MinVec().Y() - aNewSize.MinVec().Y(),
                    0.0);

                // Transformation bilden
                aTransMat.Identity();
                aTransMat.Translate(aTransVec);

                // aeusseres und inneres Polygon skalieren
                aLocalFront.Transform(aTransMat);
                aOuterFront.Transform(aTransMat);
            }
            else
            {
                // Polygon kopieren
                aOuterFront = aLocalFront;

                // notwendige Normalen erzeugen
                PolyPolygon3D aGrowDirection;
                AddInBetweenNormals(aLocalFront, aLocalBack, aGrowDirection, bSmoothLeft);

                // Aeusseres Polygon verkleinern
                GrowPoly(aOuterFront, aGrowDirection, -fDiagLen);
                aOuterFront.CorrectGrownPoly(aLocalFront);

                // Inneres Polygon nach innen verschieben
                GrowPoly(aLocalFront, aNormalsOuterFront, -fDiagLen);
            }

            // eventuell noch glaetten
            if(bSmoothLeft)
            {
                if(bSmoothFrontBack)
                    AddInBetweenNormals(aOuterFront, aLocalFront, aNormalsOuterFront, bSmoothLeft);
                AddInBetweenNormals(aOuterFront, aLocalFront, aNormalsLeft, bSmoothLeft);
            }

            // vordere Zwischenstuecke erzeugen
            CreateInBetween(aOuterFront, aLocalFront,
                aNormalsOuterFront, aNormalsLeft,
                bCreateNormals,
                fSurroundFactor,
                fTextureStart,
                fTextureDepth * fPercentDiag,
                bRotateTexture90);

            // Vorderseite erzeugen
            CreateFront(aOuterFront, aNormalsOuterFront, bCreateNormals, bCreateTexture);

            // Weitere Texturwerte setzen
            fTexMidStart += fTextureDepth * fPercentDiag;
            fTexMidDepth -= fTextureDepth * fPercentDiag;
        }
        else
        {
            if(pPrev)
                AddInBetweenNormals(*pPrev, rFront, aNormalsLeft, bSmoothLeft);
        }

        // Mit Scraegen, Rueckseite
        if(bCreateBack)
        {
            PolyPolygon3D aOuterBack;
            PolyPolygon3D aNormalsOuterBack;
            AddBackNormals(aLocalBack, aNormalsOuterBack, aOffset);

            if(bCharacterExtrude)
            {
                // Polygon kopieren
                aOuterBack = aLocalBack;

                // notwendige Normalen erzeugen
                PolyPolygon3D aGrowDirection;
                AddInBetweenNormals(aLocalFront, aLocalBack, aGrowDirection, bSmoothRight);

                // Groesse inneres Polygon merken
                Volume3D aOldSize(aLocalBack.GetPolySize());

                // Inneres Polygon vergroessern
                GrowPoly(aLocalBack, aGrowDirection, fDiagLen);

                // Inneres Polygon nach innen verschieben
                GrowPoly(aLocalBack, aNormalsOuterBack, -fDiagLen);

                // Neue Groesse inneres Polygon feststellen
                Volume3D aNewSize(aLocalBack.GetPolySize());

                // Skalierung feststellen (nur X,Y)
                Vector3D aScaleVec(aOldSize.GetWidth() / aNewSize.GetWidth(),
                    aOldSize.GetHeight() / aNewSize.GetHeight(),
                    1.0);

                // Transformation bilden
                Matrix4D aTransMat;
                aTransMat.Scale(aScaleVec);

                // aeusseres und inneres Polygon skalieren
                aLocalBack.Transform(aTransMat);
                aOuterBack.Transform(aTransMat);

                // Neue Groesse aktualisieren
                aNewSize = aLocalBack.GetPolySize();

                // Translation feststellen
                Vector3D aTransVec(aOldSize.MinVec().X() - aNewSize.MinVec().X(),
                    aOldSize.MinVec().Y() - aNewSize.MinVec().Y(),
                    0.0);

                // Transformation bilden
                aTransMat.Identity();
                aTransMat.Translate(aTransVec);

                // aeusseres und inneres Polygon skalieren
                aLocalBack.Transform(aTransMat);
                aOuterBack.Transform(aTransMat);
            }
            else
            {
                // Polygon kopieren
                aOuterBack = aLocalBack;

                // notwendige Normalen erzeugen
                PolyPolygon3D aGrowDirection;
                AddInBetweenNormals(aLocalFront, aLocalBack, aGrowDirection, bSmoothRight);

                // Aeusseres Polygon verkleinern
                GrowPoly(aOuterBack, aGrowDirection, -fDiagLen);
                aOuterBack.CorrectGrownPoly(aLocalBack);

                // Inneres Polygon nach innen verschieben
                GrowPoly(aLocalBack, aNormalsOuterBack, -fDiagLen);
            }

            // eventuell noch glaetten
            if(bSmoothRight)
            {
                if(bSmoothFrontBack)
                    AddInBetweenNormals(aLocalBack, aOuterBack, aNormalsOuterBack, bSmoothRight);
                AddInBetweenNormals(aLocalBack, aOuterBack, aNormalsRight, bSmoothRight);
            }

            // vordere Zwischenstuecke erzeugen
            // hintere Zwischenstuecke erzeugen
            CreateInBetween(aLocalBack, aOuterBack,
                aNormalsRight, aNormalsOuterBack,
                bCreateNormals,
                fSurroundFactor,
                fTextureStart + (fTextureDepth * (1.0 - fPercentDiag)),
                fTextureDepth * fPercentDiag,
                bRotateTexture90);

            // Rueckseite erzeugen
            CreateBack(aOuterBack, aNormalsOuterBack, bCreateNormals, bCreateTexture);

            // Weitere Texturwerte setzen
            fTexMidDepth -= fTextureDepth * fPercentDiag;
        }
        else
        {
            if(pNext)
                AddInBetweenNormals(rBack, *pNext, aNormalsRight, bSmoothRight);
        }

        // eigentliches Zwischenstueck
        CreateInBetween(aLocalFront, aLocalBack,
            aNormalsLeft, aNormalsRight,
            bCreateNormals,
            fSurroundFactor,
            fTexMidStart,
            fTexMidDepth,
            bRotateTexture90);
    }
}

/*************************************************************************
|*
|* Parameter Geometrieerzeugung setzen
|*
\************************************************************************/

void E3dCompoundObject::SetCreateNormals(BOOL bNew)
{
    if(bCreateNormals != bNew)
    {
        bCreateNormals = bNew;
        bGeometryValid = FALSE;
    }
}

void E3dCompoundObject::SetCreateTexture(BOOL bNew)
{
    if(bCreateTexture != bNew)
    {
        bCreateTexture = bNew;
        bGeometryValid = FALSE;
    }
}

/*************************************************************************
|*
|* DisplayGeometry rausruecken
|*
\************************************************************************/

B3dGeometry& E3dCompoundObject::GetDisplayGeometry()
{
    // Geometrie herstellen
    if(!bGeometryValid)
        ReCreateGeometry();

    return aDisplayGeometry;
}

/*************************************************************************
|*
|* Material des Objektes
|*
\************************************************************************/

void E3dCompoundObject::SetMaterialAmbientColor(const Color& rColor)
{
    if(aMaterialAmbientColor != rColor)
    {
        aMaterialAmbientColor = rColor;
    }
}

void E3dCompoundObject::SetBackMaterial(const B3dMaterial& rNew)
{
    if(aBackMaterial != rNew)
    {
        aBackMaterial = rNew;
    }
}

void E3dCompoundObject::SetUseDifferentBackMaterial(BOOL bNew)
{
    if(bUseDifferentBackMaterial != bNew)
    {
        bUseDifferentBackMaterial = bNew;
    }
}

/*************************************************************************
|*
|* 3D Ausgabe
|*
\************************************************************************/

void E3dCompoundObject::Paint3D(ExtOutputDevice& rOut, Base3D* pBase3D,
    const SdrPaintInfoRec& rInfoRec, UINT16 nDrawFlags)
{
    // call parent, draw all subobjects
    E3dObject::Paint3D(rOut, pBase3D, rInfoRec, nDrawFlags);

    // Feststellen, ob das Objekt dargestellt werden muss, was die Layer angeht
    BOOL bPrinter = (pBase3D->GetOutputDevice()->GetOutDevType()==OUTDEV_PRINTER);
    const SetOfByte* pVisiLayer=&rInfoRec.aPaintLayer;
    E3dScene* pScene = GetScene();
    BOOL bOnlySelectedCriteria = (pScene && (!pScene->DoDrawOnlySelected() || GetSelected()));

    if((!bPrinter || IsPrintable()) && pVisiLayer->IsSet(GetLayer()) && bOnlySelectedCriteria)
    {
        // Ausgabeparameter setzen
        BOOL bDrawOutline;
        BOOL bDrawObject;
        BOOL bIsLineDraft((rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTLINE) != 0);
        BOOL bIsFillDraft((rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTFILL) != 0);
        SetBase3DParams(rOut, pBase3D, bDrawObject, bDrawOutline, nDrawFlags,
            (rInfoRec.pPV && rInfoRec.pPV->GetView().DoVisualizeEnteredGroup()) ? rInfoRec.bNotActive : FALSE,
            bIsLineDraft, bIsFillDraft);

        // Culling?
        pBase3D->SetCullMode(GetDoubleSided() ? Base3DCullNone : Base3DCullBack);

        // Objekt flat darstellen?
        BOOL bForceFlat = ((GetNormalsKind() > 0) && !(GetNormalsKind() > 1));
        pBase3D->SetForceFlat(bForceFlat);

        // Geometrie ausgeben
        if(bDrawObject)
            pBase3D->DrawPolygonGeometry(GetDisplayGeometry());

        // Outline ausgeben
        if(bDrawOutline && pBase3D->GetLightGroup())
        {
            BOOL bLightingWasEnabled = pBase3D->GetLightGroup()->IsLightingEnabled();
            pBase3D->GetLightGroup()->EnableLighting(FALSE);
            pBase3D->SetLightGroup(pBase3D->GetLightGroup());

            // #79585#
            pBase3D->SetActiveTexture();

            // #78972#
            // detect if lines need to be drawn specifically
            const SfxItemSet& rSet = GetItemSet();
            sal_Int32 nLineWidth = ((const XLineWidthItem&)(rSet.Get(XATTR_LINEWIDTH))).GetValue();
            XLineStyle aLineStyle = ((const XLineStyleItem&)(rSet.Get(XATTR_LINESTYLE))).GetValue();
            BOOL bDrawLineSolidHair = (aLineStyle == XLINE_SOLID && nLineWidth == 0);

            if(bDrawLineSolidHair)
            {
                // simply draw the object geometry as line (as done before)
                pBase3D->DrawPolygonGeometry(GetDisplayGeometry(), TRUE);
            }
            else
            {
                // convert object geometry to line geometry and draw as polygons
                // in 3D space
                PolyPolygon3D aPolyPoly3D;
                PolyPolygon3D aLinePoly3D;

                // get LineStyleParameterPack
                LineStyleParameterPack aLineAttr(rSet, FALSE, *rOut.GetOutDev());
                LineGeometryCreator aLineCreator(aLineAttr, aPolyPoly3D, aLinePoly3D, FALSE);

                // get camera set
                B3dTransformationSet* pTransSet = pBase3D->GetTransformationSet();

                // get object geometry in eye coor
                PolyPolygon3D aLinePolyPolygon;
                GetLineGeometry(aLinePolyPolygon);
                Matrix4D aMatObjectToEye = pTransSet->GetObjectTrans();
                aMatObjectToEye *= pTransSet->GetOrientation();

                for(sal_uInt16 nInd(0); nInd < aLinePolyPolygon.Count(); nInd++)
                {
                    // create line geometry for polygon in eye coor to
                    // have it always orthogonal to camera plane
                    Polygon3D aLinePoly = aLinePolyPolygon.GetObject(nInd);
                    aLinePoly.Transform(aMatObjectToEye);
                    aLineCreator.AddPolygon3D(aLinePoly);
                }

                if(aPolyPoly3D.Count() || aLinePoly3D.Count())
                {
                    Color aColorLine = ((const XLineColorItem&)(rSet.Get(XATTR_LINECOLOR))).GetValue();
                    BOOL bGhosted = (rInfoRec.pPV && rInfoRec.pPV->GetView().DoVisualizeEnteredGroup()) ? rInfoRec.bNotActive : FALSE;

                    if(bGhosted)
                        aColorLine = Color((aColorLine.GetRed() >> 1) + 0x80, (aColorLine.GetGreen() >> 1) + 0x80, (aColorLine.GetBlue() >> 1) + 0x80);

                    // set line color
                    pBase3D->SetColor(aColorLine);

                    // set base color
                    sal_uInt16 nLineTrans = ((const XLineTransparenceItem&)(rSet.Get(XATTR_LINETRANSPARENCE))).GetValue();
                    Color aColorLineWithTransparency(aColorLine);
                    aColorLineWithTransparency.SetTransparency((UINT8)(nLineTrans * 255 / 100));

                    pBase3D->SetMaterial(aColorLine, Base3DMaterialAmbient);
                    pBase3D->SetMaterial(aColorLineWithTransparency, Base3DMaterialDiffuse);
                    pBase3D->SetMaterial(GetMaterialSpecular(), Base3DMaterialSpecular);
                    pBase3D->SetMaterial(GetMaterialEmission(), Base3DMaterialEmission);
                    pBase3D->SetShininess(GetMaterialSpecularIntensity());
                    if(GetUseDifferentBackMaterial())
                    {
                        pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialAmbient), Base3DMaterialAmbient, Base3DMaterialBack);
                        pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialDiffuse), Base3DMaterialDiffuse, Base3DMaterialBack);
                        pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialSpecular), Base3DMaterialSpecular, Base3DMaterialBack);
                        pBase3D->SetMaterial(aBackMaterial.GetMaterial(Base3DMaterialEmission), Base3DMaterialEmission, Base3DMaterialBack);
                        pBase3D->SetShininess(aBackMaterial.GetShininess(), Base3DMaterialBack);
                    }

                    if((pBase3D->GetOutputDevice()->GetDrawMode() & DRAWMODE_WHITEFILL) != 0)
                    {
                        // set material to black and white mode
                        Color aColorWhite(COL_WHITE);
                        Color aColorWhiteWithTransparency(COL_WHITE);
                        aColorWhiteWithTransparency.SetTransparency((UINT8)(nLineTrans * 255 / 100));

                        pBase3D->SetMaterial(aColorWhite, Base3DMaterialAmbient);
                        pBase3D->SetMaterial(aColorWhiteWithTransparency, Base3DMaterialDiffuse);

                        if(GetUseDifferentBackMaterial())
                        {
                            pBase3D->SetMaterial(aColorWhite, Base3DMaterialAmbient, Base3DMaterialBack);
                            pBase3D->SetMaterial(aColorWhiteWithTransparency, Base3DMaterialDiffuse, Base3DMaterialBack);
                        }
                    }

                    if(aPolyPoly3D.Count())
                    {
                        // draw the line geometry as area polygons
                        pBase3D->SetRenderMode(Base3DRenderFill);
                        pBase3D->SetPolygonOffset(Base3DPolygonOffsetLine, FALSE);

                        for(sal_uInt32 a(0); a < aPolyPoly3D.Count(); a++)
                        {
                            // start new primitive
                            pBase3D->StartPrimitive(Base3DPolygon);
                            const Polygon3D& rPolygon = aPolyPoly3D[(sal_uInt16)a];

                            for(sal_uInt32 b(0); b < rPolygon.GetPointCount(); b++)
                            {
                                // use backward loop to have the correct orientation since
                                // the geometry was created in eye coor (with Z-flipped coordinate system)
                                Vector3D aVec = rPolygon[rPolygon.GetPointCount() - sal_uInt16(b + 1)];
                                aVec = pTransSet->EyeToObjectCoor(aVec);
                                pBase3D->AddVertex(aVec);
                            }

                            // draw primitive
                            pBase3D->EndPrimitive();
                        }
                    }

                    if(aLinePoly3D.Count())
                    {
                        // draw the line geometry as 3d lines
                        pBase3D->SetRenderMode(Base3DRenderLine);
                        pBase3D->SetPolygonOffset(Base3DPolygonOffsetLine, TRUE);

                        for(sal_uInt32 a(0); a < aLinePoly3D.Count(); a++)
                        {
                            // start new primitive
                            pBase3D->StartPrimitive(Base3DLineStrip);
                            const Polygon3D& rPolygon = aLinePoly3D[(sal_uInt16)a];

                            for(sal_uInt32 b(0); b < rPolygon.GetPointCount(); b++)
                            {
                                Vector3D aVec = rPolygon[sal_uInt16(b)];
                                aVec = pTransSet->EyeToObjectCoor(aVec);
                                pBase3D->AddVertex(aVec);
                            }

                            // draw primitive
                            pBase3D->EndPrimitive();
                        }

                        pBase3D->SetPolygonOffset(Base3DPolygonOffsetLine, FALSE);
                    }
                }
            }

            pBase3D->GetLightGroup()->EnableLighting(bLightingWasEnabled);
            pBase3D->SetLightGroup(pBase3D->GetLightGroup());
        }
    }

#ifdef DBG_UTIL     // SnapRect und BoundRect zum testen zeichnen
    static BOOL bDoDrawSnapBoundToMakeThemVisible = FALSE;
    if(bDoDrawSnapBoundToMakeThemVisible)
    {
        OutputDevice* pOut = rOut.GetOutDev();

        // SnapRect in Rot
        Rectangle aTempRect = GetSnapRect();
        pOut->SetLineColor(Color(COL_RED));
        pOut->SetFillColor();
        pOut->DrawRect(aTempRect);

        // BoundRect in Gruen
        aTempRect = GetBoundRect();
        pOut->SetLineColor(Color(COL_GREEN));
        pOut->SetFillColor();
        pOut->DrawRect(aTempRect);
    }
#endif
}

/*************************************************************************
|*
|* Objekt als Kontur in das Polygon einfuegen
|*
\************************************************************************/

void E3dCompoundObject::TakeContour3D(XPolyPolygon& rPoly)
{
    // call parent
    E3dObject::TakeContour3D(rPoly);

    // Kontur dieses Objektes liefern
    UINT32 nPolyCounter = 0;
    UINT32 nEntityCounter = 0;
    UINT32 nUpperBound;
    B3dEntityBucket& rEntityBucket = GetDisplayGeometry().GetEntityBucket();
    GeometryIndexValueBucket& rIndexBucket = GetDisplayGeometry().GetIndexBucket();
    B3dTransformationSet& rSet = GetScene()->GetCameraSet();
    Vector3D aPoint;
    Point aNewPoint;

    // ObjectTrans setzen
    Matrix4D mTransform = GetFullTransform();
    rSet.SetObjectTrans(mTransform);

    while(nPolyCounter < rIndexBucket.Count())
    {
        // Naechstes Primitiv
        nUpperBound = rIndexBucket[nPolyCounter++].GetIndex();
        XPolygon aNewPart(UINT16(nUpperBound - nEntityCounter));
        UINT16 nIndex = 0;

        while(nEntityCounter < nUpperBound)
        {
            aPoint = rSet.ObjectToViewCoor(rEntityBucket[nEntityCounter++].Point().GetVector3D());
            aNewPart[nIndex  ].X() = (long)(aPoint.X() + 0.5);
            aNewPart[nIndex++].Y() = (long)(aPoint.Y() + 0.5);
        }

        // Teilprimitiv einfuegen
        rPoly.Insert(aNewPart);
    }

    // add shadow now too (#61279#)
    PolyPolygon aShadowPolyPoly;
    GetShadowPolygon(aShadowPolyPoly);

    for(UINT16 a = 0; a < aShadowPolyPoly.Count(); a++)
    {
        XPolygon aNewPart(aShadowPolyPoly[a]);
        rPoly.Insert(aNewPart);
    }
}

/*************************************************************************
|*
|* Schatten fuer 3D-Objekte zeichnen
|*
\************************************************************************/

void E3dCompoundObject::DrawShadows(Base3D *pBase3D, ExtOutputDevice& rXOut,
    const Rectangle& rBound, const Volume3D& rVolume,
    const SdrPaintInfoRec& rInfoRec)
{
    // call parent
    E3dObject::DrawShadows(pBase3D, rXOut, rBound, rVolume, rInfoRec);

    // Schatten fuer dieses Objekt zeichnen
    // Feststellen, ob das Objekt dargestellt werden muss, was die Layer angeht
    BOOL bPrinter = (pBase3D->GetOutputDevice()->GetOutDevType()==OUTDEV_PRINTER);
    const SetOfByte* pVisiLayer=&rInfoRec.aPaintLayer;
    if(DoDrawShadow()
        && (!bPrinter || IsPrintable())
        && pVisiLayer->IsSet(GetLayer()))
    {
        // ObjectTrans setzen
        Matrix4D mTransform = GetFullTransform();
        GetScene()->GetCameraSet().SetObjectTrans(mTransform);

        // Schattenpolygon holen
        PolyPolygon aShadowPoly;
        GetShadowPolygon(aShadowPoly);

        // ...und Zeichnen
        DrawShadowPolygon(aShadowPoly, rXOut);
    }
}

void E3dCompoundObject::GetShadowPolygon(PolyPolygon& rPoly)
{
    INT32 nXDist = GetShadowXDistance();
    INT32 nYDist = GetShadowYDistance();
    UINT32 nPolyCounter = 0;
    UINT32 nEntityCounter = 0;
    UINT32 nUpperBound;
    UINT16 nPolyPos = 0;
    Point aPolyPoint;

    // Buckets der Geometrie holen
    B3dTransformationSet& rSet = GetScene()->GetCameraSet();
    B3dEntityBucket& rEntityBucket = GetDisplayGeometry().GetEntityBucket();
    GeometryIndexValueBucket& rIndexBucket = GetDisplayGeometry().GetIndexBucket();

    if(GetShadow3D())
    {
        // 3D Schatten. Nimm Lichtquelle und Ebene. Projiziere
        // die Punkte und jage sie durch die 3D Darstellung.
        Vector3D aLampPositionOrDirection;
        BOOL bDirectionalSource(TRUE);
        Vector3D aGroundPosition;
        Vector3D aGroundDirection;
        B3dLightGroup& rLightGroup = GetScene()->GetLightGroup();

        // Lampe waehlen
        Base3DLightNumber aLightNumber = Base3DLight0;
        BOOL bLightNumberValid(FALSE);
        while(!bLightNumberValid && aLightNumber <= Base3DLight7)
        {
            if(rLightGroup.IsEnabled(aLightNumber))
                bLightNumberValid = TRUE;
            else
                aLightNumber = (Base3DLightNumber)((UINT16)aLightNumber + 1);
        }

        if(bLightNumberValid)
        {
            // Position oder Vektor aus der Lampe extrahieren
            if(rLightGroup.IsDirectionalSource(aLightNumber))
            {
                // Nur Richtung vorhanden
                aLampPositionOrDirection = -rLightGroup.GetDirection(aLightNumber);
                aLampPositionOrDirection.Normalize();
            }
            else
            {
                // Nur Position vorhanden
                aLampPositionOrDirection = rLightGroup.GetPosition(aLightNumber);
                bDirectionalSource = FALSE;
            }

            // Ebene holen, Richtung in Augkoordinaten
            aGroundDirection = -GetScene()->GetShadowPlaneDirection();
            aGroundDirection.Normalize();

            // Ist die Lampe auch vor der Ebene?
            Vector3D aLightNormal = aLampPositionOrDirection;
            if(!bDirectionalSource)
            {
                // Nur Position vorhanden, berechne einen Lichtvektor
                aLightNormal = GetDisplayGeometry().GetEntityBucket()[0].Point().GetVector3D()
                    - aLampPositionOrDirection;
                aLightNormal.Normalize();
            }

            double fLightAndNormal = aLightNormal.Scalar(aGroundDirection);
            B3dVolume aVolume = rSet.GetDeviceVolume();

            // auf Augkoordinaten umstellen
            double fTemp = aVolume.MinVec().Z();
            aVolume.MinVec().Z() = -aVolume.MaxVec().Z();
            aVolume.MaxVec().Z() = -fTemp;

            if(fLightAndNormal > 0.0)
            {
                // Position der Ebene in Augkoordinaten setzen
                aGroundPosition.X() = (aGroundDirection.X() < 0.0) ? aVolume.MinVec().X() : aVolume.MaxVec().X();
                aGroundPosition.Y() = (aGroundDirection.Y() < 0.0) ? aVolume.MinVec().Y() : aVolume.MaxVec().Y();
                aGroundPosition.Z() = aVolume.MinVec().Z() - ((aVolume.MaxVec().Z() - aVolume.MinVec().Z()) / 8.0);

                // Skalar der Ebenengleichung holen
                double fGroundScalar = -aGroundPosition.Scalar(aGroundDirection);

                // ObjectTrans setzen
                BOOL bPolygonVisible(TRUE);
                B3dTransformationSet& rSet = GetScene()->GetCameraSet();
                Matrix4D mTransform = GetFullTransform();
                rSet.SetObjectTrans(mTransform);

                while(nPolyCounter < rIndexBucket.Count())
                {
                    // Naechstes Primitiv
                    nUpperBound = rIndexBucket[nPolyCounter++].GetIndex();
                    nPolyPos = 0;
                    bPolygonVisible = TRUE;
                    Polygon aPoly((UINT16)(nUpperBound - nEntityCounter));

                    // Polygon fuellen
                    while(nEntityCounter < nUpperBound)
                    {
                        // Naechsten Punkt holen
                        Vector3D aPoint = rEntityBucket[nEntityCounter++].Point().GetVector3D();

                        // Auf Augkoordinaten umrechnen
                        aPoint = rSet.ObjectToEyeCoor(aPoint);

                        // Richtung bestimmen
                        Vector3D aDirection = aLampPositionOrDirection;
                        if(!bDirectionalSource)
                        {
                            aDirection = aPoint - aLampPositionOrDirection;
                            aDirection.Normalize();
                        }

                        // Schnittpunkt berechnen (N.D)
                        double fDiv = aGroundDirection.Scalar(aDirection);
                        if(fabs(fDiv) < SMALL_DVALUE)
                        {
                            bPolygonVisible = FALSE;
                        }
                        else
                        {
                            fDiv = -((fGroundScalar + aGroundDirection.Scalar(aPoint)) / fDiv);
                            aPoint += aDirection * fDiv;
                        }

                        // Punkt normal transformieren
                        if(bPolygonVisible)
                        {
                            // Auf ViewKoordinaten
                            Vector3D aShadowPoint = rSet.EyeToViewCoor(aPoint);
                            aPolyPoint.X() = (long)(aShadowPoint.X() + 0.5) + nXDist;
                            aPolyPoint.Y() = (long)(aShadowPoint.Y() + 0.5) + nYDist;
                            aPoly[nPolyPos++] = aPolyPoint;
                        }
                    }

                    // Teilpolygon einfuegen
                    rPoly.Insert(aPoly);
                }
            }
        }
    }
    else
    {
        // ObjectTrans setzen
        Matrix4D mTransform = GetFullTransform();
        rSet.SetObjectTrans(mTransform);

        if(FALSE /* #78972# DrawShadowAsOutline()*/)
        {
            //SubPolygon mit Punktpaaren bilden
            UINT16 nPolySize = (UINT16)(rEntityBucket.Count() * 2);
            Polygon aPoly(nPolySize);
            Point aLastPolyPoint;
            nPolyPos = 0;

            while(nPolyCounter < rIndexBucket.Count())
            {
                // Naechstes Primitiv
                nUpperBound = rIndexBucket[nPolyCounter++].GetIndex();

                // Polygon bilden
                BOOL bLastLineVisible = rEntityBucket[nUpperBound - 1].IsEdgeVisible();
                if(bLastLineVisible)
                {
                    Vector3D aLast = rEntityBucket[nUpperBound - 1].Point().GetVector3D();
                    aLast = rSet.ObjectToViewCoor(aLast);
                    aLastPolyPoint.X() = (long)(aLast.X() + 0.5) + nXDist;
                    aLastPolyPoint.Y() = (long)(aLast.Y() + 0.5) + nYDist;
                }

                while(nEntityCounter < nUpperBound)
                {
                    Vector3D aPoint = rEntityBucket[nEntityCounter].Point().GetVector3D();
                    aPoint = rSet.ObjectToViewCoor(aPoint);
                    aPolyPoint.X() = (long)(aPoint.X() + 0.5) + nXDist;
                    aPolyPoint.Y() = (long)(aPoint.Y() + 0.5) + nYDist;

                    // Linie aLast, aPoint erfassen
                    if(bLastLineVisible)
                    {
                        if(nPolyPos + 2 > nPolySize)
                        {
                            // Polygon muss groesser
                            nPolySize *= 2;
                            aPoly.SetSize(nPolySize);
                        }
                        aPoly[nPolyPos++] = aLastPolyPoint;
                        aPoly[nPolyPos++] = aPolyPoint;
                    }

                    // naechster Punkt
                    aLastPolyPoint = aPolyPoint;
                    bLastLineVisible = rEntityBucket[nEntityCounter++].IsEdgeVisible();
                }
            }

            // Wahre Groesse setzen
            aPoly.SetSize(nPolyPos);

            // Teilpolygon einfuegen
            rPoly.Insert(aPoly);
        }
        else
        {
            while(nPolyCounter < rIndexBucket.Count())
            {
                // Naechstes Primitiv
                nUpperBound = rIndexBucket[nPolyCounter++].GetIndex();
                Polygon aPoly((UINT16)(nUpperBound - nEntityCounter));
                nPolyPos = 0;

                // Polygon fuellen
                while(nEntityCounter < nUpperBound)
                {
                    Vector3D aPoint = rEntityBucket[nEntityCounter++].Point().GetVector3D();
                    aPoint = rSet.ObjectToViewCoor(aPoint);
                    aPolyPoint.X() = (long)(aPoint.X() + 0.5) + nXDist;
                    aPolyPoint.Y() = (long)(aPoint.Y() + 0.5) + nYDist;
                    aPoly[nPolyPos++] = aPolyPoint;
                }

                // Teilpolygon einfuegen
                rPoly.Insert(aPoly);
            }
        }
    }
}

void E3dCompoundObject::DrawShadowPolygon(PolyPolygon& rPoly, ExtOutputDevice& rXOut)
{
    Color aCol = GetShadowColor();
    OutputDevice *pDevice = rXOut.GetOutDev();
    BOOL bDrawAsOutline(DrawShadowAsOutline());

    UINT16 nTransparence = GetShadowTransparence();
    if(nTransparence)
    {
        if(nTransparence != 100)
        {
            // transparence, draw to metafile and then transparent to
            // outdev
            UINT8 nScaledTrans((UINT8)((nTransparence * 255)/100));
            Color aTransColor(nScaledTrans, nScaledTrans, nScaledTrans);
            Gradient aGradient(GRADIENT_LINEAR, aTransColor, aTransColor);
            GDIMetaFile aMetaFile;
            VirtualDevice aVDev;
            MapMode aMap(rXOut.GetOutDev()->GetMapMode());

            // StepCount to someting small
            aGradient.SetSteps(3);

            // create BoundRectangle
            Rectangle aBound(rPoly.GetBoundRect());

            // prepare VDev and MetaFile
            aVDev.EnableOutput(FALSE);
            aVDev.SetMapMode(rXOut.GetOutDev()->GetMapMode());
            aMetaFile.Record(&aVDev);

            if(bDrawAsOutline)
            {
                aVDev.SetLineColor(aCol);
                aVDev.SetFillColor();
            }
            else
            {
                aVDev.SetLineColor();
                aVDev.SetFillColor(aCol);
            }

            aVDev.SetFont(rXOut.GetOutDev()->GetFont());
            aVDev.SetDrawMode(rXOut.GetOutDev()->GetDrawMode());
            aVDev.SetRefPoint(rXOut.GetOutDev()->GetRefPoint());

            // create output
            for(UINT16 a(0); a < rPoly.Count(); a++)
                aMetaFile.AddAction(new MetaPolygonAction(rPoly[a]));

            // draw metafile
            aMetaFile.Stop();
            aMetaFile.WindStart();
            aMap.SetOrigin(aBound.TopLeft());
            aMetaFile.SetPrefMapMode(aMap);
            aMetaFile.SetPrefSize(aBound.GetSize());
            rXOut.GetOutDev()->DrawTransparent(aMetaFile, aBound.TopLeft(), aBound.GetSize(), aGradient);
        }
    }
    else
    {
        // no transparence, draw all single polys directly
        if(bDrawAsOutline)
        {
            pDevice->SetLineColor(aCol);
            pDevice->SetFillColor();
        }
        else
        {
            pDevice->SetLineColor();
            pDevice->SetFillColor(aCol);
        }

        for(UINT16 a(0); a < rPoly.Count(); a++)
            pDevice->DrawPolygon(rPoly[a]);
    }
}

/*************************************************************************
|*
|* convert given PolyPolygon3D to screen coor
|*
\************************************************************************/

XPolyPolygon E3dCompoundObject::TransformToScreenCoor(const PolyPolygon3D &rExtrudePoly)
{
    XPolyPolygon aNewPolyPolygon;
    B3dTransformationSet& rSet = GetScene()->GetCameraSet();

    // set ObjectTrans
    Matrix4D mTransform = GetFullTransform();
    rSet.SetObjectTrans(mTransform);

    // transform base polygon to screen coor
    for(UINT16 a=0;a<rExtrudePoly.Count();a++)
    {
        const Polygon3D &rExtPoly = rExtrudePoly[a];
        BOOL bClosed = rExtPoly.IsClosed();
        XPolygon aNewPoly(rExtPoly.GetPointCount() + (bClosed ? 1 : 0));

        UINT16 b;
        for(b=0;b<rExtPoly.GetPointCount();b++)
        {
            Vector3D aPoint = rSet.ObjectToViewCoor(rExtPoly[b]);
            aNewPoly[b].X() = (long)(aPoint.X() + 0.5);
            aNewPoly[b].Y() = (long)(aPoint.Y() + 0.5);
        }

        if(bClosed)
            aNewPoly[b] = aNewPoly[0];

        aNewPolyPolygon.Insert(aNewPoly);
    }

    return aNewPolyPolygon;
}

/*************************************************************************
|*
|* Attribute setzen
|*
\************************************************************************/

// private support routines for ItemSet access. NULL pointer means clear item.
void E3dCompoundObject::PostItemChange(const sal_uInt16 nWhich)
{
    // call parent
    E3dObject::PostItemChange(nWhich);

    // handle value change
    switch(nWhich)
    {
        case SDRATTR_3DOBJ_DOUBLE_SIDED:
        {
            bGeometryValid = FALSE;
            break;
        }
        case SDRATTR_3DOBJ_NORMALS_KIND:
        {
            bGeometryValid = FALSE;
            break;
        }
        case SDRATTR_3DOBJ_NORMALS_INVERT:
        {
            bGeometryValid = FALSE;
            break;
        }
        case SDRATTR_3DOBJ_TEXTURE_PROJ_X:
        {
            bGeometryValid = FALSE;
            break;
        }
        case SDRATTR_3DOBJ_TEXTURE_PROJ_Y:
        {
            bGeometryValid = FALSE;
            break;
        }
    }
}

// EOF
