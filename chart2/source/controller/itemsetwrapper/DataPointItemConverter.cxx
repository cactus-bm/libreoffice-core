/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: DataPointItemConverter.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-25 08:40:39 $
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
#include "DataPointItemConverter.hxx"
#include "SchWhichPairs.hxx"
#include "macros.hxx"
#include "ItemPropertyMap.hxx"

#include "GraphicPropertyItemConverter.hxx"
#include "CharacterPropertyItemConverter.hxx"
#include "StatisticsItemConverter.hxx"
#include "SeriesOptionsItemConverter.hxx"
#include "DataSeriesHelper.hxx"

#ifndef _SVX_CHRTITEM_HXX
#include <svx/chrtitem.hxx>
#endif
#ifndef _COM_SUN_STAR_CHART2_DATAPOINTLABEL_HPP_
#include <com/sun/star/chart2/DataPointLabel.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_SYMBOL_HPP_
#include <com/sun/star/chart2/Symbol.hpp>
#endif

// header for class XFillColorItem
#ifndef _SVX_XFLCLIT_HXX
#include <svx/xflclit.hxx>
#endif
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SVX_SIZEITEM_HXX
#include <svx/sizeitem.hxx>
#endif

#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif
#ifndef _SV_GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _COM_SUN_STAR_GRAPHIC_XGRAPHIC_HPP_
#include <com/sun/star/graphic/XGraphic.hpp>
#endif

// for SVX_SYMBOLTYPE_...
#ifndef _SVX_TAB_LINE_HXX
#include <svx/tabline.hxx>
#endif

#include <functional>
#include <algorithm>

using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;

namespace
{
::comphelper::ItemPropertyMapType & lcl_GetDataPointPropertyMap()
{
    static ::comphelper::ItemPropertyMapType aDataPointPropertyMap(
        ::comphelper::MakeItemPropertyMap
//        IPM_MAP_ENTRY( CHATTR_PIE_SEGMENT_OFFSET, "Offset", 0 )
        IPM_MAP_ENTRY( SCHATTR_STYLE_SHAPE, "Geometry3D", 0 )
        );

    return aDataPointPropertyMap;
};

sal_Int32 lcl_getSymbolStyleForSymbol( const chart2::Symbol & rSymbol )
{
    sal_Int32 nStyle = SVX_SYMBOLTYPE_UNKNOWN;
    switch( rSymbol.Style )
    {
        case chart2::SymbolStyle_NONE:
            nStyle = SVX_SYMBOLTYPE_NONE;
            break;
        case chart2::SymbolStyle_AUTO:
            nStyle = SVX_SYMBOLTYPE_AUTO;
            break;
        case chart2::SymbolStyle_GRAPHIC:
            nStyle = SVX_SYMBOLTYPE_BRUSHITEM;
            break;
        case chart2::SymbolStyle_STANDARD:
            nStyle = rSymbol.StandardSymbol;
            break;

        case chart2::SymbolStyle_POLYGON:
            // to avoid warning
        case chart2::SymbolStyle_MAKE_FIXED_SIZE:
            // nothing
            break;
    }
    return nStyle;
}
} // anonymous namespace

namespace chart
{
namespace wrapper
{

DataPointItemConverter::DataPointItemConverter(
    const uno::Reference< frame::XModel > & xChartModel,
    const uno::Reference< uno::XComponentContext > & xContext,
    const uno::Reference< beans::XPropertySet > & rPropertySet,
    SfxItemPool& rItemPool,
    SdrModel& rDrawModel,
    NumberFormatterWrapper * pNumFormatter,
    const uno::Reference< lang::XMultiServiceFactory > & xNamedPropertyContainerFactory,
    GraphicPropertyItemConverter::eGraphicObjectType eMapTo /* = FILL_PROPERTIES */,
    ::std::auto_ptr< awt::Size > pRefSize /* = NULL */,
    bool bDataSeries /* = false */,
    bool bUseSpecialFillColor /* = false */,
    sal_Int32 nSpecialFillColor /* =0 */,
    bool bOverwriteLabelsForAttributedDataPointsAlso /*false*/
    ) :
        ItemConverter( rPropertySet, rItemPool ),
        m_pNumberFormatterWrapper( pNumFormatter ),
        m_bDataSeries( bDataSeries ),
        m_bOverwriteLabelsForAttributedDataPointsAlso(m_bDataSeries && bOverwriteLabelsForAttributedDataPointsAlso),
        m_bUseSpecialFillColor(bUseSpecialFillColor),
        m_nSpecialFillColor(nSpecialFillColor)
{
    m_aConverters.push_back( new GraphicPropertyItemConverter(
                                 rPropertySet, rItemPool, rDrawModel, xNamedPropertyContainerFactory, eMapTo ));
    m_aConverters.push_back( new CharacterPropertyItemConverter( rPropertySet, rItemPool, pRefSize,
                                                                 C2U( "ReferenceDiagramSize" )));
    if( bDataSeries )
    {
        m_aConverters.push_back( new StatisticsItemConverter( xChartModel, rPropertySet, rItemPool ));
        m_aConverters.push_back( new SeriesOptionsItemConverter( xChartModel, xContext, rPropertySet, rItemPool ));
    }
}

DataPointItemConverter::~DataPointItemConverter()
{
    ::std::for_each( m_aConverters.begin(), m_aConverters.end(),
                     ::comphelper::DeleteItemConverterPtr() );
}

void DataPointItemConverter::FillItemSet( SfxItemSet & rOutItemSet ) const
{
    ::std::for_each( m_aConverters.begin(), m_aConverters.end(),
                     ::comphelper::FillItemSetFunc( rOutItemSet ));

    // own items
    ItemConverter::FillItemSet( rOutItemSet );

    if( m_bUseSpecialFillColor )
    {
        Color aColor(m_nSpecialFillColor);
        rOutItemSet.Put( XFillColorItem( String(), aColor ) );
    }
}

bool DataPointItemConverter::ApplyItemSet( const SfxItemSet & rItemSet )
{
    bool bResult = false;

    ::std::for_each( m_aConverters.begin(), m_aConverters.end(),
                     ::comphelper::ApplyItemSetFunc( rItemSet, bResult ));

    // own items
    return ItemConverter::ApplyItemSet( rItemSet ) || bResult;
}

const USHORT * DataPointItemConverter::GetWhichPairs() const
{
    // must span all used items!
    if( m_bDataSeries )
        return nRowWhichPairs;
    return nDataPointWhichPairs;
}

bool DataPointItemConverter::GetItemProperty( tWhichIdType nWhichId, tPropertyNameWithMemberId & rOutProperty ) const
{
    ::comphelper::ItemPropertyMapType & rMap( lcl_GetDataPointPropertyMap());
    ::comphelper::ItemPropertyMapType::const_iterator aIt( rMap.find( nWhichId ));

    if( aIt == rMap.end())
        return false;

    rOutProperty =(*aIt).second;
    return true;
}


bool DataPointItemConverter::ApplySpecialItem(
    USHORT nWhichId, const SfxItemSet & rItemSet )
    throw( uno::Exception )
{
    bool bChanged = false;
    uno::Any aValue;

    switch( nWhichId )
    {
        case SCHATTR_DATADESCR_DESCR:
        {
            const SvxChartDataDescrItem & rItem =
                static_cast< const SvxChartDataDescrItem & >(
                    rItemSet.Get( nWhichId ));

            chart2::DataPointLabel aLabel;
            if( GetPropertySet()->getPropertyValue( C2U( "Label" )) >>= aLabel )
            {
                switch( rItem.GetValue())
                {
                    case CHDESCR_NONE:
                        aLabel.ShowNumber = sal_False;
                        aLabel.ShowNumberInPercent = sal_False;
                        aLabel.ShowCategoryName = sal_False;
                        break;
                    case CHDESCR_VALUE:
                        aLabel.ShowNumber = sal_True;
                        aLabel.ShowNumberInPercent = sal_False;
                        aLabel.ShowCategoryName = sal_False;
                        break;
                    case CHDESCR_PERCENT:
                        aLabel.ShowNumber = sal_False;
                        aLabel.ShowNumberInPercent = sal_True;
                        aLabel.ShowCategoryName = sal_False;
                        break;
                    case CHDESCR_TEXT:
                        aLabel.ShowNumber = sal_False;
                        aLabel.ShowNumberInPercent = sal_False;
                        aLabel.ShowCategoryName = sal_True;
                        break;
                    case CHDESCR_TEXTANDPERCENT:
                        aLabel.ShowNumber = sal_False;
                        aLabel.ShowNumberInPercent = sal_True;
                        aLabel.ShowCategoryName = sal_True;
                        break;
                    case CHDESCR_TEXTANDVALUE:
                        aLabel.ShowNumber = sal_True;
                        aLabel.ShowNumberInPercent = sal_False;
                        aLabel.ShowCategoryName = sal_True;
                        break;
                    default:
                        break;
                }

                aValue <<= aLabel;

                uno::Any aOldValue( GetPropertySet()->getPropertyValue( C2U( "Label" ) ));
                if( m_bOverwriteLabelsForAttributedDataPointsAlso )
                {
                    Reference< chart2::XDataSeries > xSeries( GetPropertySet(), uno::UNO_QUERY);
                    if( aValue != aOldValue ||
                        DataSeriesHelper::hasAttributedDataPointDifferentValue( xSeries, C2U( "Label" ), aOldValue ) )
                    {
                        DataSeriesHelper::setPropertyAlsoToAllAttributedDataPoints( xSeries, C2U( "Label" ), aValue );
                        bChanged = true;
                    }
                }
                else if( aValue != aOldValue )
                {
                    GetPropertySet()->setPropertyValue( C2U( "Label" ), aValue );
                    bChanged = true;
                }
            }
        }
        break;

        case SCHATTR_DATADESCR_SHOW_SYM:
        {
            const SvxChartDataDescrItem & rItem =
                static_cast< const SvxChartDataDescrItem & >(
                    rItemSet.Get( nWhichId ));

            uno::Any aOldValue( GetPropertySet()->getPropertyValue( C2U( "Label" ) ));
            chart2::DataPointLabel aLabel;
            if( aOldValue >>= aLabel )
            {
                sal_Bool bOldValue = aLabel.ShowLegendSymbol;
                aLabel.ShowLegendSymbol = static_cast< sal_Bool >( rItem.GetValue() );
                if( m_bOverwriteLabelsForAttributedDataPointsAlso )
                {
                    Reference< chart2::XDataSeries > xSeries( GetPropertySet(), uno::UNO_QUERY);
                    if( bOldValue != aLabel.ShowLegendSymbol ||
                        DataSeriesHelper::hasAttributedDataPointDifferentValue( xSeries, C2U( "Label" ), aOldValue ) )
                    {
                        DataSeriesHelper::setPropertyAlsoToAllAttributedDataPoints( xSeries, C2U( "Label" ), uno::makeAny( aLabel ) );
                        bChanged = true;
                    }
                }
                else if( bOldValue != aLabel.ShowLegendSymbol )
                {
                    GetPropertySet()->setPropertyValue( C2U( "Label" ), uno::makeAny( aLabel ));
                    bChanged = true;
                }
            }
        }
        break;

        case SID_ATTR_NUMBERFORMAT_VALUE:
        {
            if( m_pNumberFormatterWrapper )
            {
                sal_Int32 nFmt = static_cast< sal_Int32 >(
                    static_cast< const SfxUInt32Item & >(
                        rItemSet.Get( nWhichId )).GetValue());

                aValue = uno::makeAny(nFmt);
                if( GetPropertySet()->getPropertyValue( C2U( "NumberFormat" )) != aValue )
                {
                    GetPropertySet()->setPropertyValue( C2U( "NumberFormat" ), aValue );
                    bChanged = true;
                }
            }
            else
            {
                OSL_ENSURE( false, "No NumberFormatterWrapper !" );
            }
        }
        break;

        case SCHATTR_STYLE_SYMBOL:
        {
            sal_Int32 nStyle =
                static_cast< const SfxInt32Item & >(
                    rItemSet.Get( nWhichId )).GetValue();
            chart2::Symbol aSymbol;

            GetPropertySet()->getPropertyValue( C2U( "Symbol" )) >>= aSymbol;
            sal_Int32 nOldStyle = lcl_getSymbolStyleForSymbol( aSymbol );

            if( nStyle != nOldStyle )
            {
                bool bDeleteSymbol = false;
                switch( nStyle )
                {
                    case SVX_SYMBOLTYPE_NONE:
                        aSymbol.Style = chart2::SymbolStyle_NONE;
                        break;
                    case SVX_SYMBOLTYPE_AUTO:
                        aSymbol.Style = chart2::SymbolStyle_AUTO;
                        break;
                    case SVX_SYMBOLTYPE_BRUSHITEM:
                        aSymbol.Style = chart2::SymbolStyle_GRAPHIC;
                        break;
                    case SVX_SYMBOLTYPE_UNKNOWN:
                        bDeleteSymbol = true;
                        break;

                    default:
                        aSymbol.Style = chart2::SymbolStyle_STANDARD;
                        aSymbol.StandardSymbol = nStyle;
                }

                if( bDeleteSymbol )
                    GetPropertySet()->setPropertyValue( C2U( "Symbol" ), uno::Any());
                else
                    GetPropertySet()->setPropertyValue( C2U( "Symbol" ),
                                                        uno::makeAny( aSymbol ));
                bChanged = true;
            }
        }
        break;

        case SCHATTR_SYMBOL_SIZE:
        {
            Size aSize = static_cast< const SvxSizeItem & >(
                rItemSet.Get( nWhichId )).GetSize();
            chart2::Symbol aSymbol;

            GetPropertySet()->getPropertyValue( C2U( "Symbol" )) >>= aSymbol;
            if( aSize.getWidth() != aSymbol.Size.Width ||
                aSize.getHeight() != aSymbol.Size.Height )
            {
                aSymbol.Size.Width = aSize.getWidth();
                aSymbol.Size.Height = aSize.getHeight();

                GetPropertySet()->setPropertyValue( C2U( "Symbol" ), uno::makeAny( aSymbol ));
                bChanged = true;
            }
        }
        break;

        case SCHATTR_SYMBOL_BRUSH:
        {
            const SvxBrushItem & rBrshItem( static_cast< const SvxBrushItem & >(
                                                rItemSet.Get( nWhichId )));
            uno::Any aXGraphicAny;
            const Graphic *pGraphic( rBrshItem.GetGraphic());
            if( pGraphic )
            {
                uno::Reference< graphic::XGraphic > xGraphic( pGraphic->GetXGraphic());
                if( xGraphic.is())
                {
                    aXGraphicAny <<= xGraphic;
                    chart2::Symbol aSymbol;
                    GetPropertySet()->getPropertyValue( C2U( "Symbol" )) >>= aSymbol;
                    if( aSymbol.Graphic != xGraphic )
                    {
                        aSymbol.Graphic = xGraphic;
                        GetPropertySet()->setPropertyValue( C2U( "Symbol" ), uno::makeAny( aSymbol ));
                        bChanged = true;
                    }
                }
            }
        }
        break;
    }

    return bChanged;
}

void DataPointItemConverter::FillSpecialItem(
    USHORT nWhichId, SfxItemSet & rOutItemSet ) const
    throw( uno::Exception )
{
    switch( nWhichId )
    {
        case SCHATTR_DATADESCR_DESCR:
        case SCHATTR_DATADESCR_SHOW_SYM:
        {
            chart2::DataPointLabel aLabel;
            if( GetPropertySet()->getPropertyValue( C2U( "Label" )) >>= aLabel )
            {
                SvxChartDataDescr aDescr;

                if( aLabel.ShowNumber )
                {
                    if( aLabel.ShowCategoryName )
                        aDescr = CHDESCR_TEXTANDVALUE;
                    else
                        aDescr = CHDESCR_VALUE;
                }
                else if( aLabel.ShowNumberInPercent )
                {
                    if( aLabel.ShowCategoryName )
                        aDescr = CHDESCR_TEXTANDPERCENT;
                    else
                        aDescr = CHDESCR_PERCENT;
                }
                else
                {
                    if( aLabel.ShowCategoryName )
                        aDescr = CHDESCR_TEXT;
                    else
                        aDescr = CHDESCR_NONE;
                }

                rOutItemSet.Put( SvxChartDataDescrItem( aDescr, SCHATTR_DATADESCR_DESCR ));
                rOutItemSet.Put( SfxBoolItem( SCHATTR_DATADESCR_SHOW_SYM, aLabel.ShowLegendSymbol ));

                if( m_bOverwriteLabelsForAttributedDataPointsAlso )
                {
                    if( DataSeriesHelper::hasAttributedDataPointDifferentValue(
                        Reference< chart2::XDataSeries >( GetPropertySet(), uno::UNO_QUERY), C2U( "Label" ), uno::makeAny(aLabel) ) )
                    {
                        rOutItemSet.InvalidateItem(SCHATTR_DATADESCR_DESCR);
                        rOutItemSet.InvalidateItem(SCHATTR_DATADESCR_SHOW_SYM);
                    }
                }
            }
        }
        break;

        case SID_ATTR_NUMBERFORMAT_VALUE:
        {
            if( m_pNumberFormatterWrapper )
            {
                sal_Int32 nKey;
                if( GetPropertySet()->getPropertyValue( C2U( "NumberFormat" )) >>= nKey )
                {
                    rOutItemSet.Put( SfxUInt32Item( nWhichId, nKey ));
                }
            }
            else
            {
                OSL_ENSURE( false, "No NumberFormatterWrapper !" );
            }
        }
        break;

        case SCHATTR_STYLE_SYMBOL:
        {
            chart2::Symbol aSymbol;
            if( GetPropertySet()->getPropertyValue( C2U( "Symbol" )) >>= aSymbol )
                rOutItemSet.Put( SfxInt32Item( nWhichId, lcl_getSymbolStyleForSymbol( aSymbol ) ));
        }
        break;

        case SCHATTR_SYMBOL_SIZE:
        {
            chart2::Symbol aSymbol;
            if( GetPropertySet()->getPropertyValue( C2U( "Symbol" )) >>= aSymbol )
                rOutItemSet.Put(
                    SvxSizeItem( nWhichId, Size( aSymbol.Size.Width, aSymbol.Size.Height ) ));
        }
        break;

        case SCHATTR_SYMBOL_BRUSH:
        {
            chart2::Symbol aSymbol;
            if(( GetPropertySet()->getPropertyValue( C2U( "Symbol" )) >>= aSymbol )
               && aSymbol.Graphic.is() )
            {
                rOutItemSet.Put( SvxBrushItem( Graphic( aSymbol.Graphic ), GPOS_MM, SCHATTR_SYMBOL_BRUSH ));
            }
        }
        break;
   }
}

} //  namespace wrapper
} //  namespace chart
