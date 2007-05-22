/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: VLegendSymbolFactory.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 19:26:43 $
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
#include "VLegendSymbolFactory.hxx"
#include "macros.hxx"
#include "PropertyMapper.hxx"
#include "ShapeFactory.hxx"
#include "ObjectIdentifier.hxx"

#ifndef _COM_SUN_STAR_DRAWING_LINESTYLE_HPP_
#include <com/sun/star/drawing/LineStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_SYMBOL_HPP_
#include <com/sun/star/chart2/Symbol.hpp>
#endif

// header for define DBG_ASSERT
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

// uncomment to disable line dashes at the border of boxes
// #define DISABLE_DASHES_AT_BORDER

using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;

namespace
{
void lcl_setPropetiesToShape(
    const Reference< beans::XPropertySet > & xProp,
    const Reference< drawing::XShape > & xShape,
    ::chart::VLegendSymbolFactory::tPropertyType ePropertyType )
{
    const ::chart::tPropertyNameMap & aFilledSeriesNameMap( ::chart::PropertyMapper::getPropertyNameMapForFilledSeriesProperties());
    const ::chart::tPropertyNameMap & aLineSeriesNameMap( ::chart::PropertyMapper::getPropertyNameMapForLineSeriesProperties());
    const ::chart::tPropertyNameMap & aLineNameMap( ::chart::PropertyMapper::getPropertyNameMapForLineProperties());
    const ::chart::tPropertyNameMap & aFillNameMap( ::chart::PropertyMapper::getPropertyNameMapForFillProperties());
    const ::chart::tPropertyNameMap & aFillLineNameMap( ::chart::PropertyMapper::getPropertyNameMapForFillAndLineProperties());

    Reference< beans::XPropertySet > xShapeProp( xShape, uno::UNO_QUERY );
    if( xProp.is() && xShapeProp.is() )
    {
        ::chart::tPropertyNameValueMap aValueMap;
        switch( ePropertyType )
        {
            case ::chart::VLegendSymbolFactory::PROP_TYPE_FILLED_SERIES:
                ::chart::PropertyMapper::getValueMap( aValueMap, aFilledSeriesNameMap, xProp );
                break;
            case ::chart::VLegendSymbolFactory::PROP_TYPE_LINE_SERIES:
                ::chart::PropertyMapper::getValueMap( aValueMap, aLineSeriesNameMap, xProp );
                break;
            case ::chart::VLegendSymbolFactory::PROP_TYPE_LINE:
                ::chart::PropertyMapper::getValueMap( aValueMap, aLineNameMap, xProp );
                break;
            case ::chart::VLegendSymbolFactory::PROP_TYPE_FILL:
                ::chart::PropertyMapper::getValueMap( aValueMap, aFillNameMap, xProp );
                break;
            case ::chart::VLegendSymbolFactory::PROP_TYPE_FILL_AND_LINE:
                ::chart::PropertyMapper::getValueMap( aValueMap, aFillLineNameMap, xProp );
                break;
        }

        ::chart::tNameSequence aPropNames;
        ::chart::tAnySequence aPropValues;
        ::chart::PropertyMapper::getMultiPropertyListsFromValueMap( aPropNames, aPropValues, aValueMap );
        ::chart::PropertyMapper::setMultiProperties( aPropNames, aPropValues, xShapeProp );
    }
}

} // anonymous namespace

namespace chart
{

// static
Reference< drawing::XShape > VLegendSymbolFactory::createSymbol(
    const Reference< drawing::XShapes > xSymbolContainer,
    chart2::LegendSymbolStyle eStyle,
    const Reference< lang::XMultiServiceFactory > & xShapeFactory,
    const Reference< beans::XPropertySet > & xLegendEntryProperties,
    tPropertyType ePropertyType, const uno::Any& rExplicitSymbol )
{
    Reference< drawing::XShape > xResult;

    if( ! (xSymbolContainer.is() &&
           xShapeFactory.is()))
        return xResult;

    xResult.set( xShapeFactory->createInstance(
                     C2U( "com.sun.star.drawing.GroupShape" )), uno::UNO_QUERY );
    xSymbolContainer->add( xResult );
    Reference< drawing::XShapes > xResultGroup( xResult, uno::UNO_QUERY );
    if( ! xResultGroup.is())
        return xResult;

    // aspect ratio of symbols is always 3:2
    awt::Size aBoundSize( 3000, 2000 );
    bool bUseBox = false;

    // add an invisible square box to maintain aspect ratio
    switch( eStyle )
    {
        case chart2::LegendSymbolStyle_BOX:
        case chart2::LegendSymbolStyle_HORIZONTAL_LINE:
        case chart2::LegendSymbolStyle_VERTICAL_LINE:
        case chart2::LegendSymbolStyle_DIAGONAL_LINE:
        case chart2::LegendSymbolStyle_LINE_WITH_BOX:
        case chart2::LegendSymbolStyle_LINE_WITH_SYMBOL:
        case chart2::LegendSymbolStyle_CIRCLE:
        {
            Reference< drawing::XShape > xBound( ShapeFactory(xShapeFactory).createInvisibleRectangle(
                xResultGroup, aBoundSize  ));
            break;
        }

        case chart2::LegendSymbolStyle_BAR:
        case chart2::LegendSymbolStyle_RECTANGLE:
        case chart2::LegendSymbolStyle_STRETCHED_RECTANGLE:
        case chart2::LegendSymbolStyle_USER_DEFINED:
        default:
            break;
    }

    // create symbol
    switch( eStyle )
    {
        case chart2::LegendSymbolStyle_BOX:
        case chart2::LegendSymbolStyle_BAR:
        case chart2::LegendSymbolStyle_RECTANGLE:
        case chart2::LegendSymbolStyle_STRETCHED_RECTANGLE:
        case chart2::LegendSymbolStyle_CIRCLE:
        {
            try
            {
                Reference< drawing::XShape > xShape;

                if( eStyle == chart2::LegendSymbolStyle_CIRCLE )
                    xShape.set( xShapeFactory->createInstance(
                                    C2U( "com.sun.star.drawing.EllipseShape" )), uno::UNO_QUERY );
                else
                    xShape.set( xShapeFactory->createInstance(
                                    C2U( "com.sun.star.drawing.RectangleShape" )), uno::UNO_QUERY );

                if( xShape.is())
                {
                    xResultGroup->add( xShape );
                    if( eStyle == chart2::LegendSymbolStyle_BOX ||
                        eStyle == chart2::LegendSymbolStyle_CIRCLE )
                    {
                        xShape->setSize( awt::Size( 2000, 2000 ));
                        xShape->setPosition( awt::Point( 500, 0 ));
                    }
                    else
                    {
                        xShape->setSize( aBoundSize );
                    }
                }

                lcl_setPropetiesToShape( xLegendEntryProperties, xShape, ePropertyType ); // PROP_TYPE_FILLED_SERIES );

#ifdef DISABLE_DASHES_AT_BORDER
                // don't allow dashed border style
                Reference< beans::XPropertySet > xShapeProp( xShape, uno::UNO_QUERY );
                if( xShapeProp.is())
                {
                    drawing::LineStyle aLineStyle;
                    if( ( xShapeProp->getPropertyValue( C2U("LineStyle")) >>= aLineStyle ) &&
                        aLineStyle == drawing::LineStyle_DASH )
                    {
                        aLineStyle = drawing::LineStyle_SOLID;
                        xShapeProp->setPropertyValue( C2U("LineStyle"), uno::makeAny( aLineStyle ));
                    }
                }
#endif
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
            break;
        }

        case chart2::LegendSymbolStyle_HORIZONTAL_LINE:
        {
            try
            {
                Reference< drawing::XShape > xLine(
                    xShapeFactory->createInstance(
                        C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY );
                if( xLine.is())
                {
                    xResultGroup->add( xLine );
                    xLine->setSize(  awt::Size( 3000, 0 ));
                    xLine->setPosition( awt::Point( 0, 1000 ));

                    lcl_setPropetiesToShape( xLegendEntryProperties, xLine, ePropertyType ); // PROP_TYPE_LINE_SERIES );
                }
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
            break;
        }

        case chart2::LegendSymbolStyle_VERTICAL_LINE:
        {
            try
            {
                Reference< drawing::XShape > xLine(
                    xShapeFactory->createInstance(
                        C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY );
                if( xLine.is())
                {
                    xResultGroup->add( xLine );
                    xLine->setSize(  awt::Size( 0, 2000 ));
                    xLine->setPosition( awt::Point( 1500, 0 ));

                    lcl_setPropetiesToShape( xLegendEntryProperties, xLine, ePropertyType ); // PROP_TYPE_LINE_SERIES );
                }
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
            break;
        }

        case chart2::LegendSymbolStyle_DIAGONAL_LINE:
        {
            try
            {
                Reference< drawing::XShape > xLine(
                    xShapeFactory->createInstance(
                        C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY );
                if( xLine.is())
                {
                    xResultGroup->add( xLine );
                    xLine->setSize(  awt::Size( 2000, 2000 ));
                    xLine->setPosition( awt::Point( 500, 0 ));

                    lcl_setPropetiesToShape( xLegendEntryProperties, xLine, ePropertyType ); // PROP_TYPE_LINE_SERIES );
                }
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
            break;
        }

        case chart2::LegendSymbolStyle_LINE_WITH_BOX:
            bUseBox = true;
            // fall-through intended
        case chart2::LegendSymbolStyle_LINE_WITH_SYMBOL:
            try
            {
                Reference< drawing::XShape > xLine(
                    xShapeFactory->createInstance(
                        C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY );
                if( xLine.is())
                {
                    xResultGroup->add( xLine );
                    xLine->setSize(  awt::Size( 3000, 0 ));
                    xLine->setPosition( awt::Point( 0, 1000 ));

                    lcl_setPropetiesToShape( xLegendEntryProperties, xLine, ePropertyType );
                }

                Reference< drawing::XShape > xSymbol;
                const sal_Int32 nSize = 1500;
                if( bUseBox )
                {
                    xSymbol.set( xShapeFactory->createInstance(
                                     C2U( "com.sun.star.drawing.RectangleShape" )), uno::UNO_QUERY );
                    xResultGroup->add( xSymbol );

                    if( xSymbol.is())
                    {
                        xSymbol->setSize( awt::Size( nSize, nSize ));
                        xSymbol->setPosition( awt::Point( 1500 - nSize/2, 1000 - nSize/2 ));

                        lcl_setPropetiesToShape( xLegendEntryProperties, xSymbol, ePropertyType );
                    }
                }
                else
                {
                    chart2::Symbol aSymbol;

                    if( rExplicitSymbol >>= aSymbol )
                    {
                        drawing::Direction3D aSymbolSize( nSize, nSize, 0 );
                        drawing::Position3D aPos( 1500, 1000, 0 );
                        ShapeFactory aFactory( xShapeFactory );
                        if( aSymbol.Style == chart2::SymbolStyle_STANDARD )
                        {
                            // border of symbols always black
                            aSymbol.BorderColor = 0x000000;
                            // take series color as fill color
                            xLegendEntryProperties->getPropertyValue( C2U("Color")) >>= aSymbol.FillColor;

                            xSymbol.set( aFactory.createSymbol2D(
                                             xResultGroup,
                                             aPos,
                                             aSymbolSize,
                                             aSymbol.StandardSymbol,
                                             aSymbol.BorderColor,
                                             aSymbol.FillColor ));
                        }
                        else if( aSymbol.Style == chart2::SymbolStyle_GRAPHIC )
                        {
                            xSymbol.set( aFactory.createGraphic2D(
                                             xResultGroup,
                                             aPos,
                                             aSymbolSize,
                                             aSymbol.Graphic ));
                        }
                        else if( aSymbol.Style == chart2::SymbolStyle_AUTO )
                        {
                            DBG_ERROR("the given parameter is not allowed to contain an automatic symbol style");
                        }
                    }
                }
            }
            catch( uno::Exception & ex )
            {
                ASSERT_EXCEPTION( ex );
            }
            break;

        case chart2::LegendSymbolStyle_USER_DEFINED:
            break;

        default:
            // just to remove warning (there is an auto-generated extra label)
            break;
    }

    return xResult;
}

// static
Reference< drawing::XShape >
    VLegendSymbolFactory::createJapaneseCandleStickSymbol(
        const Reference< drawing::XShapes > xSymbolContainer,
        const Reference< lang::XMultiServiceFactory > & xShapeFactory,
        const Reference< beans::XPropertySet > & xLegendEntryProperties,
        bool bWhiteDay )
{
    Reference< drawing::XShape > xResult;

    if( ! (xSymbolContainer.is() &&
           xShapeFactory.is()))
        return xResult;

    xResult.set( xShapeFactory->createInstance(
                     C2U( "com.sun.star.drawing.GroupShape" )), uno::UNO_QUERY );
    xSymbolContainer->add( xResult );
    Reference< drawing::XShapes > xResultGroup( xResult, uno::UNO_QUERY );
    if( ! xResultGroup.is())
        return xResult;

    // aspect ratio of symbols is always 3:2
    awt::Size aBoundSize( 3000, 2000 );

    try
    {
        // create bound
        Reference< drawing::XShape > xBound( ShapeFactory(xShapeFactory).createInvisibleRectangle(
            xResultGroup, aBoundSize  ));

        Reference< drawing::XShape > xLine(
            xShapeFactory->createInstance(
                C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY );
        if( xLine.is())
        {
            xResultGroup->add( xLine );
            xLine->setSize(  awt::Size( 0, 250 ));
            xLine->setPosition( awt::Point( 1500, 0 ));

            lcl_setPropetiesToShape( xLegendEntryProperties, xLine, PROP_TYPE_LINE );
        }
        xLine.set(
            xShapeFactory->createInstance(
                C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY );
        if( xLine.is())
        {
            xResultGroup->add( xLine );
            xLine->setSize(  awt::Size( 0, 250 ));
            xLine->setPosition( awt::Point( 1500, 1750 ));

            lcl_setPropetiesToShape( xLegendEntryProperties, xLine, PROP_TYPE_LINE );
        }

        Reference< drawing::XShape > xBox(
            xShapeFactory->createInstance(
                C2U( "com.sun.star.drawing.RectangleShape" )), uno::UNO_QUERY );
        if( xBox.is())
        {
            xResultGroup->add( xBox );
            sal_Int32 nSizeX = 1000;
            sal_Int32 nSizeY = 1500;
            xBox->setSize( awt::Size( nSizeX, nSizeY ));
            xBox->setPosition( awt::Point( 1500 - nSizeX/2, 1000 - nSizeY/2 ));

            // set CID to symbol for selection
            ShapeFactory::setShapeName(
                xBox, ObjectIdentifier::createClassifiedIdentifierWithParent(
                    OBJECTTYPE_LEGEND_ENTRY,
                    rtl::OUString(),
                    ObjectIdentifier::createChildParticleWithIndex( bWhiteDay ? OBJECTTYPE_DATA_STOCK_GAIN : OBJECTTYPE_DATA_STOCK_LOSS, 0 )
                     ));

            lcl_setPropetiesToShape( xLegendEntryProperties, xBox, PROP_TYPE_FILL_AND_LINE );
        }
    }
    catch( uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }

    return xResult;
}

// static
Reference< drawing::XShape >
    VLegendSymbolFactory::createStockLineSymbol(
        const Reference< drawing::XShapes > xSymbolContainer,
        const Reference< lang::XMultiServiceFactory > & xShapeFactory,
        const Reference< beans::XPropertySet > & xLegendEntryProperties,
        tStockLineType eType )
{
    Reference< drawing::XShape > xResult;

    if( ! (xSymbolContainer.is() &&
           xShapeFactory.is()))
        return xResult;

    xResult.set( xShapeFactory->createInstance(
                     C2U( "com.sun.star.drawing.GroupShape" )), uno::UNO_QUERY );
    xSymbolContainer->add( xResult );
    Reference< drawing::XShapes > xResultGroup( xResult, uno::UNO_QUERY );
    if( ! xResultGroup.is())
        return xResult;

    // aspect ratio of symbols is always 3:2
    awt::Size aBoundSize( 3000, 2000 );

    try
    {
        // create bound
        Reference< drawing::XShape > xBound( ShapeFactory(xShapeFactory).createInvisibleRectangle(
            xResultGroup, aBoundSize  ));

        Reference< drawing::XShape > xLine(
            xShapeFactory->createInstance(
                C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY );
        if( xLine.is())
        {
            xResultGroup->add( xLine );
            xLine->setSize(  awt::Size( 0, 2000 ));
            xLine->setPosition( awt::Point( 1500, 0 ));

            lcl_setPropetiesToShape( xLegendEntryProperties, xLine, PROP_TYPE_LINE_SERIES );
        }

        if( eType != STOCK_LINE_TYPE_VERT )
        {
            xLine.set(
                xShapeFactory->createInstance(
                    C2U( "com.sun.star.drawing.LineShape" )), uno::UNO_QUERY_THROW );
            xResultGroup->add( xLine );
            xLine->setSize( awt::Size( 500, 0 ));
            xLine->setPosition( awt::Point( ( eType == STOCK_LINE_TYPE_OPEN ) ? 1000 : 1500, 1000 ));

            lcl_setPropetiesToShape( xLegendEntryProperties, xLine, PROP_TYPE_LINE_SERIES );
        }
    }
    catch( uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }

    return xResult;
}

} //  namespace chart
