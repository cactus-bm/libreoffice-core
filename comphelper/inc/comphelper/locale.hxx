/*************************************************************************
 *
 *  $RCSfile: locale.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 15:30:08 $
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

#ifndef _COMPHELPER_LOCALE_HXX_
#define _COMPHELPER_LOCALE_HXX_

//_______________________________________________
// includes

#include <vector>

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

//_______________________________________________
// namespace

namespace comphelper{

//_______________________________________________
// definitions

/** @short  A Locale object represents a specific geographical, political, or cultural region.

    @descr  This Locale class can be used to:
            - provide the different parts of a Locale (Language, Country, Variant)
            - converting it from/to ISO formated string values (like e.g. "en-US")
            - provide some predefined (static) Locale objects
 */
class Locale
{
    //-------------------------------------------
    // const

    public:

        /** @short seperates LANGUAGE and COUNTRY part of an ISO formated Locale. */
        static const sal_Unicode SEPERATOR_LC;

        /** @short seperates COUNTRY and VARIANT part of an ISO formated Locale. */
        static const sal_Unicode SEPERATOR_CV;

        /** @short seperates COUNTRY and VARIANT part of an ISO formated Locale.
            @descr Its true for some linux derivates only :-( */
        static const sal_Unicode SEPERATOR_CV_LINUX;

        /** @short some predefined Locale objects. */
        static const Locale EN_US();
        static const Locale EN();
        static const Locale DE_DE();
        static const Locale DE_CH();
        static const Locale DE_AT();
        static const Locale AR();
        static const Locale CA();
        static const Locale CS();
        static const Locale DA();
        static const Locale EL();
        static const Locale ES();
        static const Locale FI();
        static const Locale FR();
        static const Locale HE();
        static const Locale HI_IN();
        static const Locale HU();
        static const Locale IT();
        static const Locale JA();
        static const Locale KO();
        static const Locale NL();
        static const Locale PL();
        static const Locale PT();
        static const Locale PT_BR();
        static const Locale RU();
        static const Locale SK();
        static const Locale SL();
        static const Locale SV();
        static const Locale TH();
        static const Locale TR();
        static const Locale X_DEFAULT();
        static const Locale X_COMMENT();
        static const Locale X_TRANSLATE();
        static const Locale X_NOTRANSLATE();
        static const Locale ZH_CN();
        static const Locale ZH_TW();

    //-------------------------------------------
    // types

    public:

        /** @short will be throw during convertion, if a Locale cant be interpreted. */
        struct MalFormedLocaleException
        {
            public:
                ::rtl::OUString Message;

                MalFormedLocaleException()
                {}

                MalFormedLocaleException(const ::rtl::OUString& sMessage)
                    : Message(sMessage)
                {}
        };

    //-------------------------------------------
    // member

    private :

        //---------------------------------------
        /** @short  must be a valid ISO Language Code.

            @descr  These codes are the lower-case two-letter codes as defined by ISO-639.
                    You can find a full list of these codes at a number of sites, such as:
                    <BR><a href ="http://www.ics.uci.edu/pub/ietf/http/related/iso639.txt">
                    http://www.ics.uci.edu/pub/ietf/http/related/iso639.txt</a>
         */
        ::rtl::OUString m_sLanguage;

        //---------------------------------------
        /** @short  must be a valid ISO Country Code.
            @descr  These codes are the upper-case two-letter codes as defined by ISO-3166.
                    You can find a full list of these codes at a number of sites, such as:
                    <BR><a href="http://www.chemie.fu-berlin.de/diverse/doc/ISO_3166.html">
                    http://www.chemie.fu-berlin.de/diverse/doc/ISO_3166.html</a>
         */
        ::rtl::OUString m_sCountry;

        //---------------------------------------
        /** @short  Variant codes are vendor and browser-specific.
            @descr  For example, use WIN for Windows, MAC for Macintosh, and POSIX for POSIX.
                    Where there are two variants, separate them with an underscore, and
                    put the most important one first. For example, a Traditional Spanish collation
                    might be referenced, with "ES", "ES", "Traditional_WIN".
         */
        ::rtl::OUString m_sVariant;

    //-------------------------------------------
    // interface

    public :

        //---------------------------------------
        /** @short  needed by outside users!

            @descr  Otherwise it wouldnt be possible to use
                    any instance of such Locale static ...
         */
        Locale();

        //---------------------------------------
        /** @short      construct a Locale from an ISO formated string value.

            @seealso    fromISO()

            @param      sISO
                        an ISO formated string value, which can be parsed and
                        tokenized into a Lamnguage, Country and Variant part.

            @throw      MalFormedLocaleException
                        if conversion failed.
          */
        Locale(const ::rtl::OUString& sISO)
            throw(MalFormedLocaleException);

        //---------------------------------------
        /** @short      construct a Locale from language, country and variant.

            @seealso    setLanguage()
            @seealso    setCountry()
            @seealso    setVariant()

            @param      sLanguage
                        lowercase two-letter ISO-639 code.

            @param      sCountry
                        uppercase two-letter ISO-3166 code.

            @param      sVariant
                        vendor and browser specific code.
          */
        Locale(const ::rtl::OUString& sLanguage                   ,
               const ::rtl::OUString& sCountry                    ,
               const ::rtl::OUString& sVariant = ::rtl::OUString());

        //---------------------------------------
        /** @short  copy constructor.

            @param  aCopy
                    the copy object.
         */
        Locale(const Locale& aCopy);

        //---------------------------------------
        /** @short  returns the language code for this locale.

            @descr  That will either be the empty string or
                    a lowercase ISO 639 code.

            @return [string]
                    the language code.
         */
        ::rtl::OUString getLanguage() const;

        //---------------------------------------
        /** @short  returns the country/region code for this locale.

            @descr  That will either be the empty string or an
                    upercase ISO 3166 2-letter code.

            @return [string]
                    the country code.
         */
        ::rtl::OUString getCountry() const;

        //---------------------------------------
        /** @short  returns the variant code for this locale.

            @return [string]
                    the variant code.
         */
        ::rtl::OUString getVariant() const;

        //---------------------------------------
        /** @short  set the new language code for this locale.

            @descr  That will either be the empty string or
                    a lowercase ISO 639 code.

            @param  sLanguage
                    the language code.
         */
        void setLanguage(const ::rtl::OUString& sLanguage);

        //---------------------------------------
        /** @short  set the new country/region code for this locale.

            @descr  That will either be the empty string or an
                    upercase ISO 3166 2-letter code.

            @param  sCountry
                    the country code.
         */
        void setCountry(const ::rtl::OUString& sCountry);

        //---------------------------------------
        /** @short  set the new variant code for this locale.

            @param  sVariant
                    the variant code.
         */
        void setVariant(const ::rtl::OUString& sVariant);

        //---------------------------------------
        /** @short      take over new Locale informations.

            @seealso    Locale(const ::rtl::OUString& sISO)

            @param      sISO
                        an ISO formated string value, which can be parsed and
                        tokenized into a Lamnguage, Country and Variant part.
                        e.g. "en-US" or "en-US_WIN"

            @throw      MalFormedLocaleException
                        if conversion failed.
          */
        void fromISO(const ::rtl::OUString& sISO)
            throw(MalFormedLocaleException);

        //---------------------------------------
        /** @short  converts this Locale to an ISO formated string value.

            @descr  The different parts of this Locale will be assempled
                    e.g. to "en-US" or "en-US_WIN"

            @return [string]
                    the ISO formated string.
          */
        ::rtl::OUString toISO() const;

        //---------------------------------------
        /** @short  check, if two Locale objects are equals.

            @descr  All parts of a Locale (means Language, Country and Variant)
                    will be checked.

            @param  aComparable
                    the Locale object for compare.

            @return [boolean]
                    TRUE if both objects uses the same values for
                    Language, Country and Variant.
         */
        sal_Bool equals(const Locale& aComparable) const;

        //---------------------------------------
        /** @short  check, if two Locale objects
                    uses the same language.

            @descr  The Country and Variant parts of a Locale
                    wont be checked here.

            @return [boolean]
                    TRUE if both objects uses the same
                    Language value.
         */
        sal_Bool similar(const Locale& aComparable) const;

        //---------------------------------------
        /** @short      search for an equal or at least for a similar
                        Locale in a list of possible ones.

            @descr      First it searches for a Locale, which is equals
                        to the reference Locale.
                        (means: same Language, Country, Variant)

                        If the reference Locale couldnt be located, it will
                        tried again - but we are checking for "similar" Locales then.
                        (means: same Language)

                        If no similar Locale could be located, we search
                        for a Locale "en-US" inside the given Locale list.

                        If "en-US" could not be located, we search for
                        a Locale "en" inside the given list.

                        Further fallbacks are not implemented :-)

            @seealso    equals()
            @seealso    similar()

            @param      lISOList
                        the list of possible Locales
                        (as formated ISO strings).

            @param      sReferenceISO
                        the reference Locale, which should be searched
                        if its equals or similar to any Locale inside
                        the provided Locale list.

            @return     An iterator, which points to the found element
                        inside the given Locale list.
                        If no matching Locale could be found, it points
                        to the end of the list.

            @throw      [MalFormedLocaleException]
                        if at least one ISO formated string couldnt
                        be converted to a valid Locale Object.
         */
        static ::std::vector< ::rtl::OUString >::const_iterator getFallback(const ::std::vector< ::rtl::OUString >& lISOList     ,
                                                                            const ::rtl::OUString&                  sReferenceISO)
            throw(MalFormedLocaleException);

        //---------------------------------------
        /** @short      search for the next possible fallback locale.

            @descr      Instead of getFallback(vector<>, string) this method
                        uses the given locale and decide by using an algorithm
                        which locale can be the next possible one.

                        Algorithm:
                        - if locale has country return language only
                        - if locale different "en-US" return "en-US"
                        - if locale "en-US" return "en"

            @param      aLocale [in/out]!
                        the incoming value will be used to start
                        search for a possible fallback ...
                        and in case such fallback was found this parameter
                        will be used for return too.

            @return     TRUE if the parameter aLocale contains a new fallback value;
                        FALSE otherwise.
         */
        static sal_Bool getFallback(Locale& aLocale);

        //---------------------------------------
        /** @short      assign elements of another locale
                        to this instance.

            @param      rCopy
                        another locale object.
         */
        void operator=(const Locale& rCopy);

        //---------------------------------------
        /** @short      check if two Locale objects are equals.

            @seealso    equals()

            @param      aComparable
                        the Locale object for compare.

            @return     [boolean]
                        TRUE if both objects uses the same values for
                        Language, Country and Variant.
         */
        sal_Bool operator==(const Locale& aComparable) const;

        //---------------------------------------
        /** @short  check if two Locale objects are different.

            @param  aComparable
                    the Locale object for compare.

            @return [boolean]
                    TRUE if at least one part of such Locale
                    isnt the same.
         */
        sal_Bool operator!=(const Locale& aComparable) const;
};

} // namespace salhelper

#endif // _COMPHELPER_LOCALE_HXX_
