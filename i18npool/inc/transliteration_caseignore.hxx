/*************************************************************************
 *
 *  $RCSfile: transliteration_caseignore.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: er $ $Date: 2002-03-26 17:57:44 $
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
#ifndef TRANSLITERATION_CASE_IGNORE_H
#define TRANSLITERATION_CASE_IGNORE_H

#define TRANSLITERATION_UPPER_LOWER
#include "transliteration_body.hxx"

namespace com { namespace sun { namespace star { namespace i18n {

class Transliteration_caseignore: public Transliteration_body
{
public:
    Transliteration_caseignore();

    // Methods which are shared.
    sal_Int16 SAL_CALL getType() throw(com::sun::star::uno::RuntimeException);

    void SAL_CALL loadModule( TransliterationModules modName, const com::sun::star::lang::Locale& rLocale )
    throw(com::sun::star::uno::RuntimeException);

    com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL transliterateRange(
        const rtl::OUString& str1, const rtl::OUString& str2 )
    throw(com::sun::star::uno::RuntimeException);

    sal_Bool SAL_CALL equals(
        const rtl::OUString& str1, sal_Int32 pos1, sal_Int32 nCount1, sal_Int32& nMatch1,
        const rtl::OUString& src2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2)
    throw(com::sun::star::uno::RuntimeException);

    sal_Int32 SAL_CALL compareSubstring(
        const rtl::OUString& s1, sal_Int32 off1, sal_Int32 len1,
        const rtl::OUString& s2, sal_Int32 off2, sal_Int32 len2)
    throw(com::sun::star::uno::RuntimeException);

    sal_Int32 SAL_CALL compareString(
        const rtl::OUString& s1,
        const rtl::OUString& s2)
    throw(com::sun::star::uno::RuntimeException);

    sal_Unicode SAL_CALL getNextChar(const sal_Unicode *str, sal_Int32& idx, sal_Int32 len, MappingElement& e)
    throw(com::sun::star::uno::RuntimeException);

protected:
    TransliterationModules moduleLoaded;
private:
    sal_Int32 SAL_CALL compare(
        const rtl::OUString& str1, sal_Int32 pos1, sal_Int32 nCount1, sal_Int32& nMatch1,
        const rtl::OUString& str2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2)
    throw(com::sun::star::uno::RuntimeException);
};

class Transliteration_simplecaseignore: public Transliteration_caseignore
{
public:
    Transliteration_simplecaseignore();
};

} } } }

#endif
