/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: managelang.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2007-01-29 17:01:16 $
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
#include "precompiled_basctl.hxx"

#include "managelang.hxx"
#include "iderid.hxx"
#include "localizationmgr.hxx"
#include "iderdll.hxx"
#include "basidesh.hxx"
#include "basobj.hxx"

#include "managelang.hrc"
#include "dlgresid.hrc"
#include "helpid.hrc"

#ifndef _COM_SUN_STAR_TEXT_XBREAKITERATOR_HPP_
#include <com/sun/star/i18n/XBreakIterator.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_WORDTYPE_HPP_
#include <com/sun/star/i18n/WordType.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_RESOURCE_XSTRINGRESOURCEMANAGER_HPP_
#include <com/sun/star/resource/XStringResourceManager.hpp>
#endif
#include <comphelper/processfactory.hxx>

#ifndef _VCL_UNOHELP_HXX
#include <vcl/unohelp.hxx>
#endif
#ifndef _VCL_I18NHELP_HXX
//#include <vcl/i18nhelp.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _SVX_LANGTAB_HXX
#include <svx/langtab.hxx>
#endif
#include <sfx2/bindings.hxx>

using namespace ::com::sun::star::i18n;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::resource;
using namespace ::com::sun::star::uno;

bool localesAreEqual( const Locale& rLocaleLeft, const Locale& rLocaleRight )
{
    bool bRet = ( rLocaleLeft.Language.equals( rLocaleRight.Language ) &&
                  rLocaleLeft.Country.equals( rLocaleRight.Country ) &&
                  rLocaleLeft.Variant.equals( rLocaleRight.Variant ) );
    return bRet;
}

namespace {
    long getLongestWordWidth( const String& rText, const Window& rWin )
    {
        long nWidth = 0;
        Reference< XBreakIterator > xBreakIter( vcl::unohelper::CreateBreakIterator() );
        sal_Int32 nStartPos = 0;
        const Locale aLocale = Application::GetSettings().GetUILocale();
        Boundary aBoundary = xBreakIter->getWordBoundary(
            rText, nStartPos, aLocale, WordType::ANYWORD_IGNOREWHITESPACES, sal_True );

        while ( aBoundary.startPos != aBoundary.endPos )
        {
            nStartPos = aBoundary.endPos;
            String sWord( rText.Copy(
                (USHORT)aBoundary.startPos,
                (USHORT)aBoundary.endPos - (USHORT)aBoundary.startPos ) );
            long nTemp = rWin.GetCtrlTextWidth( sWord );
            if ( nTemp > nWidth )
                nWidth = nTemp;
            aBoundary = xBreakIter->nextWord(
                rText, nStartPos, aLocale, WordType::ANYWORD_IGNOREWHITESPACES );
        }

        return nWidth;
    }
}

ManageLanguageDialog::ManageLanguageDialog( Window* pParent, LocalizationMgr* _pLMgr ) :

    ModalDialog( pParent, IDEResId( RID_DLG_MANAGE_LANGUAGE ) ),

    m_aLanguageFT       ( this, ResId( FT_LANGUAGE ) ),
    m_aLanguageLB       ( this, ResId( LB_LANGUAGE ) ),
    m_aAddPB            ( this, ResId( PB_ADD_LANG ) ),
    m_aDeletePB         ( this, ResId( PB_DEL_LANG ) ),
    m_aMakeDefPB        ( this, ResId( PB_MAKE_DEFAULT ) ),
    m_aInfoFT           ( this, ResId( FT_INFO ) ),
    m_aBtnLine          ( this, ResId( FL_BUTTONS ) ),
    m_aHelpBtn          ( this, ResId( PB_HELP ) ),
    m_aCloseBtn         ( this, ResId( PB_CLOSE ) ),
    m_pLocalizationMgr  ( _pLMgr ),
    m_sDefLangStr       (       ResId( STR_DEF_LANG ) ),
    m_sDeleteStr        (       ResId( STR_DELETE ) ),
    m_sCreateLangStr    (       ResId( STR_CREATE_LANG ) )

{
    FreeResource();

    Init();
    FillLanguageBox();
    SelectHdl( NULL );
}

ManageLanguageDialog::~ManageLanguageDialog()
{
    ClearLanguageBox();
}

void ManageLanguageDialog::Init()
{
    // get current IDE
    BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
    String sLibName = pIDEShell->GetCurLibName();
    // set dialog title with library name
    String sText = GetText();
    sText.SearchAndReplace( String::CreateFromAscii("$1"), sLibName );
    SetText( sText );
    // set handler
    m_aAddPB.SetClickHdl( LINK( this, ManageLanguageDialog, AddHdl ) );
    m_aDeletePB.SetClickHdl( LINK( this, ManageLanguageDialog, DeleteHdl ) );
    m_aMakeDefPB.SetClickHdl( LINK( this, ManageLanguageDialog, MakeDefHdl ) );
    m_aLanguageLB.SetSelectHdl( LINK( this, ManageLanguageDialog, SelectHdl ) );

    m_aLanguageLB.EnableMultiSelection( TRUE );
    CalcInfoSize();
}

void ManageLanguageDialog::CalcInfoSize()
{
    String sInfoStr = m_aInfoFT.GetText();
    long nInfoWidth = m_aInfoFT.GetSizePixel().Width();
    long nLongWord = getLongestWordWidth( sInfoStr, m_aInfoFT );
    long nTxtWidth = m_aInfoFT.GetCtrlTextWidth( sInfoStr ) + nLongWord;
    long nLines = ( nTxtWidth / nInfoWidth ) + 1;
    if ( nLines > INFO_LINES_COUNT )
    {
        Size aFTSize = m_aLanguageFT.GetSizePixel();
        Size aSize = m_aInfoFT.GetSizePixel();
        long nNewHeight = aFTSize.Height() * nLines;
        long nDelta = nNewHeight - aSize.Height();
        aSize.Height() = nNewHeight;
        m_aInfoFT.SetSizePixel( aSize );

        aSize = m_aLanguageLB.GetSizePixel();
        aSize.Height() -= nDelta;
        m_aLanguageLB.SetSizePixel( aSize );

        Point aNewPos = m_aInfoFT.GetPosPixel();
        aNewPos.Y() -= nDelta;
        m_aInfoFT.SetPosPixel( aNewPos );
        aNewPos = m_aMakeDefPB.GetPosPixel();
        aNewPos.Y() -= nDelta;
        m_aMakeDefPB.SetPosPixel( aNewPos );
    }
}

void ManageLanguageDialog::FillLanguageBox()
{
    DBG_ASSERT( m_pLocalizationMgr, "ManageLanguageDialog::FillLanguageBox(): no localization manager" );

    if ( m_pLocalizationMgr->isLibraryLocalized() )
    {
        SvxLanguageTable aLangTable;
        Locale aDefaultLocale = m_pLocalizationMgr->getStringResourceManager()->getDefaultLocale();
        Sequence< Locale > aLocaleSeq = m_pLocalizationMgr->getStringResourceManager()->getLocales();
        const Locale* pLocale = aLocaleSeq.getConstArray();
        INT32 i, nCount = aLocaleSeq.getLength();
        for ( i = 0;  i < nCount;  ++i )
        {
            bool bIsDefault = localesAreEqual( aDefaultLocale, pLocale[i] );
            LanguageType eLangType = SvxLocaleToLanguage( pLocale[i] );
            String sLanguage = aLangTable.GetString( eLangType );
            if ( bIsDefault )
            {
                sLanguage += ' ';
                sLanguage += m_sDefLangStr;
            }
            USHORT nPos = m_aLanguageLB.InsertEntry( sLanguage );
            m_aLanguageLB.SetEntryData( nPos, new LanguageEntry( sLanguage, pLocale[i], bIsDefault ) );
        }
    }
    else
        m_aLanguageLB.InsertEntry( m_sCreateLangStr );
}

void ManageLanguageDialog::ClearLanguageBox()
{
    USHORT i, nCount = m_aLanguageLB.GetEntryCount();
    for ( i = 0; i < nCount; ++i )
    {
        LanguageEntry* pEntry = (LanguageEntry*)( m_aLanguageLB.GetEntryData(i) );
        if ( pEntry )
            delete pEntry;
    }
    m_aLanguageLB.Clear();
}

IMPL_LINK( ManageLanguageDialog, AddHdl, Button *, EMPTYARG )
{
    SetDefaultLanguageDialog aDlg( this, m_pLocalizationMgr );
    if ( RET_OK == aDlg.Execute() )
    {
        // add new locales
        Sequence< Locale > aLocaleSeq = aDlg.GetLocales();
        m_pLocalizationMgr->handleAddLocales( aLocaleSeq );
        // update listbox
        ClearLanguageBox();
        FillLanguageBox();

        SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
        if ( pBindings )
            pBindings->Invalidate( SID_BASICIDE_CURRENT_LANG );
    }
    return 1;
}

IMPL_LINK( ManageLanguageDialog, DeleteHdl, Button *, EMPTYARG )
{
    QueryBox aQBox( this, IDEResId( RID_QRYBOX_LANGUAGE ) );
    aQBox.SetButtonText( RET_OK, m_sDeleteStr );
    if ( aQBox.Execute() == RET_OK )
    {
        USHORT i, nCount = m_aLanguageLB.GetSelectEntryCount();
        USHORT nPos = m_aLanguageLB.GetSelectEntryPos();
        // remove locales
        Sequence< Locale > aLocaleSeq( nCount );
        for ( i = 0; i < nCount; ++i )
        {
            USHORT nSelPos = m_aLanguageLB.GetSelectEntryPos(i);
            LanguageEntry* pEntry = (LanguageEntry*)( m_aLanguageLB.GetEntryData( nSelPos ) );
            if ( pEntry )
                aLocaleSeq[i] = pEntry->m_aLocale;
        }
        m_pLocalizationMgr->handleRemoveLocales( aLocaleSeq );
        // update listbox
        ClearLanguageBox();
        FillLanguageBox();
        // reset selection
        nCount = m_aLanguageLB.GetEntryCount();
        if ( nCount <= nPos )
            nPos = nCount - 1;
        m_aLanguageLB.SelectEntryPos( nPos );
        SelectHdl( NULL );
    }
    return 1;
}

IMPL_LINK( ManageLanguageDialog, MakeDefHdl, Button *, EMPTYARG )
{
    USHORT nPos = m_aLanguageLB.GetSelectEntryPos();
    LanguageEntry* pSelectEntry = (LanguageEntry*)( m_aLanguageLB.GetEntryData( nPos ) );
    if ( pSelectEntry && !pSelectEntry->m_bIsDefault )
    {
        // set new default entry
        m_pLocalizationMgr->handleSetDefaultLocale( pSelectEntry->m_aLocale );
        // update Listbox
        ClearLanguageBox();
        FillLanguageBox();
        // reset selection
        m_aLanguageLB.SelectEntryPos( nPos );
        SelectHdl( NULL );
    }

    return 1;
}

IMPL_LINK( ManageLanguageDialog, SelectHdl, ListBox *, EMPTYARG )
{
    USHORT nCount = m_aLanguageLB.GetEntryCount();
    bool bEmpty = ( !nCount ||
                    m_aLanguageLB.GetEntryPos( m_sCreateLangStr ) != LISTBOX_ENTRY_NOTFOUND );
    bool bSelect = ( m_aLanguageLB.GetSelectEntryPos() != LISTBOX_ENTRY_NOTFOUND );
    bool bEnable = ( !bEmpty && bSelect != false );

    m_aDeletePB.Enable( bEnable != false );
    m_aMakeDefPB.Enable( bEnable != false && nCount > 1 && m_aLanguageLB.GetSelectEntryCount() == 1 );

    return 1;
}

// class SetDefaultLanguageDialog -----------------------------------------------

SetDefaultLanguageDialog::SetDefaultLanguageDialog( Window* pParent, LocalizationMgr* _pLMgr ) :

    ModalDialog( pParent, IDEResId( RID_DLG_SETDEF_LANGUAGE ) ),

    m_aLanguageFT   ( this, ResId( FT_DEF_LANGUAGE ) ),
    m_pLanguageLB   ( new SvxLanguageBox( this, ResId( LB_DEF_LANGUAGE ) ) ),
    m_pCheckLangLB  ( NULL ),
    m_aInfoFT       ( this, ResId( FT_DEF_INFO ) ),
    m_aBtnLine      ( this, ResId( FL_DEF_BUTTONS ) ),
    m_aOKBtn        ( this, ResId( PB_DEF_OK ) ),
    m_aCancelBtn    ( this, ResId( PB_DEF_CANCEL ) ),
    m_aHelpBtn      ( this, ResId( PB_DEF_HELP ) ),

    m_pLocalizationMgr( _pLMgr )

{
    if ( m_pLocalizationMgr->isLibraryLocalized() )
    {
        // change to "Add Interface Language" mode
        SetHelpId( HID_BASICIDE_ADDNEW_LANGUAGE );
        m_pCheckLangLB = new SvxCheckListBox( this, ResId( LB_ADD_LANGUAGE ) );
        SetText( String( ResId( STR_ADDLANG_TITLE ) ) );
        m_aLanguageFT.SetText( String( ResId( STR_ADDLANG_LABEL ) ) );
        m_aInfoFT.SetText( String( ResId( STR_ADDLANG_INFO ) ) );
    }

    FreeResource();

    FillLanguageBox();
    CalcInfoSize();
}

SetDefaultLanguageDialog::~SetDefaultLanguageDialog()
{
    delete m_pLanguageLB;
    delete m_pCheckLangLB;
}

void SetDefaultLanguageDialog::FillLanguageBox()
{
    // fill list with all languages
    m_pLanguageLB->SetLanguageList( LANG_LIST_ALL, FALSE );
    // remove the already localized languages
    Sequence< Locale > aLocaleSeq = m_pLocalizationMgr->getStringResourceManager()->getLocales();
    const Locale* pLocale = aLocaleSeq.getConstArray();
    INT32 i, nCount = aLocaleSeq.getLength();
    for ( i = 0;  i < nCount;  ++i )
        m_pLanguageLB->RemoveLanguage( SvxLocaleToLanguage( pLocale[i] ) );

    // fill checklistbox if not in default mode
    if ( m_pLocalizationMgr->isLibraryLocalized() )
    {
        USHORT j, nCount_ = m_pLanguageLB->GetEntryCount();
        for ( j = 0;  j < nCount_;  ++j )
        {
            m_pCheckLangLB->InsertEntry(
                m_pLanguageLB->GetEntry(j), LISTBOX_APPEND, m_pLanguageLB->GetEntryData(j) );
        }
        delete m_pLanguageLB;
        m_pLanguageLB = NULL;
    }
    else
        // preselect current UI language
        m_pLanguageLB->SelectLanguage( Application::GetSettings().GetUILanguage() );
}

void SetDefaultLanguageDialog::CalcInfoSize()
{
    String sInfoStr = m_aInfoFT.GetText();
    long nInfoWidth = m_aInfoFT.GetSizePixel().Width();
    long nLongWord = getLongestWordWidth( sInfoStr, m_aInfoFT );
    long nTxtWidth = m_aInfoFT.GetCtrlTextWidth( sInfoStr ) + nLongWord;
    long nLines = ( nTxtWidth / nInfoWidth ) + 1;
    if ( nLines > INFO_LINES_COUNT )
    {
        Size aFTSize = m_aLanguageFT.GetSizePixel();
        Size aSize = m_aInfoFT.GetSizePixel();
        long nNewHeight = aFTSize.Height() * nLines;
        long nDelta = nNewHeight - aSize.Height();
        aSize.Height() = nNewHeight;
        m_aInfoFT.SetSizePixel( aSize );

        Window* pWin = ( m_pLanguageLB != NULL ) ? dynamic_cast< Window* >( m_pLanguageLB )
                                                 : dynamic_cast< Window* >( m_pCheckLangLB );
        aSize = pWin->GetSizePixel();
        aSize.Height() -= nDelta;
        pWin->SetSizePixel( aSize );

        Point aNewPos = m_aInfoFT.GetPosPixel();
        aNewPos.Y() -= nDelta;
        m_aInfoFT.SetPosPixel( aNewPos );
    }
}

Sequence< Locale > SetDefaultLanguageDialog::GetLocales() const
{
    bool bNotLocalized = !m_pLocalizationMgr->isLibraryLocalized();
    INT32 nSize = bNotLocalized ? 1 : m_pCheckLangLB->GetCheckedEntryCount();
    Sequence< Locale > aLocaleSeq( nSize );
    if ( bNotLocalized )
    {
        Locale aLocale;
        SvxLanguageToLocale( aLocale, m_pLanguageLB->GetSelectLanguage() );
        aLocaleSeq[0] = aLocale;
    }
    else
    {
        USHORT i, nCount = static_cast< USHORT >( m_pCheckLangLB->GetEntryCount() );
        INT32 j = 0;
        for ( i = 0; i < nCount; ++i )
        {
            if ( m_pCheckLangLB->IsChecked(i) )
            {
                LanguageType eType = LanguageType( (ULONG)m_pCheckLangLB->GetEntryData(i) );
                Locale aLocale;
                SvxLanguageToLocale( aLocale, eType );
                aLocaleSeq[j++] = aLocale;
            }
        }
        DBG_ASSERT( nSize == j, "SetDefaultLanguageDialog::GetLocales(): invalid indexes" );
    }
    return aLocaleSeq;
}

