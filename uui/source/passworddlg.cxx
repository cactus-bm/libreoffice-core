/*************************************************************************
 *
 *  $RCSfile: passworddlg.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: mav $ $Date: 2001-06-26 13:35:41 $
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

#ifndef _SVT_FILEDLG_HXX
#include <svtools/filedlg.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif

#ifndef UUI_IDS_HRC
#include <ids.hrc>
#endif
#ifndef UUI_PASSWORDDLG_HRC
#include <passworddlg.hrc>
#endif
#ifndef UUI_PASSWORDDLG_HXX
#include <passworddlg.hxx>
#endif

// MasterPasswordDialog---------------------------------------------------

// -----------------------------------------------------------------------

IMPL_LINK( MasterPasswordDialog, OKHdl_Impl, OKButton *, EMPTYARG )
{
    // todo: compare both passwords and show message box if there are not equal!!
    if( nDialogMode != ::com::sun::star::task::PasswordRequestMode_PASSWORD_CREATE
     || aEDMasterPassword.GetText() == aEDMasterPasswordRepeat.GetText() )
        EndDialog( RET_OK );
    else
    {
        String aErrorMsg( ResId( STR_ERROR_PASSWORDS_NOT_IDENTICAL, pResourceMgr ));
        ErrorBox aErrorBox( this, WB_OK, aErrorMsg );
        aErrorBox.Execute();
        aEDMasterPassword.SetText( String() );
        aEDMasterPasswordRepeat.SetText( String() );
        aEDMasterPassword.GrabFocus();
    }
    return 1;
}

// -----------------------------------------------------------------------

MasterPasswordDialog::MasterPasswordDialog
(
    Window*                                     pParent,
    ::com::sun::star::task::PasswordRequestMode aDialogMode,
    ResMgr*                                     pResMgr
) :

    ModalDialog( pParent, ResId( DLG_UUI_PASSWORD, pResMgr ) ),

    aInfoEnterPassword      ( this, ResId( INFO_ENTER_PASSWORD ) ),
    aFTMasterPassword       ( this, ResId( FT_MASTERPASSWORD ) ),
    aEDMasterPassword       ( this, ResId( ED_MASTERPASSWORD ) ),
    aFTMasterPasswordRepeat ( this, ResId( FT_MASTERPASSWORD_REPEAT ) ),
    aEDMasterPasswordRepeat ( this, ResId( ED_MASTERPASSWORD_REPEAT ) ),
    aOKBtn                  ( this, ResId( BTN_MASTERPASSWORD_OK ) ),
    aCancelBtn              ( this, ResId( BTN_MASTERPASSWORD_CANCEL ) ),
    aHelpBtn                ( this, ResId( BTN_MASTERPASSWORD_HELP ) ),
    nDialogMode             ( aDialogMode ),
    pResourceMgr            ( pResMgr )
{
    // hide until a help is avalable
    aHelpBtn.Hide();

    if ( nDialogMode != ::com::sun::star::task::PasswordRequestMode_PASSWORD_CREATE )
    {
        if( nDialogMode == ::com::sun::star::task::PasswordRequestMode_PASSWORD_REENTER )
        {
            String aErrorMsg( ResId( STR_ERROR_PASSWORD_WRONG, pResourceMgr ));
            ErrorBox aErrorBox( this, WB_OK, aErrorMsg );
            aErrorBox.Execute();
        }

        aFTMasterPasswordRepeat.Hide();
        aEDMasterPasswordRepeat.Hide();

        Size aNewSiz = GetSizePixel();
        aNewSiz.Height() -= 20;
        SetSizePixel( aNewSiz );
    }

    FreeResource();

    aOKBtn.SetClickHdl( LINK( this, MasterPasswordDialog, OKHdl_Impl ) );
};
