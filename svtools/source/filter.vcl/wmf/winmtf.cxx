/*************************************************************************
 *
 *  $RCSfile: winmtf.cxx,v $
 *
 *  $Revision: 1.32 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-24 15:03:09 $
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


#include "winmtf.hxx"
#include <vcl/metaact.hxx>
#ifndef _SV_METRIC_HXX
#include <vcl/metric.hxx>
#endif

// ------------------------------------------------------------------------

#define WIN_MTF_MAX_POLYPOLYCOUNT   16

void WinMtfClipPath::ImpUpdateType()
{
    if ( !aPolyPoly.Count() )
        eType = EMPTY;
    else if ( aPolyPoly.IsRect() )
        eType = RECTANGLE;
    else
        eType = COMPLEX;

    bNeedsUpdate = sal_True;
}

void WinMtfClipPath::IntersectClipRect( const Rectangle& rRect )
{
    if ( !aPolyPoly.Count() )
        aPolyPoly = Polygon( rRect );
    else if ( aPolyPoly.Count() < WIN_MTF_MAX_POLYPOLYCOUNT )
    {
        Polygon aPolygon( rRect );
        PolyPolygon aIntersection;
        PolyPolygon aPolyPolyRect( aPolygon );
        aPolyPoly.GetIntersection( aPolyPolyRect, aIntersection );
        aPolyPoly = aIntersection;
    }
    ImpUpdateType();
}

void WinMtfClipPath::ExcludeClipRect( const Rectangle& rRect )
{
    if ( aPolyPoly.Count() && ( aPolyPoly.Count() < WIN_MTF_MAX_POLYPOLYCOUNT ) )
    {
        Polygon aPolygon( rRect );
        PolyPolygon aPolyPolyRect( aPolygon );
        PolyPolygon aDifference;
        aPolyPoly.GetDifference( aPolyPolyRect, aDifference );
        aPolyPoly = aDifference;
    }
    ImpUpdateType();
}

void WinMtfClipPath::SetClipPath( const PolyPolygon& rPolyPolygon, sal_Int32 nClippingMode )
{
    if ( !rPolyPolygon.Count() )
        aPolyPoly = rPolyPolygon;
    else if ( rPolyPolygon.Count() < WIN_MTF_MAX_POLYPOLYCOUNT )
    {
        PolyPolygon aNewClipPath;
        switch ( nClippingMode )
        {
            case RGN_OR :
                aPolyPoly.GetUnion( rPolyPolygon, aNewClipPath );
            break;
            case RGN_XOR :
                aPolyPoly.GetXOR( rPolyPolygon, aNewClipPath );
            break;
            case RGN_DIFF :
                aPolyPoly.GetDifference( rPolyPolygon, aNewClipPath );
            break;
            case RGN_AND :
                aPolyPoly.GetIntersection( rPolyPolygon, aNewClipPath );
            break;
            case RGN_COPY :
                aNewClipPath = rPolyPolygon;
            break;
        }
        aPolyPoly = aNewClipPath;
    }
    ImpUpdateType();
}

void WinMtfClipPath::MoveClipRegion( const Size& rSize )
{
    aPolyPoly.Move( rSize.Width(), rSize.Height() );
    bNeedsUpdate = sal_True;
}

// ------------------------------------------------------------------------

void WinMtfPathObj::AddPoint( const Point& rPoint )
{
    if ( bClosed )
        Insert( Polygon(), POLYPOLY_APPEND );
    Polygon& rPoly = ((PolyPolygon&)*this)[ Count() - 1 ];
    rPoly.Insert( rPoly.GetSize(), rPoint, POLY_NORMAL );
    bClosed = sal_False;
}

void WinMtfPathObj::AddPolyLine( const Polygon& rPolyLine )
{
    if ( bClosed )
        Insert( Polygon(), POLYPOLY_APPEND );
    Polygon& rPoly = ((PolyPolygon&)*this)[ Count() - 1 ];
    rPoly.Insert( rPoly.GetSize(), rPolyLine );
    bClosed = sal_False;
}

void WinMtfPathObj::AddPolygon( const Polygon& rPoly )
{
    Insert( rPoly, POLYPOLY_APPEND );
    bClosed = sal_True;
}

void WinMtfPathObj::AddPolyPolygon( const PolyPolygon& rPolyPoly )
{
    sal_uInt16 i, nCount = rPolyPoly.Count();
    for ( i = 0; i < nCount; i++ )
        Insert( rPolyPoly[ i ], POLYPOLY_APPEND );
    bClosed = sal_True;
}

void WinMtfPathObj::ClosePath()
{
    bClosed = sal_True;
}

// ------------------------------------------------------------------------

WinMtfFontStyle::WinMtfFontStyle( LOGFONTW& rFont )
{
    CharSet eCharSet;
    switch ( rFont.lfCharSet )
    {
        case ANSI_CHARSET:
            eCharSet = RTL_TEXTENCODING_MS_1252;
        break;

        case SYMBOL_CHARSET:
            eCharSet = RTL_TEXTENCODING_SYMBOL;
        break;

        case SHIFTJIS_CHARSET:
            eCharSet = RTL_TEXTENCODING_SHIFT_JIS;
        break;

        case CHINESEBIG5_CHARSET:
            eCharSet = RTL_TEXTENCODING_BIG5;
        break;

        case HANGEUL_CHARSET :
            eCharSet = RTL_TEXTENCODING_MS_949;
        break;

        case OEM_CHARSET:
        case DEFAULT_CHARSET:
        default:
            eCharSet = gsl_getSystemTextEncoding();
        break;
    }
    aFont.SetCharSet( eCharSet );
    CharSet eFontNameEncoding = eCharSet == RTL_TEXTENCODING_SYMBOL ?
        RTL_TEXTENCODING_MS_1252 : eCharSet;
    aFont.SetName( UniString( (char*)rFont.lfFaceName, eFontNameEncoding ) );
    FontFamily eFamily;
    switch ( rFont.lfPitchAndFamily & 0xf0 )
    {
        case FF_ROMAN:
            eFamily = FAMILY_ROMAN;
        break;

        case FF_SWISS:
            eFamily = FAMILY_SWISS;
        break;

        case FF_MODERN:
            eFamily = FAMILY_MODERN;
        break;

        case FF_SCRIPT:
            eFamily = FAMILY_SCRIPT;
        break;

        case FF_DECORATIVE:
             eFamily = FAMILY_DECORATIVE;
        break;

        default:
            eFamily = FAMILY_DONTKNOW;
        break;
    }
    aFont.SetFamily( eFamily );

    FontPitch ePitch;
    switch ( rFont.lfPitchAndFamily & 0x0f )
    {
        case FIXED_PITCH:
            ePitch = PITCH_FIXED;
        break;

        case DEFAULT_PITCH:
        case VARIABLE_PITCH:
        default:
            ePitch = PITCH_VARIABLE;
        break;
    }
    aFont.SetPitch( ePitch );

    FontWeight eWeight;
    if( rFont.lfWeight <= FW_THIN )
        eWeight = WEIGHT_THIN;
    else if( rFont.lfWeight <= FW_ULTRALIGHT )
        eWeight = WEIGHT_ULTRALIGHT;
    else if( rFont.lfWeight <= FW_LIGHT )
        eWeight = WEIGHT_LIGHT;
    else if( rFont.lfWeight <  FW_MEDIUM )
        eWeight = WEIGHT_NORMAL;
    else if( rFont.lfWeight == FW_MEDIUM )
        eWeight = WEIGHT_MEDIUM;
    else if( rFont.lfWeight <= FW_SEMIBOLD )
        eWeight = WEIGHT_SEMIBOLD;
    else if( rFont.lfWeight <= FW_BOLD )
        eWeight = WEIGHT_BOLD;
    else if( rFont.lfWeight <= FW_ULTRABOLD )
        eWeight = WEIGHT_ULTRABOLD;
    else
        eWeight = WEIGHT_BLACK;
    aFont.SetWeight( eWeight );

    if( rFont.lfItalic )
        aFont.SetItalic( ITALIC_NORMAL );

    if( rFont.lfUnderline )
        aFont.SetUnderline( UNDERLINE_SINGLE );

    if( rFont.lfStrikeOut )
        aFont.SetStrikeout( STRIKEOUT_SINGLE );

    if ( rFont.lfOrientation )
        aFont.SetOrientation( (short)rFont.lfOrientation );
    else
        aFont.SetOrientation( (short)rFont.lfEscapement );

    Size aFontSize( Size( rFont.lfWidth, rFont.lfHeight ) );
    if ( rFont.lfHeight > 0 )
    {
        // converting the cell height into a font height
        VirtualDevice aVDev;
        aVDev.SetMapMode( MapMode( MAP_100TH_MM ) );
        aFont.SetSize( aFontSize );
        aVDev.SetFont( aFont );
        FontMetric aMetric( aVDev.GetFontMetric() );
        long nHeight = aMetric.GetAscent() + aMetric.GetDescent();
        if ( nHeight )
        {
            aFontSize.Height() *= rFont.lfHeight;
            aFontSize.Height() /= nHeight;
        }
    }
    else
        aFontSize.Height() *= -1;

    aFont.SetSize( aFontSize );
};

// ------------------------------------------------------------------------

#ifdef WIN_MTF_ASSERT
const void WinMtfAssertHandler( const sal_Char* pAction, sal_uInt32 nFlags )
{
    static sal_Bool     bOnlyOnce;
    static sal_Int32    nAssertCount;

    if ( nFlags & WIN_MTF_ASSERT_INIT )
        nAssertCount = 0;
    if ( nFlags & WIN_MTF_ASSERT_ONCE )
       bOnlyOnce = sal_True;
    if ( nFlags & WIN_MTF_ASSERT_MIFE )
    {
        if ( ( nAssertCount == 0 ) || ( bOnlyOnce == sal_False ) )
        {
            ByteString aText( "WMF/EMF Import: " );
            if ( pAction )
            {
                ByteString aAction( pAction );
                aText.Append( aAction );
            }
            aText.Append( " needs to be implemented (SJ)" );
            DBG_ASSERT( 0, aText.GetBuffer() );
        }
        nAssertCount++;
    }
}
#endif

// ------------------------------------------------------------------------

WinMtf::WinMtf( WinMtfOutput* pWinMtfOutput, SvStream& rStreamWMF, PFilterCallback pcallback, void * pcallerdata ) :
    pOut                ( pWinMtfOutput ),
    pCallback           ( pcallback ),
    pCallerData         ( pcallerdata ),
    pWMF                ( &rStreamWMF )
{
#ifdef WIN_MTF_ASSERT
    // we want to assert not implemented features, but we do this
    // only once, so that nobody is handicaped by getting too much assertions
    // I hope this will bring more testdocuments, without support of these
    // testdocuments the implementation of missing features won't be possible. (SJ)
    WinMtfAssertHandler( NULL, WIN_MTF_ASSERT_INIT | WIN_MTF_ASSERT_ONCE );
#endif

    SvLockBytes *pLB = pWMF->GetLockBytes();
    if ( pLB )
        pLB->SetSynchronMode( TRUE );

    nStartPos = pWMF->Tell();

    pOut->SetDevOrg( Point() );
}

// ------------------------------------------------------------------------

WinMtf::~WinMtf()
{
    delete pOut;
}

// ------------------------------------------------------------------------

BOOL WinMtf::Callback( USHORT nPercent )
{
    if ( pCallback != NULL )
    {
        if( (*pCallback)( pCallerData, nPercent ) )
        {
            pWMF->SetError( SVSTREAM_FILEFORMAT_ERROR );
            return TRUE;
        }
    }
    return FALSE;
}

// ------------------------------------------------------------------------

Color WinMtf::ReadColor()
{
    UINT32 nColor;
    *pWMF >> nColor;
    return Color( (BYTE)nColor, (BYTE)( nColor >> 8 ), (BYTE)( nColor >> 16 ) );
};

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

Point WinMtfOutput::ImplMap( const Point& rPt )
{
    if ( mnWinExtX && mnWinExtY )
    {
        double fX2, fX = rPt.X();
        double fY2, fY = rPt.Y();

        fX2 = fX * maXForm.eM11 + fY * maXForm.eM21 + maXForm.eDx;
        fY2 = fX * maXForm.eM12 + fY * maXForm.eM22 + maXForm.eDy;

        if ( mnGfxMode == GM_COMPATIBLE )
        {
            switch( mnMapMode )
            {
                case MM_LOENGLISH :
                {
                    fY2 *= -1;
                    fX2 -= mnWinOrgX;
                    fY2 -= mnWinOrgY;
                    fX2 *= 25.40;
                    fY2 *= 25.40;
                    fX2 += mnDevOrgX;
                    fY2 += mnDevOrgY;
                }
                break;
                case MM_HIENGLISH :
                {
                    fY2 *= -1;
                    fX2 -= mnWinOrgX;
                    fY2 -= mnWinOrgY;
                    fX2 *= 2.540;
                    fY2 *= 2.540;
                    fX2 += mnDevOrgX;
                    fY2 += mnDevOrgY;
                }
                break;
                case MM_LOMETRIC :
                {
                    fY2 *= -1;
                    fX2 -= mnWinOrgX;
                    fY2 -= mnWinOrgY;
                    fX2 *= 10;
                    fY2 *= 10;
                    fX2 += mnDevOrgX;
                    fY2 += mnDevOrgY;
                }
                break;
                case MM_HIMETRIC :
                {
                    fY2 *= -1;
                    fX2 -= mnWinOrgX;
                    fY2 -= mnWinOrgY;
                    fX2 += mnDevOrgX;
                    fY2 += mnDevOrgY;
                }
                break;
                default :
                {
                    fX2 -= mnWinOrgX;
                    fY2 -= mnWinOrgY;
                    fX2 /= mnWinExtX;
                    fY2 /= mnWinExtY;
                    fX2 *= mnDevWidth;
                    fY2 *= mnDevHeight;
                    fX2 += mnDevOrgX;
                    fY2 += mnDevOrgY;   // fX2, fY2 now in device units
                    fX2 *= (double)mnMillX * 100.0 / (double)mnPixX;
                    fY2 *= (double)mnMillY * 100.0 / (double)mnPixY;
                }
                break;
            }
            fX2 -= mrclFrame.Left();
            fY2 -= mrclFrame.Top();
        }
        return Point( FRound( fX2 ), FRound( fY2 ) );
    }
    else
        return Point();
};

// ------------------------------------------------------------------------

Size WinMtfOutput::ImplMap( const Size& rSz )
{
    if ( mnWinExtX && mnWinExtY )
    {
        double fX = rSz.Width() * maXForm.eM11;
        double fY = rSz.Width() * maXForm.eM12;
        double fWidth = sqrt( fX * fX + fY * fY );
        if ( rSz.Width() < 0 )
            fWidth *= -1;

        fX = rSz.Height() * maXForm.eM21;
        fY = rSz.Height() * maXForm.eM22;
        double fHeight = sqrt( fX * fX + fY * fY );
        if ( rSz.Height() < 0 )
            fHeight *= -1;

        if ( mnGfxMode == GM_COMPATIBLE )
        {
            switch( mnMapMode )
            {
                case MM_LOENGLISH :
                {
                    fWidth *= 25.40;
                    fHeight*=-25.40;
                }
                break;
                case MM_HIENGLISH :
                {
                    fWidth *= 2.540;
                    fHeight*=-2.540;
                }
                break;
                case MM_LOMETRIC :
                {
                    fWidth *= 10;
                    fHeight*=-10;
                }
                break;
                case MM_HIMETRIC :
                {
                    fHeight *= -1;
                }
                break;
                default :
                {
                    fWidth /= mnWinExtX;
                    fHeight /= mnWinExtY;
                    fWidth *= mnDevWidth;
                    fHeight *= mnDevHeight;
                    fWidth *= (double)mnMillX * 100 / (double)mnPixX;
                    fHeight *= (double)mnMillY * 100 / (double)mnPixY;
                }
                break;
            }
        }
        return Size( FRound( fWidth ), FRound( fHeight ) );
    }
    else
        return Size();
}

//-----------------------------------------------------------------------------------

Rectangle WinMtfOutput::ImplMap( const Rectangle& rRect )
{
    return Rectangle( ImplMap( rRect.TopLeft() ), ImplMap( rRect.GetSize() ) );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ImplMap( Font& rFont )
{
    // !!! HACK: Wir setzen die Breite jetzt immer auf Null,
    // da OS die Breite unterschiedlich interpretieren;
    // muss spaeter in SV portabel gemacht werden ( KA 08.02.96 )
    Size  aFontSize = ImplMap ( rFont.GetSize() );

    if( aFontSize.Height() < 0 )
        aFontSize.Height() *= -1;

    rFont.SetSize( aFontSize );

    if( ( mnWinExtX * mnWinExtY ) < 0 )
        rFont.SetOrientation( 3600 - rFont.GetOrientation() );
}

//-----------------------------------------------------------------------------------

Polygon& WinMtfOutput::ImplMap( Polygon& rPolygon )
{
    UINT16 nPoints = rPolygon.GetSize();
    for ( UINT16 i = 0; i < nPoints; i++ )
    {
        rPolygon[ i ] = ImplMap( rPolygon[ i ] );
    }
    return rPolygon;
}

//-----------------------------------------------------------------------------------

PolyPolygon& WinMtfOutput::ImplMap( PolyPolygon& rPolyPolygon )
{
    UINT16 nPolys = rPolyPolygon.Count();
    for ( UINT16 i = 0; i < nPolys; ImplMap( rPolyPolygon[ i++ ] ) );
    return rPolyPolygon;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SelectObject( INT32 nIndex )
{
    GDIObj* pGDIObj = NULL;

    if ( nIndex & ENHMETA_STOCK_OBJECT )
        pGDIObj = new GDIObj();
    else
    {
        nIndex &= 0xffff;       // zur Sicherheit: mehr als 65535 nicht zulassen

        if ( (UINT32)nIndex < mnEntrys )
            pGDIObj = mpGDIObj[ nIndex ];
    }

    if( pGDIObj == NULL )
        return;

    if ( nIndex & ENHMETA_STOCK_OBJECT )
    {
        UINT16 nStockId = (BYTE)nIndex;
        switch( nStockId )
        {
            case WHITE_BRUSH :
            {
                pGDIObj->Set( GDI_BRUSH, new WinMtfFillStyle( Color( COL_WHITE ) ) );
            }
            break;
            case LTGRAY_BRUSH :
            {
                pGDIObj->Set( GDI_BRUSH, new WinMtfFillStyle( Color( COL_LIGHTGRAY ) ) );
            }
            break;
            case GRAY_BRUSH :
            case DKGRAY_BRUSH :
            {
                pGDIObj->Set( GDI_BRUSH, new WinMtfFillStyle( Color( COL_GRAY ) ) );
            }
            break;
            case BLACK_BRUSH :
            {
                pGDIObj->Set( GDI_BRUSH, new WinMtfFillStyle( Color( COL_BLACK ) ) );
            }
            break;
            case NULL_BRUSH :
            {
                pGDIObj->Set( GDI_BRUSH, new WinMtfFillStyle( Color( COL_TRANSPARENT ), TRUE ) );
            }
            break;
            case WHITE_PEN :
            {
                pGDIObj->Set( GDI_PEN, new WinMtfLineStyle( Color( COL_WHITE ) ) );
            }
            break;
            case BLACK_PEN :
            {
                pGDIObj->Set( GDI_PEN, new WinMtfLineStyle( Color( COL_BLACK ) ) );
            }
            break;
            case NULL_PEN :
            {
                pGDIObj->Set( GDI_PEN, new WinMtfLineStyle( Color( COL_TRANSPARENT ), TRUE ) );
            }
            break;
            default:
            break;
        }
    }
    if ( pGDIObj->pStyle )
    {
        switch( pGDIObj->eType )
        {
            case GDI_PEN :
                maLineStyle = (WinMtfLineStyle*)pGDIObj->pStyle;
            break;
            case GDI_BRUSH :
                maFillStyle = (WinMtfFillStyle*)pGDIObj->pStyle;
            break;
            case GDI_FONT :
                maFont = ((WinMtfFontStyle*)pGDIObj->pStyle)->aFont;
            break;
        }
    }
    if ( nIndex & ENHMETA_STOCK_OBJECT )
        delete pGDIObj;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetFont( const Font& rFont )
{
    maFont = rFont;
}

//-----------------------------------------------------------------------------------

const Font& WinMtfOutput::GetFont() const
{
    return maFont;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetBkMode( UINT32 nMode )
{
    mnBkMode = nMode;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetBkColor( const Color& rColor )
{
    maBkColor = rColor;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetTextColor( const Color& rColor )
{
    maTextColor = rColor;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetTextAlign( UINT32 nAlign )
{
    mnTextAlign = nAlign;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ImplResizeObjectArry( UINT32 nNewEntrys )
{
    GDIObj** pGDIObj = new GDIObj*[ mnEntrys << 1 ];
    UINT32 nIndex;
    for ( nIndex = 0; nIndex < mnEntrys; nIndex++ )
        pGDIObj[ nIndex ] = mpGDIObj[ nIndex ];
    for ( mnEntrys = nNewEntrys; nIndex < mnEntrys; pGDIObj[ nIndex++ ] = NULL );
    delete[] mpGDIObj, mpGDIObj = pGDIObj;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ImplDrawClippedPolyPolygon( const PolyPolygon& rPolyPoly )
{
    if ( rPolyPoly.Count() )
    {
        ImplSetNonPersistentLineColorTransparenz();
        if ( rPolyPoly.Count() == 1 )
        {
            if ( rPolyPoly.IsRect() )
                mpGDIMetaFile->AddAction( new MetaRectAction( rPolyPoly.GetBoundRect() ) );
            else
            {
                Polygon aPoly( rPolyPoly[ 0 ] );
                sal_uInt16 nCount = aPoly.GetSize();
                if ( nCount )
                {
                    if ( aPoly[ nCount - 1 ] != aPoly[ 0 ] )
                    {
                        Point aPoint( aPoly[ 0 ] );
                        aPoly.Insert( nCount, aPoint );
                    }
                    mpGDIMetaFile->AddAction( new MetaPolygonAction( aPoly ) );
                }
            }
        }
        else
            mpGDIMetaFile->AddAction( new MetaPolyPolygonAction( rPolyPoly ) );
    }
}


//-----------------------------------------------------------------------------------

void WinMtfOutput::CreateObject( GDIObjectType eType, void* pStyle )
{
    if ( pStyle )
    {
        if ( eType == GDI_FONT )
            ImplMap( ((WinMtfFontStyle*)pStyle)->aFont );
        else if ( eType == GDI_PEN )
        {
            Size aSize( ((WinMtfLineStyle*)pStyle)->aLineInfo.GetWidth(), 0 );
            ((WinMtfLineStyle*)pStyle)->aLineInfo.SetWidth( ImplMap( aSize ).Width() );
            if ( ((WinMtfLineStyle*)pStyle)->aLineInfo.GetStyle() == LINE_DASH )
            {
                aSize.Width() += 1;
                long nDotLen = ImplMap( aSize ).Width();
                ((WinMtfLineStyle*)pStyle)->aLineInfo.SetDistance( nDotLen );
                ((WinMtfLineStyle*)pStyle)->aLineInfo.SetDotLen( nDotLen );
                ((WinMtfLineStyle*)pStyle)->aLineInfo.SetDashLen( nDotLen * 4 );
            }
        }
    }
    UINT32 nIndex;
    for ( nIndex = 0; nIndex < mnEntrys; nIndex++ )
    {
        if ( mpGDIObj[ nIndex ] == NULL )
            break;
    }
    if ( nIndex == mnEntrys )
        ImplResizeObjectArry( mnEntrys << 1 );

    mpGDIObj[ nIndex ] = new GDIObj( eType, pStyle );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::CreateObject( INT32 nIndex, GDIObjectType eType, void* pStyle )
{
    if ( ( nIndex & ENHMETA_STOCK_OBJECT ) == 0 )
    {
        nIndex &= 0xffff;       // zur Sicherheit: mehr als 65535 nicht zulassen
        if ( pStyle )
        {
            if ( eType == GDI_FONT )
                ImplMap( ((WinMtfFontStyle*)pStyle)->aFont );
            else if ( eType == GDI_PEN )
            {
                Size aSize( ((WinMtfLineStyle*)pStyle)->aLineInfo.GetWidth(), 0 );
                ((WinMtfLineStyle*)pStyle)->aLineInfo.SetWidth( ImplMap( aSize ).Width() );
                if ( ((WinMtfLineStyle*)pStyle)->aLineInfo.GetStyle() == LINE_DASH )
                {
                    aSize.Width() += 1;
                    long nDotLen = ImplMap( aSize ).Width();
                    ((WinMtfLineStyle*)pStyle)->aLineInfo.SetDistance( nDotLen );
                    ((WinMtfLineStyle*)pStyle)->aLineInfo.SetDotLen( nDotLen );
                    ((WinMtfLineStyle*)pStyle)->aLineInfo.SetDashLen( nDotLen * 4 );
                }
            }
        }
        if ( (UINT32)nIndex >= mnEntrys )
            ImplResizeObjectArry( nIndex + 16 );

        if ( mpGDIObj[ nIndex ] != NULL )
            delete mpGDIObj[ nIndex ];

        mpGDIObj[ nIndex ] = new GDIObj( eType, pStyle );
    }
    else
        delete pStyle;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DeleteObject( sal_Int32 nIndex )
{
    if ( ( nIndex & ENHMETA_STOCK_OBJECT ) == 0 )
    {
        if ( (sal_uInt32)nIndex < mnEntrys )
        {
            delete mpGDIObj[ nIndex ];
            mpGDIObj[ nIndex ] = NULL;
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::IntersectClipRect( const Rectangle& rRect )
{
    aClipPath.IntersectClipRect( ImplMap( rRect ) );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ExcludeClipRect( const Rectangle& rRect )
{
    aClipPath.ExcludeClipRect( ImplMap( rRect ) );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::MoveClipRegion( const Size& rSize )
{
    aClipPath.MoveClipRegion( ImplMap( rSize ) );
}

void WinMtfOutput::SetClipPath( const PolyPolygon& rPolyPolygon, sal_Int32 nClippingMode )
{
    aClipPath.SetClipPath( rPolyPolygon, nClippingMode );
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

WinMtfOutput::WinMtfOutput( GDIMetaFile& rGDIMetaFile ) :
    mpGDIMetaFile       ( &rGDIMetaFile ),
    mnTextAlign         ( TA_LEFT | TA_TOP | TA_NOUPDATECP ),
    mnLatestTextAlign   ( 0 ),
    mnBkMode            ( OPAQUE ),
    mnLatestBkMode      ( 0 ),
    maBkColor           ( COL_WHITE ),
    maLatestBkColor     ( 0x12345678 ),
    mbNopMode           ( sal_False ),
    maActPos            ( Point() ),
    meRasterOp          ( ROP_OVERPAINT ),
    meLatestRasterOp    ( ROP_INVERT ),
    mnEntrys            ( 16 ),
    mnGfxMode           ( GM_COMPATIBLE ),
    mnMapMode           ( MM_TEXT ),
    mnDevOrgX           ( 0 ),
    mnDevOrgY           ( 0 ),
    mnDevWidth          ( 1 ),
    mnDevHeight         ( 1 ),
    mnWinOrgX           ( 0 ),
    mnWinOrgY           ( 0 ),
    mnWinExtX           ( 1 ),
    mnWinExtY           ( 1 ),
    mnPixX              ( 100 ),
    mnPixY              ( 100 ),
    mnMillX             ( 1 ),
    mnMillY             ( 1 )
{
    mpGDIMetaFile->AddAction( new MetaPushAction( PUSH_CLIPREGION ) );  // The original clipregion has to be on top
    maFont.SetCharSet( gsl_getSystemTextEncoding() );                   // of the stack so it can always be restored
    mpGDIObj = new GDIObj*[ mnEntrys ];                                 // this is necessary to be able to support
    for ( UINT32 i = 0; i < mnEntrys; i++ )                             // SetClipRgn( NULL ) and similar ClipRgn actions (SJ)
    {
        mpGDIObj[ i ] = NULL;
    }
    maLatestLineStyle.aLineColor = Color( 0x12, 0x34, 0x56 );
    maLatestFillStyle.aFillColor = Color( 0x12, 0x34, 0x56 );

    mnRop = R2_BLACK + 1;
    SetRasterOp( R2_BLACK );
};

//-----------------------------------------------------------------------------------

WinMtfOutput::~WinMtfOutput()
{
    while( maSaveStack.Count() )
        delete maSaveStack.Pop();

    mpGDIMetaFile->AddAction( new MetaPopAction() );
    mpGDIMetaFile->SetPrefMapMode( MAP_100TH_MM );
    if ( mrclFrame.IsEmpty() )
        mpGDIMetaFile->SetPrefSize( Size( mnDevWidth, mnDevHeight ) );
    else
        mpGDIMetaFile->SetPrefSize( mrclFrame.GetSize() );

    for ( UINT32 i = 0; i < mnEntrys; i++ )
    {
        delete mpGDIObj[ i ];
    }
    delete[] mpGDIObj;
};

//-----------------------------------------------------------------------------------

void WinMtfOutput::UpdateClipRegion()
{
    if ( aClipPath.bNeedsUpdate )
    {
        aClipPath.bNeedsUpdate = sal_False;

        mpGDIMetaFile->AddAction( new MetaPopAction() );                    // taking the orignal clipregion
        mpGDIMetaFile->AddAction( new MetaPushAction( PUSH_CLIPREGION ) );  //

        switch ( aClipPath.GetType() )
        {
            case RECTANGLE :
            case COMPLEX :
            {
//              we will not generate a RegionClipRegion Action, because this action
//              cannot be saved to the wmf format - saving to wmf always happens
//              if the placeholder graphic for ole objects is generated. (SJ)

//              Region aClipRegion( aClipPath.GetClipPath() );
//              mpGDIMetaFile->AddAction( new MetaISectRegionClipRegionAction( aClipRegion ) );

                Rectangle aClipRect( aClipPath.GetClipPath().GetBoundRect() );
                mpGDIMetaFile->AddAction( new MetaISectRectClipRegionAction( aClipRect ) );
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ImplSetNonPersistentLineColorTransparenz()
{
    Color aColor(  COL_TRANSPARENT);
    WinMtfLineStyle aTransparentLine( aColor, TRUE );
    if ( ! ( maLatestLineStyle == aTransparentLine ) )
    {
        maLatestLineStyle = aTransparentLine;
        mpGDIMetaFile->AddAction( new MetaLineColorAction( aTransparentLine.aLineColor, !aTransparentLine.bTransparent ) );
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::UpdateLineStyle()
{
    if (!( maLatestLineStyle == maLineStyle ) )
    {
        maLatestLineStyle = maLineStyle;
        mpGDIMetaFile->AddAction( new MetaLineColorAction( maLineStyle.aLineColor, !maLineStyle.bTransparent ) );
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::UpdateFillStyle()
{
    if (!( maLatestFillStyle == maFillStyle ) )
    {
        maLatestFillStyle = maFillStyle;
        mpGDIMetaFile->AddAction( new MetaFillColorAction( maFillStyle.aFillColor, !maFillStyle.bTransparent ) );
    }
}

//-----------------------------------------------------------------------------------

sal_uInt32 WinMtfOutput::SetRasterOp( UINT32 nRasterOp )
{
    sal_uInt32 nRetROP = mnRop;
    if ( nRasterOp != mnRop )
    {
        mnRop = nRasterOp;
        static WinMtfFillStyle aNopFillStyle;
        static WinMtfLineStyle aNopLineStyle;

        if ( mbNopMode && ( nRasterOp != R2_NOP ) )
        {   // beim uebergang von R2_NOP auf anderen Modus
            // gesetzten Pen und Brush aktivieren
            maFillStyle = aNopFillStyle;
            maLineStyle = aNopLineStyle;
            mbNopMode = sal_False;
        }
        switch( nRasterOp )
        {
            case R2_NOT:
                meRasterOp = ROP_INVERT;
            break;

            case R2_XORPEN:
                meRasterOp = ROP_XOR;
            break;

            case R2_NOP:
            {
                meRasterOp = ROP_OVERPAINT;
                if( mbNopMode == sal_False )
                {
                    aNopFillStyle = maFillStyle;
                    aNopLineStyle = maLineStyle;
                    maFillStyle = WinMtfFillStyle( Color( COL_TRANSPARENT ), TRUE );
                    maLineStyle = WinMtfLineStyle( Color( COL_TRANSPARENT ), TRUE );
                    mbNopMode = sal_True;
                }
            }
            break;

            default:
                meRasterOp = ROP_OVERPAINT;
            break;
        }
    }
    if ( nRetROP != nRasterOp )
        mpGDIMetaFile->AddAction( new MetaRasterOpAction( meRasterOp ) );
    return nRetROP;
};

//-----------------------------------------------------------------------------------

void WinMtfOutput::StrokeAndFillPath( sal_Bool bStroke, sal_Bool bFill )
{
    if ( aPathObj.Count() )
    {
        UpdateClipRegion();
        UpdateLineStyle();
        UpdateFillStyle();
        if ( bFill )
        {
            if ( !bStroke )
            {
                mpGDIMetaFile->AddAction( new MetaPushAction( PUSH_LINECOLOR ) );
                mpGDIMetaFile->AddAction( new MetaLineColorAction( Color(), FALSE ) );
            }
            if ( aPathObj.Count() == 1 )
                mpGDIMetaFile->AddAction( new MetaPolygonAction( aPathObj.GetObject( 0 ) ) );
            else
                mpGDIMetaFile->AddAction( new MetaPolyPolygonAction( aPathObj ) );

            if ( !bStroke )
                mpGDIMetaFile->AddAction( new MetaPopAction() );
        }
        else
        {
            sal_uInt16 i, nCount = aPathObj.Count();
            for ( i = 0; i < nCount; i++ )
                mpGDIMetaFile->AddAction( new MetaPolyLineAction( aPathObj[ i ], maLineStyle.aLineInfo ) );
        }
        ClearPath();
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawPixel( const Point& rSource, const Color& rColor )
{
    mpGDIMetaFile->AddAction( new MetaPixelAction( ImplMap( rSource), rColor ) );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::MoveTo( const Point& rPoint, sal_Bool bRecordPath )
{
    Point aDest( ImplMap( rPoint ) );
    if ( bRecordPath )
        aPathObj.AddPoint( aDest );
    maActPos = aDest;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::LineTo( const Point& rPoint, sal_Bool bRecordPath )
{
    UpdateClipRegion();

    Point aDest( ImplMap( rPoint ) );
    if ( bRecordPath )
        aPathObj.AddPoint( aDest );
    else
    {
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaLineAction( maActPos, aDest, maLineStyle.aLineInfo ) );
    }
    maActPos = aDest;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawLine( const Point& rSource, const Point& rDest )
{
    UpdateClipRegion();
    UpdateLineStyle();
    mpGDIMetaFile->AddAction( new MetaLineAction( ImplMap( rSource), ImplMap( rDest ), maLineStyle.aLineInfo ) );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawRect( const Rectangle& rRect, BOOL bEdge )
{
    UpdateClipRegion();
    UpdateFillStyle();

    if ( aClipPath.GetType() == COMPLEX )
    {
        Polygon aPoly( ImplMap( rRect ) );
        PolyPolygon aPolyPolyRect( aPoly );
        PolyPolygon aDest;
        aClipPath.GetClipPath().GetIntersection( aPolyPolyRect, aDest );
        ImplDrawClippedPolyPolygon( aDest );
    }
    else
    {
        if ( bEdge )
        {
            if ( maLineStyle.aLineInfo.GetWidth() || ( maLineStyle.aLineInfo.GetStyle() == LINE_DASH ) )
            {
                ImplSetNonPersistentLineColorTransparenz();
                mpGDIMetaFile->AddAction( new MetaRectAction( ImplMap( rRect ) ) );
                UpdateLineStyle();
                mpGDIMetaFile->AddAction( new MetaPolyLineAction( Polygon( ImplMap( rRect ) ),maLineStyle.aLineInfo ) );
            }
            else
            {
                UpdateLineStyle();
                mpGDIMetaFile->AddAction( new MetaRectAction( ImplMap( rRect ) ) );
            }
        }
        else
        {
            ImplSetNonPersistentLineColorTransparenz();
            mpGDIMetaFile->AddAction( new MetaRectAction( ImplMap( rRect ) ) );
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawRoundRect( const Rectangle& rRect, const Size& rSize )
{
    UpdateClipRegion();
    UpdateLineStyle();
    UpdateFillStyle();
    mpGDIMetaFile->AddAction( new MetaRoundRectAction( ImplMap( rRect ), labs( ImplMap( rSize ).Width() ), labs( ImplMap( rSize ).Height() ) ) );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawEllipse( const Rectangle& rRect )
{
    UpdateClipRegion();
    UpdateFillStyle();

    if ( maLineStyle.aLineInfo.GetWidth() || ( maLineStyle.aLineInfo.GetStyle() == LINE_DASH ) )
    {
        Point aCenter( ImplMap( rRect.Center() ) );
        Size  aRad( ImplMap( Size( rRect.GetWidth() / 2, rRect.GetHeight() / 2 ) ) );

        ImplSetNonPersistentLineColorTransparenz();
        mpGDIMetaFile->AddAction( new MetaEllipseAction( ImplMap( rRect ) ) );
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaPolyLineAction( Polygon( aCenter, aRad.Width(), aRad.Height() ), maLineStyle.aLineInfo ) );
    }
    else
    {
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaEllipseAction( ImplMap( rRect ) ) );
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawArc( const Rectangle& rRect, const Point& rStart, const Point& rEnd, BOOL bTo )
{
    UpdateClipRegion();
    UpdateLineStyle();
    UpdateFillStyle();

    Rectangle   aRect( ImplMap( rRect ) );
    Point       aStart( ImplMap( rStart ) );
    Point       aEnd( ImplMap( rEnd ) );

    if ( maLineStyle.aLineInfo.GetWidth() || ( maLineStyle.aLineInfo.GetStyle() == LINE_DASH ) )
        mpGDIMetaFile->AddAction( new MetaPolyLineAction( Polygon( aRect, aStart, aEnd, POLY_ARC ), maLineStyle.aLineInfo ) );
    else
        mpGDIMetaFile->AddAction( new MetaArcAction( aRect, aStart, aEnd ) );

    if ( bTo )
        maActPos = aEnd;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawPie( const Rectangle& rRect, const Point& rStart, const Point& rEnd )
{
    UpdateClipRegion();
    UpdateFillStyle();

    Rectangle   aRect( ImplMap( rRect ) );
    Point       aStart( ImplMap( rStart ) );
    Point       aEnd( ImplMap( rEnd ) );

    if ( maLineStyle.aLineInfo.GetWidth() || ( maLineStyle.aLineInfo.GetStyle() == LINE_DASH ) )
    {
        ImplSetNonPersistentLineColorTransparenz();
        mpGDIMetaFile->AddAction( new MetaPieAction( aRect, aStart, aEnd ) );
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaPolyLineAction( Polygon( aRect, aStart, aEnd, POLY_PIE ), maLineStyle.aLineInfo ) );
    }
    else
    {
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaPieAction( aRect, aStart, aEnd ) );
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawChord( const Rectangle& rRect, const Point& rStart, const Point& rEnd )
{
    UpdateClipRegion();
    UpdateFillStyle();

    Rectangle   aRect( ImplMap( rRect ) );
    Point       aStart( ImplMap( rStart ) );
    Point       aEnd( ImplMap( rEnd ) );

    if ( maLineStyle.aLineInfo.GetWidth() || ( maLineStyle.aLineInfo.GetStyle() == LINE_DASH ) )
    {
        ImplSetNonPersistentLineColorTransparenz();
        mpGDIMetaFile->AddAction( new MetaChordAction( aRect, aStart, aEnd ) );
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaPolyLineAction( Polygon( aRect, aStart, aEnd, POLY_CHORD ), maLineStyle.aLineInfo ) );
    }
    else
    {
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaChordAction( aRect, aStart, aEnd ) );
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawPolygon( Polygon& rPolygon, sal_Bool bRecordPath )
{
    UpdateClipRegion();
    ImplMap( rPolygon );
    if ( bRecordPath )
        aPathObj.AddPolygon( rPolygon );
    else
    {
        UpdateFillStyle();

        if ( aClipPath.GetType() == COMPLEX )
        {
            PolyPolygon aPolyPoly( rPolygon );
            PolyPolygon aDest;
            aClipPath.GetClipPath().GetIntersection( aPolyPoly, aDest );
            ImplDrawClippedPolyPolygon( aDest );
        }
        else
        {
            if ( maLineStyle.aLineInfo.GetWidth() || ( maLineStyle.aLineInfo.GetStyle() == LINE_DASH ) )
            {
                USHORT nCount = rPolygon.GetSize();
                if ( nCount )
                {
                    if ( rPolygon[ nCount - 1 ] != rPolygon[ 0 ] )
                    {
                        Point aPoint( rPolygon[ 0 ] );
                        rPolygon.Insert( nCount, aPoint );
                    }
                }
                ImplSetNonPersistentLineColorTransparenz();
                mpGDIMetaFile->AddAction( new MetaPolygonAction( rPolygon ) );
                UpdateLineStyle();
                mpGDIMetaFile->AddAction( new MetaPolyLineAction( rPolygon, maLineStyle.aLineInfo ) );
            }
            else
            {
                UpdateLineStyle();
                mpGDIMetaFile->AddAction( new MetaPolygonAction( rPolygon ) );
            }
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawPolyPolygon( PolyPolygon& rPolyPolygon, sal_Bool bRecordPath )
{
    UpdateClipRegion();

    ImplMap( rPolyPolygon );

    if ( bRecordPath )
        aPathObj.AddPolyPolygon( rPolyPolygon );
    else
    {
        UpdateFillStyle();

        if ( aClipPath.GetType() == COMPLEX )
        {
            PolyPolygon aDest;
            aClipPath.GetClipPath().GetIntersection( rPolyPolygon, aDest );
            ImplDrawClippedPolyPolygon( aDest );
        }
        else
        {
            UpdateLineStyle();
            mpGDIMetaFile->AddAction( new MetaPolyPolygonAction( rPolyPolygon ) );
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawPolyLine( Polygon& rPolygon, sal_Bool bTo, sal_Bool bRecordPath )
{
    UpdateClipRegion();

    ImplMap( rPolygon );
    if ( bTo )
    {
        rPolygon[ 0 ] = maActPos;
        maActPos = rPolygon[ rPolygon.GetSize() - 1 ];
    }
    if ( bRecordPath )
        aPathObj.AddPolyLine( rPolygon );
    else
    {
        UpdateLineStyle();
        mpGDIMetaFile->AddAction( new MetaPolyLineAction( rPolygon, maLineStyle.aLineInfo ) );
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawPolyBezier( Polygon& rPolygon, sal_Bool bTo, sal_Bool bRecordPath )
{
    UpdateClipRegion();

    sal_uInt16 nPoints = rPolygon.GetSize();
    if ( ( nPoints >= 4 ) && ( ( ( nPoints - 4 ) % 3 ) == 0 ) )
    {
        ImplMap( rPolygon );
        if ( bTo )
        {
            rPolygon[ 0 ] = maActPos;
            maActPos = rPolygon[ nPoints - 1 ];
        }
        sal_uInt16 i;
        for ( i = 0; ( i + 2 ) < nPoints; )
        {
            rPolygon.SetFlags( i++, POLY_NORMAL );
            rPolygon.SetFlags( i++, POLY_CONTROL );
            rPolygon.SetFlags( i++, POLY_CONTROL );
        }
        if ( bRecordPath )
            aPathObj.AddPolyLine( rPolygon );
        else
        {
            UpdateLineStyle();
            mpGDIMetaFile->AddAction( new MetaPolyLineAction( rPolygon, maLineStyle.aLineInfo ) );
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::DrawText( Point& rPosition, String& rText, sal_Int32* pDXArry, sal_Bool bRecordPath, sal_Int32 nGfxMode )
{
    UpdateClipRegion();

    rPosition = ImplMap( nGfxMode == GM_ADVANCED ? Point() : rPosition );
    sal_Int32 nOldGfxMode = GetGfxMode();
    SetGfxMode( GM_COMPATIBLE );
    if ( pDXArry )
    {
        sal_Int32 i, nSum, nLen = rText.Len();

        for( i = 0, nSum = 0; i < nLen; i++ )
        {
            sal_Int32 nTemp = ImplMap( Size( pDXArry[ i ], 0 ) ).Width();
            nSum += nTemp;
            pDXArry[ i ] = nSum;
        }
    }
    SetGfxMode( nGfxMode );
    sal_Bool bChangeFont = sal_False;
    if ( mnLatestTextAlign != mnTextAlign )
    {
        bChangeFont = sal_True;
        mnLatestTextAlign = mnTextAlign;
        TextAlign eTextAlign;
        if ( ( mnTextAlign & TA_BASELINE) == TA_BASELINE )
            eTextAlign = ALIGN_BASELINE;
        else if( ( mnTextAlign & TA_BOTTOM) == TA_BOTTOM )
            eTextAlign = ALIGN_BOTTOM;
        else
            eTextAlign = ALIGN_TOP;
        mpGDIMetaFile->AddAction( new MetaTextAlignAction( eTextAlign ) );
    }
    if ( maLatestTextColor != maTextColor )
    {
        bChangeFont = sal_True;
        maLatestTextColor = maTextColor;
        mpGDIMetaFile->AddAction( new MetaTextColorAction( maTextColor ) );
    }
    sal_Bool bChangeFillColor = sal_False;
    if ( maLatestBkColor != maBkColor )
    {
        bChangeFillColor = sal_True;
        maLatestBkColor = maBkColor;
    }
    if ( mnLatestBkMode != mnBkMode )
    {
        bChangeFillColor = sal_True;
        mnLatestBkMode = mnBkMode;
    }
    if ( bChangeFillColor )
    {
        bChangeFont = sal_True;
        mpGDIMetaFile->AddAction( new MetaTextFillColorAction( maFont.GetFillColor(), !maFont.IsTransparent() ) );
    }

    Font aTmp( maFont );
    aTmp.SetColor( maTextColor );
    aTmp.SetFillColor( maBkColor );

    if( mnBkMode == TRANSPARENT )
        maFont.SetTransparent( sal_True );
    else
        maFont.SetTransparent( sal_False );

    if ( ( mnTextAlign & TA_BASELINE) == TA_BASELINE )
        aTmp.SetAlign( ALIGN_BASELINE );
    else if( ( mnTextAlign & TA_BOTTOM) == TA_BOTTOM )
        aTmp.SetAlign( ALIGN_BOTTOM );
    else
        aTmp.SetAlign( ALIGN_TOP );

    if ( nGfxMode == GM_ADVANCED )
    {
        Point aP1( ImplMap( Point() ) );
        Point aP2( ImplMap( Point( 0, aTmp.GetHeight() ) ) );
        aP2.X() -= aP1.X();
        aP2.Y() -= aP1.Y();
        double fX = aP2.X();
        double fY = aP2.Y();
        double fHeight = sqrt( fX * fX + fY * fY );
        aTmp.SetHeight( (sal_Int32)fHeight );

        double fOrientation = acos( fX / sqrt( fX * fX + fY * fY ) ) * 57.29577951308;
        if ( fY > 0 )
            fOrientation = 360 - fOrientation;
        fOrientation += 90;
        aTmp.SetOrientation( sal_Int16( fOrientation * 10.0 ) );
    }
    if ( bChangeFont || ( maLatestFont != aTmp ) )
    {
        maLatestFont = aTmp;
        mpGDIMetaFile->AddAction( new MetaFontAction( aTmp ) );
        mpGDIMetaFile->AddAction( new MetaTextAlignAction( aTmp.GetAlign() ) );
        mpGDIMetaFile->AddAction( new MetaTextColorAction( aTmp.GetColor() ) );
        mpGDIMetaFile->AddAction( new MetaTextFillColorAction( aTmp.GetFillColor(), !aTmp.IsTransparent() ) );
    }
    if( mnTextAlign & ( TA_UPDATECP | TA_RIGHT_CENTER ) )
    {
        VirtualDevice   aVDev;
        sal_Int32       nTextWidth;

        aVDev.SetMapMode( MapMode( MAP_100TH_MM ) );
        aVDev.SetFont( maFont );

        if( pDXArry )
        {
            UINT32 nLen = rText.Len();
            nTextWidth = aVDev.GetTextWidth( rText.GetChar( (sal_uInt16)( nLen - 1 ) ) );
            if( nLen > 1 )
                nTextWidth += pDXArry[ nLen - 2 ];
        }
        else
            nTextWidth = aVDev.GetTextWidth( rText );

        if( mnTextAlign & TA_UPDATECP )
            rPosition = maActPos;

        if ( mnTextAlign & TA_RIGHT_CENTER )
        {
            double fLenght = ( ( mnTextAlign & TA_RIGHT_CENTER ) == TA_RIGHT ) ? nTextWidth : nTextWidth >> 1;
            rPosition.X() -= (sal_Int32)( fLenght * cos( maFont.GetOrientation() * F_PI1800 ) );
            rPosition.Y() -= (sal_Int32)(-( fLenght * sin( maFont.GetOrientation() * F_PI1800 ) ) );
        }

        if( mnTextAlign & TA_UPDATECP )
            maActPos.X() = rPosition.X() + nTextWidth;
    }
    if ( bRecordPath )
    {
        // ToDo
    }
    else
    {
        if( pDXArry )
            mpGDIMetaFile->AddAction( new MetaTextArrayAction( rPosition, rText, pDXArry, 0, STRING_LEN ) );
        else
        {
            VirtualDevice   aVDev;
            aVDev.SetMapMode( MapMode( MAP_100TH_MM ) );
            aVDev.SetFont( maFont );
            long* pOwnDx = new long[ rText.Len() ];
            aVDev.GetTextArray( rText, pOwnDx, 0, STRING_LEN );
            mpGDIMetaFile->AddAction( new MetaTextArrayAction( rPosition, rText, pOwnDx, 0, STRING_LEN ) );
            delete[] pOwnDx;
        }
    }
    SetGfxMode( nOldGfxMode );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ImplDrawBitmap( const Point& rPos, const Size& rSize, const BitmapEx rBitmap )
{
    BitmapEx aBmpEx( rBitmap );
    if ( aClipPath.GetType() == COMPLEX )
    {
        VirtualDevice aVDev;
        MapMode aMapMode( MAP_100TH_MM );
        aMapMode.SetOrigin( Point( -rPos.X(), -rPos.Y() ) );
        Size aOutputSizePixel( aVDev.LogicToPixel( rSize, aMapMode ) );
        Size aSizePixel( rBitmap.GetSizePixel() );
        if ( aOutputSizePixel.Width() && aOutputSizePixel.Height() )
        {
            aMapMode.SetScaleX( Fraction( aSizePixel.Width(), aOutputSizePixel.Width() ) );
            aMapMode.SetScaleY( Fraction( aSizePixel.Height(), aOutputSizePixel.Height() ) );
        }
        aVDev.SetMapMode( aMapMode );
        aVDev.SetOutputSizePixel( aSizePixel );
        aVDev.SetFillColor( Color( COL_BLACK ) );
        PolyPolygon aClip( aClipPath.GetClipPath() );
        aVDev.DrawPolyPolygon( aClip );
        Bitmap aMask( aVDev.GetBitmap( rPos, rSize ).CreateMask( Color( COL_WHITE ) ) );
        if ( aBmpEx.IsTransparent() )
        {
            if ( rBitmap.GetTransparentColor() == Color( COL_WHITE ) )
                aMask.CombineSimple( rBitmap.GetMask(), BMP_COMBINE_OR );
            else
                aMask.CombineSimple( rBitmap.GetMask(), BMP_COMBINE_AND );
            aBmpEx = BitmapEx( rBitmap.GetBitmap(), aMask );
        }
        else
            aBmpEx = BitmapEx( rBitmap.GetBitmap(), aMask );
    }
    if ( aBmpEx.IsTransparent() )
        mpGDIMetaFile->AddAction( new MetaBmpExScaleAction( rPos, rSize, aBmpEx ) );
    else
        mpGDIMetaFile->AddAction( new MetaBmpScaleAction( rPos, rSize, aBmpEx.GetBitmap() ) );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ResolveBitmapActions( List& rSaveList )
{
    UpdateClipRegion();

    sal_uInt32 nObjects     = rSaveList.Count();
    sal_uInt32 nObjectsLeft = nObjects;

    while ( nObjectsLeft )
    {
        sal_uInt32      i, nObjectsOfSameSize = 0;
        sal_uInt32      nObjectStartIndex = nObjects - nObjectsLeft;

        BSaveStruct*    pSave = (BSaveStruct*)rSaveList.GetObject( nObjectStartIndex );
        Rectangle       aRect( pSave->aOutRect );

        for ( i = nObjectStartIndex; i < nObjects; )
        {
            nObjectsOfSameSize++;
            if ( ++i < nObjects )
            {
                pSave = (BSaveStruct*)rSaveList.GetObject( i );
                if ( pSave->aOutRect != aRect )
                    break;
            }
        }
        Point   aPos( ImplMap( aRect.TopLeft() ) );
        Size    aSize( ImplMap( aRect.GetSize() ) );

        for ( i = nObjectStartIndex; i < ( nObjectStartIndex + nObjectsOfSameSize ); i++ )
        {
            pSave = (BSaveStruct*)rSaveList.GetObject( i );

            sal_uInt32  nWinRop = pSave->nWinRop;
            sal_uInt8   nRasterOperation = (sal_uInt8)( nWinRop >> 16 );

            sal_uInt32  nUsed =  0;
            if ( ( nRasterOperation & 0xf )  != ( nRasterOperation >> 4 ) )
                nUsed |= 1;     // pattern is used
            if ( ( nRasterOperation & 0x33 ) != ( ( nRasterOperation & 0xcc ) >> 2 ) )
                nUsed |= 2;     // source is used
            if ( ( nRasterOperation & 0xaa ) != ( ( nRasterOperation & 0x55 ) << 1 ) )
                nUsed |= 4;     // destination is used

            if ( (nUsed & 1) && (( nUsed & 2 ) == 0) )
            {   // patterns aren't well supported yet
                sal_uInt32 nOldRop = SetRasterOp( ROP_OVERPAINT );  // in this case nRasterOperation is either 0 or 0xff
                UpdateFillStyle();
                DrawRect( aRect, FALSE );
                SetRasterOp( nOldRop );
            }
            else
            {
                sal_Bool bDrawn = sal_False;

                if ( i == nObjectStartIndex )   // optimizing, sometimes it is possible to create just one transparent bitmap
                {
                    if ( nObjectsOfSameSize == 2 )
                    {
                        BSaveStruct* pSave2 = (BSaveStruct*)rSaveList.GetObject( i + 1 );
                        if ( ( nWinRop == SRCPAINT ) && ( pSave2->nWinRop == SRCAND ) )
                        {
                            if ( ( pSave->aBmp.GetPrefSize() == pSave2->aBmp.GetPrefSize() ) &&
                                    ( pSave->aBmp.GetPrefMapMode() == pSave2->aBmp.GetPrefMapMode() ) )
                            {
                                Bitmap aMask( pSave->aBmp ); aMask.Invert();
                                BitmapEx aBmpEx( pSave2->aBmp, aMask );
                                ImplDrawBitmap( aPos, aSize, aBmpEx );
                                bDrawn = sal_True;
                                i++;
                            }
                        }
                    }
                }

                if ( !bDrawn )
                {
                    Push();
                    sal_uInt32  nOldRop = SetRasterOp( R2_COPYPEN );
                    Bitmap      aBitmap( pSave->aBmp );
                    sal_uInt32  nOperation = ( nRasterOperation & 0xf );
                    switch( nOperation )
                    {
                        case 0x1 :
                        case 0xe :
                        {
                            SetRasterOp( R2_XORPEN );
                            ImplDrawBitmap( aPos, aSize, aBitmap );
                            SetRasterOp( R2_COPYPEN );
                            Bitmap  aMask( aBitmap );
                            aMask.Invert();
                            BitmapEx aBmpEx( aBitmap, aMask );
                            ImplDrawBitmap( aPos, aSize, aBmpEx );
                            if ( nOperation == 0x1 )
                            {
                                SetRasterOp( R2_NOT );
                                DrawRect( aRect, FALSE );
                            }
                        }
                        break;
                        case 0x7 :
                        case 0x8 :
                        {
                            Bitmap  aMask( aBitmap );
                            if ( ( nUsed & 1 ) && ( nRasterOperation & 0xb0 ) == 0xb0 )     // pattern used
                            {
                                aBitmap.Convert( BMP_CONVERSION_24BIT );
                                aBitmap.Erase( maFillStyle.aFillColor );
                            }
                            BitmapEx aBmpEx( aBitmap, aMask );
                            ImplDrawBitmap( aPos, aSize, aBmpEx );
                            if ( nOperation == 0x7 )
                            {
                                SetRasterOp( R2_NOT );
                                DrawRect( aRect, FALSE );
                            }
                        }
                        break;

                        case 0x4 :
                        case 0xb :
                        {
                            SetRasterOp( R2_NOT );
                            DrawRect( aRect, FALSE );
                            SetRasterOp( R2_COPYPEN );
                            Bitmap  aMask( aBitmap );
                            aBitmap.Invert();
                            BitmapEx aBmpEx( aBitmap, aMask );
                            ImplDrawBitmap( aPos, aSize, aBmpEx );
                            SetRasterOp( R2_XORPEN );
                            ImplDrawBitmap( aPos, aSize, aBitmap );
                            if ( nOperation == 0xb )
                            {
                                SetRasterOp( R2_NOT );
                                DrawRect( aRect, FALSE );
                            }
                        }
                        break;

                        case 0x2 :
                        case 0xd :
                        {
                            Bitmap  aMask( aBitmap );
                            aMask.Invert();
                            BitmapEx aBmpEx( aBitmap, aMask );
                            ImplDrawBitmap( aPos, aSize, aBmpEx );
                            SetRasterOp( R2_XORPEN );
                            ImplDrawBitmap( aPos, aSize, aBitmap );
                            if ( nOperation == 0xd )
                            {
                                SetRasterOp( R2_NOT );
                                DrawRect( aRect, FALSE );
                            }
                        }
                        break;
                        case 0x6 :
                        case 0x9 :
                        {
                            SetRasterOp( R2_XORPEN );
                            ImplDrawBitmap( aPos, aSize, aBitmap );
                            if ( nOperation == 0x9 )
                            {
                                SetRasterOp( R2_NOT );
                                DrawRect( aRect, FALSE );
                            }
                        }
                        break;

                        case 0x0 :  // WHITENESS
                        case 0xf :  // BLACKNESS
                        {                                                   // in this case nRasterOperation is either 0 or 0xff
                            maFillStyle = WinMtfFillStyle( Color( nRasterOperation, nRasterOperation, nRasterOperation ) );
                            UpdateFillStyle();
                            DrawRect( aRect, FALSE );
                        }
                        break;

                        case 0x3 :  // only source is used
                        case 0xc :
                        {
                            if ( nRasterOperation == 0x33 )
                                aBitmap.Invert();
                            ImplDrawBitmap( aPos, aSize, aBitmap );
                        }
                        break;

                        case 0x5 :  // only destination is used
                        {
                            SetRasterOp( R2_NOT );
                            DrawRect( aRect, FALSE );
                        }
                        case 0xa :  // no operation
                        break;
                    }
                    SetRasterOp( nOldRop );
                    Pop();
                }
            }
        }
        nObjectsLeft -= nObjectsOfSameSize;
    }

    void* pPtr;
    for ( pPtr = rSaveList.First(); pPtr; pPtr = rSaveList.Next() )
        delete (BSaveStruct*)pPtr;
    rSaveList.Clear();
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetDevOrg( const Point& rPoint )
{
    mnDevOrgX = rPoint.X();
    mnDevOrgY = rPoint.Y();
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetDevOrgOffset( INT32 nXAdd, INT32 nYAdd )
{
    mnDevOrgX += nXAdd;
    mnDevOrgY += nYAdd;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetDevExt( const Size& rSize )
{
    if ( rSize.Width() && rSize.Height() )
    {
        switch( mnMapMode )
        {
            case MM_ISOTROPIC :
            case MM_ANISOTROPIC :
            {
                mnDevWidth = rSize.Width();
                mnDevHeight = rSize.Height();
            }
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ScaleDevExt( double fX, double fY )
{
    mnDevWidth = FRound( mnDevWidth * fX );
    mnDevHeight = FRound( mnDevHeight * fY );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetWinOrg( const Point& rPoint )
{
    mnWinOrgX = rPoint.X();
    mnWinOrgY = rPoint.Y();
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetWinOrgOffset( INT32 nXAdd, INT32 nYAdd )
{
    mnWinOrgX += nXAdd;
    mnWinOrgY += nYAdd;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetWinExt( const Size& rSize )
{

    if( rSize.Width() && rSize.Height() )
    {
        switch( mnMapMode )
        {
            case MM_ISOTROPIC :
            case MM_ANISOTROPIC :
            {
                mnWinExtX = rSize.Width();
                mnWinExtY = rSize.Height();
            }
        }
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ScaleWinExt( double fX, double fY )
{
    mnWinExtX = FRound( mnWinExtX * fX );
    mnWinExtY = FRound( mnWinExtY * fY );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetrclBounds( const Rectangle& rRect )
{
    mrclBounds = rRect;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetrclFrame( const Rectangle& rRect )
{
    mrclFrame = rRect;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetRefPix( const Size& rSize )
{
    mnPixX = rSize.Width();
    mnPixY = rSize.Height();
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetRefMill( const Size& rSize )
{
    mnMillX = rSize.Width();
    mnMillY = rSize.Height();
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetMapMode( sal_uInt32 nMapMode )
{
    mnMapMode = nMapMode;
    if ( nMapMode == MM_TEXT )
    {
        mnWinExtX = mnDevWidth;
        mnWinExtY = mnDevHeight;
    }
    else if ( mnMapMode == MM_HIMETRIC )
    {
        mnWinExtX = mnMillX * 100;
        mnWinExtY = mnMillY * 100;
    }
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::SetWorldTransform( const XForm& rXForm )
{
    maXForm.eM11 = rXForm.eM11;
    maXForm.eM12 = rXForm.eM12;
    maXForm.eM21 = rXForm.eM21;
    maXForm.eM22 = rXForm.eM22;
    maXForm.eDx = rXForm.eDx;
    maXForm.eDy = rXForm.eDy;
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::ModifyWorldTransform( const XForm& rXForm, UINT32 nMode )
{
    switch( nMode )
    {
        case MWT_IDENTITY :
        {
            maXForm.eM11 = maXForm.eM12 = maXForm.eM21 = maXForm.eM22 = 1.0f;
            maXForm.eDx = maXForm.eDx = 0.0f;
        }
        break;

        case MWT_RIGHTMULTIPLY :
        case MWT_LEFTMULTIPLY :
        {
            const XForm* pLeft;
            const XForm* pRight;

            if ( nMode == MWT_LEFTMULTIPLY )
            {
                pLeft = &rXForm;
                pRight = &maXForm;
            }
            else
            {
                pLeft = &maXForm;
                pRight = &rXForm;
            }

            float aF[3][3];
            float bF[3][3];
            float cF[3][3];

            aF[0][0] = pLeft->eM11;
            aF[0][1] = pLeft->eM12;
            aF[0][2] = 0;
            aF[1][0] = pLeft->eM21;
            aF[1][1] = pLeft->eM22;
            aF[1][2] = 0;
            aF[2][0] = pLeft->eDx;
            aF[2][1] = pLeft->eDy;
            aF[2][2] = 1;

            bF[0][0] = pRight->eM11;
            bF[0][1] = pRight->eM12;
            bF[0][2] = 0;
            bF[1][0] = pRight->eM21;
            bF[1][1] = pRight->eM22;
            bF[1][2] = 0;
            bF[2][0] = pRight->eDx;
            bF[2][1] = pRight->eDy;
            bF[2][2] = 1;

            int i, j, k;
            for ( i = 0; i < 3; i++ )
            {
              for ( j = 0; j < 3; j++ )
              {
                 cF[i][j] = 0;
                 for ( k = 0; k < 3; k++ )
                    cF[i][j] += aF[i][k] * bF[k][j];
              }
            }
            maXForm.eM11 = cF[0][0];
            maXForm.eM12 = cF[0][1];
            maXForm.eM21 = cF[1][0];
            maXForm.eM22 = cF[1][1];
            maXForm.eDx = cF[2][0];
            maXForm.eDy = cF[2][1];
        }
        break;
    }
 }

//-----------------------------------------------------------------------------------

void WinMtfOutput::Push()                       // !! to be able to access the original ClipRegion it
{                                               // is not allowed to use the MetaPushAction()
    UpdateClipRegion();                         // (the original clip region is on top of the stack) (SJ)
    SaveStruct* pSave = new SaveStruct;

    pSave->aLineStyle = maLineStyle;
    pSave->aFillStyle = maFillStyle;

    pSave->aFont = maFont;
    pSave->aTextColor = maTextColor;
    pSave->nTextAlign = mnTextAlign;
    pSave->nMapMode = mnMapMode;
    pSave->nGfxMode = mnGfxMode;
    pSave->nBkMode = mnBkMode;
    pSave->aBkColor = maBkColor;

    pSave->aActPos = maActPos;
    pSave->aXForm = maXForm;
    pSave->eRasterOp = meRasterOp;

    pSave->nWinOrgX = mnWinOrgX;
    pSave->nWinOrgY = mnWinOrgY;
    pSave->nWinExtX = mnWinExtX;
    pSave->nWinExtY = mnWinExtY;
    pSave->nDevOrgX = mnDevOrgX;
    pSave->nDevOrgY = mnDevOrgY;
    pSave->nDevWidth = mnDevWidth;
    pSave->nDevHeight = mnDevHeight;

    pSave->aPathObj = aPathObj;
    pSave->aClipPath = aClipPath;
    maSaveStack.Push( pSave );
}

//-----------------------------------------------------------------------------------

void WinMtfOutput::Pop()
{
    // Die aktuellen Daten vom Stack holen
    if( maSaveStack.Count() )
    {
        // Die aktuelle Daten auf dem Stack sichern
        SaveStruct* pSave = maSaveStack.Pop();

        maLineStyle = pSave->aLineStyle;
        maFillStyle = pSave->aFillStyle;

        maFont = pSave->aFont;
        maTextColor = pSave->aTextColor;
        mnTextAlign = pSave->nTextAlign;
        mnBkMode = pSave->nBkMode;
        mnGfxMode = pSave->nGfxMode;
        mnMapMode = pSave->nMapMode;
        maBkColor = pSave->aBkColor;

        maActPos = pSave->aActPos;
        maXForm = pSave->aXForm;
        meRasterOp = pSave->eRasterOp;

        mnWinOrgX = pSave->nWinOrgX;
        mnWinOrgY = pSave->nWinOrgY;
        mnWinExtX = pSave->nWinExtX;
        mnWinExtY = pSave->nWinExtY;
        mnDevOrgX = pSave->nDevOrgX;
        mnDevOrgY = pSave->nDevOrgY;
        mnDevWidth = pSave->nDevWidth;
        mnDevHeight = pSave->nDevHeight;

        aPathObj = pSave->aPathObj;
        if ( ! ( aClipPath == pSave->aClipPath ) )
        {
            aClipPath = pSave->aClipPath;
            aClipPath.bNeedsUpdate = sal_True;
        }
        if ( meLatestRasterOp != meRasterOp )
            mpGDIMetaFile->AddAction( new MetaRasterOpAction( meRasterOp ) );
        delete pSave;
    }
}

