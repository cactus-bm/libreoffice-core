/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tp_DataLabel.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 00:14:23 $
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
#include "tp_DataLabel.hxx"
#include "TabPages.hrc"
#include "ResId.hxx"
#include "SchSfxItemIds.hxx"

#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#define ITEMID_CHARTDATADESCR   SCHATTR_DATADESCR_DESCR
#ifndef _SVX_CHRTITEM_HXX
#include <svx/chrtitem.hxx>
#endif

//.............................................................................
namespace chart
{
//.............................................................................

SchDataDescrTabPage::SchDataDescrTabPage(Window* pWindow,
                                         const SfxItemSet& rInAttrs) :
        SfxTabPage(pWindow, SchResId(TP_DATA_DESCR), rInAttrs),
        aCbValue(this, ResId(CB_VALUE)),
        aRbNumber(this, ResId(RB_NUMBER)),
        aRbPercent(this, ResId(RB_PERCENT)),
        aCbText(this, ResId(CB_TEXT)),
        aCbSymbol(this, ResId(CB_SYMBOL))
{
    FreeResource();

    aCbValue.SetClickHdl( LINK( this, SchDataDescrTabPage, EnableHdl ));
    aCbText.SetClickHdl(  LINK( this, SchDataDescrTabPage, EnableHdl ));
}

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

SchDataDescrTabPage::~SchDataDescrTabPage()
{
}

/*************************************************************************
|*
|*  Handler fuer ShowSym-Button enablen/disablen
|*
\*************************************************************************/

IMPL_LINK( SchDataDescrTabPage, EnableHdl, CheckBox *, pControl )
{
    aCbSymbol.Enable( aCbValue.IsChecked() || aCbText.IsChecked() );
    if( pControl == &aCbValue )
    {
        aRbPercent.Enable( aCbValue.IsChecked() );
        aRbNumber.Enable( aCbValue.IsChecked() );
    }

    return 0;
}

/*************************************************************************
|*
|* Erzeugung
|*
\*************************************************************************/

SfxTabPage* SchDataDescrTabPage::Create(Window* pWindow,
                                        const SfxItemSet& rOutAttrs)
{
    return new SchDataDescrTabPage(pWindow, rOutAttrs);
}

/*************************************************************************
|*
|* Fuellt uebergebenen Item-Set mit Dialogbox-Attributen
|*
\*************************************************************************/

BOOL SchDataDescrTabPage::FillItemSet(SfxItemSet& rOutAttrs)
{
    BOOL bText = aCbText.IsChecked();
    SvxChartDataDescr eDescr;

    if( aCbValue.IsChecked() )
    {
        if( aRbNumber.IsChecked() )
            eDescr = (bText? CHDESCR_TEXTANDVALUE : CHDESCR_VALUE);
        else
            eDescr = (bText? CHDESCR_TEXTANDPERCENT : CHDESCR_PERCENT);
    }
    else if( aCbText.IsChecked() )
        eDescr = CHDESCR_TEXT;
    else
        eDescr = CHDESCR_NONE;

    rOutAttrs.Put(SvxChartDataDescrItem(eDescr));
    rOutAttrs.Put(SfxBoolItem( SCHATTR_DATADESCR_SHOW_SYM,
                               aCbSymbol.IsChecked()) );
    return TRUE;
}

/*************************************************************************
|*
|* Initialisierung
|*
\*************************************************************************/

void SchDataDescrTabPage::Reset(const SfxItemSet& rInAttrs)
{
    const SfxPoolItem *pPoolItem = NULL;

    // default state
    aRbNumber.Enable( FALSE );
    aRbPercent.Enable( FALSE );
    aCbSymbol.Enable( FALSE );

    if( rInAttrs.GetItemState(SCHATTR_DATADESCR_SHOW_SYM,
                              TRUE, &pPoolItem) == SFX_ITEM_SET )
        aCbSymbol.Check( ((const SfxBoolItem*)pPoolItem)->GetValue() );

    if( rInAttrs.GetItemState(SCHATTR_DATADESCR_DESCR,
                              TRUE, &pPoolItem) == SFX_ITEM_SET )
    {
        switch( ((const SvxChartDataDescrItem*)pPoolItem)->GetValue() )
        {
            case CHDESCR_VALUE:
                aCbValue.Check();
                aRbNumber.Check();
                EnableHdl( &aCbValue );
                break;

            case CHDESCR_PERCENT:
                aCbValue.Check();
                aRbPercent.Check();
                EnableHdl( &aCbValue );
                break;

            case CHDESCR_TEXT:
                aCbText.Check();
                EnableHdl( &aCbText );
                break;

            case CHDESCR_TEXTANDPERCENT:
                aCbText.Check();
                aCbValue.Check();
                aRbPercent.Check();
                EnableHdl( &aCbValue );
                break;

            case CHDESCR_TEXTANDVALUE:
                aCbText.Check();
                aCbValue.Check();
                aRbNumber.Check();
                EnableHdl( &aCbValue );
                break;

            default:
                break;
        }
    }
    if( !aRbNumber.IsChecked() && !aRbPercent.IsChecked() )
        aRbNumber.Check();
}

//.............................................................................
} //namespace chart
//.............................................................................
