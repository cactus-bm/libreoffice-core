/*************************************************************************
 *
 *  $RCSfile: scene3d.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: pjunck $ $Date: 2004-11-03 10:42:11 $
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

#ifndef _SVDITER_HXX
#include "svditer.hxx"
#endif

#ifndef _SVDIO_HXX
#include "svdio.hxx"
#endif

#if defined( UNX ) || defined( ICC )
#include <stdlib.h>
#endif

#ifndef _E3D_GLOBL3D_HXX
#include "globl3d.hxx"
#endif

#ifndef _SVDPAGE_HXX
#include "svdpage.hxx"
#endif

#ifndef _SFXSTYLE_HXX
#include <svtools/style.hxx>
#endif

#ifndef _E3D_SCENE3D_HXX
#include "scene3d.hxx"
#endif

#ifndef _E3D_UNDO_HXX
#include "e3dundo.hxx"
#endif

#ifndef _B3D_BASE3D_HXX
#include <goodies/base3d.hxx>
#endif

#ifndef _E3D_PLIGHT3D_HXX
#include "plight3d.hxx"
#endif

#ifndef _E3D_DLIGHT3D_HXX
#include "dlight3d.hxx"
#endif

#ifndef _SVDTRANS_HXX
#include "svdtrans.hxx"
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

#ifndef _SVX3DITEMS_HXX
#include "svx3ditems.hxx"
#endif

#ifndef _SFX_WHITER_HXX
#include <svtools/whiter.hxx>
#endif

#ifndef _SVX_XFLFTRIT_HXX
#include "xflftrit.hxx"
#endif

#ifndef _SDR_PROPERTIES_E3DSCENEPROPERTIES_HXX
#include <svx/sdr/properties/e3dsceneproperties.hxx>
#endif

// #110094#
#ifndef _SDR_CONTACT_VIEWCONTACTOFE3DSCENE_HXX
#include <svx/sdr/contact/viewcontactofe3dscene.hxx>
#endif

// for ::std::sort
#include <algorithm>

#define ITEMVALUE(ItemSet,Id,Cast)  ((const Cast&)(ItemSet).Get(Id)).GetValue()

//////////////////////////////////////////////////////////////////////////////
// #110988#

class ImpRemap3DDepth
{
    sal_uInt32                  mnOrdNum;
    double                      mfMinimalDepth;

    // bitfield
    unsigned                    mbIsScene : 1;

public:
    ImpRemap3DDepth(sal_uInt32 nOrdNum, double fMinimalDepth);
    ImpRemap3DDepth(sal_uInt32 nOrdNum);
    ~ImpRemap3DDepth();

    // for ::std::sort
    bool operator<(const ImpRemap3DDepth& rComp) const;

    sal_uInt32 GetOrdNum() const { return mnOrdNum; }
    sal_Bool IsScene() const { return mbIsScene; }
};

ImpRemap3DDepth::ImpRemap3DDepth(sal_uInt32 nOrdNum, double fMinimalDepth)
:   mnOrdNum(nOrdNum),
    mfMinimalDepth(fMinimalDepth),
    mbIsScene(sal_False)
{
}

ImpRemap3DDepth::ImpRemap3DDepth(sal_uInt32 nOrdNum)
:   mnOrdNum(nOrdNum),
    mbIsScene(sal_True)
{
}

ImpRemap3DDepth::~ImpRemap3DDepth()
{
}

bool ImpRemap3DDepth::operator<(const ImpRemap3DDepth& rComp) const
{
    if(IsScene())
    {
        return sal_False;
    }
    else
    {
        if(rComp.IsScene())
        {
            return sal_True;
        }
        else
        {
            return mfMinimalDepth < rComp.mfMinimalDepth;
        }
    }
}

// typedefs for a vector of ImpRemap3DDepths
typedef ::std::vector< ImpRemap3DDepth > ImpRemap3DDepthVector;

//////////////////////////////////////////////////////////////////////////////
// #110988#

class Imp3DDepthRemapper
{
    ImpRemap3DDepthVector       maVector;

public:
    Imp3DDepthRemapper(E3dScene& rScene);
    ~Imp3DDepthRemapper();

    sal_uInt32 RemapOrdNum(sal_uInt32 nOrdNum) const;
};

Imp3DDepthRemapper::Imp3DDepthRemapper(E3dScene& rScene)
{
    // only called when rScene.GetSubList() and nObjCount > 1L
    SdrObjList* pList = rScene.GetSubList();
    const sal_uInt32 nObjCount(pList->GetObjCount());

    for(sal_uInt32 a(0L); a < nObjCount; a++)
    {
        SdrObject* pCandidate = pList->GetObj(a);

        if(pCandidate)
        {
            if(pCandidate->ISA(E3dCompoundObject))
            {
                // single 3d object, calc depth
                const double fMinimalDepth(((E3dCompoundObject*)pCandidate)->GetMinimalDepthInViewCoor(rScene));
                ImpRemap3DDepth aEntry(a, fMinimalDepth);
                maVector.push_back(aEntry);
            }
            else
            {
                // scene, use standard entry for scene
                ImpRemap3DDepth aEntry(a);
                maVector.push_back(aEntry);
            }
        }
    }

    // now, we need to sort the maVector by it's members minimal depth. The
    // smaller, the nearer to the viewer.
    ::std::sort(maVector.begin(), maVector.end());
}

Imp3DDepthRemapper::~Imp3DDepthRemapper()
{
}

sal_uInt32 Imp3DDepthRemapper::RemapOrdNum(sal_uInt32 nOrdNum) const
{
    if(nOrdNum < maVector.size())
    {
        nOrdNum = maVector[(maVector.size() - 1) - nOrdNum].GetOrdNum();
    }

    return nOrdNum;
}

//////////////////////////////////////////////////////////////////////////////
// BaseProperties section

sdr::properties::BaseProperties* E3dScene::CreateObjectSpecificProperties()
{
    return new sdr::properties::E3dSceneProperties(*this);
}

//////////////////////////////////////////////////////////////////////////////
// #110094# DrawContact section

sdr::contact::ViewContact* E3dScene::CreateObjectSpecificViewContact()
{
    return new sdr::contact::ViewContactOfE3dScene(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEINIT1(E3dScene, E3dObject);

/*************************************************************************
|*
|* E3dScene-Konstruktor
|*
\************************************************************************/

E3dScene::E3dScene()
:   E3dObject(),
    aCamera(Vector3D(0,0,4), Vector3D()),
    bDoubleBuffered(FALSE),
    bClipping(FALSE),
    //BFS01nSaveStatus (0),
    //BFS01nRestStatus (0),
    bFitInSnapRect(TRUE),
    aPaintTime(),
    nDisplayQuality(255),
    // #110988#
    mp3DDepthRemapper(0L),
    bDrawOnlySelected(FALSE)
{
    // Defaults setzen
    E3dDefaultAttributes aDefault;
    SetDefaultAttributes(aDefault);
}

E3dScene::E3dScene(E3dDefaultAttributes& rDefault)
:   E3dObject(),
    aCamera(Vector3D(0,0,4), Vector3D()),
    bDoubleBuffered(FALSE),
    bClipping(FALSE),
    //BFS01nSaveStatus (0),
    //BFS01nRestStatus (0),
    bFitInSnapRect(TRUE),
    aPaintTime(),
    nDisplayQuality(255),
    // #110988#
    mp3DDepthRemapper(0L),
    bDrawOnlySelected(FALSE)
{
    // Defaults setzen
    SetDefaultAttributes(rDefault);
}

void E3dScene::SetDefaultAttributes(E3dDefaultAttributes& rDefault)
{
    // Fuer OS/2 die FP-Exceptions abschalten
#if defined(OS2)
#define SC_FPEXCEPTIONS_ON()    _control87( MCW_EM, 0 )
#define SC_FPEXCEPTIONS_OFF()   _control87( MCW_EM, MCW_EM )
    SC_FPEXCEPTIONS_OFF();
#endif

    // Fuer WIN95/NT die FP-Exceptions abschalten
#if defined(WNT) || defined(WIN)
#define SC_FPEXCEPTIONS_ON()    _control87( _MCW_EM, 0 )
#define SC_FPEXCEPTIONS_OFF()   _control87( _MCW_EM, _MCW_EM )
    SC_FPEXCEPTIONS_OFF();
#endif

    // Defaults setzen

    // set defaults for LightGroup from ItemPool
    aLightGroup.SetModelTwoSide(GetTwoSidedLighting());
    aLightGroup.SetIntensity( GetLightColor1(), Base3DMaterialDiffuse, Base3DLight0);
    aLightGroup.SetIntensity( GetLightColor2(), Base3DMaterialDiffuse, Base3DLight1);
    aLightGroup.SetIntensity( GetLightColor3(), Base3DMaterialDiffuse, Base3DLight2);
    aLightGroup.SetIntensity( GetLightColor4(), Base3DMaterialDiffuse, Base3DLight3);
    aLightGroup.SetIntensity( GetLightColor5(), Base3DMaterialDiffuse, Base3DLight4);
    aLightGroup.SetIntensity( GetLightColor6(), Base3DMaterialDiffuse, Base3DLight5);
    aLightGroup.SetIntensity( GetLightColor7(), Base3DMaterialDiffuse, Base3DLight6);
    aLightGroup.SetIntensity( GetLightColor8(), Base3DMaterialDiffuse, Base3DLight7);
    aLightGroup.SetGlobalAmbientLight(GetGlobalAmbientColor());
    aLightGroup.Enable( GetLightOnOff1(), Base3DLight0);
    aLightGroup.Enable( GetLightOnOff2(), Base3DLight1);
    aLightGroup.Enable( GetLightOnOff3(), Base3DLight2);
    aLightGroup.Enable( GetLightOnOff4(), Base3DLight3);
    aLightGroup.Enable( GetLightOnOff5(), Base3DLight4);
    aLightGroup.Enable( GetLightOnOff6(), Base3DLight5);
    aLightGroup.Enable( GetLightOnOff7(), Base3DLight6);
    aLightGroup.Enable( GetLightOnOff8(), Base3DLight7);
    aLightGroup.SetDirection( GetLightDirection1(), Base3DLight0);
    aLightGroup.SetDirection( GetLightDirection2(), Base3DLight1);
    aLightGroup.SetDirection( GetLightDirection3(), Base3DLight2);
    aLightGroup.SetDirection( GetLightDirection4(), Base3DLight3);
    aLightGroup.SetDirection( GetLightDirection5(), Base3DLight4);
    aLightGroup.SetDirection( GetLightDirection6(), Base3DLight5);
    aLightGroup.SetDirection( GetLightDirection7(), Base3DLight6);
    aLightGroup.SetDirection( GetLightDirection8(), Base3DLight7);

    bDither = rDefault.GetDefaultDither();

    // Alte Werte initialisieren
    aCamera.SetViewWindow(-2, -2, 4, 4);
    aCameraSet.SetDeviceRectangle(-2, 2, -2, 2);
    aCamera.SetDeviceWindow(Rectangle(0, 0, 10, 10));
    Rectangle aRect(0, 0, 10, 10);
    aCameraSet.SetViewportRectangle(aRect);
    //BFS01nSortingMode = E3D_SORT_FAST_SORTING | E3D_SORT_IN_PARENTS | E3D_SORT_TEST_LENGTH;

    // set defaults for Camera from ItemPool
    aCamera.SetProjection(GetPerspective());
    Vector3D aActualPosition = aCamera.GetPosition();
    double fNew = GetDistance();

    if(fabs(fNew - aActualPosition.Z()) > 1.0)
    {
        aCamera.SetPosition( Vector3D( aActualPosition.X(), aActualPosition.Y(), fNew) );
    }

    fNew = GetFocalLength() / 100.0;
    aCamera.SetFocalLength(fNew);
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

E3dScene::~E3dScene()
{
    // #110988#
    ImpCleanup3DDepthMapper();
}

// #110988#
void E3dScene::ImpCleanup3DDepthMapper()
{
    if(mp3DDepthRemapper)
    {
        delete mp3DDepthRemapper;
        mp3DDepthRemapper = 0L;
    }
}

// #110988#
sal_uInt32 E3dScene::RemapOrdNum(sal_uInt32 nOrdNum) const
{
    if(!mp3DDepthRemapper)
    {
        const sal_uInt32 nObjCount(GetSubList() ? GetSubList()->GetObjCount() : 0L);

        if(nObjCount > 1L)
        {
            ((E3dScene*)this)->mp3DDepthRemapper = new Imp3DDepthRemapper((E3dScene&)(*this));
        }
    }

    if(mp3DDepthRemapper)
    {
        return mp3DDepthRemapper->RemapOrdNum(nOrdNum);
    }

    return nOrdNum;
}

/*************************************************************************
|*
|* Feststellen, ob die Szene transparente Teile enthaelt
|*
\************************************************************************/

BOOL E3dScene::AreThereTransparentParts() const
{
    BOOL bRetval(FALSE);

    SdrObjListIter a3DIterator(*pSub, IM_DEEPWITHGROUPS);
    while ( !bRetval && a3DIterator.IsMore() )
    {
        SdrObject* pObj = a3DIterator.Next();

        // Nur darstellbare Objekte bewerten
        if(pObj->ISA(E3dCompoundObject))
        {
            // get const ItemSet reference
            const SfxItemSet& rSet = pObj->GetMergedItemSet();

            // Flaechenattribut testen
            UINT16 nFillTrans = ((const XFillTransparenceItem&)(rSet.Get(XATTR_FILLTRANSPARENCE))).GetValue();
            if(nFillTrans != 0)
                bRetval = TRUE;

            if(!bRetval)
            {
                // Linienattribut testen
                UINT16 nLineTransparence = ((const XLineTransparenceItem&)(rSet.Get(XATTR_LINETRANSPARENCE))).GetValue();
                if(nLineTransparence != 0)
                    bRetval = TRUE;

                if(!bRetval)
                {
                    // test FloatTransparence
                    const XFillFloatTransparenceItem& rFloatTrans = ((const XFillFloatTransparenceItem&)(rSet.Get(XATTR_FILLFLOATTRANSPARENCE)));
                    if(rFloatTrans.IsEnabled())
                        bRetval = TRUE;
                }
            }
        }
    }
    return bRetval;
}

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/

UINT16 E3dScene::GetObjIdentifier() const
{
    return E3D_SCENE_ID;
}

/*************************************************************************
|*
|* Anzahl der Handles zurueckgeben
|*
\************************************************************************/

USHORT E3dScene::GetHdlCount() const
{
    // Ueberladung aus E3dObject rueckgaengig machen
    return SdrAttrObj::GetHdlCount();
}

/*************************************************************************
|*
|* Handle-Liste fuellen
|*
\************************************************************************/

void E3dScene::AddToHdlList(SdrHdlList& rHdlList) const
{
    // Ueberladung aus E3dObject rueckgaengig machen
    SdrAttrObj::AddToHdlList(rHdlList);
}

/*************************************************************************
|*
\************************************************************************/

FASTBOOL E3dScene::HasSpecialDrag() const
{
    return FALSE;
}

/*************************************************************************
|*
|* SetSnapRect
|*
\************************************************************************/

void E3dScene::NbcSetSnapRect(const Rectangle& rRect)
{
    SetRectsDirty();
    E3dObject::NbcSetSnapRect(rRect);
    aCamera.SetDeviceWindow(rRect);
    aCameraSet.SetViewportRectangle((Rectangle&)rRect);

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* Objekt verschieben
|*
\************************************************************************/

void E3dScene::NbcMove(const Size& rSize)
{
    Rectangle aNewSnapRect = GetSnapRect();
    MoveRect(aNewSnapRect, rSize);
    NbcSetSnapRect(aNewSnapRect);
}

/*************************************************************************
|*
|* Objekt Resizen
|*
\************************************************************************/

void E3dScene::NbcResize(const Point& rRef, const Fraction& rXFact,
                                            const Fraction& rYFact)
{
    Rectangle aNewSnapRect = GetSnapRect();
    ResizeRect(aNewSnapRect, rRef, rXFact, rYFact);
    NbcSetSnapRect(aNewSnapRect);
}

/*************************************************************************
|*
|* Neue Kamera setzen, und dabei die Szene und ggf. das BoundVolume
|* als geaendert markieren
|*
\************************************************************************/

void E3dScene::SetCamera(const Camera3D& rNewCamera)
{
    // Alte Kamera setzen
    aCamera = rNewCamera;
    ((sdr::properties::E3dSceneProperties&)GetProperties()).SetSceneItemsFromCamera();

    SetRectsDirty();

    // Neue Kamera aus alter fuellen
    Camera3D& rCam = (Camera3D&)GetCamera();

    // Ratio abschalten
    if(rCam.GetAspectMapping() == AS_NO_MAPPING)
        GetCameraSet().SetRatio(0.0);

    // Abbildungsgeometrie setzen
    Vector3D aVRP = rCam.GetViewPoint();
    Vector3D aVPN = aVRP - rCam.GetVRP();
    Vector3D aVUV = rCam.GetVUV();

    // #91047# use SetViewportValues() to set VRP, VPN and VUV as vectors, too.
    // Else these values would not be exported/imported correctly.
    //GetCameraSet().SetOrientation(aVRP, aVPN, aVUV);
    GetCameraSet().SetViewportValues(aVRP, aVPN, aVUV);

    // Perspektive setzen
    GetCameraSet().SetPerspective(rCam.GetProjection() == PR_PERSPECTIVE);
    GetCameraSet().SetViewportRectangle((Rectangle&)rCam.GetDeviceWindow());

    // E3dLabel-Objekte muessen neu an die Projektion angepasst werden
    if ( aLabelList.Count() > 0 )
    {
        SetBoundVolInvalid();
        SetRectsDirty();
    }

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* 3D-Objekt einfuegen
|*
\************************************************************************/

void E3dScene::NewObjectInserted(const E3dObject* p3DObj)
{
    E3dObject::NewObjectInserted(p3DObj);

    if ( p3DObj == this )
        return;

    if ( p3DObj->ISA(E3dLabelObj) )
    {
        aLabelList.Insert((E3dLabelObj*) p3DObj, LIST_APPEND);
    }

    // falls Unterobjekte vorhanden sind, auch diese pruefen
    if ( p3DObj->IsGroupObject() )
    {
        SdrObjListIter a3DIterator(*p3DObj, IM_DEEPWITHGROUPS);

        while ( a3DIterator.IsMore() )
        {
            SdrObject* pObj = a3DIterator.Next();

            if ( pObj->ISA(E3dLabelObj) )
            {
                aLabelList.Insert((E3dLabelObj*) pObj, LIST_APPEND);
            }
        }
    }

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* Parent ueber Aenderung eines Childs informieren
|*
\************************************************************************/

void E3dScene::StructureChanged(const E3dObject* p3DObj)
{
    E3dObject::StructureChanged(p3DObj);
    SetRectsDirty();

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* Double Buffering aus-/einschalten
|*
\************************************************************************/

void E3dScene::SetDoubleBuffered(FASTBOOL bBuff)
{
    if ( bDoubleBuffered != (BOOL)bBuff )
    {
        bDoubleBuffered = bBuff;
        SetRectsDirty();
    }
}

/*************************************************************************
|*
|* Clipping auf umschliessendes Rechteck der Szene aus-/einschalten
|*
\************************************************************************/

void E3dScene::SetClipping(FASTBOOL bClip)
{
    if ( bClipping != (BOOL)bClip )
    {
        bClipping = bClip;
        SetRectsDirty();
    }
}

/*************************************************************************
|*
|* Einpassen der Objekte in umschliessendes Rechteck aus-/einschalten
|*
\************************************************************************/

void E3dScene::SetFitInSnapRect(FASTBOOL bFit)
{
    if ( bFitInSnapRect != (BOOL)bFit )
    {
        bFitInSnapRect = bFit;
        SetRectsDirty();
    }
}

/*************************************************************************
|*
|* Einpassen der Projektion aller Szenenobjekte in das
|* umschliessende Rechteck
|*
\************************************************************************/

Volume3D E3dScene::FitInSnapRect()
{
    // Alter Kram
    Matrix4D aFullTrans = GetFullTransform();
    aCamera.FitViewToVolume(GetBoundVolume(), aFullTrans);

    // Neuer Kram
    // Maximas holen in Augkoordinaten zwecks Z-Werten
    Volume3D aNewVol;
    Vector3D aTfVec;
    Vol3DPointIterator aIter(GetBoundVolume());

    GetCameraSet().SetObjectTrans(aFullTrans);
    while ( aIter.Next(aTfVec) )
    {
        aTfVec = GetCameraSet().ObjectToEyeCoor(aTfVec);
        aNewVol.Union(aTfVec);
    }

    // ... und merken
    double fZMin = -aNewVol.MaxVec().Z();
    double fZMax = -aNewVol.MinVec().Z();

    // Jetzt XY-Werte projizieren auf Projektionsflaeche
    // in Device-Koordinaten
    Matrix4D aWorldToDevice = GetCameraSet().GetOrientation();
    if(aCamera.GetProjection() == PR_PERSPECTIVE)
        aWorldToDevice.Frustum(-1.0, 1.0, -1.0, 1.0, fZMin, fZMax);
    else
        aWorldToDevice.Ortho(-1.0, 1.0, -1.0, 1.0, fZMin, fZMax);
    aNewVol.Reset();
    aIter.Reset();
    while ( aIter.Next(aTfVec) )
    {
        aTfVec = GetCameraSet().ObjectToWorldCoor(aTfVec);
        aTfVec *= aWorldToDevice;
        aNewVol.Union(aTfVec);
    }

    // Labels behandeln
    ULONG nLabelCnt = aLabelList.Count();
    if ( nLabelCnt > 0 )
    {
        // Vorlaeufige Projektion bestimmen und Transformation in
        // ViewKoordinaten bestimmen
        Matrix4D aMatWorldToView = GetCameraSet().GetOrientation();
        if(aCamera.GetProjection() == PR_PERSPECTIVE)
            aMatWorldToView.Frustum(aNewVol.MinVec().X(), aNewVol.MaxVec().X(),
            aNewVol.MinVec().Y(), aNewVol.MaxVec().Y(), fZMin, fZMax);
        else
            aMatWorldToView.Ortho(aNewVol.MinVec().X(), aNewVol.MaxVec().X(),
            aNewVol.MinVec().Y(), aNewVol.MaxVec().Y(), fZMin, fZMax);

        // Logische Abmessungen der Szene holen
        Rectangle aSceneRect = GetSnapRect();

        // Matrix DeviceToView aufbauen
        Vector3D aTranslate, aScale;

        aTranslate[0] = (double)aSceneRect.Left() + (aSceneRect.GetWidth() / 2.0);
        aTranslate[1] = (double)aSceneRect.Top() + (aSceneRect.GetHeight() / 2.0);
        aTranslate[2] = ZBUFFER_DEPTH_RANGE / 2.0;

        // Skalierung
        aScale[0] = (aSceneRect.GetWidth() - 1) / 2.0;
        aScale[1] = (aSceneRect.GetHeight() - 1) / -2.0;
        aScale[2] = ZBUFFER_DEPTH_RANGE / 2.0;

        aMatWorldToView.Scale(aScale);
        aMatWorldToView.Translate(aTranslate);

        // Inverse Matrix ViewToDevice aufbauen
        Matrix4D aMatViewToWorld(aMatWorldToView);
        aMatViewToWorld.Invert();

        for (ULONG i = 0; i < nLabelCnt; i++)
        {
            E3dLabelObj* p3DObj = aLabelList.GetObject(i);
            const SdrObject* pObj = p3DObj->Get2DLabelObj();

            // View- Abmessungen des Labels holen
            const Rectangle& rObjRect = pObj->GetLogicRect();

            // Position des Objektes in Weltkoordinaten ermitteln
            Matrix4D aObjTrans = p3DObj->GetFullTransform();
            Vector3D aObjPos = aObjTrans * p3DObj->GetPosition();

            // View-Position des Objektes feststellen
            // nach ViewKoordinaten
            aObjPos *= aMatWorldToView;

            // Relative Position des Labels in View-Koordinaten
            Vector3D aRelPosOne(pObj->GetRelativePos(), aObjPos.Z());
            aRelPosOne.X() += aObjPos.X();
            aRelPosOne.Y() += aObjPos.Y();
            Vector3D aRelPosTwo(aRelPosOne);
            aRelPosTwo.X() += rObjRect.GetWidth();
            aRelPosTwo.Y() += rObjRect.GetHeight();

            // Jetzt Eckpunkte in DeviceKoordinaten bestimmen und
            // den Abmessungen hinzufuegen
            aRelPosOne *= aMatViewToWorld;
            aRelPosOne *= aWorldToDevice;
            aNewVol.Union(aRelPosOne);

            aRelPosTwo *= aMatViewToWorld;
            aRelPosTwo *= aWorldToDevice;
            aNewVol.Union(aRelPosTwo);
        }
    }

    // Z-Werte eintragen
    aNewVol.MinVec().Z() = fZMin;
    aNewVol.MaxVec().Z() = fZMax;

    // #110988#
    ImpCleanup3DDepthMapper();

    // Rueckgabewert setzen
    return aNewVol;
}

/*************************************************************************
|*
|* Uebergeordnetes Szenenobjekt bestimmen
|*
\************************************************************************/

E3dScene* E3dScene::GetScene() const
{
    if(GetParentObj())
        return GetParentObj()->GetScene();
    else
        return (E3dScene*)this;
}

/*************************************************************************
|*
|* TransformationSet vorbereiten
|*
\************************************************************************/

void E3dScene::InitTransformationSet()
{
    Rectangle aBound(GetSnapRect());

    // GeometricSet reset und mit pBase3D assoziieren
    B3dCamera& rSet = GetCameraSet();

    // Transformation auf Weltkoordinaten holen
    Matrix4D mTransform = GetFullTransform();
    rSet.SetObjectTrans(mTransform);

    // 3D Ausgabe vorbereiten, Maximas holen in DeviceKoordinaten
    Volume3D aVolume = FitInSnapRect();

    // Maximas fuer Abbildung verwenden
    rSet.SetDeviceVolume(aVolume, FALSE);
    rSet.SetViewportRectangle(aBound);

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* sichern mit neuer Methode und zukunftskompatibilitaet
|* Die Zahl 3560 ist die Major-Update-Nummer * 10 zu der die Umstellung
|* erfolgte. Dies ist leider das korrekte Verhalten, die 3d-Engine hat keine
|* eigene Versionsnummer sondern ist an die der Drawing-Engine gekoppelt.
|* Probleme gibt es immer dann wenn einen neue Version ein altes Format
|* schreiben soll: Hier wird von der Drawing-Engine trotzdem die neue Nummer
|* verwendet.
|*
\************************************************************************/

//BFS01void E3dScene::WriteData(SvStream& rOut) const
//BFS01{
//BFS01#ifndef SVX_LIGHT
//BFS01 long nVersion = rOut.GetVersion(); // Build_Nr * 10 z.B. 3810
//BFS01 if(nVersion < 3830)
//BFS01 {
//BFS01     // Hier die Lichtobjekte erzeugen, um im alten Format schreiben zu koennen
//BFS01     ((E3dScene*)(this))->CreateLightObjectsFromLightGroup();
//BFS01 }
//BFS01
//BFS01 // Schreiben
//BFS01 E3dObject::WriteData(rOut);
//BFS01
//BFS01 if(nVersion < 3830)
//BFS01 {
//BFS01     // Lichtobjekte wieder wegnehmen
//BFS01     ((E3dScene*)(this))->RemoveLightObjects();
//BFS01 }
//BFS01 else
//BFS01 {
//BFS01#ifdef E3D_STREAMING
//BFS01     SdrDownCompat aCompat(rOut, STREAM_WRITE);
//BFS01#ifdef DBG_UTIL
//BFS01     aCompat.SetID("B3dLightGroup");
//BFS01#endif
//BFS01     // LightGroup schreiben
//BFS01     aLightGroup.WriteData(rOut);
//BFS01
//BFS01#endif
//BFS01 }
//BFS01
//BFS01#ifdef E3D_STREAMING
//BFS01 SdrDownCompat aCompat(rOut, STREAM_WRITE);
//BFS01#ifdef DBG_UTIL
//BFS01 aCompat.SetID("E3dScene");
//BFS01#endif
//BFS01
//BFS01 DBG_ASSERT (rOut.GetVersion(),"3d-Engine: Keine Version am Stream gesetzt!");
//BFS01 if (rOut.GetVersion() < 3560) // FG: Das ist der Zeitpunkt der Umstellung
//BFS01 {
//BFS01     rOut << aCamera;
//BFS01 }
//BFS01 if (rOut.GetVersion() >= 3560)
//BFS01 {
//BFS01     aCamera.WriteData(rOut);
//BFS01 }
//BFS01
//BFS01 rOut << BOOL(bDoubleBuffered);
//BFS01 rOut << BOOL(bClipping);
//BFS01 rOut << BOOL(bFitInSnapRect);
//BFS01 rOut << nSortingMode;
//BFS01
//BFS01 // neu ab 377:
//BFS01 Vector3D aPlaneDirection = GetShadowPlaneDirection();
//BFS01 rOut << aPlaneDirection;
//BFS01
//BFS01 // neu ab 383:
//BFS01 rOut << (BOOL)bDither;
//BFS01
//BFS01 // neu ab 384:
//BFS01 sal_uInt16 nShadeMode = GetShadeMode();
//BFS01 if(nShadeMode == 0)
//BFS01     rOut << (sal_uInt16)Base3DFlat;
//BFS01 else if(nShadeMode == 1)
//BFS01     rOut << (sal_uInt16)Base3DPhong;
//BFS01 else
//BFS01     rOut << (sal_uInt16)Base3DSmooth;
//BFS01 rOut << (BOOL)(nShadeMode > 2);
//BFS01
//BFS01#endif
//BFS01#endif   // #ifndef SVX_LIGHT
//BFS01}

/*************************************************************************
|*
|* laden
|*
\************************************************************************/

//BFS01void E3dScene::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
//BFS01{
//BFS01 if (ImpCheckSubRecords (rHead, rIn))
//BFS01 {
//BFS01     E3dObject::ReadData(rHead, rIn);
//BFS01
//BFS01     if(CountNumberOfLights())
//BFS01     {
//BFS01         // An dieser Stelle die gelesenen Lampen ausmerzen
//BFS01         // und in die neue Struktur ueberfuehren
//BFS01         FillLightGroup();
//BFS01         RemoveLightObjects();
//BFS01     }
//BFS01     long nVersion = rIn.GetVersion(); // Build_Nr * 10 z.B. 3810
//BFS01     if(nVersion >= 3830)
//BFS01     {
//BFS01         SdrDownCompat aCompat(rIn, STREAM_READ);
//BFS01#ifdef DBG_UTIL
//BFS01         aCompat.SetID("B3dLightGroup");
//BFS01#endif
//BFS01         if(aCompat.GetBytesLeft())
//BFS01         {
//BFS01             // LightGroup lesen
//BFS01             aLightGroup.ReadData(rIn);
//BFS01         }
//BFS01     }
//BFS01
//BFS01     SdrDownCompat aCompat(rIn, STREAM_READ);
//BFS01#ifdef DBG_UTIL
//BFS01     aCompat.SetID("E3dScene");
//BFS01#endif
//BFS01     BOOL bTmp;
//BFS01
//BFS01     DBG_ASSERT (rIn.GetVersion(),"3d-Engine: Keine Version am Stream gesetzt!");
//BFS01
//BFS01     if ((rIn.GetVersion() < 3560) || (rHead.GetVersion() <= 12))
//BFS01     {
//BFS01         rIn >> aCamera;
//BFS01     }
//BFS01     if ((rIn.GetVersion() >= 3560) && (rHead.GetVersion() >= 13))
//BFS01     {
//BFS01         aCamera.ReadData(rHead, rIn);
//BFS01     }
//BFS01
//BFS01     // Neue Kamera aus alter fuellen
//BFS01     Camera3D& rCam = (Camera3D&)GetCamera();
//BFS01
//BFS01     // Ratio abschalten
//BFS01     if(rCam.GetAspectMapping() == AS_NO_MAPPING)
//BFS01         GetCameraSet().SetRatio(0.0);
//BFS01
//BFS01     // Abbildungsgeometrie setzen
//BFS01     Vector3D aVRP = rCam.GetViewPoint();
//BFS01     Vector3D aVPN = aVRP - rCam.GetVRP();
//BFS01     Vector3D aVUV = rCam.GetVUV();
//BFS01     GetCameraSet().SetOrientation(aVRP, aVPN, aVUV);
//BFS01
//BFS01     // Perspektive setzen
//BFS01     GetCameraSet().SetPerspective(rCam.GetProjection() == PR_PERSPECTIVE);
//BFS01     GetCameraSet().SetViewportRectangle((Rectangle&)rCam.GetDeviceWindow());
//BFS01
//BFS01     rIn >> bTmp; bDoubleBuffered = bTmp;
//BFS01     rIn >> bTmp; bClipping = bTmp;
//BFS01     rIn >> bTmp; bFitInSnapRect = bTmp;
//BFS01
//BFS01     if (aCompat.GetBytesLeft() >= sizeof(UINT32))
//BFS01     {
//BFS01         rIn >> nSortingMode;
//BFS01     }
//BFS01
//BFS01     // neu ab 377:
//BFS01     if (aCompat.GetBytesLeft() >= sizeof(Vector3D))
//BFS01     {
//BFS01         Vector3D aShadowVec;
//BFS01         rIn >> aShadowVec;
//BFS01         SetShadowPlaneDirection(aShadowVec);
//BFS01     }
//BFS01
//BFS01     // neu ab 383:
//BFS01     if (aCompat.GetBytesLeft() >= sizeof(BOOL))
//BFS01     {
//BFS01         rIn >> bTmp; bDither = bTmp;
//BFS01     }
//BFS01
//BFS01     // neu ab 384:
//BFS01     if (aCompat.GetBytesLeft() >= sizeof(UINT16))
//BFS01     {
//BFS01         UINT16 nTmp;
//BFS01         rIn >> nTmp;
//BFS01         if(nTmp == (Base3DShadeModel)Base3DFlat)
//BFS01         {
//BFS01             GetProperties().SetObjectItemDirect(Svx3DShadeModeItem(0));
//BFS01         }
//BFS01         else if(nTmp == (Base3DShadeModel)Base3DPhong)
//BFS01         {
//BFS01             GetProperties().SetObjectItemDirect(Svx3DShadeModeItem(1));
//BFS01         }
//BFS01         else
//BFS01         {
//BFS01             GetProperties().SetObjectItemDirect(Svx3DShadeModeItem(2));
//BFS01         }
//BFS01     }
//BFS01     if (aCompat.GetBytesLeft() >= sizeof(BOOL))
//BFS01     {
//BFS01         rIn >> bTmp;
//BFS01         if(bTmp)
//BFS01         {
//BFS01             GetProperties().SetObjectItemDirect(Svx3DShadeModeItem(3));
//BFS01         }
//BFS01     }
//BFS01
//BFS01     // SnapRects der Objekte ungueltig
//BFS01     SetRectsDirty();
//BFS01
//BFS01     // Transformationen initialisieren, damit bei RecalcSnapRect()
//BFS01     // richtig gerechnet wird
//BFS01     InitTransformationSet();
//BFS01
//BFS01     RebuildLists();
//BFS01
//BFS01     // set items from combined read objects like lightgroup and camera
//BFS01     ((sdr::properties::E3dSceneProperties&)GetProperties()).SetLightItemsFromLightGroup(aLightGroup);
//BFS01     ((sdr::properties::E3dSceneProperties&)GetProperties()).SetSceneItemsFromCamera();
//BFS01 }
//BFS01}

/*************************************************************************
|*
|* Einpassen der Objekte in umschliessendes Rechteck aus-/einschalten
|*
\************************************************************************/

void E3dScene::FitSnapRectToBoundVol()
{
    Vector3D aTfVec;
    Volume3D aFitVol;

    SetBoundVolInvalid();
    Matrix4D aTransform = aCamera.GetViewTransform() * GetFullTransform(); // #112587#
    Vol3DPointIterator aIter(GetBoundVolume(), &aTransform);
    Rectangle aRect;

    while ( aIter.Next(aTfVec) )
    {
        aCamera.DoProjection(aTfVec);
        aFitVol.Union(aTfVec);
        Vector3D aZwi = aCamera.MapToDevice(aTfVec);
        Point aP((long)aZwi.X(), (long)aZwi.Y());
        aRect.Union(Rectangle(aP, aP));
    }
    aCamera.SetViewWindow(aFitVol.MinVec().X(), aFitVol.MinVec().Y(),
        aFitVol.GetWidth(), aFitVol.GetHeight());
    SetSnapRect(aRect);

    // Die SnapRects aller beteiligten Objekte muessen auf dieser
    // veraenderten Basis aufgebaut werden, invalidiere diese. Das
    // eigene kann auch invalidiert werden, da ein RecalcSnapRect
    // an einer Szene nur aus der Kamera liest
    SetRectsDirty();

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* Falls die Geometrie einer Szene sich ausgedehnt/vermindert hat,
|* muss das Volume und das SnapRect angepasst werden
|*
\************************************************************************/

void E3dScene::CorrectSceneDimensions()
{
    // SnapRects der Objekte ungueltig
    SetRectsDirty();

    // SnapRect anpassen, invalidiert auch die SnapRects
    // der enthaltenen Objekte
    FitSnapRectToBoundVol();

    // Neues BoundVolume der Kamera holen
    Volume3D aVolume = FitInSnapRect();

    // Neues BoundVolume an der Kamera setzen
    GetCameraSet().SetDeviceVolume(aVolume, FALSE);

    // Danach noch die SnapRects der enthaltenen Objekte
    // invalidieren, um diese auf der neuen Grundlage berechnen
    // zu lassen (falls diese von FitInSnapRect() berechnet wurden)
    SetRectsDirty();

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* Zuweisungsoperator
|*
\************************************************************************/

void E3dScene::operator=(const SdrObject& rObj)
{
    E3dObject::operator=(rObj);

    const E3dScene& r3DObj = (const E3dScene&) rObj;
    aCamera          = r3DObj.aCamera;
    bDoubleBuffered  = r3DObj.bDoubleBuffered;
    bClipping        = r3DObj.bClipping;
    bFitInSnapRect   = r3DObj.bFitInSnapRect;
    //BFS01nSortingMode     = r3DObj.nSortingMode;

    // neu ab 377:
    aCameraSet = r3DObj.aCameraSet;
    ((sdr::properties::E3dSceneProperties&)GetProperties()).SetSceneItemsFromCamera();

    // neu ab 383:
    aLightGroup = r3DObj.aLightGroup;
    ((sdr::properties::E3dSceneProperties&)GetProperties()).SetLightItemsFromLightGroup(aLightGroup);

    bDither = r3DObj.bDither;

    bBoundVolValid = FALSE;
    RebuildLists();

    SetRectsDirty();

    // #110988#
    ImpCleanup3DDepthMapper();
}

/*************************************************************************
|*
|* Licht- und Labelobjektlisten neu aufbauen (nach Laden, Zuweisung)
|*
\************************************************************************/

void E3dScene::RebuildLists()
{
    // zuerst loeschen
    aLabelList.Clear();
    SdrLayerID nLayerID = GetLayer();

    SdrObjListIter a3DIterator(*pSub, IM_FLAT);

    // dann alle Objekte in der Szene pruefen
    while ( a3DIterator.IsMore() )
    {
        E3dObject* p3DObj = (E3dObject*) a3DIterator.Next();
        p3DObj->NbcSetLayer(nLayerID);
        NewObjectInserted(p3DObj);
    }
}

/*************************************************************************
|*
|* erstelle neues GeoData-Objekt
|*
\************************************************************************/

SdrObjGeoData *E3dScene::NewGeoData() const
{
    return new E3DSceneGeoData;
}

/*************************************************************************
|*
|* uebergebe aktuelle werte an das GeoData-Objekt
|*
\************************************************************************/

void E3dScene::SaveGeoData(SdrObjGeoData& rGeo) const
{
    E3dObject::SaveGeoData (rGeo);

    ((E3DSceneGeoData &) rGeo).aCamera                = aCamera;
    ((E3DSceneGeoData &) rGeo).aLabelList             = aLabelList;
}

/*************************************************************************
|*
|* uebernehme werte aus dem GeoData-Objekt
|*
\************************************************************************/

void E3dScene::RestGeoData(const SdrObjGeoData& rGeo)
{
    E3dObject::RestGeoData (rGeo);

    aLabelList = ((E3DSceneGeoData &) rGeo).aLabelList;
    SetCamera (((E3DSceneGeoData &) rGeo).aCamera);
    FitSnapRectToBoundVol();
}

/*************************************************************************
|*
|* Am StyleSheet wurde etwas geaendert, also Scene aendern
|*
\************************************************************************/

void E3dScene::SFX_NOTIFY(SfxBroadcaster &rBC,
                          const TypeId   &rBCType,
                          const SfxHint  &rHint,
                          const TypeId   &rHintType)
{
    SetRectsDirty();
    E3dObject::SFX_NOTIFY(rBC, rBCType, rHint, rHintType);
}

/*************************************************************************
|*
\************************************************************************/

void E3dScene::RotateScene (const Point& rRef, long nWink, double sn, double cs)
{
    Point UpperLeft, LowerRight, Center, NewCenter;

    UpperLeft = aOutRect.TopLeft();
    LowerRight = aOutRect.BottomRight();

    long dxOutRectHalf = labs(UpperLeft.X() - LowerRight.X());
    dxOutRectHalf /= 2;
    long dyOutRectHalf = labs(UpperLeft.Y() - LowerRight.Y());
    dyOutRectHalf /= 2;

    Rectangle RectQuelle(aOutRect), RectZiel(aOutRect);

       // Nur der Mittelpunkt wird bewegt. Die Ecken werden von NbcMove bewegt.
       // Fuer das Drehen wird von mir ein kartesisches Koordinatensystem verwendet in dem der Drehpunkt
       // der Nullpunkt ist und die Y- Achse nach oben ansteigt, die X-Achse nach rechts.
       // Dies muss bei den Y-Werten beachtet werden. (Auf dem Blatt zeigt die Y-Achse nach unten
    Center.X() = (UpperLeft.X() + dxOutRectHalf) - rRef.X();
    Center.Y() = -((UpperLeft.Y() + dyOutRectHalf) - rRef.Y());
                  // Ein paar Spezialfaelle zuerst abhandeln (n*90 Grad n ganzzahlig)
    if (sn==1.0 && cs==0.0) { // 90�
        NewCenter.X() = -Center.Y();
        NewCenter.Y() = -Center.X();
    } else if (sn==0.0 && cs==-1.0) { // 180�
        NewCenter.X() = -Center.X();
        NewCenter.Y() = -Center.Y();
    } else if (sn==-1.0 && cs==0.0) { // 270�
        NewCenter.X() =  Center.Y();
        NewCenter.Y() = -Center.X();
    }
    else          // Hier wird um einen beliebigen Winkel in mathematisch positiver Richtung gedreht!
    {             // xneu = x * cos(alpha) - y * sin(alpha)
                  // yneu = x * sin(alpha) + y * cos(alpha)
                  // Unten Rechts wird nicht gedreht: die Seiten von RectQuelle muessen parallel
                  // zu den Koordinatenachsen bleiben.
        NewCenter.X() = (long) (Center.X() * cs - Center.Y() * sn);
        NewCenter.Y() = (long) (Center.X() * sn + Center.Y() * cs);
    }

    Size Differenz;
    Point DiffPoint = (NewCenter - Center);
    Differenz.Width() = DiffPoint.X();
    Differenz.Height() = -DiffPoint.Y();  // Man beachte dass die Y-Achse nach unten positiv gezaehlt wird.
    NbcMove (Differenz);  // fuehrt die eigentliche Koordinatentransformation durch.
}

/*************************************************************************
|*
|* Get the name of the object (singular)
|*
\************************************************************************/

void E3dScene::TakeObjNameSingul(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNameSingulScene3d);

    String aName( GetName() );
    if(aName.Len())
    {
        rName += sal_Unicode(' ');
        rName += sal_Unicode('\'');
        rName += aName;
        rName += sal_Unicode('\'');
    }
}

/*************************************************************************
|*
|* Get the name of the object (plural)
|*
\************************************************************************/

void E3dScene::TakeObjNamePlural(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNamePluralScene3d);
}

/*************************************************************************
|*
|* Die NbcRotate-Routine �berl�dt die des SdrObject. Die Idee ist die Scene
|* drehen zu koennen und relativ zur Lage der Scene dann auch die Objekte
|* in der Scene
|*
\************************************************************************/

void E3dScene::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
{
        // Also derzeit sind die Klebepunkte relativ zum aOutRect der Szene definiert. Vor dem Drehen
        // werden die Klebepunkte relativ zur Seite definiert. Sie nehmen an der Drehung der Szene noch nicht Teil
        // dafuer gibt es den
    SetGlueReallyAbsolute(TRUE);

        // So dass war die Szene, ab jetzt kommen die Objekte in der Szene
        // 3D-Objekte gibt es nur ein einziges das kann zwar mehrere Fl�chen haben aber die Flaechen
        // muessen ja nicht zusammenhaengend sein
        // es ermoeglicht den Zugriff auf Kindobjekte
        // Ich gehe also die gesamte Liste durch und rotiere um die Z-Achse die durch den
        // Mittelpunkt von aOutRect geht (Satz von Steiner), also RotateZ

    RotateScene (rRef, nWink, sn, cs);  // Rotiert die Szene
    double fWinkelInRad = nWink/100 * F_PI180;
    NbcRotateZ(fWinkelInRad);
    FitSnapRectToBoundVol();
    SetRectsDirty();    // Veranlasst eine Neuberechnung aller BoundRects
    NbcRotateGluePoints(rRef,nWink,sn,cs);  // Rotiert die Klebepunkte (die haben noch Koordinaten relativ
                                            // zum Urpsung des Blattes
    SetGlueReallyAbsolute(FALSE);  // ab jetzt sind sie wieder relativ zum BoundRect (also dem aOutRect definiert)
    SetRectsDirty();
}

/*************************************************************************
|*
|* Licht-Objekte rauswerfen
|*
\************************************************************************/

//BFS01void E3dScene::RemoveLightObjects()
//BFS01{
//BFS01 SdrObjList* pSubList = GetSubList();
//BFS01 if(pSubList)
//BFS01 {
//BFS01     SdrObjListIter a3DIterator(*pSubList, IM_DEEPWITHGROUPS);
//BFS01     while ( a3DIterator.IsMore() )
//BFS01     {
//BFS01         E3dObject* pObj = (E3dObject*) a3DIterator.Next();
//BFS01         DBG_ASSERT(pObj->ISA(E3dObject), "AW: In Szenen sind nur 3D-Objekte erlaubt!");
//BFS01         if(pObj->ISA(E3dLight))
//BFS01         {
//BFS01             // Weg damit
//BFS01             Remove3DObj(pObj);
//BFS01         }
//BFS01     }
//BFS01 }
//BFS01}

/*************************************************************************
|*
|* Licht-Objekte erzeugen, um kompatibel zur 4.0
|* speichern zu koennen
|*
\************************************************************************/

//BFS01void E3dScene::CreateLightObjectsFromLightGroup()
//BFS01{
//BFS01 if(aLightGroup.IsLightingEnabled())
//BFS01 {
//BFS01     // Global Ambient Light
//BFS01     const Color& rAmbient = aLightGroup.GetGlobalAmbientLight();
//BFS01     if(rAmbient != Color(COL_BLACK))
//BFS01         Insert3DObj(new E3dLight(Vector3D(), rAmbient, 1.0));
//BFS01
//BFS01     // Andere Lichter
//BFS01     for(UINT16 a=0;a<BASE3D_MAX_NUMBER_LIGHTS;a++)
//BFS01     {
//BFS01         B3dLight& rLight = aLightGroup.GetLightObject((Base3DLightNumber)(Base3DLight0 + a));
//BFS01         if(rLight.IsEnabled())
//BFS01         {
//BFS01             if(rLight.IsDirectionalSource())
//BFS01             {
//BFS01                 // erzeuge E3dDistantLight
//BFS01                 Insert3DObj(new E3dDistantLight(Vector3D(),
//BFS01                     rLight.GetPosition(),
//BFS01                     rLight.GetIntensity(Base3DMaterialDiffuse), 1.0));
//BFS01             }
//BFS01             else
//BFS01             {
//BFS01                 // erzeuge E3dPointLight
//BFS01                 Insert3DObj(new E3dPointLight(rLight.GetPosition(),
//BFS01                     rLight.GetIntensity(Base3DMaterialDiffuse), 1.0));
//BFS01             }
//BFS01         }
//BFS01     }
//BFS01 }
//BFS01}

/*************************************************************************
|*
|* Beleuchtung aus dem alten Beleuchtungsmodell uebernehmen
|*
\************************************************************************/

//BFS01void E3dScene::FillLightGroup()
//BFS01{
//BFS01 SdrObjList* pSubList = GetSubList();
//BFS01 BOOL bLampFound = FALSE;
//BFS01
//BFS01 if(pSubList)
//BFS01 {
//BFS01     SdrObjListIter a3DIterator(*pSubList, IM_DEEPWITHGROUPS);
//BFS01     Base3DLightNumber eLight = Base3DLight0;
//BFS01
//BFS01     // AmbientLight aus
//BFS01     aLightGroup.SetGlobalAmbientLight(Color(COL_BLACK));
//BFS01
//BFS01     while ( a3DIterator.IsMore() )
//BFS01     {
//BFS01         E3dObject* pObj = (E3dObject*) a3DIterator.Next();
//BFS01         DBG_ASSERT(pObj->ISA(E3dObject), "AW: In Szenen sind nur 3D-Objekte erlaubt!");
//BFS01         if(pObj->ISA(E3dLight) && eLight <= Base3DLight7)
//BFS01         {
//BFS01             E3dLight* pLight = (E3dLight*)pObj;
//BFS01             bLampFound = TRUE;
//BFS01
//BFS01             // pLight in Base3D Konvention aktivieren
//BFS01             if(pLight->IsOn())
//BFS01             {
//BFS01                 if(pLight->ISA(E3dPointLight))
//BFS01                 {
//BFS01                     // ist ein E3dPointLight
//BFS01                     // Position, keine Richtung
//BFS01                     B3dColor aCol(pLight->GetColor().GetColor());
//BFS01                     aCol *= pLight->GetIntensity();
//BFS01                     aLightGroup.SetIntensity(aCol, Base3DMaterialDiffuse, eLight);
//BFS01                     aLightGroup.SetIntensity(Color(COL_WHITE), Base3DMaterialSpecular, eLight);
//BFS01                     Vector3D aPos = pLight->GetPosition();
//BFS01                     aLightGroup.SetPosition(aPos, eLight);
//BFS01
//BFS01                     // Lichtquelle einschalten
//BFS01                     aLightGroup.Enable(TRUE, eLight);
//BFS01
//BFS01                     // Naechstes Licht in Base3D
//BFS01                     eLight = (Base3DLightNumber)(eLight + 1);
//BFS01                 }
//BFS01                 else if(pLight->ISA(E3dDistantLight))
//BFS01                 {
//BFS01                     // ist ein E3dDistantLight
//BFS01                     // Richtung, keine Position
//BFS01                     B3dColor aCol(pLight->GetColor().GetColor());
//BFS01                     aCol *= pLight->GetIntensity();
//BFS01                     aLightGroup.SetIntensity(aCol, Base3DMaterialDiffuse, eLight);
//BFS01                     aLightGroup.SetIntensity(Color(COL_WHITE), Base3DMaterialSpecular, eLight);
//BFS01                     Vector3D aDir = ((E3dDistantLight *)pLight)->GetDirection();
//BFS01                     aLightGroup.SetDirection(aDir, eLight);
//BFS01
//BFS01                     // Lichtquelle einschalten
//BFS01                     aLightGroup.Enable(TRUE, eLight);
//BFS01
//BFS01                     // Naechstes Licht in Base3D
//BFS01                     eLight = (Base3DLightNumber)(eLight + 1);
//BFS01                 }
//BFS01                 else
//BFS01                 {
//BFS01                     // nur ein E3dLight, gibt ein
//BFS01                     // ambientes licht, auf globales aufaddieren
//BFS01                     B3dColor aCol(pLight->GetColor().GetColor());
//BFS01                     aCol *= pLight->GetIntensity();
//BFS01                     aCol += (const B3dColor &)aLightGroup.GetGlobalAmbientLight();
//BFS01                     aLightGroup.SetGlobalAmbientLight(aCol);
//BFS01                 }
//BFS01             }
//BFS01         }
//BFS01     }
//BFS01
//BFS01     // Alle anderen Lichter ausschalten
//BFS01     while(eLight <= Base3DLight7)
//BFS01     {
//BFS01         aLightGroup.Enable(FALSE, eLight);
//BFS01         eLight = (Base3DLightNumber)(eLight + 1);
//BFS01     }
//BFS01 }
//BFS01
//BFS01 // Beleuchtung einschalten, falls Lampen vorhanden
//BFS01 aLightGroup.EnableLighting(bLampFound);
//BFS01}

/*************************************************************************
|*
|* Lichter zaehlen
|*
\************************************************************************/

//BFS01UINT16 E3dScene::CountNumberOfLights()
//BFS01{
//BFS01 UINT16 nNumLights = 0;
//BFS01
//BFS01 SdrObjList* pSubList = GetSubList();
//BFS01 if(pSubList)
//BFS01 {
//BFS01     SdrObjListIter a3DIterator(*pSubList, IM_DEEPWITHGROUPS);
//BFS01     while ( a3DIterator.IsMore() )
//BFS01     {
//BFS01         E3dObject* pObj = (E3dObject*) a3DIterator.Next();
//BFS01         DBG_ASSERT(pObj->ISA(E3dObject), "AW: In Szenen sind nur 3D-Objekte erlaubt!");
//BFS01         if(pObj->ISA(E3dLight))
//BFS01         {
//BFS01             // Zaehlen...
//BFS01             nNumLights++;
//BFS01         }
//BFS01     }
//BFS01 }
//BFS01 return nNumLights;
//BFS01}

/*************************************************************************
|*
|* SnapRect berechnen
|*
\************************************************************************/

void E3dScene::RecalcSnapRect()
{
    E3dScene* pScene = GetScene();
    if(pScene == this)
    {
        // Szene wird als 2D-Objekt benutzt, nimm SnapRect aus der
        // 2D Bildschrimdarstellung
        Camera3D& rCam = (Camera3D&)pScene->GetCamera();
        maSnapRect = rCam.GetDeviceWindow();
    }
    else
    {
        // Szene ist selbst Mitglied einer anderen Szene, hole das
        // SnapRect als zusammengesetztes Objekt
        E3dObject::RecalcSnapRect();
    }
}

/*************************************************************************
|*
|* Aufbrechen
|*
\************************************************************************/

BOOL E3dScene::IsBreakObjPossible()
{
    // Szene ist aufzubrechen, wenn alle Mitglieder aufzubrechen sind
    SdrObjList* pSubList = GetSubList();
    if(pSubList)
    {
        SdrObjListIter a3DIterator(*pSubList, IM_DEEPWITHGROUPS);
        while ( a3DIterator.IsMore() )
        {
            E3dObject* pObj = (E3dObject*) a3DIterator.Next();
            DBG_ASSERT(pObj->ISA(E3dObject), "AW: In Szenen sind nur 3D-Objekte erlaubt!");
            if(!pObj->IsBreakObjPossible())
                return FALSE;
        }
    }
    return TRUE;
}

Vector3D E3dScene::GetShadowPlaneDirection() const
{
    double fWink = (double)GetShadowSlant() * F_PI180;
    Vector3D aShadowPlaneDir(0.0, sin(fWink), cos(fWink));
    aShadowPlaneDir.Normalize();
    return aShadowPlaneDir;
}

void E3dScene::SetShadowPlaneDirection(const Vector3D& rVec)
{
    UINT16 nSceneShadowSlant = (UINT16)((atan2(rVec.Y(), rVec.Z()) / F_PI180) + 0.5);
    GetProperties().SetObjectItemDirect(Svx3DShadowSlantItem(nSceneShadowSlant));
}


// #115662#
// helper class for in-between results from E3dScene::HitTest
class ImplPairDephAndObject
{
public:
    SdrObject* pObject;
    double fDepth;

    // for ::std::sort
    bool operator<(const ImplPairDephAndObject& rComp) const;
};

bool ImplPairDephAndObject::operator<(const ImplPairDephAndObject& rComp) const
{
    if(fDepth < rComp.fDepth)
        return true;
    return false;
}

// #115662#
// For new chart, calculate the number of hit contained 3D objects at given point,
// give back the count and a depth-sorted list of SdrObjects (a Vector). The vector will be
// changed, at least cleared.
sal_uInt32 E3dScene::HitTest(const Point& rHitTestPosition, ::std::vector< SdrObject* >& o_rResult)
{
    // prepare output variables
    sal_uInt32 nRetval(0L);
    o_rResult.clear();
    SdrObjList* pList = GetSubList();

    if(pList && pList->GetObjCount())
    {
        SdrObjListIter aIterator(*pList, IM_DEEPNOGROUPS);
        ::std::vector< ImplPairDephAndObject > aDepthAndObjectResults;

        while(aIterator.IsMore())
        {
            SdrObject* pObj = aIterator.Next();

            if(pObj->ISA(E3dCompoundObject))
            {
                // get HitLine in local 3D ObjectKoordinates
                Matrix4D mTransform = ((E3dCompoundObject*)pObj)->GetFullTransform();
                GetCameraSet().SetObjectTrans(mTransform);

                // create HitPoint Front und Back, transform to local object coordinates
                Vector3D aFront(rHitTestPosition.X(), rHitTestPosition.Y(), 0.0);
                Vector3D aBack(rHitTestPosition.X(), rHitTestPosition.Y(), ZBUFFER_DEPTH_RANGE);
                aFront = GetCameraSet().ViewToObjectCoor(aFront);
                aBack = GetCameraSet().ViewToObjectCoor(aBack);

                // make BoundVolume HitTest for speedup first
                const Volume3D& rBoundVol = ((E3dCompoundObject*)pObj)->GetBoundVolume();

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
                                // BoundVol is hit, get geometry cuts now
                                Vector3DVector aParameter;
                                B3dGeometry& rGeometry = ((E3dCompoundObject*)pObj)->GetDisplayGeometry();
                                rGeometry.GetAllCuts(aParameter, aFront, aBack);

                                if(aParameter.size())
                                {
                                    // take first cut as base, use Z-Coor in ViewCoor (0 ..ZBUFFER_DEPTH_RANGE)
                                    ImplPairDephAndObject aTempResult;
                                    Vector3D aTempVector(aParameter[0]);
                                    aTempVector = GetCameraSet().ObjectToViewCoor(aTempVector);

                                    aTempResult.pObject = pObj;
                                    aTempResult.fDepth = aTempVector.Z();

                                    // look for cut points in front of the first one
                                    Vector3DVector::iterator aIterator(aParameter.begin());
                                    aIterator++;

                                    for(;aIterator != aParameter.end(); aIterator++)
                                    {
                                        Vector3D aTempVector(*aIterator);
                                        aTempVector = GetCameraSet().ObjectToViewCoor(aTempVector);

                                        // use the smallest one
                                        if(aTempVector.Z() < aTempResult.fDepth)
                                        {
                                            aTempResult.fDepth = aTempVector.Z();
                                        }
                                    }

                                    // remember smallest cut with this object
                                    aDepthAndObjectResults.push_back(aTempResult);
                                }
                            }
                        }
                    }
                }
            }
        }

        // fill nRetval
        nRetval = aDepthAndObjectResults.size();

        if(nRetval)
        {
            // sort aDepthAndObjectResults by depth
            ::std::sort(aDepthAndObjectResults.begin(), aDepthAndObjectResults.end());

            // copy SdrObject pointers to return result set
            ::std::vector< ImplPairDephAndObject >::iterator aIterator(aDepthAndObjectResults.begin());

            for(;aIterator != aDepthAndObjectResults.end(); aIterator++)
            {
                o_rResult.push_back(aIterator->pObject);
            }
        }
    }

    return nRetval;
}

// eof
