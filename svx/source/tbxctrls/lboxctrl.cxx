/*************************************************************************
 *
 *  $RCSfile: lboxctrl.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: jp $ $Date: 2001-10-16 07:47:14 $
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

#ifdef   _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX
#include <vcl/toolbox.hxx>
#endif
#ifndef _SV_EVENT_HXX
#include <vcl/event.hxx>
#endif
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#ifndef _SFXTBXCTRL_HXX
#include <sfx2/tbxctrl.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXDISPATCH_HXX
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXVIEWSH_HXX
#include <sfx2/viewsh.hxx>
#endif
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _STDCTRL_HXX
#include <svtools/stdctrl.hxx>
#endif
#ifndef _SFXSLSTITM_HXX
#include <svtools/slstitm.hxx>
#endif
#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
#endif

#ifndef _SVX_DIALMGR_HXX
#include <dialmgr.hxx>
#endif
#ifndef _SVX_LBOXCTRL_HXX_
#include <lboxctrl.hxx>
#endif

#include <svxids.hrc>
#include <dialogs.hrc>

#include "lboxctrl.hrc"

class SvxPopupWindowListBox;

/////////////////////////////////////////////////////////////////

class SvxPopupWindowListBox : public SfxPopupWindow
{
    FixedInfo       aInfo;
    ListBox *       pListBox;
    ToolBox &       rToolBox;
    BOOL            bUserSel;

    // disallow copy-constructor and assignment-operator

    SvxPopupWindowListBox(const& );
    SvxPopupWindowListBox & operator = (const& );

    SvxPopupWindowListBox( USHORT nSlotId,
                           ToolBox& rTbx, USHORT nTbxItemId );

public:
    SvxPopupWindowListBox( USHORT nSlotId,
                           ToolBox& rTbx,
                           SfxBindings &rBindings );
    virtual ~SvxPopupWindowListBox();

    // SfxPopupWindow
    virtual SfxPopupWindow *    Clone() const;
    virtual void                PopupModeEnd();
    virtual void                StateChanged( USHORT nSID, SfxItemState eState,
                                              const SfxPoolItem* pState );

    void                        StartSelection();
    inline ListBox &            GetListBox()    { return *pListBox; }
    inline FixedInfo &          GetInfo()       { return aInfo; }

    BOOL                        IsUserSelected() const          { return bUserSel; }
    void                        SetUserSelected( BOOL bVal )    { bUserSel = bVal; }
};

/////////////////////////////////////////////////////////////////

SvxPopupWindowListBox::SvxPopupWindowListBox(
            USHORT nSlotId,
            ToolBox& rTbx,
            SfxBindings &rBindings ) :

    SfxPopupWindow( nSlotId, SVX_RES( RID_SVXTBX_UNDO_REDO_CTRL ), rBindings ),
    aInfo       ( this, ResId( FT_NUM_OPERATIONS ) ),
    rToolBox    ( rTbx ),
    bUserSel    ( FALSE )
{
    DBG_ASSERT( nSlotId == GetId(), "id mismatch" );
    pListBox = new ListBox( this, SVX_RES( LB_SVXTBX_UNDO_REDO_CTRL ) );
    FreeResource();
    pListBox->EnableMultiSelection( TRUE, TRUE );
    SetBackground( GetSettings().GetStyleSettings().GetDialogColor() );
    pListBox->GrabFocus();
}


SvxPopupWindowListBox::~SvxPopupWindowListBox()
{
    delete pListBox;
}


SfxPopupWindow* SvxPopupWindowListBox::Clone() const
{
    return new SvxPopupWindowListBox( GetId(), rToolBox,
                                      (SfxBindings &) GetBindings() );
}


void SvxPopupWindowListBox::PopupModeEnd()
{
    rToolBox.EndSelection();
    SfxPopupWindow::PopupModeEnd();
    //FloatingWindow::PopupModeEnd();

    Window* pShellWnd = SfxViewShell::Current()->GetWindow();
    if (pShellWnd)
        pShellWnd->GrabFocus();
}


void SvxPopupWindowListBox::StateChanged(
        USHORT nSID, SfxItemState eState, const SfxPoolItem* pState )
{
    rToolBox.EnableItem( GetId(), (GetItemState(pState) != SFX_ITEM_DISABLED) );
    SfxPopupWindow::StateChanged( nSID, eState, pState );
}


void SvxPopupWindowListBox::StartSelection()
{
    rToolBox.StartSelection();
}


/////////////////////////////////////////////////////////////////

SFX_IMPL_TOOLBOX_CONTROL( SvxListBoxControl, SfxStringItem );


SvxListBoxControl::SvxListBoxControl(
                            USHORT nId, ToolBox& rTbx, SfxBindings& rBind )
    :SfxToolBoxControl( nId, rTbx, rBind ),
    pPopupWin   ( 0 )
{
    rTbx.SetItemBits( nId, TIB_DROPDOWN | rTbx.GetItemBits( nId ) );
    rTbx.Invalidate();
}


SvxListBoxControl::~SvxListBoxControl()
{
}


SfxPopupWindow* SvxListBoxControl::CreatePopupWindow()
{
    DBG_ERROR( "not implemented" );
    return 0;
}


SfxPopupWindowType SvxListBoxControl::GetPopupWindowType() const
{
    return SFX_POPUPWINDOW_ONTIMEOUT;
}


void SvxListBoxControl::StateChanged(
        USHORT nSID, SfxItemState eState, const SfxPoolItem* pState )
{
    GetToolBox().EnableItem( GetId(),
                            SFX_ITEM_DISABLED != GetItemState(pState) );
}


IMPL_LINK( SvxListBoxControl, PopupModeEndHdl, void *, EMPTYARG )
{
    if( pPopupWin && 0 == pPopupWin->GetPopupModeFlags()  &&
        pPopupWin->IsUserSelected() )
    {
        USHORT nCount = pPopupWin->GetListBox().GetSelectEntryCount();
        SfxUInt16Item aItem( GetId(), nCount );
        GetBindings().GetDispatcher()->Execute( GetId(),
                                        SFX_CALLMODE_SYNCHRON, &aItem, 0L );
    }
    return 0;
}


void SvxListBoxControl::Impl_SetInfo( USHORT nCount )
{
    DBG_ASSERT( pPopupWin, "NULL pointer, PopupWindow missing" );
    String aText( aActionStr );
    aText.SearchAndReplaceAllAscii( "$(ARG1)", String::CreateFromInt32( nCount ) );
    pPopupWin->GetInfo().SetText( aText );
}


IMPL_LINK( SvxListBoxControl, SelectHdl, void *, EMPTYARG )
{
    if (pPopupWin)
    {
        //pPopupWin->SetUserSelected( FALSE );

        ListBox &rListBox = pPopupWin->GetListBox();
        if (rListBox.IsTravelSelect())
            Impl_SetInfo( rListBox.GetSelectEntryCount() );
        else
        {
            pPopupWin->SetUserSelected( TRUE );
            pPopupWin->EndPopupMode( 0 );
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////

SFX_IMPL_TOOLBOX_CONTROL( SvxUndoRedoControl, SfxStringItem );

SvxUndoRedoControl::~SvxUndoRedoControl()
{
}


SfxPopupWindow* SvxUndoRedoControl::CreatePopupWindow()
{
    const USHORT nId = GetId();
    DBG_ASSERT( SID_UNDO == nId || SID_REDO == nId, "mismatching ids" );

    const SfxPoolItem* pState = 0;
    SfxBindings &rBindings = GetBindings();
    SfxDispatcher &rDispatch = *rBindings.GetDispatcher();
    SfxItemState eState = rDispatch.QueryState( SID_UNDO == nId
                                                ? SID_GETUNDOSTRINGS
                                                : SID_GETREDOSTRINGS, pState );

    if( eState >= SFX_ITEM_AVAILABLE  &&  pState)
    {
        ToolBox& rBox = GetToolBox();

        pPopupWin = new SvxPopupWindowListBox( nId, rBox, rBindings );
        pPopupWin->SetPopupModeEndHdl( LINK( this, SvxUndoRedoControl,
                                                PopupModeEndHdl ) );
        ListBox &rListBox = pPopupWin->GetListBox();
        rListBox.SetSelectHdl( LINK( this, SvxUndoRedoControl, SelectHdl ) );

        SfxStringListItem &rItem = *(SfxStringListItem *) pState;
        const List* pLst = rItem.GetList();
        DBG_ASSERT( pLst, "no undo actions available" );
        if( pLst )
            for( long nI = 0, nEnd = pLst->Count(); nI < nEnd; ++nI )
                rListBox.InsertEntry( *((String*)pLst->GetObject( nI )) );
        rListBox.SelectEntryPos( 0 );

        aActionStr = String( SVX_RES( SID_UNDO == nId
                                        ? RID_SVXSTR_NUM_UNDO_ACTIONS
                                           : RID_SVXSTR_NUM_REDO_ACTIONS ) );
        Impl_SetInfo( rListBox.GetSelectEntryCount() );

        // position window at the bottom-left of the toolbox icon.
        // The -2 offset takes the distance from the item-rect to
        // the toolbox border into account (can't be obtained from
        // the toolbox).
        Rectangle aItemRect( rBox.GetItemRect( nId ) );
        aItemRect.Bottom() += aItemRect.GetHeight() - 2;

        {
            MouseEvent aMEvt( aItemRect.TopLeft(), 1, 0, 0, 0 );
            Link aSave( rBox.GetSelectHdl() );
            rBox.SetSelectHdl( Link() );
            rBox.Tracking( TrackingEvent( aMEvt, ENDTRACK_END ) );
            rBox.SetSelectHdl( aSave );
        }

        pPopupWin->StartPopupMode( aItemRect );
        pPopupWin->StartSelection();
    }
    return pPopupWin;
}



