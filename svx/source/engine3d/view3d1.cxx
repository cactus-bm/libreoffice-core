/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: view3d1.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 04:59:34 $
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

#define ITEMID_COLOR            SID_ATTR_3D_LIGHTCOLOR

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SVDITER_HXX
#include "svditer.hxx"
#endif

#ifndef _SVDPOOL_HXX
#include "svdpool.hxx"
#endif

#ifndef _SVDMODEL_HXX
#include "svdmodel.hxx"
#endif

#ifndef _XOUTX_HXX
#include "xoutx.hxx"
#endif

#ifndef _SVX_SVXIDS_HRC
#include "svxids.hrc"
#endif

#ifndef _XTABLE_HXX
#include "xtable.hxx"
#endif

#ifndef _FM_FMVIEW_HXX
#include "fmview.hxx"
#endif

#ifndef _SVX_DIALOGS_HRC
#include "dialogs.hrc"
#endif

#ifndef _SVX_DIALMGR_HXX
#include "dialmgr.hxx"
#endif

#ifndef _E3D_GLOBL3D_HXX
#include "globl3d.hxx"
#endif

#ifndef _E3D_OBJ3D_HXX
#include "obj3d.hxx"
#endif

#ifndef _E3D_POLYSC3D_HXX
#include "polysc3d.hxx"
#endif

#ifndef _SVXE3DITEM_HXX
#include "e3ditem.hxx"
#endif

#ifndef _SVX_COLRITEM_HXX
#include "colritem.hxx"
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

#ifndef _E3D_UNDO_HXX
#include "e3dundo.hxx"
#endif

#ifndef _E3D_VIEW3D_HXX
#include "view3d.hxx"
#endif

#ifndef _E3D_CUBE3D_HXX
#include "cube3d.hxx"
#endif

#ifndef _SVX_XFLCLIT_HXX
#include "xflclit.hxx"
#endif

#ifndef _SVDOGRP_HXX
#include "svdogrp.hxx"
#endif

/*************************************************************************
|*
|* Konvertierung in Polygone
|*
\************************************************************************/

void E3dView::ConvertMarkedToPolyObj(BOOL bLineToArea)
{
    SdrObject* pNewObj = NULL;

    if (GetMarkedObjectCount() == 1)
    {
        SdrObject* pObj = GetMarkedObjectByIndex(0);

        if (pObj && pObj->ISA(E3dPolyScene))
        {
            BOOL bBezier = FALSE;
            pNewObj = ((E3dPolyScene*) pObj)->ConvertToPolyObj(bBezier, bLineToArea);

            if (pNewObj)
            {
                BegUndo(SVX_RESSTR(RID_SVX_3D_UNDO_EXTRUDE));
                ReplaceObject(pObj, *GetPageViewPvNum(0), pNewObj);
                EndUndo();
            }
        }
    }

    if (!pNewObj)
    {
        SdrView::ConvertMarkedToPolyObj(bLineToArea);
    }
}

/*************************************************************************
|*
|* Get3DAttributes
|*
\************************************************************************/

void Imp_E3dView_InorderRun3DObjects(const SdrObject* pObj, sal_uInt32& rMask, BOOL bCorrectScenes)
{
    if(pObj->ISA(E3dLatheObj))
    {
        rMask |= 0x0001;
    }
    else if(pObj->ISA(E3dExtrudeObj))
    {
        rMask |= 0x0002;
    }
    else if(pObj->ISA(E3dSphereObj))
    {
        rMask |= 0x0004;
    }
    else if(pObj->ISA(E3dCubeObj))
    {
        rMask |= 0x0008;
    }
    else if(pObj->IsGroupObject())
    {
        if(bCorrectScenes && pObj->ISA(E3dScene))
            ((E3dScene*)pObj)->CorrectSceneDimensions();

        SdrObjList* pList = pObj->GetSubList();
        for(sal_uInt32 a(0); a < pList->GetObjCount(); a++)
            Imp_E3dView_InorderRun3DObjects(pList->GetObj(a), rMask, bCorrectScenes);
    }
}

SfxItemSet E3dView::Get3DAttributes(E3dScene* pInScene, BOOL /*bOnly3DAttr*/) const
{
    // ItemSet mit entspr. Bereich anlegen
    SfxItemSet aSet(
        pMod->GetItemPool(),
        SDRATTR_START,      SDRATTR_END,
        SID_ATTR_3D_INTERN, SID_ATTR_3D_INTERN,
        0, 0);

    sal_uInt32 nSelectedItems(0L);

    if(pInScene)
    {
        // special scene
        aSet.Put(pInScene->GetMergedItemSet());
    }
    else
    {
        // get attributes from all selected objects
        MergeAttrFromMarked(aSet, FALSE);

        // calc flags for SID_ATTR_3D_INTERN
        const SdrMarkList& rMarkList = GetMarkedObjectList();
        sal_uInt32 nMarkCnt(rMarkList.GetMarkCount());

        for(sal_uInt32 a(0); a < nMarkCnt; a++)
        {
            SdrObject* pObj = GetMarkedObjectByIndex(a);
            Imp_E3dView_InorderRun3DObjects(pObj, nSelectedItems, FALSE);
        }
    }

    // setze SID_ATTR_3D_INTERN auf den Status der selektierten Objekte
    aSet.Put(SfxUInt32Item(SID_ATTR_3D_INTERN, nSelectedItems));

    // DefaultValues pflegen
    if(!nSelectedItems  && !pInScene)
    {
        // Defaults holen und hinzufuegen
        SfxItemSet aDefaultSet(pMod->GetItemPool(), SDRATTR_3D_FIRST, SDRATTR_3D_LAST);
        GetAttributes(aDefaultSet);
        aSet.Put(aDefaultSet);

        // ... aber keine Linien fuer 3D
        aSet.Put(XLineStyleItem (XLINE_NONE));

        // #84061# new defaults for distance and focal length
        aSet.Put(Svx3DDistanceItem(100));
        aSet.Put(Svx3DFocalLengthItem(10000));
    }

    // ItemSet zurueckgeben
    return(aSet);
}

/*************************************************************************
|*
|* Set3DAttributes:
|*
\************************************************************************/

void E3dView::Set3DAttributes( const SfxItemSet& rAttr, E3dScene* pInScene, BOOL bReplaceAll)
{
    sal_uInt32 nSelectedItems(0L);

    if(pInScene)
    {
        //pInScene->SetItemSetAndBroadcast(rAttr, bReplaceAll);
        pInScene->SetMergedItemSetAndBroadcast(rAttr, bReplaceAll);
    }
    else
    {
        // set at selected objects
        SetAttrToMarked(rAttr, bReplaceAll);

        // Durchlauf mit Korrektur der veraenderten Szenen,
        // da die enthaltenen Objekte geometrisch veraendert sein koennen
        const SdrMarkList& rMarkList = GetMarkedObjectList();
        sal_uInt32 nMarkCnt(rMarkList.GetMarkCount());

        for(sal_uInt32 a(0); a < nMarkCnt; a++)
        {
            SdrObject* pObj = GetMarkedObjectByIndex(a);
            Imp_E3dView_InorderRun3DObjects(pObj, nSelectedItems, TRUE);
        }

    }

    // DefaultValues pflegen
    if(!nSelectedItems && !pInScene)
    {
        // Defaults setzen
        SfxItemSet aDefaultSet(pMod->GetItemPool(), SDRATTR_3D_FIRST, SDRATTR_3D_LAST);
        aDefaultSet.Put(rAttr);
        SetAttributes(aDefaultSet);

    }
}

double E3dView::GetDefaultCamPosZ()
{
    return (double)((const SfxUInt32Item&)pMod->GetItemPool().GetDefaultItem(SDRATTR_3DSCENE_DISTANCE)).GetValue();
}

double E3dView::GetDefaultCamFocal()
{
    return (double)((const SfxUInt32Item&)pMod->GetItemPool().GetDefaultItem(SDRATTR_3DSCENE_FOCAL_LENGTH)).GetValue();
}

