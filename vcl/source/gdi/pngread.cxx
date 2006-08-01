/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pngread.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: ihi $ $Date: 2006-08-01 09:27:51 $
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

#include "pngread.hxx"

#include <cmath>
#include <rtl/crc.h>
#include <rtl/memory.h>
#include <rtl/alloc.h>
#include <tools/zcodec.hxx>
#include <tools/stream.hxx>
#include "bmpacc.hxx"
#include "svapp.hxx"
#include "alpha.hxx"

#ifndef _OSL_ENDIAN_H_
#include <osl/endian.h>
#endif

// -----------
// - Defines -
// -----------

#define PNGCHUNK_IHDR       0x49484452
#define PNGCHUNK_PLTE       0x504c5445
#define PNGCHUNK_IDAT       0x49444154
#define PNGCHUNK_IEND       0x49454e44
#define PNGCHUNK_bKGD       0x624b4744
#define PNGCHUNK_cHRM       0x6348524d
#define PNGCHUNK_gAMA       0x67414d41
#define PNGCHUNK_hIST       0x68495354
#define PNGCHUNK_pHYs       0x70485973
#define PNGCHUNK_sBIT       0x73425420
#define PNGCHUNK_tIME       0x74494d45
#define PNGCHUNK_tEXt       0x74455874
#define PNGCHUNK_tRNS       0x74524e53
#define PNGCHUNK_zTXt       0x7a545874
#define PMGCHUNG_msOG       0x6d734f47      // Microsoft Office Animated GIF

#define PNG_TRANS_VAL       0x1

#define VIEWING_GAMMA       2.5
#define DISPLAY_GAMMA       1.25

namespace vcl
{
// -----------
// - statics -
// -----------

static const BYTE aBlockHeight[ 8 ] =
{
    0, 8, 8, 4, 4, 2, 2, 1
};

// ------------------------------------------------------------------------------

static const BYTE aBlockWidth[ 8 ]  =
{
    0, 8, 4, 4, 2, 2, 1, 1
};

// ------------------------------------------------------------------------------

static const BYTE mpDefaultColorTable[ 256 ] =
{   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

/* ------------------------------------------------------------------------
   SJ: InitChunkSeq reads all PNG chunks. The rStm stream position will be at
   the end of the file afterwards.
*/
const sal_Bool InitChunkSeq( SvStream& rStm, std::vector< vcl::PNGReader::ChunkData >& rChunkSeq )
{
    sal_Bool    bRet = sal_True;
    sal_uInt16  nIStmOldMode = rStm.GetNumberFormatInt();
    rStm.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );

        // check PNG file header magic number
    sal_uInt32 nDummy[ 2 ] = {0, 0};
    rStm >> nDummy[0] >> nDummy[1];
    if( ( nDummy[0] != 0x89504e47 ) || ( nDummy[1] != 0x0d0a1a0a ) )
        bRet = sal_False;
    else
    {
        sal_uInt32 nChunkLen, nCRC32, nCheck, nType = 0;
        while( !rStm.IsEof() && ( rStm.GetError() == ERRCODE_NONE ) )
        {
            rChunkSeq.resize( rChunkSeq.size() + 1 );
            vcl::PNGReader::ChunkData& rChunkData = rChunkSeq.back();
            rStm >> nChunkLen
                 >> nType;

            rChunkData.nType = nType;
            #if defined(__LITTLEENDIAN) || defined(OSL_LITENDIAN)
                nType = SWAPLONG( nType );
            #endif
            nCRC32 = rtl_crc32( 0, &nType, 4 );
            if ( nChunkLen && !rStm.IsEof() )
            {
                rChunkData.aData.resize( nChunkLen );
                sal_uInt8* pPtr = &rChunkData.aData[ 0 ];
                rStm.Read( pPtr, nChunkLen );
                nCRC32 = rtl_crc32( nCRC32, pPtr, nChunkLen );
            }
            rStm >> nCheck;

            if ( rChunkData.nType == PNGCHUNK_IEND )    // SJ: not checking crc for the IEND chunk
                break;                                  // because of I25246

            if ( nCRC32 != nCheck )
            {
                bRet = sal_False;
                break;
            }
        }
        if ( !rChunkSeq.size() || ( rChunkSeq[ 0 ].nType != PNGCHUNK_IHDR ) )
            bRet = sal_False;
    }
    rStm.SetNumberFormatInt( nIStmOldMode );
    return bRet;
}

// -------------
// - PNGReaderImpl -
// -------------

class PNGReaderImpl
{
    friend class vcl::PNGReader;

    std::vector< vcl::PNGReader::ChunkData >    maChunkSeq;
    std::vector< sal_uInt8 >::iterator          maDataIter;
    std::vector< sal_uInt8 >::iterator          maDataEnd;
    sal_Int32                                   mnChunkLen;

    Bitmap*             mpBmp;
    BitmapWriteAccess*  mpAcc;
    Bitmap*             mpMaskBmp;
    AlphaMask*          mpAlphaMask;
    BitmapWriteAccess*  mpMaskAcc;
    ZCodec*             mpZCodec;
    BYTE*               mpInflateInBuf; // as big as the size of a scanline + alphachannel + 1
    BYTE*               mpScanprior;    // pointer to the latest scanline
    BYTE*               mpTransTab;     //
    BYTE*               mpScan;         // pointer in the current scanline
    BYTE*               mpColorTable;   //
    sal_uInt32          mnChunkType;    // Chunk which is currently open
    sal_uInt32          mnWidth;
    sal_uInt32          mnHeight;
    sal_uInt32          mnBBP;          // number of bytes per pixel
    sal_uInt32          mnScansize;     // max size of scanline
    sal_uInt32          mnPrefWidth;    // preferred width in meter
    sal_uInt32          mnPrefHeight;   // preferred Height in meter
    sal_uInt32          mnYpos;         // latest y position;
    USHORT              mnIStmOldMode;
    USHORT              mnDepth;        // pixel depth
    BYTE                mnTransRed;
    BYTE                mnTransGreen;
    BYTE                mnTransBlue;
    BYTE                mnBitDepth;     // sample depth
    BYTE                mnColorType;
    BYTE                mnCompressionType;
    BYTE                mnFilterType;
    BYTE                mnInterlaceType;
    BYTE                mnPass;         // if interlaced the latest pass ( 1..7 ) else 7
    BYTE                cTransIndex1;
    BYTE                cNonTransIndex1;
    BOOL                mbStatus;
    BOOL                mbTransparent;  // graphic includes an tRNS Chunk or an alpha Channel
    BOOL                mbAlphaChannel; // is true for ColorType 4 and 6
    BOOL                mbRGBTriple;
    BOOL                mbPalette;      // FALSE if we need a Palette
    BOOL                mbGrayScale;
    BOOL                mbzCodecInUse;
    BOOL                mbIDAT;         // TRUE if finished with the complete IDAT...
    BOOL                mbGamma;        // TRUE if Gamma Correction available
    BOOL                mbpHYs;         // TRUE if pysical size of pixel available

    void                ImplSetPixel( sal_uInt32 y, sal_uInt32 x, BYTE nPalIndex );
    void                ImplSetPixel( sal_uInt32 y, sal_uInt32 x, const BitmapColor & );
    void                ImplSetPixel( sal_uInt32 y, sal_uInt32 x, const BitmapColor &, BOOL bTrans );
    void                ImplSetAlphaPixel( sal_uInt32 y, sal_uInt32 x, BYTE nPalIndex, BYTE nAlpha );
    void                ImplSetAlphaPixel( sal_uInt32 y, sal_uInt32 x, const BitmapColor&, BYTE nAlpha );
    void                ImplReadIDAT();
    void                ImplResizeScanline();
    void                ImplGetFilter( sal_uInt32 nXStart=0, sal_uInt32 nXAdd=1 );
    void                ImplReadTransparent();
    void                ImplGetGamma();
    void                ImplGetBackground();
    BYTE                ImplScaleColor();
    BOOL                ImplReadHeader();
    BOOL                ImplReadPalette();
    void                ImplGetGrayPalette( sal_uInt16 );
    sal_uInt32          ImplReadsal_uInt32();

public:

                                    PNGReaderImpl( SvStream& rStm );
                                    ~PNGReaderImpl();

    BitmapEx                        Read();
};

// ------------------------------------------------------------------------------

PNGReaderImpl::PNGReaderImpl( SvStream& rPNG ) :
    mpBmp           ( NULL ),
    mpAcc           ( NULL ),
    mpMaskBmp       ( NULL ),
    mpAlphaMask     ( NULL ),
    mpMaskAcc       ( NULL ),
    mpZCodec        ( new ZCodec( DEFAULT_IN_BUFSIZE, DEFAULT_OUT_BUFSIZE, MAX_MEM_USAGE ) ),
    mpInflateInBuf  ( NULL ),
    mpScanprior     ( NULL ),
    mpTransTab      ( NULL ),
    mpColorTable    ( (sal_uInt8*) mpDefaultColorTable ),
    mbzCodecInUse   ( sal_False ),
    mbGamma         ( sal_False ),
    mbpHYs          ( sal_False )
{
    mbStatus = InitChunkSeq( rPNG, maChunkSeq );
}

// ------------------------------------------------------------------------

PNGReaderImpl::~PNGReaderImpl()
{
    if ( mbzCodecInUse )
        mpZCodec->EndCompression();

    if( mpColorTable != mpDefaultColorTable )
        delete[] mpColorTable;

    rtl_freeMemory( mpScanprior );

    delete mpBmp;
    delete mpAlphaMask;
    delete mpMaskBmp;
    delete[] mpTransTab;
    delete[] mpInflateInBuf;
    delete mpZCodec;
}

// ------------------------------------------------------------------------

BitmapEx PNGReaderImpl::Read()
{
    BitmapEx    aRet;

    std::vector< vcl::PNGReader::ChunkData >::iterator aIter( maChunkSeq.begin() );
    std::vector< vcl::PNGReader::ChunkData >::iterator aEnd ( maChunkSeq.end() );

    while( mbStatus && ( aIter != aEnd ) && ( aIter->nType != PNGCHUNK_IEND ) )
    {
        maDataIter = aIter->aData.begin();
        maDataEnd  = aIter->aData.end();
        mnChunkLen = aIter->aData.size();

        switch ( aIter->nType )
        {
            case PNGCHUNK_IHDR :
            {
                mbStatus = ImplReadHeader();
            }
            break;

            case PNGCHUNK_gAMA :                                // the gamma chunk must precede
            {
                if ( mbIDAT == FALSE )                          // the 'IDAT' and also the
                    ImplGetGamma();                             // 'PLTE'(if available )
            }
            break;

            case PNGCHUNK_PLTE :
            {
                if ( !mbPalette )
                    mbStatus = ImplReadPalette();
            }
            break;

            case PNGCHUNK_tRNS :
            {
                if ( !mbIDAT )                                  // the tRNS chunk must precede the IDAT
                    ImplReadTransparent();
            }
            break;

            case PNGCHUNK_bKGD :                                // the background chunk must appear
            {
                if ( ( mbIDAT == FALSE ) && mbPalette )         // before the 'IDAT' and after the
                    ImplGetBackground();                        // PLTE(if available ) chunk.
            }
            break;

            case PNGCHUNK_IDAT :
            {
                if ( !mbIDAT )      // the gfx is finished, but there may be left a zlibCRC of about 4Bytes
                    ImplReadIDAT();
            }
            break;

            case PNGCHUNK_pHYs :
            {
                if ( !mbIDAT && mnChunkLen == 9 )
                {
                    mnPrefWidth = ImplReadsal_uInt32();
                    mnPrefHeight= ImplReadsal_uInt32();

                    sal_uInt8 nUnitSpecifier = *maDataIter++;

                    if ( nUnitSpecifier == 1 )
                        mbpHYs = sal_True;
                }
            }
            break;
        }
        aIter++;
    }
    if ( mpAcc )
        mpBmp->ReleaseAccess( mpAcc ), mpAcc = NULL;

    if ( mpMaskAcc )
    {
        if ( mpAlphaMask )
            mpAlphaMask->ReleaseAccess( mpMaskAcc );
        else if ( mpMaskBmp )
            mpMaskBmp->ReleaseAccess( mpMaskAcc );

        mpMaskAcc = NULL;
    }

    if ( mbStatus )
    {
        if ( mpAlphaMask )
            aRet = BitmapEx( *mpBmp, *mpAlphaMask );
        else if ( mpMaskBmp )
            aRet = BitmapEx( *mpBmp, *mpMaskBmp );
        else
            aRet = *mpBmp;

        if ( mbpHYs && mnPrefWidth && mnPrefHeight )
        {
            sal_Int32 nPrefSizeX = (sal_Int32)( 100000.0 * ( (double)mnWidth / mnPrefWidth ) );
            sal_Int32 nPrefSizeY = (sal_Int32)( 100000.0 * ( (double)mnHeight / mnPrefHeight ) );

            aRet.SetPrefMapMode( MAP_100TH_MM );
            aRet.SetPrefSize( Size( nPrefSizeX, nPrefSizeY ) );
        }
    }
    if( !mbStatus )
        aRet.Clear();

    return aRet;
}

// ------------------------------------------------------------------------

BOOL PNGReaderImpl::ImplReadHeader()
{
    mnWidth = ImplReadsal_uInt32();
    mnHeight = ImplReadsal_uInt32();

    if ( ( mnWidth == 0 ) || ( mnHeight == 0 ) )
        return FALSE;

    mnBitDepth = *maDataIter++;
    mnColorType = *maDataIter++;

    if ( (mnCompressionType = *maDataIter++) != 0 )
        return FALSE;

    if ( (mnFilterType = *maDataIter++) != 0 )
        return FALSE;

    switch ( mnInterlaceType = *maDataIter++ )  // filter type valid ?
    {
        case 0 :
            mnPass = 7;
            break;
        case 1 :
            mnPass = 0;
            break;
        default:
            return FALSE;
    }
    mnYpos = 0;
    mbPalette = TRUE;
    mbIDAT = mbAlphaChannel = mbTransparent = FALSE;
    mbGrayScale = mbRGBTriple = FALSE;
    mnDepth = mnBitDepth;
    mnScansize = ( ( mnWidth * mnBitDepth ) + 7 ) >> 3;

    // valid color types are 0,2,3,4 & 6
    switch ( mnColorType )
    {
        case 0 :    // each pixel is a grayscale
        {
            switch ( mnBitDepth )
            {
                case 2 :
                    mnDepth = 8;    // we have to expand the bitmap
                case 16 :
                    mnDepth >>= 1;  // we have to reduce the bitmap
                case 1 :
                case 4 :
                case 8 :
                    mbGrayScale = TRUE;
                    break;
                default :
                    return FALSE;
            }
        }
        break;

        case 2 :    // each pixel is an RGB triple
        {
            mbRGBTriple = TRUE;
            mnScansize*=3;
            switch ( mnBitDepth )
            {
                case 16 :           // we have to reduce the bitmap
                case 8 :
                    mnDepth = 24;
                    break;
                default :
                    return FALSE;
            }
        }
        break;

        case 3 :    // each pixel is a palette index
        {
            switch ( mnBitDepth )
            {
                case 2 :
                    mnDepth = 4;    // we have to expand the bitmap
                case 1 :
                case 4 :
                case 8 :
                    mbPalette = FALSE;
                    break;
                default :
                    return FALSE;
            }
        }
        break;

        case 4 :    // each pixel is a grayscale sample followed by an alpha sample
        {
            mnScansize <<= 1;
            mbAlphaChannel = TRUE;
            switch ( mnBitDepth )
            {
                case 16 :
                    mnDepth >>= 1;  // we have to reduce the bitmap
                case 8 :
                    mbGrayScale = TRUE;
                    break;
                default :
                    return FALSE;
            }
        }
        break;

        case 6 :    // each pixel is an RGB triple followed by an alpha sample
        {
            mbRGBTriple = TRUE;
            mnScansize*=4;
            mbAlphaChannel = TRUE;
            switch (mnBitDepth )
            {
                case 16 :           // we have to reduce the bitmap
                case 8 :
                    mnDepth = 24;
                    break;
                default :
                    return FALSE;
            }
        }
        break;

        default :
            return FALSE;
    }
    mnBBP = mnScansize / mnWidth;

    if ( !mnBBP )
        mnBBP = 1;

    mnScansize++;       // each scanline includes one filterbyte
    mpScan = mpInflateInBuf = new BYTE[ mnScansize ];
    mpScanprior = (sal_uInt8*)rtl_allocateZeroMemory( mnScansize );

    if ( mnInterlaceType )
        ImplResizeScanline();

    mpBmp = new Bitmap( Size( mnWidth, mnHeight ), mnDepth);
    mpAcc = mpBmp->AcquireWriteAccess();
    if ( !mpAcc )
        return FALSE;

    if ( mbAlphaChannel )
    {
        mpAlphaMask = new AlphaMask( Size( mnWidth, mnHeight ) );
        mpMaskAcc = mpAlphaMask->AcquireWriteAccess();
        if ( !mpMaskAcc )
            return FALSE;
    }

    if ( mbGrayScale )
        ImplGetGrayPalette( mnBitDepth );

    return TRUE;
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplGetGrayPalette( sal_uInt16 nBitDepth )
{
    sal_uInt32  nAdd, nStart = 0;
    sal_uInt16  nPaletteEntryCount;

    switch ( nBitDepth )
    {
        case 1 :
        {
            nPaletteEntryCount = 2;
            nAdd = 255;
        }
        break;
        case 2 :
        {
            nPaletteEntryCount = 16;
            nAdd = 85;
        }
        break;
        case 4 :
        {
            nPaletteEntryCount = 16;
            nAdd = 17;
        }
        break;

        default:
        case 8 :
        {
            nPaletteEntryCount = 256;
            nAdd = 1;
        }
        break;
    }
    mpAcc->SetPaletteEntryCount( nPaletteEntryCount );
    for ( sal_uInt32 i = 0; nStart < 256; i++, nStart += nAdd )
        mpAcc->SetPaletteColor( (USHORT)i, BitmapColor( mpColorTable[ nStart ],
            mpColorTable[ nStart ], mpColorTable[ nStart ] ) );
}

// ------------------------------------------------------------------------

BOOL PNGReaderImpl::ImplReadPalette()
{
    sal_uInt32 nCount = mnChunkLen / 3;

    if ( ( ( mnChunkLen % 3 ) == 0 ) && ( ( 0 < nCount ) && ( nCount <= 256 ) ) && mpAcc )
    {
        BYTE    nRed, nGreen, nBlue;

        mbPalette = TRUE;
        mpAcc->SetPaletteEntryCount( (USHORT) nCount );

        for ( USHORT i = 0; i < nCount; i++ )
        {
            nRed =   mpColorTable[ *maDataIter++ ];
            nGreen = mpColorTable[ *maDataIter++ ];
            nBlue =  mpColorTable[ *maDataIter++ ];
            mpAcc->SetPaletteColor( i, Color( nRed, nGreen, nBlue ) );
        }
    }
    else
        mbStatus = FALSE;

    return mbStatus;
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplReadTransparent()
{
    if ( mpTransTab == NULL )
    {
        switch ( mnColorType )
        {
            case 0 :
            {
                if ( mnChunkLen == 2 )
                {
                    mpTransTab = new sal_uInt8[ 256 ];
                    rtl_fillMemory( mpTransTab, 256, 0xff );
                    // color type 0 and 4 is always greyscale,
                    // so the return value can be used as index
                    sal_uInt8 nIndex = ImplScaleColor();
                    mpTransTab[ nIndex ] = 0;

                    mpAlphaMask = new AlphaMask( Size( mnWidth, mnHeight ) );
                    mpMaskAcc = mpAlphaMask->AcquireWriteAccess();
                    if ( mpMaskAcc )
                        mbTransparent = sal_True;
                }
            }
            break;

            case 2 :
            {
                if ( mnChunkLen == 6 )
                {
                    mnTransRed = ImplScaleColor();
                    mnTransGreen = ImplScaleColor();
                    mnTransBlue = ImplScaleColor();

                    mpMaskBmp = new Bitmap( Size( mnWidth, mnHeight ), 1 );
                    mpMaskAcc = mpMaskBmp->AcquireWriteAccess();

                    if ( mpMaskAcc )
                    {
                        mbTransparent = mpMaskAcc != NULL;

                        const Color aWhite( COL_WHITE );
                        cTransIndex1 = (BYTE)mpMaskAcc->GetBestPaletteIndex( aWhite );
                        cNonTransIndex1 = cTransIndex1 ? 0 : 1;
                    }
                }
            }
            break;

            case 3 :
            {
                if ( mnChunkLen <= 256 )
                {
                    mpTransTab = new BYTE [ 256 ];
                    rtl_fillMemory( mpTransTab, 256, 0xff );
                    rtl_copyMemory( mpTransTab, &(*maDataIter), mnChunkLen );
                    maDataIter += mnChunkLen;

                    mpAlphaMask = new AlphaMask( Size( mnWidth, mnHeight ) );
                    mpMaskAcc = mpAlphaMask->AcquireWriteAccess();
                    if ( mpMaskAcc )
                        mbTransparent = sal_True;
                }
            }
            break;
        }
    }
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplGetGamma()
{
    sal_uInt32  nGammaValue = ImplReadsal_uInt32();
    double      fGamma = ( ( VIEWING_GAMMA / DISPLAY_GAMMA ) * ( (double)nGammaValue / 100000 ) );
    double      fInvGamma = ( fGamma <= 0.0 || fGamma > 10.0 ) ? 1.0 : ( 1.0 / fGamma );

    if ( fInvGamma != 1.0 )
    {
        mbGamma = TRUE;

        if ( mpColorTable == mpDefaultColorTable )
            mpColorTable = new sal_uInt8[ 256 ];

        for ( sal_Int32 i = 0; i < 256; i++ )
            mpColorTable[ i ] = (sal_uInt8)(pow((double)i/255.0, fInvGamma) * 255.0 + 0.5);

        if ( mbGrayScale )
            ImplGetGrayPalette( mnDepth );
    }
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplGetBackground()
{
    Point       aPoint;
    Rectangle   aRectangle( aPoint, Size( mnWidth, mnHeight ) );

    switch ( mnColorType )
    {
        case 3 :
        {
            if ( mnChunkLen == 1 )
            {
                UINT16 nCol = *maDataIter++;
                if ( nCol < mpAcc->GetPaletteEntryCount() )
                {
                    mpAcc->SetFillColor( (const Color&)mpAcc->GetPaletteColor( (BYTE)nCol ) );
                    mpAcc->FillRect( aRectangle );
                    break;
                }
            }
        }
        break;

        case 0 :
        case 4 :
        {
            if ( mnChunkLen == 2 )
            {
                // the color type 0 and 4 is always greyscale,
                // so the return value can be used as index
                sal_uInt8 nIndex = ImplScaleColor();
                mpAcc->SetFillColor( (const Color&)mpAcc->GetPaletteColor( nIndex ) );
                mpAcc->FillRect( aRectangle );
            }
        }
        break;

        case 2 :
        case 6 :
        {
            if ( mnChunkLen == 6 )
            {
                sal_uInt8 nRed = ImplScaleColor();
                sal_uInt8 nGreen = ImplScaleColor();
                sal_uInt8 nBlue = ImplScaleColor();
                mpAcc->SetFillColor( Color( nRed, nGreen, nBlue ) );
                mpAcc->FillRect( aRectangle );
            }
        }
        break;
    }
}

// ------------------------------------------------------------------------

// for color type 0 and 4 (greyscale) the return value is always index to the color
//                2 and 6 (RGB)       the return value is always the 8 bit color component
sal_uInt8 PNGReaderImpl::ImplScaleColor()
{
    sal_uInt32 nMask = ( ( 1 << mnBitDepth ) - 1 );
    sal_uInt16 nCol = ( *maDataIter++ << 8 );

    nCol += *maDataIter++ & (sal_uInt16)nMask;

    if ( mnBitDepth > 8 )   // convert 16bit graphics to 8
        nCol >>= 8;

    return (sal_uInt8) nCol;
}

// ------------------------------------------------------------------------
// ImplReadIDAT reads as much bitmap data as possible

void PNGReaderImpl::ImplReadIDAT()
{
    sal_Int32 nToRead, nRead;
    if ( mnChunkLen )       // Chunk empty ?
    {
        if ( mbzCodecInUse == FALSE )
        {
            mbzCodecInUse = TRUE;
            mpZCodec->BeginCompression( ZCODEC_PNG_DEFAULT );
        }
        mpZCodec->SetBreak( mnChunkLen );
        SvMemoryStream aIStrm( &(*maDataIter), mnChunkLen, STREAM_READ );

        while ( ( mpZCodec->GetBreak() ) )
        {
            // get bytes needed to fill the current scanline
            nToRead = mnScansize - ( mpScan - mpInflateInBuf );

            if ( ( nRead = mpZCodec->ReadAsynchron( aIStrm, mpScan, nToRead ) ) < 0 )
            {
                mbStatus = FALSE;
                break;
            }
            if ( nRead < nToRead )
            {
                mpScan += nRead;            // ZStream is Broken
                break;
            }
            else
            {
                mpScan = mpInflateInBuf;    // this scanline is Finished

                if ( mnInterlaceType == 0   )
                {
                    ImplGetFilter ( 0, 1 );
                    mnYpos++;
                }
                else
                {
                    // interlace mode
                    switch ( mnPass )
                    {
                        case 1 :                                                                            // pass 1
                        {
                            ImplGetFilter ( 0, 8 );
                            mnYpos += 8;
                        }
                        break;

                        case 2 :                                                                            // pass 2
                        {
                            ImplGetFilter ( 4, 8 );
                            mnYpos += 8;
                        }
                        break;

                        case 3 :                                                                            // pass 3
                        {
                            if ( mnYpos >= 4 )
                            {
                                ImplGetFilter ( 0, 4 );
                            }
                            mnYpos += 8;
                        }
                        break;

                        case 4 :                                                                            // pass 4
                        {
                            ImplGetFilter ( 2, 4 );
                            mnYpos += 4;
                        }
                        break;

                        case 5 :                                                                            // pass 5
                        {
                            if ( mnYpos >= 2 )
                            {
                                ImplGetFilter ( 0, 2 );
                            }
                            mnYpos += 4;
                        }
                        break;

                        case 6 :                                                                            // pass 6
                        {
                                ImplGetFilter ( 1, 2 );
                                mnYpos += 2;
                        }
                        break;

                        case 7 :                                                                            // pass 7
                        {
                            if ( mnYpos >= 1 )
                            {
                                ImplGetFilter ( 0, 1 );
                            }
                            mnYpos += 2;
                        }
                        break;
                    }
                }
            }

            if ( mnYpos >= mnHeight )
            {
                if ( mnPass == 7 )
                    break;
                else
                {
                    ImplResizeScanline();
                }
            }
        }
    }

    if ( ( mnPass >= 7 ) && ( mnYpos >= mnHeight ) )
    {
        mpZCodec->EndCompression();
        mbzCodecInUse = FALSE;
        mbIDAT = TRUE;
    }
}

// ---------------------------------------------------------------------------------------------------

void PNGReaderImpl::ImplResizeScanline( void )
{
    long nScansize;

    while ( mnPass < 7 )
    {
        sal_uInt32 nX = 0;

        mnYpos = 0;
        mnPass++;

        switch ( mnPass )
        {
            case 1 :
                nScansize = ( mnWidth + 7 ) >> 3;
            break;

            case 2 :
                nX += 4;
                nScansize = ( mnWidth + 3 ) >> 3;
            break;

            case 3 :
                mnYpos += 4;
                nScansize = ( mnWidth + 3 ) >> 2;
            break;

            case 4 :
                nX += 2;
                nScansize = ( mnWidth + 1) >> 2;
            break;

            case 5 :
                mnYpos += 2;
                nScansize = ( mnWidth + 1 ) >> 1;
            break;

            case 6 :
                nX++;
                nScansize = mnWidth >> 1;
            break;

            case 7 :
                mnYpos++;
                nScansize = mnWidth;
            break;

            default:
                nScansize = 0;
            break;
        }

        if ( ( mnYpos >= mnHeight ) || ( nX >= mnWidth ) )  // is pass to be skipped ?
            continue;

        // in Interlace mode the size of scanline is not constant
        // so first we calculate the number of entrys
        mnScansize = nScansize * mnBitDepth;

        if ( mbRGBTriple )
            mnScansize *= 3;

        // convert bitsize to byte
        mnScansize = ( mnScansize + 7 ) >> 3;

        if ( mbAlphaChannel )
            mnScansize += ( ( nScansize * mnBitDepth ) >> 3 );

        rtl_zeroMemory( mpScanprior, ++mnScansize );
        break;
    }
}

// ---------------------------------------------------------------------------------------------------
// ImplGetFilter writes the complete Scanline (nY) - in interlace mode the parameter nXStart and nXAdd
// appends to the currently used pass
// the complete size of scanline will be returned - in interlace mode zero is possible!

void PNGReaderImpl::ImplGetFilter ( sal_uInt32 nXStart, sal_uInt32 nXAdd )
{
    BYTE*       pTmp = mpInflateInBuf;
    BYTE*       p1;
    BYTE*       p2;
    BYTE*       p3;
    BYTE*       p4;
    BYTE        nFilterType;
    BYTE        nCol = 0;
    sal_uInt32  nXIndex, nX, nY = mnYpos, n1, n2, na, nb, nc;
    sal_Int32   np, npa, npb, npc;

    sal_uInt32 nBBP = mnBBP;

    if ( nXStart < (sal_uInt32)mnWidth )
    {
        nFilterType = *pTmp++;  // the filter type may change each scanline

        switch ( nFilterType )
        {
            case 1 :
            {
                p1 = pTmp + nBBP;
                p2 = pTmp;

                for (; p1 < pTmp + mnScansize - 1; ++p1)
                    *p1 = sal::static_int_cast<BYTE>(*p1 + ( *p2++ ));
            }
            break;

            case 2 :
            {
                p1 = pTmp;
                p2 = mpScanprior+1;

                for (; p1 < pTmp + mnScansize - 1; ++p1)
                    *p1 = sal::static_int_cast<BYTE>(*p1 + ( *p2++ ));
            }
            break;

            case 3 :
            {
                p1 = pTmp;
                p2 = mpScanprior + 1;
                p3 = pTmp-nBBP;

                for (; p1 < pTmp + mnScansize - 1; ++p1)
                {
                    n1 = (BYTE)( *p2++ );
                    n2 = ( p3 >= pTmp ) ? (BYTE)*p3 : 0;
                    p3++;
                    *p1 = sal::static_int_cast<BYTE>(*p1 + (BYTE)( ( n1 + n2 ) >> 1 ));
                }
            }
            break;

            case 4 :
            {
                p1 = pTmp;
                p2 = p1 - nBBP;
                p3 = mpScanprior + 1;
                p4 = p3 - nBBP;

                for (; p1 < pTmp + mnScansize - 1; ++p1)
                {
                    nb = *p3++;
                    if ( p2 >= pTmp )
                    {
                        na = *p2;
                        nc = *p4;
                    }
                    else
                        na = nc = 0;

                    np = na + nb;
                    np -= nc;
                    npa = np - na;
                    npb = np - nb;
                    npc = np - nc;

                    if ( npa < 0 )
                        npa =-npa;
                    if ( npb < 0 )
                        npb =-npb;
                    if ( npc < 0 )
                        npc =-npc;

                    if ( ( npa <= npb ) && ( npa <= npc ) )
                        *p1 = sal::static_int_cast<BYTE>(*p1 + (BYTE)na);
                    else if ( npb <= npc )
                        *p1 = sal::static_int_cast<BYTE>(*p1 + (BYTE)nb);
                    else
                        *p1 = sal::static_int_cast<BYTE>(*p1 + (BYTE)nc);

                    p2++;
                    p4++;
                }
            }
            break;
            // undefined FilterType -> we will let them pass and display the data unfiltered
        }
        if ( mpAcc->HasPalette() )  // alphachannel is not allowed by pictures including palette entries
        {
            switch ( mpAcc->GetBitCount() )
            {
                case 1 :
                {
                    if ( mbTransparent )
                    {
                        for ( nX = nXStart, nXIndex = 0; nX < (sal_uInt32)mnWidth; nX += nXAdd, nXIndex++ )
                        {
                            sal_uInt32 nShift = ( nXIndex & 7 ) ^ 7;

                            if ( nShift == 0 )
                                nCol = ( *pTmp++ ) & 1;
                            else
                                nCol = sal::static_int_cast<BYTE>(( *pTmp >> nShift ) & 1);

                            ImplSetAlphaPixel( nY, nX, nCol, mpTransTab[ nCol ] );
                        }
                    }
                    else
                    {
                        for ( nX = nXStart, nXIndex = 0; nX < (sal_uInt32)mnWidth; nX += nXAdd, nXIndex++ )
                        {
                            sal_uInt32 nShift = ( nXIndex & 7 ) ^ 7;

                            if ( nShift == 0 )
                                ImplSetPixel( nY, nX, ( *pTmp++ & 1 ) );
                            else
                                ImplSetPixel( nY, nX, sal::static_int_cast<BYTE>(( *pTmp >> nShift ) & 1) );
                        }
                    }
                }
                break;

                case 4 :
                {
                    if ( mbTransparent )
                    {
                        if ( mnBitDepth == 4 )  // maybe the source is a two bit graphic
                        {
                            for ( nX = nXStart, nXIndex = 0; nX < mnWidth; nX += nXAdd, nXIndex++ )
                            {
                                if( nXIndex & 1 )
                                {
                                    ImplSetAlphaPixel( nY, nX, *pTmp & 0x0f, mpTransTab[ *pTmp & 0x0f ] );
                                    pTmp++;
                                }
                                else
                                {
                                    ImplSetAlphaPixel( nY, nX, ( *pTmp >> 4 ) & 0x0f, mpTransTab[ *pTmp >> 4 ] );
                                }
                            }
                        }
                        else
                        {
                            for ( nX = nXStart, nXIndex = 0; nX < mnWidth; nX += nXAdd, nXIndex++ )
                            {
                                switch( nXIndex & 3 )
                                {
                                    case 0 :
                                        nCol = *pTmp >> 6;
                                    break;

                                    case 1 :
                                        nCol = ( *pTmp >> 4 ) & 0x03 ;
                                    break;

                                    case 2 :
                                        nCol = ( *pTmp >> 2 ) & 0x03;
                                    break;

                                    case 3 :
                                        nCol = ( *pTmp++ ) & 0x03;
                                    break;
                                }

                                ImplSetAlphaPixel( nY, nX, nCol, mpTransTab[ nCol ] );
                            }
                        }
                    }
                    else
                    {
                        if ( mnBitDepth == 4 )  // maybe the source is a two bit graphic
                        {
                            for ( nX = nXStart, nXIndex = 0; nX < mnWidth; nX += nXAdd, nXIndex++ )
                            {
                                if( nXIndex & 1 )
                                    ImplSetPixel( nY, nX, *pTmp++ & 0x0f );
                                else
                                    ImplSetPixel( nY, nX, ( *pTmp >> 4 ) & 0x0f );
                            }
                        }
                        else
                        {
                            for ( nX = nXStart, nXIndex = 0; nX < mnWidth; nX += nXAdd, nXIndex++ )
                            {
                                switch( nXIndex & 3 )
                                {
                                    case 0 :
                                        ImplSetPixel( nY, nX, *pTmp >> 6 );
                                    break;

                                    case 1 :
                                        ImplSetPixel( nY, nX, ( *pTmp >> 4 ) & 0x03 );
                                    break;

                                    case 2 :
                                        ImplSetPixel( nY, nX, ( *pTmp >> 2 ) & 0x03 );
                                    break;

                                    case 3 :
                                        ImplSetPixel( nY, nX, *pTmp++ & 0x03 );
                                    break;
                                }
                            }
                        }
                    }
                }
                break;

                case 8 :
                {
                    if ( mbAlphaChannel )
                    {
                        if ( mnBitDepth == 8 )  // maybe the source is a 16 bit grayscale
                        {
                            for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 2 )
                                ImplSetAlphaPixel( nY, nX, pTmp[ 0 ], pTmp[ 1 ] );
                        }
                        else
                        {
                            for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 4 )
                                ImplSetAlphaPixel( nY, nX, pTmp[ 0 ], pTmp[ 2 ] );
                        }
                    }
                    else
                    {
                        if ( mbTransparent )
                        {
                            if ( mnBitDepth == 8 )  // maybe the source is a 16 bit grayscale
                            {
                                for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp++ )
                                    ImplSetAlphaPixel( nY, nX, *pTmp, mpTransTab[ *pTmp ] );
                            }
                            else
                            {
                                for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 2 )
                                    ImplSetAlphaPixel( nY, nX, *pTmp, mpTransTab[ *pTmp ] );
                            }
                        }
                        else
                        {
                            if ( mnBitDepth == 8 )  // maybe the source is a 16 bit grayscale
                            {
                                if ( mnPass == 7 )  // mnPass == 7 -> no interlace or whole scanline is available
                                    mpAcc->CopyScanline( nY, pTmp, BMP_FORMAT_8BIT_PAL, mnScansize -1 );
                                else
                                {
                                    for ( nX = nXStart; nX < mnWidth; nX += nXAdd )
                                        ImplSetPixel( nY, nX, *pTmp++ );
                                }
                            }
                            else
                            {
                                for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 2 )
                                    ImplSetPixel( nY, nX, *pTmp );
                            }
                        }
                    }
                }
                break;

                default :
                    mbStatus = FALSE;
                break;
            }
        }
        else
        {
            if ( mbAlphaChannel || mbTransparent )
            {
                if ( mbAlphaChannel )
                {
                    if ( mnBitDepth == 8 )  // maybe the source is a 16 bit each sample
                    {
                        if ( mpColorTable != mpDefaultColorTable )
                        {
                            for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 4 )
                                ImplSetAlphaPixel( nY, nX, BitmapColor( mpColorTable[ pTmp[ 0 ] ],
                                                                            mpColorTable[ pTmp[ 1 ] ],
                                                                                mpColorTable[ pTmp[ 2 ] ] ), pTmp[ 3 ] );
                        }
                        else
                        {
//                          if ( mnPass == 7 )  // mnPass == 7 -> no interlace or whole scanline is available
//                              mpAcc->CopyScanline( nY, pTmp, BMP_FORMAT_32BIT_TC_RGBA, mnScansize -1 );
//                          else
                            {
                                for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 4 )
                                    ImplSetAlphaPixel( nY, nX, BitmapColor( pTmp[ 0 ], pTmp[ 1 ], pTmp[ 2 ] ), pTmp[ 3 ] );
                            }
                        }
                    }
                    else
                    {
                        for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 8 )
                            ImplSetAlphaPixel( nY, nX, BitmapColor( mpColorTable[ pTmp[ 0 ] ],
                                                                        mpColorTable[ pTmp[ 2 ] ],
                                                                            mpColorTable[ pTmp[ 4 ] ] ), pTmp[6] );
                    }
                }
                else // Transparency chunk
                {
                    sal_Bool    bTransparent;
                    sal_uInt8   nRed, nGreen, nBlue;

                    if ( mnBitDepth == 8 )  // maybe the source is a 16 bit each sample
                    {
                        for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 3 )
                        {
                            nRed = pTmp[ 0 ];
                            nGreen = pTmp[ 1 ];
                            nBlue = pTmp[ 2 ];
                            bTransparent = ( ( nRed == mnTransRed )
                                            && ( nGreen == mnTransGreen )
                                                && ( nBlue == mnTransBlue ) );

                            ImplSetPixel( nY, nX, BitmapColor( mpColorTable[ nRed ],
                                                                mpColorTable[ nGreen ],
                                                                    mpColorTable[ nBlue ] ), bTransparent );
                        }
                    }
                    else
                    {
                        for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 6 )
                        {
                            nRed = pTmp[ 0 ];
                            nGreen = pTmp[ 2 ];
                            nBlue = pTmp[ 4 ];
                            bTransparent = ( ( nRed == mnTransRed )
                                            && ( nGreen == mnTransGreen )
                                                && ( nBlue == mnTransBlue ) );

                            ImplSetPixel( nY, nX, BitmapColor( mpColorTable[ nRed ],
                                                                mpColorTable[ nGreen ],
                                                                    mpColorTable[ nBlue ] ), bTransparent );
                        }
                    }
                }
            }
            else
            {
                if ( mnBitDepth == 8 )  // maybe the source is a 16 bit each sample
                {
                    if ( mpColorTable != mpDefaultColorTable )
                    {
                        for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 3 )
                            ImplSetPixel( nY, nX, BitmapColor( mpColorTable[ pTmp[ 0 ] ],
                                                                mpColorTable[ pTmp[ 1 ] ],
                                                                    mpColorTable[ pTmp[ 2 ] ] ) );
                    }
                    else
                    {
                        if ( mnPass == 7 )  // mnPass == 7 -> no interlace or whole scanline is available
                            mpAcc->CopyScanline( nY, pTmp, BMP_FORMAT_24BIT_TC_RGB, mnScansize -1 );
                        else
                        {
                            for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 3 )
                                ImplSetPixel( nY, nX, BitmapColor( pTmp[ 0 ], pTmp[ 1 ], pTmp[ 2 ] ) );
                        }

                    }
                }
                else
                {
                    for ( nX = nXStart; nX < mnWidth; nX += nXAdd, pTmp += 6 )
                        ImplSetPixel( nY, nX, BitmapColor( mpColorTable[ pTmp[ 0 ] ],
                                                            mpColorTable[ pTmp[ 2 ] ],
                                                                mpColorTable[ pTmp[ 4 ] ] ) );
                }
            }
        }
    }

    rtl_copyMemory( mpScanprior, mpInflateInBuf, mnScansize );
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplSetPixel( sal_uInt32 nY, sal_uInt32 nX, BYTE nPalIndex )
{
    if ( mnPass == 7 )
        mpAcc->SetPixel( nY, nX, (BYTE)nPalIndex );
    else
    {
        sal_uInt32 nTX;

        for ( int nBHeight = 0; nBHeight < aBlockHeight[ mnPass ]; nBHeight++ )
        {
            nTX = nX;

            for ( int nBWidth = 0; nBWidth < aBlockWidth[ mnPass ]; nBWidth++ )
            {
                mpAcc->SetPixel( nY, nTX, (BYTE)nPalIndex );
                if ( ++nTX == mnWidth )
                    break;
            }
            if ( ++nY == mnHeight )
                break;
        }
    }
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplSetPixel( sal_uInt32 nY, sal_uInt32 nX, const BitmapColor& rBitmapColor )
{
    if ( mnPass == 7 )
        mpAcc->SetPixel( nY, nX, rBitmapColor );
    else
    {
        sal_uInt32 nTX;

        for ( int nBHeight = 0; nBHeight < aBlockHeight[ mnPass ]; nBHeight++ )
        {
            nTX = nX;

            for ( int nBWidth = 0; nBWidth < aBlockWidth[ mnPass ]; nBWidth++ )
            {
                mpAcc->SetPixel( nY, nTX, rBitmapColor );
                if ( ++nTX == mnWidth )
                    break;
            }
            if ( ++nY == mnHeight )
                break;
        }
    }
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplSetPixel( sal_uInt32 nY, sal_uInt32 nX, const BitmapColor& rBitmapColor, BOOL bTrans )
{
    if ( mnPass == 7 )
    {
        mpAcc->SetPixel( nY, nX, rBitmapColor );

        if ( bTrans )
            mpMaskAcc->SetPixel( nY, nX, cTransIndex1 );
        else
            mpMaskAcc->SetPixel( nY, nX, cNonTransIndex1 );
    }
    else
    {
        sal_uInt32 nTX;

        for ( int nBHeight = 0; nBHeight < aBlockHeight[ mnPass ]; nBHeight++ )
        {
            nTX = nX;

            for ( int nBWidth = 0; nBWidth < aBlockWidth[ mnPass ]; nBWidth++ )
            {
                mpAcc->SetPixel( nY, nTX, rBitmapColor );

                if ( bTrans )
                    mpMaskAcc->SetPixel( nY, nTX, cTransIndex1 );
                else
                    mpMaskAcc->SetPixel( nY, nTX, cNonTransIndex1 );

                if ( ++nTX == mnWidth )
                    break;
            }

            if ( ++nY == mnHeight )
                break;
        }
    }
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplSetAlphaPixel( sal_uInt32 nY, sal_uInt32 nX, BYTE nPalIndex, BYTE nAlpha )
{
    if ( mnPass == 7 )
    {
        mpAcc->SetPixel( nY, nX, (BYTE)nPalIndex );
        mpMaskAcc->SetPixel( nY, nX, 255 - nAlpha );
    }
    else
    {
        sal_uInt32 nTX;

        for ( int nBHeight = 0; nBHeight < aBlockHeight[ mnPass ]; nBHeight++ )
        {
            nTX = nX;

            for ( int nBWidth = 0; nBWidth < aBlockWidth[ mnPass ]; nBWidth++ )
            {
                mpAcc->SetPixel( nY, nTX, (BYTE)nPalIndex );
                mpMaskAcc->SetPixel( nY, nX, 255 - nAlpha );
                if ( ++nTX == mnWidth )
                    break;
            }
            if ( ++nY == mnHeight )
                break;
        }
    }
}

// ------------------------------------------------------------------------

void PNGReaderImpl::ImplSetAlphaPixel( sal_uInt32 nY, sal_uInt32 nX, const BitmapColor& rBitmapColor, BYTE nAlpha )
{
    if ( mnPass == 7 )
    {
        mpAcc->SetPixel( nY, nX, rBitmapColor );
        mpMaskAcc->SetPixel( nY, nX, 255 - nAlpha );
    }
    else
    {
        sal_uInt32 nTX;

        for ( int nBHeight = 0; nBHeight < aBlockHeight[ mnPass ]; nBHeight++ )
        {
            nTX = nX;

            for ( int nBWidth = 0; nBWidth < aBlockWidth[ mnPass ]; nBWidth++ )
            {
                mpAcc->SetPixel( nY, nTX, rBitmapColor );
                mpMaskAcc->SetPixel( nY, nTX, 255 - nAlpha );
                if ( ++nTX == mnWidth )
                    break;
            }

            if ( ++nY == mnHeight )
                break;
        }
    }
}

// ------------------------------------------------------------------------

sal_uInt32 PNGReaderImpl::ImplReadsal_uInt32()
{
    sal_uInt32 nRet;
    nRet = *maDataIter++;
    nRet <<= 8;
    nRet |= *maDataIter++;
    nRet <<= 8;
    nRet |= *maDataIter++;
    nRet <<= 8;
    nRet |= *maDataIter++;
    return nRet;
}

// ------------------------------------------------------------------------

// -------------
// - PNGReader -
// -------------

PNGReader::PNGReader( SvStream& rIStm ) :
    mpImpl( new ::vcl::PNGReaderImpl( rIStm ) )
{
}

// ------------------------------------------------------------------------

PNGReader::~PNGReader()
{
    delete mpImpl;
}

// ------------------------------------------------------------------------

BitmapEx PNGReader::Read()
{
    return mpImpl->Read();
}

// ------------------------------------------------------------------------

const std::vector< vcl::PNGReader::ChunkData >& PNGReader::GetChunks() const
{
    return mpImpl->maChunkSeq;
}

} // namespace vcl
