/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: collator_unicode.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-10-17 15:43:36 $
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

#include <rtl/ustrbuf.hxx>
#include <collator_unicode.hxx>
#include <com/sun/star/i18n/CollatorOptions.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::rtl;

namespace com { namespace sun { namespace star { namespace i18n {

Collator_Unicode::Collator_Unicode()
{
    implementationName = "com.sun.star.i18n.Collator_Unicode";
    collator = NULL;
    rulesImage = NULL;
}

Collator_Unicode::~Collator_Unicode()
{
    if (collator) delete collator;
}

sal_Int32 SAL_CALL
Collator_Unicode::compareSubstring( const OUString& str1, sal_Int32 off1, sal_Int32 len1,
    const OUString& str2, sal_Int32 off2, sal_Int32 len2) throw(RuntimeException)
{
    return collator->compare(str1.getStr() + off1, len1, str2.getStr() + off2, len2);
}

sal_Int32 SAL_CALL
Collator_Unicode::compareString( const OUString& str1, const OUString& str2) throw(RuntimeException)
{
    return collator->compare(str1.getStr(), str2.getStr());
}

sal_Int32 SAL_CALL
Collator_Unicode::loadCollatorAlgorithm(const OUString& rAlgorithm, const lang::Locale& rLocale, sal_Int32 options)
    throw(RuntimeException)
{
    if (!collator) {
        // load ICU collator
        UErrorCode status = U_ZERO_ERROR;
        if (rulesImage) {
            collator = new RuleBasedCollator(rulesImage, status);
        } else {
           // load ICU collator
            /** ICU collators are loaded using a locale only.
                ICU uses Variant as collation algorithm name (like de__PHONEBOOK
                locale), note the empty territory (Country) designator in this special
                case here. The icu::Locale contructor changes the algorithm name to
                uppercase itself, so we don't have to bother with that.
            */
           icu::Locale icuLocale(
                   OUStringToOString(rLocale.Language, RTL_TEXTENCODING_ASCII_US).getStr(),
                   OUStringToOString(rLocale.Country, RTL_TEXTENCODING_ASCII_US).getStr(),
                   OUStringToOString(rAlgorithm, RTL_TEXTENCODING_ASCII_US).getStr());

            collator = (RuleBasedCollator*) icu::Collator::createInstance(icuLocale, status);
        }
        if (! U_SUCCESS(status))
            throw RuntimeException();
    }

    if (options & CollatorOptions::CollatorOptions_IGNORE_CASE_ACCENT)
        collator->setStrength(Collator::PRIMARY);
    else if (options & CollatorOptions::CollatorOptions_IGNORE_CASE)
        collator->setStrength(Collator::SECONDARY);
    else
        collator->setStrength(Collator::TERTIARY);

    return(0);
}


OUString SAL_CALL
Collator_Unicode::getImplementationName() throw( RuntimeException )
{
    return OUString::createFromAscii(implementationName);
}

sal_Bool SAL_CALL
Collator_Unicode::supportsService(const rtl::OUString& rServiceName) throw( RuntimeException )
{
    return !rServiceName.compareToAscii(implementationName);
}

Sequence< OUString > SAL_CALL
Collator_Unicode::getSupportedServiceNames() throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    aRet[0] = OUString::createFromAscii(implementationName);
    return aRet;
}

} } } }
