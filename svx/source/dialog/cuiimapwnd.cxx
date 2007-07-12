/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cuiimapwnd.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: ihi $ $Date: 2007-07-12 10:54:36 $
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

#ifdef SVX_DLLIMPLEMENTATION
#undef SVX_DLLIMPLEMENTATION
#endif

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_HELP_HXX //autogen
#include <vcl/help.hxx>
#endif

#ifndef _SFXSIDS_HRC
#include <sfx2/sfxsids.hrc>     // SID_ATTR_MACROITEM
#endif
#define _ANIMATION

#ifndef _MACROPG_HXX //autogen
#include <sfx2/macropg.hxx>
#endif
#ifndef _GOODIES_IMAPRECT_HXX //autogen
#include <svtools/imaprect.hxx>
#endif
#ifndef _GOODIES_IMAPCIRC_HXX //autogen
#include <svtools/imapcirc.hxx>
#endif
#ifndef _GOODIES_IMAPPOLY_HXX //autogen
#include <svtools/imappoly.hxx>
#endif
#ifndef _URLBMK_HXX //autogen
#include <svtools/urlbmk.hxx>
#endif

#include <xoutbmp.hxx>
#include <svx/dialmgr.hxx>
#include <svx/dialogs.hrc>
#include <svx/svxids.hrc>
#include <imapdlg.hrc>
#include <imapwnd.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdorect.hxx>
#include <svx/svdocirc.hxx>
#include <svx/svdopath.hxx>
#include <svx/xfltrit.hxx>
#include <svx/svdpagv.hxx>

#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _SVX_FILLITEM_HXX //autogen
#include <svx/xfillit.hxx>
#endif
#ifndef _SVX_XLINIIT_HXX //autogen
#include <svx/xlineit.hxx>
#endif

#include <sot/formats.hxx>

#include "cuiimapwnd.hxx" //CHINA001

#define TRANSCOL Color( COL_WHITE )

/*************************************************************************
|*
|*  URLDlg
|*
\************************************************************************/

URLDlg::URLDlg( Window* pWindow, const String& rURL, const String& rAlternativeText, const String& rDescription, const String& rTarget, const String& rName, TargetList& rTargetList )
: ModalDialog( pWindow, SVX_RES( RID_SVXDLG_IMAPURL ) )
, maFtURL( this, SVX_RES( FT_URL1 ) )
, maEdtURL( this, SVX_RES( EDT_URL ) )
, maFtTarget( this, SVX_RES( FT_TARGET ) )
, maCbbTargets( this, SVX_RES( CBB_TARGETS ) )
, maFtName( this, SVX_RES( FT_NAME ) )
, maEdtName( this, SVX_RES( EDT_NAME ) )
, maFtAlternativeText( this, SVX_RES( FT_URLDESCRIPTION ) )
, maEdtAlternativeText( this, SVX_RES( EDT_URLDESCRIPTION ) )
, maFtDescription( this, SVX_RES( FT_DESCRIPTION ) )
, maEdtDescription( this, SVX_RES( EDT_DESCRIPTION ) )
, maFlURL( this, SVX_RES( FL_URL ) )
, maBtnHelp( this, SVX_RES( BTN_HELP1 ) )
, maBtnOk( this, SVX_RES( BTN_OK1 ) )
, maBtnCancel( this, SVX_RES( BTN_CANCEL1 ) )
{
    FreeResource();

    maEdtURL.SetText( rURL );
    maEdtAlternativeText.SetText( rAlternativeText );
    maEdtDescription.SetText( rDescription );
    maEdtName.SetText( rName );

    for( String* pStr = rTargetList.First(); pStr; pStr = rTargetList.Next() )
        maCbbTargets.InsertEntry( *pStr );

    if( !rTarget.Len() )
        maCbbTargets.SetText( String::CreateFromAscii( "_self" ) );
    else
        maCbbTargets.SetText( rTarget );
}
