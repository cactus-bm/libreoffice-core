/*************************************************************************
 *
 *  $RCSfile: fuconcustomshape.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-10-12 17:57:42 $
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

//------------------------------------------------------------------------

#ifndef SC_FUCONCUSTOMSHAPE_HXX
#include <fuconcustomshape.hxx>
#endif
#ifndef _GALLERY_HXX_
#include <svx/gallery.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _FM_FMMODEL_HXX
#include <svx/fmmodel.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX
#include <svtools/itempool.hxx>
#endif
#ifndef _SVDPAGE_HXX
#include <svx/svdpage.hxx>
#endif
#ifndef _SVDOASHP_HXX
#include <svx/svdoashp.hxx>
#endif
#ifndef _EEITEM_HXX
#include <svx/eeitem.hxx>
#endif
#ifndef _SDTAGITM_HXX
#include <svx/sdtagitm.hxx>
#endif
#include <svx/svdview.hxx>
#include "fuconuno.hxx"
#include "tabvwsh.hxx"
#include "sc.hrc"

//------------------------------------------------------------------------

FuConstCustomShape::FuConstCustomShape( ScTabViewShell* pViewSh, Window* pWin, SdrView* pView, SdrModel* pDoc, SfxRequest& rReq )
    : FuConstruct( pViewSh, pWin, pView, pDoc, rReq )
{
    const SfxItemSet* pArgs = rReq.GetArgs();
    if ( pArgs )
    {
        const SfxStringItem& rItm = (const SfxStringItem&)pArgs->Get( rReq.GetSlot() );
        aCustomShape = rItm.GetValue();
    }
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

FuConstCustomShape::~FuConstCustomShape()
{
}

/*************************************************************************
|*
|* MouseButtonDown-event
|*
\************************************************************************/

BOOL __EXPORT FuConstCustomShape::MouseButtonDown(const MouseEvent& rMEvt)
{
    // #95491# remember button state for creation of own MouseEvents
    SetMouseButtonCode(rMEvt.GetButtons());

    BOOL bReturn = FuConstruct::MouseButtonDown(rMEvt);
    if ( rMEvt.IsLeft() && !pView->IsAction() )
    {
        Point aPnt( pWindow->PixelToLogic( rMEvt.GetPosPixel() ) );
        pWindow->CaptureMouse();
        pView->BegCreateObj(aPnt);

        SdrObject* pObj = pView->GetCreateObj();
        if ( pObj )
        {
            SetAttributes( pObj );
            sal_Bool bForceFillStyle = sal_True;
            sal_Bool bForceNoFillStyle = sal_False;
            if ( ((SdrObjCustomShape*)pObj)->UseNoFillStyle() )
            {
                bForceFillStyle = sal_False;
                bForceNoFillStyle = sal_True;
            }
            if ( bForceNoFillStyle )
                pObj->SetMergedItem( XFillStyleItem( XFILL_NONE ) );
        }

        bReturn = TRUE;
    }
    return bReturn;
}

/*************************************************************************
|*
|* MouseMove-event
|*
\************************************************************************/

BOOL __EXPORT FuConstCustomShape::MouseMove(const MouseEvent& rMEvt)
{
    return FuConstruct::MouseMove(rMEvt);
}

/*************************************************************************
|*
|* MouseButtonUp-event
|*
\************************************************************************/

BOOL __EXPORT FuConstCustomShape::MouseButtonUp(const MouseEvent& rMEvt)
{
    // #95491# remember button state for creation of own MouseEvents
    SetMouseButtonCode(rMEvt.GetButtons());

    BOOL bReturn = FALSE;

    if ( pView->IsCreateObj() && rMEvt.IsLeft() )
    {
        Point aPnt( pWindow->PixelToLogic( rMEvt.GetPosPixel() ) );
        pView->EndCreateObj(SDRCREATE_FORCEEND);
        bReturn = TRUE;
    }
    return (FuConstruct::MouseButtonUp(rMEvt) || bReturn);
}

/*************************************************************************
|*
|* Tastaturereignisse bearbeiten
|*
|* Wird ein KeyEvent bearbeitet, so ist der Return-Wert TRUE, andernfalls
|* FALSE.
|*
\************************************************************************/

BOOL __EXPORT FuConstCustomShape::KeyInput(const KeyEvent& rKEvt)
{
    BOOL bReturn = FuConstruct::KeyInput(rKEvt);
    return(bReturn);
}

/*************************************************************************
|*
|* Function aktivieren
|*
\************************************************************************/

void FuConstCustomShape::Activate()
{
    pView->SetCurrentObj( OBJ_CUSTOMSHAPE, SdrInventor );

    aNewPointer = Pointer( POINTER_DRAW_RECT );
    aOldPointer = pWindow->GetPointer();
    pViewShell->SetActivePointer( aNewPointer );

    SdrLayer* pLayer = pView->GetModel()->GetLayerAdmin().GetLayerPerID(SC_LAYER_CONTROLS);
    if (pLayer)
        pView->SetActiveLayer( pLayer->GetName() );

    FuConstruct::Activate();
}

/*************************************************************************
|*
|* Function deaktivieren
|*
\************************************************************************/

void FuConstCustomShape::Deactivate()
{
    FuConstruct::Deactivate();

    SdrLayer* pLayer = pView->GetModel()->GetLayerAdmin().GetLayerPerID(SC_LAYER_FRONT);
    if (pLayer)
        pView->SetActiveLayer( pLayer->GetName() );

    pViewShell->SetActivePointer( aOldPointer );
}

// #98185# Create default drawing objects via keyboard
SdrObject* FuConstCustomShape::CreateDefaultObject(const sal_uInt16 nID, const Rectangle& rRectangle)
{
    // case SID_FM_CREATE_CONTROL:

    SdrObject* pObj = SdrObjFactory::MakeNewObject(
        pView->GetCurrentObjInventor(), pView->GetCurrentObjIdentifier(),
        0L, pDrDoc);

    if(pObj)
    {
        pObj->SetLogicRect(rRectangle);
    }

    return pObj;
}

/*************************************************************************
|*
|* applying attributes
|*
\************************************************************************/

void FuConstCustomShape::SetAttributes( SdrObject* pObj )
{
    sal_Bool bAttributesAppliedFromGallery = sal_False;

    SdrCustomShapeGeometryItem aGeometryItem( (SdrCustomShapeGeometryItem&)pObj->GetMergedItem( SDRATTR_CUSTOMSHAPE_GEOMETRY ) );
    if ( GalleryExplorer::GetSdrObjCount( GALLERY_THEME_POWERPOINT ) )
    {
        std::vector< rtl::OUString > aObjList;
        if ( GalleryExplorer::FillObjListTitle( GALLERY_THEME_POWERPOINT, aObjList ) )
        {
            sal_uInt16 i;
            for ( i = 0; i < aObjList.size(); i++ )
            {
                if ( aObjList[ i ].equalsIgnoreAsciiCase( aCustomShape ) )
                {
                    FmFormModel aFormModel;
                    SfxItemPool& rPool = aFormModel.GetItemPool();
                    rPool.FreezeIdRanges();
                    if ( GalleryExplorer::GetSdrObj( GALLERY_THEME_POWERPOINT, i, &aFormModel ) )
                    {
                        const SdrObject* pSourceObj = aFormModel.GetPage( 0 )->GetObj( 0 );
                        if( pSourceObj )
                        {
                            const SfxItemSet& rSource = pSourceObj->GetMergedItemSet();
                            SfxItemSet aDest( pObj->GetModel()->GetItemPool(),              // ranges from SdrAttrObj
                            SDRATTR_START, SDRATTRSET_SHADOW,
                            SDRATTRSET_OUTLINER, SDRATTRSET_MISC,
                            SDRATTR_TEXTDIRECTION, SDRATTR_TEXTDIRECTION,
                            // Graphic Attributes
                            SDRATTR_GRAF_FIRST, SDRATTRSET_GRAF,
                            // 3d Properties
                            SDRATTR_3D_FIRST, SDRATTR_3D_LAST,
                            // CustomShape properties
                            SDRATTR_CUSTOMSHAPE_FIRST, SDRATTR_CUSTOMSHAPE_LAST,
                            // range from SdrTextObj
                            EE_ITEMS_START, EE_ITEMS_END,
                            // end
                            0, 0);
                            aDest.Set( rSource );
                            pObj->SetMergedItemSet( aDest );
                            sal_Int32 nAngle = pSourceObj->GetRotateAngle();
                            if ( nAngle )
                            {
                                double a = nAngle * F_PI18000;
                                pObj->NbcRotate( pObj->GetSnapRect().Center(), nAngle, sin( a ), cos( a ) );
                            }
                            bAttributesAppliedFromGallery = sal_True;
                        }
                    }
                    break;
                }
            }
        }
    }
    if ( !bAttributesAppliedFromGallery )
    {
        const rtl::OUString sPredefinedType( RTL_CONSTASCII_USTRINGPARAM ( "PredefinedType" ) );
        com::sun::star::beans::PropertyValue aPropVal;
        aPropVal.Name = sPredefinedType;
        aPropVal.Value <<= aCustomShape;
        aGeometryItem.SetPropertyValue( aPropVal );
        pObj->SetMergedItem( aGeometryItem );
        pObj->SetMergedItem( SdrTextAutoGrowHeightItem( sal_False ) );
    }
}

