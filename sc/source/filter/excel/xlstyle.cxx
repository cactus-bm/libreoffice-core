/*************************************************************************
 *
 *  $RCSfile: xlstyle.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2003-04-28 15:35:47 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif
#pragma hdrstop

// ============================================================================

#ifndef SC_XLSTYLE_HXX
#include "xlstyle.hxx"
#endif

#ifndef _SV_FONT_HXX
#include <vcl/font.hxx>
#endif
#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif


// Color data =================================================================

/** Default color table for BIFF2. */
static const ColorData pDefColorTable2[] = { 0x000000, 0xFFFFFF };

/** Default color table for BIFF3/BIFF4. */
static const ColorData pDefColorTable3[] =
{
/*  8 */    0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF,
/* 16 */    0x800000, 0x008000, 0x000080, 0x808000, 0x800080, 0x008080, 0xC0C0C0, 0x808080
};

/** Default color table for BIFF5/BIFF7. */
static const ColorData pDefColorTable5[] =
{
/*  8 */    0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF,
/* 16 */    0x800000, 0x008000, 0x000080, 0x808000, 0x800080, 0x008080, 0xC0C0C0, 0x808080,
/* 24 */    0x8080FF, 0x802060, 0xFFFFC0, 0xA0E0E0, 0x600080, 0xFF8080, 0x0080C0, 0xC0C0FF,
/* 32 */    0x000080, 0xFF00FF, 0xFFFF00, 0x00FFFF, 0x800080, 0x800000, 0x008080, 0x0000FF,
/* 40 */    0x00CFFF, 0x69FFFF, 0xE0FFE0, 0xFFFF80, 0xA6CAF0, 0xDD9CB3, 0xB38FEE, 0xE3E3E3,
/* 48 */    0x2A6FF9, 0x3FB8CD, 0x488436, 0x958C41, 0x8E5E42, 0xA0627A, 0x624FAC, 0x969696,
/* 56 */    0x1D2FBE, 0x286676, 0x004500, 0x453E01, 0x6A2813, 0x85396A, 0x4A3285, 0x424242
};

/** Default color table for BIFF8. */
static const ColorData pDefColorTable8[] =
{
/*  8 */    0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF,
/* 16 */    0x800000, 0x008000, 0x000080, 0x808000, 0x800080, 0x008080, 0xC0C0C0, 0x808080,
/* 24 */    0x9999FF, 0x993366, 0xFFFFCC, 0xCCFFFF, 0x660066, 0xFF8080, 0x0066CC, 0xCCCCFF,
/* 32 */    0x000080, 0xFF00FF, 0xFFFF00, 0x00FFFF, 0x800080, 0x800000, 0x008080, 0x0000FF,
/* 40 */    0x00CCFF, 0xCCFFFF, 0xCCFFCC, 0xFFFF99, 0x99CCFF, 0xFF99CC, 0xCC99FF, 0xFFCC99,
/* 48 */    0x3366FF, 0x33CCCC, 0x99CC00, 0xFFCC00, 0xFF9900, 0xFF6600, 0x666699, 0x969696,
/* 56 */    0x003366, 0x339966, 0x003300, 0x333300, 0x993300, 0x993366, 0x333399, 0x333333
};


// ----------------------------------------------------------------------------

XclDefaultPalette::XclDefaultPalette( XclBiff eBiff ) :
    mpColorTable( NULL ),
    mnTableSize( 0 ),
    mnIndexOffset( 0 )
{
    if( eBiff != xlBiffUnknown )
        SetDefaultColors( eBiff );
}

void XclDefaultPalette::SetDefaultColors( XclBiff eBiff )
{
    switch( eBiff )
    {
        case xlBiff2:
            mpColorTable = pDefColorTable2;
            mnTableSize = STATIC_TABLE_SIZE( pDefColorTable2 );
            mnIndexOffset = EXC_COLOR_OFFSET2;
        break;
        case xlBiff3:
        case xlBiff4:
            mpColorTable = pDefColorTable3;
            mnTableSize = STATIC_TABLE_SIZE( pDefColorTable3 );
            mnIndexOffset = EXC_COLOR_OFFSET3;
        break;
        case xlBiff5:
        case xlBiff7:
            mpColorTable = pDefColorTable5;
            mnTableSize = STATIC_TABLE_SIZE( pDefColorTable5 );
            mnIndexOffset = EXC_COLOR_OFFSET5;
        break;
        case xlBiff8:
            mpColorTable = pDefColorTable8;
            mnTableSize = STATIC_TABLE_SIZE( pDefColorTable8 );
            mnIndexOffset = EXC_COLOR_OFFSET8;
        break;
        default:
            DBG_ERROR_BIFF();
    }
}

ColorData XclDefaultPalette::GetDefColorData( sal_uInt16 nXclIndex, ColorData nDefault ) const
{
    return ((mnIndexOffset <= nXclIndex) && (nXclIndex < mnIndexOffset + mnTableSize)) ?
        mpColorTable[ nXclIndex - mnIndexOffset ] : nDefault;
}


// Font Data ==================================================================

XclFontData::XclFontData()
{
    Clear();
}

XclFontData::XclFontData( const Font& rFont )
{
    FillFromFont( rFont );
}

void XclFontData::Clear()
{
    maName.Erase();
    maStyle.Erase();
    meUnderline = xlUnderlNone;
    meEscapem = xlEscNone;
    mnHeight = 0;
    mnColor = EXC_FONT_AUTOCOLOR;
    mnWeight = EXC_FONTWGHT_DONTKNOW;
    mnFamily = EXC_FONTFAM_SYSTEM;
    mnCharSet = EXC_FONTCSET_DONTKNOW;
    mbItalic = mbStrikeout = mbOutline = mbShadow = false;
}

void XclFontData::FillFromFont( const Font& rFont )
{
    maName = XclTools::GetXclFontName( rFont.GetName() );   // #106246# substitute with MS fonts
    maStyle.Erase();
    SetScUnderline( rFont.GetUnderline() );
    meEscapem = xlEscNone;
    SetScHeight( rFont.GetSize().Height() );
    mnColor = EXC_FONT_AUTOCOLOR;
    SetScWeight( rFont.GetWeight() );
    SetScFamily( rFont.GetFamily() );
    SetScCharSet( rFont.GetCharSet() );
    SetScPosture( rFont.GetItalic() );
    SetScStrikeout( rFont.GetStrikeout() );
    mbOutline = !!rFont.IsOutline();  // BOOL->bool
    mbShadow = !!rFont.IsShadow();    // BOOL->bool
}

FontFamily XclFontData::GetScFamily( CharSet eDefCharSet ) const
{
    FontFamily eScFamily;
    // ! format differs from Windows documentation: family is in lower nibble, pitch unknown
    switch( mnFamily & 0x0F )
    {
        case EXC_FONTFAM_ROMAN:         eScFamily = FAMILY_ROMAN;       break;
        case EXC_FONTFAM_SWISS:         eScFamily = FAMILY_SWISS;       break;
        case EXC_FONTFAM_MODERN:        eScFamily = FAMILY_MODERN;      break;
        case EXC_FONTFAM_SCRIPT:        eScFamily = FAMILY_SCRIPT;      break;
        case EXC_FONTFAM_DECORATIVE:    eScFamily = FAMILY_DECORATIVE;  break;
        default:
            eScFamily =
                ((eDefCharSet == RTL_TEXTENCODING_APPLE_ROMAN) &&
                (maName.EqualsIgnoreCaseAscii( "Geneva" ) || maName.EqualsIgnoreCaseAscii( "Chicago" ))) ?
                FAMILY_SWISS : FAMILY_DONTKNOW;
    }
    return eScFamily;
}

CharSet XclFontData::GetScCharSet() const
{
    return rtl_getTextEncodingFromWindowsCharset( mnCharSet );
}

FontItalic XclFontData::GetScPosture() const
{
    return mbItalic ? ITALIC_NORMAL : ITALIC_NONE;
}

FontWeight XclFontData::GetScWeight() const
{
    FontWeight eScWeight;

    if( !mnWeight )             eScWeight = WEIGHT_DONTKNOW;
    else if( mnWeight < 150 )   eScWeight = WEIGHT_THIN;
    else if( mnWeight < 250 )   eScWeight = WEIGHT_ULTRALIGHT;
    else if( mnWeight < 325 )   eScWeight = WEIGHT_LIGHT;
    else if( mnWeight < 375 )   eScWeight = WEIGHT_SEMILIGHT;
    else if( mnWeight < 450 )   eScWeight = WEIGHT_NORMAL;
    else if( mnWeight < 550 )   eScWeight = WEIGHT_MEDIUM;
    else if( mnWeight < 650 )   eScWeight = WEIGHT_SEMIBOLD;
    else if( mnWeight < 750 )   eScWeight = WEIGHT_BOLD;
    else if( mnWeight < 850 )   eScWeight = WEIGHT_ULTRABOLD;
    else                        eScWeight = WEIGHT_BLACK;

    return eScWeight;
}

FontUnderline XclFontData::GetScUnderline() const
{
    FontUnderline eScUnderl = UNDERLINE_NONE;
    switch( meUnderline )
    {
        case xlUnderlSingle:
        case xlUnderlSingleAcc: eScUnderl = UNDERLINE_SINGLE;  break;
        case xlUnderlDouble:
        case xlUnderlDoubleAcc: eScUnderl = UNDERLINE_DOUBLE;  break;
    }
    return eScUnderl;
}

SvxEscapement XclFontData::GetScEscapement() const
{
    SvxEscapement eScEscapem = SVX_ESCAPEMENT_OFF;
    switch( meEscapem )
    {
        case xlEscSuper:    eScEscapem = SVX_ESCAPEMENT_SUPERSCRIPT;    break;
        case xlEscSub:      eScEscapem = SVX_ESCAPEMENT_SUBSCRIPT;      break;
    }
    return eScEscapem;
}

FontStrikeout XclFontData::GetScStrikeout() const
{
    return mbStrikeout ? STRIKEOUT_SINGLE : STRIKEOUT_NONE;
}

void XclFontData::SetScHeight( sal_Int32 nTwips )
{
    mnHeight = static_cast< sal_uInt16 >( ::std::min( nTwips, 0x7FFFL ) );
}

void XclFontData::SetScFamily( FontFamily eScFamily )
{
    switch( eScFamily )
    {
        case FAMILY_DONTKNOW:   mnCharSet = EXC_FONTFAM_DONTKNOW;   break;
        case FAMILY_DECORATIVE: mnCharSet = EXC_FONTFAM_DECORATIVE; break;
        case FAMILY_MODERN:     mnCharSet = EXC_FONTFAM_MODERN;     break;
        case FAMILY_ROMAN:      mnCharSet = EXC_FONTFAM_ROMAN;      break;
        case FAMILY_SCRIPT:     mnCharSet = EXC_FONTFAM_SCRIPT;     break;
        case FAMILY_SWISS:      mnCharSet = EXC_FONTFAM_SWISS;      break;
        case FAMILY_SYSTEM:     mnCharSet = EXC_FONTFAM_SYSTEM;     break;
        default:
            DBG_ERRORFILE( "XclFontData::SetScFamily - unknown font family" );
            mnCharSet = EXC_FONTFAM_DONTKNOW;
    }
}

void XclFontData::SetScCharSet( CharSet eScCharSet )
{
    mnCharSet = rtl_getBestWindowsCharsetFromTextEncoding( eScCharSet );
}


void XclFontData::SetScPosture( FontItalic eScPosture )
{
    mbItalic = (eScPosture == ITALIC_OBLIQUE) || (eScPosture == ITALIC_NORMAL);
}

void XclFontData::SetScWeight( FontWeight eScWeight )
{
    switch( eScWeight )
    {
        case WEIGHT_DONTKNOW:   mnWeight = EXC_FONTWGHT_DONTKNOW;   break;
        case WEIGHT_THIN:       mnWeight = EXC_FONTWGHT_THIN;       break;
        case WEIGHT_ULTRALIGHT: mnWeight = EXC_FONTWGHT_ULTRALIGHT; break;
        case WEIGHT_LIGHT:      mnWeight = EXC_FONTWGHT_LIGHT;      break;
        case WEIGHT_SEMILIGHT:  mnWeight = EXC_FONTWGHT_SEMILIGHT;  break;
        case WEIGHT_NORMAL:     mnWeight = EXC_FONTWGHT_NORMAL;     break;
        case WEIGHT_MEDIUM:     mnWeight = EXC_FONTWGHT_MEDIUM;     break;
        case WEIGHT_SEMIBOLD:   mnWeight = EXC_FONTWGHT_SEMIBOLD;   break;
        case WEIGHT_BOLD:       mnWeight = EXC_FONTWGHT_BOLD;       break;
        case WEIGHT_ULTRABOLD:  mnWeight = EXC_FONTWGHT_ULTRABOLD;  break;
        case WEIGHT_BLACK:      mnWeight = EXC_FONTWGHT_BLACK;      break;
        default:                mnWeight = EXC_FONTWGHT_NORMAL;
    }
}

void XclFontData::SetScUnderline( FontUnderline eScUnderl )
{
    switch( eScUnderl )
    {
        case UNDERLINE_SINGLE:  meUnderline = xlUnderlSingle;       break;
        case UNDERLINE_DOUBLE:  meUnderline = xlUnderlDouble;       break;
        case UNDERLINE_DOTTED:  meUnderline = xlUnderlSingleAcc;    break;
        default:                meUnderline = xlUnderlNone;
    }
}

void XclFontData::SetScEscapement( SvxEscapement eScEscapem )
{
    switch( eScEscapem )
    {
        case SVX_ESCAPEMENT_SUPERSCRIPT:    meEscapem = xlEscSuper; break;
        case SVX_ESCAPEMENT_SUBSCRIPT:      meEscapem = xlEscSub;   break;
        default:                            meEscapem = xlEscNone;
    }
}

void XclFontData::SetScStrikeout( FontStrikeout eScStrikeout )
{
    mbStrikeout =
        (eScStrikeout == STRIKEOUT_SINGLE) || (eScStrikeout == STRIKEOUT_DOUBLE) ||
        (eScStrikeout == STRIKEOUT_BOLD) || (eScStrikeout == STRIKEOUT_SLASH) ||
        (eScStrikeout == STRIKEOUT_X);
}

bool operator==( const XclFontData& rLeft, const XclFontData& rRight )
{
    return
        (rLeft.mnHeight    == rRight.mnHeight)    &&
        (rLeft.mnWeight    == rRight.mnWeight)    &&
        (rLeft.mnColor     == rRight.mnColor)     &&
        (rLeft.meUnderline == rRight.meUnderline) &&
        (rLeft.meEscapem   == rRight.meEscapem)   &&
        (rLeft.mnFamily    == rRight.mnFamily)    &&
        (rLeft.mnCharSet   == rRight.mnCharSet)   &&
        (rLeft.mbItalic    == rRight.mbItalic)    &&
        (rLeft.mbStrikeout == rRight.mbStrikeout) &&
        (rLeft.mbOutline   == rRight.mbOutline)   &&
        (rLeft.mbShadow    == rRight.mbShadow)    &&
        (rLeft.maName      == rRight.maName);
}


// Cell formatting data (XF) ==================================================

XclCellProt::XclCellProt() :
    mbLocked( true ),       // default in Excel and Calc
    mbHidden( false )
{
}

bool operator==( const XclCellProt& rLeft, const XclCellProt& rRight )
{
    return (rLeft.mbLocked == rRight.mbLocked) && (rLeft.mbHidden == rRight.mbHidden);
}


// ----------------------------------------------------------------------------

XclCellAlign::XclCellAlign() :
    meHorAlign( xlHAlign_Default ),
    meVerAlign( xlVAlign_Default ),
    meTextDir( xlTextDir_Default ),
    meOrient( xlTextOrient_Default ),
    mnRotation( 0 ),
    mnIndent( 0 ),
    mbWrapped( false )
{
}

bool operator==( const XclCellAlign& rLeft, const XclCellAlign& rRight )
{
    return
        (rLeft.meHorAlign == rRight.meHorAlign) && (rLeft.meVerAlign == rRight.meVerAlign) &&
        (rLeft.meTextDir  == rRight.meTextDir)  &&
        (rLeft.meOrient   == rRight.meOrient)   && (rLeft.mnRotation == rRight.mnRotation) &&
        (rLeft.mnIndent   == rRight.mnIndent)   && (rLeft.mbWrapped  == rRight.mbWrapped);
}


// ----------------------------------------------------------------------------

XclCellBorder::XclCellBorder() :
    mnLeftColor( EXC_XF_NOCOLOR ),
    mnRightColor( EXC_XF_NOCOLOR ),
    mnTopColor( EXC_XF_NOCOLOR ),
    mnBottomColor( EXC_XF_NOCOLOR ),
    mnLeftLine( EXC_LINE_NONE ),
    mnRightLine( EXC_LINE_NONE ),
    mnTopLine( EXC_LINE_NONE ),
    mnBottomLine( EXC_LINE_NONE )
{
}

bool operator==( const XclCellBorder& rLeft, const XclCellBorder& rRight )
{
    return
        (rLeft.mnLeftColor == rRight.mnLeftColor) && (rLeft.mnRightColor  == rRight.mnRightColor)  &&
        (rLeft.mnTopColor  == rRight.mnTopColor)  && (rLeft.mnBottomColor == rRight.mnBottomColor) &&
        (rLeft.mnLeftLine  == rRight.mnLeftLine)  && (rLeft.mnRightLine   == rRight.mnRightLine)   &&
        (rLeft.mnTopLine   == rRight.mnTopLine)   && (rLeft.mnBottomLine  == rRight.mnBottomLine);
}


// ----------------------------------------------------------------------------

XclCellArea::XclCellArea() :
    mnForeColor( EXC_XF_AUTOCOLOR ),
    mnBackColor( EXC_XF_TRANSPCOLOR ),
    mnPattern( EXC_PATT_NONE )
{
}

bool XclCellArea::IsTransparent() const
{
    return (mnPattern == EXC_PATT_NONE) &&
        (((mnForeColor == EXC_XF_AUTOCOLOR) && (mnBackColor == EXC_XF_TRANSPCOLOR)) ||
        (mnForeColor == EXC_XF_NOCOLOR));
}

bool operator==( const XclCellArea& rLeft, const XclCellArea& rRight )
{
    return
        (rLeft.mnForeColor == rRight.mnForeColor) && (rLeft.mnBackColor == rRight.mnBackColor) &&
        (rLeft.mnPattern == rRight.mnPattern);
}


// ----------------------------------------------------------------------------

XclXFBase::XclXFBase( bool bCellXF ) :
    mnParent( bCellXF ? EXC_XF_DEFAULTSTYLE : EXC_XF_STYLEPARENT ),
    mbCellXF( bCellXF )
{
    SetAllUsedFlags( false );
}

void XclXFBase::SetAllUsedFlags( bool bUsed )
{
    mbProtUsed = mbFontUsed = mbFmtUsed = mbAlignUsed = mbBorderUsed = mbAreaUsed = bUsed;
}

bool XclXFBase::Equals( const XclXFBase& rCmp ) const
{
    return
        (mbCellXF     == rCmp.mbCellXF)     && (mnParent    == rCmp.mnParent)    &&
        (mbProtUsed   == rCmp.mbProtUsed)   && (mbFontUsed  == rCmp.mbFontUsed)  &&
        (mbFmtUsed    == rCmp.mbFmtUsed)    && (mbAlignUsed == rCmp.mbAlignUsed) &&
        (mbBorderUsed == rCmp.mbBorderUsed) && (mbAreaUsed  == rCmp.mbAreaUsed);
}

// ============================================================================
