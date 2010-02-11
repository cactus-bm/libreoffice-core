/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: thessubmenu.cxx,v $
 * $Revision: 1.0 $
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
#include "precompiled_sfx2.hxx"

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/linguistic2/XThesaurus.hpp>
#include <com/sun/star/linguistic2/XMeaning.hpp>
#include <com/sun/star/linguistic2/XLinguServiceManager.hpp>

#include <comphelper/processfactory.hxx>
#include <svl/stritem.hxx>
#include <tools/debug.hxx>
#include <vcl/graph.hxx>
#include <svtools/filter.hxx>


#include <vector>

#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewsh.hxx>
#include "thessubmenu.hxx"


using namespace ::com::sun::star;
using ::rtl::OUString;


// STATIC DATA -----------------------------------------------------------

SFX_IMPL_MENU_CONTROL(SfxThesSubMenuControl, SfxStringItem);

////////////////////////////////////////////////////////////

String GetThesaurusReplaceText_Impl( const ::rtl::OUString &rText )
{
    // The strings returned by the thesaurus sometimes have some
    // explanation text put in between '(' and ')' or a trailing '*'.
    // These parts should not be put in the ReplaceEdit Text that may get
    // inserted into the document. Thus we strip them from the text.

    String aText( rText );

    xub_StrLen nPos = aText.Search( sal_Unicode('(') );
    while (STRING_NOTFOUND != nPos)
    {
        xub_StrLen nEnd = aText.Search( sal_Unicode(')'), nPos );
        if (STRING_NOTFOUND != nEnd)
            aText.Erase( nPos, nEnd-nPos+1 );
        else
            break;
        nPos = aText.Search( sal_Unicode('(') );
    }

    nPos = aText.Search( sal_Unicode('*') );
    if (STRING_NOTFOUND != nPos)
        aText.Erase( nPos );

    // remove any possible remaining ' ' that may confuse the thesaurus
    // when it gets called with the text
    aText.EraseLeadingAndTrailingChars( sal_Unicode(' ') );

    return aText;
}

////////////////////////////////////////////////////////////


/*
    Ctor; setzt Select-Handler am Menu und traegt Menu
    in seinen Parent ein.
 */
SfxThesSubMenuControl::SfxThesSubMenuControl( USHORT nSlotId, Menu &rMenu, SfxBindings &rBindings )
    : SfxMenuControl( nSlotId, rBindings ),
    pMenu(new PopupMenu),
    rParent(rMenu)
{
    rMenu.SetPopupMenu(nSlotId, pMenu);
    pMenu->SetSelectHdl(LINK(this, SfxThesSubMenuControl, MenuSelect));
    pMenu->Clear();
    rParent.EnableItem( GetId(), FALSE );
}


SfxThesSubMenuControl::~SfxThesSubMenuControl()
{
    delete pMenu;
}


/*
    Statusbenachrichtigung;
    Ist die Funktionalit"at disabled, wird der entsprechende
    Menueeintrag im Parentmenu disabled, andernfalls wird er enabled.
 */
void SfxThesSubMenuControl::StateChanged(
    USHORT /*nSID*/,
    SfxItemState eState,
    const SfxPoolItem* /*pState*/ )
{
    rParent.EnableItem(GetId(), SFX_ITEM_AVAILABLE == eState );
}


/*
    Select-Handler des Menus;
    das selektierte Verb mit ausgef"uhrt,
 */
IMPL_LINK_INLINE_START( SfxThesSubMenuControl, MenuSelect, Menu *, pSelMenu )
{
    const USHORT nSlotId = pSelMenu->GetCurItemId();
    if( nSlotId )
        GetBindings().Execute(nSlotId);
    return 1;
}
IMPL_LINK_INLINE_END( SfxThesSubMenuControl, MenuSelect, Menu *, pSelMenu )


PopupMenu* SfxThesSubMenuControl::GetPopup() const
{
    return pMenu;
}


////////////////////////////////////////////////////////////

OUString SfxThesSubMenuHelper::GetText(
    const String &rLookUpString,
    xub_StrLen nDelimPos )
{
    return OUString( rLookUpString.Copy( 0, nDelimPos ) );
}


void SfxThesSubMenuHelper::GetLocale(
    lang::Locale /*out */ &rLocale,
    const String &rLookUpString,
    xub_StrLen nDelimPos  )
{
    String aIsoLang( rLookUpString.Copy( nDelimPos + 1) );
    const xub_StrLen nPos = aIsoLang.Search( '-' );
    if (nPos != STRING_NOTFOUND)
    {
        rLocale.Language    = aIsoLang.Copy( 0, nPos );
        rLocale.Country     = aIsoLang.Copy( nPos + 1 );
        rLocale.Variant     = String::EmptyString();
    }
}


SfxThesSubMenuHelper::SfxThesSubMenuHelper()
{
    try
    {
        uno::Reference< lang::XMultiServiceFactory >  xMSF( ::comphelper::getProcessServiceFactory(), uno::UNO_QUERY_THROW );
        m_xLngMgr = uno::Reference< linguistic2::XLinguServiceManager >( xMSF->createInstance(
                OUString( RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.linguistic2.LinguServiceManager" ))), uno::UNO_QUERY_THROW );
        m_xThesarus = m_xLngMgr->getThesaurus();
    }
    catch (uno::Exception &e)
    {
        (void) e;
        DBG_ASSERT( 0, "failed to get thesaurus" );
    }
}


SfxThesSubMenuHelper::~SfxThesSubMenuHelper()
{
}


bool SfxThesSubMenuHelper::IsSupportedLocale( const lang::Locale & rLocale ) const
{
    return m_xThesarus.is() && m_xThesarus->hasLocale( rLocale );
}


bool SfxThesSubMenuHelper::GetMeanings(
    std::vector< OUString > & rSynonyms,
    const OUString & rWord,
    const lang::Locale & rLocale,
    sal_Int16 nMaxSynonms )
{
    bool bHasMoreSynonyms = false;
    rSynonyms.clear();
    if (IsSupportedLocale( rLocale ) && rWord.getLength() && nMaxSynonms > 0)
    {
        try
        {
            // get all meannings
            const uno::Sequence< uno::Reference< linguistic2::XMeaning > > aMeaningSeq(
                    m_xThesarus->queryMeanings( rWord, rLocale, uno::Sequence< beans::PropertyValue >() ));
            const uno::Reference< linguistic2::XMeaning > *pxMeaning = aMeaningSeq.getConstArray();
            const sal_Int32 nMeanings = aMeaningSeq.getLength();

            // iterate over all meanings until nMaxSynonms are found or all meanings are processed
            sal_Int32 nCount = 0;
            sal_Int32 i = 0;
            for ( ;  i < nMeanings && nCount < nMaxSynonms;  ++i)
            {
                const uno::Sequence< OUString > aSynonymSeq( pxMeaning[i]->querySynonyms() );
                const OUString *pSynonyms = aSynonymSeq.getConstArray();
                const sal_Int32 nSynonyms = aSynonymSeq.getLength();
                sal_Int32 k = 0;
                for ( ;  k < nSynonyms && nCount < nMaxSynonms;  ++k)
                {
                    rSynonyms.push_back( pSynonyms[k] );
                    ++nCount;
                }
                bHasMoreSynonyms = k < nSynonyms;    // any synonym from this meaning skipped?
            }

            bHasMoreSynonyms |= i < nMeanings;   // any meaning skipped?
        }
        catch (uno::Exception &e)
        {
            (void) e;
            DBG_ASSERT( 0, "failed to get synonyms" );
        }
    }
    return bHasMoreSynonyms;
}


String SfxThesSubMenuHelper::GetThesImplName( const lang::Locale &rLocale ) const
{
    String aRes;
    DBG_ASSERT( m_xLngMgr.is(), "LinguServiceManager missing" );
    if (m_xLngMgr.is())
    {
        uno::Sequence< OUString > aServiceNames = m_xLngMgr->getConfiguredServices(
                OUString::createFromAscii("com.sun.star.linguistic2.Thesaurus"), rLocale );
        // there should be at most one thesaurus configured for each language
        DBG_ASSERT( aServiceNames.getLength() <= 1, "more than one thesaurus found. Should not be possible" );
        if (aServiceNames.getLength() == 1)
            aRes = aServiceNames[0];
    }
    return aRes;
}

////////////////////////////////////////////////////////////

//!! temporary implement locally:
//!! once MBAs latest CWS is integrated this functions are available in svtools
//!! under a slightly different name

#include <sfx2/docfile.hxx>

#define IMPGRF_INIKEY_ASLINK        "ImportGraphicAsLink"
#define IMPGRF_INIKEY_PREVIEW       "ImportGraphicPreview"
#define IMPGRF_CONFIGNAME           String(DEFINE_CONST_UNICODE("ImportGraphicDialog"))

GraphicFilter* lcl_GetGrfFilter()
{
    return GraphicFilter::GetGraphicFilter();
}

// -----------------------------------------------------------------------

int lcl_LoadGraphic( const String &rPath, const String &rFilterName,
                 Graphic& rGraphic, GraphicFilter* pFilter,
                 USHORT* pDeterminedFormat )
{
    if ( !pFilter )
        pFilter = ::lcl_GetGrfFilter();

    const USHORT nFilter = rFilterName.Len() && pFilter->GetImportFormatCount()
                    ? pFilter->GetImportFormatNumber( rFilterName )
                    : GRFILTER_FORMAT_DONTKNOW;

    SfxMedium* pMed = 0;

    // dann teste mal auf File-Protokoll:
    SvStream* pStream = NULL;
    INetURLObject aURL( rPath );

    if ( aURL.HasError() || INET_PROT_NOT_VALID == aURL.GetProtocol() )
    {
        aURL.SetSmartProtocol( INET_PROT_FILE );
        aURL.SetSmartURL( rPath );
    }
    else if ( INET_PROT_FILE != aURL.GetProtocol() )
    {
        // z.Z. nur auf die aktuelle DocShell
        pMed = new SfxMedium( rPath, STREAM_READ, TRUE );
        pMed->DownLoad();
        pStream = pMed->GetInStream();
    }
    int nRes = GRFILTER_OK;

    if ( !pStream )
        nRes = pFilter->ImportGraphic( rGraphic, aURL, nFilter, pDeterminedFormat );
    else
        nRes = pFilter->ImportGraphic( rGraphic, rPath, *pStream,
                                       nFilter, pDeterminedFormat );
    if ( pMed )
        delete pMed;
    return nRes;
}


