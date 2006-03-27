/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drawsh2.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-27 10:06:16 $
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

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

//------------------------------------------------------------------
#ifndef _COM_SUN_STAR_EMBED_EMBEDMISC_HPP_
#include <com/sun/star/embed/EmbedMisc.hpp>
#endif

#include "scitems.hxx"
#include <svx/eeitem.hxx>
#include <svx/sizeitem.hxx>
#include <svx/svdpagv.hxx>
#include <svx/xdef.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/viewfrm.hxx>
#include <svtools/ptitem.hxx>
#include <svtools/whiter.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdouno.hxx>
#ifndef _SVX_EXTRUSION_BAR_HXX
#include <svx/extrusionbar.hxx>
#endif
#ifndef _SVX_FONTWORK_BAR_HXX
#include <svx/fontworkbar.hxx>
#endif

#include "drawsh.hxx"
#include "drawview.hxx"
#include "viewdata.hxx"
#include "sc.hrc"
#include "tabvwsh.hxx"


#ifndef _SVDOOLE2_HXX
#include <svx/svdoole2.hxx>
#endif
#ifndef _SVDCAPT_HXX
#include <svx/svdocapt.hxx>
#endif

USHORT ScGetFontWorkId();       // in drtxtob

using namespace com::sun::star;

//------------------------------------------------------------------

ScDrawShell::ScDrawShell( ScViewData* pData ) :
    SfxShell(pData->GetViewShell()),
    pViewData( pData )
{
    SetPool( &pViewData->GetScDrawView()->GetModel()->GetItemPool() );
    SetUndoManager( pViewData->GetSfxDocShell()->GetUndoManager() );
    SetHelpId( HID_SCSHELL_DRAWSH );
    SetName(String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("Drawing")));
}

ScDrawShell::~ScDrawShell()
{
}

void ScDrawShell::GetState( SfxItemSet& rSet )          // Zustaende / Toggles
{
    ScDrawView* pView    = pViewData->GetScDrawView();
    SdrDragMode eMode    = pView->GetDragMode();

    rSet.Put( SfxBoolItem( SID_OBJECT_ROTATE, eMode == SDRDRAG_ROTATE ) );
    rSet.Put( SfxBoolItem( SID_OBJECT_MIRROR, eMode == SDRDRAG_MIRROR ) );
    rSet.Put( SfxBoolItem( SID_BEZIER_EDIT, !pView->IsFrameDragSingles() ) );

    USHORT nFWId = ScGetFontWorkId();
    SfxViewFrame* pViewFrm = pViewData->GetViewShell()->GetViewFrame();
    rSet.Put(SfxBoolItem(SID_FONTWORK, pViewFrm->HasChildWindow(nFWId)));

        // Notes always default to Page anchor.
    bool bDisableAnchor = false;
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    ULONG nMarkCount = rMarkList.GetMarkCount();
    if ( nMarkCount == 1 )
    {
            SdrObject* pObj = rMarkList.GetMark( 0 )->GetObj();
            if( pObj && pObj->GetObjIdentifier() == OBJ_CAPTION && pObj->GetLayer() == SC_LAYER_INTERN)
        {
                bDisableAnchor = true;
                rSet.DisableItem( SID_ANCHOR_PAGE );
                rSet.DisableItem( SID_ANCHOR_CELL );
        }
    }

    if ( !bDisableAnchor )
    {
        switch( pView->GetAnchor() )
        {
        case SCA_PAGE:
            rSet.Put( SfxBoolItem( SID_ANCHOR_PAGE, TRUE ) );
            rSet.Put( SfxBoolItem( SID_ANCHOR_CELL, FALSE ) );
        break;

        case SCA_CELL:
        rSet.Put( SfxBoolItem( SID_ANCHOR_PAGE, FALSE ) );
        rSet.Put( SfxBoolItem( SID_ANCHOR_CELL, TRUE ) );
        break;

        default:
        rSet.Put( SfxBoolItem( SID_ANCHOR_PAGE, FALSE ) );
        rSet.Put( SfxBoolItem( SID_ANCHOR_CELL, FALSE ) );
        break;
        }
    }
}

void ScDrawShell::GetDrawFuncState( SfxItemSet& rSet )      // Funktionen disablen
{
    ScDrawView* pView = pViewData->GetScDrawView();

    //  #111711# call IsMirrorAllowed first to make sure ForcePossibilities (and thus CheckMarked)
    //  is called before GetMarkCount, so the nMarkCount value is valid for the rest of this method.
    if (!pView->IsMirrorAllowed(TRUE,TRUE))
    {
        rSet.DisableItem( SID_MIRROR_HORIZONTAL );
        rSet.DisableItem( SID_MIRROR_VERTICAL );
    }

    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    ULONG nMarkCount = rMarkList.GetMarkCount();

    if ( nMarkCount <= 1 || !pView->IsGroupPossible() )
        rSet.DisableItem( SID_GROUP );
    if ( nMarkCount == 0 || !pView->IsUnGroupPossible() )
        rSet.DisableItem( SID_UNGROUP );
    if ( nMarkCount != 1 || !pView->IsGroupEnterPossible() )
        rSet.DisableItem( SID_ENTER_GROUP );
    if ( !pView->IsGroupEntered() )
        rSet.DisableItem( SID_LEAVE_GROUP );

    if ( nMarkCount <= 1 )                      // nichts oder nur ein Objekt selektiert
    {
            //  Ausrichtung
        rSet.DisableItem( SID_OBJECT_ALIGN_LEFT );      // keine Ausrichtung an der Seite
        rSet.DisableItem( SID_OBJECT_ALIGN_CENTER );
        rSet.DisableItem( SID_OBJECT_ALIGN_RIGHT );
        rSet.DisableItem( SID_OBJECT_ALIGN_UP );
        rSet.DisableItem( SID_OBJECT_ALIGN_MIDDLE );
        rSet.DisableItem( SID_OBJECT_ALIGN_DOWN );

        // pseudo slots for Format menu
        rSet.DisableItem( SID_ALIGN_ANY_LEFT );
        rSet.DisableItem( SID_ALIGN_ANY_HCENTER );
        rSet.DisableItem( SID_ALIGN_ANY_RIGHT );
        rSet.DisableItem( SID_ALIGN_ANY_TOP );
        rSet.DisableItem( SID_ALIGN_ANY_VCENTER );
        rSet.DisableItem( SID_ALIGN_ANY_BOTTOM );
    }

    if ( !nMarkCount || pView->HasMarkedControl() )
    {
        //  Layer von Controls darf nicht veraendert werden
        rSet.DisableItem( SID_OBJECT_HEAVEN );
        rSet.DisableItem( SID_OBJECT_HELL );
    }
    else
    {
        if(AreAllObjectsOnLayer(SC_LAYER_FRONT,rMarkList))
        {
            rSet.DisableItem( SID_OBJECT_HEAVEN );
        }
        else if(AreAllObjectsOnLayer(SC_LAYER_BACK,rMarkList))
        {
            rSet.DisableItem( SID_OBJECT_HELL );
        }
    }

    BOOL bCanRename = FALSE;
    if ( nMarkCount == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetObj();
        SdrLayerID nLayerID = pObj->GetLayer();
        if ( nLayerID != SC_LAYER_INTERN )
            bCanRename = TRUE;                          // #i51351# anything except internal objects can be renamed

        // #91929#; don't show original size entry if not possible
        UINT16 nObjType = pObj->GetObjIdentifier();
        if ( nObjType == OBJ_OLE2 )
        {
            SdrOle2Obj* pOleObj = static_cast<SdrOle2Obj*>(rMarkList.GetMark( 0 )->GetObj());
            if (pOleObj->GetObjRef().is() &&
                ((pOleObj->GetObjRef()->getStatus( pOleObj->GetAspect() ) & embed::EmbedMisc::MS_EMBED_RECOMPOSEONRESIZE) ) )
                //TODO/LATER: why different slots in Draw and Calc?
                rSet.DisableItem(SID_ORIGINALSIZE);
        }
        else if ( nObjType == OBJ_CAPTION )
        {
            if ( nLayerID == SC_LAYER_INTERN )
            {
                // SdrCaptionObj() Notes cannot be cut/copy in isolation from
                // their cells.
                rSet.DisableItem( SID_CUT );
                rSet.DisableItem( SID_COPY );
                // Notes always default to Page anchor.
                rSet.DisableItem( SID_ANCHOR_TOGGLE );
            }
        }
    }
    if ( !bCanRename )
        rSet.DisableItem( SID_RENAME_OBJECT );

    if ( !nMarkCount )                          // nichts selektiert
    {
            //  Anordnung
        rSet.DisableItem( SID_FRAME_UP );
        rSet.DisableItem( SID_FRAME_DOWN );
        rSet.DisableItem( SID_FRAME_TO_TOP );
        rSet.DisableItem( SID_FRAME_TO_BOTTOM );
            //  Clipboard / loeschen
        rSet.DisableItem( SID_DELETE );
        rSet.DisableItem( SID_DELETE_CONTENTS );
        rSet.DisableItem( SID_CUT );
        rSet.DisableItem( SID_COPY );
            //  sonstiges
        rSet.DisableItem( SID_ANCHOR_TOGGLE );
        rSet.DisableItem( SID_ORIGINALSIZE );
        rSet.DisableItem( SID_ATTR_TRANSFORM );
    }

    if ( rSet.GetItemState( SID_ENABLE_HYPHENATION ) != SFX_ITEM_UNKNOWN )
    {
        SfxItemSet aAttrs( pView->GetModel()->GetItemPool() );
        pView->GetAttributes( aAttrs );
        if( aAttrs.GetItemState( EE_PARA_HYPHENATE ) >= SFX_ITEM_AVAILABLE )
        {
            BOOL bValue = ( (const SfxBoolItem&) aAttrs.Get( EE_PARA_HYPHENATE ) ).GetValue();
            rSet.Put( SfxBoolItem( SID_ENABLE_HYPHENATION, bValue ) );
        }
    }

    svx::ExtrusionBar::getState( pView, rSet );
    svx::FontworkBar::getState( pView, rSet );
}

//
//          Attribute fuer Drawing-Objekte
//

void ScDrawShell::GetDrawAttrState( SfxItemSet& rSet )
{
    Point       aMousePos   = pViewData->GetMousePosPixel();
    Window*     pWindow     = pViewData->GetActiveWin();
    ScDrawView* pDrView     = pViewData->GetScDrawView();
    Point       aPos        = pWindow->PixelToLogic(aMousePos);
    BOOL        bHasMarked  = pDrView->AreObjectsMarked();

    if( bHasMarked )
    {
        rSet.Put( pDrView->GetAttrFromMarked(FALSE) );

        // Wenn die View selektierte Objekte besitzt, muessen entspr. Items
        // von SFX_ITEM_DEFAULT (_ON) auf SFX_ITEM_DISABLED geaendert werden

        SfxWhichIter aIter( rSet, XATTR_LINE_FIRST, XATTR_FILL_LAST );
        USHORT nWhich = aIter.FirstWhich();
        while( nWhich )
        {
            if( SFX_ITEM_DEFAULT == rSet.GetItemState( nWhich ) )
                rSet.DisableItem( nWhich );

            nWhich = aIter.NextWhich();
        }
    }
    else
        rSet.Put( pDrView->GetDefaultAttr() );

    SdrPageView* pPV = pDrView->GetPageViewPvNum(0);
    if ( pPV )
    {
        // #i52073# when a sheet with an active OLE object is deleted,
        // the slot state is queried without an active page view

        //  Items for position and size (see ScGridWindow::UpdateStatusPosSize, #108137#)

        // #i34458# The SvxSizeItem in SID_TABLE_CELL is no longer needed by
        // SvxPosSizeStatusBarControl, it's enough to have it in SID_ATTR_SIZE.

        BOOL bActionItem = FALSE;
        if ( pDrView->IsAction() )              // action rectangle
        {
            Rectangle aRect;
            pDrView->TakeActionRect( aRect );
            if ( !aRect.IsEmpty() )
            {
                pPV->LogicToPagePos(aRect);
                rSet.Put( SfxPointItem( SID_ATTR_POSITION, aRect.TopLeft() ) );
                Size aSize( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() );
                rSet.Put( SvxSizeItem( SID_ATTR_SIZE, aSize ) );
                bActionItem = TRUE;
            }
        }
        if ( !bActionItem )
        {
            if ( pDrView->AreObjectsMarked() )      // selected objects
            {
                Rectangle aRect = pDrView->GetAllMarkedRect();
                pPV->LogicToPagePos(aRect);
                rSet.Put( SfxPointItem( SID_ATTR_POSITION, aRect.TopLeft() ) );
                Size aSize( aRect.Right() - aRect.Left(), aRect.Bottom() - aRect.Top() );
                rSet.Put( SvxSizeItem( SID_ATTR_SIZE, aSize ) );
            }
            else                                // mouse position
            {
                // aPos is initialized above
                pPV->LogicToPagePos(aPos);
                rSet.Put( SfxPointItem( SID_ATTR_POSITION, aPos ) );
                rSet.Put( SvxSizeItem( SID_ATTR_SIZE, Size( 0, 0 ) ) );
            }
        }
    }
}

void ScDrawShell::GetAttrFuncState(SfxItemSet &rSet)
{
    //  Dialoge fuer Draw-Attribute disablen, wenn noetig

    ScDrawView* pDrView = pViewData->GetScDrawView();
    SfxItemSet aViewSet = pDrView->GetAttrFromMarked(FALSE);

    if ( aViewSet.GetItemState( XATTR_LINESTYLE ) == SFX_ITEM_DEFAULT )
    {
        rSet.DisableItem( SID_ATTRIBUTES_LINE );
        rSet.DisableItem( SID_ATTR_LINEEND_STYLE );     // Tbx-Controller
    }

    if ( aViewSet.GetItemState( XATTR_FILLSTYLE ) == SFX_ITEM_DEFAULT )
        rSet.DisableItem( SID_ATTRIBUTES_AREA );
}

BOOL ScDrawShell::AreAllObjectsOnLayer(USHORT nLayerNo,const SdrMarkList& rMark)
{
    BOOL bResult=TRUE;
    ULONG nCount = rMark.GetMarkCount();
    for (ULONG i=0; i<nCount; i++)
    {
        SdrObject* pObj = rMark.GetMark(i)->GetObj();
        if ( !pObj->ISA(SdrUnoObj) )
        {
            if(nLayerNo!=pObj->GetLayer())
            {
                bResult=FALSE;
                break;
            }
        }
    }
    return bResult;
}


