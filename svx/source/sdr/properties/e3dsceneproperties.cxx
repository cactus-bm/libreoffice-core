/*************************************************************************
 *
 *  $RCSfile: e3dsceneproperties.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: pjunck $ $Date: 2004-11-03 10:50:32 $
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

#ifndef _SDR_PROPERTIES_E3DSCENEPROPERTIES_HXX
#include <svx/sdr/properties/e3dsceneproperties.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef _SFX_WHITER_HXX
#include <svtools/whiter.hxx>
#endif

#ifndef _SVDDEF_HXX
#include <svddef.hxx>
#endif

#ifndef _E3D_SCENE3D_HXX
#include <scene3d.hxx>
#endif

#ifndef _SVDITER_HXX
#include <svditer.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace properties
    {
        E3dSceneProperties::E3dSceneProperties(SdrObject& rObj)
        :   E3dProperties(rObj)
        {
        }

        E3dSceneProperties::E3dSceneProperties(const E3dSceneProperties& rProps, SdrObject& rObj)
        :   E3dProperties(rProps, rObj)
        {
        }

        E3dSceneProperties::~E3dSceneProperties()
        {
        }

        BaseProperties& E3dSceneProperties::Clone(SdrObject& rObj) const
        {
            return *(new E3dSceneProperties(*this, rObj));
        }

        const SfxItemSet& E3dSceneProperties::GetObjectItemSet() const
        {
            //DBG_ASSERT(sal_False, "E3dSceneProperties::GetObjectItemSet() maybe the wrong call (!)");
            return E3dProperties::GetObjectItemSet();
        }

        const SfxItemSet& E3dSceneProperties::GetMergedItemSet() const
        {
            // prepare ItemSet
            if(mpItemSet)
            {
                // filter for SDRATTR_3DSCENE_ items, only keep those items
                SfxItemSet aNew(*mpItemSet->GetPool(), SDRATTR_3DSCENE_FIRST, SDRATTR_3DSCENE_LAST);
                aNew.Put(*mpItemSet);
                mpItemSet->ClearItem();
                mpItemSet->Put(aNew);
            }
            else
            {
                // No ItemSet yet, force local ItemSet
                GetObjectItemSet();
            }

            // collect all ItemSets of contained 3d objects
            const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
            const sal_uInt32 nCount(pSub->GetObjCount());

            for(sal_uInt32 a(0L); a < nCount; a++)
            {
                SdrObject* pObj = pSub->GetObj(a);

                if(pObj && pObj->ISA(E3dCompoundObject))
                {
                    const SfxItemSet& rSet = pObj->GetMergedItemSet();
                    SfxWhichIter aIter(rSet);
                    sal_uInt16 nWhich(aIter.FirstWhich());

                    while(nWhich)
                    {
                        // Leave out the SDRATTR_3DSCENE_ range, this would only be double
                        // and always equal.
                        if(nWhich <= SDRATTR_3DSCENE_FIRST || nWhich >= SDRATTR_3DSCENE_LAST)
                        {
                            if(SFX_ITEM_DONTCARE == rSet.GetItemState(nWhich, FALSE))
                            {
                                mpItemSet->InvalidateItem(nWhich);
                            }
                            else
                            {
                                mpItemSet->MergeValue(rSet.Get(nWhich), TRUE);
                            }
                        }

                        nWhich = aIter.NextWhich();
                    }
                }
            }

            // call parent
            return E3dProperties::GetMergedItemSet();
        }

        void E3dSceneProperties::SetMergedItemSet(const SfxItemSet& rSet, sal_Bool bClearAllItems)
        {
            // Set SDRATTR_3DOBJ_ range at contained objects.
            const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
            const sal_uInt32 nCount(pSub->GetObjCount());

            if(nCount)
            {
                // Generate filtered ItemSet which contains all but the SDRATTR_3DSCENE items.
                SfxItemSet& rNewSet = *(rSet.Clone(TRUE));
                DBG_ASSERT(&rNewSet, "E3dSceneProperties::SetMergedItemSet(): Could not clone ItemSet (!)");

                for(sal_uInt16 b(SDRATTR_3DSCENE_FIRST); b <= SDRATTR_3DSCENE_LAST; b++)
                {
                    rNewSet.ClearItem(b);
                }

                if(rNewSet.Count())
                {
                    for(sal_uInt32 a(0L); a < nCount; a++)
                    {
                        SdrObject* pObj = pSub->GetObj(a);

                        if(pObj && pObj->ISA(E3dCompoundObject))
                        {
                            // set merged ItemSet at contained 3d object.
                            pObj->SetMergedItemSet(rNewSet, bClearAllItems);
                        }
                    }
                }
            }

            // call parent. This will set items on local object, too.
            E3dProperties::SetMergedItemSet(rSet, bClearAllItems);
        }

        void E3dSceneProperties::SetMergedItem(const SfxPoolItem& rItem)
        {
            const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
            const sal_uInt32 nCount(pSub->GetObjCount());

            for(sal_uInt32 a(0L); a < nCount; a++)
            {
                pSub->GetObj(a)->SetMergedItem(rItem);
            }
        }

        void E3dSceneProperties::ClearMergedItem(const sal_uInt16 nWhich)
        {
            const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
            const sal_uInt32 nCount(pSub->GetObjCount());

            for(sal_uInt32 a(0L); a < nCount; a++)
            {
                pSub->GetObj(a)->ClearMergedItem(nWhich);
            }
        }

        void E3dSceneProperties::PostItemChange(const sal_uInt16 nWhich)
        {
            // call parent
            E3dProperties::PostItemChange(nWhich);

            // local changes
            E3dScene& rObj = (E3dScene&)GetSdrObject();
            rObj.StructureChanged(&((E3dScene&)GetSdrObject()));

            switch(nWhich)
            {
                case SDRATTR_3DSCENE_PERSPECTIVE            :
                case SDRATTR_3DSCENE_DISTANCE               :
                case SDRATTR_3DSCENE_FOCAL_LENGTH           :
                {
                    // #83387#, #83391#
                    // one common function for the camera attributes
                    // since SetCamera() sets all three back to the ItemSet
                    Camera3D aSceneCam(rObj.GetCamera());
                    sal_Bool bChange(sal_False);

                    // for SDRATTR_3DSCENE_PERSPECTIVE:
                    if(aSceneCam.GetProjection() != rObj.GetPerspective())
                    {
                        aSceneCam.SetProjection(rObj.GetPerspective());
                        bChange = sal_True;
                    }

                    // for SDRATTR_3DSCENE_DISTANCE:
                    Vector3D aActualPosition = aSceneCam.GetPosition();
                    double fNew = rObj.GetDistance();

                    if(fNew != aActualPosition.Z())
                    {
                        aSceneCam.SetPosition(Vector3D(aActualPosition.X(), aActualPosition.Y(), fNew));
                        bChange = sal_True;
                    }

                    // for SDRATTR_3DSCENE_FOCAL_LENGTH:
                    fNew = rObj.GetFocalLength() / 100.0;

                    if(aSceneCam.GetFocalLength() != fNew)
                    {
                        aSceneCam.SetFocalLength(fNew);
                        bChange = sal_True;
                    }

                    // for all
                    if(bChange)
                    {
                        rObj.SetCamera(aSceneCam);
                    }

                    break;
                }
                case SDRATTR_3DSCENE_TWO_SIDED_LIGHTING     :
                {
                    rObj.GetLightGroup().SetModelTwoSide(rObj.GetTwoSidedLighting());
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_1           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor1(), Base3DMaterialDiffuse, Base3DLight0);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_2           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor2(), Base3DMaterialDiffuse, Base3DLight1);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_3           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor3(), Base3DMaterialDiffuse, Base3DLight2);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_4           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor4(), Base3DMaterialDiffuse, Base3DLight3);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_5           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor5(), Base3DMaterialDiffuse, Base3DLight4);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_6           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor6(), Base3DMaterialDiffuse, Base3DLight5);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_7           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor7(), Base3DMaterialDiffuse, Base3DLight6);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTCOLOR_8           :
                {
                    rObj.GetLightGroup().SetIntensity( rObj.GetLightColor8(), Base3DMaterialDiffuse, Base3DLight7);
                    break;
                }
                case SDRATTR_3DSCENE_AMBIENTCOLOR           :
                {
                    rObj.GetLightGroup().SetGlobalAmbientLight(rObj.GetGlobalAmbientColor());
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_1              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff1(), Base3DLight0);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_2              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff2(), Base3DLight1);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_3              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff3(), Base3DLight2);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_4              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff4(), Base3DLight3);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_5              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff5(), Base3DLight4);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_6              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff6(), Base3DLight5);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_7              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff7(), Base3DLight6);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTON_8              :
                {
                    rObj.GetLightGroup().Enable( rObj.GetLightOnOff8(), Base3DLight7);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_1       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection1(), Base3DLight0);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_2       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection2(), Base3DLight1);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_3       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection3(), Base3DLight2);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_4       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection4(), Base3DLight3);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_5       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection5(), Base3DLight4);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_6       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection6(), Base3DLight5);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_7       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection7(), Base3DLight6);
                    break;
                }
                case SDRATTR_3DSCENE_LIGHTDIRECTION_8       :
                {
                    rObj.GetLightGroup().SetDirection( rObj.GetLightDirection8(), Base3DLight7);
                    break;
                }

                // these are Item changes which may shrink/expand the object, e.g. line
                // size and line on/off
                case XATTR_LINESTYLE :
                case XATTR_LINEWIDTH :
                {
                    rObj.CorrectSceneDimensions();
                    break;
                }
            }
        }

        void E3dSceneProperties::SetStyleSheet(SfxStyleSheet* pNewStyleSheet, sal_Bool bDontRemoveHardAttr)
        {
            const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
            const sal_uInt32 nCount(pSub->GetObjCount());

            for(sal_uInt32 a(0L); a < nCount; a++)
            {
                pSub->GetObj(a)->SetStyleSheet(pNewStyleSheet, bDontRemoveHardAttr);
            }
        }

        SfxStyleSheet* E3dSceneProperties::GetStyleSheet() const
        {
            SfxStyleSheet* pRetval = 0L;

            const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
            const sal_uInt32 nCount(pSub->GetObjCount());

            for(sal_uInt32 a(0L); a < nCount; a++)
            {
                SfxStyleSheet* pCandidate = pSub->GetObj(a)->GetStyleSheet();

                if(pRetval)
                {
                    if(pCandidate != pRetval)
                    {
                        // different StyleSheelts, return none
                        return 0L;
                    }
                }
                else
                {
                    pRetval = pCandidate;
                }
            }

            return pRetval;
        }

//BFS01     void E3dSceneProperties::PreProcessSave()
//BFS01     {
//BFS01         const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
//BFS01         const sal_uInt32 nCount(pSub->GetObjCount());
//BFS01
//BFS01         for(sal_uInt32 a(0L); a < nCount; a++)
//BFS01         {
//BFS01             pSub->GetObj(a)->GetProperties().PreProcessSave();
//BFS01         }
//BFS01     }

//BFS01     void E3dSceneProperties::PostProcessSave()
//BFS01     {
//BFS01         const SdrObjList* pSub = ((const E3dScene&)GetSdrObject()).GetSubList();
//BFS01         const sal_uInt32 nCount(pSub->GetObjCount());
//BFS01
//BFS01         for(sal_uInt32 a(0L); a < nCount; a++)
//BFS01         {
//BFS01             pSub->GetObj(a)->GetProperties().PostProcessSave();
//BFS01         }
//BFS01     }

        void E3dSceneProperties::MoveToItemPool(SfxItemPool* pSrcPool, SfxItemPool* pDestPool, SdrModel* pNewModel)
        {
            if(pSrcPool && pDestPool && (pSrcPool != pDestPool))
            {
                // call parent
                E3dProperties::MoveToItemPool(pSrcPool, pDestPool, pNewModel);

                // own reaction, but only with outmost scene
                E3dScene& rObj = (E3dScene&)GetSdrObject();
                const SdrObjList* pSubList = rObj.GetSubList();

                if(pSubList && rObj.GetScene() == &rObj)
                {
                    SdrObjListIter a3DIterator(*pSubList, IM_DEEPWITHGROUPS);

                    while(a3DIterator.IsMore())
                    {
                        E3dObject* pObj = (E3dObject*)a3DIterator.Next();
                        DBG_ASSERT(pObj->ISA(E3dObject), "In scenes there are only 3D objects allowed (!)");
                        pObj->GetProperties().MoveToItemPool(pSrcPool, pDestPool, pNewModel);
                    }
                }
            }
        }

        void E3dSceneProperties::SetLightItemsFromLightGroup(B3dLightGroup& rLightGroup)
        {
            // force ItemSet
            GetObjectItemSet();

            // TwoSidedLighting
            mpItemSet->Put(Svx3DTwoSidedLightingItem(rLightGroup.GetModelTwoSide()));

            // LightColors
            mpItemSet->Put(Svx3DLightcolor1Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight0)));
            mpItemSet->Put(Svx3DLightcolor2Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight1)));
            mpItemSet->Put(Svx3DLightcolor3Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight2)));
            mpItemSet->Put(Svx3DLightcolor4Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight3)));
            mpItemSet->Put(Svx3DLightcolor5Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight4)));
            mpItemSet->Put(Svx3DLightcolor6Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight5)));
            mpItemSet->Put(Svx3DLightcolor7Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight6)));
            mpItemSet->Put(Svx3DLightcolor8Item(rLightGroup.GetIntensity(Base3DMaterialDiffuse, Base3DLight7)));

            // AmbientColor
            mpItemSet->Put(Svx3DAmbientcolorItem(rLightGroup.GetGlobalAmbientLight()));

            // LightOn
            mpItemSet->Put(Svx3DLightOnOff1Item(rLightGroup.IsEnabled(Base3DLight0)));
            mpItemSet->Put(Svx3DLightOnOff2Item(rLightGroup.IsEnabled(Base3DLight1)));
            mpItemSet->Put(Svx3DLightOnOff3Item(rLightGroup.IsEnabled(Base3DLight2)));
            mpItemSet->Put(Svx3DLightOnOff4Item(rLightGroup.IsEnabled(Base3DLight3)));
            mpItemSet->Put(Svx3DLightOnOff5Item(rLightGroup.IsEnabled(Base3DLight4)));
            mpItemSet->Put(Svx3DLightOnOff6Item(rLightGroup.IsEnabled(Base3DLight5)));
            mpItemSet->Put(Svx3DLightOnOff7Item(rLightGroup.IsEnabled(Base3DLight6)));
            mpItemSet->Put(Svx3DLightOnOff8Item(rLightGroup.IsEnabled(Base3DLight7)));

            // LightDirection
            mpItemSet->Put(Svx3DLightDirection1Item(rLightGroup.GetDirection( Base3DLight0 )));
            mpItemSet->Put(Svx3DLightDirection2Item(rLightGroup.GetDirection( Base3DLight1 )));
            mpItemSet->Put(Svx3DLightDirection3Item(rLightGroup.GetDirection( Base3DLight2 )));
            mpItemSet->Put(Svx3DLightDirection4Item(rLightGroup.GetDirection( Base3DLight3 )));
            mpItemSet->Put(Svx3DLightDirection5Item(rLightGroup.GetDirection( Base3DLight4 )));
            mpItemSet->Put(Svx3DLightDirection6Item(rLightGroup.GetDirection( Base3DLight5 )));
            mpItemSet->Put(Svx3DLightDirection7Item(rLightGroup.GetDirection( Base3DLight6 )));
            mpItemSet->Put(Svx3DLightDirection8Item(rLightGroup.GetDirection( Base3DLight7 )));
        }

        void E3dSceneProperties::SetSceneItemsFromCamera()
        {
            // force ItemSet
            GetObjectItemSet();

            E3dScene& rObj = (E3dScene&)GetSdrObject();
            Camera3D aSceneCam(rObj.GetCamera());

            // ProjectionType
            mpItemSet->Put(Svx3DPerspectiveItem((UINT16)aSceneCam.GetProjection()));

            // CamPos
            mpItemSet->Put(Svx3DDistanceItem((UINT32)(aSceneCam.GetPosition().Z() + 0.5)));

            // FocalLength
            mpItemSet->Put(Svx3DFocalLengthItem((UINT32)((aSceneCam.GetFocalLength() * 100.0) + 0.5)));
        }
    } // end of namespace properties
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
