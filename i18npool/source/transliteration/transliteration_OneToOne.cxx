/*************************************************************************
 *
 *  $RCSfile: transliteration_OneToOne.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: bustamam $ $Date: 2002-03-26 13:23:20 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
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
// prevent internal compiler error with MSVC6SP3
#include <stl/utility>

#include <transliteration_OneToOne.hxx>

using namespace com::sun::star::uno;
using namespace rtl;

namespace com { namespace sun { namespace star { namespace i18n {

sal_Int16 SAL_CALL transliteration_OneToOne::getType() throw(RuntimeException)
{
    // This type is also defined in com/sun/star/util/TransliterationType.hdl
    return TransliterationType::ONE_TO_ONE;
}

OUString SAL_CALL
transliteration_OneToOne::folding( const OUString& inStr, sal_Int32 startPos,
    sal_Int32 nCount, Sequence< sal_Int32 >& offset) throw(RuntimeException)
{
    throw RuntimeException();
}

sal_Bool SAL_CALL
transliteration_OneToOne::equals( const OUString& str1, sal_Int32 pos1, sal_Int32 nCount1,
    sal_Int32& nMatch1, const OUString& str2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2 )
    throw(RuntimeException)
{
    throw RuntimeException();
}

Sequence< OUString > SAL_CALL
transliteration_OneToOne::transliterateRange( const OUString& str1, const OUString& str2 )
    throw(RuntimeException)
{
    throw RuntimeException();
}


OUString SAL_CALL
transliteration_OneToOne::transliterate( const OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, oneToOneMapping& table )
  throw(RuntimeException)
{
  // Create a string buffer which can hold nCount + 1 characters.
  // The reference count is 0 now.
  rtl_uString * newStr = x_rtl_uString_new_WithLength( nCount ); // defined in x_rtl_ustring.h
  sal_Unicode * dst = newStr->buffer;
  const sal_Unicode * src = inStr.getStr() + startPos;

  // Translation
  while (nCount -- > 0) {
    sal_Unicode c = *src++;
    *dst ++ = table[ c ];
  }
  *dst = (sal_Unicode) 0;

  return OUString( newStr ); // defined in rtl/usrting. The reference count is increased from 0 to 1.
}

OUString SAL_CALL
transliteration_OneToOne::transliterate( const OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, Sequence< sal_Int32 >& offset, oneToOneMapping& table )
  throw(RuntimeException)
{
  // Create a string buffer which can hold nCount + 1 characters.
  // The reference count is 0 now.
  rtl_uString * newStr = x_rtl_uString_new_WithLength( nCount ); // defined in x_rtl_ustring.h
  sal_Unicode * dst = newStr->buffer;
  const sal_Unicode * src = inStr.getStr() + startPos;

  // Allocate nCount length to offset argument.
  offset.realloc( nCount );
  sal_Int32 *p = offset.getArray();
  sal_Int32 position = startPos;

  // Translation
  while (nCount -- > 0) {
    sal_Unicode c = *src++;
    *dst ++ = table[ c ];
    *p ++ = position ++;
  }
  *dst = (sal_Unicode) 0;

  return OUString( newStr ); // defined in rtl/usrting. The reference count is increased from 0 to 1.
}

OUString SAL_CALL
transliteration_OneToOne::transliterate( const OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, Sequence< sal_Int32 >& offset, TransFunc func )
  throw(RuntimeException)
{
  // Create a string buffer which can hold nCount + 1 characters.
  // The reference count is 0 now.
  rtl_uString * newStr = x_rtl_uString_new_WithLength( nCount ); // defined in x_rtl_ustring.h
  sal_Unicode * dst = newStr->buffer;
  const sal_Unicode * src = inStr.getStr() + startPos;

  // Allocate nCount length to offset argument.
  offset.realloc( nCount );
  sal_Int32 *p = offset.getArray();
  sal_Int32 position = startPos;

  // Translation
  while (nCount -- > 0) {
    sal_Unicode c = *src++;
    *dst ++ = func( c );
    *p ++ = position ++;
  }
  *dst = (sal_Unicode) 0;

  return OUString( newStr ); // defined in rtl/usrting. The reference count is increased from 0 to 1.
}

} } } }

