/*************************************************************************
 *
 *  $RCSfile: metric.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-31 13:22:58 $
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

#include <impfont.hxx>
#include <metric.hxx>

#include <vector>
#include <set>

// =======================================================================

ImplFontMetric::ImplFontMetric()
:   mnRefCount( 1 ),
    mnMiscFlags( 0 ),
    mnAscent( 0 ),
    mnDescent( 0 ),
    mnIntLeading( 0 ),
    mnExtLeading( 0 ),
    mnLineHeight( 0 ),
    mnSlant( 0 )
{}

// -----------------------------------------------------------------------

inline void ImplFontMetric::AddReference()
{
    ++mnRefCount;
}

// -----------------------------------------------------------------------

inline void ImplFontMetric::DeReference()
{
    if( --mnRefCount <= 0 )
        delete this;
}

// -----------------------------------------------------------------------

bool ImplFontMetric::operator==( const ImplFontMetric& r ) const
{
    if( mnMiscFlags  != r.mnMiscFlags )
        return false;
    if( mnAscent     != r.mnAscent )
        return false;
    if( mnDescent    != r.mnDescent )
        return false;
    if( mnIntLeading != r.mnIntLeading )
        return false;
    if( mnExtLeading != r.mnExtLeading )
        return false;
    if( mnSlant      != r.mnSlant )
        return false;

    return true;
}

// =======================================================================

FontInfo::FontInfo()
:   mpImplMetric( new ImplFontMetric )
{}

// -----------------------------------------------------------------------

FontInfo::FontInfo( const FontInfo& rInfo )
:  Font( rInfo )
{
    mpImplMetric = rInfo.mpImplMetric;
    mpImplMetric->AddReference();
}

// -----------------------------------------------------------------------

FontInfo::~FontInfo()
{
    mpImplMetric->DeReference();
}

// -----------------------------------------------------------------------

FontInfo& FontInfo::operator=( const FontInfo& rInfo )
{
    Font::operator=( rInfo );

    if( mpImplMetric != rInfo.mpImplMetric )
    {
        mpImplMetric->DeReference();
        mpImplMetric = rInfo.mpImplMetric;
        mpImplMetric->AddReference();
    }

    return *this;
}

// -----------------------------------------------------------------------

BOOL FontInfo::operator==( const FontInfo& rInfo ) const
{
    if( !Font::operator==( rInfo ) )
        return FALSE;
    if( mpImplMetric == rInfo.mpImplMetric )
        return TRUE;
    if( *mpImplMetric == *rInfo.mpImplMetric  )
        return TRUE;
    return FALSE;
}

// -----------------------------------------------------------------------

FontType FontInfo::GetType() const
{
    return (mpImplMetric->IsScalable() ? TYPE_SCALABLE : TYPE_RASTER);
}

// -----------------------------------------------------------------------

BOOL FontInfo::IsDeviceFont() const
{
    return mpImplMetric->IsDeviceFont();
}

// -----------------------------------------------------------------------

BOOL FontInfo::SupportsLatin() const
{
    return mpImplMetric->SupportsLatin();
}

// -----------------------------------------------------------------------

BOOL FontInfo::SupportsCJK() const
{
    return mpImplMetric->SupportsCJK();
}

// -----------------------------------------------------------------------

BOOL FontInfo::SupportsCTL() const
{
    return mpImplMetric->SupportsCTL();
}

// =======================================================================

FontMetric::FontMetric( const FontMetric& rMetric )
:    FontInfo( rMetric )
{}

// -----------------------------------------------------------------------

long FontMetric::GetAscent() const
{
    return mpImplMetric->GetAscent();
}

// -----------------------------------------------------------------------

long FontMetric::GetDescent() const
{
    return mpImplMetric->GetDescent();
}

// -----------------------------------------------------------------------

long FontMetric::GetIntLeading() const
{
    return mpImplMetric->GetIntLeading();
}

// -----------------------------------------------------------------------

long FontMetric::GetExtLeading() const
{
    return mpImplMetric->GetExtLeading();
}

// -----------------------------------------------------------------------

long FontMetric::GetLineHeight() const
{
    return mpImplMetric->GetLineHeight();
}

// -----------------------------------------------------------------------

long FontMetric::GetSlant() const
{
    return mpImplMetric->GetSlant();
}

// -----------------------------------------------------------------------

FontMetric& FontMetric::operator =( const FontMetric& rMetric )
{
    FontInfo::operator=( rMetric );
    return *this;
}

// -----------------------------------------------------------------------

BOOL FontMetric::operator==( const FontMetric& rMetric ) const
{
    return FontInfo::operator==( rMetric );
}

// =======================================================================

ImplFontCharMap::ImplFontCharMap( int nRangePairs, const sal_uInt32* pRangeCodes )
:   mpRangeCodes( pRangeCodes ),
    mnRangeCount( nRangePairs ),
    mnCharCount( 0 ),
    mnRefCount( 1 )
{
    while( --nRangePairs >= 0 )
    {
        sal_uInt32 cFirst = *(pRangeCodes++);
        sal_uInt32 cLast  = *(pRangeCodes++);
        mnCharCount += cLast - cFirst;
    }
}

static ImplFontCharMap* pDefaultImplFontCharMap = NULL;
static const sal_uInt32 pDefaultRangeCodes[] = {0x0020,0xD800, 0xE000,0xFFF0};

// -----------------------------------------------------------------------

ImplFontCharMap::~ImplFontCharMap()
{
    if( mpRangeCodes != pDefaultRangeCodes )
        delete[] mpRangeCodes;
}

// -----------------------------------------------------------------------

ImplFontCharMap* ImplFontCharMap::GetDefaultMap()
{
    if( pDefaultImplFontCharMap )
        pDefaultImplFontCharMap->AddReference();
    else
        pDefaultImplFontCharMap = new ImplFontCharMap( 2, pDefaultRangeCodes );
    return pDefaultImplFontCharMap;
}

// -----------------------------------------------------------------------

bool ImplFontCharMap::IsDefaultMap() const
{
    return (mpRangeCodes == pDefaultRangeCodes);
}

// -----------------------------------------------------------------------

void ImplFontCharMap::AddReference()
{
    ++mnRefCount;
}

// -----------------------------------------------------------------------

void ImplFontCharMap::DeReference()
{
    if( --mnRefCount <= 0 )
        if( this != pDefaultImplFontCharMap )
            delete this;
}

// -----------------------------------------------------------------------

int ImplFontCharMap::GetCharCount() const
{
    return mnCharCount;
}

// -----------------------------------------------------------------------

int ImplFontCharMap::ImplFindRangeIndex( sal_uInt32 cChar ) const
{
    int nLower = 0;
    int nMid   = mnRangeCount;
    int nUpper = 2 * mnRangeCount - 1;
    while( nLower < nUpper )
    {
        if( cChar >= mpRangeCodes[ nMid ] )
            nLower = nMid;
        else
            nUpper = nMid - 1;
        nMid = (nLower + nUpper + 1) / 2;
    }

    return nMid;
}

// -----------------------------------------------------------------------

bool ImplFontCharMap::HasChar( sal_uInt32 cChar ) const
{
    int nRange = ImplFindRangeIndex( cChar );
    if( nRange==0 && cChar<mpRangeCodes[0] )
        return false;
    return ((nRange & 1) == 0);
}

// -----------------------------------------------------------------------

// returns the number of chars supported by the font, which
// are inside the unicode range from cMin to cMax (inclusive)
int ImplFontCharMap::CountCharsInRange( sal_uInt32 cMin, sal_uInt32 cMax ) const
{
    int nCount = 0;

    // find and adjust range and char count for cMin
    int nRangeMin = ImplFindRangeIndex( cMin );
    if( nRangeMin & 1 )
        ++nRangeMin;
    else if( cMin > mpRangeCodes[ nRangeMin ] )
        nCount -= cMin - mpRangeCodes[ nRangeMin ];

    // find and adjust range and char count for cMax
    int nRangeMax = ImplFindRangeIndex( cMax );
    if( nRangeMax & 1 )
        --nRangeMax;
    else
        nCount -= mpRangeCodes[ nRangeMax+1 ] - cMax - 1;

    // count chars in complete ranges between cMin and cMax
    for( int i = nRangeMin; i <= nRangeMax; i+=2 )
        nCount += mpRangeCodes[i+1] - mpRangeCodes[i];

    return nCount;
}

// -----------------------------------------------------------------------

sal_uInt32 ImplFontCharMap::GetFirstChar() const
{
    return mpRangeCodes[0];
}

// -----------------------------------------------------------------------

sal_uInt32 ImplFontCharMap::GetLastChar() const
{
    return (mpRangeCodes[ 2*mnRangeCount-1 ] - 1);
}

// -----------------------------------------------------------------------

sal_uInt32 ImplFontCharMap::GetNextChar( sal_uInt32 cChar ) const
{
    if( cChar < GetFirstChar() )
        return GetFirstChar();
    if( cChar >= GetLastChar() )
        return GetLastChar();

    int nRange = ImplFindRangeIndex( cChar + 1 );
    if( nRange & 1 )                       // outside of range?
        return mpRangeCodes[ nRange + 1 ]; // => first in next range
    return (cChar + 1);
}

// -----------------------------------------------------------------------

sal_uInt32 ImplFontCharMap::GetPrevChar( sal_uInt32 cChar ) const
{
    if( cChar <= GetFirstChar() )
        return GetFirstChar();
    if( cChar > GetLastChar() )
        return GetLastChar();

    int nRange = ImplFindRangeIndex( cChar - 1 );
    if( nRange & 1 )                            // outside a range?
        return (mpRangeCodes[ nRange ] - 1);    // => last in prev range
    return (cChar - 1);
}

// -----------------------------------------------------------------------

int ImplFontCharMap::GetIndexFromChar( sal_uInt32 cChar ) const
{
    // TODO: improve linear walk?
    int nCharIndex = 0;
    const sal_uInt32* pRange = &mpRangeCodes[0];
    for( int i = 0; i < mnRangeCount; ++i )
    {
        sal_uInt32 cFirst = *(pRange++);
        sal_uInt32 cLast  = *(pRange++);
        if( cChar >= cLast )
            nCharIndex += cLast - cFirst;
        else if( cChar >= cFirst )
            return nCharIndex + (cChar - cFirst);
        else
            break;
    }

    return -1;
}

// -----------------------------------------------------------------------

sal_uInt32 ImplFontCharMap::GetCharFromIndex( int nCharIndex ) const
{
    // TODO: improve linear walk?
    const sal_uInt32* pRange = &mpRangeCodes[0];
    for( int i = 0; i < mnRangeCount; ++i )
    {
        sal_uInt32 cFirst = *(pRange++);
        sal_uInt32 cLast  = *(pRange++);
        nCharIndex -= cLast - cFirst;
        if( nCharIndex < 0 )
            return (cLast + nCharIndex);
    }

    // we can only get here with an out-of-bounds charindex
    return mpRangeCodes[0];
}

// =======================================================================

static unsigned GetUInt( const unsigned char* p ) { return((p[0]<<24)+(p[1]<<16)+(p[2]<<8)+p[3]);}
static unsigned GetUShort( const unsigned char* p ){ return((p[0]<<8)+p[1]);}

// TODO: move CMAP parsing directly into the ImplFontCharMap class
bool ParseCMAP( const unsigned char* pCmap, int nLength, CmapResult& rResult )
{
    if( GetUShort( pCmap ) ) // simple check for CMAP corruption
        return false;

    // find the most interesting subtable in the CMAP
    rResult.mbRecoded = false;
    rtl_TextEncoding eRecodeFrom = RTL_TEXTENCODING_UNICODE;
    int nOffset = 0;
    int nFormat = -1;
    int nBestVal = 0;
    int nSubTables = GetUShort( pCmap + 2 );
    for( const unsigned char* p = pCmap + 4; --nSubTables >= 0; p += 8 )
    {
        int nPlatform = GetUShort( p );
        int nEncoding = GetUShort( p+2 );
        int nPlatformEncoding = (nPlatform << 8) + nEncoding;

        int nValue;
        rtl_TextEncoding eTmpEncoding = RTL_TEXTENCODING_UNICODE;
        switch( nPlatformEncoding )
        {
            case 0x000: nValue = 20; break;                             // Unicode 1.0
            case 0x001: nValue = 21; break;                             // Unicode 1.1
            case 0x002: nValue = 22; break;                             // iso10646_1993
            case 0x003: nValue = 23; break;                             // UCS-2
            case 0x004: nValue = 24; break;                             // UCS-4
            case 0x100: nValue = 22; break;                             // Mac Unicode<2.0
            case 0x103: nValue = 23; break;                             // Mac Unicode>2.0
            case 0x300: nValue =  5; rResult.mbSymbolic = true; break;  // Win Symbol
            case 0x301: nValue = 28; break;                             // Win UCS-2
            case 0x30A: nValue = 29; break;                             // Win-UCS-4
            case 0x302: nValue = 11; eTmpEncoding = RTL_TEXTENCODING_SHIFT_JIS; break;
            case 0x303: nValue = 12; eTmpEncoding = RTL_TEXTENCODING_GB_18030; break;
            case 0x304: nValue = 11; eTmpEncoding = RTL_TEXTENCODING_BIG5; break;
            case 0x305: nValue = 11; eTmpEncoding = RTL_TEXTENCODING_MS_949; break;
            case 0x306: nValue = 11; eTmpEncoding = RTL_TEXTENCODING_MS_1361; break;
            default:    nValue = 0; break;
        }

        if( nValue <= 0 )   // ignore unknown encodings
            continue;

        int nTmpOffset = GetUInt( p+4 );
        int nTmpFormat = GetUShort( pCmap + nTmpOffset );
        if( nTmpFormat == 12 )                  // 32bit code -> glyph map format
            nValue += 3;
        else if( nTmpFormat != 4 )              // 16bit code -> glyph map format
            continue;                           // ignore other formats

        if( nBestVal < nValue )
        {
            nBestVal = nValue;
            nOffset = nTmpOffset;
            nFormat = nTmpFormat;
            eRecodeFrom = eTmpEncoding;
        }
    }

    // parse the best CMAP subtable
    int nRangeCount = 0;
    sal_uInt32* pCodePairs = NULL;

    // format 4, the most common 16bit char mapping table
    if( (nFormat == 4) && ((nOffset+16) < nLength) )
    {
        int nSegCount = GetUShort( pCmap + nOffset + 6 );
        nRangeCount = nSegCount/2 - 1;
        pCodePairs = new sal_uInt32[ nRangeCount * 2 ];
        const unsigned char* pLimit = pCmap + nOffset + 14;
        const unsigned char* pBegin = pLimit + 2 + nSegCount;
        sal_uInt32* pCP = pCodePairs;
        for( int i = 0; i < nRangeCount; ++i )
        {
            sal_uInt32 cMinChar = GetUShort( pBegin + 2*i );
            sal_uInt32 cMaxChar = GetUShort( pLimit + 2*i );
            if( cMinChar > cMaxChar )   // no sane font should trigger this
                break;
            if( cMaxChar == 0xFFFF )
                break;
            *(pCP++) = cMinChar;
            *(pCP++) = cMaxChar + 1;
        }
        nRangeCount = (pCP - pCodePairs) / 2;
    }
    // format 12, the most common 32bit char mapping table
    else if( (nFormat == 12) && ((nOffset+16) < nLength) )
    {
        nRangeCount = GetUInt( pCmap + nOffset + 12 );
        pCodePairs = new sal_uInt32[ nRangeCount * 2 ];
        const unsigned char* pGroup = pCmap + nOffset + 16;
        sal_uInt32* pCP = pCodePairs;
        for( int i = 0; i < nRangeCount; ++i )
        {
            sal_uInt32 cMinChar = GetUInt( pGroup + 0 );
            sal_uInt32 cMaxChar = GetUInt( pGroup + 4 );
            pGroup += 12;
#if 1       // TODO: remove unicode baseplane clipping for UCS-4 support
            if( cMinChar > 0xFFFF )
                continue;
            if( cMaxChar > 0xFFFF )
                cMaxChar = 0xFFFF;
#else
            if( cMinChar > cMaxChar )   // no sane font should trigger this
                break;
#endif
            *(pCP++) = cMinChar;
            *(pCP++) = cMaxChar + 1;
        }
        nRangeCount = (pCP - pCodePairs) / 2;
    }

    if( nRangeCount <= 0 )
    {
        // even when no CMAP is available we know it for symbol fonts
        if( rResult.mbSymbolic )
        {
            nRangeCount = 2;
            pCodePairs = new sal_uInt32[4];
            pCodePairs[0] = 0x0020;    // aliased symbols
            pCodePairs[1] = 0x0100;
            pCodePairs[2] = 0xF020;    // original symbols
            pCodePairs[3] = 0xF100;
            return true;
        }

        return false;
    }

    // recode the code ranges to their unicode encoded ranges if needed
    rtl_TextToUnicodeConverter aConverter = NULL;
    rtl_UnicodeToTextContext aCvtContext = NULL;

    rResult.mbRecoded = ( eRecodeFrom != RTL_TEXTENCODING_UNICODE );
    if( rResult.mbRecoded )
    {
        aConverter = rtl_createTextToUnicodeConverter( eRecodeFrom );
        aCvtContext = rtl_createTextToUnicodeContext( aConverter );
    }

    if( aConverter && aCvtContext )
    {
        // determine the set of supported unicodes from encoded ranges
        typedef std::set<sal_uInt32> IntSet;
        IntSet aSupportedUnicodes;

        static const int NINSIZE = 64;
        static const int NOUTSIZE = 64;
        sal_Char    cCharsInp[ NINSIZE ];
        sal_Unicode cCharsOut[ NOUTSIZE ];
        sal_uInt32* pCP = pCodePairs;
        for( int i = 0; i < nRangeCount; ++i )
        {
            sal_uInt32 cMin = *(pCP++);
            sal_uInt32 cEnd = *(pCP++);
            while( cMin < cEnd )
            {
                int j = 0;
                for(; (cMin < cEnd) && (j < NINSIZE); ++cMin )
                {
                    if( cMin >= 0x0100 )
                        cCharsInp[ j++ ] = static_cast<sal_Char>(cMin >> 8);
                    if( (cMin >= 0x0100) || (cMin < 0x00A0)  )
                        cCharsInp[ j++ ] = static_cast<sal_Char>(cMin);
                }

                sal_uInt32 nCvtInfo;
                sal_Size nSrcCvtBytes;
                int nOutLen = rtl_convertTextToUnicode(
                    aConverter, aCvtContext,
                    cCharsInp, j, cCharsOut, NOUTSIZE,
                    RTL_TEXTTOUNICODE_FLAGS_INVALID_IGNORE
                    | RTL_TEXTTOUNICODE_FLAGS_UNDEFINED_IGNORE,
                    &nCvtInfo, &nSrcCvtBytes );

                for( j = 0; j < nOutLen; ++j )
                    aSupportedUnicodes.insert( cCharsOut[j] );
            }
        }

        rtl_destroyTextToUnicodeConverter( aCvtContext );
        rtl_destroyTextToUnicodeConverter( aConverter );

        // convert the set of supported unicodes to ranges
        typedef std::vector<sal_uInt32> IntVector;
        IntVector aSupportedRanges;

        IntSet::const_iterator itChar = aSupportedUnicodes.begin();
        for(; itChar != aSupportedUnicodes.end(); ++itChar )
        {
            if( aSupportedRanges.empty()
            || (aSupportedRanges.back() != *itChar) )
            {
                // add new range beginning with current unicode
                aSupportedRanges.push_back( *itChar );
                aSupportedRanges.push_back( 0 );
            }

            // extend existing range to include current unicode
            aSupportedRanges.back() = *itChar + 1;
        }

        // make a pCodePairs array using the vector from above
        delete[] pCodePairs;
        nRangeCount = aSupportedRanges.size() / 2;
        if( nRangeCount <= 0 )
            return false;
        pCodePairs = new sal_uInt32[ nRangeCount * 2 ];
        IntVector::const_iterator itInt = aSupportedRanges.begin();
        for( pCP = pCodePairs; itInt != aSupportedRanges.end(); ++itInt )
            *(pCP++) = *itInt;
    }

    rResult.mpCodes = pCodePairs;
    rResult.mnCount = nRangeCount;
    return true;
}

// =======================================================================

FontCharMap::FontCharMap()
:   mpImpl( ImplFontCharMap::GetDefaultMap() )
{}

// -----------------------------------------------------------------------

FontCharMap::~FontCharMap()
{
    mpImpl->DeReference();
    mpImpl = NULL;
}

// -----------------------------------------------------------------------

int FontCharMap::GetCharCount() const
{
    return mpImpl->GetCharCount();
}

// -----------------------------------------------------------------------

int FontCharMap::CountCharsInRange( sal_uInt32 cMin, sal_uInt32 cMax ) const
{
    return mpImpl->CountCharsInRange( cMin, cMax );
}

// -----------------------------------------------------------------------

void FontCharMap::Reset( ImplFontCharMap* pNewMap )
{
    if( pNewMap == NULL )
    {
        mpImpl->DeReference();
        mpImpl = ImplFontCharMap::GetDefaultMap();
    }
    else if( pNewMap != mpImpl )
    {
        mpImpl->DeReference();
        mpImpl = pNewMap;
        mpImpl->AddReference();
    }
}

// -----------------------------------------------------------------------

BOOL FontCharMap::IsDefaultMap() const
{
    return mpImpl->IsDefaultMap();
}

// -----------------------------------------------------------------------

BOOL FontCharMap::HasChar( sal_uInt32 cChar ) const
{
    return mpImpl->HasChar( cChar );
}

// -----------------------------------------------------------------------

sal_uInt32 FontCharMap::GetFirstChar() const
{
    return mpImpl->GetFirstChar();
}

// -----------------------------------------------------------------------

sal_uInt32 FontCharMap::GetLastChar() const
{
    return mpImpl->GetLastChar();
}

// -----------------------------------------------------------------------

sal_uInt32 FontCharMap::GetNextChar( sal_uInt32 cChar ) const
{
    return mpImpl->GetNextChar( cChar );
}

// -----------------------------------------------------------------------

sal_uInt32 FontCharMap::GetPrevChar( sal_uInt32 cChar ) const
{
    return mpImpl->GetPrevChar( cChar );
}

// -----------------------------------------------------------------------

int FontCharMap::GetIndexFromChar( sal_uInt32 cChar ) const
{
    return mpImpl->GetIndexFromChar( cChar );
}

// -----------------------------------------------------------------------

sal_uInt32 FontCharMap::GetCharFromIndex( int nIndex ) const
{
    return mpImpl->GetCharFromIndex( nIndex );
}

// =======================================================================
