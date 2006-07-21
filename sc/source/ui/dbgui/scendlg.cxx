/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scendlg.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-21 13:27:48 $
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
#include "precompiled_sc.hxx"

#undef SC_DLLIMPLEMENTATION



#include "scitems.hxx"
#include <svx/drawitem.hxx>
#include <svx/xtable.hxx>
#include <sfx2/objsh.hxx>
#include <svtools/useroptions.hxx>
#include <vcl/msgbox.hxx>

#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif

#include "global.hxx"
#include "globstr.hrc"
#include "tabvwsh.hxx"
#include "viewdata.hxx"
#include "document.hxx"
#include "scresid.hxx"
#include "scendlg.hrc"
#include "scendlg.hxx"

//========================================================================

ScNewScenarioDlg::ScNewScenarioDlg( Window* pParent, const String& rName, BOOL bEdit, BOOL bSheetProtected)

    :   ModalDialog     ( pParent, ScResId( RID_SCDLG_NEWSCENARIO ) ),
        aBtnOk          ( this, ScResId( BTN_OK ) ),
        aBtnCancel      ( this, ScResId( BTN_CANCEL ) ),
        aBtnHelp        ( this, ScResId( BTN_HELP ) ),
        aEdName         ( this, ScResId( ED_NAME ) ),
        aEdComment      ( this, ScResId( ED_COMMENT ) ),
        aCbShowFrame    ( this, ResId( CB_SHOWFRAME ) ),
        aLbColor        ( this, ResId( LB_COLOR ) ),
        //aCbPrintFrame ( this, ResId( CB_PRINTFRAME ) ),
        aCbTwoWay       ( this, ResId( CB_TWOWAY ) ),
        //aCbAttrib     ( this, ResId( CB_ATTRIB ) ),
        //aCbValue      ( this, ResId( CB_VALUE ) ),
        aCbCopyAll      ( this, ResId( CB_COPYALL ) ),
        aCbProtect      ( this, ResId( CB_PROTECT ) ),
        aFlName         ( this, ResId( FL_NAME )),
        aFlComment      ( this, ResId( FL_COMMENT ) ),
        aFlOptions      ( this, ResId( FL_OPTIONS ) ),
        aDefScenarioName( rName ),
        bIsEdit         ( bEdit )
{
    if (bIsEdit)
        SetText(String(ResId(STR_EDIT)));

    SfxObjectShell* pDocSh = SfxObjectShell::Current();
    if ( pDocSh )
    {
        const SfxPoolItem* pItem = pDocSh->GetItem( ITEMID_COLOR_TABLE );
        if ( pItem )
        {
            XColorTable* pColorTable = ((SvxColorTableItem*)pItem)->GetColorTable();
            if (pColorTable)
            {
                aLbColor.SetUpdateMode( FALSE );
                long nCount = pColorTable->Count();
                for ( long n=0; n<nCount; n++ )
                {
                    XColorEntry* pEntry = pColorTable->GetColor(n);
                    aLbColor.InsertEntry( pEntry->GetColor(), pEntry->GetName() );
                }
                aLbColor.SetUpdateMode( TRUE );
            }
        }
    }

    SvtUserOptions aUserOpt;

    String aComment( ScResId( STR_CREATEDBY ) );

    aComment += ' ';
    aComment += aUserOpt.GetFirstName();
    aComment += ' ';
    aComment += aUserOpt.GetLastName();
    aComment.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ", " ));
    aComment += String( ScResId( STR_ON ) );
    aComment += ' ';
    aComment += ScGlobal::GetpLocaleData()->getDate( Date() );//CHINA001 aComment += ScGlobal::pLocaleData->getDate( Date() );
    aComment.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ", " ));
    aComment += ScGlobal::GetpLocaleData()->getTime( Time() );//CHINA001  aComment += ScGlobal::pLocaleData->getTime( Time() );

    aEdComment  .SetText( aComment );
    aEdName     .SetText( rName );
    aBtnOk      .SetClickHdl( LINK( this, ScNewScenarioDlg, OkHdl ) );
    aCbShowFrame.SetClickHdl( LINK( this, ScNewScenarioDlg, EnableHdl ) );

    FreeResource();

    aLbColor.SelectEntry( Color( COL_LIGHTGRAY ) );
    aCbShowFrame.Check(TRUE);
    //aCbPrintFrame.Check(TRUE);
    aCbTwoWay.Check(TRUE);
    //aCbAttrib.Check(FALSE);
    //aCbValue.Check(FALSE);
    aCbCopyAll.Check(FALSE);
    aCbProtect.Check(TRUE);

    if (bIsEdit)
        aCbCopyAll.Enable(FALSE);
    // If the Sheet is protected then we disable the Scenario Protect input
    // and default it to true above. Note we are in 'Add' mode here as: if
    // Sheet && scenario protection are true, then we cannot edit this dialog.
    if (bSheetProtected)
        aCbProtect.Enable(FALSE);

    //! die drei funktionieren noch nicht...
    /*
    aCbPrintFrame.Enable(FALSE);
    aCbAttrib.Enable(FALSE);
    aCbValue.Enable(FALSE);
    */
}

//------------------------------------------------------------------------

__EXPORT ScNewScenarioDlg::~ScNewScenarioDlg()
{
}

//------------------------------------------------------------------------

void ScNewScenarioDlg::GetScenarioData( String& rName, String& rComment,
                                        Color& rColor, USHORT& rFlags ) const
{
    rComment = aEdComment.GetText();
    rName    = aEdName.GetText();

    if ( rName.Len() == 0 )
        rName = aDefScenarioName;

    rColor = aLbColor.GetSelectEntryColor();
    USHORT nBits = 0;
    if (aCbShowFrame.IsChecked())
        nBits |= SC_SCENARIO_SHOWFRAME;
    /*
    if (aCbPrintFrame.IsChecked())
        nBits |= SC_SCENARIO_PRINTFRAME;
    */
    if (aCbTwoWay.IsChecked())
        nBits |= SC_SCENARIO_TWOWAY;
    /*
    if (aCbAttrib.IsChecked())
        nBits |= SC_SCENARIO_ATTRIB;
    if (aCbValue.IsChecked())
        nBits |= SC_SCENARIO_VALUE;
    */
    if (aCbCopyAll.IsChecked())
        nBits |= SC_SCENARIO_COPYALL;
    if (aCbProtect.IsChecked())
        nBits |= SC_SCENARIO_PROTECT;
    rFlags = nBits;
}

void ScNewScenarioDlg::SetScenarioData( const String& rName, const String& rComment,
                                        const Color& rColor, USHORT nFlags )
{
    aEdComment.SetText(rComment);
    aEdName.SetText(rName);
    aLbColor.SelectEntry(rColor);

    aCbShowFrame.Check ( (nFlags & SC_SCENARIO_SHOWFRAME)  != 0 );
    EnableHdl( &aCbShowFrame );
    //aCbPrintFrame.Check( (nFlags & SC_SCENARIO_PRINTFRAME) != 0 );
    aCbTwoWay.Check    ( (nFlags & SC_SCENARIO_TWOWAY)     != 0 );
    //aCbAttrib.Check    ( (nFlags & SC_SCENARIO_ATTRIB)     != 0 );
    //aCbValue.Check     ( (nFlags & SC_SCENARIO_VALUE)      != 0 );
    //  CopyAll nicht
    aCbProtect.Check    ( (nFlags & SC_SCENARIO_PROTECT)     != 0 );
}

//------------------------------------------------------------------------

IMPL_LINK( ScNewScenarioDlg, OkHdl, OKButton *, EMPTYARG )
{
    String      aName   ( aEdName.GetText() );
    ScDocument* pDoc    = ((ScTabViewShell*)SfxViewShell::Current())->
                                GetViewData()->GetDocument();

    aName.EraseLeadingChars( ' ' );
    aName.EraseTrailingChars( ' ' );
    aEdName.SetText( aName );

    if ( !pDoc->ValidTabName( aName ) )
    {
        InfoBox( this, ScGlobal::GetRscString( STR_INVALIDTABNAME ) ).
            Execute();
        aEdName.GrabFocus();
    }
    else if ( !bIsEdit && !pDoc->ValidNewTabName( aName ) )
    {
        InfoBox( this, ScGlobal::GetRscString( STR_NEWTABNAMENOTUNIQUE ) ).
            Execute();
        aEdName.GrabFocus();
    }
    else
        EndDialog( RET_OK );
    return 0;

    //! beim Editieren testen, ob eine andere Tabelle den Namen hat!
}

//------------------------------------------------------------------------

IMPL_LINK( ScNewScenarioDlg, EnableHdl, CheckBox *, pBox )
{
    if( pBox == &aCbShowFrame )
        aLbColor.Enable( aCbShowFrame.IsChecked() );
    return 0;
}

