/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuconcs.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 15:41:56 $
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
#include "precompiled_sd.hxx"

#include "fuconcs.hxx"

#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif


#include <svx/svxids.hrc>
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>

#include "app.hrc"
#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif

#ifndef _SVX_XLNSTWIT_HXX //autogen
#include <svx/xlnstwit.hxx>
#endif
#ifndef _SVX_XLNEDWIT_HXX //autogen
#include <svx/xlnedwit.hxx>
#endif
#ifndef _SVX_XLNEDIT_HXX //autogen
#include <svx/xlnedit.hxx>
#endif
#ifndef _SVX_XLNSTIT_HXX //autogen
#include <svx/xlnstit.hxx>
#endif
#ifndef _SVX_XLNWTIT_HXX //autogen
#include <svx/xlnwtit.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SDTMFITM_HXX //autogen
#include <svx/sdtmfitm.hxx>
#endif
#ifndef _SXEKITM_HXX //autogen
#include <svx/sxekitm.hxx>
#endif
#ifndef _SDERITM_HXX //autogen
#include <svx/sderitm.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVDOPATH_HXX //autogen
#include <svx/svdopath.hxx>
#endif
#ifndef _SVDOCIRC_HXX //autogen
#include <svx/svdocirc.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVX_ADJITEM_HXX
#include <svx/adjitem.hxx>
#endif
#ifndef _XTABLE_HXX
#include <svx/xtable.hxx>
#endif
#ifndef _SDASITM_HXX
#include <svx/sdasitm.hxx>
#endif
#ifndef _SVX_TBXCUSTOMSHAPES_HXX
#include <svx/tbxcustomshapes.hxx>
#endif
#ifndef _SVDOASHP_HXX
#include <svx/svdoashp.hxx>
#endif
#ifndef _SDTAGITM_HXX
#include <svx/sdtagitm.hxx>
#endif

// #88751#
#ifndef _SVDCAPT_HXX
#include <svx/svdocapt.hxx>
#endif

// #97016#
#ifndef _SVDOMEAS_HXX
#include <svx/svdomeas.hxx>
#endif

#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_TOOL_BAR_MANAGER_HXX
#include "ToolBarManager.hxx"
#endif
// #109583#
#ifndef _SVX_WRITINGMODEITEM_HXX
#include <svx/writingmodeitem.hxx>
#endif
#ifndef _GALLERY_HXX_
#include <svx/gallery.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX
#include <svtools/itempool.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#include "sdresid.hxx"
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#include "sdpage.hxx"
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "stlpool.hxx"
#include "drawdoc.hxx"
#include "res_bmp.hrc"
#include "glob.hrc"

namespace sd {

TYPEINIT1( FuConstructCustomShape, FuConstruct );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuConstructCustomShape::FuConstructCustomShape (
        ViewShell*          pViewSh,
        ::sd::Window*       pWin,
        ::sd::View*         pView,
        SdDrawDocument*     pDoc,
        SfxRequest&         rReq ) :
    FuConstruct(pViewSh, pWin, pView, pDoc, rReq)
{
}

FunctionReference FuConstructCustomShape::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq, bool bPermanent )
{
    FuConstructCustomShape* pFunc;
    FunctionReference xFunc( pFunc = new FuConstructCustomShape( pViewSh, pWin, pView, pDoc, rReq ) );
    xFunc->DoExecute(rReq);
    pFunc->SetPermanent( bPermanent );
    return xFunc;
}

void FuConstructCustomShape::DoExecute( SfxRequest& rReq )
{
    FuConstruct::DoExecute( rReq );

    const SfxItemSet* pArgs = rReq.GetArgs();
    if ( pArgs )
    {
        const SfxStringItem& rItm = (const SfxStringItem&)pArgs->Get( rReq.GetSlot() );
        aCustomShape = rItm.GetValue();
    }

    mpViewShell->GetViewShellBase().GetToolBarManager()->SetToolBar(
        ToolBarManager::TBG_FUNCTION,
        ToolBarManager::msDrawingObjectToolBar);
}

/*************************************************************************
|*
|* MouseButtonDown-event
|*
\************************************************************************/

BOOL FuConstructCustomShape::MouseButtonDown(const MouseEvent& rMEvt)
{
    BOOL bReturn = FuConstruct::MouseButtonDown(rMEvt);

    if ( rMEvt.IsLeft() && !mpView->IsAction() )
    {
        Point aPnt( mpWindow->PixelToLogic( rMEvt.GetPosPixel() ) );

        mpWindow->CaptureMouse();
        USHORT nDrgLog = USHORT ( mpWindow->PixelToLogic(Size(DRGPIX,0)).Width() );

        mpView->BegCreateObj(aPnt, (OutputDevice*) NULL, nDrgLog);

        SdrObject* pObj = mpView->GetCreateObj();
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
            SfxItemSet aAttr(mpDoc->GetPool());
            SetStyleSheet( aAttr, pObj, bForceFillStyle, bForceNoFillStyle );
            pObj->SetMergedItemSet(aAttr);
        }
    }

    return bReturn;
}

/*************************************************************************
|*
|* MouseMove-event
|*
\************************************************************************/

BOOL FuConstructCustomShape::MouseMove(const MouseEvent& rMEvt)
{
    return FuConstruct::MouseMove(rMEvt);
}

/*************************************************************************
|*
|* MouseButtonUp-event
|*
\************************************************************************/

BOOL FuConstructCustomShape::MouseButtonUp(const MouseEvent& rMEvt)
{
    sal_Bool bReturn(sal_False);

    if(mpView->IsCreateObj() && rMEvt.IsLeft())
    {
        SdrObject* pObj = mpView->GetCreateObj();
        if( pObj && mpView->EndCreateObj( SDRCREATE_FORCEEND ) )
        {
            bReturn = sal_True;
        }
    }
    bReturn = FuConstruct::MouseButtonUp (rMEvt) || bReturn;

    if (!bPermanent)
        mpViewShell->GetViewFrame()->GetDispatcher()->Execute(SID_OBJECT_SELECT, SFX_CALLMODE_ASYNCHRON);

    return bReturn;
}

/*************************************************************************
|*
|* Tastaturereignisse bearbeiten
|*
|* Wird ein KeyEvent bearbeitet, so ist der Return-Wert TRUE, andernfalls
|* FALSE.
|*
\************************************************************************/

BOOL FuConstructCustomShape::KeyInput(const KeyEvent& rKEvt)
{
    BOOL bReturn = FuConstruct::KeyInput(rKEvt);
    return(bReturn);
}

/*************************************************************************
|*
|* Function aktivieren
|*
\************************************************************************/

void FuConstructCustomShape::Activate()
{
    mpView->SetCurrentObj( OBJ_CUSTOMSHAPE );
    FuConstruct::Activate();
}

/*************************************************************************
|*
|* Attribute fuer das zu erzeugende Objekt setzen
|*
\************************************************************************/

void FuConstructCustomShape::SetAttributes( SdrObject* pObj )
{
    sal_Bool bAttributesAppliedFromGallery = sal_False;

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
                        const SdrPage* pPage = aFormModel.GetPage( 0 );
                        if ( pPage )
                        {
                            const SdrObject* pSourceObj = pPage->GetObj( 0 );
                            if( pSourceObj )
                            {
                                const SfxItemSet& rSource = pSourceObj->GetMergedItemSet();
                                SfxItemSet aDest( pObj->GetModel()->GetItemPool(),              // ranges from SdrAttrObj
                                SDRATTR_START, SDRATTR_SHADOW_LAST,
                                SDRATTR_MISC_FIRST, SDRATTR_MISC_LAST,
                                SDRATTR_TEXTDIRECTION, SDRATTR_TEXTDIRECTION,
                                // Graphic Attributes
                                SDRATTR_GRAF_FIRST, SDRATTR_GRAF_LAST,
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


    /*
                                com::sun::star::uno::Any aAny;
                                if ( ((SdrCustomShapeGeometryItem&)pObj->GetMergedItem( SDRATTR_CUSTOMSHAPE_GEOMETRY )).QueryValue( aAny ) )
                                {
                                    aGeometryItem.PutValue( aAny );
                                    pObj->SetMergedItem( aGeometryItem );
                                    bAttributesAppliedFromGallery = sal_True;
                                }
    */
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    if ( !bAttributesAppliedFromGallery )
    {
        pObj->SetMergedItem( SvxAdjustItem( SVX_ADJUST_CENTER, EE_PARA_JUST ) );
        pObj->SetMergedItem( SdrTextVertAdjustItem( SDRTEXTVERTADJUST_CENTER ) );
        pObj->SetMergedItem( SdrTextHorzAdjustItem( SDRTEXTHORZADJUST_BLOCK ) );
        pObj->SetMergedItem( SdrTextAutoGrowHeightItem( sal_False ) );
        ((SdrObjCustomShape*)pObj)->MergeDefaultAttributes( &aCustomShape );
    }
}

// #97016#
SdrObject* FuConstructCustomShape::CreateDefaultObject(const sal_uInt16, const Rectangle& rRectangle)
{
    SdrObject* pObj = SdrObjFactory::MakeNewObject(
        mpView->GetCurrentObjInventor(), mpView->GetCurrentObjIdentifier(),
        0L, mpDoc);

    if( pObj )
    {
        Rectangle aRect( rRectangle );
        if ( doConstructOrthogonal() )
            ImpForceQuadratic( aRect );
        pObj->SetLogicRect( aRect );
        SetAttributes( pObj );
        SfxItemSet aAttr(mpDoc->GetPool());
        SetStyleSheet(aAttr, pObj);
        pObj->SetMergedItemSet(aAttr);
    }
    return pObj;
}

// #i33136#
bool FuConstructCustomShape::doConstructOrthogonal() const
{
    return SdrObjCustomShape::doConstructOrthogonal(aCustomShape);
}

} // end of namespace sd
