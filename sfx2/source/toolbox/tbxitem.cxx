/*************************************************************************
 *
 *  $RCSfile: tbxitem.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: cd $ $Date: 2002-04-19 07:25:58 $
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

#include <string>           // prevent conflict with STL includes

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _URLBMK_HXX //autogen
#include <svtools/urlbmk.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX
#include <vcl/toolbox.hxx>
#endif

#include <svtools/imagemgr.hxx>
#include <comphelper/processfactory.hxx>
#include <framework/menuconfiguration.hxx>
#include <vcl/taskpanelist.hxx>

#ifndef INCLUDED_SVTOOLS_MENUOPTIONS_HXX
#include <svtools/menuoptions.hxx>
#endif

#pragma hdrstop

#include "tbxctrl.hxx"
#include "mnumgr.hxx"
#include "stbmgr.hxx"
#include "dispatch.hxx"
#include "fltfnc.hxx"
#include "msg.hxx"
#include "msgpool.hxx"
#include "statcach.hxx"
#include "viewfrm.hxx"
#include "arrdecl.hxx"
#include "sfxtypes.hxx"
#include "macrconf.hxx"
#include "genlink.hxx"
#include "sfxresid.hxx"
#include "toolbox.hrc"
#include "sfx.hrc"
#include "module.hxx"
#include "appdata.hxx"
#include "docfile.hxx"
#include "docfac.hxx"
#include "referers.hxx"
#include "frmhtmlw.hxx"
#include "app.hxx"
#include "unoctitm.hxx"
#include "helpid.hrc"
#include "imagemgr.hxx"
#include "workwin.hxx"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;

//--------------------------------------------------------------------

static USHORT nLastId_Impl = SID_NEWDOCDIRECT;

//====================================================================

SFX_IMPL_TOOLBOX_CONTROL_ARG(SfxToolBoxControl, SfxStringItem, TRUE);
SFX_IMPL_TOOLBOX_CONTROL(SfxAppToolBoxControl_Impl, SfxStringItem);
SFX_IMPL_TOOLBOX_CONTROL(SfxDragToolBoxControl_Impl, SfxStringItem);
SFX_IMPL_TOOLBOX_CONTROL(SfxHistoryToolBoxControl_Impl, SfxStringItem);
SFX_IMPL_TOOLBOX_CONTROL(SfxReloadToolBoxControl_Impl, SfxBoolItem);

//--------------------------------------------------------------------

SfxToolBoxControl::SfxToolBoxControl
(
    USHORT          nId,
    ToolBox&        rBox,
    SfxBindings&    rBindings,
    BOOL            bShowStringItems
)
:   SfxControllerItem(nId, rBindings),
    pBox(&rBox),
    bShowString(bShowStringItems),
    pFact(0)
{
    nSelectModifier = 0;
    DBG( CheckConfigure_Impl(SFX_SLOT_TOOLBOXCONFIG) );
}

//--------------------------------------------------------------------

SfxToolBoxControl::~SfxToolBoxControl()
{
    if ( SfxMacroConfig::IsMacroSlot( GetId() ) )
        SFX_APP()->GetMacroConfig()->ReleaseSlotId(GetId());
}

//--------------------------------------------------------------------

SfxToolBoxControl* SfxToolBoxControl::CreateControl( USHORT nId,
        ToolBox *pBox, SfxBindings &rBindings, SfxModule *pMod )
{
    SfxToolBoxControl *pCtrl;
    SfxApplication *pApp = SFX_APP();
    if ( !pMod )
    {
        SfxDispatcher *pDisp = rBindings.GetDispatcher_Impl();
        pMod = pDisp ? pApp->GetActiveModule( pDisp->GetFrame() ) :0;
    }

    SfxSlotPool *pSlotPool;
    if ( pMod )
        pSlotPool = pMod->GetSlotPool();
    else
        pSlotPool = &pApp->GetSlotPool();
    TypeId aSlotType = pSlotPool->GetSlotType(nId);
    if ( aSlotType )
    {
        if ( pMod )
        {
            SfxTbxCtrlFactArr_Impl *pFactories = pMod->GetTbxCtrlFactories_Impl();
            if ( pFactories )
            {
                SfxTbxCtrlFactArr_Impl &rFactories = *pFactories;
                for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
                    if ( rFactories[nFactory]->nTypeId == aSlotType &&
                         ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                           ( rFactories[nFactory]->nSlotId == nId) ) )
                    {
                        pCtrl = rFactories[nFactory]->pCtor( nId, *pBox, rBindings );
                        pCtrl->pFact = rFactories[nFactory];
                        return pCtrl;
                    }
            }
        }

        SfxTbxCtrlFactArr_Impl &rFactories = pApp->GetTbxCtrlFactories_Impl();

        for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
            if ( rFactories[nFactory]->nTypeId == aSlotType &&
                 ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                   ( rFactories[nFactory]->nSlotId == nId) ) )
            {
                pCtrl = rFactories[nFactory]->pCtor( nId, *pBox, rBindings );
                pCtrl->pFact = rFactories[nFactory];
                return pCtrl;
            }
    }

    return new SfxToolBoxControl( nId, *pBox, rBindings );
}

//--------------------------------------------------------------------

void SfxToolBoxControl::StateChanged
(
    USHORT              nId,
    SfxItemState        eState,
    const SfxPoolItem*  pState
)
{
    DBG_MEMTEST();
    DBG_ASSERT( GetId() == nId, "strange SID" );
    DBG_ASSERT( pBox != 0, "setting state to dangling ToolBox" );

    if ( GetId() >= SID_OBJECTMENU0 && GetId() <= SID_OBJECTMENU_LAST )
        return;

    // enabled/disabled-Flag pauschal korrigieren
    pBox->EnableItem( GetId(), eState != SFX_ITEM_DISABLED );

    TriState eTri = STATE_NOCHECK;
    switch ( eState )
    {
        case SFX_ITEM_AVAILABLE:
        {
            if ( pState->ISA(SfxBoolItem) )
            {
                // BoolItem fuer checken
                if ( ((const SfxBoolItem*)pState)->GetValue() )
                    eTri = STATE_CHECK;
            }
            else if ( pState->ISA(SfxEnumItemInterface) &&
                ((SfxEnumItemInterface *)pState)->HasBoolValue())
            {
                // EnumItem wie Bool behandeln
                if ( ((const SfxEnumItemInterface *)pState)->GetBoolValue() )
                    eTri = STATE_CHECK;
            }
            else if ( bShowString && pState->ISA(SfxStringItem) )
                pBox->SetItemText(nId, ((const SfxStringItem*)pState)->GetValue() );
            break;
        }

        case SFX_ITEM_DONTCARE:
            eTri = STATE_DONTKNOW;
    }

    pBox->SetItemState( GetId(), eTri );
}

//--------------------------------------------------------------------

void SfxToolBoxControl::Select( USHORT nModifier )
{
    nSelectModifier = nModifier;
    Select( BOOL((nModifier & KEY_MOD1)!=0) );
}

//--------------------------------------------------------------------

void SfxToolBoxControl::Select( BOOL bMod1 )
{
    GetBindings().Execute( GetId(), 0, nSelectModifier );
}

//--------------------------------------------------------------------

void SfxToolBoxControl::DoubleClick()
{
}

//--------------------------------------------------------------------

void SfxToolBoxControl::Click()
{
}

//--------------------------------------------------------------------

SfxPopupWindowType SfxToolBoxControl::GetPopupWindowType() const
{
    return SFX_POPUPWINDOW_NONE;
}

//--------------------------------------------------------------------

SfxPopupWindow* SfxToolBoxControl::CreatePopupWindow()
{
    if ( GetId() >= SID_OBJECTMENU0 && GetId() <= SID_OBJECTMENU_LAST )
    {
        USHORT nConfigId;
        PopupMenu *pPopup = SFX_APP()->GetMenuBarManager()->
            GetObjectMenu( GetId(), nConfigId );
        if ( pPopup )
        {
            Rectangle aItemRect( GetToolBox().GetItemRect(GetId()) );

            Point aPos;
            switch ( GetToolBox().GetAlign() )
            {
                case WINDOWALIGN_TOP:
                    aPos = aItemRect.BottomLeft();
                    break;

                case WINDOWALIGN_BOTTOM:
                    aPos = aItemRect.TopLeft();
                    break;

                case WINDOWALIGN_LEFT:
                    aPos = aItemRect.TopRight();
                    break;

                case WINDOWALIGN_RIGHT:
                    aPos = aItemRect.TopLeft();
                    break;
            }

            pPopup->Execute( pBox, aPos );
        }
    }

    return 0;
}

SfxPopupWindow* SfxToolBoxControl::CreatePopupWindowCascading()
{
    return 0;
}

//--------------------------------------------------------------------

Window* SfxToolBoxControl::CreateItemWindow( Window * )
{
    return 0;
}

SfxToolBoxControl* SfxToolBoxControl::ReInit_Impl()
{
    USHORT nId = GetId();
    SfxBindings& rBindings = GetBindings();
    ToolBox *pBox = &GetToolBox();

    // Test, welche ToolboxController-Factory im aktuellen Kontext einen
    // Controller mit der Id dieses Controllers erzeugen k"onnte
    SfxToolBoxControl *pCtrl = this;
    SfxApplication *pApp = SFX_APP();
    SfxDispatcher *pDisp = rBindings.GetDispatcher_Impl();
    SfxModule *pMod = pDisp ? pApp->GetActiveModule( pDisp->GetFrame() ) :0;
    SfxSlotPool *pSlotPool;
    if ( pMod )
        pSlotPool = pMod->GetSlotPool();
    else
        pSlotPool = &pApp->GetSlotPool();
    TypeId aSlotType = pSlotPool->GetSlotType(nId);
    if ( aSlotType )
    {
        if ( pMod )
        {
            SfxTbxCtrlFactArr_Impl *pFactories = pMod->GetTbxCtrlFactories_Impl();
            if ( pFactories )
            {
                SfxTbxCtrlFactArr_Impl &rFactories = *pFactories;
                for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
                    if ( rFactories[nFactory]->nTypeId == aSlotType &&
                         ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                           ( rFactories[nFactory]->nSlotId == nId) ) )
                    {
                        if ( pFact != rFactories[nFactory] )
                        {
                            // Es wurde eine Factory gefunden, und es ist eine
                            // andere als die dieses Controllers, d.h. durch
                            // einen Contextwechsel wurde auch der Controller
                            // ge"andert
                            pCtrl = rFactories[nFactory]->pCtor( nId, *pBox, rBindings );
                            pCtrl->pFact = rFactories[nFactory];
                        }

                        // Die gefundene Factory ist die gleiche wie die dieses
                        // Controllers, d.h. es hat sich nichts ge"andert
                        return pCtrl;
                    }
            }
        }

        SfxTbxCtrlFactArr_Impl &rFactories = pApp->GetTbxCtrlFactories_Impl();

        for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
            if ( rFactories[nFactory]->nTypeId == aSlotType &&
                 ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                   ( rFactories[nFactory]->nSlotId == nId) ) )
            {
                if ( pFact != rFactories[nFactory] )
                {
                    // Es wurde eine Factory gefunden, und es ist eine
                    // andere als die dieses Controllers, d.h. durch
                    // einen Contextwechsel wurde auch der Controller
                    // ge"andert
                    pCtrl = rFactories[nFactory]->pCtor( nId, *pBox, rBindings );
                    pCtrl->pFact = rFactories[nFactory];
                }

                // Die gefundene Factory ist die gleiche wie die dieses
                // Controllers, d.h. es hat sich nichts ge"andert
                return pCtrl;
            }
    }

    // Wenn der aktuelle Controller eine Factory hat, aber aktuell kein gefunden
    // wird, mu\s dieser Controller durch einen StandardController ersetzt werden,
    return pFact ? new SfxToolBoxControl( nId, *pBox, rBindings ) : this;
}

//--------------------------------------------------------------------

BOOL SfxPopupWindow::Close()
{
    bFloating = FALSE;
    FloatingWindow::Close();

    if ( SFX_APP()->IsDowning() )
    {
        Delete(0);
    }
    else
    {
        UnBind();
        GetpApp()->PostUserEvent(LINK(this, SfxPopupWindow, Delete) , (void*)0);
    }

    return TRUE;
}

//--------------------------------------------------------------------

void SfxPopupWindow::PopupModeEnd()
{
    //! to allow PopupModeEndHdl to be called
    FloatingWindow::PopupModeEnd();

    if ( IsVisible() )
    {
        // wurde abgerissen
        GetBindings().GetStateCache(GetId())->DeleteFloatingWindows();
        bFloating = TRUE;
    }
    else
        Close();
}

//--------------------------------------------------------------------

void SfxPopupWindow::DeleteFloatingWindow()
{
    if ( bFloating )
    {
        HACK(dieses muss via Parameter durchgereicht werden)
        SfxPopupAction eAction = GetBindings().GetPopupAction_Impl();
        switch ( eAction )
        {
            case SFX_POPUP_DELETE:
                Hide();
                Delete(0);
                break;

            case SFX_POPUP_HIDE:
            {
                SfxStateCache* pCache = GetBindings().GetStateCache( GetId() );
                pCache->SetState(SFX_ITEM_DISABLED, 0);
                if ( GetParent() !=  SFX_APP()->GetTopWindow() )
                    SetParent( SFX_APP()->GetTopWindow() );
                break;
            }
            case SFX_POPUP_SHOW:
            {
                if ( GetParent() !=  SFX_APP()->GetTopWindow() )
                    SetParent( SFX_APP()->GetTopWindow() );

                // Achtung: k"onnte wegen seines Kontextes unsichtbar sein!
                SfxBindings &rBindings = GetBindings();
                rBindings.Invalidate( GetId() );
                rBindings.Update( GetId() );
                break;
            }
        }
    }
}

//--------------------------------------------------------------------

SfxPopupWindow::SfxPopupWindow( USHORT nId, WinBits nBits, SfxBindings &rBindings ):
    SfxControllerItem(nId, rBindings.GetSubBindings_Impl() ? *rBindings.GetSubBindings_Impl(TRUE) : rBindings ),
    FloatingWindow( SFX_APP()->GetTopWindow(), nBits ),
    bFloating(FALSE)
{
    bCascading = FALSE;
    rBindings.GetWorkWindow_Impl()->GetWindow()->GetSystemWindow()->GetTaskPaneList()->AddWindow( this );
}

//--------------------------------------------------------------------

SfxPopupWindow::SfxPopupWindow( USHORT nId, const ResId &rId, SfxBindings &rBindings ):
    SfxControllerItem(nId, rBindings.GetSubBindings_Impl() ? *rBindings.GetSubBindings_Impl(TRUE) : rBindings ),
    FloatingWindow( SFX_APP()->GetTopWindow(), rId ),
    bFloating(FALSE)
{
    bCascading = FALSE;
    rBindings.GetWorkWindow_Impl()->GetWindow()->GetSystemWindow()->GetTaskPaneList()->AddWindow( this );
}

SfxPopupWindow::~SfxPopupWindow()
{
    GetBindings().GetWorkWindow_Impl()->GetWindow()->GetSystemWindow()->GetTaskPaneList()->RemoveWindow( this );
}

void SfxPopupWindow::MouseMove( const MouseEvent& rMEvt )
{
    if ( bCascading == FALSE )
        FloatingWindow::MouseMove( rMEvt );
    else
    {
        // MouseMove-Event an die Children forwarden
        Point       aPos = rMEvt.GetPosPixel();
        Point       aScrPos = OutputToScreenPixel( aPos );
        int i = 0;
        Window* pWindow = GetChild( i );
        while ( pWindow )
        {
            MouseEvent aMEvt( pWindow->ScreenToOutputPixel( aScrPos ),
                              rMEvt.GetClicks(), rMEvt.GetMode(),
                              rMEvt.GetButtons(), rMEvt.GetModifier() );
            pWindow->MouseMove( rMEvt );
            pWindow->Update();
            i++;
            pWindow = GetChild( i );
        }
    }
}

void SfxPopupWindow::StartCascading_Impl()
{
    bCascading= TRUE;
}

void SfxPopupWindow::EndCascading_Impl()
{
    bCascading = FALSE;
}

//--------------------------------------------------------------------

SfxPopupWindow* SfxPopupWindow::Clone() const

/*  [Beschreibung]

    Diese Methode mu\s "uberladen werden, um dieses Popup auch im
    Presentations-Modus anzuzeigen. Sie wird gerufen, wenn ein Show()
    sinnlos w"are, da der Parent nicht das Presentations-Window ist.
    Beim neu erzeugen wird automatisch das neue Top-Window verwendet, so
    da\s der Parent das Presentations-Window ist und das neue Popup somit
    sichtbar ist.
*/

{
    return 0;
}

//--------------------------------------------------------------------

void SfxPopupWindow::StateChanged( USHORT nSID, SfxItemState eState,
                                   const SfxPoolItem* pState )
/*  [Bescheibung]

    Siehe auch <SfxControllerItem::StateChanged()>. Au\serdem wird
    bei eState==SFX_ITEM_DISABLED das Popup gehided und in allen anderen
    F"allen, falls es floating ist, wieder angezeigt. Daher mu\s die
    Basisklasse i.d.R. gerufen werden.

    Es findet wegen des Parents eine Sonderbehandlung f"ur den
    Presentationsmodus statt.

*/

{
    if ( SFX_ITEM_DISABLED == eState )
    {
        Hide();
    }
    else if ( bFloating )
    {
        Show();
    }
}

//--------------------------------------------------------------------

IMPL_LINK( SfxPopupWindow, Delete, void *, pvoid )
{
/*
    if ( pvoid )
    {
        SfxBindings& rBindings = GetBindings();
        rBindings.DENTERREGISTRATIONS();
        SfxPopupWindow *pNew = Clone();
        if ( pNew )
        {
            pNew->SetPosSizePixel( GetPosPixel(), GetSizePixel() );
            pNew->bFloating = TRUE;
            rBindings.DLEAVEREGISTRATIONS();
            rBindings.Invalidate( GetId() );
            rBindings.Update( GetId() );
        }
    }
*/
    if ( aDeleteLink.IsSet() )
        aDeleteLink.Call( this );
    delete this;
    return 0;
}

//--------------------------------------------------------------------

SfxAppToolBoxControl_Impl::SfxAppToolBoxControl_Impl
(
    USHORT nId, ToolBox& rBox, SfxBindings &rBindings
)
    : SfxToolBoxControl( nId, rBox, rBindings )
    , bBigImages( FALSE )
    , pMenu( 0 )
{
    aTimer.SetTimeout( 250 );
    aTimer.SetTimeoutHdl( LINK( this, SfxAppToolBoxControl_Impl, Timeout ) );
    rBox.SetHelpId( nId, HID_TBXCONTROL_FILENEW );
    rBox.SetItemBits( nId,  rBox.GetItemBits( nId ) | TIB_DROPDOWN);

    // Determine the current background color of the menus
    const StyleSettings& rSettings = Application::GetSettings().GetStyleSettings();
    m_bWasHiContrastMode    = rSettings.GetMenuColor().IsDark();
    m_bShowMenuImages       = SvtMenuOptions().IsMenuIconsEnabled();

    SetImage( String() );
}

SfxAppToolBoxControl_Impl::~SfxAppToolBoxControl_Impl()
{
    delete pMenu;
}

void SfxAppToolBoxControl_Impl::SetImage( const String &rURL )
{
    String aURL = rURL;
    if( !rURL.Len() )
    {
        aURL = DEFINE_CONST_UNICODE("private:factory/");
        aURL += String::CreateFromAscii(SfxObjectFactory::GetDefaultFactory().GetShortName());
    }

    GetToolBox().SetItemImage( GetId(),
                               SvFileInformationManager::GetImage( INetURLObject( aURL ),
                               FALSE,
                                GetToolBox().GetBackground().GetColor().IsDark() ) );
}

void SfxAppToolBoxControl_Impl::StateChanged
(
    USHORT              nId,
    SfxItemState        eState,
    const SfxPoolItem*  pState
)
{
    if ( !aLastURL.Len() && pState && pState->ISA(SfxStringItem) )
    {
        GetToolBox().EnableItem( GetId(), eState != SFX_ITEM_DISABLED );
        SetImage( ((const SfxStringItem*)pState)->GetValue() );
    }
    else
        SfxToolBoxControl::StateChanged( nId, eState, pState );
}

//--------------------------------------------------------------------

void SfxAppToolBoxControl_Impl::Select( BOOL bMod1 )
{
    aTimer.Stop();
    if( aLastURL.Len() )
    {
        if ( aLastURL.CompareToAscii( "slot:", 5 ) == COMPARE_EQUAL )
        {
            USHORT nId = (USHORT) aLastURL.Copy( 5 ).ToInt32();
            GetBindings().Execute( nId );
        }
        else
        {
            SfxStringItem aName( SID_FILE_NAME, aLastURL );
            SfxStringItem aReferer( SID_REFERER, DEFINE_CONST_UNICODE(SFX_REFERER_NEWMENU) );
            SfxBoolItem aTemplate( SID_TEMPLATE, TRUE );
            SfxStringItem aTarget( SID_TARGETNAME, String::CreateFromAscii("_blank") );

            const SfxPoolItem* aItems[5];
            aItems[4] = 0;
            aItems[0] = &aName;
            aItems[1] = &aReferer;
            aItems[2] = &aTemplate;
            aItems[3] = &aTarget;
            GetBindings().Execute( SID_OPENDOC, aItems, 0, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
        }
    }
    else
        SfxToolBoxControl::Select( bMod1 );
}

//--------------------------------------------------------------------
long Select_Impl( void* pHdl, void* pVoid );


IMPL_LINK( SfxAppToolBoxControl_Impl, Activate, Menu *, pMenu )
{
    if ( pMenu )
    {
        const StyleSettings& rSettings = Application::GetSettings().GetStyleSettings();
        BOOL bIsHiContrastMode  = rSettings.GetMenuColor().IsDark();
        BOOL bShowMenuImages    = SvtMenuOptions().IsMenuIconsEnabled();

        if (( bIsHiContrastMode != m_bWasHiContrastMode ) ||
            ( bShowMenuImages   != m_bShowMenuImages    )    )
        {
            m_bWasHiContrastMode = bIsHiContrastMode;
            m_bShowMenuImages    = bShowMenuImages;

            USHORT nCount = pMenu->GetItemCount();
            for ( USHORT nSVPos = 0; nSVPos < nCount; nSVPos++ )
            {
                USHORT nId = pMenu->GetItemId( nSVPos );
                if ( pMenu->GetItemType( nSVPos ) != MENUITEM_SEPARATOR )
                {
                    if ( bShowMenuImages )
                    {
                        sal_Bool        bImageSet = sal_False;
                        ::rtl::OUString aImageId;
                        ::framework::MenuConfiguration::Attributes* pMenuAttributes =
                            (::framework::MenuConfiguration::Attributes*)pMenu->GetUserValue( nId );

                        if ( pMenuAttributes )
                            aImageId = pMenuAttributes->aImageId; // Retrieve image id from menu attributes

                        if ( aImageId.getLength() > 0 )
                        {
                            Reference< ::com::sun::star::frame::XFrame > xFrame;
                            Image aImage = GetImage( xFrame, aImageId, FALSE, bIsHiContrastMode );
                            if ( !!aImage )
                            {
                                bImageSet = sal_True;
                                pMenu->SetItemImage( nId, aImage );
                            }
                        }

                        String aCmd( pMenu->GetItemCommand( nId ) );
                        if ( !bImageSet && aCmd.Len() )
                        {
                            Image aImage = SvFileInformationManager::GetImage( aCmd, FALSE, bIsHiContrastMode );
                            if ( !!aImage )
                                pMenu->SetItemImage( nId, aImage );
                        }
                    }
                    else
                        pMenu->SetItemImage( nId, Image() );
                }
            }
        }

        return TRUE;
    }

    return FALSE;
}

//--------------------------------------------------------------------

IMPL_LINK( SfxAppToolBoxControl_Impl, Timeout, Timer *, pTimer )
{
    SfxApplication* pApp = SFX_APP();
    ToolBox& rBox = GetToolBox();
    Rectangle aRect(  rBox.GetItemRect( GetId() ) );

    USHORT nId = GetId();
    BOOL bNew = FALSE;

    if ( !pMenu )
    {
        Reference <com::sun::star::lang::XMultiServiceFactory> aXMultiServiceFactory(::comphelper::getProcessServiceFactory());
        ::framework::MenuConfiguration aConf( aXMultiServiceFactory );
        Reference<com::sun::star::frame::XFrame> aXFrame( GetBindings().GetDispatcher_Impl()->GetFrame()->GetFrame()->GetFrameInterface() );
        pMenu = aConf.CreateBookmarkMenu( aXFrame, BOOKMARK_NEWMENU );
    }

    if( pMenu )
    {
        pMenu->SetSelectHdl( Link( this, Select_Impl ) );
        pMenu->SetActivateHdl( LINK( this, SfxAppToolBoxControl_Impl, Activate ));
        rBox.SetItemDown( GetId(), TRUE );
        USHORT nSelected = pMenu->Execute( &rBox, aRect, POPUPMENU_EXECUTE_UP );
        if ( nSelected )
        {
            aLastURL = pMenu->GetItemCommand( nSelected );
            SetImage( pMenu->GetItemCommand( nSelected ) );
        }

        rBox.SetItemDown( GetId(), FALSE );
    }

    return 0;
}

//--------------------------------------------------------------------

void SfxAppToolBoxControl_Impl::Click( )
{
    if ( GetId() == SID_NEWDOCDIRECT )
        aTimer.Start();
    else
        Timeout(0);
}

//--------------------------------------------------------------------

SfxDragToolBoxControl_Impl::SfxDragToolBoxControl_Impl
(
    USHORT nId, ToolBox& rBox, SfxBindings &rBindings
)
    :   SfxToolBoxControl( nId, rBox, rBindings )
{
}

Window* SfxDragToolBoxControl_Impl::CreateItemWindow( Window *pParent )
{
    return new SfxDragButton_Impl( pParent, GetBindings() );
}

void SfxDragToolBoxControl_Impl::Select( BOOL bMod1 )
{
    GetBindings().Execute( SID_CREATELINK, NULL, 0, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
}

SfxDragButton_Impl::SfxDragButton_Impl( Window *pParent, SfxBindings& rBind  )
    : FixedImage( pParent )
    , pBindings( &rBind )
{
    SetBackground( pParent->GetBackground() );
    Image aImage( SfxResId( RID_GREPIMAGE ) );
    SetImage( aImage );
    Size aSize( aImage.GetSizePixel() );
    aSize.Width() += 6;
    SetSizePixel( aSize );
}

void SfxDragButton_Impl::Command ( const CommandEvent& rCEvt )
{
    if( rCEvt.GetCommand() != COMMAND_STARTDRAG )
        return;
 /*
    SfxViewFrame *pViewFrame = SfxViewFrame::Current();
    if ( pViewFrame )
    {
        SfxFrame *pFrame = pBindings->GetDispatcher()->GetFrame()->GetFrame()->GetTopFrame();
        SfxObjectShell* pSh = pFrame->GetCurrentDocument();
        if( pSh && pSh->HasName() )
        {
            DragServer::Clear();
            INetBookmark aBmk(
                SfxFrameHTMLWriter::CreateURL( pFrame ),
                pSh->GetTitle( SFX_TITLE_CAPTION ) );
            aBmk.CopyDragServer();
            Pointer aMovePtr = Pointer( POINTER_MOVEDATA );
            Pointer aCopyPtr = Pointer( POINTER_COPYDATA );
            Pointer aLinkPtr = Pointer( POINTER_LINKDATA );
            ExecuteDrag( aMovePtr, aCopyPtr, aLinkPtr, DRAG_COPYABLE | DRAG_LINKABLE );
        }
    }
  */
}

void SfxDragButton_Impl::MouseMove( const MouseEvent& rEvt )
{
    SetPointer( Pointer( POINTER_HAND ) );
}

void SfxDragButton_Impl::MouseButtonDown( const MouseEvent& rEvt )
{
}

SfxHistoryToolBoxControl_Impl::SfxHistoryToolBoxControl_Impl( USHORT nId,
        ToolBox& rBox, SfxBindings &rBindings )
  : SfxToolBoxControl( nId, rBox, rBindings )
{
    aTimer.SetTimeout( 250 );
    aTimer.SetTimeoutHdl( LINK( this, SfxHistoryToolBoxControl_Impl, Timeout ) );
}

void SfxHistoryToolBoxControl_Impl::Select( BOOL bMod1 )
{
    aTimer.Stop();
    SfxToolBoxControl::Select( bMod1 );
}

IMPL_LINK( SfxHistoryToolBoxControl_Impl, Timeout, Timer *, pTimer )
{
    ToolBox& rBox = GetToolBox();

    rBox.SetItemDown( GetId(), TRUE );

    SfxApplication* pApp = SFX_APP();
    Rectangle aRect(  rBox.GetItemRect( GetId() ) );
    aRect.SetPos( rBox.OutputToScreenPixel( aRect.TopLeft() ) );

    SfxFrame *pTop = GetBindings().GetDispatcher()->GetFrame()->GetFrame()->GetTopFrame();
    if ( pTop->ExecuteHistoryMenu_Impl( GetId(), aRect, POPUPMENU_EXECUTE_UP ) )
        rBox.SetItemDown( GetId(), FALSE );
    return 0;
}

void SfxHistoryToolBoxControl_Impl::Click( )
{
    aTimer.Start();
    // SfxToolBoxControl::Click();
}

SfxReloadToolBoxControl_Impl::SfxReloadToolBoxControl_Impl( USHORT nId,
        ToolBox& rBox, SfxBindings &rBindings )
  : SfxToolBoxControl( nId, rBox, rBindings )
{
}

void SfxReloadToolBoxControl_Impl::Select( USHORT nModifier )
{
    SfxBoolItem aItem( SID_RELOAD, TRUE );
    const SfxPoolItem* aItems[2];
    aItems[1] = 0;
    aItems[0] = &aItem;
    GetBindings().Execute( GetId(),
            aItems, nModifier, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
}

SfxUnoToolBoxControl* SfxToolBoxControl::CreateControl( const String& rCmd,
        USHORT nId, ToolBox *pBox, SfxBindings &rBindings )
{
    return new SfxUnoToolBoxControl( rCmd, nId, pBox, rBindings );
}

SfxUnoToolBoxControl::SfxUnoToolBoxControl( const String& rCmd, USHORT nId,
    ToolBox *pBox, SfxBindings& rBindings )
    : SfxToolBoxControl( nId, *pBox, rBindings )
{
    UnBind();
    pUnoCtrl = new SfxUnoControllerItem( this, rBindings, rCmd );
    pUnoCtrl->acquire();
    pUnoCtrl->GetNewDispatch();
}

SfxUnoToolBoxControl::~SfxUnoToolBoxControl()
{
    pUnoCtrl->UnBind();
    pUnoCtrl->release();
}

void SfxUnoToolBoxControl::Select( BOOL bMod1 )
{
    pUnoCtrl->Execute();
}


