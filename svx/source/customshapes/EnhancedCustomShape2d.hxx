/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: EnhancedCustomShape2d.hxx,v $
 * $Revision: 1.11 $
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

#ifndef _ENHANCEDCUSTOMSHAPE2D_HXX
#define _ENHANCEDCUSTOMSHAPE2D_HXX

#include <svx/msdffdef.hxx>
#include <svx/msdffimp.hxx>
#include <svx/sdasitm.hxx>
#include <com/sun/star/uno/Sequence.h>
#ifndef _com_sun_star_beans_PropertyValues_hpp__
#include <com/sun/star/beans/PropertyValues.hpp>
#endif
#include <com/sun/star/awt/Point.hpp>
#include <svtools/itemset.hxx>
#ifndef __com_sun_star_drawing_EnhancedCustomShapeParameterPair_hpp__
#include <com/sun/star/drawing/EnhancedCustomShapeParameterPair.hpp>
#endif
#include <com/sun/star/drawing/EnhancedCustomShapeSegment.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeParameter.hpp>
#ifndef __COM_SUN_STAR_DRAWING_ENHANCEDCUSTOMSHAPETEXTFRAME_HPP__
#include <com/sun/star/drawing/EnhancedCustomShapeTextFrame.hpp>
#endif
#include <com/sun/star/drawing/EnhancedCustomShapeAdjustmentValue.hpp>
#include <EnhancedCustomShapeFunctionParser.hxx>

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif
#include <vector>

#define DFF_CUSTOMSHAPE_FLIP_V      1
#define DFF_CUSTOMSHAPE_FLIP_H      2
#define DFF_CUSTOMSHAPE_EXCH            4

class SdrModel;
class SfxItemSet;
struct SvxMSDffVertPair;
struct SvxMSDffCalculationData;
struct SvxMSDffTextRectangles;
class SvxMSDffAdjustmentHandle;

#define HANDLE_FLAGS_MIRRORED_X             0x0001
#define HANDLE_FLAGS_MIRRORED_Y             0x0002
#define HANDLE_FLAGS_SWITCHED               0x0004
#define HANDLE_FLAGS_POLAR                  0x0008
#define HANDLE_FLAGS_MAP                    0x0010
#define HANDLE_FLAGS_RANGE_X_MINIMUM        0x0020
#define HANDLE_FLAGS_RANGE_X_MAXIMUM        0x0040
#define HANDLE_FLAGS_RANGE_Y_MINIMUM        0x0080
#define HANDLE_FLAGS_RANGE_Y_MAXIMUM        0x0100
#define HANDLE_FLAGS_RADIUS_RANGE_MINIMUM   0x0200
#define HANDLE_FLAGS_RADIUS_RANGE_MAXIMUM   0x0400

// MSDFF_HANDLE_FLAGS_RANGE_Y seems to be not defined in
// escher, but we are using it internally in to differentiate
// between X_RANGE and Y_RANGE

class SdrPathObj;
class EnhancedCustomShape2d : public SfxItemSet
{
        SdrObject*                  pCustomShapeObj;
        MSO_SPT                     eSpType;

        sal_Int32                   nCoordLeft;
        sal_Int32                   nCoordTop;
        sal_Int32                   nCoordWidth;
        sal_Int32                   nCoordHeight;
        Rectangle                   aLogicRect;

        double                      fXScale;
        double                      fYScale;
        double                      fXRatio;
        double                      fYRatio;

        sal_Int32                   nXRef;
        sal_Int32                   nYRef;
        sal_uInt32                  nFlags;
        sal_uInt32                  nColorData;

        /*

        */
        com::sun::star::uno::Sequence< rtl::OUString >                                                  seqEquations;
        std::vector< ::boost::shared_ptr< EnhancedCustomShape::ExpressionNode > >                       vNodesSharedPtr;


        com::sun::star::uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeSegment >            seqSegments;
        com::sun::star::uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeParameterPair>       seqCoordinates;
        com::sun::star::uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeTextFrame >          seqTextFrames;
        com::sun::star::uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeParameterPair>       seqGluePoints;
        com::sun::star::uno::Sequence< com::sun::star::drawing::EnhancedCustomShapeAdjustmentValue >    seqAdjustmentValues;
        com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValues >                          seqHandles;

        sal_Bool                    bTextFlow       : 1;
        sal_Bool                    bFilled         : 1;
        sal_Bool                    bStroked        : 1;

        sal_Bool                    bFlipH;
        sal_Bool                    bFlipV;
        sal_Int32                   nRotateAngle;

        sal_Bool                    SetAdjustValueAsDouble( const double& rValue, const sal_Int32 nIndex );
        Color                       GetColorData( const Color& rFillColor, sal_uInt32 nIndex );
        sal_Bool                    GetParameter( double& rParameterReturnValue,  const com::sun::star::drawing::EnhancedCustomShapeParameter&,
                                                    const sal_Bool bReplaceGeoWidth, const sal_Bool bReplaceGeoHeight ) const;
        Point                       GetPoint( const com::sun::star::drawing::EnhancedCustomShapeParameterPair&,
                                                    const sal_Bool bScale = sal_True, const sal_Bool bReplaceGeoSize = sal_False ) const;

        static void                 SwapStartAndEndArrow( SdrObject* pObj );

        void                        CreateSubPath( sal_uInt16& rSrcPt, sal_uInt16& rSegmentInd, std::vector< SdrPathObj* >& rObjectList,
                                                                                                sal_Bool bLineGeometryNeededOnly, sal_Bool bSortFilledObjectsToBack );
        SdrObject*                  CreatePathObj( sal_Bool bLineGeometryNeededOnly );
        const sal_Int32*            ApplyShapeAttributes( const SdrCustomShapeGeometryItem& rItem );

    public :

        struct Handle
        {
            sal_uInt32  nFlags;

            sal_Bool    bMirroredX;
            sal_Bool    bMirroredY;
            sal_Bool    bSwitched;

            com::sun::star::drawing::EnhancedCustomShapeParameterPair   aPosition;
            com::sun::star::drawing::EnhancedCustomShapeParameterPair   aPolar;

            com::sun::star::drawing::EnhancedCustomShapeParameter       aRadiusRangeMinimum;
            com::sun::star::drawing::EnhancedCustomShapeParameter       aRadiusRangeMaximum;
            com::sun::star::drawing::EnhancedCustomShapeParameter       aXRangeMinimum;
            com::sun::star::drawing::EnhancedCustomShapeParameter       aXRangeMaximum;
            com::sun::star::drawing::EnhancedCustomShapeParameter       aYRangeMinimum;
            com::sun::star::drawing::EnhancedCustomShapeParameter       aYRangeMaximum;
        };

        sal_Bool                    IsFlipVert() { return bFlipV; };
        sal_Bool                    IsFlipHorz() { return bFlipH; };
        sal_Int32                   GetRotateAngle() { return nRotateAngle; };

        SdrObject*                  CreateLineGeometry();
        SdrObject*                  CreateObject( sal_Bool bLineGeometryNeededOnly );
        void                        ApplyGluePoints( SdrObject* pObj );
        Rectangle                   GetTextRect() const;

        sal_uInt32                  GetHdlCount() const;
        sal_Bool                    GetHandlePosition( const sal_uInt32 nIndex, Point& rReturnPosition ) const;
        sal_Bool                    SetHandleControllerPosition( const sal_uInt32 nIndex, const com::sun::star::awt::Point& rPosition );

        EnhancedCustomShape2d( SdrObject* pSdrObjCustomShape );
        ~EnhancedCustomShape2d();

        enum EnumFunc
        {
            ENUM_FUNC_PI,
            ENUM_FUNC_LEFT,
            ENUM_FUNC_TOP,
            ENUM_FUNC_RIGHT,
            ENUM_FUNC_BOTTOM,
            ENUM_FUNC_XSTRETCH,
            ENUM_FUNC_YSTRETCH,
            ENUM_FUNC_HASSTROKE,
            ENUM_FUNC_HASFILL,
            ENUM_FUNC_WIDTH,
            ENUM_FUNC_HEIGHT,
            ENUM_FUNC_LOGWIDTH,
            ENUM_FUNC_LOGHEIGHT
        };
        double                      GetEnumFunc( const EnumFunc eVal ) const;

        double                      GetAdjustValueAsDouble( const sal_Int32 nIndex ) const;
        double                      GetEquationValueAsDouble( const sal_Int32 nIndex ) const;
        sal_Int32                   GetAdjustValueAsInteger( const sal_Int32 nIndex, const sal_Int32 nDefault = 0 ) const;

        static rtl::OUString        GetEquation( const sal_uInt16 nFlags, sal_Int16 nPara1, sal_Int16 nPara2, sal_Int16 nPara3 );

        static void                 AppendEnhancedCustomShapeEquationParameter( rtl::OUString& rParameter, const sal_Int16 nPara, const sal_Bool bIsSpecialValue );

        static void                 SetEnhancedCustomShapeEquationParameter( com::sun::star::drawing::EnhancedCustomShapeParameter&
                                        rParameter, const sal_Int16 nPara, const sal_Bool bIsSpecialValue );
        static void                 SetEnhancedCustomShapeParameter( com::sun::star::drawing::EnhancedCustomShapeParameter&
                                        rParameter, const sal_Int32 nValue );
        static void                 SetEnhancedCustomShapeHandleParameter( com::sun::star::drawing::EnhancedCustomShapeParameter&
                                        rParameter, const sal_Int32 nPara, const sal_Bool bIsSpecialValue, sal_Bool bHorz );
        static sal_Bool             ConvertSequenceToEnhancedCustomShape2dHandle( const com::sun::star::beans::PropertyValues& rHandleProperties,
                                        EnhancedCustomShape2d::Handle& rDestinationHandle );
};
#endif

