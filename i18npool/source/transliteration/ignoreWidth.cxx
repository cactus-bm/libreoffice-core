/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ignoreWidth.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 09:29:34 $
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

#define TRANSLITERATION_Width
#include <transliteration_Ignore.hxx>
#define TRANSLITERATION_fullwidthToHalfwidth
#define TRANSLITERATION_halfwidthToFullwidth
#include <transliteration_OneToOne.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace rtl;

namespace com { namespace sun { namespace star { namespace i18n {

OUString SAL_CALL
ignoreWidth::folding( const OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, Sequence< sal_Int32 >& offset )
  throw(RuntimeException)
{
    halfwidthToFullwidth t1;
    return t1.transliterate(inStr, startPos, nCount, offset);
}


Sequence< OUString > SAL_CALL
ignoreWidth::transliterateRange( const OUString& str1, const OUString& str2 )
  throw(RuntimeException)
{
    halfwidthToFullwidth t1;
    fullwidthToHalfwidth t2;

    return transliteration_Ignore::transliterateRange(str1, str2, t1, t2);
}

sal_Unicode SAL_CALL
ignoreWidth::transliterateChar2Char( sal_Unicode inChar) throw(RuntimeException, MultipleCharsOutputException)
{
    halfwidthToFullwidth t1;
    return t1.transliterateChar2Char(inChar);
}

} } } }
