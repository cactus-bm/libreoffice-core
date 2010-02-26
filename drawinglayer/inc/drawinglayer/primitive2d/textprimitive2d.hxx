/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTPRIMITIVE2D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTPRIMITIVE2D_HXX

#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <tools/string.hxx>
#include <vcl/font.hxx>
#include <basegfx/color/bcolor.hxx>
#include <vector>
#include <com/sun/star/lang/Locale.hpp>
#include <drawinglayer/attribute/fontattribute.hxx>

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace basegfx {
    class B2DPolyPolygon;
    typedef ::std::vector< B2DPolyPolygon > B2DPolyPolygonVector;
}

class OutputDevice;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        /** TextSimplePortionPrimitive2D class

            This is the basic primitive for representing a text portion. It contains
            all needed information. If it is not handled by a renderer, it's decomposition
            will provide the text PolyPolygon outlines as filled polygons, correctly
            transformed.

            To get better text quality, it is suggested to handle tis primitive directly
            in a renderer. In that case, e.g. hintings on the system can be supported.
         */
        class TextSimplePortionPrimitive2D : public BufferedDecompositionPrimitive2D
        {
        private:
            /// text range transformation from unit range ([0.0 .. 1.0]) to text range
            basegfx::B2DHomMatrix                   maTextTransform;

            /// The text, used from maTextPosition up to maTextPosition + maTextLength
            String                                  maText;

            /// The index from where on maText is used
            xub_StrLen                              maTextPosition;

            /// The length for maText usage, starting from maTextPosition
            xub_StrLen                              maTextLength;

            /// The DX array scale-independent in unit coordinates
            ::std::vector< double >                 maDXArray;

            /// The font to use
            attribute::FontAttribute                maFontAttribute;

            /// The Locale for the text
            ::com::sun::star::lang::Locale          maLocale;

            /// font color
            basegfx::BColor                         maFontColor;

            /// #i96669# add simple range buffering for this primitive
            basegfx::B2DRange                       maB2DRange;

        protected:
            /// local decomposition.
            virtual Primitive2DSequence create2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const;

        public:
            /// constructor
            TextSimplePortionPrimitive2D(
                const basegfx::B2DHomMatrix& rNewTransform,
                const String& rText,
                xub_StrLen aTextPosition,
                xub_StrLen aTextLength,
                const ::std::vector< double >& rDXArray,
                const attribute::FontAttribute& rFontAttribute,
                const ::com::sun::star::lang::Locale& rLocale,
                const basegfx::BColor& rFontColor);

            /// helpers
            /** get text outlines as polygons and their according ObjectTransformation. Handles all
                the necessary VCL outline extractins, scaling adaptions and other stuff.
             */
            void getTextOutlinesAndTransformation(basegfx::B2DPolyPolygonVector& rTarget, basegfx::B2DHomMatrix& rTransformation) const;

            /// data read access
            const basegfx::B2DHomMatrix& getTextTransform() const { return maTextTransform; }
            const String& getText() const { return maText; }
            xub_StrLen getTextPosition() const { return maTextPosition; }
            xub_StrLen getTextLength() const { return maTextLength; }
            const ::std::vector< double >& getDXArray() const { return maDXArray; }
            const attribute::FontAttribute& getFontAttribute() const { return maFontAttribute; }
            const ::com::sun::star::lang::Locale& getLocale() const { return  maLocale; }
            const basegfx::BColor& getFontColor() const { return maFontColor; }

            /// compare operator
            virtual bool operator==( const BasePrimitive2D& rPrimitive ) const;

            /// get range
            virtual basegfx::B2DRange getB2DRange(const geometry::ViewInformation2D& rViewInformation) const;

            /// provide unique ID
            DeclPrimitrive2DIDBlock()
        };

        /// small helper to have a compare operator for Locale
        bool LocalesAreEqual(const ::com::sun::star::lang::Locale& rA, const ::com::sun::star::lang::Locale& rB);

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTPRIMITIVE2D_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
