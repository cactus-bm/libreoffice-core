/*************************************************************************
 *
 *  $RCSfile: cjkoptions.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: dg $ $Date: 2001-09-26 15:41:15 $
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

#pragma hdrstop

#ifndef _SVTOOLS_CJKOPTIONS_HXX
#include "cjkoptions.hxx"
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::rtl;

#define C2U(cChar) OUString::createFromAscii(cChar)
/* -----------------------------10.04.01 12:39--------------------------------

 ---------------------------------------------------------------------------*/
class SvtCJKOptions_Impl : public utl::ConfigItem
{
    static Sequence<OUString> aPropertyNames;

    sal_Bool        bIsLoaded;
    sal_Bool        bCJKFont;
    sal_Bool        bVerticalText;
    sal_Bool        bAsianTypography;
    sal_Bool        bJapaneseFind;
    sal_Bool        bRuby;
    sal_Bool        bChangeCaseMap;
    sal_Bool        bDoubleLines;
    sal_Bool        bEmphasisMarks;
    sal_Bool        bVerticalCallOut;

public:
    SvtCJKOptions_Impl();
    ~SvtCJKOptions_Impl();

    virtual void    Notify( const com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
    virtual void    Commit();
    void            Load();

    sal_Bool IsLoaded()                         { return bIsLoaded;         }

    sal_Bool IsCJKFontEnabled() const           { return bCJKFont;          }
    sal_Bool IsVerticalTextEnabled() const      { return bVerticalText;     }
    sal_Bool IsAsianTypographyEnabled() const   { return bAsianTypography;  }
    sal_Bool IsJapaneseFindEnabled() const      { return bJapaneseFind;     }
    sal_Bool IsRubyEnabled() const              { return bRuby;             }
    sal_Bool IsChangeCaseMapEnabled() const     { return bChangeCaseMap;    }
    sal_Bool IsDoubleLinesEnabled() const       { return bDoubleLines;      }
    sal_Bool IsEmphasisMarksEnabled() const     { return bEmphasisMarks;    }
    sal_Bool IsVerticalCallOutEnabled() const   { return bVerticalCallOut;  }

    sal_Bool IsAnyEnabled() const {
        return  bCJKFont||bVerticalText||bAsianTypography||bJapaneseFind||
                bRuby||bChangeCaseMap||bDoubleLines||bEmphasisMarks||bVerticalCallOut;   }
    void    SetAll(sal_Bool bSet);
};
/*-- 10.04.01 12:41:57---------------------------------------------------

  -----------------------------------------------------------------------*/
Sequence<OUString> SvtCJKOptions_Impl::aPropertyNames;

SvtCJKOptions_Impl::SvtCJKOptions_Impl() :
    utl::ConfigItem(C2U("Office.Common/I18N/CJK")),
    bIsLoaded(sal_False),
    bCJKFont(sal_True),
    bVerticalText(sal_True),
    bAsianTypography(sal_True),
    bJapaneseFind(sal_True),
    bRuby(sal_True),
    bChangeCaseMap(sal_True),
    bDoubleLines(sal_True),
    bEmphasisMarks(sal_True),
    bVerticalCallOut(sal_True)
{
}
/*-- 10.04.01 12:41:57---------------------------------------------------

  -----------------------------------------------------------------------*/
SvtCJKOptions_Impl::~SvtCJKOptions_Impl()
{
}
/* -----------------------------20.04.01 14:34--------------------------------

 ---------------------------------------------------------------------------*/
void    SvtCJKOptions_Impl::SetAll(sal_Bool bSet)
{
    bCJKFont = bVerticalText = bAsianTypography = bJapaneseFind =
            bRuby = bChangeCaseMap = bDoubleLines = bEmphasisMarks = bVerticalCallOut = bSet;
    SetModified();
}
/*-- 10.04.01 12:41:56---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtCJKOptions_Impl::Load()
{
    if(!aPropertyNames.getLength())
    {
        aPropertyNames.realloc(9);
        OUString* pNames = aPropertyNames.getArray();

        pNames[0] = C2U("CJKFont");
        pNames[1] = C2U("VerticalText");
        pNames[2] = C2U("AsianTypography");
        pNames[3] = C2U("JapaneseFind");
        pNames[4] = C2U("Ruby");
        pNames[5] = C2U("ChangeCaseMap");
        pNames[6] = C2U("DoubleLines");
        pNames[7] = C2U("EmphasisMarks");
        pNames[8] = C2U("VerticalCallOut");

        EnableNotification( aPropertyNames );
    }
    Sequence< Any > aValues = GetProperties(aPropertyNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT( aValues.getLength() == aPropertyNames.getLength(), "GetProperties failed" );
    if ( aValues.getLength() == aPropertyNames.getLength() )
    {
        for ( int nProp = 0; nProp < aPropertyNames.getLength(); nProp++ )
        {
            if( pValues[nProp].hasValue() )
            {
                sal_Bool bValue = *(sal_Bool*)pValues[nProp].getValue();
                switch ( nProp )
                {
                    case 0: bCJKFont = bValue; break;
                    case 1: bVerticalText = bValue; break;
                    case 2: bAsianTypography = bValue; break;
                    case 3: bJapaneseFind = bValue; break;
                    case 4: bRuby = bValue; break;
                    case 5: bChangeCaseMap = bValue; break;
                    case 6: bDoubleLines = bValue; break;
                    case 7: bEmphasisMarks = bValue; break;
                    case 8: bVerticalCallOut = bValue; break;
                }
            }
        }
    }
    bIsLoaded = sal_True;
}
/*-- 10.04.01 12:41:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void    SvtCJKOptions_Impl::Notify( const Sequence< OUString >& aPropertyNames )
{
    Load();
}
/*-- 10.04.01 12:41:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void    SvtCJKOptions_Impl::Commit()
{
    OUString* pNames = aPropertyNames.getArray();
    Sequence<Any> aValues(aPropertyNames.getLength());
    Any* pValues = aValues.getArray();

    const Type& rType = ::getBooleanCppuType();
    BOOL bVal;
    for(int nProp = 0; nProp < aPropertyNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case  0: bVal = bCJKFont; break;
            case  1: bVal = bVerticalText; break;
            case  2: bVal = bAsianTypography; break;
            case  3: bVal = bJapaneseFind; break;
            case  4: bVal = bRuby; break;
            case  5: bVal = bChangeCaseMap; break;
            case  6: bVal = bDoubleLines; break;
            case  7: bVal = bEmphasisMarks; break;
            case  8: bVal = bVerticalCallOut; break;
        }
        pValues[nProp].setValue(&bVal, getBooleanCppuType());
    }
    PutProperties(aPropertyNames, aValues);
}
// global ----------------------------------------------------------------

static SvtCJKOptions_Impl*  pCJKOptions = NULL;
static sal_Int32            nCJKRefCount = 0;
static ::osl::Mutex         aCJKMutex;

// class SvtCJKOptions --------------------------------------------------

SvtCJKOptions::SvtCJKOptions(sal_Bool bDontLoad)
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( aCJKMutex );
    if ( !pCJKOptions )
        pCJKOptions = new SvtCJKOptions_Impl;
    if( !bDontLoad && !pCJKOptions->IsLoaded())
        pCJKOptions->Load();

    ++nCJKRefCount;
    pImp = pCJKOptions;
}

// -----------------------------------------------------------------------

SvtCJKOptions::~SvtCJKOptions()
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( aCJKMutex );
    if ( !--nCJKRefCount )
        DELETEZ( pCJKOptions );
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsCJKFontEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsCJKFontEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsVerticalTextEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsVerticalTextEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsAsianTypographyEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsAsianTypographyEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsJapaneseFindEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsJapaneseFindEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsRubyEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsRubyEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsChangeCaseMapEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsChangeCaseMapEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsDoubleLinesEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsDoubleLinesEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsEmphasisMarksEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsEmphasisMarksEnabled();
}
// -----------------------------------------------------------------------
sal_Bool SvtCJKOptions::IsVerticalCallOutEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsVerticalCallOutEnabled();
}
/*-- 20.04.01 14:32:04---------------------------------------------------

  -----------------------------------------------------------------------*/
void        SvtCJKOptions::SetAll(sal_Bool bSet)
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    pCJKOptions->SetAll(bSet);
}
/*-- 20.04.01 14:32:06---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool    SvtCJKOptions::IsAnyEnabled() const
{
    DBG_ASSERT(pCJKOptions->IsLoaded(), "CJK options not loaded")
    return pCJKOptions->IsAnyEnabled();
}



