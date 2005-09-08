/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ShapeFactory.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 01:44:33 $
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
#ifndef _CHART2_VIEW_SHAPEFACTORY_HXX
#define _CHART2_VIEW_SHAPEFACTORY_HXX

#include "DatapointGeometry.hxx"
#include "PropertyMapper.hxx"
#include "VLineProperties.hxx"


#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POINTSEQUENCESEQUENCE_HPP_
#include <com/sun/star/drawing/PointSequenceSequence.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POLYPOLYGONSHAPE3D_HPP_
#include <com/sun/star/drawing/PolyPolygonShape3D.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPES_HPP_
#include <com/sun/star/drawing/XShapes.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

//.............................................................................
namespace chart
{
//.............................................................................

class Stripe;
class ShapeFactory
{
public:
    ShapeFactory(::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory> xFactory)
        {m_xShapeFactory = xFactory;}

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >
        createGroup2D(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShapes >& xTarget
                , ::rtl::OUString aName = ::rtl::OUString() );


    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >
        createGroup3D(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::drawing::XShapes >& xTarget
                , ::rtl::OUString aName = ::rtl::OUString() );

    //------

    enum tErrorBarDirection
    {
        ERROR_BAR_UP,
        ERROR_BAR_RIGHT,
        ERROR_BAR_DOWN,
        ERROR_BAR_LEFT
    };

    //------

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
            createCube(   const ::com::sun::star::uno::Reference<
                                ::com::sun::star::drawing::XShapes >& xTarget
                        , const DataPointGeometry& rGeometry
                        , const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xSourceProp
                        , const tPropertyNameMap& rPropertyNameMap );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createRoundedCube(const ::com::sun::star::uno::Reference<
                                ::com::sun::star::drawing::XShapes >& xTarget
                        , const DataPointGeometry& rGeometry );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createCylinder(   const ::com::sun::star::uno::Reference<
                                ::com::sun::star::drawing::XShapes >& xTarget
                        , const DataPointGeometry& rGeometry );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createPyramid(    const ::com::sun::star::uno::Reference<
                                ::com::sun::star::drawing::XShapes >& xTarget
                        , const DataPointGeometry& rGeometry );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createCone(       const ::com::sun::star::uno::Reference<
                                ::com::sun::star::drawing::XShapes >& xTarget
                        , const DataPointGeometry& rGeometry );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createPieSegment2D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , double fStartAngleDegree, double fWidthAngleDegree
                    , double fInnerXRadius, double fOuterXRadius, double fOuterYRadius
                    , const ::com::sun::star::drawing::Position3D& rOrigin );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createPieSegment( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , double fStartAngleDegree, double fWidthAngleDegree
                    , double fInnerXRadius, double fOuterXRadius, double fOuterYRadius
                    , const ::com::sun::star::drawing::Position3D& rOrigin
                    , double fDepth );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createStripe( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const Stripe& rStripe
                    , const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xSourceProp
                    , const tPropertyNameMap& rPropertyNameMap
                    , sal_Bool bDoubleSided = true);

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createArea3D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const ::com::sun::star::drawing::PolyPolygonShape3D& rPolyPolygon
                    , double fDepth);

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createArea2D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const ::com::sun::star::drawing::PolyPolygonShape3D& rPolyPolygon);

    static sal_Int32 getSymbolCount();

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createSymbol2D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const ::com::sun::star::drawing::Position3D& rPos
                    , const ::com::sun::star::drawing::Direction3D& rSize
                    , sal_Int32 nStandardSymbol
                    , sal_Int32 nFillColor=0 );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createSymbol3D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const ::com::sun::star::drawing::Position3D& rPos
                    , const ::com::sun::star::drawing::Direction3D& rSize
                    , sal_Int32 nStandardSymbol
                    , sal_Int32 nFillColor=0 );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createLine2D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const ::com::sun::star::drawing::PointSequenceSequence& rPoints
                    , const VLineProperties& rLineProperties );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createLine3D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const ::com::sun::star::drawing::PolyPolygonShape3D& rPoints
                    , const VLineProperties& rLineProperties );

    /** creates an error bar shape, that is a line with a small perpendicular
        line on top like the letter 'T'.

        @param bClip
            If true, the perpendicular line is omitted.

        @todo If bClip is true, the main line could end in dots.
     */
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createErrorBar2D( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const ::com::sun::star::drawing::Position3D& rPos
                    , const ::com::sun::star::drawing::Direction3D& rSize
                    , tErrorBarDirection eDirection
                    , bool bClip
            );
//                     , const tNameSequence& rPropNames
//                     , const tAnySequence& rPropValues );

    //------------------- create 2D elements:

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createText( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget2D
                    , const ::rtl::OUString& rText
                    , const tNameSequence& rPropNames
                    , const tAnySequence& rPropValues
                    , const ::com::sun::star::uno::Any& rATransformation
                     );

    //------

     static void setShapeName( const ::com::sun::star::uno::Reference<
                                     ::com::sun::star::drawing::XShape >& xShape
                             , const rtl::OUString& rName );

     static ::com::sun::star::uno::Any makeTransformation( const ::com::sun::star::awt::Point& rScreenPosition2D, double fRotationAnglePi=0.0 );

     static rtl::OUString getStackedString( const rtl::OUString& rString, bool bStacked=true );

private:
    ShapeFactory();

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        impl_createCube( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const DataPointGeometry& rGeometry
                    , sal_Bool bRounded );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        impl_createConeOrCylinder( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& xTarget
                    , const DataPointGeometry& rGeometry
                    , sal_Int32 nSegments
                    , bool bCylinder = false);

    //member:
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>
        m_xShapeFactory;
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif
