/*************************************************************************
 *
 *  $RCSfile: hangulhanja.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 14:32:06 $
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
#ifndef SVX_HANGUL_HANJA_CONVERSION_HXX
#include "hangulhanja.hxx"
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx> //add CHINA001
#endif
// header for class SvtLinguConfig
#ifndef _SVTOOLS_LINGUCFG_HXX_
#include <svtools/lingucfg.hxx>
#endif
#ifndef _SVTOOLS_LINGUPROPS_HXX_
#include <svtools/linguprops.hxx>
#endif

#include <set>
#include <map>

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_XBREAKITERATOR_HPP_
#include <com/sun/star/i18n/XBreakIterator.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HPP_
#include <com/sun/star/i18n/ScriptType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_UNICODESCRIPT_HPP_
#include <com/sun/star/i18n/UnicodeScript.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_XTEXTCONVERSION_HPP_
#include <com/sun/star/i18n/XTextConversion.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_TEXTCONVERSIONTYPE_HPP_
#include <com/sun/star/i18n/TextConversionType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_TEXTCONVERSIONOPTION_HPP_
#include <com/sun/star/i18n/TextConversionOption.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_WORDTYPE_HPP_
#include <com/sun/star/i18n/WordType.hpp>
#endif

#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif
#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif

#include "svxdlg.hxx" //CHINA001
#include <dialogs.hrc> //CHINA001

#ifndef _UNO_LINGU_HXX
#include <unolingu.hxx>
#endif

#define HHC HangulHanjaConversion

//.............................................................................
namespace svx
{
//.............................................................................

    using namespace ::rtl;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::i18n;
    using namespace ::com::sun::star::i18n::TextConversionOption;
    using namespace ::com::sun::star::i18n::TextConversionType;
    using namespace ::com::sun::star::lang;
/*
    using HangulHanjaConversion::ReplacementAction;
    using HangulHanjaConversion::eExchange;
    using HangulHanjaConversion::eReplacementBracketed;
    using HangulHanjaConversion::eOriginalBracketed;
    using HangulHanjaConversion::eReplacementAbove;
    using HangulHanjaConversion::eOriginalAbove;
    using HangulHanjaConversion::eReplacementBelow;
    using HangulHanjaConversion::eOriginalBelow;

    using HangulHanjaConversion::eHangulToHanja;
    using HangulHanjaConversion::eHanjaToHangul;

    using HangulHanjaConversion::eSimpleConversion;
    using HangulHanjaConversion::eHangulBracketed;
    using HangulHanjaConversion::eHanjaBracketed;
    using HangulHanjaConversion::eRubyHanjaAbove;
    using HangulHanjaConversion::eRubyHanjaBelow;
    using HangulHanjaConversion::eRubyHangulAbove;
    using HangulHanjaConversion::eRubyHangulBelow;

    using ::com::sun::star::i18n::TextConversionType::TO_HANJA;
    using ::com::sun::star::i18n::TextConversionType::TO_HANGUL;
    using ::com::sun::star::i18n::TextConversionOption::CHARACTER_BY_CHARACTER;
    using ::com::sun::star::i18n::TextConversionOption::NONE;
*/
    //=========================================================================
    //= HangulHanjaConversion_Impl
    //=========================================================================
    //using HangulHanjaConversion::ConversionFormat;

    class HangulHanjaConversion_Impl
    {
    private:
        typedef ::std::set< OUString, ::std::less< OUString > >                 StringBag;
        typedef ::std::map< OUString, OUString, ::std::less< OUString > >   StringMap;

    private:
        StringBag               m_sIgnoreList;
        StringMap               m_aChangeList;
        static StringMap        m_aRecentlyUsedList;

        // general
        AbstractHangulHanjaConversionDialog* //CHINA001 HangulHanjaConversionDialog*
                                m_pConversionDialog;    // the dialog to display for user interaction
        Window*                 m_pUIParent;            // the parent window for any UI we raise
        Reference< XMultiServiceFactory >
                                m_xORB;                 // the service factory to use
        Reference< XTextConversion >
                                m_xConverter;           // the text conversion service
        Locale                  m_aSourceLocale;        // the locale we're working with

        // additions for Chinese simplified / traditional conversion
        HHC::ConversionType     m_eConvType;        // conversion type (Hangul/Hanja, simplified/traditional Chinese,...)
        LanguageType            m_nSourceLang;      // just a 'copy' of m_aSourceLocale in order in order to
                                                    // save the applications from always converting to this
                                                    // type in their implementations
        LanguageType            m_nTargetLang;      // target language of new replacement text
        const Font*             m_pTargetFont;      // target font of new replacement text
        sal_Int32               m_nConvOptions;     // text conversion options (as used by 'getConversions')
        sal_Bool                m_bIsInteractive;   // specifies if the conversion requires user interaction
                                                    // (and likeley a specialised dialog) or if it is to run
                                                    // automatically without any user interaction.
                                                    // True for Hangul / Hanja conversion
                                                    // False for Chinese simlified / traditional conversion

        HangulHanjaConversion*  m_pAntiImpl;            // our "anti-impl" instance

        // options
        sal_Bool                    m_bByCharacter;                 // are we in "by character" mode currently?
        HHC::ConversionFormat       m_eConversionFormat;            // the current format for the conversion
        HHC::ConversionDirection    m_ePrimaryConversionDirection;  // the primary conversion direction
        HHC::ConversionDirection    m_eCurrentConversionDirection;  // the primary conversion direction

        //options from Hangul/Hanja Options dialog (also saved to configuration)
        bool                    m_bIgnorePostPositionalWord;
        bool                    m_bShowRecentlyUsedFirst;
        bool                    m_bAutoReplaceUnique;

        // state
        OUString                m_sCurrentPortion;      // the text which we are currently working on
        sal_Int32               m_nCurrentStartIndex;   // the start index within m_sCurrentPortion of the current convertible portion
        sal_Int32               m_nCurrentEndIndex;     // the end index (excluding) within m_sCurrentPortion of the current convertible portion
        sal_Int32               m_nReplacementBaseIndex;// index which ReplaceUnit-calls need to be relative to
        Sequence< OUString >
                                m_aCurrentSuggestions;  // the suggestions for the current unit
                                                        // (means for the text [m_nCurrentStartIndex, m_nCurrentEndIndex) in m_sCurrentPortion)
        sal_Bool                m_bTryBothDirections;   // specifies if noth conversion directions should be tried when looking for convertible characters


    public:
        HangulHanjaConversion_Impl(
            Window* _pUIParent,
            const Reference< XMultiServiceFactory >& _rxORB,
            const Locale& _rSourceLocale,
            const Locale& _rTargetLocale,
            const Font* _pTargetFont,
            sal_Int32 _nConvOptions,
            sal_Bool _bIsInteractive,
            HangulHanjaConversion* _pAntiImpl );

    public:

        static void         SetUseSavedConversionDirectionState( sal_Bool bVal );

                void        DoDocumentConversion( );

        inline  sal_Bool    IsByCharacter( ) const { return m_bByCharacter; }

        inline  sal_Bool    IsValid() const { return m_xConverter.is(); }

        inline sal_Bool                 GetTryBothDirections()          { return m_bTryBothDirections; }
        inline HHC::ConversionDirection GetPrimaryConversionDirection() { return m_ePrimaryConversionDirection; }

        inline LanguageType GetSourceLang() const   { return m_nSourceLang; }
        inline LanguageType GetTargetLang() const   { return m_nTargetLang; }
        inline const Font * GetTargetFont() const   { return m_pTargetFont; }
        inline sal_Int32    GetConvOptions() const  { return m_nConvOptions; }
        inline sal_Bool     IsInteractive() const   { return m_bIsInteractive; }

    protected:
        void    createDialog();

        /** continue with the conversion, return <TRUE/> if and only if the complete conversion is done
            @param _bRepeatCurrentUnit
                if <TRUE/>, an implNextConvertible will be called initially to advance to the next convertible.
                if <FALSE/>, the method will initially work with the current convertible unit
        */
        sal_Bool ContinueConversion( bool _bRepeatCurrentUnit );

    private:
        DECL_LINK( OnOptionsChanged, void* );
        DECL_LINK( OnIgnore, void* );
        DECL_LINK( OnIgnoreAll, void* );
        DECL_LINK( OnChange, void* );
        DECL_LINK( OnChangeAll, void* );
        DECL_LINK( OnByCharClicked, CheckBox* );
        DECL_LINK( OnConversionTypeChanged, void* );
        DECL_LINK( OnFind, void* );

        /** proceed, after the current convertible has been handled

            <p><b>Attention:</b>
                When returning from this method, the dialog may have been deleted!</p>

            @param _bRepeatCurrentUnit
                will be passed to the <member>ContinueConversion</member> call
        */
        void    implProceed( bool _bRepeatCurrentUnit );

        // change the current convertible, and do _not_ proceed
        void    implChange( const OUString& _rChangeInto );

        /** find the next convertible piece of text, with possibly advancing to the next portion

            @see HangulHanjaConversion::GetNextPortion
        */
        sal_Bool    implNextConvertible( bool _bRepeatUnit );

        /** find the next convertible unit within the current portion
            @param _bRepeatUnit
                if <TRUE/>, the search will start at the beginning of the current unit,
                if <FALSE/>, it will start at the end of the current unit
        */
        bool        implNextConvertibleUnit( const sal_Int32 _nStartAt );

        /** retrieves the next portion, with setting the index members properly
            @return
                <TRUE/> if and only if there is a next portion
        */
        bool        implRetrieveNextPortion( );

        /** fill in m_ePrimaryConversionDirection from m_sCurrentPortion
            (used to determine the initial(!) conversion direction)

            @return
                <FALSE/> if and only if something went wrong
        */
        bool        implDeterminePrimaryDirection( );

        /** member m_aCurrentSuggestions and m_nCurrentEndIndex are updated according to the other settings and current dictionaries

            if _bAllowSearchNextConvertibleText is true _nStartAt is used as starting point to search the next
            convertible text portion. This may result in changing of the member m_nCurrentStartIndex additionally.

            @return
                <TRUE/> if Suggestions were found
        */
        bool        implUpdateSuggestions( const bool _bAllowSearchNextConvertibleText=false, const sal_Int32 _nStartAt=-1 );

        /** reads the options from Hangul/Hanja Options dialog that are saved to configuration
        */
        void implReadOptionsFromConfiguration();

        /** get the string currently considered to be replaced or ignored
        */
        ::rtl::OUString GetCurrentUnit() const;

        /** read options from configuration, update suggestion list and dialog content
        */
        void implUpdateData();

        /** get the conversion direction dependent from m_eConvType and m_ePrimaryConversionDirection
            in case of switching the direction is allowed this can be triggered with parameter bSwitchDirection
        */
        sal_Int16 implGetConversionType( bool bSwitchDirection=false ) const;
    };

    //=========================================================================
    //= HangulHanjaConversion_Impl
    //=========================================================================
    //-------------------------------------------------------------------------
    // static member initialization
    HangulHanjaConversion_Impl::StringMap HangulHanjaConversion_Impl::m_aRecentlyUsedList = HangulHanjaConversion_Impl::StringMap();

    //-------------------------------------------------------------------------
    HangulHanjaConversion_Impl::HangulHanjaConversion_Impl( Window* _pUIParent,
        const Reference< XMultiServiceFactory >& _rxORB,
        const Locale& _rSourceLocale,
        const Locale& _rTargetLocale,
        const Font* _pTargetFont,
        sal_Int32 _nOptions,
        sal_Bool _bIsInteractive,
        HangulHanjaConversion* _pAntiImpl )
        :m_pUIParent( _pUIParent )
        ,m_pAntiImpl( _pAntiImpl )
        ,m_pConversionDialog( NULL )
        ,m_nCurrentStartIndex( 0 )
        ,m_nCurrentEndIndex( 0 )
        ,m_nReplacementBaseIndex( 0 )
        ,m_bTryBothDirections( sal_True )
        ,m_xORB( _rxORB )
        ,m_aSourceLocale( _rSourceLocale )
        ,m_nSourceLang( SvxLocaleToLanguage( _rSourceLocale ) )
        ,m_nTargetLang( SvxLocaleToLanguage( _rTargetLocale ) )
        ,m_pTargetFont( _pTargetFont )
        ,m_bIsInteractive( _bIsInteractive )
    {
        implReadOptionsFromConfiguration();

        DBG_ASSERT( m_xORB.is(), "HangulHanjaConversion_Impl::HangulHanjaConversion_Impl: no ORB!" );

        // determine conversion type
        if (m_nSourceLang == LANGUAGE_KOREAN && m_nTargetLang == LANGUAGE_KOREAN)
            m_eConvType = HHC::eConvHangulHanja;
        else if (m_nSourceLang == LANGUAGE_CHINESE_TRADITIONAL && m_nTargetLang == LANGUAGE_CHINESE_SIMPLIFIED  ||
                 m_nSourceLang == LANGUAGE_CHINESE_SIMPLIFIED  && m_nTargetLang == LANGUAGE_CHINESE_TRADITIONAL)
            m_eConvType = HHC::eConvSimplifiedTraditional;
        else
            DBG_ERROR( "failed to determine conversion type from languages" );

        // set remaining conversion parameters to their default values
        m_nConvOptions      = _nOptions;
        m_bByCharacter      = 0 != (_nOptions & CHARACTER_BY_CHARACTER);
        m_eConversionFormat = HHC::eSimpleConversion;
        m_ePrimaryConversionDirection = HHC::eHangulToHanja;    // used for eConvHangulHanja
        m_eCurrentConversionDirection = HHC::eHangulToHanja;    // used for eConvHangulHanja

        if ( m_xORB.is() )
        {
            OUString sTextConversionService( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.i18n.TextConversion" ) );
            m_xConverter = m_xConverter.query( m_xORB->createInstance( sTextConversionService ) );
            if ( !m_xConverter.is() )
                ShowServiceNotAvailableError( m_pUIParent, sTextConversionService, sal_True );
        }

    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion_Impl::createDialog()
    {
        DBG_ASSERT( m_bIsInteractive, "createDialog when the conversion should not be interactive?" );
        if ( m_bIsInteractive && !m_pConversionDialog )
        {
            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
            if(pFact)
            {
                m_pConversionDialog = pFact->CreateHangulHanjaConversionDialog(m_pUIParent, m_ePrimaryConversionDirection, ResId(RID_SVX_MDLG_HANGULHANJA));
                DBG_ASSERT(m_pConversionDialog, "Dialogdiet fail!");//CHINA001

                m_pConversionDialog->EnableRubySupport( m_pAntiImpl->HasRubySupport() );

                m_pConversionDialog->SetByCharacter( m_bByCharacter );
                m_pConversionDialog->SetConversionFormat( m_eConversionFormat );
                m_pConversionDialog->SetConversionDirectionState( m_bTryBothDirections, m_ePrimaryConversionDirection );

                // the handlers
                m_pConversionDialog->SetOptionsChangedHdl( LINK( this, HangulHanjaConversion_Impl, OnOptionsChanged ) );
                m_pConversionDialog->SetIgnoreHdl( LINK( this, HangulHanjaConversion_Impl, OnIgnore ) );
                m_pConversionDialog->SetIgnoreAllHdl( LINK( this, HangulHanjaConversion_Impl, OnIgnoreAll ) );
                m_pConversionDialog->SetChangeHdl( LINK( this, HangulHanjaConversion_Impl, OnChange ) );
                m_pConversionDialog->SetChangeAllHdl( LINK( this, HangulHanjaConversion_Impl, OnChangeAll ) );
                m_pConversionDialog->SetClickByCharacterHdl( LINK( this, HangulHanjaConversion_Impl, OnByCharClicked ) );
                m_pConversionDialog->SetConversionFormatChangedHdl( LINK( this, HangulHanjaConversion_Impl, OnConversionTypeChanged ) );
                m_pConversionDialog->SetFindHdl( LINK( this, HangulHanjaConversion_Impl, OnFind ) );
            }
        }
    }

    //-------------------------------------------------------------------------
    sal_Int16 HangulHanjaConversion_Impl::implGetConversionType( bool bSwitchDirection ) const
    {
        sal_Int16 nConversionType = -1;
        if (m_eConvType == HHC::eConvHangulHanja)
            nConversionType = HHC::eHangulToHanja == ( m_ePrimaryConversionDirection && !bSwitchDirection ) ? TO_HANJA : TO_HANGUL;
        else if (m_eConvType == HHC::eConvSimplifiedTraditional)
            nConversionType = LANGUAGE_CHINESE_SIMPLIFIED == m_nTargetLang ? TO_SCHINESE : TO_TCHINESE;
        DBG_ASSERT( nConversionType != -1, "unexpected conversion type" );
        return nConversionType;
    }

    //-------------------------------------------------------------------------
    bool HangulHanjaConversion_Impl::implUpdateSuggestions( bool _bAllowSearchNextConvertibleText, const sal_Int32 _nStartAt )
    {
        // parameters for the converter
        sal_Int32 nStartSearch = m_nCurrentStartIndex;
        if( _bAllowSearchNextConvertibleText )
            nStartSearch = _nStartAt;

        sal_Int32 nLength = m_sCurrentPortion.getLength() - nStartSearch;
        sal_Int16 nConversionType = implGetConversionType();
        sal_Int32 nConversionOption = IsByCharacter() ? CHARACTER_BY_CHARACTER : NONE;
        if( m_bIgnorePostPositionalWord )
            nConversionOption = nConversionOption | IGNORE_POST_POSITIONAL_WORD;

        sal_Bool bFoundAny = sal_True;
        try
        {
            TextConversionResult aResult = m_xConverter->getConversions(
                m_sCurrentPortion,
                nStartSearch,
                nLength,
                m_aSourceLocale,
                nConversionType,
                nConversionOption
            );
            sal_Bool bFoundPrimary = aResult.Boundary.startPos < aResult.Boundary.endPos;
            bFoundAny = bFoundPrimary;

            if ( m_bTryBothDirections )
            {   // see if we find another convertible when assuming the other direction
                TextConversionResult aSecondResult = m_xConverter->getConversions(
                    m_sCurrentPortion,
                    nStartSearch,
                    nLength,
                    m_aSourceLocale,
                    implGetConversionType( true ), // switched!
                    nConversionOption
                );
                if ( aSecondResult.Boundary.startPos < aSecondResult.Boundary.endPos )
                {   // we indeed found such a convertible

                    // in case the first attempt (with the original conversion direction)
                    // didn't find anything
                    if  (   !bFoundPrimary
                        // or if the second location is _before_ the first one
                        ||  ( aSecondResult.Boundary.startPos < aResult.Boundary.startPos )
                        )
                    {
                        // then use the second finding
                        aResult = aSecondResult;

                        // our current conversion direction changed now
                        m_eCurrentConversionDirection = ( HHC::eHangulToHanja == m_ePrimaryConversionDirection )
                            ? HHC::eHanjaToHangul : HHC::eHangulToHanja;
                        bFoundAny = sal_True;
                    }
                }
            }

            if( _bAllowSearchNextConvertibleText )
            {
                //this might change the current position
                m_aCurrentSuggestions = aResult.Candidates;
                m_nCurrentStartIndex = aResult.Boundary.startPos;
                m_nCurrentEndIndex = aResult.Boundary.endPos;
            }
            else
            {
                //the change of starting position is not allowed
                if( m_nCurrentStartIndex == aResult.Boundary.startPos
                    && aResult.Boundary.endPos != aResult.Boundary.startPos )
                {
                    m_aCurrentSuggestions = aResult.Candidates;
                    m_nCurrentEndIndex = aResult.Boundary.endPos;
                }
                else
                {
                    m_aCurrentSuggestions.realloc( 0 );
                    if( m_sCurrentPortion.getLength() >= m_nCurrentStartIndex+1 )
                        m_nCurrentEndIndex = m_nCurrentStartIndex+1;
                }
            }

            //put recently used string to front:
            if( m_bShowRecentlyUsedFirst && m_aCurrentSuggestions.getLength()>1 )
            {
                ::rtl::OUString sCurrentUnit( GetCurrentUnit() );
                StringMap::const_iterator aRecentlyUsed = m_aRecentlyUsedList.find( sCurrentUnit );
                bool bUsedBefore = aRecentlyUsed != m_aRecentlyUsedList.end();
                if( bUsedBefore && m_aCurrentSuggestions[0] != aRecentlyUsed->second )
                {
                    sal_Int32 nCount = m_aCurrentSuggestions.getLength();
                    Sequence< ::rtl::OUString > aTmp(nCount);
                    aTmp[0]=aRecentlyUsed->second;
                    sal_Int32 nDiff = 1;
                    for( sal_Int32 n=1; n<nCount; n++)//we had 0 already
                    {
                        if( nDiff && m_aCurrentSuggestions[n-nDiff]==aRecentlyUsed->second )
                            nDiff=0;
                        aTmp[n]=m_aCurrentSuggestions[n-nDiff];
                    }
                    m_aCurrentSuggestions = aTmp;
                }
            }
        }
        catch( const Exception& e )
        {
            e;  // make compiler happy
            DBG_ERROR( "HangulHanjaConversion_Impl::implNextConvertibleUnit: caught an exception!" );

            //!!! at least we want to move on in the text in order
            //!!! to avoid an endless loop...
            return false;
        }
        return bFoundAny;
    }

    //-------------------------------------------------------------------------
    bool HangulHanjaConversion_Impl::implNextConvertibleUnit( const sal_Int32 _nStartAt )
    {
        m_aCurrentSuggestions.realloc( 0 );

        // ask the TextConversion service for the next convertible piece of text
        sal_Int32 nStartLookupAt = _nStartAt;

        // get current primary conversion direction to use (if set)
        if (m_pAntiImpl->IsUseSavedConversionDirectionState())
            m_ePrimaryConversionDirection = m_pAntiImpl->m_ePrimaryConversionDirectionSave;
        else if (m_pConversionDialog && !m_pConversionDialog->GetUseBothDirections())
        {
            m_ePrimaryConversionDirection
                    = m_pConversionDialog->GetDirection( m_ePrimaryConversionDirection );
        }
        // save curently used value for possible later use
        m_pAntiImpl->m_ePrimaryConversionDirectionSave = m_ePrimaryConversionDirection;

        if (m_eConvType == HHC::eConvHangulHanja) // for eConvSimplifiedTraditional: there is only a single direction (specified by m_nTargetLang)
        {
            if (m_pAntiImpl->IsUseSavedConversionDirectionState())
                m_bTryBothDirections = m_pAntiImpl->m_bTryBothDirectionsSave;
            else
                // if the dialog does not yet exist we'll try both conversion directions
                m_bTryBothDirections = m_pConversionDialog ? m_pConversionDialog->GetUseBothDirections() : sal_True;
            // save curently used value for possible later use
            m_pAntiImpl->m_bTryBothDirectionsSave = m_bTryBothDirections;
        }

        // until we know better, assume that this very conversion attempt will end up with
        // the conversion direction which is our primary direction
        m_eCurrentConversionDirection = m_ePrimaryConversionDirection;

        bool bFoundAny = implUpdateSuggestions( true, _nStartAt );

        return  bFoundAny &&
                (m_nCurrentStartIndex < m_sCurrentPortion.getLength());
    }

    //-------------------------------------------------------------------------
    bool HangulHanjaConversion_Impl::implRetrieveNextPortion( )
    {
        m_sCurrentPortion = OUString();
        m_pAntiImpl->GetNextPortion( m_sCurrentPortion );
        m_nReplacementBaseIndex = 0;
        m_nCurrentStartIndex = m_nCurrentEndIndex = 0;

        return 0 != m_sCurrentPortion.getLength();
    }

    //-------------------------------------------------------------------------
    sal_Bool HangulHanjaConversion_Impl::implNextConvertible( bool _bRepeatUnit )
    {
        if ( _bRepeatUnit || ( m_nCurrentEndIndex < m_sCurrentPortion.getLength() ) )
        {
            if ( implNextConvertibleUnit(
                        _bRepeatUnit
                    ?   ( IsByCharacter() ? m_nCurrentStartIndex : m_nCurrentStartIndex )
                    :   m_nCurrentEndIndex
                ) )
                return sal_True;
        }

        // no convertible text in the current portion anymore
        // -> advance to the next portion
        do
        {
            // next portion
            if ( implRetrieveNextPortion( ) )
            {   // there is a next portion
                // -> find the next convertible unit in the current portion
                if ( implNextConvertibleUnit( 0 ) )
                    return sal_True;
            }
        }
        while ( m_sCurrentPortion.getLength() );

        // no more portions
        return sal_False;
    }

    //-------------------------------------------------------------------------
    ::rtl::OUString HangulHanjaConversion_Impl::GetCurrentUnit() const
    {
        DBG_ASSERT( m_nCurrentStartIndex < m_sCurrentPortion.getLength(),
            "HangulHanjaConversion_Impl::GetCurrentUnit: invalid index into current portion!" );
        DBG_ASSERT( m_nCurrentEndIndex <= m_sCurrentPortion.getLength(),
            "HangulHanjaConversion_Impl::GetCurrentUnit: invalid index into current portion!" );
        DBG_ASSERT( m_nCurrentStartIndex <= m_nCurrentEndIndex,
            "HangulHanjaConversion_Impl::GetCurrentUnit: invalid interval!" );

        ::rtl::OUString sCurrentUnit = m_sCurrentPortion.copy( m_nCurrentStartIndex, m_nCurrentEndIndex - m_nCurrentStartIndex );
        return sCurrentUnit;
    }

    //-------------------------------------------------------------------------
    sal_Bool HangulHanjaConversion_Impl::ContinueConversion( bool _bRepeatCurrentUnit )
    {
        sal_Bool bNeedUserInteraction = sal_False;  // when we leave here, do we need user interaction?
        sal_Bool bDocumentDone = sal_False;         // did we already check the whole document?

        while ( !bDocumentDone && !bNeedUserInteraction && implNextConvertible( _bRepeatCurrentUnit ) )
        {
            ::rtl::OUString sCurrentUnit( GetCurrentUnit() );

            // do we need to ignore it?
            sal_Bool bAlwaysIgnoreThis = m_sIgnoreList.end() != m_sIgnoreList.find( sCurrentUnit );

            // do we need to change it?
            StringMap::const_iterator aChangeListPos = m_aChangeList.find( sCurrentUnit );
            sal_Bool bAlwaysChangeThis = m_aChangeList.end() != aChangeListPos;

            // do we automatically change this?
            sal_Bool bAutoChange = m_bAutoReplaceUnique && m_aCurrentSuggestions.getLength() == 1;

            if (!m_bIsInteractive)
            {
                // silent conversion (e.g. for simplified/traditional Chinese)...
                if(m_aCurrentSuggestions.getLength()>0)
                    implChange( m_aCurrentSuggestions.getConstArray()[0] );
            }
            else if (bAutoChange)
            {
                implChange( m_aCurrentSuggestions.getConstArray()[0] );
            }
            else if ( bAlwaysChangeThis )
            {
                implChange( aChangeListPos->second );
            }
            else if ( !bAlwaysIgnoreThis )
            {
                // here we need to ask the user for what to do with the text
                // for this, allow derivees to highlight the current text unit in a possible document view
                m_pAntiImpl->HandleNewUnit( m_nCurrentStartIndex - m_nReplacementBaseIndex, m_nCurrentEndIndex - m_nReplacementBaseIndex );

                DBG_ASSERT( m_pConversionDialog, "we should always have a dialog here!" );
                if( m_pConversionDialog )
                    m_pConversionDialog->SetCurrentString( sCurrentUnit, m_aCurrentSuggestions );

                // do not look for the next convertible: We have to wait for the user to interactivly
                // decide what happens with the current convertible
                bNeedUserInteraction = sal_True;
            }
        }

        /*
        if ( bDocumentDone )
            return sal_True;            // we explicitly know that the complete document is done
        else if ( bNeedUserInteraction )
            return sal_False;           // the doc is not done, we found a convertible, but need the user to decide
        else
            return sal_True;            // we did not find a next convertible, so the document is implicitly done
        */

        return  bDocumentDone || !bNeedUserInteraction;
    }

    //-------------------------------------------------------------------------
    bool HangulHanjaConversion_Impl::implDeterminePrimaryDirection( )
    {
        // - For eConvHangulHanja the primary direction is determined by
        // the first encountered Korean character.
        // - For eConvSimplifiedTraditional the conversion direction
        // is already specified by the source language.

        bool bSuccess = true;

        if (m_eConvType == HHC::eHangulToHanja)
        {
            m_ePrimaryConversionDirection = HHC::eHangulToHanja;    // default

            bSuccess = false;
            try
            {
                // get the break iterator service
                OUString sBreakIteratorService( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.i18n.BreakIterator" ) );
                Reference< XInterface > xBI( m_xORB->createInstance( OUString( sBreakIteratorService ) ) );
                Reference< XBreakIterator > xBreakIter( xBI, UNO_QUERY );
                if ( !xBreakIter.is() )
                {
                    ShowServiceNotAvailableError( m_pUIParent, sBreakIteratorService, sal_True );
                }
                else
                {
                    sal_Int32 nNextAsianScript = xBreakIter->beginOfScript( m_sCurrentPortion, m_nCurrentStartIndex, ScriptType::ASIAN );
                    if ( -1 == nNextAsianScript )
                        nNextAsianScript = xBreakIter->nextScript( m_sCurrentPortion, m_nCurrentStartIndex, ScriptType::ASIAN );
                    if ( ( nNextAsianScript >= m_nCurrentStartIndex ) && ( nNextAsianScript < m_sCurrentPortion.getLength() ) )
                    {   // found asian text

                        // determine if it's Hangul
                        CharClass aCharClassificaton( m_xORB, m_aSourceLocale );
                        sal_Int16 nScript = aCharClassificaton.getScript( m_sCurrentPortion, nNextAsianScript );
                        if  (   ( UnicodeScript_kHangulJamo == nScript )
                            ||  ( UnicodeScript_kHangulCompatibilityJamo == nScript )
                            ||  ( UnicodeScript_kHangulSyllable == nScript )
                            )
                        {
                            m_ePrimaryConversionDirection = HHC::eHangulToHanja;
                        }
                        else
                        {
                            m_ePrimaryConversionDirection = HHC::eHanjaToHangul;
                        }

                        bSuccess = true;
                    }
                }
            }
            catch( const Exception& e )
            {
                e;  // make compiler happy
                DBG_ERROR( "HangulHanjaConversion_Impl::implDeterminePrimaryDirection: caught an exception!" );
            }
        }

        return bSuccess;
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion_Impl::DoDocumentConversion( )
    {
        // clear the change-all list - it's to be re-initialized for every single document
        {
            StringMap aEmpty;
            m_aChangeList.swap( aEmpty );
        }

        // first of all, we need to guess the direction of our conversion - it is determined by the first
        // hangul or hanja character in the first text
        if ( !implRetrieveNextPortion() )
        {
            DBG_WARNING( "HangulHanjaConversion_Impl::DoDocumentConversion: why did you call me if you do have nothing to convert?" );
            // nothing to do
            return;
        }
        if ( !implDeterminePrimaryDirection( ) )
            // something went wrong, has already been asserted
            return;


        if (m_bIsInteractive  &&  m_eConvType == HHC::eConvHangulHanja)
        {
            //always open dialog if at least having a hangul or hanja text portion
            createDialog();
            implUpdateData();
            m_pConversionDialog->Execute();
            DELETEZ( m_pConversionDialog );
        }
        else
        {
#ifdef DBG_UTIL
            sal_Bool bCompletelyDone =
#endif
            ContinueConversion( sal_False );
            DBG_ASSERT( bCompletelyDone, "HangulHanjaConversion_Impl::DoDocumentConversion: ContinueConversion should have returned true here!" );
        }
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion_Impl::implProceed( bool _bRepeatCurrentUnit )
    {
        if ( ContinueConversion( _bRepeatCurrentUnit ) )
        {   // we're done with the whole document
            DBG_ASSERT( !m_bIsInteractive || m_pConversionDialog, "HangulHanjaConversion_Impl::implProceed: we should not reach this here without dialog!" );
            if ( m_pConversionDialog )
                m_pConversionDialog->EndDialog( RET_OK );
        }
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion_Impl::implChange( const OUString& _rChangeInto )
    {
        if( !_rChangeInto.getLength() )
            return;

        // translate the conversion format into a replacement action
        // this translation depends on whether we have a Hangul original, or a Hanja original

        HHC::ReplacementAction eAction( HHC::eExchange );

        if (m_eConvType == HHC::eConvHangulHanja)
        {
            // is the original we're about to change in Hangul?
            sal_Bool bOriginalIsHangul = HHC::eHangulToHanja == m_eCurrentConversionDirection;

            switch ( m_eConversionFormat )
            {
                case HHC::eSimpleConversion: eAction = HHC::eExchange; break;
                case HHC::eHangulBracketed:  eAction = bOriginalIsHangul ? HHC::eOriginalBracketed : HHC::eReplacementBracketed; break;
                case HHC::eHanjaBracketed:   eAction = bOriginalIsHangul ? HHC::eReplacementBracketed : HHC::eOriginalBracketed; break;
                case HHC::eRubyHanjaAbove:   eAction = bOriginalIsHangul ? HHC::eReplacementAbove : HHC::eOriginalAbove; break;
                case HHC::eRubyHanjaBelow:   eAction = bOriginalIsHangul ? HHC::eReplacementBelow : HHC::eOriginalBelow; break;
                case HHC::eRubyHangulAbove:  eAction = bOriginalIsHangul ? HHC::eOriginalAbove : HHC::eReplacementAbove; break;
                case HHC::eRubyHangulBelow:  eAction = bOriginalIsHangul ? HHC::eOriginalBelow : HHC::eReplacementBelow; break;
                default:
                    DBG_ERROR( "HangulHanjaConversion_Impl::implChange: invalid/unexpected conversion format!" );
            }
        }

        // the proper indicies (the wrapper implementation needs indicies relative to the
        // previous replacement)
        DBG_ASSERT( ( m_nReplacementBaseIndex <= m_nCurrentStartIndex ) && ( m_nReplacementBaseIndex <= m_nCurrentEndIndex ),
            "HangulHanjaConversion_Impl::implChange: invalid replacement base!" );

        sal_Int32 nStartIndex = m_nCurrentStartIndex - m_nReplacementBaseIndex;
        sal_Int32 nEndIndex = m_nCurrentEndIndex - m_nReplacementBaseIndex;

        //remind this decision
        m_aRecentlyUsedList[ GetCurrentUnit() ] = _rChangeInto;

        // do the replacement
        m_pAntiImpl->ReplaceUnit( nStartIndex, nEndIndex, _rChangeInto, eAction );


        // adjust the replacement base
        m_nReplacementBaseIndex = m_nCurrentEndIndex;
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion_Impl::implReadOptionsFromConfiguration()
    {
        SvtLinguConfig  aLngCfg;
        aLngCfg.GetProperty( UPH_IS_IGNORE_POST_POSITIONAL_WORD ) >>= m_bIgnorePostPositionalWord;
        aLngCfg.GetProperty( UPH_IS_SHOW_ENTRIES_RECENTLY_USED_FIRST ) >>= m_bShowRecentlyUsedFirst;
        aLngCfg.GetProperty( UPH_IS_AUTO_REPLACE_UNIQUE_ENTRIES ) >>= m_bAutoReplaceUnique;
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion_Impl::implUpdateData()
    {
        implReadOptionsFromConfiguration();
        implUpdateSuggestions();

        if(m_pConversionDialog)
        {
            ::rtl::OUString sCurrentUnit( GetCurrentUnit() );

            m_pConversionDialog->SetCurrentString( sCurrentUnit, m_aCurrentSuggestions );
            m_pConversionDialog->FocusSuggestion();
        }

        m_pAntiImpl->HandleNewUnit( m_nCurrentStartIndex - m_nReplacementBaseIndex, m_nCurrentEndIndex - m_nReplacementBaseIndex );
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnOptionsChanged, void*, NOTINTERESTEDIN )
    {
        //options and dictionaries might have been changed
        //-> update our internal settings and the dialog
        implUpdateData();

        return 0L;
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnIgnore, void*, NOTINTERESTEDIN )
    {
        // simply ignore, and proceed
        implProceed( sal_False );
        return 0L;
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnIgnoreAll, void*, NOTINTERESTEDIN )
    {
        DBG_ASSERT( m_pConversionDialog, "HangulHanjaConversion_Impl::OnIgnoreAll: no dialog! How this?" );

        if ( m_pConversionDialog )
        {
            String sCurrentUnit = m_pConversionDialog->GetCurrentString();
            DBG_ASSERT( m_sIgnoreList.end() == m_sIgnoreList.find( sCurrentUnit ),
                "HangulHanjaConversion_Impl, OnIgnoreAll: shouldn't this have been ignored before" );

            // put into the "ignore all" list
            m_sIgnoreList.insert( sCurrentUnit );

            // and proceed
            implProceed( sal_False );
        }

        return 0L;
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnChange, void*, NOTINTERESTEDIN )
    {
        // change
        DBG_ASSERT( m_pConversionDialog, "we should always have a dialog here!" );
        if( m_pConversionDialog )
            implChange( m_pConversionDialog->GetCurrentSuggestion( ) );
        // and proceed
        implProceed( sal_False );

        return 0L;
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnChangeAll, void*, NOTINTERESTEDIN )
    {
        DBG_ASSERT( m_pConversionDialog, "HangulHanjaConversion_Impl::OnChangeAll: no dialog! How this?" );
        if ( m_pConversionDialog )
        {
            OUString sCurrentUnit( m_pConversionDialog->GetCurrentString() );
            OUString sChangeInto( m_pConversionDialog->GetCurrentSuggestion( ) );

            if( sChangeInto.getLength() )
            {
                // change the current occurence
                implChange( sChangeInto );

                // put into the "change all" list
                m_aChangeList.insert( StringMap::value_type( sCurrentUnit, sChangeInto ) );
            }

            // and proceed
            implProceed( sal_False );
        }

        return 0L;
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnByCharClicked, CheckBox*, _pBox )
    {
        m_bByCharacter = _pBox->IsChecked();

        // continue conversion, without advancing to the next unit, but instead continuing with the current unit
        implProceed( sal_True );
        return 0L;
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnConversionTypeChanged, void*, NOTINTERESTEDIN )
    {
        DBG_ASSERT( m_pConversionDialog, "we should always have a dialog here!" );
        if( m_pConversionDialog )
            m_eConversionFormat = m_pConversionDialog->GetConversionFormat( );
        return 0L;
    }

    //-------------------------------------------------------------------------
    IMPL_LINK( HangulHanjaConversion_Impl, OnFind, void*, NOTINTERESTEDIN )
    {
        DBG_ASSERT( m_pConversionDialog, "HangulHanjaConversion_Impl::OnFind: where did this come from?" );
        if ( m_pConversionDialog )
        {
            try
            {
                OUString sNewOriginal( m_pConversionDialog->GetCurrentSuggestion( ) );
                Sequence< OUString > aSuggestions;

                DBG_ASSERT( m_xConverter.is(), "HangulHanjaConversion_Impl::OnFind: no converter!" );
                TextConversionResult aToHanja = m_xConverter->getConversions(
                    sNewOriginal,
                    0, sNewOriginal.getLength(),
                    m_aSourceLocale,
                    TextConversionType::TO_HANJA,
                    TextConversionOption::NONE
                );
                TextConversionResult aToHangul = m_xConverter->getConversions(
                    sNewOriginal,
                    0, sNewOriginal.getLength(),
                    m_aSourceLocale,
                    TextConversionType::TO_HANGUL,
                    TextConversionOption::NONE
                );

                bool bHaveToHanja = ( aToHanja.Boundary.startPos < aToHanja.Boundary.endPos );
                bool bHaveToHangul = ( aToHangul.Boundary.startPos < aToHangul.Boundary.endPos );

                TextConversionResult* pResult = NULL;
                if ( bHaveToHanja && bHaveToHangul )
                {   // it found convertibles in both directions -> use the first
                    if ( aToHangul.Boundary.startPos < aToHanja.Boundary.startPos )
                        pResult = &aToHangul;
                    else
                        pResult = &aToHanja;
                }
                else if ( bHaveToHanja )
                {   // only found toHanja
                    pResult = &aToHanja;
                }
                else
                {   // only found toHangul
                    pResult = &aToHangul;
                }
                if ( pResult )
                    aSuggestions = pResult->Candidates;

                m_pConversionDialog->SetCurrentString( sNewOriginal, aSuggestions, false );
                m_pConversionDialog->FocusSuggestion();
            }
            catch( const Exception& e )
            {
                e;  // make compiler happy
                DBG_ERROR( "HangulHanjaConversion_Impl::OnFind: caught an exception!" );
            }
        }
        return 0L;
    }

    //=========================================================================
    //= HangulHanjaConversion
    //=========================================================================
    //-------------------------------------------------------------------------

    // static member initialization
    sal_Bool    HangulHanjaConversion::m_bUseSavedValues        = sal_False;
    sal_Bool    HangulHanjaConversion::m_bTryBothDirectionsSave = sal_False;
    HHC::ConversionDirection HangulHanjaConversion::m_ePrimaryConversionDirectionSave   = HHC::eHangulToHanja;

    //-------------------------------------------------------------------------
    HangulHanjaConversion::HangulHanjaConversion( Window* _pUIParent,
        const Reference< XMultiServiceFactory >& _rxORB,
        const Locale& _rSourceLocale, const Locale& _rTargetLocale,
        const Font* _pTargetFont,
        sal_Int32 _nOptions, sal_Bool _bIsInteractive)
        :m_pImpl( new HangulHanjaConversion_Impl( _pUIParent, _rxORB, _rSourceLocale, _rTargetLocale, _pTargetFont, _nOptions, _bIsInteractive, this ) )
    {
    }

    //-------------------------------------------------------------------------
    HangulHanjaConversion::~HangulHanjaConversion( )
    {
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion::SetUseSavedConversionDirectionState( sal_Bool bVal )
    {
        m_bUseSavedValues = bVal;
    }

    //-------------------------------------------------------------------------
    sal_Bool HangulHanjaConversion::IsUseSavedConversionDirectionState()
    {
        return m_bUseSavedValues;
    }

    //-------------------------------------------------------------------------
    LanguageType HangulHanjaConversion::GetSourceLanguage( ) const
    {
        return m_pImpl->GetSourceLang();
    }

    //-------------------------------------------------------------------------
    LanguageType HangulHanjaConversion::GetTargetLanguage( ) const
    {
        return m_pImpl->GetTargetLang();
    }

    //-------------------------------------------------------------------------
    const Font * HangulHanjaConversion::GetTargetFont( ) const
    {
        return m_pImpl->GetTargetFont();
    }

    //-------------------------------------------------------------------------
    sal_Int32 HangulHanjaConversion::GetConversionOptions( ) const
    {
        return m_pImpl->GetConvOptions();
    }

    //-------------------------------------------------------------------------
    sal_Bool HangulHanjaConversion::IsInteractive( ) const
    {
        return m_pImpl->IsInteractive();
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion::HandleNewUnit( const sal_Int32 _nStartIndex, const sal_Int32 _nEndIndex )
    {
        // nothing to do, only derived classes need this.
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion::GetNextPortion( OUString& /* [out] */ _rNextPortion )
    {
        DBG_ERROR( "HangulHanjaConversion::GetNextPortion: to be overridden!" );
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion::ReplaceUnit( const sal_Int32 _nUnitStart, const sal_Int32 _nUnitEnd,
            const OUString& _rReplaceWith, ReplacementAction _eAction )
    {
        DBG_ERROR( "HangulHanjaConversion::ReplaceUnit: to be overridden!" );
    }

    //-------------------------------------------------------------------------
    sal_Bool HangulHanjaConversion::HasRubySupport() const
    {
        DBG_ERROR( "HangulHanjaConversion::HasRubySupport: to be overridden!" );
        return sal_False;
    }

    //-------------------------------------------------------------------------
    void HangulHanjaConversion::ConvertDocument()
    {
        if ( m_pImpl->IsValid() )
            m_pImpl->DoDocumentConversion( );
    }

//.............................................................................
}   // namespace svx
//.............................................................................
