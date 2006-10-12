/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: shapeexport4.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 14:42:02 $
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
#include "precompiled_xmloff.hxx"

#ifndef _B3D_HMATRIX_HXX
#include <goodies/hmatrix.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_HOMOGENMATRIX_HPP_
#include <com/sun/star/drawing/HomogenMatrix.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_POLYPOLYGONSHAPE3D_HPP_
#include <com/sun/star/drawing/PolyPolygonShape3D.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_PROJECTIONMODE_HPP_
#include <com/sun/star/drawing/ProjectionMode.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_SHADEMODE_HPP_
#include <com/sun/star/drawing/ShadeMode.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_DIRECTION3D_HPP_
#include <com/sun/star/drawing/Direction3D.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_POSITION3D_HPP_
#include <com/sun/star/drawing/Position3D.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_CAMERAGEOMETRY_HPP_
#include <com/sun/star/drawing/CameraGeometry.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_DOUBLESEQUENCE_HPP_
#include <com/sun/star/drawing/DoubleSequence.hpp>
#endif

#ifndef _XMLOFF_SHAPEEXPORT_HXX
#include "shapeexport.hxx"
#endif

#ifndef _SDPROPLS_HXX
#include "sdpropls.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _XEXPTRANSFORM_HXX
#include "xexptran.hxx"
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif

#ifndef _ENHANCED_CUSTOMSHAPE_TOKEN_HXX
#include "EnhancedCustomShapeToken.hxx"
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XIDENTIFIERCONTAINER_HPP_
#include <com/sun/star/container/XIdentifierContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_SHADEMODE_HPP_
#include <com/sun/star/drawing/ShadeMode.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPEPARAMETERTYPE_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeParameterType.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPEPARAMETERPARI_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeParameterPair.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPEGLUEPOINTTYPE_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeGluePointType.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPETEXTFRAME_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeTextFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPESEGMENT_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeSegment.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPESEGMENTCOMMAND_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeSegmentCommand.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPEADJUSTMENTVALUE_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeAdjustmentValue.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPETEXTPATHMODE_HPP_
#include <com/sun/star/drawing/EnhancedCustomShapeTextPathMode.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUES_HPP_
#include <com/sun/star/beans/PropertyValues.hpp>
#endif
#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#include "xmlnmspe.hxx"

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::xmloff::token;
using namespace ::xmloff::EnhancedCustomShapeToken;

//////////////////////////////////////////////////////////////////////////////

void ExportParameter( rtl::OUStringBuffer& rStrBuffer, const com::sun::star::drawing::EnhancedCustomShapeParameter& rParameter )
{
    if ( rStrBuffer.getLength() )
        rStrBuffer.append( (sal_Unicode)' ' );
    if ( rParameter.Value.getValueTypeClass() == uno::TypeClass_DOUBLE )
    {
        double fNumber = 0.0;
        rParameter.Value >>= fNumber;
        ::rtl::math::doubleToUStringBuffer( rStrBuffer, fNumber, rtl_math_StringFormat_Automatic, rtl_math_DecimalPlaces_Max, '.', sal_True );
    }
    else
    {
        sal_Int32 nValue = 0;
        rParameter.Value >>= nValue;

        switch( rParameter.Type )
        {
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::EQUATION :
            {
                rStrBuffer.append( (sal_Unicode)'?' );
                rStrBuffer.append( (sal_Unicode)'f' );
                rStrBuffer.append( rtl::OUString::valueOf( nValue ) );
            }
            break;

            case com::sun::star::drawing::EnhancedCustomShapeParameterType::ADJUSTMENT :
            {
                rStrBuffer.append( (sal_Unicode)'$' );
                rStrBuffer.append( rtl::OUString::valueOf( nValue ) );
            }
            break;

            case com::sun::star::drawing::EnhancedCustomShapeParameterType::BOTTOM :
                rStrBuffer.append( GetXMLToken( XML_BOTTOM ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::RIGHT :
                rStrBuffer.append( GetXMLToken( XML_RIGHT ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::TOP :
                rStrBuffer.append( GetXMLToken( XML_TOP ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::LEFT :
                rStrBuffer.append( GetXMLToken( XML_LEFT ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::XSTRETCH :
                rStrBuffer.append( GetXMLToken( XML_XSTRETCH ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::YSTRETCH :
                rStrBuffer.append( GetXMLToken( XML_YSTRETCH ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::HASSTROKE :
                rStrBuffer.append( GetXMLToken( XML_HASSTROKE ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::HASFILL :
                rStrBuffer.append( GetXMLToken( XML_HASFILL ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::WIDTH :
                rStrBuffer.append( GetXMLToken( XML_WIDTH ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::HEIGHT :
                rStrBuffer.append( GetXMLToken( XML_HEIGHT ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::LOGWIDTH :
                rStrBuffer.append( GetXMLToken( XML_LOGWIDTH ) ); break;
            case com::sun::star::drawing::EnhancedCustomShapeParameterType::LOGHEIGHT :
                rStrBuffer.append( GetXMLToken( XML_LOGHEIGHT ) ); break;
            default :
                rStrBuffer.append( rtl::OUString::valueOf( nValue ) );
        }
    }
}

void ImpExportEquations( SvXMLExport& rExport, const uno::Sequence< rtl::OUString >& rEquations )
{
    sal_Int32 i;
    for ( i = 0; i < rEquations.getLength(); i++ )
    {
        rtl::OUString aStr( String( 'f' ) );
        aStr += rtl::OUString::valueOf( i );
        rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_NAME, aStr );

        aStr = rEquations[ i ];
        sal_Int32 nIndex = 0;
        do
        {
            nIndex = aStr.indexOf( (sal_Unicode)'?', nIndex );
            if ( nIndex != -1 )
            {
                rtl::OUString aNew( aStr.copy( 0, nIndex + 1 ) );
                aNew += String( 'f' );
                aNew += aStr.copy( nIndex + 1, ( aStr.getLength() - nIndex ) - 1 );
                aStr = aNew;
                nIndex++;
            }
        } while( nIndex != -1 );
        rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_FORMULA, aStr );
        SvXMLElementExport aOBJ( rExport, XML_NAMESPACE_DRAW, XML_EQUATION, sal_True, sal_True );
    }
}

void ImpExportHandles( SvXMLExport& rExport, const uno::Sequence< beans::PropertyValues >& rHandles )
{
    sal_uInt32 i, j, nElements = rHandles.getLength();
    if ( nElements )
    {
        rtl::OUString       aStr;
        rtl::OUStringBuffer aStrBuffer;

        for ( i = 0; i < nElements; i++ )
        {
            const uno::Sequence< beans::PropertyValue >& rPropSeq = rHandles[ i ];
            for ( j = 0; j < (sal_uInt32)rPropSeq.getLength(); j++ )
            {
                const beans::PropertyValue& rPropVal = rPropSeq[ j ];
                switch( EASGet( rPropVal.Name ) )
                {
                    case EAS_Position :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameterPair aPosition;
                        if ( rPropVal.Value >>= aPosition )
                        {
                            ExportParameter( aStrBuffer, aPosition.First );
                            ExportParameter( aStrBuffer, aPosition.Second );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_POSITION, aStr );
                        }
                    }
                    break;
                    case EAS_MirroredX :
                    {
                        sal_Bool bMirroredX = sal_Bool();
                        if ( rPropVal.Value >>= bMirroredX )
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_MIRROR_HORIZONTAL,
                                bMirroredX ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                    }
                    break;
                    case EAS_MirroredY :
                    {
                        sal_Bool bMirroredY = sal_Bool();
                        if ( rPropVal.Value >>= bMirroredY )
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_MIRROR_VERTICAL,
                                bMirroredY ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                    }
                    break;
                    case EAS_Switched :
                    {
                        sal_Bool bSwitched = sal_Bool();
                        if ( rPropVal.Value >>= bSwitched )
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_SWITCHED,
                                bSwitched ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                    }
                    break;
                    case EAS_Polar :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameterPair aPolar;
                        if ( rPropVal.Value >>= aPolar )
                        {
                            ExportParameter( aStrBuffer, aPolar.First );
                            ExportParameter( aStrBuffer, aPolar.Second );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_POLAR, aStr );
                        }
                    }
                    break;
                    case EAS_RadiusRangeMinimum :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameter aRadiusRangeMinimum;
                        if ( rPropVal.Value >>= aRadiusRangeMinimum )
                        {
                            ExportParameter( aStrBuffer, aRadiusRangeMinimum );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_RADIUS_RANGE_MINIMUM, aStr );
                        }
                    }
                    break;
                    case EAS_RadiusRangeMaximum :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameter aRadiusRangeMaximum;
                        if ( rPropVal.Value >>= aRadiusRangeMaximum )
                        {
                            ExportParameter( aStrBuffer, aRadiusRangeMaximum );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_RADIUS_RANGE_MAXIMUM, aStr );
                        }
                    }
                    break;
                    case EAS_RangeXMinimum :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameter aXRangeMinimum;
                        if ( rPropVal.Value >>= aXRangeMinimum )
                        {
                            ExportParameter( aStrBuffer, aXRangeMinimum );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_RANGE_X_MINIMUM, aStr );
                        }
                    }
                    break;
                    case EAS_RangeXMaximum :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameter aXRangeMaximum;
                        if ( rPropVal.Value >>= aXRangeMaximum )
                        {
                            ExportParameter( aStrBuffer, aXRangeMaximum );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_RANGE_X_MAXIMUM, aStr );
                        }
                    }
                    break;
                    case EAS_RangeYMinimum :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameter aYRangeMinimum;
                        if ( rPropVal.Value >>= aYRangeMinimum )
                        {
                            ExportParameter( aStrBuffer, aYRangeMinimum );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_RANGE_Y_MINIMUM, aStr );
                        }
                    }
                    break;
                    case EAS_RangeYMaximum :
                    {
                        com::sun::star::drawing::EnhancedCustomShapeParameter aYRangeMaximum;
                        if ( rPropVal.Value >>= aYRangeMaximum )
                        {
                            ExportParameter( aStrBuffer, aYRangeMaximum );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_HANDLE_RANGE_Y_MAXIMUM, aStr );
                        }
                    }
                    break;
                    default:
                        break;
                }
            }
            SvXMLElementExport aOBJ( rExport, XML_NAMESPACE_DRAW, XML_HANDLE, sal_True, sal_True );
        }
    }
    else
    {
        SvXMLElementExport aOBJ( rExport, XML_NAMESPACE_DRAW, XML_HANDLE, sal_True, sal_True );
    }
}

void ImpExportEnhancedPath( SvXMLExport& rExport,
    const uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeParameterPair >& rCoordinates,
        const uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeSegment >& rSegments )
{

    rtl::OUString       aStr;
    rtl::OUStringBuffer aStrBuffer;

    sal_Int32 i, j, k, l;

    sal_Int32 nCoords = rCoordinates.getLength();
    sal_Int32 nSegments = rSegments.getLength();
    sal_Bool bSimpleSegments = nSegments == 0;
    if ( bSimpleSegments )
        nSegments = 4;
    for ( j = i = 0; j < nSegments; j++ )
    {
        com::sun::star::drawing::EnhancedCustomShapeSegment aSegment;
        if ( bSimpleSegments )
        {
            // if there are not enough segments we will default them
            switch( j )
            {
                case 0 :
                {
                    aSegment.Count = 1;
                    aSegment.Command = com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::MOVETO;
                }
                break;
                case 1 :
                {
                    aSegment.Count = (sal_Int16)Min( nCoords - 1, (sal_Int32)32767 );
                    aSegment.Command = com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::LINETO;
                }
                break;
                case 2 :
                {
                    aSegment.Count = 1;
                    aSegment.Command = com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::CLOSESUBPATH;
                }
                break;
                case 3 :
                {
                    aSegment.Count = 1;
                    aSegment.Command = com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ENDSUBPATH;
                }
                break;
            }
        }
        else
            aSegment = rSegments[ j ];

        if ( aStrBuffer.getLength() )
            aStrBuffer.append( (sal_Unicode)' ' );

        sal_Int32 nParameter = 0;
        switch( aSegment.Command )
        {
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::CLOSESUBPATH :
                aStrBuffer.append( (sal_Unicode)'Z' ); break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ENDSUBPATH :
                aStrBuffer.append( (sal_Unicode)'N' ); break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::NOFILL :
                aStrBuffer.append( (sal_Unicode)'F' ); break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::NOSTROKE :
                aStrBuffer.append( (sal_Unicode)'S' ); break;

            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::MOVETO :
                aStrBuffer.append( (sal_Unicode)'M' ); nParameter = 1; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::LINETO :
                aStrBuffer.append( (sal_Unicode)'L' ); nParameter = 1; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::CURVETO :
                aStrBuffer.append( (sal_Unicode)'C' ); nParameter = 3; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ANGLEELLIPSETO :
                aStrBuffer.append( (sal_Unicode)'T' ); nParameter = 3; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ANGLEELLIPSE :
                aStrBuffer.append( (sal_Unicode)'U' ); nParameter = 3; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ARCTO :
                aStrBuffer.append( (sal_Unicode)'A' ); nParameter = 4; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ARC :
                aStrBuffer.append( (sal_Unicode)'B' ); nParameter = 4; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::CLOCKWISEARCTO :
                aStrBuffer.append( (sal_Unicode)'W' ); nParameter = 4; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::CLOCKWISEARC :
                aStrBuffer.append( (sal_Unicode)'V' ); nParameter = 4; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ELLIPTICALQUADRANTX :
                aStrBuffer.append( (sal_Unicode)'X' ); nParameter = 1; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::ELLIPTICALQUADRANTY :
                aStrBuffer.append( (sal_Unicode)'Y' ); nParameter = 1; break;
            case com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::QUADRATICCURVETO :
                aStrBuffer.append( (sal_Unicode)'Q' ); nParameter = 2; break;

            default : // ups, seems to be something wrong
            {
                aSegment.Count = 1;
                aSegment.Command = com::sun::star::drawing::EnhancedCustomShapeSegmentCommand::LINETO;
            }
            break;
        }
        if ( nParameter )
        {
            for ( k = 0; k < aSegment.Count; k++ )
            {
                if ( ( i + nParameter ) <= nCoords )
                {
                    for ( l = 0; l < nParameter; l++ )
                    {
                        ExportParameter( aStrBuffer, rCoordinates[ i ].First );
                        ExportParameter( aStrBuffer, rCoordinates[ i++ ].Second );
                    }
                }
                else
                {
                    j = nSegments;  // error -> exiting
                    break;
                }
            }
        }
    }
    aStr = aStrBuffer.makeStringAndClear();
    rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_ENHANCED_PATH, aStr );
}

void ImpExportEnhancedGeometry( SvXMLExport& rExport, const uno::Reference< beans::XPropertySet >& xPropSet )
{
    sal_Bool bEquations = sal_False;
    uno::Sequence< rtl::OUString > aEquations;

    sal_Bool bHandles = sal_False;
    uno::Sequence< beans::PropertyValues > aHandles;

    sal_Bool bCoordinates = sal_False;
    uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeSegment > aSegments;
    uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeParameterPair > aCoordinates;

    uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeAdjustmentValue > aAdjustmentValues;

    rtl::OUString       aStr;
    rtl::OUStringBuffer aStrBuffer;
    SvXMLUnitConverter& rUnitConverter = rExport.GetMM100UnitConverter();

    uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

    // geometry
    const rtl::OUString sCustomShapeGeometry( RTL_CONSTASCII_USTRINGPARAM( "CustomShapeGeometry" ) );
    if ( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName( sCustomShapeGeometry ) )
    {
        uno::Any aGeoPropSet( xPropSet->getPropertyValue( sCustomShapeGeometry ) );
        uno::Sequence< beans::PropertyValue > aGeoPropSeq;

        if ( aGeoPropSet >>= aGeoPropSeq )
        {
            const rtl::OUString sCustomShapeType( RTL_CONSTASCII_USTRINGPARAM( "NonPrimitive" ) );
            rtl::OUString aCustomShapeType( sCustomShapeType );

            sal_Int32 j, nGeoPropCount = aGeoPropSeq.getLength();
            for ( j = 0; j < nGeoPropCount; j++ )
            {
                const beans::PropertyValue& rGeoProp = aGeoPropSeq[ j ];
                switch( EASGet( rGeoProp.Name ) )
                {
                    case EAS_Type :
                    {
                        rGeoProp.Value >>= aCustomShapeType;
                    }
                    break;
                    case EAS_MirroredX :
                    {
                        sal_Bool bMirroredX = sal_Bool();
                        if ( rGeoProp.Value >>= bMirroredX )
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_MIRROR_HORIZONTAL,
                                bMirroredX ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                    }
                    break;
                    case EAS_MirroredY :
                    {
                        sal_Bool bMirroredY = sal_Bool();
                        if ( rGeoProp.Value >>= bMirroredY )
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_MIRROR_VERTICAL,
                                bMirroredY ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                    }
                    break;
                    case EAS_ViewBox :
                    {
                        awt::Rectangle aRect;
                        if ( rGeoProp.Value >>= aRect )
                        {
                            SdXMLImExViewBox aViewBox( aRect.X, aRect.Y, aRect.Width, aRect.Height );
                            rExport.AddAttribute( XML_NAMESPACE_SVG, XML_VIEWBOX, aViewBox.GetExportString() );
                        }
                    }
                    break;
                    case EAS_TextRotateAngle :
                    {
                        double fTextRotateAngle;
                        if ( rGeoProp.Value >>= fTextRotateAngle )
                        {
                            rUnitConverter.convertDouble( aStrBuffer, fTextRotateAngle );
                            aStr = aStrBuffer.makeStringAndClear();
                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TEXT_ROTATE_ANGLE, aStr );
                        }
                    }
                    break;
                    case EAS_Extrusion :
                    {
                        uno::Sequence< beans::PropertyValue > aExtrusionPropSeq;
                        if ( rGeoProp.Value >>= aExtrusionPropSeq )
                        {
                            sal_Int32 i, nCount = aExtrusionPropSeq.getLength();
                            for ( i = 0; i < nCount; i++ )
                            {
                                const beans::PropertyValue& rProp = aExtrusionPropSeq[ i ];
                                switch( EASGet( rProp.Name ) )
                                {
                                    case EAS_Extrusion :
                                    {
                                        sal_Bool bExtrusionOn = sal_Bool();
                                        if ( rProp.Value >>= bExtrusionOn )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION,
                                                bExtrusionOn ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_Brightness :
                                    {
                                        double fExtrusionBrightness;
                                        if ( rProp.Value >>= fExtrusionBrightness )
                                        {
                                            rUnitConverter.convertDouble( aStrBuffer, fExtrusionBrightness, sal_False, MAP_RELATIVE, MAP_RELATIVE );
                                            aStrBuffer.append( (sal_Unicode)'%' );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_BRIGHTNESS, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Depth :
                                    {
                                        com::sun::star::drawing::EnhancedCustomShapeParameterPair aDepthParaPair;
                                        if ( rProp.Value >>= aDepthParaPair )
                                        {
                                            double fDepth;
                                            if ( aDepthParaPair.First.Value >>= fDepth )
                                            {
                                                rExport.GetMM100UnitConverter().convertDouble( aStrBuffer, fDepth, sal_True );
                                                ExportParameter( aStrBuffer, aDepthParaPair.Second );
                                                aStr = aStrBuffer.makeStringAndClear();
                                                rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_DEPTH, aStr );
                                            }
                                        }
                                    }
                                    break;
                                    case EAS_Diffusion :
                                    {
                                        double fExtrusionDiffusion;
                                        if ( rProp.Value >>= fExtrusionDiffusion )
                                        {
                                            rUnitConverter.convertDouble( aStrBuffer, fExtrusionDiffusion, sal_False, MAP_RELATIVE, MAP_RELATIVE );
                                            aStrBuffer.append( (sal_Unicode)'%' );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_DIFFUSION, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_NumberOfLineSegments :
                                    {
                                        sal_Int32 nExtrusionNumberOfLineSegments;
                                        if ( rProp.Value >>= nExtrusionNumberOfLineSegments )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_NUMBER_OF_LINE_SEGMENTS, rtl::OUString::valueOf( nExtrusionNumberOfLineSegments ) );
                                    }
                                    break;
                                    case EAS_LightFace :
                                    {
                                        sal_Bool bExtrusionLightFace = sal_Bool();
                                        if ( rProp.Value >>= bExtrusionLightFace )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_LIGHT_FACE,
                                                bExtrusionLightFace ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_FirstLightHarsh :
                                    {
                                        sal_Bool bExtrusionFirstLightHarsh = sal_Bool();
                                        if ( rProp.Value >>= bExtrusionFirstLightHarsh )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_FIRST_LIGHT_HARSH,
                                                bExtrusionFirstLightHarsh ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_SecondLightHarsh :
                                    {
                                        sal_Bool bExtrusionSecondLightHarsh = sal_Bool();
                                        if ( rProp.Value >>= bExtrusionSecondLightHarsh )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_SECOND_LIGHT_HARSH,
                                                bExtrusionSecondLightHarsh ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_FirstLightLevel :
                                    {
                                        double fExtrusionFirstLightLevel;
                                        if ( rProp.Value >>= fExtrusionFirstLightLevel )
                                        {
                                            rUnitConverter.convertDouble( aStrBuffer, fExtrusionFirstLightLevel, sal_False, MAP_RELATIVE, MAP_RELATIVE );
                                            aStrBuffer.append( (sal_Unicode)'%' );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_FIRST_LIGHT_LEVEL, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_SecondLightLevel :
                                    {
                                        double fExtrusionSecondLightLevel;
                                        if ( rProp.Value >>= fExtrusionSecondLightLevel )
                                        {
                                            rUnitConverter.convertDouble( aStrBuffer, fExtrusionSecondLightLevel, sal_False, MAP_RELATIVE, MAP_RELATIVE );
                                            aStrBuffer.append( (sal_Unicode)'%' );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_SECOND_LIGHT_LEVEL, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_FirstLightDirection :
                                    {
                                        drawing::Direction3D aExtrusionFirstLightDirection;
                                        if ( rProp.Value >>= aExtrusionFirstLightDirection )
                                        {
                                            Vector3D aVec3D( aExtrusionFirstLightDirection.DirectionX, aExtrusionFirstLightDirection.DirectionY,
                                                aExtrusionFirstLightDirection.DirectionZ );
                                            rUnitConverter.convertVector3D( aStrBuffer, aVec3D );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_FIRST_LIGHT_DIRECTION, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_SecondLightDirection :
                                    {
                                        drawing::Direction3D aExtrusionSecondLightDirection;
                                        if ( rProp.Value >>= aExtrusionSecondLightDirection )
                                        {
                                            Vector3D aVec3D( aExtrusionSecondLightDirection.DirectionX, aExtrusionSecondLightDirection.DirectionY,
                                                aExtrusionSecondLightDirection.DirectionZ );
                                            rUnitConverter.convertVector3D( aStrBuffer, aVec3D );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_SECOND_LIGHT_DIRECTION, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Metal :
                                    {
                                        sal_Bool bExtrusionMetal = sal_Bool();
                                        if ( rProp.Value >>= bExtrusionMetal )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_METAL,
                                                bExtrusionMetal ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_ShadeMode :
                                    {
                                        // shadeMode
                                        drawing::ShadeMode eShadeMode;
                                        if( rProp.Value >>= eShadeMode )
                                        {
                                            if( eShadeMode == drawing::ShadeMode_FLAT )
                                                aStr = GetXMLToken( XML_FLAT );
                                            else if( eShadeMode == drawing::ShadeMode_PHONG )
                                                aStr = GetXMLToken( XML_PHONG );
                                            else if( eShadeMode == drawing::ShadeMode_SMOOTH )
                                                aStr = GetXMLToken( XML_GOURAUD );
                                            else
                                                aStr = GetXMLToken( XML_DRAFT );
                                        }
                                        else
                                        {
                                            // ShadeMode enum not there, write default
                                            aStr = GetXMLToken( XML_FLAT);
                                        }
                                        rExport.AddAttribute( XML_NAMESPACE_DR3D, XML_SHADE_MODE, aStr );
                                    }
                                    break;
                                    case EAS_RotateAngle :
                                    {
                                        com::sun::star::drawing::EnhancedCustomShapeParameterPair aRotateAngleParaPair;
                                        if ( rProp.Value >>= aRotateAngleParaPair )
                                        {
                                            ExportParameter( aStrBuffer, aRotateAngleParaPair.First );
                                            ExportParameter( aStrBuffer, aRotateAngleParaPair.Second );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_ROTATION_ANGLE, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_RotationCenter :
                                    {
                                        drawing::Direction3D aExtrusionRotationCenter;
                                        if ( rProp.Value >>= aExtrusionRotationCenter )
                                        {
                                            Vector3D aVec3D( aExtrusionRotationCenter.DirectionX, aExtrusionRotationCenter.DirectionY,
                                                aExtrusionRotationCenter.DirectionZ );
                                            rUnitConverter.convertVector3D( aStrBuffer, aVec3D );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_ROTATION_CENTER, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Shininess :
                                    {
                                        double fExtrusionShininess;
                                        if ( rProp.Value >>= fExtrusionShininess )
                                        {
                                            rUnitConverter.convertDouble( aStrBuffer, fExtrusionShininess, sal_False, MAP_RELATIVE, MAP_RELATIVE );
                                            aStrBuffer.append( (sal_Unicode)'%' );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_SHININESS, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Skew :
                                    {
                                        com::sun::star::drawing::EnhancedCustomShapeParameterPair aSkewParaPair;
                                        if ( rProp.Value >>= aSkewParaPair )
                                        {
                                            ExportParameter( aStrBuffer, aSkewParaPair.First );
                                            ExportParameter( aStrBuffer, aSkewParaPair.Second );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_SKEW, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Specularity :
                                    {
                                        double fExtrusionSpecularity;
                                        if ( rProp.Value >>= fExtrusionSpecularity )
                                        {
                                            rUnitConverter.convertDouble( aStrBuffer, fExtrusionSpecularity, sal_False, MAP_RELATIVE, MAP_RELATIVE );
                                            aStrBuffer.append( (sal_Unicode)'%' );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_SPECULARITY, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_ProjectionMode :
                                    {
                                        drawing::ProjectionMode eProjectionMode;
                                        if ( rProp.Value >>= eProjectionMode )
                                            rExport.AddAttribute( XML_NAMESPACE_DR3D, XML_PROJECTION,
                                                eProjectionMode == drawing::ProjectionMode_PARALLEL ? GetXMLToken( XML_PARALLEL ) : GetXMLToken( XML_PERSPECTIVE ) );
                                    }
                                    break;
                                    case EAS_ViewPoint :
                                    {
                                        drawing::Position3D aExtrusionViewPoint;
                                        if ( rProp.Value >>= aExtrusionViewPoint )
                                        {
                                            rUnitConverter.convertPosition3D( aStrBuffer, aExtrusionViewPoint );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_VIEWPOINT, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Origin :
                                    {
                                        com::sun::star::drawing::EnhancedCustomShapeParameterPair aOriginParaPair;
                                        if ( rProp.Value >>= aOriginParaPair )
                                        {
                                            ExportParameter( aStrBuffer, aOriginParaPair.First );
                                            ExportParameter( aStrBuffer, aOriginParaPair.Second );
                                            aStr = aStrBuffer.makeStringAndClear();
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_ORIGIN, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Color :
                                    {
                                        sal_Bool bExtrusionColor = sal_Bool();
                                        if ( rProp.Value >>= bExtrusionColor )
                                        {
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_COLOR,
                                                bExtrusionColor ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                        }
                                    }
                                    break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    break;
                    case EAS_TextPath :
                    {
                        uno::Sequence< beans::PropertyValue > aTextPathPropSeq;
                        if ( rGeoProp.Value >>= aTextPathPropSeq )
                        {
                            sal_Int32 i, nCount = aTextPathPropSeq.getLength();
                            for ( i = 0; i < nCount; i++ )
                            {
                                const beans::PropertyValue& rProp = aTextPathPropSeq[ i ];
                                switch( EASGet( rProp.Name ) )
                                {
                                    case EAS_TextPath :
                                    {
                                        sal_Bool bTextPathOn = sal_Bool();
                                        if ( rProp.Value >>= bTextPathOn )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TEXT_PATH,
                                                bTextPathOn ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_TextPathMode :
                                    {
                                        com::sun::star::drawing::EnhancedCustomShapeTextPathMode eTextPathMode;
                                        if ( rProp.Value >>= eTextPathMode )
                                        {
                                            switch ( eTextPathMode )
                                            {
                                                case com::sun::star::drawing::EnhancedCustomShapeTextPathMode_NORMAL: aStr = GetXMLToken( XML_NORMAL ); break;
                                                case com::sun::star::drawing::EnhancedCustomShapeTextPathMode_PATH  : aStr = GetXMLToken( XML_PATH );   break;
                                                case com::sun::star::drawing::EnhancedCustomShapeTextPathMode_SHAPE : aStr = GetXMLToken( XML_SHAPE );  break;
                                                default:
                                                    break;
                                            }
                                            if ( aStr.getLength() )
                                                rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TEXT_PATH_MODE, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_ScaleX :
                                    {
                                        sal_Bool bScaleX = sal_Bool();
                                        if ( rProp.Value >>= bScaleX )
                                        {
                                            aStr = bScaleX ? GetXMLToken( XML_SHAPE ) : GetXMLToken( XML_PATH );
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TEXT_PATH_SCALE, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_SameLetterHeights :
                                    {
                                        sal_Bool bSameLetterHeights = sal_Bool();
                                        if ( rProp.Value >>= bSameLetterHeights )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TEXT_PATH_SAME_LETTER_HEIGHTS,
                                                bSameLetterHeights ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    break;
                    case EAS_Path :
                    {
                        uno::Sequence< beans::PropertyValue > aPathPropSeq;
                        if ( rGeoProp.Value >>= aPathPropSeq )
                        {
                            sal_Int32 i, nCount = aPathPropSeq.getLength();
                            for ( i = 0; i < nCount; i++ )
                            {
                                const beans::PropertyValue& rProp = aPathPropSeq[ i ];
                                switch( EASGet( rProp.Name ) )
                                {
                                    case EAS_ExtrusionAllowed :
                                    {
                                        sal_Bool bExtrusionAllowed = sal_Bool();
                                        if ( rProp.Value >>= bExtrusionAllowed )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_EXTRUSION_ALLOWED,
                                                bExtrusionAllowed ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_ConcentricGradientFillAllowed :
                                    {
                                        sal_Bool bConcentricGradientFillAllowed = sal_Bool();
                                        if ( rProp.Value >>= bConcentricGradientFillAllowed )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_CONCENTRIC_GRADIENT_FILL_ALLOWED,
                                                bConcentricGradientFillAllowed ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_TextPathAllowed  :
                                    {
                                        sal_Bool bTextPathAllowed = sal_Bool();
                                        if ( rProp.Value >>= bTextPathAllowed )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TEXT_PATH_ALLOWED,
                                                bTextPathAllowed ? GetXMLToken( XML_TRUE ) : GetXMLToken( XML_FALSE ) );
                                    }
                                    break;
                                    case EAS_GluePoints :
                                    {
                                        com::sun::star::uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeParameterPair> aGluePoints;
                                        if ( rProp.Value >>= aGluePoints )
                                        {
                                            sal_Int32 k, nElements = aGluePoints.getLength();
                                            if ( nElements )
                                            {
                                                for( k = 0; k < nElements; k++ )
                                                {
                                                    ExportParameter( aStrBuffer, aGluePoints[ k ].First );
                                                    ExportParameter( aStrBuffer, aGluePoints[ k ].Second );
                                                }
                                                aStr = aStrBuffer.makeStringAndClear();
                                            }
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_GLUE_POINTS, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_GluePointType :
                                    {
                                        sal_Int16 nGluePointType = sal_Int16();
                                        if ( rProp.Value >>= nGluePointType )
                                        {
                                            switch ( nGluePointType )
                                            {
                                                case com::sun::star::drawing::EnhancedCustomShapeGluePointType::NONE     : aStr = GetXMLToken( XML_NONE );    break;
                                                case com::sun::star::drawing::EnhancedCustomShapeGluePointType::SEGMENTS : aStr = GetXMLToken( XML_SEGMENTS ); break;
                                                case com::sun::star::drawing::EnhancedCustomShapeGluePointType::RECT     : aStr = GetXMLToken( XML_RECTANGLE ); break;
                                            }
                                            if ( aStr.getLength() )
                                                rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_GLUE_POINT_TYPE, aStr );
                                        }
                                    }
                                    break;
                                    case EAS_Coordinates :
                                    {
                                        bCoordinates = ( rProp.Value >>= aCoordinates );
                                    }
                                    break;
                                    case EAS_Segments :
                                    {
                                        rProp.Value >>= aSegments;
                                    }
                                    break;
                                    case EAS_StretchX :
                                    {
                                        sal_Int32 nStretchPoint;
                                        if ( rProp.Value >>= nStretchPoint )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_PATH_STRETCHPOINT_X, rtl::OUString::valueOf( nStretchPoint ) );
                                    }
                                    break;
                                    case EAS_StretchY :
                                    {
                                        sal_Int32 nStretchPoint;
                                        if ( rProp.Value >>= nStretchPoint )
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_PATH_STRETCHPOINT_Y, rtl::OUString::valueOf( nStretchPoint ) );
                                    }
                                    break;
                                    case EAS_TextFrames :
                                    {
                                        com::sun::star::uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeTextFrame > aPathTextFrames;
                                        if ( rProp.Value >>= aPathTextFrames )
                                        {
                                            if ( (sal_uInt16)aPathTextFrames.getLength() )
                                            {
                                                sal_uInt16 k, nElements = (sal_uInt16)aPathTextFrames.getLength();
                                                for ( k = 0; k < nElements; k++ )
                                                {
                                                    ExportParameter( aStrBuffer, aPathTextFrames[ k ].TopLeft.First );
                                                    ExportParameter( aStrBuffer, aPathTextFrames[ k ].TopLeft.Second );
                                                    ExportParameter( aStrBuffer, aPathTextFrames[ k ].BottomRight.First );
                                                    ExportParameter( aStrBuffer, aPathTextFrames[ k ].BottomRight.Second );
                                                }
                                                aStr = aStrBuffer.makeStringAndClear();
                                            }
                                            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TEXT_AREAS, aStr );
                                        }
                                    }
                                    break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                    break;
                    case EAS_Equations :
                    {
                        bEquations = ( rGeoProp.Value >>= aEquations );
                    }
                    break;
                    case EAS_Handles :
                    {
                        bHandles = ( rGeoProp.Value >>= aHandles );
                    }
                    break;
                    case EAS_AdjustmentValues :
                    {
                        rGeoProp.Value >>= aAdjustmentValues;
                    }
                    break;
                    default:
                        break;
                }
            }   // for
            rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_TYPE, aCustomShapeType );

            // adjustments
            sal_Int32 nAdjustmentValues = aAdjustmentValues.getLength();
            if ( nAdjustmentValues )
            {
                sal_Int32 i, nValue;
                for ( i = 0; i < nAdjustmentValues; i++ )
                {
                    if ( i )
                        aStrBuffer.append( (sal_Unicode)' ' );

                    const com::sun::star::drawing::EnhancedCustomShapeAdjustmentValue& rAdj = aAdjustmentValues[ i ];
                    if ( rAdj.State == beans::PropertyState_DIRECT_VALUE )
                    {
                        if ( rAdj.Value.getValueTypeClass() == uno::TypeClass_DOUBLE )
                        {
                            double fValue;
                            rAdj.Value >>= fValue;
                            rUnitConverter.convertDouble( aStrBuffer, fValue );
                        }
                        else
                        {
                            rAdj.Value >>= nValue;
                            rUnitConverter.convertNumber( aStrBuffer, nValue );
                        }
                    }
                    else
                        rUnitConverter.convertNumber( aStrBuffer, 0 );          // this should not be, but better than setting nothing
                }
                aStr = aStrBuffer.makeStringAndClear();
                rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_MODIFIERS, aStr );
            }
            if ( bCoordinates )
                ImpExportEnhancedPath( rExport, aCoordinates, aSegments );
        }
    }
    SvXMLElementExport aOBJ( rExport, XML_NAMESPACE_DRAW, XML_ENHANCED_GEOMETRY, sal_True, sal_True );
    if ( bEquations )
        ImpExportEquations( rExport, aEquations );
    if ( bHandles )
        ImpExportHandles( rExport, aHandles );
}

void XMLShapeExport::ImpExportCustomShape(
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType, sal_Int32 nFeatures, com::sun::star::awt::Point* pRefPoint )
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if ( xPropSet.is() )
    {
        rtl::OUString aStr;
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

        // Transformation
        ImpExportNewTrans( xPropSet, nFeatures, pRefPoint );

        if ( xPropSetInfo.is() )
        {
            const rtl::OUString sCustomShapeEngine( RTL_CONSTASCII_USTRINGPARAM( "CustomShapeEngine" ) );
            if ( xPropSetInfo->hasPropertyByName( sCustomShapeEngine ) )
            {
                uno::Any aEngine( xPropSet->getPropertyValue( sCustomShapeEngine ) );
                if ( ( aEngine >>= aStr ) && aStr.getLength() )
                    mrExport.AddAttribute( XML_NAMESPACE_DRAW, XML_ENGINE, aStr );
            }
            const rtl::OUString sCustomShapeData( RTL_CONSTASCII_USTRINGPARAM( "CustomShapeData" ) );
            if ( xPropSetInfo->hasPropertyByName( sCustomShapeData ) )
            {
                uno::Any aData( xPropSet->getPropertyValue( sCustomShapeData ) );
                if ( ( aData >>= aStr ) && aStr.getLength() )
                    mrExport.AddAttribute( XML_NAMESPACE_DRAW, XML_DATA, aStr );
            }
        }
        sal_Bool bCreateNewline( (nFeatures & SEF_EXPORT_NO_WS) == 0 ); // #86116#/#92210#
        SvXMLElementExport aOBJ( mrExport, XML_NAMESPACE_DRAW, XML_CUSTOM_SHAPE, bCreateNewline, sal_True );
        ImpExportEvents( xShape );
        ImpExportGluePoints( xShape );
        ImpExportText( xShape );
        ImpExportEnhancedGeometry( mrExport, xPropSet );
    }
}

