/*************************************************************************
 *
 *  $RCSfile: soicon.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: ssa $ $Date: 2001-07-31 08:19:43 $
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

#define _SV_SOICON_CXX

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <salunx.h>

#ifndef _SV_SALDISP_HXX
#include <saldisp.hxx>
#endif
#ifndef _SV_SALBMP_HXX
#include <salbmp.hxx>
#endif
#ifndef _SV_SALBTYPE_HXX
#include <salbtype.hxx>
#endif
#ifndef _SV_IMPBMP_HXX
#include <impbmp.hxx>
#endif
#ifndef _SV_BITMAP_HXX
#include <bitmap.hxx>
#endif
#ifndef _SV_BITMAP_HXX
#include <bitmapex.hxx>
#endif
#ifndef _SV_GRAPH_HXX
#include <graph.hxx>
#endif
#ifndef _SV_CVTGRF_HXX
#include <cvtgrf.hxx>
#endif
#ifndef _SV_STRHELPER_HXX
#include <strhelper.hxx>
#endif
#ifndef _SV_SOICON_HXX
#include <soicon.hxx>
#endif

#include <tools/stream.hxx>
#include <tools/string.hxx>

#include "soicons.inc"

typedef struct {
    int id;                 // 'resource' id
    char **xpmdata[4];      // 4 resolutions
    Pixmap mPixmap[4], mMask[4];    // caches
    } SOICON;

static SOICON soicons[] = {
    {1, _001_star_butterfly_l_xpm, _001_star_butterfly_m_xpm, _001_star_butterfly_s_xpm, _001_star_butterfly_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {2, _002_text_document_l_xpm, _002_text_document_m_xpm, _002_text_document_s_xpm, _002_text_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {3, _003_text_template_l_xpm, _003_text_template_m_xpm, _003_text_template_s_xpm, _003_text_template_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {4, _004_spreadsheet_document_l_xpm, _004_spreadsheet_document_m_xpm, _004_spreadsheet_document_s_xpm, _004_spreadsheet_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {5, _005_spreadsheet_template_l_xpm, _005_spreadsheet_template_m_xpm, _005_spreadsheet_template_s_xpm, _005_spreadsheet_template_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {6, _006_drawing_document_l_xpm, _006_drawing_document_m_xpm, _006_drawing_document_s_xpm, _006_drawing_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {7, _007_drawing_template_l_xpm, _007_drawing_template_m_xpm, _007_drawing_template_s_xpm, _007_drawing_template_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {8, _008_presentation_document_l_xpm, _008_presentation_document_m_xpm, _008_presentation_document_s_xpm, _008_presentation_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {9, _009_presentation_template_l_xpm, _009_presentation_template_m_xpm, _009_presentation_template_s_xpm, _009_presentation_template_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {10, _010_presentation_compressed_l_xpm, _010_presentation_compressed_m_xpm, _010_presentation_compressed_s_xpm, _010_presentation_compressed_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {11, _011_global_document_l_xpm, _011_global_document_m_xpm, _011_global_document_s_xpm, _011_global_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {12, _012_html_document_l_xpm, _012_html_document_m_xpm, _012_html_document_s_xpm, _012_html_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {13, _013_chart_document_l_xpm, _013_chart_document_m_xpm, _013_chart_document_s_xpm, _013_chart_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {14, _014_database_document_l_xpm, _014_database_document_m_xpm, _014_database_document_s_xpm, _014_database_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {15, _015_math_document_l_xpm, _015_math_document_m_xpm, _015_math_document_s_xpm, _015_math_document_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {16, _016_template_l_xpm, _016_template_m_xpm, _016_template_s_xpm, _016_template_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {17, _017_macrolibrary_l_xpm, _017_macrolibrary_m_xpm, _017_macrolibrary_s_xpm, _017_macrolibrary_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {100, _100_player_l_xpm, _100_player_m_xpm, _100_player_s_xpm, _100_player_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {500, _500_setup_l_xpm, _500_setup_m_xpm, _500_setup_s_xpm, _500_setup_t_xpm,
        0, 0, 0, 0, 0, 0, 0, 0},
    {0, NULL, NULL, NULL,
        0, 0, 0, 0, 0, 0, 0, 0}
    };


static void ConvertXpm( SalDisplay* pDisplay, char *xpm[], Pixmap& aPixmap, Pixmap& aMask )
{
    int nWidth, nHeight, nColors, nCharsPerPixel;
    XColor *pColors;
    char *pColorAlias;
    int nElement = 0,nColor = 0,i,nX,nY;
    char pColorString[256];
    BOOL bTransparent = FALSE;

    sscanf( xpm[ nElement++ ], "%d%d%d%d", &nWidth, &nHeight,
            &nColors, &nCharsPerPixel );
#if defined DBG_UTIL || defined DEBUG
    fprintf( stderr, "ConvertXpm: converting width = %d height = %d ncolors = %d chars_per_pixel = %d\n", nWidth, nHeight, nColors, nCharsPerPixel );
#endif
    nColor  = 0;
    pColors = new XColor[ nColors ];
    pColorAlias = new char[ nColors * nCharsPerPixel ];
    while( nElement <= nColors )
    {
        char* pLine = xpm[nElement++];
        char* pStart = pLine + nCharsPerPixel;
        while( *pStart && ( pStart[0] != 'c' || ! isspace( pStart[1] ) ) )
            pStart++;
        if( *pStart )
        {
            sscanf( pStart,"c %s", pColorString);
#ifdef DEBUG
            fprintf(stderr, "pColorString=\"%s\"\n", pColorString );
#endif
            if( strncasecmp( pColorString, "None", 4 ) )
            {
                XAllocNamedColor( pDisplay->GetDisplay(),
                                  DefaultColormap( pDisplay->GetDisplay(),
                                                   pDisplay->GetScreenNumber() ),
                                  pColorString, &pColors[nColor], &pColors[nColor] );
                strncpy( &pColorAlias[nColor*nCharsPerPixel],
                         pLine, nCharsPerPixel );
                nColor++;
            }
        }
    }
    nColors = nColor+1;

    aPixmap = XCreatePixmap( pDisplay->GetDisplay(),
                                pDisplay->GetRootWindow(),
                                nWidth, nHeight,
                                pDisplay->GetRootVisual()->GetDepth() );
    XSetForeground( pDisplay->GetDisplay(),
                    DefaultGC( pDisplay->GetDisplay(),
                               pDisplay->GetScreenNumber() ),
                    BlackPixel( pDisplay->GetDisplay(),
                                pDisplay->GetScreenNumber() ) );
    XFillRectangle( pDisplay->GetDisplay(), aPixmap,
                    DefaultGC( pDisplay->GetDisplay(),
                               pDisplay->GetScreenNumber() ),
                    0,0,nWidth,nHeight );

    aMask   = XCreatePixmap( pDisplay->GetDisplay(),
                                pDisplay->GetRootWindow(),
                                nWidth, nHeight, 1 );

    XGCValues aValues;
    aValues.function   = GXset;
    aValues.foreground = 0xffffffff;
    GC aMonoGC = XCreateGC( pDisplay->GetDisplay(), aMask,
                            GCFunction|GCForeground, &aValues );

    XFillRectangle( pDisplay->GetDisplay(), aMask, aMonoGC,
                    0,0, nWidth, nHeight );
    aValues.function = GXclear;
    XChangeGC( pDisplay->GetDisplay(), aMonoGC, GCFunction, &aValues );

    for( nY=0; nY < nHeight; nY++ )
    {
        char *pRun = xpm[ nElement+nY ];
        for( nX=0; nX < nWidth; nX++ )
        {
            // get color number
            nColor = 0;
            while( nColor < nColors &&
                   strncmp( pRun, &pColorAlias[nColor*nCharsPerPixel],
                            nCharsPerPixel ) )
                nColor++;
            if( nColor < nColors )
            {
                XSetForeground( pDisplay->GetDisplay(),
                                DefaultGC( pDisplay->GetDisplay(),
                                           pDisplay->GetScreenNumber() ),
                                pColors[ nColor ].pixel );
                XDrawPoint( pDisplay->GetDisplay(),
                            aPixmap,
                            DefaultGC( pDisplay->GetDisplay(),
                                       pDisplay->GetScreenNumber() ),
                            nX, nY );

            }
            else
            {
                bTransparent = TRUE;
                XDrawPoint( pDisplay->GetDisplay(),
                            aMask, aMonoGC, nX, nY );
            }
            pRun += nCharsPerPixel;
        }
    }
    delete pColors;
    delete pColorAlias;
    XFreeGC( pDisplay->GetDisplay(), aMonoGC );

    if( ! bTransparent )
    {
#if defined DBG_UTIL || defined DEBUG
        fprintf( stderr, "ConvertXpm: keine Transparenz -> keine Maske\n" );
#endif
        XFreePixmap( pDisplay->GetDisplay(), aMask );
        aMask = 0;
    }
}

BOOL SelectAppIconPixmap( SalDisplay *pDisplay, USHORT nIcon, USHORT iconSize,
                          Pixmap& icon_pixmap, Pixmap& icon_mask)
{
    VCL_CUSTOM_ICON_FN *pCustomIcon;

    if( nIcon == 0 )    // 0 means default icon which is icon #1 actually
        nIcon = 1;

    SOICON *pIcon = soicons;
    while( pIcon->id )
    {
        if( pIcon->id == nIcon )
            break;
           pIcon++;
    }

    if( !pIcon->id || pIcon->id == 1 ) // overwrite default icon
    {
        // call custom function to read icon
        char customIconFn[256];

        sprintf( customIconFn, "%s%d", VCL_CUSTOM_ICON_BASE, nIcon );
        static void* pAppHdl = dlopen( NULL, RTLD_LAZY );
        if ( ( pCustomIcon = ( VCL_CUSTOM_ICON_FN* ) dlsym( pAppHdl, customIconFn ) )
                 != NULL )
        {
            pIcon = new SOICON[2];  // 2nd entry is terminator
            memset( pIcon, 0, 2*sizeof( SOICON ) );
            pIcon->id = nIcon;
            pCustomIcon( pIcon->xpmdata[0], pIcon->xpmdata[1], pIcon->xpmdata[2], pIcon->xpmdata[3] );
        }
    }

    DBG_ASSERT( pIcon->id, "SelectAppIconPixmap: Icon not found!");

    if( !pIcon->id )
        return FALSE;

    // search optimal size
    int i, maxSize = 0, iIcon=-1;
    for(i=0; i<4; i++)
    {
        int nWidth, nHeight, nColors, nCharsPerPixel;
        if( !pIcon->xpmdata[i] )
            continue;
        sscanf( pIcon->xpmdata[i][0], "%d%d%d%d", &nWidth, &nHeight,
                &nColors, &nCharsPerPixel );
        if( iconSize == nWidth )
        {
            iIcon = i;
            break;
        }
        if ( nWidth > maxSize )
        {
            // use largest icon if sizes don't match
            maxSize = nWidth;
            iIcon = i;
        }
    }

    DBG_ASSERT( iIcon != -1, "SelectAppIconPixmap: Invalid icon definition found!");
    if( iIcon == -1 )
    {
        if( pCustomIcon )
            delete [] pIcon;
        return FALSE;
    }

    if( !pIcon->mPixmap[iIcon] )
        ConvertXpm( pDisplay, pIcon->xpmdata[iIcon], pIcon->mPixmap[iIcon], pIcon->mMask[iIcon] );

    icon_pixmap = pIcon->mPixmap[iIcon];
    icon_mask   = pIcon->mMask[iIcon];

    if( pCustomIcon )
        delete [] pIcon;

    return TRUE;
}

static void NextLine( SvStream& rFile, ByteString& rLine, BOOL bXpm2Mode )
{
    if( bXpm2Mode )
    {
        rFile.ReadLine( rLine );
    }
    else
    {
        do
        {
            rFile.ReadLine( rLine );
            rLine = rLine.GetToken( 1, '"' );
            if( rLine.Len() )
                rLine = rLine.GetToken( 0, '"' );
        } while( ! ( rLine.Len() || rFile.IsEof() ) );
    }
}

BOOL ReadXBMFile( Display* pDisplay, const String& rFile, SalBitmap*& rpBmp )
{
    SvFileStream    aFile( rFile, STREAM_READ );
    int             nBytes = 0;
    int             nWidth= -1, nHeight=-1;
    BOOL            bSuccess = FALSE;

    rpBmp = NULL;

    // read in bitmap file ( cause XReadBitmapFileData not present
    // in Solaris 2.5.1)
    if( ! aFile.IsOpen() )
        return FALSE;

    ByteString aLine;

    while( ( nWidth < 0 || nHeight < 0 ) && ! aFile.IsEof() )
    {
        aFile.ReadLine( aLine );
        aLine = WhitespaceToSpace( aLine );

        if( aLine.GetChar(0) == '#' )
        {
            if( aLine.Search( "_width" ) != STRING_NOTFOUND )
                nWidth = aLine.GetToken( ' ', 2 ).ToInt32();
            else if( aLine.Search( "_height" ) != STRING_NOTFOUND )
                nHeight = aLine.GetToken( ' ', 2 ).ToInt32();
        }
    }

    if( nWidth <= 0 || nHeight <= 0 )
        return FALSE;

    BitmapPalette   aPal( 2 );
    const Size      aSize( nWidth, nHeight );

    aPal[ 0 ] = BitmapColor( 0, 0, 0 );
    aPal[ 1 ] = BitmapColor( 0xff, 0xff, 0xff );

    rpBmp = new SalBitmap;
    rpBmp->Create( aSize, 1, aPal );

    BitmapBuffer*   pBmpBuf = rpBmp->AcquireBuffer( FALSE );
    BYTE*           pBmpScan = pBmpBuf->mpBits + ( nHeight - 1 ) * pBmpBuf->mnScanlineSize;
    long            nX = 0, nY = 0;

    aFile.Seek( 0L );

    do
    {
        int nPos;

        aFile.ReadLine( aLine );
        aLine.ToUpperAscii();

        while( ( ( nPos = aLine.Search( "0X" ) ) != STRING_NOTFOUND ) && ( nY < nHeight ) )
        {
            BYTE cData = 0;

            for( int i = 0; i < 2; i++ )
            {
                cData *= 16;
                char c = aLine.GetChar( nPos + 2 + i);

                if( c >= '0' && c <= '9' )
                    cData += (unsigned char)( c - '0' );
                else if( c >= 'A' && c <= 'F' )
                    cData += (unsigned char)( c - 'A' + 10 );
            }

            *pBmpScan++ += ( ( cData & 1    ) << 7 ) | ( ( cData & 2    ) << 5 ) |
                           ( ( cData & 4    ) << 3 ) | ( ( cData & 8    ) << 1 ) |
                           ( ( cData & 16   ) >> 1 ) | ( ( cData & 32   ) >> 3 ) |
                           ( ( cData & 64   ) >> 5 ) | ( ( cData& 128   ) >> 7 );

            if( ( nX += 8 ) >= nWidth )
                nX = 0, pBmpScan = pBmpBuf->mpBits + ( nHeight - ++nY - 1 ) * pBmpBuf->mnScanlineSize;

            aLine.Erase( 0, nPos + 5 );
        }
    }
    while( !aFile.IsEof() && ( nY < nHeight ) );

    rpBmp->ReleaseBuffer( pBmpBuf, FALSE );

    return TRUE;
}

//------------------------------------------------------------------------------

BOOL ReadXPMFile( Display* pDisplay, const String& rFile,
                  SalBitmap*& rpBmp, SalBitmap*& rpMsk )
{
    SvFileStream    aFile( rFile, STREAM_READ );
    ByteString      aColorName, aColorString, aLine;
    int             nWidth, nHeight, nColors, nCharsPerPixel;
    int             nElement = 0,nColor = 0, i, nX, nY;
    UINT8*          pColorTable;
    char*           pColorAlias;
    BOOL            bTransparent = FALSE;
    BOOL            bXpm2Mode = FALSE;

    rpBmp = rpMsk = NULL;

    if( ! aFile.IsOpen() )
        return FALSE;

    aFile.ReadLine( aLine );
    aLine = WhitespaceToSpace( aLine );

    if( aLine.CompareTo( "! XPM", 5 ) == COMPARE_EQUAL )
        bXpm2Mode = TRUE;
    else
    {
        bXpm2Mode = FALSE;
        aFile.Seek( 0L );
    }

    NextLine( aFile, aLine, bXpm2Mode );

    nWidth = GetCommandLineToken( 0, aLine ).ToInt32();
    nHeight = GetCommandLineToken( 1, aLine ).ToInt32();
    nColors = GetCommandLineToken( 2, aLine ).ToInt32();
    nCharsPerPixel = GetCommandLineToken( 3, aLine ).ToInt32();

    if( nWidth == 0 || nHeight == 0 || nColors == 0 || nCharsPerPixel == 0 )
    {
        // not really an xpm despite the name => try to load it via GraphicConverter
        aFile.Seek( 0L );
        Graphic aGraphic;
        GraphicConverter::Import( aFile, aGraphic );

        BitmapEx aBitmapEx( aGraphic.GetBitmapEx() );

        if( aBitmapEx.GetSizePixel().Width() == 0 || aBitmapEx.GetSizePixel().Height() == 0 )
            return FALSE;

        const Bitmap aBmp( aBitmapEx.GetBitmap() );
        const Bitmap aMsk( aBitmapEx.GetMask() );

        if( !!aBmp )
        {
            rpBmp = new SalBitmap;
            rpBmp->Create( *aBmp.ImplGetImpBitmap()->ImplGetSalBitmap() );
        }

        if( !!aMsk )
        {
            rpMsk = new SalBitmap;
            rpMsk->Create( *aMsk.ImplGetImpBitmap()->ImplGetSalBitmap() );
        }

        return TRUE;
    }

    nColor  = 0;
    pColorTable = new UINT8[ (nColors+1) * 3 ];
    pColorAlias = new char[ nColors * nCharsPerPixel ];
    XColor aExactColor;
    aExactColor.flags = DoRed | DoGreen | DoBlue;
    while( nColors )
    {
        NextLine( aFile, aLine, bXpm2Mode );
        // might be a space as color variable
        aColorName = aLine.Copy( 0, nCharsPerPixel );
        aLine.Erase( 0, nCharsPerPixel );
        aLine = WhitespaceToSpace( aLine );

        int nPos = aLine.Search( " c " );
        if( nPos == STRING_NOTFOUND && aLine.CompareIgnoreCaseToAscii( "c ", 2 ) == COMPARE_EQUAL )
            nPos = 2;
        else
            nPos += 3;
        aColorString = aLine.Copy( nPos );

        if( aColorString.CompareIgnoreCaseToAscii( "none", 4 ) != COMPARE_EQUAL )
        {
            XParseColor( pDisplay,
                         DefaultColormap( pDisplay,
                                          DefaultScreen( pDisplay ) ),
                         aColorString.GetBuffer(),
                         &aExactColor );
            pColorTable[ nColor * 3 +2 ] = (UINT8)(aExactColor.red / 256 );
            pColorTable[ nColor * 3 +1 ] = (UINT8)(aExactColor.green / 256 );
            pColorTable[ nColor * 3    ] = (UINT8)(aExactColor.blue / 256 );
            strncpy( pColorAlias + nColor*nCharsPerPixel,
                     aColorName.GetBuffer(), nCharsPerPixel );
            nColor++;
        }
        nColors--;
    }
    pColorTable[ 3*nColor ] = pColorTable[ 3*nColor+1 ] =
        pColorTable[ 3*nColor+2 ] = 0;
    nColors = nColor;

    // read SalBitmap's
    BitmapPalette   aPal( 2 );
    const Size      aSize( nWidth, nHeight );

    aPal[ 0 ] = BitmapColor( 0, 0, 0 );
    aPal[ 1 ] = BitmapColor( 0xff, 0xff, 0xff );

    rpBmp = new SalBitmap;
    rpBmp->Create( aSize, 24, aPal );
    BitmapBuffer* pBmpBuf = rpBmp->AcquireBuffer( FALSE );

    rpMsk = new SalBitmap;
    rpMsk->Create( aSize, 1, aPal );
    BitmapBuffer* pMskBuf = rpMsk->AcquireBuffer( FALSE );

    for( nY=0; nY < nHeight; nY++ )
    {
        NextLine( aFile, aLine, bXpm2Mode );

        const char* pRun = aLine.GetBuffer();
        BYTE*   pBmpScan = pBmpBuf->mpBits + pBmpBuf->mnScanlineSize * ( nHeight - nY - 1 );
        BYTE*   pMskScan = pMskBuf->mpBits + pMskBuf->mnScanlineSize * ( nHeight - nY - 1 );
        UINT8   nMaskByte = 0;

        for( nX = 0; nX < nWidth; nX++ )
        {
            // get color number
            for( nColor=0; nColor < nColors; nColor++ )
            {
                if( ! strncmp( pRun, pColorAlias + nColor*nCharsPerPixel,
                               nCharsPerPixel ) )
                    break;
            }

            long nIndex = nColor * 3;

            *pBmpScan++ = pColorTable[ nIndex++ ];
            *pBmpScan++ = pColorTable[ nIndex++ ];
            *pBmpScan++ = pColorTable[ nIndex ];

            if( nColor == nColors )
                nMaskByte |= ( 1 << ( 7 - ( nX & 7 ) ) );

            pRun += nCharsPerPixel;

            if( ( ( nX & 7 ) == 7 ) || ( nX == nWidth - 1 ) )
            {
                *pMskScan++ = nMaskByte;
                nMaskByte = 0;
            }
        }
    }

    rpBmp->ReleaseBuffer( pBmpBuf, FALSE );
    rpMsk->ReleaseBuffer( pMskBuf, FALSE );
    delete[] pColorTable;
    delete[] pColorAlias;

    return TRUE;
}
