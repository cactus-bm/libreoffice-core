/*************************************************************************
 *
 *  $RCSfile: canvastools.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 18:36:22 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#define _BGFX_TOOLS_CANVASTOOLS_HXX

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif


namespace drafts { namespace com { namespace sun { namespace star { namespace geometry
{
    struct AffineMatrix2D;
    struct RealPoint2D;
    struct RealSize2D;
    struct RealRectangle2D;
    struct IntegerPoint2D;
    struct IntegerSize2D;
    struct IntegerRectangle2D;
    struct RealBezierSegment2D;
} } } } }

namespace drafts { namespace com { namespace sun { namespace star { namespace rendering
{
    class  XGraphicDevice;
    class  XPolyPolygon2D;
} } } } }

namespace com { namespace sun { namespace star { namespace awt
{
    struct Point;
    struct Size;
    struct Rectangle;
} } } }

namespace basegfx
{
    class B2DHomMatrix;
    class B2DVector;
    class B2DPoint;
    class B2DRange;
    class B2IVector;
    class B2IPoint;
    class B2IRange;
    class B2DPolygon;
    class B2DPolyPolygon;

    namespace unotools
    {
        // Polygon conversions
        // ===================================================================

        ::com::sun::star::uno::Reference< ::drafts::com::sun::star::rendering::XPolyPolygon2D >
            xPolyPolygonFromB2DPolygon( const ::com::sun::star::uno::Reference<
                                            ::drafts::com::sun::star::rendering::XGraphicDevice >&  xGraphicDevice,
                                        const ::basegfx::B2DPolygon&                                rPoly    );

        ::com::sun::star::uno::Reference< ::drafts::com::sun::star::rendering::XPolyPolygon2D >
            xPolyPolygonFromB2DPolyPolygon( const ::com::sun::star::uno::Reference<
                                                 ::drafts::com::sun::star::rendering::XGraphicDevice >&     xGraphicDevice,
                                            const ::basegfx::B2DPolyPolygon&                            rPolyPoly    );


        ::basegfx::B2DPolygon polygonFromPoint2DSequence(
            const ::com::sun::star::uno::Sequence<
                ::drafts::com::sun::star::geometry::RealPoint2D >& rPoints );

        ::basegfx::B2DPolyPolygon polyPolygonFromPoint2DSequenceSequence(
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Sequence< ::drafts::com::sun::star::geometry::RealPoint2D > >& rPoints );

        ::basegfx::B2DPolygon polygonFromBezier2DSequence(
            const ::com::sun::star::uno::Sequence<
                ::drafts::com::sun::star::geometry::RealBezierSegment2D >& rPoints );

        ::basegfx::B2DPolyPolygon polyPolygonFromBezier2DSequenceSequence(
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Sequence< ::drafts::com::sun::star::geometry::RealBezierSegment2D > >& rPoints );


        // Matrix conversions
        // ===================================================================

        ::drafts::com::sun::star::geometry::AffineMatrix2D&
            affineMatrixFromHomMatrix( ::drafts::com::sun::star::geometry::AffineMatrix2D&          matrix,
                                       const ::basegfx::B2DHomMatrix&                               transform);

        ::basegfx::B2DHomMatrix&
            homMatrixFromAffineMatrix( ::basegfx::B2DHomMatrix&                                     transform,
                                       const ::drafts::com::sun::star::geometry::AffineMatrix2D&    matrix );


        // Geometry conversions
        // ===================================================================

        ::drafts::com::sun::star::geometry::RealSize2D          size2DFromB2DSize( const ::basegfx::B2DVector& );
        ::drafts::com::sun::star::geometry::RealPoint2D         point2DFromB2DPoint( const ::basegfx::B2DPoint& );
        ::drafts::com::sun::star::geometry::RealRectangle2D     rectangle2DFromB2DRectangle( const ::basegfx::B2DRange& );

        ::basegfx::B2DVector    b2DSizeFromRealSize2D( const ::drafts::com::sun::star::geometry::RealSize2D& );
        ::basegfx::B2DPoint     b2DPointFromRealPoint2D( const ::drafts::com::sun::star::geometry::RealPoint2D& );
        ::basegfx::B2DRange     b2DRectangleFromRealRectangle2D( const ::drafts::com::sun::star::geometry::RealRectangle2D& );

        ::drafts::com::sun::star::geometry::IntegerSize2D       integerSize2DFromB2ISize( const ::basegfx::B2IVector& );
        ::drafts::com::sun::star::geometry::IntegerPoint2D      integerPoint2DFromB2IPoint( const ::basegfx::B2IPoint& );
        ::drafts::com::sun::star::geometry::IntegerRectangle2D  integerRectangle2DFromB2IRectangle( const ::basegfx::B2IRange& );

        ::basegfx::B2IVector    b2ISizeFromIntegerSize2D( const ::drafts::com::sun::star::geometry::IntegerSize2D& );
        ::basegfx::B2IPoint     b2IPointFromIntegerPoint2D( const ::drafts::com::sun::star::geometry::IntegerPoint2D& );
        ::basegfx::B2IRange     b2IRectangleFromIntegerRectangle2D( const ::drafts::com::sun::star::geometry::IntegerRectangle2D& );

        ::com::sun::star::awt::Size         awtSizeFromB2ISize( const ::basegfx::B2IVector& );
        ::com::sun::star::awt::Point        awtPointFromB2IPoint( const ::basegfx::B2IPoint& );
        ::com::sun::star::awt::Rectangle    awtRectangleFromB2IRectangle( const ::basegfx::B2IRange& );

        ::basegfx::B2IVector    b2ISizeFromAwtSize( const ::com::sun::star::awt::Size& );
        ::basegfx::B2IPoint     b2IPointFromAwtPoint( const ::com::sun::star::awt::Point& );
        ::basegfx::B2IRange     b2IRectangleFromAwtRectangle( const ::com::sun::star::awt::Rectangle& );
    }
}

#endif /* _BGFX_TOOLS_CANVASTOOLS_HXX */
