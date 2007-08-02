/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: textprimitive2d.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: aw $ $Date: 2007-08-02 11:43:43 $
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

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTPRIMITIVE2D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTPRIMITIVE2D_HXX

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_BASEPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _SV_FONT_HXX
#include <vcl/font.hxx>
#endif

#ifndef _BGFX_COLOR_BCOLOR_HXX
#include <basegfx/color/bcolor.hxx>
#endif

#include <vector>

#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        struct FontAttributes
        {
            String                                      maFamilyName;
            String                                      maStyleName;
            sal_uInt16                                  mnWeight;

            // bitfield
            unsigned                                    mbSymbol : 1;
            unsigned                                    mbVertical : 1;
            unsigned                                    mbItalic : 1;
            unsigned                                    mbOutline : 1;
            // TODO: pair kerning and CJK kerning

            // compare operator
            bool operator==(const FontAttributes& rCompare) const
            {
                return (maFamilyName == rCompare.maFamilyName
                    && maStyleName == rCompare.maStyleName
                    && mnWeight == rCompare.mnWeight
                    && mbSymbol == rCompare.mbSymbol
                    && mbVertical == rCompare.mbVertical
                    && mbItalic == rCompare.mbItalic
                    && mbOutline == rCompare.mbOutline);
            }
        };

        // helper methods for vcl font
        Font getVclFontFromFontAttributes(const FontAttributes& rFontAttributes, const basegfx::B2DVector& rFontSize, double fFontRotation);
        Font getVclFontFromFontAttributes(const FontAttributes& rFontAttributes, const basegfx::B2DHomMatrix& rTransform);
        FontAttributes getFontAttributesFromVclFont(basegfx::B2DVector& rSize, const Font& rFont);

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        class TextSimplePortionPrimitive2D : public BasePrimitive2D
        {
        private:
            basegfx::B2DHomMatrix                   maTextTransform;    // text range transformation from unit range ([0.0 .. 1.0]) to text range
            String                                  maText;             // the text
            ::std::vector< double >                 maDXArray;          // the DX array scale-independent in unit coordinates
            FontAttributes                          maFontAttributes;   // the font to use
            ::com::sun::star::lang::Locale          maLocale;           // the Locale for the text
            basegfx::BColor                         maFontColor;        // font color

        protected:
            // local decomposition.
            virtual Primitive2DSequence createLocalDecomposition(const geometry::ViewInformation2D& rViewInformation) const;

        public:
            TextSimplePortionPrimitive2D(
                const basegfx::B2DHomMatrix& rNewTransform,
                const String& rText,
                const ::std::vector< double >& rDXArray,
                const FontAttributes& rFontAttributes,
                const ::com::sun::star::lang::Locale& rLocale,
                const basegfx::BColor& rFontColor);

            // get data
            const basegfx::B2DHomMatrix& getTextTransform() const { return maTextTransform; }
            const String& getText() const { return maText; }
            const ::std::vector< double >& getDXArray() const { return maDXArray; }
            const FontAttributes& getFontAttributes() const { return maFontAttributes; }
            const ::com::sun::star::lang::Locale& getLocale() const { return  maLocale; }
            const basegfx::BColor& getFontColor() const { return maFontColor; }

            // helper to have a central conversion to font-size-scaled integer DXArray
            void getIntegerDXArray(::std::vector< sal_Int32 >& rDXArray) const;

            // compare operator
            virtual bool operator==( const BasePrimitive2D& rPrimitive ) const;

            // get range
            virtual basegfx::B2DRange getB2DRange(const geometry::ViewInformation2D& rViewInformation) const;

            // provide unique ID
            DeclPrimitrive2DIDBlock()
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTPRIMITIVE2D_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
