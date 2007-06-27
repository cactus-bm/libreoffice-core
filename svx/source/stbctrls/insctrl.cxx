/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: insctrl.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 18:54:37 $
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

// include ---------------------------------------------------------------

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif
#ifndef _STATUS_HXX //autogen
#include <vcl/status.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif

#define _SVX_INSCTRL_CXX

#include <svx/dialogs.hrc>

#include "insctrl.hxx"
#include <svx/dialmgr.hxx>

#define PAINT_OFFSET    5

SFX_IMPL_STATUSBAR_CONTROL(SvxInsertStatusBarControl, SfxBoolItem);

// class SvxInsertStatusBarControl ---------------------------------------

SvxInsertStatusBarControl::SvxInsertStatusBarControl( USHORT _nSlotId,
                                                      USHORT _nId,
                                                      StatusBar& rStb ) :

    SfxStatusBarControl( _nSlotId, _nId, rStb ),
    bInsert( TRUE )
{
    rStb.SetHelpId( _nId, _nSlotId );
}

// -----------------------------------------------------------------------

SvxInsertStatusBarControl::~SvxInsertStatusBarControl()
{
}

// -----------------------------------------------------------------------

void SvxInsertStatusBarControl::StateChanged( USHORT , SfxItemState eState,
                                              const SfxPoolItem* pState )
{
    if ( SFX_ITEM_AVAILABLE != eState )
        GetStatusBar().SetItemText( GetId(), String() );
    else
    {
        DBG_ASSERT( pState->ISA( SfxBoolItem ), "invalid item type" );
        SfxBoolItem* pItem = (SfxBoolItem*)pState;
        bInsert = pItem->GetValue();
        DrawItemText_Impl();
    }
}

// -----------------------------------------------------------------------

void SvxInsertStatusBarControl::Click()
{
    if ( !GetStatusBar().GetItemText( GetId() ).Len() )
        return;
    bInsert = !bInsert;
    SfxBoolItem aIns( GetSlotId(), bInsert );

    ::com::sun::star::uno::Any a;
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aArgs( 1 );
    aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "InsertMode" ));
    aIns.QueryValue( a );
    aArgs[0].Value = a;

    execute( aArgs );
}

// -----------------------------------------------------------------------

void SvxInsertStatusBarControl::Paint( const UserDrawEvent& )
{
    DrawItemText_Impl();
}

// -----------------------------------------------------------------------

void SvxInsertStatusBarControl::DrawItemText_Impl()
{
    USHORT _nId = RID_SVXSTR_OVERWRITE_TEXT;

    if ( bInsert )
        _nId = RID_SVXSTR_INSERT_TEXT;
    GetStatusBar().SetItemText( GetId(), SVX_RESSTR( _nId ) );
}

ULONG SvxInsertStatusBarControl::GetDefItemWidth(const StatusBar& rStb)
{
    long nWidth1 =  rStb.GetTextWidth(SVX_RESSTR(RID_SVXSTR_OVERWRITE_TEXT));
    long nWidth2 =  rStb.GetTextWidth(SVX_RESSTR(RID_SVXSTR_INSERT_TEXT));

    if(nWidth1<nWidth2)
        nWidth1=nWidth2;

    return nWidth1+PAINT_OFFSET;
}


