/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mnuitem.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 15:57:41 $
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
#include "precompiled_sfx2.hxx"

#ifdef SOLARIS
// HACK: prevent conflict between STLPORT and Workshop headers on Solaris 8
#include <ctime>
#endif

#include <string> // HACK: prevent conflict between STLPORT and Workshop includes

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SVTOOLS_IMAGEMGR_HXX
#include <svtools/imagemgr.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MENUOPTIONS_HXX
#include <svtools/menuoptions.hxx>
#endif
#include <framework/menuconfiguration.hxx>
#ifndef GCC
#endif

#include "app.hxx"
#include "sfx.hrc"
#include "msgpool.hxx"
#include "msg.hxx"
#include "bindings.hxx"
#include "dispatch.hxx"
#include "idpool.hxx"
#include "sfxtypes.hxx"
#include "macrconf.hxx"
#include "virtmenu.hxx"
#include "mnuitem.hxx"
#include "tbxctrl.hxx"
#include "arrdecl.hxx"
#include "module.hxx"
#include "unoctitm.hxx"
#include "viewfrm.hxx"
#include "imgmgr.hxx"
#include "imagemgr.hxx"
#include "sfxresid.hxx"
#include "../doc/doc.hrc"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::util;

//====================================================================

class SfxEnumMenu: public PopupMenu
{
    USHORT          nSlot;
    SfxEnumItem    *pItem;
    SfxBindings*    pBindings;

protected:
    virtual void    Select();

public:
                    SfxEnumMenu( USHORT nSlot, SfxBindings* pBind, const SfxEnumItem &rItem );
                    ~SfxEnumMenu();
};

//=========================================================================

SfxEnumMenu::SfxEnumMenu( USHORT nSlotId, SfxBindings* pBind, const SfxEnumItem &rItem ):
    nSlot( nSlotId ),
    pItem( (SfxEnumItem*) rItem.Clone() ),
    pBindings( pBind )
{
    for ( USHORT nVal = 0; nVal < pItem->GetValueCount(); ++nVal )
        InsertItem( nVal+1, pItem->GetValueTextByPos(nVal) );
    CheckItem( pItem->GetValue() + 1, TRUE );
}

//-------------------------------------------------------------------------

SfxEnumMenu::~SfxEnumMenu()
{
    delete pItem;
}

//-------------------------------------------------------------------------

void SfxEnumMenu::Select()
{
    pItem->SetValue( GetCurItemId()-1 );
    pBindings->GetDispatcher()->Execute( nSlot,
                SFX_CALLMODE_ASYNCHRON|SFX_CALLMODE_RECORD,
                pItem, 0L, 0L );
}

//--------------------------------------------------------------------

void SfxMenuControl::SetOwnMenu( SfxVirtualMenu* pMenu )
{
    pOwnMenu = pMenu;
    if ( pSubMenu )
        pSubMenu->SetParentMenu( pMenu );
}


//--------------------------------------------------------------------

// binds the instance to the specified id and assignes the title

void SfxMenuControl::Bind(
    SfxVirtualMenu* pOwn,
    USHORT nSlotId,
    const String& rTitle,
    const String &rHelpText,
    SfxBindings &rBindings )
{
    DBG_MEMTEST();

    aTitle = rTitle;
    aHelpText = rHelpText;
    pOwnMenu = pOwn;
    pSubMenu = 0;
    if ( pOwn )
        SfxControllerItem::Bind(nSlotId, &rBindings);
    else
        SetId( nSlotId );

    DBG( CheckConfigure_Impl(SFX_SLOT_MENUCONFIG) );
}


//--------------------------------------------------------------------

// binds the item to the specified menu and assignes the title

void SfxMenuControl::Bind(
    SfxVirtualMenu* pOwn,
    USHORT nSlotId,
    SfxVirtualMenu& rMenu,
    const String& rTitle,
    const String &rHelpText,
    SfxBindings &rBindings )
{
    DBG_MEMTEST();
    SetId( nSlotId );
    SetBindings(rBindings);
    pOwnMenu = pOwn;
    pSubMenu = &rMenu;
    aTitle = rTitle;
    aHelpText = rHelpText;
}

//--------------------------------------------------------------------

// ctor for explicit registration

SfxMenuControl::SfxMenuControl( BOOL bShowStrings )
:   pOwnMenu(0),
    pSubMenu(0),
    b_ShowStrings(bShowStrings)
{
    DBG_MEMTEST();
}

//--------------------------------------------------------------------

// ctor for array

SfxMenuControl::SfxMenuControl():
    pOwnMenu(0),
    pSubMenu(0),
    b_ShowStrings(FALSE)
{
    DBG_MEMTEST();
}

//--------------------------------------------------------------------

SfxMenuControl::SfxMenuControl(USHORT nSlotId, SfxBindings& rBindings):
    SfxControllerItem(nSlotId, rBindings),
    pOwnMenu(0),
    pSubMenu(0),
    b_ShowStrings(FALSE)
{
    DBG_MEMTEST();

    // Dieser Ctor soll es erm"oglichen, w"ahrend der Konstruktion schon
    // auf die Bindings zur"uckgreifen zu k"onnen, aber gebunden wird
    // wie immer erst sp"ater. Anwendung z.B. wenn im ctor der abgeleiteten
    // Klasse z.B. ein StatusForwarder erzeugt werden soll.
    UnBind();
}


//--------------------------------------------------------------------

// dtor

SfxMenuControl::~SfxMenuControl()
{
    if ( SfxMacroConfig::IsMacroSlot( GetId() ) )
        SFX_APP()->GetMacroConfig()->ReleaseSlotId(GetId());
    delete pSubMenu;
}

void SfxMenuControl::RemovePopup()
{
    DELETEZ( pSubMenu );
}

//--------------------------------------------------------------------

// changes the state in the virtual menu

void SfxMenuControl::StateChanged
(
    USHORT              nSID,
    SfxItemState        eState,
    const SfxPoolItem*  pState
)
{
    (void)nSID; //unused
    DBG_MEMTEST();
    DBG_ASSERT( nSID == GetId(), "strange SID" );
    DBG_ASSERT( pOwnMenu != 0, "setting state to dangling SfxMenuControl" );

    FASTBOOL bIsObjMenu =
                GetId() >= SID_OBJECTMENU0 && GetId() < SID_OBJECTMENU_LAST;

    // enabled/disabled-Flag pauschal korrigieren
    pOwnMenu->EnableItem( GetId(), bIsObjMenu
                ? 0 != pOwnMenu->GetSVMenu()->GetPopupMenu( GetId() )
                : eState != SFX_ITEM_DISABLED );

    if ( eState != SFX_ITEM_AVAILABLE )
    {
        // checken nur bei nicht-Object-Menus
        if ( !bIsObjMenu )
            pOwnMenu->CheckItem( GetId(), FALSE );

        // SetItemText flackert in MenuBar insbes. unter OS/2 (Bug #20658)
        if ( // !bIsObjMenu && nicht wegen "Format/Datenbank"
             pOwnMenu->GetSVMenu()->GetItemText( GetId() ) != GetTitle() )
            pOwnMenu->SetItemText( GetId(), GetTitle() );

        return;
    }

    // ggf. das alte Enum-Menu entfernen/loeschen
    //! delete pOwnMenu->GetMenu().ChangePopupMenu( GetId(), 0 );

    bool bCheck = false;
    if ( pState->ISA(SfxBoolItem) )
    {
        // BoolItem fuer checken
        DBG_ASSERT( GetId() < SID_OBJECTMENU0 || GetId() > SID_OBJECTMENU_LAST,
                    "SfxBoolItem not allowed for SID_OBJECTMENUx" );
        bCheck = ((const SfxBoolItem*)pState)->GetValue();
    }
    else if ( pState->ISA(SfxEnumItemInterface) &&
              ((SfxEnumItemInterface *)pState)->HasBoolValue() )
    {
        // EnumItem wie Bool behandeln
        DBG_ASSERT( GetId() < SID_OBJECTMENU0 || GetId() > SID_OBJECTMENU_LAST,
                    "SfxEnumItem not allowed for SID_OBJECTMENUx" );
        bCheck = ((SfxEnumItemInterface *)pState)->GetBoolValue();
    }
    else if ( ( b_ShowStrings || bIsObjMenu ) && pState->ISA(SfxStringItem) )
    {
        // MenuText aus SfxStringItem holen
        String aStr( ((const SfxStringItem*)pState)->GetValue() );
        if ( aStr.CompareToAscii("($1)",4) == COMPARE_EQUAL )
        {
            String aEntry(SfxResId(STR_UPDATEDOC));
            aEntry += ' ';
            aEntry += aStr.Copy(4);
            aStr = aEntry;
        }
        else if ( aStr.CompareToAscii("($2)",4) == COMPARE_EQUAL )
        {
            String aEntry(SfxResId(STR_CLOSEDOC_ANDRETURN));
            aEntry += aStr.Copy(4);
            aStr = aEntry;
        }

        pOwnMenu->SetItemText( GetId(), aStr );
    }

#ifdef enum_item_menu_ok
    else if ( aType == TYPE(SfxEnumItem) )
    {
        DBG_ASSERT( GetId() < SID_OBJECTMENU0 || GetId() > SID_OBJECTMENU_LAST,
                    "SfxEnumItem not allowed for SID_OBJECTMENUx" );
        pOwnMenu->GetMenu().ChangePopupMenu( GetId(), &GetBindings(),
            new SfxEnumMenu( GetId(), *(const SfxEnumItem*)pState ) );
    }
#endif

    pOwnMenu->CheckItem( GetId(), bCheck );
}

//--------------------------------------------------------------------

SfxMenuControl* SfxMenuControl::CreateImpl( USHORT /*nId*/, Menu& /*rMenu*/, SfxBindings& /*rBindings*/ )
{
    return new SfxMenuControl( TRUE );
}

//--------------------------------------------------------------------

void SfxMenuControl::RegisterControl( USHORT nSlotId, SfxModule *pMod )
{
    RegisterMenuControl( pMod, new SfxMenuCtrlFactory(
                SfxMenuControl::CreateImpl, TYPE(SfxStringItem), nSlotId ) );
}

//--------------------------------------------------------------------
void SfxMenuControl::RegisterMenuControl(SfxModule* pMod, SfxMenuCtrlFactory* pFact)
{
    SFX_APP()->RegisterMenuControl_Impl( pMod, pFact );
}

SfxMenuControl* SfxMenuControl::CreateControl( USHORT nId, Menu &rMenu, SfxBindings &rBindings )
{
    TypeId aSlotType = SFX_SLOTPOOL().GetSlotType(nId);
    if ( aSlotType )
    {
        SfxApplication *pApp = SFX_APP();
        SfxDispatcher *pDisp = rBindings.GetDispatcher_Impl();
        SfxModule *pMod = pDisp ? SfxModule::GetActiveModule( pDisp->GetFrame() ) :0;
        if ( pMod )
        {
            SfxMenuCtrlFactArr_Impl *pFactories = pMod->GetMenuCtrlFactories_Impl();
            if ( pFactories )
            {
                SfxMenuCtrlFactArr_Impl &rFactories = *pFactories;
                for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
                    if ( rFactories[nFactory]->nTypeId == aSlotType &&
                         ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                           ( rFactories[nFactory]->nSlotId == nId) ) )
                        return rFactories[nFactory]->pCtor( nId, rMenu, rBindings );
            }
        }

        SfxMenuCtrlFactArr_Impl &rFactories = pApp->GetMenuCtrlFactories_Impl();

        for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
            if ( rFactories[nFactory]->nTypeId == aSlotType &&
                 ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                   ( rFactories[nFactory]->nSlotId == nId) ) )
                return rFactories[nFactory]->pCtor( nId, rMenu, rBindings );
    }
    return 0;
}



#if SUPD>633
BOOL SfxMenuControl::IsSpecialControl( USHORT nId, SfxModule* pMod  )
{
    TypeId aSlotType = SFX_SLOTPOOL().GetSlotType( nId );
    if ( aSlotType )
    {
        if ( pMod )
        {
            SfxMenuCtrlFactArr_Impl *pFactories = pMod->GetMenuCtrlFactories_Impl();
            if ( pFactories )
            {
                SfxMenuCtrlFactArr_Impl &rFactories = *pFactories;
                for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
                    if ( rFactories[nFactory]->nTypeId == aSlotType &&
                         ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                           ( rFactories[nFactory]->nSlotId == nId) ) )
                        return TRUE;
            }
        }

        SfxMenuCtrlFactArr_Impl &rFactories = SFX_APP()->GetMenuCtrlFactories_Impl();

        for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
            if ( rFactories[nFactory]->nTypeId == aSlotType &&
                 ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                   ( rFactories[nFactory]->nSlotId == nId) ) )
                return TRUE;
    }
    return 0;
}
#else
BOOL SfxMenuControl::IsSpecialControl( USHORT nId, SfxBindings& rBindings  )
{
    TypeId aSlotType = SFX_SLOTPOOL().GetSlotType( nId );
    if ( aSlotType )
    {
        SfxApplication *pApp = SFX_APP();
        SfxDispatcher *pDisp = rBindings.GetDispatcher_Impl();
        SfxModule *pMod = pDisp ? pApp->GetActiveModule( pDisp->GetFrame() ) :0;
        if ( pMod )
        {
            SfxMenuCtrlFactArr_Impl *pFactories = pMod->GetMenuCtrlFactories_Impl();
            if ( pFactories )
            {
                SfxMenuCtrlFactArr_Impl &rFactories = *pFactories;
                for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
                    if ( rFactories[nFactory]->nTypeId == aSlotType &&
                         ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                           ( rFactories[nFactory]->nSlotId == nId) ) )
                        return TRUE;
            }
        }

        SfxMenuCtrlFactArr_Impl &rFactories = pApp->GetMenuCtrlFactories_Impl();

        for ( USHORT nFactory = 0; nFactory < rFactories.Count(); ++nFactory )
            if ( rFactories[nFactory]->nTypeId == aSlotType &&
                 ( ( rFactories[nFactory]->nSlotId == 0 ) ||
                   ( rFactories[nFactory]->nSlotId == nId) ) )
                return TRUE;
    }
    return 0;
}
#endif

//--------------------------------------------------------------------

PopupMenu* SfxMenuControl::GetPopup () const
{
    if (GetPopupMenu())
        return (PopupMenu*)GetPopupMenu()->GetSVMenu();
    else
        return 0;
}

long Select_Impl( void* pHdl, void* pVoid );

SFX_IMPL_MENU_CONTROL( SfxAppMenuControl_Impl, SfxStringItem );

SfxAppMenuControl_Impl::SfxAppMenuControl_Impl(
    USHORT nPos, Menu& rMenu, SfxBindings& rBindings )
    : SfxMenuControl( nPos, rBindings ), pMenu(0)
{
    String aText = rMenu.GetItemText( nPos );

    // Determine the current background color setting for menus
    const StyleSettings& rSettings = Application::GetSettings().GetStyleSettings();
    BOOL bIsHiContrastMode  = rSettings.GetMenuColor().IsDark();
    m_bWasHiContrastMode    = bIsHiContrastMode;
    m_bShowMenuImages       = SvtMenuOptions().IsMenuIconsEnabled();

    Reference<com::sun::star::lang::XMultiServiceFactory> aXMultiServiceFactory(::comphelper::getProcessServiceFactory());
    ::framework::MenuConfiguration aConf( aXMultiServiceFactory );
    Reference<com::sun::star::frame::XFrame> aXFrame( GetBindings().GetDispatcher_Impl()->GetFrame()->GetFrame()->GetFrameInterface() );
    pMenu = aConf.CreateBookmarkMenu( aXFrame, GetId() == SID_NEWDOCDIRECT ? BOOKMARK_NEWMENU : BOOKMARK_WIZARDMENU );
    if( pMenu )
    {
        pMenu->SetSelectHdl( Link( &(this->GetBindings()), Select_Impl ) );
        pMenu->SetActivateHdl( LINK(this, SfxAppMenuControl_Impl, Activate) );
        rMenu.SetPopupMenu( nPos, pMenu );
    }
}

SfxAppMenuControl_Impl::~SfxAppMenuControl_Impl()
{
    delete pMenu;
}

IMPL_LINK( SfxAppMenuControl_Impl, Activate, Menu *, pActMenu )
{
    if ( pActMenu )
    {
        BOOL bShowMenuImages = SvtMenuOptions().IsMenuIconsEnabled();
        const StyleSettings& rSettings = Application::GetSettings().GetStyleSettings();
        BOOL bIsHiContrastMode = rSettings.GetMenuColor().IsDark();

        if (( bIsHiContrastMode != m_bWasHiContrastMode ) ||
            ( bShowMenuImages != m_bShowMenuImages      )    )
        {
            m_bWasHiContrastMode    = bIsHiContrastMode;
            m_bShowMenuImages       = bShowMenuImages;

            USHORT nCount = pActMenu->GetItemCount();
            for ( USHORT nSVPos = 0; nSVPos < nCount; nSVPos++ )
            {
                USHORT nItemId = pActMenu->GetItemId( nSVPos );
                if ( pActMenu->GetItemType( nSVPos ) != MENUITEM_SEPARATOR )
                {
                    if ( bShowMenuImages )
                    {
                        sal_Bool        bImageSet = sal_False;
                        ::rtl::OUString aImageId;
                        ::framework::MenuConfiguration::Attributes* pMenuAttributes =
                            (::framework::MenuConfiguration::Attributes*)pMenu->GetUserValue( nItemId );

                        if ( pMenuAttributes )
                            aImageId = pMenuAttributes->aImageId; // Retrieve image id from menu attributes

                        if ( aImageId.getLength() > 0 )
                        {
                            Reference< ::com::sun::star::frame::XFrame > xFrame;
                            Image aImage = GetImage( xFrame, aImageId, FALSE, bIsHiContrastMode );
                            if ( !!aImage )
                            {
                                bImageSet = sal_True;
                                pActMenu->SetItemImage( nItemId, aImage );
                            }
                        }

                        String aCmd( pActMenu->GetItemCommand( nItemId ) );
                        if ( !bImageSet && aCmd.Len() )
                        {
                            Image aImage = SvFileInformationManager::GetImage(
                                INetURLObject(aCmd), FALSE, bIsHiContrastMode );
                            if ( !!aImage )
                                pActMenu->SetItemImage( nItemId, aImage );
                        }
                    }
                    else
                        pActMenu->SetItemImage( nItemId, Image() );
                }
            }
        }

        return TRUE;
    }

    return FALSE;
}

SfxUnoMenuControl* SfxMenuControl::CreateControl( const String& rCmd,
        USHORT nId, Menu& rMenu, SfxBindings &rBindings, SfxVirtualMenu* pVirt )
{
    return new SfxUnoMenuControl( rCmd, nId, rMenu, rBindings, pVirt );
}

SfxUnoMenuControl::SfxUnoMenuControl( const String& rCmd, USHORT nSlotId,
    Menu& rMenu, SfxBindings& rBindings, SfxVirtualMenu* pVirt )
    : SfxMenuControl( nSlotId, rBindings )
{
    Bind( pVirt, nSlotId, rMenu.GetItemText(nSlotId),
                        rMenu.GetHelpText(nSlotId), rBindings);
    UnBind();
    pUnoCtrl = new SfxUnoControllerItem( this, rBindings, rCmd );
    pUnoCtrl->acquire();
    pUnoCtrl->GetNewDispatch();
}

SfxUnoMenuControl::~SfxUnoMenuControl()
{
    pUnoCtrl->UnBind();
    pUnoCtrl->release();
}

void SfxUnoMenuControl::Select()
{
    pUnoCtrl->Execute();
}
