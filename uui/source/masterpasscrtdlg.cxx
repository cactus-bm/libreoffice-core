/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: masterpasscrtdlg.cxx,v $
 * $Revision: 1.4 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include <svtools/filedlg.hxx>
#include <vcl/msgbox.hxx>

#ifndef UUI_IDS_HRC
#include <ids.hrc>
#endif
#ifndef UUI_MASTERPASSCRTDLG_HRC
#include <masterpasscrtdlg.hrc>
#endif
#include <masterpasscrtdlg.hxx>

// MasterPasswordCreateDialog---------------------------------------------------

// -----------------------------------------------------------------------

IMPL_LINK( MasterPasswordCreateDialog, EditHdl_Impl, Edit *, EMPTYARG )
{
    aOKBtn.Enable( aEDMasterPasswordCrt.GetText().Len() >= nMinLen );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( MasterPasswordCreateDialog, OKHdl_Impl, OKButton *, EMPTYARG )
{
    // compare both passwords and show message box if there are not equal!!
    if( aEDMasterPasswordCrt.GetText() == aEDMasterPasswordRepeat.GetText() )
        EndDialog( RET_OK );
    else
    {
        String aErrorMsg( ResId( STR_ERROR_PASSWORDS_NOT_IDENTICAL, *pResourceMgr ));
        ErrorBox aErrorBox( this, WB_OK, aErrorMsg );
        aErrorBox.Execute();
        aEDMasterPasswordCrt.SetText( String() );
        aEDMasterPasswordRepeat.SetText( String() );
        aEDMasterPasswordCrt.GrabFocus();
    }
    return 1;
}

// -----------------------------------------------------------------------

MasterPasswordCreateDialog::MasterPasswordCreateDialog
(
    Window*                                     pParent,
    ResMgr*                                     pResMgr
) :

    ModalDialog( pParent, ResId( DLG_UUI_MASTERPASSWORD_CRT, *pResMgr ) ),

    aFTMasterPasswordCrt        ( this, ResId( FT_MASTERPASSWORD_CRT, *pResMgr ) ),
    aEDMasterPasswordCrt        ( this, ResId( ED_MASTERPASSWORD_CRT, *pResMgr ) ),
    aFTMasterPasswordRepeat ( this, ResId( FT_MASTERPASSWORD_REPEAT, *pResMgr ) ),
    aEDMasterPasswordRepeat ( this, ResId( ED_MASTERPASSWORD_REPEAT, *pResMgr ) ),
    aOKBtn                  ( this, ResId( BTN_MASTERPASSCRT_OK, *pResMgr ) ),
    aCancelBtn              ( this, ResId( BTN_MASTERPASSCRT_CANCEL, *pResMgr ) ),
    aHelpBtn                ( this, ResId( BTN_MASTERPASSCRT_HELP, *pResMgr ) ),
    pResourceMgr            ( pResMgr ),
    nMinLen(5)
{
    FreeResource();

    aOKBtn.Enable( sal_False );
    aOKBtn.SetClickHdl( LINK( this, MasterPasswordCreateDialog, OKHdl_Impl ) );
    aEDMasterPasswordCrt.SetModifyHdl( LINK( this, MasterPasswordCreateDialog, EditHdl_Impl ) );
};

