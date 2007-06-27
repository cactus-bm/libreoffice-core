/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: optchart.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 17:24:49 $
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

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif

#include <svx/dialogs.hrc>

#include "optchart.hxx"
#include "optchart.hrc"
#include <svx/dialmgr.hxx>

// ====================
// class ChartColorLB
// ====================
void ChartColorLB::FillBox( const SvxChartColorTable & rTab )
{
    long nCount = rTab.size();
    SetUpdateMode( FALSE );

    for( long i = 0; i < nCount; i++ )
    {
        Append( const_cast< XColorEntry * >( & rTab[ i ] ));
    }
    SetUpdateMode( TRUE );
}


// ====================
// class SvxDefaultColorOptPage
// ====================
SvxDefaultColorOptPage::SvxDefaultColorOptPage( Window* pParent, const SfxItemSet& rInAttrs ) :

    SfxTabPage( pParent, SVX_RES( RID_OPTPAGE_CHART_DEFCOLORS ), rInAttrs ),

    aGbChartColors  ( this, SVX_RES( FL_CHART_COLOR_LIST ) ),
    aLbChartColors  ( this, SVX_RES( LB_CHART_COLOR_LIST ) ),
    aGbColorBox     ( this, SVX_RES( FL_COLOR_BOX ) ),
    aValSetColorBox ( this, SVX_RES( CT_COLOR_BOX ) ),
    aPBDefault      ( this, SVX_RES( PB_RESET_TO_DEFAULT ) )
{
    FreeResource();

    aPBDefault.SetClickHdl( LINK( this, SvxDefaultColorOptPage, ResetToDefaults ) );
    aLbChartColors.SetSelectHdl( LINK( this, SvxDefaultColorOptPage, ListClickedHdl ) );
    aValSetColorBox.SetSelectHdl( LINK( this, SvxDefaultColorOptPage, BoxClickedHdl ) );

    aValSetColorBox.SetStyle( aValSetColorBox.GetStyle()
                                    | WB_VSCROLL | WB_ITEMBORDER | WB_NAMEFIELD );
    aValSetColorBox.SetColCount( 8 );
    aValSetColorBox.SetLineCount( 12 );
    aValSetColorBox.SetExtraSpacing( 0 );
    aValSetColorBox.Show();

    pChartOptions = new SvxChartOptions;
    pColorTab = new XColorTable( SvtPathOptions().GetPalettePath() );

    const SfxPoolItem* pItem = NULL;
    if ( rInAttrs.GetItemState( SID_SCH_EDITOPTIONS, FALSE, &pItem ) == SFX_ITEM_SET )
    {
        pColorConfig = SAL_STATIC_CAST( SvxChartColorTableItem*, pItem->Clone() );
    }
    else
    {
        SvxChartColorTable aTable;
        aTable.useDefault();
        pColorConfig = new SvxChartColorTableItem( SID_SCH_EDITOPTIONS, aTable );
        pColorConfig->SetOptions( pChartOptions );
    }

    Construct();
}

SvxDefaultColorOptPage::~SvxDefaultColorOptPage()
{
    // save changes
    pChartOptions->SetDefaultColors( pColorConfig->GetColorTable() );
    pChartOptions->Commit();

    delete pColorConfig;
    delete pColorTab;
    delete pChartOptions;
}

void SvxDefaultColorOptPage::Construct()
{
    if( pColorConfig )
        aLbChartColors.FillBox( pColorConfig->GetColorTable() );

    FillColorBox();

    aLbChartColors.SelectEntryPos( 0 );
    ListClickedHdl( &aLbChartColors );
}


SfxTabPage* __EXPORT SvxDefaultColorOptPage::Create( Window* pParent, const SfxItemSet& rAttrs )
{
    return new SvxDefaultColorOptPage( pParent, rAttrs );
}

BOOL __EXPORT SvxDefaultColorOptPage::FillItemSet( SfxItemSet& rOutAttrs )
{
    if( pColorConfig )
        rOutAttrs.Put( *SAL_STATIC_CAST( SfxPoolItem*, pColorConfig ));

    return TRUE;
}

void __EXPORT SvxDefaultColorOptPage::Reset( const SfxItemSet& )
{
    aLbChartColors.SelectEntryPos( 0 );
    ListClickedHdl( &aLbChartColors );
}

void SvxDefaultColorOptPage::FillColorBox()
{
    if( !pColorTab ) return;

    long nCount = pColorTab->Count();
    XColorEntry* pColorEntry;

    for( long i = 0; i < nCount; i++ )
    {
        pColorEntry = pColorTab->GetColor( i );
        aValSetColorBox.InsertItem( (USHORT) i + 1, pColorEntry->GetColor(), pColorEntry->GetName() );
    }
}


long SvxDefaultColorOptPage::GetColorIndex( const Color& rCol )
{
    if( pColorTab )
    {
        long nCount = pColorTab->Count();
        XColorEntry* pColorEntry;

        for( long i = nCount - 1; i >= 0; i-- )         // default chart colors are at the end of the table
        {
            pColorEntry = pColorTab->GetColor( i );
            if( pColorEntry && pColorEntry->GetColor() == rCol )
                return SAL_STATIC_CAST( XPropertyTable*, pColorTab )->Get( pColorEntry->GetName() );
        }
    }
    return -1L;
}



// --------------------
// event handlers
// --------------------

// ResetToDefaults
// ---------------

IMPL_LINK( SvxDefaultColorOptPage, ResetToDefaults, void *, EMPTYARG )
{
    if( pColorConfig )
    {
        pColorConfig->GetColorTable().useDefault();

        aLbChartColors.Clear();
        aLbChartColors.FillBox( pColorConfig->GetColorTable() );

        aLbChartColors.GetFocus();
    }

    return 0L;
}

// ListClickedHdl
// --------------

IMPL_LINK( SvxDefaultColorOptPage, ListClickedHdl, ChartColorLB*,  pColorList )
{
    Color aCol = pColorList->GetSelectEntryColor();

    long nIndex = GetColorIndex( aCol );

    if( nIndex == -1 )      // not found
    {
        aValSetColorBox.SetNoSelection();
    }
    else
    {
        aValSetColorBox.SelectItem( (USHORT)nIndex + 1 );       // ValueSet is 1-based
    }

    return 0L;
}

// BoxClickedHdl
// -------------

IMPL_LINK( SvxDefaultColorOptPage, BoxClickedHdl, ValueSet*, EMPTYARG )
{
    USHORT nIdx = aLbChartColors.GetSelectEntryPos();
    if( nIdx != LISTBOX_ENTRY_NOTFOUND )
    {
        XColorEntry aEntry( aValSetColorBox.GetItemColor( aValSetColorBox.GetSelectItemId() ),
                            aLbChartColors.GetSelectEntry() );

        aLbChartColors.Modify( & aEntry, nIdx );
        pColorConfig->ReplaceColorByIndex( nIdx, aEntry );

        aLbChartColors.SelectEntryPos( nIdx );  // reselect entry
    }

    return 0L;
}

