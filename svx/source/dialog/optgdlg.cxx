/*************************************************************************
 *
 *  $RCSfile: optgdlg.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-26 14:20:03 $
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

#ifndef _ZFORLIST_HXX //autogen
#include <svtools/zforlist.hxx>
#endif
#ifndef _GRFMGR_HXX
#include <goodies/grfmgr.hxx>
#endif
#ifndef _SFXFLAGITEM_HXX //autogen
#include <svtools/flagitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVTOOLS_LINGUCFG_HXX_
#include <svtools/lingucfg.hxx>
#endif
#ifndef _SFXSZITEM_HXX
#include <svtools/szitem.hxx>
#endif
#ifndef _SFXVIEWSH_HXX
#include <sfx2/viewsh.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_MNEMONIC_HXX
#include <vcl/mnemonic.hxx>
#endif
#ifndef _ISOLANG_HXX
#include <tools/isolang.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_CACHEOPTIONS_HXX
#include <svtools/cacheoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_OPTIONS3D_HXX
#include <svtools/options3d.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_FONTOPTIONS_HXX
#include <svtools/fontoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MENUOPTIONS_HXX
#include <svtools/menuoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_STARTOPTIONS_HXX
#include <svtools/startoptions.hxx>
#endif
#ifndef _SVTOOLS_LANGUAGEOPTIONS_HXX
#include <svtools/languageoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MISCPOPT_HXX
#include <svtools/miscopt.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_PRINTOPTIONS_HXX
#include <svtools/printwarningoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_SYSLOCALEOPTIONS_HXX
#include <svtools/syslocaleoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_HELPOPT_HXX
#include <svtools/helpopt.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _SVTOOLS_CTLOPTIONS_HXX
#include <svtools/ctloptions.hxx>
#endif

#include <unotools/localfilehelper.hxx>

#ifndef _SVX_CUIOPTGENRL_HXX
#include "cuioptgenrl.hxx"
#endif
#ifndef _SVX_OPTPATH_HXX
#include "optpath.hxx"
#endif
#ifndef _SVX_OPTSAVE_HXX
#include "optsave.hxx"
#endif
#ifndef _SVX_OPTLINGU_HXX
#include "optlingu.hxx"
#endif
#ifndef _XPOOL_HXX
#include "xpool.hxx"
#endif
#ifndef _SVX_DLGUTIL_HXX
#include "dlgutil.hxx"
#endif
#ifndef _SVX_TAB_AREA_HXX
#include "cuitabarea.hxx"
#endif
#ifndef _SVX_LANGTAB_HXX //autogen
#include "langtab.hxx"
#endif
#ifndef _SVX_DIALOGS_HRC
#include "dialogs.hrc"
#endif
#ifndef _UNO_LINGU_HXX
#include "unolingu.hxx"
#endif
#ifndef _SVX_SVXIDS_HRC
#include "svxids.hrc"
#endif
#define ITEMID_LANGUAGE 0
#ifndef _SVX_LANGITEM_HXX
#include "langitem.hxx"
#endif

#include "dialmgr.hxx"
#include "optgdlg.hrc"
#include "optgdlg.hxx"
#include "ofaitem.hxx"
#include <svtools/apearcfg.hxx>

#define CONFIG_LANGUAGES "OfficeLanguages"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::utl;
using namespace ::rtl;
using namespace ::sfx2;

#define C2U(cChar) OUString::createFromAscii(cChar)

#ifdef WNT
#define HELPER_PAGE_MAIL_ONLY
#else
#define HELPER_PAGE_COMPLETE
#endif

#define MAX_PROGRAM_ENTRIES     3

// class OfaMiscTabPage --------------------------------------------------

int OfaMiscTabPage::DeactivatePage( SfxItemSet* pSet )
{
    if ( pSet )
        FillItemSet( *pSet );
    return LEAVE_PAGE;
}

// -----------------------------------------------------------------------

OfaMiscTabPage::OfaMiscTabPage(Window* pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, ResId( OFA_TP_MISC, DIALOG_MGR() ), rSet ),

    aGbTwoFigure            ( this, ResId( GB_TWOFIGURE ) ),
    aFtInterpret            ( this, ResId( FT_INTERPRET ) ),
    aNfYearValue            ( this, ResId( NF_YEARVALUE ) ),
    aFtTo                   ( this, ResId( FT_TO) ),
    aHelpAgentFL            ( this, ResId( FL_HELPAGENT ) ),
    aHelpAgentCB            ( this, ResId( CB_HELPAGENT ) ),
    aHelpAgentResetFT       ( this, ResId( FT_HELPAGENT_RESET ) ),
    aHelpAgentResetBtn      ( this, ResId( PB_HELPAGENT_RESET ) ),
    aHelpAgentTimeFT        ( this, ResId( FT_HELPAGENT_TIME ) ),
    aHelpAgentTimeED        ( this, ResId( ED_HELPAGENT_TIME ) ),
    aHelpAgentTimeUnitFT    ( this, ResId( FT_HELPAGENT_TIME_UNIT ) ),
    aHelpFormattingFL       ( this, ResId( FL_HELPFORMATTING ) ),
    aStyleSheetFT           ( this, ResId( FT_STYLESHEET     ) ),
    aStyleSheetLB           ( this, ResId( LB_STYLESHEET     ) ),
    aFileDlgFL(               this, ResId( FL_FILEDLG )),
    aFileDlgCB(               this, ResId( CB_FILEDLG )),
    aDocStatusFL(             this, ResId( FL_DOCSTATUS )),
    aDocStatusCB(             this, ResId( CB_DOCSTATUS ))
{
    FreeResource();

    //quick launch only available in Win
#ifndef WNT
    aFileDlgFL.Hide();
    aFileDlgCB.Hide();
#endif

    aStrDateInfo = aFtTo.GetText();
    aNfYearValue.SetModifyHdl( LINK( this, OfaMiscTabPage, TwoFigureHdl ) );
    Link aLink = LINK( this, OfaMiscTabPage, TwoFigureConfigHdl );
    aNfYearValue.SetDownHdl( aLink );
    aNfYearValue.SetUpHdl( aLink );
    aNfYearValue.SetLoseFocusHdl( aLink );
    aNfYearValue.SetFirstHdl( aLink );
    TwoFigureConfigHdl( &aNfYearValue );

    SetExchangeSupport();

    aHelpAgentCB.SetClickHdl( LINK( this, OfaMiscTabPage, HelpAgentHdl_Impl ) );
    aHelpAgentResetBtn.SetClickHdl( LINK( this, OfaMiscTabPage, HelpAgentResetHdl_Impl ) );


    //fill default names as user data
    static const char* aStyleSheetNames[] =
    {
        "Default",
        "HighContrast1",
        "HighContrast2",
        "HighContrastBlack",
        "HighContrastWhite"
    };

    for(USHORT i = 0; i < aStyleSheetLB.GetEntryCount(); i++)
    {
        String* pData = new String(String::CreateFromAscii(aStyleSheetNames[i]));
        aStyleSheetLB.SetEntryData( i, pData );
    }
}

// -----------------------------------------------------------------------

OfaMiscTabPage::~OfaMiscTabPage()
{
    for(USHORT i = 0; i < aStyleSheetLB.GetEntryCount(); i++)
    {
        delete (String*)aStyleSheetLB.GetEntryData( i );
    }
}

// -----------------------------------------------------------------------

SfxTabPage* OfaMiscTabPage::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return new OfaMiscTabPage( pParent, rAttrSet );
}

// -----------------------------------------------------------------------

BOOL OfaMiscTabPage::FillItemSet( SfxItemSet& rSet )
{
    BOOL bModified = FALSE;

    SvtCacheOptions aCacheOptions;
    const SfxUInt16Item* pUInt16Item = PTR_CAST( SfxUInt16Item, GetOldItem( rSet, SID_ATTR_YEAR2000 ) );
    USHORT nNum = (USHORT)aNfYearValue.GetText().ToInt32();
    if ( pUInt16Item && pUInt16Item->GetValue() != nNum )
    {
        bModified = TRUE;
        rSet.Put( SfxUInt16Item( SID_ATTR_YEAR2000, nNum ) );
    }

    // help agent
    BOOL bChecked = aHelpAgentCB.IsChecked();
    if ( bChecked != aHelpAgentCB.GetSavedValue() )
    {
        rSet.Put( SfxBoolItem( SID_ATTR_AUTOHELPAGENT, aHelpAgentCB.IsChecked() ) );
        bModified = TRUE;
    }

    if ( bChecked && aHelpAgentTimeED.GetText() != aHelpAgentTimeED.GetSavedValue() )
    {
        sal_Int32 nNewTime = aHelpAgentTimeED.GetValue();
        rSet.Put( SfxInt32Item( SID_HELPAGENT_TIMEOUT, nNewTime ) );
        bModified = TRUE;
    }
    USHORT nStyleSheetPos = aStyleSheetLB.GetSelectEntryPos();
    if( nStyleSheetPos != LISTBOX_ENTRY_NOTFOUND &&
        nStyleSheetPos != aStyleSheetLB.GetSavedValue())
    {
        rSet.Put(
            SfxStringItem( SID_HELP_STYLESHEET,
            *(String*)aStyleSheetLB.GetEntryData( nStyleSheetPos )));
        bModified = TRUE;
    }

    if ( aFileDlgCB.IsChecked() != aFileDlgCB.GetSavedValue() )
    {
        SvtMiscOptions aMiscOpt;
        aMiscOpt.SetUseSystemFileDialog( !aFileDlgCB.IsChecked() );
        bModified = TRUE;
    }
    if(aDocStatusCB.IsChecked() != aDocStatusCB.GetSavedValue())
    {
        SvtPrintWarningOptions aPrintOptions;
        aPrintOptions.SetModifyDocumentOnPrintingAllowed(aDocStatusCB.IsChecked());
        bModified = TRUE;
    }
    return bModified;
}

// -----------------------------------------------------------------------

void OfaMiscTabPage::Reset( const SfxItemSet& rSet )
{
    SvtCacheOptions     aCacheOptions;
    const SfxPoolItem*  pItem;

    if ( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_YEAR2000, FALSE, &pItem ) )
    {
        aNfYearValue.SetValue( ((SfxUInt16Item*)pItem)->GetValue() );
        TwoFigureConfigHdl( &aNfYearValue );
    }
    else
    {
        aNfYearValue.Enable(FALSE);
        aGbTwoFigure.Enable(FALSE);
        aFtInterpret.Enable(FALSE);
        aFtTo.Enable(FALSE);
    }

    // Undo-Schritte
    USHORT nWhich = GetWhich( SID_ATTR_UNDO_COUNT );

    // help agent
    if ( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_AUTOHELPAGENT, FALSE, &pItem ) )
        aHelpAgentCB.Check( ( (SfxBoolItem*)pItem )->GetValue() );
    if ( SFX_ITEM_SET == rSet.GetItemState( SID_HELPAGENT_TIMEOUT, FALSE, &pItem ) )
        aHelpAgentTimeED.SetValue( ( (SfxInt32Item*)pItem )->GetValue() );
    if ( SFX_ITEM_SET == rSet.GetItemState( SID_HELP_STYLESHEET, FALSE, &pItem ) )
    {
        String sStyleSheet = ((SfxStringItem*)pItem)->GetValue();
        for(USHORT i = 0; i < aStyleSheetLB.GetEntryCount(); i++)
        {
            if(*(String*)aStyleSheetLB.GetEntryData( i ) == sStyleSheet)
            {
                aStyleSheetLB.SelectEntryPos(i);
                break;
            }
        }
    }

    aStyleSheetLB.SaveValue();
    HelpAgentHdl_Impl( &aHelpAgentCB );
    aHelpAgentCB.SaveValue();
    aHelpAgentTimeED.SaveValue();

    SvtMiscOptions aMiscOpt;
    aFileDlgCB.Check( !aMiscOpt.UseSystemFileDialog() );
    aFileDlgCB.SaveValue();

    SvtPrintWarningOptions aPrintOptions;
    aDocStatusCB.Check(aPrintOptions.IsModifyDocumentOnPrintingAllowed());
    aDocStatusCB.SaveValue();
}

// -----------------------------------------------------------------------

IMPL_LINK( OfaMiscTabPage, TwoFigureHdl, NumericField*, pEd )
{
    String aOutput( aStrDateInfo );
    String aStr( aNfYearValue.GetText() );
    International aInt;
    aStr.EraseAllChars( aInt.GetNumThousandSep() );
    long nNum = aStr.ToInt32();
    if ( aStr.Len() != 4 || nNum < aNfYearValue.GetMin() || nNum > aNfYearValue.GetMax() )
        aOutput.AppendAscii("????");
    else
    {
        nNum += 99;
        aOutput += String::CreateFromInt32( nNum );
    }
    aFtTo.SetText( aOutput );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( OfaMiscTabPage, TwoFigureConfigHdl, NumericField*, pEd )
{
    long nNum = aNfYearValue.GetValue();
    String aOutput( String::CreateFromInt32( nNum ) );
    aNfYearValue.SetText( aOutput );
    aNfYearValue.SetSelection( Selection( 0, aOutput.Len() ) );
    TwoFigureHdl( pEd );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( OfaMiscTabPage, HelpAgentHdl_Impl, CheckBox*, pBox )
{
    BOOL bEnable = pBox->IsChecked();
    aHelpAgentResetFT.Enable( bEnable );
    aHelpAgentResetBtn.Enable( bEnable );
    aHelpAgentTimeFT.Enable( bEnable );
    aHelpAgentTimeED.Enable( bEnable );
    aHelpAgentTimeUnitFT.Enable( bEnable );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( OfaMiscTabPage, HelpAgentResetHdl_Impl, PushButton*, EMPTYARG )
{
    SvtHelpOptions().resetAgentIgnoreURLCounter();
    return 0;
}

// class OfaViewTabPage --------------------------------------------------
// -----------------------------------------------------------------------
IMPL_LINK_INLINE_START( OfaViewTabPage, OpenGLHdl, CheckBox*, EMPTYARG )
{
    a3DOpenGLFasterCB.Enable( a3DOpenGLCB.IsChecked() );
    return 0;
}
IMPL_LINK_INLINE_END( OfaViewTabPage, OpenGLHdl, CheckBox*, EMPTYARG )

OfaViewTabPage::OfaViewTabPage(Window* pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, ResId( OFA_TP_VIEW, DIALOG_MGR() ), rSet ),

    aAppearanceGB       ( this, ResId( GB_APPEARANCE ) ),
    aAppearanceFT       ( this, ResId( FT_APPEARANCE ) ),
    aAppearanceLB       ( this, ResId( LB_APPEARANCE ) ),
    aScalingFT          ( this, ResId( FT_SCALING ) ),
    aScalingMF          ( this, ResId( MF_SCALING ) ),
    aStyleCB            ( this, ResId( CB_STYLE ) ),
    aMenuIconsCB        ( this, ResId( CB_MENU_ICONS )),
    aBigFT              ( this, ResId( FT_BIG ) ),
    aBigLB              ( this, ResId( LB_BIG ) ),
#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    aFontAntiAliasing   ( this, ResId( CB_FONTANTIALIASING )),
    aAAPointLimitLabel  ( this, ResId( FT_POINTLIMIT_LABEL )),
    aAAPointLimit       ( this, ResId( NF_AA_POINTLIMIT )),
    aAAPointLimitUnits  ( this, ResId( FT_POINTLIMIT_UNIT )),
#endif
    aMouseFL            ( this, ResId( FL_MOUSE ) ),
    aMousePosFT         ( this, ResId( FT_MOUSEPOS ) ),
    aMousePosLB         ( this, ResId( LB_MOUSEPOS ) ),
    aMouseMiddleFT      ( this, ResId( FT_MOUSEMIDDLE ) ),
    aMouseMiddleLB      ( this, ResId( LB_MOUSEMIDDLE ) ),
    aMouseFollowCB      ( this, ResId( CB_MOUSE_FOLLOW ) ),
    aFlatTabCtrlCB      ( this, ResId( CB_FLAT_TAB_CTRL ) ),
    aColorTabCtrlCB     ( this, ResId( CB_COLOR_TAB_CTRL ) ),
    aFontShowCB         ( this, ResId( CB_FONT_SHOW ) ),
    aShowInactiveItemsCB( this, ResId( CB_SHOW_INACTIVE ) ),
    aFontHistoryCB      ( this, ResId( CB_FONT_HISTORY ) ),
    a3DGB               ( this, ResId( GB_3D ) ),
    a3DOpenGLCB         ( this, ResId( CB_3D_OPENGL ) ),
    a3DOpenGLFasterCB   ( this, ResId( CB_3D_OPENGL_FASTER ) ),
    a3DDitheringCB      ( this, ResId( CB_3D_DITHERING ) ),
    a3DShowFullCB       ( this, ResId( CB_3D_SHOWFULL ) ),
    aWorkingSetBox      ( this, ResId( GB_WORKINGSET ) ),
    aDocViewBtn         ( this, ResId( BTN_DOCVIEW ) ),
    aOpenWinBtn         ( this, ResId( BTN_OPENWIN ) ),

    nBigLB_InitialSelection( 1 ),       // something, what doesn't really matter 'cause it's set in Reset()
    bSfxSymbolsAuto( FALSE )

{
    a3DOpenGLCB.SetClickHdl( LINK( this, OfaViewTabPage, OpenGLHdl ) );

#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    aFontAntiAliasing.SetToggleHdl( LINK( this, OfaViewTabPage, OnAntialiasingToggled ) );

    // depending on the size of the text in aAAPointLimitLabel, we slightly re-arrange aAAPointLimit and aAAPointLimitUnits
    //#110391#  if the label has no mnemonic and we are in a CJK version the mnemonic "(X)" will be added which
    //          influences the width calculation
    MnemonicGenerator aMnemonicGenerator;
    String sLabel(aAAPointLimitLabel.GetText());
    aMnemonicGenerator.RegisterMnemonic( sLabel );
    aMnemonicGenerator.CreateMnemonic( sLabel );
    sLabel.EraseAllChars('~');

    sal_Int32 nLabelWidth = aAAPointLimitLabel.GetTextWidth( sLabel );
    nLabelWidth += 3;   // small gap
    // pixels to move both controls to the left
    Size aSize = aAAPointLimitLabel.GetSizePixel();
    sal_Int32 nMoveLeft = aSize.Width() - nLabelWidth;
    // resize the first label
    aSize.Width() = nLabelWidth;
    aAAPointLimitLabel.SetSizePixel( aSize );

    // move the numeric field
    Point aPos( aAAPointLimit.GetPosPixel() );
    aPos.X() -= nMoveLeft;
    aAAPointLimit.SetPosPixel( aPos );

    // move (and resize) the units FixedText
    aPos = ( aAAPointLimitUnits.GetPosPixel() );
    aPos.X() -= nMoveLeft;
    aSize = aAAPointLimitUnits.GetSizePixel();
    aSize.Width() += nMoveLeft;
    aAAPointLimitUnits.SetPosSizePixel( aPos, aSize );
#else
    // on this platform, we do not have the anti aliasing options - move the other checkboxes accordingly
    // (in the resource, the coordinates are calculated for the AA options beeing present)
    Control* pMiscOptions[] =
    {
        &aMouseFollowCB, &aFlatTabCtrlCB, &aColorTabCtrlCB, &aFontShowCB, &aShowInactiveItemsCB, &aFontHistoryCB,
        &aMenuIconsCB
    };

    // temporaryly create the checkbox for the anti aliasing (we need to to determine it's pos)
    CheckBox* pFontAntiAliasing = new CheckBox( this, ResId( CB_FONTANTIALIASING ) );
    sal_Int32 nMoveUp = aMouseFollowCB.GetPosPixel().Y() - pFontAntiAliasing->GetPosPixel().Y();
    DELETEZ( pFontAntiAliasing );

    Point aPos;
    for ( sal_Int32 i = 0; i < sizeof( pMiscOptions ) / sizeof( pMiscOptions[0] ); ++i )
    {
        aPos = pMiscOptions[i]->GetPosPixel( );
        aPos.Y() -= nMoveUp;
        pMiscOptions[i]->SetPosPixel( aPos );
    }

#endif
    FreeResource();
}

OfaViewTabPage::~OfaViewTabPage()
{
    if( bSfxSymbolsAuto )
        SvtMiscOptions().SetSymbolSet( SFX_SYMBOLS_AUTO );
}

#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
//--- 20.08.01 10:16:12 ---------------------------------------------------
IMPL_LINK( OfaViewTabPage, OnAntialiasingToggled, void*, NOTINTERESTEDIN )
{
    sal_Bool bAAEnabled = aFontAntiAliasing.IsChecked();

    aAAPointLimitLabel.Enable( bAAEnabled );
    aAAPointLimit.Enable( bAAEnabled );
    aAAPointLimitUnits.Enable( bAAEnabled );

    return 0L;
}
#endif

/*-----------------06.12.96 11.50-------------------

--------------------------------------------------*/

SfxTabPage* OfaViewTabPage::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return new OfaViewTabPage(pParent, rAttrSet);
}

/*-----------------06.12.96 11.50-------------------

--------------------------------------------------*/

BOOL OfaViewTabPage::FillItemSet( SfxItemSet& rSet )
{
    SvtFontOptions aFontOpt;
    SvtMenuOptions aMenuOpt;
    SvtStartOptions aStartOpt;

    BOOL bModified = FALSE;
    BOOL bMenuOptModified = FALSE;

    SfxBoolItem *pItem = PTR_CAST( SfxBoolItem, GetOldItem( rSet, SID_ATTR_BUTTON_OUTSTYLE3D ) );
    if ( pItem->GetValue() == aStyleCB.IsChecked() )
    {
        bModified = TRUE;
        rSet.Put( SfxBoolItem( SID_ATTR_BUTTON_OUTSTYLE3D, !aStyleCB.IsChecked() ) );
    }

    pItem = PTR_CAST( SfxBoolItem, GetOldItem( rSet, SID_ATTR_BUTTON_BIGSIZE ) );
    UINT16 nBigLB_NewSelection = aBigLB.GetSelectEntryPos();
    if( nBigLB_InitialSelection != nBigLB_NewSelection )
    {
        // from now on it's modified, even if via auto setting the same size was set as now selected in the LB
        BOOL bBig;
        switch( nBigLB_NewSelection )
        {
            case 0:
                {
                    bBig = ( Application::GetSettings().GetStyleSettings().GetToolbarIconSize() == STYLE_TOOLBAR_ICONSIZE_LARGE );
                    //SvtMiscOptions().SetSymbolSet( SFX_SYMBOLS_AUTO );    has no long effect, because this is changed when
                    // applying the item set! -> so done in Dtor!! It's a hack, but doesn't potentially break other code
                    bSfxSymbolsAuto = TRUE;     // ... will trigger this
                }
                break;
            case 1: bBig = FALSE;   break;
            case 2: bBig = TRUE;    break;
            default:
                DBG_ERROR( "OfaViewTabPage::FillItemSet(): This state should not be possible!" );
        }

        rSet.Put( SfxBoolItem( SID_ATTR_BUTTON_BIGSIZE, bBig ) );
    }

    // Look & Feel Group
    SfxUInt16Item *pU16Item = PTR_CAST( SfxUInt16Item, GetOldItem(rSet, SID_OPT_SYSTEMLOOK));
    SystemLook eOldLook = (SystemLook)pU16Item->GetValue();
    SystemLook eNewLook;

    switch (aAppearanceLB.GetSelectEntryPos())
    {
        case 1: eNewLook = LookMacintosh;       break;
        case 2: eNewLook = LookMotif;           break;
        case 3: eNewLook = LookOSTwo;           break;
        case 0:
        default:eNewLook = LookStardivision;    break;
    }

    if ( eNewLook != eOldLook )
    {
        bModified = TRUE;
        rSet.Put( SfxUInt16Item( SID_OPT_SYSTEMLOOK, (UINT16)eNewLook ) );
    }

    // Screen Scaling
    pU16Item = PTR_CAST( SfxUInt16Item, GetOldItem(rSet, SID_OPT_SCREENSCALING));
    UINT16 nOldScale = (UINT16)pU16Item->GetValue();
    UINT16 nNewScale = (UINT16)aScalingMF.GetValue();

    if ( nNewScale != nOldScale )
    {
        bModified = TRUE;
        rSet.Put( SfxUInt16Item( SID_OPT_SCREENSCALING, nNewScale ) );
    }

    // Mouse Snap Mode
    SfxInt16Item* p16Item = PTR_CAST( SfxInt16Item, GetOldItem(rSet, SID_OPT_SNAPTYPE) );
    short eOldSnap = (SnapType)p16Item->GetValue();
    short eNewSnap = aMousePosLB.GetSelectEntryPos();
    if(eNewSnap > 2)
        eNewSnap = 2;

    if ( eNewSnap != eOldSnap )
    {
        bModified = TRUE;
        rSet.Put( SfxInt16Item( SID_OPT_SNAPTYPE, eNewSnap ) );
    }

    // Middle Mouse Button
    p16Item = PTR_CAST( SfxInt16Item, GetOldItem(rSet, SID_OPT_MIDDLE_MOUSE) );
    short eOldMiddleMouse = p16Item->GetValue();
    short eNewMiddleMouse = aMouseMiddleLB.GetSelectEntryPos();
    if(eNewMiddleMouse > 2)
        eNewMiddleMouse = 2;

    if ( eNewMiddleMouse != eOldMiddleMouse )
    {
        bModified = TRUE;
        rSet.Put( SfxInt16Item( SID_OPT_MIDDLE_MOUSE, eNewMiddleMouse ) );
    }

#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    if ( aFontAntiAliasing.IsChecked() != aFontAntiAliasing.GetSavedValue() )
    {
        rSet.Put( SfxBoolItem( SID_OPT_FONT_ANTIALIASING_ENABLED, aFontAntiAliasing.IsChecked() ) );
        bModified = TRUE;
    }

    if ( aAAPointLimit.GetValue() != aAAPointLimit.GetSavedValue().ToInt32() )
    {
        rSet.Put( SfxUInt16Item( SID_OPT_FONT_ANTIALIASING_MINPIXELS, aAAPointLimit.GetValue() ) );
        bModified = TRUE;
    }
#endif

    if ( aMouseFollowCB.IsChecked() != aMouseFollowCB.GetSavedValue() )
    {
        rSet.Put( SfxBoolItem( SID_APPEAR_MENUE_MOUSE_FOLLOW, aMouseFollowCB.IsChecked() ) );
        bModified = TRUE;
    }
    if ( aFlatTabCtrlCB.IsChecked() != aFlatTabCtrlCB.GetSavedValue() )
    {
        rSet.Put( SfxBoolItem( SID_APPEAR_SINGLE_LINE_TABCTRL, aFlatTabCtrlCB.IsChecked() ) );
        bModified = TRUE;
    }

    if ( aColorTabCtrlCB.IsChecked() != aColorTabCtrlCB.GetSavedValue() )
    {
        rSet.Put( SfxBoolItem( SID_APPEAR_COLORED_TABCTRL, aColorTabCtrlCB.IsChecked() ) );
        bModified = TRUE;
    }

    if ( aFontShowCB.IsChecked() != aFontShowCB.GetSavedValue() )
    {
        aFontOpt.EnableFontWYSIWYG( aFontShowCB.IsChecked() );
        bModified = TRUE;
    }

    if ( aShowInactiveItemsCB.IsChecked() != aShowInactiveItemsCB.GetSavedValue() )
    {
        aMenuOpt.SetEntryHidingState( aShowInactiveItemsCB.IsChecked() );
        bModified = TRUE;
    }

    if(aMenuIconsCB.IsChecked() != aMenuIconsCB.GetSavedValue())
    {
        aMenuOpt.SetMenuIconsState( aMenuIconsCB.IsChecked() );
        bModified = TRUE;
        bMenuOptModified = TRUE;
    }

    if ( aFontHistoryCB.IsChecked() != aFontHistoryCB.GetSavedValue() )
    {
        aFontOpt.EnableFontHistory( aFontHistoryCB.IsChecked() );
        bModified = TRUE;
    }
    // Workingset
    if ( aDocViewBtn.IsChecked() != aDocViewBtn.GetSavedValue() )
    {
        rSet.Put( SfxBoolItem( GetWhich( SID_ATTR_SAVEDOCVIEW ),
                               aDocViewBtn.IsChecked() ) );
        bModified |= TRUE;
    }

    if ( aOpenWinBtn.IsChecked() != aOpenWinBtn.GetSavedValue() )
    {
        rSet.Put( SfxBoolItem( GetWhich( SID_ATTR_SAVEDOCWINS ),
                               aOpenWinBtn.IsChecked() ) );
        bModified |= TRUE;
    }
    SvtOptions3D a3DOpt;
    BOOL bTemp = a3DOpt.IsOpenGL();

    if ( bTemp != a3DOpenGLCB.IsChecked() )
    {
        a3DOpt.SetOpenGL( a3DOpenGLCB.IsChecked() );
        bModified = TRUE;
    }

    BOOL bCheck = ( a3DOpenGLCB.IsChecked() && a3DOpenGLFasterCB.IsChecked() );
    if ( a3DOpt.IsOpenGL_Faster() != bCheck )
    {
        a3DOpt.SetOpenGL_Faster( bCheck );
        bModified = TRUE;
    }

    if ( a3DOpt.IsDithering() != a3DDitheringCB.IsChecked() )
    {
        a3DOpt.SetDithering( a3DDitheringCB.IsChecked() );
        bModified = TRUE;
    }

    if ( a3DOpt.IsShowFull() != a3DShowFullCB.IsChecked() )
    {
        a3DOpt.SetShowFull( a3DShowFullCB.IsChecked() );
        bModified = TRUE;
    }

    if( bMenuOptModified )
    {
        // Set changed settings to the application instance
        AllSettings aAllSettings = Application::GetSettings();
        StyleSettings aStyleSettings = aAllSettings.GetStyleSettings();
        aStyleSettings.SetUseImagesInMenus( aMenuIconsCB.IsChecked() );
        aAllSettings.SetStyleSettings(aStyleSettings);
        Application::MergeSystemSettings( aAllSettings );
        Application::SetSettings(aAllSettings);
    }

    return bModified;
}

/*-----------------06.12.96 11.50-------------------

--------------------------------------------------*/

void OfaViewTabPage::Reset( const SfxItemSet& rSet )
{
    const SfxPoolItem* pItem;

    SvtOptions3D a3DOpt;
    a3DOpenGLCB.Check( a3DOpt.IsOpenGL() );
    a3DOpenGLFasterCB.Check( a3DOpenGLCB.IsChecked() && a3DOpt.IsOpenGL_Faster() );
    OpenGLHdl( NULL );
    a3DDitheringCB.Check( a3DOpt.IsDithering() );
    a3DShowFullCB.Check( a3DOpt.IsShowFull() );

    if( SFX_ITEM_SET == rSet.GetItemState(SID_ATTR_BUTTON_OUTSTYLE3D, FALSE, &pItem) )
        aStyleCB.Check( !((SfxBoolItem*)pItem)->GetValue() );

    if( SvtMiscOptions().GetSymbolSet() == SFX_SYMBOLS_AUTO )
        nBigLB_InitialSelection = 0;
    else if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_BUTTON_BIGSIZE, FALSE, &pItem ) )
        nBigLB_InitialSelection = ( ( ( SfxBoolItem* ) pItem )->GetValue() )? 2 : 1;
    else
        nBigLB_InitialSelection = 0;
    aBigLB.SelectEntryPos( nBigLB_InitialSelection );

    // Look & Feel
    if ( SFX_ITEM_SET ==
         rSet.GetItemState( SID_OPT_SYSTEMLOOK, FALSE, &pItem ) )
    {
        SystemLook eLook = (SystemLook)((const SfxUInt16Item*)pItem)->GetValue();
        USHORT nPos;

        switch (eLook)
        {
            case LookMacintosh:     nPos = 1;   break;
            case LookMotif:         nPos = 2;   break;
            case LookOSTwo:         nPos = 3;   break;
            case LookStardivision:
            default:                nPos = 0;   break;
        }
        aAppearanceLB.SelectEntryPos(nPos);
    }
    aAppearanceLB.SaveValue();

    // Screen Scaling
    if ( SFX_ITEM_SET ==
         rSet.GetItemState( SID_OPT_SCREENSCALING, FALSE, &pItem ) )
    {
        UINT16 nScaleFactor = (UINT16)((const SfxUInt16Item*)pItem )->GetValue();
        aScalingMF.SetValue ( nScaleFactor );
    }
    aScalingMF.SaveValue();

    // Mouse Snap
    if ( SFX_ITEM_SET ==
         rSet.GetItemState( SID_OPT_SNAPTYPE, FALSE, &pItem ) )
    {
        short nSnap = (SnapType)((const SfxInt16Item*)pItem )->GetValue();
        aMousePosLB.SelectEntryPos(nSnap);
    }
    aMousePosLB.SaveValue();

    // Mouse Snap
    if ( SFX_ITEM_SET ==
         rSet.GetItemState( SID_OPT_MIDDLE_MOUSE, FALSE, &pItem ) )
    {
        short nMiddle = ((const SfxInt16Item*)pItem )->GetValue();
        aMouseMiddleLB.SelectEntryPos(nMiddle);
    }
    aMouseMiddleLB.SaveValue();

#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    if( SFX_ITEM_SET == rSet.GetItemState( SID_OPT_FONT_ANTIALIASING_ENABLED, FALSE, &pItem ) )
        aFontAntiAliasing.Check( static_cast< const SfxBoolItem* >( pItem )->GetValue() );

    if( SFX_ITEM_SET == rSet.GetItemState( SID_OPT_FONT_ANTIALIASING_MINPIXELS, FALSE, &pItem ) )
        aAAPointLimit.SetValue( static_cast< const SfxUInt16Item* >( pItem )->GetValue() );
#endif

    if(SFX_ITEM_SET == rSet.GetItemState(SID_APPEAR_MENUE_MOUSE_FOLLOW, FALSE, &pItem))
        aMouseFollowCB.Check( ((const SfxBoolItem*)pItem)->GetValue() );

    if(SFX_ITEM_SET == rSet.GetItemState(SID_APPEAR_SINGLE_LINE_TABCTRL, FALSE, &pItem))
        aFlatTabCtrlCB.Check( ((const SfxBoolItem*)pItem)->GetValue() );

    if(SFX_ITEM_SET == rSet.GetItemState(SID_APPEAR_COLORED_TABCTRL, FALSE, &pItem))
        aColorTabCtrlCB.Check(((const SfxBoolItem*)pItem)->GetValue() );

    // WorkingSet
    USHORT nWhich = GetWhich( SID_ATTR_SAVEDOCVIEW );

    if ( SFX_ITEM_SET == rSet.GetItemState( nWhich, FALSE, &pItem ) )
        aDocViewBtn.Check( ( (const SfxBoolItem*)pItem )->GetValue() );

    nWhich = GetWhich( SID_ATTR_SAVEDOCWINS );

    if ( SFX_ITEM_SET == rSet.GetItemState( nWhich, FALSE, &pItem ) )
        aOpenWinBtn.Check( ( (const SfxBoolItem*)pItem )->GetValue() );


    SvtFontOptions aFontOpt;
    aFontShowCB.Check( aFontOpt.IsFontWYSIWYGEnabled() );
    SvtMenuOptions aMenuOpt;
    aShowInactiveItemsCB.Check( aMenuOpt.IsEntryHidingEnabled() );
    aMenuIconsCB.Check(aMenuOpt.IsMenuIconsEnabled());
    aMenuIconsCB.SaveValue();

    aFontHistoryCB.Check( aFontOpt.IsFontHistoryEnabled() );

#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    aFontAntiAliasing.SaveValue();
    aAAPointLimit.SaveValue();
#endif
    aMouseFollowCB.SaveValue();
    aFlatTabCtrlCB.SaveValue();
    aColorTabCtrlCB.SaveValue();
    aFontShowCB.SaveValue();
    aShowInactiveItemsCB.SaveValue();
    aFontHistoryCB.SaveValue();
    aDocViewBtn.SaveValue();
    aOpenWinBtn.SaveValue();

#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    LINK( this, OfaViewTabPage, OnAntialiasingToggled ).Call( NULL );
#endif
}
/* -----------------------------23.11.00 14:55--------------------------------

 ---------------------------------------------------------------------------*/
class LangConfigItem_Impl : public ConfigItem
{
    Any         aValue;
    OUString    aPropertyName;
public:
    LangConfigItem_Impl(const OUString& rTree, const OUString& rProperty);
    ~LangConfigItem_Impl();

    virtual void            Commit();

    const Any&  GetValue() const {return aValue;}
    void        SetValue(Any& rValue)  {aValue = rValue; SetModified();}
};
/* -----------------------------23.11.00 15:06--------------------------------

 ---------------------------------------------------------------------------*/
LangConfigItem_Impl::LangConfigItem_Impl(
    const OUString& rTree, const OUString& rProperty) :
    ConfigItem(rTree),
    aPropertyName(rProperty)
{
    Sequence<OUString> aNames(1);
    aNames.getArray()[0] = aPropertyName;
    Sequence<Any> aValues = GetProperties(aNames);
    aValue = aValues.getConstArray()[0];
}
/* -----------------------------23.11.00 15:06--------------------------------

 ---------------------------------------------------------------------------*/
LangConfigItem_Impl::~LangConfigItem_Impl()
{}
/* -----------------------------23.11.00 15:10--------------------------------

 ---------------------------------------------------------------------------*/
void LangConfigItem_Impl::Commit()
{
    Sequence<OUString> aNames(1);
    aNames.getArray()[0] = aPropertyName;
    Sequence<Any> aValues(1);
    aValues.getArray()[0] = aValue;
    PutProperties(aNames, aValues);
}
/* -----------------------------23.11.00 14:47--------------------------------

 ---------------------------------------------------------------------------*/
/*class LanguageConfig_Impl
{
    LangConfigItem_Impl aSetup;
    LangConfigItem_Impl aProfile;
public:
    LanguageConfig_Impl();
    ~LanguageConfig_Impl();

    Sequence<OUString>  GetInstalledLanguages();
    OUString            GetCurrentLanguage();
    void                SetCurrentLanguage(const OUString& rSet);

};
/* -----------------------------23.11.00 14:53--------------------------------

 ---------------------------------------------------------------------------*/
/*LanguageConfig_Impl::LanguageConfig_Impl() :
    aSetup(C2U("Setup/Office"), C2U("Locales")),
    aProfile(C2U("UserProfile/International"), C2U("Locale"))
{
}
/* -----------------------------23.11.00 14:53--------------------------------

 ---------------------------------------------------------------------------*/
/*LanguageConfig_Impl::~LanguageConfig_Impl()
{
    if(aProfile.IsModified())
        aProfile.Commit();
}
/* -----------------------------23.11.00 15:12--------------------------------

 ---------------------------------------------------------------------------*/
/*Sequence<OUString>  LanguageConfig_Impl::GetInstalledLanguages()
{
    Sequence<OUString> aRet;
    aSetup.GetValue() >>= aRet;
    return aRet;
}
/* -----------------------------23.11.00 15:12--------------------------------

 ---------------------------------------------------------------------------*/
/*OUString LanguageConfig_Impl::GetCurrentLanguage()
{
    OUString sRet;
    aProfile.GetValue() >>= sRet;
    return sRet;
}
/* -----------------------------23.11.00 15:12--------------------------------

 ---------------------------------------------------------------------------*/
/*void LanguageConfig_Impl::SetCurrentLanguage(const OUString& rSet)
{
    Any aSet;
    aSet <<= rSet;
    aProfile.SetValue(aSet);
}
/* -----------------------------23.11.00 13:06--------------------------------

 ---------------------------------------------------------------------------*/
static sal_Bool bLanguageCurrentDoc_Impl = sal_False;
OfaLanguagesTabPage::OfaLanguagesTabPage( Window* pParent, const SfxItemSet& rSet ) :
    SfxTabPage( pParent, ResId( OFA_TP_LANGUAGES, DIALOG_MGR() ), rSet ),
    aUILanguageGB(this,         ResId(GB_UI_LANG        )),
//    aUILanguageFT(this,         ResId(FT_UI_LANG        )),
//    aUILanguageLB(this,         ResId(LB_UI_LANG        )),
    aLocaleSettingFT(this,      ResId(FT_LOCALESETTING)),
    aLocaleSettingLB(this,      ResId(LB_LOCALESETTING)),
    aCurrencyFT( this,          ResId(FT_CURRENCY       )),
    aCurrencyLB( this,          ResId(LB_CURRENCY       )),
    aLinguLanguageGB(this,      ResId(GB_LINGU_LANG     )),
    aWesternLanguageFT(this,    ResId(FT_WEST_LANG      )),
    aWesternLanguageLB(this,    ResId(LB_WEST_LANG      )),
    aAsianLanguageFT(this,      ResId(FT_ASIAN_LANG     )),
    aAsianLanguageLB(this,      ResId(LB_ASIAN_LANG     )),
    aComplexLanguageFT(this,    ResId(FT_COMPLEX_LANG   )),
    aComplexLanguageLB(this,    ResId(LB_COMPLEX_LANG   )),
    aCurrentDocCB(this,         ResId(CB_CURRENT_DOC    )),
    aAsianSupportFL(this,       ResId(FL_ASIANSUPPORT    )),
    aAsianSupportCB(this,       ResId(CB_ASIANSUPPORT   )),
    aCTLSupportFL(this,         ResId(FL_CTLSUPPORT    )),
    aCTLSupportCB(this,         ResId(CB_CTLSUPPORT   ))
{
    FreeResource();

    aWesternLanguageLB.SetLanguageList( LANG_LIST_WESTERN | LANG_LIST_ONLY_KNOWN, TRUE,  FALSE, TRUE );
    aAsianLanguageLB  .SetLanguageList( LANG_LIST_CJK     | LANG_LIST_ONLY_KNOWN, TRUE,  FALSE, TRUE );
    aComplexLanguageLB.SetLanguageList( LANG_LIST_CTL     | LANG_LIST_ONLY_KNOWN, TRUE,  FALSE, TRUE );
    aLocaleSettingLB  .SetLanguageList( LANG_LIST_ALL     | LANG_LIST_ONLY_KNOWN, FALSE, FALSE, FALSE);
    aLocaleSettingLB.InsertLanguage( LANGUAGE_SYSTEM );

    SvxLanguageTable* pLanguageTable = new SvxLanguageTable;
    // insert SYSTEM entry, no specific currency
    String aStr( pLanguageTable->GetString( LANGUAGE_SYSTEM ) );
    aCurrencyLB.InsertEntry( aStr );
    // all currencies
    String aTwoSpace( RTL_CONSTASCII_USTRINGPARAM( "  " ) );
    const NfCurrencyTable& rCurrTab = SvNumberFormatter::GetTheCurrencyTable();
    USHORT nCurrCount = rCurrTab.Count();
    // first entry is SYSTEM, skip it
    for ( USHORT j=1; j < nCurrCount; ++j )
    {
        const NfCurrencyEntry* pCurr = rCurrTab[j];
        String aStr( pCurr->GetBankSymbol() );
        aStr += aTwoSpace;
        aStr += pCurr->GetSymbol();
        aStr += aTwoSpace;
        aStr += pLanguageTable->GetString( pCurr->GetLanguage() );
        USHORT nPos = aCurrencyLB.InsertEntry( aStr );
        aCurrencyLB.SetEntryData( nPos, (void*) pCurr );
    }
    delete pLanguageTable;

    aLocaleSettingLB.SetSelectHdl( LINK( this, OfaLanguagesTabPage, LocaleSettingHdl ) );
    Link aLink( LINK( this, OfaLanguagesTabPage, SupportHdl ) );
    aAsianSupportCB.SetClickHdl( aLink );
    aCTLSupportCB.SetClickHdl( aLink );

    SvtLanguageOptions aLanguageOptions;
    aAsianSupportCB.Check( m_bOldAsian = aLanguageOptions.IsAnyEnabled() );
    aAsianSupportCB.SaveValue();
    aAsianSupportCB.Enable(!aLanguageOptions.IsReadOnly(SvtLanguageOptions::E_ALLCJK));
    SupportHdl( &aAsianSupportCB );

    aCTLSupportCB.Check( m_bOldCtl = aLanguageOptions.IsCTLFontEnabled() );
    aCTLSupportCB.SaveValue();
    aCTLSupportCB.Enable(!aLanguageOptions.IsReadOnly(SvtLanguageOptions::E_CTLFONT));
    SupportHdl( &aCTLSupportCB );
}
/*-- 23.11.00 13:06:40---------------------------------------------------

  -----------------------------------------------------------------------*/
OfaLanguagesTabPage::~OfaLanguagesTabPage()
{
//    delete pLangConfig;
}
/*-- 23.11.00 13:06:40---------------------------------------------------

  -----------------------------------------------------------------------*/
SfxTabPage* OfaLanguagesTabPage::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return new OfaLanguagesTabPage(pParent, rAttrSet);
}
/*-- 23.11.00 13:06:41---------------------------------------------------

  -----------------------------------------------------------------------*/
LanguageType lcl_LangStringToLangType(const OUString& rLang)
{
    Locale aLocale;
    aLocale.Language = rLang.copy(0, 2);
    if(rLang.getLength() >= 5)
        aLocale.Country = rLang.copy(3, 2);
    LanguageType eLangType = SvxLocaleToLanguage( aLocale );
    return eLangType;
}

/*-----------------12.02.01 09:01-------------------
 *
 * --------------------------------------------------*/
void lcl_LanguageToLocale( Locale& rLocale, const LanguageType eLang )
{
    OUString aEmpty;
    SvxLanguageToLocale( rLocale, eLang );
    switch ( eLang )
    {
        case LANGUAGE_FRENCH :
        case LANGUAGE_GERMAN :
        case LANGUAGE_ITALIAN :
        case LANGUAGE_DUTCH :
        case LANGUAGE_SPANISH :
        case LANGUAGE_PORTUGUESE :
        case LANGUAGE_DANISH :
        case LANGUAGE_GREEK :
        case LANGUAGE_JAPANESE :
        case LANGUAGE_KOREAN :
        case LANGUAGE_SWEDISH :
        case LANGUAGE_RUSSIAN :
        case LANGUAGE_POLISH :
        case LANGUAGE_TURKISH :
            rLocale.Country = aEmpty;
            break;
    }

}
/*-- 23.11.00 13:06:40---------------------------------------------------

  -----------------------------------------------------------------------*/
void lcl_UpdateAndDelete(SfxVoidItem* pInvalidItems[], SfxBoolItem* pBoolItems[], sal_uInt16 nCount)
{
    SfxViewFrame* pCurrentFrm = SfxViewFrame::Current();
    SfxViewFrame* pViewFrm = SfxViewFrame::GetFirst();
    while(pViewFrm)
    {
        SfxBindings& rBind = pViewFrm->GetBindings();
        for(sal_Int16 i = 0; i < nCount; i++)
        {
            if(pCurrentFrm == pViewFrm)
                rBind.InvalidateAll(sal_False);
            rBind.SetState( *pInvalidItems[i] );
            rBind.SetState( *pBoolItems[i] );
        }
        pViewFrm = SfxViewFrame::GetNext(*pViewFrm);
    }
    for(sal_Int16 i = 0; i < nCount; i++)
    {
        delete pInvalidItems[i];
        delete pBoolItems[i] ;
    }
}

BOOL OfaLanguagesTabPage::FillItemSet( SfxItemSet& rSet )
{
    SvtSysLocaleOptions aSysLocaleOptions;
    aSysLocaleOptions.BlockBroadcasts( TRUE );

    // user interface language
//    OUString sLang = pLangConfig->GetCurrentLanguage();
//    LanguageType eOldLang = lcl_LangStringToLangType( sLang );
//    LanguageType eUILang = aUILanguageLB.GetSelectLanguage();
//    if ( eOldLang != eUILang )
//    {
//        Locale aLocale;
//        // DO use lcl_LanguageToLocale() for country independence
//        lcl_LanguageToLocale( aLocale, eUILang );
//        OUString sNewLang = aLocale.Language;
//        if ( aLocale.Country.getLength() > 0 )
//        {
//            sNewLang += C2U("-");
//            sNewLang += aLocale.Country;
//        }
//        if ( sNewLang.getLength() > 1 )
//            pLangConfig->SetCurrentLanguage( sNewLang );
//    }

    OUString sLang = aSysLocaleOptions.GetLocaleConfigString();
    LanguageType eOldLocale = (sLang.getLength() ?
        lcl_LangStringToLangType( sLang ) : LANGUAGE_SYSTEM);
    LanguageType eNewLocale = aLocaleSettingLB.GetSelectLanguage();
    if ( eOldLocale != eNewLocale )
    {
        // an empty string denotes SYSTEM locale
        OUString sNewLang;
        if ( eNewLocale != LANGUAGE_SYSTEM )
        {
            Locale aLocale;
            // Do NOT use lcl_LanguageToLocale() or locale won't match I18N
            SvxLanguageToLocale( aLocale, eNewLocale );
            sNewLang = aLocale.Language;
            if ( aLocale.Country.getLength() > 0 )
            {
                sNewLang += C2U("-");
                sNewLang += aLocale.Country;
            }
        }
        // Set application settings before options, so listeners at the
        // options will access the new locale.
        AllSettings aSettings( Application::GetSettings() );
        aSettings.SetLanguage( eNewLocale );
        Application::SetSettings( aSettings );
        aSysLocaleOptions.SetLocaleConfigString( sNewLang );
        rSet.Put( SfxBoolItem( SID_OPT_LOCALE_CHANGED, TRUE ) );
    }

    // Configured currency, for example, USD-en-US or EUR-de-DE, or empty for
    // locale default. This must be set _after_ the locale above in order to
    // have a valid locale for broadcasting the currency change.
    OUString sOldCurr = aSysLocaleOptions.GetCurrencyConfigString();
    USHORT nCurrPos = aCurrencyLB.GetSelectEntryPos();
    const NfCurrencyEntry* pCurr = (const NfCurrencyEntry*)
        aCurrencyLB.GetEntryData( nCurrPos );
    OUString sNewCurr;
    if ( pCurr )
        sNewCurr = SvtSysLocaleOptions::CreateCurrencyConfigString(
            pCurr->GetBankSymbol(), pCurr->GetLanguage() );
    if ( sOldCurr != sNewCurr )
        aSysLocaleOptions.SetCurrencyConfigString( sNewCurr );

    BOOL bRet = FALSE;
    SfxObjectShell* pCurrentDocShell = SfxObjectShell::Current();
    Reference< XPropertySet > xLinguProp( LinguMgr::GetLinguPropertySet(), UNO_QUERY );
    SvtLinguConfig  aLinguCfg;
    BOOL bCurrentDocCBChecked = aCurrentDocCB.IsChecked();
    if(aCurrentDocCB.IsEnabled())
        bLanguageCurrentDoc_Impl = bCurrentDocCBChecked;
    BOOL bCurrentDocCBChanged = bCurrentDocCBChecked != aCurrentDocCB.GetSavedValue();

    BOOL bValChanged = aWesternLanguageLB.GetSavedValue() != aWesternLanguageLB.GetSelectEntryPos();
    if( (bCurrentDocCBChanged && !bCurrentDocCBChecked) || bValChanged)
    {
        LanguageType eSelectLang = aWesternLanguageLB.GetSelectLanguage();
        if(!bCurrentDocCBChecked)
        {
            Any aValue;
            Locale aLocale = SvxCreateLocale( eSelectLang );
            aValue <<= aLocale;
            OUString aPropName( C2U("DefaultLocale") );
            aLinguCfg.SetProperty( aPropName, aValue );
            if (xLinguProp.is())
                xLinguProp->setPropertyValue( aPropName, aValue );
        }
        if(pCurrentDocShell)
        {
            rSet.Put(SvxLanguageItem(eSelectLang, SID_ATTR_LANGUAGE));
            bRet = TRUE;
        }
    }
    bValChanged = aAsianLanguageLB.GetSavedValue() != aAsianLanguageLB.GetSelectEntryPos();
    if( (bCurrentDocCBChanged && !bCurrentDocCBChecked) || bValChanged)
    {
        LanguageType eSelectLang = aAsianLanguageLB.GetSelectLanguage();
        if(!bCurrentDocCBChecked)
        {
            Any aValue;
            Locale aLocale = SvxCreateLocale( eSelectLang );
            aValue <<= aLocale;
            OUString aPropName( C2U("DefaultLocale_CJK") );
            aLinguCfg.SetProperty( aPropName, aValue );
            if (xLinguProp.is())
                xLinguProp->setPropertyValue( aPropName, aValue );
        }
        if(pCurrentDocShell)
        {
            rSet.Put(SvxLanguageItem(eSelectLang, SID_ATTR_CHAR_CJK_LANGUAGE));
            bRet = TRUE;
        }
    }
    bValChanged = aComplexLanguageLB.GetSavedValue() != aComplexLanguageLB.GetSelectEntryPos();
    if( (bCurrentDocCBChanged && !bCurrentDocCBChecked) || bValChanged)
    {
        LanguageType eSelectLang = aComplexLanguageLB.GetSelectLanguage();
        if(!bCurrentDocCBChecked)
        {
            Any aValue;
            Locale aLocale = SvxCreateLocale( eSelectLang );
            aValue <<= aLocale;
            OUString aPropName( C2U("DefaultLocale_CTL") );
            aLinguCfg.SetProperty( aPropName, aValue );
            if (xLinguProp.is())
                xLinguProp->setPropertyValue( aPropName, aValue );
        }
        if(pCurrentDocShell)
        {
            rSet.Put(SvxLanguageItem(eSelectLang, SID_ATTR_CHAR_CTL_LANGUAGE));
            bRet = TRUE;
        }
    }

    SvtLanguageOptions aLanguageOptions;

    if(aAsianSupportCB.GetSavedValue() != aAsianSupportCB.IsChecked() )
    {
        sal_Bool bChecked = aAsianSupportCB.IsChecked();
        aLanguageOptions.SetAll(bChecked);
        //iterate over all bindings to invalidate vertical text direction

      const sal_uInt16 STATE_COUNT = 2;

      SfxBoolItem* pBoolItems[STATE_COUNT];
      pBoolItems[0] = new SfxBoolItem(SID_VERTICALTEXT_STATE, FALSE);
      pBoolItems[1] = new SfxBoolItem(SID_TEXT_FITTOSIZE_VERTICAL, FALSE);

      SfxVoidItem* pInvalidItems[STATE_COUNT];
      pInvalidItems[0] = new SfxVoidItem(SID_VERTICALTEXT_STATE);
      pInvalidItems[1] = new SfxVoidItem(SID_TEXT_FITTOSIZE_VERTICAL);

    lcl_UpdateAndDelete(pInvalidItems, pBoolItems, STATE_COUNT);
    }

    if ( aCTLSupportCB.GetSavedValue() != aCTLSupportCB.IsChecked() )
    {
        aLanguageOptions.SetCTLFontEnabled( aCTLSupportCB.IsChecked() );

        const sal_uInt16 STATE_COUNT = 1;
        SfxBoolItem* pBoolItems[STATE_COUNT];
        pBoolItems[0] = new SfxBoolItem(SID_CTLFONT_STATE, FALSE);
        SfxVoidItem* pInvalidItems[STATE_COUNT];
        pInvalidItems[0] = new SfxVoidItem(SID_CTLFONT_STATE);
        lcl_UpdateAndDelete(pInvalidItems, pBoolItems, STATE_COUNT);
    }

    if ( aSysLocaleOptions.IsModified() )
        aSysLocaleOptions.Commit();
    aSysLocaleOptions.BlockBroadcasts( FALSE );

    return FALSE;
}
//-----------------------------------------------------------------------------
void OfaLanguagesTabPage::Reset( const SfxItemSet& rSet )
{
    SvtSysLocaleOptions aSysLocaleOptions;

    //user interface languages
//    Sequence <OUString> aLangs = pLangConfig->GetInstalledLanguages();
//    const OUString* pLangs = aLangs.getConstArray();
//    aUILanguageLB.Clear();
//    for(int i = 0; i < aLangs.getLength(); i++)
//    {
//        if(pLangs[i].getLength() >= 2)
//            aUILanguageLB.InsertLanguage( lcl_LangStringToLangType(pLangs[i]) );
//    }
//    OUString sLang =  pLangConfig->GetCurrentLanguage();
//    aUILanguageLB.SelectLanguage(lcl_LangStringToLangType(sLang));

    OUString sLang = aSysLocaleOptions.GetLocaleConfigString();
    if ( sLang.getLength() )
        aLocaleSettingLB.SelectLanguage(lcl_LangStringToLangType(sLang));
    else
        aLocaleSettingLB.SelectLanguage( LANGUAGE_SYSTEM );
    aLocaleSettingLB.Enable(!aSysLocaleOptions.IsReadOnly(SvtSysLocaleOptions::E_LOCALE));

    // let LocaleSettingHdl enable/disable checkboxes for CJK/CTL support
    // #i15812# must be done *before* the configured currency is set
    LocaleSettingHdl(&aLocaleSettingLB);

    // configured currency, for example, USD-en-US or EUR-de-DE, or empty for locale default
    String aAbbrev;
    LanguageType eLang;
    const NfCurrencyEntry* pCurr = NULL;
    sLang = aSysLocaleOptions.GetCurrencyConfigString();
    if ( sLang.getLength() )
    {
        SvtSysLocaleOptions::GetCurrencyAbbrevAndLanguage( aAbbrev, eLang, sLang );
        pCurr = SvNumberFormatter::GetCurrencyEntry( aAbbrev, eLang );
    }
    // if pCurr==NULL the SYSTEM entry is selected
    USHORT nPos = aCurrencyLB.GetEntryPos( (void*) pCurr );
    aCurrencyLB.SelectEntryPos( nPos );
    aCurrencyLB.Enable(!aSysLocaleOptions.IsReadOnly(SvtSysLocaleOptions::E_CURRENCY));

    //western/CJK/CLK language
    LanguageType eCurLang = LANGUAGE_NONE;
    LanguageType eCurLangCJK = LANGUAGE_NONE;
    LanguageType eCurLangCTL = LANGUAGE_NONE;
    SfxObjectShell* pCurrentDocShell = SfxObjectShell::Current();
    //collect the configuration values first
    aCurrentDocCB.Enable(FALSE);
    //
    SvtLinguConfig  aLinguCfg;
    Any aWestLang;
    Any aCJKLang;
    Any aCTLLang;
    try
    {
        aWestLang = aLinguCfg.GetProperty(C2U("DefaultLocale"));
        Locale aLocale;
        aWestLang >>= aLocale;
        eCurLang = SvxLocaleToLanguage( aLocale );
        aCJKLang = aLinguCfg.GetProperty(C2U("DefaultLocale_CJK"));
        aCJKLang >>= aLocale;
        eCurLangCJK = SvxLocaleToLanguage( aLocale );
        aCTLLang = aLinguCfg.GetProperty(C2U("DefaultLocale_CTL"));
        aCTLLang >>= aLocale;
        eCurLangCTL = SvxLocaleToLanguage( aLocale );
    }
    catch(Exception&)
    {
    }
    //overwrite them by the values provided by the DocShell
    if(pCurrentDocShell)
    {
        aCurrentDocCB.Enable(TRUE);
        aCurrentDocCB.Check(bLanguageCurrentDoc_Impl);
        const SfxPoolItem* pLang;
        if( SFX_ITEM_SET == rSet.GetItemState(SID_ATTR_LANGUAGE, FALSE, &pLang))
            eCurLang = ((const SvxLanguageItem*)pLang)->GetValue();

        if( SFX_ITEM_SET == rSet.GetItemState(SID_ATTR_CHAR_CJK_LANGUAGE, FALSE, &pLang))
            eCurLangCJK = ((const SvxLanguageItem*)pLang)->GetValue();

        if( SFX_ITEM_SET == rSet.GetItemState(SID_ATTR_CHAR_CTL_LANGUAGE, FALSE, &pLang))
            eCurLangCTL = ((const SvxLanguageItem*)pLang)->GetValue();
    }
    if(LANGUAGE_NONE == eCurLang || LANGUAGE_DONTKNOW == eCurLang)
        aWesternLanguageLB.SelectLanguage(LANGUAGE_NONE);
    else
        aWesternLanguageLB.SelectLanguage(eCurLang);

    if(LANGUAGE_NONE == eCurLangCJK || LANGUAGE_DONTKNOW == eCurLangCJK)
        aAsianLanguageLB.SelectLanguage(LANGUAGE_NONE);
    else
        aAsianLanguageLB.SelectLanguage(eCurLangCJK);

    if(LANGUAGE_NONE == eCurLangCTL || LANGUAGE_DONTKNOW == eCurLangCTL)
        aComplexLanguageLB.SelectLanguage(LANGUAGE_NONE);
    else
        aComplexLanguageLB.SelectLanguage(eCurLangCTL);

    aWesternLanguageLB.SaveValue();
    aAsianLanguageLB.SaveValue();
    aComplexLanguageLB.SaveValue();
    aCurrentDocCB.SaveValue();

    sal_Bool bEnable = !aLinguCfg.IsReadOnly( C2U("DefaultLocale") );
    aWesternLanguageFT.Enable( bEnable );
    aWesternLanguageLB.Enable( bEnable );

    // #i15812# controls for CJK/CTL already enabled/disabled from LocaleSettingHdl
#if 0
    bEnable = ( !aLinguCfg.IsReadOnly( C2U("DefaultLocale_CJK") ) && aAsianSupportCB.IsChecked() );
    aAsianLanguageFT.Enable( bEnable );
    aAsianLanguageLB.Enable( bEnable );

    bEnable = ( !aLinguCfg.IsReadOnly( C2U("DefaultLocale_CTL") ) && aCTLSupportCB.IsChecked() );
    aComplexLanguageFT.Enable( bEnable );
    aComplexLanguageLB.Enable( bEnable );
#endif
}
/* -----------------------------20.04.01 15:09--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(  OfaLanguagesTabPage, SupportHdl, CheckBox*, pBox )
{
    DBG_ASSERT( pBox, "OfaLanguagesTabPage::SupportHdl(): pBox invalid" );

    sal_Bool bCheck = pBox->IsChecked();
    if ( &aAsianSupportCB == pBox )
    {
        bCheck = ( bCheck && !SvtLinguConfig().IsReadOnly( C2U("DefaultLocale_CJK") ) );
        aAsianLanguageFT.Enable( bCheck );
        aAsianLanguageLB.Enable( bCheck );
        if( pBox->IsEnabled() )
            m_bOldAsian = bCheck;
    }
    else if ( &aCTLSupportCB == pBox )
    {
        bCheck = ( bCheck && !SvtLinguConfig().IsReadOnly( C2U("DefaultLocale_CTL") ) );
        aComplexLanguageFT.Enable( bCheck );
        aComplexLanguageLB.Enable( bCheck );
        if( pBox->IsEnabled() )
            m_bOldCtl = bCheck;
    }
    else
    {
        DBG_ERRORFILE( "OfaLanguagesTabPage::SupportHdl(): wrong pBox" );
    }

    return 0;
}

namespace
{
    void lcl_checkLanguageCheckBox(CheckBox& _rCB,FixedLine& _rFL,sal_Bool _bNewValue,sal_Bool _bOldValue)
    {
        if ( _bNewValue )
            _rCB.Check(TRUE);
        else
            _rCB.Check( _bOldValue );
// #i15082# do not call SaveValue() in running dialog...
//      _rCB.SaveValue();
        _rCB.Enable( !_bNewValue );
        _rFL.Enable( !_bNewValue );
    }
}
/* -----------------08.06.01 17:56-------------------

 --------------------------------------------------*/
IMPL_LINK( OfaLanguagesTabPage, LocaleSettingHdl, SvxLanguageBox*, pBox )
{
    LanguageType eLang = pBox->GetSelectLanguage();
    sal_uInt16 nType = SvtLanguageOptions::GetScriptTypeOfLanguage(eLang);
    // first check if CTL must be enabled
    sal_Bool bIsCTLFixed = nType & SCRIPTTYPE_COMPLEX;
    lcl_checkLanguageCheckBox(aCTLSupportCB,aCTLSupportFL,bIsCTLFixed,m_bOldCtl);
    SupportHdl( &aCTLSupportCB );
    // second check if CJK must be enabled
    sal_Bool bIsCJKFixed = nType & SCRIPTTYPE_ASIAN;
    lcl_checkLanguageCheckBox(aAsianSupportCB,aAsianSupportFL,bIsCJKFixed,m_bOldAsian);
    SupportHdl( &aAsianSupportCB );

    USHORT nPos;
    if ( eLang == LANGUAGE_SYSTEM )
        nPos = aCurrencyLB.GetEntryPos( (void*) NULL );
    else
    {
        const NfCurrencyEntry* pCurr = &SvNumberFormatter::GetCurrencyEntry( eLang );
        nPos = aCurrencyLB.GetEntryPos( (void*) pCurr );
    }
    aCurrencyLB.SelectEntryPos( nPos );
    return 0;
}
/* -----------------------------21.06.01 09:33--------------------------------

 ---------------------------------------------------------------------------*/
class OfaHelperProgramsCfg_Impl : public utl::ConfigItem
{
    friend class OfaHelperProgramsTabPage;
    OUString sHttp;
    OUString sHttps;
    OUString sFtp;
    OUString sFile;
    OUString sMailto;

    sal_Bool bROHttp;
    sal_Bool bROHttps;
    sal_Bool bROFtp;
    sal_Bool bROFile;
    sal_Bool bROMailto;

    const Sequence<OUString> GetPropertyNames();
public:
    OfaHelperProgramsCfg_Impl();
    virtual ~OfaHelperProgramsCfg_Impl();

    virtual void    Commit();

};
/* -----------------------------21.06.01 09:41--------------------------------

 ---------------------------------------------------------------------------*/
OfaHelperProgramsCfg_Impl::OfaHelperProgramsCfg_Impl() :
        ConfigItem(C2U("Office.Common/ExternalApps")),
        bROHttp(sal_True),
        bROHttps(sal_True),
        bROFtp(sal_True),
        bROFile(sal_True),
        bROMailto(sal_True)
{
    const Sequence< OUString > aNames = GetPropertyNames();
    const Sequence< Any > aValues = GetProperties(aNames);
    const Sequence< sal_Bool > aROStates = GetReadOnlyStates(aNames);

    const Any* pValues = aValues.getConstArray();
    const sal_Bool* pROStates = aROStates.getConstArray();

    for(sal_Int32 nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case 0 :
            {
                pValues[nProp] >>= sHttp;
                bROHttp = pROStates[nProp];
            }
            break;//http
            case 1 :
            {
                pValues[nProp] >>= sHttps;
                bROHttps = pROStates[nProp];
            }
            break;//https
            case 2 :
            {
                pValues[nProp] >>= sFtp;
                bROFtp = pROStates[nProp];
            }
            break;//ftp
            case 3 :
            {
                pValues[nProp] >>= sFile;
                bROFile = pROStates[nProp];
            }
            break;//file
            case 4 :
            {
                pValues[nProp] >>= sMailto;
                bROMailto = pROStates[nProp];
            }
            break;//mailto
        }
    }
}
/* -----------------------------21.06.01 09:39--------------------------------

 ---------------------------------------------------------------------------*/
OfaHelperProgramsCfg_Impl::~OfaHelperProgramsCfg_Impl()
{}
/* -----------------------------21.06.01 09:38--------------------------------

 ---------------------------------------------------------------------------*/
const Sequence<OUString> OfaHelperProgramsCfg_Impl::GetPropertyNames()
{
    Sequence<OUString> aRet(5);
    OUString* pRet = aRet.getArray();
    pRet[0] = C2U("http");
    pRet[1] = C2U("https");
    pRet[2] = C2U("ftp");
    pRet[3] = C2U("file");
    pRet[4] = C2U("mailto");
    return aRet;
}
/* -----------------------------21.06.01 09:45--------------------------------

 ---------------------------------------------------------------------------*/
void    OfaHelperProgramsCfg_Impl::Commit()
{
    const Sequence< OUString > aOrgNames = GetPropertyNames();
    sal_Int32 nOrgCount = aOrgNames.getLength();

    Sequence< OUString > aNames(nOrgCount);
    Sequence< Any > aValues(nOrgCount);
    sal_Int32 nRealCount = 0;

    OUString* pNames = aNames.getArray();
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aOrgNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case  0:
            {
                if (!bROHttp)
                {
                    pNames[nRealCount] = aOrgNames[nProp];
                    pValues[nRealCount] <<= sHttp;
                    ++nRealCount;
                }
            }
            break;//http
            case  1:
            {
                if (!bROHttps)
                {
                    pNames[nRealCount] = aOrgNames[nProp];
                    pValues[nRealCount] <<= sHttps;
                    ++nRealCount;
                }
            }
            break;//https
            case  2:
            {
                if (!bROFtp)
                {
                    pNames[nRealCount] = aOrgNames[nProp];
                    pValues[nRealCount] <<= sFtp;
                    ++nRealCount;
                }
            }
            break;//ftp
            case  3:
            {
                if (!bROFile)
                {
                    pNames[nRealCount] = aOrgNames[nProp];
                    pValues[nRealCount] <<= sFile;
                    ++nRealCount;
                }
            }
            break;//file
            case  4:
            {
                if (!bROMailto)
                {
                    pNames[nRealCount] = aOrgNames[nProp];
                    pValues[nRealCount] <<= sMailto;
                    ++nRealCount;
                }
            }
            break;//mailto
        }
    }
    aNames.realloc(nRealCount);
    aValues.realloc(nRealCount);
    PutProperties(aNames, aValues);
}

/* -----------------------------22.06.01 11:47--------------------------------

 ---------------------------------------------------------------------------*/
class OfaMailProfilesCfg_Impl : public utl::ConfigItem
{
    friend class OfaHelperProgramsTabPage;
    Sequence<OUString> aProfiles;
    // variables
    OUString sCommandProfile;
    OUString sProgram;
    sal_Bool bUseDefaultMailer;
    // readonly states
    sal_Bool bROCommandProfile;
    sal_Bool bROProgram;
    sal_Bool bROUseDefaultMailer;

    const Sequence<OUString> GetPropertyNames();
public:
    OfaMailProfilesCfg_Impl();
    virtual ~OfaMailProfilesCfg_Impl();

    virtual void    Commit();

};
/* -----------------------------22.06.01 11:49--------------------------------

 ---------------------------------------------------------------------------*/
OfaMailProfilesCfg_Impl::OfaMailProfilesCfg_Impl() :
    utl::ConfigItem(C2U("Office.Common/ExternalMailer")),
    bROCommandProfile(sal_False),
    bROProgram(sal_False),
    bROUseDefaultMailer(sal_False),
#ifdef HELPER_PAGE_COMPLETE
    bUseDefaultMailer(sal_False)
#else
    bUseDefaultMailer(sal_True)
#endif
{
    const Sequence< OUString > aNames = GetPropertyNames();
    const Sequence< Any > aValues = GetProperties(aNames);
    const Sequence< sal_Bool > aROStates = GetReadOnlyStates(aNames);
    const Any* pValues = aValues.getConstArray();
    const sal_Bool* pROStates = aROStates.getConstArray();
    for(sal_Int32 nProp = 0; nProp < aValues.getLength(); nProp++)
    {
        if(pValues[nProp].hasValue())
        {
            switch(nProp)
            {
                case 0 :
                {
                    pValues[nProp] >>= sCommandProfile;
                    bROCommandProfile = pROStates[nProp];
                }
                break;
                case 1 :
                {
                    pValues[nProp] >>= sProgram;
                    bROProgram = pROStates[nProp];
                }
                break;
                case 2 :
                {
                    pValues[nProp] >>= bUseDefaultMailer;
                    bROUseDefaultMailer = pROStates[nProp];
                }
                break;
            }
        }
    }
    aProfiles = GetNodeNames(C2U("Profiles"));
}
/* -----------------------------22.06.01 11:50--------------------------------

 ---------------------------------------------------------------------------*/
OfaMailProfilesCfg_Impl::~OfaMailProfilesCfg_Impl()
{
}
/* -----------------------------22.06.01 11:53--------------------------------

 ---------------------------------------------------------------------------*/
const Sequence<OUString> OfaMailProfilesCfg_Impl::GetPropertyNames()
{
    Sequence<OUString> aRet(3);
    OUString* pRet = aRet.getArray();
    pRet[0] = C2U("CommandProfile");
    pRet[1] = C2U("Program");
    pRet[2] = C2U("UseDefaultMailer");
    return aRet;
}
/* -----------------------------22.06.01 11:52--------------------------------

 ---------------------------------------------------------------------------*/
void OfaMailProfilesCfg_Impl::Commit()
{
    const Sequence< OUString > aOrgNames = GetPropertyNames();
    sal_Int32 nOrgCount = aOrgNames.getLength();

    Sequence< OUString > aNames(nOrgCount);
    Sequence< Any > aValues(nOrgCount);
    sal_Int32 nRealCount = 0;

    for(int nProp = 0; nProp < nOrgCount; nProp++)
    {
        switch(nProp)
        {
            case  0:
            {
                if (!bROCommandProfile)
                {
                    aNames[nRealCount] = aOrgNames[nProp];
                    aValues[nRealCount] <<= sCommandProfile;
                    ++nRealCount;
                }
            }
            break;
            case  1:
            {
                if (!bROProgram)
                {
                    aNames[nRealCount] = aOrgNames[nProp];
                    aValues[nRealCount] <<= sProgram;
                    ++nRealCount;
                }
            }
            break;
            case  2:
            {
                if (!bROUseDefaultMailer)
                {
                    aNames[nRealCount] = aOrgNames[nProp];
                    aValues[nRealCount] = makeAny(bUseDefaultMailer);
                    ++nRealCount;
                }
            }
            break;
        }
    }
    aNames.realloc(nRealCount);
    aValues.realloc(nRealCount);
    PutProperties(aNames, aValues);
}
/* -----------------------------21.06.01 09:33--------------------------------

 ---------------------------------------------------------------------------*/
struct OfaHelperProgramsTabPage_Impl
{
    OfaHelperProgramsCfg_Impl aAppConfig;
    OfaMailProfilesCfg_Impl aMailConfig;
};

OfaHelperProgramsTabPage::OfaHelperProgramsTabPage(Window* pParent, const SfxItemSet& rSet) :
    SfxTabPage(pParent, ResId( OFA_TP_HELPERPROG, DIALOG_MGR() ), rSet),
    aHelpFI(this,           ResId(FI_HELP           )),
    aMailFL(this,           ResId(FL_MAIL           )),
    aProgramFT(this,        ResId(FT_PROGRAM        )),
    aAdditionalMailFT(this, ResId(FT_ADD_MAIL       )),
    aProfilesLB(this,       ResId(LB_PROFILES       )),
    aMailerURLED(this,      ResId(ED_MAILERURL      )),
    aMailerURLPB(this,      ResId(PB_MAILERURL      )),
    aLinkFL(this,           ResId(FL_LINK           )),
    aHTTPFT(this,           ResId(FT_HTTP           )),
    aHTTPED(this,           ResId(ED_HTTP           )),
    aHTTPPB(this,           ResId(PB_HTTP           )),
    aHTTPSFT(this,          ResId(FT_HTTPS          )),
    aHTTPSED(this,          ResId(ED_HTTPS          )),
    aHTTPSPB(this,          ResId(PB_HTTPS          )),
    aFTPFT(this,            ResId(FT_FTP            )),
    aFTPED(this,            ResId(ED_FTP            )),
    aFTPPB(this,            ResId(PB_FTP            )),
    aMailerFT(this,         ResId(FT_MAILER         )),
    aMailerED(this,         ResId(ED_MAILER         )),
    aMailerPB(this,         ResId(PB_MAILER         )),
    aDocManagerFL(this,     ResId(FL_DOCMANAGER     )),
    aDocManagerFT(this,     ResId(FT_DOCMANAGER     )),
    aDocManagerED(this,     ResId(ED_DOCMANAGER     )),
    aDocManagerPB(this,     ResId(PB_DOCMANAGER     )),
    m_sMozilla(             ResId(STR_MOZILLA_PROGRAM_NAME  )),
    m_sNetscape(            ResId(STR_NETSCAPE_PROGRAM_NAME )),
    m_sDefaultFilterName(   ResId(STR_DEFAULT_FILENAME      )),
    pImpl(new OfaHelperProgramsTabPage_Impl)
{
    FreeResource();

    Link aLink(LINK(this, OfaHelperProgramsTabPage, FileDialogHdl_Impl));
    aMailerURLPB.SetClickHdl(aLink);
    aHTTPPB.SetClickHdl(aLink);
    aHTTPSPB.SetClickHdl(aLink);
    aFTPPB.SetClickHdl(aLink);
    aMailerPB.SetClickHdl(aLink);
#ifdef HELPER_PAGE_COMPLETE
    aDocManagerPB.SetClickHdl(aLink);
#else
    aDocManagerPB.Hide();
#endif

    aProfilesLB.SetSelectHdl(LINK(this, OfaHelperProgramsTabPage, ProfileHdl_Impl));
}
/* -----------------------------21.06.01 07:46--------------------------------

 ---------------------------------------------------------------------------*/
OfaHelperProgramsTabPage::~OfaHelperProgramsTabPage()
{
    delete pImpl;
}
/* -----------------------------21.06.01 07:46--------------------------------

 ---------------------------------------------------------------------------*/
SfxTabPage*  OfaHelperProgramsTabPage::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return new OfaHelperProgramsTabPage(pParent, rAttrSet);
}
/* -----------------------------21.06.01 07:46--------------------------------

 ---------------------------------------------------------------------------*/
BOOL OfaHelperProgramsTabPage::FillItemSet( SfxItemSet& rSet )
{
    BOOL bMailModified = FALSE;
    if(!pImpl->aMailConfig.bROProgram && aMailerURLED.GetSavedValue() != aMailerURLED.GetText())
    {
        pImpl->aMailConfig.sProgram = aMailerURLED.GetText();
        bMailModified = TRUE;
    }
    if(!pImpl->aMailConfig.bROCommandProfile && aProfilesLB.GetSelectEntryPos() != aProfilesLB.GetSavedValue())
    {
        pImpl->aMailConfig.sCommandProfile = aProfilesLB.GetSelectEntry();
#ifndef HELPER_PAGE_COMPLETE
        pImpl->aMailConfig.bUseDefaultMailer = aProfilesLB.GetSelectEntryPos() == 0;
#endif // HELPER_PAGE_COMPLETE
        bMailModified = TRUE;
    }
    if ( bMailModified )
        pImpl->aMailConfig.Commit();


    BOOL bAppModified = FALSE;
    if(!pImpl->aAppConfig.bROHttp && aHTTPED.GetSavedValue() != aHTTPED.GetText())
    {
        pImpl->aAppConfig.sHttp = aHTTPED.GetText();
        bAppModified = TRUE;
    }
    if(!pImpl->aAppConfig.bROHttps && aHTTPSED.GetSavedValue() != aHTTPSED.GetText())
    {
        pImpl->aAppConfig.sHttps = aHTTPSED.GetText();
        bAppModified = TRUE;
    }
    if(!pImpl->aAppConfig.bROFtp && aFTPED.GetSavedValue() != aFTPED.GetText())
    {
        pImpl->aAppConfig.sFtp = aFTPED.GetText();
        bAppModified = TRUE;
    }
    if(!pImpl->aAppConfig.bROMailto && aMailerED.GetSavedValue() != aMailerED.GetText())
    {
        pImpl->aAppConfig.sMailto = aMailerED.GetText();
        bAppModified = TRUE;
    }
#ifdef HELPER_PAGE_COMPLETE
    if(!pImpl->aAppConfig.bROFile && aDocManagerED.GetSavedValue() != aDocManagerED.GetText())
    {
        pImpl->aAppConfig.sFile = aDocManagerED.GetText();
        bAppModified = TRUE;
    }
#endif
    if(bAppModified)
    {
        pImpl->aAppConfig.Commit();
    }
    return FALSE;
}
/* -----------------------------21.06.01 07:46--------------------------------

 ---------------------------------------------------------------------------*/
void OfaHelperProgramsTabPage::Reset( const SfxItemSet& rSet )
{
    aProfilesLB.Enable (FALSE);
    aMailerURLED.Enable(TRUE );
    aMailerURLPB.Enable(TRUE );

#ifdef HELPER_PAGE_COMPLETE
    aProfilesLB.RemoveEntry(0);
#endif // HELPER_PAGE_COMPLETE

    const OUString* pProfiles   = pImpl->aMailConfig.aProfiles.getConstArray();
    const OUString* pEnd        = pProfiles + pImpl->aMailConfig.aProfiles.getLength();
    for(; pProfiles != pEnd ; ++pProfiles)
    {
        String sProfile(*pProfiles);
        if ( LISTBOX_ENTRY_NOTFOUND == aProfilesLB.GetEntryPos(sProfile) )
            aProfilesLB.InsertEntry(sProfile);
    }
#ifndef HELPER_PAGE_COMPLETE
    if ( !pImpl->aMailConfig.bROUseDefaultMailer && pImpl->aMailConfig.bUseDefaultMailer )
        aProfilesLB.SelectEntryPos(0);
    else
#endif //HELPER_PAGE_COMPLETE
        aProfilesLB.SelectEntry(pImpl->aMailConfig.sCommandProfile);

    if ( !pImpl->aMailConfig.bROCommandProfile )
    {
        aProfilesLB.Enable(TRUE);
#ifndef HELPER_PAGE_COMPLETE
        if ( 0 == aProfilesLB.GetSelectEntryPos() )
        {
            aMailerURLED.Enable(FALSE);
            aMailerURLPB.Enable(FALSE);
        }
#endif // HELPER_PAGE_COMPLETE
    }

    aMailerURLED.SetText(pImpl->aMailConfig.sProgram);
    aMailerURLED.SaveValue();
    aProfilesLB.SaveValue();
    aMailerURLED.Enable(!pImpl->aMailConfig.bROProgram);
    aMailerURLPB.Enable(!pImpl->aMailConfig.bROProgram);
    aProgramFT.Enable  (!pImpl->aMailConfig.bROProgram);

    aMailFL.Enable(aProfilesLB.IsEnabled() ||
                   aProgramFT.IsEnabled() ||
                   aMailerURLED.IsEnabled() ||
                   aMailerURLPB.IsEnabled());

    aAdditionalMailFT.Enable(aProfilesLB.IsEnabled() ||
                             aProgramFT.IsEnabled() ||
                             aMailerURLED.IsEnabled() ||
                             aMailerURLPB.IsEnabled());

    aHTTPED.SetText(pImpl->aAppConfig.sHttp);
    aHTTPSED.SetText(pImpl->aAppConfig.sHttps);
    aFTPED.SetText(pImpl->aAppConfig.sFtp);
    aMailerED.SetText(pImpl->aAppConfig.sMailto);
#ifdef HELPER_PAGE_COMPLETE
    aDocManagerED.SetText(pImpl->aAppConfig.sFile);
#endif
    aHTTPED.SaveValue();
    aHTTPSED.SaveValue();
    aFTPED.SaveValue();
    aMailerED.SaveValue();
#ifdef HELPER_PAGE_COMPLETE
    aDocManagerED.SaveValue();
#endif

    aHTTPED.Enable(!pImpl->aAppConfig.bROHttp);
    aHTTPSED.Enable(!pImpl->aAppConfig.bROHttps);
    aFTPED.Enable(!pImpl->aAppConfig.bROFtp);
    aMailerED.Enable(!pImpl->aAppConfig.bROMailto);
#ifdef HELPER_PAGE_COMPLETE
    aDocManagerED.Enable(!pImpl->aAppConfig.bROFile);
#else
    aDocManagerED.Hide();
#endif

    aHTTPFT.Enable(aHTTPED.IsEnabled());
    aHTTPSFT.Enable(aHTTPSED.IsEnabled());
    aFTPFT.Enable(aFTPED.IsEnabled());
    aMailerFT.Enable(aMailerED.IsEnabled());
#ifdef HELPER_PAGE_COMPLETE
    aDocManagerFT.Enable(aDocManagerED.IsEnabled());
#else
    aDocManagerFT.Hide();
#endif

    aHTTPPB.Enable(aHTTPED.IsEnabled());
    aHTTPSPB.Enable(aHTTPSED.IsEnabled());
    aFTPPB.Enable(aFTPED.IsEnabled());
    aMailerPB.Enable(aMailerED.IsEnabled());
#ifdef HELPER_PAGE_COMPLETE
    aDocManagerPB.Enable(aDocManagerED.IsEnabled());
#endif

    aLinkFL.Enable(aHTTPED.IsEnabled() || aHTTPSED.IsEnabled() || aFTPED.IsEnabled() || aMailerED.IsEnabled());
#ifdef HELPER_PAGE_COMPLETE
    aDocManagerFL.Enable(aDocManagerED.IsEnabled());
#else
    aLinkFL.Hide();
    aHTTPFT.Hide();
    aHTTPED.Hide();
    aHTTPPB.Hide();
    aHTTPSFT.Hide();
    aHTTPSED.Hide();
    aHTTPSPB.Hide();
    aFTPFT.Hide();
    aFTPED.Hide();
    aFTPPB.Hide();
    aMailerFT.Hide();
    aMailerED.Hide();
    aMailerPB.Hide();

    aDocManagerFL.Hide();
#endif

    // check if the controls are correctly enabled
    ProfileHdl_Impl(&aProfilesLB);
}
/* -----------------------------21.06.01 07:46--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(  OfaHelperProgramsTabPage, FileDialogHdl_Impl, PushButton*, pButton )
{
    Edit* pEdit = NULL;
    String sFilterName,sExtension,sPath;
    if ( &aMailerURLPB == pButton && !pImpl->aMailConfig.bROProgram )
    {
        sPath = aMailerURLED.GetText();
        USHORT nEntryPos = aProfilesLB.GetSelectEntryPos();
#ifdef HELPER_PAGE_COMPLETE
        // in case of HELPER_PAGE_COMPLETE we have lost the first entry!
        if ( nEntryPos == 1 || nEntryPos == 2 )
#else
        if ( nEntryPos == 2 || nEntryPos == 3 )
#endif // HELPER_PAGE_COMPLETE
        {
            sFilterName = m_sMozilla;
#ifdef HELPER_PAGE_COMPLETE
            if ( !sPath.Len() )
                sPath.AppendAscii("/opt/mozilla");
#endif // HELPER_PAGE_COMPLETE
        }
        else
        {
            sFilterName = m_sNetscape;
#ifdef HELPER_PAGE_COMPLETE
            if ( !sPath.Len() )
                sPath.AppendAscii("/opt/netscape");
#endif // HELPER_PAGE_COMPLETE
        }
#ifdef HELPER_PAGE_COMPLETE
        sExtension.AppendAscii("*");
#else
        sExtension.AppendAscii("*.exe");
#endif // HELPER_PAGE_COMPLETE
        pEdit = &aMailerURLED;
    }
    else if ( &aHTTPPB == pButton && !pImpl->aAppConfig.bROHttp )
        pEdit = &aHTTPED;
    else if ( &aHTTPSPB == pButton && !pImpl->aAppConfig.bROHttps )
        pEdit = &aHTTPSED;
    else if ( &aFTPPB == pButton && !pImpl->aAppConfig.bROFtp )
        pEdit = &aFTPED;
    else if ( &aMailerPB == pButton && !pImpl->aAppConfig.bROMailto )
        pEdit = &aMailerED;
#ifdef HELPER_PAGE_COMPLETE
    else if ( &aDocManagerPB == pButton && !pImpl->aAppConfig.bROFile )
        pEdit = &aDocManagerED;
#endif

    if ( pEdit != NULL )
    {
        FileDialogHelper aHelper( FILEOPEN_SIMPLE, WB_OPEN );
    String sUrl;
        if ( !sPath.Len() )
            sPath = pEdit->GetText();
        ::utl::LocalFileHelper::ConvertPhysicalNameToURL(sPath,sUrl);
        aHelper.SetDisplayDirectory(sUrl);
        if ( sFilterName.Len() )
            aHelper.AddFilter(sFilterName,sExtension);
        else
            aHelper.AddFilter(  m_sDefaultFilterName,
#ifdef HELPER_PAGE_COMPLETE
                                String::CreateFromAscii("*")
#else
                                String::CreateFromAscii("*.*")
#endif // HELPER_PAGE_COMPLETE
                                );
        if ( ERRCODE_NONE == aHelper.Execute() )
    {
        sUrl = aHelper.GetPath();
        ::utl::LocalFileHelper::ConvertURLToPhysicalName(sUrl,sPath);
            pEdit->SetText(sPath);
    }
    }
    return 0;
}
/* -----------------------------21.06.01 07:46--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(  OfaHelperProgramsTabPage, ProfileHdl_Impl, ListBox*, pLB )
{
#ifdef HELPER_PAGE_COMPLETE
    BOOL bEnable = TRUE;
#else
    BOOL bEnable = aProfilesLB.GetSelectEntryPos() != 0;
#endif // HELPER_PAGE_COMPLETE
    bEnable = bEnable && !pImpl->aMailConfig.bROProgram;

    aProfilesLB.Enable(!pImpl->aMailConfig.bROCommandProfile);

    aAdditionalMailFT.Enable(bEnable);
    aMailerURLED.Enable(bEnable);
    aMailerURLPB.Enable(bEnable);

    return 0;
}
// -----------------------------------------------------------------------------
