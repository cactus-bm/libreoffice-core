/*************************************************************************
 *
 *  $RCSfile: textconversion_zh.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-10-22 08:16:29 $
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

#include <assert.h>
#include <textconversion.hxx>
#include <com/sun/star/i18n/TextConversionType.hpp>
#include <com/sun/star/i18n/TextConversionOption.hpp>
#include <com/sun/star/linguistic2/ConversionDirection.hpp>
#include <com/sun/star/linguistic2/ConversionDictionaryType.hpp>
#include <i18nutil/x_rtl_ustring.h>

using namespace com::sun::star::lang;
using namespace com::sun::star::i18n;
using namespace com::sun::star::linguistic2;
using namespace com::sun::star::uno;
using namespace rtl;

namespace com { namespace sun { namespace star { namespace i18n {

// defined in stc_char.cxx and stc_word.cxx  generated from stc_char.dic and stc_word.dic
// by genconv_dict
extern const sal_uInt16* getSTC_CharIndex_S2T();
extern const sal_Unicode* getSTC_CharData_S2T();
extern const sal_uInt16* getSTC_CharIndex_S2V();
extern const sal_Unicode* getSTC_CharData_S2V();
extern const sal_uInt16* getSTC_CharIndex_T2S();
extern const sal_Unicode* getSTC_CharData_T2S();

extern const sal_uInt16* getSTC_WordIndex_S2T(sal_Int32 &count);
extern const sal_uInt16* getSTC_WordIndex_T2S(sal_Int32 &count);
extern const sal_uInt16* getSTC_WordEntry_S2T();
extern const sal_uInt16* getSTC_WordEntry_T2S();
extern const sal_Unicode* getSTC_WordData(sal_Int32 &count);

TextConversion_zh::TextConversion_zh( const Reference < XMultiServiceFactory >& xMSF )
{
    Reference < XInterface > xI;
    xI = xMSF->createInstance(
        OUString::createFromAscii( "com.sun.star.linguistic2.ConversionDictionaryList" ));
    if ( xI.is() )
        xI->queryInterface( getCppuType((const Reference< XConversionDictionaryList>*)0) ) >>= xCDL;

    implementationName = "com.sun.star.i18n.TextConversion_zh";
}

sal_Unicode SAL_CALL getOneCharConversion(sal_Unicode ch, const sal_Unicode* Data, const sal_uInt16* Index)
{
    sal_Unicode address = Index[ch>>8];
    if (address != 0xFFFF)
        address = Data[address + (ch & 0xFF)];
    return (address != 0xFFFF) ? address : ch;
}

OUString SAL_CALL getCharConversion(const OUString& aText, sal_Int32 nStartPos, sal_Int32 nLength, sal_Bool toSChinese, sal_Int32 nConversionOptions)
{
    const sal_Unicode *Data;
    const sal_uInt16 *Index;

    if (toSChinese) {
        Data = getSTC_CharData_T2S();
        Index = getSTC_CharIndex_T2S();
    } else if (nConversionOptions & TextConversionOption::USE_CHARACTER_VARIANTS) {
        Data = getSTC_CharData_S2V();
        Index = getSTC_CharIndex_S2V();
    } else {
        Data = getSTC_CharData_S2T();
        Index = getSTC_CharIndex_S2T();
    }

    rtl_uString * newStr = x_rtl_uString_new_WithLength( nLength ); // defined in x_rtl_ustring.h
    for (sal_Int32 i = 0; i < nLength; i++)
        newStr->buffer[i] =
            getOneCharConversion(aText[nStartPos+i], Data, Index);
    return OUString( newStr->buffer, nLength);
}

OUString SAL_CALL getWordConversion(const OUString& aText, sal_Int32 nStartPos, sal_Int32 nLength, sal_Bool toSChinese, sal_Int32 nConversionOptions)
{
    sal_Int32 dictLen = 0;
    const sal_Unicode *wordData = getSTC_WordData(dictLen);
    sal_Int32 maxLen = 0;
    const sal_uInt16 *index;
    const sal_uInt16 *entry;
    const sal_Unicode *charData;
    const sal_uInt16 *charIndex;

    if (toSChinese) {
        index = getSTC_WordIndex_T2S(maxLen);
        entry = getSTC_WordEntry_T2S();
        charData = getSTC_CharData_T2S();
        charIndex = getSTC_CharIndex_T2S();
    } else {
        index = getSTC_WordIndex_S2T(maxLen);
        entry = getSTC_WordEntry_S2T();
        if (nConversionOptions & TextConversionOption::USE_CHARACTER_VARIANTS) {
            charData = getSTC_CharData_S2V();
            charIndex = getSTC_CharIndex_S2V();
        } else {
            charData = getSTC_CharData_S2T();
            charIndex = getSTC_CharIndex_S2T();
        }
    }

    if (!wordData || !index || !entry) // no word mapping defined, do char2char conversion.
        return getCharConversion(aText, nStartPos, nLength, toSChinese, nConversionOptions);

    rtl_uString * newStr = x_rtl_uString_new_WithLength( nLength*2 ); // defined in x_rtl_ustring.h
    sal_Int32 currPos = 0, count = 0;
    while (currPos < nLength) {
        sal_Int32 len = nLength - currPos;
        sal_Bool found = sal_False;
        if (len > maxLen)
            len = maxLen;
        for (; len > 0 && ! found; len--) {
            if (index[len+1] - index[len] > 0) {
                sal_Int32 bottom = (sal_Int32) index[len];
                sal_Int32 top = (sal_Int32) index[len+1] - 1;
                OUString word = aText.copy(nStartPos + currPos, len);

                while (bottom <= top && !found) {
                    sal_Int32 current = (top + bottom) / 2;
                    const sal_Int32 result = word.compareTo(wordData + entry[current]);
                    if (result < 0)
                        top = current - 1;
                    else if (result > 0)
                        bottom = current + 1;
                    else {
                        if (toSChinese)   // Traditionary/Simplified conversion,
                            for (current = entry[current]-1; current > 0 && wordData[current-1]; current--);
                        else  // Simplified/Traditionary conversion, forwards search for next word
                            current = entry[current] + word.getLength() + 1;
                        while (wordData[current])
                            newStr->buffer[count++] = wordData[current++];
                        currPos += word.getLength();
                        found = sal_True;
                    }
                }
            }
        }
        if (!found) {
            newStr->buffer[count++] =
                getOneCharConversion(aText[nStartPos+currPos], charData, charIndex);
            currPos++;
        }
    }
    return OUString( newStr->buffer, count);
}

TextConversionResult SAL_CALL
TextConversion_zh::getConversions( const OUString& aText, sal_Int32 nStartPos, sal_Int32 nLength,
    const Locale& aLocale, sal_Int16 nConversionType, sal_Int32 nConversionOptions)
    throw(  RuntimeException, IllegalArgumentException, NoSupportException )
{
    TextConversionResult result;

    result.Candidates.realloc(1);
    result.Candidates[0] = getConversion( aText, nStartPos, nLength, aLocale, nConversionType, nConversionOptions);
    result.Boundary.startPos = nStartPos;
    result.Boundary.endPos = nStartPos + nLength;

    return result;
}

OUString SAL_CALL
TextConversion_zh::getConversion( const OUString& aText, sal_Int32 nStartPos, sal_Int32 nLength,
    const Locale& aLocale, sal_Int16 nConversionType, sal_Int32 nConversionOptions)
    throw(  RuntimeException, IllegalArgumentException, NoSupportException )
{
    if (aLocale.Language.equalsAscii("zh") &&
            ( nConversionType == TextConversionType::TO_SCHINESE ||
            nConversionType == TextConversionType::TO_TCHINESE) ) {

        OUString result;
        sal_Bool c2c = nConversionOptions & TextConversionOption::CHARACTER_BY_CHARACTER;
        sal_Bool toSChinese = nConversionType == TextConversionType::TO_SCHINESE;

        // user dictionary
        if (xCDL.is() && (!c2c || nLength == 1)) {
            Sequence < OUString > conversions;
            try {
                conversions = xCDL->queryConversions(aText, nStartPos, nLength,
                        aLocale, ConversionDictionaryType::SCHINESE_TCHINESE,
                        /*toSChinese ?*/ ConversionDirection_FROM_LEFT /*: ConversionDirection_FROM_RIGHT*/,
                        nConversionOptions);
            }
            catch (...) {
                // catch all exceptions (especially the NoSupportException
                // when there is no user defined dictionary!)
                // to allow querying the system dictionary in the next line
            }
            if (conversions.getLength() > 0)
                result = conversions[0];
        }

        // system word to word dictionary
        if (!c2c && nLength > 1 && result.getLength() == 0)
            result = getWordConversion(aText, nStartPos, nLength, toSChinese, nConversionOptions);

        // system char to char dictionary
        if (result.getLength() == 0)
            result = getCharConversion(aText, nStartPos, nLength, toSChinese, nConversionOptions);

        return result;
    } else
        throw NoSupportException(); // Conversion type is not supported in this service.
}

sal_Bool SAL_CALL
TextConversion_zh::interactiveConversion( const Locale& aLocale, sal_Int16 nTextConversionType, sal_Int32 nTextConversionOptions )
    throw(  RuntimeException, IllegalArgumentException, NoSupportException )
{
    return sal_False;
}

} } } }
