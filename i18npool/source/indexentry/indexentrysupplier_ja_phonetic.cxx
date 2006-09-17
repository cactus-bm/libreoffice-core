/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: indexentrysupplier_ja_phonetic.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 09:19:11 $
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
#include "precompiled_i18npool.hxx"

#include <indexentrysupplier_ja_phonetic.hxx>
#include <data/indexdata_alphanumeric.h>
#include <data/indexdata_ja_phonetic.h>
#include <string.h>

using namespace ::rtl;

namespace com { namespace sun { namespace star { namespace i18n {

OUString SAL_CALL IndexEntrySupplier_ja_phonetic::getIndexCharacter( const OUString& rIndexEntry,
    const lang::Locale& /*rLocale*/, const OUString& /*rSortAlgorithm*/ )
    throw (com::sun::star::uno::RuntimeException)
{
    sal_Unicode ch=rIndexEntry.toChar();
    sal_uInt16 first = idx[ ch >> 8 ];
    if (first == 0xFFFF) {
        // using alphanumeric index for non-define stirng
        return OUString(&idxStr[(ch & 0xFF00) ? 0 : ch], 1);
    } else {
        sal_Unicode *idx2 = strstr(implementationName, "syllable") ? syllable : consonant;
        return OUString(&idx2[ first + (ch & 0xff) ], 1);
    }
}

OUString SAL_CALL IndexEntrySupplier_ja_phonetic::getIndexKey( const OUString& IndexEntry,
    const OUString& PhoneticEntry, const lang::Locale& rLocale )
    throw (com::sun::star::uno::RuntimeException)
{
    return getIndexCharacter( PhoneticEntry.getLength() > 0 ? PhoneticEntry : IndexEntry, rLocale, OUString());
}

sal_Int16 SAL_CALL IndexEntrySupplier_ja_phonetic::compareIndexEntry(
    const OUString& IndexEntry1, const OUString& PhoneticEntry1, const lang::Locale& rLocale1,
    const OUString& IndexEntry2, const OUString& PhoneticEntry2, const lang::Locale& rLocale2 )
    throw (com::sun::star::uno::RuntimeException)
{
    sal_Int16 result = sal::static_int_cast<sal_Int16>( collator->compareString(
        IndexEntrySupplier_ja_phonetic::getIndexKey(IndexEntry1, PhoneticEntry1, rLocale1),
        IndexEntrySupplier_ja_phonetic::getIndexKey(IndexEntry2, PhoneticEntry2, rLocale2)) );

    if (result == 0)
        return IndexEntrySupplier_Common::compareIndexEntry(
                    IndexEntry1, PhoneticEntry1, rLocale1,
                    IndexEntry2, PhoneticEntry2, rLocale2);
    return result;
}

static sal_Char first[] = "ja_phonetic (alphanumeric first)";
sal_Bool SAL_CALL IndexEntrySupplier_ja_phonetic_alphanumeric_first_by_syllable::loadAlgorithm(
    const com::sun::star::lang::Locale& rLocale, const OUString& /*SortAlgorithm*/,
    sal_Int32 collatorOptions ) throw (com::sun::star::uno::RuntimeException)
{
    return collator->loadCollatorAlgorithm(OUString::createFromAscii(first), rLocale, collatorOptions) == 0;
}
sal_Bool SAL_CALL IndexEntrySupplier_ja_phonetic_alphanumeric_first_by_consonant::loadAlgorithm(
    const com::sun::star::lang::Locale& rLocale, const OUString& /*SortAlgorithm*/,
    sal_Int32 collatorOptions ) throw (com::sun::star::uno::RuntimeException)
{
    return collator->loadCollatorAlgorithm(OUString::createFromAscii(first), rLocale, collatorOptions) == 0;
}

static sal_Char last[] = "ja_phonetic (alphanumeric last)";
sal_Bool SAL_CALL IndexEntrySupplier_ja_phonetic_alphanumeric_last_by_syllable::loadAlgorithm(
    const com::sun::star::lang::Locale& rLocale, const OUString& /*SortAlgorithm*/,
    sal_Int32 collatorOptions ) throw (com::sun::star::uno::RuntimeException)
{
    return collator->loadCollatorAlgorithm(OUString::createFromAscii(last), rLocale, collatorOptions) == 0;
}
sal_Bool SAL_CALL IndexEntrySupplier_ja_phonetic_alphanumeric_last_by_consonant::loadAlgorithm(
    const com::sun::star::lang::Locale& rLocale, const OUString& /*SortAlgorithm*/,
    sal_Int32 collatorOptions ) throw (com::sun::star::uno::RuntimeException)
{
    return collator->loadCollatorAlgorithm(OUString::createFromAscii(last), rLocale, collatorOptions) == 0;
}

} } } }
