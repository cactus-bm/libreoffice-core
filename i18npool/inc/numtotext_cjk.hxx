/*************************************************************************
 *
 *  $RCSfile: numtotext_cjk.hxx,v $
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
#ifndef _L10N_TRANSLITERATION_NUMTOTEXT_CJK_HXX_
#define _L10N_TRANSLITERATION_NUMTOTEXT_CJK_HXX_

#include <transliteration_Numeric.hxx>

namespace com { namespace sun { namespace star { namespace i18n {

struct NumberMultiplier {
    sal_Int16 power;
    sal_Unicode multiplierChar;
};

class NumToText_CJK : public transliteration_Numeric {
public:
    NumToText_CJK();

    virtual rtl::OUString SAL_CALL transliterate( const ::rtl::OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, com::sun::star::uno::Sequence< sal_Int32 >& offset ) throw(com::sun::star::uno::RuntimeException);

protected:
    const NumberMultiplier *numberMultiplier;
    const sal_Unicode *numberChar;
    sal_Int16 numberFlag;

private:
    sal_Bool SAL_CALL numberMaker(const sal_Unicode *str, sal_Int32 begin, sal_Int32 len,
        sal_Unicode *dst, sal_Int32& count, sal_Unicode numberChar, sal_Int32** offset);
};

#define TRANSLITERATION_NUMTOTEXT_CJK( name ) \
class name : public NumToText_CJK \
{ \
public: \
    name (); \
};

#ifdef TRANSLITERATION_ALL
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextLower_zh_CN)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextUpper_zh_CN)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextLower_zh_TW)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextUpper_zh_TW)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextInformalLower_ko)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextInformalUpper_ko)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextInformalHangul_ko)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextFormalLower_ko)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextFormalUpper_ko)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextFormalHangul_ko)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextKanjiLongModern_ja_JP)
TRANSLITERATION_NUMTOTEXT_CJK(NumToTextKanjiLongTraditional_ja_JP)
#endif
#undef TRANSLITERATION_NUMTOTEXT

} } } }

#endif // _L10N_TRANSLITERATION_NUMTOTEXT_HXX_
