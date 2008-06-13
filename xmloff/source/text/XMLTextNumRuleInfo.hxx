/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: XMLTextNumRuleInfo.hxx,v $
 * $Revision: 1.9 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _XMLOFF_XMLTEXTNUMRULEINFO_HXX
#define _XMLOFF_XMLTEXTNUMRULEINFO_HXX

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/container/XIndexReplace.hpp>

namespace com { namespace sun { namespace star {
    namespace text { class XTextContent; }
} } }
#include <sal/types.h>

class XMLTextListAutoStylePool;

/** information about list and list style for a certain paragraph

    OD 2008-04-24 #refactorlists#
    Complete refactoring of the class and enhancement of the class for lists.
    These changes are considered by method <XMLTextParagraphExport::exportListChange(..)>
*/
class XMLTextNumRuleInfo
{
    // numbering rules instance and its name
    ::com::sun::star::uno::Reference <
                        ::com::sun::star::container::XIndexReplace > mxNumRules;
    ::rtl::OUString     msNumRulesName;

    // paragraph's list attributes
    ::rtl::OUString     msListId;
    sal_Int16           mnListStartValue;
    sal_Int16           mnListLevel;
    sal_Bool            mbIsNumbered;
    sal_Bool            mbIsRestart;

//    // numbering rules' attributes
//    sal_Bool            mbIsOrdered;
//    sal_Bool            mbIsNumRulesNamed;
    // --> OD 2008-05-07 #refactorlists#
    sal_Int16           mnListLevelStartValue;
    // <--

    // --> OD 2006-09-27 #i69627#
    sal_Bool mbOutlineStyleAsNormalListStyle;
    // <--

public:

    XMLTextNumRuleInfo();

    inline XMLTextNumRuleInfo& operator=( const XMLTextNumRuleInfo& rInfo );

    // --> OD 2006-09-27 #i69627#
    void Set( const ::com::sun::star::uno::Reference <
                        ::com::sun::star::text::XTextContent > & rTextContnt,
              const sal_Bool bOutlineStyleAsNormalListStyle,
              const XMLTextListAutoStylePool& rListAutoPool );
    // <--
    inline void Reset();

    inline const ::rtl::OUString& GetNumRulesName() const
    {
        return msNumRulesName;
    }
    inline const ::com::sun::star::uno::Reference <
        ::com::sun::star::container::XIndexReplace >& GetNumRules() const
    {
        return mxNumRules;
    }
//    sal_Bool IsOrdered() const { return mbIsOrdered; }
//    sal_Bool IsNamed() const { return mbIsNumRulesNamed; }
    inline const sal_Int16 GetListLevelStartValue() const
    {
        return mnListLevelStartValue;
    }

    inline const ::rtl::OUString& GetListId() const
    {
        return msListId;
    }

    inline sal_Int16 GetLevel() const
    {
        return mnListLevel;
    }

    inline sal_Bool HasStartValue() const
    {
        return mnListStartValue != -1;
    }
    inline sal_uInt32 GetStartValue() const
    {
        return mnListStartValue;
    }

    inline sal_Bool IsNumbered() const
    {
        return mbIsNumbered;
    }
    inline sal_Bool IsRestart() const
    {
        return mbIsRestart;
    }

    sal_Bool BelongsToSameList( const XMLTextNumRuleInfo& rCmp ) const;

    inline sal_Bool HasSameNumRules( const XMLTextNumRuleInfo& rCmp ) const
    {
//        return ( mbIsNumRulesNamed && rCmp.mbIsNumRulesNamed )
//               ? ( rCmp.msNumRulesName == msNumRulesName )
//               : ( rCmp.mxNumRules == mxNumRules );
        return rCmp.msNumRulesName == msNumRulesName;
    }
};

inline XMLTextNumRuleInfo& XMLTextNumRuleInfo::operator=(
        const XMLTextNumRuleInfo& rInfo )
{
    msNumRulesName = rInfo.msNumRulesName;
    mxNumRules = rInfo.mxNumRules;
    msListId = rInfo.msListId;
    mnListStartValue = rInfo.mnListStartValue;
    mnListLevel = rInfo.mnListLevel;
    mbIsNumbered = rInfo.mbIsNumbered;
    mbIsRestart = rInfo.mbIsRestart;
//    mbIsOrdered = rInfo.mbIsOrdered;
//    mbIsNumRulesNamed = rInfo.mbIsNumRulesNamed;
    // --> OD 2006-09-27 #i69627#
    mbOutlineStyleAsNormalListStyle = rInfo.mbOutlineStyleAsNormalListStyle;
    // <--

    return *this;
}

inline void XMLTextNumRuleInfo::Reset()
{
    mxNumRules = 0;
    msNumRulesName = ::rtl::OUString();
    msListId = ::rtl::OUString();
    mnListStartValue = -1;
    mnListLevel = 0;
    // --> OD 2006-09-27 #i69627#
//    bIsNumbered = bIsOrdered = bIsRestart = bIsNamed = sal_False;
//    mbIsNumbered = mbIsOrdered = mbIsRestart = mbIsNumRulesNamed =
    mbIsNumbered = mbIsRestart =
    mbOutlineStyleAsNormalListStyle = sal_False;
    // <--
}
#endif  //  _XMLOFF_XMLTEXTNUMRULEINFO_HXX
