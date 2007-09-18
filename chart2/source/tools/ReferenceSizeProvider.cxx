/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ReferenceSizeProvider.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2007-09-18 15:09:57 $
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

#include "ReferenceSizeProvider.hxx"
#include "RelativeSizeHelper.hxx"
#include "ChartModelHelper.hxx"
#include "DiagramHelper.hxx"
#include "macros.hxx"
#include "AxisHelper.hxx"

#include <com/sun/star/chart2/XTitled.hpp>
#include <com/sun/star/chart2/XTitle.hpp>
#include <com/sun/star/chart2/XDataSeries.hpp>

#include <vector>

using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;

// ================================================================================

namespace chart
{

ReferenceSizeProvider::ReferenceSizeProvider(
    awt::Size aPageSize,
    awt::Size aDiagramSize,
    const Reference< XChartDocument > & xChartDoc ) :
        m_aPageSize( aPageSize ),
        m_aDiagramSize( aDiagramSize ),
        m_xChartDoc( xChartDoc ),
        m_bUseAutoScale( getAutoResizeState( xChartDoc ) == AUTO_RESIZE_YES )
{}

awt::Size ReferenceSizeProvider::getPageSize() const
{
    return m_aPageSize;
}

awt::Size ReferenceSizeProvider::getDiagramSize() const
{
    return m_aDiagramSize;
}

bool ReferenceSizeProvider::useAutoScale() const
{
    return m_bUseAutoScale;
}

void ReferenceSizeProvider::impl_setValuesAtTitled(
    const Reference< XTitled > & xTitled )
{
    if( xTitled.is())
    {
        Reference< XTitle > xTitle( xTitled->getTitleObject());
        if( xTitle.is())
            setValuesAtTitle( xTitle );
    }
}

void ReferenceSizeProvider::setValuesAtTitle(
    const Reference< XTitle > & xTitle )
{
    try
    {
        Reference< beans::XPropertySet > xTitleProp( xTitle, uno::UNO_QUERY_THROW );
        awt::Size aOldRefSize;
        bool bHasOldRefSize(
            xTitleProp->getPropertyValue( C2U("ReferencePageSize")) >>= aOldRefSize );

        // set from auto-resize on to off -> adapt font sizes at XFormattedStrings
        if( bHasOldRefSize && ! useAutoScale())
        {
            uno::Sequence< uno::Reference< XFormattedString > > aStrSeq(
                xTitle->getText());
            for( sal_Int32 i=0; i<aStrSeq.getLength(); ++i )
            {
                RelativeSizeHelper::adaptFontSizes(
                    Reference< beans::XPropertySet >( aStrSeq[i], uno::UNO_QUERY ),
                    aOldRefSize, getPageSize());
            }
        }

        setValuesAtPropertySet( xTitleProp, REF_PAGE, /* bAdaptFontSizes = */ false );
    }
    catch( const uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

void ReferenceSizeProvider::setValuesAtAllDataSeries()
{
    Reference< XDiagram > xDiagram( ChartModelHelper::findDiagram( m_xChartDoc ));

    // DataSeries/Points
    ::std::vector< Reference< XDataSeries > > aSeries(
        DiagramHelper::getDataSeriesFromDiagram( xDiagram ));

    for( ::std::vector< Reference< XDataSeries > >::const_iterator aIt( aSeries.begin());
         aIt != aSeries.end(); ++aIt )
    {
        Reference< beans::XPropertySet > xSeriesProp( *aIt, uno::UNO_QUERY );
        if( xSeriesProp.is())
        {
            // data points
            Sequence< sal_Int32 > aPointIndexes;
            try
            {
                if( xSeriesProp->getPropertyValue( C2U("AttributedDataPoints")) >>= aPointIndexes )
                {
                    for( sal_Int32 i=0; i< aPointIndexes.getLength(); ++i )
                        setValuesAtPropertySet(
                            (*aIt)->getDataPointByIndex( aPointIndexes[i] ), REF_DIAGRAM );
                }
            }
            catch( const uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }

            //it is important to correct the datapoint properties first as they do reference the series properties
            setValuesAtPropertySet( xSeriesProp, REF_DIAGRAM );
        }
    }
}

void ReferenceSizeProvider::setValuesAtPropertySet(
    const Reference< beans::XPropertySet > & xProp,
    ReferenceSizeProvider::ReferenceSizeType eType,
    bool bAdaptFontSizes /* = true */ )
{
    if( ! xProp.is())
        return;

    static const OUString aPageRefStr( RTL_CONSTASCII_USTRINGPARAM("ReferencePageSize"));
    static const OUString aDiaRefStr(  RTL_CONSTASCII_USTRINGPARAM("ReferenceDiagramSize"));

    try
    {
        OUString aRefSizeName( (eType == REF_PAGE) ? aPageRefStr : aDiaRefStr );
        awt::Size aRefSize( (eType == REF_PAGE) ? getPageSize() : getDiagramSize() );
        awt::Size aOldRefSize;
        bool bHasOldRefSize( xProp->getPropertyValue( aRefSizeName ) >>= aOldRefSize );

        if( useAutoScale())
        {
            if( ! bHasOldRefSize )
                xProp->setPropertyValue( aRefSizeName, uno::makeAny( aRefSize ));
        }
        else
        {
            if( bHasOldRefSize )
            {
                xProp->setPropertyValue( aRefSizeName, uno::Any());

                // adapt font sizes
                if( bAdaptFontSizes )
                    RelativeSizeHelper::adaptFontSizes( xProp, aOldRefSize, aRefSize );
            }
        }
    }
    catch( const uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

void ReferenceSizeProvider::getAutoResizeFromPropSet(
    const Reference< beans::XPropertySet > & xProp,
    ReferenceSizeProvider::ReferenceSizeType eType,
    ReferenceSizeProvider::AutoResizeState & rInOutState )
{
    static const OUString aPageRefStr( RTL_CONSTASCII_USTRINGPARAM("ReferencePageSize"));
    static const OUString aDiaRefStr(  RTL_CONSTASCII_USTRINGPARAM("ReferenceDiagramSize"));
    AutoResizeState eSingleState = AUTO_RESIZE_UNKNOWN;

    if( xProp.is())
    {
        try
        {
            if( xProp->getPropertyValue((eType == REF_PAGE) ? aPageRefStr : aDiaRefStr ).hasValue())
                eSingleState = AUTO_RESIZE_YES;
            else
                eSingleState = AUTO_RESIZE_NO;
        }
        catch( uno::Exception )
        {
            // unknown property -> state stays unknown
        }
    }

    // curent state unknown => nothing changes.  Otherwise if current state
    // differs from state so far, we have an ambiguity
    if( rInOutState == AUTO_RESIZE_UNKNOWN )
    {
        rInOutState = eSingleState;
    }
    else if( eSingleState != AUTO_RESIZE_UNKNOWN &&
        eSingleState != rInOutState )
    {
        rInOutState = AUTO_RESIZE_AMBIGUOUS;
    }
}

void ReferenceSizeProvider::getAutoResizeFromTitle(
    const Reference< XTitle > & xTitle,
    ReferenceSizeProvider::AutoResizeState & rInOutState )
{
    Reference< beans::XPropertySet > xProp( xTitle, uno::UNO_QUERY );
    if( xProp.is())
        getAutoResizeFromPropSet( xProp, REF_PAGE, rInOutState );
}

void ReferenceSizeProvider::impl_getAutoResizeFromTitled(
    const Reference< XTitled > & xTitled,
    ReferenceSizeProvider::AutoResizeState & rInOutState )
{
    if( xTitled.is())
    {
        Reference< XTitle > xTitle( xTitled->getTitleObject());
        if( xTitle.is())
            getAutoResizeFromTitle( xTitle, rInOutState );
    }
}

ReferenceSizeProvider::AutoResizeState ReferenceSizeProvider::getAutoResizeState() const
{
    return getAutoResizeState( m_xChartDoc );
}

/** Retrieves the state auto-resize from all objects that support this
    feature.  If all objects return the same state, AUTO_RESIZE_YES or
    AUTO_RESIZE_NO is returned.

    If no object supporting the feature is found, AUTO_RESIZE_UNKNOWN is
    returned.  If there are multiple objects, some with state YES and some
    with state NO, AUTO_RESIZE_AMBIGUOUS is returned.
*/
ReferenceSizeProvider::AutoResizeState ReferenceSizeProvider::getAutoResizeState(
    const Reference< XChartDocument > & xChartDoc )
{
    AutoResizeState eResult = AUTO_RESIZE_UNKNOWN;

    // Main Title
    Reference< XTitled > xDocTitled( xChartDoc, uno::UNO_QUERY );
    if( xDocTitled.is())
        impl_getAutoResizeFromTitled( xDocTitled, eResult );
    if( eResult == AUTO_RESIZE_AMBIGUOUS )
        return eResult;

    // diagram is needed by the rest of the objects
    Reference< XDiagram > xDiagram( ChartModelHelper::findDiagram( xChartDoc ), uno::UNO_QUERY );
    if( ! xDiagram.is())
        return eResult;

    // Sub Title
    Reference< XTitled > xDiaTitled( xDiagram, uno::UNO_QUERY );
    if( xDiaTitled.is())
        impl_getAutoResizeFromTitled( xDiaTitled, eResult );
    if( eResult == AUTO_RESIZE_AMBIGUOUS )
        return eResult;

    // Legend
    Reference< beans::XPropertySet > xLegendProp( xDiagram->getLegend(), uno::UNO_QUERY );
    if( xLegendProp.is())
        getAutoResizeFromPropSet( xLegendProp, REF_PAGE, eResult );
    if( eResult == AUTO_RESIZE_AMBIGUOUS )
        return eResult;

    // Axes (incl. Axis Titles)
    Sequence< Reference< XAxis > > aAxes( AxisHelper::getAllAxesOfDiagram( xDiagram ) );
    for( sal_Int32 i=0; i<aAxes.getLength(); ++i )
    {
        Reference< beans::XPropertySet > xProp( aAxes[i], uno::UNO_QUERY );
        if( xProp.is())
            getAutoResizeFromPropSet( xProp, REF_DIAGRAM, eResult );
        Reference< XTitled > xTitled( aAxes[i], uno::UNO_QUERY );
        if( xTitled.is())
        {
            impl_getAutoResizeFromTitled( xTitled, eResult );
            if( eResult == AUTO_RESIZE_AMBIGUOUS )
                return eResult;
        }
    }

    // DataSeries/Points
    ::std::vector< Reference< XDataSeries > > aSeries(
        DiagramHelper::getDataSeriesFromDiagram( xDiagram ));

    for( ::std::vector< Reference< XDataSeries > >::const_iterator aIt( aSeries.begin());
         aIt != aSeries.end(); ++aIt )
    {
        Reference< beans::XPropertySet > xSeriesProp( *aIt, uno::UNO_QUERY );
        if( xSeriesProp.is())
        {
            getAutoResizeFromPropSet( xSeriesProp, REF_DIAGRAM, eResult );
            if( eResult == AUTO_RESIZE_AMBIGUOUS )
                return eResult;

            // data points
            Sequence< sal_Int32 > aPointIndexes;
            try
            {
                if( xSeriesProp->getPropertyValue( C2U("AttributedDataPoints")) >>= aPointIndexes )
                {
                    for( sal_Int32 i=0; i< aPointIndexes.getLength(); ++i )
                    {
                        getAutoResizeFromPropSet(
                            (*aIt)->getDataPointByIndex( aPointIndexes[i] ),
                            REF_DIAGRAM, eResult );
                        if( eResult == AUTO_RESIZE_AMBIGUOUS )
                            return eResult;
                    }
                }
            }
            catch( const uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
        }
    }

    return eResult;
}

void ReferenceSizeProvider::toggleAutoResizeState()
{
    setAutoResizeState( m_bUseAutoScale ? AUTO_RESIZE_NO : AUTO_RESIZE_YES );
}


/** sets the auto-resize at all objects that support this feature for text.
    eNewState must be either AUTO_RESIZE_YES or AUTO_RESIZE_NO
*/
void ReferenceSizeProvider::setAutoResizeState( ReferenceSizeProvider::AutoResizeState eNewState )
{
    m_bUseAutoScale = (eNewState == AUTO_RESIZE_YES);

    // Main Title
    impl_setValuesAtTitled( Reference< XTitled >( m_xChartDoc, uno::UNO_QUERY ));

    // diagram is needed by the rest of the objects
    Reference< XDiagram > xDiagram( ChartModelHelper::findDiagram( m_xChartDoc ), uno::UNO_QUERY );
    if( ! xDiagram.is())
        return;

    // Sub Title
    impl_setValuesAtTitled( Reference< XTitled >( xDiagram, uno::UNO_QUERY ));

    // Legend
    Reference< beans::XPropertySet > xLegendProp( xDiagram->getLegend(), uno::UNO_QUERY );
    if( xLegendProp.is())
        setValuesAtPropertySet( xLegendProp, REF_PAGE );

    // Axes (incl. Axis Titles)
    Sequence< Reference< XAxis > > aAxes( AxisHelper::getAllAxesOfDiagram( xDiagram ) );
    for( sal_Int32 i=0; i<aAxes.getLength(); ++i )
    {
        Reference< beans::XPropertySet > xProp( aAxes[i], uno::UNO_QUERY );
        if( xProp.is())
            setValuesAtPropertySet( xProp, REF_DIAGRAM );
        impl_setValuesAtTitled( Reference< XTitled >( aAxes[i], uno::UNO_QUERY ));
    }

    // DataSeries/Points
    setValuesAtAllDataSeries();

    // recalculate new state (in case it stays unknown or is ambiguous
    m_bUseAutoScale = (getAutoResizeState( m_xChartDoc ) == AUTO_RESIZE_YES);
}

} //  namespace chart
