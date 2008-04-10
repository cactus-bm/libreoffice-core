/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: spelldsp.hxx,v $
 * $Revision: 1.8 $
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

#ifndef _LINGUISTIC_SPELLDSP_HXX_
#define _LINGUISTIC_SPELLDSP_HXX_

#include "lngopt.hxx"
#include "misc.hxx"
#include "iprcache.hxx"

#include <uno/lbnames.h>            // CPPU_CURRENT_LANGUAGE_BINDING_NAME macro, which specify the environment type
#include <cppuhelper/implbase1.hxx> // helper for implementations
#include <cppuhelper/implbase2.hxx> // helper for implementations
#include <cppuhelper/implbase7.hxx> // helper for implementations
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceDisplayName.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValues.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/linguistic2/XSpellChecker.hpp>
#include <com/sun/star/linguistic2/XSpellChecker1.hpp>
#include <com/sun/star/linguistic2/XSearchableDictionaryList.hpp>
#include <com/sun/star/linguistic2/XLinguServiceEventBroadcaster.hpp>
#include <tools/table.hxx>


class LngSvcMgr;

///////////////////////////////////////////////////////////////////////////

class SeqLangSvcEntry_Spell
{
    friend class SpellCheckerDispatcher;
    friend BOOL SvcListHasLanguage(
                            const SeqLangSvcEntry_Spell &rEntry,
                            INT16 nLanguage );

    ::com::sun::star::uno::Sequence< ::rtl::OUString >          aSvcImplNames;
    ::com::sun::star::uno::Sequence<
        ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XSpellChecker > >    aSvcRefs;
    ::com::sun::star::uno::Sequence<
        ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XSpellChecker1 > >   aSvc1Refs;

//  INT16           nLang;     //used as key in the table
    SvcFlags        aFlags;

public:
    SeqLangSvcEntry_Spell() {}
    SeqLangSvcEntry_Spell( const ::com::sun::star::uno::Sequence<
            ::rtl::OUString > &rSvcImplNames );
    ~SeqLangSvcEntry_Spell();

    BOOL    IsAlreadyWarned() const     { return aFlags.bAlreadyWarned != 0; }
    void    SetAlreadyWarned(BOOL bVal) { aFlags.bAlreadyWarned = 0 != bVal; }
    BOOL    IsDoWarnAgain() const       { return aFlags.bDoWarnAgain != 0; }
    void    SetDoWarnAgain(BOOL bVal)   { aFlags.bDoWarnAgain = 0 != bVal; }
};

DECLARE_TABLE( SpellSvcList, SeqLangSvcEntry_Spell * )


class SpellCheckerDispatcher :
    public cppu::WeakImplHelper2
    <
        ::com::sun::star::linguistic2::XSpellChecker,
        ::com::sun::star::linguistic2::XSpellChecker1
    >,
    public LinguDispatcher
{
    SpellSvcList        aSvcList;
    LinguOptions        aOpt;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >                     xPropSet;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSearchableDictionaryList >  xDicList;

    LngSvcMgr                   &rMgr;
    linguistic::IPRSpellCache   *pExtCache; // SpellCache for external SpellCheckers
                                    // (usually those not called via XSpellChecker1)
                                    // One for all of them.

    // disallow copy-constructor and assignment-operator for now
    SpellCheckerDispatcher(const SpellCheckerDispatcher &);
    SpellCheckerDispatcher & operator = (const SpellCheckerDispatcher &);

    inline linguistic::IPRSpellCache &  GetExtCache() const;

    inline ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
            GetPropSet();
    inline ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSearchableDictionaryList >
            GetDicList();

    void    ClearSvcList();

    BOOL    isValid_Impl(const ::rtl::OUString& aWord, INT16 nLanguage,
                    const ::com::sun::star::beans::PropertyValues& aProperties,
                    BOOL bCheckDics)
                throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IllegalArgumentException );

    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSpellAlternatives >
            spell_Impl(const ::rtl::OUString& aWord, INT16 nLanguage,
                    const ::com::sun::star::beans::PropertyValues& aProperties,
                    BOOL bCheckDics)
                throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IllegalArgumentException );

    BOOL    isValidInAny(const ::rtl::OUString& aWord,
                         const ::com::sun::star::uno::Sequence< INT16 >& aLanguages,
                         const ::com::sun::star::beans::PropertyValues& aProperties)
                throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IllegalArgumentException );

    com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSpellAlternatives >
            spellInAny(const ::rtl::OUString& aWord,
                       const ::com::sun::star::uno::Sequence< INT16 >& aLanguages,
                       const ::com::sun::star::beans::PropertyValues& aProperties,
                       INT16 nPreferredResultLang)
                throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IllegalArgumentException );

public:
    SpellCheckerDispatcher( LngSvcMgr &rLngSvcMgr );
    virtual ~SpellCheckerDispatcher();

    // XSupportedLocales (for XSpellChecker)
    virtual ::com::sun::star::uno::Sequence<
            ::com::sun::star::lang::Locale > SAL_CALL
        getLocales()
            throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL
        hasLocale( const ::com::sun::star::lang::Locale& aLocale )
            throw(::com::sun::star::uno::RuntimeException);

    // XSpellChecker
    virtual sal_Bool SAL_CALL
        isValid( const ::rtl::OUString& aWord,
                const ::com::sun::star::lang::Locale& aLocale,
                const ::com::sun::star::beans::PropertyValues& aProperties )
            throw(::com::sun::star::lang::IllegalArgumentException,
                  ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XSpellAlternatives > SAL_CALL
        spell( const ::rtl::OUString& aWord,
                const ::com::sun::star::lang::Locale& aLocale,
                const ::com::sun::star::beans::PropertyValues& aProperties )
            throw(::com::sun::star::lang::IllegalArgumentException,
                  ::com::sun::star::uno::RuntimeException);

    // XSupportedLanguages (for XSpellChecker1)
    virtual ::com::sun::star::uno::Sequence< sal_Int16 > SAL_CALL
        getLanguages()
            throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL
        hasLanguage( sal_Int16 nLanguage )
            throw(::com::sun::star::uno::RuntimeException);

    // XSpellChecker1 (same as XSpellChecker but sal_Int16 for language)
    virtual sal_Bool SAL_CALL
        isValid( const ::rtl::OUString& aWord, sal_Int16 nLanguage,
                const ::com::sun::star::beans::PropertyValues& aProperties )
            throw(::com::sun::star::lang::IllegalArgumentException,
                  ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XSpellAlternatives > SAL_CALL
        spell( const ::rtl::OUString& aWord, sal_Int16 nLanguage,
                const ::com::sun::star::beans::PropertyValues& aProperties )
            throw(::com::sun::star::lang::IllegalArgumentException,
                  ::com::sun::star::uno::RuntimeException);

    // LinguDispatcher
    virtual void
        SetServiceList( const ::com::sun::star::lang::Locale &rLocale,
                const ::com::sun::star::uno::Sequence<
                    rtl::OUString > &rSvcImplNames );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString >
        GetServiceList( const ::com::sun::star::lang::Locale &rLocale ) const;
    virtual DspType
        GetDspType() const;
};


inline linguistic::IPRSpellCache & SpellCheckerDispatcher::GetExtCache() const
{
    if (!pExtCache)
        ((SpellCheckerDispatcher *) this)->pExtCache = new linguistic::IPRSpellCache( 997 );
    return *pExtCache;
}


inline ::com::sun::star::uno::Reference<
    ::com::sun::star::beans::XPropertySet >
        SpellCheckerDispatcher::GetPropSet()
{
    return xPropSet.is() ?
        xPropSet : xPropSet = linguistic::GetLinguProperties();
}


inline ::com::sun::star::uno::Reference<
    ::com::sun::star::linguistic2::XSearchableDictionaryList >
        SpellCheckerDispatcher::GetDicList()
{
    return xDicList.is() ?
        xDicList : xDicList = linguistic::GetSearchableDictionaryList();
}


///////////////////////////////////////////////////////////////////////////

#endif

