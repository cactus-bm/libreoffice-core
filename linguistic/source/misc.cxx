/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: misc.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-25 12:24:38 $
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
#include "precompiled_linguistic.hxx"

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _FSYS_HXX
#include <tools/fsys.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
//#ifndef _RTL_USTRBUF_HXX_
//#include <rtl/ustrbuf.hxx>
//#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _SVTOOLS_LNGMISC_HXX_
#include <svtools/lngmisc.hxx>
#endif
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef INCLUDED_I18NPOOL_MSLANGID_HXX
#include <i18npool/mslangid.hxx>
#endif

#ifndef _COM_SUN_STAR_UCB_XCOMMANDENVIRONMENT_HPP_
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XFASTPROPERTYSET_HPP_
#include <com/sun/star/beans/XFastPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XTERMINATELISTENER_HPP_
#include <com/sun/star/frame/XTerminateListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif

#include <com/sun/star/beans/PropertyValues.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/linguistic2/XDictionary1.hpp>
#include <com/sun/star/linguistic2/DictionaryType.hpp>
#include <com/sun/star/linguistic2/XSearchableDictionaryList.hpp>
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif

#include "misc.hxx"
#include "defs.hxx"
#include "lngprops.hxx"
#include <hyphdta.hxx>


using namespace utl;
using namespace osl;
using namespace rtl;
using namespace com::sun::star;
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::i18n;
using namespace com::sun::star::linguistic2;

namespace linguistic
{

///////////////////////////////////////////////////////////////////////////

osl::Mutex &    GetLinguMutex()
{
    static osl::Mutex   aMutex;
    return aMutex;
}

///////////////////////////////////////////////////////////////////////////

LocaleDataWrapper & GetLocaleDataWrapper( INT16 nLang )
{
    static LocaleDataWrapper aLclDtaWrp(
                getProcessServiceFactory(),
                CreateLocale( Application::GetSettings().GetUILanguage() ) );

    const Locale &rLcl = aLclDtaWrp.getLoadedLocale();
    Locale aLcl( CreateLocale( nLang ) );
    if (aLcl.Language != rLcl.Language ||
        aLcl.Country  != rLcl.Country  ||
        aLcl.Variant  != rLcl.Variant)
        aLclDtaWrp.setLocale( aLcl );
    return aLclDtaWrp;
}

///////////////////////////////////////////////////////////////////////////

/**
    returns text-encoding used for ByteString unicode String conversion
 */
rtl_TextEncoding GetTextEncoding( INT16 nLanguage )
{
    DBG_ASSERT( nLanguage != LANGUAGE_NONE, "invalid language argument" )
    static INT16 nLastLanguage = LANGUAGE_NONE;

    // set default value for unknown languages
    static rtl_TextEncoding nEncoding = RTL_TEXTENCODING_DONTKNOW;

    if (nLastLanguage != nLanguage)
    {
        //!! IPR uses textencodings Latin-1, Latin-2, Latin-5 and Latin-7 !!

        nLastLanguage = nLanguage;
        switch (nLanguage)
        {
            case LANGUAGE_GERMAN :
            case LANGUAGE_GERMAN_SWISS :
            case LANGUAGE_ENGLISH_US :
            case LANGUAGE_ENGLISH_UK :
            case LANGUAGE_FRENCH :
            case LANGUAGE_ITALIAN :
            case LANGUAGE_SPANISH :
            case LANGUAGE_CATALAN :
            case LANGUAGE_PORTUGUESE :
            case LANGUAGE_PORTUGUESE_BRAZILIAN :
            case LANGUAGE_DANISH :
            case LANGUAGE_DUTCH :
            case LANGUAGE_SWEDISH :
            case LANGUAGE_FINNISH :
            case LANGUAGE_NORWEGIAN_BOKMAL :
            case LANGUAGE_NORWEGIAN_NYNORSK :
            case LANGUAGE_AFRIKAANS :
            case LANGUAGE_ENGLISH_EIRE :
            case LANGUAGE_ENGLISH_AUS :
#ifdef WNT
                    nEncoding = RTL_TEXTENCODING_MS_1252;   break;
#else
                    nEncoding = RTL_TEXTENCODING_ISO_8859_1;   break;
#endif
            case LANGUAGE_CZECH :
            case LANGUAGE_HUNGARIAN :
            case LANGUAGE_POLISH :
#ifdef WNT
                    nEncoding = RTL_TEXTENCODING_MS_1250;   break;
#else
                    nEncoding = RTL_TEXTENCODING_ISO_8859_2;   break;
#endif
            case LANGUAGE_RUSSIAN :
#ifdef WNT
                    nEncoding = RTL_TEXTENCODING_MS_1251;   break;
#else
                    nEncoding = RTL_TEXTENCODING_ISO_8859_5;   break;
#endif
            case LANGUAGE_GREEK :
#ifdef WNT
                    nEncoding = RTL_TEXTENCODING_MS_1253;   break;
#else
                    nEncoding = RTL_TEXTENCODING_ISO_8859_7;   break;
#endif
            default:
                    DBG_ERROR( "unexpected language" );
        }
    }

    return nEncoding;
}

///////////////////////////////////////////////////////////////////////////

static inline sal_Int32 Minimum( sal_Int32 n1, sal_Int32 n2, sal_Int32 n3 )
{
    sal_Int32 nMin = n1 < n2 ? n1 : n2;
    return nMin < n3 ? nMin : n3;
}

///////////////////////////////////////////////////////////////////////////

class IntArray2D
{
private:
    sal_Int32  *pData;
    int         n1, n2;

public:
    IntArray2D( int nDim1, int nDim2 );
    ~IntArray2D();

    sal_Int32 & Value( int i, int k  );
};

IntArray2D::IntArray2D( int nDim1, int nDim2 )
{
    n1 = nDim1;
    n2 = nDim2;
    pData = new sal_Int32[n1 * n2];
}

IntArray2D::~IntArray2D()
{
    delete[] pData;
}

sal_Int32 & IntArray2D::Value( int i, int k  )
{
    DBG_ASSERT( 0 <= i && i < n1, "first index out of range" );
    DBG_ASSERT( 0 <= k && k < n2, "first index out of range" );
    DBG_ASSERT( i * n2 + k < n1 * n2, "index out of range" );
    return pData[ i * n2 + k ];
}


sal_Int32 LevDistance( const OUString &rTxt1, const OUString &rTxt2 )
{
    sal_Int32 nLen1 = rTxt1.getLength();
    sal_Int32 nLen2 = rTxt2.getLength();

    if (nLen1 == 0)
        return nLen2;
    if (nLen2 == 0)
        return nLen1;

    IntArray2D aData( nLen1 + 1, nLen2 + 1 );

    sal_Int32 i, k;
    for (i = 0;  i <= nLen1;  ++i)
        aData.Value(i, 0) = i;
    for (k = 0;  k <= nLen2;  ++k)
        aData.Value(0, k) = k;
    for (i = 1;  i <= nLen1;  ++i)
    {
        for (k = 1;  k <= nLen2;  ++k)
        {
            sal_Unicode c1i = rTxt1.getStr()[i - 1];
            sal_Unicode c2k = rTxt2.getStr()[k - 1];
            sal_Int32 nCost = c1i == c2k ? 0 : 1;
            sal_Int32 nNew = Minimum( aData.Value(i-1, k  ) + 1,
                                       aData.Value(i  , k-1) + 1,
                                       aData.Value(i-1, k-1) + nCost );
            // take transposition (exchange with left or right char) in account
            if (2 < i && 2 < k)
            {
                int nT = aData.Value(i-2, k-2) + 1;
                if (rTxt1.getStr()[i - 2] != c1i)
                    ++nT;
                if (rTxt2.getStr()[k - 2] != c2k)
                    ++nT;
                if (nT < nNew)
                    nNew = nT;
            }

            aData.Value(i, k) = nNew;
        }
    }
    sal_Int32 nDist = aData.Value(nLen1, nLen2);
    return nDist;
 }

///////////////////////////////////////////////////////////////////////////

BOOL IsUseDicList( const PropertyValues &rProperties,
        const Reference< XPropertySet > &rxProp )
{
    BOOL bRes = TRUE;

    INT32 nLen = rProperties.getLength();
    const PropertyValue *pVal = rProperties.getConstArray();
    INT32 i;

    for ( i = 0;  i < nLen;  ++i)
    {
        if (UPH_IS_USE_DICTIONARY_LIST == pVal[i].Handle)
        {
            pVal[i].Value >>= bRes;
            break;
        }
    }
    if (i >= nLen)  // no temporary value found in 'rProperties'
    {
        Reference< XFastPropertySet > xFast( rxProp, UNO_QUERY );
        if (xFast.is())
            xFast->getFastPropertyValue( UPH_IS_USE_DICTIONARY_LIST ) >>= bRes;
    }

    return bRes;
}


BOOL IsIgnoreControlChars( const PropertyValues &rProperties,
        const Reference< XPropertySet > &rxProp )
{
    BOOL bRes = TRUE;

    INT32 nLen = rProperties.getLength();
    const PropertyValue *pVal = rProperties.getConstArray();
    INT32 i;

    for ( i = 0;  i < nLen;  ++i)
    {
        if (UPH_IS_IGNORE_CONTROL_CHARACTERS == pVal[i].Handle)
        {
            pVal[i].Value >>= bRes;
            break;
        }
    }
    if (i >= nLen)  // no temporary value found in 'rProperties'
    {
        Reference< XFastPropertySet > xFast( rxProp, UNO_QUERY );
        if (xFast.is())
            xFast->getFastPropertyValue( UPH_IS_IGNORE_CONTROL_CHARACTERS ) >>= bRes;
    }

    return bRes;
}


static BOOL lcl_HasHyphInfo( const Reference<XDictionaryEntry> &xEntry )
{
    BOOL bRes = FALSE;
    if (xEntry.is())
    {
        // there has to be (at least one) '=' denoting a hyphenation position
        // and it must not be before any character of the word
        sal_Int32 nIdx = xEntry->getDictionaryWord().indexOf( '=' );
        bRes = nIdx != -1  &&  nIdx != 0;
    }
    return bRes;
}


Reference< XDictionaryEntry > SearchDicList(
        const Reference< XDictionaryList > &xDicList,
        const OUString &rWord, INT16 nLanguage,
        BOOL bSearchPosDics, BOOL bSearchSpellEntry )
{
    MutexGuard  aGuard( GetLinguMutex() );

    Reference< XDictionaryEntry > xEntry;

    if (!xDicList.is())
        return xEntry;

    const uno::Sequence< Reference< XDictionary > >
            aDics( xDicList->getDictionaries() );
    const Reference< XDictionary >
            *pDic = aDics.getConstArray();
    INT32 nDics = xDicList->getCount();

    INT32 i;
    for (i = 0;  i < nDics;  i++)
    {
        Reference< XDictionary1 > axDic( pDic[i], UNO_QUERY );

        DictionaryType  eType = axDic->getDictionaryType();
        INT16           nLang = axDic->getLanguage();

        if ( axDic.is() && axDic->isActive()
            && (nLang == nLanguage  ||  nLang == LANGUAGE_NONE) )
        {
            DBG_ASSERT( eType != DictionaryType_MIXED,
                "lng : unexpected dictionary type" );

            if (   (!bSearchPosDics  &&  eType == DictionaryType_NEGATIVE)
                || ( bSearchPosDics  &&  eType == DictionaryType_POSITIVE))
            {
                if ( (xEntry = axDic->getEntry( rWord )).is() )
                {
                    if (bSearchSpellEntry || lcl_HasHyphInfo( xEntry ))
                        break;
                }
                xEntry = 0;
            }
        }
    }

    return xEntry;
}


///////////////////////////////////////////////////////////////////////////

LanguageType LocaleToLanguage( const Locale& rLocale )
{
    //  empty Locale -> LANGUAGE_NONE
    if ( rLocale.Language.getLength() == 0 )
        return LANGUAGE_NONE;

    return MsLangId::convertLocaleToLanguage( rLocale );
}


Locale& LanguageToLocale( Locale& rLocale, LanguageType eLang )
{
    if ( eLang != LANGUAGE_NONE /* &&  eLang != LANGUAGE_SYSTEM */)
        MsLangId::convertLanguageToLocale( eLang, rLocale );

    return rLocale;
}

Locale CreateLocale( LanguageType eLang )
{
    Locale aLocale;
    if ( eLang != LANGUAGE_NONE /* &&  eLang != LANGUAGE_SYSTEM */)
        return MsLangId::convertLanguageToLocale( eLang );

    return aLocale;
}

uno::Sequence< Locale > LangSeqToLocaleSeq( const uno::Sequence< INT16 > &rLangSeq )
{
    const INT16 *pLang = rLangSeq.getConstArray();
    INT32 nCount = rLangSeq.getLength();

    uno::Sequence< Locale > aLocales( nCount );
    Locale *pLocale = aLocales.getArray();
    for (INT32 i = 0;  i < nCount;  ++i)
    {
        LanguageToLocale( pLocale[i], pLang[ i ] );
    }

    return aLocales;
}

uno::Sequence< INT16 >
    LocaleSeqToLangSeq( uno::Sequence< Locale > &rLocaleSeq )
{
    const Locale *pLocale = rLocaleSeq.getConstArray();
    INT32 nCount = rLocaleSeq.getLength();

    uno::Sequence< INT16 >  aLangs( nCount );
    INT16 *pLang = aLangs.getArray();
    for (INT32 i = 0;  i < nCount;  ++i)
    {
        pLang[i] = LocaleToLanguage( pLocale[i] );

    }

    return aLangs;
}

///////////////////////////////////////////////////////////////////////////

BOOL    IsReadOnly( const String &rURL, BOOL *pbExist )
{
    BOOL bRes = FALSE;
    BOOL bExists = FALSE;

    if (rURL.Len() > 0)
    {
        try
        {
            Reference< ::com::sun::star::ucb::XCommandEnvironment > xCmdEnv;
            ::ucb::Content aContent( rURL, xCmdEnv );

            bExists = aContent.isDocument();
            if (bExists)
            {
                Any aAny( aContent.getPropertyValue( A2OU( "IsReadOnly" ) ) );
                aAny >>= bRes;
            }
        }
        catch (Exception &)
        {
            bRes = TRUE;
        }
    }

    if (pbExist)
        *pbExist = bExists;
    return bRes;
}

///////////////////////////////////////////////////////////////////////////


static BOOL GetAltSpelling( INT16 &rnChgPos, INT16 &rnChgLen, OUString &rRplc,
        Reference< XHyphenatedWord > &rxHyphWord )
{
    BOOL bRes = rxHyphWord->isAlternativeSpelling();
    if (bRes)
    {
        OUString aWord( rxHyphWord->getWord() ),
                 aHyphenatedWord( rxHyphWord->getHyphenatedWord() );
        INT16   nHyphenationPos     = rxHyphWord->getHyphenationPos();
        /*INT16   nHyphenPos          = rxHyphWord->getHyphenPos()*/;
        const sal_Unicode *pWord    = aWord.getStr(),
                          *pAltWord = aHyphenatedWord.getStr();

        // at least char changes directly left or right to the hyphen
        // should(!) be handled properly...
        //! nHyphenationPos and nHyphenPos differ at most by 1 (see above)
        //! Beware: eg "Schiffahrt" in German (pre spelling reform)
        //! proves to be a bit nasty (nChgPosLeft and nChgPosRight overlap
        //! to an extend.)

        // find first different char from left
        sal_Int32   nPosL    = 0,
                    nAltPosL = 0;
        for (INT16 i = 0 ;  pWord[ nPosL ] == pAltWord[ nAltPosL ];  nPosL++, nAltPosL++, i++)
        {
            // restrict changes area beginning to the right to
            // the char immediately following the hyphen.
            //! serves to insert the additional "f" in "Schiffahrt" at
            //! position 5 rather than position 6.
            if (i >= nHyphenationPos + 1)
                break;
        }

        // find first different char from right
        sal_Int32   nPosR    = aWord.getLength() - 1,
                    nAltPosR = aHyphenatedWord.getLength() - 1;
        for ( ;  nPosR >= nPosL  &&  nAltPosR >= nAltPosL
                    &&  pWord[ nPosR ] == pAltWord[ nAltPosR ];
                nPosR--, nAltPosR--)
            ;

        rnChgPos = sal::static_int_cast< INT16 >(nPosL);
        rnChgLen = sal::static_int_cast< INT16 >(nPosR - nPosL + 1);
        DBG_ASSERT( rnChgLen >= 0, "nChgLen < 0");

        sal_Int32 nTxtStart = nPosL;
        sal_Int32 nTxtLen   = nAltPosL - nPosL + 1;
        rRplc = aHyphenatedWord.copy( nTxtStart, nTxtLen );
    }
    return bRes;
}


static INT16 GetOrigWordPos( const OUString &rOrigWord, INT16 nPos )
{
    INT32 nLen = rOrigWord.getLength();
    INT32 i = -1;
    while (nPos >= 0  &&  i++ < nLen)
    {
        sal_Unicode cChar = rOrigWord[i];
        BOOL bSkip = IsHyphen( cChar ) || IsControlChar( cChar );
        if (!bSkip)
            --nPos;
    }
    return sal::static_int_cast< INT16 >((0 <= i  &&  i < nLen) ? i : -1);
}


INT32 GetPosInWordToCheck( const OUString &rTxt, INT32 nPos )
{
    INT32 nRes = -1;
    INT32 nLen = rTxt.getLength();
    if (0 <= nPos  &&  nPos < nLen)
    {
        nRes = 0;
        for (INT32 i = 0;  i < nPos;  ++i)
        {
            sal_Unicode cChar = rTxt[i];
            BOOL bSkip = IsHyphen( cChar ) || IsControlChar( cChar );
            if (!bSkip)
                ++nRes;
        }
    }
    return nRes;
}


Reference< XHyphenatedWord > RebuildHyphensAndControlChars(
        const OUString &rOrigWord,
        Reference< XHyphenatedWord > &rxHyphWord )
{
    Reference< XHyphenatedWord > xRes;
    if (rOrigWord.getLength() && rxHyphWord.is())
    {
        INT16    nChgPos = 0,
                 nChgLen = 0;
        OUString aRplc;
        BOOL bAltSpelling = GetAltSpelling( nChgPos, nChgLen, aRplc, rxHyphWord );
#if OSL_DEBUG_LEVEL > 1
        OUString aWord( rxHyphWord->getWord() );
#endif

        OUString aOrigHyphenatedWord;
        INT16 nOrigHyphenPos        = -1;
        INT16 nOrigHyphenationPos   = -1;
        if (!bAltSpelling)
        {
            aOrigHyphenatedWord = rOrigWord;
            nOrigHyphenPos      = GetOrigWordPos( rOrigWord, rxHyphWord->getHyphenPos() );
            nOrigHyphenationPos = GetOrigWordPos( rOrigWord, rxHyphWord->getHyphenationPos() );
        }
        else
        {
            //! should at least work with the German words
            //! B�-c-k-er and Sc-hif-fah-rt

            OUString aLeft, aRight;
            INT16 nPos = GetOrigWordPos( rOrigWord, nChgPos );

            // get words like Sc-hif-fah-rt to work correct
            INT16 nHyphenationPos = rxHyphWord->getHyphenationPos();
            if (nChgPos > nHyphenationPos)
                --nPos;

            aLeft = rOrigWord.copy( 0, nPos );
            aRight = rOrigWord.copy( nPos + nChgLen );

            aOrigHyphenatedWord =  aLeft;
            aOrigHyphenatedWord += aRplc;
            aOrigHyphenatedWord += aRight;

            nOrigHyphenPos      = sal::static_int_cast< INT16 >(aLeft.getLength() +
                                  rxHyphWord->getHyphenPos() - nChgPos);
            nOrigHyphenationPos = GetOrigWordPos( rOrigWord, nHyphenationPos );
        }

        if (nOrigHyphenPos == -1  ||  nOrigHyphenationPos == -1)
        {
            DBG_ERROR( "failed to get nOrigHyphenPos or nOrigHyphenationPos" );
        }
        else
        {
            INT16 nLang = LocaleToLanguage( rxHyphWord->getLocale() );
            xRes = new HyphenatedWord(
                        rOrigWord, nLang, nOrigHyphenationPos,
                        aOrigHyphenatedWord, nOrigHyphenPos );
        }

    }
    return xRes;
}


///////////////////////////////////////////////////////////////////////////


static CharClass & lcl_GetCharClass()
{
    static CharClass aCC( CreateLocale( LANGUAGE_ENGLISH_US ) );
    return aCC;
}


osl::Mutex & lcl_GetCharClassMutex()
{
    static osl::Mutex   aMutex;
    return aMutex;
}


BOOL IsUpper( const String &rText, xub_StrLen nPos, xub_StrLen nLen, INT16 nLanguage )
{
    MutexGuard  aGuard( lcl_GetCharClassMutex() );

    CharClass &rCC = lcl_GetCharClass();
    rCC.setLocale( CreateLocale( nLanguage ) );
    sal_Int32 nFlags = rCC.getStringType( rText, nPos, nLen );
    return      (nFlags & KCharacterType::UPPER)
            && !(nFlags & KCharacterType::LOWER);
}


BOOL IsLower( const String &rText, xub_StrLen nPos, xub_StrLen nLen, INT16 nLanguage )
{
    MutexGuard  aGuard( lcl_GetCharClassMutex() );

    CharClass &rCC = lcl_GetCharClass();
    rCC.setLocale( CreateLocale( nLanguage ) );
    sal_Int32 nFlags = rCC.getStringType( rText, nPos, nLen );
    return      (nFlags & KCharacterType::LOWER)
            && !(nFlags & KCharacterType::UPPER);
}


String ToLower( const String &rText, INT16 nLanguage )
{
    MutexGuard  aGuard( lcl_GetCharClassMutex() );

    CharClass &rCC = lcl_GetCharClass();
    rCC.setLocale( CreateLocale( nLanguage ) );
    return rCC.lower( rText );
}


String ToUpper( const String &rText, INT16 nLanguage )
{
    MutexGuard  aGuard( lcl_GetCharClassMutex() );

    CharClass &rCC = lcl_GetCharClass();
    rCC.setLocale( CreateLocale( nLanguage ) );
    return rCC.upper( rText );
}


String ToTitle( const String &rText, INT16 nLanguage )
{
    MutexGuard  aGuard( lcl_GetCharClassMutex() );

    CharClass &rCC = lcl_GetCharClass();
    rCC.setLocale( CreateLocale( nLanguage ) );
    return rCC.toTitle( rText, 0, rText.Len() );
}


sal_Unicode ToLower( const sal_Unicode cChar, INT16 nLanguage )
{
    MutexGuard  aGuard( lcl_GetCharClassMutex() );

    CharClass &rCC = lcl_GetCharClass();
    rCC.setLocale( CreateLocale( nLanguage ) );
    return rCC.lower( cChar ).GetChar(0);
}


sal_Unicode ToUpper( const sal_Unicode cChar, INT16 nLanguage )
{
    MutexGuard  aGuard( lcl_GetCharClassMutex() );

    CharClass &rCC = lcl_GetCharClass();
    rCC.setLocale( CreateLocale( nLanguage ) );
    return rCC.upper( cChar ).GetChar(0);
}


BOOL HasDigits( const String &rText )
{
    xub_StrLen nLen = rText.Len();

    xub_StrLen i = 0;
    while (i < nLen)
    {
        sal_Unicode cChar = rText.GetChar( i++ );
        if ((sal_Unicode)'0' <= cChar  &&  cChar <= (sal_Unicode)'9')
            return TRUE;
    }
    return FALSE;
}


BOOL IsNumeric( const String &rText )
{
    BOOL bRes = FALSE;
    xub_StrLen nLen = rText.Len();
    if (nLen)
    {
        bRes = TRUE;
        xub_StrLen i = 0;
        while (i < nLen)
        {
            sal_Unicode cChar = rText.GetChar( i++ );
            if ( !((sal_Unicode)'0' <= cChar  &&  cChar <= (sal_Unicode)'9') )
            {
                bRes = FALSE;
                break;
            }
        }
    }
    return bRes;
}


///////////////////////////////////////////////////////////////////////////

Reference< XInterface > GetOneInstanceService( const char *pServiceName )
{
    Reference< XInterface > xRef;

    if (pServiceName)
    {
        Reference< XMultiServiceFactory >  xMgr( getProcessServiceFactory() );
        if (xMgr.is())
        {
            xRef = xMgr->createInstance( A2OU( pServiceName ) );
        }
    }

    return xRef;
}

Reference< XPropertySet > GetLinguProperties()
{
    return Reference< XPropertySet > (
        GetOneInstanceService( SN_LINGU_PROPERTIES ), UNO_QUERY );
}

Reference< XSearchableDictionaryList > GetSearchableDictionaryList()
{
    return Reference< XSearchableDictionaryList > (
        GetOneInstanceService( SN_DICTIONARY_LIST ), UNO_QUERY );
}

Reference< XDictionaryList > GetDictionaryList()
{
    return Reference< XDictionaryList > (
        GetOneInstanceService( SN_DICTIONARY_LIST ), UNO_QUERY );
}

///////////////////////////////////////////////////////////////////////////

AppExitListener::AppExitListener()
{
    // add object to Desktop EventListeners in order to properly call
    // the AtExit function at appliction exit.
    Reference< XMultiServiceFactory >
        xMgr = getProcessServiceFactory();

    if (xMgr.is())
    {
        xDesktop = Reference< frame::XDesktop >(
                xMgr->createInstance( A2OU( SN_DESKTOP ) ), UNO_QUERY );
    }
}

AppExitListener::~AppExitListener()
{
}


void AppExitListener::Activate()
{
    if (xDesktop.is())
        xDesktop->addTerminateListener( this );
}


void AppExitListener::Deactivate()
{
    if (xDesktop.is())
        xDesktop->removeTerminateListener( this );
}


void SAL_CALL
    AppExitListener::disposing( const EventObject& rEvtSource )
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );

    if (xDesktop.is()  &&  rEvtSource.Source == xDesktop)
    {
        xDesktop = NULL;    //! release reference to desktop
    }
}


void SAL_CALL
    AppExitListener::queryTermination( const EventObject& /*rEvtSource*/ )
        throw(frame::TerminationVetoException, RuntimeException)
{
    //MutexGuard    aGuard( GetLinguMutex() );
}


void SAL_CALL
    AppExitListener::notifyTermination( const EventObject& rEvtSource )
        throw(RuntimeException)
{
    MutexGuard  aGuard( GetLinguMutex() );

    if (xDesktop.is()  &&  rEvtSource.Source == xDesktop)
    {
        AtExit();
    }
}

///////////////////////////////////////////////////////////////////////////

}   // namespace linguistic

