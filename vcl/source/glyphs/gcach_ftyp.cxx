/*************************************************************************
 *
 *  $RCSfile: gcach_ftyp.cxx,v $
 *
 *  $Revision: 1.18 $
 *  last change: $Author: hdu $ $Date: 2001-03-14 16:41:48 $
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

#include <gcach_ftyp.hxx>

#include <svapp.hxx>
#include <outfont.hxx>
#include <bitmap.hxx>
#include <bmpacc.hxx>

//#include <rtl/ustring>        // used only for string=>hashvalue
#include <osl/file.hxx>
#include <poly.hxx>

#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/tttables.h"
#include "freetype/tttags.h"
#include "freetype/ttnameid.h"

#include <vector>

// -----------------------------------------------------------------------

static FT_Library aLibFT = 0;

// =======================================================================
// FreetypeManager
// =======================================================================

#include "freetype/internal/ftobjs.h"
#include "freetype/internal/sfnt.h"
#include "freetype/internal/ftstream.h"

// =======================================================================

size_t std::hash<FtFontInfo*>::operator()( const FtFontInfo* pFI ) const
{
    size_t nHash = (size_t)pFI;
    return nHash;
}

// -----------------------------------------------------------------------

bool std::equal_to<FtFontInfo*>::operator()( const FtFontInfo* pA, const FtFontInfo* pB ) const
{
    return (pA == pB);
}

// -----------------------------------------------------------------------

FreetypeManager::FreetypeManager()
{
    FT_Error rcFT = FT_Init_FreeType( &aLibFT );
}

// -----------------------------------------------------------------------

FreetypeManager::~FreetypeManager()
{
    FT_Error rcFT = FT_Done_FreeType( aLibFT );
}

// -----------------------------------------------------------------------

void FreetypeManager::AddFontFile( const String& rNormalizedName,
    int nFaceNum, int nFontId, const ImplFontData* pData )
{
    if( !rNormalizedName.Len() )
        return;

    FtFontInfo* const pFontInfo = new FtFontInfo;

    pFontInfo->aNativeFileName  = ::rtl::OString( rNormalizedName.GetBuffer(),
        rNormalizedName.Len(), RTL_TEXTENCODING_UNICODE );
    pFontInfo->nFaceNum = nFaceNum;
    pFontInfo->nFontId  = nFontId;

    ImplFontData& rData = pFontInfo->aFontData;
    rData               = *pData;
    rData.mpSysData     = pFontInfo;
    rData.mpNext        = NULL;

    // using unicode emulation for non-symbol fonts
    if( rData.meCharSet != RTL_TEXTENCODING_SYMBOL )
        rData.meCharSet = RTL_TEXTENCODING_UNICODE;
    rData.meScript      = SCRIPT_DONTKNOW;

    maFontList.insert( pFontInfo );
}

// -----------------------------------------------------------------------

long FreetypeManager::AddFontDir( const String& rNormalizedName )
{
    osl::Directory aDir( rNormalizedName );
    osl::FileBase::RC rcOSL = aDir.open();
    if( rcOSL != osl::FileBase::E_None )
        return 0;

    long nCount = 0;

    osl::DirectoryItem aDirItem;
    while( (rcOSL = aDir.getNextItem( aDirItem, 20 )) == osl::FileBase::E_None )
    {
        osl::FileStatus aFileStatus( FileStatusMask_NativePath );

        rcOSL = aDirItem.getFileStatus( aFileStatus );
        ::rtl::OUString aUFileName = aFileStatus.getNativePath();
        ::rtl::OString aCFileName = rtl::OUStringToOString( aUFileName, RTL_TEXTENCODING_DONTKNOW );
        const char* pszFontFileName = aCFileName.getStr();

        FT_FaceRec_* aFaceFT = NULL;
        for( int nFaceNum = 0, nMaxFaces = 1; nFaceNum < nMaxFaces; ++nFaceNum )
        {
            FT_Error rcFT = FT_New_Face( aLibFT, pszFontFileName, nFaceNum, &aFaceFT );
            if( (rcFT != FT_Err_Ok) || (aFaceFT == NULL) )
                break;

            if( !FT_IS_SCALABLE( aFaceFT ) )    // ignore non-TT fonts for now
                continue;

            nMaxFaces = aFaceFT->num_faces;

            FtFontInfo* const pFontInfo = new FtFontInfo;
            pFontInfo->aNativeFileName  = aCFileName;
            pFontInfo->nFaceNum         = nFaceNum;

            ImplFontData& rData = pFontInfo->aFontData;

            rData.mpNext        = NULL;
            rData.mpSysData     = pFontInfo;

            // TODO: prefer unicode names if available
            // TODO: prefer locale specific names if available?
            rData.maName        = String::CreateFromAscii( aFaceFT->family_name );
            rData.maStyleName   = String::CreateFromAscii( aFaceFT->style_name );

            rData.mnWidth       = 0;
            rData.mnHeight      = 0;

            // TODO: extract better font characterization data from font
            rData.meFamily      = FAMILY_DONTKNOW;

            rData.meCharSet = RTL_TEXTENCODING_UNICODE;
            for( int i = aFaceFT->num_charmaps; --i >= 0; )
            {
                const FT_CharMap aCM = aFaceFT->charmaps[i];
                if(
                // TODO for FT>=2.0: "(aCM->platform_id == TT_PLATFORM_MICROSOFT) &&"
                    (aCM->encoding_id == TT_MS_ID_SYMBOL_CS) )
                    rData.meCharSet = RTL_TEXTENCODING_SYMBOL;
            }

            rData.meScript      = SCRIPT_DONTKNOW;
            rData.mePitch       = FT_IS_FIXED_WIDTH( aFaceFT ) ? PITCH_FIXED : PITCH_VARIABLE;
            rData.meWidthType   = WIDTH_DONTKNOW;
            rData.meWeight      = (aFaceFT->style_flags & FT_STYLE_FLAG_BOLD) ? WEIGHT_BOLD : WEIGHT_NORMAL;
            rData.meItalic      = (aFaceFT->style_flags & FT_STYLE_FLAG_ITALIC) ? ITALIC_NORMAL : ITALIC_NONE;

            rData.mnVerticalOrientation = 0;
            rData.mbOrientation = true;
            rData.mbDevice      = false;
            rData.mnQuality     = 0;        // prefer client-side fonts if available

            FT_Done_Face( aFaceFT );

            maFontList.insert( pFontInfo );
            ++nCount;
        }
    }

    aDir.close();
    return nCount;
}

// -----------------------------------------------------------------------

long FreetypeManager::FetchFontList( ImplDevFontList* pToAdd ) const
{
    long nCount = 0;
    for( FontList::const_iterator it(maFontList.begin()); it != maFontList.end(); ++it, ++nCount )
        pToAdd->Add( new ImplFontData( (*it)->aFontData ) );
    return nCount;
}

// -----------------------------------------------------------------------

void FreetypeManager::ClearFontList( )
{
    for( FontList::iterator it(maFontList.begin()); it != maFontList.end(); )
        delete *(it++);
    maFontList.clear();
}

// -----------------------------------------------------------------------

FreetypeServerFont* FreetypeManager::CreateFont( const ImplFontSelectData& rFSD )
{
    FtFontInfo* pFI = (FtFontInfo*)rFSD.mpFontData->mpSysData;
    if( maFontList.find( pFI ) != maFontList.end() )
    {
        FreetypeServerFont* pFont = new FreetypeServerFont( rFSD, *pFI );
        return pFont;
    }

    return NULL;
}

// =======================================================================
// FreetypeServerFont
// =======================================================================

FreetypeServerFont::FreetypeServerFont( const ImplFontSelectData& rFSD, const FtFontInfo& rFI )
:   ServerFont(rFSD),
    mrFontInfo(const_cast<FtFontInfo&>(rFI)),
    maFaceFT(NULL)
{
    const char* pszFontFileName = rFI.aNativeFileName.getStr();

    FT_Error rc = FT_New_Face( aLibFT, pszFontFileName, rFI.nFaceNum, &maFaceFT );
    if( rc != FT_Err_Ok )
        return;

    FT_Encoding eEncoding = ft_encoding_unicode;
    if( mrFontInfo.aFontData.meCharSet == RTL_TEXTENCODING_SYMBOL )
    {
        // TODO for FT>200b8 use "ft_encoding_symbol"
        //### TODO: some PS symbol fonts don't map their symbols correctly
        eEncoding = ft_encoding_none;
eEncoding = ft_encoding_symbol;//###
    }
    rc = FT_Select_Charmap( maFaceFT, eEncoding );

    mnWidth = rFSD.mnWidth;
    if( !mnWidth )
        mnWidth = rFSD.mnHeight;
    rc = FT_Set_Pixel_Sizes( maFaceFT, mnWidth, rFSD.mnHeight );

    ApplyGSUB( rFSD );

    // TODO: query GASP table for load flags
    mnLoadFlags = FT_LOAD_DEFAULT;

    if( rFSD.mnOrientation != 0 )       // no embedded bitmap for rotated text
        mnLoadFlags |= FT_LOAD_NO_BITMAP;
    //if( nSin != 0 && nCos != 0 )      // hinting for 0/90/180/270 degrees only
    //  mnLoadFlags |= FT_LOAD_NO_HINTING;
    //if( rFSD.mbVertical )
    //  mnLoadFlags |= FT_LOAD_VERTICAL_LAYOUT;
}

// -----------------------------------------------------------------------

FreetypeServerFont::~FreetypeServerFont()
{
    if( maFaceFT )
        FT_Done_Face( maFaceFT );
}

// -----------------------------------------------------------------------

void FreetypeServerFont::FetchFontMetric( ImplFontMetricData& rTo, long& rFactor ) const
{
    rFactor = 0x100;

    rTo.mnWidth             = mnWidth;

    const FT_Size_Metrics& rMetrics = maFaceFT->size->metrics;
    rTo.mnAscent            = (+rMetrics.ascender + 32) >> 6;
    // TODO: change +desc to -desc for FT_Version>2.0beta8
    rTo.mnDescent           = (+rMetrics.descender + 32) >> 6;
    rTo.mnLeading           = ((rMetrics.height + 32) >> 6) - (rTo.mnAscent + rTo.mnDescent);
    rTo.mnSlant             = 0;

    rTo.maName              = mrFontInfo.aFontData.maName;
    rTo.maStyleName         = mrFontInfo.aFontData.maStyleName;

    rTo.mnFirstChar         = 0x0020;
    rTo.mnLastChar          = 0xFFFE;

    rTo.mnOrientation       = GetFontSelData().mnOrientation;

    rTo.meCharSet           = mrFontInfo.aFontData.meCharSet;
    rTo.meFamily            = mrFontInfo.aFontData.meFamily;
    rTo.meWeight            = mrFontInfo.aFontData.meWeight;
    rTo.meItalic            = mrFontInfo.aFontData.meItalic;
    rTo.mePitch             = mrFontInfo.aFontData.mePitch;

    rTo.meType              = TYPE_SCALABLE;
    rTo.mbDevice            = FALSE;

    rTo.mnStrikeoutSize     = rTo.mnAscent / 6;
    rTo.mnStrikeoutOffset   = +rTo.mnAscent / 3;

    const TT_OS2* pOS2 = (const TT_OS2*)FT_Get_Sfnt_Table( maFaceFT, ft_sfnt_os2 );
    if( pOS2 && (~pOS2->version != 0) )
    {
        const double fScale = (double)GetFontSelData().mnHeight / maFaceFT->units_per_EM;
        rTo.mnAscent        = (long)( +pOS2->usWinAscent * fScale + 0.5 );
        rTo.mnDescent       = (long)( +pOS2->usWinDescent * fScale + 0.5 );
        rTo.mnLeading       = (long)( (+pOS2->usWinAscent + pOS2->usWinDescent - maFaceFT->units_per_EM) * fScale + 0.5 );

        rTo.mnFirstChar     = pOS2->usFirstCharIndex;
        rTo.mnLastChar      = pOS2->usLastCharIndex;

        rTo.mnStrikeoutSize     = Min( 1L, (long)( pOS2->yStrikeoutSize * fScale + 0.5 ) );
        rTo.mnStrikeoutOffset   = (long)( pOS2->yStrikeoutPosition * fScale + 0.5 );
    }
}

// -----------------------------------------------------------------------

static int SetVerticalFlags( sal_Unicode nChar )
{
    if ( (nChar >= 0x1100 && nChar <= 0x11f9)   // Hangul Jamo
    ||  (nChar >= 0x3000 && nChar <= 0xfaff) )  // other CJK
    {
        if( nChar == 0x2010 || nChar == 0x2015
        ||  nChar == 0x2016 || nChar == 0x2026
        || (nChar >= 0x3008 && nChar <= 0x3017) )
            return 0;   // not turned
/*
        else if( nChar == 0x3001 || nChar == 0x3002 )
        {
            return VCLASS_TRANSFORM1;
        }
        else if( nChar == 0x3041 || nChar == 0x3043
        ||  nChar == 0x3045 || nChar == 0x3047
        ||  nChar == 0x3049 || nChar == 0x3063
        ||  nChar == 0x3083 || nChar == 0x3085
        ||  nChar == 0x3087 || nChar == 0x308e
        ||  nChar == 0x30a1 || nChar == 0x30a3
        ||  nChar == 0x30a5 || nChar == 0x30a7
        ||  nChar == 0x30a9 || nChar == 0x30c3
        ||  nChar == 0x30e3 || nChar == 0x30e5
        ||  nChar == 0x30e7 || nChar == 0x30ee
        ||  nChar == 0x30f5 || nChar == 0x30f6 )
        {
            return VCLASS_TRANSFORM2;
        }
*/
        else if ( nChar == 0x30fc )
            return +2;  // right
        return +1;      // left
    }

    return 0;
}

static inline void SetGlyphFlags( int& nGlyphIndex, int nFlags )
{
    nGlyphIndex |= (nFlags << 28);
}

static inline void SplitGlyphFlags( int& nGlyphIndex, int& nGlyphFlags )
{
    nGlyphFlags = (nGlyphIndex >> 28);
    nGlyphIndex &= 0x00ffffff;
}

static void SetTransform( int nSin, int nCos, int nHeight, int nGlyphFlags, FT_Glyph& rGlyphFT )
{
    FT_Vector aVector;
    FT_Matrix aMatrix;
    switch( nGlyphFlags )
    {
    case +1:    // left
        aMatrix.xx = -nSin;
        aMatrix.yy = -nSin;
        aMatrix.xy = -nCos;
        aMatrix.yx = +nCos;
        aVector.x  = +(nHeight * nCos) >> 10;
        aVector.y  = +(nHeight * nSin) >> 10;
        break;
    case +2:    // right
        aMatrix.xx = -nSin;
        aMatrix.yy = -nSin;
        aMatrix.xy = +nCos;
        aMatrix.yx = -nCos;
        aVector.x  = -(nHeight * nCos) >> 10;
        aVector.y  = -(nHeight * nSin) >> 10;
        break;
    default:    // straight
        aMatrix.xx = +nCos;
        aMatrix.yy = +nCos;
        aMatrix.xy = -nSin;
        aMatrix.yx = +nSin;
        aVector.x = 0;
        aVector.y = 0;
        break;
    }

    FT_Glyph_Transform( rGlyphFT, &aMatrix, &aVector );
}

// -----------------------------------------------------------------------

int FreetypeServerFont::GetGlyphIndex( sal_Unicode aChar ) const
{
    if( mrFontInfo.aFontData.meCharSet == RTL_TEXTENCODING_SYMBOL )
        aChar |= 0xF000;    // emulate W2K hig/low mapping of symbols

    int nGlyphIndex = FT_Get_Char_Index( maFaceFT, aChar );

    // do glyph substitution if necessary
    GlyphSubstitution::const_iterator it = aGlyphSubstitution.find( nGlyphIndex );
    // use OpenType substitution if available
    if( it != aGlyphSubstitution.end() )
    {
#ifdef DEBUG
        fprintf(stderr,"GetGlyphIndex(0x%04X) subst 0x%04X=>0x%04X\n", aChar, nGlyphIndex, (*it).second);
#endif
        nGlyphIndex = (*it).second;
    }

    // CJK vertical writing needs special treatment
    if( GetFontSelData().mbVertical )
    {
        int nVertFlags = SetVerticalFlags( aChar );
        SetGlyphFlags( nGlyphIndex, nVertFlags );
    }

    return nGlyphIndex;
}

// -----------------------------------------------------------------------

void FreetypeServerFont::InitGlyphData( int nGlyphIndex, GlyphData& rGD ) const
{
    int nGlyphFlags;
    SplitGlyphFlags( nGlyphIndex, nGlyphFlags );

    FT_Error rc = FT_Load_Glyph( maFaceFT, nGlyphIndex, mnLoadFlags );
    if( rc != FT_Err_Ok )
    {
        // we get here e.g. when a PS font lacks the default glyph
        rGD.SetCharWidth( 0 );
        rGD.SetDelta( 0, 0 );
        rGD.SetOffset( 0, 0 );
        rGD.SetSize( Size( 0, 0 ) );
        return;
    }

    rGD.SetCharWidth( (maFaceFT->glyph->metrics.horiAdvance + 32) >> 6 );

    FT_Glyph aGlyphFT;
    rc = FT_Get_Glyph( maFaceFT->glyph, &aGlyphFT );

    rGD.SetDelta( (aGlyphFT->advance.x + 0x8000) >> 16, -((aGlyphFT->advance.y + 0x8000) >> 16) );

    FT_BBox aBbox;
    FT_Glyph_Get_CBox( aGlyphFT, ft_glyph_bbox_pixels, &aBbox );
    if( aBbox.yMin > aBbox.yMax )   // circumvent freetype bug
    {
        int t=aBbox.yMin; aBbox.yMin=aBbox.yMax, aBbox.yMax=t;
    }
    rGD.SetOffset( aBbox.xMin, -aBbox.yMax );
    rGD.SetSize( Size( (aBbox.xMax-aBbox.xMin+1), (aBbox.yMax-aBbox.yMin) ) );

    FT_Done_Glyph( aGlyphFT );
}

// -----------------------------------------------------------------------

bool FreetypeServerFont::GetGlyphBitmap1( int nGlyphIndex, RawBitmap& rRawBitmap ) const
{
    int nGlyphFlags;
    SplitGlyphFlags( nGlyphIndex, nGlyphFlags );
    FT_Int nLoadFlags = mnLoadFlags;
    if( nGlyphFlags != 0 )
        nLoadFlags |= FT_LOAD_NO_BITMAP;

    FT_Error rc = FT_Load_Glyph( maFaceFT, nGlyphIndex, nLoadFlags );
    if( rc != FT_Err_Ok )
        return false;

    FT_Glyph aGlyphFT;
    rc = FT_Get_Glyph( maFaceFT->glyph, &aGlyphFT );
    if( rc != FT_Err_Ok )
        return false;

    if( aGlyphFT->format != ft_glyph_format_bitmap )
    {
        if( (nCos!=0x10000) || (nGlyphFlags!=0) )
            SetTransform( nSin, nCos, GetFontSelData().mnHeight,
                nGlyphFlags, aGlyphFT );

        if( aGlyphFT->format == ft_glyph_format_outline )
            ((FT_OutlineGlyphRec*)aGlyphFT )->outline.flags |= ft_outline_high_precision;
        rc = FT_Glyph_To_Bitmap( &aGlyphFT, ft_render_mode_mono, NULL, TRUE );
        if( rc != FT_Err_Ok )
            return false;
    }

    const FT_BitmapGlyph& rBmpGlyphFT = reinterpret_cast<const FT_BitmapGlyph&>(aGlyphFT);
    // autohinting miscaculates the offsets below by +-1
    rRawBitmap.mnXOffset        = +rBmpGlyphFT->left;
    rRawBitmap.mnYOffset        = -rBmpGlyphFT->top;

    const FT_Bitmap& rBitmapFT  = rBmpGlyphFT->bitmap;
    rRawBitmap.mnHeight         = rBitmapFT.rows;
    rRawBitmap.mnWidth          = rBitmapFT.width;
    rRawBitmap.mnScanlineSize   = rBitmapFT.pitch;
    rRawBitmap.mnBitCount       = 1;

    const ULONG nNeededSize = rRawBitmap.mnScanlineSize * rRawBitmap.mnHeight;
    if( rRawBitmap.mnAllocated < nNeededSize )
    {
        delete[] rRawBitmap.mpBits;
        rRawBitmap.mnAllocated = 2*nNeededSize;
        rRawBitmap.mpBits = new unsigned char[ rRawBitmap.mnAllocated ];
    }

    const unsigned char* pSrc = rBitmapFT.buffer;
    unsigned char* pDest = rRawBitmap.mpBits;
     for( int i = nNeededSize; --i >= 0; )
         *(pDest++) = ~*(pSrc++);

    FT_Done_Glyph( aGlyphFT );
    return true;
}

// -----------------------------------------------------------------------

bool FreetypeServerFont::GetGlyphBitmap8( int nGlyphIndex, RawBitmap& rRawBitmap ) const
{
    int nGlyphFlags;
    SplitGlyphFlags( nGlyphIndex, nGlyphFlags );

    FT_Int nLoadFlags = mnLoadFlags | FT_LOAD_NO_BITMAP;
    FT_Error rc = FT_Load_Glyph( maFaceFT, nGlyphIndex, nLoadFlags );
    if( rc != FT_Err_Ok )
        return false;

    FT_Glyph aGlyphFT;
    rc = FT_Get_Glyph( maFaceFT->glyph, &aGlyphFT );
    if( rc != FT_Err_Ok )
        return false;

    if( aGlyphFT->format == ft_glyph_format_outline )
    {
        if( (nCos!=0x10000) || (nGlyphFlags!=0) )
            SetTransform( nSin, nCos, GetFontSelData().mnHeight,
                nGlyphFlags, aGlyphFT );

        ((FT_OutlineGlyph)aGlyphFT)->outline.flags |= ft_outline_high_precision;
    }

    rc = FT_Glyph_To_Bitmap( &aGlyphFT, ft_render_mode_normal, NULL, TRUE );
    if( rc != FT_Err_Ok )
        return false;

    const FT_BitmapGlyph& rBmpGlyphFT = reinterpret_cast<const FT_BitmapGlyph&>(aGlyphFT);
    rRawBitmap.mnXOffset        = +rBmpGlyphFT->left;
    rRawBitmap.mnYOffset        = -rBmpGlyphFT->top;

    const FT_Bitmap& rBitmapFT  = rBmpGlyphFT->bitmap;
    rRawBitmap.mnHeight         = rBitmapFT.rows;
    rRawBitmap.mnWidth          = rBitmapFT.width;
    rRawBitmap.mnScanlineSize   = (rBitmapFT.pitch + 3) & -4;
    rRawBitmap.mnBitCount       = 8;

    const ULONG nNeededSize = rRawBitmap.mnScanlineSize * rRawBitmap.mnHeight;
    if( rRawBitmap.mnAllocated < nNeededSize )
    {
        delete[] rRawBitmap.mpBits;
        rRawBitmap.mnAllocated = 2*nNeededSize;
        rRawBitmap.mpBits = new unsigned char[ rRawBitmap.mnAllocated ];
    }

    const unsigned char* pSrc = rBitmapFT.buffer;
    unsigned char* pDest = rRawBitmap.mpBits;
    for( int y = rRawBitmap.mnHeight, x; --y >= 0 ; )
    {
        for( x = 0; x < rBitmapFT.width; ++x )
            *(pDest++) = *(pSrc++);
        for(; x < rRawBitmap.mnScanlineSize; ++x )
            *(pDest++) = 0;
    }

    FT_Done_Glyph( aGlyphFT );
    return true;
}

// -----------------------------------------------------------------------
// kerning stuff
// -----------------------------------------------------------------------

ULONG FreetypeServerFont::GetKernPairs( ImplKernPairData** ppKernPairs ) const
{
    *ppKernPairs = NULL;
    if( !FT_HAS_KERNING( maFaceFT ) )
        return 0;

    // first figure out which glyph pairs are involved in kerning
    SFNT_Interface* pSFNT = (SFNT_Interface*) FT_Get_Module_Interface( aLibFT, "sfnt" );
    if( !pSFNT )
        return 0;

    FT_ULong nKernLength = 0;
    FT_Error rcFT = pSFNT->load_any( (TT_Face)maFaceFT, TTAG_kern, 0, NULL, &nKernLength );
    if( rcFT != FT_Err_Ok )
        return 0;

    FT_Byte* const pKern = new FT_Byte[ nKernLength ];
    rcFT = pSFNT->load_any( (TT_Face)maFaceFT, TTAG_kern, 0, pKern, &nKernLength );

    typedef std::vector<ImplKernPairData> KernVector;
    KernVector aKernGlyphVector;
    ImplKernPairData aKernPair;

    const FT_Byte* pBuffer = pKern;
    USHORT nVersion = NEXT_UShort( pBuffer );
    USHORT nTableCnt = NEXT_UShort( pBuffer );
    if( nVersion != 0 )     // ignore Apple's versions for now
        nTableCnt = 0;
    for( USHORT nTableIdx = 0; nTableIdx < nTableCnt; ++nTableIdx )
    {
        USHORT nSubVersion  = NEXT_UShort( pBuffer );
        USHORT nSubLength   = NEXT_UShort( pBuffer );
        USHORT nSubCoverage = NEXT_UShort( pBuffer );
        if( (nSubCoverage&0x03) != 0x01 )   // no interest in minimum info here
            continue;
        switch( nSubCoverage >> 8 )
        {
            case 0: // version 0, kerning format 0
            {
                USHORT nPairs = NEXT_UShort( pBuffer );
                pBuffer += 6;   // skip search hints
                aKernGlyphVector.reserve( aKernGlyphVector.size() + nPairs );
                for( int i = 0; i < nPairs; ++i )
                {
                    aKernPair.mnChar1 = NEXT_UShort( pBuffer );
                    aKernPair.mnChar2 = NEXT_UShort( pBuffer );
                    /*long nUnscaledKern=*/ NEXT_Short( pBuffer );
                    aKernGlyphVector.push_back( aKernPair );
                }
            }
            break;

            case 2: // version 0, kerning format 2
            {
                const FT_Byte* pSubTable = pBuffer;
                /*USHORT nRowWidth  =*/ NEXT_UShort( pBuffer );
                USHORT nOfsLeft     = NEXT_UShort( pBuffer );
                USHORT nOfsRight    = NEXT_UShort( pBuffer );
                USHORT nOfsArray    = NEXT_UShort( pBuffer );

                const FT_Byte* pTmp = pSubTable + nOfsLeft;
                USHORT nFirstLeft   = NEXT_UShort( pTmp );
                USHORT nLastLeft    = NEXT_UShort( pTmp ) + nFirstLeft - 1;

                pTmp = pSubTable + nOfsRight;
                USHORT nFirstRight  = NEXT_UShort( pTmp );
                USHORT nLastRight   = NEXT_UShort( pTmp ) + nFirstRight - 1;

                ULONG nPairs = (ULONG)(nLastLeft - nFirstLeft + 1) * (nLastRight - nFirstRight + 1);
                aKernGlyphVector.reserve( aKernGlyphVector.size() + nPairs );

                pTmp = pSubTable + nOfsArray;
                for( int nLeft = nFirstLeft; nLeft < nLastLeft; ++nLeft )
                {
                    aKernPair.mnChar1 = nLeft;
                    for( int nRight = 0; nRight < nLastRight; ++nRight )
                    {
                        if( NEXT_Short( pTmp ) != 0 )
                        {
                            aKernPair.mnChar2 = nRight;
                            aKernGlyphVector.push_back( aKernPair );
                        }
                    }
                }
            }
            break;
        }
    }

    delete[] pKern;

    // now create VCL's ImplKernPairData[] format for all glyph pairs
    ULONG nKernCount = aKernGlyphVector.size();
    if( nKernCount )
    {
        // prepare glyphindex to character mapping
        // TODO: this is needed to support VCL's existing kerning infrastructure,
        // eliminate it up by redesigning kerning infrastructure to work with glyph indizes
        typedef std::hash_multimap<USHORT,sal_Unicode> Cmap;
        Cmap aCmap;
        for( sal_Unicode aChar = 0x0001; aChar < 0xFFFE; ++aChar )
        {
            USHORT nGlyphIndex = GetGlyphIndex( aChar );
            if( nGlyphIndex )
                aCmap.insert( Cmap::value_type( nGlyphIndex, aChar ) );
        }

        // translate both glyph indizes in kerning pairs to characters
        // problem is that these are 1:n mappings...
        KernVector aKernCharVector;
        aKernCharVector.reserve( nKernCount );
        KernVector::iterator it;
        for( it = aKernGlyphVector.begin(); it != aKernGlyphVector.end(); ++it )
        {
            FT_Vector aKernVal;
            FT_Error rcFT = FT_Get_Kerning( maFaceFT, it->mnChar1, it->mnChar2,
                ft_kerning_default, &aKernVal );
            aKernPair.mnKern = aKernVal.x;
            if( (aKernPair.mnKern == 0) || (rcFT != FT_Err_Ok) )

                continue;

            typedef std::pair<Cmap::iterator,Cmap::iterator> CPair;
            const CPair p1 = aCmap.equal_range( it->mnChar1 );
            const CPair p2 = aCmap.equal_range( it->mnChar2 );
            for( Cmap::const_iterator i1 = p1.first; i1 != p1.second; ++i1 )
            {
                aKernPair.mnChar1 = (*i1).second;
                for( Cmap::const_iterator i2 = p2.first; i2 != p2.second; ++i2 )
                {
                    aKernPair.mnChar2 = (*i2).second;
                    aKernCharVector.push_back( aKernPair );
                }
            }
        }

        // now move the resulting vector into VCL's ImplKernPairData[] format
        nKernCount = aKernCharVector.size();
        ImplKernPairData* pTo = new ImplKernPairData[ nKernCount ];
        *ppKernPairs = pTo;
        for( it = aKernCharVector.begin(); it != aKernCharVector.end(); ++it, ++pTo )
        {
            pTo->mnChar1 = it->mnChar1;
            pTo->mnChar2 = it->mnChar2;
            pTo->mnKern = it->mnKern;
        }
    }

    return nKernCount;
}

// -----------------------------------------------------------------------
// outline stuff
// -----------------------------------------------------------------------

class PolyArgs
{
public:
                PolyArgs( PolyPolygon& rPolyPoly, USHORT nMaxPoints, long nHeight );
                ~PolyArgs();

    void        AddPoint( long nX, long nY, PolyFlags);
    void        ClosePolygon();

    long        GetPosX() const { return maPosition.x;}
    long        GetPosY() const { return maPosition.y;}

private:
    PolyPolygon& mrPolyPoly;

    Point*      mpPointAry;
    BYTE*       mpFlagAry;

    FT_Vector   maPosition;
    USHORT      mnMaxPoints;
    USHORT      mnPoints;
    USHORT      mnPoly;
    long        mnHeight;
    bool        bHasOffline;
};

// -----------------------------------------------------------------------

PolyArgs::PolyArgs( PolyPolygon& rPolyPoly, USHORT nMaxPoints, long nHeight )
:   mrPolyPoly(rPolyPoly),
    mnMaxPoints(nMaxPoints),
    mnPoints(0),
    mnPoly(0),
    mnHeight(nHeight),
    bHasOffline(false)
{
    mpPointAry  = new Point [ mnMaxPoints ];
    mpFlagAry   = new BYTE  [ mnMaxPoints ];

    mrPolyPoly.Clear();
}

// -----------------------------------------------------------------------

PolyArgs::~PolyArgs()
{
    delete[] mpFlagAry;
    delete[] mpPointAry;
}

// -----------------------------------------------------------------------

void PolyArgs::AddPoint( long nX, long nY, PolyFlags aFlag )
{
    DBG_ASSERT( (mnPoints < mnMaxPoints), "FTGlyphOutline: AddPoint overflow!" );

    maPosition.x = nX;
    maPosition.y = nY;
    mpPointAry[ mnPoints ] = Point( (nX + 32) >> 6, mnHeight - ((nY + 32) >> 6) );
    mpFlagAry[ mnPoints++ ]= aFlag;
    bHasOffline |= (aFlag != POLY_NORMAL);
}

// -----------------------------------------------------------------------

void PolyArgs::ClosePolygon()
{
    if( !mnPoly++ )
        return;

    // freetype seems to always close the polygon with an ON_CURVE point
    // PolyPoly wants to close the polygon itself => remove last point
    DBG_ASSERT( (mnPoints >= 2), "FTGlyphOutline: PolyFinishNum failed!" );
    --mnPoints;
    DBG_ASSERT( (mpPointAry[0]==mpPointAry[mnPoints]), "FTGlyphOutline: PolyFinishEq failed!" );
    DBG_ASSERT( (mpFlagAry[0]==POLY_NORMAL), "FTGlyphOutline: PolyFinishFE failed!" );
    DBG_ASSERT( (mpFlagAry[mnPoints]==POLY_NORMAL), "FTGlyphOutline: PolyFinishFS failed!" );

    Polygon aPoly( mnPoints, mpPointAry, (bHasOffline ? mpFlagAry : NULL) );
    mrPolyPoly.Insert( aPoly );

    mnPoints = 0;
    bHasOffline = false;
}

// -----------------------------------------------------------------------

// TODO: wait till all compilers accept that calling conventions
// for functions are the same independent of implementation constness,
// then uncomment the const-tokens in the function interfaces below
static int FT_move_to( FT_Vector* /*const*/ p0, void* vpPolyArgs )
{
    PolyArgs& rA = *reinterpret_cast<PolyArgs*>(vpPolyArgs);

    // move_to implies a new polygon => finish old polygon first
    rA.ClosePolygon();

    rA.AddPoint( p0->x, p0->y, POLY_NORMAL );
    return 0;
}

static int FT_line_to( FT_Vector* /*const*/ p1, void* vpPolyArgs )
{
    PolyArgs& rA = *reinterpret_cast<PolyArgs*>(vpPolyArgs);
    rA.AddPoint( p1->x, p1->y, POLY_NORMAL );
    return 0;
}

static int FT_conic_to( FT_Vector* /*const*/ p1, FT_Vector* /*const*/ p2, void* vpPolyArgs )
{
    PolyArgs& rA = *reinterpret_cast<PolyArgs*>(vpPolyArgs);

    // VCL's Polygon only provides cubic beziers
    const long nX1 = (2 * rA.GetPosX() + 4 * p1->x + 3) / 6;
    const long nY1 = (2 * rA.GetPosY() + 4 * p1->y + 3) / 6;
    rA.AddPoint( nX1, nY1, POLY_CONTROL );

    const long nX2 = (2 * p2->x + 4 * p1->x + 3) / 6;
    const long nY2 = (2 * p2->y + 4 * p1->y + 3) / 6;
    rA.AddPoint( nX2, nY2, POLY_CONTROL );

    rA.AddPoint( p2->x, p2->y, POLY_NORMAL );
    return 0;
}

static int FT_cubic_to( FT_Vector* /*const*/ p1, FT_Vector* /*const*/ p2, FT_Vector* /*const*/ p3, void* vpPolyArgs )
{
    PolyArgs* const pA = reinterpret_cast<PolyArgs*>(vpPolyArgs);
    pA->AddPoint( p1->x, p1->y, POLY_CONTROL );
    pA->AddPoint( p2->x, p2->y, POLY_CONTROL );
    pA->AddPoint( p3->x, p3->y, POLY_NORMAL );
    return 0;
}

// -----------------------------------------------------------------------

bool FreetypeServerFont::GetGlyphOutline( int nGlyphIndex, PolyPolygon& rPolyPoly ) const
{
    FT_Int nLoadFlags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
    FT_Error rc = FT_Load_Glyph( maFaceFT, nGlyphIndex, nLoadFlags );

    FT_Glyph aGlyphFT;
    rc = FT_Get_Glyph( maFaceFT->glyph, &aGlyphFT );

    if( aGlyphFT->format != ft_glyph_format_outline )
        return false;

    FT_Outline& rOutline = reinterpret_cast<FT_OutlineGlyphRec*>( aGlyphFT ) -> outline;
    const long nMaxPoints = rOutline.n_points * 2;
    const long nHeight = GetFontSelData().mnHeight;
    PolyArgs aPolyArg( rPolyPoly, nMaxPoints, nHeight );

    FT_Outline_Funcs aFuncs;
    aFuncs.move_to  = &FT_move_to;
    aFuncs.line_to  = &FT_line_to;
    aFuncs.conic_to = &FT_conic_to;
    aFuncs.cubic_to = &FT_cubic_to;
    aFuncs.shift    = 0;
    aFuncs.delta    = 0;
    rc = FT_Outline_Decompose( &rOutline, &aFuncs, (void*)&aPolyArg );
    aPolyArg.ClosePolygon();    // close last polygon

    FT_Done_Glyph( aGlyphFT );

    return true;
}

// -----------------------------------------------------------------------

bool FreetypeServerFont::ApplyGSUB( const ImplFontSelectData& rFSD )
{
    SFNT_Interface* pSFNT = (SFNT_Interface*) FT_Get_Module_Interface( aLibFT, "sfnt" );
    if( !pSFNT )
        return false;

    // get length of GSUB table if it exists
    FT_ULong nLength = 0;
    FT_Error rcFT = pSFNT->load_any( (TT_Face)maFaceFT, TTAG_GSUB, 0, NULL, &nLength );
    if( rcFT != FT_Err_Ok )
        return false;

#define MKTAG(s) ((((((s[0]<<8)+s[1])<<8)+s[2])<<8)+s[3])

    typedef std::vector<ULONG> ReqFeatureTagList;
    ReqFeatureTagList aReqFeatureTagList;
    if( rFSD.mbVertical )
        aReqFeatureTagList.push_back( MKTAG("vert") );
    ULONG nRequestedScript = 0;     //MKTAG("hani");//### TODO: where to get script?
    ULONG nRequestedLangsys = 0;    //MKTAG("ZHT"); //### TODO: where to get langsys?
    // TODO: request more features depending on script and language system

    if( aReqFeatureTagList.size() == 0) // nothing to do
        return true;

    // load GSUB table into memory
    FT_Byte* const pGsubBase = new FT_Byte[ nLength ];
    rcFT = pSFNT->load_any( (TT_Face)maFaceFT, TTAG_GSUB, 0, pGsubBase, &nLength );

    // parse GSUB header
    const FT_Byte* pGsubHeader = pGsubBase;
    const ULONG nVersion            = NEXT_Long( pGsubHeader );
    const USHORT nOfsScriptList     = NEXT_UShort( pGsubHeader );
    const USHORT nOfsFeatureTable   = NEXT_UShort( pGsubHeader );
    const USHORT nOfsLookupList     = NEXT_UShort( pGsubHeader );

    typedef std::vector<USHORT> UshortList;
    UshortList aFeatureIndexList;
    UshortList aFeatureOffsetList;

#ifdef DEBUG
    fprintf(stderr,"***GSUB\n");
#endif
    // parse Script Table
    const FT_Byte* pScriptHeader = pGsubBase + nOfsScriptList;
    const USHORT nCntScript = NEXT_UShort( pScriptHeader );
    for( USHORT nScriptIndex = 0; nScriptIndex < nCntScript; ++nScriptIndex )
    {
        const ULONG nTag            = NEXT_Long( pScriptHeader ); // e.g. hani/arab/kana/hang
        const USHORT nOfsScriptTable= NEXT_UShort( pScriptHeader );
#ifdef DEBUG
        fprintf(stderr,"Script[%d] tag=%.4s, ofs=%d\n", nScriptIndex, &nTag, nOfsScriptTable);
#endif
        if( (nTag != nRequestedScript) && (nRequestedScript != 0) )
            continue;

        const FT_Byte* pScriptTable     = pGsubBase + nOfsScriptList + nOfsScriptTable;
        const USHORT nDefaultLangsysOfs = NEXT_UShort( pScriptTable );
        const USHORT nCntLangSystem     = NEXT_UShort( pScriptTable );
        USHORT nLangsysOffset = 0;
#ifdef DEBUG
        fprintf(stderr,"ofs=%d => def=%d, cnt=%d\n", nOfsScriptTable, nDefaultLangsysOfs, nCntLangSystem );
#endif
        for( USHORT nLangsysIndex = 0; nLangsysIndex < nCntLangSystem; ++nLangsysIndex )
        {
            const ULONG nTag    = NEXT_Long( pScriptTable );    // e.g. KOR/ZHS/ZHT/JAN
            const USHORT nOffset= NEXT_UShort( pScriptTable );
#ifdef DEBUG
            fprintf(stderr,"Langsys[%d] tag=%.4s, ofs=%d\n", nLangsysIndex, &nTag, nOffset);
#endif
            if( (nTag != nRequestedLangsys) && (nRequestedLangsys != 0) )
                continue;
            nLangsysOffset = nOffset;
            break;
        }

        if( (nDefaultLangsysOfs != 0) && (nDefaultLangsysOfs != nLangsysOffset) )
        {
            const FT_Byte* pLangSys = pGsubBase + nOfsScriptList + nOfsScriptTable + nDefaultLangsysOfs;
            const USHORT nLookupOrder   = NEXT_UShort( pLangSys );
            const USHORT nReqFeatureIdx = NEXT_UShort( pLangSys );
            const USHORT nCntFeature    = NEXT_UShort( pLangSys );
            aFeatureIndexList.push_back( nReqFeatureIdx );
#ifdef DEBUG
            fprintf(stderr,"Feature defreq=%d, ofs=%d cnt=%d\n", nReqFeatureIdx, nDefaultLangsysOfs, nCntFeature );
#endif
            for( USHORT i = 0; i < nCntFeature; ++i )
            {
                const USHORT nFeatureIndex = NEXT_UShort( pLangSys );
#ifdef DEBUG
                fprintf(stderr,"Feature defopt=%d\n", nFeatureIndex);
#endif
                aFeatureIndexList.push_back( nFeatureIndex );
            }
        }

        if( nLangsysOffset != 0 )
        {
            const FT_Byte* pLangSys = pGsubBase + nOfsScriptList + nOfsScriptTable + nLangsysOffset;
            const USHORT nLookupOrder   = NEXT_UShort( pLangSys );
            const USHORT nReqFeatureIdx = NEXT_UShort( pLangSys );
            const USHORT nCntFeature    = NEXT_UShort( pLangSys );
            aFeatureIndexList.push_back( nReqFeatureIdx );
#ifdef DEBUG
            fprintf(stderr,"Feature req=%d, ofs=%d cnt=%d\n", nReqFeatureIdx, nLangsysOffset, nCntFeature );
#endif
            for( USHORT i = 0; i < nCntFeature; ++i )
            {
                const USHORT nFeatureIndex = NEXT_UShort( pLangSys );
#ifdef DEBUG
                fprintf(stderr,"Feature opt=%d\n", nFeatureIndex);
#endif
                aFeatureIndexList.push_back( nFeatureIndex );
            }
        }
    }

    if( !aFeatureIndexList.size() )
        return true;

    UshortList aLookupIndexList;
    UshortList aLookupOffsetList;

#ifdef DEBUG
    fprintf(stderr,"*parsing Feature Table ofs=%d\n", nOfsFeatureTable);
#endif
    // parse Feature Table
    const FT_Byte* pFeatureHeader = pGsubBase + nOfsFeatureTable;
    const USHORT nCntFeature = NEXT_UShort( pFeatureHeader );
#ifdef DEBUG
    fprintf(stderr,"nFeatureCount=%d\n", nCntFeature);
#endif
    for( USHORT nFeatureIndex = 0; nFeatureIndex < nCntFeature; ++nFeatureIndex )
    {
        const ULONG nTag    = NEXT_Long( pFeatureHeader ); // e.g. locl/vert/trad/smpl/liga/fina/...
        const USHORT nOffset= NEXT_UShort( pFeatureHeader );

#ifdef DEBUG
        fprintf(stderr,"Feature[%d] tag=%.4s, ofs=%d\n", nFeatureIndex, &nTag, nOffset );
#endif

        // feature (required && (requested || available))?
        if( (aFeatureIndexList[0] != nFeatureIndex)
        &&  (!std::count( aReqFeatureTagList.begin(), aReqFeatureTagList.end(), nTag))
        ||  (!std::count( aFeatureIndexList.begin(), aFeatureIndexList.end(), nFeatureIndex) ) )
            continue;

        const FT_Byte* pFeatureTable = pGsubBase + nOfsFeatureTable + nOffset;
        const USHORT nCntLookups = NEXT_UShort( pFeatureTable );
#ifdef DEBUG
        fprintf(stderr,"=> FeatLookup cnt=%d\n", nCntLookups);
#endif
        for( USHORT i = 0; i < nCntLookups; ++i )
        {
            const USHORT nLookupIndex = NEXT_UShort( pFeatureTable );
#ifdef DEBUG
            fprintf(stderr,"=> FeatLookup[%d][%d] idx=%d\n", nFeatureIndex, i, nLookupIndex);
#endif
            aLookupIndexList.push_back( nLookupIndex );
        }
        if( nCntLookups == 0 ) //### hack needed by Mincho/Gothic/Mingliu/Simsun/...
            aLookupIndexList.push_back( 0 );
    }

    // parse Lookup List
#ifdef DEBUG
    fprintf(stderr,"*parsing Lookup Table ofs=%d\n", nOfsLookupList);
#endif
    const FT_Byte* pLookupHeader = pGsubBase + nOfsLookupList;
    const USHORT nCntLookupTable = NEXT_UShort( pLookupHeader );
    for( USHORT nLookupIdx = 0; nLookupIdx < nCntLookupTable; ++nLookupIdx )
    {
        const USHORT nOffset = NEXT_UShort( pLookupHeader );
#ifdef DEBUG
        fprintf(stderr,"=> Lookup[%d] ofs=%d\n", nLookupIdx, nOffset);
#endif
        if( std::count( aLookupIndexList.begin(), aLookupIndexList.end(), nLookupIdx ) )
            aLookupOffsetList.push_back( nOffset );
    }

    UshortList::const_iterator it = aLookupOffsetList.begin();
    for(; it != aLookupOffsetList.end(); ++it )
    {
        const USHORT nOfsLookupTable = *it;
        const FT_Byte* pLookupTable = pGsubBase + nOfsLookupList + nOfsLookupTable;
        const USHORT eLookupType        = NEXT_UShort( pLookupTable );
        const USHORT eLookupFlag        = NEXT_UShort( pLookupTable );
        const USHORT nCntLookupSubtable = NEXT_UShort( pLookupTable );
#ifdef DEBUG
        fprintf(stderr,"Lookup.ofs=%d type=%d flag=0x%02X, cnt=%d\n", nOfsLookupTable, eLookupType, eLookupFlag, nCntLookupSubtable );
#endif

        // TODO: switch( eLookupType )
        if( eLookupType != 1 )  // TODO: once we go beyond SingleSubst
            continue;

        for( USHORT nSubTableIdx = 0; nSubTableIdx < nCntLookupSubtable; ++nSubTableIdx )
        {
            const USHORT nOfsSubLookupTable = NEXT_UShort( pLookupTable );
            const FT_Byte* pSubLookup = pGsubBase + nOfsLookupList + nOfsLookupTable + nOfsSubLookupTable;

            const USHORT nFmtSubstitution   = NEXT_UShort( pSubLookup );
            const USHORT nOfsCoverage       = NEXT_UShort( pSubLookup );

            typedef std::pair<USHORT,USHORT> GlyphSubst;
            typedef std::vector<GlyphSubst> SubstVector;
            SubstVector aSubstVector;

            const FT_Byte* pCoverage    = pGsubBase + nOfsLookupList + nOfsLookupTable + nOfsSubLookupTable + nOfsCoverage;
            const USHORT nFmtCoverage   = NEXT_UShort( pCoverage );
            switch( nFmtCoverage )
            {
                case 1:         // Coverage Format 1
                    {
                        const USHORT nCntGlyph = NEXT_UShort( pCoverage );
                        aSubstVector.reserve( nCntGlyph );
#ifdef DEBUG
                        fprintf(stderr,"cov1.count = %d\n", nCntGlyph );
#endif
                        for( USHORT i = 0; i < nCntGlyph; ++i )
                        {
                            const USHORT nGlyphId = NEXT_UShort( pCoverage );
                            aSubstVector.push_back( GlyphSubst( nGlyphId, 0 ) );
                        }
                    }
                    break;

                case 2:         // Coverage Format 2
                    {
                        const USHORT nCntRange = NEXT_UShort( pCoverage );
                        for( int i = nCntRange; --i >= 0; )
                        {
                            const USHORT nGlyph0 = NEXT_UShort( pCoverage );
                            const USHORT nGlyph1 = NEXT_UShort( pCoverage );
                            const USHORT nCovIdx = NEXT_UShort( pCoverage );
                            for( USHORT j = nGlyph0; j <= nGlyph1; ++j )
                                aSubstVector.push_back( GlyphSubst( j + nCovIdx, 0 ) );
#ifdef DEBUG
                            fprintf(stderr,"cov2 0x%04X - 0x%04X\n", nGlyph0, nGlyph1 );
#endif
                        }
                    }
                    break;
            }

            SubstVector::iterator it( aSubstVector.begin() );

            switch( nFmtSubstitution )
            {
                case 1:     // Single Substitution Format 1
                    {
                        const USHORT nDeltaGlyphId = NEXT_UShort( pSubLookup );
                        for(; it != aSubstVector.end(); ++it )
                            (*it).second = (*it).first + nDeltaGlyphId;
#ifdef DEBUG
                        fprintf(stderr,"ssub1 delta = 0x%04X\n", nDeltaGlyphId );
#endif
                    }
                    break;

                case 2:     // Single Substitution Format 2
                    {
                        const USHORT nCntGlyph = NEXT_UShort( pSubLookup );
#ifdef DEBUG
                        fprintf(stderr,"ssub2.count = %d\n", nCntGlyph );
#endif
                        for( int i = nCntGlyph; (it != aSubstVector.end()) && (--i>=0); ++it )
                        {
                            const USHORT nGlyphId = NEXT_UShort( pSubLookup );
                            (*it).second = nGlyphId;
                        }
                    }
                    break;
            }

            DBG_ASSERT( (it == aSubstVector.end()), "lookup<->coverage table mismatch" );
            // now apply the glyph substitutions that have been collected in this subtable
            for( it = aSubstVector.begin(); it != aSubstVector.end(); ++it )
                aGlyphSubstitution[ (*it).first ] =  (*it).second;
#ifdef DEBUG
            fprintf(stderr,"SingleSubst.count=%d, buckets=%d\n", aSubstVector.size(), aGlyphSubstitution.bucket_count() );
//          for( it = aSubstVector.begin(); it != aSubstVector.end(); ++it )
//              fprintf(stderr,"\t0x%04X => 0x%04X\n", (*it).first, (*it).second );
#endif
        }
    }

    delete[] pGsubBase;
    return true;
}

// =======================================================================
