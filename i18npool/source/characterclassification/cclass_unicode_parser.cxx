/*************************************************************************
 *
 *  $RCSfile: cclass_unicode_parser.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: bustamam $ $Date: 2002-08-14 18:48:28 $
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

#include <cclass_unicode.hxx>
#include <unicode.hxx>

#ifndef _TOOLS_INTN_HXX
#include <tools/intn.hxx>
#endif
#ifndef _ISOLANG_HXX
#include <tools/isolang.hxx>
#endif
#ifndef _TOOLS_SOLMATH_HXX
#include <tools/solmath.hxx>
#endif

#ifndef _COM_SUN_STAR_I18N_KPARSETOKENS_HPP_
#include <com/sun/star/i18n/KParseTokens.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_KPARSETYPE_HPP_
#include <com/sun/star/i18n/KParseType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_UNICODETYPE_HPP_
#include <com/sun/star/i18n/UnicodeType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_XLOCALEDATA_HPP_
#include <com/sun/star/i18n/XLocaleData.hpp>
#endif
#include <drafts/com/sun/star/i18n/NativeNumberMode.hpp>

#include <string.h>     // memcpy()

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::rtl;
using namespace ::drafts::com::sun::star::i18n;

namespace com { namespace sun { namespace star { namespace i18n {

const UPT_FLAG_TYPE cclass_Unicode::TOKEN_ILLEGAL       = 0x00000000;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_CHAR          = 0x00000001;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_CHAR_BOOL = 0x00000002;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_CHAR_WORD = 0x00000004;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_CHAR_VALUE    = 0x00000008;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_CHAR_STRING   = 0x00000010;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_CHAR_DONTCARE= 0x00000020;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_BOOL          = 0x00000040;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_WORD          = 0x00000080;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_WORD_SEP      = 0x00000100;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_VALUE     = 0x00000200;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_VALUE_SEP = 0x00000400;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_VALUE_EXP = 0x00000800;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_VALUE_SIGN    = 0x00001000;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_VALUE_EXP_VALUE   = 0x00002000;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_VALUE_DIGIT   = 0x00004000;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_NAME_SEP      = 0x20000000;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_STRING_SEP    = 0x40000000;
const UPT_FLAG_TYPE cclass_Unicode::TOKEN_EXCLUDED      = 0x80000000;

#define TOKEN_DIGIT_FLAGS (TOKEN_CHAR_VALUE | TOKEN_VALUE | TOKEN_VALUE_EXP | TOKEN_VALUE_EXP_VALUE | TOKEN_VALUE_DIGIT)

// Default identifier/name specification is [A-Za-z_][A-Za-z0-9_]*

const sal_uInt8 cclass_Unicode::nDefCnt = 128;
const UPT_FLAG_TYPE cclass_Unicode::pDefaultParserTable[ nDefCnt ] =
{
// (...) == Calc formula compiler specific, commented out and modified

    /* \0 */    TOKEN_EXCLUDED,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
    /*  9 \t */ TOKEN_CHAR_DONTCARE | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,     // (TOKEN_ILLEGAL)
                TOKEN_ILLEGAL,
    /* 11 \v */ TOKEN_CHAR_DONTCARE | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,     // (TOKEN_ILLEGAL)
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
                TOKEN_ILLEGAL,
    /*  32   */ TOKEN_CHAR_DONTCARE | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  33 ! */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  34 " */ TOKEN_CHAR_STRING | TOKEN_STRING_SEP,
    /*  35 # */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_WORD_SEP)
    /*  36 $ */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_CHAR_WORD | TOKEN_WORD)
    /*  37 % */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_VALUE)
    /*  38 & */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  39 ' */ TOKEN_NAME_SEP,
    /*  40 ( */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  41 ) */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  42 * */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  43 + */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP | TOKEN_VALUE_EXP | TOKEN_VALUE_SIGN,
    /*  44 , */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_CHAR_VALUE | TOKEN_VALUE)
    /*  45 - */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP | TOKEN_VALUE_EXP | TOKEN_VALUE_SIGN,
    /*  46 . */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_WORD | TOKEN_CHAR_VALUE | TOKEN_VALUE)
    /*  47 / */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    //for ( i = 48; i < 58; i++ )
    /*  48 0 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  49 1 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  50 2 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  51 3 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  52 4 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  53 5 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  54 6 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  55 7 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  56 8 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  57 9 */ TOKEN_DIGIT_FLAGS | TOKEN_WORD,
    /*  58 : */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_WORD)
    /*  59 ; */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  60 < */ TOKEN_CHAR_BOOL | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  61 = */ TOKEN_CHAR | TOKEN_BOOL | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  62 > */ TOKEN_CHAR_BOOL | TOKEN_BOOL | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  63 ? */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_CHAR_WORD | TOKEN_WORD)
    /*  64 @ */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    //for ( i = 65; i < 91; i++ )
    /*  65 A */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  66 B */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  67 C */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  68 D */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  69 E */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  70 F */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  71 G */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  72 H */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  73 I */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  74 J */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  75 K */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  76 L */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  77 M */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  78 N */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  79 O */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  80 P */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  81 Q */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  82 R */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  83 S */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  84 T */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  85 U */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  86 V */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  87 W */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  88 X */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  89 Y */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  90 Z */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  91 [ */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    /*  92 \ */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    /*  93 ] */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    /*  94 ^ */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,
    /*  95 _ */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  96 ` */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    //for ( i = 97; i < 123; i++ )
    /*  97 a */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  98 b */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /*  99 c */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 100 d */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 101 e */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 102 f */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 103 g */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 104 h */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 105 i */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 106 j */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 107 k */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 108 l */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 109 m */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 110 n */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 111 o */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 112 p */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 113 q */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 114 r */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 115 s */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 116 t */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 117 u */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 118 v */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 119 w */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 120 x */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 121 y */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 122 z */ TOKEN_CHAR_WORD | TOKEN_WORD,
    /* 123 { */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    /* 124 | */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    /* 125 } */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    /* 126 ~ */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP,  // (TOKEN_ILLEGAL // UNUSED)
    /* 127   */ TOKEN_CHAR | TOKEN_WORD_SEP | TOKEN_VALUE_SEP   // (TOKEN_ILLEGAL // UNUSED)
};


const sal_Int32 cclass_Unicode::pParseTokensType[ nDefCnt ] =
{
    /* \0 */    KParseTokens::ASC_OTHER,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
    /*  9 \t */ KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
    /* 11 \v */ KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
                KParseTokens::ASC_CONTROL,
    /*  32   */ KParseTokens::ASC_OTHER,
    /*  33 ! */ KParseTokens::ASC_OTHER,
    /*  34 " */ KParseTokens::ASC_OTHER,
    /*  35 # */ KParseTokens::ASC_OTHER,
    /*  36 $ */ KParseTokens::ASC_DOLLAR,
    /*  37 % */ KParseTokens::ASC_OTHER,
    /*  38 & */ KParseTokens::ASC_OTHER,
    /*  39 ' */ KParseTokens::ASC_OTHER,
    /*  40 ( */ KParseTokens::ASC_OTHER,
    /*  41 ) */ KParseTokens::ASC_OTHER,
    /*  42 * */ KParseTokens::ASC_OTHER,
    /*  43 + */ KParseTokens::ASC_OTHER,
    /*  44 , */ KParseTokens::ASC_OTHER,
    /*  45 - */ KParseTokens::ASC_OTHER,
    /*  46 . */ KParseTokens::ASC_DOT,
    /*  47 / */ KParseTokens::ASC_OTHER,
    //for ( i = 48; i < 58; i++ )
    /*  48 0 */ KParseTokens::ASC_DIGIT,
    /*  49 1 */ KParseTokens::ASC_DIGIT,
    /*  50 2 */ KParseTokens::ASC_DIGIT,
    /*  51 3 */ KParseTokens::ASC_DIGIT,
    /*  52 4 */ KParseTokens::ASC_DIGIT,
    /*  53 5 */ KParseTokens::ASC_DIGIT,
    /*  54 6 */ KParseTokens::ASC_DIGIT,
    /*  55 7 */ KParseTokens::ASC_DIGIT,
    /*  56 8 */ KParseTokens::ASC_DIGIT,
    /*  57 9 */ KParseTokens::ASC_DIGIT,
    /*  58 : */ KParseTokens::ASC_COLON,
    /*  59 ; */ KParseTokens::ASC_OTHER,
    /*  60 < */ KParseTokens::ASC_OTHER,
    /*  61 = */ KParseTokens::ASC_OTHER,
    /*  62 > */ KParseTokens::ASC_OTHER,
    /*  63 ? */ KParseTokens::ASC_OTHER,
    /*  64 @ */ KParseTokens::ASC_OTHER,
    //for ( i = 65; i < 91; i++ )
    /*  65 A */ KParseTokens::ASC_UPALPHA,
    /*  66 B */ KParseTokens::ASC_UPALPHA,
    /*  67 C */ KParseTokens::ASC_UPALPHA,
    /*  68 D */ KParseTokens::ASC_UPALPHA,
    /*  69 E */ KParseTokens::ASC_UPALPHA,
    /*  70 F */ KParseTokens::ASC_UPALPHA,
    /*  71 G */ KParseTokens::ASC_UPALPHA,
    /*  72 H */ KParseTokens::ASC_UPALPHA,
    /*  73 I */ KParseTokens::ASC_UPALPHA,
    /*  74 J */ KParseTokens::ASC_UPALPHA,
    /*  75 K */ KParseTokens::ASC_UPALPHA,
    /*  76 L */ KParseTokens::ASC_UPALPHA,
    /*  77 M */ KParseTokens::ASC_UPALPHA,
    /*  78 N */ KParseTokens::ASC_UPALPHA,
    /*  79 O */ KParseTokens::ASC_UPALPHA,
    /*  80 P */ KParseTokens::ASC_UPALPHA,
    /*  81 Q */ KParseTokens::ASC_UPALPHA,
    /*  82 R */ KParseTokens::ASC_UPALPHA,
    /*  83 S */ KParseTokens::ASC_UPALPHA,
    /*  84 T */ KParseTokens::ASC_UPALPHA,
    /*  85 U */ KParseTokens::ASC_UPALPHA,
    /*  86 V */ KParseTokens::ASC_UPALPHA,
    /*  87 W */ KParseTokens::ASC_UPALPHA,
    /*  88 X */ KParseTokens::ASC_UPALPHA,
    /*  89 Y */ KParseTokens::ASC_UPALPHA,
    /*  90 Z */ KParseTokens::ASC_UPALPHA,
    /*  91 [ */ KParseTokens::ASC_OTHER,
    /*  92 \ */ KParseTokens::ASC_OTHER,
    /*  93 ] */ KParseTokens::ASC_OTHER,
    /*  94 ^ */ KParseTokens::ASC_OTHER,
    /*  95 _ */ KParseTokens::ASC_UNDERSCORE,
    /*  96 ` */ KParseTokens::ASC_OTHER,
    //for ( i = 97; i < 123; i++ )
    /*  97 a */ KParseTokens::ASC_LOALPHA,
    /*  98 b */ KParseTokens::ASC_LOALPHA,
    /*  99 c */ KParseTokens::ASC_LOALPHA,
    /* 100 d */ KParseTokens::ASC_LOALPHA,
    /* 101 e */ KParseTokens::ASC_LOALPHA,
    /* 102 f */ KParseTokens::ASC_LOALPHA,
    /* 103 g */ KParseTokens::ASC_LOALPHA,
    /* 104 h */ KParseTokens::ASC_LOALPHA,
    /* 105 i */ KParseTokens::ASC_LOALPHA,
    /* 106 j */ KParseTokens::ASC_LOALPHA,
    /* 107 k */ KParseTokens::ASC_LOALPHA,
    /* 108 l */ KParseTokens::ASC_LOALPHA,
    /* 109 m */ KParseTokens::ASC_LOALPHA,
    /* 110 n */ KParseTokens::ASC_LOALPHA,
    /* 111 o */ KParseTokens::ASC_LOALPHA,
    /* 112 p */ KParseTokens::ASC_LOALPHA,
    /* 113 q */ KParseTokens::ASC_LOALPHA,
    /* 114 r */ KParseTokens::ASC_LOALPHA,
    /* 115 s */ KParseTokens::ASC_LOALPHA,
    /* 116 t */ KParseTokens::ASC_LOALPHA,
    /* 117 u */ KParseTokens::ASC_LOALPHA,
    /* 118 v */ KParseTokens::ASC_LOALPHA,
    /* 119 w */ KParseTokens::ASC_LOALPHA,
    /* 120 x */ KParseTokens::ASC_LOALPHA,
    /* 121 y */ KParseTokens::ASC_LOALPHA,
    /* 122 z */ KParseTokens::ASC_LOALPHA,
    /* 123 { */ KParseTokens::ASC_OTHER,
    /* 124 | */ KParseTokens::ASC_OTHER,
    /* 125 } */ KParseTokens::ASC_OTHER,
    /* 126 ~ */ KParseTokens::ASC_OTHER,
    /* 127   */ KParseTokens::ASC_OTHER
};


// static
const sal_Unicode* cclass_Unicode::StrChr( const sal_Unicode* pStr, sal_Unicode c )
{
    if ( !pStr )
        return NULL;
    while ( *pStr )
    {
        if ( *pStr == c )
            return pStr;
        pStr++;
    }
    return NULL;
}


sal_Int32 cclass_Unicode::getParseTokensType( sal_Unicode c )
{
    if ( c < nDefCnt )
        return pParseTokensType[ sal_uInt8(c) ];
    else
    {

        //! all KParseTokens::UNI_... must be matched
        switch ( unicode::getUnicodeType( c ) )
        {
            case UnicodeType::UPPERCASE_LETTER :
                return KParseTokens::UNI_UPALPHA;
            break;
            case UnicodeType::LOWERCASE_LETTER :
                return KParseTokens::UNI_LOALPHA;
            break;
            case UnicodeType::TITLECASE_LETTER :
                return KParseTokens::UNI_TITLE_ALPHA;
            break;
            case UnicodeType::MODIFIER_LETTER :
                return KParseTokens::UNI_MODIFIER_LETTER;
            break;
            case UnicodeType::OTHER_LETTER :
                return KParseTokens::UNI_OTHER_LETTER;
            break;
            case UnicodeType::DECIMAL_DIGIT_NUMBER :
                return KParseTokens::UNI_DIGIT;
            break;
            case UnicodeType::LETTER_NUMBER :
                return KParseTokens::UNI_LETTER_NUMBER;
            break;
            case UnicodeType::OTHER_NUMBER :
                return KParseTokens::UNI_OTHER_NUMBER;
            break;
        }

        return KParseTokens::UNI_OTHER;
    }
}

sal_Bool cclass_Unicode::setupInternational( const Locale& rLocale )
{
    sal_Bool bChanged = (aParserLocale.Language != rLocale.Language
        || aParserLocale.Country != rLocale.Country
        || aParserLocale.Variant != rLocale.Variant);
    if ( bChanged )
    {
        aParserLocale.Language = rLocale.Language;
        aParserLocale.Country = rLocale.Country;
        aParserLocale.Variant = rLocale.Variant;
    }
    if ( !xLocaleData.is() && xMSF.is() )
    {
        Reference <
            XInterface > xI =
            xMSF->createInstance( OUString(
            RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.i18n.LocaleData" ) ) );
        if ( xI.is() )
        {
            Any x = xI->queryInterface( getCppuType((const Reference< XLocaleData>*)0) );
            x >>= xLocaleData;
        }
    }
    return bChanged;
}


void cclass_Unicode::setupParserTable( const Locale& rLocale, sal_Int32 startCharTokenType,
            const OUString& userDefinedCharactersStart, sal_Int32 contCharTokenType,
            const OUString& userDefinedCharactersCont )
{
    BOOL bIntlEqual = (rLocale.Language == aParserLocale.Language &&
        rLocale.Country == aParserLocale.Country &&
        rLocale.Variant == aParserLocale.Variant);
    if ( !pTable || !bIntlEqual ||
            startCharTokenType != nStartTypes ||
            contCharTokenType != nContTypes ||
            userDefinedCharactersStart != aStartChars ||
            userDefinedCharactersCont != aContChars )
        initParserTable( rLocale, startCharTokenType, userDefinedCharactersStart,
            contCharTokenType, userDefinedCharactersCont );
}


void cclass_Unicode::initParserTable( const Locale& rLocale, sal_Int32 startCharTokenType,
            const OUString& userDefinedCharactersStart, sal_Int32 contCharTokenType,
            const OUString& userDefinedCharactersCont )
{
    // (Re)Init
    setupInternational( rLocale );
    // Memory of pTable is reused.
    if ( !pTable )
        pTable = new UPT_FLAG_TYPE[nDefCnt];
    memcpy( pTable, pDefaultParserTable, sizeof(UPT_FLAG_TYPE) * nDefCnt );
    // Start and cont tables only need reallocation if different length.
    if ( pStart && userDefinedCharactersStart.getLength() != aStartChars.getLength() )
    {
        delete [] pStart;
        pStart = NULL;
    }
    if ( pCont && userDefinedCharactersCont.getLength() != aContChars.getLength() )
    {
        delete [] pCont;
        pCont = NULL;
    }
    nStartTypes = startCharTokenType;
    nContTypes = contCharTokenType;
    aStartChars = userDefinedCharactersStart;
    aContChars = userDefinedCharactersCont;

    // specials
    if( xLocaleData.is() )
    {
        LocaleDataItem aItem =
            xLocaleData->getLocaleItem( aParserLocale );
//!TODO: theoretically separators may be a string, adjustment would have to be
//! done here and in parsing and in SolarMath::StringToDouble
        cGroupSep = aItem.thousandSeparator.getStr()[0];
        cDecimalSep = aItem.decimalSeparator.getStr()[0];
    }

    if ( cGroupSep < nDefCnt )
        pTable[cGroupSep] |= TOKEN_VALUE;
    if ( cDecimalSep < nDefCnt )
        pTable[cDecimalSep] |= TOKEN_CHAR_VALUE | TOKEN_VALUE;

    // Modify characters according to KParseTokens definitions.
    {
        using namespace KParseTokens;
        sal_uInt8 i;

        if ( !(nStartTypes & ASC_UPALPHA) )
            for ( i = 65; i < 91; i++ )
                pTable[i] &= ~TOKEN_CHAR_WORD;  // not allowed as start character
        if ( !(nContTypes & ASC_UPALPHA) )
            for ( i = 65; i < 91; i++ )
                pTable[i] &= ~TOKEN_WORD;       // not allowed as cont character

        if ( !(nStartTypes & ASC_LOALPHA) )
            for ( i = 97; i < 123; i++ )
                pTable[i] &= ~TOKEN_CHAR_WORD;  // not allowed as start character
        if ( !(nContTypes & ASC_LOALPHA) )
            for ( i = 97; i < 123; i++ )
                pTable[i] &= ~TOKEN_WORD;       // not allowed as cont character

        if ( nStartTypes & ASC_DIGIT )
            for ( i = 48; i < 58; i++ )
                pTable[i] |= TOKEN_CHAR_WORD;   // allowed as start character
        if ( !(nContTypes & ASC_DIGIT) )
            for ( i = 48; i < 58; i++ )
                pTable[i] &= ~TOKEN_WORD;       // not allowed as cont character

        if ( !(nStartTypes & ASC_UNDERSCORE) )
            pTable[95] &= ~TOKEN_CHAR_WORD;     // not allowed as start character
        if ( !(nContTypes & ASC_UNDERSCORE) )
            pTable[95] &= ~TOKEN_WORD;          // not allowed as cont character

        if ( nStartTypes & ASC_DOLLAR )
            pTable[36] |= TOKEN_CHAR_WORD;      // allowed as start character
        if ( nContTypes & ASC_DOLLAR )
            pTable[36] |= TOKEN_WORD;           // allowed as cont character

        if ( nStartTypes & ASC_DOT )
            pTable[46] |= TOKEN_CHAR_WORD;      // allowed as start character
        if ( nContTypes & ASC_DOT )
            pTable[46] |= TOKEN_WORD;           // allowed as cont character

        if ( nStartTypes & ASC_COLON )
            pTable[58] |= TOKEN_CHAR_WORD;      // allowed as start character
        if ( nContTypes & ASC_COLON )
            pTable[58] |= TOKEN_WORD;           // allowed as cont character

        if ( nStartTypes & ASC_CONTROL )
            for ( i = 1; i < 32; i++ )
                pTable[i] |= TOKEN_CHAR_WORD;   // allowed as start character
        if ( nContTypes & ASC_CONTROL )
            for ( i = 1; i < 32; i++ )
                pTable[i] |= TOKEN_WORD;        // allowed as cont character

        if ( nStartTypes & ASC_ANY_BUT_CONTROL )
            for ( i = 32; i < nDefCnt; i++ )
                pTable[i] |= TOKEN_CHAR_WORD;   // allowed as start character
        if ( nContTypes & ASC_ANY_BUT_CONTROL )
            for ( i = 32; i < nDefCnt; i++ )
                pTable[i] |= TOKEN_WORD;        // allowed as cont character

    }

    // Merge in (positively override with) user defined characters.
    // StartChars
    sal_Int32 nLen = aStartChars.getLength();
    if ( nLen )
    {
        if ( !pStart )
            pStart = new UPT_FLAG_TYPE[ nLen ];
        const sal_Unicode* p = aStartChars.getStr();
        for ( sal_Int32 j=0; j<nLen; j++, p++ )
        {
            pStart[j] = TOKEN_CHAR_WORD;
            if ( *p < nDefCnt )
                pTable[*p] |= TOKEN_CHAR_WORD;
        }
    }
    // ContChars
    nLen = aContChars.getLength();
    if ( nLen )
    {
        if ( !pCont )
            pCont = new UPT_FLAG_TYPE[ nLen ];
        const sal_Unicode* p = aContChars.getStr();
        for ( sal_Int32 j=0; j<nLen; j++ )
        {
            pCont[j] = TOKEN_WORD;
            if ( *p < nDefCnt )
                pTable[*p] |= TOKEN_WORD;
        }
    }
}


void cclass_Unicode::destroyParserTable()
{
    if ( pCont )
        delete [] pCont;
    if ( pStart )
        delete [] pStart;
    if ( pTable )
        delete [] pTable;
}


UPT_FLAG_TYPE cclass_Unicode::getFlags( sal_Unicode c )
{
    UPT_FLAG_TYPE nMask;
    if ( c < nDefCnt )
        nMask = pTable[ sal_uInt8(c) ];
    else
        nMask = getFlagsExtended( c );
    switch ( eState )
    {
        case ssGetChar :
        case ssGetWordFirstChar :
            if ( !(nMask & TOKEN_CHAR_WORD) )
            {
                nMask |= getStartCharsFlags( c );
                if ( nMask & TOKEN_CHAR_WORD )
                    nMask &= ~TOKEN_EXCLUDED;
            }
        break;
        case ssGetValue :
        case ssGetWord :
            if ( !(nMask & TOKEN_WORD) )
            {
                nMask |= getContCharsFlags( c );
                if ( nMask & TOKEN_WORD )
                    nMask &= ~TOKEN_EXCLUDED;
            }
        break;
        // other cases aren't needed
    }
    return nMask;
}


UPT_FLAG_TYPE cclass_Unicode::getFlagsExtended( sal_Unicode c )
{
    if ( c == cGroupSep )
        return TOKEN_VALUE;
    else if ( c == cDecimalSep )
        return TOKEN_CHAR_VALUE | TOKEN_VALUE;
    using namespace i18n;
    BOOL bStart = (eState == ssGetChar || eState == ssGetWordFirstChar);
    sal_Int32 nTypes = (bStart ? nStartTypes : nContTypes);

    //! all KParseTokens::UNI_... must be matched
    switch ( unicode::getUnicodeType( c ) )
    {
        case UnicodeType::UPPERCASE_LETTER :
            return (nTypes & KParseTokens::UNI_UPALPHA) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL;
        break;
        case UnicodeType::LOWERCASE_LETTER :
            return (nTypes & KParseTokens::UNI_LOALPHA) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL;
        break;
        case UnicodeType::TITLECASE_LETTER :
            return (nTypes & KParseTokens::UNI_TITLE_ALPHA) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL;
        break;
        case UnicodeType::MODIFIER_LETTER :
            return (nTypes & KParseTokens::UNI_MODIFIER_LETTER) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL;
        break;
        case UnicodeType::OTHER_LETTER :
            return (nTypes & KParseTokens::UNI_OTHER_LETTER) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL;
        break;
        case UnicodeType::DECIMAL_DIGIT_NUMBER :
            return ((nTypes & KParseTokens::UNI_DIGIT) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL) | TOKEN_DIGIT_FLAGS;
        break;
        case UnicodeType::LETTER_NUMBER :
            return ((nTypes & KParseTokens::UNI_LETTER_NUMBER) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL) | TOKEN_DIGIT_FLAGS;
        break;
        case UnicodeType::OTHER_NUMBER :
            return ((nTypes & KParseTokens::UNI_OTHER_NUMBER) ?
                (bStart ? TOKEN_CHAR_WORD : TOKEN_WORD) :
                TOKEN_ILLEGAL) | TOKEN_DIGIT_FLAGS;
        break;
        case UnicodeType::SPACE_SEPARATOR :
            return ((nTypes & KParseTokens::IGNORE_LEADING_WS) ?
                TOKEN_CHAR_DONTCARE : (bStart ? TOKEN_CHAR_WORD : (TOKEN_CHAR_DONTCARE | TOKEN_WORD_SEP | TOKEN_VALUE_SEP) ));
        break;
    }

    return TOKEN_ILLEGAL;
}


UPT_FLAG_TYPE cclass_Unicode::getStartCharsFlags( sal_Unicode c )
{
    if ( pStart )
    {
        const sal_Unicode* pStr = aStartChars.getStr();
        const sal_Unicode* p = StrChr( pStr, c );
        if ( p )
            return pStart[ p - pStr ];
    }
    return TOKEN_ILLEGAL;
}


UPT_FLAG_TYPE cclass_Unicode::getContCharsFlags( sal_Unicode c )
{
    if ( pCont )
    {
        const sal_Unicode* pStr = aContChars.getStr();
        const sal_Unicode* p = StrChr( pStr, c );
        if ( p )
            return pCont[ p - pStr ];
    }
    return TOKEN_ILLEGAL;
}


void cclass_Unicode::parseText( ParseResult& r, const OUString& rText, sal_Int32 nPos, sal_Int32 nTokenType )
{
    using namespace i18n;
    const sal_Unicode* const pStart = rText.getStr() + nPos;
    const sal_Unicode* pSym = pStart;
    const sal_Unicode* pSrc = pSym;
    OUString aSymbol;
    sal_Unicode c = *pSrc;
    sal_Unicode cLast = 0;
    BOOL bQuote = FALSE;
    BOOL bMightBeWord = TRUE;
    BOOL bMightBeWordLast = TRUE;
    eState = ssGetChar;
    while ( (c != 0) && (eState != ssStop) )
    {
        pSrc++;
        UPT_FLAG_TYPE nMask = getFlags( c );
        if ( nMask & TOKEN_EXCLUDED )
            eState = ssBounce;
        if ( bMightBeWord )
        {   // only relevant for ssGetValue fall back
            if ( eState == ssGetChar )
                bMightBeWord = ((nMask & TOKEN_CHAR_WORD) != 0);
            else
                bMightBeWord = ((nMask & TOKEN_WORD) != 0);
        }
        sal_Int32 nParseTokensType = getParseTokensType( c );
        switch (eState)
        {
            case ssGetChar :
            {
                if ( nMask & TOKEN_CHAR_VALUE )
                {   //! must be first, may fall back to ssGetWord via bMightBeWord
                    eState = ssGetValue;
                    if ( (nMask & TOKEN_VALUE_DIGIT) && 128 <= c )
                        r.TokenType = KParseType::UNI_NUMBER;
                    else
                        r.TokenType = KParseType::ASC_NUMBER;
                }
                else if ( nMask & TOKEN_CHAR_WORD )
                {
                    eState = ssGetWord;
                    r.TokenType = KParseType::IDENTNAME;
                }
                else if ( nMask & TOKEN_NAME_SEP )
                {
                    eState = ssGetWordFirstChar;
                    bQuote = TRUE;
                    pSym++;
                    nParseTokensType = 0;   // will be taken of first real character
                    r.TokenType = KParseType::SINGLE_QUOTE_NAME;
                }
                else if ( nMask & TOKEN_CHAR_STRING )
                {
                    eState = ssGetString;
                    pSym++;
                    nParseTokensType = 0;   // will be taken of first real character
                    r.TokenType = KParseType::DOUBLE_QUOTE_STRING;
                }
                else if ( nMask & TOKEN_CHAR_DONTCARE )
                {
                    if ( nStartTypes & KParseTokens::IGNORE_LEADING_WS )
                    {
                        r.LeadingWhiteSpace++;
                        pSym++;
                        nParseTokensType = 0;   // wait until real character
                        bMightBeWord = TRUE;
                    }
                    else
                        eState = ssBounce;
                }
                else if ( nMask & TOKEN_CHAR_BOOL )
                {
                    eState = ssGetBool;
                    r.TokenType = KParseType::BOOLEAN;
                }
                else if ( nMask & TOKEN_CHAR )
                {   //! must be last
                    eState = ssStop;
                    r.TokenType = KParseType::ONE_SINGLE_CHAR;
                }
                else
                    eState = ssBounce;      // not known
            }
            break;
            case ssGetValue :
            {
                if ( (nMask & TOKEN_VALUE_DIGIT) && 128 <= c )
                    r.TokenType = KParseType::UNI_NUMBER;
                if ( nMask & TOKEN_VALUE )
                    ;   // keep it going
                else if ( c == 'E' || c == 'e' )
                {
                    UPT_FLAG_TYPE nNext = getFlags( *pSrc );
                    if ( nNext & TOKEN_VALUE_EXP )
                        ;   // keep it going
                    else if ( bMightBeWord && ((nNext & TOKEN_WORD) || !*pSrc) )
                    {   // might be a numerical name (1.2efg)
                        eState = ssGetWord;
                        r.TokenType = KParseType::IDENTNAME;
                    }
                    else
                        eState = ssStopBack;
                }
                else if ( nMask & TOKEN_VALUE_SIGN )
                {
                    if ( (cLast == 'E') || (cLast == 'e') )
                    {
                        UPT_FLAG_TYPE nNext = getFlags( *pSrc );
                        if ( nNext & TOKEN_VALUE_EXP_VALUE )
                            ;   // keep it going
                        else if ( bMightBeWord && ((nNext & TOKEN_WORD) || !*pSrc) )
                        {   // might be a numerical name (1.2e+fg)
                            eState = ssGetWord;
                            r.TokenType = KParseType::IDENTNAME;
                        }
                        else
                            eState = ssStopBack;
                    }
                    else if ( bMightBeWord )
                    {   // might be a numerical name (1.2+fg)
                        eState = ssGetWord;
                        r.TokenType = KParseType::IDENTNAME;
                    }
                    else
                        eState = ssStopBack;
                }
                else if ( bMightBeWord && (nMask & TOKEN_WORD) )
                {   // might be a numerical name (1995.A1)
                    eState = ssGetWord;
                    r.TokenType = KParseType::IDENTNAME;
                }
                else
                    eState = ssStopBack;
            }
            break;
            case ssGetWordFirstChar :
                eState = ssGetWord;
                // fall thru
            case ssGetWord :
            {
                if ( nMask & TOKEN_WORD )
                    ;   // keep it going
                else if ( nMask & TOKEN_NAME_SEP )
                {
                    if ( bQuote )
                    {
                        if ( cLast == '\\' )
                        {   // escaped
                            aSymbol += OUString( pSym, pSrc - pSym - 2 );
                            aSymbol += OUString(&c);
                        }
                        else
                        {
                            eState = ssStop;
                            aSymbol += OUString( pSym, pSrc - pSym - 1 );
                        }
                        pSym = pSrc;
                    }
                    else
                        eState = ssStopBack;
                }
                else if ( bQuote )
                    ;   // keep it going
                else
                    eState = ssStopBack;
            }
            break;
            case ssGetString :
            {
                if ( nMask & TOKEN_STRING_SEP )
                {
                    if ( cLast == '\\' )
                    {   // escaped
                        aSymbol += OUString( pSym, pSrc - pSym - 2 );
                        aSymbol += OUString(&c);
                    }
                    else if ( c == *pSrc &&
                            !(nContTypes & KParseTokens::TWO_DOUBLE_QUOTES_BREAK_STRING) )
                    {   // "" => literal " escaped
                        aSymbol += OUString( pSym, pSrc - pSym );
                        pSrc++;
                    }
                    else
                    {
                        eState = ssStop;
                        aSymbol += OUString( pSym, pSrc - pSym - 1 );
                    }
                    pSym = pSrc;
                }
            }
            break;
            case ssGetBool :
            {
                if ( (nMask & TOKEN_BOOL) )
                    eState = ssStop;    // maximum 2: <, >, <>, <=, >=
                else
                    eState = ssStopBack;
            }
            break;
        }
        if ( !(r.TokenType & nTokenType) )
        {
            if ( (r.TokenType & (KParseType::ASC_NUMBER | KParseType::UNI_NUMBER))
                    && (nTokenType & KParseType::IDENTNAME) && bMightBeWord )
                ;   // keep a number which might be a word
            else if ( r.LeadingWhiteSpace == (pSrc - pStart) )
                ;   // keep ignored white space
            else
                eState = ssBounce;
        }
        if ( eState == ssBounce )
        {
            r.TokenType = 0;
            eState = ssStopBack;
        }
        if ( eState == ssStopBack )
        {   // put back
            pSrc--;
            bMightBeWord = bMightBeWordLast;
            eState = ssStop;
        }
        if ( eState != ssStop )
        {
            if ( !r.StartFlags )
                r.StartFlags |= nParseTokensType;
            else
                r.ContFlags |= nParseTokensType;
        }
        bMightBeWordLast = bMightBeWord;
        cLast = c;
        c = *pSrc;
    }
    // r.CharLen is the length in characters (not code points) of the parsed
    // token not including any leading white space, change this calculation if
    // multi-code-point Unicode characters are to be supported.
    r.CharLen = pSrc - pStart - r.LeadingWhiteSpace;
    r.EndPos = nPos + (pSrc - pStart);
    if ( r.TokenType & KParseType::ASC_NUMBER )
    {
        int nErrno;
        r.Value = SolarMath::StringToDouble( pStart + r.LeadingWhiteSpace,
            cGroupSep, cDecimalSep, nErrno );
        if ( bMightBeWord )
            r.TokenType |= KParseType::IDENTNAME;
    }
    else if ( r.TokenType & KParseType::UNI_NUMBER )
    {
        if ( !xNatNumSup.is() )
        {
#define NATIVENUMBERSUPPLIER_SERVICENAME "com.sun.star.i18n.NativeNumberSupplier"
            if ( xMSF.is() )
            {
                xNatNumSup = Reference< XNativeNumberSupplier > (
                        xMSF->createInstance( OUString(
                                RTL_CONSTASCII_USTRINGPARAM(
                                    NATIVENUMBERSUPPLIER_SERVICENAME ) ) ),
                        UNO_QUERY );
            }
            if ( !xNatNumSup.is() )
            {
                throw RuntimeException( OUString(
#ifndef PRODUCT
                    RTL_CONSTASCII_USTRINGPARAM(
                        "cclass_Unicode::parseText: can't instanciate "
                        NATIVENUMBERSUPPLIER_SERVICENAME )
#endif
                    ), *this );
            }
#undef NATIVENUMBERSUPPLIER_SERVICENAME
        }
        OUString aTmp( pStart + r.LeadingWhiteSpace, r.EndPos - nPos +
                r.LeadingWhiteSpace );
        // transliterate to ASCII
        aTmp = xNatNumSup->getNativeNumberString( aTmp, aParserLocale,
                NativeNumberMode::NATNUM0 );
        int nErrno;
        r.Value = SolarMath::StringToDouble( aTmp.getStr(), cGroupSep,
                cDecimalSep, nErrno );
        if ( bMightBeWord )
            r.TokenType |= KParseType::IDENTNAME;
    }
    else if ( r.TokenType & (KParseType::SINGLE_QUOTE_NAME | KParseType::DOUBLE_QUOTE_STRING) )
    {
        if ( pSym < pSrc )
        {   //! open quote
            aSymbol += OUString( pSym, pSrc - pSym );
            r.TokenType |= KParseType::MISSING_QUOTE;
        }
        r.DequotedNameOrString = aSymbol;
    }
}

} } } }
