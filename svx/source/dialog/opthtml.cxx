/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: opthtml.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 21:44:14 $
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

#ifdef SVX_DLLIMPLEMENTATION
#undef SVX_DLLIMPLEMENTATION
#endif

// include ---------------------------------------------------------------

#include "htmlcfg.hxx"
#include "opthtml.hxx"
#include "opthtml.hrc"
#include "dialogs.hrc"
#include "helpid.hrc"
#include "dialmgr.hxx"

// Umwandlung der Modi zu den Positionen in der Listbox
const USHORT aPosToExportArr[] =
{
    HTML_CFG_HTML32,
    HTML_CFG_MSIE_40,
    HTML_CFG_NS40,
    HTML_CFG_WRITER
};

//#define DEPRECATED_ENTRY  0xFFFF

const USHORT aExportToPosArr[] =
{
    0,  //HTML 3.2
    1,  //MS Internet Explorer 4.0
    3,  //StarWriter
    2   //Netscape Navigator 4.0
};
// -----------------------------------------------------------------------

OfaHtmlTabPage::OfaHtmlTabPage(Window* pParent, const SfxItemSet& rSet) :
        SfxTabPage( pParent, ResId( RID_OFAPAGE_HTMLOPT, DIALOG_MGR() ), rSet ),
    aFontSizeGB     ( this, ResId( GB_FONTSIZE     ) ),
    aSize1FT        ( this, ResId( FT_SIZE1        ) ),
    aSize1NF        ( this, ResId( NF_SIZE1        ) ),
    aSize2FT        ( this, ResId( FT_SIZE2        ) ),
    aSize2NF        ( this, ResId( NF_SIZE2        ) ),
    aSize3FT        ( this, ResId( FT_SIZE3        ) ),
    aSize3NF        ( this, ResId( NF_SIZE3        ) ),
    aSize4FT        ( this, ResId( FT_SIZE4        ) ),
    aSize4NF        ( this, ResId( NF_SIZE4        ) ),
    aSize5FT        ( this, ResId( FT_SIZE5        ) ),
    aSize5NF        ( this, ResId( NF_SIZE5        ) ),
    aSize6FT        ( this, ResId( FT_SIZE6        ) ),
    aSize6NF        ( this, ResId( NF_SIZE6        ) ),
    aSize7FT        ( this, ResId( FT_SIZE7        ) ),
    aSize7NF        ( this, ResId( NF_SIZE7        ) ),
    aImportGB       ( this, ResId( GB_IMPORT       ) ),
    aUnknownTagCB   ( this, ResId( CB_UNKNOWN_TAGS ) ),
    aIgnoreFontNamesCB( this, ResId( CB_IGNORE_FONTNAMES ) ),
    aExportGB       ( this, ResId( GB_EXPORT       ) ),
    aExportLB       ( this, ResId( LB_EXPORT       ) ),
    aPrintExtensionCB( this,ResId(CB_PRINT_EXTENSION )),
    aStarBasicCB    ( this, ResId( CB_STARBASIC    ) ),
    aStarBasicWarningCB(this, ResId( CB_STARBASIC_WARNING    ) ),
    aSaveGrfLocalCB ( this, ResId( CB_LOCAL_GRF    ) ),
    aCharSetFT      ( this, ResId( FT_CHARSET      ) ),
    aCharSetLB      ( this, ResId( LB_CHARSET      ) )

{
    FreeResource();
    aExportLB.SetSelectHdl(LINK(this, OfaHtmlTabPage, ExportHdl_Impl));
    aStarBasicCB.SetClickHdl(LINK(this, OfaHtmlTabPage, CheckBoxHdl_Impl));

    // initialize the characterset listbox
    aCharSetLB.FillWithMimeAndSelectBest();
}

/*-----------------02.09.96 13.47-------------------

--------------------------------------------------*/

OfaHtmlTabPage::~OfaHtmlTabPage()
{
}

/*-----------------02.09.96 13.47-------------------

--------------------------------------------------*/

SfxTabPage* OfaHtmlTabPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet )
{
    return new OfaHtmlTabPage(pParent, rAttrSet);
}

/*-----------------02.09.96 13.47-------------------

--------------------------------------------------*/

BOOL OfaHtmlTabPage::FillItemSet( SfxItemSet& rSet )
{
    SvxHtmlOptions* pHtmlOpt = SvxHtmlOptions::Get();
    if(aSize1NF.GetSavedValue() != aSize1NF.GetText())
        pHtmlOpt->SetFontSize(0, (USHORT)aSize1NF.GetValue());
    if(aSize2NF.GetSavedValue() != aSize2NF.GetText())
        pHtmlOpt->SetFontSize(1, (USHORT)aSize2NF.GetValue());
    if(aSize3NF.GetSavedValue() != aSize3NF.GetText())
        pHtmlOpt->SetFontSize(2, (USHORT)aSize3NF.GetValue());
    if(aSize4NF.GetSavedValue() != aSize4NF.GetText())
        pHtmlOpt->SetFontSize(3, (USHORT)aSize4NF.GetValue());
    if(aSize5NF.GetSavedValue() != aSize5NF.GetText())
        pHtmlOpt->SetFontSize(4, (USHORT)aSize5NF.GetValue());
    if(aSize6NF.GetSavedValue() != aSize6NF.GetText())
        pHtmlOpt->SetFontSize(5, (USHORT)aSize6NF.GetValue());
    if(aSize7NF.GetSavedValue() != aSize7NF.GetText())
        pHtmlOpt->SetFontSize(6, (USHORT)aSize7NF.GetValue());

    if(aUnknownTagCB.IsChecked() != aUnknownTagCB.GetSavedValue())
        pHtmlOpt->SetImportUnknown(aUnknownTagCB.IsChecked());

    if(aIgnoreFontNamesCB.IsChecked() != aIgnoreFontNamesCB.GetSavedValue())
        pHtmlOpt->SetIgnoreFontFamily(aIgnoreFontNamesCB.IsChecked());

    if(aExportLB.GetSelectEntryPos() != aExportLB.GetSavedValue())
        pHtmlOpt->SetExportMode(aPosToExportArr[aExportLB.GetSelectEntryPos()]);

    if(aStarBasicCB.IsChecked() != aStarBasicCB.GetSavedValue())
        pHtmlOpt->SetStarBasic(aStarBasicCB.IsChecked());

    if(aStarBasicWarningCB.IsChecked() != aStarBasicWarningCB.GetSavedValue())
        pHtmlOpt->SetStarBasicWarning(aStarBasicWarningCB.IsChecked());

    if(aSaveGrfLocalCB.IsChecked() != aSaveGrfLocalCB.GetSavedValue())
        pHtmlOpt->SetSaveGraphicsLocal(aSaveGrfLocalCB.IsChecked());

    if(aPrintExtensionCB.IsChecked() != aPrintExtensionCB.GetSavedValue())
        pHtmlOpt->SetPrintLayoutExtension(aPrintExtensionCB.IsChecked());

    if( aCharSetLB.GetSelectTextEncoding() != pHtmlOpt->GetTextEncoding() )
        pHtmlOpt->SetTextEncoding( aCharSetLB.GetSelectTextEncoding() );

    return FALSE;
}

/*-----------------02.09.96 13.47-------------------

--------------------------------------------------*/

void OfaHtmlTabPage::Reset( const SfxItemSet& rSet )
{
    SvxHtmlOptions* pHtmlOpt = SvxHtmlOptions::Get();
    aSize1NF.SetValue(pHtmlOpt->GetFontSize(0));
    aSize2NF.SetValue(pHtmlOpt->GetFontSize(1));
    aSize3NF.SetValue(pHtmlOpt->GetFontSize(2));
    aSize4NF.SetValue(pHtmlOpt->GetFontSize(3));
    aSize5NF.SetValue(pHtmlOpt->GetFontSize(4));
    aSize6NF.SetValue(pHtmlOpt->GetFontSize(5));
    aSize7NF.SetValue(pHtmlOpt->GetFontSize(6));
    aUnknownTagCB.Check(pHtmlOpt->IsImportUnknown());
    aIgnoreFontNamesCB.Check(pHtmlOpt->IsIgnoreFontFamily());
    USHORT nExport = pHtmlOpt->GetExportMode();
    if( nExport >= ( sizeof( aExportToPosArr ) / sizeof( USHORT ) ) )
        nExport = 4;    // default for bad config entry is NS 4.0
    USHORT nPosArr = aExportToPosArr[ nExport ];
//  if( nPosArr == DEPRECATED_ENTRY )
//      nPosArr = aExportToPosArr[ 4 ];     // again: NS 4.0 is default
    aExportLB.SelectEntryPos( nPosArr );
    aExportLB.SaveValue();

    ExportHdl_Impl(&aExportLB);

    aStarBasicCB .Check(pHtmlOpt->IsStarBasic());
    aStarBasicWarningCB .Check(pHtmlOpt->IsStarBasicWarning());
    aStarBasicWarningCB.Enable(!aStarBasicCB.IsChecked());
    aSaveGrfLocalCB.Check(pHtmlOpt->IsSaveGraphicsLocal());
    aPrintExtensionCB.Check(pHtmlOpt->IsPrintLayoutExtension());

    aPrintExtensionCB.SaveValue();
    aStarBasicCB .SaveValue();
    aStarBasicWarningCB .SaveValue();
    aSaveGrfLocalCB.SaveValue();
    aSize1NF.SaveValue();
    aSize2NF.SaveValue();
    aSize3NF.SaveValue();
    aSize4NF.SaveValue();
    aSize5NF.SaveValue();
    aSize6NF.SaveValue();
    aSize7NF.SaveValue();
    aUnknownTagCB.SaveValue();
    aIgnoreFontNamesCB.SaveValue();

    if( !pHtmlOpt->IsDefaultTextEncoding() &&
        aCharSetLB.GetSelectTextEncoding() != pHtmlOpt->GetTextEncoding() )
        aCharSetLB.SelectTextEncoding( pHtmlOpt->GetTextEncoding() );
}

/*-----------------16.04.98 16:03-------------------

--------------------------------------------------*/
IMPL_LINK(OfaHtmlTabPage, ExportHdl_Impl, ListBox*, pBox)
{
    USHORT nExport = aPosToExportArr[ pBox->GetSelectEntryPos() ];
    switch( nExport )
    {
        case HTML_CFG_MSIE_40:
        case HTML_CFG_NS40  :
        case HTML_CFG_WRITER :
            aPrintExtensionCB.Enable(TRUE);
        break;
        default: aPrintExtensionCB.Enable(FALSE);
    }

    return 0;
}
/* -----------------05.02.99 09:17-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK(OfaHtmlTabPage, CheckBoxHdl_Impl, CheckBox*, pBox)
{
    aStarBasicWarningCB.Enable(!pBox->IsChecked());
    return 0;
}


