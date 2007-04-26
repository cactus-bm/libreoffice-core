/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dinfedt.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-26 10:10:51 $
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

#ifndef GCC
#endif

#include "dinfedt.hxx"
#include "sfxresid.hxx"

#include "sfx.hrc"
#include "dinfedt.hrc"

// class InfoEdit_Impl ---------------------------------------------------

void InfoEdit_Impl::KeyInput( const KeyEvent& rKEvent )
{
    if ( rKEvent.GetCharCode() != '~' )
        Edit::KeyInput( rKEvent );
}

// class SfxDocInfoEditDlg -----------------------------------------------

SfxDocInfoEditDlg::SfxDocInfoEditDlg( Window* pParent ) :

    ModalDialog( pParent, SfxResId( DLG_DOCINFO_EDT ) ),

    aInfoFL     ( this, SfxResId( FL_INFO ) ),
    aInfo1ED    ( this, SfxResId( ED_INFO1 ) ),
    aInfo2ED    ( this, SfxResId( ED_INFO2 ) ),
    aInfo3ED    ( this, SfxResId( ED_INFO3 ) ),
    aInfo4ED    ( this, SfxResId( ED_INFO4 ) ),
    aOkBT       ( this, SfxResId( BT_OK ) ),
    aCancelBT   ( this, SfxResId( BT_CANCEL ) ),
    aHelpBtn    ( this, SfxResId( BTN_HELP ) )

{
    FreeResource();
}

