/*************************************************************************
*
*  OpenOffice.org - a multi-platform office productivity suite
*
*  $RCSfile: salatslayout.cxx,v $
*
*  $Revision: 1.2 $
*
*  last change: $Author: rt $ $Date: 2007-07-05 08:24:16 $
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

#ifndef _SV_SALGDI_HXX
#include <salgdi.hxx>
#endif
#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALGDI_H
#include <salgdi.h>
#endif
#ifndef _SV_SALLAYOUT_HXX
#include <sallayout.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include <premac.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <postmac.h>

// =======================================================================

class ATSLayout : public SalLayout
{
public:
                    ATSLayout( AquaSalGraphics*, double fFontScale );
    virtual         ~ATSLayout();

    virtual bool    LayoutText( ImplLayoutArgs& );
    virtual void    AdjustLayout( ImplLayoutArgs& );
    virtual void    DrawText( SalGraphics& ) const;

    virtual int     GetNextGlyphs( int nLen, long* pGlyphs, Point& rPos, int&,
                        long* pGlyphAdvances, int* pCharIndexes ) const;

    virtual long    GetTextWidth() const;
    virtual long    FillDXArray( long* pDXArray ) const;
    virtual int     GetTextBreak( long nMaxWidth, long nCharExtra, int nFactor ) const;
    virtual void    GetCaretPositions( int nArraySize, long* pCaretXArray ) const;
    virtual bool    GetGlyphOutlines( SalGraphics&, PolyPolyVector& ) const;
    virtual bool    GetBoundRect( SalGraphics&, Rectangle& ) const;

    // for glyph+font+script fallback
    virtual void    InitFont();
    virtual void    MoveGlyph( int nStart, long nNewXPos );
    virtual void    DropGlyph( int nStart );
    virtual void    Simplify( bool bIsBase );

private:
    AquaSalGraphics*    mpGraphics;
    double              mfFontScale;        // allows metrics emulation of huge font sizes
    ATSUTextLayout      maATSULayout;
    int                 mnCharCount;        // ==mnEndCharPos-mnMinCharPos

private:
    mutable Fixed       mnCachedWidth;      // cached value of resulting typographical width
    mutable int         mnGlyphCount;       // Glyph count
    mutable Fixed       mnNotdefWidth;
    Fixed               mnBaseAdv;          // x-offset relative to Layout origin

private:
    bool    InitGIA() const;
    bool    GetIdealX() const;
    bool    GetDeltaY() const;

    mutable ATSGlyphRef*        mpGlyphIds;         // Glyphs IDs
    mutable Fixed*              mpCharWidths;       // map rel char pos to charwidth
    mutable int*                mpChars2Glyphs;     // map rel char pos to abs glyph pos
    mutable int*                mpGlyphs2Chars;     // map abs glyph pos to abs char pos
    mutable bool*               mpGlyphRTLFlags;    // BiDi status for glyphs: true=>RTL
    mutable Fixed*              mpGlyphAdvances;    // Contains glyph complete width for each glyph in current layout
    mutable Fixed*              mpGlyphOrigAdvs;    // Saves mpGlyphAdvances
    mutable Fixed*              mpDeltaY;           // Delta Y from baseline
    mutable sal_Unicode*        mpOutGlyphs;

    enum { MARKED_OUTGLYPH=0xFFFE, DROPPED_OUTGLYPH=0xFFFF};
};


// =======================================================================

ATSLayout::ATSLayout( AquaSalGraphics* pGraphics, double fFontScale )
:   mpGraphics( pGraphics ),
    mfFontScale( fFontScale ),
    maATSULayout( NULL ),
    mnCharCount( 0 ),
    mnCachedWidth( 0 ),
    mnGlyphCount( -1 ),
    mnNotdefWidth( 0 ),
    mnBaseAdv( 0 ),
    mpGlyphIds( NULL ),
    mpCharWidths( NULL ),
    mpChars2Glyphs( NULL ),
    mpGlyphs2Chars( NULL ),
    mpGlyphRTLFlags( NULL ),
    mpGlyphAdvances( NULL ),
    mpGlyphOrigAdvs( NULL ),
    mpDeltaY( NULL ),
    mpOutGlyphs( NULL )
{}

// -----------------------------------------------------------------------

ATSLayout::~ATSLayout()
{
    if( mpDeltaY )
        ATSUDirectReleaseLayoutDataArrayPtr( NULL,
            kATSUDirectDataBaselineDeltaFixedArray, (void**)&mpDeltaY );

    if ( maATSULayout )
        ATSUDisposeTextLayout( maATSULayout );

    if(mpGlyphRTLFlags)
        delete[] mpGlyphRTLFlags;
    delete[] mpGlyphs2Chars;
    delete[] mpChars2Glyphs;
    if( mpCharWidths != mpGlyphAdvances )
        delete[] mpCharWidths;
    delete[] mpGlyphOrigAdvs;
    delete[] mpGlyphAdvances;
    delete[] mpOutGlyphs;
    delete[] mpGlyphIds;
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::LayoutText : Manage text layouting
 *
 * @param rArgs: contains array of char to be layouted, starting and ending position of the text to layout
 *
 * Manage text layouting : choose glyph to represent characters using the style maATSUStyle
 *
 * @return : true if everything is ok
**/
bool ATSLayout::LayoutText( ImplLayoutArgs& rArgs )
{
    if( maATSULayout )
        ATSUDisposeTextLayout( maATSULayout );

    maATSULayout = NULL;

    // Layout text
    // set up our locals, verify parameters...
    DBG_ASSERT( (rArgs.mpStr!=NULL), "ATSLayout::LayoutText() with rArgs.mpStr==NULL !!!");
    DBG_ASSERT( (mpGraphics->maATSUStyle!=NULL), "ATSLayout::LayoutText() with maATSUStyle==NULL !!!");

    SalLayout::AdjustLayout( rArgs );
    mnCharCount = mnEndCharPos - mnMinCharPos;

    // Workaround a bug in ATSUI with empty string
    if( mnCharCount<=0 )
        return false;

#if (OSL_DEBUG_LEVEL > 3)
    Fixed fFontSize = 0;
    ByteCount nDummy;
    ATSUGetAttribute( mpGraphics->maATSUStyle, kATSUSizeTag, sizeof(fFontSize), &fFontSize, &nDummy);
    String aUniName( &rArgs.mpStr[rArgs.mnMinCharPos], mnCharCount );
    ByteString aCName( aUniName, RTL_TEXTENCODING_UTF8 );
    AquaLog( "ATSLayout( \"%s\" %d..%d of %d) with h=%4.1f\n",
        aCName.GetBuffer(),rArgs.mnMinCharPos,rArgs.mnEndCharPos,rArgs.mnLength,Fix2X(fFontSize) );
#endif

    // create the ATSUI layout
    UniCharCount nRunLengths[1] = { mnCharCount };
    const int nRunCount = sizeof(nRunLengths)/sizeof(*nRunLengths);
    OSStatus eStatus = ATSUCreateTextLayoutWithTextPtr( rArgs.mpStr,
        rArgs.mnMinCharPos, mnCharCount, rArgs.mnLength,
        nRunCount, &nRunLengths[0], &mpGraphics->maATSUStyle,
        &maATSULayout);

    DBG_ASSERT( (eStatus==noErr), "ATSUCreateTextLayoutWithTextPtr failed\n");
    if( eStatus != noErr )
        return false;

    return true;
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::AdjustLayout : Adjust layout style
 *
 * @param rArgs: contains attributes relevant to do a text specific layout
 *
 * Adjust text layouting : move glyphs to match the requested logical widths
 *
 * @return : none
**/
void ATSLayout::AdjustLayout( ImplLayoutArgs& rArgs )
{
    int nPixelWidth = rArgs.mnLayoutWidth;
    if( !nPixelWidth && rArgs.mpDXArray ) {
        // for now we are only interested in the layout width
        // TODO: account for individual logical widths
        nPixelWidth = rArgs.mpDXArray[ mnCharCount - 1 ];

        // workaround for ATSUI not using trailing spaces for justification
        int i = mnCharCount;
        while( (--i > 0) && IsSpacingGlyph( rArgs.mpStr[mnMinCharPos+i]|GF_ISCHAR ) )
            nPixelWidth -= rArgs.mpDXArray[i] - rArgs.mpDXArray[i-1];

        // TODO: use all mpDXArray elements for layouting
    }
    // return early if there is nothing to do
    if( !nPixelWidth )
        return;

    ATSUAttributeTag nTags[2];
    ATSUAttributeValuePtr nVals[2];
    ByteCount nBytes[2];

    Fixed nFixedWidth = FloatToFixed( nPixelWidth / mfFontScale );
    mnCachedWidth = nFixedWidth;
    Fract nFractFactor = kATSUFullJustification;
    nTags[0] = kATSULineWidthTag;
    nVals[0] = &nFixedWidth;
    nBytes[0] = sizeof(Fixed);
    nTags[1] = kATSULineJustificationFactorTag;
    nVals[1] = &nFractFactor;
    nBytes[1] = sizeof(Fract);
    OSStatus eStatus = ATSUSetLayoutControls( maATSULayout, 2, nTags, nBytes, nVals );
    if( eStatus != noErr )
        return;
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::DrawText : Draw text to screen
 *
 * @param rGraphics: device to draw to
 *
 * Draw the layouted text to screen and offscreen copy
 *
 * @return : none
**/
void ATSLayout::DrawText( SalGraphics& rGraphics ) const
{
    // short circuit if there is nothing to do
    if( (mnCharCount <= 0)
    ||  !mpGraphics->CheckContext() )
        return;

    // the view is vertically flipped => flipped glyphs
    // so apply a temporary transformation that it flips back
    // also compensate if the font was size limited
    CGContextSaveGState( mpGraphics->mrContext );
    CGContextScaleCTM( mpGraphics->mrContext, +mfFontScale, -mfFontScale );
    CGContextSetShouldAntialias( mpGraphics->mrContext, !mpGraphics->mbNonAntialiasedText );

    // prepare ATSUI drawing attributes
    static const ItemCount nMaxControls = 8;
    ATSUAttributeTag theTags[ nMaxControls ];
    ByteCount theSizes[ nMaxControls];
    ATSUAttributeValuePtr theValues[ nMaxControls ];
    ItemCount numcontrols = 0;

    // Tell ATSUI to use CoreGraphics
    theTags[numcontrols] = kATSUCGContextTag;
    theSizes[numcontrols] = sizeof( CGContextRef );
    theValues[numcontrols++] = &(mpGraphics->mrContext);

    // Rotate if necessary
    if( mpGraphics->mnATSUIRotation != 0 )
    {
        Fixed theAngle = mpGraphics->mnATSUIRotation;
        theTags[numcontrols] = kATSULineRotationTag;
        theSizes[numcontrols] = sizeof( Fixed );
        theValues[numcontrols++] = &theAngle;
    }

    DBG_ASSERT( (numcontrols <= nMaxControls), "ATSLayout::DrawText() numcontrols overflow" );
    OSStatus theErr = ATSUSetLayoutControls (maATSULayout, numcontrols, theTags, theSizes, theValues);
    DBG_ASSERT( (theErr==noErr), "ATSLayout::DrawText ATSUSetLayoutControls failed!\n" );

    // Draw the text
    const Point aPos = GetDrawPosition( Point(mnBaseAdv, 0) );
    const Fixed nFixedX = FloatToFixed( +aPos.X() / mfFontScale );
    const Fixed nFixedY = FloatToFixed( -aPos.Y() / mfFontScale ); // adjusted for y-mirroring
    theErr = ATSUDrawText( maATSULayout, mnMinCharPos, mnCharCount, nFixedX, nFixedY );
    DBG_ASSERT( (theErr==noErr), "ATSLayout::DrawText ATSUDrawText failed!\n" );

    // request an update of the changed window area
    if( mpGraphics->IsWindowGraphics() )
    {
        Rect drawRect; // rectangle of the changed area
        theErr = ATSUMeasureTextImage( maATSULayout,
            mnMinCharPos, mnCharCount, nFixedX, nFixedY, &drawRect );
        if( theErr == noErr )
            mpGraphics->RefreshRect( drawRect.left, drawRect.bottom,
                drawRect.right - drawRect.left,
                drawRect.bottom - drawRect.top );
    }

    // restore the original graphic context transformations
    CGContextRestoreGState( mpGraphics->mrContext );
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::GetNextGlyphs : Get next glyphs informations
 *
 * @param nLen: max number of char
 * @param pGlyphs: returned array of glyph ids
 * @param rPos: returned x starting position
 * @param nStart: index of the first requested glyph
 * @param pGlyphAdvances: returned array of glyphs advances
 * @param pCharIndexes: returned array of char indexes
 *
 * Returns infos about the next requested glyphs
 *
 * @return : index of next glyph
**/
int ATSLayout::GetNextGlyphs( int nLen, long* pGlyphIDs, Point& rPos, int& nStart,
    long* pGlyphAdvances, int* pCharIndexes ) const
{
#if (OSL_DEBUG_LEVEL > 2)
    AquaLog("-->%s\n",__func__);
#endif

    if( nStart < 0 )                // first glyph requested?
        nStart = 0;

    // get glyph measurements
    InitGIA();
    // some measurements are only needed for multi-glyph results
    if( nLen > 1 )
    {
        GetIdealX();
        GetDeltaY();
    }

    if( nStart >= mnGlyphCount )    // no glyph left?
        return 0;

    // calculate glyph position relative to layout base
    // TODO: avoid for nStart!=0 case by reusing rPos
    Fixed nXOffset = mnBaseAdv;
    for( int i = 0; i < nStart; ++i )
        nXOffset += mpGlyphAdvances[ i ];

    Fixed nYOffset = 0;
    if( mpDeltaY )
        nYOffset = mpDeltaY[ nStart ];

    // calculate absolute position in pixel units
    const Point aRelativePos( Fix2Long(nXOffset*mfFontScale), Fix2Long(nYOffset*mfFontScale) );
    rPos = GetDrawPosition( aRelativePos );

    // update return values {nGlyphIndex,nCharPos,nGlyphAdvance}
    int nCount = 0;
    while( nCount < nLen )
    {
        ++nCount;
        *(pGlyphIDs++) = mpGlyphIds[nStart];
        if( pGlyphAdvances )
            *(pGlyphAdvances++) = static_cast<long>(mfFontScale * FixedToFloat(mpGlyphAdvances[ nStart ]));
        if( pCharIndexes )
        {
            int nCharPos;
            if( mpGlyphs2Chars[nStart] )
                nCharPos = nStart + mnMinCharPos;
            else
                nCharPos = mpGlyphs2Chars[nStart];
            *(pCharIndexes++) = nCharPos;
        }

        // stop at last glyph
        if( ++nStart >= mnGlyphCount )
            break;

        // stop when next the x-position is unexpected
        if( !pGlyphAdvances && mpGlyphOrigAdvs )
            if( mpGlyphAdvances[nStart-1] != mpGlyphOrigAdvs[nStart-1] )
                break;

        // stop when the next y-position is unexpected
        if( mpDeltaY )
            if( mpDeltaY[nStart-1] != mpDeltaY[nStart] )
                break;
    }

    return nCount;
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::GetTextWidth : Get typographic width of layouted text
 *
 * Get typographic bounds of the text
 *
 * @return : text width
**/
long ATSLayout::GetTextWidth() const
{
    if( mnCharCount <= 0 )
        return 0;

    DBG_ASSERT( (maATSULayout != NULL), "ATSLayout::GetTextWidth() with maATSULayout==NULL !\n");

    if( !mnCachedWidth )
    {
        // prepare precise measurements on pixel based or reference-device
        const UInt16 eTypeOfBounds = (mfFontScale==1.0) ? kATSUseDeviceOrigins : kATSUseFractionalOrigins;

        // determine number of needed measurement trapezoids
        ItemCount nMaxBounds = 0;
        OSStatus err = ATSUGetGlyphBounds( maATSULayout, 0, 0, mnMinCharPos, mnCharCount,
            eTypeOfBounds, 0, NULL, &nMaxBounds );
        if( (err != noErr)
        ||  (nMaxBounds <= 0) )
            return 0;

        // get the trapezoids
        typedef std::vector<ATSTrapezoid> TrapezoidVector;
        TrapezoidVector aTrapezoidVector( nMaxBounds );
        ItemCount nBoundsCount = 0;
        err = ATSUGetGlyphBounds( maATSULayout, 0, 0, mnMinCharPos, mnCharCount,
            eTypeOfBounds, nMaxBounds, &aTrapezoidVector[0], &nBoundsCount );
        if( err != noErr )
            return 0;

        DBG_ASSERT( (nBoundsCount <= nMaxBounds), "ATSLayout::GetTextWidth() : too many trapezoids !\n");

        // find the bound extremas
        Fixed nLeftBound = 0;
        Fixed nRightBound = 0;
        for( ItemCount i = 0; i < nBoundsCount; ++i )
        {
            const ATSTrapezoid& rTrap = aTrapezoidVector[i];
            if( (i == 0) || (nLeftBound < rTrap.lowerLeft.x) )
                nLeftBound = rTrap.lowerLeft.x;
            if( (i == 0) || (nRightBound > rTrap.lowerRight.x) )
                nRightBound = rTrap.lowerRight.x;
        }

        // measure the bound extremas
        mnCachedWidth = nRightBound - nLeftBound;
    }

    const long nScaledWidth = static_cast<long>( mfFontScale * FixedToFloat(mnCachedWidth) );
    return nScaledWidth;
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::FillDXArray : Get Char widths
 *
 * @param pDXArray: returned array of DX
 *
 * Fill the pDXArray with horizontal deltas : CharWidths
 *
 * @return : typographical width of the complete text layout
**/
long ATSLayout::FillDXArray( long* pDXArray ) const
{
    // Get infos about glyphs
    InitGIA();

    // get cache the text layout's typographical width
    if( !mnCachedWidth )
    {
        long nWidth = mnBaseAdv;
        for( int i = 0; i < mnCharCount; ++i )
            nWidth += mpCharWidths[ i ];

        mnCachedWidth = nWidth;
    }

    // distribute the widths among the string elements
    if( pDXArray != NULL )
    {
        for( int i = 0; i < mnCharCount; ++i )
            pDXArray[ i ] = static_cast<long>(mfFontScale * FixedToFloat(mpCharWidths[i]));
    }

    const long nScaledWidth = static_cast<long>(mfFontScale * FixedToFloat(mnCachedWidth));
    return nScaledWidth;
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::GetTextBreak : Find line break depending on width
 *
 * @param nMaxWidth : maximal logical text width in subpixel units
 * @param nCharExtra: expanded/condensed spacing in subpixel units
 * @param nFactor:    number of subpixel units per pixel
 *
 * Measure the layouted text to find the typographical line break
 * the result is needed by the language specific line breaking
 *
 * @return : string index corresponding to the suggested line break
**/
int ATSLayout::GetTextBreak( long nMaxWidth, long nCharExtra, int nFactor ) const
{
    const long nPixelWidth = (nMaxWidth - (nCharExtra * mnCharCount)) / nFactor;
    const ATSUTextMeasurement nATSUMaxWidth = FloatToFixed( nPixelWidth / mfFontScale );

    // TODO: massage ATSUBreakLine to like inword breaks:
    //   we prefer BreakInWord instead of ATSUBreakLine trying to be smart
    //   and moving the soft break inbetween words, as the ATSUI API says
    UniCharArrayOffset nBreakPos = mnMinCharPos;
    OSStatus nStatus = ATSUBreakLine( maATSULayout, mnMinCharPos,
        nATSUMaxWidth, false, &nBreakPos );

    if( (nStatus != noErr) && (nStatus != kATSULineBreakInWord) )
    {
        AquaLog("ATSUBreakLine => %d\n", nStatus);
        return( STRING_LEN );
    }

    return( nBreakPos );
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::GetCaretPositions : Find positions of carets
 *
 * @param nMaxIndex position to which we want to find the carets
 *
 * Fill the array of positions of carets (for cursors and selections)
 *
 * @return : none
**/
void ATSLayout::GetCaretPositions( int nMaxIndex, long* pCaretXArray ) const
{
    DBG_ASSERT( ((nMaxIndex>0)&&!(nMaxIndex&1)),
        "ATSLayout::GetCaretPositions() : invalid number of caret pairs requested");

#if 0 // measure caret positions without directly using the ATSUI caret API
    // Get infos about glyphs
    InitGIA();

    Fixed nXPos = mnBaseAdv;
    if( !mpGlyphs2Chars )
    {
        // set the caret positions for a simple layout
        for( int i = 0; i < nMaxIndex; i += 2 )
        {
            const long nLongXPos = static_cast<long>(mfFontScale * FixedToFloat(nXPos));
            pCaretXArray[ i ] = nLongXPos;
            nXPos += mpGlyphAdvances[ i>>1 ];
            pCaretXArray[ i+1 ] = nLongXPos;
        }
    }
    else
    {
        // set the caret positions for a complex layout
        for( int i = 0; i < nMaxIndex; ++i )
            pCaretXArray[ i ] = -1;

        // assign glyph positions to character positions
        for( int i = 0; i < mnGlyphCount; ++i )
        {
            int nCurrIdx = mpGlyphs2Chars[ i ] - mnMinCharPos;
            Fixed nXRight = nXPos + mpCharWidths[ nCurrIdx ];
            const long nLongLeft  = static_cast<long>(mfFontScale * FixedToFloat(nXPos));
            const long nLongRight = static_cast<long>(mfFontScale * FixedToFloat(nXRight));
            nXPos += mpGlyphAdvances[ i ];

            //AquaLog("i=%ld < %ld nCurrIdx=%ld nXPos=%ld nXRight=%ld\n",i,mnGlyphCount,nCurrIdx,nXPos,nXRight);
            nCurrIdx *= 2;
            if( nCurrIdx >= nMaxIndex )
                continue;
            if( !(mpGlyphRTLFlags && mpGlyphRTLFlags[i]) )
            {
                //AquaLog("nCurrIdx=%ld nXPos=%ld nXRight=%ld\n",nCurrIdx,nXPos,nXRight);
                // normal positions for LTR case
                pCaretXArray[ nCurrIdx ]   = nLongLeft;
                pCaretXArray[ nCurrIdx+1 ] = nLongRight;
            }
            else
            {
                //AquaLog("RTL - nCurrIdx=%ld nXPos=%ld nXRight=%ld\n",nCurrIdx,nXPos,nXRight);
                // reverse positions for RTL case
                pCaretXArray[ nCurrIdx ]   = nLongRight;
                pCaretXArray[ nCurrIdx+1 ] = nLongLeft;
            }
        }
    }

    // guess character carets when no directly matching glyph bounds were available
    // TODO: RTL-case
    long nLastLeft = static_cast<long>(mfFontScale * FixedToFloat(mnBaseAdv));
    long nLastRight = nLastLeft;
    for( int i = 0; i < nMaxIndex; i+=2 )
    {
        // update left caret position
        if( pCaretXArray[i+0] >= 0 )
            nLastLeft = pCaretXArray[i+0];
        else
            pCaretXArray[i+0] = nLastLeft;
        // update right caret position
        if( pCaretXArray[i+1] >= 0 )
            nLastRight = pCaretXArray[i+1];
        else
            pCaretXArray[i+1] = nLastRight;
    }
#else
    // initialize the caret positions
    for( int i = 0; i < nMaxIndex; ++i )
        pCaretXArray[ i ] = -1;

    for( int n = 0; n <= mnCharCount; ++n )
    {
        // measure the characters cursor position
        typedef unsigned char Boolean;
        const Boolean bIsLeading = true;
        ATSUCaret aCaret0, aCaret1;
        Boolean bIsSplit;
        OSStatus eStatus = ATSUOffsetToCursorPosition( maATSULayout,
            mnMinCharPos + n, bIsLeading, kATSUByCharacter,
            &aCaret0, &aCaret1, &bIsSplit );
        if( eStatus != noErr )
            continue;
        const Fixed nFixedPos = mnBaseAdv + aCaret0.fX;
        // convert the measurement to pixel units
        const long nPixelPos = static_cast<long>(mfFontScale * FixedToFloat(nFixedPos));
        // update previous trailing position
        if( n > 0 )
            pCaretXArray[2*n-1] = nPixelPos;
        // update current leading position
        if( 2*n >= nMaxIndex )
            break;
        pCaretXArray[2*n+0] = nPixelPos;
    }
#endif
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::GetBoundRect : Get rectangle dim containing the layouted text
 *
 * @param rVCLRect: rectangle of text image (layout) measures
 *
 * Get ink bounds of the text
 *
 * @return : measurement valid
**/
bool ATSLayout::GetBoundRect( SalGraphics&, Rectangle& rVCLRect ) const
{
    const Point aPos = GetDrawPosition( Point(mnBaseAdv, 0) );
    const Fixed nFixedX = FloatToFixed( +aPos.X() / mfFontScale );
    const Fixed nFixedY = FloatToFixed( +aPos.Y() / mfFontScale );

    Rect aMacRect;
    OSStatus eStatus = ATSUMeasureTextImage( maATSULayout,
        mnMinCharPos, mnCharCount, nFixedX, nFixedY, &aMacRect );
    if( eStatus != noErr )
        return false;

    // ATSU top-bottom are vertically flipped from a VCL aspect
    rVCLRect.Left()     = static_cast<long>(mfFontScale * aMacRect.left);
    rVCLRect.Top()      = static_cast<long>(mfFontScale * aMacRect.top);
    rVCLRect.Right()    = static_cast<long>(mfFontScale * aMacRect.right);
    rVCLRect.Bottom()   = static_cast<long>(mfFontScale * aMacRect.bottom);
    return true;
}

// -----------------------------------------------------------------------
/**
 * ATSLayout::InitGIA() : Get many informations about layouted text
 *
 * Fills arrays of information about the gylph layout previously done
 *  in ASTLayout::LayoutText() : glyph advance (width), glyph delta Y (from baseline),
 *  mapping between glyph index and character index, chars widths
 *
 * @return : true if everything could be computed, otherwise false
**/
bool ATSLayout::InitGIA() const
{
    // no need to run InitGIA more than once on the same ATSLayout object
    if( mnGlyphCount >= 0 )
        return true;
    mnGlyphCount = 0;

    // Workaround a bug in ATSUI with empty string
    if( mnCharCount <=  0 )
        return false;

    // get details about the glyph layout
    ItemCount iLayoutDataCount;
    const ATSLayoutRecord* pALR;
    OSStatus eStatus = ATSUDirectGetLayoutDataArrayPtrFromTextLayout(
        maATSULayout, mnMinCharPos, kATSUDirectDataLayoutRecordATSLayoutRecordCurrent,
        (void**)&pALR, &iLayoutDataCount );
    DBG_ASSERT( (eStatus==noErr), "ATSLayout::InitGIA() : no ATSLayoutRecords!\n");
    if( (eStatus != noErr)
    || (iLayoutDataCount <= 1) )
        return false;

    // initialize glyph details
    mpGlyphIds      = new ATSGlyphRef[ iLayoutDataCount ];
    mpGlyphAdvances = new Fixed[ iLayoutDataCount ];
    mpGlyphs2Chars  = new int[ iLayoutDataCount ];

    // initialize character details
    mpCharWidths    = new Fixed[ mnCharCount ];
    mpChars2Glyphs  = new int[ mnCharCount ];
    for( int n = 0; n < mnCharCount; ++n )
    {
        mpCharWidths[ n ] = 0;
        mpChars2Glyphs[ n ] = -1;
    }

    // measure details of the glyph layout
    Fixed nLeftPos = 0;
    for( ItemCount i = 0; i < iLayoutDataCount; ++i )
    {
        const ATSLayoutRecord& rALR = pALR[i];

        // distribute the widths as fairly as possible among the chars
        const int nRelativeIdx = (rALR.originalOffset / 2);
        if( i+1 < iLayoutDataCount )
            mpCharWidths[ nRelativeIdx ] += pALR[i+1].realPos - rALR.realPos;

        // new glyph is available => finish measurement of old glyph
        if( mnGlyphCount > 0 )
            mpGlyphAdvances[ mnGlyphCount-1 ] = rALR.realPos - nLeftPos;

        // ignore marker or deleted glyphs
        if( rALR.glyphID >= MARKED_OUTGLYPH )
            continue;

        DBG_ASSERT( !(rALR.flags & kATSGlyphInfoTerminatorGlyph),
            "ATSLayout::InitGIA(): terminator glyph not marked as deleted!" )

        // store details of the visible glyphs
        nLeftPos = rALR.realPos;
        mpGlyphIds[ mnGlyphCount ] = rALR.glyphID;

        // map visible glyphs to their counterparts in the UTF16-character array
        mpGlyphs2Chars[ mnGlyphCount ] = nRelativeIdx + mnMinCharPos;
        mpChars2Glyphs[ nRelativeIdx ] = mnGlyphCount;

        ++mnGlyphCount;
    }

    // measure complete width
    mnCachedWidth = mnBaseAdv;
    mnCachedWidth += pALR[iLayoutDataCount-1].realPos - pALR[0].realPos;

#if (OSL_DEBUG_LEVEL > 1)
    Fixed nWidthSum = mnBaseAdv;
    for( int n = 0; n < mnCharCount; ++n )
        nWidthSum += mpCharWidths[ n ];
    DBG_ASSERT( (nWidthSum==mnCachedWidth),
        "ATSLayout::InitGIA(): measured widths do not match!\n" );
#endif

    // Release data array ptr
    ATSUDirectReleaseLayoutDataArrayPtr(NULL,
        kATSUDirectDataLayoutRecordATSLayoutRecordCurrent, (void**)&pALR );

    return true;
}

bool ATSLayout::GetIdealX() const
{
    // compute the ideal advance widths only once
    if( mpGlyphOrigAdvs != NULL )
        return true;

    DBG_ASSERT( (mpGlyphIds!=NULL), "GetIdealX() called with mpGlyphIds==NULL !" );
    DBG_ASSERT( (mpGraphics->maATSUStyle!=NULL), "GetIdealX called with maATSUStyle==NULL !" );

    // TODO: cache ideal metrics per glyph?
    std::vector<ATSGlyphIdealMetrics> aIdealMetrics;
    aIdealMetrics.resize( mnGlyphCount );
    OSStatus theErr = ATSUGlyphGetIdealMetrics( mpGraphics->maATSUStyle,
        mnGlyphCount, &mpGlyphIds[0], sizeof(*mpGlyphIds), &aIdealMetrics[0] );
    DBG_ASSERT( (theErr==noErr), "ATSUGlyphGetIdealMetrics failed!");
    if( theErr != noErr )
        return false;

    mpGlyphOrigAdvs = new Fixed[ mnGlyphCount ];
    for( int i = 0;i < mnGlyphCount;++i )
        mpGlyphOrigAdvs[i] = FloatToFixed( aIdealMetrics[i].advance.x );

    return true;
}

bool ATSLayout::GetDeltaY() const
{
    // don't bother to get the same delta-y-array more than once
    if( mpDeltaY != NULL )
        return true;

#if 0
    // get and keep the y-deltas in the mpDeltaY member variable
    // => release it in the destructor
    ItemCount nDeltaCount = 0;
    OSStatus theErr = ATSUDirectGetLayoutDataArrayPtrFromTextLayout(
        maATSULayout, mnMinCharPos, kATSUDirectDataBaselineDeltaFixedArray,
        (void**)&mpDeltaY, &nDeltaCount );

    DBG_ASSERT( (theErr==noErr ), "mpDeltaY - ATSUDirectGetLayoutDataArrayPtrFromTextLayout failed!\n");
    if( theErr != noErr )
        return false;

    if( mpDeltaY == NULL )
        return true;

    if( nDeltaCount != (ItemCount)mnGlyphCount )
    {
        DBG_WARNING( "ATSLayout::GetDeltaY() : wrong deltaY count!" );
        ATSUDirectReleaseLayoutDataArrayPtr( NULL,
            kATSUDirectDataBaselineDeltaFixedArray, (void**)&mpDeltaY );
        mpDeltaY = NULL;
        return false;
    }
#endif

    return true;
}

// =======================================================================

#if 0
// helper class to convert ATSUI outlines to VCL PolyPolygons
class PolyArgs
{
public:
                PolyArgs();
                ~PolyArgs();

    void        Init( PolyPolygon* pPolyPoly, long nXOffset, long nYOffset );
    void        AddPoint( const Float32Point&, PolyFlags );
    void        ClosePolygon();

private:
    PolyPolygon* mpPolyPoly;
    long        mnXOffset, mnYOffset;

    Point*      mpPointAry;
    BYTE*       mpFlagAry;
    USHORT      mnMaxPoints;

    USHORT      mnPointCount;
    USHORT      mnPolyCount;
    bool        mbHasOffline;
};

// -----------------------------------------------------------------------

PolyArgs::PolyArgs()
:   mpPolyPoly(NULL),
    mnPointCount(0),
    mnPolyCount(0),
    mbHasOffline(false)
{
    mnMaxPoints = 256;
    mpPointAry  = new Point[ mnMaxPoints ];
    mpFlagAry   = new BYTE [ mnMaxPoints ];
}

// -----------------------------------------------------------------------

PolyArgs::~PolyArgs()
{
    delete[] mpFlagAry;
    delete[] mpPointAry;
}

// -----------------------------------------------------------------------

void PolyArgs::Init( PolyPolygon* pPolyPoly, long nXOffset, long nYOffset )
{
    mnXOffset = nXOffset;
    mnYOffset = nYOffset;
    mpPolyPoly = pPolyPoly;

    mpPolyPoly->Clear();
    mnPointCount = 0;
    mnPolyCount  = 0;
}

// -----------------------------------------------------------------------

void PolyArgs::AddPoint( const Float32Point& rPoint, PolyFlags eFlags )
{
    if( mnPointCount >= mnMaxPoints )
    {
        // resize if needed (TODO: use STL?)
        mnMaxPoints *= 4;
        Point* mpNewPoints = new Point[ mnMaxPoints ];
        BYTE* mpNewFlags = new BYTE[ mnMaxPoints ];
        for( int i = 0; i < mnPointCount; ++i )
        {
            mpNewPoints[ i ] = mpPointAry[ i ];
            mpNewFlags[ i ] = mpFlagAry[ i ];
        }
        delete[] mpFlagAry;
        delete[] mpPointAry;
        mpPointAry = mpNewPoints;
        mpFlagAry = mpNewFlags;
    }

    // convert to pixels and add startpoint offset
    int nXPos = Float32ToInt( rPoint.x );
    int nYPos = Float32ToInt( rPoint.y );
    mpPointAry[ mnPointCount ] = Point( nXPos + mnXOffset, nYPos + mnYOffset );
    // set point flags
    mpFlagAry[ mnPointCount++ ]= eFlags;
    mbHasOffline |= (eFlags != POLY_NORMAL);
}

// -----------------------------------------------------------------------

void PolyArgs::ClosePolygon()
{
    if( !mnPolyCount++ )
         return;

    // append finished polygon
    Polygon aPoly( mnPointCount, mpPointAry, (mbHasOffline ? mpFlagAry : NULL) );
    mpPolyPoly->Insert( aPoly );

    // prepare for new polygon
    mnPointCount = 0;
    mbHasOffline = false;
}
#endif
// =======================================================================
#if 0
// helper functions for ATSLayout::GetGlyphOutlines()
OSStatus MyATSCubicMoveToCallback( const Float32Point *pt1,
   void* pData )
{
    PolyArgs& rA = *reinterpret_cast<PolyArgs*>(pData);
    // MoveTo implies a new polygon => finish old polygon first
    rA.ClosePolygon();
    rA.AddPoint( *pt1, POLY_NORMAL );
}

OSStatus MyATSCubicLineToCallback( const Float32Point* pt1,
    void* pData )
{
    PolyArgs& rA = *reinterpret_cast<PolyArgs*>(pData);
    rA.AddPoint( *pt1, POLY_NORMAL );
}

OSStatus MyATSCubicCurveToCallback( const Float32Point* pt1,
    const Float32Point* pt2, const Float32Point* pt3, void* pData )
{
    PolyArgs& rA = *reinterpret_cast<PolyArgs*>(pData);
    rA.AddPoint( *pt1, POLY_CONTROL );
    rA.AddPoint( *pt2, POLY_CONTROL );
    rA.AddPoint( *pt3, POLY_NORMAL );
}

OSStatus MyATSCubicClosePathCallback (
   void *pData )
{
    PolyArgs& rA = *reinterpret_cast<PolyArgs*>(pData);
    rA.ClosePolygon();
}
#endif
// -----------------------------------------------------------------------

bool ATSLayout::GetGlyphOutlines( SalGraphics&, PolyPolyVector& rPPV ) const
{
    AquaLog("-->%s not implemented yet\n",__func__);

    return false;

    /*
    rPPV.clear();

    if( !InitGIA() )
        return false;

    rPPV.resize( mpGIA->numGlyphs );
    PolyArgs aPolyArgs;
    const ATSUGlyphInfo* pG = mpGIA->glyphs;
    for( int i = 0; i < mpGIA->numGlyphs; ++i, ++pG )
    {
        // convert glyphid at glyphpos to outline
        GlyphID nGlyphId = pG->glyphID;
        long nDeltaY = Float32ToInt( pG->deltaY );
        aPolyArgs.Init( &rPPV[i], pG->screenX, nDeltaY );
        OSStatus nStatus, nCBStatus;
        nStatus = ATSUGlyphGetCubicPaths(
            mpGraphics->maATSUStyle, nGlyphId,
            MyATSCubicMoveToCallback, MyATSCubicLineToCallback,
            MyATSCubicCurveToCallback, MyATSCubicClosePathCallback,
            &aPolyArgs, &nCBStatus );

        if( (nStatus != noErr) && (nCBStatus != noErr) )
        {
            AquaLog("ATSUCallback = %d,%d\n", nStatus, nCBStatus );
            rPPV.resize( i );
            break;
        }
    }

    return true;
     */
}

// -----------------------------------------------------------------------

void ATSLayout::InitFont()
{
    // TODO to allow glyph fallback
}

// -----------------------------------------------------------------------

void ATSLayout::MoveGlyph( int nStart, long nNewXPos )
{
    AquaLog("-->%s\n",__func__);

    if( nStart > mnGlyphCount )
        return;

    // calculate the current x-position of the requested glyph
    // TODO: cache absolute positions
    Fixed nXPos = mnBaseAdv;
    for( int i = 0; i < nStart; ++i )
        nXPos += mpGlyphAdvances[i];

    // calculate the difference to the current glyph position
    Fixed nDelta = nNewXPos - nXPos;

    // adjust the width of the layout if it was already cached
    if( mnCachedWidth )
        mnCachedWidth += nDelta;

    // depending on whether the requested glyph is leftmost in the layout
    // adjust either the layout's or the requested glyph's relative position
    if( nStart > 0 )
        mpGlyphAdvances[ nStart-1 ] += nDelta;
    else
        mnBaseAdv += nDelta;
}

// -----------------------------------------------------------------------

void ATSLayout::DropGlyph( int nStart )
{
    AquaLog("-->%s\n",__func__);

    mpOutGlyphs[ nStart ] = DROPPED_OUTGLYPH;
}

// -----------------------------------------------------------------------

void ATSLayout::Simplify( bool bIsBase )
{
    AquaLog("-->%s\n",__func__);

    // return early if no glyph has been dropped
    int i = mnGlyphCount;
    while( (--i >= 0) && (mpOutGlyphs[ i ] != DROPPED_OUTGLYPH) );
    if( i < 0 )
        return;

    // convert the layout to a sparse layout if it is not already
    if( !mpGlyphs2Chars )
    {
        mpGlyphs2Chars = new int[ mnGlyphCount ];
        mpCharWidths = new Fixed[ mnCharCount ];
        // assertion: mnGlyphCount == mnCharCount
        for( int k = 0; k < mnGlyphCount; ++k )
        {
            mpGlyphs2Chars[ k ] = mnMinCharPos + k;
            mpCharWidths[ k ] = mpGlyphAdvances[ k ];
        }
    }

    // remove dropped glyphs that are rightmost in the layout
    for( i = mnGlyphCount; --i >= 0; )
    {
        if( mpOutGlyphs[ i ] != DROPPED_OUTGLYPH )
            break;
        if( mnCachedWidth )
            mnCachedWidth -= mpGlyphAdvances[ i ];
        int nRelCharPos = mpGlyphs2Chars[ i ] - mnMinCharPos;
        if( nRelCharPos >= 0 )
            mpCharWidths[ nRelCharPos ] = 0;
    }
    mnGlyphCount = i + 1;

    // keep original glyph widths around
    if( !mpGlyphOrigAdvs )
    {
        mpGlyphOrigAdvs = new Fixed[ mnGlyphCount ];
        for( int k = 0; k < mnGlyphCount; ++k )
            mpGlyphOrigAdvs[ k ] = mpGlyphAdvances[ k ];
    }

    // remove dropped glyphs inside the layout
    int nNewGC = 0;
    for( i = 0; i < mnGlyphCount; ++i )
    {
        if( mpOutGlyphs[ i ] == DROPPED_OUTGLYPH )
        {
            // adjust relative position to last valid glyph
            int nDroppedWidth = mpGlyphAdvances[ i ];
            mpGlyphAdvances[ i ] = 0;
            if( nNewGC > 0 )
                mpGlyphAdvances[ nNewGC-1 ] += nDroppedWidth;
            else
                mnBaseAdv += nDroppedWidth;

            // zero the virtual char width for the char that has a fallback
            int nRelCharPos = mpGlyphs2Chars[ i ] - mnMinCharPos;
            if( nRelCharPos >= 0 )
                mpCharWidths[ nRelCharPos ] = 0;
        }
        else
        {
            if( nNewGC != i )
            {
                // rearrange the glyph array to get rid of the dropped glyph
                mpOutGlyphs[ nNewGC ]     = mpOutGlyphs[ i ];
                mpGlyphAdvances[ nNewGC ] = mpGlyphAdvances[ i ];
                mpGlyphOrigAdvs[ nNewGC ] = mpGlyphOrigAdvs[ i ];
                mpGlyphs2Chars[ nNewGC ]  = mpGlyphs2Chars[ i ];
            }
            ++nNewGC;
        }
    }

    mnGlyphCount = nNewGC;
    if( mnGlyphCount <= 0 )
        mnCachedWidth = mnBaseAdv = 0;
}

// =======================================================================

SalLayout* AquaSalGraphics::GetTextLayout( ImplLayoutArgs& rArgs, int nFallbackLevel )
{
    ATSLayout* pATSLayout = new ATSLayout( this, mfFontScale );
    return pATSLayout;
}

// =======================================================================

