/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fullwidthToHalfwidth.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 09:24:57 $
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

// prevent internal compiler error with MSVC6SP3
#include <utility>

#include <i18nutil/widthfolding.hxx>
#define TRANSLITERATION_fullwidthToHalfwidth
#include <transliteration_OneToOne.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace rtl;

namespace com { namespace sun { namespace star { namespace i18n {

fullwidthToHalfwidth::fullwidthToHalfwidth()
{
    func = (TransFunc) 0;
    table = &widthfolding::getfull2halfTable();
    transliterationName = "fullwidthToHalfwidth";
    implementationName = "com.sun.star.i18n.Transliteration.FULLWIDTH_HALFWIDTH";
}

/**
 * Transliterate fullwidth to halfwidth.
 * The output is a reference of OUString. You MUST delete this object when you do not need to use it any more
 * The output string contains a transliterated string only, not whole string.
 */
OUString SAL_CALL
fullwidthToHalfwidth::transliterate( const OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, Sequence< sal_Int32 >& offset )
  throw(RuntimeException)
{
    // Decomposition: GA --> KA + voice-mark
    const OUString& newStr = widthfolding::decompose_ja_voiced_sound_marks (inStr, startPos, nCount, offset, useOffset);

    // One to One mapping
    useOffset = sal_False;
    const OUString &tmp = transliteration_OneToOne::transliterate( newStr, 0, newStr.getLength(), offset);
    useOffset = sal_True;
    return tmp;
}

sal_Unicode SAL_CALL
fullwidthToHalfwidth::transliterateChar2Char( sal_Unicode inChar)
  throw(RuntimeException, MultipleCharsOutputException)
{
    sal_Unicode newChar = widthfolding::decompose_ja_voiced_sound_marksChar2Char (inChar);
    if (newChar == 0xFFFF)
        throw MultipleCharsOutputException();
    return transliteration_OneToOne::transliterateChar2Char(inChar);
}

} } } }

