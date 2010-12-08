/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#ifndef _SFX_PASSWD_HXX
#define _SFX_PASSWD_HXX

#include "sal/config.h"
#include <sfx2/dllapi.h>
#include <vcl/button.hxx>
#include <vcl/dialog.hxx>
#include <vcl/edit.hxx>
#include <vcl/fixed.hxx>
#include <sfx2/app.hxx>

// defines ---------------------------------------------------------------

#define SHOWEXTRAS_NONE      ((USHORT)0x0000)
#define SHOWEXTRAS_USER      ((USHORT)0x0001)
#define SHOWEXTRAS_CONFIRM   ((USHORT)0x0002)
#define SHOWEXTRAS_PASSWORD2 ((USHORT)0x0004)
#define SHOWEXTRAS_CONFIRM2  ((USHORT)0x0008)
#define SHOWEXTRAS_ALL       ((USHORT)(SHOWEXTRAS_USER | SHOWEXTRAS_CONFIRM))

// class SfxPasswordDialog -----------------------------------------------

class SFX2_DLLPUBLIC SfxPasswordDialog : public ModalDialog
{
private:
    FixedLine       maPasswordBox;
    FixedText       maUserFT;
    Edit            maUserED;
    FixedText       maPasswordFT;
    Edit            maPasswordED;
    FixedText       maConfirmFT;
    Edit            maConfirmED;
    FixedLine       maPassword2Box;
    FixedText       maPassword2FT;
    Edit            maPassword2ED;
    FixedText       maConfirm2FT;
    Edit            maConfirm2ED;

    OKButton        maOKBtn;
    CancelButton    maCancelBtn;
    HelpButton      maHelpBtn;

    USHORT          mnMinLen;
    USHORT          mnExtras;

    bool            mbAsciiOnly;
    DECL_DLLPRIVATE_LINK( EditModifyHdl, Edit* );
    DECL_DLLPRIVATE_LINK( OKHdl, OKButton* );

public:
    SfxPasswordDialog( Window* pParent, const String* pGroupText = NULL );

    String          GetUser() const { return maUserED.GetText(); }
    String          GetPassword() const { return maPasswordED.GetText(); }
    String          GetConfirm() const { return maConfirmED.GetText(); }

    String          GetPassword2() const { return maPassword2ED.GetText(); }
    String          GetConfirm2() const { return maConfirm2ED.GetText(); }
    void            SetGroup2Text( const String& i_rText ) { maPassword2Box.SetText( i_rText ); }

    void            SetMinLen( USHORT Len );
    void            SetMaxLen( USHORT Len );
    void            SetEditHelpId( const rtl::OString& rId ) { maPasswordED.SetHelpId( rId ); }
    void            ShowExtras( USHORT nExtras ) { mnExtras = nExtras; }
    void            AllowAsciiOnly( bool i_bAsciiOnly = true ) { mbAsciiOnly = i_bAsciiOnly; }

    virtual short   Execute();
};

#endif // #ifndef _SFX_PASSWD_HXX

