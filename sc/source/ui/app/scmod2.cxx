/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: scmod2.cxx,v $
 * $Revision: 1.12 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"



//------------------------------------------------------------------

#include <svx/unolingu.hxx>
#include <svtools/lingucfg.hxx>

#include <com/sun/star/linguistic2/XThesaurus.hpp>
#include <com/sun/star/lang/Locale.hpp>

using namespace com::sun::star;

#include "scmod.hxx"

//------------------------------------------------------------------

#define LINGUPROP_AUTOSPELL         "IsSpellAuto"
#define LINGUPROP_HIDEAUTO          "IsSpellHide"

//------------------------------------------------------------------

// static
void ScModule::GetSpellSettings( USHORT& rDefLang, USHORT& rCjkLang, USHORT& rCtlLang,
                                    BOOL& rAutoSpell, BOOL& rHideAuto )
{
    //  use SvtLinguConfig instead of service LinguProperties to avoid
    //  loading the linguistic component
    SvtLinguConfig aConfig;

    SvtLinguOptions aOptions;
    aConfig.GetOptions( aOptions );

    rDefLang = aOptions.nDefaultLanguage;
    rCjkLang = aOptions.nDefaultLanguage_CJK;
    rCtlLang = aOptions.nDefaultLanguage_CTL;
    rAutoSpell = aOptions.bIsSpellAuto;
    rHideAuto = aOptions.bIsSpellHideMarkings;
}

// static
void ScModule::SetAutoSpellProperty( BOOL bSet )
{
    //  use SvtLinguConfig instead of service LinguProperties to avoid
    //  loading the linguistic component
    SvtLinguConfig aConfig;

    uno::Any aAny;
    aAny <<= bSet;
    aConfig.SetProperty( rtl::OUString::createFromAscii( LINGUPROP_AUTOSPELL ), aAny );
}

// static
void ScModule::SetHideAutoProperty( BOOL bSet )
{
    //  use SvtLinguConfig instead of service LinguProperties to avoid
    //  loading the linguistic component
    SvtLinguConfig aConfig;

    uno::Any aAny;
    aAny <<= bSet;
    aConfig.SetProperty( rtl::OUString::createFromAscii( LINGUPROP_HIDEAUTO ), aAny );
}


// static
BOOL ScModule::HasThesaurusLanguage( USHORT nLang )
{
    if ( nLang == LANGUAGE_NONE )
        return FALSE;

    lang::Locale aLocale;
    SvxLanguageToLocale( aLocale, nLang );

    BOOL bHasLang = FALSE;
    try
    {
        uno::Reference< linguistic2::XThesaurus > xThes(LinguMgr::GetThesaurus());
        if ( xThes.is() )
            bHasLang = xThes->hasLocale( aLocale );
    }
    catch( uno::Exception& )
    {
        DBG_ERROR("Error in Thesaurus");
    }

    return bHasLang;
}


