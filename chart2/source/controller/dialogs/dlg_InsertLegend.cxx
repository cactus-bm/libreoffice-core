/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dlg_InsertLegend.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 16:31:32 $
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
#include "precompiled_chart2.hxx"
#include "dlg_InsertLegend.hxx"
#include "dlg_InsertLegend.hrc"

#include "ResId.hxx"
#include "SchSfxItemIds.hxx"

// header for enum SvxChartLegendPos
#ifndef _SVX_CHRTITEM_HXX
#include <svx/chrtitem.hxx>
#endif
// header for class SfxItemPool
#ifndef _SFXITEMPOOL_HXX
#include <svtools/itempool.hxx>
#endif

//.............................................................................
namespace chart
{
//.............................................................................

SchLegendDlg::SchLegendDlg(Window* pWindow, const SfxItemSet& rInAttrs) :
    ModalDialog(pWindow, SchResId(DLG_LEGEND)),
    aCbxShow(this, SchResId(CBX_SHOW)),
    aRbtLeft(this, SchResId(RBT_LEFT)),
    aRbtTop(this, SchResId(RBT_TOP)),
    aRbtRight(this, SchResId(RBT_RIGHT)),
    aRbtBottom(this, SchResId(RBT_BOTTOM)),
    aFlLegend(this, SchResId(FL_LEGEND)),
    aBtnOK(this, SchResId(BTN_OK)),
    aBtnCancel(this, SchResId(BTN_CANCEL)),
    aBtnHelp(this, SchResId(BTN_HELP)),
    m_rInAttrs(rInAttrs)
{
    FreeResource();

    aCbxShow.SetClickHdl (LINK(this, SchLegendDlg, CbxClick));

    Reset();
}

SchLegendDlg::~SchLegendDlg()
{
}

void SchLegendDlg::Reset()
{
    SvxChartLegendPos ePos = CHLEGEND_NONE;

    const SfxPoolItem* pPoolItem = NULL;
    if( m_rInAttrs.GetItemState( SCHATTR_LEGEND_POS,
                               TRUE, &pPoolItem ) != SFX_ITEM_SET )
        pPoolItem = &(m_rInAttrs.GetPool()->GetDefaultItem( SCHATTR_LEGEND_POS ));

    if( pPoolItem )
        ePos = ((const SvxChartLegendPosItem*)pPoolItem)->GetValue();

    switch( ePos )
    {
        case CHLEGEND_LEFT:
            aRbtLeft.Check(TRUE);
            break;
        case CHLEGEND_TOP:
            aRbtTop.Check(TRUE);
            break;
        case CHLEGEND_NONE:
        case CHLEGEND_RIGHT:
            aRbtRight.Check(TRUE);
            break;
        case CHLEGEND_BOTTOM:
            aRbtBottom.Check(TRUE);
            break;
        default:
            break;
    }

    aCbxShow.Check (ePos != CHLEGEND_NONE);
    aRbtLeft.Enable (ePos != CHLEGEND_NONE);
    aRbtTop.Enable (ePos != CHLEGEND_NONE);
    aRbtRight.Enable (ePos != CHLEGEND_NONE);
    aRbtBottom.Enable (ePos != CHLEGEND_NONE);
}

void SchLegendDlg::GetAttr(SfxItemSet& _rOutAttrs)
{
    SvxChartLegendPos ePos;

    if ( ! aCbxShow.IsChecked())
        ePos = CHLEGEND_NONE;
    else if (aRbtLeft.IsChecked())
        ePos = CHLEGEND_LEFT;
    else if (aRbtTop.IsChecked())
        ePos = CHLEGEND_TOP;
    else if (aRbtRight.IsChecked())
        ePos = CHLEGEND_RIGHT;
    else if (aRbtBottom.IsChecked())
        ePos = CHLEGEND_BOTTOM;
    else
        ePos = CHLEGEND_NONE;

    _rOutAttrs.Put(SvxChartLegendPosItem(ePos, SCHATTR_LEGEND_POS));
}




IMPL_LINK (SchLegendDlg, CbxClick, CheckBox *, pBtn)
{
    BOOL    bChecked = aCbxShow.IsChecked();

    aRbtLeft.Enable (bChecked);
    aRbtTop.Enable (bChecked);
    aRbtRight.Enable (bChecked);
    aRbtBottom.Enable (bChecked);

    return 0;
}

//.............................................................................
} //namespace chart
//.............................................................................
