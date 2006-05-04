/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: salgdi3.cxx,v $
 *
 *  $Revision: 1.78 $
 *
 *  last change: $Author: rt $ $Date: 2006-05-04 15:14:05 $
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

#include <string.h>
#include <malloc.h>

#ifndef _SVWIN_H
#include <tools/svwin.h>
#endif

#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif
#ifndef _OSL_FILE_HXX
#include <osl/file.hxx>
#endif
#ifndef _OSL_THREAD_HXX
#include <osl/thread.hxx>
#endif
#ifndef _OSL_PROCESS_HXX
#include <osl/process.h>
#endif

#ifndef _SV_SVAPP_HXX
#include <svapp.hxx>
#endif
#ifndef _SV_WINCOMP_HXX
#include <wincomp.hxx>
#endif
#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALGDI_H
#include <salgdi.h>
#endif
#ifndef _SV_OUTFONT_HXX
#include <outfont.hxx>
#endif
#ifndef _SV_FONT_HXX
#include <font.hxx>
#endif
#ifndef _SV_SALLAYOUT_HXX
#include <sallayout.hxx>
#endif

#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif
#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif


#ifndef _DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef __SUBFONT_H
#include <psprint/list.h>
#include <psprint/sft.h>
#endif
#ifndef _RTL_TEXTCVT_H
#include <rtl/textcvt.h>
#endif

#ifdef GCP_KERN_HACK
#include <algorithm>
#endif

#include <tools/stream.hxx>
#include <rtl/bootstrap.hxx>


#include <vector>
#include <set>

#ifndef INCLUDED_MAP
#include <map>
#define INCLUDED_MAP
#endif


static const int MAXFONTHEIGHT = 2048;

// -----------
// - Inlines -
// -----------

inline FIXED FixedFromDouble( double d )
{
    const long l = (long) ( d * 65536. );
    return *(FIXED*) &l;
}

// -----------------------------------------------------------------------

inline int IntTimes256FromFixed(FIXED f)
{
    int nFixedTimes256 = (f.value << 8) + ((f.fract+0x80) >> 8);
    return nFixedTimes256;
}

// =======================================================================

// these variables can be static because they store system wide settings
static bool bImplSalCourierScalable = false;
static bool bImplSalCourierNew = false;


// =======================================================================

typedef std::map< String, String > FontNameMap;

class ImplFontNameCache
{
private:
    FontNameMap     aFontNames;
    rtl::OUString   aCacheFileName;
    String          aBaseURL;
    BOOL            bModified;

protected:
    String  OptimizeURL( const String& rURL ) const;

public:
            ImplFontNameCache( const String& rCacheFileName, const String& rBaseURL );
            ~ImplFontNameCache();

    String  GetFontName( const String& rFontFileName ) const;
    void    AddFontName( const String& rFontFileName, const String& rFontName );
};

ImplFontNameCache::ImplFontNameCache( const String& rFileNameURL, const String& rBaseURL ) : aBaseURL( rBaseURL )
{
    bModified = FALSE;
    aBaseURL.ToLowerAscii();    // Windows only, no problem...

    osl::FileBase::getSystemPathFromFileURL( rFileNameURL, aCacheFileName );

    SvFileStream aCacheFile( aCacheFileName, STREAM_READ );
    if ( aCacheFile.IsOpen() )
    {
        // read...
        String aFontFileURL, aFontName;
        do
        {
            aCacheFile.ReadByteString( aFontFileURL, RTL_TEXTENCODING_UTF8 );
            aCacheFile.ReadByteString( aFontName, RTL_TEXTENCODING_UTF8 );
            if ( aFontFileURL.Len() )
                aFontNames.insert( FontNameMap::value_type(aFontFileURL,aFontName) );
        }
        while ( aFontFileURL.Len() );
    }
}

ImplFontNameCache::~ImplFontNameCache()
{
    if ( bModified )
    {
        SvFileStream aCacheFile( aCacheFileName, STREAM_WRITE|STREAM_TRUNC );
        if ( aCacheFile.IsWritable() )
        {
            FontNameMap::const_iterator aIter = aFontNames.begin();
            while ( aIter != aFontNames.end() )
            {
                String aFontFileURL( (*aIter).first );
                String aFontName( (*aIter).second );
                aCacheFile.WriteByteString( aFontFileURL, RTL_TEXTENCODING_UTF8 );
                aCacheFile.WriteByteString( aFontName, RTL_TEXTENCODING_UTF8 );
                aIter++;
            }
            // EOF Marker
            String aEmptyStr;
            aCacheFile.WriteByteString( aEmptyStr, RTL_TEXTENCODING_UTF8 );
            aCacheFile.WriteByteString( aEmptyStr, RTL_TEXTENCODING_UTF8 );
        }
    }
}

String ImplFontNameCache::OptimizeURL( const String& rURL ) const
{
    String aOptimizedFontFileURL( rURL );
    aOptimizedFontFileURL.ToLowerAscii();   // Windows only, no problem...
    if ( aOptimizedFontFileURL.CompareTo( aBaseURL, aBaseURL.Len() ) == COMPARE_EQUAL )
        aOptimizedFontFileURL = aOptimizedFontFileURL.Copy( aBaseURL.Len() );
    return aOptimizedFontFileURL;
}

String ImplFontNameCache::GetFontName( const String& rFontFileName ) const
{
    String aName;
    FontNameMap::const_iterator it = aFontNames.find( OptimizeURL( rFontFileName ) );
    if( it != aFontNames.end() )
    {
        aName = it->second;
    }
    return aName;
}

void ImplFontNameCache::AddFontName( const String& rFontFileName, const String& rFontName )
{
    DBG_ASSERT( rFontFileName.Len() && rFontName.Len(), "ImplFontNameCache::AddFontName - invalid data!" );
    if ( rFontFileName.Len() && rFontName.Len() )
    {
        aFontNames.insert( FontNameMap::value_type( OptimizeURL( rFontFileName ),rFontName ) );
        bModified = TRUE;
    }
}

// =======================================================================

struct ImplEnumInfo
{
    HDC                 mhDC;
    ImplDevFontList*    mpList;
    String*             mpName;
    LOGFONTA*           mpLogFontA;
    LOGFONTW*           mpLogFontW;
    UINT                mnPreferedCharSet;
    bool                mbCourier;
    bool                mbImplSalCourierScalable;
    bool                mbImplSalCourierNew;
    bool                mbPrinter;
};

// =======================================================================

static CharSet ImplCharSetToSal( BYTE nCharSet )
{
    rtl_TextEncoding eTextEncoding;

    if ( nCharSet == OEM_CHARSET )
    {
        UINT nCP = (USHORT)GetOEMCP();
        switch ( nCP )
        {
            // It is unclear why these two (undefined?) code page numbers are
            // handled specially here:
            case 1004:  eTextEncoding = RTL_TEXTENCODING_MS_1252; break;
            case 65400: eTextEncoding = RTL_TEXTENCODING_SYMBOL; break;
            default:
                eTextEncoding = rtl_getTextEncodingFromWindowsCodePage(nCP);
                break;
        };
    }
    else
    {
        if( nCharSet )
            eTextEncoding = rtl_getTextEncodingFromWindowsCharset( nCharSet );
        else
            eTextEncoding = RTL_TEXTENCODING_UNICODE;
    }

    return eTextEncoding;
}

// -----------------------------------------------------------------------

static BYTE ImplCharSetToWin( CharSet eCharSet )
{
    return rtl_getBestWindowsCharsetFromTextEncoding( eCharSet );
}

// -----------------------------------------------------------------------

static FontFamily ImplFamilyToSal( BYTE nFamily )
{
    switch ( nFamily & 0xF0 )
    {
        case FF_DECORATIVE:
            return FAMILY_DECORATIVE;

        case FF_MODERN:
            return FAMILY_MODERN;

        case FF_ROMAN:
            return FAMILY_ROMAN;

        case FF_SCRIPT:
            return FAMILY_SCRIPT;

        case FF_SWISS:
            return FAMILY_SWISS;
    }

    return FAMILY_DONTKNOW;
}

// -----------------------------------------------------------------------

static BYTE ImplFamilyToWin( FontFamily eFamily )
{
    switch ( eFamily )
    {
        case FAMILY_DECORATIVE:
            return FF_DECORATIVE;

        case FAMILY_MODERN:
            return FF_MODERN;

        case FAMILY_ROMAN:
            return FF_ROMAN;

        case FAMILY_SCRIPT:
            return FF_SCRIPT;

        case FAMILY_SWISS:
            return FF_SWISS;

        case FAMILY_SYSTEM:
            return FF_SWISS;
    }

    return FF_DONTCARE;
}

// -----------------------------------------------------------------------

static FontWeight ImplWeightToSal( int nWeight )
{
    if ( nWeight <= FW_THIN )
        return WEIGHT_THIN;
    else if ( nWeight <= FW_ULTRALIGHT )
        return WEIGHT_ULTRALIGHT;
    else if ( nWeight <= FW_LIGHT )
        return WEIGHT_LIGHT;
    else if ( nWeight < FW_MEDIUM )
        return WEIGHT_NORMAL;
    else if ( nWeight == FW_MEDIUM )
        return WEIGHT_MEDIUM;
    else if ( nWeight <= FW_SEMIBOLD )
        return WEIGHT_SEMIBOLD;
    else if ( nWeight <= FW_BOLD )
        return WEIGHT_BOLD;
    else if ( nWeight <= FW_ULTRABOLD )
        return WEIGHT_ULTRABOLD;
    else
        return WEIGHT_BLACK;
}

// -----------------------------------------------------------------------

static int ImplWeightToWin( FontWeight eWeight )
{
    switch ( eWeight )
    {
        case WEIGHT_THIN:
            return FW_THIN;

        case WEIGHT_ULTRALIGHT:
            return FW_ULTRALIGHT;

        case WEIGHT_LIGHT:
            return FW_LIGHT;

        case WEIGHT_SEMILIGHT:
        case WEIGHT_NORMAL:
            return FW_NORMAL;

        case WEIGHT_MEDIUM:
            return FW_MEDIUM;

        case WEIGHT_SEMIBOLD:
            return FW_SEMIBOLD;

        case WEIGHT_BOLD:
            return FW_BOLD;

        case WEIGHT_ULTRABOLD:
            return FW_ULTRABOLD;

        case WEIGHT_BLACK:
            return FW_BLACK;
    }

    return 0;
}

// -----------------------------------------------------------------------

inline FontPitch ImplLogPitchToSal( BYTE nPitch )
{
    if ( nPitch & FIXED_PITCH )
        return PITCH_FIXED;
    else
        return PITCH_VARIABLE;
}

// -----------------------------------------------------------------------

inline FontPitch ImplMetricPitchToSal( BYTE nPitch )
{
    // Sausaecke bei MS !! siehe NT Hilfe
    if ( !(nPitch & TMPF_FIXED_PITCH) )
        return PITCH_FIXED;
    else
        return PITCH_VARIABLE;
}

// -----------------------------------------------------------------------

inline BYTE ImplPitchToWin( FontPitch ePitch )
{
    if ( ePitch == PITCH_FIXED )
        return FIXED_PITCH;
    else if ( ePitch == PITCH_VARIABLE )
        return VARIABLE_PITCH;
    else
        return DEFAULT_PITCH;
}

// -----------------------------------------------------------------------

static ImplDevFontAttributes WinFont2DevFontAttributes( const ENUMLOGFONTEXA& rEnumFont,
    const NEWTEXTMETRICA& rMetric, DWORD nFontType )
{
    ImplDevFontAttributes aDFA;

    const LOGFONTA rLogFont = rEnumFont.elfLogFont;

    // get font face attributes
    aDFA.meFamily       = ImplFamilyToSal( rLogFont.lfPitchAndFamily );
    aDFA.meWidthType    = WIDTH_DONTKNOW;
    aDFA.meWeight       = ImplWeightToSal( rLogFont.lfWeight );
    aDFA.meItalic       = (rLogFont.lfItalic) ? ITALIC_NORMAL : ITALIC_NONE;
    aDFA.mePitch        = ImplLogPitchToSal( rLogFont.lfPitchAndFamily );
    aDFA.mbSymbolFlag   = (rLogFont.lfCharSet == SYMBOL_CHARSET);

    // get the font face name
    aDFA.maName = ImplSalGetUniString( rLogFont.lfFaceName );

    // use the face's style name only if it looks reasonable
    const char* pStyleName = (const char*)rEnumFont.elfStyle;
    const char* pEnd = pStyleName + sizeof( rEnumFont.elfStyle );
    const char* p = pStyleName;
    for(; *p && (p < pEnd); ++p )
        if( (0x00 < *p) && (*p < 0x20) )
            break;
    if( p < pEnd )
        aDFA.maStyleName = ImplSalGetUniString( pStyleName );

    // get device specific font attributes
    aDFA.mbOrientation  = (nFontType & RASTER_FONTTYPE) == 0;
    aDFA.mbDevice       = (rMetric.tmPitchAndFamily & TMPF_DEVICE) != 0;

    aDFA.mbEmbeddable   = false;
    aDFA.mbSubsettable  = false;
    if( (rMetric.tmPitchAndFamily & TMPF_TRUETYPE) != 0 )
        if( (rMetric.tmPitchAndFamily & TMPF_DEVICE) == 0 )
            // TODO: implement type1 or CFF subsetting
            if( 0 == (rMetric.ntmFlags & (NTM_PS_OPENTYPE | NTM_TYPE1) ) )
                aDFA.mbSubsettable = true;
    // for now we can only embed Type1 fonts
    if( 0 != (rMetric.ntmFlags & NTM_TYPE1 ) )
        aDFA.mbEmbeddable = true;

    // heuristics for font quality
    // -   standard-type1 > opentypeTT > truetype > non-standard-type1 > raster
    // -   subsetting > embedding > none
    aDFA.mnQuality = 0;
    if( rMetric.tmPitchAndFamily & TMPF_TRUETYPE )
        aDFA.mnQuality += 50;
    if( rMetric.ntmFlags & NTM_TT_OPENTYPE )
        aDFA.mnQuality += 10;
    if( aDFA.mbSubsettable )
        aDFA.mnQuality += 200;
    else if( aDFA.mbEmbeddable )
        aDFA.mnQuality += 100;

    // #i38665# prefer Type1 versions of the standard postscript fonts
    if( aDFA.mbEmbeddable )
    {
        if( aDFA.maName.EqualsAscii( "AvantGarde" )
        ||  aDFA.maName.EqualsAscii( "Bookman" )
        ||  aDFA.maName.EqualsAscii( "Courier" )
        ||  aDFA.maName.EqualsAscii( "Helvetica" )
        ||  aDFA.maName.EqualsAscii( "NewCenturySchlbk" )
        ||  aDFA.maName.EqualsAscii( "Palatino" )
        ||  aDFA.maName.EqualsAscii( "Symbol" )
        ||  aDFA.maName.EqualsAscii( "Times" )
        ||  aDFA.maName.EqualsAscii( "ZapfChancery" )
        ||  aDFA.maName.EqualsAscii( "ZapfDingbats" ) )
            aDFA.mnQuality += 500;
    }

    aDFA.meEmbeddedBitmap = EMBEDDEDBITMAP_DONTKNOW;
    aDFA.meAntiAlias = ANTIALIAS_DONTKNOW;

    // TODO: add alias names

    return aDFA;
}

// -----------------------------------------------------------------------

static ImplDevFontAttributes WinFont2DevFontAttributes( const ENUMLOGFONTEXW& rEnumFont,
    const NEWTEXTMETRICW& rMetric, DWORD nFontType )
{
    ImplDevFontAttributes aDFA;

    const LOGFONTW rLogFont = rEnumFont.elfLogFont;

    // get font face attributes
    aDFA.meFamily       = ImplFamilyToSal( rLogFont.lfPitchAndFamily );
    aDFA.meWidthType    = WIDTH_DONTKNOW;
    aDFA.meWeight       = ImplWeightToSal( rLogFont.lfWeight );
    aDFA.meItalic       = (rLogFont.lfItalic) ? ITALIC_NORMAL : ITALIC_NONE;
    aDFA.mePitch        = ImplLogPitchToSal( rLogFont.lfPitchAndFamily );
    aDFA.mbSymbolFlag   = (rLogFont.lfCharSet == SYMBOL_CHARSET);

    // get the font face name
    aDFA.maName = rLogFont.lfFaceName;

    // use the face's style name only if it looks reasonable
    const wchar_t* pStyleName = rEnumFont.elfStyle;
    const wchar_t* pEnd = pStyleName + sizeof(rEnumFont.elfStyle)/sizeof(*rEnumFont.elfStyle);
    const wchar_t* p = pStyleName;
    for(; *p && (p < pEnd); ++p )
        if( *p < 0x0020 )
            break;
    if( p < pEnd )
        aDFA.maStyleName = pStyleName;

    // get device specific font attributes
    aDFA.mbOrientation  = (nFontType & RASTER_FONTTYPE) == 0;
    aDFA.mbDevice       = (rMetric.tmPitchAndFamily & TMPF_DEVICE) != 0;

    aDFA.mbEmbeddable   = false;
    aDFA.mbSubsettable  = false;
    if( (rMetric.tmPitchAndFamily & TMPF_TRUETYPE) != 0 )
        if( (rMetric.tmPitchAndFamily & TMPF_DEVICE) == 0 )
            // TODO: implement type1 or CFF subsetting
            if( 0 == (rMetric.ntmFlags & (NTM_PS_OPENTYPE | NTM_TYPE1) ) )
                aDFA.mbSubsettable = true;
    // for now we can only embed Type1 fonts
    if( rMetric.ntmFlags & NTM_TYPE1 )
        aDFA.mbEmbeddable = true;

    // heuristics for font quality
    // -   standard-type1 > opentypeTT > truetype > non-standard-type1 > raster
    // -   subsetting > embedding > none
    aDFA.mnQuality = 0;
    if( rMetric.tmPitchAndFamily & TMPF_TRUETYPE )
        aDFA.mnQuality += 50;
    if( rMetric.ntmFlags & NTM_TT_OPENTYPE )
        aDFA.mnQuality += 10;
    if( aDFA.mbSubsettable )
        aDFA.mnQuality += 200;
    else if( aDFA.mbEmbeddable )
        aDFA.mnQuality += 100;

    // #i38665# prefer Type1 versions of the standard postscript fonts
    if( aDFA.mbEmbeddable )
    {
        if( aDFA.maName.EqualsAscii( "AvantGarde" )
        ||  aDFA.maName.EqualsAscii( "Bookman" )
        ||  aDFA.maName.EqualsAscii( "Courier" )
        ||  aDFA.maName.EqualsAscii( "Helvetica" )
        ||  aDFA.maName.EqualsAscii( "NewCenturySchlbk" )
        ||  aDFA.maName.EqualsAscii( "Palatino" )
        ||  aDFA.maName.EqualsAscii( "Symbol" )
        ||  aDFA.maName.EqualsAscii( "Times" )
        ||  aDFA.maName.EqualsAscii( "ZapfChancery" )
        ||  aDFA.maName.EqualsAscii( "ZapfDingbats" ) )
            aDFA.mnQuality += 500;
    }

    aDFA.meEmbeddedBitmap = EMBEDDEDBITMAP_DONTKNOW;
    aDFA.meAntiAlias = ANTIALIAS_DONTKNOW;

    // TODO: add alias names
    return aDFA;
}

// -----------------------------------------------------------------------

static ImplFontData* ImplLogMetricToDevFontDataA( const ENUMLOGFONTEXA* pLogFont,
                                         const NEWTEXTMETRICA* pMetric,
                                         DWORD nFontType )
{
    int nHeight = 0;
    if ( nFontType & RASTER_FONTTYPE )
        nHeight = pMetric->tmHeight - pMetric->tmInternalLeading;

    ImplFontData* pData = new ImplWinFontData(
        WinFont2DevFontAttributes(*pLogFont, *pMetric, nFontType),
        nHeight,
        pLogFont->elfLogFont.lfCharSet,
        pMetric->tmPitchAndFamily );

    return pData;
}

// -----------------------------------------------------------------------

static ImplFontData* ImplLogMetricToDevFontDataW( const ENUMLOGFONTEXW* pLogFont,
                                         const NEWTEXTMETRICW* pMetric,
                                         DWORD nFontType )
{
    int nHeight = 0;
    if ( nFontType & RASTER_FONTTYPE )
        nHeight = pMetric->tmHeight - pMetric->tmInternalLeading;

    ImplFontData* pData = new ImplWinFontData(
        WinFont2DevFontAttributes(*pLogFont, *pMetric, nFontType),
        nHeight,
        pLogFont->elfLogFont.lfCharSet,
        pMetric->tmPitchAndFamily );

    return pData;
}

// -----------------------------------------------------------------------

void ImplSalLogFontToFontA( HDC hDC, const LOGFONTA& rLogFont, Font& rFont )
{
    String aFontName( ImplSalGetUniString( rLogFont.lfFaceName ) );
    if ( aFontName.Len() )
    {
        rFont.SetName( aFontName );
        rFont.SetCharSet( ImplCharSetToSal( rLogFont.lfCharSet ) );
        rFont.SetFamily( ImplFamilyToSal( rLogFont.lfPitchAndFamily ) );
        rFont.SetPitch( ImplLogPitchToSal( rLogFont.lfPitchAndFamily ) );
        rFont.SetWeight( ImplWeightToSal( rLogFont.lfWeight ) );

        long nFontHeight = rLogFont.lfHeight;
        if ( nFontHeight < 0 )
            nFontHeight = -nFontHeight;
        long nDPIY = GetDeviceCaps( hDC, LOGPIXELSY );
        if( !nDPIY )
            nDPIY = 600;
        nFontHeight *= 72;
        nFontHeight += nDPIY/2;
        nFontHeight /= nDPIY;
        rFont.SetSize( Size( 0, nFontHeight ) );
        rFont.SetOrientation( (short)rLogFont.lfEscapement );
        if ( rLogFont.lfItalic )
            rFont.SetItalic( ITALIC_NORMAL );
        else
            rFont.SetItalic( ITALIC_NONE );
        if ( rLogFont.lfUnderline )
            rFont.SetUnderline( UNDERLINE_SINGLE );
        else
            rFont.SetUnderline( UNDERLINE_NONE );
        if ( rLogFont.lfStrikeOut )
            rFont.SetStrikeout( STRIKEOUT_SINGLE );
        else
            rFont.SetStrikeout( STRIKEOUT_NONE );
    }
}

// -----------------------------------------------------------------------

void ImplSalLogFontToFontW( HDC hDC, const LOGFONTW& rLogFont, Font& rFont )
{
    XubString aFontName( rLogFont.lfFaceName );
    if ( aFontName.Len() )
    {
        rFont.SetName( aFontName );
        rFont.SetCharSet( ImplCharSetToSal( rLogFont.lfCharSet ) );
        rFont.SetFamily( ImplFamilyToSal( rLogFont.lfPitchAndFamily ) );
        rFont.SetPitch( ImplLogPitchToSal( rLogFont.lfPitchAndFamily ) );
        rFont.SetWeight( ImplWeightToSal( rLogFont.lfWeight ) );

        long nFontHeight = rLogFont.lfHeight;
        if ( nFontHeight < 0 )
            nFontHeight = -nFontHeight;
        long nDPIY = GetDeviceCaps( hDC, LOGPIXELSY );
        if( !nDPIY )
            nDPIY = 600;
        nFontHeight *= 72;
        nFontHeight += nDPIY/2;
        nFontHeight /= nDPIY;
        rFont.SetSize( Size( 0, nFontHeight ) );
        rFont.SetOrientation( (short)rLogFont.lfEscapement );
        if ( rLogFont.lfItalic )
            rFont.SetItalic( ITALIC_NORMAL );
        else
            rFont.SetItalic( ITALIC_NONE );
        if ( rLogFont.lfUnderline )
            rFont.SetUnderline( UNDERLINE_SINGLE );
        else
            rFont.SetUnderline( UNDERLINE_NONE );
        if ( rLogFont.lfStrikeOut )
            rFont.SetStrikeout( STRIKEOUT_SINGLE );
        else
            rFont.SetStrikeout( STRIKEOUT_NONE );
    }
}

// =======================================================================

ImplWinFontData::ImplWinFontData( const ImplDevFontAttributes& rDFS,
    int nHeight, WIN_BYTE eWinCharSet, WIN_BYTE nPitchAndFamily )
:   ImplFontData( rDFS, 0 ),
    meWinCharSet( eWinCharSet ),
    mnPitchAndFamily( nPitchAndFamily ),
    mpFontCharSets( NULL ),
    mpUnicodeMap( NULL ),
    mbGsubRead( false ),
    mbDisableGlyphApi( false ),
    mbHasKoreanRange( false ),
    mbHasCJKSupport( false ),
    mbAliasSymbolsLow( false ),
    mbAliasSymbolsHigh( false )
{
    SetBitmapSize( 0, nHeight );

    if( eWinCharSet == SYMBOL_CHARSET )
    {
        if( (nPitchAndFamily & TMPF_TRUETYPE) != 0 )
        {
            // truetype fonts need their symbols as U+F0xx
            mbAliasSymbolsHigh = true;
        }
        else if( (nPitchAndFamily & (TMPF_VECTOR|TMPF_DEVICE))
                                 == (TMPF_VECTOR|TMPF_DEVICE) )
        {
            // scalable device fonts (e.g. builtin printer fonts)
            // need their symbols as U+00xx
            mbAliasSymbolsLow  = true;
        }
        else if( (nPitchAndFamily & (TMPF_VECTOR|TMPF_TRUETYPE)) == 0 )
        {
            // bitmap fonts need their symbols as U+F0xx
            mbAliasSymbolsHigh = true;
        }
    }
}

// -----------------------------------------------------------------------

ImplWinFontData::~ImplWinFontData()
{
    delete[] mpFontCharSets;

    if( mpUnicodeMap )
        mpUnicodeMap->DeReference();
}

// -----------------------------------------------------------------------

void ImplWinFontData::UpdateFromHDC( HDC hDC )
{
    // short circuit if already initialized
    if( mpUnicodeMap != NULL )
        return;

    ReadCmapTable( hDC );
    ReadOs2Table( hDC );

    // even if the font works some fonts have problems with the glyph API
    // => the heuristic below tries to figure out which fonts have the problem
    TEXTMETRICA aTextMetric;
    if( GetTextMetricsA( hDC, &aTextMetric ) )
        if( !(aTextMetric.tmPitchAndFamily & TMPF_TRUETYPE)
        ||   (aTextMetric.tmPitchAndFamily & TMPF_DEVICE) )
            mbDisableGlyphApi = true;

#if 0
    // #110548# more important than #107885# => TODO: better solution
    DWORD nFLI = GetFontLanguageInfo( hDC );
    if( 0 == (nFLI & GCP_GLYPHSHAPE) )
        mbDisableGlyphApi = true;
#endif
}

// -----------------------------------------------------------------------

bool ImplWinFontData::HasGSUBstitutions( HDC hDC ) const
{
    if( !mbGsubRead )
        ReadGsubTable( hDC );
    return !maGsubTable.empty();
}

// -----------------------------------------------------------------------

bool ImplWinFontData::IsGSUBstituted( sal_Unicode cChar ) const
{
    return( maGsubTable.find( cChar ) != maGsubTable.end() );
}

// -----------------------------------------------------------------------

ImplFontCharMap* ImplWinFontData::GetImplFontCharMap()
{
    mpUnicodeMap->AddReference();
    return mpUnicodeMap;
}

// -----------------------------------------------------------------------

static unsigned GetUInt( const unsigned char* p ) { return((p[0]<<24)+(p[1]<<16)+(p[2]<<8)+p[3]);}
static unsigned GetUShort( const unsigned char* p ){ return((p[0]<<8)+p[1]);}
static signed GetSShort( const unsigned char* p ){ return((short)((p[0]<<8)+p[1]));}
static inline DWORD CalcTag( const char p[4]) { return (p[0]+(p[1]<<8)+(p[2]<<16)+(p[3]<<24)); }

void ImplWinFontData::ReadOs2Table( HDC hDC )
{
    const DWORD Os2Tag = CalcTag( "OS/2" );
    DWORD nLength = ::GetFontData( hDC, Os2Tag, 0, NULL, 0 );
    if( (nLength == GDI_ERROR) || !nLength )
        return;
    std::vector<unsigned char> aOS2map( nLength );
    unsigned char* pOS2map = &aOS2map[0];
    DWORD nRC = ::GetFontData( hDC, Os2Tag, 0, pOS2map, nLength );
    sal_uInt32 nVersion = GetUShort( pOS2map );
    if ( nVersion >= 0x0001 && nLength >= 58 )
    {
        // We need at least version 0x0001 (TrueType rev 1.66)
        // to have access to the needed struct members.
        sal_uInt32 ulUnicodeRange1 = GetUInt( pOS2map + 42 );
        sal_uInt32 ulUnicodeRange2 = GetUInt( pOS2map + 46 );
        sal_uInt32 ulUnicodeRange3 = GetUInt( pOS2map + 50 );
        sal_uInt32 ulUnicodeRange4 = GetUInt( pOS2map + 54 );

        // Check for CJK capabilities of the current font
        mbHasCJKSupport = (ulUnicodeRange2 & 0x2fff0000)
                        | (ulUnicodeRange3 & 0x00000001);
        mbHasKoreanRange= (ulUnicodeRange1 & 0x10000000)
                        | (ulUnicodeRange2 & 0x01100000);
    }
}

// -----------------------------------------------------------------------

void ImplWinFontData::ReadGsubTable( HDC hDC ) const
{
    mbGsubRead = true;

    // check the existence of a GSUB table
    const DWORD GsubTag = CalcTag( "GSUB" );
    DWORD nRC = ::GetFontData( hDC, GsubTag, 0, NULL, 0 );
    if( (nRC == GDI_ERROR) || !nRC )
        return;

    // TODO: directly read the GSUB table instead of going through sft

    // get raw font file data
    DWORD nFontSize = ::GetFontData( hDC, 0, 0, NULL, 0 );
    if( nFontSize == GDI_ERROR )
        return;
    std::vector<char> aRawFont( nFontSize+1 );
    aRawFont[ nFontSize ] = 0;
    DWORD nFontSize2 = ::GetFontData( hDC, 0, 0, (void*)&aRawFont[0], nFontSize );
    if( nFontSize != nFontSize2 )
        return;

    // open font file
    sal_uInt32 nFaceNum = 0;
    if( !aRawFont[0] )  // TTC candidate
        nFaceNum = ~0;  // indicate "TTC font extracts only"

    TrueTypeFont* pTTFont = NULL;
    ::OpenTTFont( &aRawFont[0], nFontSize, nFaceNum, &pTTFont );
    if( !pTTFont )
        return;

    // add vertically substituted characters to list
    static const sal_Unicode aGSUBCandidates[] = {
        0x0020, 0x0080, // ASCII
        0x2000, 0x2600, // misc
        0x3000, 0x3100, // CJK punctutation
        0x3300, 0x3400, // squared words
        0xFF00, 0xFFF0, // halfwidth|fullwidth forms
    0 };

    for( const sal_Unicode* pPair = aGSUBCandidates; *pPair; pPair += 2 )
        for( sal_Unicode cChar = pPair[0]; cChar < pPair[1]; ++cChar )
            if( ::MapChar( pTTFont, cChar, 0 ) != ::MapChar( pTTFont, cChar, 1 ) )
                maGsubTable.insert( cChar ); // insert GSUBbed unicodes

    CloseTTFont( pTTFont );
}

// -----------------------------------------------------------------------

void ImplWinFontData::ReadCmapTable( HDC hDC )
{
    CmapResult aResult;
    aResult.mnPairCount = 0;
    aResult.mbSymbolic  = (meWinCharSet == SYMBOL_CHARSET);
    aResult.mbRecoded   = true;

    // get the CMAP table from the font which is selected into the DC
    const DWORD CmapTag = CalcTag( "cmap" );
    DWORD nRC = ::GetFontData( hDC, CmapTag, 0, NULL, 0 );
    // read the CMAP table if available
    if( nRC != GDI_ERROR )
    {
        const int nLength = nRC;
        std::vector<unsigned char> aCmap( nLength );
        unsigned char* pCmap = &aCmap[0];
        nRC = ::GetFontData( hDC, CmapTag, 0, pCmap, nLength );
        // parse the CMAP table
        if( nRC == nLength )
            ParseCMAP( pCmap, nLength, aResult );
    }

    mbDisableGlyphApi |= aResult.mbRecoded;

    if( aResult.mnPairCount > 0 )
        mpUnicodeMap = new ImplFontCharMap( aResult.mnPairCount,
            aResult.mpPairCodes, aResult.mpStartGlyphs );
    else
        mpUnicodeMap = ImplFontCharMap::GetDefaultMap();
}

// =======================================================================

void WinSalGraphics::SetTextColor( SalColor nSalColor )
{
    COLORREF aCol = PALETTERGB( SALCOLOR_RED( nSalColor ),
                                SALCOLOR_GREEN( nSalColor ),
                                SALCOLOR_BLUE( nSalColor ) );

    if( !mbPrinter &&
        GetSalData()->mhDitherPal &&
        ImplIsSysColorEntry( nSalColor ) )
    {
        aCol = PALRGB_TO_RGB( aCol );
    }

    ::SetTextColor( mhDC, aCol );
}

// -----------------------------------------------------------------------

int CALLBACK SalEnumQueryFontProcExW( const ENUMLOGFONTEXW*,
                                      const NEWTEXTMETRICEXW*,
                                      DWORD, LPARAM lParam )
{
    *((bool*)(void*)lParam) = true;
    return 0;
}

// -----------------------------------------------------------------------

int CALLBACK SalEnumQueryFontProcExA( const ENUMLOGFONTEXA*,
                                      const NEWTEXTMETRICEXA*,
                                      DWORD, LPARAM lParam )
{
    *((bool*)(void*)lParam) = true;
    return 0;
}

// -----------------------------------------------------------------------

bool ImplIsFontAvailable( HDC hDC, const UniString& rName )
{
    bool bAvailable = false;

    if ( aSalShlData.mbWNT )
    {
        // Test, if Font available
        LOGFONTW aLogFont;
        memset( &aLogFont, 0, sizeof( aLogFont ) );
        aLogFont.lfCharSet = DEFAULT_CHARSET;

        UINT nNameLen = rName.Len();
        if ( nNameLen > (sizeof( aLogFont.lfFaceName )/sizeof( wchar_t ))-1 )
            nNameLen = (sizeof( aLogFont.lfFaceName )/sizeof( wchar_t ))-1;
        memcpy( aLogFont.lfFaceName, rName.GetBuffer(), nNameLen*sizeof( wchar_t ) );
        aLogFont.lfFaceName[nNameLen] = 0;

        EnumFontFamiliesExW( hDC, &aLogFont, (FONTENUMPROCW)SalEnumQueryFontProcExW,
                             (LPARAM)(void*)&bAvailable, 0 );
    }
    else
    {
        ByteString aTemp = ImplSalGetWinAnsiString( rName );

        // Test, if Font available
        LOGFONTA aLogFont;
        memset( &aLogFont, 0, sizeof( aLogFont ) );
        aLogFont.lfCharSet = DEFAULT_CHARSET;

        UINT nNameLen = aTemp.Len();
        if ( nNameLen > sizeof( aLogFont.lfFaceName )-1 )
            nNameLen = sizeof( aLogFont.lfFaceName )-1;
        memcpy( aLogFont.lfFaceName, aTemp.GetBuffer(), nNameLen );
        aLogFont.lfFaceName[nNameLen] = 0;

        EnumFontFamiliesExA( hDC, &aLogFont, (FONTENUMPROCA)SalEnumQueryFontProcExA,
                             (LPARAM)(void*)&bAvailable, 0 );
    }

    return bAvailable;
}

// -----------------------------------------------------------------------

void ImplGetLogFontFromFontSelect( HDC hDC,
                                   const ImplFontSelectData* pFont,
                                   LOGFONTW& rLogFont,
                                   bool bTestVerticalAvail )
{
    UniString   aName;
    if ( pFont->mpFontData )
        aName = pFont->mpFontData->maName;
    else
        aName = pFont->maName.GetToken( 0 );

    UINT nNameLen = aName.Len();
    if ( nNameLen > (sizeof( rLogFont.lfFaceName )/sizeof( wchar_t ))-1 )
        nNameLen = (sizeof( rLogFont.lfFaceName )/sizeof( wchar_t ))-1;
    memcpy( rLogFont.lfFaceName, aName.GetBuffer(), nNameLen*sizeof( wchar_t ) );
    rLogFont.lfFaceName[nNameLen] = 0;

    if( !pFont->mpFontData )
    {
        rLogFont.lfCharSet = pFont->IsSymbolFont() ? SYMBOL_CHARSET : DEFAULT_CHARSET;
        rLogFont.lfPitchAndFamily = ImplPitchToWin( pFont->mePitch )
                                  | ImplFamilyToWin( pFont->meFamily );
    }
    else
    {
        ImplWinFontData* pWinFontData = reinterpret_cast<ImplWinFontData*>( pFont->mpFontData );
        rLogFont.lfCharSet        = pWinFontData->GetCharSet();
        rLogFont.lfPitchAndFamily = pWinFontData->GetPitchAndFamily();
    }

    rLogFont.lfWeight          = ImplWeightToWin( pFont->meWeight );
    rLogFont.lfHeight          = (int)-pFont->mnHeight;
    rLogFont.lfWidth           = (int)pFont->mnWidth;
    rLogFont.lfUnderline       = 0;
    rLogFont.lfStrikeOut       = 0;
    rLogFont.lfItalic          = (pFont->meItalic) != ITALIC_NONE;
    rLogFont.lfEscapement      = pFont->mnOrientation;
    rLogFont.lfOrientation     = rLogFont.lfEscapement;
    rLogFont.lfClipPrecision   = CLIP_DEFAULT_PRECIS;
    rLogFont.lfQuality         = DEFAULT_QUALITY;
    rLogFont.lfOutPrecision    = OUT_TT_PRECIS;
    if ( pFont->mnOrientation )
        rLogFont.lfClipPrecision |= CLIP_LH_ANGLES;

    // disable antialiasing if requested
    if ( pFont->mbNonAntialiased )
        rLogFont.lfQuality = NONANTIALIASED_QUALITY;

    // select vertical mode if requested and available
    if( pFont->mbVertical && nNameLen )
    {
        // vertical fonts start with an '@'
        memmove( &rLogFont.lfFaceName[1], &rLogFont.lfFaceName[0],
            sizeof(rLogFont.lfFaceName)-sizeof(rLogFont.lfFaceName[0]) );
        rLogFont.lfFaceName[0] = '@';

        // check availability of vertical mode for this font
        bool bAvailable = false;
        EnumFontFamiliesExW( hDC, &rLogFont, (FONTENUMPROCW)SalEnumQueryFontProcExW,
                         (LPARAM)&bAvailable, 0 );

        if( !bAvailable )
        {
            // restore non-vertical name if not vertical mode isn't available
            memcpy( &rLogFont.lfFaceName[0], aName.GetBuffer(), nNameLen*sizeof(wchar_t) );
            if( nNameLen < LF_FACESIZE )
                rLogFont.lfFaceName[nNameLen] = '\0';
        }
    }
}

// -----------------------------------------------------------------------

static void ImplGetLogFontFromFontSelect( HDC hDC,
                                   const ImplFontSelectData* pFont,
                                   LOGFONTA& rLogFont,
                                   bool bTestVerticalAvail )
{
    ByteString aName;
    if( pFont->mpFontData )
        aName = ImplSalGetWinAnsiString( pFont->mpFontData->maName );
    else
        aName = ImplSalGetWinAnsiString( pFont->maName.GetToken( 0 ) );

    int nNameLen = aName.Len();
    if( nNameLen > LF_FACESIZE )
        nNameLen = LF_FACESIZE;
    memcpy( rLogFont.lfFaceName, aName.GetBuffer(), nNameLen );
    if( nNameLen < LF_FACESIZE )
        rLogFont.lfFaceName[nNameLen] = '\0';

    if( !pFont->mpFontData )
    {
        rLogFont.lfCharSet = pFont->IsSymbolFont() ? SYMBOL_CHARSET : DEFAULT_CHARSET;
        rLogFont.lfPitchAndFamily = ImplPitchToWin( pFont->mePitch )
                                  | ImplFamilyToWin( pFont->meFamily );
    }
    else
    {
        ImplWinFontData* pWinFontData = reinterpret_cast<ImplWinFontData*>( pFont->mpFontData );
        rLogFont.lfCharSet        = pWinFontData->GetCharSet();
        rLogFont.lfPitchAndFamily = pWinFontData->GetPitchAndFamily();
    }

    rLogFont.lfWeight           = ImplWeightToWin( pFont->meWeight );
    rLogFont.lfHeight           = (int)-pFont->mnHeight;
    rLogFont.lfWidth            = (int)pFont->mnWidth;
    rLogFont.lfUnderline        = 0;
    rLogFont.lfStrikeOut        = 0;
    rLogFont.lfItalic           = (pFont->meItalic) != ITALIC_NONE;
    rLogFont.lfEscapement       = pFont->mnOrientation;
    rLogFont.lfOrientation      = rLogFont.lfEscapement; // ignored by W98
    rLogFont.lfClipPrecision    = CLIP_DEFAULT_PRECIS;
    rLogFont.lfQuality          = DEFAULT_QUALITY;
    rLogFont.lfOutPrecision     = OUT_TT_PRECIS;
    if( pFont->mnOrientation )
        rLogFont.lfClipPrecision |= CLIP_LH_ANGLES;

    // disable antialiasing if requested
    if( pFont->mbNonAntialiased )
        rLogFont.lfQuality = NONANTIALIASED_QUALITY;

    // select vertical mode if requested and available
    if( pFont->mbVertical && nNameLen )
    {
        // vertical fonts start with an '@'
        memmove( &rLogFont.lfFaceName[1], &rLogFont.lfFaceName[0],
                    sizeof(rLogFont.lfFaceName)-sizeof(rLogFont.lfFaceName[0]) );
        rLogFont.lfFaceName[0] = '@';

        // check availability of vertical mode for this font
        bool bAvailable = false;
        EnumFontFamiliesExA( hDC, &rLogFont, (FONTENUMPROCA)SalEnumQueryFontProcExA,
                         (LPARAM)&bAvailable, 0 );

        if( !bAvailable )
        {
            // restore non-vertical name if vertical mode is not supported
            memcpy( rLogFont.lfFaceName, aName.GetBuffer(), nNameLen );
            if( nNameLen < LF_FACESIZE )
                rLogFont.lfFaceName[nNameLen] = '\0';
        }
    }
}

// -----------------------------------------------------------------------

USHORT WinSalGraphics::SetFont( ImplFontSelectData* pFont, int nFallbackLevel )
{
    // return early if there is no new font
    if( !pFont )
    {
    for( int i = nFallbackLevel; i < MAX_FALLBACK; ++i )
    {
            if( mhFonts[ i ] )
                DeleteFont( mhFonts[ i ] );
            mhFonts[ i ] = 0;
    }
        return 0;
    }

    DBG_ASSERT( pFont->mpFontData, "WinSalGraphics mpFontData==NULL");
    mpWinFontEntry[ nFallbackLevel ] = reinterpret_cast<ImplWinFontEntry*>( pFont->mpFontEntry );
    mpWinFontData[ nFallbackLevel ] = reinterpret_cast<ImplWinFontData*>( pFont->mpFontData );

    HFONT hNewFont = 0;
    HFONT hOldFont;

    HDC hdcScreen = 0;
    if( mbVirDev )
        // only required for virtual devices, see below for details
        hdcScreen = GetDC(0);

    if( aSalShlData.mbWNT )
    {
        LOGFONTW aLogFont;
        ImplGetLogFontFromFontSelect( mhDC, pFont, aLogFont, true );

        // on the display we prefer Courier New when Courier is a
        // bitmap only font and we need to stretch or rotate it
        if( mbScreen
        &&  (pFont->mnWidth != 0
          || pFont->mnOrientation != 0
          || pFont->mpFontData == NULL
          || (pFont->mpFontData->GetHeight() != pFont->mnHeight))
        && !bImplSalCourierScalable
        && bImplSalCourierNew
        && (ImplSalWICompareAscii( aLogFont.lfFaceName, "Courier" ) == 0) )
            lstrcpynW( aLogFont.lfFaceName, L"Courier New", 11 );

        // limit font requests to MAXFONTHEIGHT
        // TODO: share MAXFONTHEIGHT font instance
        if( -aLogFont.lfHeight <= MAXFONTHEIGHT )
            mfFontScale = 1.0;
        else
        {
            mfFontScale = -aLogFont.lfHeight / (float)MAXFONTHEIGHT;
            aLogFont.lfHeight = -MAXFONTHEIGHT;
            aLogFont.lfWidth = static_cast<int>( aLogFont.lfWidth / mfFontScale );
        }

        hNewFont = CreateFontIndirectW( &aLogFont );
        if( hdcScreen )
        {
            // select font into screen hdc first to get an antialiased font
            // see knowledge base article 305290:
            // "PRB: Fonts Not Drawn Antialiased on Device Context for DirectDraw Surface"
            SelectFont( hdcScreen, SelectFont( hdcScreen , hNewFont ) );
        }
        hOldFont = SelectFont( mhDC, hNewFont );

        TEXTMETRICW aTextMetricW;
        if( !GetTextMetricsW( mhDC, &aTextMetricW ) )
        {
            // the selected font doesn't work => try a replacement
            // TODO: use its font fallback instead
            lstrcpynW( aLogFont.lfFaceName, L"Courier New", 11 );
            aLogFont.lfPitchAndFamily = FIXED_PITCH;
            HFONT hNewFont2 = CreateFontIndirectW( &aLogFont );
            SelectFont( mhDC, hNewFont2 );
            DeleteFont( hNewFont );
            hNewFont = hNewFont2;
        }
    }
    else
    {
        if( !mpLogFont )
             // mpLogFont is needed for getting the kerning pairs
             // TODO: get them from somewhere else
            mpLogFont = new LOGFONTA;
        LOGFONTA& aLogFont = *mpLogFont;
        ImplGetLogFontFromFontSelect( mhDC, pFont, aLogFont, true );

        // on the display we prefer Courier New when Courier is a
        // bitmap only font and we need to stretch or rotate it
        if( mbScreen
        &&  (pFont->mnWidth != 0
          || pFont->mnOrientation != 0
          || pFont->mpFontData == NULL
          || (pFont->mpFontData->GetHeight() != pFont->mnHeight))
        && !bImplSalCourierScalable
        && bImplSalCourierNew
        && (stricmp( aLogFont.lfFaceName, "Courier" ) == 0) )
            strncpy( aLogFont.lfFaceName, "Courier New", 11 );

        // limit font requests to MAXFONTHEIGHT
        // TODO: share MAXFONTHEIGHT font instance
        if( -aLogFont.lfHeight <= MAXFONTHEIGHT )
            mfFontScale = 1.0;
        else
        {
            mfFontScale = -aLogFont.lfHeight / (float)MAXFONTHEIGHT;
            aLogFont.lfHeight = -MAXFONTHEIGHT;
            aLogFont.lfWidth = static_cast<int>( aLogFont.lfWidth / mfFontScale );
        }

        hNewFont = CreateFontIndirectA( &aLogFont );
        if( hdcScreen )
        {
            // select font into screen hdc first to get an antialiased font
            // see knowledge base article 305290:
            // "PRB: Fonts Not Drawn Antialiased on Device Context for DirectDraw Surface"
            SelectFont( hdcScreen, SelectFont( hdcScreen , hNewFont ) );
        }
        hOldFont = SelectFont( mhDC, hNewFont );

        TEXTMETRICA aTextMetricA;
        // when the font doesn't work try a replacement
        if ( !GetTextMetricsA( mhDC, &aTextMetricA ) )
        {
            // the selected font doesn't work => try a replacement
            // TODO: use its font fallback instead
            LOGFONTA aTempLogFont = aLogFont;
            strncpy( aTempLogFont.lfFaceName, "Courier New", 11 );
            aTempLogFont.lfPitchAndFamily = FIXED_PITCH;
            HFONT hNewFont2 = CreateFontIndirectA( &aTempLogFont );
            SelectFont( mhDC, hNewFont2 );
            DeleteFont( hNewFont );
            hNewFont = hNewFont2;
        }
    }

    if( hdcScreen )
        ReleaseDC( NULL, hdcScreen );

    if( !mhDefFont )
    {
        // keep default font
        mhDefFont = hOldFont;
    }
    else
    {
        // dereference unused fonts
        for( int i = nFallbackLevel; i < MAX_FALLBACK; ++i )
        {
            if( mhFonts[i] )
            {
                DeleteFont( mhFonts[i] );
                mhFonts[i] = 0;
            }
        }
    }

    // store new font in correct layer
    mhFonts[ nFallbackLevel ] = hNewFont;
    // now the font is live => update font face
    if( mpWinFontData[ nFallbackLevel ] )
        mpWinFontData[ nFallbackLevel ]->UpdateFromHDC( mhDC );

    if( !nFallbackLevel )
    {
        mbFontKernInit = TRUE;
        if ( mpFontKernPairs )
        {
            delete[] mpFontKernPairs;
            mpFontKernPairs = NULL;
        }
        mnFontKernPairCount = 0;
    }

    mnFontCharSetCount = 0;

    // some printers have higher internal resolution, so their
    // text output would be different from what we calculated
    // => suggest DrawTextArray to workaround this problem
    if ( mbPrinter )
        return SAL_SETFONT_USEDRAWTEXTARRAY;
    else
        return 0;
}

// -----------------------------------------------------------------------

void WinSalGraphics::GetFontMetric( ImplFontMetricData* pMetric )
{
    if ( aSalShlData.mbWNT )
    {
        wchar_t aFaceName[LF_FACESIZE+60];
        if( ::GetTextFaceW( mhDC, sizeof(aFaceName)/sizeof(wchar_t), aFaceName ) )
            pMetric->maName = aFaceName;
    }
    else
    {
        char aFaceName[LF_FACESIZE+60];
        if( ::GetTextFaceA( mhDC, sizeof(aFaceName), aFaceName ) )
            pMetric->maName = ImplSalGetUniString( aFaceName );
    }

    TEXTMETRICA aWinMetric;
    if( !GetTextMetricsA( mhDC, &aWinMetric ) )
        return;

    // device independent font attributes
    pMetric->meFamily       = ImplFamilyToSal( aWinMetric.tmPitchAndFamily );;
    pMetric->mbSymbolFlag   = (aWinMetric.tmCharSet == SYMBOL_CHARSET);
    pMetric->meWeight       = ImplWeightToSal( aWinMetric.tmWeight );
    pMetric->mePitch        = ImplMetricPitchToSal( aWinMetric.tmPitchAndFamily );
    pMetric->meItalic       = aWinMetric.tmItalic ? ITALIC_NORMAL : ITALIC_NONE;
    pMetric->mnSlant        = 0;

    // device dependend font attributes
    pMetric->mbDevice       = (aWinMetric.tmPitchAndFamily & TMPF_DEVICE) != 0;
    pMetric->mbScalableFont = (aWinMetric.tmPitchAndFamily & (TMPF_VECTOR|TMPF_TRUETYPE)) != 0;
    if( pMetric->mbScalableFont )
    {
        // check if there are kern pairs
        // TODO: does this work with GPOS kerning?
        DWORD nKernPairs = ::GetKerningPairsA( mhDC, 0, NULL );
        pMetric->mbKernableFont = (nKernPairs > 0);
    }
    else
    {
        // bitmap fonts cannot be rotated directly
        pMetric->mnOrientation  = 0;
        // bitmap fonts have no kerning
        pMetric->mbKernableFont = false;
    }

    // transformation dependend font metrics
    pMetric->mnWidth        = static_cast<int>( mfFontScale * aWinMetric.tmAveCharWidth );
    pMetric->mnIntLeading   = static_cast<int>( mfFontScale * aWinMetric.tmInternalLeading );
    pMetric->mnExtLeading   = static_cast<int>( mfFontScale * aWinMetric.tmExternalLeading );
    pMetric->mnAscent       = static_cast<int>( mfFontScale * aWinMetric.tmAscent );
    pMetric->mnDescent      = static_cast<int>( mfFontScale * aWinMetric.tmDescent );

    // #107888# improved metric compatibility for Asian fonts...
    // TODO: assess workaround below for CWS >= extleading
    // TODO: evaluate use of aWinMetric.sTypo* members for CJK
    if( mpWinFontData[0] && mpWinFontData[0]->SupportsCJK() )
    {
        pMetric->mnIntLeading += pMetric->mnExtLeading;

        // #109280# The line height for Asian fonts is too small.
        // Therefore we add half of the external leading to the
        // ascent, the other half is added to the descent.
        const long nHalfTmpExtLeading = pMetric->mnExtLeading / 2;
        const long nOtherHalfTmpExtLeading = pMetric->mnExtLeading - nHalfTmpExtLeading;

        // #110641# external leading for Asian fonts.
        // The factor 0.3 has been confirmed with experiments.
        long nCJKExtLeading = static_cast<long>(0.30 * (pMetric->mnAscent + pMetric->mnDescent));
        nCJKExtLeading -= pMetric->mnExtLeading;
        pMetric->mnExtLeading = (nCJKExtLeading > 0) ? nCJKExtLeading : 0;

        pMetric->mnAscent   += nHalfTmpExtLeading;
        pMetric->mnDescent  += nOtherHalfTmpExtLeading;

        // #109280# HACK korean only: increase descent for wavelines and impr
        if( !aSalShlData.mbWNT )
            if( mpWinFontData[0]->SupportsKorean() )
                pMetric->mnDescent += pMetric->mnExtLeading;
    }
}

// -----------------------------------------------------------------------

int CALLBACK SalEnumCharSetsProcExA( const ENUMLOGFONTEXA* pLogFont,
                                     const NEWTEXTMETRICEXA* pMetric,
                                     DWORD nFontType, LPARAM lParam )
{
    WinSalGraphics* pData = (WinSalGraphics*)lParam;
    // Charset already in the list?
    for ( BYTE i = 0; i < pData->mnFontCharSetCount; i++ )
    {
        if ( pData->mpFontCharSets[i] == pLogFont->elfLogFont.lfCharSet )
            return 1;
    }
    pData->mpFontCharSets[pData->mnFontCharSetCount] = pLogFont->elfLogFont.lfCharSet;
    pData->mnFontCharSetCount++;
    return 1;
}

// -----------------------------------------------------------------------

static void ImplGetAllFontCharSets( WinSalGraphics* pData )
{
    if ( !pData->mpFontCharSets )
        pData->mpFontCharSets = new BYTE[256];

    LOGFONTA aLogFont;
    memset( &aLogFont, 0, sizeof( aLogFont ) );
    aLogFont.lfCharSet = DEFAULT_CHARSET;
    GetTextFaceA( pData->mhDC, sizeof( aLogFont.lfFaceName ), aLogFont.lfFaceName );
    EnumFontFamiliesExA( pData->mhDC, &aLogFont, (FONTENUMPROCA)SalEnumCharSetsProcExA,
                         (LPARAM)(void*)pData, 0 );
}

// -----------------------------------------------------------------------

static void ImplAddKerningPairs( WinSalGraphics* pData )
{
    ULONG nPairs = ::GetKerningPairsA( pData->mhDC, 0, NULL );
    if ( !nPairs )
        return;

    CHARSETINFO aInfo;
    if ( !TranslateCharsetInfo( (DWORD*)(ULONG)GetTextCharset( pData->mhDC ), &aInfo, TCI_SRCCHARSET ) )
        return;

    if ( !pData->mpFontKernPairs )
        pData->mpFontKernPairs = new KERNINGPAIR[nPairs];
    else
    {
        KERNINGPAIR* pOldPairs = pData->mpFontKernPairs;
        pData->mpFontKernPairs = new KERNINGPAIR[nPairs+pData->mnFontKernPairCount];
        memcpy( pData->mpFontKernPairs, pOldPairs,
                pData->mnFontKernPairCount*sizeof( KERNINGPAIR ) );
        delete[] pOldPairs;
    }

    UINT            nCP = aInfo.ciACP;
    ULONG           nOldPairs = pData->mnFontKernPairCount;
    KERNINGPAIR*    pTempPair = pData->mpFontKernPairs+pData->mnFontKernPairCount;
    nPairs = ::GetKerningPairsA( pData->mhDC, nPairs, pTempPair );
    for ( ULONG i = 0; i < nPairs; i++ )
    {
        unsigned char   aBuf[2];
        wchar_t         nChar;
        int             nLen;
        BOOL            bAdd = TRUE;

        // None-ASCII?, then we must convert the char
        if ( (pTempPair->wFirst > 125) || (pTempPair->wFirst == 92) )
        {
            if ( pTempPair->wFirst < 256 )
            {
                aBuf[0] = (unsigned char)pTempPair->wFirst;
                nLen = 1;
            }
            else
            {
                aBuf[0] = (unsigned char)(pTempPair->wFirst >> 8);
                aBuf[1] = (unsigned char)(pTempPair->wFirst & 0xFF);
                nLen = 2;
            }
            if ( MultiByteToWideChar( nCP, MB_PRECOMPOSED | MB_USEGLYPHCHARS,
                                      (const char*)aBuf, nLen, &nChar, 1 ) )
                pTempPair->wFirst = nChar;
            else
                bAdd = FALSE;
        }
        if ( (pTempPair->wSecond > 125) || (pTempPair->wSecond == 92) )
        {
            if ( pTempPair->wSecond < 256 )
            {
                aBuf[0] = (unsigned char)pTempPair->wSecond;
                nLen = 1;
            }
            else
            {
                aBuf[0] = (unsigned char)(pTempPair->wSecond >> 8);
                aBuf[1] = (unsigned char)(pTempPair->wSecond & 0xFF);
                nLen = 2;
            }
            if ( MultiByteToWideChar( nCP, MB_PRECOMPOSED | MB_USEGLYPHCHARS,
                                      (const char*)aBuf, nLen, &nChar, 1 ) )
                pTempPair->wSecond = nChar;
            else
                bAdd = FALSE;
        }

        // TODO: get rid of linear search!
        KERNINGPAIR* pTempPair2 = pData->mpFontKernPairs;
        for ( ULONG j = 0; j < nOldPairs; j++ )
        {
            if ( (pTempPair2->wFirst == pTempPair->wFirst) &&
                 (pTempPair2->wSecond == pTempPair->wSecond) )
            {
                bAdd = FALSE;
                break;
            }
            pTempPair2++;
        }

        if ( bAdd )
        {
            KERNINGPAIR* pDestPair = pData->mpFontKernPairs+pData->mnFontKernPairCount;
            if ( pDestPair != pTempPair )
                memcpy( pDestPair, pTempPair, sizeof( KERNINGPAIR ) );
            pData->mnFontKernPairCount++;
        }

        pTempPair++;
    }
}

// -----------------------------------------------------------------------

ULONG WinSalGraphics::GetKernPairs( ULONG nPairs, ImplKernPairData* pKernPairs )
{
    DBG_ASSERT( sizeof( KERNINGPAIR ) == sizeof( ImplKernPairData ),
                "WinSalGraphics::GetKernPairs(): KERNINGPAIR != ImplKernPairData" );

    if ( mbFontKernInit )
    {
        if( mpFontKernPairs )
        {
            delete[] mpFontKernPairs;
            mpFontKernPairs = NULL;
        }
        mnFontKernPairCount = 0;

        if ( aSalShlData.mbWNT )
        {
            KERNINGPAIR* pPairs = NULL;
            int nCount = ::GetKerningPairsW( mhDC, 0, NULL );
            if( nCount )
            {
#ifdef GCP_KERN_HACK
                pPairs = new KERNINGPAIR[ nCount+1 ];
                mpFontKernPairs = pPairs;
                mnFontKernPairCount = nCount;
                ::GetKerningPairsW( mhDC, nCount, pPairs );
#else // GCP_KERN_HACK
                pPairs = pKernPairs;
                nCount = (nCount < nPairs) : nCount : nPairs;
                ::GetKerningPairsW( mhDC, nCount, pPairs );
                return nCount;
#endif // GCP_KERN_HACK
            }
        }
        else
        {
            if ( !mnFontCharSetCount )
                ImplGetAllFontCharSets( this );

            if ( mnFontCharSetCount <= 1 )
                ImplAddKerningPairs( this );
            else
            {
                // Query All Kerning Pairs from all possible CharSets
                for ( BYTE i = 0; i < mnFontCharSetCount; i++ )
                {
                    mpLogFont->lfCharSet = mpFontCharSets[i];
                    HFONT hNewFont = CreateFontIndirectA( mpLogFont );
                    HFONT hOldFont = SelectFont( mhDC, hNewFont );
                    ImplAddKerningPairs( this );
                    SelectFont( mhDC, hOldFont );
                    DeleteFont( hNewFont );
                }
            }
        }

        mbFontKernInit = FALSE;

        std::sort( mpFontKernPairs, mpFontKernPairs + mnFontKernPairCount, ImplCmpKernData );
    }

    if( !pKernPairs )
        return mnFontKernPairCount;
    else if( mpFontKernPairs )
    {
        if ( nPairs < mnFontKernPairCount )
            nPairs = mnFontKernPairCount;
        memcpy( pKernPairs, mpFontKernPairs,
                nPairs*sizeof( ImplKernPairData ) );
        return nPairs;
    }

    return 0;
}

// -----------------------------------------------------------------------

ImplFontCharMap* WinSalGraphics::GetImplFontCharMap() const
{
    if( !mpWinFontData[0] )
        return ImplFontCharMap::GetDefaultMap();
    return mpWinFontData[0]->GetImplFontCharMap();
}

// -----------------------------------------------------------------------

int CALLBACK SalEnumFontsProcExA( const ENUMLOGFONTEXA* pLogFont,
                                  const NEWTEXTMETRICEXA* pMetric,
                                  DWORD nFontType, LPARAM lParam )
{
    ImplEnumInfo* pInfo = (ImplEnumInfo*)(void*)lParam;
    if ( !pInfo->mpName )
    {
        // Ignore vertical fonts
        if ( pLogFont->elfLogFont.lfFaceName[0] != '@' )
        {
            if ( !pInfo->mbImplSalCourierNew )
                pInfo->mbImplSalCourierNew = stricmp( pLogFont->elfLogFont.lfFaceName, "Courier New" ) == 0;
            if ( !pInfo->mbImplSalCourierScalable )
                pInfo->mbCourier = stricmp( pLogFont->elfLogFont.lfFaceName, "Courier" ) == 0;
            else
                pInfo->mbCourier = FALSE;
            String aName( ImplSalGetUniString( pLogFont->elfLogFont.lfFaceName ) );
            pInfo->mpName = &aName;
            strncpy( pInfo->mpLogFontA->lfFaceName, pLogFont->elfLogFont.lfFaceName, LF_FACESIZE );
            pInfo->mpLogFontA->lfCharSet = pLogFont->elfLogFont.lfCharSet;
            EnumFontFamiliesExA( pInfo->mhDC, pInfo->mpLogFontA, (FONTENUMPROCA)SalEnumFontsProcExA,
                                 (LPARAM)(void*)pInfo, 0 );
            pInfo->mpLogFontA->lfFaceName[0] = '\0';
            pInfo->mpLogFontA->lfCharSet = DEFAULT_CHARSET;
            pInfo->mpName = NULL;
            pInfo->mbCourier = FALSE;
        }
    }
    else
    {
        // ignore non-scalable non-device font on printer
        if( pInfo->mbPrinter )
            if( (nFontType & RASTER_FONTTYPE) && !(nFontType & DEVICE_FONTTYPE) )
                return 1;

        ImplFontData* pData = ImplLogMetricToDevFontDataA( pLogFont, &(pMetric->ntmTm), nFontType );

        // prefer the system character set, so that we get as much as
        // possible important characters. In the other case we could only
        // display a limited set of characters (#87309#)
        if ( pInfo->mnPreferedCharSet == pLogFont->elfLogFont.lfCharSet )
            pData->mnQuality += 100;

        // knowing Courier to be scalable is nice
        if( pInfo->mbCourier )
            pInfo->mbImplSalCourierScalable |= pData->IsScalable();

        pInfo->mpList->Add( pData );
    }

    return 1;
}

// -----------------------------------------------------------------------

int CALLBACK SalEnumFontsProcExW( const ENUMLOGFONTEXW* pLogFont,
                                  const NEWTEXTMETRICEXW* pMetric,
                                  DWORD nFontType, LPARAM lParam )
{
    ImplEnumInfo* pInfo = (ImplEnumInfo*)(void*)lParam;
    if ( !pInfo->mpName )
    {
        // Ignore vertical fonts
        if ( pLogFont->elfLogFont.lfFaceName[0] != '@' )
        {
            if ( !pInfo->mbImplSalCourierNew )
                pInfo->mbImplSalCourierNew = ImplSalWICompareAscii( pLogFont->elfLogFont.lfFaceName, "Courier New" ) == 0;
            if ( !pInfo->mbImplSalCourierScalable )
                pInfo->mbCourier = ImplSalWICompareAscii( pLogFont->elfLogFont.lfFaceName, "Courier" ) == 0;
            else
                pInfo->mbCourier = FALSE;
            String aName( pLogFont->elfLogFont.lfFaceName );
            pInfo->mpName = &aName;
            memcpy( pInfo->mpLogFontW->lfFaceName, pLogFont->elfLogFont.lfFaceName, (aName.Len()+1)*sizeof( wchar_t ) );
            pInfo->mpLogFontW->lfCharSet = pLogFont->elfLogFont.lfCharSet;
            EnumFontFamiliesExW( pInfo->mhDC, pInfo->mpLogFontW, (FONTENUMPROCW)SalEnumFontsProcExW,
                                 (LPARAM)(void*)pInfo, 0 );
            pInfo->mpLogFontW->lfFaceName[0] = '\0';
            pInfo->mpLogFontW->lfCharSet = DEFAULT_CHARSET;
            pInfo->mpName = NULL;
            pInfo->mbCourier = FALSE;
        }
    }
    else
    {
        // ignore non-scalable non-device font on printer
        if( pInfo->mbPrinter )
            if( (nFontType & RASTER_FONTTYPE) && !(nFontType & DEVICE_FONTTYPE) )
                return 1;

        ImplFontData* pData = ImplLogMetricToDevFontDataW( pLogFont, &(pMetric->ntmTm), nFontType );

        // knowing Courier to be scalable is nice
        if( pInfo->mbCourier )
            pInfo->mbImplSalCourierScalable |= pData->IsScalable();

        pInfo->mpList->Add( pData );
    }

    return 1;
}

// -----------------------------------------------------------------------

struct TempFontItem
{
    ::rtl::OUString maFontFilePath;
    ::rtl::OString maResourcePath;
    TempFontItem* mpNextItem;
};

bool ImplAddTempFont( SalData& rSalData, const String& rFontFileURL )
{
    int nRet = 0;
    ::rtl::OUString aUSytemPath;
    OSL_VERIFY( !osl::FileBase::getSystemPathFromFileURL( rFontFileURL, aUSytemPath ) );

#ifdef FR_PRIVATE   // wingdi.h, but only if _WIN32_WINNT >= 0x0500, which is currently not true.
    OSVERSIONINFO aVersion;
    aVersion.dwOSVersionInfoSize = sizeof(aVersion);
    if( ::GetVersionEx( &aVersion ) && (aVersion.dwMajorVersion >= 5) )
    {
        nRet = AddFontResourceExW( aUSytemPath.getStr(), FR_PRIVATE, NULL );
    }
    else
#endif
    {
        static int nCounter = 0;
        char aFileName[] = "soAA.fot";
        aFileName[2] = 'A' + (15 & (nCounter>>4));
        aFileName[3] = 'A' + (15 & nCounter);
        char aResourceName[512];
        int nMaxLen = sizeof(aResourceName)/sizeof(*aResourceName) - 16;
        int nLen = ::GetTempPathA( nMaxLen, aResourceName );
        ::strncpy( aResourceName + nLen, aFileName, sizeof( aResourceName )- nLen );
        // security: end buffer in any case
        aResourceName[ 511 ] = 0;
        ::DeleteFileA( aResourceName );

        rtl_TextEncoding theEncoding = osl_getThreadTextEncoding();
        ::rtl::OString aCFileName = rtl::OUStringToOString( aUSytemPath, theEncoding );
        // TODO: font should be private => need to investigate why it doesn't work then
        if( !::CreateScalableFontResourceA( 0, aResourceName, aCFileName.getStr(), NULL ) )
            return false;
        ++nCounter;

        nRet = ::AddFontResourceA( aResourceName );
        if( nRet > 0 )
        {
            TempFontItem* pNewItem = new TempFontItem;
            pNewItem->maResourcePath = rtl::OString( aResourceName );
            pNewItem->maFontFilePath = aUSytemPath.getStr();
            pNewItem->mpNextItem = rSalData.mpTempFontItem;
            rSalData.mpTempFontItem = pNewItem;
        }
    }

    return (nRet > 0);
}

// -----------------------------------------------------------------------

void ImplReleaseTempFonts( SalData& rSalData )
{
    int nCount = 0;
    while( TempFontItem* p = rSalData.mpTempFontItem )
    {
        ++nCount;
        if( p->maResourcePath.getLength() )
        {
            const char* pResourcePath = p->maResourcePath.getStr();
            ::RemoveFontResourceA( pResourcePath );
            ::DeleteFileA( pResourcePath );
        }
        else
        {
            if( aSalShlData.mbWNT )
                ::RemoveFontResourceW( p->maFontFilePath.getStr() );
            else
            {
                // poor man's string conversion because converter is gone
                int nLen = p->maFontFilePath.getLength();
                char* pNameA = new char[ nLen + 1 ];
                for( int i = 0; i < nLen; ++i )
                    pNameA[i] = (char)(p->maFontFilePath.getStr())[i];
                pNameA[ nLen ] = 0;
                ::RemoveFontResourceA( pNameA );
                delete[] pNameA;
            }
        }

        rSalData.mpTempFontItem = p->mpNextItem;
        delete p;
    }

    // notify everybody
    if( nCount )
        ::SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, NULL );
}

// -----------------------------------------------------------------------

String ImplGetFontNameFromFile( SalData& rSalData, const String& rFontFileURL )
{
    int nRet = 0;
    ::rtl::OUString aUSytemPath;
    OSL_VERIFY( !osl::FileBase::getSystemPathFromFileURL( rFontFileURL, aUSytemPath ) );

    String aFontName;

    // Create temporary file name
    char aFileName[] = "soAAT.fot";
    char aResourceName[512];
    int nMaxLen = sizeof(aResourceName)/sizeof(*aResourceName) - 16;
    int nLen = ::GetTempPathA( nMaxLen, aResourceName );
    ::strncpy( aResourceName + nLen, aFileName, Max( 0, nMaxLen - nLen ));
    ::DeleteFileA( aResourceName );

    // Create font resource file (typically with a .fot file name extension).
    rtl_TextEncoding theEncoding = osl_getThreadTextEncoding();
    ::rtl::OString aCFileName = rtl::OUStringToOString( aUSytemPath, theEncoding );
    ::CreateScalableFontResourceA( 0, aResourceName, aCFileName.getStr(), NULL );

    // Open font resource file
    rtl::OUString aFotFileName = rtl::OStringToOUString( aResourceName, osl_getThreadTextEncoding() );
    osl::FileBase::getFileURLFromSystemPath( aFotFileName, aFotFileName );
    osl::File aFotFile( aFotFileName );
    osl::FileBase::RC aError = aFotFile.open( osl_File_OpenFlag_Read );
    if ( aError == osl::FileBase::E_None )
    {
        static bool bInit = false;
        static char aFontTag[] = "FONTRES:";
        static char aSkip[256];
        if ( !bInit )
        {
            // Initialize skipping array
            memset( aSkip, strlen( aFontTag ), sizeof( aSkip ));
            aSkip['F'] = 7;
            aSkip['O'] = 6;
            aSkip['N'] = 5;
            aSkip['T'] = 4;
            aSkip['R'] = 3;
            aSkip['E'] = 2;
            aSkip['S'] = 1;
            aSkip[':'] = 0;
            bInit = TRUE;
        }

        char        aBuffer[2048];
        sal_Bool    bFound( sal_True );
        sal_Bool    bEOF( sal_False );
        sal_Int32   nPatternLen = strlen( aFontTag );
        sal_uInt64  nBytesRead;
        ByteString  fontName;

        // Read .fot file header and retrieve font name
        aFotFile.read( aBuffer, sizeof( aBuffer ), nBytesRead );
        if ( nBytesRead > nPatternLen )
        {
            sal_Int32 i, j;
            for ( i = nPatternLen-1, j = nPatternLen-1; j > 0; i--, j-- )
            {
                while ( aBuffer[i] != aFontTag[j] )
                {
                    sal_Int32 nSkip = aSkip[aBuffer[i]];
                    i += ( nPatternLen-j > nSkip ) ? nPatternLen-j : nSkip;
                    if ( i >= nBytesRead )
                    {
                        // Font tag not found
                        aFotFile.close();
                        ::DeleteFileA( aResourceName );
                        return String();
                    }
                    else
                        j = nPatternLen-1;
                }
            }

            // Retrieve font name from position (add tag size)
            i += nPatternLen;
            while ( i < nBytesRead && aBuffer[i] != 0 )
                fontName += aBuffer[i++];
        }

        // Convert byte string to unicode string
        aFontName = String( fontName, osl_getThreadTextEncoding() );
    }

    // Clean up
    aFotFile.close();
    ::DeleteFileA( aResourceName );

    return aFontName;
}

// -----------------------------------------------------------------------

bool WinSalGraphics::AddTempDevFont( ImplDevFontList* pFontList,
    const String& rFontFileURL, const String& rFontName )
{
    RTL_LOGFILE_TRACE1( "WinSalGraphics::AddTempDevFont(): %s", rtl::OUStringToOString( rFontFileURL, RTL_TEXTENCODING_ASCII_US ).getStr() );

    String aFontName( rFontName );

    // Search Font Name in Cache
     if( !aFontName.Len() && mpFontNameCache )
        aFontName = mpFontNameCache->GetFontName( rFontFileURL );

    // Retrieve font name from font resource
     if( !aFontName.Len() )
     {
        aFontName = ImplGetFontNameFromFile( *GetSalData(), rFontFileURL );
        if ( mpFontNameCache && aFontName.Len() )
            mpFontNameCache->AddFontName( rFontFileURL, aFontName );
     }

    if ( !aFontName.Len() )
        return false;

    if( ::ImplIsFontAvailable( mhDC, aFontName ) )
        return false;

    // remember temp font for cleanup later
    if( !ImplAddTempFont( *GetSalData(), rFontFileURL ) )
        return false;

    UINT nPreferedCharSet = DEFAULT_CHARSET;
    if ( !aSalShlData.mbWNT )
    {
        // for W98 guess charset preference from active codepage
        CHARSETINFO aCharSetInfo;
        DWORD nCP = GetACP();
        if ( TranslateCharsetInfo( (DWORD*)nCP, &aCharSetInfo, TCI_SRCCODEPAGE ) )
            nPreferedCharSet = aCharSetInfo.ciCharset;
    }

    // create matching FontData struct
    ImplDevFontAttributes aDFA;
    aDFA.maName       = aFontName;
    aDFA.mnQuality    = 1000;
    aDFA.mbDevice     = true;
    aDFA.mbSymbolFlag = false; // TODO: how to know it without accessing the font?
    aDFA.meFamily     = FAMILY_DONTKNOW;
    aDFA.meWidthType  = WIDTH_DONTKNOW;
    aDFA.meWeight     = WEIGHT_DONTKNOW;
    aDFA.meItalic     = ITALIC_DONTKNOW;
    aDFA.mePitch      = PITCH_DONTKNOW;;
    aDFA.mbSubsettable= false;
    aDFA.mbEmbeddable = false;
    aDFA.meEmbeddedBitmap = EMBEDDEDBITMAP_DONTKNOW;
    aDFA.meAntiAlias = ANTIALIAS_DONTKNOW;

    /*
    // TODO: improve ImplDevFontAttributes using "FONTRES:" from *.fot file
    aDFS.maName = // using "FONTRES:" from file
    if( rFontName != aDFS.maName )
        aDFS.maMapName = aFontName;
    */

    ImplFontData* pFontData = new ImplWinFontData( aDFA, 0,
        nPreferedCharSet, TMPF_VECTOR|TMPF_TRUETYPE );
    pFontList->Add( pFontData );
    return true;
}

// -----------------------------------------------------------------------

void WinSalGraphics::GetDevFontList( ImplDevFontList* pFontList )
{
    // make sure all fonts are registered at least temporarily
    static bool bOnce = true;
    if( bOnce )
    {
        bOnce = false;

        // determine font path
        // since we are only interested in fonts that could not be
        // registered before because of missing administration rights
        // only the font path of the user installation is needed
        ::rtl::OUString aPath;
        osl_getExecutableFile( &aPath.pData );
        ::rtl::OUString aExecutableFile( aPath );
        aPath = aPath.copy( 0, aPath.lastIndexOf('/') );
        String aFontDirUrl = aPath.copy( 0, aPath.lastIndexOf('/') );
        aFontDirUrl += String( RTL_CONSTASCII_USTRINGPARAM("/share/fonts/truetype") );

        // collect fonts in font path that could not be registered
        osl::Directory aFontDir( aFontDirUrl );
        osl::FileBase::RC rcOSL = aFontDir.open();
        if( rcOSL == osl::FileBase::E_None )
        {
            osl::DirectoryItem aDirItem;
            String aEmptyString;

            ::rtl::OUString aBootStrap = aExecutableFile.copy( 0, aExecutableFile.lastIndexOf('/')+1 );
            aBootStrap += String( RTL_CONSTASCII_USTRINGPARAM( SAL_CONFIGFILE( "bootstrap" ) ) );
            rtl::Bootstrap aBootstrap( aBootStrap );
            ::rtl::OUString aUserPath;
            aBootstrap.getFrom( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "UserInstallation" ) ), aUserPath );
            aUserPath += String( RTL_CONSTASCII_USTRINGPARAM("/user/config/fontnames.dat") );
            String aBaseURL = aPath.copy( 0, aPath.lastIndexOf('/')+1 );
            mpFontNameCache = new ImplFontNameCache( aUserPath, aBaseURL );

            while( aFontDir.getNextItem( aDirItem, 10 ) == osl::FileBase::E_None )
            {
                osl::FileStatus aFileStatus( FileStatusMask_FileURL );
                rcOSL = aDirItem.getFileStatus( aFileStatus );
                if ( rcOSL == osl::FileBase::E_None )
                    AddTempDevFont( pFontList, aFileStatus.getFileURL(), aEmptyString );
            }

            delete mpFontNameCache; // will write cache, if needed.
            mpFontNameCache = NULL;
        }
    }

    ImplEnumInfo aInfo;
    aInfo.mhDC          = mhDC;
    aInfo.mpList        = pFontList;
    aInfo.mpName        = NULL;
    aInfo.mpLogFontA    = NULL;
    aInfo.mpLogFontW    = NULL;
    aInfo.mbCourier     = false;
    aInfo.mbPrinter     = mbPrinter;
    if ( !mbPrinter )
    {
        aInfo.mbImplSalCourierScalable  = false;
        aInfo.mbImplSalCourierNew       = false;
    }
    else
    {
        aInfo.mbImplSalCourierScalable  = true;
        aInfo.mbImplSalCourierNew       = true;
    }

    aInfo.mnPreferedCharSet = DEFAULT_CHARSET;
    DWORD nCP = GetACP();
    CHARSETINFO aCharSetInfo;
    if ( TranslateCharsetInfo( (DWORD*)nCP, &aCharSetInfo, TCI_SRCCODEPAGE ) )
        aInfo.mnPreferedCharSet = aCharSetInfo.ciCharset;

    if ( aSalShlData.mbWNT )
    {
        LOGFONTW aLogFont;
        memset( &aLogFont, 0, sizeof( aLogFont ) );
        aLogFont.lfCharSet = DEFAULT_CHARSET;
        aInfo.mpLogFontW = &aLogFont;
        EnumFontFamiliesExW( mhDC, &aLogFont,
            (FONTENUMPROCW)SalEnumFontsProcExW, (LPARAM)(void*)&aInfo, 0 );
    }
    else
    {
        LOGFONTA aLogFont;
        memset( &aLogFont, 0, sizeof( aLogFont ) );
        aLogFont.lfCharSet = DEFAULT_CHARSET;
        aInfo.mpLogFontA = &aLogFont;
        EnumFontFamiliesExA( mhDC, &aLogFont,
            (FONTENUMPROCA)SalEnumFontsProcExA, (LPARAM)(void*)&aInfo, 0 );
    }

    // Feststellen, was es fuer Courier-Schriften auf dem Bildschirm gibt,
    // um in SetFont() evt. Courier auf Courier New zu mappen
    if ( !mbPrinter )
    {
        bImplSalCourierScalable = aInfo.mbImplSalCourierScalable;
        bImplSalCourierNew      = aInfo.mbImplSalCourierNew;
    }
}

// ----------------------------------------------------------------------------

void WinSalGraphics::GetDevFontSubstList( OutputDevice* pOutDev )
{}

// -----------------------------------------------------------------------

BOOL WinSalGraphics::GetGlyphBoundRect( long nIndex, Rectangle& rRect )
{
    HDC hDC = mhDC;

    // use unity matrix
    MAT2 aMat;
    aMat.eM11 = aMat.eM22 = FixedFromDouble( 1.0 );
    aMat.eM12 = aMat.eM21 = FixedFromDouble( 0.0 );

    UINT nGGOFlags = GGO_METRICS;
    if( !(nIndex & GF_ISCHAR) )
        nGGOFlags |= GGO_GLYPH_INDEX;
    nIndex &= GF_IDXMASK;

    GLYPHMETRICS aGM;
    DWORD nSize = GDI_ERROR;
    if ( aSalShlData.mbWNT )
        nSize = ::GetGlyphOutlineW( hDC, nIndex, nGGOFlags, &aGM, 0, NULL, &aMat );
    else if( (nGGOFlags & GGO_GLYPH_INDEX) || (nIndex <= 255) )
        nSize = ::GetGlyphOutlineA( hDC, nIndex, nGGOFlags, &aGM, 0, NULL, &aMat );

    if( nSize == GDI_ERROR )
        return false;

    rRect = Rectangle( Point( +aGM.gmptGlyphOrigin.x, -aGM.gmptGlyphOrigin.y ),
        Size( aGM.gmBlackBoxX, aGM.gmBlackBoxY ) );
    rRect.Left()    = static_cast<int>( mfFontScale * rRect.Left() );
    rRect.Right()   = static_cast<int>( mfFontScale * rRect.Right() );
    rRect.Top()     = static_cast<int>( mfFontScale * rRect.Top() );
    rRect.Bottom()  = static_cast<int>( mfFontScale * rRect.Bottom() );
    return true;
}

// -----------------------------------------------------------------------

BOOL WinSalGraphics::GetGlyphOutline( long nIndex,
    ::basegfx::B2DPolyPolygon& rB2DPolyPoly )
{
    rB2DPolyPoly.clear();

    BOOL bRet = FALSE;
    HDC  hDC = mhDC;

    // use unity matrix
    MAT2 aMat;
    aMat.eM11 = aMat.eM22 = FixedFromDouble( 1.0 );
    aMat.eM12 = aMat.eM21 = FixedFromDouble( 0.0 );

    UINT nGGOFlags = GGO_NATIVE;
    if( !(nIndex & GF_ISCHAR) )
        nGGOFlags |= GGO_GLYPH_INDEX;
    nIndex &= GF_IDXMASK;

    GLYPHMETRICS aGlyphMetrics;
    DWORD nSize1 = GDI_ERROR;
    if ( aSalShlData.mbWNT )
        nSize1 = ::GetGlyphOutlineW( hDC, nIndex, nGGOFlags, &aGlyphMetrics, 0, NULL, &aMat );
    else if( (nGGOFlags & GGO_GLYPH_INDEX) || (nIndex <= 255) )
        nSize1 = ::GetGlyphOutlineA( hDC, nIndex, nGGOFlags, &aGlyphMetrics, 0, NULL, &aMat );

    if( !nSize1 )       // blank glyphs are ok
        bRet = TRUE;
    else if( nSize1 != GDI_ERROR )
    {
        BYTE*   pData = new BYTE[ nSize1 ];
        ULONG   nTotalCount = 0;
        DWORD   nSize2;
        if ( aSalShlData.mbWNT )
            nSize2 = ::GetGlyphOutlineW( hDC, nIndex, nGGOFlags,
                &aGlyphMetrics, nSize1, pData, &aMat );
        else
            nSize2 = ::GetGlyphOutlineA( hDC, nIndex, nGGOFlags,
                &aGlyphMetrics, nSize1, pData, &aMat );

        if( nSize1 == nSize2 )
        {
            bRet = TRUE;

            int     nPtSize = 512;
            Point*  pPoints = new Point[ nPtSize ];
            BYTE*   pFlags = new BYTE[ nPtSize ];

            TTPOLYGONHEADER* pHeader = (TTPOLYGONHEADER*)pData;
            while( (BYTE*)pHeader < pData+nSize2 )
            {
                // only outline data is interesting
                if( pHeader->dwType != TT_POLYGON_TYPE )
                    break;

                // get start point; next start points are end points
                // of previous segment
                int nPnt = 0;

                long nX = IntTimes256FromFixed( pHeader->pfxStart.x );
                long nY = IntTimes256FromFixed( pHeader->pfxStart.y );
                pPoints[ nPnt ] = Point( nX, nY );
                pFlags[ nPnt++ ] = POLY_NORMAL;

                bool bHasOfflinePoints = false;
                TTPOLYCURVE* pCurve = (TTPOLYCURVE*)( pHeader + 1 );
                pHeader = (TTPOLYGONHEADER*)( (BYTE*)pHeader + pHeader->cb );
                while( (BYTE*)pCurve < (BYTE*)pHeader )
                {
                    int nNeededSize = nPnt + 16 + 3 * pCurve->cpfx;
                    if( nPtSize < nNeededSize )
                    {
                        Point* pOldPoints = pPoints;
                        BYTE* pOldFlags = pFlags;
                        nPtSize = 2 * nNeededSize;
                        pPoints = new Point[ nPtSize ];
                        pFlags = new BYTE[ nPtSize ];
                        for( int i = 0; i < nPnt; ++i )
                        {
                            pPoints[ i ] = pOldPoints[ i ];
                            pFlags[ i ] = pOldFlags[ i ];
                        }
                        delete[] pOldPoints;
                        delete[] pOldFlags;
                    }

                    int i = 0;
                    if( TT_PRIM_LINE == pCurve->wType )
                    {
                        while( i < pCurve->cpfx )
                        {
                            nX = IntTimes256FromFixed( pCurve->apfx[ i ].x );
                            nY = IntTimes256FromFixed( pCurve->apfx[ i ].y );
                            ++i;
                            pPoints[ nPnt ] = Point( nX, nY );
                            pFlags[ nPnt ] = POLY_NORMAL;
                            ++nPnt;
                        }
                    }
                    else if( TT_PRIM_QSPLINE == pCurve->wType )
                    {
                        bHasOfflinePoints = true;
                        while( i < pCurve->cpfx )
                        {
                            // get control point of quadratic bezier spline
                            nX = IntTimes256FromFixed( pCurve->apfx[ i ].x );
                            nY = IntTimes256FromFixed( pCurve->apfx[ i ].y );
                            ++i;
                            Point aControlP( nX, nY );

                            // calculate first cubic control point
                            // P0 = 1/3 * (PBeg + 2 * PQControl)
                            nX = pPoints[ nPnt-1 ].X() + 2 * aControlP.X();
                            nY = pPoints[ nPnt-1 ].Y() + 2 * aControlP.Y();
                            pPoints[ nPnt+0 ] = Point( (2*nX+3)/6, (2*nY+3)/6 );
                            pFlags[ nPnt+0 ] = POLY_CONTROL;

                            // calculate endpoint of segment
                            nX = IntTimes256FromFixed( pCurve->apfx[ i ].x );
                            nY = IntTimes256FromFixed( pCurve->apfx[ i ].y );

                            if ( i+1 >= pCurve->cpfx )
                            {
                                // endpoint is either last point in segment => advance
                                ++i;
                            }
                            else
                            {
                                // or endpoint is the middle of two control points
                                nX += IntTimes256FromFixed( pCurve->apfx[ i-1 ].x );
                                nY += IntTimes256FromFixed( pCurve->apfx[ i-1 ].y );
                                nX = (nX + 1) / 2;
                                nY = (nY + 1) / 2;
                                // no need to advance, because the current point
                                // is the control point in next bezier spline
                            }

                            pPoints[ nPnt+2 ] = Point( nX, nY );
                            pFlags[ nPnt+2 ] = POLY_NORMAL;

                            // calculate second cubic control point
                            // P1 = 1/3 * (PEnd + 2 * PQControl)
                            nX = pPoints[ nPnt+2 ].X() + 2 * aControlP.X();
                            nY = pPoints[ nPnt+2 ].Y() + 2 * aControlP.Y();
                            pPoints[ nPnt+1 ] = Point( (2*nX+3)/6, (2*nY+3)/6 );
                            pFlags[ nPnt+1 ] = POLY_CONTROL;

                            nPnt += 3;
                        }
                    }

                    // next curve segment
                    pCurve = (TTPOLYCURVE*)&pCurve->apfx[ i ];
                }

                // end point is start point for closed contour
                // disabled, because Polygon class closes the contour itself
                // pPoints[nPnt++] = pPoints[0];
                // #i35928#
                // Added again, but add only when not yet closed
                if(pPoints[nPnt - 1] != pPoints[0])
                {
                    if( bHasOfflinePoints )
                        pFlags[nPnt] = pFlags[0];

                    pPoints[nPnt++] = pPoints[0];
                }

                // convert y-coordinates W32 -> VCL
                for( int i = 0; i < nPnt; ++i )
                    pPoints[i].Y() = -pPoints[i].Y();

                // insert into polypolygon
                Polygon aPoly( nPnt, pPoints, (bHasOfflinePoints ? pFlags : NULL) );
                // convert to B2DPolyPolygon
                // TODO: get rid of the intermediate PolyPolygon
                rB2DPolyPoly.append( aPoly.getB2DPolygon() );
            }

            delete[] pPoints;
            delete[] pFlags;
        }

        delete[] pData;
    }

    // rescaling needed for the PolyPolygon conversion
    if( rB2DPolyPoly.count() )
    {
        ::basegfx::B2DHomMatrix aMatrix;
        aMatrix.scale( 1.0/256, 1.0/256 );
        aMatrix.scale( mfFontScale, mfFontScale );
        rB2DPolyPoly.transform( aMatrix );
    }

    return bRet;
}

// -----------------------------------------------------------------------

// TODO:  Replace this class with boost::scoped_array
class ScopedCharArray
{
public:
    inline explicit ScopedCharArray(char * pArray): m_pArray(pArray) {}

    inline ~ScopedCharArray() { delete[] m_pArray; }

    inline char * get() const { return m_pArray; }

private:
    char * m_pArray;
};

class ScopedFont
{
public:
    explicit ScopedFont(WinSalGraphics & rData);

    ~ScopedFont();

private:
    WinSalGraphics & m_rData;
    HFONT m_hOrigFont;
};

ScopedFont::ScopedFont(WinSalGraphics & rData): m_rData(rData)
{
    m_hOrigFont = m_rData.mhFonts[0];
    m_rData.mhFonts[0] = 0; // avoid deletion of current font
}

ScopedFont::~ScopedFont()
{
    if( m_hOrigFont )
    {
        // restore original font, destroy temporary font
        HFONT hTempFont = m_rData.mhFonts[0];
        m_rData.mhFonts[0] = m_hOrigFont;
        SelectObject( m_rData.mhDC, m_hOrigFont );
        DeleteObject( hTempFont );
    }
}

class ScopedTrueTypeFont
{
public:
    inline ScopedTrueTypeFont(): m_pFont(0) {}

    ~ScopedTrueTypeFont();

    int open(void * pBuffer, sal_uInt32 nLen, sal_uInt32 nFaceNum);

    inline TrueTypeFont * get() const { return m_pFont; }

private:
    TrueTypeFont * m_pFont;
};

ScopedTrueTypeFont::~ScopedTrueTypeFont()
{
    if (m_pFont != 0)
        CloseTTFont(m_pFont);
}

int ScopedTrueTypeFont::open(void * pBuffer, sal_uInt32 nLen,
                             sal_uInt32 nFaceNum)
{
    OSL_ENSURE(m_pFont == 0, "already open");
    return OpenTTFont(pBuffer, nLen, nFaceNum, &m_pFont);
}

BOOL WinSalGraphics::CreateFontSubset( const rtl::OUString& rToFile,
    ImplFontData* pFont, long* pGlyphIDs, sal_uInt8* pEncoding,
    sal_Int32* pGlyphWidths, int nGlyphCount, FontSubsetInfo& rInfo )
{
    // create matching ImplFontSelectData
    // we need just enough to get to the font file data
    // use height=1000 for easier debugging (to match psprint's font units)
    ImplFontSelectData aIFSD( *pFont, Size(0,1000), 0, false );

    // TODO: much better solution: move SetFont and restoration of old font to caller
    ScopedFont aOldFont(*this);
    SetFont( &aIFSD, 0 );

#if OSL_DEBUG_LEVEL > 1
    // get font metrics
    TEXTMETRICA aWinMetric;
    if( !::GetTextMetricsA( mhDC, &aWinMetric ) )
        return FALSE;

    DBG_ASSERT( !(aWinMetric.tmPitchAndFamily & TMPF_DEVICE), "cannot subset device font" );
    DBG_ASSERT( aWinMetric.tmPitchAndFamily & TMPF_TRUETYPE, "can only subset TT font" );
#endif

    // get raw font file data
    DWORD nFontSize1 = ::GetFontData( mhDC, 0, 0, NULL, 0 );
    if( nFontSize1 == GDI_ERROR )
        return FALSE;
    ScopedCharArray xRawFontData(new char[ nFontSize1 ]);
    DWORD nFontSize2 = ::GetFontData( mhDC, 0, 0, (void*)xRawFontData.get(), nFontSize1 );
    if( nFontSize1 != nFontSize2 )
        return FALSE;

    // open font file
    sal_uInt32 nFaceNum = 0;
    if( !*xRawFontData.get() )  // TTC candidate
        nFaceNum = ~0;  // indicate "TTC font extracts only"

    ScopedTrueTypeFont aSftTTF;
    int nRC = aSftTTF.open( xRawFontData.get(), nFontSize1, nFaceNum );
    if( nRC != SF_OK )
        return FALSE;

    TTGlobalFontInfo aTTInfo;
    ::GetTTGlobalFontInfo( aSftTTF.get(), &aTTInfo );
    rInfo.m_nFontType   = SAL_FONTSUBSETINFO_TYPE_TRUETYPE;
    rInfo.m_aPSName     = ImplSalGetUniString( aTTInfo.psname );
    rInfo.m_nAscent     = +aTTInfo.winAscent;
    rInfo.m_nDescent    = -aTTInfo.winDescent;
    rInfo.m_aFontBBox   = Rectangle( Point( aTTInfo.xMin, aTTInfo.yMin ),
                                    Point( aTTInfo.xMax, aTTInfo.yMax ) );
    rInfo.m_nCapHeight  = aTTInfo.yMax; // Well ...

    // subset glyphs and get their properties
    // take care that subset fonts require the NotDef glyph in pos 0
    int nOrigCount = nGlyphCount;
    USHORT    aShortIDs[ 256 ];
    sal_uInt8 aTempEncs[ 256 ];

    int nNotDef=-1, i;
    for( i = 0; i < nGlyphCount; ++i )
    {
        aTempEncs[i] = pEncoding[i];
        sal_uInt32 nGlyphIdx = pGlyphIDs[i] & GF_IDXMASK;
        if( pGlyphIDs[i] & GF_ISCHAR )
        {
            bool bVertical = (pGlyphIDs[i] & GF_ROTMASK) != 0;
            nGlyphIdx = ::MapChar( aSftTTF.get(), nGlyphIdx, bVertical );
            if( nGlyphIdx == 0 && pFont->IsSymbolFont() )
            {
                // #i12824# emulate symbol aliasing U+FXXX <-> U+0XXX
                nGlyphIdx = pGlyphIDs[i] & GF_IDXMASK;
                nGlyphIdx = (nGlyphIdx & 0xF000) ? (nGlyphIdx & 0x00FF) : (nGlyphIdx | 0xF000 );
                nGlyphIdx = ::MapChar( aSftTTF.get(), nGlyphIdx, bVertical );
            }
        }
        aShortIDs[i] = static_cast<USHORT>( nGlyphIdx );
        if( !nGlyphIdx )
            if( nNotDef < 0 )
                nNotDef = i; // first NotDef glyph found
    }

    if( nNotDef != 0 )
    {
        // add fake NotDef glyph if needed
        if( nNotDef < 0 )
            nNotDef = nGlyphCount++;

        // NotDef glyph must be in pos 0 => swap glyphids
        aShortIDs[ nNotDef ] = aShortIDs[0];
        aTempEncs[ nNotDef ] = aTempEncs[0];
        aShortIDs[0] = 0;
        aTempEncs[0] = 0;
    }
    DBG_ASSERT( nGlyphCount < 257, "too many glyphs for subsetting" );

    // fill pWidth array
    TTSimpleGlyphMetrics* pMetrics =
        ::GetTTSimpleGlyphMetrics( aSftTTF.get(), aShortIDs, nGlyphCount, aIFSD.mbVertical );
    if( !pMetrics )
        return FALSE;
    sal_uInt16 nNotDefAdv   = pMetrics[0].adv;
    pMetrics[0].adv         = pMetrics[nNotDef].adv;
    pMetrics[nNotDef].adv   = nNotDefAdv;
    for( i = 0; i < nOrigCount; ++i )
        pGlyphWidths[i] = pMetrics[i].adv;
    free( pMetrics );

    // write subset into destination file
    rtl::OUString aSysPath;
    if( osl_File_E_None != osl_getSystemPathFromFileURL( rToFile.pData, &aSysPath.pData ) )
        return FALSE;
    rtl_TextEncoding aThreadEncoding = osl_getThreadTextEncoding();
    ByteString aToFile( rtl::OUStringToOString( aSysPath, aThreadEncoding ) );
    nRC = ::CreateTTFromTTGlyphs( aSftTTF.get(), aToFile.GetBuffer(), aShortIDs,
            aTempEncs, nGlyphCount, 0, NULL, 0 );
    return nRC == SF_OK;
}

//--------------------------------------------------------------------------

const void* WinSalGraphics::GetEmbedFontData( ImplFontData* pFont,
    const sal_Unicode* pUnicodes, sal_Int32* pCharWidths,
    FontSubsetInfo& rInfo, long* pDataLen )
{
    // create matching ImplFontSelectData
    // we need just enough to get to the font file data
    ImplFontSelectData aIFSD( *pFont, Size(0,1000), 0, false );

    // TODO: much better solution: move SetFont and restoration of old font to caller
    ScopedFont aOldFont(*this);
    SetFont( &aIFSD, 0 );

    // get the raw font file data
    DWORD nFontSize1 = ::GetFontData( mhDC, 0, 0, NULL, 0 );
    if( nFontSize1 == GDI_ERROR || nFontSize1 <= 0 )
        return NULL;
    *pDataLen = nFontSize1;
    void* pData = reinterpret_cast<void*>(new char[ nFontSize1 ]);
    DWORD nFontSize2 = ::GetFontData( mhDC, 0, 0, pData, nFontSize1 );
    if( nFontSize1 != nFontSize2 )
        *pDataLen = 0;

    // get important font properties
    TEXTMETRICA aTm;
    if( !::GetTextMetricsA( mhDC, &aTm ) )
        *pDataLen = 0;
    rInfo.m_nFontType   = SAL_FONTSUBSETINFO_TYPE_TYPE1;
    WCHAR aFaceName[64];
    int nFNLen = ::GetTextFaceW( mhDC, 64, aFaceName );
    // #i59854# strip eventual null byte
    while( nFNLen > 0 && aFaceName[nFNLen-1] == 0 )
        nFNLen--;
    if( nFNLen == 0 )
        *pDataLen = 0;
    rInfo.m_aPSName     = String( aFaceName, nFNLen );
    rInfo.m_nAscent     = +aTm.tmAscent;
    rInfo.m_nDescent    = -aTm.tmDescent;
    rInfo.m_aFontBBox   = Rectangle( Point( -aTm.tmOverhang, -aTm.tmDescent ),
              Point( aTm.tmMaxCharWidth, aTm.tmAscent+aTm.tmExternalLeading ) );
    rInfo.m_nCapHeight  = aTm.tmAscent; // Well ...

    // get individual character widths
    for( int i = 0; i < 256; ++i )
    {
        int nCharWidth = 0;
        const sal_Unicode cChar = pUnicodes[i];
        if( !::GetCharWidth32W( mhDC, cChar, cChar, &nCharWidth ) )
            *pDataLen = 0;
        pCharWidths[i] = nCharWidth;
    }

    if( !*pDataLen )
    {
        FreeEmbedFontData( pData, nFontSize1 );
        pData = NULL;
    }

    return pData;
}

//--------------------------------------------------------------------------

void WinSalGraphics::FreeEmbedFontData( const void* pData, long nLen )
{
    delete[] reinterpret_cast<char*>(const_cast<void*>(pData));
}

//--------------------------------------------------------------------------

const std::map< sal_Unicode, sal_Int32 >* WinSalGraphics::GetFontEncodingVector(
    ImplFontData* pFont, const std::map< sal_Unicode,rtl::OString>** pNonEncoded )
{
    // TODO: even for builtin fonts we get here... why?
    if( !pFont->IsEmbeddable() )
        return NULL;

    // fill the encoding vector
    typedef std::map< sal_Unicode,sal_Int32> Code2Int;
    Code2Int& rMap = *new Code2Int;
#if 0
    // TODO: get correct encoding vector
    ImplWinFontData* pWinFontData = reinterpret_cast<ImplWinFontData*>(pFont);

    GLYPHSET aGlyphSet;
    aGlyphSet.cbThis = sizeof(aGlyphSet);
    DWORD aW = ::GetFontUnicodeRanges( mhDC, &aGlyphSet);
#else
    for( int i = 32; i < 256; ++i )
        rMap[i] = i;
    if( pNonEncoded )
        *pNonEncoded = NULL;
#endif

    return &rMap;
}

//--------------------------------------------------------------------------

void WinSalGraphics::DrawServerFontLayout( const ServerFontLayout& )
{}

//--------------------------------------------------------------------------
