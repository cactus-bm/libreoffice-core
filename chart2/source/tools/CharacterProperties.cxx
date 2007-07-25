/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: CharacterProperties.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-25 08:55:28 $
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
#include "CharacterProperties.hxx"
#include "ContainerHelper.hxx"
#include "macros.hxx"

#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/lang/Locale.hpp>

#include <com/sun/star/awt/FontFamily.hpp>
#include <com/sun/star/awt/CharSet.hpp>
#include <com/sun/star/awt/FontPitch.hpp>
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/style/CaseMap.hpp>
#include <com/sun/star/text/FontRelief.hpp>
#include <com/sun/star/text/FontEmphasis.hpp>
#include <com/sun/star/text/RubyAdjust.hpp>
#include <com/sun/star/awt/FontStrikeout.hpp>

#include <comphelper/InlineContainer.hxx>

using namespace ::com::sun::star;

using ::com::sun::star::beans::Property;

using ::rtl::OUString;

namespace chart
{

void CharacterProperties::AddPropertiesToVector(
    ::std::vector< Property > & rOutProperties )
{
    // CharacterProperties
    rOutProperties.push_back(
        Property( C2U( "CharFontName" ),
                  PROP_CHAR_FONT_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    rOutProperties.push_back(
        Property( C2U( "CharFontStyleName" ),
                  PROP_CHAR_FONT_STYLE_NAME,
                  ::getCppuType( reinterpret_cast< const  OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharFontFamily (see awt.FontFamily)
    rOutProperties.push_back(
        Property( C2U( "CharFontFamily" ),
                  PROP_CHAR_FONT_FAMILY,
                  ::getCppuType( reinterpret_cast< const  sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontCharSet (see awt.CharSet)
    rOutProperties.push_back(
        Property( C2U( "CharFontCharSet" ),
                  PROP_CHAR_FONT_CHAR_SET,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontPitch (see awt.FontPitch)
    rOutProperties.push_back(
        Property( C2U( "CharFontPitch" ),
                  PROP_CHAR_FONT_PITCH,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharColor
    rOutProperties.push_back(
        Property( C2U( "CharColor" ),
                  PROP_CHAR_COLOR,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharBackColor
//     rOutProperties.push_back(
//         Property( C2U( "CharBackColor" ),
//                   PROP_CHAR_BACKGROUND_COLOR,
//                   ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT));
    // CharEscapement
    rOutProperties.push_back(
        Property( C2U( "CharEscapement" ),
                  PROP_CHAR_ESCAPEMENT,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharHeight
    rOutProperties.push_back(
        Property( C2U( "CharHeight" ),
                  PROP_CHAR_CHAR_HEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharUnderline (see awt.FontUnderline)
    rOutProperties.push_back(
        Property( C2U( "CharUnderline" ),
                  PROP_CHAR_UNDERLINE,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharUnderlineColor
    rOutProperties.push_back(
        Property( C2U( "CharUnderlineColor" ),
                  PROP_CHAR_UNDERLINE_COLOR,
                  ::getCppuType( reinterpret_cast< const sal_Int32 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharUnderlineHasColor
    rOutProperties.push_back(
        Property( C2U( "CharUnderlineHasColor" ),
                  PROP_CHAR_UNDERLINE_HAS_COLOR,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharWeight (see awt.FontWeight)
    rOutProperties.push_back(
        Property( C2U( "CharWeight" ),
                  PROP_CHAR_WEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharPosture
    rOutProperties.push_back(
        Property( C2U( "CharPosture" ),
                  PROP_CHAR_POSTURE,
                  ::getCppuType( reinterpret_cast< const awt::FontSlant * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    rOutProperties.push_back(
        Property( C2U( "CharAutoKerning" ),
                  PROP_CHAR_AUTO_KERNING,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    rOutProperties.push_back(
        Property( C2U( "CharKerning" ),
                  PROP_CHAR_KERNING,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharCaseMap (see style.CaseMap)
//     rOutProperties.push_back(
//         Property( C2U( "CharCaseMap" ),
//                   PROP_CHAR_CASE_MAPPING,
//                   ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));


    // CharRotation
//     rOutProperties.push_back(
//         Property( C2U( "CharRotation" ),
//                   PROP_CHAR_ROTATION,
//                   ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));

//     // CharScaleWidth
//     rOutProperties.push_back(
//         Property( C2U( "CharScaleWidth" ),
//                   PROP_CHAR_SCALE_WIDTH,
//                   ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharEscapementHeight
    rOutProperties.push_back(
        Property( C2U( "CharEscapementHeight" ),
                  PROP_CHAR_ESCAPEMENT_HEIGHT,
                  ::getCppuType( reinterpret_cast< const sal_Int8 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    // CharCrossedOut
//     rOutProperties.push_back(
//         Property( C2U( "CharCrossedOut" ),
//                   PROP_CHAR_CROSSED_OUT,
//                   ::getBooleanCppuType(),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharStrikeout (see awt.FontStrikeout)
    rOutProperties.push_back(
        Property( C2U( "CharStrikeout" ),
                  PROP_CHAR_STRIKE_OUT,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharWordMode
    rOutProperties.push_back(
        Property( C2U( "CharWordMode" ),
                  PROP_CHAR_WORD_MODE,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFlash
//     rOutProperties.push_back(
//         Property( C2U( "CharFlash" ),
//                   PROP_CHAR_FLASH,
//                   ::getBooleanCppuType(),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharLocale
    rOutProperties.push_back(
        Property( C2U( "CharLocale" ),
                  PROP_CHAR_LOCALE,
                  ::getCppuType( reinterpret_cast< const lang::Locale * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharShadowed
    rOutProperties.push_back(
        Property( C2U( "CharShadowed" ),
                  PROP_CHAR_SHADOWED,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharContoured
    rOutProperties.push_back(
        Property( C2U( "CharContoured" ),
                  PROP_CHAR_CONTOURED,
                  ::getBooleanCppuType(),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharRelief (see text.FontRelief)
    rOutProperties.push_back(
        Property( C2U( "CharRelief" ),
                  PROP_CHAR_RELIEF,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    // CharEmphasize (see text.FontEmphasis)
    rOutProperties.push_back(
        Property( C2U( "CharEmphasis" ),
                  PROP_CHAR_EMPHASIS,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
//     // RubyText
//     rOutProperties.push_back(
//         Property( C2U( "RubyText" ),
//                   PROP_CHAR_RUBY_TEXT,
//                   ::getCppuType( reinterpret_cast< const OUString * >(0)),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));
//     // RubyAdjust (see text.RubyAdjust)
//     rOutProperties.push_back(
//         Property( C2U( "RubyAdjust" ),
//                   PROP_CHAR_RUBY_ADJUST,
//                   ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));
//     // RubyCharStyleName
//     rOutProperties.push_back(
//         Property( C2U( "RubyStyleName" ),
//                   PROP_CHAR_RUBY_STYLE_NAME,
//                   ::getCppuType( reinterpret_cast< const OUString * >(0)),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));
//     // RubyIsAbove
//     rOutProperties.push_back(
//         Property( C2U( "RubyIsAbove" ),
//                   PROP_CHAR_RUBY_IS_ABOVE,
//                   ::getBooleanCppuType(),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));
//     // CharNoHyphenation
//     rOutProperties.push_back(
//         Property( C2U( "InhibitHyphenation" ),
//                   PROP_CHAR_INHIBIT_HYPHENATION,
//                   ::getBooleanCppuType(),
//                   beans::PropertyAttribute::BOUND
//                   | beans::PropertyAttribute::MAYBEDEFAULT ));

    // CharacterPropertiesAsian
    // =====
    // CharFontNameAsian
    rOutProperties.push_back(
        Property( C2U( "CharFontNameAsian" ),
                  PROP_CHAR_ASIAN_FONT_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontStyleNameAsian
    rOutProperties.push_back(
        Property( C2U( "CharFontStyleNameAsian" ),
                  PROP_CHAR_ASIAN_FONT_STYLE_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharFontFamilyAsian (see awt.FontFamily)
    rOutProperties.push_back(
        Property( C2U( "CharFontFamilyAsian" ),
                  PROP_CHAR_ASIAN_FONT_FAMILY,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontCharSetAsian (see awt.CharSet)
    rOutProperties.push_back(
        Property( C2U( "CharFontCharSetAsian" ),
                  PROP_CHAR_ASIAN_CHAR_SET,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontPitchAsian (see awt.FontPitch)
    rOutProperties.push_back(
        Property( C2U( "CharFontPitchAsian" ),
                  PROP_CHAR_ASIAN_FONT_PITCH,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharHeightAsian
    rOutProperties.push_back(
        Property( C2U( "CharHeightAsian" ),
                  PROP_CHAR_ASIAN_CHAR_HEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharWeightAsian
    rOutProperties.push_back(
        Property( C2U( "CharWeightAsian" ),
                  PROP_CHAR_ASIAN_WEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharPostureAsian
    rOutProperties.push_back(
        Property( C2U( "CharPostureAsian" ),
                  PROP_CHAR_ASIAN_POSTURE,
                  ::getCppuType( reinterpret_cast< const awt::FontSlant * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharLocaleAsian
    rOutProperties.push_back(
        Property( C2U( "CharLocaleAsian" ),
                  PROP_CHAR_ASIAN_LOCALE,
                  ::getCppuType( reinterpret_cast< const lang::Locale * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));

    // CharacterPropertiesComplex
    // ===
    // CharFontNameComplex
    rOutProperties.push_back(
        Property( C2U( "CharFontNameComplex" ),
                  PROP_CHAR_COMPLEX_FONT_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontStyleNameComplex
    rOutProperties.push_back(
        Property( C2U( "CharFontStyleNameComplex" ),
                  PROP_CHAR_COMPLEX_FONT_STYLE_NAME,
                  ::getCppuType( reinterpret_cast< const OUString * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT
                  | beans::PropertyAttribute::MAYBEVOID ));
    // CharFontFamilyComplex (see awt.FontFamily)
    rOutProperties.push_back(
        Property( C2U( "CharFontFamilyComplex" ),
                  PROP_CHAR_COMPLEX_FONT_FAMILY,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontCharSetComplex (see awt.CharSet)
    rOutProperties.push_back(
        Property( C2U( "CharFontCharSetComplex" ),
                  PROP_CHAR_COMPLEX_CHAR_SET,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharFontPitchComplex (see awt.FontPitch)
    rOutProperties.push_back(
        Property( C2U( "CharFontPitchComplex" ),
                  PROP_CHAR_COMPLEX_FONT_PITCH,
                  ::getCppuType( reinterpret_cast< const sal_Int16 * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharHeightComplex
    rOutProperties.push_back(
        Property( C2U( "CharHeightComplex" ),
                  PROP_CHAR_COMPLEX_CHAR_HEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharWeightComplex
    rOutProperties.push_back(
        Property( C2U( "CharWeightComplex" ),
                  PROP_CHAR_COMPLEX_WEIGHT,
                  ::getCppuType( reinterpret_cast< const float * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharPostureComplex
    rOutProperties.push_back(
        Property( C2U( "CharPostureComplex" ),
                  PROP_CHAR_COMPLEX_POSTURE,
                  ::getCppuType( reinterpret_cast< const awt::FontSlant * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
    // CharLocaleComplex
    rOutProperties.push_back(
        Property( C2U( "CharLocaleComplex" ),
                  PROP_CHAR_COMPLEX_LOCALE,
                  ::getCppuType( reinterpret_cast< const lang::Locale * >(0)),
                  beans::PropertyAttribute::BOUND
                  | beans::PropertyAttribute::MAYBEDEFAULT ));
}

void CharacterProperties::AddDefaultsToMap(
    ::chart::tPropertyValueMap & rOutMap )
{
    const float fDefaultFontHeight = 13.0;

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_FONT_NAME ));
    rOutMap[ PROP_CHAR_FONT_NAME ] =
        uno::makeAny( C2U( "Albany" ));
    //todo correct font default:
    //see static Font OutputDevice::GetDefaultFont( USHORT nType, LanguageType eLang, ULONG nFlags, const OutputDevice* pOutDev = NULL );
    //or  SvxFontItem ... (old chart source/core/chtmodel.cxx :Font aCJKFont )
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_FONT_FAMILY ));
    rOutMap[ PROP_CHAR_FONT_FAMILY ] =
        uno::makeAny( awt::FontFamily::SWISS );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_FONT_CHAR_SET ));
    rOutMap[ PROP_CHAR_FONT_CHAR_SET ] =
        uno::makeAny( awt::CharSet::DONTKNOW );//use DONTKNOW instead of SYSTEM to avoid assertion issue 50249
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_FONT_PITCH ));
    rOutMap[ PROP_CHAR_FONT_PITCH ] =
        uno::makeAny( awt::FontPitch::VARIABLE );

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COLOR ));
    rOutMap[ PROP_CHAR_COLOR ] =
        uno::makeAny( sal_Int32(-1) );//automatic color (COL_AUTO)

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ESCAPEMENT ));
    rOutMap[ PROP_CHAR_ESCAPEMENT ] =
        uno::makeAny( sal_Int16(0) );


    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_CHAR_HEIGHT ));
    rOutMap[ PROP_CHAR_CHAR_HEIGHT ] =
        uno::makeAny( fDefaultFontHeight );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_UNDERLINE ));
    rOutMap[ PROP_CHAR_UNDERLINE ] =
        uno::makeAny( awt::FontUnderline::NONE );

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_UNDERLINE_COLOR ));
    rOutMap[ PROP_CHAR_UNDERLINE_COLOR ] =
        uno::makeAny( sal_Int32(-1) );//automatic color (COL_AUTO)

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_UNDERLINE_HAS_COLOR ));
    rOutMap[ PROP_CHAR_UNDERLINE_HAS_COLOR ] =
        uno::makeAny( sal_Bool( sal_False ) );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_WEIGHT ));
    rOutMap[ PROP_CHAR_WEIGHT ] =
        uno::makeAny( awt::FontWeight::NORMAL );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_POSTURE ));
    rOutMap[ PROP_CHAR_POSTURE ] =
        uno::makeAny( awt::FontSlant_NONE );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_AUTO_KERNING ));
    rOutMap[ PROP_CHAR_AUTO_KERNING ] =
        uno::makeAny( sal_True );

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_KERNING ));
    rOutMap[ PROP_CHAR_KERNING ] =
        uno::makeAny( sal_Int16(0) );//todo correct default


//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_CASE_MAPPING ));
//     rOutMap[ PROP_CHAR_CASE_MAPPING ] =
//         uno::makeAny( style::CaseMap::NONE );
//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ROTATION ));
//     rOutMap[ PROP_CHAR_ROTATION ] =
//         uno::makeAny( sal_Int16( 0 ) );
//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_SCALE_WIDTH ));
//     rOutMap[ PROP_CHAR_SCALE_WIDTH ] =
//         uno::makeAny( sal_Int16( 71 ) );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ESCAPEMENT_HEIGHT ));
    rOutMap[ PROP_CHAR_ESCAPEMENT_HEIGHT ] =
        uno::makeAny( sal_Int8( 0 ) );

//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_CROSSED_OUT ));
//     rOutMap[ PROP_CHAR_CROSSED_OUT ] =
//         uno::makeAny( sal_Bool( sal_False ) );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_STRIKE_OUT ));
    rOutMap[ PROP_CHAR_STRIKE_OUT ] =
        uno::makeAny( sal_Int16( awt::FontStrikeout::NONE ) );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_WORD_MODE ));
    rOutMap[ PROP_CHAR_WORD_MODE ] =
        uno::makeAny( sal_Bool( sal_False ) );
//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_FLASH ));
//     rOutMap[ PROP_CHAR_FLASH ] =
//         uno::makeAny( sal_Bool( sal_False ) );

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_LOCALE ));
    rOutMap[ PROP_CHAR_LOCALE ] =
        uno::makeAny( lang::Locale( C2U( "US" ), C2U( "en" ), OUString()) );//todo correct default
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_SHADOWED ));
    rOutMap[ PROP_CHAR_SHADOWED ] =
        uno::makeAny( sal_Bool( sal_False ) );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_CONTOURED ));
    rOutMap[ PROP_CHAR_CONTOURED ] =
        uno::makeAny( sal_Bool( sal_False ) );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_RELIEF ));
    rOutMap[ PROP_CHAR_RELIEF ] =
        uno::makeAny( text::FontRelief::NONE );

    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_EMPHASIS ));
    rOutMap[ PROP_CHAR_EMPHASIS ] =
        uno::makeAny( text::FontEmphasis::NONE );
//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_RUBY_ADJUST ));
//     rOutMap[ PROP_CHAR_RUBY_ADJUST ] =
//         uno::makeAny( text::RubyAdjust_INDENT_BLOCK );
//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_RUBY_STYLE_NAME ));
//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_RUBY_IS_ABOVE ));
//     rOutMap[ PROP_CHAR_RUBY_IS_ABOVE ] =
//         uno::makeAny( sal_Bool( sal_True ) );
//     OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_INHIBIT_HYPHENATION ));
//     rOutMap[ PROP_CHAR_INHIBIT_HYPHENATION ] =
//         uno::makeAny( sal_Bool( sal_False ) );

    // Asian (com.sun.star.style.CharacterPropertiesAsian)
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_CHAR_HEIGHT ));
    rOutMap[ PROP_CHAR_ASIAN_CHAR_HEIGHT ] =
        uno::makeAny( fDefaultFontHeight );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_WEIGHT ));
    rOutMap[ PROP_CHAR_ASIAN_WEIGHT ] =
        uno::makeAny( awt::FontWeight::NORMAL );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_POSTURE ));
    rOutMap[ PROP_CHAR_ASIAN_POSTURE ] =
        uno::makeAny( awt::FontSlant_NONE );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_LOCALE ));
    rOutMap[ PROP_CHAR_ASIAN_LOCALE ] =
        uno::makeAny( lang::Locale( C2U( "US" ), C2U( "en" ), OUString()) );//todo correct default
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_FONT_NAME ));
    rOutMap[ PROP_CHAR_ASIAN_FONT_NAME ] =
        uno::makeAny( C2U( "Andale Sans UI" ));
    //todo correct font default:
    //see static Font OutputDevice::GetDefaultFont( USHORT nType, LanguageType eLang, ULONG nFlags, const OutputDevice* pOutDev = NULL );
    //or  SvxFontItem ... (old chart source/core/chtmodel.cxx :Font aCJKFont )
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_FONT_FAMILY ));
    rOutMap[ PROP_CHAR_ASIAN_FONT_FAMILY ] =
        uno::makeAny( awt::FontFamily::SWISS );//todo correct default
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_CHAR_SET ));
    rOutMap[ PROP_CHAR_ASIAN_CHAR_SET ] =
        uno::makeAny( awt::CharSet::DONTKNOW );//use DONTKNOW instead of SYSTEM to avoid assertion issue 50249
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_ASIAN_FONT_PITCH ));
    rOutMap[ PROP_CHAR_ASIAN_FONT_PITCH ] =
        uno::makeAny( awt::FontPitch::VARIABLE );

    // Complex Text Layout (com.sun.star.style.CharacterPropertiesComplex)
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_CHAR_HEIGHT ));
    rOutMap[ PROP_CHAR_COMPLEX_CHAR_HEIGHT ] =
        uno::makeAny( fDefaultFontHeight );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_WEIGHT ));
    rOutMap[ PROP_CHAR_COMPLEX_WEIGHT ] =
        uno::makeAny( awt::FontWeight::NORMAL );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_POSTURE ));
    rOutMap[ PROP_CHAR_COMPLEX_POSTURE ] =
        uno::makeAny( awt::FontSlant_NONE );
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_LOCALE ));
    rOutMap[ PROP_CHAR_COMPLEX_LOCALE ] =
        uno::makeAny( lang::Locale( C2U( "US" ), C2U( "en" ), OUString()) );//todo correct default
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_FONT_NAME ));
    rOutMap[ PROP_CHAR_COMPLEX_FONT_NAME ] =
        uno::makeAny( C2U( "Tahoma" ));//todo correct default
    //todo correct font default:
    //see static Font OutputDevice::GetDefaultFont( USHORT nType, LanguageType eLang, ULONG nFlags, const OutputDevice* pOutDev = NULL );
    //or  SvxFontItem ... (old chart source/core/chtmodel.cxx :Font aCJKFont )
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_FONT_FAMILY ));
    rOutMap[ PROP_CHAR_COMPLEX_FONT_FAMILY ] =
        uno::makeAny( awt::FontFamily::SWISS );//todo correct default
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_CHAR_SET ));
    rOutMap[ PROP_CHAR_COMPLEX_CHAR_SET ] =
        uno::makeAny( awt::CharSet::DONTKNOW );//use DONTKNOW instead of SYSTEM to avoid assertion issue 50249
    OSL_ASSERT( rOutMap.end() == rOutMap.find( PROP_CHAR_COMPLEX_FONT_PITCH ));
    rOutMap[ PROP_CHAR_COMPLEX_FONT_PITCH ] =
        uno::makeAny( awt::FontPitch::VARIABLE );
}

bool CharacterProperties::IsCharacterPropertyHandle( sal_Int32 nHandle )
{
    return ( FAST_PROPERTY_ID_START_CHAR_PROP <= nHandle &&
             nHandle < CharacterProperties::FAST_PROPERTY_ID_END_CHAR_PROP );
}

// static
awt::FontDescriptor CharacterProperties::createFontDescriptorFromPropertySet(
    const uno::Reference< beans::XMultiPropertySet > & xMultiPropSet )
{
    awt::FontDescriptor aResult;
    // Note: keep this sorted!
    ::comphelper::MakeVector< OUString > aPropNames
        ( C2U("CharFontCharSet"));                // CharSet
    aPropNames
        ( C2U("CharFontFamily"))                  // Family
        ( C2U("CharFontName"))                    // Name
        ( C2U("CharFontPitch"))                   // Pitch
        ( C2U("CharFontStyleName"))               // StyleName
        ( C2U("CharHeight"))                      // Height
        ( C2U("CharPosture"))                     // Slant
        ( C2U("CharStrikeout"))                   // Strikeout
        ( C2U("CharUnderline"))                   // Underline
        ( C2U("CharWeight"))                      // Weight
        ( C2U("CharWordMode"))                    // WordLineMode
        ;

    uno::Sequence< OUString > aPropNameSeq( ContainerHelper::ContainerToSequence( aPropNames ));
    uno::Sequence< uno::Any > aValues( xMultiPropSet->getPropertyValues( aPropNameSeq ));

    sal_Int32 i=0;
    // Note keep this sorted according to the list above (comments are the fieldnames)
    aValues[ i++ ]  >>= aResult.CharSet;
    aValues[ i++ ]  >>= aResult.Family;
    aValues[ i++ ]  >>= aResult.Name;
    aValues[ i++ ]  >>= aResult.Pitch;
    aValues[ i++ ]  >>= aResult.StyleName;
    float fCharHeight;
    aValues[ i++ ]  >>= fCharHeight;
    aResult.Height = static_cast< sal_Int16 >( fCharHeight );
    aValues[ i++ ]  >>= aResult.Slant;
    aValues[ i++ ]  >>= aResult.Strikeout;
    aValues[ i++ ]  >>= aResult.Underline;
    aValues[ i++ ]  >>= aResult.Weight;
    aValues[ i++ ]  >>= aResult.WordLineMode;
    OSL_ASSERT( i == aValues.getLength());

    return aResult;
}

} //  namespace chart
