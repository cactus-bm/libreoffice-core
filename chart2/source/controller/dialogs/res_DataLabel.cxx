/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: res_DataLabel.cxx,v $
 * $Revision: 1.5 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_chart2.hxx"

#include "res_DataLabel.hxx"
#include "res_DataLabel_IDs.hrc"

#include "Strings.hrc"
#include "ResId.hxx"
#include "chartview/ChartSfxItemIds.hxx"
#include "NoWarningThisInCTOR.hxx"
#include "dlg_NumberFormat.hxx"

// header for class SvxNumberInfoItem
#include <svx/numinf.hxx>
#include <svtools/eitem.hxx>
// for SfxUInt32Item
#include <svtools/intitem.hxx>
// header for class SfxStringItem
#include <svtools/stritem.hxx>
//SfxIntegerListItem
#include <svtools/ilstitem.hxx>

// header for define RET_OK
#include <vcl/msgbox.hxx>
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
// header for SvNumberFormatter
#include <svtools/zforlist.hxx>
#ifndef _SVT_CONTROLDIMS_HRC_
#include <svtools/controldims.hrc>
#endif


//.............................................................................
namespace chart
{
//.............................................................................

namespace
{

bool lcl_ReadNumberFormatFromItemSet( const SfxItemSet& rSet, USHORT nValueWhich, USHORT nSourceFormatWhich, ULONG& rnFormatKeyOut, bool& rbSourceFormatOut, bool& rbSourceFormatMixedStateOut )
{
    bool bSet = false;
    const SfxPoolItem *pItem1 = NULL;
    if( rSet.GetItemState( nValueWhich, TRUE, &pItem1 ) == SFX_ITEM_SET )
    {
        const SfxUInt32Item * pNumItem = dynamic_cast< const SfxUInt32Item * >( pItem1 );
        if( pNumItem )
        {
            rnFormatKeyOut = pNumItem->GetValue();
            bSet = true;
        }
    }

    rbSourceFormatMixedStateOut=true;
    const SfxPoolItem *pItem2 = NULL;
    if( rSet.GetItemState( nSourceFormatWhich, TRUE, &pItem2 ) == SFX_ITEM_SET )
    {
        const SfxBoolItem * pBoolItem = dynamic_cast< const SfxBoolItem * >( pItem2 );
        if( pBoolItem )
        {
            rbSourceFormatOut = pBoolItem->GetValue();
            rbSourceFormatMixedStateOut=false;
        }
    }
    return bSet;
}

void lcl_setBoolItemToCheckBox( const SfxItemSet& rInAttrs, USHORT nWhichId, CheckBox& rCheckbox )
{
    rCheckbox.EnableTriState( FALSE );

    const SfxPoolItem *pPoolItem = NULL;
    if( rInAttrs.GetItemState(nWhichId, TRUE, &pPoolItem) == SFX_ITEM_SET )
        rCheckbox.Check( ((const SfxBoolItem*)pPoolItem)->GetValue() );
    else
    {
        rCheckbox.EnableTriState( TRUE );
        rCheckbox.SetState( STATE_DONTKNOW );
    }
}

}//end anonymous namespace

DataLabelResources::DataLabelResources( Window* pWindow, const SfxItemSet& rInAttrs )
    : m_aCBNumber(pWindow, SchResId(CB_VALUE_AS_NUMBER)),
    m_aPB_NumberFormatForValue(pWindow, SchResId(PB_NUMBERFORMAT)),
    m_aCBPercent(pWindow, SchResId(CB_VALUE_AS_PERCENTAGE)),
    m_aPB_NumberFormatForPercent(pWindow, SchResId(PB_PERCENT_NUMBERFORMAT)),
    m_aCBCategory(pWindow, SchResId(CB_CATEGORY)),
    m_aCBSymbol(pWindow, SchResId(CB_SYMBOL)),
    m_aSeparatorResources(pWindow),
    m_aFT_LabelPlacement(pWindow, SchResId(FT_LABEL_PLACEMENT)),
    m_aLB_LabelPlacement(pWindow, SchResId(LB_LABEL_PLACEMENT)),
    m_pNumberFormatter(0),
    m_bNumberFormatMixedState(true),
    m_bPercentFormatMixedState(true),
    m_nNumberFormatForValue(0),
    m_nNumberFormatForPercent(11),
    m_bSourceFormatMixedState(true),
    m_bPercentSourceMixedState(true),
    m_bSourceFormatForValue(true),
    m_bSourceFormatForPercent(true),
    m_pWindow(pWindow),
    m_pPool(rInAttrs.GetPool())
{
    std::map< sal_Int32, XubString > aPlacementToStringMap;
    for( sal_Int32 nEnum=0; nEnum<m_aLB_LabelPlacement.GetEntryCount(); ++nEnum )
        aPlacementToStringMap[nEnum]=m_aLB_LabelPlacement.GetEntry(static_cast<USHORT>(nEnum));

    ::com::sun::star::uno::Sequence < sal_Int32 > aAvailabelPlacementList;
    const SfxPoolItem *pPoolItem = NULL;
    if( rInAttrs.GetItemState(SCHATTR_DATADESCR_AVAILABLE_PLACEMENTS, TRUE, &pPoolItem) == SFX_ITEM_SET )
        aAvailabelPlacementList =((const SfxIntegerListItem*)pPoolItem)->GetConstSequence();

    m_aLB_LabelPlacement.Clear();

    for( sal_Int32 nN=0; nN<aAvailabelPlacementList.getLength(); ++nN )
    {
        USHORT nListBoxPos = static_cast<USHORT>( nN );
        sal_Int32 nPlacement = aAvailabelPlacementList[nN];
        m_aPlacementToListBoxMap[nPlacement]=nListBoxPos;
        m_aListBoxToPlacementMap[nListBoxPos]=nPlacement;
        m_aLB_LabelPlacement.InsertEntry( aPlacementToStringMap[nPlacement] );
    }

    m_aLB_LabelPlacement.SetDropDownLineCount(m_aLB_LabelPlacement.GetEntryCount());

    Size aPBSize( m_aPB_NumberFormatForPercent.GetSizePixel() );
    long nMinWidth = ::std::max( m_aPB_NumberFormatForPercent.CalcMinimumSize().getWidth(), m_aPB_NumberFormatForValue.CalcMinimumSize().getWidth() );
    aPBSize.setWidth( nMinWidth+20 );//the min with is to small to fit, hm... so add alittle

    m_aPB_NumberFormatForValue.SetSizePixel( aPBSize );
    m_aPB_NumberFormatForPercent.SetSizePixel( aPBSize );

    long nWantedMinRightBorder = m_aPB_NumberFormatForPercent.GetPosPixel().X() +  m_aPB_NumberFormatForPercent.GetSizePixel().Width() - 1;

    Size aSize( m_aFT_LabelPlacement.GetSizePixel() );
    aSize.setWidth( m_aFT_LabelPlacement.CalcMinimumSize().getWidth() );
    m_aFT_LabelPlacement.SetSizePixel(aSize);

    Size aControlDistance( pWindow->LogicToPixel( Size(RSC_SP_CTRL_DESC_X,RSC_SP_CTRL_GROUP_Y), MapMode(MAP_APPFONT) ) );
    long nWantedMinLeftBorder = m_aFT_LabelPlacement.GetPosPixel().X() + aSize.getWidth () + aControlDistance.Width();;

    m_aSeparatorResources.PositionBelowControl(m_aCBSymbol);
    m_aSeparatorResources.AlignListBoxWidthAndXPos( nWantedMinLeftBorder, nWantedMinRightBorder, m_aLB_LabelPlacement.CalcMinimumSize().getWidth() );
    m_aSeparatorResources.Show(true);

    aSize = m_aLB_LabelPlacement.GetSizePixel();
    aSize.setWidth( m_aSeparatorResources.GetCurrentListBoxSize().getWidth() );
    m_aLB_LabelPlacement.SetSizePixel(aSize);

    long nYDiff = m_aFT_LabelPlacement.GetPosPixel().Y() - m_aLB_LabelPlacement.GetPosPixel().Y();
    Point aPos( m_aSeparatorResources.GetCurrentListBoxPosition() );
    aPos.Y() = m_aSeparatorResources.GetBottom();
    aPos.Y() += aControlDistance.Height();
    m_aLB_LabelPlacement.SetPosPixel(aPos);

    aPos.X() = m_aFT_LabelPlacement.GetPosPixel().X();
    aPos.Y() += nYDiff;
    m_aFT_LabelPlacement.SetPosPixel(aPos);

    m_aPB_NumberFormatForValue.SetClickHdl( LINK( this, DataLabelResources, NumberFormatDialogHdl ) );
    m_aPB_NumberFormatForPercent.SetClickHdl( LINK( this, DataLabelResources, NumberFormatDialogHdl ) );
    m_aCBNumber.SetClickHdl( LINK( this, DataLabelResources, CheckHdl ));
    m_aCBPercent.SetClickHdl( LINK( this, DataLabelResources, CheckHdl ));
    m_aCBCategory.SetClickHdl(  LINK( this, DataLabelResources, CheckHdl ));
    m_aCBSymbol.SetClickHdl(  LINK( this, DataLabelResources, CheckHdl ));

    m_bNumberFormatMixedState = !lcl_ReadNumberFormatFromItemSet( rInAttrs, SID_ATTR_NUMBERFORMAT_VALUE, SID_ATTR_NUMBERFORMAT_SOURCE, m_nNumberFormatForValue, m_bSourceFormatForValue, m_bSourceFormatMixedState );
    m_bPercentFormatMixedState = !lcl_ReadNumberFormatFromItemSet( rInAttrs, SCHATTR_PERCENT_NUMBERFORMAT_VALUE, SCHATTR_PERCENT_NUMBERFORMAT_SOURCE, m_nNumberFormatForPercent, m_bSourceFormatForPercent , m_bPercentSourceMixedState);
}

DataLabelResources::~DataLabelResources()
{
}

void DataLabelResources::SetNumberFormatter( SvNumberFormatter* pFormatter )
{
    m_pNumberFormatter = pFormatter;
}

IMPL_LINK( DataLabelResources, NumberFormatDialogHdl, PushButton *, pButton )
{
    if( !m_pPool || !m_pNumberFormatter )
    {
        DBG_ERROR("Missing item pool or number formatter");
        return 1;
    }

    if( pButton == &m_aPB_NumberFormatForValue && !m_aCBNumber.IsChecked())
        m_aCBNumber.Check();
    else if( pButton == &m_aPB_NumberFormatForPercent && !m_aCBPercent.IsChecked())
        m_aCBPercent.Check();

    SfxItemSet aNumberSet = NumberFormatDialog::CreateEmptyItemSetForNumberFormatDialog( *m_pPool );
    aNumberSet.Put (SvxNumberInfoItem( m_pNumberFormatter, (const USHORT)SID_ATTR_NUMBERFORMAT_INFO));

    bool bPercent = ( pButton == &m_aPB_NumberFormatForPercent );

    ULONG& rnFormatKey = bPercent ? m_nNumberFormatForPercent : m_nNumberFormatForValue;
    bool& rUseSourceFormat = bPercent ? m_bSourceFormatForPercent : m_bSourceFormatForValue;
    bool& rbMixedState = bPercent ? m_bPercentFormatMixedState : m_bNumberFormatMixedState;
    bool& rbSourceMixedState = bPercent ? m_bPercentSourceMixedState : m_bSourceFormatMixedState;

    if(!rbMixedState)
        aNumberSet.Put( SfxUInt32Item( SID_ATTR_NUMBERFORMAT_VALUE, rnFormatKey ));
    aNumberSet.Put( SfxBoolItem( SID_ATTR_NUMBERFORMAT_SOURCE, rUseSourceFormat ));

    NumberFormatDialog aDlg(m_pWindow, aNumberSet);
    if( bPercent )
        aDlg.SetText( String( SchResId( STR_DLG_NUMBERFORMAT_FOR_PERCENTAGE_VALUE ) ) );
    if( RET_OK == aDlg.Execute() )
    {
        const SfxItemSet* pResult = aDlg.GetOutputItemSet();
        if( pResult )
        {
            bool bOldSource = rUseSourceFormat;
            ULONG nOldFormat = rnFormatKey;
            bool bOldMixedState = rbMixedState || rbSourceMixedState;

            rbMixedState = !lcl_ReadNumberFormatFromItemSet( *pResult, SID_ATTR_NUMBERFORMAT_VALUE, SID_ATTR_NUMBERFORMAT_SOURCE, rnFormatKey, rUseSourceFormat, rbSourceMixedState );

            //todo this maybe can be removed when the numberformatter dialog does handle mixed state for source format correctly
            if( bOldMixedState && bOldSource == rUseSourceFormat && nOldFormat == rnFormatKey )
                rbMixedState = rbSourceMixedState = true;
        }
    }
    return 0;
}

IMPL_LINK( DataLabelResources, CheckHdl, CheckBox*, pBox )
{
    if( pBox )
        pBox->EnableTriState( FALSE );
    EnableControls();
    return 0;
}

void DataLabelResources::EnableControls()
{
    m_aCBSymbol.Enable( m_aCBNumber.IsChecked() || m_aCBPercent.IsChecked() || m_aCBCategory.IsChecked() );

    //enable separator
    {
        long nNumberOfCheckedLabelParts = 0;
        if( m_aCBNumber.IsChecked() )
            ++nNumberOfCheckedLabelParts;
        if( m_aCBPercent.IsChecked() )
            ++nNumberOfCheckedLabelParts;
        if( m_aCBCategory.IsChecked() )
            ++nNumberOfCheckedLabelParts;
        m_aSeparatorResources.Enable( nNumberOfCheckedLabelParts > 1 );
        bool bEnablePlacement = nNumberOfCheckedLabelParts > 0 && m_aLB_LabelPlacement.GetEntryCount()>1;
        m_aFT_LabelPlacement.Enable( bEnablePlacement );
        m_aLB_LabelPlacement.Enable( bEnablePlacement );
    }

    m_aPB_NumberFormatForValue.Enable( m_pNumberFormatter && m_aCBNumber.IsChecked() );
    m_aPB_NumberFormatForPercent.Enable( m_pNumberFormatter && m_aCBPercent.IsChecked() );
}

BOOL DataLabelResources::FillItemSet( SfxItemSet& rOutAttrs ) const
{
    if( m_aCBNumber.IsChecked() )
    {
        if( !m_bNumberFormatMixedState )
            rOutAttrs.Put( SfxUInt32Item( SID_ATTR_NUMBERFORMAT_VALUE, m_nNumberFormatForValue ));
        if( !m_bSourceFormatMixedState )
            rOutAttrs.Put( SfxBoolItem( SID_ATTR_NUMBERFORMAT_SOURCE, m_bSourceFormatForValue ));
    }
    if( m_aCBPercent.IsChecked() )
    {
        if( !m_bPercentFormatMixedState )
            rOutAttrs.Put( SfxUInt32Item( SCHATTR_PERCENT_NUMBERFORMAT_VALUE, m_nNumberFormatForPercent ));
        if( !m_bPercentSourceMixedState )
            rOutAttrs.Put( SfxBoolItem( SCHATTR_PERCENT_NUMBERFORMAT_SOURCE, m_bSourceFormatForPercent ));
    }

    if( m_aCBNumber.GetState()!= STATE_DONTKNOW )
        rOutAttrs.Put( SfxBoolItem( SCHATTR_DATADESCR_SHOW_NUMBER, m_aCBNumber.IsChecked() ) );
    if( m_aCBPercent.GetState()!= STATE_DONTKNOW )
        rOutAttrs.Put( SfxBoolItem( SCHATTR_DATADESCR_SHOW_PERCENTAGE, m_aCBPercent.IsChecked() ) );
    if( m_aCBCategory.GetState()!= STATE_DONTKNOW )
        rOutAttrs.Put( SfxBoolItem( SCHATTR_DATADESCR_SHOW_CATEGORY, m_aCBCategory.IsChecked() ) );
    if( m_aCBSymbol.GetState()!= STATE_DONTKNOW )
        rOutAttrs.Put( SfxBoolItem( SCHATTR_DATADESCR_SHOW_SYMBOL, m_aCBSymbol.IsChecked()) );

    rOutAttrs.Put( SfxStringItem( SCHATTR_DATADESCR_SEPARATOR, m_aSeparatorResources.GetValue() ) );
    ::std::map< USHORT, sal_Int32 >::const_iterator aIt( m_aListBoxToPlacementMap.find(m_aLB_LabelPlacement.GetSelectEntryPos()) );
    if(aIt!=m_aListBoxToPlacementMap.end())
    {
        sal_Int32 nValue = aIt->second;
        rOutAttrs.Put( SfxInt32Item( SCHATTR_DATADESCR_PLACEMENT, nValue ) );
    }

    return TRUE;
}

void DataLabelResources::Reset(const SfxItemSet& rInAttrs)
{
    // default state
    m_aCBSymbol.Enable( FALSE );

    lcl_setBoolItemToCheckBox( rInAttrs, SCHATTR_DATADESCR_SHOW_NUMBER, m_aCBNumber );
    lcl_setBoolItemToCheckBox( rInAttrs, SCHATTR_DATADESCR_SHOW_PERCENTAGE, m_aCBPercent );
    lcl_setBoolItemToCheckBox( rInAttrs, SCHATTR_DATADESCR_SHOW_CATEGORY, m_aCBCategory );
    lcl_setBoolItemToCheckBox( rInAttrs, SCHATTR_DATADESCR_SHOW_SYMBOL, m_aCBSymbol );

    m_bNumberFormatMixedState = !lcl_ReadNumberFormatFromItemSet( rInAttrs, SID_ATTR_NUMBERFORMAT_VALUE, SID_ATTR_NUMBERFORMAT_SOURCE, m_nNumberFormatForValue, m_bSourceFormatForValue, m_bSourceFormatMixedState );
    m_bPercentFormatMixedState = !lcl_ReadNumberFormatFromItemSet( rInAttrs, SCHATTR_PERCENT_NUMBERFORMAT_VALUE, SCHATTR_PERCENT_NUMBERFORMAT_SOURCE, m_nNumberFormatForPercent, m_bSourceFormatForPercent ,  m_bPercentSourceMixedState);

    const SfxPoolItem *pPoolItem = NULL;
    if( rInAttrs.GetItemState(SCHATTR_DATADESCR_SEPARATOR, TRUE, &pPoolItem) == SFX_ITEM_SET )
        m_aSeparatorResources.SetValue( ((const SfxStringItem*)pPoolItem)->GetValue() );
    else
        m_aSeparatorResources.SetDefault();

    if( rInAttrs.GetItemState(SCHATTR_DATADESCR_PLACEMENT, TRUE, &pPoolItem) == SFX_ITEM_SET )
    {
        sal_Int32 nPlacement = ((const SfxInt32Item*)pPoolItem)->GetValue();
        ::std::map< sal_Int32, USHORT >::const_iterator aIt( m_aPlacementToListBoxMap.find(nPlacement) );
        if(aIt!=m_aPlacementToListBoxMap.end())
        {
            USHORT nPos = aIt->second;
            m_aLB_LabelPlacement.SelectEntryPos( nPos );
        }
        else
            m_aLB_LabelPlacement.SetNoSelection();
    }
    else
        m_aLB_LabelPlacement.SetNoSelection();


    EnableControls();
}

//.............................................................................
} //namespace chart
//.............................................................................
