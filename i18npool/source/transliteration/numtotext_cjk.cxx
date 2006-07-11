/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: numtotext_cjk.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-11 12:51:12 $
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

#define TRANSLITERATION_ALL
#include <numtotext_cjk.hxx>
#include <bullet.h>
#include <com/sun/star/i18n/NativeNumberMode.hpp>

namespace com { namespace sun { namespace star { namespace i18n {

#define TRANSLITERATION_NUMTOTEXT( name, number ) \
NumToText##name::NumToText##name() \
{ \
        nNativeNumberMode = number; \
        tableSize = 0; \
        transliterationName = "NumToText"#name; \
        implementationName = "com.sun.star.i18n.Transliteration.NumToText"#name; \
}
using namespace com::sun::star::i18n::NativeNumberMode;

TRANSLITERATION_NUMTOTEXT( Lower_zh_CN, NATNUM4)
TRANSLITERATION_NUMTOTEXT( Upper_zh_CN, NATNUM5)
TRANSLITERATION_NUMTOTEXT( Lower_zh_TW, NATNUM4)
TRANSLITERATION_NUMTOTEXT( Upper_zh_TW, NATNUM5)
TRANSLITERATION_NUMTOTEXT( Fullwidth_zh_CN, NATNUM6)
TRANSLITERATION_NUMTOTEXT( Fullwidth_zh_TW, NATNUM6)
TRANSLITERATION_NUMTOTEXT( Fullwidth_ja_JP, NATNUM6)
TRANSLITERATION_NUMTOTEXT( Fullwidth_ko, NATNUM6)
TRANSLITERATION_NUMTOTEXT( FormalLower_ko, NATNUM4)
TRANSLITERATION_NUMTOTEXT( FormalUpper_ko, NATNUM5)
TRANSLITERATION_NUMTOTEXT( FormalHangul_ko, NATNUM10)
TRANSLITERATION_NUMTOTEXT( InformalLower_ko, NATNUM7)
TRANSLITERATION_NUMTOTEXT( InformalUpper_ko, NATNUM8)
TRANSLITERATION_NUMTOTEXT( InformalHangul_ko, NATNUM11)
TRANSLITERATION_NUMTOTEXT( KanjiLongTraditional_ja_JP, NATNUM5)
TRANSLITERATION_NUMTOTEXT( KanjiLongModern_ja_JP, NATNUM4)
TRANSLITERATION_NUMTOTEXT( Date_zh, NATNUM7)
TRANSLITERATION_NUMTOTEXT( KanjiShortTraditional_ja_JP, NATNUM8)
TRANSLITERATION_NUMTOTEXT( KanjiShortModern_ja_JP, NATNUM7)
#undef TRANSLITERATION_NUMTOTEXT

#define TRANSLITERATION_NUMTOTEXT( name, _table, recycle ) \
NumToText##name::NumToText##name() \
{ \
        table = _table;\
        tableSize = sizeof(_table) / sizeof(sal_Unicode); \
        recycleSymbol = recycle; \
        transliterationName = "NumToText"#name; \
        implementationName = "com.sun.star.i18n.Transliteration.NumToText"#name; \
}

TRANSLITERATION_NUMTOTEXT ( AIUFullWidth_ja_JP, table_AIUFullWidth_ja_JP, sal_True)
TRANSLITERATION_NUMTOTEXT ( AIUHalfWidth_ja_JP, table_AIUHalfWidth_ja_JP, sal_True)
TRANSLITERATION_NUMTOTEXT ( IROHAFullWidth_ja_JP, table_IROHAFullWidth_ja_JP, sal_True)
TRANSLITERATION_NUMTOTEXT ( IROHAHalfWidth_ja_JP, table_IROHAHalfWidth_ja_JP, sal_True)
TRANSLITERATION_NUMTOTEXT ( CircledNumber, table_CircledNumber, sal_False)
TRANSLITERATION_NUMTOTEXT ( TianGan_zh, table_TianGan_zh, sal_False)
TRANSLITERATION_NUMTOTEXT ( DiZi_zh, table_DiZi_zh, sal_False)
TRANSLITERATION_NUMTOTEXT ( HangulJamo_ko, table_HangulJamo_ko, sal_True)
TRANSLITERATION_NUMTOTEXT ( HangulSyllable_ko, table_HangulSyllable_ko, sal_True)
TRANSLITERATION_NUMTOTEXT ( HangulCircledJamo_ko, table_HangulCircledJamo_ko, sal_True)
TRANSLITERATION_NUMTOTEXT ( HangulCircledSyllable_ko, table_HangulCircledSyllable_ko, sal_True)
#undef TRANSLITERATION_NUMTOTEXT

// Only to prevent warnings gcc 3.4.1  (defined but not used)
sal_Unicode dummy = table_CyrillicUpperLetter_bg[0];
sal_Unicode dummy1 = table_CyrillicLowerLetter_bg[0];
sal_Unicode dummy2 = table_CyrillicUpperLetter_ru[0];
sal_Unicode dummy3 = table_CyrillicLowerLetter_ru[0];
} } } }
