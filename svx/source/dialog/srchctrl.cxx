/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: srchctrl.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 17:39:43 $
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
#include <tools/pstm.hxx>
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif

#include <svx/svxids.hrc>

#define _SVX_SRCHDLG_CXX // damit private-Methoden vom SrchDlg bekannt sind



#include "srchctrl.hxx"
#include "srchdlg.hxx"
#include <sfx2/srchitem.hxx>

// class SvxSearchFamilyControllerItem -----------------------------------

SvxSearchController::SvxSearchController
(
    USHORT _nId,
    SfxBindings& rBind,
    SvxSearchDialog& rDlg
) :
    SfxControllerItem( _nId, rBind ),

    rSrchDlg( rDlg )
{
}

// -----------------------------------------------------------------------

void SvxSearchController::StateChanged( USHORT nSID, SfxItemState eState,
                                        const SfxPoolItem* pState )
{
    if ( SFX_ITEM_AVAILABLE == eState )
    {
        if ( SID_STYLE_FAMILY1 <= nSID && nSID <= SID_STYLE_FAMILY4 )
        {
            SfxObjectShell* pShell = SfxObjectShell::Current();

            if ( pShell && pShell->GetStyleSheetPool() )
                rSrchDlg.TemplatesChanged_Impl( *pShell->GetStyleSheetPool() );
        }
        else if ( SID_SEARCH_OPTIONS == nSID )
        {
            DBG_ASSERT( pState->ISA(SfxUInt16Item), "wrong item type" );
            USHORT nFlags = (USHORT)( (SfxUInt16Item*)pState )->GetValue();
            rSrchDlg.EnableControls_Impl( nFlags );
        }
        else if ( SID_SEARCH_ITEM == nSID )
        {
            DBG_ASSERT( pState->ISA(SvxSearchItem), "wrong item type" );
            rSrchDlg.SetItem_Impl( (const SvxSearchItem*)pState );
        }
    }
    else if ( SID_SEARCH_OPTIONS == nSID || SID_SEARCH_ITEM == nSID )
        rSrchDlg.EnableControls_Impl( 0 );
}


