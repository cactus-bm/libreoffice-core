/*************************************************************************
 *
 *  $RCSfile: salgdi3.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:05:43 $
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

#define _SV_SALGDI3_CXX

// -=-= #includes =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <alloca.h>

#include <salunx.h>

#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif
#ifndef _SV_SALDISP_HXX
#include <saldisp.hxx>
#endif
#ifndef _SV_SALGDI_HXX
#include <salgdi.hxx>
#endif
#ifndef _SV_OUTFONT_HXX
#include <outfont.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _SV_POLY_HXX
#include <poly.hxx>
#endif
#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif

#include <tools/debug.hxx>
#include <tools/stream.hxx>

#ifndef PRINTER_DUMMY
#define Font XLIB_Font
#define Region XLIB_Region
#include <xprinter/xp.h>
#undef Font
#undef Region
#endif

#ifndef ANSI1252_HXX_
#include "ansi1252.hxx"
#endif
#ifndef XLFD_ATTRIBUTE_HXX
#include "xlfd_attr.hxx"
#endif
#ifndef XLFD_SIMPLE_HXX
#include "xlfd_smpl.hxx"
#endif
#ifndef XLFD_EXTENDED_HXX
#include "xlfd_extd.hxx"
#endif
#ifndef SAL_CONVERTER_CACHE_HXX_
#include "salcvt.hxx"
#endif
#include <osl/types.h>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef PRINTER_DUMMY
static void
FaxPhoneComment( Display* pDisplay, const sal_Unicode* pStr, USHORT nLen )
{
    #define FAX_PHONE_TOKEN          "@@#"
    #define FAX_PHONE_TOKEN_LENGTH   3
    #define FAX_END_TOKEN            "@@"

    USHORT nPos;
    ByteString aPhone( pStr, nLen, gsl_getSystemTextEncoding() );

    static ByteString aPhoneNumber;
    static BOOL   bIsCollecting = FALSE;

    if( ! bIsCollecting )
    {
        if( ( nPos = aPhone.Search( FAX_PHONE_TOKEN ) ) != STRING_NOTFOUND )
        {
            aPhone.Erase( 0, nPos + FAX_PHONE_TOKEN_LENGTH );
            bIsCollecting = TRUE;
            aPhoneNumber.Erase();
        }
    }
    if( bIsCollecting )
    {
        if( ( nPos = aPhone.Search( FAX_END_TOKEN ) ) != STRING_NOTFOUND )
        {
            aPhone.Erase( nPos );
            bIsCollecting = FALSE;
        }
        aPhoneNumber += aPhone;
        if( ! bIsCollecting )
        {
            aPhone = "PhoneNumber(";
            aPhone += aPhoneNumber;
            aPhone += ")\n";
            XpPSComment( pDisplay, aPhone.GetBuffer() );
            aPhoneNumber = ByteString();
        }
    }
    if( aPhoneNumber.Len() > 1024 )
    {
        bIsCollecting = FALSE;
        aPhoneNumber = ByteString();
    }
}
#endif

// ----------------------------------------------------------------------------
//
// SalDisplay
//
// ----------------------------------------------------------------------------

XlfdStorage*
SalDisplay::GetXlfdList()
{
    if ( mpFontList != NULL )
    {
        return mpFontList;
    }
    else
    {
        // on a display an xlfd of *-0-0-75-75-* means this is a scalable
        // bitmap font, thus it is ugly and thus to avoid. On a printer
        // *-0-0-300-300-* means this is a printer resident font thus nice
        // thus to prefer :-(
        eDeviceT eDevice = this->IsDisplay() ? eDeviceDisplay : eDevicePrinter;

        mpFactory  = new AttributeProvider( eDevice );
        mpFontList = new XlfdStorage();

        int i, nFontCount;
        const int nMaxCount  = 64 * 1024 - 1;
        Display *pDisplay = GetDisplay();
        char **ppFontList = XListFonts(pDisplay, "-*", nMaxCount, &nFontCount);

        //
        // create a list of simple Xlfd font information
        //

        Xlfd  *pXlfdList = (Xlfd*)malloc( nFontCount * sizeof(Xlfd) );
        int    nXlfdCount = 0;

        for ( i = 0; i < nFontCount; i++ )
        {
            if ( pXlfdList[ nXlfdCount ].FromString(ppFontList[i], mpFactory) )
                ++nXlfdCount;
        }

        XFreeFontNames( ppFontList );

        // classification information is needed for sorting, classification
        // of charset (i.e. iso8859-1 <-> ansi-1252) depends on wether the
        // display points to a printer or to a real display. On a printer all
        // iso8859-1 fonts are really capable of ansi-1252
        mpFactory->AddClassification();
        // add some pretty print description
        mpFactory->AddAnnotation();
        // misc feature checking
        mpFactory->TagFeature();

        // sort according to font style
        qsort( pXlfdList, nXlfdCount, sizeof(Xlfd), XlfdCompare );

        //
        // create a font list with merged encoding information
        //

        BitmapXlfdStorage   aBitmapList;
        ScalableXlfd       *pScalableFont = NULL;
        PrinterFontXlfd    *pPrinterFont  = NULL;

        int nFrom = 0;
        for ( i = 0; i < nXlfdCount; i++ )
        {
            // exclude openlook glyph and cursor
            Attribute *pAttr = mpFactory->RetrieveFamily(pXlfdList[i].mnFamily);
            if ( pAttr->HasFeature(   XLFD_FEATURE_OL_GLYPH
                                    | XLFD_FEATURE_OL_CURSOR) )
            {
                continue;
            }
            // exclude fonts with unknown encoding
            if ( pXlfdList[i].GetEncoding() == RTL_TEXTENCODING_DONTKNOW )
            {
                continue;
            }

            Bool bSameOutline = pXlfdList[i].SameFontoutline(pXlfdList + nFrom);
            XlfdFonttype eType = pXlfdList[i].Fonttype();

            // flush the old merged font list if the name doesn't match any more
            if ( !bSameOutline )
            {
                mpFontList->Add( pScalableFont ); pScalableFont = NULL;
                mpFontList->Add( pPrinterFont );  pPrinterFont  = NULL;
                mpFontList->Add( &aBitmapList );  aBitmapList.Reset();
            }

            // merge the font or generate a new one
            switch( eType )
            {
                case eTypeScalable:

                    if ( pScalableFont == NULL )
                        pScalableFont = new ScalableXlfd;
                    pScalableFont->AddEncoding(pXlfdList + i);

                    break;

                case eTypeBitmap:

                    aBitmapList.AddBitmapFont( pXlfdList + i );

                    break;

                case eTypePrinterBuiltIn:
                case eTypePrinterDownload:

                    if ( pPrinterFont == NULL )
                        pPrinterFont = new PrinterFontXlfd;
                    pPrinterFont->AddEncoding( pXlfdList + i );

                    break;

                case eTypeScalableBitmap:
                default:

                    break;
            }

            nFrom = i;
        }

        // flush the merged list into the global list
        mpFontList->Add( pScalableFont );
        mpFontList->Add( pPrinterFont );
        mpFontList->Add( &aBitmapList );

        // cleanup the list of simple font information
        if ( pXlfdList != NULL )
            free( pXlfdList );

        return mpFontList;
    }
}

// ---------------------------------------------------------------------------

ExtendedFontStruct*
SalDisplay::GetFont( ExtendedXlfd *pRequestedFont, int nPixelSize )
{
    if( !pFontCache_ )
    {
        mpCvtCache = new SalConverterCache;
        pFontCache_ = new SalFontCache( 64, 64, 16 ); // ???
    }
    else
    {
        ExtendedFontStruct *pItem;
        for ( pItem  = pFontCache_->First();
              pItem != NULL;
              pItem  = pFontCache_->Next() )
        {
            if ( pItem->Match(pRequestedFont, nPixelSize) )
            {
                if( pFontCache_->GetCurPos() )
                {
                    pFontCache_->Remove( pItem );
                    pFontCache_->Insert( pItem, 0UL );
                }
                return pItem;
            }
        }
    }

    // before we expand the cache, we look for very old and unused items
    if( pFontCache_->Count() >= 64 )
    {
        ExtendedFontStruct *pItem;
        for ( pItem = pFontCache_->Last();
              pItem != NULL;
              pItem = pFontCache_->Prev() )
        {
            if( 1 == pItem->GetRefCount() )
            {
                pFontCache_->Remove( pItem );
                pItem->ReleaseRef();

                if( pFontCache_->Count() < 64 )
                    break;
            }
        }
    }

    ExtendedFontStruct *pItem = new ExtendedFontStruct( GetDisplay(),
                                    nPixelSize, pRequestedFont, mpCvtCache );
    pFontCache_->Insert( pItem, 0UL );
    pItem->AddRef();

    return pItem;
}

// ---------------------------------------------------------------------------

void
SalDisplay::DestroyFontCache()
{
    if( pFontCache_ )
    {
        ExtendedFontStruct *pItem = pFontCache_->First();
        while( pItem )
        {
            delete pItem;
            pItem = pFontCache_->Next();
        }
        delete pFontCache_;
    }
    if( mpFontList )
    {
        mpFontList->Dispose();
        delete mpFontList;
    }
    if ( mpFactory )
    {
        delete mpFactory;
    }
    if ( mpCvtCache )
    {
        delete mpCvtCache;
    }

    pFontCache_ = (SalFontCache*)NULL;
    mpFontList = (XlfdStorage*)NULL;
    mpFactory  = (AttributeProvider*)NULL;
    mpCvtCache = (SalConverterCache*)NULL;
}

// ----------------------------------------------------------------------------
//
// SalGraphicsData
//
// ----------------------------------------------------------------------------

GC
SalGraphicsData::SelectFont()
{
    Display *pDisplay = GetXDisplay();

    if( !pFontGC_ )
    {
        XGCValues values;
        values.subwindow_mode       = IncludeInferiors;
        values.fill_rule            = EvenOddRule;      // Pict import/ Gradient
        values.graphics_exposures   = True;
        values.foreground           = nTextPixel_;

        pFontGC_ = XCreateGC( pDisplay, hDrawable_,
                                GCSubwindowMode | GCFillRule
                              | GCGraphicsExposures | GCForeground,
                              &values );
    }

    if( !bFontGC_ )
    {
        XSetForeground( pDisplay, pFontGC_, nTextPixel_ );
        SetClipRegion( pFontGC_ );
        bFontGC_ = TRUE;
    }

    return pFontGC_;
}

//--------------------------------------------------------------------------

// Select the max size of a font, which is token for real
// This routine is (and should be) called only once, the result should be
// stored in some static variable

static int
GetMaxFontHeight()
{
    #define DEFAULT_MAXFONTHEIGHT 250

    int  nMaxFontHeight = 0;

    char *FontHeight = getenv ("SAL_MAXFONTHEIGHT");
    if (FontHeight)
        nMaxFontHeight = atoi (FontHeight);

    if (nMaxFontHeight <= 0)
        nMaxFontHeight = DEFAULT_MAXFONTHEIGHT;

    return nMaxFontHeight;
}

void
SalGraphicsData::SetFont( const ImplFontSelectData *pEntry )
{
    bFontGC_    = FALSE;
    xFont_      = NULL; // ->ReleaseRef()
    aScale_     = Fraction( 1, 1 );
    nFontOrientation_ = pEntry->mnOrientation;

    if( pEntry->mpFontData && pEntry->mpFontData->mpSysData )
    {
        ExtendedXlfd *pSysFont = (ExtendedXlfd*)pEntry->mpFontData->mpSysData;
        static int nMaxFontHeight = GetMaxFontHeight();

        USHORT         nH, nW;
        if( bWindow_ )
        {
            // see BugId #44528# FontWork (-> #45038#) and as well Bug #47127#
            if( pEntry->mnHeight > nMaxFontHeight )
                nH = nMaxFontHeight;
            else if( pEntry->mnHeight > 2 )
                nH = pEntry->mnHeight;
            else
                nH = 2;
            nW = 0; // pEntry->mnWidth;
        }
        else
        {
            nH = pEntry->mnHeight;
            nW = pEntry->mnWidth;
        }

        xFont_ = GetDisplay()->GetFont( pSysFont, nH );
        if( pEntry->mnHeight > nMaxFontHeight || pEntry->mnHeight < 2 )
            aScale_ = Fraction( pEntry->mnHeight, nH );
    }
    else
    {
        #ifdef DEBUG
        // XXX Fix me: provide a fallback for poor font installations
        // we may be reach this if no font matches the GUI font
        // MS Sans Serif;Geneva;Helv;WarpSans;Dialog;Lucida; ... */
        fprintf( stderr, "SalGraphicsData::SetFont: Invalid Font Selection\n" );
        #endif
    }
}

//--------------------------------------------------------------------------

static sal_Unicode
SwapBytes( const sal_Unicode nIn )
{
    return ((nIn >> 8) & 0x00ff) | ((nIn << 8) & 0xff00);
}


// draw string in a specific multibyte encoding
static void
ConvertTextItem16( XTextItem16* pTextItem,
        SalConverterCache* pCvt, rtl_TextEncoding nEncoding )
{
    if ( pTextItem && pTextItem->nchars > 0 )
    {
        // convert the string into the font encoding
        sal_Size  nSize;
        sal_Size  nBufferSize = pTextItem->nchars * 2;
        sal_Char *pBuffer = (sal_Char*)alloca( nBufferSize );

        nSize = ConvertStringUTF16( (sal_Unicode*)pTextItem->chars, pTextItem->nchars,
                        pBuffer, nBufferSize, pCvt->GetU2TConverter(nEncoding));

        sal_Char *pTextChars = (sal_Char*)pTextItem->chars;
        int n = 0, m = 0;

        if (   nEncoding == RTL_TEXTENCODING_GB_2312
            || nEncoding == RTL_TEXTENCODING_GBT_12345
            || nEncoding == RTL_TEXTENCODING_GBK
            || nEncoding == RTL_TEXTENCODING_BIG5 )
        {
            // GB and Big5 needs special treatment since chars can be single or
            // double byte: encoding is
            // [ 0x00 - 0x7f ] | [ 0x81 - 0xfe ] [ 0x40 - 0x7e 0x80 - 0xfe ]
            while ( n < nSize )
            {
                if ( (unsigned char)pBuffer[ n ] < 0x80 )
                {
                    pTextChars[ m++ ] = 0x0;
                    pTextChars[ m++ ] = pBuffer[ n++ ];
                }
                else
                {
                    pTextChars[ m++ ] = pBuffer[ n++ ];
                    pTextChars[ m++ ] = pBuffer[ n++ ];
                }
            }
            pTextItem->nchars = m / 2;
        }
        else
        if ( pCvt->IsSingleByteEncoding(nEncoding) )
        {
            // Single Byte encoding has to be padded
            while ( n < nSize )
            {
                pTextChars[ m++ ] = 0x0;
                pTextChars[ m++ ] = pBuffer[ n++ ];
            }
            pTextItem->nchars = nSize;
        }
        else
        {
            while ( n < nSize )
            {
                pTextChars[ m++ ] = pBuffer[ n++ ];
            }
            pTextItem->nchars = nSize / 2;
        }
    }
}

// XXX this is a hack since XPrinter is not multibyte capable
// XXX for printing this routine is called for each character
void
XPrinterDrawText16( Display* pDisplay, Drawable nDrawable, GC nGC,
        int nX, int nY, int nAngle, XTextItem16 *pTextItem16, int nItem )
{
    // convert XTextItem16 to XTextItem
    XTextItem *pTextItem = (XTextItem*)alloca( nItem * sizeof(XTextItem) );

    for ( int nCurItem = 0; nCurItem < nItem; nCurItem++ )
    {
        int      nChars      = pTextItem16[ nCurItem ].nchars;
        char*    pDstCharPtr = (char*)alloca( nChars * sizeof(char) );
        XChar2b* pSrcCharPtr = pTextItem16[ nCurItem ].chars;

        pTextItem[ nCurItem ].chars  = pDstCharPtr;
        pTextItem[ nCurItem ].nchars = nChars;
        pTextItem[ nCurItem ].delta  = pTextItem16[ nCurItem ].delta;
        pTextItem[ nCurItem ].font   = pTextItem16[ nCurItem ].font;

        for ( int nCurChar = 0;
              nCurChar < nChars;
              nCurChar++, pDstCharPtr++, pSrcCharPtr++ )
        {
            *pDstCharPtr = (char)pSrcCharPtr->byte2;
        }
    }

    if ( nAngle != 0 )
    {
        for ( int nCurItem = 0; nCurItem < nItem; nCurItem++ )
        {
            // XXX FIXME this is broken, because nX and nY is not sufficiently updated
            XSetFont( pDisplay, nGC, pTextItem[ nItem ].font );
            if ( XSalCanDrawRotString(pDisplay, nGC) )
            {
                XSalDrawRotString( pDisplay, nDrawable, nGC, nX, nY,
                    pTextItem[ nCurItem ].chars, pTextItem[ nCurItem ].nchars, nAngle );
            }
            else
            {
                XDrawString( pDisplay, nDrawable, nGC, nX, nY,
                    pTextItem[ nCurItem ].chars, pTextItem[ nCurItem ].nchars );
            }
        }
    }
    else
    {
        XDrawText( pDisplay, nDrawable, nGC, nX, nY, pTextItem, nItem );
    }
}

// draw string in one of the fonts / encodings that are available in the
// extended font
static void
DrawString( Display *pDisplay, Drawable nDrawable, GC nGC,
        int nX, int nY, const sal_Unicode *pStr, int nLength, int nAngle,
        SalConverterCache *pCvt, ExtendedFontStruct *pFont )
{
    // sanity check
    if ( pFont == NULL || nLength == 0 )
        return;

    rtl_TextEncoding nAsciiEnc = pFont->GetAsciiEncoding();

    if ( nAsciiEnc == RTL_TEXTENCODING_UNICODE )
    {
        // plain Unicode, can handle all chars and can be handled straight forward
        XFontStruct* pFontStruct = pFont->GetFontStruct( nAsciiEnc );

        if ( pFontStruct == NULL )
            return;

        XSetFont( pDisplay, nGC, pFontStruct->fid );

        #ifdef OSL_LITENDIAN
        sal_Unicode *pBuffer = (sal_Unicode*)alloca( nLength * sizeof(sal_Unicode) );
        for ( int i = 0; i < nLength; i++ )
            pBuffer[ i ] = SwapBytes(pStr[ i ]) ;
        #else
        sal_Unicode *pBuffer = const_cast<sal_Unicode*>(pStr);
        #endif

        XDrawString16( pDisplay, nDrawable, nGC, nX, nY, (XChar2b*)pBuffer, nLength );
    }
    else
    {
        // convert the string to a XTextItem16 with each item chars having the
        // encoding matching the font of fontid
        XTextItem16 *pTextItem = (XTextItem16*)alloca( nLength * sizeof(XTextItem16) );
        XChar2b *pMBChar = (XChar2b*)alloca( nLength * sizeof(XChar2b) );
        memcpy( pMBChar, pStr, nLength * sizeof(XChar2b) );

        rtl_TextEncoding nEncoding   = nAsciiEnc;
        XFontStruct*     pFontStruct = pFont->GetFontStruct( nEncoding );

        if ( pFontStruct == NULL )
            return;

        for ( int nChar = 0, nItem = -1; nChar < nLength; nChar++ )
        {
            rtl_TextEncoding nOldEnc = nEncoding;
            pFont->GetFontStruct( pStr[nChar], &nEncoding, &pFontStruct, pCvt );

            if ( (nItem != -1) && (pFontStruct->fid == pTextItem[ nItem ].font) )
            {
                pTextItem[ nItem ].nchars += 1;
            }
            else
            {
                if ( nItem != -1 )
                    ConvertTextItem16( &pTextItem[ nItem ], pCvt, nOldEnc );

                ++nItem;

                pTextItem[ nItem ].chars  = pMBChar + nChar;
                pTextItem[ nItem ].delta  = 0;
                pTextItem[ nItem ].font   = pFontStruct->fid;
                pTextItem[ nItem ].nchars = 1;
            }
        }
        ConvertTextItem16( &pTextItem[ nItem ], pCvt, nEncoding );
        ++nItem;

        if ( XSalIsDisplay( pDisplay ) )
            XDrawText16( pDisplay, nDrawable, nGC, nX, nY, pTextItem, nItem );
        else
            XPrinterDrawText16( pDisplay, nDrawable, nGC, nX, nY, nAngle,
                    pTextItem, nItem );
    }
}

//--------------------------------------------------------------------------

void
SalGraphicsData::DrawText( long nX, long nY,
        const sal_Unicode *pStr, USHORT nLen )
{
#ifndef PRINTER_DUMMY
    if( bPrinter_ )
        FaxPhoneComment( GetXDisplay(), pStr, nLen );
#endif

    #ifdef __notdef__
    // XXX Fix me this part is not unicode / multibyte aware

    // Bug: #45670#
    // some monospace ISO8859-1 fonts have a hole between chars 128 and 159
    // some applications assume these characters have also the default width
    if( ! bPrinter_                                 &&
        PITCH_FIXED == xFont_->GetFont()->mePitch   &&
        nLen > 1 )
    {
        XFontStruct *pXFS   = GetFontInfo();
        long         nWidth = xFont_->GetDim()->GetWidth();

        if( xFont_->GetFixedWidth() != nWidth
            || xFont_->GetDefaultWidth() != nWidth )
        {
            unsigned int min_char   = pXFS->min_char_or_byte2;
            unsigned int max_char   = pXFS->max_char_or_byte2;
            XCharStruct *pXCS       = pXFS->per_char - min_char;

            for( USHORT i = 0; i < nLen-1; i++ )
            {
                unsigned int c = ((unsigned char*)pStr)[i];

                long nW = c < min_char || c > max_char || ! pXFS->per_char
                    ? xFont_->GetDefaultWidth()
                    : pXCS[c].width;

                if( nW != nWidth )
                {
                    long *pDXAry = new long[nLen];

                    for( i = 0; i < nLen; i++ )
                        pDXAry[i] = nWidth * (i+1);

                    DrawText( nX, nY, pStr, nLen, pDXAry );

                    delete pDXAry;

                    return;
                }
            }
        }
    }

    #endif /* __notdef__ */

    Display             *pDisplay = GetXDisplay();
    SalConverterCache   *pCvt     = GetDisplay()->GetConverter();
    GC                  pGC       = SelectFont();

    DrawString( pDisplay, hDrawable_, pGC, nX, nY,
            pStr, nLen, nFontOrientation_ * 64 / 10, pCvt, xFont_ );
}

void
SalGraphics::DrawText( long nX, long nY, const xub_Unicode* pStr, USHORT nLen )
{
    maGraphicsData.DrawText( nX, nY, pStr, nLen );
}

//--------------------------------------------------------------------------

static BOOL
CheckNoNegativeCoordinateWorkaround()
{
    /* Motivation: one of our clients uses a Solaris 2.4 X86 system with an
       XServer for the Matrox Mystique graphics card. This board/server
       sometimes does not draw Text with negative x-coordinates into a
       virtual device (for unknown reasons). A stock X-server just clips the
       part in the negative area. */
    static int nCheck = -2;
    if( nCheck == -2 )
    {
        char* pCmp = getenv( "SAL_NO_NEGATIVE_TEXT_OFFSET" );
        if( pCmp && ! strncasecmp( pCmp, "true", 4 ) )
            nCheck = 1;
        else
            nCheck = 0;
    }
    return nCheck ? TRUE : FALSE;
}

void
SalGraphicsData::DrawText(
        long nX, long nY,
        const sal_Unicode* pStr, USHORT nLen, const long* pDXAry )
{
    #ifndef PRINTER_DUMMY
    if( bPrinter_ )
        FaxPhoneComment( GetXDisplay(), pStr, nLen );
    #endif
    GC pGC = SelectFont();

    // workaround for problems with negative coordinates
    long* pTmpAry = NULL;
    if( nX < 0 && CheckNoNegativeCoordinateWorkaround() )
    {
        long nOldX = nX;
        while( nX < 0 )
        {
            nX = nOldX + *pDXAry;
            pStr++, pDXAry++, nLen--;
            if( nLen < 1 )
                return;
        }
        pTmpAry = new long[ nLen ];
        for( int q = 0; q < nLen-1; q++ )
            pTmpAry[q] = pDXAry[q] - ( nX - nOldX );
        pDXAry = pTmpAry;
    }

    // draw every single character
    SalConverterCache *pCvt = GetDisplay()->GetConverter();
    int angle = nFontOrientation_ * 64 / 10;
    Polygon aPolygon(1);
    Point   aOrigin( nX, nY );
    Point   aCharPos;

    DrawString( GetXDisplay(), hDrawable_, pGC,
            aOrigin.X(), aOrigin.Y(), pStr, 1, angle, pCvt, xFont_ );

    for( int i = 1; i < nLen ; i++ )
    {
        aCharPos = Point( aOrigin.X() + pDXAry[ i - 1 ], aOrigin.Y() );
        aPolygon.SetPoint( aCharPos, 0 );
        aPolygon.Rotate( aOrigin, nFontOrientation_ );
        aCharPos = aPolygon.GetPoint( 0 );

        DrawString( GetXDisplay(), hDrawable_, pGC,
                aCharPos.X(), aCharPos.Y(), pStr + i, 1, angle, pCvt, xFont_ );
    }

    if( pTmpAry )
        delete pTmpAry;
}

// ----------------------------------------------------------------------------
//
// SalGraphics
//
// ----------------------------------------------------------------------------

USHORT
SalGraphics::SetFont( ImplFontSelectData *pEntry )
{
    maGraphicsData.SetFont( pEntry );
    return _IsPrinter() ? SAL_SETFONT_USEDRAWTEXTARRAY : 0;
}

// ----------------------------------------------------------------------------

void
SalGraphics::DrawTextArray( long nX, long nY,
        const xub_Unicode *pStr, USHORT nLen, const long *pDXAry )
{
    maGraphicsData.DrawText( nX, nY, pStr, nLen, pDXAry );
}

// ----------------------------------------------------------------------------

void
SalGraphics::SetTextColor( SalColor nSalColor )
{
    if( _GetTextColor() != nSalColor )
    {
        _GetTextColor()     = nSalColor;
        _GetTextPixel()     = _GetPixel( nSalColor );
        _IsFontGC()         = FALSE;
    }
}

// ----------------------------------------------------------------------------

void
SalGraphics::GetDevFontList( ImplDevFontList *pList )
{
    XlfdStorage* pFonts = _GetDisplay()->GetXlfdList();

    for ( int nIdx = 0; nIdx < pFonts->GetCount(); nIdx++ )
    {
        ImplFontData *pFontData = new ImplFontData;
        pFonts->Get(nIdx)->ToImplFontData( pFontData );
        pList->Add( pFontData );
    }
}

// ----------------------------------------------------------------------------

inline long
sal_DivideNeg( long n1, long n2 )
{
    return ( n1 < 0 ) ? (n1 - n2 / 2) / n2 : (n1 + n2 / 2) / n2;
}

void
SalGraphics::GetFontMetric( ImplFontMetricData *pMetric )
{
    ExtendedFontStruct* pFont = maGraphicsData.xFont_;

    if ( pFont != NULL )
    {
        pFont->ToImplFontMetricData( pMetric );

        if( XSalCanDrawRotString( maGraphicsData.GetXDisplay(), None ) )
            pMetric->mnOrientation = maGraphicsData.nFontOrientation_;

        long n;

        n = maGraphicsData.aScale_.GetNumerator();
        if( n != 1 )
        {
            pMetric->mnWidth    *= n;
            pMetric->mnAscent   *= n;
            pMetric->mnDescent  *= n;
            pMetric->mnLeading  *= n;
            pMetric->mnSlant    *= n;
        }

        n = maGraphicsData.aScale_.GetDenominator();
        if( n != 1 )
        {
            pMetric->mnWidth    = Divide( pMetric->mnWidth, n );
            pMetric->mnAscent   = sal_DivideNeg( pMetric->mnAscent,  n );
            pMetric->mnDescent  = sal_DivideNeg( pMetric->mnDescent, n );
            pMetric->mnLeading  = sal_DivideNeg( pMetric->mnLeading, n );
            pMetric->mnSlant    = sal_DivideNeg( pMetric->mnSlant,   n );
        }
    }
}

// ---------------------------------------------------------------------------

static long
InitializeWidthArray( long *pWidthArray, sal_Size nItems, int nValue = 0  )
{
    const long nPrecision = 1;

    for ( int i = 0; i < nItems; i++, pWidthArray++ )
        *pWidthArray = nValue;

    return nPrecision;
}

long
SalGraphics::GetCharWidth( USHORT nChar1, USHORT nChar2, long  *pWidthAry )
{
    // return the precision of the calculated charwidth, e.g. 1000 = 3 digits
    // defaultet to 1 for now
    const long nPrecision = 1;
    int nRequestedWidth = nChar2 - nChar1 + 1;
    int nCharWidth;

    // XXX sanity check, this may happen if no font at all is installed
    // or no system font matches the requirements for the user interface
    if ( maGraphicsData.xFont_ == NULL )
        return InitializeWidthArray( pWidthAry, nRequestedWidth, 12 );

    // the font should know it's metrics best
    SalDisplay *pSalDisplay = maGraphicsData.GetDisplay();

    nCharWidth = maGraphicsData.xFont_->GetCharWidth(
            pSalDisplay->GetConverter(), nChar1, nChar2, pWidthAry );

    // XXX sanity check, this may happen if the font cannot be loaded/queried
    // either because of a garbled fontpath or because of invalid fontfile
    if ( nCharWidth != nRequestedWidth )
        InitializeWidthArray( pWidthAry + nCharWidth,
                nRequestedWidth - nCharWidth );

    // handle internal scaling
    const long nNumerator   = maGraphicsData.aScale_.GetNumerator();
    const long nDenominator = maGraphicsData.aScale_.GetDenominator();
    long *pPtr;
    sal_Unicode nChar;

    if ( nNumerator != 1 )
        for( pPtr = pWidthAry, nChar = nChar1; nChar <= nChar2; nChar++, pPtr++)
            *pPtr *= nNumerator;
    if ( nDenominator != 1 )
        for( pPtr = pWidthAry, nChar = nChar1; nChar <= nChar2; nChar++, pPtr++)
            *pPtr = Divide( *pPtr, nDenominator );

    // return
    return nPrecision;
}

// ---------------------------------------------------------------------------

extern unsigned char TranslateCharName( char* );

ULONG
SalGraphics::GetKernPairs( ULONG nPairs, ImplKernPairData *pKernPairs )
{
    if( ! _IsPrinter() )
        return 0;

    // get pair kerning table ( internal data from xprinter )
    int i, nCurPair=0;

    // XXX Fix me, improve this to be multi encoding aware: merge kern
    // pair list for all encodings available in the xfont
    rtl_TextEncoding nEncoding = maGraphicsData.xFont_->GetAsciiEncoding();
    XFontStruct *pXFS = maGraphicsData.xFont_->GetFontStruct( nEncoding );
    XExtData    *pXES = pXFS->ext_data;

    for( i = 0; pXES && i < 2; i++, pXES = pXES->next );
    if( i < 2 )
        return 0;
    XpPairKernData* pXpPKD = (XpPairKernData*)(pXES->private_data);
    PairKernData*   pPKD   = pXpPKD->pkd;

    for( i = 0, nCurPair=0; i < pXpPKD->numOfPairs; i++ )
    {
        unsigned char c1 = TranslateCharName( pPKD[i].name1 );
        unsigned char c2 = TranslateCharName( pPKD[i].name2 );
        if( c1 && c2 )
        {
            if( pKernPairs && nCurPair < nPairs )
            {
                pKernPairs[ nCurPair ].mnChar1 = c1;
                pKernPairs[ nCurPair ].mnChar2 = c2;
                pKernPairs[ nCurPair ].mnKern =
                        (long)(pPKD[i].xamt * pXpPKD->pointsize / 1000 );
            }
            nCurPair++;
        }
    }

    return nCurPair;
}

// ---------------------------------------------------------------------------

BOOL
SalGraphics::GetGlyphBoundRect( xub_Unicode c,
        long *pX, long *pY, long *pDX, long *pDY )
{
    return FALSE;
}

// ---------------------------------------------------------------------------

ULONG
SalGraphics::GetGlyphOutline( xub_Unicode c,
        USHORT **ppPolySizes, SalPoint **ppPoints, BYTE **ppFlags )
{
    return 0;
}

