/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: gcach_vdev.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 12:14:20 $
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

#include <gcach_vdev.hxx>

#include <svapp.hxx>
#include <bitmap.hxx>
#include <outfont.hxx>
#include <virdev.hxx>
#include <metric.hxx>

// =======================================================================
// VirtDevServerFont
// =======================================================================

// -----------------------------------------------------------------------

void VirtDevServerFont::AnnounceFonts( ImplDevFontList* pToAdd )
{
    // TODO: get fonts on server but not on client,
    // problem is that currently there is no serverside virtual device...
    VirtualDevice vdev( 1 );
    long nCount = vdev.GetDevFontCount();

    for( int i = 0; i < nCount; ++i )
    {
        const FontInfo aFontInfo = vdev.GetDevFont( i );

        ImplFontData& rData = *new ImplFontData;
        rData.SetSysData( new FontSysData( (void*)SERVERFONT_MAGIC ) );

        rData.maName        = aFontInfo.GetName();
        rData.maStyleName   = aFontInfo.GetStyleName();
        rData.mnWidth       = aFontInfo.GetWidth();
        rData.mnHeight      = aFontInfo.GetHeight();
        rData.meFamily      = aFontInfo.GetFamily();
        rData.meCharSet     = aFontInfo.GetCharSet();
        rData.mePitch       = aFontInfo.GetPitch();
        rData.meWidthType   = aFontInfo.GetWidthType();
        rData.meWeight      = aFontInfo.GetWeight();
        rData.meItalic      = aFontInfo.GetItalic();
        rData.meType        = aFontInfo.GetType();
        rData.meFamily      = aFontInfo.GetFamily();

        rData.mbOrientation = true;         // TODO: where to get this info?
        rData.mbDevice      = false;
        rData.mnQuality     = 0;            // prefer client-side fonts if available

        pToAdd->Add( &rData );
    }
}

// -----------------------------------------------------------------------

void VirtDevServerFont::ClearFontList()
{
    // TODO
}

// -----------------------------------------------------------------------

VirtDevServerFont* VirtDevServerFont::CreateFont( const ImplFontSelectData& rFSD )
{
    VirtDevServerFont* pServerFont = NULL;
    // TODO: search list of VirtDevServerFonts, return NULL if not found
    // pServerFont = new VirtDevServerFont( rFSD );
    return pServerFont;
}

// -----------------------------------------------------------------------

VirtDevServerFont::VirtDevServerFont( const ImplFontSelectData& rFSD )
:   ServerFont( rFSD)
{}

// -----------------------------------------------------------------------

void VirtDevServerFont::FetchFontMetric( ImplFontMetricData& rTo, long& rFactor ) const
{
    const ImplFontSelectData& aFSD = GetFontSelData();

    Font aFont;
    aFont.SetName       ( aFSD.maName );
    aFont.SetStyleName  ( aFSD.maStyleName );
    aFont.SetHeight     ( aFSD.mnHeight );
    aFont.SetWidth      ( aFSD.mnWidth );
    aFont.SetOrientation( aFSD.mnOrientation );
    aFont.SetVertical   ( GetFontSelData().mbVertical );

    VirtualDevice vdev( 1 );
    FontMetric aMetric( vdev.GetFontMetric( aFont ) );

    rFactor = 0x100;

    rTo.mnAscent        = aMetric.GetAscent();
    rTo.mnDescent       = aMetric.GetDescent();
    rTo.mnIntLeading    = aMetric.GetIntLeading();
    rTo.mnExtLeading    = aMetric.GetExtLeading();
    rTo.mnSlant     = aMetric.GetSlant();
    rTo.meType      = aMetric.GetType();
    rTo.mnFirstChar     = 0x0020;   // TODO: where to get this info?
    rTo.mnLastChar      = 0xFFFE;   // TODO: where to get this info?

    rTo.mnWidth         = aFSD.mnWidth;
    rTo.maName          = aFSD.maName;
    rTo.maStyleName     = aFSD.maStyleName;
    rTo.mnOrientation   = aFSD.mnOrientation;
    rTo.meFamily        = aFSD.meFamily;
    rTo.meCharSet       = aFSD.meCharSet;
    rTo.meWeight        = aFSD.meWeight;
    rTo.meItalic        = aFSD.meItalic;
    rTo.mePitch         = aFSD.mePitch;
    rTo.mbDevice        = FALSE;
}

// -----------------------------------------------------------------------

int VirtDevServerFont::GetGlyphIndex( sal_Unicode aChar ) const
{
    return aChar;
}

// -----------------------------------------------------------------------

void VirtDevServerFont::InitGlyphData( int nGlyphIndex, GlyphData& rGD ) const
{
    Font aFont;
    aFont.SetName       ( GetFontSelData().maName );
    aFont.SetStyleName  ( GetFontSelData().maStyleName );
    aFont.SetHeight     ( GetFontSelData().mnHeight );
    aFont.SetWidth      ( GetFontSelData().mnWidth );
    aFont.SetOrientation( GetFontSelData().mnOrientation );
    aFont.SetVertical   ( GetFontSelData().mbVertical );

    VirtualDevice vdev( 1 );
    vdev.SetFont( aFont );

    // get glyph metrics
    sal_Int32 nCharWidth = 10;
// TODO:    vdev.GetCharWidth( nGlyphIndex, nGlyphIndex, &nCharWidth );
    rGD.SetCharWidth( nCharWidth );

    sal_Unicode aChar = nGlyphIndex;
    String aGlyphStr( &aChar, 1 );
    Rectangle aRect;
    if( vdev.GetTextBoundRect( aRect, aGlyphStr, 0, 1 ) )
    {
        rGD.SetOffset( aRect.Top(), aRect.Left() );
        rGD.SetDelta( vdev.GetTextWidth( nGlyphIndex ), 0 );
        rGD.SetSize( aRect.GetSize() );
    }
}

// -----------------------------------------------------------------------

bool VirtDevServerFont::GetAntialiasAdvice( void ) const
{
    return false;
}

// -----------------------------------------------------------------------

bool VirtDevServerFont::GetGlyphBitmap1( int nGlyphIndex, RawBitmap& ) const
{
    /*
    sal_Unicode aChar = nGlyphIndex;
    String aGlyphStr( &aChar, 1 );

    // draw bitmap
    vdev.SetOutputSizePixel( aSize, TRUE );
    vdev.DrawText( Point(0,0)-rGD.GetMetric().GetOffset(), aGlyphStr );

    // create new glyph item

    const Bitmap& rBitmap = vdev.GetBitmap( Point(0,0), aSize );
    rGD.SetBitmap( new Bitmap( rBitmap ) );
    return true;
    */
    return false;
}

// -----------------------------------------------------------------------

bool     VirtDevServerFont::GetGlyphBitmap8( int nGlyphIndex, RawBitmap& ) const
{
    return false;
}

// -----------------------------------------------------------------------

int VirtDevServerFont::GetGlyphKernValue( int, int ) const
{
    return 0;
}

// -----------------------------------------------------------------------

ULONG VirtDevServerFont::GetKernPairs( ImplKernPairData** ppImplKernPairs ) const
{
    Font aFont;
    aFont.SetName       ( GetFontSelData().maName );
    aFont.SetStyleName  ( GetFontSelData().maStyleName );
    aFont.SetHeight     ( GetFontSelData().mnHeight );
    aFont.SetWidth      ( GetFontSelData().mnWidth );
    aFont.SetOrientation( GetFontSelData().mnOrientation );
    aFont.SetVertical   ( GetFontSelData().mbVertical );

    VirtualDevice vdev( 1 );
    vdev.SetFont( aFont );

    ULONG nPairs = vdev.GetKerningPairCount();
    if( nPairs > 0 )
    {
        KerningPair* const pKernPairs = new KerningPair[ nPairs ];
        vdev.GetKerningPairs( nPairs, pKernPairs );

        *ppImplKernPairs = new ImplKernPairData[ nPairs ];
        ImplKernPairData* pTo = *ppImplKernPairs;
        KerningPair* pFrom = pKernPairs;
        for ( ULONG n = 0; n < nPairs; n++ )
        {
            pTo->mnChar1    = pFrom->nChar1;
            pTo->mnChar2    = pFrom->nChar2;
            pTo->mnKern     = pFrom->nKern;
            ++pFrom;
            ++pTo;
        }

        delete[] pKernPairs;
    }

    return nPairs;
}

// -----------------------------------------------------------------------

bool VirtDevServerFont::GetGlyphOutline( int nGlyphIndex, PolyPolygon& rPolyPoly ) const
{
    return false;
    /*
    Font aFont;
    aFont.SetName       ( GetFontSelData().maName );
    aFont.SetStyleName  ( GetFontSelData().maStyleName );
    aFont.SetHeight     ( GetFontSelData().mnHeight );
    aFont.SetWidth      ( GetFontSelData().mnWidth );
    aFont.SetOrientation( GetFontSelData().mnOrientation );
    aFont.SetVertical   ( GetFontSelData().mbVertical );

    VirtualDevice vdev( 1 );
    vdev.SetFont( aFont );

    const bool bOptimize = true;

    sal_Unicode aChar = nGlyphIndex;
    String aGlyphStr( &aChar, 1 );
    return vdev.GetTextOutline( rPolyPoly, aGlyphStr, 0, 1, bOptimize );
    */
}

// =======================================================================
