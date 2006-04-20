/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: soldlg.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2006-04-20 15:15:01 $
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

#include <tools/geninfo.hxx>
//#include "depapp.hxx"
#include "soldep.hxx"
#include "soldlg.hxx"
#include "soldlg.hrc"

#ifndef SOLARIS
#define SIZE( nX, nY)   \
    LogicToLogic(Size(nX,nY),&MapMode(MAP_APPFONT),&GetMapMode())
#define POS(nX, nY) \
    LogicToLogic(Point(nX,nY),&MapMode(MAP_APPFONT),&GetMapMode())
#else
#define SIZE( nX, nY)   \
        LogicToLogic(Size(nX,nY),MapMode(MAP_APPFONT),GetMapMode())
#define POS(nX, nY) \
        LogicToLogic(Point(nX,nY),MapMode(MAP_APPFONT),GetMapMode())
#endif
//
// class SolNewProjectDlg
//

/*****************************************************************************/
SolNewProjectDlg::SolNewProjectDlg( Window* pParent, const ResId& rResId )
/*****************************************************************************/
                : ModalDialog( pParent, rResId ),
                maOkButton( this, DtSodResId( BTN_OK )),
                maCancelButton( this, DtSodResId( BTN_CANCEL )),
                maFTName( this, DtSodResId( FT_PRJNAME )),
                maEName( this, DtSodResId( EDIT_PRJNAME )),
                maFTShort( this, DtSodResId( FT_PRJSHORT )),
                maEShort( this, DtSodResId( EDIT_PRJSHORT )),
                maFTDeps( this, DtSodResId( FT_PRJDEPS )),
                maEDeps( this, DtSodResId( EDIT_PRJDEPS ))
{
    FreeResource();
    maOkButton.SetClickHdl( LINK( this, SolNewProjectDlg, OkHdl ));
    maCancelButton.SetClickHdl( LINK( this, SolNewProjectDlg, CancelHdl ));
}

/*****************************************************************************/
IMPL_LINK( SolNewProjectDlg, OkHdl, Button*, pOkBtn )
/*****************************************************************************/
{
    EndDialog( 1 );
    return 0;
}

/*****************************************************************************/
IMPL_LINK( SolNewProjectDlg, CancelHdl, Button*,  pCancelBtn )
/*****************************************************************************/
{
    EndDialog( 0 );
    return 0;
}

//
// class SolNewDirectoryDlg
//

/*****************************************************************************/
SolNewDirectoryDlg::SolNewDirectoryDlg( Window* pParent, const ResId& rResId )
/*****************************************************************************/
                : ModalDialog( pParent, rResId ),
                maOkButton( this, DtSodResId( BTN_OK )),
                maCancelButton( this, DtSodResId( BTN_CANCEL )),
                maFTName( this, DtSodResId( FT_DIRNAME )),
                maEName( this, DtSodResId( EDIT_DIRNAME )),
                maFTFlag( this, DtSodResId( FT_DIRFLAG )),
                maEFlag( this, DtSodResId( EDIT_DIRFLAG )),
                maFTDeps( this, DtSodResId( FT_DIRDEPS )),
                maEDeps( this, DtSodResId( EDIT_DIRDEPS )),
                maFTAction( this, DtSodResId( FT_DIRACTION )),
                maEAction( this, DtSodResId( EDIT_DIRACTION )),
                maFTEnv( this, DtSodResId( FT_DIRENV )),
                maEEnv( this, DtSodResId( EDIT_DIRENV ))
{
    FreeResource();
    maOkButton.SetClickHdl( LINK( this, SolNewDirectoryDlg, OkHdl ));
    maCancelButton.SetClickHdl( LINK( this, SolNewDirectoryDlg, CancelHdl ));
}

/*****************************************************************************/
IMPL_LINK( SolNewDirectoryDlg, OkHdl, Button*, pOkBtn )
/*****************************************************************************/
{
    EndDialog( 1 );
    return 0;
}

/*****************************************************************************/
IMPL_LINK( SolNewDirectoryDlg, CancelHdl, Button*,  pCancelBtn )
/*****************************************************************************/
{
    EndDialog( 0 );
    return 0;
}

//
// class SolHelpDlg
//

/*****************************************************************************/
SolHelpDlg::SolHelpDlg( Window* pParent, const ResId& rResId )
/*****************************************************************************/
                : ModalDialog( pParent, rResId ),
                maOkButton( this, DtSodResId( BTN_OK )),
                maMLEHelp( this, DtSodResId( EDIT_HELP ))
{
    FreeResource();
    maOkButton.SetClickHdl( LINK( this, SolHelpDlg, OkHdl ));
}

/*****************************************************************************/
IMPL_LINK( SolHelpDlg, OkHdl, Button*, pOkBtn )
/*****************************************************************************/
{
    EndDialog( 1 );
    return 0;
}

//
// class SolSelectVersionDlg
//

/*****************************************************************************/
SolSelectVersionDlg::SolSelectVersionDlg(
    Window *pParent, GenericInformationList *pStandLst )
/*****************************************************************************/
                : ModalDialog( pParent, DtSodResId( DLG_VERSIONSELECT )),
                maVersionListBox( this, DtSodResId( DLG_VERSIONSELECT_LISTBOX )),
                maVersionGroupBox( this, DtSodResId( DLG_VERSIONSELECT_GROUP )),
                maOKButton( this, DtSodResId( DLG_VERSIONSELECT_OK  )),
                maCancelButton( this, DtSodResId( DLG_VERSIONSELECT_CANCEL ))
{
    FreeResource();

    //Fill the ListBox with MWS versions (e.g. SRC680) from "stand.lst"
    for ( ULONG i = 0; i < pStandLst->Count(); i++ ) {
        String sVersion( *pStandLst->GetObject( i ), RTL_TEXTENCODING_ASCII_US );
        maVersionListBox.InsertEntry( sVersion );
    }

    if ( pStandLst->Count())
        maVersionListBox.SelectEntryPos( 0 );

    maVersionListBox.SetDoubleClickHdl(
        LINK( this, SolSelectVersionDlg, DoubleClickHdl ));
}

/*****************************************************************************/
ByteString SolSelectVersionDlg::GetVersion()
/*****************************************************************************/
{
    //Returns the selected version
    ByteString sReturn(
        maVersionListBox.GetSelectEntry(), RTL_TEXTENCODING_ASCII_US );

    return sReturn;
}

/*****************************************************************************/
IMPL_LINK( SolSelectVersionDlg, DoubleClickHdl, ListBox *, pBox )
/*****************************************************************************/
{
    EndDialog( RET_OK );
    return 0;
}

//
// class SolAutoarrangeDlg
//

/*****************************************************************************/
SolAutoarrangeDlg::SolAutoarrangeDlg( Window *pParent )
/*****************************************************************************/
                : ModelessDialog( pParent, DtSodResId( DLG_AUTOARRANGE )),
                maGroupBox( this, DtSodResId( DLG_AUTOARRANGE_GROUP )),
                maModuleText( this, DtSodResId( DLG_AUTOARRANGE_TEXT_MODULE )),
                maOverallText( this, DtSodResId( DLG_AUTOARRANGE_TEXT_OVERALL )),
                maModuleBar( this ),
                maOverallBar( this )
{
    FreeResource();

    maModuleBar.SetPosPixel( POS( 8, 28 ));
    maOverallBar.SetPosPixel( POS( 8, 60 ));
    maModuleBar.SetSizePixel( SIZE( 208,12 ));
    maOverallBar.SetSizePixel( SIZE( 208,12 ));

    maModuleBar.Show();
    maOverallBar.Show();

       maModuleText.Show();
    maOverallText.Show();
}

/**********************************************************************************/

SolFindProjectDlg::SolFindProjectDlg( Window *pParent, ObjWinList* pObjList )
                : ModalDialog( pParent, DtSodResId( DLG_FIND_PROJECT )),
                maCombobox( this, DtSodResId( DLG_FIND_PROJECT_COMBOBOX )),
                maOKButton( this, DtSodResId( DLG_FIND_PROJECT_OK )),
                maCancelButton( this, DtSodResId( DLG_FIND_PROJECT_CANCEL ))
{
    FreeResource();
    maCombobox.SetDropDownLineCount(15);
//    SolDep* pSolDep = ((MyApp*)GetpApp())->GetSolDep();
//    ObjWinList* pObjList = pSolDep->GetObjectList();
    ULONG n = pObjList->Count();
    //Fill combobox
    for (ULONG i=0; i<n; i++) {
        ByteString prjname = pObjList->GetObject( i )->GetBodyText();
        if (prjname != ByteString("null"))                                 //null_project
            maCombobox.InsertEntry( String(prjname,RTL_TEXTENCODING_UTF8) );
        }
}

ByteString SolFindProjectDlg::GetProject()
{
    return ByteString(maCombobox.GetText(),RTL_TEXTENCODING_UTF8);
}
