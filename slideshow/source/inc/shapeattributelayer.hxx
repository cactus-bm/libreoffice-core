/*************************************************************************
 *
 *  $RCSfile: shapeattributelayer.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 19:19:38 $
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

#ifndef _SLIDESHOW_SHAPEATTRIBUTELAYER_HXX
#define _SLIDESHOW_SHAPEATTRIBUTELAYER_HXX

#ifndef _COM_SUN_STAR_UTIL_TRISTATE_HPP_
#include <com/sun/star/util/TriState.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_FILLSTYLE_HPP_
#include <com/sun/star/drawing/FillStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_LINESTYLE_HPP_
#include <com/sun/star/drawing/LineStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTSLANT_HPP_
#include <com/sun/star/awt/FontSlant.hpp>
#endif

#ifndef _COMPHELPER_OPTIONALVALUE_HXX
#include <comphelper/optionalvalue.hxx>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

#ifndef _BGFX_VECTOR_B2DSIZE_HXX
#include <basegfx/vector/b2dsize.hxx>
#endif
#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_RANGE_B2DRECTANGLE_HXX
#include <basegfx/range/b2drectangle.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif

#include <stack>

#include <state.hxx>
#include <rgbcolor.hxx>


namespace presentation
{
    namespace internal
    {
        class ShapeAttributeLayer;

        typedef ::boost::shared_ptr< ShapeAttributeLayer > ShapeAttributeLayerSharedPtr;

        /** Encapsulates all modifiable attributes of a shape.

            This class holds all modifiable attributes of a shape, and
            at the same time provides means to layer attributes on top
            of each other..

            And yes, there's a reason why we even pass bools and ints
            by const reference. Namely, that makes the set* methods
            differ only in the value type, which greatly reduces
            template variability (e.g. in AnimationFactory).
         */
        class ShapeAttributeLayer
        {
        public:
            /** Create a ShapeAttributeLayer instance, with all
                attributes set to default.

                Furthermore, this constructor gets a pointer to a
                child layer, which is used as the fallback (or the
                base value) for all attributes

                @param rChildLayer
                Layer below this one

                @attention
                This method is only supposed to be called from Shape objects
             */
            explicit ShapeAttributeLayer( const ShapeAttributeLayerSharedPtr& rChildLayer );

            // Children management methods
            //------------------------------------------------------------------

            /** Revoke the given layer.

                This method revokes the given layer from this object
                or one of the children. That is, if this object does
                have children, and the given layer is no direct child,
                it is recursively passed to the children for removal.

                @return true, if removal was successful.

                @attention
                This method is only supposed to be called from Shape objects
             */
            bool revokeChildLayer( const ShapeAttributeLayerSharedPtr& rChildLayer );

            /** Query the child layer of this object.

                @attention
                This method is only supposed to be called from Shape objects
             */
            ShapeAttributeLayerSharedPtr getChildLayer() const;

            /** Set the additive mode for possible child attributes

                This method sets the additive mode for child
                attributes. That is the way underlying attribute
                layers are combined with this one (i.e. to overrule
                lower layers, or how to combine the values). The
                default is
                ::com::sun::star::animations::AnimationAdditiveMode::BASE,
                which means, take the value of the underlying layers,
                or from the model shape itself.

                @param nMode
                Must be one of
                ::com::sun::star::animations::AnimationAdditiveMode.
             */
            void setAdditiveMode( sal_Int16 nMode );

            // Attribute methods
            //------------------------------------------------------------------

            /** Query whether the width attribute is valid.
             */
            bool isWidthValid() const;
            /** Query the current width of the shape
             */
            double getWidth() const;
            /** Set the new width of the shape

                @param rNewWidth
                A negative width mirrors the shape.
             */
            void setWidth( const double& rNewWidth );

            /** Query whether the height attribute is valid.
             */
            bool isHeightValid() const;
            /** Query the current height of the shape
             */
            double getHeight() const;
            /** Set the new height of the shape

                @param rNewHeight
                A negative height mirrors the shape.
             */
            void setHeight( const double& rNewHeight );

            /** Set the new size of the shape

                @param rNewSize
                A negative size mirrors the shape.
             */
            void setSize( const ::basegfx::B2DSize& rNewSize );

            /** Query whether the x position attribute is valid
             */
            bool isPosXValid() const;
            /** Query the current x position of the shape.

                The current x position of the shape is always relative
                to the <em>center</em> of the shape (in contrast to
                the Shape::getPosSize() and Shape::getUpdateArea()
                methods).
             */
            double getPosX() const;
            /** Set the new x position of the shape

                The current x position of the shape is always relative
                to the <em>center</em> of the shape (in contrast to
                the Shape::getPosSize() and Shape::getUpdateArea()
                methods).
             */
            void setPosX( const double& rNewX );

            /** Query whether the y position attribute is valid
             */
            bool isPosYValid() const;
            /** Query the current y position of the shape

                The current y position of the shape is always relative
                to the <em>center</em> of the shape (in contrast to
                the Shape::getPosSize() and Shape::getUpdateArea()
                methods).
             */
            double getPosY() const;
            /** Set the new y position of the shape

                The current y position of the shape is always relative
                to the <em>center</em> of the shape (in contrast to
                the Shape::getPosSize() and Shape::getUpdateArea()
                methods).
             */
            void setPosY( const double& rNewY );

            /** Set the new position of the shape

                The current position of the shape is always relative
                to the <em>center</em> of the shape (in contrast to
                the Shape::getPosSize() and Shape::getUpdateArea()
                methods).
             */
            void setPosition( const ::basegfx::B2DPoint& rNewPos );

            /** Query whether the rotation angle attribute is valid
             */
            bool isRotationAngleValid() const;
            /** Query the current rotation angle of the shape

                @return the rotation angle in degrees.
             */
            double getRotationAngle() const;
            /** Set the new rotation angle of the shape

                @param rNewAngle
                New rotation angle in degrees.
             */
            void setRotationAngle( const double& rNewAngle );

            /** Query whether the shear x angle attribute is valid
             */
            bool isShearXAngleValid() const;
            /** Query the current shear angle at the x axis of the shape

                @return the shear angle in degrees.
             */
            double getShearXAngle() const;
            /** Set the new shear angle at the x axis of the shape

                @param rNewAngle
                New shear angle in radians.
             */
            void setShearXAngle( const double& rNewAngle );

            /** Query whether the shear y angle attribute is valid
             */
            bool isShearYAngleValid() const;
            /** Query the current shear angle at the y axis of the shape

                @return the shear angle in degrees.
             */
            double getShearYAngle() const;
            /** Set the new shear angle at the y axis of the shape

                @param rNewAngle
                New shear angle in radians.
             */
            void setShearYAngle( const double& rNewAngle );

            /** Query whether the alpha attribute is valid
             */
            bool isAlphaValid() const;
            /** Query the current alpha value of the shape
             */
            double getAlpha() const;
            /** Set the new alpha value of the shape

                @param rNewValue
                New alpha value, must be in the [0,1] range
             */
            void setAlpha( const double& rNewValue );

            /** Query whether the clip attribute is valid
             */
            bool isClipValid() const;
            /** Query the current clip polygon of the shape
             */
            ::basegfx::B2DPolyPolygon getClip() const;
            /** Set the new clip polygon of the shape

                @param rNewClip
                New clip polygon, must have size of one and will be
                scaled-up to the shape size
             */
            void setClip( const ::basegfx::B2DPolyPolygon& rNewClip );

            /** Query whether the fill color attribute is valid
             */
            bool isFillColorValid() const;
            /** Get the fill color for the whole shape.

                If there's no unique fill color, the color from the
                first filled polygon is returned.
             */
            RGBColor getFillColor() const;
            /** Set the fill color globally for the whole shape.
             */
            void setFillColor( const RGBColor& nNewColor );

            /** Query whether the line color attribute is valid
             */
            bool isLineColorValid() const;
            /** Get the line color for the whole shape.

                If there's no unique line color, the color from the
                first line is returned.
             */
            RGBColor  getLineColor() const;
            /** Set the line color globally for the whole shape.
             */
            void setLineColor( const RGBColor& nNewColor );

            /** Query whether the fill mode attribute is valid
             */
            bool isFillStyleValid() const;
            /** Get the current fill mode for polygon fillings.

                @returns the current style
             */
            sal_Int16 getFillStyle() const;
            /** Changes polygon fillings.
             */
            void setFillStyle( const sal_Int16& rStyle );

            /** Query whether the line mode attribute is valid
             */
            bool isLineStyleValid() const;
            /** Get the current line mode for line drawing.

                @returns the current line style
             */
            sal_Int16 getLineStyle() const;
            /** Set line style for the whole shape
             */
            void setLineStyle( const sal_Int16& rStyle );

            /** Query whether the visibility state attribute is valid
             */
            bool isVisibilityValid() const;
            /** Get the current shape visibility.

                @returns true for visible, false for invisible.
             */
            bool getVisibility() const;
            /** Set the shape visibility
             */
            void setVisibility( const bool& bVisible );

            /** Query whether the char color attribute is valid
             */
            bool isCharColorValid() const;
            /** Get the text color for the whole shape.

                If there's no unique text color, the color from the
                first text drawn is returned.
             */
            RGBColor getCharColor() const;
            /** Set the text color globally for the whole shape.
             */
            void setCharColor( const RGBColor& nNewColor );

            /** Query whether the char rotation angle attribute is valid
             */
            bool isCharRotationAngleValid() const;
            /** Query the current text rotation angle of the shape

                @return the text rotation angle in degrees.
             */
            double getCharRotationAngle() const;
            /** Set the new text rotation angle of the shape

                @param rNewAngle
                New text rotation angle in degrees.
             */
            void setCharRotationAngle( const double& rNewAngle );

            /** Query whether the char weight attribute is valid
             */
            bool isCharWeightValid() const;
            /** Get the current char weight value for the whole shape.

                @returns the value for the char weight. The value must
                be out of the ::com::sun::star::awt::FontWeight
                constant group.
             */
            double getCharWeight() const;
            /** Set the char weight globally for the whole shape.

                The value must be out of the
                ::com::sun::star::awt::FontWeight constant group.
             */
            void setCharWeight( const double& rStyle );

            /** Query whether the underline mode attribute is valid
             */
            bool isUnderlineModeValid() const;
            /** Get the current text underline status for the whole shape.

                If there is no unique underline status, false is returned.

                @returns true for underlined text, false for normal.
             */
            sal_Int16 getUnderlineMode() const;
            /** Set the underline status globally for the whole shape
             */
            void setUnderlineMode( const sal_Int16& bUnderline );

            /** Query whether the font family attribute is valid
             */
            bool isFontFamilyValid() const;
            /** Get the current text font family for the whole shape.

                If there is no unique font family, the font family of
                the first text of the shape is returned.
             */
            ::rtl::OUString getFontFamily() const;
            /** Set the text font family name globally for the whole shape
             */
            void setFontFamily( const ::rtl::OUString& rName );

            /** Query whether the italic mode attribute is valid
             */
            bool isCharPostureValid() const;
            /** Get the current text italic style for the whole shape.

                @returns the italic style. The value returned is one
                of the ::com::sun::star::awt::FontSlant enums
             */
            sal_Int16 getCharPosture() const;
            /** Set the italic style globally for the whole shape.

                The value must be one of the
                ::com::sun::star::awt::FontSlant enums.
            */
            void setCharPosture( const sal_Int16& rStyle );

            /** Query whether the char scaling attribute is valid
             */
            bool isCharScaleValid() const;
            /** Query the current char scaling attribute globally for
                the shape.

                The char scaling changes the scale of the whole shape
                text (uniformely, i.e. both in x and in y direction).
             */
            double getCharScale() const;
            /** Set the new char scale globally for the shape

                @param rNewScale
                New char scale
             */
            void setCharScale( const double& rNewScale );

            // State change query methods
            // ==========================

            State::StateId getTransformationState() const;
            State::StateId getClipState() const;
            State::StateId getAlphaState() const;
            State::StateId getPositionState() const;
            State::StateId getContentState() const;

        private:
            // default copy/assignment operator is okay
            // ShapeAttributeLayer(const ShapeAttributeLayer&);
            // ShapeAttributeLayer& operator=( const ShapeAttributeLayer& );

            bool haveChild() const { return mpChild.get() != NULL; }
            void updateStateIds();

            template< typename T > T calcValue( const T&                   rCurrValue,
                                                bool                           bThisInstanceValid,
                                                bool (ShapeAttributeLayer::*pIsValid)() const,
                                                T   (ShapeAttributeLayer::*pGetValue)() const ) const;

            ShapeAttributeLayerSharedPtr                mpChild; // may be NULL

            ::basegfx::B2DSize                          maSize;
            ::basegfx::B2DPoint                         maPosition;
            ::basegfx::B2DPolyPolygon                   maClip;

            ::rtl::OUString                             maFontFamily;

            double                                      mnRotationAngle;
            double                                      mnShearXAngle;
            double                                      mnShearYAngle;
            double                                      mnAlpha;
            double                                      mnCharRotationAngle;
            double                                      mnCharScale;
            double                                      mnCharWeight;

            ::com::sun::star::drawing::FillStyle        meFillStyle;
            ::com::sun::star::drawing::LineStyle        meLineStyle;
            ::com::sun::star::awt::FontSlant            meCharPosture;
            sal_Int16                                   mnUnderlineMode;

            RGBColor                                    maFillColor;
            RGBColor                                    maLineColor;
            RGBColor                                    maCharColor;

            State::StateId                              mnTransformationState;
            State::StateId                              mnClipState;
            State::StateId                              mnAlphaState;
            State::StateId                              mnPositionState;
            State::StateId                              mnContentState;

            sal_Int16                                   mnAdditiveMode;

            bool                                        mbVisibility            : 1;

            bool                                        mbWidthValid            : 1;
            bool                                        mbHeightValid           : 1;
            bool                                        mbPosXValid             : 1;
            bool                                        mbPosYValid             : 1;
            bool                                        mbClipValid             : 1;

            bool                                        mbFontFamilyValid       : 1;

            bool                                        mbRotationAngleValid    : 1;
            bool                                        mbShearXAngleValid      : 1;
            bool                                        mbShearYAngleValid      : 1;

            bool                                        mbAlphaValid            : 1;

            bool                                        mbCharRotationAngleValid: 1;
            bool                                        mbCharScaleValid        : 1;

            bool                                        mbFillColorValid        : 1;
            bool                                        mbLineColorValid        : 1;
            bool                                        mbCharColorValid        : 1;

            bool                                        mbFillStyleValid        : 1;
            bool                                        mbLineStyleValid        : 1;
            bool                                        mbCharWeightValid       : 1;
            bool                                        mbUnderlineModeValid    : 1;
            bool                                        mbCharPostureValid      : 1;
            bool                                        mbVisibilityValid       : 1;
        };

    }
}

#endif /* _SLIDESHOW_SHAPEATTRIBUTELAYER_HXX */
