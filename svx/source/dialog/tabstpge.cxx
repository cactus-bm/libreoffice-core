/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tabstpge.cxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 04:42:50 $
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

// include ---------------------------------------------------------------

#define ITEMID_TABSTOP  0
#define ITEMID_LRSPACE  0
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif

#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif

#ifndef _RULER_HXX //autogen
#include <svtools/ruler.hxx>
#endif

#define _SVX_TABSTPGE_CXX

#include "dialogs.hrc"
#include "tabstpge.hrc"

#include "lrspitem.hxx"
#include "tabstpge.hxx"
#include "dialmgr.hxx"
#include "dlgutil.hxx"

#ifndef _SFXMODULE_HXX
#include <sfx2/module.hxx>
#endif
#ifndef _SVTOOLS_CJKOPTIONS_HXX
#include <svtools/cjkoptions.hxx>
#endif

#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#include <sfx2/request.hxx> //CHINA001
#include <svxids.hrc> //add CHINA001
#include <svtools/intitem.hxx> //add CHINA001


// class TabWin_Impl -----------------------------------------------------

class TabWin_Impl : public Window
{
private:
    USHORT  nTabStyle;

public:
    TabWin_Impl( Window* pParent, const ResId& rId, USHORT nStyle ) :
        Window( pParent, rId ),
        nTabStyle( nStyle ) {}

    virtual void    Paint( const Rectangle& rRect );
};

// static ----------------------------------------------------------------

static USHORT pRanges[] =
{
    SID_ATTR_TABSTOP,
    SID_ATTR_TABSTOP_OFFSET,
    0
};

// C-Funktion ------------------------------------------------------------

void FillUpWithDefTabs_Impl( long nDefDist, SvxTabStopItem& rTabs )
{
    if( rTabs.Count() )
        return;
    {
        SvxTabStop aSwTabStop( nDefDist, SVX_TAB_ADJUST_DEFAULT );
        rTabs.Insert( aSwTabStop );
    }
}

// class TabWin_Impl -----------------------------------------------------

void TabWin_Impl::Paint( const Rectangle& )
{
    // Tabulatoren malen
    Point aPnt;
    Size aSize = GetOutputSizePixel();
    aPnt.X() = aSize.Width() / 2;
    aPnt.Y() = aSize.Height() / 2;
    Ruler::DrawTab( this, aPnt, nTabStyle );
}

// class SvxTabulatorTabPage ---------------------------------------------

SvxTabulatorTabPage::SvxTabulatorTabPage( Window* pParent,
                                          const SfxItemSet& rAttr ):

    SfxTabPage( pParent, ResId( RID_SVXPAGE_TABULATOR, DIALOG_MGR() ), rAttr ),

    aTabBox         ( this, ResId( ED_TABPOS ) ),
    aTabLabel       ( this, ResId( FL_TABPOS ) ),
    aTabLabelVert   ( this, ResId( FL_TABPOS_VERT ) ),
    aLeftTab        ( this, ResId( BTN_TABTYPE_LEFT ) ),
    aRightTab       ( this, ResId( BTN_TABTYPE_RIGHT ) ),
    aCenterTab      ( this, ResId( BTN_TABTYPE_CENTER ) ),
    aDezTab         ( this, ResId( BTN_TABTYPE_DECIMAL ) ),
    pLeftWin        ( new TabWin_Impl( this, ResId( WIN_TABLEFT ), RULER_TAB_LEFT|WB_HORZ ) ),
    pRightWin       ( new TabWin_Impl( this, ResId( WIN_TABRIGHT ), RULER_TAB_RIGHT|WB_HORZ ) ),
    pCenterWin      ( new TabWin_Impl( this, ResId( WIN_TABCENTER ), RULER_TAB_CENTER|WB_HORZ ) ),
    pDezWin         ( new TabWin_Impl( this, ResId( WIN_TABDECIMAL ), RULER_TAB_DECIMAL|WB_HORZ ) ),
    aDezCharLabel   ( this, ResId( FT_TABTYPE_DECCHAR ) ),
    aDezChar        ( this, ResId( ED_TABTYPE_DECCHAR ) ),
    aTabTypeLabel   ( this, ResId( FL_TABTYPE ) ),
    aNoFillChar     ( this, ResId( BTN_FILLCHAR_NO ) ),
    aFillPoints     ( this, ResId( BTN_FILLCHAR_POINTS ) ),
    aFillDashLine   ( this, ResId( BTN_FILLCHAR_DASHLINE ) ),
    aFillSolidLine  ( this, ResId( BTN_FILLCHAR_UNDERSCORE ) ),
    aFillSpecial    ( this, ResId( BTN_FILLCHAR_OTHER ) ),
    aFillChar       ( this, ResId( ED_FILLCHAR_OTHER ) ),
    aFillLabel      ( this, ResId( FL_FILLCHAR ) ),
    aNewBtn         ( this, ResId( BTN_NEW ) ),
    aDelAllBtn      ( this, ResId( BTN_DELALL ) ),
    aDelBtn         ( this, ResId( BTN_DEL ) ),

    aAktTab     ( 0 ),
    aNewTabs    ( 0, 0, SVX_TAB_ADJUST_LEFT, GetWhich( SID_ATTR_TABSTOP ) ),
    nDefDist    ( 0 ),
    eDefUnit( FUNIT_100TH_MM ),
    bCheck      ( FALSE )

{
    SvtCJKOptions aCJKOptions;
    if(aCJKOptions.IsAsianTypographyEnabled())
    {
        aLeftTab  .SetText(String(ResId(   ST_LEFTTAB_ASIAN )));
        aRightTab .SetText(String(ResId(   ST_RIGHTTAB_ASIAN )));
    }

    // diese Page braucht ExchangeSupport
    SetExchangeSupport();


    // Metrik einstellen
    FieldUnit eFUnit = GetModuleFieldUnit( &rAttr );
    SetFieldUnit( aTabBox, eFUnit );

    // Buttons initialisieren
    aNewBtn.SetClickHdl( LINK( this,SvxTabulatorTabPage, NewHdl_Impl ) );
    aDelBtn.SetClickHdl( LINK( this,SvxTabulatorTabPage, DelHdl_Impl ) );
    aDelAllBtn.SetClickHdl( LINK( this,SvxTabulatorTabPage, DelAllHdl_Impl ) );

    Link aLink = LINK( this, SvxTabulatorTabPage, TabTypeCheckHdl_Impl );
    aLeftTab.SetClickHdl( aLink );
    aRightTab.SetClickHdl( aLink );
    aDezTab.SetClickHdl( aLink );
    aCenterTab.SetClickHdl( aLink );

    aDezChar.SetLoseFocusHdl( LINK( this,  SvxTabulatorTabPage, GetDezCharHdl_Impl ) );
    aDezChar.SetMaxTextLen(1);
    aDezChar.Disable();
    aDezCharLabel.Disable();

    aLink = LINK( this, SvxTabulatorTabPage, FillTypeCheckHdl_Impl );
    aNoFillChar.SetClickHdl( aLink );
    aFillPoints.SetClickHdl( aLink );
    aFillDashLine.SetClickHdl( aLink );
    aFillSolidLine.SetClickHdl( aLink );
    aFillSpecial.SetClickHdl( aLink );
    aFillChar.SetLoseFocusHdl( LINK( this,  SvxTabulatorTabPage, GetFillCharHdl_Impl ) );
    aFillChar.SetMaxTextLen(1);
    aFillChar.Disable();

    aTabBox.SetDoubleClickHdl( LINK( this, SvxTabulatorTabPage, SelectHdl_Impl ) );
    aTabBox.SetModifyHdl( LINK( this, SvxTabulatorTabPage, ModifyHdl_Impl ) );

    // das Default-Dezimalzeichen vom System holen
    LocaleDataWrapper aLocaleWrapper( ::comphelper::getProcessServiceFactory(), Application::GetSettings().GetLocale() );
    aAktTab.GetDecimal() = aLocaleWrapper.getNumDecimalSep().GetChar(0);
    FreeResource();
}

// -----------------------------------------------------------------------

SvxTabulatorTabPage::~SvxTabulatorTabPage()
{
    delete pLeftWin;
    delete pRightWin;
    delete pCenterWin;
    delete pDezWin;
}

// -----------------------------------------------------------------------

USHORT* SvxTabulatorTabPage::GetRanges()
{
    return pRanges;
}

// -----------------------------------------------------------------------

BOOL SvxTabulatorTabPage::FillItemSet( SfxItemSet& rSet )
{
    BOOL bModified = FALSE;

    // Hier die Werte aus den Controls eintueten
    if ( aNewBtn.IsEnabled() )
        NewHdl_Impl( 0 );

    // call at first the LoseFocus-Handler
    GetDezCharHdl_Impl( &aDezChar );
    GetFillCharHdl_Impl( &aFillChar );

    FillUpWithDefTabs_Impl( nDefDist, aNewTabs );
    SfxItemPool* pPool = rSet.GetPool();
    MapUnit eUnit = (MapUnit)pPool->GetMetric( GetWhich( SID_ATTR_TABSTOP ) );
    const SfxPoolItem* pOld = GetOldItem( rSet, SID_ATTR_TABSTOP );

    if ( MAP_100TH_MM != eUnit )
    {
        // Wenn sich im ItemSet ein LRSpaceItem mit negativen Erstzeileneinzug
        // befindet, muss im TabStopItem auf der Position 0 ein DefTab sein.
        const SfxPoolItem* pLRSpace;
        // wenn nicht im neuen Set, dann vielleicht im alten
        if ( SFX_ITEM_SET !=
             rSet.GetItemState( GetWhich( SID_ATTR_LRSPACE ), TRUE, &pLRSpace ) )
            pLRSpace = GetOldItem( rSet, SID_ATTR_LRSPACE );

        if ( pLRSpace && ( (SvxLRSpaceItem*)pLRSpace )->GetTxtFirstLineOfst() < 0 )
        {
            SvxTabStop aNull( 0, SVX_TAB_ADJUST_DEFAULT );
            aNewTabs.Insert( aNull );
        }

        SvxTabStopItem aTmp( aNewTabs );
        aTmp.Remove( 0, aTmp.Count() );

        for ( USHORT i = 0; i < aNewTabs.Count(); ++i )
        {
            SvxTabStop aTmpStop = aNewTabs[i];
            aTmpStop.GetTabPos() =
                LogicToLogic( aTmpStop.GetTabPos(), MAP_100TH_MM, eUnit );
            aTmp.Insert( aTmpStop );
        }

        if ( !pOld || !( *( (SvxTabStopItem*)pOld ) == aTmp ) )
        {
            rSet.Put( aTmp );
            bModified = TRUE;
        }
    }
    else if ( !pOld || !( *( (SvxTabStopItem*)pOld ) == aNewTabs ) )
    {
        rSet.Put( aNewTabs );
        bModified = TRUE;
    }
    return bModified;
}

// -----------------------------------------------------------------------

SfxTabPage* SvxTabulatorTabPage::Create( Window* pParent,
                                         const SfxItemSet& rSet)
{
    return ( new SvxTabulatorTabPage( pParent, rSet ) );
}

// -----------------------------------------------------------------------

void SvxTabulatorTabPage::Reset( const SfxItemSet& rSet )
{
    SfxItemPool* pPool = rSet.GetPool();
    MapUnit eUnit = (MapUnit)pPool->GetMetric( GetWhich( SID_ATTR_TABSTOP ) );

    // Aktuelle Tabs
    const SfxPoolItem* pItem = GetItem( rSet, SID_ATTR_TABSTOP );

    if ( pItem )
    {
        if ( MAP_100TH_MM != eUnit )
        {
            SvxTabStopItem aTmp( *( (const SvxTabStopItem*)pItem ) );
            aNewTabs.Remove( 0, aNewTabs.Count() );

            for ( USHORT i = 0; i < aTmp.Count(); ++i )
            {
                SvxTabStop aTmpStop = aTmp[i];
                aTmpStop.GetTabPos() =
                    LogicToLogic( aTmpStop.GetTabPos(), eUnit, MAP_100TH_MM );
                aNewTabs.Insert( aTmpStop );
            }
        }
        else
            aNewTabs = *( (const SvxTabStopItem*)pItem );
    }
    else
        aNewTabs.Remove( 0, aNewTabs.Count() );

    // Default-Tab - Abstand
    nDefDist = SVX_TAB_DEFDIST;
    pItem = GetItem( rSet, SID_ATTR_TABSTOP_DEFAULTS );

    if ( pItem )
        nDefDist = LogicToLogic(
            (long)((const SfxUInt16Item*)pItem)->GetValue(), eUnit, MAP_100TH_MM );

    // Aktuell selektierte Tab-Pos
    USHORT nTabPos = 0;
    pItem = GetItem( rSet, SID_ATTR_TABSTOP_POS );

    if ( pItem )
        nTabPos = ( (const SfxUInt16Item*)pItem )->GetValue();

    InitTabPos_Impl( nTabPos );
}

// -----------------------------------------------------------------------

void SvxTabulatorTabPage::DisableControls( const USHORT nFlag )
{
    if ( ( TABTYPE_LEFT & nFlag ) == TABTYPE_LEFT )
    {
        aLeftTab.Disable();
        pLeftWin->Disable();
    }
    if ( ( TABTYPE_RIGHT & nFlag ) == TABTYPE_RIGHT )
    {
        aRightTab.Disable();
        pRightWin->Disable();
    }
    if ( ( TABTYPE_CENTER & nFlag ) == TABTYPE_CENTER )
    {
        aCenterTab.Disable();
        pCenterWin->Disable();
    }
    if ( ( TABTYPE_DEZIMAL & nFlag ) == TABTYPE_DEZIMAL )
    {
        aDezTab.Disable();
        pDezWin->Disable();
        aDezCharLabel.Disable();
        aDezChar.Disable();
    }
    if ( ( TABTYPE_ALL & nFlag ) == TABTYPE_ALL )
        aTabTypeLabel.Disable();
    if ( ( TABFILL_NONE & nFlag ) == TABFILL_NONE )
        aNoFillChar.Disable();
    if ( ( TABFILL_POINT & nFlag ) == TABFILL_POINT )
        aFillPoints.Disable();
    if ( ( TABFILL_DASHLINE & nFlag ) == TABFILL_DASHLINE )
        aFillDashLine.Disable();
    if ( ( TABFILL_SOLIDLINE & nFlag ) == TABFILL_SOLIDLINE )
        aFillSolidLine.Disable();
    if ( ( TABFILL_SPECIAL & nFlag ) == TABFILL_SPECIAL )
    {
        aFillSpecial.Disable();
        aFillChar.Disable();
    }
    if ( ( TABFILL_ALL & nFlag ) == TABFILL_ALL )
        aFillLabel.Disable();
}

// -----------------------------------------------------------------------

int SvxTabulatorTabPage::DeactivatePage( SfxItemSet* _pSet )
{
    if ( _pSet )
        FillItemSet( *_pSet );
    return LEAVE_PAGE;
}

// -----------------------------------------------------------------------

void SvxTabulatorTabPage::InitTabPos_Impl( USHORT nTabPos )
{
    aTabBox.Clear();

    long nOffset = 0;
    const SfxPoolItem* pItem = 0;
    if ( GetItemSet().GetItemState( SID_ATTR_TABSTOP_OFFSET, TRUE, &pItem )
            == SFX_ITEM_SET )
    {
        nOffset = ( (const SfxInt32Item*)pItem )->GetValue();
        MapUnit eUnit = (MapUnit)GetItemSet().GetPool()->GetMetric( GetWhich( SID_ATTR_TABSTOP ) );
        nOffset = OutputDevice::LogicToLogic( nOffset, eUnit, MAP_100TH_MM  );
    }

    // Aktuelle TabPos korrigieren und Defaults-Tabs
    for ( USHORT i = 0; i < aNewTabs.Count(); i++ )
    {
        if ( aNewTabs[i].GetAdjustment() != SVX_TAB_ADJUST_DEFAULT )
        {
            aTabBox.InsertValue( aTabBox.Normalize(
                aNewTabs[i].GetTabPos() + nOffset ), eDefUnit );
        }
        else
            aNewTabs.Remove( i-- );
    }

    // aktuellen Tabulator auswaehlen
    const USHORT nSize = aNewTabs.Count();

    if ( nTabPos >= nSize )
        nTabPos = 0;

    // alle RadioButtons erstmal ausschalten
    aLeftTab.Check( TRUE );
    aNoFillChar.Check( TRUE );

    if( aTabBox.GetEntryCount() > 0 )
    {
        aTabBox.SetText( aTabBox.GetEntry( nTabPos ) );
        aAktTab = aNewTabs[nTabPos];

        SetFillAndTabType_Impl();
        aNewBtn.Disable();
        aDelBtn.Enable();
    }
    else
    {   // kein Eintrag dann ist 0 der Default-Wert
        aTabBox.SetValue( 0, eDefUnit );

        aNewBtn.Enable();
        aDelBtn.Disable();
    }
}

// -----------------------------------------------------------------------

void SvxTabulatorTabPage::SetFillAndTabType_Impl()
{
    RadioButton* pTypeBtn = 0;
    RadioButton* pFillBtn = 0;

    aDezChar.Disable();
    aDezCharLabel.Disable();

    if ( aAktTab.GetAdjustment() == SVX_TAB_ADJUST_LEFT )
        pTypeBtn = &aLeftTab;
    else if ( aAktTab.GetAdjustment() == SVX_TAB_ADJUST_RIGHT )
        pTypeBtn = &aRightTab;
    else if ( aAktTab.GetAdjustment() == SVX_TAB_ADJUST_DECIMAL )
    {
        pTypeBtn = &aDezTab;
        aDezChar.Enable();
        aDezCharLabel.Enable();
        aDezChar.SetText( String( (sal_Unicode)aAktTab.GetDecimal() ) );
    }
    else if ( aAktTab.GetAdjustment() == SVX_TAB_ADJUST_CENTER )
        pTypeBtn = &aCenterTab;

    if ( pTypeBtn )
        pTypeBtn->Check();

    aFillChar.Disable();
    aFillChar.SetText( String() );

    if ( aAktTab.GetFill() == ' ' )
        pFillBtn = &aNoFillChar;
    else if ( aAktTab.GetFill() == '-' )
        pFillBtn = &aFillDashLine;
    else if ( aAktTab.GetFill() == '_' )
        pFillBtn = &aFillSolidLine;
    else if ( aAktTab.GetFill() == '.' )
        pFillBtn = &aFillPoints;
    else
    {
        pFillBtn = &aFillSpecial;
        aFillChar.Enable();
        aFillChar.SetText( String( (sal_Unicode)aAktTab.GetFill() ) );
    }
    pFillBtn->Check();
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, NewHdl_Impl, Button *, pBtn )
{
    // Einen neuen Hinzufuegen und Selectieren
    // Wert aus der Anzeige holen
    long nVal = aTabBox.Denormalize( aTabBox.GetValue( eDefUnit ) );

    // Wenn der pBtn == 0 && der Value == 0 dann keinen Tab Erzeugen
    // weil ueber OK erzeugt
    if ( nVal == 0 && pBtn == 0 )
        return 0;

    long nOffset = 0;
    const SfxPoolItem* pItem = 0;

    if ( GetItemSet().GetItemState( SID_ATTR_TABSTOP_OFFSET, TRUE, &pItem ) ==
         SFX_ITEM_SET )
    {
        nOffset = ( (const SfxInt32Item*)pItem )->GetValue();
        MapUnit eUnit = (MapUnit)GetItemSet().GetPool()->GetMetric( GetWhich( SID_ATTR_TABSTOP ) );
        nOffset = OutputDevice::LogicToLogic( nOffset, eUnit, MAP_100TH_MM  );
    }
    const long nReal = nVal - nOffset;
    USHORT nSize = aTabBox.GetEntryCount();

    USHORT i;
    for( i = 0; i < nSize; i++ )
    {
        if ( nReal < aNewTabs[i].GetTabPos() )
            break;
    }

    // ListBox-Eintrag vornehmen
    aTabBox.InsertValue( aTabBox.Normalize( nVal ), eDefUnit, i );
    aAktTab.GetTabPos() = nReal;
    SvxTabAdjust eAdj = SVX_TAB_ADJUST_LEFT;

    if ( aRightTab.IsChecked() )
        eAdj = SVX_TAB_ADJUST_RIGHT;
    else if ( aCenterTab.IsChecked() )
        eAdj = SVX_TAB_ADJUST_CENTER;
    else if ( aDezTab.IsChecked() )
        eAdj = SVX_TAB_ADJUST_DECIMAL;

    aAktTab.GetAdjustment() = eAdj;
    aNewTabs.Insert( aAktTab );

    aNewBtn.Disable();
    aDelBtn.Enable();
    aTabBox.GrabFocus();

    // falls kein RadioButton geclickt wurde,
    // muss trotzdem geputtet werden
    bCheck |= TRUE;
    // set the selection into the position Edit
    aTabBox.SetSelection(Selection(0, aTabBox.GetText().Len()));
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, DelHdl_Impl, Button *, EMPTYARG )
{
    USHORT nPos = aTabBox.GetValuePos( aTabBox.GetValue() );

    if ( nPos == COMBOBOX_ENTRY_NOTFOUND )
        return 0;

    if ( aTabBox.GetEntryCount() == 1 )
    {
        DelAllHdl_Impl( 0 );
        return 0;
    }

    // Tab loeschen
    aTabBox.RemoveEntry( nPos );
    aNewTabs.Remove( nPos );

    // aAktTab neu setzen
    const USHORT nSize = aNewTabs.Count();

    if ( nSize > 0 )
    {
        // Pos korrigieren
        nPos = ( ( nSize - 1 ) >= nPos) ? nPos : nPos - 1;
        aTabBox.SetValue( aTabBox.GetValue( nPos ) );
        aAktTab = aNewTabs[nPos];
    }

    // Falls keine Tabs Enable Disable Controls
    if ( aTabBox.GetEntryCount() == 0 )
    {
        aDelBtn.Disable();
        aNewBtn.Enable();
        aTabBox.GrabFocus();
    }

    // falls kein RadioButton geclickt wurde,
    // muss trotzdem geputtet werden
    bCheck |= TRUE;
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, DelAllHdl_Impl, Button *, EMPTYARG )
{
    if ( aNewTabs.Count() )
    {
        aNewTabs = SvxTabStopItem( 0 );
        InitTabPos_Impl();

        // damit in FillItemSet() geputtet wird
        bCheck |= TRUE;
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, TabTypeCheckHdl_Impl, RadioButton *, pBox )
{
    bCheck |= TRUE;
    SvxTabAdjust eAdj;
    aDezChar.Disable();
    aDezCharLabel.Disable();
    aDezChar.SetText( String() );

    if ( pBox == &aLeftTab )
        eAdj = SVX_TAB_ADJUST_LEFT;
    else if ( pBox == &aRightTab )
        eAdj = SVX_TAB_ADJUST_RIGHT;
    else if ( pBox == &aCenterTab )
        eAdj = SVX_TAB_ADJUST_CENTER;
    else
    {
        eAdj = SVX_TAB_ADJUST_DECIMAL;
        aDezChar.Enable();
        aDezCharLabel.Enable();
        aDezChar.SetText( String( (sal_Unicode)aAktTab.GetDecimal() ) );
    }

    aAktTab.GetAdjustment() = eAdj;
    USHORT nPos = aTabBox.GetValuePos( aTabBox.GetValue( eDefUnit ), eDefUnit );

    if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
    {
        aNewTabs.Remove( nPos );
        aNewTabs.Insert( aAktTab );
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, FillTypeCheckHdl_Impl, RadioButton *, pBox )
{
    bCheck |= TRUE;
    BYTE cFill = ' ';
    aFillChar.SetText( String() );
    aFillChar.Disable();

    if( pBox == &aFillSpecial )
        aFillChar.Enable();
    else if ( pBox == &aNoFillChar )
        cFill = ' ';
    else if ( pBox == &aFillSolidLine )
        cFill = '_';
    else if ( pBox == &aFillPoints )
        cFill = '.';
    else if ( pBox == &aFillDashLine )
        cFill = '-';

    aAktTab.GetFill() = cFill;
    USHORT nPos = aTabBox.GetValuePos( aTabBox.GetValue( eDefUnit ), eDefUnit );

    if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
    {
        aNewTabs.Remove( nPos );
        aNewTabs.Insert( aAktTab );
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, GetFillCharHdl_Impl, Edit *, pEdit )
{
    String aChar( pEdit->GetText() );

    if ( aChar.Len() > 0 )
        aAktTab.GetFill() = aChar.GetChar( 0 );

    const USHORT nPos = aTabBox.GetValuePos( aTabBox.GetValue( eDefUnit ), eDefUnit);
    if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
    {
        aNewTabs.Remove( nPos );
        aNewTabs.Insert( aAktTab );
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, GetDezCharHdl_Impl, Edit *, pEdit )
{
    String aChar( pEdit->GetText() );
    if ( aChar.Len() > 0 && ( aChar.GetChar( 0 ) >= ' '))
        aAktTab.GetDecimal() = aChar.GetChar( 0 );

    USHORT nPos = aTabBox.GetValuePos( aTabBox.GetValue( eDefUnit ), eDefUnit );
    if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
    {
        aNewTabs.Remove( nPos );
        aNewTabs.Insert( aAktTab );
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, SelectHdl_Impl, MetricBox *, EMPTYARG )
{
    USHORT nPos = aTabBox.GetValuePos( aTabBox.GetValue( eDefUnit ), eDefUnit );
    if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
    {
        aAktTab = aNewTabs[nPos];
        aNewBtn.Disable();
        SetFillAndTabType_Impl();
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxTabulatorTabPage, ModifyHdl_Impl, MetricBox *, EMPTYARG )
{
    USHORT nPos = aTabBox.GetValuePos( aTabBox.GetValue( eDefUnit ), eDefUnit );
    if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
    {
        aAktTab = aNewTabs[nPos];
        SetFillAndTabType_Impl();

        aAktTab.GetTabPos() =
            aTabBox.Denormalize( aTabBox.GetValue( eDefUnit ) );

        aNewBtn.Disable();
        aDelBtn.Enable();
        return 0;
    }
    aNewBtn.Enable();
    aDelBtn.Disable();
    return 0;
}
//add CHINA001 Begin
void        SvxTabulatorTabPage::PageCreated(SfxAllItemSet aSet)
{
    SFX_ITEMSET_ARG (&aSet,pControlItem,SfxUInt16Item,SID_SVXTABULATORTABPAGE_CONTROLFLAGS,sal_False);
    if (pControlItem)
        DisableControls(pControlItem->GetValue());
}
//end of CHINA001
