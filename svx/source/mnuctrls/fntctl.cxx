/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fntctl.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 18:32:27 $
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

#include <string> // HACK: prevent conflict between STLPORT and Workshop headern

#ifndef _STDMENU_HXX //autogen
#include <svtools/stdmenu.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif

#include <svx/fntctl.hxx>       //
#include <svx/svxids.hrc>

#ifndef _SVX_FLSTITEM_HXX //autogen
#include "flstitem.hxx"
#endif
#ifndef _SVX_FONTITEM_HXX //autogen
#include "fontitem.hxx"
#endif

// STATIC DATA -----------------------------------------------------------

SFX_IMPL_MENU_CONTROL(SvxFontMenuControl, SvxFontItem);

//--------------------------------------------------------------------

/*  [Beschreibung]

    Ctor; setzt den Select-Handler am Men"u und tr"agt das Men"u
    in seinen Parent ein.
*/

SvxFontMenuControl::SvxFontMenuControl
(
    USHORT          _nId,
    Menu&           rMenu,
    SfxBindings&    rBindings
) :
    pMenu   ( new FontNameMenu ),
    rParent ( rMenu )
{
    rMenu.SetPopupMenu( _nId, pMenu );
    pMenu->SetSelectHdl( LINK( this, SvxFontMenuControl, MenuSelect ) );
    StartListening( rBindings );
    FillMenu();
}

//--------------------------------------------------------------------

/*  [Beschreibung]

    F"ullt das Men"u mit den aktuellen Fonts aus der Fontlist
    der DocumentShell.
*/

void SvxFontMenuControl::FillMenu()
{
    SfxObjectShell *pDoc = SfxObjectShell::Current();

    if ( pDoc )
    {
        const SvxFontListItem* pFonts =
            (const SvxFontListItem*)pDoc->GetItem( SID_ATTR_CHAR_FONTLIST );
        const FontList* pList = pFonts ? pFonts->GetFontList(): 0;
        DBG_ASSERT( pList, "Kein Fonts gefunden" );
        pMenu->Fill( pList );
    }
}

//--------------------------------------------------------------------

/*  [Beschreibung]

    Statusbenachrichtigung;
    f"ullt ggf. das Men"u mit den aktuellen Fonts aus der Fontlist
    der DocumentShell.
    Ist die Funktionalit"at disabled, wird der entsprechende
    Men"ueintrag im Parentmen"u disabled, andernfalls wird er enabled.
    Der aktuelle Font wird mit einer Checkmark versehen.
*/

void SvxFontMenuControl::StateChanged(

    USHORT, SfxItemState eState, const SfxPoolItem* pState )

{
    rParent.EnableItem( GetId(), SFX_ITEM_DISABLED != eState );

    if ( SFX_ITEM_AVAILABLE == eState )
    {
        if ( !pMenu->GetItemCount() )
            FillMenu();
        const SvxFontItem* pFontItem = PTR_CAST( SvxFontItem, pState );
        String aFont;

        if ( pFontItem )
            aFont = pFontItem->GetFamilyName();
        pMenu->SetCurName( aFont );
    }
}

//--------------------------------------------------------------------

/*  [Beschreibung]

    Statusbenachrichtigung "uber Bindings;  bei DOCCHANGED
    wird das Men"u mit den aktuellen Fonts aus der Fontlist
    der DocumentShell gef"ullt.
*/

void SvxFontMenuControl::SFX_NOTIFY( SfxBroadcaster&, const TypeId& rBCType,
                                 const SfxHint& rHint, const TypeId& rHintType )
{
    if ( rHint.Type() != TYPE(SfxSimpleHint) &&
         ( (SfxSimpleHint&)rHint ).GetId() == SFX_HINT_DOCCHANGED )
        FillMenu();
}

//--------------------------------------------------------------------

/*  [Beschreibung]

    Select-Handler des Men"us; der Name des selektierten Fonts
    wird in einem SvxFontItem verschickt. Das F"ullen mit den
    weiteren Fontinformationen mu\s durch die Applikation geschehen.
*/

IMPL_LINK_INLINE_START( SvxFontMenuControl, MenuSelect, FontNameMenu *, pMen )
{
    SvxFontItem aItem( GetId() );
    aItem.GetFamilyName() = pMen->GetCurName();
    GetBindings().GetDispatcher()->Execute( GetId(), SFX_CALLMODE_RECORD, &aItem, 0L );
    return 0;
}
IMPL_LINK_INLINE_END( SvxFontMenuControl, MenuSelect, FontNameMenu *, pMen )

//--------------------------------------------------------------------

/*  [Beschreibung]

    Dtor; gibt das Men"u frei.
*/

SvxFontMenuControl::~SvxFontMenuControl()
{
    delete pMenu;
}

//--------------------------------------------------------------------

/*  [Beschreibung]

    Gibt das Men"u zur"uck
*/

PopupMenu* SvxFontMenuControl::GetPopup() const
{
    return pMenu;
}



