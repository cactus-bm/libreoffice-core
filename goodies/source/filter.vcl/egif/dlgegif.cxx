/*************************************************************************
 *
 *  $RCSfile: dlgegif.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 16:30:11 $
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

#pragma hdrstop
#include <tools/ref.hxx>
#include <vcl/config.hxx>
#include <vcl/msgbox.hxx>
#include "dlgegif.hxx"
#include "dlgegif.hrc"
#include "strings.hrc"

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/

DlgExportEGIF::DlgExportEGIF( FltCallDialogParameter& rPara ) :
                ModalDialog         ( rPara.pWindow, ResId( DLG_EXPORT_GIF, rPara.pResMgr ) ),
                aCbxInterlaced      ( this, ResId( CBX_INTERLACED ) ),
                aCbxTranslucent     ( this, ResId( CBX_TRANSLUCENT ) ),
                aGrpMode            ( this, ResId( GRP_MODE ) ),
                aGrpDraw            ( this, ResId( GRP_DRAW ) ),
                aBtnOK              ( this, ResId( BTN_OK ) ),
                aBtnCancel          ( this, ResId( BTN_CANCEL ) ),
                aBtnHelp            ( this, ResId( BTN_HELP ) ),
                pConfig             ( rPara.pCfg ),
                pMgr                ( rPara.pResMgr )
{
    FreeResource();

    String aInterlaceStr( ResId( KEY_INTER, pMgr ) );
    String aTranslucentStr( ResId( KEY_TRANS, pMgr ) );
    // Config-Parameter lesen
    BOOL bInterlaced = ( pConfig->ReadKey( ByteString( aInterlaceStr, RTL_TEXTENCODING_UTF8 ) ).ToInt32() != 0 );
    BOOL bTranslucent = ( (char) pConfig->ReadKey( ByteString( aTranslucentStr, RTL_TEXTENCODING_UTF8 ) ).ToInt32() != 0 );

    aCbxInterlaced.Check( bInterlaced );
    aCbxTranslucent.Check( bTranslucent );

    aBtnOK.SetClickHdl( LINK( this, DlgExportEGIF, OK ) );
}

/*************************************************************************
|*
|* Speichert eingestellte Werte in ini-Datei
|*
\************************************************************************/

IMPL_LINK( DlgExportEGIF, OK, void *, EMPTYARG )
{

    // Config-Parameter schreiben
    String aInterlaceStr( ResId( KEY_INTER, pMgr ) );
    String aTranslucentStr( ResId( KEY_TRANS, pMgr ) );

    ByteString aStr;

    if ( aCbxInterlaced.IsChecked() )
        aStr = '1';
    else
        aStr = '0';

    pConfig->WriteKey( ByteString( aInterlaceStr, RTL_TEXTENCODING_UTF8 ), aStr );

    if ( aCbxTranslucent.IsChecked() )
        aStr = '1';
    else
        aStr = '0';

    pConfig->WriteKey( ByteString( aTranslucentStr, RTL_TEXTENCODING_UTF8 ), aStr );

    EndDialog( RET_OK );

    return 0;
}



