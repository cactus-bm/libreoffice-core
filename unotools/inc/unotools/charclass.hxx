/*************************************************************************
 *
 *  $RCSfile: charclass.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: fs $ $Date: 2001-05-17 13:04:24 $
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

#ifndef _UNOTOOLS_CHARCLASS_HXX
#define _UNOTOOLS_CHARCLASS_HXX

#include <ctype.h>      // isdigit(), isalpha()

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

#ifndef _COM_SUN_STAR_I18N_KCHARACTERTYPE_HPP_
#include <com/sun/star/i18n/KCharacterType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_KPARSETOKENS_HPP_
#include <com/sun/star/i18n/KParseTokens.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_KPARSETYPE_HPP_
#include <com/sun/star/i18n/KParseType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_PARSERESULT_HPP_
#include <com/sun/star/i18n/ParseResult.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_XCHARACTERCLASSIFICATION_HPP_
#include <com/sun/star/i18n/XCharacterClassification.hpp>
#endif

class String;
namespace com { namespace sun { namespace star {
    namespace lang {
        class XMultiServiceFactory;
    }
}}}

const sal_Int32 nCharClassAlphaType =
    ::com::sun::star::i18n::KCharacterType::UPPER |
    ::com::sun::star::i18n::KCharacterType::LOWER |
    ::com::sun::star::i18n::KCharacterType::TITLE_CASE;

const sal_Int32 nCharClassAlphaTypeMask =
    nCharClassAlphaType |
    ::com::sun::star::i18n::KCharacterType::PRINTABLE |
    ::com::sun::star::i18n::KCharacterType::BASE_FORM;

const sal_Int32 nCharClassLetterType =
    nCharClassAlphaType |
    ::com::sun::star::i18n::KCharacterType::LETTER;

const sal_Int32 nCharClassLetterTypeMask =
    nCharClassAlphaTypeMask |
    ::com::sun::star::i18n::KCharacterType::LETTER;

const sal_Int32 nCharClassNumericType =
    ::com::sun::star::i18n::KCharacterType::DIGIT;

const sal_Int32 nCharClassNumericTypeMask =
    nCharClassNumericType |
    ::com::sun::star::i18n::KCharacterType::PRINTABLE |
    ::com::sun::star::i18n::KCharacterType::BASE_FORM;


class CharClass
{
    ::com::sun::star::lang::Locale  aLocale;
    ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XCharacterClassification >    xCC;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xSMgr;

                                // not implemented, prevent usage
                                CharClass( const CharClass& );
            CharClass&          operator=( const CharClass& );

                                // instantiate component somehow
            void                getComponentInstance();

public:
    /// Preferred ctor with service manager specified
                                CharClass(
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & xSF,
                                    const ::com::sun::star::lang::Locale& rLocale
                                    );

    /// Depricated ctor, tries to get a process service manager or to load the
    /// library directly.
                                CharClass(
                                    const ::com::sun::star::lang::Locale& rLocale
                                    );

                                ~CharClass();

    /// set a new Locale
            void                setLocale( const ::com::sun::star::lang::Locale& rLocale );

    /// get current Locale
    const ::com::sun::star::lang::Locale& getLocale() const { return aLocale; }


    /// isdigit() on ascii values
    static  inline  sal_Bool    isAsciiDigit( sal_Unicode c )
        { return c < 128 ? (isdigit( (unsigned char) c ) != 0) : 0; }

    /// isalpha() on ascii values
    static  inline  sal_Bool    isAsciiAlpha( sal_Unicode c )
        { return c < 128 ? (isalpha( (unsigned char) c ) != 0) : 0; }

    /// isalnum() on ascii values
    static  inline  sal_Bool    isAsciiAlphaNumeric( sal_Unicode c )
        { return c < 128 ? (isalnum( (unsigned char) c ) != 0) : 0; }

    /// isdigit() on ascii values of entire string
    static  sal_Bool            isAsciiNumeric( const String& rStr );

    /// isalpha() on ascii values of entire string
    static  sal_Bool            isAsciiAlpha( const String& rStr );

    /// isalnum() on ascii values of entire string
    static  sal_Bool            isAsciiAlphaNumeric( const String& rStr );

    /// whether type is pure alpha or not, e.g. return of getStringType
    static  inline  sal_Bool    isAlphaType( sal_Int32 nType )
        {
            return ((nType & nCharClassAlphaType) != 0) &&
                ((nType & ~(nCharClassAlphaTypeMask)) == 0);
        }

    /// whether type is pure numeric or not, e.g. return of getStringType
    static  inline  sal_Bool    isNumericType( sal_Int32 nType )
        {
            return ((nType & nCharClassNumericType) != 0) &&
                ((nType & ~(nCharClassNumericTypeMask)) == 0);
        }

    /// whether type is pure alphanumeric or not, e.g. return of getStringType
    static  inline  sal_Bool    isAlphaNumericType( sal_Int32 nType )
        {
            return ((nType & (nCharClassAlphaType |
                nCharClassNumericType)) != 0) &&
                ((nType & ~(nCharClassAlphaTypeMask |
                nCharClassNumericTypeMask)) == 0);
        }

    /// whether type is pure letter or not, e.g. return of getStringType
    static  inline  sal_Bool    isLetterType( sal_Int32 nType )
        {
            return ((nType & nCharClassLetterType) != 0) &&
                ((nType & ~(nCharClassLetterTypeMask)) == 0);
        }

    /// whether type is pure letternumeric or not, e.g. return of getStringType
    static  inline  sal_Bool    isLetterNumericType( sal_Int32 nType )
        {
            return ((nType & (nCharClassLetterType |
                nCharClassNumericType)) != 0) &&
                ((nType & ~(nCharClassLetterTypeMask |
                nCharClassNumericTypeMask)) == 0);
        }


    // Wrapper implementations of class CharacterClassification

            String              toUpper( const String& rStr, xub_StrLen nPos, xub_StrLen nCount ) const;
            String              toLower( const String& rStr, xub_StrLen nPos, xub_StrLen nCount ) const;
            String              toTitle( const String& rStr, xub_StrLen nPos, xub_StrLen nCount ) const;

            ::rtl::OUString     toUpper_rtl( const ::rtl::OUString& rStr, sal_Int32 nPos, sal_Int32 nCount ) const;
            ::rtl::OUString     toLower_rtl( const ::rtl::OUString& rStr, sal_Int32 nPos, sal_Int32 nCount ) const;
                // use the postfix because compilers could get confused by the both similar versions
                // (me thinks they shouldn't, but in fact MSCV 6 does)

            sal_Int16           getType( const String& rStr, xub_StrLen nPos ) const;
            sal_Int16           getCharacterDirection( const String& rStr, xub_StrLen nPos ) const;
            sal_Int16           getScript( const String& rStr, xub_StrLen nPos ) const;
            sal_Int32           getCharacterType( const String& rStr, xub_StrLen nPos ) const;
            sal_Int32           getStringType( const String& rStr, xub_StrLen nPos, xub_StrLen nCount ) const;

    ::com::sun::star::i18n::ParseResult parseAnyToken(
                                    const String& rStr,
                                    sal_Int32 nPos,
                                    sal_Int32 nStartCharFlags,
                                    const String& userDefinedCharactersStart,
                                    sal_Int32 nContCharFlags,
                                    const String& userDefinedCharactersCont ) const;

    ::com::sun::star::i18n::ParseResult parsePredefinedToken(
                                    sal_Int32 nTokenType,
                                    const String& rStr,
                                    sal_Int32 nPos,
                                    sal_Int32 nStartCharFlags,
                                    const String& userDefinedCharactersStart,
                                    sal_Int32 nContCharFlags,
                                    const String& userDefinedCharactersCont ) const;


    // Functionality of class International methods

            sal_Bool            isAlpha( const String& rStr, xub_StrLen nPos ) const;
            sal_Bool            isLetter( const String& rStr, xub_StrLen nPos ) const;
            sal_Bool            isDigit( const String& rStr, xub_StrLen nPos ) const;
            sal_Bool            isAlphaNumeric( const String& rStr, xub_StrLen nPos ) const;
            sal_Bool            isLetterNumeric( const String& rStr, xub_StrLen nPos ) const;
            sal_Bool            isAlpha( const String& rStr ) const;
            sal_Bool            isLetter( const String& rStr ) const;
            sal_Bool            isNumeric( const String& rStr ) const;
            sal_Bool            isAlphaNumeric( const String& rStr ) const;
            sal_Bool            isLetterNumeric( const String& rStr ) const;

            void                toUpper( String& rStr ) const
                                    { rStr = toUpper( rStr, 0, rStr.Len() ); }
            void                toLower( String& rStr ) const
                                    { rStr = toLower( rStr, 0, rStr.Len() ); }
    inline  String              upper( const String& rStr ) const
                                    { return toUpper( rStr, 0, rStr.Len() ); }
    inline  String              lower( const String& rStr ) const
                                    { return toLower( rStr, 0, rStr.Len() ); }

        const ::rtl::OUString&  toUpper_rtl( ::rtl::OUString& _rStr ) const { return _rStr = toUpper_rtl( _rStr, 0, _rStr.getLength() ); }
        const ::rtl::OUString&  toLower_rtl( ::rtl::OUString& _rStr ) const { return _rStr = toLower_rtl( _rStr, 0, _rStr.getLength() ); }
};



#endif // _UNOTOOLS_CHARCLASS_HXX
