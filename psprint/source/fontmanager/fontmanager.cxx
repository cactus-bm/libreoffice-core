/*************************************************************************
 *
 *  $RCSfile: fontmanager.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: pl $ $Date: 2001-05-09 10:51:35 $
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

#define Window XLIB_Window
#include <X11/Xlib.h>
#undef Window
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif
#ifndef _UTL_ATOM_HXX_
#include <unotools/atom.hxx>
#endif
#ifndef _PSPRINT_FONTMANAGER_HXX_
#include <psprint/fontmanager.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _PSPRINT_HELPER_HXX_
#include <psprint/helper.hxx>
#endif
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _PSPRINT_STRHELPER_HXX_
#include <psprint/strhelper.hxx>
#endif
#ifndef _PSPRINT_PPDPARSER_HXX_
#include <psprint/ppdparser.hxx>
#endif
#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif

#include <parseAFM.hxx>
#include <sft.h>

#ifdef DEBUG
#include <sys/times.h>
#include <stdio.h>
#endif

#ifdef SOLARIS
#include <alloca.h>
#endif

#include <adobeenc.tab> // get encoding table for AFM metrics

#define PRINTER_METRICDIR "fontmetric"

using namespace utl;
using namespace psp;
using namespace osl;
using namespace rtl;

/*
 *  static helpers
 */

inline sal_uInt16 getUInt16BE( const byte*& pBuffer )
{
    sal_uInt16 nRet = (sal_uInt16)pBuffer[1] |
        (((sal_uInt16)pBuffer[0]) << 8);
    pBuffer+=2;
    return nRet;
}

static italic::type parseItalic( const ByteString& rItalic )
{
    italic::type eItalic = italic::Unknown;
    if( rItalic.EqualsIgnoreCaseAscii( "i" ) )
        eItalic = italic::Italic;
    else if( rItalic.EqualsIgnoreCaseAscii( "o" ) )
        eItalic = italic::Oblique;
    else
        eItalic = italic::Upright;
    return eItalic;
}

// -------------------------------------------------------------------------

static weight::type parseWeight( const ByteString& rWeight )
{
    weight::type eWeight = weight::Unknown;
    if( rWeight.Search( "bold" ) != STRING_NOTFOUND )
    {
        if( rWeight.Search( "emi" ) != STRING_NOTFOUND ) // semi, demi
            eWeight = weight::SemiBold;
        else if( rWeight.Search( "ultra" ) != STRING_NOTFOUND )
            eWeight = weight::UltraBold;
        else
            eWeight = weight::Bold;
    }
    else if( rWeight.Search( "light" ) != STRING_NOTFOUND )
    {
        if( rWeight.Search( "emi" ) != STRING_NOTFOUND ) // semi, demi
            eWeight = weight::SemiLight;
        else if( rWeight.Search( "ultra" ) != STRING_NOTFOUND )
            eWeight = weight::UltraLight;
        else
            eWeight = weight::Light;
    }
    else if( rWeight.Search( "black" ) != STRING_NOTFOUND )
        eWeight = weight::Black;
    else if( rWeight.Equals( "demi" ) )
        eWeight = weight::SemiBold;
    else if( rWeight.Equals( "book" ) ||
             rWeight.Equals( "semicondensed" ) )
        eWeight = weight::Light;
    else if( rWeight.Equals( "medium" ) || rWeight.Equals( "roman" ) )
        eWeight = weight::Medium;
    else
        eWeight = weight::Normal;
    return eWeight;
}

// -------------------------------------------------------------------------

static width::type parseWidth( const ByteString& rWidth )
{
    width::type eWidth = width::Unknown;
    if( rWidth.Equals( "bold" ) ||
        rWidth.Equals( "semiexpanded" ) )
        eWidth = width::SemiExpanded;
    else if( rWidth.Equals( "condensed" ) ||
             rWidth.Equals( "narrow" ) )
        eWidth = width::Condensed;
    else if( rWidth.Equals( "double wide" ) ||
             rWidth.Equals( "extraexpanded" ) ||
             rWidth.Equals( "ultraexpanded" ) )
        eWidth = width::UltraExpanded;
    else if( rWidth.Equals( "expanded" ) ||
             rWidth.Equals( "wide" ) )
        eWidth = width::Expanded;
    else if( rWidth.Equals( "extracondensed" ) )
        eWidth = width::ExtraCondensed;
    else if( rWidth.Equals( "semicondensed" ) )
        eWidth = width::SemiCondensed;
    else if( rWidth.Equals( "ultracondensed" ) )
        eWidth = width::UltraCondensed;
    else
        eWidth = width::Normal;

    return eWidth;
}

// -------------------------------------------------------------------------

/*
 *  PrintFont implementations
 */
PrintFontManager::PrintFont::PrintFont( fonttype::type eType ) :
        m_eType( eType ),
        m_nFamilyName( 0 ),
        m_nPSName( 0 ),
        m_eItalic( italic::Unknown ),
        m_eWidth( width::Unknown ),
        m_eWeight( weight::Unknown ),
        m_ePitch( pitch::Unknown ),
        m_aEncoding( RTL_TEXTENCODING_DONTKNOW ),
        m_pMetrics( NULL ),
        m_nAscend( 0 ),
        m_nDescend( 0 ),
        m_nLeading( 0 )
{
}

// -------------------------------------------------------------------------

PrintFontManager::PrintFont::~PrintFont()
{
    if( m_pMetrics )
        delete m_pMetrics;
}

// -------------------------------------------------------------------------

PrintFontManager::Type1FontFile::~Type1FontFile()
{
}

// -------------------------------------------------------------------------

PrintFontManager::TrueTypeFontFile::~TrueTypeFontFile()
{
}

// -------------------------------------------------------------------------

PrintFontManager::BuiltinFont::~BuiltinFont()
{
}

// -------------------------------------------------------------------------

bool PrintFontManager::Type1FontFile::queryMetricPage( int nPage, MultiAtomProvider* pProvider )
{
    return readAfmMetrics( PrintFontManager::get().getAfmFile( this ), pProvider );
}

// -------------------------------------------------------------------------

bool PrintFontManager::BuiltinFont::queryMetricPage( int nPage, MultiAtomProvider* pProvider )
{
    return readAfmMetrics( PrintFontManager::get().getAfmFile( this ), pProvider );
}

// -------------------------------------------------------------------------

bool PrintFontManager::TrueTypeFontFile::queryMetricPage( int nPage, MultiAtomProvider* pProvider )
{
    bool bSuccess = false;

    ByteString aFile( PrintFontManager::get().getFontFile( this ) );

    TrueTypeFont* pTTFont = NULL;

    if( OpenTTFont( aFile.GetBuffer(), m_nCollectionEntry < 0 ? 0 : m_nCollectionEntry, &pTTFont ) == SF_OK )
    {
        if( ! m_pMetrics )
        {
            m_pMetrics = new PrintFontMetrics;
            memset (m_pMetrics->m_aPages, 0, sizeof(m_pMetrics->m_aPages));
        }
        m_pMetrics->m_aPages[ nPage/8 ] |= (1 << ( nPage & 7 ));
        int i;
        uint16 table[256];

        for( i = 0; i < 256; i++ )
            table[ i ] = 256*nPage + i;

        int nCharacters = nPage < 255 ? 256 : 254;
        MapString( pTTFont, table, nCharacters, NULL, 0 );
        TTSimpleGlyphMetrics* pMetrics = GetTTSimpleCharMetrics( pTTFont, nPage*256, nCharacters, 0 );
        if( pMetrics )
        {
            for( i = 0; i < nCharacters; i++ )
            {
                if( table[i] )
                {
                    CharacterMetric& rChar = m_pMetrics->m_aMetrics[ nPage*256 + i ];
                    rChar.width = pMetrics[ i ].adv;
                    rChar.height = m_aGlobalMetricX.height;
                }
            }

            free( pMetrics );
        }

        for( i = 0; i < 256; i++ )
            table[ i ] = 256*nPage + i;
        MapString( pTTFont, table, nCharacters, NULL, 1 );
        pMetrics = GetTTSimpleCharMetrics( pTTFont, nPage*256, nCharacters, 1 );
        if( pMetrics )
        {
            for( i = 0; i < nCharacters; i++ )
            {
                if( table[i] )
                {
                    CharacterMetric& rChar = m_pMetrics->m_aMetrics[ nPage*256 + i + ( 1 << 16 ) ];
                    rChar.width = m_aGlobalMetricY.width;
                    rChar.height = pMetrics[ i ].adv;
                }
            }
            free( pMetrics );
        }

        if( ! m_pMetrics->m_bKernPairsQueried )
        {
            m_pMetrics->m_bKernPairsQueried = true;
            // this is really a hack
            // in future MapString/KernGlyphs should be used
            // but vcl is not in a state where that could be used
            // so currently we get kernpairs by accessing the raw data
            struct _TrueTypeFont* pImplTTFont = (struct _TrueTypeFont*)pTTFont;

            if( pImplTTFont->nkern && pImplTTFont->kerntype == KT_MICROSOFT )
            {
                // create a glyph -> character mapping
                ::std::hash_map< sal_uInt16, sal_Unicode > aGlyphMap;
                ::std::hash_map< sal_uInt16, sal_Unicode >::iterator left, right;
                for( i = 21; i < 0xfffd; i++ )
                {
                    sal_uInt16 nGlyph = MapChar( pTTFont, (sal_Unicode)i, 0 ); // kerning for horz only
                    if( nGlyph != 0 )
                        aGlyphMap[ nGlyph ] = (sal_Unicode)i;
                }


                KernPair aPair;
                for( i = 0; i < pImplTTFont->nkern; i++ )
                {
                    const byte* pTable = pImplTTFont->kerntables[i];

                    sal_uInt16 nVersion     = getUInt16BE( pTable );
                    sal_uInt16 nLength      = getUInt16BE( pTable );
                    sal_uInt16 nCoverage    = getUInt16BE( pTable );

                    aPair.kern_x    = 0;
                    aPair.kern_y    = 0;
                    switch( nCoverage >> 8 )
                    {
                        case 0:
                        {
                            sal_uInt16 nPairs = getUInt16BE( pTable );
                            pTable += 6;
                            for( int n = 0; n < nPairs; n++ )
                            {
                                sal_uInt16 nLeftGlyph   = getUInt16BE( pTable );
                                sal_uInt16 nRightGlyph  = getUInt16BE( pTable );
                                sal_Int16 nKern         = (sal_Int16)getUInt16BE( pTable );

                                left = aGlyphMap.find( nLeftGlyph );
                                right = aGlyphMap.find( nRightGlyph );
                                if( left != aGlyphMap.end() && right != aGlyphMap.end() )
                                {
                                    aPair.first     = left->second;
                                    aPair.second    = right->second;
                                    switch( nCoverage & 1 )
                                    {
                                        case 1:
                                            aPair.kern_x = (int)nKern * 1000 / pImplTTFont->unitsPerEm;
                                            m_pMetrics->m_aXKernPairs.push_back( aPair );
                                            break;
                                        case 0:
                                            aPair.kern_y = (int)nKern * 1000 / pImplTTFont->unitsPerEm;
                                            m_pMetrics->m_aYKernPairs.push_back( aPair );
                                            break;
                                    }
                                }
                            }
                        }
                        break;
                        case 2:

                        {
                            const byte* pSubTable = pTable;
                            sal_uInt16 nRowWidth    = getUInt16BE( pTable );
                            sal_uInt16 nOfLeft      = getUInt16BE( pTable );
                            sal_uInt16 nOfRight     = getUInt16BE( pTable );
                            sal_uInt16 nOfArray     = getUInt16BE( pTable );
                            const byte* pTmp = pSubTable + nOfLeft;
                            sal_uInt16 nFirstLeft   = getUInt16BE( pTmp );
                            sal_uInt16 nLastLeft    = getUInt16BE( pTmp ) + nFirstLeft - 1;
                            pTmp = pSubTable + nOfRight;
                            sal_uInt16 nFirstRight  = getUInt16BE( pTmp );
                            sal_uInt16 nLastRight   = getUInt16BE( pTmp ) + nFirstRight -1;

                            int nPairs = (int)(nLastLeft-nFirstLeft+1)*(int)(nLastRight-nFirstRight+1);
                            for( aPair.first = nFirstLeft; aPair.first < nLastLeft; aPair.first++ )
                            {
                                for( aPair.second = 0; aPair.second < nLastRight; aPair.second++ )
                                {
                                    sal_Int16 nKern = (sal_Int16)getUInt16BE( pTmp );
                                    switch( nCoverage & 1 )
                                    {
                                        case 1:
                                            aPair.kern_x = (int)nKern * 1000 / pImplTTFont->unitsPerEm;
                                            m_pMetrics->m_aXKernPairs.push_back( aPair );
                                            break;
                                        case 0:
                                            aPair.kern_y = (int)nKern * 1000 / pImplTTFont->unitsPerEm;
                                            m_pMetrics->m_aYKernPairs.push_back( aPair );
                                            break;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
#ifdef DEBUG
            fprintf( stderr, "found %d/%d kern pairs for %s\n",
                     m_pMetrics->m_aXKernPairs.size(),
                     m_pMetrics->m_aYKernPairs.size(),
                     OUStringToOString( pProvider->getString( ATOM_FAMILYNAME, m_nFamilyName ), RTL_TEXTENCODING_MS_1252 ).getStr() );
#endif
        }

        CloseTTFont( pTTFont );
        bSuccess = true;
    }
    return bSuccess;
}

// -------------------------------------------------------------------------

bool PrintFontManager::PrintFont::readAfmMetrics( const OString& rFileName, MultiAtomProvider* pProvider )
{
    int i;
    FILE* fp = fopen( rFileName.getStr(), "r" );
    if( ! fp )
        return false;
    FontInfo* pInfo = NULL;
    int nResult = parseFile( fp, &pInfo, P_ALL );
    fclose( fp );
    if( nResult != ok )
    {
        if( pInfo )
            freeFontInfo( pInfo );
        return false;
    }

    // fill in global info

    // family name (if not already set)
    if( ! m_nFamilyName )
    {
        OUString aFamily( OStringToOUString( pInfo->gfi->familyName, RTL_TEXTENCODING_ISO_8859_1 ) );
        if( ! aFamily.getLength() )
            aFamily = OStringToOUString( pInfo->gfi->fontName, RTL_TEXTENCODING_ISO_8859_1 );
        m_nFamilyName = pProvider->getAtom( ATOM_FAMILYNAME, aFamily, sal_True );
    }

    // PSName
    OUString aPSName( OStringToOUString( pInfo->gfi->fontName, RTL_TEXTENCODING_ISO_8859_1 ) );
    m_nPSName = pProvider->getAtom( ATOM_PSNAME, aPSName, sal_True );

    // italic
    if( pInfo->gfi->italicAngle > 0 )
        m_eItalic = italic::Oblique;
    else if( pInfo->gfi->italicAngle < 0 )
        m_eItalic = italic::Italic;
    else
        m_eItalic = italic::Upright;

    // weight
    m_eWeight = parseWeight( ByteString( pInfo->gfi->weight ).ToLowerAscii() );

    // pitch
    m_ePitch = pInfo->gfi->isFixedPitch ? pitch::Fixed : pitch::Variable;

    // encoding - only set if unknown
    // try to parse the font name and decide wether it might be a
    // japanese font. Who invented this PITA ?
    int nTokens = aPSName.getTokenCount( '-' );
    if( m_aEncoding == RTL_TEXTENCODING_DONTKNOW && nTokens > 2 &&
        ( ! aPSName.getToken( nTokens-1, '-' ).compareToAscii( "H" )    ||
          ! aPSName.getToken( nTokens-1, '-' ).compareToAscii( "V" ) )
        )
    {
        static const char* pEncs[] =
            {
                "EUC",
                "RKSJ",
                "SJ"
            };
        static const rtl_TextEncoding aEncs[] =
            {
                RTL_TEXTENCODING_EUC_JP,
                RTL_TEXTENCODING_SHIFT_JIS,
                RTL_TEXTENCODING_JIS_X_0208
            };

        for( int enc = 0; enc < sizeof( aEncs )/sizeof(aEncs[0]) && m_aEncoding == RTL_TEXTENCODING_DONTKNOW; enc++ )
            for( int i = 1; i < nTokens-1 && m_aEncoding == RTL_TEXTENCODING_DONTKNOW; i++ )
            {
                OUString aEncoding( aPSName.getToken( i, '-' ) );
                if( ! aEncoding.compareToAscii( pEncs[enc] ) )
                    m_aEncoding = aEncs[ enc ];
            }

        // default is jis
        if( m_aEncoding == RTL_TEXTENCODING_DONTKNOW )
            m_aEncoding = RTL_TEXTENCODING_JIS_X_0208;
#ifdef DEBUG
        fprintf( stderr, "Encoding %d for %s\n", m_aEncoding, pInfo->gfi->fontName );
#endif
    }
    int nAdobeEncoding = 0;
    if( pInfo->gfi->encodingScheme )
    {
        if( !strcmp( pInfo->gfi->encodingScheme, "AdobeStandardEncoding" ) )
            nAdobeEncoding = 1;
        else if( !strcmp( pInfo->gfi->encodingScheme, "Symbol") )
            nAdobeEncoding = 2;
        else if( !strcmp( pInfo->gfi->encodingScheme, "FontSpecific") )
            nAdobeEncoding = 3;

        if( m_aEncoding == RTL_TEXTENCODING_DONTKNOW )
            m_aEncoding = nAdobeEncoding == 1 ?
                RTL_TEXTENCODING_MS_1252 : RTL_TEXTENCODING_SYMBOL;
    }
    else if( m_aEncoding == RTL_TEXTENCODING_DONTKNOW )
        m_aEncoding = RTL_TEXTENCODING_MS_1252;

    // ascend
    m_nAscend = pInfo->gfi->fontBBox.ury;

    // descend
    // descends have opposite sign of our definition
    m_nDescend = -pInfo->gfi->fontBBox.lly;

    // fallback to ascender, descender
    // interestigly the BBox seems to describe Ascender and Descender better
    // as we understand it
    if( m_nAscend == 0 )
        m_nAscend = pInfo->gfi->ascender;
    if( m_nDescend == 0)
        m_nDescend = -pInfo->gfi->descender;

    m_nLeading = m_nAscend + m_nDescend - 1000;

    if( m_pMetrics )
        delete m_pMetrics;
    m_pMetrics = new PrintFontMetrics;
    // mark all pages as queried
    memset( m_pMetrics->m_aPages, 0xff, sizeof( m_pMetrics->m_aPages ) );

    m_aGlobalMetricX.width = m_aGlobalMetricY.width =
        pInfo->gfi->charwidth ? pInfo->gfi->charwidth : pInfo->gfi->fontBBox.urx;
    m_aGlobalMetricX.height = m_aGlobalMetricY.height =
        pInfo->gfi->capHeight ? pInfo->gfi->capHeight : pInfo->gfi->fontBBox.ury;

    // fill in character metrics

    // first transform the character codes to unicode
    // note: this only works with single byte encodings
    sal_Unicode* pUnicodes = (sal_Unicode*)alloca( pInfo->numOfChars * sizeof(sal_Unicode));
    CharMetricInfo* pChar = pInfo->cmi;

    for( i = 0; i < pInfo->numOfChars; i++, pChar++ )
    {
        if( pChar->code != -1 )
        {
            ByteString aTranslate;
            if( pChar->code & 0xff000000 )
                aTranslate += (char)(pChar->code >> 24 );
            if( pChar->code & 0xffff0000 )
                aTranslate += (char)((pChar->code & 0x00ff0000) >> 16 );
            if( pChar->code & 0xffffff00 )
                aTranslate += (char)((pChar->code & 0x0000ff00) >> 8 );
            aTranslate += (char)(pChar->code & 0xff);
            String aUni( aTranslate, m_aEncoding );
            pUnicodes[i] = *aUni.GetBuffer();
        }
        else
            pUnicodes[i] = 0;
    }

    // now fill in the character metrics
    // parseAFM.cxx effectively only supports direction 0 (horizontal)
    pChar = pInfo->cmi;
    CharacterMetric aMetric;
    for( i = 0; i < pInfo->numOfChars; i++, pChar++ )
    {
        if( pChar->code == -1 && ! pChar->name )
            continue;

        aMetric.width   = pChar->wx ? pChar->wx : pChar->charBBox.urx;
        aMetric.height  = pChar->wy ? pChar->wy : pChar->charBBox.ury - pChar->charBBox.lly;
        if( aMetric.width == 0 && aMetric.height == 0 )
            // guess something for e.g. space
            aMetric.width = m_aGlobalMetricX.width/4;

        if( ( nAdobeEncoding == 0 ) ||
            ( ( nAdobeEncoding == 3 ) && ( m_aEncoding != RTL_TEXTENCODING_SYMBOL ) ) )
        {
            if( pChar->code != -1 )
            {
                m_pMetrics->m_aMetrics[ pUnicodes[i] ] = aMetric;
            }
            else if( pChar->name )
            {
                for( int n = 0; n < sizeof(aAdobeStandardNoneCodes)/sizeof(aAdobeStandardNoneCodes[0]); n++ )
                {
                    if( ! strcmp( pChar->name, aAdobeStandardNoneCodes[n].pAdobename ) )
                    {
                        m_pMetrics->m_aMetrics[ aAdobeStandardNoneCodes[n].aUnicode ] = aMetric;
                    }
                }
            }
        }
        else if( nAdobeEncoding == 1 )
        {
            if( pChar->code == -1 )
            {
                if( pChar->name )
                {

                    for( int n = 0; n < sizeof(aAdobeStandardNoneCodes)/sizeof(aAdobeStandardNoneCodes[0]); n++ )
                    {
                        if( ! strcmp( pChar->name, aAdobeStandardNoneCodes[n].pAdobename ) )
                        {
                            m_pMetrics->m_aMetrics[ aAdobeStandardNoneCodes[n].aUnicode ] = aMetric;
                        }
                    }
                }
            }
            else
            {
                for( int n = 0; n < sizeof(aAdobeStandardConvTable)/sizeof(aAdobeStandardConvTable[0]); n++ )
                {
                    if( pChar->code == aAdobeStandardConvTable[n].aAdobecode )
                    {
                        m_pMetrics->m_aMetrics[ aAdobeStandardConvTable[n].aUnicode ] = aMetric;
                        break;
                    }
                }
            }
        }
        else if( nAdobeEncoding == 2 )
        {
            if( pChar->code == -1 )
            {
                if( pChar->name )
                {
                    for( int n = 0; n < sizeof(aAdobeSymbolConvTable)/sizeof(aAdobeSymbolConvTable[0]); n++ )
                    {
                        if( ! strcmp( pChar->name, aAdobeStandardNoneCodes[n].pAdobename ) )
                        {
                            m_pMetrics->m_aMetrics[ aAdobeStandardNoneCodes[n].aUnicode ] = aMetric;
                        }
                    }
                }
            }
            else
            {
                for( int n = 0; n < sizeof(aAdobeSymbolConvTable)/sizeof(aAdobeSymbolConvTable[0]); n++ )
                {
                    if( pChar->code == aAdobeSymbolConvTable[n].aAdobecode )
                    {
                        m_pMetrics->m_aMetrics[ aAdobeSymbolConvTable[n].aUnicode ] = aMetric;
                        break;
                    }
                }
            }
        }
        else if( nAdobeEncoding == 3 )
        {
            if( pChar->code != -1 )
            {
                sal_Unicode code = 0xf000 + pChar->code;
                m_pMetrics->m_aMetrics[ code ] = aMetric;
                // maybe should try to find the name in the convtabs ?
            }
        }
    }

    m_pMetrics->m_aXKernPairs.clear();
    m_pMetrics->m_aYKernPairs.clear();

    // now fill in the kern pairs
    // parseAFM.cxx effectively only supports direction 0 (horizontal)
    PairKernData* pKern = pInfo->pkd;
    KernPair aPair;
    for( i = 0; i < pInfo->numOfPairs; i++, pKern++ )
    {
        aPair.first = 0;
        aPair.second = 0;
        // currently we have to find the adobe character names
        // in the already parsed character metrics to find
        // the corresponding UCS2 code which is a bit dangerous
        // since the character names are not required
        // in the metric descriptions
        pChar = pInfo->cmi;
        for( int j = 0;
             j < pInfo->numOfChars && ( aPair.first == 0 || aPair.second == 0 );
             j++, pChar++ )
        {
            if( pChar->code != -1 )
            {
                if( ! strcmp( pKern->name1, pChar->name ? pChar->name : "" ) )
                    aPair.first = pUnicodes[ j ];
                if( ! strcmp( pKern->name2, pChar->name ? pChar->name : "" ) )
                    aPair.second = pUnicodes[ j ];
            }
        }
        if( aPair.first && aPair.second )
        {
            aPair.kern_x = pKern->xamt;
            aPair.kern_y = pKern->yamt;
            m_pMetrics->m_aXKernPairs.push_back( aPair );
        }
    }
    m_pMetrics->m_bKernPairsQueried = true;

    freeFontInfo( pInfo );
    return true;
}

// -------------------------------------------------------------------------

/*
 *  one instance only
 */
PrintFontManager& PrintFontManager::get()
{
    static PrintFontManager* theManager = NULL;
    if( ! theManager )
    {
        theManager = new PrintFontManager();
        theManager->initialize();
    }
    return *theManager;
}

// -------------------------------------------------------------------------

/*
 *  the PrintFontManager
 */

PrintFontManager::PrintFontManager() :
        m_pAtoms( new MultiAtomProvider() ),
        m_nNextFontID( 1 ),
        m_nNextDirAtom( 1 )
{
}

// -------------------------------------------------------------------------

PrintFontManager::~PrintFontManager()
{
    for( ::std::hash_map< fontID, PrintFont* >::const_iterator it = m_aFonts.begin(); it != m_aFonts.end(); ++it )
        delete (*it).second;
    delete m_pAtoms;
}

// -------------------------------------------------------------------------

const OString& PrintFontManager::getDirectory( int nAtom ) const
{
    static OString aEmpty;
    ::std::hash_map< int, OString >::const_iterator it( m_aAtomToDir.find( nAtom ) );
    return it != m_aAtomToDir.end() ? it->second : aEmpty;
}

// -------------------------------------------------------------------------

int PrintFontManager::getDirectoryAtom( const OString& rDirectory, bool bCreate )
{
    int nAtom = 0;
    ::std::hash_map< OString, int, OStringHash >::const_iterator it
          ( m_aDirToAtom.find( rDirectory ) );
    if( it != m_aDirToAtom.end() )
        nAtom = it->second;
    else if( bCreate )
    {
        nAtom = m_nNextDirAtom++;
        m_aDirToAtom[ rDirectory ] = nAtom;
        m_aAtomToDir[ nAtom ] = rDirectory;
    }
    return nAtom;
}

// -------------------------------------------------------------------------

int PrintFontManager::addFontFile( const ::rtl::OString& rFileName, int nFaceNum )
{
    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();
    INetURLObject aPath( OStringToOUString( rFileName, aEncoding ), INET_PROT_FILE, INetURLObject::ENCODE_ALL );
    OString aName( OUStringToOString( aPath.GetName(), aEncoding ) );
    OString aDir( OUStringToOString( aPath.GetPath(), aEncoding ) );

    int nDirID = getDirectoryAtom( aDir, true );
    fontID nFontId = findFontFileID( nDirID, aName );
    if( !nFontId )
    {
        ::std::list< PrintFont* > aNewFonts;
        if( analyzeFontFile( nDirID, aName, false, OString(), aNewFonts ) )
        {
            for( ::std::list< PrintFont* >::iterator it = aNewFonts.begin();
                 it != aNewFonts.end(); ++it )
                m_aFonts[ nFontId = m_nNextFontID++ ] = *it;
        }
    }
    return nFontId;
}
// -------------------------------------------------------------------------

bool PrintFontManager::analyzeFontFile( int nDirID, const OString& rFontFile, bool bReadFile, const OString& rXLFD, ::std::list< PrintFontManager::PrintFont* >& rNewFonts ) const
{
    rNewFonts.clear();
    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();

    OString aDir( getDirectory( nDirID ) );

    ByteString aExt( rFontFile.getToken( rFontFile.getTokenCount( '.' )-1, '.' ) );
    if( aExt.EqualsIgnoreCaseAscii( "pfb" ) || aExt.EqualsIgnoreCaseAscii( "pfa" ) )
    {
        // check for corresponding afm metric
        // first look for an adjacent file

        ByteString aName( rFontFile );
        aName.Erase( aName.Len()-4 );
        aName.Append( ".afm" );

        ByteString aFilePath( aDir );
        aFilePath.Append( '/' );
        aFilePath.Append( aName );

        ByteString aAfmFile;
        if( access( aFilePath.GetBuffer(), F_OK ) )
        {
            // try in subdirectory afm instead
            aFilePath = aDir;
            aFilePath.Append( "/afm/" );
            aFilePath.Append( aName );

            if( ! access( aFilePath.GetBuffer(), F_OK ) )
            {
                aAfmFile = "afm/";
                aAfmFile += aName;
            }
        }
        else
            aAfmFile = aName;

        if( aAfmFile.Len() )
        {
            Type1FontFile* pFont = new Type1FontFile();
            pFont->m_nDirectory     = nDirID;

            pFont->m_aFontFile      = rFontFile;
            pFont->m_aMetricFile    = aAfmFile;

            if( bReadFile )
            {
                if( ! pFont->readAfmMetrics( getAfmFile( pFont ), m_pAtoms ) )
                {
                    delete pFont;
                    pFont = NULL;
                }
            }
            else
                getFontAttributesFromXLFD( pFont, rXLFD );
            if( pFont )
                rNewFonts.push_back( pFont );
        }
    }
    else if( aExt.EqualsIgnoreCaseAscii( "ttf" ) )
    {
        TrueTypeFontFile* pFont     = new TrueTypeFontFile();
        pFont->m_nDirectory         = nDirID;
        pFont->m_aFontFile          = rFontFile;
        pFont->m_nCollectionEntry   = -1;

        if( bReadFile )
        {
            if( ! analyzeTrueTypeFile( pFont ) )
            {
                delete pFont;
                pFont = NULL;
            }
        }
        else
            getFontAttributesFromXLFD( pFont, rXLFD );
        if( pFont )
            rNewFonts.push_back( pFont );
    }
    else if( aExt.EqualsIgnoreCaseAscii( "ttc" ) )
    {
        OString aFullPath( aDir );
        aFullPath += "/";
        aFullPath += rFontFile;

        // get number of ttc entries
        int nLength = CountTTCFonts( aFullPath.getStr() );
        if( nLength )
        {
#ifdef DEBUG
            fprintf( stderr, "%s contains %d fonts\n   XLFD=\"%s\"\n", aFullPath.getStr(), nLength, rXLFD.getStr() );
#endif
            for( int i = 0; i < nLength; i++ )
            {
                TrueTypeFontFile* pFont     = new TrueTypeFontFile();
                pFont->m_nDirectory         = nDirID;
                pFont->m_aFontFile          = rFontFile;
                pFont->m_nCollectionEntry   = i;
                getFontAttributesFromXLFD( pFont, rXLFD );
                if( ! analyzeTrueTypeFile( pFont ) )
                {
                    delete pFont;
                    pFont = NULL;
                }
                else
                    rNewFonts.push_back( pFont );
            }
        }
#ifdef DEBUG
        else
            fprintf( stderr, "CountTTCFonts( \"%s/%s\" ) failed\n", getDirectory(nDirID).getStr(), rFontFile.getStr() );
#endif
    }
    return ! rNewFonts.empty();
}

// -------------------------------------------------------------------------

fontID PrintFontManager::findFontBuiltinID( int nPSNameAtom ) const
{
    fontID nID = 0;
    ::std::hash_map< int, PrintFont* >::const_iterator it;
    for( it = m_aFonts.begin(); nID == 0 && it != m_aFonts.end(); ++it )
    {
        if( it->second->m_eType == fonttype::Builtin &&
            it->second->m_nPSName == nPSNameAtom )
            nID = it->first;
    }
    return nID;
}

// -------------------------------------------------------------------------

fontID PrintFontManager::findFontFileID( int nDirID, const OString& rFontFile ) const
{
    fontID nID = 0;
    ::std::hash_map< int, PrintFont* >::const_iterator it;
    for( it = m_aFonts.begin(); nID == 0 && it != m_aFonts.end(); ++it )
    {
        switch( it->second->m_eType )
        {
            case fonttype::Type1:
            {
                Type1FontFile* const pFont = static_cast< Type1FontFile* const >((*it).second);
                if( pFont->m_nDirectory == nDirID &&
                    pFont->m_aFontFile == rFontFile )
                    nID = it->first;
            }
            break;
            case fonttype::TrueType:
            {
                TrueTypeFontFile* const pFont = static_cast< TrueTypeFontFile* const >((*it).second);
                if( pFont->m_nDirectory == nDirID &&
                    pFont->m_aFontFile == rFontFile )
                    nID = it->first;
            }
            break;
            default:
                break;
        }
    }
    return nID;
}

// -------------------------------------------------------------------------

void PrintFontManager::getFontAttributesFromXLFD( PrintFont* pFont, const ByteString& rXLFD ) const
{
    if( rXLFD.GetTokenCount( '-' ) != 15 )
        return;

    ByteString aFamilyXLFD( WhitespaceToSpace( rXLFD.GetToken( 2, '-' ) ) );
    int nTokens = aFamilyXLFD.GetTokenCount( ' ' );
    ByteString aFamilyName;
    for( int nToken = 0; nToken < nTokens; nToken++ )
    {
        ByteString aToken( aFamilyXLFD.GetToken( nToken, ' ' ) );
        ByteString aNewToken( aToken.GetChar( 0 ) );
        aNewToken.ToUpperAscii();
        aNewToken += aToken.Copy( 1 );
        if( nToken > 0 )
            aFamilyName.Append( ' ' );
        aFamilyName += aNewToken;
    }

    pFont->m_nFamilyName =
        m_pAtoms->getAtom( ATOM_FAMILYNAME,
                           String( aFamilyName, RTL_TEXTENCODING_ISO_8859_1 ),
                           sal_True );
    ByteString aToken;

    // evaluate weight
    aToken = rXLFD.GetToken( 3,  '-' ).ToLowerAscii();
    pFont->m_eWeight = parseWeight( aToken );

    // evaluate slant
    aToken = rXLFD.GetToken( 4, '-' );
    pFont->m_eItalic = parseItalic( aToken );

    // evaluate width
    aToken = rXLFD.GetToken( 5,  '-' ).ToLowerAscii();
    pFont->m_eWidth = parseWidth( aToken );

    // evaluate pitch
    aToken = rXLFD.GetToken( 11,  '-' ).ToLowerAscii();
    if( aToken.Equals( "c" ) || aToken.Equals( "m" ) )
        pFont->m_ePitch = pitch::Fixed;
    else
        pFont->m_ePitch = pitch::Variable;

    // get encoding
    aToken = rXLFD.GetToken( 6, '-' ).ToLowerAscii();
    if( aToken.Search( "symbol" ) != STRING_NOTFOUND )
        pFont->m_aEncoding = RTL_TEXTENCODING_SYMBOL;
    else
    {
        aToken =WhitespaceToSpace(  rXLFD.GetToken( 14 ) );
        if( aToken.EqualsIgnoreCaseAscii( "symbol" ) )
            pFont->m_aEncoding = RTL_TEXTENCODING_SYMBOL;
        else
        {
            aToken = rXLFD.GetToken( 13, '-' );
            aToken += '-';
            aToken += WhitespaceToSpace( rXLFD.GetToken( 14, '-' ) );
            pFont->m_aEncoding = rtl_getTextEncodingFromUnixCharset( aToken.GetBuffer() );
        }
    }

    // handle iso8859-1 as ms1252 to fill the "gap" starting at 0x80
    if( pFont->m_aEncoding == RTL_TEXTENCODING_ISO_8859_1 )
        pFont->m_aEncoding = RTL_TEXTENCODING_MS_1252;

    switch( pFont->m_eType )
    {
        case fonttype::Type1:
            static_cast<Type1FontFile*>(pFont)->m_aXLFD = rXLFD;
            break;
        case fonttype::TrueType:
            static_cast<TrueTypeFontFile*>(pFont)->m_aXLFD = rXLFD;
            break;
    }
}

// -------------------------------------------------------------------------

ByteString PrintFontManager::getXLFD( PrintFont* pFont ) const
{
    if( pFont->m_eType == fonttype::Type1 )
    {
        if( static_cast<Type1FontFile*>(pFont)->m_aXLFD.getLength() )
            return static_cast<Type1FontFile*>(pFont)->m_aXLFD;
    }
    if( pFont->m_eType == fonttype::TrueType )
    {
        if( static_cast<TrueTypeFontFile*>(pFont)->m_aXLFD.getLength() )
            return static_cast<TrueTypeFontFile*>(pFont)->m_aXLFD;
    }

    ByteString aXLFD( "-misc-" );
    aXLFD += ByteString( String( m_pAtoms->getString( ATOM_FAMILYNAME, pFont->m_nFamilyName ) ), RTL_TEXTENCODING_ISO_8859_1 );
    aXLFD += '-';
    switch( pFont->m_eWeight )
    {
        case weight::Thin:              aXLFD += "thin";break;
        case weight::UltraLight:        aXLFD += "ultralight";break;
        case weight::Light:         aXLFD += "light";break;
        case weight::SemiLight:     aXLFD += "semilight";break;
        case weight::Normal:            aXLFD += "normal";break;
        case weight::Medium:            aXLFD += "medium";break;
        case weight::SemiBold:          aXLFD += "semibold";break;
        case weight::Bold:              aXLFD += "bold";break;
        case weight::UltraBold:     aXLFD += "ultrabold";break;
        case weight::Black:         aXLFD += "black";break;
    }
    aXLFD += '-';
    switch( pFont->m_eItalic )
    {
        case italic::Upright:           aXLFD += 'r';break;
        case italic::Oblique:           aXLFD += 'o';break;
        case italic::Italic:            aXLFD += 'i';break;
    }
    aXLFD += '-';
    switch( pFont->m_eWidth )
    {
        case width::UltraCondensed: aXLFD += "ultracondensed";break;
        case width::ExtraCondensed: aXLFD += "extracondensed";break;
        case width::Condensed:          aXLFD += "condensed";break;
        case width::SemiCondensed:      aXLFD += "semicondensed";break;
        case width::Normal:         aXLFD += "normal";break;
        case width::SemiExpanded:       aXLFD += "semiexpanded";break;
        case width::Expanded:           aXLFD += "expanded";break;
        case width::ExtraExpanded:      aXLFD += "extraexpanded";break;
        case width::UltraExpanded:      aXLFD += "ultraexpanded";break;
    }
    aXLFD += "--0-0-0-0-";
    aXLFD += pFont->m_ePitch == pitch::Fixed ? "m" : "p";
    aXLFD += "-0-";
    aXLFD += rtl_getBestUnixCharsetFromTextEncoding( pFont->m_aEncoding );

    return aXLFD;
}

// -------------------------------------------------------------------------

bool PrintFontManager::analyzeTrueTypeFile( PrintFont* pFont ) const
{
    bool bSuccess = false;
    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();
    ByteString aFile = getFontFile( pFont );
    TrueTypeFont* pTTFont = NULL;

    TrueTypeFontFile* pTTFontFile = static_cast< TrueTypeFontFile* >(pFont);
    int nFace = pTTFontFile->m_nCollectionEntry;
    if( OpenTTFont( aFile.GetBuffer(), nFace < 0 ? 0 : nFace, &pTTFont ) == SF_OK )
    {
        TTGlobalFontInfo aInfo;
        GetTTGlobalFontInfo( pTTFont, & aInfo );

        // set family name from XLFD if possible
        if( ! pFont->m_nFamilyName )
        {
            if( ! aInfo.family || ! *aInfo.family )
                // poor font does not have a family name
                // name it to file name minus ".tt{f|c}"
                pFont->m_nFamilyName = m_pAtoms->getAtom( ATOM_FAMILYNAME, OStringToOUString( pTTFontFile->m_aFontFile.copy( 0, pTTFontFile->m_aFontFile.getLength()-4 ), aEncoding ) );
            else
                pFont->m_nFamilyName = m_pAtoms->getAtom( ATOM_FAMILYNAME, String( ByteString( aInfo.family ), aEncoding ), sal_True );
        }


        pFont->m_nPSName = m_pAtoms->getAtom( ATOM_PSNAME, String( ByteString( aInfo.psname ), aEncoding ), sal_True );
        switch( aInfo.weight )
        {
            case FW_THIN:           pFont->m_eWeight = weight::Thin; break;
            case FW_EXTRALIGHT: pFont->m_eWeight = weight::UltraLight; break;
            case FW_LIGHT:          pFont->m_eWeight = weight::Light; break;
            case FW_MEDIUM:     pFont->m_eWeight = weight::Medium; break;
            case FW_SEMIBOLD:       pFont->m_eWeight = weight::SemiBold; break;
            case FW_BOLD:           pFont->m_eWeight = weight::Bold; break;
            case FW_EXTRABOLD:      pFont->m_eWeight = weight::UltraBold; break;
            case FW_BLACK:          pFont->m_eWeight = weight::Black; break;

            case FW_NORMAL:
            default:        pFont->m_eWeight = weight::Normal; break;
        }

        switch( aInfo.width )
        {
            case FWIDTH_ULTRA_CONDENSED:    pFont->m_eWidth = width::UltraCondensed; break;
            case FWIDTH_EXTRA_CONDENSED:    pFont->m_eWidth = width::ExtraCondensed; break;
            case FWIDTH_CONDENSED:          pFont->m_eWidth = width::Condensed; break;
            case FWIDTH_SEMI_CONDENSED: pFont->m_eWidth = width::SemiCondensed; break;
            case FWIDTH_SEMI_EXPANDED:      pFont->m_eWidth = width::SemiExpanded; break;
            case FWIDTH_EXPANDED:           pFont->m_eWidth = width::Expanded; break;
            case FWIDTH_EXTRA_EXPANDED: pFont->m_eWidth = width::ExtraExpanded; break;
            case FWIDTH_ULTRA_EXPANDED: pFont->m_eWidth = width::UltraExpanded; break;

            case FWIDTH_NORMAL:
            default:                        pFont->m_eWidth = width::Normal; break;
        }

        pFont->m_ePitch = aInfo.pitch ? pitch::Fixed : pitch::Variable;
        pFont->m_eItalic = aInfo.italicAngle == 0 ? italic::Upright : ( aInfo.italicAngle < 0 ? italic::Italic : italic::Oblique );

        pFont->m_aEncoding = aInfo.symbolEncoded ? RTL_TEXTENCODING_SYMBOL : RTL_TEXTENCODING_UCS2;

        pFont->m_aGlobalMetricY.width = pFont->m_aGlobalMetricX.width = aInfo.xMax - aInfo.xMin;
        pFont->m_aGlobalMetricY.height = pFont->m_aGlobalMetricX.height = aInfo.yMax - aInfo.yMin;

        if( aInfo.winAscent && aInfo.winDescent )
        {
            pFont->m_nAscend    = aInfo.winAscent;
            pFont->m_nDescend   = aInfo.winDescent;
            pFont->m_nLeading   = pFont->m_nAscend + pFont->m_nDescend - 1000;
        }
        else if( aInfo.typoAscender && aInfo.typoDescender )
        {
            pFont->m_nLeading   = aInfo.typoLineGap;
            pFont->m_nAscend    = aInfo.typoAscender;
            pFont->m_nDescend   = -aInfo.typoDescender;
        }
        else
        {
            pFont->m_nLeading   = aInfo.linegap;
            pFont->m_nAscend    = aInfo.ascender;
            pFont->m_nDescend   = -aInfo.descender;
        }

        // last try: font bounding box
        if( pFont->m_nAscend == 0 )
            pFont->m_nAscend = aInfo.yMax;
        if( pFont->m_nDescend == 0 )
            pFont->m_nDescend = -aInfo.yMin;
        if( pFont->m_nLeading == 0 )
            pFont->m_nLeading = 15 * (pFont->m_nAscend+pFont->m_nDescend) / 100;

        if( pFont->m_nAscend && pFont->m_nDescend )
            pFont->m_aGlobalMetricX.height = pFont->m_aGlobalMetricY.height = pFont->m_nAscend + pFont->m_nDescend;

        CloseTTFont( pTTFont );
        bSuccess = true;
    }
#ifdef DEBUG
    else
        fprintf( stderr, "could not OpenTTFont \"%s\"\n", aFile.GetBuffer() );
#endif

    return bSuccess;
}

// -------------------------------------------------------------------------

static void normPath( ByteString& rPath )
{
    while( rPath.SearchAndReplace( "//", "/" ) != STRING_NOTFOUND )
        ;
    if( rPath.Len() > 0 && rPath.GetChar( rPath.Len()-1 ) == '/' )
        rPath.Erase( rPath.Len()-1 );
}


void PrintFontManager::initialize( void* pInitDisplay )
{
    long aDirEntBuffer[ (sizeof(struct dirent)+_PC_NAME_MAX)+1 ];

    // initialize may be called twice in the future
    {
        for( ::std::hash_map< fontID, PrintFont* >::const_iterator it = m_aFonts.begin(); it != m_aFonts.end(); ++it )
            delete (*it).second;
        m_nNextFontID = 1;
        m_aFonts.clear();
        m_aFontDirectories.clear();
    }

#ifdef DEBUG
    clock_t aStart;
    clock_t aStep1;
    clock_t aStep2;
    clock_t aStep3;
    int nBuiltinFonts = 0;

    struct tms tms;

    aStart = times( &tms );
#endif

    // part one - look for downloadable fonts
    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();
    static const char* pSalPrivatePath = getenv( "SAL_FONTPATH_PRIVATE" );

    // search for the fonts in SAL_PRIVATE_FONTPATH first; those are
    // the TrueType fonts installed with the office
    if( pSalPrivatePath )
    {
        ByteString aPath( pSalPrivatePath );
        int nTokens = aPath.GetTokenCount( ';' );
        for( int i = 0; i < nTokens; i++ )
        {
            ByteString aToken( aPath.GetToken( i, ';' ) );
            normPath( aToken );
            m_aFontDirectories.push_back( aToken );
        }
    }

    Display *pDisplay = (Display*)pInitDisplay;

    if( ! pDisplay )
        pDisplay = XOpenDisplay( NULL );


    // get font paths to look for fonts
    int nPaths = 0, i;
    char** pPaths = XGetFontPath( pDisplay, &nPaths );

    for( i = 0; i < nPaths; i++ )
    {
        ByteString aPath( pPaths[i] );
        normPath( aPath );
        m_aFontDirectories.push_back( aPath );
    }

    if( nPaths )
        XFreeFontPath( pPaths );

    if( ! pInitDisplay )
        XCloseDisplay( pDisplay );

    // insert some standard directories
    m_aFontDirectories.push_back( "/usr/openwin/lib/X11/fonts/Type1" );
    m_aFontDirectories.push_back( "/usr/openwin/lib/X11/fonts/Type1/sun" );
    m_aFontDirectories.push_back( "/usr/X11R6/lib/X11/fonts/Type1" );
#ifdef SOLARIS

    /* cde specials, from /usr/dt/bin/Xsession: here are the good fonts,
       the OWfontpath file may contain as well multiple lines as a comma
       separated list of fonts in each line. to make it even more weird
       environment variables are allowed as well */

    const char* lang = getenv("LANG");
    if ( lang != NULL )
    {
        String aOpenWinDir( String::CreateFromAscii( "/usr/openwin/lib/locale/" ) );
        aOpenWinDir.AppendAscii( lang );
        aOpenWinDir.AppendAscii( "/OWfontpath" );

        SvFileStream aStream( aOpenWinDir, STREAM_READ );

        // TODO: replace environment variables
        while( aStream.IsOpen() && ! aStream.IsEof() )
        {
            ByteString aLine;
            aStream.ReadLine( aLine );
            normPath( aLine );
            m_aFontDirectories.push_back( aLine );
        }
    }

#endif /* SOLARIS */

    // search for font files in each path
    ::std::list< OString >::iterator dir_it;
    for( dir_it = m_aFontDirectories.begin(); dir_it != m_aFontDirectories.end(); ++dir_it )
    {
        OString aPath( *dir_it );
        // there may be ":unscaled" directories (see XFree86)
        // it should be safe to ignore them since they should not
        // contain any of our recognbizeable fonts
        OUString aUniPath;
        FileBase::normalizePath( OStringToOUString( aPath, aEncoding ), aUniPath );

        // read fonts.dir if possible
        ::std::hash_map< OString, ByteString, OStringHash > aFontsDir;
        ByteString aGccDummy( aPath );
        String aFontsDirPath( aGccDummy, aEncoding );
        aFontsDirPath.AppendAscii( "/fonts.dir" );
        int nDirID = getDirectoryAtom( aPath, true );
        SvFileStream aStream( aFontsDirPath, STREAM_READ );
        if( aStream.IsOpen() )
        {
            ByteString aLine;
            while( ! aStream.IsEof() )
            {
                aStream.ReadLine( aLine );
                ByteString aFileName( GetCommandLineToken( 0, aLine ) );
                ByteString aXLFD( aLine.Copy( aFileName.Len() ) );
                if( aFileName.Len() && aXLFD.Len() )
                    aFontsDir[ aFileName ] = aXLFD;
            }
        }

        DIR* pDIR = opendir( aPath.getStr() );
        struct dirent* pEntry = (struct dirent*)aDirEntBuffer;
        if( pDIR )
        {
            while( ! readdir_r( pDIR, (struct dirent*)aDirEntBuffer, &pEntry ) && pEntry )
            {
                OString aFileName( pEntry->d_name );
                struct stat aStat;
                ByteString aFilePath( aPath );
                aFilePath.Append( '/' );
                aFilePath.Append( ByteString( aFileName ) );
                if( ! stat( aFilePath.GetBuffer(), &aStat )     &&
                    S_ISREG( aStat.st_mode ) )
                {
                    if( findFontFileID( nDirID, aFileName ) == 0 )
                    {
                        ByteString aXLFD;
                        ::std::hash_map< OString, ByteString, OStringHash >::const_iterator it =
                              aFontsDir.find( aFileName );
                        if( it != aFontsDir.end() )
                            aXLFD = (*it).second;

                        // fill in font attributes from XLFD rather
                        // than reading every file
                        ::std::list< PrintFont* > aNewFonts;
                        if( analyzeFontFile( nDirID, aFileName, aXLFD.Len() ? false : true, aXLFD, aNewFonts ) )
                        {
                            for( ::std::list< PrintFont* >::iterator it = aNewFonts.begin(); it != aNewFonts.end(); ++it )
                            {
                                m_aFonts[ m_nNextFontID++ ] = *it;
#ifdef DEBUG
                                fprintf( stderr, "adding font \"%s\" from file \"%s/%s\"\n", OUStringToOString( m_pAtoms->getString( ATOM_FAMILYNAME, (*it)->m_nFamilyName ), RTL_TEXTENCODING_MS_1252 ).getStr(), aPath.getStr(), aFileName.getStr() );
#endif
                            }
                        }
                    }
                }
            }
            closedir( pDIR );
        }
    }

#ifdef DEBUG
    aStep1 = times( &tms );
#endif

    // part two - look for metrics for builtin printer fonts
    OUString aPath( getPrinterPath() );
    nPaths = aPath.getTokenCount( ':' );
    for(  i = 0; i < nPaths; i++ )
    {
        OString aDir( OUStringToOString( aPath.getToken( i, ':' ), aEncoding ) );
        aDir += "/"PRINTER_METRICDIR;
        DIR* pDIR = opendir( aDir.getStr() );
        if( pDIR )
        {
            struct dirent* pDirEntry = (struct dirent*)aDirEntBuffer;
            OString aSysDir( aDir );
            int nDirAtom = getDirectoryAtom( aDir, true );

            // get cache information
            aSysDir += "/builtincache";
            struct stat aStat;
            time_t nCacheDate = 0;
            if( ! stat( aSysDir.getStr(), &aStat ) )
                nCacheDate = aStat.st_mtime;
            Config aCache( OStringToOUString( aSysDir, aEncoding ) );

            while( ! readdir_r( pDIR, (struct dirent*)aDirEntBuffer, &pDirEntry ) && pDirEntry )
            {
                ByteString aFile( aDir );
                aFile += '/';
                aFile += pDirEntry->d_name;
                if( ! stat( aFile.GetBuffer(), &aStat )
                    && S_ISREG( aStat.st_mode )
                    )
                {
                    OString aFileName( pDirEntry->d_name, strlen( pDirEntry->d_name ) );
                    OString aExt( aFileName.getToken( aFileName.getTokenCount( '.' )-1, '.' ) );
                    if( aExt.equalsIgnoreCase( "afm" ) )
                    {
                        BuiltinFont* pFont = new BuiltinFont;
                        pFont->m_nDirectory     = nDirAtom;
                        pFont->m_aMetricFile    = aFileName;

                        //  first try the cache
                        bool bWasCached = false;
                        if( aCache.HasGroup( aFileName ) )
                        {
                            aCache.SetGroup( aFileName );
                            if( aStat.st_mtime <= nCacheDate )
                            {
                                bWasCached = true;
                                pFont->m_nFamilyName= m_pAtoms->getAtom( ATOM_FAMILYNAME, String( aCache.ReadKey( "FamilyName" ), RTL_TEXTENCODING_UTF8 ), sal_True );
                                pFont->m_nPSName    = m_pAtoms->getAtom( ATOM_PSNAME, String( aCache.ReadKey( "PSName" ), RTL_TEXTENCODING_UTF8 ), sal_True );
                                pFont->m_eItalic    = (italic::type)aCache.ReadKey( "Italic" ).ToInt32();
                                pFont->m_eWidth     = (width::type)aCache.ReadKey( "Width" ).ToInt32();
                                pFont->m_ePitch     = (pitch::type)aCache.ReadKey( "Pitch" ).ToInt32();
                                pFont->m_eWeight    = (weight::type)aCache.ReadKey( "Weight" ).ToInt32();
                                pFont->m_aEncoding  = (rtl_TextEncoding)aCache.ReadKey( "Encoding" ).ToInt32();
                                pFont->m_nAscend    = aCache.ReadKey( "Ascend" ).ToInt32();
                                pFont->m_nDescend   = aCache.ReadKey( "Descend" ).ToInt32();
                                pFont->m_nLeading   = aCache.ReadKey( "Leading" ).ToInt32();
                                ByteString aValue = aCache.ReadKey( "GlobalMetrics" );
                                pFont->m_aGlobalMetricX.width = aValue.GetToken( 0, ',' ).ToInt32();
                                pFont->m_aGlobalMetricX.height = aValue.GetToken( 1, ',' ).ToInt32();
                                pFont->m_aGlobalMetricY.width = aValue.GetToken( 2, ',' ).ToInt32();
                                pFont->m_aGlobalMetricY.height = aValue.GetToken( 3, ',' ).ToInt32();
                                m_aFonts[ m_nNextFontID++ ] = pFont;
#ifdef DEBUG
                                nBuiltinFonts++;
#endif
                            }
                        }

                        if( ! bWasCached )
                        {
                            if( pFont->readAfmMetrics( aFile, m_pAtoms ) &&
                                findFontBuiltinID( pFont->m_nPSName ) == 0
                                )
                            {
                                m_aFonts[ m_nNextFontID++ ] = pFont;
#ifdef DEBUG
                                nBuiltinFonts++;
#endif

                                // update the cache
                                aCache.SetGroup( aFileName );
                                aCache.WriteKey( "FamilyName", ByteString( String( m_pAtoms->getString( ATOM_FAMILYNAME, pFont->m_nFamilyName ) ), RTL_TEXTENCODING_UTF8 ) );
                                aCache.WriteKey( "PSName", ByteString( String( m_pAtoms->getString( ATOM_PSNAME, pFont->m_nPSName ) ), RTL_TEXTENCODING_UTF8 ) );
                                aCache.WriteKey( "Italic", ByteString::CreateFromInt32( (int)pFont->m_eItalic ) );
                                aCache.WriteKey( "Width", ByteString::CreateFromInt32( (int)pFont->m_eWidth ) );
                                aCache.WriteKey( "Weight", ByteString::CreateFromInt32( (int)pFont->m_eWeight ) );
                                aCache.WriteKey( "Pitch", ByteString::CreateFromInt32( (int)pFont->m_ePitch ) );
                                aCache.WriteKey( "Encoding", ByteString::CreateFromInt32( (int)pFont->m_aEncoding ) );
                                aCache.WriteKey( "Ascend", ByteString::CreateFromInt32( pFont->m_nAscend ) );
                                aCache.WriteKey( "Descend", ByteString::CreateFromInt32( pFont->m_nDescend ) );
                                aCache.WriteKey( "Leading", ByteString::CreateFromInt32( pFont->m_nLeading ) );
                                ByteString aValue( ByteString::CreateFromInt32( pFont->m_aGlobalMetricX.width ) );
                                aValue += ',';
                                aValue += ByteString::CreateFromInt32( pFont->m_aGlobalMetricX.height );
                                aValue += ',';
                                aValue += ByteString::CreateFromInt32( pFont->m_aGlobalMetricY.width );
                                aValue += ',';
                                aValue += ByteString::CreateFromInt32( pFont->m_aGlobalMetricY.height );
                                aCache.WriteKey( "GlobalMetrics", aValue );
                            }
                            else
                                delete pFont;
                        }
                    }
                }
            }
            closedir( pDIR );
        }
    }

#ifdef DEBUG
    aStep2 = times( &tms );
#endif

    // part three - fill in family styles
    ::std::hash_map< fontID, PrintFont* >::iterator font_it;
    for (font_it = m_aFonts.begin(); font_it != m_aFonts.end(); ++font_it)
    {
        ::std::hash_map< int, family::type >::const_iterator it =
              m_aFamilyTypes.find( font_it->second->m_nFamilyName );
        if (it != m_aFamilyTypes.end())
            continue;
        const ::rtl::OUString& rFamily =
            m_pAtoms->getString( ATOM_FAMILYNAME, font_it->second->m_nFamilyName);
        family::type eType = matchFamilyName( rFamily );
        m_aFamilyTypes[ font_it->second->m_nFamilyName ] = eType;
    }

#ifdef DEBUG
    aStep3 = times( &tms );
    fprintf( stderr, "PrintFontManager::initialize: collected %d fonts (%d builtin)\n", m_aFonts.size(), nBuiltinFonts );
    fprintf( stderr, "Step 1 took %lf seconds\n", (double)(aStep1 - aStart)/(double)CLK_TCK );
    fprintf( stderr, "Step 2 took %lf seconds\n", (double)(aStep2 - aStep1)/(double)CLK_TCK );
    fprintf( stderr, "Step 3 took %lf seconds\n", (double)(aStep3 - aStep2)/(double)CLK_TCK );
#endif
}

// -------------------------------------------------------------------------
inline bool
equalPitch (psp::pitch::type from, psp::pitch::type to)
{
    return from == to;
}

inline bool
equalWeight (psp::weight::type from, psp::weight::type to)
{
    return from > to ? (from - to) <= 3 : (to - from) <= 3;
}

inline bool
equalItalic (psp::italic::type from, psp::italic::type to)
{
    if ( (from == psp::italic::Italic) || (from == psp::italic::Oblique) )
        return (to == psp::italic::Italic) || (to == psp::italic::Oblique);
    return to == from;
}
inline bool
equalEncoding (rtl_TextEncoding from, rtl_TextEncoding to)
{
    if ((from == RTL_TEXTENCODING_ISO_8859_1) || (from == RTL_TEXTENCODING_MS_1252))
        return (to == RTL_TEXTENCODING_ISO_8859_1) || (to == RTL_TEXTENCODING_MS_1252);
    return from == to;
}

void PrintFontManager::getFontList( ::std::list< fontID >& rFontIDs, const PPDParser* pParser ) const
{
    rFontIDs.clear();
    ::std::hash_map< fontID, PrintFont* >::const_iterator it;
    ::std::list< PrintFont* > aBuiltinFonts;

    for( it = m_aFonts.begin(); it != m_aFonts.end(); ++it )
    {
        if( pParser && it->second->m_eType == fonttype::Builtin )
        {
            int nFonts = pParser->getFonts();
            String aPSName = m_pAtoms->getString( ATOM_PSNAME, it->second->m_nPSName );
            for( int j = 0; j < nFonts; j++ )
            {
                if( aPSName.Equals( pParser->getFont( j ) ) )
                {
                    rFontIDs.push_back( it->first );
                    aBuiltinFonts.push_back( it->second );
                    break;
                }
            }
        }
        else
            rFontIDs.push_back( it->first );
    }

    if( pParser )
    {
        // remove doubles for builtins
        ::std::list< fontID >::iterator font_it;
        ::std::list< fontID >::iterator temp_it;
        font_it = rFontIDs.begin();
        while( font_it != rFontIDs.end() )
        {
            temp_it = font_it;
            ++temp_it;
            PrintFont* pFont = getFont( *font_it );
            if( pFont->m_eType != fonttype::Builtin )
            {
                const OUString& rFontFamily( m_pAtoms->getString( ATOM_FAMILYNAME, pFont->m_nFamilyName ) );

                for( ::std::list< PrintFont* >::const_iterator bit = aBuiltinFonts.begin();
                     bit != aBuiltinFonts.end(); ++bit )
                {
                    if( ! equalItalic  (pFont->m_eItalic, (*bit)->m_eItalic) )
                        continue;
                    if( ! equalWeight  (pFont->m_eWeight, (*bit)->m_eWeight) )
                        continue;
                    if( ! equalPitch   (pFont->m_ePitch,  (*bit)->m_ePitch) )
                        continue;
                    if( ! equalEncoding(pFont->m_aEncoding, (*bit)->m_aEncoding) )
                        continue;
                    const OUString& rBuiltinFamily( m_pAtoms->getString( ATOM_FAMILYNAME, (*bit)->m_nFamilyName ) );
                    if( rFontFamily.equalsIgnoreCase( rBuiltinFamily ) )
                    {
                        // remove double
                        rFontIDs.erase( font_it );
                        break;
                    }
                }
            }
            font_it = temp_it;
        }
    }
}

// -------------------------------------------------------------------------

void PrintFontManager::fillPrintFontInfo( PrintFont* pFont, FastPrintFontInfo& rInfo ) const
{
    ::std::hash_map< int, family::type >::const_iterator style_it =
          m_aFamilyTypes.find( pFont->m_nFamilyName );
    rInfo.m_eType           = pFont->m_eType;
    rInfo.m_aFamilyName     = m_pAtoms->getString( ATOM_FAMILYNAME, pFont->m_nFamilyName );
    rInfo.m_eFamilyStyle    = style_it != m_aFamilyTypes.end() ? style_it->second : family::Unknown;
    rInfo.m_eItalic         = pFont->m_eItalic;
    rInfo.m_eWidth          = pFont->m_eWidth;
    rInfo.m_eWeight         = pFont->m_eWeight;
    rInfo.m_ePitch          = pFont->m_ePitch;
    rInfo.m_aEncoding       = pFont->m_aEncoding;
}

// -------------------------------------------------------------------------

void PrintFontManager::fillPrintFontInfo( PrintFont* pFont, PrintFontInfo& rInfo ) const
{
    if( pFont->m_nAscend == 0 && pFont->m_nDescend == 0 )
    {
        // might be a truetype font not analyzed or type1 without metrics read
        if( pFont->m_eType == fonttype::Type1 )
            pFont->readAfmMetrics( getAfmFile( pFont ), m_pAtoms );
        else if( pFont->m_eType == fonttype::TrueType )
            analyzeTrueTypeFile( pFont );
    }

    fillPrintFontInfo( pFont, static_cast< FastPrintFontInfo& >( rInfo ) );

    rInfo.m_nAscend         = pFont->m_nAscend;
    rInfo.m_nDescend        = pFont->m_nDescend;
    rInfo.m_nLeading        = pFont->m_nLeading;
    rInfo.m_nWidth          = pFont->m_aGlobalMetricX.width < pFont->m_aGlobalMetricY.width ? pFont->m_aGlobalMetricY.width : pFont->m_aGlobalMetricX.width;
}

// -------------------------------------------------------------------------

void PrintFontManager::getFontListWithInfo( ::std::list< PrintFontInfo >& rFonts, const PPDParser* pParser ) const
{
    rFonts.clear();
    ::std::list< fontID > aFontList;
    getFontList( aFontList, pParser );

    ::std::list< fontID >::iterator it;
    for( it = aFontList.begin(); it != aFontList.end(); ++it )
    {
        PrintFontInfo aInfo;
        aInfo.m_nID = *it;
        fillPrintFontInfo( getFont( *it ), aInfo );
        rFonts.push_back( aInfo );
    }
}

// -------------------------------------------------------------------------

void PrintFontManager::getFontListWithFastInfo( ::std::list< FastPrintFontInfo >& rFonts, const PPDParser* pParser ) const
{
    rFonts.clear();
    ::std::list< fontID > aFontList;
    getFontList( aFontList, pParser );

    ::std::list< fontID >::iterator it;
    for( it = aFontList.begin(); it != aFontList.end(); ++it )
    {
        FastPrintFontInfo aInfo;
        aInfo.m_nID = *it;
        fillPrintFontInfo( getFont( *it ), aInfo );
        rFonts.push_back( aInfo );
    }
}

// -------------------------------------------------------------------------

bool PrintFontManager::getFontInfo( fontID nFontID, PrintFontInfo& rInfo ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( pFont )
    {
        rInfo.m_nID = nFontID;
        fillPrintFontInfo( pFont, rInfo );
    }
    return pFont ? true : false;
}

// -------------------------------------------------------------------------

bool PrintFontManager::getFontFastInfo( fontID nFontID, FastPrintFontInfo& rInfo ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( pFont )
    {
        rInfo.m_nID = nFontID;
        fillPrintFontInfo( pFont, rInfo );
    }
    return pFont ? true : false;
}

// -------------------------------------------------------------------------

family::type PrintFontManager::matchFamilyName( const ::rtl::OUString& rFamily ) const
{
    typedef struct {
        const char*  mpName;
        sal_uInt16   mnLength;
        family::type meType;
    } family_t;

#define InitializeClass( p, a ) p, sizeof(p) - 1, a
    const family_t pFamilyMatch[] =  {
        { InitializeClass( "arial",                  family::Swiss )  },
        { InitializeClass( "arioso",                 family::Script ) },
        { InitializeClass( "avant garde",            family::Swiss )  },
        { InitializeClass( "avantgarde",             family::Swiss )  },
        { InitializeClass( "bembo",                  family::Roman )  },
        { InitializeClass( "bookman",                family::Roman )  },
        { InitializeClass( "conga",                  family::Roman )  },
        { InitializeClass( "courier",                family::Modern ) },
        { InitializeClass( "curl",                   family::Script ) },
        { InitializeClass( "fixed",                  family::Modern ) },
        { InitializeClass( "gill",                   family::Swiss )  },
        { InitializeClass( "helmet",                 family::Modern ) },
        { InitializeClass( "helvetica",              family::Swiss )  },
        { InitializeClass( "international",          family::Modern ) },
        { InitializeClass( "lucida",                 family::Swiss )  },
        { InitializeClass( "new century schoolbook", family::Roman )  },
        { InitializeClass( "palatino",               family::Roman )  },
        { InitializeClass( "roman",                  family::Roman )  },
        { InitializeClass( "sans serif",             family::Swiss )  },
        { InitializeClass( "sansserif",              family::Swiss )  },
        { InitializeClass( "serf",                   family::Roman )  },
        { InitializeClass( "serif",                  family::Roman )  },
        { InitializeClass( "times",                  family::Roman )  },
        { InitializeClass( "utopia",                 family::Roman )  },
        { InitializeClass( "zapf chancery",          family::Script ) },
        { InitializeClass( "zapfchancery",           family::Script ) }
    };

    rtl::OString aFamily = rtl::OUStringToOString( rFamily, RTL_TEXTENCODING_ISO_8859_1 );
    sal_uInt32 nLower = 0;
    sal_uInt32 nUpper = sizeof(pFamilyMatch) / sizeof(pFamilyMatch[0]);

    while( nLower < nUpper )
    {
        sal_uInt32 nCurrent = (nLower + nUpper) / 2;
        const family_t* pHaystack = pFamilyMatch + nCurrent;
        sal_Int32  nComparison = rtl_str_compareIgnoreCase_WithLength(
                                                                      aFamily.getStr(), pHaystack->mpName, pHaystack->mnLength );

        if( nComparison < 0 )
            nUpper = nCurrent;
        else
            if( nComparison > 0 )
                nLower = nCurrent + 1;
            else
                return pHaystack->meType;
    }

    return family::Unknown;
}

// -------------------------------------------------------------------------

family::type PrintFontManager::getFontFamilyType( fontID nFontID ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( !pFont )
        return family::Unknown;

    ::std::hash_map< int, family::type >::const_iterator it =
          m_aFamilyTypes.find( pFont->m_nFamilyName );
    return (it != m_aFamilyTypes.end()) ? it->second : family::Unknown;
}


// -------------------------------------------------------------------------

const ::rtl::OUString& PrintFontManager::getFontFamily( fontID nFontID ) const
{
    PrintFont* pFont = getFont( nFontID );
    return m_pAtoms->getString( ATOM_FAMILYNAME, pFont ? pFont->m_nFamilyName : INVALID_ATOM );
}

// -------------------------------------------------------------------------

OString PrintFontManager::getAfmFile( PrintFont* pFont ) const
{
    OString aMetricPath;
    if( pFont )
    {
        switch( pFont->m_eType )
        {
            case fonttype::Type1:
            {
                Type1FontFile* pPSFont = static_cast< Type1FontFile* >(pFont);
                aMetricPath = getDirectory( pPSFont->m_nDirectory );
                aMetricPath += "/";
                aMetricPath += pPSFont->m_aMetricFile;
            }
            break;
            case fonttype::Builtin:
            {
                BuiltinFont* pBuiltinFont = static_cast< BuiltinFont* >(pFont);
                aMetricPath = getDirectory( pBuiltinFont->m_nDirectory );
                aMetricPath += "/";
                aMetricPath += pBuiltinFont->m_aMetricFile;
            }
            break;
        }
    }
    return aMetricPath;
}

// -------------------------------------------------------------------------

OString PrintFontManager::getFontFile( PrintFont* pFont ) const
{
    OString aPath;

    if( pFont && pFont->m_eType == fonttype::Type1 )
    {
        Type1FontFile* pPSFont = static_cast< Type1FontFile* >(pFont);
        ::std::hash_map< int, OString >::const_iterator it = m_aAtomToDir.find( pPSFont->m_nDirectory );
        aPath = it->second;
        aPath += "/";
        aPath += pPSFont->m_aFontFile;
    }
    else if( pFont && pFont->m_eType == fonttype::TrueType )
    {
        TrueTypeFontFile* pTTFont = static_cast< TrueTypeFontFile* >(pFont);
        ::std::hash_map< int, OString >::const_iterator it = m_aAtomToDir.find( pTTFont->m_nDirectory );
        aPath = it->second;
        aPath += "/";
        aPath += pTTFont->m_aFontFile;
    }
    return aPath;
}

// -------------------------------------------------------------------------

const ::rtl::OUString& PrintFontManager::getPSName( fontID nFontID ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( pFont && pFont->m_nPSName == 0 )
    {
        if( pFont->m_eType == fonttype::TrueType )
            analyzeTrueTypeFile( pFont );
    }

    return m_pAtoms->getString( ATOM_PSNAME, pFont ? pFont->m_nPSName : INVALID_ATOM );
}

// -------------------------------------------------------------------------

const CharacterMetric& PrintFontManager::getGlobalFontMetric( fontID nFontID, bool bHorizontal ) const
{
    static CharacterMetric aMetric;
    PrintFont* pFont = getFont( nFontID );
    return pFont ? ( bHorizontal ? pFont->m_aGlobalMetricX : pFont->m_aGlobalMetricY ) : aMetric;
}

// -------------------------------------------------------------------------

int PrintFontManager::getFontAscend( fontID nFontID ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( pFont->m_nAscend == 0 && pFont->m_nDescend == 0 )
    {
        // might be a truetype font not yet analyzed
        if( pFont->m_eType == fonttype::TrueType )
            analyzeTrueTypeFile( pFont );
    }
    return pFont->m_nAscend;
}

// -------------------------------------------------------------------------

int PrintFontManager::getFontDescend( fontID nFontID ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( pFont->m_nAscend == 0 && pFont->m_nDescend == 0 )
    {
        // might be a truetype font not yet analyzed
        if( pFont->m_eType == fonttype::TrueType )
            analyzeTrueTypeFile( pFont );
    }
    return pFont->m_nDescend;
}

// -------------------------------------------------------------------------

int PrintFontManager::getFontLeading( fontID nFontID ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( pFont->m_nAscend == 0 && pFont->m_nDescend == 0 )
    {
        // might be a truetype font not yet analyzed
        if( pFont->m_eType == fonttype::TrueType )
            analyzeTrueTypeFile( pFont );
    }
    return pFont->m_nLeading;
}

// -------------------------------------------------------------------------

OUString PrintFontManager::getFontXLFD( fontID nFontID ) const
{
    PrintFont* pFont = getFont( nFontID );
    return pFont ? OStringToOUString( getXLFD( pFont ), RTL_TEXTENCODING_ISO_8859_1 ) : OUString();
}

// -------------------------------------------------------------------------

const ::std::list< KernPair >& PrintFontManager::getKernPairs( fontID nFontID, bool bVertical ) const
{
    static ::std::list< KernPair > aEmpty;

    PrintFont* pFont = getFont( nFontID );
    if( ! pFont )
        return aEmpty;

    if( ! pFont->m_pMetrics || ! pFont->m_pMetrics->m_bKernPairsQueried )
        pFont->queryMetricPage( 0, m_pAtoms );
    if( ! pFont->m_pMetrics || ! pFont->m_pMetrics->m_bKernPairsQueried )
        return aEmpty;
    return bVertical ? pFont->m_pMetrics->m_aYKernPairs : pFont->m_pMetrics->m_aXKernPairs;
}

// -------------------------------------------------------------------------

bool PrintFontManager::getMetrics( fontID nFontID, const sal_Unicode* pString, int nLen, CharacterMetric* pArray, bool bVertical ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( ! pFont )
        return false;

    if( pFont->m_nAscend == 0 && pFont->m_nDescend == 0 )
    {
        // might be a font not yet analyzed
        if( pFont->m_eType == fonttype::Type1 )
            pFont->readAfmMetrics( getAfmFile( pFont ), m_pAtoms );
        else if( pFont->m_eType == fonttype::TrueType )
            analyzeTrueTypeFile( pFont );
    }

    for( int i = 0; i < nLen; i++ )
    {
        if( ! pFont->m_pMetrics ||
            ! ( pFont->m_pMetrics->m_aPages[ pString[i] >> 11 ] & ( 1 << ( ( pString[i] >> 8 ) & 7 ) ) ) )
            pFont->queryMetricPage( pString[i] >> 8, m_pAtoms );
        pArray[i].width = pArray[i].height = -1;
        if( pFont->m_pMetrics )
        {
            int effectiveCode = pString[i];
            effectiveCode |= bVertical ? 1 << 16 : 0;
            ::std::hash_map< int, CharacterMetric >::const_iterator it =
                  pFont->m_pMetrics->m_aMetrics.find( effectiveCode );
            if( it != pFont->m_pMetrics->m_aMetrics.end() )
                pArray[ i ] = it->second;
        }
    }

    return true;
}

// -------------------------------------------------------------------------

bool PrintFontManager::getMetrics( fontID nFontID, sal_Unicode minCharacter, sal_Unicode maxCharacter, CharacterMetric* pArray, bool bVertical ) const
{
    PrintFont* pFont = getFont( nFontID );
    if( ! pFont )
        return false;

    if( pFont->m_nAscend == 0 && pFont->m_nDescend == 0 )
    {
        // might be a font not yet analyzed
        if( pFont->m_eType == fonttype::Type1 )
            pFont->readAfmMetrics( getAfmFile( pFont ), m_pAtoms );
        else if( pFont->m_eType == fonttype::TrueType )
            analyzeTrueTypeFile( pFont );
    }

    for( sal_Unicode code = minCharacter; code <= maxCharacter; code++ )
    {
        if( ! pFont->m_pMetrics ||
            ! ( pFont->m_pMetrics->m_aPages[ code >> 11 ] & ( 1 << ( ( code >> 8 ) & 7 ) ) ) )
            pFont->queryMetricPage( code >> 8, m_pAtoms );
        pArray[ code - minCharacter ].width     = -1;
        pArray[ code - minCharacter ].height    = -1;
        if( pFont->m_pMetrics )
        {
            int effectiveCode = code;
            effectiveCode |= bVertical ? 1 << 16 : 0;
            ::std::hash_map< int, CharacterMetric >::const_iterator it =
                  pFont->m_pMetrics->m_aMetrics.find( effectiveCode );
            if( it != pFont->m_pMetrics->m_aMetrics.end() )
                pArray[ code - minCharacter ] = it->second;
        }
    }
    return true;
}

// -------------------------------------------------------------------------

static bool createPath( const ByteString& rPath )
{
    bool bSuccess = false;
#ifdef DEBUG
    fprintf( stderr, "createPath( %s )\n", rPath.GetBuffer() );
#endif

    if( access( rPath.GetBuffer(), F_OK ) )
    {
        int nPos = rPath.SearchBackward( '/' );
        if( nPos != STRING_NOTFOUND )
            while( nPos > 0 && rPath.GetChar( nPos ) == '/' )
                nPos--;

        if( nPos != STRING_NOTFOUND && nPos != 0 && createPath( rPath.Copy( 0, nPos+1 ) ) )
        {
#ifdef DEBUG
            fprintf( stderr, "mkdir ", rPath.GetBuffer() );
#endif
            bSuccess = mkdir( rPath.GetBuffer(), 0777 ) ? false : true;
#ifdef DEBUG
            fprintf( stderr, "%s\n", bSuccess ? "succeeded" : "failed" );
#endif
        }
    }
    else
        bSuccess = true;

    return bSuccess;
}


// -------------------------------------------------------------------------

int PrintFontManager::importFonts( const ::std::list< OUString >& rFiles, ImportFontCallback* pCallback )
{
    int nSuccess = 0;

    // find a directory with write access
    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();
    SvFileStream aFontsDir;
    int nDirID;
    INetURLObject aDir;
    for( ::std::list< OString >::const_iterator dir_it = m_aFontDirectories.begin();
         ! ( aFontsDir.IsOpen() && aFontsDir.IsWritable() ) && dir_it != m_aFontDirectories.end(); ++dir_it )
    {
        // there must be a writable fonts.dir in that directory
        aDir = INetURLObject( OStringToOUString( *dir_it, aEncoding ), INET_PROT_FILE, INetURLObject::ENCODE_ALL );
        nDirID = getDirectoryAtom( *dir_it, false );
        INetURLObject aFDir( aDir );
        ByteString aDirPath( aFDir.PathToFileName(), aEncoding );
        if( createPath( aDirPath ) )
        {
            aFDir.Append( String( RTL_CONSTASCII_USTRINGPARAM( "fonts.dir" ) ) );
            aFontsDir.Open( aFDir.PathToFileName(), STREAM_READ | STREAM_WRITE );
        }
    }
    if( aFontsDir.IsOpen() )
    {
        aFontsDir.SetLineDelimiter( LINEEND_LF );
        // we have a suitable path
        // read the fonts.dir
        ::std::list< ByteString > aLines;
        ::std::list< ByteString >::iterator line_it;
        ByteString aLine;
        while( ! aFontsDir.IsEof() )
        {
            aFontsDir.ReadLine( aLine );
            if( aLine.Len() )
                aLines.push_back( aLine );
        }
        if( aLines.begin() != aLines.end() )
            aLines.pop_front(); // not interested in the number of lines

        // copy the font files and add them to fonts.dir
        // do not overwrite existing files unless user wants it that way
        for( ::std::list< OUString >::const_iterator font_it = rFiles.begin();
             font_it != rFiles.end(); ++font_it )
        {
            INetURLObject aFrom( *font_it, INET_PROT_FILE, INetURLObject::ENCODE_ALL );
            INetURLObject aTo( aDir );
            aTo.Append( aFrom.GetName() );

            if( pCallback )
                pCallback->progress( aTo.PathToFileName() );

            if( pCallback && pCallback->isCanceled() )
                break;

            if( ! access( ByteString( aTo.PathToFileName(), aEncoding ).GetBuffer(), F_OK ) )
            {
                if( ! ( pCallback ? pCallback->queryOverwriteFile( aTo.PathToFileName() ) : false ) )
                    continue;
            }
            // look for afm if necessary
            OUString aAfmCopied;
            if( aFrom.getExtension().EqualsIgnoreCaseAscii( "pfa" ) ||
                aFrom.getExtension().EqualsIgnoreCaseAscii( "pfb" ) )
            {
                INetURLObject aFromAfm( aFrom );
                aFromAfm.setExtension( String( RTL_CONSTASCII_USTRINGPARAM( "afm" ) ) );
                if( access( ByteString( aFromAfm.PathToFileName(), aEncoding ).GetBuffer(), F_OK ) )
                {
                    aFromAfm.setExtension( String( RTL_CONSTASCII_USTRINGPARAM( "AFM" ) ) );
                    if( access( ByteString( aFromAfm.PathToFileName(), aEncoding ).GetBuffer(), F_OK ) )
                    {
                        aFromAfm.removeSegment();
                        aFromAfm.Append( String( RTL_CONSTASCII_USTRINGPARAM( "afm" ) ) );
                        aFromAfm.Append( aTo.GetName() );
                        aFromAfm.setExtension( String( RTL_CONSTASCII_USTRINGPARAM( "afm" ) ) );
                        if( access( ByteString( aFromAfm.PathToFileName(), aEncoding ).GetBuffer(), F_OK ) )
                        {
                            aFromAfm.setExtension( String( RTL_CONSTASCII_USTRINGPARAM( "AFM" ) ) );
                            if( access( ByteString( aFromAfm.PathToFileName(), aEncoding ).GetBuffer(), F_OK ) )
                            {
                                // give up
                                if( pCallback )
                                    pCallback->importFontFailed( aTo.PathToFileName(), ImportFontCallback::NoAfmMetric );
                                continue;
                            }
                        }
                    }
                }
                INetURLObject aToAfm( aTo );
                aToAfm.setExtension( String( RTL_CONSTASCII_USTRINGPARAM( "afm" ) ) );
                OUString aFromPath, aToPath;
                FileBase::normalizePath( aFromAfm.PathToFileName(), aFromPath );
                FileBase::normalizePath( aToAfm.PathToFileName(), aToPath );

                FileBase::RC nError = File::copy( aFromPath, aToPath );
                if( nError )
                {
                    if( pCallback )
                        pCallback->importFontFailed( aTo.PathToFileName(), ImportFontCallback::AfmCopyFailed );
                    continue;
                }
                aAfmCopied = aToPath;
            }
            // copy font file
            OUString aFontFrom, aFontTo;
            FileBase::normalizePath( aFrom.PathToFileName(), aFontFrom );
            FileBase::normalizePath( aTo.PathToFileName(), aFontTo );
            if( File::copy( aFontFrom, aFontTo ) )
            {
                if( aAfmCopied.getLength() )
                    File::remove( aAfmCopied );
                if( pCallback )
                    pCallback->importFontFailed( aTo.PathToFileName(), ImportFontCallback::FontCopyFailed );
                continue;
            }

            ::std::list< PrintFont* > aNewFonts;
            ::std::list< PrintFont* >::iterator it;
            if( analyzeFontFile( nDirID, OUStringToOString( aTo.GetName(), aEncoding ), true, OString(), aNewFonts ) )
            {
                // remove all fonts for the same file
                // discarding their font ids
                ::std::hash_map< fontID, PrintFont* >::iterator current, next;
                current = m_aFonts.begin();
                OString aFileName( OUStringToOString( aTo.GetName(), aEncoding ) );
                while( current != m_aFonts.end() )
                {
                    bool bRemove = false;
                    switch( current->second->m_eType )
                    {
                        case fonttype::Type1:
                            if( static_cast<Type1FontFile*>(current->second)->m_aFontFile == aFileName )
                                bRemove = true;
                            break;
                        case fonttype::TrueType:
                            if( static_cast<TrueTypeFontFile*>(current->second)->m_aFontFile == aFileName )
                                bRemove = true;
                            break;
                    }
                    if( bRemove )
                    {
                        next = current;
                        ++next;
                        delete current->second;
                        m_aFonts.erase( current );
                        current = next;
                    }
                    else
                        ++current;
                }

                DBG_ASSERT( !findFontFileID( nDirID, aFileName ), "not all fonts removed for file" );

                nSuccess++;
                for( it = aNewFonts.begin(); it != aNewFonts.end(); ++it )
                {
                    m_aFonts[ m_nNextFontID++ ] = *it;
                    aLine = ByteString( aTo.GetName(), aEncoding );
                    aLine += ' ';
                    aLine += getXLFD( *it );

                    ByteString aFile( aTo.GetName(), aEncoding );
                    for( line_it = aLines.begin(); line_it != aLines.end(); ++line_it )
                    {
                        if( line_it->GetToken( 0, ' ' ).Equals( aFile ) )
                        {
                            *line_it = aLine;
                            break;
                        }
                    }
                    if( line_it == aLines.end() )
                        aLines.push_back( aLine );
                }
            }
        }
        aFontsDir.Seek( STREAM_SEEK_TO_BEGIN );
        aFontsDir.SetStreamSize( 0 );
        aFontsDir.WriteLine( ByteString::CreateFromInt32( aLines.size() ) );
        for( line_it = aLines.begin(); line_it != aLines.end(); ++line_it )
            aFontsDir.WriteLine( *line_it );

        // rehash X font path
        Display* pDisplay = XOpenDisplay( NULL );
        if( pDisplay )
        {
            int nPaths = 0;
            char** pFontPaths = XGetFontPath( pDisplay, &nPaths );
            XSetFontPath( pDisplay, pFontPaths, nPaths );
            if( pFontPaths && nPaths )
                XFreeFontPath( pFontPaths );
            XCloseDisplay( pDisplay );
        }
    }
    else if( pCallback )
        pCallback->importFontsFailed( ImportFontCallback::NoWritableDirectory );

    return nSuccess;
}

// -------------------------------------------------------------------------

bool PrintFontManager::checkImportPossible() const
{
    bool bSuccess = false;

    // find a directory with write access
    SvFileStream aFontsDir;
    INetURLObject aDir;
    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();
    for( ::std::list< OString >::const_iterator dir_it = m_aFontDirectories.begin();
         ! ( aFontsDir.IsOpen() && aFontsDir.IsWritable() ) && dir_it != m_aFontDirectories.end(); ++dir_it )
    {
        // there must be a writable fonts.dir in that directory
        aDir = INetURLObject( OStringToOUString( *dir_it, aEncoding ), INET_PROT_FILE, INetURLObject::ENCODE_ALL );
        INetURLObject aFDir( aDir );
        ByteString aDirPath( aFDir.PathToFileName(), aEncoding );
        if( createPath( aDirPath ) )
        {
            aFDir.Append( String( RTL_CONSTASCII_USTRINGPARAM( "fonts.dir" ) ) );
            aFontsDir.Open( aFDir.PathToFileName(), STREAM_READ | STREAM_WRITE );
        }
    }
    if( aFontsDir.IsOpen() && aFontsDir.IsWritable() )
    {
#ifdef DEBUG
        fprintf( stderr, "found writable %s\n", ByteString( aFontsDir.GetFileName(), osl_getThreadTextEncoding() ).GetBuffer() );
#endif
        bSuccess = true;
    }

    return bSuccess;
}

// -------------------------------------------------------------------------

bool PrintFontManager::checkChangeFontPropertiesPossible( fontID nFontID ) const
{
    bool bSuccess = false;
    PrintFont* pFont = getFont( nFontID );
    if( pFont )
    {
        OString aFontsDirPath;
        switch( pFont->m_eType )
        {
            case fonttype::Type1:
                aFontsDirPath = getDirectory( static_cast< Type1FontFile* >(pFont)->m_nDirectory );
                break;
            case fonttype::TrueType:
                aFontsDirPath = getDirectory( static_cast< TrueTypeFontFile* >(pFont)->m_nDirectory );
                break;
        }
        if( aFontsDirPath.getLength() )
        {
            OUString aUniPath, aFDPath;
            FileBase::normalizePath( OStringToOUString( aFontsDirPath, osl_getThreadTextEncoding() ), aUniPath );
            aUniPath += OUString::createFromAscii( "/fonts.dir" );
            FileBase::getSystemPathFromNormalizedPath( aUniPath, aFDPath );
            SvFileStream aFontsDir( aFDPath, STREAM_READ | STREAM_WRITE );
            if( aFontsDir.IsOpen() && aFontsDir.IsWritable() )
                bSuccess = true;
        }
    }
    return bSuccess;
}

// -------------------------------------------------------------------------

bool PrintFontManager::changeFontProperties( fontID nFontID, const ::rtl::OUString& rXLFD )
{
    bool bSuccess = false;
    if( ! checkChangeFontPropertiesPossible( nFontID ) )
        return bSuccess;

    rtl_TextEncoding aEncoding = osl_getThreadTextEncoding();
    PrintFont* pFont = getFont( nFontID );
    OString aFontsDirPath;
    ByteString aFontFile;

    switch( pFont->m_eType )
    {
        case fonttype::Type1:
            aFontsDirPath = getDirectory( static_cast< Type1FontFile* >(pFont)->m_nDirectory );
            aFontFile = static_cast< Type1FontFile* >(pFont)->m_aFontFile;
            break;
        case fonttype::TrueType:
            aFontsDirPath = getDirectory( static_cast< TrueTypeFontFile* >(pFont)->m_nDirectory );
            aFontFile = static_cast< TrueTypeFontFile* >(pFont)->m_aFontFile;
            break;
    }
    OUString aUniPath, aFDPath;
    FileBase::normalizePath( OStringToOUString( aFontsDirPath, aEncoding ), aUniPath );
    aUniPath += OUString::createFromAscii( "/fonts.dir" );
    FileBase::getSystemPathFromNormalizedPath( aUniPath, aFDPath );

    SvFileStream aFontsDir( aFDPath, STREAM_READ | STREAM_WRITE );
    aFontsDir.SetLineDelimiter( LINEEND_LF );
    if( aFontsDir.IsOpen() && aFontsDir.IsWritable() )
    {
        ByteString aXLFD( OUStringToOString( rXLFD, RTL_TEXTENCODING_ISO_8859_1 ) );
        ::std::list< ByteString > aLines;
        ByteString aLine;
        aFontsDir.ReadLine( aLine ); // pop line count
        while( ! aFontsDir.IsEof() )
        {
            aFontsDir.ReadLine( aLine );
            if( GetCommandLineToken( 0, aLine ) == aFontFile )
            {
                bSuccess = true;
                aLine = aFontFile;
                aLine += ' ';
                aLine += aXLFD;
            }
            if( aLine.Len() )
                aLines.push_back( aLine );
        }
        if( ! bSuccess ) // ??? this should not happen
        {
            bSuccess = true;
            aLine = aFontFile;
            aLine += ' ';
            aLine += aXLFD;
            aLines.push_back( aLine );
        }
        // write the file
        aFontsDir.Seek( STREAM_SEEK_TO_BEGIN );
        aFontsDir.SetStreamSize( 0 );
        // write number of fonts
        aFontsDir.WriteLine( ByteString::CreateFromInt32( aLines.size() ) );
        while( aLines.begin() != aLines.end() )
        {
            aFontsDir.WriteLine( aLines.front() );
            aLines.pop_front();
        }
        getFontAttributesFromXLFD( pFont, aXLFD );
    }
    return bSuccess;
}
